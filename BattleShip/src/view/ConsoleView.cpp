#include "battleship/view/ConsoleView.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <string_view>

namespace kmr::battleship {

void ConsoleView::showInstructions() {
    output_ << "\nHOW TO PLAY\n"
            << "You play against the computer on separate 10 x 10 boards.\n"
            << "First place your ships. Then take turns firing at one enemy square.\n"
            << "Hit every square of a ship to sink it. Sink all enemy ships to win!\n"
            << "The computer's ships stay hidden until you hit them.\n"
            << "\nHOW TO PLACE A SHIP\n"
            << "Rows are letters A-J (top to bottom); columns are numbers 1-10 (left to right).\n"
            << "A coordinate combines both: B3 means row B, column 3.\n"
            << "1. Enter the ship's starting square, such as B3, and press Enter.\n"
            << "2. Enter H or V and press Enter. The game places the entire ship for you.\n"
            << "   H = horizontal: extends RIGHT from the starting square.\n"
            << "   V = vertical: extends DOWN from the starting square.\n"
            << "   Example for a 3-square ship starting at B3:\n"
            << "     H occupies B3, B4, B5. V occupies B3, C3, D3.\n"
            << "Ships must fit on the board and cannot overlap. They may touch.\n"
            << "You cannot move a ship after it has been placed.\n"
            << "Enter Q at a coordinate or direction prompt to quit.\n";
}
void ConsoleView::showMessage(const std::string& message) { output_ << message << '\n'; }

namespace {
// Shared row formatting keeps placement and battle boards consistent.
std::string boardRow(const Board& board, std::size_t row, bool revealShips) {
    std::string text(1, static_cast<char>('A' + row));
    text += "   ";
    for (std::size_t col = 0; col < Board::SIZE; ++col) {
        const Cell& cell = board.cellAt(Coordinate(row, col));
        char symbol = '~';
        if (cell.attacked) symbol = cell.shipIndex ? 'X' : 'o';
        else if (revealShips && cell.shipIndex) symbol = 'S';
        text += symbol;
        if (col + 1 < Board::SIZE) text += ' ';
    }
    return text;
}
}  // namespace

void ConsoleView::displayBoard(const Board& board, bool revealShips) const {
    output_ << (revealShips ? "Legend: ~ water | S your ship | X hit | o miss\n" : "Legend: ~ untried square | X hit | o miss\n");
    output_ << "    1 2 3 4 5 6 7 8 9 10\n";
    for (std::size_t row = 0; row < Board::SIZE; ++row) {
        output_ << boardRow(board, row, revealShips) << '\n';
    }
}

void ConsoleView::announceSunk(const Ship& ship, bool humanScored) {
    output_ << "\n*** " << (humanScored ? "You sank the CPU's " : "The CPU sank your ")
            << ship.name() << " (" << ship.length() << " squares)! ***\n";
}

void ConsoleView::displayBoards(const Board& human, const Board& computer) const {
    output_ << "\nSCORE - ships sunk\n"
            << "You: " << computer.sunkShipCount() << " / " << computer.shipCount()
            << " | CPU: " << human.sunkShipCount() << " / " << human.shipCount() << '\n'
            << "Ships remaining - Yours: " << human.shipCount() - human.sunkShipCount()
            << " | CPU: " << computer.shipCount() - computer.sunkShipCount() << '\n';
    output_ << "CPU fleet: ";
    for (std::size_t i = 0; i < computer.shipCount(); ++i) {
        const Ship& ship = computer.ships().get(i);
        if (i != 0) output_ << " | ";
        output_ << ship.name() << " (" << ship.length() << "): "
                << (ship.isSunk() ? "SUNK" : "remaining");
    }
    output_ << '\n';
    output_ << "\nYOUR BOARD                  CPU BOARD\n"
            << "CPU fires here              You fire here\n"
            << "    1 2 3 4 5 6 7 8 9 10        1 2 3 4 5 6 7 8 9 10\n";
    for (std::size_t row = 0; row < Board::SIZE; ++row) {
        output_ << boardRow(human, row, true) << "     "
                << boardRow(computer, row, false) << '\n';
    }
    output_ << "S = your ship | X = hit | o = miss\n"
            << "~ = water on your board; untried square on the CPU board.\n"
            << "Unhit CPU ships are hidden.\n\n";
}
std::optional<std::string> ConsoleView::readSafeLine(const std::string& prompt) {
    output_ << prompt;
    std::string line;
    if (!std::getline(input_, line)) return std::nullopt;
    if (line.size() > 64) {
        output_ << "Input is too long.\n";
        return std::string{};
    }
    return line;
}

std::optional<Coordinate> ConsoleView::requestCoordinate(const std::string& prompt) {
    while (true) {
        auto line = readSafeLine(prompt);
        if (!line) return std::nullopt;
        if (*line == "q" || *line == "Q") return std::nullopt;
        if (auto coordinate = Coordinate::parse(*line)) return coordinate;
        output_ << "Invalid coordinate. Enter A1 through J10, or Q to quit.\n";
    }
}

std::optional<Orientation> ConsoleView::requestOrientation(const std::string& prompt) {
    while (true) {
        auto line = readSafeLine(prompt);
        if (!line) return std::nullopt;
        if (*line == "q" || *line == "Q") return std::nullopt;
        line->erase(std::remove_if(line->begin(), line->end(), [](unsigned char ch) { return std::isspace(ch) != 0; }), line->end());
        if (line->size() == 1) {
            const char value = static_cast<char>(std::toupper(static_cast<unsigned char>((*line)[0])));
            if (value == 'H') return Orientation::Horizontal;
            if (value == 'V') return Orientation::Vertical;
        }
        output_ << "Invalid orientation. Enter H to extend RIGHT, V to extend DOWN, or Q to quit.\n";
    }
}

std::string ConsoleView::requestPlayerName() {
    while (true) {
        auto line = readSafeLine("Your name: ");
        if (!line) return "Player";
        const auto first = line->find_first_not_of(" \t\r\n");
        const auto last = line->find_last_not_of(" \t\r\n");
        if (first != std::string::npos) {
            std::string name = line->substr(first, last - first + 1);
            if (name.size() <= 40) return name;
        }
        output_ << "Name must contain 1-40 visible characters.\n";
    }
}

}  // namespace kmr::battleship



