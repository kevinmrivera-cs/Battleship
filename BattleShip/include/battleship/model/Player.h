#pragma once

#include "battleship/model/Board.h"
#include <string>

namespace kmr::battleship {

/**
 * @class Player
 * @author Kevin Munoz-Rivera
 * @brief Combines a player name with that player's board.
 */
class Player {
public:
    explicit Player(std::string name);
    [[nodiscard]] const std::string& name() const noexcept { return name_; }
    Board& board() noexcept { return board_; }
    const Board& board() const noexcept { return board_; }
private:
    std::string name_;
    Board board_;
};

}  // namespace kmr::battleship
