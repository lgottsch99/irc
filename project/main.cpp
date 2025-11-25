#include "headers/Server.hpp"

/* TODOs
Server:
	creating listening socket
	polling
	signal handling ctrl d, ctrl c


irc interpretation


*/

int main(int argc, char *argv[])
{
	(void)argv;
	if (argc != 3)
	{		
		std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	try
	{
		Server irc_server(argv);
		
		irc_server.start();

	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return 0;
}