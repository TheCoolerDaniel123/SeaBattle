#pragma once

#include <string>

// A parsed line typed by the player. The same grammar is used while placing ships and
// while playing; the game decides which commands make sense at the moment.
struct Command
{
    enum class Type
    {
        Unknown,
        Tiles,    // "a4" or "a4c4": one tile, or the two ends of a ship
        Delete,   // "del" or "del3"
        Clear,
        Random,
        Name,
        Hints,
        Borders,
        Help,
        Reveal,   // debug only
        Quit
    };

    Type type = Type::Unknown;
    int first = -1;   // Tiles: first tile index
    int second = -1;  // Tiles: second tile index, -1 if absent
    int length = 0;   // Delete: ship length, 0 = any
};

Command ParseCommand(const std::string& input);
