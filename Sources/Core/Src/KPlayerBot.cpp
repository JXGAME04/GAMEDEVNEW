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
#include "Scene/ObstacleDef.h"  // Obstacle_NULL - kiem o trong khi lach dam dong
#include "KRegion.h"        // duyet m_NpcList de tim quai theo region
#include "KMath.h"         // g_GetDistance
#include "KPlayerChat.h"   // KPlayerChat::NpcChat - bot noi chuyen
#include "KTabFile.h"      // doc kho cau thoai chat.txt
#include "KObjSet.h"     // ObjSet / Object[] - quet do roi cho bot nhat
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
	PB_WalkState chase;                       // lo trinh A* DUOI DANH (rieng, khong dung chung)
	int          aCam[8];                     // so den: muc tieu hong (8 o - phan bien 18/08:
	                                          // 4 o bi xoay vong de khi >4 con cung ket)
	unsigned int aCamTick[8];                 // moc cam tung con (het han thi tha)
	unsigned int nChaseLegTick;               // gion nhip chang thang cuoi khi ap sat
	unsigned int nChatCamToi;                 // moc tick duoc phep noi cau tiep theo
	int          nBuocRaX, nBuocRaY;          // buoc ra khoi diem dat chan sau khi doi map
	// ---- lach dam dong ----
	int          nJamX, nJamY;                // diem neo do "dam chan tai cho"
	unsigned int nJamTick;                    // tu bao gio dung quanh diem neo
	unsigned int nLachTick;                   // lan lach gan nhat (gion nhip)
	int          nLachDem;                    // dem lan lach - xoay huong khoi dau, chong
	                                          // lach qua roi lach nguoc lai cung mot cap o
	// ---- dung vat pham ----
	unsigned int nUongTick;                   // lan uong binh gan nhat
	unsigned int nMoTuiTick;                  // lan mo tui duoc pham gan nhat
	int          nHpTruoc;                    // HP muc tieu nhip truoc (do sat thuong that)
	unsigned int nDameTick;                   // lan xac nhan sat thuong gan nhat (gion log)
	// ---- luyen cap ----
	int          nBaiIdx;                     // chi so bai luyen dang nham (-1 = chua chon)
	int          nBaiLevel;                   // cap bot luc chon bai (len cap thi chon lai)
	unsigned int nDoiMapTick;                 // giai cho giua hai lan thu doi map
	int          nChatCuoi;                   // con tro cau thoai (chong lap giua cac bot)
	// ---- di hoang tim quai ----
	PB_WalkState roam;                        // trang thai di bo RIENG cho viec di hoang
	int          nRoamX, nRoamY;              // diem dang nham toi (MPS), 0 = chua co
	unsigned int nRoamTick;                   // moc boc diem lan cuoi
	// ---- nhat do roi ----
	int          nLootObjIdx;                 // chi so Object dang nham nhat (0 = khong)
	int          nLootObjID;                  // m_nID chot danh tinh (chong tai su dung khe)
	int          aLootBan[4];                 // obj nhat truot -> ne 30 giay (4 o, phan bien
	                                          // 18/08: 1 o thi 2 obj hong la ne ping-pong)
	unsigned int aLootBanTick[4];             // moc ne tung obj
	PB_WalkState loot;                        // duong di A* rieng cho viec nhat do
	unsigned int nLootScanTick;               // lan quet do roi gan nhat (gion nhip)
	unsigned int nDonTuiTick;                 // lan don tui gan nhat
	unsigned int nPhamViTick;                 // dong ho "toi noi ma van ngoai tam" (6s -> cam)
	unsigned int nBienLogTick;                // gion log [BotBien] 1 dong/giay
	// ---- tra loi PM cho (gion nhip nhu nguoi that) ----
	char         szPmTraLoi[224];             // cau dang cho gui (0 dau = khong co)
	int          nPmSenderIdx;                // nguoi hoi
	unsigned int nPmDenHan;                   // toi han nay moi gui (0 = khong cho)
	unsigned int nPmCamToi;                   // som nhat duoc nhan cau hoi ke tiep
	unsigned int nPmLapHash;                  // hash cau tra loi truoc (chong lap y het)
	// ---- gian dan (chu game 18/08: "len map check xung quanh co hon 20 bot thi
	//      tu di chuyen di toa do xa de luyen") ----
	unsigned int nDanKiemTick;                // lan kiem mat do gan nhat
	unsigned int nDanDenHan;                  // 0 = khong di tan; khac 0 = han chot di tan
	unsigned int nDanNghiToi;                 // sau mot lan di tan, nghi den moc nay moi kiem lai
	// ---- tu cuu khi ket luoi + phat lai vu khi (19/08 chieu) ----
	unsigned int nCuuKiemTick;                // nhip soi ket (10 giay/lan, re)
	unsigned int nCuuTick;                    // lan cuu gan nhat (gion 60 giay)
	int          nAStarThua;                  // so lan roam A* thua LIEN TIEP
	unsigned int nVuKhiTick;                  // lan phat lai vu khi gan nhat
	int          nVuKhiThu;                   // so lan phat lai vu khi (tran 5)
	int          nTrangBiLevel;               // cap lan cuoi da thu mac do (0 = chua)
	unsigned int nQhtTick;                    // lan cham Que Hoa Tuu gan nhat
	// ---- Da Tau (19/08 trua) ----
	unsigned int nDaTauTick;                  // gion nhip hanh dong Da Tau
	unsigned int nDaTauNghi;                  // >0: nghi (du 40/ngay) toi moc nay
	int          nDaTauNut;                   // con tro thu 6 nut ruong thuong
	int          nDaTauDaGhi;                 // (het dung tu dot 4 - giu cho don gian)
	int          nDaTauChon;                  // 1 = duoc boc NGAU NHIEN lam Da Tau
	int          nDaTauKe;                    // dem no luc doi nhiem vu xau (huy/xin moi)
	unsigned int nDaTauHan;                   // (loai 6) han chot gom manh (45 phut)
	unsigned int nDaTauThu;                   // (loai 6/sap) moc ky farm / chau hang
	int          nBanSap;                     // 1 = duoc boc ra thanh ngoi ban sap
	int          nBanSapXong;                 // 1 = da bay hang + mo sap
	int          nBanSapNgoi;                 // 1 = da ngoi xuong (do_sit)
	// ---- may trang thai Da Tau (19/08 toi #2) ----
	int          nDtPha;                      // DTB_* - pha dang o
	int          nDtToiNpcThu;                // so nhip da co gang toi NPC
	int          nDtXaFuThu;                  // so lan kich godatau ma chua sang map
	int          nDtTraThu;                   // so lan tra ma course van = 1
	int          nDtTuiThu;                   // so lan tra hut vi tui day
	int          nDtCoCu;                     // so cuon nhip truoc (do tien do)
	unsigned int nDtFarmMoc;                  // moc lan CUOI cuon tang (chong farm vo han)
	unsigned int nNguaTick;                   // gion nhip kiem cuoi ngua trong thanh
	// ---- to doi ----
	int          nWantParty;                  // chot MOT LAN khi sinh: bot nay co muon vao nhom
	                                          // (%PB_NHOM_RATE) - roll lai moi luot la ai roi
	                                          // cung co luc trung, cuoi cung 100% deu vao nhom
	PB_WalkState follow;                      // duong A* bam theo doi truong
	unsigned int nFollowNghiToi;              // >0: follow vua thua A*, nghi toi moc nay
	// ---- luu du lieu (18/08) ----
	unsigned int nLuuTick;                    // GetGameTime() lan luu gan nhat (nhip 30 giay)
	// ---- vong sang / buff / bua ----
	int          nAuraSkill;                  // vong sang dang bat (0 = chua co)
	int          nAuraLevel;                  // cap bot luc chon vong sang
	unsigned int nBuffTick;                   // moc buff lan cuoi
	int          nAuraPhien;                  // Nga Mi: dang o phien vong sang nao (0/1)
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

// Ghi nhat ky khi mot bot DAP TRAP (goi tu KNpc::CheckTrap, truoc khi bo qua).
// De ngoai nay vi pb_Log la static - KNpc.cpp khong goi thang duoc.
void PB_TrapLog(int nPlayerIdx, unsigned long dwScriptId, int nMapX, int nMapY)
{
	pb_Log("[BotTrap] %s dap trap id=%08X tai o(%d,%d) -> BO QUA (bot mien trap)\n",
	       (nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER)
	           ? Player[nPlayerIdx].m_PlayerName : "?",
	       (unsigned int)dwScriptId, nMapX, nMapY);
}

#include <stdarg.h>
void PB_LogNgoai(const char* szFmt, ...)
{
	char szMsg[512];
	va_list ap;
	va_start(ap, szFmt);
	_vsnprintf(szMsg, sizeof(szMsg) - 1, szFmt, ap);
	va_end(ap);
	szMsg[sizeof(szMsg) - 1] = 0;
	pb_Log("%s", szMsg);
}

int PB_IsBot(int nPlayerIdx)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return 0;
	for (int i = 0; i < s_botCount; i++)
		// (18/08 phan bien) PHAI so ca dwID: khe bot bi thao ngoai duong pb_KillBot
		// (vd GM KickOutPlayer) roi cap lai cho NGUOI THAT ma chi so nPlayerIdx thi
		// nguoi that se bi coi la bot (dong dau blob, mien trap, Lua IsBot=1).
		if (s_bots[i].nPlayerIdx == nPlayerIdx
		 && s_bots[i].dwID == Player[nPlayerIdx].m_dwID)
			return 1;
	return 0;
}

// ===========================================================================
// TO DOI (chu game 18/08): "bot tu mo to doi - thanh vien tu theo sau doi truong,
// du cap nhay map thi tu thoat party, giai tan nhom doi voi doi truong, ti le lap
// nhom phai gioi han". Khuon lay tu ban tham khao DT_FormBotTeam +
// ManageGrindParties (KBotManager.cpp:1754/:1830) - khac biet JX1:
//   SetCreatTeamFlag -> SetCanTeamFlag; khong co GetMapStyle (thay bang "cung
//   subworld"); ban tham khao KHONG co ti le (ghep 100%) nen ti le la them moi.
// Vao nhom truc tiep qua GetInviteReply (KPlayerTeam.cpp:414) = bam "dong y" ho,
// khong can goi tin; goi tin dong bo ben trong tu toi nguoi that quanh do.
// TUYET DOI khong dung KPlayer::TeamDismiss - vong for cua no (KPlayer.cpp:1836)
// thieu guard slot -1, cham Player[-1].
#define PB_NHOM_RATE     100   // (19/08 trua chu game: "cho bot party full")                    // % bot muon vao nhom
#define PB_NHOM_MAX_MEM  7     // (19/08 trua) full nhom 8 nguoi = MAX_TEAM_MEMBER                     // toi da 3 thanh vien (nhom 4 nguoi)
#define PB_NHOM_QUAN_MS  5000                  // nhip quan ly nhom
#define PB_NHOM_XAO_MS   600000                // 10 phut xao lai nhom mot lan

// ---- Da Tau: task value + kich ban + cap toi thieu (dung tu PB_SetDaTau tro di)
#define PB_DT_COURSE  1028
#define PB_DT_LOAI    1021
#define PB_DT_MAP     1031
#define PB_DT_CAN     1032
#define PB_DT_CO      1025
#define PB_DT_NGAY    2420
#define PB_DT_SCRIPT  "\\script\\global\\seasonnpc.lua"
#define PB_DT_CAP_TOI_THIEU  80    // (19/08 toi) chu game: chi bot cap >= 80
// (19/08 toi #2) TRAN SO CUON nhan lam. Bang tasklink_findmaps.txt o bac link 11
// (bot >= cap 80) chi con dong Num=15 (95,24% trong so) va Num=3 loai MAT CHI.
// 15 cuon dia do ~ 13.500 con quai; mat chi CHI rot tu boss xanh ~1/31.360 con.
// => nhan nhiem vu nang la treo may vinh vien; qua tran thi DOI NHIEM VU (mien phi).
#define PB_DT_CUON_TOI_DA    5
// cac pha cua may trang thai Da Tau
#define DTB_NGHI        0
#define DTB_TOI_NPC     1
#define DTB_THOAI       2
#define DTB_LOC         3
#define DTB_TOI_XAPHU   4
#define DTB_GODATAU     5
#define DTB_FARM_NV     6
#define DTB_FARM_BAI    7
#define DTB_VE_TRA      8
#define DTB_THUONG      9
#define PB_BAM_GAN       150                   // ban kinh bam theo doi truong (MPS)
#define PB_BAM_NHA       200                   // xa hon nay la bo danh de bam theo

static bool pb_TrongNhom(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		return false;
	const KPlayerTeam& t = Player[nIdx].m_cTeam;
	return (t.m_nFlag != 0 && t.m_nID >= 0 && t.m_nID < MAX_TEAM);
}

static int pb_DoiTruongCua(int nIdx)
{
	if (!pb_TrongNhom(nIdx))
		return 0;
	const int c = g_Team[Player[nIdx].m_cTeam.m_nID].m_nCaptain;
	return (c > 0 && c < MAX_PLAYER) ? c : 0;
}

static void pb_RoiNhom(int nIdx, const char* szLyDo)
{
	if (!pb_TrongNhom(nIdx))
		return;
	const int bTruong = (pb_DoiTruongCua(nIdx) == nIdx) ? 1 : 0;
	PLAYER_APPLY_LEAVE_TEAM sLeave;
	memset(&sLeave, 0, sizeof(sLeave));
	sLeave.ProtocolType = c2s_teamapplyleave;
	// LeaveTeam -> KTeam::DeleteMember: doi truong = giai tan CA nhom (nhanh nay
	// CO guard slot -1, KPlayerTeam.cpp:745), thanh vien = rut mot minh.
	Player[nIdx].LeaveTeam((BYTE*)&sLeave);
	pb_Log("[BotNhom] %s %s (%s)\n", Player[nIdx].m_PlayerName,
	       bTruong ? "GIAI TAN nhom" : "roi nhom", szLyDo);
}

