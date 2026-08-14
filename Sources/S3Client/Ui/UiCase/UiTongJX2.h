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

#define TJX2_UI_ROWS		16	// = defTONG_JX2_RECORD_LINES (nhat ky ve 16 dong)
#define TJX2_UI_TABS		5
#define TJX2_UI_ACTS		6

// O mau alpha lam NEN cho panel chi tiet thanh vien. PHAI la cua so CON
// (khong ve o PaintWindow cua cua so cha): anh nen trang m_PageBg* cung la
// con va ve SAU cha, nen moi thu cha tu ve deu bi de mat - dung loi "chua
// co nen xanh mo" chu game bao.
class KTJX2Shade : public KWndWindow
{
public:
	virtual void	PaintWindow();
};

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
	void				LoadFunMaskImage();	// nap anh ten trang con theo m_nFunSub

private:
	void	Initialize();
	void	RequestPage(int nPage, int nStart);
	void	SendOp(int nOp, unsigned long dwTarget, int nP1, int nP2, const char* pszText);
	// hoi truoc khi lam: nho lai thao tac roi mo hop xac nhan
	void	AskThenSendOp(const char* pszSection, const char* pszKey,
				int nOp, unsigned long dwTarget, int nP1, int nP2);
	void	SwitchPage(int nPage);
	void	RenderInfo();
	void	RenderRecruit();
	void	RenderFunUse();
	void	RenderRecord();
	void	RenderTongList();
	void	RenderOtherZM();
	void	RenderMembers(int nOffset = 0);
	void	RenderWorkshop();
	void	RenderAnnounce();
	void	ClearRows();
	void	ClearMemberRows();	// chi xoa panel danh sach phai
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
	KWndText80			m_MList[TJX2_UI_ROWS];			// danh sach thanh vien (MOI TAB, tach khoi m_Row)
	KWndImage			m_WsIconBg[8];			// nen icon khu (sprite nen goc, 35x48)
	KWndImage			m_WsIconHL[8];			// lop CAO SANG khi khu da lap (33x33)
	KWndText80			m_WsRank[8];			// chu cap ngay duoi tung icon
	KWndLabeledButton	m_WsSub[3];				// 3 nut trang con cua luoi khu
	KWndImage			m_WsIconSel;			// khung chon khu (sprite chon goc)
	KWndImage			m_WsArt;				// buc tranh nen 298x226 cua trang tac phuong
	KWndLabeledButton	m_RecToggle;			// dong/mo tuyen (trang chieu mo)
	KWndButton			m_BtnRowSel[TJX2_UI_ROWS];	// vung bam chon dong (trong suot)
	KWndLabeledButton	m_BtnTab[TJX2_UI_TABS];
	KWndLabeledButton	m_BtnAct[TJX2_UI_ACTS];
	KWndLabeledButton	m_BtnPrev, m_BtnNext;
	// hang dieu khien duoi panel danh sach (blueprint co san, truoc chua noi day)
	KWndLabeledButton	m_MOnline;			// o kiem "Hien thi tren mang"
	KWndLabeledButton	m_MSort;			// nut menu sap xep (7 muc trong ini)
	KWndLabeledButton	m_MJump;			// nut "Chuyen den"
	KWndText80			m_MPage;			// so trang dang xem
	KWndEdit32			m_MPageEdit;		// o nhap trang muon den
	KWndText80			m_Row[TJX2_UI_ROWS];
	KWndText80			m_Info[29];				// trang Tin tuc: 29 o = TJX2_INFO_NUM
	KWndImage			m_InfoBg[29];			// khung nen do / thanh ong cua tung o
											// (KWndText KHONG doc khoa Image= -
											// phai co lop KWndImage chay song song)
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
	KWndText80			m_FunP[7];				// khoi Tin tuc ca nhan: 1 tieu de + 3 hang
											// (Cong hien ca nhan / Ngan quy / Ngan sach kien thiet)
	KWndImage			m_FunPBg[6];			// khung do + thanh ong cua 3 hang do
	KWndImage			m_FunMask;				// anh ten trang con 1-4 (Fun_ImgSubPageMask)
	KWndText80			m_MDet[7];				// panel XANH chi tiet thanh vien
	KTJX2Shade			m_MShade;			// nen mo xanh cua panel chi tiet
	KWndText80			m_RowDim[TJX2_UI_ROWS];			// dong danh sach mau xam (offline)
	KWndLabeledButton	m_FunBtn[26];			// nut hanh dong (>= TJX2_FUN_BTNS = 24)
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
	int		m_nRecStart;		// trang chieu mo: don xin dau tien dang ve
	int		m_nSel;				// dong dang chon trong danh sach thanh vien
	int		m_bOnlineFirst;		// o kiem: dua nguoi dang tren mang len truoc
	int		m_nSortMode;		// kieu sap xep dang chon (0..6 theo menu ini)
	int		m_nOrd[10];			// thu tu HIEN THI -> chi so thanh vien trong goi
	// thao tac dang cho xac nhan (hop UIMessageBox tra ket qua qua WND_M_OTHER_WORK_RESULT)
	int		m_nPendOp;			// -1 = khong co
	unsigned long m_dwPendTarget;
	int		m_nAmtOp;			// op cho hop nhap so tien (-1 = khong co)
	unsigned long m_dwAmtTarget;
	int		m_nPendP1, m_nPendP2;
	char	m_szPendText[32];	// chuoi kem thao tac dang cho xac nhan (ten bang...)
	int		m_nSelWs;			// khu tac phuong dang chon (1..7) - TACH RIENG khoi
								// m_nSel: dung chung khien o trang Phuong tho thanh
								// vien thu 2 luon bi danh dau chon
	// du lieu trang hien tai (ban chep goi sync gan nhat)
	unsigned char	m_byInfo[512];
	unsigned char	m_byMember[1024];
	unsigned char	m_byWs[256];
	unsigned char	m_byRecruit[512];
	int		m_bHasInfo, m_bHasMember, m_bHasWs;
	int		m_bHasRecruit;
	int		m_nRecQX, m_nRecHD[4];	// gia tri menu dang chon (gui khi bam Luu)
	unsigned char	m_byRecord[2048];	// RECORD_SYNC 16 dong ~1671B (1300 cu la VUT GOI -> nhat ky trong)
	unsigned char	m_byList[800];
	int		m_bHasList;
	KWndImage		m_ZmBg;					// nen trang 2x2 Xem tin Bang khac
	KWndText80		m_ZmName[4];
	KWndText80		m_ZmInfo[4];
	KWndText512		m_ZmJiyu[4];			// loi nhan chieu mo (den 128 byte)
	KWndText80		m_ZmQxT[4];
	KWndText80		m_ZmQx[4];
	KWndText80		m_ZmHdT[4];
	KWndText512		m_ZmHd[4];				// 4 ten hoat dong noi chuoi
	KWndLabeledButton	m_ZmLook[4];
	KWndLabeledButton	m_ZmApply[4];
	KWndLabeledButton	m_ZmPrev;
	KWndLabeledButton	m_ZmNext;
	unsigned char	m_byZM[900];
	int		m_bHasZM;
	int		m_nZmStart;
	int		m_bHasRecord;
	int		m_nRcSub;				// 0 muc tieu tuan / 1 thong bao / 2 bang vu / 3 lich su
	int		m_nFunMode;				// trang FunUse: 0 = Tin tuc (an nut), 1 = Su dung chuc nang
	int		m_nFunSub;				// nhom nut 1..4 khi o che do Su dung chuc nang
	DWORD	m_dwRtId[14];			// RightID cua tung nut quyen (doc tu ini)
	int		m_bMDet;
	int		m_nMDetTop;		// y tuyet doi (trong cua so) cua dong dau panel
	int		m_nMDetRows;	// so dong panel dang hien (0 = khong ve nen)
				// dang hien panel chi tiet thanh vien
};

#endif // __UITONGJX2_H__
