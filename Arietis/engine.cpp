#include "stdafx.h"
#include "engine.h"

#include "gui/mainframe.h"
#include "gui/cpupanel.h"

#include "instruction.h"

ArietisEngine::ArietisEngine() : m_debugger(this)
{

}

ArietisEngine::~ArietisEngine()
{

}

void ArietisEngine::Initialize()
{
    m_debugger.Initialize();
}

void ArietisEngine::OnPreExecute( Processor *cpu, const Instruction *inst )
{
    m_disassembler.OnPreExecute(cpu, inst);
    m_gui->GetCpuPanel()->OnPreExecute(cpu, inst);
    m_debugger.OnPreExecute(cpu, inst);
}

void ArietisEngine::OnPostExecute( Processor *cpu, const Instruction *inst )
{
    m_debugger.OnPostExecute(cpu, inst);
}

void ArietisEngine::OnStepInto()
{ 
    m_debugger.OnStepInto(); 
}

void ArietisEngine::Log( const wxString &s )
{
    m_gui->DebugLog(s);
}

void ArietisEngine::SetGuiFrame( ArietisFrame *frame )
{
    m_gui = frame;
    m_disassembler.RegisterDataUpdateHandler([this](const Disassembler::InstDisasmMap *insts) {
        this->m_gui->GetCpuPanel()->OnDataUpdate(insts);
    });

//     m_disassembler.RegisterInstDisasmHandler([this](const Disassembler::InstVector &insts) {
//         this->m_gui->GetCpuPanel()->OnInstDisasm(insts);
//     });
//     m_disassembler.RegisterPtrChangeHandler([this](u32 addr) {
//         this->m_gui->GetCpuPanel()->OnPtrChange(addr);
//     });
}
