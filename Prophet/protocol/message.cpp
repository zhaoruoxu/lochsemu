#include "stdafx.h"
#include "message.h"
#include "engine.h"

#include "analyzers/procscope.h"
#include "analyzers/traceexec.h"
#include "analyzers/callstack.h"
#include "analyzers/msgaccess.h"
#include "analyzers/msgtree.h"
#include "analyzers/msgformat.h"
#include "analyzers/procexec.h"
#include "algorithms/alganalyzer.h"
#include "analyzers/tokenize_refiner.h"
#include "analyzers/parallel_detector.h"
#include "analyzers/sanitize_refiner.h"

const char *FieldFormatName[] = {
    "unknown", "separator", "keyword", "length", "fixed_length", "var_length", NULL
};

Message::Message(const MemRegion &r, cpbyte data )
    : m_region(r), m_parent(NULL)
{
    m_id = -1;
    m_traceBegin = m_traceEnd = 0;
    m_data = new byte[m_region.Len];
    memcpy(m_data, data, m_region.Len);
    m_name = "t";
    m_tag = NULL;
    m_clearNode = false;
}

Message::Message(const MemRegion &r, cpbyte data, Message *parent, 
                 const MemRegion &pr, AlgTag *tag, bool clearNode )
    : m_region(r), m_parent(parent), m_parentRegion(pr)
{
    m_id = -1;
    m_traceBegin = m_traceEnd = 0;
    m_data = new byte[m_region.Len];
    memcpy(m_data, data, m_region.Len);
    m_tag = tag;
    m_clearNode = clearNode;
}

Message::~Message()
{
    SAFE_DELETE_ARRAY(m_data);
    SAFE_DELETE(m_accesslog);
    SAFE_DELETE(m_fieldTree);
    SAFE_DELETE(m_tag);
    for (auto &submsg : m_children) {
        SAFE_DELETE(submsg);
    }
    m_children.clear();
}

void Message::Analyze( MessageManager *msgmgr, const RunTrace &trace )
{
    TraceExec traceExe(trace);
    ProcScope procScope;

    std::string dir = g_engine.GetArchiveDir() + g_engine.GetArchiveFileName() + "\\";
    LxCreateDirectory(dir.c_str());
    trace.Dump(File(dir + "trace_" + GetName() + ".txt", "w"));

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

    SanitizeRefiner().RefineTree(*m_fieldTree);
    TokenizeRefiner(this).RefineTree(*m_fieldTree);
    ParallelFieldDetector().RefineTree(*m_fieldTree);

    TaintEngine *taint = msgmgr->GetTaint();
    taint->Reset();
    taint->TaintRule_LoadMemory();
    taint->TaintMemRegion(m_region);
    AdvAlgEngine alg(msgmgr, this, 32);
    ProcExec procExe(&callStack, taint);
    procExe.Add(&alg);
    traceExe.Add(taint, &callStack, &procExe);
    traceExe.RunMessage(this);
}

void Message::SetTraceRange( int beginIncl, int endIncl )
{
    m_traceBegin = beginIncl;
    m_traceEnd = endIncl;
}

void Message::Insert( Message *msg )
{
    MessageTreeNode *node = m_fieldTree->FindNode(msg->GetParentRegion());
    if (node == NULL) {
        LxError("Cannot insert sub-message into message\n");
        return;
    }
    char name[64];
    sprintf(name, "%s_%d", m_name.c_str(), m_children.size());
    msg->m_name = name;
    node->SetSubMessage(msg);
    m_children.push_back(msg);

    if (msg->m_clearNode) {
        node->ClearChildren();
    }
}

void Message::SetID( int id )
{
    m_id = id;
    char buf[64];
    sprintf(buf, "T%d", m_id);
    m_name = buf;
}

void Message::DumpTree( File &f ) const
{
    m_fieldTree->Dump(f);
    for (auto &ch : m_children) {
        ch->DumpTree(f);
    }
}

AlgTag::AlgTag( const std::string &name, const std::string &desc )
    : AlgName(name), Description(desc)
{

}

AlgTag::~AlgTag()
{
    for (auto &param : Params) {
        SAFE_DELETE(param);
    }
    Params.clear();
}

void AlgTag::DumpDot( File &f ) const
{
    fprintf(f.Ptr(), "\"");
    fprintf(f.Ptr(), "-- %s(%s) --\\l", AlgName.c_str(), Description.c_str());
    for (auto &p : Params) {
        fprintf(f.Ptr(), "[%x:%d] %s %s\\l", p->Mem.Addr, p->Mem.Len,
            p->Type.c_str(), ByteArrayToDotString(p->Data, p->Mem.Len, 16).c_str());
    }
    fprintf(f.Ptr(), "\"");
}

AlgParam::AlgParam( const std::string &t, const MemRegion &r, cpbyte d )
    : Type(t), Mem(r)
{
    Data = new byte[r.Len];
    memcpy(Data, d, r.Len);
}

AlgParam::~AlgParam()
{
    SAFE_DELETE(Data);
}
