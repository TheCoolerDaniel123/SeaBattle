#pragma once

#include <cstddef>
#include <string>

// Helpers for the 10x10 board.
// A tile is addressed by an index 0..99 (row * 10 + column). Players see tiles as
// a letter (column) plus a number (row), e.g. "c4".
namespace coord
{
    inline constexpr int kSize = 10;
    inline constexpr int kCount = kSize * kSize;

    constexpr int Row(int tile) { return tile / kSize; }
    constexpr int Col(int tile) { return tile % kSize; }
    constexpr bool IsValid(int tile) { return tile >= 0 && tile < kCount; }
    constexpr int Index(int row, int col) { return row * kSize + col; }

    // Moves a tile by (dRow, dCol). Returns false if the result is outside the board.
    bool Offset(int tile, int dRow, int dCol, int& result);

    // Index -> "c4". The tile must be valid.
    std::string Name(int tile);

    // Reads a tile name ("a1" .. "j10", lowercase) from text starting at pos.
    // On success moves pos past the name and returns true.
    bool Parse(const std::string& text, std::size_t& pos, int& tile);
}
