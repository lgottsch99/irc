#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>

#include "Client.hpp"

// class Client;

/*
An IRC channel:
    - exists independently of any one client
    - can have many clients
    - can outlive a single client joining/leaving
    - must be globally unique by name
*/

class Channel {
    private:
        std::string _name;
        std::string _topic;
        std::string _key;
        unsigned int    _userLimit;

        bool _mode_i;        // invite only
        // bool _mode_t;        // only ops can set topic
        bool _mode_k;        // has key
        bool _mode_l;        // has user limit

        std::set<Client*> _users;
        std::set<Client*> _operators;
        std::set<Client*> _invited;

    public:
        Channel();
        Channel(const std::string& name);
        ~Channel();

        const std::string& getName() const;
        // const std::string& getTopic() const;
        // const std::set<Client*>& getUsers() const;
        // const std::set<Client*>& getOperators() const;

        void setInviteMode(bool);
        void setKey(const std::string &);
        void setLimit(unsigned int);

        void removeKey(void);
        void removeLimit(void);
        const std::string& getKey() const;

        bool isInviteOnly(void) const;
        unsigned int getUserLimit(void) const;
        unsigned int getNumOfUsers(void) const;
        
        bool hasKey(void) const;

        // bool isTopicRestricted() const;
        // bool hasUserLimit() const;

        bool isOperator(Client*) const;
        bool isInvited(Client*) const;
        bool hasUser(Client*) const;

        // void setTopic(const std::string&);
        // void setKey(const std::string&);
        // void clearKey();
        // void setUserLimit(size_t);
        // void clearUserLimit();
        
        void addUser(Client*);
        void removeUser(Client*);
        void addOperator(Client*);
        void removeOperator(Client*);
        void addInvite(Client*);
        void removeInvite(Client*);
};

#endif
