#pragma once
#include "tool"
#include "ConnectionQueue.h"
#include "RequestQueue.h"
#include "HTTPRequest.h"
#define bufferSize 2048

class HTTPReader
{
public:

	static void handleRead(boost::asio::ip::tcp::socket* s,bool needDeleteSocket=1)
	{
		if (s == 0)
			return;
		if (!s->is_open())
		{
			if (needDeleteSocket)
			{
				debug("Socket is closed when reading", "delete socket;");
				delete s;
			}else
				debug("Socket is closed when reading", "keep socket;");
			return;
		}
		HTTPRequest* r = new HTTPRequest(s, bufferSize);
		readRequest(r);
	}
	
	static void readRequest(HTTPRequest*r)
	{
		debug("readRequest", "");
		boost::asio::async_read_until(*(r->socket), *(r->buffer), "\r\n\r\n",
			boost::bind(&HTTPReader::check, r,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	static void check(HTTPRequest*r, boost::system::error_code ec, size_t bytes_transferred)
	{
		if (ec.value())
		{
			debug("readingERROR", ec.value() << ec.message());
			delete r;
			return;
		}
		size_t bodyStickySize = r->buffer->size()-bytes_transferred;
		debug("headBufferreallsize:", bodyStickySize+ bytes_transferred);
		debug("bytes_transferred", bytes_transferred);
		r->headLength = bytes_transferred;
		std::istream is(r->buffer);
		r->head->resize(bytes_transferred);
		is.read(&(*(r->head))[0], bytes_transferred);
		debug("headsize:", r->head->size());
		if (r->head->at(bytes_transferred - 1) != '\n')
			goto end;
		if (r->head->at(bytes_transferred - 2) != '\r')
			goto end;
		if (r->head->at(bytes_transferred - 3) != '\n')
			goto end;
		if (r->head->at(bytes_transferred - 4) != '\r')
			goto end;
		if (bodyStickySize>0)
		{
			r->body.resize(bodyStickySize);
			is.read(&(r->body)[0], bodyStickySize);
			r->bodyLength = bodyStickySize;
		}
		debug("headsize:", r->head->size());
		debug("budysize", r->body);
		complished(r);
		return;
	end:
		delete r;
		debug("readingERROR", "unknow");
		return;
	}
	static void complished(HTTPRequest*r)
	{
		if (r->headLength < 15)
		{
			delete r; 
			debug("readingERROR", "complish error");
			return;
		}
		r->heads = new std::map<std::string, std::string>();
		std::vector<std::string> v;
		boost::split_regex(v, *(r->head), boost::regex(": |\r\n"));// boost::is_any_of(":\r\n"));
		if (boost::ends_with(v[0], "HTTP/1.1"))
		{
			size_t p = v[0].find(' ');
			Tool::substr(r->type, v[0], 0, p);
			Tool::substr(r->originalURL, v[0], p + 1, v[0].find(' ', p + 1));
			size_t urlsize = r->originalURL.find('?');
			if (urlsize < r->originalURL.size())
			{
				string parm;
				parm = r->originalURL.substr(urlsize + 1);
				Tool::substr(r->URL, v[0], 1 + p, p + 1 + urlsize);
				r->setHasParm(1);
				std::vector<std::string> pv;
				boost::split_regex(pv, parm, boost::regex("&|="));// boost::is_any_of(":\r\n"));
				for (int i = 0; i < pv.size(); i += 2)
				{
					if (i + 1 < pv.size())
						r->parms->insert(strstrPair(pv[i], pv[i + 1]));
					else
						r->parms->insert(strstrPair("parm"+ to_string(i/2), pv[i]));
				}
			}
			else
				r->URL = r->originalURL;
			debug(r->socket->remote_endpoint().address().to_string() << ":" << r->type, r->originalURL<<" url "<<r->URL);
		}
		else {
			r->isComplished = 0;
			delete r;
			debug("readingERROR", "complish error");
			return ;
		}
		size_t bodySize=0;
		for (int i = 1; i < v.size() - 1; ++i)
		{
			debug(v[i], v[i + 1]);
			if (v[i] == "Host")
			{
				r->heads->insert(strstrPair(v[i], v[i + 1]));
				++i;
				continue;
			}
			if (v[i] == "Connection")
			{
				r->heads->insert(strstrPair(v[i], v[i + 1]));
				++i;
				continue;
			}
			if (v[i] == "Cache-Control")
			{

				r->heads->insert(strstrPair(v[i], v[i + 1]));
				++i;
				continue;
			}
			if (v[i] == "Upgrade-Insecure-Requests")
			{
				r->heads->insert(strstrPair(v[i], v[i + 1]));
				++i;
				continue;
			}
			if (v[i] == "User-Agent")
			{
				r->heads->insert(strstrPair(v[i], v[i + 1]));
				++i;
				continue;
			}
			if (v[i] == "Accept")
			{
				r->heads->insert(strstrPair(v[i], v[i + 1]));
				++i;
				continue;
			}
			if (v[i] == "Accept-Encoding")
			{
				r->heads->insert(strstrPair(v[i], v[i + 1]));
				++i;
				continue;
			}
			if (v[i] == "Cookie")
			{
				r->heads->insert(strstrPair(v[i], v[i + 1]));
				++i;
				continue;
			}
			if (v[i] == "Content-Type")
			{
				r->heads->insert(strstrPair(v[i], v[i + 1]));
				++i;
				continue;
			}
			if (v[i] == "Content-Length")
			{
				if (r->type == "POST")
				{
					bodySize = Tool::stringToInt(v[i + 1])- r->bodyLength;
				}
				r->heads->insert(strstrPair(v[i], v[i + 1]));
				++i;
				continue;
			}
		}
		
		if (bodySize>0)
		{			
			if (bodySize < bufferSize)
				readBody(r, bodySize);
			return;
		}
		debug("read finished 1", "");
		r->complished();
		(*rqueue) << r;
	}
	static void readBody(HTTPRequest *r, size_t needread)
	{
		debug("try read body", needread);
		r->bodybuffer = new boost::asio::streambuf(needread);
		boost::asio::async_read(*(r->socket), *(r->bodybuffer), boost::bind(&HTTPReader::finishRead, r,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
		return ;
	}
	static void finishRead(HTTPRequest *r, boost::system::error_code ec, size_t bytes_transferred)
	{
		if (ec.value())
		{
			debug("readingERROR", ec.value() << ec.message());
			delete r;
			return;
		}
		std::istream is(r->bodybuffer);
		size_t size= r->body.size();
		r->body.resize(size+ bytes_transferred);
		is.read(&(r->body)[size], bytes_transferred);
		r->bodyLength = size + bytes_transferred;
		debug("read finished 2", r->body<< bytes_transferred);
		r->complished();
		(*rqueue) << r;
	}
	int enable=1;
	static RequestQueue *rqueue;
};
