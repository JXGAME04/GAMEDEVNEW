// UiChienLenh.cpp - [CL 04/09 DOT2] cua so CHIEN LENH ve theo DUNG bo cuc VLTK 2.0. Xem UiChienLenh.h.
// Chu Viet trong .cpp ghi bang OCTAL (\325...) - escape hex cua C tham lam, an luon chu so hex dung sau.
#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "GameDataDef.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "UiChienLenh.h"

extern iCoreShell* g_pCoreShell;
extern int SCREEN_WIDTH;	// iCoreShell toan cuc cua S3Client (nhu UiAuction.cpp)

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CL_MAIN_INI			"chienlenh.ini"
#define CL_ICON_INI			"chienlenh_icon.ini"
#define CL_COL_LEFT			72			// [ElemImgRankAwardInfoBG] Left
#define CL_COL_STEP			57			// Width cot
#define CL_ROW_TOP			3			// [ElemImgMissionInfoBG] Top
#define CL_ROW_STEP			40			// Height 38 + cach 2 (season_ticket_2026.lua:386)
#define CL_BLINK_FRAMES		9
#define CL_MSG_FRAMES		(18 * 3)

// chu Viet (TCVN3, octal): "Hoat dong con: " / "ngay" / "gio" / "phut" / "giay" / "Da ket thuc"
static const char* CL_TXT_CON		= "Ho\271t \256\351ng c\337n: ";
static const char* CL_TXT_NGAY		= "ng\265y";
static const char* CL_TXT_GIO		= "gi\352";
static const char* CL_TXT_PHUT		= "ph\363t";
static const char* CL_TXT_GIAY		= "gi\251y";
static const char* CL_TXT_HET		= "\247\267 k\325t th\363c";

static int  s_bGameStarted = 0;
static int  s_bPendingOpen = 0;
static int  s_nPendingIcon = -1;

static void sSendOp(int nOp, int nParam)
{
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_CHIENLENH_UI, (unsigned int)nOp, nParam);
}

static void sSendReq(int nOp, int nA, int nB)
{
	KCLUiReq req;
	req.nA = nA;
	req.nB = nB;
	sSendOp(nOp, (int)&req);
}

//////////////////////////////////////////////////////////////////////
// KUiCLScrollWnd - chuyen tiep len cha (KWndButton chi bao cha TRUC TIEP)
//////////////////////////////////////////////////////////////////////
int KUiCLScrollWnd::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if ((uMsg == WND_N_BUTTON_CLICK || uMsg == WND_N_SCORLLBAR_POS_CHANGED) && m_pParentWnd)
		return m_pParentWnd->WndProc(uMsg, uParam, nParam);
	return KWndImage::WndProc(uMsg, uParam, nParam);
}

//////////////////////////////////////////////////////////////////////
// KUiCLAwardCell
//////////////////////////////////////////////////////////////////////
KUiCLAwardCell::KUiCLAwardCell()
{
	memset(&m_Data, 0, sizeof(m_Data));
	m_bClaimable = 0;
	m_nTempItemIdx = 0;
	m_bVip = 0;
}

void KUiCLAwardCell::Build()
{
	// thu tu AddChild = thu tu ve (sau de len truoc)
	AddChild(&m_IconBg);
	m_IconBg.AddChild(&m_Icon);
	m_IconBg.AddChild(&m_Box);
	m_IconBg.AddChild(&m_Num);
	AddChild(&m_Gray);
	AddChild(&m_Light);
	AddChild(&m_Effect);
	AddChild(&m_Finish);
	AddChild(&m_Lock);
}

