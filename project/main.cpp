#include "headers/Server.hpp"

/* TODOs
Server:
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
		Server irc_server;		
		irc_server.startup(argv);

		irc_server.poll_loop(); //can be put into startup() ft later

	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		//irc_server.server_shutdown();
	}
	return 0;
}