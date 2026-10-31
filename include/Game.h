#pragma once

#include <string>

#include "AIField.h"
#include "BotPlayer.h"
#include "PlayerField.h"

class Game
{
public:
    explicit Game(BotPlayer::Difficulty difficulty, bool playerFirst = true);

    // Runs the whole game: ship placement, battle, result screen.
    void Run();

private:
    enum class Result { PlayerWon, BotWon, Quit };
    enum class Answer { Yes, No, Quit };

    bool SetupPlayerFleet();
    Answer AskToStart();
    void RenamePlayer();

    Result PlayMatch();
    std::string PlayerShoot(int tile);
    std::string BotShoot();
    void ShowHelp() const;
    void ShowResult(Result result);

    void DrawBoards() const;
    void ToggleBorders();

    PlayerField playerField_;
    AIField botField_;
    BotPlayer bot_;
    std::string playerName_ = "Player";
    bool playerTurn_;
    bool bordersVisible_ = true;
};
