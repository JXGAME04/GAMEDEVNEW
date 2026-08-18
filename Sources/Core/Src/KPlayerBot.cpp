//---------------------------------------------------------------------------
// KPlayerBot.cpp - xem KPlayerBot.h.
//
// Core build voi PCH "Use" qua KCore.h - moi thu TRUOC dong include nay deu bi bo qua,
// nen KCore.h PHAI dung dau tien (giong KSimCity.cpp / KJx2WarInfra.cpp).
//---------------------------------------------------------------------------
#include "KCore.h"
#include "KWin32.h"

#ifdef _SERVER

#include "LuaLib.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "KFaction.h"       // FACTIONS_PRR_SERIES - so phai moi he
#include "KRandom.h"        // g_Random
#include "KFilePath.h"      // g_FileName2Id - doi duong script -> ma bam de nhan dien NPC
#include "KPlayerBot.h"
// TRoleData / S3DBI_RoleBaseInfo. Dung DUNG duong dan ma CoreServerShell.cpp:33 dang dung
// de chac chan lay cung mot ban voi phan con lai cua Core (cay nay co nhieu ban
// S3DBInterface.h: Headers\, Lib\, Sources\Core\lib\ - chung KHAC NHAU).
#include "../../lib/S3DBInterface.h"
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------- trang thai
enum PB_STATE
{
	PB_FREE = 0,
	PB_WAIT_ROLELIST,     // da hoi danh sach nhan vat, cho tra loi
	PB_WAIT_ROLEDATA,     // da hoi du lieu nhan vat, cho blob
};

struct PB_Pending
{
	int             nState;
	unsigned long   ulIdentity;
	int             nWaited;                  // so tick da cho
	char            szAccount[64];
	char            szRole[64];
};

// ---------------------------------------------------------------- viec cua bot
// Dot nay bot chi co MOT viec: tu chay bo toi NPC mon phai dung ngu hanh roi xin vao.
// Cac muc sau (xem tin tuc, noi chuyen, luyen cap) noi tiep bang cach them trang thai,
// khong phai dung lai khung.
enum PB_AI
{
	PB_AI_IDLE = 0,       // khong co viec gi
	PB_AI_GOTO_FACTION,   // dang chay bo toi NPC mon phai
	PB_AI_IN_FACTION,     // da vao phai xong
	PB_AI_GIVEUP,         // khong toi duoc (khong tim thay NPC / khong co duong)
};

// ---------------------------------------------------------------------------
// 10 NPC MON PHAI.
//
// Ca 10 deu dung o map 53 (Ba Lang Huyen) - CHINH la map bot sinh ra - trong mot khoi
// 60x81 o quanh tam (1612,3168), nen bot chi phai chay nhieu nhat ~45 o.
// Nguon: script\startgame\thon\balanghuyen.lua:57-66 (goi tu startgame.lua:163).
//
// CO Y KHONG DONG CUNG TOA DO o day: NPC duoc tim LUC CHAY theo m_ActionScriptID
// (= g_FileName2Id cua duong script, gan tai ScriptFuns.cpp:6394). Chu game doi cho
// dat NPC trong lua thi bot van tim dung, khong phai sua lai ma C++ va build lai.
//
// Thu tu 0..9 PHAI khop FACTION_INFO[] cua script\header\factionhead.lua:5-17 va bang
// FactionName[] tai KNpc.cpp:11064-11074 - da doi chieu du 10/10. Chinh so nay la tham
// so cua gianhapmonphai(n).
//
// nSeries la ngu hanh BAT BUOC: check_yes (factionhead.lua:89-95) chan thang neu
// GetSeries() cua nhan vat khac ngu hanh cua phai. Cong thuc phai = series*2 + {0,1}
// khop 10/10 voi bang nay.
// ---------------------------------------------------------------------------
struct PB_FacNpc
{
	int         nSeries;      // 0 Kim, 1 Moc, 2 Thuy, 3 Hoa, 4 Tho
	const char* szScript;     // duong script NPC = khoa tim NPC trong the gioi
	const char* szTen;        // ten phai, chi de ghi nhat ky (ASCII)
};

static const PB_FacNpc s_facNpc[MAX_FACTION] =
{
	{ 0, "\\script\\npcthon\\npcmonphai\\thieulam.lua",   "Thieu Lam"   },  // 0
	{ 0, "\\script\\npcthon\\npcmonphai\\thienvuong.lua", "Thien Vuong" },  // 1
	{ 1, "\\script\\npcthon\\npcmonphai\\duongmon.lua",   "Duong Mon"   },  // 2
	{ 1, "\\script\\npcthon\\npcmonphai\\ngudoc.lua",     "Ngu Doc"     },  // 3
	{ 2, "\\script\\npcthon\\npcmonphai\\ngami.lua",      "Nga Mi"      },  // 4
	{ 2, "\\script\\npcthon\\npcmonphai\\thuyyen.lua",    "Thuy Yen"    },  // 5
	{ 3, "\\script\\npcthon\\npcmonphai\\caibang.lua",    "Cai Bang"    },  // 6
	{ 3, "\\script\\npcthon\\npcmonphai\\thiennhan.lua",  "Thien Nhan"  },  // 7
	{ 4, "\\script\\npcthon\\npcmonphai\\vodang.lua",     "Vo Dang"     },  // 8
	{ 4, "\\script\\npcthon\\npcmonphai\\conlon.lua",     "Con Lon"     },  // 9
};

