#include "HTTPResponse.h"
HTTPResponse::HTTPResponse(boost::asio::ip::tcp::socket* socket) :socket(socket)
{
	addHead("Server", "myServer");
	addHead("Date", Tool::getDate());
	body="Server Test";
	complish();
	
}
void HTTPResponse::complish()
{
	if (isComplished)
		return;
	if (body.size() == 0)
	{
		addHead("Content-Length", "14");
		addHead("Content-Type", "text/html");
		body="server is down";
		isComplished = 1;
		return;
	}
	if (body.size())
	{
		if (needCompress)
		{
			std::string buffer;
			body = Tool::getGZIP(buffer, body);
			addHead("Content-Encoding", "gzip");
		}
		addHead("Content-Length", std::to_string(body.size()));
		debug("HTTPResponse lenth", body.size());
	}
	
	for (auto i = heads.begin(); i != heads.end(); ++i)
	{
		printl((*i).first + ": " + (*i).second + "rn");
	}
	isComplished = 1;
}
HTTPResponse::HTTPResponse(HTTPRequest *req, bool needCompress) :socket(req->socket)
{
	
	auto r = req->heads->find("Connection");
	if (r != req->heads->end() && r->second == "keep-alive")
		keepalive = 1;
	debug("Connection", keepalive);
	addHead("Server", "myServer");
	addHead("Date", Tool::getDate());
}
void HTTPResponse::addHead(const char* tab, const char* value)
{
	string k(tab), v(value);
	auto i = heads.find(k);
	if (i != heads.end())
		heads.erase(i);
	heads.insert(strstrPair(k, v));
}
void HTTPResponse::addHead(std::string tab, std::string value)
{
	auto i = heads.find(tab);
	if (i != heads.end())
		heads.erase(i);
	heads.insert(strstrPair(tab, value));
}
void HTTPResponse::addBody(const std::string &body)
{
	if (isComplished)
		return;
	this->body+=body;
}
void HTTPResponse::addBody(CachedStaticFile* body)
{
	if (body == 0)
		return;
	if (isComplished)
		return;
	addHead("Content-Length", std::to_string(body->len));
	addHead("Content-Type", body->type);
	bodyFile = body;
	if (body->isCompress)
		addHead("Content-Encoding", "gzip");
	isComplished = 1;
}
void HTTPResponse::operator<<(const std::string &body)
{
	addBody(body);
}
#ifndef MYINTERFACE
void HTTPResponse::operator<<(CachedStaticFile *body)
{
	addBody(body);
}
void HTTPResponse::release(bool needDeleteSocket)
{
	if (socket != 0&& needDeleteSocket)
	{
		if (socket->is_open())
		{
			socket->close();
		}
		socket->release();
		delete socket;
		socket = 0;
	}
}
#endif 
HTTPResponse::~HTTPResponse()
{

	
}