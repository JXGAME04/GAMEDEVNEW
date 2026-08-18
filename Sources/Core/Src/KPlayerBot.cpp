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
#include "KItem.h"          // Item[] - doc DetailType / Particular / Series cua vu khi
#include "KItemSet.h"       // ItemSet.Add - tao vat pham
#include "KItemList.h"      // InsertEquipment / Equip / GetEquipment / RemoveItemIdx
#include "GameDataDef.h"    // itempart_weapon, LOCK_STATE_LOCK
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
	PB_AI_IDLE = 0,       // khong co viec gi, dung cho lenh
	PB_AI_SCATTER,        // vua vao game: TU DI BO tan ra cho khoi chong dong mot cho
	PB_AI_GOTO_FACTION,   // dang chay bo toi NPC mon phai (CHI vao khi GM goi PB_JoinFaction)
	PB_AI_IN_FACTION,     // da vao phai xong
	PB_AI_GIVEUP,         // khong toi duoc (khong tim thay NPC / khong co duong)
};

// Ban kinh tan ra mac dinh, tinh bang O luoi (1 o = 32 MPS). Dat qua lon thi bot di lau va de
// lot sang vung khong co duong; 40 o (~1280 MPS) du rong cho ca thi tran ma van toi duoc NPC.
#define PB_SCATTER_TILES    40
// So lan boc diem tan ra khac truoc khi chiu dung yen.
#define PB_SCATTER_MAX_TRY  6
// Toi dich tan ra thi coi la xong khi con cach chung nay (rong tay hon di gap NPC).
#define PB_SCATTER_ARRIVE   160

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
	int          nScatterX;                   // diem tan ra (MPS), 0 = chua boc
	int          nScatterY;
	int          nScatterTry;                 // so diem tan ra da boc hut
	int          nGaveWeapon;                 // 1 = da trao vu khi nhap mon (chi mot lan)
	int          nLastLevel;                  // cap do lan cuoi da cong diem tiem nang
};

static PB_Pending   s_pending[PB_MAX_PENDING];
static PB_Bot       s_bots[PB_MAX_BOTS];
static int          s_botCount   = 0;
static int          s_bKillAll   = 0;   // 1 = dang rut dan go het bot
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
	// quay tai cho cho xong.
	//
	// !! PHAI LAP THEO nStep, TUYET DOI KHONG LAY GIA TRI TRA VE LAM CO DUNG.
	//
	// Ban dau viet "while (...) { if (!LoadDBPlayerInfo(...)) break; }" va no THOAT NGAY
	// O VONG DAU, khien bot khong bao gio co ky nang / nhiem vu / MOT MON DO NAO.
	// Chuoi nhan qua (da doc tan dong):
	//   LoadDBPlayerInfo o STEP_BASE_INFO tra ve chinh ket qua SendSyncData
	//     (KPlayerDBFuns.cpp:46 nRetValue = SendSyncData(...) roi :202 return nRetValue)
	//   -> KPlayer::SendSyncData buoc 0 goi SubWorld[].SendSyncData(m_nIndex, m_nNetConnectIdx)
	//      (KPlayer.cpp:1109)
	//   -> KSubWorld.cpp:2268 g_pServer->PackDataToClient(nClient, ...)
	//   -> bot co m_nNetConnectIdx = -1 (KPlayerSet::Add da dat, KPlayerSet.cpp:213) nen tham so
	//      'const unsigned long&' hoa 0xFFFFFFFF, truot cong 'ulnClientID < m_nPlayerMaxCount'
	//      (ServerStage.cpp:396) -> tra E_FAIL -> SendSyncData FALSE -> ham tra 0.
	// Tuc voi bot thi gia tri tra ve LUON = 0 ngay ca khi nap THANH CONG.
	//
	// Nhung nStep VAN TIEN dung: nStep++ nam trong nhanh nRet == 1 (nap xong mot mau) va ca
	// nhanh nRet == -1 (bo qua buoc hong) - xem KPlayerDBFuns.cpp:47/54/70/77/93/100/154/161/179/186.
	// Con nRet == 0 nghia la con mau nua (vd tui do nap 10 mon moi luot, nParam tu tien).
	// => lap theo nStep la dung ban chat. Cay tham khao cung lam y het:
	//    USVOLAM PlayerAI\KBotManager.cpp:1497 "while (nStep < STEP_SYNC_END && nSafety > 0)".
	//
	// Rao 500 vong: tui do nap 10 mon/luot nen can nhieu luot hon so buoc; con 'default:'
	// (KPlayerDBFuns.cpp:199) tu dat nStep = STEP_SYNC_END nen vong luon co duong ra.
	{
		int          nStep  = STEP_BASE_INFO;
		unsigned int nParam = 0;
		int          nGuard = 500;
		while (nStep < STEP_SYNC_END && nGuard-- > 0)
			Player[nIdx].LoadDBPlayerInfo((BYTE*)Player[nIdx].m_SaveBuffer, nStep, nParam);
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
		// Vua vao game thi TU DI BO TAN RA, KHONG vao phai. Vao phai chi xay ra khi chu game
		// goi PB_JoinFaction() - de con kip nhin bot dan hang ra truoc.
		b.nAi        = PB_AI_SCATTER;
		b.nRetry     = 0;
		b.nNextTry   = 0;
		b.nScatterX  = 0;
		b.nScatterY  = 0;
		b.nScatterTry = 0;
		b.nGaveWeapon = 0;
		b.nLastLevel  = 0;
		b.walk.Reset();          // khe co the da dung cho bot truoc do -> phai xoa lo trinh cu
		s_botCount++;
	}

	pb_FreePending(p);
}

