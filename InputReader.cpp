#include "InputReader.h"

void InputReader::start()
{
	while (true)
	{
		std::string str;
		std::getline(std::cin, str);

		std::istringstream ss(str); // Used for breaking words 
		std::string command; // To store individual words 
		if (ss >> command)
		{
			if (command == "cr") {
				std::string name;
				if (!(ss >> name) || !ss.eof())
					incorrectSyntax();
				else
					std::cout << "create " << name << std::endl;
			}
			else if (command == "de") {
				std::string name;
				if (!(ss >> name) || !ss.eof())
					incorrectSyntax();
				else
					std::cout << "delete " << name << std::endl;
			}
			else if (command == "op") {
				std::string name;
				if (!(ss >> name) || !ss.eof())
					incorrectSyntax();
				else
					std::cout << "open " << name << std::endl;
			}
			else if (command == "cl") {
				int index;
				if (!(ss >> index) || !ss.eof())
					incorrectSyntax();
				else
					std::cout << "close " << index << std::endl;
			}
			else if (command == "rd") {
				int index, count;
				if (!(ss >> index, ss >> count) || !ss.eof())
					incorrectSyntax();
				else
					std::cout << "read " << index << ' ' << count << std::endl;
			}
			else if (command == "wr") {
				int index, count;
				char c;
				if (!(ss >> index, ss >> c, ss >> count) || !ss.eof())
					incorrectSyntax();
				else
					std::cout << "write " << index << ' ' << c << ' ' << count << std::endl;
			}
			else if (command == "sk") {
				int index, pos;
				if (!(ss >> index, ss >> pos) || !ss.eof())
					incorrectSyntax();
				else
					std::cout << "seak " << index << ' ' << pos << std::endl;
			}
			else if (command == "dr") {
				std::cout << "directory\n";
			}
			else if (command == "in") {
				std::string name;
				if (!(ss >> name) || !ss.eof())
					incorrectSyntax();
				else
					std::cout << "in " << name << std::endl;
			}
			else if (command == "sv") {
				std::string name;
				if (!(ss >> name) || !ss.eof())
					incorrectSyntax();
				else
					std::cout << "save " << name << std::endl;
			}
			else {
				std::cout << '\'' << command << "' is not recognized as an internal or external command,operable program or batch file.\n";
			}


		}
	}
}

void InputReader::incorrectSyntax()
{
	std::cout << "The syntax of the command is incorrect.\n";
}
