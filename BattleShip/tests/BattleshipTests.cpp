#include "TestHarness.h"
#include "battleship/controller/GameController.h"
#include "battleship/model/Board.h"
#include "battleship/model/Coordinate.h"
#include "battleship/model/Ship.h"
#include "battleship/view/ConsoleView.h"

#include <sstream>
#include <string>

using namespace kmr::battleship;

int main() {
    kmr::test::Runner runner;

    runner.add("Coordinate parser accepts board range", [] {
        auto a1 = Coordinate::parse("a1");
        auto j10 = Coordinate::parse(" J10 ");
        REQUIRE(a1.has_value()); REQUIRE_EQ(a1->row(), std::size_t{0}); REQUIRE_EQ(a1->column(), std::size_t{0});
        REQUIRE(j10.has_value()); REQUIRE_EQ(j10->row(), std::size_t{9}); REQUIRE_EQ(j10->column(), std::size_t{9});
        REQUIRE(!Coordinate::parse("K1")); REQUIRE(!Coordinate::parse("A0")); REQUIRE(!Coordinate::parse("A11"));
    });

    runner.add("Board rejects overlap and out of bounds", [] {
        Board board;
        REQUIRE(board.placeShip("Destroyer", 2, Coordinate(0,0), Orientation::Horizontal));
        REQUIRE(!board.placeShip("Overlap", 2, Coordinate(0,1), Orientation::Vertical));
        REQUIRE(!board.placeShip("TooFar", 5, Coordinate(9,9), Orientation::Horizontal));
        REQUIRE_EQ(board.shipCount(), std::size_t{1});
    });

    runner.add("Board attack hit miss duplicate and sunk", [] {
        Board board;
        REQUIRE(board.placeShip("Destroyer", 2, Coordinate(0,0), Orientation::Horizontal));
        REQUIRE(board.attack(Coordinate(9,9)) == AttackResult::Miss);
        REQUIRE(board.attack(Coordinate(9,9)) == AttackResult::AlreadyAttacked);
        REQUIRE(board.attack(Coordinate(0,0)) == AttackResult::Hit);
        REQUIRE(board.attack(Coordinate(0,1)) == AttackResult::Sunk);
        REQUIRE(board.allShipsSunk());
    });

    runner.add("ConsoleView reprompts bad coordinate", [] {
        std::istringstream input("Z99\nB3\n");
        std::ostringstream output;
        ConsoleView view(input, output);
        auto coordinate = view.requestCoordinate("Fire: ");
        REQUIRE(coordinate.has_value());
        REQUIRE_EQ(coordinate->toString(), std::string("B3"));
        REQUIRE(output.str().find("Invalid coordinate") != std::string::npos);
    });

    runner.add("Console enemy view hides unhit ships", [] {
        Board board;
        board.placeShip("Destroyer", 2, Coordinate(0,0), Orientation::Horizontal);
        std::istringstream input;
        std::ostringstream output;
        ConsoleView view(input, output);
        view.displayBoard(board, false);
        REQUIRE(output.str().find('S') == std::string::npos);
    });

    runner.add("Paired boards show attacks and hide unhit CPU ships", [] {
        Board human, computer;
        REQUIRE(human.placeShip("Destroyer", 2, Coordinate(0,0), Orientation::Horizontal));
        REQUIRE(computer.placeShip("Destroyer", 2, Coordinate(0,0), Orientation::Horizontal));
        human.attack(Coordinate(0,0));
        computer.attack(Coordinate(0,0));
        computer.attack(Coordinate(0,2));
        std::istringstream input;
        std::ostringstream output;
        ConsoleView view(input, output);
        view.displayBoards(human, computer);
        const std::string text = output.str();
        const auto rowStart = text.find("\nA   ");
        REQUIRE(rowStart != std::string::npos);
        const auto rowEnd = text.find('\n', rowStart + 1);
        const auto row = text.substr(rowStart + 1, rowEnd - rowStart - 1);
        REQUIRE(row.find("A   X S ~") == 0);
        REQUIRE(row.find("A   X ~ o", 1) != std::string::npos);
        REQUIRE(row.find('S', row.find("A   ", 1)) == std::string::npos);
    });

    runner.add("CPU responds once to a shot and both boards refresh", [] {
        std::istringstream input("Player\nA1\nH\nB1\nH\nC1\nH\nD1\nH\nE1\nH\nJ10\nQ\n");
        std::ostringstream output;
        ConsoleView view(input, output);
        GameController controller(view);
        controller.run();
        const auto text = output.str();
        const auto shot = text.find("Computer fired at ");
        REQUIRE(shot != std::string::npos);
        REQUIRE(text.find("Computer fired at ", shot + 1) == std::string::npos);
        REQUIRE(text.find("YOUR BOARD", shot) != std::string::npos);
        REQUIRE(text.find("CPU BOARD", shot) != std::string::npos);
        REQUIRE(text.find("Game ended.", shot) != std::string::npos);
    });

    runner.add("Sunk count changes only when a complete ship sinks", [] {
        Board board;
        REQUIRE_EQ(board.sunkShipCount(), std::size_t{0});
        board.placeShip("Destroyer", 2, Coordinate(0,0), Orientation::Horizontal);
        board.placeShip("Cruiser", 3, Coordinate(1,0), Orientation::Horizontal);
        board.attack(Coordinate(9,9));
        board.attack(Coordinate(0,0));
        REQUIRE_EQ(board.sunkShipCount(), std::size_t{0});
        REQUIRE(board.attack(Coordinate(0,1)) == AttackResult::Sunk);
        REQUIRE_EQ(board.sunkShipCount(), std::size_t{1});
        REQUIRE(board.attack(Coordinate(0,1)) == AttackResult::AlreadyAttacked);
        REQUIRE_EQ(board.sunkShipCount(), std::size_t{1});
        for (std::size_t col = 0; col < 3; ++col) board.attack(Coordinate(1,col));
        REQUIRE_EQ(board.sunkShipCount(), std::size_t{2});
        REQUIRE(board.allShipsSunk());
    });

    runner.add("Score credits attacker and announces named ships for both players", [] {
        Board human, computer;
        human.placeShip("Cruiser", 3, Coordinate(0,0), Orientation::Horizontal);
        computer.placeShip("Destroyer", 2, Coordinate(0,0), Orientation::Horizontal);
        computer.attack(Coordinate(0,0));
        computer.attack(Coordinate(0,1));
        std::istringstream input;
        std::ostringstream output;
        ConsoleView view(input, output);
        view.announceSunk(computer.ships().get(0), true);
        view.displayBoards(human, computer);
        REQUIRE(output.str().find("You: 1 / 1 | CPU: 0 / 1") != std::string::npos);
        REQUIRE(output.str().find("Yours: 1 | CPU: 0") != std::string::npos);
        REQUIRE(output.str().find("You sank the CPU's Destroyer (2 squares)!") != std::string::npos);
        REQUIRE(output.str().find("Destroyer (2): SUNK") != std::string::npos);
        for (std::size_t col = 0; col < 3; ++col) human.attack(Coordinate(0,col));
        view.announceSunk(human.ships().get(0), false);
        view.displayBoards(human, computer);
        REQUIRE(output.str().find("You: 1 / 1 | CPU: 1 / 1") != std::string::npos);
        REQUIRE(output.str().find("The CPU sank your Cruiser (3 squares)!") != std::string::npos);
    });

    return runner.run();
}