void KUiCLAwardCell::LoadScheme(KIniFile* pIni, int bVip)
{
	m_bVip = bVip;
	const char* szP = bVip ? "Vip" : "Low";
	char sz[64];
	sprintf(sz, "ElemImg%sAwardBG", szP);		Init(pIni, sz);
	sprintf(sz, "ElemImg%sAwardLight", szP);	m_Light.Init(pIni, sz);
	sprintf(sz, "ElemImg%sAwardEffect", szP);	m_Effect.Init(pIni, sz);
	sprintf(sz, "ElemImg%sAwardIconBG", szP);	m_IconBg.Init(pIni, sz);
	sprintf(sz, "ElemImg%sAwardIcon", szP);		m_Icon.Init(pIni, sz);
	m_Box.Init(pIni, sz);						// cung hinh chu nhat 24x24 voi anh mac dinh
	m_Box.SetObjectGenre(CGOG_PLAYERSELLITEM);
	sprintf(sz, "ElemTxt%sAwardNum", szP);		m_Num.Init(pIni, sz);
	sprintf(sz, "ElemImg%sAwardGray", szP);		m_Gray.Init(pIni, sz);
	sprintf(sz, "ElemImg%sAwardFinish", szP);	m_Finish.Init(pIni, sz);
	if (bVip)
		m_Lock.Init(pIni, "ElemImgVipAwardLock");
	ClearCell();
}

void KUiCLAwardCell::ReleaseItem()
{
	if (m_nTempItemIdx > 0 && g_pCoreShell)
		g_pCoreShell->GetGameData(GDI_ITEM_CHAT, false, m_nTempItemIdx);
	m_nTempItemIdx = 0;
	m_Box.HoldObject(CGOG_NOTHING, 0, 0, 0);
	m_Box.Hide();
}

void KUiCLAwardCell::ClearCell()
{
	ReleaseItem();
	memset(&m_Data, 0, sizeof(m_Data));
	m_bClaimable = 0;
	m_IconBg.Hide();
	m_Icon.Hide();
	m_Num.Hide();
	m_Gray.Hide();
	m_Light.Hide();
	m_Effect.Hide();
	m_Finish.Hide();
	m_Lock.Hide();
	Show();		// nen o luon hien (2.0: ImgLowAwardBG luon co, chi hide khi khong co moc)
}

void KUiCLAwardCell::Fill(const KCLUiAward* p, int nScore, int nGot, int bVipOk)
{
	ReleaseItem();
	if (!p || p->nIdx <= 0)
	{
		ClearCell();
		Hide();
		return;
	}
	m_Data = *p;
	Show();
	m_IconBg.Show();
	// bieu tuong that qua vat pham tam (GDI_ITEM_CHAT), nhu hop thu / dau gia
	m_Icon.Show();
	if (g_pCoreShell && p->Item.m_nID)
	{
		int nIdx = g_pCoreShell->GetGameData(GDI_ITEM_CHAT, true, (int)&p->Item);
		if (nIdx > 0)
		{
			m_nTempItemIdx = nIdx;
			m_Box.HoldObject(CGOG_PLAYERSELLITEM, (unsigned int)nIdx, 1, 1);
			m_Box.Show();
			m_Icon.Hide();
		}
	}
	if (p->nCount > 1)
	{
		char sz[16];
		sprintf(sz, "%d", p->nCount);
		m_Num.SetText(sz);
		m_Num.Show();
	}
	else
		m_Num.Hide();

	int bClaimed = (p->nIdx <= nGot);
	int bReached = (nScore >= p->nNeedScore);
	int bLocked = (m_bVip && !bVipOk);
	m_bClaimable = 0;
	m_Gray.Hide();
	m_Light.Hide();
	m_Effect.Hide();
	m_Finish.Hide();
	m_Lock.Hide();
	if (bLocked)
	{
		m_Gray.Show();
		m_Lock.Show();
	}
	else if (bClaimed)
		m_Finish.Show();
	else if (bReached)
	{
		m_Light.Show();
		m_Effect.Show();
		m_bClaimable = 1;
	}
	else
		m_Gray.Show();
}

void KUiCLAwardCell::Blink(int nFrame)
{
	if (!m_bClaimable)
		return;
	if ((nFrame / CL_BLINK_FRAMES) & 1)
		m_Effect.Show();
	else
		m_Effect.Hide();
}

//////////////////////////////////////////////////////////////////////
// KUiCLAwardCol
//////////////////////////////////////////////////////////////////////
KUiCLAwardCol::KUiCLAwardCol()
{
	m_nIdx = 0;
}

void KUiCLAwardCol::Build()
{
	AddChild(&m_ProgBg);
	m_ProgBg.AddChild(&m_Prog);
	AddChild(&m_RankBg);
	m_RankBg.AddChild(&m_Rank);
	AddChild(&m_Low);
	AddChild(&m_Vip);
	m_Low.Build();
	m_Vip.Build();
}

