#include "stdafx.h"
#include "message.h"
#include "engine.h"
#include "cryptohelp.h"

#include "analyzers/procscope.h"
#include "analyzers/traceexec.h"
#include "analyzers/callstack.h"
#include "analyzers/msgaccess.h"
#include "analyzers/msgtree.h"
#include "analyzers/procexec.h"
#include "algorithms/alganalyzer.h"
#include "analyzers/tokenize_refiner.h"
#include "analyzers/parallel_detector.h"
#include "analyzers/sanitize_refiner.h"
#include "analyzers/direction_field.h"

const char *FieldFormatName[] = {
    "unknown", "separator", "keyword", "length", "fixed_length", "var_length", NULL
};

Message::Message(const MemRegion &r, cpbyte data )
    : m_region(r), m_parent(NULL), m_accesslog(NULL), m_fieldTree(NULL)
{
    m_id = -1;
    m_traceBegin = m_traceEnd = 0;
    m_data = new byte[m_region.Len];
    memcpy(m_data, data, m_region.Len);
    m_tag = NULL;
    m_clearNode = false;
    ResolveType();
}

Message::Message(const MemRegion &r, cpbyte data, Message *parent, 
                 const MemRegion &pr, AlgTag *tag, bool clearNode )
    : m_region(r), m_parent(parent), m_parentRegion(pr), m_accesslog(NULL), m_fieldTree(NULL)
{
    m_id = -1;
    m_traceBegin = m_traceEnd = 0;
    m_data = new byte[m_region.Len];
    memcpy(m_data, data, m_region.Len);
    m_tag = tag;
    m_clearNode = clearNode;
    ResolveType();
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

bool Message::Analyze( MessageManager *msgmgr, const RunTrace &trace )
{
    std::string name = GetName();
    LxInfo("Analyzing message %s ...\n", name.c_str());
    TraceExec traceExe(trace);
    ProcScope procScope;

    std::string dir = g_engine.GetArchiveDir() + g_engine.GetArchiveFileName() + "\\";
    LxCreateDirectory(dir.c_str());
    //trace.DumpMsg(this, File(dir + "trace_" + GetName() + ".txt", "w"));

    if (m_traceBegin == 0 && m_traceEnd == 0) {
        LxInfo("Dummy message found\n");
        m_fieldTree = new MsgTree(this);
        m_fieldTree->Construct(NULL, StackHashComparator());
    } else {
        // Get Procedures
        traceExe.Add(&procScope);
        traceExe.RunMessage(this);
        procScope.Dump(File(dir + "procs_" + GetName() + ".txt", "w"), g_engine.GetDisassembler());

        CallStack callStack(&procScope);
        m_accesslog = new MessageAccessLog(this);
        m_accesslog->SetCallStack(&callStack);
        traceExe.Add(&callStack, m_accesslog);
        traceExe.RunMessage(this);

        if (m_accesslog->Count() == 0) {
            return false;
        }

        m_accesslog->Dump(File(dir + "message_access_" + GetName() + ".txt", "w"));

        m_fieldTree = new MsgTree(this);
        m_fieldTree->Construct(m_accesslog, StackHashComparator());
        m_fieldTree->UpdateHistory(m_accesslog);

        //SanitizeRefiner().RefineTree(*m_fieldTree);
        TokenizeRefiner(this, m_type, 1).RefineTree(*m_fieldTree);
        ParallelFieldDetector(3).RefineTree(*m_fieldTree);

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

    if (m_parent != NULL) {
        MemRegion parentData;
        if (m_parent->SearchData(m_data, m_region.Len, parentData)) {
            TreeNode *node = m_parent->GetTree()->FindOrCreateNode(parentData);
            if (node) {
                node->AddLink(NodeLink(
                    m_fieldTree->GetRoot(), this, m_tag ? m_tag->AlgName.c_str() : "", "both"
                    ));
                LxInfo("link added %08x-%d\n", parentData.Addr, parentData.Len);
            }
        }
    }
    LxInfo("Message %s analysis complete\n", name.c_str());
    return true;
}

void Message::AnalyzeAll( MessageManager *msgmgr, const RunTrace &trace )
{
    TraceExec traceExe(trace);
    TaintEngine *taint = msgmgr->GetTaint();

    taint->Reset();
    taint->TaintRule_LoadDefault();
    taint->TaintMemRegion(m_region);
    DirectionField df(this, taint);
    traceExe.Add(taint, &df);
    traceExe.RunMessage(this);
    LxInfo("Post-analyzing Message %s complate\n", GetName().c_str());

    for (auto &msg : m_children) {
        msg->AnalyzeAll(msgmgr, trace);
    }
}

void Message::SetTraceRange( int beginIncl, int endIncl )
{
    m_traceBegin = beginIncl;
    m_traceEnd = endIncl;
}

void Message::Insert( Message *msg )
{
    TreeNode *node = m_fieldTree->FindOrCreateNode(msg->GetParentRegion());
    if (node == NULL) {
        LxError("Cannot insert sub-message into message\n");
        return;
    }

    if (node->Length() > msg->Size() && msg->m_clearNode) {
        node->ClearChildren();
        node = m_fieldTree->FindOrCreateNode(msg->GetParentRegion());
    }

    char name[64];
    sprintf(name, "%s_%d", m_name.c_str(), m_children.size());
    msg->m_name = name;
    node->AddSubMessage(msg);
    m_children.push_back(msg);

    if (msg->m_clearNode) {
        node->ClearChildren();
    }
}

void Message::SetID( int id )
{
    m_id = id;
    char buf[64];
    sprintf(buf, "%03d", m_id);
    m_name = buf;
}

void Message::DumpTree( File &f ) const
{
    m_fieldTree->Dump(f);
    for (auto &ch : m_children) {
        ch->DumpTree(f);
    }
}

bool Message::SearchData( cpbyte p, int len, MemRegion &r )
{
    for (int i = 0; i < (int) (m_region.Len - len + 1); i++) {
        if (CompareByteArray(p, m_data + i, len) == 0) {
            r.Addr = m_region.Addr + i;
            r.Len = len;
            return true;
        }
    }
    return false;
}

void Message::ResolveType()
{
    m_type = MESSAGE_ASCII;
    for (uint i = 0; i < m_region.Len; i++) {
        if (m_data[i] > 0x7f) {
            m_type = MESSAGE_BINARY;
            break;
        }
    }
}

std::string Message::GetTypeString() const
{
    switch (m_type) {
    case MESSAGE_ASCII:
        return "Text_ASCII";
    case MESSAGE_BINARY:
        return "Binary";
    default:
        return "Unknown";
    }
}

std::string Message::GetName() const
{
    return GetTypeString() + "_" + m_name;
}

AlgTag::AlgTag( const std::string &name, const std::string &desc, u32 proc )
    : AlgName(name), Description(desc), Proc(proc)
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
    fprintf(f.Ptr(), "-- %s(%s) Proc_%08x --\\l", AlgName.c_str(), Description.c_str(), Proc);
    int count = 1;
    for (auto &p : Params) {
        if (p->Mem.Addr <= 9) {     // 8 gp regs and Eip
            // register
            Assert(p->Mem.Len <= 4);    // 32-bit
            u32 val = *(reinterpret_cast<u32p>(p->Data));
            fprintf(f.Ptr(), "(%d)[%s] %s %08xh\\l", count++,
                InstContext::RegNames[p->Mem.Addr].c_str(), p->Type.c_str(), val);
        } else {  
            // memory
            fprintf(f.Ptr(), "(%d)[%x:%d] %s %s H=%.4f\\l", count++, 
                p->Mem.Addr, p->Mem.Len, p->Type.c_str(), 
                ByteArrayToDotString(p->Data, p->Mem.Len, 16).c_str(), p->Entropy);
        }
    }
    fprintf(f.Ptr(), "\"");
}

void AlgTag::AddParam( const std::string &t, const MemRegion &r, cpbyte d )
{
    Params.push_back(new AlgParam(t, r, d));
}

AlgParam::AlgParam( const std::string &t, const MemRegion &r, cpbyte d )
    : Type(t), Mem(r)
{
    Data = new byte[r.Len];
    memcpy(Data, d, r.Len);
    Entropy = CalculateEntropy(d, r.Len);
}

AlgParam::~AlgParam()
{
    SAFE_DELETE(Data);
}
