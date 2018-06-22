#pragma once
#include "tool"
#include "CachedStaticFile.h"
#include "HTTPRequest.h"
class HTTPResponse
{
public:
	int httpStatus=200;
	std::map<std::string,std::string> *heads;
	std::vector<std::string> bodys;
	boost::asio::ip::tcp::socket* socket = 0;
	bool isComplished = 0;
	bool isCloseSocket = 1;
	bool needCompress=0;
	bool isChunked = 0;
	HTTPResponse(boost::asio::ip::tcp::socket* socket) :socket(socket)
	{
		heads = new std::map<std::string, std::string>();
		addHead("Server","myServer");
		addHead("Date", Tool::getDate());
		bodys.push_back("Server Test");
		complish();
	}
	void complish()
	{
		if (isComplished)
			return;
		if (bodys.size() == 0)
		{
			addHead("Content-Length", "14");
			addHead("Content-Type", "text/html");
			bodys.push_back("server is down");
			isComplished = 1;
			return;
		}
		if (bodys.size() == 1)
		{
			addHead("Content-Length", std::to_string(bodys[0].size()));
			debug("HTTPResponse lenth", bodys[0].size());
		}
		else {
			addHead("Transfer-Encoding", "chunked");
		}
		isComplished = 1;
		for (auto i = heads->begin(); i !=heads->end(); ++i)
		{
			printl((*i).first + ": " + (*i).second + "rn");
		}
	}
	HTTPResponse(HTTPRequest *req, bool needCompress=0):socket(req->socket)
	{
		heads = new std::map<std::string, std::string>();
		addHead("Server", "myServer");
	    addHead("Date", Tool::getDate());
	}
	void addHead(const char* tab, const char* value)
	{
		string k(tab), v(value);
		auto i = heads->find(k);
		if (i != heads->end())
			heads->erase(i);
		heads->insert(strstrPair(k, v));
	}
	void addHead(std::string tab, std::string value)
	{
		auto i = heads->find(tab);
		if (i != heads->end())
			heads->erase(i);
		heads->insert(strstrPair(tab, value));
	}
	void addBody(const std::string &body)
	{	
		if (needCompress)
		{
			std::string buffer;
			bodys.push_back(Tool::getGZIP(buffer, body));
			addHead("Content-Encoding", "gzip");
		}
		else
			bodys.push_back(body);
	}
	void addBody(CachedStaticFile* body)
	{
		if (body == 0)
			return;
		addHead("Content-Length", std::to_string(body->len));
		addHead("Content-Type", body->type);
		bodys.push_back(*body->data);
		if(body->isCompress)
			addHead("Content-Encoding", "gzip");
		isComplished = 1;
	}
	HTTPResponse* operator<<(const std::string &body)
	{
	}
	HTTPResponse* operator<<(CachedStaticFile *body)
	{
		addBody(body);
		return this;
	}
	~HTTPResponse()
	{
		if (socket != 0)
		{
			if(socket->is_open())
			{
				socket->close();
			}
			socket->release();
			delete socket;
		}
		delete heads;
	}
};

