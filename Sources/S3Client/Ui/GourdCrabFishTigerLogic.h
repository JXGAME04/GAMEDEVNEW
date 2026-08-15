#ifndef GOURD_CRAB_FISH_TIGER_LOGIC_H
#define GOURD_CRAB_FISH_TIGER_LOGIC_H

#include <vector>
#include <random> // Required for RollDice
#include <GameDataDef.h>

// Define the structure for a player's bet
struct Bet {
    DiceFace face; // The face the player is betting on
    int amount;    // The amount of money bet
};

// Function to simulate rolling three dice
// Returns a vector containing three randomly generated DiceFace values
std::vector<DiceFace> RollDice();

// Function to calculate the total winnings based on the dice roll and player bets
// diceRoll: A vector representing the outcome of the three dice
// playerBets: A vector of bets made by the player
// Returns the net change in the player's money (winnings - losses)
int CalculateWinnings(const std::vector<DiceFace>& diceRoll, const std::vector<Bet>& playerBets);

// Helper function to determine if a bet is winning and count matches
// betFace: The DiceFace the player bet on
// diceRoll: The outcome of the three dice
// matchCount: Output parameter to store the number of dice matching betFace
// Returns true if betFace is present in diceRoll, false otherwise
bool IsBetWinning(DiceFace betFace, const std::vector<DiceFace>& diceRoll, int& matchCount);

#endif // GOURD_CRAB_FISH_TIGER_LOGIC_H
