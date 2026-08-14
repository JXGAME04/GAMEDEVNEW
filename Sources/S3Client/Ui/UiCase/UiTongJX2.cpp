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
#include "UiTongGetString.h"
#include "UiInformation.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"	// KRUShadow + DrawPrimitives (ve nen panel)
extern iRepresentShell*	g_pRepresentShell;	// khai bao nhu MouseHover.cpp:16 / PopupMenu.cpp:21		// UIMessageBox - hop xac nhan truoc thao tac khong hoan tac duoc

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

// 14 quyen theo DUNG thu tu mat na ban goc (= s_dwJX2RightList server;
// lech mot vi tri la phan quyen sai bit)
static const char* s_szRightName[14] =
{
	"B\346 nhi\326m - mi\324n nhi\326m", "\247\346i phe", "\247\346i t\252n",
	"Tr\364c xu\312t", "Nh\313t k\375", "Qu\266n l\375 li\252n minh",
	"Th\250ng c\312p", "Tho\270i \310n", "Khu v\371c", "T\270c ph\255\352ng",
	"Ng\251n qu\374", "M\364c ti\252u tu\307n", "Th\265nh th\336", "K\374 n\250ng",
};
static const DWORD s_dwRightId[14] =
{
	1002, 1003, 1004, 1901, 1903, 1101, 2001,
	1902, 2004, 9001, 3001, 2005, 2003, 2006,
};

// Ten 7 khu tac phuong - chep NGUYEN VAN cot NAME cua bang du lieu ban
// Linux (settings/tong/workshop/workshops.txt), da cat dau cach thua o cuoi.
static const char* s_szWsName[8] =
{
	"",
	"Khu Binh gi\270p",
	"Khu Thi\252n C\253ng",
	"Khu M\306t n\271",
	"Khu Luy\326n t\313p",
	"Khu Thi\252n \375",
	"Khu L\324 v\313t",
	"Khu ho\271t \256\351ng",
};

// 21 anh icon = 7 khu x 3 trang thai, lay tu chinh bang du lieu tren:
//   [t][0] = OPEN_ICON (da lap va dang mo)
//   [t][1] = CLOSE_ICON (da lap nhung dang dong)
//   [t][2] = UNFOUNDED_ICON (chua lap)
static const char* s_szWsIcon[8][3] =
{
	{ "", "", "" },
	{ "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\261\370\274\327\267\273_0.spr",
	  "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\261\370\274\327\267\273_2.spr",
	  "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\261\370\274\327\267\273_1.spr" },
	{ "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\314\354\271\244\267\273_0.spr",
	  "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\314\354\271\244\267\273_2.spr",
	  "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\314\354\271\244\267\273_1.spr" },
	{ "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\303\346\276\337\267\273_0.spr",
	  "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\303\346\276\337\267\273_2.spr",
	  "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\303\346\276\337\267\273_1.spr" },
	{ "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\312\324\301\266\267\273_0.spr",
	  "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\312\324\301\266\267\273_2.spr",
	  "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\312\324\301\266\267\273_1.spr" },
	{ "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\314\354\322\342\267\273_0.spr",
	  "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\314\354\322\342\267\273_2.spr",
	  "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\314\354\322\342\267\273_1.spr" },
	{ "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\300\361\316\357\267\273_0.spr",
	  "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\300\361\316\357\267\273_2.spr",
	  "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\300\361\316\357\267\273_1.spr" },
	{ "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\273\356\266\257\267\273_0.spr",
	  "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\273\356\266\257\267\273_2.spr",
	  "\\spr\\Ui3\\\260\357\273\341\275\347\303\346\\\327\367\267\273\320\305\317\242\322\263\\\273\356\266\257\267\273_1.spr" },
};

// ten 10 mon phai JX1 (id 1..10) - TCVN3
static const char* s_szFaction[11] =
{
	"-", "Thi\325u L\251m", "Thi\252n V\255\254ng", "\247\255\352ng M\253n",
	"Ng\362 \247\351c", "Nga My", "Th\363y Y\252n", "C\270i Bang",
	"Thi\252n Nh\311n", "V\342 \247ang", "C\253n L\253n",
};

// Ten 5 chuc vu - NGUYEN VAN bang chuoi ban Linux (stringtable_client.txt:481-485).
// Muc 0 va 3 cua ban goc CO dau cach o cuoi, giu de dung tung byte.
// CAM dung enum TONG_MEMBER_FIGURE cua JX1 (GameDataDef.h) de danh chi so:
// enum do co thu tu NGUOC (MEMBER=0 ... MASTER=3), con fork JX2 la
// 0 = Bang chu ... 4 = An sy.
static const char* s_szFigure[5] =
{
	"Bang ch\361 ", "Tr\255\353ng L\267o", "\247\351i tr\255\353ng",
	"\247\326 t\366 ", "\310n s\374",
};

// Trang Tin tuc: bind NGUYEN VAN section blueprint 帮会基础信息页面.ini
// (Info_<ten section>); nhan Title dat chuoi TCVN3, Txt do RenderInfo dien.
struct TJX2InfoCtl
{
	const char* szSec;		// ten section goc trong blueprint
	const char* szLabel;	// nhan TCVN3 (NULL = o du lieu)
};
// Trang "Tin tuc" = ban thiet ke BASEINFO (trang thong tin co ban) cua ban
// Linux, KHONG phai FUNUSE (trang chuc nang) nhu phien truoc ket luan nham -
// vi lay nham ban ve nen o thu hai cua hang 1 bi hieu la "Lien minh" va bi do
// ten BANG CHU vao. Chi lay DUNG nhung hang co trong anh mau: ban goc con
// 7 hang nua (ServiceFee/StandFund/BattleFund/StoredBuildFund/Liveness/
// MyLiveness/DailyCost) va 3 trong so do CHONG TOA DO TUYET DOI voi hang dang
// hien (Liveness trung khit Ngan quy, MyLiveness trung khit Ngan sach kien
// thiet, ServiceFee giao Dang cap kien thiet) - nap ca 11 hang la chu de len nhau.
#define TJX2_INFO_NUM	29
static const TJX2InfoCtl s_sInfoCtl[TJX2_INFO_NUM] =
{
	{"TitleTongInfo",     "Tin t\370c"},
	{"TitleTongName",     "Bang h\351i"},          {"TxtTongName",      NULL},
	{"TitleMaster",       "Bang ch\361"},          {"TxtMaster",        NULL},
	{"TitleLeague",       "Li\252n minh"},         {"TxtLeague",        NULL},
	{"TitleCamp",         "Phe"},                  {"TxtCamp",          NULL},
	{"TitleTongLevel",    "\247\274ng c\312p"},    {"TxtTongLevel",     NULL},
	{"TitleMemberNum",    "Nh\251n s\350"},        {"TxtMemberNum",     NULL},
	{"TitleBuildLevel",   "\247\274ng c\312p ki\325n thi\325t"}, {"TxtBuildLevel", NULL},
	{"TitleTongCapital",  "Ng\251n qu\374"},       {"TxtTongCapital",   NULL},
	{"TitleBuildFund",    "Ng\251n s\270ch ki\325n thi\325t"}, {"TxtBuildFund", NULL},
	{"TitleTotalOffer",   "C\350ng hi\325n d\371 tr\367"}, {"TxtTotalOffer", NULL},
	{"TitlePersonalInfo", "Tin t\370c c\270 nh\251n"},
	{"TitlePersonalOffer","C\350ng hi\325n c\270 nh\251n"}, {"TxtPersonalOffer", NULL},
	{"TitleWeeklyOffer",  "C\350ng hi\325n tu\307n"}, {"TxtWeeklyOffer", NULL},
	{"TxtHelpTitle",      "Gi\363p \256\354"},
	{"Van1",              "v\271n l\255\356ng"},   {"Van2",             "v\271n l\255\356ng"},
};
// Chi so o GIA TRI de RenderInfo do so lieu vao (khop bang tren)
#define TJX2_INFO_TONGNAME		2
#define TJX2_INFO_MASTER		4
#define TJX2_INFO_LEAGUE		6
#define TJX2_INFO_CAMP			8
#define TJX2_INFO_TONGLEVEL		10
#define TJX2_INFO_MEMBERNUM		12
#define TJX2_INFO_BUILDLEVEL	14
#define TJX2_INFO_CAPITAL		16
#define TJX2_INFO_BUILDFUND		18
#define TJX2_INFO_TOTALOFFER	20
#define TJX2_INFO_MYOFFER		23
#define TJX2_INFO_WEEKOFFER		25

// 14 nut quyen cua blueprint trang Phan phoi (RightID doc tu chinh ini)
static const char* s_szRtSec[14] =
{
	"BtnDepose", "BtnChangeCamp", "BtnChangeTitle", "BtnKickOut",
	"BtnRecordEvent", "BtnLeagueManage", "BtnUpgradeBuildLevel", "BtnForceToRetire",
	"BtnMapManagement", "BtnWorkshopManagement", "BtnFundManagement",
	"BtnWeekGoalManagement", "BtnCityManagement", "BtnStuntManagement",
};

