#include "stdafx.h"
#include "dbtrace.h"

#include "sqlite/SQLiteC++.h"

DBTrace::DBTrace()
{
	
}

DBTrace::~DBTrace()
{
	SAFE_DELETE(m_db);
}

void DBTrace::Initialize( const char * dbpath )
{
	LxInfo("Creating database %s\n", dbpath);
	m_db = new SQLite::Database(dbpath, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
	
	//SQLite::Statement s(*m_db, "DROP TABLE test IF EXIST");
	//s.exec();
	SQLite::Transaction t(*m_db);
	m_db->exec("DROP TABLE IF EXISTS test");
	m_db->exec("CREATE TABLE test (msg INTEGER PRIMARY KEY)");
	t.commit();
}

void DBTrace::Serialize( Json::Value &root ) const 
{

}

void DBTrace::Deserialize( Json::Value &root )
{

}
