#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "SpringGame.h"
#include <vector>                     // Already in SpringGame.h, but good practice for .cpp

#include "../ShortcutKey.h"
#include "../../../core/src/coreshell.h"
#include <KTabFile.h>


#include <random> // Add at the top if not already included
extern iCoreShell* g_pCoreShell;

#define	SCHEME_INI				"springgame.ini"
// Define a mapping for DiceFace to UI elements if needed for clarity or flexibility
// For now, we'll map them directly in WndProc and UpdateDiceImages.

KUiSpringGame* KUiSpringGame::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´̣¿ª´°¿Ú£¬·µ»ØÎ¨̉»µÄ̉»¸öÀà¶ÔÏóÊµÀư
//--------------------------------------------------------------------------
KUiSpringGame* KUiSpringGame::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiSpringGame;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf) {
		m_pSelf->Show();
		BauCuaData	Data;

		Data.nActionType = BAUCUA_GET_INFO;
		g_pCoreShell->OperationRequest(GOI_BAUCUA, (unsigned int)&Data, 0);
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±Ơ´°¿Ú
//--------------------------------------------------------------------------
void KUiSpringGame::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

KUiSpringGame* KUiSpringGame::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//³ơÊ¼»¯
void KUiSpringGame::Initialize()
{
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_NORMAL);
	AddChild(&m_Background);
	AddChild(&m_Close);
	AddChild(&m_MakeHostBtn); // Repurposed as RollDiceBtn
	AddChild(&m_HostGetCoinBtn);
	AddChild(&m_GetLastCoinBtn);
	AddChild(&m_HostNameTxt);
	AddChild(&m_HostName);
	//AddChild(&m_HostRestTimeTxt);
	//AddChild(&m_HostRestTime);
	AddChild(&m_HostCoinTxt);
	AddChild(&m_HostCoin);
	//AddChild(&m_MinBetCoinTxt);
	//AddChild(&m_MinBetCoin);
	//AddChild(&m_MaxBetCoinTxt);
	//AddChild(&m_MaxBetCoin);
	AddChild(&m_CoinTxt);
	AddChild(&m_Coin);
	AddChild(&m_Dice);
	AddChild(&m_Dice_0);
	AddChild(&m_Dice_1);
	AddChild(&m_Dice_2);
	AddChild(&m_BetBtn_0);
	AddChild(&m_BetBtn_1);
	AddChild(&m_BetBtn_2);
	AddChild(&m_BetBtn_3);
	AddChild(&m_BetBtn_4);
	AddChild(&m_BetBtn_5);
	AddChild(&m_InfoText);
	AddChild(&m_ResultText);
	AddChild(&m_SpringNo);
	AddChild(&m_SpringNoTxt);
	AddChild(&m_RemainTime);
	AddChild(&m_RemainTxt);
	AddChild(&m_DepositAmount);
	AddChild(&m_DepositBtn);
	AddChild(&m_WithdrawBtn);
	AddChild(&m_BetAmountTxt);
	AddChild(&m_BetAmountInput);
	AddChild(&m_CancelBetBtn);
	// AddChild(&m_RollDiceBtn); // If a new button was used
	int i = 0;
	for (i = 0; i < 6; i++) {
		AddChild(&m_CoinShadow[i]);
		AddChild(&m_CoinInput[i]);
		AddChild(&m_LastTotalCoinTxt[i]);
		AddChild(&m_LastTotalCoin[i]);
		//AddChild(&m_FeedRate[i]);
		AddChild(&m_BetTotalCoinTxt[i]);
		AddChild(&m_BetTotalCoin[i]);
		AddChild(&m_BetCoin[i]);
	}
	// Initialize game variables
	m_playerCoins = 0;
	char coinStr[16];
	sprintf(coinStr, "%d", m_playerCoins);
	m_Coin.SetText(coinStr);

	m_lastDiceRoll.resize(3);
	// Set initial dice faces (e.g., all GOURD)
	// This assumes GOURD = 0 in DiceFace enum and corresponds to frame 0 in sprite
	m_lastDiceRoll[0] = DiceFace::GOURD;
	m_lastDiceRoll[1] = DiceFace::CRAB; // Example: GOURD, CRAB, FISH
	m_lastDiceRoll[2] = DiceFace::FISH;
	UpdateDiceImages(m_lastDiceRoll); // Update images to initial state


	KUiPlayerBaseInfo	Info;
	memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
	g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int)&Info, 0);
	strncpy(m_currentPlayerName, Info.Name, sizeof(m_currentPlayerName));
}
extern int SCREEN_WIDTH;
//ÔØÈë½çĂæ·½°¸
void KUiSpringGame::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			
			if (SCREEN_WIDTH == 1024)
			{
				m_pSelf->Init(&Ini, "Main1024");
			}
			else
			{
				m_pSelf->Init(&Ini, "Main");
			}
			m_pSelf->LoadScheme(&Ini); // Calls the other LoadScheme
			char		imgBuff[128]; // Renamed to avoid conflict with Buff
			Ini.GetString("GameBkg", "GameBkg_0", "", imgBuff, sizeof(imgBuff));
			if (imgBuff[0] != 0)
			{
				m_pSelf->m_Background.SetImage(ISI_T_SPR, imgBuff);
				//m_Background.SetFrame(0);
			}
			Ini.GetString("Dice", "Dice_0", "", imgBuff, sizeof(imgBuff));
			if (imgBuff[0] != 0)
			{
				// Initial dice images are set in Initialize() via UpdateDiceImages
				// These lines might conflict or be redundant if sprite sheets are complex
				// For now, we rely on UpdateDiceImages and the enum mapping.
				 m_pSelf->m_Dice_0.SetImage(ISI_T_SPR, imgBuff, true);
				 m_pSelf->m_Dice_0.SetFrame(static_cast<int>(DiceFace::GOURD)); // Example
				 m_pSelf->m_Dice_1.SetImage(ISI_T_SPR, imgBuff, true);
				 m_pSelf->m_Dice_1.SetFrame(static_cast<int>(DiceFace::CRAB));  // Example
				 m_pSelf->m_Dice_2.SetImage(ISI_T_SPR, imgBuff, true);
				 m_pSelf->m_Dice_2.SetFrame(static_cast<int>(DiceFace::FISH)); // Example
			}
			// m_pSelf->m_Coin.SetText("10000"); // Set in Initialize
			m_pSelf->m_HostCoin.SetText("0");
			//m_pSelf->m_HostRestTime.SetText("100");
			m_pSelf->m_HostName.SetText("M¸y Chñ");
			//m_pSelf->m_MinBetCoin.SetText("10V");
			//m_pSelf->m_MaxBetCoin.SetText("1000V");
			//m_pSelf->m_InfoText.SetText("Moi ban dat cuoc!");
			//m_pSelf->m_InfoText.SetTextColor(0xffef00);
			m_pSelf->m_SpringNo.SetText("1");
			m_pSelf->m_RemainTime.SetText("60");
		}
	}
}
void KUiSpringGame::LoadScheme(class KIniFile* pIni)
{
	char	TabChat[32];
	m_Background.Init(pIni, "GameBkg");
	m_Close.Init(pIni, "btnClose");
	//m_GetLastCoinBtn.Init(pIni, "GetLastCoinBtn");
	m_MakeHostBtn.Init(pIni, "MakeHostBtn"); // Repurposed as RollDiceBtn
	// m_RollDiceBtn.Init(pIni, "RollDiceBtn"); // If a new button was used
	m_HostGetCoinBtn.Init(pIni, "HostGetCoinBtn");
	m_HostNameTxt.Init(pIni, "HostNameTxt");
	m_HostName.Init(pIni, "HostName");
	//m_HostRestTimeTxt.Init(pIni, "HostRestTimeTxt");
	//m_HostRestTime.Init(pIni, "HostRestTime");
	m_HostCoinTxt.Init(pIni, "HostCoinTxt");
	m_HostCoin.Init(pIni, "HostCoin");
	//Init(pIni, "MinBetCoinTxt");
	//m_MinBetCoin.Init(pIni, "MinBetCoin");
	//m_MaxBetCoinTxt.Init(pIni, "MaxBetCoinTxt");
	//m_MaxBetCoin.Init(pIni, "MaxBetCoin");
	m_CoinTxt.Init(pIni, "CoinTxt");
	m_Coin.Init(pIni, "Coin");
	m_Dice.Init(pIni, "Dice");
	m_Dice_0.Init(pIni, "Dice_0");
	m_Dice_1.Init(pIni, "Dice_1");
	m_Dice_2.Init(pIni, "Dice_2");
	m_BetBtn_0.Init(pIni, "BetBtn_0");
	m_BetBtn_1.Init(pIni, "BetBtn_1");
	m_BetBtn_2.Init(pIni, "BetBtn_2");
	m_BetBtn_3.Init(pIni, "BetBtn_3");
	m_BetBtn_4.Init(pIni, "BetBtn_4");
	m_BetBtn_5.Init(pIni, "BetBtn_5");
	m_InfoText.Init(pIni, "InfoText");
	m_ResultText.Init(pIni, "ResultText");
	m_SpringNo.Init(pIni, "SpringNo");
	m_SpringNoTxt.Init(pIni, "SpringNoTxt");
	m_RemainTime.Init(pIni, "RemainTime");
	m_RemainTxt.Init(pIni, "RemainTxt");
	m_DepositAmount.Init(pIni, "DepositAmount");
	m_DepositBtn.Init(pIni, "DepositBtn");
	m_WithdrawBtn.Init(pIni, "WithdrawBtn");
	m_BetAmountTxt.Init(pIni, "BetAmountTxt");
	m_BetAmountInput.Init(pIni, "BetAmountInput");
	m_BetAmountInput.SetText("10");
	m_DepositAmount.SetText("10");
	m_CancelBetBtn.Init(pIni, "CancelBetBtn");
	int i = 0;
	char buff[64];
	memset(buff, 0, sizeof(buff));
	for (i = 0; i < 6; i++) {
		//sprintf(buff, "CoinShadow_%d", i);
		//m_CoinShadow[i].Init(pIni, buff);
		sprintf(buff, "CoinInput_%d", i);
		m_CoinInput[i].Init(pIni, buff);
		sprintf(buff, "LastTotalCoinTxt_%d", i);
		m_LastTotalCoinTxt[i].Init(pIni, buff);
		sprintf(buff, "LastTotalCoin_%d", i);
		m_LastTotalCoin[i].SetText("100");
		m_LastTotalCoin[i].Init(pIni, buff);
		sprintf(buff, "FeedRate_%d", i);
		//m_FeedRate[i].Init(pIni, buff);
		sprintf(buff, "BetTotalCoinTxt_%d", i);
		m_BetTotalCoinTxt[i].Init(pIni, buff);
		sprintf(buff, "BetTotalCoin_%d", i);
		m_BetTotalCoin[i].Init(pIni, buff);
		//m_BetTotalCoin[i].SetText("998");
		m_BetCoin[i].Init(pIni, buff);
	}

    for (i = 0; i < 6; i++) {
        //m_CoinShadow[i].SetText("CoinShadow");
        //m_FeedRate[i].SetText("1/6");

        char lastTotalStr[16];
        char betCoinStr[16];
        sprintf(lastTotalStr, "0");
        sprintf(betCoinStr, "0");

        m_LastTotalCoin[i].SetText(lastTotalStr);
        m_BetCoin[i].SetText(betCoinStr);
    }
	this->lastTick = 0;
}

