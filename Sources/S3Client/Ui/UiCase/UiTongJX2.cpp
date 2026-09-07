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
#include "UiTongAssignBox.h"	// cua so bo nhiem chuc vu (che do JX2)
#include "UiTongGrant.h"	// cua so phat ngan luong / cong hien theo chuc vu
#include "../elem/wndedit.h"
#include "../elem/wndlist2.h"
#include "../elem/wndscrollbar.h"
#include "UiTongManager.h"
#include "UiTongCreateSheet.h"
#include "UiTongGetString.h"
#include "UiInformation.h"
#include "UiPlayerBar.h"	// [BH100] InputNameMsg (mat dam tu menu nguoi choi)
#include "../elem/PopupMenu.h"	// [BH100] menu sap xep / chieu mo / nguoi choi
#include "../elem/MouseHover.h"	// [BH100] tooltip thanh vien khi bam ten
#include "../../../Engine/Src/Text.h"	// [BH100] TEncodeText cho <color>
#include "../../../Represent/iRepresent/iRepresentShell.h"	// KRUShadow + DrawPrimitives (ve nen panel)
extern iRepresentShell*	g_pRepresentShell;	// khai bao nhu MouseHover.cpp:16 / PopupMenu.cpp:21

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

// [BH100 07/09] 6 tab = 6 section nut cua cua so chinh ban goc (chu nung san trong sprite)
static const char* s_szTabSec[TJX2_UI_TABS] =
{
	"BtnBaseInfo", "BtnZhaoMu", "BtnWorkshop", "BtnFunUse", "BtnRightManage", "BtnTongRecord",
};

