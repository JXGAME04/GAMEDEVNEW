#pragma once
#include "../elem/wndpage.h"
#include "../elem/wndbutton.h"
#include "../elem/wndtext.h"
#include "../../../core/src/gamedatadef.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndShadow.h"
#include "../Elem/WndList2.h"
/*
Author: Fong KiÒu
Function: Ui Auto Qu¶n lý tæ ®éi
File: UiAutoParty.h
Date: 2021
*/

class KUiAutoParty : protected KWndShowAnimate
{
public:
	static KUiAutoParty* OpenWindow(bool bshow);				
	static void			 CloseWindow(bool bDestroy);
	static KUiAutoParty*	GetIfVisible();
	static void		LoadScheme();
private:
	KUiAutoParty();
	~KUiAutoParty();
	void		Initialize();						
	int			WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	
	void		OnClickButton(KWndButton* pBtn);
	void		PopUpInsertPlayerTeam();
	void		SetPlayerListTeam();
	BOOL	InsertPlayerListTeam(char *playerName);
	void		LoadAutoPartySetting();
	void		SaveAutoPartySetting();
	void		ProcessPopUpSelectInsertPlayerJoinTeam(int nAction);
private:
	static KUiAutoParty* m_pSelf;
private:
	KWndLabeledButton	m_Close;
	KWndLabeledButton	m_InsertMem;
	KWndLabeledButton	m_DeleteMem;

	KWndButton			m_AutoPartyAllBtn;
	KWndButton			m_AutoPartyLeadBtn;
	KWndButton			m_AutoPartyRefuInviteBtn;//tõ chèi mäi lêi mêi
	KWndButton			m_AutoPartyPlayerListBtn;
	KWndList2			m_CoordL;
	KWndScrollBar		m_CoordL_Scroll;	
	KWndButton			m_AutoPartyLeaveTeam1Btn;
	KWndButton			m_AutoPartyLeaveTeam2Btn;
	char						m_MoveMpsList[defMAX_AUTO_MOVEMPSL][32];
	KWndText80			m_NameFollowPeople;
	char						m_cFollowName[32];
};
