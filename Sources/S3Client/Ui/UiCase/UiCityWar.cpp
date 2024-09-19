/*
Author: Fong KiÒu
Date: 08/2021
Function: UiCityWar C«ng thµnh chiÕn report bÊm nót ctr ~
File: UCityWar.cpp
*/

#include "KWin32.h"
#include "KIniFile.h"
#include "GameDataDef.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "../Elem/MouseHover.h"
#include "UiCityWar.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KDebug.h"
#include "../../../core/src/coreshell.h"
#include <crtdbg.h>

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;

KUiCityWar*			KUiCityWar::m_pSelf = NULL;

#define		SCHEME_INI			"UCityWar.ini"

KUiCityWar::KUiCityWar()
{

}

KUiCityWar::~KUiCityWar()
{

}

KUiCityWar* KUiCityWar::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiCityWar;
		m_pSelf->Initialize();
	}

	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->Show();
		m_pSelf->BringToTop();
	}
	return m_pSelf;
}

void KUiCityWar::CloseWindow(bool bDestory)
{
	if (m_pSelf)
	{
		m_pSelf->Hide();
		if (bDestory)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

KUiCityWar* KUiCityWar::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}

void KUiCityWar::Initialize()
{
	AddChild(&m_TitleTop1);
	AddChild(&m_TitleTop2);

	for(int i=0; i<NUM_T3; i++)
	{
		AddChild(&m_table3[i]);
		AddChild(&m_table3val[i]);
	}

	for(i=0; i<NUM_T4; i++)
	{
		AddChild(&m_table4[i]);
		AddChild(&m_table4val[i]);
	}

	char szScheme[128];
	g_UiBase.GetCurSchemePath(szScheme, sizeof(szScheme));
	LoadScheme(szScheme);
	Wnd_AddWindow(this, WL_TOPMOST);	
}

void KUiCityWar::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	char		Buff2[128];
	KIniFile	Ini;

	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);

	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		Ini.GetString("Main", "LineColor", "", Buff, sizeof(Buff));
		m_LineColor = GetColor(Buff);

		Ini.GetInteger("TopPos", "LinePos1", 0, &m_nLinePos1);
		Ini.GetInteger("TopPos", "LinePos2", 0, &m_nLinePos2);
		Ini.GetInteger("TopPos", "TopInfoPos", 0, &m_nTopInfoPos);
		Ini.GetInteger("Main", "TitleFont", 0, &m_nTitleFont);
		Ini.GetInteger("Main", "ValueFont", 0, &m_nValueFont);

		m_TitleTop1.Init(&Ini, "YellowTxt");
		Ini.GetString("Table8", "CellText_1_1", "", Buff, sizeof(Buff));
		m_TitleTop1.SetText(Buff);
		m_TitleTop1.SetPosition(25, m_nTopInfoPos);

		m_TitleTop2.Init(&Ini, "YellowTxt");
		Ini.GetString("Table9", "CellText_1_1", "", Buff, sizeof(Buff));
		m_TitleTop2.SetText(Buff);
		m_TitleTop2.SetPosition(360, m_nTopInfoPos);

		int table3Y = m_nTopInfoPos + 5;
		for(int i=0; i<NUM_T3; i++)
		{
			m_table3[i].Init(&Ini, "YellowTxt");
			sprintf(Buff2, "CellText_%d_1", i + 1);
			Ini.GetString("Table3", Buff2, "...", Buff, sizeof(Buff));
			m_table3[i].SetText(Buff);
			table3Y = table3Y + 25;
			m_table3[i].SetPosition(25, table3Y);

			m_table3val[i].Init(&Ini, "WhiteTxt");
			m_table3val[i].SetText("0");
			m_table3val[i].SetPosition(25 + 200, table3Y);

		}

		int table4Y = m_nTopInfoPos + 5;
		for(i=0; i<NUM_T4; i++)
		{
			m_table4[i].Init(&Ini, "YellowTxt");
			sprintf(Buff2, "CellText_%d_1", i + 1);
			Ini.GetString("Table4", Buff2, "...", Buff, sizeof(Buff));
			m_table4[i].SetText(Buff);
			table4Y = table4Y + 25;
			m_table4[i].SetPosition(25 + 335, table4Y);

			m_table4val[i].Init(&Ini, "WhiteTxt");
			m_table4val[i].SetText("V« hÖ");
			m_table4val[i].SetPosition(25 + 435, table4Y);
		}
	}
}

void KUiCityWar::Breathe()
{
	TMissionLadderSelfInfo		tmp;
	memset(tmp.szMissionName,0,sizeof(tmp.szMissionName));
	g_pCoreShell->GetGameData(GDI_MISSION_SELFDATA, (unsigned int)(&tmp), NULL);
	if(tmp.szMissionName[0])
	{
		m_table3val[0].SetIntText(tmp.nParam[2]);//player giÕt ®­îc
		m_table3val[1].SetIntText(tmp.nParam[3]);//sè trô ®· ph¸
		m_table3val[2].SetIntText(tmp.nParam[6]);//®iÓm tÝch luü
		for (int i = 0; i < MISSION_STATNUM; i ++)
		{
			TMissionLadderInfo		tmp;
			memset(tmp.Name,0,sizeof(tmp.Name));
			g_pCoreShell->GetGameData(GDI_MISSION_RANKDATA, (unsigned int)(&tmp), i);
			if(tmp.Name[0])
			{
				tmp.nParam[MISSION_PARAM_AVAILABLE]?m_table4val[i].SetText(tmp.Name):m_table4val[i].Clear();//tªn player
			}
		}
	}
}

int KUiCityWar::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	return 0;
}

void KUiCityWar::PaintWindow()
{
	KWndShadow::PaintWindow();
	if (g_pCoreShell)
	{
		KRULine	Line;
		Line.Color.Color_dw = m_LineColor;

		GetAbsolutePos(&Line.oPosition.nX, &Line.oPosition.nY);
		Line.oPosition.nX = Line.oPosition.nX + 25;//add by phong kiÒu
		Line.oPosition.nY += m_nLinePos1;
		Line.oEndPos.nZ = Line.oPosition.nZ = 0;
		GetSize(&Line.oEndPos.nX, NULL);
		Line.oEndPos.nX+=Line.oPosition.nX;
		Line.oEndPos.nX = Line.oEndPos.nX - 385; //add by phong kiÒu
		Line.oEndPos.nY = Line.oPosition.nY;
		g_pRepresentShell->DrawPrimitives(1, &Line, RU_T_LINE, true);

		GetAbsolutePos(&Line.oPosition.nX, &Line.oPosition.nY);
		Line.oPosition.nX = Line.oPosition.nX + 360; //add by phong kiÒu
		Line.oPosition.nY += m_nLinePos2;
		Line.oEndPos.nX = Line.oPosition.nX + 300 - 25;//add by phong kiÒu
		Line.oEndPos.nY = Line.oPosition.nY;
		g_pRepresentShell->DrawPrimitives(1, &Line, RU_T_LINE, true);

		GetAbsolutePos(&Line.oPosition.nX, &Line.oPosition.nY);//add by phong kiÒu line 3
		Line.oPosition.nY = Line.oPosition.nY + 2;
		Line.oPosition.nX = Line.oPosition.nX + 330;
		Line.oEndPos.nX = Line.oPosition.nX;
		Line.oEndPos.nY = Line.oPosition.nY + 300 - 4;
		g_pRepresentShell->DrawPrimitives(1, &Line, RU_T_LINE, true);
	}
}