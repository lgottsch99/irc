#include "../headers/CommandHandler.hpp"
#include <iostream>

// ---------------- Member Functions  ----------------

void CommandHandler::handleCmd()
{
    std::cout << "Entered command handler." << std::endl;

    std::map<std::string, handlerFunc>::iterator it = _handlers.find(_cmd.command);

    if (it == _handlers.end())
    {
        std::cout << "Unknown command" << std::endl;
    }
    else
    {
        std::cout << "Command recognised and going to the execution." << std::endl;
        (this->*it->second)();
    }

    // correctlyFormattedMessage = formatMsg(someMessage);
    // server->replyToClient(client, correctlyFormattedMessage);
}

// void CommandHandler::sendToChannel(const std::string &channelName, const std::string &msg, Client *exceptClient)
// {
// }

// void sendToClient(int fd, const std::string& msg);

// ---------------- 4.1 CONNECTION REGISTRATION ----------------

/*
4.1.1 Password message
    Command: PASS <password>

    The password can and must be set before any attempt to register the connection is made.

    Example:
           PASS secretpasswordhere
*/
void CommandHandler::_handlePass()
{
    if (_cmd.params.empty())
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
        std::cout << "Use: PASS <password>" << std::endl;
    }
    else if (_client->isRegistered())
    {
        std::cout << "ERR_ALREADYREGISTRED" << std::endl;
    }
    else if (!_cmd.params[0].compare(_server->getPassword()))
    {
        _client->setAuthenticated(true);
        std::cout << "Authorised successfully with the password from client: " << _cmd.params[0] << std::endl;
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
bool CommandHandler::_isNameDublicate(Server *server, std::string name, bool (CommandHandler::*compareFunc)(Client *, const std::string &))
{
    for (std::map<int, Client *>::iterator it = server->Clients.begin(); it != server->Clients.end(); ++it)
    {
        if (it->second && (this->*compareFunc)(it->second, name))
            return true;
    }
    return false;
}

/*
4.1.2 Nick message
    Command: NICK <nickname>

    NICK message is used to give user a nickname or change the previous one.

    Examples:
        NICK Wiz                        ; Introducing new nick "Wiz".
*/
void CommandHandler::_handleNick()
{
    if (_cmd.params.empty()) // should i check for too many params? or just ignore the rest of the params and take into account only the first one and what about the trailing?
    {
        std::cout << "ERR_NONICKNAMEGIVEN" << std::endl;
    
    else if (_checkNickChars(cmd.params[0])) // check for whitespaces and special characters - should not be present
    {
        std::cout << "ERR_ERRONEUSNICKNAME" << std::endl;
    }
    else if (!client->isAuthenticated())
        std::cout << "Client is not authorised. password first." << std::endl; // theres no error for that in the protocol?
    }
    else if (_isNameDublicate(_server, _cmd.params[0], &CommandHandler::_compareNick))
    {
        std::cout << "ERR_NICKNAMEINUSE" << std::endl;
    }
    else
    {
        _client->setNickname(_cmd.params[0]);
        std::cout << "Set the nickname successfully to: " << _client->getNickname() << std::endl;
    }
}

/*
4.1.3 User message
    Command: USER <username> <hostname> <servername> :<realname>

    Only after both USER and NICK have been received from a client does a user become registered.

    Examples:
        USER guest :Ronnie Reagan   ; User registering themselves with a
                                    username of "guest" and real name
                                    "Ronnie Reagan".
*/
void CommandHandler::_handleUser() // while the NICK command is allowed to change the nickname, can the client change the usernmae and real name?
{
    if (!_client->isAuthenticated())
    {
        std::cout << "Client is not authorised. password first." << std::endl;
    }
    else if (_client->getNickname().empty())
    {
        std::cout << "Set the Nick first." << std::endl;
    }
    else if (_cmd.params.empty() || _cmd.trailing.empty()) // empty realname?
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
        std::cout << "Use: USER <username> <hostname> <servername> :<realname> or just USER <username> :<realname>" << std::endl;
    }
    else if (_isNameDublicate(_server, _cmd.params[0], &CommandHandler::_compareUser))
    {
        std::cout << "ERR_ALREADYREGISTRED" << std::endl;
    }
    else
    {
        _client->setUsername(_cmd.params[0]);
        _client->setRealname(_cmd.trailing);
        _client->setRegistered(true);
        std::cout << "Set the username and registered the client successfully: " << _client->getUsername() << " and " << _client->getRealname() << std::endl;
    }
}

