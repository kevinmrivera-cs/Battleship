#pragma once

#include "battleship/model/Cell.h"
#include "battleship/model/Coordinate.h"
#include "battleship/model/Ship.h"
#include "kmr/ds/MyArrayList.h"
#include <array>
#include <cstddef>
#include <string>

namespace kmr::battleship {

enum class Orientation { Horizontal, Vertical };
enum class AttackResult { Miss, Hit, Sunk, AlreadyAttacked };

/**
 * @class Board
 * @author Kevin Munoz-Rivera
 * @brief Owns the 10x10 board, fleet, placement rules, and attack rules.
 */
class Board {
public:
    static constexpr std::size_t SIZE = 10;

    /**
     * @brief Attempts to place a ship.
     * @return true when placement is valid; false for overlap or out-of-bounds placement.
     */
    bool placeShip(const std::string& name, std::size_t length,
                   const Coordinate& start, Orientation orientation);

    /** @brief Attacks one valid coordinate and reports the outcome. */
    AttackResult attack(const Coordinate& coordinate);

    [[nodiscard]] const Cell& cellAt(const Coordinate& coordinate) const;
    [[nodiscard]] const kmr::ds::MyArrayList<Ship>& ships() const noexcept { return ships_; }
    /** @brief Counts fully sunk ships; repeated shots never add to the score. */
    [[nodiscard]] std::size_t sunkShipCount() const noexcept;
    [[nodiscard]] bool allShipsSunk() const noexcept;
    [[nodiscard]] std::size_t shipCount() const noexcept { return ships_.size(); }

private:
    std::array<Cell, SIZE * SIZE> cells_{};
    kmr::ds::MyArrayList<Ship> ships_;

    static std::size_t indexOf(const Coordinate& coordinate) noexcept;
};

}  // namespace kmr::battleship

