#include "headers/Client.hpp"
#include "headers/CommandHandler.hpp"
#include "headers/Parser.hpp"

int main() // main for testing the command handler
{
	Client *client = new Client;
	Client *client2 = new Client;
	Server server;

	server.setPass("password");
	std::cout << "Password: " << server.getPassword() << std::endl;
	server.Clients.insert(std::make_pair(10, client));
	server.Clients.insert(std::make_pair(11, client2));

	// PASS blapassword
	ParsedCommand cmd;
	cmd.name = "PASS";
	cmd.args.push_back("password");

	CommandHandler::handleCmd(&server, client, cmd);
	std::cout << "Authenticated yay or nay? " << client->isAuthenticated() << "\n";

	// NICK Yeva
	cmd.name = "NICK";
	cmd.args.clear();
	cmd.args.push_back("Yeva");
	CommandHandler::handleCmd(&server, client, cmd);
	std::cout << "Nickname: " << client->getNickname() << "\n";
	CommandHandler::handleCmd(&server, client2, cmd);
	std::cout << "Nickname: " << client2->getNickname() << "\n";

	// USER yevauser 0.0.0.0 ircserver :Yeva Real
	cmd.name = "USER";
	cmd.args.push_back("yevauser");
	cmd.args.push_back("Yeva for real");
	CommandHandler::handleCmd(&server, client, cmd);
	std::cout << "Username: " << client->getUsername() << "\n";
	std::cout << "Realname: " << client->getRealname() << "\n";
	std::cout << "Registered? " << client->isRegistered() << "\n";
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
// 		std::cout << "init server\n";
// 		irc_server.init(argv);
// 		irc_server.pollLoop();
// 	}
// 	catch (const std::exception &e)

// int main()
// {
// 	IrcMessage message;
// 	message = Parser::parseLine("AAaf :Hello ");
// 	printIrcMessage(message);
// }
