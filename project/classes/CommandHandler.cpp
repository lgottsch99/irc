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
void CommandHandler::_handlePass(Server *server, Client *client, const IrcMessage &cmd)
{
    if (cmd.params.empty())
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
    }
    else if (client->isRegistered())
    {
        std::cout << "ERR_ALREADYREGISTRED" << std::endl;
    }
    else if (!cmd.params[0].compare(server->getPassword()))
    {
        client->setAuthenticated(true);
        std::cout << "Authorised successfully with the password from client: " << cmd.params[0] << std::endl;
    }
    else
    {
        std::cout << "Wrong password." << std::endl;
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
        if (it->second && it->second->isRegistered() && compareFunc(it->second, name))
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
void CommandHandler::_handleNick(Server *server, Client *client, const IrcMessage &cmd)
{
    if (cmd.params.empty()) // should i check for too many params? or just ignore the rest of the params and take into account only the first one
    {                       // and what about the trailing?
        std::cout << "ERR_NONICKNAMEGIVEN" << std::endl;
    }
    else if (_checkNickChars(cmd.params[0])) // check for whitespaces and special characters - should not be present
    {
        std::cout << "ERR_ERRONEUSNICKNAME" << std::endl;
    }
    else if (!client->isAuthenticated())
    {
        std::cout << "Client is not authorised. password first." << std::endl; // theres no error for that in the protocol?
    }
    else if (_isNameDublicate(server, cmd.params[0], &_compareNick))
    {
        std::cout << "ERR_NICKNAMEINUSE" << std::endl;
    }
    else
    {
        client->setNickname(cmd.params[0]);
        std::cout << "Set the nickname successfully: " << client->getNickname() << std::endl;
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
void CommandHandler::_handleUser(Server *server, Client *client, const IrcMessage &cmd) // while the NICK command is allowed to change the nickname, can the client change the usernmae and real name?
{
    if (cmd.params.empty() || cmd.trailing.empty()) // it didnt work with empty realname
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
    }
    else if (_isNameDublicate(server, cmd.params[0], &_compareUser))
    {
        std::cout << "ERR_ALREADYREGISTRED" << std::endl;
    }
    else
    {
        client->setUsername(cmd.params[0]);
        client->setRealname(cmd.trailing);
        client->setRegistered(true);
        std::cout << "Set the username and registered the client successfully: " << client->getUsername() << " and " << client->getRealname() << std::endl;
    }
}

/*
4.1.6 Quit

        Command: QUIT
    Parameters: [<Quit message>]

    A client session is ended with a quit message.  The server must close
    the connection to a client which sends a QUIT message. If a "Quit
    Message" is given, this will be sent instead of the default message,
    the nickname.

    If, for some other reason, a client connection is closed without  the
    client  issuing  a  QUIT  command  (e.g.  client  dies and EOF occurs
    on socket), the server is required to fill in the quit  message  with
    some sort  of  message  reflecting the nature of the event which
    caused it to happen.

    Examples:
        QUIT :Gone to have lunch        ; Preferred message format.
*/
void CommandHandler::_handleQuit(Server *server, Client *client, const IrcMessage &cmd) // should i check for params?
{
    if (!cmd.trailing.empty())
    {
        std::cout << "Client sent a message: " << cmd.trailing << std::endl;
    }
    server->markClientToDisconnect(client->fd);
}

void CommandHandler::handleCmd(Server *server, Client *client, const IrcMessage &cmd)
{
    CommandHandler::_handlers["PASS"] = &_handlePass;
    CommandHandler::_handlers["NICK"] = &_handleNick;
    CommandHandler::_handlers["USER"] = &_handleUser;
    CommandHandler::_handlers["QUIT"] = &_handleQuit;

    std::map<std::string, handlerFunc>::iterator it = _handlers.find(cmd.command);

    if (it == _handlers.end())
    {
        std::cout << "Unknown command" << std::endl; // format the message
    }
    else
    {
        std::cout << "Command recognised and going to the execution." << std::endl;
        it->second(server, client, cmd); // executes the handler function
    }

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


bool CommandHandler::_checkNickChars(const std::string &name)
{
    // the nick should not start with digits or hyphens or a slash
    if (isdigit(static_cast<unsigned char>(name[0])) ||
    name[0] == '-' || name[0] == '/')
        return true;
    // allowed chars: A-Z, a-z, 0-9, [ ] \ ^ _ { | } -
    std::string specialChars = "[]\\^_{|}-";
    for (int i = 0; name[i]; i++)
    {
        if (!(isalnum(name[i]) || specialChars.find(name[i]) != name.npos) || isspace(name[i]))
            return true;
    }
    // the nick should not be longer than NICK_MAX
    if (name.length() > NICK_MAX)
        return true;
    return false;      
}