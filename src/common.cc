#include "common.hh"
#include <sstream>

namespace src
{

std::vector<std::string> split(const std::string &str, char c)
{
    std::stringstream sin;
    std::vector<std::string> ret;
    sin.str(str);
    std::string part;
    while(std::getline(sin, part, c))
    {
        if(part.length())
            ret.emplace_back(std::move(part));
    }
    return ret;
}
}// namespace src
