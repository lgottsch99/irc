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




Mandatory Methods
appendToBuffer(const char*)
extractCompleteLines() → returns std::vector<std::string>
sendMessage(const std::string&)
*/

#include <string>
#include <iostream>
#include "Channel.hpp"

class Channel;

class Client {
	private:
	public:
		Client();
		~Client();

		int			fd; //fd for client socket
		std::string	ip_add;

		//for auth
		bool authenticated;   // after PASS+NICK+USER succeeds
		bool registered; // fully connected (001 welcome sent)
		bool has_sent_pass;
		bool has_sent_nick;
		bool has_sent_user;
		bool cap_negotiated;
		std::string nickname;
		std::string username;
		std::string realname;

		std::string recv_buffer;     // store partial lines from recv()
		std::string send_buffer;     // for queued writes if send() would block
		std::vector<Channel*> channels;  // channels the client is in



};

#endif