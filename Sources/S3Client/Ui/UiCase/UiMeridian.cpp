#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "UiMeridian.h"

#include "../ShortcutKey.h"
#include "UiInformation.h"							// [KM 27/08b] UIMessageBox
#include "../../../core/src/coreshell.h"
#include <KTabFile.h>
#include <time.h>								// [KM 27/08b-13] dem nguoc han Khi Doanh
// [KM 27/08] Khong include KCore.h duoc tu thu muc nay (no dung duong dan
// tuong doi ../../Headers/IClient.h, chi giai duoc tu S3Client\). Khai bao thang.
__declspec(dllimport) int  g_AutoLogOn();
__declspec(dllimport) void g_AutoLog(const char* szFmt, ...);
#define KMLOG(...) do { if (g_AutoLogOn()) g_AutoLog(__VA_ARGS__); } while (0)
extern iCoreShell* g_pCoreShell;

#define	SCHEME_INI				"Meridian\\Meridian.ini"
#define	SCHEME_INI_CONFIRM		"Meridian\\MeridianConfirm.ini"
// [KM 27/08] Ban chuan chi ve 16 O VAT LY moi mach; cap 17-32 phu anh cao cap len
// chinh 16 o do (do duoc: 192/192 cap trung vi tri tuyet doi, 192/192 khac anh).
#define	KM_SLOT					(MAX_MERIDIAN_LEVEL / 2)
#define	KM_BREATH				8		// so cham cua trang Khi Doanh Dan Dien
// [KM 27/08b] Bien nhiem vu diem nguyen luc. Chan Nguyen giu 362 (mapping co san
// cua du an - item script da cong don vao 362, doi id la mat diem nguoi choi).
// Huyen Nguyen dung DUNG id chuan 4318 (MAX_TASK da nang 4200 -> 4600).
#define	KM_TASK_CHANNGUYEN		362
#define	KM_TASK_HUYENNGUYEN		4318

KUiMeridian* KUiMeridian::m_pSelf = NULL;
KUiMeridianBuff* KUiMeridianBuff::m_pSelf = NULL;
KUiMeridianConfirm* KUiMeridianConfirm::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´̣¿ª´°¿Ú£¬·µ»ØÎ¨̉»µÄ̉»¸öÀà¶ÔÏóÊµÀư
//--------------------------------------------------------------------------
// ========================= [KM 27/08] KUiMeridianBuff =========================
// "Xem thuoc tinh tang bac Kinh Mach" - noi dung y het qiyingdantian_buff.lua:
//   3 dong theo he nhan vat (ky nang 1501-1505, chua co -> Vo He),
//   8 dong thuong cap 32 (2106-2113) - mach nao dat cap 32 thi hien gia tri.
KUiMeridianBuff* KUiMeridianBuff::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiMeridianBuff;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
		m_pSelf->Show();
	return m_pSelf;
}

void KUiMeridianBuff::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

KUiMeridianBuff* KUiMeridianBuff::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

// chen ten thuoc tinh toi cot 40 roi noi gia tri (nhu strfill_left cua ban chuan)
static void KM_ThemDong(char* pDich, int nMax, const char* pTen, const char* pGiaTri)
{
	char szDong[96];
	int nLen = (int)strlen(pTen);
	if (nLen > 60)
		nLen = 60;
	memcpy(szDong, pTen, nLen);
	while (nLen < 40)
		szDong[nLen++] = ' ';
	szDong[nLen] = 0;
	strcat_s(szDong, pGiaTri);
	strcat_s(szDong, "\n");
	strcat_s(pDich, nMax, szDong);
}

void KUiMeridianBuff::Initialize()
{
	char Scheme[256], Buff[256];
	KIniFile Ini;
	g_UiBase.GetCurSchemePath(Scheme, 256);
	sprintf(Buff, "%s\\%s", Scheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "BuffMain");
		m_TieuDe.Init(&Ini, "BuffTitle");
		m_DauCot.Init(&Ini, "BuffSkillTitle");
		m_DanhSach.Init(&Ini, "BuffSkillList");
		m_Dong.Init(&Ini, "BuffClose");
		DungNoiDung(&Ini);
	}
	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_TOPMOST);
	AddChild(&m_TieuDe);
	AddChild(&m_DauCot);
	AddChild(&m_DanhSach);
	AddChild(&m_Dong);
}

void KUiMeridianBuff::DungNoiDung(KIniFile* pIni)
{
	char szHe[40], szKey[16], szTen[80], szDau[96];
	int i;
	m_TieuDe.SetText("Thu\351c t\335nh t\250ng b\313c Kinh M\271ch");
	strcpy_s(szDau, "T\252n thu\351c t\335nh");
	i = (int)strlen(szDau);
	while (i < 40)
		szDau[i++] = ' ';
	szDau[i] = 0;
	strcat_s(szDau, "Gi\270 tr\336 thu\351c t\335nh");
	m_DauCot.SetText(szDau);

	memset(m_CapMach, 0, sizeof(m_CapMach));
	if (g_pCoreShell)
		g_pCoreShell->GetGameData(GDI_PLAYER_MERIDIAN, (unsigned int)&m_CapMach, sizeof(m_CapMach));
	int nSeries = g_pCoreShell ? g_pCoreShell->GetGameData(GDI_PLAYER_SERIES, 0, 0) : -1;
	strcpy_s(szHe, "h\326 ");
	if (nSeries >= 0 && nSeries <= 4)
	{
		sprintf_s(szKey, "%d_Target", nSeries);
		pIni->GetString("EffectTarget", szKey, "", szHe + strlen(szHe), 24);
	}

	m_szDanhSach[0] = 0;
	// [KM 27/08b-13] 3 dong dau theo TRANG THAI THAT cua buff Khi Doanh (task 4450)
	int nHanKD = g_pCoreShell ? (int)g_pCoreShell->GetGameData(GDI_TASK_SAVE_VALUE, 4450, 0) : 0;
	int bCoKD = (nHanKD - (int)time(NULL)) > 0;
	sprintf_s(szTen, "S\270t th\255\254ng l\252n %s t\250ng: ", szHe);
	KM_ThemDong(m_szDanhSach, sizeof(m_szDanhSach), szTen, bCoKD ? "15%" : "V\253 H\326 ");
	sprintf_s(szTen, "S\270t th\255\254ng t\365 %s gi\266m: ", szHe);
	KM_ThemDong(m_szDanhSach, sizeof(m_szDanhSach), szTen, bCoKD ? "15%" : "V\253 H\326 ");
	KM_ThemDong(m_szDanhSach, sizeof(m_szDanhSach), "Kh\270ng T\312t C\266 (D\255\254ng) t\250ng: ", bCoKD ? "5%" : "V\253 H\326 ");
	static const char* KM_L32_TEN[8] = {
		"C\255\352ng H\343a Ng\362 H\265nh %: T\250ng",
		"Nh\255\356c H\343a Ng\362 H\265nh %: Gi\266m",
		"Sinh L\371c T\350i \247a (D\255\254ng): T\250ng ",
		"Ph\337ng th\361 v\313t l\375 (d\255\254ng) %: T\250ng",
		"B\250ng Ph\337ng (D\255\254ng) %: T\250ng",
		"H\341a Ph\337ng (D\255\254ng) %: T\250ng",
		"L\253i Ph\337ng (D\255\254ng) %: T\250ng",
		"\247\351c Ph\337ng (D\255\254ng) %: T\250ng",
	};
	static const char* KM_L32_TRI[8] = { "200", "200", "15000", "5%", "5%", "5%", "5%", "5%" };
	for (i = 0; i < 8; i++)
	{
		int nDat = (i < MAX_MERIDIAN && m_CapMach[i] >= MAX_MERIDIAN_LEVEL);
		KM_ThemDong(m_szDanhSach, sizeof(m_szDanhSach), KM_L32_TEN[i], nDat ? KM_L32_TRI[i] : "V\253 H\326 ");
	}
	m_DanhSach.SetText(m_szDanhSach);
}

