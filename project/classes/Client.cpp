#include "../headers/Client.hpp"

// ------------ Constructors -----------------------------
//default
Client::Client()
{
	std::cout << "(Client) Default constructor\n";

}

//destructor
Client::~Client()
{
	std::cout << "(Client) Destructor\n";
}



// ------------ Member FTs -----------------------------

/* checks if recv_buffer includes a full irc command line.
If yes: returns that single line and removes it from recv buffer
Returns:
		single line string, (if full irc cmd present)
	OR 	"" (empty str), (if no full cmd presnt)
*/
std::string Client::extract_line(void)
{
	std::string del = "\r\n"; //irc protocol
	std::string line = "";

	size_t del_pos = recv_buf.find(del);
	if (del_pos == std::string::npos)
	{
		std::cout << "Client: No full line yet..\n";
	}
	else
	{
		//get line up to del
		line = recv_buf.substr(0, del_pos + 1);
			//overwrite \r\n
		int x = line.size();
		line[x] = '\0';
		line[x - 1] = '\0';
		// std::cout << "Single line: " << line << std::endl;
		
		//update buf
		recv_buf.erase(0, del_pos + 2);
		// std::cout << "Remaining buf:\n" << recv_buf << std::endl;
	}
	return line;
}
