#include "battleship/model/Player.h"
#include <stdexcept>
#include <utility>

namespace kmr::battleship {
Player::Player(std::string name) : name_(std::move(name)) {
    if (name_.empty() || name_.size() > 40) throw std::invalid_argument("Player name must contain 1-40 characters");
}
}  // namespace kmr::battleship
