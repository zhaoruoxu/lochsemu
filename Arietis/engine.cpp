#include "stdafx.h"
#include "engine.h"
#include "event.h"
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
    //m_mainEntryEntered  = false;
    m_enabled           = true;
    m_isArchiveLoaded   = false;

    m_archive.AddObject("plugins",      &m_plugins);
    m_archive.AddObject("debugger",     &m_debugger);
    m_archive.AddObject("tracer",       &m_tracer);
    m_archive.AddObject("taintengine",  &m_taint);

    CreateArchiveDirectory();
}

void AEngine::OnPreExecute( Processor *cpu, const Instruction *inst )
{
    if (!m_enabled) return;
    PreExecuteEvent event(this, cpu, inst);

    m_plugins.OnPreExecute(event, true);
    if (event.IsVetoed()) return;

    m_tracer.OnPreExecute(event);
    m_disassembler.OnPreExecute(event);
    m_debugger.OnPreExecute(event);

    m_plugins.OnPreExecute(event, false);
}

void AEngine::OnPostExecute( Processor *cpu, const Instruction *inst )
{
    if (!m_enabled) return;
    PostExecuteEvent event(this, cpu, inst);

    m_plugins.OnPostExecute(event, true);
    if (event.IsVetoed()) return;

    m_taint.OnPostExecute(event);
    m_tracer.OnPostExecute(event);

    m_plugins.OnPostExecute(event, false);
}

void AEngine::OnMemRead( const Processor *cpu, u32 addr, u32 nbytes, cpbyte data )
{
    if (!m_enabled) return;
    MemReadEvent event(this, cpu, addr, nbytes, data);

    m_plugins.OnMemRead(event, true);
    if (event.IsVetoed()) return;

    // other modules

    m_plugins.OnMemRead(event, false);
}

void AEngine::OnMemWrite( const Processor *cpu, u32 addr, u32 nbytes, cpbyte data )
{
    if (!m_enabled) return;
    MemWriteEvent event(this, cpu, addr, nbytes, data);

    m_plugins.OnMemWrite(event, true);
    if (event.IsVetoed()) return;

    m_plugins.OnMemWrite(event, false);
}

void AEngine::OnProcessPreRun( const Process *proc, const Processor *cpu )
{
    if (!m_enabled) return;

    ProcessPreRunEvent event(this, proc, cpu);

    m_plugins.OnProcessPreRun(event, true);
    if (event.IsVetoed()) return;

    m_debugger.OnProcessPreRun(event);

    m_plugins.OnProcessPreRun(event, false);
}

void AEngine::OnProcessPostRun( const Process *proc )
{
    if (!m_enabled) return;
    ProcessPostRunEvent event(this, proc);

    m_plugins.OnProcessPostRun(event, true);
    if (event.IsVetoed()) return;

    SaveArchive();

    m_plugins.OnProcessPostRun(event, false);
}

void AEngine::OnProcessPreLoad( const PeLoader *loader )
{
    if (!m_enabled) return;
    ProcessPreLoadEvent event(this, loader);

    m_plugins.OnProcessPreLoad(event, true);
    if (event.IsVetoed()) return;

    m_plugins.OnProcessPreLoad(event, false);
}

void AEngine::OnProcessPostLoad( const PeLoader *loader )
{
    if (!m_enabled) return;
    ProcessPostLoadEvent event(this, loader);

    m_plugins.OnProcessPostLoad(event, true);
    if (event.IsVetoed()) return;

    LoadArchive(loader->GetModuleInfo(0)->Name);
    m_debugger.OnProcessPostLoad(event);
    m_gui->OnProcessLoaded(m_emulator->Path());

    m_plugins.OnProcessPostLoad(event, false);
}

void AEngine::OnWinapiPreCall( Processor *cpu, uint apiIndex )
{
    if (!m_enabled) return;
    WinapiPreCallEvent event(this, cpu, apiIndex);

    m_plugins.OnWinapiPreCall(event, true);
    if (event.IsVetoed()) return;

    m_plugins.OnWinapiPreCall(event, false);
}

void AEngine::OnWinapiPostCall( Processor *cpu, uint apiIndex )
{
    if (!m_enabled) return;
    WinapiPostCallEvent event(this, cpu, apiIndex);

    m_plugins.OnWinapiPostCall(event, true);
    if (event.IsVetoed()) return;

    m_plugins.OnWinapiPostCall(event, false);
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



