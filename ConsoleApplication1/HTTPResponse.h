#pragma once
#include "tool"
#include "CachedStaticFile.h"
#include "HTTPRequest.h"
#include "HTTPReader.h"
class HTTPResponse
{
public:
	int httpStatus=200;
	std::map<std::string,std::string> heads;
	std::string body;
	CachedStaticFile* bodyFile = 0;
	boost::asio::ip::tcp::socket* socket = 0;
	bool keepalive = 0;
	bool isComplished = 0;
	bool isCloseSocket = 1;
	bool needDeleteSocketAfterClosed = 1;
	bool needCompress=0;
	bool isChunked = 0;
	HTTPResponse(boost::asio::ip::tcp::socket* socket);
	void complish();
	HTTPResponse(HTTPRequest *req, bool needCompress = 0);
	void addHead(const char* tab, const char* value);
	void addHead(std::string tab, std::string value);
	void addBody(const std::string &body);
	void addBody(CachedStaticFile* body);
	void operator<<(const std::string &body);
#ifndef MYINTERFACE
	void operator<<(CachedStaticFile *body);
	void release(bool needDeleteSocket = 0);
#endif
	~HTTPResponse();
};