int KUiMeridianBuff::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Dong)
			KUiMeridianBuff::CloseWindow();
		break;
	default:
		return KWndWindow::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

KUiMeridian* KUiMeridian::OpenWindow()
{
	KMLOG("[KM-UI] 01 OpenWindow vao, m_pSelf=%p", (void*)m_pSelf);
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiMeridian;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
		m_pSelf->Show();
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±Ơ´°¿Ú
//--------------------------------------------------------------------------
void KUiMeridian::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

KUiMeridian* KUiMeridian::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//³ơÊ¼»¯
void KUiMeridian::Initialize()
{
	KMLOG("[KM-UI] 02 Initialize vao");
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_NORMAL);
	AddChild(&m_Background);
	AddChild(&m_LevelText);
	AddChild(&m_RankWorldText);
	int i = 0;
	for (i = 0; i < MAX_MERIDIAN+1; i++)
		AddChild(&m_Btn[i]);
	for (i = 0; i < 15; i++) {
		AddChild(&m_Connections[i]);
	}
	for (i = 0; i < MAX_MERIDIAN_LEVEL; i++) {
		AddChild(&m_MerLevelLabel[i]);
		AddChild(&m_Points[i]);
	}
	// [KM 27/08b] DUONG NGANG ra cot ten huyet - da bat lai duoc sau khi va tan goc
	// cu treo (KWndLine khong khoi tao endX/endY, xem WndLine.h). O day CHI dung
	// 16 duong ngang, moi duong deu duoc SetEnd + kep bien truoc khi Show.
	// 15 duong noi cac huyet (m_Connections) VAN de nguyen, khong Init, khong ve.
	for (i = 0; i < KM_SLOT; i++)
		AddChild(&m_NameLine[i]);
	AddChild(&m_Close);
	for (i = 0; i < 3; i++)								// [KM 27/08]
		AddChild(&m_BtnBreath[i]);
	AddChild(&m_BtnViewBuff);
	AddChild(&m_txtCurZY);
	AddChild(&m_txtCurXY);
	AddChild(&m_BreathBuffInfo);
	memset(m_LastLevel, 0xFF, sizeof(m_LastLevel));		// [KM 27/08] ep dung lai trang o lan ve dau
	m_nLastBtn = 0;

	KMLOG("[KM-UI] 03 da them con xong");
	memset(m_MeridianLevel, 0, sizeof(m_MeridianLevel));
	//Get Meridian data
	g_pCoreShell->GetGameData(GDI_PLAYER_MERIDIAN, (unsigned int)&m_MeridianLevel, sizeof(m_MeridianLevel));
}
extern int SCREEN_WIDTH;
//ÔØÈë½çĂæ·½°¸
void KUiMeridian::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (m_pSelf->m_pIni)							// [KM 27/08] doi giao dien -> bo ban cu
		{
			delete m_pSelf->m_pIni;
			m_pSelf->m_pIni = NULL;
		}
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->LoadScheme(&Ini);
			if (SCREEN_WIDTH == 1024) {
			int nX, nY;
			int dX, dY;
			dX = (1024 - 800) / 2 + 8;
			dY = 0;

			m_pSelf->GetPosition(&nX, &nY);
			m_pSelf->SetPosition(nX + dX, nY + dY);
			}
		}
	}
}
void KUiMeridian::LoadScheme(class KIniFile* pIni)
{
	KMLOG("[KM-UI] 04 LoadScheme(ini) vao");
	char	TabChat[32];
	m_Background.Init(pIni, "BackGround");
	int i = 0;
	for (i = 0; i < MAX_MERIDIAN+1; i++)
	{
		sprintf(TabChat, "Btn%d", i+1);
		m_Btn[i].Init(pIni, TabChat);
	}
	m_Close.Init(pIni, "CloseBtn");
	for (i = 0; i < KM_SLOT; i++)						// [KM 27/08b] duong ngang ra ten huyet
		m_NameLine[i].Init(pIni, "NameLine");
	// [KM 27/08] phan Khi Doanh Dan Dien cua ban chuan
	m_BtnBreath[0].Init(pIni, "Btn1DayBreath");
	m_BtnBreath[1].Init(pIni, "Btn7DaysBreath");
	m_BtnBreath[2].Init(pIni, "Btn30DaysBreath");
	m_BtnViewBuff.Init(pIni, "BtnViewMoreBuff");
	m_txtCurZY.Init(pIni, "txtCurZYCount");
	m_txtCurXY.Init(pIni, "txtCurXYCount");
	m_BreathBuffInfo.Init(pIni, "BreathBuffInfo");
	// [KM 27/08b] KHONG SetSize nua: rong 220 lam o chu trum len ca cot nut
	// 1/7/30 ngay + Xem thuoc tinh (x=221). Kich thuoc lay tu ini (x 8..214).
	KMLOG("[KM-UI] 05 LoadScheme(ini) xong");
}

//ÖØĐÂ³ơÊ¼»¯½çĂæ
void KUiMeridian::DefaultScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			int	nValue1, nValue2;
			Ini.GetInteger("Main", "Left", 0, &nValue1);
			Ini.GetInteger("Main", "Top", 0, &nValue2);
			m_pSelf->SetPosition(nValue1, nValue2);
		}
	}
}

void KUiMeridian::UpdateMeridianLevel() {

	//Get Meridian data
	g_pCoreShell->GetGameData(GDI_PLAYER_MERIDIAN, (unsigned int)&m_MeridianLevel, sizeof(m_MeridianLevel));
}
void KUiMeridian::UpdateMeridian()
{
	// [KM 27/08] Chi dung lai trang khi that su doi trang hoac doi cap.
	// Doi trang phai an ngay; con doi cap thi chan toi da 1 lan/giay de
	// khong bao gio co the dung lai lien tuc lam nghen vong ve.
	if (m_nLastBtn != btnNo)
	{
		RebuildPage();
		return;
	}
	if (memcmp(m_LastLevel, m_MeridianLevel, sizeof(m_LastLevel)) != 0)
	{
		int nNow = (int)GetTickCount();
		if (nNow - m_nLastBuild < 1000)
			return;
		RebuildPage();
	}
}

