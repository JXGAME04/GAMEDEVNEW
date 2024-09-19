#pragma once

#include "../Elem/WndText.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndEdit.h"
#include "../../../Core/Src/GameDatadef.h"

#define	MAX_TOP_INFO	11
#define	MAX_BATTLE_GROUP	5

enum BATTLE_MODE
{
	BATTLE_M_SMALL = 0,
	BATTLE_M_BIG,	
	BATTLE_M_COUNT,
};

class KUiBattleReport : public KWndImage
{
public:
	static KUiBattleReport* OpenWindow();
	static void				CloseWindow(bool bDestroy);
	static KUiBattleReport*	GetIfVisible();
	static void				LoadScheme(const char* pScheme);
	static void				UpdateRankWorld(const char* psWorldRank, BYTE nType);
	static void				SetMode(BATTLE_MODE eMode);	
private:
	KUiBattleReport();
	~KUiBattleReport();
	void	Initialize();
	void	LoadScheme(KIniFile* pIni, BATTLE_MODE eMode);
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	
	void	Clear();
private:
	static KUiBattleReport* m_pSelf;
	KWndButton	m_SwitchBtn;	
	KWndText32	m_NumPlayerT;
	KWndText32	m_NumPlayerK;	
	KWndText32	m_TimeRemaining;
	KWndText32	m_PointPlayerT;
	KWndText32	m_PointPlayerK;
	KWndText32	m_PointPlayer;
	KWndText32	m_Team[MAX_TOP_INFO];
	KWndText32	m_Group[MAX_TOP_INFO];	
	KWndText32	m_Name[MAX_TOP_INFO];
	KWndText32	m_Point[MAX_TOP_INFO];	
	KWndText32	m_PlayerPK[MAX_TOP_INFO];
	KWndText32	m_KillNpc[MAX_TOP_INFO];		
	KWndText32	m_Death[MAX_TOP_INFO];		
	KWndText32	m_CurrentKill[MAX_TOP_INFO];
	KWndText32	m_CurrentKillMax[MAX_TOP_INFO];
	KWndText32	m_NumRobFlag[MAX_TOP_INFO];	
	char		szRank[MAX_BATTLE_GROUP][32];
};