// ---------------------------------------------------------------- go bot
// Go THAT SU mot bot khoi the gioi. Tra 1 neu da go.
//
// Ban truoc chi dat s_botCount = 0 nen bot van song nhan nhan trong the gioi, chi la khong con
// ai dieu khien - dung nhu chu game bao "go bot ma bot khong bien mat". Go dung phai mo HAI
// O KHOA, va ca hai deu nam tren duong danh cho nguoi choi CO ket noi:
//
//   1. KPlayer::WaitForRemove (KPlayer.cpp:968-974) danh dau m_bIsQuiting = TRUE CHI KHI
//      m_nLixian == 0. Bot mang PB_LIXIAN_BOT (=3) nen no lai dat m_bIsQuiting = FALSE,
//      va IsWaitingRemove() tra FALSE => RemoveQuiting khong lam gi.
//      => phai ha m_nLixian ve 0 TRUOC khi goi PrepareRemove.
//
//   2. KPlayerSet::RemoveQuiting (KPlayerSet.cpp:449) thoat ngay neu
//      m_nNetConnectIdx == -1, TRU KHI m_bForeQuit = TRUE. Bot luon la -1.
//      => phai bat m_bForeQuit.
//
// Khuon nay chinh la thu KPlayerSet::RemoveAllPlayerLixianByAccount (KPlayerSet.cpp:1055-1059)
// dang dung de go nguoi uy thac: ha lixian roi goi thang RemoveQuiting.
//
// AN TOAN cho nguoi that: da khoa hai lop truoc do (chi so nam trong s_bots + m_dwID khop),
// nen khong the cham vao mot nguoi choi that trung khe.
//
// KHONG LUU DB, va do la CO Y: bot la nhan vat tam. PrepareRemove o cay nay cung khong luu
// (dong Player[].Save() bi chu thich chet tai KPlayerSet.cpp:393), no chi don sach chat/
// nhiem vu/to doi/giao dich/co bac/PK roi WaitForRemove - dung thu ta can de khong bo lai
// rac trong to doi hay phien giao dich cua nguoi that.
static int pb_KillBot(PB_Bot& b)
{
	const int nIdx = b.nPlayerIdx;
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		return 0;
	// IsMatch la ham cua KNpc, KPlayer khong co - so thang m_dwID de chac khe chua bi
	// cap lai cho nguoi khac.
	if (Player[nIdx].m_dwID == 0 || Player[nIdx].m_dwID != b.dwID)
		return 0;

	Player[nIdx].m_nLixian   = 0;      // mo khoa 1
	Player[nIdx].m_bForeQuit = TRUE;   // mo khoa 2

	PlayerSet.PrepareRemove(nIdx);     // don sach + WaitForRemove()
	PlayerSet.RemoveQuiting(nIdx);     // go NPC + region + tui do + tra khe

	return 1;
}

