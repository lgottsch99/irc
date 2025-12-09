#include "../headers/CommandHandler.hpp"
#include <iostream>

/*
4.1.1 Password message
    Command: PASS
    Parameters: <password>

    The PASS command is used to set a 'connection password'.  The
    password can and must be set before any attempt to register the
    connection is made.  Currently this requires that clients send a PASS
    command before sending the NICK/USER combination and servers *must*
    send a PASS command before any SERVER command.  The password supplied
    must match the one contained in the C/N lines (for servers) or I
    lines (for clients).  It is possible to send multiple PASS commands
    before registering but only the last one sent is used for
    verification and it may not be changed once registered.
    Numeric Replies:
            ERR_NEEDMOREPARAMS
            ERR_ALREADYREGISTRED
*/
void CommandHandler::handlePass(Client *client, const ParsedCommand &cmd)
{
    std::string password = "password"; // server->getPassword()?
    // std::cout << "the arg[1]: " << cmd.args[0] << std::endl;
    // std::cout << "Result of the password comparison: " << cmd.args[0].compare(password) << std::endl;
    if (!cmd.args[0].compare(password))
        client->setAuthenticated(true);
}

/*
4.1.2 Nick message

    Command: NICK
    Parameters: <nickname> [ <hopcount> ]

    NICK message is used to give user a nickname or change the previous
    one.  The <hopcount> parameter is only used by servers to indicate
    how far away a nick is from its home server.  A local connection has
    a hopcount of 0.  If supplied by a client, it must be ignored.

    If a NICK message arrives at a server which already knows about an
    identical nickname for another client, a nickname collision occurs.
    As a result of a nickname collision, all instances of the nickname
    are removed from the server's database, and a KILL command is issued
    to remove the nickname from all other server's database. If the NICK
    message causing the collision was a nickname change, then the
    original (old) nick must be removed as well.

    If the server recieves an identical NICK from a client which is
    directly connected, it may issue an ERR_NICKCOLLISION to the local
    client, drop the NICK command, and not generate any kills.
*/
// void CommandHandler::handleNick(Client *client, const ParsedCommand &cmd){
// }

void CommandHandler::handleCmd(Client *client, const ParsedCommand &cmd)
{
    if (!cmd.name.compare("PASS"))
        handlePass(client, cmd);
    // else if (!cmd.name.compare("NICK"))
    //     handleNick(client, cmd);

    // will use an interator and find later instead of if-else statements
}

// void CommandHandler::sendToChannel(const std::string &channelName, const std::string &msg, Client *exceptClient)
// {
// }

// void sendToClient(int fd, const std::string& msg);

CommandHandler::CommandHandler() : _server(NULL)
{
}

CommandHandler::CommandHandler(Server *server) : _server(server)
{
    // initialise the map
}

CommandHandler::~CommandHandler()
{
}
