#include "../headers/Channel.hpp"

Channel::Channel(void)
{
	std::cout << "(Channel) Default constructor\n";

}
Channel::~Channel()
{
	std::cout << "(Channel) Destructor\n";

	//close all open sockets / fds

}
