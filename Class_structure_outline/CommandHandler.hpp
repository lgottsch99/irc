#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#include <string>
#include "ParsedCommand.hpp"

class Server;
class Client;

class CommandHandler {
private:
    Server* _server;

public:
    CommandHandler(Server* server);
    ~CommandHandler();

	/*  client* here is the client that has sent the command
	*/
    void handleCmd(Client* client, const ParsedCommand& cmd);
	
	//internally forwards the msg to server:  server->send_response(fd, msg);
    void sendToClient(int fd, const std::string& msg);
   
	/* sends msg to multiple clients, internally: 
	1.lookup channel memmbers 
	2. iterate over users and sendToClient(user->fd, msg) for each (except if client is marked as except(aka the sender of the msg))
	*/
	void sendToChannel(const std::string& channelName,
                       const std::string& msg,
                       Client* exceptClient);
};

#endif
