#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include "Client.hpp"

class Client;
/*

Methods:
addUser(Client*)
removeUser(Client*)
broadcast(const std::string&, Client* exclude = NULL)
isOperator(Client*)
setOperator(Client*, bool)
checkKey(), checkInvite(), checkLimit()
*/

class Channel
{
	private:
	public:
		Channel();
		~Channel();

		std::string name;
		std::string topic;

		std::set<Client*> users;
		std::set<Client*> operators;

		bool mode_i;      // invite only
		bool mode_t;      // ops control topic
		bool mode_k;      // key mode
		bool mode_l;      // user limit
		std::string key;
		size_t user_limit;

		std::set<Client*> invited_users;

};

#endif