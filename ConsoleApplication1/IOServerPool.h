#pragma once
#include "tool"

class IOServerPool
{
	size_t currentIndex = 0;
	vector<boost::asio::io_service *> services;
	IOServerPool(int number=8);
	static IOServerPool *instance;
	static void ioServiceRun(boost::asio::io_service * s);
public:	
	static IOServerPool *getInstance(int number = 8);
	void stopAll()
	{
		for (auto i = services.begin(); i != services.end(); ++i)
		{
			(*i)->stop();
		}
	}
	boost::asio::io_service *getService()
	{
		currentIndex += 1;
		if (currentIndex < services.size())
		{
			return services.at(currentIndex);
		}
		currentIndex = 0;
		return services.at(0);
	}
	~IOServerPool();
};

