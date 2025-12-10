#include "../headers/Client.hpp"

void Client::setNickname(const std::string &name)
{
    _nickname = name;
}

void Client::setUsername(const std::string &name)
{
    _username = name;
}

void Client::setRealname(const std::string &name)
{
    _realname = name;
}

bool Client::isAuthenticated() const
{
    return _authenticated;
}

bool Client::isRegistered() const
{
    return _registered;
}

void Client::setAuthenticated(bool v)
{
    _authenticated = v;
}

void Client::setRegistered(bool v)
{
    _registered = v;
}

Client::Client(int fd, const std::string &ip) : _fd(fd), _ip(ip),
                                                _authenticated(false), _registered(false), _nickname(""), _username(""), _realname("")
{
    (void)_fd;
    (void)_ip;
}

Client::~Client()
{
}
