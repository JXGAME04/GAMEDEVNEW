/*
 * File:     UiTaskGuide.cpp
 * Desc:     Bang "Chi nam nhiem vu" (F11; ten cua so "NewTask" nhu ban Linux/JX2).
 *           Giao dien nap tu <scheme>\uitaskguide\taskguide.ini (bo cuc + anh SPR
 *           y het ban tham chieu). Noi dung Da Tau soan lai tu UI\taskui_random.lua
 *           cua chinh client nay (chuoi TCVN3 trong UiTaskGuideStr.h, sinh tu dong).
 * Creation: 2026/08/19
 */
#include "KWin32.h"
#include "KIniFile.h"
#include <stdio.h>
#include <string.h>
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiSoundSetting.h"
#include "UiTaskGuide.h"
#include "UiTaskTrace.h"
#include "UiTaskGuideStr.h"
#include "UiTaskGuideSatThuBoss.h"	// [C18] SINH TU DONG tu killer.txt
#include "../UiBase.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "../../../core/src/CoreShell.h"
#include "../../../core/src/gamedatadef.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KTabFile.h"

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI_TASKGUIDE	"uitaskguide\\taskguide.ini"
#define TASKGUIDE_DATAU_TASKID	6
#define TASKGUIDE_TINSU_TASKID	7	// [TIN SU 21/08] task 1201..1218 (nt_setTask -> SetSaveVal -> UI_TASKVALUE)
#define TASKGUIDE_BANGCHIEN_TASKID	8	// [CHI NAM 24/08] tongwar: task 2369..2378 (nt_setTask co SyncTaskValue)
#define TASKGUIDE_BACHNHAN_TASKID	9	// [CHI NAM 24/08] bairenleitai: task 2709 (luot exp/ngay)
#define TASKGUIDE_SATTHU_TASKID	10	// [3HD 25/08] san boss sat thu: task 1082/1192/1193/1217 (nt_setTask co SyncTaskValue)

// ---- bang du lieu tasklink (dia thang pak - KPakFile doc dia truoc) ----
enum
{
	DTG_TAB_BUYGOODS = 0,
	DTG_TAB_FINDGOODS,
	DTG_TAB_SHOWGOODS,
	DTG_TAB_FINDMAPS,
	DTG_TAB_UPGROUND,
	DTG_TAB_WORLDMAPS,
	DTG_TAB_COUNT,
};

static const char* s_szDTGTabPath[DTG_TAB_COUNT] =
{
	"\\settings\\task\\tasklink_buygoods.txt",
	"\\settings\\task\\tasklink_findgoods.txt",
	"\\settings\\task\\tasklink_showgoods.txt",
	"\\settings\\task\\tasklink_findmaps.txt",
	"\\settings\\task\\tasklink_upground.txt",
	"\\settings\\task\\tasklink_worldmaps.txt",
};

static KTabFile	s_DTGTab[DTG_TAB_COUNT];
static int		s_bDTGTabLoaded[DTG_TAB_COUNT] = { 0 };

static KTabFile* DTG_GetTab(int nIdx)
{
	if (nIdx < 0 || nIdx >= DTG_TAB_COUNT)
		return NULL;
	if (!s_bDTGTabLoaded[nIdx])
	{
		if (!s_DTGTab[nIdx].Load((LPSTR)s_szDTGTabPath[nIdx]))
			return NULL;	// thieu tep -> thu lai lan mo sau
		s_bDTGTabLoaded[nIdx] = 1;
	}
	return &s_DTGTab[nIdx];
}

static void DTG_Cell(KTabFile* pTab, int nRow, const char* szCol, char* pOut, int nSize)
{
	pOut[0] = 0;
	if (pTab)
		pTab->GetString(nRow, (LPSTR)szCol, (LPSTR)"", pOut, nSize - 1);
}

static int DTG_CellInt(KTabFile* pTab, int nRow, const char* szCol)
{
	int nVal = 0;
	if (pTab)
		pTab->GetInteger(nRow, (LPSTR)szCol, 0, &nVal);
	return nVal;
}

// gia tri task cua nhan vat (client giu ban sao, dong bo qua UI_TASKVALUE)
static int DTG_TaskVal(int nTaskId)
{
	if (g_pCoreShell)
		return g_pCoreShell->GetGameData(GDI_TASK_SAVE_VALUE, (unsigned int)nTaskId, 0);
	return 0;
}

static int DTG_Repute()
{
	if (g_pCoreShell)
		return g_pCoreShell->GetGameData(GDI_PLAYER_REPUTE_VALUE, 0, 0);
	return 0;
}

//////////////////////////////////////////////////////////////////////////

KUiTaskGuide*	KUiTaskGuide::m_pSelf = NULL;

KUiTaskGuide::KUiTaskGuide()
{
	m_szTaskIniPath[0] = 0;
	m_nEntryCount = 0;
	m_nCurEntry = -1;
}

KUiTaskGuide::~KUiTaskGuide()
{
}

KUiTaskGuide* KUiTaskGuide::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiTaskGuide;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
		// lam moi du lieu moi lan mo (task value co the da doi khi dong bang)
		if (m_pSelf->m_nCurEntry >= 0)
			m_pSelf->ShowTask(m_pSelf->m_nCurEntry);
	}
	return m_pSelf;
}

