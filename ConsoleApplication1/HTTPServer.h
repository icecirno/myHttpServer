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
		listener(this,ah), connections()
	{
		applicationManager = HTTPApplicationManager::getInstance(ah);
		HTTPReader::rqueue = &request;
		boost::thread(boost::bind(&HTTPServer::commandLine, this));
		for (int i = 0; i < ah->readerCounts; ++i)
		{
			Processor *r = new Processor(request, response);
			debug("new Processor", "");
			processor.push_back(r);
			boost::thread(boost::bind(&Processor::start, r, &enable));
		}
		for (int i = 0; i < ah->senderCounts; ++i)
		{
			HTTPSender *r = new HTTPSender(response, connections);
			debug("new HTTPSender", "");
			senders.push_back(r);
			boost::thread(boost::bind(&HTTPSender::start, r, &enable));
		}
		
	}
	void startlisten()
	{
		return listener.startlisten();
	}
	~HTTPServer()
	{
		delete applicationManager;
	}
	void commandLine();
	bool enable=1;
	HTTPApplicationManager *applicationManager=0;
	std::vector<Processor*> processor;
	std::vector<HTTPSender*> senders;
	TCPListener listener;
	RequestQueue request;
	ResponseQueue response;
	ConnectionQueue connections;
};

