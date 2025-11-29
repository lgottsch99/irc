#include "../headers/CommandHandler.hpp"
#include "../headers/Server.hpp"

CommandHandler::CommandHandler(Server* server)
{
	std::cout << "(CommandHandler) Default constructor\n";
	(void)server;
}
CommandHandler::~CommandHandler()
{
	std::cout << "(CommandHandler) Destructor\n";

	//close all open sockets / fds

}
