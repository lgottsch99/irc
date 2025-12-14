#include "../headers/Channel.hpp"
#include <iostream>

// ---------------- Member Functions ----------------

// ---------------- Operations with user sets ----------------

bool Channel::hasUser(Client *client) const
{
    if (_users.find(client) != _users.end())
        return true;
    return false;
}

void Channel::addUser(Client *client)
{
    std::cout << "Adding a user to the channel: " << _name << std::endl;
    _users.insert(client);
}
void Channel::removeUser(Client *client)
{
    std::cout << "Removing a user from the channel: " << _name << std::endl;
    _users.erase(client);
}
void Channel::addOperator(Client *client)
{
    std::cout << "Adding an operator to the channel: " << _name << std::endl;
    _operators.insert(client);
}
void Channel::removeOperator(Client *client)
{
    std::cout << "Removing an operator from the channel: " << _name << std::endl;
    _operators.erase(client);
}
void Channel::addInvite(Client *client)
{
    _invited.insert(client);
}
void Channel::removeInvite(Client *client)
{
    _invited.erase(client);
}

bool Channel::isOperator(Client *client) const
{
    if (_operators.find(client) == _operators.end())
        return false;
    return true;
}

bool Channel::isInvited(Client *client) const
{
    if (_invited.find(client) == _invited.end())
        return false;
    return true;
}

// ---------------- Getters ----------------

const std::string &Channel::getName() const
{
    return _name;
}

const std::string &Channel::getKey(void) const
{
    return _key;
}

unsigned int Channel::getUserLimit(void) const
{
    return _userLimit;
}

unsigned int Channel::getNumOfUsers(void) const
{
    return _users.size();
}

// ---------------- Setters + Modes ----------------

void Channel::setInviteMode(bool v)
{
    _mode_i = v;
}

void Channel::setKey(const std::string &key) // what if the key is already set?
{
    _mode_k = true;
    _key = key;
}

void Channel::setLimit(unsigned int n)
{
    std::cout << "Received number in setLimit(): " << n << std::endl;
    _mode_l = true;
    _userLimit = n;
}

bool Channel::isInviteOnly(void) const
{
    return _mode_i;
}

bool Channel::hasKey(void) const
{
    return _mode_k;
}

void Channel::removeKey(void)
{
    _mode_k = false;
    _key.clear();
}


void Channel::removeLimit(void)
{
    _mode_l = false;
    _userLimit = 10000;
}

// ---------------- Constructors ----------------

Channel::Channel(void) : _name("")
{
    std::cout << "(Channel) Default constructor\n";
}

/*  Channels names are strings (beginning with a '&' or '#' character) of
    length up to 200 characters.  Apart from the the requirement that the
    first character being either '&' or '#'; the only restriction on a
    channel name is that it may not contain any spaces (' '), a control G
    (^G or ASCII 7), or a comma (',' which is used as a list item
    separator by the protocol).

    &	LOCAL	            This channel type is known only to the server they are created on,
                            and as such only clients on that server can join it. Each instance
                            of thischannel type is contained per server.
    #	NETWORK             This channel type is known to all servers that are connected to the network.
    !	NETWORK_SAFE        These channels work similar to the '#' channels, except the name is prefixed
                            not only with the '!' but also a set of alpha-numeric digits. This is an
                            alternative to the time-stamp method of avoiding channel-takeovers during network splits.
    +	NETWORK_UNMODERATED These unmoderated channels work almost exactly the same as '#' channels,
                            except nobody can obtain channel operator status on them. Some implementations
                            seem to set the channel mode to '+nt' upon creation, however most
                            implementations act as if modes +nt are set but don't announce them as being set.*/
Channel::Channel(const std::string &name) : _name(name)
{
    std::cout << "(Channel) Overload constructor\n";
}

Channel::~Channel(void)
{
    std::cout << "(Channel) Destructor\n";
}
