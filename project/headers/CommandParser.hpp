#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP

/*
This extracts commands from raw lines.
Input:
"JOIN #test"
→ output:
struct ParsedCommand {
    std::string name;               // "JOIN"
    std::vector<std::string> args;  // ["#test"]
}
Responsibilities:
Trim CRLF
Split into command + args
Expand trailing parameter starting with ":" (per RFC)
This is NOT where you implement semantics.
It just splits the input.

example:
input: "PRIVMSG #test :hello world"
output:
name = "PRIVMSG"
args = ["#test", "hello world"]
Trailing argument after : is handled here.
*/

#include <string>
#include <vector>
#include <iostream>


struct ParsedCommand
{
    std::string                 name;
    std::vector<std::string>    args;
};

class CommandParser
{
	public:
		CommandParser();
		~CommandParser();

		ParsedCommand parse(const std::string& line) const;

	private:
		std::vector<std::string> split(const std::string &s) const;
};


#endif