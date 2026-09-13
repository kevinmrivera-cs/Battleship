#pragma once
#include <cstddef>
#include <optional>

namespace kmr::battleship {

/**
 * @struct Cell
 * @author Kevin Munoz-Rivera
 * @brief One board square, including optional ship ownership and attack state.
 */
struct Cell {
    std::optional<std::size_t> shipIndex;
    bool attacked{false};
};

}  // namespace kmr::battleship
