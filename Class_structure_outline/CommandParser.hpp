#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP

#include "ParsedCommand.hpp"
#include <string>

class CommandParser {
public:
    CommandParser();
    ~CommandParser();

    ParsedCommand parse(const std::string& line) const;
};

#endif