// Dung sat NPC bao nhieu thi coi la "toi noi". 96 MPS = 3 o, dung tam doi thoai.
// KHONG nham vao DUNG o cua NPC: JX1 tinh NPC LA TUONG nen o do luon la vat can.
#define PB_FAC_ARRIVE_MPS   96
// So lan A* that bai lien tiep truoc khi bo cuoc, va giai cho giua hai lan.
#define PB_FAC_MAX_RETRY    5
#define PB_FAC_RETRY_TICK   (GAME_FPS * 3)

struct PB_Bot
{
	int          nPlayerIdx;
	DWORD        dwID;                        // chot danh tinh, chong tai su dung khe
	char         szAccount[64];
	int          nAi;                         // PB_AI_*
	int          nFaction;                    // phai muc tieu 0..9, -1 = chua chon
	int          nRetry;                      // so lan A* that bai lien tiep
	unsigned int nNextTry;                    // moc tick duoc phep thu lai
	PB_WalkState walk;                        // trang thai di bo A*
};

static PB_Pending   s_pending[PB_MAX_PENDING];
static PB_Bot       s_bots[PB_MAX_BOTS];
static int          s_botCount   = 0;
static PB_DbSender  s_pfnSend    = NULL;

// Hang doi tai khoan cho sinh. Vong tron don gian.
static char         s_queue[PB_MAX_BOTS * 2][64];
static int          s_qHead = 0, s_qTail = 0;

// ulIdentity cua bot LUON bat bit 31 de GameServer loc duoc khoi luong tra loi thong thuong.
// KHONG bao gio phat 0xFFFFFFFF: gia tri do da duoc duong luu nhan vat dung lam gia tri canh
// (KSOServer.cpp:3164 sProcessData.ulIdentity = -1).
static unsigned long s_ulNext = 0x80000001ul;

static unsigned long pb_NextIdentity()
{
	if (s_ulNext >= 0xFFFFFFF0ul)
		s_ulNext = 0x80000001ul;
	return s_ulNext++;
}

// ---------------------------------------------------------------- tro giup
static PB_Pending* pb_AllocPending()
{
	for (int i = 0; i < PB_MAX_PENDING; i++)
		if (s_pending[i].nState == PB_FREE)
			return &s_pending[i];
	return NULL;
}

static PB_Pending* pb_FindPending(unsigned long ulIdentity)
{
	for (int i = 0; i < PB_MAX_PENDING; i++)
		if (s_pending[i].nState != PB_FREE && s_pending[i].ulIdentity == ulIdentity)
			return &s_pending[i];
	return NULL;
}

static void pb_FreePending(PB_Pending* p)
{
	if (p)
		memset(p, 0, sizeof(PB_Pending));
}

// ---------------------------------------------------------------- cua ngoai
void PB_SetSender(PB_DbSender pfn)
{
	s_pfnSend = pfn;
}

int PB_GetCount()
{
	return s_botCount;
}

int PB_IsBot(int nPlayerIdx)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return 0;
	for (int i = 0; i < s_botCount; i++)
		if (s_bots[i].nPlayerIdx == nPlayerIdx)
			return 1;
	return 0;
}

int PB_Spawn(const char* szAccountName)
{
	if (!szAccountName || !szAccountName[0])
		return 0;
	if ((int)strlen(szAccountName) >= 32)      // Player[].m_AccoutName gioi han
		return 0;
	if (s_botCount >= PB_MAX_BOTS)
		return 0;

	int nNext = (s_qTail + 1) % (PB_MAX_BOTS * 2);
	if (nNext == s_qHead)                      // hang doi day
		return 0;

	strncpy(s_queue[s_qTail], szAccountName, 63);
	s_queue[s_qTail][63] = 0;
	s_qTail = nNext;
	return 1;
}

int PB_SpawnRange(int nFrom, int nTo)
{
	int nOk = 0;
	char sz[32];
	for (int i = nFrom; i <= nTo; i++)
	{
		sprintf(sz, "%d", i);
		if (!PB_Spawn(sz))
			break;                             // day hang doi hoac cham tran
		nOk++;
	}
	return nOk;
}

