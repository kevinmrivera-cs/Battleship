cmake_minimum_required(VERSION 3.20)
project(BattleShip LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include(CTest)

# Reusable, independently buildable library; kept in its existing folder.
add_subdirectory(DataStructures)

add_library(battleship_core
    src/model/Board.cpp
    src/model/Coordinate.cpp
    src/model/Player.cpp
    src/model/Ship.cpp
    src/view/ConsoleView.cpp
    src/controller/GameController.cpp
)
target_include_directories(battleship_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(battleship_core PUBLIC cxx_std_20)
target_link_libraries(battleship_core PUBLIC portfolio_datastructures)

add_executable(battleship src/main.cpp)
target_link_libraries(battleship PRIVATE battleship_core)

if(BUILD_TESTING)
    add_executable(battleship_tests tests/BattleshipTests.cpp)
    target_include_directories(battleship_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/DataStructures/tests")
    target_link_libraries(battleship_tests PRIVATE battleship_core)
    add_test(NAME BattleshipTests COMMAND battleship_tests)
endif()
