#pragma once
 
#ifndef __PROPHET_PROTOCOL_DBTRACE_H__
#define __PROPHET_PROTOCOL_DBTRACE_H__

#include "prophet.h"
#include "utilities.h"

namespace SQLite {
	class Database;
}

class DBTrace : ISerializable {
public:
	DBTrace();
	~DBTrace();

	void	Initialize(const char * dbpath);

	void	Serialize(Json::Value &root) const override;
	void	Deserialize(Json::Value &root) override;
private:
	SQLite::Database *	m_db;
};


#endif // __PROPHET_PROTOCOL_DBTRACE_H__