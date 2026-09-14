/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong KiÒu
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
#ifdef JX_MOBILE
	OPTION_I_LIA,			// [CAMERA 13/09 TUYCHON] lia canh (mot ngon keo) - Platform/JxLiaCanh
	OPTION_I_NHINRONG,		// chum hai ngon nhin rong
	OPTION_I_VENHANH,		// lia ve nhanh
	OPTION_I_HQQUAI,		// [HAOQUANG 14/09] vong hao quang duoi chan quai tinh anh / boss (Core/Src/KNpc.cpp)
	OPTION_I_HQTRANGBI,		// vong hao quang trang bi cua minh
	OPTION_I_SANGVATROI,	// cot sang + loe vat pham roi (Core/Src/KObj.cpp, VATROI 14/09)
#endif
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
	int		WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam);	
	void	Initialize();					
	void	LoadScheme(KIniFile* pIni);
	void	StoreSetting();
	void	UpdateAllToggleBtn();
	void	UpdateAllStatusImg();
#ifdef JX_MOBILE
	void	PaintWindow();	// [CAMERA 13/09 TUYCHON] nen mo sau cac hang cong tac (main2.spr trong suot o vung do)
#endif
private:
	static KUiOptions2* m_pSelf;
private:
	
	#define MAX_TOGGLE_BTN_COUNT 4
#ifdef JX_MOBILE
#undef MAX_TOGGLE_BTN_COUNT
#define MAX_TOGGLE_BTN_COUNT 10	// [HAOQUANG 14/09] 7 -> 10 (+ hao quang quai / hao quang trang bi / sang vat roi): 5 hang tu Top=70, cach 31 (android/sinh_uioptions2_haoquang.py); [CAMERA 13/09 TUYCHON] = OPTION_INDEX_COUNT2
	typedef char JxKiemSoMucToiUu[(MAX_TOGGLE_BTN_COUNT == OPTION_INDEX_COUNT2) ? 1 : -1];	// khac nhau = vong khoi tao tran mang
#endif

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
#ifdef JX_MOBILE
#undef MAX_TOGGLE_BTN_COUNT
#define MAX_TOGGLE_BTN_COUNT 4	// [CAMERA 13/09 TUYCHON] tra lai 4 cho KUiOptions (UiOptions.cpp include ca hai header, mang cua no chi 4 -> vong for tran); UiOptions2.cpp dat lai 7 sau include
#endif
