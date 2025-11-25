#include "../headers/Server.hpp"

//----------- Constructors ------------
//default
Server::Server(char *argv[])
{
	std::cout << "(Server) Default constructor\n";

	init_serv(argv);
}


//destructor
Server::~Server()
{
	std::cout << "(Server) Destructor\n";
}


// -------------- Exceptions --------------

char const *Server::PortInvalidException::what(void) const throw()
{
	   return ("Port invalid!\n");
};
char const *Server::PwInvalidException::what(void) const throw()
{
	   return ("Password invalid!\n");
};


//----------- Member Fts ------------

/* checks args to be valid and initiates port & pw
throws exc: in case of invalid pw / invalid port

(total port range on linux is 0-65535, but check (ephemeral range = usable range) on school comp with 
cat /proc/sys/net/ipv4/ip_local_port_range
)
*/
void Server::init_serv(char *argv[])
{
	std::string port = argv[1];
	std::string pw = argv[2];
	int	port_num;

	std::cout << "Port: " << port << std::endl;
	std::cout << "Pw: " << pw << std::endl;

	//make sure port number is valid
		//only digits
		//in valid range
	if (only_digits(port))
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
			throw PortInvalidException();
	}
	else
		throw PortInvalidException();
		
	//save password
		//check not empty? 				TODO maybe more pw rules??
	if (pw.length() >= 1)
		_password = pw;
	else
		throw PwInvalidException();

}


bool Server::only_digits(std::string str)
{
	for (int i = 0; i < (int)str.length(); i++)
	{
		if (std::isdigit(str[i]) == 0)
			return false;
	}
	return true;
}