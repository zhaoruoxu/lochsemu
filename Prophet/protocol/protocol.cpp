#include "stdafx.h"
#include "protocol.h"
#include "event.h"
#include "engine.h"
#include "taint/taintengine.h"
#include "message.h"

/* Analyzers */
#include "analyzers/direction_field.h"
#include "analyzers/separator.h"

Protocol::Protocol( ProEngine *engine )
    : m_engine(engine), m_apiprocessor(this), m_msgmanager(this),
    m_tracer(this)
{
    ZeroMemory(m_analyzers, sizeof(m_analyzers));
    ZeroMemory(m_messages, sizeof(m_messages));
    m_totalAnalyzers    = 0;
    m_totalMessages     = 0;
    m_taint     = NULL;
    m_enabled   = true;
    m_state     = Idle;
    m_eipPreExec = 0;
    m_tracing   = false;
}

Protocol::~Protocol()
{
    for (int i = 0; i < m_totalAnalyzers; i++) {
        SAFE_DELETE(m_analyzers[i]);
    }
    for (int i = 0; i < m_totalMessages; i++) {
        SAFE_DELETE(m_messages[i]);
    }
}

void Protocol::Initialize()
{
    m_taint = m_engine->GetTaintEngine();
    m_apiprocessor.Initialize();
    m_msgmanager.Initialize();

    RegisterAnalyzer(new DirectionField(this));
    RegisterAnalyzer(new SeparatorKeyword(this));
    
    for (int i = 0; i < m_totalAnalyzers; i++) {
        m_analyzers[i]->Initialize();
    }
}

void Protocol::Serialize( Json::Value &root ) const 
{
    root["enabled"] = m_enabled;
    
    Json::Value analyzers;
    for (int i = 0; i < m_totalAnalyzers; i++) {
        ProtocolAnalyzer *pa = m_analyzers[i];
        Json::Value paroot;
        pa->Serialize(paroot);
        analyzers[pa->GetName()] = paroot;
    }
    root["analyzers"] = analyzers;

    Json::Value apiProc;
    m_apiprocessor.Serialize(apiProc);
    root["api_processor"] = apiProc;

    Json::Value msgMgr;
    m_msgmanager.Serialize(msgMgr);
    root["message_manager"] = msgMgr;

    Json::Value runtrace;
    m_tracer.Serialize(runtrace);
    root["taint_trace"] = runtrace;
}

void Protocol::Deserialize( Json::Value &root )
{
    m_enabled = root.get("enabled", m_enabled).asBool();

    Json::Value analyzers = root["analyzers"];
    if (!analyzers.isNull()) {
        for (int i = 0; i < m_totalAnalyzers; i++) {
            ProtocolAnalyzer *pa = m_analyzers[i];
            Json::Value paroot = analyzers[pa->GetName()];
            if (paroot.isNull()) continue;
            pa->Deserialize(paroot);
        }
    }

    Json::Value apiProc = root["api_processor"];
    if (!apiProc.isNull()) {
        m_apiprocessor.Deserialize(apiProc);
    }

    Json::Value msgMgr = root["message_manager"];
    if (!msgMgr.isNull()) {
        m_msgmanager.Deserialize(msgMgr);
    }

    Json::Value runtrace = root["run_trace"];
    if (!runtrace.isNull()) {
        m_tracer.Deserialize(runtrace);
    }
}

void Protocol::RegisterAnalyzer( ProtocolAnalyzer *analyzer )
{
    if (m_totalAnalyzers >= MaxAnalyzers) {
        LxFatal("Too many protocol analyzers!\n");
    }
    m_analyzers[m_totalAnalyzers++] = analyzer;
    LxInfo("Prophet protocol analyzer registered: %s\n", analyzer->GetName().c_str());
}

void Protocol::AddMessage( Message *msg )
{
    if (m_totalMessages >= MaxMessages) {
        LxFatal("Too many protocol messages!\n");
    }
    m_messages[m_totalMessages++] = msg;
}

