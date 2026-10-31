#include "PlayerField.h"

#include <iterator>

namespace
{
    // The first lines explain the symbols and stay visible during the whole game;
    // the rest explains the commands and is shown only while ships are being placed.
    constexpr std::size_t kLegendLines = 5;

    const char* const kHelp[] = {
        "The upper field is yours and the lower (appears after start) is the bot's",
        "[ ] means unknown area on the map",
        "[o] means the tile was shot, but ship isn't there",
        "[#] means your ship is placed there",
        "[*] means the ship was placed there and now it got shot",
        "['] means you can't place ship on this tile",
        "",
        "Command example | Command purpose",
        "---------------------------------",
        "a4c4 (or a4)    | Place ship on the map using '[begin_pos][end_pos]' (or just '[pos]' for single ones)",
        "del3 (or del)   | Delete last ship of length N you placed using 'delN' (or just 'del' for any last ship)",
        "clear           | Delete all ships on the map",
        "random          | Your ships will be placed randomly",
        "name            | Change your name",
        "hints           | Enable/disable showing ['] on the tiles next to ships (enabled by default)",
        "borders         | Enable/disable the borders between tiles",
        "quit            | Leave the game"};
}

char PlayerField::TileSymbol(int tile) const
{
    const TileData& data = Tile(tile);
    if(showHints_ && !IsFleetComplete() && data.type == TileType::Unknown && data.occupied > 0)
        return '\'';
    return Field::TileSymbol(tile);
}

std::string PlayerField::SideText(int line) const
{
    const std::size_t shown = IsFleetComplete() ? kLegendLines : std::size(kHelp);
    if(line < 1 || static_cast<std::size_t>(line - 1) >= shown)
        return "";
    return kHelp[line - 1];
}
