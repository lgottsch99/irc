#include "../headers/Channel.hpp"
#include <iostream>

// ---------------- Member Functions ----------------

// ---------------- Operations with users ----------------

bool Channel::hasUser(Client *client) const
{
    if (_users.find(client) != _users.end())
        return true;
    return false;
}

bool Channel::isOperator(Client *client) const
{
    if (_operators.find(client) == _operators.end())
        return false;
    return true;
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

std::string Channel::getTopic(void) const
{
    return _topic;
}

bool Channel::isInviteOnly(void) const
{
    return _mode_i;
}

bool Channel::isTopicRestricted() const
{
    return _mode_t;
}

bool Channel::hasKey(void) const
{
    return _mode_k;
}

// ---------------- Setters + Modes ----------------

void Channel::setTopic(const std::string &topic)
{
    _topic = topic;
    _mode_t = true;
}

void Channel::setKey(const std::string &key) // what if the key is already set?
{
    _key = key;
    _mode_k = true;
}

void Channel::setLimit(unsigned int n)
{
    _userLimit = n;
    _mode_l = true;
}

void Channel::setInviteMode(bool v)
{
    _mode_i = v;
}

void Channel::setTopicMode(bool v)
{
    _mode_t = v;
}

void Channel::removeKey(void)
{
    _key.clear();
    _mode_k = false;
}

void Channel::removeLimit(void)
{
    _userLimit = 0;
    _mode_l = false;
}

// ---------------- Constructors ----------------

Channel::Channel(void) : _name("")
{
    std::cout << "(Channel) Default constructor\n";
}

Channel::Channel(const std::string &name) : _name(name)
{
    std::cout << "(Channel) Overload constructor\n";
}

Channel::~Channel(void)
{
    std::cout << "(Channel) Destructor\n";
}
