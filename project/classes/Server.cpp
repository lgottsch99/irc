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
	shutdown();
}



// ------------ Member FTs -----------------------------

/* clean exit of server in case of error or stop signal
	-> closes all existing socket connections
	->clears ALL Clients & data
*/
void Server::shutdown(void)
{
	//TODO

	//close existing connections?

	//delete all client instances
	for (std::map<int, Client*>::iterator it = Clients.begin(); it != Clients.end(); it++)
		delete it->second;
	// remove map container
	Clients.clear();

	//go thru pollfd struct and close all open fds (includes Servsocket!)
	for (int i = 0; i < (int)_pollfds.size(); i++)
	{
		if (_pollfds[i].fd >= 0)
			close(_pollfds[i].fd);
	}
}

/* removes single client

!!! Client *irc connection* needs to be CLOSED at this point
*/
void Server::remove_single_client(int client_fd)
{
	std::cout << "Removing a client..\n";


	//delete & erase Client Instance from map
	std::map<int, Client*>::iterator it = Clients.find(client_fd);
	delete (it->second);
	Clients.erase(it);

	std::vector<struct pollfd>::iterator pit = _pollfds.begin();
	while (pit != _pollfds.end())
	{
		if (pit->fd == client_fd)
		{
			_pollfds.erase(pit);
			break;
		}
		pit++;
	}
	close(client_fd);
}


/* handling ctrl c, ctrl / for server shutdown
*/
void Server::SignalHandler(int signum)
{
	(void)signum;
	Server::_signal = true; //-> set the static boolean to true to stop the server
}

