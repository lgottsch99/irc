#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <vector>
#include <poll.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Client.hpp"


class Server {
private:
    int         _port;
    std::string _password;
    int         _serverSocketFd;
	static bool _signal; //-> static boolean for signal, to shutdown in clean way

    std::vector<struct pollfd> _pollfds; 

	void _shutdown(void);
	bool _str_is_digit(std::string str);
	void _validate_args(char *argv[]);

public:
    Server();
    ~Server();

	//CommandHandler* CmdHandler;
    //CommandParser   Parser;

    // int FD → Client object
    std::map<int, Client*> Clients;

    // Name → Channel object
    //std::map<std::string, Channel*> Channels;

	static void SignalHandler(int signum);

    void init(char *argv[]);
    // void pollLoop();

    // void acceptNewClient();
    // void receiveData(int fd);

    // called by handler to server
    void sendResponse(int fd, const std::string& msg); //INTERFACE  handler -> server

    // Accessors for handler
    // std::map<int, Client*>& getClients();
    // std::map<std::string, Channel*>& getChannels();
	// void	disconnectClient(Client* client);
	// void	removeChannel(const std::string& name);


};

#endif
