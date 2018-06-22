#pragma once
#include <string>
#include "SqlConnector.h"
#include "SqlRecord.h"
class SqlQuery
{
public:
	SqlQuery(SqlConnector*db);
	~SqlQuery();
	SqlRecord & exec(std::string &sql);
};

