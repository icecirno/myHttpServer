#include "HTTPServer.h"
#include "ArgumentHandle.h"
int main(int argc, char** argv)
{
	HTTPServer server(new ArgumentHandle());
	server.startlisten();
	return 0;
}