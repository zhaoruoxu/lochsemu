#include "stdafx.h"
#include "engine.h"

#include "gui/mainframe.h"
#include "gui/cpupanel.h"
#include "gui/tracepanel.h"

#include "instruction.h"
#include "processor.h"
#include "buildver.h"

AEngine::AEngine() : m_debugger(this), m_tracer(this)
{
    m_emulator  = NULL;
}

AEngine::~AEngine()
{
    //Persist();
}

void AEngine::Initialize(Emulator *emu)
{
    Intro();
    m_emulator      = emu;
    m_debugger.Initialize();
    m_taint.Initialize();
    m_tracerEnabled = g_config.GetInt("Tracer", "EnableOnStart", 1) != 0;
    m_currEip       = 0;
    m_totalExecuted = 0;
    m_skipDllEntries    = g_config.GetInt("Engine", "SkipDllEntries", 1) != 0;
    m_mainEntryEntered  = false;
    m_enabled       = true;
}

void AEngine::OnPreExecute( Processor *cpu, const Instruction *inst )
{
    if (!m_enabled) return;
    //m_archive[cpu->EIP] = Json::Value(inst->Main.CompleteInstr);
    m_currEip = cpu->EIP;
    
    m_disassembler.OnPreExecute(cpu, inst);
    if (m_skipDllEntries && !m_mainEntryEntered) return;
    
    //m_gui->GetCpuPanel()->OnPreExecute(cpu, inst);
    m_debugger.OnPreExecute(cpu, inst);

    
}

void AEngine::OnPostExecute( Processor *cpu, const Instruction *inst )
{
    if (!m_enabled) return;
    ++m_totalExecuted;
    if (m_skipDllEntries && !m_mainEntryEntered) return;
    //m_debugger.OnPostExecute(cpu, inst);
    
    if (m_tracerEnabled) {
        m_tracer.TraceInst(cpu, m_currEip, m_totalExecuted);
    }
}

void AEngine::OnProcessPostLoad( const PeLoader *loader )
{
//     std::string dir = LxGetModuleDirectory(g_module);
//     m_archiveFilePath = dir + loader->GetModuleInfo(0)->Name + ".json";
//     if (LxFileExists(m_archiveFilePath.c_str())) {
//         Json::Reader reader;
//         std::ifstream fin(m_archiveFilePath.c_str());
//         if (!reader.parse(fin, m_archive)) {
//             LxFatal("Error parsing archive file %s\n", m_archiveFilePath.c_str());
//         }
//     }
    if (!m_enabled) return;
    m_gui->OnProcessLoaded(m_emulator->Path());
}

// void AEngine::Persist()
// {
//     std::ofstream fout(m_archiveFilePath, std::ios_base::out | std::ios_base::trunc);
//     fout << m_archive;
// }

void AEngine::SetGuiFrame( ArietisFrame *frame )
{
    m_gui = frame;
    m_disassembler.RegisterDataUpdateHandler([this](const InstSection *insts) {
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
    if (!m_enabled) return;
    m_mainEntryEntered = true;
    m_debugger.OnProcPreRun(proc, cpu);

    //Persist();
}

InstContext AEngine::GetCurrentInstContext() const
{
    InstContext context;
    m_debugger.UpdateInstContext(context);
    m_disassembler.UpdateInstContext(context);
    m_taint.UpdateInstContext(context);
    return context;
}

void AEngine::ReportBusy( bool isBusy )
{
    m_gui->ReportBusy(isBusy);
}

void AEngine::Intro() const
{
    LxWarning("********************************************************\n");
    LxWarning("\n");
    LxWarning("        Arietis ver %x build %d\n", ArietisVersion, ARIETIS_BUILD_VERSION);
    LxWarning("\n");
    LxWarning("********************************************************\n");
}

void AEngine::Terminate()
{
    m_enabled = false;
    //m_emulator->Terminate();
    m_debugger.OnTerminate();
}

const std::string InstContext::FlagNames[] = {
    "OF", "SF", "ZF", "AF", "PF", "CF", /* "TF", "IF", "DF", "NT", "RF" */
};

const std::string InstContext::RegNames[] = {
    "Eax", "Ecx", "Edx", "Ebx", "Esp", "Ebp", "Esi", "Edi", "Eip",
};