//ÖØĐÂ³ơÊ¼»¯½çĂæ
void KUiSpringGame::DefaultScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			int	nValue1, nValue2;
			Ini.GetInteger("Main", "Left", 0, &nValue1);
			Ini.GetInteger("Main", "Top", 0, &nValue2);
			m_pSelf->SetPosition(nValue1, nValue2);
		}
	}
}
/*
struct BauCuaStatusSend {
	int playerDeposit;
	char hostId[32];
	int hostDeposit;
	int lastRoundBets[6];
	int currentBets[6];
	int lastDiceResult[3]; 
	int roundId;
	char commitmentHash[SHA256_DIGEST_LENGTH];
	int remainingSeconds;
};*/
void KUiSpringGame::UpdateInfo(BauCuaStatusSend Info) 
{

	if (Info.roundId != m_nRoundId && m_nRoundId != 0) {
		// Reset the last round bets and player current bets when a new round starts
		char buff[64];
		if (m_nOldRoundCoin < Info.playerDeposit) {
			sprintf(buff, "B¹n ®· win %d xu !", Info.playerDeposit - m_nOldRoundCoin);
			m_ResultText.SetText(buff);
			//m_ResultText.SetTextColor(0x7AFF00);
		}
		else if (m_nOldRoundCoin > Info.playerDeposit) {
			sprintf(buff, "B¹n ®a thua %d xu !", m_nOldRoundCoin - Info.playerDeposit);
			m_ResultText.SetText(buff);
			//m_ResultText.SetTextColor(0x7A7BFF);
		}
		else {
			sprintf(buff, "Xin mêi ®Æt c­îc!");
			m_ResultText.SetText(buff);
		}
	}

	m_nOldRoundCoin = Info.playerDeposit;
	m_playerCoins = Info.playerDeposit;
	//only update remaining seconds each round
	if (GetTickCount() - m_nLastUpdateCounting > 10*1000 || m_nLastUpdateCounting == 0) {
		remainSeconds = Info.remainingSeconds;
		m_nLastUpdateCounting = GetTickCount(); // Reset the countdown timer
	}
	//remainSeconds = Info.remainingSeconds;
	m_currentBets.clear();
	// Populate m_currentBets from currentBets
	for (int i = 0; i < static_cast<int>(DiceFace::COUNT); ++i) {
		Bet newBet;
		newBet.face = static_cast<DiceFace>(i);
		newBet.amount = Info.currentBets[i];
		m_currentBets.push_back(newBet);
	}
	m_lastRoundBets.clear();
	// Populate m_lastRoundBets from lastRoundBets
	for (int i = 0; i < static_cast<int>(DiceFace::COUNT); ++i) {
		Bet newBet;
		newBet.face = static_cast<DiceFace>(i);
		newBet.amount = Info.lastRoundBets[i];
		m_lastRoundBets.push_back(newBet);
	}
	// Populate m_playerCurrentBets from playerCurrentBet
	m_playerCurrentBets.clear();
	for (int i = 0; i < static_cast<int>(DiceFace::COUNT); ++i) {
		Bet newBet;
		newBet.face = static_cast<DiceFace>(i);
		newBet.amount = Info.playerCurrentBet[i];
		m_playerCurrentBets.push_back(newBet);

		if (Info.roundId == m_nRoundId) {
			m_nOldRoundCoin += newBet.amount;
		}
	}

	m_nRoundId = Info.roundId;
	memcpy(m_commitmentHash, Info.commitmentHash, SHA256_DIGEST_LENGTH);
	strncpy(m_currentHost, Info.hostId, sizeof(m_currentHost));
	if (strcmp(m_currentHost, "") == 0) {
		strncpy(m_currentHost, "M¸y Chñ", sizeof(m_currentHost));
	}
	m_lastDiceRoll.clear();
	for (size_t i = 0; i < 3; ++i) {
		// Cast the int back to DiceFace
		m_lastDiceRoll.push_back(static_cast<DiceFace>(Info.lastDiceResult[i]));
	}
	m_hostDeposit = Info.hostDeposit;
}
void KUiSpringGame::Breathe()
{
    int currentTick = GetTickCount();

	BauCuaData	Data;

	Data.nActionType = BAUCUA_GET_INFO;

	if (!strcmp(m_currentPlayerName, m_currentHost))//hien nhuong cai
	{
		m_MakeHostBtn.Hide();
		m_HostGetCoinBtn.Show();
	}
	else {
		m_HostGetCoinBtn.Hide();
		m_MakeHostBtn.Show();
	}
    // On first call or after reset, initialize
    if (this->lastTick == 0) {
		this->lastTick = currentTick;
		char remainSecondsStr[8];
		sprintf(remainSecondsStr, "%d", remainSeconds);
        m_RemainTime.SetText(remainSecondsStr);
        return;
    }

    // Check if a second has passed
    if (currentTick - this->lastTick >= 1000) {
		g_pCoreShell->OperationRequest(GOI_BAUCUA, (unsigned int)&Data, 0);
		this->lastTick += 1000;
        if (remainSeconds > 0) {
            remainSeconds--;
            char buf[8];
            sprintf(buf, "%d", remainSeconds);
            m_RemainTime.SetText(buf);
        }
    }

     //Optional: Reset when countdown reaches 0
     if (remainSeconds == 0) {
         remainSeconds = 60;
		 this->lastTick = currentTick;
         m_RemainTime.SetText("60");
		 int i;
		 for (i = 0; i < 6; i++) {
			 m_CoinInput[i].SetText("");
		 }
     }
	 //Update game info UI
	 int i = 0;
	 char buff[64];
	 memset(buff, 0, sizeof(buff));
	 char lastTotalStr[16];
	 char betCoinStr[16];
	 char coinStr[16];

	 if (m_lastRoundBets.size() == static_cast<int>(DiceFace::COUNT) && m_lastRoundBets.size() == static_cast<int>(DiceFace::COUNT)) {
		 for (int i = 0; i < static_cast<int>(DiceFace::COUNT); ++i) {
			 std::cout << "Face " << i << ":" << std::endl;
			 sprintf(lastTotalStr, "%d", m_lastRoundBets[i].amount);
			 sprintf(betCoinStr, "%d", m_currentBets[i].amount);
			 sprintf(coinStr, "%d", m_playerCurrentBets[i].amount);

			 m_LastTotalCoin[i].SetText(lastTotalStr);
			 m_BetCoin[i].SetText(betCoinStr);
			 m_CoinInput[i].SetText(coinStr);
		 }
	 }
	 sprintf(buff, "%d", m_playerCoins);
	m_Coin.SetText(buff);
	UpdateDiceImages(m_lastDiceRoll); // Update dice images based on last roll
	// Update host info
	m_HostName.SetText(m_currentHost);
	sprintf(buff, "%d", m_nRoundId);
	m_SpringNo.SetText(buff);
	sprintf(buff, "%d", m_hostDeposit);
	m_HostCoin.SetText(buff);
}

