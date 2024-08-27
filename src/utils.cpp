#include <vector>
#include <string>

std::vector<std::string> ft_split(std::string str, std::string delim)
{
    std::string tmp;
    std::vector<std::string> vec;
    int posBegin = 0;
    int posEnd = 0;
    if (!delim.size())
        return (vec);
    while(true)
    {
        posEnd = str.find(delim, posBegin);
        if (posEnd == std::string::npos)
            break;
        tmp = str.substr(posBegin, posEnd - posBegin);
        vec.push_back(tmp);
        posBegin = posEnd + delim.size();
    }
    vec.push_back(str.substr(posBegin));
    return (vec);
}
