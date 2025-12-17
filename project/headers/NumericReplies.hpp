#ifndef NUMERICREPLIES
#define NUMERICREPLIES

enum Numeric
{
    RPL_WELCOME        = 1,
    RPL_YOURHOST       = 2,
    RPL_CREATED        = 3,
    RPL_MYINFO         = 4,

    RPL_TOPIC          = 332,
    RPL_NAMREPLY       = 353,
    RPL_ENDOFNAMES     = 366,

    ERR_NOSUCHNICK     = 401,
    ERR_NOSUCHCHANNEL  = 403,
    ERR_NICKNAMEINUSE  = 433,
    ERR_NEEDMOREPARAMS = 461,
    ERR_CHANOPRIVSNEEDED = 482
};

#endif