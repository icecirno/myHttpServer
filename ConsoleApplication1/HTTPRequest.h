#pragma once
#include"tool"
class HTTPRequest
{	
	
public:
	bool hasParm = 0;
	boost::asio::ip::tcp::socket* socket=0;
	boost::asio::streambuf *buffer;
	char data[2049] = {0};
	char body[2049] = {0};
	std::string afterURL;
	std::string type;
	std::string URL;
	std::map<std::string,std::string> *heads=0;
	size_t headLength=0;
	bool isComplished = 0;
	HTTPRequest(boost::asio::ip::tcp::socket* socket)
	{		
		this->socket = socket;
		buffer = new boost::asio::streambuf(2048);
	}
	bool is_open()
	{
		if (socket == 0)
			return 0;
		return socket->is_open();
	}
	
	void release()
	{
		if (socket == 0)
			return;
		if (socket->is_open())
		{
			socket->cancel(); 
			socket->close();
		}
		socket->release();
		delete socket;
	}
	~HTTPRequest()
	{
		delete buffer;
		delete heads;
	}
	

};

