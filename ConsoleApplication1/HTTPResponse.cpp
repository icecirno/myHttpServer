#include "HTTPResponse.h"

HTTPResponse::HTTPResponse(boost::asio::ip::tcp::socket* socket) :socket(socket)
{
	addHead("Server", "myServer");
	addHead("Date", Tool::getDate());
	bodys.push_back("Server Test");
	complish();
}
void HTTPResponse::complish()
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
		int j = 0;
		for (auto i = bodys.begin(); i != bodys.end(); ++i)
		{
			j += i->size();
		}
		addHead("Content-Length", std::to_string(j));
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
	if (needCompress)
	{
		std::string buffer;
		bodys.push_back(Tool::getGZIP(buffer, body));
		addHead("Content-Encoding", "gzip");
	}
	else
		bodys.push_back(body);
}
void HTTPResponse::addBody(CachedStaticFile* body)
{
	if (body == 0)
		return;
	if (isComplished)
		return;
	addHead("Content-Length", std::to_string(body->len));
	addHead("Content-Type", body->type);
	bodys.push_back(*body->data);
	if (body->isCompress)
		addHead("Content-Encoding", "gzip");
	isComplished = 1;
}
HTTPResponse HTTPResponse::operator<<(const std::string &body)
{
	addBody(body);
	return *this;
}
#ifndef MYINTERFACE
HTTPResponse HTTPResponse::operator<<(CachedStaticFile *body)
{
	addBody(body);
	return *this;
}
#endif 
HTTPResponse::~HTTPResponse()
{
	
	if (socket != 0)
	{
		if(!keepalive)
		{
			if (socket->is_open())
			{
				socket->close();
			}
			socket->release();
			delete socket;
		}
	}

}