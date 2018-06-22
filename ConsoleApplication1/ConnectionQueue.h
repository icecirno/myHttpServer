#pragma once
#include "HTTPRequest.h"
#include "ArgumentHandle.h"
#include "Queue.h"
class ConnectionQueue
{
public:
	ConnectionQueue()
	{
	}
	void keepThisConnection(HTTPRequest* c)
	{
		if (c == 0)return;
		if (!c->is_open())return;
		keepconnectQueue.locker.lock();
		keepconnectQueue.data.push_back(c);
		keepconnectQueue.locker.unlock();
		debug("keepKLConnection", c->socket->remote_endpoint().address().to_string());
	}
	ConnectionQueue& operator<<(boost::asio::ip::tcp::socket* c)
	{
		if (c == 0)return *this;
		if (!c->is_open())return *this;
		currenQueue.locker.lock();
		currenQueue.data.push_back(c);
		currenQueue.locker.unlock();
	}
	void clear()
	{
		keepconnectQueue.locker.lock();
		if (keepconnectQueue.data.size())
			for (auto i = keepconnectQueue.data.begin(); i != keepconnectQueue.data.end(); ++i)
			{
				if ((*i) != 0)
				{
					(*i)->release();
					delete (*i);
				}
			}
		keepconnectQueue.data.clear();
		keepconnectQueue.locker.unlock();

		currenQueue.locker.lock();
		for(auto i= currenQueue.data.begin();i!= currenQueue.data.end();++i)
		{
			if ((*i) != 0)
			{
				(*i)->cancel();
				(*i)->close();
				(*i)->release();
				delete (*i);
			}
		}
		currenQueue.data.clear();
		currenQueue.locker.unlock();
		
	}
	boost::asio::ip::tcp::socket* get()
	{
		boost::asio::ip::tcp::socket* c = 0;
		currenQueue.locker.lock();
		if (currenQueue.data.size())
		{
			c = *currenQueue.data.begin();
			currenQueue.data.pop_front();
		}
		currenQueue.locker.unlock();
		return c;
	}
	size_t size()
	{
		return currenQueue.size();
	}
	HTTPRequest* getOthers()
	{
		HTTPRequest* c = 0;
		keepconnectQueue.locker.lock();
		if (keepconnectQueue.data.size())
		{
			c = *(keepconnectQueue.data.begin());
			keepconnectQueue.data.pop_front();
		}
		keepconnectQueue.locker.unlock();
		return c;
	}
	~ConnectionQueue() {}
	Queue<boost::asio::ip::tcp::socket> currenQueue;
	Queue<HTTPRequest> keepconnectQueue;
};

