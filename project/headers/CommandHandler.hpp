#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Server.hpp"

#define NICK_MAX 30

#define DISABLE 0
#define ENABLE 1
#define INVALID 2

/* TO-DO
	1. JOIN #foo,#bar fubar,foobar
	2. validate a name of a channel: #, &
	3. handle multiple modes in one command line
	4. implement NAMES command here
	5. add sendToChannel()
*/

class Server;

class CommandHandler {
	private:
		Server*		_server;
		Client*		_client;
		IrcMessage	_cmd;

		typedef void (CommandHandler::*handlerFunc)(void);
		typedef void (CommandHandler::*modeFunc)(Channel *channel, int sign);

		std::map<std::string, handlerFunc>	_handlers;
        std::map<char, modeFunc>			_modes;

		void _handlePass();
		void _handleNick();
		void _handleUser();
		void _handleQuit();
		void _handleJoin();
		void _handleMode();
		void _handleTopic();
		void _handleInvite();
		void _handleKick();

		void _modeInvite(Channel *channel, int signIsPositive);
		void _modeKey(Channel *channel, int signIsPositive);
		void _modeLimit(Channel *channel, int signIsPositive);
		void _modeTopic(Channel *channel, int signIsPositive);
		void _modeOperator(Channel *channel, int signIsPositive);

		void _init_modes();
		bool _isNameDublicate(Server *server, std::string name, bool (CommandHandler::*compareFunc)(Client*, const std::string&));
		bool _compareNick(Client *client, const std::string &name);
		bool _compareUser(Client *client, const std::string &name);
		bool _checkNickChars(const std::string &name);
		int _identifySign(char sign);

		bool _tryRegister();
		void _sendToAllChannels(const std::string& trailing);

	public:
		CommandHandler();
		CommandHandler(Server* server, Client* client, const IrcMessage &cmd);
		~CommandHandler();

		void handleCmd(); // client* here is the client that sent the command
		
		// void sendToChannel(const std::string& channelName, const std::string& msg, Client* exceptClient);

		// void sendToClient(int fd, const std::string& msg); // internally forwards the msg to server:  server->send_response(fd, msg);
};

/* sends msg to multiple clients, internally: 
	1. lookup channel members 
	2. iterate over users and sendToClient(user->fd, msg) for each (except if client is marked as except (aka the sender of the msg))
*/

#endif