void Protocol::OnPreExecute( PreExecuteEvent &event )
{
    if (!m_enabled) return;
    m_eipPreExec = event.Cpu->EIP;
    
    if (m_state == ProcessingMessage) {
//         for (int i = 0; i < m_totalAnalyzers; i++) {
//             ProtocolAnalyzer *pa = m_analyzers[i];
//             if (pa->IsEnabled() && pa->HasHandlerFlag(PreExecuteHandler))
//                 pa->OnPreExecute(event);
//         }
    }
}

void Protocol::OnPostExecute( PostExecuteEvent &event )
{
    if (!m_enabled) return;

    if (m_tracing) {
        m_tracer.Trace(event.Cpu);
    }

    if (m_state == ProcessingMessage) {
//         for (int i = 0; i < m_totalAnalyzers; i++) {
//             ProtocolAnalyzer *pa = m_analyzers[i];
//             if (pa->IsEnabled() && pa->HasHandlerFlag(PostExecuteHandler))
//                 pa->OnPostExecute(event);
//         }
    }
}

void Protocol::OnMemRead( MemReadEvent &event )
{
    if (!m_enabled) return;

    if (m_state == ProcessingMessage) {
//         for (int i = 0; i < m_totalAnalyzers; i++) {
//             ProtocolAnalyzer *pa = m_analyzers[i];
//             if (pa->IsEnabled() && pa->HasHandlerFlag(MemReadHandler))
//                 pa->OnMemRead(event);
//         }
    }
}

void Protocol::OnMemWrite( MemWriteEvent &event )
{
    if (!m_enabled) return;

    if (m_state == ProcessingMessage) {
//         for (int i = 0; i < m_totalAnalyzers; i++) {
//             ProtocolAnalyzer *pa = m_analyzers[i];
//             if (pa->IsEnabled() && pa->HasHandlerFlag(MemWriteHandler))
//                 pa->OnMemWrite(event);
//         }
    }
}

void Protocol::OnProcessPreRun( ProcessPreRunEvent &event )
{
    if (!m_enabled) return;

    if (m_state == ProcessingMessage) {
//         for (int i = 0; i < m_totalAnalyzers; i++) {
//             ProtocolAnalyzer *pa = m_analyzers[i];
//             if (pa->IsEnabled() && pa->HasHandlerFlag(ProcessPreRunHandler))
//                 pa->OnProcessPreRun(event);
//         }
    }
}

void Protocol::OnProcessPostRun( ProcessPostRunEvent &event )
{
    if (!m_enabled) return;

    if (m_state == ProcessingMessage) {
        OnMessageEnd(MessageEndEvent(this));

//         for (int i = 0; i < m_totalAnalyzers; i++) {
//             ProtocolAnalyzer *pa = m_analyzers[i];
//             if (pa->IsEnabled() && pa->HasHandlerFlag(ProcessPostRunHandler))
//                 pa->OnProcessPostRun(event);
//         }
    }
}

void Protocol::OnProcessPreLoad( ProcessPreLoadEvent &event )
{
    if (!m_enabled) return;

    if (m_state == ProcessingMessage) {
//         for (int i = 0; i < m_totalAnalyzers; i++) {
//             ProtocolAnalyzer *pa = m_analyzers[i];
//             if (pa->IsEnabled() && pa->HasHandlerFlag(ProcessPreLoadHandler))
//                 pa->OnProcessPreLoad(event);
//         }
    }
}

void Protocol::OnProcessPostLoad( ProcessPostLoadEvent &event )
{
    if (!m_enabled) return;

    if (m_state == ProcessingMessage) {
//         for (int i = 0; i < m_totalAnalyzers; i++) {
//             ProtocolAnalyzer *pa = m_analyzers[i];
//             if (pa->IsEnabled() && pa->HasHandlerFlag(ProcessPostLoadHandler))
//                 pa->OnProcessPostLoad(event);
//         }
    }
}

