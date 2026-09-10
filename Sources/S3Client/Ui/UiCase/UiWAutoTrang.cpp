//---------------------------------------------------------------------------
// [ANDROID 11/09 WAUTO B2] Trang noi dung chay theo bang - xem UiWAutoTrang.h. Chuoi TCVN3 sinh bang vn_edit.py (android/va_nguon_android_wauto6.py).
//---------------------------------------------------------------------------
#include "KWin32.h"
#ifdef JX_ANDROID
#include "KIniFile.h"
#include "KDebug.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../Elem/PopupMenu.h"
#include "../UiBase.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iRepresentShell*	g_pRepresentShell;	// nhu WndImage.cpp / WndEdit.cpp
#include "UiWAutoTrang.h"
#include "UiWAutoBang.h"
#include "../../Platform/JxWAutoNoiBo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" int JxCore_WAutoDanhSachChieu(IPCSkillInfo* pOut, int nMax);	// CoreShell.cpp (chi Android)

#define WA_CHON_CAO		24
#define WA_MENU_TOI_DA	40

// Bong (RU_T_SHADOW): KRepresentShell3 ve voi alpha = 255 - (a << 3) -> a trong 0..31, a CANG NHO cang DAC
static void WA_Bong(int x0, int y0, int x1, int y1, int r, int g, int b, int a)
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

// Khung vien 1 px (RU_T_RECT -> DrawRectFrame, mau dac)
static void WA_Vien(int x0, int y0, int x1, int y1, int r, int g, int b)
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

// ---------------------------------------------------------------- o nhap co nen
void KWndNhapWA::PaintWindow()
{
	if (g_pRepresentShell && (m_Style & WND_S_VISIBLE))
	{
		WA_Bong(m_nAbsoluteLeft - 3, m_nAbsoluteTop - 2, m_nAbsoluteLeft + m_Width + 3, m_nAbsoluteTop + m_Height + 1, 12, 14, 18, 6);
		WA_Vien(m_nAbsoluteLeft - 3, m_nAbsoluteTop - 2, m_nAbsoluteLeft + m_Width + 2, m_nAbsoluteTop + m_Height, 150, 120, 70);
	}
	KWndEdit512::PaintWindow();
}

// ---------------------------------------------------------------- o nhom
// bo mau theo Mau= (vien r,g,b ; nen tieu de r,g,b) - 6 bo, khac nhau ro tren nen khung toi; chu tieu de trang nga
static const unsigned char s_aMauHop[6][6] = {
	{ 150, 110,  60,  60, 40, 10 },	// vang nau
	{  60, 140, 140,  10, 50, 50 },	// xanh ngoc
	{  80, 110, 180,  15, 25, 60 },	// xanh lam
	{  90, 150,  80,  15, 50, 15 },	// xanh la
	{ 150,  90, 160,  50, 15, 60 },	// tim
	{ 190, 120,  50,  70, 35, 10 },	// cam
};

KWndHopNhomWA::KWndHopNhomWA()
{
	m_szTieuDe[0] = 0;
	m_nMau = 0;
}

int KWndHopNhomWA::Init(KIniFile* pIniFile, const char* pSection)
{
	if (!KWndWindow::Init(pIniFile, pSection))
		return false;
	m_szTieuDe[0] = 0;
	pIniFile->GetString(pSection, "TieuDe", "", m_szTieuDe, sizeof(m_szTieuDe));
	pIniFile->GetInteger(pSection, "Mau", 0, &m_nMau);
	if (m_nMau < 0 || m_nMau >= 6)
		m_nMau = 0;
	return true;
}

void KWndHopNhomWA::PaintWindow()
{
	if (!g_pRepresentShell || !(m_Style & WND_S_VISIBLE))
		return;
	const unsigned char* m = s_aMauHop[m_nMau];
	int x0 = m_nAbsoluteLeft, y0 = m_nAbsoluteTop, x1 = x0 + m_Width, y1 = y0 + m_Height;
	WA_Bong(x0, y0, x1, y1, 0, 0, 0, 22);				// toi nhe (~30%) de tach nhom khoi nen khung
	WA_Bong(x0, y0, x1, y0 + 3, m[3], m[4], m[5], 6);	// dai mau mong sat mep tren (nhan mau nhom ca khi khong tieu de)
	WA_Vien(x0, y0, x1 - 1, y1 - 1, m[0], m[1], m[2]);
	int n = (int)strlen(m_szTieuDe);
	if (n > 0)
	{
		WA_Bong(x0 + 8, y0 + 2, x0 + 8 + n * 6 + 10, y0 + 18, m[3], m[4], m[5], 2);	// nen mau nhom sau tieu de
		WA_Vien(x0 + 8, y0 + 2, x0 + 8 + n * 6 + 10, y0 + 18, m[0], m[1], m[2]);
		g_pRepresentShell->OutputText(12, m_szTieuDe, n, x0 + 13, y0 + 4, 0x00F0EAD8, 0, TEXT_IN_SINGLE_PLANE_COORD, 0x00000000);
	}
}