// nhan 14 o kiem quyen (cung thu tu mat na s_dwRightId) - TCVN3 co dau
static const char* s_szChkLabel[14] =
{
	"B\346 nhi\326m", "\247\346i phe", "\247\346i t\252n", "Tr\364c xu\312t",
	"Nh\313t k\375", "Li\252n minh", "Th\250ng c\312p", "Tho\270i \310n",
	"Khu v\371c", "T\270c ph\255\352ng", "Ng\251n qu\374", "M\364c ti\252u tu\307n",
	"Th\265nh th\336", "K\374 n\250ng",
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
#define TJX2_UI_PAGE_TONGLIST	7
#define TJX2_UI_PAGE_OTHERZM	8	// trang 2x2 xem chieu mo bang khac (blueprint rieng)	// danh sach bang (mo duoc khi CHUA vao bang)
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
// Menu trang Chieu mo - chep NGUYEN VAN tu ban thiet ke goc
// D:\ServerLinux\Patch\ui\ui3_1024\<Chieu mo>.ini [QingXiangMenu] MenuItemCount=6
// va [HuoDongMenu] MenuItemCount=11. Chi so 0 = chua chon (menu goc danh so tu 1).
#define TJX2_QX_NUM		7
static const char* s_szRecQX[TJX2_QX_NUM] =
{
	"(ch\255a ch\355n)",
	"R\266nh m\355i ch\254i", "Th\335ch PK", "Tranh b\270",
	"D\371 th\325 v\253 tranh", "T\335nh n\250ng PVE", "K\325t giao",
};
#define TJX2_HD_NUM		12
static const char* s_szRecHD[TJX2_HD_NUM] =
{
	"(ch\255a ch\355n)",
	"T\350ng Kim", "V\255\356t \266i", "\247\270nh Boss", "C\253ng Th\265nh",
	"Phong L\250ng \256\351", "Nhi\326m v\364 D\267 T\310u", "Vi\252m \256\325",
	"T\335n S\370", "M\351c Nh\251n", "V\342 l\251m li\252n \256\312u",
	"Thi\252n Tr\327 M\313t C\266nh",
};

// 11 = gop tien ca nhan vao QUY KIEN THIET (COP_DONATE, nhan cong hien);
// 13 = nap tien ca nhan vao NGAN QUY bang (COP_DEPOSIT_MONEY) - hai duong
// tien TACH RIENG nhu ban goc (MONEY2BUILDFUND / MONEYFUND_ADD).
// 12 = doi phe bang hoi: KHONG can ma lenh JX2 moi - he JX1 da co san tron bo
// duong day (ApplyTongChangeCamp kiem chuc vu + tru tien, relay co DBChangeCamp),
// di y het cach nut "Roi bang" dang lam qua GTOI_TONG_ACTION.
#define TJX2_FUN_BTNS	24
static const TJX2FunBtn s_sFunBtn[TJX2_FUN_BTNS] =
{
	{"BtnUpgradeBuildLevel", "N\251ng c\312p", 0},
	{"BtnAssignTongOffer",   "Ph\270t", 1},
	{"BtnGetTongMoney",      "R\363t", 9},	// nguyen van ban goc; chuoi 4 byte
											// "Nhan" cua ta bi nut rong 25px cat thanh "N.."
	{"BtnAssignTongMoney",   "Ph\270t", 10},
	{"BtnTransformMoney",    "\247\346i", -1},
	{"BtnRecruit",           "Chi\252u m\351", 4},	// sprite goc -
											// KHONG co trong pak -> ini dung 5
	{"BtnKickOut",           "\247u\346i ng\255\352i", 2},
	{"BtnDepose",            "Tru\312t ch\370c", 3},
	{"Btn_DispenseOffer",    "Ph\270t c\350ng hi\325n", 1},
	{"BtnCreateTongMap",     "T\271o l\267nh \256\336a", 5},
	{"BtnChangeCamp",       "\247\346i phe", 12},
	{"BtnConfigureTongMap",  "", -1},
	{"BtnTongStunt",         "", -1},
	{"BtnStorePersonalOffer","G\366i", 7},	// nguyen van blueprint (Label=G\366i)
	{"BtnLeaveTong",         "R\352i bang", 8},
	{"BtnStoreBuildFund",    "G\366i", 11},
	{"BtnStoreTongMoney",    "G\366i", 13},	// nap ngan quy (MONEYFUND_ADD ban goc)
	// trang con 3 - LIEN MINH (nhan nguyen van blueprint, ke ca loi go
	// "li\252m" cua ban goc va dau cach dau nhan dai than)
	{"BtnCreateUnion",       "L\313p li\252n minh", 14},
	{"BtnApplyJionUnion",    "V\265o li\252m minh", 15},
	{"BtnAcceptUnionReq",    "Gia nh\313p li\252n minh", 16},
	{"BtnLeaveUnion",        "H\361y li\252n minh", 17},
	{"BtnKickUnionTong",     "Tr\364c xu\312t", 18},
	// trang con 4 - DAI THAN QUOC GIA
	{"BtnAppointMinister",   " \361y nhi\326m \256\271i th\307n", 19},
	{"BtnFireMinister",      " C\270ch ch\370c \256\271i th\307n", 20},
};

// Trang Phuong tho: bind blueprint (Ws_*). Title co nhan TCVN3, Txt do render dien.
static const TJX2FunTxt s_sWsTxt[6] =
{
	{"TitleWorkshopNum", "S\350 t\270c ph\255\352ng"}, {"TxtWorkshopNum", NULL},
	{"TitleMaxRank", "Gi\355i h\271n c\312p"}, {"TxtMaxRank", NULL},
	{"TitleServiceFee", "B\266o tr\327 h\265ng ng\265y"}, {"TxtServiceFee", NULL},
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

// Nhan 4 muc con trang Nhat ky - NGUYEN VAN khoa Label= cua ban thiet ke goc
// (truoc day ta tu rut gon thanh "Thong bao" / "Bang vu" / "Lich su").
static const char* s_szRcSub[4] =
{
	"M\364c ti\252u tu\307n", "C\253ng c\270o bang h\351i",
	"S\371 ki\326n bang h\351i", "L\336ch s\366 bang h\351i",
};

//////////////////////////////////////////////////////////////////////

KUiTongJX2::KUiTongJX2()
{
	m_nPage = defTONG_JX2_PAGE_INFO;
	m_nStart = 0;
	m_nRecStart = 0;
	m_nSel = 0;
	m_nSelWs = 1;
	m_bOnlineFirst = 0;
	m_nSortMode = 0;
	m_nPendOp = -1;
	m_szPendText[0] = 0;
	m_nAmtOp = -1;
	m_dwAmtTarget = 0;
	m_dwPendTarget = 0;
	m_nPendP1 = 0;
	m_nPendP2 = 0;
	memset(m_nOrd, 0, sizeof(m_nOrd));
	m_bHasInfo = 0;
	m_bHasMember = 0;
	m_bHasWs = 0;
	m_bHasRecruit = 0;
	m_bHasRecord = 0;
	m_bHasList = 0;
	memset(m_byList, 0, sizeof(m_byList));
	m_bHasZM = 0;
	m_nZmStart = 0;
	memset(m_byZM, 0, sizeof(m_byZM));
	m_nRcSub = 2;	// mac dinh Bang vu (so su kien)
	m_nFunMode = 0;
	m_nFunSub = 1;
	m_bMDet = 0;
	m_nMDetTop = 0;
	m_nMDetRows = 0;
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
		ms_pSelf->SwitchPage(defTONG_JX2_PAGE_INFO);	// trang Tin tuc (BASEINFO)
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
		// chua vao bang: mo trang 2x2 Xem tin Bang khac (dung vai tro
		// [BtnOtherZhaoMu] cua ban goc); nut Tao moi van mo don tao bang
		KUiTongJX2* pWnd = OpenWindow();
		if (pWnd)
			pWnd->SwitchPage(TJX2_UI_PAGE_OTHERZM);
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
	// nen trang Chuc nang cung la NEN PHAN TRANG - phai add o day cho nam DUOI
	// cung. Truoc day add sau m_BtnRowSel nen no trum kin panel danh sach:
	// vua che danh sach thanh vien vua nuot cu bam (Trans=0 = dac).
	AddChild(&m_FunBg);
	// buc tranh nen trang Tac phuong: anh nen phan trang [PageBg3] co mot LO
	// TRONG SUOT dung 298x226 tai (28,76), va khung cua so chinh cung trong
	// suot cho do -> khong ve cai nay thi nhin XUYEN qua cua so ra canh game.
	// Ban thiet ke goc de no o file RIENG (trang con tac phuong) nen tai lieu
	// tra cuu bo sot, keo theo ta bo sot ca buc tranh.
	AddChild(&m_WsArt);
	for (i = 0; i < 14; i++)
		AddChild(&m_Rt[i]);
	AddChild(&m_RtAll);
	AddChild(&m_RtApply);
	for (i = 0; i < 3; i++)
		AddChild(&m_ColHdr[i]);
	for (i = 0; i < TJX2_UI_ROWS; i++)
		AddChild(&m_MList[i]);
	for (i = 1; i <= 7; i++)
		AddChild(&m_WsIconBg[i]);
	// Thu tu ve dung ban goc: nen (35x48) -> ICON THAT cua khu -> cao sang ->
	// chu cap -> khung chon. m_WsIcon vua la anh icon vua la o BAM nen phai
	// nam DUOI cac lop trang tri, va cac lop do phai Enable(false) de khong
	// nuot cu bam (giong [HLImage] ban goc co NoMsg=1 Enable=0).
	for (i = 1; i <= 7; i++)
		AddChild(&m_WsIcon[i]);
	for (i = 0; i < 8; i++)
		AddChild(&m_WsIconHL[i]);
	for (i = 0; i < 8; i++)
		AddChild(&m_WsRank[i]);
	for (i = 0; i < 3; i++)
		AddChild(&m_WsSub[i]);
	AddChild(&m_WsIconSel);
	for (i = 0; i < 4; i++)
		AddChild(&m_Bot[i]);
	AddChild(&m_RecToggle);
	for (i = 0; i < TJX2_UI_TABS; i++)
		AddChild(&m_BtnTab[i]);
	for (i = 0; i < TJX2_UI_ROWS; i++)
		AddChild(&m_Row[i]);
	// nen do phai AddChild TRUOC chu, vi AddChild noi vao CUOI danh sach va
	// cua so ve theo dung thu tu do -> them sau = ve len tren = che mat chu
	for (i = 0; i < TJX2_INFO_NUM; i++)
		AddChild(&m_InfoBg[i]);
	for (i = 0; i < TJX2_INFO_NUM; i++)
		AddChild(&m_Info[i]);
	for (i = 0; i < TJX2_UI_ROWS; i++)
		AddChild(&m_BtnRowSel[i]);
	for (i = 0; i < TJX2_UI_ACTS; i++)
		AddChild(&m_BtnAct[i]);
	AddChild(&m_BtnPrev);
	AddChild(&m_BtnNext);
	AddChild(&m_MOnline);
	AddChild(&m_MSort);
	AddChild(&m_MJump);
	AddChild(&m_MPage);
	AddChild(&m_MPageEdit);
	for (i = 0; i < 15; i++)
		AddChild(&m_FunTxtBg[i]);
	for (i = 0; i < 6; i++)
		AddChild(&m_FunPBg[i]);
	// anh ten trang con: AddChild TRUOC cum nut hanh dong (AddChild noi
	// vao CUOI va ve theo thu tu do - them sau la de len tren, che nut)
	AddChild(&m_FunMask);
	for (i = 0; i < 15; i++)
		AddChild(&m_FunTxt[i]);
	for (i = 0; i < 7; i++)
		AddChild(&m_FunP[i]);
	for (i = 0; i < 14; i++)
		AddChild(&m_RowDim[i]);
	for (i = 0; i < TJX2_FUN_BTNS; i++)
		AddChild(&m_FunBtn[i]);
	for (i = 0; i < 4; i++)
		AddChild(&m_FunSub[i]);
	AddChild(&m_BtnFun);
	for (i = 0; i < 6; i++)
		AddChild(&m_WsTxt[i]);
	for (i = 0; i < 6; i++)
		AddChild(&m_WsBtn[i]);
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
	// trang 2x2 Xem tin Bang khac: AddChild CUOI de nut the nam TREN cac
	// lop dung chung (m_Row rong van an chuot neu nam tren nut cua the)
	{
		int z;
		AddChild(&m_ZmBg);
		for (z = 0; z < 4; z++)
		{
			AddChild(&m_ZmName[z]);
			AddChild(&m_ZmInfo[z]);
			AddChild(&m_ZmJiyu[z]);
			AddChild(&m_ZmQxT[z]);
			AddChild(&m_ZmQx[z]);
			AddChild(&m_ZmHdT[z]);
			AddChild(&m_ZmHd[z]);
		}
		for (z = 0; z < 4; z++)
		{
			AddChild(&m_ZmLook[z]);
			AddChild(&m_ZmApply[z]);
		}
		AddChild(&m_ZmPrev);
		AddChild(&m_ZmNext);
	}

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	// tab dung anh 招募页vn cua ban Linux (chu Viet nung san trong anh) -> khong ve chu de
	for (i = 0; i < TJX2_UI_TABS; i++)
		m_BtnTab[i].SetLabel("");
	// m_BtnPrev/m_BtnNext: nhan tu ini (Label blueprint)
	// nhan cac nut lay TRUC TIEP tu key Label trong ini (blueprint nung san)
	m_RecQX.SetLabel(s_szRecQX[0]);
	for (i = 0; i < 4; i++)
		m_RecHD[i].SetLabel(s_szRecHD[0]);
	m_BtnFun.SetLabel("");	// sprite nut co chu san tren cua so chinh
	// m_WsBtn: nhan tu ini
	// KHONG ve so tren icon: ban goc khong co so, va cap khu da hien o dong
	// chu ngay duoi tung icon (m_WsRank).
	for (i = 0; i < 4; i++)
		m_RcSub[i].SetLabel(s_szRcSub[i]);
	// KHONG SetLabel cho hai nut nay: section [Rc_BtnLeaveWord] va
	// [Rc_BtnEditAnnounce] KHONG co khoa Label= trong khi cac nut khac deu co
	// => sprite cua chung DA NUNG CHU SAN, ve them chu la chong chu.
	m_BtnList.SetLabel("");	// sprite nut co san chu
	for (i = 0; i < TJX2_FUN_BTNS; i++)
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
	// Ba o tieu de cot THAT SU la TxtRank / TxtTitle / TxtType. [Fun_TitleName]
	// KHONG phai control - no la BANG CHUOI de doi chu luc chay (danh sach
	// thanh vien -> "Thanh vien"/"Chuc vu"; danh sach bang lien minh ->
	// "Bang hoi"/"Chuc vu"). Truoc day gan [Fun_TitleName] cho m_ColHdr[1]
	// nen o do khong co toa do (0x0) va vo hinh -> chi doc duoc "Hang  Ten".
	{
		char szH[64];
		ms_pSelf->m_MOnline.Init(&Ini, "Fun_BtnOnlinePriority");
		ms_pSelf->m_MSort.Init(&Ini, "Fun_BtnMemberSortMenu");
		ms_pSelf->m_MJump.Init(&Ini, "Fun_BtnJump");
		ms_pSelf->m_MPage.Init(&Ini, "Fun_TitlePage");
		ms_pSelf->m_MPageEdit.Init(&Ini, "Fun_EditBoxDestPage");
		// KHONG SetLabel cho nut sap xep: section [Fun_BtnMemberSortMenu] khong
		// co khoa Label= (khac cac nut ben canh) => sprite cua no da nung chu
		// san, ve them chu la chong chu.
		ms_pSelf->m_ColHdr[0].Init(&Ini, "Fun_TxtRank");
		ms_pSelf->m_ColHdr[1].Init(&Ini, "Fun_TxtTitle");
		ms_pSelf->m_ColHdr[2].Init(&Ini, "Fun_TxtType");
		Ini.GetString("Fun_TitleName", "MemberTitle", "", szH, sizeof(szH));
		if (szH[0])
			ms_pSelf->m_ColHdr[1].SetText(szH);
		Ini.GetString("Fun_TitleName", "UnionTongFigureTitle", "", szH, sizeof(szH));
		if (szH[0])
			ms_pSelf->m_ColHdr[2].SetText(szH);
	}
	// 4 nut day tro DUNG bon o cua ban thiet ke goc (toa do + sprite goc), thay
	// cho 4 section Bot_* tu che truoc day (sprite "nut trang" tam DAC nen ra
	// 4 thanh dai phang). Chu game da xac nhan nut thu 2 doc la "Lam moi" -
	// khop [BtnRefresh] cua ban goc.
	// KHONG SetLabel: 4 sprite goc DA CO CHU NUNG SAN (chu game chup duoc canh
	// chu den de len chu vang). Nhan trong ini cung rong san.
	ms_pSelf->m_Bot[0].Init(&Ini, "BtnEnterMap");
	ms_pSelf->m_Bot[1].Init(&Ini, "BtnRefresh");
	ms_pSelf->m_Bot[2].Init(&Ini, "BtnTongList");
	ms_pSelf->m_Bot[3].Init(&Ini, "BtnClose");
	ms_pSelf->m_RecToggle.Init(&Ini, "Rec_ToggleRecruit");
	ms_pSelf->m_RecToggle.SetLabel("\247\343ng/m\353 tuy\323n");
	// BO HAN khoi 13 muc Fun2_*: chung la ban dan NHAM tu trang Thong tin co ban
	// sang trang Chuc nang. Ca 13 muc deu de len mot muc cua ban thiet ke goc
	// trang Chuc nang - do la nguon goc cua chu "van luong" thua, dong tieu de
	// "Giup do" thua va hang "Cong hien tuan" thua ma chu game thay. Ba thu do
	// VAN CO o tab Tin tuc (khoi Info_*) dung nhu anh mau.
	{
		for (i = 1; i <= 7; i++)
		{
			sprintf(szSec, "Ws_IconBg%d", i);
			ms_pSelf->m_WsIconBg[i].Init(&Ini, szSec);
			sprintf(szSec, "Ws_IconHL%d", i);
			ms_pSelf->m_WsIconHL[i].Init(&Ini, szSec);
			ms_pSelf->m_WsIconHL[i].Enable(false);	// lop trang tri, khong an chuot
			sprintf(szSec, "Ws_Rank%d", i);
			ms_pSelf->m_WsRank[i].Init(&Ini, szSec);
			ms_pSelf->m_WsRank[i].Enable(false);	// chu khong duoc nuot chuot cua o bam
		}
		ms_pSelf->m_WsIconSel.Init(&Ini, "Ws_IconSel");
		ms_pSelf->m_WsArt.Init(&Ini, "Ws_ArtBg");
		for (i = 0; i < TJX2_UI_ROWS; i++)
			ms_pSelf->m_MList[i].SetPosition(341, 68 + i * 24);
		for (i = 1; i <= 7; i++)
			ms_pSelf->m_WsIcon[i].SetLabel("");	// bo so - da co hinh nen
	}
	for (i = 0; i < TJX2_INFO_NUM; i++)
	{
		sprintf(szSec, "Info_%s", s_sInfoCtl[i].szSec);
		ms_pSelf->m_InfoBg[i].Init(&Ini, szSec);	// doc khoa Image= (neu co)
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
	// HAI HANG con lai cua khoi Tin tuc ca nhan - ban thiet ke goc CO du
	// ba hang (Cong hien ca nhan / Ngan quy / Ngan sach kien thiet), moi
	// hang = khung do + o so + nut Gui. Truoc day chi bind hang dau nen
	ms_pSelf->m_FunPBg[2].Init(&Ini, "Fun_TitleTongMoney2");
	ms_pSelf->m_FunP[3].Init(&Ini, "Fun_TitleTongMoney2");
	ms_pSelf->m_FunP[3].SetText("Ng\251n qu\374");
	ms_pSelf->m_FunPBg[3].Init(&Ini, "Fun_TxtTongMoney2");
	ms_pSelf->m_FunP[4].Init(&Ini, "Fun_TxtTongMoney2");
	ms_pSelf->m_FunPBg[4].Init(&Ini, "Fun_TitleBuildFund2");
	ms_pSelf->m_FunP[5].Init(&Ini, "Fun_TitleBuildFund2");
	ms_pSelf->m_FunP[5].SetText("Ng\251n s\270ch ki\325n thi\325t");
	ms_pSelf->m_FunPBg[5].Init(&Ini, "Fun_TxtBuildFund2");
	ms_pSelf->m_FunP[6].Init(&Ini, "Fun_TxtBuildFund2");
	ms_pSelf->m_FunMask.Init(&Ini, "Fun_ImgSubPageMask");
	ms_pSelf->m_FunMask.Enable(false);	// chi la anh nen, khong nuot chuot
	for (i = 0; i < 7; i++)
	{
		sprintf(szSec, "MDet%d", i);
		ms_pSelf->m_MDet[i].Init(&Ini, szSec);
		// KHONG duoc an chuot cua nut chon dong nam DUOI: m_MDet/m_RowDim
		// AddChild SAU m_BtnRowSel nen nam TREN, va KWndText tra 1 cho
		// WM_LBUTTONDOWN nen nuot han cu bam (78% dien tich moi dong).
		ms_pSelf->m_MDet[i].Enable(false);
	}
	for (i = 0; i < TJX2_UI_ROWS; i++)
	{
		sprintf(szSec, "RowDim%d", i);
		ms_pSelf->m_RowDim[i].Init(&Ini, szSec);
		ms_pSelf->m_RowDim[i].Enable(false);	// xem ghi chu o m_MDet
		sprintf(szSec, "Row%d", i);
		ms_pSelf->m_MList[i].Init(&Ini, szSec);	// muon font/mau sang cua Row
		ms_pSelf->m_MList[i].SetPosition(341, 68 + i * 24);
	}
	for (i = 0; i < TJX2_FUN_BTNS; i++)
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
	for (i = 0; i < 3; i++)
	{
		sprintf(szSec, "Ws_SubPage%d", i);
		ms_pSelf->m_WsSub[i].Init(&Ini, szSec);
		ms_pSelf->m_WsSub[i].SetLabel("");
	}
	ms_pSelf->m_WsSub[0].CheckButton(1);	// JX1 chi co 7 khu = vua mot trang luoi
	ms_pSelf->m_WsSub[1].Enable(false);
	ms_pSelf->m_WsSub[2].Enable(false);
	ms_pSelf->m_WsSel.Init(&Ini, "Ws_TitleServiceFee");	// muon font/mau
	// dat DUOI buc tranh nen (tranh 28..326 x 76..302), khong dam vao panel
	// danh sach thanh vien ben phai (bat dau x=341)
	ms_pSelf->m_WsSel.SetPosition(28, 310);
	ms_pSelf->m_WsSel.SetSize(298, 16);
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
	// [BtnClose] (430,442,126x18) de len [Bot_Close] (478,437) va [Bot_Other]
	// (320,437) - hai nut cung lam mot viec. Giu cum 4 nut day theo anh mau.
	ms_pSelf->m_BtnClose.Hide();
	ms_pSelf->m_BtnClose.Enable(false);
	// trang 2x2 Xem tin Bang khac (section Ozm_* + PageBg5 sinh tu blueprint)
	ms_pSelf->m_ZmBg.Init(&Ini, "PageBg5");
	ms_pSelf->m_ZmBg.Enable(false);	// nen dac (Trans=0) AddChild cuoi - khong duoc nuot chuot
	{
		char szSec[28];
		int z;
		for (z = 0; z < 4; z++)
		{
			sprintf(szSec, "Ozm_Name%d", z + 1);
			ms_pSelf->m_ZmName[z].Init(&Ini, szSec);
			ms_pSelf->m_ZmName[z].Enable(false);
			ms_pSelf->m_ZmName[z].SetText("");	// xoa chu mau design-time (KWndText::Init doc Text=)
			sprintf(szSec, "Ozm_Info%d", z + 1);
			ms_pSelf->m_ZmInfo[z].Init(&Ini, szSec);
			ms_pSelf->m_ZmInfo[z].Enable(false);
			ms_pSelf->m_ZmInfo[z].SetText("");
			sprintf(szSec, "Ozm_JiYu%d", z + 1);
			ms_pSelf->m_ZmJiyu[z].Init(&Ini, szSec);
			ms_pSelf->m_ZmJiyu[z].Enable(false);
			ms_pSelf->m_ZmJiyu[z].SetText("");
			sprintf(szSec, "Ozm_QingXiangTitle%d", z + 1);
			ms_pSelf->m_ZmQxT[z].Init(&Ini, szSec);
			ms_pSelf->m_ZmQxT[z].Enable(false);
			ms_pSelf->m_ZmQxT[z].SetText("Khuynh h\255\355ng bang h\351i");
			sprintf(szSec, "Ozm_QingXiang%d", z + 1);
			ms_pSelf->m_ZmQx[z].Init(&Ini, szSec);
			ms_pSelf->m_ZmQx[z].Enable(false);
			ms_pSelf->m_ZmQx[z].SetText("");
			sprintf(szSec, "Ozm_HuoDongTitle%d", z + 1);
			ms_pSelf->m_ZmHdT[z].Init(&Ini, szSec);
			ms_pSelf->m_ZmHdT[z].Enable(false);
			ms_pSelf->m_ZmHdT[z].SetText(z == 0 ? "Ho\271t \256\351ng bang h\351i"
				: "Ho\271t \256\351ng bang h\351i:");
			sprintf(szSec, "Ozm_HuoDong%d", z + 1);
			ms_pSelf->m_ZmHd[z].Init(&Ini, szSec);
			ms_pSelf->m_ZmHd[z].Enable(false);
			ms_pSelf->m_ZmHd[z].SetText("");
			sprintf(szSec, "Ozm_LookBtn%d", z + 1);
			ms_pSelf->m_ZmLook[z].Init(&Ini, szSec);
			sprintf(szSec, "Ozm_ApplyBtn%d", z + 1);
			ms_pSelf->m_ZmApply[z].Init(&Ini, szSec);
		}
	}
	ms_pSelf->m_ZmPrev.Init(&Ini, "Ozm_LastPage");
	ms_pSelf->m_ZmNext.Init(&Ini, "Ozm_NextPage");
}

//////////////////////////////////////////////////////////////////////

void KUiTongJX2::RequestPage(int nPage, int nStart)
{
	sTJX2Log("[REQ] trangUI=%d start=%d", nPage, nStart);
	// nPage bi gan lai ben duoi thanh ma trang SERVER, nen moi so sanh theo ma
	// trang UI phai dung ban sao nay - truoc day nhanh RECRUIT/RECORD nam sau
	// phep gan nen KHONG BAO GIO chay (danh sach thanh vien khong duoc xin).
	const int nUiPage = nPage;
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
	else if (nPage == TJX2_UI_PAGE_OTHERZM)
		nPage = defTONG_JX2_PAGE_OTHERZM;
	else if (nPage == defTONG_JX2_PAGE_WS)
	{
		// phuong tho can them danh sach thanh vien cho panel phai
		if (g_pCoreShell)
			g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW, defTONG_JX2_PAGE_MEMBER, 0);
	}
	if (nUiPage == defTONG_JX2_PAGE_INFO)
	{
		// Trang Tin tuc co panel danh sach thanh vien ben phai.
		// Trang Chieu mo va Nhat ky thi KHONG (ban goc khong co MemberList).
		if (g_pCoreShell)
			g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW, defTONG_JX2_PAGE_MEMBER, 0);
	}
	if (g_pCoreShell)
		g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW, (unsigned int)nPage, nStart);
}

