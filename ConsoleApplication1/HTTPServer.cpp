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
			applicationManager->unloadALLDll();
			if (listener.isListening())
				listener.stop();
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
			
			return;
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