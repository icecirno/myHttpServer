#include "HTTPRequest.h"
HTTPRequest::HTTPRequest(boost::asio::ip::tcp::socket* socket, size_t bufferSize)
{

	this->socket = socket;
	head = new string();
	buffer = new boost::asio::streambuf(bufferSize);
}
bool HTTPRequest::is_open()
{
	if (socket == 0)
		return 0;
	return socket->is_open();
}
void HTTPRequest::complished()
{
	// 		for (auto i = heads->begin(); i != heads->end(); ++i)
	// 		{
	// 			debug(i->first, i->second);
	// 		}
	delete buffer;
	buffer = 0;
	delete bodybuffer;
	bodybuffer = 0;
	delete head;
	head = 0;
	isComplished = 1;
}
void HTTPRequest::setHasParm(bool b)
{
	if (b)
	{
		parms = new std::map<std::string, std::string>();
	}
	hasParm = b;
}
void HTTPRequest::release()
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
	socket = 0;
}
HTTPRequest::~HTTPRequest()
{
	delete heads;
	if (parms)
		delete parms;
	
	delete buffer;
	delete head;
	delete bodybuffer;
	if (isComplished)
		return;
	if (socket != 0)
	{
		if (socket->is_open())
		{
			socket->cancel();
			socket->close();
			delete socket;
		}
	}
}