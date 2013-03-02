#include "stdafx.h"
#include "plugin.h"
#include "engine.h"

#include "processor.h"
#include "memory.h"

static const std::string TaintSegment   = ".taint";

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
    std::vector<SectionInfo>    meminfo = cpu->Mem->GetMemoryInfo();
    TaintEngine *taint  = GetManager()->GetEngine()->GetTaintEngine();
    for (auto &sec : meminfo) {
        Section *psec = cpu->Mem->GetSection(sec.base);
        if (!psec->IsAllCommitted()) 
            continue;
        if (psec->GetDesc().Desc.find(TaintSegment) != std::string::npos) {
            LxInfo("TaintDirective: Tainting %s segment from %08x to %08x\n",
                psec->GetDesc().Desc.c_str(), psec->Base(), psec->Size());
            taint->TaintMemoryRanged(psec->Base(), psec->Size(), m_taintAllBits);
            continue;
        }
    }
}

static TaintDirective t;