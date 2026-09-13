//---------------------------------------------------------------------------
// [ANDROID 11/09 WAUTO B2] Trang noi dung chay theo bang - xem UiWAutoTrang.h. Chuoi TCVN3 sinh bang vn_edit.py (android/va_nguon_android_wauto6.py).
//---------------------------------------------------------------------------
#include "KWin32.h"
#ifdef JX_MOBILE
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
#include "UiWAutoDsach.h"	// [WAUTO 12/09] bang phu: danh sach + bo chon dai
#include "../../Platform/JxWAutoNoiBo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" int JxCore_WAutoDanhSachChieu(IPCSkillInfo* pOut, int nMax);	// CoreShell.cpp (chi Android)
extern "C" int JxCore_WAutoSoLieu(int* pnSo, int nSoMax, char* szMap, int nMapMax, char* szKN, int nKNMax);
extern "C" int JxCore_WAutoViTri(int* pnMapId, char* szMap, int nMapMax, int* pnX, int* pnY);
extern "C" int JxCore_WAutoTenQuanhDay(char* pOut, int nMax);

#define WA_CHON_CAO		24
// WA_MENU_TOI_DA / WA_MENU_DAI: dinh nghia trong UiWAutoTrang.h (dung trong khai bao thanh vien)

static char s_szTenQuanh[100][32];	// [WAUTO 12/09] ten nguoi choi quanh day cho hop "Theo sau"

// [WAUTO 12/09] Mot chieu co thuoc danh sach con nao khong - dung dieu kien cua WAuto.exe (WAuto.cpp:3057-3097).
// Game chi tra MOT danh sach phang (GetAllSkillByType) nen phai loc lai o day; khong loc thi chon duoc
// chieu sai khe (vi du nhet chieu danh vao o "Vong sang").
static int WA_ChieuHop(const IPCSkillInfo& s, int nLoc)
{
	switch (nLoc)
	{
	case WA_LC_K:	return s.bAura ? 0 : 1;
	case WA_LC_S:	return (!s.bAura && (s.bState || s.nStyle == 2) && s.bAlly) ? 1 : 0;
	case WA_LC_SE:	return (s.bState && !s.bAlly) ? 1 : 0;
	case WA_LC_BP:	return (s.nStyle <= 1 && !s.bAlly) ? 1 : 0;
	case WA_LC_A:	return s.bAura ? 1 : 0;
	}
	return 1;
}

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

// ---------------------------------------------------------------- nhan chu cham duoc
// [WAUTO 12/09] Cham vao CHU canh o tick = cham vao o tick. Bao len cha bang WND_N_BUTTON_CLICK cua chinh nhan;
// KUiWAutoTrang::WndProc tim o tick nao nhan nhan do lam nhan cua no (nKheNhan) roi bat/tat.
int KWndNhanWA::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	if (uMsg == WM_LBUTTONDOWN && m_pParentWnd && !IsDisable())
	{
		m_pParentWnd->WndProc(WND_N_BUTTON_CLICK, (KUPARAM)(KWndWindow*)this, 0);
		return 1;
	}
	return KWndText80::WndProc(uMsg, uParam, nParam);
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
	m_nDongMenu = 0;
	m_uSongKe = 0;
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
				// [WAUTO 12/09] hop chon nguon DONG (lay luc chay) van bat neu co viec: Theo sau lay ten nguoi quanh day
				m_Chon[m.nKhe].Enable(m.nOff >= 0 && (m.nNguon != WA_NGUON_KHAC || m.nViec != WA_V_KHONG));
			}
			break;
		case WA_MUC_NUT:
		case WA_MUC_DSACH:		// [WAUTO 12/09] o danh sach = nut "Sua" + dong tom tat (noi dung sua trong bang phu)
			sprintf(szMuc, "Nut%d", m.nKhe);
			if (m.nKhe < WA_TR_NUT && Ini.IsSectionExist(szMuc))
			{
				m_Nut[m.nKhe].Init(&Ini, szMuc);
				m_Nut[m.nKhe].Show();
				m_Nut[m.nKhe].Enable(m.nViec != WA_V_KHONG);	// [WAUTO 12/09] nut nao co viec thi bat
			}
			break;
		default:
			break;
		}
	}
	DienGiaTri();
	g_DebugLog("[WAUTO-UI] tab %d: %d muc, %d hang, %d cot (%s)", nTab, m_pTab->nMuc, m_pTab->nHang, m_pTab->nCot, m_pTab->szIni);
	if (UiLayPhongCay() != 1000)	// [PHONGBANG 14/09 e] bang Auto dang phong: vi tri / co cua trang + widget da phong ngay trong Init
		UiPhongChuCay(UiLayPhongCay());	// (KWndWindow::Init -> UiToaDo_ApChoOInit); rieng co chu doc SAU do -> phong lai chu
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
			{
				int v = LayInt(&m) ? 1 : 0;
				if (m.nViec == WA_V_UUTIEN_GAN)	// [WAUTO 12/09] cap radio: "Khoang cach" = nPriority BANG 0
					v = !v;
				m_Tick[m.nKhe].CheckButton(v);
			}
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
	CapNhatSong();		// [WAUTO 12/09] o so lieu + dong tom tat danh sach
}