// [KM 27/08] Dung lai trang dang mo dung theo ban chuan client VLTK:
//   - 16 o vat ly; cap 17-32 phu anh diem cao cap len chinh 16 o do
//   - toa do trong ini de NGUYEN VAN nhu trang chuan, cong offset cua [BackGround]
//   - ten huyet dat o 2 cot co dinh LeftX / RightX cua [MerTxt] (khong dat theo do dai chuoi)
//   - keo duong ngang tu huyet ra cot ten
// [KM 27/08] Meridian.ini nay 104 KB / 452 muc. Nap lai moi lan dung se lam DO game
// (nguoi viet truoc da phai chu thich dong Ini.Load trong UpdateMeridian vi ly do nay).
// Nap mot lan roi giu lai; doi giao dien thi LoadScheme se bo ban cu di.
KIniFile* KUiMeridian::LayIni()
{
	if (m_pIni)
		return m_pIni;
	char Scheme[256], Buff[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	sprintf(Buff, "%s\\%s", Scheme, SCHEME_INI);
	KIniFile* p = new KIniFile;
	if (!p || !p->Load(Buff))
	{
		if (p)
			delete p;
		return NULL;
	}
	m_pIni = p;
	return m_pIni;
}

// [KM 27/08] Hai dong duoi cung cua ban chuan: [txtCurZYCount] va [txtCurXYCount].
// Diem lay tu ban sao bien nhiem vu ma may chu da dong bo (UI_TASKVALUE, id >= 256).
void KUiMeridian::CapNhatNguyenLuc()
{
	char szBuf[128];
	int nZY = 0, nXY = 0;
	if (g_pCoreShell)
	{
		nZY = g_pCoreShell->GetGameData(GDI_TASK_SAVE_VALUE, (unsigned int)KM_TASK_CHANNGUYEN, 0);
		nXY = g_pCoreShell->GetGameData(GDI_TASK_SAVE_VALUE, (unsigned int)KM_TASK_HUYENNGUYEN, 0);
	}
	sprintf_s(szBuf, "Ch\251n Nguy\252n hi\326n t\271i: %d \256i\323m", nZY);
	m_txtCurZY.SetText(szBuf);
	sprintf_s(szBuf, "Huy\322n Nguy\252n hi\326n t\271i: %d \256i\323m", nXY);
	m_txtCurXY.SetText(szBuf);
}

// [KM 27/08] Ba dong mo ta trang Khi Doanh Dan Dien (dung nhu ban chuan):
//   duy tri / sat thuong len he X tang / sat thuong gay ra boi he X giam.
// Ten he doc tu [EffectTarget] theo he ngu hanh nhan vat. Gia tri con 'Khong'
// vi bo ky nang 1501-1505 (mua bang nut 1/7/30 ngay) thuoc dot may chu.
void KUiMeridian::DungChuKhiDoanh(KIniFile* pIni)
{
	// [KM 27/08b-13] Ba dong hien TRANG THAI THAT cua buff (nhu ban chuan dung
	// GetSkillState voi ky nang 1501-1505). Du an luu HAN o bien nhiem vu 4450,
	// may chu tu dong bo xuong (UI_TASKVALUE) nen client doc bang GDI_TASK_SAVE_VALUE.
	char szHe[32], szKey[16], szDong[192], szHan[64];
	int nSeries = g_pCoreShell ? g_pCoreShell->GetGameData(GDI_PLAYER_SERIES, 0, 0) : -1;
	szHe[0] = 0;
	if (nSeries >= 0 && nSeries <= 4)
	{
		sprintf_s(szKey, "%d_Target", nSeries);
		pIni->GetString("EffectTarget", szKey, "", szHe, 32);
	}
	int nHan = g_pCoreShell ? (int)g_pCoreShell->GetGameData(GDI_TASK_SAVE_VALUE, 4450, 0) : 0;
	int nConLai = nHan - (int)time(NULL);
	if (nConLai > 0)
	{
		int nNgay = nConLai / 86400;
		int nGio = (nConLai - nNgay * 86400) / 3600;
		int nPhut = (nConLai - nNgay * 86400 - nGio * 3600) / 60;
		if (nNgay > 0)
			sprintf_s(szHan, "%d ng\265y %d gi\352", nNgay, nGio);
		else
			sprintf_s(szHan, "%d gi\352 %d ph\363t", nGio, nPhut);
	}
	else
		strcpy_s(szHan, "Kh\253ng");
	const char* szMuc = (nConLai > 0) ? "15%" : "Kh\253ng";
	m_szBreathInfo[0] = 0;
	sprintf_s(szDong, "Kh\335 Doanh \247an \247i\322n duy tr\327: %s", szHan);
	strcat_s(m_szBreathInfo, szDong);
	strcat_s(m_szBreathInfo, "\n\n");
	sprintf_s(szDong, "S\270t th\255\254ng l\252n h\326 %s t\250ng: %s", szHe, szMuc);
	strcat_s(m_szBreathInfo, szDong);
	strcat_s(m_szBreathInfo, "\n\n");
	sprintf_s(szDong, "S\270t th\255\254ng g\251y ra b\353i h\326 %s gi\266m: %s", szHe, szMuc);
	strcat_s(m_szBreathInfo, szDong);
	m_BreathBuffInfo.SetText(m_szBreathInfo);
}

void KUiMeridian::RebuildPage()
{
	char		Buff1[128], Mer[32], Key[32];
	int			left = 0, top = 0, LR = 0, i = 0;

	KMLOG("[KM-UI] 06 RebuildPage vao, btnNo=%d", btnNo);
	KIniFile* pIni = LayIni();						// [KM 27/08] khong nap lai moi lan nua
	if (!pIni)
		return;
	KIniFile& Ini = *pIni;

	int nBgLeft = 0, nBgTop = 0, nLeftX = 75, nRightX = 220, nTxtW = 70;
	Ini.GetInteger("BackGround", "Left", 0, &nBgLeft);
	Ini.GetInteger("BackGround", "Top", 0, &nBgTop);
	Ini.GetInteger("MerTxt", "LeftX", 75, &nLeftX);
	Ini.GetInteger("MerTxt", "RightX", 220, &nRightX);
	Ini.GetInteger("MerTxt", "Width", 70, &nTxtW);

	KMLOG("[KM-UI] 07 da nap ini xong");
	int nSlot = (btnNo == 1) ? KM_BREATH : KM_SLOT;
	int nLv = (btnNo >= 2 && btnNo - 2 < MAX_MERIDIAN) ? m_MeridianLevel[btnNo - 2] : 0;
	int nLop1 = (nLv > KM_SLOT) ? KM_SLOT : nLv;		// so o sang o lop thuong
	int nLop2 = (nLv > KM_SLOT) ? nLv - KM_SLOT : 0;	// so o sang o lop cao cap

	for (i = 0; i < MAX_MERIDIAN_LEVEL; i++)
	{
		m_Points[i].Hide();
		m_MerLevelLabel[i].Hide();
	}
	for (i = 0; i < KM_SLOT; i++)						// [KM 27/08b]
		m_NameLine[i].Hide();

	KMLOG("[KM-UI] 08 an het xong, nSlot=%d nLv=%d nLop1=%d nLop2=%d", nSlot, nLv, nLop1, nLop2);
	for (i = 0; i < nSlot; i++)
	{
		KMLOG("[KM-UI] 09 o %d: bat dau", i);
		sprintf_s(Mer, "Mer%dPoint_%d", btnNo, i);
		m_Points[i].Init(&Ini, Mer);
		m_Points[i].GetPosition(&left, &top);
		left += nBgLeft;
		top += nBgTop;
		m_Points[i].SetPosition(left, top);
		m_Points[i].SetFrame(i < nLop1 ? 1 : 0);
		m_Points[i].Show();
		KMLOG("[KM-UI] 10 o %d: lop 1 xong", i);

		int j = KM_SLOT + i;
		if (j < MAX_MERIDIAN_LEVEL && i < nLop2)
		{
			int l2 = 0, t2 = 0;
			sprintf_s(Mer, "Mer%dPointP2_%d", btnNo, i);
			m_Points[j].Init(&Ini, Mer);
			m_Points[j].GetPosition(&l2, &t2);
			m_Points[j].SetPosition(l2 + nBgLeft, t2 + nBgTop);
			m_Points[j].SetFrame(1);
			m_Points[j].Show();
		}

		KMLOG("[KM-UI] 11 o %d: lop 2 xong", i);
		if (btnNo == 1)
			continue;									// trang Khi Doanh Dan Dien khong co ten huyet

		sprintf_s(Mer, "Mer%dLR", btnNo);
		sprintf_s(Key, "Acup_%d", i);
		Ini.GetInteger(Mer, Key, 0, &LR);

		sprintf_s(Mer, "Mer%dName", btnNo);
		sprintf_s(Key, "Name_%d", (nLv >= KM_SLOT) ? (i + KM_SLOT) : i);
		Ini.GetString(Mer, Key, "", Buff1, 128);
		if (Buff1[0] == 0)
			continue;

		m_MerLevelLabel[i].Init(&Ini, "MerTxt");
		m_MerLevelLabel[i].SetText(Buff1);
		int nTxtX = nBgLeft + (LR ? nRightX : nLeftX);
		m_MerLevelLabel[i].SetPosition(nTxtX, top - 2);
		m_MerLevelLabel[i].Show();

		// [KM 27/08b] duong ngang noi cham huyet voi ten (nhu ban goc). Toa do luon
		// duoc tinh va kep bien; KHONG bao gio de KWndLine chay voi endX chua dat.
		{
			int x1 = LR ? (left + 10) : (nTxtX + nTxtW);
			int x2 = LR ? nTxtX : left;
			int dx = x2 - x1;
			if (dx > 3 || dx < -3)
			{
				if (dx > 512)
					dx = 512;
				if (dx < -512)
					dx = -512;
				m_NameLine[i].SetPosition(x1, top + 5);
				m_NameLine[i].SetEnd(dx, 0);
				m_NameLine[i].SetColor(i < nLop1 ? 0xff8ec4ac : 0xff5a6b64);
				m_NameLine[i].Show();
			}
		}

		KMLOG("[KM-UI] 12 o %d: ten + duong ngang xong", i);
	}

	// [KM 27/08] 4 nut + o mo ta chi thuoc trang Khi Doanh Dan Dien
	for (i = 0; i < 3; i++)
	{
		if (btnNo == 1) m_BtnBreath[i].Show(); else m_BtnBreath[i].Hide();
	}
	if (btnNo == 1)
	{
		m_BtnViewBuff.Show();
		DungChuKhiDoanh(pIni);
		m_BreathBuffInfo.Show();
	}
	else
	{
		m_BtnViewBuff.Hide();
		m_BreathBuffInfo.Hide();
		KUiMeridianBuff::CloseWindow();				// roi trang thi dong cua so xem
	}
	KMLOG("[KM-UI] 13 xong het vong, sap cap nhat diem");
	CapNhatNguyenLuc();
	m_txtCurZY.Show();
	m_txtCurXY.Show();

	KMLOG("[KM-UI] 14 RebuildPage XONG btnNo=%d", btnNo);
	m_nLastBtn = btnNo;
	m_nLastBuild = (int)GetTickCount();				// [KM 27/08]
	memcpy(m_LastLevel, m_MeridianLevel, sizeof(m_LastLevel));
}

void KUiMeridian::Breathe()
{
	int currentTick = GetTickCount();
	if (currentTick - this->lastTick >= 200) {
		this->lastTick = currentTick;
		UpdateData();
		UpdateMeridianLevel();							// [KM 27/08] lay cap moi truoc khi so
		UpdateMeridian();
		CapNhatNguyenLuc();								// [KM 27/08] diem co the doi bat cu luc nao
		// [KM 27/08b-13] lam moi trang thai Khi Doanh 1 lan/giay (dem nguoc thoi han)
		if (btnNo == 1 && (int)GetTickCount() - m_nTinDenHan > 0)
		{
			m_nTinDenHan = (int)GetTickCount() + 1000;
			KIniFile* pI = LayIni();
			if (pI)
				DungChuKhiDoanh(pI);
		}
	}
}

// [KM 27/08b] Dat cua so xung huyet NGAY CANH bang kinh mach (nhu chu game
// muon), thay vi de chong len nhau. Uu tien canh phai; neu tran man hinh thi
// lat sang canh trai; het duong thi ep vao trong man hinh.
static void KM_DatCuaSoKeBen(KWndWindow* pChinh, KWndWindow* pPhu)
{
	if (pChinh == NULL || pPhu == NULL)
		return;
	int nL = 0, nT = 0, nW = 0, nH = 0, nWp = 0, nHp = 0;
	pChinh->GetPosition(&nL, &nT);
	pChinh->GetSize(&nW, &nH);
	pPhu->GetSize(&nWp, &nHp);
	if (nWp <= 0)
		nWp = 318;
	int nX = nL + nW + 2;
	if (nX + nWp > SCREEN_WIDTH)
		nX = nL - nWp - 2;						// khong du cho ben phai -> lat sang trai
	if (nX < 0)
		nX = 0;
	pPhu->SetPosition(nX, nT + 56);				// ngang hang vung ve cua bang mach
}

int KUiMeridian::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	KIniFile	Ini;
	char		Buff[128];
	char		Buff1[128];
	char Scheme[256];
	char	Mer[32];
	char	Key[32];
	int left, top;
	int i = 0;
	int LR = 0; //0 L, 1 R
	switch (uMsg)
	{
	case WND_M_MENUITEM_SELECTED:
		break;
	case WND_N_CHILD_MOVE:
	case WND_N_LIST_ITEM_SEL:
		break;
	case WND_M_POPUPMENU:
		break;
	case WND_N_BUTTON_CLICK:
	{
		//
		for (i = 0; i < MAX_MERIDIAN + 1; i++) {
			if (uParam == (unsigned int)(KWndWindow*)&m_Btn[i])
			{
				m_Btn[i].CheckButton(true);
				btnNo = i+1;
			}
			else {
				m_Btn[i].CheckButton(false);
			}
		}

		// [KM 27/08] dung lai trang theo ban chuan (16 o + lop cao cap + ten 2 cot + duong ngang)
		RebuildPage();
		//Xy ly khi click
		KIniFile* pIniC = LayIni();						// [KM 27/08] dung ban da nap
		if (btnNo >= 2 && pIniC) {
			KIniFile& Ini = *pIniC;
			// [KM 27/08] 16 o vat ly: mach da du 16 cap thi bam o i la mo cap i+17
			int nLvCur = m_MeridianLevel[btnNo - 2];
			for (i = 0; i < KM_SLOT; i++) {
				int nMuc = (nLvCur >= KM_SLOT) ? (i + 1 + KM_SLOT) : (i + 1);
				if (uParam == (unsigned int)(KWndWindow*)&m_Points[i] && nMuc > nLvCur) {
					if (KUiMeridianConfirm::GetIfVisible())
					{
						KUiMeridianConfirm::CloseWindow();	// [KM 27/08b] khong con day bang mach
					}
					else
					{
						KUiMeridianConfirm* pself = KUiMeridianConfirm::OpenWindow();
						KM_DatCuaSoKeBen(this, pself);		// [KM 27/08b] nam ke ben, khong chong
						char titleBuff[256];
						pself->SetMeridian(btnNo - 2, nMuc);		// [KM 27/08]
						sprintf_s(Mer, "Mer%dName", btnNo); //Huyet vi
						sprintf_s(Key, "Name_%d", nMuc - 1);		// [KM 27/08]
						Ini.GetString(Mer, Key, "", Buff1, 128);

						sprintf_s(Mer, "Btn%d", btnNo);				//Mach
						Ini.GetString(Mer, "Label", "", Buff, 128);
						sprintf_s(titleBuff, "\247\266 Th\253ng Huy\326t %s", Buff1);
						pself->SetTitle(titleBuff);
						pself->SetInfo();
					}
				}
			}
		}
		// [KM 27/08] 4 nut trang Khi Doanh Dan Dien (theo game_y.exe 0x4870C0):
		// 1/7/30 ngay - dieu kien chuan: ca 8 mach dau dat cap >= 16.
		// Goi 0xB4 + bo ky nang 1501-1505 thuoc dot may chu - ghi log cho vet.
		if (uParam == (unsigned int)(KWndWindow*)&m_BtnViewBuff)
		{
			if (KUiMeridianBuff::GetIfVisible())
				KUiMeridianBuff::CloseWindow();
			else
				KUiMeridianBuff::OpenWindow();
		}
		for (i = 0; i < 3; i++)
		{
			if (uParam == (unsigned int)(KWndWindow*)&m_BtnBreath[i])
			{
				int nDu = 1, j;
				for (j = 0; j < 8; j++)
				{
					if (m_MeridianLevel[j] < KM_SLOT)
						nDu = 0;
				}
				KMLOG("[KM-UI] KDDD nut %d ngay, du dieu kien 8 mach>=16: %d",
					i == 0 ? 1 : (i == 1 ? 7 : 30), nDu);
				// [KM 27/08b] truoc day bam xong im lang. Nay bao ro dieu kien + chi phi
				// doc tu [BreathCosts] cua chinh tep chuan.
				{
					char szKeyC[24], szTin[400];
					int nNgay = 0, nGia = 0, nHon = 0;
					KIniFile* pIC = LayIni();
					if (pIC)
					{
						sprintf_s(szKeyC, "%d_Day", i);
						pIC->GetInteger("BreathCosts", szKeyC, 0, &nNgay);
						sprintf_s(szKeyC, "%d_Pill", i);
						pIC->GetInteger("BreathCosts", szKeyC, 0, &nGia);
						sprintf_s(szKeyC, "%d_Soul", i);
						pIC->GetInteger("BreathCosts", szKeyC, 0, &nHon);
					}
					char szPill[48], szSoul[48];
					szPill[0] = szSoul[0] = 0;
					if (pIC)
					{
						pIC->GetString("BreathCosts", "PillName", "\256i\323m Ch\251n Nguy\252n", szPill, 48);
						pIC->GetString("BreathCosts", "SoulName", "Kh\335 Doanh \247an", szSoul, 48);
					}
					if (!nDu)
						sprintf_s(szTin, "Ch\255a m\353 \256\255\356c Kh\335 Doanh \247an \247i\322n. \247i\322u ki\326n: c\266 8 kinh m\271ch \256\307u ph\266i \256\271t c\312p 16 tr\353 l\252n.");
					else
						sprintf_s(szTin, "G\343i %d ng\265y. Nguy\252n li\326u c\307n: %d %s v\265 %d %s. Kh\251u tr\365 nguy\252n li\326u \256ang ch\352 m\353 \353 \256\356t m\270y ch\361.", nNgay, nGia, szPill, nHon, szSoul);
					UIMessageBox(szTin);
					// [KM 27/08b-13] KHONG dong den 3 dong trang Khi Doanh khi bam nut
					// (y chu game). Ket qua mua do MAY CHU bao ve khung chat.
					// [KM 27/08b] GUI YEU CAU MUA THAT (ma nguong 100+goi; may chu kiem
					// dieu kien va tru nguyen lieu, roi bao ket qua ve khung chat).
					if (nDu && g_pCoreShell)
					{
						SetMeridianData Mua;
						Mua.WayProtected = 0;
						Mua.WayEnhanced = 0;
						Mua.Type = 100 + i;
						Mua.Level = 0;
						g_pCoreShell->OperationRequest(GOI_SET_PLAYER_MERIDIAN, (unsigned int)&Mua, 0);
					}
				}
			}
		}
		m_Btn[btnNo-1].CheckButton(true);
		if(uParam == (unsigned int)(KWndWindow*)&m_Close)
			KUiMeridian::CloseWindow();
		break;
	}
	default:
		return KWndWindow::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

KUiMeridianConfirm* KUiMeridianConfirm::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiMeridianConfirm;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
		m_pSelf->Show();
	return m_pSelf;
}

void KUiMeridianConfirm::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

KUiMeridianConfirm* KUiMeridianConfirm::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

void KUiMeridianConfirm::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_CONFIRM);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->LoadScheme(&Ini);
		}
	}
}

