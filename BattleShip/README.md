# Battleship in C++

**Author: Kevin Munoz-Rivera**

A console Battleship class project built with C++20 and CMake. Play against a computer opponent while exploring reusable data structures adapted from Java coursework to C++.

## Features

- Human-versus-CPU gameplay on two 10 x 10 boards.
- Step-by-step ship placement with coordinate and direction examples.
- Side-by-side player and CPU boards during battle.
- Automatic CPU turns with queued follow-up targets after a hit.
- Live scores showing ships sunk and ships remaining for both players.
- Named ship-sinking announcements and a CPU fleet status list.
- Checks for invalid input, overlapping ships, out-of-bounds placement, and repeated shots.
- A separate DataStructures library and automated C++ tests.

## How to play

Your goal is to sink all five CPU ships before the CPU sinks yours.

| Ship | Squares occupied |
| --- | --- |
| Carrier | 5 |
| Battleship | 4 |
| Cruiser | 3 |
| Submarine | 3 |
| Destroyer | 2 |

1. Enter your name.
2. For each ship, enter a starting coordinate: `B3` means row B, column 3. Rows run from A to J; columns run from 1 to 10.
3. Enter `H` to extend the ship **right**, or `V` to extend it **down**. A three-square ship starting at B3 occupies B3, B4, B5 horizontally, or B3, C3, D3 vertically.
4. Keep ships within the board and avoid overlaps. Ships may touch, but cannot be moved after placement.
5. Once your fleet is ready, enter an enemy coordinate to fire. The CPU responds automatically after each valid shot unless that shot ends the game.
6. Use the board markers and remaining ship sizes to choose your next shot. Enter `Q` at a coordinate or direction prompt to quit.

| Marker | Meaning |
| --- | --- |
| `S` | Your unhit ship |
| `X` | A hit |
| `o` | A miss |
| `~` | Water on your board; an untried square on the CPU board |

Unhit CPU ships stay hidden. The score counts fully sunk ships, not individual hits. Sinking a ship reveals its name and size in an announcement.

## Requirements

- A compiler supporting C++20.
- CMake 3.20 or newer.
- A compatible build tool, such as Ninja.
- CLion is optional; it provides a convenient CMake workflow.

The game uses the C++ standard library and the included DataStructures library. No game engine or external testing framework is required.

## Build and run in CLion

1. Open the repository folder containing the top-level `CMakeLists.txt`.
2. Configure a C++ toolchain. On Windows, the existing project was verified with CLion's bundled MinGW toolchain.
3. Load or reload the CMake project using the Debug profile.
4. Select the **Battleship** CMake run configuration, which builds the `battleship` target.
5. Click **Run** and use the console to play.

Build the complete CMake target rather than running `main.cpp` as a single file: the game also needs its model, controller, view, and library. CMake supplies the include paths automatically.

## Build from a terminal

Run these commands from the repository root with CMake, Ninja, and your C++ compiler available in the terminal:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

Run the game on Windows using PowerShell:

```powershell
.\build\battleship.exe
```

On Linux or macOS with a suitable toolchain:

```sh
./build/battleship
```

The current project has been verified on Windows with CLion's bundled GNU 15.2.0 compiler and Ninja. Other platforms have not been verified in this workspace.

Tests are enabled by default. Add `-DBUILD_TESTING=OFF` to the configure command for a game-only build.

## Project organization

```text
BattleShip/
|-- CMakeLists.txt
|-- README.md
|-- PROJECT_STATUS.md
|-- .run/                      Shared CLion run configuration
|-- include/battleship/        Game declarations (.h)
|   |-- model/
|   |-- controller/
|   `-- view/
|-- src/                       Game implementations (.cpp)
|   |-- main.cpp
|   |-- model/
|   |-- controller/
|   `-- view/
|-- tests/                     Battleship tests
`-- DataStructures/            Independently buildable C++ library
    |-- CMakeLists.txt
    |-- README.md
    |-- include/kmr/ds/        Template and data-structure headers
    `-- tests/                 Library tests and shared test harness
```

### Why both include and src?

These folders are complementary. For example, `Board.h` declares the Board class and its available operations; `Board.cpp` implements those operations. They are not duplicate files. Many reusable data structures are templates, so their implementations are kept in headers where the compiler can access them.

### Code responsibilities

- **Model:** stores ships, boards, coordinates, and game state; applies placement and attack rules.
- **View:** displays instructions, boards, scores, and messages; reads console input.
- **Controller:** manages setup, player and CPU turns, and game completion.

DataStructures remains separate from the game logic and can be built without Battleship.

## Data structures

The game uses `MyArrayList<Ship>` for each fleet and `CircularArrayQueue<Coordinate>` for CPU follow-up targets. The fixed-size board uses `std::array`.

The reusable library also includes linked and ordered lists, a priority queue, stacks, queues, a trie, binary search trees, AVL and red-black trees, hash tables, and radix sort. See the README inside DataStructures for details.

To build and test only the library:

```sh
cmake -S DataStructures -B DataStructures/build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build DataStructures/build
ctest --test-dir DataStructures/build --output-on-failure
```

## Tests

CTest runs two test executables using an included C++ test harness:

- **BattleshipTests:** coordinates, placement, attacks, console input, hidden CPU ships, paired boards, CPU turn flow, scoring, and sinking announcements.
- **DataStructuresTests:** list, queue, stack, tree, hash table, trie, priority queue, and sorting behavior.

At the latest code verification, all **23 test cases** passed across both suites. JUnit is not used because this project is C++.

## Learning focus

This project practices C++ classes, templates, ownership, reusable data structures, code organization, input validation, and automated testing. Class documentation uses Doxygen-style comments.

Generated build folders and local IDE settings are excluded by `.gitignore`. The shared `.run` configuration can be committed with the source code.

