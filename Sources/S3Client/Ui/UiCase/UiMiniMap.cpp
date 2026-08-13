/*****************************************************************************************
//	Copyright : Kingsoft 2003
//	Author	:   Fong Ki“u
//	CreateTime:	2021-4-21
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "UiMiniMap.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "UiUnlockBox.h"
#include "UiWorldMap.h"
#include "UiMapCave.h"
#include "UiFindPos.h"
#include "UiGetString.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/gamedatadef.h"
#include    "../../Engine/Src/KSG_StringProcess.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../Elem/TextPic.h"
#include <time.h>

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*			g_pCoreShell;
extern IInlinePicEngineSink *g_pIInlinePicSink;
extern int SCREEN_WIDTH;
#define	SCHEME_INI_SMALL	"UiMiniMapSmall.ini"
#define	SCHEME_INI_BIG		"UiMiniMapBig.ini"
#define	SCHEME_INI_NOPIC			"UiMiniMapNotPic.ini"
#define	SCHEME_INI_BIGEX			"UiMiniMapEx.ini"
#define	defMSG_FORMAT_SCENEPOS 		"%d/%d"
#define	defMSG_FORMAT_SCENEPOSF 	"%d/%d T◊m"

#define MAP_RECOVER_TIME		3000
#define	NOT_DRAGING_MAP			-1
#define	WAIT_TO_BE_SET_BACK		-2
#define	IS_DRAGING_MAP			(m_OldPos.x != NOT_DRAGING_MAP)
#define	IS_WAIT_TO_SET_BACK		(m_OldPos.x == WAIT_TO_BE_SET_BACK)

//const char*	s_ModeName[MINIMAP_M_COUNT] = 
//{
//	"1", 
//	"2",
//	"3",
//	"4"
//};

KUiMiniMap*			KUiMiniMap::ms_pSelf = NULL;
static MINIMAP_MODE	s_eMapMode = MINIMAP_M_NONE;
static MINIMAP_MODE	s_eMapOldMode = MINIMAP_M_BRIEF_PIC;
static BOOL	m_bFlagging = FALSE;
KUiMiniMap::KUiMiniMap()
{
	m_OldPos.x = NOT_DRAGING_MAP;
	m_szFlagImage[0] = 0;
}

KUiMiniMap* KUiMiniMap::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiMiniMap;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->Show();
		ms_pSelf->BringToTop();
	}
	return ms_pSelf;
}

KUiMiniMap* KUiMiniMap::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}

void KUiMiniMap::Initialize()
{
	AddChild(&m_Shadow);
	AddChild(&m_SceneName);
	AddChild(&m_SwitchBtn);
	AddChild(&m_WorldMapBtn);
	AddChild(&m_CaveMapBtn);
	AddChild(&m_FlagBtn);
	AddChild(&m_LockBtn);
	AddChild(&m_ScenePos);
	m_Style &= ~WND_S_VISIBLE;

	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_NORMAL);
}

void KUiMiniMap::CloseWindow()
{
	if (ms_pSelf)
	{
		ms_pSelf->StopScrollMap();
		ms_pSelf->MapMoveBack();
		if (g_pCoreShell)
			g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING, SCENE_PLACE_MAP_ELEM_NONE, 0);
		ms_pSelf->Destroy();
		ms_pSelf = NULL;
	}
}

void KUiMiniMap::LoadScheme(const char* pScheme)
{
	if (ms_pSelf)
	{
		char		szBuf[128];
		KIniFile	Ini;

		if (s_eMapMode == MINIMAP_M_BRIEF_NOT_PIC)
			sprintf(szBuf, "%s\\%s", pScheme, SCHEME_INI_NOPIC);
		else if (s_eMapMode == MINIMAP_M_BRIEF_PIC_BROWSEEX)
			sprintf(szBuf, "%s\\%s", pScheme, SCHEME_INI_BIGEX);
		else if (s_eMapMode == MINIMAP_M_BRIEF_PIC)
			sprintf(szBuf, "%s\\%s", pScheme, SCHEME_INI_SMALL);
		else
			sprintf(szBuf, "%s\\%s", pScheme, SCHEME_INI_BIG);
			
		if (Ini.Load(szBuf))
		{
			ms_pSelf->LoadScheme(&Ini);
			if (SCREEN_WIDTH == 1024)
			{
				int nX, nY;
				int dX, dY;
				dX = 1024 - 800;
				ms_pSelf->GetPosition(&nX, &nY);
				ms_pSelf->SetPosition(nX + dX, nY);
				ms_pSelf->Hide();
				ms_pSelf->Show();
			}
		}
	}
}

void KUiMiniMap::LoadScheme(KIniFile* pIni)
{
	if (pIni == NULL)
		return;
	Init(pIni, "MiniMap");
	m_Shadow.Init(pIni, "NameShadow");
	m_SceneName.Init(pIni, "SceneName");
	m_ScenePos.Init(pIni, "ScenePos");
	m_SwitchBtn.Init(pIni, "SwitchBtn");
	m_WorldMapBtn.Init(pIni, "WorldMapBtn");
	m_CaveMapBtn.Init(pIni, "CaveMapBtn");
	m_FlagBtn.Init(pIni, "BtnFlag");	
	m_LockBtn.Init(pIni, "LockBtn");

	pIni->GetInteger("MapRect", "Left",   0, (int*)&m_MapPos.x);
	pIni->GetInteger("MapRect", "Top",    0, (int*)&m_MapPos.y);
	pIni->GetInteger("MapRect", "Width",  0, (int*)&m_MapSize.cx);
	pIni->GetInteger("MapRect", "Height", 0, (int*)&m_MapSize.cy);
	if(!m_szFlagImage[0])
	{
		pIni->GetString("BtnFlag", "FlagImage", "", m_szFlagImage, sizeof(m_szFlagImage));
		pIni->GetInteger("BtnFlag", "FlagOffset", 14, &m_nFlagOffset);
		if (g_pCoreShell)
			g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_FLAGIMG, (unsigned int)&m_szFlagImage, m_nFlagOffset);
	}
	if (IsVisible() && g_pCoreShell)
	{
		g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING,
			SCENE_PLACE_MAP_ELEM_PIC | SCENE_PLACE_MAP_ELEM_CHARACTER | SCENE_PLACE_MAP_ELEM_PARTNER,
			(m_MapSize.cx | (m_MapSize.cy << 16)));
	}
}

int KUiMiniMap::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_SwitchBtn)
			MapSetMode(s_eMapMode == MINIMAP_M_BRIEF_PIC ? MINIMAP_M_BRIEF_PIC_BROWSE : MINIMAP_M_BRIEF_PIC);
		else if (uParam == (unsigned int)(KWndWindow*)&m_WorldMapBtn)
			MapSetMode(MINIMAP_M_WORLD_MAP);
		else if (uParam == (unsigned int)(KWndWindow*)&m_CaveMapBtn)
			MapSetMode(MINIMAP_M_CAVELIST_MAP);
		else if (uParam == (unsigned int)(KWndWindow*)&m_FlagBtn)
		{
			/*BOOL bFlag = g_pCoreShell->GetFlagMode();
			g_pCoreShell->SetFlagMode(!bFlag);
			if(bFlag)
				g_pCoreShell->SetPaintMode(0);
			else
				g_pCoreShell->SetPaintMode(1);*/
			m_bFlagging = !m_bFlagging;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ScenePos)
		{
			KUiFindPos::OpenWindow();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_LockBtn)
		{
			if (g_pCoreShell->GetGameData(GDI_IS_CHEST_UNLOCKED, 0, 0))
			{
				// Ruong dang mo - Khoa ruong lai
				g_pCoreShell->OperationRequest(GOI_CP_LOCK, 0, 0);
			}
			else
			{
				// Ruong dang khoa ~> Mo cua so nhap pass ruong
				KUiUnlockBox::OpenWindow();
			}
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MOUSEHOVER:
		{
			int nCursorX, nCursorY;
			Wnd_GetCursorPos(&nCursorX, &nCursorY);
			int nX = m_MapPos.x + m_nAbsoluteLeft;
			int nY = m_MapPos.y + m_nAbsoluteTop;
			if(nCursorX >= nX && nCursorX < nX + m_MapSize.cx
			&& nCursorY >= nY && nCursorY < nY + m_MapSize.cy)
			{
				if(uMsg == WM_LBUTTONDOWN)
				{
					if(m_bFlagging)
						m_bFlagging = FALSE;
					g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FLAG_ON_TARGET,
						nCursorX - nX, nCursorY - nY);
				}
			}
			else
				Wnd_TransmitInputToGameSpace(uMsg, uParam, nParam);
			break;
		}
	/*case WM_LBUTTONDOWN: 
		break;
	case WM_LBUTTONUP://toa do
		{ 	
			int nCursorX, nCursorY;
			Wnd_GetCursorPos(&nCursorX, &nCursorY);		
			if (g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_DIRECT_MAP, nCursorX, nCursorY))
			{
				g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_DO_DIRECT_MAP, nCursorX, nCursorY);
			}
			else
			{
				g_pCoreShell->SetPaintMode(1);
				g_pCoreShell->SetFlagMode(false);
				g_pCoreShell->DirectFindPos(nCursorX, nCursorY, TRUE, TRUE);
			    g_pCoreShell->AutoMove();
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
	case WM_MOUSEHOVER:
		//Wnd_TransmitInputToGameSpace(uMsg, uParam, nParam);
		int nCursorX, nCursorY;
		Wnd_GetCursorPos(&nCursorX, &nCursorY);	
		if (g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_DIRECT_MAP, nCursorX, nCursorY))
			Wnd_SwitchCursor(MOUSE_CURSOR_PICK);
		else
		{
			Wnd_SwitchCursor(MOUSE_CURSOR_NORMAL);
			Wnd_TransmitInputToGameSpace(uMsg, uParam, nParam);
		}
		break;*/
	case WND_N_CHILD_MOVE:
		if (uParam == (unsigned int)(KWndWindow*)&m_SceneName ||
			uParam == (unsigned int)(KWndWindow*)&m_ScenePos ||
			uParam == (unsigned int)(KWndWindow*)&m_Shadow)
		{
			KWndWindow* pWnd = (KWndWindow*)uParam;
			int nMoveOffsetX = (short)(nParam & 0xffff);
			int nMoveOffsetY = (short)((nParam >> 16) & 0xffff);

			int x, y;
			pWnd->GetPosition(&x, &y);
			x -= nMoveOffsetX;
			y -= nMoveOffsetY;
			pWnd->SetPosition(x, y);
			GetPosition(&x, &y);
			x += nMoveOffsetX;
			y += nMoveOffsetY;
			SetPosition(x, y);
		}
		break;
	case WM_RBUTTONDOWN:
		Wnd_SetCapture(this);
		Wnd_GetCursorPos((int *)&m_OldPos.x, (int *)&m_OldPos.y);
		break;
	case WM_MOUSEMOVE:
	{
		int nCursorX, nCursorY;
		Wnd_GetCursorPos(&nCursorX, &nCursorY);	
		if(IS_DRAGING_MAP && (uParam & MK_RBUTTON))
		{
			int nCursorX, nCursorY;
			Wnd_GetCursorPos(&nCursorX, &nCursorY);
			m_nOffsetX = m_OldPos.x - nCursorX;
			m_nOffsetY = m_OldPos.y - nCursorY;
			m_OldPos.x   =  nCursorX;
			m_OldPos.y   =  nCursorY;
			MapScroll(uParam & MK_CONTROL);
		}
		else
		{
			int nX = m_MapPos.x + m_nAbsoluteLeft;
			int nY = m_MapPos.y + m_nAbsoluteTop;
			if(nCursorX >= nX && nCursorX < nX + m_MapSize.cx
			&& nCursorY >= nY && nCursorY < nY + m_MapSize.cy)
				break;
			Wnd_TransmitInputToGameSpace(uMsg, uParam, nParam);
		}
		break;
	}
	case WM_RBUTTONUP:
		if (IS_DRAGING_MAP && ((uParam & MK_CONTROL) == 0) && g_pCoreShell)
			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, 0);
		StopScrollMap();
		break;
	/*case WND_M_OTHER_WORK_RESULT:
		if (nParam > 0)
		{
			int x=0,y=0,i=0;
			const char* pcszTemp = (const char*)nParam;
			char Separator = 0;
			while(pcszTemp[i])
			{
				if(pcszTemp[i] < '0' || pcszTemp[i] > '9')
				{
					Separator = pcszTemp[i];
					break;
				}
				i++;
			}
			x = KSG_StringGetInt(&pcszTemp, 255);
			KSG_StringSkipSymbol(&pcszTemp, Separator);
			y = KSG_StringGetInt(&pcszTemp, 255);
			if (g_pCoreShell->GetFlagMode())
				g_pCoreShell->SetFlagMode(false);
			g_pCoreShell->SetPaintMode(1);
			g_pCoreShell->DirectFindPos(x, y, FALSE, TRUE);
		}
		break;*/
	default:
		nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

