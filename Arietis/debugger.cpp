#include "stdafx.h"
#include "debugger.h"
#include "parallel.h"
#include "engine.h"
#include "processor.h"
#include "instruction.h"

#include "gui/mainframe.h"

enum DebuggerState {
    STATE_RUNNING,
    STATE_SINGLESTEP,
    STATE_STEPOVER,
    STATE_STEPOUT,
    STATE_TERMINATED,
};

Debugger::Debugger(ArietisEngine *engine) : m_engine(engine)
{
}

Debugger::~Debugger()
{
}

void Debugger::Initialize()
{

}

void Debugger::OnPreExecute( Processor *cpu, const Instruction *inst )
{
    m_engine->GetGUI()->DebugStepCallback(cpu, inst);
    m_semaphore.Wait();

    //m_engine->Log(wxString::Format("Executing [%08x]  %s", cpu->EIP, inst->Main.CompleteInstr));
}

void Debugger::OnPostExecute( Processor *cpu, const Instruction *inst )
{

}

void Debugger::OnStepInto()
{
    m_semaphore.Post();
    
}