// ---------------------------------------------------------------- gui yeu cau
static int pb_AskRoleList(const char* szAccount)
{
	if (!s_pfnSend)
		return 0;
	PB_Pending* p = pb_AllocPending();
	if (!p)
		return 0;

	p->nState     = PB_WAIT_ROLELIST;
	p->ulIdentity = pb_NextIdentity();
	p->nWaited    = 0;
	strncpy(p->szAccount, szAccount, sizeof(p->szAccount) - 1);
	p->szAccount[sizeof(p->szAccount) - 1] = 0;
	p->szRole[0] = 0;

	if (!s_pfnSend(PB_ASK_ROLELIST, p->ulIdentity, p->szAccount))
	{
		pb_FreePending(p);
		return 0;
	}
	return 1;
}

// ---------------------------------------------------------------- nhan tra loi
void PB_OnRoleList(const PB_DB_RESULT* pRes)
{
	if (!pRes || !pRes->pPayload || pRes->nPayloadLen < 1)
		return;
	PB_Pending* p = pb_FindPending(pRes->ulIdentity);
	if (!p || p->nState != PB_WAIT_ROLELIST)
		return;

	// Dinh dang do Goddess dung (Goddess\ClientNode.cpp:329-336):
	//   pDataBuffer[0]   = so nhan vat
	//   pDataBuffer[1..] = mang S3DBI_RoleBaseInfo
	// GameServer da boc bo phan dau TProcessData, pPayload tro thang vao pDataBuffer.
	const BYTE* pb    = (const BYTE*)pRes->pPayload;
	int         nRole = pb[0];
	if (nRole <= 0)
	{
		// Tai khoan khong co nhan vat nao trong roledb -> bo qua lang le.
		pb_FreePending(p);
		return;
	}
	if (pRes->nPayloadLen < 1 + (int)sizeof(S3DBI_RoleBaseInfo))
	{
		pb_FreePending(p);
		return;
	}

	const S3DBI_RoleBaseInfo* pList = (const S3DBI_RoleBaseInfo*)(pb + 1);
	if (!pList[0].szName[0])
	{
		pb_FreePending(p);
		return;
	}

	// Chi lay nhan vat DAU TIEN => moi tai khoan dung mot bot.
	strncpy(p->szRole, pList[0].szName, sizeof(p->szRole) - 1);
	p->szRole[sizeof(p->szRole) - 1] = 0;

	p->nState     = PB_WAIT_ROLEDATA;
	p->ulIdentity = pb_NextIdentity();
	p->nWaited    = 0;

	if (!s_pfnSend || !s_pfnSend(PB_ASK_ROLEDATA, p->ulIdentity, p->szRole))
		pb_FreePending(p);
}

