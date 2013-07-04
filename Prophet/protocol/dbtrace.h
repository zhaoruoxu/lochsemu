#pragma once
 
#ifndef __PROPHET_PROTOCOL_DBTRACE_H__
#define __PROPHET_PROTOCOL_DBTRACE_H__

#include "prophet.h"
#include "utilities.h"

namespace SQLite {
	class Database;
	class Statement;
}

class DBTrace : ISerializable {
public:
	DBTrace();
	~DBTrace();

	void	Initialize(const char * dbpath);

	void	Serialize(Json::Value &root) const override;
	void	Deserialize(Json::Value &root) override;

	void	TraceMessage(Message *msg);
    void    TraceInsts(RunTrace &rt, Message *msg);
private:
	void	InitTables();
	void    InitStatements();
private:
    long long m_instid;
	SQLite::Database *	m_db;
	SQLite::Statement * m_stmtInsertMessage;
    SQLite::Statement * m_stmtInsertMessageData;
    SQLite::Statement * m_stmtInsertInst;
};


#endif // __PROPHET_PROTOCOL_DBTRACE_H__