#include "../headers/CommandHandler.hpp"
#include <iostream>

std::map<std::string, CommandHandler::handlerFunc> CommandHandler::_handlers;

// ---------------- 4.1 CONNECTION REGISTRATION ----------------

/*
4.1.1 Password message
    Command: PASS <password>

    The password can and must be set before any attempt to
    register the connection is made.

    Numeric Replies:
            ERR_NEEDMOREPARAMS
            ERR_ALREADYREGISTRED
    Example:
           PASS secretpasswordhere
*/
void CommandHandler::_handlePass(Server *server, Client *client, const IrcMessage &cmd)
{
    if (cmd.params.empty())
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
    else if (client->isRegistered())
        std::cout << "ERR_ALREADYREGISTRED" << std::endl;
    else if (!cmd.params[0].compare(server->getPassword()))
    {
        client->setAuthenticated(true);
        std::cout << "Authorised successfully with the password from client: " << cmd.params[0] << std::endl;
    }
    else
        std::cout << "Wrong password." << std::endl;
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
        if (it->second && compareFunc(it->second, name))
            return true;
    }
    return false;
}

/*
4.1.2 Nick message
    Command: NICK <nickname>

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
    if (cmd.params.empty()) // should i check for too many params? or just ignore the rest of the params and take into account only the first one and what about the trailing?
        std::cout << "ERR_NONICKNAMEGIVEN" << std::endl;
    // else if () // check for whitespaces and special characters - should not be present
    // {
    //     std::cout << "ERR_ERRONEUSNICKNAME" << std::endl;
    // }
    else if (!client->isAuthenticated())
        std::cout << "Client is not authorised. password first." << std::endl; // theres no error for that in the protocol?
    else if (_isNameDublicate(server, cmd.params[0], &_compareNick))
        std::cout << "ERR_NICKNAMEINUSE" << std::endl;
    else
    {
        client->setNickname(cmd.params[0]);
        std::cout << "Set the nickname successfully: " << client->getNickname() << std::endl;
    }
}

/*
4.1.3 User message
    Command: USER <username> <hostname> <servername> :<realname>

    Only after both USER and NICK have been received from a client does a user become registered.

    Numeric Replies:
            ERR_NEEDMOREPARAMS              ERR_ALREADYREGISTRED
    Examples:
        USER guest :Ronnie Reagan   ; User registering themselves with a
                                    username of "guest" and real name
                                    "Ronnie Reagan".
*/
void CommandHandler::_handleUser(Server *server, Client *client, const IrcMessage &cmd) // while the NICK command is allowed to change the nickname, can the client change the usernmae and real name?
{
    if (!client->isAuthenticated())
        std::cout << "Client is not authorised. password first." << std::endl;
    else if (client->getNickname().empty())
        std::cout << "Set the Nick first." << std::endl;
    else if (cmd.params.empty() || cmd.trailing.empty()) // it didnt work with empty realname
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
    else if (_isNameDublicate(server, cmd.params[0], &_compareUser))
        std::cout << "ERR_ALREADYREGISTRED" << std::endl;
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
    Command: QUIT [<Quit message>]

    A client session is ended with a quit message. If a "Quit
    Message" is given, this will be sent instead of the default message,
    the nickname.

    Examples:
        QUIT :Gone to have lunch        ; Preferred message format.
*/
void CommandHandler::_handleQuit(Server *server, Client *client, const IrcMessage &cmd) // should i check for params?
{
    if (!cmd.trailing.empty())
        std::cout << "Client sent a message before they quit: " << cmd.trailing << std::endl;
    server->markClientToDisconnect(client->fd);
}

// ---------------- 4.2 CHANNEL OPERATIONS ----------------

/*
4.2.1 Join message
    Command: JOIN <channel>{,<channel>} [<key>{,<key>}]

    The JOIN command is used by a client to start listening to a specific
    channel.

    Numeric Replies:
            ERR_NEEDMOREPARAMS
            ERR_INVITEONLYCHAN              ERR_BADCHANNELKEY
            ERR_CHANNELISFULL
            ERR_NOSUCHCHANNEL               ERR_TOOMANYCHANNELS // is there a limit?
            RPL_TOPIC
    Examples:
        JOIN #foobar                    ; join channel #foobar.
        JOIN &foo fubar                 ; join channel &foo using key "fubar".
        JOIN #foo,&bar fubar            ; join channel #foo using key "fubar"
                                        and &bar using no key.
        JOIN #foo,#bar fubar,foobar     ; join channel #foo using key "fubar".
                                        and channel #bar using key "foobar".
        JOIN #foo,#bar                  ; join channels #foo and #bar.
*/
void CommandHandler::_handleJoin(Server *server, Client *client, const IrcMessage &cmd)
{  // need to add a util that will split a string by commas into vector params
    if (cmd.params.empty())
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
    else if (!client->isRegistered())
        std::cout << "Client is not registered." << std::endl;
    // validate a channel name
    // ERR_NOSUCHCHANNEL
    else
    {
        std::cout << "Searching for the channel under the name: " << cmd.params[0] << std::endl;

        std::map<std::string, Channel *>::iterator it = server->Channels.find(cmd.params[0]);

        if (it == server->Channels.end())
        {
            server->createChannel(cmd.params[0]);
            it = server->Channels.find(cmd.params[0]);
            it->second->addOperator(client);
            it->second->addUser(client);
            client->addToChannel(it->second);
        }
        else
        {
            if (client->hasChannel(it->second))
                std::cout << "Client already is in the channel." << std::endl;
            else if (it->second->isInviteOnly())
                std::cout << "ERR_INVITEONLYCHAN" << std::endl;
            else if (it->second->hasKey() && ((cmd.params.size() >= 2 && cmd.params[1].compare(it->second->getKey())) || cmd.params.size() < 2))
                std::cout << "ERR_BADCHANNELKEY" << std::endl;
            else if (it->second->getUserLimit() > 0 && it->second->getUserLimit() <= it->second->getNumOfUsers())
                std::cout << "ERR_CHANNELISFULL" << std::endl;
            else
            {
                it->second->addUser(client);
                client->addToChannel(it->second);
                std::cout << "RPL_TOPIC" << std::endl;
                std::cout << "RPL_NAMREPLY" << std::endl;
            }
        }
    }
}

// i - invite-only channel flag
void CommandHandler::_modeInvite(const IrcMessage &cmd, Channel *channel, int signIsPositive)
{
    (void)cmd;
    if (signIsPositive)
        channel->setInviteMode(true);
    else
        channel->setInviteMode(false);
}

// k - set a channel key (password)
void CommandHandler::_modeKey(const IrcMessage &cmd, Channel *channel, int signIsPositive) // what if key is already set? we change?
{
    if (signIsPositive)
        channel->setKey(cmd.params[2]);
    else
        channel->removeKey();
}

// l - set the user limit to channel
void CommandHandler::_modeLimit(const IrcMessage &cmd, Channel *channel, int signIsPositive) // do we restrict this?
{
    std::cout << "passing to the setLimit(): " << cmd.params[2] << "\nAfter conversion: " << strtoul(cmd.params[2].c_str(), NULL, 10) << std::endl;
    if (signIsPositive)
    {
        unsigned long n = strtoul(cmd.params[2].c_str(), NULL, 10);
        if (n == 0 || n == ULONG_MAX || errno == ERANGE || n > 100000)
            std::cout << "Enter a valid number. The valid range is [1..100,000]." << std::endl;
        else
            channel->setLimit(n);
    }
    else
        channel->removeLimit();
}

int _identifySign(char sign)
{
    std::cout << "identifySign received the following character: " << sign << std::endl;

    if (sign == '+')
        return ENABLE;
    else if (sign == '-')
        return DISABLE;
    else
        return INVALID;
}

/*
4.2.3 Mode message
    Command: MODE <channel> {[+|-]|o|i|t|l|k} [<limit>] [<user>]

    The MODE command is provided so that channel operators may change the
    characteristics of `their' channel.
*/
void CommandHandler::_handleMode(Server *server, Client *client, const IrcMessage &cmd) // do we handle multiple modes in one command?
{
    Channel *channel = server->getChannel(cmd.params[0]);

    if (cmd.params.empty())
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
        return;
    }

    if (channel && client->hasChannel(channel))
    {
        int sign = _identifySign(cmd.params[1][0]);

        if (cmd.params[1].length() >= 2 && sign != INVALID)
        {
            std::map<char, modeFunc> modes;

            modes['i'] = &_modeInvite;
            modes['k'] = &_modeKey;
            modes['l'] = &_modeLimit;

            std::cout << "the mode aka cmd.params[0]: " << cmd.params[0] << "\nthe char aka cmd.params[1][1]: " << cmd.params[1][1] << "\nthe sign aka param[1][0]: " << cmd.params[1][0] << std::endl;

            std::map<char, modeFunc>::iterator it = modes.find(cmd.params[1][1]);

            if (it == modes.end())
                std::cout << "No such mode." << std::endl;
            else
                it->second(cmd, channel, sign);
        }
        else
            std::cout << "Wrong format of the parameter." << std::endl;
    }
    else
        std::cout << "Client has joined no such channel or no such channel exists." << std::endl;
}

