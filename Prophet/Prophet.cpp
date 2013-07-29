// Prophet.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "prophet.h"
#include "winapi.h"
#include "instruction.h"
#include "peloader.h"

#include "engine.h"

#include <json/json.h>

#include "gui/gui.h"

PluginHandle    g_handle;
Config          g_config;

Emulator *      g_emulator;
ProEngine       g_engine;
Semaphore       g_guiSem;


PROPHET_API bool LochsEmu_Plugin_Initialize(const LochsEmuInterface *lochsemu, PluginInfo *info)
{
    g_handle    = lochsemu->Handle;
    g_emulator  = lochsemu->Emulator;
    strcpy_s(info->Name, sizeof(info->Name), "Prophet");

    std::string dir = LxGetModuleDirectory(g_module);
    std::string cfgFile = dir + "prophet.ini";
    g_config.Initialize(cfgFile.c_str());

    if (!g_config.GetInt("General", "Enabled", 1)) {
        LxInfo("Prophet is disabled\n");
        return false;
    }

    g_engine.Initialize(lochsemu->Emulator);

    if (g_config.GetInt("General", "GUI", 0) != 0) {
        RunGUI();
        g_guiSem.Wait();
    } else {
        g_engine.NoGUI();
    }

    return true;
}

PROPHET_API void LochsEmu_Process_PreLoad( PeLoader *loader )
{
    g_engine.OnProcessPreLoad(loader);
}

PROPHET_API void LochsEmu_Process_PostLoad(PeLoader *loader)
{
    g_engine.OnProcessPostLoad(loader);
}

PROPHET_API void LochsEmu_Winapi_PreCall( Processor *cpu, uint apiIndex )
{
    g_engine.OnWinapiPreCall(cpu, apiIndex);
}

PROPHET_API void LochsEmu_Winapi_PostCall( Processor *cpu, uint apiIndex )
{
    g_engine.OnWinapiPostCall(cpu, apiIndex);
}

PROPHET_API void LochsEmu_Processor_PreExecute(Processor *cpu, const Instruction *inst)
{
    g_engine.OnPreExecute(cpu, inst);
}

PROPHET_API void LochsEmu_Processor_PostExecute(Processor *cpu, const Instruction *inst)
{
    g_engine.OnPostExecute(cpu, inst);
}

PROPHET_API void LochsEmu_Processor_MemRead( const Processor *cpu, u32 address, u32 nBytes, cpbyte data )
{
    g_engine.OnMemRead(cpu, address, nBytes, data);
}

PROPHET_API void LochsEmu_Processor_MemWrite( const Processor *cpu, u32 address, u32 nBytes, cpbyte data )
{
    g_engine.OnMemWrite(cpu, address, nBytes, data);
}

PROPHET_API void LochsEmu_Process_PreRun(const Process *proc, Processor *cpu)
{
    g_engine.OnProcessPreRun(proc, cpu);
}

PROPHET_API void LochsEmu_Process_PostRun( const Process *proc )
{
    g_engine.OnProcessPostRun(proc);
}

PROPHET_API bool LochsEmu_Plugin_Cleanup( void )
{
    if (g_engine.GUIEnabled()) {
        WaitForGUI();
    } else {
        g_engine.Terminate();
    }
    
    return true;
}

PROPHET_API void LochsEmu_Thread_Create( Thread *thrd )
{
    g_engine.OnThreadCreate(thrd);
}

PROPHET_API void LochsEmu_Thread_Exit( Thread *thrd )
{
    g_engine.OnThreadExit(thrd);
}





