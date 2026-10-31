#pragma once

#include <algorithm>
#include <cctype>
#include <string>

namespace text
{
    inline std::string Trim(const std::string& value)
    {
        const char* spaces = " \t\r\n";
        const std::size_t begin = value.find_first_not_of(spaces);
        if(begin == std::string::npos)
            return "";
        const std::size_t end = value.find_last_not_of(spaces);
        return value.substr(begin, end - begin + 1);
    }

    inline std::string ToLower(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return value;
    }
}