void KUiMeridianConfirm::DefaultScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_CONFIRM);
		if (Ini.Load(Buff))
		{
			int	nValue1, nValue2;
			Ini.GetInteger("Main", "Left", 0, &nValue1);
			Ini.GetInteger("Main", "Top", 0, &nValue2);
			m_pSelf->SetPosition(nValue1, nValue2);
		}
	}
}

// [KM 27/08] Cach mac dinh theo GetDefaultWay cua client.lua chuan:
//   cap <= 16 va mach 3 hoac 4 (Xung mach / Doi mach) -> duong Dai Ho Mach Don.
void KUiMeridianConfirm::SetMeridian(int Type, int Level)
{
	m_nType = Type;
	m_nLevel = Level;
	if (Level <= 16 && (Type + 1 == 3 || Type + 1 == 4))
		m_nWay = 3;
	else
		m_nWay = 0;
}

void KUiMeridianConfirm::Initialize()
{
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_TOPMOST);
	AddChild(&Title);
	AddChild(&m_Info);
	AddChild(&m_cbWay[0]);
	AddChild(&m_cbWay[1]);
	AddChild(&m_cbWay[2]);
	AddChild(&m_imgWay[0]);
	AddChild(&m_imgWay[1]);
	AddChild(&m_imgWay[2]);
	AddChild(&m_btnOk);
	AddChild(&m_btnOkOne);
	AddChild(&m_btnClose);
}

