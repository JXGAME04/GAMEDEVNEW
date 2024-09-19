/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong Ki“u
//	CreateTime:	2021-9-25
*****************************************************************************************/
#pragma once

#include "Windows.h"
#include "WndWindow.h"
#include "../Elem/WndMessage.h"

#define	OBJCONT_S_ENABLE_CLICK_EMPTY	0x00008000
#define	OBJCONT_S_ACCEPT_FREE			0x00004000
#define	OBJCONT_S_HAVEOBJBGCOLOR		0x00002000	
#define	OBJCONT_S_TRACE_PUT_POS			0x00001000
#define	OBJCONT_S_DISABLE_PICKPUT		0x00000800	
#define	OBJCONT_F_MOUSE_HOVER			0x00000400

class KWndObjectBox : public KWndWindow
{
public:
	KWndObjectBox();
	virtual int		Init(KIniFile* pIniFile, const char* pSection);	
	void	LoadScheme(const char* pScheme);			
	void	Celar();									
	void	SetObjectGenre(unsigned int uGenre);		
	int		GetObject(KUiDraggedObject& Obj) const;		
	void	HoldObject(unsigned int uGenre, unsigned int uId, int nDataW, int nDataH);
	void	Clone(KWndObjectBox* pCopy);
	void	SetContainerId(int nId);
	void	EnablePickPut(bool bEnable);
	//vong sang
	int m_time ;
	int n_mpoint;
	int m_count[8];
	unsigned int m_nColor;
	void DrawPoint(int x, int y, unsigned int wColor);
	// Ket thuc
protected:
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	int		DropObject(bool bTestOnly);				
	void	PaintWindow();							
	unsigned int		m_uAcceptableGenre;			
	KUiDraggedObject	m_Object;
	int					m_nContainerId;
	unsigned long       m_ulTimeDelay;
	unsigned short      m_nFrame;
	BOOL                m_bCountFrame;
private:

	//khi di chuyen item giua f3 va f4
	//	CVongSang	m_vongsang;
};

class KWndObjectMatrix : public KWndWindow
{
public:
	KWndObjectMatrix();
	virtual ~KWndObjectMatrix();
	virtual int		Init(KIniFile* pIniFile, const char* pSection);	
	void			Clear();									
	int				AddObject(KUiDraggedObject* pObject, int nCount);	
	int				RemoveObject(KUiDraggedObject* pOjbect);			
	int				GetObject(KUiDraggedObject& Obj, int x, int y) const;
	void			EnableTracePutPos(bool bEnable);
	void			SetContainerId(int nId);
	void			EnablePickPut(bool bEnable);

	//edit by phong kieu code vong sang item
	int m_color ;
	int m_time ;
	int n_mpoint;
	int m_count[8];
	unsigned int m_nColor;
	void DrawPoint(int x, int y, unsigned int wColor);

protected:
	void			Clone(KWndObjectMatrix* pCopy);
private:
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	
	void	PaintWindow();										
	int		GetObjectAt(int x, int y);							
	int		PickUpObjectAt(int x, int y);						
	int		DropObject(int x, int y, bool bTestOnly);			
	int		TryDropObjAtPos(const RECT& dor, KUiDraggedObject*& pOverlaped);
	void	DropObject(int x, int y, KUiDraggedObject* pToPickUpObj);		

protected:
	int				m_nNumUnitHori;		
	int				m_nNUmUnitVert;		
	int				m_nUnitWidth;		
	int				m_nUnitHeight;		
	int				m_nUnitBorder;		
	int				m_nNumObjects;		
	KUiDraggedObject* m_pObjects;		
	int				m_nMouseOverObj;

	int				m_nPutPosX;
	int				m_nPutPosY;
	int				m_nPutWidth;
	int				m_nPutHeight;
	int				m_nContainerId;
	unsigned long       m_ulTimeDelay;
	unsigned short      m_nFrame;
	BOOL                m_bCountFrame;
private:
		//	CVongSang	*m_vongsang;
};

void WndObjContainerInit(KIniFile* pIni);