#include "../headers/Server.hpp"


// ------------ Constructors -----------------------------

//default
Server::Server()
{
	std::cout << "(Server) Default constructor\n";

}

//destructor
Server::~Server()
{
	std::cout << "(Server) Destructor\n";

	//close all open sockets / fds
	_shutdown();
}



// ------------ Member FTs -----------------------------

/* clean exit of server in case of error or stop signal
	-> closes all existing socket connections & fds
*/
void Server::_shutdown(void)
{
	//TODO
}

/* checking if each char is numeric
*/
bool Server::_str_is_digit(std::string str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (std::isdigit(str[i]) == 0)
			return false;
	}
	return true;
}

/* initializing server with port & pw
	-> throws EXCEPTIONS if invalid
*/
void Server::init(char* argv[])
{
	std::string port = argv[1];
	std::string pw = argv[2];

	//check port not empty
	if (!port.empty())
	{
		std::cout << "port: " << port << std::endl;

		//check port numbers only and within valid range between 0 and 65535
		if (_str_is_digit(port))
		{
			std::cout << "Port only digits\n";

			std::stringstream ss;
			int num;

			ss << port;
			ss >> num;
			if (num < 0 || num > 65535)
				throw std::runtime_error("ERROR: PORT outside valid range.");
			else
				_port = num;
		}
		else
			throw std::runtime_error("ERROR: PORT invalid.");
	}
	else
		throw std::runtime_error("ERROR: PORT cannot be empty.");

	//check pw not empty
	if (!pw.empty())
	{
		std::cout << "pw: " << pw << std::endl;
		_password = pw;
	}
	else
		throw std::runtime_error("ERROR: PASSWORD cannot be empty.");

	std::cout << "Port is: " << _port << std::endl << "Pw is: " << _password << std::endl;

}