// Ma nhan dien hop xac nhan cua rieng cua so nay (tra ve qua WND_M_OTHER_WORK_RESULT)
#define TJX2_CONFIRM_ID		0x5701
// hop nhap SO TIEN/DIEM cho 5 thao tac ngan quy (ban goc nguoi choi tu go so)
#define TJX2_AMOUNT_ID		0x5703
// hop nhap TEN (lien minh / bang) - gui nguyen chuoi qua szText
#define TJX2_UNAME_ID		0x5704

// Nho lai thao tac roi mo hop xac nhan. Chuoi nhac lay THANG tu ini (ban thiet ke
// goc da chep san day du, truoc day khong dong ma nao doc toi).
void KUiTongJX2::AskThenSendOp(const char* pszSection, const char* pszKey,
	int nOp, unsigned long dwTarget, int nP1, int nP2)
{
	char szMsg[256];
	szMsg[0] = 0;
	{
		KIniFile Ini;
		char szScheme[256], szPath[300];
		// GetCurSchemePath tra ve THU MUC - phai noi them ten file y het
		// LoadScheme lam, neu khong Ini.Load luon that bai va moi hop xac nhan
		// deu roi ve chuoi mac dinh.
		g_UiBase.GetCurSchemePath(szScheme, sizeof(szScheme));
		sprintf(szPath, "%s\\%s", szScheme, TONG_JX2_INI);
		if (Ini.Load(szPath))
			Ini.GetString(pszSection, pszKey, "", szMsg, sizeof(szMsg));
	}
	if (!szMsg[0])
		strcpy(szMsg, "C\343 ch\276c th\371c hi\326n thao t\270c n\265y kh\253ng?");
	m_nPendOp = nOp;
	m_dwPendTarget = dwTarget;
	m_nPendP1 = nP1;
	m_nPendP2 = nP2;
	UIMessageBox(szMsg, this, "X\270c nh\313n", "Hu\373 b\341", TJX2_CONFIRM_ID);
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
	// Xin lai trang sau khi thao tac - NHUNG BO QUA voi cac lenh mang CHUOI.
	// GameServer tra loi goi VIEW tu ban sao cuc bo, ma ban sao chi doi khi
	// relay phat echo ve; xin lai ngay lap tuc thi chac chan van nhan chuoi CU
	// va ghi de len o soan thao => nguoi choi tuong "khong luu duoc", phai dong
	// cua so mo lai moi thay. Ba lenh nay se tu hien khi doi tab / bam lam moi.
	if (nOp != defTONG_JX2_COP_SAVE_RECRUIT &&
		nOp != defTONG_JX2_COP_SETANN &&
		nOp != defTONG_JX2_COP_LEAVE_WORD)
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
			// trang Chieu mo KHONG hien danh sach thanh vien -> bo qua
			if (ms_pSelf->m_nPage == TJX2_UI_PAGE_RECRUIT)
				break;
			if (ms_pSelf->m_nPage != TJX2_UI_PAGE_TONGLIST)
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
	case defTONG_JX2_PAGE_OTHERZM:
		if (nLen <= (int)sizeof(ms_pSelf->m_byZM))
		{
			memcpy(ms_pSelf->m_byZM, pData, nLen);
			ms_pSelf->m_bHasZM = 1;
			if (ms_pSelf->m_nPage == TJX2_UI_PAGE_OTHERZM)
				ms_pSelf->RenderOtherZM();
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
		m_Row[i].SetText("");
	ClearMemberRows();
}

// Chi xoa panel danh sach ben PHAI (m_MList / m_RowDim / m_MDet), khong cham
// m_Row cua noi dung trang dang ve. RenderMembers phai dung ham nay thay cho
// ClearRows: truoc day RenderRecruit ghi danh sach don xin vao m_Row[0..4] roi
// goi RenderMembers, ma RenderMembers mo dau bang ClearRows nen xoa trang lai
// dung nhung dong vua ghi -> "xin vao bang khong hien de duyet".
void KUiTongJX2::ClearMemberRows()
{
	for (int i = 0; i < TJX2_UI_ROWS; i++)
	{
		m_RowDim[i].SetText("");
		m_MList[i].SetText("");
	}
	for (int d = 0; d < 7; d++)
		m_MDet[d].SetText("");
}

// bo cuc dong: INFO/WS/ANN o vung noi dung trai; MEMBER/RIGHT nam panel danh sach phai
void KUiTongJX2::RepositionRows()
{
	int i;
	// LUU Y: cac nhanh doi ca VI TRI lan KICH THUOC, ma LoadScheme chi chay
	// dung mot lan -> dau moi vong lap phai TRA VE MAC DINH cho ca 4 mang
	// truoc khi nhanh hien tai ghi de. Thieu buoc do la ghe mot tab mot lan
	// la cac tab khac ket bo cuc cua tab do cho toi khi khoi dong lai Game.exe.
	BOOL bList = (m_nPage == defTONG_JX2_PAGE_MEMBER || m_nPage == defTONG_JX2_PAGE_RIGHT ||
		m_nPage == TJX2_UI_PAGE_FUNUSE);
	for (i = 0; i < TJX2_UI_ROWS; i++)
	{
		// Mac dinh (bo cuc danh sach thanh vien) cho CA 4 mang, MOI vong:
		// m_MList 540x20 theo [Row0], m_RowDim 225x16 theo [RowDim0],
		// m_Row/m_BtnRowSel 230x20 tai (341,68+i*24). Nhanh nao can bo cuc
		// khac thi ghi de ben duoi - khong nhanh nao duoc phep de mang o
		// trang thai rieng cua minh sau khi roi trang.
		m_Row[i].SetPosition(341, 68 + i * 24);
		m_Row[i].SetSize(230, 20);
		m_MList[i].SetPosition(341, 68 + i * 24);
		m_MList[i].SetSize(540, 20);
		m_RowDim[i].SetPosition(341, 68 + i * 24);
		m_RowDim[i].SetSize(225, 16);
		m_BtnRowSel[i].SetPosition(341, 68 + i * 24);
		m_BtnRowSel[i].SetSize(230, 20);
		// Mau chu cung la trang thai bi tung trang nhuom (RenderTongList doi
		// m_Row sang xanh...) va khong tu tra lai - reset ve mau ini goc:
		// [Row0] 255,253,122; [RowDim0] 120,120,120. Ham render nao can mau
		// khac se tu SetTextColor truoc khi SetText nhu van lam.
		m_Row[i].SetTextColor(0xFF000000 | (255 << 16) | (253 << 8) | 122);
		m_MList[i].SetTextColor(0xFF000000 | (255 << 16) | (253 << 8) | 122);
		m_RowDim[i].SetTextColor(0xFF000000 | (120 << 16) | (120 << 8) | 120);
		if (bList)
		{
			m_Row[i].SetPosition(341, 68 + i * 24);
			m_Row[i].SetSize(230, 20);
			m_RowDim[i].SetPosition(341, 68 + i * 24);
			m_BtnRowSel[i].SetPosition(341, 68 + i * 24);
			m_BtnRowSel[i].SetSize(230, 20);
			m_BtnRowSel[i].Enable(true);
		}
		else if (m_nPage == defTONG_JX2_PAGE_INFO)
		{
			// Trang Tin tuc: noi dung trai dung cac o Info_* rieng, m_Row khong
			// dung toi; chi can panel danh sach phai bam chon duoc.
			m_RowDim[i].SetPosition(341, 68 + i * 24);
			m_BtnRowSel[i].SetPosition(341, 68 + i * 24);
			m_BtnRowSel[i].SetSize(230, 20);
			m_BtnRowSel[i].Enable(true);
		}
		else if (m_nPage == 4)
		{
			// vung RecordList cua blueprint (5,25 doi +18/+52), 14 dong buoc 22
			m_Row[i].SetPosition(25, 79 + i * 22);
			m_Row[i].SetSize(540, 20);
			m_BtnRowSel[i].SetPosition(25, 79 + i * 22);
			m_BtnRowSel[i].SetSize(230, 20);
			m_BtnRowSel[i].Enable(false);
		}
		else if (m_nPage == TJX2_UI_PAGE_TONGLIST)
		{
			// ba cot that: ten bang | bang chu | cap / thanh vien
			m_Row[i].SetPosition(40, 70 + i * 24);
			m_Row[i].SetSize(160, 20);
			m_MList[i].SetPosition(205, 70 + i * 24);
			m_MList[i].SetSize(150, 20);
			m_RowDim[i].SetPosition(365, 70 + i * 24);
			m_RowDim[i].SetSize(140, 20);
			m_BtnRowSel[i].SetPosition(40, 70 + i * 24);
			m_BtnRowSel[i].SetSize(470, 20);
			m_BtnRowSel[i].Enable(i >= 1 && i <= 10);
		}
		else if (m_nPage == TJX2_UI_PAGE_RECRUIT)
		{
			// Dat DUNG vung [Rec_ApplyerList] cua ban thiet ke goc:
			// (308,92) rong 212 cao 400 -> 8 dong buoc 19px, con cach cum nut
			// [Rec_AcceptApply] (Top=397) mot khoang an toan.
			m_Row[i].SetPosition(310, 94 + i * 19);
			m_Row[i].SetSize(210, 16);
			m_BtnRowSel[i].SetPosition(308, 92 + i * 19);
			m_BtnRowSel[i].SetSize(212, 19);
			m_BtnRowSel[i].Enable(i < 8);
		}
		else if (m_nPage == defTONG_JX2_PAGE_WS)
		{
			// phuong tho: chon KHU bang icon, con panel phai la danh sach thanh
			// vien binh thuong (ban thiet ke goc trang nay CO MemberList) - bam
			// dong = chon thanh vien, an toan vi khu dang chon da co bien rieng
			m_Row[i].SetPosition(341, 68 + i * 24);
			m_Row[i].SetSize(230, 20);
			m_RowDim[i].SetPosition(341, 68 + i * 24);
			m_BtnRowSel[i].SetPosition(341, 68 + i * 24);
			m_BtnRowSel[i].SetSize(230, 20);
			m_BtnRowSel[i].Enable(true);
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
	m_nRecStart = 0;
	m_nSel = 0;
	m_nSelWs = 1;	// khu tac phuong mac dinh (bien RIENG, khong dung chung m_nSel)
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
		if (nPage == TJX2_UI_PAGE_OTHERZM)
			nBg = -1;	// trang 2x2 dung nen rieng m_ZmBg
		for (int i = 0; i < TJX2_UI_TABS; i++)
		{
			if (i == nBg)
				m_PageBg[i].Show();
			else
				m_PageBg[i].Hide();
		}
		if (nPage == TJX2_UI_PAGE_OTHERZM)
			m_ZmBg.Show();
		else
			m_ZmBg.Hide();
	}
	// cum control trang 2x2 Xem tin Bang khac
	{
		BOOL bZm = (nPage == TJX2_UI_PAGE_OTHERZM);
		for (int i = 0; i < 4; i++)
		{
			if (bZm)
			{
				m_ZmName[i].Show(); m_ZmInfo[i].Show(); m_ZmJiyu[i].Show();
				m_ZmQxT[i].Show(); m_ZmQx[i].Show();
				m_ZmHdT[i].Show(); m_ZmHd[i].Show();
				m_ZmLook[i].Show(); m_ZmLook[i].Enable(true);
				m_ZmApply[i].Show(); m_ZmApply[i].Enable(true);
			}
			else
			{
				m_ZmName[i].Hide(); m_ZmInfo[i].Hide(); m_ZmJiyu[i].Hide();
				m_ZmQxT[i].Hide(); m_ZmQx[i].Hide();
				m_ZmHdT[i].Hide(); m_ZmHd[i].Hide();
				m_ZmLook[i].Hide(); m_ZmLook[i].Enable(false);
				m_ZmApply[i].Hide(); m_ZmApply[i].Enable(false);
			}
		}
		if (bZm) { m_ZmPrev.Show(); m_ZmPrev.Enable(true); m_ZmNext.Show(); m_ZmNext.Enable(true); }
		else { m_ZmPrev.Hide(); m_ZmPrev.Enable(false); m_ZmNext.Hide(); m_ZmNext.Enable(false); }
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
		for (i = 0; i < 7; i++)
			if (bFun) m_FunP[i].Show(); else m_FunP[i].Hide();
		if (bBtn) m_FunMask.Show(); else m_FunMask.Hide();
		for (i = 0; i < 6; i++)
			if (bFun) m_FunPBg[i].Show(); else m_FunPBg[i].Hide();
		// nhom nut giua theo sub-page (cac nut hanh dong deu thuoc sub 1 tru map)
		// nhom 2 = lanh dia + doi phe (dung nhom cua ban goc: BtnChangeCamp
		// nam cung o Top=176 voi BtnCreateTongMap/BtnConfigureTongMap)
		static const int s_nFunBtnSub[TJX2_FUN_BTNS] =
			{ 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0, 0, 0,
			  3, 3, 3, 3, 3, 4, 4 };
			// 0 = luon hien khi mode nut (khoi tien/ca nhan/roi bang)
			// 1 = sub nhan su (Recruit/KickOut/Depose/DispenseOffer)
			// 2 = sub lanh dia (CreateTongMap/ConfigureTongMap/TongStunt)
		for (i = 0; i < TJX2_FUN_BTNS; i++)
		{
			BOOL bShow = bBtn && s_sFunBtn[i].nAct >= 0 &&
				(s_nFunBtnSub[i] == 0 || s_nFunBtnSub[i] == m_nFunSub);
			if (bShow && s_sFunBtn[i].nAct >= 14 && s_sFunBtn[i].nAct <= 18 && m_bHasInfo)
			{
				// nut lien minh an/hien theo trang thai (blueprint dung chung o):
				// chua vao: Lap + Xin vao; thanh vien: Huy(roi); minh chu:
				// Duyet don + Truc xuat + Huy(giai tan)
				TONG_JX2_INFO_SYNC* pIU = (TONG_JX2_INFO_SYNC*)m_byInfo;
				BOOL bIn = (pIU->m_dwUnionID != 0);
				BOOL bLead = (pIU->m_bUnionLeader != 0);
				switch (s_sFunBtn[i].nAct)
				{
				case 14: bShow = !bIn; break;
				case 15: bShow = !bIn; break;
				case 16: bShow = bIn && bLead; break;
				case 17: bShow = bIn; break;
				case 18: bShow = bIn && bLead; break;
				}
			}
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
		// TJX2_UI_PAGE_RECRUIT KHONG nam trong danh sach: ban thiet ke goc cua
		// trang Chieu mo khong co MemberList/TxtRank/TxtTitle/TxtType/
		// BtnPrevPage/BtnNextPage/BtnJump/BtnOnlinePriority - panel phai cua
		// trang do danh RIENG cho danh sach don xin ([ApplyerList]).
		BOOL bL = (nPage == defTONG_JX2_PAGE_MEMBER || nPage == defTONG_JX2_PAGE_RIGHT ||
			nPage == TJX2_UI_PAGE_FUNUSE || nPage == defTONG_JX2_PAGE_WS ||
			nPage == defTONG_JX2_PAGE_INFO);
		// trang Danh sach bang muon lai hai lop nay lam COT 2 va COT 3
		BOOL bCol = (bL || nPage == TJX2_UI_PAGE_TONGLIST);
		for (i = 0; i < TJX2_UI_ROWS; i++)
			if (bCol) m_MList[i].Show(); else m_MList[i].Hide();
		for (i = 0; i < TJX2_UI_ROWS; i++)
			if (bCol) m_RowDim[i].Show(); else m_RowDim[i].Hide();
		for (i = 0; i < 7; i++)
			if (bL) m_MDet[i].Show(); else m_MDet[i].Hide();
		for (i = 0; i < 3; i++)
			if (bL) m_ColHdr[i].Show(); else m_ColHdr[i].Hide();
		// hang dieu khien duoi panel: o kiem tren mang / nut sap xep / chuyen trang
		if (bL) { m_MOnline.Show(); m_MOnline.Enable(true); } else { m_MOnline.Hide(); m_MOnline.Enable(false); }
		if (bL) { m_MSort.Show(); m_MSort.Enable(true); } else { m_MSort.Hide(); m_MSort.Enable(false); }
		if (bL) { m_MJump.Show(); m_MJump.Enable(true); } else { m_MJump.Hide(); m_MJump.Enable(false); }
		if (bL) m_MPage.Show(); else m_MPage.Hide();
		if (bL) { m_MPageEdit.Show(); m_MPageEdit.Enable(true); } else { m_MPageEdit.Hide(); m_MPageEdit.Enable(false); }
		// Nut dong/mo tuyen: ban thiet ke goc KHONG CO (JX2 chan nguoi xin bang
		// hai nguong cap, khong bang cong tac), va no de len nut [Save].
		m_RecToggle.Hide();
		m_RecToggle.Enable(false);
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
			if (bWs) { m_WsIcon[i].Show(); m_WsIcon[i].Enable(true); m_WsIconBg[i].Show(); }
			else { m_WsIcon[i].Hide(); m_WsIcon[i].Enable(false); m_WsIconBg[i].Hide(); }
			// lop cao sang + chu cap do RenderWorkshop bat/tat theo tung khu
			if (!bWs) { m_WsIconHL[i].Hide(); m_WsRank[i].Hide(); }
		}
		for (i = 0; i < 3; i++)
		{
			if (bWs) m_WsSub[i].Show(); else m_WsSub[i].Hide();
		}
		if (bWs) m_WsIconSel.Show(); else m_WsIconSel.Hide();
		if (bWs) m_WsArt.Show(); else m_WsArt.Hide();
		// KHONG doi nhan nut day: sprite goc da nung chu san
	}
	// bo control trang Nhat ky
	{
		int i;
		BOOL bRc = (nPage == 4);
		for (i = 0; i < 4; i++)
		{
			if (bRc) { m_RcSub[i].Show(); m_RcSub[i].Enable(true); }
			else { m_RcSub[i].Hide(); m_RcSub[i].Enable(false); }
			// Bat sang theo kieu RADIO. Nut co CheckBox=1 nen tu lat RIENG no,
			// khong co logic nhom: luc moi mo KHONG nut nao sang (chu mau
			// 77,77,77 tren nen toi = "chu den" chu game thay), bam qua lai thi
			// NHIEU nut cung sang. Phai dat SAU Enable vi Enable cung doi khung.
		}
		for (i = 0; i < 4; i++)
			m_RcSub[i].CheckButton((bRc && i == m_nRcSub) ? 1 : 0);
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
		for (int i = 0; i < TJX2_INFO_NUM; i++)
		{
			if (nPage == defTONG_JX2_PAGE_INFO)
			{
				m_InfoBg[i].Show();
				m_Info[i].Show();
			}
			else
			{
				m_InfoBg[i].Hide();
				m_Info[i].Hide();
			}
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
	else if (nPage == TJX2_UI_PAGE_OTHERZM)
	{
		m_nZmStart = 0;
		if (m_bHasZM)
			RenderOtherZM();
	}
	RequestPage(nPage, 0);
}

void KUiTongJX2::SetupActions()
{
	int i;
	for (i = 0; i < TJX2_UI_ACTS; i++)
	{
		m_BtnAct[i].SetLabel("");
		m_BtnAct[i].Enable(false);
		m_BtnAct[i].Hide();	// hang nut cu da BO - khong ve khung rong nua
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
		// Do so lieu vao dung o gia tri cua ban thiet ke BASEINFO.
		// LUU Y ma tran doanh: camp_justice = 1, camp_evil = 2, camp_balance = 3
		// (GameDataDef.h) - KHONG phai 0/1/2. Truoc day tra bang bang chi so
		// tho nen bang Chinh phai hien ra "Ta phai".
		static const char* szCamp[4] =
			{"", "Ch\335nh ph\270i", "T\265 ph\270i", "Trung l\313p"};
		m_Info[TJX2_INFO_TONGNAME].SetText(p->m_szTongName);
		m_Info[TJX2_INFO_MASTER].SetText(p->m_szMaster);
		// Lien minh: goi tin chua mang ten lien minh (server co field 10 =
		// UnionID nhung BuildClientView chua doc) -> de TRONG dung anh mau,
		// khong ghi "-" nua.
		m_Info[TJX2_INFO_LEAGUE].SetText(p->m_szUnionName[0] ? p->m_szUnionName : "");
		m_Info[TJX2_INFO_CAMP].SetText(
			(p->m_btCamp >= 1 && p->m_btCamp <= 3) ? szCamp[p->m_btCamp] : "");
		sprintf(sz, "%d", p->m_nTongLevel);
		m_Info[TJX2_INFO_TONGLEVEL].SetText(sz);
		sprintf(sz, "%d", (int)p->m_wMemberTotal);
		m_Info[TJX2_INFO_MEMBERNUM].SetText(sz);
		sprintf(sz, "%d", p->m_nLevel);		// field 13 = cap KIEN THIET
		m_Info[TJX2_INFO_BUILDLEVEL].SetText(sz);
		sprintf(sz, "%.0f", (double)p->m_nMoney);
		m_Info[TJX2_INFO_CAPITAL].SetText(sz);
		// chi MOT so: o rong 100px, them "(tuan x/y)" la tran ra ngoai khung
		sprintf(sz, "%u", p->m_dwBuildFund);
		m_Info[TJX2_INFO_BUILDFUND].SetText(sz);
		sprintf(sz, "%u", p->m_dwStoredOffer);
		m_Info[TJX2_INFO_TOTALOFFER].SetText(sz);
		sprintf(sz, "%u", p->m_dwMyOffer);
		m_Info[TJX2_INFO_MYOFFER].SetText(sz);
		sprintf(sz, "%u", p->m_dwMyWeekOffer);
		m_Info[TJX2_INFO_WEEKOFFER].SetText(sz);
		if (m_bHasMember)
			RenderMembers();	// panel danh sach thanh vien ben phai
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

// nOffset = so dong dau panel phai da bi trang khac chiem (trang Chieu mo
// giu 5 dong dau cho danh sach don xin vao bang)
void KUiTongJX2::RenderMembers(int nOffset)
{
	if (!m_bHasMember)
		return;
	TONG_JX2_MEMBER_SYNC* p = (TONG_JX2_MEMBER_SYNC*)m_byMember;
	char sz[120];
	ClearMemberRows();
	// KHONG ve dong tieu de o day: ba o tieu de cot da la control rieng
	// (m_ColHdr, section TxtRank/TxtTitle/TxtType tai Top=53). Dong tu sinh
	// truoc day nam o y=68 ngay duoi chung = HAI dong tieu de chong nhau.
	int i;
	// Thu tu hien thi: mac dinh giu nguyen thu tu may chu gui, roi ap o kiem
	// "Hien thi tren mang" (dua nguoi online len truoc) va kieu sap xep dang
	// chon o nut menu. m_nOrd[dong hien thi] = chi so thanh vien trong goi;
	// m_nSel van la CHI SO THANH VIEN (khong phai dong) de moi cho khac dung
	// chung khong phai doi.
	int nCnt = (int)p->m_btCount;
	if (nCnt > defTONG_JX2_VIEW_MEMBERS)
		nCnt = defTONG_JX2_VIEW_MEMBERS;
	for (i = 0; i < nCnt; i++)
		m_nOrd[i] = i;
	{
		int a, b;
		for (a = 1; a < nCnt; a++)		// chen truc tiep - giu on dinh
		{
			int nKey = m_nOrd[a];
			for (b = a - 1; b >= 0; b--)
			{
				TONG_JX2_ONE_MEMBER* p1 = &p->m_sMember[m_nOrd[b]];
				TONG_JX2_ONE_MEMBER* p2 = &p->m_sMember[nKey];
				int nCmp = 0;
				if (m_bOnlineFirst && p1->m_btOnline != p2->m_btOnline)
					nCmp = p2->m_btOnline ? 1 : -1;
				else
				{
					switch (m_nSortMode)
					{
					case 0: nCmp = (int)p2->m_btLevel - (int)p1->m_btLevel; break;
					case 1:
					case 2:
					case 3: nCmp = (int)p2->m_dwWeekOffer - (int)p1->m_dwWeekOffer; break;
					case 4: nCmp = (p2->m_btFigure == 4 ? 1 : 0) - (p1->m_btFigure == 4 ? 1 : 0); break;	// Item_4 = An sy
					case 5: nCmp = (int)p1->m_btFigure - (int)p2->m_btFigure; break;
					case 6: nCmp = (int)p2->m_dwLastActive - (int)p1->m_dwLastActive; break;
					default: nCmp = 0; break;
					}
				}
				if (nCmp <= 0)
					break;
				m_nOrd[b + 1] = m_nOrd[b];
			}
			m_nOrd[b + 1] = nKey;
		}
	}
	for (i = 0; i < nCnt && i + nOffset < TJX2_UI_ROWS; i++)
	{
		TONG_JX2_ONE_MEMBER* pM = &p->m_sMember[m_nOrd[i]];
		if (m_nPage == defTONG_JX2_PAGE_RIGHT)
		{
			// liet ke quyen dang co theo mat na
			char szR[64];
			szR[0] = 0;
			int nR = 0;
			for (int b = 0; b < defTONG_JX2_RIGHT_COUNT && nR < 4; b++)
			{
				if (pM->m_wRights & (1 << b))
				{
					char szOne[12];
					sprintf(szOne, "%u ", s_dwRightId[b]);
					strcat(szR, szOne);
					nR++;
				}
			}
			sprintf(sz, "%s%-16s %-10s quyen: %s", (m_nOrd[i] == m_nSel) ? "> " : "  ",
				pM->m_szName, pM->m_btFigure < 5 ? s_szFigure[pM->m_btFigure] : "?",
				szR[0] ? szR : "(khong)");
		}
		else
		{
			// Cot 3 cua ban goc la "loai hinh" (TxtType): GIA TRI cua tieu
			// chi sap xep dang chon, khong phai chuc vu co dinh - menu sort
			// bi dong bang thu tu (chu thich cwm trong blueprint) chinh vi
			// cot nay doi theo no.
			char szV[40];
			switch (m_nSortMode)
			{
			case 1:
				{
					long nDays = 1;
					if (pM->m_dwJoinTime)
						nDays = (long)((time(NULL) - (time_t)pM->m_dwJoinTime) / 86400) + 1;
					if (nDays < 1)
						nDays = 1;
					sprintf(szV, "%.1f", (double)pM->m_dwOffer / (double)nDays);
				}
				break;
			case 2:
			case 3: sprintf(szV, "%u", pM->m_dwWeekOffer); break;
			case 4: strcpy(szV, pM->m_btFigure == 4 ? "\310n s\374" : "-"); break;
			case 5: strcpy(szV, pM->m_btFigure < 5 ? s_szFigure[pM->m_btFigure] : "?"); break;
			case 6:
				if (pM->m_dwLastActive)
				{
					time_t nTA = (time_t)pM->m_dwLastActive;
					struct tm* pTA = localtime(&nTA);
					sprintf(szV, "%02d-%02d %02d:%02d", pTA->tm_mday, pTA->tm_mon + 1,
						pTA->tm_hour, pTA->tm_min);
				}
				else
					strcpy(szV, "-");
				break;
			default: sprintf(szV, "%d", (int)pM->m_btLevel); break;
			}
			sprintf(sz, "%s%2d  %-16s %-12s", (m_nOrd[i] == m_nSel) ? ">" : " ",
				(int)p->m_wStart + i + 1, pM->m_szName, szV);
		}

		// MAU dung bang mau cua ban thiet ke goc ([Fun_MemberList]):
		//   OnlineColor      236,238,111   (vang nhat)
		//   OfflineColor       0,134,132   (xanh tham)
		//   Online/OfflineSelColor 34,228,36 (xanh sang - dong dang chon)
		// Truoc day ta dung 255,253,122 cho online va 120,120,120 (xam) cho
		// offline nen khac han anh mau.
		{
			BOOL bSel = (m_nOrd[i] == m_nSel);
			unsigned int uCol;
			if (bSel)
				uCol = 0xFF000000 | (34 << 16) | (228 << 8) | 36;
			else if (pM->m_btOnline)
				uCol = 0xFF000000 | (236 << 16) | (238 << 8) | 111;
			else
				uCol = 0xFF000000 | (0 << 16) | (134 << 8) | 132;
			// ve tren MOT lop duy nhat (m_MList) va doi mau luc chay - lop
			// m_RowDim chi con de trong
			m_MList[i + nOffset].SetTextColor(uCol);
			m_MList[i + nOffset].SetText(sz);
			m_RowDim[i + nOffset].SetText("");
		}
	}
	// panel XANH chi tiet nguoi dang chon (nhu ban Linux, hien o moi trang co danh sach)
	m_nMDetRows = 0;
	if (m_bMDet && m_nSel < (int)p->m_btCount)
	{
		// Ban goc: panel bung ra NGAY DUOI dong vua kich (khong phai o vi tri
		// co dinh). m_nSel la chi so THANH VIEN, phai doi nguoc ra DONG hien
		// thi qua m_nOrd (cung bang dung o vong ve ben tren).
		int nRow = 0;
		{
			int q;
			for (q = 0; q < nCnt; q++)
				if (m_nOrd[q] == m_nSel)
				{
					nRow = q + nOffset;
					break;
				}
		}
		// 5 dong chi tiet nam ngay duoi dong do; neu cham day panel thi day
		// len tren de khong tran ra ngoai khung danh sach (14 dong)
		int nFirst = nRow + 1;
		if (nFirst + 5 > TJX2_UI_ROWS)
			nFirst = TJX2_UI_ROWS - 5;
		if (nFirst < 1)
			nFirst = 1;
		for (int nCl = nFirst; nCl < nFirst + 5 && nCl < TJX2_UI_ROWS; nCl++)
		{
			m_MList[nCl].SetText("");
			m_RowDim[nCl].SetText("");
		}
		{
			// dat 5 o chu vao dung cho + ghi lai de PaintWindow ve nen
			int k;
			for (k = 0; k < 5; k++)
				m_MDet[k].SetPosition(341, 68 + (nFirst + k) * 24);
			m_nMDetTop = 68 + nFirst * 24;
			m_nMDetRows = 5;
		}
		TONG_JX2_ONE_MEMBER* pSel = &p->m_sMember[m_nSel];
		char szT[64];
		// Panel = DUNG 5 truong cua ban goc, chuoi nguyen van khoi
		// stringtable_client.txt:475-479 (G_STR_NAME / G_STR_TITLE /
		// G_STR_CURRENT_LEVEL / G_STR_CURRENT_OFFER / G_STR_JION_TIME).
		// "Danh hieu"/"danh hieu" la cach ban dia hoa goc dich 2 nhan
		// khac nhau (zh: Ho ten / Xung hao) - giu nguyen van.
		sprintf(szT, "Danh hi\326u  %s", pSel->m_szName);
		m_MDet[0].SetText(szT);
		sprintf(szT, "danh hi\326u  %s",
			pSel->m_btFigure < 5 ? s_szFigure[pSel->m_btFigure] : "?");
		m_MDet[1].SetText(szT);
		sprintf(szT, "\247\274ng c\312p hi\326n t\271i  %d", (int)pSel->m_btLevel);
		m_MDet[2].SetText(szT);
		sprintf(szT, "\247i\323m c\350ng hi\325n hi\326n t\271i  %u", pSel->m_dwOffer);
		m_MDet[3].SetText(szT);
		struct tm* pTm;
		time_t nT = (time_t)pSel->m_dwJoinTime;
		if (nT)
		{
			pTm = localtime(&nT);
			sprintf(szT, "Th\352i gian nh\313p bang  %02d-%02d-%04d %02d:%02d",
				pTm->tm_mday, pTm->tm_mon + 1, pTm->tm_year + 1900,
				pTm->tm_hour, pTm->tm_min);
		}
		else
			strcpy(szT, "Th\352i gian nh\313p bang  -");
		m_MDet[4].SetText(szT);
		m_MDet[5].SetText("");
		m_MDet[6].SetText("");
	}
	// so trang dang xem (o [Fun_TitlePage] canh o nhap "Chuyen den")
	sprintf(sz, "%d", m_nStart / defTONG_JX2_VIEW_MEMBERS + 1);
	m_MPage.SetText(sz);
	LoadChecksFromSel();
}

// Ve NEN panel chi tiet thanh vien roi moi de control con ve chu len tren.
// Ban goc khong dung sprite (quet pak khong co anh nen nao cho panel) ma
// dung mau nen cua chinh danh sach: [MemberList] SelBgColor=110,110,90 +
// SelBgColorAlpha=100 (bon trang blueprint chep y het nhau). Quy uoc dong
// goi alpha lay tu KWndMessageListBox: (255 - alpha) << 21.
void KUiTongJX2::PaintWindow()
{
	KWndImage::PaintWindow();
	if (!m_bMDet || m_nMDetRows <= 0 || !g_pRepresentShell)
		return;
	KRUShadow sBg;
	sBg.oPosition.nX = m_nAbsoluteLeft + 341;
	sBg.oPosition.nY = m_nAbsoluteTop + m_nMDetTop;
	sBg.oEndPos.nX = sBg.oPosition.nX + 225;
	sBg.oEndPos.nY = sBg.oPosition.nY + m_nMDetRows * 24;
	sBg.Color.Color_dw = ((110 << 16) | (110 << 8) | 90) |
		(((unsigned int)(255 - 100) << 21) & 0xff000000);
	g_pRepresentShell->DrawPrimitives(1, &sBg, RU_T_SHADOW, true);
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
		// Lop CAO SANG + chu CAP cho TUNG khu (ban goc: nen icon luon ve, anh
		// cao sang chi ve khi khu DA LAP; duoi moi icon la mot dong chu cap).
		// Truoc day ca 7 o deu hien y het nhau nen khong phan biet duoc khu nao
		// da lap, khu nao chua.
		for (t = 1; t <= 7; t++)
		{
			// ICON THAT cua tung khu, doi theo trang thai dung nhu ban goc:
			// chua lap -> anh mo; da lap va mo -> anh sang; da lap ma dong ->
			// anh xam. Truoc day ca 7 o deu ve mot anh chung nen "thieu icon".
			int nSt = !pWs->m_sWs[t].btExist ? 2 : (pWs->m_sWs[t].btOpen ? 0 : 1);
			m_WsIcon[t].SetImage(ISI_T_SPR, s_szWsIcon[t][nSt]);
			if (pWs->m_sWs[t].btExist)
			{
				m_WsIconHL[t].Show();
				sprintf(szV, "%d", (int)pWs->m_sWs[t].wLevel);
				m_WsRank[t].SetText(szV);
				m_WsRank[t].Show();
			}
			else
			{
				m_WsIconHL[t].Hide();
				m_WsRank[t].SetText("");
				m_WsRank[t].Hide();
			}
		}
		// khung chon dat len icon khu dang chon - lay THANG toa do cua chinh
		// control da Init tu ini, khong khai bao toa do luoi o hai noi nua
		if (m_nSelWs >= 1 && m_nSelWs <= 7)
		{
			int nX = 0, nY = 0;
			m_WsIcon[m_nSelWs].GetPosition(&nX, &nY);
			m_WsIconSel.SetPosition(nX, nY);
		}
	}
	ClearRows();	// trang phuong tho khong dung danh sach chu (giong ban Linux)
	if (m_bHasWs && m_nSelWs >= 1 && m_nSelWs <= 7)
	{
		TONG_JX2_WS_SYNC* pW2 = (TONG_JX2_WS_SYNC*)m_byWs;
		char szD[120];
		if (pW2->m_sWs[m_nSelWs].btExist)
			sprintf(szD, "Khu %d %s: c\312p %d [%s]  s\266n l\255\356ng %u  c\312p d\357ng %u",
				m_nSelWs, s_szWsName[m_nSelWs], (int)pW2->m_sWs[m_nSelWs].wLevel,
				pW2->m_sWs[m_nSelWs].btOpen ? "MO" : "DONG",
				pW2->m_sWs[m_nSelWs].dwOutput, pW2->m_sWs[m_nSelWs].dwUseLevel);
		else
			sprintf(szD, "Khu %d %s: (ch\255a l\313p)", m_nSelWs, s_szWsName[m_nSelWs]);
		m_WsSel.SetText(szD);
	}
	if (m_bHasMember)
		RenderMembers();	// panel danh sach thanh vien ben phai (nhu ban Linux)
}

// nap trang thai 12 o kiem tu mat na quyen cua nguoi dang chon (trang Phan phoi)
void KUiTongJX2::LoadChecksFromSel()
{
	// Tab "Phan phoi chuc nang" chay o ma trang MEMBER (bang s_nTabPage khong
	// co PAGE_RIGHT) nen phai nhan CA HAI ma trang, neu khong thi o kiem khong
	// bao gio nap trang thai va bam "Phan quyen" se THU SACH quyen truong lao.
	if ((m_nPage != defTONG_JX2_PAGE_MEMBER && m_nPage != defTONG_JX2_PAGE_RIGHT) ||
		!m_bHasMember)
		return;
	TONG_JX2_MEMBER_SYNC* pM = (TONG_JX2_MEMBER_SYNC*)m_byMember;
	WORD wMask = 0;
	if (m_nSel < (int)pM->m_btCount)
		wMask = pM->m_sMember[m_nSel].m_wRights;
	for (int b = 0; b < 14; b++)
	{
		int nBit = -1;
		for (int r = 0; r < defTONG_JX2_RIGHT_COUNT; r++)
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
		// Chi TRUONG LAO nhan duoc quyen (bang chu mac dinh toan quyen).
		// Truoc day return IM LANG nen chon bang chu / doi truong / de tu /
		// an sy deu "bam khong thay gi xay ra".
		UIMessageBox("Ch\330 c\343 th\323 ph\251n quy\322n cho Tr\255\353ng L\267o.",
			this, "\247\343ng", NULL, 0);
		return;
	}
	for (int b = 0; b < 14; b++)
	{
		int nBit = -1;
		for (int r = 0; r < defTONG_JX2_RIGHT_COUNT; r++)
			if (s_dwRightId[r] == m_dwRtId[b])
				nBit = r;
		if (nBit < 0)
			continue;	// RightID la trong ini khong nam trong mat na 14 bit
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
	// Trang Chieu mo KHONG hien danh sach thanh vien (ban thiet ke goc
	// khong co MemberList/TxtRank/BtnPrevPage... o trang nay - chu game
	// da xac nhan lai). Panel phai chi danh cho danh sach don xin.
	if (!m_bHasRecruit)
		return;
	TONG_JX2_RECRUIT_SYNC* p = (TONG_JX2_RECRUIT_SYNC*)m_byRecruit;
	m_RecJiyu.SetText(p->m_szRecruit);
	sprintf(sz, "%d", (int)p->m_btAutoLv);
	m_RecAuto.SetText(sz);
	sprintf(sz, "%d", (int)p->m_btRefuseLv);
	m_RecRefuse.SetText(sz);
	m_nRecQX = p->m_btTendency % TJX2_QX_NUM;
	m_RecQX.SetLabel(s_szRecQX[m_nRecQX]);
	for (i = 0; i < 4; i++)
	{
		m_nRecHD[i] = p->m_btAct[i] % TJX2_HD_NUM;
		m_RecHD[i].SetLabel(s_szRecHD[m_nRecHD[i]]);
	}
	{
		// KHONG ve them dong tieu de nao o day: ban thiet ke goc da co san HAI
		// tieu de la [Rec_ApplyTitle] va [Rec_ApplyerName]. Dong "== Don xin
		// vao bang X-Y/Z ==" ta tu them truoc day la tieu de THU BA, de len
		// [Rec_ApplyerName] - dung cho "du thong tin" chu game noi.
		// Server gui CA 8 don trong MOT goi nen ve het, khong can lat trang.
		int nTotal = (int)p->m_btApplyCount;
		int nShow = nTotal;
		m_nRecStart = 0;
		if (nShow > 8)
			nShow = 8;
		if (nShow < 0)
			nShow = 0;
		for (i = 0; i < nShow; i++)
		{
			TONG_JX2_ONE_APPLY* pA = &p->m_sApply[i];
			// cot cap can theo tieu de [Rec_ApplyerName]: ten roi cap
			sprintf(sz, "%s%-22s%d", (i == m_nSel) ? ">" : " ",
				pA->m_szName, (int)pA->m_wLevel);
			m_Row[i].SetText(sz);
		}
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
	// O nay la "Lien minh" (TxtTongUnion) chu KHONG phai ten bang chu - ban
	// thiet ke trang Chuc nang khong co o hien bang chu. Truoc day do nham
	// p->m_szMaster vao day nen tren giao dien hien "Lien minh CaiBang".
	m_FunTxt[4].SetText(p->m_szUnionName[0] ? p->m_szUnionName : "-");
	sprintf(sz, "%d", p->m_nLevel);
	m_FunTxt[6].SetText(sz);	// Dang cap kien thiet
	sprintf(sz, "%u", p->m_dwStoredOffer);
	m_FunTxt[8].SetText(sz);
	sprintf(sz, "%.0f", (double)p->m_nMoney);
	m_FunTxt[10].SetText(sz);
	// O nay rong 98px va nen la THANH TIEN DO - ban goc chi hien MOT SO, muc
	// tuan the hien bang do dai thanh. Nhoi ca "(tuan x/y)" vao day chinh la
	// ly do dong bi cat cut tren anh chu game gui.
	sprintf(sz, "%u", p->m_dwBuildFund);
	m_FunTxt[12].SetText(sz);
	sprintf(sz, "%u", p->m_dwWarFund);
	m_FunTxt[14].SetText(sz);
	sprintf(sz, "%u", p->m_dwMyOffer);
	m_FunP[2].SetText(sz);
	// hai o so cua hai hang moi: dung chinh hai gia tri ma hai hang cung
	// ten o khoi tren dang hien (Ngan quy = m_nMoney, Ngan sach kien thiet
	// = m_dwBuildFund) - goi tin khong co truong tien rieng cua ca nhan
	sprintf(sz, "%.0f", (double)p->m_nMoney);
	m_FunP[4].SetText(sz);
	sprintf(sz, "%u", p->m_dwBuildFund);
	m_FunP[6].SetText(sz);
	if (m_bHasMember)
		RenderMembers();
}

// Trang Nhat ky: 4 muc con - thong bao co khung sua; bang vu/lich su doc ring
void KUiTongJX2::RenderRecord()
{
	int i;
	ClearRows();
	// Trang Nhat ky KHONG co panel danh sach thanh vien: ban thiet ke goc chi co
	// RecordList + 4 nut muc con + khung sua thong bao, khong co MemberList.
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
	// Chia CO T THAT bang ba lop control rieng thay vi dem ky tu: font cua game
	// la font TI LE nen dem "%-18s" khong bao gio thang hang (do la ly do anh
	// chu game gui bi lech). m_Row = ten bang, m_MList = bang chu,
	// m_RowDim = cap + so thanh vien; ca ba deu ranh o trang nay.
	m_Row[0].SetTextColor(0xFF000000 | (255 << 16) | (237 << 8) | 165);
	m_Row[0].SetText("T\252n bang");
	m_MList[0].SetText("Bang ch\361");
	m_RowDim[0].SetText("C\312p / Th\265nh vi\252n");
	if (!m_bHasList)
		return;
	TONG_JX2_TONGLIST_SYNC* p = (TONG_JX2_TONGLIST_SYNC*)m_byList;
	if (p->m_btCount == 0)
	{
		m_Row[2].SetText("(ch\255a c\343 bang h\351i n\265o)");
		return;
	}
	for (i = 0; i < (int)p->m_btCount && i < defTONG_JX2_LIST_ROWS; i++)
	{
		// mau ten bang theo ban goc: 153,255,255 (xanh nhat) - truoc day ve
		// mau vang cua section [Row0]
		BOOL bSel = (i == m_nSel);
		unsigned int uCol = bSel ?
			(0xFF000000 | (34 << 16) | (228 << 8) | 36) :
			(0xFF000000 | (153 << 16) | (255 << 8) | 255);
		m_Row[i + 1].SetTextColor(uCol);
		sprintf(sz, "%s%s", bSel ? "> " : "  ", p->m_sTong[i].m_szName);
		m_Row[i + 1].SetText(sz);
		m_MList[i + 1].SetTextColor(uCol);
		m_MList[i + 1].SetText(p->m_sTong[i].m_szMaster);
		sprintf(sz, "%d  /  %d", (int)p->m_sTong[i].m_btLevel,
			(int)p->m_sTong[i].m_wMember);
		m_RowDim[i + 1].SetTextColor(uCol);
		m_RowDim[i + 1].SetText(sz);
	}
}

//////////////////////////////////////////////////////////////////////

// Trang 2x2 Xem tin Bang khac: do du lieu 4 the theo blueprint
// (Name/Info/JiYu/QingXiang/HuoDong + 2 nut moi the).
void KUiTongJX2::RenderOtherZM()
{
	if (!m_bHasZM)
		return;
	TONG_JX2_OTHERZM_SYNC* p = (TONG_JX2_OTHERZM_SYNC*)m_byZM;
	char sz[200];
	int i;
	for (i = 0; i < 4; i++)
	{
		if (i < (int)p->m_btCount)
		{
			TONG_JX2_ONE_ZM* pZ = &p->m_sZM[i];
			m_ZmName[i].SetText(pZ->m_szName);
			// [Main] blueprint: so + LevelTxt ("c\312p") + so + PeopleCountTxt
			// (" ng\255\352i") ghep thanh dong tin tuc
			sprintf(sz, "%dc\312p  %d ng\255\352i", (int)pZ->m_btLevel, (int)pZ->m_wMember);
			m_ZmInfo[i].SetText(sz);
			m_ZmJiyu[i].SetText(pZ->m_szRecruit);
			m_ZmQx[i].SetText(pZ->m_btTendency < TJX2_QX_NUM ? s_szRecQX[pZ->m_btTendency] : "?");
			{
				int nAdd = 0;
				int a;
				sz[0] = 0;
				for (a = 0; a < 4; a++)
				{
					BYTE bt = pZ->m_btAct[a];
					if (bt >= 1 && bt < TJX2_HD_NUM)
					{
						if (nAdd)
							strcat(sz, "  ");
						strcat(sz, s_szRecHD[bt]);
						nAdd++;
					}
				}
				m_ZmHd[i].SetText(nAdd ? sz : s_szRecHD[0]);
			}
			m_ZmLook[i].Show();
			m_ZmLook[i].Enable(true);
			m_ZmApply[i].Show();
			m_ZmApply[i].Enable(true);
		}
		else
		{
			m_ZmName[i].SetText("");
			m_ZmInfo[i].SetText("");
			m_ZmJiyu[i].SetText("");
			m_ZmQx[i].SetText("");
			m_ZmHd[i].SetText("");
			m_ZmLook[i].Hide();
			m_ZmLook[i].Enable(false);
			m_ZmApply[i].Hide();
			m_ZmApply[i].Enable(false);
		}
	}
}

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
			AskThenSendOp(NULL, NULL, defTONG_JX2_COP_KICK, dwTarget, 0, 0);
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
			SendOp(defTONG_JX2_COP_WS_ADD, 0, m_nSelWs, 0, NULL);
		else if (nIdx == 1)
		{
			TONG_JX2_WS_SYNC* pW = (TONG_JX2_WS_SYNC*)m_byWs;
			if (m_bHasWs && m_nSelWs >= 1 && m_nSelWs <= 7)
				SendOp(pW->m_sWs[m_nSelWs].btOpen ? defTONG_JX2_COP_WS_CLOSE : defTONG_JX2_COP_WS_OPEN,
					0, m_nSelWs, 0, NULL);
		}
		else if (nIdx == 2)
			SendOp(defTONG_JX2_COP_WS_UP, 0, m_nSelWs, 0, NULL);
		else if (nIdx == 3)
			SendOp(defTONG_JX2_COP_SETSTUNT, 0, m_nSelWs, 0, NULL);
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
	case WND_M_OTHER_WORK_RESULT:
		// ket qua hop xac nhan: nParam khac 0 = nguoi choi bam "Xac nhan"
		Show();
		if (uParam == TJX2_CONFIRM_ID)
		{
			if (nParam && m_nPendOp >= 0)
				SendOp(m_nPendOp, m_dwPendTarget, m_nPendP1, m_nPendP2,
					m_szPendText[0] ? m_szPendText : NULL);
			m_nPendOp = -1;
			m_szPendText[0] = 0;
			return 1;
		}
		if (uParam == TJX2_UNAME_ID)
		{
			if (nParam && m_nAmtOp >= 0)
			{
				const char* pszNm = (const char*)nParam;
				if (pszNm[0] && m_nAmtOp == defTONG_JX2_COP_UNION_KICK)
				{
					// thao tac pha huy: hoi xac nhan (chuoi blueprint) roi moi gui
					int nOpK = m_nAmtOp;
					m_nAmtOp = -1;
					strncpy(m_szPendText, pszNm, sizeof(m_szPendText) - 1);
					m_szPendText[sizeof(m_szPendText) - 1] = 0;
					AskThenSendOp("UnionStr", "StrUnionKickTong", nOpK, 0, 0, 0);
					return 1;
				}
				if (pszNm[0])
					SendOp(m_nAmtOp, m_dwAmtTarget, 0, 0, pszNm);
			}
			m_nAmtOp = -1;
			return 1;
		}
		if (uParam == TJX2_AMOUNT_ID)
		{
			// nParam = con tro chuoi so nguoi choi vua go trong hop nhap
			if (nParam && m_nAmtOp >= 0)
			{
				int nAmt = atoi((const char*)nParam);
				if (nAmt > 0 && m_nAmtOp == defTONG_JX2_COP_MINISTER_FIRE)
				{
					// cach chuc = pha huy: hoi xac nhan, slot mang trong nP1
					int nOpF = m_nAmtOp;
					m_nAmtOp = -1;
					AskThenSendOp("UnionStr", "StrFireMinister", nOpF, 0, nAmt, 0);
					return 1;
				}
				if (nAmt > 0)
					SendOp(m_nAmtOp, m_dwAmtTarget, nAmt, 0, NULL);
			}
			m_nAmtOp = -1;
			return 1;
		}
		break;

	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)&m_BtnClose)
		{
			CloseWindow(false);
			return 1;
		}
		{
			int i;
			// Tab0 "Tin tuc" = trang BASEINFO (defTONG_JX2_PAGE_INFO). Truoc day
			// tro nham vao FUNUSE nen ve ban thiet ke trang CHUC NANG: hang 1
			// cua FUNUSE la [TongName][TongUnion] chu khong phai [TongName]
			// [Master], nen ten bang chu roi vao o "Lien minh".
			// Nut "Su dung chuc nang" (m_BtnFun) van mo TJX2_UI_PAGE_FUNUSE.
			static const int s_nTabPage[TJX2_UI_TABS] =
			{
				defTONG_JX2_PAGE_INFO, TJX2_UI_PAGE_RECRUIT,
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
						m_nPage == TJX2_UI_PAGE_FUNUSE ||
						m_nPage == defTONG_JX2_PAGE_INFO ||
						m_nPage == defTONG_JX2_PAGE_WS)
					{
						// dong 0 gio la thanh vien dau tien (het dong tieu de tu sinh).
						// m_nOrd doi DONG HIEN THI -> chi so thanh vien trong goi
						// (danh sach co the da duoc sap xep lai).
						int nM = (i < defTONG_JX2_VIEW_MEMBERS) ? m_nOrd[i] : i;
						if (m_nSel == nM)
							m_bMDet = !m_bMDet;	// bam lai dong dang chon: bat/tat panel
						else
						{
							m_nSel = nM;
							m_bMDet = 1;
						}
						RenderMembers();
					}
					else if (m_nPage == TJX2_UI_PAGE_RECRUIT)
					{
						// danh sach don bat dau ngay tu dong 0 (khong con
						// dong tieu de tu sinh chiem cho)
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
					// KHONG lat lai o kiem o day: nut co CheckBox=1 nen
					// KWndButton::OnLBtnDown da tu lat truoc khi bao len
					// (WndButton.cpp:314-323). Lat them lan nua = khong doi.
					return 1;
				}
			}
			if (uParam == (unsigned int)&m_RtAll)
			{
				// nut co CheckBox=1 nen da TU LAT truoc khi bao len - doc lai
				// trang thai do de BAT/TAT ca cum, truoc day chi bat mot chieu
				int nAll = m_RtAll.IsButtonChecked() ? 1 : 0;
				for (q = 0; q < 14; q++)
					m_Rt[q].CheckButton(nAll);
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
					{
						// anh ten trang con doc thang tu ini (4 khoa ImgSubPage1-4
						// cua chinh section Fun_ImgSubPageMask - ban thiet ke goc)
						KIniFile Ini;
						char szScheme[256], szPath[300], szKey[24], szImg[128];
						g_UiBase.GetCurSchemePath(szScheme, sizeof(szScheme));
						sprintf(szPath, "%s\\%s", szScheme, TONG_JX2_INI);
						if (Ini.Load(szPath))
						{
							sprintf(szKey, "ImgSubPage%d", m_nFunSub);
							Ini.GetString("Fun_ImgSubPageMask", szKey, "", szImg, sizeof(szImg));
							if (szImg[0])
								m_FunMask.SetImage(ISI_T_SPR, szImg, false);
						}
					}
					// radio: chi nut dang chon sang (xem ghi chu o m_RcSub)
					for (int z = 0; z < 4; z++)
						m_FunSub[z].CheckButton((z + 1 == m_nFunSub) ? 1 : 0);
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
				else if (m_nPage == TJX2_UI_PAGE_OTHERZM)
					RequestPage(m_nPage, m_nZmStart);	// lam moi trang 2x2
				else
					// dung vai tro goc cua [BtnEnterMap]: VAO BON BANG
					// (nut "Xem tin Bang khac" da nam o m_Bot[2])
					SendOp(defTONG_JX2_COP_ENTER_MAP, 0, 0, 0, NULL);
				return 1;
			}
			if (uParam == (unsigned int)&m_Bot[1])
			{
				// O nay cua ban goc la [BtnRefresh] = LAM MOI (chu game da xac
				// nhan chu tren nut). Rieng o trang Danh sach bang - noi nguoi
				// CHUA CO BANG di vao - van giu duong mo don tao bang, vi cum
				// nut hanh dong da bi an va NPC tao bang dang tat tren may chu.
				if (m_nPage == TJX2_UI_PAGE_TONGLIST || m_nPage == TJX2_UI_PAGE_OTHERZM)
					KUiTongCreateSheet::OpenWindow();
				else
					RequestPage(m_nPage, m_nStart);
				return 1;
			}
			if (uParam == (unsigned int)&m_Bot[2])
			{
				// ban goc = trang 2x2 xem chieu mo bang khac, khong phai bang cot
				SwitchPage(TJX2_UI_PAGE_OTHERZM);
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
			for (f = 0; f < TJX2_FUN_BTNS; f++)
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
					// duoi nguoi khoi bang - khong hoan tac duoc, phai hoi truoc
					if (dwFT)
						AskThenSendOp(NULL, NULL, defTONG_JX2_COP_KICK, dwFT, 0, 0);
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
					m_nAmtOp = defTONG_JX2_COP_STORE_OFFER;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("C\312t c\350ng hi\325n (\256i\323m)",
						"100", this, TJX2_AMOUNT_ID, 1, 7);
					break;
				case 11:
					// gop tien ca nhan vao QUY KIEN THIET (nhan cong hien)
					m_nAmtOp = defTONG_JX2_COP_DONATE;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("G\343p ki\325n thi\325t (v\271n)",
						"100", this, TJX2_AMOUNT_ID, 1, 7);
					break;
				case 14:
					// lap lien minh: nhap ten (<= 20 ky tu de vua hop nhap)
					m_nAmtOp = defTONG_JX2_COP_UNION_CREATE;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("T\252n li\252n minh",
						"", this, TJX2_UNAME_ID, 1, 31);
					break;
				case 15:
					// xin vao lien minh: nhap ten mot bang thuoc lien minh
					m_nAmtOp = defTONG_JX2_COP_UNION_APPLY;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("T\252n bang thu\351c li\252n minh",
						"", this, TJX2_UNAME_ID, 1, 31);
					break;
				case 16:
					// minh chu duyet: nhap ten bang xin vao
					m_nAmtOp = defTONG_JX2_COP_UNION_ACCEPT;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("T\252n bang xin gia nh\313p",
						"", this, TJX2_UNAME_ID, 1, 31);
					break;
				case 17:
					// roi / giai tan lien minh - hoi xac nhan nguyen van blueprint
					if (m_bHasInfo && ((TONG_JX2_INFO_SYNC*)m_byInfo)->m_bUnionLeader)
						AskThenSendOp("UnionStr", "StrUnionDismiss",
							defTONG_JX2_COP_UNION_LEAVE, 0, 0, 0);
					else
						AskThenSendOp("UnionStr", "StrUnionLeave",
							defTONG_JX2_COP_UNION_LEAVE, 0, 0, 0);
					break;
				case 18:
					m_nAmtOp = defTONG_JX2_COP_UNION_KICK;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("T\252n bang mu\350n \256u\346i",
						"", this, TJX2_UNAME_ID, 1, 31);
					break;
				case 19:
					// phong dai than cho THANH VIEN DANG CHON; nhap so chuc
					if (dwFT)
					{
						m_nAmtOp = defTONG_JX2_COP_MINISTER_SET;
						m_dwAmtTarget = dwFT;
						KUiTongGetString::OpenWindow("Ch\370c: 1 TT / 2 NS / 3 TP",
							"1", this, TJX2_AMOUNT_ID, 1, 1);
					}
					break;
				case 20:
					m_nAmtOp = defTONG_JX2_COP_MINISTER_FIRE;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("C\270ch ch\370c: 1 TT / 2 NS / 3 TP",
						"1", this, TJX2_AMOUNT_ID, 1, 1);
					break;
				case 13:
					// nap tien ca nhan vao NGAN QUY bang (MONEYFUND_ADD)
					m_nAmtOp = defTONG_JX2_COP_DEPOSIT_MONEY;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("N\271p ng\251n qu\374 (v\271n)",
						"100", this, TJX2_AMOUNT_ID, 1, 7);
					break;
				case 12:
					{
						// DOI PHE: xoay vong Chinh -> Ta -> Trung lap theo phe
						// dang co. Di lenh JX2 (COP_CHANGE_CAMP) chu KHONG di
						// duong JX1: ca ba cong kiem tien cua duong JX1 deu doc
						// tui tien m_dwMoney - tui do tach rieng khoi ngan quy
						// JX2 va luon bang 0, nen bang co nhieu tien van bi bao
						// "khong du tien".
						int nCamp = 1;
						if (m_bHasInfo)
						{
							TONG_JX2_INFO_SYNC* pI = (TONG_JX2_INFO_SYNC*)m_byInfo;
							nCamp = (pI->m_btCamp >= 1 && pI->m_btCamp <= 3) ?
								(pI->m_btCamp % 3) + 1 : 1;
						}
						SendOp(defTONG_JX2_COP_CHANGE_CAMP, 0, nCamp, 0, NULL);
					}
					break;
				case 9:
					m_nAmtOp = defTONG_JX2_COP_DRAW_MONEY;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("R\363t ng\251n qu\374 (v\271n)",
						"100", this, TJX2_AMOUNT_ID, 1, 7);
					break;
				case 10:
					if (dwFT)
					{
						m_nAmtOp = defTONG_JX2_COP_PAY_MEMBER;
						m_dwAmtTarget = dwFT;
						KUiTongGetString::OpenWindow("Ph\270t ti\322n (v\271n)",
							"100", this, TJX2_AMOUNT_ID, 1, 7);
					}
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
					m_nSelWs = w;
					RenderWorkshop();
					return 1;
				}
			}
			for (w = 0; w < 6; w++)
			{
				if (uParam != (unsigned int)&m_WsBtn[w])
					continue;
				if (m_nSelWs < 1 || m_nSelWs > 7)
					return 1;
				// Nam thao tac nay HOI TRUOC KHI LAM - chuoi nhac lay tu
				// [Ws_WarnInfo] cua ban thiet ke goc. Ban goc CO Y khong hoi
				// khi dat cap dung nen nut do van lam ngay.
				switch (w)
				{
				case 0:
					AskThenSendOp("Ws_WarnInfo", "LearnPrompt",
						defTONG_JX2_COP_WS_ADD, 0, m_nSelWs, 0);
					break;
				case 1:
					AskThenSendOp("Ws_WarnInfo", "OpenPrompt",
						defTONG_JX2_COP_WS_OPEN, 0, m_nSelWs, 0);
					break;
				case 2:
					AskThenSendOp("Ws_WarnInfo", "ClosePrompt",
						defTONG_JX2_COP_WS_CLOSE, 0, m_nSelWs, 0);
					break;
				case 3:
					AskThenSendOp("Ws_WarnInfo", "UpgradePrompt",
						defTONG_JX2_COP_WS_UP, 0, m_nSelWs, 0);
					break;
				case 4:
					{
						// cap su dung: xoay vong 1..10 moi lan bam va gui gia tri THAT
						// (truoc day cung 10, nguoi choi khong chon duoc cap nao khac)
						TONG_JX2_WS_SYNC* pWl = (TONG_JX2_WS_SYNC*)m_byWs;
						int nLv = m_bHasWs ? (int)pWl->m_sWs[m_nSelWs].dwUseLevel : 0;
						nLv = (nLv % 10) + 1;
						SendOp(defTONG_JX2_COP_WS_SETLV, 0, m_nSelWs, nLv, NULL);
					}
					break;
				case 5:
					AskThenSendOp("Ws_WarnInfo", "DeletePrompt",
						defTONG_JX2_COP_WS_DEL, 0, m_nSelWs, 0);
					break;
				}
				return 1;
			}
		}
		if (uParam == (unsigned int)&m_RecQX)
		{
			m_nRecQX = (m_nRecQX + 1) % TJX2_QX_NUM;
			m_RecQX.SetLabel(s_szRecQX[m_nRecQX]);
			return 1;
		}
		{
			int r;
			for (r = 0; r < 4; r++)
			{
				if (uParam == (unsigned int)&m_RecHD[r])
				{
					m_nRecHD[r] = (m_nRecHD[r] + 1) % TJX2_HD_NUM;
					m_RecHD[r].SetLabel(s_szRecHD[m_nRecHD[r]]);
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
			// Server gui CA 8 don (tran) trong MOT goi va ta ve het 8 dong,
			// nen khong con trang de lat. Hai nut nay lam moi danh sach.
			RequestPage(m_nPage, 0);
			return 1;
		}
		// --- hang dieu khien duoi panel danh sach thanh vien ---
		if (uParam == (unsigned int)&m_MOnline)
		{
			// nut co CheckBox=1 nen no TU LAT truoc khi bao len day
			m_bOnlineFirst = m_MOnline.IsButtonChecked() ? 1 : 0;
			if (m_bHasMember)
				RenderMembers();
			return 1;
		}
		if (uParam == (unsigned int)&m_MSort)
		{
			// xoay vong 7 kieu sap xep. KHONG SetLabel len nut: sprite
			// "tra cuu danh sach" da nung san chu (section khong co Label=),
			// ve chong len vua de chu vua bi cat ("Diem ..").
			m_nSortMode = (m_nSortMode + 1) % 7;
			if (m_bHasMember)
				RenderMembers();
			return 1;
		}
		{
			int z;
			for (z = 0; z < 4; z++)
			{
				if (uParam == (unsigned int)&m_ZmApply[z])
				{
					TONG_JX2_OTHERZM_SYNC* pZ = (TONG_JX2_OTHERZM_SYNC*)m_byZM;
					if (m_bHasZM && z < (int)pZ->m_btCount)
						SendOp(defTONG_JX2_COP_APPLY_JOIN, pZ->m_sZM[z].m_dwNameID, 0, 0, NULL);
					return 1;
				}
				if (uParam == (unsigned int)&m_ZmLook[z])
				{
					// "Xem chi tiet": hop thong tin bang du lieu da dong bo
					TONG_JX2_OTHERZM_SYNC* pZ = (TONG_JX2_OTHERZM_SYNC*)m_byZM;
					if (m_bHasZM && z < (int)pZ->m_btCount)
					{
						TONG_JX2_ONE_ZM* pOne = &pZ->m_sZM[z];
						static const char* szC[4] = {"?", "Ch\335nh ph\270i", "T\265 ph\270i", "Trung l\313p"};
						char szD[320];
						sprintf(szD, "Bang h\351i: %s   Bang ch\361: %s   Phe: %s   "
							"\247\274ng c\312p ki\325n thi\325t: %d   Nh\251n s\350: %d   "
							"T\371 nh\313n t\365 c\312p: %d   T\365 ch\350i d\255\355i c\312p: %d",
							pOne->m_szName, pOne->m_szMaster,
							(pOne->m_btCamp >= 1 && pOne->m_btCamp <= 3) ? szC[pOne->m_btCamp] : "?",
							(int)pOne->m_btLevel, (int)pOne->m_wMember,
							(int)pOne->m_btAutoLv, (int)pOne->m_btRefuseLv);
						UIMessageBox(szD, this, "\247\343ng", 0, 0x5702);
					}
					return 1;
				}
			}
		}
		if (uParam == (unsigned int)&m_ZmPrev)
		{
			if (m_nZmStart >= 4)
			{
				m_nZmStart -= 4;
				RequestPage(TJX2_UI_PAGE_OTHERZM, m_nZmStart);
			}
			return 1;
		}
		if (uParam == (unsigned int)&m_ZmNext)
		{
			TONG_JX2_OTHERZM_SYNC* pZ = (TONG_JX2_OTHERZM_SYNC*)m_byZM;
			if (m_bHasZM && m_nZmStart + 4 < (int)pZ->m_wTotal)
			{
				m_nZmStart += 4;
				RequestPage(TJX2_UI_PAGE_OTHERZM, m_nZmStart);
			}
			return 1;
		}
		if (uParam == (unsigned int)&m_MJump)
		{
			char szPg[16];
			szPg[0] = 0;
			m_MPageEdit.GetText(szPg, sizeof(szPg), false);
			int nPg = atoi(szPg);
			if (nPg < 1)
				nPg = 1;
			m_nStart = (nPg - 1) * defTONG_JX2_VIEW_MEMBERS;
			m_nSel = 0;
			RequestPage(m_nPage, m_nStart);
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
