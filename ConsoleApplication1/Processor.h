#pragma once
#include "RequestQueue.h"
#include "ResponseQueue.h"
#include "HTTPApplicationManager.h"
class Processor
{
	ResponseQueue &responseQueue;
	RequestQueue &requestQueue;
	HTTPApplicationManager &applicationManager;
public:
	Processor(HTTPApplicationManager &am,RequestQueue &req, ResponseQueue &res):
		requestQueue(req), responseQueue(res), applicationManager(am)
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
			debug("Processor", "");
			if (r->socket->is_open())
				debug("Processor", "socketOK");
			responseQueue << applicationManager.exec(r);
			continue;
		end:
			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}
		this->enable = 0;
		debug("stop", *enable);
	}
	~Processor()
	{
	}
	int enable = 1;
};

