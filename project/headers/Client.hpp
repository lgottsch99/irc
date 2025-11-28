#ifndef CLIENT_HPP
#define CLIENT_HPP

/*
class for single client connection info, each client connected to server gets its own client class instance

The Client class should contain things like:
fd (socket)
nickname
username
hostname (optional)
buffer for partial incoming messages
channels the client is in
operator flags, 
registration state, etc.
*/

#include <string>
#include <iostream>

class Client {
	private:
	public:
		Client();
		~Client();

		int			fd; //fd for client socket
		std::string	ip_add;

		std::string msg_buffer; //for incoming messages and leftovers
		//for auth
		bool registered;
		bool has_sent_pass;
		bool has_sent_nick;
		bool has_sent_user;
		std::string nickname;
		std::string username;
		std::string realname;




};

#endif