// Ghep bIdx vao nhom cua a (a tu lap nhom neu chua co). Tra true khi vao duoc.
static bool pb_GhepNhom(int a, int bIdx)
{
	if (a <= 0 || a >= MAX_PLAYER || bIdx <= 0 || bIdx >= MAX_PLAYER || a == bIdx)
		return false;
	// CHOT AN TOAN: ca hai phai la BOT khong client - khong bao gio dong vao nhom
	// cua nguoi that (khuon DT_FormBotTeam, KBotManager.cpp:1758-1760).
	if (!PB_IsBot(a) || !PB_IsBot(bIdx))
		return false;
	if (Player[a].m_nNetConnectIdx != -1 || Player[bIdx].m_nNetConnectIdx != -1)
		return false;

	KPlayerTeam& ta = Player[a].m_cTeam;
	if (ta.m_nFlag == 0)
	{
		ta.SetCanTeamFlag(a, TRUE);
		PLAYER_APPLY_CREATE_TEAM cmd;
		memset(&cmd, 0, sizeof(cmd));
		if (!ta.CreateTeam(a, &cmd))
		{
			pb_Log("[BotNhom] %s LAP NHOM THAT BAI (CreateTeam tu choi)\n",
			       Player[a].m_PlayerName);
			return false;
		}
	}
	if (ta.m_nFlag == 0 || ta.m_nID < 0 || ta.m_nID >= MAX_TEAM
	 || g_Team[ta.m_nID].m_nCaptain != a)
		return false;
	if (g_Team[ta.m_nID].m_nMemNum >= PB_NHOM_MAX_MEM)
		return false;

	// (19/08 toi #2 - don rac log) NHOM DA DAY THEO SUC DOI TRUONG thi DUNG
	// MOI: GetInviteReply se tu choi va sinh mot dong log. Ngay 19/08 co
	// 120.281/244.802 dong log (49%) la "VAO NHOM ... THAT BAI (mem=4/tran=4)"
	// - loi khong phai o ghep nhom ma o cho HOI truoc khi moi.
	if (g_Team[ta.m_nID].m_nMemNum >= g_Team[ta.m_nID].CalcCaptainPower())
		return false;

	// "bam dong y" ho thanh vien: ghi vao danh sach moi roi GetInviteReply(.., 1)
	Player[bIdx].m_cTeam.SetCanTeamFlag(bIdx, TRUE);
	ta.m_nInviteList[ta.m_nListPos] = bIdx;
	ta.m_nListPos = (ta.m_nListPos + 1) % MAX_TEAM_MEMBER;
	ta.GetInviteReply(a, bIdx, 1);
	const bool bOk = (Player[bIdx].m_cTeam.m_nFlag != 0
	               && Player[bIdx].m_cTeam.m_nID == ta.m_nID);
	if (!bOk)
		pb_Log("[BotNhom] %s VAO NHOM %s THAT BAI (GetInviteReply tu choi: mem=%d/tran=%d, mo=%d)\n",
		       Player[bIdx].m_PlayerName, Player[a].m_PlayerName,
		       g_Team[ta.m_nID].m_nMemNum, g_Team[ta.m_nID].CalcCaptainPower(),
		       (int)g_Team[ta.m_nID].IsOpen());
	if (bOk)
		pb_Log("[BotNhom] %s vao nhom cua %s (%d/%d thanh vien)\n",
		       Player[bIdx].m_PlayerName, Player[a].m_PlayerName,
		       g_Team[ta.m_nID].m_nMemNum, PB_NHOM_MAX_MEM);
	return bOk;
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

	// Chong sinh TRUNG (18/08): bam "Goi 1000" hai lan thi tai khoan da co bot song
	// se bi PlayerSet.Add tao them mot nhan vat CUNG TEN - tu choi ngay tu day.
	// (Khoa role o Goddess cung chan duong nap lan hai, nhung hai yeu cau co the
	// cung bay truoc khi khoa kip dat - phai tu giu o day.)
	for (int nTrung = 0; nTrung < s_botCount; nTrung++)
	{
		// (phan bien) strcmpi cho khop cach engine so tai khoan (KPlayerSet.cpp:1053);
		// entry ma dwID da lech (khe bi thao ngoai luong) khong duoc quyen chan.
		const int nIdxTrung = s_bots[nTrung].nPlayerIdx;
		if (nIdxTrung <= 0 || nIdxTrung >= MAX_PLAYER
		 || Player[nIdxTrung].m_dwID != s_bots[nTrung].dwID)
			continue;
		if (strcmpi(s_bots[nTrung].szAccount, p->szAccount) == 0)
		{
			pb_FreePending(p);
			return;
		}
	}

	// pDataBuffer[0] = co thanh cong, pDataBuffer[1..] = blob TRoleData.
	const BYTE* pb = (const BYTE*)pRes->pPayload;
	if (pb[0] != 1)
	{
		// Goddess tra -1 khi role dang bi KHOA (ClientNode.cpp:605 chi doc khi
		// !IsRoleLock) - ghi log de van hanh biet vi sao goi bot ma khong ra.
		pb_Log("[BotLuu] %s: Goddess khong tra du lieu (role dang bi khoa boi phien cu?)\n",
		       p->szRole);
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

	// ---- BOT CU HAY BOT MOI? (18/08 - he luu du lieu bot) ----
	// Blob do CHINH he bot luu mang dau PB_BLOB_DAU o truong chet irevivaly (dong tai
	// CoreServerShell::SavePlayerDataAtOnce). Blob tuoi tu taobot_bdb KHONG co dau:
	// tool chi memcpy mau roi doi ten/he/gioi tinh, nSect ghi 0xFF (taobot_bdb.cpp:598-644).
	// Sua truc tiep tren BAN SAO trong m_SaveBuffer - khong dong vao pPayload goc.
	TRoleData* pSua   = (TRoleData*)Player[nIdx].m_SaveBuffer;
	const int  bBotCu = (pSua->BaseInfo.irevivaly == PB_BLOB_DAU);
	// (phan bien) "bot cu HOP LE" = co dau VA nSect hop le. Neu ai do chay lai
	// taobot_bdb lay MAU la mot bot da luu (blob mang dau nhung nSect bi tool ghi
	// 0xFF) thi phai coi la BOT MOI - van xoa ky nang mau, van boc phai - neu khong
	// se tai hien dung loi "phai Ngu Doc cam dao danh skill dao Thien Vuong".
	const int  nSectBlob = (char)pSua->BaseInfo.nSect;
	const int  bCuCoPhai = (bBotCu && nSectBlob >= 0 && nSectBlob < MAX_FACTION);
	if (bBotCu && pSua->BaseInfo.ifightlevel < PB_CAP_GIU_VITRI)
	{
		// Luat chu game 18/08: DUOI cap 20 goi ra van ve diem hoi sinh map 53 nhu
		// truoc nay; TU cap 20 bot dung nguyen cho da luu (cUseRevive=0 + ientergame*
		// do SavePlayerBaseInfo ghi luc luu, KPlayerDBFuns.cpp:933-935).
		// GetRevivalPosFromId(53,19) chinh la diem lui san co cua LoadPlayerBaseInfo.
		pSua->BaseInfo.cUseRevive = 1;
		pSua->BaseInfo.irevivalid = 53;
		pSua->BaseInfo.irevivalx  = 19;   // LoadPlayerBaseInfo doc irevivalx lam ReviveID
	}

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

	// XOA SACH ky nang THUA KE tu nhan vat mau. Nhan vat mau TestBOT von la nguoi
	// THIEN VUONG - blob mang nguyen bo chieu Thien Vuong, va tu khi vong nap duoc va
	// (18/08) thi bot NAP DUOC bo chieu do that. Hau qua chu game bat duoc: "nhan vat
	// phai Ngu Doc cam dao danh skill dao Thien Vuong" - bo chon chieu thay chieu dao
	// khop dung vu khi la chon, dau biet no cua phai khac.
	// Xoa het ngay tu dau: hockynang() luc vao phai se day DUNG bo chieu cua phai minh.
	// (18/08) CHI xoa voi blob TUOI tu taobot_bdb - bot cu nap lai thi bo ky nang
	// trong blob la bo chieu THAT cua phai no, xoa la mat sach cong luyen.
	// (phan bien) dieu kien la bCuCoPhai chu khong phai bBotCu: dau + nSect hop le.
	if (!bCuCoPhai)
	{
		const int nNpcClean = Player[nIdx].m_nIndex;
		if (nNpcClean > 0 && nNpcClean < MAX_NPC)
			Npc[nNpcClean].m_SkillList.RemoveAllSkill();
	}

	// Bot khong co ket noi mang. PB_LIXIAN_BOT (=3) la thu giu cho bot khoi bi
	// IsLoginTimeOut giet sau 10 giay MA khong dam vao hai gia tri co nghia san (1 va 2) -
	// xem khoi chu thich dai tai KPlayerBot.h.
	Player[nIdx].m_nNetConnectIdx = -1;
	Player[nIdx].m_nLixian        = PB_LIXIAN_BOT;
	// Tran nhom = level_lead_exp.txt theo m_dwLeadLevel; blob mau co the mang 0
	// -> GetMemNumFromLevel tra 1 (KPlayerSet.cpp:885) = nhom sap con 1 nguoi.
	if (Player[nIdx].m_dwLeadLevel < 1)
		Player[nIdx].m_dwLeadLevel = 1;

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

	// (18/08) Bot luon luu VI TRI THAT: m_bUseReviveIdWhenLogin phai la 0 de
	// SavePlayerBaseInfo ghi cUseRevive=0 + toa do dang dung (KPlayerDBFuns.cpp:933-935).
	// Release() da dat 0 nhung dat tuong minh cho chac - bot khong bao gio di qua
	// Lua SetPlayerRevivalOptionWhenLogout nhu nguoi that.
	Player[nIdx].SetLoginType(0);

	// (18/08) KHOA role o Goddess ngay khi bot song. Thieu khoa thi moi goi luu sau
	// nay bi _SaveRoleInfo VUT (doi IsRoleLockBySelf, ClientNode.cpp:431). Nguoi that
	// duoc khoa dung cho nay tren duong vao game (KSOServer.cpp:2864-2869). Khoa se
	// duoc tra khi go bot (luu bLeave=true) hoac khi GameServer ngat khoi Goddess
	// (~CClientNode -> UnlockAllRole).
	if (!s_pfnSend || !s_pfnSend(PB_ASK_LOCKROLE, 0, Player[nIdx].m_PlayerName))
		pb_Log("[BotLuu] %s: gui KHOA role that bai - se tu khoa lai khi goi luu dau"
		       " bao loi ve\n", Player[nIdx].m_PlayerName);

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
	if (bCuCoPhai)
		// (18/08) Bot cu da o trong phai: nSect trong blob la phai THAT da vao (0..9,
		// do gianhapmonphai dat va SavePlayerBaseInfo ghi lai). Blob tuoi taobot_bdb
		// ghi 0xFF -> (char)-1. bCuCoPhai da tinh ngay sau khi doc dau o tren.
		nFaction = nSectBlob;
	else
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
		b.chase.Reset();
		memset(b.aCam, 0, sizeof(b.aCam));
		memset(b.aCamTick, 0, sizeof(b.aCamTick));
		b.nChaseLegTick = 0;
		b.nChatCamToi = 0;
		b.nBuocRaX = 0;   b.nBuocRaY = 0;
		b.nJamX = 0;      b.nJamY = 0;       b.nJamTick = 0;  b.nLachTick = 0;
		b.nUongTick = 0;  b.nMoTuiTick = 0;  b.nHpTruoc = 0;  b.nDameTick = 0;
		b.nLootObjIdx = 0;  b.nLootObjID = 0;
		memset(b.aLootBan, 0, sizeof(b.aLootBan));
		memset(b.aLootBanTick, 0, sizeof(b.aLootBanTick));
		b.loot.Reset();  b.nLootScanTick = 0;  b.nDonTuiTick = 0;
		b.nPhamViTick = 0;  b.nBienLogTick = 0;  b.nLachDem = 0;
		b.szPmTraLoi[0] = 0;  b.nPmSenderIdx = 0;  b.nPmDenHan = 0;
		b.nPmCamToi = 0;  b.nPmLapHash = 0;
		// g_Random den tu engine.lib DUNG SAN (Lib/x64/Engine Server Release) va
		// DONG BANG THEO GIAY: moi lan goi trong cung mot giay tra CUNG gia tri
		// (log 16:05: 30 bot sinh cung giay deu gieo=68; ghi chu KSimCity.cpp:1329
		// da tung cham mat bay nay). Tron chi so bot vao de mot loat sinh van rai
		// deu ma giua cac lan boot van ngau nhien.
		const int nGieoNhom = ((int)g_Random(100) + s_botCount * 37) % 100;
		b.nWantParty = (nGieoNhom < PB_NHOM_RATE) ? 1 : 0;
		b.nDanKiemTick = 0;  b.nDanDenHan = 0;  b.nDanNghiToi = 0;
		// [chan doan 18/08 - GO khi xong] tk bao "muon nhom 0" du 988 con dang danh:
		// in 30 lan boc dau de doi chieu gia tri thuc te cua g_Random tai day.
		if (s_botCount < 30)
			pb_Log("[BotNhom] roll #%d %s: gieo=%d nguong=%d -> muon nhom = %d\n",
			       s_botCount, Player[nIdx].m_PlayerName, nGieoNhom, PB_NHOM_RATE, b.nWantParty);
		b.follow.Reset();
		b.nFollowNghiToi = 0;
		b.nBaiIdx = -1;   b.nBaiLevel = 0;    b.nDoiMapTick = 0;
		b.nChatCuoi = 0;
		b.roam.Reset();   b.nRoamX = 0;  b.nRoamY = 0;  b.nRoamTick = 0;
		b.nAuraSkill = 0; b.nAuraLevel = 0; b.nBuffTick = 0; b.nAuraPhien = 0;
		b.nCuuKiemTick = 0;  b.nCuuTick = 0;  b.nAStarThua = 0;   // khe tai dung mang
		b.nVuKhiTick = 0;    b.nVuKhiThu = 0;                     // rac cu -> phai xoa
		b.nTrangBiLevel = 0;  b.nQhtTick = 0;
		b.nDaTauTick = 0;  b.nDaTauNghi = 0;  b.nDaTauNut = 0;  b.nDaTauDaGhi = 0;
		b.nDaTauChon = 0;  b.nDaTauKe = 0;    b.nDaTauHan = 0;  b.nDaTauThu = 0;
		b.nBanSap = 0;     b.nBanSapXong = 0;  b.nBanSapNgoi = 0;  b.nNguaTick = 0;
		b.nDtPha = 0;  b.nDtToiNpcThu = 0;  b.nDtXaFuThu = 0;  b.nDtTraThu = 0;
		b.nDtTuiThu = 0;  b.nDtCoCu = 0;  b.nDtFarmMoc = 0;
		b.walk.Reset();          // khe co the da dung cho bot truoc do -> phai xoa lo trinh cu
		b.nLuuTick = (unsigned int)g_SubWorldSet.GetGameTime();   // luu dinh ky tinh tu luc sinh
		if (bCuCoPhai)
		{
			// (18/08) Bot cu nap lai: dung ngay cho da luu, KHONG tan ra / KHONG di
			// xin phai lai. nGaveWeapon=1 la BAT BUOC: de 0 thi luot phat vu khi se
			// HUY vu khi dang cam trong do luu (pb_GiveFactionWeapon thao + RemoveItemIdx
			// truoc khi phat cai moi cap 1).
			b.nAi         = PB_AI_IN_FACTION;
			b.nGaveWeapon = 1;
			pb_Log("[BotLuu] %s nap lai bot cu: cap %d phai %d%s\n",
			       Player[nIdx].m_PlayerName, (int)pSua->BaseInfo.ifightlevel, nFaction,
			       (pSua->BaseInfo.ifightlevel < PB_CAP_GIU_VITRI)
			           ? " (duoi cap 20 -> ve map 53)" : " (giu nguyen cho cu)");
		}
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
// (18/08 DOI THIET KE) GO BOT GIO CO LUU DB - chu game yeu cau bot giu du lieu nhu
// nguoi that. Trinh tu soi guong PlayerLogoutGateway (KSOServer.cpp:3549-3582): luu nam
// SAU PrepareRemove (don chat/to doi/giao dich/co bac/PK + WaitForRemove - KHONG dong
// den tui do, m_ItemList chi bi xoa trong RemoveQuiting) va TRUOC RemoveQuiting, nen
// anh chup luu la day du. bLeave=true de Goddess luu xong tu tra khoa role.
static int pb_KillBot(PB_Bot& b)
{
	const int nIdx = b.nPlayerIdx;
	if (nIdx <= 0 || nIdx >= MAX_PLAYER)
		return 0;
	// IsMatch la ham cua KNpc, KPlayer khong co - so thang m_dwID de chac khe chua bi
	// cap lai cho nguoi khac.
	if (Player[nIdx].m_dwID == 0 || Player[nIdx].m_dwID != b.dwID)
		return 0;

	// Roi/giai tan nhom TRUOC khi go - khong de lai khe g_Team mo coi tro vao
	// player index da tai su dung.
	pb_RoiNhom(nIdx, "go bot");

	Player[nIdx].m_nLixian   = 0;      // mo khoa 1
	Player[nIdx].m_bForeQuit = TRUE;   // mo khoa 2

	PlayerSet.PrepareRemove(nIdx);     // don sach + WaitForRemove()

	// (18/08) LUU roi moi thao khe - xem khoi chu thich dau ham. Luu that bai
	// (duong DB dut, Save() tra 0...) van phai TRA KHOA de phien sau nap lai duoc.
	if (s_pfnSend)
	{
		// (phan bien) playerlogout.lua / script OnLogout vua chay trong PrepareRemove
		// co the goi SetLogoutRV(1) lat co hoi-sinh-khi-vao - ep lai 0 de bot >= cap 20
		// LUON luu vi tri that (KPlayerDBFuns.cpp:934 doc co nay).
		Player[nIdx].SetLoginType(0);
		if (!s_pfnSend(PB_ASK_SAVEROLE_LEAVE, (unsigned long)nIdx, Player[nIdx].m_PlayerName))
			s_pfnSend(PB_ASK_UNLOCKROLE, 0, Player[nIdx].m_PlayerName);
	}

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
// ---- (18/08) luu du lieu bot dinh ky - NHIP NHU NGUOI CHOI ----
// Nguoi that duoc AutoSave cua engine luu 30s/lan, nhung CanSave chan netidx==-1 nen
// bot phai tu quay vong trong PB_Breathe. Nhip 30s/bot (yeu cau chu game "luu tu dong
// nhu nguoi choi"). 1000 bot / 30s = ~33 goi/giay - DA KIEM AN TOAN bang ma:
//   - BDB mo voi DB_TXN_NOSYNC (Goddess DBTable.cpp:33) -> put khong fsync, ~us moi goi;
//   - GUI Goddess AddOutputString tu cat tran 100 dong ListBox (IDBRoleServer.cpp:731);
//   - bo dieu tiet duoi day tran 2 goi/nhip = 36 goi/giay, du 33 va khong the boc dau;
//   - phi con lai: file log Goddess phinh ~170MB/ngay khi du 1000 bot (chap nhan duoc,
//     muon thi xoay vong file log - ghi trong so tay van hanh).
#define PB_LUU_MOI_GIAY  30
static int          s_nLuuCon      = 0;   // con tro vong quet luu dinh ky
static int          s_nPerfLuuDem  = 0;   // so goi luu da gui (in kem [BotPerf] moi 10s)
static int          s_bLuuTatCa    = 0;   // 1 = dang don luu ep toan bo (PB_SaveAll)
static unsigned int s_nLuuTatCaMoc = 0;   // GetGameTime() luc bam PB_SaveAll

#define PB_KILL_PER_TICK  5    // 1000 bot -> ~11 giay. (18/08 phan bien) ha tu 20:
                               // moi con go la MOT goi luu day du day vao socket Goddess
                               // DONG BO (WSASend) - 20 con/nhip x ~15-69KB de nghen khung

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
	// (18/08 phan bien) huy dot SaveAll dang do: tung con se duoc pb_KillBot luu
	// bLeave=true roi, giu bo dem lai chi lam dot bot SAU bi luu ep oan + in XONG sai.
	s_bLuuTatCa = 0;
	s_nLuuCon   = 0;
	pb_Log("[Bot] xep hang go %d bot (rut dan %d con moi nhip, moi con LUU truoc khi go)\n",
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
// CHE DO BEN VUNG (18/08 toi): 1000 bot sinh DAN ~30 giay + vao phai mat vai phut,
// ma hai lenh menu von la ONE-SHOT quet danh sach TAI THOI DIEM BAM -> bot sinh/
// vao phai SAU khi bam lenh dung i vinh vien ([BotNhom] tk 14:50 "0 tu do muon
// nhom" mot phan vi vay). Gio lenh admin la CHE DO: co bat len thi bot den muon
// cung tu ap trong pb_DriveBot.
static int s_nPbCheDoNhapMon = 0;
static int s_nPbCheDoDanh    = 0;
// (19/08 trua) chu game: "add TTL + Que Hoa Tuu la ham trong lenh bai, muon add
// khi nao thi add" -> mac dinh TAT, bat/tat qua PB_SetBuff (menu lenh bai).
static int s_nPbChamBuff     = 0;
// So bot toi da duoc di lam nhiem vu Da Tau (0 = tat; bot nLech < nguong thi lam).
static int s_nPbDaTauMax     = 0;

int PB_JoinFaction()
{
	s_nPbCheDoNhapMon = 1;
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
	s_nPbCheDoDanh = bOn ? 1 : 0;
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

// (18/08) Xep hang luu NGAY toan bo bot (bam truoc khi tat server). Khong gui mot
// luot 1000 goi ~ chuc MB lam nghen duong sang Goddess - PB_Breathe don 5 goi/nhip,
// 1000 bot het ~11 giay; xong in "[BotLuu] da gui luu XONG toan bo bot".
int PB_SaveAll()
{
	if (s_botCount <= 0)
		return 0;
	if (s_bKillAll)
		return 0;                      // dang go het - tung con tu luu khi go roi
	s_bLuuTatCa    = 1;
	s_nLuuTatCaMoc = (unsigned int)g_SubWorldSet.GetGameTime();
	pb_Log("[BotLuu] xep hang luu %d bot (5 goi/nhip ~%d giay; bot dang chet se hoi"
	       " sinh ~3 giay roi luu not)\n",
	       s_botCount, s_botCount / (5 * GAME_FPS) + 1);
	return s_botCount;
}

int LuaPB_SaveAll(Lua_State* L)
{
	Lua_PushNumber(L, PB_SaveAll());
	return 1;
}

// (18/08 phan bien) Goddess tra -1 cho mot goi luu (thuong do role KHONG con bi khoa
// boi node nay - vd Goddess restart lam mat bang khoa RAM). GameServer bao ve day:
// neu khe la bot thi ghi log; khoa moi da duoc GameServer gui lai ngay khi thay bao
// loi, nhip luu 30 giay ke tiep tu luu bu. Tra 1 = dung la bot.
int PB_OnSaveFailed(int nPlayerIdx)
{
	for (int i = 0; i < s_botCount; i++)
	{
		PB_Bot& b = s_bots[i];
		if (b.nPlayerIdx != nPlayerIdx)
			continue;
		if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER
		 || Player[nPlayerIdx].m_dwID != b.dwID)
			return 0;                      // khe da doi chu (vd bot vua bi go) - bo qua
		// Khong dung den nLuuTick: moc vua duoc stamp luc GUI nen nhip 30 giay ke
		// tiep tu luu lai - luc do khoa moi (GameServer vua gui lai) da nam o Goddess.
		// (Truoc day tru "PB_LUU_MOI_GIAY - 60" - doi nhip xuong 30s la ra so am,
		// unsigned wrap lam den han NGAY LAP TUC -> spam luu; bo han cho lanh.)
		pb_Log("[BotLuu] %s: Goddess TU CHOI luu (mat khoa role?) - da xin khoa lai,"
		       " nhip 30 giay ke tiep se luu bu\n", Player[nPlayerIdx].m_PlayerName);
		return 1;
	}
	return 0;
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

// (19/08 trua) Bat/tat cham Tien Thao Lo + Que Hoa Tuu cho ca dan bot (lenh bai).
int PB_SetBuff(int bOn)
{
	s_nPbChamBuff = bOn ? 1 : 0;
	if (bOn)
		for (int i = 0; i < s_botCount; i++)
			s_bots[i].nQhtTick = 0;        // ep cham lai ngay o nhip 2 giay ke tiep
	pb_Log("[BotBuff] cham TTL + Que Hoa Tuu: %s (%d bot)\n",
	       bOn ? "BAT" : "TAT", s_botCount);
	return s_botCount;
}

int LuaPB_SetBuff(Lua_State* L)
{
	int bOn = (Lua_GetTopIndex(L) >= 1) ? (int)Lua_ValueToNumber(L, 1) : 1;
	Lua_PushNumber(L, PB_SetBuff(bOn));
	return 1;
}

// (19/08 chieu) Dat so bot lam Da Tau - boc NGAU NHIEN trong ca dan (chu game:
// "phai random trong 1000 bot chu khong theo thu tu"). nMax < 0 = chi doc.
int PB_SetDaTau(int nMax)
{
	if (nMax < 0)
	{
		int n = 0;
		for (int i = 0; i < s_botCount; i++)
			if (s_bots[i].nDaTauChon)
				n++;
		return n;
	}
	if (nMax > PB_MAX_BOTS)
		nMax = PB_MAX_BOTS;
	s_nPbDaTauMax = nMax;

	// (19/08 toi #2) HAI SUA QUAN TRONG, ca hai deu tu log that ngay 19/08:
	// 1. CHI LAY BOT CAP >= 80 (luat chu game). Truoc day boc tren CA DAN nen
	//    boc 200 con chi trung ~14 con du cap (log 16:30: 68/1000 bot >= 80,
	//    16 con "nhap mon" that) - nhin nhu "bot khong chiu di lam nhiem vu".
	// 2. GIU NGUYEN bot DANG LAM: bam lai menu (chu game bam 16:30:55 roi
	//    16:31:40) truoc day BOC LAI TU DAU -> bot vua teleport sang map nhiem
	//    vu bi bo chon -> pb_RaBai keo thang ve bai luyen (CaoVinh338: teleport
	//    map 122 luc 16:30:57, bi bo chon 16:31:40, ve bai map 198 ngay giay do).
	//    Nay chi THEM cho du so hoac BOT phan thua - bot dang lam khong bi dung.
	static int aId[PB_MAX_BOTS];
	int nSong = 0, nDangChon = 0, nDuCap = 0;
	for (int i = 0; i < s_botCount; i++)
	{
		const int p = s_bots[i].nPlayerIdx;
		if (p <= 0 || p >= MAX_PLAYER)
			continue;
		const int nn = Player[p].m_nIndex;
		const int nCap = (nn > 0 && nn < MAX_NPC) ? Npc[nn].m_Level : 0;
		const int nCourseI = (int)Player[p].m_cTask.GetSaveVal(PB_DT_COURSE);
		if (nCap < PB_DT_CAP_TOI_THIEU)
		{
			// (D3) chi cat co khi bot KHONG dang giua nhiem vu - doc cap loi /
			// tut cap giua chung khong duoc bo roi bot o map nhiem vu
			if (nCourseI != 1)
				s_bots[i].nDaTauChon = 0;
			continue;
		}
		nDuCap++;
		if (s_bots[i].nBanSap)
		{
			if (nCourseI != 1)         // (D4) dang giua nhiem vu thi de yen
				s_bots[i].nDaTauChon = 0;
			continue;
		}
		if (s_bots[i].nDaTauChon)
			nDangChon++;
		else
			aId[nSong++] = i;          // ung vien co the them
	}

	if (nDangChon > nMax)
	{
		// bot bot: nha nhung con CHUA vao viec truoc (course 0 = chua nhap mon)
		for (int i = s_botCount - 1; i >= 0 && nDangChon > nMax; i--)
		{
			if (!s_bots[i].nDaTauChon)
				continue;
			const int p = s_bots[i].nPlayerIdx;
			if (p > 0 && p < MAX_PLAYER
			 && (int)Player[p].m_cTask.GetSaveVal(PB_DT_COURSE) == 1)
				continue;              // dang giua nhiem vu - de yen
			s_bots[i].nDaTauChon = 0;
			nDangChon--;
		}
		// (D5) vong hai: VAN chua bo lai bot dang giua nhiem vu - tha du so
		// con hon cat ngang mot nhiem vu dang lam (bot se tu nha khi tra xong).
		for (int i = s_botCount - 1; i >= 0 && nDangChon > nMax; i--)
		{
			if (!s_bots[i].nDaTauChon)
				continue;
			const int p2 = s_bots[i].nPlayerIdx;
			if (p2 > 0 && p2 < MAX_PLAYER
			 && (int)Player[p2].m_cTask.GetSaveVal(PB_DT_COURSE) == 1)
				continue;
			s_bots[i].nDaTauChon = 0;
			nDangChon--;
		}
	}
	else if (nDangChon < nMax && nSong > 0)
	{
		// Fisher-Yates tren ung vien du cap (tron chi so - g_Random dong bang giay)
		for (int i = nSong - 1; i > 0; i--)
		{
			const int j = ((int)g_Random(i + 1) + i * 17) % (i + 1);
			const int t2 = aId[i];  aId[i] = aId[j];  aId[j] = t2;
		}
		for (int i = 0; i < nSong && nDangChon < nMax; i++)
		{
			s_bots[aId[i]].nDaTauChon = 1;
			nDangChon++;
		}
	}
	pb_Log("[BotDT] chon %d bot lam Da Tau (yeu cau %d; %d/%d bot du cap %d)\n",
	       nDangChon, nMax, nDuCap, s_botCount, PB_DT_CAP_TOI_THIEU);
	return nDangChon;
}

int LuaPB_SetDaTau(Lua_State* L)
{
	int nMax = (Lua_GetTopIndex(L) >= 1) ? (int)Lua_ValueToNumber(L, 1) : -1;
	Lua_PushNumber(L, PB_SetDaTau(nMax));
	return 1;
}

// (19/08 chieu) Dat so bot ra thanh ngoi ban sap - boc NGAU NHIEN. nMax < 0 = doc.
int PB_SetBanSap(int nMax)
{
	if (nMax < 0)
	{
		int n = 0;
		for (int i = 0; i < s_botCount; i++)
			if (s_bots[i].nBanSap)
				n++;
		return n;
	}
	if (nMax > PB_MAX_BOTS)
		nMax = PB_MAX_BOTS;
	// dong sap cu (bot bi bo chon tu ve bai luyen nho pb_RaBai)
	for (int i = 0; i < s_botCount; i++)
	{
		if (!s_bots[i].nBanSap)
			continue;
		s_bots[i].nBanSap = 0;
		s_bots[i].nBanSapXong = 0;
		const int p = s_bots[i].nPlayerIdx;
		const int nn = (p > 0 && p < MAX_PLAYER) ? Player[p].m_nIndex : 0;
		if (nn > 0 && nn < MAX_NPC)
			Npc[nn].m_BaiTan = 0;
	}
	static int aId[PB_MAX_BOTS];
	int nSong = 0;
	for (int i = 0; i < s_botCount; i++)
	{
		if (s_bots[i].nPlayerIdx <= 0)
			continue;
		if (s_bots[i].nDaTauChon)
			continue;                  // dang lam Da Tau thi khoi ban sap
		aId[nSong++] = i;
	}
	for (int i = nSong - 1; i > 0; i--)
	{
		const int j = ((int)g_Random(i + 1) + i * 29) % (i + 1);
		const int t2 = aId[i];  aId[i] = aId[j];  aId[j] = t2;
	}
	int nChon = 0;
	for (int i = 0; i < nSong && nChon < nMax; i++)
	{
		s_bots[aId[i]].nBanSap = 1;
		nChon++;
	}
	pb_Log("[BotSap] boc NGAU NHIEN %d bot (trong %d con song) ra thanh ban sap\n",
	       nChon, nSong);
	return nChon;
}

int LuaPB_SetBanSap(Lua_State* L)
{
	int nMax = (Lua_GetTopIndex(L) >= 1) ? (int)Lua_ValueToNumber(L, 1) : -1;
	Lua_PushNumber(L, PB_SetBanSap(nMax));
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

	// (19/08 ep bot trong map) DICH NGOAI MAP -> bao thua NGAY. Truoc day
	// FindPathServer KEP dich ngoai luoi ve o mep roi tra "duong tron ven"
	// (KSubWorld.cpp:3296-3301), PB_WalkTo tuong dich hop le va chang cuoi
	// do_run thang vao toa do THO ngoai map - bot ep sat mep nhu lao ra ngoai.
	// Moi caller deu co san nhanh xu ly -1 (boc diem khac / cam muc tieu / bo qua).
	{
		int nRB = -1, nMXB = 0, nMYB = 0, nDXB = 0, nDYB = 0;
		SubWorld[nSubIdx].Mps2Map(nDstMpsX, nDstMpsY, &nRB, &nMXB, &nMYB, &nDXB, &nDYB);
		if (nRB < 0)
			return -1;
	}

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

			// DANH THUC neu lenh dang bi NUOT: ProcCommand chi thi hanh khi m_ProcessAI != 0.
			// Khi no = 0, lenh bi an mat va CHI duoc danh thuc neu m_Doing == do_stand
			// (KNpc.cpp:929-933). Bot ket o trang thai KHAC do_stand voi ProcessAI = 0 la
			// DONG BANG VINH VIEN - moi do_run ta phat deu bi nuot khong dau vet.
			// Danh thuc y het khuon do_revive cua chinh engine (KNpc.cpp:889-893).
			if (!Npc[nNpcIdx].IsCanInput())   // IsCanInput() = doc m_ProcessAI (KNpc.h:826)
			{
				{
					static unsigned int s_uKetThucLog = 0;
					if (now - s_uKetThucLog >= (unsigned int)(GAME_FPS / 2))
					{
						s_uKetThucLog = now;
						pb_Log("[BotKet] npc=%d lenh bi nuot (doing=%d, procAI=0) -> danh thuc\n",
						       nNpcIdx, (int)Npc[nNpcIdx].m_Doing);
					}
				}
				// m_ProcessAI / DoStand deu PRIVATE - di cong cong khai: SetProcessAI
				// (KNpc.h:676, inline public) bat AI day TRUOC, roi SendCommand(do_stand)
				// de ProcCommand (luc nay nAI = 1) thi hanh DoStand that.
				Npc[nNpcIdx].SetProcessAI(TRUE);
				Npc[nNpcIdx].SendCommand(do_stand);
			}
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
			// In MA LOI de bot.log ke duoc vi sao: -2 luoi chua nap, -3 dich ket vat can,
			// -4 xuat phat ket vat can, -1 dau vao sai, 0 A* chiu thua. Gion 3 dong/giay.
			static unsigned int s_uAstarLog = 0;
			const unsigned int uNow2 = SubWorld[nSubIdx].m_dwCurrentTime;
			if (uNow2 - s_uAstarLog >= (unsigned int)(GAME_FPS / 3))
			{
				s_uAstarLog = uNow2;
				pb_Log("[BotA*] npc=%d map=%d tu o(%d,%d) den o(%d,%d) LOI ret=%d\n",
				       nNpcIdx, SubWorld[nSubIdx].m_SubWorldID, bx / 32, by / 32,
				       nDstMpsX / 32, nDstMpsY / 32, nRet);
			}
			st.path.clear();
			return -1;
		}
		// path[0] LUON la block bot DANG DUNG (FindPath_Block lan nguoc cameFrom tu dich ve
		// xuat phat). Block to toi 16x32 o nen TAM cua no co the cach bot ca tram MPS -
		// nham vao do la buoc GIAT LUI dau tien, dung trieu chung "tien roi giat lui" ma
		// ban tham khao canh bao (KBotCombat.cpp:552-556). Bo qua no ngay tu dau.
		st.nKieuDuong = nRet;    // 1 = duong tron ven, 2 = duong CUT (dich khong toi duoc)
		st.idx = 1;
	}

	// ---- B2: tien con tro waypoint ----
	// Bo qua waypoint bot DA di qua: cach duoi 64 MPS (2 o) thi coi nhu xong.
	bool bAdvanced = false;
	while (st.idx < (int)st.path.size())
	{
		// Nham DIEM GAN NHAT trong block chu KHONG phai tam: block to toi 16x32 o nen
		// tam co the nam NGUOC huong di -> bot "ra khoi trap roi chay lui vao trong",
		// dung nhu chu game quan sat. Diem gan nhat cua block KE thi luon o phia truoc.
		int wx = 0, wy = 0;
		if (!SubWorld[nSubIdx].BlockNearestMps(st.path[st.idx], bx, by, wx, wy))
		{
			st.idx++; bAdvanced = true; continue;
		}
		__int64 dx = (__int64)bx - wx;
		__int64 dy = (__int64)by - wy;
		// da dam vao block (diem gan nhat ngay canh minh) -> qua waypoint ke
		if (dx * dx + dy * dy <= (__int64)48 * 48)
		{
			st.idx++; bAdvanced = true; continue;
		}
		break;
	}

	if (bAdvanced)
	{
		st.noAdvanceCalls = 0;
	}
	// (19/08) dem ca CHANG CUOI (idx == size) - nhung o chang cuoi CHI dem khi bot
	// KHONG dich chuyen that (neo lastMoveTick cua B5a lam moi moi khi nhuc nhich
	// >32 MPS): block gop co the rong toi 512x1024 MPS, dang chay ngon 2 giay chua
	// toi noi ma bao thua la CAM OAN quai / chen nghi follow gia (phan bien chot).
	else if (!st.path.empty()
	      && (st.idx < (int)st.path.size() || now - st.lastMoveTick >= (unsigned int)GAME_FPS)
	      && ++st.noAdvanceCalls >= GAME_FPS * 2)
	{
		// ---- B5b: ~2 giay khong tien duoc waypoint nao ----
		st.noAdvanceCalls = 0;
		if (st.repathCount < 3)
		{
			st.repathCount++;
			// PHAI hung ma tra ve (phan bien 18/08): tra <= 0 la path da bi xoa trang
			// (FindPathServer clear ngay khi vao) - truoc day lo di, nhanh "het waypoint"
			// duoi kia lien do_run THANG vao dich tho khong qua A* = xuyen bien.
			const int nRet2 = SubWorld[nSubIdx].FindPathServer(bx, by, nDstMpsX, nDstMpsY, st.path, true);
			if (nRet2 <= 0 || st.path.empty())
			{
				st.Reset();
				return -1;
			}
			st.nKieuDuong = nRet2;
			st.idx = 1;   // bo block xuat phat, nhu tren
		}
		else
		{
			// Thu 3 lan van khong nhuc nhich -> bo cuoc. In TRANG THAI SONG cua than bot
			// de bot.log ke duoc vi sao (dang lam gi, AI thuc hay ngu, the luc con khong).
			// 1000 bot chen nhau tung phun 6.880 dong/2 phut - moi dong mot fopen
			// bot.log tren luong game. Gion 2 dong/giay TOAN CUC, hanh vi giu nguyen.
			{
				static unsigned int s_uKetBoLog = 0;
				const unsigned int uNow4 = SubWorld[nSubIdx].m_dwCurrentTime;
				if (uNow4 - s_uKetBoLog >= (unsigned int)(GAME_FPS / 2))
				{
					s_uKetBoLog = uNow4;
					pb_Log("[BotKet] npc=%d bo cuoc sau 3 lan: doing=%d procAI=%d theluc=%d/%d"
					       " toc(di/chay)=%d/%d o(%d,%d)\n",
					       nNpcIdx, (int)Npc[nNpcIdx].m_Doing, (int)Npc[nNpcIdx].IsCanInput(),
					       (int)Npc[nNpcIdx].m_CurrentStamina, (int)Npc[nNpcIdx].m_CurrentStaminaMax,
					       (int)Npc[nNpcIdx].m_CurrentWalkSpeed, (int)Npc[nNpcIdx].m_CurrentRunSpeed,
					       bx / 32, by / 32);
				}
			}
			st.Reset();
			return -1;
		}
	}

	// DUONG CUT (FindPathServer tra 2 = dich KHONG toi duoc, A* chi dan toi block
	// gan dich nhat): di het duong ma B0 chua bao toi noi -> BAO THUA cho noi goi
	// xu ly (cam muc tieu / boc diem khac). Truoc day nhanh duoi day do_run THANG
	// vao dich tho MOI NHIP, vinh vien - duong xuyen bien con sot (phan bien 18/08).
	if (st.nKieuDuong == 2 && st.idx >= (int)st.path.size())
	{
		st.Reset();
		return -1;
	}

	// Het waypoint (duong TRON VEN) ma chua toi dich: chang cuoi KHONG nham thang
	// vao dich tho nua. (19/08 ep bot trong map) FindPathServer co the da THAY THE
	// dich (ket vat can -> FindFreeBlockAround; vung rong thieu du lieu -> block
	// hop le gan nhat) ma van tra 1 - "kiem khong vat can" chi dung cho BLOCK CUOI,
	// khong phai toa do tho. Kep dich vao block cuoi (da xac nhan di duoc): dich
	// nam trong block thi giu nguyen nhu cu; dich bi thay the thi dung o diem hop
	// le gan dich nhat, va neu da toi sat diem kep ma van chua "toi noi" theo B0
	// thi bao thua de caller boc diem khac - khong ep mep, khong lao ra ngoai.
	int tx = nDstMpsX, ty = nDstMpsY;
	if (st.idx < (int)st.path.size())
	{
		int wx = 0, wy = 0;
		if (SubWorld[nSubIdx].BlockNearestMps(st.path[st.idx], bx, by, wx, wy) && wx > 0 && wy > 0)
		{
			tx = wx; ty = wy;
		}
	}
	else if (!st.path.empty())
	{
		int wx = 0, wy = 0;
		if (SubWorld[nSubIdx].BlockNearestMps(st.path[(int)st.path.size() - 1],
		                                      nDstMpsX, nDstMpsY, wx, wy)
		 && wx > 0 && wy > 0)
		{
			tx = wx; ty = wy;
			// (19/08 phan bien) chi coi la "dich bi thay the" khi diem kep KHAC dich
			// tho - dich nam trong block cuoi thi kep tra ve chinh no, di tiep binh
			// thuong (khong phu thuoc gia dinh nArriveMps > 48 nua).
			if (wx != nDstMpsX || wy != nDstMpsY)
			{
				const __int64 dxK = (__int64)bx - tx;
				const __int64 dyK = (__int64)by - ty;
				if (dxK * dxK + dyK * dyK <= (__int64)48 * 48)
				{
					st.Reset();
					return -1;
				}
			}
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
	// Ghi lai o luoi tui ma InsertEquipment vua chiem: Equip chi doi nPlace sang
	// pos_equip chu KHONG nha o luoi (KItemList.cpp:1185 - duong client that equip
	// tu pos_hand nen engine chua bao gio lo). Khong nha thi vu khi 1x3/1x4 chiem
	// chet 3-4 o tui suot doi bot (phan bien 18/08).
	int nWpnRoomX = -1, nWpnRoomY = -1;
	{
		const int q2 = Player[nIdx].m_ItemList.FindSame(nNew);
		if (q2 > 0 && Player[nIdx].m_ItemList.m_Items[q2].nPlace == pos_equiproom)
		{
			nWpnRoomX = Player[nIdx].m_ItemList.m_Items[q2].nX;
			nWpnRoomY = Player[nIdx].m_ItemList.m_Items[q2].nY;
		}
	}
	// Equip o cay nay chi co HAI tham so (KItemList.h:104); ban tham khao co tham so thu ba
	// bUpdateSkin - dung chep nguyen chu ky sang.
	Player[nIdx].m_ItemList.Equip(nNew, -1);
	if (nWpnRoomX >= 0
	 && Player[nIdx].m_ItemList.GetEquipment(itempart_weapon) == nNew)
		Player[nIdx].m_ItemList.m_Room[room_equipment].PickUpItem(
			nNew, nWpnRoomX, nWpnRoomY, Item[nNew].GetWidth(), Item[nNew].GetHeight());

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
// TRANG BI THEO CAP (chu game 19/08 chieu): tieu not tiem nang ton dong, mac bo
// Hoang Kim "Kim Phong", cuoi ngua Tuc Suong cap 10, dat 81 doi vu khi cap 10.
// Duong tao do la duong that cua du an: ItemSet.AddGoldItem (y het LuaAddGoldItem,
// ScriptFuns.cpp:4081) va ItemSet.Add (khuon pb_GiveFactionWeapon). Moi mon tao
// xong PHAI qua CanEquip (KItemList.cpp:928 - kiem yeu cau cap/chi so nhu client
// that lam truoc khi gui goi mac do; Equip phia server KHONG tu kiem) - chua du
// dieu kien thi HUY mon do, lan LEN CAP sau thu lai => "di cap mac tung mon".
// ===========================================================================

// Chen vao tui -> kiem CanEquip -> mac -> nha o luoi tui ma Equip chiem chet
// (meo PickUpItem giong pb_GiveFactionWeapon; KItemList.cpp:1185 khong tu nha).
// nPlace = -1 de Equip tu chon cho theo loai; rieng NHAN THU HAI phai chi dinh
// itempart_ring2 vi GetEquipPlace(equip_ring) LUON tra ring1 (KItemList.cpp:1394).
// Tra 1 = da mac len nguoi; 0 = khong du dieu kien / tui day (mon do DA huy sach).
static int pb_MacVaoNguoi(int nIdx, int nNew, int nPlace)
{
	if (nNew <= 0)
		return 0;
	if (!Player[nIdx].m_ItemList.CanEquip(nNew, -1))
	{
		ItemSet.Remove(nNew);              // chua chen vao tui - huy thang o kho item
		return 0;
	}
	Item[nNew].LockItem(LOCK_STATE_LOCK);  // khoa nhu vu khi nhap mon - khong lam roi duoc
	Player[nIdx].m_ItemList.InsertEquipment(nNew, false);
	const int q2 = Player[nIdx].m_ItemList.FindSame(nNew);
	if (q2 <= 0)
	{
		ItemSet.Remove(nNew);              // tui day (InsertEquipment void - kiem lai sau)
		return 0;
	}
	int nRoomX = -1, nRoomY = -1;
	if (Player[nIdx].m_ItemList.m_Items[q2].nPlace == pos_equiproom)
	{
		nRoomX = Player[nIdx].m_ItemList.m_Items[q2].nX;
		nRoomY = Player[nIdx].m_ItemList.m_Items[q2].nY;
	}
	if (!Player[nIdx].m_ItemList.Equip(nNew, nPlace))
	{
		Player[nIdx].m_ItemList.RemoveItemIdx(nNew, Item[nNew].GetStackNum());
		return 0;
	}
	if (nRoomX >= 0)
		Player[nIdx].m_ItemList.m_Room[room_equipment].PickUpItem(
			nNew, nRoomX, nRoomY, Item[nNew].GetWidth(), Item[nNew].GetHeight());
	return 1;
}

// 9 mon Kim Phong = goldequip.txt dong 178-186; id AddGoldItem = DONG - 1 (kiem
// chung: trangbihoangkim.lua:31 "AddGoldItem(2083-2)--dao" -> id 2081 = dong 2082
// Kim O Bon Nhuoc GIOI DAO; NPC tan thu hotrotanthu.lua:86 phat dung bo nay bang
// for i=177,185. Chu game noi "176-184" la lech 1 theo cach dem 0.
// CA 9 mon mang nGoldId = 36 (cot m_nId) -> nhan biet "da mac" = GetGoldId()==36.
static const int s_nKpId[9]  = { 177, 178, 179, 180, 181, 182, 183, 184, 185 };
static const int s_nKpCho[9] = { itempart_head, itempart_body, itempart_amulet,
                                 itempart_ring1, itempart_belt, itempart_cuff,
                                 itempart_pendant, itempart_foot, itempart_ring2 };
#define PB_KP_GOLDID  36

static void pb_TrangBiTheoCap(int nIdx, int nNpcIdx, PB_Bot& b)
{
	// ---- 1. tieu not tiem nang ton dong ----
	// LOI CU: pb_AllocAttribPoints chi duoc goi MOT LAN luc vao phai; chu thich cu
	// noi "nhanh IN_FACTION ben tren se tieu tiep" nhung nhanh do KHONG TON TAI ->
	// diem don dong khong tieu, sinh khi khong tang => bot cap 82 max HP 440
	// (bot.log 09:5x) bi quai bai 80 giet lien tuc. Goi lai moi lan len cap la het.
	if (Player[nIdx].m_nAttributePoint > 0)
		pb_AllocAttribPoints(nIdx, b.nFaction);

	const int nLevel = Npc[nNpcIdx].m_Level;

	// ---- 2. bo Kim Phong: mon nao chua mac ma du dieu kien thi mac ----
	for (int k = 0; k < 9; k++)
	{
		const int nDangMac = Player[nIdx].m_ItemList.GetEquipment(s_nKpCho[k]);
		if (nDangMac > 0 && Item[nDangMac].GetGoldId() == PB_KP_GOLDID)
			continue;              // mon nay xong tu truoc (ke ca qua restart)
		const int nNew = ItemSet.AddGoldItem(s_nKpId[k], NULL, Npc[nNpcIdx].m_Series,
		                                     0, 0, 0, 0, 0, 0, 0, 0);
		if (nNew <= 0)
			continue;
		if (pb_MacVaoNguoi(nIdx, nNew,
		                   (s_nKpCho[k] == itempart_ring2) ? itempart_ring2 : -1))
			pb_Log("[BotTrangBi] %s cap %d mac Kim Phong mon %d/9 (goldequip id %d)\n",
			       Player[nIdx].m_PlayerName, nLevel, k + 1, s_nKpId[k]);
	}

	// ---- 3. ngua Tuc Suong cap 10 (horse.txt dong 30-31: detail 10 = equip_horse,
	//         particular 2; cot cap co ban 9 va 10 - chu game dan lay cap 10) ----
	{
		const int nNgua = Player[nIdx].m_ItemList.GetEquipment(itempart_horse);
		const bool bDaCo = (nNgua > 0 && Item[nNgua].GetDetailType() == equip_horse
		                 && Item[nNgua].GetParticular() == 2
		                 && Item[nNgua].GetLevel() >= 10);
		if (!bDaCo)
		{
			int nMagic[MAX_ITEM_MAGICLEVEL];
			ZeroMemory(nMagic, sizeof(nMagic));
			const int nNew = ItemSet.Add(0, 0, Npc[nNpcIdx].m_Series, 10, 0,
			                             equip_horse, 2, nMagic,
			                             g_SubWorldSet.GetGameVersion(), 0);
			if (nNew > 0 && pb_MacVaoNguoi(nIdx, nNew, -1))
				pb_Log("[BotTrangBi] %s cap %d cuoi ngua Tuc Suong cap 10\n",
				       Player[nIdx].m_PlayerName, nLevel);
		}
	}

	// ---- 4. dat cap 81: doi vu khi len CAP 10 cung loai dang cam ----
	// (giu nguyen detail/particular/he - chi nang bac item nhu chu game dan;
	// khong dong vao vu khi Hoang Kim neu sau nay bot duoc phat)
	if (nLevel >= 81)
	{
		const int nW = Player[nIdx].m_ItemList.GetEquipment(itempart_weapon);
		if (nW > 0 && Item[nW].GetGoldId() == 0 && Item[nW].GetLevel() < 10)
		{
			int nMagic[MAX_ITEM_MAGICLEVEL];
			ZeroMemory(nMagic, sizeof(nMagic));
			const int nNew = ItemSet.Add(0, 0, Item[nW].GetSeries(), 10, 0,
			                             Item[nW].GetDetailType(),
			                             Item[nW].GetParticular(), nMagic,
			                             g_SubWorldSet.GetGameVersion(), 0);
			// kiem DU DIEU KIEN truoc roi moi huy vu khi cu - nguoc thu tu la co
			// cua so bot tay khong (neu van hong o buoc mac, [BotVuKhi] 10s vot lai)
			if (nNew > 0 && !Player[nIdx].m_ItemList.CanEquip(nNew, -1))
				ItemSet.Remove(nNew);
			else if (nNew > 0)
			{
				Player[nIdx].m_ItemList.RemoveItemIdx(nW, Item[nW].GetStackNum());
				if (pb_MacVaoNguoi(nIdx, nNew, -1))
				{
					b.nAtkSkill = 0;   // chon lai chieu theo vu khi (cung loai - cho chac)
					pb_Log("[BotTrangBi] %s cap %d len vu khi cap 10 (detail %d parti %d)\n",
					       Player[nIdx].m_PlayerName, nLevel,
					       Item[nNew].GetDetailType(), Item[nNew].GetParticular());
				}
			}
		}
	}
}

// ===========================================================================
// BOT TU CHAY NHIEM VU DA TAU (chu game 19/08 trua)
//
// Bot trong "gioi han Da Tau" (lenh bai dat qua PB_SetDaTau; bot co nLech <
// nguong thi lam) tu choi vong lap tasklink nhu nguoi that: dung canh NPC Da Tau
// -> nhan -> lam -> tra -> chon ruong thuong -> lap; du 40/ngay tu ve bai luyen.
//
// Cach lam (AUTO_DATAU_SPEC.md + doc tung nhanh seasonnpc.lua):
// - Trang thai doc tu TASK VALUE: 1028 course, 1021 loai, 1031 map loai-4,
//   1032 byte2 = so cuon can, 1025 = da co, 2420 = so nhiem vu hom nay (tran 40).
// - Doi thoai KHONG di duong goi tin: goi thang ham dap an cua seasonnpc.lua qua
//   ExecuteScript (SCRIPT_PLAYERINDEX tu tro ve bot - y het duong vao phai "go").
//   course 0 -> Task_Confirm; 3 -> Task_TaskProcess (script TU kiem tran 40/ngay
//   + tu reset sang ngay moi); 2 -> thu lan luot 6 nut ruong (nut khong thuoc bo
//   3 dang mo: Prise_Chon chi mo lai cua so, vo hai - seasonnpc.lua:1316).
// - Loai 4 (dia do chi / mat chi, ~35%): LAM THAT - nho Xa Phu cho di dung toa do
//   (station.lua godatau: tl_getMapInfo + NewWorld + SetFightState) -> danh quai
//   tai do (hook nhat cuon TU CONG 1025) -> du -> ve NPC Task_Accept (tl_checktask
//   kiem 1025 that su).
// - Loai 1/2/3 (mua/nop/khoe do), 5 (chi so), 6 (manh SHXT): bot khong the lam
//   nhu nguoi (mua tiem, giu kho do, PK...) - coi nhu "lam xong ngoai man": goi
//   DUNG chuoi nhanh-thanh-cong cua script goc (Task_AwardRecord + Task_GiveAward).
//   Thuong/moc 10-40/cong hien bang/tran ngay van do script that phat.
// - Moi bot mot "NPC nha" (nLech % 10 trong 10 vi tri spec muc 1) de tram bot
//   khong dun mot cho.
// ===========================================================================

struct PB_DtNpc { int nMap, nX, nY; };
static const PB_DtNpc s_dtNpc[10] =
{
	{   1, 1620, 3089 },    // Phuong Tuong
	{  11, 3154, 5067 },    // Thanh Do
	{  20, 3537, 6231 },    // Giang Tan Thon
	{  37, 1736, 3101 },    // Bien Kinh
	{  53, 1626, 3172 },    // Ba Lang Huyen
	{  78, 1595, 3288 },    // Tuong Duong
	{  80, 1745, 2967 },    // Duong Chau
	{ 121, 1960, 4501 },    // Long Mon Tran
	{ 162, 1651, 3226 },    // Dai Ly
	{ 176, 1562, 2979 },    // Lam An
};

// ===========================================================================
// TIEN ICH DUNG CHUNG (19/08 toi)
// ===========================================================================

// Map thanh thi / thon (10 map co NPC Da Tau) - dung cho luat CUOI NGUA.
static int pb_LaThanhThi(int nMapId)
{
	for (int i = 0; i < 10; i++)
		if (s_dtNpc[i].nMap == nMapId)
			return 1;
	return 0;
}

// O nay DUNG DUOC khong (tren ban do bat ky, ke ca ban do bot chua o):
// engine bao trong VA luoi A* khong chan. Dung TRUOC moi ChangeWorld/SetPos
// de khong bao gio dap bot vao trong tuong (loi "ket giua tuong" map 79).
static bool pb_ODuoc(int nSubIdx, int nMpsX, int nMpsY)
{
	if (nSubIdx < 0 || nSubIdx >= MAX_SUBWORLD)
		return false;
	if (nMpsX <= 32 || nMpsY <= 32)
		return false;
	int nR = -1, nMX = 0, nMY = 0, nOX = 0, nOY = 0;
	SubWorld[nSubIdx].Mps2Map(nMpsX, nMpsY, &nR, &nMX, &nMY, &nOX, &nOY);
	if (nR < 0)
		return false;
	if (SubWorld[nSubIdx].m_Region[nR].GetBarrierMin(nMX, nMY, nOX, nOY, FALSE)
	    != Obstacle_NULL)
		return false;
	if (SubWorld[nSubIdx].CellObsSrv(nMpsX, nMpsY) == 1)
		return false;             // vung rong du lieu / vach - luoi A* da chan
	return true;
}

// Tim O TRONG gan diem goc (don vi O), quet xoan oc ban kinh 0..nRMax o.
// Tra 1 + toa do MPS. nLech xoay huong khoi dau -> nhieu bot khong dun 1 o.
static int pb_ODat(int nSubIdx, int nOX, int nOY, int nLech, int nRMax,
                   int* pnX, int* pnY)
{
	static const int aDx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	static const int aDy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
	for (int r = 0; r <= nRMax; r++)
	{
		for (int h = 0; h < 8; h++)
		{
			const int dir = (nLech + h) & 7;
			const int x = nOX * 32 + aDx[dir] * r * 32;
			const int y = nOY * 32 + aDy[dir] * r * 32;
			if (pb_ODuoc(nSubIdx, x, y))
			{
				*pnX = x;
				*pnY = y;
				return 1;
			}
			if (r == 0)
				break;                // tam chi co mot o
		}
	}
	return 0;
}

// so sanh chuoi con, KHONG phan biet hoa thuong (ActionScript trong pak co the
// viet hoa lung tung - chinh cai bay da lam godatau khong chay).
static bool pb_CoChuoi(const char* szNguon, const char* szTim)
{
	if (!szNguon || !szTim || !szTim[0])
		return false;
	for (int i = 0; szNguon[i]; i++)
	{
		int j = 0;
		while (szTim[j])
		{
			char a = szNguon[i + j];
			char b2 = szTim[j];
			if (a >= 'A' && a <= 'Z') a = (char)(a - 'A' + 'a');
			if (b2 >= 'A' && b2 <= 'Z') b2 = (char)(b2 - 'A' + 'a');
			if (a != b2)
				break;
			j++;
		}
		if (!szTim[j])
			return true;
	}
	return false;
}

// QUET NPC THAT tren ban do (chu game: "quet npc nhiem vu cho chinh xac, no nam
// o file pak"): duyet danh sach NPC cua TUNG region va so chuoi trong
// Npc[].ActionScript - truong nay nap thang tu du lieu NPC trong pak
// (KPlayer::DialogNpc:8080 dung chinh no de chay script khi nguoi choi bam NPC).
// Tra khe NPC GAN NHAT hoac 0.
static int pb_TimNpc(int nSub, const char* szScript, int bx, int by)
{
	if (nSub < 0 || nSub >= MAX_SUBWORLD)
		return 0;
	const int nTong = SubWorld[nSub].m_nTotalRegion;
	int nBest = 0;
	__int64 nBestD = 0;
	for (int r = 0; r < nTong; r++)
	{
		KIndexNode* pNode = (KIndexNode*)SubWorld[nSub].m_Region[r].m_NpcList.GetHead();
		while (pNode)
		{
			const int i = pNode->m_nIndex;
			pNode = (KIndexNode*)pNode->GetNext();
			if (i <= 0 || i >= MAX_NPC)              continue;
			if (Npc[i].m_dwID == 0)                  continue;
			if (Npc[i].m_Kind == kind_player)        continue;
			if (!Npc[i].ActionScript[0])             continue;
			if (!pb_CoChuoi(Npc[i].ActionScript, szScript)) continue;
			int ex = 0, ey = 0;
			Npc[i].GetMpsPos(&ex, &ey);
			const __int64 dd = (__int64)(ex - bx) * (ex - bx)
			               + (__int64)(ey - by) * (ey - by);
			if (!nBest || dd < nBestD)
			{
				nBest  = i;
				nBestD = dd;
			}
		}
	}
	return nBest;
}

// Ban nho: NPC Da Tau / Xa Phu dung YEN suot doi nen chi quet MOT LAN moi ban do
// (quet toan region x 200 bot x moi 2 giay la ton CPU vo ich). 0 = chua quet,
// -1 = ban do khong co, >0 = khe NPC. Kiem lai tinh hop le truoc khi tin.
#define PB_NPC_DT   0
#define PB_NPC_XP   1
static int s_npcNho[2][MAX_SUBWORLD];

static int pb_TimNpcNho(int nSub, int nLoaiNpc, const char* szScript, int bx, int by)
{
	if (nSub < 0 || nSub >= MAX_SUBWORLD || nLoaiNpc < 0 || nLoaiNpc > 1)
		return 0;
	const int nNho = s_npcNho[nLoaiNpc][nSub];
	if (nNho > 0 && nNho < MAX_NPC
	 && Npc[nNho].m_dwID != 0
	 && Npc[nNho].m_SubWorldIndex == nSub
	 && Npc[nNho].m_Kind != kind_player
	 && Npc[nNho].ActionScript[0]
	 && pb_CoChuoi(Npc[nNho].ActionScript, szScript))
		return nNho;                   // ban nho con dung
	if (nNho == -1)
		return 0;                      // da quet, ban do nay khong co NPC do
	const int nTim = pb_TimNpc(nSub, szScript, bx, by);
	s_npcNho[nLoaiNpc][nSub] = (nTim > 0) ? nTim : -1;
	if (nTim <= 0)
		pb_Log("[BotDT] map %d KHONG co NPC '%s' (quet %d region) - dung toa do bang\n",
		       SubWorld[nSub].m_SubWorldID, szScript, SubWorld[nSub].m_nTotalRegion);
	return nTim;
}

// CUOI NGUA khi di chuyen trong thanh thi/thon (chu game 19/08 toi).
static void pb_CuoiNguaTrongThanh(int nIdx, int nNpcIdx, int nSub, PB_Bot& b,
                                  unsigned int now)
{
	if (now - b.nNguaTick < (unsigned int)(GAME_FPS * 5))
		return;
	b.nNguaTick = now;
	if (!pb_LaThanhThi(SubWorld[nSub].m_SubWorldID))
		return;
	if (Npc[nNpcIdx].m_bRideHorse)
		return;
	if (Player[nIdx].m_ItemList.GetEquipment(itempart_horse) <= 0)
		return;
	Player[nIdx].CheckRideHorse(FALSE);        // FALSE = LEN ngua (xem KPlayer.cpp:9926)
}

// (D12 - 19/08 toi #2) DA XOA bang s_dtCuon + ham pb_DtToaDo: nhiem vu loai 6
// truoc day bi ChangeWorld toi "map co cuon roi" ma 8/10 map do la CHINH THANH,
// va diem ha canh la canh NPC Da Tau -> bot dung giua thanh khong co quai, dung
// im ca buoi (bang chung: NgoTai278 5 phut o map 80, 0 dong BotDame/BotDanh).
// Nay loai 6 lam o BAI LUYEN thuong (DTB_FARM_BAI): manh 1027 cong tu BAT KY
// cuon nao nhat duoc khi khong giu nhiem vu loai 4 khop map - khong can di dau.

// (D18) Ban do hien tai CO PHAI mot trong 10 thanh co NPC Da Tau? Tra chi so
// trong s_dtNpc, hoac -1. Truoc day "thanh nha" chot cung theo chi so khe bot
// nen bot dang dung NGAY TRONG thanh 53 van bi ep nhay ve thanh 37 - chinh la
// canh "phu ve thanh" chu game thay.
// khai truoc: pb_DonTui dinh nghia phia duoi nhung DTB_VE_TRA can goi
static void pb_DonTui(int nIdx, PB_Bot& b, unsigned int now);

static int pb_DtIdxThanh(int nMapId)
{
	for (int i = 0; i < 10; i++)
		if (s_dtNpc[i].nMap == nMapId)
			return i;
	return -1;
}

// Ve THANH NHA cua bot (dap xuong O TRONG cach NPC vai o - KHONG bao gio dap vao
// trong tuong nhu truoc). Tra 1 neu toi duoc.
static int pb_DtVeThanh(int nIdx, int nNpcIdx, PB_Bot& b, const PB_DtNpc& nha, int nLech)
{
	const int nSubT = g_SubWorldSet.SearchWorld(nha.nMap);
	int nVx = 0, nVy = 0;
	int nOk = 0;
	if (nSubT >= 0)
	{
		// dap xuong cach NPC 6..15 o (nhu vua tu cong thanh buoc vao), o TRONG
		const int nXaO = 6 + (nLech % 10);
		static const int aGx[4] = { 1, -1, 0, 0 };
		static const int aGy[4] = { 0, 0, 1, -1 };
		const int g2 = nLech & 3;
		if (pb_ODat(nSubT, nha.nX + aGx[g2] * nXaO, nha.nY + aGy[g2] * nXaO,
		            nLech, 8, &nVx, &nVy))
			nOk = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nVx, nVy) == 1);
		if (!nOk && pb_ODat(nSubT, nha.nX, nha.nY, nLech, 12, &nVx, &nVy))
			nOk = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nVx, nVy) == 1);
	}
	if (!nOk)
		nOk = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nha.nX * 32, nha.nY * 32) == 1);
	if (nOk)
	{
		b.nTargetNpc = 0;
		b.walk.Reset();  b.chase.Reset();  b.roam.Reset();
		b.nRoamX = 0;    b.nRoamY = 0;
		b.nBuocRaX = 0;  b.nBuocRaY = 0;
	}
	return nOk;
}

