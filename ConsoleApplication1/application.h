#pragma once
#include "HTTPRequest.h"
#include "HTTPResponse.h"
class application
{
public:
	virtual int onLoad() = 0;
	virtual void onUnload() = 0;
	virtual bool onFilterRequest(HTTPRequest&r) = 0;
	virtual bool run(HTTPRequest&r, HTTPResponse& result) = 0;
};