/*
4.2.4 Topic message
    Command: TOPIC <channel> [<topic>]

    The TOPIC message is used to change or view the topic of a channel.
    The topic for channel <channel> is returned if there is no <topic>
    given.  If the <topic> parameter is present, the topic for that
    channel will be changed, if the channel modes permit this action.

    Numeric Replies:
            ERR_NEEDMOREPARAMS              ERR_NOTONCHANNEL
            RPL_NOTOPIC                     RPL_TOPIC
            ERR_CHANOPRIVSNEEDED
    Examples:
        :Wiz TOPIC #test :New topic     ;User Wiz setting the topic.
        TOPIC #test :another topic      ;set the topic on #test to "another topic".
        TOPIC #test                     ;check the topic for #test.
*/
void CommandHandler::_handleTopic(Server *server, Client *client, const IrcMessage &cmd)
{
    if (cmd.params.empty())
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
        return;
    }
    (void)server;
    (void)client;
    (void)cmd;
}

/*
4.2.7 Invite message
    Command: INVITE <nickname> <channel>

    The INVITE message is used to invite users to a channel.  The
    parameter <nickname> is the nickname of the person to be invited to
    the target channel <channel>.  There is no requirement that the
    channel the target user is being invited to must exist or be a valid
    channel.  To invite a user to a channel which is invite only (MODE
    +i), the client sending the invite must be recognised as being a
    channel operator on the given channel.

    Numeric Replies:
            ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
            ERR_NOTONCHANNEL                ERR_USERONCHANNEL
            ERR_CHANOPRIVSNEEDED
            RPL_INVITING                    RPL_AWAY
    Examples:
        :Angel INVITE Wiz #Dust         ; User Angel inviting WiZ to channel #Dust
        INVITE Wiz #Twilight_Zone       ; Command to invite WiZ to #Twilight_zone
*/
void CommandHandler::_handleInvite(Server *server, Client *client, const IrcMessage &cmd)
{
    if (cmd.params.empty())
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
        return;
    }
    (void)server;
    (void)client;
    (void)cmd;
}