void PB_OnRoleData(const PB_DB_RESULT* pRes)
{
	if (!pRes || !pRes->pPayload || pRes->nPayloadLen <= 1)
		return;
	PB_Pending* p = pb_FindPending(pRes->ulIdentity);
	if (!p || p->nState != PB_WAIT_ROLEDATA)
		return;

	if (s_botCount >= PB_MAX_BOTS)
	{
		pb_FreePending(p);
		return;
	}

	// pDataBuffer[0] = co thanh cong, pDataBuffer[1..] = blob TRoleData.
	const BYTE* pb = (const BYTE*)pRes->pPayload;
	if (pb[0] != 1)
	{
		pb_FreePending(p);
		return;
	}
	const TRoleData* pData = (const TRoleData*)(pb + 1);
	if (pRes->nPayloadLen - 1 < (int)sizeof(TRoleData) || !pData->BaseInfo.szName[0])
	{
		pb_FreePending(p);
		return;
	}

	// MIN da biet: KPlayer::LaunchPlayer (KPlayer.cpp:6768) goi g_pServer->Release() roi
	// dong 6776 dung con tro NULL khi m_nChestPW == 214519 (nap tu BaseInfo.ipassrole,
	// KPlayerDBFuns.cpp:382). Mot blob mang gia tri do se giet tang mang may chu.
	if (pData->BaseInfo.ipassrole == 214519)
	{
		pb_FreePending(p);
		return;
	}

	// ---- Tu day soi guong CoreServerShell::AddCharacter (CoreServerShell.cpp:130-153) ----
	// Co y KHONG dich lai KBotManager cua cay tham khao: duong nguoi choi that o chinh cay
	// nay da la khuon dung nhat, di theo no thi it rui ro hon nhieu.
	GUID guid;
	memset(&guid, 0, sizeof(guid));
	guid.Data1 = 0xB07ul + (unsigned long)s_botCount;   // dai rieng, tranh dung GUID nguoi that

	int nIdx = PlayerSet.Add((char*)pData->BaseInfo.szName, &guid);
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
	{
		pb_FreePending(p);
		return;
	}

	strncpy(Player[nIdx].m_AccoutName, pData->BaseInfo.caccname, sizeof(Player[nIdx].m_AccoutName) - 1);
	Player[nIdx].m_AccoutName[sizeof(Player[nIdx].m_AccoutName) - 1] = 0;
	strncpy(Player[nIdx].m_PlayerName, pData->BaseInfo.szName, sizeof(Player[nIdx].m_PlayerName) - 1);
	Player[nIdx].m_PlayerName[sizeof(Player[nIdx].m_PlayerName) - 1] = 0;

	DWORD dwLen = pData->dwDataLen;
	if (dwLen > sizeof(Player[nIdx].m_SaveBuffer))
		dwLen = sizeof(Player[nIdx].m_SaveBuffer);
	ZeroMemory(Player[nIdx].m_SaveBuffer, sizeof(Player[nIdx].m_SaveBuffer));
	memcpy(Player[nIdx].m_SaveBuffer, pData, dwLen);
	Player[nIdx].m_pStatusLoadPlayerInfo = Player[nIdx].m_SaveBuffer;

	// ---- Nap du lieu: soi guong CoreServerShell::PlayerDbLoading (:155-181) ----
	// Nguoi that duoc nap dan qua nhieu khung theo nhip goi tin; bot khong co client nen
	// quay tai cho cho xong. Rao 200 vong de mot blob hong khong treo may chu.
	{
		int          nStep  = 0;
		unsigned int nParam = 0;
		int          nGuard = 0;
		while (nGuard++ < 200)
		{
			if (!Player[nIdx].LoadDBPlayerInfo((BYTE*)Player[nIdx].m_SaveBuffer, nStep, nParam))
				break;
		}
		Player[nIdx].m_pStatusLoadPlayerInfo = NULL;
	}

	// Bot khong co ket noi mang. PB_LIXIAN_BOT (=3) la thu giu cho bot khoi bi
	// IsLoginTimeOut giet sau 10 giay MA khong dam vao hai gia tri co nghia san (1 va 2) -
	// xem khoi chu thich dai tai KPlayerBot.h.
	Player[nIdx].m_nNetConnectIdx = -1;
	Player[nIdx].m_nLixian        = PB_LIXIAN_BOT;

	// LaunchPlayer tra 0 = THANH CONG o cay nay (KPlayer.cpp:6797 return 0).
	if (Player[nIdx].LaunchPlayer() != 0)
	{
		pb_FreePending(p);
		return;
	}

	// BAT CO GIAY PHEP. Duong nguoi choi that lam viec nay ngay sau LaunchPlayer:
	//   KSOServer.cpp:2826  AddPlayerToWorld2(nIndex, m_LicReg)
	//   -> CoreServerShell.cpp:192  Player[nIndex].LaunchPlayer2(value)
	//   -> KPlayer.cpp:6760  m_nLicReg = value
	// Bo qua buoc nay thi m_nLicReg giu nguyen false (dat o KPlayer.cpp:327) va NAM thu
	// se HONG LANG LE, khong mot dong bao loi:
	//   KPlayer.cpp:4037 + 4060  AddFaction  -> khong vao phai duoc
	//   KPlayer.cpp:4878         EatMecidine -> khong an thuoc duoc
	//   KPlayer.cpp:6726/6752    CostMoney / AddMoney -> khong tieu/nhan tien duoc
	//   KPlayer.cpp:6552         (return som)
	Player[nIdx].LaunchPlayer2(true);

	// ---- CHON PHAI, NHUNG CHUA VAO ----
	//
	// Ban truoc goi thang KPlayer::AddFaction (KPlayer.cpp:4058) ngay tai day. Chay thi
	// chay, nhung THIEU HAI THU va do la ly do "ten khong doi mau" va "khong co ky nang":
	//
	//   1. AddFaction chi goi Npc[].SetCamp() -> dat m_Camp. Ma MAU TEN tren man hinh do
	//      DUY NHAT m_CurrentCamp quyet dinh (KNpc::PaintInfo switch(m_CurrentCamp),
	//      KNpc.cpp:5868). Client nhan NPC_CHGCAMP_SYNC cung chi ghi m_Camp
	//      (KProtocolProcess.cpp:547). => ten bot giu nguyen mau trang.
	//   2. AddFaction khong he hoc ky nang.
	//
	// Nguoi choi THAT khong dinh vi ho di duong Lua: gianhapmonphai(n)
	// (script\header\factionhead.lua:19-30) lam DU NAM viec -
	//   SetFaction -> SetCamp -> SetCurCamp -> SetRank -> hockynang
	// tuc co ca SetCurCamp (mau ten) lan hockynang (ky nang). Vay thi cho bot di dung
	// duong do luon, thay vi va tung trieu chung mot.
	//
	// Va vi gianhapmonphai() nam trong ham go() cua script NPC (vd thieulam.lua:41-43) -
	// dung thu ma nguoi choi bam vao khi chon "Gia nhap ... phai" - nen bot phai DEN
	// NOI truoc da. Chon phai o day, con viec chay bo + goi go() thi de pb_DriveBot lo.
	//
	// Phai PHAI khop ngu hanh, hai lop cung chan:
	//   KPlayerFaction::AddFaction (KPlayerFaction.cpp:78-81) doi nFactionID thuoc
	//     [nSeries*FACTIONS_PRR_SERIES, (nSeries+1)*FACTIONS_PRR_SERIES)
	//   check_yes (factionhead.lua:89-95) doi GetSeries() == series cua phai.
	// Ngu hanh cua bot den tu BaseInfo.ifiveprop -> Npc[].m_Series (KPlayerDBFuns.cpp:392),
	// ma tool taobot_bdb da rai deu 0..4 => moi bot co dung 2 phai hop le, boc ngau nhien 1.
	int nFaction = -1;
	{
		int nNpcIdx = Player[nIdx].m_nIndex;
		if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
		{
			int nSeries = Npc[nNpcIdx].m_Series;
			if (nSeries >= series_metal && nSeries < series_num)
				nFaction = nSeries * FACTIONS_PRR_SERIES
				         + (int)g_Random(FACTIONS_PRR_SERIES);
		}
	}

	{
		PB_Bot& b = s_bots[s_botCount];
		b.nPlayerIdx = nIdx;
		b.dwID       = Player[nIdx].m_dwID;
		strncpy(b.szAccount, p->szAccount, sizeof(b.szAccount) - 1);
		b.szAccount[sizeof(b.szAccount) - 1] = 0;
		b.nFaction   = nFaction;
		b.nAi        = (nFaction >= 0) ? PB_AI_GOTO_FACTION : PB_AI_IDLE;
		b.nRetry     = 0;
		b.nNextTry   = 0;
		b.walk.Reset();          // khe co the da dung cho bot truoc do -> phai xoa lo trinh cu
		s_botCount++;
	}

	pb_FreePending(p);
}