void KUiTaskGuide::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->Hide();
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

KUiTaskGuide* KUiTaskGuide::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

void KUiTaskGuide::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
		m_pSelf->LoadSchemeSelf(pScheme);
}

void KUiTaskGuide::OnTaskValueChanged(int nTaskId)
{
	if (m_pSelf == NULL || !m_pSelf->IsVisible())
		return;
	if (m_pSelf->m_nCurEntry < 0 || m_pSelf->m_nCurEntry >= m_pSelf->m_nEntryCount)
		return;
	// dang xem trang Da Tau -> ve lai (moi id lien quan deu re, khoi loc)
	if (m_pSelf->m_Entries[m_pSelf->m_nCurEntry].nTaskId == TASKGUIDE_DATAU_TASKID)
	{
		m_pSelf->BuildDaTauText();
		m_pSelf->UpdateButtons();	// course doi -> nut Bo nhiem vu doi trang thai
	}
	else if (m_pSelf->m_Entries[m_pSelf->m_nCurEntry].nTaskId == TASKGUIDE_TINSU_TASKID)
	{
		if ((nTaskId >= 1201 && nTaskId <= 1218) || nTaskId == 4128)	// [24/08] 4128 = bo dem ngay
			m_pSelf->BuildTinSuText();
	}
	else if (m_pSelf->m_Entries[m_pSelf->m_nCurEntry].nTaskId == TASKGUIDE_BANGCHIEN_TASKID)
	{
		if (nTaskId >= 2369 && nTaskId <= 2378)
			m_pSelf->BuildBangChienText();
	}
	else if (m_pSelf->m_Entries[m_pSelf->m_nCurEntry].nTaskId == TASKGUIDE_BACHNHAN_TASKID)
	{
		if (nTaskId == 2709)
			m_pSelf->BuildBachNhanText();
	}
	else if (m_pSelf->m_Entries[m_pSelf->m_nCurEntry].nTaskId == TASKGUIDE_SATTHU_TASKID)
	{
		if (nTaskId == 1082 || nTaskId == 1192 || nTaskId == 1193 || nTaskId == 1217)
			m_pSelf->BuildSatThuText();
	}
}

void KUiTaskGuide::RefreshButtons()
{
	if (m_pSelf && m_pSelf->IsVisible())
		m_pSelf->UpdateButtons();
}

// Ban rut gon 1 dong cho khung Theo doi nhiem vu (KUiTaskTrace).
void KUiTaskGuide::BuildBriefLine(char* pOut, int nSize)
{
	pOut[0] = 0;
	int nType   = DTG_TaskVal(1021);
	int nCourse = DTG_TaskVal(1028);
	int nRow    = DTG_TaskVal(1030);
	char szBuf[256];
	szBuf[0] = 0;

	if (nType == 0)
	{
		strncpy(szBuf, DTG_BRIEF_NONE, sizeof(szBuf) - 1);
	}
	else if (nCourse == 2 || nCourse == 3)
	{
		strncpy(szBuf, DTG_BRIEF_RETURN, sizeof(szBuf) - 1);
	}
	else
	{
		KTabFile* pTab = NULL;
		switch (nType)
		{
		case 1:
			strncpy(szBuf, DTG_BRIEF_T1, sizeof(szBuf) - 1);
			break;
		case 2:
			strncpy(szBuf, DTG_BRIEF_T2, sizeof(szBuf) - 1);
			break;
		case 3:
			strncpy(szBuf, DTG_BRIEF_T3, sizeof(szBuf) - 1);
			break;
		case 4:
			pTab = DTG_GetTab(DTG_TAB_FINDMAPS);
			if (pTab)
			{
				char szNum[64];
				DTG_Cell(pTab, nRow, "Num", szNum, sizeof(szNum));
				const char* pLoai = (DTG_CellInt(pTab, nRow, "MapType") == 1) ?
					DTG_LOAI_DIADO : DTG_LOAI_MAT;
				sprintf(szBuf, DTG_BRIEF_T4_FMT, pLoai, DTG_TaskVal(1025), szNum);
			}
			break;
		case 5:
			strncpy(szBuf, DTG_BRIEF_T5, sizeof(szBuf) - 1);
			break;
		case 6:
			pTab = DTG_GetTab(DTG_TAB_WORLDMAPS);
			if (pTab)
			{
				char szNum[64];
				DTG_Cell(pTab, nRow, "Num", szNum, sizeof(szNum));
				sprintf(szBuf, DTG_BRIEF_T6_FMT, DTG_TaskVal(1027), szNum);
			}
			break;
		}
	}
	szBuf[sizeof(szBuf) - 1] = 0;
	strncpy(pOut, szBuf, nSize - 1);
	pOut[nSize - 1] = 0;
}

void KUiTaskGuide::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_TaskList);
	AddChild(&m_SelTaskName);
	AddChild(&m_Content);
	AddChild(&m_ContentScroll);
	AddChild(&m_BtnClose);
	AddChild(&m_BtnQuit);
	AddChild(&m_BtnTrace);
	AddChild(&m_BtnCancelTrace);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, sizeof(Scheme));
	LoadSchemeSelf(Scheme);

	LoadTaskList();
	FillTaskList();

	Wnd_AddWindow(this);

	// tu chon dong nhiem vu dau tien (bo qua tieu de nhom)
	for (int i = 0; i < m_nEntryCount; i++)
	{
		if (m_Entries[i].nTaskId >= 0)
		{
			m_TaskList.GetMessageListBox()->SetCurSel(i);
			ShowTask(i);
			break;
		}
	}
}

