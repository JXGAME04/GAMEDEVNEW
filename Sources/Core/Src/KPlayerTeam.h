//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
// File:	KPlayerTeam.h
// Date:	2020.01.06
// Code:	Fong KiÒu
// Desc:	Team Class
//---------------------------------------------------------------------------

#ifndef KPLAYERTEAM_H
#define KPLAYERTEAM_H

#include	"KPlayerDef.h"

#define		MAX_TEAM			MAX_PLAYER

#ifdef _SERVER
class KPlayerTeam	
{
	friend class KPlayer;
public:
	int		m_nFlag;			
	int		m_nID;				// id
	int		m_nFigure;			// TEAM_CAPTAIN TEAM_MEMBER
	int		m_nApplyCaptainID;	// npc id
	int		m_nInviteList[MAX_TEAM_MEMBER];	
	int		m_nListPos;			

private:
	BOOL	m_bCanTeamFlag;

public:
	KPlayerTeam() {Release();};
	void	Release();
	BOOL	CreateTeam(int nIdx, PLAYER_APPLY_CREATE_TEAM *pCreateTeam);
	void	InviteAdd(int nIdx, TEAM_INVITE_ADD_COMMAND *pAdd);
	void	GetInviteReply(int nSelfIdx, int nTargetIdx, int nResult);
	void	SetCanTeamFlag(int nSelfIdx, BOOL bFlag);
	BOOL	GetCanTeamFlag() {return m_bCanTeamFlag;};
};
#endif

#ifndef _SERVER
class KTeamApplyList
{
public:
	DWORD	m_dwNpcID;			// npc id
	DWORD	m_dwTimer;			// 
	int		m_nLevel;			// 
	char	m_szName[32];		// 
public:
	KTeamApplyList() {Release();};
	void	Release() {m_dwNpcID = 0; m_dwTimer = 0; m_nLevel = 0; m_szName[0] = 0;};
};

class KPlayerTeam
{
public:
	int		m_nFlag;			
	int		m_nFigure;			// TEAM_CAPTAIN TEAM_MEMBER
	int		m_nApplyCaptainID;	// npc id
	DWORD	m_dwApplyTimer;		
	BOOL	m_bAutoRefuseInviteFlag;
	KTeamApplyList	m_sApplyList[MAX_TEAM_APPLY_LIST];	
	char		m_AutoPT_PlayerList[defMAX_AUTO_MOVEMPSL][32];//qu¶n lý tæ ®éi
	BOOL	m_bAutoAccecpt;
	BOOL	m_bAutoAccecptAll;//vµo tÊt c¶ c¸c nhãm
	BOOL	m_bPTTongCheckBox;//vµo c¸c nhãm ng­êi mêi cïng bang
public:
	KPlayerTeam();
	void	Release();
	void	ReleaseList();
	BOOL	ApplyCreate();			
	void	InviteAdd(DWORD dwNpcID);						
	void	ReceiveInvite(TEAM_INVITE_ADD_SYNC *pInvite);	
	void	ReplyInvite(int nIdx, int nResult);				
	void	SetAutoRefuseInvite(BOOL bFlag);				
	BOOL	GetAutoRefuseState();							
	int		GetInfo(KUiPlayerTeam *pTeam);
	int		GetTotalMemer(); //add by phong kiÒu
	void	DeleteMemerTimeAbsent(DWORD m_dwNpcID);//add by phong kiÒu ®uæi khái nhãm v¾ng mÆt qu¸ l©u
	void	UpdateInterface();								
	void	DeleteOneFromApplyList(DWORD dwNpcID);			
};
#endif

class KTeam
{
private:
	int		m_nIndex;					
public:
	int		m_nState;									// Team_S_Open Team_S_Close
	int		m_nCaptain;									// id player index npc id
	int		m_nMember[MAX_TEAM_MEMBER];					// id player index npc id
	int		m_nMemNum;								
#ifndef _SERVER
	int		m_nMemLevel[MAX_TEAM_MEMBER + 1];			
	char	m_szMemName[MAX_TEAM_MEMBER + 1][32];	
	DWORD	m_nTeamServerID;						
#endif

public:
	KTeam();											// ¹¹Ôìº¯Êý
	void	Release();									// Çå¿Õ
	void	SetIndex(int nIndex);						// Éè¶¨ Team ÔÚ g_Team ÖÐµÄÎ»ÖÃ
	BOOL	SetTeamOpen();								// Éè¶¨¶ÓÎé×´Ì¬£º´ò¿ª£¨ÔÊÐí½ÓÊÜÐÂ³ÉÔ±£©
	BOOL	SetTeamClose();								// Éè¶¨¶ÓÎé×´Ì¬£º¹Ø±Õ£¨²»ÔÊÐí½ÓÊÜÐÂ³ÉÔ±£©
	int		CalcCaptainPower();							// ¼ÆËã¶Ó³¤ÄÜÍ³Ë§¶ÓÔ±µÄÈËÊý
	int		FindFree();									// Ñ°ÕÒ¶ÓÔ±¿ÕÎ»
	int		FindMemberID(DWORD dwNpcID);				// Ñ°ÕÒ¾ßÓÐÖ¸¶¨npc idµÄ¶ÓÔ±£¨²»°üÀ¨¶Ó³¤£©
#ifdef _SERVER
	BOOL	CreateTeam(int nPlayerIndex);
	BOOL	AddMember(int nPlayerIndex);				
	BOOL	DeleteMember(int nPlayerIndex);				
	BOOL	CheckFull();								
	BOOL	IsOpen();									
	BOOL	CheckIn(int nPlayerIndex);					
#endif
#ifndef _SERVER
	void	CreateTeam(int nCaptainNpcID, char *lpszCaptainName, int nCaptainLevel, DWORD nTeamServerID);
	BOOL	AddMember(DWORD dwNpcID, int nLevel, char *lpszNpcName);
	void	DeleteMember(DWORD dwNpcID);				
	int		GetMemberInfo(KUiPlayerItem *pList, int nCount);	
	BOOL	CheckIn(int nPlayerIndex);
#endif

private:
#ifdef _SERVER
	BOOL	CheckAddCondition(int nPlayerIndex);		
#endif

};

extern	KTeam	g_Team[MAX_TEAM];

#ifdef _SERVER
class KTeamSet
{
public:
	void	Init();									
	int		CreateTeam(int nPlayerID);
private:
	int		FindFree();	
};

extern	KTeamSet	g_TeamSet;

#endif


#endif
