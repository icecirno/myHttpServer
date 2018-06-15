#pragma once
#include "Queue.h"
#include "HTTPResponse.h"
class ResponseQueue :
	public Queue<HTTPResponse>
{
public:
	ResponseQueue();
	~ResponseQueue();
};