/*
4.1.6 Quit
    Command: QUIT [<Quit message>]

    A client session is ended with a quit message. If a "Quit Message" is given,
    this will be sent instead of the default message, the nickname.

    Examples:
        QUIT :Gone to have lunch        ; Preferred message format.
*/
void CommandHandler::_handleQuit() // should i check for params?
{
    if (!_cmd.trailing.empty())
        std::cout << "Client sent a message before they quit: " << _cmd.trailing << std::endl;
    _server->markClientToDisconnect(_client->fd);
}

// ---------------- 4.2 CHANNEL OPERATIONS ----------------

/*
4.2.1 Join message
    Command: JOIN <channel>{,<channel>} [<key>{,<key>}]

    The JOIN command is used by a client to start listening to a specific channel.

    Examples:
        JOIN #foobar                    ; join channel #foobar.
        JOIN &foo fubar                 ; join channel &foo using key "fubar".
        JOIN #foo,&bar fubar            ; join channel #foo using key "fubar"
                                        and &bar using no key.
        JOIN #foo,#bar fubar,foobar     ; join channel #foo using key "fubar".
                                        and channel #bar using key "foobar".
        JOIN #foo,#bar                  ; join channels #foo and #bar.
*/
void CommandHandler::_handleJoin()
{ // need to add a util that will split a string by commas into vector params
    if (_cmd.params.empty())
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
        std::cout << "Use: JOIN <channel>{,<channel>} [<key>{,<key>}]" << std::endl;
    }
    else if (!_client->isRegistered())
        std::cout << "Client is not registered." << std::endl;
    // validate a channel name
    // ERR_NOSUCHCHANNEL
    else
    {
        std::map<std::string, Channel *>::iterator it = _server->Channels.find(_cmd.params[0]);

        if (it == _server->Channels.end())
        {
            std::cout << "Channel not found - creating a new one: " << _cmd.params[0] << std::endl;
            _server->createChannel(_cmd.params[0]);
            it = _server->Channels.find(_cmd.params[0]);
            it->second->addOperator(_client);
            it->second->addUser(_client);
            _client->addToChannel(it->second);
        }
        else
        {
            if (_client->hasChannel(it->second))
                std::cout << "Client already is in the channel." << std::endl;
            else if (it->second->isInviteOnly() && !_client->isInvited(it->first))
                std::cout << "ERR_INVITEONLYCHAN" << std::endl;
            else if (it->second->hasKey() && ((_cmd.params.size() >= 2 && _cmd.params[1].compare(it->second->getKey())) || _cmd.params.size() < 2))
                std::cout << "ERR_BADCHANNELKEY" << std::endl;
            else if (it->second->getUserLimit() > 0 && it->second->getUserLimit() <= it->second->getNumOfUsers())
                std::cout << "ERR_CHANNELISFULL" << std::endl;
            else
            {
                it->second->addUser(_client);
                _client->addToChannel(it->second);
                std::cout << "RPL_TOPIC" << std::endl;
                std::cout << "RPL_NAMREPLY" << std::endl;
            }
        }
    }
}

/*
4.2.8 Kick command
    Command: KICK <channel> <user> [<comment>]

    The KICK command can be  used  to  forcibly  remove  a  user  from  a
    channel.   It  'kicks  them  out'  of the channel (forced PART).

    Examples:
        KICK &Melbourne Matthew                 ; Kick Matthew from &Melbourne
        KICK #Finnish John :Speaking English    ; Kick John from #Finnish using
                                                "Speaking English" as the reason (comment).
    NOTE:
    It is possible to extend the KICK command parameters to the
    following:
        <channel>{,<channel>} <user>{,<user>} [<comment>]
*/
void CommandHandler::_handleKick()
{
    if (_cmd.params.size() < 2)
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
        std::cout << "Use: KICK <channel> <user> [<comment>]" << std::endl;
        return;
    }

    Channel *channel = _server->getChannel(_cmd.params[0]);

    if (!channel)
        std::cout << "ERR_NOSUCHCHANNEL" << std::endl;
    else if (!channel->hasUser(_client))
        std::cout << "ERR_NOTONCHANNEL - user trying to kick" << std::endl;
    else if (!channel->isOperator(_client))
        std::cout << "ERR_CHANOPRIVSNEEDED" << std::endl;
    else
    {
        Client *kickedClient = _server->getClient(_cmd.params[1]);

        std::cout << "Client bing kicked nick: " << kickedClient->getNickname() << std::endl;

        if (!channel->hasUser(kickedClient))
            std::cout << "ERR_NOTONCHANNEL - user being kicked" << std::endl;
        else
        {
            std::cout << "Removing the user " << kickedClient->getNickname();
            if (!_cmd.trailing.empty())
            {
                std::cout << " with a message: " << _cmd.trailing;
            }
            std::cout << std::endl;
            channel->removeUser(kickedClient);
            kickedClient->leaveChannel(channel);
        }
    }
}

