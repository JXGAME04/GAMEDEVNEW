//=====================================
// File name    : 
// Develop      : Anthony
//=====================================

#pragma once
#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndShowAnimate.h"
#include "../../core/src/GameDataDef.h"

class KUiTeamManager2 : protected KWndShowAnimate
{
public:
    static KUiTeamManager2 *OpenWindow();
	static KUiTeamManager2 *GetUiCapture();
	static KUiTeamManager2 *GetIfVisible();
	static KUiTeamManager2* GetUiTeamManager();
    static void         CloseWindow();

private:
    KUiTeamManager2();
    ~KUiTeamManager2();

    static KUiTeamManager2* m_pSelf; 
    void Initialize();
    void LoadScheme(const char* pScheme);
    int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void Clear();
	void SetFactionIcon(BYTE faction, KWndButton &btn);
	void	PaintWindow();
public:
	void UpdateData(KUiPlayerTeam* pInfo);
	BYTE eShowTMG;
private:

	KWndButton		m_btnSwitch;			// ÑûÇë
	KWndButton		m_btnCaptainFlag;				// Ìß³ö

	KWndText32		a_LeaderAbility[8];
	KWndButton		a_IconHead[8];
	KWndButton		a_btnBackGround[8];

	KUiPlayerTeam	m_Info;
	KUiPlayerItem*	m_pPlayersList;
	KUiPlayerPaintTeamMNG* nPainTMG;
	int nCountS;
	int	nWeightOffset;
	int	nHeightOffset_life;
	int	nHeightOffset_mana;
	int	nOffSet_member;
	int	nWid;
	int	nHei_life;
	int	nHei_mana;
};


