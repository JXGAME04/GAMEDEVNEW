// KJx2CityWar.cpp - xem KJx2CityWar.h. DOT E cong thanh JX2 (E3).
// Persist: \settings\jx2citywar.txt (tmp + MoveFileEx REPLACE - khong dung KIniFile::Save).

// LUU Y: Core build voi PCH "Use" qua KCore.h - moi thu TRUOC dong include nay
// deu bi compiler bo qua, nen KCore.h PHAI dung dau tien.
#include "KCore.h"
#include "KGameKV.h"
#include "KWin32.h"

#ifdef _SERVER

#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KSubWorld.h"
#include "KPlayerSet.h"
#include "KPlayer.h"
#include "KTongJX2.h"
#include "KJx2League.h"
#include "KJx2CityWar.h"
#include "KProtocol.h"
#include "KPlayerDef.h"
#include "CoreUseNameDef.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

// GetSubWorldIndex(L): doc global "SubWorld" cua state goi (ScriptFuns.cpp:476,
// khong co khai bao trong header nao)
extern int GetSubWorldIndex(Lua_State* L);
// GetPlayerIndex: idiom KTongJX2.cpp:32 (cho nhom ArenaCredits)
extern int GetPlayerIndex(Lua_State* L);
// E6: doi chu thanh -> cap/thu Title Thai Thu (KJx2Title.cpp) + field 48
// OccupyCityDay (KTongJX2.cpp - duong CITY_OCCUPY_R goc tong_mix.lua:1074-1082)
extern void KJx2Title_GrantByName(const char* szPlayerName, int nTitleId);
extern void KJx2Title_RevokeByName(const char* szPlayerName, int nTitleId);
extern void KTongJX2_SetOccupyCityDayC(DWORD dwTongID, int nDay);
extern DWORD KTongJX2_GetFieldC(DWORD dwTongID, WORD wKey);

//////////////////////////////////////////////////////////////////////
// Store
//////////////////////////////////////////////////////////////////////
#define JX2CW_CITY_MAX		8		// dung 1..7
#define JX2CW_MAPID_MAX		4		// AreaIncludesNN cho phep nhieu map cach dau phay
#define JX2CW_FILE			"\\settings\\jx2citywar.txt"
#define JX2CW_KV_NS			"jx2.citywar"
#define JX2CW_INI			"\\settings\\citywar.ini"

struct KJx2City
{
	int		nState;					// JX2CW_STATE_*
	int		nMapIds[JX2CW_MAPID_MAX];
	int		nMapIdCount;
	char	szAreaName[64];			// byte TCVN3 nguyen van tu ini
	char	szOwnerTong[64];		// ten bang chiem thanh ("" = vo chu)
	char	szMaster[64];			// ten NHAN VAT Thai Thu (citybulletin so voi GetName())
	char	szChallenger[64];		// ten bang khieu chien ("" = khong co)
	int		nTax;
	int		nPriceParam;			// goc: -1 -> hien thi 10
	int		nOccupyDate;			// yyyymmdd luc doi chu
	int		nTaxSetDate;			// yyyymmdd lan dat thue gan nhat (1 lan/ngay - E7)
};

static KJx2City	s_Cities[JX2CW_CITY_MAX];
// [LOI DAI CN 21/08] persist loi dai - than o khoi ARENA phia duoi
static void sArenaLoadBegin();
static void sArenaLoadLine(const char* szLine);
static void sArenaLoadFinish();
static void sArenaSaveLines(FILE* f);
static bool		s_bStoreLoaded = false;		// da nap config + mirror
static bool		s_bWorldApplied = false;	// da ghi vao KSubWorld (sau khi map nap)

// [CitySettings] - E7 dung cho thue/dieu kien; thu tu = KJx2CityWar_GetSetting
static int s_nSettings[9] = { 1000000, 18, 20, 20, 37, 22, 23, 7, 30 };

static void sStrCpy(char* szDst, const char* szSrc, int nDstSize)
{
	strncpy(szDst, szSrc ? szSrc : "", nDstSize - 1);
	szDst[nDstSize - 1] = 0;
}

static int sToday()
{
	time_t t = time(NULL);
	struct tm* pTm = localtime(&t);
	if (!pTm)
		return 0;
	return (pTm->tm_year + 1900) * 10000 + (pTm->tm_mon + 1) * 100 + pTm->tm_mday;
}

