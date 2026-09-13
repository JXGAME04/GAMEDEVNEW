# -*- coding: utf-8 -*-
#
# [WAUTO 12/09] Hoan thien WAuto mobile - BANG PHU (danh sach + bo chon dai).
#
#  1. TAO Sources/S3Client/Ui/UiCase/UiWAutoDsach.h/.cpp
#     - KWndDsachWA    : o danh sach cham duoc (dong cao 22 px cho ngon tay), cuon bang hai nut, chon mot dong.
#     - KUiWAutoDsach  : BANG PHU phu len vung noi dung cua khung Auto, ba che do
#         0 SUA DANH SACH  cho 6 danh sach cua WAuto (toa do di tuan, loc thuoc tinh, khong nhat theo ten,
#                          moi/vao nhom, ban dien Lien dau, thu tu ngu hanh) - thay 4 o danh sach + 2 hop
#                          thoai con cua ban PC (IDD_NOPICK_DIALOG, IDD_SETSERIES_DIALOG)
#         1 CHON NGUON     chon mot dong lam nguon them vao danh sach (thuoc tinh / ten vat pham / ten quanh day)
#         2 CHON MOT MUC   thay MENU cho cac hop chon DAI (danh sach chieu toi 72 dong, ban do Xa Phu 31 dong)
#                          - KPopupMenu khong cuon duoc nen dong thu 24 tro di cham khong toi (man 604 px)
#  2. TAO android/du_lieu_ghi_de/ui/ui3/uiwauto_dsach.ini (bo cuc bang phu)
#  3. VA  UiWAuto.h/.cpp : ghep bang phu vao khung (ve sau cung = nam tren), doi tab thi dong bang phu
#  4. VA  android/CMakeLists.txt : them UiWAutoDsach.cpp vao target main
#
# Chi mobile (JX_MOBILE); tep moi khong nam trong vcxproj nao nen ban PC khong biet toi. Chay lai vo hai.

import io
import os
import re
import sys

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # noqa: E402

DAU = "[WAUTO 12/09]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def nl_cua(s):
    return "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"


def thay(s, cu, moi, ten, so=1):
    nl = nl_cua(s)
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho (%d): %s" % (so, s.count(c), ten))


def va(p, dau, ham):
    s = doc(p)
    if dau in s:
        print("   bo qua (da va):", p)
        return
    ghi(p, ham(s))
    print("   da va:", p)


def tcvn_hoa(s):
    """doi moi doan @@tieng Viet@@ trong nguon C sang TCVN3 (latin-1)"""
    return re.sub(r"@@(.*?)@@", lambda m: vn(m.group(1)), s, flags=re.S)


def tao(p, noi_dung, dau):
    if os.path.exists(p) and dau in doc(p):
        print("   bo qua (da co):", p)
        return
    ghi(p, tcvn_hoa(noi_dung).replace("\n", "\r\n"))
    print("   da tao:", p)


# ================================================================ UiWAutoDsach.h
H = r'''//---------------------------------------------------------------------------
// [WAUTO 12/09] BANG PHU cua khung WAuto trong game: sua 6 danh sach cua WAuto va thay MENU cho cac hop chon DAI.
//
// Ban PC de bon o danh sach ngay trong the (toa do di tuan, loc thuoc tinh, moi/vao nhom, ban dien Lien dau) va hai
// hop thoai con (IDD_NOPICK_DIALOG "khong nhat theo ten", IDD_SETSERIES_DIALOG "thu tu ngu hanh"). Tren dien thoai
// mot o danh sach 3 dong trong the 720x432 thi khong cham noi, nen ca sau cai dung CHUNG bang phu nay: the chi con
// mot dong tom tat + nut "Sua".
//
// Bang phu cung thay KPopupMenu cho hop chon dai: KPopupMenu khong cuon (PopupMenu.cpp:171 chieu cao = so dong x cao
// dong ~26 px) nen tren man 604 px, tu dong 24 tro di la cham khong toi - danh sach chieu co the toi 72 dong
// (defSKILLNUMGET) va hop "Di Xa Phu" co 31 dong.
//
// Chi bien dich khi JX_MOBILE; khong nam trong vcxproj nao (them thang vao target main cua android/CMakeLists.txt).
//---------------------------------------------------------------------------
#ifndef UiWAutoDsach_H
#define UiWAutoDsach_H
#ifdef JX_MOBILE
#include "../Elem/WndWindow.h"
#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndText.h"
#include "UiWAutoTrang.h"	// KWndNhapWA
#include "../../../Core/Src/ipc_shared.h"

#define WA_DS_DONG		22		// cao mot dong danh sach (px) - du cho ngon tay
#define WA_DS_TOI_DA	100		// so dong toi da giu trong o (nguoi quanh day toi 100; szNOPName 60; chieu 72)
#define WA_DS_CHU		64		// do dai moi dong
#define WA_DS_NUT		6		// Them / Len / Xuong / Xoa / Xoa het / Lay o day

// O danh sach cham duoc: ve nen toi + tung dong, dong dang chon co vach sang. Cuon bang KUiWAutoDsach (hai nut).
class KWndDsachWA : public KWndWindow
{
public:
	KWndDsachWA();
	virtual void	PaintWindow();
	int		WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam);
	void	XoaHet();
	int		Them(const char* sz);				// -1 = day
	int		So() const			{ return m_nSo; }
	int		Chon() const		{ return m_nChon; }
	void	DatChon(int n);
	void	Cuon(int nBuoc);					// +1 / -1 trang
	int		SoDongHien() const;
	int		Dau() const			{ return m_nDau; }
	const char*	Dong(int i) const;
private:
	char	m_szDong[WA_DS_TOI_DA][WA_DS_CHU];
	int		m_nSo;
	int		m_nChon;
	int		m_nDau;								// dong dau dang hien (cuon)
};

class KUiWAutoDsach : public KWndWindow
{
public:
	KUiWAutoDsach();
	void	KhoiTao(KWndWindow* pTrang);		// AddChild kho widget; pTrang = trang noi dung (an di khi bang phu mo)
	void	NapBoCuc();							// doc uiwauto_dsach.ini (goi sau khi cha da co scheme)
	// che do 0: sua mot trong sau danh sach (nViec = WA_V_DS_*)
	static int	MoSua(int nViec);
	// che do 2: chon mot muc trong danh sach DAI; chon xong bao ve pGoi bang WND_M_MENUITEM_SELECTED (nhu KPopupMenu)
	static int	MoChonMuc(KWndWindow* pGoi, int nKhe, const char* szTieuDe, const char* const* pDong, int nDong, int nChon);
	static int	DangMo();
	static void	DongLai();
	virtual void	PaintWindow();
	int		WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam);
private:
	static KUiWAutoDsach*	s_pSelf;
	void	Mo(int nCheDo);
	void	Dong();
	void	NapDanhSach();						// che do 0: dien lai o danh sach tu autoData
	void	NapNguon();							// che do 1: dien nguon (thuoc tinh / ten vat pham / ten quanh day)
	void	CapNhatNut();
	void	ChonNguon(int nDong);
	void	Them();
	void	Doi(int nBuoc);						// doi cho dong dang chon len / xuong
	autoData*	CauHinh();
	KWndText80			m_TieuDe;
	KWndText80			m_Huong;
	KWndText80			m_NhanSo;
	KWndDsachWA			m_Dsach;
	KWndLabeledButton	m_Cuon[2];
	KWndLabeledButton	m_Nguon;
	KWndNhapWA			m_So;
	KWndLabeledButton	m_Nut[WA_DS_NUT];
	KWndLabeledButton	m_Dong;
	KWndWindow*			m_pTrang;
	int					m_nCheDo;				// 0 sua danh sach / 1 chon nguon / 2 chon mot muc
	int					m_nViec;				// WA_V_DS_* (che do 0 / 1)
	int					m_nNguonChon;			// chi so dong nguon dang chon (-1 = chua)
	char				m_szNguon[WA_DS_CHU];	// chu cua dong nguon dang chon
	KWndWindow*			m_pGoi;					// che do 2: ai goi
	int					m_nKhe;					// che do 2: khe hop chon cua ben goi
	const char* const*	m_pDong;				// che do 2: cac dong
	int					m_nDong;
};
#endif // JX_MOBILE
#endif
'''

