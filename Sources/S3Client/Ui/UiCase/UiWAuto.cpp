//---------------------------------------------------------------------------
// [ANDROID 11/09 WAUTO B1] Khung Auto trong game - xem UiWAuto.h. Chu Viet trong chuoi: TCVN3 (sinh bang vn_edit.py
// trong android/va_nguon_android_wauto4.py), khong dung Edit/Write thuong.
//---------------------------------------------------------------------------
#include "KWin32.h"
#ifdef JX_ANDROID
#include "KIniFile.h"
#include "KDebug.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "UiWAuto.h"
#include "../../Platform/JxWAutoNoiBo.h"
#include "../Elem/PopupMenu.h"	// [ANDROID 11/09 WAUTO B2 c]
#include <stdio.h>
#include <string.h>

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern "C" int JxCore_WAutoHoatDong(char* szOut, int nMax);	// CoreShell.cpp (chi Android): dong "auto dang lam gi" (TCVN3)

#define SCHEME_INI	"UiWAuto.ini"

// [ANDROID 11/09 WAUTO B2] (nhom, tab con) -> so hieu tab toan cuc (thu tu TEN_TAB cua sinh_bang_wauto.py = thu tu the WAuto.exe): dung de NapTab
static const int s_aNhomTabId[WA_UI_SO_NHOM][WA_UI_TAB_MOI_NHOM] = {
	{ 2, 13, 7, 3, 4, -1, -1, -1 },	// Dieu khien: Chien dau, Chieu KH, PK, Di chuyen, Nhat do
	{ 5, 1, 6, -1, -1, -1, -1, -1 },	// Hau can: Hau can, Phuc hoi, To doi
	{ 0, -1, -1, -1, -1, -1, -1, -1 },	// Cai dat: Co ban
	{ 8, 9, 14, 10, 12, 11, -1, -1 },	// Hoat dong: Da Tau, Tong Kim, CTC, Lien dau, Sat thu, H.dong
};

KUiWAuto* KUiWAuto::m_pSelf = NULL;

// Ten nhom + tab con: dung thu tu s_aNhomTab cua WAutoUI/WAuto.cpp (04/09), bo Ac chinh va D.nhap (mobile mot may mot nhan vat).
static const char* const s_aTenNhom[WA_UI_SO_NHOM] = { "§iÒu khiÓn", "HËu cÇn", "Cµi ®Æt", "Ho¹t ®éng" };
struct WAUiNhom { const char* aTen[WA_UI_TAB_MOI_NHOM]; int nSo; };
static const WAUiNhom s_aNhom[WA_UI_SO_NHOM] = {
	{ { "ChiÕn ®Êu", "Chiªu KH", "PK", "Di chuyÓn", "NhÆt ®å", NULL, NULL, NULL }, 5 },
	{ { "HËu cÇn", "Phôc håi", "Tæ ®éi", NULL, NULL, NULL, NULL, NULL }, 3 },
	{ { "C¬ b¶n", NULL, NULL, NULL, NULL, NULL, NULL, NULL }, 1 },
	{ { "D· TÈu", "Tèng Kim", "CTC", "Liªn ®Êu", "S¸t thñ", "H.®éng", NULL, NULL }, 6 },
};

KUiWAuto::KUiWAuto()
{
	m_nNhom = 0;
	m_nTab = 0;
	m_nBatCu = -1;
	m_uTrangThaiKe = 0;
	m_nAnTam = 0;
}

KUiWAuto* KUiWAuto::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiWAuto;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		int nW = 0, nH = 0;
		m_pSelf->GetSize(&nW, &nH);
		m_pSelf->SetPosition((SCREEN_WIDTH - nW) / 2, (SCREEN_HEIGHT - nH) / 2);	// giua khung ve (1040x604 hay 1188x616 deu vua)
		m_pSelf->m_nBatCu = -1;
		m_pSelf->CapNhatBatTat();
		m_pSelf->CapNhatTrangThai(1);
		m_pSelf->m_Trang.DienGiaTri();	// [ANDROID 11/09 WAUTO B2] cau hinh co the da doi (nhan vat khac / tep .dat moi)
		m_pSelf->Show();
	}
	return m_pSelf;
}

KUiWAuto* KUiWAuto::GetIfVisible()
{
	return (m_pSelf && m_pSelf->IsVisible()) ? m_pSelf : NULL;
}

void KUiWAuto::CloseWindow()
{
	if (m_pSelf)
		m_pSelf->Hide();
}

