#pragma once
#include "tool"
class CachedStaticFile
{
public:
	std::string *data;
	std::string name;
	bool isCompress=1;
	CachedStaticFile()
	{
	}
	~CachedStaticFile()
	{
	}
};

