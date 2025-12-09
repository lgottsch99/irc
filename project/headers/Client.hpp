#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>

// class Channel;

class Client {
private:
    int              _fd;
    std::string      _ip;

    // Registration
    // bool _hasSentPass; // why?
    // bool _hasSentNick;
    // bool _hasSentUser;

    bool _authenticated;   // PASS ok
    // bool _registered;      // NICK+USER completed, WELCOME sent

    // std::string _nickname;
    // std::string _username;
    // std::string _realname;

    // std::set<Channel*> _channels; // Channels the client has joined 

    // std::string _recvBuffer;
    // std::string _sendBuffer;

public:
    Client(int fd, const std::string& ip);
    ~Client();

    // int getFd() const;
    // const std::string&  getIP() const;
    // const std::string&  getNickname() const;
    // const std::string&  getUsername() const;
    // const std::string&  getRealname() const;

    // bool hasSentPass() const;
    // bool hasSentNick() const;
    // bool hasSentUser() const;
    bool isAuthenticated() const;
    // bool isRegistered() const;

    // const std::set<Channel*>& getChannels() const;
    // const std::string& getRecvBuffer() const;
    // const std::string& getSendBuffer() const;

    // void setNickname(const std::string&);
    // void setUsername(const std::string&);
    // void setRealname(const std::string&);
    // void setSentPass(bool v);
    // void setSentNick(bool v);
    // void setSentUser(bool v);
    void setAuthenticated(bool v);
    // void setRegistered(bool v);

    // void appendToRecvBuffer(const std::string&);
    // void appendToSendBuffer(const std::string&);
    // void clearRecvBuffer();
    // void clearSendBuffer();
    // void addChannel(Channel*);
    // void removeChannel(Channel*);
};

#endif
