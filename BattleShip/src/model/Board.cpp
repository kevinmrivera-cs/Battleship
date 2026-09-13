#include "battleship/model/Board.h"

namespace kmr::battleship {

std::size_t Board::indexOf(const Coordinate& coordinate) noexcept {
    return coordinate.row() * SIZE + coordinate.column();
}

bool Board::placeShip(const std::string& name, std::size_t length,
                      const Coordinate& start, Orientation orientation) {
    if (length == 0 || length > 5) return false;
    const std::size_t endRow = start.row() + (orientation == Orientation::Vertical ? length - 1 : 0);
    const std::size_t endCol = start.column() + (orientation == Orientation::Horizontal ? length - 1 : 0);
    if (endRow >= SIZE || endCol >= SIZE) return false;

    for (std::size_t offset = 0; offset < length; ++offset) {
        const std::size_t row = start.row() + (orientation == Orientation::Vertical ? offset : 0);
        const std::size_t col = start.column() + (orientation == Orientation::Horizontal ? offset : 0);
        const Coordinate coordinate(row, col);
        if (cells_[indexOf(coordinate)].shipIndex.has_value()) return false;
    }

    const std::size_t shipIndex = ships_.size();
    ships_.add(Ship(name, length));
    for (std::size_t offset = 0; offset < length; ++offset) {
        const Coordinate coordinate(
            start.row() + (orientation == Orientation::Vertical ? offset : 0),
            start.column() + (orientation == Orientation::Horizontal ? offset : 0));
        cells_[indexOf(coordinate)].shipIndex = shipIndex;
    }
    return true;
}

AttackResult Board::attack(const Coordinate& coordinate) {
    Cell& cell = cells_[indexOf(coordinate)];
    if (cell.attacked) return AttackResult::AlreadyAttacked;
    cell.attacked = true;
    if (!cell.shipIndex) return AttackResult::Miss;

    Ship& ship = ships_.get(*cell.shipIndex);
    ship.registerHit();
    return ship.isSunk() ? AttackResult::Sunk : AttackResult::Hit;
}

const Cell& Board::cellAt(const Coordinate& coordinate) const {
    return cells_[indexOf(coordinate)];
}

std::size_t Board::sunkShipCount() const noexcept {
    std::size_t count = 0;
    for (std::size_t i = 0; i < ships_.size(); ++i) {
        if (ships_.get(i).isSunk()) ++count;
    }
    return count;
}

bool Board::allShipsSunk() const noexcept {
    if (ships_.isEmpty()) return false;
    for (std::size_t i = 0; i < ships_.size(); ++i) {
        if (!ships_.get(i).isSunk()) return false;
    }
    return true;
}

}  // namespace kmr::battleship