// ---------------------------------------------------------------- trang
KUiWAutoTrang::KUiWAutoTrang()
{
	m_nTab = -1;
	m_pTab = NULL;
	m_nChieu = -1;
	m_bDangDien = 0;
	m_nAnTam = 0;
}

void KUiWAutoTrang::KhoiTao()
{
	int i;
	for (i = 0; i < WA_TR_HOP; i++)		AddChild(&m_Hop[i]);	// o nhom ve truoc, nam duoi
	for (i = 0; i < WA_TR_NHAN; i++)	AddChild(&m_Nhan[i]);
	for (i = 0; i < WA_TR_TICK; i++)	AddChild(&m_Tick[i]);
	for (i = 0; i < WA_TR_NHAP; i++)	AddChild(&m_Nhap[i]);
	for (i = 0; i < WA_TR_CHON; i++)	AddChild(&m_Chon[i]);
	for (i = 0; i < WA_TR_NUT; i++)		AddChild(&m_Nut[i]);
	AnHet();
}

void KUiWAutoTrang::AnHet()
{
	int i;
	for (i = 0; i < WA_TR_HOP; i++)		m_Hop[i].Hide();
	for (i = 0; i < WA_TR_NHAN; i++)	m_Nhan[i].Hide();
	for (i = 0; i < WA_TR_TICK; i++)	m_Tick[i].Hide();
	for (i = 0; i < WA_TR_NHAP; i++)	m_Nhap[i].Hide();
	for (i = 0; i < WA_TR_CHON; i++)	m_Chon[i].Hide();
	for (i = 0; i < WA_TR_NUT; i++)		m_Nut[i].Hide();
	m_nAnTam = 0;
}

const WAUiMuc* KUiWAutoTrang::Muc(int nLoai, int nKhe)
{
	if (!m_pTab)
		return NULL;
	for (int i = 0; i < m_pTab->nMuc; i++)
		if (m_pTab->pMuc[i].nLoai == nLoai && m_pTab->pMuc[i].nKhe == nKhe)
			return &m_pTab->pMuc[i];
	return NULL;
}

// Dia chi truong trong autoData dang dung: offset + chi so * co phan tu (mang bTKGio[4]...). Chuoi: ca mang.
void* KUiWAutoTrang::DiaChi(const WAUiMuc* p)
{
	if (!p || p->nOff < 0)
		return NULL;
	int nPhanTu = (p->nKieu == WA_KIEU_SHORT) ? 2 : ((p->nKieu == WA_KIEU_CHUOI) ? (int)p->nCo : 4);
	int nOff = p->nOff + ((p->nKieu == WA_KIEU_CHUOI) ? 0 : (int)p->nChiSo * nPhanTu);
	if (nPhanTu <= 0 || nOff < 0 || nOff + nPhanTu > WA_SIZEOF_AUTODATA)
		return NULL;
	autoData* pCH = JxWAuto_CauHinh();
	if (!pCH || sizeof(autoData) != WA_SIZEOF_AUTODATA)	// bang sinh tu ipc_shared.h khac ban dang dich -> khong ghi bua
		return NULL;
	return (BYTE*)pCH + nOff;
}

int KUiWAutoTrang::LayInt(const WAUiMuc* p)
{
	void* a = DiaChi(p);
	if (!a)
		return 0;
	if (p->nKieu == WA_KIEU_SHORT)
		return *(short*)a;
	return *(int*)a;
}

void KUiWAutoTrang::DatInt(const WAUiMuc* p, int v)
{
	void* a = DiaChi(p);
	if (!a)
		return;
	if (p->nKieu == WA_KIEU_SHORT)
		*(short*)a = (short)v;
	else
		*(int*)a = v;
}

