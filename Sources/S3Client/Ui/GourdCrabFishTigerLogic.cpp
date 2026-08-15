#include "KWin32.h"
#include "GourdCrabFishTigerLogic.h"
#include <iostream>
#include <algorithm> // Required for std::count

// Function to simulate rolling three dice
std::vector<DiceFace> RollDice() {
    std::vector<DiceFace> diceRoll;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 5); // 0 to 5 for the 6 faces

    for (int i = 0; i < 3; ++i) {
        diceRoll.push_back(static_cast<DiceFace>(distrib(gen)));
    }
    return diceRoll;
}

// Helper function to determine if a bet is winning and count matches
bool IsBetWinning(DiceFace betFace, const std::vector<DiceFace>& diceRoll, int& matchCount) {
    matchCount = 0;
    for (DiceFace face : diceRoll) {
        if (face == betFace) {
            matchCount++;
        }
    }
    return matchCount > 0;
}

// Function to calculate the total winnings based on the dice roll and player bets
int CalculateWinnings(const std::vector<DiceFace>& diceRoll, const std::vector<Bet>& playerBets) {
    int totalWinnings = 0;

    for (const auto& bet : playerBets) {
        int matchCount = 0;
        if (IsBetWinning(bet.face, diceRoll, matchCount)) {
            // If the bet face matches any of the dice, calculate winnings based on match count
            totalWinnings += bet.amount * matchCount;
        } else {
            // If the bet face does not match any of the dice, the bet amount is lost
            totalWinnings -= bet.amount;
        }
    }
    return totalWinnings;
}