// ---------------------------------------------------------------- go bot
int PB_RemoveAll()
{
	int nGo = 0;
	for (int i = 0; i < s_botCount; i++)
	{
		int nIdx = s_bots[i].nPlayerIdx;
		if (nIdx <= 0 || nIdx >= MAX_PLAYER)
			continue;
		// IsMatch la ham cua KNpc, KPlayer khong co - so thang m_dwID de chac khe chua bi
		// cap lai cho nguoi khac.
		if (Player[nIdx].m_dwID != s_bots[i].dwID)
			continue;
		// Duong go dung dan can them mot dot rieng (chuoi PrepareRemove -> WaitForRemove ->
		// SavePlayerData bi chan boi m_nNetConnectIdx == -1 se tra FALSE va lam khe ket
		// vinh vien). Tam thoi chi danh dau de khong ai coi day la bot nua.
		nGo++;
	}
	s_botCount = 0;
	return nGo;
}

// ============================================================================
// Ham Lua cho lenh GM. Dang ky trong ScriptFuns.cpp (bang GameScriptFuns[]).
// ============================================================================

// PB_AddBot(nTu, nDen) -> so tai khoan da xep vao hang doi.
//   Bot duoc sinh DAN theo nhip (PB_DRAIN_PER_TICK moi tick) chu khong sinh mot lua,
//   de khong nghen hang gui cua GameServer sang Goddess.
int LuaPB_AddBot(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nTu  = (int)Lua_ValueToNumber(L, 1);
	int nDen = (Lua_GetTopIndex(L) >= 2) ? (int)Lua_ValueToNumber(L, 2) : nTu;
	if (nTu < 1 || nDen < nTu)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	// kep theo tran bot de mot lenh go nham khong lam trao hang doi
	if (nDen - nTu + 1 > PB_MAX_BOTS)
		nDen = nTu + PB_MAX_BOTS - 1;

	Lua_PushNumber(L, PB_SpawnRange(nTu, nDen));
	Lua_PushNumber(L, PB_MAX_BOTS);      // tra them tran de Lua bao cho GM biet
	return 2;
}

// PB_BotCount() -> so bot dang song
int LuaPB_BotCount(Lua_State* L)
{
	Lua_PushNumber(L, PB_GetCount());
	Lua_PushNumber(L, PB_MAX_BOTS);
	return 2;
}

// PB_ClearBot() -> so bot da go khoi so
int LuaPB_ClearBot(Lua_State* L)
{
	Lua_PushNumber(L, PB_RemoveAll());
	return 1;
}

// ============================================================================
// DI BO BANG A* SERVER
//
// Vi sao khong phat thang MOT lenh di toi dich:
//   KNpcFindPath chi cho ne DUNG MOT vat can moi lenh SendCommand
//   (KNpcFindPath.cpp:106-111), vat can thu hai la DoStand vinh vien; cong voi viec JX1
//   tinh NPC LA TUONG khi buoc sang o moi (KSubWorld::TestBarrierMin ->
//   KRegion::GetBarrierMin(bCheckNpc = TRUE)). Nen di xa bang men-tuong la ket.
//   A* tra ve lo trinh DA NE SAN: moi chang giua hai waypoint la duong thong, gioi han
//   mot-lan-ne khong con can nua.
//
// Khac gi bot NPC SimCity: o do KHONG co A* nen phai lach bang cach chia lenh thanh nhieu
// buoc ngan 128 MPS (SC_STEP_MPS) - moi buoc mua duoc mot "tin dung ne". Co A* that roi thi
// lop lach do thanh thua, nen KHONG dung lai co che cua KSimCity o day.
// ============================================================================

