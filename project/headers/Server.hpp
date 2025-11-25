#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <stdlib.h> 
#include <sstream>
#include <cctype> //is digit


class Server {
	private:
		int 		_port_num;
		std::string	_password;

		void init_serv(char *argv[]);
		bool only_digits(std::string str);
	
	public:
		Server(char *argv[]); //constructor
		~Server(); //destructor


	class PortInvalidException : public std::exception 
	{
		public:
			const char * what(void) const throw();
	};
	class PwInvalidException : public std::exception 
	{
		public:
			const char * what(void) const throw();
	};


};

#endif