void KUiTaskGuide::LoadSchemeSelf(const char* pScheme)
{
	char		Buff[256];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_TASKGUIDE);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		m_Title.Init(&Ini, "Title");
		m_TaskList.Init(&Ini, "TaskList");
		m_SelTaskName.Init(&Ini, "SelTaskName");
		m_Content.Init(&Ini, "TaskContent");
		m_ContentScroll.Init(&Ini, "DownScroll");
		m_Content.SetScrollbar(&m_ContentScroll);
		m_BtnClose.Init(&Ini, "BtnClose");
		m_BtnQuit.Init(&Ini, "QuitTaskButton");
		m_BtnTrace.Init(&Ini, "TraceButton");
		m_BtnCancelTrace.Init(&Ini, "CancelTraceButton");
		UpdateButtons();

		Ini.GetString("Main", "TaskIni", "\\UI\\uitasklist.ini",
			m_szTaskIniPath, sizeof(m_szTaskIniPath) - 1);
	}
}

void KUiTaskGuide::LoadTaskList()
{
	m_nEntryCount = 0;
	KIniFile Ini;
	if (m_szTaskIniPath[0] == 0)
		strcpy(m_szTaskIniPath, "\\UI\\uitasklist.ini");
	if (!Ini.Load(m_szTaskIniPath))
		return;
	// cac section dat ten [0]..[31]; tep goc co the khong lien so -> quet het dai
	for (int i = 0; i < TASKGUIDE_MAX_ENTRY * 2 && m_nEntryCount < TASKGUIDE_MAX_ENTRY; i++)
	{
		char szSection[16];
		sprintf(szSection, "%d", i);
		KTaskGuideEntry* pEntry = &m_Entries[m_nEntryCount];
		pEntry->szName[0] = 0;
		Ini.GetString(szSection, "Name", "", pEntry->szName, sizeof(pEntry->szName) - 1);
		if (pEntry->szName[0] == 0)
			continue;
		Ini.GetInteger(szSection, "TaskId", -1, &pEntry->nTaskId);
		m_nEntryCount++;
	}
}

void KUiTaskGuide::FillTaskList()
{
	KWndMessageListBox* pList = m_TaskList.GetMessageListBox();
	pList->Clear();
	if (m_nEntryCount <= 0)
		return;
	pList->SetCapability(m_nEntryCount);
	for (int i = 0; i < m_nEntryCount; i++)
	{
		char szLine[TASKGUIDE_NAME_LEN + 8];
		strncpy(szLine, m_Entries[i].szName, sizeof(szLine) - 1);
		szLine[sizeof(szLine) - 1] = 0;
		int nLen = TEncodeText(szLine, strlen(szLine));
		pList->AddOneMessage(szLine, nLen);
	}
}

void KUiTaskGuide::ShowTask(int nEntry)
{
	if (nEntry < 0 || nEntry >= m_nEntryCount)
		return;
	m_nCurEntry = nEntry;

	KTaskGuideEntry* pEntry = &m_Entries[nEntry];
	if (pEntry->nTaskId < 0)
	{
		// tieu de nhom - khong co noi dung
		m_SelTaskName.SetText("", 0);
		m_Content.Clear();
		return;
	}

	// ten nhiem vu (bo dau '+' trang tri o dau)
	const char* pName = pEntry->szName;
	while (*pName == '+' || *pName == ' ')
		pName++;
	m_SelTaskName.SetText(pName);

	if (pEntry->nTaskId == TASKGUIDE_DATAU_TASKID)
	{
		BuildDaTauText();
	}
	else if (pEntry->nTaskId == TASKGUIDE_TINSU_TASKID)
	{
		BuildTinSuText();
	}
	else if (pEntry->nTaskId == TASKGUIDE_BANGCHIEN_TASKID)
	{
		BuildBangChienText();
	}
	else if (pEntry->nTaskId == TASKGUIDE_BACHNHAN_TASKID)
	{
		BuildBachNhanText();
	}
	else if (pEntry->nTaskId == TASKGUIDE_SATTHU_TASKID)
	{
		BuildSatThuText();
	}
	else
	{
		m_Content.Clear();
		AddLine(DTG_NO_SUPPORT);
	}
	UpdateButtons();
}

// [TIN SU 21/08] Soan noi dung nhiem vu Tin Su (port Linux task\tollgate\messenger):
//   1204 = tuyen (1 Thanh Do->Dai Ly, 2 Dai Ly->Thanh Do; 0 = chua nhan)
//   1203 = buoc ai Thien Bao Kho: 10 da nhan / 20 dang trong ai / 21 tam ngung /
//          25 ra ai chua du ruong / 30 da vuot ai (posthouse.lua, messenger_turenpc.lua)
//   1201 = ma thu tu 5 ruong phai mo (5 chu so), 1202 = ruong da mo theo thu tu
//   1205 = diem tich luy, 1206 = cap danh hieu, 1218 = so lan hoan thanh hom nay
static void TS_Digits(int nCode, char* szOut, int nOutSize, int* pnCount)
{
	char szRev[16];
	int n = 0;
	while (nCode > 0 && n < 10)
	{
		szRev[n++] = (char)('0' + (nCode % 10));
		nCode /= 10;
	}
	int k = 0;
	for (int i = n - 1; i >= 0 && k < nOutSize - 2; i--)
	{
		if (k)
			szOut[k++] = ',';
		szOut[k++] = szRev[i];
	}
	szOut[k] = 0;
	if (pnCount)
		*pnCount = n;
}