# ================================================================ UiWAutoDsach.cpp
CPP = r'''//---------------------------------------------------------------------------
// [WAUTO 12/09] Bang phu cua khung WAuto - xem UiWAutoDsach.h. Chuoi tieng Viet: TCVN3 (sinh bang vn_edit.py trong
// android/va_nguon_android_wauto21.py, khong sua bang trinh soan thao thuong).
//---------------------------------------------------------------------------
#include "KWin32.h"
#ifdef JX_MOBILE
#include "KIniFile.h"
#include "KDebug.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iRepresentShell*	g_pRepresentShell;
#include "UiWAutoDsach.h"
#include "UiWAutoBang.h"
#include "../../Platform/JxWAutoNoiBo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" int JxCore_WAutoViTri(int* pnMapId, char* szMap, int nMapMax, int* pnX, int* pnY);
extern "C" int JxCore_WAutoTenQuanhDay(char* pOut, int nMax);
extern "C" int JxCore_WAutoTenVatPham(char* pOut, int nMax);

#define WA_DS_SCHEME	"UiWAuto_Dsach.ini"

// ---------------------------------------------------------------- ve nen (nhu UiWAutoTrang.cpp; de static cho khoi trung ten tren ELF)
static void WAD_Bong(int x0, int y0, int x1, int y1, int r, int g, int b, int a)
{
	KRUShadow S;
	S.oPosition.nX = x0;
	S.oPosition.nY = y0;
	S.oPosition.nZ = 0;
	S.oEndPos.nX = x1;
	S.oEndPos.nY = y1;
	S.oEndPos.nZ = 0;
	S.Color.Color_b.r = (unsigned char)r;
	S.Color.Color_b.g = (unsigned char)g;
	S.Color.Color_b.b = (unsigned char)b;
	S.Color.Color_b.a = (unsigned char)a;
	g_pRepresentShell->DrawPrimitives(1, &S, RU_T_SHADOW, true);
}

static void WAD_Vien(int x0, int y0, int x1, int y1, int r, int g, int b)
{
	KRURect R;
	R.oPosition.nX = x0;
	R.oPosition.nY = y0;
	R.oPosition.nZ = 0;
	R.oEndPos.nX = x1;
	R.oEndPos.nY = y1;
	R.oEndPos.nZ = 0;
	R.Color.Color_b.r = (unsigned char)r;
	R.Color.Color_b.g = (unsigned char)g;
	R.Color.Color_b.b = (unsigned char)b;
	R.Color.Color_b.a = 255;
	g_pRepresentShell->DrawPrimitives(1, &R, RU_T_RECT, true);
}

// ---------------------------------------------------------------- o danh sach
KWndDsachWA::KWndDsachWA()
{
	m_nSo = 0;
	m_nChon = -1;
	m_nDau = 0;
	m_szDong[0][0] = 0;
}

int KWndDsachWA::SoDongHien() const
{
	int n = m_Height / WA_DS_DONG;
	return n > 0 ? n : 1;
}

const char* KWndDsachWA::Dong(int i) const
{
	if (i < 0 || i >= m_nSo)
		return "";
	return m_szDong[i];
}

void KWndDsachWA::XoaHet()
{
	m_nSo = 0;
	m_nChon = -1;
	m_nDau = 0;
}

int KWndDsachWA::Them(const char* sz)
{
	if (m_nSo >= WA_DS_TOI_DA)
		return -1;
	strncpy(m_szDong[m_nSo], sz ? sz : "", WA_DS_CHU - 1);
	m_szDong[m_nSo][WA_DS_CHU - 1] = 0;
	return m_nSo++;
}

void KWndDsachWA::DatChon(int n)
{
	m_nChon = (n >= 0 && n < m_nSo) ? n : -1;
	if (m_nChon >= 0)
	{
		int nHien = SoDongHien();
		if (m_nChon < m_nDau)
			m_nDau = m_nChon;
		else if (m_nChon >= m_nDau + nHien)
			m_nDau = m_nChon - nHien + 1;
	}
}

void KWndDsachWA::Cuon(int nBuoc)
{
	int nHien = SoDongHien();
	m_nDau += nBuoc * nHien;
	if (m_nDau > m_nSo - nHien)
		m_nDau = m_nSo - nHien;
	if (m_nDau < 0)
		m_nDau = 0;
}

void KWndDsachWA::PaintWindow()
{
	if (!g_pRepresentShell || !(m_Style & WND_S_VISIBLE))
		return;
	int x0 = m_nAbsoluteLeft, y0 = m_nAbsoluteTop, x1 = x0 + m_Width, y1 = y0 + m_Height;
	WAD_Bong(x0, y0, x1, y1, 10, 12, 16, 4);
	WAD_Vien(x0, y0, x1 - 1, y1 - 1, 150, 120, 70);
	int nHien = SoDongHien();
	for (int i = 0; i < nHien; i++)
	{
		int k = m_nDau + i;
		if (k >= m_nSo)
			break;
		int yd = y0 + i * WA_DS_DONG;
		if (k == m_nChon)
		{
			WAD_Bong(x0 + 1, yd + 1, x1 - 1, yd + WA_DS_DONG - 1, 90, 70, 20, 6);
			WAD_Vien(x0 + 1, yd + 1, x1 - 2, yd + WA_DS_DONG - 2, 200, 170, 90);
		}
		int n = (int)strlen(m_szDong[k]);
		int nMax = (m_Width - 12) / 6;
		if (n > nMax)
			n = nMax;
		g_pRepresentShell->OutputText(12, m_szDong[k], n, x0 + 6, yd + 4, (k == m_nChon) ? 0x00FFF0A0 : 0x00E0E0E0,
			0, TEXT_IN_SINGLE_PLANE_COORD, 0x00000000);
	}
	if (m_nSo > nHien)
	{	// vach bao con dong o tren / duoi
		char sz[32];
		snprintf(sz, sizeof(sz), "%d-%d/%d", m_nDau + 1, (m_nDau + nHien < m_nSo) ? m_nDau + nHien : m_nSo, m_nSo);
		g_pRepresentShell->OutputText(12, sz, (int)strlen(sz), x1 - 60, y1 - 15, 0x00A0A0A0, 0, TEXT_IN_SINGLE_PLANE_COORD, 0x00000000);
	}
}

int KWndDsachWA::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	if (uMsg == WM_LBUTTONDOWN)
	{
		int y = (int)(short)HIWORD(nParam);
		int i = (y - m_nAbsoluteTop) / WA_DS_DONG;
		int k = m_nDau + i;
		if (i >= 0 && k < m_nSo)
		{
			m_nChon = k;
			if (m_pParentWnd)
				m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL, (KUPARAM)(KWndWindow*)this, k);
		}
		return 1;
	}
	return KWndWindow::WndProc(uMsg, uParam, nParam);
}

// ---------------------------------------------------------------- cau hinh tung danh sach
// bNguon: 0 khong co / 1 bang thuoc tinh / 2 ten vat pham trong hanh trang / 3 ten nguoi choi quanh day
struct WADsCau
{
	int			nViec;
	const char*	szTieuDe;
	int			nToiDa;
	int			bNguon;
	int			bSo;		// co o nhap so di kem (loc thuoc tinh)
	int			bThuTu;		// co nut Len / Xuong
	int			bLay;		// co nut "Lay o day" (toa do dang dung)
	int			bXoa;		// co nut Xoa / Xoa het
	const char*	szHuong;
};

static const WADsCau s_aDsCau[] = {
	{ WA_V_DS_TOADO,     "@@Tọa độ đi tuần@@",           24, 0, 0, 1, 1, 1,
	  "@@Auto chạy lần lượt từ trên xuống rồi lặp lại. Đứng đúng chỗ muốn đi qua rồi bấm Lấy ở đây.@@" },
	{ WA_V_DS_LOC,       "@@Lọc thuộc tính khi nhặt@@",  40, 1, 1, 0, 0, 1,
	  "@@Chỉ nhặt trang bị có dòng thuộc tính này và đạt giá trị tối thiểu. Chọn dòng, gõ giá trị rồi bấm Thêm.@@" },
	{ WA_V_DS_KHONGNHAT, "@@Không nhặt theo tên@@",      60, 2, 0, 0, 0, 1,
	  "@@Danh sách lấy từ các món đang có trong hành trang. Tên trong danh sách thì auto bỏ qua không nhặt.@@" },
	{ WA_V_DS_TODOI,     "@@Mời / vào nhóm theo tên@@",  24, 3, 0, 1, 0, 1,
	  "@@Chỉ mời và chỉ nhận lời mời của các tên trong danh sách. Bấm Chọn tên để lấy người đang đứng quanh.@@" },
	{ WA_V_DS_LIENDAU,   "@@Bạn diễn Liên đấu@@",         8, 3, 0, 0, 0, 1,
	  "@@Tối đa 8 tên, auto chỉ tổ đội với người gặp đầu tiên. Cả hai máy đều phải có tên của nhau.@@" },
	{ WA_V_DS_NGUHANH,   "@@Thứ tự ưu tiên ngũ hành@@",   5, 0, 0, 1, 0, 0,
	  "@@Máy PK chọn mục tiêu theo thứ tự hệ này, từ trên xuống. Chỉ đổi được thứ tự, không thêm bớt.@@" },
};
#define WA_DS_SO_CAU	(sizeof(s_aDsCau) / sizeof(s_aDsCau[0]))

static const WADsCau* WA_DsCau(int nViec)
{
	for (int i = 0; i < (int)WA_DS_SO_CAU; i++)
		if (s_aDsCau[i].nViec == nViec)
			return &s_aDsCau[i];
	return NULL;
}

static const char* const s_aNguHanh[5] = { "@@Kim@@", "@@Mộc@@", "@@Thủy@@", "@@Hỏa@@", "@@Thổ@@" };

static const char* WA_TenMagic(int nId)
{
	for (int i = 0; i < WA_SO_MAGIC; i++)
		if (s_WAMagic[i].nId == nId)
			return s_WAMagic[i].szTen;
	return "?";
}

// ---------------------------------------------------------------- doc / sua du lieu tung danh sach
static int WA_DsSo(int nViec, autoData* p)
{
	if (!p)
		return 0;
	switch (nViec)
	{
	case WA_V_DS_TOADO:		return p->nCoordCount;
	case WA_V_DS_LOC:		return p->nFtMaCount;
	case WA_V_DS_KHONGNHAT:	return p->nNOPCount;
	case WA_V_DS_TODOI:		return p->nIJPtCount;
	case WA_V_DS_LIENDAU:	return p->nLDPtCount;
	case WA_V_DS_NGUHANH:	return 5;
	}
	return 0;
}

static void WA_DsDong(int nViec, autoData* p, int i, char* sz, int nMax)
{
	sz[0] = 0;
	if (!p)
		return;
	switch (nViec)
	{
	case WA_V_DS_TOADO:
		snprintf(sz, nMax, "%d. %d / %d", i + 1, p->sMoveCoord[i].x, p->sMoveCoord[i].y);
		break;
	case WA_V_DS_LOC:
		snprintf(sz, nMax, "%s >= %d", WA_TenMagic(p->nFtMagic[i][0]), p->nFtMagic[i][1]);
		break;
	case WA_V_DS_KHONGNHAT:
		snprintf(sz, nMax, "%s", p->szNOPName[i]);
		break;
	case WA_V_DS_TODOI:
		snprintf(sz, nMax, "%s", p->szIJPtName[i]);
		break;
	case WA_V_DS_LIENDAU:
		snprintf(sz, nMax, "%s", p->szLDPtName[i]);
		break;
	case WA_V_DS_NGUHANH:
	{
		int h = p->nSerPy[i];
		snprintf(sz, nMax, "%d. %s", i + 1, (h >= 0 && h < 5) ? s_aNguHanh[h] : "?");
		break;
	}
	}
}

// tra 1 neu da doi du lieu
static int WA_DsThem(int nViec, autoData* p, int nNguon, const char* szTen, int nSo)
{
	int i;
	if (!p)
		return 0;
	switch (nViec)
	{
	case WA_V_DS_TOADO:
	{
		int nMapId = 0, x = 0, y = 0;
		char szMap[32] = { 0 };
		if (p->nCoordCount >= 24 || !JxCore_WAutoViTri(&nMapId, szMap, sizeof(szMap), &x, &y))
			return 0;
		p->sMoveCoord[p->nCoordCount].x = x;
		p->sMoveCoord[p->nCoordCount].y = y;
		p->nCoordCount++;
		p->nMoveMapId = nMapId;		// nhu nut "Lay" cua ban PC: toa do di kem ban do dang dung
		strncpy(p->szMoveMap, szMap, sizeof(p->szMoveMap) - 1);
		p->szMoveMap[sizeof(p->szMoveMap) - 1] = 0;
		return 1;
	}
	case WA_V_DS_LOC:
	{
		if (nNguon < 0 || nNguon >= WA_SO_MAGIC)
			return 0;
		int nId = s_WAMagic[nNguon].nId;
		for (i = 0; i < p->nFtMaCount; i++)
			if (p->nFtMagic[i][0] == nId)
			{
				p->nFtMagic[i][1] = nSo;		// da co dong nay -> chi doi gia tri (giong CBN_SELCHANGE ben PC)
				return 1;
			}
		if (p->nFtMaCount >= 40)
			return 0;
		p->nFtMagic[p->nFtMaCount][0] = nId;
		p->nFtMagic[p->nFtMaCount][1] = nSo;
		p->nFtMaCount++;
		return 1;
	}
	case WA_V_DS_KHONGNHAT:
		if (!szTen || !szTen[0] || p->nNOPCount >= 60)
			return 0;
		for (i = 0; i < p->nNOPCount; i++)
			if (!strcmp(p->szNOPName[i], szTen))
				return 0;
		strncpy(p->szNOPName[p->nNOPCount], szTen, 79);
		p->szNOPName[p->nNOPCount][79] = 0;
		p->nNOPCount++;
		return 1;
	case WA_V_DS_TODOI:
		if (!szTen || !szTen[0] || p->nIJPtCount >= 24)
			return 0;
		for (i = 0; i < p->nIJPtCount; i++)
			if (!strcmp(p->szIJPtName[i], szTen))
				return 0;
		strncpy(p->szIJPtName[p->nIJPtCount], szTen, 31);
		p->szIJPtName[p->nIJPtCount][31] = 0;
		p->nIJPtCount++;
		return 1;
	case WA_V_DS_LIENDAU:
		if (!szTen || !szTen[0] || p->nLDPtCount >= 8)
			return 0;
		for (i = 0; i < p->nLDPtCount; i++)
			if (!strcmp(p->szLDPtName[i], szTen))
				return 0;
		strncpy(p->szLDPtName[p->nLDPtCount], szTen, 31);
		p->szLDPtName[p->nLDPtCount][31] = 0;
		p->nLDPtCount++;
		return 1;
	}
	return 0;
}

static int WA_DsXoa(int nViec, autoData* p, int k)
{
	int i;
	if (!p || k < 0 || k >= WA_DsSo(nViec, p))
		return 0;
	switch (nViec)
	{
	case WA_V_DS_TOADO:
		for (i = k; i < p->nCoordCount - 1; i++)
			p->sMoveCoord[i] = p->sMoveCoord[i + 1];
		p->nCoordCount--;
		return 1;
	case WA_V_DS_LOC:
		for (i = k; i < p->nFtMaCount - 1; i++)
		{
			p->nFtMagic[i][0] = p->nFtMagic[i + 1][0];
			p->nFtMagic[i][1] = p->nFtMagic[i + 1][1];
		}
		p->nFtMaCount--;
		return 1;
	case WA_V_DS_KHONGNHAT:
		for (i = k; i < p->nNOPCount - 1; i++)
			memcpy(p->szNOPName[i], p->szNOPName[i + 1], 80);
		p->nNOPCount--;
		return 1;
	case WA_V_DS_TODOI:
		for (i = k; i < p->nIJPtCount - 1; i++)
			memcpy(p->szIJPtName[i], p->szIJPtName[i + 1], 32);
		p->nIJPtCount--;
		return 1;
	case WA_V_DS_LIENDAU:
		for (i = k; i < p->nLDPtCount - 1; i++)
			memcpy(p->szLDPtName[i], p->szLDPtName[i + 1], 32);
		p->nLDPtCount--;
		return 1;
	}
	return 0;
}

static int WA_DsXoaHet(int nViec, autoData* p)
{
	if (!p)
		return 0;
	switch (nViec)
	{
	case WA_V_DS_TOADO:		p->nCoordCount = 0; return 1;
	case WA_V_DS_LOC:		p->nFtMaCount = 0;  return 1;
	case WA_V_DS_KHONGNHAT:	p->nNOPCount = 0;   return 1;
	case WA_V_DS_TODOI:		p->nIJPtCount = 0;  return 1;
	case WA_V_DS_LIENDAU:	p->nLDPtCount = 0;  return 1;
	}
	return 0;
}

static int WA_DsHoanDoi(int nViec, autoData* p, int a, int b)
{
	int n = WA_DsSo(nViec, p);
	if (!p || a < 0 || b < 0 || a >= n || b >= n)
		return 0;
	switch (nViec)
	{
	case WA_V_DS_TOADO:
	{
		autoCoord t = p->sMoveCoord[a];
		p->sMoveCoord[a] = p->sMoveCoord[b];
		p->sMoveCoord[b] = t;
		return 1;
	}
	case WA_V_DS_TODOI:
	{
		char t[32];
		memcpy(t, p->szIJPtName[a], 32);
		memcpy(p->szIJPtName[a], p->szIJPtName[b], 32);
		memcpy(p->szIJPtName[b], t, 32);
		return 1;
	}
	case WA_V_DS_NGUHANH:
	{
		short t = p->nSerPy[a];
		p->nSerPy[a] = p->nSerPy[b];
		p->nSerPy[b] = t;
		return 1;
	}
	}
	return 0;
}

// ---------------------------------------------------------------- bang phu
KUiWAutoDsach* KUiWAutoDsach::s_pSelf = NULL;

KUiWAutoDsach::KUiWAutoDsach()
{
	m_pTrang = NULL;
	m_nCheDo = 0;
	m_nViec = 0;
	m_nNguonChon = -1;
	m_szNguon[0] = 0;
	m_pGoi = NULL;
	m_nKhe = 0;
	m_pDong = NULL;
	m_nDong = 0;
}

void KUiWAutoDsach::KhoiTao(KWndWindow* pTrang)
{
	int i;
	m_pTrang = pTrang;
	AddChild(&m_TieuDe);
	AddChild(&m_Huong);
	AddChild(&m_NhanSo);
	AddChild(&m_Dsach);
	for (i = 0; i < 2; i++)
		AddChild(&m_Cuon[i]);
	AddChild(&m_Nguon);
	AddChild(&m_So);
	for (i = 0; i < WA_DS_NUT; i++)
		AddChild(&m_Nut[i]);
	AddChild(&m_Dong);
	s_pSelf = this;
	Hide();
}

void KUiWAutoDsach::NapBoCuc()
{
	char Scheme[128];
	char Buff[256];
	char szMuc[32];
	KIniFile Ini;
	int i;
	g_UiBase.GetCurSchemePath(Scheme, 128);
	sprintf(Buff, "%s\\%s", Scheme, WA_DS_SCHEME);
	if (!Ini.Load(Buff))
	{
		g_DebugLog("[WAUTO-DS] khong doc duoc bo cuc %s", Buff);
		return;
	}
	Init(&Ini, "Bang");
	m_TieuDe.SetText("");
	m_TieuDe.Init(&Ini, "TieuDe");
	m_Huong.SetText("");
	m_Huong.Init(&Ini, "Huong");
	m_NhanSo.SetText("");
	m_NhanSo.Init(&Ini, "NhanSo");
	m_Dsach.Init(&Ini, "Dsach");
	for (i = 0; i < 2; i++)
	{
		sprintf(szMuc, "Cuon%d", i);
		m_Cuon[i].Init(&Ini, szMuc);
	}
	m_Nguon.Init(&Ini, "Nguon");
	m_So.Init(&Ini, "So");
	for (i = 0; i < WA_DS_NUT; i++)
	{
		sprintf(szMuc, "Nut%d", i);
		m_Nut[i].Init(&Ini, szMuc);
	}
	m_Dong.Init(&Ini, "Dong");
	Hide();
}

autoData* KUiWAutoDsach::CauHinh()
{
	autoData* p = JxWAuto_CauHinh();
	if (!p || sizeof(autoData) != WA_SIZEOF_AUTODATA)
		return NULL;
	return p;
}

int KUiWAutoDsach::DangMo()
{
	return (s_pSelf && s_pSelf->IsVisible()) ? 1 : 0;
}

void KUiWAutoDsach::DongLai()
{
	if (s_pSelf)
		s_pSelf->Dong();
}

int KUiWAutoDsach::MoSua(int nViec)
{
	if (!s_pSelf || !WA_DsCau(nViec))
		return 0;
	s_pSelf->m_nViec = nViec;
	s_pSelf->m_nNguonChon = -1;
	s_pSelf->m_szNguon[0] = 0;
	s_pSelf->Mo(0);
	return 1;
}

int KUiWAutoDsach::MoChonMuc(KWndWindow* pGoi, int nKhe, const char* szTieuDe, const char* const* pDong, int nDong, int nChon)
{
	if (!s_pSelf || !pDong || nDong <= 0)
		return 0;
	s_pSelf->m_pGoi = pGoi;
	s_pSelf->m_nKhe = nKhe;
	s_pSelf->m_pDong = pDong;
	s_pSelf->m_nDong = nDong;
	s_pSelf->m_TieuDe.SetText(szTieuDe ? szTieuDe : "");
	s_pSelf->m_Dsach.XoaHet();
	for (int i = 0; i < nDong && i < WA_DS_TOI_DA; i++)
		s_pSelf->m_Dsach.Them(pDong[i]);
	s_pSelf->m_Dsach.DatChon(nChon);
	s_pSelf->Mo(2);
	return 1;
}

void KUiWAutoDsach::Mo(int nCheDo)
{
	m_nCheDo = nCheDo;
	if (nCheDo == 0)
	{
		const WADsCau* c = WA_DsCau(m_nViec);
		m_TieuDe.SetText(c ? c->szTieuDe : "");
		m_Huong.SetText(c ? c->szHuong : "");
		NapDanhSach();
	}
	else if (nCheDo == 1)
	{
		NapNguon();
	}
	else
	{
		m_Huong.SetText("@@Chạm một dòng để chọn.@@");
	}
	CapNhatNut();
	if (m_pTrang)
		m_pTrang->Hide();		// trang khong nhan cham nua (PtInWindow doi WND_S_VISIBLE)
	Show();
}

void KUiWAutoDsach::Dong()
{
	if (m_nCheDo == 1)
	{	// dang chon nguon -> ve lai che do sua danh sach
		Mo(0);
		return;
	}
	Hide();
	m_pGoi = NULL;
	m_pDong = NULL;
	m_nDong = 0;
	if (m_pTrang)
		m_pTrang->Show();
}

void KUiWAutoDsach::NapDanhSach()
{
	char sz[WA_DS_CHU];
	autoData* p = CauHinh();
	int n = WA_DsSo(m_nViec, p);
	int nCu = m_Dsach.Chon();
	m_Dsach.XoaHet();
	for (int i = 0; i < n && i < WA_DS_TOI_DA; i++)
	{
		WA_DsDong(m_nViec, p, i, sz, sizeof(sz));
		m_Dsach.Them(sz);
	}
	if (nCu >= 0)
		m_Dsach.DatChon(nCu < n ? nCu : n - 1);
}

void KUiWAutoDsach::NapNguon()
{
	const WADsCau* c = WA_DsCau(m_nViec);
	int i, n;
	m_Dsach.XoaHet();
	if (!c)
		return;
	if (c->bNguon == 1)
	{
		m_TieuDe.SetText("@@Chọn dòng thuộc tính@@");
		for (i = 0; i < WA_SO_MAGIC; i++)
			m_Dsach.Them(s_WAMagic[i].szTen);
	}
	else if (c->bNguon == 2)
	{
		static char s_szTen[60][80];
		m_TieuDe.SetText("@@Chọn món trong hành trang@@");
		n = JxCore_WAutoTenVatPham((char*)s_szTen, 60);
		for (i = 0; i < n; i++)
			m_Dsach.Them(s_szTen[i]);
		if (n <= 0)
			m_Huong.SetText("@@Hành trang đang trống - bỏ vào túi một món rồi mở lại.@@");
	}
	else if (c->bNguon == 3)
	{
		static char s_szTen2[100][32];
		m_TieuDe.SetText("@@Chọn tên người chơi quanh đây@@");
		n = JxCore_WAutoTenQuanhDay((char*)s_szTen2, 100);
		for (i = 0; i < n; i++)
			m_Dsach.Them(s_szTen2[i]);
		if (n <= 0)
			m_Huong.SetText("@@Quanh đây chưa thấy người chơi nào.@@");
	}
}

void KUiWAutoDsach::CapNhatNut()
{
	const WADsCau* c = WA_DsCau(m_nViec);
	int bSua = (m_nCheDo == 0 && c != NULL);
	int i;
	for (i = 0; i < WA_DS_NUT; i++)
		m_Nut[i].Hide();
	m_Nguon.Hide();
	m_So.Hide();
	m_NhanSo.Hide();
	if (!bSua)
		return;
	if (c->bNguon)
	{
		m_Nguon.SetLabel(m_szNguon[0] ? m_szNguon : "@@(chọn)@@");
		m_Nguon.Show();
		m_Nut[0].Show();			// Them
	}
	if (c->bSo)
	{
		m_So.Show();
		m_NhanSo.Show();
	}
	if (c->bThuTu)
	{
		m_Nut[1].Show();			// Len
		m_Nut[2].Show();			// Xuong
	}
	if (c->bXoa)
	{
		m_Nut[3].Show();			// Xoa
		m_Nut[4].Show();			// Xoa het
	}
	if (c->bLay)
		m_Nut[5].Show();			// Lay o day
}

void KUiWAutoDsach::ChonNguon(int nDong)
{
	const char* s = m_Dsach.Dong(nDong);
	m_nNguonChon = nDong;
	strncpy(m_szNguon, s ? s : "", WA_DS_CHU - 1);
	m_szNguon[WA_DS_CHU - 1] = 0;
	Mo(0);							// ve che do sua danh sach
}

void KUiWAutoDsach::Them()
{
	const WADsCau* c = WA_DsCau(m_nViec);
	autoData* p = CauHinh();
	if (!c || !p)
		return;
	if (c->bNguon && m_nNguonChon < 0)
	{
		m_Huong.SetText("@@Chọn một dòng ở ô bên dưới trước đã.@@");
		return;
	}
	if (WA_DsThem(m_nViec, p, m_nNguonChon, m_szNguon, c->bSo ? m_So.GetIntNumber() : 0))
	{
		JxWAuto_LuuCauHinh();
		NapDanhSach();
	}
	else
		m_Huong.SetText("@@Không thêm được: đã đủ số dòng tối đa hoặc tên đã có trong danh sách.@@");
}

void KUiWAutoDsach::Doi(int nBuoc)
{
	autoData* p = CauHinh();
	int k = m_Dsach.Chon();
	if (!p || k < 0)
		return;
	if (WA_DsHoanDoi(m_nViec, p, k, k + nBuoc))
	{
		JxWAuto_LuuCauHinh();
		NapDanhSach();
		m_Dsach.DatChon(k + nBuoc);
	}
}

void KUiWAutoDsach::PaintWindow()
{
	if (!g_pRepresentShell || !(m_Style & WND_S_VISIBLE))
		return;
	int x0 = m_nAbsoluteLeft, y0 = m_nAbsoluteTop, x1 = x0 + m_Width, y1 = y0 + m_Height;
	WAD_Bong(x0, y0, x1, y1, 6, 8, 12, 1);			// gan nhu dac: che han trang phia duoi
	WAD_Bong(x0, y0, x1, y0 + 3, 150, 110, 60, 5);
	WAD_Vien(x0, y0, x1 - 1, y1 - 1, 190, 160, 90);
}

int KUiWAutoDsach::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	int i;
	if (uMsg == WND_N_BUTTON_CLICK)
	{
		if (uParam == (KUPARAM)(KWndWindow*)&m_Dong)
		{
			Dong();
			return 1;
		}
		for (i = 0; i < 2; i++)
			if (uParam == (KUPARAM)(KWndWindow*)&m_Cuon[i])
			{
				m_Dsach.Cuon(i ? 1 : -1);
				return 1;
			}
		if (uParam == (KUPARAM)(KWndWindow*)&m_Nguon)
		{
			Mo(1);
			return 1;
		}
		for (i = 0; i < WA_DS_NUT; i++)
			if (uParam == (KUPARAM)(KWndWindow*)&m_Nut[i])
			{
				autoData* p = CauHinh();
				switch (i)
				{
				case 0:		// Them (tu nguon dang chon)
					Them();
					break;
				case 1:		// Len
					Doi(-1);
					break;
				case 2:		// Xuong
					Doi(1);
					break;
				case 3:		// Xoa
					if (p && WA_DsXoa(m_nViec, p, m_Dsach.Chon()))
					{
						JxWAuto_LuuCauHinh();
						NapDanhSach();
					}
					break;
				case 4:		// Xoa het
					if (p && WA_DsXoaHet(m_nViec, p))
					{
						JxWAuto_LuuCauHinh();
						NapDanhSach();
					}
					break;
				case 5:		// Lay o day (toa do dang dung)
					if (p && WA_DsThem(m_nViec, p, -1, NULL, 0))
					{
						JxWAuto_LuuCauHinh();
						NapDanhSach();
					}
					else
						m_Huong.SetText("@@Không lấy được: đã đủ 24 tọa độ, hoặc chưa vào game.@@");
					break;
				}
				return 1;
			}
	}
	else if (uMsg == WND_N_LIST_ITEM_SEL)
	{
		if (uParam == (KUPARAM)(KWndWindow*)&m_Dsach)
		{
			if (m_nCheDo == 1)
				ChonNguon((int)nParam);
			else if (m_nCheDo == 2)
			{
				KWndWindow* pGoi = m_pGoi;
				int nKhe = m_nKhe;
				int nMuc = (int)nParam;
				Dong();
				if (pGoi)
					pGoi->WndProc(WND_M_MENUITEM_SELECTED, (KUPARAM)pGoi, MAKELONG(nMuc, nKhe));
			}
			return 1;
		}
	}
	return KWndWindow::WndProc(uMsg, uParam, nParam);
}
#endif // JX_MOBILE
'''

