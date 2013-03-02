#include "stdafx.h"
#include "plugin.h"
#include "engine.h"

#include "processor.h"
#include "memory.h"

static const uint TaintBegin    = 0x14159265;
static const uint TaintEnd      = 0x65921514;
static const uint TaintGapMax   = 4096;

class TaintDirective : public Plugin {
public:
    TaintDirective() : Plugin("TaintDirective") {}
    ~TaintDirective() {}

    void    Initialize() override;
    virtual void    OnProcessPreRun(const Process *proc, const Processor *cpu) override;

private:
    bool    m_taintAllBits;
};

void TaintDirective::Initialize()
{
    Enable(g_config.GetInt("PluginTaintDirective", "Enabled", 1) != 0);
    m_taintAllBits = g_config.GetInt("PluginTaintDirective", "TaintAllBits", 1) != 0;
}

void TaintDirective::OnProcessPreRun( const Process *proc, const Processor *cpu )
{
    Memory *mem = cpu->Mem;
    std::vector<SectionInfo>    meminfo = mem->GetMemoryInfo();
    TaintEngine *taint  = GetManager()->GetEngine()->GetTaintEngine();
    for (auto &sec : meminfo) {
        Section *psec = mem->GetSection(sec.base);
        if (!psec->IsAllCommitted()) 
            continue;
        uint ptr        = 0;
        uint idxBegin   = 0;
        uint idxEnd     = 0;
        pbyte pdata     = mem->GetRawData(sec.base);
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

static TaintDirective t;