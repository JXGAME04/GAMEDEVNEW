#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <chrono>
#include <algorithm>
#include <random>
#include <openssl/sha.h>

std::string DiceFaceToString(DiceFace face);

struct Bet {
    std::string playerId;
    std::map<DiceFace, int> faceToAmount;
};

struct GameRound {
    std::string gameId;
    std::string serverSeed;
    std::string commitmentHash;
    std::vector<DiceFace> diceResults; // 0-5 for 6 symbols
    std::string hostPlayerId;
};

class BauCua {
public:
    BauCua(int roundIntervalSeconds, int hostThreshold, int hostWinShare);
    void run();
    void deposit(const std::string& playerId, int amount);
    bool canWithdraw(const std::string& playerId) const;
    int withdraw(const std::string& playerId);
    bool placeBet(const std::string& playerId, const std::map<DiceFace, int>& faceToAmount);
    bool cancelPlayerBets(const std::string& playerId);
    bool allowPlayerToHost(const std::string& playerId);
    void noHost(const std::string& playerId);
    void saveDeposits();
    void loadDeposits();
    std::vector<DiceFace> getLastGameResult() const;
    BauCuaStatus BauCua::getBauCuaStatusForPlayer(const std::string& playerId);

private:
	GameRound currentRound;
    int currentRoundId = 0;
    std::map<DiceFace, int> lastRoundBets;
    int serverBalance;
    void startRound();
    GameRound generateGameRound(const std::string& hostPlayerId);
    std::string generateRandomSeed();
    std::string sha256(const std::string& input);
    std::vector<DiceFace> getDiceResults(const std::string& hashStr);
    std::vector<DiceFace> rollDice();
    void resolveBets(const std::vector<DiceFace>& diceResults);
    void logResult(const std::vector<DiceFace>& diceResults);
    void logAction(const std::string& action);
	bool isLocked() const {
		return m_bLock;
	}

    std::vector<DiceFace> lastDiceResult;
    std::chrono::seconds m_lastRoundTime;
    int roundInterval;
    int hostThreshold;
    std::string currentHost;
    std::vector<Bet> bets;
    std::unordered_map<std::string, int> balances;
    std::unordered_set<std::string> activePlayers;
    std::map<std::string, int> lockedInBet;
	bool m_bLock = false; // Lock to prevent concurrent access
    int hostWinShare = 10;
};

extern CORE_API BauCua g_BauCua;