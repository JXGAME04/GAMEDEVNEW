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
#define CWS_NOTIFY_SIGNUP			"@@G_CITYWAR_NOTIFY_SIGNUP@@"
#define CWS_NOTIFY_SIGNUP_OK		"@@G_CITYWAR_NOTIFY_SIGNUP_OK@@"
#define CWS_NOTIFY_SIGNUP_TOALL		"@@G_CITYWAR_NOTIFY_SIGNUP_TOALL@@"
#define CWS_NOTIFY_NO_SIGNUP		"@@G_CITYWAR_NOTIFY_NO_SIGNUP@@"
#define CWS_NOTIFY_SINGLE_SIGNUP	"@@G_CITYWAR_NOTIFY_SINGLE_SIGNUP@@"
#define CWS_NOTIFY_SIGNUP_COMPLETE	"@@G_CITYWAR_NOTIFY_SIGNUP_COMPLETE@@"
#define CWS_NOTIFY_ARENA_START		"@@G_CITYWAR_NOTIFY_ARENA_START@@"
#define CWS_NOTIFY_ARENA_RESULT		"@@G_CITYWAR_NOTIFY_ARENA_RESULT@@"
#define CWS_NOTIFY_ARENA_RESULT2	"@@G_CITYWAR_NOTIFY_ARENA_RESULT2@@"
#define CWS_NOTIFY_WAR_STARTED		"@@G_CITYWAR_NOTIFY_WAR_STARTED@@"
#define CWS_NOTIFY_WAR_RESULT3		"@@G_CITYWAR_NOTIFY_WAR_RESULT3@@"
#define CWS_NOTIFY_SIGNUP_OUT		"@@G_CITYWAR_NOTIFY_SIGNUP_OUT@@"
#define CWS_NOTIFY_SIGNUP_OUT2		"@@G_CITYWAR_NOTIFY_SIGNUP_OUT2@@"
#define CWS_WARN_SIGNUPTIMEOUT		"@@G_CITYWAR_WARNING_SIGNUPTIMEOUT@@"
#define CWS_WARN_ALREADYSIGNUP		"@@G_CITYWAR_WARNING_ALREADYSIGNUP@@"
#define CWS_WARN_NOTENOUGHFEE		"@@G_CITYWAR_WARNING_NOTENOUGHFEE@@"
#define CWS_WARN_LEVELLOW			"@@G_CITYWAR_WARNING_LEVELLOW@@"
#define CWS_WARN_NOTTONGLEADER		"@@G_CITYWAR_WARNING_NOTTONGLEADER@@"
#define CWS_WARN_TONGERROR			"@@G_CITYWAR_WARNING_TONGERROR@@"
#define CWS_WARN_BECHALLENGER		"@@G_CITYWAR_WARNING_BECHALLENGER@@"
#define CWS_WARN_BECITYOWNER		"@@G_CITYWAR_WARNING_BECITYOWNER@@"
#define CWS_WARN_TOOFEWCROWS		"@@G_CITYWAR_WARNING_TOOFEWCROWS@@"
#define CWS_WARN_PERIODNOTMATCH		"@@G_CITYWAR_WARNING_PERIODNOTMATCH@@"
#define CWS_DATA_0					"@@G_CITYWARDATA_0@@"
#define CWS_DATA_1					"@@G_CITYWARDATA_1@@"
#define CWS_DATA_2					"@@G_CITYWARDATA_2@@"
#define CWS_DATA_3					"@@G_CITYWARDATA_3@@"
#define CWS_DATA_4					"@@G_CITYWARDATA_4@@"
#define CWS_DATA_5					"@@G_CITYWARDATA_5@@"
#define CWS_DATA_6					"@@G_CITYWARDATA_6@@"
#define CWS_DATA_7					", "
#define CWS_DATA_8					"@@G_CITYWARDATA_8@@"

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
	for (int i = 1; i < MAX_PLAYER; i++)
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