void KUiWAutoTrang::NapTab(int nTab)
{
	char Scheme[128];
	char Buff[256];
	char szMuc[32];
	KIniFile Ini;
	int i;
	if (nTab < 0 || nTab >= WA_TR_SO_TAB)
		return;
	m_nTab = nTab;
	m_pTab = &s_WAUiTab[nTab];
	AnHet();
	g_UiBase.GetCurSchemePath(Scheme, 128);
	sprintf(Buff, "%s\\%s", Scheme, m_pTab->szIni);
	if (!Ini.Load(Buff))
	{
		g_DebugLog("[WAUTO-UI] tab %d: khong doc duoc bo cuc %s", nTab, Buff);
		return;
	}
	Init(&Ini, "Trang");
	for (i = 0; i < WA_TR_HOP; i++)
	{
		sprintf(szMuc, "Hop%d", i);
		if (Ini.IsSectionExist(szMuc))
		{
			m_Hop[i].Init(&Ini, szMuc);
			m_Hop[i].Show();
		}
	}
	for (i = 0; i < WA_TR_NHAN; i++)
	{
		sprintf(szMuc, "Nhan%d", i);
		if (Ini.IsSectionExist(szMuc))
		{
			m_Nhan[i].SetText("");		// KWndText::Init chi doc Text= khi dang rong
			m_Nhan[i].Init(&Ini, szMuc);
			m_Nhan[i].Show();
		}
	}
	for (i = 0; i < m_pTab->nMuc; i++)
	{
		const WAUiMuc& m = m_pTab->pMuc[i];
		switch (m.nLoai)
		{
		case WA_MUC_TICK:
			sprintf(szMuc, "Tick%d", m.nKhe);
			if (m.nKhe < WA_TR_TICK && Ini.IsSectionExist(szMuc))
			{
				m_Tick[m.nKhe].Init(&Ini, szMuc);
				m_Tick[m.nKhe].Show();
				m_Tick[m.nKhe].Enable(m.nOff >= 0);
			}
			break;
		case WA_MUC_NHAP:
			sprintf(szMuc, "Nhap%d", m.nKhe);
			if (m.nKhe < WA_TR_NHAP && Ini.IsSectionExist(szMuc))
			{
				m_Nhap[m.nKhe].Init(&Ini, szMuc);
				m_Nhap[m.nKhe].Show();
				m_Nhap[m.nKhe].Enable(m.nOff >= 0);
			}
			break;
		case WA_MUC_CHON:
			sprintf(szMuc, "Chon%d", m.nKhe);
			if (m.nKhe < WA_TR_CHON && Ini.IsSectionExist(szMuc))
			{
				m_Chon[m.nKhe].Init(&Ini, szMuc);
				m_Chon[m.nKhe].Show();
				m_Chon[m.nKhe].Enable(m.nOff >= 0 && m.nNguon != WA_NGUON_KHAC);
			}
			break;
		case WA_MUC_NUT:
			sprintf(szMuc, "Nut%d", m.nKhe);
			if (m.nKhe < WA_TR_NUT && Ini.IsSectionExist(szMuc))
			{
				m_Nut[m.nKhe].Init(&Ini, szMuc);
				m_Nut[m.nKhe].Show();
				m_Nut[m.nKhe].Enable(0);		// (B2) nut hanh dong: lam o buoc cua tung tab
			}
			break;
		default:
			break;
		}
	}
	DienGiaTri();
	g_DebugLog("[WAUTO-UI] tab %d: %d muc, %d hang, %d cot (%s)", nTab, m_pTab->nMuc, m_pTab->nHang, m_pTab->nCot, m_pTab->szIni);
}

void KUiWAutoTrang::DienGiaTri()
{
	if (!m_pTab)
		return;
	m_nChieu = -1;		// nap lai danh sach chieu (nhan vat vua hoc them chieu)
	m_bDangDien = 1;
	for (int i = 0; i < m_pTab->nMuc; i++)
	{
		const WAUiMuc& m = m_pTab->pMuc[i];
		switch (m.nLoai)
		{
		case WA_MUC_TICK:
			if (m.nKhe < WA_TR_TICK)
				m_Tick[m.nKhe].CheckButton(LayInt(&m) ? 1 : 0);
			break;
		case WA_MUC_NHAP:
			if (m.nKhe < WA_TR_NHAP)
			{
				if (m.nKieu == WA_KIEU_CHUOI)
				{
					const char* a = (const char*)DiaChi(&m);
					m_Nhap[m.nKhe].SetText(a ? a : "");
				}
				else
					m_Nhap[m.nKhe].SetIntText(LayInt(&m));
			}
			break;
		case WA_MUC_CHON:
			DienChon(&m);
			break;
		default:
			break;
		}
	}
	m_bDangDien = 0;
}