// [CHI NAM 24/08] Bang Chien: doc task 2369 diem tran, 2370 so lan chet, 2371 so nguoi ha,
// 2373 quan ham 1..5, 2375 chuoi ha cao nhat, 2376 tran mang, 2378 tong diem ca mua.
// [24/08 hau phan bien] Task dang "dem theo ngay" cua may chu ma hoa la
// yymmdd*256 + so_lan (script\activitysys\playerfunlib.lua). Neu phan ngay khac
// hom nay thi so dem coi nhu 0. Engine giai ma y het o CoreShell.cpp.
static int DTG_DailyCount(int nRaw)
{
	if (nRaw <= 0)
		return 0;
	SYSTEMTIME st;
	GetLocalTime(&st);
	int nToday = (st.wYear % 100) * 10000 + st.wMonth * 100 + st.wDay;
	if ((nRaw / 256) != nToday)
		return 0;
	return nRaw % 256;
}

// [FIX 24/08 v2] Diem tich luy Tin Su (task 1205) tung bi bo dem ngay ghi de thanh
// yymmdd*256 (~66 trieu). May chu da tach bo dem sang 4128 va co ham don rac, nhung ham do
// chi chay khi nguoi choi noi chuyen Dich Quan. Chan them o day: moc doi thuong cao nhat
// chi 1500 nen gia tri >= 1.000.000 chac chan la rac -> hien 0.
static int DTG_TinSuPoint()
{
	int nPoint = DTG_TaskVal(1205);
	if (nPoint >= 1000000 || nPoint < 0)
		return 0;
	return nPoint;
}

void KUiTaskGuide::BuildBangChienText()
{
	m_Content.Clear();
	int nPoint  = DTG_TaskVal(2369);
	int nDeath  = DTG_TaskVal(2370);
	int nKill   = DTG_TaskVal(2371);
	int nRank   = DTG_TaskVal(2373);
	int nSeries = DTG_TaskVal(2375);
	int nMaxDie = DTG_TaskVal(2376);
	int nTotal  = DTG_TaskVal(2378);
	// [24/08 hau phan bien] phai xet CA quan ham (2373) va tran mang (2376) - hai truong
	// nay duoc dat NGAY LUC BAO DANH (tongwar_trap.lua:132,141) nen dang trong tran ma
	// chi xet 4 truong kia se bao nham "chua tham gia".
	if (nPoint == 0 && nKill == 0 && nTotal == 0 && nDeath == 0 && nRank == 0 && nMaxDie == 0)
	{
		AddLine(TW_NOTASK);
		return;
	}
	static const char* s_szRank[6] = { TW_RANK_0, TW_RANK_1, TW_RANK_2, TW_RANK_3, TW_RANK_4, TW_RANK_5 };
	if (nRank < 0 || nRank > 5)
		nRank = 0;
	char szLine[512];
	sprintf(szLine, TW_INFO_FMT, nPoint, nTotal);
	AddLine(szLine);
	sprintf(szLine, TW_KILL_FMT, nKill, nSeries);
	AddLine(szLine);
	sprintf(szLine, TW_DEATH_FMT, nDeath, nMaxDie);
	AddLine(szLine);
	sprintf(szLine, TW_RANK_FMT, s_szRank[nRank]);
	AddLine(szLine);
}

// [CHI NAM 24/08] Bach Nhan Loi Dai: task 2709 ma hoa yymmdd*256 + so luot da dung.
// Tran 50 luot/ngay (BR_TRAN_LUOT_NGAY trong cauhinh_hoatdong.lua).
void KUiTaskGuide::BuildBachNhanText()
{
	m_Content.Clear();
	AddLine(BR_INFO);
	AddLine(BR_HOWTO);
	const int nLimit = 50;
	int nRaw = DTG_TaskVal(2709);
	int nUsed = (nRaw > 0) ? (nRaw % 256) : 0;
	if (nUsed > nLimit)
		nUsed = nLimit;
	char szLine[256];
	if (nUsed >= nLimit)
		AddLine(BR_LIMIT_FULL);
	else
	{
		sprintf(szLine, BR_LIMIT_FMT, nUsed, nLimit);
		AddLine(szLine);
	}
}

