# SeaBattle

[![CI](https://github.com/TheCoolerDaniel123/SeaBattle/actions/workflows/ci.yml/badge.svg)](https://github.com/TheCoolerDaniel123/SeaBattle/actions/workflows/ci.yml) [![Latest release](https://img.shields.io/github/v/release/TheCoolerDaniel123/SeaBattle)](https://github.com/TheCoolerDaniel123/SeaBattle/releases/latest)

A console Battleship game for one player against a computer opponent ("SeaFighter"), written in C++17.

```
  | A | B | C | D | E | F | G | H | I | J |
-------------------------------------------
1 |   |   |   |   | # |   | # | # | # | # |
-------------------------------------------
2 |   |   |   |   | # |   |   |   |   |   |
...
```

## Features

- Classic 10x10 board and fleet: 1x4, 2x3, 3x2 and 4x1 ships; ships may not touch each other, even diagonally.
- Place ships by hand, or randomly with one command.
- Two bot levels:
  - **Easy** - shoots at random tiles and finishes off any ship it hits.
  - **Hard** - searches along diagonals that every ship must cross, then finishes off wounded ships.
- The bot only knows what a real player would know: the result of its own shots.
- No dependencies, works on Windows, Linux and macOS.

## Download

Windows users: grab `SeaBattle.exe` from the [Releases](../../releases) page. It is a single file, no DLLs needed.

## Build

Requires a C++17 compiler (GCC, Clang or MSVC) and optionally CMake 3.13+.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/seabattle            # build\seabattle.exe on Windows
ctest --test-dir build       # run the unit tests
```

With MinGW the runtime is linked statically, so the resulting `.exe` runs on any Windows machine.

**Code::Blocks:** open `project/SeaBattle.cbp` (the Debug target also enables the `showme` cheat command described below).

**Without CMake:**

```sh
g++ -std=c++17 -O2 -Iinclude src/*.cpp -o seabattle
```

## How to play

Choose a difficulty, then place your fleet. Coordinates are a letter (column) and a number (row), e.g. `a1`, `j10`; input is not case sensitive.

| Command | Where | Purpose |
|---|---|---|
| `a4c4` / `a4` | placing | Place a ship from `a4` to `c4`, or a single-tile ship on `a4` |
| `del3` / `del` | placing | Remove the last placed ship of length 3 / of any length |
| `clear` | placing | Remove all ships |
| `random` | placing | Place the whole fleet randomly |
| `name` | placing | Change your name |
| `hints` | placing | Show/hide `'` on tiles where no ship can be placed |
| `a1` | playing | Shoot at tile `a1` |
| `borders` | both | Show/hide the borders between tiles |
| `help` | playing | Short help |
| `quit` / `exit` | anywhere | Leave the game |

Symbols: ` ` unknown, `o` miss, `#` ship, `*` hit ship. A hit lets you shoot again; a miss passes the turn.

Debug builds (`-DSEABATTLE_DEBUG`, or `cmake -DSEABATTLE_DEBUG=ON`) add the `showme` command that reveals the bot's ships.

## Continuous integration

GitHub Actions (`.github/workflows/ci.yml`) builds and tests the project on Linux, macOS and Windows (MSVC and MinGW), and runs the tests under AddressSanitizer/UBSan. Pushing a tag like `v1.0` builds a static `SeaBattle.exe` and attaches it to a GitHub release automatically.

## Project structure

```
include/   headers
src/       implementation
tests/     unit tests (no framework needed)
.github/   CI workflow
project/   Code::Blocks project
```

| Module | Responsibility |
|---|---|
| `Coord` | tile indices, names like `c4`, neighbour helpers |
| `Ship` | ship geometry, hits, surrounding area |
| `Field` | board, fleet rules, shots, rendering; `PlayerField` / `AIField` decide what is visible |
| `BotPlayer` | the computer opponent |
| `Command` | parsing of the text typed by the player |
| `Game` | game flow: setup, battle, result |
| `Console` | the only place with OS-specific terminal code |

## License

MIT, see [LICENSE](LICENSE).
