#include "KCore.h"
#include "BauCua.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <random>
#include <sstream>
#include <nlohmann/json.hpp> // External JSON library: https://github.com/nlohmann/json
#include <base64.h>
#include <KPlayerSet.h>
#include <KPlayer.h>
#include <algorithm> 
#include <openssl/rand.h>
#include <random>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#ifdef _SERVER
using json = nlohmann::json;

#define BAUCUA_DURATION 60 // seconds
#define BAUCUA_HOST_THRESHOLD 100 // Minimum balance to become host
#define BAUCUA_HOST_WIN_SHARE 10 // Percentage of winnings for host

BauCua g_BauCua(BAUCUA_DURATION, BAUCUA_HOST_THRESHOLD, BAUCUA_HOST_WIN_SHARE);

std::string DiceFaceToString(DiceFace face) {
    switch (face) {
    case DiceFace::FISH: return "🐟 Fish";
    case DiceFace::CRAB: return "🦀 Crab";
    case DiceFace::DEER: return "🦌 Deer";
    case DiceFace::GOURD: return "🍐 Gourd";
    case DiceFace::ROOSTER: return "🐔 Rooster";
    case DiceFace::SHRIMP: return "🦐 Shrimp";
    }
    return "Unknown";
}

BauCua::BauCua(int roundIntervalSeconds, int hostThreshold, int hostWinShare)
    : roundInterval(roundIntervalSeconds), hostThreshold(hostThreshold), hostWinShare(hostWinShare) {
    loadDeposits();
	m_lastRoundTime = std::chrono::seconds(0);
    serverBalance = 10000000;
	currentRound = generateGameRound(currentHost);
}

void BauCua::deposit(const std::string& playerId, int amount) {
    if (isLocked())
        return; // Prevent action if game is locked
    balances[playerId] += amount;
    activePlayers.insert(playerId);
    logAction(playerId + " deposited " + std::to_string(amount));
    saveDeposits();
}

bool BauCua::canWithdraw(const std::string& playerId) const {
    return lockedInBet.count(playerId) == 0 || lockedInBet.at(playerId) == 0;
}

int BauCua::withdraw(const std::string& playerId) {
    if (isLocked())
        return false; // Prevent action if game is locked
    // Check if player is in the current round's bets
    for (const auto& bet : bets) {
        if (bet.playerId == playerId) {
            logAction(playerId + " attempted withdrawal during active bet. Denied.");
            return false;
        }
    }
    if (!canWithdraw(playerId)) return false;
    int ret = balances[playerId];
    balances[playerId] = 0;
    logAction(playerId + " withdrew " + std::to_string(ret) + ".");
    saveDeposits();
    return ret;
}


bool BauCua::placeBet(const std::string& playerId, const std::map<DiceFace, int>& faceToAmount) {
    if (isLocked())
        return false; // Prevent action if game is locked
    if (balances[playerId] <= 0) return false;

    if (playerId == currentHost) return false;
    int totalBet = 0;
    for (const auto& p : faceToAmount) totalBet += p.second;
    if (totalBet > balances[playerId]) return false;

    balances[playerId] -= totalBet;
    lockedInBet[playerId] += totalBet;
    bets.push_back({ playerId, faceToAmount });

    std::ostringstream betDetails;
    for (const auto& p : faceToAmount) {
        betDetails << DiceFaceToString(p.first) << ": " << p.second << ", ";
    }
    logAction(playerId + " placed a bet of " + std::to_string(totalBet) + " [" + betDetails.str() + "]");
    
    saveDeposits();
    return true;
}