/*
4.2.8 Kick command
    Command: KICK <channel> <user> [<comment>]

    The KICK command can be  used  to  forcibly  remove  a  user  from  a
    channel.   It  'kicks  them  out'  of the channel (forced PART).

    Only a channel operator may kick another user out of a  channel.
    Each  server that  receives  a KICK message checks that it is valid
    (ie the sender is actually a  channel  operator)  before  removing
    the  victim  from  the channel.

    Numeric Replies:
            ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
            ERR_BADCHANMASK                 ERR_CHANOPRIVSNEEDED
            ERR_NOTONCHANNEL
    Examples:
        KICK &Melbourne Matthew                 ; Kick Matthew from &Melbourne
        KICK #Finnish John :Speaking English    ; Kick John from #Finnish using
                                                "Speaking English" as the reason (comment).
        :WiZ KICK #Finnish John         ; KICK message from WiZ to remove John
                                        from channel #Finnish
    NOTE:
    It is possible to extend the KICK command parameters to the
    following:
        <channel>{,<channel>} <user>{,<user>} [<comment>]
*/
void CommandHandler::_handleKick(Server *server, Client *client, const IrcMessage &cmd)
{
    if (cmd.params.empty())
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
        return;
    }
    (void)server;
    (void)client;
    (void)cmd;
}

/* A channel operator is identified by the '@' symbol next to their
   nickname whenever it is associated with a channel (ie replies to the
   NAMES, WHO and WHOIS commands). */

// ---------------- Member Functions  ----------------

void CommandHandler::handleCmd(Server *server, Client *client, const IrcMessage &cmd)
{
    std::cout << "Entered command handler." << std::endl;
    CommandHandler::_handlers["PASS"] = &_handlePass;
    CommandHandler::_handlers["NICK"] = &_handleNick;
    CommandHandler::_handlers["USER"] = &_handleUser;
    CommandHandler::_handlers["QUIT"] = &_handleQuit;
    CommandHandler::_handlers["JOIN"] = &_handleJoin;
    CommandHandler::_handlers["MODE"] = &_handleMode;

    std::map<std::string, handlerFunc>::iterator it = _handlers.find(cmd.command);

    if (it == _handlers.end())
        std::cout << "Unknown command" << std::endl; // format the message
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
