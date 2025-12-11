#include "../headers/CommandHandler.hpp"
#include <iostream>

std::map<std::string, CommandHandler::handlerFunc> CommandHandler::_handlers;

/*
4.1.1 Password message
    Command: PASS
    Parameters: <password>

    The PASS command is used to set a 'connection password'.  The
    password can and must be set before any attempt to register the
    connection is made.  Currently this requires that clients send a PASS
    command before sending the NICK/USER combination. It is possible to
    send multiple PASS commands before registering but only the last one
    sent is used for verification and it may not be changed once registered.

    Numeric Replies:
            ERR_NEEDMOREPARAMS
            ERR_ALREADYREGISTRED

    Example:
           PASS secretpasswordhere
*/
void CommandHandler::_handlePass(Server *server, Client *client, const ParsedCommand &cmd)
{
    if (cmd.args.empty())
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
    }
    else if (client->isRegistered())
    {
        std::cout << "ERR_ALREADYREGISTRED" << std::endl;
    }
    else
    {
        if (!cmd.args[0].compare(server->getPassword()))
            client->setAuthenticated(true);
    }
}

bool CommandHandler::_compareNick(Client *client, const std::string &name)
{
    return !client->getNickname().compare(name);
}

bool CommandHandler::_compareUser(Client *client, const std::string &name)
{
    return !client->getUsername().compare(name);
}

// Util to check whether a client with the same name already exists
bool CommandHandler::_isNameDublicate(Server *server, std::string name, bool (*compareFunc)(Client *, const std::string &))
{
    for (std::map<int, Client *>::iterator it = server->Clients.begin(); it != server->Clients.end(); ++it)
    {
        // if (it->second && !it->second->getNickname().compare(name))
        //     return true;
        if (it->second && compareFunc(it->second, name))
            return true;
    }
    return false;
}

/*
4.1.2 Nick message

    Command: NICK
    Parameters: <nickname>

    NICK message is used to give user a nickname or change the previous
    one.

    Numeric Replies:
           ERR_NONICKNAMEGIVEN             ERR_ERRONEUSNICKNAME
           ERR_NICKNAMEINUSE

    Examples:
        NICK Wiz                        ; Introducing new nick "Wiz".
*/
void CommandHandler::_handleNick(Server *server, Client *client, const ParsedCommand &cmd)
{
    if (cmd.args.empty()) // should i check for too many params? or just ignore the rest of the params and take into account only the first one
    {                     // and what about the trailing?
        std::cout << "ERR_NONICKNAMEGIVEN" << std::endl;
    }
    // else if () // check for whitespaces and special characters - should not be present
    // {
    //     std::cout << "ERR_ERRONEUSNICKNAME" << std::endl;
    // }
    else if (!client->isAuthenticated())
    {
        std::cout << "Client is not authorised. password first." << std::endl; // theres no error for that in the protocol?
    }
    else if (_isNameDublicate(server, cmd.args[0], &_compareNick))
    {
        std::cout << "ERR_NICKNAMEINUSE" << std::endl;
    }
    else
    {
        client->setNickname(cmd.args[0]);
    }
}

/*
4.1.3 User message

    Command: USER
    Parameters: <username> :<realname>

    The USER message is used at the beginning of connection to specify
    the username, hostname, servername and realname of a new user. Only
    after both USER and NICK have been received from a client does a user become registered.

    It must be noted that realname parameter must be the last parameter,
    because it may contain space characters and must be prefixed with a
    colon (':') to make sure this is recognised as such.

    Numeric Replies:
            ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED

    Examples:
        USER guest :Ronnie Reagan
                                    ; User registering themselves with a
                                    username of "guest" and real name
                                    "Ronnie Reagan".
*/
void CommandHandler::_handleUser(Server *server, Client *client, const ParsedCommand &cmd)
{
    if (cmd.args.empty())
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
    }
    else if (_isNameDublicate(server, cmd.args[0], &_compareUser))
    {
        std::cout << "ERR_ALREADYREGISTRED" << std::endl;
    }
    else
    {
        client->setUsername(cmd.args[0]);
        client->setRealname(cmd.args[1]);
        client->setRegistered(true);
    }
}

void CommandHandler::handleCmd(Server *server, Client *client, const ParsedCommand &cmd)
{
    std::cout << "\n--Passed params in the handler--\nName: " << cmd.name << std::endl;
    for (size_t i = 0; i < cmd.args.size(); i++)
    {
        std::cout << "Args: " << cmd.args[i] << std::endl;
    }

    CommandHandler::_handlers["PASS"] = &_handlePass;
    CommandHandler::_handlers["NICK"] = &_handleNick;
    CommandHandler::_handlers["USER"] = &_handleUser;

    std::map<std::string, handlerFunc>::iterator it = _handlers.find(cmd.name);

    if (it == _handlers.end())
    {
        std::cout << "Unknown command" << std::endl; // use send response with a formatted message
    }

    it->second(server, client, cmd); // execute the handler function

    // correctlyFormattedMessage = formatMsg(someMessage);
    // server->replyToClient(client, correctlyFormattedMessage);
}

// void CommandHandler::sendToChannel(const std::string &channelName, const std::string &msg, Client *exceptClient)
// {
// }

// void sendToClient(int fd, const std::string& msg);

// ---------------- Constructors ----------------

CommandHandler::CommandHandler(void)
{
    std::cout << "(CommandHandler) Default constructor\n";
}

CommandHandler::~CommandHandler(void)
{
    std::cout << "(CommandHandler) Destructor\n";
}
