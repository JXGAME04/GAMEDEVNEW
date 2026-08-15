//=====================================
// File name    : 
// Develop      : Anthony
//=====================================

#pragma once
#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndShowAnimate.h"
#include "../../core/src/GameDataDef.h"

class KUiTargetInfo : protected KWndShowAnimate
{
public:
    static KUiTargetInfo *OpenWindow();
	static KUiTargetInfo *GetUiCapture();
	static KUiTargetInfo *GetIfVisible();
	static KUiTargetInfo* GetUiTargetInfo();
    static void         CloseWindow();

private:
    KUiTargetInfo();
    ~KUiTargetInfo();

    static KUiTargetInfo* m_pSelf; 
    void Initialize();
    void LoadScheme(const char* pScheme);
    int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void Clear();
	void SetFactionIcon(BYTE series, KWndButton &btn);
	void	PaintWindow();
public:
	void UpdateData(KUiTargetDetailInfo* pInfo);
	BYTE eShowTMG;
private:

	KWndButton		m_btnSwitch;			// ÑûÇë
	KWndButton		m_btnCaptainFlag;				// Ìß³ö

	KWndButton		a_IconHead;
	KWndButton		a_btnBackGround;
	KWndText32		m_pLifePercent;
	KWndText32		m_pTargetName;

	KUiTargetDetailInfo	m_Info;
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


