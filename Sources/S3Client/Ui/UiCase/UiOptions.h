/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong Ki“u
//	CreateTime:	2020
*****************************************************************************************/
#pragma once

#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndList.h"
//#include "../../Engine/Src/LinkStruct.h"

struct KPopupMenuData;

enum	SWORD_ONLINE_OPTION_INDEX
{
	OPTION_I_START = 0,
	OPTION_I_DYNALIGHT = OPTION_I_START,	
	OPTION_I_WEATHER,						
	OPTION_I_PERSPECTIVE,
	OPTION_I_QUALITY,				
	OPTION_INDEX_COUNT,
};

struct KToggleOptionItem
{
	char	szName[32];	
	bool	bInvalid;	
	short	bEnable;	
};

class KUiOptions : protected KWndImage
{
public:
	static KUiOptions*	OpenWindow(KWndWindow* pReturn = NULL);
	static KUiOptions*	GetIfVisible();					
	static void			CloseWindow();					
	static void			LoadScheme(const char* pScheme);
	static void			LoadSetting(bool bReload, bool bUpdate);
//	void				SetPerspective(int);
//	void				SetDynaLight(int);
	void				SetMusicValue(int);
	void				SetSoundValue(int);
	void				SetBrightness(int);
//	void                SwitchWeather();
	void				ToggleOption(int nIndex);

private:
	KUiOptions();
	~KUiOptions() {}
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	
	void	OnScrollBarPosChanged(KWndWindow* pWnd, int nPos);	
//	void	PopupSkinMenu();
	void	CancelMenu();
	void	StoreSetting();
	void	UpdateSettingSet(int eSet, bool bOnlyUpdateUi = false);
	void	Initialize();					
	void    PopupSeleteSetMenu(int nX, int nY);
	void	LoadScheme(KIniFile* pIni);
	void	UpdateAllToggleBtn();
	void	UpdateAllStatusImg();

private:
	static KUiOptions* m_pSelf;
private:
	KWndWindow* m_pReturn;

	KWndButton		m_ShortcutKeyBtn;
	KWndButton		m_CloseBtn;		
//	KWndLabeledButton	m_SkinBtn;	
	KWndScrollBar	m_BrightnessScroll;
	KWndScrollBar	m_BGMValue;		
	KWndScrollBar	m_SoundValue;		
	KPopupMenuData*	m_pSkinMenu;

	KWndLabeledButton m_ShortcutSetView;

	int	m_nBrightness, m_nSoundValue, m_nMusicValue;
	int m_nShortcutSet;

	#define	MAX_TOGGLE_BTN_COUNT	4

	KWndScrollBar		m_Scroll;
	KWndLabeledButton	m_ToggleBtn[MAX_TOGGLE_BTN_COUNT];
	KWndImage		m_StatusImage[MAX_TOGGLE_BTN_COUNT];
	unsigned int	m_uEnableTextColor;		
	unsigned int	m_uDisableTextColor;
	unsigned int	m_uInvalidTextColor;	
	int				m_nStatusEnableFrame;
	int				m_nStatusDisableFrame;
	int				m_nStatusInvalidFrame;

	KToggleOptionItem	m_ToggleItemList[OPTION_INDEX_COUNT];
	int					m_nFirstControlableIndex;
	int					m_nToggleBtnValidCount;
	int					m_nToggleItemCount;
};