void KUiCLAwardCol::LoadScheme(KIniFile* pIni, int nCol)
{
	Init(pIni, "ElemImgRankAwardInfoBG");
	m_ProgBg.Init(pIni, "ElemImgProgressBg");
	m_Prog.Init(pIni, "ElemImgProgress");
	m_RankBg.Init(pIni, "ElemImgAwardRankBG");
	m_Rank.Init(pIni, "ElemTxtAwardRank");
	m_Low.LoadScheme(pIni, 0);
	m_Vip.LoadScheme(pIni, 1);
	// cot i dat tai Left = 72 + i*57 trong ImgAwardListBG (season_ticket_2026.lua:337-340)
	SetPosition(CL_COL_LEFT + nCol * CL_COL_STEP, 0);
	ClearCol();
}

void KUiCLAwardCol::ReleaseItems()
{
	m_Low.ReleaseItem();
	m_Vip.ReleaseItem();
}

void KUiCLAwardCol::ClearCol()
{
	m_nIdx = 0;
	m_Low.ClearCell();
	m_Vip.ClearCell();
	m_Low.Hide();
	m_Vip.Hide();
	m_Prog.SetPart(0, 1);
	m_Rank.SetText("");
	Hide();
}

void KUiCLAwardCol::Fill(const KCLUiAward* p, const KCLUiInfo* pInf, int nPrevNeed)
{
	if (!p || !pInf || p->nIdx <= 0)
	{
		ClearCol();
		return;
	}
	m_nIdx = p->nIdx;
	Show();
	char sz[16];
	sprintf(sz, "LV%d", p->nCap);
	m_Rank.SetText(sz);
	// thanh tien do: tu moc truoc toi moc nay
	int nFull = p->nNeedScore - nPrevNeed;
	int nPart = pInf->nScore - nPrevNeed;
	if (nFull < 1)
		nFull = 1;
	if (nPart < 0)
		nPart = 0;
	if (nPart > nFull)
		nPart = nFull;
	m_Prog.SetPart(nPart, nFull);
	// moi moc chi thuoc MOT nhanh: o kia de trong (2.0: hide IMG_BG cua nhanh khong co)
	if (p->nBranch == CLUI_BRANCH_VIP)
	{
		m_Low.Fill(NULL, 0, 0, 0);
		m_Vip.Fill(p, pInf->nScore, pInf->nGotVip, pInf->nVip);
	}
	else
	{
		m_Low.Fill(p, pInf->nScore, pInf->nGotLow, 1);
		m_Vip.Fill(NULL, 0, 0, 0);
	}
}

//////////////////////////////////////////////////////////////////////
// KUiCLMissionRow
//////////////////////////////////////////////////////////////////////
KUiCLMissionRow::KUiCLMissionRow()
{
	memset(&m_Data, 0, sizeof(m_Data));
}

void KUiCLMissionRow::Build()
{
	AddChild(&m_BgFinish);
	AddChild(&m_IconBg);
	AddChild(&m_Icon);
	AddChild(&m_Num);
	AddChild(&m_Info);
	AddChild(&m_BtnGoto);
	AddChild(&m_BtnGet);
	AddChild(&m_BtnGot);
}

void KUiCLMissionRow::LoadScheme(KIniFile* pIni, int nRow)
{
	Init(pIni, "ElemImgMissionInfoBG");
	m_BgFinish.Init(pIni, "ElemImgMissionInfoBG_Finish");
	m_IconBg.Init(pIni, "ElemImgMissionAwardIconBG");
	m_Icon.Init(pIni, "ElemImgMissionAwardIcon");
	m_Num.Init(pIni, "ElemTxtMissionAwardIconNum");
	m_Info.Init(pIni, "ElemTxtMissionInfo");
	m_BtnGoto.Init(pIni, "ElemBtnGoToMissionArea");
	m_BtnGet.Init(pIni, "ElemBtnGetMissionAward");
	m_BtnGot.Init(pIni, "ElemBtnHaveGetAward");
	m_BtnGot.Enable(0);
	// hang i tai Top = 3 + i*40 (season_ticket_2026.lua:386-388)
	SetPosition(CL_ROW_TOP, CL_ROW_TOP + nRow * CL_ROW_STEP);
	ClearRow();
}

