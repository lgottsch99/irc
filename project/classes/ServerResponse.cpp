#include "Server.hpp"
#include "Client.hpp"
#include <iomanip>

void Server::sendNumeric(Client *c, Numeric code, const std::vector<std::string> &params, const std::string &trailing)
{
    std::ostringstream msg;
    msg << ":" << _serverName
        << " " << std::setw(3) << std::setfill('0') << code
        << " " << (c->getNickname().empty() ? c->getNickname() : "*");

    for (size_t i = 0; i < params.size(); i++)
        msg << " " << params[i];

    if (!trailing.empty())
        msg << " :" << trailing;

    // check length and truncate here

    msg << "\r\n";
    // send msg.str();
    replyToClient(c, msg.str());
}

void Server::_broadcastFromUser(
    const Client *from,
    const std::string &command,
    const std::vector<std::string> &params,
    const std::string &trailing,
    const Channel *channel) // i can just pass the whole class IrcMessage instead of its parts separately to make the prototype simpler
{
    (void)channel;
    std::ostringstream msg;

    msg << ":" << from->getNickname()
        << "!" << from->getUsername()
        // << "@" << from.hostname()
        << " " << command;

    for (size_t i = 0; i < params.size(); i++)
        msg << " " << params[i];

    if (!trailing.empty())
        msg << " :" << trailing;

    // check length and truncate here

    msg << "\r\n";

    // channel.sendToAll(msg.str(), &from);
}

// operator nickname starts with '@'