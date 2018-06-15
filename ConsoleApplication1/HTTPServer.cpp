#include "HTTPServer.h"
void HTTPServer::commandLine()
{
	while (1)
	{
		std::string comd;
		std::cin >> comd;

		if (comd == "exit")
		{
			enable = 0;
			for (int i = readers.size() - 1; i >= 0; --i)
			{
				debug("fuck reader", i);
				readers[i]->stop();
				debug("fucked readers", i);
				delete readers[i];
			}
			for (int i = processor.size() - 1; i >= 0; --i)
			{
				debug("fuck processor", i);
				processor[i]->stop();
				debug("fucked processor", i);
				delete processor[i];
			}
			for (int i = senders.size() - 1; i >= 0; --i)
			{
				debug("fuck processor", i);
				senders[i]->stop();
				debug("fucked processor", i);
				delete senders[i];
			}
			connections.clear();
			if (listener.isListening())
				listener.stop();
			return;
		}
		else if (comd == "size")
		{
			debug("connection counts", connections.size());
		}
		else if (comd == "clear")
		{
			system("cls");
		}
		else
		{
			printl("unknow command");
		}
	}
}