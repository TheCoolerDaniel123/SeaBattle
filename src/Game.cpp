#include "Game.h"

#include <iostream>

#include "Command.h"
#include "Console.h"
#include "Coord.h"
#include "StringUtil.h"

namespace
{
    constexpr std::size_t kMaxNameLength = 20;
}

Game::Game(BotPlayer::Difficulty difficulty, bool playerFirst)
    : bot_(difficulty), playerTurn_(playerFirst)
{
}

void Game::Run()
{
    if(!SetupPlayerFleet())
        return;
    botField_.PlaceRandomly();

    const Result result = PlayMatch();
    if(result != Result::Quit)
        ShowResult(result);
}

void Game::DrawBoards() const
{
    console::Clear();
    playerField_.Draw(std::cout);
    std::cout << '\n';
    botField_.Draw(std::cout);
}

void Game::ToggleBorders()
{
    bordersVisible_ = !bordersVisible_;
    playerField_.SetBorders(bordersVisible_);
    botField_.SetBorders(bordersVisible_);
}

// ------------------------------------------------------------- ship placement

bool Game::SetupPlayerFleet()
{
    std::string note;
    for(;;)
    {
        if(playerField_.IsFleetComplete())
        {
            switch(AskToStart())
            {
                case Answer::Yes:
                    return true;
                case Answer::Quit:
                    return false;
                case Answer::No:
                    playerField_.RemoveLast();  // reopen one slot so something can be changed
                    note = "The last ship was removed, place it again or use other commands";
                    break;
            }
            continue;
        }

        console::Clear();
        playerField_.Draw(std::cout);
        std::cout << "\nTo start, you have to place:\n";
        for(int length = 1; length <= kMaxShipLength; ++length)
            std::cout << playerField_.Remaining(length) << " ship(s) of length " << length << '\n';
        if(!note.empty())
        {
            std::cout << '\n' << note << '\n';
            note.clear();
        }
        std::cout << '\n' << playerName_ << "> ";

        std::string line;
        if(!console::ReadLine(line))
            return false;

        const Command command = ParseCommand(line);
        switch(command.type)
        {
            case Command::Type::Tiles:
            {
                const Ship ship = command.second >= 0 ? Ship(command.first, command.second)
                                                      : Ship(command.first);
                switch(playerField_.Place(ship))
                {
                    case Field::PlaceResult::Ok:
                        break;
                    case Field::PlaceResult::InvalidShip:
                        note = "A ship must be a straight line of 1 to " + std::to_string(kMaxShipLength) + " tiles";
                        break;
                    case Field::PlaceResult::Blocked:
                        note = "A ship can't be placed there: the tiles are taken or too close to another ship";
                        break;
                    case Field::PlaceResult::NoneLeft:
                        note = "You have no ships of length " + std::to_string(ship.Length()) + " left to place";
                        break;
                }
                break;
            }
            case Command::Type::Delete:
                if(!playerField_.RemoveLast(command.length))
                    note = "There is no such ship to delete";
                break;
            case Command::Type::Clear:
                playerField_.Clear();
                break;
            case Command::Type::Random:
                playerField_.PlaceRandomly();
                break;
            case Command::Type::Name:
                RenamePlayer();
                break;
            case Command::Type::Hints:
                playerField_.ToggleHints();
                break;
            case Command::Type::Borders:
                ToggleBorders();
                break;
            case Command::Type::Quit:
                return false;
            case Command::Type::Help:
                note = "The list of commands is shown to the right of the field";
                break;
            case Command::Type::Reveal:
            case Command::Type::Unknown:
                note = "Unknown command, example of placing a ship: a4c4";
                break;
        }
    }
}

Game::Answer Game::AskToStart()
{
    for(;;)
    {
        console::Clear();
        playerField_.Draw(std::cout);
        std::cout << "\nDo you want to start the game with this field?\n"
                     "y - Yes, let's go\n"
                     "n - No, let me change something\n> ";

        std::string line;
        if(!console::ReadLine(line))
            return Answer::Quit;
        line = text::ToLower(text::Trim(line));
        if(line == "y" || line == "yes")
            return Answer::Yes;
        if(line == "n" || line == "no")
            return Answer::No;
        if(line == "quit" || line == "exit")
            return Answer::Quit;
    }
}

