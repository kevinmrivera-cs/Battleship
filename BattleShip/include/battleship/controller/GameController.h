#pragma once

#include "battleship/model/GameState.h"
#include "battleship/model/Player.h"
#include "battleship/view/ConsoleView.h"
#include "kmr/ds/CircularArrayQueue.h"
#include <array>
#include <random>
#include <string>

namespace kmr::battleship {

/**
 * @class GameController
 * @author Kevin Munoz-Rivera
 * @brief Coordinates model changes and View interactions for a console Battleship game.
 */
class GameController {
public:
    explicit GameController(ConsoleView& view);

    /** @brief Runs one complete human-vs-computer game until win or quit. */
    void run();

private:
    struct FleetSpec { const char* name; std::size_t length; };
    static constexpr std::array<FleetSpec, 5> FLEET{{
        {"Carrier", 5}, {"Battleship", 4}, {"Cruiser", 3}, {"Submarine", 3}, {"Destroyer", 2}
    }};

    ConsoleView& view_;
    Player human_{"Player"};
    Player computer_{"Computer"};
    GameState state_{GameState::Setup};
    std::mt19937 rng_;
    kmr::ds::CircularArrayQueue<Coordinate> targetQueue_;

    bool setupHuman();
    void setupComputer();
    bool humanTurn();
    void computerTurn();
    Coordinate nextComputerTarget();
    void queueAdjacentTargets(const Coordinate& hit);
    bool wasAttacked(const Board& board, const Coordinate& coordinate) const;
    static std::string attackMessage(AttackResult result, const Coordinate& coordinate, const std::string& actor);
};

}  // namespace kmr::battleship