# ================================================================ bo cuc bang phu
INI = r'''; [WAUTO 12/09] Bo cuc BANG PHU cua khung WAuto (KUiWAutoDsach, UiWAutoDsach.cpp): sua 6 danh sach cua WAuto
; va thay menu cho cac hop chon dai. Bang phu phu len vung noi dung cua khung 720x432 (y 88..384).
; Chu thich chi o dong rieng (KIniFile khong cat chu thich sau gia tri).
[Bang]
Left=16
Top=88
Width=688
Height=296

[TieuDe]
Left=12
Top=6
Width=470
Height=20
Font=14
Color=255,236,170
BorderColor=0,0,0
HAlign=0
Text=

[Dong]
Left=614
Top=5
Width=60
Height=22
Trans=0
Image=\spr\uinew\uiautonew\nut_do_60.spr
Up=0
Down=1
LabelYOffset=4
Font=12
Color=230,230,230
OverColor=255,255,160
SelectColor=255,255,0
DisableColor=140,120,120
Label=@@Đóng@@

[Dsach]
Left=12
Top=32
Width=560
Height=176

[Cuon0]
Left=582
Top=40
Width=84
Height=22
Trans=0
Image=\spr\uinew\uiautonew\nut_do_84.spr
Up=0
Down=1
LabelYOffset=4
Font=12
Color=230,230,230
OverColor=255,255,160
SelectColor=255,255,0
DisableColor=140,120,120
Label=@@Cuộn lên@@

[Cuon1]
Left=582
Top=76
Width=84
Height=22
Trans=0
Image=\spr\uinew\uiautonew\nut_do_84.spr
Up=0
Down=1
LabelYOffset=4
Font=12
Color=230,230,230
OverColor=255,255,160
SelectColor=255,255,0
DisableColor=140,120,120
Label=@@Cuộn xuống@@

[Nguon]
Left=12
Top=214
Width=200
Height=24
Trans=0
Image=\spr\uinew\uiautonew\hop_chon_200.spr
Up=0
Down=1
LabelXOffset=-9
LabelYOffset=5
Font=12
Color=255,255,255
OverColor=255,255,160
SelectColor=255,255,0
DisableColor=140,120,120

[NhanSo]
Left=220
Top=218
Width=30
Height=16
Font=12
Color=255,252,178
BorderColor=0,0,0
HAlign=0
Text=>=

[So]
Left=252
Top=218
Width=60
Height=16
Font=12
Color=255,255,255
BorderColor=60,50,30
MaxLen=6
Type=1

[Nut0]
Left=12
Top=246
Width=60
Height=22
Trans=0
Image=\spr\uinew\uiautonew\nut_do_60.spr
Up=0
Down=1
LabelYOffset=4
Font=12
Color=230,230,230
OverColor=255,255,160
SelectColor=255,255,0
DisableColor=140,120,120
Label=@@Thêm@@

[Nut1]
Left=78
Top=246
Width=60
Height=22
Trans=0
Image=\spr\uinew\uiautonew\nut_do_60.spr
Up=0
Down=1
LabelYOffset=4
Font=12
Color=230,230,230
OverColor=255,255,160
SelectColor=255,255,0
DisableColor=140,120,120
Label=@@Lên@@

[Nut2]
Left=144
Top=246
Width=60
Height=22
Trans=0
Image=\spr\uinew\uiautonew\nut_do_60.spr
Up=0
Down=1
LabelYOffset=4
Font=12
Color=230,230,230
OverColor=255,255,160
SelectColor=255,255,0
DisableColor=140,120,120
Label=@@Xuống@@

[Nut3]
Left=210
Top=246
Width=60
Height=22
Trans=0
Image=\spr\uinew\uiautonew\nut_do_60.spr
Up=0
Down=1
LabelYOffset=4
Font=12
Color=230,230,230
OverColor=255,255,160
SelectColor=255,255,0
DisableColor=140,120,120
Label=@@Xóa@@

[Nut4]
Left=276
Top=246
Width=84
Height=22
Trans=0
Image=\spr\uinew\uiautonew\nut_do_84.spr
Up=0
Down=1
LabelYOffset=4
Font=12
Color=230,230,230
OverColor=255,255,160
SelectColor=255,255,0
DisableColor=140,120,120
Label=@@Xóa hết@@

[Nut5]
Left=366
Top=246
Width=120
Height=22
Trans=0
Image=\spr\uinew\uiautonew\nut_do_120.spr
Up=0
Down=1
LabelYOffset=4
Font=12
Color=230,230,230
OverColor=255,255,160
SelectColor=255,255,0
DisableColor=140,120,120
Label=@@Lấy ở đây@@

[Huong]
Left=12
Top=274
Width=660
Height=16
Font=12
Color=180,200,220
BorderColor=0,0,0
HAlign=0
Text=
'''