// So bot go moi nhip. GO DAN CHU KHONG GO MOT LUOT - hai ly do:
//   1. RemoveQuiting cham vao region, NpcSet, tui do va ban goi tin s2c_npcremove ra 9 region;
//      don ca chuc khe trong mot khung lam khung do dai bat thuong.
//   2. PrepareRemove chay playerlogout.lua. Go NGAY trong lenh Lua cua lenh bai admin tuc la
//      goi Lua LONG TRONG Lua. Hoan sang PB_Breathe la tranh han chuyen do.
// Cay tham khao dung dung co che nay (KBotManager::RemoveAllBots chi bat co m_bDrainAllBots
// roi de Tick rut dan, KBotManager.cpp:3653-3662).
#define PB_KILL_PER_TICK  4

int PB_RemoveAll()
{
	// CHI XEP HANG. Viec go that su do PB_Breathe lam, moi nhip vai con.
	//
	// TUYET DOI KHONG de bot song sot mot khung nao voi m_bIsQuiting = TRUE:
	// PlayerLogoutGateway (KSOServer.cpp:3518) thay IsCharacterQuiting thi goi SavePlayerData,
	// ma KPlayer::Save chan ngay dong dau voi m_nNetConnectIdx == -1 (KPlayer.cpp:1031) -> tra
	// FALSE -> "continue" -> RemoveQuitingPlayer KHONG BAO GIO chay -> khe KET VINH VIEN kem
	// spam console 18 dong/giay.
	// Vi vay pb_KillBot lam TRON chuoi trong MOT lan goi, con o day chi xep hang, KHONG dat
	// truoc bat ky co nao len bot.
	s_bKillAll = 1;
	printf("[Bot] xep hang go %d bot (rut dan %d con moi nhip)\n",
		   s_botCount, PB_KILL_PER_TICK);
	return s_botCount;
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

// Ra lenh cho moi bot dang ranh di vao phai. Tra so bot da nhan lenh.
//
// TACH RIENG khoi luc sinh bot LA CO Y (chu game yeu cau): sinh xong bot chi tan ra roi dung
// cho, co goi lenh nay thi moi lu luot keo di. Nhu vay con kip nhin bot dan hang ra, va chu
// game chu dong duoc thoi diem test.
//
// Bot da vao phai roi (PB_AI_IN_FACTION) thi BO QUA. Bot tung bo cuoc (PB_AI_GIVEUP) thi
// CHO DI LAI - goi lenh lan nua chinh la cach thu lai bang tay.
int PB_JoinFaction()
{
	int nRa = 0;
	for (int i = 0; i < s_botCount; i++)
	{
		PB_Bot& b = s_bots[i];
		if (b.nFaction < 0 || b.nAi == PB_AI_IN_FACTION)
			continue;
		b.nAi      = PB_AI_GOTO_FACTION;
		b.nRetry   = 0;
		b.nNextTry = 0;
		b.walk.Reset();
		nRa++;
	}
	printf("[Bot] ra lenh vao phai cho %d bot\n", nRa);
	return nRa;
}

int LuaPB_JoinFaction(Lua_State* L)
{
	Lua_PushNumber(L, PB_JoinFaction());
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
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
		return false;

	// !! PHAI loai kind_player, KHONG duoc chi so m_ActionScriptID.
	// Khi mot NGUOI CHOI THAT noi chuyen voi NPC mon phai, KPlayer::DialogNpc goi
	// ExecuteScript(Npc[nIdx].m_ActionScriptID, "main", nIdx) voi bGlobal MAC DINH = true
	// (KPlayer.h:546), ma ExecuteScript o nhanh bGlobal GHI CHINH ID SCRIPT DO len NPC CUA
	// NGUOI CHOI (KPlayer.cpp:6825-6826 Npc[m_nIndex].m_ActionScriptID = dwScriptId).
	// => sau khi mot nguoi that hoi NPC Thieu Lam, NPC cua chinh ho cung mang id script do,
	//    va bot se bam nham roi chay toi... nguoi choi do thay vi NPC.
	if (Npc[nNpcIdx].m_Kind == kind_player)
		return false;

	return Npc[nNpcIdx].m_ActionScriptID == dwScriptId
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

// ===========================================================================
// TRAO VU KHI NHAP MON THEO PHAI (cap 1)
//
// Khuon lay tu cay tham khao: GiveBotFactionWeapon (USVOLAM PlayerAI\KBotManager.cpp:2010).
// Vu khi dinh danh bang cap (nDetail, nParticular) theo bang [WeaponLimit] trong
// settings\gamesetting.ini:134-144:
//   can chien (detail 0): 0 Kiem  1 Don Dao  2 Con Bong  3 Thuong  4 Song Chuy  5 Song Dao
//   tam xa   (detail 1): 0 Phi Tieu  1 Phi Dao  2 Tu Tien
//
// HAI DIEU CHINH so voi ban tham khao, theo dung yeu cau chu game:
//   1. BO HAN particular 6 (Trien Thu / Chuong) - "du an toi khong co vu khi trien thu
//      nen cac phai noi cong khong can add vu khi". Ban tham khao chen {0,6} vao gan het
//      moi phai; bo no di thi bang con lai KHOP DUNG 10/10 danh sach chu game dua.
//   2. "Kich" KHONG phai mot ho rieng - trong JX1 no la ho THUONG (particular 3), cai ten
//      Kich chi xuat hien o cap cao ("Pha Thien Kich"). "Bao vu" la ho TU TIEN (tam xa,
//      particular 2). Nen Thien Nhan lay ho Thuong, Duong Mon lay ca ho Tu Tien.
//
// Ngu hanh vu khi = nFaction / 2 - dung cong thuc phai = series*2 + {0,1} da chot.
// ===========================================================================
struct PB_WpnOpt { int d, p; };

static const PB_WpnOpt s_wTL[] = { {0,1}, {0,2} };          // 0 Thieu Lam : Dao, Bong
static const PB_WpnOpt s_wTV[] = { {0,3}, {0,4}, {0,1} };   // 1 Thien Vuong: Thuong, Chuy, Dao
static const PB_WpnOpt s_wDM[] = { {1,2}, {1,0}, {1,1} };   // 2 Duong Mon  : Bao vu(Tu Tien), Phi Tieu, Phi Dao
static const PB_WpnOpt s_wDao[] = { {0,1} };                // 3 Ngu Doc, 5 Thuy Yen, 9 Con Lon: Dao
static const PB_WpnOpt s_wKiem[] = { {0,0} };               // 4 Nga My, 8 Vo Dang: Kiem
static const PB_WpnOpt s_wBong[] = { {0,2} };               // 6 Cai Bang  : Bong
static const PB_WpnOpt s_wThuong[] = { {0,3} };             // 7 Thien Nhan: Kich = ho Thuong

static void pb_GiveFactionWeapon(int nIdx, int nFaction)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER || nFaction < 0 || nFaction >= MAX_FACTION)
		return;

	const PB_WpnOpt* pPool = s_wDao;
	int nPool = 1;
	switch (nFaction)
	{
	case 0: pPool = s_wTL;     nPool = 2; break;
	case 1: pPool = s_wTV;     nPool = 3; break;
	case 2: pPool = s_wDM;     nPool = 3; break;
	case 4: case 8: pPool = s_wKiem;   nPool = 1; break;
	case 6: pPool = s_wBong;   nPool = 1; break;
	case 7: pPool = s_wThuong; nPool = 1; break;
	default: break;            // 3 Ngu Doc / 5 Thuy Yen / 9 Con Lon = Dao
	}

	const PB_WpnOpt& w = pPool[(int)g_Random(nPool)];
	const int nSeries = nFaction / 2;

	// Thao vu khi cu neu co. Bot nhan ban tu mot nhan vat mau nen CO THE dang cam san vu
	// khi cua nhan vat do - khong thao thi no giu nguyen va ky nang mon phai danh khong trung.
	const int nOld = Player[nIdx].m_ItemList.GetEquipment(itempart_weapon);
	if (nOld > 0)
	{
		// RemoveItemIdx voi so luong = ca chong. TRUYEN 0 LA KHONG XOA GI (KItemList.cpp:
		// nNum=0 -> SetStackNum(nStack - 0) tuc giu nguyen). KItemList::Remove tu goi UnEquip
		// cho o pos_equip nen khong ro ri.
		Player[nIdx].m_ItemList.RemoveItemIdx(nOld, Item[nOld].GetStackNum());
	}

	int nMagic[MAX_ITEM_MAGICLEVEL];
	ZeroMemory(nMagic, sizeof(nMagic));
	// (nItemNature=0, nItemGenre=0 item_equip, nSeries, nLevel=1, nLuck=0, nDetail, nParticular)
	const int nNew = ItemSet.Add(0, 0, nSeries, 1, 0, w.d, w.p, nMagic,
						  g_SubWorldSet.GetGameVersion(), 0);
	if (nNew <= 0)
	{
		printf("[BotVuKhi] %s: ItemSet.Add THAT BAI (detail=%d parti=%d series=%d)\n",
			   Player[nIdx].m_PlayerName, w.d, w.p, nSeries);
		return;
	}

	Item[nNew].LockItem(LOCK_STATE_LOCK);              // khoa lai de bot khong lam roi
	Player[nIdx].m_ItemList.InsertEquipment(nNew, false);
	// Equip o cay nay chi co HAI tham so (KItemList.h:104), ban tham khao co tham so thu ba
	// bUpdateSkin - dung chep nguyen chu ky sang.
	Player[nIdx].m_ItemList.Equip(nNew, -1);

	printf("[BotVuKhi] %s phai %s: detail=%d parti=%d he=%d, dang cam=%d\n",
		   Player[nIdx].m_PlayerName, s_facNpc[nFaction].szTen, w.d, w.p, nSeries,
		   (int)(Player[nIdx].m_ItemList.GetEquipment(itempart_weapon) > 0));
}

