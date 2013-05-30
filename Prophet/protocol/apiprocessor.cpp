#include "stdafx.h"
#include "apiprocessor.h"
#include "protocol.h"
#include "engine.h"

#include "processor.h"
#include "winapi.h"

ApiProcessor::ApiProcessor( Protocol *protocol )
    : m_protocol(protocol)
{
    ZeroMemory(m_handlerPreCall,            sizeof(m_handlerPreCall));
    ZeroMemory(m_handlerPostCall,           sizeof(m_handlerPostCall));
    ZeroMemory(m_isHandlerEnabledPreCall,   sizeof(m_isHandlerEnabledPreCall));
    ZeroMemory(m_isHandlerEnabledPostCall,  sizeof(m_isHandlerEnabledPostCall));
    m_enabled = true;
    //m_taint = NULL;
}

ApiProcessor::~ApiProcessor()
{

}

void ApiProcessor::Initialize()
{
    //m_taint = m_protocol->GetEngine()->GetTaintEngine();
    InitializeDefaultHandlers();
}

void ApiProcessor::OnWinapiPreCall( WinapiPreCallEvent &event )
{
    if (!m_enabled) return;

    int index = event.ApiIndex;
    if (m_isHandlerEnabledPreCall[index]) {
        Assert(m_handlerPreCall[index] != NULL);
        (this->*(m_handlerPreCall[index]))(event.Cpu);
    }
}

void ApiProcessor::OnWinapiPostCall( WinapiPostCallEvent &event )
{
    if (!m_enabled) return;

    int index = event.ApiIndex;
    if (m_isHandlerEnabledPostCall[index]) {
        Assert(m_handlerPostCall[index] != NULL);
        (this->*(m_handlerPostCall[index]))(event.Cpu);
    }
}

void ApiProcessor::Serialize( Json::Value &root ) const 
{
    root["enabled"] = m_enabled;

    Json::Value preCallApis, postCallApis;
    for (int i = 0; i < ApiCountMax; i++) {
        if (m_handlerPreCall[i] != NULL) {
            Json::Value api;
            SerializeApi(api, i, true);
            preCallApis.append(api);
        }
        if (m_handlerPostCall[i] != NULL) {
            Json::Value api;
            SerializeApi(api, i, false);
            postCallApis.append(api);
        }
    }
    root["pre_call_apis"]   = preCallApis;
    root["post_call_apis"]  = postCallApis;
}

void ApiProcessor::Deserialize( Json::Value &root )
{
    m_enabled = root.get("enabled", m_enabled).asBool();

    Json::Value preCallApis = root["pre_call_apis"];
    if (!preCallApis.isNull()) {
        for (uint i = 0; i < preCallApis.size(); i++) {
            DeserializeApi(preCallApis[i], true);
        }
    }
    Json::Value postCallApis = root["post_call_apis"];
    if (!postCallApis.isNull()) {
        for (uint i = 0; i < postCallApis.size(); i++) {
            DeserializeApi(postCallApis[i], false);
        }
    }
}

void ApiProcessor::SerializeApi( Json::Value &root, int index, bool isPreCallApi ) const
{
    //root["index"]   = index;
    root["dll"]     = LxGetWinAPIModuleName(index);
    root["name"]    = LxGetWinAPIName(index);
    root["enabled"] = isPreCallApi ? m_isHandlerEnabledPreCall[index] 
        : m_isHandlerEnabledPostCall[index];
}

void ApiProcessor::DeserializeApi( Json::Value &root, bool isPreCallApi )
{
    const char *dllName = root["dll"].asCString();
    const char *funcName = root["name"].asCString();

    uint index = QueryWinAPIIndexByName(dllName, funcName);

    if (index == 0) {
        LxError("ApiProcessor: WinApi not found %s.%s\n", dllName, funcName);
        return;
    }

    bool enabled = root["enabled"].asBool();
    if (isPreCallApi)
        m_isHandlerEnabledPreCall[index]    = enabled;
    else
        m_isHandlerEnabledPostCall[index]   = enabled;
}

void ApiProcessor::InitializeDefaultHandlers()
{
    RegisterHandlerPreCall("kernel32.dll", "WriteFile", &ApiProcessor::Handler_WriteFile, true);
    RegisterHandlerPreCall("ws2_32.dll", "send", &ApiProcessor::Handler_send, true);

    RegisterHandlerPostCall("kernel32.dll", "ReadFile", &ApiProcessor::Handler_ReadFile, true);
    RegisterHandlerPostCall("ws2_32.dll", "recv", &ApiProcessor::Handler_recv, true);
}

