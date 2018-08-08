#pragma once
#include "tool"
#include "ArgumentHandle.h"
#include "IOServerPool.h"
#include "HTTPReader.h"
class HTTPServer;
typedef void(*AcceptCallBack)(HTTPServer *,boost::asio::ip::tcp::socket *);
class TCPListener 
{
	
public:
	TCPListener(HTTPServer* server, ArgumentHandle*ah){
		this->server = server;
		port = ah->serverPort;
		ioServerPool = IOServerPool::getInstance(ah->processorCounts);
		ioService = new boost::asio::io_service();
		this->acceptor = new boost::asio::ip::tcp::acceptor(*ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), ah->serverPort));
	}
	~TCPListener() 
	{
		delete acceptor;
		delete ioServerPool;
	}
	void keepListen()
	{
		boost::asio::ip::tcp::socket *s = new boost::asio::ip::tcp::socket(*(ioServerPool->getService()));
		acceptor->async_accept(*s, boost::bind(&TCPListener::handleAccept, this, s, boost::asio::placeholders::error));
	}
	void startlisten()
	{
		enable = 1;
		boost::asio::ip::tcp::socket *s = new boost::asio::ip::tcp::socket(*(ioServerPool->getService()));
		acceptor->async_accept(*s, boost::bind(&TCPListener::handleAccept, this, s, boost::asio::placeholders::error));
		debug("start listen at", port);
		ioService->run(); 
		return;
	}
	void stop()
	{
		enable = 0;
		ioServerPool->stopAll();
		ioService->stop();
		debug("Tcplistener", "stoped");
	}
	void handleAccept(boost::asio::ip::tcp::socket *s, boost::system::error_code ec)
	{
		if (ec.value())
		{
			debug("AcceptingERROR", ec.value() << ec.message());
			return;
		}
		debug("Tcplistener", "get new connection");
		HTTPReader::handleRead(s);
		if(enable)
			keepListen();
	}
	bool isListening()
	{
		return enable;
	}
	boost::asio::io_service *ioService=0;
private:
	int port = 80;
	bool enable=0;
	
	HTTPServer* server;
	IOServerPool *ioServerPool=0;
	boost::asio::ip::tcp::acceptor *acceptor;
	
};