void KUiWAuto::BatTatCuaSo()
{
	if (GetIfVisible())
		CloseWindow();
	else
		OpenWindow();
}

void KUiWAuto::HuyCuaSo()
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

void KUiWAuto::Initialize()
{
	int i;
	for (i = 0; i < WA_UI_SO_NHOM; i++)
		AddChild(&m_Nhom[i]);
	for (i = 0; i < WA_UI_TAB_MOI_NHOM; i++)
		AddChild(&m_Tab[i]);
	AddChild(&m_BatTat);
	AddChild(&m_TrangThai);
	AddChild(&m_TenTab);
	m_Trang.KhoiTao();				// [ANDROID 11/09 WAUTO B2] kho widget cua trang, roi moi ghep trang vao khung
	AddChild(&m_Trang);
	AddChild(&m_Dong);
	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 128);
	LoadScheme(Scheme);
	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_NORMAL);
	for (i = 0; i < WA_UI_SO_NHOM; i++)
		m_Nhom[i].SetLabel(s_aTenNhom[i]);	// [ANDROID 11/09 WAUTO B1 c] KWndLabeledButton
	m_Dong.SetText("§ãng");
	ChonNhom(0);
}

void KUiWAuto::LoadScheme(const char* pScheme)
{
	char Buff[256];
	char szMuc[32];
	KIniFile Ini;
	int i;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (!Ini.Load(Buff))
	{
		g_DebugLog("[WAUTO-UI] khong doc duoc bo cuc %s", Buff);
		return;
	}
	Init(&Ini, "Main");
	for (i = 0; i < WA_UI_SO_NHOM; i++)
	{
		sprintf(szMuc, "Nhom%d", i);
		m_Nhom[i].Init(&Ini, szMuc);
	}
	for (i = 0; i < WA_UI_TAB_MOI_NHOM; i++)
	{
		sprintf(szMuc, "Tab%d", i);
		m_Tab[i].Init(&Ini, szMuc);
	}
	m_BatTat.Init(&Ini, "BatTat");
	m_TrangThai.Init(&Ini, "TrangThai");
	m_TenTab.Init(&Ini, "TenTab");
	m_Dong.Init(&Ini, "Dong");
}

void KUiWAuto::ChonNhom(int nNhom)
{
	int i;
	if (nNhom < 0 || nNhom >= WA_UI_SO_NHOM)
		return;
	m_nNhom = nNhom;
	for (i = 0; i < WA_UI_SO_NHOM; i++)
		m_Nhom[i].CheckButton(i == nNhom);
	for (i = 0; i < WA_UI_TAB_MOI_NHOM; i++)
	{
		if (i < s_aNhom[nNhom].nSo && s_aNhom[nNhom].aTen[i])
		{
			m_Tab[i].SetLabel(s_aNhom[nNhom].aTen[i]);
			m_Tab[i].Show();
		}
		else
		{
			m_Tab[i].SetLabel("");
			m_Tab[i].Hide();
		}
	}
	ChonTab(0);
}

void KUiWAuto::ChonTab(int nTab)
{
	char sz[80];
	if (nTab < 0 || nTab >= s_aNhom[m_nNhom].nSo)
		return;
	m_nTab = nTab;
	for (int i = 0; i < WA_UI_TAB_MOI_NHOM; i++)
		m_Tab[i].CheckButton(i == nTab);
	// (B1) chua co noi dung: ghi ten tab + loi nhac; B2 thay bang trang chay theo bang
	snprintf(sz, sizeof(sz), "%s  -  %s", s_aNhom[m_nNhom].aTen[nTab], "néi dung tab nµy cã ë b­íc B2");
	m_TenTab.SetText(sz);
	// [ANDROID 11/09 WAUTO B2] nap trang noi dung cua tab nay (bang sinh tu WAuto.rc); co noi dung thi giau dong ten tab
	{
		int nId = s_aNhomTabId[m_nNhom][nTab];
		if (nId >= 0)
		{
			m_Trang.NapTab(nId);
			m_Trang.Show();
			m_TenTab.Hide();
		}
		else
		{
			m_Trang.Hide();
			m_TenTab.Show();
		}
	}
}

void KUiWAuto::CapNhatBatTat()
{
	int nBat = JxWAuto_DangBat() ? 1 : 0;
	if (nBat != m_nBatCu)
	{
		m_nBatCu = nBat;
		m_BatTat.CheckButton(nBat);		// CheckBox: Up = "Bat Auto" (dang tat), Down = "Tat Auto" (dang bat)
	}
}

