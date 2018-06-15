#pragma once
#include "tool"
#include"HTTPRequest.h"
#include"HTTPResponse.h"
#include "ArgumentHandle.h"
class HTTPApplicationManager
{
public:
	HTTPApplicationManager(ArgumentHandle* ah)
	{}
	~HTTPApplicationManager()
	{}
	HTTPResponse *exec(HTTPRequest*r)
	{
		if (r == 0)
			return new HTTPResponse(r->socket);
		return new HTTPResponse(r->socket);
		debug("HTTPApplicationManager","exec");
	}
};

