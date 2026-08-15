#pragma once
#include "../Elem/WndToolBar.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndLabeledButton.h"
#include <GameDataDef.h>
#include "../Elem/WndLine.h"
#include <vector> // Added for std::vector
#include "../GourdCrabFishTigerLogic.h" // Added for game logic
#include "../Elem/WndEdit.h"

#define SHA256_DIGEST_LENGTH 32
// Forward declare enums and structs from GourdCrabFishTigerLogic.h
// to avoid including the full header here, reducing compilation dependencies.
enum class DiceFace; // From GourdCrabFishTigerLogic.h
struct Bet;         // From GourdCrabFishTigerLogic.h

class KUiSpringGame : public KWndToolBar
{
public:
	static KUiSpringGame* OpenWindow();
	static void				CloseWindow();
	static KUiSpringGame* GetIfVisible();
	static void				LoadScheme(const char* pScheme);
	static void				DefaultScheme(const char* pScheme);
	static KUiSpringGame* GetSelf() { return m_pSelf; }
	void UpdateInfo(BauCuaStatusSend Info);
private:
	~KUiSpringGame() {}
	void	Initialize();
	void	Breathe();
	void	LoadScheme(KIniFile* pIni);
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void UpdateDiceImages(const std::vector<DiceFace>& roll); // Helper function declaration

	void OnReceiveDiceRollFromServer(const std::vector<DiceFace>& serverRoll, int netWinnings);

	void OnMakeHostResponseFromServer(bool success);

	int btnNo = 1;
	int m_nLastUpdateCounting = 0; // Last update counting for UI
private:
	int remainSeconds  = 60;

	static KUiSpringGame* m_pSelf;

	KWndButton	m_Close;
	KWndLabeledButton	m_MakeHostBtn; // Repurposed as RollDiceBtn
	KWndLabeledButton	m_HostGetCoinBtn;
	KWndLabeledButton	m_GetLastCoinBtn;
	KWndText80 m_HostNameTxt;
	KWndText80 m_HostName;
	KWndText80 m_HostRestTimeTxt;
	KWndText80 m_HostRestTime;
	KWndText80 m_HostCoinTxt;
	KWndText80 m_HostCoin;
	KWndText80 m_MinBetCoinTxt;
	KWndText80 m_MinBetCoin;
	KWndText80 m_MaxBetCoinTxt;
	KWndText80 m_MaxBetCoin;
	KWndText80 m_CoinTxt;
	KWndText80 m_Coin; // This will display player's coins
	KWndImage m_Dice;
	KWndImage m_Dice_0;
	KWndImage m_Dice_1;
	KWndImage m_Dice_2;
	KWndLabeledButton m_BetBtn_0;
	KWndLabeledButton m_BetBtn_1;
	KWndLabeledButton m_BetBtn_2;
	KWndLabeledButton m_BetBtn_3;
	KWndLabeledButton m_BetBtn_4;
	KWndLabeledButton m_BetBtn_5;
	KWndImage	m_Background;
	KWndText256 m_InfoText;
	KWndText256 m_ResultText;
	KWndText80 m_SpringNo;
	KWndText80 m_SpringNoTxt;
	KWndText80 m_RemainTime;
	KWndText80 m_RemainTxt;
	KWndText80 m_CoinShadow[6];
	KWndText80 m_CoinInput[6];
	KWndText80 m_LastTotalCoinTxt[6];
	KWndText80 m_LastTotalCoin[6];
	KWndText80 m_FeedRate[6];
	KWndText80 m_BetTotalCoinTxt[6];
	KWndText80 m_BetTotalCoin[6];
	KWndText80 m_BetCoin[6];
	KWndEdit32 m_DepositAmount;
	KWndLabeledButton	m_DepositBtn;
	KWndLabeledButton	m_WithdrawBtn;
	KWndEdit32 m_BetAmountInput;
	KWndText80 m_BetAmountTxt;
	KWndLabeledButton m_CancelBetBtn;
	int m_HostCoinAmount; // Host's coin amount
	int lastTick;
	// Gourd Crab Fish Tiger game logic members
	int m_playerCoins;
	std::vector<Bet> m_currentBets;
	std::vector<Bet> m_lastRoundBets;
	std::vector<Bet> m_playerCurrentBets;
	std::vector<DiceFace> m_lastDiceRoll;
	int m_nRoundId = 0; // Current round ID
	int m_nOldRoundCoin = 0; // Previous round ID
	char m_currentHost[32]; // Current host player ID
	char m_commitmentHash[SHA256_DIGEST_LENGTH];
	int m_hostDeposit = 0;
	char m_currentPlayerName[32]; // Current player's name
	// KWndLabeledButton m_RollDiceBtn; // Assuming m_MakeHostBtn can be repurposed or a new button added later
};