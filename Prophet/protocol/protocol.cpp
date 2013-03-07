#include "stdafx.h"
#include "protocol.h"
#include "event.h"

Protocol::Protocol( ProEngine *engine )
    : m_engine(engine), m_controller(this)
{
    m_enabled   = true;
    m_state     = BetweenSession;
}

Protocol::~Protocol()
{

}

void Protocol::Initialize()
{
    m_controller.Initialize();
    ReorderAnalyzers();

    for (int i = 0; i < m_totalAnalyzers; i++) {
        m_analyzers[i]->Initialize();
    }
}

// void Protocol::OnWinapiPostCall( WinapiPostCallEvent &event )
// {
//     m_pretaint.OnWinapiPostCall(event);
// }

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
}

void Protocol::Deserialize( Json::Value &root )
{
    m_enabled = root.get("enabled", m_enabled).asBool();
    Json::Value analyzers = root["analyzers"];
    if (analyzers.isNull()) return;

    for (int i = 0; i < m_totalAnalyzers; i++) {
        ProtocolAnalyzer *pa = m_analyzers[i];
        Json::Value paroot = analyzers[pa->GetName()];
        if (paroot.isNull()) continue;
        pa->Deserialize(paroot);
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

void Protocol::OnPreExecute( PreExecuteEvent &event )
{
    if (!m_enabled) return;
    
    if (m_state == InSession) {
        for (int i = 0; i < m_totalAnalyzers; i++) {
            ProtocolAnalyzer *pa = m_analyzers[i];
            if (pa->IsEnabled() && pa->HasHandlerFlag(PreExecuteHandler))
                pa->OnPreExecute(event);
        }
    }
}

void Protocol::OnPostExecute( PostExecuteEvent &event )
{
    if (!m_enabled) return;

    if (m_state == InSession) {
        for (int i = 0; i < m_totalAnalyzers; i++) {
            ProtocolAnalyzer *pa = m_analyzers[i];
            if (pa->IsEnabled() && pa->HasHandlerFlag(PostExecuteHandler))
                pa->OnPostExecute(event);
        }
    }
}

void Protocol::OnMemRead( MemReadEvent &event )
{
    if (!m_enabled) return;

    if (m_state == InSession) {
        for (int i = 0; i < m_totalAnalyzers; i++) {
            ProtocolAnalyzer *pa = m_analyzers[i];
            if (pa->IsEnabled() && pa->HasHandlerFlag(MemReadHandler))
                pa->OnMemRead(event);
        }
    }
}

void Protocol::OnMemWrite( MemWriteEvent &event )
{
    if (!m_enabled) return;

    if (m_state == InSession) {
        for (int i = 0; i < m_totalAnalyzers; i++) {
            ProtocolAnalyzer *pa = m_analyzers[i];
            if (pa->IsEnabled() && pa->HasHandlerFlag(MemWriteHandler))
                pa->OnMemWrite(event);
        }
    }
}

void Protocol::OnProcessPreRun( ProcessPreRunEvent &event )
{
    if (!m_enabled) return;

    if (m_state == InSession) {
        for (int i = 0; i < m_totalAnalyzers; i++) {
            ProtocolAnalyzer *pa = m_analyzers[i];
            if (pa->IsEnabled() && pa->HasHandlerFlag(ProcessPreRunHandler))
                pa->OnProcessPreRun(event);
        }
    }
}

void Protocol::OnProcessPostRun( ProcessPostRunEvent &event )
{
    if (!m_enabled) return;

    if (m_state == InSession) {
        for (int i = 0; i < m_totalAnalyzers; i++) {
            ProtocolAnalyzer *pa = m_analyzers[i];
            if (pa->IsEnabled() && pa->HasHandlerFlag(ProcessPostRunHandler))
                pa->OnProcessPostRun(event);
        }
    }
}

void Protocol::OnProcessPreLoad( ProcessPreLoadEvent &event )
{
    if (!m_enabled) return;

    if (m_state == InSession) {
        for (int i = 0; i < m_totalAnalyzers; i++) {
            ProtocolAnalyzer *pa = m_analyzers[i];
            if (pa->IsEnabled() && pa->HasHandlerFlag(ProcessPreLoadHandler))
                pa->OnProcessPreLoad(event);
        }
    }
}

void Protocol::OnProcessPostLoad( ProcessPostLoadEvent &event )
{
    if (!m_enabled) return;

    if (m_state == InSession) {
        for (int i = 0; i < m_totalAnalyzers; i++) {
            ProtocolAnalyzer *pa = m_analyzers[i];
            if (pa->IsEnabled() && pa->HasHandlerFlag(ProcessPostLoadHandler))
                pa->OnProcessPostLoad(event);
        }
    }
}

void Protocol::OnWinapiPreCall( WinapiPreCallEvent &event )
{
    if (!m_enabled) return;

    if (m_state == InSession) {
        for (int i = 0; i < m_totalAnalyzers; i++) {
            ProtocolAnalyzer *pa = m_analyzers[i];
            if (pa->IsEnabled() && pa->HasHandlerFlag(WinapiPreCallHandler))
                pa->OnWinapiPreCall(event);
        }
    }
}

void Protocol::OnWinapiPostCall( WinapiPostCallEvent &event )
{
    if (!m_enabled) return;

    if (m_state == InSession) {
        for (int i = 0; i < m_totalAnalyzers; i++) {
            ProtocolAnalyzer *pa = m_analyzers[i];
            if (pa->IsEnabled() && pa->HasHandlerFlag(WinapiPostCallHandler))
                pa->OnWinapiPostCall(event);
        }
    }
}

void Protocol::ReorderAnalyzers()
{
    std::sort(m_analyzers, m_analyzers + m_totalAnalyzers, ProtocolAnalyzer::Compare);

    LxInfo("Prophet protocol analyzers order:\n");
    for (int i = 0; i < m_totalAnalyzers; i++) {
        LxInfo("%8d: %s\n", m_analyzers[i]->GetExecOrder(), m_analyzers[i]->GetName().c_str());
    }
    LxInfo("\n");
}
