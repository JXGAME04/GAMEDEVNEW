// -------------------------------------------------------------------------
//	File: UiBreakItem.h
//	Author: Fong Kieu
//	Date: 2021
// -------------------------------------------------------------------------
#ifndef __UiBreakItem_H__
#define __UiBreakItem_H__

#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndEdit.h"
#include "../../../core/src/gamedatadef.h"
#include "../Elem/WndLabeledButton.h"

struct KUiBreakInfo;

class KUiBreakItem : protected KWndImage
{
public:
	static KUiBreakItem*	OpenWindow(KUiObjAtContRegion* pObj, KUiItemBuySelInfo* pInfo, int nMaxItem);
	static KUiBreakItem*	GetIfVisible();
	static void				CloseWindow(bool bDestroy);	
public:
	bool					IsCheck;		
private:
	KUiBreakItem();
	~KUiBreakItem() {}
	int		Initialize();								
	void	Show();
	void	Hide();
	void	LoadScheme(const char* pScheme);			
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnCancel();
	void	OnCheck();	
	void	OnOk();
	void	OnCheckInput();
	void	OnIncrease();
	void	OnDecrease();
private:
	static KUiBreakItem*	m_pSelf;
	KWndText32			m_Title;
	KWndEdit32			m_Number;
	int					m_nMaxItem;
	KWndLabeledButton	m_OkBtn;
	KWndLabeledButton	m_CancelBtn;
	KUiObjAtContRegion	m_ItemInfo;
	KWndText32			m_ItemName;
	KUiItemBuySelInfo	m_BreakInfo;
	KWndButton			m_Increase;
	KWndButton			m_Decrease;
	KWndButton			m_CheckBtn;	
};

#endif // __UiBreakItem_H__