// ===========================================================================
// TU CONG DIEM TIEM NANG (goi moi lan len cap)
//
// Co che lay tu cay tham khao AllocateBotAttributePoints (KBotManager.cpp:2083), nhung
// TY LE thi theo dung chu game chot (ban tham khao chia 50/50, khac han):
//   Ngoai cong (mac dinh) : 50% Suc manh, 25% Sinh khi, 25% Than phap
//   Duong Mon             : 60% Than phap, 40% Sinh khi
//   Vo Dang               : 70% Noi cong, 30% Sinh khi
//   Noi cong (neu co)     : 20% Suc manh, 10% Than phap, 70% Sinh khi
//
// GIA DINH DA NEU RO: chu game chi diem danh RIENG hai phai Duong Mon va Vo Dang, nen 8
// phai con lai di duong "ngoai cong". Nhanh "noi cong" van giu nguyen trong ma, kich hoat
// bang vu khi ho Trien Thu (particular 6) - du an hien khong co ho do nen nhanh nay nam cho.
//
// Ten bien THAT trong engine (da doi chieu, khong doan):
//   Suc manh = m_nStrength   Than phap = m_nDexterity
//   Sinh khi = m_nVitality   Noi cong = m_nEngergy   (engine viet sai chinh ta "Engergy")
// Diem chua tieu = m_nAttributePoint (KPlayer.h:346), moi cap +5.
// Bon ham AddBase* la CONG DUY NHAT co kiem du diem (nData > 0 && nData <= m_nAttributePoint)
// va chung TRU DAN vao quy, nen phai chia sao cho TONG DUNG BANG quy va goi lan luot.
// ===========================================================================
// Cong diem vao MOT chi so, di dung duong goi tin cua nguoi choi that.
static void pb_AddAttrib(int nIdx, int nWhich, int nPoint)
{
	if (nPoint <= 0)
		return;
	PLAYER_ADD_BASE_ATTRIBUTE_COMMAND cmd;
	cmd.ProtocolType  = c2s_playeraddbaseattribute;
	cmd.m_btAttribute = (BYTE)nWhich;
	cmd.m_nAddNo      = nPoint;
	Player[nIdx].AddBaseAttribute((BYTE*)&cmd);
}

