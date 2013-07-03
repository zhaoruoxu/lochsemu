#include "stdafx.h"
#include "dbtrace.h"
#include "message.h"

#include "sqlite/SQLiteC++.h"

DBTrace::DBTrace()
{
	
}

DBTrace::~DBTrace()
{
    SAFE_DELETE(m_stmtInsertMessage);
	SAFE_DELETE(m_db);
}

void DBTrace::Initialize( const char * dbpath )
{
	LxInfo("Creating database %s\n", dbpath);
	m_db = new SQLite::Database(dbpath, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
	
	InitTables();
    InitStatements();
}

void DBTrace::Serialize( Json::Value &root ) const 
{

}

void DBTrace::Deserialize( Json::Value &root )
{

}

void DBTrace::InitTables()
{
    try {
        SQLite::Transaction t(*m_db);
        m_db->exec("DROP TABLE IF EXISTS messages");
        m_db->exec("DROP TABLE IF EXISTS messages_data");
        m_db->exec("CREATE TABLE messages (id INTEGER PRIMARY KEY, trace_count INTEGER, length INTEGER)");
        m_db->exec("CREATE TABLE messages_data (id INTEGER, offset INTEGER, data CHAR, FOREIGN KEY(id) REFERENCES messages(id))");
        t.commit();
    } catch (std::exception &e) {
        LxFatal("Sqlite Exception: %s\n", e.what());
    }
}

void DBTrace::TraceMessage( Message *msg )
{
    try {
        SQLite::Transaction t(*m_db);
        m_stmtInsertMessage->bind(":id", msg->GetID());
        m_stmtInsertMessage->bind(":trace_count", msg->GetTraceEnd() - msg->GetTraceBegin() + 1);
        m_stmtInsertMessage->bind(":length", msg->Size());
        m_stmtInsertMessage->exec();
        m_db->exec("INSERT INTO messages_data VALUES(23, 3, 'a')");
        t.commit();
    } catch (std::exception &e) {
        LxFatal("Sqlite Exception: %s\n", e.what());
    }

}

void DBTrace::InitStatements()
{
    m_stmtInsertMessage = new SQLite::Statement(*m_db, "INSERT INTO messages VALUES ( :id, :trace_count, :length )");
}