#define PB_TILE(v)   ((v) / 32)   // MPS -> o luoi (1 o = 32 MPS)

int PB_WalkTo(int nNpcIdx, int nDstMpsX, int nDstMpsY, int nSubIdx,
              PB_WalkState& st, int nArriveMps)
{
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC || nSubIdx < 0 || nSubIdx >= MAX_SUBWORLD)
		return -1;
	if (nDstMpsX <= 0 || nDstMpsY <= 0)
		return -1;

	int bx = 0, by = 0;
	Npc[nNpcIdx].GetMpsPos(&bx, &by);

	// ---- B0: toi noi chua ----
	{
		__int64 dx = (__int64)bx - nDstMpsX;
		__int64 dy = (__int64)by - nDstMpsY;
		if (dx * dx + dy * dy <= (__int64)nArriveMps * nArriveMps)
		{
			st.Reset();
			return 1;
		}
	}

	const unsigned int now   = SubWorld[nSubIdx].m_dwCurrentTime;
	const int          nMap  = SubWorld[nSubIdx].m_SubWorldID;
	const int          tileX = PB_TILE(nDstMpsX);
	const int          tileY = PB_TILE(nDstMpsY);

	// ---- B5a: bo do DUNG YEN ----
	// Nhuc nhich hon 32 MPS (1 o) = dang di binh thuong. Dung yen qua 3 giay thuong la bi
	// mot NPC dung chan duong -> phai tim lai duong CO tinh NPC lam vat can.
	bool bForceRepath = false;
	{
		int mdx = bx - st.lastPosX; if (mdx < 0) mdx = -mdx;
		int mdy = by - st.lastPosY; if (mdy < 0) mdy = -mdy;
		if (st.lastMoveTick == 0 || mdx > 32 || mdy > 32)
		{
			st.lastPosX = bx; st.lastPosY = by; st.lastMoveTick = now;
		}
		else if (now - st.lastMoveTick >= (unsigned int)(GAME_FPS * 3))
		{
			st.lastPosX = bx; st.lastPosY = by; st.lastMoveTick = now;
			bForceRepath = true;
		}
	}

	// ---- B1: tinh (lai) lo trinh ----
	// Khoa theo (ban do, O LUOI cua dich) chu khong theo MPS: dich xe dich vai MPS thi khong
	// phai tinh lai ca lo trinh.
	const bool bNeedPath = st.path.empty()
	                    || st.mapId     != nMap
	                    || st.destTileX != tileX
	                    || st.destTileY != tileY
	                    || bForceRepath;
	if (bNeedPath)
	{
		if (!bForceRepath)
		{
			st.repathCount    = 0;
			st.noAdvanceCalls = 0;
		}
		st.mapId     = nMap;
		st.destTileX = tileX;
		st.destTileY = tileY;
		st.idx       = 0;

		// bCheckNpc chi bat khi da tung ket: quet co tinh NPC dat hon nhieu, ma NPC thi hay
		// tu tan di, nen lan dau cu di duong "tinh" cho re.
		const bool bCheckNpc = (bForceRepath || st.repathCount > 0);
		int nRet = SubWorld[nSubIdx].FindPathServer(bx, by, nDstMpsX, nDstMpsY, st.path, bCheckNpc);
		if (nRet <= 0 || st.path.empty())
		{
			st.path.clear();
			return -1;
		}
	}

	// ---- B2: tien con tro waypoint ----
	// Bo qua waypoint bot DA di qua: cach duoi 64 MPS (2 o) thi coi nhu xong.
	bool bAdvanced = false;
	while (st.idx < (int)st.path.size())
	{
		int wx = 0, wy = 0;
		if (!SubWorld[nSubIdx].BlockCenterMps(st.path[st.idx], wx, wy))
		{
			st.idx++; bAdvanced = true; continue;
		}
		__int64 dx = (__int64)bx - wx;
		__int64 dy = (__int64)by - wy;
		if (dx * dx + dy * dy <= (__int64)64 * 64)
		{
			st.idx++; bAdvanced = true; continue;
		}
		break;
	}

	if (bAdvanced)
	{
		st.noAdvanceCalls = 0;
	}
	else if (!st.path.empty() && ++st.noAdvanceCalls >= GAME_FPS * 2)
	{
		// ---- B5b: ~2 giay khong tien duoc waypoint nao ----
		st.noAdvanceCalls = 0;
		if (st.repathCount < 3)
		{
			st.repathCount++;
			SubWorld[nSubIdx].FindPathServer(bx, by, nDstMpsX, nDstMpsY, st.path, true);
			st.idx = 0;
		}
		else
		{
			// Thu 3 lan van khong nhuc nhich -> bo cuoc, de ben goi quyet dinh lam gi tiep.
			st.Reset();
			return -1;
		}
	}

	// Het waypoint ma chua toi dich = lo trinh chi MOT PHAN (FindPathServer tra 2).
	// Chang cuoi nham thang toi dich.
	int tx = nDstMpsX, ty = nDstMpsY;
	if (st.idx < (int)st.path.size())
	{
		int wx = 0, wy = 0;
		if (SubWorld[nSubIdx].BlockCenterMps(st.path[st.idx], wx, wy) && wx > 0 && wy > 0)
		{
			tx = wx; ty = wy;
		}
	}

	// ---- B3: phat lenh ----
	// CHI phat khi dich DOI, hoac khi engine da dung bot lai (do_stand). Phat lai cung mot
	// dich moi nhip vua ton goi tin (DoWalk broadcast s2c_npcwalk) vua lam pathfinder cua
	// engine reset lien tuc.
	const bool bStopped = (Npc[nNpcIdx].m_Doing == do_stand);
	if (tx != st.lastSendX || ty != st.lastSendY || bStopped)
	{
		Npc[nNpcIdx].SendCommand(do_run, tx, ty);
		st.lastSendX = tx;
		st.lastSendY = ty;
	}
	return 0;
}

