/*******************************************************************************
File        : UiTongJX2.h
Description : Cua so bang hoi kieu JX2 (5 trang: thong tin / thanh vien /
              quyen han / tac phuong / thong bao) - du lieu tu ban sao GS
              qua goi TONG_JX2_*_SYNC (xem CLIENT_BANGHOI_SPEC.md).
********************************************************************************/

#ifndef __UITONGJX2_H__
#define __UITONGJX2_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "../elem/wndlabeledbutton.h"
#include "../elem/wndimage.h"
#include "../elem/wndtext.h"

#define TJX2_UI_ROWS		14
#define TJX2_UI_TABS		5
#define TJX2_UI_ACTS		6

class KUiTongJX2 : KWndImage
{
public:
	KUiTongJX2();
	virtual ~KUiTongJX2();

	static KUiTongJX2*	OpenWindow();
	static KUiTongJX2*	GetIfVisible();
	// bam icon bang hoi tren thanh giao dien: dang mo -> dong; co bang -> mo
	// cua so JX2; chua vao bang -> mo cua so cu (tao / xin vao bang)
	static void			ToggleFromIcon();
	static void			CloseWindow(bool bDestroy = true);
	static void			LoadScheme(const char* pScheme);
	// nhan goi TONG_JX2_*_SYNC nguyen ven; nLen < 0 = lenh MO cua so tu server
	static void			DataArrive(unsigned char* pData, int nLen);

	virtual int			WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private:
	void	Initialize();
	void	RequestPage(int nPage, int nStart);
	void	SendOp(int nOp, unsigned long dwTarget, int nP1, int nP2, const char* pszText);
	void	SwitchPage(int nPage);
	void	RenderInfo();
	void	RenderMembers();
	void	RenderWorkshop();
	void	RenderAnnounce();
	void	ClearRows();
	void	SetupActions();
	void	OnAction(int nIdx);
	void	RepositionRows();		// bo cuc dong theo trang (trai / panel danh sach phai)
	void	LoadChecksFromSel();	// nap o kiem tu mat na quyen cua nguoi dang chon
	void	ApplyRights();			// PHAN QUYEN: gui them/thu theo o kiem da doi

private:
	static KUiTongJX2*	ms_pSelf;

	KWndButton			m_BtnClose;
	KWndImage			m_PageBg[TJX2_UI_TABS];	// nen phan trang (帮会分页-*) tung tab
	KWndLabeledButton	m_Chk[12];				// luoi o kiem 12 quyen (trang Phan phoi)
	KWndButton			m_BtnRowSel[TJX2_UI_ROWS];	// vung bam chon dong (trong suot)
	KWndLabeledButton	m_BtnTab[TJX2_UI_TABS];
	KWndLabeledButton	m_BtnAct[TJX2_UI_ACTS];
	KWndLabeledButton	m_BtnPrev, m_BtnNext;
	KWndText80			m_Row[TJX2_UI_ROWS];
	KWndText80			m_Info[32];				// trang Tin tuc: field nguyen van blueprint

	int		m_nPage;			// defTONG_JX2_PAGE_*
	int		m_nStart;			// trang thanh vien: chi so dau
	int		m_nSel;				// dong dang chon (trang TV/QH) / khu dang chon (TP)
	// du lieu trang hien tai (ban chep goi sync gan nhat)
	unsigned char	m_byInfo[512];
	unsigned char	m_byMember[1024];
	unsigned char	m_byWs[256];
	int		m_bHasInfo, m_bHasMember, m_bHasWs;
};

#endif // __UITONGJX2_H__
