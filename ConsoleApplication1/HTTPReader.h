#pragma once
#include "tool"
#include "ConnectionQueue.h"
#include "RequestQueue.h"
#include "HTTPRequest.h"
class HTTPReader
{
public:
	HTTPReader(ConnectionQueue &cqueue, RequestQueue &rqueue):cqueue(cqueue), rqueue(rqueue)
	{
	}
	~HTTPReader()
	{}
	void stop()
	{
		while (enable);
	}
	void start(bool *enable)
	{
		boost::asio::ip::tcp::socket* s;
		while (*enable==1)
		{
			s = cqueue.get();
			if (s == 0)
				goto end;
			if(!s->is_open())
				goto end;
			HTTPRequest *r = new HTTPRequest(s);
			handleRead(r);
			continue;
		end:
			boost::this_thread::sleep(boost::posix_time::microseconds(100));
			continue;
		other:
			HTTPRequest *Or = cqueue.getOthers();
			if (Or == 0)
				goto end;
			handleRead(Or);
			continue;
		}
		this->enable = 0;
	}
	bool handleRead(HTTPRequest*r)
	{
		if (r == 0)
			return 0;
		if (r->socket == 0)
			return 0;
		if (!r->socket->is_open())
		{
			r->release();
			delete r;
			return 0;
		}
		boost::system::error_code ec;
		if (readRequest(r,ec))
		{
			rqueue << r;
			return 1;
		}
		else if (ec.value())
			debug("readingERROR", ec.value() << ec.message());
		delete r;
		return 0;
	}
	
	bool readRequest(HTTPRequest*r,boost::system::error_code& ec, int timeout = 10)
	{
		r->buffer = new boost::asio::streambuf(2048);
		boost::asio::steady_timer timer(r->socket->get_io_context());
		timer.expires_from_now(std::chrono::nanoseconds(5000000000));
		timer.async_wait(boost::bind(&HTTPReader::readTimeOut,this, r, boost::asio::placeholders::error));
		r->headLength += boost::asio::read_until(*(r->socket), *(r->buffer), "\r\n\r\n", ec);
		if (r->headLength >= 2048)
		{
			timer.cancel();
			return 0;
		}
		if (check(r))
		{
			timer.cancel();
			if (complished(r,ec))
				return 1;
			return 0;
		}
		timer.cancel();
		return 0;
	}
	bool check(HTTPRequest*r)
	{
		std::istream is(r->buffer);
		is.read(r->data, r->headLength);
		if (r->data[r->headLength - 1] != '\n')
			return 0;
		if (r->data[r->headLength - 2] != '\r')
			return 0;
		if (r->data[r->headLength - 3] != '\n')
			return 0;
		if (r->data[r->headLength - 4] != '\r')
			return 0;
		return 1;
	}
	void readTimeOut(HTTPRequest*r,boost::system::error_code ec)
	{
		if (ec.value() == 0)
		{
			debug("readTimeOut", "close socket");
			r->release();
			return;
		}
	}
	bool complished(HTTPRequest*r,boost::system::error_code& ec)
	{
		if (r->headLength < 15)
			return 0;
		r->heads = new std::map<std::string, std::string>();
		std::vector<std::string> v;
		boost::split_regex(v, r->data, boost::regex(": |\r\n"));// boost::is_any_of(":\r\n"));
		if (boost::ends_with(v[0], "HTTP/1.1"))
		{
			size_t p = v[0].find(' ');
			Tool::substr(r->type, v[0], 0, p);
			Tool::substr(r->URL, v[0], p + 1, v[0].find(' ', p + 1));
			size_t urlsize = r->URL.find('?');
			debug(r->socket->remote_endpoint().address().to_string() << ":" << r->type, r->URL);
			if (urlsize < r->URL.size())
			{
				r->afterURL = r->URL.substr(urlsize + 1);
				Tool::substr(r->URL, v[0], 1 + p, p + 1 + urlsize);
				r->hasParm = 1;
			}
		}
		else {
			r->isComplished = 0;
			return 0;
		}
		for (int i = 1; i < v.size() - 1; ++i)
		{
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
					int j = Tool::stringToInt(v[i + 1]);
					if (j < 2048)
						if (!readBody(r,ec, j))
							return 0;
				}
				r->heads->insert(strstrPair(v[i], v[i + 1]));
				++i;
				continue;
			}
		}
		r->isComplished = 1;
		return r->isComplished;
	}
	bool readBody(HTTPRequest *r,boost::system::error_code& ec, size_t needread)
	{
		debug("try read body", r->body);
		size_t in_avail = r->buffer->in_avail();
		if (in_avail&&in_avail < 2048)
		{
			std::istream is(r->buffer);
			is.read(r->body, in_avail);
		}
		boost::system::error_code cec;
		boost::asio::steady_timer timer(r->socket->get_io_context());
		timer.expires_from_now(std::chrono::nanoseconds(5000000000));
		timer.async_wait(boost::bind(&HTTPReader::readTimeOut,this, r, boost::asio::placeholders::error));
		while (needread > in_avail)
		{
			in_avail += r->socket->read_some(boost::asio::buffer(r->body + in_avail, 2048 - in_avail), cec);
			if (cec)
			{
				ec = cec;
				return 1;
			}
		}
		r->hasParm = 1;
		return 1;
	}
	int enable=1;
	ConnectionQueue &cqueue;
	RequestQueue &rqueue;
};

