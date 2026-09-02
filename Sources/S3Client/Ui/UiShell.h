/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong Ki襲
//	CreateTime:	2002-7-17
*****************************************************************************************/
#pragma once

void		UiPostQuitMsg();						
bool		UiIsAlreadyQuit();						
int			UiInit();								
int			UiStart();								
void		UiExit();								
void		UiPaint(int nGameLoop);					
void		UiSetScreenSize(int nWidth, int nHeight);
int			UiHeartBeat();							
void		UiProcessInput(unsigned int uMsg, unsigned int uParam, int nParam);
void		UiSetGraphicDevice(void* pDevice);		
void		UiStartGame();							
void		UiOnGameServerConnected();				
void		UiOnGameServerStartSyncEnd(bool changegs);	  //fix by phong ki襲 change gs m蕋 skill icon		
void		UiResumeGame();							
void		UiEndGame();							
void		UiSetSwitchSceneStatus(int bSwitching);

#include "Elem/ComWindow.h"
#include "Elem/WndButton.h"

class Player_Life : public KWndImageTextButton
{
public:
	DECLARE_COMCLASS(Player_Life)
	void UpdateData();
	void OnButtonClick();
	static bool m_bText;
};

//魔法
class Player_Mana : public KWndImageTextButton
{
public:
	DECLARE_COMCLASS(Player_Mana)
	void UpdateData();
	void OnButtonClick();
	static bool m_bText;
};

// [VHTD 02/09g] thanh khien tinh (staticmagicshield_p: Tieu Dao Vu 2139, Phat Y_Thuan 2134...) - 'ong mau thu 2' de len thanh sinh luc
class Player_Shield : public KWndImageTextButton
{
public:
	DECLARE_COMCLASS(Player_Shield)
	void UpdateData();
	void OnButtonClick();
};

//体力
class Player_Stamina : public KWndImageTextButton
{
public:
	DECLARE_COMCLASS(Player_Stamina)
	void UpdateData();
	void OnButtonClick();
	static bool m_bText;
};

//经验
class Player_Exp : public KWndImageTextButton
{
public:
	DECLARE_COMCLASS(Player_Exp)
	void UpdateData();
	void OnButtonClick();
	int  GetToolTipInfo(char* szTip, int nMax);
	static bool m_bText;
};

//等级
class Player_Level : public KWndImageTextButton
{
public:
	DECLARE_COMCLASS(Player_Level)
	void UpdateData();
};

//世界排名
class Player_WorldSort : public KWndImageTextButton
{
public:
	DECLARE_COMCLASS(Player_WorldSort)
	void OnButtonClick();
	void UpdateData();
};

//状态
class Player_Status : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_Status)
	void OnButtonClick();
	const char*	GetShortKey();
};

//物品
class Player_Items : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_Items)
	void OnButtonClick();
	const char*	GetShortKey();
};


//武功
class Player_Skills : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_Skills)
	void OnButtonClick();
	const char*	GetShortKey();
};

//队伍
class Player_Team : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_Team)
	void OnButtonClick();
	const char*	GetShortKey();
};

//门派
class Player_Faction : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_Faction)
	void OnButtonClick();
	const char*	GetShortKey();
};

// Auto Play by dammejx (dang minh vu)
class Player_AutoPlay : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_AutoPlay)
	void OnButtonClick();
	void UpdateData();
	const char*	GetShortKey();
};

class Player_ItemEx : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_ItemEx)
	void OnButtonClick();
	const char*	GetShortKey();
};

//打坐
// Nut GHI BAN DIEN .jxr tren thanh cong cu.
// Ten lop PHAI dung y "Player_Recorder" vi no la khoa tra cuu ClassType trong
// tep ini, va day cung la ten lop cua ban tham chieu (VFTABLE 0x0069A0BC).
class Player_Recorder : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_Recorder)
	void OnButtonClick();
	const char*	GetShortKey();
	void UpdateData();
};

class Player_Sit : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_Sit)
	void OnButtonClick();
	void UpdateData();
	const char*	GetShortKey();
};

//跑步
class Player_Run : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_Run)
	void OnButtonClick();
	void UpdateData();
	const char*	GetShortKey();
};

//上马
class Player_Horse : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_Horse)
	void OnButtonClick();
	void UpdateData();
	const char*	GetShortKey();
};

//交易
class Player_Exchange : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_Exchange)
	void OnButtonClick();
	void UpdateData();
	const char*	GetShortKey();
};

//PK开关
class Player_PK : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_PK)
	void OnButtonClick();
	void UpdateData();
	const char*	GetShortKey();
};

class Player_Friend : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_Friend)
	void OnButtonClick();
	void UpdateData();
	const char*	GetShortKey();
};

class Player_Options : public KWndButton
{
public:
	DECLARE_COMCLASS(Player_Options)
	void OnButtonClick();
	void UpdateData();
	const char*	GetShortKey();
};

