#include <vector>
#include <string>
#include <iostream>


std::vector<std::string> ft_split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> vec;
    size_t posBegin = 0;
    size_t posEnd = 0;
    if (str.empty() || delim.empty())
        return vec;
    while(true)
    {
        posEnd = str.find(delim, posBegin);
        // std::cout << "posBegin: " << posBegin << " posEnd: " << posEnd << " E-B" << posEnd - posBegin << std::endl;

        if (posEnd == std::string::npos)
            break;
        if (posBegin != posEnd)
            vec.push_back(str.substr(posBegin, posEnd - posBegin));
        posBegin = posEnd + delim.size();
    }
    if (posBegin < str.size())
        vec.push_back(str.substr(posBegin, std::string::npos));
    return (vec);
}
