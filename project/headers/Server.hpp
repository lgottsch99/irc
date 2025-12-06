#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <string.h>
#include <vector>
#include <poll.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <cerrno>


#include "Client.hpp"


class Server {
private:
    int         _port;
    std::string _password;
    int         _serverSocketFd;
	static bool _signal; //-> static boolean for signal, to shutdown in clean way

    std::vector<struct pollfd> _pollfds;

	bool _str_is_digit(std::string str);
	void _validate_args(char *argv[]);
	void _setup_signal_handling(void);
	void _accept_new_client(void);
	void _receive_data(int client_fd);
	void _sendMsgBuf(pollfd* pfd);

	std::vector<int> _clients_to_disconnect; //tracker of fd of client that has some error
	
	void _disconnect_clients(void);
	void _remove_single_client(int client_fd);

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
    void pollLoop();
	void shutdown(void);


    // called by handler to server
    void replyToClient(Client* client, const std::string& msg); //INTERFACE  handler -> server

	void markClientToDisconnect(int client_fd);
	
    // Accessors for handler
    // std::map<int, Client*>& getClients();
    // std::map<std::string, Channel*>& getChannels();
	// void	disconnectClient(Client* client);
	// void	removeChannel(const std::string& name);


};

#endif
