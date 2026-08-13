/*******************************************************************************
File        : UiTongJX2.cpp
Description : Cua so bang hoi kieu JX2 - hien du lieu ban sao GS (goi
              TONG_JX2_*_SYNC) va gui thao tac (GTOI_TONG_JX2_VIEW/OP).
              Mo tu script server bang OpenTongJX2() (UI_OPENTONGJX2).
********************************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "KPlayerDef.h"

#include "../elem/wnds.h"
#include "../elem/wndmessage.h"

#include "../UiBase.h"

#include "UiTongJX2.h"
#include "../elem/wndedit.h"
#include "../elem/wndlist2.h"
#include "../elem/wndscrollbar.h"
#include "UiTongManager.h"
#include "UiTongCreateSheet.h"

#include "../../Core/Src/KProtocol.h"
#include "../../../Headers/KProtocolDef.h"

#define TONG_JX2_INI	"UiTongJX2.ini"

// nhat ky chan doan cua so bang hoi (jx_tongjx2.log canh Game.exe)
static void sTJX2Log(const char* fmt, ...)
{
	FILE* f = fopen("jx_tongjx2.log", "a");
	if (!f)
		return;
	va_list ap;
	va_start(ap, fmt);
	vfprintf(f, fmt, ap);
	va_end(ap);
	fprintf(f, "\n");
	fclose(f);
}

extern iCoreShell* g_pCoreShell;
extern int SCREEN_WIDTH;

KUiTongJX2* KUiTongJX2::ms_pSelf = NULL;

static const char* s_szTabName[TJX2_UI_TABS] =
{
	"Thong tin", "Thanh vien", "Quyen han", "Tac phuong", "Thong bao",
};

// ten 12 quyen theo thu tu mat na (KProtocol.h)
static const char* s_szRightName[12] =
{
	"1000 giao quyen", "1003 doi phe", "1101 quan ly", "1901 duoi nguoi",
	"1902 thoai an", "2001 nang cap", "2004 lanh dia", "2005 muc tieu tuan",
	"2006 tuyet ky", "2007 tuyen chien", "3001 quy bang", "9001 tac phuong",
};
static const DWORD s_dwRightId[12] =
{
	1000, 1003, 1101, 1901, 1902, 2001, 2004, 2005, 2006, 2007, 3001, 9001,
};

static const char* s_szWsName[8] =
{
	"", "Binh Giap", "Thien Cong", "Mat Na", "Thi Luyen", "Thien Y", "Le Vat", "Hoat Dong",
};

// ten 10 mon phai JX1 (id 1..10) - TCVN3
static const char* s_szFaction[11] =
{
	"-", "Thi\325u L\251m", "Thi\252n V\255\254ng", "\247\255\352ng M\253n",
	"Ng\362 \247\351c", "Nga My", "Th\363y Y\252n", "C\270i Bang",
	"Thi\252n Nh\311n", "V\342 \247ang", "C\253n L\253n",
};

static const char* s_szFigure[5] =
{
	"Bang ch\361", "Tr\255\353ng l\267o", "\247\351i tr\255\353ng", "Bang ch\363ng", "An si",
};

// Trang Tin tuc: bind NGUYEN VAN section blueprint 帮会基础信息页面.ini
// (Info_<ten section>); nhan Title dat chuoi TCVN3, Txt do RenderInfo dien.
struct TJX2InfoCtl
{
	const char* szSec;		// ten section goc trong blueprint
	const char* szLabel;	// nhan TCVN3 (NULL = o du lieu)
};
static const TJX2InfoCtl s_sInfoCtl[32] =
{
	{"TitleTongInfo",    "Th\253ng tin bang h\351i"},
	{"TitleTongName",    "T\252n bang"},        {"TxtTongName",    NULL},
	{"TitleMaster",      "Bang ch\361"},        {"TxtMaster",      NULL},
	{"TitleLeague",      "Li\252n minh"},       {"TxtLeague",      NULL},
	{"TitleCamp",        "Phe ph\270i"},        {"TxtCamp",        NULL},
	{"TitleTongLevel",   "C\312p bang"},        {"TxtTongLevel",   NULL},
	{"TitleMemberNum",   "Th\265nh vi\252n"},   {"TxtMemberNum",   NULL},
	{"TitleTongCapital", "Ng\251n qu\374 bang"},{"TxtTongCapital", NULL},
	{"TitleBuildFund",   "Qu\374 ki\325n thi\325t"},{"TxtBuildFund", NULL},
	{"TitleBattleFund",  "Qu\374 chi\325n b\336"},{"TxtBattleFund", NULL},
	{"TitleServiceFee",  "Ph\335 duy tr\327"},  {"TxtServiceFee",  NULL},
	{"TitleStandFund",   "Tr\356 c\312p"},      {"TxtStandFund",   NULL},
	{"TitleTotalOffer",  "Qu\374 d\371 tr\367"},{"TxtTotalOffer",  NULL},
	{"TitleStoredBuildFund", "Ki\325n thi\325t d\371 tr\367"},{"TxtStoredBuildFund", NULL},
	{"TitlePersonalInfo","Th\253ng tin c\270 nh\251n"},
	{"TitlePersonalOffer","C\350ng hi\325n"},   {"TxtPersonalOffer", NULL},
	{"TitleLiveness",    "Kinh nghi\326m bang"},{"TxtLiveness",    NULL},
};

// 14 nut quyen cua blueprint trang Phan phoi (RightID doc tu chinh ini)
static const char* s_szRtSec[14] =
{
	"BtnDepose", "BtnChangeCamp", "BtnChangeTitle", "BtnKickOut",
	"BtnRecordEvent", "BtnLeagueManage", "BtnUpgradeBuildLevel", "BtnForceToRetire",
	"BtnMapManagement", "BtnWorkshopManagement", "BtnFundManagement",
	"BtnWeekGoalManagement", "BtnCityManagement", "BtnStuntManagement",
};

// nhan 12 o kiem quyen (cung thu tu mat na s_dwRightId) - TCVN3 co dau
static const char* s_szChkLabel[12] =
{
	"Giao quy\322n", "\247\346i phe", "Qu\266n l\375", "Tr\364c xu\312t",
	"Tho\270i \310n", "N\251ng c\312p", "L\267nh \256\336a", "M\364c ti\252u tu\307n",
	"Tuy\326t k\374", "Tuy\252n chi\325n", "Ng\251n qu\374", "T\270c ph\255\352ng",
};

// Nhan trang Chieu mo: NGUYEN VAN byte TCVN3 tu blueprint (Text= cua tung section)
struct TJX2RecLbl { const char* szSec; const char* szLabel; };
static const TJX2RecLbl s_sRecLbl[8] =
{
	{"JiyuTitle",       "Tin nh\276n bang h\351i:"},
	{"ZhaoMuTitle",     "Tin t\370c chi\252u m\351 bang h\351i"},
	{"QingXiangTitle",  "Khuynh h\255\355ng ch\361 y\325u bang h\351i:"},
	{"HuoDongTitle",    "Ho\271t \256\351ng ch\361 y\325u bang h\351i:"},
	{"AutoAcceptTitle", "Cao h\254n ______c\312p t\371 \256\351ng nh\313p bang"},
	{"RefuseLevelTitle","T\365 ch\350i gamer d\255\355i______ c\312p xin nh\313p bang"},
	{"ApplyTitle",      "Danh s\270ch th\265nh vi\252n xin gia nh\313p bang h\351i"},
	{"ApplyerName",     "T\252n gamer xin gia nh\313p             \247\274ng c\312p"},
};
#define TJX2_UI_PAGE_RECRUIT	5	// trang UI chieu mo (goi server PAGE_RECRUIT)
// Trang Su dung chuc nang: bind nguyen van section blueprint (Fun_<ten goc>).
#define TJX2_UI_PAGE_FUNUSE	6
#define TJX2_UI_PAGE_TONGLIST	7	// danh sach bang (mo duoc khi CHUA vao bang)
struct TJX2FunTxt { const char* szSec; const char* szLabel; };	// szLabel NULL = o du lieu
static const TJX2FunTxt s_sFunTxt[15] =
{
	{"TitleTongInfo", "Tin t\370c"},
	{"TitleTongName", "T\252n bang"},        {"TxtTongName", NULL},
	{"TitleTongUnion", "Li\252n minh"},      {"TxtTongUnion", NULL},
	{"TitleBuildLevel", "\247\274ng c\312p"}, {"TxtBuildLevel", NULL},
	{"TitleTotalOffer", "T\346ng c\350ng hi\325n"}, {"TxtTotalOffer", NULL},
	{"TitleTongMoney", "Ng\251n qu\374"},    {"TxtTongMoney", NULL},
	{"TitleBuildFund", "Qu\374 ki\325n thi\325t"}, {"TxtBuildFund", NULL},
	{"TitleBattleFund", "Qu\374 chi\325n b\336"},  {"TxtBattleFund", NULL},
};
// 3 o cuoi khoi trai + khoi ca nhan dung chung m_FunTxt? khong du cho - gop vao bang nut du lieu:
struct TJX2FunBtn { const char* szSec; const char* szLabel; int nAct; };
// nAct: 0 nang cap bang / 1 phat cong hien / 2 duoi / 3 truat chuc / 4 chieu mo
//       5 tao lanh dia / 6 xoa lanh dia / 7 cat cong hien / 8 roi bang / -1 chua mo
static const TJX2FunBtn s_sFunBtn[14] =
{
	{"BtnUpgradeBuildLevel", "N\251ng c\312p", 0},
	{"BtnAssignTongOffer",   "Ph\270t", 1},
	{"BtnGetTongMoney",      "Nh\313n", 9},
	{"BtnAssignTongMoney",   "Ph\270t", 10},
	{"BtnTransformMoney",    "\247\346i", -1},
	{"BtnRecruit",           "Chi\252u m\351", 4},
	{"BtnKickOut",           "\247u\346i ng\255\352i", 2},
	{"BtnDepose",            "Tru\312t ch\370c", 3},
	{"Btn_DispenseOffer",    "Ph\270t c\350ng hi\325n", 1},
	{"BtnCreateTongMap",     "T\271o l\267nh \256\336a", 5},
	{"BtnConfigureTongMap",  "", -1},
	{"BtnTongStunt",         "", -1},
	{"BtnStorePersonalOffer","C\312t", 7},
	{"BtnLeaveTong",         "R\352i bang", 8},
};

// Trang Phuong tho: bind blueprint (Ws_*). Title co nhan TCVN3, Txt do render dien.
static const TJX2FunTxt s_sWsTxt[6] =
{
	{"TitleWorkshopNum", "S\350 khu"},      {"TxtWorkshopNum", NULL},
	{"TitleMaxRank", "H\271ng t\350i \256a"}, {"TxtMaxRank", NULL},
	{"TitleServiceFee", "Ph\335 duy tr\327"}, {"TxtServiceFee", NULL},
};
static const char* s_szWsBtnSec[6] =
{
	"BtnLearnWorkshop", "BtnOpenWorkshop", "BtnCloseWorkshop",
	"BtnUpgradeWorkshop", "BtnSetUseLevelWorkshop", "BtnDeleteWorkshop",
};
static const char* s_szWsBtnLbl[6] =
{
	"L\313p khu", "M\353 khu", "\247\343ng khu",
	"N\251ng c\312p khu", "\247\306t c\312p d\357ng", "X\343a khu",
};

static const char* s_szRcSub[4] =
{
	"M\364c ti\252u tu\307n", "Th\253ng b\270o", "Bang v\364", "L\336ch s\366",
};

//////////////////////////////////////////////////////////////////////

KUiTongJX2::KUiTongJX2()
{
	m_nPage = defTONG_JX2_PAGE_INFO;
	m_nStart = 0;
	m_nSel = 0;
	m_bHasInfo = 0;
	m_bHasMember = 0;
	m_bHasWs = 0;
	m_bHasRecruit = 0;
	m_bHasRecord = 0;
	m_bHasList = 0;
	memset(m_byList, 0, sizeof(m_byList));
	m_nRcSub = 2;	// mac dinh Bang vu (so su kien)
	m_nFunMode = 0;
	m_nFunSub = 1;
	m_bMDet = 0;
	memset(m_dwRtId, 0, sizeof(m_dwRtId));
	memset(m_byRecord, 0, sizeof(m_byRecord));
	m_nRecQX = 0;
	memset(m_nRecHD, 0, sizeof(m_nRecHD));
	memset(m_byRecruit, 0, sizeof(m_byRecruit));
	memset(m_byInfo, 0, sizeof(m_byInfo));
	memset(m_byMember, 0, sizeof(m_byMember));
	memset(m_byWs, 0, sizeof(m_byWs));
}

KUiTongJX2::~KUiTongJX2()
{
}

KUiTongJX2* KUiTongJX2::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiTongJX2;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		ms_pSelf->Show();
		ms_pSelf->m_nFunMode = 0;
		ms_pSelf->SwitchPage(TJX2_UI_PAGE_FUNUSE);	// trang Tin tuc kieu Linux
	}
	return ms_pSelf;
}

// Bam icon bang hoi: toggle. GTOI_TONG_JX2_VIEW tra 0 khi CHUA vao bang
// (Core client kiem m_dwTongNameID) -> mo cua so cu de tao / xin vao bang.
void KUiTongJX2::ToggleFromIcon()
{
	if (GetIfVisible())
	{
		CloseWindow(false);
		return;
	}
	if (g_pCoreShell && g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW, defTONG_JX2_PAGE_INFO, 0))
		OpenWindow();
	else
	{
		// chua vao bang: mo cua so JX2 trang Danh sach bang (xem + xin gia nhap);
		// nut Tao bang tren trang nay mo don tao bang he cu
		KUiTongJX2* pWnd = OpenWindow();
		if (pWnd)
			pWnd->SwitchPage(TJX2_UI_PAGE_TONGLIST);
	}
}

KUiTongJX2* KUiTongJX2::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}

void KUiTongJX2::CloseWindow(bool bDestroy)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		if (bDestroy)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}

void KUiTongJX2::Initialize()
{
	int i;
	// nen phan trang add TRUOC de chu/nut ve de len tren
	for (i = 0; i < TJX2_UI_TABS; i++)
		AddChild(&m_PageBg[i]);
	for (i = 0; i < 14; i++)
		AddChild(&m_Rt[i]);
	AddChild(&m_RtAll);
	AddChild(&m_RtApply);
	for (i = 0; i < 3; i++)
		AddChild(&m_ColHdr[i]);
	for (i = 0; i < 4; i++)
		AddChild(&m_Bot[i]);
	AddChild(&m_RecToggle);
	for (i = 0; i < TJX2_UI_TABS; i++)
		AddChild(&m_BtnTab[i]);
	for (i = 0; i < TJX2_UI_ROWS; i++)
		AddChild(&m_Row[i]);
	for (i = 0; i < 32; i++)
		AddChild(&m_Info[i]);
	for (i = 0; i < TJX2_UI_ROWS; i++)
		AddChild(&m_BtnRowSel[i]);
	for (i = 0; i < TJX2_UI_ACTS; i++)
		AddChild(&m_BtnAct[i]);
	AddChild(&m_BtnPrev);
	AddChild(&m_BtnNext);
	AddChild(&m_FunBg);
	for (i = 0; i < 15; i++)
		AddChild(&m_FunTxtBg[i]);
	for (i = 0; i < 2; i++)
		AddChild(&m_FunPBg[i]);
	for (i = 0; i < 15; i++)
		AddChild(&m_FunTxt[i]);
	for (i = 0; i < 3; i++)
		AddChild(&m_FunP[i]);
	for (i = 0; i < 14; i++)
		AddChild(&m_RowDim[i]);
	for (i = 0; i < 14; i++)
		AddChild(&m_FunBtn[i]);
	for (i = 0; i < 4; i++)
		AddChild(&m_FunSub[i]);
	AddChild(&m_BtnFun);
	for (i = 0; i < 6; i++)
		AddChild(&m_WsTxt[i]);
	for (i = 0; i < 6; i++)
		AddChild(&m_WsBtn[i]);
	for (i = 1; i <= 7; i++)
		AddChild(&m_WsIcon[i]);
	AddChild(&m_WsSel);
	for (i = 0; i < 4; i++)
		AddChild(&m_RcSub[i]);
	AddChild(&m_RcEditor);
	AddChild(&m_RcLeaveWord);
	AddChild(&m_RcSave);
	AddChild(&m_BtnList);
	for (i = 0; i < 7; i++)
		AddChild(&m_MDet[i]);
	for (i = 0; i < 8; i++)
		AddChild(&m_RecLbl[i]);
	AddChild(&m_RecJiyu);
	AddChild(&m_RecAuto);
	AddChild(&m_RecRefuse);
	AddChild(&m_RecQX);
	for (i = 0; i < 4; i++)
		AddChild(&m_RecHD[i]);
	AddChild(&m_RecSave);
	AddChild(&m_RecAccept);
	AddChild(&m_RecDeny);
	AddChild(&m_RecPrev);
	AddChild(&m_RecNext);
	AddChild(&m_BtnClose);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	// tab dung anh 招募页vn cua ban Linux (chu Viet nung san trong anh) -> khong ve chu de
	for (i = 0; i < TJX2_UI_TABS; i++)
		m_BtnTab[i].SetLabel("");
	// m_BtnPrev/m_BtnNext: nhan tu ini (Label blueprint)
	// nhan cac nut lay TRUC TIEP tu key Label trong ini (blueprint nung san)
	m_RecQX.SetLabel("0");
	for (i = 0; i < 4; i++)
		m_RecHD[i].SetLabel("0");
	m_BtnFun.SetLabel("");	// sprite nut co chu san tren cua so chinh
	// m_WsBtn: nhan tu ini
	for (i = 1; i <= 7; i++)
	{
		char szN[8];
		sprintf(szN, "%d", i);
		m_WsIcon[i].SetLabel(szN);
	}
	for (i = 0; i < 4; i++)
		m_RcSub[i].SetLabel(s_szRcSub[i]);
	m_RcLeaveWord.SetLabel("L\255u l\352i");
	m_RcSave.SetLabel("S\366a th\253ng b\270o");
	m_BtnList.SetLabel("");	// sprite nut co san chu
	for (i = 0; i < 14; i++)
		m_FunBtn[i].SetLabel(s_sFunBtn[i].szLabel);
	for (i = 0; i < 4; i++)
		m_FunSub[i].SetLabel("");

	Wnd_AddWindow(this);
}

void KUiTongJX2::LoadScheme(const char* pScheme)
{
	if (!ms_pSelf)
		return;
	char Buff[128];
	KIniFile Ini;
	sprintf(Buff, "%s\\%s", pScheme, TONG_JX2_INI);
	if (!Ini.Load(Buff))
		return;

	if (SCREEN_WIDTH == 1024)
		ms_pSelf->Init(&Ini, "Main1024");
	else
		ms_pSelf->Init(&Ini, "Main");

	char szSec[32];
	int i;
	for (i = 0; i < TJX2_UI_TABS; i++)
	{
		sprintf(szSec, "PageBg%d", i);
		ms_pSelf->m_PageBg[i].Init(&Ini, szSec);
	}
	for (i = 0; i < TJX2_UI_TABS; i++)
	{
		sprintf(szSec, "Tab%d", i);
		ms_pSelf->m_BtnTab[i].Init(&Ini, szSec);
	}
	for (i = 0; i < TJX2_UI_ROWS; i++)
	{
		sprintf(szSec, "Row%d", i);
		ms_pSelf->m_Row[i].Init(&Ini, szSec);
	}
	for (i = 0; i < TJX2_UI_ACTS; i++)
	{
		sprintf(szSec, "Act%d", i);
		ms_pSelf->m_BtnAct[i].Init(&Ini, szSec);
	}
	for (i = 0; i < 14; i++)
	{
		sprintf(szSec, "Rt_%s", s_szRtSec[i]);
		ms_pSelf->m_Rt[i].Init(&Ini, szSec);
		int nRid = 0;
		Ini.GetInteger(szSec, "RightID", 0, &nRid);
		ms_pSelf->m_dwRtId[i] = (DWORD)nRid;
	}
	ms_pSelf->m_RtAll.Init(&Ini, "Rt_BtnSelectAll");
	ms_pSelf->m_RtApply.Init(&Ini, "Rt_BtnDistribute");
	ms_pSelf->m_ColHdr[0].Init(&Ini, "Fun_TxtRank");
	ms_pSelf->m_ColHdr[1].Init(&Ini, "Fun_TitleName");
	ms_pSelf->m_ColHdr[2].Init(&Ini, "Fun_TxtTitle");
	ms_pSelf->m_Bot[0].Init(&Ini, "Bot_Join");
	ms_pSelf->m_Bot[1].Init(&Ini, "Bot_Create");
	ms_pSelf->m_Bot[2].Init(&Ini, "Bot_Other");
	ms_pSelf->m_Bot[3].Init(&Ini, "Bot_Close");
	ms_pSelf->m_RecToggle.Init(&Ini, "Rec_ToggleRecruit");
	ms_pSelf->m_RecToggle.SetLabel("\247\343ng/m\353 tuy\323n");
	for (i = 0; i < 32; i++)
	{
		sprintf(szSec, "Info_%s", s_sInfoCtl[i].szSec);
		ms_pSelf->m_Info[i].Init(&Ini, szSec);
		if (s_sInfoCtl[i].szLabel)
			ms_pSelf->m_Info[i].SetText(s_sInfoCtl[i].szLabel);
	}
	for (i = 0; i < TJX2_UI_ROWS; i++)
	{
		sprintf(szSec, "RowSel%d", i);
		ms_pSelf->m_BtnRowSel[i].Init(&Ini, szSec);
	}
	for (i = 0; i < 8; i++)
	{
		sprintf(szSec, "Rec_%s", s_sRecLbl[i].szSec);
		ms_pSelf->m_RecLbl[i].Init(&Ini, szSec);
		char szTx[96];
		szTx[0] = 0;
		Ini.GetString(szSec, "Text", "", szTx, sizeof(szTx));
		if (szTx[0])
			ms_pSelf->m_RecLbl[i].SetText(szTx);
		else
			ms_pSelf->m_RecLbl[i].SetText(s_sRecLbl[i].szLabel);
	}
	ms_pSelf->m_FunBg.Init(&Ini, "Fun_PageBg");
	for (i = 0; i < 15; i++)
	{
		sprintf(szSec, "Fun_%s", s_sFunTxt[i].szSec);
		ms_pSelf->m_FunTxtBg[i].Init(&Ini, szSec);	// khung do / thanh ong cua blueprint
		ms_pSelf->m_FunTxt[i].Init(&Ini, szSec);
		// nhan lay NGUYEN VAN tu ini blueprint (Label= cua khung chu)
		char szLbl[64];
		szLbl[0] = 0;
		Ini.GetString(szSec, "Label", "", szLbl, sizeof(szLbl));
		if (szLbl[0])
			ms_pSelf->m_FunTxt[i].SetText(szLbl);
		else if (s_sFunTxt[i].szLabel)
			ms_pSelf->m_FunTxt[i].SetText(s_sFunTxt[i].szLabel);
	}
	ms_pSelf->m_FunP[0].Init(&Ini, "Fun_TxtPersonalInfo");
	ms_pSelf->m_FunP[0].SetText("Tin t\370c c\270 nh\251n");
	ms_pSelf->m_FunPBg[0].Init(&Ini, "Fun_TitlePersonalOffer");
	ms_pSelf->m_FunP[1].Init(&Ini, "Fun_TitlePersonalOffer");
	ms_pSelf->m_FunP[1].SetText("C\350ng hi\325n c\270 nh\251n");
	ms_pSelf->m_FunPBg[1].Init(&Ini, "Fun_TxtPersonalOffer");	// thanh ong
	ms_pSelf->m_FunP[2].Init(&Ini, "Fun_TxtPersonalOffer");
	for (i = 0; i < 7; i++)
	{
		sprintf(szSec, "MDet%d", i);
		ms_pSelf->m_MDet[i].Init(&Ini, szSec);
	}
	for (i = 0; i < TJX2_UI_ROWS; i++)
	{
		sprintf(szSec, "RowDim%d", i);
		ms_pSelf->m_RowDim[i].Init(&Ini, szSec);
	}
	for (i = 0; i < 14; i++)
	{
		sprintf(szSec, "Fun_%s", s_sFunBtn[i].szSec);
		ms_pSelf->m_FunBtn[i].Init(&Ini, szSec);
	}
	for (i = 0; i < 4; i++)
	{
		sprintf(szSec, "Fun_BtnSubPage%d", i + 1);
		ms_pSelf->m_FunSub[i].Init(&Ini, szSec);
	}
	for (i = 0; i < 6; i++)
	{
		sprintf(szSec, "Ws_%s", s_sWsTxt[i].szSec);
		ms_pSelf->m_WsTxt[i].Init(&Ini, szSec);
		if (s_sWsTxt[i].szLabel)
			ms_pSelf->m_WsTxt[i].SetText(s_sWsTxt[i].szLabel);
	}
	for (i = 0; i < 6; i++)
	{
		sprintf(szSec, "Ws_%s", s_szWsBtnSec[i]);
		ms_pSelf->m_WsBtn[i].Init(&Ini, szSec);
	}
	for (i = 1; i <= 7; i++)
	{
		sprintf(szSec, "Ws_Icon%d", i);
		ms_pSelf->m_WsIcon[i].Init(&Ini, szSec);
	}
	ms_pSelf->m_WsSel.Init(&Ini, "Ws_TitleServiceFee");	// muon font/mau
	ms_pSelf->m_WsSel.SetPosition(46, 226);
	ms_pSelf->m_WsSel.SetSize(300, 16);
	ms_pSelf->m_RcSub[0].Init(&Ini, "Rc_BtnWeekDaily");
	ms_pSelf->m_RcSub[1].Init(&Ini, "Rc_BtnAnnounce");
	ms_pSelf->m_RcSub[2].Init(&Ini, "Rc_BtnTongAffair");
	ms_pSelf->m_RcSub[3].Init(&Ini, "Rc_BtnTongHistory");
	ms_pSelf->m_RcEditor.Init(&Ini, "Rc_AnnounceEditor");
	ms_pSelf->m_RcLeaveWord.Init(&Ini, "Rc_BtnLeaveWord");
	ms_pSelf->m_RcSave.Init(&Ini, "Rc_BtnEditAnnounce");
	ms_pSelf->m_BtnList.Init(&Ini, "BtnTongList");
	ms_pSelf->m_BtnFun.Init(&Ini, "BtnFunUse");
	ms_pSelf->m_RecJiyu.Init(&Ini, "Rec_Jiyu");
	ms_pSelf->m_RecAuto.Init(&Ini, "Rec_AutoAcceptLevel");
	ms_pSelf->m_RecRefuse.Init(&Ini, "Rec_RefuseLevel");
	ms_pSelf->m_RecQX.Init(&Ini, "Rec_QingXiangBtn");
	ms_pSelf->m_RecHD[0].Init(&Ini, "Rec_HuoDongBtn1");
	ms_pSelf->m_RecHD[1].Init(&Ini, "Rec_HuoDongBtn2");
	ms_pSelf->m_RecHD[2].Init(&Ini, "Rec_HuoDongBtn3");
	ms_pSelf->m_RecHD[3].Init(&Ini, "Rec_HuoDongBtn4");
	ms_pSelf->m_RecSave.Init(&Ini, "Rec_Save");
	ms_pSelf->m_RecAccept.Init(&Ini, "Rec_AcceptApply");
	ms_pSelf->m_RecDeny.Init(&Ini, "Rec_RefuseApply");
	ms_pSelf->m_RecPrev.Init(&Ini, "Rec_LastPage");
	ms_pSelf->m_RecNext.Init(&Ini, "Rec_NextPage");
	ms_pSelf->m_BtnPrev.Init(&Ini, "BtnPrev");
	ms_pSelf->m_BtnNext.Init(&Ini, "BtnNext");
	ms_pSelf->m_BtnClose.Init(&Ini, "BtnClose");
}

//////////////////////////////////////////////////////////////////////

void KUiTongJX2::RequestPage(int nPage, int nStart)
{
	sTJX2Log("[REQ] trangUI=%d start=%d", nPage, nStart);
	if (nPage == TJX2_UI_PAGE_RECRUIT)
		nPage = defTONG_JX2_PAGE_RECRUIT;
	else if (nPage == 4)
	{
		nPage = defTONG_JX2_PAGE_RECORD;
		nStart = (m_nRcSub == 3) ? 1 : 0;	// lich su / su kien
	}
	else if (nPage == TJX2_UI_PAGE_FUNUSE)
	{
		// trang chuc nang can CA thong tin bang CA danh sach thanh vien
		if (g_pCoreShell)
			g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW, defTONG_JX2_PAGE_INFO, 0);
		nPage = defTONG_JX2_PAGE_MEMBER;
	}
	else if (nPage == TJX2_UI_PAGE_TONGLIST)
		nPage = defTONG_JX2_PAGE_TONGLIST;
	else if (nPage == defTONG_JX2_PAGE_WS)
	{
		// phuong tho can them danh sach thanh vien cho panel phai
		if (g_pCoreShell)
			g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW, defTONG_JX2_PAGE_MEMBER, 0);
	}
	if (g_pCoreShell)
		g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW, (unsigned int)nPage, nStart);
}

void KUiTongJX2::SendOp(int nOp, unsigned long dwTarget, int nP1, int nP2, const char* pszText)
{
	if (!g_pCoreShell)
		return;
	KUiTongJX2Op sOp;
	memset(&sOp, 0, sizeof(sOp));
	sOp.nOp = nOp;
	sOp.dwTarget = dwTarget;
	sOp.nParam1 = nP1;
	sOp.nParam2 = nP2;
	if (pszText)
		strncpy(sOp.szText, pszText, sizeof(sOp.szText) - 1);
	g_pCoreShell->TongOperation(GTOI_TONG_JX2_OP, (unsigned int)&sOp, 0);
	sTJX2Log("[OP ] op=%d target=%u p1=%d p2=%d", nOp, (unsigned)dwTarget, nP1, nP2);
	// xin lai trang sau khi thao tac (relay echo ve GS truoc khi minh hoi lai)
	RequestPage(m_nPage, m_nStart);
}

void KUiTongJX2::DataArrive(unsigned char* pData, int nLen)
{
	if (nLen < 0)
	{
		// lenh mo cua so tu server (OpenTongJX2)
		OpenWindow();
		return;
	}
	if (!ms_pSelf || !pData || nLen < 5)
		return;

	int nPage = pData[4];	// {BYTE ProtocolType; WORD wLength; BYTE btMsgId;} + btPage
	sTJX2Log("[SYN] trangSV=%d len=%d trangUI=%d", nPage, nLen, ms_pSelf->m_nPage);
	switch (nPage)
	{
	case defTONG_JX2_PAGE_INFO:
		if (nLen <= (int)sizeof(ms_pSelf->m_byInfo))
		{
			memcpy(ms_pSelf->m_byInfo, pData, nLen);
			ms_pSelf->m_bHasInfo = 1;
			if (ms_pSelf->m_nPage == defTONG_JX2_PAGE_INFO)
				ms_pSelf->RenderInfo();
			else if (ms_pSelf->m_nPage == 4)
				ms_pSelf->RenderAnnounce();
			else if (ms_pSelf->m_nPage == TJX2_UI_PAGE_FUNUSE)
				ms_pSelf->RenderFunUse();
		}
		break;
	case defTONG_JX2_PAGE_MEMBER:
	case defTONG_JX2_PAGE_RIGHT:
		if (nLen <= (int)sizeof(ms_pSelf->m_byMember))
		{
			memcpy(ms_pSelf->m_byMember, pData, nLen);
			ms_pSelf->m_bHasMember = 1;
			if (ms_pSelf->m_nPage == defTONG_JX2_PAGE_MEMBER ||
				ms_pSelf->m_nPage == defTONG_JX2_PAGE_RIGHT ||
				ms_pSelf->m_nPage == TJX2_UI_PAGE_FUNUSE ||
				ms_pSelf->m_nPage == defTONG_JX2_PAGE_WS)
				ms_pSelf->RenderMembers();
		}
		break;
	case defTONG_JX2_PAGE_WS:
		if (nLen <= (int)sizeof(ms_pSelf->m_byWs))
		{
			memcpy(ms_pSelf->m_byWs, pData, nLen);
			ms_pSelf->m_bHasWs = 1;
			if (ms_pSelf->m_nPage == defTONG_JX2_PAGE_WS)
				ms_pSelf->RenderWorkshop();
		}
		break;
	case defTONG_JX2_PAGE_TONGLIST:
		if (nLen <= (int)sizeof(ms_pSelf->m_byList))
		{
			memcpy(ms_pSelf->m_byList, pData, nLen);
			ms_pSelf->m_bHasList = 1;
			if (ms_pSelf->m_nPage == TJX2_UI_PAGE_TONGLIST)
				ms_pSelf->RenderTongList();
		}
		break;
	case defTONG_JX2_PAGE_RECORD:
		if (nLen <= (int)sizeof(ms_pSelf->m_byRecord))
		{
			memcpy(ms_pSelf->m_byRecord, pData, nLen);
			ms_pSelf->m_bHasRecord = 1;
			if (ms_pSelf->m_nPage == 4)
				ms_pSelf->RenderRecord();
		}
		break;
	case defTONG_JX2_PAGE_RECRUIT:
		if (nLen <= (int)sizeof(ms_pSelf->m_byRecruit))
		{
			memcpy(ms_pSelf->m_byRecruit, pData, nLen);
			ms_pSelf->m_bHasRecruit = 1;
			if (ms_pSelf->m_nPage == TJX2_UI_PAGE_RECRUIT)
				ms_pSelf->RenderRecruit();
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////

void KUiTongJX2::ClearRows()
{
	for (int i = 0; i < TJX2_UI_ROWS; i++)
	{
		m_Row[i].SetText("");
		m_RowDim[i].SetText("");
	}
	for (int d = 0; d < 7; d++)
		m_MDet[d].SetText("");
}

// bo cuc dong: INFO/WS/ANN o vung noi dung trai; MEMBER/RIGHT nam panel danh sach phai
void KUiTongJX2::RepositionRows()
{
	int i;
	BOOL bList = (m_nPage == defTONG_JX2_PAGE_MEMBER || m_nPage == defTONG_JX2_PAGE_RIGHT ||
		m_nPage == TJX2_UI_PAGE_FUNUSE);
	for (i = 0; i < TJX2_UI_ROWS; i++)
	{
		if (bList)
		{
			m_Row[i].SetPosition(341, 68 + i * 24);
			m_RowDim[i].SetPosition(341, 68 + i * 24);
			m_BtnRowSel[i].SetPosition(341, 68 + i * 24);
			m_BtnRowSel[i].Enable(true);
		}
		else if (m_nPage == 4)
		{
			// vung RecordList cua blueprint (5,25 doi +18/+52), 14 dong buoc 22
			m_Row[i].SetPosition(25, 79 + i * 22);
			m_BtnRowSel[i].SetPosition(25, 79 + i * 22);
			m_BtnRowSel[i].Enable(false);
		}
		else if (m_nPage == TJX2_UI_PAGE_TONGLIST)
		{
			m_Row[i].SetPosition(40, 70 + i * 24);
			m_BtnRowSel[i].SetPosition(40, 70 + i * 24);
			m_BtnRowSel[i].Enable(i >= 1 && i <= 10);
		}
		else if (m_nPage == TJX2_UI_PAGE_RECRUIT)
		{
			// vung danh sach don xin (blueprint ApplyerList 290,40 doi +18/+52)
			m_Row[i].SetPosition(310, 96 + i * 24);
			m_BtnRowSel[i].SetPosition(310, 96 + i * 24);
			m_BtnRowSel[i].Enable(i < 8);
		}
		else if (m_nPage == defTONG_JX2_PAGE_WS)
		{
			// phuong tho: panel thanh vien ben phai CHI HIEN THI (chon khu bang icon)
			m_Row[i].SetPosition(341, 68 + i * 24);
			m_RowDim[i].SetPosition(341, 68 + i * 24);
			m_BtnRowSel[i].SetPosition(341, 68 + i * 24);
			m_BtnRowSel[i].Enable(false);
		}
		else
		{
			m_Row[i].SetPosition(30, 64 + i * 23);
			m_BtnRowSel[i].SetPosition(30, 64 + i * 23);
			m_BtnRowSel[i].Enable(false);
		}
	}
}

void KUiTongJX2::SwitchPage(int nPage)
{
	m_nPage = nPage;
	m_nStart = 0;
	m_nSel = (nPage == defTONG_JX2_PAGE_WS) ? 1 : 0;
	// hien dung nen phan trang cua TAB (trang UI 5 = tab 1 chieu mo;
	// trang thanh vien/quyen hop nhat = tab 2)
	{
		int nBg = 0;
		if (nPage == TJX2_UI_PAGE_FUNUSE)
			nBg = -1;	// trang chuc nang dung nen rieng m_FunBg
		else if (nPage == TJX2_UI_PAGE_RECRUIT)
			nBg = 1;
		else if (nPage == defTONG_JX2_PAGE_MEMBER || nPage == defTONG_JX2_PAGE_RIGHT)
			nBg = 2;
		else if (nPage == defTONG_JX2_PAGE_WS)
			nBg = 3;
		else if (nPage == 4)
			nBg = 4;
		for (int i = 0; i < TJX2_UI_TABS; i++)
		{
			if (i == nBg)
				m_PageBg[i].Show();
			else
				m_PageBg[i].Hide();
		}
	}
	// nut quyen blueprint chi hien o trang Phan phoi
	{
		BOOL bRt = (nPage == defTONG_JX2_PAGE_MEMBER || nPage == defTONG_JX2_PAGE_RIGHT);
		for (int i = 0; i < 14; i++)
		{
			if (bRt) { m_Rt[i].Show(); m_Rt[i].Enable(true); }
			else { m_Rt[i].Hide(); m_Rt[i].Enable(false); }
		}
		if (bRt) { m_RtAll.Show(); m_RtAll.Enable(true); m_RtApply.Show(); m_RtApply.Enable(true); }
		else { m_RtAll.Hide(); m_RtAll.Enable(false); m_RtApply.Hide(); m_RtApply.Enable(false); }
	}
	// bo control trang chuc nang: mode 0 = Tin tuc (chi thong tin),
	// mode 1 = Su dung chuc nang (them cac nut hanh dong)
	{
		int i;
		BOOL bFun = (nPage == TJX2_UI_PAGE_FUNUSE);
		BOOL bBtn = (bFun && m_nFunMode == 1);
		if (bFun) m_FunBg.Show(); else m_FunBg.Hide();
		for (i = 0; i < 15; i++)
		{
			if (bFun) { m_FunTxt[i].Show(); m_FunTxtBg[i].Show(); }
			else { m_FunTxt[i].Hide(); m_FunTxtBg[i].Hide(); }
		}
		for (i = 0; i < 3; i++)
			if (bFun) m_FunP[i].Show(); else m_FunP[i].Hide();
		for (i = 0; i < 2; i++)
			if (bFun) m_FunPBg[i].Show(); else m_FunPBg[i].Hide();
		// nhom nut giua theo sub-page (cac nut hanh dong deu thuoc sub 1 tru map)
		static const int s_nFunBtnSub[14] =
			{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 0, 0 };
			// 0 = luon hien khi mode nut (khoi tien/ca nhan/roi bang)
			// 1 = sub nhan su (Recruit/KickOut/Depose/DispenseOffer)
			// 2 = sub lanh dia (CreateTongMap/ConfigureTongMap/TongStunt)
		for (i = 0; i < 14; i++)
		{
			BOOL bShow = bBtn && s_sFunBtn[i].nAct >= 0 &&
				(s_nFunBtnSub[i] == 0 || s_nFunBtnSub[i] == m_nFunSub);
			if (bShow)
			{
				m_FunBtn[i].Show();
				m_FunBtn[i].Enable(true);
			}
			else
			{
				m_FunBtn[i].Hide();
				m_FunBtn[i].Enable(false);
			}
		}
		for (i = 0; i < 4; i++)
		{
			if (bBtn) { m_FunSub[i].Show(); m_FunSub[i].Enable(true); }
			else { m_FunSub[i].Hide(); m_FunSub[i].Enable(false); }
		}
		m_bMDet = 0;
	}
	// lop chu xam (offline) + panel xanh chi tiet + dong chi tiet khu
	{
		int i;
		BOOL bL = (nPage == defTONG_JX2_PAGE_MEMBER || nPage == defTONG_JX2_PAGE_RIGHT ||
			nPage == TJX2_UI_PAGE_FUNUSE || nPage == defTONG_JX2_PAGE_WS);
		for (i = 0; i < TJX2_UI_ROWS; i++)
			if (bL) m_RowDim[i].Show(); else m_RowDim[i].Hide();
		for (i = 0; i < 7; i++)
			if (bL) m_MDet[i].Show(); else m_MDet[i].Hide();
		for (i = 0; i < 3; i++)
			if (bL) m_ColHdr[i].Show(); else m_ColHdr[i].Hide();
		if (nPage == TJX2_UI_PAGE_RECRUIT) { m_RecToggle.Show(); m_RecToggle.Enable(true); }
		else { m_RecToggle.Hide(); m_RecToggle.Enable(false); }
		if (nPage == defTONG_JX2_PAGE_WS) m_WsSel.Show(); else m_WsSel.Hide();
	}
	// bo control trang Phuong tho
	{
		int i;
		BOOL bWs = (nPage == defTONG_JX2_PAGE_WS);
		for (i = 0; i < 6; i++)
			if (bWs) m_WsTxt[i].Show(); else m_WsTxt[i].Hide();
		for (i = 0; i < 6; i++)
		{
			if (bWs) { m_WsBtn[i].Show(); m_WsBtn[i].Enable(true); }
			else { m_WsBtn[i].Hide(); m_WsBtn[i].Enable(false); }
		}
		for (i = 1; i <= 7; i++)
		{
			if (bWs) { m_WsIcon[i].Show(); m_WsIcon[i].Enable(true); }
			else { m_WsIcon[i].Hide(); m_WsIcon[i].Enable(false); }
		}
	}
	// bo control trang Nhat ky
	{
		int i;
		BOOL bRc = (nPage == 4);
		for (i = 0; i < 4; i++)
		{
			if (bRc) { m_RcSub[i].Show(); m_RcSub[i].Enable(true); }
			else { m_RcSub[i].Hide(); m_RcSub[i].Enable(false); }
		}
		BOOL bEd = (bRc && m_nRcSub == 1);
		if (bEd) m_RcEditor.Show(); else m_RcEditor.Hide();
		if (bEd) { m_RcSave.Show(); m_RcSave.Enable(true); }
		else { m_RcSave.Hide(); m_RcSave.Enable(false); }
		if (bRc) { m_RcLeaveWord.Show(); m_RcLeaveWord.Enable(true); }
		else { m_RcLeaveWord.Hide(); m_RcLeaveWord.Enable(false); }
	}
	// bo control trang chieu mo
	{
		int i;
		BOOL bRec = (nPage == TJX2_UI_PAGE_RECRUIT);
		for (i = 0; i < 8; i++)
			if (bRec) m_RecLbl[i].Show(); else m_RecLbl[i].Hide();
		if (bRec) m_RecJiyu.Show(); else m_RecJiyu.Hide();
		if (bRec) m_RecAuto.Show(); else m_RecAuto.Hide();
		if (bRec) m_RecRefuse.Show(); else m_RecRefuse.Hide();
		if (bRec) m_RecQX.Show(); else m_RecQX.Hide();
		for (i = 0; i < 4; i++)
			if (bRec) m_RecHD[i].Show(); else m_RecHD[i].Hide();
		if (bRec) m_RecSave.Show(); else m_RecSave.Hide();
		if (bRec) m_RecAccept.Show(); else m_RecAccept.Hide();
		if (bRec) m_RecDeny.Show(); else m_RecDeny.Hide();
		if (bRec) m_RecPrev.Show(); else m_RecPrev.Hide();
		if (bRec) m_RecNext.Show(); else m_RecNext.Hide();
	}
	// field Tin tuc chi hien o trang Tin tuc (trang nay khong dung Row)
	{
		for (int i = 0; i < 32; i++)
		{
			if (nPage == defTONG_JX2_PAGE_INFO)
				m_Info[i].Show();
			else
				m_Info[i].Hide();
		}
	}
	// tab SANG dung trang dang mo (nut tab la checkbox sprite)
	{
		int nTabOn = -1;
		if (nPage == TJX2_UI_PAGE_FUNUSE && m_nFunMode == 0) nTabOn = 0;
		else if (nPage == TJX2_UI_PAGE_RECRUIT) nTabOn = 1;
		else if (nPage == defTONG_JX2_PAGE_MEMBER || nPage == defTONG_JX2_PAGE_RIGHT) nTabOn = 2;
		else if (nPage == defTONG_JX2_PAGE_WS) nTabOn = 3;
		else if (nPage == 4) nTabOn = 4;
		for (int t = 0; t < TJX2_UI_TABS; t++)
			m_BtnTab[t].CheckButton(t == nTabOn ? 1 : 0);
		m_BtnFun.CheckButton(nPage == TJX2_UI_PAGE_FUNUSE && m_nFunMode == 1 ? 1 : 0);
	}
	// nut phan trang danh sach chi hien o trang co danh sach thanh vien
	{
		BOOL bPg = (nPage == defTONG_JX2_PAGE_MEMBER || nPage == defTONG_JX2_PAGE_RIGHT ||
			nPage == TJX2_UI_PAGE_FUNUSE || nPage == defTONG_JX2_PAGE_WS ||
			nPage == TJX2_UI_PAGE_TONGLIST);
		if (bPg) { m_BtnPrev.Show(); m_BtnPrev.Enable(true); m_BtnNext.Show(); m_BtnNext.Enable(true); }
		else { m_BtnPrev.Hide(); m_BtnPrev.Enable(false); m_BtnNext.Hide(); m_BtnNext.Enable(false); }
		m_BtnList.Hide();	// trung chuc nang voi nut day Xem tin Bang khac
		m_BtnList.Enable(false);
	}
	RepositionRows();
	ClearRows();
	SetupActions();
	if (nPage == 4)
		RenderRecord();
	else if (nPage == TJX2_UI_PAGE_RECRUIT)
		RenderRecruit();
	else if (nPage == TJX2_UI_PAGE_FUNUSE)
		RenderFunUse();
	else if (nPage == TJX2_UI_PAGE_TONGLIST)
		RenderTongList();
	RequestPage(nPage, 0);
}

void KUiTongJX2::SetupActions()
{
	int i;
	for (i = 0; i < TJX2_UI_ACTS; i++)
	{
		m_BtnAct[i].SetLabel("");
		m_BtnAct[i].Enable(false);
	}
	switch (m_nPage)
	{
	case defTONG_JX2_PAGE_INFO:
		break;	// hang Act da BO toan bo (ban Linux khong co)
	case defTONG_JX2_PAGE_MEMBER:
	case defTONG_JX2_PAGE_RIGHT:
		break;	// dung 14 nut quyen blueprint + nut Phan quyen rieng
	case TJX2_UI_PAGE_RECRUIT:
	case TJX2_UI_PAGE_FUNUSE:
		break;	// 2 trang nay dung bo nut rieng cua blueprint
	case TJX2_UI_PAGE_TONGLIST:
		break;	// dung nut day Bot_Join / Bot_Create
	case defTONG_JX2_PAGE_WS:
		break;	// bo nut blueprint m_WsBtn (tuyet ky dat qua NPC do dang)
	case 4:	// nhat ky - dung bo nut rieng cua blueprint
		break;
	}
	for (i = 0; i < TJX2_UI_ACTS; i++)
	{
		char szLabel[8];
		if (m_BtnAct[i].GetLabel(szLabel, sizeof(szLabel)) > 0)
			m_BtnAct[i].Enable(true);
	}
}

void KUiTongJX2::RenderInfo()
{
	if (!m_bHasInfo)
		return;
	TONG_JX2_INFO_SYNC* p = (TONG_JX2_INFO_SYNC*)m_byInfo;
	char sz[120];
	ClearRows();
	if (m_nPage == defTONG_JX2_PAGE_INFO)
	{
		// do du lieu vao dung cac o Txt* cua blueprint (chi so theo s_sInfoCtl)
		static const char* szCamp[3] = {"Ch\335nh ph\270i", "T\265 ph\270i", "Trung l\313p"};
		m_Info[2].SetText(p->m_szTongName);
		m_Info[4].SetText(p->m_szMaster);
		sprintf(sz, "%u", (unsigned)0);
		m_Info[6].SetText("-");
		m_Info[8].SetText(p->m_btCamp < 3 ? szCamp[p->m_btCamp] : "-");
		sprintf(sz, "%d", p->m_nLevel);
		m_Info[10].SetText(sz);
		sprintf(sz, "%d", (int)p->m_wMemberTotal);
		m_Info[12].SetText(sz);
		sprintf(sz, "%.0f", (double)p->m_nMoney);
		m_Info[14].SetText(sz);
		sprintf(sz, "%u  (tu\307n %u/%u)", p->m_dwBuildFund, p->m_dwWeekBuild, p->m_dwWeekUpper);
		m_Info[16].SetText(sz);
		sprintf(sz, "%u", p->m_dwWarFund);
		m_Info[18].SetText(sz);
		sprintf(sz, "%u", p->m_dwMaintain);
		m_Info[20].SetText(sz);
		sprintf(sz, "%u", p->m_dwPerStand);
		m_Info[22].SetText(sz);
		sprintf(sz, "%u", p->m_dwStoredOffer);
		m_Info[24].SetText(sz);
		sprintf(sz, "%u", p->m_dwStoredBuild);
		m_Info[26].SetText(sz);
		sprintf(sz, "%u", p->m_dwMyOffer);
		m_Info[29].SetText(sz);
		sprintf(sz, "%d", p->m_nExp);
		m_Info[31].SetText(sz);
		return;
	}
	sprintf(sz, "Bang: %s   Bang chu: %s", p->m_szTongName, p->m_szMaster);
	m_Row[0].SetText(sz);
	sprintf(sz, "Cap bang: %d   Kinh nghiem: %d   Thanh vien: %d", p->m_nLevel, p->m_nExp, (int)p->m_wMemberTotal);
	m_Row[1].SetText(sz);
	sprintf(sz, "Ngan quy: %.0f luong", (double)p->m_nMoney);
	m_Row[2].SetText(sz);
	sprintf(sz, "Quy kien thiet: %u   (tuan nay %u / tran %u)", p->m_dwBuildFund, p->m_dwWeekBuild, p->m_dwWeekUpper);
	m_Row[3].SetText(sz);
	sprintf(sz, "Quy chien bi: %u   Duy tri/ngay: %u", p->m_dwWarFund, p->m_dwMaintain);
	m_Row[4].SetText(sz);
	sprintf(sz, "Quy du tru: %u   Kien thiet du tru: %u", p->m_dwStoredOffer, p->m_dwStoredBuild);
	m_Row[5].SetText(sz);
	sprintf(sz, "Tro cap moi nguoi: %u", p->m_dwPerStand);
	m_Row[6].SetText(sz);
	sprintf(sz, "Ngay hoat dong: %d   Tuan: %d", p->m_nDay, p->m_nWeek);
	m_Row[7].SetText(sz);
	if (p->m_dwStuntID && p->m_dwStuntID < 8)
		sprintf(sz, "Tuyet ky: %s (%s)", s_szWsName[0] == NULL ? "" : "", "");
	if (p->m_dwStuntID)
		sprintf(sz, "Tuyet ky ID %u (%s)", p->m_dwStuntID, p->m_dwStuntOn ? "hieu luc" : "thieu chien bi");
	else
		sprintf(sz, "Tuyet ky: chua dat");
	m_Row[8].SetText(sz);
	sprintf(sz, "Chuc vu cua ban: %s   Cong hien: %u",
		p->m_btMyFigure < 5 ? s_szFigure[p->m_btMyFigure] : "?", p->m_dwMyOffer);
	m_Row[9].SetText(sz);
	if (p->m_szAnnounce[0])
	{
		sprintf(sz, "Thong bao: %.90s", p->m_szAnnounce);
		m_Row[10].SetText(sz);
	}
}

void KUiTongJX2::RenderMembers()
{
	if (!m_bHasMember)
		return;
	TONG_JX2_MEMBER_SYNC* p = (TONG_JX2_MEMBER_SYNC*)m_byMember;
	char sz[120];
	ClearRows();
	// tieu de 3 cot nhu ban Linux: Hang / Thanh vien / Chuc vu
	sprintf(sz, "H\271ng  Th\265nh vi\252n        Ch\370c v\364      (%d-%d/%d)",
		(int)p->m_wStart + 1, (int)p->m_wStart + p->m_btCount, (int)p->m_wTotal);
	m_Row[0].SetText(sz);
	int i;
	for (i = 0; i < (int)p->m_btCount && i + 1 < TJX2_UI_ROWS; i++)
	{
		TONG_JX2_ONE_MEMBER* pM = &p->m_sMember[i];
		if (m_nPage == defTONG_JX2_PAGE_RIGHT)
		{
			// liet ke quyen dang co theo mat na
			char szR[64];
			szR[0] = 0;
			int nR = 0;
			for (int b = 0; b < 12 && nR < 4; b++)
			{
				if (pM->m_wRights & (1 << b))
				{
					char szOne[12];
					sprintf(szOne, "%u ", s_dwRightId[b]);
					strcat(szR, szOne);
					nR++;
				}
			}
			sprintf(sz, "%s%-16s %-10s quyen: %s", (i == m_nSel) ? "> " : "  ",
				pM->m_szName, pM->m_btFigure < 5 ? s_szFigure[pM->m_btFigure] : "?",
				szR[0] ? szR : "(khong)");
		}
		else
		{
			sprintf(sz, "%s%2d  %-16s %-12s", (i == m_nSel) ? ">" : " ",
				(int)p->m_wStart + i + 1, pM->m_szName,
				pM->m_btFigure < 5 ? s_szFigure[pM->m_btFigure] : "?");
		}
		// online: chu sang (m_Row); offline: chu xam (m_RowDim)
		if (pM->m_btOnline)
		{
			m_Row[i + 1].SetText(sz);
			m_RowDim[i + 1].SetText("");
		}
		else
		{
			m_Row[i + 1].SetText("");
			m_RowDim[i + 1].SetText(sz);
		}
	}
	// panel XANH chi tiet nguoi dang chon (nhu ban Linux, hien o moi trang co danh sach)
	if (m_bMDet && m_nSel < (int)p->m_btCount)
	{
		// vung panel (dong 4..11) de trong cho chu xanh khoi chong len danh sach
		for (int nCl = 4; nCl <= 11 && nCl < TJX2_UI_ROWS; nCl++)
		{
			m_Row[nCl].SetText("");
			m_RowDim[nCl].SetText("");
		}
		TONG_JX2_ONE_MEMBER* pSel = &p->m_sMember[m_nSel];
		char szT[64];
		m_MDet[0].SetText(pSel->m_szName);
		sprintf(szT, "Danh hi\326u: %s",
			pSel->m_btFigure < 5 ? s_szFigure[pSel->m_btFigure] : "?");
		m_MDet[1].SetText(szT);
		struct tm* pTm;
		time_t nT = (time_t)pSel->m_dwJoinTime;
		if (nT)
		{
			pTm = localtime(&nT);
			sprintf(szT, "Ng\265y gia nh\313p  %02d-%02d-%04d %02d:%02d",
				pTm->tm_mday, pTm->tm_mon + 1, pTm->tm_year + 1900,
				pTm->tm_hour, pTm->tm_min);
		}
		else
			strcpy(szT, "Ng\265y gia nh\313p  -");
		m_MDet[2].SetText(szT);
		nT = (time_t)pSel->m_dwLastActive;
		if (nT)
		{
			pTm = localtime(&nT);
			sprintf(szT, "Ho\271t \256\351ng g\307n \256\251y  %02d-%02d-%04d %02d:%02d",
				pTm->tm_mday, pTm->tm_mon + 1, pTm->tm_year + 1900,
				pTm->tm_hour, pTm->tm_min);
		}
		else
			strcpy(szT, "Ho\271t \256\351ng g\307n \256\251y  -");
		m_MDet[3].SetText(szT);
		sprintf(szT, "C\350ng hi\325n tu\307n  %u", pSel->m_dwWeekOffer);
		m_MDet[4].SetText(szT);
		{
			long nDays = 1;
			if (pSel->m_dwJoinTime)
				nDays = (long)((time(NULL) - (time_t)pSel->m_dwJoinTime) / 86400) + 1;
			if (nDays < 1)
				nDays = 1;
			sprintf(szT, "\247i\323m c\350ng hi\325n trung b\327nh h\265ng ng\265y  %.2f",
				(double)pSel->m_dwOffer / (double)nDays);
			m_MDet[5].SetText(szT);
		}
		sprintf(szT, "Ho\265n th\265nh m\364c ti\252u tu\307n  %u", pSel->m_dwWeekGoal);
		m_MDet[6].SetText(szT);
	}
	LoadChecksFromSel();
}

void KUiTongJX2::RenderWorkshop()
{
	if (m_bHasWs)
	{
		TONG_JX2_WS_SYNC* pWs = (TONG_JX2_WS_SYNC*)m_byWs;
		char szV[32];
		int nNum = 0, nMax = 0, t;
		for (t = 1; t <= 7; t++)
		{
			if (!pWs->m_sWs[t].btExist)
				continue;
			nNum++;
			if ((int)pWs->m_sWs[t].wLevel > nMax)
				nMax = pWs->m_sWs[t].wLevel;
		}
		sprintf(szV, "%d/7", nNum);
		m_WsTxt[1].SetText(szV);
		sprintf(szV, "%d", nMax);
		m_WsTxt[3].SetText(szV);
		if (m_bHasInfo)
		{
			sprintf(szV, "%u", ((TONG_JX2_INFO_SYNC*)m_byInfo)->m_dwMaintain);
			m_WsTxt[5].SetText(szV);
		}
		// icon khu: khu dang chon danh dau *, khu chua lap danh dau so mo
		for (t = 1; t <= 7; t++)
		{
			char szI[8];
			if (t == m_nSel)
				sprintf(szI, "*%d", t);
			else
				sprintf(szI, "%d%s", t, pWs->m_sWs[t].btExist ? "" : "-");
			m_WsIcon[t].SetLabel(szI);
		}
	}
	ClearRows();	// trang phuong tho khong dung danh sach chu (giong ban Linux)
	if (m_bHasWs && m_nSel >= 1 && m_nSel <= 7)
	{
		TONG_JX2_WS_SYNC* pW2 = (TONG_JX2_WS_SYNC*)m_byWs;
		char szD[120];
		if (pW2->m_sWs[m_nSel].btExist)
			sprintf(szD, "Khu %d %s: c\312p %d [%s]  s\266n l\255\356ng %u  c\312p d\357ng %u",
				m_nSel, s_szWsName[m_nSel], (int)pW2->m_sWs[m_nSel].wLevel,
				pW2->m_sWs[m_nSel].btOpen ? "MO" : "DONG",
				pW2->m_sWs[m_nSel].dwOutput, pW2->m_sWs[m_nSel].dwUseLevel);
		else
			sprintf(szD, "Khu %d %s: (ch\255a l\313p)", m_nSel, s_szWsName[m_nSel]);
		m_WsSel.SetText(szD);
	}
}

// nap trang thai 12 o kiem tu mat na quyen cua nguoi dang chon (trang Phan phoi)
void KUiTongJX2::LoadChecksFromSel()
{
	if (m_nPage != defTONG_JX2_PAGE_RIGHT || !m_bHasMember)
		return;
	TONG_JX2_MEMBER_SYNC* pM = (TONG_JX2_MEMBER_SYNC*)m_byMember;
	WORD wMask = 0;
	if (m_nSel < (int)pM->m_btCount)
		wMask = pM->m_sMember[m_nSel].m_wRights;
	for (int b = 0; b < 14; b++)
	{
		int nBit = -1;
		for (int r = 0; r < 12; r++)
			if (s_dwRightId[r] == m_dwRtId[b])
				nBit = r;
		m_Rt[b].CheckButton((nBit >= 0 && (wMask & (1 << nBit))) ? 1 : 0);
	}
}

// PHAN QUYEN: so o kiem voi quyen hien co cua nguoi chon -> gui them / thu tung quyen.
// Dich phai la TRUONG LAO (luat JX2 4.4) - server kiem lai lan nua.
void KUiTongJX2::ApplyRights()
{
	if (!m_bHasMember)
		return;
	TONG_JX2_MEMBER_SYNC* pM = (TONG_JX2_MEMBER_SYNC*)m_byMember;
	if (m_nSel >= (int)pM->m_btCount)
		return;
	TONG_JX2_ONE_MEMBER* pOne = &pM->m_sMember[m_nSel];
	if (pOne->m_btFigure != 1)
	{
		// chi truong lao nhan duoc quyen (bang chu mac dinh toan quyen)
		return;
	}
	for (int b = 0; b < 14; b++)
	{
		int nBit = -1;
		for (int r = 0; r < 12; r++)
			if (s_dwRightId[r] == m_dwRtId[b])
				nBit = r;
		if (nBit < 0)
			continue;	// quyen ngoai mat na 12 bit (1002/1004/1903/2003) - chua ho tro
		int nWant = m_Rt[b].IsButtonChecked() ? 1 : 0;
		int nHave = (pOne->m_wRights & (1 << nBit)) ? 1 : 0;
		if (nWant == nHave)
			continue;
		SendOp(nWant ? defTONG_JX2_COP_ADDRIGHT : defTONG_JX2_COP_DELRIGHT,
			pOne->m_dwNameID, (int)s_dwRightId[nBit], 0, NULL);
	}
}

void KUiTongJX2::RenderAnnounce()
{
	ClearRows();
	m_Row[0].SetText("Thong bao bang:");
	if (m_bHasInfo)
	{
		TONG_JX2_INFO_SYNC* p = (TONG_JX2_INFO_SYNC*)m_byInfo;
		char sz[120];
		if (p->m_szAnnounce[0])
		{
			sprintf(sz, "%.100s", p->m_szAnnounce);
			m_Row[2].SetText(sz);
		}
		else
			m_Row[2].SetText("(chua co thong bao)");
	}
	m_Row[5].SetText("Chi bang chu duoc dat / xoa thong bao.");
}

// Trang chieu mo: do du lieu RECRUIT_SYNC vao khung blueprint
void KUiTongJX2::RenderRecruit()
{
	int i;
	char sz[120];
	ClearRows();
	if (!m_bHasRecruit)
		return;
	if (g_pCoreShell)
	{
		int nOpen = g_pCoreShell->TongOperation(GTOI_TONG_GET_RECRUIT, 0, 0);
		m_RecToggle.SetLabel(nOpen ?
			"\247ang m\353 tuy\323n - b\312m \256\323 \256\343ng" :
			"\247ang \256\343ng tuy\323n - b\312m \256\323 m\353");
	}
	TONG_JX2_RECRUIT_SYNC* p = (TONG_JX2_RECRUIT_SYNC*)m_byRecruit;
	m_RecJiyu.SetText(p->m_szRecruit);
	sprintf(sz, "%d", (int)p->m_btAutoLv);
	m_RecAuto.SetText(sz);
	sprintf(sz, "%d", (int)p->m_btRefuseLv);
	m_RecRefuse.SetText(sz);
	m_nRecQX = p->m_btTendency % 10;
	sprintf(sz, "%d", m_nRecQX);
	m_RecQX.SetLabel(sz);
	for (i = 0; i < 4; i++)
	{
		m_nRecHD[i] = p->m_btAct[i] % 10;
		sprintf(sz, "%d", m_nRecHD[i]);
		m_RecHD[i].SetLabel(sz);
	}
	if (p->m_btApplyCount == 0)
		m_Row[0].SetText("(ch\255a c\343 \256\254n xin v\265o bang)");
	for (i = 0; i < (int)p->m_btApplyCount && i < defTONG_JX2_APPLY_MAX; i++)
	{
		sprintf(sz, "%s%-16s  c\312p %d", (i == m_nSel) ? "> " : "  ",
			p->m_sApply[i].m_szName, (int)p->m_sApply[i].m_wLevel);
		m_Row[i].SetText(sz);
	}
}

// Trang Su dung chuc nang: do so lieu INFO vao cac o Txt cua blueprint
void KUiTongJX2::RenderFunUse()
{
	if (!m_bHasInfo)
		return;
	TONG_JX2_INFO_SYNC* p = (TONG_JX2_INFO_SYNC*)m_byInfo;
	char sz[120];
	m_FunTxt[2].SetText(p->m_szTongName);
	m_FunTxt[4].SetText(p->m_szMaster);
	sprintf(sz, "%d    Nh\251n s\350 %d", p->m_nLevel, (int)p->m_wMemberTotal);
	m_FunTxt[6].SetText(sz);
	sprintf(sz, "%u", p->m_dwStoredOffer);
	m_FunTxt[8].SetText(sz);
	sprintf(sz, "%.0f", (double)p->m_nMoney);
	m_FunTxt[10].SetText(sz);
	sprintf(sz, "%u (tu\307n %u/%u)", p->m_dwBuildFund, p->m_dwWeekBuild, p->m_dwWeekUpper);
	m_FunTxt[12].SetText(sz);
	sprintf(sz, "%u", p->m_dwWarFund);
	m_FunTxt[14].SetText(sz);
	sprintf(sz, "%u", p->m_dwMyOffer);
	m_FunP[2].SetText(sz);
	if (m_bHasMember)
		RenderMembers();
}

// Trang Nhat ky: 4 muc con - thong bao co khung sua; bang vu/lich su doc ring
void KUiTongJX2::RenderRecord()
{
	int i;
	ClearRows();
	if (m_nRcSub == 1)
	{
		// muc Thong bao: khung sua + noi dung hien tai
		if (m_bHasRecord)
			m_RcEditor.SetText(((TONG_JX2_RECORD_SYNC*)m_byRecord)->m_szAnnounce);
		return;
	}
	if (m_nRcSub == 0)
	{
		// muc tieu tuan: hien tu du lieu INFO (WeekGoal o field 22..28 - xem #17)
		m_Row[0].SetText("M\364c ti\252u tu\307n xem trong bang th\253ng tin.");
		return;
	}
	if (!m_bHasRecord)
		return;
	TONG_JX2_RECORD_SYNC* p = (TONG_JX2_RECORD_SYNC*)m_byRecord;
	if (p->m_btCount == 0)
		m_Row[0].SetText("(ch\255a c\343 b\266n ghi)");
	for (i = 0; i < (int)p->m_btCount && i < defTONG_JX2_RECORD_LINES; i++)
		m_Row[i].SetText(p->m_szLine[i]);
}

// Danh sach bang toan may chu (xem/xin gia nhap - mo duoc khi chua vao bang)
void KUiTongJX2::RenderTongList()
{
	int i;
	char sz[120];
	ClearRows();
	m_Row[0].SetText("T\252n bang           Bang ch\361        C\312p  TV");
	if (!m_bHasList)
		return;
	TONG_JX2_TONGLIST_SYNC* p = (TONG_JX2_TONGLIST_SYNC*)m_byList;
	if (p->m_btCount == 0)
		m_Row[2].SetText("(ch\255a c\343 bang h\351i n\265o)");
	for (i = 0; i < (int)p->m_btCount && i < defTONG_JX2_LIST_ROWS; i++)
	{
		sprintf(sz, "%s%-18s %-14s %3d %4d", (i == m_nSel) ? "> " : "  ",
			p->m_sTong[i].m_szName, p->m_sTong[i].m_szMaster,
			(int)p->m_sTong[i].m_btLevel, (int)p->m_sTong[i].m_wMember);
		m_Row[i + 1].SetText(sz);
	}
}

//////////////////////////////////////////////////////////////////////

void KUiTongJX2::OnAction(int nIdx)
{
	sTJX2Log("[ACT] idx=%d trangUI=%d sel=%d", nIdx, m_nPage, m_nSel);
	TONG_JX2_MEMBER_SYNC* pM = (TONG_JX2_MEMBER_SYNC*)m_byMember;
	DWORD dwTarget = 0;
	if (m_bHasMember && m_nSel < (int)pM->m_btCount)
		dwTarget = pM->m_sMember[m_nSel].m_dwNameID;

	switch (m_nPage)
	{
	case defTONG_JX2_PAGE_INFO:
		if (nIdx == 0)
			SendOp(defTONG_JX2_COP_DONATE, 0, 10, 0, NULL);
		else if (nIdx == 1)
			SendOp(defTONG_JX2_COP_DONATE, 0, 100, 0, NULL);
		else if (nIdx == 2)
			SendOp(defTONG_JX2_COP_UPGRADE, 0, 0, 0, NULL);
		else if (nIdx == 3)
			SendOp(defTONG_JX2_COP_DEGRADE, 0, 0, 0, NULL);
		else if (nIdx == 4)
		{
			// roi bang: dung duong hanh dong san co cua he cu (GTOI_TONG_ACTION)
			if (g_pCoreShell && m_bHasInfo)
			{
				TONG_JX2_INFO_SYNC* pI = (TONG_JX2_INFO_SYNC*)m_byInfo;
				KTongOperationParam sParam;
				KTongMemberItem sMe;
				memset(&sParam, 0, sizeof(sParam));
				memset(&sMe, 0, sizeof(sMe));
				sParam.eOper = TONG_ACTION_LEAVE;
				sParam.nData[0] = pI->m_btMyFigure;
				sParam.nData[1] = -1;
				strncpy(sMe.Name, pI->m_szSelf, sizeof(sMe.Name) - 1);
				g_pCoreShell->TongOperation(GTOI_TONG_ACTION, (unsigned int)&sParam, (int)&sMe);
				CloseWindow(false);
			}
		}
		else if (nIdx == 5)
			RequestPage(m_nPage, m_nStart);
		break;

	case defTONG_JX2_PAGE_MEMBER:
	case defTONG_JX2_PAGE_RIGHT:
		if (nIdx == 0 && dwTarget)
			SendOp(defTONG_JX2_COP_KICK, dwTarget, 0, 0, NULL);
		else if (nIdx == 1 && dwTarget)
			SendOp(defTONG_JX2_COP_GRANT, dwTarget, 10, 0, NULL);
		else if (nIdx == 2 && dwTarget)
			SendOp(defTONG_JX2_COP_SET_FIGURE, dwTarget, 1, 0, NULL);
		else if (nIdx == 3 && dwTarget)
			SendOp(defTONG_JX2_COP_SET_FIGURE, dwTarget, 2, 0, NULL);
		else if (nIdx == 4 && dwTarget)
			SendOp(defTONG_JX2_COP_SET_FIGURE, dwTarget, 3, 0, NULL);
		else if (nIdx == 5)
			ApplyRights();
		break;

	case defTONG_JX2_PAGE_WS:
		if (nIdx == 0)
			SendOp(defTONG_JX2_COP_WS_ADD, 0, m_nSel, 0, NULL);
		else if (nIdx == 1)
		{
			TONG_JX2_WS_SYNC* pW = (TONG_JX2_WS_SYNC*)m_byWs;
			if (m_bHasWs && m_nSel >= 1 && m_nSel <= 7)
				SendOp(pW->m_sWs[m_nSel].btOpen ? defTONG_JX2_COP_WS_CLOSE : defTONG_JX2_COP_WS_OPEN,
					0, m_nSel, 0, NULL);
		}
		else if (nIdx == 2)
			SendOp(defTONG_JX2_COP_WS_UP, 0, m_nSel, 0, NULL);
		else if (nIdx == 3)
			SendOp(defTONG_JX2_COP_SETSTUNT, 0, m_nSel, 0, NULL);
		else if (nIdx == 4)
			SendOp(defTONG_JX2_COP_SETSTUNT, 0, 0, 0, NULL);
		else if (nIdx == 5)
			RequestPage(m_nPage, m_nStart);
		break;

	case 4:	// thong bao
		if (nIdx == 0)
			SendOp(defTONG_JX2_COP_SETANN, 0, 0, 0,
				"Chao mung den voi bang hoi! Online deu de nhan tro cap.");
		else if (nIdx == 1)
			SendOp(defTONG_JX2_COP_SETANN, 0, 0, 0, "");
		else if (nIdx == 5)
			RequestPage(defTONG_JX2_PAGE_INFO, 0);
		break;
	case TJX2_UI_PAGE_TONGLIST:
		if (nIdx == 0)
		{
			TONG_JX2_TONGLIST_SYNC* pL = (TONG_JX2_TONGLIST_SYNC*)m_byList;
			if (m_bHasList && m_nSel < (int)pL->m_btCount)
				SendOp(defTONG_JX2_COP_APPLY_JOIN, pL->m_sTong[m_nSel].m_dwNameID, 0, 0, NULL);
		}
		else if (nIdx == 1)
			KUiTongCreateSheet::OpenWindow();	// don tao bang he cu
		else if (nIdx == 5)
			RequestPage(m_nPage, m_nStart);
		break;
	}
}

int KUiTongJX2::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)&m_BtnClose)
		{
			CloseWindow(false);
			return 1;
		}
		{
			int i;
			// tab -> trang UI: tab1 = chieu mo (JX2), tab2 = quan thanh vien
			// Tab0 Tin tuc = trang FunUse che do THONG TIN (dung nhu ban Linux)
			static const int s_nTabPage[TJX2_UI_TABS] =
			{
				TJX2_UI_PAGE_FUNUSE, TJX2_UI_PAGE_RECRUIT,
				defTONG_JX2_PAGE_MEMBER, defTONG_JX2_PAGE_WS, 4,
			};
			for (i = 0; i < TJX2_UI_TABS; i++)
			{
				if (uParam == (unsigned int)&m_BtnTab[i])
				{
					if (i == 0)
						m_nFunMode = 0;
					SwitchPage(s_nTabPage[i]);
					return 1;
				}
			}
			for (i = 0; i < TJX2_UI_ACTS; i++)
			{
				if (uParam == (unsigned int)&m_BtnAct[i])
				{
					OnAction(i);
					return 1;
				}
			}
			// bam vao dong: chon thanh vien (dong 0 la tieu de) / chon khu tac phuong
			for (i = 0; i < TJX2_UI_ROWS; i++)
			{
				if (uParam == (unsigned int)&m_BtnRowSel[i])
				{
					if (m_nPage == defTONG_JX2_PAGE_MEMBER || m_nPage == defTONG_JX2_PAGE_RIGHT ||
						m_nPage == TJX2_UI_PAGE_FUNUSE)
					{
						if (i >= 1)
						{
							if (m_nSel == i - 1)
								m_bMDet = !m_bMDet;	// bam lai dong dang chon: bat/tat panel
							else
							{
								m_nSel = i - 1;
								m_bMDet = 1;
							}
							RenderMembers();
						}
					}
					else if (m_nPage == defTONG_JX2_PAGE_WS)
					{
						if (i >= 1 && i <= 7)
						{
							m_nSel = i;
							RenderWorkshop();
						}
					}
					else if (m_nPage == TJX2_UI_PAGE_RECRUIT)
					{
						if (i < 8)
						{
							m_nSel = i;
							RenderRecruit();
						}
					}
					else if (m_nPage == TJX2_UI_PAGE_TONGLIST)
					{
						if (i >= 1)
						{
							m_nSel = i - 1;
							RenderTongList();
						}
					}
					return 1;
				}
			}
		}
		if (uParam == (unsigned int)&m_BtnFun)
		{
			m_nFunMode = 1;	// tab Su dung chuc nang
			SwitchPage(TJX2_UI_PAGE_FUNUSE);
			return 1;
		}
		if (uParam == (unsigned int)&m_BtnList)
		{
			SwitchPage(TJX2_UI_PAGE_TONGLIST);
			return 1;
		}
		{
			int q;
			for (q = 0; q < 14; q++)
			{
				if (uParam == (unsigned int)&m_Rt[q])
				{
					m_Rt[q].CheckButton(m_Rt[q].IsButtonChecked() ? 0 : 1);
					return 1;
				}
			}
			if (uParam == (unsigned int)&m_RtAll)
			{
				for (q = 0; q < 14; q++)
					m_Rt[q].CheckButton(1);
				return 1;
			}
			if (uParam == (unsigned int)&m_RtApply)
			{
				ApplyRights();
				return 1;
			}
			for (q = 0; q < 4; q++)
			{
				if (uParam == (unsigned int)&m_FunSub[q])
				{
					m_nFunSub = q + 1;
					SwitchPage(TJX2_UI_PAGE_FUNUSE);
					return 1;
				}
			}
			if (uParam == (unsigned int)&m_RecToggle)
			{
				if (g_pCoreShell)
				{
					int nOpen = g_pCoreShell->TongOperation(GTOI_TONG_GET_RECRUIT, 0, 0);
					g_pCoreShell->TongOperation(GTOI_TONG_RECRUIT, nOpen ? 0 : 1, 0);
					sTJX2Log("[REC] doi trang thai tuyen: %d -> %d", nOpen, nOpen ? 0 : 1);
				}
				return 1;
			}
			if (uParam == (unsigned int)&m_Bot[0])
			{
				// Vao bang nay: dang o danh sach bang -> xin vao bang dang chon
				if (m_nPage == TJX2_UI_PAGE_TONGLIST)
				{
					TONG_JX2_TONGLIST_SYNC* pL = (TONG_JX2_TONGLIST_SYNC*)m_byList;
					if (m_bHasList && m_nSel < (int)pL->m_btCount)
						SendOp(defTONG_JX2_COP_APPLY_JOIN, pL->m_sTong[m_nSel].m_dwNameID, 0, 0, NULL);
				}
				else
					SwitchPage(TJX2_UI_PAGE_TONGLIST);
				return 1;
			}
			if (uParam == (unsigned int)&m_Bot[1])
			{
				KUiTongCreateSheet::OpenWindow();
				return 1;
			}
			if (uParam == (unsigned int)&m_Bot[2])
			{
				SwitchPage(TJX2_UI_PAGE_TONGLIST);
				return 1;
			}
			if (uParam == (unsigned int)&m_Bot[3])
			{
				CloseWindow(false);
				return 1;
			}
		}
		{
			int f;
			for (f = 0; f < 14; f++)
			{
				if (uParam != (unsigned int)&m_FunBtn[f])
					continue;
				TONG_JX2_MEMBER_SYNC* pFM = (TONG_JX2_MEMBER_SYNC*)m_byMember;
				DWORD dwFT = 0;
				if (m_bHasMember && m_nSel < (int)pFM->m_btCount)
					dwFT = pFM->m_sMember[m_nSel].m_dwNameID;
				switch (s_sFunBtn[f].nAct)
				{
				case 0:
					SendOp(defTONG_JX2_COP_UPGRADE, 0, 0, 0, NULL);
					break;
				case 1:
					if (dwFT)
						SendOp(defTONG_JX2_COP_GRANT, dwFT, 10, 0, NULL);
					break;
				case 2:
					if (dwFT)
						SendOp(defTONG_JX2_COP_KICK, dwFT, 0, 0, NULL);
					break;
				case 3:
					if (dwFT)
						SendOp(defTONG_JX2_COP_SET_FIGURE, dwFT, 3, 0, NULL);
					break;
				case 4:
					SwitchPage(TJX2_UI_PAGE_RECRUIT);
					break;
				case 5:
					SendOp(defTONG_JX2_COP_MAP_CREATE, 0, 0, 0, NULL);
					break;
				case 6:
					SendOp(defTONG_JX2_COP_MAP_DELETE, 0, 0, 0, NULL);
					break;
				case 7:
					SendOp(defTONG_JX2_COP_STORE_OFFER, 0, 10, 0, NULL);
					break;
				case 9:
					SendOp(defTONG_JX2_COP_DRAW_MONEY, 0, 10, 0, NULL);	// rut 10 van
					break;
				case 10:
					if (dwFT)
						SendOp(defTONG_JX2_COP_PAY_MEMBER, dwFT, 10, 0, NULL);	// phat 10 van
					break;
				case 8:
					{
						// muon duong roi bang cua trang Tin tuc (OnAction xet m_nPage)
						int nKeep = m_nPage;
						m_nPage = defTONG_JX2_PAGE_INFO;
						OnAction(4);
						m_nPage = nKeep;
					}
					break;
				}
				return 1;
			}
		}
		{
			int rc;
			for (rc = 0; rc < 4; rc++)
			{
				if (uParam == (unsigned int)&m_RcSub[rc])
				{
					m_nRcSub = rc;
					SwitchPage(4);
					return 1;
				}
			}
			if (uParam == (unsigned int)&m_RcSave)
			{
				char szAnn[256];
				szAnn[0] = 0;
				m_RcEditor.GetText(szAnn, sizeof(szAnn), false);
				szAnn[127] = 0;
				SendOp(defTONG_JX2_COP_SETANN, 0, 0, 0, szAnn);
				return 1;
			}
			if (uParam == (unsigned int)&m_RcLeaveWord)
			{
				char szWord[256];
				szWord[0] = 0;
				m_RcEditor.GetText(szWord, sizeof(szWord), false);
				szWord[127] = 0;
				if (szWord[0])
					SendOp(defTONG_JX2_COP_LEAVE_WORD, 0, 0, 0, szWord);
				return 1;
			}
		}
		{
			int w;
			for (w = 1; w <= 7; w++)
			{
				if (uParam == (unsigned int)&m_WsIcon[w])
				{
					m_nSel = w;
					RenderWorkshop();
					return 1;
				}
			}
			for (w = 0; w < 6; w++)
			{
				if (uParam != (unsigned int)&m_WsBtn[w])
					continue;
				if (m_nSel < 1 || m_nSel > 7)
					return 1;
				switch (w)
				{
				case 0:
					SendOp(defTONG_JX2_COP_WS_ADD, 0, m_nSel, 0, NULL);
					break;
				case 1:
					SendOp(defTONG_JX2_COP_WS_OPEN, 0, m_nSel, 0, NULL);
					break;
				case 2:
					SendOp(defTONG_JX2_COP_WS_CLOSE, 0, m_nSel, 0, NULL);
					break;
				case 3:
					SendOp(defTONG_JX2_COP_WS_UP, 0, m_nSel, 0, NULL);
					break;
				case 4:
					SendOp(defTONG_JX2_COP_WS_SETLV, 0, m_nSel, 10, NULL);
					break;
				case 5:
					SendOp(defTONG_JX2_COP_WS_DEL, 0, m_nSel, 0, NULL);
					break;
				}
				return 1;
			}
		}
		if (uParam == (unsigned int)&m_RecQX)
		{
			char szN[8];
			m_nRecQX = (m_nRecQX + 1) % 10;
			sprintf(szN, "%d", m_nRecQX);
			m_RecQX.SetLabel(szN);
			return 1;
		}
		{
			int r;
			for (r = 0; r < 4; r++)
			{
				if (uParam == (unsigned int)&m_RecHD[r])
				{
					char szN[8];
					m_nRecHD[r] = (m_nRecHD[r] + 1) % 10;
					sprintf(szN, "%d", m_nRecHD[r]);
					m_RecHD[r].SetLabel(szN);
					return 1;
				}
			}
		}
		if (uParam == (unsigned int)&m_RecSave)
		{
			// thu thap noi dung trang chieu mo -> COP_SAVE_RECRUIT
			char szJiyu[256];
			char szNum[16];
			int nAuto = 0, nRefuse = 0;
			szJiyu[0] = 0;
			m_RecJiyu.GetText(szJiyu, sizeof(szJiyu), false);
			szNum[0] = 0;
			m_RecAuto.GetText(szNum, sizeof(szNum), false);
			nAuto = atoi(szNum);
			szNum[0] = 0;
			m_RecRefuse.GetText(szNum, sizeof(szNum), false);
			nRefuse = atoi(szNum);
			if (nAuto < 0) nAuto = 0;
			if (nAuto > 200) nAuto = 200;
			if (nRefuse < 0) nRefuse = 0;
			if (nRefuse > 200) nRefuse = 200;
			szJiyu[127] = 0;
			int nP1 = (m_nRecQX & 15) | ((m_nRecHD[0] & 15) << 4) | ((m_nRecHD[1] & 15) << 8)
				| ((m_nRecHD[2] & 15) << 12) | ((m_nRecHD[3] & 15) << 16);
			int nP2 = (nAuto & 255) | ((nRefuse & 255) << 8);
			SendOp(defTONG_JX2_COP_SAVE_RECRUIT, 0, nP1, nP2, szJiyu);
			return 1;
		}
		if (uParam == (unsigned int)&m_RecAccept || uParam == (unsigned int)&m_RecDeny)
		{
			TONG_JX2_RECRUIT_SYNC* pR = (TONG_JX2_RECRUIT_SYNC*)m_byRecruit;
			if (m_bHasRecruit && m_nSel >= 0 && m_nSel < (int)pR->m_btApplyCount)
				SendOp(uParam == (unsigned int)&m_RecAccept ?
					defTONG_JX2_COP_ACCEPT_APPLY : defTONG_JX2_COP_REFUSE_APPLY,
					pR->m_sApply[m_nSel].m_dwNameID, 0, 0, NULL);
			return 1;
		}
		if (uParam == (unsigned int)&m_RecPrev || uParam == (unsigned int)&m_RecNext)
		{
			RequestPage(m_nPage, 0);	// danh sach don toi da 8 - lam moi
			return 1;
		}
		if (uParam == (unsigned int)&m_BtnPrev)
		{
			if (m_nStart >= defTONG_JX2_VIEW_MEMBERS)
				m_nStart -= defTONG_JX2_VIEW_MEMBERS;
			else
				m_nStart = 0;
			m_nSel = 0;
			RequestPage(m_nPage, m_nStart);
			return 1;
		}
		if (uParam == (unsigned int)&m_BtnNext)
		{
			TONG_JX2_MEMBER_SYNC* pM = (TONG_JX2_MEMBER_SYNC*)m_byMember;
			if (m_bHasMember && m_nStart + defTONG_JX2_VIEW_MEMBERS < (int)pM->m_wTotal)
			{
				m_nStart += defTONG_JX2_VIEW_MEMBERS;
				m_nSel = 0;
				RequestPage(m_nPage, m_nStart);
			}
			return 1;
		}
		break;
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}