// Tra: 2 = khong lam / dang nghi -> luyen thuong; 1 = dang farm (loai 4 dung map /
// loai 6 dang o map cuon) -> danh quai tai cho, KHONG keo ve bai; 0 = tu dieu khien.
//
// ===========================================================================
// MAY TRANG THAI BOT LAM NHIEM VU DA TAU (viet lai 19/08 toi #2 sau dot dieu tra
// 13 agent doc ma + phap y bot.log). Vong doi day du, dung nguyen ly nguoi choi:
//
//   TOI_NPC -> THOAI(Task_Confirm/Task_TaskProcess) -> LOC
//     -> [T4: TOI_XAPHU -> GODATAU -> FARM_NV]  hoac  [T5/T6: FARM_BAI]
//     -> VE_TRA(Task_Accept) -> THUONG(finish_*) -> TOI_NPC ...
//   du 40 nhiem vu/ngay -> NGHI 30 phut -> ve bai luyen cay cap nhu thuong.
//
// Hop dong tra ve GIU NGUYEN voi pb_DriveBot:
//   0 = ham nay dang tu dieu khien -> pb_DriveBot return ngay
//   1 = dang farm TREN MAP NHIEM VU -> chan pb_RaBai keo ve bai
//   2 = nha may -> bot luyen cap binh thuong
//
// BA NGO CUT DA BIT (deu tim ra tu log that + bang du lieu):
//  * Nhiem vu MAT CHI (1032 byte thap == 2): cuon 212 CHI rot tu boss xanh
//    (~1/31.360 con) -> KHONG NHAN, doi nhiem vu.
//  * Nhiem vu doi > PB_DT_CUON_TOI_DA cuon: bac link 11 toan dong Num=15
//    (~13.500 con quai) -> KHONG NHAN, doi nhiem vu.
//  * Farm khong ra cuon: 20 phut khong tang so cuon -> doi nhiem vu (mien phi).
// ===========================================================================
static int pb_DaTau(int nIdx, int nNpcIdx, int nSub, PB_Bot& b, int nLech)
{
	// ---------------- chan chung (ap cho moi pha) ----------------
	if (!b.nDaTauChon)
		return 2;
	if (Npc[nNpcIdx].m_Level < PB_DT_CAP_TOI_THIEU)
		return 2;
	if (Npc[nNpcIdx].m_Doing == do_death || Npc[nNpcIdx].m_Doing == do_revive)
		return 2;                      // dang nam cho hoi sinh - de vong hoi sinh lo

	const unsigned int nowT = SubWorld[nSub].m_dwCurrentTime;
	if (b.nDaTauNghi)
	{
		if (nowT < b.nDaTauNghi)
			return 2;
		b.nDaTauNghi = 0;
		b.nDtPha     = DTB_TOI_NPC;
	}

	// NPC gian (huy lau qua 3 lan): duong xoa phat DUY NHAT cua he goc la gap lai
	// NPC sau ~10 phut (tasklink_entence tu reset 1036). Nghi 11 phut cho chac.
	if ((int)Player[nIdx].m_cTask.GetSaveVal(1036) == 10)
	{
		Player[nIdx].ExecuteScript((char*)PB_DT_SCRIPT, (char*)"tasklink_entence", 0, false);
		if ((int)Player[nIdx].m_cTask.GetSaveVal(1036) == 10)
		{
			b.nDaTauNghi = nowT + (unsigned int)(GAME_FPS * 660);
			pb_Log("[BotDT] %s bi Da Tau gian -> ve bai luyen, 11 phut gap lai\n",
			       Player[nIdx].m_PlayerName);
			return 2;
		}
	}

	const int nCourse = (int)Player[nIdx].m_cTask.GetSaveVal(PB_DT_COURSE);
	if (nCourse != 1)
	{
		b.nDaTauHan = 0;  b.nDaTauThu = 0;  b.nDaTauKe = 0;
		b.nDtFarmMoc = 0; b.nDtCoCu = 0;    b.nDtTraThu = 0;
	}
	// dong bo pha theo course - chong lech trang thai sau restart / nap lai bot
	if (nCourse == 0 || nCourse == 3)
	{
		if (b.nDtPha != DTB_TOI_NPC && b.nDtPha != DTB_THOAI)
			b.nDtPha = DTB_TOI_NPC;
	}
	else if (nCourse == 2)
		b.nDtPha = DTB_THUONG;
	else if (b.nDtPha == DTB_TOI_NPC || b.nDtPha == DTB_THOAI)
		b.nDtPha = DTB_LOC;      // vua nhan duoc nhiem vu -> sang loc

	// "Thanh nha": uu tien CHINH thanh bot dang dung (D18), khong thi theo khe bot
	int nIdxThanh = pb_DtIdxThanh(SubWorld[nSub].m_SubWorldID);
	if (nIdxThanh < 0)
		nIdxThanh = nLech % 10;
	const PB_DtNpc& nha = s_dtNpc[nIdxThanh];

	// so lieu nhiem vu dang giu (chi co nghia khi course == 1)
	const int nLoai  = (nCourse == 1)
	                 ? (int)Player[nIdx].m_cTask.GetSaveVal(PB_DT_LOAI) : 0;
	const int n1032  = (int)Player[nIdx].m_cTask.GetSaveVal(PB_DT_CAN);
	const int nKieu  = n1032 & 0xFF;              // 1 = Dia Do Chi (205), 2 = Mat Chi (212)
	const int nCan   = (n1032 >> 8) & 0xFF;       // so cuon phai gom
	const int nCo    = (int)Player[nIdx].m_cTask.GetSaveVal(PB_DT_CO);
	const int nMapNv = (int)Player[nIdx].m_cTask.GetSaveVal(PB_DT_MAP);

	switch (b.nDtPha)
	{
	// ------------------------------------------------ 1. CHAY TOI NPC DA TAU
	case DTB_TOI_NPC:
	{
		if (SubWorld[nSub].m_SubWorldID != nha.nMap)
		{
			if (nowT - b.nDaTauTick < (unsigned int)(GAME_FPS * 3))
				return 0;
			b.nDaTauTick = nowT;
			if (pb_TrongNhom(nIdx))
				pb_RoiNhom(nIdx, "di lam nhiem vu Da Tau");
			if (!pb_DtVeThanh(nIdx, nNpcIdx, b, nha, nLech))
			{
				pb_Log("[BotDT] %s KHONG toi duoc thanh %d - nghi 10 phut\n",
				       Player[nIdx].m_PlayerName, nha.nMap);
				b.nDaTauNghi = nowT + (unsigned int)(GAME_FPS * 600);
				return 2;
			}
			b.nDtToiNpcThu = 0;
			return 0;
		}
		// dung thanh roi -> chay bo toi NPC (quet NPC THAT tren ban do)
		int nBx = 0, nBy = 0;
		Npc[nNpcIdx].GetMpsPos(&nBx, &nBy);
		int nTx = nha.nX * 32, nTy = nha.nY * 32;
		const int nNpcDt = pb_TimNpcNho(nSub, PB_NPC_DT, "seasonnpc", nBx, nBy);
		if (nNpcDt > 0)
			Npc[nNpcDt].GetMpsPos(&nTx, &nTy);
		const __int64 ddx = (__int64)nBx - nTx;
		const __int64 ddy = (__int64)nBy - nTy;
		if (ddx * ddx + ddy * ddy > (__int64)(96 * 96))
		{
			if (++b.nDtToiNpcThu > 600)        // ~1-2 phut khong toi noi
			{
				b.nDtToiNpcThu = 0;
				b.nDaTauNghi = nowT + (unsigned int)(GAME_FPS * 300);
				pb_Log("[BotDT] %s khong toi duoc NPC Da Tau o thanh %d - nghi 5 phut\n",
				       Player[nIdx].m_PlayerName, nha.nMap);
				return 2;
			}
			pb_CuoiNguaTrongThanh(nIdx, nNpcIdx, nSub, b, nowT);
			const int nW = PB_WalkTo(nNpcIdx, nTx + 64, nTy, nSub, b.walk, 96);
			if (nW == 0)
				return 0;
			if (nW < 0)
			{
				int dx3 = 0, dy3 = 0;
				if (pb_ODat(nSub, nTx / 32, nTy / 32, nLech, 6, &dx3, &dy3))
					Npc[nNpcIdx].SetPos(dx3, dy3);
				b.walk.Reset();
			}
		}
		b.nDtToiNpcThu = 0;
		b.nDtPha = DTB_THOAI;
		return 0;
	}

	// ------------------------------------------------ 2. NOI CHUYEN VOI NPC
	case DTB_THOAI:
	{
		if (nowT - b.nDaTauTick < (unsigned int)(GAME_FPS * 2))
			return 0;
		b.nDaTauTick = nowT;
		if (nCourse == 0)
		{
			pb_Log("[BotDT] %s nhap mon Da Tau (Task_Confirm)\n", Player[nIdx].m_PlayerName);
			Player[nIdx].ExecuteScript((char*)PB_DT_SCRIPT, (char*)"Task_Confirm", 0, false);
		}
		else if (nCourse == 3)
		{
			if ((int)Player[nIdx].m_cTask.GetSaveVal(PB_DT_NGAY) >= 40)
			{
				pb_Log("[BotDT] %s DU 40 nhiem vu hom nay -> ve bai luyen theo cap\n",
				       Player[nIdx].m_PlayerName);
				b.nDaTauNghi = nowT + (unsigned int)(GAME_FPS * 1800);
				b.nDtPha = DTB_NGHI;
				return 2;
			}
			Player[nIdx].ExecuteScript((char*)PB_DT_SCRIPT, (char*)"Task_TaskProcess", 0, false);
		}
		// doc lai course NGAY sau lenh de biet thuc su den dau
		const int nC2 = (int)Player[nIdx].m_cTask.GetSaveVal(PB_DT_COURSE);
		if (nC2 == 1)
			b.nDtPha = DTB_LOC;
		else if (nC2 == 2)
			b.nDtPha = DTB_THUONG;
		return 0;
	}

	// ------------------------------------------------ 3. LOC NHIEM VU
	case DTB_LOC:
	{
		if (nCourse != 1)
		{
			b.nDtPha = (nCourse == 2) ? DTB_THUONG : DTB_TOI_NPC;
			return 0;
		}
		if (nowT - b.nDaTauTick < (unsigned int)(GAME_FPS * 2))
			return 0;
		b.nDaTauTick = nowT;

		int bNhan = 0;
		if (nLoai == 4)
		{
			if (nCan <= 0 || nMapNv <= 0)
				return 0;                      // task chua sync xong - cho nhip sau
			bNhan = (nKieu == 1 && nCan <= PB_DT_CUON_TOI_DA);
		}
		else if (nLoai == 5 || nLoai == 6)
			bNhan = 1;

		if (bNhan)
		{
			b.nDaTauKe = 0;
			if (nLoai == 4)
			{
				b.nDtCoCu    = nCo;
				b.nDtFarmMoc = nowT;
				b.nDtXaFuThu = 0;
				b.nDtPha = (SubWorld[nSub].m_SubWorldID == nMapNv)
				         ? DTB_FARM_NV : DTB_TOI_XAPHU;
				pb_Log("[BotDT] %s NHAN nhiem vu loai 4: gom %d cuon o map %d\n",
				       Player[nIdx].m_PlayerName, nCan, nMapNv);
			}
			else
			{
				b.nDaTauHan = nowT + (unsigned int)(GAME_FPS * 2700);   // 45 phut
				b.nDaTauThu = nowT;
				b.nDtPha    = DTB_FARM_BAI;
				pb_Log("[BotDT] %s NHAN nhiem vu loai %d - vua luyen cap vua lam\n",
				       Player[nIdx].m_PlayerName, nLoai);
			}
			return 0;
		}

		// KHONG nhan -> doi nhiem vu MIEN PHI (tl_dealtask: khong cham 2420 tran
		// ngay / 2797 huy / 1036 gian / luot huy). Chi lay loai 4 nhe, 5 va 6.
		if (++b.nDaTauKe > 40)
		{
			b.nDaTauKe = 0;
			b.nDaTauNghi = nowT + (unsigned int)(GAME_FPS * 300);
			pb_Log("[BotDT] %s doi 40 lan chua ra nhiem vu vua suc - nghi 5 phut\n",
			       Player[nIdx].m_PlayerName);
			return 2;
		}
		{
			static unsigned int s_uDoiLog = 0;
			if (nowT - s_uDoiLog >= (unsigned int)(GAME_FPS * 5))
			{
				s_uDoiLog = nowT;
				pb_Log("[BotDT] %s bo nhiem vu loai %d (kieu %d, can %d cuon) - doi de khac,"
				       " khong ton gi\n", Player[nIdx].m_PlayerName, nLoai, nKieu, nCan);
			}
		}
		Player[nIdx].ExecuteScript((char*)PB_DT_SCRIPT, (char*)"PB_BotDoiNhiemVu", 0, false);
		return 0;
	}

	// ------------------------------------------------ 4. CHAY TOI XA PHU
	case DTB_TOI_XAPHU:
	{
		if (nCourse != 1 || nLoai != 4)
		{
			b.nDtPha = DTB_LOC;
			return 0;
		}
		if (SubWorld[nSub].m_SubWorldID == nMapNv)
		{
			b.nDtPha     = DTB_FARM_NV;
			b.nDtFarmMoc = nowT;
			return 1;
		}
		if (pb_TrongNhom(nIdx))
			pb_RoiNhom(nIdx, "di lam nhiem vu Da Tau");
		if (SubWorld[nSub].m_SubWorldID != nha.nMap)
		{
			if (nowT - b.nDaTauTick < (unsigned int)(GAME_FPS * 3))
				return 0;
			b.nDaTauTick = nowT;
			pb_Log("[BotDT] %s dang o map %d - ve thanh %d gap Xa Phu\n",
			       Player[nIdx].m_PlayerName, SubWorld[nSub].m_SubWorldID, nha.nMap);
			if (!pb_DtVeThanh(nIdx, nNpcIdx, b, nha, nLech))
			{
				b.nDaTauNghi = nowT + (unsigned int)(GAME_FPS * 600);
				pb_Log("[BotDT] %s KHONG ve duoc thanh %d - nghi 10 phut\n",
				       Player[nIdx].m_PlayerName, nha.nMap);
				return 2;
			}
			return 0;
		}
		// dang o thanh -> chay bo toi Xa Phu THAT
		int bx2 = 0, by2 = 0;
		Npc[nNpcIdx].GetMpsPos(&bx2, &by2);
		const int nXp = pb_TimNpcNho(nSub, PB_NPC_XP, "xaphu", bx2, by2);
		if (nXp > 0)
		{
			int px = 0, py = 0;
			Npc[nXp].GetMpsPos(&px, &py);
			const __int64 dxp = (__int64)bx2 - px;
			const __int64 dyp = (__int64)by2 - py;
			if (dxp * dxp + dyp * dyp > (__int64)(PB_FAC_ARRIVE_MPS * PB_FAC_ARRIVE_MPS))
			{
				if (++b.nDtToiNpcThu > 600)
				{
					b.nDtToiNpcThu = 0;
					pb_Log("[BotDT] %s khong toi duoc Xa Phu - doi nhiem vu khac\n",
					       Player[nIdx].m_PlayerName);
					Player[nIdx].ExecuteScript((char*)PB_DT_SCRIPT,
					                           (char*)"PB_BotDoiNhiemVu", 0, false);
					b.nDtPha = DTB_LOC;
					return 0;
				}
				pb_CuoiNguaTrongThanh(nIdx, nNpcIdx, nSub, b, nowT);
				const int nW2 = PB_WalkTo(nNpcIdx, px, py, nSub, b.walk, PB_FAC_ARRIVE_MPS);
				if (nW2 == 0)
					return 0;
				if (nW2 < 0)
				{
					int dx2 = 0, dy2 = 0;
					if (pb_ODat(nSub, px / 32, py / 32, nLech, 6, &dx2, &dy2))
						Npc[nNpcIdx].SetPos(dx2, dy2);
					b.walk.Reset();
				}
			}
		}
		b.nDtToiNpcThu = 0;
		b.nDtPha = DTB_GODATAU;
		return 0;
	}

	// ------------------------------------------------ 5. KICH XA PHU CHO DI
	case DTB_GODATAU:
	{
		if (nCourse != 1 || nLoai != 4)
		{
			b.nDtPha = DTB_LOC;
			return 0;
		}
		if (SubWorld[nSub].m_SubWorldID == nMapNv)
		{
			b.nDtXaFuThu = 0;
			b.nDtFarmMoc = nowT;
			b.nDtCoCu    = nCo;
			b.nDtPha     = DTB_FARM_NV;
			pb_Log("[BotDT] %s DA TOI map nhiem vu %d - bat dau gom cuon (%d/%d)\n",
			       Player[nIdx].m_PlayerName, nMapNv, nCo, nCan);
			return 1;
		}
		if (nowT - b.nDaTauTick < (unsigned int)(GAME_FPS * 2))
			return 0;
		b.nDaTauTick = nowT;
		if (++b.nDtXaFuThu > 5)
		{
			b.nDtXaFuThu = 0;
			pb_Log("[BotDT] %s kich Xa Phu 5 lan van khong sang duoc map %d"
			       " - doi nhiem vu khac\n", Player[nIdx].m_PlayerName, nMapNv);
			Player[nIdx].ExecuteScript((char*)PB_DT_SCRIPT, (char*)"PB_BotDoiNhiemVu", 0, false);
			b.nDtPha = DTB_LOC;
			return 0;
		}
		pb_Log("[BotDT] %s kich Xa Phu 'Den noi lam nhiem vu da tau' -> map %d (%d/%d)\n",
		       Player[nIdx].m_PlayerName, nMapNv, nCo, nCan);
		// duong dan PHAI viet THUONG: g_FileName2Id bam tung byte, khong ha chu
		Player[nIdx].ExecuteScript((char*)"\\script\\global\\station.lua",
		                           (char*)"godatau", 0, false);
		b.nTargetNpc = 0;
		b.walk.Reset();  b.chase.Reset();  b.roam.Reset();
		b.nRoamX = 0;    b.nRoamY = 0;
		b.nBuocRaX = 0;  b.nBuocRaY = 0;
		return 0;
	}

	// ------------------------------------------------ 6. GOM CUON TAI MAP NHIEM VU
	case DTB_FARM_NV:
	{
		if (nCourse != 1 || nLoai != 4)
		{
			b.nDtPha = (nCourse == 2) ? DTB_THUONG : DTB_LOC;
			return 0;
		}
		if (nCo >= nCan && nCan > 0)
		{
			pb_Log("[BotDT] %s DU %d/%d cuon -> ve NPC tra nhiem vu\n",
			       Player[nIdx].m_PlayerName, nCo, nCan);
			b.nDtPha    = DTB_VE_TRA;
			b.nDtTraThu = 0;
			return 0;
		}
		if (SubWorld[nSub].m_SubWorldID != nMapNv)
		{
			b.nDtPha = DTB_TOI_XAPHU;        // lac map -> ve thanh di lai
			return 0;
		}
		if (nCo > b.nDtCoCu)
		{
			b.nDtCoCu    = nCo;
			b.nDtFarmMoc = nowT;             // co tien do -> gia han
			pb_Log("[BotDT] %s nhat cuon (%d/%d) tai map %d\n",
			       Player[nIdx].m_PlayerName, nCo, nCan, nMapNv);
		}
		else if (b.nDtFarmMoc && nowT - b.nDtFarmMoc > (unsigned int)(GAME_FPS * 1200))
		{
			// 20 phut khong them duoc cuon nao -> nhiem vu nay khong kha thi
			pb_Log("[BotDT] %s farm 20 phut khong ra cuon (%d/%d) - doi nhiem vu khac\n",
			       Player[nIdx].m_PlayerName, nCo, nCan);
			Player[nIdx].ExecuteScript((char*)PB_DT_SCRIPT, (char*)"PB_BotDoiNhiemVu", 0, false);
			b.nDtFarmMoc = 0;
			b.nDtPha     = DTB_LOC;
			return 0;
		}
		return 1;                            // danh quai tai cho, KHONG cho ra bai
	}

	// ------------------------------------------------ 7. LOAI 5/6: VUA LUYEN VUA LAM
	case DTB_FARM_BAI:
	{
		if (nCourse != 1)
		{
			b.nDtPha = (nCourse == 2) ? DTB_THUONG : DTB_LOC;
			return 0;
		}
		if (nLoai == 4)
		{
			b.nDtPha = DTB_LOC;
			return 0;
		}
		if (b.nDaTauHan && nowT > b.nDaTauHan)
		{
			pb_Log("[BotDT] %s loai %d qua 45 phut chua xong - doi nhiem vu khac\n",
			       Player[nIdx].m_PlayerName, nLoai);
			Player[nIdx].ExecuteScript((char*)PB_DT_SCRIPT, (char*)"PB_BotDoiNhiemVu", 0, false);
			b.nDaTauHan = 0;
			b.nDtPha    = DTB_LOC;
			return 0;
		}
		if (nowT - b.nDaTauThu >= (unsigned int)(GAME_FPS * 300))
		{
			b.nDaTauThu = nowT;              // toi ky -> ve NPC thu tra
			b.nDtPha    = DTB_VE_TRA;
			b.nDtTraThu = 0;
			return 0;
		}
		return 2;                            // luyen cap binh thuong, cuon nhat duoc tu cong
	}

	// ------------------------------------------------ 8. VE NPC TRA NHIEM VU
	case DTB_VE_TRA:
	{
		if (nCourse == 2)
		{
			b.nDtPha = DTB_THUONG;
			return 0;
		}
		if (nCourse != 1)
		{
			b.nDtPha = DTB_TOI_NPC;
			return 0;
		}
		if (SubWorld[nSub].m_SubWorldID != nha.nMap)
		{
			if (nowT - b.nDaTauTick < (unsigned int)(GAME_FPS * 3))
				return 0;
			b.nDaTauTick = nowT;
			if (pb_TrongNhom(nIdx))
				pb_RoiNhom(nIdx, "ve tra nhiem vu Da Tau");
			if (!pb_DtVeThanh(nIdx, nNpcIdx, b, nha, nLech))
			{
				b.nDaTauNghi = nowT + (unsigned int)(GAME_FPS * 600);
				return 2;
			}
			return 0;
		}
		// toi thanh -> chay toi NPC
		int nBx2 = 0, nBy2 = 0;
		Npc[nNpcIdx].GetMpsPos(&nBx2, &nBy2);
		int nTx2 = nha.nX * 32, nTy2 = nha.nY * 32;
		const int nNpcDt2 = pb_TimNpcNho(nSub, PB_NPC_DT, "seasonnpc", nBx2, nBy2);
		if (nNpcDt2 > 0)
			Npc[nNpcDt2].GetMpsPos(&nTx2, &nTy2);
		const __int64 dx4 = (__int64)nBx2 - nTx2;
		const __int64 dy4 = (__int64)nBy2 - nTy2;
		if (dx4 * dx4 + dy4 * dy4 > (__int64)(96 * 96))
		{
			pb_CuoiNguaTrongThanh(nIdx, nNpcIdx, nSub, b, nowT);
			const int nW3 = PB_WalkTo(nNpcIdx, nTx2 + 64, nTy2, nSub, b.walk, 96);
			if (nW3 == 0)
				return 0;
			if (nW3 < 0)
			{
				int dx5 = 0, dy5 = 0;
				if (pb_ODat(nSub, nTx2 / 32, nTy2 / 32, nLech, 6, &dx5, &dy5))
					Npc[nNpcIdx].SetPos(dx5, dy5);
				b.walk.Reset();
			}
			return 0;
		}
		if (nowT - b.nDaTauTick < (unsigned int)(GAME_FPS * 2))
			return 0;
		b.nDaTauTick = nowT;

		// TUI PHAI CON >= 5 O: Task_Accept chan ngay dong dau neu thieu (seasonnpc.lua)
		// - khong guard thi bot goi mai, im lang, khong bao gio tra duoc.
		if (Player[nIdx].m_ItemList.CalcFreeItemCellCount(1, 1, room_equipment) < 5)
		{
			b.nDonTuiTick = 0;                 // ep don tui ngay nhip sau
			pb_DonTui(nIdx, b, nowT);
			if (++b.nDtTuiThu > 5)
			{
				b.nDtTuiThu = 0;
				b.nDaTauNghi = nowT + (unsigned int)(GAME_FPS * 900);
				pb_Log("[BotDT] %s tui day khong tra duoc nhiem vu - nghi 15 phut\n",
				       Player[nIdx].m_PlayerName);
				return 2;
			}
			return 0;
		}
		b.nDtTuiThu = 0;
		Player[nIdx].ExecuteScript((char*)PB_DT_SCRIPT, (char*)"Task_Accept", 0, false);
		// doc lai course SAU lenh - day moi la bang chung tra duoc hay khong
		const int nC3 = (int)Player[nIdx].m_cTask.GetSaveVal(PB_DT_COURSE);
		if (nC3 == 2)
		{
			pb_Log("[BotDT] %s TRA XONG nhiem vu loai %d -> chon ruong thuong\n",
			       Player[nIdx].m_PlayerName, nLoai);
			b.nDaTauNut = 0;
			b.nDtPha    = DTB_THUONG;
			return 0;
		}
		if (++b.nDtTraThu > 8)
		{
			pb_Log("[BotDT] %s tra HUT 8 lan (loai %d, %d/%d) - doi nhiem vu khac\n",
			       Player[nIdx].m_PlayerName, nLoai, nCo, nCan);
			Player[nIdx].ExecuteScript((char*)PB_DT_SCRIPT, (char*)"PB_BotDoiNhiemVu", 0, false);
			b.nDtTraThu = 0;
			b.nDtPha    = DTB_LOC;
			return 0;
		}
		// chua tra duoc -> quay lai lam tiep
		b.nDtPha = (nLoai == 4) ? DTB_FARM_NV : DTB_FARM_BAI;
		if (nLoai != 4)
			b.nDaTauThu = nowT;                // bat ky thu tra 5 phut moi
		return 0;
	}

	// ------------------------------------------------ 9. CHON RUONG THUONG
	case DTB_THUONG:
	{
		if (nCourse != 2)
		{
			b.nDaTauNut = 0;
			b.nDtPha = (nCourse == 1) ? DTB_LOC : DTB_TOI_NPC;
			return 0;
		}
		if (nowT - b.nDaTauTick < (unsigned int)(GAME_FPS * 2))
			return 0;
		b.nDaTauTick = nowT;
		static const char* aNut[6] = { "finish_item", "finish_exp", "finish_money",
		                               "quest_random", "finish_point", "finish_lucky" };
		Player[nIdx].ExecuteScript((char*)PB_DT_SCRIPT,
		                           (char*)aNut[b.nDaTauNut % 6], 0, false);
		if (++b.nDaTauNut >= 8)
		{
			b.nDaTauNut  = 0;
			b.nDaTauNghi = nowT + (unsigned int)(GAME_FPS * 300);
			pb_Log("[BotDT] %s khong nhan duoc thuong (course van 2) - nghi 5 phut\n",
			       Player[nIdx].m_PlayerName);
			return 2;
		}
		if ((int)Player[nIdx].m_cTask.GetSaveVal(PB_DT_COURSE) == 3)
		{
			b.nDaTauNut = 0;
			b.nDtPha    = DTB_TOI_NPC;         // xin nhiem vu ke tiep
		}
		return 0;
	}

	default:
		b.nDtPha = DTB_TOI_NPC;
		return 0;
	}
}
// ===========================================================================
// BOT RA THANH NGOI BAN SAP (chu game 19/08 chieu - thay trang tri SC_ NPC)
//
// Bot duoc boc (PB_SetBanSap, ngau nhien) ra khu trung tam thanh "nha", bay
// 3-5 mon TRANG SUC TRANG (nhan 0/3/0, lien 0/4/0, boi 0/9/0 - dung nguon do
// nguoi choi can nop nhiem vu Da Tau loai 2 dong 'n') voi GIA = gia goc x2,
// mo sap THAT: m_BaiTan=1 + ShopName tu settings/simcity/stall_adv.txt.
// Nguoi choi XEM + MUA qua dung duong TradeBuyItem cua nguoi that
// (KProtocolProcess.cpp:6060 - hoan toan server-side, tien vao tui bot).
// Het hang -> 5 phut sau chau dot moi (don truoc trang suc gia-0 ton kho).
// ===========================================================================
static char s_sapTen[24][32];
static int  s_sapTenSo = -1;

