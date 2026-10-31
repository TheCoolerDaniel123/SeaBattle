#include "Coord.h"

namespace coord
{
    bool Offset(int tile, int dRow, int dCol, int& result)
    {
        const int row = Row(tile) + dRow;
        const int col = Col(tile) + dCol;
        if(row < 0 || row >= kSize || col < 0 || col >= kSize)
            return false;
        result = Index(row, col);
        return true;
    }

    std::string Name(int tile)
    {
        return std::string(1, static_cast<char>('a' + Col(tile))) + std::to_string(Row(tile) + 1);
    }

    bool Parse(const std::string& text, std::size_t& pos, int& tile)
    {
        std::size_t p = pos;
        if(p >= text.size() || text[p] < 'a' || text[p] >= 'a' + kSize)
            return false;
        const int col = text[p++] - 'a';

        if(p >= text.size() || text[p] < '0' || text[p] > '9')
            return false;
        int number = text[p++] - '0';
        if(p < text.size() && text[p] >= '0' && text[p] <= '9')
            number = number * 10 + (text[p++] - '0');  // only "10" can be valid here
        if(number < 1 || number > kSize)
            return false;

        tile = Index(number - 1, col);
        pos = p;
        return true;
    }
}
