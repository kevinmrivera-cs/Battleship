#pragma once

#include "battleship/model/Board.h"
#include "battleship/model/Coordinate.h"
#include <istream>
#include <optional>
#include <ostream>
#include <string>

namespace kmr::battleship {

/**
 * @class ConsoleView
 * @author Kevin Munoz-Rivera
 * @brief Console-only View. It performs terminal input/output but owns no game rules.
 */
class ConsoleView {
public:
    ConsoleView(std::istream& input, std::ostream& output) : input_(input), output_(output) {}

    /** @brief Explains the goal, coordinates, and ship placement to first-time players. */
    void showInstructions();
    void showMessage(const std::string& message);
    void displayBoard(const Board& board, bool revealShips) const;
    /** @brief Shows both boards together, keeping unhit CPU ships hidden. */
    /** @brief Announces the name and size of a newly sunk ship. */
    void announceSunk(const Ship& ship, bool humanScored);
    void displayBoards(const Board& human, const Board& computer) const;
    std::optional<Coordinate> requestCoordinate(const std::string& prompt);
    std::optional<Orientation> requestOrientation(const std::string& prompt);
    std::string requestPlayerName();

private:
    std::istream& input_;
    std::ostream& output_;

    std::optional<std::string> readSafeLine(const std::string& prompt);
};

}  // namespace kmr::battleship



