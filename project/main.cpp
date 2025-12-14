#include "headers/Client.hpp"
#include "headers/CommandHandler.hpp"
#include "headers/Server.hpp"
#include <iostream>

void printSeparator(const std::string &testName)
{
	std::cout << "\n========================================\n";
	std::cout << "TEST: " << testName << "\n";
	std::cout << "========================================\n";
}

void printChannels(Server &server)
{
	std::cout << "\n--- CHANNELS ON SERVER ---\n";
	if (server.Channels.empty())
	{
		std::cout << "  (No channels exist)\n";
		return;
	}
	for (std::map<std::string, Channel*>::iterator it = server.Channels.begin(); 
		 it != server.Channels.end(); ++it)
	{
		std::cout << "  Channel: " << it->first << "\n";
	}
	std::cout << "--- END CHANNELS ---\n";
}

void printClients(Server &server)
{
	std::cout << "\n--- CLIENTS ON SERVER ---\n";
	if (server.Clients.empty())
	{
		std::cout << "  (No clients connected)\n";
		return;
	}
	for (std::map<int, Client*>::iterator it = server.Clients.begin(); 
		 it != server.Clients.end(); ++it)
	{
		Client *c = it->second;
		std::cout << "  FD " << it->first << ": ";
		std::cout << "Nick=" << (c->getNickname().empty() ? "(none)" : c->getNickname()) << ", ";
		std::cout << "User=" << (c->getUsername().empty() ? "(none)" : c->getUsername()) << ", ";
		std::cout << "Auth=" << c->isAuthenticated() << ", ";
		std::cout << "Reg=" << c->isRegistered() << "\n";
	}
	std::cout << "--- END CLIENTS ---\n";
}

void printChannelMemberships(Server &server)
{
	std::cout << "\n--- CHANNEL MEMBERSHIPs ---\n";
	if (server.Channels.empty())
	{
		std::cout << "  (No channels exist)\n";
	}
	else
	{
		for (std::map<std::string, Channel*>::iterator it = server.Channels.begin(); it != server.Channels.end(); ++it)
		{
			Channel *ch = it->second;
			std::cout << "  Channel " << it->first << " users: ";
			bool first = true;
			for (std::map<int, Client*>::iterator cit = server.Clients.begin(); cit != server.Clients.end(); ++cit)
			{
				if (ch->hasUser(cit->second))
				{
					if (!first)
						std::cout << ", ";
					std::cout << (cit->second->getNickname().empty() ? "(unnamed)" : cit->second->getNickname());
					first = false;
				}
			}
			if (first)
				std::cout << "(no users)";
			std::cout << "\n    (operators not listed; API missing)\n";
		}
	}
	std::cout << "--- END CHANNEL MEMBERSHIP ---\n";

	std::cout << "\n--- CLIENT → CHANNELS ---\n";
	for (std::map<int, Client*>::iterator cit = server.Clients.begin(); cit != server.Clients.end(); ++cit)
	{
		Client *c = cit->second;
		std::cout << "  " << (c->getNickname().empty() ? "(unnamed)" : c->getNickname()) << " joined: ";
		bool first = true;
		for (std::map<std::string, Channel*>::iterator it = server.Channels.begin(); it != server.Channels.end(); ++it)
		{
			if (it->second->hasUser(c))
			{
				if (!first)
					std::cout << ", ";
				std::cout << it->first;
				first = false;
			}
		}
		if (first)
			std::cout << "(no channels)";
		std::cout << "\n";
	}
	std::cout << "--- END CLIENT → CHANNELS ---\n";
}