void KUiCLMissionRow::ClearRow()
{
	memset(&m_Data, 0, sizeof(m_Data));
	m_BgFinish.Hide();
	m_IconBg.Hide();
	m_Icon.Hide();
	m_Num.Hide();
	m_Info.SetText("");
	m_BtnGoto.Hide();
	m_BtnGet.Hide();
	m_BtnGot.Hide();
	Hide();
}

void KUiCLMissionRow::Fill(const KCLUiMission* p)
{
	if (!p || p->nId <= 0)
	{
		ClearRow();
		return;
	}
	m_Data = *p;
	Show();
	m_IconBg.Show();
	m_Icon.Show();
	char sz[16];
	sprintf(sz, "%d", p->nScore);
	m_Num.SetText(sz);
	m_Num.Show();
	// tieu de + tien do (khi muc tieu > 1)
	char szInfo[CLUI_TITLE_LEN + 32];
	if (p->nTarget > 1 && p->nState == CLUI_NV_CHUA)
		sprintf(szInfo, "%s (%d/%d)", p->szTitle, p->nProg < p->nTarget ? p->nProg : p->nTarget, p->nTarget);
	else
		strcpy(szInfo, p->szTitle);
	m_Info.SetText(szInfo);
	// ba nut chong nhau, chi hien MOT (2.0: Den / Nhan / Da nhan)
	m_BtnGoto.Hide();
	m_BtnGet.Hide();
	m_BtnGot.Hide();
	m_BgFinish.Hide();
	switch (p->nState)
	{
	case CLUI_NV_LINH:
		m_BgFinish.Show();
		m_BtnGot.Show();
		break;
	case CLUI_NV_XONG:
		m_BtnGet.Show();
		break;
	default:
		m_BtnGoto.Show();
		break;
	}
}

//////////////////////////////////////////////////////////////////////
// KUiChienLenh
//////////////////////////////////////////////////////////////////////
KUiChienLenh* KUiChienLenh::ms_pSelf = NULL;

KUiChienLenh::KUiChienLenh()
{
	memset(&m_Info, 0, sizeof(m_Info));
	memset(m_Award, 0, sizeof(m_Award));
	memset(m_Mission, 0, sizeof(m_Mission));
	m_nAward = 0;
	m_nPage = 0;
	m_nTab = CLUI_KIND_DAY;
	m_nScrollTop = 0;
	m_nBlinkFrame = 0;
	m_nLastRemainSec = -1;
	m_nMsgLeft = 0;
	m_szScheme[0] = 0;
}

KUiChienLenh::~KUiChienLenh()
{
	ms_pSelf = NULL;
}

KUiChienLenh* KUiChienLenh::Prepare()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiChienLenh;
		if (ms_pSelf)
		{
			ms_pSelf->Initialize();
			ms_pSelf->Hide();
		}
	}
	return ms_pSelf;
}

KUiChienLenh* KUiChienLenh::OpenWindow()
{
	Prepare();
	if (ms_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->BringToTop();
		ms_pSelf->Show();
		ms_pSelf->Refresh();
	}
	return ms_pSelf;
}

KUiChienLenh* KUiChienLenh::GetIfVisible()
{
	return (ms_pSelf && ms_pSelf->IsVisible()) ? ms_pSelf : NULL;
}

void KUiChienLenh::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		if (ms_pSelf->IsVisible())
			UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->Hide();
		sSendOp(CLUI_OP_CLOSE, 0);
		if (bDestory)
		{
			for (int i = 0; i < CLUI_COL_COUNT; i++)
				ms_pSelf->m_Col[i].ReleaseItems();
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}

