#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>

class Client
{
	private:

	public:
		Client();
		~Client();

		int fd; //client socket fd. needed by server
		std::string ip_address; // client ip. mainly for identification and logging
		std::string recv_buf; //buffer for incoming data. needed by server

		std::string extract_line(void);
		
		
		std::string nickname; //Each client is distinguished from other clients by a unique nickname. comes with NICK cmd
		std::string username; //sername of the client on that host. comes with USER cmd
		std::string realname; //real name/address of the host that the client is connecting from. comes with USER cmd




};

#endif