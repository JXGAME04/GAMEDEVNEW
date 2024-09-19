/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong Ki“u
//	CreateTime:	2020
//	File: UiOptions2.h
*****************************************************************************************/
//#pragma once

#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndList.h"
//#include "../../Engine/Src/LinkStruct.h"

enum	SWORD_ONLINE_OPTION_INDEX2
{
	OPTION_I_START2 = 0,
	OPTION_I_GIAMPLAYER = OPTION_I_START2,	
	OPTION_I_MATNPC,						
	OPTION_I_MATPLAYER,
	OPTION_I_GIAMSKILL,				
	OPTION_INDEX_COUNT2,
};

struct KToggleOptionItem2
{
	char	szName[32];	
	bool	bInvalid;	
	short	bEnable;	
};

class KUiOptions2 : protected KWndImage
{
public:
	static KUiOptions2*	OpenWindow();
	static KUiOptions2*	GetIfVisible();					
	static void			CloseWindow();					
	static void			LoadScheme(const char* pScheme);
	static void			LoadSetting(bool bReload, bool bUpdate);
	void				ToggleOption(int nIndex);
private:
	KUiOptions2();
	~KUiOptions2() {}
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	
	void	Initialize();					
	void	LoadScheme(KIniFile* pIni);
	void	StoreSetting();
	void	UpdateAllToggleBtn();
	void	UpdateAllStatusImg();
private:
	static KUiOptions2* m_pSelf;
private:
	
	#define MAX_TOGGLE_BTN_COUNT 4

	KWndButton		m_CloseBtn;
	KWndLabeledButton	m_ToggleBtn[MAX_TOGGLE_BTN_COUNT];
	KWndImage		m_StatusImage[MAX_TOGGLE_BTN_COUNT];
	unsigned int	m_uEnableTextColor;		
	unsigned int	m_uDisableTextColor;
	unsigned int	m_uInvalidTextColor;	
	int				m_nStatusEnableFrame;
	int				m_nStatusDisableFrame;
	int				m_nStatusInvalidFrame;

	KToggleOptionItem2	m_ToggleItemList[OPTION_INDEX_COUNT2];
	int					m_nFirstControlableIndex;
	int					m_nToggleBtnValidCount;
	int					m_nToggleItemCount;
};
