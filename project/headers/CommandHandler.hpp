#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <string>
#include "Server.hpp"
#define NICK_MAX 30

#define DISABLE 0
#define ENABLE 1
#define INVALID 2

/* TO-DO
	1. JOIN #foo,#bar fubar,foobar
	2. validate a name of a channel: #, &, +
	3. merge nickname validation
	4. format messages correctly for the replyToClient
	5. actually send responses to the server
	6. implement commands: TOPIC, INVITE, KICK
	7. test MODE command
*/

class Server;

class CommandHandler {
	private:
		typedef void (*handlerFunc)(Server* server, Client *client, const IrcMessage &cmd);
		typedef void (*modeFunc)(const IrcMessage &cmd, Channel *channel, int sign);

		static std::map<std::string, handlerFunc> _handlers;

		// static std::map<char, > _modes;

		static void _handlePass(Server* server, Client *client, const IrcMessage &cmd);
		static void _handleNick(Server* server, Client *client, const IrcMessage &cmd);
		static void _handleUser(Server* server, Client *client, const IrcMessage &cmd);
		static void _handleQuit(Server *server, Client *client, const IrcMessage &cmd);
		// static void _handleOper(Server *server, Client *client, const IrcMessage &cmd);

		static void _handleJoin(Server *server, Client *client, const IrcMessage &cmd);
		static void _handleMode(Server *server, Client *client, const IrcMessage &cmd);
		static void _handleTopic(Server *server, Client *client, const IrcMessage &cmd);
		static void _handleInvite(Server *server, Client *client, const IrcMessage &cmd);
		static void _handleKick(Server *server, Client *client, const IrcMessage &cmd);

		static bool _isNameDublicate(Server *server, std::string name, bool (*compareFunc)(Client*, const std::string&));
		static bool _compareNick(Client *client, const std::string &name);
		static bool _checkNickChars(const std::string &name);
		static bool _compareUser(Client *client, const std::string &name);
		// static void	_handleModesUtil(const std::vector<std::string> &params, Channel *channel);

		static void _modeInvite(const IrcMessage &cmd, Channel*, int);
		static void _modeKey(const IrcMessage &cmd, Channel*, int);
		static void _modeLimit(const IrcMessage &cmd, Channel*, int);

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

#endif
