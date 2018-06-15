#pragma once
#include "Queue.h"
#include "HTTPRequest.h"
class RequestQueue :
	public Queue<HTTPRequest>
{
public:
	RequestQueue& operator<<(HTTPRequest* c)
	{
		if (c == 0)return *this;
		locker.lock();
		data.push_back(c);
		locker.unlock();
	}
	RequestQueue()
	{
	}
	~RequestQueue() {}
};

