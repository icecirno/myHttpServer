#pragma once
#include"tool"
class HTTPRequest
{	
public:
	boost::asio::ip::tcp::socket* socket=0;
	char data[1024];
	int lenth=0;
	bool isComplished = 0;
	HTTPRequest(boost::asio::ip::tcp::socket* socket)
	{
		this->socket = socket;
	}
	~HTTPRequest()
	{
	}
	void complished(int len)
	{
		lenth = len;
		string s(data, len);
		std::vector<std::string> heads;
		std::vector<std::string> head;
		boost::split(heads, s, boost::is_any_of("[\r\n]"));
		for(int i=heads.size()-1;i>=0;--i)
			if (heads[i].size())
			{
				boost::split(head, heads[i], boost::is_any_of("[:]"));
				for (int i = head.size() - 1; i >= 0; --i)
				{
				}
				head.clear();
			}
		isComplished = 1;
	}
	int needReadBody()
	{
		return 0;
	}
	void setBody(boost::asio::streambuf&buffer)
	{

	}

};