static void pb_NapTenSap()
{
	if (s_sapTenSo >= 0)
		return;
	s_sapTenSo = 0;
	FILE* f = fopen("settings\\simcity\\stall_adv.txt", "rb");
	if (!f)
		return;
	char sz[128];
	while (s_sapTenSo < 24 && fgets(sz, sizeof(sz), f))
	{
		int n = (int)strlen(sz);
		while (n > 0 && (sz[n - 1] == '\r' || sz[n - 1] == '\n'))
			sz[--n] = 0;
		if (n <= 0 || n >= 31)
			continue;
		strcpy(s_sapTen[s_sapTenSo++], sz);
	}
	fclose(f);
}

static void pb_BanSap(int nIdx, int nNpcIdx, int nSub, PB_Bot& b)
{
	const int nLech = (int)(&b - s_bots);
	const unsigned int nowT = SubWorld[nSub].m_dwCurrentTime;
	const PB_DtNpc& nha = s_dtNpc[nLech % 10];

	if (SubWorld[nSub].m_SubWorldID != nha.nMap)
	{
		if (nowT - b.nDaTauTick < (unsigned int)(GAME_FPS * 3))
			return;
		b.nDaTauTick = nowT;
		if (pb_TrongNhom(nIdx))
			pb_RoiNhom(nIdx, "ra thanh ngoi ban sap");
		// (19/08 toi chu game) CHI ngoi O TRONG quanh NPC Da Tau - truoc day dap
		// theo luoi 12x8 cung nhac nen bot ngoi vao GOC LAG / trong bui truc.
		const int nSubT = g_SubWorldSet.SearchWorld(nha.nMap);
		int nSx = 0, nSy = 0;
		int bDat = 0;
		int nTamX = nha.nX, nTamY = nha.nY;
		if (nSubT >= 0)
		{
			// tam = NPC Da Tau THAT tren ban do (toa do bang chi la duong lui)
			const int nDtNpc2 = pb_TimNpcNho(nSubT, PB_NPC_DT, "seasonnpc",
			                                 nha.nX * 32, nha.nY * 32);
			if (nDtNpc2 > 0)
			{
				int tx2 = 0, ty2 = 0;
				Npc[nDtNpc2].GetMpsPos(&tx2, &ty2);
				nTamX = tx2 / 32;
				nTamY = ty2 / 32;
			}
			// rai quanh NPC theo vong tron ban kinh 3..10 o, moi bot mot goc rieng
			const int nBk = 3 + (nLech % 8);
			static const int aQx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
			static const int aQy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
			const int q3 = (nLech / 8) & 7;
			if (pb_ODat(nSubT, nTamX + aQx[q3] * nBk, nTamY + aQy[q3] * nBk,
			            nLech, 6, &nSx, &nSy))
				bDat = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nSx, nSy) == 1);
			if (!bDat && pb_ODat(nSubT, nTamX, nTamY, nLech, 12, &nSx, &nSy))
				bDat = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nSx, nSy) == 1);
		}
		if (!bDat)
			Npc[nNpcIdx].ChangeWorld(nha.nMap, nha.nX * 32, nha.nY * 32);
		b.nTargetNpc = 0;
		b.nBanSapNgoi = 0;
		b.walk.Reset();  b.chase.Reset();  b.roam.Reset();
		b.nRoamX = 0;    b.nRoamY = 0;
		b.nBuocRaX = 0;  b.nBuocRaY = 0;
		return;
	}

	if (!b.nBanSapXong
	 || (Player[nIdx].GetTradeCount() == 0
	     && nowT - b.nDaTauThu >= (unsigned int)(GAME_FPS * 300)))
	{
		b.nBanSapXong = 1;
		b.nDaTauThu = nowT;
		pb_NapTenSap();

		// don trang suc gia-0 ton kho (vd mat gia sau restart) truoc khi chau
		for (int q = 1; q < MAX_PLAYER_ITEM; q++)
		{
			const int g = Player[nIdx].m_ItemList.m_Items[q].nIdx;
			if (g <= 0 || g >= MAX_ITEM)
				continue;
			if (Player[nIdx].m_ItemList.m_Items[q].nPlace != pos_equiproom)
				continue;
			if (Player[nIdx].m_ItemList.m_Items[q].nPrice > 0)
				continue;
			if (Item[g].GetGenre() != item_equip)
				continue;
			const int nDe = Item[g].GetDetailType();
			if (nDe == 3 || nDe == 4 || nDe == 9)
				Player[nIdx].m_ItemList.RemoveItemIdx(g, Item[g].GetStackNum());
		}

		int nBay = 0;
		const int nMuon = 3 + (nLech % 3);
		for (int t2 = 0; t2 < nMuon; t2++)
		{
			static const int aDet[3] = { 3, 4, 9 };   // nhan / lien / boi
			const int nDet = aDet[(nLech + t2) % 3];
			const int nLv  = 1 + (((int)g_Random(10) + nLech * 7 + t2 * 3) % 10);
			// (19/08 toi #2 - chu game: "bay do XANH nhu do danh npc rot ra, khong
			// phai do TIM"). LUAT MAU cua engine (KItem.cpp:3200 GetColorItem):
			//   nGoldId/NATURE_GOLD -> vang | NATURE_PLATINA -> bach kim
			//   IsPurple() -> TIM ; IsPurple() = (m_CommonAttrib.nPoint != 0) (:3222)
			//   con lai co m_aryMagicAttrib[0].nAttribType != 0 -> XANH ; het -> trang
			// Ban truoc dung AddItemSet2 voi nPoint = so opt -> Gen_Equipment goi
			// SetPoint(nPoint) (KItemGenerator.cpp:417) -> MOI MON THANH TIM.
			// DUONG DO XANH DUNG (chinh la duong quai rot do): nPoint == 0 VA truyen
			// mang nMagicLevel co gia tri -> Gen_Equipment nhanh :368-374 goi
			// Gen_MagicAttrib(nType, pnaryMALevel, ...) - ham nay doc pnaryMALevel[i]
			// lam CAP thuoc tinh phu, DUNG o phan tu = 0 dau tien (:441-443), roc
			// theo m_DropRate nhu do rot that. ItemSet.Add KHONG co tham so nPoint
			// nen luon nPoint = 0 -> dung ham nay.
			// Rai thuoc tinh phu Y HET duong quai rot do (KNpc::LoseSingleItem):
			// cap phu 1..6 (tran MAX_MAGIC_LEVEL = 6), rai lan luot 6 o, gap o
			// truot thi DUNG (Gen_MagicAttrib doc toi phan tu 0 dau tien la thoi).
			// Mang PHAI khai [MAX_ITEM_MAGICLEVEL] (16) + ZeroMemory: Gen_MagicAttrib
			// lap toi i < 16 va co doc pnaryMALevel[8].
			int nMagic[MAX_ITEM_MAGICLEVEL];
			ZeroMemory(nMagic, sizeof(nMagic));
			const int nMay    = 10 + (int)g_Random(21);   // do may 10..30: hang co gia
			const int nCapPhu = 1 + (((int)g_Random(6) + nLech + t2) % 6);   // 1..6
			for (int o2 = 0; o2 < 6; o2++)
			{
				if (!g_Random(2 + (nMay > 10 ? 10 : nMay)))
					break;
				nMagic[o2] = nCapPhu;
			}
			if (nMagic[0] == 0)
				nMagic[0] = nCapPhu;              // khong co opt nao = do TRANG, bi vut
			// ItemSet.Add KHONG co tham so nPoint => luon nPoint = 0 => Gen_MagicAttrib
			// => do XANH (KItemGenerator nhanh nPoint == 0). AddItemSet2 co nPoint,
			// truyen khac 0 la SetPoint => IsPurple => do TIM (loi ban truoc).
			const int nNew = ItemSet.Add(0, 0, (nLech + t2) % 5, nLv, nMay, nDet, 0,
			                             nMagic, g_SubWorldSet.GetGameVersion(), 0,
			                             KItemSet::genXOpt(nMay));
			if (nNew <= 0)
				continue;
			Player[nIdx].m_ItemList.InsertEquipment(nNew, false);
			const int q2 = Player[nIdx].m_ItemList.FindSame(nNew);
			if (q2 <= 0)
			{
				ItemSet.Remove(nNew);
				continue;              // tui day
			}
			// kiem chung MAU ngay tai cho: chi giu do XANH (co opt). Do trang
			// (khong roc duoc opt nao) thi bo, khoi bay hang vo dung.
			if (Item[nNew].GetColorItem() != green_item)
			{
				static unsigned int s_uMauLog = 0;
				if (nowT - s_uMauLog >= (unsigned int)(GAME_FPS * 30))
				{
					s_uMauLog = nowT;
					pb_Log("[BotSap] %s bo mon khong phai do xanh (mau=%d, detail=%d cap=%d)\n",
					       Player[nIdx].m_PlayerName, Item[nNew].GetColorItem(), nDet, nLv);
				}
				Player[nIdx].m_ItemList.RemoveItemIdx(nNew, Item[nNew].GetStackNum());
				continue;
			}
			int nGia = Item[nNew].m_CommonAttrib.nPrice * 2;
			if (nGia < 500)
				nGia = 500;
			Player[nIdx].m_ItemList.m_Items[q2].nPrice = nGia;
			Item[nNew].m_CommonAttrib.uPrice = nGia;
			nBay++;
		}

		if (s_sapTenSo > 0)
			strncpy(Npc[nNpcIdx].ShopName, s_sapTen[nLech % s_sapTenSo], 31);
		else
			strncpy(Npc[nNpcIdx].ShopName, "Sap tap hoa nho", 31);
		Npc[nNpcIdx].ShopName[31] = 0;
		Npc[nNpcIdx].m_BaiTan = 1;
		Npc[nNpcIdx].SendCommand(do_stand);
		b.nBanSapNgoi = 0;             // ep ngoi lai o nhanh duoi
		pb_Log("[BotSap] %s mo sap '%s' tai map %d (%d mon trang suc co opt)\n",
		       Player[nIdx].m_PlayerName, Npc[nNpcIdx].ShopName, nha.nMap, nBay);
	}

	// (19/08 toi chu game) NGUOI BAN SAP PHAI NGOI XEP BANG, KHONG CUOI NGUA.
	// Khuon SC_SetBotSit (KSimCity.cpp:1455): ha m_nTimeIdleValue roi do_sit.
	if (Npc[nNpcIdx].m_bRideHorse)
		Player[nIdx].CheckRideHorse(TRUE);        // TRUE = XUONG ngua
	if (!b.nBanSapNgoi || Npc[nNpcIdx].m_Doing != do_sit)
	{
		if (nowT - b.nDaTauTick >= (unsigned int)(GAME_FPS * 5))
		{
			b.nDaTauTick  = nowT;
			b.nBanSapNgoi = 1;
			Npc[nNpcIdx].m_nTimeIdleValue = 0;
			Npc[nNpcIdx].SendCommand(do_sit);
		}
	}
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
#define PB_BAI_TRAN  120   // tran bot cho MOI bai luyen (gian dam dong 18/08)

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

	// GOM CA BAC KE DUOI (18/08 toi: 1000 bot cung lua cap don het vao 1-2 map cua
	// dung bac minh -> nghen than nhau + nung nong region keo ca quai day + client
	// ve vai tram nguoi mot man hinh = "rat lag"). Lay them bai bac duoi (quai yeu
	// hon chut van an exp) de so map gian ~gap doi, kem TRAN PB_BAI_TRAN bot/bai.
	int aTm[PB_SO_BAI], n = 0;
	for (int i = 0; i < PB_SO_BAI; i++)
		if (s_bai[i].nCapToiThieu == nMoc
		 || (nMoc > 10 && s_bai[i].nCapToiThieu >= nMoc - 10
		     && s_bai[i].nCapToiThieu < nMoc))
			aTm[n++] = i;
	if (n <= 0)
		return -1;

	// (19/08 chieu - chu game: "chia deu cac map cung cap khong gom mot map")
	// Truoc day lay BAI DAU TIEN con duoi tran ke tu diem boc -> nghin con don vao
	// 1-2 map dau danh sach. Nay CHIA DEU: vong 1 chon bai IT BOT NHAT trong cac
	// ung vien DUNG BAC; ca bac cham tran PB_BAI_TRAN thi vong 2 mo xuong ca ung
	// vien bac ke duoi (van lay it nhat). Diem boc xoay nBat giu vai tro xe hoa
	// khi nhieu bai dong so bot.
	const int nBat = ((int)g_Random(n) + nLech * 7) % n;
	int nChon = -1, nDongChon = 0x7fffffff;
	for (int v = 0; v < n; v++)
	{
		const int nUng = aTm[(nBat + v) % n];
		if (s_bai[nUng].nCapToiThieu != nMoc)
			continue;                   // vong 1: chi bai DUNG bac
		int nDong = 0;
		for (int q = 0; q < s_botCount; q++)
			if (s_bots[q].nBaiIdx == nUng)
				nDong++;
		if (nDong < nDongChon)
		{
			nDongChon = nDong;
			nChon     = nUng;
		}
	}
	if (nChon >= 0 && nDongChon < PB_BAI_TRAN)
		return nChon;
	// bai dung bac day het (hoac hiem khi khong co): xet MOI ung vien ke ca bac duoi
	for (int v = 0; v < n; v++)
	{
		const int nUng = aTm[(nBat + v) % n];
		int nDong = 0;
		for (int q = 0; q < s_botCount; q++)
			if (s_bots[q].nBaiIdx == nUng)
				nDong++;
		if (nDong < nDongChon)
		{
			nDongChon = nDong;
			nChon     = nUng;
		}
	}
	return (nChon >= 0) ? nChon : aTm[nBat];
}

#define PB_VISION_MPS     700          // tam tim quai
#define PB_CAST_GAP       4            // so khung toi thieu giua hai lenh danh
#define PB_LAG_SECONDS    10           // bo muc tieu neu chung nay giay khong sut 10% mau

// ---------------------------------------------------------------------------
// SO DEN MUC TIEU. Bai hoc tu log that (bot.log 18/08 09:14): bot bo muc tieu 5222 vi
// "10 giay khong sut duoc mau", roi pb_FindTarget lai chon DUNG con 5222 vi no van la
// con gan nhat - lap vo han, bot dung im ca phut. Bo muc tieu ma khong cam thi vo nghia.
// Cam 45 giay roi tha (quai co the da doi cho / bot da doi vi tri).
// ---------------------------------------------------------------------------
#define PB_CAM_GIAY   45

static void pb_CamMucTieu(PB_Bot& b, int t, unsigned int now)
{
	int k = 0;
	for (int i = 1; i < 8; i++)
		if (b.aCamTick[i] < b.aCamTick[k]) k = i;   // de len o cu nhat
	b.aCam[k]     = t;
	b.aCamTick[k] = now;
}

static bool pb_BiCam(const PB_Bot& b, int t, unsigned int now)
{
	for (int i = 0; i < 8; i++)
		if (b.aCam[i] == t && now - b.aCamTick[i] < (unsigned int)(GAME_FPS * PB_CAM_GIAY))
			return true;
	return false;
}

// Tim quai GAN NHAT quanh bot.
//
// CO Y KHONG chep NpcSet.AutoGetNpcNear cua ban client (KNpcSet.cpp:1126): ham do quet TOAN BO
// m_UseIdx - o client thi khong sao vi client chi giu vai tram NPC quanh minh, nhung o server
// la duyet CA THE GIOI moi khung cho MOI bot. Thay bang quet theo REGION: region cua bot +
// 8 region ke, dung m_NpcList co san (KRegion.h:35).
static int pb_FindTarget(int nNpcIdx, int nVision, const PB_Bot& b, unsigned int now)
{
	const int nSub = Npc[nNpcIdx].m_SubWorldIndex;
	const int nReg = Npc[nNpcIdx].m_RegionIndex;
	if (nSub < 0 || nSub >= MAX_SUBWORLD || nReg < 0)
		return 0;

	int bx = 0, by = 0;
	Npc[nNpcIdx].GetMpsPos(&bx, &by);

	// TOP-8 ung vien gan nhat, roi MOI BOT CHON MOT CON KHAC NHAU theo chi so cua no.
	// Bai hoc tu bot.log 10:37: 18 con cung chon "gan nhat tuyet doi" -> ca dam do ve
	// 4-5 con quai (5100/5102/5103/5139), chong cui len nhau thanh cum o(2554-2557,
	// 3479-3483) - NPC la tuong nen giat tai cho. Hai con thoat duoc chinh la hai con
	// tinh co boc trung muc tieu lech dan.
	int aId[8], aD[8];
	int nCand = 0;
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
			// Dot nay bot CHI danh quai; nguoi choi va bot khac de dot PK rieng.
			if (Npc[i].m_Kind == kind_player)                continue;
			if (!(NpcSet.GetRelation(nNpcIdx, i) & relation_enemy)) continue;
			if (pb_BiCam(b, i, now))                         continue;

			int ex = 0, ey = 0;
			Npc[i].GetMpsPos(&ex, &ey);
			const int d = g_GetDistance(bx, by, ex, ey);
			if (d >= nVision)
				continue;
			if (nCand < 8)
			{
				int pos = nCand++;
				while (pos > 0 && aD[pos - 1] > d)
				{
					aId[pos] = aId[pos - 1]; aD[pos] = aD[pos - 1]; pos--;
				}
				aId[pos] = i; aD[pos] = d;
			}
			else if (d < aD[7])
			{
				int pos = 7;
				while (pos > 0 && aD[pos - 1] > d)
				{
					aId[pos] = aId[pos - 1]; aD[pos] = aD[pos - 1]; pos--;
				}
				aId[pos] = i; aD[pos] = d;
			}
		}
	}
	if (nCand <= 0)
		return 0;

	// chi so bot = vi tri trong s_bots (b la tham chieu vao mang do)
	const int nLech = (int)(&b - &s_bots[0]);
	return aId[(unsigned)nLech % (unsigned)nCand];
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

	int nBest = 0, nBestLv = 0, nBestRank = 0, nBestRq = -1, nBestDmg = -1;
	// NHAT KY CHON CHIEU: liet ke TUNG chieu trong danh sach kem ly do loai, de doc
	// bot.log la biet ngay vi sao mot phai "khong co chieu de danh" - khong doan mo.
	//   HE=x  chieu mang ngu hanh x khac he bot     KIEU=x  style x khong phai Missles/Melee
	//   AURA  vong sang   SELF/KDICH  nham ban than / khong nham dich   EQT=x  can vu khi x
	char szD[560];
	int  nD = 0;
	szD[0] = 0;
