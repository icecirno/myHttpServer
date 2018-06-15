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
			rqueue << readHead(s);
			continue;
		end:			
			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}
		this->enable = 0;
		debug("stop", *enable);
	}
	HTTPRequest* readHead(boost::asio::ip::tcp::socket* socket)
	{
		int readed = 0,len=0;
		boost::system::error_code ec;
		HTTPRequest *r = new HTTPRequest(socket);
		boost::asio::steady_timer timer(socket->get_io_service());
		timer.expires_from_now(std::chrono::seconds(10));
		timer.async_wait(boost::bind(&HTTPReader::readTimeout, this, readed, socket));
		do
		{
			len = socket->read_some(boost::asio::buffer(r->data, 1024), ec);
			if (ec.value())
			{
				delete r;
				return 0;
			}
			if (readed >= 2)
			{
				delete r;
				return 0;
			}
		}
		while ((!check(r->data,len))&&len);
		readed += 1;
		timer.cancel();
		r->complished(len);
		debug("readall","ok");
		return r;
	}
	bool check(char*c,int len)
	{
		if(c[len - 1] != '\n')
			return 0;
		if (c[len - 2] != '\r')
			return 0;
		if (c[len - 3] != '\n')
			return 0;
		if (c[len - 4] != '\r')
			return 0;
		debug("checked", len);
		return 1;
	}
	void readTimeout(int &cancal, boost::asio::ip::tcp::socket* s)
	{
		if (cancal)
			return;
		debug("readTimeout", cancal);
		s->cancel();
		cancal = 2;
	}
	int enable=1;
	ConnectionQueue &cqueue;
	RequestQueue &rqueue;
};