// ten bang -> ten bang chu (btFigure == 0) tren ban sao g_TongJX2; khong co -> ""
static void sMasterOfTong(const char* szTong, char* szOut, int nOutSize)
{
	szOut[0] = 0;
	if (!szTong || !szTong[0])
		return;
	KTongJX2Tong* pTong = g_TongJX2.FindTong(g_FileName2Id((LPSTR)szTong));
	if (!pTong)
		return;
	std::map<DWORD, KTongJX2Member>::iterator it;
	for (it = pTong->mapMember.begin(); it != pTong->mapMember.end(); ++it)
	{
		if (it->second.btFigure == 0)
		{
			sStrCpy(szOut, it->second.szName, nOutSize);
			return;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Config (\settings\citywar.ini)
//////////////////////////////////////////////////////////////////////
static void sLoadConfig()
{
	char szRoot[MAX_PATH], szPath[MAX_PATH];
	g_GetRootPath(szRoot);
	sprintf(szPath, "%s%s", szRoot, JX2CW_INI);
	KIniFile Ini;
	if (!Ini.Load(szPath))
	{
		g_DebugLog((LPSTR)"KJx2CityWar: khong doc duoc %s", szPath);
		return;
	}
	char szKey[32], szVal[128];
	for (int i = 1; i <= 7; i++)
	{
		sprintf(szKey, "AreaName%02d", i);
		Ini.GetString("CityArea", szKey, "", s_Cities[i].szAreaName, sizeof(s_Cities[i].szAreaName));
		sprintf(szKey, "AreaIncludes%02d", i);
		szVal[0] = 0;
		Ini.GetString("CityArea", szKey, "", szVal, sizeof(szVal));
		s_Cities[i].nMapIdCount = 0;
		const char* p = szVal;
		while (*p && s_Cities[i].nMapIdCount < JX2CW_MAPID_MAX)
		{
			while (*p == ' ' || *p == ',' || *p == '\t')
				p++;
			if (*p < '0' || *p > '9')
				break;
			int n = 0;
			while (*p >= '0' && *p <= '9')
				n = n * 10 + (*p++ - '0');
			s_Cities[i].nMapIds[s_Cities[i].nMapIdCount++] = n;
		}
	}
	static const char* szSetKey[9] = { "SignUpFee", "MinTongLevel", "MaxExchangeTax",
		"MaxPriceParam", "MinTongCrowNumber", "StartSetTaxTime", "EndSetTaxTime",
		"WarCycleValue", "SupplyLineBuildScale" };
	for (int k = 0; k < 9; k++)
		Ini.GetInteger("CitySettings", szSetKey[k], s_nSettings[k], &s_nSettings[k]);
}

//////////////////////////////////////////////////////////////////////
// Persist mirror (dinh dang dong; ten TCVN3 co the chua space -> de CUOI dong)
//   C <id> <state> <tax> <priceparam> <occupydate>
//   O <id> <ten bang chiem thanh...>
//   M <id> <ten Thai Thu...>
//   H <id> <ten bang khieu chien...>
//////////////////////////////////////////////////////////////////////
static void sCityPath(char* szOut, bool bTmp)
{
	char szRoot[MAX_PATH];
	g_GetRootPath(szRoot);
	sprintf(szOut, "%s%s%s", szRoot, JX2CW_FILE, bTmp ? ".tmp" : "");
}

static char* sChopEol(char* sz)
{
	int n = (int)strlen(sz);
	while (n > 0 && (sz[n - 1] == '\n' || sz[n - 1] == '\r'))
		sz[--n] = 0;
	return sz;
}

static void sLoadMirror()
{
	char szPath[MAX_PATH];
	sCityPath(szPath, false);
	FILE* f = fopen(szPath, "rb");
	if (!f)
		return;
	char szLine[512];
	int nVer = 1;		// [LOI DAI CN 21/08] 'V 2' = state 0..5; mirror cu (khong co V) 1->4, 2->5
	sArenaLoadBegin();
	while (fgets(szLine, sizeof(szLine), f))
	{
		sChopEol(szLine);
		if (szLine[0] && szLine[1] != ' ')
			continue;	// dong hong/cut - bo qua, khong parse "ma" byte cu trong dem
		if (szLine[0] == 'V')
		{
			sscanf(szLine + 2, "%d", &nVer);
		}
		else if (szLine[0] == 'G' || szLine[0] == 'A' || szLine[0] == 'F' || szLine[0] == 'B' || szLine[0] == 'R')
		{
			sArenaLoadLine(szLine);
		}
		else if (szLine[0] == 'C')
		{
			int nId = 0, nState = 0, nTax = 0, nPP = 0, nDate = 0;
			if (sscanf(szLine + 2, "%d %d %d %d %d", &nId, &nState, &nTax, &nPP, &nDate) == 5 &&
				nId >= 1 && nId <= 7)
			{
				if (nVer < 2)
					nState = (nState == 1) ? JX2CW_STATE_WARDECIDED : (nState == 2) ? JX2CW_STATE_ATWAR : JX2CW_STATE_NORMAL;
				s_Cities[nId].nState = (nState >= JX2CW_STATE_NORMAL && nState <= JX2CW_STATE_ATWAR) ? nState : JX2CW_STATE_NORMAL;
				s_Cities[nId].nTax = nTax;
				s_Cities[nId].nPriceParam = nPP;
				s_Cities[nId].nOccupyDate = nDate;
			}
		}
		else if (szLine[0] == 'D')
		{
			// 'D id yyyymmdd' - ngay dat thue gan nhat (E7; dong rieng de
			// giu tuong thich nguoc voi mirror cu 5 truong dong C)
			int nId = 0, nDate = 0;
			if (sscanf(szLine + 2, "%d %d", &nId, &nDate) == 2 && nId >= 1 && nId <= 7)
				s_Cities[nId].nTaxSetDate = nDate;
		}
		else if (szLine[0] == 'O' || szLine[0] == 'M' || szLine[0] == 'H')
		{
			int nId = 0, nPos = 0;
			if (sscanf(szLine + 2, "%d %n", &nId, &nPos) >= 1 && nPos > 0 &&
				nId >= 1 && nId <= 7)
			{
				char* szDst = (szLine[0] == 'O') ? s_Cities[nId].szOwnerTong :
					(szLine[0] == 'M') ? s_Cities[nId].szMaster : s_Cities[nId].szChallenger;
				sStrCpy(szDst, szLine + 2 + nPos, sizeof(s_Cities[nId].szOwnerTong));
			}
		}
	}
	fclose(f);
	sArenaLoadFinish();
}

static void sSaveMirror()
{
	char szTmp[MAX_PATH], szPath[MAX_PATH];
	sCityPath(szTmp, true);
	sCityPath(szPath, false);
	FILE* f = fopen(szTmp, "wb");
	if (!f)
		return;
	fprintf(f, "V 2\n");	// [LOI DAI CN 21/08] state 0..5 + cac dong G/A/F/B/R loi dai
	for (int i = 1; i <= 7; i++)
	{
		KJx2City* p = &s_Cities[i];
		fprintf(f, "C %d %d %d %d %d\n", i, p->nState, p->nTax, p->nPriceParam, p->nOccupyDate);
		if (p->nTaxSetDate)
			fprintf(f, "D %d %d\n", i, p->nTaxSetDate);
		if (p->szOwnerTong[0])
			fprintf(f, "O %d %s\n", i, p->szOwnerTong);
		if (p->szMaster[0])
			fprintf(f, "M %d %s\n", i, p->szMaster);
		if (p->szChallenger[0])
			fprintf(f, "H %d %s\n", i, p->szChallenger);
	}
	sArenaSaveLines(f);
	// dia day/loi ghi: KHONG de .tmp cut de len file tot (cung luat voi KJx2League)
	BOOL bOk = !ferror(f);
	if (fclose(f) != 0)
		bOk = FALSE;
	if (bOk)
	{
		MoveFileEx(szTmp, szPath, MOVEFILE_REPLACE_EXISTING);
#ifdef _SERVER
		// (20/08) Ghi xong tep thi day luon len MySQL. BAT DONG BO nen khong
		// cham vao vong lap game. Tu day tep chi con la ban dem cuc bo.
		KGameKV::PutFile(JX2CW_KV_NS, "file", szPath, true);
#endif
	}
	else
	{
		DeleteFile(szTmp);
		g_DebugLog((LPSTR)"KJx2CityWar: GHI HONG jx2citywar.txt.tmp - giu file cu");
	}
}

//////////////////////////////////////////////////////////////////////
// Nap store + dong bo KSubWorld
//////////////////////////////////////////////////////////////////////
static void sEnsureStore()
{
	if (s_bStoreLoaded)
		return;
	s_bStoreLoaded = true;
	memset(s_Cities, 0, sizeof(s_Cities));
	for (int i = 0; i < JX2CW_CITY_MAX; i++)
		s_Cities[i].nPriceParam = -1;	// goc: -1 -> hien thi 10
	sLoadConfig();
	sLoadMirror();
}

// owner ghi dang dem " %s " (khop hien thi client cu); "" khi vo chu
static void sApplyCityToSubWorld(int nCityID)
{
	if (nCityID < 1 || nCityID > 7)
		return;
	KJx2City* p = &s_Cities[nCityID];
	char szPad[32];
	if (p->szOwnerTong[0])
	{
		int n = (int)strlen(p->szOwnerTong);
		if (n > 29)
			n = 29;
		szPad[0] = ' ';
		memcpy(szPad + 1, p->szOwnerTong, n);
		szPad[n + 1] = ' ';
		szPad[n + 2] = 0;
	}
	else
		szPad[0] = 0;
	int nTax = p->nTax;
	if (nTax < 0)
		nTax = 0;
	if (nTax > s_nSettings[2])
		nTax = s_nSettings[2];
	for (int m = 0; m < p->nMapIdCount; m++)
	{
		int nMapId = p->nMapIds[m];
		if (nMapId <= 0)
			continue;
		// 3/7 thanh nap 2 instance subworld - phai ghi MOI instance cung map id
		for (int w = 0; w < MAX_SUBWORLD; w++)
		{
			if (SubWorld[w].m_SubWorldID == nMapId)
			{
				strcpy(SubWorld[w].m_CityOwnTong, szPad);	// szPad <= 31 byte + NUL
				SubWorld[w].m_CityTax = (BYTE)nTax;
			}
		}
	}
}

void KJx2CityWar_Breathe()
{
	if (s_bWorldApplied)
		return;
	s_bWorldApplied = true;
	sEnsureStore();
	// Tran KHONG song qua restart (goc: state nam RAM engine, restart la mat;
	// ta persist mirror nen phai TU HA luc boot - thieu la timer 18 poll 5'
	// mo lai tran VO HAN khi state 2 ket lai; phan bien E3 #4).
	int bChanged = 0;
	for (int i = 1; i <= 7; i++)
	{
		if (s_Cities[i].nState == JX2CW_STATE_ATWAR)
		{
			s_Cities[i].nState = JX2CW_STATE_NORMAL;
			s_Cities[i].szChallenger[0] = 0;
			bChanged = 1;
			g_DebugLog((LPSTR)"KJx2CityWar: boot ha state DANG DANH cua thanh %d ve thuong", i);
		}
	}
	if (bChanged)
		sSaveMirror();
	for (int i = 1; i <= 7; i++)
	{
		sApplyCityToSubWorld(i);
		// re-grant Title Thai Thu tu mirror (KEHOACH 1.2: title nam RAM,
		// restart mat - ownership cap lai tu mirror luc boot)
		if (s_Cities[i].szMaster[0])
			KJx2Title_GrantByName(s_Cities[i].szMaster, 152 + i);
	}
}

//////////////////////////////////////////////////////////////////////
// Helper doi so
//////////////////////////////////////////////////////////////////////
static int sArgCity(Lua_State* L, int nStackIdx)
{
	if (!Lua_IsNumber(L, nStackIdx))
		return 0;
	int n = (int)Lua_ValueToNumber(L, nStackIdx);
	return (n >= 1 && n <= 7) ? n : 0;
}

// ten thanh (byte TCVN3, khop relay TB_CITYWAR_ARRANGE) -> id; 0 = khong thay
static int sCityByName(const char* szName)
{
	if (!szName || !szName[0])
		return 0;
	for (int i = 1; i <= 7; i++)
	{
		if (strcmp(s_Cities[i].szAreaName, szName) == 0)
			return i;
	}
	return 0;
}

// E6: nghiep vu di kem DOI CHU thanh (goi tu AppointViceroy + NotifyWarResult
// cong thang, SAU khi store da cap nhat, voi anh chup chu/CU truoc khi ghi de):
// - Title Thai Thu id 152+city: THU cua cuu + CAP cho tan (DIEUTRA muc 1/4)
// - field 48 OccupyCityDay: tan = TONG_GetDay (field 20) cua bang tan, cuu = 0
//   (duong CITY_OCCUPY_R goc - tong_mix.lua:1074-1082)
static void sOwnerChanged(int nCityID, const char* szOldOwner, const char* szOldMaster)
{
	KJx2City* p = &s_Cities[nCityID];
	if (szOldMaster && szOldMaster[0])
		KJx2Title_RevokeByName(szOldMaster, 152 + nCityID);
	if (p->szMaster[0])
		KJx2Title_GrantByName(p->szMaster, 152 + nCityID);
	if (szOldOwner && szOldOwner[0] && strcmp(szOldOwner, p->szOwnerTong) != 0)
		KTongJX2_SetOccupyCityDayC(g_FileName2Id((LPSTR)szOldOwner), 0);
	if (p->szOwnerTong[0])
	{
		DWORD dwID = g_FileName2Id((LPSTR)p->szOwnerTong);
		KTongJX2_SetOccupyCityDayC(dwID, (int)KTongJX2_GetFieldC(dwID, 20));
	}
}

//////////////////////////////////////////////////////////////////////
// LOI DAI BANG HOI - ban CN goc (21/08/2026). Dich nguoc relay s3relay_y
// (KCityWarDataRelay: StartSignUp 0x0809AB8C / SignUp 0x0809AE32 / EndSignUp
// 0x0809C0BA / CutTo16 0x0809B736 / BuildBracket 0x0809CC62 / StartArena
// 0x0809CF60 / AddArenaResult 0x0809D4F0 / StartCityWar 0x0809E13C) + GS
// jx_linux_y (KCityWarDataGS: BuildArenaPairs 0x0805A480 / AddArenaResult
// 0x080593A0 / 14 ham Lua). Dac ta: DACTA_LOIDAI_BANGHOI_CN_WORKFLOW.md.
// Du an KHONG co relay -> ca hai phia gop vao day; kenh SDB/0x0A = goi ham
// truc tiep; tin tuc = AddLocalNews noi bo; lich = timerserver_ctc.lua.
// Chinh sach chu game: KHONG co dieu kien "khao nghiem" (tongInfo+0x24 - bang
// ta khong co truong tuong ung).
//////////////////////////////////////////////////////////////////////
#include <vector>
#include <string>
#include "KPlayerChat.h"
#include "KNewProtocolProcess.h"

#define JX2CW_ARENA_SLOTS	8		// ArenaID 0..7 = map 213..220
#define JX2CW_SIGNUP_MAX	16		// relay: size > 0x10 -> cat theo phi (0x0809C816)
#define JX2CW_BYE			0xFFFF

struct KJx2ArenaPair   { WORD wA; WORD wB; BYTE btFlag; };	// btFlag 0 cho dau / 1 B thua / 2 A thua
struct KJx2ArenaResult { WORD wA; WORD wB; BYTE btResult; };	// btResult 1 A thang / 2 B thang

struct KJx2CityArena
{
	std::vector<std::string>		vecTong;		// bang bao danh (chi so = "tong index")
	std::vector<int>				vecFee;			// phi dau thau song song
	std::vector<KJx2ArenaResult>	vecResult;
	std::vector<DWORD>				vecBracket;		// low16 = tong index / 0xFFFF bye; high16 0 song / 1 vua thua / 2 da loai
	std::vector<KJx2ArenaPair>		vecPair;		// cap cua VONG HIEN TAI; chi so = ArenaID
	int								nLevel;			// vong dang cho (1-based); 0 = chua / da xong
};

static KJx2CityArena	s_Arena[JX2CW_CITY_MAX];
static int				s_bArenaRunning = 0;	// goc G+0x1A4 / R+0x19C
static int				s_nArenaCity = -1;		// goc G+0x1A8 / R+0x1A0 (-1 = roi)
static bool				s_bArenaSeeded = false;

// ---- chuoi (lang\vn\stringtable_relay.txt cua relay, byte TCVN3 nguyen van) ----
#define CWS_NOTIFY_SIGNUP			"L«i ®µi trong thµnh %s ®· b¾t ®Çu b¸o danh, hoan nghªnh c¸c bang héi ®· ®¹t ®Õn ®¼ng cÊp %d ®Õn b¸o danh thi ®Êu! "
#define CWS_NOTIFY_SIGNUP_OK		"Bang héi cña B¹n ®· ®¨ng ký thµnh c«ng, xin ®îi L«i ®µi khai m¹c!! "
#define CWS_NOTIFY_SIGNUP_TOALL		"%s bang héi ®· b¸o danh tham gia cuéc thi ®Êu L«i ®µi trong thµnh %s!!"
#define CWS_NOTIFY_NO_SIGNUP		"%s cuéc thi ®Êu L«i ®µi trong thµnh kh«ng ng­êi b¸o danh, tuÇn nµy v¹n sù v« ­u. "
#define CWS_NOTIFY_SINGLE_SIGNUP	"%s cuéc thi ®Êu L«i ®µi trong thµnh chØ cã %s mét bang héi b¸o danh, xem nh­ ®· ®ñ t­ c¸ch khiªu chiÕn!! "
#define CWS_NOTIFY_SIGNUP_COMPLETE	"%s L«i ®µi trong thµnh ®· chuÈn bÞ hoµn tÊt, tæng céng cã %d bang héi tham chiÕn, lÞch thi ®Êu ®· x¸c ®Þnh, lÇn nµy xin c¸c bang héi thi ®Êiu ®óng giê. "
#define CWS_NOTIFY_ARENA_START		"%s L«i ®µi trong thµnh ®· b¾t ®Çu thi ®Êu, mêi nh÷ng ®éi ®¨ng ký ®Çu tiªn vµo thi ®Êu, kh«ng ®­îc chËm trÔ "
#define CWS_NOTIFY_ARENA_RESULT		"Bang héi %s trong trËn nµy ®· ®¸nh b¹i bang héi %s, chuÈn bÞ cho trËn ®Êu míi!"
#define CWS_NOTIFY_ARENA_RESULT2	"Bang héi %s trong qu¸ tr×nh thi ®Êu ®· ®¸nh b¹i bang héi %s, ®­îc quyÒn c«ng thµnh %s!! "
#define CWS_NOTIFY_WAR_STARTED		"§¹i chiÕn trong thµnh %s ®· chÝnh thøc b¾t ®Çu, bang héi %s vµ %s lËp tøc b¾t ®Çu chiÕn ®Êu kÞch liÖt!! "
#define CWS_NOTIFY_WAR_RESULT3		"Trong thµnh %s tr­íc m¾t ch­a cã Th¸i Thó, bang héi %s ®­îc c«ng nhËn trë thµnh ng­êi thèng trÞ ®Çu tiªn!! "
#define CWS_NOTIFY_SIGNUP_OUT		"Sè tiÒn tÝch lòy ch­a ®ñ kh«ng ®ñ, bang héi cña B¹n b¸o danh %s thÊt b¹i!! "
#define CWS_NOTIFY_SIGNUP_OUT2		"TiÒn tÝch lòy %d l­îng ®· tr¶ l¹i ng©n quü cña bang héi, xin ®Õn lÊy. "
#define CWS_WARN_SIGNUPTIMEOUT		"Xin lçi! Giai ®o¹n b¸o danh ®· kÕt thóc."
#define CWS_WARN_ALREADYSIGNUP		"Bang héi cña B¹n ®· b¸o danh råi!"
#define CWS_WARN_NOTENOUGHFEE		"Xin lçi! Ng©n s¸ch bang ph¸i cña B¹n kh«ng ®ñ ®Ó nép phÝ b¸o danh %d l­îng."
#define CWS_WARN_LEVELLOW			"Xin lçi! Bang héi cña B¹n ®¼ng cÊp kh«ng ®ñ %d cÊp."
#define CWS_WARN_NOTTONGLEADER		"B¹n kh«ng ph¶i lµ Bang chñ cña bang héi!"
#define CWS_WARN_TONGERROR			"V« hiÖu bang héi!!"
#define CWS_WARN_BECHALLENGER		"B¹n ®· cã ®­îc quyÒn khiªu chiÕn trong thµnh %s!!"
#define CWS_WARN_BECITYOWNER		"Bang héi cña B¹n ®· lµ ng­êi thèng trÞ trong thµnh  %s råi!!"
#define CWS_WARN_TOOFEWCROWS		"Xin lçi! Bang héi cña B¹n nh©n sè kh«ng ®ñ %d ng­êi, vÉn kh«ng ®ñ t­ c¸ch. "
#define CWS_WARN_PERIODNOTMATCH		"Giai ®o¹n tr­íc kh«ng phï hîp, lÏ ra lµ %d, th× l¹i lµ %d. "
#define CWS_DATA_0					"< Vßng §¶ L«i §µi míi nhÊt >\n"
#define CWS_DATA_1					"Hai bªn quyÕt ®Êu:"
#define CWS_DATA_2					"TrËn thi ®Êu b¸n kÕt:"
#define CWS_DATA_3					"L­ît thi ®Êu thø %u:"
#define CWS_DATA_4					"%s ®Êu víi %s"
#define CWS_DATA_5					"%s th¾ng %s"
#define CWS_DATA_6					"<< Khu vùc thµnh %s >>\n"
#define CWS_DATA_7					", "
#define CWS_DATA_8					"Thi ®Êu vÉn ch­a tiÕn hµnh..."

// ---- tien ich ----
extern void KTongJX2_AddMoneyC(DWORD dwTongID, __int64 nDelta);
extern __int64 KTongJX2_GetMoneyC(DWORD dwTongID);
extern void KTongJX2_SetFieldC(DWORD dwTongID, WORD wKey, DWORD dwVal);

// AddLocalNews noi bo (khuon LuaAddLocalNews nhanh _SERVER, ScriptFuns.cpp)
static void sLocalNews(const char* szMsg)
{
	if (!szMsg || !szMsg[0])
		return;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_NEWSINFO;
	UiInfo.m_bOptionNum = NEWSMESSAGE_NORMAL;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	g_StrCpyLen(UiInfo.m_pContent, (char*)szMsg, 256);
	UiInfo.m_nBufferLen = strlen(((char*)UiInfo.m_pContent));
	UiInfo.m_bParam1 = 0;
	UiInfo.m_bParam2 = 1;
	UiInfo.ProtocolType = (BYTE)s2c_scriptaction;
	UiInfo.m_wProtocolLong = sizeof(PLAYER_SCRIPTACTION_SYNC) - MAX_SCIRPTACTION_BUFFERNUM + UiInfo.m_nBufferLen - 1;
	g_NewProtocolProcess.BroadcastLocalServer(&UiInfo, UiInfo.m_wProtocolLong + 1);
	g_DebugLog((LPSTR)"KJx2CityWar[arena] NEWS: %.200s", szMsg);
}

static void sMsgPlayer(int nPlayerIndex, const char* szMsg)
{
	if (nPlayerIndex > 0 && szMsg && szMsg[0])
		KPlayerChat::SendSystemInfo(1, nPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD, (char*)szMsg, strlen(szMsg));
}

// gui cho bang chu (dang online) cua bang szTong
static void sMsgTongMaster(const char* szTong, const char* szMsg)
{
	char szMaster[64];
	sMasterOfTong(szTong, szMaster, sizeof(szMaster));
	if (!szMaster[0])
		return;
	for (int i = 1; i <= PlayerSet.GetPlayerMaxNumber(); i++)
	{
		if (Player[i].m_nIndex > 0 && strcmp(Player[i].m_PlayerName, szMaster) == 0)
		{
			sMsgPlayer(i, szMsg);
			return;
		}
	}
}

static void sArenaClear(int c)
{
	if (c < 0 || c >= JX2CW_CITY_MAX)
		return;
	s_Arena[c].vecTong.clear();
	s_Arena[c].vecFee.clear();
	s_Arena[c].vecResult.clear();
	s_Arena[c].vecBracket.clear();
	s_Arena[c].vecPair.clear();
	s_Arena[c].nLevel = 0;
}

static int sLog2(int n)
{
	int k = 0;
	while (n > 1) { n >>= 1; k++; }
	return k;
}

static const char* sTongName(int c, int idx)
{
	if (c < 1 || c > 7 || idx < 0 || idx >= (int)s_Arena[c].vecTong.size())
		return "";
	return s_Arena[c].vecTong[idx].c_str();
}

// ---- 3.1 BuildBracket (R 0x0809CC62) - goi 1 lan luc EndSignUp ----
static void sBuildArenaPairs(int c, int bKeep);
static int sBuildBracket(int c)
{
	KJx2CityArena* pA = &s_Arena[c];
	int N = (int)pA->vecTong.size();
	if (N <= 1)
		return 0;
	int nSize = 1;
	while (nSize < N)
		nSize <<= 1;
	pA->vecBracket.assign(nSize, 0xFFFFFFFF);
	std::vector<int> vecUsed(N, 0);
	if (!s_bArenaSeeded)
	{
		srand((unsigned)time(NULL));
		s_bArenaSeeded = true;
	}
	int nUsed = 0;
	for (int i = 0; i < nSize; i += 2)				// khe CHAN truoc, KHONG kiem count (0x0809CD95)
	{
		if (nUsed >= N)
			break;	// (ta them: goc khong co vi size/2 <= N luon dung; giu an toan)
		int r = rand() % N;
		while (vecUsed[r])
			r = (r + 1) % N;
		vecUsed[r] = 1; nUsed++;
		pA->vecBracket[i] = (DWORD)r;
	}
	for (int i = 1; i < nSize && nUsed < N; i += 2)	// khe LE bang phan con lai (0x0809CE3F)
	{
		int r = rand() % N;
		while (vecUsed[r])
			r = (r + 1) % N;
		vecUsed[r] = 1; nUsed++;
		pA->vecBracket[i] = (DWORD)r;
	}
	pA->vecResult.clear();
	sBuildArenaPairs(c, 0);
	return 1;
}

// ---- 3.2 BuildArenaPairs (G 0x0805A480) - chay sau moi ket qua ----
static void sBuildArenaPairs(int c, int bKeep)
{
	KJx2CityArena* pA = &s_Arena[c];
	int nSize = (int)pA->vecBracket.size();
	if (nSize <= 1)
		return;
	int n = nSize, nLevel = 1;
	for (;;)
	{
		pA->vecPair.clear();
		int nSeg = nSize / n;
		bool bAllDone = true;
		for (int i = 0; i < nSize; i += 2 * nSeg)
		{
			int iA = -1, iB = -1;
			for (int k = i; k < i + nSeg && k < nSize; k++)
			{
				DWORD v = pA->vecBracket[k];
				if ((v & 0xFFFF) != JX2CW_BYE && (v >> 16) != 2) { iA = k; break; }
			}
			for (int k = i + nSeg; k < i + 2 * nSeg && k < nSize; k++)
			{
				DWORD v = pA->vecBracket[k];
				if ((v & 0xFFFF) != JX2CW_BYE && (v >> 16) != 2) { iB = k; break; }
			}
			if (iA < 0 || iB < 0)
				continue;		// thieu 1 ben -> bye, ben con lai tu len vong sau
			DWORD vA = pA->vecBracket[iA], vB = pA->vecBracket[iB];
			KJx2ArenaPair sPair;
			sPair.wA = (WORD)(vA & 0xFFFF);
			sPair.wB = (WORD)(vB & 0xFFFF);
			sPair.btFlag = (vA >> 16) ? 2 : ((vB >> 16) ? 1 : 0);
			if (sPair.btFlag == 0)
				bAllDone = false;
			pA->vecPair.push_back(sPair);
			pA->nLevel = nLevel;	// goc ghi TRONG nhanh co cap (phan bien: 0x0805A62B)
		}
		if (!bAllDone)
			return;				// day la vong hien tai
		for (int k = 0; k < nSize; k++)
		{
			if (pA->vecBracket[k] >> 16)
				pA->vecBracket[k] = (pA->vecBracket[k] & 0xFFFF) | 0x20000;	// loai han
		}
		n >>= 1; nLevel++;
		if (n <= 1)
		{
			pA->vecPair.clear();
			pA->nLevel = 0;
			if (!bKeep)
				s_bArenaRunning = 0;
			return;
		}
	}
}

// ---- AddArenaResult loi (G 0x080593A0 + R 0x080A04A6) ----
// tra 0 OK / 1 tham so / 2 trung (da co ket qua) ; ghi ban ghi, danh thua, dung lai cap
static int sAddArenaResultCore(int c, int nA, int nB, int nResult, int bKeep)
{
	KJx2CityArena* pA = &s_Arena[c];
	int N = (int)pA->vecTong.size();
	if (nA < 0 || nB < 0 || nA >= N || nB >= N || nA == nB || (nResult != 1 && nResult != 2))
		return 1;
	// tim ban ghi cung cap (A,B) hoac (B,A) (phan bien: goc so 2 chieu)
	int nFound = -1;
	for (int i = 0; i < (int)pA->vecResult.size(); i++)
	{
		KJx2ArenaResult* pR = &pA->vecResult[i];
		if ((pR->wA == nA && pR->wB == nB) || (pR->wA == nB && pR->wB == nA))
		{
			nFound = i;
			break;
		}
	}
	if (nFound >= 0 && pA->vecResult[nFound].btResult != 0)
		return 2;		// goc: khong de, khong danh thua (ta chan han - phan bien goi y b)
	KJx2ArenaResult sR;
	sR.wA = (WORD)nA; sR.wB = (WORD)nB; sR.btResult = (BYTE)nResult;
	if (nFound >= 0)
		pA->vecResult[nFound] = sR;
	else
		pA->vecResult.push_back(sR);
	int nLoser = (nResult == 2) ? nA : nB;
	for (int k = 0; k < (int)pA->vecBracket.size(); k++)
	{
		if ((pA->vecBracket[k] & 0xFFFF) == (DWORD)nLoser)
			pA->vecBracket[k] = (pA->vecBracket[k] & 0xFFFF) | 0x10000;	// vua thua
	}
	sBuildArenaPairs(c, bKeep);
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Persist (phan mo rong cua jx2citywar.txt - xem sSaveMirror):
//   V 2                       phien ban mirror (state 0..5)
//   G <running> <city>        toan cuc loi dai
//   A <city> <ten bang>       bang bao danh (thu tu = tong index)
//   F <city> <phi>            phi song song voi A
//   B <city> <n> <i1..in>     bracket low16 (0xFFFF = bye) - cung thu tu relay field 11
//   R <city> <a> <b> <res>    ket qua - nap xong thi REPLAY qua AddArenaResult(bKeep=1)
//////////////////////////////////////////////////////////////////////
struct KJx2ArenaLoadTmp { std::vector<KJx2ArenaResult> vecR[JX2CW_CITY_MAX]; int bRunning; int nCity; bool bHasG; };
static KJx2ArenaLoadTmp* s_pArenaLoad = NULL;
static void sArenaLoadBegin()
{
	if (s_pArenaLoad)
		delete s_pArenaLoad;
	s_pArenaLoad = new KJx2ArenaLoadTmp;
	s_pArenaLoad->bRunning = 0; s_pArenaLoad->nCity = -1; s_pArenaLoad->bHasG = false;
	for (int c = 0; c < JX2CW_CITY_MAX; c++)
		sArenaClear(c);
}

static void sArenaLoadLine(const char* szLine)
{
	if (!s_pArenaLoad)
		return;
	int c = 0, nPos = 0;
	switch (szLine[0])
	{
	case 'G':
		if (sscanf(szLine + 2, "%d %d", &s_pArenaLoad->bRunning, &s_pArenaLoad->nCity) == 2)
			s_pArenaLoad->bHasG = true;
		break;
	case 'A':
		if (sscanf(szLine + 2, "%d %n", &c, &nPos) >= 1 && nPos > 0 && c >= 1 && c <= 7)
			s_Arena[c].vecTong.push_back(std::string(szLine + 2 + nPos));
		break;
	case 'F':
		{
			int nFee = 0;
			if (sscanf(szLine + 2, "%d %d", &c, &nFee) == 2 && c >= 1 && c <= 7)
				s_Arena[c].vecFee.push_back(nFee);
		}
		break;
	case 'B':
		{
			int n = 0;
			if (sscanf(szLine + 2, "%d %d %n", &c, &n, &nPos) >= 2 && c >= 1 && c <= 7 && n > 0 && n <= 64)
			{
				const char* p = szLine + 2 + nPos;
				s_Arena[c].vecBracket.clear();
				for (int i = 0; i < n; i++)
				{
					int v = 0, nRead = 0;
					if (sscanf(p, "%d %n", &v, &nRead) < 1)
						break;
					p += nRead;
					s_Arena[c].vecBracket.push_back((DWORD)(v & 0xFFFF));
				}
			}
		}
		break;
	case 'R':
		{
			int a = 0, b = 0, r = 0;
			if (sscanf(szLine + 2, "%d %d %d %d", &c, &a, &b, &r) == 4 && c >= 1 && c <= 7)
			{
				KJx2ArenaResult sR;
				sR.wA = (WORD)a; sR.wB = (WORD)b; sR.btResult = (BYTE)r;
				s_pArenaLoad->vecR[c].push_back(sR);
			}
		}
		break;
	}
}

// sau khi doc het tep: bracket -> replay ket qua (bKeep=1) -> toan cuc (thu tu goc G 0x08059A7F / 0x08059800)
static void sArenaLoadFinish()
{
	if (!s_pArenaLoad)
		return;
	for (int c = 1; c <= 7; c++)
	{
		KJx2CityArena* pA = &s_Arena[c];
		if (pA->vecFee.size() != pA->vecTong.size())
			pA->vecFee.assign(pA->vecTong.size(), s_nSettings[0]);	// goc G: mac dinh SignUpFee
		if (!pA->vecBracket.empty())
		{
			sBuildArenaPairs(c, 0);
			for (int i = 0; i < (int)s_pArenaLoad->vecR[c].size(); i++)
			{
				KJx2ArenaResult* pR = &s_pArenaLoad->vecR[c][i];
				sAddArenaResultCore(c, pR->wA, pR->wB, pR->btResult, 1);
			}
		}
	}
	if (s_pArenaLoad->bHasG)
	{
		s_bArenaRunning = s_pArenaLoad->bRunning ? 1 : 0;
		s_nArenaCity = (s_pArenaLoad->nCity >= 1 && s_pArenaLoad->nCity <= 7) ? s_pArenaLoad->nCity : -1;
	}
	delete s_pArenaLoad;
	s_pArenaLoad = NULL;
}

static void sArenaSaveLines(FILE* f)
{
	fprintf(f, "G %d %d\n", s_bArenaRunning, s_nArenaCity);
	for (int c = 1; c <= 7; c++)
	{
		KJx2CityArena* pA = &s_Arena[c];
		for (int i = 0; i < (int)pA->vecTong.size(); i++)
		{
			fprintf(f, "A %d %s\n", c, pA->vecTong[i].c_str());
			fprintf(f, "F %d %d\n", c, (i < (int)pA->vecFee.size()) ? pA->vecFee[i] : s_nSettings[0]);
		}
		if (!pA->vecBracket.empty())
		{
			fprintf(f, "B %d %d", c, (int)pA->vecBracket.size());
			for (int k = 0; k < (int)pA->vecBracket.size(); k++)
				fprintf(f, " %d", (int)(pA->vecBracket[k] & 0xFFFF));
			fprintf(f, "\n");
		}
		for (int i = 0; i < (int)pA->vecResult.size(); i++)
			fprintf(f, "R %d %d %d %d\n", c, pA->vecResult[i].wA, pA->vecResult[i].wB, pA->vecResult[i].btResult);
	}
}

//////////////////////////////////////////////////////////////////////
// 4 pha (goc: 4 API Lua cua relay, goi tu task script theo lich)
//////////////////////////////////////////////////////////////////////

// StartSignUp(c): R 0x0809AB8C - xoa du lieu thanh cu, state 1, tin SIGNUP
static int sStartSignUp(int c)
{
	KJx2City* p = &s_Cities[c];
	// (phan bien 6.7) tran tuan truoc treo state 3 -> cuong che tat truoc khi mo tuan moi
	if (s_bArenaRunning && s_nArenaCity == c)
	{
		s_bArenaRunning = 0;
		s_nArenaCity = -1;
		g_DebugLog((LPSTR)"KJx2CityWar[arena]: StartSignUp thanh %d - cuong che tat loi dai treo", c);
	}
	sArenaClear(c);
	p->nState = JX2CW_STATE_SIGNUP;
	char szMsg[512];
	sprintf(szMsg, CWS_NOTIFY_SIGNUP, p->szAreaName, s_nSettings[1]);
	sLocalNews(szMsg);
	sSaveMirror();
	return 1;
}

// CutTo16 (R 0x0809B736): sap theo phi giam dan, tu vi tri 16 -> hoan tien + bao SIGNUP_OUT
static void sCutTo16(int c)
{
	KJx2CityArena* pA = &s_Arena[c];
	int N = (int)pA->vecTong.size();
	if (N <= JX2CW_SIGNUP_MAX)
		return;
	for (int i = 0; i < N - 1; i++)
	{
		for (int j = i + 1; j < N; j++)
		{
			if (pA->vecFee[j] > pA->vecFee[i])
			{
				std::swap(pA->vecFee[i], pA->vecFee[j]);
				std::swap(pA->vecTong[i], pA->vecTong[j]);
			}
		}
	}
	char szMsg[512];
	for (int i = N - 1; i >= JX2CW_SIGNUP_MAX; i--)
	{
		KTongJX2_AddMoneyC(g_FileName2Id((LPSTR)pA->vecTong[i].c_str()), (__int64)pA->vecFee[i]);
		sprintf(szMsg, CWS_NOTIFY_SIGNUP_OUT, s_Cities[c].szAreaName);
		sMsgTongMaster(pA->vecTong[i].c_str(), szMsg);
		g_DebugLog((LPSTR)"KJx2CityWar[arena]: cat bang [%s] (phi %d) khoi loi dai thanh %d, hoan tien",
			pA->vecTong[i].c_str(), pA->vecFee[i], c);
		pA->vecTong.erase(pA->vecTong.begin() + i);
		pA->vecFee.erase(pA->vecFee.begin() + i);
	}
}

// EndSignUp(c): R 0x0809C0BA
static int sEndSignUp(int c)
{
	KJx2City* p = &s_Cities[c];
	KJx2CityArena* pA = &s_Arena[c];
	char szMsg[512];
	if (p->nState != JX2CW_STATE_SIGNUP)
	{
		sprintf(szMsg, CWS_WARN_PERIODNOTMATCH, JX2CW_STATE_SIGNUP, p->nState);
		g_DebugLog((LPSTR)"KJx2CityWar[arena]: EndSignUp thanh %d - %s", c, szMsg);
		return 0;
	}
	int N = (int)pA->vecTong.size();
	if (N == 0)
	{
		p->nState = JX2CW_STATE_NORMAL;
		sprintf(szMsg, CWS_NOTIFY_NO_SIGNUP, p->szAreaName);
		sLocalNews(szMsg);
	}
	else if (N == 1)
	{
		p->nState = JX2CW_STATE_WARDECIDED;
		sStrCpy(p->szChallenger, pA->vecTong[0].c_str(), sizeof(p->szChallenger));
		KTongJX2_SetFieldC(g_FileName2Id((LPSTR)p->szChallenger), 11, 3);	// TongState 3 = khieu chien gia
		sprintf(szMsg, CWS_NOTIFY_SINGLE_SIGNUP, p->szAreaName, p->szChallenger);
		sLocalNews(szMsg);
	}
	else
	{
		sCutTo16(c);
		sBuildBracket(c);
		p->nState = JX2CW_STATE_BRACKET;
		sprintf(szMsg, CWS_NOTIFY_SIGNUP_COMPLETE, p->szAreaName, (int)pA->vecTong.size());
		sLocalNews(szMsg);
	}
	sSaveMirror();
	return 1;
}

// StartArena(c): R 0x0809CF60 - doi state 2, bracket != rong, chua co ket qua, khong co loi dai khac
static int sStartArena(int c)
{
	KJx2City* p = &s_Cities[c];
	KJx2CityArena* pA = &s_Arena[c];
	char szMsg[512];
	if (p->nState != JX2CW_STATE_BRACKET)
	{
		sprintf(szMsg, CWS_WARN_PERIODNOTMATCH, JX2CW_STATE_BRACKET, p->nState);
		g_DebugLog((LPSTR)"KJx2CityWar[arena]: StartArena thanh %d - %s", c, szMsg);
		return 0;
	}
	if (pA->vecBracket.empty() || !pA->vecResult.empty() || s_bArenaRunning)
	{
		g_DebugLog((LPSTR)"KJx2CityWar[arena]: StartArena thanh %d bi tu choi (bracket %d, ketqua %d, running %d/%d)",
			c, (int)pA->vecBracket.size(), (int)pA->vecResult.size(), s_bArenaRunning, s_nArenaCity);
		return 0;
	}
	for (int i = 0; i < (int)pA->vecTong.size(); i++)
		KTongJX2_SetFieldC(g_FileName2Id((LPSTR)pA->vecTong[i].c_str()), 11, 2);	// TongState 2 = dang dau loi dai
	p->nState = JX2CW_STATE_ARENA;
	s_nArenaCity = c;
	s_bArenaRunning = 1;
	sBuildArenaPairs(c, 0);
	sprintf(szMsg, CWS_NOTIFY_ARENA_START, p->szAreaName);
	sLocalNews(szMsg);
	sSaveMirror();
	return 1;
}

// StartCityWar(c): R 0x0809E13C - doi szChallenger != rong; chu rong -> chiem luon; co chu -> state 5
static int sStartCityWar(int c)
{
	KJx2City* p = &s_Cities[c];
	char szMsg[512];
	if (!p->szChallenger[0])
	{
		g_DebugLog((LPSTR)"KJx2CityWar[arena]: StartCityWar thanh %d - khong co khieu chien gia", c);
		return 0;
	}
	if (s_bArenaRunning && s_nArenaCity == c)
	{
		// loi dai chua ket (phan bien 6.7): cuong che tat, giu challenger hien co
		s_bArenaRunning = 0;
		s_nArenaCity = -1;
		g_DebugLog((LPSTR)"KJx2CityWar[arena]: StartCityWar thanh %d - cuong che tat loi dai treo", c);
	}
	if (!p->szOwnerTong[0])
	{
		char szOldOwner[64], szOldMaster[64];
		sStrCpy(szOldOwner, p->szOwnerTong, sizeof(szOldOwner));
		sStrCpy(szOldMaster, p->szMaster, sizeof(szOldMaster));
		sStrCpy(p->szOwnerTong, p->szChallenger, sizeof(p->szOwnerTong));
		sMasterOfTong(p->szOwnerTong, p->szMaster, sizeof(p->szMaster));
		p->nOccupyDate = sToday();
		KTongJX2_SetFieldC(g_FileName2Id((LPSTR)p->szChallenger), 11, 0);
		p->szChallenger[0] = 0;
		p->nState = JX2CW_STATE_NORMAL;
		sOwnerChanged(c, szOldOwner, szOldMaster);
		sprintf(szMsg, CWS_NOTIFY_WAR_RESULT3, p->szAreaName, p->szOwnerTong);
		sLocalNews(szMsg);
		sSaveMirror();
		sApplyCityToSubWorld(c);
		return 1;
	}
	KTongJX2_SetFieldC(g_FileName2Id((LPSTR)p->szOwnerTong), 11, 4);
	KTongJX2_SetFieldC(g_FileName2Id((LPSTR)p->szChallenger), 11, 4);
	p->nState = JX2CW_STATE_ATWAR;
	sprintf(szMsg, CWS_NOTIFY_WAR_STARTED, p->szAreaName, p->szOwnerTong, p->szChallenger);
	sLocalNews(szMsg);
	sSaveMirror();
	return 1;
}

// SignUp (GS 0x08058650 + R 0x0809AE32): 10 dieu kien (bo "khao nghiem")
static void sSignUpArena(Lua_State* L, int c, int nFee)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0)
		return;
	KJx2City* p = &s_Cities[c];
	KJx2CityArena* pA = &s_Arena[c];
	char szMsg[512];
	const char* szTong = Player[nPlayerIndex].m_cTong.m_szName;
	DWORD dwTongID = Player[nPlayerIndex].m_cTong.m_dwTongNameID;
	if (!szTong[0] || !dwTongID)
		return;		// (1) khong bang -> goc bo qua im lang
	KTongJX2Tong* pTong = g_TongJX2.FindTong(dwTongID);
	if (!pTong)
	{
		sMsgPlayer(nPlayerIndex, CWS_WARN_TONGERROR);
		return;
	}
	char szMaster[64];
	sMasterOfTong(szTong, szMaster, sizeof(szMaster));
	if (strcmp(szMaster, Player[nPlayerIndex].m_PlayerName) != 0)
	{
		sMsgPlayer(nPlayerIndex, CWS_WARN_NOTTONGLEADER);
		return;
	}
	if (p->nState != JX2CW_STATE_SIGNUP)
	{
		sMsgPlayer(nPlayerIndex, CWS_WARN_SIGNUPTIMEOUT);
		return;
	}
	for (int i = 1; i <= 7; i++)
	{
		int nSt = s_Cities[i].nState;
		if (nSt >= JX2CW_STATE_SIGNUP && nSt <= JX2CW_STATE_ARENA)
		{
			for (int k = 0; k < (int)s_Arena[i].vecTong.size(); k++)
			{
				if (s_Arena[i].vecTong[k] == szTong)
				{
					sMsgPlayer(nPlayerIndex, CWS_WARN_ALREADYSIGNUP);
					return;
				}
			}
		}
	}
	for (int i = 1; i <= 7; i++)
	{
		if (strcmp(s_Cities[i].szChallenger, szTong) == 0)
		{
			sprintf(szMsg, CWS_WARN_BECHALLENGER, s_Cities[i].szAreaName);
			sMsgPlayer(nPlayerIndex, szMsg);
			return;
		}
	}
	for (int i = 1; i <= 7; i++)
	{
		if (strcmp(s_Cities[i].szOwnerTong, szTong) == 0)
		{
			sprintf(szMsg, CWS_WARN_BECITYOWNER, s_Cities[i].szAreaName);
			sMsgPlayer(nPlayerIndex, szMsg);
			return;
		}
	}
	if ((int)pTong->mapMember.size() < s_nSettings[4])
	{
		sprintf(szMsg, CWS_WARN_TOOFEWCROWS, s_nSettings[4]);
		sMsgPlayer(nPlayerIndex, szMsg);
		return;
	}
	if ((int)g_TongJX2.GetField(dwTongID, 6) < s_nSettings[1])	// field 6 = cap bang (TONG_GetExpLevel)
	{
		sprintf(szMsg, CWS_WARN_LEVELLOW, s_nSettings[1]);
		sMsgPlayer(nPlayerIndex, szMsg);
		return;
	}
	if (nFee < s_nSettings[0] || KTongJX2_GetMoneyC(dwTongID) < (__int64)nFee)	// so sanh CO DAU (phan bien)
	{
		sprintf(szMsg, CWS_WARN_NOTENOUGHFEE, nFee);
		sMsgPlayer(nPlayerIndex, szMsg);
		return;
	}
	KTongJX2_AddMoneyC(dwTongID, -(__int64)nFee);
	pA->vecTong.push_back(std::string(szTong));
	pA->vecFee.push_back(nFee);
	sMsgPlayer(nPlayerIndex, CWS_NOTIFY_SIGNUP_OK);
	sprintf(szMsg, CWS_NOTIFY_SIGNUP_TOALL, szTong, p->szAreaName);
	sLocalNews(szMsg);
	g_DebugLog((LPSTR)"KJx2CityWar[arena]: bang [%s] bao danh loi dai thanh %d, phi %d (nguoi %s)",
		szTong, c, nFee, Player[nPlayerIndex].m_PlayerName);
	sSaveMirror();
}

// NotifyArenaResult (G 0x08122800 + R 0x0809D4F0): ghi ket qua, tin, ket cay -> challenger
static void sNotifyArenaResult(int nArenaID, int bAWin)
{
	if (nArenaID < 0 || nArenaID >= JX2CW_ARENA_SLOTS)
		return;
	int c = s_nArenaCity;
	if (!s_bArenaRunning || c < 1 || c > 7 || s_Cities[c].nState != JX2CW_STATE_ARENA)
	{
		g_DebugLog((LPSTR)"KJx2CityWar[arena]: NotifyArenaResult(%d) khi khong co loi dai (running %d city %d)",
			nArenaID, s_bArenaRunning, c);
		return;
	}
	KJx2CityArena* pA = &s_Arena[c];
	if (nArenaID >= (int)pA->vecPair.size() || pA->vecPair[nArenaID].btFlag != 0)
	{
		g_DebugLog((LPSTR)"KJx2CityWar[arena]: NotifyArenaResult(%d) repeat error (pair %d)", nArenaID, (int)pA->vecPair.size());
		return;
	}
	int nA = pA->vecPair[nArenaID].wA, nB = pA->vecPair[nArenaID].wB;
	int nRes = bAWin ? 1 : 2;
	int nErr = sAddArenaResultCore(c, nA, nB, nRes, 0);
	if (nErr)
	{
		g_DebugLog((LPSTR)"KJx2CityWar[arena]: AddArenaResult loi %d (A %d B %d res %d)", nErr, nA, nB, nRes);
		return;
	}
	const char* szWin = sTongName(c, bAWin ? nA : nB);
	const char* szLose = sTongName(c, bAWin ? nB : nA);
	char szMsg[512];
	if (s_bArenaRunning)
	{
		sprintf(szMsg, CWS_NOTIFY_ARENA_RESULT, szWin, szLose);
		sLocalNews(szMsg);
	}
	else
	{
		// het cay: vo dich = ben thang cua ban ghi vua nhan (R 0x0809D856)
		KJx2City* p = &s_Cities[c];
		sStrCpy(p->szChallenger, szWin, sizeof(p->szChallenger));
		p->nState = JX2CW_STATE_WARDECIDED;
		s_nArenaCity = -1;
		for (int i = 0; i < (int)pA->vecTong.size(); i++)
			KTongJX2_SetFieldC(g_FileName2Id((LPSTR)pA->vecTong[i].c_str()), 11, 0);
		KTongJX2_SetFieldC(g_FileName2Id((LPSTR)p->szChallenger), 11, 3);
		sprintf(szMsg, CWS_NOTIFY_ARENA_RESULT2, szWin, szLose, p->szAreaName);
		sLocalNews(szMsg);
	}
	sSaveMirror();
}

//////////////////////////////////////////////////////////////////////
// Ham Lua - 4 pha + bao danh (ten trung API relay goc)
//////////////////////////////////////////////////////////////////////
int LuaStartSignUp(Lua_State* L)	{ sEnsureStore(); int c = sArgCity(L, 1); Lua_PushNumber(L, c ? sStartSignUp(c) : 0); return 1; }
int LuaEndSignUp(Lua_State* L)		{ sEnsureStore(); int c = sArgCity(L, 1); Lua_PushNumber(L, c ? sEndSignUp(c) : 0); return 1; }
int LuaStartArena(Lua_State* L)		{ sEnsureStore(); int c = sArgCity(L, 1); Lua_PushNumber(L, c ? sStartArena(c) : 0); return 1; }
int LuaStartCityWar(Lua_State* L)	{ sEnsureStore(); int c = sArgCity(L, 1); Lua_PushNumber(L, c ? sStartCityWar(c) : 0); return 1; }

// SignUpCityWarArena(CityID, Fee) - infocenter_head.lua:118 (SignUpFinal)
int LuaSignUpCityWarArena(Lua_State* L)
{
	sEnsureStore();
	int c = sArgCity(L, 1);
	if (c && Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
		sSignUpArena(L, c, (int)Lua_ValueToNumber(L, 2));
	return 0;
}

//////////////////////////////////////////////////////////////////////
// 14 ham Lua phia GS (muc 4 dac ta) - thay stub E4
//////////////////////////////////////////////////////////////////////
static int sArgArenaID(Lua_State* L)
{
	if (!Lua_IsNumber(L, 1))
		return -1;
	int n = (int)Lua_ValueToNumber(L, 1);
	return (n >= 0 && n < JX2CW_ARENA_SLOTS) ? n : -1;
}

// (0..7) -> 1 khi cap nArenaID cua thanh dang dau ton tai va chua co ket qua
int LuaIsArenaBegin(Lua_State* L)
{
	sEnsureStore();
	int id = sArgArenaID(L);
	int nRet = 0;
	if (id >= 0 && s_bArenaRunning && s_nArenaCity >= 1 && s_nArenaCity <= 7)
	{
		KJx2CityArena* pA = &s_Arena[s_nArenaCity];
		if (id < (int)pA->vecPair.size() && pA->vecPair[id].btFlag == 0)
			nRet = 1;
	}
	Lua_PushNumber(L, nRet);
	return 1;
}

// (0..7) -> ten A, ten B (chi khi flag == 0); khac -> "",""
int LuaGetArenaBothSides(Lua_State* L)
{
	sEnsureStore();
	int id = sArgArenaID(L);
	if (id >= 0 && s_nArenaCity >= 1 && s_nArenaCity <= 7)
	{
		KJx2CityArena* pA = &s_Arena[s_nArenaCity];
		if (id < (int)pA->vecPair.size() && pA->vecPair[id].btFlag == 0)
		{
			Lua_PushString(L, (char*)sTongName(s_nArenaCity, pA->vecPair[id].wA));
			Lua_PushString(L, (char*)sTongName(s_nArenaCity, pA->vecPair[id].wB));
			return 2;
		}
	}
	Lua_PushString(L, (char*)"");
	Lua_PushString(L, (char*)"");
	return 2;
}

// (nArenaID - bo qua) -> thanh dang dau loi dai (1..7) / 0
int LuaGetArenaCityArea(Lua_State* L)
{
	sEnsureStore();
	Lua_PushNumber(L, (s_nArenaCity >= 1 && s_nArenaCity <= 7) ? s_nArenaCity : 0);
	return 1;
}

int LuaGetArenaTargetCity(Lua_State* L)
{
	sEnsureStore();
	Lua_PushNumber(L, (s_nArenaCity >= 1 && s_nArenaCity <= 7) ? s_nArenaCity : 0);
	return 1;
}

// () -> vong dang cho (1..) ; thanh sai -> 0 gia tri (nhu goc)
int LuaGetArenaLevel(Lua_State* L)
{
	sEnsureStore();
	if (s_nArenaCity < 1 || s_nArenaCity > 7)
		return 0;
	Lua_PushNumber(L, s_Arena[s_nArenaCity].nLevel);
	return 1;
}

int LuaGetArenaTotalLevel(Lua_State* L)
{
	sEnsureStore();
	int n = 0;
	if (s_nArenaCity >= 1 && s_nArenaCity <= 7)
		n = sLog2((int)s_Arena[s_nArenaCity].vecBracket.size());
	Lua_PushNumber(L, n);
	return 1;
}

int LuaGetArenaTotalLevelByCity(Lua_State* L)
{
	sEnsureStore();
	int c = sArgCity(L, 1);
	Lua_PushNumber(L, c ? sLog2((int)s_Arena[c].vecBracket.size()) : 0);
	return 1;
}

static void sCatLimited(char* szDst, int nDstSize, const char* szAdd)
{
	int nCur = (int)strlen(szDst), nAdd = (int)strlen(szAdd);
	if (nCur + nAdd >= nDstSize - 1)
		nAdd = nDstSize - 1 - nCur;
	if (nAdd > 0)
	{
		memcpy(szDst + nCur, szAdd, nAdd);
		szDst[nCur + nAdd] = 0;
	}
}

// (1..7) -> lich vong hien tai (G 0x08122000); chua co cap -> CWS_DATA_8
int LuaGetArenaSchedule(Lua_State* L)
{
	sEnsureStore();
	int c = sArgCity(L, 1);
	char szOut[1024];
	szOut[0] = 0;
	if (c)
	{
		KJx2CityArena* pA = &s_Arena[c];
		int nLevels = sLog2((int)pA->vecBracket.size());
		if (pA->vecPair.empty() || nLevels == 0 || pA->nLevel == 0)
			sStrCpy(szOut, CWS_DATA_8, sizeof(szOut));
		else
		{
			char szLine[256];
			sStrCpy(szOut, CWS_DATA_0, sizeof(szOut));
			if (pA->nLevel == nLevels)
				sCatLimited(szOut, sizeof(szOut), CWS_DATA_1);
			else if (pA->nLevel == nLevels - 1)
				sCatLimited(szOut, sizeof(szOut), CWS_DATA_2);
			else
			{
				sprintf(szLine, CWS_DATA_3, (unsigned)pA->nLevel);
				sCatLimited(szOut, sizeof(szOut), szLine);
			}
			for (int i = 0; i < (int)pA->vecPair.size(); i++)
			{
				KJx2ArenaPair* pP = &pA->vecPair[i];
				if (i)
					sCatLimited(szOut, sizeof(szOut), CWS_DATA_7);
				if (pP->btFlag == 0)
					sprintf(szLine, CWS_DATA_4, sTongName(c, pP->wA), sTongName(c, pP->wB));
				else if (pP->btFlag == 1)
					sprintf(szLine, CWS_DATA_5, sTongName(c, pP->wA), sTongName(c, pP->wB));
				else
					sprintf(szLine, CWS_DATA_5, sTongName(c, pP->wB), sTongName(c, pP->wA));
				sCatLimited(szOut, sizeof(szOut), szLine);
			}
		}
	}
	Lua_PushString(L, szOut);
	return 1;
}

// (1..7[,nLevel bo qua]) -> moi ket qua da co (G 0x08121D00); rong -> CWS_DATA_8
int LuaGetArenaInfoByCity(Lua_State* L)
{
	sEnsureStore();
	int c = sArgCity(L, 1);
	char szOut[1024];
	szOut[0] = 0;
	if (c)
	{
		KJx2CityArena* pA = &s_Arena[c];
		if (pA->vecResult.empty())
			sStrCpy(szOut, CWS_DATA_8, sizeof(szOut));
		else
		{
			char szLine[256];
			sprintf(szOut, CWS_DATA_6, s_Cities[c].szAreaName);
			for (int i = 0; i < (int)pA->vecResult.size(); i++)
			{
				KJx2ArenaResult* pR = &pA->vecResult[i];
				if (pR->btResult == 1)
					sprintf(szLine, CWS_DATA_5, sTongName(c, pR->wA), sTongName(c, pR->wB));
				else
					sprintf(szLine, CWS_DATA_5, sTongName(c, pR->wB), sTongName(c, pR->wA));
				sCatLimited(szOut, sizeof(szOut), szLine);
				sCatLimited(szOut, sizeof(szOut), "\n");
			}
		}
	}
	Lua_PushString(L, szOut);
	return 1;
}

// (nArenaID, bSide1Win) -> 0 gia tri; head.lua:143/147 WinBonus
int LuaNotifyArenaResult(Lua_State* L)
{
	sEnsureStore();
	int id = sArgArenaID(L);
	if (id >= 0 && Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
		sNotifyArenaResult(id, ((int)Lua_ValueToNumber(L, 2) != 0) ? 1 : 0);
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Nhom ham Lua
//////////////////////////////////////////////////////////////////////

// (1..7) -> szOwnerTong, szMaster ; fail -> "","" (citybulletin.lua:15 nhan 2;
// citywar_function.lua:219 + relay :131 dung gia tri 1 so "" la vo chu)
int LuaGetCityOwner(Lua_State* L)
{
	sEnsureStore();
	int n = sArgCity(L, 1);
	Lua_PushString(L, n ? s_Cities[n].szOwnerTong : (char*)"");
	Lua_PushString(L, n ? s_Cities[n].szMaster : (char*)"");
	return 2;
}

// (1..7) -> (CONG = challenger, THU = owner); chi tra khi CA HAI non-empty
// (head.lua:302-310: GetGamerName(1) = doi 2 = thu; mission.lua:86 dao thu tu la CHU DICH goc)
int LuaGetCityWarBothSides(Lua_State* L)
{
	sEnsureStore();
	int n = sArgCity(L, 1);
	if (n && s_Cities[n].szChallenger[0] && s_Cities[n].szOwnerTong[0])
	{
		Lua_PushString(L, s_Cities[n].szChallenger);
		Lua_PushString(L, s_Cities[n].szOwnerTong);
	}
	else
	{
		Lua_PushString(L, (char*)"");
		Lua_PushString(L, (char*)"");
	}
	return 2;
}

int LuaGetCityAreaName(Lua_State* L)
{
	sEnsureStore();
	int n = sArgCity(L, 1);
	Lua_PushString(L, n ? s_Cities[n].szAreaName : (char*)"");
	return 1;
}

// () -> khu 0..7 cua MAP dang chay script (global "SubWorld" cua state goi)
int LuaGetCityArea(Lua_State* L)
{
	sEnsureStore();
	int nCity = 0;
	int nIdx = GetSubWorldIndex(L);
	if (nIdx >= 0)
	{
		int nMapId = SubWorld[nIdx].m_SubWorldID;
		for (int i = 1; i <= 7 && !nCity; i++)
		{
			for (int m = 0; m < s_Cities[i].nMapIdCount; m++)
			{
				if (s_Cities[i].nMapIds[m] == nMapId)
				{
					nCity = i;
					break;
				}
			}
		}
	}
	Lua_PushNumber(L, nCity);
	return 1;
}

// (1..7) -> chuoi mo ta toi gian (goc 6 nhanh theo btState - E6 lam giau bang
// bo chuoi G_CITYWAR_*); citybulletin.lua:4 noi chuoi nen KHONG duoc tra nil
int LuaGetCitySummary(Lua_State* L)
{
	sEnsureStore();
	int n = sArgCity(L, 1);
	char szOut[512];
	szOut[0] = 0;
	if (n)
	{
		KJx2City* p = &s_Cities[n];
		if (!p->szOwnerTong[0])
			sprintf(szOut, "Thµnh %s hiÖn v« chñ.", p->szAreaName);
		else
			sprintf(szOut, "Thµnh %s do bang [%s] trÊn gi÷, Th¸i Thó lµ [%s].",
				p->szAreaName, p->szOwnerTong, p->szMaster);
		if (p->nState == JX2CW_STATE_WARDECIDED && p->szChallenger[0])
		{
			char szLine[128];
			sprintf(szLine, "\nBang [%s] sÏ khiªu chiÕn thµnh nµy.", p->szChallenger);
			strcat(szOut, szLine);
		}
		else if (p->nState == JX2CW_STATE_ATWAR)
			strcat(szOut, "\nChiÕn sù ®ang diÔn ra!");
	}
	Lua_PushString(L, szOut);
	return 1;
}

// goc gui goi 0xAC (809B, 7 thanh) cho client JX2 - client ta khong hieu -> no-op
int LuaGetAllCitySummary(Lua_State* L)
{
	return 0;
}

int LuaSyncCitySummary(Lua_State* L)
{
	return 0;
}

// goc gui 0xA3 [BYTE 1][DWORD tax1][DWORD tax2] - ta thay bang UI rieng o E7
int LuaOpenCityManageUI(Lua_State* L)
{
	return 0;
}

// (1..7) -> 0/1; PHAI tra SO (GetWarOfCity so sanh ~= 0, nil se lam thanh 1 luon dung)
int LuaHaveBeginWar(Lua_State* L)
{
	sEnsureStore();
	int n = sArgCity(L, 1);
	Lua_PushNumber(L, (n && s_Cities[n].nState == JX2CW_STATE_ATWAR) ? 1 : 0);
	return 1;
}

// (1..7, bAttackerWin) - cong thang: owner=challenger + master + occupydate;
// MOI nhanh: clear challenger + state ve thuong (thieu la timer 18 poll 5' mo lai
// tran VO HAN). KHONG dung field 48 - script CITY_OCCUPY_R goc tu goi
// TONG_ApplySetOccupyCityDay (tong_mix.lua:936 ban server1, noi o E6).
int LuaNotifyWarResult(Lua_State* L)
{
	sEnsureStore();
	int n = sArgCity(L, 1);
	if (n && Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
	{
		int bAttackerWin = ((int)Lua_ValueToNumber(L, 2) != 0) ? 1 : 0;
		KJx2City* p = &s_Cities[n];
		if (bAttackerWin && p->szChallenger[0])
		{
			char szOldOwner[64], szOldMaster[64];
			sStrCpy(szOldOwner, p->szOwnerTong, sizeof(szOldOwner));
			sStrCpy(szOldMaster, p->szMaster, sizeof(szOldMaster));
			sStrCpy(p->szOwnerTong, p->szChallenger, sizeof(p->szOwnerTong));
			sMasterOfTong(p->szOwnerTong, p->szMaster, sizeof(p->szMaster));
			p->nOccupyDate = sToday();
			sOwnerChanged(n, szOldOwner, szOldMaster);
		}
		p->szChallenger[0] = 0;
		p->nState = JX2CW_STATE_NORMAL;
		sSaveMirror();
		sApplyCityToSubWorld(n);
		g_DebugLog((LPSTR)"KJx2CityWar: NotifyWarResult thanh %d, cong thang=%d, chu=[%s]",
			n, bAttackerWin, p->szOwnerTong);
	}
	return 0;
}

// (szCityName, szTongName) - TEN THANH (relay 19h: thanh vo chu -> bo nhiem ngay).
// E6 se cap Title Thai Thu (152+cityid) cho tan + thu cua cuu tai duong script.
int LuaAppointViceroy(Lua_State* L)
{
	sEnsureStore();
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsString(L, 1) && Lua_IsString(L, 2))
	{
		int n = sCityByName(Lua_ValueToString(L, 1));
		const char* szTong = Lua_ValueToString(L, 2);
		if (n && szTong && szTong[0])
		{
			KJx2City* p = &s_Cities[n];
			char szOldOwner[64], szOldMaster[64];
			sStrCpy(szOldOwner, p->szOwnerTong, sizeof(szOldOwner));
			sStrCpy(szOldMaster, p->szMaster, sizeof(szOldMaster));
			sStrCpy(p->szOwnerTong, szTong, sizeof(p->szOwnerTong));
			sMasterOfTong(szTong, p->szMaster, sizeof(p->szMaster));
			p->nOccupyDate = sToday();
			// don rac chu ky loi truoc: bo nhiem thang = thanh KHONG co tran
			// (phan bien E3 #5 - khong don thi GetCityWarBothSides bat gia)
			p->szChallenger[0] = 0;
			p->nState = JX2CW_STATE_NORMAL;
			sArenaClear(n);		// [LOI DAI CN] bo nhiem tay = xoa bang dau/ket qua cua thanh
			sOwnerChanged(n, szOldOwner, szOldMaster);
			sSaveMirror();
			sApplyCityToSubWorld(n);
			g_DebugLog((LPSTR)"KJx2CityWar: AppointViceroy thanh %d chu=[%s] thaithu=[%s]",
				n, p->szOwnerTong, p->szMaster);
		}
		else
			// %.60s: g_DebugLog dem 256B + vsprintf khong chan, doi 1 la chuoi Lua dai tuy y
			g_DebugLog((LPSTR)"KJx2CityWar: AppointViceroy truot (thanh [%.60s] khong khop ini)",
				Lua_IsString(L, 1) ? Lua_ValueToString(L, 1) : "");
	}
	return 0;
}

// (szCityName, szTongName) -> ghi khieu chien gia + state WARDECIDED (mai 20h danh)
int LuaAppointChallenger(Lua_State* L)
{
	sEnsureStore();
	if (Lua_GetTopIndex(L) >= 2 && Lua_IsString(L, 1) && Lua_IsString(L, 2))
	{
		int n = sCityByName(Lua_ValueToString(L, 1));
		const char* szTong = Lua_ValueToString(L, 2);
		if (n && szTong && szTong[0])
		{
			KJx2City* p = &s_Cities[n];
			sStrCpy(p->szChallenger, szTong, sizeof(p->szChallenger));
			p->nState = JX2CW_STATE_WARDECIDED;
			sArenaClear(n);		// [LOI DAI CN] goc R 0x080988C4: xoa toan bo bao danh/bracket/ket qua thanh
			if (s_nArenaCity == n) { s_bArenaRunning = 0; s_nArenaCity = -1; }
			sSaveMirror();
			g_DebugLog((LPSTR)"KJx2CityWar: AppointChallenger thanh %d khieuchien=[%s]",
				n, p->szChallenger);
		}
		else
			g_DebugLog((LPSTR)"KJx2CityWar: AppointChallenger truot (thanh [%.60s] khong khop ini)",
				Lua_IsString(L, 1) ? Lua_ValueToString(L, 1) : "");
	}
	return 0;
}

// (1..7) -> 1 khi dang trong pha bao danh (league 508 cua thanh, task 1 == 1 -
// relay bat 18h ha 19h; infocenter_head.lua:90,102,107 so == 1)
int LuaIsSigningUp(Lua_State* L)
{
	sEnsureStore();
	int n = sArgCity(L, 1);
	// [LOI DAI CN 21/08] nguon = state thanh (goc G 0x08115930), khong con league 508
	Lua_PushNumber(L, (n && s_Cities[n].nState == JX2CW_STATE_SIGNUP) ? 1 : 0);
	return 1;
}

// (1..7) -> so bang da bao danh (member cua league 508); 0 call site Lua -
// giu dung ngu nghia "kho bao danh" cua DIEUTRA muc 1
int LuaNumOfSignUpTongs(Lua_State* L)
{
	sEnsureStore();
	int n = sArgCity(L, 1);
	// [LOI DAI CN 21/08] goc G 0x080FDEF0: chi dem khi state 1..3
	int nRet = 0;
	if (n && s_Cities[n].nState >= JX2CW_STATE_SIGNUP && s_Cities[n].nState <= JX2CW_STATE_ARENA)
		nRet = (int)s_Arena[n].vecTong.size();
	Lua_PushNumber(L, nRet);
	return 1;
}

// (1..7, nIndex 0-based) -> ten bang ; fail -> ""
int LuaGetSignUpTongName(Lua_State* L)
{
	sEnsureStore();
	int n = sArgCity(L, 1);
	const char* szName = "";
	if (n && Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
		szName = sTongName(n, (int)Lua_ValueToNumber(L, 2));	// [LOI DAI CN 21/08] goc G 0x0812C690
	Lua_PushString(L, (char*)szName);
	return 1;
}

// co bit tren NGUOI CHOI o ban goc; 0 call site Lua -> stub an toan
int LuaDisabledChatCity(Lua_State* L)
{
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaIsDisabledChatCity(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

// (1..7, nState 0..5) - [LOI DAI CN 21/08] chi con cho admin ep trang thai; lich
// chinh thuc dung StartSignUp/EndSignUp/StartArena/StartCityWar (ten goc relay)
int LuaCTC_JX2_SetCityState(Lua_State* L)
{
	sEnsureStore();
	int n = sArgCity(L, 1);
	int nOk = 0;
	if (n && Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
	{
		int nState = (int)Lua_ValueToNumber(L, 2);
		if (nState >= JX2CW_STATE_NORMAL && nState <= JX2CW_STATE_ATWAR)
		{
			s_Cities[n].nState = nState;
			sSaveMirror();
			nOk = 1;
		}
	}
	Lua_PushNumber(L, nOk);
	return 1;
}

// (szTongName) -> 1 = dang THU thanh co tran, 2 = dang CONG, nil = khong thuoc
// tran nao (DIEUTRA muc 2: "1 so / nil")
int LuaGetCityWarTongCamp(Lua_State* L)
{
	sEnsureStore();
	if (Lua_IsString(L, 1))
	{
		const char* szTong = Lua_ValueToString(L, 1);
		if (szTong && szTong[0])
		{
			for (int i = 1; i <= 7; i++)
			{
				if (s_Cities[i].nState == JX2CW_STATE_NORMAL)
					continue;
				if (strcmp(s_Cities[i].szOwnerTong, szTong) == 0)
				{
					Lua_PushNumber(L, 1);
					return 1;
				}
				if (strcmp(s_Cities[i].szChallenger, szTong) == 0)
				{
					Lua_PushNumber(L, 2);
					return 1;
				}
			}
		}
	}
	return 0;	// nil
}

//////////////////////////////////////////////////////////////////////
// ArenaCredits (E4): task value nguoi choi, remap 3179 -> 2894
// (MAX_TASK = 3000; 3179 se im lang vo hieu - KPlayerTask.cpp:69-81)
//////////////////////////////////////////////////////////////////////
#define JX2CW_ARENACREDIT_TASK 2894

int LuaGetArenaCredits(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	Lua_PushNumber(L, (nPlayerIndex > 0) ?
		Player[nPlayerIndex].m_cTask.GetSaveVal(JX2CW_ARENACREDIT_TASK) : 0);
	return 1;
}

static int sArenaCreditWrite(Lua_State* L, int nMode)	// 0 set / 1 add / 2 sub
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex > 0 && Lua_IsNumber(L, 1))
	{
		int nVal = (int)Lua_ValueToNumber(L, 1);
		int nCur = Player[nPlayerIndex].m_cTask.GetSaveVal(JX2CW_ARENACREDIT_TASK);
		int nNew = (nMode == 0) ? nVal : (nMode == 1) ? nCur + nVal : nCur - nVal;
		if (nNew < 0)
			nNew = 0;
		Player[nPlayerIndex].m_cTask.SetSaveVal(JX2CW_ARENACREDIT_TASK, nNew);
	}
	return 0;
}

int LuaSetArenaCredits(Lua_State* L)	{ return sArenaCreditWrite(L, 0); }
int LuaAddArenaCredits(Lua_State* L)	{ return sArenaCreditWrite(L, 1); }
int LuaReduceArenaCredits(Lua_State* L)	{ return sArenaCreditWrite(L, 2); }

//////////////////////////////////////////////////////////////////////
// E7 (khong protocol moi): dat thue qua thoai NPC quan thanh.
// (nCityID, nTax) -> 0 OK / 1 khong phai Thai Thu / 2 ngoai khung gio
// (StartSetTaxTime..EndSetTaxTime, goc 22h-23h) / 3 hom nay da dat
// (TAXALREADYSET1) / 4 tham so sai. Goc mo UI 0xA3 - client ta khong co;
// gating + kinh te y het goc, UI = menu thoai (deviation ghi ban giao).
//////////////////////////////////////////////////////////////////////
int LuaCTC_JX2_SetTax(Lua_State* L)
{
	sEnsureStore();
	int nRet = 4;
	int nPlayerIndex = GetPlayerIndex(L);
	int n = sArgCity(L, 1);
	if (n && nPlayerIndex > 0 && Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
	{
		int nTax = (int)Lua_ValueToNumber(L, 2);
		KJx2City* p = &s_Cities[n];
		if (nTax < 0 || nTax > s_nSettings[2])
			nRet = 4;
		else if (!p->szMaster[0] ||
			strcmp(p->szMaster, Player[nPlayerIndex].m_PlayerName) != 0)
			nRet = 1;
		else
		{
			time_t t = time(NULL);
			struct tm* pTm = localtime(&t);
			int nHr = pTm ? pTm->tm_hour : -1;
			if (nHr < s_nSettings[5] || nHr >= s_nSettings[6])
				nRet = 2;
			else if (p->nTaxSetDate == sToday())
				nRet = 3;
			else
			{
				p->nTax = nTax;
				p->nTaxSetDate = sToday();
				sSaveMirror();
				sApplyCityToSubWorld(n);
				g_DebugLog((LPSTR)"KJx2CityWar: Thai Thu [%.60s] dat thue thanh %d = %d",
					Player[nPlayerIndex].m_PlayerName, n, nTax);
				nRet = 0;
			}
		}
	}
	Lua_PushNumber(L, nRet);
	return 1;
}

// (nCityID) -> nTax hien tai (cho thoai quan thanh hien so)
int LuaCTC_JX2_GetTax(Lua_State* L)
{
	sEnsureStore();
	int n = sArgCity(L, 1);
	Lua_PushNumber(L, n ? s_Cities[n].nTax : 0);
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Cho ScriptFuns (de-hardcode setter map 78) + E7
//////////////////////////////////////////////////////////////////////
BOOL KJx2CityWar_SetTaxByCity(int nCityID, int nTax)
{
	sEnsureStore();
	if (nCityID < 1 || nCityID > 7)
		return FALSE;
	if (nTax < 0)
		nTax = 0;
	if (nTax > s_nSettings[2])
		nTax = s_nSettings[2];
	s_Cities[nCityID].nTax = nTax;
	sSaveMirror();
	sApplyCityToSubWorld(nCityID);
	return TRUE;
}

BOOL KJx2CityWar_SetOwnerByCity(int nCityID, const char* szOwnerRaw)
{
	sEnsureStore();
	if (nCityID < 1 || nCityID > 7)
		return FALSE;
	KJx2City* p = &s_Cities[nCityID];
	// trim space 2 dau: caller Lua cu quen dem " %s " thi cung khong lot vao store
	// (space dau con pha round-trip mirror - %n cua sscanf nuot; phan bien E3 #7)
	while (szOwnerRaw && *szOwnerRaw == ' ')
		szOwnerRaw++;
	sStrCpy(p->szOwnerTong, szOwnerRaw, sizeof(p->szOwnerTong));
	int nLen = (int)strlen(p->szOwnerTong);
	while (nLen > 0 && p->szOwnerTong[nLen - 1] == ' ')
		p->szOwnerTong[--nLen] = 0;
	sMasterOfTong(p->szOwnerTong, p->szMaster, sizeof(p->szMaster));
	sSaveMirror();
	sApplyCityToSubWorld(nCityID);
	return TRUE;
}

int KJx2CityWar_GetSetting(int nIdx)
{
	sEnsureStore();
	if (nIdx < 0 || nIdx >= 9)
		return 0;
	return s_nSettings[nIdx];
}

#endif // _SERVER
