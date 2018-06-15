#include "tool"
#include "HTTPServer.h"
#include "ArgumentHandle.h"
int main(int argc, char** argv)
{
	try {
		HTTPServer server(new ArgumentHandle(argc, argv));
		server.startlisten();
	}
	catch (std::exception& e)
	{
		debug("error", e.what());
		while (getchar()!='|');
	}
	return 0;
}