#pragma once

#include <array>
#include <vector>

#include "Coord.h"
#include "Field.h"

// The computer opponent ("SeaFighter"). It only learns what a real player would:
// the outcome of each of its shots.
class BotPlayer
{
public:
    enum class Difficulty
    {
        Easy = 1,  // hunts at random
        Hard = 2   // hunts along diagonals that every ship must cross
    };

    struct Shot
    {
        int tile;             // -1 when there was nothing left to shoot
        ShotOutcome outcome;
    };

    explicit BotPlayer(Difficulty difficulty);

    Shot Shoot(Field& enemy);

private:
    int ChooseTarget();
    std::vector<int> TargetCandidates() const;
    int PickHuntTarget() const;
    void BuildHuntOrder();
    void MarkAreaAroundDestroyedShip();

    Difficulty difficulty_;
    std::array<bool, coord::kCount> processed_{};  // tiles that must not be shot again
    std::vector<int> hits_;                        // sorted hits on the ship being finished
    std::vector<int> huntOrder_;                   // preferred tiles for the Hard level
};
