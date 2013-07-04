#include "stdafx.h"
#include "dbtrace.h"
#include "message.h"
#include "runtrace.h"
#include "sqlite/SQLiteC++.h"

DBTrace::DBTrace()
{
	m_instid = 0;
}

DBTrace::~DBTrace()
{
    SAFE_DELETE(m_stmtInsertMessage);
    SAFE_DELETE(m_stmtInsertMessageData);
    SAFE_DELETE(m_stmtInsertInst);
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
        m_db->exec("DROP TABLE IF EXISTS insts");
        m_db->exec("CREATE TABLE messages (id INTEGER PRIMARY KEY, "
            "trace_count INTEGER, length INTEGER)");
        m_db->exec("CREATE TABLE messages_data (id INTEGER, offset INTEGER, "
            "data INTEGER, FOREIGN KEY(id) REFERENCES messages(id))");
        m_db->exec("CREATE TABLE insts (id BIGINT, offset INTEGER, "
            "msg_id INTEGER)");
        t.commit();
    } catch (std::exception &e) {
        LxFatal("Sqlite Exception: %s\n", e.what());
    }
}

void DBTrace::InitStatements()
{
    m_stmtInsertMessage = new SQLite::Statement(*m_db, 
        "INSERT INTO messages VALUES ( :id, :trace_count, :length )");
    m_stmtInsertMessageData = new SQLite::Statement(*m_db,
        "INSERT INTO messages_data VALUES ( :id, :offset, :data )");
    m_stmtInsertInst = new SQLite::Statement(*m_db,
        "INSERT INTO insts VALUES ( :id, :offset, :msg_id )");
}


void DBTrace::TraceMessage( Message *msg )
{
    try {
        SQLite::Transaction t(*m_db);
        m_stmtInsertMessage->bind(":id", msg->GetID());
        m_stmtInsertMessage->bind(":trace_count", 
            msg->GetTraceEnd() - msg->GetTraceBegin() + 1);
        m_stmtInsertMessage->bind(":length", msg->Size());
        m_stmtInsertMessage->exec();
        m_stmtInsertMessage->reset();

        for (int i = 0; i < msg->Size(); i++) {
            m_stmtInsertMessageData->bind(":id", msg->GetID());
            m_stmtInsertMessageData->bind(":offset", i);
            m_stmtInsertMessageData->bind(":data", msg->Get(i));
            m_stmtInsertMessageData->exec();
            m_stmtInsertMessageData->reset();
        }

        t.commit();
    } catch (std::exception &e) {
        LxFatal("Sqlite Exception: %s\n", e.what());
    }

}

void DBTrace::TraceInsts( RunTrace &rt, Message *msg )
{
    try {
        SQLite::Transaction t(*m_db);
        for (int i = 0; i < rt.Count(); i++) {
            const TContext *ctx = rt.Get(i);
            m_stmtInsertInst->bind(":id", m_instid++);
            m_stmtInsertInst->bind(":offset", (int) ctx->Eip);
            m_stmtInsertInst->bind(":msg_id", msg->GetID());
            m_stmtInsertInst->exec();
            m_stmtInsertInst->reset();
        }
        t.commit();
    } catch (std::exception &e) {
        LxFatal("Sqlite Exception: %s\n", e.what());
    }
}