void KUiMiniMap::StopScrollMap()
{
	if (IS_DRAGING_MAP)
	{
		m_uLastScrollTime = IR_GetCurrentTime();
		Wnd_ReleaseCapture();
		m_OldPos.x = WAIT_TO_BE_SET_BACK;
	}
}

void KUiMiniMap::MapMoveBack()
{
	m_OldPos.x = NOT_DRAGING_MAP;
	m_nOffsetX = 0;
	m_nOffsetY = 0;
	if (g_pCoreShell)
	{
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, 0);
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, 0, 0);
	}
}

void KUiMiniMap::PaintWindow()
{
	KWndImage::PaintWindow();
	if (g_pCoreShell)
	{
		int nX = m_MapPos.x + m_nAbsoluteLeft;
		int nY = m_MapPos.y + m_nAbsoluteTop;
		g_pCoreShell->SceneMapOperation(GSMOI_PAINT_SCENE_MAP, nX, nY);

		if (g_pRepresentShell)
		{
			KRURect	rect;
			rect.Color.Color_dw = 0xff000000;
			rect.oEndPos.nX = rect.oPosition.nX = nX - 1;
			rect.oEndPos.nY = rect.oPosition.nY = nY - 1;
			rect.oEndPos.nZ = rect.oPosition.nZ = 0;
			rect.oEndPos.nX += m_MapSize.cx;
			rect.oEndPos.nY += m_MapSize.cy;
			g_pRepresentShell->DrawPrimitives(1, &rect, RU_T_RECT, true);
			if(m_bFlagging)
			{
				KRUImage	Img;
				strcpy(Img.szImage, m_szFlagImage);
				Img.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
				Img.nType = ISI_T_SPR;
				Img.uImage = 0;
				Img.nISPosition = IMAGE_IS_POSITION_INIT;
				Img.bRenderFlag = 0;
				Img.nFrame = 0;
				Img.oPosition.nZ = 0;
				int nCursorX, nCursorY;
				Wnd_GetCursorPos(&nCursorX, &nCursorY);
				Img.oPosition.nX = nCursorX;
				Img.oPosition.nY = nCursorY - m_nFlagOffset;
				g_pRepresentShell->DrawPrimitives(1, &Img, RU_T_IMAGE, true);
			}
		}
	}
}

