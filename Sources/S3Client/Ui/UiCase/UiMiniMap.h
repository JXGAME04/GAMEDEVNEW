/*****************************************************************************************
//	Copyright : Kingsoft 2003
//	Author	:   Fong Ki“u
//	CreateTime:	2021-4-21
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "../Elem/WndImage.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndShadow.h"
#include "../Elem/PopupMenu.h"
#include "../../Core/Src/GameDataDef.h"

enum MINIMAP_MODE
{
	MINIMAP_M_BRIEF_NOT_PIC = -1,			
	MINIMAP_M_BRIEF_PIC = 0,			
	MINIMAP_M_BRIEF_PIC_BROWSE,
	MINIMAP_M_BRIEF_PIC_BROWSEEX,
	MINIMAP_M_WORLD_MAP,				
	MINIMAP_M_CAVELIST_MAP,			
	MINIMAP_M_NONE,				
	MINIMAP_M_COUNT,
};

struct KUiSceneTimeInfo;

class KUiMiniMap : public KWndImage
{
public:

	static KUiMiniMap*	OpenWindow();					
	static KUiMiniMap*	GetIfVisible();					
	static void			CloseWindow();					

	static void			SetMode(MINIMAP_MODE eMode);	
	static void			LoadScheme(const char* pScheme);
	static void			UpdateSceneTimeInfo(KUiSceneTimeInfo* pInfo);

	static KUiMiniMap*	GetSelf() {return ms_pSelf;}

	void	Show();										
	void	Hide();										

private:
	KUiMiniMap();
	~KUiMiniMap() {}
	void	Initialize();
	void	PaintWindow();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	LoadScheme(KIniFile* pIni);		
	void	SetStatus(bool bSmall);			
	void    MapScroll(int nbScrollScene);	
	void	StopScrollMap();				
	void	MapMoveBack();					
	virtual void	Breathe();

private:
	static KUiMiniMap*	ms_pSelf;
private:
	unsigned int	m_uShowElem;
	POINT			m_MapPos;
	SIZE			m_MapSize;
	KWndButton		m_SwitchBtn;
	KWndText32		m_SceneName;
	KWndImageTextButton		m_ScenePos;
	KWndShadow		m_Shadow;
	KWndButton		m_WorldMapBtn;
	KWndButton		m_CaveMapBtn;
	KWndButton		m_FlagBtn;
	KWndButton		m_LockBtn;
	
	POINT           m_OldPos;
	unsigned int	m_uLastScrollTime;           
	int             m_nOffsetX;             
	int             m_nOffsetY;             

	int				m_MpsX;
	int				m_MpsY;
	DWORD			m_MpsID;
	char 			m_szMapName[80];
};

void	MapToggleStatus();
void	MapSetMode(MINIMAP_MODE eMode);	
MINIMAP_MODE MapGetMode();