# ================================================================ va UiWAuto
def va_uiwauto_h(s):
    s = thay(s, [
        '#include "UiWAutoTrang.h"\t// [ANDROID 11/09 WAUTO B2] trang noi dung chay theo bang',
    ], [
        '#include "UiWAutoTrang.h"\t// [ANDROID 11/09 WAUTO B2] trang noi dung chay theo bang',
        '#include "UiWAutoDsach.h"\t// %s bang phu: sua danh sach + chon muc trong danh sach dai' % DAU,
    ], "UiWAuto.h: include bang phu")
    s = thay(s, [
        "\tKUiWAutoTrang\tm_Trang;\t\t// [ANDROID 11/09 WAUTO B2] trang noi dung (kho widget dung chung 15 tab)",
    ], [
        "\tKUiWAutoTrang\tm_Trang;\t\t// [ANDROID 11/09 WAUTO B2] trang noi dung (kho widget dung chung 15 tab)",
        "\tKUiWAutoDsach\tm_BangPhu;\t\t// %s ghep SAU CUNG -> ve sau cung va bat cham truoc (TopChildFromPoint di tu em ut)" % DAU,
    ], "UiWAuto.h: thanh vien bang phu")
    return s


def va_uiwauto_cpp(s):
    # ghep bang phu sau cung
    s = thay(s, [
        "\tm_Trang.KhoiTao();\t\t\t\t// [ANDROID 11/09 WAUTO B2] kho widget cua trang, roi moi ghep trang vao khung",
        "\tAddChild(&m_Trang);",
        "\tAddChild(&m_Dong);",
    ], [
        "\tm_Trang.KhoiTao();\t\t\t\t// [ANDROID 11/09 WAUTO B2] kho widget cua trang, roi moi ghep trang vao khung",
        "\tAddChild(&m_Trang);",
        "\tAddChild(&m_Dong);",
        "\tm_BangPhu.KhoiTao(&m_Trang);\t// %s ghep SAU CUNG: ve sau cung (Paint di theo thu tu) va bat cham truoc" % DAU,
        "\tAddChild(&m_BangPhu);",
    ], "UiWAuto.cpp: ghep bang phu")
    # nap bo cuc bang phu sau LoadScheme
    s = thay(s, [
        "\tchar Scheme[128];",
        "\tg_UiBase.GetCurSchemePath(Scheme, 128);",
        "\tLoadScheme(Scheme);",
    ], [
        "\tchar Scheme[128];",
        "\tg_UiBase.GetCurSchemePath(Scheme, 128);",
        "\tLoadScheme(Scheme);",
        "\tm_BangPhu.NapBoCuc();\t\t\t// %s bo cuc rieng (uiwauto_dsach.ini)" % DAU,
    ], "UiWAuto.cpp: nap bo cuc bang phu")
    return s