void KUiMiniMap::Breathe()
{
	if (IS_WAIT_TO_SET_BACK && g_pCoreShell && IR_IsTimePassed(MAP_RECOVER_TIME, m_uLastScrollTime))
	{
		MapMoveBack();
	}
	/*
	int nCursorX, nCursorY;	
	if (g_pCoreShell->GetFlagMode())
	{
		Wnd_GetCursorPos(&nCursorX, &nCursorY);	
		g_pCoreShell->DirectFindPos(nCursorX, nCursorY, TRUE, FALSE);
	}*/

	if (g_pCoreShell && g_pCoreShell->GetGameData(GDI_IS_CHEST_UNLOCKED, 0, 0))
	{
		m_LockBtn.CheckButton(0);
	}
	else
	{
		m_LockBtn.CheckButton(1);
	}
	//g_pCoreShell->AutoMove();

}

void KUiMiniMap::UpdateSceneTimeInfo(KUiSceneTimeInfo* pInfo)
{
	if (ms_pSelf && pInfo)
	{
		ms_pSelf->m_SceneName.SetText(pInfo->szSceneName);
		strcpy(ms_pSelf->m_szMapName,pInfo->szSceneName);

		ms_pSelf->m_MpsID = pInfo->nSceneId;
		/*ms_pSelf->m_MpsX = pInfo->nScenePos0/8;
		ms_pSelf->m_MpsY = pInfo->nScenePos1/8;
				
		char Buff[16];
		if (g_pCoreShell->GetFlagMode() && g_pCoreShell->GetPaintMode())
		{
			KUiSceneTimeInfoOften Spot;
			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO_OFTEN, (unsigned int)&Spot, 0);
			if (Spot.nScenePos0 && Spot.nScenePos1)
				sprintf(Buff, defMSG_FORMAT_SCENEPOSF, Spot.nScenePos0, Spot.nScenePos1);
			else
				sprintf(Buff, defMSG_FORMAT_SCENEPOSF, ms_pSelf->m_MpsX, ms_pSelf->m_MpsY);
		}
		else
			sprintf(Buff, s_eMapMode == MINIMAP_M_BRIEF_NOT_PIC?defMSG_FORMAT_SCENEPOS:defMSG_FORMAT_SCENEPOSF, ms_pSelf->m_MpsX, ms_pSelf->m_MpsY);
			
		ms_pSelf->m_ScenePos.SetText(Buff);		*/
		char szBuf[64];
		int nCursorX, nCursorY;
		Wnd_GetCursorPos(&nCursorX, &nCursorY);
		int nX = ms_pSelf->m_MapPos.x + ms_pSelf->m_nAbsoluteLeft;
		int nY = ms_pSelf->m_MapPos.y + ms_pSelf->m_nAbsoluteTop;
		if(m_bFlagging && (nCursorX >= nX && nCursorX < nX + ms_pSelf->m_MapSize.cx
		&& nCursorY >= nY && nCursorY < nY + ms_pSelf->m_MapSize.cy))
		{
			nCursorX -= nX;
			nCursorY -= nY;
			int nParam = (nCursorX << 16) | nCursorY;
			nCursorX = g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_GET_FLAGPOS, 0, nParam);
			nCursorY = g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_GET_FLAGPOS, 1, nParam);
			if(s_eMapMode == MINIMAP_M_BRIEF_NOT_PIC)
			sprintf(szBuf, defMSG_FORMAT_SCENEPOS, nCursorX, nCursorY);
			else
			sprintf(szBuf, defMSG_FORMAT_SCENEPOSF, nCursorX, nCursorY);
		}
		else
		{
			if(s_eMapMode == MINIMAP_M_BRIEF_NOT_PIC)
			sprintf(szBuf, defMSG_FORMAT_SCENEPOS, pInfo->nScenePos0 / 8, pInfo->nScenePos1 / 8);
			else
			sprintf(szBuf, defMSG_FORMAT_SCENEPOSF, pInfo->nScenePos0 / 8, pInfo->nScenePos1 / 8);
		}
		ms_pSelf->m_ScenePos.SetText(szBuf);
	}
}

