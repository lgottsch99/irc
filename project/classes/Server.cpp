#include "../headers/Server.hpp"

//----------- Constructors ------------
bool Server::_signal_received = false;

//default
Server::Server(): commandHandler(this)
{
	std::cout << "(Server) Default constructor\n";

	_ServerName = "my.local.irc.server";
	_ServerSocketFd = -1;


}


//destructor
Server::~Server()
{
	std::cout << "(Server) Destructor\n";

	//close all open sockets / fds

}




//----------- Member Fts ------------


void Server::_check_port_pw(char *argv[])
/* checks args to be valid and initiates port & pw
throws exc: in case of invalid pw / invalid port

(total port range on linux is 0-65535, but check (ephemeral range = usable range) on school comp with 
cat /proc/sys/net/ipv4/ip_local_port_range
mostly usable range: 1023-65535
)
*/
{
	std::string port = argv[1];
	std::string pw = argv[2];
	int	port_num;

	std::cout << "Port: " << port << std::endl;
	std::cout << "Pw: " << pw << std::endl;

	//make sure port number is valid
		//only digits
		//in valid range
	if (_only_digits(port))
	{
		std::cout << "Port only digits\n";
		//convert to int
		std::stringstream ss;

		ss << port; //insert str into ss
		ss >> port_num; //extract numeric value from ss
		std::cout << "port_num: " << port_num << std::endl;
		
		if (port_num >= 1 && port_num <= 65535)
			_port_num = port_num;
		else
			throw (std::runtime_error("Server Port number invalid!")); //PortInvalidException();
	}
	else
		throw (std::runtime_error("Server Port number invalid!")); //PortInvalidException();
		
	//save password
		//check not empty? 				TODO maybe more pw rules??
	if (pw.length() >= 1)
		_password = pw;
	else
		throw  (std::runtime_error("Server Password invalid!")); //PwInvalidException();

}


bool Server::_only_digits(std::string str)
{
	for (int i = 0; i < (int)str.length(); i++)
	{
		if (std::isdigit(str[i]) == 0)
			return false;
	}
	return true;
}



