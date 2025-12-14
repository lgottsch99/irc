#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <set>

// #include "Channel.hpp"

class Channel;

class Client {
    private:
        bool _authenticated; // PASS ok
        bool _registered;    // NICK+USER completed, WELCOME sent

        std::string _nickname;
        std::string _username;
        std::string _realname;

        std::set<Channel*> _channels; // Channels the client has joined

    public:
        Client(void);
        ~Client(void);

        std::string  getNickname(void) const;
        std::string  getUsername(void) const;
        std::string  getRealname(void) const;

        bool isAuthenticated(void) const;
        bool isRegistered(void) const;
        
	    bool hasChannel(Channel*) const;
        void addToChannel(Channel*);
        void leaveChannel(Channel*);

        void setNickname(const std::string &);
        void setUsername(const std::string &);
        void setRealname(const std::string &);
        void setAuthenticated(bool v);
        void setRegistered(bool v);

        // void appendToRecvBuffer(const std::string&);
        // void appendToSendBuffer(const std::string&);
        // void clearRecvBuffer();
        // void clearSendBuffer();

        // for Lilli
        int fd;                 // client socket fd. needed by server
        std::string ip_address; // client ip. mainly for identification and logging
        std::string recv_buf;   // buffer for incoming data. needed by server
        std::string send_buf;   // buffer to send msg back to client (numeric replies or chat msgs). needed by server

        std::string extract_line(void); // needed by server
};

#endif