void KUiMeridianConfirm::Breathe()
{
}

// doc mot khoi so lieu "a,b,c,..." tu ini
static int KM_DocCSV(KIniFile* pIni, const char* pSec, const char* pKey, int* pRa, int nSo)
{
	char Buff[96];
	pIni->GetString(pSec, pKey, "", Buff, 96);
	if (Buff[0] == 0)
		return 0;
	int n = 0;
	char* p = Buff;
	while (n < nSo)
	{
		pRa[n++] = atoi(p);
		p = strchr(p, ',');
		if (!p)
			break;
		p++;
	}
	return n;
}

// [KM 27/08b] Bang MeridianProtect cua ban chuan (common.lua client VLTK): duong
// "Huyet vi bao ve" chi mo cho mach 1,2 (nhom A) va 5,6,7,8 (nhom B); moi (mach,
// cap) dinh san CAP cua Huyet Long Dang, so luong luon 10 cai. 0 = vo hieu.
static int KM_ProtectCap(int nMach, int nCap)
{
	static const int nNhomA[17] = { 0, 0,1,2,3,4,5,6,7,8,9, 0,0,12, 0,0,0 };
	static const int nNhomB[17] = { 0, 0,0,0,0, 4,5,6,2,8,9, 0,11,12, 0,14,0 };
	if (nCap < 1 || nCap > 16)
		return 0;
	if (nMach == 1 || nMach == 2)
		return nNhomA[nCap];
	if (nMach >= 5 && nMach <= 8)
		return nNhomB[nCap];
	return 0;
}