void Server::startup(char *argv[])
/* checks port&password
then creates listening socket and enters server loop
*/
{
	_check_port_pw(argv);

	//create listening socket (AF_INET : using IPv4 protocol; SOCK_STREAM: TCP socket (not udp))
	_ServerSocketFd = socket(AF_INET, SOCK_STREAM, 0); //TODO close fd
	if (_ServerSocketFd == -1)
	{	
		throw (std::runtime_error("Error: Could create Server Socket"));
	}


	//define server address (htons(): Converts port to network byte order. INADDR_ANY: Accept connections on any IP.)
	/*Before a server can accept connections, it must tell the OS where to listen. This is done by filling out an address structure.
	https://man7.org/linux/man-pages/man3/sockaddr.3type.html 
	*/
	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(_port_num); //host to network short, Computers store multi-byte data (like port numbers) using a specific byte order (e.g., Little-Endian on x86 processors). Network protocols use a standard Network Byte Order (Big-Endian) to ensure data is interpreted correctly across different machine types. Purpose: It converts the port number (a 16-bit "short" integer) from the machine's native host byte order to the required network byte order.
	serverAddress.sin_addr.s_addr = INADDR_ANY; //constant represents the IP address 0.0.0.0 ,  tells the socket to listen for connections on all network interfaces (all available IP addresses) on the host machine.

	//TODO nonblocking, reusing address
	int en = 1; //bool flag to actually SET the socket option. if 0 we disable the option
	if (setsockopt(_ServerSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) //-> set the socket option (SO_REUSEADDR) to immediately reuse the address, helpful for dev
		throw(std::runtime_error("ERROR: failed to set option (SO_REUSEADDR) on socket"));
	
	if (fcntl(_ServerSocketFd, F_SETFL, O_NONBLOCK) == -1) //-> set the socket option (O_NONBLOCK) for non-blocking socket
		throw(std::runtime_error("ERROR: failed to set option (O_NONBLOCK) on socket"));
	
		/* connects the socket file descriptor created in Step 1 to the specific address (IP and Port) defined in sockaddr_in
	pivotal step where the OS formally reserves the specified port (_port_num) on the machine's network interface(s) and associates it with the serverSocket handle.
	*/
	if (bind(_ServerSocketFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
	{	
		throw (std::runtime_error("Error: Could not bind Server Socket"));
	}

	/* hanges the state of the socket from a general endpoint to a passive listening
	*/
	if (listen(_ServerSocketFd, SOMAXCONN) == -1) //SOMAXCONN constant representing the maximum allowed backlog for your system
	{	
		throw (std::runtime_error("Error: Server could not start listening"));
	}

	/* pollfd struct for serversocket, struct used for monitoring file descriptors for I/O events. 
	It’s commonly employed with the poll() system call to perform multiplexed I/O, allowing a program to efficiently wait for events on
	multiple file descriptors simultaneously without having to resort to blocking I/O operations.
	*/
	struct pollfd NewPoll;
	NewPoll.fd = _ServerSocketFd;
	NewPoll.events = POLLIN; //Input -> set the event to POLLIN for reading data
	NewPoll.revents = 0; //Output -> set to 0 before we call poll(), so that we get actual listening stage
	_poll_fds.push_back(NewPoll); //-> add the server socket to the pollfd at index 0
	

	std::cout << "---- SERVER listening on port " << _port_num << std::endl;


}


void Server::poll_loop(void)
{
	while (_signal_received == false) // run server until shutdown sig received
	{

		//poll
		if ((poll(&_poll_fds[0], _poll_fds.size(), -1) == -1) && _signal_received == false)
			throw (std::runtime_error("ERROR: poll failed."));

		//check all fds
		for (int i = 0; i < (int)_poll_fds.size(); i++)
		{
			if (_poll_fds[i].revents & POLLIN) // revents is bitmask, can have multiple states at once, & checks if POLLIN bit is part of bitmask
			{
				if (_poll_fds[i].fd == _ServerSocketFd) // server socket has new connection request
					accept_new_client();
				else	//client is talking
					receive_new_data(_poll_fds[i].fd);
			}
		}

	}
	//close all fds
	server_shutdown();

}

void Server::accept_new_client(void)
{
	std::cout << "in accepting new client..." << std::endl;

	//create new client
	Client NewClient;
	struct pollfd NewPoll;
	struct sockaddr_in client_addr; //needed bc accept() needs a place to write addr info
	socklen_t len = sizeof(client_addr);

	//accept new client socket, accept()creates new socket&fd
	int newfd = accept(_ServerSocketFd, (sockaddr *)&(client_addr), &len);
	if (newfd == -1)
	{
		std::cout << "Accept() failed" << std::endl;
		return;
	}

	if (fcntl(newfd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "setting new socket option fcntl() failed" << std::endl;
		close(newfd);
		return;
	}

	//add new client to pollfd
	NewPoll.fd = newfd;
	NewPoll.events = POLLIN; // we want to read data on server
	NewPoll.revents = 0;

	NewClient.fd = newfd; //TODO maybe make private +getters/setters laetr
	NewClient.ip_add = inet_ntoa(client_addr.sin_addr); //convert the ip address to string


	//add client to _client_vec
	_client_vec.push_back(NewClient);
	//add fd to _poll_fds
	_poll_fds.push_back(NewPoll);

	std::cout << "New client connected" << std::endl;
}

void Server::receive_new_data(int client_socket_fd)
{

	//create buffer for data
	char buf[1024];
	//clear buffer data (might be garbage in it, better to reset)
	memset(buf, 0, sizeof(buf));

	//recv()
	ssize_t bytes = recv(client_socket_fd, buf, sizeof(buf) - 1, 0); //buf -1 to reserve space for \0

	// get Client to client_fd
	Client* client_ptr = NULL;
	for (int i = 0; i < (int)_client_vec.size(); i++)
	{
		if (_client_vec[i].fd == client_socket_fd)
		{	
			client_ptr = &_client_vec[i];
			break;
		}
	}

	//check if client disconnected 
	//if yes clear client and close fd
	if (bytes <= 0)
	{
		std::cout << "Client disconnected." << std::endl;
		//TODO clear Client in _client_vec, close its fd
		//.... 

		close(client_socket_fd);
	}
	else 	//else process data 
	{
		buf[bytes] = '\0';
		std::cout << "Client " << client_socket_fd << " data: \n" << buf << std::endl;

		//append buf to Client.recv_buffer
		client_ptr->recv_buffer.append(buf);

		std::cout << "\nfull recv buffer:\n" << client_ptr->recv_buffer << std::endl;

		//check if full line received (could be mult. cmds at once)
		std::string &buffer = client_ptr->recv_buffer;
		size_t pos;
        while ((pos = buffer.find("\r\n")) != std::string::npos)
        {
            std::string line = buffer.substr(0, pos);
            buffer.erase(0, pos + 2);

            if (line.empty())
                continue;

            std::cout << "Line extracted: " << line << std::endl;

            // --- CONNECTION POINT: CAP LOGIC HERE ---
            // A minimal parser can just check for the start of the command
            if (line.rfind("CAP LS", 0) == 0) // Check if line starts with "CAP LS"
            {
                std::cout << "Handling CAP LS..." << std::endl;
                
                // 1. Send the minimal CAP LS response (no capabilities supported)
                // This informs the client the server supports CAP but offers nothing.
                // Format: :<server_name> CAP * LS :
                std::string cap_ls_response = ":" + _ServerName + " CAP * LS :";
                send_response(client_socket_fd, cap_ls_response);

                // 2. Send the CAP END response
                // This is CRITICAL. It tells the client to STOP CAP negotiation
                // and proceed with the traditional PASS/NICK/USER registration.
                // Format: CAP * END
                send_response(client_socket_fd, "CAP * END");

                // You might also want to set a flag on the client
                // indicating that CAP negotiation is finished.
                client_ptr->cap_negotiated = true; // Requires adding this member to your Client class
            }
            // else if (line.rfind("CAP", 0) == 0) { ... handle other CAP commands ... }
            else
            {
				//CONNECTION POINT TO IRC INTERNAL LOGIC
				
                // This is where your main command parser will go
                CommandParser parser;
                // ParsedCommand cmd = parser.parse(line);
                // commandHandler.execute(client_ptr, cmd);
            }
        }
    }
	// 	size_t pos;
	// 	while ((pos = buffer.find("\r\n")) != std::string::npos)
	// 	{
	// 		// Extract one full IRC message
	// 		std::string line = buffer.substr(0, pos);
			
	// 		// Remove that line from buffer (remove \r\n as well)
	// 		buffer.erase(0, pos + 2);

	// 		// Empty lines should be ignored (IRC clients sometimes send them)
	// 		if (line.empty())
	// 			continue;
		
	// 		std::cout << "Line extracted: " << line << std::endl;
			
			
	// 		//TODO   Process command "line"
	// 		CommandParser parser;
	// 		// ParsedCommand cmd = parser.parse(line); //TODO
	// 		// commandHandler.execute(client_ptr, cmd); //TODO

	// 	}


	// 	//CONNECTION POINT TO IRC INTERNAL LOGIC
	// 		//append msg to client buf
	// 		//check& extract full line
	// 		// handle command
	// 		/*When a full line is received:
	// 			ParsedCommand cmd = parser.parse(line);
	// 			commandHandler.execute(client, cmd); //CONNECTION FUNCTION TO INTERNAL IRC LOGIC
	// 		*/

	// }

}



void Server::server_shutdown(void)
{
	//close all fds
	for (int i = 0; i < (int)_poll_fds.size(); i++)
	{
		close(_poll_fds[i].fd);
	}
}

// Server::send_response - A conceptual helper function to send data to a client
void Server::send_response(int client_socket_fd, const std::string& message)
{
    // IRC responses MUST end with \r\n
    std::string full_response = message + "\r\n";
    send(client_socket_fd, full_response.c_str(), full_response.length(), 0);
}