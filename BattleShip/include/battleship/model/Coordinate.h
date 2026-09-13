#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

namespace kmr::battleship {

/**
 * @class Coordinate
 * @author Kevin Munoz-Rivera
 * @brief Immutable-style board coordinate using zero-based row and column values.
 */
class Coordinate {
public:
    static constexpr std::size_t BOARD_SIZE = 10;

    Coordinate(std::size_t row, std::size_t column);

    [[nodiscard]] std::size_t row() const noexcept { return row_; }
    [[nodiscard]] std::size_t column() const noexcept { return column_; }

    /** @brief Parses A1 through J10, ignoring surrounding whitespace and letter case. */
    static std::optional<Coordinate> parse(std::string_view text);
    /** @brief Returns the display form such as B7. */
    [[nodiscard]] std::string toString() const;

    friend bool operator==(const Coordinate&, const Coordinate&) = default;

private:
    std::size_t row_;
    std::size_t column_;
};

}  // namespace kmr::battleship
