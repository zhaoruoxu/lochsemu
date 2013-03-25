#pragma once
 
#ifndef __PROPHET_PLUGIN_TAINT_DIRECTIVE_H__
#define __PROPHET_PLUGIN_TAINT_DIRECTIVE_H__
 
#include "plugin.h"
#include "engine.h"
#include "utilities.h"
#include "event.h"

#include "processor.h"
#include "memory.h"

/*
 * This plugin automatically taints data segments that have prefix '.taint',
 * and instructions in code segment which are surrounded by 
 * TAINT_CODE_BEGIN() and TAINT_CODE_END() macros.
 */

static const std::string TaintSegmentPrefix = ".taint";
static const uint       TaintBegin          = 0x14159265;
static const uint       TaintEnd            = 0x65921514;
static const uint       TaintDataSeg        = 0xf1edcacf;

class TaintDirective : public Plugin {
public:
    TaintDirective(ProPluginManager *manager) 
        : Plugin(manager, "TaintDirective", PreExecuteHandler | PostExecuteHandler | ProcessPreRunHandler) 
    {
        m_taintAllBits = false;
        m_lastEip = 0;
    }

    void    Initialize() override;
    virtual void    OnProcessPreRun(ProcessPreRunEvent &event, bool firstTime) override;
    virtual void    OnPreExecute(PreExecuteEvent &event, bool firstTime) override {
        if (!firstTime) return;
        m_lastEip = event.Cpu->EIP;
    }
    virtual void    OnPostExecute(PostExecuteEvent &event, bool firstTime) override;

    virtual void    Serialize(Json::Value &root) const override;
    virtual void    Deserialize(Json::Value &root) override;

private:
    void    DoTaint(const Processor *cpu);

private:
    bool    m_taintAllBits;
    u32     m_lastEip;
};


 
#endif // __PROPHET_PLUGIN_TAINT_DIRECTIVE_H__