#define PB_DIAG(...)  do { if (nD < (int)sizeof(szD) - 24) \
	nD += _snprintf(szD + nD, sizeof(szD) - nD - 1, __VA_ARGS__); } while (0)
	KSkillList& sl = Npc[nNpcIdx].m_SkillList;
	for (int i = 1; i < MAX_NPCSKILL; i++)
	{
		const int id = sl.m_Skills[i].SkillId;
		if (id <= 0 || id >= MAX_SKILL)
			continue;
		int lv = sl.m_Skills[i].CurrentSkillLevel;
		if (lv <= 0) lv = sl.m_Skills[i].SkillLevel;
		// CAP 0 van phai XET: hockynang hoc moi thu o cap 0 (SKILLNORMAL toan {id, 0},
		// factionhead.lua:161-173). Bo qua cap 0 thi khong bao gio chon duoc de nang cap.
		if (lv <= 0)
			lv = 1;

		KSkill* p = (KSkill*)g_SkillManager.GetSkill(id, lv);
		if (!p)                       { PB_DIAG(" %d:NULL", id); continue; }
		// Day an toan: chieu mang ngu hanh KHAC he bot = chieu cua phai khac (thua ke
		// hoac nhet nham) -> khong bao gio dung. RemoveAllSkill o tren da don goc,
		// luoi nay do phong blob mau doi sau nay.
		{
			const int nSr = p->GetSkillSeries();
			if (nSr >= 0 && nSr < series_num && nSr != Npc[nNpcIdx].m_Series)
			{ PB_DIAG(" %d:HE=%d", id, nSr); continue; }
		}
		// 29 dong du lieu co AttackRadius = 0 - chon phai la te liet (cong engine
		// "dist <= 0 + 20" gan nhu khong bao gio dat) -> loai thang (phan bien 18/08)
		if (p->GetAttackRadius() <= 0) { PB_DIAG(" %d:TAM0", id); continue; }
		if (p->IsAura())              { PB_DIAG(" %d:AURA", id);  continue; }
		if (p->IsTargetSelf())        { PB_DIAG(" %d:SELF", id);  continue; }
		if (!p->IsTargetEnemy())      { PB_DIAG(" %d:KDICH", id); continue; }
		// CHI giu chieu SAT THUONG THAT (dan bay / can chien). Ban tham khao lam dung
		// vay va ghi ro ly do (RepickBotCombatSkills, KBotManager.cpp:1246): chieu
		// TRANG THAI (khong che / doc / lam cham) thuong hop MOI vu khi + cap yeu cau
		// cao nen DANH BAI chieu that trong bang xep hang - bot cast no thi khong co
		// sat thuong, nhin nhu DUNG YEN hoac quay ve dam tay. Chinh la ba trieu chung
		// chu game bat 18/08: Duong Mon dung im, Thieu Lam bong dung im, Cai Bang
		// quyen danh tay.
		{
			const int nSt = p->GetSkillStyle();
			if (nSt != SKILL_SS_Missles && nSt != SKILL_SS_Melee)
			{ PB_DIAG(" %d:KIEU=%d", id, nSt); continue; }
		}

		const int eq = p->GetEquipLimit();
		int nRank = 0;
		if (eq == -2)          nRank = 1;         // moi vu khi
		else if (eq == nWant)  nRank = 2;         // khop dung ho -> an dut
		else                   { PB_DIAG(" %d:EQT=%d", id, eq); continue; }

		// BAC XEP HANG THU HAI: cap yeu cau (rqTier) - hoc tu bot.log 11:51 doi chieu
		// skills.txt cua DU AN: id 1/2/53 deu la "Cong kich vat ly" (DON DANH THUONG,
		// reqLevel 0, EqtLimit -2) va nam DAU danh sach ky nang, nen luat hoa cu
		// "cung rank cung cap -> lay con dung truoc" khien bot tay khong cast chieu 53
		// = dam tay dung nghia den, du danh sach CO san chieu phai that (122/145/85...
		// deu OK,r1). Xep them bac reqLevel (kep 80) thi chieu phai (rq >= 10) luon
		// thang don danh thuong (rq 0) - dung khuon rqTier cua ban tham khao
		// (RepickBotCombatSkills, KBotManager.cpp:1262-1270).
		int nRq = p->GetSkillReqLevel();
		if (nRq > 80) nRq = 80;
		// CHOT toi bo sot lan truoc, ap NGUYEN VAN ban tham khao (KBotManager.cpp:1243):
		// "bo neu nBotLevel < rqTier" - khong co no thi bac rqTier uu tien chieu rq CAO
		// NHAT ke ca chieu bot CHUA DU CAP dung -> cast roi im lang, KHONG PHAI NAO danh
		// duoc ca (chu game bat 18/08 ngay sau ban rqTier).
		if (Npc[nNpcIdx].m_Level < nRq)
		{ PB_DIAG(" %d:CAP=%d", id, nRq); continue; }
		// UU TIEN chieu CO DON SAT THUONG (khuon DealsDamage cua ban tham khao,
		// KBotManager.cpp:1247). Lam dang UU TIEN chu khong chan cung: neu ca danh
		// sach deu khong co don sat thuong thi van chon duoc, khong tai dien canh
		// "khong phai nao danh duoc".
		const int nDmg = (p->GetDamageAttribsNum() > 0) ? 1 : 0;
		PB_DIAG(" %d:OK(lv%d,r%d,rq%d,d%d)", id, lv, nRank, nRq, nDmg);

		if (nRank > nBestRank
		 || (nRank == nBestRank && nDmg > nBestDmg)
		 || (nRank == nBestRank && nDmg == nBestDmg && nRq > nBestRq)
		 || (nRank == nBestRank && nDmg == nBestDmg && nRq == nBestRq && lv > nBestLv))
		{
			nBestRank = nRank;
			nBestDmg  = nDmg;
			nBestRq   = nRq;
			nBest     = id;
			nBestLv   = lv;
		}
	}
	// 1000 bot: chi in nhat ky chon chieu khi THAT BAI (nBest = 0) - truong hop
	// thanh cong da duoc kiem chung xong 18/08 sang, in nua la ngap log.
	if (nBest == 0)
		pb_Log("[BotChon] %s (he %d, can vu khi %d):%s -> chon %d\n",
		       Player[nIdx].m_PlayerName, (int)Npc[nNpcIdx].m_Series, nWant,
		       szD[0] ? szD : " (danh sach rong)", nBest);
#undef PB_DIAG
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

	// DAN HANG truoc khi doi map: nDoiMapTick giu "moc som nhat duoc phep thu".
	//
	// VI SAO: ca 20 bot cung nhan lenh danh quai mot luc -> cung ChangeWorld toi CUNG MOT
	// O cung mot khung. JX1 tinh NPC LA TUONG, nen 20 than chong mot o thi buoc chan dau
	// tien cua moi con deu bi 19 con kia chan - mot lan ne roi DoStand, phat lai lenh lai
	// dung tiep => nhin nhu "di toi roi giat lui ve vi tri cu" (dung loi chu game ta).
	// Moi con cham hon con truoc 1 giay: con truoc da BUOC RA khoi diem dat chan truoc
	// khi con sau dap xuong.
	// 18/08 toi: nLech * 1 giay thiet ke cho 20 bot (toi da 20s). Voi 1000 bot,
	// bot cuoi cho ~17 PHUT -> "chi len map vai acc, con dung thanh rat nhieu".
	// Ep ve cua so 60 giay: ~17 bot doi map moi giay, van du gian de khong chong o.
	if (b.nDoiMapTick == 0)
		b.nDoiMapTick = now + (unsigned int)((nLech % 60) * GAME_FPS);
	if (now < b.nDoiMapTick)
		return 0;
	b.nDoiMapTick = now + (unsigned int)(GAME_FPS * 10);   // lan thu ke tiep (neu that bai)

	// DU CAP NHAY MAP -> tu thoat party (thanh vien) / giai tan nhom (doi truong)
	// - dung nguyen loi chu game dan 18/08. Lam TRUOC ChangeWorld de dong doi
	// khong bam theo sang map moi.
	if (pb_TrongNhom(nIdx))
		pb_RoiNhom(nIdx, "du cap doi map luyen");

	b.nTargetNpc = 0;
	b.walk.Reset();
	// Dat chan LECH NHAU: moi bot mot o rieng quanh diem den (luoi 5x4, cach nhau 2-3 o)
	// de khong bao gio co hai con chong mot o. O lech bi chan thi lui ve dung diem goc.
	// luoi 9x7 = 63 o rieng (truoc 5x4 = 20 - du cho 20 bot, 1000 bot thi 50 con
	// chong mot o). O lech bi chan thi nhanh duoi tu lui ve diem goc nhu cu.
	// (19/08 toi) O LECH PHAI LA O TRONG: truoc day cong thang +-8 o roi
	// ChangeWorld - ChangeWorld chi kiem KHUNG region chu KHONG kiem vat can,
	// nen o ban do hang lang hep (map 79 Tuong Duong Mat Dao) bot dap thang vao
	// TRONG TUONG roi ket vinh vien ("bot ket o tuong" chu game bao).
	const int nDx = ((nLech % 9) - 4) * 2;
	const int nDy = (((nLech / 9) % 7) - 3) * 3;
	const int nSubBai = g_SubWorldSet.SearchWorld(bai.nMapId);
	int nRet = 0;
	if (nSubBai >= 0)
	{
		int nDx2 = 0, nDy2 = 0;
		if (pb_ODat(nSubBai, bai.nOX + nDx, bai.nOY + nDy, nLech, 10, &nDx2, &nDy2))
			nRet = Npc[nNpcIdx].ChangeWorld(bai.nMapId, nDx2, nDy2);
		if (nRet != 1 && pb_ODat(nSubBai, bai.nOX, bai.nOY, nLech, 16, &nDx2, &nDy2))
			nRet = Npc[nNpcIdx].ChangeWorld(bai.nMapId, nDx2, nDy2);
	}
	if (nRet != 1)
		nRet = Npc[nNpcIdx].ChangeWorld(bai.nMapId, bai.nOX * 32, bai.nOY * 32);
	if (nRet == 1)
	{
		// Diem dat chan la CHO MOI NGUOI CHOI CUNG DAT CHAN - thuong sat trap vao/ra.
		// Dung nguyen do la bot ke trap ("khong ra khoi trap duoc"). Buoc ra 8-11 o
		// theo huong ngau nhien NGAY khi toi noi, truoc khi lam bat cu viec gi khac.
		{
			const int nXa   = 8 + (int)g_Random(4);
			const int nGocX = ((int)g_Random(2) * 2 - 1);
			const int nGocY = ((int)g_Random(2) * 2 - 1);
			// diem buoc ra cung phai la O TRONG (khong thi PB_WalkTo tra -1 hoai)
			int nRx = 0, nRy = 0;
			if (nSubBai >= 0
			 && pb_ODat(nSubBai, bai.nOX + nGocX * nXa, bai.nOY + nGocY * nXa,
			            nLech, 8, &nRx, &nRy))
			{
				b.nBuocRaX = nRx;
				b.nBuocRaY = nRy;
			}
			else
			{
				b.nBuocRaX = 0;
				b.nBuocRaY = 0;
			}
			b.roam.Reset();
		}
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
static int pb_FindRoamSpot(int nIdx, int nNpcIdx, int nSub, int nLech, int* pnX, int* pnY)
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

	// (19/08 trua - chu game) DOI TRUONG nhom chon diem VANG BOT nhat: dem bot trong
	// ~20 o quanh tung ung vien, lay it nhat (hoa nhau lay diem XA minh nhat) -> ca
	// doi bam theo doi truong, cac nhom tu dan rai toan map thay vi gom mot cho.
	{
		if (pb_TrongNhom(nIdx) && pb_DoiTruongCua(nIdx) == nIdx)
		{
			int nTot = -1;
			int nItNhat = 0x7fffffff;
			__int64 nXaNhat = -1;
			for (int c = 0; c < nCand; c++)
			{
				int nDong = 0;
				for (int q = 0; q < s_botCount; q++)
				{
					const int p = s_bots[q].nPlayerIdx;
					if (p <= 0 || p >= MAX_PLAYER) continue;
					const int nn = Player[p].m_nIndex;
					if (nn <= 0 || nn >= MAX_NPC) continue;
					if (Npc[nn].m_SubWorldIndex != nSub) continue;
					int qx = 0, qy = 0;
					Npc[nn].GetMpsPos(&qx, &qy);
					int ddx = qx - aX[c]; if (ddx < 0) ddx = -ddx;
					int ddy = qy - aY[c]; if (ddy < 0) ddy = -ddy;
					if (ddx <= 640 && ddy <= 640)
						nDong++;
				}
				const __int64 dxa = (__int64)(aX[c] - bx) * (aX[c] - bx)
				                 + (__int64)(aY[c] - by) * (aY[c] - by);
				if (nDong < nItNhat || (nDong == nItNhat && dxa > nXaNhat))
				{
					nItNhat = nDong;
					nXaNhat = dxa;
					nTot    = c;
				}
			}
			if (nTot >= 0)
			{
				*pnX = aX[nTot];
				*pnY = aY[nTot];
				return 1;
			}
		}
	}

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
//  2. BUA / BI DONG (SKILL_SS_PassivityNpcState, SkillDef.h:105) - KHONG DUNG
//     Ban tham khao co cast lai moi khi len cap (ReapplyEligibleBotPassives), nhung
//     chu game chot BO cho bot: "bua thi khong cho bot dung vi khong can thiet".
//     Day la QUYET DINH, khong phai bo sot - dung tu y them lai.
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

	// Gom TAT CA vong sang bot du cap, giu 4 cai co cap yeu cau cao nhat.
	int aAura[4] = { 0, 0, 0, 0 };
	int aRq[4]   = { -1, -1, -1, -1 };
	int nAuraNum = 0;
	int nBuff = 0;
	KSkillList& sl = Npc[nNpcIdx].m_SkillList;

	for (int i = 1; i < MAX_NPCSKILL; i++)
	{
		const int id = sl.m_Skills[i].SkillId;
		if (id <= 0 || id >= MAX_SKILL)
			continue;
		int lv = sl.m_Skills[i].CurrentSkillLevel;
		if (lv <= 0) lv = sl.m_Skills[i].SkillLevel;
		// CAP 0 van phai XET: hockynang hoc moi thu o cap 0 (SKILLNORMAL toan {id, 0},
		// factionhead.lua:161-173). Bo qua cap 0 thi khong bao gio chon duoc de nang cap.
		if (lv <= 0)
			lv = 1;

		KSkill* p = (KSkill*)g_SkillManager.GetSkill(id, lv);
		if (!p)
			continue;

		// khong dung vong sang / buff cua phai khac (thua ke tu nhan vat mau)
		{
			const int nSr3 = p->GetSkillSeries();
			if (nSr3 >= 0 && nSr3 < series_num && nSr3 != Npc[nNpcIdx].m_Series)
				continue;
		}

		// Cap yeu cau kep tran 80 y ban tham khao (tren nua la ky nang 90/150).
		int rq = p->GetSkillReqLevel();
		if (rq > 80) rq = 80;
		if (nLevel < rq)
			continue;

		if (p->IsAura())
		{
			// chen theo thu tu cap yeu cau GIAM DAN, giu 4 cai dau
			for (int k = 0; k < 4; k++)
			{
				if (rq > aRq[k])
				{
					for (int m = 3; m > k; m--) { aRq[m] = aRq[m-1]; aAura[m] = aAura[m-1]; }
					aRq[k]   = rq;
					aAura[k] = id;
					if (nAuraNum < 4) nAuraNum++;
					break;
				}
			}
			continue;
		}

		// BUA / BI DONG (SKILL_SS_PassivityNpcState) CO Y KHONG DUNG:
		// chu game chot "bua thi khong cho bot dung vi khong can thiet".
		// Giu chu thich nay de dot sau khong ai tuong la bi bo sot.
		const int st = p->GetSkillStyle();
		if (st == SKILL_SS_InitiativeNpcState && p->IsTargetSelf())
		{
			p->Cast(nNpcIdx, -1, nNpcIdx);
			nBuff++;
		}
	}

	// ---- chon vong sang ----
	//
	// Engine chi giu DUOC MOT vong sang mot luc: m_ActiveAuraID la mot so nguyen
	// (KNpc.h:289). Nen "hai vong sang" chi co the la LUAN CHUYEN theo thoi gian,
	// khong phai bat dong thoi - va do cung la y chu game.
	//
	// RIENG NGA MI (phai 4): phai nay co nhieu vong sang mo dan theo cap. Neu bot da du
	// cap cho tu 2 cai tro len thi doi qua doi lai moi ky buff, thay vi om mai mot cai.
	// Cac phai khac giu nguyen: luon dung cai co cap yeu cau cao nhat.
	int nChon = 0;
	if (b.nFaction == 4 && nAuraNum >= 2)
	{
		b.nAuraPhien = (b.nAuraPhien + 1) & 1;
		nChon = b.nAuraPhien;
	}

	if (aAura[nChon] > 0 && aAura[nChon] != b.nAuraSkill)
	{
		b.nAuraSkill = aAura[nChon];
		if (b.nFaction == 4 && nAuraNum >= 2)
			pb_Log("[BotVongSang] %s (Nga Mi) cap %d luan chuyen -> vong sang %d"
				   " (co %d cai du cap)\n",
				   Player[nIdx].m_PlayerName, nLevel, b.nAuraSkill, nAuraNum);
		else
			pb_Log("[BotVongSang] %s cap %d bat vong sang %d\n",
				   Player[nIdx].m_PlayerName, nLevel, b.nAuraSkill);
	}
	if (b.nAuraSkill > 0)
		Npc[nNpcIdx].SetAuraSkill(b.nAuraSkill);

	if (bLenCap && nBuff)
		pb_Log("[BotBuff] %s cap %d: %d buff (bua bi dong: KHONG dung)\n",
			   Player[nIdx].m_PlayerName, nLevel, nBuff);
}

// ===========================================================================
// DUNG VAT PHAM: uong binh khi thieu mau, mo tui duoc pham khi sap het binh,
// va giu buff Tien Thao Lo (x2 kinh nghiem).
//
// Du lieu that (trich tu script/settings, dot khao sat 18/08):
//   Tui duoc pham : Genre 6 (item_magicscript), Detail 1, Particular 4813
//                   (magicscript.txt:4815). Mo bang EatMecidine -> chay
//                   tuiduocpham.lua: rot 20 binh/luot, tu cooldown 10 giay,
//                   doi cap >= 10.
//   Binh mau      : "Thua Tien Mat (Dai)" Genre 1 (item_medicine) - hoi 150HP+150MP.
//   Tien Thao Lo  : buff = skill 440 cap 1 (skills.txt:441), 1 gio; script goc
//                   xiancaolu.lua:19-25 chi cho an khi GetNpcExpRate() <= 100.
//
// Nguong "con <= 2 binh thi mo tui" theo dung ban tham khao (KBotAutoAI_DaTau.cpp:110
// kPouchRefillBelow = 3 - tuc duoi 3 binh la rot them).
// ===========================================================================
static void pb_DungVatPham(int nIdx, int nNpcIdx, PB_Bot& b, unsigned int now, int nLech)
{
	// nhe nhang: 2 giay xet mot lan, lech nhip theo bot
	if (((now + nLech) % (unsigned int)(GAME_FPS * 2)) != 0)
		return;

	// ---- giu buff Tien Thao Lo ----
	// m_CurrentExpEnhance goc = 100; buff 440 day len tren 100. Het buff thi cast lai
	// (di dung duong LuaAddSkillState: CastStateSkill, ScriptFuns.cpp:11829).
	if (s_nPbChamBuff && Npc[nNpcIdx].m_CurrentExpEnhance <= 100)
	{
		KSkill* pTTL = (KSkill*)g_SkillManager.GetSkill(440, 1);
		if (pTTL)
		{
			pTTL->CastStateSkill(nNpcIdx, 0, 0, 3600 * GAME_FPS, TRUE);
			pb_Log("[BotTTL] %s an Tien Thao Lo (x2 kinh nghiem, 1 gio)\n",
			       Player[nIdx].m_PlayerName);
		}
	}

	// ---- giu buff Que Hoa Tuu (+20 may man 30 phut - chu game 19/08) ----
	// Item that (magicscript.txt:126, genre 6/detail 1/parti 124) chay
	// \script\item\ghj.lua -> AddSkillState(450, 1, 1, 30*60*18, -1) va DOI phai
	// co to doi. Bot cham thang trang thai nhu Tien Thao Lo o tren (khoi can item,
	// khoi can nhom); khong co cach doc "con buff hay khong" re tien nen lam moi
	// theo dong ho: som 1 phut truoc han 30 phut.
	if (s_nPbChamBuff
	 && (b.nQhtTick == 0 || now - b.nQhtTick >= (unsigned int)(GAME_FPS * 60 * 29)))
	{
		b.nQhtTick = now;
		KSkill* pQht = (KSkill*)g_SkillManager.GetSkill(450, 1);
		if (pQht)
		{
			pQht->CastStateSkill(nNpcIdx, 0, 0, 30 * 60 * GAME_FPS, TRUE);
			pb_Log("[BotQHT] %s cham Que Hoa Tuu (+20 may man, 30 phut)\n",
			       Player[nIdx].m_PlayerName);
		}
	}

	// ---- dem binh + tim tui trong hanh trang ----
	int nBinh = 0, nBinhIdx = 0, nTuiIdx = 0;
	for (int q = 0; q < MAX_PLAYER_ITEM; q++)
	{
		const int g = Player[nIdx].m_ItemList.m_Items[q].nIdx;
		if (g <= 0)
			continue;
		const int nGenre = Item[g].GetGenre();
		if (nGenre == item_medicine)
		{
			nBinh++;
			if (!nBinhIdx) nBinhIdx = g;
		}
		else if (nGenre == item_magicscript && Item[g].GetDetailType() == 1
		      && Item[g].GetParticular() == 4813)
			nTuiIdx = g;
	}

	// ---- uong binh khi mau duoi 50% ----
	if (nBinhIdx && Npc[nNpcIdx].m_CurrentLife * 2 < Npc[nNpcIdx].m_CurrentLifeMax
	 && now - b.nUongTick >= (unsigned int)(GAME_FPS * 5))
	{
		b.nUongTick = now;
		Player[nIdx].m_ItemList.EatMecidine(nBinhIdx);
		pb_Log("[BotUong] %s uong binh (HP %d/%d, con %d binh)\n",
		       Player[nIdx].m_PlayerName, (int)Npc[nNpcIdx].m_CurrentLife,
		       (int)Npc[nNpcIdx].m_CurrentLifeMax, nBinh - 1);
	}

	// ---- con it hon 3 binh thi mo tui rot them (script tu gioi han 10 giay/luot) ----
	if (nTuiIdx && nBinh < 3 && now - b.nMoTuiTick >= (unsigned int)(GAME_FPS * 12))
	{
		b.nMoTuiTick = now;
		Player[nIdx].m_ItemList.EatMecidine(nTuiIdx);
		pb_Log("[BotTui] %s mo tui duoc pham (dang con %d binh)\n",
		       Player[nIdx].m_PlayerName, nBinh);
	}
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
		if (!pb_FindRoamSpot(nIdx, nNpcIdx, nSub, nLech, &rx, &ry))
			return 0;              // ban do het quai - dung yen cho hoi sinh
		b.nRoamX = rx;
		b.nRoamY = ry;
		b.roam.Reset();
		// (19/08 trua) co ngua thi cuoi chay duong xa nhu nguoi that; toi noi
		// chieu cam-tren-ngua se tu xuong o ngay truoc luc ra don (khoi bi
		// CanCastSkill tu choi im lang).
		if (!Npc[nNpcIdx].m_bRideHorse
		 && Player[nIdx].m_ItemList.GetEquipment(itempart_horse) > 0)
			Player[nIdx].CheckRideHorse(FALSE);
		{
			static unsigned int s_uHoangLog = 0;
			if (now - s_uHoangLog >= (unsigned int)(GAME_FPS / 2))
			{
				s_uHoangLog = now;
				pb_Log("[BotHoang] %s het quai gan -> di toi o %d,%d (map %d)\n",
				       Player[nIdx].m_PlayerName, rx / 32, ry / 32, SubWorld[nSub].m_SubWorldID);
			}
		}
	}

	// Dung sat quai la du - toi noi thi vong danh se tu bat duoc muc tieu.
	const int nRet = PB_WalkTo(nNpcIdx, b.nRoamX, b.nRoamY, nSub, b.roam, 400);
	if (nRet == 0)
	{
		b.nAStarThua = 0;          // dang di duoc = khong ket
		return 1;                  // dang di
	}

	if (nRet < 0)
	{
		b.nAStarThua++;            // (19/08 chieu) thua lien tiep -> [BotCuu] xet
		static unsigned int s_uHoangLog2 = 0;
		if (now - s_uHoangLog2 >= (unsigned int)(GAME_FPS / 2))
		{
			s_uHoangLog2 = now;
			pb_Log("[BotHoang] %s khong tim duoc duong toi o %d,%d - boc cho khac\n",
			       Player[nIdx].m_PlayerName, b.nRoamX / 32, b.nRoamY / 32);
		}
	}
	else
		b.nAStarThua = 0;          // toi noi binh thuong = duong van tot

	b.nRoamX = 0;                  // toi noi hoac tac duong -> lan sau boc cho khac
	b.nRoamY = 0;
	b.roam.Reset();
	return 0;
}

// ===========================================================================
// NHAT DO ROI + DON TUI (chu game 18/08: "bot nhat do khi npc rot ra roi xoa di
// chi de lai tui mau - binh mau mana - tho dia phu - than hanh phu - tien thao lo").
//
// Nhat qua DUNG cong nguoi choi that: KPlayer::ServerPickUpItem (KPlayer.cpp:4656)
// - tu kiem chu quyen (m_nBelong), cung ban do, khoang cach <= 200 MPS (bin phuong
//   PLAYER_PICKUP_SERVER_DISTANCE = 40000, GameDataDef.h:28), roi AddKIL vao tui.
// - goi tin PLAYER_PICKUP_ITEM_COMMAND can O TRONG cu the trong tui: tim bang
//   m_Room[room_equipment].FindEmptyPlace (KInventory.h:36) y nhu client lam.
// KHONG dung KObjSet::SearchObjAt - ham do quy doi qua toa do VIEWPORT cua client
// (KObjSet.cpp:999), phia server khong co viewport nen ket qua sai. Tu quet
// ObjSet.GetNext (cong khai, KObjSet.cpp:1216) va loc bang toa do MPS that.
//
// Sau khi nhat: don tui theo DANH SACH GIU do chu game chot -
//   giu  genre item_medicine (=1)      : moi loai binh/thuoc HP-MP
//   giu  genre item_magicscript (=6) detail 1, particular thuoc bang s_nGiuPhu
//   xoa  moi thu khac dang nam trong TUI (pos_equiproom) - khong dung den do
//        dang mac tren nguoi (pos_equip) hay vu khi dang cam.

// particular cua cac vat pham DUOC GIU (magicscript.txt, chi so dong - 1, da doi
// chieu tung file lua: tuiduocpham 4813 / thodiaphuvh 437 / shenxingfu 1271 /
// tien thao lo 70+71+1182):
// 4821 = Tui duoc pham HOAT DONG (tuiduocphamtk.lua) - chu game chot giu 18/08.
static const int s_nGiuPhu[] = { 4813, 4821, 437, 1271, 70, 71, 1182 };

// ---- do hieu nang (dieu tra "lag hon nhieu" 18/08) ----
extern int g_nPbAstarDem;              // KSubWorld.cpp: so lan A* chay
static int s_nPerfQuetDo = 0;          // so lan quet do roi toan ObjSet

#define PB_LOOT_VISION    640          // chi nhat do roi trong 20 o quanh bot
#define PB_LOOT_SCAN_GAP  (GAME_FPS * 6)   // quet nhan khi ranh; giet xong van quet NGAY
#define PB_LOOT_PICK_MPS  150          // vao sat co nay moi phat lenh nhat (cong 200)
#define PB_DONTUI_GAP     (GAME_FPS * 12)

// Ne mot obj nhat truot trong 30 giay - vong 4 o, de len o cu nhat.
static void pb_NeDo(PB_Bot& b, int nObjID, unsigned int now)
{
	int k = 0;
	for (int i = 1; i < 4; i++)
		if (b.aLootBanTick[i] < b.aLootBanTick[k]) k = i;
	b.aLootBan[k]     = nObjID;
	b.aLootBanTick[k] = now;
}

static bool pb_DoBiNe(const PB_Bot& b, int nObjID, unsigned int now)
{
	for (int i = 0; i < 4; i++)
		if (b.aLootBan[i] == nObjID
		 && now - b.aLootBanTick[i] < (unsigned int)(GAME_FPS * 30))
			return true;
	return false;
}

