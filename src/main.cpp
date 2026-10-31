#include <iostream>
#include <string>

#include "BotPlayer.h"
#include "Console.h"
#include "Game.h"
#include "StringUtil.h"

namespace
{
    // Returns false if the player wants to leave (or the input is closed).
    bool ChooseDifficulty(BotPlayer::Difficulty& difficulty)
    {
        for(;;)
        {
            console::Clear();
            std::cout << "Choose difficulty:\n"
                         "[1] - Easy (the bot shoots at random)\n"
                         "[2] - Hard (the bot searches the field in a pattern)\n> ";

            std::string line;
            if(!console::ReadLine(line))
                return false;
            line = text::ToLower(text::Trim(line));
            if(line == "1")
            {
                difficulty = BotPlayer::Difficulty::Easy;
                return true;
            }
            if(line == "2")
            {
                difficulty = BotPlayer::Difficulty::Hard;
                return true;
            }
            if(line == "quit" || line == "exit")
                return false;
        }
    }
}

int main()
{
    BotPlayer::Difficulty difficulty = BotPlayer::Difficulty::Easy;
    if(!ChooseDifficulty(difficulty))
        return 0;

    Game game(difficulty);
    game.Run();
    return 0;
}
