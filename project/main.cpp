#include "headers/Client.hpp"
#include "headers/CommandHandler.hpp"
#include <iostream>

int main() // main for testing the command handler
{
	int fd = 7;
	std::string ip = "0.0.0.0";
	
	CommandHandler handler;
	Client client(fd, ip);

	// PASS blapassword
	ParsedCommand cmd;
	cmd.name = "PASS";
	cmd.args.push_back("blapassword");
	handler.handleCmd(&client, cmd);
	std::cout << "Authenticated yay or nay? " << client.isAuthenticated() << "\n";

	// // NICK Yeva
	// cmd.name = "NICK";
	// cmd.args = {"Yeva"};
	// handler.handleCmd(&client, cmd);
	// std::cout << "Nickname: " << client.getNickname() << "\n";

	// // USER yevauser 0.0.0.0 ircserver :Yeva Real
	// cmd.name = "USER";
	// cmd.args = {"yevauser", "0.0.0.0", "ircserver", "Yeva for real"};
	// handler.handleCmd(&client, cmd);
	// std::cout << "Username: " << client.getUsername() << "\n";
	// std::cout << "Realname: " << client.getRealname() << "\n";
	// std::cout << "Registered? " << client.isRegistered() << "\n";
	Server irc_server;
	
	try
	{	
		std::cout << "init server\n";
		irc_server.init(argv);
		irc_server.pollLoop();

	return 0;
}

// int main(int argc, char *argv[])
// {
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
