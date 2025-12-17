#ifndef NUMERICREPLIES
#define NUMERICREPLIES

enum Numeric
{
    RPL_WELCOME        = 1,
    // RPL_YOURHOST       = 2,
    // RPL_CREATED        = 3,
    // RPL_MYINFO         = 4,

    RPL_TOPIC          = 332,
    RPL_NOTOPIC = 331,
    RPL_NAMREPLY       = 353,
    RPL_ENDOFNAMES     = 366,

    ERR_NOSUCHNICK     = 401,
    ERR_NOSUCHCHANNEL  = 403,
    ERR_NICKNAMEINUSE  = 433, // Nickname is already in use
    ERR_NEEDMOREPARAMS = 461,
    ERR_CHANOPRIVSNEEDED = 482,
    ERR_UNKNOWNCOMMAND = 421,
    ERR_ALREADYREGISTERED = 462,
    ERR_PASSWDMISMATCH = 464,
    ERR_NONICKNAMEGIVEN = 431,
    ERR_ERRONEUSNICKNAME = 432,
    ERR_USERNOTINCHANNEL = 441, // "<nick> <channel> :They aren't on that channel"
    ERR_NOTONCHANNEL = 442, // "<channel> :You're not on that channel"
    ERR_NOTREGISTERED = 451,
    ERR_INVITEONLYCHAN = 473,
    ERR_BADCHANNELKEY = 475,
    ERR_CHANNELISFULL = 471,
    ERR_USERONCHANNEL = 443
};

#endif