// [3HD 25/08] San boss Sat Thu (ban Linux): task 1082 = chi so boss dang nhan
// (1..160, moi nhom 20 chi so = mot bac cap 20..90; chi nhom 90 = 141..160 con
// phat thuong), 1193 = so lan giet hom nay (tran 8).
void KUiTaskGuide::BuildSatThuText()
{
	m_Content.Clear();
	AddLine(ST3_INFO);
	AddLine(ST3_HOWTO);
	int nBoss = DTG_TaskVal(1082);
	char szLine[512];
	if (nBoss >= 1 && nBoss <= 160)
	{
		int nCapNhom = 20 + ((nBoss - 1) / 20) * 10;	// 1-20=cap20 ... 141-160=cap90
		// [C22] hien 'con thu x/20 cua nhom' - so bang (141..160) lam nguoi choi
		// tuong nham la so lan nhan; x = thu tu trong nhom nguoi choi da tu chon.
		sprintf(szLine, ST3_CUR_FMT, ((nBoss - 1) % 20) + 1, nCapNhom);
		AddLine(szLine);
		// [C18] muc tieu: ten + noi o (co toa do) tu bang nuong killer.txt
		sprintf(szLine, ST3_TARGET_FMT, s_szST3BossName[nBoss], s_szST3BossInfo[nBoss]);
		AddLine(szLine);
		AddLine(ST3_HINT_GO);	// [C20]
		if (nBoss >= 141)
			AddLine(ST3_GROUP90);
		else
			AddLine(ST3_GROUPLOW);
	}
	else
	{
		AddLine(ST3_NOTASK);
	}
	int nUsed = DTG_TaskVal(1193);
	if (nUsed < 0) nUsed = 0;
	sprintf(szLine, ST3_LIMIT_FMT, nUsed);
	AddLine(szLine);
}
void KUiTaskGuide::BuildTinSuText()
{
	m_Content.Clear();
	int nRoute = DTG_TaskVal(1204);
	int nState = DTG_TaskVal(1203);
	if (nRoute == 0 || nState == 0)
	{
		AddLine(TS_NOTASK);
	}
	else
	{
		char szLine[1024];
		const char* szFrom = (nRoute == 2) ? TS_ROUTE_1B : TS_ROUTE_1A;
		const char* szTo   = (nRoute == 2) ? TS_ROUTE_1A : TS_ROUTE_1B;
		sprintf(szLine, TS_ROUTE_FMT, szFrom, szTo);
		AddLine(szLine);
		switch (nState)
		{
		case 10:
			AddLine(TS_ST10);
			break;
		case 20:
			{
				char szNeed[32], szDone[32];
				int nDone = 0;
				TS_Digits(DTG_TaskVal(1201), szNeed, sizeof(szNeed), NULL);
				TS_Digits(DTG_TaskVal(1202), szDone, sizeof(szDone), &nDone);
				if (!szDone[0])
					strcpy(szDone, TS_NONE);
				sprintf(szLine, TS_ST20_FMT, szNeed, szDone, nDone);
				AddLine(szLine);
			}
			break;
		case 21:
			AddLine(TS_ST21);
			break;
		case 25:
			AddLine(TS_ST25);
			break;
		case 30:
			sprintf(szLine, TS_ST30, szTo);
			AddLine(szLine);
			break;
		default:
			AddLine(TS_ST10);
			break;
		}
	}
	static const char* s_szTitle[6] = { TS_TITLE_0, TS_TITLE_1, TS_TITLE_2, TS_TITLE_3, TS_TITLE_4, TS_TITLE_5 };
	int nTitle = DTG_TaskVal(1206);
	if (nTitle < 0 || nTitle > 5)
		nTitle = 0;
	char szPt[512];
	// [24/08 hau phan bien] 1218 la so TRON DOI (khong noi nao dat lai 0) - bo dem NGAY
	// that la 4128 (doi tu 1205 hom nay, dang yymmdd*256 + so_lan).
	sprintf(szPt, TS_POINT_FMT, DTG_TinSuPoint(), s_szTitle[nTitle], DTG_DailyCount(DTG_TaskVal(4128)));
	AddLine(szPt);
}

// Bat/tat 3 nut duoi theo dung nghia ban goc, ap cho he Da Tau:
// - Bo nhiem vu: chi khi dang lam (course 1) - gui tg_quit, server mo hop
//   xac nhan huy CHUAN (du luat phat / tru luot / 100 manh SHXT).
// - Theo doi / Huy theo doi: bat tat khung KUiTaskTrace.
// [C33] CAC he dang duoc 'Theo doi' (truoc day chi giu DUY NHAT 1 he).
// Chi so = TaskId (6..15). Mac dinh khong theo doi he nao - nguoi choi tu bam.
#define TRACE_MAX_TASKID 16
static bool s_abTraced[TRACE_MAX_TASKID] = { false };

bool KUiTaskGuide::IsTracedTask(int nTaskId)
{
	if (nTaskId < 0 || nTaskId >= TRACE_MAX_TASKID)
		return false;
	return s_abTraced[nTaskId];
}

void KUiTaskGuide::SetTracedTask(int nTaskId, bool bOn)
{
	if (nTaskId < 0 || nTaskId >= TRACE_MAX_TASKID)
		return;
	s_abTraced[nTaskId] = bOn;
}

int KUiTaskGuide::GetTracedList(int* pOut, int nMax)
{
	int n = 0;
	for (int i = 0; i < TRACE_MAX_TASKID && n < nMax; i++)
		if (s_abTraced[i])
			pOut[n++] = i;
	return n;
}

// he dau tien dang theo doi (giu tuong thich cho ma cu)
int KUiTaskGuide::GetTracedTaskId()
{
	for (int i = 0; i < TRACE_MAX_TASKID; i++)
		if (s_abTraced[i])
			return i;
	return TASKGUIDE_DATAU_TASKID;
}

