// Arietis.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Arietis.h"
#include "winapi.h"

#include <json/json.h>

#include "taint/taint.h"
#include "taint/engine.h"
#include "rdebug/rdebugger.h"

PluginHandle    g_handle;
Config          g_config;

RDebugger       g_rdebugger;
TaintEngine     g_engine;

ARIETIS_API bool LochsEmu_Plugin_Initialize(const LochsEmuInterface *lochsemu, PluginInfo *info)
{
    g_handle = lochsemu->Handle;
    strcpy_s(info->Name, sizeof(info->Name), "Arietis");

    std::string cfgFile = LxGetModuleDirectory(g_module) + "lochsdbg.ini";
    g_config.Initialize(cfgFile.c_str());

    if (!g_config.GetInt("General", "Enabled", 1)) {
        LxInfo("Arietis is disabled\n");
        return false;
    }

    Json::Value root;
    Json::Reader reader;
    std::ifstream fin("../Arietis/test.json");

    char buf[256];
    GetCurrentDirectoryA(256, buf);

    if (!fin.is_open()) {
        LxError("Cannot open test.json\n");
    }
    if (!reader.parse(fin, root)) {
        LxFatal("Error loading test.json\n");
    }

    g_engine.Initialize();

    return true;
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
    

}

ARIETIS_API void LochsEmu_Processor_PostExecute(Processor *cpu, const Instruction *inst)
{
    g_engine.OnPostExecute(cpu, inst);
}