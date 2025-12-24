#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include "Server.hpp"
#include <climits>

#define NICK_MAX 30

#define POSITIVE 1
#define NEGATIVE 0

/* TO-DO
	1. JOIN #foo,#bar fubar,foobar - split
	2. validate a name of a channel: #, &
	3. handle multiple modes in one command line
	4. implement ping, pong, names, notice, privmsg
	5. add sendToChannel()
*/

class Server;

class CommandHandler	{
	private:
		Server *_server;
		Client *_client;
		IrcMessage _cmd;

		typedef struct s_mode
		{
			char sign;			  // '+' or '-'
			char mode;			  // 'i', 't', 'k', 'o', 'l'
			std::string arg; // empty if not required
		} t_mode;

		typedef std::vector<t_mode> t_mode_vect;

		typedef void (CommandHandler::*handlerFunc)(void);
		typedef void (CommandHandler::*modeFunc)(Channel *channel, const t_mode &mode);

		std::map<std::string, handlerFunc> _handlers;
		std::map<char, modeFunc> _modes;

		void _handlePass();
		void _handleNick();
		void _handleUser();
		void _handleQuit();
		void _handleJoin();
		void _handleMode();
		void _handleTopic();
		void _handleInvite();
		void _handleKick();
		void _handlePing();
		void _handlePong();
		void _handleNotice();
		void _handlePrivmsg();

		void _modeInvite(Channel *channel, const t_mode &mode);
		void _modeKey(Channel *channel, const t_mode &mode);
		void _modeLimit(Channel *channel, const t_mode &mode);
		void _modeTopic(Channel *channel, const t_mode &mode);
		void _modeOperator(Channel *channel, const t_mode &mode);

		void _init_modes();
		// t_mode _parseMode(const IrcMessage &_cmd);
		t_mode_vect _parseMode(const IrcMessage &_cmd);
		bool _modeNeedsArg(char mode, char sign);

		bool _isNameDublicate(Server *server, std::string name, bool (CommandHandler::*compareFunc)(Client *, const std::string &));
		bool _compareNick(Client *client, const std::string &name);
		static bool _checkNickChars(const std::string &name);
		bool _compareUser(Client *client, const std::string &name);
		// bool _checkNickChars(const std::string &name);

		bool _tryRegister();

	public:
		CommandHandler();
		CommandHandler(Server *server, Client *client, const IrcMessage &cmd);
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
