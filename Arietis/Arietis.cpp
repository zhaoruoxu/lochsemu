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
    GetCurrentDirectory(256, buf);

    if (!fin.is_open()) {
        LxError("Cannot open test.json\n");
    }
    if (!reader.parse(fin, root)) {
        LxFatal("Error loading test.json\n");
    }

    Taint t(23);

    LxInfo("haha = %s\n", root["haha"].asCString());

    g_rdebugger.Connect("127.0.0.1", 7777);

    return true;
}

ARIETIS_API void LochsEmu_Winapi_PreCall( Processor *cpu, uint apiIndex )
{
    auto dllName = LxGetWinAPIModuleName(apiIndex);
    auto apiName = LxGetWinAPIName(apiIndex);
    char buf[256];
    sprintf(buf, "%s:%s", dllName, apiName);
    g_rdebugger.SendString(buf);
    LxWarning("ARIETIS PRE  %s:%s\n", dllName, apiName);
}

ARIETIS_API void LochsEmu_Winapi_PostCall( Processor *cpu, uint apiIndex )
{
    auto dllName = LxGetWinAPIModuleName(apiIndex);
    auto apiName = LxGetWinAPIName(apiIndex);
    LxWarning("ARIETIS POST %s:%s\n", dllName, apiName);
}