static void pb_DonTui(int nIdx, PB_Bot& b, unsigned int now)
{
	if (b.nDonTuiTick && now - b.nDonTuiTick < (unsigned int)PB_DONTUI_GAP)
		return;
	b.nDonTuiTick = now;

	int nXoa = 0;
	for (int q = 1; q < MAX_PLAYER_ITEM; q++)
	{
		const int g = Player[nIdx].m_ItemList.m_Items[q].nIdx;
		if (g <= 0 || g >= MAX_ITEM)
			continue;
		// CHI dong den do nam trong TUI - khong dung do dang mac / dang cam
		if (Player[nIdx].m_ItemList.m_Items[q].nPlace != pos_equiproom)
			continue;
		if (Player[nIdx].m_ItemList.m_Items[q].nPrice > 0)
			continue;                                   // hang dang bay ban tren sap (19/08)
		const int nGe = Item[g].GetGenre();
		if (nGe == item_medicine)
			continue;                                   // binh mau / binh mana
		if (nGe == item_task)
			continue;                                   // do nhiem vu: engine coi la KHONG THE
											            // MAT (KItem.h:252-266 khoa ban/vut cung)
		if (nGe == item_townportal)
			continue;                                   // Tho Dia Phu THUONG (TownPortal.txt, genre 5)
		if (nGe == item_mine && Item[g].GetDetailType() == 1
		 && Item[g].GetParticular() == 4)
			continue;                                   // Tho Dia phu VINH VIEN (mine.txt:6)
		if (nGe == item_magicscript && Item[g].GetDetailType() == 1)
		{
			const int nPa = Item[g].GetParticular();
			int bGiu = 0;
			for (int h = 0; h < (int)(sizeof(s_nGiuPhu) / sizeof(s_nGiuPhu[0])); h++)
				if (s_nGiuPhu[h] == nPa) { bGiu = 1; break; }
			if (bGiu)
				continue;
		}
		// so luong PHAI >= 1: RemoveItemIdx voi nNum = 0 la KHONG XOA GI (bay cu,
		// xem chu thich cho vu khi nhap mon o tren) - do khong xep chong tra 0.
		int nSo = Item[g].GetStackNum();
		if (nSo < 1)
			nSo = 1;
		if (Player[nIdx].m_ItemList.RemoveItemIdx(g, nSo))
			nXoa++;
	}
	if (nXoa)
		pb_Log("[BotXoaDo] %s don tui: xoa %d mon, giu binh thuoc + tui mau + phu + tien thao lo\n",
		       Player[nIdx].m_PlayerName, nXoa);
}

// Mot nhip NHAT DO. Tra 1 = dang ban (dang di toi / vua nhat), 0 = khong co gi nhat.
static int pb_NhatDo(int nIdx, int nNpcIdx, int nSub, PB_Bot& b)
{
	const unsigned int now = SubWorld[nSub].m_dwCurrentTime;

	// kiem lai obj dang nham: khe Object tai su dung rat nhanh nen phai doi chieu m_nID
	if (b.nLootObjIdx > 0)
	{
		KObj& o = Object[b.nLootObjIdx];
		// dataID = 0: object da bi nhat trong CUNG frame (ServerPickUpItem xoa dataID
		// nhung viec go khoi ObjSet cho den MessageLoop SAU PB_Breathe) - khong bat la
		// Item[0] W=0 -> tuong "tui day" gia (phan bien 18/08).
		if (o.m_nIndex <= 0 || o.m_nID != b.nLootObjID
		 || o.m_nSubWorldID != Npc[nNpcIdx].m_SubWorldIndex
		 || (o.m_nKind != Obj_Kind_Item && o.m_nKind != Obj_Kind_Money)
		 || (o.m_nKind == Obj_Kind_Item && o.m_nItemDataID <= 0))
		{
			b.nLootObjIdx = 0;
			b.loot.Reset();
		}
	}

	// quet tim do roi gan nhat (gion nhip 2 giay cho nhe)
	if (b.nLootObjIdx == 0)
	{
		if (b.nLootScanTick && now - b.nLootScanTick < (unsigned int)PB_LOOT_SCAN_GAP)
			return 0;
		b.nLootScanTick = now;
		s_nPerfQuetDo++;

		int bx = 0, by = 0;
		Npc[nNpcIdx].GetMpsPos(&bx, &by);
		int nGanNhat = PB_LOOT_VISION + 1;
		for (int i = ObjSet.GetNext(0); i != 0; i = ObjSet.GetNext(i))
		{
			KObj& o = Object[i];
			if (o.m_nIndex <= 0)
				continue;
			if (o.m_nKind != Obj_Kind_Item && o.m_nKind != Obj_Kind_Money)
				continue;
			if (o.m_nSubWorldID != Npc[nNpcIdx].m_SubWorldIndex)
				continue;
			if (o.m_nKind == Obj_Kind_Item
			 && (o.m_nItemDataID <= 0 || o.m_nItemDataID >= MAX_ITEM))
				continue;
			// CHI nhat do CUA MINH (quai minh giet, m_nBelong = chi so Player minh).
			// TUYET DOI khong nhat do vo chu (-1): do nguoi choi that VUT XUONG DAT
			// cung mang -1 ngay lap tuc (KPlayer.cpp:4997/5170) - khong chan thi bot
			// hut do trao tay cua nguoi that trong 2 giay (phan bien 18/08). Do het
			// han chu quyen (33s) cung ve -1 -> bo qua luon, do minh thi minh nhat
			// trong vong 2 giay roi.
			// (D22) CUON NHIEM VU DA TAU: nhat ke ca khi het chu quyen (33 giay ->
			// m_nBelong ve -1). Cuon la vat pham nhiem vu, nguoi that khong vut ra
			// dat bao gio, nen khong co chuyen "hut do trao tay" nhu do thuong.
			int bCuonNv = 0;
			if (o.m_nKind == Obj_Kind_Item && o.m_nItemDataID > 0
			 && o.m_nItemDataID < MAX_ITEM
			 && Item[o.m_nItemDataID].GetGenre() == item_magicscript
			 && Item[o.m_nItemDataID].GetDetailType() == 1
			 && (Item[o.m_nItemDataID].GetParticular() == 205
			  || Item[o.m_nItemDataID].GetParticular() == 212))
				bCuonNv = 1;
			if (o.m_nBelong != nIdx && !(bCuonNv && o.m_nBelong < 0))
				continue;
			if (pb_DoBiNe(b, o.m_nID, now))
				continue;                              // obj vua nhat truot - ne tam
			int ox = 0, oy = 0;
			o.GetMpsPos(&ox, &oy);
			const int nD = g_GetDistance(bx, by, ox, oy);
			if (nD < nGanNhat)
			{
				nGanNhat      = nD;
				b.nLootObjIdx = i;
				b.nLootObjID  = o.m_nID;
			}
		}
		if (b.nLootObjIdx == 0)
			return 0;
		b.loot.Reset();
	}

	KObj& o = Object[b.nLootObjIdx];
	int ox = 0, oy = 0;
	o.GetMpsPos(&ox, &oy);
	int bx = 0, by = 0;
	Npc[nNpcIdx].GetMpsPos(&bx, &by);

	// con xa -> di toi bang A* (khong bao gio chay thang - luat chong xuyen bien)
	if (g_GetDistance(bx, by, ox, oy) > PB_LOOT_PICK_MPS)
	{
		const int nW = PB_WalkTo(nNpcIdx, ox, oy, nSub, b.loot, 128);
		if (nW < 0)
		{
			pb_Log("[BotNhat] %s khong co duong A* toi do roi o(%d,%d) -> bo qua\n",
			       Player[nIdx].m_PlayerName, ox / 32, oy / 32);
			pb_NeDo(b, b.nLootObjID, now);
			b.nLootObjIdx  = 0;
			b.loot.Reset();
			return 0;
		}
		return 1;                                      // dang di toi (ke ca nW==1: nhip sau nhat)
	}

	// da vao sat: phat lenh nhat dung cong nguoi choi that
	PLAYER_PICKUP_ITEM_COMMAND pk;
	memset(&pk, 0, sizeof(pk));
	pk.m_nObjID = o.m_nID;
	const int nTienNho  = (o.m_nKind == Obj_Kind_Money) ? o.m_nMoneyNum : 0;
	const int nItemNho  = (o.m_nKind == Obj_Kind_Item) ? o.m_nItemDataID : 0;
	if (o.m_nKind == Obj_Kind_Item)
	{
		if (nItemNho <= 0 || nItemNho >= MAX_ITEM)
		{
			// object vua bi tieu trong cung frame - bo, khong phai "tui day"
			b.nLootObjIdx = 0;
			b.loot.Reset();
			return 1;
		}
		// Tim o trong bang FindRoom - ham engine THAT dung (SearchPosition,
		// KItemList.cpp:153). FindEmptyPlace dinh dung ban dau la HAM CHET chua ai
		// goi bao gio: tra toa do HOAN VI x/y va lech dung (cao, rong) (phan bien
		// 18/08 mo phong: mon thu 2 tro di truot sach, bot chi nhat duoc dung 1 mon).
		POINT pt;
		if (!Player[nIdx].m_ItemList.m_Room[room_equipment].FindRoom(
				Item[nItemNho].GetWidth(), Item[nItemNho].GetHeight(), &pt))
		{
			pb_Log("[BotNhat] %s tui DAY, khong nhat duoc -> don tui, ne mon nay 30s\n",
			       Player[nIdx].m_PlayerName);
			b.nDonTuiTick = 0;
			pb_DonTui(nIdx, b, now);
			// PHAI nha muc tieu + ne: giu nguyen la lap "don tui MOI NHIP" vinh vien
			// khi tui toan do thuoc danh sach giu (phan bien 18/08).
			pb_NeDo(b, b.nLootObjID, now);
			b.nLootObjIdx = 0;
			b.loot.Reset();
			return 1;
		}
		pk.m_btPosType = pos_equiproom;
		pk.m_btPosX    = (BYTE)pt.x;
		pk.m_btPosY    = (BYTE)pt.y;
	}
	const BOOL bOk = Player[nIdx].ServerPickUpItem((BYTE*)&pk);
	if (bOk)
	{
		if (nTienNho)
			pb_Log("[BotNhat] %s nhat %d luong tien roi\n",
			       Player[nIdx].m_PlayerName, nTienNho);
		else
			pb_Log("[BotNhat] %s nhat do roi: %s (item %d)\n",
			       Player[nIdx].m_PlayerName, Item[nItemNho].GetName(), nItemNho);
	}
	else
	{
		pb_Log("[BotNhat] %s nhat TRUOT obj %d (item %d) - bi tu choi, ne 30 giay\n",
		       Player[nIdx].m_PlayerName, pk.m_nObjID, nItemNho);
		pb_NeDo(b, b.nLootObjID, now);
	}
	b.nLootObjIdx = 0;
	b.loot.Reset();
	return 1;
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
		// NANG FULL KY NANG THEO CAP (chu game chot 18/08): moi lan len cap keo MOI ky nang
		// da hoc len dung cap bot (tran 20). RIENG ky nang moc 80 tro len chi mo 1 CAP -
		// "con lai bot tu luyen len cap" bang su dung, y nhu nguoi choi.
		// BO QUA bua bi dong (SKILL_SS_PassivityNpcState): KSkillList::Add TU CAST bua khi
		// nang (KSkillList.cpp:456-458) ma chu game da chot bot KHONG dung bua.
		{
			const int nLvBot = Npc[nNpcIdx].m_Level;
			int nNang = 0;
			KSkillList& sl2 = Npc[nNpcIdx].m_SkillList;
			for (int q = 1; q < MAX_NPCSKILL; q++)
			{
				const int id2 = sl2.m_Skills[q].SkillId;
				if (id2 <= 0 || id2 >= MAX_SKILL)
					continue;
				KSkill* p2 = (KSkill*)g_SkillManager.GetSkill(id2, 1);
				if (!p2)
					continue;
				if (p2->GetSkillStyle() == SKILL_SS_PassivityNpcState)
					continue;
				// khong nang chieu khac ngu hanh (chieu phai khac thua ke tu mau)
				{
					const int nSr2 = p2->GetSkillSeries();
					if (nSr2 >= 0 && nSr2 < series_num && nSr2 != Npc[nNpcIdx].m_Series)
						continue;
				}
				const int rq2 = p2->GetSkillReqLevel();
				if (nLvBot < rq2)
					continue;                    // chua du cap mo ky nang nay
				int nMuon = (rq2 >= 80) ? 1 : nLvBot;
				if (nMuon > 20) nMuon = 20;
				if (sl2.m_Skills[q].SkillLevel < nMuon)
				{
					sl2.Add(id2, nMuon);
					nNang++;
				}
			}
			if (nNang)
				pb_Log("[BotDanh] %s cap %d: nang %d ky nang theo cap\n",
				       Player[nIdx].m_PlayerName, nLvBot, nNang);
			// doc lai cap cua chieu da chon (neu co) sau khi nang dong loat
			if (b.nAtkSkill > 0)
			{
				const int nO = sl2.FindSame(b.nAtkSkill);
				if (nO)
					b.nAtkSkillLv = sl2.m_Skills[nO].CurrentSkillLevel;
			}
		}
		if (b.nAtkSkill != nCu)
			pb_Log("[BotDanh] %s cap %d dung chieu %d (cap %d, kieu/eqt xem skills.txt)\n",
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
		{
			// VUA GIET XONG -> quet do roi NGAY nhip sau (chu game 18/08: "nhat do
			// uu tien hon"), khong ngoi doi gion 2 giay - do cua minh roi ra la
			// chay lai nhat lien roi moi bat con ke.
			if (Npc[t].m_Doing == do_death)
				b.nLootScanTick = 0;
			t = 0;
		}
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
			// In kem HP muc tieu: HP con 100% = don danh KHONG AN (cast hong/sat thuong 0);
			// HP co sut nhung cham = danh an nhung yeu - hai benh khac nhau, chua khac nhau.
			pb_Log("[BotDanh] %s bo muc tieu %d: %d giay khong sut mau -> cam %d giay"
			       " (HP %d/%d, chieu %d cap %d)\n",
				   Player[nIdx].m_PlayerName, t, PB_LAG_SECONDS, PB_CAM_GIAY,
				   (int)Npc[t].m_CurrentLife, (int)Npc[t].m_CurrentLifeMax,
				   b.nAtkSkill, b.nAtkSkillLv);
			pb_CamMucTieu(b, t, now);
			t = 0;
		}
	}

	// XAC NHAN SAT THUONG THAT (chu game yeu cau 18/08: "log phai danh dung ky nang
	// co sat thuong"): HP muc tieu tut so voi nhip truoc = chieu DANG AN DON.
	// In toi da 1 dong / 10 giay / bot de khong ngap log.
	if (t)
	{
		const int nHpNay = (int)Npc[t].m_CurrentLife;
		if (b.nHpTruoc > 0 && nHpNay < b.nHpTruoc
		 && now - b.nDameTick >= (unsigned int)(GAME_FPS * 120))
		{
			b.nDameTick = now;
			pb_Log("[BotDame] %s chieu %d cap %d AN DON: muc tieu %d HP %d -> %d\n",
			       Player[nIdx].m_PlayerName, b.nAtkSkill, b.nAtkSkillLv,
			       t, b.nHpTruoc, nHpNay);
		}
		b.nHpTruoc = nHpNay;
	}
	else
		b.nHpTruoc = 0;

	if (!t)
	{
		t = pb_FindTarget(nNpcIdx, PB_VISION_MPS, b, now);
		b.nTargetNpc = t;
		b.chase.Reset();
		b.nPhamViTick = 0;    // dong ho ngoai tam thuoc muc tieu CU, khong mang sang
		b.nLagTick   = now;
		b.nLagLife   = t ? Npc[t].m_CurrentLife : 0;
		if (!t)
			return 0;
	}

	// ---- ap sat: di toi DIEM TRUOC MAT muc tieu, KHONG di vao o cua no ----
	//
	// Chep DUNG khuon ban tham khao (KBotCombat.cpp:550-583), nguyen van chu thich cua ho:
	//   "Chase a point at attack range IN FRONT of the target, not the target tile itself
	//    (occupied) -- pathing into an occupied tile makes the chaser bounce / circle".
	// O cua muc tieu la VAT CAN (JX1 tinh NPC la tuong). Ban truoc nham thang vao o do -
	// A* dan toi noi roi bot doi/xoay quanh muc tieu, nhin nhu KHONG di chuyen. Nay:
	//   1. diem nham = tren duong noi hai ben, cach muc tieu (AR - 16) - luon O PHIA TRUOC;
	//   2. di bang A* (PB_WalkTo) voi nguong toi 224 y het ban tham khao;
	//   3. A* bao TOI NOI (nWalk==1) -> chay THANG chang cuoi ngan <= 224 MPS. RIENG
	//      A* KHONG CO DUONG thi tu 18/08 la CAM muc tieu chu khong chay thang nua
	//      (chay thang khong qua A* = nguon bot xuyen bien map).
	{
		KSkill* pSk = (KSkill*)g_SkillManager.GetSkill(b.nAtkSkill, b.nAtkSkillLv);
		// Tam <= 0 (du lieu co 29 chieu AttackRadius = 0) cung roi ve 128: nAR = 0 la
		// gate "nDist > 0" vinh vien dung (phan bien 18/08). pb_PickSkill cung da loc TAM0.
		const int nAR = (pSk && pSk->GetAttackRadius() > 0) ? pSk->GetAttackRadius() : 128;
		int tx2 = 0, ty2 = 0;
		Npc[t].GetMpsPos(&tx2, &ty2);
		int bx2 = 0, by2 = 0;
		Npc[nNpcIdx].GetMpsPos(&bx2, &by2);
		const int nDist = g_GetDistance(bx2, by2, tx2, ty2);
		// Cong chap nhan cua CHINH engine la "khoang cach <= tam + 20" (CanCastSkill,
		// KSkills.cpp:330; ban server bo he so 0.8). Duoi khi vuot muc do, ra don khi
		// trong muc do - DONG NHAT voi engine, khong siet hon (phan bien 18/08: siet ve
		// dung nAR thi diem nham luoi 128 co the vinh vien khong lot vao tam).
		if (nDist > nAR + 20)
		{
			int adist = nAR - 16;             // dung ngay ben trong tam danh
			if (adist < 1) adist = 1;
			int aimX = tx2, aimY = ty2;
			if (nDist > adist)
			{
				aimX = tx2 - (tx2 - bx2) * adist / nDist;
				aimY = ty2 - (ty2 - by2) * adist / nDist;
			}
			// TOA SANG NGANG theo chi so bot (vuong goc huong tien, +/- 2 o): nhieu bot
			// danh CUNG mot con ma dung chung mot diem nham (lai qua luoi 128) la lai
			// chong cui. Toa ngang thi ca dam dan hang vong quanh muc tieu.
			if (nDist > 0)
			{
				// Bien do toa ngang phai CO GIAN THEO TAM (phan bien 18/08: +/-128 voi
				// chieu tam 75 la diem nham nam vinh vien NGOAI tam -> khong bao gio ra
				// don). Buoc = tam/8 kep [8..32] -> lech toi da tam/4, tran 64.
				const int nLech2 = (int)(&b - &s_bots[0]);
				int nBuocLat = nAR / 8;
				if (nBuocLat < 8)  nBuocLat = 8;
				if (nBuocLat > 32) nBuocLat = 32;
				const int nLat = ((nLech2 % 5) - 2) * nBuocLat;
				if (nLat)
				{
					aimX += (-(ty2 - by2) * nLat) / nDist;
					aimY += ((tx2 - bx2) * nLat) / nDist;
				}
			}
			// Diem nham THO (chua lam tron): chang thang cuoi se nham vao DAY - y het
			// ban tham khao (KBotCombat.cpp:578-580 MoveTo diem chua lam tron). Ban
			// lam tron chi lam DICH A* de khoa cache lo trinh khoi doi moi khung;
			// phan bien 18/08 chi ra: chay toi diem DA lam tron (lech toi 90 MPS) la
			// bot dau o "diem chet" ngoai tam, dung nhin quai vinh vien.
			const int nAimThoX = aimX;
			const int nAimThoY = aimY;
			aimX = aimX / 128 * 128 + 64;
			aimY = aimY / 128 * 128 + 64;
			// (19/08 phan bien) quai dung sat mep khung map: diem THO hop le nhung diem
			// LAM TRON (+/-64 MPS) co the nhay ra ngoai khung -> F-A trong PB_WalkTo se
			// tra -1 va bot CAM OAN quai hoan toan danh duoc. Diem tron hong thi dung
			// thang diem tho lam dich A* (chi kem on dinh cache lo trinh, khong sai).
			{
				int nRt = -1, nMXt = 0, nMYt = 0, nOXt = 0, nOYt = 0;
				SubWorld[nSub].Mps2Map(aimX, aimY, &nRt, &nMXt, &nMYt, &nOXt, &nOYt);
				if (nRt < 0)
				{
					aimX = nAimThoX;
					aimY = nAimThoY;
				}
			}

			// CHAN XUYEN BIEN: kiem diem nham THO (diem se chay toi that) - phai nam
			// TRONG ban do VA khong phai vat can dia hinh. Mps2Map chi kiem khung
			// region (phan bien 18/08: nuoc / vach nui van lot qua) nen phai kem
			// GetBarrierMin nhu [BotLach] da dung (bCheckNpc = FALSE: quai di dong,
			// chi can chac dia hinh).
			{
				int nRv = -1, nMXv = 0, nMYv = 0, nOXv = 0, nOYv = 0;
				bool bHong = (nAimThoX < 32 || nAimThoY < 32);
				if (!bHong)
				{
					SubWorld[nSub].Mps2Map(nAimThoX, nAimThoY, &nRv, &nMXv, &nMYv, &nOXv, &nOYv);
					bHong = (nRv < 0)
					     || (SubWorld[nSub].m_Region[nRv].GetBarrierMin(nMXv, nMYv, nOXv, nOYv, FALSE)
					         != Obstacle_NULL)
					     // (19/08 chieu) quai dung trong VUNG RONG du lieu / tren vach
					     // (luoi chan nhung engine cho di): duoi theo la bot di vao vung
					     // rong roi ket - chinh duong DoKiet382 bi [BotCuu] keo ve xong
					     // 60 giay sau lai o trong vung rong. Cam luon tu day.
					     || SubWorld[nSub].CellObsSrv(nAimThoX, nAimThoY) == 1;
				}
				if (bHong)
				{
					static unsigned int s_uBienLog1 = 0;
					if (now - s_uBienLog1 >= (unsigned int)GAME_FPS)
					{
						s_uBienLog1 = now;
						pb_Log("[BotBien] %s diem nham o(%d,%d) ngoai map/vat can -> cam muc tieu %d\n",
						       Player[nIdx].m_PlayerName, nAimThoX / 32, nAimThoY / 32, t);
					}
					pb_CamMucTieu(b, t, now);
					b.nTargetNpc = 0;
					b.chase.Reset();
					return 0;
				}
			}

			// DANG DUOI thi lam tuoi dong ho "khong sut mau": truoc day dong ho chay ca
			// luc duoi, nen bot duoi 10 giay chua toi noi la CAM OAN muc tieu roi lang
			// thang con khac (bot.log 10:57: HaDuy19 cam 5222 xong nham 5267 cach dung
			// 36 MPS). Ca dan cam-doi-cam lien tuc -> di cu vong quanh -> tac duong.
			// Dong ho chi duoc phep dem khi DA VAO TAM danh.
			b.nLagTick = now;
			b.nLagLife = Npc[t].m_CurrentLife;

			const int nWalk = PB_WalkTo(nNpcIdx, aimX, aimY, nSub, b.chase, 224);
			if (nWalk < 0)
			{
				// A* KHONG co duong / duong CUT -> cam muc tieu, TUYET DOI khong chay
				// thang thay the (nguon xuyen bien). Log gion 1 dong/giay/bot - phan
				// bien 18/08: khu >4 con ket la log moi nhip, fopen 18 lan/giay/bot.
				static unsigned int s_uBienLog2 = 0;
				if (now - s_uBienLog2 >= (unsigned int)GAME_FPS)
				{
					s_uBienLog2 = now;
					pb_Log("[BotBien] %s A* chiu thua toi o(%d,%d) -> cam muc tieu %d, KHONG chay thang\n",
					       Player[nIdx].m_PlayerName, aimX / 32, aimY / 32, t);
				}
				pb_CamMucTieu(b, t, now);
				b.nTargetNpc = 0;
				b.chase.Reset();
				return 0;
			}
			if (nWalk == 1)
			{
				// A* bao DA TOI vung 224 quanh dich A* ma van ngoai tam: chay chang
				// thang cuoi vao diem nham THO (da kiem hop le + khong vat can o tren).
				// DUONG THOAT (phan bien 18/08 doi): 6 giay van chua lot vao tam -> bo,
				// cam muc tieu. Truoc day dong ho "khong sut mau" bi lam tuoi moi nhip
				// duoi nen trang thai ket ngoai tam KHONG BAO GIO duoc tha.
				if (b.nPhamViTick == 0)
					b.nPhamViTick = now;
				else if (now - b.nPhamViTick >= (unsigned int)(GAME_FPS * 6))
				{
					pb_Log("[BotPhamVi] %s toi noi ma van ngoai tam (chieu %d tam %d, cach %d) -> cam muc tieu %d\n",
					       Player[nIdx].m_PlayerName, b.nAtkSkill, nAR, nDist, t);
					b.nPhamViTick = 0;
					pb_CamMucTieu(b, t, now);
					b.nTargetNpc = 0;
					b.chase.Reset();
					return 0;
				}
				if (now - b.nChaseLegTick >= (unsigned int)(GAME_FPS / 2))
				{
					b.nChaseLegTick = now;
					Npc[nNpcIdx].SendCommand(do_run, nAimThoX, nAimThoY);
				}
			}
			else
				b.nPhamViTick = 0;             // dang di binh thuong - xoa dong ho ngoai tam
			return 1;                          // dang ap sat - chua ra don
		}
		b.chase.Reset();                       // da vao tam
		b.nPhamViTick = 0;
	}

	// ---- ra don ----
	// Khong phat moi khung: DoWalk/DoSkill deu phat goi tin ra vung lan can.
	// KNpc::DoSkill co nhanh #ifdef _SERVER TU phat do_run toi muc tieu khi con ngoai tam
	// (KNpc.cpp:2338-2349), nen KHONG can tu dan bot vao gan - engine lo phan ap sat.
	if (now - b.nCastTick >= (unsigned int)PB_CAST_GAP)
	{
		// KIEM PHAM VI nam o CONG DUOI ben tren (nDist <= nAR + 20 moi xuong duoc day,
		// dung bang cong CanCastSkill cua engine). Phan bien 18/08 chung minh khoi kiem
		// "lan cuoi" tung dat o day la MA CHET: cung nhip, cung dau vao, dieu kien
		// khong bao gio dung -> da go. Duong thoat ket-ngoai-tam nam o nhanh nWalk==1.
		b.nCastTick = now;
		// Dung khuon may chu dung cho nguoi choi that khi nhan c2s_npcskill
		// (KProtocolProcess::NpcSkillCommand, KProtocolProcess.cpp:4926):
		//   SendCommand(do_skill, <id chieu>, -1, <khe NPC muc tieu>)
		// ProcCommand tu goi FindSame + SetActiveSkill (KNpc.cpp:861-871) nen khong
		// phai dat chieu truoc.
		// THAM DO truoc khi phat: hoi thang CanCastSkill cua DU AN (KSkills.cpp:177).
		// Bi tu choi thi in ly do ra bot.log thay vi de DoSkill "goto Exit" im lang.
		{
			KSkill* pDo = (KSkill*)g_SkillManager.GetSkill(b.nAtkSkill, b.nAtkSkillLv);
			if (pDo)
			{
				// (19/08 trua - chu game) chieu CAM tren ngua (HorseLimited=1) -> xuong
				// truoc khi danh; chieu DOI tren ngua (=2) -> leo len. CanCastSkill tu
				// choi IM LANG khi sai the cuoi (KSkills.cpp:287/304) nen chinh truoc.
				const int nHanNgua = pDo->GetHorseLimit();
				if (nHanNgua == 1 && Npc[nNpcIdx].m_bRideHorse)
					Player[nIdx].CheckRideHorse(TRUE);      // dang cuoi -> xuong ngua
				else if (nHanNgua == 2 && !Npc[nNpcIdx].m_bRideHorse
				      && Player[nIdx].m_ItemList.GetEquipment(itempart_horse) > 0)
					Player[nIdx].CheckRideHorse(FALSE);     // chieu doi cuoi -> len ngua
				int nP1 = -1, nP2 = t;
				if (pDo->CanCastSkill(nNpcIdx, nP1, nP2) == 0
				 && now - b.nUongTick >= (unsigned int)(GAME_FPS * 5))
				{
					pb_Log("[BotCast] %s chieu %d cap %d BI TU CHOI (eqt=%d,"
					       " vukhi detail=%d parti=%d, mana=%d/%d)\n",
					       Player[nIdx].m_PlayerName, b.nAtkSkill, b.nAtkSkillLv,
					       pDo->GetEquipLimit(),
					       Player[nIdx].m_ItemList.GetWeaponType(),
					       Player[nIdx].m_ItemList.GetWeaponParticular(),
					       (int)Npc[nNpcIdx].m_CurrentMana, (int)Npc[nNpcIdx].m_CurrentManaMax);
				}
			}
		}
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
	// GIOI HAN CUNG theo bot (chu game: "noi nham lien tuc, phai gioi han time nhu ban
	// goc"): du xac suat co trung lien tiep, moi bot van phai nghi toi thieu 60 giay,
	// cong ngau nhien den 120 - nhin tong the ca dam bot khong con do chuyen deu deu.
	if (b.nChatCamToi && now < b.nChatCamToi)
		return;
	if ((int)g_Random(1000) >= s_pbChatRate)
		return;
	b.nChatCamToi = now + (unsigned int)(GAME_FPS * (60 + (int)g_Random(61) + nLech));

	b.nChatCuoi = (b.nChatCuoi + 7 + nLech) % s_pbChatCount;
	char* p = s_pbChat[b.nChatCuoi];
	const int nLen = (int)strlen(p);
	if (nLen > 0)
		// false = CHI bong bong, KHONG ghi vao khung chat kenh phu can (chu game doi
		// y 18/08 chieu: "tat luon kenh chat phu can khong cho bot chat kenh do" -
		// cau bot nham khong hop khung chat, chi de lam nen som vai). Muon bat lai:
		// doi tham so cuoi thanh true (co m_btIsShowMsgPad, client co san nhanh xu ly).
		KPlayerChat::NpcChat(nNpcIdx, p, nLen, false);
}

// ===========================================================================
// CHAT KENH THE GIOI
//
// Ban goc linux trong tay CHI co bong bong (sim.fun.lua:13 NpcChat, tac gia tu
// ghi "tam revert NpcChat (bong bong)"); phan chat kenh nam o ban vdk.so moi hon
// khong co trong kho - chi con file lieu chatworld_msg.txt. Vay nen lam bang
// duong JX1 co san: KPlayerChat::SendSystemInfo (KPlayerChat.cpp:1773) bom thang
// CHAT_CHANNELCHAT_SYNC voi ten bot + id kenh toi tung nguoi that - khong di qua
// S3Relay (relay se tu choi vi bot khong co ket noi chat, ChannelMgr.cpp:897).
//
// ID kenh do relay CAP LUC CHAY theo ten (PLAYERCOMM_NOTIFYCHANNELID di ngang
// GameServer tren duong xuong) - khong co hang so de ghi cung. PB_GhiNhoKenh hung
// bang nay; kenh THE GIOI = kenh dau tien co ten KHONG mang khoa the ca nhan
// ('#' cua Team#/Faction#/Tong#, '\\' cua man hinh) va khong phai "GM" -
// client hoi kenh theo thu tu ini nen the gioi luon toi truoc CH_SONG/CH_JIN.
// Moi kenh thay duoc deu in [BotKenh] de doi tay neu chon sai.
#define PB_KENH_MAX      24
#define PB_TG_GAP_MS     45000                 // gian cach toi thieu giua 2 cau the gioi
#define PB_TG_GAP_THEM   45000                 // + ngau nhien 0..45s nua

#define PB_TG_MAX  64
static char s_pbChatTG[PB_TG_MAX][PB_CHAT_LEN];
static int  s_pbChatTGCount = 0;

