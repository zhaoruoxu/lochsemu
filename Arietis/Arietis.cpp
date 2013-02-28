// Arietis.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Arietis.h"
#include "winapi.h"
#include "instruction.h"
#include "peloader.h"

#include "engine.h"

#include <json/json.h>

#include "gui/gui.h"

PluginHandle    g_handle;
Config          g_config;

Emulator *      g_emulator;
AEngine         g_engine;
Semaphore       g_guiSem;

ARIETIS_API bool LochsEmu_Plugin_Initialize(const LochsEmuInterface *lochsemu, PluginInfo *info)
{
    g_handle    = lochsemu->Handle;
    g_emulator  = lochsemu->Emulator;
    strcpy_s(info->Name, sizeof(info->Name), "Arietis");

    std::string dir = LxGetModuleDirectory(g_module);
    std::string cfgFile = dir + "arietis.ini";
    g_config.Initialize(cfgFile.c_str());

    if (!g_config.GetInt("General", "Enabled", 1)) {
        LxInfo("Arietis is disabled\n");
        return false;
    }

    g_engine.Initialize(lochsemu->Emulator);

    RunGUI();

    g_guiSem.Wait();
    return true;
}

ARIETIS_API void LochsEmu_Process_PostLoad           (PeLoader *loader)
{
    g_engine.OnProcessPostLoad(loader);
}

ARIETIS_API void LochsEmu_Winapi_PreCall( Processor *cpu, uint apiIndex )
{
    //auto dllName = LxGetWinAPIModuleName(apiIndex);
    //auto apiName = LxGetWinAPIName(apiIndex);
    //char buf[256];
    //sprintf(buf, "%s:%s", dllName, apiName);
    //if (!g_rdebugger.SendString(buf)) {
    //    LxFatal("SendString failed\n");
    //}
    //LxWarning("ARIETIS PRE  %s:%s\n", dllName, apiName);
}

ARIETIS_API void LochsEmu_Winapi_PostCall( Processor *cpu, uint apiIndex )
{
    //auto dllName = LxGetWinAPIModuleName(apiIndex);
    //auto apiName = LxGetWinAPIName(apiIndex);
    //LxWarning("ARIETIS POST %s:%s\n", dllName, apiName);
}

ARIETIS_API void LochsEmu_Processor_PreExecute(Processor *cpu, const Instruction *inst)
{
    g_engine.OnPreExecute(cpu, inst);

}

ARIETIS_API void LochsEmu_Processor_PostExecute(Processor *cpu, const Instruction *inst)
{
    g_engine.OnPostExecute(cpu, inst);
}

ARIETIS_API void LochsEmu_Process_PreRun            (const Process *proc, Processor *cpu)
{
    g_engine.OnProcessPreRun(proc, cpu);
}