// [WAUTO 12/09] doc lai MOI o tick cua tab: cap radio "Uu tien" cua the PK phai doi theo nhau
void KUiWAutoTrang::DienTick()
{
	if (!m_pTab)
		return;
	for (int i = 0; i < m_pTab->nMuc; i++)
	{
		const WAUiMuc& m = m_pTab->pMuc[i];
		if (m.nLoai != WA_MUC_TICK || m.nKhe >= WA_TR_TICK)
			continue;
		int v = LayInt(&m) ? 1 : 0;
		if (m.nViec == WA_V_UUTIEN_GAN)
			v = !v;
		m_Tick[m.nKhe].CheckButton(v);
	}
}

// [WAUTO 12/09] Dien cac o CHU chay theo trang thai: so lieu nhan vat (the Co ban) va ban do / diem da dat (the Di chuyen),
// cung dong tom tat cua moi o danh sach. Ban PC dien may o nay trong ProcIpcCommand / UpdateUI.
void KUiWAutoTrang::CapNhatSong()
{
	char sz[128];
	char szMap[40] = { 0 };
	char szKN[48] = { 0 };
	int aSo[12] = { 0 };
	int bCo, i;
	if (!m_pTab)
		return;
	bCo = JxCore_WAutoSoLieu(aSo, 12, szMap, sizeof(szMap), szKN, sizeof(szKN));
	for (i = 0; i < m_pTab->nMuc; i++)
	{
		const WAUiMuc& m = m_pTab->pMuc[i];
		if (m.nLoai == WA_MUC_SOLIEU)
		{
			if (m.nKhe >= WA_TR_NHAN)
				continue;
			sz[0] = 0;
			switch (m.nChiSo)
			{
			case WA_SL_SINHLUC:		if (bCo) snprintf(sz, sizeof(sz), "%d/%d", aSo[0], aSo[1]); break;
			case WA_SL_NOILUC:		if (bCo) snprintf(sz, sizeof(sz), "%d/%d", aSo[2], aSo[3]); break;
			case WA_SL_THELUC:		if (bCo) snprintf(sz, sizeof(sz), "%d/%d", aSo[4], aSo[5]); break;
			case WA_SL_DANGCAP:		if (bCo) snprintf(sz, sizeof(sz), "%d", aSo[6]); break;
			case WA_SL_BANDO:		if (bCo) snprintf(sz, sizeof(sz), "%s", szMap); break;
			case WA_SL_TOADO:		if (bCo) snprintf(sz, sizeof(sz), "%d / %d", aSo[8], aSo[9]); break;
			case WA_SL_KINHNGHIEM:	if (bCo) snprintf(sz, sizeof(sz), "%s", szKN); break;
			case WA_SL_MAP_AP:
			{
				const char* a = (const char*)DiaChi(&m);
				if (a && a[0])
					snprintf(sz, sizeof(sz), "%s", a);
				break;
			}
			case WA_SL_DIEM_X:
			case WA_SL_DIEM_Y:
				if (m.nOff >= 0)
					snprintf(sz, sizeof(sz), "%d", LayInt(&m));
				break;
			}
			m_Nhan[m.nKhe].SetText(sz[0] ? sz : "-");
		}
		else if (m.nLoai == WA_MUC_DSACH && m.nKheNhan < WA_TR_NHAN)
		{
			KUiWAutoDsach::TomTat(m.nViec, sz, sizeof(sz));
			m_Nhan[m.nKheNhan].SetText(sz);
		}
	}
}

const WAUiMuc* KUiWAutoTrang::MucTheoIdc(const char* szIdc)
{
	if (!m_pTab || !szIdc)
		return NULL;
	for (int i = 0; i < m_pTab->nMuc; i++)
		if (m_pTab->pMuc[i].szIdc && !strcmp(m_pTab->pMuc[i].szIdc, szIdc))
			return &m_pTab->pMuc[i];
	return NULL;
}