// [KM 27/08b] Mat na duong Long Hon Ho The cua ban chuan: tbAvailableMeridian
// {1,2,5,6,7,8} va tbAvailableLevel {3,4,5,6,7,8,15,16} (client.lua chuan).
static int KM_EnhanceMach(int nMach)
{
	return (nMach == 1 || nMach == 2 || (nMach >= 5 && nMach <= 8));
}
static int KM_EnhanceCap(int nCap)
{
	return ((nCap >= 3 && nCap <= 8) || nCap == 15 || nCap == 16);
}

void KUiMeridianConfirm::LoadScheme(KIniFile* pIni)
{
	char Sec[24], Key[16];
	int i, j;
	Title.Init(pIni, "txtTitle");
	m_Info.Init(pIni, "ListInfo");
	m_cbWay[0].Init(pIni, "cbNormalWay");
	m_cbWay[1].Init(pIni, "cbProtectionWay");
	m_cbWay[2].Init(pIni, "cbEnhanceRateWay");
	m_btnOk.Init(pIni, "btnOk");
	m_btnOkOne.Init(pIni, "btnOk_OnlyOneWay");
	m_btnClose.Init(pIni, "btnClose");

	// [KM 27/08b] O DANH DAU nhu ban chuan: anh tick 3 khung doc tu chinh cac khoa
	// StatusImage/Status*Frame/ImageWidth/ImageHeight cua muc cb (tep chuan co san
	// trong MeridianConfirm.ini). Khuon giong UiOptions2 cua chinh du an.
	{
		static const char* KM_CB[3] = { "cbNormalWay", "cbProtectionWay", "cbEnhanceRateWay" };
		char szAnh[128];
		int nL, nT, nCao, nAW, nAH;
		pIni->GetInteger(KM_CB[0], "StatusEnableFrame", 0, &m_nTickCo);
		pIni->GetInteger(KM_CB[0], "StatusDisableFrame", 1, &m_nTickKhong);
		pIni->GetInteger(KM_CB[0], "StatusInvalidFrame", 2, &m_nTickXam);
		for (i = 0; i < 3; i++)
		{
			szAnh[0] = 0; nL = nT = 0; nCao = 19; nAW = 14; nAH = 14;
			pIni->GetString(KM_CB[i], "StatusImage", "", szAnh, 128);
			pIni->GetInteger(KM_CB[i], "Left", 0, &nL);
			pIni->GetInteger(KM_CB[i], "Top", 0, &nT);
			pIni->GetInteger(KM_CB[i], "Height", 19, &nCao);
			pIni->GetInteger(KM_CB[i], "ImageWidth", 14, &nAW);
			pIni->GetInteger(KM_CB[i], "ImageHeight", 14, &nAH);
			if (szAnh[0])
			{
				m_imgWay[i].SetImage(ISI_T_SPR, szAnh);
				m_imgWay[i].SetPosition(nL, nT + (nCao - nAH) / 2);
				m_imgWay[i].SetSize(nAW, nAH);
				// chu nhich phai chua cho anh tick (ImageAlignRight=0 cua chuan)
				m_cbWay[i].SetPosition(nL + nAW + 2, nT);
			}
		}
	}

	
	// [KM 27/08] so lieu chuan (sinh tu meridian_data.lua) - doc mot lan
	memset(m_TyLeLuot, 0, sizeof(m_TyLeLuot));
	memset(m_HuyetLong, 0, sizeof(m_HuyetLong));
	memset(m_DaiHoMach, 0, sizeof(m_DaiHoMach));
	for (i = 0; i < 2; i++)
	{
		sprintf_s(Sec, "TyLeLuot%d", i);
		for (j = 17; j <= 32; j++)
		{
			sprintf_s(Key, "Cap%d", j);
			// [KM 27/08b] giu DU: [0]=so muc doc duoc, [1..5]=ty le theo so lan hong
			m_TyLeLuot[i][j][0] = KM_DocCSV(pIni, Sec, Key, &m_TyLeLuot[i][j][1], 5);
		}
	}
	for (i = 1; i <= 8; i++)
	{
		sprintf_s(Sec, "HuyetLong%d", i);
		for (j = 1; j <= 16; j++)
		{
			sprintf_s(Key, "Lv%d", j);
			KM_DocCSV(pIni, Sec, Key, m_HuyetLong[i][j], 7);
		}
	}
	for (j = 1; j <= 16; j++)
	{
		sprintf_s(Key, "Lv%d", j);
		KM_DocCSV(pIni, "DaiHoMach", Key, m_DaiHoMach[j], 3);
	}
}

