// KTongSet.h: interface for the CTongSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KTONGSET_H__B42782F1_FA08_4D1C_A209_1ED1F5E0BAA3__INCLUDED_)
#define AFX_KTONGSET_H__B42782F1_FA08_4D1C_A209_1ED1F5E0BAA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	"KTongControl.h"

#define		defTONG_SET_INIT_POINT_NUM		16

class CTongSet
{
public:
	CTongSet();
	virtual ~CTongSet();

private:
	CTongControl**	m_pcTong;
	int				m_nTongPointSize;		// Pointer m_pcTong currently allocated memory size (how many)

public:
	void			Init();
	void			DeleteAll();
	BOOL			InitFromDB();
	// ==== JX2 port ====
	CTongControl*	JX2_FindByNameID(DWORD dwTongNameID);
	int			JX2_UnionMembers(DWORD dwUnionID, CTongControl** ppOut, int nMax);	// liet ke bang thuoc lien minh (field 10)
	CTongControl*	JX2_GetByIndex(int nIndex);	// -1 = het mang, NULL = o trong
	void			JX2_SendFullDump(class CNetConnect* pConn);

	BOOL			SearchOne(DWORD dwTongNameID, char *lpszName, STONG_ONE_LEADER_INFO *pInfo);

	// Create a Guild
	int				Create(int nCamp, char *lpszPlayerName, char *lpszTongName, int nSex = 0);
	// Add a guild member, if return == 0 success else return error id
	int				AddMember(char *lpszPlayerName, char *lpszTongName, int nSex = 0);
	// Get a guild faction
	int				GetTongCamp(int nTongIdx);

	int				GetTongLevel(int nTongIdx);

	int				GetTongExp(int nTongIdx);

	BOOL			GetTongFull(int nTongIdx);

	BOOL			GetMasterName(int nTongIdx, char *lpszName);

	BOOL			GetMemberTitle(int nTongIdx, char *lpszTitle, int nSex);

	BOOL			GetTongHeadInfo(DWORD dwTongNameID, STONG_HEAD_INFO_SYNC *pInfo);

	BOOL			GetTongManagerInfo(STONG_GET_MANAGER_INFO_COMMAND *pApply, STONG_MANAGER_INFO_SYNC *pInfo);

	BOOL			GetTongMemberInfo(STONG_GET_MEMBER_INFO_COMMAND *pApply, STONG_MEMBER_INFO_SYNC *pInfo);

	BOOL			Instate(STONG_INSTATE_COMMAND *pInstate, STONG_INSTATE_SYNC *pSync);

	BOOL			Kick(STONG_KICK_COMMAND *pKick, STONG_KICK_SYNC *pSync);

	BOOL			Leave(STONG_LEAVE_COMMAND *pLeave, STONG_LEAVE_SYNC *pSync);

	BOOL			AcceptMaster(STONG_ACCEPT_MASTER_COMMAND *pAccept);

	BOOL			GetLoginData(STONG_GET_LOGIN_DATA_COMMAND *pLogin, STONG_LOGIN_DATA_SYNC *pSync);

	BOOL			AcceptTitle(STONG_ACCEPT_TITLE_COMMAND *pAccept);

	BOOL			AcceptSexTitle(STONG_ACCEPT_SEX_TITLE_COMMAND *pAccept);

	BOOL			ChangeMoney(STONG_MONEY_COMMAND *pMoney,STONG_MONEY_SYNC *Sync);
	
	int				ChangeCamp(STONG_CHANGE_CAMP_COMMAND *pChange);

	int				ChangeRecruit(STONG_CHANGE_RECRUIT_COMMAND *pChange);

	int				GetExtPoint(STONG_GET_EXTPOINT_COMMAND *pChange);

	BOOL			UpdateExtPoint(STONG_UPDATE_EXTPOINT_COMMAND *pChange);

	BOOL				ChangeTongLevel(STONG_CHANGE_LEVEL_COMMAND *pChange);

	BOOL				ChangeTongExp(STONG_CHANGE_EXP_COMMAND* pChange);

	BOOL				ChangeTongWayEdit(STONG_CHANGE_WAYEDIT_COMMAND *pChange);

	BOOL				ChangeTongNextTarger(STONG_CHANGE_NEXTTARGET_COMMAND *pChange);

public:
};

#endif // !defined(AFX_KTONGSET_H__B42782F1_FA08_4D1C_A209_1ED1F5E0BAA3__INCLUDED_)
