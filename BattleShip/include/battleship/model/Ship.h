#pragma once

#include <cstddef>
#include <string>

namespace kmr::battleship {

/**
 * @class Ship
 * @author Kevin Munoz-Rivera
 * @brief Stores a Battleship vessel's name, length, and accumulated hits.
 */
class Ship {
public:
    Ship(std::string name, std::size_t length);

    /** @brief Records one hit, never allowing hits to exceed ship length. */
    void registerHit();
    [[nodiscard]] bool isSunk() const noexcept { return hits_ >= length_; }
    [[nodiscard]] const std::string& name() const noexcept { return name_; }
    [[nodiscard]] std::size_t length() const noexcept { return length_; }
    [[nodiscard]] std::size_t hits() const noexcept { return hits_; }

private:
    std::string name_;
    std::size_t length_;
    std::size_t hits_{0};
};

}  // namespace kmr::battleship
