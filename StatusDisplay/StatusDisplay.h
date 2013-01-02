// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the STATUSDISPLAY_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// STATUSDISPLAY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef STATUSDISPLAY_EXPORTS
#define STATUSDISPLAY_API extern "C" __declspec(dllexport)
#else
#define STATUSDISPLAY_API extern "C" __declspec(dllimport)
#endif

#include "pluginapi.h"

using namespace LochsEmu;

extern HMODULE g_module;

void RunGUI();

STATUSDISPLAY_API bool LochsEmu_Plugin_Initialize(const LochsEmuInterface *lochsemu, PluginInfo *info);