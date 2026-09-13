#include "battleship/controller/GameController.h"
#include "battleship/view/ConsoleView.h"
#include <exception>
#include <iostream>

int main() {
    try {
        kmr::battleship::ConsoleView view(std::cin, std::cout);
        kmr::battleship::GameController controller(view);
        controller.run();
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << '\n';
        return 1;
    }
}
