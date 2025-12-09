#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>

class Client;

class Channel {
    private:
        std::string       _name;
        std::string       _topic;

        std::set<Client*> _users;
        std::set<Client*> _operators;
        std::set<Client*> _invited; // ?

        bool _mode_i;        // invite only
        bool _mode_t;        // only ops can set topic
        bool _mode_k;        // has key
        bool _mode_l;        // has user limit

        std::string _key;
        size_t      _userLimit;

    public:
        Channel();
        Channel(const std::string& name);
        ~Channel();

        const std::string& getName() const;
        const std::string& getTopic() const;
        const std::set<Client*>& getUsers() const;
        const std::set<Client*>& getOperators() const;

        bool isInviteOnly() const;
        bool isTopicRestricted() const;
        bool hasKey() const;
        bool hasUserLimit() const;
        const std::string& getKey() const;
        size_t getUserLimit() const;

        bool isOperator(Client*) const;
        bool hasUser(Client*) const;
        bool isInvited(Client*) const;

        void setTopic(const std::string&);
        void setKey(const std::string&);
        void clearKey();
        void setUserLimit(size_t);
        void clearUserLimit();
        
        void addUser(Client*);
        void removeUser(Client*);
        void addOperator(Client*);
        void removeOperator(Client*);
        void addInvite(Client*);
        void removeInvite(Client*);
};

#endif