def va_uiwauto_cpp_b(s):
    # doi tab / dong khung thi dong bang phu (khong thi mo lai khung van thay bang phu, trang bi an)
    s = thay(s, [
        "void KUiWAuto::ChonTab(int nTab)",
        "{",
        "	char sz[80];",
        "	if (nTab < 0 || nTab >= s_aNhom[m_nNhom].nSo)",
        "		return;",
    ], [
        "void KUiWAuto::ChonTab(int nTab)",
        "{",
        "	char sz[80];",
        "	if (nTab < 0 || nTab >= s_aNhom[m_nNhom].nSo)",
        "		return;",
        "	KUiWAutoDsach::DongLai();		// %s doi tab thi dong bang phu (dang sua danh sach cua tab cu)" % DAU,
    ], "UiWAuto.cpp: ChonTab dong bang phu")
    s = thay(s, [
        "void KUiWAuto::CloseWindow()",
        "{",
        "	if (m_pSelf)",
    ], [
        "void KUiWAuto::CloseWindow()",
        "{",
        "	KUiWAutoDsach::DongLai();		// %s dong khung khi dang mo bang phu -> tra trang lai truoc" % DAU,
        "	if (m_pSelf)",
    ], "UiWAuto.cpp: CloseWindow dong bang phu")
    return s