static void pb_AllocAttribPoints(int nIdx, int nFaction)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		return;
	const int nQuy = Player[nIdx].m_nAttributePoint;
	if (nQuy <= 0)
		return;

	// Bot co dang cam vu khi ho Trien Thu khong (nhanh noi cong).
	const int nWpn   = Player[nIdx].m_ItemList.GetEquipment(itempart_weapon);
	const int nParti = (nWpn > 0) ? Item[nWpn].GetParticular() : -1;

	int nSM = 0, nTP = 0, nSK = 0, nNC = 0;   // suc manh / than phap / sinh khi / noi cong

	if (nFaction == 2)                       // Duong Mon
	{
		nTP = nQuy * 60 / 100;
		nSK = nQuy - nTP;
	}
	else if (nFaction == 8)                  // Vo Dang
	{
		nNC = nQuy * 70 / 100;
		nSK = nQuy - nNC;
	}
	else if (nParti == 6)                    // nhanh noi cong (Trien Thu) - hien chua dung
	{
		nSM = nQuy * 20 / 100;
		nTP = nQuy * 10 / 100;
		nSK = nQuy - nSM - nTP;              // phan du don het vao sinh khi
	}
	else                                     // ngoai cong
	{
		nSM = nQuy * 50 / 100;
		nTP = nQuy * 25 / 100;
		nSK = nQuy - nSM - nTP;
	}

	// Bon ham AddBaseStrength/Dexterity/Vitality/Engergy deu PRIVATE o cay nay
	// (KPlayer.h:622-625). Cong CONG KHAI la AddBaseAttribute(BYTE*) o KPlayer.h:788 - chinh
	// la thu KProtocolProcess::PlayerAddBaseAttribute goi khi nhan goi tin cua nguoi choi
	// that (KProtocolProcess.cpp:5200-5202). Di duong nay tuc bot cong diem y HET nguoi that.
	// AddBase* tu tru vao quy nen tong dung bang nQuy la khong bao gio bi tu choi.
	pb_AddAttrib(nIdx, ATTRIBUTE_STRENGTH, nSM);
	pb_AddAttrib(nIdx, ATTRIBUTE_DEXTERITY, nTP);
	pb_AddAttrib(nIdx, ATTRIBUTE_VITALITY, nSK);
	pb_AddAttrib(nIdx, ATTRIBUTE_ENGERGY, nNC);

	printf("[BotDiem] %s (%s) chia %d diem: SM=%d TP=%d SK=%d NC=%d, con lai=%d\n",
		   Player[nIdx].m_PlayerName, s_facNpc[nFaction].szTen, nQuy, nSM, nTP, nSK, nNC,
		   Player[nIdx].m_nAttributePoint);
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

	const unsigned int nowAll = SubWorld[nSub].m_dwCurrentTime;

	// ---------------------------------------------------------------- TAN RA
	// Ca 1000 bot deu nhan ban tu MOT nhan vat mau nen mang y het mot toa do vao game
	// (taobot_bdb.cpp KHONG ghi de ientergameid/x/y), vi vay goi ra la chong dong mot cho.
	// Cach xu ly: cho bot TU DI BO tan ra - khong dich chuyen tuc thoi - de nhin nhu nguoi
	// that dan hang ra khoi cong lang.
	// Diem den boc ngau nhien quanh cho sinh; KHONG can tu kiem vat can vi neu diem roi vao
	// tuong thi FindPathServer tra -1 va ta boc diem khac (tu sua sai, khong can API moi).
	if (b.nAi == PB_AI_SCATTER)
	{
		if (b.nNextTry && nowAll < b.nNextTry)
			return;

		if (b.nScatterX == 0 && b.nScatterY == 0)
		{
			int bx = 0, by = 0;
			Npc[nNpcIdx].GetMpsPos(&bx, &by);
			const int nR = PB_SCATTER_TILES;
			// boc lech theo ca hai truc, tru di nua ban kinh de ra so am duong
			b.nScatterX = bx + ((int)g_Random(nR * 2 + 1) - nR) * 32;
			b.nScatterY = by + ((int)g_Random(nR * 2 + 1) - nR) * 32;
			if (b.nScatterX <= 0) b.nScatterX = bx;
			if (b.nScatterY <= 0) b.nScatterY = by;
			b.walk.Reset();
		}

		const int nRetS = PB_WalkTo(nNpcIdx, b.nScatterX, b.nScatterY, nSub,
		                            b.walk, PB_SCATTER_ARRIVE);
		if (nRetS == 0)
			return;                       // dang di

		if (nRetS < 0 && ++b.nScatterTry < PB_SCATTER_MAX_TRY)
		{
			// diem do khong toi duoc -> boc diem khac o nhip sau
			b.nScatterX = 0;
			b.nScatterY = 0;
			b.walk.Reset();
			b.nNextTry = nowAll + GAME_FPS;
			return;
		}

		// Toi noi, hoac da boc hut qua nhieu lan -> thoi, dung do cho lenh vao phai.
		Npc[nNpcIdx].SendCommand(do_stand);
		b.walk.Reset();
		b.nNextTry = 0;
		b.nAi      = PB_AI_IDLE;
		return;
	}

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
		printf("[Bot] %s BO CUOC: NPC %s o ban do khac (bot map=%d, npc map=%d)\n",
		       Player[nIdx].m_PlayerName, s_facNpc[b.nFaction].szTen,
		       SubWorld[nSub].m_SubWorldID,
		       SubWorld[Npc[nFacNpc].m_SubWorldIndex].m_SubWorldID);
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
		{
			b.nAi = PB_AI_GIVEUP;
			// PHAI in. Truoc day PB_AI_GIVEUP la ngo cut IM LANG: dinh mot lan la bot dung
			// yen mai mai, nhin tu ngoai khong phan biet duoc voi "bot hong". Goi
			// PB_JoinFaction() lan nua se cho no di lai.
			int bx = 0, by = 0;
			Npc[nNpcIdx].GetMpsPos(&bx, &by);
			printf("[Bot] %s BO CUOC sau %d lan khong tim duoc duong toi %s"
			       " (bot o o %d,%d -> NPC o o %d,%d, map %d)."
			       " Kiem dong [BotA*] luoi luc boot xem luoi co nap khong.\n",
			       Player[nIdx].m_PlayerName, PB_FAC_MAX_RETRY, s_facNpc[b.nFaction].szTen,
			       bx / 32, by / 32, nx / 32, ny / 32, SubWorld[nSub].m_SubWorldID);
		}
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
	{
		printf("[Bot] %s da vao %s (phai %d, he %d)\n",
		       Player[nIdx].m_PlayerName, s_facNpc[b.nFaction].szTen,
		       b.nFaction, Npc[nNpcIdx].m_Series);

		// Trao vu khi nhap mon MOT LAN. Ban tham khao lam y het o day: vao phai xong moi trao,
		// va dat co de KHONG boc lai moi lan len cap (KBotManager.cpp:2950-2956).
		// Khong co vu khi hop ho thi ky nang mon phai vua hoc se bi CanCastSkill tu choi
		// IM LANG (KSkills.cpp:230-267) va bot danh khong ra chieu.
		if (!b.nGaveWeapon)
		{
			b.nGaveWeapon = 1;
			pb_GiveFactionWeapon(nIdx, b.nFaction);
		}

		// Tieu luon so diem dang ton (nhan vat mau co the da tich san), tu do moi lan len cap
		// se tieu tiep o nhanh PB_AI_IN_FACTION ben tren.
		b.nLastLevel = Npc[nNpcIdx].m_Level;
		pb_AllocAttribPoints(nIdx, b.nFaction);
	}
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

	// rut dan hang go. Lam TRUOC vong nhip de khong dieu khien khe vua thao.
	if (s_bKillAll)
	{
		for (int n = 0; n < PB_KILL_PER_TICK && s_botCount > 0; n++)
		{
			// Go tu CUOI danh sach: khong phai don mang, va chi so cac khe con lai khong doi.
			pb_KillBot(s_bots[s_botCount - 1]);
			s_botCount--;
		}
		if (s_botCount == 0)
		{
			s_bKillAll = 0;
			printf("[Bot] da go xong toan bo bot\n");
		}
		return;                            // dang don dep thi khoan sinh them / dieu khien
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