void KUiWAutoTrang::NapChieu()
{
	m_nChieu = JxCore_WAutoDanhSachChieu(m_aChieu, defSKILLNUMGET);
	if (m_nChieu < 0)
		m_nChieu = 0;
	if (m_nChieu > defSKILLNUMGET)
		m_nChieu = defSKILLNUMGET;
}

void KUiWAutoTrang::DienChon(const WAUiMuc* p)
{
	char sz[64];
	int i, v;
	if (!p || p->nKhe >= WA_TR_CHON)
		return;
	v = LayInt(p);
	sz[0] = 0;
	if (p->nNguon == WA_NGUON_CHIEU)
	{
		if (m_nChieu < 0)
			NapChieu();
		if (v == 0)
			strcpy(sz, "Kh«ng thiÕt lËp");
		else
		{
			snprintf(sz, sizeof(sz), "#%d", v);
			for (i = 0; i < m_nChieu; i++)
				if (m_aChieu[i].nId == v)
				{
					strncpy(sz, m_aChieu[i].szName, 31);
					sz[31] = 0;
					break;
				}
		}
	}
	else if (p->pLuaChon && p->nLuaChon > 0)
	{
		if (v < 0 || v >= (int)p->nLuaChon)
			v = 0;
		strncpy(sz, p->pLuaChon[v], 31);
		sz[31] = 0;
	}
	else
		strcpy(sz, "(ch­a hç trî)");
	// chu dai hon phan trai mui ten (rong - 24) / 6 ky tu -> cat + ".." (menu van hien du)
	{
		int w = 0, h = 0, nMax;
		m_Chon[p->nKhe].GetSize(&w, &h);
		nMax = (w - 24) / 6;
		if (nMax >= 4 && (int)strlen(sz) > nMax)
		{
			sz[nMax - 2] = '.';
			sz[nMax - 1] = '.';
			sz[nMax] = 0;
		}
	}
	m_Chon[p->nKhe].SetLabel(sz);
}

void KUiWAutoTrang::MoMenuChon(int nKhe)
{
	const WAUiMuc* p = Muc(WA_MUC_CHON, nKhe);
	int i, n = 0, x = 0, y = 0;
	if (!p || nKhe >= WA_TR_CHON)
		return;
	if (p->nNguon == WA_NGUON_CHIEU)
	{
		NapChieu();
		n = m_nChieu + 1;
	}
	else if (p->pLuaChon)
		n = p->nLuaChon;
	if (n <= 0)
		return;
	if (n > WA_MENU_TOI_DA)
		n = WA_MENU_TOI_DA;
	KPopupMenuData* pMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(n));
	if (!pMenu)
		return;
	KPopupMenu::InitMenuData(pMenu, n);		// uID = chi so
	pMenu->nNumItem = 0;
	pMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (i = 0; i < n; i++)
	{
		const char* s = (p->nNguon == WA_NGUON_CHIEU) ? ((i == 0) ? "Kh«ng thiÕt lËp" : m_aChieu[i - 1].szName) : p->pLuaChon[i];
		strncpy(pMenu->Items[i].szData, s ? s : "", 63);
		pMenu->Items[i].szData[63] = 0;
		pMenu->Items[i].uDataLen = strlen(pMenu->Items[i].szData);
		pMenu->nNumItem++;
	}
	m_Chon[nKhe].GetAbsolutePos(&x, &y);
	pMenu->nX = x;
	pMenu->nY = y + WA_CHON_CAO;		// ALW: nua duoi man hinh thi menu mo LEN tren diem nay
	KPopupMenu::Popup(pMenu, this, nKhe);
	AnDuoiMenu(1);
}

void KUiWAutoTrang::ChonMenu(int nKhe, int nMuc)
{
	const WAUiMuc* p = Muc(WA_MUC_CHON, nKhe);
	int v;
	if (!p || nMuc < 0)
		return;
	if (p->nNguon == WA_NGUON_CHIEU)
		v = (nMuc == 0 || nMuc - 1 >= m_nChieu) ? 0 : m_aChieu[nMuc - 1].nId;
	else
		v = nMuc;
	DatInt(p, v);
	DienChon(p);
	DaDoi();
	g_DebugLog("[WAUTO-UI] %s = %d", p->szIdc, v);
}

