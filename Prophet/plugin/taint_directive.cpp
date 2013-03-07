#include "stdafx.h"
#include "plugin.h"
#include "engine.h"
#include "utilities.h"
#include "event.h"

#include "processor.h"
#include "memory.h"

/*
 * This plugin automatically taints data segments that have prefix '.taint',
 * and instructions in code segment which are surrounded by 
 * TAINT_CODE_BEGIN() and TAINT_CODE_END() macros.
 */

static const std::string TaintSegmentPrefix = ".taint";
static const uint       TaintBegin          = 0x14159265;
static const uint       TaintEnd            = 0x65921514;
static const uint       TaintDataSeg        = 0xf1edcacf;

class TaintDirective : public Plugin {
public:
    TaintDirective() : Plugin("TaintDirective", PreExecuteHandler | PostExecuteHandler | ProcessPreRunHandler) {}
    ~TaintDirective() {}

    void    Initialize() override;
    virtual void    OnProcessPreRun(ProcessPreRunEvent &event, bool firstTime) override;
    virtual void    OnPreExecute(PreExecuteEvent &event, bool firstTime) override {
        if (!firstTime) return;
        m_lastEip = event.Cpu->EIP;
    }
    virtual void    OnPostExecute(PostExecuteEvent &event, bool firstTime) override;

    virtual void    Serialize(Json::Value &root) const override;
    virtual void    Deserialize(Json::Value &root) override;

private:
    void    DoTaint(const Processor *cpu);

private:
    bool    m_taintAllBits;
    u32     m_lastEip;
};

void TaintDirective::Initialize()
{
    m_lastEip = 0;
}

void TaintDirective::OnProcessPreRun( ProcessPreRunEvent &event, bool firstTime )
{
    if (!firstTime) return;
    DoTaint(event.Cpu);
}

void TaintDirective::OnPostExecute( PostExecuteEvent &event, bool firstTime )
{
    if (firstTime) return;
    if (event.Inst->Main.Inst.Opcode != 0xeb || 
        event.Inst->Main.Inst.AddrValue - m_lastEip != 0x06)
        return;     // only 'jmp eip+4'
    
    u32 magic = 0;
    event.Cpu->Mem->Read32(event.Cpu->EIP - 4, &magic);
    if (magic != TaintDataSeg) return;

    DoTaint(event.Cpu);
}

void TaintDirective::DoTaint( const Processor *cpu )
{
    std::vector<SectionInfo>    meminfo = cpu->Mem->GetMemoryInfo();
    TaintEngine *taint  = GetEngine()->GetTaintEngine();
    for (auto &sec : meminfo) {
        Section *psec = cpu->Mem->GetSection(sec.base);
        if (!psec->IsAllCommitted()) 
            continue;
        if (psec->GetDesc().Desc.find(TaintSegmentPrefix) != std::string::npos) {
            LxInfo("TaintDirective: Tainting %s segment from %08x to %08x\n",
                psec->GetDesc().Desc.c_str(), psec->Base(), psec->Size());
            taint->TaintMemoryRanged(psec->Base(), psec->Size(), m_taintAllBits);
            continue;
        }

        uint ptr        = 0;
        uint idxBegin   = 0;
        uint idxEnd     = 0;
        pbyte pdata     = cpu->Mem->GetRawData(sec.base);
        while (ptr <= sec.size - 4) {
            u32p p      = reinterpret_cast<u32p>(&pdata[ptr]);
            if (*p == TaintBegin) {
                ptr     += 4;
                idxBegin = sec.base + ptr;
                continue;
            }
            if (*p == TaintEnd && idxBegin != 0) {
                idxEnd  = sec.base + ptr;
                // taint
                LxInfo("TaintDirective: Tainting memory from %08x to %08x\n", idxBegin, idxEnd);
                taint->TaintMemoryRanged(idxBegin, idxEnd - idxBegin, m_taintAllBits);
                idxBegin = idxEnd = 0;
            }
            ptr++;
        }
    }
}

void TaintDirective::Serialize( Json::Value &root ) const 
{
    Plugin::Serialize(root);
    root["taint_all_bits"] = m_taintAllBits;
}

void TaintDirective::Deserialize( Json::Value &root )
{
    Plugin::Deserialize(root);
    m_taintAllBits = root.get("taint_all_bits", m_taintAllBits).asBool();
}

TaintDirective t;