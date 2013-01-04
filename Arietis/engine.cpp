#include "stdafx.h"
#include "engine.h"

#include "gui/mainframe.h"

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
