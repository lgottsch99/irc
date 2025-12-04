#include "../headers/Server.hpp"


// ------------ Constructors -----------------------------


bool Server::_signal = false; //-> initialize the static boolean

//default
Server::Server(): _serverSocketFd(-1)
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


	if (_serverSocketFd != -1)
		close(_serverSocketFd);
}

/* handling ctrl c, ctrl / for server shutdown
*/
void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::_signal = true; //-> set the static boolean to true to stop the server
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

/*validating port & pw
	-> throws EXCEPTIONS if invalid
*/
void Server::_validate_args(char *argv[])
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

}


/* initializing server, creating server socket
	-> throws EXCEPTIONS if invalid
*/
void Server::init(char* argv[])
{
	_validate_args(argv);
	std::cout << "Port is: " << _port << std::endl << "Pw is: " << _password << std::endl << "CREATING SEERVER SOCKET NOW!" << std::endl;

	//server socket (socket - setoptions - bind - listen)

	int _serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocketFd == -1)
		throw std::runtime_error("ERROR: Server socket creation failed.");
	
	//set socket conf
		//NONBLOCKING + (Reuseaddr for dev)
	//TODO



	/*
	struct sockaddr_in {
    sa_family_t    sin_family; // AF_INET
    in_port_t      sin_port;   // port
    struct in_addr sin_addr;   // IP address
	};
	*/
	struct sockaddr_in address; //holding all network conf that our sockets need (doc https://man7.org/linux/man-pages/man3/sockaddr.3type.html)
	address.sin_family = AF_INET; //-> set address family to ipv4
	address.sin_port = htons((uint16_t)_port); // convert port to network byte order (big endian), network always uses big e, most modern machines little e
	address.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY is a constant (0.0.0.0) defined in <netinet/in.h> (alternative: address.sin_addr.s_addr = inet_addr("192.168.1.100"))


	//bind to addr
	if (bind(_serverSocketFd, (const struct sockaddr) &address, sizeof(address)) == -1)
		throw std::runtime_error("ERROR: Server socket bind failed.");

	//make it a server side socket -> listen
	if (listen(_serverSocketFd, SOMAXCONN) == -1) // SOMAXCONN is system dependend max value of backlog(man listen) -> makes it portable
		throw std::runtime_error("ERROR: Server socket listen failed.");




}