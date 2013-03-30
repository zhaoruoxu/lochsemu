#pragma once

#ifndef __CORE_EMULATOR_H__
#define __CORE_EMULATOR_H__

#include "lochsemu.h"
#include "singleton.h"

BEGIN_NAMESPACE_LOCHSEMU()

#define LX_CMDLINE_SIZE 1024

class LX_API Emulator {
public:
    Emulator();
    virtual ~Emulator();

public:
    LxResult        Initialize();
    LxResult        LoadModule(int argc, LPSTR argv[]);
    void            Run();
    void            Reset();
    Processor *     GetProcessorMain();
    Processor *     GetProcessorByThreadID(ThreadID id);

public:
    Memory *        Mem() const 
    { 
        Assert(m_memory); 
        return m_memory; 
    }

    Process *       Proc() const 
    { 
        return m_process; 
    }

    RefProcess *    RefProc() const 
    { 
        Assert(m_refProcess); 
        return m_refProcess; 
    }

    PluginManager * Plugins() const 
    { 
        Assert(m_pluginManager); 
        return m_pluginManager; 
    }

    PeLoader *      Loader() const 
    { 
        Assert(m_loader); 
        return m_loader; 
    }

    LPCSTR          Path() const 
    { 
        return m_path; 
    }

    LPCSTR          CmdLine() const 
    { 
        return m_cmdline; 
    }

    bool            IsLoaded() const 
    { 
        return m_loaded; 
    }

    void            Terminate();
public:
    u32             InquireStackBase(void);

    u32             InquireStackLimit(void);

    u32             InquirePebAddress(void);


public:
    void            ConstructCmdline(int argc, LPSTR argv[]);

    HMODULE         TranslateModule(HMODULE hModule);

    HMODULE         GetRealHModule() const 
    { 
        return m_hModule; 
    }

protected:
    Memory *        m_memory;
    Process *       m_process;
    RefProcess *    m_refProcess;
    PeLoader *      m_loader;
    PluginManager * m_pluginManager;
    char            m_path[MAX_PATH];
    char            m_cmdline[LX_CMDLINE_SIZE];
    bool            m_loaded;
    HMODULE         m_hModule;


};

END_NAMESPACE_LOCHSEMU()

#endif // __CORE_EMULATOR_H__
