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
	std::string URL;
	std::string type;
	std::string originalURL;
	std::map<std::string,std::string> *heads=0;
	std::map<std::string, std::string> *parms = 0;
	size_t headLength=0;
	size_t bodyLength = 0;
	bool hasParm =0;
	bool isComplished = 0;
	HTTPRequest(boost::asio::ip::tcp::socket* socket, size_t bufferSize = 2048);
	void setHasParm(bool b = 0);
	bool is_open();
	void complished();
	void release();
	~HTTPRequest();
};