// ============================================================================
// TU CHAY BO TOI NPC MON PHAI ROI XIN VAO
// ============================================================================

// Khe NPC mon phai da tim duoc, nho lai de khoi quet MAX_NPC moi nhip.
static int s_facNpcIdx[MAX_FACTION] = { 0 };

// Khe nay co con dung la NPC chay script do khong (khe co the da cap lai cho NPC khac).
static bool pb_IsFacNpc(int nNpcIdx, DWORD dwScriptId)
{
	return nNpcIdx > 0 && nNpcIdx < MAX_NPC
	    && Npc[nNpcIdx].m_ActionScriptID == dwScriptId
	    && Npc[nNpcIdx].m_SubWorldIndex >= 0
	    && Npc[nNpcIdx].m_RegionIndex   >= 0;
}

// Tim khe NPC cua phai nFaction trong the gioi. Tra 0 neu khong co.
//
// Nhan dien bang m_ActionScriptID chu KHONG bang toa do hay ma mau NPC: day moi la khoa
// dung ban chat - "NPC nao chay script gia nhap Thieu Lam" thi chinh la NPC Thieu Lam.
// g_FileName2Id (KFilePath.cpp:442) la ham bam THUAN, khong doc dia, nen goi thoai mai.
static int pb_FindFacNpc(int nFaction)
{
	if (nFaction < 0 || nFaction >= MAX_FACTION)
		return 0;

	const DWORD dwScriptId = g_FileName2Id((LPSTR)s_facNpc[nFaction].szScript);

	if (pb_IsFacNpc(s_facNpcIdx[nFaction], dwScriptId))
		return s_facNpcIdx[nFaction];

	s_facNpcIdx[nFaction] = 0;
	for (int i = 1; i < MAX_NPC; i++)
	{
		if (!pb_IsFacNpc(i, dwScriptId))
			continue;
		s_facNpcIdx[nFaction] = i;
		return i;
	}
	return 0;
}