const char* KUiTaskGuide::GetTaskTitle(int nTaskId)
{
	if (nTaskId == TASKGUIDE_SATTHU_TASKID)
		return ST3_TRACE_TITLE;
	return DTG_TRACE_TITLE;
}

// ban rut gon cua MOT he bat ky (khung theo doi liet ke nhieu he)
void KUiTaskGuide::BuildTraceLineOf(int nTaskId, char* pOut, int nSize)
{
	if (nTaskId == TASKGUIDE_SATTHU_TASKID)
	{
		int nBoss = DTG_TaskVal(1082);
		if (nBoss >= 1 && nBoss <= ST3_BOSS_MAX)
		{
			int nUsed = DTG_TaskVal(1193);
			if (nUsed < 0) nUsed = 0;
			_snprintf(pOut, nSize - 1, ST3_BRIEF_FMT, s_szST3BossName[nBoss], s_szST3BossInfo[nBoss], nUsed);
			pOut[nSize - 1] = 0;
		}
		else
		{
			strncpy(pOut, ST3_BRIEF_NONE, nSize - 1);
			pOut[nSize - 1] = 0;
		}
		return;
	}
	BuildBriefLine(pOut, nSize);	// mac dinh: Da Tau
}

void KUiTaskGuide::UpdateButtons()
{
	bool bDT = (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount &&
		m_Entries[m_nCurEntry].nTaskId == TASKGUIDE_DATAU_TASKID);
	// [C18] tab San Boss Sat Thu cung dung duoc 3 nut nhu Da Tau
	bool bST = (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount &&
		m_Entries[m_nCurEntry].nTaskId == TASKGUIDE_SATTHU_TASKID);
	int nCourse = bDT ? DTG_TaskVal(1028) : 0;
	int nBossST = bST ? DTG_TaskVal(1082) : 0;
	m_BtnQuit.Enable((bDT && nCourse == 1) || (bST && nBossST >= 1 && nBossST <= ST3_BOSS_MAX));
	// [C33] moi he theo doi doc lap: bat 'Theo doi' khi he DANG CHON chua theo doi
	int nCurTask = (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount) ? m_Entries[m_nCurEntry].nTaskId : -1;
	bool bCoBan = (bDT || bST);	// he da co ban rut gon de hien trong khung
	m_BtnTrace.Enable(bCoBan && !IsTracedTask(nCurTask));
	m_BtnCancelTrace.Enable(bCoBan && IsTracedTask(nCurTask));
}

// [C18] dong rut gon cho khung Theo doi - theo tab da bam 'Theo doi'.
void KUiTaskGuide::BuildTraceLine(char* pOut, int nSize)
{
	if (GetTracedTaskId() == TASKGUIDE_SATTHU_TASKID)
	{
		int nBoss = DTG_TaskVal(1082);
		if (nBoss >= 1 && nBoss <= ST3_BOSS_MAX)
		{
			int nUsed = DTG_TaskVal(1193);
			if (nUsed < 0) nUsed = 0;
			_snprintf(pOut, nSize - 1, ST3_BRIEF_FMT, s_szST3BossName[nBoss], s_szST3BossInfo[nBoss], nUsed);
			pOut[nSize - 1] = 0;
		}
		else
		{
			strncpy(pOut, ST3_BRIEF_NONE, nSize - 1);
			pOut[nSize - 1] = 0;
		}
		return;
	}
	BuildBriefLine(pOut, nSize);
}
// [FIX 24/08 v2] TEncodeText (Engine\Src\Text.cpp:468) coi MOI byte > 0x80 la chu Han 2 byte
// va nuot LUON byte ke tiep. Tieng Viet TCVN3 la 1 byte => neu dau '<' cua mot the roi dung
// vao vi tri byte-duoi cua mot cap nhu vay thi the KHONG con duoc phan tich, phan "color>"
// in thang ra man hinh.
//
// Phai xu ly SAU KHI ghep chuoi: trong khuon mau "<color=yellow>%s<color>" thi truoc '<' la
// ky tu 's' (an toan), chi khi %s duoc thay bang ten ket thuc bang chu CO DAU (vd "Dai Ly",
// "chua co") thi the moi hong. Vi vay quet rieng tep chuoi la KHONG DU.
//
// Cach lam: MO PHONG dung buoc nhay cua TEncodeText tu dau chuoi; chi khi '<' roi vao vi tri
// byte-duoi moi chen 1 dau cach lam byte-duoi thay the, day '<' thanh byte dau.
static void DTG_FixTagAfterVn(char* pBuf, size_t nCap)
{
	if (!pBuf)
		return;
	char szTmp[2048];
	size_t i = 0, o = 0;
	size_t nLen = strlen(pBuf);
	while (i < nLen && o + 3 < sizeof(szTmp))
	{
		unsigned char c = (unsigned char)pBuf[i];
		if (c > 0x80 && i + 1 < nLen)
		{
			if (pBuf[i + 1] == '<')
			{
				szTmp[o++] = pBuf[i];
				szTmp[o++] = ' ';	// byte-duoi gia, hy sinh thay cho '<'
				i++;				// '<' se thanh byte dau o vong sau
			}
			else
			{
				szTmp[o++] = pBuf[i];
				szTmp[o++] = pBuf[i + 1];
				i += 2;
			}
		}
		else
		{
			szTmp[o++] = pBuf[i];
			i++;
		}
	}
	while (i < nLen && o + 1 < sizeof(szTmp))
		szTmp[o++] = pBuf[i++];
	szTmp[o] = 0;
	strncpy(pBuf, szTmp, nCap - 1);
	pBuf[nCap - 1] = 0;
}

