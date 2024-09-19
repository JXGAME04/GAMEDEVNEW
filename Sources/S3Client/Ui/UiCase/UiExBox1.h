/*****************************************************************************************
//	界面--储物箱界面
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-21
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../elem/WndButton.h"
#include "../elem/WndText.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndShowAnimate.h"

struct KUiObjAtRegion;

class KUiExBox1 : protected KWndShowAnimate
{
public:
	static KUiExBox1* OpenWindow();		//打开窗口，返回唯一的一个类对象实例
	static KUiExBox1* OpenWindow2();
	static KUiExBox1* GetIfVisible();		//如果窗口正被显示，则返回实例指针
	static void			CloseWindow();		//关闭窗口
	static void			LoadScheme(const char* pScheme);//载入界面方案
	void			UpdateItem(KUiObjAtRegion* pItem, int bAdd);//物品变化更新
	static void			LoadScheme2(const char* pScheme); //New
private:
	KUiExBox1() {}
	~KUiExBox1() {}
	void	Initialize();							//初始化
	void	Initialize2();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void	UpdateData();
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	OnGetMoney(int nMoney);
private:
	static KUiExBox1*		m_pSelf;
private:
	KWndButton			m_SelectionOne;
	KWndButton			m_BuyTwo;
	KWndButton			m_BuyThree;
	KWndButton			m_CloseBtn;
	KWndObjectMatrix	m_ItemBoxMot;
	KWndText32			m_InfoTextPage;
	KWndText32			m_TextPage;
};