void Server::_setup_signal_handling(void)
{
	struct sigaction sa;
	sa.sa_handler = &Server::SignalHandler;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sigaction(SIGINT, &sa, NULL); //ctrl c
	sigaction(SIGQUIT, &sa, NULL); //ctrl backslash
	sigaction(SIGTERM, &sa, NULL); //kill

	//SIGPIPE needed???? TODO

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
		//check port numbers only and within valid range between 0 and 65535
		if (_str_is_digit(port))
		{
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
		_password = pw;
	else
		throw std::runtime_error("ERROR: PASSWORD cannot be empty.");

}


/* initializing server, creating server socket
	-> throws EXCEPTIONS if invalid
*/
void Server::init(char* argv[])
{
	_validate_args(argv);
	std::cout << "Port is: " << _port << std::endl << "Pw is: " << _password << std::endl << "CREATING SERVER SOCKET...\n" << std::endl;

	_setup_signal_handling();

	//server socket (socket - setoptions - bind - listen)
	_serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocketFd == -1)
	{
		std::cerr << "errno = " << errno << std::endl;
		throw std::runtime_error("ERROR: Server socket creation failed.");
	}
	
	//set socket conf
		//(Reuseaddr for dev)
	int enable = 1;
	if (setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)//SOL_SOCKET: option on socket level, 
	{
		close(_serverSocketFd);
		throw std::runtime_error("ERROR: setting SO_REUSEADDR failed.");
	}

		// NONBLOCKING
	if (fcntl(_serverSocketFd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(_serverSocketFd);
		throw std::runtime_error("ERROR: setting O_NONBLOCK failed.");
	}

	/*
	struct sockaddr_in {
    sa_family_t    sin_family; // AF_INET
    in_port_t      sin_port;   // port
    struct in_addr sin_addr;   // IP address
	};
	*/
	struct sockaddr_in address; //holding all network conf that our sockets need (doc https://man7.org/linux/man-pages/man3/sockaddr.3type.html)
	memset(&address, 0, sizeof(address));  // preventing garbage
	address.sin_family = AF_INET; //-> set address family to ipv4
	address.sin_port = htons(_port); // convert port to network byte order (big endian), network always uses big e, most modern machines little e
	address.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY is a constant (0.0.0.0) defined in <netinet/in.h> (alternative: address.sin_addr.s_addr = inet_addr("192.168.1.100"))

	//bind to addr
	if (bind(_serverSocketFd, (const struct sockaddr*) &address, sizeof(address)) == -1)
	{
		close(_serverSocketFd);
		if (errno == 13)
			throw std::runtime_error("ERROR: Server socket bind failed. Use port > 1024");
		throw std::runtime_error("ERROR: Server socket bind failed.");
	}

	//make it a server side socket -> listen
	if (listen(_serverSocketFd, SOMAXCONN) == -1) // SOMAXCONN is system dependend max value of backlog(man listen) -> makes it portable
	{
		close(_serverSocketFd);
		throw std::runtime_error("ERROR: Server socket listen failed.");
	}

	//add server socket to poll struct
	struct pollfd NewPoll;
	NewPoll.fd = _serverSocketFd;
	NewPoll.events = POLLIN; //serv socket is only listening at start
	NewPoll.revents = 0;
	_pollfds.push_back(NewPoll);

	std::cout << "SERVER SOCKET READY!" << std::endl;
}

/* loop checks if any data is incoming on any socket, 
	then decides how to process the data
	->runs forever until signal (ctrl c, ctrl \) or crash
*/
void Server::pollLoop(void)
{
	std::cout << "Starting to poll...\n";

	while (Server::_signal == false) //run until sig received
	{
		// std::cout << "waiting...\n";

		//poll
		//READ: check all sockets for incoming data
		if (poll(&_pollfds[0], _pollfds.size(), -1) == -1 && Server::_signal == false)
			throw std::runtime_error("ERROR: poll() failed.");

		for (size_t i = 0; i < _pollfds.size(); i++)
		{
			if (_pollfds[i].revents == POLLIN) //data to READ
			{
				if (_pollfds[i].fd == _serverSocketFd) //new client wants to connect
					_accept_new_client();
				else
					_receive_data(_pollfds[i].fd); //get data from registered client
			}

			// if (_pollfds[i].revents == POLLOUT) //possible to write
			
			//TODO
		}		
	}
}

/* creates CLIENT class & corresponding socket
*/
void Server::_accept_new_client(void)
{
	std::cout << "accepting new client....\n";

	Client *NewClient = new Client;
	struct pollfd newpollfd;
	struct sockaddr_in clientaddr;

	socklen_t len = sizeof(clientaddr);
	//accept (create new socket)
	int clientfd = accept(_serverSocketFd, (sockaddr *)&clientaddr, &len);
	if (clientfd == -1)
		throw std::runtime_error("ERROR: accepting new connection.");

	//set new socket opt NONBLOCK
	if (fcntl(clientfd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("ERROR: NEW CLIENT setting O_NONBLOCK failed.");

	//create new pollfd for poll()
	newpollfd.fd = clientfd;
	newpollfd.events = POLLIN;
	newpollfd.revents = 0;
	_pollfds.push_back(newpollfd);

	//create new Client instance
	NewClient->fd = clientfd;
	NewClient->ip_address = inet_ntoa(clientaddr.sin_addr); //conv net addr to string
	Clients.insert(std::make_pair(clientfd, NewClient)); //add to client map

	std::cout << "NEW CLIENT ACCEPTED!" << std::endl;
}

/* reads incoming data, passes it to PARSER & HANDLER
*/
void Server::_receive_data(int fd)
{
	char buf[1024];
	memset(buf, 0, sizeof(buf)); //avoid garbage

	ssize_t bytes = recv(fd, &buf, sizeof(buf) - 1, 0);
	
	//check if client disconnected (bytes -1 or 0) 
	if (bytes <= 0 && (errno != EAGAIN && errno != EWOULDBLOCK)) //EAGAIN & EWOULDBLOCK signal try again later -> not real error
		remove_single_client(fd);
	else //process data
	{
		buf[bytes] = '\0';
		std::cout << buf << std::endl;

		//append to client recv buffer
		std::map<int, Client*>::iterator it = Clients.find(fd);
		it->second->recv_buf.append(buf);

		//extract full lines if any
		bool lines_available = true;
		while (lines_available)
		{
			std::string line = it->second->extract_line();
			if (line.empty())
			{
				lines_available = false;
				break;
			}
		
			//PASS to PARSER
			std::cout << "-----> SENDING LINE TO PARSE: " << line << std::endl;
			
			//PASS to HANDLER	
			
		}
	}
}
