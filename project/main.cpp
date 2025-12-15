#include "headers/Client.hpp"
#include "headers/CommandHandler.hpp"
#include "headers/Server.hpp"
#include <iostream>

// ==================== HELPER FUNCTIONS ====================

void printSeparator(const std::string &testName)
{
	std::cout << "\n" << std::string(50, '=') << "\n";
	std::cout << "TEST: " << testName << "\n";
	std::cout << std::string(50, '=') << "\n";
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
			std::cout << "\n    (operators not listed)\n";
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

	// ==================== SETUP ====================
	Server server;
	server.setPass("testpass");

	// Create test clients
	Client *alice = new Client();
	Client *bob = new Client();
	Client *charlie = new Client();
	Client *dave = new Client();

	alice->fd = 10;
	bob->fd = 11;
	charlie->fd = 12;
	dave->fd = 13;

	server.Clients[alice->fd] = alice;
	server.Clients[bob->fd] = bob;
	server.Clients[charlie->fd] = charlie;
	server.Clients[dave->fd] = dave;

	// Pre-register all clients for channel tests
	alice->setAuthenticated(true);
	alice->setNickname("Alice");
	alice->setUsername("alice_user");
	alice->setRealname("Alice Wonderland");
	alice->setRegistered(true);

	bob->setAuthenticated(true);
	bob->setNickname("Bob");
	bob->setUsername("bob_user");
	bob->setRealname("Bob The Builder");
	bob->setRegistered(true);

	charlie->setAuthenticated(true);
	charlie->setNickname("Charlie");
	charlie->setUsername("charlie_user");
	charlie->setRealname("Charlie Brown");
	charlie->setRegistered(true);

	dave->setAuthenticated(true);
	dave->setNickname("Dave");
	dave->setUsername("dave_user");
	dave->setRealname("Dave Grohl");
	dave->setRegistered(true);

	IrcMessage cmd;

	// ==================== JOIN TESTS ====================

	// printSeparator("JOIN - Create new channel #general");
	// cmd.command = "JOIN";
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.trailing = "";
	// CommandHandler handler(&server, alice, cmd);
	// handler.handleCmd();

	// printSeparator("JOIN - Second user joins #general");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// CommandHandler handler2(&server, bob, cmd);
	// handler2.handleCmd();

	// printSeparator("JOIN - Duplicate join attempt (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// CommandHandler handler3(&server, alice, cmd);
	// handler3.handleCmd();

	// printSeparator("JOIN - No parameters (should fail)");
	// cmd.params.clear();
	// CommandHandler handler4(&server, charlie, cmd);
	// handler4.handleCmd();

	// printSeparator("JOIN - Create multiple channels");
	// cmd.params.clear();
	// cmd.params.push_back("#testing");
	// CommandHandler handler5(&server, charlie, cmd);
	// handler5.handleCmd();

	// cmd.params.clear();
	// cmd.params.push_back("#random");
	// CommandHandler handler6(&server, dave, cmd);
	// handler6.handleCmd();

	// printSeparator("JOIN - User joins existing channel");
	// cmd.params.clear();
	// cmd.params.push_back("#testing");
	// CommandHandler handler7(&server, alice, cmd);
	// handler7.handleCmd();

	// printChannels(server);
	// printChannelMemberships(server);

	// // ==================== MODE TESTS ====================

	// printSeparator("MODE - Set invite-only (+i) on #general");
	// cmd.command = "MODE";
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("+i");
	// cmd.trailing = "";
	// CommandHandler modeH1(&server, alice, cmd);
	// modeH1.handleCmd();

	// printSeparator("MODE - Non-operator tries to set mode (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("-i");
	// CommandHandler modeH2(&server, bob, cmd);
	// modeH2.handleCmd();

	// printSeparator("MODE - Remove invite-only (-i) on #general");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("-i");
	// CommandHandler modeH3(&server, alice, cmd);
	// modeH3.handleCmd();

	// printSeparator("MODE - Set channel key (+k)");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("+k");
	// cmd.params.push_back("mykey123");
	// CommandHandler modeH4(&server, alice, cmd);
	// modeH4.handleCmd();

	// printSeparator("MODE - Remove channel key (-k)");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("-k");
	// CommandHandler modeH5(&server, alice, cmd);
	// modeH5.handleCmd();

	// printSeparator("MODE - Set user limit (+l) with valid number");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("+l");
	// cmd.params.push_back("10");
	// CommandHandler modeH6(&server, alice, cmd);
	// modeH6.handleCmd();

	// printSeparator("MODE - Set user limit with invalid number (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("+l");
	// cmd.params.push_back("notanumber");
	// CommandHandler modeH7(&server, alice, cmd);
	// modeH7.handleCmd();

	// printSeparator("MODE - Set user limit with zero (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("+l");
	// cmd.params.push_back("0");
	// CommandHandler modeH8(&server, alice, cmd);
	// modeH8.handleCmd();

	// printSeparator("MODE - Remove user limit (-l)");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("-l");
	// CommandHandler modeH9(&server, alice, cmd);
	// modeH9.handleCmd();

	// printSeparator("MODE - Set topic restriction (+t)");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("+t");
	// CommandHandler modeH10(&server, alice, cmd);
	// modeH10.handleCmd();

	// printSeparator("MODE - Remove topic restriction (-t)");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("-t");
	// CommandHandler modeH11(&server, alice, cmd);
	// modeH11.handleCmd();

	// printSeparator("MODE - Missing parameters (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// CommandHandler modeH12(&server, alice, cmd);
	// modeH12.handleCmd();

	// printSeparator("MODE - Invalid mode character (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("+x");
	// CommandHandler modeH13(&server, alice, cmd);
	// modeH13.handleCmd();

	// printSeparator("MODE - On non-existent channel (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("#nonexistent");
	// cmd.params.push_back("+i");
	// CommandHandler modeH14(&server, alice, cmd);
	// modeH14.handleCmd();

	// printSeparator("MODE - User not in channel (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("#testing");
	// cmd.params.push_back("+i");
	// CommandHandler modeH15(&server, bob, cmd);
	// modeH15.handleCmd();

	// ==================== TOPIC TESTS ====================

	printSeparator("TOPIC - Set topic on #general");
	cmd.command = "TOPIC";
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.trailing = "Welcome to general discussion";
	CommandHandler topicH1(&server, alice, cmd);
	topicH1.handleCmd();

	printSeparator("TOPIC - Get topic");
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.trailing = "";
	CommandHandler topicH2(&server, bob, cmd);
	topicH2.handleCmd();

	printSeparator("TOPIC - Change topic");
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.trailing = "New topic: Rules updated!";
	CommandHandler topicH3(&server, alice, cmd);
	topicH3.handleCmd();

	printSeparator("TOPIC - Non-operator changes topic with restriction");
	cmd.command = "MODE";
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.params.push_back("+t");
	CommandHandler modeRestrict(&server, alice, cmd);
	modeRestrict.handleCmd();

	cmd.command = "TOPIC";
	cmd.params.clear();
	cmd.params.push_back("#general");
	cmd.trailing = "Trying to change restricted topic";
	CommandHandler topicH4(&server, bob, cmd);
	topicH4.handleCmd();

	printSeparator("TOPIC - Missing parameters (should fail)");
	cmd.params.clear();
	cmd.trailing = "";
	CommandHandler topicH5(&server, alice, cmd);
	topicH5.handleCmd();

	printSeparator("TOPIC - Non-existent channel (should fail)");
	cmd.params.clear();
	cmd.params.push_back("#nonexistent");
	cmd.trailing = "test";
	CommandHandler topicH6(&server, alice, cmd);
	topicH6.handleCmd();

	printSeparator("TOPIC - User not in channel (should fail)");
	cmd.params.clear();
	cmd.params.push_back("#testing");
	cmd.trailing = "Should fail";
	CommandHandler topicH7(&server, bob, cmd);
	topicH7.handleCmd();

	// // ==================== INVITE TESTS ====================

	// printSeparator("INVITE - Basic invite");
	// cmd.command = "INVITE";
	// cmd.params.clear();
	// cmd.params.push_back("Dave");
	// cmd.params.push_back("#general");
	// cmd.trailing = "";
	// CommandHandler inviteH1(&server, alice, cmd);
	// inviteH1.handleCmd();

	// printSeparator("INVITE - Missing parameters (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("Dave");
	// CommandHandler inviteH2(&server, alice, cmd);
	// inviteH2.handleCmd();

	// printSeparator("INVITE - Invite non-existent user (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("NonExistent");
	// cmd.params.push_back("#general");
	// CommandHandler inviteH3(&server, alice, cmd);
	// inviteH3.handleCmd();

	// printSeparator("INVITE - Invite to non-existent channel");
	// cmd.params.clear();
	// cmd.params.push_back("Dave");
	// cmd.params.push_back("#nonexistent");
	// CommandHandler inviteH4(&server, alice, cmd);
	// inviteH4.handleCmd();

	// printSeparator("INVITE - Non-member tries to invite (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("Dave");
	// cmd.params.push_back("#testing");
	// CommandHandler inviteH5(&server, bob, cmd);
	// inviteH5.handleCmd();

	// printSeparator("INVITE - Invite already on channel (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("Bob");
	// cmd.params.push_back("#general");
	// CommandHandler inviteH6(&server, alice, cmd);
	// inviteH6.handleCmd();

	// // ==================== KICK TESTS ====================

	// printSeparator("KICK - Basic kick");
	// cmd.command = "KICK";
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("Bob");
	// cmd.trailing = "Spamming";
	// CommandHandler kickH1(&server, alice, cmd);
	// kickH1.handleCmd();

	// printSeparator("KICK - Non-operator tries to kick (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("Alice");
	// cmd.trailing = "";
	// CommandHandler kickH2(&server, bob, cmd);
	// kickH2.handleCmd();

	// printSeparator("KICK - Missing parameters (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// CommandHandler kickH3(&server, alice, cmd);
	// kickH3.handleCmd();

	// printSeparator("KICK - Non-existent channel (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("#nonexistent");
	// cmd.params.push_back("Bob");
	// cmd.trailing = "";
	// CommandHandler kickH4(&server, alice, cmd);
	// kickH4.handleCmd();

	// printSeparator("KICK - Kick user not in channel (should fail)");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("Bob");
	// CommandHandler kickH5(&server, alice, cmd);
	// kickH5.handleCmd();

	// printSeparator("KICK - With reason message");
	// cmd.params.clear();
	// cmd.params.push_back("#general");
	// cmd.params.push_back("Charlie");
	// cmd.trailing = "Being too quiet";
	// CommandHandler kickH6(&server, alice, cmd);
	// kickH6.handleCmd();

	// ==================== FINAL STATE ====================
	printSeparator("FINAL SERVER STATE");
	printClients(server);
	printChannels(server);
	printChannelMemberships(server);

	std::cout << "\n╔════════════════════════════════════════╗\n";
	std::cout << "║  ALL TESTS COMPLETED                   ║\n";
	std::cout << "╚════════════════════════════════════════╝\n\n";

	return 0;
}

