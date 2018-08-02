#pragma once
#include"tool"
class HTTPRequest
{	
	
public:
	boost::asio::ip::tcp::socket* socket=0;
	boost::asio::streambuf *buffer;
	boost::asio::streambuf *bodybuffer;
	string *head=0;
	string body;
	std::string afterURL;
	std::string type;
	std::string URL;
	std::map<std::string,std::string> *heads=0;
	size_t headLength=0;
	size_t bodyLength = 0;
	bool isComplished = 0;
	HTTPRequest(boost::asio::ip::tcp::socket* socket,size_t bufferSize=2048)
	{		
		this->socket = socket;
		head = new string();
		buffer = new boost::asio::streambuf(bufferSize);
	}
	bool is_open()
	{
		if (socket == 0)
			return 0;
		return socket->is_open();
	}
	void complished()
	{
		delete buffer;
		delete bodybuffer;
		delete head;
		isComplished = 1;
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
		if (isComplished)
			return;
		delete buffer;
		delete heads;
	}
	

};

