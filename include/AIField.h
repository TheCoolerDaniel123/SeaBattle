#pragma once

#include "Field.h"

// The bot's board: its ships stay hidden until revealed.
class AIField : public Field
{
public:
    void RevealShips(bool reveal) { revealShips_ = reveal; }
    void ToggleReveal() { revealShips_ = !revealShips_; }

protected:
    char TileSymbol(int tile) const override;

private:
    bool revealShips_ = false;
};
