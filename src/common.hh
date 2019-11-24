#ifndef _COMMON_HH_
#define _COMMON_HH_
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

namespace src
{
std::vector<std::string> split(const std::string &str, char c);

class input_helper
{
    private:
        std::ifstream fin;
        char deli;
    public:
        input_helper(const std::string &s, char d = ',') : fin(s), deli(d)
        { 
            if(!fin) throw std::runtime_error("file not found!\n");
            //std::getline(fin,this->header); 
        }
        std::vector<std::string> next()
        {
            std::string line{};
            std::getline(fin, line);
            if(line[0] == '#') line = ""; //ignore comments
            return split(line,deli);
        }
        bool hasNext(){return !fin.eof();}
        ~input_helper() {fin.close();}
};
}// namespace src


#endif