void KUiChienLenh::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_Close);
	AddChild(&m_Help);
	AddChild(&m_Remain);
	AddChild(&m_BtnBuy);
	AddChild(&m_AwardBg);
	m_AwardBg.AddChild(&m_RankTitle);
	m_AwardBg.AddChild(&m_Rank);
	m_AwardBg.AddChild(&m_LowTitle);
	m_AwardBg.AddChild(&m_VipTitle);
	int i;
	for (i = 0; i < CLUI_COL_COUNT; i++)
	{
		m_AwardBg.AddChild(&m_Col[i]);
		m_Col[i].Build();
	}
	AddChild(&m_Prev);
	AddChild(&m_Next);
	AddChild(&m_DayTabBg);
	m_DayTabBg.AddChild(&m_DayTab);
	AddChild(&m_WeekTabBg);
	m_WeekTabBg.AddChild(&m_WeekTab);
	AddChild(&m_ListBg);
	m_ListBg.AddChild(&m_Scroll);
	for (i = 0; i < CLUI_ROW_COUNT; i++)
	{
		m_ListBg.AddChild(&m_Row[i]);
		m_Row[i].Build();
	}
	AddChild(&m_Msg);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this);
}

void KUiChienLenh::LoadScheme(const char* pScheme)
{
	if (!ms_pSelf)
		return;
	char Buff[256];
	KIniFile Ini;
	sprintf(Buff, "%s\\%s", pScheme, CL_MAIN_INI);
	strncpy(ms_pSelf->m_szScheme, pScheme, sizeof(ms_pSelf->m_szScheme) - 1);
	if (!Ini.Load(Buff))
		return;
	KUiChienLenh* p = ms_pSelf;
	p->Init(&Ini, "Main");
	p->m_Title.Init(&Ini, "TxtTitle");
	p->m_Close.Init(&Ini, "BtnClose");
	p->m_Help.Init(&Ini, "BtnHelp");
	p->m_Remain.Init(&Ini, "TxtRemainTime");
	p->m_BtnBuy.Init(&Ini, "BtnBuyVipCard");
	p->m_AwardBg.Init(&Ini, "ImgAwardListBG");
	p->m_RankTitle.Init(&Ini, "TxtPlayerRankTitle");
	p->m_Rank.Init(&Ini, "TxtPlayerRank");
	p->m_LowTitle.Init(&Ini, "ImgLowAwardTitle");
	p->m_VipTitle.Init(&Ini, "ImgVipAwardTitle");
	int i;
	for (i = 0; i < CLUI_COL_COUNT; i++)
		p->m_Col[i].LoadScheme(&Ini, i);
	p->m_Prev.Init(&Ini, "BtnPrevPage");
	p->m_Next.Init(&Ini, "BtnNextPage");
	p->m_DayTabBg.Init(&Ini, "ImgDailyMissionBG");
	p->m_DayTab.Init(&Ini, "TxtDailyMission");
	p->m_WeekTabBg.Init(&Ini, "ImgWeeklyMissionBG");
	p->m_WeekTab.Init(&Ini, "TxtWeeklyMission");
	p->m_ListBg.Init(&Ini, "DlyList");
	p->m_Scroll.Init(&Ini, "DlyScr");
	p->m_Scroll.SetValueRange(0, 0);
	for (i = 0; i < CLUI_ROW_COUNT; i++)
		p->m_Row[i].LoadScheme(&Ini, i);
	// dong thong bao ngan: dung mau [TxtRemainTime], dat ngay duoi
	p->m_Msg.Init(&Ini, "TxtRemainTime");
	int nX = 0, nY = 0;
	p->m_Remain.GetPosition(&nX, &nY);
	p->m_Msg.SetPosition(nX, nY + 16);
	p->m_Msg.SetText("");
	p->m_Msg.Hide();
	p->Refresh();
}

// ---- du lieu tu Core ----
void KUiChienLenh::ClearData()
{
	memset(m_Award, 0, sizeof(m_Award));
	memset(m_Mission, 0, sizeof(m_Mission));
	m_nAward = 0;
}

void KUiChienLenh::SetInfo(const KCLUiInfo* p)
{
	if (p)
		m_Info = *p;
}

void KUiChienLenh::SetAward(const KCLUiAward* p)
{
	if (!p || p->nIdx < 1 || p->nIdx > CLUI_MAX_AWARD)
		return;
	m_Award[p->nIdx - 1] = *p;
	if (p->nIdx > m_nAward)
		m_nAward = p->nIdx;
}

