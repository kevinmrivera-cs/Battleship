#include "battleship/controller/GameController.h"

#include <chrono>
#include <stdexcept>

namespace kmr::battleship {

GameController::GameController(ConsoleView& view)
    : view_(view), rng_(static_cast<std::mt19937::result_type>(
          std::chrono::steady_clock::now().time_since_epoch().count())) {}

void GameController::run() {
    view_.showMessage("=== Battleship ===");
    view_.showInstructions();
    human_ = Player(view_.requestPlayerName());
    if (!setupHuman()) { state_ = GameState::Quit; view_.showMessage("Game cancelled."); return; }
    setupComputer();
    view_.showMessage("\nYour fleet is ready! The computer has placed its ships too.\nBATTLE: Choose a square on the enemy board to fire at, such as B3.\nX marks a hit; o marks a miss. Choose a new square each turn.\nYou fire first. Enter Q to quit.");
    state_ = GameState::InProgress;

    view_.displayBoards(human_.board(), computer_.board());
    while (state_ == GameState::InProgress) {
        if (!humanTurn()) { state_ = GameState::Quit; break; }
        view_.displayBoards(human_.board(), computer_.board());
        if (computer_.board().allShipsSunk()) { state_ = GameState::HumanWon; break; }
        view_.showMessage("CPU turn - the computer fires at your board.");
        computerTurn();
        view_.displayBoards(human_.board(), computer_.board());
        if (human_.board().allShipsSunk()) { state_ = GameState::ComputerWon; break; }
    }
    if (state_ == GameState::HumanWon) view_.showMessage("You sank the entire enemy fleet. You win!");
    else if (state_ == GameState::ComputerWon) view_.showMessage("The computer sank your fleet.");
    else if (state_ == GameState::Quit) view_.showMessage("Game ended.");
}

bool GameController::setupHuman() {
    view_.showMessage("\nYOUR FLEET (place each ship once):");
    for (const auto& spec : FLEET) {
        view_.showMessage(std::string("  ") + spec.name + " - " + std::to_string(spec.length) + " squares");
    }
    std::size_t shipNumber = 0;
    for (const auto& spec : FLEET) {
        ++shipNumber;
        view_.showMessage("\nShip " + std::to_string(shipNumber) + " of " + std::to_string(FLEET.size()) + ": " + spec.name + " (" + std::to_string(spec.length) + " squares)");
        while (true) {
            view_.displayBoard(human_.board(), true);
            auto coordinate = view_.requestCoordinate(std::string("Place ") + spec.name + " (length " + std::to_string(spec.length) + " squares). Starting square (e.g. B3, or Q to quit): ");
            if (!coordinate) return false;
            auto orientation = view_.requestOrientation("Direction: H = RIGHT, V = DOWN (or Q to quit): ");
            if (!orientation) return false;
            if (human_.board().placeShip(spec.name, spec.length, *coordinate, *orientation)) {
                const Coordinate end(
                    coordinate->row() + (*orientation == Orientation::Vertical ? spec.length - 1 : 0),
                    coordinate->column() + (*orientation == Orientation::Horizontal ? spec.length - 1 : 0));
                view_.showMessage(std::string(spec.name) + " placed from " + coordinate->toString() + " to " + end.toString() + ".");
                break;
            }
            view_.showMessage("Ship not placed: it overlaps an existing ship or extends past the board.\nChoose a new starting square with enough free squares to its RIGHT (H) or DOWN (V).");
        }
    }
    return true;
}

void GameController::setupComputer() {
    std::uniform_int_distribution<int> coordinate(0, 9);
    std::uniform_int_distribution<int> orientation(0, 1);
    for (const auto& spec : FLEET) {
        bool placed = false;
        for (int attempts = 0; attempts < 10000 && !placed; ++attempts) {
            const Coordinate start(static_cast<std::size_t>(coordinate(rng_)), static_cast<std::size_t>(coordinate(rng_)));
            const Orientation direction = orientation(rng_) == 0 ? Orientation::Horizontal : Orientation::Vertical;
            placed = computer_.board().placeShip(spec.name, spec.length, start, direction);
        }
        if (!placed) throw std::runtime_error("Computer fleet placement failed after bounded retries");
    }
}

bool GameController::humanTurn() {
    while (true) {
        auto coordinate = view_.requestCoordinate("Your turn - enemy square to fire at (e.g. B3, or Q to quit): ");
        if (!coordinate) return false;
        const AttackResult result = computer_.board().attack(*coordinate);
        view_.showMessage(attackMessage(result, *coordinate, human_.name()));
        if (result == AttackResult::Sunk) {
            view_.announceSunk(computer_.board().ships().get(*computer_.board().cellAt(*coordinate).shipIndex), true);
        }
        if (result != AttackResult::AlreadyAttacked) return true;
    }
}

void GameController::computerTurn() {
    const Coordinate coordinate = nextComputerTarget();
    const AttackResult result = human_.board().attack(coordinate);
    view_.showMessage(attackMessage(result, coordinate, computer_.name()));
    if (result == AttackResult::Sunk) {
        view_.announceSunk(human_.board().ships().get(*human_.board().cellAt(coordinate).shipIndex), false);
    }
    if (result == AttackResult::Hit) queueAdjacentTargets(coordinate);
}

Coordinate GameController::nextComputerTarget() {
    while (!targetQueue_.isEmpty()) {
        Coordinate candidate = targetQueue_.dequeue();
        if (!wasAttacked(human_.board(), candidate)) return candidate;
    }
    std::uniform_int_distribution<int> coordinate(0, 9);
    for (int attempts = 0; attempts < 1000; ++attempts) {
        Coordinate candidate(static_cast<std::size_t>(coordinate(rng_)), static_cast<std::size_t>(coordinate(rng_)));
        if (!wasAttacked(human_.board(), candidate)) return candidate;
    }
    // Deterministic fallback guarantees progress even after unlucky random retries.
    for (std::size_t row = 0; row < Board::SIZE; ++row) {
        for (std::size_t col = 0; col < Board::SIZE; ++col) {
            Coordinate candidate(row, col);
            if (!wasAttacked(human_.board(), candidate)) return candidate;
        }
    }
    throw std::logic_error("No valid computer target remains");
}

void GameController::queueAdjacentTargets(const Coordinate& hit) {
    const int row = static_cast<int>(hit.row());
    const int col = static_cast<int>(hit.column());
    const std::array<std::pair<int, int>, 4> offsets{{{-1,0},{1,0},{0,-1},{0,1}}};
    for (const auto& [dr, dc] : offsets) {
        const int nextRow = row + dr;
        const int nextCol = col + dc;
        if (nextRow >= 0 && nextRow < 10 && nextCol >= 0 && nextCol < 10) {
            Coordinate candidate(static_cast<std::size_t>(nextRow), static_cast<std::size_t>(nextCol));
            if (!wasAttacked(human_.board(), candidate)) targetQueue_.enqueue(candidate);
        }
    }
}

bool GameController::wasAttacked(const Board& board, const Coordinate& coordinate) const {
    return board.cellAt(coordinate).attacked;
}

std::string GameController::attackMessage(AttackResult result, const Coordinate& coordinate, const std::string& actor) {
    std::string message = actor + " fired at " + coordinate.toString() + ": ";
    switch (result) {
        case AttackResult::Miss: return message + "miss.";
        case AttackResult::Hit: return message + "hit!";
        case AttackResult::Sunk: return message + "ship sunk!";
        case AttackResult::AlreadyAttacked: return message + "already attacked; choose another coordinate.";
    }
    return message;
}

}  // namespace kmr::battleship