bool BauCua::cancelPlayerBets(const std::string& playerId) {
    if (isLocked())
        return false; // Prevent action if game is locked
    bool found = false;
    int refund = 0;

    auto it = bets.begin();
    while (it != bets.end()) {
        if (it->playerId == playerId) {
            // Sum up refund
            for (const auto& p : it->faceToAmount) {
                refund += p.second;
            }

            // Remove bet
            it = bets.erase(it);
            found = true;
        }
        else {
            ++it;
        }
    }

    if (found) {
        balances[playerId] += refund;
        lockedInBet[playerId] = 0;
        logAction(playerId + " canceled bets and was refunded " + std::to_string(refund));
        saveDeposits();
        return true;
    }

    logAction(playerId + " attempted to cancel bets, but no bets found.");
    return false;
}


bool BauCua::allowPlayerToHost(const std::string& playerId) {
    if (isLocked())
        return false;

    // khong cho lam host khi dang cuoc
    for (const auto& bet : bets) {
        if (bet.playerId == playerId) {
            logAction(playerId + " attempted to become host while having active bets. Denied.");
            return false;
        }
    }

    int bal = balances[playerId];

   
    if (currentHost.empty()) {
        if (bal >= hostThreshold) {
            currentHost = playerId;
            logAction(playerId + " is now the game host.");
            return true;
        }
        return false;
    }

 
    if (bal > balances[currentHost]) {
        currentHost = playerId;
        logAction(playerId + " overtakes and becomes the new host.");
        return true;
    }

    return false;
}


void BauCua::noHost(const std::string& playerId) {
    if(isLocked())
		return; // Prevent action if game is locked
    if (currentHost == playerId) {
        currentHost = "";
        logAction("Noone is now hosting the game, Server handles.");
    }
}

void BauCua::run() {
    auto currentTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
    if (currentTime - m_lastRoundTime > std::chrono::seconds(roundInterval)) {
        startRound();
        m_lastRoundTime = currentTime;
		currentRound = generateGameRound(currentHost); // Generate new round
    }
}

void BauCua::startRound()
{
    m_bLock = true;
    m_lastRoundTime = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch());

    
    int idx = PlayerSet.GetFirstPlayer();
    bool foundHost = false;
    while (idx > 0)
    {
        std::string playerId = Player[idx].GetPlayerName();
        if (playerId == currentHost)
        {
            foundHost = true;
            break;
        }
        idx = PlayerSet.GetNextPlayer();
    }
    if (!foundHost || balances[currentHost] < hostThreshold)
        currentHost = "";

    PlayerSet.GetFirstPlayer();

    logAction("=== Round " + std::to_string(currentRoundId) + "|" +
        currentRound.commitmentHash + "|" + currentRound.serverSeed + " started ===");

   
    std::vector<DiceFace> diceResults =
        getDiceResults(currentRound.commitmentHash);

    logResult(diceResults);
    resolveBets(diceResults);
    lastDiceResult = diceResults;

    bets.clear();
    lockedInBet.clear();
    saveDeposits();

    currentRoundId++;

    if (balances[currentHost] < hostThreshold)
        currentHost = "";

    m_bLock = false;

    
    currentRound = generateGameRound(currentHost);
}


GameRound BauCua::generateGameRound(const std::string& hostPlayerId) {
    GameRound round;
    round.gameId = currentRoundId;
    round.serverSeed = generateRandomSeed();
    round.commitmentHash = sha256(round.serverSeed);
   // round.diceResults = getDiceResults(sha256(round.serverSeed + round.gameId));
    round.hostPlayerId = hostPlayerId;
    return round;
}


//std::string BauCua::generateRandomSeed() {
//    static std::mt19937_64 rng(std::random_device{}());
//    std::stringstream ss;
//    for (int i = 0; i < 4; ++i)
//        ss << std::hex << rng();
//    return ss.str();
//}

std::string BauCua::generateRandomSeed() {
    unsigned char seed[32]; // 256 bits
    if (RAND_bytes(seed, sizeof(seed)) != 1)
        throw std::runtime_error("RAND_bytes failed");
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < sizeof(seed); ++i)
        ss << std::setw(2) << static_cast<int>(seed[i]);
    return ss.str(); // hex-encoded 64 chars
}

