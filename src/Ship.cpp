#include "Ship.h"

#include <algorithm>

#include "Coord.h"

Ship::Ship(int tileA, int tileB)
    : first_(std::min(tileA, tileB)), last_(std::max(tileA, tileB)), alive_(0)
{
    alive_ = Length();
}

int Ship::Length() const
{
    if(!coord::IsValid(first_) || !coord::IsValid(last_))
        return 0;
    if(coord::Row(first_) == coord::Row(last_))
        return last_ - first_ + 1;
    if(coord::Col(first_) == coord::Col(last_))
        return (last_ - first_) / coord::kSize + 1;
    return 0;
}

bool Ship::IsValid() const
{
    const int length = Length();
    return length >= 1 && length <= kMaxShipLength;
}

int Ship::Step() const
{
    const bool vertical = first_ != last_ && coord::Col(first_) == coord::Col(last_);
    return vertical ? coord::kSize : 1;
}

std::vector<int> Ship::Tiles() const
{
    std::vector<int> tiles;
    if(Length() == 0)
        return tiles;
    for(int tile = first_; tile <= last_; tile += Step())
        tiles.push_back(tile);
    return tiles;
}

std::set<int> Ship::NearbyArea() const
{
    std::set<int> area;
    const std::vector<int> body = Tiles();
    for(int tile : body)
    {
        for(int dRow = -1; dRow <= 1; ++dRow)
        {
            for(int dCol = -1; dCol <= 1; ++dCol)
            {
                int neighbour;
                if((dRow != 0 || dCol != 0) && coord::Offset(tile, dRow, dCol, neighbour))
                    area.insert(neighbour);
            }
        }
    }
    for(int tile : body)
        area.erase(tile);
    return area;
}

void Ship::RegisterHit()
{
    if(alive_ > 0)
        --alive_;
}
