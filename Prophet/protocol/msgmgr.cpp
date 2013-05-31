#include "stdafx.h"
#include "msgmgr.h"
#include "engine.h"
#include "message.h"
#include "gui/mainframe.h"


MessageManager::MessageManager( Protocol *protocol )
    : m_protocol(protocol), m_format(this), m_tracer(protocol)
{
    m_message = NULL;
    m_breakOnMsgBegin   = false;
    m_breakOnMsgEnd     = false;
    m_autoShowMemory    = true;
    m_tracing           = false;
}

MessageManager::~MessageManager()
{
    SAFE_DELETE(m_message);
}

void MessageManager::Initialize()
{
    //m_taint = m_protocol->GetEngine()->GetTaintEngine();
    m_format.Initialize();
}

void MessageManager::OnMessageBegin( MessageBeginEvent &event )
{
    if (event.MessageLen >= Taint::GetWidth()) {
        LxError("Prophet: message length(%d) >= taint width(%d)!\n", event.MessageLen,
            Taint::GetWidth());
    }
    //m_taint->TaintMemoryRanged(event.MessageAddr, event.MessageLen, false);
    m_message = new Message(event.MessageAddr, event.MessageLen, event.MessageData);
    m_format.OnMessageBegin(event);

    m_tracer.Begin();
    m_tracing = true;

    if (m_breakOnMsgBegin) {
        char buf[256];
        sprintf(buf, "message begins: addr=%08x, len=%08x", event.MessageAddr, 
            event.MessageLen);
        m_protocol->GetEngine()->BreakOnNextInst(buf);

        if (m_autoShowMemory) {
            m_protocol->GetEngine()->GetGUI()->ShowInMemory(event.MessageAddr);
        }
    }
}

void MessageManager::OnMessageEnd( MessageEndEvent &event )
{
    m_format.OnMessageEnd(event);

    m_protocol->AddMessage(m_message);
    //m_protocol->GetEngine()->GetGUI()->ShowMessage(m_message);
    m_message = NULL;

    int nTraces = m_tracer.Count();
    m_tracer.End();
    m_tracing = false;
    LxInfo("Finished %d run-traces\n", nTraces);

    if (m_breakOnMsgEnd) {
        char buf[256];
        sprintf(buf, "message ends: addr=%08x, len=%08x", 
            event.MessageAddr, event.MessageLen);
        m_protocol->GetEngine()->BreakOnNextInst(buf);
    }
}

void MessageManager::Serialize( Json::Value &root ) const 
{
    root["break_on_message_begin"]  = m_breakOnMsgBegin;
    root["break_on_message_end"]    = m_breakOnMsgEnd;
    root["auto_show_memory"]        = m_autoShowMemory;

    Json::Value formatsyn;
    m_format.Serialize(formatsyn);
    root["format_synthesizer"] = formatsyn;

    Json::Value runtrace;
    m_tracer.Serialize(runtrace);
    root["run_trace"] = runtrace;
}

void MessageManager::Deserialize( Json::Value &root )
{
    m_breakOnMsgBegin   = root.get("break_on_message_begin", m_breakOnMsgBegin).asBool();
    m_breakOnMsgEnd     = root.get("break_on_message_end", m_breakOnMsgEnd).asBool();
    m_autoShowMemory    = root.get("auto_show_memory", m_autoShowMemory).asBool();

    Json::Value formatsyn = root["format_synthesizer"];
    if (!formatsyn.isNull())
        m_format.Deserialize(formatsyn);

    Json::Value runtrace = root["run_trace"];
    if (!runtrace.isNull())
        m_tracer.Deserialize(runtrace);
}

void MessageManager::SubmitLengthField( int first, int last, int target )
{
    Assert(first >= 0   && first < m_message->Size());
    Assert(last >= 0    && last < m_message->Size());
    Assert(target == -1 || target >= 0  && target < m_message->Size());
    // m_format.SubmitLengthField(first, last, target);
}

void MessageManager::SubmitToken( byte t, int first, int last )
{
    Assert(first >= 0   && first < m_message->Size());
    Assert(last >= 0    && last < m_message->Size());
    // m_format.SubmitToken(t, first, last);
}

void MessageManager::SubmitFixedLen( int first, int last )
{
    Assert(first >= 0   && first < m_message->Size());
    Assert(last >= 0    && last < m_message->Size());
    // m_format.SubmitFixedLenField(first, last);
}

void MessageManager::OnPostExecute( PostExecuteEvent &event )
{
    if (m_tracing)
        m_tracer.Trace(event.Cpu);
}