// Nap kho cau kenh the gioi tu settings/simcity/chatworld.txt - chinh la
// chatworld_msg.txt 119 cau RAO VAT cua ban goc linux (mo vdk 18/08 xac nhan:
// file CUNG BANG MA voi ma nguon du an, chep thang khong chuyen ma; 20 dong co
// cho trong {item} de nhet LINK DO). Khong co file thi lui ve nhom "giaodich"
// cua chat.txt. Nap luoi mot lan.
static void pb_NapChatTG()
{
	static int s_bDaNap = 0;
	if (s_bDaNap)
		return;
	s_bDaNap = 1;

	FILE* f = fopen("settings/simcity/chatworld.txt", "rb");
	if (f)
	{
		char szDong[512];
		while (s_pbChatTGCount < PB_TG_MAX && fgets(szDong, sizeof(szDong), f))
		{
			char* p = szDong;
			while (*p == ' ' || *p == '\t')
				p++;                              // moi dong goc co mot dau cach dau dong
			int nL = (int)strlen(p);
			while (nL > 0 && (p[nL-1] == '\r' || p[nL-1] == '\n'
						   || p[nL-1] == ' '  || p[nL-1] == '\t'))
				p[--nL] = 0;
			if (nL <= 0)
				continue;
			strncpy(s_pbChatTG[s_pbChatTGCount], p, PB_CHAT_LEN - 1);
			s_pbChatTG[s_pbChatTGCount][PB_CHAT_LEN - 1] = 0;
			s_pbChatTGCount++;
		}
		fclose(f);
		pb_Log("[BotKenh] nap %d cau rao vat tu chatworld.txt\n", s_pbChatTGCount);
		return;
	}

	// du phong: khong co chatworld.txt -> lay nhom giaodich cua chat.txt
	KTabFile tab;
	if (!tab.Load((LPSTR)"/settings/simcity/chat.txt"))
		return;
	const int nH = tab.GetHeight();
	for (int row = 2; row <= nH && s_pbChatTGCount < PB_TG_MAX; row++)
	{
		char szT[64] = { 0 };
		tab.GetString(row, 1, (LPSTR)"", szT, sizeof(szT));
		if (strcmp(szT, "giaodich") != 0)
			continue;
		char szC[PB_CHAT_LEN] = { 0 };
		tab.GetString(row, 2, (LPSTR)"", szC, PB_CHAT_LEN);
		if (!szC[0])
			continue;
		strncpy(s_pbChatTG[s_pbChatTGCount], szC, PB_CHAT_LEN - 1);
		s_pbChatTG[s_pbChatTGCount][PB_CHAT_LEN - 1] = 0;
		s_pbChatTGCount++;
	}
	pb_Log("[BotKenh] nap %d cau (du phong chat.txt)\n", s_pbChatTGCount);
}

// ---------------------------------------------------------------------------
// LINK DO trong chat: client JX1 CO SAN trinh doc token "[40 so phay nhau]"
// (UiPlayerBar.cpp:2067-2219 phia go, UiMsgCentrePad.cpp:323 + CoreShell.cpp:8529
// phia doc, NUM_INFO_ITEM_CHAT = 39 dau phay). Thu tu 40 truong:
//   1 idTen  2 genre  3 detail  4 particular  5 series  6 level  7 luck
//   8 version  9 randomSeed  10 idx  11 price  12 x  13 y  14 point  15 stack
//   16 enchance  17 goldid  18 yearexp  19 lock  20 durability  21 hlock
//   22 nature  23 maxOpt  24..39 generatorLevel[0..15] (MOI so deu co dau phay
//   sau no, ke ca so cuoi, roi dong ngoac).
// Tao mot mon TAM bang dung duong ItemSet.Add cua trao vu khi nhap mon, doc du
// lieu that (seed/version/magic) de tooltip client dung ra mon that, roi huy.
static void pb_TaoLinkDo(char* szOut, int nMax)
{
	szOut[0] = 0;
	// vai mau vu khi quen mat de rao ban (detail, particular nhu bang nhap mon)
	static const int s_aMauDo[][2] = {
		{ 0, 1 },   // dao
		{ 0, 0 },   // kiem
		{ 0, 2 },   // bong
		{ 0, 3 },   // thuong
		{ 0, 4 },   // chuy
		{ 1, 1 },   // phi tieu
	};
	const int nMau = (int)g_Random(sizeof(s_aMauDo) / sizeof(s_aMauDo[0]));
	int nMagicTG[MAX_ITEM_MAGICLEVEL];
	ZeroMemory(nMagicTG, sizeof(nMagicTG));
	const int nDo = ItemSet.Add(0, 0, (int)g_Random(5), 1 + (int)g_Random(8), 0,
							  s_aMauDo[nMau][0], s_aMauDo[nMau][1], nMagicTG,
							  g_SubWorldSet.GetGameVersion(), 0);
	if (nDo <= 0 || nDo >= MAX_ITEM)
		return;

	KItemGeneratorParam* pTS = Item[nDo].GetItemParam();
	int nViet = _snprintf(szOut, nMax - 1,
		"[%lu,%d,%d,%d,%d,%d,%d,%d,%u,0,0,0,0,0,1,0,0,0,0,%d,0,%d,%d,",
		(unsigned long)Item[nDo].GetID(), Item[nDo].GetGenre(),
		Item[nDo].GetDetailType(), Item[nDo].GetParticular(),
		Item[nDo].GetSeries(), Item[nDo].GetLevel(),
		pTS->nLuck, pTS->nVersion, (unsigned int)pTS->uRandomSeed,
		Item[nDo].GetDurability(), Item[nDo].GetNature(),
		Item[nDo].GetMaxOptMultiply());
	for (int q = 0; q < MAX_ITEM_MAGICLEVEL && nViet > 0 && nViet < nMax - 8; q++)
		nViet += _snprintf(szOut + nViet, nMax - nViet - 1, "%d,",
						   pTS->nGeneratorLevel[q]);
	if (nViet > 0 && nViet < nMax - 2)
	{
		szOut[nViet]     = ']';
		szOut[nViet + 1] = 0;
	}
	else
		szOut[0] = 0;

	ItemSet.Remove(nDo);              // mon tam chi de doc du lieu, huy ngay
}

struct PB_Kenh
{
	char          szTen[36];
	unsigned long dwId;
};
static PB_Kenh s_aKenh[PB_KENH_MAX];
static int     s_nKenhDem = 0;
static unsigned long s_dwKenhTheGioi = (unsigned long)-1;

void PB_GhiNhoKenh(const char* szTen, unsigned long dwId)
{
	if (!szTen || !szTen[0] || dwId == (unsigned long)-1)
		return;
	for (int i = 0; i < s_nKenhDem; i++)
		if (s_aKenh[i].dwId == dwId)
			return;                             // da biet
	if (s_nKenhDem < PB_KENH_MAX)
	{
		strncpy(s_aKenh[s_nKenhDem].szTen, szTen, sizeof(s_aKenh[0].szTen) - 1);
		s_aKenh[s_nKenhDem].szTen[sizeof(s_aKenh[0].szTen) - 1] = 0;
		s_aKenh[s_nKenhDem].dwId = dwId;
		s_nKenhDem++;
		pb_Log("[BotKenh] thay kenh \"%s\" id=%lu\n", szTen, dwId);
	}
	if (s_dwKenhTheGioi == (unsigned long)-1
	 && !strchr(szTen, '#') && !strchr(szTen, '\\')
	 && _stricmp(szTen, "GM") != 0)
	{
		s_dwKenhTheGioi = dwId;
		pb_Log("[BotKenh] CHON kenh THE GIOI = \"%s\" (id=%lu)\n", szTen, dwId);
	}
}

// Thinh thoang mot bot ngau nhien noi mot cau len kenh the gioi (45-90 giay/cau
// cho CA DAN - kenh chat on hon bong bong nhieu nen phai giu thua).
static void pb_ChatTheGioi()
{
	static DWORD s_dwMoc = 0;
	static DWORD s_dwCho = 0;
	if (s_dwKenhTheGioi == (unsigned long)-1 || s_botCount <= 0)
		return;
	pb_NapChatTG();
	if (s_pbChatTGCount <= 0)
		return;
	const DWORD dwNow = GetTickCount();
	if (s_dwCho == 0)
		s_dwCho = PB_TG_GAP_MS + (DWORD)g_Random(PB_TG_GAP_THEM);
	if (s_dwMoc == 0)
	{
		s_dwMoc = dwNow;
		return;
	}
	if (dwNow - s_dwMoc < s_dwCho)
		return;
	s_dwMoc = dwNow;
	s_dwCho = PB_TG_GAP_MS + (DWORD)g_Random(PB_TG_GAP_THEM);

	// chon mot bot con song ngau nhien lam nguoi noi
	const int nBat = (int)g_Random(s_botCount);
	int nNoi = 0;
	for (int i = 0; i < s_botCount; i++)
	{
		PB_Bot& b = s_bots[(nBat + i) % s_botCount];
		const int p = b.nPlayerIdx;
		if (p <= 0 || p >= MAX_PLAYER || Player[p].m_dwID != b.dwID)
			continue;
		const int n = Player[p].m_nIndex;
		if (n <= 0 || n >= MAX_NPC)
			continue;
		if (Npc[n].m_Doing == do_death || Npc[n].m_Doing == do_revive)
			continue;
		nNoi = p;
		break;
	}
	if (nNoi <= 0)
		return;

	char* szGoc = s_pbChatTG[(int)g_Random(s_pbChatTGCount)];
	char szCau[480];
	// 20/119 cau co cho trong {item}: nhet LINK DO that vao (client tu ve ten do
	// mau vang bam xem duoc) - tra loi yeu cau "bot post do len kenh chat" 18/08.
	const char* pItem = strstr(szGoc, "{item}");
	if (pItem)
	{
		char szLink[320];
		pb_TaoLinkDo(szLink, sizeof(szLink));
		if (!szLink[0])
			return;                            // khong tao duoc mon -> bo luot nay
		const int nTruoc = (int)(pItem - szGoc);
		_snprintf(szCau, sizeof(szCau) - 1, "%.*s%s%s",
		          nTruoc, szGoc, szLink, pItem + 6);
		szCau[sizeof(szCau) - 1] = 0;
	}
	else
	{
		strncpy(szCau, szGoc, sizeof(szCau) - 1);
		szCau[sizeof(szCau) - 1] = 0;
	}
	const int nLen = (int)strlen(szCau);
	if (nLen <= 0 || nLen > 250)
		return;                                // sentlen la BYTE - giu duoi 250

	// bom toi TUNG nguoi that (nType=1). TUYET DOI khong dung nType=0: vong lap
	// ben trong SendSystemInfo khong loc m_nNetConnectIdx=-1 cua bot.
	int nGui = 0;
	int nIdx = PlayerSet.GetFirstPlayer();
	while (nIdx)
	{
		if (Player[nIdx].m_nNetConnectIdx >= 0 && Player[nIdx].m_dwID != 0)
		{
			KPlayerChat::SendSystemInfo(1, nIdx, Player[nNoi].m_PlayerName,
			                            szCau, nLen, (int)s_dwKenhTheGioi);
			nGui++;
		}
		nIdx = PlayerSet.GetNextPlayer();
	}
	if (nGui)
		pb_Log("[BotKenh] %s noi kenh the gioi (toi %d nguoi): \"%.60s\"\n",
		       Player[nNoi].m_PlayerName, nGui, szCau);
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

// Gui loi dap vao DUNG CUA SO CHAT MAT cua nguoi gui (dung kenh, dung mau).
//
// Ban truoc dung KPlayerChat::SendSystemInfo - ham do dong goi CHAT_CHANNELCHAT_SYNC
// (KPlayerChat.cpp:1796) nen client ve vao kenh HE THONG, sai mau chat mat (chu game
// bat duoc). Duong ve dung cua chat mat la chat_someonechat: client nhan
// CHAT_SOMEONECHAT_SYNC + text -> MSNMessageArrival (KProtocolProcess.cpp:4121-4127),
// hien dung cua so PM voi mau cua du an.
// Khuon dong goi chep y het SendSystemInfo, chi thay struct; kich thuoc that cua goi
// tinh bang sizeof nen khong lech du hai cay khac cau truc.
static void pb_GuiChatMat(int nSenderIdx, int nBotIdx, const char* szMsg, int nLen)
{
	if (nLen <= 0 || nLen > 200)
		return;
	char szGoi[512];
	const size_t chatsize = sizeof(CHAT_SOMEONECHAT_SYNC) + nLen;
	const size_t pckgsize = sizeof(tagExtendProtoHeader) + chatsize;
	if (pckgsize > sizeof(szGoi))
		return;

	tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)szGoi;
	pExHeader->ProtocolType = s2c_extendchat;
	pExHeader->wLength      = (WORD)(pckgsize - 1);

	CHAT_SOMEONECHAT_SYNC* pSync = (CHAT_SOMEONECHAT_SYNC*)(pExHeader + 1);
	pSync->ProtocolType = chat_someonechat;
	pSync->wSize        = (WORD)(chatsize - 1);
	pSync->packageID    = (DWORD)-1;
	memset(pSync->someone, 0, sizeof(pSync->someone));
	strncpy(pSync->someone, Player[nBotIdx].m_PlayerName, _NAME_LEN - 1);
	pSync->sentlen      = (BYTE)nLen;
	memcpy(pSync + 1, szMsg, nLen);

	g_pServer->PackDataToClient(Player[nSenderIdx].m_nNetConnectIdx, pExHeader, pckgsize);
}

// Echo CAU HOI ve cho chinh nguoi gui.
//
// Binh thuong khi PM, may chu chat (S3Relay) tra ve CHAT_FEEDBACK va CLIENT ve dong
// "minh -> ten kia: ..." tu goi do (KProtocolProcess.cpp:4135-4149: sau CHAT_FEEDBACK la
// [DWORD channelid = -1][dstName _NAME_LEN][BYTE sentlen][text]). Ta chan PM gui bot
// TRUOC khi no len S3Relay nen feedback khong bao gio ve - nguoi gui khong thay cau
// minh vua go (chu game bat duoc). Nay tu dung goi feedback y het, code = codeSucc,
// packageID phai VONG LAI dung tu goi CMD de client khop cau hoi nao.
static void pb_GuiChatMatEcho(int nSenderIdx, const char* szBotName,
                              const char* szMsg, int nLen, unsigned long nPackageID)
{
	if (nLen <= 0 || nLen > 200)
		return;
	char szGoi[512];
	const size_t duoisize = sizeof(DWORD) + _NAME_LEN + 1 + nLen;
	const size_t pckgsize = sizeof(tagExtendProtoHeader) + sizeof(CHAT_FEEDBACK) + duoisize;
	if (pckgsize > sizeof(szGoi))
		return;

	tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)szGoi;
	pExHeader->ProtocolType = s2c_extendchat;
	pExHeader->wLength      = (WORD)(pckgsize - 1);

	CHAT_FEEDBACK* pFb = (CHAT_FEEDBACK*)(pExHeader + 1);
	pFb->ProtocolType = chat_feedback;
	pFb->packageID    = nPackageID;
	pFb->code         = codeSucc;

	BYTE* pDuoi = (BYTE*)(pFb + 1);
	*(DWORD*)pDuoi = (DWORD)-1;                       // -1 = kenh chat mat
	pDuoi += sizeof(DWORD);
	memset(pDuoi, 0, _NAME_LEN);
	strncpy((char*)pDuoi, szBotName, _NAME_LEN - 1);  // "gui toi ai" = ten bot
	pDuoi += _NAME_LEN;
	*pDuoi++ = (BYTE)nLen;
	memcpy(pDuoi, szMsg, nLen);

	g_pServer->PackDataToClient(Player[nSenderIdx].m_nNetConnectIdx, pExHeader, pckgsize);
}

static const char* s_PmTraLoiChung[] = {
	"Minh dang ban luyen cap, noi chuyen sau nhe.",
	"U, minh nghe day.",
	"Ban can gi khong?",
	"Minh khong hieu y ban lam.",
	"Chut nua ranh minh noi tiep nhe.",
	"He he, vay ha.",
	"Ok ban.",
	"Minh dang danh quai, hoi kho tra loi dai.",
};

// hash chuoi don gian (djb2) - chong tra loi y het cau lien truoc
static unsigned int pb_HashChuoi(const char* s)
{
	unsigned int h = 5381;
	while (*s) h = h * 33 + (unsigned char)*s++;
	return h;
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
	// Thu tu co chu y: "bot" xet TRUOC (nguoi choi hay thu "ban la bot ha"), roi cac cau
	// hoi cu the, cuoi cung moi toi chao hoi chung chung.
	if (strstr(szLow, "bot ") || strstr(szLow, " bot") || strstr(szLow, "auto"))
		sprintf(szTraLoi, "Bot gi dau, minh choi that ma :)");
	else if (strstr(szLow, "solo") || strstr(szLow, "pk "))
		sprintf(szTraLoi, "Minh khong solo dau, dang ban luyen cap.");
	else if (strstr(szLow, "co do") || strstr(szLow, "trang bi") || strstr(szLow, "vu khi")
	      || strstr(szLow, "xin ") || strstr(szLow, "cho minh") || strstr(szLow, "tang "))
		sprintf(szTraLoi, "Do minh tu kiem duoc thoi, khong cho duoc dau.");
	else if (strstr(szLow, "chao") || strstr(szLow, "hi ") || strstr(szLow, "hello")
	      || strstr(szLow, "alo"))
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

	// ---- khong khop tu khoa -> cau chung CO NGHIA, khong bao gio im lang ----
	// Ban truoc boc ngau nhien tu kho chat.txt (kho NOI NHAM cong khai) nen ra nhung cau
	// tren troi kieu "Toi khong the hat live!" giua cuoc noi chuyen rieng - chu game bat
	// duoc ngay. PM la doi thoai 1-1, phai dung bo cau soan rieng.
	if (!szTraLoi[0])
	{
		const int nSo = (int)(sizeof(s_PmTraLoiChung) / sizeof(s_PmTraLoiChung[0]));
		static int s_nXoay = 0;
		int nPick;
		if (pB)
		{
			pB->nChatCuoi = (pB->nChatCuoi + 1) % nSo;
			nPick = pB->nChatCuoi;
		}
		else
			nPick = (s_nXoay++) % nSo;
		strncpy(szTraLoi, s_PmTraLoiChung[nPick], sizeof(szTraLoi) - 1);
		szTraLoi[sizeof(szTraLoi) - 1] = 0;
	}

	const int nOut = (int)strlen(szTraLoi);
	if (nOut <= 0)
		return 0;

	// Echo cau hoi van hien NGAY (do la dong "minh vua noi" cua chinh nguoi hoi).
	pb_GuiChatMatEcho(p->nSenderIdx, Player[nBot].m_PlayerName, p->szMsg, nLen, p->nPackageID);

	// GION NHIP + GUI TRE nhu nguoi that (chu game 18/08: "pm mat la bot tra loi
	// lien... gioi han time rep"): cau tra loi KHONG gui ngay ma hen 2-6 giay;
	// dang co cau cho gui / con trong han cam 8-15 giay thi lam ngo luon.
	{
		const int nNpcB = Player[nBot].m_nIndex;
		if (nNpcB <= 0 || nNpcB >= MAX_NPC || !pB)
		{
			// khong co khe theo doi (hiem) -> gui thang nhu cu
			pb_GuiChatMat(p->nSenderIdx, nBot, szTraLoi, nOut);
			return 1;
		}
		const int nSubB = Npc[nNpcB].m_SubWorldIndex;
		const unsigned int now = (nSubB >= 0 && nSubB < MAX_SUBWORLD)
								   ? SubWorld[nSubB].m_dwCurrentTime : 0;
		if (pB->nPmDenHan != 0 || (pB->nPmCamToi != 0 && now < pB->nPmCamToi))
		{
			pb_Log("[BotPM] %s lam ngo %s (dang ban tra loi / trong han cam)\n",
			       Player[nBot].m_PlayerName, Player[p->nSenderIdx].m_PlayerName);
			return 1;
		}
		// chong lap: cau y het cau vua tra loi lan truoc -> doi sang cau chung khac
		unsigned int nH = pb_HashChuoi(szTraLoi);
		if (nH == pB->nPmLapHash)
		{
			const int nSo2 = (int)(sizeof(s_PmTraLoiChung) / sizeof(s_PmTraLoiChung[0]));
			pB->nChatCuoi = (pB->nChatCuoi + 1) % nSo2;
			strncpy(szTraLoi, s_PmTraLoiChung[pB->nChatCuoi], sizeof(szTraLoi) - 1);
			szTraLoi[sizeof(szTraLoi) - 1] = 0;
			nH = pb_HashChuoi(szTraLoi);
		}
		pB->nPmLapHash = nH;
		strncpy(pB->szPmTraLoi, szTraLoi, sizeof(pB->szPmTraLoi) - 1);
		pB->szPmTraLoi[sizeof(pB->szPmTraLoi) - 1] = 0;
		pB->nPmSenderIdx = p->nSenderIdx;
		pB->nPmDenHan    = now + GAME_FPS * 2 + g_Random(GAME_FPS * 4);
		pb_Log("[BotPM] %s <- %s: \"%.60s\" | hen dap: \"%.60s\"\n",
		       Player[nBot].m_PlayerName, Player[p->nSenderIdx].m_PlayerName,
		       p->szMsg, szTraLoi);
	}
	return 1;
}

