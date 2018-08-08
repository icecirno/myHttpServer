#pragma once
#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS   
#include <boost/shared_ptr.hpp>
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "application.h"
class activity;
typedef bool(*runPointer)(HTTPRequest *, HTTPResponse *, activity*);
#define addRouter(ROUTER,...) bool hasRouter() { return 1; }runPointer getFunction(string url){ROUTER __VA_ARGS__ return 0;}
#define router(X,Y) if (url == X)return Y;
class activity
{
public:
	virtual runPointer getFunction(string url) { return 0; }
	virtual bool hasRouter(){ return 0; }
	virtual int onLoad(application*app) = 0;
	virtual void onUnload() = 0;
};