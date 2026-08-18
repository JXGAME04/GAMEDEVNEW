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
#include "GameDataDef.h"    // itempart_weapon, LOCK_STATE_LOCK, HAND_PARTICULAR
#include "KSkills.h"        // KSkill - doc EqtLimit / SkillStyle / IsTargetEnemy
#include "KSkillManager.h"  // g_SkillManager.GetSkill(id, level)
#include "SkillDef.h"       // SKILL_SS_* - phan biet vong sang / buff / bua
#include "KRegion.h"        // duyet m_NpcList de tim quai theo region
#include "KMath.h"         // g_GetDistance
#include "KPlayerChat.h"   // KPlayerChat::NpcChat - bot noi chuyen
#include "KTabFile.h"      // doc kho cau thoai chat.txt
#include "KPlayerBot.h"
// TRoleData / S3DBI_RoleBaseInfo. Dung DUNG duong dan ma CoreServerShell.cpp:33 dang dung
// de chac chan lay cung mot ban voi phan con lai cua Core (cay nay co nhieu ban
// S3DBInterface.h: Headers\, Lib\, Sources\Core\lib\ - chung KHAC NHAU).
#include "../../lib/S3DBInterface.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

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
	PB_AI_FIGHT,          // dang danh quai tai cho
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

// ---------------------------------------------------------------------------
// PHAI NAO NHAN GIOI TINH NAO
//
// Engine KHONG chan: khong mot dong nao trong 10 script mon phai, trong factionhead.lua,
// hay trong KPlayerFaction kiem gioi tinh. Day la luat THIET KE, cua vao la luc TAO NHAN
// VAT. Nen neu ta khong tu giu thi bot nam se chui tot vao Nga My / Thuy Yen - dung nhu
// chu game bat duoc.
//
//   Thieu Lam (0), Thien Vuong (1): chi NAM  <- ca hai phai cua he KIM
//   Nga My (4),    Thuy Yen (5)   : chi NU   <- ca hai phai cua he THUY
//   con lai                       : ca hai
//
// m_nSex: 0 = nam, khac 0 = nu (KPlayerDBFuns.cpp:238-246, bSex -> PLAYER_FEMALE/MALE_NPCTEMPLATEID).
// ---------------------------------------------------------------------------
static bool pb_SexOk(int nFaction, int nSex)
{
	const bool bNu = (nSex != 0);
	if (nFaction == 0 || nFaction == 1)   return !bNu;   // Thieu Lam / Thien Vuong: nam
	if (nFaction == 4 || nFaction == 5)   return bNu;    // Nga My / Thuy Yen: nu
	return true;
}

// Bo dem LUAN PHIEN trong tung he. Truoc day boc bang g_Random(2) nen voi so bot it thi
// phan bo lech han (chu game bao "chua random deu cac phai"). Luan phien thi 20 bot ra
// dung 2 con moi phai, khong phu thuoc may man.
static int s_facTurn[series_num] = { 0 };

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
	unsigned int nChetTuTick;                 // moc tick phat hien bot nam cho hoi sinh (0 = dang song)
	// ---- chien dau ----
	int          nAtkSkill;                   // id chieu danh da ghim (0 = chua chon)
	int          nAtkSkillLv;                 // cap cua chieu do
	int          nAtkPickLevel;               // cap bot luc chon chieu (len cap thi chon lai)
	int          nTargetNpc;                  // khe NPC muc tieu (0 = chua co)
	unsigned int nLagTick;                    // moc do "danh mai khong sut mau"
	int          nLagLife;                    // mau muc tieu tai moc do
	unsigned int nCastTick;                   // moc phat lenh danh lan cuoi
	// ---- luyen cap ----
	int          nBaiIdx;                     // chi so bai luyen dang nham (-1 = chua chon)
	int          nBaiLevel;                   // cap bot luc chon bai (len cap thi chon lai)
	unsigned int nDoiMapTick;                 // giai cho giua hai lan thu doi map
	int          nChatCuoi;                   // con tro cau thoai (chong lap giua cac bot)
	// ---- di hoang tim quai ----
	PB_WalkState roam;                        // trang thai di bo RIENG cho viec di hoang
	int          nRoamX, nRoamY;              // diem dang nham toi (MPS), 0 = chua co
	unsigned int nRoamTick;                   // moc boc diem lan cuoi
	// ---- vong sang / buff / bua ----
	int          nAuraSkill;                  // vong sang dang bat (0 = chua co)
	int          nAuraLevel;                  // cap bot luc chon vong sang
	unsigned int nBuffTick;                   // moc buff lan cuoi
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

