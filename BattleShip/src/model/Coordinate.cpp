#include "battleship/model/Coordinate.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <stdexcept>

namespace kmr::battleship {

Coordinate::Coordinate(std::size_t row, std::size_t column) : row_(row), column_(column) {
    if (row >= BOARD_SIZE || column >= BOARD_SIZE) {
        throw std::out_of_range("Coordinate must be inside the 10x10 board");
    }
}

std::optional<Coordinate> Coordinate::parse(std::string_view text) {
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front())) != 0) text.remove_prefix(1);
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back())) != 0) text.remove_suffix(1);
    if (text.size() < 2 || text.size() > 3) return std::nullopt;

    const char rowChar = static_cast<char>(std::toupper(static_cast<unsigned char>(text.front())));
    if (rowChar < 'A' || rowChar > 'J') return std::nullopt;

    int columnNumber = 0;
    const auto digits = text.substr(1);
    const char* begin = digits.data();
    const char* end = digits.data() + digits.size();
    const auto result = std::from_chars(begin, end, columnNumber);
    if (result.ec != std::errc{} || result.ptr != end || columnNumber < 1 || columnNumber > 10) return std::nullopt;

    return Coordinate(static_cast<std::size_t>(rowChar - 'A'), static_cast<std::size_t>(columnNumber - 1));
}

std::string Coordinate::toString() const {
    std::string result;
    result.push_back(static_cast<char>('A' + row_));
    result += std::to_string(column_ + 1);
    return result;
}

}  // namespace kmr::battleship
