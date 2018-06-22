#pragma once
#include "RequestQueue.h"
#include "ResponseQueue.h"
#include "HTTPApplicationManager.h"
class Processor
{
	ResponseQueue &responseQueue;
	RequestQueue &requestQueue;
public:
	Processor(RequestQueue &req, ResponseQueue &res):
		requestQueue(req), responseQueue(res)
	{
	}
	void stop()
	{
		while (enable);
	}
	void start(bool *enable)
	{
		HTTPRequest* r;
		while (*enable == 1)
		{
			r = requestQueue.get();
			
			if (r == 0)
				goto end;
			if (r->socket->is_open())
			responseQueue << HTTPApplicationManager::exec(r);
			continue;
		end:
			boost::this_thread::sleep(boost::posix_time::microseconds(100));
		}
		this->enable = 0;
		debug("stop", *enable);
	}
	~Processor()
	{
	}
	int enable = 1;
};

