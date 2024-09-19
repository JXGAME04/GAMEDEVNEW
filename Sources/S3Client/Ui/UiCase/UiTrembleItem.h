/*******************************************************************************
File        : UiTrembleItem.h
Creator     : Fong KiÒu
create data : 09-08-2020(mm-dd-yyyy)
Description : Kh¶m n¹m
********************************************************************************/

#if !defined(AFX_UITREMBLEITEM_H__69079BE2_10C3_4AA0_A3C2_190285964E36__INCLUDED_)
#define AFX_UITREMBLEITEM_H__69079BE2_10C3_4AA0_A3C2_190285964E36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "../elem/wndobjcontainer.h"
#include "../elem/wndbutton.h"
#include "../elem/wndimage.h"

#define _ITEM_TREMBLE_COUNT 8

class KUiTrembleItem : public KWndImage
{
public:
	KUiTrembleItem();
	virtual ~KUiTrembleItem();

	static        KUiTrembleItem* OpenWindow();      
	static        KUiTrembleItem* GetIfVisible();   
	static void   CloseWindow(bool bDestory = TRUE); 
	static void   LoadScheme(const char* pScheme);  
public:
	void		  UpdateItem(KUiObjAtRegion* pItem, int bAdd);
	void		  UpdateAllItem();
	void		  UpdateData();
	void          ApplyAssemble();
private:
	static        KUiTrembleItem *ms_pSelf;

private:
	void          Initialize();
	virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	virtual void  Breathe();
	void		  OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
private:
	void		  ProcessAssemble();
	int           PlayEffect();                      
private:
	enum THIS_INTERFACE_STATUS
	{
		STATUS_WAITING_MATERIALS = 0,
		STATUS_BEGIN_TREMBLE,
		STATUS_TREMBLING,
		STATUS_CHANGING_ITEM,
	};

private:
	KWndObjectBox m_ItemBox[_ITEM_TREMBLE_COUNT];
	KWndButton    m_Confirm;                        
	KWndButton    m_Cancel;                          

	KWndImage     m_TrembleEffect;                   
private:
	int           m_nStatus;                         
};

#endif 