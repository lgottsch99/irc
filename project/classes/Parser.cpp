#include "Parser.hpp"

// private constructors
Parser::Parser(){}
Parser::Parser(const Parser& parser){ (void)parser; }
Parser& Parser::operator=(const Parser& parser) { (void)parser; return(*this); }

// private members
void Parser::handleTrailing(IrcMessage& message, std::string& line)
{
    size_t colonPos = line.find(" :");
    
}


// the main thing: parseLine()
IrcMessage Parser::parseLine(const std::string& input)
{
    IrcMessage message;
    std::string line = input;
    handleTrailing(message, line);
}