// 14 quyen theo DUNG thu tu mat na ban goc (= s_dwJX2RightList server;
// lech mot vi tri la phan quyen sai bit)
static const char* s_szRightName[14] =
{
	"BÊ nhi÷m - mi‘n nhi÷m", "ßÊi phe", "ßÊi t™n",
	"TrÙc xu t", "NhÀt k˝", "Qu∂n l˝ li™n minh",
	"Th®ng c p", "Tho∏i »n", "Khu v˘c", "T∏c ph≠Íng",
	"Ng©n qu¸", "MÙc ti™u tu«n", "Thµnh thﬁ", "K¸ n®ng",
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
	"Khu Binh gi∏p",
	"Khu Thi™n C´ng",
	"Khu M∆t nπ",
	"Khu Luy÷n tÀp",
	"Khu Thi™n ˝",
	"Khu L‘ vÀt",
	"Khu hoπt ÆÈng",
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
static const char* s_szFaction[MAX_FACTION_NUM + 1] =	// [HOASON 01/09] 13 phai (id 1..13)
{
	"-", "Thi’u L©m", "Thi™n V≠¨ng", "ß≠Íng M´n",
	"NgÚ ßÈc", "Nga My", "ThÛy Y™n", "C∏i Bang",
	"Thi™n Nh…n", "V‚ ßang", "C´n L´n",
	"Hoa S¨n", "VÚ HÂn", "Ti™u Dao",
};

// Ten 5 chuc vu - NGUYEN VAN bang chuoi ban Linux (stringtable_client.txt:481-485).
// Muc 0 va 3 cua ban goc CO dau cach o cuoi, giu de dung tung byte.
// CAM dung enum TONG_MEMBER_FIGURE cua JX1 (GameDataDef.h) de danh chi so:
// enum do co thu tu NGUOC (MEMBER=0 ... MASTER=3), con fork JX2 la
// 0 = Bang chu ... 4 = An sy.
static const char* s_szFigure[5] =
{
	"Bang chÒ ", "Tr≠Îng L∑o", "ßÈi tr≠Îng",
	"ß÷ tˆ ", "»n s¸",
};

// Trang Tin tuc: bind NGUYEN VAN section blueprint Â∏Æ‰ºöÂü∫Á°Ä‰ø°ÊÅØÈ°µÈù¢.ini
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
// TJX2_INFO_NUM = 35 (UiTongJX2.h)
static const TJX2InfoCtl s_sInfoCtl[TJX2_INFO_NUM] =
{
	{"TitleTongInfo",     "Tin t¯c"},
	{"TitleTongName",     "Bang hÈi"},          {"TxtTongName",      NULL},
	{"TitleMaster",       "Bang chÒ"},          {"TxtMaster",        NULL},
	{"TitleLeague",       "Li™n minh"},         {"TxtLeague",        NULL},
	{"TitleCamp",         "Phe"},                  {"TxtCamp",          NULL},
	{"TitleTongLevel",    "ßºng c p"},    {"TxtTongLevel",     NULL},
	{"TitleMemberNum",    "Nh©n sË"},        {"TxtMemberNum",     NULL},
	{"TitleBuildLevel",   "ßºng c p ki’n thi’t"}, {"TxtBuildLevel", NULL},
	{"TitleTongCapital",  "Ng©n qu¸"},       {"TxtTongCapital",   NULL},
	{"TitleBuildFund",    "Ng©n s∏ch ki’n thi’t"}, {"TxtBuildFund", NULL},
	{"TitleTotalOffer",   "CËng hi’n d˘ tr˜"}, {"TxtTotalOffer", NULL},
	{"TitlePersonalInfo", "Tin t¯c c∏ nh©n"},
	{"TitlePersonalOffer","CËng hi’n c∏ nh©n"}, {"TxtPersonalOffer", NULL},
	{"TitleWeeklyOffer",  "CËng hi’n tu«n"}, {"TxtWeeklyOffer", NULL},
	{"TxtHelpTitle",      "GiÛp ÆÏ"},
	{"Van1",              ""},   {"Van2",             ""},	// [BH100] exe khong nap Van1/Van2 - don vi ' van' nam trong so
	// 15/08 (+4): 2 hang TRONG SAN cua blueprint, khong chong toa do hang nao
	// dang hien (BattleFund Top=188, StoredBuildFund Top=245; DailyCost chong
	// BattleFund nen khong nap). Quy chien bi dang la cau hoi van hanh ma UI
	// khong he hien no.
	{"TitleBattleFund",   "Ng©n s∏ch chi’n bﬁ"}, {"TxtBattleFund", NULL},
	{"TitleStoredBuildFund", "Ki’n thi’t d˘ tr˜"}, {"TxtStoredBuildFund", NULL},
	// [BH100 07/09] hang 'chien bi bao tri tuan' cua ban goc (Top=207, giua BattleFund va StoredBuildFund)
	{"TitleStandFund",   "Ti“n duy tr◊ chi’n bﬁ tu«n"}, {"TxtStandFund", NULL},
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
#define TJX2_INFO_WARFUND		30	// 15/08: o gia tri "Ngan sach chien bi"
#define TJX2_INFO_STOREDBUILD	32	// 15/08: o gia tri "Kien thiet du tru"
#define TJX2_INFO_STANDFUND		34	// [BH100] o gia tri 'chien bi bao tri tuan'
#define TJX2_INFO_HELPTITLE		26

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
	"BÊ nhi÷m", "ßÊi phe", "ßÊi t™n", "TrÙc xu t",
	"NhÀt k˝", "Li™n minh", "Th®ng c p", "Tho∏i »n",
	"Khu v˘c", "T∏c ph≠Íng", "Ng©n qu¸", "MÙc ti™u tu«n",
	"Thµnh thﬁ", "K¸ n®ng",
};

// Nhan trang Chieu mo: NGUYEN VAN byte TCVN3 tu blueprint (Text= cua tung section)
struct TJX2RecLbl { const char* szSec; const char* szLabel; };
static const TJX2RecLbl s_sRecLbl[8] =
{
	{"JiyuTitle",       "Tin nhæn bang hÈi:"},
	{"ZhaoMuTitle",     "Tin t¯c chi™u mÈ bang hÈi"},
	{"QingXiangTitle",  "Khuynh h≠Ìng chÒ y’u bang hÈi:"},
	{"HuoDongTitle",    "Hoπt ÆÈng chÒ y’u bang hÈi:"},
	{"AutoAcceptTitle", "Cao h¨n ______c p t˘ ÆÈng nhÀp bang"},
	{"RefuseLevelTitle","Tı chËi gamer d≠Ìi______ c p xin nhÀp bang"},
	{"ApplyTitle",      "Danh s∏ch thµnh vi™n xin gia nhÀp bang hÈi"},
	{"ApplyerName",     "T™n gamer xin gia nhÀp             ßºng c p"},
};
#define TJX2_UI_PAGE_RECRUIT	5	// trang UI chieu mo (goi server PAGE_RECRUIT)
// Trang Su dung chuc nang: bind nguyen van section blueprint (Fun_<ten goc>).
#define TJX2_UI_PAGE_FUNUSE	6
#define TJX2_UI_PAGE_TONGLIST	7
#define TJX2_UI_PAGE_OTHERZM	8	// trang 2x2 xem chieu mo bang khac (blueprint rieng)	// danh sach bang (mo duoc khi CHUA vao bang)
struct TJX2FunTxt { const char* szSec; const char* szLabel; };	// szLabel NULL = o du lieu
static const TJX2FunTxt s_sFunTxt[TJX2_FUN_TXTS] =
{
	{"TitleTongInfo", "Tin t¯c"},
	{"TitleTongName", "T™n bang"},        {"TxtTongName", NULL},
	{"TitleTongUnion", "Li™n minh"},      {"TxtTongUnion", NULL},
	{"TitleBuildLevel", "ßºng c p"}, {"TxtBuildLevel", NULL},
	{"TitleTotalOffer", "TÊng cËng hi’n"}, {"TxtTotalOffer", NULL},
	{"TitleTongMoney", "Ng©n qu¸"},    {"TxtTongMoney", NULL},
	{"TitleBuildFund", "Qu¸ ki’n thi’t"}, {"TxtBuildFund", NULL},
	{"TitleBattleFund", "Qu¸ chi’n bﬁ"},  {"TxtBattleFund", NULL},
	{"TitleStoredBuildFund", "Ki’n thi’t d˘ tr˜"}, {"TxtStoredBuildFund", NULL},	// [BH100]
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
	"(ch≠a chÌn)",
	"R∂nh mÌi ch¨i", "Th›ch PK", "Tranh b∏",
	"D˘ th’ v´ tranh", "T›nh n®ng PVE", "K’t giao",
};
#define TJX2_HD_NUM		12
static const char* s_szRecHD[TJX2_HD_NUM] =
{
	"(ch≠a chÌn)",
	"TËng Kim", "V≠Ót ∂i", "ß∏nh Boss", "C´ng Thµnh",
	"Phong L®ng ÆÈ", "Nhi÷m vÙ D∑ T»u", "Vi™m Æ’",
	"T›n S¯", "MÈc Nh©n", "V‚ l©m li™n Æ u",
	"Thi™n Tr◊ MÀt C∂nh",
};

// 11 = gop tien ca nhan vao QUY KIEN THIET (COP_DONATE, nhan cong hien);
// 13 = nap tien ca nhan vao NGAN QUY bang (COP_DEPOSIT_MONEY) - hai duong
// tien TACH RIENG nhu ban goc (MONEY2BUILDFUND / MONEYFUND_ADD).
// 12 = doi phe bang hoi: KHONG can ma lenh JX2 moi - he JX1 da co san tron bo
// duong day (ApplyTongChangeCamp kiem chuc vu + tru tien, relay co DBChangeCamp),
// di y het cach nut "Roi bang" dang lam qua GTOI_TONG_ACTION.
// TJX2_FUN_BTNS = 34 (UiTongJX2.h). nAct:
//  0 nang cap / 1 phat cong hien / 2 duoi / 3 bo nhiem / 4 chieu mo / 5 tao lanh dia /
//  6 xoa lanh dia / 7 gui cong hien / 8 roi bang / 9 rut / 10 phat ngan luong /
//  11 gop kien thiet / 12 doi phe / 13 nap ngan quy / 14-18 lien minh / 19-20 dai than /
//  [BH100] 21 ngan quy->kien thiet / 22 kien thiet->chien bi / 23 thiet lap lanh dia /
//  24 ky nang (cot bieu tuong) / 25 ep thoai an / 26 chuyen vi / 27 doi danh hieu /
//  28 doi ten nam / 29 doi ten nu / 30 thoai an (ban than)
static const TJX2FunBtn s_sFunBtn[TJX2_FUN_BTNS] =
{
	{"BtnUpgradeBuildLevel", "N©ng c p", 0},
	{"BtnAssignTongOffer",   "Ph∏t", 1},
	{"BtnGetTongMoney",      "RÛt", 9},
	{"BtnAssignTongMoney",   "Ph∏t", 10},
	{"BtnTransformMoney",    "Chuy”n", 21},
	{"BtnRecruit",           "Chi™u mÈ", -1},	// [BH100] exe KHONG nap nut nay (cm_funuse) - chieu mo o tab rieng
	{"BtnKickOut",           "ßuÊi ng≠Íi", 2},
	{"BtnDepose",            "Tru t ch¯c", 3},
	{"Btn_DispenseOffer",    "Ph∏t cËng hi’n", 1},
	{"BtnCreateTongMap",     "Tπo l∑nh Æﬁa", 5},
	{"BtnChangeCamp",       "ßÊi phe", 12},
	{"BtnConfigureTongMap",  "Thi’t lÀp", 23},
	{"BtnTongStunt",         "K¸ n®ng", 24},
	{"BtnStorePersonalOffer","Gˆi", 7},
	{"BtnLeaveTong",         "RÍi bang", 8},
	{"BtnStoreBuildFund",    "Gˆi", 11},
	{"BtnStoreTongMoney",    "Gˆi", 13},
	{"BtnCreateUnion",       "LÀp li™n minh", 14},
	{"BtnApplyJionUnion",    "Vµo li™m minh", 15},
	{"BtnAcceptUnionReq",    "Gia nhÀp li™n minh", 16},
	{"BtnLeaveUnion",        "HÒy li™n minh", 17},
	{"BtnKickUnionTong",     "TrÙc xu t", 18},
	{"BtnAppointMinister",   " Òy nhi÷m Æπi th«n", 19},
	{"BtnFireMinister",      " C∏ch ch¯c Æπi th«n", 20},
	// [BH100 07/09] 10 nut ban goc con thieu
	{"BtnTransformBuildFund","Chuy”n", 22},
	{"BtnForceToRetire",     "Tho∏i »n", 25},
	{"BtnDemise",            "Chuy”n vﬁ", 26},
	{"BtnChangeTitle",       "ßÊi t™n", 27},
	{"BtnChangeMaleTitle",   "ßÊi t™n nam", 28},
	{"BtnChangeFemaleTitle", "ßÊi t™n n˜", 29},
	{"BtnRetire",            "Tho∏i »n", 30},
	{"BtnTongChallenge",     "Tuy™n chi’n", -1},	// Enable=0 trong ban goc
	{"BtnTongDetect",        "Th∏m th›nh", -1},	// Enable=0 trong ban goc
	{"BtnHelp",              "", -1},	// khong co xu ly trong game_y.exe
};
// trang con cua tung nut (0 = luon hien; 1 nhan su; 2 lanh dia; 3 lien minh; 4 dai than) - theo toa do blueprint
static const int s_nFunBtnSub[TJX2_FUN_BTNS] =
{
	0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0, 0, 0,
	3, 3, 3, 3, 3, 4, 4,
	0, 1, 1, 1, 1, 1, 0, 2, 2, 0,
};

// Trang Phuong tho: bind blueprint (Ws_*). Title co nhan TCVN3, Txt do render dien.
static const TJX2FunTxt s_sWsTxt[6] =
{
	{"TitleWorkshopNum", "SË t∏c ph≠Íng"}, {"TxtWorkshopNum", NULL},
	{"TitleMaxRank", "GiÌi hπn c p"}, {"TxtMaxRank", NULL},
	{"TitleServiceFee", "B∂o tr◊ hµng ngµy"}, {"TxtServiceFee", NULL},
};
static const char* s_szWsBtnSec[6] =
{
	"BtnLearnWorkshop", "BtnOpenWorkshop", "BtnCloseWorkshop",
	"BtnUpgradeWorkshop", "BtnSetUseLevelWorkshop", "BtnDeleteWorkshop",
};
static const char* s_szWsBtnLbl[6] =
{
	"LÀp khu", "MÎ khu", "ß„ng khu",
	"N©ng c p khu", "ß∆t c p dÔng", "X„a khu",
};

// Nhan 4 muc con trang Nhat ky - NGUYEN VAN khoa Label= cua ban thiet ke goc
// (truoc day ta tu rut gon thanh "Thong bao" / "Bang vu" / "Lich su").
static const char* s_szRcSub[4] =
{
	"MÙc ti™u tu«n", "C´ng c∏o bang hÈi",
	"S˘ ki÷n bang hÈi", "Lﬁch sˆ bang hÈi",
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
	m_bRcEdit = 0;
	m_nRtSub = 0;
	memset(m_nRtPage, 0, sizeof(m_nRtPage));
	m_dwViewTong = 0;
	m_nListMode = 0;
	m_nTipMember = -1;
	m_dwLastRowClick = 0;
	m_nLastRowClick = -1;
	m_nMenuKind = 0;
	m_nSexTitle = 0;
	m_nTongSort = 0;
	m_bHasUnion = 0;
	m_bNoTong = 0;
	m_bHasWeek = 0;
	memset(m_byUnion, 0, sizeof(m_byUnion));
	memset(m_byWeek, 0, sizeof(m_byWeek));
	memset(m_szHelp, 0, sizeof(m_szHelp));
	memset(m_szFunHelp, 0, sizeof(m_szFunHelp));
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
		ms_pSelf->m_nFunMode = 1;
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
	{
		if (ms_pSelf)
			ms_pSelf->m_bNoTong = 0;
		OpenWindow();
	}
	else
	{
		// chua vao bang: ban goc chi hien tab [BtnOtherZhaoMu] (2x2 chieu mo bang khac);
		// nut Tao moi van mo don tao bang
		KUiTongJX2* pWnd = OpenWindow();
		if (pWnd)
		{
			pWnd->m_bNoTong = 1;
			pWnd->SwitchPage(TJX2_UI_PAGE_OTHERZM);
		}
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
	KUiTongListJX2::CloseWindow(false);	// [BH100]
	g_MouseOver.CancelMouseHoverInfo();
	if (ms_pSelf)
	{
		ms_pSelf->m_dwViewTong = 0;
		ms_pSelf->m_nTipMember = -1;
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
	for (i = 0; i < TJX2_UI_BGS; i++)
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
	AddChild(&m_RtSub[0]);	// [BH100]
	AddChild(&m_RtSub[1]);
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
	AddChild(&m_BtnOtherZm);	// [BH100]
	for (i = 0; i < TJX2_UI_ROWS; i++)
		AddChild(&m_Row[i]);
	// nen do phai AddChild TRUOC chu, vi AddChild noi vao CUOI danh sach va
	// cua so ve theo dung thu tu do -> them sau = ve len tren = che mat chu
	for (i = 0; i < TJX2_INFO_NUM; i++)
		AddChild(&m_InfoBg[i]);
	// [BH100] thanh tien do nam TREN khung nen, DUOI chu so
	AddChild(&m_BarBuild);
	AddChild(&m_BarOffer);
	m_BarBuild.Enable(false);
	m_BarOffer.Enable(false);
	for (i = 0; i < TJX2_INFO_NUM; i++)
		AddChild(&m_Info[i]);
	AddChild(&m_InfoHelpTxt);
	// vung bam tren tieu de (trong suot) - AddChild SAU chu de nhan chuot
	for (i = 0; i < TJX2_INFO_NUM; i++)
		AddChild(&m_InfoHelp[i]);
	for (i = 0; i < TJX2_UI_ROWS; i++)
		AddChild(&m_BtnRowSel[i]);
	for (i = 0; i < TJX2_UI_ACTS; i++)
		AddChild(&m_BtnAct[i]);
	AddChild(&m_BtnPrev);
	AddChild(&m_BtnNext);
	AddChild(&m_MOnline);
	AddChild(&m_MSort);
	AddChild(&m_MTongSort);	// [BH100]
	AddChild(&m_MJump);
	AddChild(&m_MPage);
	AddChild(&m_MPageEdit);
	for (i = 0; i < TJX2_FUN_TXTS; i++)
		AddChild(&m_FunTxtBg[i]);
	AddChild(&m_BarFunBuild);
	AddChild(&m_BarFunOffer);
	m_BarFunBuild.Enable(false);
	m_BarFunOffer.Enable(false);
	for (i = 0; i < 6; i++)
		AddChild(&m_FunPBg[i]);
	// anh ten trang con: AddChild TRUOC cum nut hanh dong (AddChild noi
	// vao CUOI va ve theo thu tu do - them sau la de len tren, che nut)
	AddChild(&m_FunMask);
	for (i = 0; i < TJX2_FUN_TXTS; i++)
		AddChild(&m_FunTxt[i]);
	for (i = 0; i < 7; i++)
		AddChild(&m_FunP[i]);
	for (i = 0; i < TJX2_UI_ROWS; i++)
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
	AddChild(&m_BtnHelp);	// [BH100] BtnTongHelp
	// nen mo phai nam TREN ten (m_MList da add truoc) va DUOI chu panel
	AddChild(&m_MShade);
	m_MShade.Enable(false);
	m_MShade.Hide();
	for (i = 0; i < 7; i++)
		AddChild(&m_MDet[i]);
	AddChild(&m_RcList);
	AddChild(&m_RcScroll);
	m_RcList.SetScrollbar(&m_RcScroll);	// khuon UiFriendInterview.cpp:213
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

	// tab dung anh ÊãõÂãüÈ°µvn cua ban Linux (chu Viet nung san trong anh) -> khong ve chu de
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
	// [BH100] nut 'Su dung chuc nang' cu = tab 3 bay gio; nut rieng an han
	m_BtnFun.Hide();
	m_BtnFun.Enable(false);
	for (i = 0; i < 7; i++)
	{
		m_MDet[i].Hide();
		m_MDet[i].Enable(false);
	}
	m_InfoHelpTxt.Enable(false);

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
	for (i = 0; i < TJX2_UI_BGS; i++)
	{
		sprintf(szSec, "PageBg%d", i);
		ms_pSelf->m_PageBg[i].Init(&Ini, szSec);
	}
	// [BH100] 6 tab tu 6 section nut cua cua so chinh ban goc (Tab0..4 cu bo)
	for (i = 0; i < TJX2_UI_TABS; i++)
		ms_pSelf->m_BtnTab[i].Init(&Ini, s_szTabSec[i]);
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
		int nSub = 0;	// [BH100] SubPage= 0 Quyen han / 1 Phan phat
		Ini.GetInteger(szSec, "SubPage", 0, &nSub);
		ms_pSelf->m_nRtPage[i] = nSub ? 1 : 0;
	}
	ms_pSelf->m_RtSub[0].Init(&Ini, "Rt_BtnSubPage_0");	// [BH100] 2 trang con (CheckBox=1, radio tay)
	ms_pSelf->m_RtSub[1].Init(&Ini, "Rt_BtnSubPage_1");
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
		ms_pSelf->m_MTongSort.Init(&Ini, "Fun_BtnTongSortMenu");	// [BH100]
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
	ms_pSelf->m_RecToggle.SetLabel("ß„ng/mÎ tuy”n");
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
		for (i = 1; i <= 7; i++)
			ms_pSelf->m_WsIcon[i].SetLabel("");	// bo so - da co hinh nen
	}
	for (i = 0; i < TJX2_INFO_NUM; i++)
	{
		sprintf(szSec, "Info_%s", s_sInfoCtl[i].szSec);
		ms_pSelf->m_InfoBg[i].Init(&Ini, szSec);	// doc khoa Image= (neu co)
		ms_pSelf->m_Info[i].Init(&Ini, szSec);
		if (s_sInfoCtl[i].szLabel)
		{
			// [BH100] uu tien Label= cua ban thiet ke goc
			char szLb[96];
			szLb[0] = 0;
			Ini.GetString(szSec, "Label", "", szLb, sizeof(szLb));
			ms_pSelf->m_Info[i].SetText(szLb[0] ? szLb : s_sInfoCtl[i].szLabel);
		}
		// [BH100] HelpInfo= -> bam tieu de hien o Info_TxtHelp (ban goc BaseInfo WndProc)
		ms_pSelf->m_szHelp[i][0] = 0;
		Ini.GetString(szSec, "HelpInfo", "", ms_pSelf->m_szHelp[i], sizeof(ms_pSelf->m_szHelp[i]));
		ms_pSelf->m_InfoHelp[i].Init(&Ini, "RowSel0");	// nut trong suot, khong anh
		{
			int nL = 0, nT = 0, nW = 0, nH = 0;
			Ini.GetInteger(szSec, "Left", 0, &nL);
			Ini.GetInteger(szSec, "Top", 0, &nT);
			Ini.GetInteger(szSec, "Width", 0, &nW);
			Ini.GetInteger(szSec, "Height", 0, &nH);
			ms_pSelf->m_InfoHelp[i].SetPosition(nL, nT);
			ms_pSelf->m_InfoHelp[i].SetSize(nW, nH);
			ms_pSelf->m_InfoHelp[i].Enable(ms_pSelf->m_szHelp[i][0] ? true : false);
		}
	}
	ms_pSelf->m_InfoHelpTxt.Init(&Ini, "Info_TxtHelp");
	ms_pSelf->m_InfoHelpTxt.SetText("");
	{
		// [BH100] thanh tien do dat dung o so (Txt) cua BuildFund / PersonalOffer
		int nX = 0, nY = 0, nW = 0, nH = 0;
		ms_pSelf->m_Info[TJX2_INFO_BUILDFUND].GetPosition(&nX, &nY);
		ms_pSelf->m_Info[TJX2_INFO_BUILDFUND].GetSize(&nW, &nH);
		ms_pSelf->m_BarBuild.SetPosition(nX + 2, nY + 2);
		ms_pSelf->m_BarBuild.SetSize(nW - 4, nH - 4);
		ms_pSelf->m_Info[TJX2_INFO_MYOFFER].GetPosition(&nX, &nY);
		ms_pSelf->m_Info[TJX2_INFO_MYOFFER].GetSize(&nW, &nH);
		ms_pSelf->m_BarOffer.SetPosition(nX + 2, nY + 2);
		ms_pSelf->m_BarOffer.SetSize(nW - 4, nH - 4);
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
	for (i = 0; i < TJX2_FUN_TXTS; i++)
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
		ms_pSelf->m_szFunHelp[i][0] = 0;
		Ini.GetString(szSec, "HelpInfo", "", ms_pSelf->m_szFunHelp[i], sizeof(ms_pSelf->m_szFunHelp[i]));
	}
	{
		// [BH100] thanh tien do trang chuc nang: Fun_TxtBuildFund (12) + Fun_TxtPersonalOffer
		int nX = 0, nY = 0, nW = 0, nH = 0;
		ms_pSelf->m_FunTxt[12].GetPosition(&nX, &nY);
		ms_pSelf->m_FunTxt[12].GetSize(&nW, &nH);
		ms_pSelf->m_BarFunBuild.SetPosition(nX + 2, nY + 2);
		ms_pSelf->m_BarFunBuild.SetSize(nW - 4, nH - 4);
	}
	ms_pSelf->m_FunP[0].Init(&Ini, "Fun_TxtPersonalInfo");
	ms_pSelf->m_FunP[0].SetText("Tin t¯c c∏ nh©n");
	ms_pSelf->m_FunPBg[0].Init(&Ini, "Fun_TitlePersonalOffer");
	ms_pSelf->m_FunP[1].Init(&Ini, "Fun_TitlePersonalOffer");
	ms_pSelf->m_FunP[1].SetText("CËng hi’n c∏ nh©n");
	ms_pSelf->m_FunPBg[1].Init(&Ini, "Fun_TxtPersonalOffer");	// thanh ong
	ms_pSelf->m_FunP[2].Init(&Ini, "Fun_TxtPersonalOffer");
	{
		int nX = 0, nY = 0, nW = 0, nH = 0;
		ms_pSelf->m_FunP[2].GetPosition(&nX, &nY);
		ms_pSelf->m_FunP[2].GetSize(&nW, &nH);
		ms_pSelf->m_BarFunOffer.SetPosition(nX + 2, nY + 2);
		ms_pSelf->m_BarFunOffer.SetSize(nW - 4, nH - 4);
	}
	// HAI HANG con lai cua khoi Tin tuc ca nhan - ban thiet ke goc CO du
	// ba hang (Cong hien ca nhan / Ngan quy / Ngan sach kien thiet), moi
	// hang = khung do + o so + nut Gui. Truoc day chi bind hang dau nen
	ms_pSelf->m_FunPBg[2].Init(&Ini, "Fun_TitleTongMoney2");
	ms_pSelf->m_FunP[3].Init(&Ini, "Fun_TitleTongMoney2");
	ms_pSelf->m_FunP[3].SetText("Ng©n qu¸");
	ms_pSelf->m_FunPBg[3].Init(&Ini, "Fun_TxtTongMoney2");
	ms_pSelf->m_FunP[4].Init(&Ini, "Fun_TxtTongMoney2");
	ms_pSelf->m_FunPBg[4].Init(&Ini, "Fun_TitleBuildFund2");
	ms_pSelf->m_FunP[5].Init(&Ini, "Fun_TitleBuildFund2");
	ms_pSelf->m_FunP[5].SetText("Ng©n s∏ch ki’n thi’t");
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
	ms_pSelf->m_BtnOtherZm.Init(&Ini, "BtnOtherZhaoMu");	// [BH100]
	ms_pSelf->m_BtnOtherZm.SetLabel("");
	ms_pSelf->m_BtnHelp.Init(&Ini, "BtnTongHelp");
	KUiTongListJX2::LoadScheme(pScheme);
	KUiTongHelpJX2::LoadScheme(pScheme);
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
	ms_pSelf->m_RcList.Init(&Ini, "Rc_RecordList_List");
	ms_pSelf->m_RcScroll.Init(&Ini, "Rc_RecordList_Scroll");	// tu nap _Btn
	ms_pSelf->m_BtnPrev.Init(&Ini, "Fun_BtnPrevPage");	// [BH100] dung section goc (Truoc / Ke)
	ms_pSelf->m_BtnNext.Init(&Ini, "Fun_BtnNextPage");
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
			ms_pSelf->m_ZmQxT[z].SetText("Khuynh h≠Ìng bang hÈi");
			sprintf(szSec, "Ozm_QingXiang%d", z + 1);
			ms_pSelf->m_ZmQx[z].Init(&Ini, szSec);
			ms_pSelf->m_ZmQx[z].Enable(false);
			ms_pSelf->m_ZmQx[z].SetText("");
			sprintf(szSec, "Ozm_HuoDongTitle%d", z + 1);
			ms_pSelf->m_ZmHdT[z].Init(&Ini, szSec);
			ms_pSelf->m_ZmHdT[z].Enable(false);
			ms_pSelf->m_ZmHdT[z].SetText(z == 0 ? "Hoπt ÆÈng bang hÈi"
				: "Hoπt ÆÈng bang hÈi:");
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
	sTJX2Log("[REQ] trangUI=%d start=%d xem=%u", nPage, nStart, (unsigned)m_dwViewTong);
	if (!g_pCoreShell)
		return;
	KUiTongJX2View sV;
	memset(&sV, 0, sizeof(sV));
	sV.dwTarget = m_dwViewTong;
	sV.nSort = m_nSortMode;
	sV.nOnline = m_bOnlineFirst;
	const int nUiPage = nPage;
	if (m_dwViewTong)
	{
		// [BH100] dang xem bang khac: chi trang Tin tuc (+ danh sach thanh vien chi doc)
		sV.nPage = defTONG_JX2_PAGE_INFO; sV.nStart = 0;
		g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW2, (unsigned int)&sV, 0);
		sV.nPage = defTONG_JX2_PAGE_MEMBER; sV.nStart = m_nStart;
		g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW2, (unsigned int)&sV, 0);
		return;
	}
	if (nPage == TJX2_UI_PAGE_RECRUIT)
		nPage = defTONG_JX2_PAGE_RECRUIT;
	else if (nPage == 4)
	{
		if (m_nRcSub == 0)
			nPage = defTONG_JX2_PAGE_WEEKGOAL;	// [BH100] bao cao muc tieu tuan
		else
		{
			nPage = defTONG_JX2_PAGE_RECORD;
			nStart = (m_nRcSub == 3) ? 1 : 0;	// lich su / su kien (thong bao di kem)
		}
	}
	else if (nPage == TJX2_UI_PAGE_FUNUSE)
	{
		// trang chuc nang can CA thong tin bang CA danh sach (thanh vien hoac bang lien minh)
		sV.nPage = defTONG_JX2_PAGE_INFO; sV.nStart = 0;
		g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW2, (unsigned int)&sV, 0);
		nPage = (m_nListMode == 2) ? defTONG_JX2_PAGE_UNIONLIST :
			(m_nListMode == 1) ? defTONG_JX2_PAGE_TONGLIST : defTONG_JX2_PAGE_MEMBER;
	}
	else if (nPage == TJX2_UI_PAGE_TONGLIST)
		nPage = defTONG_JX2_PAGE_TONGLIST;
	else if (nPage == TJX2_UI_PAGE_OTHERZM)
		nPage = defTONG_JX2_PAGE_OTHERZM;
	else if (nPage == defTONG_JX2_PAGE_WS || nUiPage == defTONG_JX2_PAGE_INFO ||
		nUiPage == defTONG_JX2_PAGE_MEMBER || nUiPage == defTONG_JX2_PAGE_RIGHT)
	{
		// cac trang co panel danh sach thanh vien ben phai
		if (nUiPage != defTONG_JX2_PAGE_MEMBER && nUiPage != defTONG_JX2_PAGE_RIGHT)
		{
			sV.nPage = defTONG_JX2_PAGE_MEMBER; sV.nStart = m_nStart;
			g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW2, (unsigned int)&sV, 0);
		}
		if (nUiPage == defTONG_JX2_PAGE_INFO)
		{
			sV.nPage = defTONG_JX2_PAGE_INFO; sV.nStart = 0;
			g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW2, (unsigned int)&sV, 0);
			return;
		}
	}
	sV.nPage = nPage;
	sV.nStart = nStart;
	g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW2, (unsigned int)&sV, 0);
}

