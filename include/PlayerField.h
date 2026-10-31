#pragma once

#include "Field.h"

// The board the human sees as their own: all ships are visible, and while the fleet
// is being placed it also shows the tiles where a ship can't be put.
class PlayerField : public Field
{
public:
    void ToggleHints() { showHints_ = !showHints_; }

protected:
    char TileSymbol(int tile) const override;
    std::string SideText(int line) const override;

private:
    bool showHints_ = true;
};
