#pragma once

#include <array>
#include <set>
#include <vector>

inline constexpr int kMaxShipLength = 4;

// How many ships of each length a fleet has: index 0 -> length 1, index 3 -> length 4.
inline constexpr std::array<int, kMaxShipLength> kFleetLayout = {4, 3, 2, 1};
inline constexpr int kFleetSize = 10;

// A straight ship described by its first and last tile (first <= last).
class Ship
{
public:
    Ship(int tileA, int tileB);
    explicit Ship(int tile) : Ship(tile, tile) {}

    // True for a horizontal or vertical line of 1..kMaxShipLength tiles inside the board.
    bool IsValid() const;

    // Number of tiles in the line; 0 if the two tiles don't form a straight line.
    int Length() const;

    // Distance between neighbouring tiles of the ship: 1 (horizontal) or 10 (vertical).
    int Step() const;

    int First() const { return first_; }
    int Last() const { return last_; }
    std::vector<int> Tiles() const;

    // Tiles touching the ship (including diagonals), without the ship itself.
    std::set<int> NearbyArea() const;

    void RegisterHit();
    bool IsDestroyed() const { return alive_ == 0; }
    int Alive() const { return alive_; }

private:
    int first_;
    int last_;
    int alive_;
};
