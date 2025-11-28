#include "../headers/Client.hpp"


Client::Client(void)
{
	std::cout << "(Client) Default constructor\n";
	fd = -1;

	registered = false;
	has_sent_pass = false;
	has_sent_nick = false;
	has_sent_user = false;

}
Client::~Client()
{
	std::cout << "(Client) Destructor\n";

	//close all open sockets / fds

}
