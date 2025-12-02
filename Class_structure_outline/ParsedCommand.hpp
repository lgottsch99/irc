#ifndef PARSEDCOMMAND_HPP
#define PARSEDCOMMAND_HPP

#include <string>
#include <vector>

struct ParsedCommand {
    std::string              name;
    std::vector<std::string> args;
};

#endif
