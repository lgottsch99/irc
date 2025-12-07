#ifndef PARSERHPP
#define PARSERHPP

#include <vector>
#include <string>


struct IrcMessage
{
    std::string command;
    std::vector<std::string> params;
    std::string trailing;      // empty if none
};

class Parser
{
    private:
    Parser();
    Parser(const Parser&);
    Parser& operator=(const Parser&);
    // members
    static void handleTrailing(IrcMessage&, std::string&);
    public:
    static IrcMessage parseLine(const std::string& input);
};

#endif