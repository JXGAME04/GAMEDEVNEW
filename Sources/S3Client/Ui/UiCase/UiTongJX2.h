/*******************************************************************************
File        : UiTongJX2.h
Description : Cua so bang hoi kieu JX2 (6 tab nhu ban Linux: Tin tuc / Chieu mo /
              Tac phuong / Su dung chuc nang / Phan phoi / Nhat ky) - du lieu tu
              ban sao GS qua goi TONG_JX2_*_SYNC (xem CLIENT_BANGHOI_SPEC.md va
              PHANTICH_BANGHOI_UI_VS_LINUX_0709.md phan dac ta game_y.exe).
              [BH100 07/09] lam theo ban goc 100%: panel thanh vien 3 cot 25 dong,
              tooltip khi bam, menu sap xep, 2 trang con quyen han, thanh tien do,
              giup do, cua so danh sach bang + xem bang khac, muc tieu tuan.
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
#include "../elem/WndMessageListBox.h"
#include "../elem/wndscrollbar.h"

#define TJX2_UI_ROWS		25	// [BH100] 25 dong/trang danh sach (ban goc MaxMsgCount=25); nhat ky dung 16 dong dau
#define TJX2_UI_TABS		6	// [BH100] 6 tab dung ban goc (BtnBaseInfo/ZhaoMu/Workshop/FunUse/RightManage/TongRecord)
#define TJX2_UI_BGS			5	// nen phan trang PageBg0..4
#define TJX2_UI_ACTS		6
#define TJX2_INFO_NUM		35	// [BH100] +2: TitleStandFund/TxtStandFund
#define TJX2_FUN_TXTS		17	// [BH100] +2: TitleStoredBuildFund/TxtStoredBuildFund
#define TJX2_FUN_BTNS		34	// [BH100] +10 nut ban goc con thieu

// O mau alpha lam NEN cho panel (cua so CON de ve DUOI chu, TREN anh nen)
class KTJX2Shade : public KWndWindow
{
public:
	KTJX2Shade() { m_uRGB = (20 << 16) | (90 << 8) | 70; m_nAlpha = 150; }
	void			SetShade(unsigned int uRGB, int nAlpha) { m_uRGB = uRGB; m_nAlpha = nAlpha; }
	virtual void	PaintWindow();
private:
	unsigned int	m_uRGB;
	int				m_nAlpha;
};

// [BH100] thanh tien do (o TxtBuildFund / TxtPersonalOffer cua ban goc dung anh thanh
// tien do: phan da dat to mau theo ti le). Cua so con, khong an chuot.
class KTJX2Bar : public KWndWindow
{
public:
	KTJX2Bar() { m_nPercent = 0; m_uColor = 0; }
	void			SetBar(int nPercent, unsigned int uColor) { m_nPercent = nPercent; m_uColor = uColor; }
	virtual void	PaintWindow();
private:
	int				m_nPercent;
	unsigned int	m_uColor;
};

class KUiTongJX2;

// [BH100] nut chon dong cua panel thanh vien: tooltip theo co che nut JX1 (WM_MOUSEHOVER ->
// GetToolTipInfo) - thong tin 5 dong cua thanh vien tren dong do (ban goc: bam ten hien tip)
class KTJX2RowBtn : public KWndButton
{
public:
	KTJX2RowBtn() { m_pOwner = NULL; m_nRow = -1; }
	virtual int		GetToolTipInfo(char* szTip, int nMax);
	KUiTongJX2*		m_pOwner;
	int				m_nRow;
};

// [BH100] cua so TRO GIUP bang hoi (nut BtnTongHelp goc mo he tro giup chi tiet cua JX2;
// noi dung chu de bang hoi trong ban VN chi con 4 trang anh 800x600 - hien lan luot)
class KUiTongHelpJX2 : public KWndImage
{
public:
	static KUiTongHelpJX2*	OpenWindow();
	static KUiTongHelpJX2*	GetIfVisible();
	static void				CloseWindow(bool bDestroy = true);
	static void				LoadScheme(const char* pScheme);
	virtual int				WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
private:
	void	Initialize();
	void	ShowPage(int nPage);
private:
	static KUiTongHelpJX2*	ms_pSelf;
	KWndLabeledButton	m_BtnClose, m_BtnPrev, m_BtnNext;
	char				m_szPage[4][160];
	int					m_nPage;
	int					m_nPages;
};

// [BH100] cua so DANH SACH BANG (blueprint "guild-list": 120x463, 25 ten/trang) - mo
// bang nut BtnTongList tren cua so chinh; chon mot bang -> cua so chinh xem bang do.
class KUiTongListJX2 : public KWndImage
{
public:
	static KUiTongListJX2*	OpenWindow();
	static KUiTongListJX2*	GetIfVisible();
	static void				CloseWindow(bool bDestroy = true);
	static void				LoadScheme(const char* pScheme);
	static void				OnListData(unsigned char* pData, int nLen);	// TONG_JX2_TONGLIST_SYNC
	virtual int				WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
private:
	void	Initialize();
	void	Render();
	void	Request(int nStart);
private:
	static KUiTongListJX2*	ms_pSelf;
	KWndLabeledButton	m_BtnClose;
	KWndText80			m_Row[TJX2_UI_ROWS];
	KWndButton			m_RowSel[TJX2_UI_ROWS];
	KWndLabeledButton	m_BtnPrev, m_BtnNext;
	unsigned char		m_byList[2048];
	int					m_bHas;
	int					m_nStart;
	int					m_nSel;
};

class KUiTongJX2 : KWndImage
{
	friend class KUiTongListJX2;
public:
	// cau noi cho cua so con (AssignBox/Grant) gui op khi khong co con tro this
	static void	SendOpStatic(int nOp, unsigned long dwTarget, int nP1, int nP2, const char* pszText);
	KUiTongJX2();
	virtual ~KUiTongJX2();

	static KUiTongJX2*	OpenWindow();
	static KUiTongJX2*	GetIfVisible();
	// bam icon bang hoi tren thanh giao dien: dang mo -> dong; co bang -> mo
	// cua so JX2; chua vao bang -> trang 2x2 xem chieu mo bang khac
	static void			ToggleFromIcon();
	static void			CloseWindow(bool bDestroy = true);
	static void			LoadScheme(const char* pScheme);
	// nhan goi TONG_JX2_*_SYNC nguyen ven; nLen < 0 = lenh MO cua so tu server
	static void			DataArrive(unsigned char* pData, int nLen);
	// [BH100] xem bang khac (chi trang Tin tuc, chi doc); 0 = ve bang minh
	static void			ViewTong(unsigned long dwTongID);

	virtual int			WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void				LoadFunMaskImage();	// nap anh ten trang con theo m_nFunSub
	int				GetMemberTip(int nRow, char* szOut, int nMax);	// [BH100] chu tooltip (da TEncodeText) cua dong nRow, 0 = khong co

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
	void	RenderWeekGoal();		// [BH100] bao cao muc tieu tuan vao m_RcList
	void	RenderTongList();
	void	RenderUnionList();		// [BH100] panel phai = bang trong lien minh
	void	RenderTongListPanel();	// [BH100] panel phai = toan bo bang (trang con 2)
	BOOL	HasMyRight(unsigned long dwRightID);	// [BH100] bang chu = moi quyen; con lai theo mat na INFO
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
	void	UpdateRightSub();		// [BH100] hien nut quyen cua trang con dang chon
	void	ShowMemberTip(int nMember);	// [BH100] tooltip 5 dong khi bam ten
	void	HideMemberTip();
	void	PopupMenu(int nKind, int nCount, const char* const* ppszItems);	// [BH100] menu sap xep / chieu mo
	void	PopupIniMenu(int nKind, const char* pszSection, const char* pszKeyFmt, int nCount);
	void	UpdateFunButtons();		// [BH100] hien/an nut trang chuc nang theo chuc vu + trang con
	void	ReadIniString(const char* pszSection, const char* pszKey, char* pszOut, int nSize);
	const char*	GetIniString(const char* pszSection, const char* pszKey, char* pszBuf, int nSize);

private:
	static KUiTongJX2*	ms_pSelf;

	KWndButton			m_BtnClose;
	KWndImage			m_PageBg[TJX2_UI_BGS];	// nen phan trang tung tab
	KWndLabeledButton	m_Chk[12];				// (bo dung - thay bang m_Rt)
	KWndLabeledButton	m_Rt[14];				// 14 nut quyen blueprint (RightID doc tu ini)
	KWndLabeledButton	m_RtAll;				// Chon tat ca
	KWndLabeledButton	m_RtApply;				// Phan quyen (BtnDistribute)
	KWndLabeledButton	m_RtSub[2];				// [BH100] 2 trang con: Quyen han / Phan phat
	KWndLabeledButton	m_Bot[4];				// 4 nut day: Vao bang/Lam moi/Xem tin/Dong
	KWndText80			m_ColHdr[3];			// tieu de cot Hang/Ten/loai hinh
	KWndText80			m_MList[TJX2_UI_ROWS];	// cot TEN (x395)
	KWndImage			m_WsIconBg[8];			// nen icon khu (sprite nen goc, 35x48)
	KWndImage			m_WsIconHL[8];			// lop CAO SANG khi khu da lap (33x33)
	KWndText80			m_WsRank[8];			// chu cap ngay duoi tung icon
	KWndLabeledButton	m_WsSub[3];				// 3 nut trang con cua luoi khu
	KWndImage			m_WsIconSel;			// khung chon khu (sprite chon goc)
	KWndImage			m_WsArt;				// buc tranh nen 298x226 cua trang tac phuong
	KWndLabeledButton	m_RecToggle;			// dong/mo tuyen (trang chieu mo) - an
	KTJX2RowBtn		m_BtnRowSel[TJX2_UI_ROWS];	// vung bam chon dong (trong suot) + tooltip thanh vien
	KWndLabeledButton	m_BtnTab[TJX2_UI_TABS];
	KWndLabeledButton	m_BtnOtherZm;			// [BH100] tab 'chieu mo bang khac' khi CHUA co bang (BtnOtherZhaoMu)
	KWndButton			m_BtnHelp;				// [BH100] nut tro giup goc tren cua so chinh (BtnTongHelp)
	KWndLabeledButton	m_BtnAct[TJX2_UI_ACTS];
	KWndLabeledButton	m_BtnPrev, m_BtnNext;
	// hang dieu khien duoi panel danh sach
	KWndLabeledButton	m_MOnline;			// o kiem "Hien thi tren mang"
	KWndLabeledButton	m_MSort;			// nut menu sap xep thanh vien (7 muc)
	KWndLabeledButton	m_MTongSort;		// [BH100] nut menu sap xep danh sach bang (5 muc, trang lien minh)
	KWndLabeledButton	m_MJump;			// nut "Chuyen den"
	KWndText80			m_MPage;			// so trang dang xem
	KWndEdit32			m_MPageEdit;		// o nhap trang muon den
	KWndText80			m_Row[TJX2_UI_ROWS];	// cot HANG (x343) / dong noi dung trang khac
	KWndText80			m_Info[TJX2_INFO_NUM];	// trang Tin tuc (BASEINFO)
	KWndImage			m_InfoBg[TJX2_INFO_NUM];	// khung nen / thanh ong cua tung o
	KWndButton			m_InfoHelp[TJX2_INFO_NUM];	// [BH100] vung bam tren TIEU DE -> hien HelpInfo o TxtHelp
	KWndText512			m_InfoHelpTxt;			// [BH100] o Info_TxtHelp (MultiLine 250x45)
	KTJX2Bar			m_BarBuild;				// [BH100] thanh tien do kien thiet tuan (Info_TxtBuildFund)
	KTJX2Bar			m_BarOffer;				// [BH100] thanh tien do cong hien tuan ca nhan (Info_TxtPersonalOffer)
	KTJX2Bar			m_BarFunBuild;			// [BH100] thanh tien do o Fun_TxtBuildFund
	KTJX2Bar			m_BarFunOffer;			// [BH100] thanh tien do o Fun_TxtPersonalOffer
	// trang Chieu mo
	KWndText80			m_RecLbl[8];			// 8 nhan tinh
	KWndEdit512			m_RecJiyu;				// van an chieu mo
	KWndEdit32			m_RecAuto;				// cap tu dong nhan
	KWndEdit32			m_RecRefuse;			// tu choi duoi cap
	KWndLabeledButton	m_RecQX;				// khuynh huong (menu)
	KWndLabeledButton	m_RecHD[4];				// 4 hoat dong chu yeu (menu)
	KWndLabeledButton	m_RecSave;				// luu (sprite co chu san)
	KWndLabeledButton	m_RecAccept, m_RecDeny;	// duyet / tu choi don
	KWndLabeledButton	m_RecPrev, m_RecNext;
	// trang Su dung chuc nang
	KWndImage			m_FunBg;
	KWndText80			m_FunTxt[TJX2_FUN_TXTS];	// nhan + o so (bang s_sFunTxt)
	KWndImage			m_FunTxtBg[TJX2_FUN_TXTS];	// khung do / thanh ong
	KWndText80			m_FunP[7];				// khoi Tin tuc ca nhan: 1 tieu de + 3 hang
	KWndImage			m_FunPBg[6];			// khung do + thanh ong cua 3 hang do
	KWndImage			m_FunMask;				// anh ten trang con 1-4 (Fun_ImgSubPageMask)
	KWndText80			m_MDet[7];				// (giu de tuong thich - khong dung nua)
	KTJX2Shade			m_MShade;				// (giu de tuong thich - khong dung nua)
	KWndText80			m_RowDim[TJX2_UI_ROWS];	// cot GIA TRI (x468)
	KWndLabeledButton	m_FunBtn[TJX2_FUN_BTNS + 2];	// nut hanh dong
	KWndLabeledButton	m_FunSub[4];			// 4 nut sub-page
	KWndLabeledButton	m_BtnFun;				// (cu) nut mo trang chuc nang - nay la tab 3, an
	// trang Phuong tho
	KWndText80			m_WsTxt[6];				// So khu/Hang toi da/Phi duy tri (Title+Txt)
	KWndLabeledButton	m_WsBtn[6];				// Lap/Mo/Dong/Nang/Dat cap dung/Xoa
	KWndLabeledButton	m_WsIcon[8];			// icon khu 1..7 (luoi deu)
	KWndText80			m_WsSel;				// dong chi tiet khu dang chon
	// trang Nhat ky
	KWndLabeledButton	m_RcSub[4];				// Muc tieu tuan/Thong bao/Bang vu/Lich su
	KWndEdit512			m_RcEditor;				// khung sua thong bao
	KWndLabeledButton	m_RcLeaveWord, m_RcSave;	// LeaveWord (gui) / EditAnnounce (bat/tat khung sua)
	KWndLabeledButton	m_BtnList;				// nut Danh sach bang tren cua so chinh

	int		m_nPage;			// defTONG_JX2_PAGE_*
	int		m_nStart;			// trang thanh vien: chi so dau
	int		m_nRecStart;		// trang chieu mo: don xin dau tien dang ve
	int		m_nSel;				// dong dang chon trong danh sach thanh vien
	int		m_bOnlineFirst;		// o kiem: dua nguoi dang tren mang len truoc
	int		m_nSortMode;		// kieu sap xep dang chon (0..6 theo menu ini)
	int		m_nTongSort;		// [BH100] kieu sap xep danh sach bang lien minh (0..4)
	int		m_nOrd[TJX2_UI_ROWS];	// thu tu HIEN THI -> chi so thanh vien trong goi
	// thao tac dang cho xac nhan (hop UIMessageBox tra ket qua qua WND_M_OTHER_WORK_RESULT)
	int		m_nPendOp;			// -1 = khong co
	unsigned long m_dwPendTarget;
	int		m_nAmtOp;			// op cho hop nhap so tien (-1 = khong co)
	unsigned long m_dwAmtTarget;
	int		m_nPendP1, m_nPendP2;
	char	m_szPendText[32];	// chuoi kem thao tac dang cho xac nhan (ten bang...)
	int		m_nSelWs;			// khu tac phuong dang chon (1..7)
	// du lieu trang hien tai (ban chep goi sync gan nhat)
	unsigned char	m_byInfo[512];
	unsigned char	m_byMember[4096];	// [BH100] 25 thanh vien x 102 byte = 2560 (2048 cu VUT goi -> danh sach trong)
	unsigned char	m_byWs[256];
	unsigned char	m_byRecruit[512];
	int		m_bHasInfo, m_bHasMember, m_bHasWs;
	int		m_bHasRecruit;
	int		m_nRecQX, m_nRecHD[4];	// gia tri menu dang chon (gui khi bam Luu)
	unsigned char	m_byRecord[2048];	// RECORD_SYNC 16 dong ~1671B
	unsigned char	m_byList[2048];		// [BH100] TONGLIST 25 dong
	int		m_bHasList;
	unsigned char	m_byUnion[2048];	// [BH100] UNIONLIST 25 dong
	int		m_bHasUnion;
	unsigned char	m_byWeek[256];		// [BH100] WEEKGOAL_SYNC
	int		m_bHasWeek;
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
	int		m_bRcEdit;				// [BH100] dang mo khung sua thong bao
	int		m_nFunMode;				// trang FunUse: luon 1 (giu de tuong thich)
	int		m_nFunSub;				// nhom nut 1..4 khi o che do Su dung chuc nang
	int		m_nRtSub;				// [BH100] trang con quyen han dang chon (0/1)
	int		m_nRtPage[14];			// [BH100] SubPage= cua tung nut quyen (doc tu ini)
	DWORD	m_dwRtId[14];			// RightID cua tung nut quyen (doc tu ini)
	int		m_bMDet;				// (khong dung)
	KWndMessageListBox	m_RcList;
	KWndScrollBar		m_RcScroll;
	int		m_nMDetTop;
	int		m_nMDetRows;
	// [BH100] xem bang khac + tooltip + menu
	unsigned long	m_dwViewTong;		// bang KHAC dang xem (0 = bang minh)
	int		m_nListMode;			// panel phai: 0 thanh vien / 1 toan bo bang / 2 bang lien minh
	int		m_bNoTong;				// [BH100] mo khi chua co bang: chi tab chieu mo bang khac
	int		m_nTipMember;			// thanh vien dang hien tooltip (-1 = khong)
	unsigned long	m_dwLastRowClick;	// tick lan bam dong gan nhat (phat hien bam kep)
	int		m_nLastRowClick;
	int		m_nMenuKind;			// menu dang mo: 1 sap xep thanh vien / 2 sap xep bang / 3 khuynh huong / 4..7 hoat dong 1-4 / 8 nguoi choi
	int		m_nSexTitle;			// 0 nam / 1 nu cho hop nhap danh hieu chung
	char	m_szHelp[TJX2_INFO_NUM][256];	// HelpInfo cua tung tieu de trang Tin tuc
	char	m_szFunHelp[TJX2_FUN_TXTS][256];	// HelpInfo trang chuc nang (khong hien - giu)
};

#endif // __UITONGJX2_H__
