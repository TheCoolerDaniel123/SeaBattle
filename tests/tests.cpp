// Minimal self-contained tests (no framework needed). Run: ./seabattle_tests
#include <iostream>
#include <set>
#include <string>

#include "BotPlayer.h"
#include "Command.h"
#include "Coord.h"
#include "Field.h"
#include "PlayerField.h"
#include "Random.h"
#include "Ship.h"

namespace
{
    int failures = 0;

    void Check(bool condition, const char* expression, int line)
    {
        if(!condition)
        {
            ++failures;
            std::cerr << "FAILED (line " << line << "): " << expression << '\n';
        }
    }
}

#define CHECK(expr) Check((expr), #expr, __LINE__)

static int T(const char* name)  // tile index from a name like "c4"
{
    std::size_t pos = 0;
    int tile = -1;
    coord::Parse(name, pos, tile);
    return tile;
}

static void TestCoords()
{
    CHECK(T("a1") == 0);
    CHECK(T("j10") == 99);
    CHECK(T("c4") == 32);
    CHECK(coord::Name(99) == "j10");
    CHECK(coord::Name(0) == "a1");
    for(int tile = 0; tile < coord::kCount; ++tile)
        CHECK(T(coord::Name(tile).c_str()) == tile);

    std::size_t pos = 0;
    int tile = -1;
    CHECK(!coord::Parse("k1", pos, tile));
    CHECK(!coord::Parse("a0", pos, tile));
    CHECK(!coord::Parse("a11", pos, tile));
    CHECK(!coord::Parse("1a", pos, tile));
    int out = -1;
    CHECK(coord::Offset(0, -1, 0, out) == false);
    CHECK(coord::Offset(9, 0, 1, out) == false);   // must not wrap to the next row
    CHECK(coord::Offset(99, 1, 0, out) == false);
    CHECK(coord::Offset(9, 1, 0, out) && out == 19);
}

static void TestCommands()
{
    Command c = ParseCommand("a4");
    CHECK(c.type == Command::Type::Tiles && c.first == T("a4") && c.second == -1);
    c = ParseCommand("  A4C4 ");
    CHECK(c.type == Command::Type::Tiles && c.first == T("a4") && c.second == T("c4"));
    c = ParseCommand("a10a7");
    CHECK(c.type == Command::Type::Tiles && c.first == T("a10") && c.second == T("a7"));
    c = ParseCommand("a1a10");
    CHECK(c.type == Command::Type::Tiles && c.second == T("a10"));
    CHECK(ParseCommand("a1c").type == Command::Type::Unknown);
    CHECK(ParseCommand("a1c3x").type == Command::Type::Unknown);
    CHECK(ParseCommand("a11").type == Command::Type::Unknown);
    CHECK(ParseCommand("").type == Command::Type::Unknown);
    c = ParseCommand("del");
    CHECK(c.type == Command::Type::Delete && c.length == 0);
    c = ParseCommand("del3");
    CHECK(c.type == Command::Type::Delete && c.length == 3);
    CHECK(ParseCommand("del5").type == Command::Type::Unknown);
    CHECK(ParseCommand("random").type == Command::Type::Random);
    CHECK(ParseCommand("HELP").type == Command::Type::Help);
    CHECK(ParseCommand("exit").type == Command::Type::Quit);
}

static void TestShip()
{
    Ship s(T("c4"), T("a4"));  // order of the ends doesn't matter
    CHECK(s.IsValid() && s.Length() == 3 && s.Step() == 1);
    CHECK(s.Tiles() == (std::vector<int>{T("a4"), T("b4"), T("c4")}));

    Ship v(T("b2"), T("b5"));
    CHECK(v.IsValid() && v.Length() == 4 && v.Step() == 10);

    CHECK(Ship(T("a1")).IsValid() && Ship(T("a1")).Length() == 1);
    CHECK(!Ship(T("a1"), T("b2")).IsValid());      // diagonal
    CHECK(!Ship(T("a1"), T("a5")).IsValid());      // too long
    CHECK(!Ship(T("j1"), T("a2")).IsValid());      // line wrapping over the row edge

    // A single tile in a corner has 3 neighbours, in the middle 8.
    CHECK(Ship(T("a1")).NearbyArea().size() == 3);
    CHECK(Ship(T("j10")).NearbyArea().size() == 3);
    CHECK(Ship(T("e5")).NearbyArea().size() == 8);
    // A horizontal ship of 2 in the middle: 4 tiles above, 4 below and 2 at the sides.
    CHECK(Ship(T("e5"), T("f5")).NearbyArea().size() == 10);

    Ship hit(T("a1"), T("a2"));
    hit.RegisterHit();
    CHECK(!hit.IsDestroyed() && hit.Alive() == 1);
    hit.RegisterHit();
    CHECK(hit.IsDestroyed());
}