// Ma nhan dien hop xac nhan cua rieng cua so nay (tra ve qua WND_M_OTHER_WORK_RESULT)
#define TJX2_CONFIRM_ID		0x5701
// hop nhap SO TIEN/DIEM cho 5 thao tac ngan quy (ban goc nguoi choi tu go so)
#define TJX2_AMOUNT_ID		0x5703
// hop nhap TEN (lien minh / bang) - gui nguyen chuoi qua szText
#define TJX2_UNAME_ID		0x5704
// [BH100 07/09]
#define TJX2_TITLE_ID		0x5705	// hop nhap danh hieu ghe -> COP_SET_TITLE
#define TJX2_SEXTITLE_ID	0x5706	// hop nhap danh hieu nam/nu -> COP_SET_SEX_TITLE
#define TJX2_RIGHTS_ID		0x5707	// xac nhan 'giao quyen nay cho %s' -> ApplyRights
#define TJX2_WORD_ID		0x5708	// hop nhap loi nhan (LeaveWord o trang Bang vu)
#define TJX2_MENU_ID		0x5710	// uParam cua KPopupMenu::Popup (+ m_nMenuKind)

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
		strcpy(szMsg, "C„ chæc th˘c hi÷n thao t∏c nµy kh´ng?");
	m_nPendOp = nOp;
	m_dwPendTarget = dwTarget;
	m_nPendP1 = nP1;
	m_nPendP2 = nP2;
	UIMessageBox(szMsg, this, "X∏c nhÀn", "Hu˚ b·", TJX2_CONFIRM_ID);
}

void KUiTongJX2::SendOpStatic(int nOp, unsigned long dwTarget, int nP1, int nP2, const char* pszText)
{
	if (ms_pSelf)
		ms_pSelf->SendOp(nOp, dwTarget, nP1, nP2, pszText);
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
				ms_pSelf->RenderRecord();
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
			if (ms_pSelf->m_nPage != TJX2_UI_PAGE_TONGLIST &&
				!(ms_pSelf->m_nPage == TJX2_UI_PAGE_FUNUSE && ms_pSelf->m_nListMode == 2))
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
			if (KUiTongListJX2::GetIfVisible())	// [BH100] cua so danh sach bang
				KUiTongListJX2::OnListData(pData, nLen);
			else if (ms_pSelf->m_nPage == TJX2_UI_PAGE_FUNUSE && ms_pSelf->m_nListMode == 1)
				ms_pSelf->RenderTongListPanel();
			else if (ms_pSelf->m_nPage == TJX2_UI_PAGE_TONGLIST)
				ms_pSelf->RenderTongList();
		}
		break;
	case defTONG_JX2_PAGE_UNIONLIST:	// [BH100]
		if (nLen <= (int)sizeof(ms_pSelf->m_byUnion))
		{
			memcpy(ms_pSelf->m_byUnion, pData, nLen);
			ms_pSelf->m_bHasUnion = 1;
			if (ms_pSelf->m_nPage == TJX2_UI_PAGE_FUNUSE && ms_pSelf->m_nListMode == 2)
				ms_pSelf->RenderUnionList();
		}
		break;
	case defTONG_JX2_PAGE_WEEKGOAL:	// [BH100]
		if (nLen <= (int)sizeof(ms_pSelf->m_byWeek))
		{
			memcpy(ms_pSelf->m_byWeek, pData, nLen);
			ms_pSelf->m_bHasWeek = 1;
			if (ms_pSelf->m_nPage == 4 && ms_pSelf->m_nRcSub == 0)
				ms_pSelf->RenderWeekGoal();
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
			// D5: dong bo con tro trang voi lat cat server vua gui - server
			// day nguoc (PushViewTo) luon la trang 0, khong dong bo thi nut
			// Trang ke nhay vot
			ms_pSelf->m_nRecStart =
				(int)((TONG_JX2_RECRUIT_SYNC*)ms_pSelf->m_byRecruit)->m_wApplyStart;
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
		if (m_nPage != 4 && m_nPage != TJX2_UI_PAGE_RECRUIT && m_nPage != TJX2_UI_PAGE_TONGLIST)
			m_Row[i].SetText("");	// [BH100] cot Hang dung chung m_Row
	}
}

// bo cuc dong: trang co panel danh sach phai = 3 cot Hang(343)/Ten(395)/Loai hinh(468),
// 25 dong buoc 13px trong khung [Fun_MemberList] 341,67 212x340 (ban goc 25 dong).
void KUiTongJX2::RepositionRows()
{
	int i;
	BOOL bList = (m_nPage == defTONG_JX2_PAGE_MEMBER || m_nPage == defTONG_JX2_PAGE_RIGHT ||
		m_nPage == TJX2_UI_PAGE_FUNUSE || m_nPage == defTONG_JX2_PAGE_INFO ||
		m_nPage == defTONG_JX2_PAGE_WS);
	for (i = 0; i < TJX2_UI_ROWS; i++)
	{
		// mac dinh = bo cuc 3 cot cua panel phai
		int nY = 68 + i * 13;
		m_Row[i].SetPosition(343, nY);
		m_Row[i].SetSize(40, 13);
		m_MList[i].SetPosition(395, nY);
		m_MList[i].SetSize(72, 13);
		m_RowDim[i].SetPosition(468, nY);
		m_RowDim[i].SetSize(88, 13);
		m_BtnRowSel[i].SetPosition(341, nY);
		m_BtnRowSel[i].SetSize(212, 13);
		m_Row[i].SetTextColor(0xFF000000 | (255 << 16) | (253 << 8) | 122);
		m_MList[i].SetTextColor(0xFF000000 | (255 << 16) | (253 << 8) | 122);
		m_RowDim[i].SetTextColor(0xFF000000 | (120 << 16) | (120 << 8) | 120);
		if (bList)
			m_BtnRowSel[i].Enable(true);
		else if (m_nPage == 4)
		{
			// vung RecordList cua blueprint (khong dung m_Row nua - co m_RcList)
			m_Row[i].SetPosition(25, 79 + i * 22);
			m_Row[i].SetSize(540, 20);
			m_BtnRowSel[i].SetPosition(25, 79 + i * 22);
			m_BtnRowSel[i].SetSize(230, 20);
			m_BtnRowSel[i].Enable(false);
		}
		else if (m_nPage == TJX2_UI_PAGE_TONGLIST)
		{
			// ba cot that: ten bang | bang chu | cap / thanh vien (trang cho nguoi chua co bang)
			m_Row[i].SetPosition(40, 70 + i * 14);
			m_Row[i].SetSize(160, 13);
			m_MList[i].SetPosition(205, 70 + i * 14);
			m_MList[i].SetSize(150, 13);
			m_RowDim[i].SetPosition(365, 70 + i * 14);
			m_RowDim[i].SetSize(140, 13);
			m_BtnRowSel[i].SetPosition(40, 70 + i * 14);
			m_BtnRowSel[i].SetSize(470, 13);
			m_BtnRowSel[i].Enable(i >= 1);
		}
		else if (m_nPage == TJX2_UI_PAGE_RECRUIT)
		{
			// vung [Rec_ApplyerList] cua ban thiet ke goc: (308,92) rong 212, 8 dong buoc 19px
			m_Row[i].SetPosition(310, 94 + i * 19);
			m_Row[i].SetSize(210, 16);
			m_BtnRowSel[i].SetPosition(308, 92 + i * 19);
			m_BtnRowSel[i].SetSize(212, 19);
			m_BtnRowSel[i].Enable(i < 8);
		}
		else
		{
			m_Row[i].SetPosition(30, 64 + i * 23);
			m_BtnRowSel[i].Enable(false);
		}
	}
}

