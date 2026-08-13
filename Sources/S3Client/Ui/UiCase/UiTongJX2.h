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
#include "../elem/wndedit.h"

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
	void	RenderRecruit();
	void	RenderFunUse();
	void	RenderRecord();
	void	RenderTongList();
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
	KWndLabeledButton	m_Chk[12];				// (bo dung - thay bang m_Rt)
	KWndLabeledButton	m_Rt[14];				// 14 nut quyen blueprint (RightID doc tu ini)
	KWndLabeledButton	m_RtAll;				// Chon tat ca
	KWndLabeledButton	m_RtApply;				// Phan quyen (BtnDistribute)
	KWndLabeledButton	m_Bot[4];				// 4 nut day: Vao bang nay/Tao moi/Xem tin/Dong
	KWndText80			m_ColHdr[3];			// tieu de cot Hang/Thanh vien/Chuc vu
	KWndText80			m_MList[14];			// danh sach thanh vien (MOI TAB, tach khoi m_Row)
	KWndImage			m_WsIconBg[8];			// nen icon khu (sprite nen goc)
	KWndImage			m_WsIconSel;			// khung chon khu (sprite chon goc)
	KWndText80			m_Fun2[13];				// hang bo sung tab Tin tuc theo anh mau
	KWndLabeledButton	m_RecToggle;			// dong/mo tuyen (trang chieu mo)
	KWndButton			m_BtnRowSel[TJX2_UI_ROWS];	// vung bam chon dong (trong suot)
	KWndLabeledButton	m_BtnTab[TJX2_UI_TABS];
	KWndLabeledButton	m_BtnAct[TJX2_UI_ACTS];
	KWndLabeledButton	m_BtnPrev, m_BtnNext;
	KWndText80			m_Row[TJX2_UI_ROWS];
	KWndText80			m_Info[32];				// trang Tin tuc: field nguyen van blueprint
	// trang Chieu mo (blueprint trang chieu mo - nhan giu NGUYEN VAN byte TCVN3)
	KWndText80			m_RecLbl[8];			// 8 nhan tinh
	KWndEdit512			m_RecJiyu;				// van an chieu mo
	KWndEdit32			m_RecAuto;				// cap tu dong nhan
	KWndEdit32			m_RecRefuse;			// tu choi duoi cap
	KWndLabeledButton	m_RecQX;				// khuynh huong (xoay vong 0..9)
	KWndLabeledButton	m_RecHD[4];				// 4 hoat dong chu yeu
	KWndLabeledButton	m_RecSave;				// luu (sprite co chu san)
	KWndLabeledButton	m_RecAccept, m_RecDeny;	// duyet / tu choi don
	KWndLabeledButton	m_RecPrev, m_RecNext;
	// trang Su dung chuc nang (blueprint trang chuc nang)
	KWndImage			m_FunBg;
	KWndText80			m_FunTxt[15];			// nhan + o so (bang s_sFunTxt)
	KWndImage			m_FunTxtBg[15];			// khung do / thanh ong (Image cua section)
	KWndText80			m_FunP[3];				// khoi Tin tuc ca nhan
	KWndImage			m_FunPBg[2];			// khung + thanh ong cong hien ca nhan
	KWndText80			m_MDet[7];				// panel XANH chi tiet thanh vien
	KWndText80			m_RowDim[14];			// dong danh sach mau xam (offline)
	KWndLabeledButton	m_FunBtn[14];			// nut hanh dong (bang s_sFunBtn)
	KWndLabeledButton	m_FunSub[4];			// 4 nut sub-page
	KWndLabeledButton	m_BtnFun;				// nut mo trang (tren cua so chinh)
	// trang Phuong tho theo blueprint
	KWndText80			m_WsTxt[6];				// So khu/Hang toi da/Phi duy tri (Title+Txt)
	KWndLabeledButton	m_WsBtn[6];				// Lap/Mo/Dong/Nang/Dat cap dung/Xoa
	KWndLabeledButton	m_WsIcon[8];			// icon khu 1..7 (luoi deu)
	KWndText80			m_WsSel;				// dong chi tiet khu dang chon
	// trang Nhat ky theo blueprint trang ghi chep
	KWndLabeledButton	m_RcSub[4];				// Muc tieu tuan/Thong bao/Bang vu/Lich su
	KWndEdit512			m_RcEditor;				// khung sua thong bao
	KWndLabeledButton	m_RcLeaveWord, m_RcSave;
	KWndLabeledButton	m_BtnList;				// nut Danh sach bang tren cua so chinh

	int		m_nPage;			// defTONG_JX2_PAGE_*
	int		m_nStart;			// trang thanh vien: chi so dau
	int		m_nSel;				// dong dang chon (trang TV/QH) / khu dang chon (TP)
	// du lieu trang hien tai (ban chep goi sync gan nhat)
	unsigned char	m_byInfo[512];
	unsigned char	m_byMember[1024];
	unsigned char	m_byWs[256];
	unsigned char	m_byRecruit[512];
	int		m_bHasInfo, m_bHasMember, m_bHasWs;
	int		m_bHasRecruit;
	int		m_nRecQX, m_nRecHD[4];	// gia tri menu dang chon (gui khi bam Luu)
	unsigned char	m_byRecord[1300];
	unsigned char	m_byList[800];
	int		m_bHasList;
	int		m_bHasRecord;
	int		m_nRcSub;				// 0 muc tieu tuan / 1 thong bao / 2 bang vu / 3 lich su
	int		m_nFunMode;				// trang FunUse: 0 = Tin tuc (an nut), 1 = Su dung chuc nang
	int		m_nFunSub;				// nhom nut 1..4 khi o che do Su dung chuc nang
	DWORD	m_dwRtId[14];			// RightID cua tung nut quyen (doc tu ini)
	int		m_bMDet;				// dang hien panel chi tiet thanh vien
};

#endif // __UITONGJX2_H__