std::string BauCua::sha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)input.c_str(), input.length(), hash);
    std::ostringstream result;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        result << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return result.str();
}
std::vector<DiceFace> BauCua::getDiceResults(const std::string& clientHash)
{
    std::vector<DiceFace> result;

    long long totalBet[6] = {0};
    for (const auto& bet : bets)
        for (const auto& p : bet.faceToAmount)
            totalBet[(int)p.first] += p.second;

    double baseWeight[6];
    double sum = 0.0;

    const double BOOST_NO_BET = 3.0;     
    const long long BIG_BET_LIMIT = 800; 
    for (int i = 0; i < 6; ++i)
    {
        long long bet = totalBet[i];

        if (bet == 0)
        {
            baseWeight[i] = BOOST_NO_BET;
        }
        else if (bet <= 100)
        {
           
            baseWeight[i] = BOOST_NO_BET * 0.50;
        }
        else if (bet <= 400)
        {
           
            double factor = (double)bet / 100.0;  
            baseWeight[i] = (BOOST_NO_BET * 0.20) / factor;
        }
        else
        {
            
            double factor = (double)bet / 200.0;  
            baseWeight[i] = (BOOST_NO_BET * 0.05) / factor;
        }

        sum += baseWeight[i];
    }

    
    if (sum <= 0.0) sum = 1.0;
    for (int i = 0; i < 6; ++i)
        baseWeight[i] /= sum;

    auto pickFace = [&](int rollIndex) -> DiceFace {

        double w[6];
        for (int i = 0; i < 6; ++i)
            w[i] = baseWeight[i];

        
        for (int i = 0; i < 6; ++i)
        {
            if (totalBet[i] >= BIG_BET_LIMIT &&
                rollIndex > 0 &&
                std::count(result.begin(), result.end(), (DiceFace)i))
            {
                w[i] = 0.0;
            }
        }

        
        double s = 0.0;
        for (int i = 0; i < 6; i++)
            s += w[i];

        if (s <= 0.0)
        {
            for (int i = 0; i < 6; ++i)
                w[i] = baseWeight[i];
            s = 0.0;
            for (int i = 0; i < 6; ++i) s += w[i];
        }

        for (int i = 0; i < 6; ++i)
            w[i] /= s;

        // RNG
        unsigned char buf[8];
        RAND_bytes(buf, sizeof(buf));
        uint64_t v; memcpy(&v, buf, 8);
        double r = (v % 1000000) / 1000000.0;

        double acc = 0;
        for (int i = 0; i < 6; ++i)
        {
            acc += w[i];
            if (r <= acc)
                return (DiceFace)i;
        }

        return DiceFace::SHRIMP;
    };

    result.push_back(pickFace(0));
    result.push_back(pickFace(1));
    result.push_back(pickFace(2));

    return result;
}





std::vector<DiceFace> BauCua::getLastGameResult() const {
    return lastDiceResult;
}
std::vector<DiceFace> BauCua::rollDice() 
{
    std::vector<DiceFace> result;
    unsigned char buf[3];

    RAND_bytes(buf, 3);

    for (int i = 0; i < 3; ++i)
    {
        int v = buf[i] % 6;
        result.push_back(static_cast<DiceFace>(v));
    }

    return result;
}