int main()
{
	std::cout << "\n╔════════════════════════════════════════╗\n";
	std::cout << "║   IRC COMMAND HANDLER - TEST SUITE     ║\n";
	std::cout << "╚════════════════════════════════════════╝\n";

	// Setup
	Server server;
	server.setPass("secretpass");
	std::cout << "\n[SETUP] Server password set to: " << server.getPassword() << "\n";

	Client *alice = new Client();
	Client *bob = new Client();
	Client *charlie = new Client();

	alice->fd = 10;
	bob->fd = 11;
	charlie->fd = 12;

	server.Clients.insert(std::make_pair(alice->fd, alice));
	server.Clients.insert(std::make_pair(bob->fd, bob));
	server.Clients.insert(std::make_pair(charlie->fd, charlie));

	// std::cout << "[SETUP] Created 3 clients: Alice (fd=10), Bob (fd=11), Charlie (fd=12)\n";
	// printClients(server);

	IrcMessage cmd;

	// // =============== TEST PASS COMMAND ===============
	// printSeparator("PASS - Correct Password");
	cmd.command = "PASS";
	cmd.params.clear();
	cmd.params.push_back("secretpass");
	cmd.trailing = "";
	
	// std::cout << "[CMD] PASS secretpass\n";
	CommandHandler::handleCmd(&server, alice, cmd);
	// std::cout << "[RESULT] Alice authenticated: " << alice->isAuthenticated() << "\n";

	// printSeparator("PASS - Wrong Password");
	// cmd.params.clear();
	// cmd.params.push_back("wrongpass");
	// std::cout << "[CMD] PASS wrongpass\n";
	// CommandHandler::handleCmd(&server, bob, cmd);
	// std::cout << "[RESULT] Bob authenticated: " << bob->isAuthenticated() << "\n";

	// printSeparator("PASS - No Parameters");
	// cmd.params.clear();
	// std::cout << "[CMD] PASS (no params)\n";
	// CommandHandler::handleCmd(&server, charlie, cmd);
	// std::cout << "[RESULT] Charlie authenticated: " << charlie->isAuthenticated() << "\n";

	// Authenticate Bob properly for future tests
	cmd.params.clear();
	cmd.params.push_back("secretpass");
	CommandHandler::handleCmd(&server, bob, cmd);
	// std::cout << "\n[SETUP] Bob authenticated with correct password\n";

	// =============== TEST NICK COMMAND ===============
	// printSeparator("NICK - Valid Nickname");
	cmd.command = "NICK";
	cmd.params.clear();
	cmd.params.push_back("Alice");
	// std::cout << "[CMD] NICK Alice\n";
	CommandHandler::handleCmd(&server, alice, cmd);
	// std::cout << "[RESULT] Alice's nickname: " << alice->getNickname() << "\n";

	// printSeparator("NICK - Duplicate Nickname");
	// cmd.params.clear();
	// cmd.params.push_back("Alice");
	// std::cout << "[CMD] NICK Alice (Bob trying to use Alice's nick)\n";
	// CommandHandler::handleCmd(&server, bob, cmd);
	// std::cout << "[RESULT] Bob's nickname: " << (bob->getNickname().empty() ? "(none)" : bob->getNickname()) << "\n";

	// printSeparator("NICK - Valid Unique Nickname");
	cmd.params.clear();
	cmd.params.push_back("BobTheBuilder");
	// std::cout << "[CMD] NICK BobTheBuilder\n";
	CommandHandler::handleCmd(&server, bob, cmd);
	// std::cout << "[RESULT] Bob's nickname: " << bob->getNickname() << "\n";

	// printSeparator("NICK - No Parameters");
	// cmd.params.clear();
	// std::cout << "[CMD] NICK (no params)\n";
	// CommandHandler::handleCmd(&server, alice, cmd);

	// printSeparator("NICK - Without Authentication");
	// cmd.params.clear();
	// cmd.params.push_back("CharlieBrown");
	// std::cout << "[CMD] NICK CharlieBrown (Charlie not authenticated)\n";
	// CommandHandler::handleCmd(&server, charlie, cmd);
	// std::cout << "[RESULT] Charlie's nickname: " << (charlie->getNickname().empty() ? "(none)" : charlie->getNickname()) << "\n";

	// printClients(server);

	// // =============== TEST USER COMMAND ===============
	// printSeparator("USER - Valid Registration");
	cmd.command = "USER";
	cmd.params.clear();
	cmd.params.push_back("alice_user");
	cmd.trailing = "Alice Wonderland";
	// std::cout << "[CMD] USER alice_user :Alice Wonderland\n";
	CommandHandler::handleCmd(&server, alice, cmd);
	// std::cout << "[RESULT] Username: " << alice->getUsername() << "\n";
	// std::cout << "[RESULT] Realname: " << alice->getRealname() << "\n";
	// std::cout << "[RESULT] Registered: " << alice->isRegistered() << "\n";

	// printSeparator("USER - Valid Registration for Bob");
	cmd.params.clear();
	cmd.params.push_back("bob_user");
	cmd.trailing = "Bob The Great";
	// std::cout << "[CMD] USER bob_user :Bob The Great\n";
	CommandHandler::handleCmd(&server, bob, cmd);
	// std::cout << "[RESULT] Username: " << bob->getUsername() << "\n";
	// std::cout << "[RESULT] Realname: " << bob->getRealname() << "\n";
	// std::cout << "[RESULT] Registered: " << bob->isRegistered() << "\n";

	// printSeparator("USER - Duplicate Username");
	// cmd.params.clear();
	// cmd.params.push_back("alice_user");
	// cmd.trailing = "Fake Alice";
	// std::cout << "[CMD] USER alice_user :Fake Alice (Bob trying Alice's username)\n";
	// CommandHandler::handleCmd(&server, bob, cmd);

	// printSeparator("USER - Missing Parameters");
	// cmd.params.clear();
	// cmd.trailing = "";
	// std::cout << "[CMD] USER (no params)\n";
	// CommandHandler::handleCmd(&server, charlie, cmd);

	printClients(server);

	// =============== TEST JOIN COMMAND ===============
	// printSeparator("JOIN - Create New Channel");
	cmd.command = "JOIN";
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.trailing = "";
	// std::cout << "[CMD] JOIN #general (Alice)\n";
	CommandHandler::handleCmd(&server, alice, cmd);
	// printChannels(server);

	// printSeparator("JOIN - Join Existing Channel");
	cmd.params.clear();
	cmd.params.push_back("#general");
	// std::cout << "[CMD] JOIN #general (Bob)\n";
	CommandHandler::handleCmd(&server, bob, cmd);
	// printChannels(server);

	// printSeparator("JOIN - Duplicate Join Attempt");
	cmd.params.clear();
	cmd.params.push_back("#general");
	// std::cout << "[CMD] JOIN #general (Alice again)\n";
	CommandHandler::handleCmd(&server, alice, cmd);
	// printChannels(server);

	// printSeparator("JOIN - Missing Parameters");
	cmd.params.clear();
	// std::cout << "[CMD] JOIN (no params) (Charlie)\n";
	CommandHandler::handleCmd(&server, charlie, cmd);

	// printSeparator("JOIN - Unregistered Client");
	cmd.params.clear();
	cmd.params.push_back("#unreg");
	// std::cout << "[CMD] JOIN #unreg (Charlie unregistered)\n";
	CommandHandler::handleCmd(&server, charlie, cmd);

	printChannels(server);

	// =============== TEST MODE COMMAND ===============
	printSeparator("MODE - Set Invite-Only (+i)");
	cmd.command = "MODE";
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.params.push_back("+i");
	std::cout << "[CMD] MODE #general +i\n";
	CommandHandler::handleCmd(&server, alice, cmd);

	printSeparator("MODE - Remove Invite-Only (-i)");
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.params.push_back("-i");
	std::cout << "[CMD] MODE #general -i\n";
	CommandHandler::handleCmd(&server, alice, cmd);

	printSeparator("MODE - Set Channel Key (+k)");
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.params.push_back("+k");
	cmd.params.push_back("secret123");
	std::cout << "[CMD] MODE #general +k secret123\n";
	CommandHandler::handleCmd(&server, alice, cmd);

	printSeparator("MODE - Remove Channel Key (-k)");
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.params.push_back("-k");
	std::cout << "[CMD] MODE #general -k\n";
	CommandHandler::handleCmd(&server, alice, cmd);

	printSeparator("MODE - Set User Limit (+l)");
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.params.push_back("+l");
	cmd.params.push_back("50");
	std::cout << "[CMD] MODE #general +l 50\n";
	CommandHandler::handleCmd(&server, alice, cmd);

	printSeparator("MODE - Invalid User Limit");
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.params.push_back("+l");
	cmd.params.push_back("notanumber");
	std::cout << "[CMD] MODE #general +l notanumber\n";
	CommandHandler::handleCmd(&server, alice, cmd);

	printSeparator("MODE - Remove User Limit (-l)");
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.params.push_back("-l");
	std::cout << "[CMD] MODE #general -l\n";
	CommandHandler::handleCmd(&server, alice, cmd);

	printSeparator("MODE - Unknown Mode");
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.params.push_back("+x");
	std::cout << "[CMD] MODE #general +x\n";
	CommandHandler::handleCmd(&server, alice, cmd);

	printSeparator("MODE - Missing Parameters");
	cmd.params.clear();
	cmd.params.push_back("#general");
	std::cout << "[CMD] MODE #general (no mode specified)\n";
	CommandHandler::handleCmd(&server, alice, cmd);

	printSeparator("MODE - Non-member trying to set mode");
	charlie->setAuthenticated(true);
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.params.push_back("+i");
	std::cout << "[CMD] MODE #general +i (Charlie not in channel)\n";
	CommandHandler::handleCmd(&server, charlie, cmd);

	printSeparator("MODE - Non-existent Channel");
	cmd.params.clear();
	cmd.params.push_back("#nonexistent");
	cmd.params.push_back("+i");
	std::cout << "[CMD] MODE #nonexistent +i\n";
	CommandHandler::handleCmd(&server, alice, cmd);

	// // =============== TEST TOPIC COMMAND ===============
	// printSeparator("TOPIC - Command Test");
	// cmd.command = "TOPIC";
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.trailing = "Welcome to the general channel!";
	// std::cout << "[CMD] TOPIC #general :Welcome to the general channel!\n";
	// std::cout << "[INFO] TOPIC command is not fully implemented yet\n";
	// CommandHandler::handleCmd(&server, alice, cmd);

	// // =============== TEST INVITE COMMAND ===============
	// printSeparator("INVITE - Command Test");
	// cmd.command = "INVITE";
	// cmd.params.clear();
	// cmd.params.push_back("BobTheBuilder");
	// cmd.params.push_back("#random");
	// std::cout << "[CMD] INVITE BobTheBuilder #random\n";
	// std::cout << "[INFO] INVITE command is not fully implemented yet\n";
	// CommandHandler::handleCmd(&server, alice, cmd);

	// // =============== TEST KICK COMMAND ===============
	// printSeparator("KICK - Command Test");
	// cmd.command = "KICK";
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("BobTheBuilder");
	// cmd.trailing = "Spamming";
	// std::cout << "[CMD] KICK #general BobTheBuilder :Spamming\n";
	// std::cout << "[INFO] KICK command is not fully implemented yet\n";
	// CommandHandler::handleCmd(&server, alice, cmd);

	// // =============== TEST QUIT COMMAND ===============
	// printSeparator("QUIT - With Message");
	// cmd.command = "QUIT";
	// cmd.params.clear();
	// cmd.trailing = "Goodbye everyone!";
	// std::cout << "[CMD] QUIT :Goodbye everyone!\n";
	// CommandHandler::handleCmd(&server, bob, cmd);
	// std::cout << "[INFO] Bob should be marked for disconnection\n";

	// printSeparator("QUIT - Without Message");
	// cmd.params.clear();
	// cmd.trailing = "";
	// std::cout << "[CMD] QUIT (no message)\n";
	// CommandHandler::handleCmd(&server, charlie, cmd);
	// std::cout << "[INFO] Charlie should be marked for disconnection\n";

	// =============== FINAL STATE ===============
	printSeparator("FINAL STATE");
	printClients(server);
	printChannels(server);
	printChannelMemberships(server);

	std::cout << "\n╔════════════════════════════════════════╗\n";
	std::cout << "║  ALL TESTS COMPLETED                   ║\n";
	std::cout << "╚════════════════════════════════════════╝\n\n";

	return 0;
}

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
// 		std::cout << "init server\n";
// 		irc_server.init(argv);
// 		irc_server.pollLoop();

// 	}
// 	catch(const std::exception& e)
// 	{
// 		std::cerr << e.what() << '\n';
// 		irc_server.shutdown();
// 	}
// 	return 0;
// }
