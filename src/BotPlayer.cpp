#include "BotPlayer.h"

#include <algorithm>

#include "Random.h"
#include "Ship.h"

BotPlayer::BotPlayer(Difficulty difficulty) : difficulty_(difficulty)
{
    if(difficulty_ == Difficulty::Hard)
        BuildHuntOrder();
}

// Every straight line of four tiles crosses each diagonal of the form (col - row) % 4 == k,
// so shooting those tiles first finds every ship in at most 25 shots. The diagonals that lie
// halfway between them (k + 2) share the same checkerboard colour and are used next.
void BotPlayer::BuildHuntOrder()
{
    const int k = rnd::Int(0, 3);
    std::vector<int> primary, secondary;
    for(int tile = 0; tile < coord::kCount; ++tile)
    {
        const int diagonal = ((coord::Col(tile) - coord::Row(tile)) % 4 + 4) % 4;
        if(diagonal == k)
            primary.push_back(tile);
        else if(diagonal == (k + 2) % 4)
            secondary.push_back(tile);
    }
    std::shuffle(primary.begin(), primary.end(), rnd::Engine());
    std::shuffle(secondary.begin(), secondary.end(), rnd::Engine());
    huntOrder_ = primary;
    huntOrder_.insert(huntOrder_.end(), secondary.begin(), secondary.end());
}

// Tiles worth shooting while a wounded ship is being finished.
std::vector<int> BotPlayer::TargetCandidates() const
{
    std::vector<int> result;
    auto consider = [&](int from, int dRow, int dCol)
    {
        int tile;
        if(coord::Offset(from, dRow, dCol, tile) && !processed_[tile])
            result.push_back(tile);
    };

    if(hits_.size() == 1)  // direction unknown yet: try all four neighbours
    {
        consider(hits_[0], -1, 0);
        consider(hits_[0], 1, 0);
        consider(hits_[0], 0, -1);
        consider(hits_[0], 0, 1);
    }
    else if(hits_.size() > 1)  // the ship's line is known: extend it at both ends
    {
        const bool horizontal = coord::Row(hits_.front()) == coord::Row(hits_.back());
        const int dRow = horizontal ? 0 : 1;
        const int dCol = horizontal ? 1 : 0;
        consider(hits_.front(), -dRow, -dCol);
        consider(hits_.back(), dRow, dCol);
    }
    return result;
}

int BotPlayer::PickHuntTarget() const
{
    if(difficulty_ == Difficulty::Hard)
        for(int tile : huntOrder_)
            if(!processed_[tile])
                return tile;

    std::vector<int> free;
    for(int tile = 0; tile < coord::kCount; ++tile)
        if(!processed_[tile])
            free.push_back(tile);
    if(free.empty())
        return -1;
    return free[rnd::Int(0, static_cast<int>(free.size()) - 1)];
}

int BotPlayer::ChooseTarget()
{
    const std::vector<int> candidates = TargetCandidates();
    if(!candidates.empty())
        return candidates[rnd::Int(0, static_cast<int>(candidates.size()) - 1)];
    hits_.clear();  // nothing left to follow up (can't happen in a normal game)
    return PickHuntTarget();
}

// A destroyed ship can't have neighbours, so everything around it is skipped from now on.
void BotPlayer::MarkAreaAroundDestroyedShip()
{
    const Ship ship(hits_.front(), hits_.back());
    for(int tile : ship.NearbyArea())
        processed_[tile] = true;
    hits_.clear();
}

BotPlayer::Shot BotPlayer::Shoot(Field& enemy)
{
    for(;;)
    {
        const int tile = ChooseTarget();
        if(tile < 0)
            return {-1, ShotOutcome::AlreadyShot};

        processed_[tile] = true;
        const ShotOutcome outcome = enemy.ReceiveShot(tile);
        switch(outcome)
        {
            case ShotOutcome::AlreadyShot:  // the tile was shot before the bot knew; pick another
                continue;
            case ShotOutcome::Miss:
                break;
            case ShotOutcome::Hit:
            case ShotOutcome::Destroyed:
                hits_.push_back(tile);
                std::sort(hits_.begin(), hits_.end());
                if(outcome == ShotOutcome::Destroyed)
                    MarkAreaAroundDestroyedShip();
                break;
        }
        return {tile, outcome};
    }
}