void Game::RenamePlayer()
{
    console::Clear();
    playerField_.Draw(std::cout);
    std::cout << "\nEnter your name: ";

    std::string name;
    if(!console::ReadLine(name))
        return;
    name = text::Trim(name);
    if(name.empty())
        return;
    playerName_ = name.substr(0, kMaxNameLength);
}

// --------------------------------------------------------------------- battle

Game::Result Game::PlayMatch()
{
    std::string event = "Your first move; type 'help' for more info";
    for(;;)
    {
        if(playerTurn_)
        {
            DrawBoards();
            std::cout << event << '\n' << playerName_ << "> ";

            std::string line;
            if(!console::ReadLine(line))
                return Result::Quit;

            const Command command = ParseCommand(line);
            switch(command.type)
            {
                case Command::Type::Tiles:
                    if(command.second >= 0)
                        event = "Shoot one tile at a time, for example: a1";
                    else
                        event = PlayerShoot(command.first);
                    break;
                case Command::Type::Help:
                    ShowHelp();
                    break;
                case Command::Type::Borders:
                    ToggleBorders();
                    break;
                case Command::Type::Quit:
                    return Result::Quit;
#ifdef SEABATTLE_DEBUG
                case Command::Type::Reveal:
                    botField_.ToggleReveal();
                    break;
#endif
                default:
                    event = "Unknown command, type 'help' for more info";
                    break;
            }
            if(!botField_.HasAliveShips())
                return Result::PlayerWon;
        }
        else
        {
            event = BotShoot();
            DrawBoards();
            std::cout << event << '\n';
            if(!playerField_.HasAliveShips())
                return Result::BotWon;
            console::WaitForEnter();
        }
    }
}

std::string Game::PlayerShoot(int tile)
{
    const std::string where = coord::Name(tile);
    switch(botField_.ReceiveShot(tile))
    {
        case ShotOutcome::Miss:
            playerTurn_ = false;
            return "You missed on " + where + ", now it's SeaFighter's turn";
        case ShotOutcome::Hit:
            return "You hit a ship on " + where + ", shoot again";
        case ShotOutcome::Destroyed:
            return "You destroyed the ship, shooting its final part on " + where + ", shoot again";
        case ShotOutcome::AlreadyShot:
            break;
    }
    return "You can't shoot the same tile twice, make another choice";
}

std::string Game::BotShoot()
{
    const BotPlayer::Shot shot = bot_.Shoot(playerField_);
    const std::string where = coord::Name(shot.tile);
    switch(shot.outcome)
    {
        case ShotOutcome::Miss:
            playerTurn_ = true;
            return "SeaFighter shot " + where + " and missed, now it's your turn";
        case ShotOutcome::Hit:
            return "SeaFighter shot " + where + " and hit your ship, it shoots again";
        case ShotOutcome::Destroyed:
            return "SeaFighter shot " + where + " and destroyed your ship, it shoots again";
        case ShotOutcome::AlreadyShot:
            break;
    }
    playerTurn_ = true;  // the bot had nothing to shoot at
    return "SeaFighter has no tiles left to shoot";
}

void Game::ShowHelp() const
{
    console::Clear();
    std::cout << "The format of input is: '[tile_to_shoot]'\n"
                 "Example: a1\n\n"
                 "Other commands:\n"
                 "borders - get rid of the borders between tiles (or bring them back)\n"
                 "quit    - leave the game\n\n";
    console::WaitForEnter("Press Enter to go back to the game...");
}

void Game::ShowResult(Result result)
{
    botField_.RevealShips(true);
    DrawBoards();
    if(result == Result::PlayerWon)
        std::cout << "\nYou destroyed the whole enemy fleet. Congratulations, " << playerName_ << "!\n\n";
    else
        std::cout << "\nSeaFighter destroyed your fleet. Better luck next time!\n\n";
    console::WaitForEnter("Press Enter to exit...");
}