/*
4.2.7 Invite message
    Command: INVITE <nickname> <channel>

    The INVITE message is used to invite users to a channel.

    Examples:
        INVITE Wiz #Twilight_Zone       ; Command to invite WiZ to #Twilight_zone
*/
void CommandHandler::_handleInvite()
{
    if (_cmd.params.size() < 2)
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
        std::cout << "Use: INVITE <nickname> <channel>" << std::endl;
        return;
    }

    Channel *channel = _server->getChannel(_cmd.params[1]);
    Client *invitedClient = _server->getClient(_cmd.params[0]);

    std::cout << "The invited client's nickname: " << _cmd.params[0] << std::endl;
    std::cout << "Found client on the server: " << invitedClient->getNickname() << std::endl;

    if (invitedClient)
    {
        if (channel)
        {
            if (channel->isInviteOnly() && !channel->isOperator(_client))
            {
                std::cout << "ERR_CHANOPRIVSNEEDED" << std::endl;
            }
            else if (!channel->hasUser(_client))
            {
                std::cout << "ERR_NOTONCHANNEL" << std::endl;
            }
            else if (channel->hasUser(invitedClient))
            {
                std::cout << "ERR_USERONCHANNEL" << std::endl;
            }
            else
            {
                std::cout << "RPL_INVITING" << std::endl;
                _client->addInvited(_cmd.params[1]);
            }
        }
        else
        {
            std::cout << "RPL_INVITING" << std::endl;
            _client->addInvited(_cmd.params[1]);
        }
    }
    else
        std::cout << "ERR_NOSUCHNICK" << std::endl;
}

/*
4.2.4 Topic message
    Command: TOPIC <channel> [<topic>]

    The TOPIC message is used to change or view the topic of a channel.

    Examples:
        TOPIC #test :another topic      ;set the topic on #test to "another topic".
        TOPIC #test                     ;check the topic for #test.
*/
void CommandHandler::_handleTopic()
{
    if (_cmd.params.empty())
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
        std::cout << "Use: TOPIC <channel> [:<topic>]" << std::endl;
        return;
    }

    Channel *channel = _server->getChannel(_cmd.params[0]);

    if (channel && _client->hasChannel(channel))
    {
        if (channel->isTopicRestricted() && !channel->isOperator(_client))
            std::cout << "ERR_CHANOPRIVSNEEDED" << std::endl;
        else
        {
            if (!_cmd.trailing.empty()) // only trailing or a param too?
            {
                std::cout << "Setting the topic to a new one: " << _cmd.trailing << std::endl;
                channel->setTopic(_cmd.trailing);
            }
            else
            {
                if (channel->getTopic().empty())
                    std::cout << "RPL_NOTOPIC" << std::endl;
                else
                    std::cout << "RPL_TOPIC: " << channel->getTopic() << std::endl;
            }
        }
    }
    else
        std::cout << "ERR_NOTONCHANNEL or no such channel exists." << std::endl;
}

// ---------------- MODES ----------------

// i - invite-only channel flag
void CommandHandler::_modeInvite(Channel *channel, int signIsPositive) // do i need to check if its already invite-only?
{
    if (signIsPositive)
    {
        channel->setInviteMode(true);
        std::cout << "Set invite-only mode in the channel " << channel->getName() << std::endl;
    }
    else
    {
        channel->setInviteMode(false);
        std::cout << "Removed invite-only mode from the channel " << channel->getName() << std::endl;
    }
}

// t - set the restrictions on the topic modification
void CommandHandler::_modeTopic(Channel *channel, int signIsPositive)
{
    if (signIsPositive)
    {
        channel->setTopicMode(true);
        std::cout << "Restricted setting a topic to operators only in the channel " << channel->getName() << std::endl;
    }
    else
    {
        channel->setTopicMode(false);
        std::cout << "Allowed setting a topic to any participants in the channel " << channel->getName() << std::endl;
    }
}

// k - set a channel key (password)
void CommandHandler::_modeKey(Channel *channel, int signIsPositive) // what if key is already set? we change?
{
    if (signIsPositive)
    {
        channel->setKey(_cmd.params[2]);
        std::cout << "Set a password for the channel " << channel->getName() << std::endl;
    }
    else
    {
        channel->removeKey();
        std::cout << "Removed a password for the channel " << channel->getName() << std::endl;
    }
}

