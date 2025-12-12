#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <string>
#include "Server.hpp"
#define NICK_MAX 30

class Server;

class CommandHandler { // to-do make it more readable
	private:
		typedef void (*handlerFunc)(Server* server, Client *client, const IrcMessage &cmd);

		static std::map<std::string, handlerFunc> _handlers;

		static void _handlePass(Server* server, Client *client, const IrcMessage &cmd);
		static void _handleNick(Server* server, Client *client, const IrcMessage &cmd);
		static void _handleUser(Server* server, Client *client, const IrcMessage &cmd);
		static void _handleQuit(Server *server, Client *client, const IrcMessage &cmd);

		static bool _isNameDublicate(Server *server, std::string name, bool (*compareFunc)(Client*, const std::string&));
		static bool _compareNick(Client *client, const std::string &name);
		static bool _checkNickChars(const std::string &name);
		static bool _compareUser(Client *client, const std::string &name);

	public:
		CommandHandler(void);
		~CommandHandler(void);

		static void handleCmd(Server* server, Client* client, const IrcMessage& cmd); // client* here is the client that sent the command
		
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