BauCuaStatus BauCua::getBauCuaStatusForPlayer(const std::string& playerId) {
    BauCuaStatus status;

    status.playerDeposit = balances.count(playerId) ? balances[playerId] : 0;
    status.hostId = currentHost;
    status.hostDeposit = currentHost.empty() ? 0 : balances[currentHost];
    status.lastRoundBets = lastRoundBets;

    // Sum up current round's total bets per face
    status.currentBets.clear();
    for (const auto& bet : bets) {
        for (const auto& p : bet.faceToAmount) {
            status.currentBets[p.first] += p.second;
			if (bet.playerId == playerId) {
                status.playerCurrentBet[p.first] += p.second; // Sum player's bets
			}
        }
    }

    status.roundId = currentRoundId;

	auto now = std::chrono::system_clock::now().time_since_epoch();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_lastRoundTime).count();
    status.remainingSeconds = max(0, roundInterval - static_cast<int>(elapsed));
    status.lastDiceResult = lastDiceResult;
    status.commitmentHash = currentRound.commitmentHash;

    return status;
}
template <typename T>
constexpr T clamp(const T& value, const T& min_value, const T& max_value) {
    return max(min_value, min(value, max_value));
}
void BauCua::resolveBets(const std::vector<DiceFace>& diceResults) {
    std::unordered_map<std::string, long long> winAmount;    // extra payouts (amount * count) per player
    std::unordered_map<std::string, long long> returnedBet; // original stake returned per player
    std::map<DiceFace, int> faceCounts;
    lastRoundBets.clear();

    // Count faces
    for (DiceFace f : diceResults) faceCounts[f]++;

    // Compute totalBet and populate lastRoundBets
    long long totalBet = 0;
    for (const auto& bet : bets) {
        for (const auto& p : bet.faceToAmount) {
            lastRoundBets[p.first] += p.second;
            totalBet += (long long)p.second;
        }
    }

    // Compute per-player returned bets and win amounts, and log losses/wins
    const long long nNotifyThreshold = 10;
    for (const auto& bet : bets) {
        long long playerWin = 0;
        long long playerReturn = 0;
        int winFace = -1;
        for (const auto& p : bet.faceToAmount) {
            DiceFace face = p.first;
            int amount = p.second;
            int count = faceCounts[face]; // 0..3
            if (count > 0) {
                playerWin += (long long)amount * count; // extra payout for each appearance
                playerReturn += (long long)amount;      // original stake returned for that face
                winFace = (int)face; // Track winning face
            }
            else {
                if (amount > 0) {
                    logAction(bet.playerId + " lost " + std::to_string(amount) + " for face " + DiceFaceToString(face));
                }
            }
        }
        if (playerWin > 0 || playerReturn > 0) {
            winAmount[bet.playerId] += playerWin;
            returnedBet[bet.playerId] += playerReturn;
            logAction(bet.playerId + " won " + std::to_string(playerWin) + " and returned bet " + std::to_string(playerReturn));
            if (playerWin > nNotifyThreshold) {
                char scriptName[255];
                char tmp[64];
                sprintf(scriptName, "\\script\\baucua\\baucua.lua");
                sprintf(tmp, "%s|%d|%lld", bet.playerId.c_str(), winFace, playerWin);
                Player[0].ExecuteScript(scriptName, "win", tmp, false);
            }
        }
        lockedInBet[bet.playerId] = 0;
    }

    // Totals
    long long totalWinAmount = 0, totalReturnedBets = 0;
    for (const auto& kv : winAmount) totalWinAmount += kv.second;
    for (const auto& kv : returnedBet) totalReturnedBets += kv.second;
    long long totalPaidToPlayers = totalWinAmount + totalReturnedBets;

    // --- KEY FIX: net profit/loss (house perspective) ---
    long long profit = totalBet - totalPaidToPlayers; // >0 house profit, <0 house loss

    // Step 1: Credit players (their returned original stakes + extra winnings)
    for (const auto& kv : winAmount) {
        balances[kv.first] += (int)kv.second;
    }
    for (const auto& kv : returnedBet) {
        balances[kv.first] += (int)kv.second;
    }

    // Step 2: Apply house-side accounting based on net result
    long long hostBalanceBefore = currentHost.empty() ? 0 : balances[currentHost];
    hostBalanceBefore = max(0LL, hostBalanceBefore); // safety
    double hostRatio = totalBet > 0 ? (double)hostBalanceBefore / (double)totalBet : 0.0;
    hostRatio = clamp(hostRatio, 0.0, 1.0);

    if (profit > 0) {
        // House profit - split profit between host and server
        long long hostPortion = (long long)std::llround(profit * hostRatio);
        // hostWinShare is percentage (0..100) - host gets (100 - hostWinShare)% of hostPortion, if that's the design
        long long hostShare = 0;
        if (!currentHost.empty()) {
            hostShare = (long long)std::llround(hostPortion * (100.0 - (double)hostWinShare) / 100.0);
            balances[currentHost] += (int)hostShare;
        }
        long long serverShare = profit - hostShare;
        serverBalance += serverShare;

        logAction("Round ended with profit. Total bet: " + std::to_string(totalBet) +
            ", payout: " + std::to_string(totalWinAmount) +
            ", original bet back: " + std::to_string(totalReturnedBets) +
            ", host earned: " + std::to_string(hostShare) +
            ", server earned: " + std::to_string(serverShare));
    }
    else if (profit == 0) {
        // Draw: no net profit/loss — but you may still pay host some handling fee if rules require.
        // If you intended host to receive something in draw case, compute & apply here.
        long long hostGet = 0;
        logAction("Round ended draw. Total bet: " + std::to_string(totalBet) +
            ", payout: " + std::to_string(totalWinAmount) +
            ", host get: " + std::to_string(hostGet) +
            ", original bet back: " + std::to_string(totalReturnedBets) +
            ", no earn, server paid: 0");
    }
    else {
        // House loss: players were paid more than collected; host and server must cover the difference.
        long long netLoss = -profit; // positive
        long long hostCover = 0;
        if (!currentHost.empty()) {
            hostCover = min(hostBalanceBefore, netLoss);
            balances[currentHost] -= (int)hostCover;
            if (balances[currentHost] < 0) balances[currentHost] = 0;
        }
        long long serverCover = netLoss - hostCover;
        serverBalance -= serverCover;

        logAction("Round ended loss. Total bet: " + std::to_string(totalBet) +
            ", payout: " + std::to_string(totalWinAmount) +
            ", original bet back: " + std::to_string(totalReturnedBets) +
            ", host covered: " + std::to_string(hostCover) +
            ", server covered: " + std::to_string(serverCover));
    }

    // cleanup
    bets.clear();
    lockedInBet.clear();
    saveDeposits();
}

