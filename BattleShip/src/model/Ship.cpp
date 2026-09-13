#include "battleship/model/Ship.h"
#include <stdexcept>
#include <utility>

namespace kmr::battleship {

Ship::Ship(std::string name, std::size_t length) : name_(std::move(name)), length_(length) {
    if (name_.empty()) throw std::invalid_argument("Ship name cannot be empty");
    if (length_ == 0 || length_ > 5) throw std::invalid_argument("Ship length must be between 1 and 5");
}

void Ship::registerHit() {
    if (hits_ < length_) ++hits_;
}

}  // namespace kmr::battleship
