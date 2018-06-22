#pragma once
#include "tool"
class ArgumentHandle
{
public:
	ArgumentHandle()
	{	

	}
	bool useDefault = 1;
	int serverPort=80;
	int readerCounts=2;
	int senderCounts =2;
	int processorCounts = 2;
	string mainAppPath = "";
	string staticPath="\\static";
	string xmlPath="\\setting.xml";
	~ArgumentHandle()
	{
	}
};

