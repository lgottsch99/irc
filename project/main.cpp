#include "headers/Parser.hpp"

/* the final main function could look somewhat like this

*/

// int main(int argc, char *argv[])
// {
// 	(void)argv;
// 	if (argc != 3)
// 	{
// 		std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
// 		return 1;
// 	}
	
// 	Server irc_server;
	
// 	try
// 	{	
// 		std::cout <<"starting server\n";
// 		irc_server.start(argv);
// 		std::cout <<"starting polling\n";
// 		irc_server.pollLoop(); //can be put into startup() ft later

// 	}
// 	catch(const std::exception& e)
// 	{
// 		std::cerr << e.what() << '\n';
// 		irc_server.shutdown();
// 	}
// 	return 0;
// }

int main()
{
	IrcMessage message;
	message = Parser::parseLine("AAaf :Hello ");
	printIrcMessage(message);
}