// Trinh chieu Android ve anh / chu cua cua so SAU bong menu (menu bi chu cua trang de len) -> tam an widget nam duoi menu
void KUiWAutoTrang::ThuAn(KWndWindow* p, int x0, int y0, int x1, int y1)
{
	int l = 0, t = 0, w = 0, h = 0;
	if (!p->IsVisible() || m_nAnTam >= WA_TR_AN_TOI_DA)
		return;
	p->GetAbsolutePos(&l, &t);
	p->GetSize(&w, &h);
	if (l < x1 && l + w > x0 && t < y1 && t + h > y0)
	{
		p->Hide();
		m_apAnTam[m_nAnTam++] = p;
	}
}

void KUiWAutoTrang::AnDuoiMenu(int bAn)
{
	int i;
	if (!bAn)
	{
		for (i = 0; i < m_nAnTam; i++)
			m_apAnTam[i]->Show();
		m_nAnTam = 0;
		return;
	}
	KPopupMenuData* pM = KPopupMenu::GetMenuData();
	if (!pM)
		return;
	int x0 = pM->nX - 2, y0 = pM->nY - 2;
	int x1 = pM->nX + pM->nItemWidth + 2, y1 = pM->nY + pM->nItemHeight * pM->nNumItem + 4;
	for (i = 0; i < WA_TR_NHAN; i++)	ThuAn(&m_Nhan[i], x0, y0, x1, y1);
	for (i = 0; i < WA_TR_TICK; i++)	ThuAn(&m_Tick[i], x0, y0, x1, y1);
	for (i = 0; i < WA_TR_NHAP; i++)	ThuAn(&m_Nhap[i], x0, y0, x1, y1);
	for (i = 0; i < WA_TR_CHON; i++)	ThuAn(&m_Chon[i], x0, y0, x1, y1);
	for (i = 0; i < WA_TR_NUT; i++)		ThuAn(&m_Nut[i], x0, y0, x1, y1);
}

void KUiWAutoTrang::Breathe()
{
	if (m_nAnTam > 0 && KPopupMenu::GetMenuData() == NULL)	// menu bi huy ma khong bao (Cancel tu noi khac)
		AnDuoiMenu(0);
}

void KUiWAutoTrang::DaDoi()
{
	JxWAuto_LuuCauHinh();
}

int KUiWAutoTrang::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	int i;
	if (uMsg == WND_N_BUTTON_CLICK)
	{
		for (i = 0; i < WA_TR_TICK; i++)
			if (uParam == (KUPARAM)(KWndWindow*)&m_Tick[i])
			{
				const WAUiMuc* p = Muc(WA_MUC_TICK, i);
				if (p)
				{
					int v = LayInt(p) ? 0 : 1;
					DatInt(p, v);
					m_Tick[i].CheckButton(v);
					DaDoi();
					g_DebugLog("[WAUTO-UI] %s = %d", p->szIdc, v);
				}
				return 1;
			}
		for (i = 0; i < WA_TR_CHON; i++)
			if (uParam == (KUPARAM)(KWndWindow*)&m_Chon[i])
			{
				MoMenuChon(i);
				return 1;
			}
		for (i = 0; i < WA_TR_NUT; i++)
			if (uParam == (KUPARAM)(KWndWindow*)&m_Nut[i])
				return 1;		// (B2) chua co hanh dong
	}
	else if (uMsg == WND_N_EDIT_CHANGE && !m_bDangDien)
	{
		for (i = 0; i < WA_TR_NHAP; i++)
			if (uParam == (KUPARAM)(KWndWindow*)&m_Nhap[i])
			{
				const WAUiMuc* p = Muc(WA_MUC_NHAP, i);
				if (p)
				{
					if (p->nKieu == WA_KIEU_CHUOI)
					{
						char* a = (char*)DiaChi(p);
						if (a && p->nCo > 0)
						{
							m_Nhap[i].GetText(a, p->nCo, false);
							a[p->nCo - 1] = 0;
						}
					}
					else
						DatInt(p, m_Nhap[i].GetIntNumber());
					DaDoi();
				}
				return 1;
			}
	}
	else if (uMsg == WND_M_MENUITEM_SELECTED)
	{
		if (uParam == (KUPARAM)(KWndWindow*)this)
		{
			AnDuoiMenu(0);
			if ((short)LOWORD(nParam) >= 0)
				ChonMenu((int)HIWORD(nParam), (int)(short)LOWORD(nParam));
			return 1;
		}
	}
	return KWndWindow::WndProc(uMsg, uParam, nParam);
}
#endif // JX_ANDROID
