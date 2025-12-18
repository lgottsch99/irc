#include "../headers/CommandHandler.hpp"
#include <iostream>

// ---------------- Member Functions  ----------------

void CommandHandler::handleCmd()
{
    std::cout << "Entered command handler." << std::endl;
    std::map<std::string, handlerFunc>::iterator it = _handlers.find(_cmd.command);

    if (it == _handlers.end())
        _server->sendNumeric(_client, ERR_UNKNOWNCOMMAND, _cmd.params, "Unknown command");
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

bool CommandHandler::_tryRegister()
{
    if (_client->isAuthenticated())
    {
        if (!_client->getNickname().empty())
        {
            if (!_client->getUsername().empty())
                return true;
        }
    }
    return false;
}

/*
4.1.1 Password message
    Command: PASS <password>

    The password can and must be set before any attempt to register the connection is made.

    Example:
           PASS secretpasswordhere
*/
void CommandHandler::_handlePass() // no numeric on success
{
    if (_cmd.params.empty())
        _server->sendNumeric(_client, ERR_NEEDMOREPARAMS, _cmd.params, "Not enough parameters");
    else if (_client->isRegistered())
        _server->sendNumeric(_client, ERR_ALREADYREGISTERED, _cmd.params, "You may not reregister");
    else if (_cmd.params[0] == _server->getPassword())
    {
        _client->setAuthenticated(true);
        std::cout << "Authorised successfully with the password from client: " << _cmd.params[0] << std::endl;
    }
    else
    {
        _client->setAuthenticated(false);
        _server->sendNumeric(_client, ERR_PASSWDMISMATCH, _cmd.params, "Password incorrect");
    }
}

/*
4.1.2 Nick message
    Command: NICK <nickname>

    NICK message is used to give user a nickname or change the previous one.

    Examples:
        NICK Wiz                        ; Introducing new nick "Wiz".
*/
void CommandHandler::_handleNick() // no numeric on success
{
    if (_cmd.params.empty())
        _server->sendNumeric(_client, ERR_NONICKNAMEGIVEN, _cmd.params, "No nickname given");
    else if (_checkNickChars(_cmd.params[0]))
        _server->sendNumeric(_client, ERR_ERRONEUSNICKNAME, _cmd.params, "Erroneus nickname");
    else if (_isNameDublicate(_server, _cmd.params[0], &CommandHandler::_compareNick))
        _server->sendNumeric(_client, ERR_NICKNAMEINUSE, _cmd.params, "Nickname is already in use");
    else
    {
        if (!_client->getNickname().empty())
            _sendToAllChannels("User changed their nickname");
        _client->setNickname(_cmd.params[0]);
        std::cout << "Set the nickname successfully to: " << _client->getNickname() << std::endl;
    }
    if (!_client->isRegistered() && _tryRegister())
    {
        _client->setRegistered(true);
        _server->sendNumeric(_client, RPL_WELCOME, _cmd.params, "Welcome to the server!");
    }
}

void CommandHandler::_sendToAllChannels(const std::string &trailing)
{
    std::set<Channel *> channels = _client->getChannels();

    for (std::set<Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
        _server->_broadcastFromUser(_client, _cmd.command, _cmd.params, trailing, *it);
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
void CommandHandler::_handleUser()
{
    if (_client->isRegistered())
        _server->sendNumeric(_client, ERR_ALREADYREGISTERED, _cmd.params, "You may not reregister");
    else if (_cmd.params.empty() || _cmd.trailing.empty())
        _server->sendNumeric(_client, ERR_NEEDMOREPARAMS, _cmd.params, "Not enough parameters");
    else
    {
        _client->setUsername(_cmd.params[0]);
        _client->setRealname(_cmd.trailing);
        std::cout << "Set the username: " << _client->getUsername() << " and realname: " << _client->getRealname() << std::endl;
    }
    if (!_client->isRegistered() && _tryRegister())
    {
        _client->setRegistered(true);
        _server->sendNumeric(_client, RPL_WELCOME, _cmd.params, "Welcome to the server!");
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
void CommandHandler::_handleQuit()
{
    if (!_cmd.trailing.empty())
        _sendToAllChannels(_cmd.trailing);
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
        _server->sendNumeric(_client, ERR_NEEDMOREPARAMS, _cmd.params, "Not enough parameters");
    else if (!_client->isRegistered())
        _server->sendNumeric(_client, ERR_NOTREGISTERED, _cmd.params, "You have not registered");
    else if ((_cmd.params[0][0] != '#' && _cmd.params[0][0] != '&') || _cmd.params[0].length() > 200)
        _server->sendNumeric(_client, ERR_NOSUCHCHANNEL, _cmd.params, "No such channel"); // not sure about the response
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
        else if (!_client->hasChannel(it->second))
        {
            if (it->second->isInviteOnly() && !_client->isInvited(it->first))
                _server->sendNumeric(_client, ERR_INVITEONLYCHAN, _cmd.params, "Cannot join channel (+i)");
            else if (it->second->hasKey())
            {
                if ((_cmd.params.size() >= 2 && _cmd.params[1] != it->second->getKey()) || _cmd.params.size() < 2)
                    _server->sendNumeric(_client, ERR_BADCHANNELKEY, _cmd.params, "Cannot join channel (+k)");
            }
            else if (it->second->getUserLimit() > 0 && it->second->getUserLimit() <= it->second->getNumOfUsers())
                _server->sendNumeric(_client, ERR_CHANNELISFULL, _cmd.params, "Cannot join channel (+l)");
            else
            {
                it->second->addUser(_client);
                _client->addToChannel(it->second);

                if (!it->second->getTopic().empty())
                    _server->sendNumeric(_client, RPL_TOPIC, _cmd.params, it->second->getTopic());
                else
                    _server->sendNumeric(_client, RPL_NOTOPIC, _cmd.params, "No topic is set");
                // implement NAMES command
                _server->sendNumeric(_client, RPL_NAMREPLY, _cmd.params, "nicknames");
                _server->sendNumeric(_client, RPL_ENDOFNAMES, _cmd.params, "End of /NAMES list");
            }
        }
    }
}

/*
4.2.5 Names message
    Command: NAMES [<channel>{,<channel>}]

    By using the NAMES command, a user can list all nicknames that are
    visible to them on any channel that they can see.  Channel names
    which they can see are those which aren't private (+p) or secret (+s)
    or those which they are actually on.  The <channel> parameter
    specifies which channel(s) to return information about if valid.
    There is no error reply for bad channel names.

    If no <channel> parameter is given, a list of all channels and their
    occupants is returned.  At the end of this list, a list of users who
    are visible but either not on any channel or not on a visible channel
    are listed as being on `channel' "*".

    Numerics:
            RPL_NAMREPLY                    RPL_ENDOFNAMES
    Examples:
    NAMES #twilight_zone,#42        ; list visible users on #twilight_zone
                                    and #42 if the channels are visible to
                                    you.
    NAMES                           ; list all visible channels and users
*/

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
        _server->sendNumeric(_client, ERR_NEEDMOREPARAMS, _cmd.params, "Not enough parameters");
    else
    {
        Channel *channel = _server->getChannel(_cmd.params[0]);

        if (!channel)
            _server->sendNumeric(_client, ERR_NOSUCHCHANNEL, _cmd.params, "No such channel");
        else if (!channel->hasUser(_client))
            _server->sendNumeric(_client, ERR_NOTONCHANNEL, _cmd.params, "You're not on that channel");
        else if (!channel->isOperator(_client))
            _server->sendNumeric(_client, ERR_CHANOPRIVSNEEDED, _cmd.params, "You're not channel operator");
        else
        {
            Client *kickedClient = _server->getClient(_cmd.params[1]);

            if (!kickedClient)
                _server->sendNumeric(_client, ERR_NOSUCHNICK, _cmd.params, "No such nick/channel");
            else if (!channel->hasUser(kickedClient))
                _server->sendNumeric(_client, ERR_USERNOTINCHANNEL, _cmd.params, "They aren't on that channel");
            else
            {
                // if (!_cmd.trailing.empty())
                //     sendToChannel(_cmd.trailing);
                // else
                //     sendToChannel("");

                channel->removeUser(kickedClient);
                if (channel->isOperator(kickedClient))
                    channel->removeOperator(kickedClient);
                kickedClient->leaveChannel(channel);
            }
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
        _server->sendNumeric(_client, ERR_NEEDMOREPARAMS, _cmd.params, "Not enough parameters");
    else
    {
        Client *invitedClient = _server->getClient(_cmd.params[0]);
        Channel *channel = _server->getChannel(_cmd.params[1]);

        if (!invitedClient)
            _server->sendNumeric(_client, ERR_NOSUCHNICK, _cmd.params, "No such nick/channel");
        if (channel)
        {
            if (!channel->hasUser(_client))
            {
                _server->sendNumeric(_client, ERR_NOTONCHANNEL, _cmd.params, "You're not on that channel");
            }
            else if (channel->isInviteOnly() && !channel->isOperator(_client))
            {
                _server->sendNumeric(_client, ERR_CHANOPRIVSNEEDED, _cmd.params, "You're not channel operator");
            }
            else if (channel->hasUser(invitedClient))
            {
                _server->sendNumeric(_client, ERR_USERONCHANNEL, _cmd.params, "is already on channel");
            }
        }
        else
        {
            _server->sendNumeric(_client, RPL_INVITING, _cmd.params, "");
            invitedClient->addInvited(_cmd.params[1]);
        }
    }
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
        _server->sendNumeric(_client, ERR_NEEDMOREPARAMS, _cmd.params, "Not enough parameters");
    else
    {
        Channel *channel = _server->getChannel(_cmd.params[0]);

        if (!channel)
            _server->sendNumeric(_client, ERR_NOSUCHCHANNEL, _cmd.params, "No such channel");
        else if (!_client->hasChannel(channel))
            _server->sendNumeric(_client, ERR_NOTONCHANNEL, _cmd.params, "You're not on that channel");
        else if (_cmd.trailing.empty())
        {
            if (channel->getTopic().empty())
                _server->sendNumeric(_client, RPL_NOTOPIC, _cmd.params, "No topic is set");
            else
                _server->sendNumeric(_client, RPL_TOPIC, _cmd.params, channel->getTopic());
        }
        else if (channel->isTopicRestricted() && !channel->isOperator(_client))
            _server->sendNumeric(_client, ERR_CHANOPRIVSNEEDED, _cmd.params, "You're not channel operator");
        else
        {
            std::cout << "Setting the topic to a new one: " << _cmd.trailing << std::endl;
            //     sendToChannel(_cmd.trailing);
            channel->setTopic(_cmd.trailing);
        }
    }
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
    if (channel->hasKey())
        _server->sendNumeric(_client, ERR_KEYSET, _cmd.params, "Channel key already set");
    else
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
void CommandHandler::_modeLimit(Channel *channel, int signIsPositive)
{
    if (_cmd.params.size() < 3)
        _server->sendNumeric(_client, ERR_NEEDMOREPARAMS, _cmd.params, "Not enough parameters");
    else if (signIsPositive)
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
    else
    {
        channel->removeLimit();
        std::cout << "Remove a user limit from the channel " << channel->getName() << std::endl;
    }
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
void CommandHandler::_handleMode()
{
    if (_cmd.params.size() < 2)
        _server->sendNumeric(_client, ERR_NEEDMOREPARAMS, _cmd.params, "Not enough parameters");
    else
    {
        Channel *channel = _server->getChannel(_cmd.params[0]);

        if (!channel)
            _server->sendNumeric(_client, ERR_NOSUCHCHANNEL, _cmd.params, "No such channel");
        else if (!_client->hasChannel(channel))
            _server->sendNumeric(_client, ERR_NOTONCHANNEL, _cmd.params, "You're not on that channel");
        else if (!channel->isOperator(_client))
            _server->sendNumeric(_client, ERR_CHANOPRIVSNEEDED, _cmd.params, "You're not channel operator");
        else
        {
            int sign = _identifySign(_cmd.params[1][0]);

            if (_cmd.params[1].length() >= 2 && sign != INVALID)
            {
                _init_modes();
                std::map<char, modeFunc>::iterator it = _modes.find(_cmd.params[1][1]);

                if (it == _modes.end())
                    _server->sendNumeric(_client, ERR_UMODEUNKNOWNFLAG, _cmd.params, "Unknown MODE flag");
                else
                    (this->*it->second)(channel, sign);
            }
            else
                _server->sendNumeric(_client, ERR_UNKNOWNMODE, _cmd.params, "is unknown mode char to me");
        }
    }
}

// ---------------- Utils ----------------

int CommandHandler::_identifySign(char sign)
{
    if (sign == '+')
        return ENABLE;
    else if (sign == '-')
        return DISABLE;
    else
        return INVALID;
}

bool CommandHandler::_compareNick(Client *client, const std::string &name)
{
    return client->getNickname() == name;
}

bool CommandHandler::_compareUser(Client *client, const std::string &name)
{
    return client->getUsername() == name;
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

// ---------------- Constructors ----------------

CommandHandler::CommandHandler(void) : _server(NULL), _client(NULL), _cmd(IrcMessage())
{
    // std::cout << "(CommandHandler) Default constructor\n";
}

CommandHandler::CommandHandler(Server *server, Client *client, const IrcMessage &cmd) : _server(server), _client(client), _cmd(cmd)
{
    // std::cout << "(CommandHandler) Overload constructor\n";

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
    // std::cout << "(CommandHandler) Destructor\n";
}
