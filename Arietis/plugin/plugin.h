#pragma once
 
#ifndef __ARIETIS_PLUGIN_PLUGIN_H__
#define __ARIETIS_PLUGIN_PLUGIN_H__
 
#include "Arietis.h"
#include "utilities.h"

enum FuncOverride : uint {
    Func_PreExecute     = 1,
    Func_PostExecute    = 1 << 2,
    Func_MemRead        = 1 << 3,
    Func_MemWrite       = 1 << 4,
    Func_ProcessPreRun  = 1 << 5,
    Func_ProcessPostRun = 1 << 6,
    Func_ProcessPreLoad = 1 << 7,
    Func_ProcessPostLoad= 1 << 8,
    Func_WinapiPreCall  = 1 << 9,
    Func_WinapiPostCall = 1 << 10,
};



class Plugin : public ISerializable {
public:
    Plugin(const std::string name, uint ovd);
    virtual ~Plugin() {}

    std::string     GetName() const { return m_name; }
    bool            IsEnabled() const { return m_enabled; }
    void            Enable(bool isEnabled) { m_enabled = isEnabled; }
    bool            HasOverrideFlag(FuncOverride f) const { return (m_ovdFlag & f) != 0; }
    APluginManager* GetManager() { return m_manager; }
    const APluginManager *  GetManager() const { return m_manager; }

    void            Serialize(Json::Value &root) const override;
    void            Deserialize(Json::Value &root) override;

    virtual void    Initialize() {}
    virtual void    OnPreExecute        (Processor *cpu, const Instruction *inst) {}
    virtual void    OnPostExecute       (Processor *cpu, const Instruction *inst) {}
    virtual void    OnMemRead           (const Processor *cpu, u32 addr, u32 nbytes, cpbyte data) {}
    virtual void    OnMemWrite          (const Processor *cpu, u32 addr, u32 nbytes, cpbyte data) {}
    virtual void    OnProcessPreRun     (const Process *proc, const Processor *cpu) {}
    virtual void    OnProcessPostRun    (const Process *proc) {}
    virtual void    OnProcessPreLoad    (const PeLoader *loader) {}
    virtual void    OnProcessPostLoad   (const PeLoader *loader) {}
    virtual void    OnWinapiPreCall     (Processor *cpu, uint apiIndex) {}
    virtual void    OnWinapiPostCall    (Processor *cpu, uint apiIndex) {}
    
private:
    std::string     m_name;
    bool            m_enabled;
    APluginManager *m_manager;
    uint    m_ovdFlag;
};


class APluginManager : public ISerializable {
public:
    APluginManager(AEngine *engine);
    ~APluginManager();

    void            Initialize();
    void            RegisterPlugin(Plugin *plugin);
    AEngine *       GetEngine() { return m_engine; }
    const AEngine * GetEngine() const { return m_engine; }

    virtual void    Serialize(Json::Value &root) const override;
    virtual void    Deserialize(Json::Value &root) override;

    void            OnPreExecute        (Processor *cpu, const Instruction *inst);
    void            OnPostExecute       (Processor *cpu, const Instruction *inst);
    void            OnMemRead           (const Processor *cpu, u32 addr, u32 nbytes, cpbyte data);
    void            OnMemWrite          (const Processor *cpu, u32 addr, u32 nbytes, cpbyte data);
    void            OnProcessPreRun     (const Process *proc, const Processor *cpu);
    void            OnProcessPostRun    (const Process *proc);
    void            OnProcessPreLoad    (const PeLoader *loader);
    void            OnProcessPostLoad   (const PeLoader *loader);
    void            OnWinapiPreCall     (Processor *cpu, uint apiIndex);
    void            OnWinapiPostCall    (Processor *cpu, uint apiIndex);

private:
    std::vector<Plugin *>   m_plugins;
    AEngine *       m_engine;
};

#endif // __ARIETIS_PLUGIN_PLUGIN_H__