void KUiWAuto::CapNhatTrangThai(int bEp)
{
	char sz[80];
	char szHD[64];
	unsigned int uNow = timeGetTime();
	if (!bEp && uNow < m_uTrangThaiKe)
		return;
	m_uTrangThaiKe = uNow + 500;
	sz[0] = 0;
	szHD[0] = 0;
	if (JxWAuto_DangBat())
	{
		JxCore_WAutoHoatDong(szHD, sizeof(szHD));
		if (szHD[0])
			snprintf(sz, sizeof(sz), "%s: %s", "§ang bËt", szHD);
		else
			strcpy(sz, "§ang bËt: ®¸nh qu¸i trong tÇm, nhÆt ®å, uèng thuèc");
	}
	else
		strcpy(sz, "Auto ®ang t¾t. Ch¹m nót BËt Auto ®Ó tù ®¸nh, nhÆt ®å, uèng thuèc.");
	m_TrangThai.SetText(sz);
}

void KUiWAuto::Breathe()
{
	CapNhatBatTat();
	CapNhatTrangThai(0);
	m_Trang.Breathe();		// [ANDROID 11/09 WAUTO B2 b] LetMeBreathe chi toi cua so goc
	AnDuoiMenu();
}

// [ANDROID 11/09 WAUTO B2 c] Trinh chieu Android ve anh / chu cua cua so SAU bong menu -> nut nhom / tab / BAT-TAT nam duoi menu chon (dang mo) tam an,
// menu dong thi hien lai. Chay moi nhip (Breathe) nen tre nhieu nhat mot khung hinh.
void KUiWAuto::AnDuoiMenu()
{
	KPopupMenuData* pM = KPopupMenu::GetMenuData();
	int i;
	if (!pM)
	{
		for (i = 0; i < m_nAnTam; i++)
			m_apAnTam[i]->Show();
		m_nAnTam = 0;
		return;
	}
	if (m_nAnTam > 0)
		return;
	int x0 = pM->nX - 2, y0 = pM->nY - 2, x1 = pM->nX + pM->nItemWidth + 2, y1 = pM->nY + pM->nItemHeight * pM->nNumItem + 4;
	KWndWindow* ap[WA_UI_SO_NHOM + WA_UI_TAB_MOI_NHOM + 3];
	int n = 0;
	for (i = 0; i < WA_UI_SO_NHOM; i++)		ap[n++] = &m_Nhom[i];
	for (i = 0; i < WA_UI_TAB_MOI_NHOM; i++)	ap[n++] = &m_Tab[i];
	ap[n++] = &m_BatTat;
	ap[n++] = &m_TrangThai;
	ap[n++] = &m_Dong;
	for (i = 0; i < n; i++)
	{
		int l = 0, t = 0, w = 0, h = 0;
		if (!ap[i]->IsVisible())
			continue;
		ap[i]->GetAbsolutePos(&l, &t);
		ap[i]->GetSize(&w, &h);
		if (l < x1 && l + w > x0 && t < y1 && t + h > y0)
		{
			ap[i]->Hide();
			m_apAnTam[m_nAnTam++] = ap[i];
		}
	}
}

int KUiWAuto::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	if (uMsg == WND_N_BUTTON_CLICK)
	{
		int i;
		if (uParam == (KUPARAM)(KWndWindow*)&m_BatTat)
		{
			JxWAuto_Bat(!JxWAuto_DangBat());
			m_nBatCu = -1;
			CapNhatBatTat();
			CapNhatTrangThai(1);
			g_DebugLog("[WAUTO-UI] nut BAT/TAT -> %d", JxWAuto_DangBat());
			return 1;
		}
		if (uParam == (KUPARAM)(KWndWindow*)&m_Dong)
		{
			Hide();
			return 1;
		}
		for (i = 0; i < WA_UI_SO_NHOM; i++)
			if (uParam == (KUPARAM)(KWndWindow*)&m_Nhom[i])
			{
				ChonNhom(i);
				return 1;
			}
		for (i = 0; i < WA_UI_TAB_MOI_NHOM; i++)
			if (uParam == (KUPARAM)(KWndWindow*)&m_Tab[i])
			{
				ChonTab(i);
				return 1;
			}
	}
	return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
}
#endif // JX_ANDROID
