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

#include <sys/socket.h> //socket, bind, listen ect
#include <netinet/in.h> //IPv4 specific sockaddr_in
#include <arpa/inet.h> //handling network addr, inet_addr


class Server {
	private:
		int 		_port_num;
		std::string	_password;
		static bool _signal_received;		

		void _check_port_pw(char *argv[]);
		bool _only_digits(std::string str);
	
	public:
		Server(char *argv[]); //constructor
		~Server(); //destructor


		void start(void);

};

#endif