#pragma once

#ifndef __LOCHSDBG_STATISTICS_H__
#define __LOCHSDBG_STATISTICS_H__

#include "LochsDbg.h"

class LochsStatistics {
public:
    LochsStatistics();
    ~LochsStatistics();

    void        Initialize(void);
    void        OnProcessPreRun(const Process *proc, Processor *cpu);
    void        OnProcessPostRun(const Process *proc);
    void        OnProcessorPreExecute(Processor *cpu, const Instruction *inst);

private:
    std::map<std::string, int>  m_instCount;
    bool        m_enabled;
};

extern LochsStatistics LxStatistics;

#endif // __LOCHSDBG_STATISTICS_H__