int KUiSpringGame::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	KIniFile	Ini;
	char		Buff[128];
	char		Buff1[128];
	char Scheme[256];
	char	Mer[32];
	char	Key[32];
	int left, top;
	int i = 0;
	int LR = 0; //0 L, 1 R
	switch (uMsg)
	{
	case WND_M_MENUITEM_SELECTED:
		break;
	case WND_N_CHILD_MOVE:
	case WND_N_LIST_ITEM_SEL:
		break;
	case WND_M_POPUPMENU:
		break;
	case WND_N_BUTTON_CLICK:
	{
		char coinStr[16]; // For updating coin display
		int betAmount = m_BetAmountInput.GetIntNumber(); // Fixed bet amount for now

		if (uParam == (unsigned int)(KWndWindow*)&m_Close) {
			KUiSpringGame::CloseWindow();
			break;
		}

		// Betting Logic
		if (uParam == (unsigned int)(KWndWindow*)&m_BetBtn_0) { // DEER
			DiceFace targetFace = DiceFace::DEER;
			if (m_playerCoins >= betAmount) {
				m_playerCoins -= betAmount;
				m_currentBets.push_back({targetFace, betAmount});
				sprintf(coinStr, "%d", m_playerCoins);
				m_Coin.SetText(coinStr);
                // Set CoinInput text to total bet of this bet face in m_currentBets
                int totalBet = 0;
                for (const auto& bet : m_currentBets) {
                    if (bet.face == targetFace) {
                        totalBet += bet.amount;
                    }
                }

                m_CoinInput[static_cast<int>(DiceFace::DEER)].SetText(std::to_string(totalBet).c_str());
				BauCuaData	Data;

				Data.nActionType = BAUCUA_BET;
				Data.nBetType = (int)DiceFace::DEER;
				Data.nMoney = betAmount;
				g_pCoreShell->OperationRequest(GOI_BAUCUA, (unsigned int)&Data, 0);
			}
		} else if (uParam == (unsigned int)(KWndWindow*)&m_BetBtn_1) { // GOURD
			DiceFace targetFace = DiceFace::GOURD;
			if (m_playerCoins >= betAmount) {
				m_playerCoins -= betAmount;
				m_currentBets.push_back({targetFace, betAmount});
				sprintf(coinStr, "%d", m_playerCoins);
				m_Coin.SetText(coinStr);
				// Set CoinInput text to total bet of this bet face in m_currentBets
				int totalBet = 0;
				for (const auto& bet : m_currentBets) {
					if (bet.face == targetFace) {
						totalBet += bet.amount;
					}
				}
				m_CoinInput[static_cast<int>(DiceFace::GOURD)].SetText(std::to_string(totalBet).c_str());
				BauCuaData	Data;

				Data.nActionType = BAUCUA_BET;
				Data.nBetType = (int)DiceFace::GOURD;
				Data.nMoney = betAmount;
				g_pCoreShell->OperationRequest(GOI_BAUCUA, (unsigned int)&Data, 0);
			}
		} else if (uParam == (unsigned int)(KWndWindow*)&m_BetBtn_2) { // ROOSTER
			DiceFace targetFace = DiceFace::ROOSTER;
			if (m_playerCoins >= betAmount) {
				m_playerCoins -= betAmount;
				m_currentBets.push_back({targetFace, betAmount});
				sprintf(coinStr, "%d", m_playerCoins);
				m_Coin.SetText(coinStr);
				// Set CoinInput text to total bet of this bet face in m_currentBets
				int totalBet = 0;
				for (const auto& bet : m_currentBets) {
					if (bet.face == targetFace) {
						totalBet += bet.amount;
					}
				}
				m_CoinInput[static_cast<int>(DiceFace::ROOSTER)].SetText(std::to_string(totalBet).c_str());
				BauCuaData	Data;

				Data.nActionType = BAUCUA_BET;
				Data.nBetType = (int)DiceFace::ROOSTER;
				Data.nMoney = betAmount;
				g_pCoreShell->OperationRequest(GOI_BAUCUA, (unsigned int)&Data, 0);
			}
		} else if (uParam == (unsigned int)(KWndWindow*)&m_BetBtn_3) { // FISH
			DiceFace targetFace = DiceFace::FISH;
			if (m_playerCoins >= betAmount) {
				m_playerCoins -= betAmount;
				m_currentBets.push_back({targetFace, betAmount});
				sprintf(coinStr, "%d", m_playerCoins);
				m_Coin.SetText(coinStr);
				// Set CoinInput text to total bet of this bet face in m_currentBets
				int totalBet = 0;
				for (const auto& bet : m_currentBets) {
					if (bet.face == targetFace) {
						totalBet += bet.amount;
					}
				}
				m_CoinInput[static_cast<int>(DiceFace::FISH)].SetText(std::to_string(totalBet).c_str());
				BauCuaData	Data;

				Data.nActionType = BAUCUA_BET;
				Data.nBetType = (int)DiceFace::FISH;
				Data.nMoney = betAmount;
				g_pCoreShell->OperationRequest(GOI_BAUCUA, (unsigned int)&Data, 0);
			}
		} else if (uParam == (unsigned int)(KWndWindow*)&m_BetBtn_4) { // CRAB
			DiceFace targetFace = DiceFace::CRAB;
			if (m_playerCoins >= betAmount) {
				m_playerCoins -= betAmount;
				m_currentBets.push_back({targetFace, betAmount});
				sprintf(coinStr, "%d", m_playerCoins);
				m_Coin.SetText(coinStr);
				// Set CoinInput text to total bet of this bet face in m_currentBets
				int totalBet = 0;
				for (const auto& bet : m_currentBets) {
					if (bet.face == targetFace) {
						totalBet += bet.amount;
					}
				}
				m_CoinInput[static_cast<int>(DiceFace::CRAB)].SetText(std::to_string(totalBet).c_str());
				BauCuaData	Data;

				Data.nActionType = BAUCUA_BET;
				Data.nBetType = (int)DiceFace::CRAB;
				Data.nMoney = betAmount;
				g_pCoreShell->OperationRequest(GOI_BAUCUA, (unsigned int)&Data, 0);
			}
		} else if (uParam == (unsigned int)(KWndWindow*)&m_BetBtn_5) { // SHRIMP
			DiceFace targetFace = DiceFace::SHRIMP;
			if (m_playerCoins >= betAmount) {
				m_playerCoins -= betAmount;
				m_currentBets.push_back({targetFace, betAmount});
				sprintf(coinStr, "%d", m_playerCoins);
				m_Coin.SetText(coinStr);
				// Set CoinInput text to total bet of this bet face in m_currentBets
				int totalBet = 0;
				for (const auto& bet : m_currentBets) {
					if (bet.face == targetFace) {
						totalBet += bet.amount;
					}
				}
				m_CoinInput[static_cast<int>(DiceFace::SHRIMP)].SetText(std::to_string(totalBet).c_str());
				BauCuaData	Data;

				Data.nActionType = BAUCUA_BET;
				Data.nBetType = (int)DiceFace::SHRIMP;
				Data.nMoney = betAmount;
				g_pCoreShell->OperationRequest(GOI_BAUCUA, (unsigned int)&Data, 0);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_MakeHostBtn) { //Gianh quyen lam cai
			if (m_playerCoins < m_HostCoinAmount)
			{
				//thong bao("Khong du tien de lam cai!");
				return 0;
			}
			BauCuaData	Data;

			Data.nActionType = BAUCUA_MAKE_HOST;
			g_pCoreShell->OperationRequest(GOI_BAUCUA, (unsigned int)&Data, 0);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_HostGetCoinBtn) { //Gianh quyen lam cai
			if (m_playerCoins < m_HostCoinAmount)
			{
				//thong bao("Khong du tien de lam cai!");
				return 0;
			}
			BauCuaData	Data;

			Data.nActionType = BAUCUA_NO_HOST;
			g_pCoreShell->OperationRequest(GOI_BAUCUA, (unsigned int)&Data, 0);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelBetBtn) { //Cancel all bets
			if (m_playerCoins < m_HostCoinAmount)
			{
				//thong bao("Khong du tien de lam cai!");
				return 0;
			}
			BauCuaData	Data;

			Data.nActionType = BAUCUA_CANCEL_BET;
			g_pCoreShell->OperationRequest(GOI_BAUCUA, (unsigned int)&Data, 0);
			for (i = 0; i < 6; i++) {
				m_CoinInput[i].SetText("");
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_DepositBtn) {
			BauCuaData	Data;

			Data.nActionType = BAUCUA_DEPOSIT;
			Data.nMoney = m_DepositAmount.GetIntNumber();
			g_pCoreShell->OperationRequest(GOI_BAUCUA, (unsigned int)&Data, 0);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_WithdrawBtn) {
			BauCuaData	Data;

			Data.nActionType = BAUCUA_WITHDRAW;
			g_pCoreShell->OperationRequest(GOI_BAUCUA, (unsigned int)&Data, 0);
		}
		break;
	}
	default:
		return KWndWindow::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiSpringGame::UpdateDiceImages(const std::vector<DiceFace>& roll) {
	if (remainSeconds < 5 || remainSeconds > 59) {
		//set dice frame random
		std::random_device rd;  // Obtain a random number from hardware
		std::mt19937 eng(rd()); // Seed the generator
		std::uniform_int_distribution<> distr(0, static_cast<int>(DiceFace::COUNT) - 1); // Define the range
		if (m_Dice_0.IsVisible()) m_Dice_0.SetFrame(static_cast<int>(distr(eng)));
		if (m_Dice_1.IsVisible()) m_Dice_1.SetFrame(static_cast<int>(distr(eng)));
		if (m_Dice_2.IsVisible()) m_Dice_2.SetFrame(static_cast<int>(distr(eng)));
	}
	else {
		if (m_Dice_0.IsVisible()) m_Dice_0.SetFrame(static_cast<int>(m_lastDiceRoll[0]));
		if (m_Dice_1.IsVisible()) m_Dice_1.SetFrame(static_cast<int>(m_lastDiceRoll[1]));
		if (m_Dice_2.IsVisible()) m_Dice_2.SetFrame(static_cast<int>(m_lastDiceRoll[2]));
	}

}

// Placeholder for handling server-sent dice roll results
void KUiSpringGame::OnReceiveDiceRollFromServer(const std::vector<DiceFace>& serverRoll, int netWinnings)
{
    // TODO: NET_RECEIVE_ROLL_RESULT
    // This function would be called when the server sends the official dice roll and results.
    
    // m_lastDiceRoll = serverRoll; // Update with server's roll
    // m_playerCoins += netWinnings; // Use net winnings from server if it calculates it
                                 // OR recalculate based on serverRoll and local bets if only roll is sent.

    // UpdateDiceImages(m_lastDiceRoll);
    
    // char coinStr[16]; sprintf(coinStr, "%d", m_playerCoins); m_Coin.SetText(coinStr);
    
    // m_currentBets.clear(); // Clear bets after round is officially concluded by server.
    
    // Optional: Display results ("Server says: You won/lost X coins!")
}

// Placeholder for other host-related actions if they were to be implemented
void KUiSpringGame::OnMakeHostResponseFromServer(bool success)
{
    // TODO: NET_HOST_RESPONSE - Handle server response for trying to become a host
}