void ApiProcessor::RegisterHandlerPreCall( const char *dllName, const char *apiName, WinapiHandler h, bool enabled )
{
    uint index = QueryWinAPIIndexByName(dllName, apiName);
    if (index == 0) {
        LxFatal("Undefined Windows API: %s:%s\n", dllName, apiName);
    }
    if (index > ApiCountMax) {
        LxFatal("Too many Windows APIs\n");
    }
    m_handlerPreCall[index]             = h;
    m_isHandlerEnabledPreCall[index]    = enabled;
    LxInfo("Prophet: WinApi handler registered: %s:%s, %s\n", dllName, apiName,
        enabled ? "enabled" : "disabled");
}

void ApiProcessor::RegisterHandlerPostCall( const char *dllName, const char *apiName, WinapiHandler h, bool enabled )
{
    uint index = QueryWinAPIIndexByName(dllName, apiName);
    if (index == 0) {
        LxFatal("Undefined Windows API: %s:%s\n", dllName, apiName);
    }
    if (index > ApiCountMax) {
        LxFatal("Too many Windows APIs\n");
    }
    m_handlerPostCall[index]            = h;
    m_isHandlerEnabledPostCall[index]   = enabled;
    LxInfo("Prophet: WinApi handler registered: %s:%s, %s\n", dllName, apiName,
        enabled ? "enabled" : "disabled");
}

#ifdef PARAM
#undef PARAM
#define PARAM(n, type)  (type) cpu->GetStackParam32(n)
#endif

#ifdef PARAM_PTR
#undef PARAM_PTR
#define PARAM_PTR(n, type)  (type) cpu->GetStackParamPtr32(n)
#endif

#ifdef RET_VALUE
#undef RET_VALUE
#define RET_VALUE(type)     (type) cpu->EAX
#endif

void ApiProcessor::OnPreExecute( PreExecuteEvent &event )
{
    if (Instruction::IsRet(event.Inst)) {
        if (m_accumulated) {
            LxWarning("Beginning accumulated message, this may contain thread error\n");
            int len = m_accAddrPrev - m_accAddrStart + 1;
            MessageBeginEvent e(this, event.Cpu->IntID, len, m_accAddrStart, 
                event.Cpu->Mem->GetRawData(m_accAddrStart), MR_recv);
            m_protocol->OnMessageBegin(e);
            m_accumulated = false;
            m_accAddrStart = m_accAddrPrev = 0;
        }
    }
}

void ApiProcessor::Handler_recv(const Processor *cpu)
{
    int len         = RET_VALUE(int);
    u32 bufferAddr  = PARAM(1, u32);

    if (len == 1) {
        if (m_accumulated) {
            if (bufferAddr == m_accAddrPrev+1) {
                m_accAddrPrev++;
            } else {
                LxError("Cannot concatenate to accumulated message\n");
            }
        } else {
            m_accumulated = true;
            m_accAddrStart = m_accAddrPrev = bufferAddr;
        }
    } else {
        MessageBeginEvent e(this, cpu->IntID, len, bufferAddr, cpu->Mem->GetRawData(bufferAddr), MR_recv);
        m_protocol->OnMessageBegin(e);
    }
}

void ApiProcessor::Handler_send(const Processor *cpu)
{
    int len         = PARAM(2, int);
    u32 addr        = PARAM(1, u32);
    MessageEndEvent e(this, len, addr, cpu->Mem->GetRawData(addr), MR_send);
    m_protocol->OnMessageEnd(e);
}

void ApiProcessor::Handler_ReadFile(const Processor *cpu)
{
//     u32 bufferAddr  = PARAM(1, u32);        // begin address of buffer
//     u32 lenRead     = *(PARAM_PTR(3, u32p));// length of data read
//     MessageBeginEvent e(this, lenRead, bufferAddr, cpu->Mem->GetRawData(bufferAddr));
//     m_protocol->OnMessageBegin(e);
}

void ApiProcessor::Handler_WriteFile(const Processor *cpu)
{
//     u32 bufferAddr  = PARAM(1, u32);
//     int len         = PARAM(2, int);
//     MessageEndEvent e(this, len, bufferAddr, cpu->Mem->GetRawData(bufferAddr));
//     m_protocol->OnMessageEnd(e);
}


