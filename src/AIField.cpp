#include "AIField.h"

char AIField::TileSymbol(int tile) const
{
    if(!revealShips_ && Tile(tile).type == TileType::ShipPart)
        return SymbolOf(TileType::Unknown);  // hide ships from the player
    return Field::TileSymbol(tile);
}