void KUiMiniMap::Unflagging()
{
	m_bFlagging = FALSE;
}

void KUiMiniMap::Show()
{
	KWndImage::Show();
	if (g_pCoreShell)
	{
		g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING,
		SCENE_PLACE_MAP_ELEM_PIC | SCENE_PLACE_MAP_ELEM_CHARACTER | SCENE_PLACE_MAP_ELEM_PARTNER,
		(ms_pSelf->m_MapSize.cx | (ms_pSelf->m_MapSize.cy << 16)));	
	}
}

void KUiMiniMap::Hide()
{
	KWndImage::Hide();
	if (g_pCoreShell)
		g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING, SCENE_PLACE_MAP_ELEM_NONE, 0);
}

void KUiMiniMap::MapScroll(int nbScrollScene)
{
	KSceneMapInfo MapInfo;
    if(g_pCoreShell && g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_INFO, (unsigned int)&MapInfo, 0))
	{
		int nX = MapInfo.nOrigFocusH + MapInfo.nFocusOffsetH + MapInfo.nScallH * m_nOffsetX;
		int nY = MapInfo.nOrigFocusV + MapInfo.nFocusOffsetV + MapInfo.nScallV * m_nOffsetY;
		if (nX < MapInfo.nFocusMinH)
			nX = MapInfo.nFocusMinH; 
		else if (nX > MapInfo.nFocusMaxH)
			nX = MapInfo.nFocusMaxH;
		if (nY < MapInfo.nFocusMinV)
			nY = MapInfo.nFocusMinV;
		else if (nY > MapInfo.nFocusMaxV)
			nY = MapInfo.nFocusMaxV;

		nX -= MapInfo.nOrigFocusH;
		nY -= MapInfo.nOrigFocusV;

		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, nbScrollScene);
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, (unsigned int)nX, nY);
	}
}