static void TestFieldPlacement()
{
    PlayerField f;
    CHECK(f.Place(Ship(T("a1"), T("d1"))) == Field::PlaceResult::Ok);
    CHECK(f.Place(Ship(T("a2"))) == Field::PlaceResult::Blocked);       // touching
    CHECK(f.Place(Ship(T("e2"))) == Field::PlaceResult::Blocked);       // diagonal contact
    CHECK(f.Place(Ship(T("f1"))) == Field::PlaceResult::Ok);            // one tile gap is enough
    CHECK(f.Place(Ship(T("a1"), T("a5"))) == Field::PlaceResult::InvalidShip);
    CHECK(f.Place(Ship(T("a1"), T("b2"))) == Field::PlaceResult::InvalidShip);
    CHECK(f.Place(Ship(T("h8"), T("h10"))) == Field::PlaceResult::Ok);
    CHECK(f.Place(Ship(T("c8"), T("e8"))) == Field::PlaceResult::Ok);
    CHECK(f.Place(Ship(T("c4"), T("e4"))) == Field::PlaceResult::NoneLeft);   // a fleet has only two ships of length 3
    CHECK(f.Remaining(4) == 0);

    // Removing a ship frees its area completely.
    CHECK(f.RemoveLast(4) == true);
    CHECK(f.Remaining(4) == 1);
    CHECK(f.Place(Ship(T("a2"))) == Field::PlaceResult::Ok);
    CHECK(f.RemoveLast(1));
    CHECK(!f.RemoveLast(2));
    f.Clear();
    for(int tile = 0; tile < coord::kCount; ++tile)
        CHECK(f.Tile(tile).occupied == 0 && f.Tile(tile).type == TileType::Unknown);
}

static void TestRandomFleets()
{
    rnd::Seed(12345);
    for(int n = 0; n < 500; ++n)
    {
        PlayerField f;
        f.PlaceRandomly();
        CHECK(f.IsFleetComplete());
        int shipTiles = 0;
        for(int tile = 0; tile < coord::kCount; ++tile)
        {
            CHECK(f.Tile(tile).occupied >= 0);
            if(f.Tile(tile).type == TileType::ShipPart)
            {
                ++shipTiles;
                // Ships never touch: a ship tile is covered by exactly its own ship.
                CHECK(f.Tile(tile).occupied == 1);
                CHECK(f.Tile(tile).shipIndex >= 0);
            }
        }
        CHECK(shipTiles == 1 * 4 + 2 * 3 + 3 * 2 + 4 * 1);
        for(int length = 1; length <= kMaxShipLength; ++length)
            CHECK(f.Remaining(length) == 0);
    }
}

static void TestShooting()
{
    PlayerField f;
    f.Place(Ship(T("a1"), T("a2")));
    CHECK(f.ReceiveShot(T("b1")) == ShotOutcome::Miss);
    CHECK(f.ReceiveShot(T("b1")) == ShotOutcome::AlreadyShot);
    CHECK(f.ReceiveShot(T("a1")) == ShotOutcome::Hit);
    CHECK(f.ReceiveShot(T("a1")) == ShotOutcome::AlreadyShot);
    CHECK(f.HasAliveShips());
    CHECK(f.ReceiveShot(T("a2")) == ShotOutcome::Destroyed);
    CHECK(!f.HasAliveShips());
    CHECK(f.ReceiveShot(-1) == ShotOutcome::AlreadyShot);
    CHECK(f.ReceiveShot(100) == ShotOutcome::AlreadyShot);
}

static double PlayBotGames(BotPlayer::Difficulty difficulty, int games)
{
    long totalShots = 0;
    for(int g = 0; g < games; ++g)
    {
        PlayerField field;
        field.PlaceRandomly();
        BotPlayer bot(difficulty);
        std::set<int> shot;
        int shots = 0;
        while(field.HasAliveShips())
        {
            const BotPlayer::Shot s = bot.Shoot(field);
            CHECK(s.tile >= 0 && s.tile < coord::kCount);
            CHECK(s.outcome != ShotOutcome::AlreadyShot);
            CHECK(shot.insert(s.tile).second);          // never the same tile twice
            ++shots;
            if(shots > coord::kCount)
            {
                CHECK(false && "bot did not finish the game");
                break;
            }
        }
        totalShots += shots;
    }
    return static_cast<double>(totalShots) / games;
}

static void TestBot()
{
    rnd::Seed(2024);
    const double easy = PlayBotGames(BotPlayer::Difficulty::Easy, 1000);
    const double hard = PlayBotGames(BotPlayer::Difficulty::Hard, 1000);
    std::cout << "Average shots to win: easy = " << easy << ", hard = " << hard << '\n';
    CHECK(easy < 100 && hard < 100);
    CHECK(hard < easy);
}

int main()
{
    TestCoords();
    TestCommands();
    TestShip();
    TestFieldPlacement();
    TestRandomFleets();
    TestShooting();
    TestBot();

    if(failures == 0)
        std::cout << "All tests passed\n";
    else
        std::cerr << failures << " check(s) failed\n";
    return failures == 0 ? 0 : 1;
}