// Gui cau tra loi PM da hen gio (goi moi nhip tu pb_DriveBot).
static void pb_XuLyPmCho(int nBotIdx, PB_Bot& b, unsigned int now)
{
	if (b.nPmDenHan == 0 || now < b.nPmDenHan)
		return;
	b.nPmDenHan = 0;
	const int nAi = b.nPmSenderIdx;
	if (nAi <= 0 || nAi >= MAX_PLAYER
	 || Player[nAi].m_dwID == 0 || Player[nAi].m_nIndex <= 0)
		return;                                // nguoi hoi da thoat
	const int nOut = (int)strlen(b.szPmTraLoi);
	if (nOut <= 0)
		return;
	pb_GuiChatMat(nAi, nBotIdx, b.szPmTraLoi, nOut);
	// han cam 8-15 giay: trong han nay moi cau hoi den deu bi lam ngo
	b.nPmCamToi = now + GAME_FPS * (8 + (int)g_Random(8));
	b.szPmTraLoi[0] = 0;
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

	// THE LUC giu day binh: do_run TIEU the luc, va OnRun kiet suc thi tut xuong DI BO
	// (KNpc.cpp:2203-2215) - cham va co the la mot phan ly do "khong ra khoi trap".
	// Nhan vat mau nhan ban co the mang the luc 0 trong blob. Bot la dan gia lap, cu bom day.
	if (Npc[nNpcIdx].m_CurrentStaminaMax <= 0)
		Npc[nNpcIdx].m_CurrentStaminaMax = 100;
	if (Npc[nNpcIdx].m_CurrentStamina < Npc[nNpcIdx].m_CurrentStaminaMax)
		Npc[nNpcIdx].m_CurrentStamina = Npc[nNpcIdx].m_CurrentStaminaMax;
	// NOI LUC giu it nhat nua binh: chieu ton noi luc, can noi luc thi cast bi tu choi
	// IM LANG va bot tut ve DANH THUONG - dung trieu chung "mot so phai van danh thuong".
	// Nguoi that uong thuoc; bot gia lap thi bom.
	if (Npc[nNpcIdx].m_CurrentManaMax > 0
	 && Npc[nNpcIdx].m_CurrentMana < Npc[nNpcIdx].m_CurrentManaMax / 2)
		Npc[nNpcIdx].m_CurrentMana = Npc[nNpcIdx].m_CurrentManaMax;

	// Noi chuyen chay SONG SONG voi moi viec khac (di duong, danh nhau) - nguoi that cung
	// vua danh vua noi. Dat truoc cac nhanh return de khong bi nhanh nao nuot mat.
	{
		// chi so bot = vi tri trong s_bots (idiom :1787) - vong quet cu la O(n^2)
		// voi 1000 bot: ~18 trieu vong lap/giay chi de tim chi so chinh minh
		const int nLech = (int)(&b - s_bots);
		pb_Chat(nNpcIdx, b, nowAll, nLech);
	}
	// PM da hen gio thi gui - chay truoc cac nhanh return nhu chat
	pb_XuLyPmCho(nIdx, b, nowAll);

	// ap CHE DO cho bot den muon (sinh / vao phai xong SAU khi chu game bam lenh)
	if (s_nPbCheDoNhapMon && b.nAi == PB_AI_IDLE && b.nFaction >= 0)
	{
		b.nAi      = PB_AI_GOTO_FACTION;
		b.nRetry   = 0;
		b.nNextTry = 0;
		b.walk.Reset();
	}
	if (s_nPbCheDoDanh && b.nAi == PB_AI_IN_FACTION)
	{
		b.nAi        = PB_AI_FIGHT;
		b.nTargetNpc = 0;
		b.nAtkSkill  = 0;    // ep chon lai chieu theo vu khi hien tai
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

	// ---------------------------------------------------------------- NGOI BAN SAP
	// (19/08 chieu) bot duoc boc "ra thanh ban sap" (PB_SetBanSap): khong danh
	// quai / khong ra bai - dung sap THAT (m_BaiTan + hang co gia), nguoi choi
	// XEM + MUA qua dung duong TradeBuyItem cua nguoi that.
	if (b.nBanSap)
	{
		pb_BanSap(nIdx, nNpcIdx, nSub, b);
		return;
	}

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
			// tron chi so bot: g_Random dong bang theo giay (xem ghi chu o cho gieo
			// nhom) - khong tron thi ca loat bot sinh cung giay tan ra CUNG MOT diem
			{
				const int nLechTan = (int)(&b - s_bots);   // chi so bot, thay vong quet O(n)
				b.nScatterX = bx + (((int)g_Random(nR * 2 + 1) + nLechTan * 13) % (nR * 2 + 1) - nR) * 32;
				b.nScatterY = by + (((int)g_Random(nR * 2 + 1) + nLechTan * 29) % (nR * 2 + 1) - nR) * 32;
			}
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
		const int nLech = (int)(&b - s_bots);   // chi so bot, thay vong quet O(n)
		// ---- LACH DAM DONG ----
		// bot.log 10:47: ca dan nem chat trong hop 4x4 o (2554-2558, 3480-3483) tren duong
		// toi bai quai - nut co chai dia hinh + NPC-LA-TUONG, 18 than tu chan nhau, thinh
		// thoang mot con lach qua duoc (dung "chi 1 bot ra"). Nguoi that gap dam dong thi
		// LACH NGANG. Bot: 6 giay khong nhich noi qua 3 o -> buoc sang mot o TRONG gan do
		// (SetPos - dung meo rai diem cua ban tham khao, KBotManager.cpp:1525-1557; o dich
		// kiem bang GetBarrierMin bCheckNpc=TRUE de khong dap len nguoi khac) roi tinh
		// duong lai tu dau.
		{
			int jx = 0, jy = 0;
			Npc[nNpcIdx].GetMpsPos(&jx, &jy);
			const int jdx = jx - b.nJamX;
			const int jdy = jy - b.nJamY;
			// DUNG YEN RA DON la binh thuong, khong phai ket (chu game 18/08: bot dang
			// danh bi SetPos nhay 3 o moi 8 giay - "nhay coc toa do", va SetPos doi
			// region lam client go/ve NPC nhin "nhu ra vao map"). Vua cast trong 3
			// giay thi lam tuoi neo nhu the dang di chuyen.
			if (b.nJamTick == 0 || jdx > 96 || jdx < -96 || jdy > 96 || jdy < -96
			 || nowAll - b.nCastTick < (unsigned int)(GAME_FPS * 3))
			{
				b.nJamX = jx;  b.nJamY = jy;  b.nJamTick = nowAll;
			}
			else if (nowAll - b.nJamTick >= (unsigned int)(GAME_FPS * 6)
			      && nowAll - b.nLachTick >= (unsigned int)(GAME_FPS * 8))
			{
				b.nLachTick = nowAll;
				b.nJamTick  = 0;
				static const int aLx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
				static const int aLy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
				const int nLech3 = (int)(&b - &s_bots[0]);
				b.nLachDem++;
				for (int h = 0; h < 24; h++)
				{
					// + nLachDem*3 (buoc le): moi lan lach thu huong KHAC truoc,
					// khong con lach A->B roi 8 giay sau lach B->A vinh vien
					const int dir  = (nLech3 + b.nLachDem * 3 + h) & 7;
					const int cach = 3 + (h / 8);            // 3, 4 roi 5 o
					const int cx = jx + aLx[dir] * cach * 32;
					const int cy = jy + aLy[dir] * cach * 32;
					int nR = -1, nMX = 0, nMY = 0, nOX = 0, nOY = 0;
					SubWorld[nSub].Mps2Map(cx, cy, &nR, &nMX, &nMY, &nOX, &nOY);
					if (nR < 0)
						continue;
					if (SubWorld[nSub].m_Region[nR].GetBarrierMin(nMX, nMY, nOX, nOY, TRUE)
					 != Obstacle_NULL)
						continue;
					{
						static unsigned int s_uLachLog = 0;
						if (nowAll - s_uLachLog >= (unsigned int)(GAME_FPS / 3))
						{
							s_uLachLog = nowAll;
							pb_Log("[BotLach] %s ket dam dong o(%d,%d) -> lach sang o(%d,%d)\n",
							       Player[nIdx].m_PlayerName, jx / 32, jy / 32, cx / 32, cy / 32);
						}
					}
					Npc[nNpcIdx].SetPos(cx, cy);
					b.walk.Reset();
					b.chase.Reset();
					b.roam.Reset();
					b.follow.Reset();   // (19/08) lo trinh cu vo nghia sau khi dich cho
					b.nFollowNghiToi = 0;
					b.loot.Reset();
					break;
				}
			}
		}
		// ---- BAM THEO DOI TRUONG ----
		// Thanh vien cach doi truong qua PB_BAM_NHA (200 MPS, cung map) thi BO
		// DANH de chay ve gan (khuon KBotCombat.cpp:54-77 + FollowPlayer cua ban
		// tham khao). Trong 200 thi danh binh thuong - ca dan tu tum quanh doi
		// truong, nam trong ban kinh chia exp 768 (GameDataDef.h:58).
		// (D21) dang lam Da Tau thi KHONG bam theo doi truong: khoi follow co
		// "return;" chay TRUOC pb_DaTau nen bot bo nhiem vu di duoi doi truong va
		// pb_DaTau khong bao gio duoc goi de tu cuu.
		if (pb_TrongNhom(nIdx) && pb_DoiTruongCua(nIdx) != nIdx && !b.nDaTauChon)
		{
			const int nCap    = pb_DoiTruongCua(nIdx);
			const int nCapNpc = (nCap > 0) ? Player[nCap].m_nIndex : 0;
			if (nCap > 0 && PB_IsBot(nCap) && nCapNpc > 0 && nCapNpc < MAX_NPC
			 && Npc[nCapNpc].m_SubWorldIndex == nSub
			 && Npc[nCapNpc].m_Doing != do_death)
			{
				if (NpcSet.GetDistance(nNpcIdx, nCapNpc) > PB_BAM_NHA)
				{
					// di bang A* nhu moi di chuyen khac - khong bao gio ChangeWorld
					// duoi theo (luat cua ban tham khao, ServerAutoFight.cpp:1188)
					// (19/08 phan bien) PHAI hung gia tri tra ve - day la caller DUY NHAT
					// tung vut no: -1 lien tuc (doi truong dung cho A* khong toi duoc) ma
					// cu goi lai moi nhip = quet A* full moi nhip + bot dong bang. Thua
					// thi nghi 5 giay, trong luc nghi van danh/nhat tai cho binh thuong.
					// (chot) 3 dong don dep PHAI nam trong cong nghi: de ngoai thi luc
					// nghi van xoa muc tieu + reset chase MOI NHIP = quet FindTarget +
					// A* chase moi nhip, dung cai churn ma fix nay dinh diet.
					if (b.nFollowNghiToi == 0 || nowAll >= b.nFollowNghiToi)
					{
						b.nTargetNpc = 0;
						b.chase.Reset();
						b.nRoamX = 0;  b.nRoamY = 0;
						int cx = 0, cy = 0;
						Npc[nCapNpc].GetMpsPos(&cx, &cy);
						const int nF = PB_WalkTo(nNpcIdx, cx, cy, nSub, b.follow, PB_BAM_GAN);
						if (nF < 0)
						{
							b.follow.Reset();
							b.nFollowNghiToi = nowAll + (unsigned int)(GAME_FPS * 5);
						}
						else
						{
							b.nFollowNghiToi = 0;
							return;
						}
					}
				}
			}
		}
		// ---- TU CUU KHI KET LUOI + PHAT LAI VU KHI (19/08 chieu) ----
		// Chu game: "bot van dang dung ngoai map" + "mot so bot chet ve thanh khong
		// len lai map luyen cong". Phap y bot.log 09:5x ra HAI benh:
		// 1) KET LUOI: bot bi NAP LAI dung vi tri da luu tu thoi "vung rong = di duoc"
		//    (truoc 561e2163). Nay vung rong = VAT CAN toan phan -> xuat phat ket,
		//    FindFreeBlockAround chi quet vanh 1 o quanh block (block vung rong gop to
		//    toi 16x32 o) nen -4 vinh vien; hoac ket DAO (o dang dung hop le nhung A*
		//    toi moi diem roam deu thua - map 193 sang 19/08: 2 bot thua 264 lan/50').
		//    Bot dung im vo han = canh "dung ngoai map" chu game thay.
		// 2) TAY KHONG: do ben ve 0 -> KItemList::Abrade THAO vu khi nem vao tui ->
		//    don tui xoa -> 210 bot [BotCast] BI TU CHOI (vukhi detail=-1), danh khong
		//    sut mau, chet lien tuc ve thanh. Goc da chan tai Abrade (bot khong mai
		//    mon nua); day la don chua nhung con DA mat vu khi tu truoc.
		if (nowAll - b.nCuuKiemTick >= (unsigned int)(GAME_FPS * 10))
		{
			b.nCuuKiemTick = nowAll;

			// -- phat lai vu khi nhap mon khi tay khong giua doi --
			// Chi khi DA tung nhan (nGaveWeapon=1 = vao phai xong), gion 60 giay,
			// tran 5 lan: phai quyen boc trung "tay khong" (w.d < 0) la co y,
			// thu vai lan roi thoi chu khong spam ItemSet.Add moi phut.
			if (b.nGaveWeapon && b.nFaction >= 0
			 && Player[nIdx].m_ItemList.GetWeaponType() < 0
			 && b.nVuKhiThu < 5
			 && nowAll - b.nVuKhiTick >= (unsigned int)(GAME_FPS * 60))
			{
				b.nVuKhiTick = nowAll;
				b.nVuKhiThu++;
				pb_Log("[BotVuKhi] %s tay khong giua doi (do ben ve 0 da bi thao/xoa?)"
				       " -> phat lai vu khi nhap mon (lan %d)\n",
				       Player[nIdx].m_PlayerName, b.nVuKhiThu);
				pb_GiveFactionWeapon(nIdx, b.nFaction);
				if (Player[nIdx].m_ItemList.GetWeaponType() >= 0)
					b.nAtkSkill = 0;   // ep chon lai chieu theo vu khi moi
			}

			// -- trang bi theo cap (19/08 chieu): tiem nang ton dong + Kim Phong
			// + ngua Tuc Suong + vu khi cap 10 khi dat 81. Thu MOT LAN moi khi
			// LEN CAP (gate nTrangBiLevel) de khoi tao-huy item moi 10 giay.
			if (b.nFaction >= 0 && b.nTrangBiLevel != Npc[nNpcIdx].m_Level)
			{
				b.nTrangBiLevel = Npc[nNpcIdx].m_Level;
				pb_TrangBiTheoCap(nIdx, nNpcIdx, b);
			}

			// -- tu cuu khoi ket luoi: dua ve diem dat chan bai cua chinh no --
			// (diem da qua ChangeWorld + buoc-ra kiem chung ca ngay). Chi xet khi
			// dang o DUNG map bai; o sai map thi pb_RaBai ngay duoi da ChangeWorld
			// (dich chuyen, khong can duong di) nen khong ket kieu nay.
			if (nowAll - b.nCuuTick >= (unsigned int)(GAME_FPS * 60)
			 && b.nBaiIdx >= 0 && b.nBaiIdx < PB_SO_BAI
			 && SubWorld[nSub].m_SubWorldID == s_bai[b.nBaiIdx].nMapId)
			{
				int nKcx = 0, nKcy = 0;
				Npc[nNpcIdx].GetMpsPos(&nKcx, &nKcy);
				int nKet = 0;
				// T1 (19/08 trua NOI LONG): o dang dung + CA 4 o lan can la vat can
				// tren LUOI = nam sau vung rong HOAC ket trong than tuong thanh (anh
				// chu game 10:35 - bot dung giua tuong map 79). BO dieu kien engine
				// GetBarrierMin==NULL: bot DANG DUNG duoc o do tuc engine da cho phep
				// tai offset do - dieu kien ay gan nhu luon dung, chi can tro viec cuu
				// bot ket trong tuong. O goc nua-trong le loi van an toan nho 4 lan can
				// (vanh goc mong 1 o, luon co o lan can di duoc).
				if (SubWorld[nSub].CellObsSrv(nKcx, nKcy) == 1
				 && SubWorld[nSub].CellObsSrv(nKcx - 32, nKcy) == 1
				 && SubWorld[nSub].CellObsSrv(nKcx + 32, nKcy) == 1
				 && SubWorld[nSub].CellObsSrv(nKcx, nKcy - 32) == 1
				 && SubWorld[nSub].CellObsSrv(nKcx, nKcy + 32) == 1)
					nKet = 1;
				// T2: 5 lan roam lien tiep khong co duong = ket dao A*
				if (!nKet && b.nAStarThua >= 5)
					nKet = 2;

				if (nKet)
				{
					const PB_BaiLuyen& baiC = s_bai[b.nBaiIdx];
					// diem ve: dat chan bai + o lech rieng (khuon 9x7 cua pb_RaBai)
					int nVeX = baiC.nOX * 32 + ((nLech % 9) - 4) * 2 * 32;
					int nVeY = baiC.nOY * 32 + (((nLech / 9) % 7) - 3) * 3 * 32;
					{
						int nRv = -1, nMXv = 0, nMYv = 0, nOXv = 0, nOYv = 0;
						SubWorld[nSub].Mps2Map(nVeX, nVeY, &nRv, &nMXv, &nMYv, &nOXv, &nOYv);
						if (nRv < 0
						 || SubWorld[nSub].m_Region[nRv].GetBarrierMin(nMXv, nMYv, nOXv, nOYv, TRUE)
						    != Obstacle_NULL)
						{
							nVeX = baiC.nOX * 32;   // o lech hong -> ve dung diem goc
							nVeY = baiC.nOY * 32;
						}
					}
					// Diem ve cung nam trong luoi chan = map hong du lieu toan phan:
					// khong dich chuyen lung tung ([PathSrv] CANH BAO da chi ro map).
					if (SubWorld[nSub].CellObsSrv(nVeX, nVeY) != 1)
					{
						b.nCuuTick = nowAll;
						b.nAStarThua = 0;
						pb_Log("[BotCuu] %s ket %s tai o(%d,%d) -> ve bai %s o(%d,%d)\n",
						       Player[nIdx].m_PlayerName,
						       (nKet == 1) ? "trong vung rong (nap lai cho cu?)"
						                   : "dao A* (5 lan lien tiep khong duong)",
						       nKcx / 32, nKcy / 32, baiC.szTen, nVeX / 32, nVeY / 32);
						Npc[nNpcIdx].SetPos(nVeX, nVeY);
						b.nTargetNpc = 0;
						b.nRoamX = 0;  b.nRoamY = 0;
						b.walk.Reset();
						b.chase.Reset();
						b.roam.Reset();
						b.follow.Reset();
						b.nFollowNghiToi = 0;
						b.loot.Reset();
						b.nJamTick = 0;
						// buoc ra khoi diem dat chan nhu vua doi map (ne trap/dam dong)
						const int nXa2   = (8 + (int)g_Random(4)) * 32;
						const int nGocX2 = ((int)g_Random(2) * 2 - 1);
						const int nGocY2 = ((int)g_Random(2) * 2 - 1);
						b.nBuocRaX = baiC.nOX * 32 + nGocX2 * nXa2;
						b.nBuocRaY = baiC.nOY * 32 + nGocY2 * nXa2;
					}
					else if (nKet == 2)
						b.nAStarThua = 0;   // map hong toan phan: xoa dem, khoi xet lai ngay
				}
			}
		}
// ---- DA TAU (19/08 trua): 0 = pb_DaTau dang tu dieu khien (doi thoai /
		// doi map) -> dung nhip nay tai day; 1 = dang farm cuon dung map nhiem vu
		// -> danh quai tai cho, KHONG cho pb_RaBai keo ve bai; 2 = khong lam.
		int nDT = 2;
		if (s_nPbDaTauMax > 0)
			nDT = pb_DaTau(nIdx, nNpcIdx, nSub, b, nLech);
		if (nDT == 0)
			return;

// (19/08 toi) trong thanh thi/thon thi CUOI NGUA cho giong nguoi choi
		pb_CuoiNguaTrongThanh(nIdx, nNpcIdx, nSub, b, nowAll);

		// Vong sang / bua / buff xet TRUOC khi danh: bot phai vao tran voi day du
		// trang thai, giong nguoi choi bam buff truoc roi moi lao vao.
		pb_ApplyAuraBuff(nIdx, nNpcIdx, b, nowAll);
		pb_DungVatPham(nIdx, nNpcIdx, b, nowAll, nLech);
		if (nDT == 1 || pb_RaBai(nIdx, nNpcIdx, nSub, b, nLech))
		{
			// Vua dat chan sang map moi thi viec DAU TIEN la buoc ra khoi diem dat chan
			// (sat trap) - lam truoc ca danh nhau, khong thi bot dung ke trap mai.
			if (b.nBuocRaX)
			{
				const int nRa = PB_WalkTo(nNpcIdx, b.nBuocRaX, b.nBuocRaY, nSub, b.roam, 96);
				if (nRa == 0)
					return;              // dang buoc ra
				if (nRa < 0)
					// khong chay thang thay the (nguon xuyen bien) - bo qua la du:
					// [BotLach] se tu keo bot ra neu that su ket canh trap
					pb_Log("[BotBien] %s khong co duong A* buoc ra diem dat chan -> bo qua\n",
					       Player[nIdx].m_PlayerName);
				b.nBuocRaX = 0;
				b.nBuocRaY = 0;
				b.roam.Reset();
			}
			// Co quai gan thi danh; het quai gan thi DI HOANG toi cho khac tren CUNG ban do.
			// Nho vay bot tu rai ra thay vi dam mot cho o diem ChangeWorld.
			pb_DonTui(nIdx, b, nowAll);

			// ---- GIAN DAN (chu game 18/08): quanh minh 20 o ma co HON 20 bot thi bo
			// cho nay, di tan toi diem quai XA (>= 1200 MPS, pb_FindRoamSpot) roi luyen
			// o do. Kiem ~8-15 giay/lan (so le theo chi so), sau moi lan di tan nghi
			// 60 giay. Thanh vien to doi KHONG tu y di tan (phai bam theo doi truong).
#define PB_DAN_DONG      20
#define PB_DAN_BAN_KINH  640
#define PB_DAN_KIEM_GAP  (GAME_FPS * 8)
#define PB_DAN_NGHI      (GAME_FPS * 60)
#define PB_DAN_HAN       (GAME_FPS * 40)
			if (b.nDanDenHan != 0)
			{
				// dang di tan: den noi (roam tu xoa dich) hoac het han -> luyen lai
				if (nowAll >= b.nDanDenHan || (b.nRoamX == 0 && b.nRoamY == 0))
				{
					b.nDanDenHan  = 0;
					b.nDanNghiToi = nowAll + (unsigned int)PB_DAN_NGHI;
				}
				else
				{
					b.nTargetNpc = 0;          // khong vua di vua ham danh
					pb_Roam(nIdx, nNpcIdx, nSub, b, nLech);
					return;
				}
			}
			else if (nowAll - b.nDanKiemTick >= (unsigned int)(PB_DAN_KIEM_GAP + (nLech & 7) * GAME_FPS)
			      && (b.nDanNghiToi == 0 || nowAll >= b.nDanNghiToi)
			      && !(pb_TrongNhom(nIdx) && pb_DoiTruongCua(nIdx) != nIdx))
			{
				b.nDanKiemTick = nowAll;
				int bx4 = 0, by4 = 0;
				Npc[nNpcIdx].GetMpsPos(&bx4, &by4);
				int nDong = 0;
				for (int q3 = 0; q3 < s_botCount; q3++)
				{
					const int p3 = s_bots[q3].nPlayerIdx;
					if (p3 <= 0 || p3 >= MAX_PLAYER || p3 == nIdx)
						continue;
					const int n3 = Player[p3].m_nIndex;
					if (n3 <= 0 || n3 >= MAX_NPC)
						continue;
					if (Npc[n3].m_SubWorldIndex != nSub)
						continue;
					int x3 = 0, y3 = 0;
					Npc[n3].GetMpsPos(&x3, &y3);
					if (g_GetDistance(bx4, by4, x3, y3) <= PB_DAN_BAN_KINH)
						if (++nDong > PB_DAN_DONG)
							break;
				}
				if (nDong > PB_DAN_DONG)
				{
					int rx4 = 0, ry4 = 0;
					if (pb_FindRoamSpot(nIdx, nNpcIdx, nSub, nLech, &rx4, &ry4))
					{
						b.nRoamX = rx4;
						b.nRoamY = ry4;
						b.roam.Reset();
						b.nDanDenHan = nowAll + (unsigned int)PB_DAN_HAN;
						b.nTargetNpc = 0;
						b.chase.Reset();
						{
							static unsigned int s_uDanLog = 0;
							if (nowAll - s_uDanLog >= (unsigned int)(GAME_FPS / 2))
							{
								s_uDanLog = nowAll;
								pb_Log("[BotDan] %s quanh 20 o co %d bot -> di tan toi o(%d,%d)\n",
								       Player[nIdx].m_PlayerName, nDong, rx4 / 32, ry4 / 32);
							}
						}
					}
					// khong con diem quai xa (map nho/het quai) -> thoi, 8-15s sau kiem lai
				}
			}
			// NHAT TRUOC - DANH SAU (chu game 18/08: "chua thay nhat"): truoc day nhat
			// chi chay khi pb_Fight HET muc tieu, ma bai quai day thi giet xong con nay
			// lap tuc khoa con ke -> do roi nam cho toi khi sach quai (log 13:07: 1901
			// don danh / 63 lan nhat). Dao len truoc: giet xong thay do CUA MINH roi la
			// ra nhat vai giay roi danh tiep - y nhu nguoi that. An toan vi pb_NhatDo
			// chi nhan do m_nBelong = chinh minh (do minh giet moi co), khong bao gio
			// bo danh di nhat do thien ha.
			if (pb_NhatDo(nIdx, nNpcIdx, nSub, b))
			{
				b.nRoamX = 0;      // dang nhat do -> hoan di hoang
				b.nRoamY = 0;
			}
			else if (pb_Fight(nIdx, nNpcIdx, nSub, b))
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
			// CAP TUI DUOC PHAM VINH VIEN - di dung script co san cua du an
			// (vatpham.lua:187-190: AddItem(6,1,4813) khong han + khoa vinh vien).
			Player[nIdx].ExecuteScript((char*)"\\script\\global\\vatpham.lua",
			                           (char*)"tuiduocpham1238", 0, false);
			pb_Log("[BotTui] %s nhan tui duoc pham vinh vien\n", Player[nIdx].m_PlayerName);
		}

		// Tieu luon so diem dang ton (nhan vat mau co the da tich san), tu do moi lan len cap
		// se tieu tiep o nhanh PB_AI_IN_FACTION ben tren.
		b.nLastLevel = Npc[nNpcIdx].m_Level;
		pb_AllocAttribPoints(nIdx, b.nFaction);
		// (18/08) LUU NGAY sau khi vao phai xong: chot phai + ky nang + vu khi vao DB,
		// khoi mat neu server sap truoc nhip luu dinh ky 30 giay ke tiep.
		Player[nIdx].SetLoginType(0);   // (phan bien) chan co SetLogoutRV neu Lua vua lat
		if (s_pfnSend && s_pfnSend(PB_ASK_SAVEROLE, (unsigned long)nIdx, Player[nIdx].m_PlayerName))
			b.nLuuTick = (unsigned int)g_SubWorldSet.GetGameTime();
	}
	else
		pb_Log("[Bot] %s XIN VAO %s THAT BAI: m_nCurFaction=%d (he bot=%d, he phai can=%d)\n",
		       Player[nIdx].m_PlayerName, s_facNpc[b.nFaction].szTen,
		       (int)Player[nIdx].m_cFaction.m_nCurFaction,
		       Npc[nNpcIdx].m_Series, s_facNpc[b.nFaction].nSeries);
}

// ---------------------------------------------------------------- nhip
// Nhip quan ly to doi ~5 giay (khuon ManageGrindParties, gian luoc):
//   - thanh vien mo coi (doi truong mat/khac map) -> roi nhom
//   - doi truong trong nhom -> giai tan
//   - moi 10 phut XAO lai (chi giai tan; luot 5s sau tu ghep lai to hop khac)
//   - ghep bot tu do CUNG MAP co nWantParty, tran tong PB_NHOM_RATE% ca dan
static void pb_QuanLyNhom()
{
	static DWORD s_dwMocQuan = 0, s_dwMocXao = 0;
	const DWORD dwNow = GetTickCount();
	if (s_dwMocQuan && dwNow - s_dwMocQuan < PB_NHOM_QUAN_MS)
		return;
	s_dwMocQuan = dwNow;
	if (s_dwMocXao == 0)
		s_dwMocXao = dwNow;
	const bool bXao = (dwNow - s_dwMocXao >= PB_NHOM_XAO_MS);

	int aTuDo[PB_MAX_BOTS];   int nTuDo   = 0;
	int aTruong[PB_MAX_BOTS]; int nTruong = 0;
	int nDangNhom = 0;
	int nDemDanh = 0, nDemMuon = 0;       // chan doan: dem RIENG tung dieu kien

	for (int i = 0; i < s_botCount; i++)
	{
		PB_Bot& b = s_bots[i];
		const int p = b.nPlayerIdx;
		if (p <= 0 || p >= MAX_PLAYER || Player[p].m_dwID != b.dwID)
			continue;
		const int n = Player[p].m_nIndex;
		if (n <= 0 || n >= MAX_NPC)
			continue;
		const bool bChet = (Npc[n].m_Doing == do_death || Npc[n].m_Doing == do_revive);

		if (pb_TrongNhom(p))
		{
			nDangNhom++;
			if (bXao)
			{
				// luot xao CHI giai tan/roi - luot 5s ke tiep tu ghep to hop moi
				pb_RoiNhom(p, "xao lai nhom dinh ky");
				continue;
			}
			const int c = pb_DoiTruongCua(p);
			if (c == p)
			{
				if (g_Team[Player[p].m_cTeam.m_nID].m_nMemNum <= 0)
				{
					pb_RoiNhom(p, "khong con thanh vien");
					continue;
				}
				if (!bChet && b.nAi == PB_AI_FIGHT
				 && g_Team[Player[p].m_cTeam.m_nID].m_nMemNum < PB_NHOM_MAX_MEM
				 && nTruong < PB_MAX_BOTS)
					aTruong[nTruong++] = i;
			}
			else
			{
				const int nCapNpc = (c > 0) ? Player[c].m_nIndex : 0;
				if (c <= 0 || !PB_IsBot(c) || nCapNpc <= 0 || nCapNpc >= MAX_NPC)
					pb_RoiNhom(p, "doi truong khong con");
				else if (Npc[nCapNpc].m_SubWorldIndex != Npc[n].m_SubWorldIndex)
					pb_RoiNhom(p, "doi truong da sang map khac");
			}
			continue;
		}

		if (b.nAi == PB_AI_FIGHT) nDemDanh++;
		if (b.nWantParty)         nDemMuon++;
		// (D20) bot dang lam Da Tau KHONG vao danh sach ghep nhom: vua toi map
		// nhiem vu da bi moi vao nhom roi bi keo di theo doi truong (log 16:31:40:
		// LyKhang664 kich Xa Phu va vao nhom trong CUNG MOT giay).
		if (bChet || !b.nWantParty || b.nAi != PB_AI_FIGHT || b.nDaTauChon || b.nBanSap)
			continue;
		if (nTuDo < PB_MAX_BOTS)
			aTuDo[nTuDo++] = i;
	}

	if (bXao)
	{
		s_dwMocXao = dwNow;
		return;
	}

	// tong ket 30 giay/lan de bot.log ke duoc vi sao chua ghep (chu game 18/08
	// chieu: "bot chua party" - so lieu thay vi doan mo)
	{
		static int s_nDemTk = 0;
		if (++s_nDemTk >= 6)
		{
			s_nDemTk = 0;
			pb_Log("[BotNhom] tk: %d bot | %d trong nhom | %d tu do muon nhom | %d truong | dang danh %d | muon nhom %d\n",
			       s_botCount, nDangNhom, nTuDo, nTruong, nDemDanh, nDemMuon);
		}
	}

	// tran tong: khong qua PB_NHOM_RATE% so bot dang song nam trong nhom
	if (nDangNhom >= s_botCount * PB_NHOM_RATE / 100)
		return;

	// dap them vao nhom con cho (cung map) truoc
	for (int q = 0; q < nTruong && nTuDo > 0; q++)
	{
		const int pc = s_bots[aTruong[q]].nPlayerIdx;
		const int nc = Player[pc].m_nIndex;
		for (int f = 0; f < nTuDo; )
		{
			if (!pb_TrongNhom(pc)
			 || g_Team[Player[pc].m_cTeam.m_nID].m_nMemNum >= PB_NHOM_MAX_MEM)
				break;
			const int pf = s_bots[aTuDo[f]].nPlayerIdx;
			const int nf = Player[pf].m_nIndex;
			if (Npc[nf].m_SubWorldIndex == Npc[nc].m_SubWorldIndex && pb_GhepNhom(pc, pf))
				aTuDo[f] = aTuDo[--nTuDo];
			else
				f++;
		}
	}

	// ghep nhom MOI: chi giu nhom khi co it nhat mot ban cung map vao duoc
	for (int f = 0; f + 1 < nTuDo; f++)
	{
		const int pA = s_bots[aTuDo[f]].nPlayerIdx;
		const int nA = Player[pA].m_nIndex;
		if (pb_TrongNhom(pA))
			continue;
		int nGhep = 0;
		for (int g2 = f + 1; g2 < nTuDo && nGhep < PB_NHOM_MAX_MEM; )
		{
			const int pB = s_bots[aTuDo[g2]].nPlayerIdx;
			const int nB = Player[pB].m_nIndex;
			if (Npc[nB].m_SubWorldIndex == Npc[nA].m_SubWorldIndex && pb_GhepNhom(pA, pB))
			{
				aTuDo[g2] = aTuDo[--nTuDo];
				nGhep++;
			}
			else
				g2++;
		}
		// khong ai vao duoc ma lo tao nhom rong (luat "khong doi truong don doc"
		// cua ban tham khao, KBotManager.cpp:1957-1963) -> giai tan ngay
		if (nGhep == 0 && pb_TrongNhom(pA) && pb_DoiTruongCua(pA) == pA
		 && g_Team[Player[pA].m_cTeam.m_nID].m_nMemNum <= 0)
			pb_RoiNhom(pA, "khong tim duoc ban cung map");
	}
}

// (D6) moc bu quan so Da Tau - xem ghi chu trong than ham
static unsigned int s_dwMocBuDT = 0;

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

	// (D6) BU QUAN SO DA TAU moi 60 giay. Khe bot tai dung / nap lai bot deu
	// xoa nDaTauChon (khoi tao khe), khong ai bu -> quan so Da Tau chi tut ve 0.
	// PB_SetDaTau nay CHI them phan chenh lech (khong dung bot dang lam) nen
	// goi lai la an toan tuyet doi.
	if (s_nPbDaTauMax > 0)
	{
		const unsigned int dwNowDT = (unsigned int)g_SubWorldSet.GetGameTime();
		if (dwNowDT - s_dwMocBuDT >= (unsigned int)(GAME_FPS * 60))
		{
			s_dwMocBuDT = dwNowDT;
			PB_SetDaTau(s_nPbDaTauMax);
		}
	}

	// nhip tung bot (di bo + vao phai) - kem DO HIEU NANG (dieu tra lag 18/08):
	// moi 10 giay in [BotPerf] tong thoi gian nhip bot + so lan A* + so lan quet do.
	{
		static __int64 s_nPerfTong = 0;
		static DWORD   s_dwPerfMoc = 0;
		static LARGE_INTEGER s_liTanSo = { 0 };
		if (s_liTanSo.QuadPart == 0)
			QueryPerformanceFrequency(&s_liTanSo);
		LARGE_INTEGER liT0, liT1;
		QueryPerformanceCounter(&liT0);

		for (int i = 0; i < s_botCount; i++)
			pb_DriveBot(s_bots[i]);
		pb_QuanLyNhom();
		pb_ChatTheGioi();

		QueryPerformanceCounter(&liT1);
		s_nPerfTong += liT1.QuadPart - liT0.QuadPart;
		const DWORD dwNow = GetTickCount();
		if (s_dwPerfMoc == 0)
			s_dwPerfMoc = dwNow;
		else if (dwNow - s_dwPerfMoc >= 10000)
		{
			const int nMs = (int)(s_nPerfTong * 1000 / (s_liTanSo.QuadPart ? s_liTanSo.QuadPart : 1));
			pb_Log("[BotPerf] nhip bot %d ms/10s | A* %d lan | quet do %d lan | luu %d goi | %d bot\n",
			       nMs, g_nPbAstarDem, s_nPerfQuetDo, s_nPerfLuuDem, s_botCount);
			s_nPerfTong   = 0;
			g_nPbAstarDem = 0;
			s_nPerfQuetDo = 0;
			s_nPerfLuuDem = 0;
			s_dwPerfMoc   = dwNow;
		}
	}

	// ---- (18/08) LUU DINH KY / LUU EP TOAN BO ----
	// Dinh ky: toi da 2 goi/nhip (36 goi/giay), moi bot den han PB_LUU_MOI_GIAY (30
	// giay) - 1000 bot hoi tu ~31 giay/vong (do phan bien mo phong). PB_SaveAll (s_bLuuTatCa):
	// moi nhip gui toi da 5 goi cho bot CHUA luu ke tu moc s_nLuuTatCaMoc - dong XONG
	// chi in khi MOI bot hop le deu da gui that (phan bien dot 2: ban dem-theo-so-goi
	// cu gui trung bot da luu va in XONG trong khi bot dang chet chua he duoc luu).
	// Duong luu: s_pfnSend -> GameServer::SavePlayerData(idx,false) ->
	// SavePlayerDataAtOnce (dong dau PB_BLOB_DAU) -> CRC -> c2s_roleserver_saveroleinfo.
	if (s_botCount > 0 && s_pfnSend)
	{
		const unsigned int nowLuu = (unsigned int)g_SubWorldSet.GetGameTime();
		if (s_bLuuTatCa)
		{
			int nConThieu = 0;
			int nGui      = 5;
			for (int iL = 0; iL < s_botCount; iL++)
			{
				PB_Bot& lb = s_bots[iL];
				const int li = lb.nPlayerIdx;
				if (li <= 0 || li >= MAX_PLAYER || Player[li].m_dwID != lb.dwID)
					continue;              // khe hong: khong the luu, khong tinh thieu
				if ((int)(lb.nLuuTick - s_nLuuTatCaMoc) >= 0)
					continue;              // da luu trong dot nay roi
				const int nNpcLuu = Player[li].m_nIndex;
				if (nNpcLuu <= 0 || nNpcLuu >= MAX_NPC)
					continue;
				if (Npc[nNpcLuu].m_Doing == do_death || Npc[nNpcLuu].m_Doing == do_revive)
				{
					nConThieu++;           // cho hoi sinh (~3 giay) roi luu not
					continue;
				}
				if (nGui <= 0)
				{
					nConThieu++;           // qua nhip sau gui tiep
					continue;
				}
				Player[li].SetLoginType(0);
				if (s_pfnSend(PB_ASK_SAVEROLE, (unsigned long)li, Player[li].m_PlayerName))
				{
					lb.nLuuTick = nowLuu;
					nGui--;
					s_nPerfLuuDem++;
				}
				else
					nConThieu++;
			}
			if (nConThieu == 0)
			{
				s_bLuuTatCa = 0;
				pb_Log("[BotLuu] da gui luu XONG toan bo bot - doi them ~10 giay cho"
				       " Goddess ghi not roi hay tat server\n");
			}
		}
		else
		{
			// 2 goi/nhip = 36 goi/giay: du cho 1000 bot nhip 30s (can 33,3/s); dong
			// hon thi nhip tu gian ra (bo dieu tiet), khong bao gio qua tai Goddess.
			int nGui = 2;
			int nXet = (s_botCount < 16) ? s_botCount : 16;
			while (nXet-- > 0 && nGui > 0)
			{
				if (s_nLuuCon >= s_botCount)
					s_nLuuCon = 0;
				PB_Bot& lb = s_bots[s_nLuuCon++];
				const int li = lb.nPlayerIdx;
				if (li <= 0 || li >= MAX_PLAYER || Player[li].m_dwID != lb.dwID)
					continue;
				// (phan bien) khe mat NPC thi dung luu (SavePlayerBaseInfo doc Npc[m_nIndex])
				const int nNpcLuu = Player[li].m_nIndex;
				if (nNpcLuu <= 0 || nNpcLuu >= MAX_NPC)
					continue;
				// (phan bien) bot dang chet: luu luc nay ghi cUseRevive=1 lam bot >= cap
				// 20 mat vi tri - no tu hoi sinh sau ~3 giay, de nhip sau luu.
				if (Npc[nNpcLuu].m_Doing == do_death || Npc[nNpcLuu].m_Doing == do_revive)
					continue;
				if (nowLuu - lb.nLuuTick < (unsigned int)(GAME_FPS * PB_LUU_MOI_GIAY))
					continue;
				// (phan bien) ep co hoi-sinh-khi-vao ve 0 truoc MOI lan luu - script
				// nao do (citywar SetLogoutRV) co the vua lat len 1.
				Player[li].SetLoginType(0);
				if (s_pfnSend(PB_ASK_SAVEROLE, (unsigned long)li, Player[li].m_PlayerName))
				{
					lb.nLuuTick = nowLuu;
					nGui--;
					s_nPerfLuuDem++;
				}
			}
		}
	}

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
