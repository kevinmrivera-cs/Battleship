#pragma once
namespace kmr::battleship {
/** @brief Overall lifecycle state for the game controller. */
enum class GameState { Setup, InProgress, HumanWon, ComputerWon, Quit };
}  // namespace kmr::battleship