void KUiChienLenh::SetMission(const KCLUiMission* p)
{
	if (!p || p->nId < 1 || p->nId > CLUI_MAX_MISSION)
		return;
	m_Mission[p->nId] = *p;
}

void KUiChienLenh::ShowMsg(const char* szMsg)
{
	if (!szMsg)
		return;
	m_Msg.SetText(szMsg);
	m_Msg.Show();
	m_nMsgLeft = CL_MSG_FRAMES;
}

int KUiChienLenh::MissionCount(int nKind)
{
	int n = 0;
	for (int i = 1; i <= CLUI_MAX_MISSION; i++)
		if (m_Mission[i].nId == i && m_Mission[i].nKind == nKind)
			n++;
	return n;
}

const KCLUiMission* KUiChienLenh::MissionAt(int nKind, int nPos)
{
	int n = 0;
	for (int i = 1; i <= CLUI_MAX_MISSION; i++)
	{
		if (m_Mission[i].nId == i && m_Mission[i].nKind == nKind)
		{
			if (n == nPos)
				return &m_Mission[i];
			n++;
		}
	}
	return NULL;
}

void KUiChienLenh::RefreshAwards()
{
	char sz[32];
	sprintf(sz, "LV%d", m_Info.nCap);
	m_Rank.SetText(sz);
	int nPages = (m_nAward + CLUI_COL_COUNT - 1) / CLUI_COL_COUNT;
	if (nPages < 1)
		nPages = 1;
	if (m_nPage >= nPages)
		m_nPage = nPages - 1;
	if (m_nPage < 0)
		m_nPage = 0;
	for (int i = 0; i < CLUI_COL_COUNT; i++)
	{
		int k = m_nPage * CLUI_COL_COUNT + i;		// 0-based chi so moc
		if (k < m_nAward && m_Award[k].nIdx > 0)
		{
			int nPrev = (k > 0) ? m_Award[k - 1].nNeedScore : 0;
			m_Col[i].Fill(&m_Award[k], &m_Info, nPrev);
		}
		else
			m_Col[i].ClearCol();
	}
	m_Prev.Enable(m_nPage > 0);
	m_Next.Enable(m_nPage < nPages - 1);
}

void KUiChienLenh::RefreshMissions()
{
	int nTotal = MissionCount(m_nTab);
	int nMax = nTotal - CLUI_ROW_COUNT;
	if (nMax < 0)
		nMax = 0;
	if (m_nScrollTop > nMax)
		m_nScrollTop = nMax;
	if (m_nScrollTop < 0)
		m_nScrollTop = 0;
	m_Scroll.SetValueRange(0, nMax);
	m_Scroll.SetScrollPos(m_nScrollTop);
	for (int i = 0; i < CLUI_ROW_COUNT; i++)
		m_Row[i].Fill(MissionAt(m_nTab, m_nScrollTop + i));
	// the dang chon giu trang thai "nhan" (khung Down cua nut tab)
	m_DayTabBg.CheckButton(m_nTab == CLUI_KIND_DAY);
	m_WeekTabBg.CheckButton(m_nTab == CLUI_KIND_WEEK);
}

void KUiChienLenh::RefreshRemain()
{
	char sz[128];
	int nNow = (int)time(NULL);
	int nLeft = m_Info.nCloseTime - nNow;
	if (m_Info.nCloseTime <= 0)
		nLeft = 0;
	if (nLeft == m_nLastRemainSec)
		return;
	m_nLastRemainSec = nLeft;
	if (nLeft <= 0)
		sprintf(sz, "%s%s", CL_TXT_CON, CL_TXT_HET);
	else
	{
		int d = nLeft / 86400;
		int h = (nLeft / 3600) % 24;
		int m = (nLeft / 60) % 60;
		int s = nLeft % 60;
		sprintf(sz, "%s%d %s %d %s %d %s %d %s", CL_TXT_CON,
			d, CL_TXT_NGAY, h, CL_TXT_GIO, m, CL_TXT_PHUT, s, CL_TXT_GIAY);
	}
	m_Remain.SetText(sz);
}

