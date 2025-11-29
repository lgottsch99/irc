#include "headers/Server.hpp"

/* TODOs
Server:
	check empty strings input & num validation
	new client auth + pw
	msg and full line extraction

	signal handling ctrl d, ctrl c
	error handling

irc interpretation
	interal logic of handling cmds
	creating + tracking of users, channels, modes etc

*/

/* QUESTIONS?
how to send msg back to client? -> send() lol

what could good class design for internal logic be?

*/

int main(int argc, char *argv[])
{
	(void)argv;
	if (argc != 3)
	{
		std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	
	Server irc_server;
	
	try
	{	
		std::cout <<"starting server\n";
		irc_server.startup(argv);
		std::cout <<"starting polling\n";
		irc_server.poll_loop(); //can be put into startup() ft later

	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		irc_server.server_shutdown();
	}
	return 0;
}