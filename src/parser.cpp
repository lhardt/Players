#include "types.h"

#include <iostream>
#include <fstream>

bool get_next_player(std::ifstream &file, Player & p) {
	
	std::string this_line; 
	std::getline(file, this_line);

	if (this_line.length() == 0) return false;
	{
		int end = this_line.find(',');
		p.id = std::stoi(this_line.substr(0, end));
		this_line = this_line.substr(end+1);
	}
	{
		int end = this_line.find(',');
		p.name = this_line.substr(0, end);
		this_line = this_line.substr(end+1);
	}
	{
		int end = this_line.length();
		//std::string without_quotes = this_line.substr(1, end-2);
		this_line.erase(std::remove(this_line.begin(), this_line.end(), ' '), this_line.end());
		this_line.erase(std::remove(this_line.begin(), this_line.end(), '\"'), this_line.end());
		this_line += ",";

		while (this_line.length() != 0) {
			int next_comma = this_line.find(',');
			if (next_comma == -1) { break;  }
			std::string pos = this_line.substr(0, next_comma);
			this_line = this_line.substr(next_comma+1, this_line.length() - 1);

			p.positions.push_back(pos);
		}
	}

	return true;
}