void Protocol::OnWinapiPreCall( WinapiPreCallEvent &event )
{
    if (!m_enabled) return;

    m_apiprocessor.OnWinapiPreCall(event);

    if (m_state == ProcessingMessage) {
//         for (int i = 0; i < m_totalAnalyzers; i++) {
//             ProtocolAnalyzer *pa = m_analyzers[i];
//             if (pa->IsEnabled() && pa->HasHandlerFlag(WinapiPreCallHandler))
//                 pa->OnWinapiPreCall(event);
//         }
    }
}

void Protocol::OnWinapiPostCall( WinapiPostCallEvent &event )
{
    if (!m_enabled) return;

    m_apiprocessor.OnWinapiPostCall(event);

    if (m_state == ProcessingMessage) {
//         for (int i = 0; i < m_totalAnalyzers; i++) {
//             ProtocolAnalyzer *pa = m_analyzers[i];
//             if (pa->IsEnabled() && pa->HasHandlerFlag(WinapiPostCallHandler))
//                 pa->OnWinapiPostCall(event);
//         }
    }
}

void Protocol::OnMessageBegin( MessageBeginEvent &event )
{
    if (m_state == ProcessingMessage) {
        LxWarning("Already processing message, ending current message\n");
        MessageEndEvent e(this);
        OnMessageEnd(e);
    }

    if (event.MessageLen <= 0) {
        LxWarning("Message begins with length %d, ignored\n", event.MessageLen);
        return;
    }

    m_state = ProcessingMessage;
    m_engine->GetDisassembler()->GetInst(m_eipPreExec)->Desc = "Message begin";
    // m_taint->Enable(true);
    
    // m_msgmanager.OnMessageBegin(event);

//     for (int i = 0; i < m_totalAnalyzers; i++) {
//         ProtocolAnalyzer *pa = m_analyzers[i];
//         if (pa->IsEnabled() && pa->HasHandlerFlag(MessageBeginHandler))
//             pa->OnMessageBegin(event);
//     }

    BeginTrace();
}

void Protocol::OnMessageEnd( MessageEndEvent &event )
{
//     if (event.MessageLen <= 0) {
//         LxWarning("Message ends with length %d, ignored\n", event.MessageLen);
//         return;
//     }

    if (m_state == Idle) return;

    m_engine->GetDisassembler()->GetInst(m_eipPreExec)->Desc = "Message end";

    TSnapshot s(*m_taint);
    ExecuteTraces();

    int nTraces;
    EndTrace(&nTraces);
    LxInfo("Ending trace, count = %d\n", nTraces);

//     for (int i = 0; i < m_totalAnalyzers; i++) {
//         ProtocolAnalyzer *pa = m_analyzers[i];
//         if (pa->IsEnabled() && pa->HasHandlerFlag(MessageEndHandler))
//             pa->OnMessageEnd(event);
//     }

    //m_formatsyn.OnMessageEnd(event);
    //m_msgmanager.OnMessageEnd(event);

    //m_taint->Enable(false);
    m_state = Idle;
}

void Protocol::UpdateTContext( const Processor *cpu, TContext *ctx ) const
{
    m_engine->GetDisassembler()->UpdateTContext(ctx, cpu->GetPrevEip());
    m_engine->GetDebugger()->UpdateTContext(cpu, ctx);
}

void Protocol::BeginTrace()
{
    m_tracer.Begin();
    m_tracing = true;
}

void Protocol::EndTrace(int *nCount)
{
    if (nCount) *nCount = m_tracer.Count();
    m_tracing = false;
    m_tracer.End();
}

void Protocol::ExecuteTraces()
{
    const int nTraces = m_tracer.Count();
    for (int i = 0; i < nTraces; i++) {
        ExecuteTraceEvent e(this, m_tracer.Get(i));
        m_taint->OnExecuteTrace(e);
    }
}