void KUiMeridianConfirm::SetTitle(char* title)
{
	Title.SetText(title);
}

// [KM 27/08] Chu mo ta: dung tung cau lang.lua + luong client.lua cua ban chuan.
void KUiMeridianConfirm::SetInfo()
{
	KTabFile MeridiantSetting;
	char szInfo[1000], szDong[192], szNho[64];
	int RollbackLevel = 0, Rate = 0, ZY = 0, HMD = 0, DHMD = 0;
	int nMach = m_nType + 1;
	int nMoi = (nMach >= 9) ? 1 : 0;

	MeridiantSetting.Load(MERIDIAN_SETTING_FILE);
	int nDong = (m_nType) * MAX_MERIDIAN_LEVEL + m_nLevel + 1;
	MeridiantSetting.GetInteger(nDong, 4, 0, &RollbackLevel);
	MeridiantSetting.GetInteger(nDong, 5, 0, &Rate);
	MeridiantSetting.GetInteger(nDong, 10, 0, &ZY);
	MeridiantSetting.GetInteger(nDong, 11, 0, &HMD);
	MeridiantSetting.GetInteger(nDong, 12, 0, &DHMD);

	const char* tenZY = nMoi ? "Huy\322n Nguy\252n" : "Ch\251n Nguy\252n";
	const char* tenHMD = nMoi ? "\247\336nh M\271ch \247an" : "H\351 m\271ch \256\254n";
	const char* tenDHMD = nMoi ? "\247\336nh M\271ch \247an (Lv1)" : "\247\271i H\351 M\271ch \247\254n (C\312p 1)";

	szInfo[0] = 0;
	switch (m_nWay)
	{
	case 1:		// Huyet vi bao ve - GetProtectLevelUpTips
	{
		int nCapDang = KM_ProtectCap(nMach, m_nLevel);	// [KM 27/08b] bang MeridianProtect chuan
		if (Rate >= 10000 || nCapDang <= 0)
		{
			strcpy_s(szInfo, "Gi\367 huy\326t v\336 kh\253ng th\323 v\253 hi\326u v\355i huy\326t v\336 n\265y");
			break;
		}
		sprintf_s(szDong, "Xung m\271ch l\307n n\265y c\307n ti\252u hao %s: %d \256i\323m", tenZY, ZY);
		strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
		sprintf_s(szDong, "Xung m\271ch l\307n n\265y c\307n ti\252u hao %s: %d c\270i", tenHMD, HMD);
		strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
		sprintf_s(szDong, "L\307n xung huy\326t n\265y ng\255\254i c\307n ti\252u hao %s[c\312p %d]: %d c\270i", "Huy\325t Long \247\273ng", nCapDang, 10);
		strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
		sprintf_s(szDong, "T\330 l\326 th\265nh c\253ng l\307n xung huy\326t n\265y l\265: %d%%", Rate / 100);
		strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
		sprintf_s(szDong, "L\307n xung huy\326t n\265y c\361a c\270c h\271 th\312t b\271i l\265: %s", "Duy tr\327 huy\326t v\336 hi\326n t\271i.");
		strcat_s(szInfo, szDong);
		break;
	}
	case 2:		// Long Hon Ho The - tbEhanceRateWay:GetLevelUpTips
	{
		// [KM 27/08b] 3 lop mat na cua client.lua chuan; KM_EnhanceCap dong thoi chan
		// OOB (m_HuyetLong chi co cap 1..16, cap chuan mo cung chi 3-8/15/16).
		if (!KM_EnhanceMach(nMach))
		{
			strcpy_s(szInfo, "Long H\345n H\351 Th\323 \256\350i v\355i kinh m\271ch n\265y v\253 hi\326u.");
			break;
		}
		if (!KM_EnhanceCap(m_nLevel))
		{
			strcpy_s(szInfo, "Long H\345n H\351 Th\323 ch\255a m\353 \256\350i v\355i hi\326u \370ng c\361a huy\326t v\336 n\265y.");
			break;
		}
		int* hl = m_HuyetLong[nMach][m_nLevel];
		if (hl[6] <= 0 || hl[3] < 0)
		{
			strcpy_s(szInfo, "Long H\345n H\351 Th\323 \256\350i v\355i huy\326t v\336 n\265y v\253 hi\326u.");
			break;
		}
		sprintf_s(szDong, "Xung m\271ch l\307n n\265y c\307n ti\252u hao %s: %d \256i\323m", tenZY, hl[4]);
		strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
		sprintf_s(szDong, "Xung m\271ch l\307n n\265y c\307n ti\252u hao %s: %d c\270i", tenHMD, hl[5]);
		strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
		if (hl[1] > 0)
		{
			sprintf_s(szDong, "L\307n xung huy\326t n\265y ng\255\254i c\307n ti\252u hao %s[c\312p %d]: %d c\270i", "Huy\325t Long \247\273ng", hl[0], hl[1]);
			strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
		}
		if (hl[3] > 0)
		{
			sprintf_s(szDong, "L\307n xung huy\326t n\265y ng\255\254i c\307n ti\252u hao %s[c\312p %d]: %d c\270i", "Huy\325t Long \247\254n", hl[2], hl[3]);
			strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
		}
		sprintf_s(szDong, "T\330 l\326 th\265nh c\253ng l\307n xung huy\326t n\265y l\265: %d%%", hl[6] / 100);
		strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
		if (hl[6] < 10000)
		{
			sprintf_s(szDong, "L\307n xung huy\326t n\265y c\361a c\270c h\271 th\312t b\271i l\265: %s", "Duy tr\327 huy\326t v\336 hi\326n t\271i.");
			strcat_s(szInfo, szDong);
		}
		break;
	}
	case 3:		// duong Dai Ho Mach Don - tbDaHuMaiDanWay:GetLevelUpTips
	{
		int* dh = m_DaiHoMach[(m_nLevel >= 1 && m_nLevel <= 16) ? m_nLevel : 1];
		sprintf_s(szDong, "Xung m\271ch l\307n n\265y c\307n ti\252u hao %s: %d \256i\323m", tenZY, dh[0]);
		strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
		sprintf_s(szDong, "l\307n xung huy\326t n\265y c\270c h\271 c\307n ph\266i ti\252u hao \247\271i H\351 M\271ch \247\254n: %d c\270i", dh[1] / 1000);
		strcat_s(szInfo, szDong);
		strcat_s(szInfo, ", ho\306c l\265 H\351 M\271ch \247\254n b\273ng nhau."); strcat_s(szInfo, "\n\n");
		sprintf_s(szDong, "T\330 l\326 th\265nh c\253ng l\307n xung huy\326t n\265y l\265: %d%%", dh[2] / 100);
		strcat_s(szInfo, szDong);
		break;
	}
	default:	// Pho thong - GetNormalLevelUpTips
	{
		if (m_nLevel > 16)
		{
			// [KM 27/08b] so lan that bai (bao day) - so do KM_BAODAY chuan: mach 1-4
			// -> task 4440 byte 1-4; mach 5-8 -> 4441; mach 9-12 -> 4491. May chu dong
			// bo cac task nay xuong qua UI_TASKVALUE (SetSaveVal -> SyncTaskValueToClient).
			static const int KM_BD_TASK[13] = { 0, 4440,4440,4440,4440, 4441,4441,4441,4441, 4491,4491,4491,4491 };
			static const int KM_BD_POS[13]  = { 0, 1,2,3,4, 1,2,3,4, 1,2,3,4 };
			int nLan = 0;
			if (g_pCoreShell && nMach >= 1 && nMach <= 12)
			{
				int nVal = (int)g_pCoreShell->GetGameData(GDI_TASK_SAVE_VALUE, (unsigned int)KM_BD_TASK[nMach], 0);
				nLan = (nVal >> (8 * (KM_BD_POS[nMach] - 1))) & 0xFF;
			}
			int nSoMuc = m_TyLeLuot[nMoi][m_nLevel][0];
			if (nSoMuc < 1)
				nSoMuc = 1;
			if (nLan > nSoMuc - 1)
				nLan = nSoMuc - 1;
			// cong thuc chuan: GetBaoDiBaseRate(muc 0) + GetBaoDiExtraRate(muc n - muc 0)
			int nGoc = m_TyLeLuot[nMoi][m_nLevel][1] / 100;
			int nThem = m_TyLeLuot[nMoi][m_nLevel][nLan + 1] / 100 - nGoc;
			int nR = nGoc;
			sprintf_s(szDong, "Xung m\271ch l\307n n\265y t\350n %s: %d \12\12 Xung huy\326t l\307n n\265y t\350n %s: %d \256i\323m", tenDHMD, DHMD, tenZY, ZY);
			strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
			sprintf_s(szDong, "X\270c su\312t th\265nh c\253ng m\306c \256\336nh c\361a Xung Huy\326t l\307n n\265y: %d%%", nR);
			strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
			sprintf_s(szDong, "X\270c su\312t th\265nh c\253ng t\250ng th\252m c\361a Xung Huy\326t l\307n n\265y: %d%%", nThem);
			strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
			sprintf_s(szDong, "X\270c su\312t th\265nh c\253ng c\361a Xung Huy\326t l\307n n\265y: %d%%", nGoc + nThem);
			strcat_s(szInfo, szDong);
		}
		else
		{
			sprintf_s(szDong, "Xung m\271ch l\307n n\265y c\307n ti\252u hao %s: %d \256i\323m", tenZY, ZY);
			strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
			sprintf_s(szDong, "Xung m\271ch l\307n n\265y c\307n ti\252u hao %s: %d c\270i", tenHMD, HMD);
			strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
			sprintf_s(szDong, "T\330 l\326 th\265nh c\253ng l\307n xung huy\326t n\265y l\265: %d%%", Rate / 100);
			strcat_s(szInfo, szDong); strcat_s(szInfo, "\n\n");
			if (Rate >= 10000 || RollbackLevel >= m_nLevel - 1)
				sprintf_s(szNho, "%s", "Duy tr\327 huy\326t v\336 hi\326n t\271i.");
			else
				sprintf_s(szNho, "C\312p huy\326t v\336 tr\353 v\322 c\312p %d", RollbackLevel);
			sprintf_s(szDong, "L\307n xung huy\326t n\265y c\361a c\270c h\271 th\312t b\271i l\265: %s", szNho);
			strcat_s(szInfo, szDong);
		}
		break;
	}
	}
	m_Info.SetText(szInfo);

	// [KM 27/08] hien dieu khien theo cach: chi-mot-cach (duong DHMD) dung nut giua,
	// nguoc lai 3 o chon + nut phai - dung nhu 2 nut trong tep chuan.
	if (m_nWay == 3)
	{
		m_btnOkOne.Show();
		m_btnOk.Hide();
		int i;
		for (i = 0; i < 3; i++)
		{
			m_cbWay[i].Hide();
			m_imgWay[i].Hide();
		}
	}
	else
	{
		m_btnOkOne.Hide();
		m_btnOk.Show();
		// [KM 27/08b] tick 3 khung: dang chon / chua chon / xam (cach vo hieu voi
		// (mach, cap) nay - van bam duoc de xem dong giai thich, dung nhu ban chuan).
		int nHopLe[3];
		nHopLe[0] = 1;
		nHopLe[1] = (Rate < 10000 && KM_ProtectCap(nMach, m_nLevel) > 0);
		nHopLe[2] = (KM_EnhanceMach(nMach) && KM_EnhanceCap(m_nLevel)
			&& m_HuyetLong[nMach <= 8 ? nMach : 0][m_nLevel][6] > 0
			&& m_HuyetLong[nMach <= 8 ? nMach : 0][m_nLevel][3] >= 0);
		int i, nChon;
		for (i = 0; i < 3; i++)
		{
			// [KM 27/08b] m_nWay: 0=Pho thong, 1=Huyet vi bao ve, 2=Long Hon Ho The
			// (WndProc dat dung nhu vay). Ban cu so lech mot nac nen o danh dau sang
			// nham nut - chon Bao ve thi Pho thong sang.
			nChon = (m_nWay == i);
			m_cbWay[i].Show();
			m_cbWay[i].CheckButton(nChon);
			m_imgWay[i].SetFrame(!nHopLe[i] ? m_nTickXam : (nChon ? m_nTickCo : m_nTickKhong));
			m_imgWay[i].Show();
		}
	}
}

