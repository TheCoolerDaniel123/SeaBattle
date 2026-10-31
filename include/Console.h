#pragma once

#include <string>

// All terminal interaction lives here, so the game logic stays free of OS-specific code.
namespace console
{
    void Clear();

    // Reads a whole line. Returns false when the input stream is closed.
    bool ReadLine(std::string& line);

    void WaitForEnter(const std::string& prompt = "Press Enter to continue...");
}
