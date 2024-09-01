#include <vector>
#include <string>
#include <iostream>

class a
{

public:
    a(/* args */);
    ~a();
    std::vector<std::string> _vec;

};

a::a(/* args */)
{
}

a::~a()
{
}


std::vector<std::string> ft_split(const std::string& str, const std::string delim)
{
    std::vector<std::string> vec;
    int posBegin = 0;
    int posEnd = 0;
    if (str.empty() || delim.empty())
        return vec;
    std::cout <<  str.size() << std::endl;
    std::cout <<  str << std::endl;
    while(true)
    {
        posEnd = str.find(delim, posBegin);
        std::cout << "posBegin: " << posBegin << " posEnd: " << posEnd << " E - B: " << posEnd - posBegin << std::endl;
        if (posEnd == std::string::npos)
            break;
        if (posBegin != posEnd)
            vec.push_back(str.substr(posBegin, posEnd - posBegin));
        posBegin = posEnd + delim.size();
    }
    // if (posBegin < str.size())
    //     vec.push_back(str.substr(posBegin, ));
    return (vec);
}
int main()
{
    // std::string test = "Begin CAP LS\r\nPASS 42\r\nNICK dffwefw\r\nUSER scely scely 127.0.0.1 :scely";
    char test[] = "Begin CAP LS\r\nPASS 42\r\nNICK dffwefw\r\nUSER scely scely 127.0.0.1 :scely\0";
    a *sam = new a;
    sam->_vec = ft_split(test, "\r\n");
    delete sam;
}