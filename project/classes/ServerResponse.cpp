#include "Server.hpp"
#include "Client.hpp"
#include <iomanip>

void Server::_sendNumeric(Client& c, Numeric code, const std::vector<std::string>& params,
    const std::string& trailing)
{
    std::ostringstream msg;
    msg << ":" << _serverName
        << " " << std::setw(3) << std::setfill('0') << code
        << " " << c.getNickname();

    for (int i = 0; i < params.size(); i++)
        msg << " " << params[i];

    if (!trailing.empty())
        msg << " :" << trailing;

    // check length and truncate here

    msg << "\r\n";
    // send msg.str();
}

void Server::broadcastFromUser(
    const Client& from,
    const std::string& command,
    const std::vector<std::string>& params,
    const std::string& trailing,
    const Channel& channel
)
{
    std::ostringstream msg;

    msg << ":" << from.getNickname()
        << "!" << from.getUsername()
        // << "@" << from.hostname()
        << " " << command;

    for (int i = 0; i < params.size(); i++)
        msg << " " << params[i];

    if (!trailing.empty())
        msg << " :" << trailing;

    // check length and truncate here


    msg << "\r\n";

    // channel.sendToAll(msg.str(), &from);
}


// operator nickname starts with '@'