#pragma once
#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS   
#include <boost/shared_ptr.hpp>
#include "HTTPRequest.h"
#include "HTTPResponse.h"
class application
{
public:
	virtual int onLoad() = 0;
	virtual void onUnload() = 0;
};