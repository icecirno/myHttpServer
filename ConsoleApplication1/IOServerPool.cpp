#include "IOServerPool.h"

IOServerPool *IOServerPool::instance=0;
IOServerPool *IOServerPool::getInstance(int number)
{
	if(instance)
		return instance;
	instance = new IOServerPool(number);
		return instance;
}
void IOServerPool::ioServiceRun(boost::asio::io_service * s)
{
	if (s == 0)
		return;
	boost::asio::io_service::work work(*s);
	s->run();
	debug("ioService stoped", s);
}
IOServerPool::IOServerPool(int number)
{
	if (number <= 0)
		number = 1;
	for (int i=0;i<number;++i)
	{
		boost::asio::io_service* s = new boost::asio::io_service();
		services.push_back(s);
		boost::thread(boost::bind(&IOServerPool::ioServiceRun,s));
	}
}

IOServerPool::~IOServerPool()
{
	for (int i = services.size()-1; i>=0; --i)
	{
		boost::asio::io_service* s = services.at(i);
		if(s->stopped())
			continue;
		s->stop();
		delete s;
	}
}
