#include "stdafx.h"
#include "debugger.h"
#include "parallel.h"
#include "engine.h"
#include "processor.h"
#include "instruction.h"

#include "gui/mainframe.h"

Debugger::Debugger(ArietisEngine *engine) : m_engine(engine)
{
}

Debugger::~Debugger()
{
}

void Debugger::Initialize()
{
    if (g_config.GetInt("Debugger", "BreakOnEntry", 1) != 0) {
        m_state     = STATE_SINGLESTEP;
    } else {
        m_state     = STATE_RUNNING;
    }
    m_stepOverEip   = 0;
    m_currInst      = NULL;
    m_currProcessor = NULL;
}

void Debugger::OnPreExecute( Processor *cpu, const Instruction *inst )
{
    m_currProcessor = cpu;
    m_currInst      = inst;
    CheckBreakpoints(cpu, inst);

    switch (m_state) {
    case STATE_SINGLESTEP:
        {
            DoSingleStep(cpu, inst);
        } break;
    case STATE_RUNNING:
        {
            //m_engine->GetGUI()->DebugStepCallback(cpu, inst);
        } break;
    case STATE_STEPOUT:
        {

        } break;
    case STATE_STEPOVER:
        {
            if (cpu->EIP == m_stepOverEip) {
                m_state = STATE_SINGLESTEP;
                m_stepOverEip = 0;
                DoSingleStep(cpu, inst);
            }
        } break;
    case STATE_TERMINATED:
        {
            exit(0);
        } break;
    default:
        LxFatal("invalid Arietis.Debugger state\n");
    }



    //m_engine->Log(wxString::Format("Executing [%08x]  %s", cpu->EIP, inst->Main.CompleteInstr));
}

void Debugger::OnPostExecute( Processor *cpu, const Instruction *inst )
{

}

void Debugger::OnStepInto()
{
    Assert(m_state == STATE_SINGLESTEP);
    m_semaphore.Post();
}

void Debugger::OnStepOver()
{
    Assert(m_state == STATE_SINGLESTEP);
    if (NULL == m_currProcessor || NULL == m_currInst) return;

    if (Instruction::IsCall(m_currInst)) {
        m_state = STATE_STEPOVER;
        m_stepOverEip = m_currProcessor->EIP + m_currInst->Length;
    }

    m_semaphore.Post();
}

void Debugger::OnRun()
{
    m_state = STATE_RUNNING;
    m_semaphore.Post();
}

void Debugger::OnStepOut()
{
    m_engine->GetGUI()->DebugLog("StepOut not implemented");
}

void Debugger::OnToggleBreakpoint()
{

}

void Debugger::CheckBreakpoints( const Processor *cpu, const Instruction *inst )
{

}

void Debugger::DoSingleStep( const Processor *cpu, const Instruction *inst )
{
    m_engine->GetGUI()->DebugStepCallback(cpu, inst);
    m_semaphore.Wait();
}

void Debugger::OnProcPreRun( const Process *proc, const Processor *cpu )
{
    if (g_config.GetInt("Debugger", "BreakOnMainModuleEntry", 1) != 0) {
        m_state = STATE_SINGLESTEP;
        m_engine->GetGUI()->DebugLog("Main module entry reached");
    }
}
