#pragma once
 
#ifndef __PROPHET_PROTOCOL_ANALYZERS_PROCSCOPE_H__
#define __PROPHET_PROTOCOL_ANALYZERS_PROCSCOPE_H__
 
#include "protocol/analyzer.h"

/*
 * 分析函数入口点、所有指令地址以及出口
 */

class Procedure {
public:
	Procedure(u32 entry) : m_entry(entry) { Extend(m_entry); }

	void	Extend(u32 addr) { m_addrs.insert(addr); }
	void	Exit(u32 addr);
	void	Dump(File &f, Disassembler *disasm) const;
    bool    Containts(u32 addr) const;

	u32		Entry() const { return m_entry; }
	std::set<u32> &	Addrs() { return m_addrs; }
	const std::set<u32> &	Addrs() const { return m_addrs; }
	std::set<u32> &	Exits() { return m_exits; }
	const std::set<u32> &	Exits() const { return m_exits; }

private:
	u32	m_entry;
	std::set<u32>	m_addrs;
	std::set<u32>	m_exits;
};


class ProcScope : public TraceAnalyzer {
public:
	ProcScope();
	virtual ~ProcScope();

	virtual void Reset() override;
	virtual void OnExecuteTrace(ExecuteTraceEvent &event) override;
    virtual void OnProcBegin(ExecuteTraceEvent &event) override;
    virtual void OnProcEnd(ExecuteTraceEvent &event) override;
	virtual void OnComplete() override;

	Procedure *	Get(u32 entry) const;

	void	Dump(File &f, Disassembler *disasm) const;
private:
	Procedure *	GetProc(const TContext *entry);
	void	CheckValidity() const;
private:
    std::set<Procedure *> m_entryProcs;
	std::map<u32, Procedure *>	m_procs;
	std::stack<Procedure *>		m_callStack;
};
 
#endif // __PROPHET_PROTOCOL_ANALYZERS_PROCSCOPE_H__