// ---------------------------------------------------------------------------
// GHI NHAT KY RA TEP.
//
// Console cua GameServer cuon rat nhanh va lan voi log cua ca may chu, nen moi dong
// cua he bot deu duoc ghi THEM vao mot tep rieng de doc lai cho de.
//
// Tep nam o THU MUC LAM VIEC cua GameServer (chinh la bin\server), ten "bot.log".
// Mo/dong moi dong: cham hon mot chut nhung bot ghi rat thua (vai dong moi phut) va
// doi lai la KHONG BAO GIO mat dong cuoi khi may chu tat dot ngot - dieu quan trong
// hon nhieu khi dang di tim mot loi chi xay ra mot lan.
// ---------------------------------------------------------------------------
static void pb_Log(const char* szFmt, ...)
{
	char szBuf[600];
	va_list ap;
	va_start(ap, szFmt);
	_vsnprintf(szBuf, sizeof(szBuf) - 1, szFmt, ap);
	va_end(ap);
	szBuf[sizeof(szBuf) - 1] = 0;

	printf("%s", szBuf);          // van giu console cho ai dang nhin

	FILE* f = fopen("bot.log", "a");
	if (!f)
		return;
	time_t tNow = time(NULL);
	struct tm* pT = localtime(&tNow);
	if (pT)
		fprintf(f, "[%02d/%02d %02d:%02d:%02d] %s",
			  pT->tm_mday, pT->tm_mon + 1, pT->tm_hour, pT->tm_min, pT->tm_sec, szBuf);
	else
		fprintf(f, "%s", szBuf);
	fclose(f);
}

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
		const int nNpcIdx = Player[nIdx].m_nIndex;
		if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
		{
			const int nSeries = Npc[nNpcIdx].m_Series;
			const int nSex    = Npc[nNpcIdx].m_nSex;
			if (nSeries >= series_metal && nSeries < series_num)
			{
				// Luan phien trong he (phan bo deu), roi bo qua phai khong hop gioi tinh.
				const int nBatDau = s_facTurn[nSeries]++;
				for (int k = 0; k < FACTIONS_PRR_SERIES; k++)
				{
					const int nF = nSeries * FACTIONS_PRR_SERIES
					             + ((nBatDau + k) % FACTIONS_PRR_SERIES);
					if (pb_SexOk(nF, nSex))
					{
						nFaction = nF;
						break;
					}
				}
				if (nFaction < 0)
				{
					// Chi xay ra voi bot NAM he THUY: ca Nga My lan Thuy Yen deu chi nhan nu.
					// Nhan vat cu tao truoc khi taobot_bdb biet ep he Thuy = nu se roi vao day.
					pb_Log("[Bot] %s he %d gioi tinh %s: KHONG PHAI NAO NHAN"
					       " - tao lai bot bang taobot_bdb ban moi (he Thuy phai la nu)\n",
					       Player[nIdx].m_PlayerName, nSeries, nSex ? "nu" : "nam");
				}
			}
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
		b.nChetTuTick = 0;
		b.nAtkSkill = 0;  b.nAtkSkillLv = 0;  b.nAtkPickLevel = 0;
		b.nTargetNpc = 0; b.nLagTick = 0;     b.nLagLife = 0;  b.nCastTick = 0;
		b.nBaiIdx = -1;   b.nBaiLevel = 0;    b.nDoiMapTick = 0;
		b.nChatCuoi = 0;
		b.roam.Reset();   b.nRoamX = 0;  b.nRoamY = 0;  b.nRoamTick = 0;
		b.nAuraSkill = 0; b.nAuraLevel = 0; b.nBuffTick = 0;
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
	pb_Log("[Bot] xep hang go %d bot (rut dan %d con moi nhip)\n",
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
	pb_Log("[Bot] ra lenh vao phai cho %d bot\n", nRa);
	return nRa;
}

// Bat/tat che do danh quai cho moi bot da vao phai.
// Tach rieng khoi vao phai (giong PB_JoinFaction) de chu game chu dong thoi diem test.
int PB_SetFight(int bOn)
{
	int n = 0;
	for (int i = 0; i < s_botCount; i++)
	{
		PB_Bot& b = s_bots[i];
		if (bOn)
		{
			if (b.nAi != PB_AI_IN_FACTION && b.nAi != PB_AI_IDLE)
				continue;
			b.nAi        = PB_AI_FIGHT;
			b.nTargetNpc = 0;
			b.nAtkSkill  = 0;    // ep chon lai chieu theo vu khi hien tai
			n++;
		}
		else if (b.nAi == PB_AI_FIGHT)
		{
			b.nAi        = PB_AI_IDLE;
			b.nTargetNpc = 0;
			int nNpcIdx  = Player[b.nPlayerIdx].m_nIndex;
			if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
				Npc[nNpcIdx].SendCommand(do_stand);
			n++;
		}
	}
	pb_Log("[Bot] %s danh quai cho %d bot\n", bOn ? "BAT" : "TAT", n);
	return n;
}

// Kho cau thoai + muc noi. Dinh nghia SOM vi ham Lua ben duoi dung toi;
// than pb_LoadChat/pb_Chat nam gan pb_DriveBot cho de doc.
#define PB_CHAT_MAX     2048
#define PB_CHAT_LEN     160
static char s_pbChat[PB_CHAT_MAX][PB_CHAT_LEN];
static int  s_pbChatCount = 0;
static int  s_pbChatRate  = 0;    // n/1000 moi giay moi bot; 0 = tat
static int  pb_LoadChat(const char* szFile, const char* szType);

// PB_SetChat(nRate [, szFile, szType]) -> so cau da nap.
//   nRate = n/1000 moi giay moi bot. 0 = tat.
int LuaPB_SetChat(Lua_State* L)
{
	const int nRate = (Lua_GetTopIndex(L) >= 1) ? (int)Lua_ValueToNumber(L, 1) : 0;
	const char* szFile = (Lua_GetTopIndex(L) >= 2)
		  ? (const char*)Lua_ValueToString(L, 2) : "/settings/simcity/chat.txt";
	const char* szType = (Lua_GetTopIndex(L) >= 3)
		  ? (const char*)Lua_ValueToString(L, 3) : "general";

	if (nRate > 0 && s_pbChatCount <= 0)
	{
		const int n = pb_LoadChat(szFile, szType);
		pb_Log("[BotChat] nap %d cau tu %s (nhom %s)\n", n, szFile, szType);
	}
	s_pbChatRate = (nRate < 0) ? 0 : ((nRate > 1000) ? 1000 : nRate);
	pb_Log("[BotChat] muc noi chuyen = %d/1000 moi giay moi bot (kho %d cau)\n",
		   s_pbChatRate, s_pbChatCount);
	Lua_PushNumber(L, s_pbChatCount);
	return 1;
}

int LuaPB_SetFight(Lua_State* L)
{
	int bOn = (Lua_GetTopIndex(L) >= 1) ? (int)Lua_ValueToNumber(L, 1) : 1;
	Lua_PushNumber(L, PB_SetFight(bOn));
	return 1;
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
// Moi phai co nhieu DUONG DI ky nang. Cac duong dung vu khi thi ghi (detail, particular);
// rieng duong QUYEN / NOI CONG thi bot KHONG NHAN VU KHI NAO - danh tay khong.
//
// Ban tham khao dat duong do bang ho Trien Thu (particular 6). Du an nay KHONG CO ho Trien
// Thu, nen o day no thanh PB_WPN_NONE = khong trao gi ca. Truoc do toi XOA HAN lua chon
// nay di - SAI, vi nhu vay mat luon duong noi cong: vd Thieu Lam phai co DU BA duong
// dao / bong / quyen, Thuy Yen phai co dao HOAC tay khong.
struct PB_WpnOpt { int d, p; };
#define PB_WPN_NONE   { -1, -1 }        // duong quyen / noi cong: khong nhan vu khi

// 0 Thieu Lam : Dao, Bong, Quyen        (3 duong)
static const PB_WpnOpt s_wTL[]   = { {0,1}, {0,2}, PB_WPN_NONE };
// 1 Thien Vuong: Thuong, Chuy, Dao      (deu ngoai cong)
static const PB_WpnOpt s_wTV[]   = { {0,3}, {0,4}, {0,1} };
// 2 Duong Mon  : Bao vu(Tu Tien), Phi Tieu, Phi Dao  (deu tam xa)
static const PB_WpnOpt s_wDM[]   = { {1,2}, {1,0}, {1,1} };
// 3 Ngu Doc, 5 Thuy Yen, 9 Con Lon: Dao hoac tay khong
static const PB_WpnOpt s_wDao[]  = { {0,1}, PB_WPN_NONE };
// 4 Nga My, 8 Vo Dang: Kiem hoac tay khong
static const PB_WpnOpt s_wKiem[] = { {0,0}, PB_WPN_NONE };
// 6 Cai Bang  : Bong hoac tay khong
static const PB_WpnOpt s_wBong[] = { {0,2}, PB_WPN_NONE };
// 7 Thien Nhan: Kich (= ho Thuong) hoac tay khong
static const PB_WpnOpt s_wKich[] = { {0,3}, PB_WPN_NONE };

static void pb_GiveFactionWeapon(int nIdx, int nFaction)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER || nFaction < 0 || nFaction >= MAX_FACTION)
		return;

	const PB_WpnOpt* pPool = s_wDao;
	int nPool = 2;
	switch (nFaction)
	{
	case 0: pPool = s_wTL;   nPool = 3; break;
	case 1: pPool = s_wTV;   nPool = 3; break;
	case 2: pPool = s_wDM;   nPool = 3; break;
	case 4: case 8: pPool = s_wKiem; nPool = 2; break;
	case 6: pPool = s_wBong; nPool = 2; break;
	case 7: pPool = s_wKich; nPool = 2; break;
	default: break;          // 3 Ngu Doc / 5 Thuy Yen / 9 Con Lon = Dao hoac tay khong
	}

	const PB_WpnOpt& w = pPool[(int)g_Random(nPool)];

	// Thao vu khi cu TRUOC, ke ca khi boc trung duong quyen. Bot nhan ban tu mot nhan vat
	// mau nen CO THE dang cam san vu khi cua nhan vat do; khong thao thi duong quyen van
	// cam vu khi, va ky nang mon phai se danh khong dung ho.
	const int nOld = Player[nIdx].m_ItemList.GetEquipment(itempart_weapon);
	if (nOld > 0)
	{
		// RemoveItemIdx voi so luong = CA CHONG. TRUYEN 0 LA KHONG XOA GI
		// (KItemList.cpp: nNum = 0 -> SetStackNum(nStack - 0) tuc giu nguyen).
		// KItemList::Remove tu goi UnEquip cho o pos_equip nen khong ro ri.
		Player[nIdx].m_ItemList.RemoveItemIdx(nOld, Item[nOld].GetStackNum());
	}

	if (w.d < 0)
	{
		// DUONG QUYEN / NOI CONG - co y de tay khong.
		pb_Log("[BotVuKhi] %s phai %s: duong QUYEN, khong nhan vu khi\n",
			   Player[nIdx].m_PlayerName, s_facNpc[nFaction].szTen);
		return;
	}

	const int nSeries = nFaction / 2;   // phai = series*2 + {0,1}
	int nMagic[MAX_ITEM_MAGICLEVEL];
	ZeroMemory(nMagic, sizeof(nMagic));
	// (nItemNature=0, nItemGenre=0 item_equip, nSeries, nLevel=1, nLuck=0, nDetail, nParticular)
	const int nNew = ItemSet.Add(0, 0, nSeries, 1, 0, w.d, w.p, nMagic,
						  g_SubWorldSet.GetGameVersion(), 0);
	if (nNew <= 0)
	{
		pb_Log("[BotVuKhi] %s: ItemSet.Add THAT BAI (detail=%d parti=%d he=%d)\n",
			   Player[nIdx].m_PlayerName, w.d, w.p, nSeries);
		return;
	}

	Item[nNew].LockItem(LOCK_STATE_LOCK);              // khoa lai de bot khong lam roi
	Player[nIdx].m_ItemList.InsertEquipment(nNew, false);
	// Equip o cay nay chi co HAI tham so (KItemList.h:104); ban tham khao co tham so thu ba
	// bUpdateSkin - dung chep nguyen chu ky sang.
	Player[nIdx].m_ItemList.Equip(nNew, -1);

	pb_Log("[BotVuKhi] %s phai %s: detail=%d parti=%d he=%d, dang cam=%d\n",
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

	// Bot co dang cam vu khi khong - do la thu quyet dinh ngoai cong hay noi cong.
	const int nWpn = Player[nIdx].m_ItemList.GetEquipment(itempart_weapon);

	int nSM = 0, nTP = 0, nSK = 0, nNC = 0;   // suc manh / than phap / sinh khi / noi cong

	// KHONG CAM VU KHI = di duong QUYEN / NOI CONG. Day la dau hieu dang tin nhat va tu
	// sua sai: pb_GiveFactionWeapon da thao vu khi cu truoc khi boc, nen bot tay khong la
	// tay khong that.
	const bool bNoiCong = (nWpn <= 0);

	if (bNoiCong && nFaction == 8)           // Vo Dang NOI
	{
		nNC = nQuy * 70 / 100;
		nSK = nQuy - nNC;
	}
	else if (bNoiCong)                       // noi cong cac phai con lai
	{
		nSM = nQuy * 20 / 100;
		nTP = nQuy * 10 / 100;
		nSK = nQuy - nSM - nTP;              // phan du don het vao sinh khi
	}
	else if (nFaction == 2)                  // Duong Mon (luon cam vu khi tam xa)
	{
		nTP = nQuy * 60 / 100;
		nSK = nQuy - nTP;
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

	pb_Log("[BotDiem] %s (%s) chia %d diem: SM=%d TP=%d SK=%d NC=%d, con lai=%d\n",
		   Player[nIdx].m_PlayerName, s_facNpc[nFaction].szTen, nQuy, nSM, nTP, nSK, nNC,
		   Player[nIdx].m_nAttributePoint);
}

// ===========================================================================
// CHIEN DAU
//
// Khuon lay tu chinh he auto cua du an, KHONG phai tu cay tham khao ngoai:
//   chon MUC TIEU  : KPlayer::FindTargetNpc (KPlayer.cpp:9133-9291)
//   chon KY NANG   : nhanh ATYPE_FIGHT (CoreShell.cpp:4589-4668)
//   dieu phoi      : KMyApp::ExtAutoLoop (S3Client.cpp:781-1048)
// Ca ba deu nam trong #ifndef _SERVER nen phia server khong co - phai viet lai.
//
// HAI DIEU HOC DUOC tu ban goc, giu nguyen o day:
//   1. Muc tieu luon la GAN NHAT. Ban goc KHONG HE doc mau cua muc tieu trong ham chon
//      (da doc tan dong KPlayer.cpp:9133-9291) - dung tu "cai tien" thanh uu tien mau thap.
//   2. Chong ket: neu 10 giay ma mau muc tieu khong tut noi 10% thi bo no ra
//      (khuon KPlayerAuto::AutoCheckNpcLag). Khong co cai nay thi mot con quai bat tu / ke
//      sau tuong la du de treo bot vinh vien.
//
// TAM NHIN kep [100,1200] y ban goc (KPlayer.cpp:9139-9146).
// ===========================================================================
// ===========================================================================
// BAI LUYEN CAP THEO CAP DO
//
// Nguon: chinh bang THAN HANH PHU cua may chu dang chay -
//   script\item\ib\shenxingfu.lua (1398 dong): tab_lv20map .. tab_lv90map,
//   rieng Hoa Son o ham go_HSBattle:708 "NewWorld(2,2605,3592)".
// Toa do trong lua la O LUOI, nen phai nhan 32 khi doi sang MPS.
//
// Cap 10 -> Hoa Son dung yeu cau chu game. Tu 20 tro len lay dung cac bai cua Than
// Hanh Phu, moi moc nhieu bai thi boc ngau nhien mot bai (rai bot ra, khong don cuc).
//
// CO Y giu DU bang ke ca ban do hien chua mo: chu game noi ban local dang mo it map
// nhung sap mo full. Bot khong toi duoc thi ChangeWorld tra khac 1 va ta bao ro,
// chu KHONG cat bot bang danh sach map hien co.
// ===========================================================================
struct PB_BaiLuyen
{
	int         nCapToiThieu;
	int         nMapId;
	int         nOX, nOY;        // O LUOI (nhan 32 ra MPS)
	const char* szTen;
};

static const PB_BaiLuyen s_bai[] =
{
	{ 10,   2, 2605, 3592, "Hoa Son" },
	{ 20,  19, 3102, 3963, "Kiem Cac Tay Nam" },
	{ 20,   7, 2276, 2825, "Tan Lang tang 1" },
	{ 30, 193, 1938, 2845, "Vu Di Son" },
	{ 30, 170, 1612, 3187, "Tho Phi Dong" },
	{ 40,  21, 2622, 4502, "Thanh Thanh Son" },
	{ 40, 167, 1575, 3239, "Diem Thuong Son" },
	{ 50, 182, 1777, 2982, "Nghiet Long Dong" },
	{ 50, 164, 1611, 3187, "Thien Tam Thap" },
	{ 60,  79, 1600, 3206, "Tuong Duong Mat Dao" },
	{ 60,  56, 1516, 3443, "Hoanh Son Phai" },
	{ 60, 166, 1649, 3231, "Thien Tam Thap tang 3" },
	{ 70, 319, 1630, 3587, "Lam Du Quan" },
	{ 70, 123, 1702, 3350, "Lao Ho Dong" },
	{ 70, 206, 1603, 3215, "Tan Lang tang 2" },
	{ 80, 224, 1622, 3118, "Sa Mac dia bieu" },
	{ 80, 198, 1521, 2947, "Thanh Khe Dong" },
	{ 80, 320, 1147, 3123, "Chan nui Truong Bach" },
	{ 80, 181, 1425, 2999, "Luong Thuy Dong" },
	{ 90, 875, 1576, 3177, "Hac Sa Dong" },
	{ 90, 322, 1589, 3164, "Truong Bach Son Bac" },
	{ 90, 321,  967, 2313, "Truong Bach Son Nam" },
	{ 90,  75, 1811, 3012, "Khoa Lang Dong" },
	{ 90, 225, 1474, 3275, "Sa Mac Me Cung 1" },
	{ 90, 226, 1560, 3184, "Sa Mac Me Cung 2" },
	{ 90, 227, 1588, 3237, "Sa Mac Me Cung 3" },
	{ 90, 336, 1124, 3187, "Phong Lang Do" },
	{ 90, 340, 1845, 3438, "Mac Cao Quat" },
	{ 90, 144, 1691, 3020, "Duoc Vuong Dong tang 4" },
	{ 90,  93, 1529, 3166, "Tien Cuc Dong Mat Cung" },
	{ 90, 124, 1675, 3418, "Can Vien Dong Me Cung" },
	{ 90, 152, 1672, 3361, "Tuyet Bao Dong tang 8" },
};
#define PB_SO_BAI  (int)(sizeof(s_bai) / sizeof(s_bai[0]))

// Chon bai hop cap: lay MOC CAO NHAT ma bot du cap, roi boc ngau nhien trong moc do.
// Tra chi so trong s_bai, hoac -1 neu chua du cap 10.
static int pb_ChonBai(int nLevel, int nLech)
{
	int nMoc = -1;
	for (int i = 0; i < PB_SO_BAI; i++)
		if (nLevel >= s_bai[i].nCapToiThieu && s_bai[i].nCapToiThieu > nMoc)
			nMoc = s_bai[i].nCapToiThieu;
	if (nMoc < 0)
		return -1;

	int aTm[PB_SO_BAI], n = 0;
	for (int i = 0; i < PB_SO_BAI; i++)
		if (s_bai[i].nCapToiThieu == nMoc)
			aTm[n++] = i;
	if (n <= 0)
		return -1;
	// tron them nLech (chi so bot) de 20 bot khong cung boc mot bai - cung meo
	// KSimCity.cpp:1329 da dung, vi g_Random goi lien tiep trong mot khung hay ra giong nhau.
	return aTm[((int)g_Random(n) + nLech * 7) % n];
}

#define PB_VISION_MPS     700          // tam tim quai
#define PB_CAST_GAP       4            // so khung toi thieu giua hai lenh danh
#define PB_LAG_SECONDS    10           // bo muc tieu neu chung nay giay khong sut 10% mau

// Tim quai GAN NHAT quanh bot.
//
// CO Y KHONG chep NpcSet.AutoGetNpcNear cua ban client (KNpcSet.cpp:1126): ham do quet TOAN BO
// m_UseIdx - o client thi khong sao vi client chi giu vai tram NPC quanh minh, nhung o server
// la duyet CA THE GIOI moi khung cho MOI bot. Thay bang quet theo REGION: region cua bot +
// 8 region ke, dung m_NpcList co san (KRegion.h:35).
static int pb_FindTarget(int nNpcIdx, int nVision)
{
	const int nSub = Npc[nNpcIdx].m_SubWorldIndex;
	const int nReg = Npc[nNpcIdx].m_RegionIndex;
	if (nSub < 0 || nSub >= MAX_SUBWORLD || nReg < 0)
		return 0;

	int bx = 0, by = 0;
	Npc[nNpcIdx].GetMpsPos(&bx, &by);

	int nBest = 0;
	int nBestD = nVision;
	for (int k = -1; k < 8; k++)
	{
		const int r = (k < 0) ? nReg : SubWorld[nSub].m_Region[nReg].m_nConnectRegion[k];
		if (r < 0)
			continue;
		KIndexNode* pNode = (KIndexNode*)SubWorld[nSub].m_Region[r].m_NpcList.GetHead();
		while (pNode)
		{
			const int i = pNode->m_nIndex;
			pNode = (KIndexNode*)pNode->GetNext();
			if (i <= 0 || i >= MAX_NPC || i == nNpcIdx)      continue;
			if (Npc[i].m_dwID == 0)                          continue;
			if (Npc[i].m_Doing == do_death || Npc[i].m_Doing == do_revive) continue;
			// Dot nay bot CHI danh quai. Bo qua nguoi choi VA bot khac - de con lai
			// mot dot rieng ban ky ve PK, khong de bot tu nhien danh nguoi that.
			if (Npc[i].m_Kind == kind_player)                continue;
			if (!(NpcSet.GetRelation(nNpcIdx, i) & relation_enemy)) continue;

			int ex = 0, ey = 0;
			Npc[i].GetMpsPos(&ex, &ey);
			const int d = g_GetDistance(bx, by, ex, ey);
			if (d < nBestD)
			{
				nBestD = d;
				nBest  = i;
			}
		}
	}
	return nBest;
}

// Chon chieu danh hop VU KHI DANG CAM va hop CAP.
//
// Quy uoc EqtLimit (KSkills.cpp:241-254, da doc): -2 = moi vu khi, -1 = tay khong,
// 0..99 = ho can chien, 100..199 = ho tam xa + 100. Rieng cay nay con quy
// HAND_PARTICULAR (6, ho Trien Thu) ve -1, tuc "tay khong" va "Trien Thu" la MOT.
// Bot di duong quyen (khong cam gi) vi vay khop dung nhom -1.
//
// Du lieu that cua may chu: 1454/1602 chieu la -2 (moi vu khi) va 5 chieu -1 (quyen Thieu
// Lam). Moi phai deu co it nhat mot chieu sat thuong -2 tu cap 10 nen khong phai nao bi cam.
//
// Uu tien: vu khi KHOP DUNG HO (rank 2) an dut "moi vu khi" (rank 1); cung rank thi lay cap
// cao hon. Chieu khong hop vu khi thi BO HAN - vi CanCastSkill se tu choi IM LANG va
// KNpc::DoSkill "goto Exit" khong mot dong bao loi (KNpc.cpp:2314).
static int pb_PickSkill(int nIdx, int nNpcIdx, int* pnLv)
{
	const int nWpn = Player[nIdx].m_ItemList.GetEquipment(itempart_weapon);
	int nWant = -1;                       // -1 = tay khong / Trien Thu
	if (nWpn > 0)
	{
		const int nDetail = Item[nWpn].GetDetailType();
		const int nParti  = Item[nWpn].GetParticular();
		if (nParti == HAND_PARTICULAR)
			nWant = -1;
		else
			nWant = (nDetail == 1) ? (nParti + 100) : nParti;
	}

	int nBest = 0, nBestLv = 0, nBestRank = 0;
	KSkillList& sl = Npc[nNpcIdx].m_SkillList;
	for (int i = 1; i < MAX_NPCSKILL; i++)
	{
		const int id = sl.m_Skills[i].SkillId;
		if (id <= 0 || id >= MAX_SKILL)
			continue;
		int lv = sl.m_Skills[i].CurrentSkillLevel;
		if (lv <= 0) lv = sl.m_Skills[i].SkillLevel;
		if (lv <= 0)
			continue;

		KSkill* p = (KSkill*)g_SkillManager.GetSkill(id, lv);
		if (!p)                       continue;
		if (p->IsAura())              continue;   // vong sang, khong phai chieu danh
		if (p->IsTargetSelf())        continue;
		if (!p->IsTargetEnemy())      continue;

		const int eq = p->GetEquipLimit();
		int nRank = 0;
		if (eq == -2)          nRank = 1;         // moi vu khi
		else if (eq == nWant)  nRank = 2;         // khop dung ho -> an dut
		else                   continue;          // khong hop -> se bi tu choi im lang

		if (nRank > nBestRank || (nRank == nBestRank && lv > nBestLv))
		{
			nBestRank = nRank;
			nBest     = id;
			nBestLv   = lv;
		}
	}
	if (pnLv) *pnLv = nBestLv;
	return nBest;
}

// Dua bot ra bai luyen hop cap. Tra 1 = da o dung bai (danh duoc), 0 = dang lo viec di.
//
// Doi ban do di duong CHINH THONG: KNpc::ChangeWorld(mapId, mpsX, mpsY) - dung thu ma
// KPlayer::Revive goi khi hoi sinh (KPlayer.cpp), va ham Lua NewWorld cung goi vao day.
// Tra 1 la thanh cong; khac 1 nghia la ban do CHUA MO tren may chu nay.
static int pb_RaBai(int nIdx, int nNpcIdx, int nSub, PB_Bot& b, int nLech)
{
	const unsigned int now = SubWorld[nSub].m_dwCurrentTime;
	const int nLevel = Npc[nNpcIdx].m_Level;

	// Chon lai bai khi chua co hoac khi da len cap (moc cao hon co the da mo).
	if (b.nBaiIdx < 0 || b.nBaiLevel != nLevel)
	{
		b.nBaiLevel = nLevel;
		b.nBaiIdx   = pb_ChonBai(nLevel, nLech);
	}
	if (b.nBaiIdx < 0)
		return 1;               // chua du cap 10 -> cu danh tai cho

	const PB_BaiLuyen& bai = s_bai[b.nBaiIdx];
	if (SubWorld[nSub].m_SubWorldID == bai.nMapId)
		return 1;               // da o dung bai

	// Giai cho de khong goi ChangeWorld lien tuc khi ban do chua mo.
	if (b.nDoiMapTick && now - b.nDoiMapTick < (unsigned int)(GAME_FPS * 10))
		return 0;
	b.nDoiMapTick = now;

	b.nTargetNpc = 0;
	b.walk.Reset();
	const int nRet = Npc[nNpcIdx].ChangeWorld(bai.nMapId, bai.nOX * 32, bai.nOY * 32);
	if (nRet == 1)
	{
		pb_Log("[BotBai] %s cap %d -> %s (map %d, o %d,%d)\n",
			   Player[nIdx].m_PlayerName, nLevel, bai.szTen, bai.nMapId, bai.nOX, bai.nOY);
		return 0;
	}

	pb_Log("[BotBai] %s: KHONG VAO DUOC %s (map %d) - ban do co the CHUA MO tren may chu."
		   " Bot danh tai cho.\n", Player[nIdx].m_PlayerName, bai.szTen, bai.nMapId);
	b.nBaiIdx = -1;             // thoi, khoi thu map do nua trong lan chon toi
	return 1;
}

// ===========================================================================
// DI HOANG TIM QUAI (chong bot gom mot cho)
//
// Bot don cuc mot cho la chuyen chac chan xay ra: ca dam cung ChangeWorld toi CUNG MOT
// diem cua bai luyen, danh sach quai quanh do, roi dung im vi khong con gi de danh.
//
// KHONG can boc tep NPC trong pak: may chu DA NAP san toan bo quai vao Npc[] khi mo ban
// do (KRegion::LoadObject -> LoadServerNpc doc REGION_NPC_FILE_SERVER trong maps.pak).
// Nen lay toa do quai DANG SONG con dung hon doc tep - quai chet/hoi sinh deu tinh dung,
// va khong phai lo dinh dang tep hay giai ma pak.
//
// Cach lam: quet MOI region cua ban do hien tai, gom nhung con quai cach bot XA HON
// nNearMps, roi boc ngau nhien mot con lam dich. Moi bot tron them chi so cua no nen 20
// bot khong cung chon mot cho.
//
// PHAI di bang A* (PB_WalkTo): phat thang mot lenh do_run di xa la ket goc ngay - moi
// lenh chi duoc DUNG MOT lan ne vat can (KNpcFindPath.cpp:106-111), vat can thu hai la
// dung im vinh vien. Dung PB_WalkState RIENG (roam) de khong xo lech lo trinh vao phai.
// ===========================================================================
#define PB_ROAM_NEAR      1200     // "gan" = trong chung nay MPS thi khong tinh la diem moi
#define PB_ROAM_MAX_CAND  64       // so ung vien gom toi da
#define PB_ROAM_RETRY     (GAME_FPS * 5)   // giai cho giua hai lan boc diem

// Tim mot con quai XA de lam dich di hoang. Tra 1 va dien toa do, 0 = khong tim duoc.
static int pb_FindRoamSpot(int nNpcIdx, int nSub, int nLech, int* pnX, int* pnY)
{
	if (nSub < 0 || nSub >= MAX_SUBWORLD)
		return 0;
	const int nTong = SubWorld[nSub].m_nTotalRegion;
	if (nTong <= 0)
		return 0;

	int bx = 0, by = 0;
	Npc[nNpcIdx].GetMpsPos(&bx, &by);

	int aX[PB_ROAM_MAX_CAND], aY[PB_ROAM_MAX_CAND];
	int nCand = 0;

	for (int r = 0; r < nTong && nCand < PB_ROAM_MAX_CAND; r++)
	{
		KIndexNode* pNode = (KIndexNode*)SubWorld[nSub].m_Region[r].m_NpcList.GetHead();
		while (pNode && nCand < PB_ROAM_MAX_CAND)
		{
			const int i = pNode->m_nIndex;
			pNode = (KIndexNode*)pNode->GetNext();
			if (i <= 0 || i >= MAX_NPC || i == nNpcIdx)      continue;
			if (Npc[i].m_dwID == 0)                          continue;
			if (Npc[i].m_Doing == do_death || Npc[i].m_Doing == do_revive) continue;
			if (Npc[i].m_Kind == kind_player)                continue;
			if (!(NpcSet.GetRelation(nNpcIdx, i) & relation_enemy)) continue;

			int ex = 0, ey = 0;
			Npc[i].GetMpsPos(&ex, &ey);
			// CHI lay con XA - gan thi bot da danh duoc roi, di toi do la dam chan tai cho.
			if (g_GetDistance(bx, by, ex, ey) <= PB_ROAM_NEAR)
				continue;

			aX[nCand] = ex;
			aY[nCand] = ey;
			nCand++;
		}
	}

	if (nCand <= 0)
		return 0;

	// Tron them chi so bot: g_Random goi lien tiep trong CUNG MOT khung hay ra giong nhau
	// (canh bao da ghi san o KSimCity.cpp:1326-1328), khong tron thi ca dam keo ve mot cho.
	const int k = ((int)g_Random(nCand) + nLech * 11) % nCand;
	*pnX = aX[k];
	*pnY = aY[k];
	return 1;
}

// ===========================================================================
// VONG SANG / BUFF / BUA - BA LOAI KHAC NHAU, BA DUONG DUNG KHAC NHAU
//
// Da doi chieu voi cay tham khao (PickBotAuras KBotManager.cpp:2213 va
// ReapplyEligibleBotPassives :2241) roi anh xa sang ten cua cay nay:
//
//  1. VONG SANG  (IsAura() == TRUE, KSkills.h:195)
//     KHONG cast nhu chieu thuong. Client bat vong sang bang PlayerSwitchAura - ham do
//     CHI CO PHIA CLIENT, nen phia server phai TU DAT: Npc[].SetAuraSkill(id)
//     (KNpc.h:719, ghi vao m_ActiveAuraID :289).
//     PHAI DAT LAI DEU DAN: m_ActiveAuraID bi xoa khi hoi sinh, khong dat lai thi bot
//     song lai la mat vong sang ma khong co dau hieu gi.
//
//  2. BUA / BI DONG (SKILL_SS_PassivityNpcState, SkillDef.h:105)
//     Cast MOT LAN moi khi len cap. Ly do ton tai (ban tham khao ghi ro): luc bot vua
//     hoc, no chua du cap nen cong CastPassivitySkill tu choi; len cap roi phai cap lai,
//     khong thi bua nam trong tui ma khong bao gio co tac dung.
//
//  3. BUFF chu dong (SKILL_SS_InitiativeNpcState + IsTargetSelf())
//     Cast len CHINH MINH theo chu ky. Day la thu nguoi choi bam tay truoc khi danh.
//
// Ca ba deu goi KSkill::Cast(nLauncher, nParam1, nParam2) (KSkills.h:172) voi
// nParam1 = -1 va nParam2 = chinh khe NPC cua bot - tuc "nham vao ban than".
// ===========================================================================
#define PB_BUFF_LAI   (GAME_FPS * 60)     // buff lai moi 60 giay

static void pb_ApplyAuraBuff(int nIdx, int nNpcIdx, PB_Bot& b, unsigned int now)
{
	const int nLevel = Npc[nNpcIdx].m_Level;
	const bool bLenCap = (b.nAuraLevel != nLevel);
	const bool bToiGio = (b.nBuffTick == 0 || now - b.nBuffTick >= (unsigned int)PB_BUFF_LAI);
	if (!bLenCap && !bToiGio)
	{
		// Khong den ky nhung VAN dat lai vong sang neu no bi mat (vd vua hoi sinh).
		if (b.nAuraSkill > 0 && Npc[nNpcIdx].m_ActiveAuraID != b.nAuraSkill)
			Npc[nNpcIdx].SetAuraSkill(b.nAuraSkill);
		return;
	}
	b.nAuraLevel = nLevel;
	b.nBuffTick  = now;

	int nAura = 0, nAuraRq = -1;
	int nBua = 0, nBuff = 0;
	KSkillList& sl = Npc[nNpcIdx].m_SkillList;

	for (int i = 1; i < MAX_NPCSKILL; i++)
	{
		const int id = sl.m_Skills[i].SkillId;
		if (id <= 0 || id >= MAX_SKILL)
			continue;
		int lv = sl.m_Skills[i].CurrentSkillLevel;
		if (lv <= 0) lv = sl.m_Skills[i].SkillLevel;
		if (lv <= 0)
			continue;

		KSkill* p = (KSkill*)g_SkillManager.GetSkill(id, lv);
		if (!p)
			continue;

		// Cap yeu cau kep tran 80 y ban tham khao: tren nua la ky nang 90/150,
		// bot chua den luc dung.
		int rq = p->GetSkillReqLevel();
		if (rq > 80) rq = 80;
		if (nLevel < rq)
			continue;

		if (p->IsAura())
		{
			// vong sang: lay cai co cap yeu cau CAO NHAT ma bot du cap
			if (rq > nAuraRq) { nAuraRq = rq; nAura = id; }
			continue;
		}

		const int st = p->GetSkillStyle();
		if (st == SKILL_SS_PassivityNpcState)
		{
			p->Cast(nNpcIdx, -1, nNpcIdx);
			nBua++;
		}
		else if (st == SKILL_SS_InitiativeNpcState && p->IsTargetSelf())
		{
			p->Cast(nNpcIdx, -1, nNpcIdx);
			nBuff++;
		}
	}

	if (nAura > 0 && nAura != b.nAuraSkill)
	{
		b.nAuraSkill = nAura;
		pb_Log("[BotVongSang] %s cap %d bat vong sang %d\n",
			   Player[nIdx].m_PlayerName, nLevel, nAura);
	}
	if (b.nAuraSkill > 0)
		Npc[nNpcIdx].SetAuraSkill(b.nAuraSkill);

	if (bLenCap && (nBua || nBuff))
		pb_Log("[BotBuff] %s cap %d: %d bua bi dong + %d buff\n",
			   Player[nIdx].m_PlayerName, nLevel, nBua, nBuff);
}

// Mot nhip DI HOANG. Tra 1 = dang di (chua toi), 0 = khong co cho di / da toi noi.
static int pb_Roam(int nIdx, int nNpcIdx, int nSub, PB_Bot& b, int nLech)
{
	const unsigned int now = SubWorld[nSub].m_dwCurrentTime;

	if (b.nRoamX == 0 && b.nRoamY == 0)
	{
		if (b.nRoamTick && now - b.nRoamTick < (unsigned int)PB_ROAM_RETRY)
			return 0;
		b.nRoamTick = now;
		int rx = 0, ry = 0;
		if (!pb_FindRoamSpot(nNpcIdx, nSub, nLech, &rx, &ry))
			return 0;              // ban do het quai - dung yen cho hoi sinh
		b.nRoamX = rx;
		b.nRoamY = ry;
		b.roam.Reset();
		pb_Log("[BotHoang] %s het quai gan -> di toi o %d,%d (map %d)\n",
			   Player[nIdx].m_PlayerName, rx / 32, ry / 32, SubWorld[nSub].m_SubWorldID);
	}

	// Dung sat quai la du - toi noi thi vong danh se tu bat duoc muc tieu.
	const int nRet = PB_WalkTo(nNpcIdx, b.nRoamX, b.nRoamY, nSub, b.roam, 400);
	if (nRet == 0)
		return 1;                  // dang di

	if (nRet < 0)
		pb_Log("[BotHoang] %s khong tim duoc duong toi o %d,%d - boc cho khac\n",
			   Player[nIdx].m_PlayerName, b.nRoamX / 32, b.nRoamY / 32);

	b.nRoamX = 0;                  // toi noi hoac tac duong -> lan sau boc cho khac
	b.nRoamY = 0;
	b.roam.Reset();
	return 0;
}

// Mot nhip DANH NHAU. Tra 1 neu dang co viec (dang danh), 0 neu khong co muc tieu.
static int pb_Fight(int nIdx, int nNpcIdx, int nSub, PB_Bot& b)
{
	const unsigned int now = SubWorld[nSub].m_dwCurrentTime;

	// KNpc::DoSkill (KNpc.cpp:2329-2330) tra ve som neu IsPlayer() && !m_FightMode,
	// nen KHONG bat co nay thi bot vung tay ma khong co gi xay ra.
	if (!Npc[nNpcIdx].m_FightMode)
		Npc[nNpcIdx].SetFightMode(TRUE);

	// Chon lai chieu khi chua co, hoac khi da len cap (co chieu manh hon).
	if (b.nAtkSkill <= 0 || b.nAtkPickLevel != Npc[nNpcIdx].m_Level)
	{
		b.nAtkPickLevel = Npc[nNpcIdx].m_Level;
		const int nCu = b.nAtkSkill;
		b.nAtkSkill = pb_PickSkill(nIdx, nNpcIdx, &b.nAtkSkillLv);
		if (b.nAtkSkill != nCu)
			pb_Log("[BotDanh] %s cap %d dung chieu %d (cap %d)\n",
				   Player[nIdx].m_PlayerName, Npc[nNpcIdx].m_Level, b.nAtkSkill, b.nAtkSkillLv);
	}
	if (b.nAtkSkill <= 0)
		return 0;

	// ---- muc tieu cu con dung khong ----
	int t = b.nTargetNpc;
	if (t > 0 && t < MAX_NPC)
	{
		if (Npc[t].m_dwID == 0 || Npc[t].m_SubWorldIndex != nSub
		 || Npc[t].m_Doing == do_death || Npc[t].m_Doing == do_revive)
			t = 0;
	}
	else
		t = 0;

	// ---- chong ket: 10 giay khong sut noi 10% mau thi bo con do ----
	if (t)
	{
		if (b.nLagLife <= 0 || Npc[t].m_CurrentLife < b.nLagLife * 9 / 10)
		{
			b.nLagLife = Npc[t].m_CurrentLife;
			b.nLagTick = now;
		}
		else if (now - b.nLagTick >= (unsigned int)(GAME_FPS * PB_LAG_SECONDS))
		{
			pb_Log("[BotDanh] %s bo muc tieu %d: %d giay khong sut duoc mau\n",
				   Player[nIdx].m_PlayerName, t, PB_LAG_SECONDS);
			t = 0;
		}
	}

	if (!t)
	{
		t = pb_FindTarget(nNpcIdx, PB_VISION_MPS);
		b.nTargetNpc = t;
		b.nLagTick   = now;
		b.nLagLife   = t ? Npc[t].m_CurrentLife : 0;
		if (!t)
			return 0;
	}

	// ---- ra don ----
	// Khong phat moi khung: DoWalk/DoSkill deu phat goi tin ra vung lan can.
	// KNpc::DoSkill co nhanh #ifdef _SERVER TU phat do_run toi muc tieu khi con ngoai tam
	// (KNpc.cpp:2338-2349), nen KHONG can tu dan bot vao gan - engine lo phan ap sat.
	if (now - b.nCastTick >= (unsigned int)PB_CAST_GAP)
	{
		b.nCastTick = now;
		// Dung khuon may chu dung cho nguoi choi that khi nhan c2s_npcskill
		// (KProtocolProcess::NpcSkillCommand, KProtocolProcess.cpp:4926):
		//   SendCommand(do_skill, <id chieu>, -1, <khe NPC muc tieu>)
		// ProcCommand tu goi FindSame + SetActiveSkill (KNpc.cpp:861-871) nen khong
		// phai dat chieu truoc.
		Npc[nNpcIdx].SendCommand(do_skill, b.nAtkSkill, -1, t);
	}
	return 1;
}

// ===========================================================================
// BOT NOI CHUYEN
//
// Kho cau: settings\simcity\chat.txt - 2745 dong, 21 nhom tinh huong, DA CO SAN tren
// may chu dang chay (khong phai chep them gi). Cot 1 = Type, cot 2 = Chat.
//
// Duong phat loi: KPlayerChat::NpcChat(nNpcIdx, ...) - CHINH duong ma bot NPC SimCity
// dang dung va da chay that. Bot KPlayer cung la mot KNpc nen dung thang duoc.
// KRegion::BroadCast loc m_nNetConnectIdx >= 0 nen bot (-1) khong tu gui cho minh,
// con nguoi that quanh do VAN NHAN du.
//
// CHONG LAP: moi bot giu con tro rieng nChatCuoi va tien theo buoc le (nhay 7 cau) nen
// hai bot canh nhau khong doc trung. g_Random goi lien tiep trong CUNG MOT KHUNG hay ra
// giong nhau (canh bao da ghi san tai KSimCity.cpp:1326-1328) nen KHONG dung mot minh no.
// ===========================================================================
// Nap kho cau. Tra so cau da nap.
// LUU Y duong dan: Lua 4.0 nuot escape "\" nen o Lua phai truyen gach XUOI
// ("/settings/simcity/chat.txt"); g_GetFullPath nhan ca hai kieu.
static int pb_LoadChat(const char* szFile, const char* szType)
{
	KTabFile tab;
	if (!tab.Load((LPSTR)szFile))
		return 0;
	s_pbChatCount = 0;
	const int nH = tab.GetHeight();
	for (int row = 2; row <= nH && s_pbChatCount < PB_CHAT_MAX; row++)   // row 1 = tieu de
	{
		char szT[64] = { 0 };
		tab.GetString(row, 1, (LPSTR)"", szT, sizeof(szT));
		if (szType && szType[0] && strcmp(szT, szType) != 0)
			continue;
		char szC[PB_CHAT_LEN] = { 0 };
		tab.GetString(row, 2, (LPSTR)"", szC, PB_CHAT_LEN);
		if (!szC[0])
			continue;
		strncpy(s_pbChat[s_pbChatCount], szC, PB_CHAT_LEN - 1);
		s_pbChat[s_pbChatCount][PB_CHAT_LEN - 1] = 0;
		s_pbChatCount++;
	}
	return s_pbChatCount;
}

// Mot nhip NOI CHUYEN cho mot bot.
static void pb_Chat(int nNpcIdx, PB_Bot& b, unsigned int now, int nLech)
{
	if (s_pbChatRate <= 0 || s_pbChatCount <= 0)
		return;
	// moi bot xet mot lan moi giay, lech nhau theo chi so de khong dong loat cung luc
	if (((now + nLech) % (unsigned int)GAME_FPS) != 0)
		return;
	if ((int)g_Random(1000) >= s_pbChatRate)
		return;

	b.nChatCuoi = (b.nChatCuoi + 7 + nLech) % s_pbChatCount;
	char* p = s_pbChat[b.nChatCuoi];
	const int nLen = (int)strlen(p);
	if (nLen > 0)
		KPlayerChat::NpcChat(nNpcIdx, p, nLen, false);
}

// ===========================================================================
// TRA LOI TIN NHAN MAT
//
// Duong di: nguoi choi go tin nhan rieng -> goi c2s_extendchat toi GameServer ->
// KSOServer bat goi chat_someonechat, hoi Core "ten nay co phai bot khong".
// Neu phai thi Core TU TRA LOI ngay va GameServer KHONG chuyen tiep len may chu chat
// (S3Relay khong biet gi ve bot phia server nen co chuyen len cung roi vao hu khong).
//
// GUI LOI DAP bang KPlayerChat::SendSystemInfo (KPlayerChat.h:159) - ham CO SAN,
// gui thang toi mot khe Player[] kem TEN NGUOI GUI. CO Y khong tu dung goi tin:
// cay nay va cay tham khao KHAC CAU TRUC goi chat (CHAT_FEEDBACK ben nay khong co
// truong item[]), tu dong byte la de lech va client doc ra rac.
//
// NOI DUNG: bat y dinh bang tu khoa ASCII khong dau. Nguoi go CO DAU (TCVN3) se truot
// het tu khoa va roi vao cau chung - nen MOI tin deu duoc tra loi, khong bao gio im.
// ===========================================================================

// Tim bot theo TEN nhan vat. Tra khe Player[] hoac 0.
static int pb_FindBotByName(const char* szName)
{
	if (!szName || !szName[0])
		return 0;
	for (int i = 0; i < s_botCount; i++)
	{
		const int nIdx = s_bots[i].nPlayerIdx;
		if (nIdx <= 0 || nIdx >= MAX_PLAYER)                 continue;
		if (Player[nIdx].m_dwID != s_bots[i].dwID)           continue;
		if (strcmp(Player[nIdx].m_PlayerName, szName) == 0)  return nIdx;
	}
	return 0;
}

// Chep sang chu thuong, chi ASCII (chu co dau TCVN3 giu nguyen, se khong khop tu khoa).
static void pb_ToLower(const char* szIn, int nLen, char* szOut, int nMax)
{
	int j = 0;
	for (int i = 0; i < nLen && j < nMax - 1; i++)
	{
		char c = szIn[i];
		if (c >= (char)0x41 && c <= (char)0x5A)
			c = (char)(c + 32);
		szOut[j++] = c;
	}
	szOut[j] = 0;
}

int PB_WhisperReply(const PB_WHISPER* p)
{
	if (!p || !p->szTarget || !p->szMsg || p->nMsgLen <= 0)
		return 0;
	if (p->nSenderIdx <= 0 || p->nSenderIdx >= MAX_PLAYER)
		return 0;

	const int nBot = pb_FindBotByName(p->szTarget);
	if (nBot <= 0)
		return 0;                       // khong phai bot -> de GameServer lo tiep nhu cu

	PB_Bot* pB = NULL;
	for (int i = 0; i < s_botCount; i++)
		if (s_bots[i].nPlayerIdx == nBot) { pB = &s_bots[i]; break; }

	char szLow[256];
	int nLen = p->nMsgLen;
	if (nLen > 200) nLen = 200;
	pb_ToLower(p->szMsg, nLen, szLow, sizeof(szLow));

	char szTraLoi[220];
	szTraLoi[0] = 0;

	// ---- bat y dinh ----
	if (strstr(szLow, "chao") || strstr(szLow, "hi ") || strstr(szLow, "hello"))
		sprintf(szTraLoi, "Chao ban, minh dang ban chut viec.");
	else if (strstr(szLow, "dang o dau") || strstr(szLow, "o dau") || strstr(szLow, "cho nao"))
	{
		const int nNpc = Player[nBot].m_nIndex;
		int mx = 0, my = 0;
		if (nNpc > 0 && nNpc < MAX_NPC)
			Npc[nNpc].GetMpsPos(&mx, &my);
		sprintf(szTraLoi, "Minh dang o o %d,%d.", mx / 32, my / 32);
	}
	else if (strstr(szLow, "cap") || strstr(szLow, "level") || strstr(szLow, "lv"))
	{
		const int nNpc = Player[nBot].m_nIndex;
		sprintf(szTraLoi, "Minh cap %d.",
			  (nNpc > 0 && nNpc < MAX_NPC) ? Npc[nNpc].m_Level : 0);
	}
	else if (strstr(szLow, "phai") || strstr(szLow, "mon phai"))
	{
		const int nF = (int)Player[nBot].m_cFaction.m_nCurFaction;
		if (nF >= 0 && nF < MAX_FACTION)
			sprintf(szTraLoi, "Minh o %s.", s_facNpc[nF].szTen);
		else
			sprintf(szTraLoi, "Minh chua vao phai nao.");
	}
	else if (strstr(szLow, "nhom") || strstr(szLow, "to doi") || strstr(szLow, "party"))
		sprintf(szTraLoi, "Minh dang di mot minh, de khi khac nhe.");
	else if (strstr(szLow, "ban gi") || strstr(szLow, "mua") || strstr(szLow, "gia"))
		sprintf(szTraLoi, "Minh khong buon ban gi dau.");

	// ---- khong khop tu khoa -> lay mot cau trong kho, KHONG BAO GIO im lang ----
	if (!szTraLoi[0])
	{
		if (s_pbChatCount > 0 && pB)
		{
			pB->nChatCuoi = (pB->nChatCuoi + 13) % s_pbChatCount;
			strncpy(szTraLoi, s_pbChat[pB->nChatCuoi], sizeof(szTraLoi) - 1);
			szTraLoi[sizeof(szTraLoi) - 1] = 0;
		}
		else
			sprintf(szTraLoi, "U, minh nghe day.");
	}

	const int nOut = (int)strlen(szTraLoi);
	if (nOut <= 0)
		return 0;

	// nChannedID = -1: kenh rieng (mac dinh cua ham). Ten nguoi gui = ten BOT de nguoi
	// choi thay dung la bot dang tra loi minh.
	KPlayerChat::SendSystemInfo(0, p->nSenderIdx, Player[nBot].m_PlayerName,
							  szTraLoi, nOut, -1);

	pb_Log("[BotPM] %s <- %s: \"%.60s\" | dap: \"%.60s\"\n",
		   Player[nBot].m_PlayerName, Player[p->nSenderIdx].m_PlayerName,
		   p->szMsg, szTraLoi);
	return 1;
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

	// Noi chuyen chay SONG SONG voi moi viec khac (di duong, danh nhau) - nguoi that cung
	// vua danh vua noi. Dat truoc cac nhanh return de khong bi nhanh nao nuot mat.
	{
		int nLech = 0;
		for (int q = 0; q < s_botCount; q++)
			if (&s_bots[q] == &b) { nLech = q; break; }
		pb_Chat(nNpcIdx, b, nowAll, nLech);
	}

	// ---------------------------------------------------------------- TU HOI SINH
	// BOT CHET LA NAM MAI neu khong co doan nay. Duong tu hoi sinh cua nguoi that nam
	// trong KPlayer::Active (KPlayer.cpp), nhung ham do MO DAU bang
	//     if (m_nNetConnectIdx == -1 || m_bExchangeServer) return;
	// nen bot khong bao gio toi duoc.
	//
	// CO Y KHONG SUA KPlayer::Active: co -1 do NGUOI CHOI UY THAC cung mang, mo cong o
	// day la doi hanh vi cua ho - va do la ham chay cho MOI nguoi choi MOI khung. Lam
	// rieng trong nhip bot thi khong dong toi ai, va ta chu dong duoc hoan toan.
	//
	// KPlayer::Revive (KPlayer.h:582, public) an toan voi bot: goi tin ve chinh bot la
	// no-op (PackDataToClient chan bien), BroadCastRevive van toi nguoi that quanh do,
	// va REMOTE_REVIVE_TYPE co san duong lui ve map 53 (52032,101696) neu diem hoi sinh
	// hong. No cung hoi day mau/noi luc/the luc roi SendCommand(do_revive).
	if (Npc[nNpcIdx].m_Doing == do_death || Npc[nNpcIdx].m_Doing == do_revive)
	{
		if (b.nChetTuTick == 0)
		{
			b.nChetTuTick = nowAll;
			b.walk.Reset();              // lo trinh cu vo nghia sau khi chet
		}
		// Cho 3 giay cho cai chet "kip dien ra" (client con dang ve), roi dung day.
		else if (Npc[nNpcIdx].m_Doing == do_revive
		      && nowAll - b.nChetTuTick >= (unsigned int)(GAME_FPS * 3))
		{
			b.nChetTuTick = 0;
			Player[nIdx].Revive(REMOTE_REVIVE_TYPE);
			pb_Log("[Bot] %s da chet -> tu hoi sinh\n", Player[nIdx].m_PlayerName);
		}
		return;                          // dang chet thi khong lam viec gi khac
	}
	b.nChetTuTick = 0;

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

	// ---------------------------------------------------------------- DANH QUAI
	if (b.nAi == PB_AI_FIGHT)
	{
		// Ra bai hop cap TRUOC, danh sau. pb_RaBai tra 1 khi da dung cho.
		int nLech = 0;
		for (int q = 0; q < s_botCount; q++)
			if (&s_bots[q] == &b) { nLech = q; break; }
		// Vong sang / bua / buff xet TRUOC khi danh: bot phai vao tran voi day du
		// trang thai, giong nguoi choi bam buff truoc roi moi lao vao.
		pb_ApplyAuraBuff(nIdx, nNpcIdx, b, nowAll);
		if (pb_RaBai(nIdx, nNpcIdx, nSub, b, nLech))
		{
			// Co quai gan thi danh; het quai gan thi DI HOANG toi cho khac tren CUNG ban do.
			// Nho vay bot tu rai ra thay vi dam mot cho o diem ChangeWorld.
			if (pb_Fight(nIdx, nNpcIdx, nSub, b))
			{
				b.nRoamX = 0;      // dang danh -> huy chuyen di hoang
				b.nRoamY = 0;
			}
			else
				pb_Roam(nIdx, nNpcIdx, nSub, b, nLech);
		}
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
		pb_Log("[Bot] %s BO CUOC: NPC %s o ban do khac (bot map=%d, npc map=%d)\n",
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
			pb_Log("[Bot] %s BO CUOC sau %d lan khong tim duoc duong toi %s"
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
		pb_Log("[Bot] %s da vao %s (phai %d, he %d)\n",
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
		pb_Log("[Bot] %s XIN VAO %s THAT BAI: m_nCurFaction=%d (he bot=%d, he phai can=%d)\n",
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
			pb_Log("[Bot] da go xong toan bo bot\n");
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