// [WAUTO 12/09] Nut co viec: hai nut "Lay" cua the Di chuyen va bon nut mo bang danh sach.
void KUiWAutoTrang::LamViec(const WAUiMuc* p)
{
	autoData* pCH = JxWAuto_CauHinh();
	char szMap[32] = { 0 };
	int nMapId = 0, x = 0, y = 0;
	if (!p || !pCH || sizeof(autoData) != WA_SIZEOF_AUTODATA)
		return;
	switch (p->nViec)
	{
	case WA_V_LAY_BANDO:
	case WA_V_LAY_DIEM:
		if (!JxCore_WAutoViTri(&nMapId, szMap, sizeof(szMap), &x, &y))
			return;
		if (p->nViec == WA_V_LAY_DIEM)
		{
			pCH->nPointX = x;
			pCH->nPointY = y;
		}
		pCH->nMoveMapId = nMapId;		// nhu ban PC: hai nut nay deu dat lai ban do ap dung
		strncpy(pCH->szMoveMap, szMap, sizeof(pCH->szMoveMap) - 1);
		pCH->szMoveMap[sizeof(pCH->szMoveMap) - 1] = 0;
		DaDoi();
		CapNhatSong();
		g_DebugLog("[WAUTO-UI] %s: map=%d %s (%d,%d)", p->szIdc, nMapId, szMap, x, y);
		break;
	case WA_V_DS_TOADO:
	case WA_V_DS_LOC:
	case WA_V_DS_KHONGNHAT:
	case WA_V_DS_TODOI:
	case WA_V_DS_NGUHANH:
	case WA_V_DS_LIENDAU:
		KUiWAutoDsach::MoSua(p->nViec);
		break;
	}
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
	int i, v = 0;
	if (!p || p->nKhe >= WA_TR_CHON)
		return;
	sz[0] = 0;
	if (p->nKieu == WA_KIEU_CHUOI)
	{	// [WAUTO 12/09] hop "Theo sau": gia tri la TEN nhan vat (szFollName), khong phai so
		const char* a = (const char*)DiaChi(p);
		if (a && a[0])
			strncpy(sz, a, 31);
		else
			strcpy(sz, "(kh«ng theo ai)");
		sz[31] = 0;
		v = 0;
	}
	else if (p->nViec == WA_V_ST_BOSS)
	{	// [WAUTO 12/09] gia tri = 141 + chi so dong (WAuto.cpp:1328)
		v = LayInt(p) - 141;
		if (v < 0 || v >= (int)p->nLuaChon)
			v = 0;
		strncpy(sz, p->pLuaChon[v], 31);
		sz[31] = 0;
	}
	else if (p->nViec == WA_V_TK_RUONG)
	{	// [WAUTO 12/09] gia tri = MA HUONG 0..4, 5 = gan nhat; danh sach tinh xep dung theo ma nen chi so = ma.
		// Thanh dang chon o "Het tran ve" KHONG co huong do -> lui ve "Gan nhat" va ghi lai, y nhu
		// WA_NapRuongHuong cua ban PC lam khi doi thanh (WAuto.cpp:160-178).
		const WAUiMuc* pVe = MucTheoIdc("IDC_COMBO_9_VE");
		int nThanh = pVe ? LayInt(pVe) : 0;
		v = LayInt(p);
		if (nThanh < 0 || nThanh >= WA_TKR_THANH)
			nThanh = 0;
		if (v < 0 || v >= (int)p->nLuaChon)
			v = (int)p->nLuaChon - 1;
		if (v < WA_TKR_HUONG && !s_WATKRuongCo[nThanh][v])
		{
			v = (int)p->nLuaChon - 1;		// dong cuoi = "Gan nhat (tu chon)" = ma 5
			if (!m_bDangDien)
			{
				DatInt(p, v);
				DaDoi();
			}
			else
				DatInt(p, v);
		}
		strncpy(sz, p->pLuaChon[v], 31);
		sz[31] = 0;
	}
	else if (p->nNguon == WA_NGUON_CHIEU)
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
		v = LayInt(p);		// [WAUTO 12/09] CAC NHANH TREN moi tu doc gia tri; nhanh chung nay truoc do dung v chua gan
							// -> hop chon tinh nao cung hien dong 0 (do 12:29: nSelMap = 1 ma o van hien dong dau).
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

// [WAUTO 12/09] Dung danh sach dong cho mot hop chon: m_apDong[i] = chu, m_anGT[i] = gia tri se ghi vao autoData.
// Tra so dong. Cac hop co viec rieng: Theo sau (ten quanh day), Ruong cua (loc theo thanh), Boss (141 + dong).
int KUiWAutoTrang::DungMenu(const WAUiMuc* p)
{
	int i, n = 0;
	m_nDongMenu = 0;
	if (!p)
		return 0;
	if (p->nViec == WA_V_THEO_SAU)
	{
		int nT = JxCore_WAutoTenQuanhDay((char*)s_szTenQuanh, 100);
		m_apDong[0] = "(kh«ng theo ai)";
		m_anGT[0] = 0;
		n = 1;
		for (i = 0; i < nT && n < WA_MENU_TOI_DA; i++)
		{
			m_apDong[n] = s_szTenQuanh[i];
			m_anGT[n] = i + 1;
			n++;
		}
	}
	else if (p->nViec == WA_V_TK_RUONG)
	{	// chi liet ke huong CO THAT cua thanh dang chon o hop "Het tran ve"; gia tri = MA HUONG
		const WAUiMuc* pVe = MucTheoIdc("IDC_COMBO_9_VE");
		int nThanh = pVe ? LayInt(pVe) : 0;
		if (nThanh < 0 || nThanh >= WA_TKR_THANH)
			nThanh = 0;
		for (i = 0; i < WA_TKR_HUONG && i < (int)p->nLuaChon; i++)
			if (s_WATKRuongCo[nThanh][i])
			{
				m_apDong[n] = p->pLuaChon[i];
				m_anGT[n] = i;
				n++;
			}
		if ((int)p->nLuaChon > WA_TKR_HUONG)
		{	// dong cuoi: "Gan nhat (tu chon)" = ma 5
			m_apDong[n] = p->pLuaChon[WA_TKR_HUONG];
			m_anGT[n] = WA_TKR_HUONG;
			n++;
		}
	}
	else if (p->nNguon == WA_NGUON_CHIEU)
	{
		NapChieu();
		m_apDong[0] = "Kh«ng thiÕt lËp";
		m_anGT[0] = 0;
		n = 1;
		for (i = 0; i < m_nChieu && n < WA_MENU_TOI_DA; i++)
			if (WA_ChieuHop(m_aChieu[i], p->nLocChieu))
			{
				m_apDong[n] = m_aChieu[i].szName;
				m_anGT[n] = m_aChieu[i].nId;
				n++;
			}
	}
	else if (p->pLuaChon)
	{
		for (i = 0; i < (int)p->nLuaChon && n < WA_MENU_TOI_DA; i++)
		{
			m_apDong[n] = p->pLuaChon[i];
			m_anGT[n] = (p->nViec == WA_V_ST_BOSS) ? (141 + i) : i;
			n++;
		}
	}
	m_nDongMenu = n;
	return n;
}

void KUiWAutoTrang::MoMenuChon(int nKhe)
{
	const WAUiMuc* p = Muc(WA_MUC_CHON, nKhe);
	int i, n = 0, x = 0, y = 0;
	if (!p || nKhe >= WA_TR_CHON)
		return;
	n = DungMenu(p);
	if (n <= 0)
		return;
	if (n >= WA_MENU_DAI)
	{	// [WAUTO 12/09] danh sach dai: KPopupMenu khong cuon (cao = so dong x 26 px) nen dong thu 24 tro di cham khong toi
		char szTieuDe[80];
		int nCu = -1;
		int vGio = (p->nKieu == WA_KIEU_CHUOI) ? -1 : LayInt(p);
		for (i = 0; i < n; i++)
			if (vGio >= 0 && m_anGT[i] == vGio)
			{
				nCu = i;
				break;
			}
		// [WAUTO 12/09] tieu de = NHAN cung hang ("Vong sang #1", "Ruong cua"...), khong phai chu dang hien trong hop
		szTieuDe[0] = 0;
		if (p->nKheNhan < WA_TR_NHAN)
			m_Nhan[p->nKheNhan].GetText(szTieuDe, sizeof(szTieuDe));
		if (!szTieuDe[0])
			strcpy(szTieuDe, "Chän mét môc");
		KUiWAutoDsach::MoChonMuc(this, nKhe, szTieuDe, m_apDong, n, nCu);
		return;
	}
	KPopupMenuData* pMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(n));
	if (!pMenu)
		return;
	KPopupMenu::InitMenuData(pMenu, n);		// uID = chi so
	pMenu->nNumItem = 0;
	pMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (i = 0; i < n; i++)
	{
		const char* s = m_apDong[i];	// [WAUTO 12/09] danh sach da dung san (loc chieu / loc huong ruong / 141+i)
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
	if (!p || nMuc < 0 || nMuc >= m_nDongMenu)
		return;
	if (p->nKieu == WA_KIEU_CHUOI)
	{	// [WAUTO 12/09] hop "Theo sau": ghi TEN vao szFollName (dong 0 = khong theo ai)
		char* a = (char*)DiaChi(p);
		if (!a || p->nCo <= 0)
			return;
		if (nMuc == 0)
			a[0] = 0;
		else
		{
			strncpy(a, m_apDong[nMuc] ? m_apDong[nMuc] : "", p->nCo - 1);
			a[p->nCo - 1] = 0;
		}
		DienChon(p);
		DaDoi();
		g_DebugLog("[WAUTO-UI] %s = %s", p->szIdc, a);
		return;
	}
	v = m_anGT[nMuc];
	DatInt(p, v);
	DienChon(p);
	if (p->nViec == WA_V_TK_RUONG || MucTheoIdc("IDC_COMBO_9_RH"))
	{	// [WAUTO 12/09] doi THANH se ve -> danh sach "Ruong cua" khac di, doc lai cho khoi lech (nhu WA_NapRuongHuong ben PC)
		const WAUiMuc* pRH = MucTheoIdc("IDC_COMBO_9_RH");
		if (pRH && pRH != p)
			DienChon(pRH);
	}
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
	{	// [WAUTO 12/09] o so lieu nhan vat chay theo game -> dien lai moi 500 ms (nhu WAuto.exe nhan PRG_MAINSYNC)
		unsigned int uNow = (unsigned int)timeGetTime();
		if (uNow >= m_uSongKe)
		{
			m_uSongKe = uNow + 500;
			CapNhatSong();
		}
	}
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
					int v;
					if (p->nViec == WA_V_UUTIEN_GAN)		// [WAUTO 12/09] cap radio cua the PK: chon la DAT han, khong dao
						v = 0;
					else if (p->nViec == WA_V_UUTIEN_NGU)
						v = 1;
					else
						v = LayInt(p) ? 0 : 1;
					DatInt(p, v);
					DienTick();								// doc lai ca tab: o cung cap tu tat
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
		for (i = 0; i < WA_TR_NHAN; i++)
			if (uParam == (KUPARAM)(KWndWindow*)&m_Nhan[i])
			{	// [WAUTO 12/09] cham vao CHU canh o tick: tim o tick nhan nhan nay lam nhan cua no
				if (!m_pTab)
					return 1;
				// [WAUTO 13/09] MOT nhan co the ung voi ca o tick lan hop chon cung hang (hang "[v] Di ban do  [hop chon]":
				// hop chon muon nhan cua tick de lam tieu de bang phu) -> quet HAI LUOT, o tick duoc uu tien.
				for (int k = 0; k < m_pTab->nMuc; k++)
				{
					const WAUiMuc& m = m_pTab->pMuc[k];
					if (m.nKheNhan == i && m.nLoai == WA_MUC_TICK && m.nKhe < WA_TR_TICK && !m_Tick[m.nKhe].IsDisable())
					{
						WndProc(WND_N_BUTTON_CLICK, (KUPARAM)(KWndWindow*)&m_Tick[m.nKhe], 0);
						return 1;
					}
				}
				for (int k = 0; k < m_pTab->nMuc; k++)
				{	// [WAUTO 12/09] cham vao nhan cua hop chon ("Vong sang #1", "Ruong cua"...) = mo hop do
					const WAUiMuc& m = m_pTab->pMuc[k];
					if (m.nKheNhan == i && m.nLoai == WA_MUC_CHON && m.nKhe < WA_TR_CHON && !m_Chon[m.nKhe].IsDisable())
					{
						MoMenuChon(m.nKhe);
						return 1;
					}
				}
				return 1;
			}
		for (i = 0; i < WA_TR_NUT; i++)
			if (uParam == (KUPARAM)(KWndWindow*)&m_Nut[i])
			{
				const WAUiMuc* p = Muc(WA_MUC_NUT, i);	// [WAUTO 12/09] nut thuong
				if (!p)
					p = Muc(WA_MUC_DSACH, i);			// hoac nut "Sua" cua o danh sach
				LamViec(p);
				return 1;
			}
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
#endif // JX_MOBILE
