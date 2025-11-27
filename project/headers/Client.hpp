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
operator flags, registration state, etc.
*/

#include <string>

class Client {
	private:
		int			_fd; //fd for client socket
		std::string	_client_ip_add;

	public:
		Client();



};

#endif