// o - set channel operator privilege
void CommandHandler::_modeOperator(Channel *channel, int signIsPositive)
{
    (void)channel;
    (void)signIsPositive;
    // Client *newOperatorClient = ;

    // if (signIsPositive)
    //     channel->addOperator() else channel->setInviteMode(false);
}

// l - set the user limit to channel
void CommandHandler::_modeLimit(Channel *channel, int signIsPositive) // do we restrict this?
{
    if (signIsPositive)
    {
        if (_cmd.params.size() < 3)
        {
            std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
        }
        else
        {
            unsigned long n = strtoul(_cmd.params[2].c_str(), NULL, 10);

            if (n == 0 || n == ULONG_MAX || errno == ERANGE || n > UINT_MAX)
            {
                std::cout << "Enter a valid number. Must be greater than 0 and smaller than " << UINT_MAX << std::endl;
            }
            else
            {
                channel->setLimit(static_cast<unsigned int>(n));
                std::cout << "Set a user limit in the channel " << channel->getName() << " to " << n << std::endl;
            }
        }
    }
    else
    {
        channel->removeLimit();
        std::cout << "Remove a user limit from the channel " << channel->getName() << std::endl;
    }
}

int _identifySign(char sign)
{
    if (sign == '+')
        return ENABLE;
    else if (sign == '-')
        return DISABLE;
    else
        return INVALID;
}

void CommandHandler::_init_modes()
{
    _modes['i'] = &CommandHandler::_modeInvite;
    _modes['k'] = &CommandHandler::_modeKey;
    _modes['l'] = &CommandHandler::_modeLimit;
    _modes['t'] = &CommandHandler::_modeTopic;
    _modes['o'] = &CommandHandler::_modeOperator;
}

/*
4.2.3 Mode message
    Command: MODE <channel> {[+|-]|o|i|t|l|k} [<limit>] [<user>]

    The MODE command is provided so that channel operators may change the
    characteristics of `their' channel.

    struct Mode {
        char sign;              // '+' or '-'
        char mode;              // 'i', 't', 'k', 'o', 'l'
        std::string argument;   // empty if not required
    };
*/
void CommandHandler::_handleMode() // do we handle multiple modes in one command?
{
    if (_cmd.params.size() < 2)
    {
        std::cout << "ERR_NEEDMOREPARAMS" << std::endl;
        std::cout << "Use: MODE <channel> {[+|-]|o|i|t|l|k} [<limit>] [<user>]" << std::endl;
        return;
    }

    Channel *channel = _server->getChannel(_cmd.params[0]);

    if (channel && _client->hasChannel(channel))
    {
        if (!channel->isOperator(_client))
        {
            std::cout << "ERR_CHANOPRIVSNEEDED" << std::endl;
        }
        else
        {
            int sign = _identifySign(_cmd.params[1][0]);

            if (_cmd.params[1].length() >= 2 && sign != INVALID)
            {
                _init_modes();

                std::map<char, modeFunc>::iterator it = _modes.find(_cmd.params[1][1]);

                if (it == _modes.end())
                    std::cout << "No such mode." << std::endl;
                else
                    (this->*it->second)(channel, sign);
            }
            else
                std::cout << "Wrong format of the parameter." << std::endl;
        }
    }
    else
        std::cout << "ERR_NOTONCHANNEL or no such channel exists." << std::endl; // probbaly need to separate these two errors
}

// ---------------- Constructors ----------------

CommandHandler::CommandHandler(void) : _server(NULL), _client(NULL), _cmd(IrcMessage())
{
    std::cout << "(CommandHandler) Default constructor\n";
}

CommandHandler::CommandHandler(Server *server, Client *client, const IrcMessage &cmd) : _server(server), _client(client), _cmd(cmd)
{
    std::cout << "(CommandHandler) Overload constructor\n";

    _handlers["PASS"] = &CommandHandler::_handlePass;
    _handlers["NICK"] = &CommandHandler::_handleNick;
    _handlers["USER"] = &CommandHandler::_handleUser;
    _handlers["QUIT"] = &CommandHandler::_handleQuit;
    _handlers["JOIN"] = &CommandHandler::_handleJoin;
    _handlers["MODE"] = &CommandHandler::_handleMode;
    _handlers["TOPIC"] = &CommandHandler::_handleTopic;
    _handlers["INVITE"] = &CommandHandler::_handleInvite;
    _handlers["KICK"] = &CommandHandler::_handleKick;
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