void MapToggleStatus()
{
	if (s_eMapMode == MINIMAP_M_NONE || s_eMapMode == MINIMAP_M_WORLD_MAP || s_eMapMode == MINIMAP_M_CAVELIST_MAP)
	{
		MapSetMode(MINIMAP_M_BRIEF_PIC);
	}
	else if (s_eMapMode == MINIMAP_M_BRIEF_PIC)
	{
		MapSetMode(MINIMAP_M_BRIEF_PIC_BROWSE);
	}
	else if (s_eMapMode == MINIMAP_M_BRIEF_NOT_PIC)
	{
	}
	else
		MapSetMode(MINIMAP_M_BRIEF_PIC);
}

MINIMAP_MODE MapGetMode()
{
	return s_eMapMode;
}

void MapSetMode(MINIMAP_MODE eMode)
{
	if (s_eMapMode == MINIMAP_M_BRIEF_NOT_PIC && eMode != MINIMAP_M_BRIEF_PIC && eMode < MINIMAP_M_WORLD_MAP)
		return;
	if ((s_eMapMode == MINIMAP_M_WORLD_MAP || s_eMapMode == MINIMAP_M_CAVELIST_MAP) && s_eMapOldMode == MINIMAP_M_BRIEF_NOT_PIC)
		eMode = MINIMAP_M_BRIEF_NOT_PIC;
	if (eMode == MINIMAP_M_BRIEF_PIC_BROWSEEX && s_eMapMode == MINIMAP_M_BRIEF_PIC_BROWSEEX)
		eMode = MINIMAP_M_BRIEF_PIC;
	if (eMode == s_eMapMode && (eMode < MINIMAP_M_NONE || eMode > MINIMAP_M_WORLD_MAP || eMode > MINIMAP_M_CAVELIST_MAP))
		return;
	if (s_eMapMode != MINIMAP_M_WORLD_MAP)
		s_eMapOldMode = s_eMapMode;
	switch(s_eMapMode = eMode)
	{
	case MINIMAP_M_NONE:			
		KUiMiniMap::CloseWindow();
		KUiWorldmap::CloseWindow();
		KUiMapCave::CloseWindow();
		break;
	case MINIMAP_M_BRIEF_NOT_PIC:
	case MINIMAP_M_BRIEF_PIC:		
	case MINIMAP_M_BRIEF_PIC_BROWSE:
	case MINIMAP_M_BRIEF_PIC_BROWSEEX:
		KUiWorldmap::CloseWindow();
		KUiMapCave::CloseWindow();
		if (KUiMiniMap::GetIfVisible() == NULL)
			KUiMiniMap::OpenWindow();
		else
		{
			char Scheme[128];
			g_UiBase.GetCurSchemePath(Scheme, 256);
			KUiMiniMap::LoadScheme(Scheme);
		}
		break;
	case MINIMAP_M_WORLD_MAP:		
		KUiMiniMap::CloseWindow();
		KUiWorldmap::OpenWindow();
		break;
	case MINIMAP_M_CAVELIST_MAP:
		KUiMiniMap::CloseWindow();
		KUiMapCave::OpenWindow();
		break;
	}
}
