#include "Command.h"

#include <cstddef>

#include "Coord.h"
#include "StringUtil.h"

namespace
{
    struct Word
    {
        const char* text;
        Command::Type type;
    };

    const Word kWords[] = {
        {"clear", Command::Type::Clear},   {"random", Command::Type::Random},
        {"name", Command::Type::Name},     {"hints", Command::Type::Hints},
        {"borders", Command::Type::Borders}, {"help", Command::Type::Help},
        {"showme", Command::Type::Reveal}, {"quit", Command::Type::Quit},
        {"exit", Command::Type::Quit}};
}

Command ParseCommand(const std::string& input)
{
    const std::string line = text::ToLower(text::Trim(input));
    Command command;

    for(const Word& word : kWords)
    {
        if(line == word.text)
        {
            command.type = word.type;
            return command;
        }
    }

    if(line.compare(0, 3, "del") == 0)
    {
        if(line.size() == 3)
        {
            command.type = Command::Type::Delete;
        }
        else if(line.size() == 4 && line[3] >= '1' && line[3] <= '0' + 4)
        {
            command.type = Command::Type::Delete;
            command.length = line[3] - '0';
        }
        return command;
    }

    std::size_t pos = 0;
    int first = -1;
    if(coord::Parse(line, pos, first))
    {
        int second = -1;
        if(pos == line.size())
        {
            command.type = Command::Type::Tiles;
            command.first = first;
        }
        else if(coord::Parse(line, pos, second) && pos == line.size())
        {
            command.type = Command::Type::Tiles;
            command.first = first;
            command.second = second;
        }
    }
    return command;
}
