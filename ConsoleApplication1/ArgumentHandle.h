#pragma once
#include "tool"
class ArgumentHandle
{
public:
	ArgumentHandle(int argc, char** argv)
	{

	}
	int serverPort=80;
	int readerCounts=2;
	int senderCounts = 2;
	int processorCounts = 2;
	string sourcePath="\\source";
	string xmlPath="\\setting";
	~ArgumentHandle()
	{
	}
};

