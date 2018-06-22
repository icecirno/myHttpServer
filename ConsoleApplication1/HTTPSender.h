#pragma once
#include "ConnectionQueue.h"
#include "ResponseQueue.h"

class HTTPSender
{
	ResponseQueue &responseQueue;
	ConnectionQueue &connectionQueue;
	int enable = 1;
public:
	HTTPSender(ResponseQueue &rq, ConnectionQueue &cq)
		:responseQueue(rq), connectionQueue(cq)
	{

	}
	void stop()
	{
		while (enable);
	}
	void start(bool *enable)
	{
		HTTPResponse* r;
		while (*enable == 1)
		{
			r = responseQueue.get();
			if (r == 0)
				goto end;
			if (!r->isComplished)
				r->complish();
			handleWrite(r);
			delete r;
			continue;
		end:
			boost::this_thread::sleep(boost::posix_time::microseconds(100));
		}
		this->enable = 0;
	}
	bool handleWrite (HTTPResponse* req)
	{
		std::string message;
		boost::system::error_code ec;
		if (req->socket->is_open())
		{
			switch (req->httpStatus)
			{
			case 200:
				message = "HTTP/1.0 200 OK\r\n";
				break;
			default:
				message = "HTTP/1.0 404 Not Found\r\n";
			}
			write(*req->socket, boost::asio::buffer(message, message.size()),ec);
			if (handleError(ec))
			{
				return 0;
			}
			auto j = req->heads->end();
			--j;
			for(auto i= req->heads->begin();i!= req->heads->end();++i)
			{ 
				if(i==j)
				message = (*i).first +": " +(*i).second+"\r\n\r\n";
				else
					message = (*i).first + ": " + (*i).second + "\r\n";
				write(*req->socket, boost::asio::buffer(message, message.size()), ec);
				if (handleError(ec))
				{
					return 0;
				}
			}
			for (auto i = req->bodys.begin(); i != req->bodys.end(); ++i)
			{
				write(*req->socket, boost::asio::buffer((*i), (*i).size()), ec);
				if (handleError(ec))
				{
					return 0;
				}
			}
		}
	}
	bool handleError(boost::system::error_code &ec)
	{
		return ec.value();
	}
	~HTTPSender() {}
};