void KUiChienLenh::Refresh()
{
	RefreshAwards();
	RefreshMissions();
	m_nLastRemainSec = -1;
	RefreshRemain();
	// da co Hao Hoa thi nut Mua tat
	m_BtnBuy.Enable(m_Info.nVip ? 0 : 1);
}

void KUiChienLenh::Breathe()
{
	if (!IsVisible())
		return;
	m_nBlinkFrame++;
	for (int i = 0; i < CLUI_COL_COUNT; i++)
	{
		m_Col[i].m_Low.Blink(m_nBlinkFrame);
		m_Col[i].m_Vip.Blink(m_nBlinkFrame);
	}
	if ((m_nBlinkFrame % 18) == 0)
		RefreshRemain();
	if (m_nMsgLeft > 0)
	{
		m_nMsgLeft--;
		if (m_nMsgLeft == 0)
			m_Msg.Hide();
	}
}

int KUiChienLenh::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int i;
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Close)
		{
			CloseWindow(false);
			return 1;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_Help)
		{
			sSendOp(CLUI_OP_HELP, 0);
			return 1;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_BtnBuy)
		{
			sSendOp(CLUI_OP_BUY_VIP, 0);
			return 1;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_Prev)
		{
			if (m_nPage > 0)
				m_nPage--;
			RefreshAwards();
			return 1;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_Next)
		{
			m_nPage++;
			RefreshAwards();
			return 1;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_DayTabBg)
		{
			m_nTab = CLUI_KIND_DAY;
			m_nScrollTop = 0;
			RefreshMissions();
			return 1;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_WeekTabBg)
		{
			m_nTab = CLUI_KIND_WEEK;
			m_nScrollTop = 0;
			RefreshMissions();
			return 1;
		}
		// o thuong: bam vao Light (nut) cua nhanh Thuong / Hao Hoa
		for (i = 0; i < CLUI_COL_COUNT; i++)
		{
			KUiCLAwardCell* pCell = NULL;
			if (uParam == (unsigned int)(KWndWindow*)&m_Col[i].m_Low.m_Light ||
				uParam == (unsigned int)(KWndWindow*)&m_Col[i].m_Low.m_Icon)
				pCell = &m_Col[i].m_Low;
			else if (uParam == (unsigned int)(KWndWindow*)&m_Col[i].m_Vip.m_Light ||
				uParam == (unsigned int)(KWndWindow*)&m_Col[i].m_Vip.m_Icon)
				pCell = &m_Col[i].m_Vip;
			if (pCell)
			{
				if (pCell->m_bClaimable && pCell->m_Data.nIdx > 0)
					sSendReq(CLUI_OP_GET_AWARD, pCell->m_Data.nIdx, pCell->m_Data.nBranch);
				return 1;
			}
		}
		// dong nhiem vu: Nhan / Den
		for (i = 0; i < CLUI_ROW_COUNT; i++)
		{
			if (m_Row[i].m_Data.nId <= 0)
				continue;
			if (uParam == (unsigned int)(KWndWindow*)&m_Row[i].m_BtnGet)
			{
				sSendOp(CLUI_OP_GET_MISSION, m_Row[i].m_Data.nId);
				return 1;
			}
			if (uParam == (unsigned int)(KWndWindow*)&m_Row[i].m_BtnGoto)
			{
				// "Den": hien goi y cach lam (tips tu may chu) roi bao Lua
				if (m_Row[i].m_Data.szTips[0])
					ShowMsg(m_Row[i].m_Data.szTips);
				sSendOp(CLUI_OP_GOTO_MISSION, m_Row[i].m_Data.nId);
				return 1;
			}
		}
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_Scroll)
		{
			m_nScrollTop = m_Scroll.GetScrollPos();
			for (i = 0; i < CLUI_ROW_COUNT; i++)
				m_Row[i].Fill(MissionAt(m_nTab, m_nScrollTop + i));
			return 1;
		}
		break;
	default:
		break;
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}

//////////////////////////////////////////////////////////////////////
// KUiCLIcon - bieu tuong tren HUD (2.0: goc tren phai, 835,60 o 1024)
//////////////////////////////////////////////////////////////////////
KUiCLIcon* KUiCLIcon::ms_pSelf = NULL;

KUiCLIcon::KUiCLIcon()
{
}

