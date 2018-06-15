#pragma once
#include "tool"
#include "ArgumentHandle.h"
class HTTPServer;
typedef void(*AcceptCallBack)(HTTPServer *,boost::asio::ip::tcp::socket *);
class TCPListener 
{
	
public:
	TCPListener(HTTPServer* server, ArgumentHandle*ah) :ioService(),acceptor(ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), ah->serverPort))
	{
		this->server = server;
		port = ah->serverPort;
	}
	void setCallBack(AcceptCallBack cb)
	{
		boost::asio::ip::tcp::socket *s = new boost::asio::ip::tcp::socket(ioService);
		acceptor.async_accept(*s, boost::bind(&TCPListener::handleAccept, this, cb, s));
	}
	size_t startlisten()
	{
		enable = 1;
		debug("startlisten", "");
		return ioService.run();
	}
	void stop()
	{
		enable = 0;
		ioService.stop();
		debug("Tcplistener", "stoped");
	}
	void handleAccept(AcceptCallBack cb, boost::asio::ip::tcp::socket *s)
	{
		cb(server, s);
		setCallBack(cb);
	}
	bool isListening()
	{
		return enable;
	}
private:
	int port = 80;
	bool enable=0;
	boost::asio::io_service ioService;
	HTTPServer* server;
	boost::asio::ip::tcp::acceptor acceptor;
	
};