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

    Example:
           PASS secretpasswordhere
*/
void CommandHandler::handlePass(Client *client, const ParsedCommand &cmd)
{
    std::string password = "password"; // server->getPassword()?
    if (!cmd.args[0].compare(password))
        client->setAuthenticated(true);
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
Client *searchClient(Server *server){
    
    return ;
}

void CommandHandler::handleNick(Client *client, const ParsedCommand &cmd)
{
    // how the fuck?
    client->setNickname(cmd.args[0]);
    // look up existing nicknames - iterate through all clients through the server? 
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
void CommandHandler::handleUser(Client *client, const ParsedCommand &cmd)
{
}

void CommandHandler::handleCmd(Client *client, const ParsedCommand &cmd)
{
    if (!cmd.name.compare("PASS"))
        handlePass(client, cmd);
    else if (!cmd.name.compare("NICK"))
        handleNick(client, cmd);
    else if (!cmd.name.compare("USER"))
        handleUser(client, cmd);

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
