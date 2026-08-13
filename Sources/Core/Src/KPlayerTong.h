//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerTong.h
// Date:	2003.08.12
// Code:	Fong Ki“u
// Desc:	KPlayerTong Class
//---------------------------------------------------------------------------

#ifndef KPLAYERTONG_H
#define KPLAYERTONG_H

class KPlayerTong
{
	friend class KPlayer;
public:
	int			m_nPlayerIndex;		
	int			m_nFlag;			
	int			m_nFigure;			
	int			m_nCamp;			
	int			m_nRecruit;
	DWORD		m_dwTongNameID;		// dword
	char		m_szName[32];		
	char		m_szTitle[32];		
	int			m_nApplyTo;			// playerindex
	DWORD		m_dwMoney;
	int			m_nTongLevel;
	int			m_nTongExp;
	bool		m_bIsFull;	// tong is full

public:
	char		m_szMasterName[32];	

private:
	BOOL		CheckAcceptAddApplyCondition();	
	BOOL		CheckRecruitClose(int nPlayerIdx, DWORD dwNameID);

public:
	void		Init(int nPlayerIdx);
	void		Clear();
	void		GetTongName(char *lpszGetName);
	void		GetTongMasterName(char *lpszGetMasterName);	
	void		GetTongTitle(char *lpszGetTitle);
	DWORD		GetTongNameID();
	void		SetTongNameID(DWORD dwID) { m_dwTongNameID = dwID; };
#ifndef _SERVER
	// ==== JX2 port: cua so bang hoi kieu JX2 ====
	void		JX2_RequestView(int nPage, int nStart);
	void		JX2_SendOp(int nOp, DWORD dwTarget, int nParam1, int nParam2, const char* pszText);
#endif
	int			CheckIn() {return m_nFlag;};
	int			GetCamp() {return m_nCamp;};
	int			GetFigure() {return m_nFigure;};
	DWORD		GetTongMoney() {return m_dwMoney;};	
	int			GetRecruit() {return m_nRecruit;};
	int			GetTongLevel() {return m_nTongLevel;}
	void		SetTongLevel(int nLevel) { m_nTongLevel = nLevel; }
	int			GetTongExp() { return m_nTongExp; }
	void		SetTongExp(int nExp) { m_nTongExp = nExp; };
	BOOL		CanGetManagerInfo(DWORD dwTongNameID);
	BOOL		CanGetMemberInfo(DWORD dwTongNameID);

#ifndef _SERVER
	BOOL		ApplyCreateTong(int nCamp, char *lpszTongName);
	void		Create(TONG_CREATE_SYNC *psCreate);
	BOOL		ApplyAddTong(DWORD dwNpcID);
	//  bFlag == TRUE  == FALSE
	void		AcceptMember(int nPlayerIdx, DWORD dwNameID, BOOL bFlag);
	BOOL		AddTong(int nCamp, char *lpszTongName, char *lpszTitle, char *lpszMaster);
	BOOL		ApplyInstate(int nCurFigure, int nCurPos, int nNewFigure, int nNewPos, char *lpszName);
	BOOL		ApplyKick(int nCurFigure, int nCurPos, char *lpszName);
	BOOL		ApplyChangeMaster(int nCurFigure, int nPos, char *lpszName);
	BOOL		ApplyLeave();

	/* 
	if nInfoID == enumTONG_APPLY_INFO_ID_SELF 
	if nInfoID == enumTONG_APPLY_INFO_ID_MASTER nParam1 
	if nInfoID == enumTONG_APPLY_INFO_ID_DIRECTOR nParam1 
	if nInfoID == enumTONG_APPLY_INFO_ID_MANAGER nParam1  nParam2  nParam3 
	if nInfoID == enumTONG_APPLY_INFO_ID_MEMBER nParam1 nParam2  nParam3 
	if nInfoID == enumTONG_APPLY_INFO_ID_ONE lpszName 
	if nInfoID == enumTONG_APPLY_INFO_ID_TONG_HEAD nParam1 NpcIndex
	*/
	BOOL		ApplyInfo(int nInfoID, int nParam1, int nParam2, int nParam3, char *lpszName = NULL);