// Mot nhip cua MOT bot.
static void pb_DriveBot(PB_Bot& b)
{
	const int nIdx = b.nPlayerIdx;
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)     return;
	if (Player[nIdx].m_dwID != b.dwID)       return;   // khe da cap lai cho nguoi khac

	const int nNpcIdx = Player[nIdx].m_nIndex;
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)  return;

	const int nSub = Npc[nNpcIdx].m_SubWorldIndex;
	if (nSub < 0 || nSub >= MAX_SUBWORLD)    return;

	if (b.nAi != PB_AI_GOTO_FACTION)
		return;

	// CO Y KHONG chan bang "m_cFaction.m_nCurFaction >= 0 thi coi nhu da vao phai".
	// Gia tri do nap thang tu blob roledb (KPlayerDBFuns.cpp:374 m_nCurFaction = nSect),
	// ma nhan vat mau do tool taobot_bdb nhan ban ra co the mang nSect = 0 - tuc doc len
	// thanh "da o Thieu Lam". Chan bang no thi MOI BOT deu dung im ngay tu dau ma khong
	// mot dong bao loi nao. Trang thai b.nAi la thu duy nhat dang tin o day.
	const unsigned int now = SubWorld[nSub].m_dwCurrentTime;
	if (b.nNextTry && now < b.nNextTry)
		return;

	const int nFacNpc = pb_FindFacNpc(b.nFaction);
	if (nFacNpc <= 0)
	{
		// Chua thay NPC trong the gioi. KHONG bo cuoc: bot co the duoc goi ra truoc khi
		// addnpcbalang() cua startgame.lua chay xong. Cu cho roi tim lai.
		b.nNextTry = now + PB_FAC_RETRY_TICK;
		return;
	}

	// Ca 10 NPC mon phai deu o map 53, dung noi bot sinh ra, nen lech ban do chi xay ra
	// neu bot da bi dich chuyen di cho khac - luc do di bo toi la vo nghia.
	if (Npc[nFacNpc].m_SubWorldIndex != nSub)
	{
		b.walk.Reset();
		b.nAi = PB_AI_GIVEUP;
		return;
	}

	int nx = 0, ny = 0;
	Npc[nFacNpc].GetMpsPos(&nx, &ny);

	const int nRet = PB_WalkTo(nNpcIdx, nx, ny, nSub, b.walk, PB_FAC_ARRIVE_MPS);

	if (nRet == 0)
	{
		b.nRetry = 0;                    // dang di duoc binh thuong
		return;
	}

	if (nRet < 0)
	{
		// Khong co duong / chua nap luoi. Nghi mot lat roi thu lai - vat can hay gap nhat
		// la NPC dung chan, ma NPC thi tu tan di, nen thu lai thuong an.
		b.walk.Reset();
		b.nNextTry = now + PB_FAC_RETRY_TICK;
		if (++b.nRetry >= PB_FAC_MAX_RETRY)
			b.nAi = PB_AI_GIVEUP;
		return;
	}

	// ---- TOI NOI: bam "gia nhap" DUNG NHU NGUOI CHOI ----
	//
	// go() cua script NPC (vd thieulam.lua:41-43) goi gianhapmonphai(n), tuc lam du
	// SetFaction -> SetCamp -> SetCurCamp -> SetRank -> hockynang
	// (script\header\factionhead.lua:19-30). Chinh la ham gan vao lua chon
	// "Gia nhap ... phai/go" ma nguoi choi bam trong hop thoai NPC.
	//
	// Di duong nay thay vi goi thang C++ AddFaction thi duoc CA HAI thu con thieu truoc
	// day: SetCurCamp (doi mau ten - KNpc::PaintInfo doc m_CurrentCamp) va hockynang
	// (hoc ky nang nhap mon). Khong phai va tung trieu chung nua.
	//
	// KPlayer::ExecuteScript (KPlayer.cpp:6816) tu dat SCRIPT_PLAYERINDEX = bot truoc khi
	// goi, nen moi ham Lua ben trong (GetSeries, AddMagic, SetCamp...) deu tac dung dung
	// len bot. bGlobal = false de KHONG ghi de m_ActionScriptID cua chinh bot.
	Npc[nNpcIdx].SendCommand(do_stand);          // dung lai truoc khi doi thoai
	Player[nIdx].ExecuteScript((char*)s_facNpc[b.nFaction].szScript, "go", 0, false);

	b.walk.Reset();
	b.nAi = PB_AI_IN_FACTION;

	// Bao ket qua RA CONSOLE. Duong Lua co the tu choi im lang (check_yes tra 0 khi ngu hanh
	// lech, hoac script chua nap), ma luc do bot van dung yen canh NPC trong nhu da xong.
	// In ra thi chu game nhin console la biet ngay ly do, khong phai doan.
	if (Player[nIdx].m_cFaction.m_nCurFaction == b.nFaction)
		printf("[Bot] %s da vao %s (phai %d, he %d)\n",
		       Player[nIdx].m_PlayerName, s_facNpc[b.nFaction].szTen,
		       b.nFaction, Npc[nNpcIdx].m_Series);
	else
		printf("[Bot] %s XIN VAO %s THAT BAI: m_nCurFaction=%d (he bot=%d, he phai can=%d)\n",
		       Player[nIdx].m_PlayerName, s_facNpc[b.nFaction].szTen,
		       (int)Player[nIdx].m_cFaction.m_nCurFaction,
		       Npc[nNpcIdx].m_Series, s_facNpc[b.nFaction].nSeries);
}

// ---------------------------------------------------------------- nhip
void PB_Breathe()
{
	// het han cho
	for (int i = 0; i < PB_MAX_PENDING; i++)
	{
		if (s_pending[i].nState == PB_FREE)
			continue;
		if (++s_pending[i].nWaited > PB_PENDING_TIMEOUT)
			pb_FreePending(&s_pending[i]);
	}

	// nhip tung bot (di bo + vao phai)
	for (int i = 0; i < s_botCount; i++)
		pb_DriveBot(s_bots[i]);

	// rut hang doi
	for (int n = 0; n < PB_DRAIN_PER_TICK; n++)
	{
		if (s_qHead == s_qTail)
			break;
		if (s_botCount >= PB_MAX_BOTS)
			break;
		if (!pb_AskRoleList(s_queue[s_qHead]))
			break;                             // het khe cho / chua co duong gui -> thu lai nhip sau
		s_qHead = (s_qHead + 1) % (PB_MAX_BOTS * 2);
	}
}

#endif // _SERVER