def va_cmake(s):
    return thay(s, [
        "    ${JX_SRC}/S3Client/Ui/UiCase/UiWAutoTrang.cpp      # [ANDROID 11/09 WAUTO B2] trang noi dung chay theo bang (UiWAutoBang.h sinh bang may)",
    ], [
        "    ${JX_SRC}/S3Client/Ui/UiCase/UiWAutoTrang.cpp      # [ANDROID 11/09 WAUTO B2] trang noi dung chay theo bang (UiWAutoBang.h sinh bang may)",
        "    ${JX_SRC}/S3Client/Ui/UiCase/UiWAutoDsach.cpp      # %s bang phu: sua 6 danh sach + chon muc trong danh sach dai" % DAU,
    ], "CMakeLists: them UiWAutoDsach.cpp")


def main():
    tao("Sources/S3Client/Ui/UiCase/UiWAutoDsach.h", H, "UiWAutoDsach_H")
    tao("Sources/S3Client/Ui/UiCase/UiWAutoDsach.cpp", CPP, "KUiWAutoDsach::")
    tao("android/du_lieu_ghi_de/ui/ui3/uiwauto_dsach.ini", INI, "[Bang]")
    va("Sources/S3Client/Ui/UiCase/UiWAuto.h", "m_BangPhu", va_uiwauto_h)
    va("Sources/S3Client/Ui/UiCase/UiWAuto.cpp", "m_BangPhu", va_uiwauto_cpp)
    va("Sources/S3Client/Ui/UiCase/UiWAuto.cpp", "KUiWAutoDsach::DongLai", va_uiwauto_cpp_b)
    va("android/CMakeLists.txt", "UiWAutoDsach.cpp", va_cmake)
    print("xong wauto9")


if __name__ == "__main__":
    main()
