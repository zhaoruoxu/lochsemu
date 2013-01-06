#include "stdafx.h"
#include "engine.h"

#include "gui/mainframe.h"
#include "gui/cpupanel.h"
#include "gui/tracepanel.h"

#include "instruction.h"
#include "processor.h"

AEngine::AEngine() : m_debugger(this), m_tracer(this)
{

}

AEngine::~AEngine()
{

}

void AEngine::Initialize()
{
    m_debugger.Initialize();
    m_tracerEnabled = g_config.GetInt("Tracer", "EnableOnStart", 1) != 0;
    m_currEip       = 0;
    m_totalExecuted = 0;
}

void AEngine::OnPreExecute( Processor *cpu, const Instruction *inst )
{
    m_currEip = cpu->EIP;
    m_disassembler.OnPreExecute(cpu, inst);
    //m_gui->GetCpuPanel()->OnPreExecute(cpu, inst);
    m_debugger.OnPreExecute(cpu, inst);
}

void AEngine::OnPostExecute( Processor *cpu, const Instruction *inst )
{
    //m_debugger.OnPostExecute(cpu, inst);
    ++m_totalExecuted;
    if (m_tracerEnabled) {
        m_tracer.TraceInst(cpu, m_currEip, m_totalExecuted);
    }
}

void AEngine::SetGuiFrame( ArietisFrame *frame )
{
    m_gui = frame;
    m_disassembler.RegisterDataUpdateHandler([this](const Disassembler::InstDisasmMap *insts) {
        this->m_gui->GetCpuPanel()->OnDataUpdate(insts);
    });
    m_gui->GetTracePanel()->SetTracer(&m_tracer);

//     m_disassembler.RegisterInstDisasmHandler([this](const Disassembler::InstVector &insts) {
//         this->m_gui->GetCpuPanel()->OnInstDisasm(insts);
//     });
//     m_disassembler.RegisterPtrChangeHandler([this](u32 addr) {
//         this->m_gui->GetCpuPanel()->OnPtrChange(addr);
//     });
}

void AEngine::OnProcessPreRun( const Process *proc, const Processor *cpu )
{
    m_debugger.OnProcPreRun(proc, cpu);
}

InstContext AEngine::GetCurrentInstContext() const
{
    InstContext context;
    m_debugger.UpdateInstContext(context);
    m_disassembler.UpdateInstContext(context);
    return context;
}

void AEngine::ReportBusy( bool isBusy )
{
    m_gui->ReportBusy(isBusy);
}

const std::string InstContext::FlagNames[] = {
    "OF", "SF", "ZF", "AF", "PF", "CF", "TF", "IF", "DF", "NT", "RF"
};
