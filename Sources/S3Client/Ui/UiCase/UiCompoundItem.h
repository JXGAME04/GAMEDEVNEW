/*****************************************************************************************
// Hop thoai che tao trang bi tim
// Author: Fong Ki“u
// Date: 2021
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../elem/wndpage.h"
#include "../Elem/WndImagePart.h"
#include "../elem/wndbutton.h"
#include "../elem/wndtext.h"
#include "../elem/WndObjContainer.h"

#include "../elem/wndlabeledbutton.h"
#include "../elem/wndscrollbar.h"
#include "../elem/wndimage.h"
#include "../elem/wndlist2.h"
#include "../Elem/WndMessageListBox.h"

#include "../../../core/src/gamedatadef.h"

#define _ITEM_COMP_COUNT			3
#define _ITEM_FORGE_COUNT			2
#define _ITEM_OUTIN_COUNT			11

//*************************************** CHE TAO HUYEN TINH ******************************************

class KUiCompoundOne : public KWndPage
{
public:
	KUiCompoundOne();
	void	Initialize();								
	void	LoadScheme(const char* pScheme);	

	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);
	void	UpdateAllItem();
	void	UpdateData();
	void    Breathe();
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	PaintWindow();	
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	OnCancel();
	void	ProcessComp();
	int     PlayEffect(); 
private:

	enum THIS_INTERFACE_STATUS
	{
		STATUS_WAITING_MATERIALS,
		STATUS_BEGIN_COMPOUND,
		STATUS_COMPOUNDING,
		STATUS_CHANGING_ITEM,
	};
	
	int				m_nStatus;

	KWndObjectBox	m_ItemBox[_ITEM_COMP_COUNT];
	KWndMessageListBox		m_Guide;
	KWndText32		m_Pos1;
	KWndText32		m_Pos2;
	KWndText32		m_Pos3;

	KWndScrollBar	m_ListScroll;                      
	KWndButton		m_Compound;
	KWndButton		m_Cancel;

	KWndImage
		          m_CompEffect1;                   
	KWndImage 
		          m_CompEffect2;                   
	KWndImage
		          m_CompEffect3;                   

};

class KUiCompoundTwo : public KWndPage
{
public:
	KUiCompoundTwo();
	void	Initialize();								
	void	LoadScheme(const char* pScheme);			

	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);
	void	UpdateAllItem();
	void	UpdateData();
	void    Breathe();
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	PaintWindow();	
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	OnCancel();
	void	ProcessComp();
	int     PlayEffect(); 
private:
	enum THIS_INTERFACE_STATUS
	{
		STATUS_WAITING_MATERIALS,
		STATUS_BEGIN_COMPOUND,
		STATUS_COMPOUNDING,
		STATUS_CHANGING_ITEM,
	};
	
	int				m_nStatus;

	KWndObjectBox	m_ItemBox[_ITEM_COMP_COUNT];
	KWndMessageListBox		m_Guide;
	KWndText32		m_Pos1;
	KWndText32		m_Pos2;
	KWndText32		m_Pos3;

	KWndScrollBar	m_ListScroll;                      
	KWndButton		m_Compound;
	KWndButton		m_Cancel;

	KWndImage
		          m_CompEffect1;                   
	KWndImage 
		          m_CompEffect2;                   
	KWndImage
		          m_CompEffect3;                     

};

class KUiCompoundThree : public KWndPage
{
public:
	KUiCompoundThree();
	void	Initialize();								
	void	LoadScheme(const char* pScheme);			

	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);
	void	UpdateAllItem();
	void	UpdateData();
	void    Breathe();
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	PaintWindow();
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	OnCancel();
	void	ProcessComp();
	int     PlayEffect(); 
private:
	enum THIS_INTERFACE_STATUS
	{
		STATUS_WAITING_MATERIALS,
		STATUS_BEGIN_COMPOUND,
		STATUS_COMPOUNDING,
		STATUS_CHANGING_ITEM,
	};
	
	int				m_nStatus;

	KWndObjectBox	m_ItemBox[_ITEM_COMP_COUNT];
	KWndMessageListBox		m_Guide;
	KWndText32		m_Pos1;
	KWndText32		m_Pos2;
	KWndText32		m_Pos3;

	int				m_nSelect;

	KWndScrollBar	m_ListScroll;                      
	KWndButton		m_Compound;
	KWndButton		m_Cancel;

	KWndImage
		          m_CompEffect1;                   
	KWndImage 
		          m_CompEffect2;                   
	KWndImage
		          m_CompEffect3;                      

};

class KUiDistill : public KWndPage
{
public:
	KUiDistill();
	void	Initialize();								
	void	LoadScheme(const char* pScheme);			

	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);
	void	UpdateAllItem();
	void	UpdateData();
	void    Breathe();
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	PaintWindow();		
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	OnCancel();
	void	ProcessDistill();
	int     PlayEffect(); 
private:
	enum THIS_INTERFACE_STATUS
	{
		STATUS_WAITING_MATERIALS,
		STATUS_BEGIN_DISTILL,
		STATUS_DISTILLING,
		STATUS_CHANGING_ITEM,
	};
	
	int				m_nStatus;

	KWndObjectBox	m_ItemBox[_ITEM_OUTIN_COUNT];
	KWndMessageListBox		m_Guide;
	KWndText32		m_Pos1;
	KWndText32		m_Pos2;
	KWndText32		m_Pos3;
	KWndText32		m_Pos4;
	
	KWndScrollBar	m_ListScroll;                      
	KWndButton		m_Distill;
	KWndButton		m_Cancle;
	
	KWndImage
					m_DistillEffect1;                   
	KWndImage
					m_DistillEffect2;                   
         
};

class KUiForge : public KWndPage
{
public:
	KUiForge();
	void	Initialize();								
	void	LoadScheme(const char* pScheme);			

	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);
	void	UpdateAllItem();
	void	UpdateData();
	void    Breathe();
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	PaintWindow();	
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	OnCancel();
	void	ProcessForge();
	int     PlayEffect(); 
private:
	enum THIS_INTERFACE_STATUS
	{
		STATUS_WAITING_MATERIALS,
		STATUS_BEGIN_FORGE,
		STATUS_GORGING,
		STATUS_CHANGING_ITEM,
	};
	
	int				m_nStatus;

	KWndObjectBox	m_ItemBox[_ITEM_FORGE_COUNT];
	KWndMessageListBox		m_Guide;
	KWndText32		m_Pos1;
	KWndText32		m_Pos2;
	
	KWndScrollBar	m_ListScroll;                      
	KWndButton		m_ForgeBtn;
	KWndButton		m_Cancle;
	
	KWndImage
					m_ForgeEffect;                   
};

class KUiEnchase : public KWndPage
{
public:
	KUiEnchase();
	void	Initialize();								
	void	LoadScheme(const char* pScheme);			

	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);
	void	UpdateAllItem();
	void	UpdateData();
	void    Breathe();
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	PaintWindow();	
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	OnCancel();
	void	ProcessEnchase();
	int     PlayEffect(); 
private:

enum THIS_INTERFACE_STATUS
	{
		STATUS_WAITING_MATERIALS,
		STATUS_BEGIN_ENCHASE,
		STATUS_ENCHASING,
		STATUS_CHANGING_ITEM,
	};
	
	int				m_nStatus;

	KWndObjectBox	m_ItemBox[_ITEM_OUTIN_COUNT];
	KWndMessageListBox		m_Guide;
	KWndText32		m_Pos1;
	KWndText32		m_Pos2;
	KWndText32		m_Pos3;
	KWndText32		m_Pos4;
	
	KWndScrollBar	m_ListScroll;                      
	KWndButton		m_Enchase;
	KWndButton		m_Cancle;
	
	KWndImage
					m_EnchaseEffect1;                   
	KWndImage
					m_EnchaseEffect2;               
};

//************************************* GIAO DIEN CHINH ***************************************************

// [LOREN 27/08] THE DO PHO - trang "Do pho Hoang Kim".
// Bo cuc theo dung ban goc (Dopho.ini rut tu pak VLTK): 8 o CHINH
// (AtlasBox + CryoliteBox + Box1..Box6) va 1 o tu chon (ItemBox).
class KUiAtlas : public KWndPage
{
public:
	KUiAtlas();
	void	Initialize();
	void	LoadScheme(const char* pScheme);

	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);
	void	UpdateAllItem();
	void	UpdateData();
	void	Breathe();
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	PaintWindow();
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	ProcessAtlas();
	void	CapNhatNguyenLieu();		// [LOREN 28/08] Do pho: ve ten nguyen lieu tren trang
	int		PlayEffect();
private:
	enum THIS_INTERFACE_STATUS
	{
		STATUS_WAITING_MATERIALS,
		STATUS_BEGIN_ATLAS,
		STATUS_ATLASING,
		STATUS_CHANGING_ITEM,
	};

	int				m_nStatus;

	KWndObjectBox	m_ItemBox[_ITEM_OUTIN_COUNT];
	KWndMessageListBox	m_Guide;
	KWndScrollBar	m_ListScroll;
	KWndButton		m_Atlas;
	KWndImage		m_AtlasEffect;
	// [LOREN 28/08] Do pho: ve ten nguyen lieu tren trang
	KWndText32		m_PosAtlas;
	KWndText32		m_PosCryolite;
	KWndText32		m_PosConsume;
	KWndText32		m_Material[6];
};

class KUiCompoundItem : public KWndPageSet
{
public:
	static KUiCompoundItem* OpenWindow();							
	static KUiCompoundItem* GetIfVisible();						
	static void		 CloseWindow(bool bDestory = TRUE);			
	static void		 LoadScheme(const char* pScheme);		
	static void		 ShowWindow(int nNum = 0);
public:
	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);
private:
	virtual void  Breathe();
	void	Initialize();							
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	PopUpContextComMenu();
	void	ProcessCom(int nWindow);
private:
	static KUiCompoundItem* m_pSelf;
private:
	
	int		nWindow;
	enum THIS_WINDOWS
	{
		WINDOWS_COMP = 0,
		WINDOWS_COMP2,
		WINDOWS_COMP3,
		WINDOWS_DISTILL,
		WINDOWS_FORGE,
		WINDOWS_ENCHASE,
		WINDOWS_ATLAS,
	};
	
	enum
	{
		CREATE_CRYOLITE = 0,				
		UP_CRYOLITE,					
		UP_PROPMINE,				
	};

	KUiCompoundOne	m_CompoundOnePad;
	KUiCompoundTwo	m_CompoundTwoPad;
	KUiCompoundThree m_CompoundThreePad;
	KUiDistill		m_DistillPad;
	KUiForge		m_ForgePad;
	KUiEnchase		m_EnchasePad;
	KUiAtlas		m_AtlasPad;		// [LOREN 27/08] THE DO PHO

	KWndButton		m_CompoundPadBtn;
	KWndButton		m_DistillPadBtn;
	KWndButton		m_ForgePadBtn;
	KWndButton		m_EnchasePadBtn;
	KWndButton		m_AtlasPadBtn;
	KWndButton		m_Close;
};
