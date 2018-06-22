#pragma once
#include <string>
class SqlConnector
{
	explicit SqlConnector();
public:
	void setHostName(std::string &hostname);
	void setPort(std::string &port);
	void setDatabaseName(std::string &dbname);
	void setUserName(std::string &username);
	void setPassword(std::string &password);
	bool open(std::string &error);
};

