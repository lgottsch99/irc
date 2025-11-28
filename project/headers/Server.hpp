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
#include <unistd.h>


#include <sys/socket.h> //socket, bind, listen ect
#include <netinet/in.h> //IPv4 specific sockaddr_in
#include <arpa/inet.h> //handling network addr, inet_addr
#include <poll.h> //-> for poll()
#include <fcntl.h> //-> for fcntl()
#include <netinet/in.h> //-> for sockaddr_in

#include "Client.hpp"

class Server {
	private:

		int 						_port_num;
		std::string					_password;
		int							_ServerSocketFd;
		static bool 				_signal_received; // indicating ctrl d or similar received	
		
		std::vector<struct pollfd>	_poll_fds; //stores client sockets. i/o listening engine. Index 0 is always the listening server socket. All other indexes correspond to connected client sockets.
		std::vector<class Client> 	_client_vec; //stores client state, not sockets.

		void _check_port_pw(char *argv[]);
		bool _only_digits(std::string str);
	
	public:
		Server(void); //constructor
		~Server(); //destructor

		//server logic
		void startup(char *argv[]);
		void poll_loop(void);
		void accept_new_client(void);
		void receive_new_data(int client_socket_fd);
		void server_shutdown(void);


		//connection btw server + internal irc logic
		void handle_command(Client& client, std::string msg); 

		//internal irc logic



	};

#endif