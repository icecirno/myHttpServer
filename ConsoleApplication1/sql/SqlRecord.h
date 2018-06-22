#pragma once
#include <string>
class SqlRecord
{
public:
	SqlRecord();
	~SqlRecord();
	void clear();
	int count();
	bool isEmpty();
	std::string *value(std::string &key);
	bool next();
};

