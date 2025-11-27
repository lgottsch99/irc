#ifndef SERVER_HPP
#define SERVER_HPP

/*
class for server, handling general server setup, listening, closing

*/

#include <iostream>
#include <string>
#include <stdlib.h> 
#include <sstream>
#include <cctype> //is digit
#include <vector>


#include <sys/socket.h> //socket, bind, listen ect
#include <netinet/in.h> //IPv4 specific sockaddr_in
#include <arpa/inet.h> //handling network addr, inet_addr
#include <poll.h> //-> for poll()

#include "Client.hpp"

class Server {
	private:
		int 						_port_num;
		std::string					_password;
		int							_ServerSocketFd;
		static bool 				_signal_received;		
		std::vector<class Client> 	_client_vec; //stores client state, not sockets.
		std::vector<struct pollfd>	_poll_fds; //stores client sockets. i/o listening engine. Index 0 is always the listening server socket. All other indexes correspond to connected client sockets.

		void _check_port_pw(char *argv[]);
		bool _only_digits(std::string str);
	
	public:
		Server(char *argv[]); //constructor
		~Server(); //destructor


		void start(void);

};

#endif