void KUiTongJX2::SwitchPage(int nPage)
{
	HideMemberTip();
	if (m_dwViewTong && nPage != defTONG_JX2_PAGE_INFO)
		nPage = defTONG_JX2_PAGE_INFO;	// [BH100] xem bang khac: chi trang Tin tuc
	m_nPage = nPage;
	m_nStart = 0;
	m_nRecStart = 0;
	m_nSel = 0;
	m_nSelWs = 1;
	if (nPage != TJX2_UI_PAGE_FUNUSE)
		m_nListMode = 0;
	else
		m_nListMode = (m_nFunSub == 2) ? 1 : (m_nFunSub == 3) ? 2 : 0;	// exe: trang con 1..4 -> mode 0/1/2/0
	// nen phan trang: 0 tin tuc / 1 chieu mo / 2 phan phoi / 3 tac phuong / 4 nhat ky
	{
		int nBg = 0;
		if (nPage == TJX2_UI_PAGE_FUNUSE || nPage == TJX2_UI_PAGE_OTHERZM)
			nBg = -1;	// nen rieng m_FunBg / m_ZmBg
		else if (nPage == TJX2_UI_PAGE_RECRUIT)
			nBg = 1;
		else if (nPage == defTONG_JX2_PAGE_MEMBER || nPage == defTONG_JX2_PAGE_RIGHT)
			nBg = 2;
		else if (nPage == defTONG_JX2_PAGE_WS)
			nBg = 3;
		else if (nPage == 4)
			nBg = 4;
		for (int i = 0; i < TJX2_UI_BGS; i++)
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
	// trang Phan phoi: 14 nut quyen theo 2 trang con + Chon tat ca + Phan quyen
	{
		BOOL bRt = (nPage == defTONG_JX2_PAGE_MEMBER || nPage == defTONG_JX2_PAGE_RIGHT);
		for (int i = 0; i < 2; i++)
		{
			if (bRt) { m_RtSub[i].Show(); m_RtSub[i].Enable(true); }
			else { m_RtSub[i].Hide(); m_RtSub[i].Enable(false); }
		}
		if (bRt) { m_RtAll.Show(); m_RtAll.Enable(true); m_RtApply.Show(); m_RtApply.Enable(true); }
		else { m_RtAll.Hide(); m_RtAll.Enable(false); m_RtApply.Hide(); m_RtApply.Enable(false); }
		if (bRt)
			UpdateRightSub();
		else
		{
			for (int i = 0; i < 14; i++) { m_Rt[i].Hide(); m_Rt[i].Enable(false); }
		}
	}
	// trang Su dung chuc nang
	{
		int i;
		BOOL bFun = (nPage == TJX2_UI_PAGE_FUNUSE);
		if (bFun) m_FunBg.Show(); else m_FunBg.Hide();
		for (i = 0; i < TJX2_FUN_TXTS; i++)
		{
			if (bFun) { m_FunTxt[i].Show(); m_FunTxtBg[i].Show(); }
			else { m_FunTxt[i].Hide(); m_FunTxtBg[i].Hide(); }
		}
		for (i = 0; i < 7; i++)
			if (bFun) m_FunP[i].Show(); else m_FunP[i].Hide();
		for (i = 0; i < 6; i++)
			if (bFun) m_FunPBg[i].Show(); else m_FunPBg[i].Hide();
		if (bFun)
		{
			LoadFunMaskImage();
			m_FunMask.Show();
			m_BarFunBuild.Show();
			m_BarFunOffer.Show();
		}
		else
		{
			m_FunMask.Hide();
			m_BarFunBuild.Hide();
			m_BarFunOffer.Hide();
		}
		for (i = 0; i < 4; i++)
		{
			if (bFun) { m_FunSub[i].Show(); m_FunSub[i].Enable(true); }
			else { m_FunSub[i].Hide(); m_FunSub[i].Enable(false); }
			m_FunSub[i].CheckButton((bFun && i + 1 == m_nFunSub) ? 1 : 0);
		}
		UpdateFunButtons();
		m_bMDet = 0;
		m_nMDetRows = 0;
		m_MShade.Hide();
		m_RcList.Hide();
		m_RcScroll.Hide();
	}
	// panel danh sach phai (3 cot + hang dieu khien)
	{
		int i;
		BOOL bL = (nPage == defTONG_JX2_PAGE_MEMBER || nPage == defTONG_JX2_PAGE_RIGHT ||
			nPage == TJX2_UI_PAGE_FUNUSE || nPage == defTONG_JX2_PAGE_WS ||
			nPage == defTONG_JX2_PAGE_INFO);
		BOOL bCol = (bL || nPage == TJX2_UI_PAGE_TONGLIST);
		for (i = 0; i < TJX2_UI_ROWS; i++)
		{
			if (bCol) { m_MList[i].Show(); m_RowDim[i].Show(); }
			else { m_MList[i].Hide(); m_RowDim[i].Hide(); }
		}
		for (i = 0; i < 7; i++)
			m_MDet[i].Hide();
		for (i = 0; i < 3; i++)
			if (bL) m_ColHdr[i].Show(); else m_ColHdr[i].Hide();
		BOOL bUnion = (bL && m_nListMode >= 1);	// mode 1/2: danh sach bang -> menu sap xep bang, khong co online/thanh vien
		if (bL && !bUnion) { m_MOnline.Show(); m_MOnline.Enable(true); } else { m_MOnline.Hide(); m_MOnline.Enable(false); }
		if (bL && !bUnion) { m_MSort.Show(); m_MSort.Enable(true); } else { m_MSort.Hide(); m_MSort.Enable(false); }
		if (bUnion) { m_MTongSort.Show(); m_MTongSort.Enable(true); } else { m_MTongSort.Hide(); m_MTongSort.Enable(false); }
		if (bL) { m_MJump.Show(); m_MJump.Enable(true); } else { m_MJump.Hide(); m_MJump.Enable(false); }
		if (bL) m_MPage.Show(); else m_MPage.Hide();
		if (bL) { m_MPageEdit.Show(); m_MPageEdit.Enable(true); } else { m_MPageEdit.Hide(); m_MPageEdit.Enable(false); }
		m_MOnline.CheckButton(m_bOnlineFirst ? 1 : 0);
		m_RecToggle.Hide();
		m_RecToggle.Enable(false);
		if (nPage == defTONG_JX2_PAGE_WS) m_WsSel.Show(); else m_WsSel.Hide();
		// tieu de cot theo che do (Fun_TitleName cua ban goc)
		{
			char szH[64];
			if (bUnion && m_nListMode == 2)
			{
				m_ColHdr[1].SetText(GetIniString("Fun_TitleName", "TongTitle", szH, sizeof(szH)));
				m_ColHdr[2].SetText(GetIniString("Fun_TitleName", "UnionTongFigureTitle", szH, sizeof(szH)));
			}
			else if (bUnion)
			{
				// mode 1 (toan bo bang): cot 3 = tieu chi dang chon o menu Fun_BtnTongSortMenu
				char szKey[16];
				sprintf(szKey, "Item_%d", m_nTongSort % 5);
				m_ColHdr[1].SetText(GetIniString("Fun_TitleName", "TongTitle", szH, sizeof(szH)));
				m_ColHdr[2].SetText(GetIniString("Fun_BtnTongSortMenu", szKey, szH, sizeof(szH)));
			}
			else
			{
				m_ColHdr[1].SetText(GetIniString("Fun_TitleName", "MemberTitle", szH, sizeof(szH)));
				m_ColHdr[2].SetText(GetIniString("Fun_TxtType", "Text", szH, sizeof(szH)));
			}
		}
	}
	// trang Phuong tho
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
			if (!bWs) { m_WsIconHL[i].Hide(); m_WsRank[i].Hide(); }
		}
		for (i = 0; i < 3; i++)
			if (bWs) m_WsSub[i].Show(); else m_WsSub[i].Hide();
		if (bWs) m_WsIconSel.Show(); else m_WsIconSel.Hide();
		if (bWs) m_WsArt.Show(); else m_WsArt.Hide();
	}
	// trang Nhat ky: 4 muc con radio; khung sua chi khi bam Hieu chinh (BtnEditAnnounce);
	// LeaveWord = gui (thong bao / loi nhan); ca hai AN o muc Lich su nhu ban goc
	{
		int i;
		BOOL bRc = (nPage == 4);
		m_bRcEdit = 0;
		for (i = 0; i < 4; i++)
		{
			if (bRc) { m_RcSub[i].Show(); m_RcSub[i].Enable(true); }
			else { m_RcSub[i].Hide(); m_RcSub[i].Enable(false); }
		}
		for (i = 0; i < 4; i++)
			m_RcSub[i].CheckButton((bRc && i == m_nRcSub) ? 1 : 0);
		m_RcEditor.Hide();
		m_RcEditor.Enable(false);
		BOOL bEd = (bRc && m_nRcSub == 1);
		if (bEd) { m_RcSave.Show(); m_RcSave.Enable(true); }
		else { m_RcSave.Hide(); m_RcSave.Enable(false); }
		BOOL bLw = (bRc && (m_nRcSub == 1 || m_nRcSub == 2));
		if (bLw) { m_RcLeaveWord.Show(); m_RcLeaveWord.Enable(true); }
		else { m_RcLeaveWord.Hide(); m_RcLeaveWord.Enable(false); }
	}
	// trang chieu mo
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
	// trang Tin tuc: o Info_* + giup do + thanh tien do
	{
		BOOL bInfo = (nPage == defTONG_JX2_PAGE_INFO);
		for (int i = 0; i < TJX2_INFO_NUM; i++)
		{
			if (bInfo) { m_InfoBg[i].Show(); m_Info[i].Show(); m_InfoHelp[i].Show(); }
			else { m_InfoBg[i].Hide(); m_Info[i].Hide(); m_InfoHelp[i].Hide(); }
		}
		if (bInfo) { m_InfoHelpTxt.Show(); m_BarBuild.Show(); m_BarOffer.Show(); }
		else { m_InfoHelpTxt.Hide(); m_BarBuild.Hide(); m_BarOffer.Hide(); }
		m_InfoHelpTxt.SetText("");
	}
	// tab SANG dung trang dang mo; xem bang khac thi khoa tab
	{
		int nTabOn = -1;
		if (nPage == defTONG_JX2_PAGE_INFO) nTabOn = 0;
		else if (nPage == TJX2_UI_PAGE_RECRUIT) nTabOn = 1;
		else if (nPage == defTONG_JX2_PAGE_WS) nTabOn = 2;
		else if (nPage == TJX2_UI_PAGE_FUNUSE) nTabOn = 3;
		else if (nPage == defTONG_JX2_PAGE_MEMBER || nPage == defTONG_JX2_PAGE_RIGHT) nTabOn = 4;
		else if (nPage == 4) nTabOn = 5;
		for (int t = 0; t < TJX2_UI_TABS; t++)
		{
			m_BtnTab[t].CheckButton(t == nTabOn ? 1 : 0);
			m_BtnTab[t].Enable(m_dwViewTong ? false : true);
			// [BH100] chua co bang: an 6 tab, chi con tab 'chieu mo bang khac' (BtnOtherZhaoMu)
			if (m_bNoTong) m_BtnTab[t].Hide(); else m_BtnTab[t].Show();
		}
		if (m_bNoTong) { m_BtnOtherZm.Show(); m_BtnOtherZm.Enable(true); m_BtnOtherZm.CheckButton(1); }
		else { m_BtnOtherZm.Hide(); m_BtnOtherZm.Enable(false); }
	}
	// nut phan trang danh sach + nut day
	{
		BOOL bPg = (nPage == defTONG_JX2_PAGE_MEMBER || nPage == defTONG_JX2_PAGE_RIGHT ||
			nPage == TJX2_UI_PAGE_FUNUSE || nPage == defTONG_JX2_PAGE_WS ||
			nPage == TJX2_UI_PAGE_TONGLIST || nPage == defTONG_JX2_PAGE_INFO);
		if (bPg) { m_BtnPrev.Show(); m_BtnPrev.Enable(true); m_BtnNext.Show(); m_BtnNext.Enable(true); }
		else { m_BtnPrev.Hide(); m_BtnPrev.Enable(false); m_BtnNext.Hide(); m_BtnNext.Enable(false); }
		// [BH100] nut Danh sach bang (BtnTongList) cua ban goc: mo/dong cua so danh sach
		BOOL bTL = (nPage != TJX2_UI_PAGE_OTHERZM && nPage != TJX2_UI_PAGE_TONGLIST);
		if (bTL) { m_BtnList.Show(); m_BtnList.Enable(true); } else { m_BtnList.Hide(); m_BtnList.Enable(false); }
		// m_Bot[2] (cung section BtnTongList) chi con dung o trang 2x2 / danh sach cu - an khi nut Danh sach bang hien
		if (bTL) { m_Bot[2].Hide(); m_Bot[2].Enable(false); } else { m_Bot[2].Show(); m_Bot[2].Enable(true); }
		if (m_bNoTong) { m_BtnHelp.Hide(); m_BtnHelp.Enable(false); } else { m_BtnHelp.Show(); m_BtnHelp.Enable(true); }
		// anh nut vao bang: 'Vao bon bang' / 'Vao bang khac' (ImageEnterMapSelf/Other)
		{
			char szImg[160];
			szImg[0] = 0;
			ReadIniString("BtnEnterMap", m_dwViewTong ? "ImageEnterMapOther" : "ImageEnterMapSelf", szImg, sizeof(szImg));
			if (szImg[0])
				m_Bot[0].SetImage(ISI_T_SPR, szImg, false);
		}
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

// [BH100] 14 nut quyen chia 2 trang con (SubPage= trong ini); radio 2 nut trang con
void KUiTongJX2::UpdateRightSub()
{
	int i;
	for (i = 0; i < 14; i++)
	{
		if (m_nRtPage[i] == m_nRtSub) { m_Rt[i].Show(); m_Rt[i].Enable(true); }
		else { m_Rt[i].Hide(); m_Rt[i].Enable(false); }
	}
	m_RtSub[0].CheckButton(m_nRtSub == 0 ? 1 : 0);
	m_RtSub[1].CheckButton(m_nRtSub == 1 ? 1 : 0);
	m_RtAll.CheckButton(0);
}

// [BH100 07/09] Theo dung FunUse refresh cua game_y.exe (0x4ea120):
//  - bLead = bang chu hoac truong lao: mat na ten trang con + nhom tien + nhom trang con chi hien khi bLead
//  - nhom trang con hien theo trang con dang chon (0/1/2/3); khoi ca nhan hien voi moi nguoi,
//    rieng Roi bang / Thoai an an voi bang chu (figure > 0)
//  - sau do BAT/TAT (Enable) theo quyen: 2001 nang cap; 3001 tien; 1901 duoi; 1902 ep thoai an;
//    1002 bo nhiem; 1004 danh hieu; 1003 doi phe; 2004 lanh dia; 2006 ky nang; 1101 lien minh;
//    chuyen vi chi bang chu; nut nhan su can co nguoi dang chon (khong phai ban than)
BOOL KUiTongJX2::HasMyRight(unsigned long dwRightID)
{
	if (!m_bHasInfo)
		return FALSE;
	TONG_JX2_INFO_SYNC* pI = (TONG_JX2_INFO_SYNC*)m_byInfo;
	if (pI->m_btMyFigure == 0)
		return TRUE;
	for (int r = 0; r < defTONG_JX2_RIGHT_COUNT; r++)
		if (s_dwRightId[r] == dwRightID)
			return (pI->m_wMyRights & (1 << r)) ? TRUE : FALSE;
	return FALSE;
}

void KUiTongJX2::UpdateFunButtons()
{
	BOOL bFun = (m_nPage == TJX2_UI_PAGE_FUNUSE);
	int nMyFig = 3;
	BOOL bIn = FALSE, bLead = FALSE;
	const char* pszSelf = "";
	if (m_bHasInfo)
	{
		TONG_JX2_INFO_SYNC* pI = (TONG_JX2_INFO_SYNC*)m_byInfo;
		nMyFig = pI->m_btMyFigure;
		bIn = (pI->m_dwUnionID != 0);
		bLead = (pI->m_bUnionLeader != 0);
		pszSelf = pI->m_szSelf;
	}
	BOOL bLead2 = (nMyFig <= 1);	// bang chu / truong lao
	TONG_JX2_MEMBER_SYNC* pM = (TONG_JX2_MEMBER_SYNC*)m_byMember;
	BOOL bSel = (m_nListMode == 0 && m_bHasMember && m_nSel >= 0 && m_nSel < (int)pM->m_btCount);
	BOOL bSelSelf = bSel && (strcmp(pM->m_sMember[m_nSel].m_szName, pszSelf) == 0);
	if (bFun && bLead2) m_FunMask.Show(); else m_FunMask.Hide();
	for (int i = 0; i < TJX2_FUN_BTNS; i++)
	{
		int nAct = s_sFunBtn[i].nAct;
		int nSub = s_nFunBtnSub[i];
		BOOL bShow = bFun && nAct >= 0 && (nSub == 0 || nSub == m_nFunSub);
		BOOL bEn = TRUE;
		if (bShow)
		{
			switch (nAct)
			{
			case 7: case 11: case 13:
				break;	// khoi ca nhan: ai cung thay
			case 8: case 30:
				bShow = (nMyFig != 0);	// exe: figure > 0 (bang chu phai chuyen vi truoc)
				break;
			case 14: bShow = bLead2 && !bIn; break;
			case 15: bShow = bLead2 && !bIn; break;
			case 16: bShow = bLead2 && bIn && bLead; break;
			case 17: bShow = bLead2 && bIn; break;
			case 18: bShow = bLead2 && bIn && bLead; break;
			default: bShow = bLead2; break;
			}
		}
		if (bShow)
		{
			switch (nAct)
			{
			case 0: bEn = HasMyRight(2001); break;
			case 1: case 9: case 10: case 21: case 22: bEn = HasMyRight(3001); break;
			case 2: bEn = HasMyRight(1901) && bSel && !bSelSelf; break;
			case 3: bEn = HasMyRight(1002) && bSel && !bSelSelf; break;
			case 25: bEn = HasMyRight(1902) && bSel && !bSelSelf; break;
			case 26: bEn = (nMyFig == 0) && bSel && !bSelSelf; break;
			case 27: bEn = (!bSel || bSelSelf) ? TRUE : HasMyRight(1004); break;
			case 28: case 29: bEn = HasMyRight(1004); break;
			case 12: bEn = HasMyRight(1003); break;
			case 5: case 23: bEn = HasMyRight(2004); break;
			case 24: bEn = HasMyRight(2006); break;
			case 14: case 15: case 16: case 17: case 18: bEn = HasMyRight(1101); break;
			case 19: case 20: bEn = (nMyFig == 0) || HasMyRight(2003); break;
			default: break;
			}
		}
		if (nAct == 30 && bShow)
		{
			// nhan Thoai an / Huy bo thoai an theo trang thai ban than (RetireLabel/UnRetireLabel)
			char szLb[48];
			m_FunBtn[i].SetLabel(GetIniString("Fun_BtnRetire", nMyFig == 4 ? "UnRetireLabel" : "RetireLabel", szLb, sizeof(szLb)));
		}
		if (bShow) { m_FunBtn[i].Show(); m_FunBtn[i].Enable(bEn ? true : false); }
		else { m_FunBtn[i].Hide(); m_FunBtn[i].Enable(false); }
	}
}

// doc mot chuoi tu UiTongJX2.ini (tra ve pszBuf; rong neu khong co)
const char* KUiTongJX2::GetIniString(const char* pszSection, const char* pszKey, char* pszBuf, int nSize)
{
	ReadIniString(pszSection, pszKey, pszBuf, nSize);
	return pszBuf;
}

void KUiTongJX2::ReadIniString(const char* pszSection, const char* pszKey, char* pszOut, int nSize)
{
	if (!pszOut || nSize <= 0)
		return;
	pszOut[0] = 0;
	KIniFile Ini;
	char szScheme[256], szPath[300];
	g_UiBase.GetCurSchemePath(szScheme, sizeof(szScheme));
	sprintf(szPath, "%s\\%s", szScheme, TONG_JX2_INI);
	if (Ini.Load(szPath))
		Ini.GetString(pszSection, pszKey, "", pszOut, nSize);
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

// [BH100] dinh dang ngan quy theo ban goc: < 1e8 so nguyen; < 1e12 '%.1f tram trieu'; con lai '%.1f van tram trieu'
static void sTJX2_FmtMoney(char* pszOut, __int64 nMoney)
{
	if (nMoney < 100000000)
		sprintf(pszOut, "%.0f", (double)nMoney);
	else if (nMoney < (__int64)1000000000000)
		sprintf(pszOut, "%.1f tr®m tri÷u", (double)nMoney / 100000000.0);
	else
		sprintf(pszOut, "%.1f vπn tr®m tri÷u", (double)nMoney / 1000000000000.0);
}

static unsigned int sTJX2_IniColor(const char* pszSec, const char* pszKey, unsigned int uDef)
{
	if (!KUiTongJX2::GetIfVisible())
		return uDef;
	char szC[48];
	szC[0] = 0;
	KIniFile Ini;
	char szScheme[256], szPath[300];
	g_UiBase.GetCurSchemePath(szScheme, sizeof(szScheme));
	sprintf(szPath, "%s\\%s", szScheme, TONG_JX2_INI);
	if (!Ini.Load(szPath))
		return uDef;
	Ini.GetString(pszSec, pszKey, "", szC, sizeof(szC));
	if (!szC[0])
		return uDef;
	int r = 0, g = 0, b = 0;
	if (sscanf(szC, "%d,%d,%d", &r, &g, &b) != 3)
		return uDef;
	return 0xFF000000 | ((r & 255) << 16) | ((g & 255) << 8) | (b & 255);
}

void KUiTongJX2::RenderInfo()
{
	if (!m_bHasInfo)
		return;
	TONG_JX2_INFO_SYNC* p = (TONG_JX2_INFO_SYNC*)m_byInfo;
	char sz[120];
	ClearRows();
	if (m_nPage != defTONG_JX2_PAGE_INFO)
		return;
	// [BH100 07/09] dung theo BaseInfo::Show cua game_y.exe: chuoi phe tu [PageBg0],
	// don vi ' van', ngan quy theo bac, chien bi DO khi thap hon chien bi bao tri tuan,
	// kien thiet VANG khi dat tran tuan + thanh tien do, cong hien ca nhan co thanh tien do.
	static unsigned int uTxt = 0, uWarn = 0, uThr = 0;
	static char szCamp[4][40];
	if (uTxt == 0)
	{
		uTxt = sTJX2_IniColor("PageBg0", "DefTxtColor", 0xFF000000 | (218 << 16) | (139 << 8) | 77);
		uWarn = sTJX2_IniColor("PageBg0", "DefWarnColor", 0xFFFF0000);
		uThr = sTJX2_IniColor("PageBg0", "DefThresholdColor", 0xFFFFFF00);
		szCamp[0][0] = 0;
		ReadIniString("PageBg0", "StringJustice", szCamp[1], 40);
		ReadIniString("PageBg0", "StringEvil", szCamp[2], 40);
		ReadIniString("PageBg0", "StringBalance", szCamp[3], 40);
	}
	m_Info[TJX2_INFO_TONGNAME].SetText(p->m_szTongName);
	m_Info[TJX2_INFO_MASTER].SetText(p->m_szMaster);
	m_Info[TJX2_INFO_LEAGUE].SetText(p->m_szUnionName[0] ? p->m_szUnionName : "");
	m_Info[TJX2_INFO_CAMP].SetText((p->m_btCamp >= 1 && p->m_btCamp <= 3) ? szCamp[p->m_btCamp] : "");
	if (p->m_btViewOther && p->m_nTongLevel == 0)
		strcpy(sz, "-");
	else
		sprintf(sz, "%d", p->m_nTongLevel);
	m_Info[TJX2_INFO_TONGLEVEL].SetText(sz);
	sprintf(sz, "%d", (int)p->m_wMemberTotal);
	m_Info[TJX2_INFO_MEMBERNUM].SetText(sz);
	sprintf(sz, "%d", p->m_nLevel);		// field 13 = cap KIEN THIET
	m_Info[TJX2_INFO_BUILDLEVEL].SetText(sz);
	sTJX2_FmtMoney(sz, p->m_nMoney);
	m_Info[TJX2_INFO_CAPITAL].SetTextColor(uTxt);
	m_Info[TJX2_INFO_CAPITAL].SetText(sz);
	{
		unsigned int uUp = p->m_dwWeekUpper;
		int nPct = uUp ? (int)((double)p->m_dwWeekBuild * 100.0 / (double)uUp) : 0;
		if (nPct > 100) nPct = 100;
		sprintf(sz, "%u vπn", p->m_dwBuildFund);
		m_Info[TJX2_INFO_BUILDFUND].SetTextColor((uUp && p->m_dwWeekBuild >= uUp) ? uThr : uTxt);
		m_Info[TJX2_INFO_BUILDFUND].SetText(sz);
		m_BarBuild.SetBar(nPct, (60 << 16) | (150 << 8) | 60);
	}
	sprintf(sz, "%u", p->m_dwStoredOffer);
	m_Info[TJX2_INFO_TOTALOFFER].SetText(sz);
	{
		int nPct = (int)((double)p->m_dwMyWeekOffer * 100.0 / 22400.0);
		if (nPct > 100) nPct = 100;
		sprintf(sz, "%u", p->m_dwMyOffer);
		m_Info[TJX2_INFO_MYOFFER].SetText(sz);
		m_BarOffer.SetBar(nPct, (60 << 16) | (150 << 8) | 60);
	}
	sprintf(sz, "%u", p->m_dwMyWeekOffer);
	m_Info[TJX2_INFO_WEEKOFFER].SetText(sz);
	sprintf(sz, "%u vπn", p->m_dwWarFund);
	m_Info[TJX2_INFO_WARFUND].SetTextColor((p->m_dwWarFund < p->m_dwStandFund) ? uWarn : uTxt);
	m_Info[TJX2_INFO_WARFUND].SetText(sz);
	sprintf(sz, "%u vπn", p->m_dwStoredBuild);
	m_Info[TJX2_INFO_STOREDBUILD].SetText(sz);
	sprintf(sz, "%u vπn", p->m_dwStandFund);
	m_Info[TJX2_INFO_STANDFUND].SetText(sz);
	if (m_bHasMember)
		RenderMembers();	// panel danh sach thanh vien ben phai
}

// [BH100 07/09] Panel thanh vien dung ban goc (MemberList 25 dong, MemberListRender):
// 3 cot Hang / Ten / gia tri theo menu sap xep; mau online/offline/dang chon; bam ten
// = tooltip 5 dong (ShowMemberTip), bam kep = menu nguoi choi. Server da sap xep.
void KUiTongJX2::RenderMembers(int nOffset)
{
	if (!m_bHasMember)
		return;
	TONG_JX2_MEMBER_SYNC* p = (TONG_JX2_MEMBER_SYNC*)m_byMember;
	char sz[120];
	ClearMemberRows();
	int i;
	int nCnt = (int)p->m_btCount;
	if (nCnt > defTONG_JX2_VIEW_MEMBERS)
		nCnt = defTONG_JX2_VIEW_MEMBERS;
	for (i = 0; i < TJX2_UI_ROWS; i++)
	{
		m_nOrd[i] = i;
		m_BtnRowSel[i].Enable(i - nOffset >= 0 && i - nOffset < nCnt);
	}
	for (i = 0; i < nCnt && i + nOffset < TJX2_UI_ROWS; i++)
	{
		TONG_JX2_ONE_MEMBER* pM = &p->m_sMember[i];
		char szV[48];
		switch (m_nSortMode)
		{
		case 1:
			{
				long nDays = 1;
				if (pM->m_dwJoinTime && (DWORD)time(NULL) > pM->m_dwJoinTime)
					nDays = (long)((time(NULL) - (time_t)pM->m_dwJoinTime) / 86400) + 1;
				sprintf(szV, "%.1f", (double)pM->m_dwOffer / (double)nDays);
			}
			break;
		case 2: sprintf(szV, "%u", pM->m_dwWeeklyOffer); break;
		case 3: sprintf(szV, "%u", pM->m_dwWeekOffer); break;
		case 4: strcpy(szV, pM->m_btFigure == 4 ? s_szFigure[4] : ""); break;
		case 5: strcpy(szV, pM->m_btFigure < 5 ? s_szFigure[pM->m_btFigure] : "?"); break;
		case 6:
			if (pM->m_dwLastActive)
			{
				time_t nTA = (time_t)pM->m_dwLastActive;
				struct tm* pTA = localtime(&nTA);
				sprintf(szV, "%02d/%02d/%02d", pTA->tm_year % 100, pTA->tm_mon + 1, pTA->tm_mday);
			}
			else
				strcpy(szV, "--/--/--");
			break;
		default: sprintf(szV, "%d", (int)pM->m_btLevel); break;
		}
		// MAU theo [Fun_MemberList]: OnlineColor 236,238,111 / OfflineColor 0,134,132 / SelColor 34,228,36
		unsigned int uCol;
		if (i == m_nSel)
			uCol = 0xFF000000 | (34 << 16) | (228 << 8) | 36;
		else if (pM->m_btOnline)
			uCol = 0xFF000000 | (236 << 16) | (238 << 8) | 111;
		else
			uCol = 0xFF000000 | (0 << 16) | (134 << 8) | 132;
		int r = i + nOffset;
		sprintf(sz, "%d", (int)p->m_wStart + i + 1);
		m_Row[r].SetTextColor(uCol);
		m_Row[r].SetText(sz);
		m_MList[r].SetTextColor(uCol);
		m_MList[r].SetText(pM->m_szName);
		m_RowDim[r].SetTextColor(uCol);
		m_RowDim[r].SetText(szV);
	}
	m_nMDetRows = 0;
	m_MShade.Hide();
	sprintf(sz, "%d", m_nStart / defTONG_JX2_VIEW_MEMBERS + 1);
	m_MPage.SetText(sz);
	LoadChecksFromSel();
	// Phan quyen chi giao duoc cho TRUONG LAO: nut Phan quyen chi sang khi chon truong lao
	if (m_nPage == defTONG_JX2_PAGE_MEMBER || m_nPage == defTONG_JX2_PAGE_RIGHT)
		m_RtApply.Enable(m_nSel < nCnt && p->m_sMember[m_nSel].m_btFigure == 1);
	if (m_nPage == TJX2_UI_PAGE_FUNUSE)
		UpdateFunButtons();	// [BH100] nut nhan su bat/tat theo nguoi dang chon (nhu exe)
}

// [BH100] tooltip thanh vien (MemberPanel WndProc LIST_ITEM_ACTIVE -> ShowMemberTip cua ban goc):
// <mau theo chuc vu>Danh hieu:<chuc vu>\ndanh hieu:<danh hieu ghe>\n\nDang cap hien tai / Diem
// cong hien hien tai / Thoi gian nhap bang (mau xanh la). Mau goc ffff33/00ffff/9966ff/999999/555555.
void KUiTongJX2::ShowMemberTip(int nMember)
{
	if (!m_bHasMember)
		return;
	TONG_JX2_MEMBER_SYNC* p = (TONG_JX2_MEMBER_SYNC*)m_byMember;
	if (nMember < 0 || nMember >= (int)p->m_btCount)
		return;
	TONG_JX2_ONE_MEMBER* pM = &p->m_sMember[nMember];
	static const char* szCol[5] = { "yellow", "cyan", "purple", "gray", "DBlue" };
	int nFig = pM->m_btFigure < 5 ? pM->m_btFigure : 3;
	char szDate[24];
	if (pM->m_dwJoinTime)
	{
		time_t nT = (time_t)pM->m_dwJoinTime;
		struct tm* pTm = localtime(&nT);
		sprintf(szDate, "%04d-%d-%d", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
	}
	else
		strcpy(szDate, "-");
	char szT[700];
	sprintf(szT,
		"<color=%s>Danh hi÷u:%s\ndanh hi÷u:%s\n<color>\n"
		"ßºng c p hi÷n tπi: <color=green>%d<color>\n"
		"ßi”m cËng hi’n hi÷n tπi: <color=green>%u<color>\n"
		"ThÍi gian nhÀp bang: <color=green>%s<color>\n",
		szCol[nFig], s_szFigure[nFig], pM->m_szTitle, (int)pM->m_btLevel, pM->m_dwOffer, szDate);
	int nLen = TEncodeText(szT, (int)strlen(szT));
	int x = 0, y = 0;
	Wnd_GetCursorPos(&x, &y);
	g_MouseOver.SetMouseHoverInfo(this, 0x7000 + nMember, x, y, true, false);
	g_MouseOver.SetMouseHoverDesc(szT, nLen, 0xFFFFFFFF);
	m_nTipMember = nMember;
}

void KUiTongJX2::HideMemberTip()
{
	if (m_nTipMember >= 0)
	{
		if (g_MouseOver.IsMoseHoverWndObj(this, 0x7000 + m_nTipMember))
			g_MouseOver.CancelMouseHoverInfo();
		m_nTipMember = -1;
	}
}

// [BH100] menu tha xuong dung KPopupMenu (nhu menu ban be cua UiChatCentre); tu xoa khi an
void KUiTongJX2::PopupMenu(int nKind, int nCount, const char* const* ppszItems)
{
	if (nCount <= 0 || !ppszItems)
		return;
	KPopupMenuData* pMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nCount));
	if (!pMenu)
		return;
	KPopupMenu::InitMenuData(pMenu, nCount);
	for (int i = 0; i < nCount; i++)
	{
		strncpy(pMenu->Items[i].szData, ppszItems[i] ? ppszItems[i] : "", 63);
		pMenu->Items[i].szData[63] = 0;
		pMenu->Items[i].uDataLen = (unsigned int)strlen(pMenu->Items[i].szData);
	}
	int x = 0, y = 0;
	Wnd_GetCursorPos(&x, &y);
	pMenu->nX = (short)x;
	pMenu->nY = (short)y;
	pMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	m_nMenuKind = nKind;
	KPopupMenu::Popup(pMenu, this, TJX2_MENU_ID + nKind);
}

// menu tu ini: cac khoa <pszKeyFmt><nFirst..nFirst+nCount-1> cua section (Item_0.. / MenuText1..)
void KUiTongJX2::PopupIniMenu(int nKind, const char* pszSection, const char* pszKeyFmt, int nCount)
{
	static char szBuf[12][64];
	const char* pp[12];
	if (nCount > 12)
		nCount = 12;
	KIniFile Ini;
	char szScheme[256], szPath[300], szKey[32];
	g_UiBase.GetCurSchemePath(szScheme, sizeof(szScheme));
	sprintf(szPath, "%s\\%s", szScheme, TONG_JX2_INI);
	if (!Ini.Load(szPath))
		return;
	int nFirst = (strncmp(pszKeyFmt, "MenuText", 8) == 0) ? 1 : 0;
	for (int i = 0; i < nCount; i++)
	{
		sprintf(szKey, "%s%d", pszKeyFmt, nFirst + i);
		szBuf[i][0] = 0;
		Ini.GetString(pszSection, szKey, "", szBuf[i], sizeof(szBuf[i]));
		pp[i] = szBuf[i];
	}
	PopupMenu(nKind, nCount, pp);
}

// [BH100] thanh tien do: to phan da dat (m_nPercent %) bang o mau alpha
void KTJX2Bar::PaintWindow()
{
	if (!g_pRepresentShell || m_nPercent <= 0)
		return;
	int nW = m_Width * (m_nPercent > 100 ? 100 : m_nPercent) / 100;
	if (nW <= 0)
		return;
	KRUShadow sBg;
	sBg.oPosition.nX = m_nAbsoluteLeft;
	sBg.oPosition.nY = m_nAbsoluteTop;
	sBg.oEndPos.nX = m_nAbsoluteLeft + nW;
	sBg.oEndPos.nY = m_nAbsoluteTop + m_Height;
	sBg.Color.Color_dw = (m_uColor & 0x00FFFFFF) | (((unsigned int)(255 - 140) << 21) & 0xff000000);
	g_pRepresentShell->DrawPrimitives(1, &sBg, RU_T_SHADOW, true);
}

// [BH100] xem bang khac: cua so chinh chi con trang Tin tuc (chi doc); 0 = ve bang minh
void KUiTongJX2::ViewTong(unsigned long dwTongID)
{
	if (!ms_pSelf)
		OpenWindow();
	if (!ms_pSelf)
		return;
	ms_pSelf->Show();
	ms_pSelf->m_dwViewTong = dwTongID;
	ms_pSelf->m_nStart = 0;
	ms_pSelf->m_nSel = 0;
	ms_pSelf->SwitchPage(defTONG_JX2_PAGE_INFO);
}

// Nap anh TEN trang con (1..4) cho m_FunMask. Section Fun_ImgSubPageMask
// cua ban thiet ke goc KHONG co khoa Image= ma co 4 khoa ImgSubPage1..4,
// nen phai tu doc theo trang dang chon - neu chi doc luc bam nut thi khi
// vua mo trang o do van TRONG (loi san phan bien bat).
void KUiTongJX2::LoadFunMaskImage()
{
	KIniFile Ini;
	char szScheme[256], szPath[300], szKey[24], szImg[128];
	int nSub = m_nFunSub;
	if (nSub < 1 || nSub > 4)
		nSub = 1;
	g_UiBase.GetCurSchemePath(szScheme, sizeof(szScheme));
	sprintf(szPath, "%s\\%s", szScheme, TONG_JX2_INI);
	if (!Ini.Load(szPath))
		return;
	sprintf(szKey, "ImgSubPage%d", nSub);
	Ini.GetString("Fun_ImgSubPageMask", szKey, "", szImg, sizeof(szImg));
	if (szImg[0])
		m_FunMask.SetImage(ISI_T_SPR, szImg, false);
}

// Nen mo cho panel chi tiet - cua so CON nam giua m_MList (ten) va m_MDet
// (chu) trong chuoi con nen dung thu tu: ten o duoi, nen mo o giua nhin
// xuyen duoc, chu panel o tren. Mau xanh dam alpha ~150/255.
void KTJX2Shade::PaintWindow()
{
	if (!g_pRepresentShell)
		return;
	KRUShadow sBg;
	sBg.oPosition.nX = m_nAbsoluteLeft;
	sBg.oPosition.nY = m_nAbsoluteTop;
	sBg.oEndPos.nX = m_nAbsoluteLeft + m_Width;
	sBg.oEndPos.nY = m_nAbsoluteTop + m_Height;
	// quy uoc alpha cua KWndMessageListBox: (255 - alpha) << 21
	sBg.Color.Color_dw = ((20 << 16) | (90 << 8) | 70) |
		(((unsigned int)(255 - 150) << 21) & 0xff000000);
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
			sprintf(szD, "Khu %d %s: c p %d [%s]  s∂n l≠Óng %u  c p dÔng %u",
				m_nSelWs, s_szWsName[m_nSelWs], (int)pW2->m_sWs[m_nSelWs].wLevel,
				pW2->m_sWs[m_nSelWs].btOpen ? "MO" : "DONG",
				pW2->m_sWs[m_nSelWs].dwOutput, pW2->m_sWs[m_nSelWs].dwUseLevel);
		else
			sprintf(szD, "Khu %d %s: (ch≠a lÀp)", m_nSelWs, s_szWsName[m_nSelWs]);
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
		UIMessageBox("Chÿ c„ th” ph©n quy“n cho Tr≠Îng L∑o.",
			this, "ß„ng", NULL, 0);
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
		// m_nRecStart giu nguyen - phan trang that (8 don/trang, relay 64)
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
// [BH100 07/09] don vi ' van', ngan quy theo bac, chien bi do khi thieu, thanh tien do kien thiet
// tuan + cong hien tuan ca nhan; panel phai = thanh vien hoac bang lien minh (trang con 3)
void KUiTongJX2::RenderFunUse()
{
	if (!m_bHasInfo)
		return;
	TONG_JX2_INFO_SYNC* p = (TONG_JX2_INFO_SYNC*)m_byInfo;
	char sz[120];
	static unsigned int uTxt = 0, uWarn = 0, uThr = 0;
	if (uTxt == 0)
	{
		uTxt = sTJX2_IniColor("Fun_PageBg", "DefTxtColor", 0xFF000000 | (218 << 16) | (139 << 8) | 77);
		uWarn = sTJX2_IniColor("Fun_PageBg", "DefWarnColor", 0xFFFF0000);
		uThr = sTJX2_IniColor("Fun_PageBg", "DefThresholdColor", 0xFFFFFF00);
	}
	m_FunTxt[2].SetText(p->m_szTongName);
	m_FunTxt[4].SetText(p->m_szUnionName[0] ? p->m_szUnionName : "-");
	sprintf(sz, "%d", p->m_nLevel);
	m_FunTxt[6].SetText(sz);	// Dang cap kien thiet
	sprintf(sz, "%u", p->m_dwStoredOffer);
	m_FunTxt[8].SetText(sz);
	sTJX2_FmtMoney(sz, p->m_nMoney);
	m_FunTxt[10].SetText(sz);
	{
		unsigned int uUp = p->m_dwWeekUpper;
		int nPct = uUp ? (int)((double)p->m_dwWeekBuild * 100.0 / (double)uUp) : 0;
		if (nPct > 100) nPct = 100;
		sprintf(sz, "%u vπn", p->m_dwBuildFund);
		m_FunTxt[12].SetTextColor((uUp && p->m_dwWeekBuild >= uUp) ? uThr : uTxt);
		m_FunTxt[12].SetText(sz);
		m_BarFunBuild.SetBar(nPct, (60 << 16) | (150 << 8) | 60);
	}
	sprintf(sz, "%u vπn", p->m_dwWarFund);
	m_FunTxt[14].SetTextColor((p->m_dwWarFund < p->m_dwStandFund) ? uWarn : uTxt);
	m_FunTxt[14].SetText(sz);
	sprintf(sz, "%u vπn", p->m_dwStoredBuild);
	m_FunTxt[16].SetText(sz);
	{
		int nPct = (int)((double)p->m_dwMyWeekOffer * 100.0 / 22400.0);
		if (nPct > 100) nPct = 100;
		sprintf(sz, "%u", p->m_dwMyOffer);
		m_FunP[2].SetText(sz);
		m_BarFunOffer.SetBar(nPct, (60 << 16) | (150 << 8) | 60);
	}
	sTJX2_FmtMoney(sz, p->m_nMoney);
	m_FunP[4].SetText(sz);
	sprintf(sz, "%u vπn", p->m_dwBuildFund);
	m_FunP[6].SetText(sz);
	UpdateFunButtons();
	if (m_nListMode == 2)
	{
		if (m_bHasUnion)
			RenderUnionList();
	}
	else if (m_nListMode == 1)
	{
		if (m_bHasList)
			RenderTongListPanel();
	}
	else if (m_bHasMember)
		RenderMembers();
}

// [BH100] panel phai o trang con Lien minh: danh sach bang trong lien minh (UnionListRender
// ban goc): Hang / ten bang / ' Minh Chu ' hoac 'Lien minh bang hoi'; sap xep theo menu
// Fun_BtnTongSortMenu (0 cap / 1 nhan so / 2 cap / 3 minh chu truoc / 4 so luong).
void KUiTongJX2::RenderUnionList()
{
	if (!m_bHasUnion)
		return;
	TONG_JX2_TONGLIST_SYNC* p = (TONG_JX2_TONGLIST_SYNC*)m_byUnion;
	char sz[120];
	ClearMemberRows();
	int i, nCnt = (int)p->m_btCount;
	if (nCnt > defTONG_JX2_LIST_ROWS)
		nCnt = defTONG_JX2_LIST_ROWS;
	for (i = 0; i < nCnt; i++)
		m_nOrd[i] = i;
	for (int a = 1; a < nCnt; a++)
	{
		int nKey = m_nOrd[a], b;
		for (b = a - 1; b >= 0; b--)
		{
			TONG_JX2_ONE_TONG* p1 = &p->m_sTong[m_nOrd[b]];
			TONG_JX2_ONE_TONG* p2 = &p->m_sTong[nKey];
			int nCmp = 0;
			switch (m_nTongSort)
			{
			case 1: case 4: nCmp = (int)p2->m_wMember - (int)p1->m_wMember; break;
			case 3: nCmp = (int)p2->m_btUnionLeader - (int)p1->m_btUnionLeader; break;
			default: nCmp = (int)p2->m_btLevel - (int)p1->m_btLevel; break;
			}
			if (nCmp <= 0)
				break;
			m_nOrd[b + 1] = m_nOrd[b];
		}
		m_nOrd[b + 1] = nKey;
	}
	for (i = 0; i < TJX2_UI_ROWS; i++)
		m_BtnRowSel[i].Enable(i < nCnt);
	for (i = 0; i < nCnt; i++)
	{
		TONG_JX2_ONE_TONG* pT = &p->m_sTong[m_nOrd[i]];
		unsigned int uCol = (m_nOrd[i] == m_nSel) ?
			(0xFF000000 | (34 << 16) | (228 << 8) | 36) : (0xFF000000 | (236 << 16) | (238 << 8) | 111);
		sprintf(sz, "%d", (int)p->m_wStart + i + 1);
		m_Row[i].SetTextColor(uCol);
		m_Row[i].SetText(sz);
		m_MList[i].SetTextColor(uCol);
		m_MList[i].SetText(pT->m_szName);
		m_RowDim[i].SetTextColor(uCol);
		m_RowDim[i].SetText(pT->m_btUnionLeader ? " Minh ChÒ " : "Li™n minh bang hÈi");
	}
	sprintf(sz, "%d", m_nStart / defTONG_JX2_LIST_ROWS + 1);
	m_MPage.SetText(sz);
}

// Trang Nhat ky (RecordShow game_y.exe): muc 0 = bao cao muc tieu tuan; muc 1 = thong bao
// (doc trong khung, bam Hieu chinh moi mo khung sua, LeaveWord = gui); muc 2/3 = so su kien /
// lich su (ring 16 dong, moi dong da co 'YYYY-MM-DD: ' tu relay).
void KUiTongJX2::RenderRecord()
{
	int i;
	ClearRows();
	if (m_nRcSub == 0)
	{
		m_RcEditor.Hide();
		m_RcEditor.Enable(false);
		if (m_bHasWeek)
			RenderWeekGoal();
		else
		{
			m_RcList.Show();
			m_RcScroll.Show();
			m_RcList.Clear();
		}
		return;
	}
	if (m_nRcSub == 1)
	{
		const char* pszAnn = "";
		if (m_bHasRecord)
			pszAnn = ((TONG_JX2_RECORD_SYNC*)m_byRecord)->m_szAnnounce;
		else if (m_bHasInfo)
			pszAnn = ((TONG_JX2_INFO_SYNC*)m_byInfo)->m_szAnnounce;
		if (m_bRcEdit)
		{
			m_RcList.Hide();
			m_RcScroll.Hide();
			m_RcEditor.Show();
			m_RcEditor.Enable(true);
			return;
		}
		m_RcEditor.Hide();
		m_RcEditor.Enable(false);
		m_RcList.Show();
		m_RcScroll.Show();
		m_RcList.Clear();
		if (pszAnn[0])
			m_RcList.AddOneMessage(pszAnn, (int)strlen(pszAnn));
		else
		{
			const char* szNone = "(ch≠a c„ th´ng b∏o)";
			m_RcList.AddOneMessage(szNone, (int)strlen(szNone));
		}
		return;
	}
	m_RcEditor.Hide();
	m_RcEditor.Enable(false);
	if (!m_bHasRecord)
		return;
	TONG_JX2_RECORD_SYNC* p = (TONG_JX2_RECORD_SYNC*)m_byRecord;
	m_RcList.Show();
	m_RcScroll.Show();
	m_RcList.Clear();
	if (p->m_btCount == 0)
	{
		const char* szNone = "(ch≠a c„ b∂n ghi)";
		m_RcList.AddOneMessage(szNone, (int)strlen(szNone));
	}
	for (i = 0; i < (int)p->m_btCount && i < defTONG_JX2_RECORD_LINES; i++)
		m_RcList.AddOneMessage(p->m_szLine[i], (int)strlen(p->m_szLine[i]));
}

static void sTJX2_AddLine(KWndMessageListBox& list, const char* pszText)
{
	char szT[400];
	strncpy(szT, pszText, sizeof(szT) - 1);
	szT[sizeof(szT) - 1] = 0;
	int nLen = TEncodeText(szT, (int)strlen(szT));
	list.AddOneMessage(szT, nLen);
}

// [BH100] bao cao muc tieu tuan - chuoi nguyen van stringtable_client.txt:465-473 cua ban
// Linux (G_STR_TONG_DATE_DESC / WEEKGOAL_LEVEL / TONG_WEEKLY_AIM / PERSONAL_WEEKLY_AIM /
// PROCESS_STATUS / TONG_PRIZE), tieu de tuan tu [PageBg4] CurWeekDesc/LastWeekDesc,
// ten su kien tu [Rc_EventType]. Tab cua ban goc tach thanh 2 dong.
void KUiTongJX2::RenderWeekGoal()
{
	if (!m_bHasWeek)
		return;
	TONG_JX2_WEEKGOAL_SYNC* w = (TONG_JX2_WEEKGOAL_SYNC*)m_byWeek;
	m_RcList.Show();
	m_RcScroll.Show();
	m_RcList.Clear();
	char sz[400], szDesc[64], szEv[64], szKey[8];
	const char* szFin = "ß∑ hoµn thµnh";
	const char* szUnf = "Ch≠a hoµn thµnh";
	sprintf(sz, "Bang hÈi lÀp Æ≠Óc <color=gold>%u<color> tu«n vµ <color=white>%u<color> ngµy", (unsigned)w->m_nWeek, (unsigned)(8 - w->m_nDaysLeft));
	sTJX2_AddLine(m_RcList, sz);
	sprintf(sz, "ßÈ kh„ mÙc ti™u tu«n: <color=green>%d <color>c p", (int)w->m_dwCurLevel);
	sTJX2_AddLine(m_RcList, sz);
	sTJX2_AddLine(m_RcList, " ");
	for (int k = 0; k < 2; k++)
	{
		DWORD dwEvent = k ? w->m_dwLEvent : w->m_dwEvent;
		DWORD dwTotal = k ? w->m_dwLTotal : w->m_dwTotal;
		DWORD dwValue = k ? w->m_dwLValue : w->m_dwValue;
		DWORD dwPlayer = k ? w->m_dwLPlayer : w->m_dwPlayer;
		DWORD dwMine = k ? w->m_dwMyLWeekGoal : w->m_dwMyWeekGoal;
		DWORD dwPT = k ? w->m_dwLPriceTong : w->m_dwPriceTong;
		DWORD dwPP = k ? w->m_dwLPricePlayer : w->m_dwPricePlayer;
		ReadIniString("PageBg4", k ? "LastWeekDesc" : "CurWeekDesc", szDesc, sizeof(szDesc));
		sprintf(szKey, "%u", (unsigned)dwEvent);
		ReadIniString("Rc_EventType", szKey, szEv, sizeof(szEv));
		if (!szEv[0])
			ReadIniString("Rc_EventType", "0", szEv, sizeof(szEv));
		sprintf(sz, "%s:---------------------<color=green>%s<color>----------------------", szDesc, szEv);
		sTJX2_AddLine(m_RcList, sz);
		sprintf(sz, "Bang hÈi c«n hoµn thµnh Æi”m cËng hi’n: %u", (unsigned)dwTotal);
		sTJX2_AddLine(m_RcList, sz);
		sprintf(sz, "Bang hÈi Æ∑ hoµn thµnh Æi”m cËng hi’n: %u", (unsigned)dwValue);
		sTJX2_AddLine(m_RcList, sz);
		sprintf(sz, "C∏ nh©n c«n hoµn thµnh Æi”m cËng hi’n: %u", (unsigned)dwPlayer);
		sTJX2_AddLine(m_RcList, sz);
		sprintf(sz, "C∏ nh©n Æ∑ hoµn thµnh Æi”m cËng hi’n: %u", (unsigned)dwMine);
		sTJX2_AddLine(m_RcList, sz);
		BOOL bTong = k ? (w->m_btLComplete != 0) : (dwTotal > 0 && dwValue >= dwTotal);
		BOOL bMe = (dwPlayer > 0 && dwMine >= dwPlayer);
		sprintf(sz, "SË ngµy cﬂn lπi: %d", k ? 0 : w->m_nDaysLeft);
		sTJX2_AddLine(m_RcList, sz);
		sprintf(sz, "Bang hÈi: <%s> C∏ nh©n: <%s>", bTong ? szFin : szUnf, bMe ? szFin : szUnf);
		sTJX2_AddLine(m_RcList, sz);
		sprintf(sz, "Ph«n th≠Îng bang hÈi: %d vπn ng©n s∏ch chi’n bﬁ", (int)dwPT);
		sTJX2_AddLine(m_RcList, sz);
		sprintf(sz, " Ph«n th≠Îng c∏ nh©n: %d Æi”m cËng hi’n %d vπn kinh nghi÷m", (int)dwPP, (int)dwPP);
		sTJX2_AddLine(m_RcList, sz);
		sTJX2_AddLine(m_RcList, " ");
	}
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
	m_Row[0].SetText("T™n bang");
	m_MList[0].SetText("Bang chÒ");
	m_RowDim[0].SetText("C p / Thµnh vi™n");
	if (!m_bHasList)
		return;
	TONG_JX2_TONGLIST_SYNC* p = (TONG_JX2_TONGLIST_SYNC*)m_byList;
	if (p->m_btCount == 0)
	{
		m_Row[2].SetText("(ch≠a c„ bang hÈi nµo)");
		return;
	}
	for (i = 0; i < (int)p->m_btCount && i < defTONG_JX2_LIST_ROWS && i + 1 < TJX2_UI_ROWS; i++)
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

// [BH100] panel phai o trang con 2 (exe: SetMode(1) = danh sach TOAN BO bang, 25/trang): Hang / ten
// bang / gia tri theo menu Fun_BtnTongSortMenu (0 cap / 1 nhan so / 2 cap / 3 lien minh / 4 so luong)
void KUiTongJX2::RenderTongListPanel()
{
	if (!m_bHasList)
		return;
	TONG_JX2_TONGLIST_SYNC* p = (TONG_JX2_TONGLIST_SYNC*)m_byList;
	char sz[120];
	ClearMemberRows();
	int i, nCnt = (int)p->m_btCount;
	if (nCnt > defTONG_JX2_LIST_ROWS)
		nCnt = defTONG_JX2_LIST_ROWS;
	for (i = 0; i < nCnt; i++)
		m_nOrd[i] = i;
	for (int a = 1; a < nCnt; a++)
	{
		int nKey = m_nOrd[a], b;
		for (b = a - 1; b >= 0; b--)
		{
			TONG_JX2_ONE_TONG* p1 = &p->m_sTong[m_nOrd[b]];
			TONG_JX2_ONE_TONG* p2 = &p->m_sTong[nKey];
			int nCmp = 0;
			switch (m_nTongSort)
			{
			case 1: case 4: nCmp = (int)p2->m_wMember - (int)p1->m_wMember; break;
			case 3: nCmp = (int)p2->m_btUnionLeader - (int)p1->m_btUnionLeader; break;
			default: nCmp = (int)p2->m_btLevel - (int)p1->m_btLevel; break;
			}
			if (nCmp <= 0)
				break;
			m_nOrd[b + 1] = m_nOrd[b];
		}
		m_nOrd[b + 1] = nKey;
	}
	for (i = 0; i < TJX2_UI_ROWS; i++)
		m_BtnRowSel[i].Enable(i < nCnt);
	for (i = 0; i < nCnt; i++)
	{
		TONG_JX2_ONE_TONG* pT = &p->m_sTong[m_nOrd[i]];
		unsigned int uCol = (m_nOrd[i] == m_nSel) ?
			(0xFF000000 | (34 << 16) | (228 << 8) | 36) : (0xFF000000 | (236 << 16) | (238 << 8) | 111);
		sprintf(sz, "%d", (int)p->m_wStart + i + 1);
		m_Row[i].SetTextColor(uCol);
		m_Row[i].SetText(sz);
		m_MList[i].SetTextColor(uCol);
		m_MList[i].SetText(pT->m_szName);
		switch (m_nTongSort)
		{
		case 1: case 4: sprintf(sz, "%d", (int)pT->m_wMember); break;
		case 3: strcpy(sz, pT->m_btUnionLeader ? " Minh ChÒ " : "-"); break;
		default: sprintf(sz, "%d", (int)pT->m_btLevel); break;
		}
		m_RowDim[i].SetTextColor(uCol);
		m_RowDim[i].SetText(sz);
	}
	sprintf(sz, "%d", m_nStart / defTONG_JX2_LIST_ROWS + 1);
	m_MPage.SetText(sz);
}

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
			sprintf(sz, "%dc p  %d ng≠Íi", (int)pZ->m_btLevel, (int)pZ->m_wMember);
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
		if (uParam == TJX2_TITLE_ID)
		{
			// [BH100] danh hieu ghe (BtnChangeTitle): m_dwAmtTarget = nguoi duoc doi (0 = ban than)
			if (nParam && ((const char*)nParam)[0])
				SendOp(defTONG_JX2_COP_SET_TITLE, m_dwAmtTarget, 0, 0, (const char*)nParam);
			m_nAmtOp = -1;
			return 1;
		}
		if (uParam == TJX2_SEXTITLE_ID)
		{
			if (nParam && ((const char*)nParam)[0])
				SendOp(defTONG_JX2_COP_SET_SEX_TITLE, 0, m_nSexTitle, 0, (const char*)nParam);
			m_nAmtOp = -1;
			return 1;
		}
		if (uParam == TJX2_WORD_ID)
		{
			if (nParam && ((const char*)nParam)[0])
				SendOp(defTONG_JX2_COP_LEAVE_WORD, 0, 0, 0, (const char*)nParam);
			return 1;
		}
		if (uParam == TJX2_RIGHTS_ID)
		{
			if (nParam)
				ApplyRights();
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

	case WND_M_MENUITEM_SELECTED:
		// [BH100] menu tha xuong: HIWORD = TJX2_MENU_ID + loai, LOWORD = muc (-1 = huy)
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			int nKind = (int)HIWORD(nParam) - TJX2_MENU_ID;
			int nItem = (short)LOWORD(nParam);
			m_nMenuKind = 0;
			if (nItem < 0)
				return 1;
			switch (nKind)
			{
			case 1:	// sap xep thanh vien (server sap xep -> xin lai trang)
				m_nSortMode = nItem % 7;
				m_nStart = 0;
				m_nSel = 0;
				HideMemberTip();
				RequestPage(m_nPage, 0);
				break;
			case 2:	// sap xep danh sach bang lien minh (client)
				m_nTongSort = nItem % 5;
				{
					char szH[64], szKey[16];
					sprintf(szKey, "Item_%d", m_nTongSort);
					if (m_nListMode == 1)
						m_ColHdr[2].SetText(GetIniString("Fun_BtnTongSortMenu", szKey, szH, sizeof(szH)));
				}
				if (m_nListMode == 2 && m_bHasUnion)
					RenderUnionList();
				else if (m_nListMode == 1 && m_bHasList)
					RenderTongListPanel();
				break;
			case 3:	// khuynh huong chieu mo (menu 1..6 -> gia tri 1..6)
				m_nRecQX = (nItem + 1) % TJX2_QX_NUM;
				m_RecQX.SetLabel(s_szRecQX[m_nRecQX]);
				break;
			case 4: case 5: case 6: case 7:	// 4 hoat dong chu yeu
				m_nRecHD[nKind - 4] = (nItem + 1) % TJX2_HD_NUM;
				m_RecHD[nKind - 4].SetLabel(s_szRecHD[m_nRecHD[nKind - 4]]);
				break;
			case 8:	// menu nguoi choi (bam kep ten): 0 = mat dam
				if (m_bHasMember && nItem == 0)
				{
					TONG_JX2_MEMBER_SYNC* pM = (TONG_JX2_MEMBER_SYNC*)m_byMember;
					if (m_nSel >= 0 && m_nSel < (int)pM->m_btCount)
						KUiPlayerBar::InputNameMsg(false, pM->m_sMember[m_nSel].m_szName, true);
				}
				break;
			}
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
			// [BH100] 6 tab ban goc: Tin tuc / Chieu mo / Tac phuong / Su dung chuc nang / Phan phoi / Nhat ky
			static const int s_nTabPage[TJX2_UI_TABS] =
			{
				defTONG_JX2_PAGE_INFO, TJX2_UI_PAGE_RECRUIT, defTONG_JX2_PAGE_WS,
				TJX2_UI_PAGE_FUNUSE, defTONG_JX2_PAGE_MEMBER, 4,
			};
			for (i = 0; i < TJX2_UI_TABS; i++)
			{
				if (uParam == (unsigned int)&m_BtnTab[i])
				{
					if (m_dwViewTong)
						return 1;	// dang xem bang khac: khoa tab
					m_nFunMode = 1;
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
			// bam vao dong: chon thanh vien (tooltip) / bam kep = menu nguoi choi / don xin / bang
			for (i = 0; i < TJX2_UI_ROWS; i++)
			{
				if (uParam == (unsigned int)&m_BtnRowSel[i])
				{
					if (m_nPage == defTONG_JX2_PAGE_MEMBER || m_nPage == defTONG_JX2_PAGE_RIGHT ||
						m_nPage == TJX2_UI_PAGE_FUNUSE ||
						m_nPage == defTONG_JX2_PAGE_INFO ||
						m_nPage == defTONG_JX2_PAGE_WS)
					{
						if (m_nListMode >= 1)
						{
							// panel danh sach bang (toan bo / lien minh): chon dong, khong co tooltip
							m_nSel = (i < TJX2_UI_ROWS) ? m_nOrd[i] : i;
							if (m_nListMode == 2 && m_bHasUnion)
								RenderUnionList();
							else if (m_nListMode == 1 && m_bHasList)
								RenderTongListPanel();
							return 1;
						}
						// [BH100] LIST_ITEM_ACTIVE cua ban goc = bam ten: chon + tooltip; bam kep
						// (2 lan trong 400 ms) = menu nguoi choi; bam lai dong dang chon = tat tooltip
						int nM = i;
						unsigned long dwNow = GetTickCount();
						BOOL bDbl = (m_nLastRowClick == nM && dwNow - m_dwLastRowClick < 400);
						m_dwLastRowClick = dwNow;
						m_nLastRowClick = nM;
						if (bDbl)
						{
							HideMemberTip();
							m_nSel = nM;
							RenderMembers();
							static const char* szPM[1] = { "MÀt Æµm" };
							PopupMenu(8, 1, szPM);
							return 1;
						}
						if (m_nSel == nM && m_nTipMember == nM)
							HideMemberTip();
						else
						{
							m_nSel = nM;
							ShowMemberTip(nM);
						}
						RenderMembers();
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
		if (uParam == (unsigned int)&m_BtnHelp)
		{
			KUiTongHelpJX2::OpenWindow();	// [BH100] BtnTongHelp goc
			return 1;
		}
		if (uParam == (unsigned int)&m_BtnOtherZm)
		{
			m_BtnOtherZm.CheckButton(1);
			SwitchPage(TJX2_UI_PAGE_OTHERZM);
			return 1;
		}
		if (uParam == (unsigned int)&m_BtnList)
		{
			// [BH100] BtnTongList cua ban goc: mo/dong cua so danh sach bang (chon bang -> xem)
			if (KUiTongListJX2::GetIfVisible())
			{
				KUiTongListJX2::CloseWindow(false);
				if (m_dwViewTong)
					ViewTong(0);
			}
			else
				KUiTongListJX2::OpenWindow();
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
				// nut co CheckBox=1 nen da TU LAT truoc khi bao len; [BH100] chi bat/tat
				// cac o cua TRANG CON dang mo (BtnSelectAll ban goc)
				int nAll = m_RtAll.IsButtonChecked() ? 1 : 0;
				for (q = 0; q < 14; q++)
					if (m_nRtPage[q] == m_nRtSub)
						m_Rt[q].CheckButton(nAll);
				return 1;
			}
			for (q = 0; q < 2; q++)
			{
				if (uParam == (unsigned int)&m_RtSub[q])
				{
					// [BH100] 2 trang con Quyen han / Phan phat (radio)
					m_nRtSub = q;
					UpdateRightSub();
					LoadChecksFromSel();
					return 1;
				}
			}
			if (uParam == (unsigned int)&m_RtApply)
			{
				// [BH100] hoi truoc nhu ban goc: [PageBg2] PromptInfo 'Xac nhan muon giao quyen nay cho %s?'
				TONG_JX2_MEMBER_SYNC* pRM = (TONG_JX2_MEMBER_SYNC*)m_byMember;
				if (!m_bHasMember || m_nSel >= (int)pRM->m_btCount)
					return 1;
				if (pRM->m_sMember[m_nSel].m_btFigure != 1)
				{
					UIMessageBox("Chÿ c„ th” ph©n quy“n cho Tr≠Îng L∑o.", this, "ß„ng", NULL, 0);
					return 1;
				}
				char szFmt[200], szMsg[256];
				ReadIniString("PageBg2", "PromptInfo", szFmt, sizeof(szFmt));
				if (!szFmt[0])
					strcpy(szFmt, "X∏c nhÀn muËn giao quy“n nµy cho %s?");
				sprintf(szMsg, szFmt, pRM->m_sMember[m_nSel].m_szName);
				UIMessageBox(szMsg, this, "X∏c nhÀn", "Hu˚ b·", TJX2_RIGHTS_ID);
				return 1;
			}
			for (q = 0; q < 4; q++)
			{
				if (uParam == (unsigned int)&m_FunSub[q])
				{
					m_nFunSub = q + 1;
					m_nListMode = (m_nFunSub == 2) ? 1 : (m_nFunSub == 3) ? 2 : 0;	// [BH100] exe: 0/1/2/0 = thanh vien / toan bo bang / bang lien minh
					LoadFunMaskImage();
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
					// [BtnEnterMap]: VAO BON BANG; [BH100] dang xem bang khac = 'Vao bang khac'
					SendOp(defTONG_JX2_COP_ENTER_MAP, m_dwViewTong, 0, 0, NULL);
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
				if (m_dwViewTong)
				{
					KUiTongListJX2::CloseWindow(false);
					ViewTong(0);	// [BH100] dang xem bang khac: Dong = ve bang minh (Esc ban goc)
					return 1;
				}
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
					// cua so PHAT theo chuc vu cua ban goc (truoc day hardcode
					// phat 10 diem cho 1 nguoi dang chon - khong co hop thoai)
					KUiTongGrant::OpenWindow(false);
					break;
				case 2:
					// duoi nguoi khoi bang - khong hoan tac duoc, phai hoi truoc
					if (dwFT)
						AskThenSendOp(NULL, NULL, defTONG_JX2_COP_KICK, dwFT, 0, 0);
					break;
				case 3:
					// mo cua so BO NHIEM chuan goc (blueprint da port san o
					// UiTongAssignBox0.ini): chon 1 trong 3 chuc. Truoc day nut
					// nay chi HA xuong bang chung - khong co loi len chuc.
					if (dwFT && m_bHasMember)
					{
						TONG_JX2_MEMBER_SYNC* pAB = (TONG_JX2_MEMBER_SYNC*)m_byMember;
						if (m_nSel < (int)pAB->m_btCount)
						{
							KUiTongAssignBox::OpenWindow();
							KUiTongAssignBox::ArrangeDataJX2(
								pAB->m_sMember[m_nSel].m_szName, dwFT,
								(int)pAB->m_sMember[m_nSel].m_btFigure);
						}
					}
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
					KUiTongGetString::OpenWindow("ßi”m cËng hi’n muËn gˆi vµo",	// G_STR_STORE_PERSONAL_OFFER
						"100", this, TJX2_AMOUNT_ID, 1, 7);
					break;
				case 11:
					// gop tien ca nhan vao QUY KIEN THIET (nhan cong hien)
					m_nAmtOp = defTONG_JX2_COP_DONATE;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("Ng©n l≠Óng muËn gˆi(Æ¨n vﬁ: vπn)",	// G_STR_BUILD_FUND_TITLE
						"100", this, TJX2_AMOUNT_ID, 1, 7);
					break;
				case 14:
					// lap lien minh: nhap ten (<= 20 ky tu de vua hop nhap)
					m_nAmtOp = defTONG_JX2_COP_UNION_CREATE;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("T™n li™n minh",
						"", this, TJX2_UNAME_ID, 1, 31);
					break;
				case 15:
					// xin vao lien minh: nhap ten mot bang thuoc lien minh
					m_nAmtOp = defTONG_JX2_COP_UNION_APPLY;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("T™n bang thuÈc li™n minh",
						"", this, TJX2_UNAME_ID, 1, 31);
					break;
				case 16:
					// minh chu duyet: nhap ten bang xin vao
					m_nAmtOp = defTONG_JX2_COP_UNION_ACCEPT;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("T™n bang xin gia nhÀp",
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
					KUiTongGetString::OpenWindow("T™n bang muËn ÆuÊi",
						"", this, TJX2_UNAME_ID, 1, 31);
					break;
				case 19:
					// [BH100] cua so UY NHIEM DAI THAN cua ban goc (cung khung 'Nhiem menh chuc vi'):
					// 3 o chon Thua Tuong / Nguyen Soai / Tien Phong cho thanh vien dang chon
					if (dwFT && m_bHasMember)
					{
						KUiTongAssignBox::OpenWindow();
						KUiTongAssignBox::ArrangeDataMinister(pFM->m_sMember[m_nSel].m_szName, dwFT);
					}
					break;
				case 21:
					// [BH100] ngan quy -> ngan sach kien thiet (G_STR_INPUT_TONG_MONEY)
					m_nAmtOp = defTONG_JX2_COP_TRANSFORM_MONEY;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("Ng©n l≠Óng muËn chuy”n h„a(Æ¨n vﬁ: vπn)", "100", this, TJX2_AMOUNT_ID, 1, 7);
					break;
				case 22:
					// [BH100] kien thiet -> chien bi (G_STR_TRANSFORM_BUILD_FUND)
					m_nAmtOp = defTONG_JX2_COP_TRANSFORM_BUILD;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("Ng©n l≠Óng chuy”n h„a (Æ¨n vﬁ: vπn)", "100", this, TJX2_AMOUNT_ID, 1, 7);
					break;
				case 23:
					SendOp(defTONG_JX2_COP_MAP_MANAGE, 0, 0, 0, NULL);	// hop thoai quan ly lanh dia
					break;
				case 24:
					SendOp(defTONG_JX2_COP_STUNT_NPC, 0, 0, 0, NULL);	// hop thoai cot bieu tuong
					break;
				case 25:
					// [BH100] ep thoai an thanh vien dang chon (StrMemberOrderToRetireInfo)
					if (dwFT && m_bHasMember)
					{
						char szFmt[200], szMsg[256];
						ReadIniString("Fun_PageBg", "StrMemberOrderToRetireInfo", szFmt, sizeof(szFmt));
						if (!szFmt[0]) strcpy(szFmt, "X∏c nhÀn muËn cho %s tho∏i »n?");
						sprintf(szMsg, szFmt, pFM->m_sMember[m_nSel].m_szName);
						m_nPendOp = defTONG_JX2_COP_FORCE_RETIRE;
						m_dwPendTarget = dwFT;
						m_nPendP1 = 1; m_nPendP2 = 0;
						UIMessageBox(szMsg, this, "X∏c nhÀn", "Hu˚ b·", TJX2_CONFIRM_ID);
					}
					break;
				case 26:
					// [BH100] chuyen ngoi bang chu cho thanh vien dang chon (StrMemberDemise)
					if (dwFT && m_bHasMember)
					{
						char szFmt[200], szMsg[256];
						ReadIniString("Fun_PageBg", "StrMemberDemise", szFmt, sizeof(szFmt));
						if (!szFmt[0]) strcpy(szFmt, "X∏c nhÀn muËn chuy”n ng´i bang chÒ lπi cho %s?");
						sprintf(szMsg, szFmt, pFM->m_sMember[m_nSel].m_szName);
						m_nPendOp = defTONG_JX2_COP_DEMISE;
						m_dwPendTarget = dwFT;
						m_nPendP1 = 0; m_nPendP2 = 0;
						UIMessageBox(szMsg, this, "X∏c nhÀn", "Hu˚ b·", TJX2_CONFIRM_ID);
					}
					break;
				case 27:
					// [BH100] doi danh hieu ghe: nguoi dang chon (bang chu / quyen 1004) hoac ban than
					{
						m_dwAmtTarget = dwFT;
						const char* pszCur = "";
						if (dwFT && m_bHasMember)
							pszCur = pFM->m_sMember[m_nSel].m_szTitle;
						KUiTongGetString::OpenWindow("Danh hi÷u mÌi", pszCur, this, TJX2_TITLE_ID, 1, 20);
					}
					break;
				case 28:
				case 29:
					m_nSexTitle = (s_sFunBtn[f].nAct == 29) ? 1 : 0;
					KUiTongGetString::OpenWindow(m_nSexTitle ? "Danh hi÷u n˜ trong bang" : "Danh hi÷u nam trong bang",
						"", this, TJX2_SEXTITLE_ID, 1, 20);
					break;
				case 30:
					// [BH100] thoai an / huy thoai an ban than (StrSelfRetireInfo / StrSelfUnRetireInfo)
					{
						int nMyFig = m_bHasInfo ? ((TONG_JX2_INFO_SYNC*)m_byInfo)->m_btMyFigure : 3;
						AskThenSendOp("Fun_PageBg", nMyFig == 4 ? "StrSelfUnRetireInfo" : "StrSelfRetireInfo",
							defTONG_JX2_COP_RETIRE, 0, nMyFig == 4 ? 0 : 1, 0);
					}
					break;
				case 20:
					m_nAmtOp = defTONG_JX2_COP_MINISTER_FIRE;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("C∏ch ch¯c: 1 TT / 2 NS / 3 TP",
						"1", this, TJX2_AMOUNT_ID, 1, 1);
					break;
				case 13:
					// nap tien ca nhan vao NGAN QUY bang (MONEYFUND_ADD)
					m_nAmtOp = defTONG_JX2_COP_DEPOSIT_MONEY;
					m_dwAmtTarget = 0;
					KUiTongGetString::OpenWindow("Ng©n l≠Óng muËn gˆi",	// G_STR_STORE_MONEY
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
					KUiTongGetString::OpenWindow("RÛt ng©n qu¸ (vπn)",
						"100", this, TJX2_AMOUNT_ID, 1, 7);
					break;
				case 10:
					// cua so PHAT NGAN LUONG theo chuc vu cua ban goc (truoc
					// day chi phat duoc cho MOT nguoi dang chon)
					KUiTongGrant::OpenWindow(true);
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
				// [BH100] BtnEditAnnounce ban goc: bat/tat khung sua (khong tu luu)
				if (m_nRcSub != 1)
					return 1;
				m_bRcEdit = !m_bRcEdit;
				if (m_bRcEdit)
				{
					const char* pszAnn = "";
					if (m_bHasRecord)
						pszAnn = ((TONG_JX2_RECORD_SYNC*)m_byRecord)->m_szAnnounce;
					else if (m_bHasInfo)
						pszAnn = ((TONG_JX2_INFO_SYNC*)m_byInfo)->m_szAnnounce;
					m_RcEditor.SetText(pszAnn);
				}
				RenderRecord();
				return 1;
			}
			if (uParam == (unsigned int)&m_RcLeaveWord)
			{
				// [BH100] BtnLeaveWord ban goc: dang sua thong bao -> GUI thong bao; o so su kien -> loi nhan
				if (m_nRcSub == 1)
				{
					if (!m_bRcEdit)
						return 1;
					char szAnn[256];
					szAnn[0] = 0;
					m_RcEditor.GetText(szAnn, sizeof(szAnn), false);
					szAnn[127] = 0;
					SendOp(defTONG_JX2_COP_SETANN, 0, 0, 0, szAnn);
					// cap nhat ban chep cuc bo de khung doc hien ngay (server chi echo sau khi relay ghi)
					if (m_bHasRecord)
						strncpy(((TONG_JX2_RECORD_SYNC*)m_byRecord)->m_szAnnounce, szAnn, 127);
					if (m_bHasInfo)
						strncpy(((TONG_JX2_INFO_SYNC*)m_byInfo)->m_szAnnounce, szAnn, 127);
					m_bRcEdit = 0;
					RenderRecord();
				}
				else if (m_nRcSub == 2)
					KUiTongGetString::OpenWindow("Tin nhæn bang hÈi", "", this, TJX2_WORD_ID, 1, 31);
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
			PopupIniMenu(3, "Rec_QingXiangMenu", "MenuText", 6);	// [BH100] menu tha xuong nhu ban goc
			return 1;
		}
		{
			int r;
			for (r = 0; r < 4; r++)
			{
				if (uParam == (unsigned int)&m_RecHD[r])
				{
					PopupIniMenu(4 + r, "Rec_HuoDongMenu", "MenuText", 11);
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
			// PHAN TRANG THAT: hang doi don gio nam o relay (toi 64 don),
			// server cat lat 8 don/trang theo wStart va bao ve wApplyTotal.
			TONG_JX2_RECRUIT_SYNC* pRp = (TONG_JX2_RECRUIT_SYNC*)m_byRecruit;
			int nTotal = m_bHasRecruit ? (int)pRp->m_wApplyTotal : 0;
			if (uParam == (unsigned int)&m_RecNext)
			{
				if (m_nRecStart + 8 < nTotal)
					m_nRecStart += 8;
			}
			else if (m_nRecStart >= 8)
				m_nRecStart -= 8;
			else
				m_nRecStart = 0;
			m_nSel = -1;
			RequestPage(m_nPage, m_nRecStart);
			return 1;
		}
		// --- hang dieu khien duoi panel danh sach thanh vien ---
		if (uParam == (unsigned int)&m_MOnline)
		{
			// nut co CheckBox=1 nen no TU LAT truoc khi bao len day
			m_bOnlineFirst = m_MOnline.IsButtonChecked() ? 1 : 0;
			m_nStart = 0;
			m_nSel = 0;
			HideMemberTip();
			RequestPage(m_nPage, 0);	// [BH100] server sap xep toan bang
			return 1;
		}
		if (uParam == (unsigned int)&m_MSort)
		{
			PopupIniMenu(1, "Fun_BtnMemberSortMenu", "Item_", 7);	// [BH100] menu 7 muc cua ban goc
			return 1;
		}
		if (uParam == (unsigned int)&m_MTongSort)
		{
			PopupIniMenu(2, "Fun_BtnTongSortMenu", "Item_", 5);
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
					// [BH100] 'Xem chi tiet' ban goc = mo cua so chinh o che do xem bang do
					TONG_JX2_OTHERZM_SYNC* pZ = (TONG_JX2_OTHERZM_SYNC*)m_byZM;
					if (m_bHasZM && z < (int)pZ->m_btCount)
						ViewTong(pZ->m_sZM[z].m_dwNameID);
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
			int nTot = m_bHasMember ? (int)pM->m_wTotal : 0;
			if (m_nListMode == 2 && m_bHasUnion)
				nTot = (int)((TONG_JX2_TONGLIST_SYNC*)m_byUnion)->m_wTotal;
			else if (m_nListMode == 1 && m_bHasList)
				nTot = (int)((TONG_JX2_TONGLIST_SYNC*)m_byList)->m_wTotal;
			if (m_nStart + defTONG_JX2_VIEW_MEMBERS < nTot)
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

//////////////////////////////////////////////////////////////////////
// [BH100 07/09] Cua so DANH SACH BANG (blueprint 'guild-list window' 120x463): 25 ten/trang,
// bam ten -> cua so chinh xem bang do; Truoc/Ke lat trang; Dong -> ve bang minh.
//////////////////////////////////////////////////////////////////////

KUiTongListJX2* KUiTongListJX2::ms_pSelf = NULL;

KUiTongListJX2* KUiTongListJX2::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiTongListJX2;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		ms_pSelf->Show();
		ms_pSelf->BringToTop();
		ms_pSelf->m_nStart = 0;
		ms_pSelf->m_nSel = -1;
		ms_pSelf->Request(0);
	}
	return ms_pSelf;
}

KUiTongListJX2* KUiTongListJX2::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}

void KUiTongListJX2::CloseWindow(bool bDestroy)
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

void KUiTongListJX2::Initialize()
{
	int i;
	m_bHas = 0;
	m_nStart = 0;
	m_nSel = -1;
	memset(m_byList, 0, sizeof(m_byList));
	for (i = 0; i < TJX2_UI_ROWS; i++)
		AddChild(&m_Row[i]);
	for (i = 0; i < TJX2_UI_ROWS; i++)
		AddChild(&m_RowSel[i]);
	AddChild(&m_BtnPrev);
	AddChild(&m_BtnNext);
	AddChild(&m_BtnClose);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this);
}

void KUiTongListJX2::LoadScheme(const char* pScheme)
{
	if (!ms_pSelf)
		return;
	char Buff[128];
	KIniFile Ini;
	sprintf(Buff, "%s\\%s", pScheme, TONG_JX2_INI);
	if (!Ini.Load(Buff))
		return;
	ms_pSelf->Init(&Ini, "TL_Main");
	ms_pSelf->m_BtnClose.Init(&Ini, "TL_BtnClose");
	ms_pSelf->m_BtnPrev.Init(&Ini, "TL_BtnPrevPage");
	ms_pSelf->m_BtnNext.Init(&Ini, "TL_BtnNextPage");
	for (int i = 0; i < TJX2_UI_ROWS; i++)
	{
		ms_pSelf->m_Row[i].Init(&Ini, "TL_Row");
		ms_pSelf->m_Row[i].SetPosition(8, 25 + i * 15);
		ms_pSelf->m_Row[i].SetSize(104, 14);
		ms_pSelf->m_Row[i].SetText("");
		ms_pSelf->m_RowSel[i].Init(&Ini, "TL_RowSel");
		ms_pSelf->m_RowSel[i].SetPosition(6, 25 + i * 15);
		ms_pSelf->m_RowSel[i].SetSize(108, 15);
	}
}

void KUiTongListJX2::Request(int nStart)
{
	if (!g_pCoreShell)
		return;
	KUiTongJX2View sV;
	memset(&sV, 0, sizeof(sV));
	sV.nPage = defTONG_JX2_PAGE_TONGLIST;
	sV.nStart = nStart;
	g_pCoreShell->TongOperation(GTOI_TONG_JX2_VIEW2, (unsigned int)&sV, 0);
}

void KUiTongListJX2::OnListData(unsigned char* pData, int nLen)
{
	if (!ms_pSelf || !pData || nLen <= 0 || nLen > (int)sizeof(ms_pSelf->m_byList))
		return;
	memcpy(ms_pSelf->m_byList, pData, nLen);
	ms_pSelf->m_bHas = 1;
	ms_pSelf->m_nStart = (int)((TONG_JX2_TONGLIST_SYNC*)ms_pSelf->m_byList)->m_wStart;
	ms_pSelf->Render();
}

void KUiTongListJX2::Render()
{
	int i;
	for (i = 0; i < TJX2_UI_ROWS; i++)
	{
		m_Row[i].SetText("");
		m_RowSel[i].Enable(false);
	}
	if (!m_bHas)
		return;
	TONG_JX2_TONGLIST_SYNC* p = (TONG_JX2_TONGLIST_SYNC*)m_byList;
	for (i = 0; i < (int)p->m_btCount && i < TJX2_UI_ROWS; i++)
	{
		unsigned int uCol = (i == m_nSel) ?
			(0xFF000000 | (34 << 16) | (228 << 8) | 36) : (0xFF000000 | (153 << 16) | (255 << 8) | 255);
		m_Row[i].SetTextColor(uCol);
		m_Row[i].SetText(p->m_sTong[i].m_szName);
		m_RowSel[i].Enable(true);
	}
}

int KUiTongListJX2::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_BUTTON_CLICK)
	{
		if (uParam == (unsigned int)&m_BtnClose)
		{
			CloseWindow(false);
			KUiTongJX2::ViewTong(0);
			return 1;
		}
		TONG_JX2_TONGLIST_SYNC* p = (TONG_JX2_TONGLIST_SYNC*)m_byList;
		if (uParam == (unsigned int)&m_BtnPrev)
		{
			if (m_nStart >= defTONG_JX2_LIST_ROWS)
				Request(m_nStart - defTONG_JX2_LIST_ROWS);
			return 1;
		}
		if (uParam == (unsigned int)&m_BtnNext)
		{
			if (m_bHas && m_nStart + defTONG_JX2_LIST_ROWS < (int)p->m_wTotal)
				Request(m_nStart + defTONG_JX2_LIST_ROWS);
			return 1;
		}
		for (int i = 0; i < TJX2_UI_ROWS; i++)
		{
			if (uParam == (unsigned int)&m_RowSel[i])
			{
				if (m_bHas && i < (int)p->m_btCount)
				{
					m_nSel = i;
					Render();
					KUiTongJX2::ViewTong(p->m_sTong[i].m_dwNameID);
				}
				return 1;
			}
		}
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}

//////////////////////////////////////////////////////////////////////
// [BH100 07/09] Cua so TRO GIUP bang hoi: nut BtnTongHelp cua cua so chinh goc mo he tro giup
// chi tiet cua JX2 (xiangxi bangzhu jiemian) o chu de bang hoi; ban VN cua chu de do chi con 4 trang anh
// 800x600 (banghui gaizao xitong.ini) -> hien lan luot, Truoc/Ke/Dong.
//////////////////////////////////////////////////////////////////////

KUiTongHelpJX2* KUiTongHelpJX2::ms_pSelf = NULL;

KUiTongHelpJX2* KUiTongHelpJX2::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiTongHelpJX2;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		ms_pSelf->Show();
		ms_pSelf->BringToTop();
		ms_pSelf->ShowPage(0);
	}
	return ms_pSelf;
}

KUiTongHelpJX2* KUiTongHelpJX2::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}

void KUiTongHelpJX2::CloseWindow(bool bDestroy)
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

void KUiTongHelpJX2::Initialize()
{
	m_nPage = 0;
	m_nPages = 0;
	memset(m_szPage, 0, sizeof(m_szPage));
	AddChild(&m_BtnPrev);
	AddChild(&m_BtnNext);
	AddChild(&m_BtnClose);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this);
}

void KUiTongHelpJX2::LoadScheme(const char* pScheme)
{
	if (!ms_pSelf)
		return;
	char Buff[128];
	KIniFile Ini;
	sprintf(Buff, "%s\\%s", pScheme, TONG_JX2_INI);
	if (!Ini.Load(Buff))
		return;
	ms_pSelf->Init(&Ini, (SCREEN_WIDTH == 1024) ? "TH_Main1024" : "TH_Main");
	ms_pSelf->m_BtnClose.Init(&Ini, "TH_BtnClose");
	ms_pSelf->m_BtnPrev.Init(&Ini, "TH_BtnPrev");
	ms_pSelf->m_BtnNext.Init(&Ini, "TH_BtnNext");
	ms_pSelf->m_nPages = 0;
	for (int i = 0; i < 4; i++)
	{
		char szKey[16];
		sprintf(szKey, "Page%d", i + 1);
		ms_pSelf->m_szPage[i][0] = 0;
		Ini.GetString("TH_Main", szKey, "", ms_pSelf->m_szPage[i], sizeof(ms_pSelf->m_szPage[i]));
		if (ms_pSelf->m_szPage[i][0])
			ms_pSelf->m_nPages = i + 1;
	}
}

void KUiTongHelpJX2::ShowPage(int nPage)
{
	if (m_nPages <= 0)
		return;
	if (nPage < 0)
		nPage = m_nPages - 1;
	if (nPage >= m_nPages)
		nPage = 0;
	m_nPage = nPage;
	SetImage(ISI_T_SPR, m_szPage[m_nPage], false);
}

int KUiTongHelpJX2::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_BUTTON_CLICK)
	{
		if (uParam == (unsigned int)&m_BtnClose)
		{
			CloseWindow(false);
			return 1;
		}
		if (uParam == (unsigned int)&m_BtnPrev)
		{
			ShowPage(m_nPage - 1);
			return 1;
		}
		if (uParam == (unsigned int)&m_BtnNext)
		{
			ShowPage(m_nPage + 1);
			return 1;
		}
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}
