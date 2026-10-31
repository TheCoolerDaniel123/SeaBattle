#include "Console.h"

#include <cstdlib>
#include <iostream>

namespace console
{
    void Clear()
    {
#ifdef _WIN32
        std::system("cls");
#else
        std::cout << "\x1b[2J\x1b[H" << std::flush;
#endif
    }

    bool ReadLine(std::string& line)
    {
        if(!std::getline(std::cin, line))
            return false;
        if(!line.empty() && line.back() == '\r')
            line.pop_back();
        return true;
    }

    void WaitForEnter(const std::string& prompt)
    {
        std::cout << prompt << std::flush;
        std::string ignored;
        std::getline(std::cin, ignored);
    }
}
