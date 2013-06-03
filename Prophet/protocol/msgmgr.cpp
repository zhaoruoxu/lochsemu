#include "stdafx.h"
#include "msgmgr.h"
#include "engine.h"
#include "message.h"
#include "protocol.h"
#include "analyzers/msgtree.h"
#include "taint/taintengine.h"

MessageManager::MessageManager( Protocol *protocol )
    : m_protocol(protocol), m_tracer(protocol)
{
    m_currRootMsg = NULL;
    m_breakOnMsgBegin   = false;
    m_breakOnMsgEnd     = false;
    m_autoShowMemory    = true;
    m_tracing           = false;
    m_currId = 0;
    m_taint = new TaintEngine();
    m_clearSubNodes = true;
}

MessageManager::~MessageManager()
{
    //SAFE_DELETE(m_currRootMsg);
    Assert(m_currRootMsg == NULL);
    //Analyze();
    Assert(m_msgQueue.empty());
    for (auto &msg : m_messages) {
        SAFE_DELETE(msg);
    }
    m_messages.clear();
    SAFE_DELETE(m_taint);
}

void MessageManager::Initialize()
{
    //m_taint = m_protocol->GetEngine()->GetTaintEngine();
    //m_format.Initialize();
}

void MessageManager::OnMessageBegin( MessageBeginEvent &event )
{
    if (event.MessageLen >= Taint::GetWidth()) {
        LxError("Prophet: message length(%d) >= taint width(%d)!\n", event.MessageLen,
            Taint::GetWidth());
    }
    //m_taint->TaintMemoryRanged(event.MessageAddr, event.MessageLen, false);
    m_currRootMsg = new Message(MemRegion(event.MessageAddr, event.MessageLen), event.MessageData);
    //m_format.OnMessageBegin(event);

    m_tracer.Begin();
    m_tracing = true;

//     if (m_breakOnMsgBegin) {
//         char buf[256];
//         sprintf(buf, "message begins: addr=%08x, len=%08x", event.MessageAddr, 
//             event.MessageLen);
//         m_protocol->GetEngine()->BreakOnNextInst(buf);
// 
//         if (m_autoShowMemory) {
//             m_protocol->GetEngine()->GetGUI()->ShowInMemory(event.MessageAddr);
//         }
//     }
}

void MessageManager::OnMessageEnd( MessageEndEvent &event )
{
    //m_format.OnMessageEnd(event);

    int nTraces = m_tracer.Count();
    EnqueueMessage(m_currRootMsg, 0, nTraces-1);
    m_currRootMsg = NULL;

    Analyze();

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
    root["clear_sub_nodes"]         = m_clearSubNodes;

//     Json::Value formatsyn;
//     m_format.Serialize(formatsyn);
//     root["format_synthesizer"] = formatsyn;

    Json::Value runtrace;
    m_tracer.Serialize(runtrace);
    root["run_trace"] = runtrace;
}

void MessageManager::Deserialize( Json::Value &root )
{
    m_breakOnMsgBegin   = root.get("break_on_message_begin", m_breakOnMsgBegin).asBool();
    m_breakOnMsgEnd     = root.get("break_on_message_end", m_breakOnMsgEnd).asBool();
    m_autoShowMemory    = root.get("auto_show_memory", m_autoShowMemory).asBool();
    m_clearSubNodes = root.get("clear_sub_nodes", m_clearSubNodes).asBool();

//     Json::Value formatsyn = root["format_synthesizer"];
//     if (!formatsyn.isNull())
//         m_format.Deserialize(formatsyn);

    Json::Value runtrace = root["run_trace"];
    if (!runtrace.isNull())
        m_tracer.Deserialize(runtrace);
}

void MessageManager::OnPostExecute( PostExecuteEvent &event )
{
    if (m_tracing)
        m_tracer.Trace(event.Cpu);
}

void MessageManager::EnqueueMessage( Message *msg, int beginIncl, int endIncl )
{
    msg->SetTraceRange(beginIncl, endIncl);
    msg->SetID(msg->GetParent() ? msg->GetParent()->GetID() : m_currId++);
    m_msgQueue.push_back(msg);
}

void MessageManager::Analyze()
{
    while (!m_msgQueue.empty()) {
        Message *msg = m_msgQueue.front();
        m_msgQueue.pop_front();

        if (msg->GetParent() != NULL) {
            msg->GetParent()->Insert(msg);
        } else {
            m_messages.push_back(msg);
        }
        
        msg->Analyze(this, m_tracer);
    }

}

void MessageManager::GenerateOutput()
{
    Analyze();

    std::string dir = g_engine.GetArchiveDir() + g_engine.GetArchiveFileName() + "\\";
    LxCreateDirectory(dir.c_str());
    // dump dots
    for (auto &msg : m_messages) {
        msg->DumpTree(File(dir + "tree_" + msg->GetName() + ".txt", "w"));
        std::string dotfile = dir + "tree_" + msg->GetName() + ".dot";
        msg->GetTree()->DumpDot(File(dotfile, "w"), true);
        DotToImage(dotfile);
    }
}