void KUiTaskGuide::AddLine(const char* pText)
{
	char szBuf[2048];
	strncpy(szBuf, pText, sizeof(szBuf) - 1);
	szBuf[sizeof(szBuf) - 1] = 0;
	DTG_FixTagAfterVn(szBuf, sizeof(szBuf));	// [FIX 24/08 v2]
	int nLen = TEncodeText(szBuf, strlen(szBuf));
	m_Content.AddOneMessage(szBuf, nLen);
}

// Soan noi dung Da Tau - port 1:1 tu showrandraskdesc (UI\taskui_random.lua),
// chi khac: (a) bo lap ten map o cau loai 4; (b) them dong dem ngay o cuoi.
void KUiTaskGuide::BuildDaTauText()
{
	m_Content.Clear();

	char szMain[1536];
	char szSub[512];
	char szCell1[256], szCell2[128], szCell3[128];
	szMain[0] = 0;

	int nType   = DTG_TaskVal(1021);
	int nRow    = DTG_TaskVal(1030);
	int nCourse = DTG_TaskVal(1028);

	if (nType == 0)
	{
		AddLine(DTG_NOTASK);
	}
	else
	{
		KTabFile* pTab = NULL;
		switch (nType)
		{
		case 1:		// mua vat pham
			pTab = DTG_GetTab(DTG_TAB_BUYGOODS);
			if (pTab)
			{
				DTG_Cell(pTab, nRow, "TaskInfo", szCell1, sizeof(szCell1));
				DTG_Cell(pTab, nRow, "TaskInfo1", szCell2, sizeof(szCell2));
				sprintf(szMain, DTG_T1_FMT, szCell1, szCell2);
			}
			break;
		case 2:		// tim vat pham (nop mat do)
			pTab = DTG_GetTab(DTG_TAB_FINDGOODS);
			if (pTab)
			{
				char szMagic[128];
				DTG_Cell(pTab, nRow, "MagicCnName", szMagic, sizeof(szMagic));
				DTG_Cell(pTab, nRow, "TaskInfo", szCell1, sizeof(szCell1));
				if (strcmp(szMagic, "n") == 0)
				{
					sprintf(szMain, DTG_T2A_FMT, szCell1);
				}
				else
				{
					DTG_Cell(pTab, nRow, "MinValue", szCell2, sizeof(szCell2));
					DTG_Cell(pTab, nRow, "MaxValue", szCell3, sizeof(szCell3));
					sprintf(szMain, DTG_T2B_FMT, szMagic, szCell2, szCell3, szCell1);
				}
			}
			break;
		case 3:		// khoe vat pham (khong mat do)
			pTab = DTG_GetTab(DTG_TAB_SHOWGOODS);
			if (pTab)
			{
				char szMagic[128];
				DTG_Cell(pTab, nRow, "MagicCnName", szMagic, sizeof(szMagic));
				DTG_Cell(pTab, nRow, "MinValue", szCell2, sizeof(szCell2));
				DTG_Cell(pTab, nRow, "MaxValue", szCell3, sizeof(szCell3));
				sprintf(szMain, DTG_T3_FMT, szMagic, szCell2, szCell3);
			}
			break;
		case 4:		// dia do chi / mat chi
			pTab = DTG_GetTab(DTG_TAB_FINDMAPS);
			if (pTab)
			{
				DTG_Cell(pTab, nRow, "TaskInfo1", szCell1, sizeof(szCell1));
				DTG_Cell(pTab, nRow, "Num", szCell2, sizeof(szCell2));
				const char* pLoai = (DTG_CellInt(pTab, nRow, "MapType") == 1) ?
					DTG_LOAI_DIADO : DTG_LOAI_MAT;
				sprintf(szMain, DTG_T4_FMT, szCell1, szCell2, pLoai,
					DTG_TaskVal(1025), pLoai);
			}
			break;
		case 5:		// nang chi so
			pTab = DTG_GetTab(DTG_TAB_UPGROUND);
			if (pTab)
			{
				int nNumType = DTG_CellInt(pTab, nRow, "NumericType");
				int nTarget  = DTG_CellInt(pTab, nRow, "NumericValue");
				DTG_Cell(pTab, nRow, "NumericValue", szCell1, sizeof(szCell1));
				int nCur = 0, nNeed = 0;
				szSub[0] = 0;
				switch (nNumType)
				{
				case 1:
					strcpy(szSub, DTG_T5_SUB1);
					break;
				case 2:
					strcpy(szSub, DTG_T5_SUB2);
					break;
				case 3:
					nCur = DTG_Repute();
					nNeed = nTarget + DTG_TaskVal(1026) - nCur;
					if (nNeed < 0) nNeed = 0;
					sprintf(szSub, DTG_T5_SUB3_FMT, nCur, nNeed);
					break;
				case 4:
					nCur = DTG_TaskVal(151);
					nNeed = nTarget + DTG_TaskVal(1026) - nCur;
					if (nNeed < 0) nNeed = 0;
					sprintf(szSub, DTG_T5_SUB4_FMT, nCur, nNeed);
					break;
				case 5:
					strcpy(szSub, DTG_T5_SUB5);
					break;
				case 6:
					nCur = DTG_TaskVal(747);
					nNeed = nTarget + DTG_TaskVal(1026) - nCur;
					if (nNeed < 0) nNeed = 0;
					sprintf(szSub, DTG_T5_SUB6_FMT, nCur, nNeed);
					break;
				}
				sprintf(szMain, DTG_T5_FMT, szCell1, szSub);
			}
			break;
		case 6:		// manh Son Ha Xa Tac
			pTab = DTG_GetTab(DTG_TAB_WORLDMAPS);
			if (pTab)
			{
				DTG_Cell(pTab, nRow, "Num", szCell1, sizeof(szCell1));
				sprintf(szMain, DTG_T6_FMT, szCell1, DTG_TaskVal(1027));
			}
			break;
		}

		if (szMain[0])
		{
			char szLine[1792];
			if (nCourse == 1)
				sprintf(szLine, "%s%s", DTG_PREFIX_LAM, szMain);
			else if (nCourse == 2 || nCourse == 3)
				sprintf(szLine, "%s%s", DTG_PREFIX_XONG, szMain);
			else
				strcpy(szLine, szMain);
			AddLine(szLine);
		}

		// loai 4 dang lam: goi y bam de tu chay den Xa Phu
		if (nType == 4 && nCourse == 1)
			AddLine(DTG_HINT_XAFU);
	}

	// dong trang thai trong ngay (them so voi ban goc - tien theo doi moc 40)
	sprintf(szSub, DTG_EXTRA_FMT, DTG_TaskVal(2420), DTG_TaskVal(2797));
	AddLine(szSub);
}

