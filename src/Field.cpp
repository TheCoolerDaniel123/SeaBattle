#include "Field.h"

#include <ostream>

#include "Random.h"

namespace
{
    constexpr char kHeader[] = "  | A | B | C | D | E | F | G | H | I | J |";
    constexpr int kSideTextGap = 5;
}

char Field::SymbolOf(TileType type)
{
    switch(type)
    {
        case TileType::Miss:     return 'o';
        case TileType::ShipPart: return '#';
        case TileType::Hit:      return '*';
        case TileType::Unknown:  break;
    }
    return ' ';
}

char Field::TileSymbol(int tile) const
{
    return SymbolOf(tiles_[tile].type);
}

std::string Field::SideText(int) const
{
    return "";
}

void Field::Draw(std::ostream& out) const
{
    const char vertical = showBorders_ ? '|' : ' ';
    const char horizontal = showBorders_ ? '-' : ' ';

    std::vector<std::string> lines;
    lines.push_back(kHeader);
    lines.push_back(std::string(43, '-'));
    for(int row = 0; row < coord::kSize; ++row)
    {
        std::string line = (row + 1 < 10) ? std::to_string(row + 1) + " |" : "10|";
        for(int col = 0; col < coord::kSize; ++col)
        {
            line += ' ';
            line += TileSymbol(coord::Index(row, col));
            line += ' ';
            line += vertical;
        }
        lines.push_back(line);
        if(row + 1 < coord::kSize)
            lines.push_back("---" + std::string(40, horizontal));
    }

    for(std::size_t i = 0; i < lines.size(); ++i)
    {
        out << lines[i];
        const std::string side = SideText(static_cast<int>(i));
        if(!side.empty())
            out << std::string(kSideTextGap, ' ') << side;
        out << '\n';
    }
}

// ---------------------------------------------------------------- fleet editing

void Field::Occupy(const Ship& ship, int delta)
{
    for(int tile : ship.Tiles())
        tiles_[tile].occupied += delta;
    for(int tile : ship.NearbyArea())
        tiles_[tile].occupied += delta;
}

void Field::RebuildShipIndex()
{
    for(TileData& tile : tiles_)
        tile.shipIndex = -1;
    for(std::size_t i = 0; i < ships_.size(); ++i)
        for(int tile : ships_[i].Tiles())
            tiles_[tile].shipIndex = static_cast<int>(i);
}

Field::PlaceResult Field::Place(const Ship& ship)
{
    if(!ship.IsValid())
        return PlaceResult::InvalidShip;
    if(Remaining(ship.Length()) <= 0)
        return PlaceResult::NoneLeft;

    const std::vector<int> body = ship.Tiles();
    for(int tile : body)
        if(tiles_[tile].occupied > 0)
            return PlaceResult::Blocked;

    ships_.push_back(ship);
    for(int tile : body)
    {
        tiles_[tile].type = TileType::ShipPart;
        tiles_[tile].shipIndex = static_cast<int>(ships_.size()) - 1;
    }
    Occupy(ship, +1);
    return PlaceResult::Ok;
}

bool Field::RemoveLast(int length)
{
    for(int i = static_cast<int>(ships_.size()) - 1; i >= 0; --i)
    {
        if(length != 0 && ships_[i].Length() != length)
            continue;
        const Ship ship = ships_[i];
        for(int tile : ship.Tiles())
            tiles_[tile].type = TileType::Unknown;
        Occupy(ship, -1);
        ships_.erase(ships_.begin() + i);
        RebuildShipIndex();
        return true;
    }
    return false;
}

void Field::Clear()
{
    tiles_.fill(TileData());
    ships_.clear();
}

void Field::PlaceRandomly()
{
    constexpr int kAttemptsPerShip = 2000;
    for(;;)  // restarts from scratch if a ship finds no room (practically never happens)
    {
        Clear();
        bool ok = true;
        // Largest ships first: they are the hardest to fit.
        for(int length = kMaxShipLength; length >= 1 && ok; --length)
        {
            for(int n = 0; n < kFleetLayout[length - 1] && ok; ++n)
            {
                ok = false;
                for(int attempt = 0; attempt < kAttemptsPerShip && !ok; ++attempt)
                {
                    const bool horizontal = rnd::Int(0, 1) == 1;
                    const int row = rnd::Int(0, coord::kSize - (horizontal ? 1 : length));
                    const int col = rnd::Int(0, coord::kSize - (horizontal ? length : 1));
                    const int first = coord::Index(row, col);
                    const int last = horizontal ? first + length - 1
                                                : first + (length - 1) * coord::kSize;
                    ok = Place(Ship(first, last)) == PlaceResult::Ok;
                }
            }
        }
        if(ok)
            return;
    }
}

int Field::ShipCount(int length) const
{
    int count = 0;
    for(const Ship& ship : ships_)
        if(ship.Length() == length)
            ++count;
    return count;
}

int Field::Remaining(int length) const
{
    if(length < 1 || length > kMaxShipLength)
        return 0;
    return kFleetLayout[length - 1] - ShipCount(length);
}

bool Field::IsFleetComplete() const
{
    return static_cast<int>(ships_.size()) == kFleetSize;
}

// ---------------------------------------------------------------------- battle

ShotOutcome Field::ReceiveShot(int tile)
{
    if(!coord::IsValid(tile))
        return ShotOutcome::AlreadyShot;

    TileData& data = tiles_[tile];
    switch(data.type)
    {
        case TileType::Unknown:
            data.type = TileType::Miss;
            return ShotOutcome::Miss;
        case TileType::ShipPart:
        {
            data.type = TileType::Hit;
            Ship& ship = ships_[data.shipIndex];
            ship.RegisterHit();
            return ship.IsDestroyed() ? ShotOutcome::Destroyed : ShotOutcome::Hit;
        }
        case TileType::Miss:
        case TileType::Hit:
            break;
    }
    return ShotOutcome::AlreadyShot;
}

bool Field::HasAliveShips() const
{
    for(const Ship& ship : ships_)
        if(!ship.IsDestroyed())
            return true;
    return false;
}
