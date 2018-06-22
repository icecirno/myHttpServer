#pragma once
#include "tool"
class CachedStaticFile
{
public:
	std::string *data;
	std::string name;
	std::string type="text/html";
	size_t len;
	bool isCompress=1;
	int cacheTime = 100;
	CachedStaticFile(std::string *data, std::string name, size_t len, bool isCompress = 1) :data(data), name(name)
	{
		this->len = len;
		this->isCompress = isCompress;
	}
	void setType(std::string  type)
	{
		this->type = type;
	}
	~CachedStaticFile()
	{
		delete data;
	}
};

