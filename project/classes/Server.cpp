#include "../headers/Server.hpp"

//----------- Constructors ------------
//default
Server::Server(char *argv[])
{
	std::cout << "(Server) Default constructor\n";
	_ServerSocketFd = -1;
	_check_port_pw(argv);
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



void Server::start(void)
/* creates listening socket and enters server loop
*/
{
	//create listening socket (AF_INET : using IPv4 protocol; SOCK_STREAM: TCP socket (not udp))
	_ServerSocketFd = socket(AF_INET, SOCK_STREAM, 0); //TODO close fd
	if (_ServerSocketFd == -1)
	{	
		throw (std::runtime_error("Error: Could create Server Socket"));
		return;
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
	int en = 1;
	if (setsockopt(_ServerSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1) //-> set the socket option (SO_REUSEADDR) to reuse the address
		throw(std::runtime_error("faild to set option (SO_REUSEADDR) on socket"));
	/* connects the socket file descriptor created in Step 1 to the specific address (IP and Port) defined in sockaddr_in
	pivotal step where the OS formally reserves the specified port (_port_num) on the machine's network interface(s) and associates it with the serverSocket handle.
	*/
	if (bind(_ServerSocketFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
	{	
		throw (std::runtime_error("Error: Could not bind Server Socket"));
		return;
	}

	/* hanges the state of the socket from a general endpoint to a passive listening
	*/
	if (listen(_ServerSocketFd, 5) == -1)
	{	
		throw (std::runtime_error("Error: Server could not start listening"));
		return;
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