void KUiCLIcon::Initialize()
{
	AddChild(&m_Btn);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this);
}

void KUiCLIcon::LoadScheme(const char* pScheme)
{
	if (!ms_pSelf)
		return;
	char Buff[256];
	KIniFile Ini;
	sprintf(Buff, "%s\\%s", pScheme, CL_ICON_INI);
	if (Ini.Load(Buff))
	{
		ms_pSelf->Init(&Ini, "Main");
		ms_pSelf->m_Btn.Init(&Ini, "BtnIcon");
		// ini la ban 800 (Left=610); ban 1024 cua 2.0 dat Left=835, Top=60
		if (SCREEN_WIDTH == 1024)
			ms_pSelf->SetPosition(835, 60);
	}
}

void KUiCLIcon::SetVisible(int bVisible)
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiCLIcon;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (!ms_pSelf)
		return;
	if (bVisible)
	{
		ms_pSelf->m_Btn.Show();
		ms_pSelf->Show();
	}
	else
		ms_pSelf->Hide();
}

int KUiCLIcon::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_BUTTON_CLICK && uParam == (unsigned int)(KWndWindow*)&m_Btn)
	{
		KUiChienLenh* p = KUiChienLenh::GetIfVisible();
		if (p)
			KUiChienLenh::CloseWindow(false);
		else
			sSendOp(CLUI_OP_OPEN, 0);	// xin may chu day du lieu; may chu tra CLUi_Open
		return 1;
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}

void KUiCLIcon::Release()
{
	if (ms_pSelf)
	{
		ms_pSelf->Destroy();
		ms_pSelf = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// Core -> UI
//////////////////////////////////////////////////////////////////////
static KUiChienLenh* sEnsure()
{
	return KUiChienLenh::Prepare();
}

void KUiChienLenh_OnCoreCmd(unsigned int uCmd, int nParam)
{
	KUiChienLenh* p;
	if (!s_bGameStarted)
	{
		switch (uCmd)
		{
		case CLUI_CMD_OPEN:
			s_bPendingOpen = 1;
			return;
		case CLUI_CMD_ICON_VISIBLE:
			s_nPendingIcon = nParam;
			return;
		default:
			break;
		}
	}
	switch (uCmd)
	{
	case CLUI_CMD_OPEN:
		KUiChienLenh::OpenWindow();
		break;
	case CLUI_CMD_CLOSE:
		KUiChienLenh::CloseWindow(false);
		break;
	case CLUI_CMD_ICON_VISIBLE:
		KUiCLIcon::SetVisible(nParam);
		break;
	case CLUI_CMD_SET_INFO:
		p = sEnsure();
		if (p)
			p->SetInfo((const KCLUiInfo*)nParam);
		break;
	case CLUI_CMD_SET_AWARD:
		p = sEnsure();
		if (p)
			p->SetAward((const KCLUiAward*)nParam);
		break;
	case CLUI_CMD_SET_MISSION:
		p = sEnsure();
		if (p)
			p->SetMission((const KCLUiMission*)nParam);
		break;
	case CLUI_CMD_CLEAR:
		p = sEnsure();
		if (p)
			p->ClearData();
		break;
	case CLUI_CMD_REFRESH:
		p = KUiChienLenh::GetSelf();
		if (p)
			p->Refresh();
		break;
	case CLUI_CMD_MSG:
		p = KUiChienLenh::GetSelf();
		if (p)
			p->ShowMsg((const char*)nParam);
		break;
	default:
		break;
	}
}

void KUiChienLenh_OnGameStart()
{
	s_bGameStarted = 1;
	if (s_nPendingIcon >= 0)
		KUiCLIcon::SetVisible(s_nPendingIcon);
	if (s_bPendingOpen)
		KUiChienLenh::OpenWindow();
	s_bPendingOpen = 0;
	s_nPendingIcon = -1;
}

void KUiChienLenh_OnGameExit()
{
	s_bGameStarted = 0;
	s_bPendingOpen = 0;
	s_nPendingIcon = -1;
	KUiChienLenh::CloseWindow(true);
	KUiCLIcon::Release();
	sSendOp(CLUI_OP_RESET, 0);
}
