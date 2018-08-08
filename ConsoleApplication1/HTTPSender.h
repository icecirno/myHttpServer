#pragma once
#include "ConnectionQueue.h"
#include "ResponseQueue.h"

class HTTPSender
{
	ResponseQueue &responseQueue;
	int enable = 1;
public:
	HTTPSender(ResponseQueue &rq)
		:responseQueue(rq)
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
			if (handleWrite(r))
			{
				debug("handleWrite", "finished");
				if (r->keepalive)
				{
					debug("handleRead", "keepalive");
					HTTPReader::handleRead(r->socket, r->needDeleteSocketAfterClosed);
					r->release(0);
				}else
					r->release(1);
				delete r;
				continue;
			}
			r->release(1);
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
			auto j = req->heads.end();
			--j;
			for(auto i= req->heads.begin();i!= req->heads.end();++i)
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
			if(req->bodyFile)
				write(*req->socket, boost::asio::buffer(*req->bodyFile->data, req->bodyFile->len), ec);
			write(*req->socket, boost::asio::buffer(req->body, req->body.size()), ec);
			if (handleError(ec))
			{
				return 0;
			}
			return 1;
		}
		debug("Socket is closed before weite", "");
		return 0;
	}
	bool handleError(boost::system::error_code &ec)
	{
		if (ec.value())
		{
			debug("get error when sending data:",ec.value()<< ec.message());
			return 1;
		}
		return 0;
	}
	~HTTPSender() {}
};

