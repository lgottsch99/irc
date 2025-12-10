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
    command before sending the NICK/USER combination and servers *must*
    send a PASS command before any SERVER command.  The password supplied
    must match the one contained in the C/N lines (for servers) or I
    lines (for clients).  It is possible to send multiple PASS commands
    before registering but only the last one sent is used for
    verification and it may not be changed once registered.

    Numeric Replies:
            ERR_NEEDMOREPARAMS
            ERR_ALREADYREGISTRED

    Example:
           PASS secretpasswordhere
*/
void CommandHandler::_handlePass(Server *server, Client *client, const ParsedCommand &cmd)
{
    if (!cmd.args[0].compare(server->getPassword()))
        client->setAuthenticated(true);
}

// Util to check whether a client with the same name already exists
bool CommandHandler::_isNameDublicate(Server *server, std::string name)
{
    for (std::map<int, Client *>::iterator it = server->Clients.begin(); it != server->Clients.end(); ++it)
    {
        if (it->second && !it->second->getNickname().compare(name))
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
           ERR_NICKNAMEINUSE               ERR_NICKCOLLISION

    Examples:
        NICK Wiz                        ; Introducing new nick "Wiz".
        :WiZ NICK Kilroy                ; WiZ changed his nickname to Kilroy.
*/
void CommandHandler::_handleNick(Server *server, Client *client, const ParsedCommand &cmd)
{
    if (_isNameDublicate(server, cmd.args[0]))
    {
        std::cout << "ERROR: found a client with the same nick" << std::endl; // should be ERR_NICKNAMEINUSE
        return;
    }
    std::cout << "Did not find any name twins" << std::endl; // should be ERR_NICKNAMEINUSE
    client->setNickname(cmd.args[0]);
}

/*
4.1.3 User message

    Command: USER
    Parameters: <username> <hostname> <servername> <realname>

    The USER message is used at the beginning of connection to specify
    the username, hostname, servername and realname of s new user.  It is
    also used in communication between servers to indicate new user
    arriving on IRC, since only after both USER and NICK have been
    received from a client does a user become registered.

    Between servers USER must to be prefixed with client's NICKname.
    Note that hostname and servername are normally ignored by the IRC
    server when the USER command comes from a directly connected client
    (for security reasons), but they are used in server to server
    communication.  This means that a NICK must always be sent to a
    remote server when a new user is being introduced to the rest of the
    network before the accompanying USER is sent.

    It must be noted that realname parameter must be the last parameter,
    because it may contain space characters and must be prefixed with a
    colon (':') to make sure this is recognised as such.

    Since it is easy for a client to lie about its username by relying
    solely on the USER message, the use of an "Identity Server" is
    recommended.  If the host which a user connects from has such a
    server enabled the username is set to that as in the reply from the
    "Identity Server".

    Numeric Replies:
            ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED

    Examples:
        USER guest tolmoon tolsun :Ronnie Reagan
                                    ; User registering themselves with a
                                    username of "guest" and real name
                                    "Ronnie Reagan".

    :testnick USER guest tolmoon tolsun :Ronnie Reagan
                                    ; message between servers with the
                                    nickname for which the USER command
                                    belongs to
*/
void CommandHandler::_handleUser(Server *server, Client *client, const ParsedCommand &cmd)
{
    (void)client;
    (void)cmd;
    (void)server;
}

void CommandHandler::handleCmd(Server* server, Client *client, const ParsedCommand &cmd)
{
    std::cout << "\n--Passed params in the handler--\nName: " << cmd.name << std::endl;
    for (size_t i = 0; i < cmd.args.size(); i++) {
        std::cout << "Args: " << cmd.args[i] << std::endl;
    }

    CommandHandler::_handlers["PASS"] = &_handlePass; // not sure if it should be in this function or outside, later
    CommandHandler::_handlers["NICK"] = &_handleNick;
    CommandHandler::_handlers["USER"] = &_handleUser;

    std::map<std::string, handlerFunc>::iterator it = _handlers.find(cmd.name); // instead of daunting if else if else if else

    if (it == _handlers.end()) {
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