int KUiMeridianConfirm::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int i;
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
	{
		for (i = 0; i < 3; i++)
		{
			if (uParam == (unsigned int)(KWndWindow*)&m_cbWay[i])
			{
				m_nWay = (i == 0) ? 0 : i;			// o 0 = pho thong, o 1 = bao ve, o 2 = Long Hon
				SetInfo();
			}
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_btnOk ||
			uParam == (unsigned int)(KWndWindow*)&m_btnOkOne)
		{
			// [KM 27/08] van dung goi GOI_SET_PLAYER_MERIDIAN co san (khong dung day
			// protocol): way 1 -> WayProtected, way 2 -> WayEnhanced, way 0/3 -> thuong.
			SetMeridianData	Data;
			Data.WayProtected = (m_nWay == 1) ? protectedway : normalway;
			Data.WayEnhanced = (m_nWay == 2) ? enhancedway : normalway;
			Data.Type = m_nType;
			Data.Level = m_nLevel;
			g_pCoreShell->OperationRequest(GOI_SET_PLAYER_MERIDIAN, (unsigned int)&Data, 0);
			KUiMeridianConfirm::CloseWindow();		// [KM 27/08b] khong con day bang mach
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_btnClose)
		{
			KUiMeridianConfirm::CloseWindow();		// [KM 27/08b] khong con day bang mach
		}
		break;
	}
	default:
		return KWndWindow::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}
