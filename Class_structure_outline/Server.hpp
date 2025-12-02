#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <vector>
#include <poll.h>
#include "Client.hpp"
#include "Channel.hpp"
#include "CommandHandler.hpp"
#include "CommandParser.hpp"

class Server {
private:
    int         _port;
    std::string _password;
    int         _listenFd;

    std::vector<pollfd> _pollfds;

    // int FD → Client object
    std::map<int, Client*> _clients;

    // Name → Channel object
    std::map<std::string, Channel*> _channels;

    CommandHandler* _commandHandler;
    CommandParser    _parser;

    void _setupSocket();
    void _addPollFd(int fd);
    void _removePollFd(int fd);
    pollfd* _getPollFd(int fd);


public:
    Server();
    ~Server();

    void start(int port, const std::string& pass);
    void pollLoop();

    void acceptNewClient();
    void receiveData(int fd);

    // called by handler to server
    void sendResponse(int fd, const std::string& msg); //INTERFACE from handler to server

    // Accessors for handler
    std::map<int, Client*>& getClients();
    std::map<std::string, Channel*>& getChannels();
	void	disconnectClient(Client* client);
	void	removeChannel(const std::string& name);

};

#endif