#ifdef TEST_GOURD_CRAB_LOGIC
// Helper function to print dice roll (for testing)
void PrintDiceRoll(const std::vector<DiceFace>& roll) {
    for (DiceFace face : roll) {
        switch (face) {
            case DiceFace::GOURD: std::cout << "GOURD "; break;
            case DiceFace::CRAB:  std::cout << "CRAB ";  break;
            case DiceFace::FISH:  std::cout << "FISH ";  break;
            case DiceFace::TIGER: std::cout << "TIGER "; break;
            case DiceFace::ROOSTER: std::cout << "ROOSTER ";break;
            case DiceFace::COIN:  std::cout << "COIN ";  break;
        }
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "Running GourdCrabFishTigerLogic Tests..." << std::endl;

    // Test 1: RollDice functionality
    std::cout << "\nTest 1: RollDice" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::vector<DiceFace> roll = RollDice();
        std::cout << "Roll " << i + 1 << ": ";
        PrintDiceRoll(roll);
        if (roll.size() != 3) {
            std::cerr << "RollDice test failed: Incorrect number of dice." << std::endl;
            return 1;
        }
    }
    std::cout << "RollDice appears to be working." << std::endl;

    // Test 2: CalculateWinnings - Simple win
    std::cout << "\nTest 2: CalculateWinnings - Simple Win" << std::endl;
    std::vector<DiceFace> roll2 = {DiceFace::GOURD, DiceFace::CRAB, DiceFace::FISH};
    std::vector<Bet> bets2 = {{DiceFace::GOURD, 10}};
    int winnings2 = CalculateWinnings(roll2, bets2);
    std::cout << "Roll: "; PrintDiceRoll(roll2);
    std::cout << "Bets: GOURD (10)" << std::endl;
    std::cout << "Expected Winnings: 10, Actual Winnings: " << winnings2 << std::endl;
    if (winnings2 != 10) {
        std::cerr << "CalculateWinnings Test 2 failed." << std::endl;
        return 1;
    }

    // Test 3: CalculateWinnings - Double win
    std::cout << "\nTest 3: CalculateWinnings - Double Win" << std::endl;
    std::vector<DiceFace> roll3 = {DiceFace::TIGER, DiceFace::TIGER, DiceFace::FISH};
    std::vector<Bet> bets3 = {{DiceFace::TIGER, 20}};
    int winnings3 = CalculateWinnings(roll3, bets3);
    std::cout << "Roll: "; PrintDiceRoll(roll3);
    std::cout << "Bets: TIGER (20)" << std::endl;
    std::cout << "Expected Winnings: 40, Actual Winnings: " << winnings3 << std::endl;
    if (winnings3 != 40) {
        std::cerr << "CalculateWinnings Test 3 failed." << std::endl;
        return 1;
    }

    // Test 4: CalculateWinnings - Triple win
    std::cout << "\nTest 4: CalculateWinnings - Triple Win" << std::endl;
    std::vector<DiceFace> roll4 = {DiceFace::ROOSTER, DiceFace::ROOSTER, DiceFace::ROOSTER};
    std::vector<Bet> bets4 = {{DiceFace::ROOSTER, 5}};
    int winnings4 = CalculateWinnings(roll4, bets4);
    std::cout << "Roll: "; PrintDiceRoll(roll4);
    std::cout << "Bets: ROOSTER (5)" << std::endl;
    std::cout << "Expected Winnings: 15, Actual Winnings: " << winnings4 << std::endl;
    if (winnings4 != 15) {
        std::cerr << "CalculateWinnings Test 4 failed." << std::endl;
        return 1;
    }

    // Test 5: CalculateWinnings - Loss
    std::cout << "\nTest 5: CalculateWinnings - Loss" << std::endl;
    std::vector<DiceFace> roll5 = {DiceFace::GOURD, DiceFace::CRAB, DiceFace::FISH};
    std::vector<Bet> bets5 = {{DiceFace::TIGER, 10}};
    int winnings5 = CalculateWinnings(roll5, bets5);
    std::cout << "Roll: "; PrintDiceRoll(roll5);
    std::cout << "Bets: TIGER (10)" << std::endl;
    std::cout << "Expected Winnings: -10, Actual Winnings: " << winnings5 << std::endl;
    if (winnings5 != -10) {
        std::cerr << "CalculateWinnings Test 5 failed." << std::endl;
        return 1;
    }

    // Test 6: CalculateWinnings - Mixed bets (win and loss)
    std::cout << "\nTest 6: CalculateWinnings - Mixed Bets" << std::endl;
    std::vector<DiceFace> roll6 = {DiceFace::COIN, DiceFace::CRAB, DiceFace::COIN};
    std::vector<Bet> bets6 = {{DiceFace::COIN, 10}, {DiceFace::FISH, 5}};
    int winnings6 = CalculateWinnings(roll6, bets6); // COIN wins 2*10=20, FISH loses 5. Total = 15
    std::cout << "Roll: "; PrintDiceRoll(roll6);
    std::cout << "Bets: COIN (10), FISH (5)" << std::endl;
    std::cout << "Expected Winnings: 15, Actual Winnings: " << winnings6 << std::endl;
    if (winnings6 != 15) {
        std::cerr << "CalculateWinnings Test 6 failed." << std::endl;
        return 1;
    }

    // Test 7: CalculateWinnings - Multiple winning bets
    std::cout << "\nTest 7: CalculateWinnings - Multiple Winning Bets" << std::endl;
    std::vector<DiceFace> roll7 = {DiceFace::GOURD, DiceFace::CRAB, DiceFace::FISH};
    std::vector<Bet> bets7 = {{DiceFace::GOURD, 10}, {DiceFace::CRAB, 5}};
    int winnings7 = CalculateWinnings(roll7, bets7); // GOURD wins 10, CRAB wins 5. Total = 15
    std::cout << "Roll: "; PrintDiceRoll(roll7);
    std::cout << "Bets: GOURD (10), CRAB (5)" << std::endl;
    std::cout << "Expected Winnings: 15, Actual Winnings: " << winnings7 << std::endl;
    if (winnings7 != 15) {
        std::cerr << "CalculateWinnings Test 7 failed." << std::endl;
        return 1;
    }
    
    // Test 8: CalculateWinnings - No bets
    std::cout << "\nTest 8: CalculateWinnings - No Bets" << std::endl;
    std::vector<DiceFace> roll8 = {DiceFace::GOURD, DiceFace::CRAB, DiceFace::FISH};
    std::vector<Bet> bets8 = {};
    int winnings8 = CalculateWinnings(roll8, bets8);
    std::cout << "Roll: "; PrintDiceRoll(roll8);
    std::cout << "Bets: (none)" << std::endl;
    std::cout << "Expected Winnings: 0, Actual Winnings: " << winnings8 << std::endl;
    if (winnings8 != 0) {
        std::cerr << "CalculateWinnings Test 8 failed." << std::endl;
        return 1;
    }

    std::cout << "\nAll GourdCrabFishTigerLogic tests passed successfully!" << std::endl;
    return 0;
}
#endif // TEST_GOURD_CRAB_LOGIC