void BauCua::logResult(const std::vector<DiceFace>& diceResults) {
    std::ofstream log("baucua/game_log.txt", std::ios::app);
    log << "Round Result: ";
    for (const auto& face : diceResults)
        log << DiceFaceToString(face) << " ";
    log << "\n";
}

void BauCua::logAction(const std::string& action) {
    std::ofstream log("baucua/game_log.txt", std::ios::app);
    time_t now = std::time(nullptr);
    log << "[" << std::ctime(&now) << "] " << action << "\n";
}

void BauCua::saveDeposits() {
    json j;
    for (const auto& p : balances)
        j["balances"][base64_encode(p.first)] = p.second;

    for (const auto& p : lockedInBet) {
        j["locked"][base64_encode(p.first)] = p.second;
    }

    std::ofstream out("baucua/deposits.json");
    out << j.dump(4);
}

void BauCua::loadDeposits() {  
   std::ifstream in("baucua/deposits.json");  
   if (!in.is_open()) return;  

   json j;  
   in >> j;  

   balances.clear();  
   lockedInBet.clear();  

   if (j.contains("balances")) {  
       for (auto& item : j["balances"].items()) {  
           const std::string& player = base64_decode(item.key());
           int balance = item.value().get<int>();  
           balances[player] = balance;  
       }  
   }  

   if (j.contains("locked")) {  
       for (auto& item : j["locked"].items()) {  
           const std::string& player = base64_decode(item.key());
           int locked = item.value().get<int>();  
           balances[player] += locked;
       }  
   }  
}
#endif // _SERVER