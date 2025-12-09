#include "../headers/Client.hpp"

bool Client::isAuthenticated() const
{
    return _authenticated;
}

void Client::setAuthenticated(bool v)
{
    _authenticated = v;
}

Client::Client(int fd, const std::string &ip) : _fd(fd), _ip(ip), _authenticated(false)
{
    (void)_fd;
    (void)_ip;
}

Client::~Client()
{
}
