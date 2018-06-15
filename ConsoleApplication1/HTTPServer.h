#pragma once
#include "tool"
#include "ArgumentHandle.h"
#include "ConnectionQueue.h"
#include "ResponseQueue.h"
#include "RequestQueue.h"
#include "TCPListener.h"
#include "Processor.h"
#include "HTTPReader.h"
#include "HTTPApplicationManager.h"
#include "HTTPSender.h"
class HTTPServer
{
public:
	HTTPServer(ArgumentHandle*ah) :
		listener(this,ah), connections(), applicationManager(ah)
	{
		boost::thread(boost::bind(&HTTPServer::commandLine, this));
		for (int i = 0; i < ah->readerCounts; ++i)
		{
			HTTPReader *r = new HTTPReader(connections, request);
			readers.push_back(r);
			boost::thread(boost::bind(&HTTPReader::start, r, &enable));
		}
		for (int i = 0; i < ah->readerCounts; ++i)
		{
			Processor *r = new Processor(applicationManager,request, response);
			processor.push_back(r);
			boost::thread(boost::bind(&Processor::start, r, &enable));
		}
		for (int i = 0; i < ah->senderCounts; ++i)
		{
			HTTPSender *r = new HTTPSender(response, connections);
			senders.push_back(r);
			boost::thread(boost::bind(&HTTPSender::start, r, &enable));
		}
		
	}
	size_t startlisten()
	{
		listener.setCallBack([](HTTPServer *server, boost::asio::ip::tcp::socket *s)
		{
			server->connections << s;
			debug("getConnection", s->remote_endpoint().address().to_string());
		});
		return listener.startlisten();
	}
	~HTTPServer()
	{
	}
	void commandLine();
	bool enable=1;
	HTTPApplicationManager applicationManager;
	std::vector<HTTPReader*> readers;
	std::vector<Processor*> processor;
	std::vector<HTTPSender*> senders;
	TCPListener listener;
	RequestQueue request;
	ResponseQueue response;
	ConnectionQueue connections;
};
