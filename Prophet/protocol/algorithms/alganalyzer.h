#pragma once
 
#ifndef __PROPHET_PROTOCOL_ALGORITHMS_ALGANALYZER_H__
#define __PROPHET_PROTOCOL_ALGORITHMS_ALGANALYZER_H__
 
#include "protocol/analyzers/procexec.h"
#include "protocol/taint/taintengine.h"

class AlgorithmAnalyzer : public ProcAnalyzer {
public:
    AlgorithmAnalyzer(TSnapshot &t);
    virtual void OnProcedure(ExecuteTraceEvent &event, const ProcContext &ctx) override;

private:
    TSnapshot &m_snapshot;
};
 
#endif // __PROPHET_PROTOCOL_ALGORITHMS_ALGANALYZER_H__