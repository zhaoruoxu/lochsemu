#include "stdafx.h"
#include "engine.h"
#include "message.h"

#include "analyzers/procscope.h"
#include "analyzers/traceexec.h"
#include "analyzers/callstack.h"
#include "analyzers/msgaccess.h"
#include "analyzers/msgtree.h"
#include "analyzers/msgformat.h"
#include "analyzers/procexec.h"
#include "algorithms/alganalyzer.h"

const char *FieldFormatName[] = {
    "unknown", "separator", "keyword", "length", "fixed_length", "var_length", NULL
};

Message::Message(u32 addr, int len, cpbyte data, Message *parent )
    : m_region(addr, len), m_parent(parent)
{
    m_id = -1;
    m_traceBegin = m_traceEnd = 0;
    m_data = new MessageByte[len];
    for (int i = 0; i < Size(); i++)
        m_data[i].Data = data[i];
}

Message::~Message()
{
    SAFE_DELETE_ARRAY(m_data);
    SAFE_DELETE(m_accesslog);
    SAFE_DELETE(m_fieldTree);
}

// std::string Message::ToString() const
// {
//     char buf[4096];
//     int i = 0;
//     for (; i < Size(); i++)
//         buf[i] = m_data[i].Data;
//     buf[i] = '\0';
//     return std::string(buf);
// }

void Message::Analyze( MessageManager *msgmgr, const RunTrace &trace )
{
    TraceExec traceExe(trace);
    ProcScope procScope;

    // Get Procedures
    traceExe.Add(&procScope);
    traceExe.RunMessage(this);

    CallStack callStack(&procScope);
    m_accesslog = new MessageAccessLog(this);
    m_accesslog->SetCallStack(&callStack);
    traceExe.Add(&callStack, m_accesslog);
    traceExe.RunMessage(this);

    m_fieldTree = new MessageTree(this);
    m_fieldTree->Construct(m_accesslog, StackHashComparator());
    m_fieldTree->UpdateHistory(m_accesslog);

    TokenizeRefiner(this).RefineTree(*m_fieldTree);
    ParallelFieldDetector().RefineTree(*m_fieldTree);

    std::string dir = g_engine.GetArchiveDir();

    //m_fieldTree.Dump(File(dir + "field_tree.txt", "w"));
    std::string dotfile = dir + "field_tree.dot";
    m_fieldTree->DumpDot(File(dotfile, "w"));
    DotToImage(dotfile);

    AdvAlgEngine alg(msgmgr, this, 32);
    ProcExec procExe(&callStack, NULL);
    procExe.Add(&alg);
    traceExe.Add(&callStack, &procExe);
    traceExe.RunMessage(this);
}

void Message::SetTraceRange( int beginIncl, int endIncl )
{
    m_traceBegin = beginIncl;
    m_traceEnd = endIncl;
}