	void		SetSelfInfo(TONG_SELF_INFO_SYNC *pInfo);
	void        OpenCreateInterface();
	BOOL		ApplyChangeTitle(int nCurFigure, int nCurPos, char *lpszName, char *lpszTitle);
	BOOL		ApplyChangeSexTitle(char *lpszTitle, int nSex);
	BOOL		ApplySaveMoney(DWORD nMoney);
	BOOL		ApplyGetMoney(DWORD nMoney);
	BOOL		ApplySndMoney(DWORD nMoney);
	BOOL		FkTongSndMoneyCheckWorld();
	BOOL		ApplyTongChangeCamp(int nCamp);
	BOOL		ChangeCamp(int nCamp);
	BOOL		ApplyTongChangeRecruit(int nRecruit);
	BOOL		ChangeRecruit(int nRecruit);
	BOOL		ApplyTongChangeLevel(int nLevel);
	BOOL		ApplyTongChangeWayEdit(char *lpszStr);
	BOOL		ApplyTongChangeNextTarget(char *lpszStr);
#endif

#ifdef _SERVER
	//  if  return == 0 else return error id
	int			CheckCreateCondition(int nCamp, char *lpszTongName);
	BOOL		Create(int nCamp, char *lpszTongName);
	BOOL		GetOpenFlag();
	BOOL		TransferAddApply(DWORD dwNpcID);
	void		SendRefuseMessage(int nPlayerIdx, DWORD dwNameID);
	BOOL		CheckAddCondition(int nPlayerIdx);
	BOOL		AddTong(int nCamp, char *lpszTongName, char *lpszMasterName, char *lpszTitleName);
	void		SendSelfInfo();
	BOOL		CheckInstatePower(TONG_APPLY_INSTATE_COMMAND *pApply);
	void		BeInstated(STONG_SERVER_TO_CORE_BE_INSTATED *pSync);
	BOOL		CheckKickPower(TONG_APPLY_KICK_COMMAND *pKick);
	void		BeKicked(STONG_SERVER_TO_CORE_BE_KICKED *pSync);
	BOOL		CheckLeavePower(TONG_APPLY_LEAVE_COMMAND *pLeave);
	void		Leave(STONG_SERVER_TO_CORE_LEAVE *pLeave);
	BOOL		CheckChangeMasterPower(TONG_APPLY_CHANGE_MASTER_COMMAND *pChange);
	BOOL		CheckGetMasterPower(STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER *pCheck);
	void		ChangeAs(STONG_SERVER_TO_CORE_CHANGE_AS *pAs);
	void		ChangeMaster(char *lpszMaster);
	void		Login(STONG_SERVER_TO_CORE_LOGIN *pLogin);
	void		DBSetTongNameID(DWORD dwID) { m_dwTongNameID = dwID; if (dwID) m_nFlag = 1;};
	BOOL		CheckChangeTitlePower(TONG_APPLY_CHANGE_TITLE_COMMAND *pChange);
	BOOL		CheckGetTitlePower(STONG_SERVER_TO_CORE_CHECK_GET_TITLE_POWER *pCheck);
	BOOL		CheckChangeSexTitlePower(TONG_APPLY_CHANGE_SEX_TITLE_COMMAND *pChange);
	void		BeChangedTitle(STONG_SERVER_TO_CORE_BE_CHANGED_TITLE *pSync);
	BOOL		CheckMoneyPower(TONG_APPLY_SAVE_COMMAND *pSave);
	void		ChangeMoney(DWORD dwMoney);
	void		ChangeFullStatus(bool bIsFull);
	int			CheckChangeCampCondition(TONG_APPLY_CHANGE_CAMP_COMMAND *pChange);
	void		BeChangedCamp(STONG_SERVER_TO_CORE_BE_CHANGED_CAMP *pSync);
	void		BeChangedRecruit(STONG_SERVER_TO_CORE_BE_CHANGED_RECRUIT *pSync);
	int			CheckChangeRecutCondition(TONG_APPLY_CHANGE_RECRUIT_COMMAND *pChange);
	int			CheckChangeLevelCondition(TONG_APPLY_CHANGE_LEVEL_COMMAND *pChange);
	int			CheckChangeWayEditCondition(TONG_APPLY_CHANGE_WAYEDIT_COMMAND *pChange);
	int			CheckChangeNextTargetCondition(TONG_APPLY_CHANGE_NEXTTARGET_COMMAND *pChange);
#endif
};
#endif