int KUiTaskGuide::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_BtnClose)
		{
			CloseWindow(false);
			return true;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_BtnQuit)
		{
			// server mo hop xac nhan huy chuan (Da Tau) / huy truc tiep (Sat Thu)
			if (g_pCoreShell)
			{
				const char* pCmd = "tg_quit";
				if (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount &&
					m_Entries[m_nCurEntry].nTaskId == TASKGUIDE_SATTHU_TASKID)
					pCmd = "st3_quit";	// [C18] -> nieshichen.lua cancel()
				g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 6, (int)pCmd);
			}
			return true;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_BtnTrace)
		{
			if (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount)
				SetTracedTask(m_Entries[m_nCurEntry].nTaskId, true);	// [C33] them he
			KUiTaskTrace::SetTraced(true);
			UpdateButtons();
			return true;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_BtnCancelTrace)
		{
			// [C33] chi bo HE DANG CHON; con he khac thi khung van mo
			if (m_nCurEntry >= 0 && m_nCurEntry < m_nEntryCount)
				SetTracedTask(m_Entries[m_nCurEntry].nTaskId, false);
			{
				int anTmp[TRACE_MAX_TASKID];
				if (GetTracedList(anTmp, TRACE_MAX_TASKID) > 0)
					KUiTaskTrace::OnTaskValueChanged(0);	// con he khac -> chi ve lai
				else
					KUiTaskTrace::SetTraced(false);
			}
			UpdateButtons();
			return true;
		}
		break;
	case WND_N_LIST_ITEM_SEL:
		if (uParam == (unsigned int)(KWndWindow*)&m_TaskList)
		{
			ShowTask(m_TaskList.GetMessageListBox()->GetCurSel());
			return true;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_Content)
		{
			// bam vao dong noi dung nhiem vu -> dan duong den Xa Phu (chi loai 4)
			TryGoXaFu();
			return true;
		}
		break;
	case WND_N_LIST_ITEM_D_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_TaskList)
		{
			// nhap dup ten nhiem vu ben trai cung co tac dung nhu bam noi dung
			TryGoXaFu();
			return true;
		}
		break;
	}
	return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
}

// Chi tac dung voi nhiem vu Da Tau loai 4 (Dia do chi / Mat chi) DANG lam:
// nho CoreShell tu chay den NPC Xa Phu roi mo thoai (muc godatau cho len map).
// Bam lan nua khi dang chay = huy (CoreShell tu xu ly toggle).
void KUiTaskGuide::TryGoXaFu()
{
	if (m_nCurEntry < 0 || m_nCurEntry >= m_nEntryCount)
		return;
	// [3HD C20] tab Sat Thu: tu chay toi boss (xa phu -> qua map -> toa do)
	if (m_Entries[m_nCurEntry].nTaskId == TASKGUIDE_SATTHU_TASKID)
	{
		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_TASKGUIDE_GOTO_SATTHU, 0, 0);
		return;
	}
	if (m_Entries[m_nCurEntry].nTaskId != TASKGUIDE_DATAU_TASKID)
		return;
	if (DTG_TaskVal(1021) != 4 || DTG_TaskVal(1028) != 1)
		return;
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_TASKGUIDE_GOTO_XAFU, 0, 0);
}
