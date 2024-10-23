#include "Bot.hpp"

Bot::Bot()
{
	std::ifstream file;
	file.open(".settings/settings.txt");
	if (!file)
	{
		std::cout << "Error open the file\n";
		_good = false;
	}
	else
	{
		std::string line;
    	while (std::getline(file, line))
			_forbbidenWord.push_back(line);
	}
}

Bot::~Bot()
{
}

bool Bot::getGood()
{
	return (this->_good);
}

bool Bot::controlStr(std::string &str)
{
	std::vector<std::string>::iterator it = _forbbidenWord.begin();
	size_t n;
	bool bad = false;
	for (; it != _forbbidenWord.end(); it++)
	{
		if ((n = str.find(*it)) != std::string::npos)
		{
			str.replace(n, it->size(), "❤️❤️❤️❤️❤️");
			bad = true;
		}
	}
	return (bad);
}
