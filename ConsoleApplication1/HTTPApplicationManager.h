#pragma once
#include "tool"
#include"HTTPRequest.h"
#include"HTTPResponse.h"
#include "ArgumentHandle.h"
#include "StaticFileManager.h"
#include "CachedStaticFile.h"



class HTTPApplicationManager
{
	StaticFileManager files; 
	HTTPApplicationManager(ArgumentHandle* ah) :files(ah)
	{}
	
public:
	
	~HTTPApplicationManager()
	{}
	static HTTPResponse *exec(HTTPRequest*r);
	static void handleResponse(HTTPResponse *respones, HTTPRequest*request, StaticFileManager& files);
	static HTTPApplicationManager *instance;
	static HTTPApplicationManager *getInstance(ArgumentHandle* ah);
};

