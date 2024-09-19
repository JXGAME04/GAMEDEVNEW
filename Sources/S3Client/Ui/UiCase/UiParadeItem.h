/*****************************************************************************************
//	File: UiParadeItem.h
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-2
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndShowAnimate.h"
#include "../elem/WndObjContainer.h"
#include "../../../core/src/gamedatadef.h"

enum UI_PLAYER_ATTRIBUTE;
struct KUiObjAtRegion;

#define _ITEM_COUNT 17

class KUiParadeItem : protected KWndShowAnimate
{
public:
	static KUiParadeItem*	OpenWindow(KUiPlayerItem* pDest);					//打开窗口，返回唯一的一个类对象实例
	static KUiParadeItem*	GetIfVisible();					//如果窗口正被显示，则返回实例指针
	static void			CloseWindow(bool bDestroy);		//关闭窗口，同时可以选则是否删除对象实例
	static void			LoadScheme(const char* pScheme);//载入界面方案

	void	UpdateData(KUiPlayerItem* pDest);

private:
	KUiParadeItem() {}
	~KUiParadeItem() {}
	void	Initialize();								//初始化	
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	//窗口函数
	void	LoadScheme(class KIniFile* pIni);			//载入界面方案
	void	UpdateBaseData(KUiPlayerItem* pDest);							//更新基本数据（人名等不易变数据）
	void	UpdateAllEquips(KUiPlayerItem* pDest);
	void	UpdateEquip(KUiObjAtRegion* pEquip, int bAdd);	//装备变化更新
private:
	static KUiParadeItem*	m_pSelf;

private:
	KWndWindow	m_Face;
	KWndText32	m_Name;
	KWndText32	m_BangHoi;
	KWndText32	m_Title;	
	KWndText32	m_WorldRank;	//edit by phong kieu xem hang nguoi choi khac
	KWndText32	m_PKValue;
	KWndButton	m_Close;
	KWndButton  m_Avatar;
	KWndText32	m_DanhVong;
	KWndText32	m_PhucDuyen;
	KWndText32	m_TrungSinh;
	KWndText32	m_MateName;

	KWndObjectBox	m_EquipBox[_ITEM_COUNT];

	KUiPlayerItem m_Dest;
};
