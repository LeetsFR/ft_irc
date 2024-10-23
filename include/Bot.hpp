#ifndef BOT_HPP
#define BOT_HPP

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

class Bot
{
	private:
		bool _good;
		std::vector<std::string> _forbbidenWord;
	public:
		Bot();
		~Bot();
		bool controlStr(std::string&);
		bool getGood();
};

#endif