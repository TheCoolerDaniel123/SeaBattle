#pragma once

#include <chrono>
#include <random>

// One shared random engine for the whole game.
namespace rnd
{
    inline std::mt19937& Engine()
    {
        static std::mt19937 engine = []
        {
            // Mix two sources: std::random_device is deterministic on some old MinGW builds.
            std::seed_seq seed{
                std::random_device{}(),
                static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())};
            return std::mt19937(seed);
        }();
        return engine;
    }

    // Uniform integer in [low, high].
    inline int Int(int low, int high)
    {
        return std::uniform_int_distribution<int>(low, high)(Engine());
    }

    // Makes the sequence reproducible (used by the tests).
    inline void Seed(unsigned value) { Engine().seed(value); }
}
