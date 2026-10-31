#pragma once

#include <array>
#include <iosfwd>
#include <string>
#include <vector>

#include "Coord.h"
#include "Ship.h"

enum class TileType { Unknown, Miss, ShipPart, Hit };

enum class ShotOutcome { AlreadyShot, Miss, Hit, Destroyed };

struct TileData
{
    TileType type = TileType::Unknown;
    int shipIndex = -1;  // index into the field's ship list, -1 when there is no ship
    int occupied = 0;    // how many ships cover this tile with their body or surrounding area
};

// Game board with a fleet. Knows nothing about input; derived classes only decide
// how the board looks to the person who sees it.
class Field
{
public:
    enum class PlaceResult { Ok, InvalidShip, Blocked, NoneLeft };

    virtual ~Field() = default;

    void Draw(std::ostream& out) const;
    void SetBorders(bool visible) { showBorders_ = visible; }

    // Fleet editing
    PlaceResult Place(const Ship& ship);
    bool RemoveLast(int length = 0);  // length 0 = the last placed ship of any length
    void Clear();
    void PlaceRandomly();
    int ShipCount(int length) const;
    int Remaining(int length) const;  // how many ships of this length are still missing
    bool IsFleetComplete() const;

    // Battle. Shots outside the board or at an already shot tile report AlreadyShot.
    ShotOutcome ReceiveShot(int tile);
    bool HasAliveShips() const;

    const TileData& Tile(int tile) const { return tiles_[tile]; }

protected:
    Field() = default;

    virtual char TileSymbol(int tile) const;
    virtual std::string SideText(int line) const;  // text printed to the right of a board line
    static char SymbolOf(TileType type);

private:
    void Occupy(const Ship& ship, int delta);
    void RebuildShipIndex();

    std::array<TileData, coord::kCount> tiles_;
    std::vector<Ship> ships_;
    bool showBorders_ = true;
};
