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
	while (fgets(szLine, sizeof(szLine), f))
	{
		sChopEol(szLine);
		if (szLine[0] && szLine[1] != ' ')
			continue;	// dong hong/cut - bo qua, khong parse "ma" byte cu trong dem
		if (szLine[0] == 'C')
		{
			int nId = 0, nState = 0, nTax = 0, nPP = 0, nDate = 0;
			if (sscanf(szLine + 2, "%d %d %d %d %d", &nId, &nState, &nTax, &nPP, &nDate) == 5 &&
				nId >= 1 && nId <= 7)
			{
				s_Cities[nId].nState = (nState >= 0 && nState <= 2) ? nState : JX2CW_STATE_NORMAL;
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
}

static void sSaveMirror()
{
	char szTmp[MAX_PATH], szPath[MAX_PATH];
	sCityPath(szTmp, true);
	sCityPath(szPath, false);
	FILE* f = fopen(szTmp, "wb");
	if (!f)
		return;
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
	int nRet = 0;
	if (n && KJx2League_GetLeagueTaskC(508, s_Cities[n].szAreaName, 1) == 1)
		nRet = 1;
	Lua_PushNumber(L, nRet);
	return 1;
}

// (1..7) -> so bang da bao danh (member cua league 508); 0 call site Lua -
// giu dung ngu nghia "kho bao danh" cua DIEUTRA muc 1
int LuaNumOfSignUpTongs(Lua_State* L)
{
	sEnsureStore();
	int n = sArgCity(L, 1);
	Lua_PushNumber(L, n ? KJx2League_GetMemberCountC(508, s_Cities[n].szAreaName) : 0);
	return 1;
}

// (1..7, nIndex 0-based) -> ten bang ; fail -> ""
int LuaGetSignUpTongName(Lua_State* L)
{
	sEnsureStore();
	int n = sArgCity(L, 1);
	const char* szName = "";
	if (n && Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
		szName = KJx2League_GetMemberNameC(508, s_Cities[n].szAreaName,
			(int)Lua_ValueToNumber(L, 2));
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

// (1..7, nState 0..2) - ha tang cho tick 5 pha (E6: 20h flip ATWAR de timer 18
// poll HaveBeginWar mo mission); KHONG co trong ban goc (goc: relay StartCityWar
// gui goi rieng) - ten co tien to CTC_JX2_ de khong dung ten goc
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

//////////////////////////////////////////////////////////////////////
// Nhom ARENA (E4) - idle nhu nhanh VN goc (DIEUTRA muc 2). Dang ky du de
// script nap + NPC thoai duoc; moi gia tri = "chua bat dau".
//////////////////////////////////////////////////////////////////////
int LuaIsArenaBegin(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetArenaBothSides(Lua_State* L)
{
	Lua_PushString(L, (char*)"");
	Lua_PushString(L, (char*)"");
	return 2;
}

int LuaGetArenaCityArea(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetArenaLevel(Lua_State* L)
{
	Lua_PushNumber(L, 0);	// 0 = chua bat dau / da xong
	return 1;
}

int LuaGetArenaTargetCity(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetArenaTotalLevel(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetArenaTotalLevelByCity(Lua_State* L)
{
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetArenaSchedule(Lua_State* L)
{
	Lua_PushString(L, (char*)"");	// call site duy nhat dem Say - "" an toan
	return 1;
}

int LuaGetArenaInfoByCity(Lua_State* L)
{
	Lua_PushString(L, (char*)"");
	return 1;
}

int LuaNotifyArenaResult(Lua_State* L)
{
	return 0;	// nuot nhu goc VN (loi dai khong chay)
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
