#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <string>
// #include "Server.hpp"
#include "ParsedCommand.hpp"
#include "Client.hpp"

class Server;
class Client;

class CommandHandler {
	private:
		Server* _server;

		// std::map<std::string, handlerFunc> handlers;

		void handlePass(Client *client, const ParsedCommand &cmd);
		void handleNick(Client *client, const ParsedCommand &cmd);
		void handleUser(Client *client, const ParsedCommand &cmd);

	public:
		CommandHandler();
		CommandHandler(Server* server);
		~CommandHandler();

		void handleCmd(Client* client, const ParsedCommand& cmd); // client* here is the client that sent the command
		
		// void sendToChannel(const std::string& channelName, const std::string& msg, Client* exceptClient);

		// void sendToClient(int fd, const std::string& msg); // internally forwards the msg to server:  server->send_response(fd, msg);
};

/* sends msg to multiple clients, internally: 
	1. lookup channel members 
	2. iterate over users and sendToClient(user->fd, msg) for each (except if client is marked as except (aka the sender of the msg))
*/

/*
Commands that affect clients:
	NICK
	USER
	PASS
	QUIT
Commands that affect channels:
	JOIN
	TOPIC
	MODE
	INVITE
	KICK
*/

#endif
