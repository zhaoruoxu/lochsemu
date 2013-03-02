#include "stdafx.h"
#include "engine.h"
#include "utilities.h"

#include "gui/mainframe.h"
#include "gui/cpupanel.h"
#include "gui/tracepanel.h"

#include "instruction.h"
#include "processor.h"
#include "buildver.h"

AEngine::AEngine() 
    : m_debugger(this), m_tracer(this), m_taint(this), m_plugins(this)
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
    m_plugins.Initialize();
    m_tracer.Enable(g_config.GetInt("Tracer", "Enabled", 1) != 0);
    //m_skipDllEntries    = g_config.GetInt("Engine", "SkipDllEntries", 1) != 0;
    m_mainEntryEntered  = false;
    m_enabled           = true;
    m_isArchiveLoaded   = false;

    CreateArchiveDirectory();
}

// void AEngine::SaveConfig()
// {
//     g_config.SetInt("Tracer", "Enabled", m_tracer.IsEnabled());
//     g_config.SetInt("Engine", "SkipDllEntries", m_skipDllEntries);
// }

void AEngine::OnPreExecute( Processor *cpu, const Instruction *inst )
{
    if (!m_enabled) return;
    m_tracer.OnPreExecute(cpu);
    m_disassembler.OnPreExecute(cpu, inst);
    m_plugins.OnPreExecute(cpu, inst);

    if (m_archive.SkipDllEntries && !m_mainEntryEntered) return;
    m_debugger.OnPreExecute(cpu, inst);
}

void AEngine::OnPostExecute( Processor *cpu, const Instruction *inst )
{
    if (!m_enabled) return;
    if (m_archive.SkipDllEntries && !m_mainEntryEntered) return;

    m_taint.OnPostExecute(cpu, inst);
    m_tracer.OnPostExecute(cpu, inst);
    m_plugins.OnPostExecute(cpu, inst);
}

void AEngine::OnMemRead( const Processor *cpu, u32 addr, u32 nbytes, cpbyte data )
{
    if (!m_enabled) return;
    m_plugins.OnMemRead(cpu, addr, nbytes, data);
}

void AEngine::OnMemWrite( const Processor *cpu, u32 addr, u32 nbytes, cpbyte data )
{
    if (!m_enabled) return;
    m_plugins.OnMemWrite(cpu, addr, nbytes, data);
}

void AEngine::OnProcessPreRun( const Process *proc, const Processor *cpu )
{
    if (!m_enabled) return;
    m_mainEntryEntered = true;
    m_debugger.OnProcPreRun(proc, cpu);
    m_plugins.OnProcessPreRun(proc, cpu);
}

void AEngine::OnProcessPostRun( const Process *proc )
{
    if (!m_enabled) return;
    m_plugins.OnProcessPostRun(proc);
}

void AEngine::OnProcessPreLoad( const PeLoader *loader )
{
    if (!m_enabled) return;
    m_plugins.OnProcessPreLoad(loader);
}

void AEngine::OnProcessPostLoad( const PeLoader *loader )
{
    if (!m_enabled) return;
    m_debugger.OnProcessPostLoad(loader);
    m_gui->OnProcessLoaded(m_emulator->Path());
    
    LoadArchive(loader->GetModuleInfo(0)->Name);
}

void AEngine::OnWinapiPreCall( Processor *cpu, uint apiIndex )
{
    if (!m_enabled) return;
    m_plugins.OnWinapiPreCall(cpu, apiIndex);
}

void AEngine::OnWinapiPostCall( Processor *cpu, uint apiIndex )
{
    if (!m_enabled) return;
    m_plugins.OnWinapiPostCall(cpu, apiIndex);
}

void AEngine::SetGuiFrame( ArietisFrame *frame )
{
    m_gui = frame;
    m_disassembler.RegisterDataUpdateHandler([this](const InstSection *insts, const Processor *cpu) {
        this->m_gui->GetCpuPanel()->OnDataUpdate(insts, cpu);
    });
    m_gui->GetTracePanel()->SetTracer(&m_tracer);
}



void AEngine::GetInstContext(InstContext *ctx) const
{
    m_debugger.UpdateInstContext(ctx);
    m_disassembler.UpdateInstContext(ctx, ctx->Eip);
    m_taint.UpdateInstContext(ctx);
}

void AEngine::GetTraceContext( TraceContext *ctx, u32 eip ) const
{
    m_debugger.UpdateTraceContext(ctx, eip);
    m_disassembler.UpdateInstContext(ctx, eip);
    m_taint.UpdateInstContext(ctx);
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
    SaveArchive();
    m_enabled = false;
    //m_emulator->Terminate();
    m_debugger.OnTerminate();
}

void AEngine::CreateArchiveDirectory()
{
    std::string dir     = LxGetModuleDirectory(g_module);
    std::string arcDir  = g_config.GetString("General", "ArchiveDir", "archive");
    m_archivePath       = dir + arcDir + "\\";
    if (!LxFileExists(m_archivePath.c_str())) {
        if (CreateDirectoryA(m_archivePath.c_str(), NULL)) {
            LxInfo("Arietis archive directory created\n");
        } else {
            LxFatal("Error creating Arietis archive directory\n");
        }
    }
}

void AEngine::LoadArchive(const char *moduleName)
{
    LPCSTR path     = m_emulator->Path();
    uint hash       = StringHash(path);
    hash            ^= FileTimeHash(path);

    char buf[MAX_PATH];
    sprintf(buf, "%08x_", hash);
    strcat(buf, moduleName);
    strcat(buf, ".json");
    m_archivePath   = m_archivePath + buf;

    m_isArchiveLoaded = true;

    if (LxFileExists(m_archivePath.c_str())) {
        std::ifstream fin(m_archivePath);
        std::stringstream ss;
        ss << fin.rdbuf();
        std::string s(ss.str());
        fin.close();

        if (!Serializer::Deserialzie(&m_archive, s)) {
            LxFatal("Error deserializing archive file\n");
        }
    }

    m_gui->OnArchiveLoaded(&m_archive);
}

void AEngine::SaveArchive()
{
    Assert(m_isArchiveLoaded);

    std::string str;
    if (!Serializer::Serialize(&m_archive, str)) {
        LxFatal("Error serializing archive file\n");
    }

    std::ofstream fout(m_archivePath);
    fout << str;
    fout.close();
}



