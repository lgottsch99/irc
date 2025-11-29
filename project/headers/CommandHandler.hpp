#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

/*
4) CommandHandler
This is the brain of the IRC server.
handles auth cmds and all other cmds



Server calls:
CommandHandler::execute(client, ParsedCommand)
You implement one method per command:
cmdCAP()
cmdPASS()
cmdNICK()
cmdUSER()
cmdJOIN()
cmdPRIVMSG()
cmdKICK()
cmdINVITE()
cmdTOPIC()
cmdMODE()

Plus internal helpers:
sendNumericReply(client, RPL_WELCOME, ...)
findChannel(name)
createChannel(name)


*/



#include <string>
#include <vector>
#include <map>

#include "CommandParser.hpp"   // You can place ParsedCommand into its own file or inside CommandParser.hpp
#include "Client.hpp"
#include "Channel.hpp"

class Server;  // forward declaration

class CommandHandler
{
	public:
		CommandHandler(Server* server);
		~CommandHandler();

		void execute(Client* client, const ParsedCommand& cmd);

	private:
		Server* _server;

		// ----- Internal handlers for IRC commands -----
		void cmdPASS(Client* client, const std::vector<std::string>& args);
		void cmdNICK(Client* client, const std::vector<std::string>& args);
		void cmdUSER(Client* client, const std::vector<std::string>& args);

		void cmdJOIN(Client* client, const std::vector<std::string>& args);
		void cmdPRIVMSG(Client* client, const std::vector<std::string>& args);

		// Operator commands
		void cmdKICK(Client* client, const std::vector<std::string>& args);
		void cmdINVITE(Client* client, const std::vector<std::string>& args);
		void cmdTOPIC(Client* client, const std::vector<std::string>& args);
		void cmdMODE(Client* client, const std::vector<std::string>& args);

		// ----- Helpers -----
		void sendNumeric(Client* client, int code, const std::string& msg);
		void registerClientIfReady(Client* client); // called after PASS/NICK/USER

		Channel* getOrCreateChannel(const std::string& name);
		Channel* findChannel(const std::string& name);
};


#endif