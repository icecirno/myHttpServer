#pragma once
#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS   
#include <boost/shared_ptr.hpp>
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "application.h"
class activity
{
public:
	virtual int onLoad(application*app) = 0;
	virtual void onUnload() = 0;
	virtual bool runActivity(HTTPRequest*r, HTTPResponse* result) = 0;
};