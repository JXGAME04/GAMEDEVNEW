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
	unsigned int nLachToi;                    // (20/08) dang DI BO lach ngang toi nhip nay:
	                                          // phai de yen, khong cho nhanh khac phat do_run
	                                          // de len (m_Command chi co MOT khe)
	int          nLachDem;                    // dem lan lach - xoay huong khoi dau, chong
	                                          // lach qua roi lach nguoc lai cung mot cap o
	// ---- dung vat pham ----
	unsigned int nUongTick;                   // lan uong binh gan nhat
	unsigned int nMoTuiTick;                  // lan mo tui duoc pham gan nhat
	int          nHpTruoc;                    // HP muc tieu nhip truoc (do sat thuong that)
	unsigned int nDameTick;                   // lan xac nhan sat thuong gan nhat (gion log)
	// ---- luyen cap ----
	int          nBaiIdx;                     // chi so bai luyen dang nham (-1 = chua chon)
	int          nBaiLevel;                   /// (20/08) MOC cap luc chon bai (10/20/../90), KHONG phai cap: doi moc moi chon lai
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
	int          nHoc90;                      // (20/08) 1 = da goi hoc bo ky nang 90 doi nay
	                                          // (bot cu nap lai goi lai 1 lan - vo hai vi
	                                          // KSkillList::Add khong bao gio HA cap chieu)
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
	// ---- TONG KIM (21/08 - chu game) ----
	int          nTk;                         // 0 = khong; 1 = ve map bao danh;
	                                          // 2 = di toi NPC bao danh; 3 = hau doanh;
	                                          // 4 = da ra tran dang danh; 5 = het tran, dung phu
	int          nTkPhe;                      // 1 = Tong, 2 = Kim (script quyet dinh camp that)
	unsigned int nTkTick;                     // moc vao pha hien tai
	int          nTkDichX, nTkDichY;          // diem doanh trai DICH dang chay toi (MPS)
	unsigned int nTkDichTick;                 // moc boc diem dich
	int          nTkGoiThu;                   // so lan da goi script bao danh
	int          nTkSanIdx;                   // [TK-SAN 28/08] npc idx dich dang san
	int          nTkKetPha;                   // [TK-KET 28/08] pha dang do nhuc nhich
	unsigned int nTkKetTick;                  // moc vao pha (tran cung 6 x han pha)
	int          nTkKetX;                     // mau vi tri lan truoc (mps)
	int          nTkKetY;
	int          nTkMuaXong;                  // 1 = da ghe Quan Y mua thuoc
	unsigned int nTkRaTick;                   // moc goi lai trap ra trai
	unsigned int uKet3Tick;                   // [TKKET3 01/09] dung yen mot cho o pha 3 tu luc nao (CHI de log)
	int          nKet3X;                      // [TKKET3] neo toa do do dung-yen
	int          nKet3Y;
	unsigned int uKet3Log;                    // [TKKET3] tiet che dong [TkKet3] 10 giay/con
	unsigned int uKet3Trap;                   // [TKKET3] tiet che dong [TkKet3-TRAP] 15 giay/con
	unsigned int nTkChoRa;                    // moc SOM NHAT duoc phep ra khoi trai
	int          nTkDaBoc;                    // (21/08) 0 = chua boc diem trung gian
	                                          // cho chang nay, 1 = boc roi
	int          nTkTgX, nTkTgY;              // diem TRUNG GIAN dang di toi (MPS), 0 = khong
	unsigned int nTkTgTick;                   // moc bat dau chang trung gian
	int          nSubTruoc;                   // subworld nhip truoc (-1 = chua biet)
	int          nBanSapXong;                 // 1 = da bay hang + mo sap
	int          nBanSapNgoi;                 // 1 = da ngoi xuong (do_sit)
	int          nSapChoXong;                 // [SapRai 28/08] 1 = da xep CHO ngoi rieng
	int          nSapDot;                     // (19/08 toi #4) so dot cham hang da qua -
	                                          // tron vao phep gieo de moi dot ra mat hang
	                                          // khac nhau (g_Random dong bang theo giay)
	// ---- may trang thai Da Tau (19/08 toi #2) ----
	int          nDtPha;                      // DTB_* - pha dang o
	int          nDtToiNpcThu;                // so nhip da co gang toi NPC
	int          nDtXaFuThu;                  // so lan kich godatau ma chua sang map
	int          nDtTraThu;                   // so lan tra ma course van = 1
	int          nDtTuiThu;                   // so lan tra hut vi tui day
	int          nDtCoCu;                     // so cuon nhip truoc (do tien do)
	unsigned int nDtFarmMoc;                  // moc lan CUOI cuon tang (chong farm vo han)
	int          nDtNeoMap;                   // (19/08 toi #4) xich cho MAP NHIEM VU loai 4:
	int          nDtNeoX, nDtNeoY;            // map + diem MPS dat chan dau tien (0 = chua co);
	                                          // chi co nghia khi nDtPha == DTB_FARM_NV va map khop
	unsigned int nVeThanhHen;                 // (19/08 toi #4) che do VE THANH: moc duoc phep
	                                          // doi map (so le 60s; 0 = chua hen)
	int          nRaBaiThu;                   // (20/08) duong Xa Phu: dem GIAY vat va (tran 300 = 5 phut)
	                                          // (di bo/khong thay NPC/A* thua) -> qua nguong
	                                          // thi fallback teleport neo-da-kiem
	int          nRaBaiGoi;                   // so lan da "bam menu" Xa Phu ma chua sang map
	int          nThanhNhaMap;                // thanh nha DONG: map thanh da "luu ruong" gan
	                                          // nhat (che do VE THANH dat); 0 = dung mac dinh
	                                          // s_dtNpc[nLech %% 10]
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

// ===========================================================================
// TONG KIM - hang so (dat o day vi pb_FindTarget can PB_TK_MAP)
//
// He Tong Kim cua may chu nay la ban "trung cap" (script\tinhnang\tong_kim_tcap).
// TUYET DOI khong tra theo tai lieu cu: KHONG co map 380/325, khong co
// baodanh-tongkim.lua, khong co KBotAutoAI_TK.cpp trong cay nay.
// ===========================================================================
#define PB_TK_MISSION      1      // MS_TONGKIM        (script\lib\lib_task.lua:282)
#define PB_TK_MAP_BD     324      // MAP_BD_TC         (lib_tktc.lua:53)
#define PB_TK_MAP        379      // MAP_TK_TC         (lib_tktc.lua:54)
#define PB_TK_CAP_MIN     80      // LEVEL_ENOUGH_TK   (lib_tktc.lua:19)
#define PB_TK_TMP_NPC      7      // TMP_INDEX_NPC     (lib_task.lua:262)
#define PB_TK_MVITRI      47      // M_VITRI_TRENDUOI  (lib_task.lua:339)
                                  // 1 = phe Tong o vi tri 1; 2 = HAI PHE DOI CHO
                                  // (lib_tktc.lua:486-508 hoan toan bo bang toa do).
                                  // Doc bang SubWorld[n].m_MissionArray.GetMissionValue()
                                  // - gia tri nam o cap MANG, KHONG phai cap mission.

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
// [BotTHP 28/08] khai bao truoc: dinh nghia nam duoi (tim "static int pb_ODat"),
// ma khoi gac goc-ket trong PB_OnRoleData (nam tren) can goi.
static int pb_ODat(int nSubIdx, int nOX, int nOY, int nLech, int nRMax,
                   int* pnX, int* pnY);

// [BotNoi 28/08] Chia dan 2 duong (chu game: "random ti le bot 2 duong noi - ngoai
// cho can bang; NOI CONG la KHONG can vu khi"): dwID LE = duong NOI CONG (tay khong,
// pb_PickSkill von uu tien chieu phep cho dung nhom dwID le - bThienNoi 23/08),
// dwID CHAN = duong NGOAI CONG (cam vu khi nhu cu). dwID roledb cap tuan tu nen
// ~50/50 va ON DINH giua cac phien - bot khong doi duong sau restart.
static int pb_BotNoi(int nIdx)
{
	return (int)(Player[nIdx].m_dwID & 1);
}

// [BotNoi-PHAI 28/08] Chu game: "fix vu khi lam loi mot so phai TOAN ky nang ngoai
// cong can vu khi (Thien Vuong - Duong Mon)". Duong NOI chi ap dung khi PHAI THAT SU
// co chieu danh duoc TAY KHONG mang don sat thuong PHEP - quet danh sach chieu cua
// chinh bot bang DUNG bo loc cua pb_PickSkill voi nWant = -1 (series khop he,
// radius > 0, khong aura/self, nham dich, style Missles/Melee, eqt -1/-2, du cap).
// TV/DM khong co chieu nao nhu vay -> tu roi ve duong NGOAI giu vu khi; phai khac
// (Ngu Doc/Nga My/Thuy Yen/Vo Dang/Con Lon...) van chia doi nhu chu muon. Bot chua
// du cap dung chieu phep thi TAM giu vu khi, len cap pb_TrangBiTheoCap se thao sau.
// [SPARSE 31/08] m_DamageAttribs la MANG THUA 17 o danh chi so theo LOAI don
// (KSkills.cpp: [2]magic [9]physics [10]cold [11]fire [12]lighting [13]poison),
// GetDamageAttribsNum() chi la BO DEM. Quet "a < nDaNum" (ban cu) chi doc vai o DAU
// thuong rong => moi phep thu don-phep luon 0. Mang KHONG duoc memset nen phai so
// DUNG enum chu khong the chi kiem khac 0.
#define PB_DA_LA(p, slot, val)  ((p)->GetDamageAttribs()[(slot)].nAttribType == (val))

// don sat thuong PHEP THAT (KHONG tinh doc: do that 199k cu cho thay doc khong bao
// mon quai tren build nay - xem 303-DOC)
static int pb_DonPhepThat(KSkill* p)
{
	return PB_DA_LA(p, 2,  magic_magicdamage_v)
	    || PB_DA_LA(p, 10, magic_colddamage_v)
	    || PB_DA_LA(p, 11, magic_firedamage_v)
	    || PB_DA_LA(p, 12, magic_lightingdamage_v);
}

static int pb_DonDoc(KSkill* p)
{
	return PB_DA_LA(p, 13, magic_poisondamage_v);
}

// o [9] dung chung cho physicsdamage_v va physicsenhance_p (KSkills.cpp cung mot case)
static int pb_DonVatLy(KSkill* p)
{
	return PB_DA_LA(p, 9, magic_physicsdamage_v) || PB_DA_LA(p, 9, magic_physicsenhance_p);
}

static int pb_CoChieuNoiTayKhong(int nNpcIdx)
{
	KSkillList& sl = Npc[nNpcIdx].m_SkillList;
	for (int i = 1; i < MAX_NPCSKILL; i++)
	{
		const int id = sl.m_Skills[i].SkillId;
		if (id <= 0 || id >= MAX_SKILL)
			continue;
		int lv = sl.m_Skills[i].CurrentSkillLevel;
		if (lv <= 0) lv = sl.m_Skills[i].SkillLevel;
		if (lv <= 0) lv = 1;
		KSkill* p = (KSkill*)g_SkillManager.GetSkill(id, lv);
		if (!p)
			continue;
		{
			const int nSr = p->GetSkillSeries();
			if (nSr >= 0 && nSr < series_num && nSr != Npc[nNpcIdx].m_Series)
				continue;
		}
		if (p->GetAttackRadius() <= 0) continue;
		if (p->IsAura())               continue;
		if (p->IsTargetSelf())         continue;
		if (!p->IsTargetEnemy())       continue;
		{
			const int nSt = p->GetSkillStyle();
			if (nSt != SKILL_SS_Missles && nSt != SKILL_SS_Melee)
				continue;
		}
		const int eq = p->GetEquipLimit();
		if (eq != -2 && eq != -1)
			continue;              // can vu khi ho cu the -> tay khong khong dung duoc
		int nRq = p->GetSkillReqLevel();
		if (nRq > 80) nRq = 80;
		if (Npc[nNpcIdx].m_Level < nRq)
			continue;              // chua du cap - cast se bi tu choi im lang
		// [SPARSE 31/08] doc theo O (ban cu quet "a < nDaNum" nen luon tra 0 -> duong
		// noi/ngoai nam im tu 28/08). Doc bi loai theo 303-DOC.
		if (pb_DonPhepThat(p))
			return 1;
	}
	return 0;
}

// [NOI-HOAN 31/08] Duong NOI CONG (28/08) tren thuc te NAM IM tu dau: ban cu quet
// "a < GetDamageAttribsNum()" tren MANG THUA nen pb_CoChieuNoiTayKhong luon tra 0
// ("[BotNoi]" = 0 dong tren toan bo 115 MB bot.log). Ban va [SPARSE 31/08] lam no
// SONG LAI - ma dau ra cua no gac khoi HUY VU KHI trong pb_TrangBiTheoCap
// (RemoveItemIdx) va chan pb_GiveFactionWeapon phat lai.
// Do that (skills.txt x factionhead.lua SKILLNORMAL, cap bot 110): 8/10 phai co
// chieu phep tay khong hop le (TL 271 / NM 80,82,91 / TY 102,113,111 / CB 122,128 /
// TN 145,138,148 / VD 153,164,165 / CL 179,182). Voi pb_BotNoi = dwID&1 (~50%%) thi
// ~400/1000 bot se bi huy vu khi NGAY lan pb_TrangBiTheoCap dau sau restart.
// Chu game CHUA duyet, va viec do KHONG lien quan loi "bot Duong Mon dung yen".
// => TAM NGAT, giu nguyen hanh vi dang chay. Doi 0 -> 1 khi muon test rieng.
#define PB_BAT_DUONG_NOI  0

// duong NOI THAT SU = dwID le VA phai co chieu noi tay khong dung duoc ngay bay gio
static int pb_BotNoiThat(int nIdx)
{
#if PB_BAT_DUONG_NOI
	if (!pb_BotNoi(nIdx))
		return 0;
	const int nNpcIdx = Player[nIdx].m_nIndex;
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
		return 0;
	return pb_CoChieuNoiTayKhong(nNpcIdx);
#else
	(void)nIdx;   // [NOI-HOAN 31/08] xem ghi chu ngay tren
	return 0;
#endif
}

// [DT-THP 28/08] Toa do TRUNG TAM thanh/thon cua Than Hanh Phu - chep nguyen van
// THON_TT_MP_ARRAY (script/item/ib/shenxingfu.lua:18-48). Chu game: "moi toa do
// chinh o Than Hanh Phu". Dung cho: gac goc-ket luc goi bot vao game (PB_OnRoleData)
// va diem dap "phu ve thanh" (pb_DtVeThanh).
static const int s_aThpDiem[][3] = {   // {mapid, oX, oY}
	{1,1591,3170},{78,1565,3219},{11,3124,5117},{162,1576,3133},{37,1722,3081},
	{80,1756,3006},{176,1573,2933},{20,3552,6194},{53,1622,3189},{99,1628,3203},
	{101,1688,3154},{100,1628,3203},{121,1951,4509},{153,1605,3220},{174,1573,3203},
	{54,1650,3169},{175,1673,3168},{103,1668,3131},{59,1650,3169},{183,1417,3540},
	{25,4046,5170},{13,1899,4978},{154,382,1369},{115,1523,3780},{45,1603,3191},
	{81,1690,3169},{131,1526,3152},
};
static int pb_ThpDiem(int nMapId, int* pnX, int* pnY)
{
	for (int i = 0; i < (int)(sizeof(s_aThpDiem) / sizeof(s_aThpDiem[0])); i++)
		if (s_aThpDiem[i][0] == nMapId)
		{
			*pnX = s_aThpDiem[i][1];
			*pnY = s_aThpDiem[i][2];
			return 1;
		}
	return 0;
}

static void pb_Log(const char* szFmt, ...)
{
	char szBuf[600];
	va_list ap;
	va_start(ap, szFmt);
	_vsnprintf(szBuf, sizeof(szBuf) - 1, szFmt, ap);
	va_end(ap);
	szBuf[sizeof(szBuf) - 1] = 0;

	// (21/08 - chu game: "tat nhung log in len gameserver tu bot do ban them vao,
	// doi qua in ra file log") BO echo console. Moi dong log cua he bot deu di
	// qua ham nay ([BotTK], [BotDanh], [BotNhat], [BotNgoai], [PathSrv],
	// [BotTrap]...) nen bo o day la tat duoc CA HE. Cua so GameServer giu nguyen
	// nhung dong thong tin cua chinh may chu; nhat ky bot xem trong bot.log.

	// (21/08 - chu game: "log nao khong dung nua thi tat di") Do thuc te: bot.log
	// dat 430 MB trong MOT phien. Loc ngay tai day theo NHAN dau dong - mot cho
	// duy nhat, khong phai sua hang tram diem goi. Muon bat lai mot nhan nao thi
	// chi viec xoa no khoi bang duoi.
	//
	// Cac nhan bi tat la cua nhung dot dieu tra DA DONG (nhat do 18/08, tui/xoa
	// do, sat thuong, to doi, hoang mang). Cac nhan cua dot dang chay - [BotTK],
	// [BotTrap], [BotDanh], [BotVongSang], [BotUong], [BotKet], [BotBien],
	// [BotLach], [BotCuu], [BotNgoai], [PathSrv] - GIU NGUYEN.
	{
		static const char* const s_szTat[] = {
			"[BotNhat]", "[BotXoaDo]", "[BotTui]",
			"[BotDame]", "[BotHoang]", "[BotNhom]",
		};
		for (int i = 0; i < (int)(sizeof(s_szTat) / sizeof(s_szTat[0])); i++)
		{
			const char* p = s_szTat[i];
			if (strncmp(szBuf, p, strlen(p)) == 0)
				return;
		}
	}

	// [24/08] Giu tep MO SAN + flush theo lo (giong g_AutoLog, KCore.cpp:806).
	// Truoc day moi dong ton mot cap fopen/fclose (30-200us); do that 24/08:
	// bot.log phinh 128 MB trong mot gio (~360 dong/giay) => 10-70 ms moi giay
	// bi tieu tren LUONG CHINH, chua ke luc Tong Kim dong nguoi.
	static FILE*  s_pTep    = NULL;	// tep giu mo
	static long   s_lCo     = 0;	// kich thuoc cong don (khoi ftell moi dong)
	static int    s_nTuFlush = 0;	// so dong ke tu lan flush truoc
	static DWORD  s_dwFlush = 0;	// moc flush gan nhat
	static int    s_nEpFlush = -1;	// [BotLog] Flush=1 -> flush moi dong

	if (s_nEpFlush < 0)
		s_nEpFlush = (int)GetPrivateProfileIntA("BotLog", "Flush", 0, ".\\config.ini");

	if (!s_pTep)
	{
		s_pTep = fopen("bot.log", "a");
		if (!s_pTep)
			return;
		fseek(s_pTep, 0, SEEK_END);
		s_lCo = ftell(s_pTep);	// chi mot lan luc mo
	}

	FILE* f = s_pTep;
	int nGhi = 0;
	time_t tNow = time(NULL);
	struct tm* pT = localtime(&tNow);
	if (pT)
		nGhi = fprintf(f, "[%02d/%02d %02d:%02d:%02d] %s",
			  pT->tm_mday, pT->tm_mon + 1, pT->tm_hour, pT->tm_min, pT->tm_sec, szBuf);
	else
		nGhi = fprintf(f, "%s", szBuf);
	if (nGhi > 0)
		s_lCo += nGhi;

	const DWORD dwNay = GetTickCount();
	++s_nTuFlush;
	if (s_nEpFlush || s_nTuFlush >= 50 || (DWORD)(dwNay - s_dwFlush) >= 500)
	{
		fflush(f);
		s_nTuFlush = 0;
		s_dwFlush = dwNay;
	}

	// Xoay tep khi qua 256 MB: PHAI dong truoc khi rename (Windows khoa tep dang mo).
	// Giu lai duoc mot doi (bot.log.1), mat ban cu nhat.
	if (s_lCo > 256L * 1024L * 1024L)
	{
		fclose(s_pTep);
		s_pTep = NULL;
		s_lCo = 0;
		s_nTuFlush = 0;
		remove("bot.log.1");
		rename("bot.log", "bot.log.1");
	}
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
#define PB_DT_CAP_TOI_THIEU  70    // (19/08 toi #3) chu game HA 80 -> 70 de test tron
                                   // vong may trang thai: nhieu bot du cap hon.
                                   // DINH CHINH theo du lieu (phan bien 19/08 toi):
                                   // levellink.txt cho 70-79 rut BAC 6, o do dong
                                   // kieu-1 nhe nhat la Num=8 (map 122/21) - van
                                   // KHONG lot tran cuon cu 5, nen phai nang tran
                                   // (ben duoi) thi nhanh loai 4 moi co viec lam.
// (19/08 toi #2) TRAN SO CUON nhan lam. Bac link 11 (cap >= 80) chi con Num=15
// (95,24% trong so) + Num=3 MAT CHI (chi roi boss xanh ~1/31.360) - deu ngo cut.
// (19/08 toi #3 - phan bien doc tasklink_findmaps.txt) bac 6 (cap 50-79) dong
// kieu-1 NHE NHAT la Num=8 (map 122 va 21); tran 5 cu lam nhanh loai 4 thanh MA
// CHET o MOI cap. Nang 8 de bot 70-79 nhan duoc loai 4 8-cuon -> test tron duong
// TOI_XAPHU/GODATAU/FARM_NV. Luoi an toan co san: farm 20 phut khong ra cuon la
// tu DOI NHIEM VU mien phi, khong co duong treo may.
#define PB_DT_CUON_TOI_DA    8
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

	// [BotTHP 28/08] Chu game: "goi bot ra nhieu con dung trong GOC KET" + "moi toa do
	// CHINH o Than Hanh Phu". Bot vao game dung vi tri luu/mau (luat 18/08 giu nguyen),
	// NHUNG neu o dang dung la O BI CHAN hoac NGOAI cua so luoi (goc ket - [BotLach]
	// lac khong thoat, [BotCuu]/T1 chi phu map bai) thi keo ve diem chuan Than Hanh Phu
	// cua map do (bang THON_TT_MP_ARRAY, shenxingfu.lua:18-48), moi con mot o rieng.
	// Map khong co trong bang (bai luyen cong...) thi giu nguyen - he cuu bai lo.
	{
		// bang toa do dung chung s_aThpDiem/pb_ThpDiem (nang len cap file 28/08 chieu)
		const int nNpcThp = Player[nIdx].m_nIndex;
		if (nNpcThp > 0 && nNpcThp < MAX_NPC)
		{
			const int nSubThp = Npc[nNpcThp].m_SubWorldIndex;
			if (nSubThp >= 0 && SubWorld[nSubThp].CoLuoiSrv())
			{
				int nTx = 0, nTy = 0;
				Npc[nNpcThp].GetMpsPos(&nTx, &nTy);
				if (SubWorld[nSubThp].CellObsSrv(nTx, nTy) != 0)
				{
					const int nMapThp = SubWorld[nSubThp].m_SubWorldID;
					int nThpX = 0, nThpY = 0;
					if (pb_ThpDiem(nMapThp, &nThpX, &nThpY))
					{
						// khuon rai 9x7 + loc o dat cua pha bao danh TK
						const int nLechT = s_botCount;
						const int nDxT = ((nLechT % 9) - 4) * 2;
						const int nDyT = (((nLechT / 9) % 7) - 3) * 2;
						int nVx = 0, nVy = 0;
						int nOkT = pb_ODat(nSubThp, nThpX + nDxT, nThpY + nDyT,
						                   nLechT, 10, &nVx, &nVy);
						if (!nOkT)
							nOkT = pb_ODat(nSubThp, nThpX, nThpY, nLechT, 16, &nVx, &nVy);
						if (nOkT)
						{
							pb_Log("[BotTHP] %s map=%d vao game o o(%d,%d) BI CHAN/ngoai luoi"
							       " -> ve diem Than Hanh Phu o(%d,%d)\n",
							       Player[nIdx].m_PlayerName, nMapThp, nTx / 32, nTy / 32,
							       nVx / 32, nVy / 32);
							Npc[nNpcThp].SetPos(nVx, nVy);
						}
					}
					else
						pb_Log("[BotTHP] %s map=%d o(%d,%d) BI CHAN nhung map khong co trong"
						       " bang Than Hanh Phu - giu nguyen cho he cuu bai\n",
						       Player[nIdx].m_PlayerName, nMapThp, nTx / 32, nTy / 32);
				}
			}
		}
	}

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
		b.nPhamViTick = 0;  b.nBienLogTick = 0;  b.nLachDem = 0;  b.nLachToi = 0;
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
		b.nTkKetPha = 0;     b.nTkKetTick = 0;  b.nTkKetX = 0;  b.nTkKetY = 0;
		b.nTkSanIdx = 0;
		b.nTrangBiLevel = 0;  b.nQhtTick = 0;  b.nHoc90 = 0;
		b.nDaTauTick = 0;  b.nDaTauNghi = 0;  b.nDaTauNut = 0;  b.nDaTauDaGhi = 0;
		b.nDaTauChon = 0;  b.nDaTauKe = 0;    b.nDaTauHan = 0;  b.nDaTauThu = 0;
		b.nBanSap = 0;     b.nBanSapXong = 0;  b.nBanSapNgoi = 0;  b.nNguaTick = 0;
		b.nSapChoXong = 0;
		// (21/08) TONG KIM - thieu doan nay thi khe tai dung ke thua "dang trong
		// Tong Kim" cua bot truoc va bot moi bi keo thang vao chien truong
		b.nTk = 0;         b.nTkPhe = 0;       b.nTkTick = 0;      b.nTkGoiThu = 0;
		b.nTkDichX = 0;    b.nTkDichY = 0;     b.nTkDichTick = 0;
		b.nTkMuaXong = 0;  b.nTkRaTick = 0;  b.nTkChoRa = 0;  b.nSubTruoc = -1;
		b.nTkDaBoc = 0;    b.nTkTgX = 0;    b.nTkTgY = 0;    b.nTkTgTick = 0;
		b.nSapDot = 0;     // khe tai dung: dot cham hang cua bot cu khong mang sang
		b.nDtPha = 0;  b.nDtToiNpcThu = 0;  b.nDtXaFuThu = 0;  b.nDtTraThu = 0;
		b.nDtTuiThu = 0;  b.nDtCoCu = 0;  b.nDtFarmMoc = 0;
		b.nDtNeoMap = 0;  b.nDtNeoX = 0;  b.nDtNeoY = 0;  b.nVeThanhHen = 0;
		b.nRaBaiThu = 0;  b.nRaBaiGoi = 0;  b.nThanhNhaMap = 0;
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
		// (21/08) bot dang trong Tong Kim: KHONG gan nhiem vu Da Tau moi. Gan
		// vao la nhanh DTB_TOI_NPC thay bot lech map roi ChangeWorld keo no ra
		// khoi chien truong sau 3 giay.
		// (phan bien) nhung VAN PHAI DEM no vao nDangChon truoc khi bo qua: bot
		// TK dang giu nhiem vu Da Tau van la mot suat: khong dem thi vong bu 60
		// giay tuong con thieu, bom them bot khac, va khi bot TK quay ve thi tong
		// so vuot tran.
		if (s_bots[i].nDaTauChon)
			nDangChon++;
		if (s_bots[i].nTk)
			continue;
		if (!s_bots[i].nDaTauChon)
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

// (19/08 toi #4) Don sach TRANG SUC SAP (nhan/lien/boi - detail 3/4/9) khoi hanh
// trang cua bot: dung khi thay dot hang moi VA khi dong sap tra bot ve bai.
// Chi dung pos_equiproom - do dang deo tren nguoi (pos_equip) khong bao gio dinh.
// Xoa ca nPrice > 0: pb_DonTui co y giu do co gia (hang dang bay), nen neu khong
// don o day thi bot bi bo chon sap se om 3-5 mon gia>0 ket vinh vien trong tui.
static void pb_DonTrangSucSap(int nIdx)
{
	for (int q = 1; q < MAX_PLAYER_ITEM; q++)
	{
		const int g = Player[nIdx].m_ItemList.m_Items[q].nIdx;
		if (g <= 0 || g >= MAX_ITEM)
			continue;
		if (Player[nIdx].m_ItemList.m_Items[q].nPlace != pos_equiproom)
			continue;
		if (Item[g].GetGenre() != item_equip)
			continue;
		const int nDe = Item[g].GetDetailType();
		if (nDe == 3 || nDe == 4 || nDe == 9)
			Player[nIdx].m_ItemList.RemoveItemIdx(g, Item[g].GetStackNum());
	}
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
		s_bots[i].nSapChoXong = 0;
		const int p = s_bots[i].nPlayerIdx;
		const int nn = (p > 0 && p < MAX_PLAYER) ? Player[p].m_nIndex : 0;
		if (nn > 0 && nn < MAX_NPC)
			Npc[nn].m_BaiTan = 0;
		// (19/08 toi #4) don hang ton truoc khi tra bot ve bai - khong don thi
		// pb_DonTui giu do nPrice > 0 va bot om hang ket tui vinh vien
		if (p > 0 && p < MAX_PLAYER && Player[p].m_nIndex > 0)
			pb_DonTrangSucSap(p);
	}
	static int aId[PB_MAX_BOTS];
	int nSong = 0;
	for (int i = 0; i < s_botCount; i++)
	{
		if (s_bots[i].nPlayerIdx <= 0)
			continue;
		if (s_bots[i].nDaTauChon)
			continue;                  // dang lam Da Tau thi khoi ban sap
		if (s_bots[i].nTk)
			continue;                  // (21/08) dang trong Tong Kim - chu game:
			                           // bot ban sap KHONG bi goi, va nguoc lai
			                           // bot dang danh tran khong bi bat ban sap
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
		s_bots[aId[i]].nSapChoXong = 0;   // [SapRai] phai xep cho ngoi rieng, ke ca dang o san thanh
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

// ===========================================================================
// (20/08 dem - chu game: "thu lam phan nguoi choi hay bot khong phai vat can
// co the dung chong len nhau xem con bi khong") CONG TAC 'NPC LA TUONG'.
// Dinh nghia that o KRegion.cpp (g_nPbNpcChan): moi diem doc m_pNpcRef phia
// server (GetBarrier / GetBarrierMin / FindPath_NpcObs) deu di qua cong tac.
// Mac dinh 0 = nguoi choi/bot/quai KHONG chan duong nhau (thi nghiem chong
// canh dam dong ep bot truot ra ngoai map); PB_SetNpcChan(1) = ve ban goc
// ngay lap tuc, khong can restart; PB_SetNpcChan(-1) = doc trang thai.
extern int g_nPbNpcChan;

int PB_SetNpcChan(int nBat)
{
	if (nBat >= 0)
	{
		g_nPbNpcChan = nBat ? 1 : 0;
		pb_Log("[BotChan] NPC la tuong: %s\n",
		       g_nPbNpcChan ? "BAT (ban goc - NPC chan duong nhau)"
		                    : "TAT (nguoi choi/bot/quai dung chong len nhau duoc)");
	}
	return g_nPbNpcChan;
}

int LuaPB_SetNpcChan(Lua_State* L)
{
	int nBat = (Lua_GetTopIndex(L) >= 1) ? (int)Lua_ValueToNumber(L, 1) : -1;
	Lua_PushNumber(L, PB_SetNpcChan(nBat));
	return 1;
}

// GOI BOT VE THANH THI / THON (chu game 19/08 toi #4): "viet 1 ham o lenh bai
// admin de keu toan bo BOT o cac map ve lai cac map thanh thi va thon, yeu cau
// chia bot ra deu cac map khong gom 1 cho". Lam thanh CHE DO bat/tat (bai hoc
// 18/08: lenh one-shot bo sot bot den muon / dang chet / dang doi map):
//   - BAT: moi bot (tru bot dang BAN SAP - da o thanh voi sap roi) ngung danh /
//     ngung may Da Tau (KHONG huy nhiem vu), ve map dich cua RIENG NO va di dao.
//   - Chia deu TUYET DOI theo chi so: map = nLech % 15 (1000 bot ~ 66-67 con/map);
//     trong map moi bot mot o rieng (luoi lech 9x7) + di dao ngau nhien quanh tam.
//   - Doi map SO LE trong cua so 60 giay (khuon pb_RaBai - 1000 con teleport cung
//     khung la nghen region sync).
//   - TAT: bot tu ve bai luyen bang duong pb_RaBai cu, Da Tau lam tiep nhiem vu.
// (21/08 - chu game: "viet lai ham goi bot ve thanh kieu nhu goi bot ve dung
// toa do moi thanh co san o than hanh phu (nen goi ve trung tam)") BANG NAY
// CHEP Y NGUYEN cach Than Hanh Phu (script\item\ib\shenxingfu.lua) dua nguoi
// choi ve thanh:
//   - 7 thanh lon: posthanh() -> Rev2Pos(map, id "Trung Tam") = toa do DIEM HOI
//     SINH trung tam (settings\RevivePos.ini, MPS/32). Lam An khong co muc
//     Trung Tam -> lay "Lam An Bac" (rev 69) la diem sat tam THON_TT nhat (9 o).
//   - thon/tran: chonthon() -> THON_TT_MP_ARRAY (tam thon). (21/08 - chu game:
//     "bo cac thon khac di chi de Ba Lang Huyen thoi") -> chi giu Ba Lang
//     Huyen (53); Giang Tan/Long Mon/Nam Nhac va 5 thon bang cu deu BO.
// nRevId = dung id phu dung de "luu ruong" tai cho dap (Thanh Do Trung Tam la
// rev 5 cua section [11] - id la KHOA TRONG SECTION tung map, khong toan cuc).
static int s_nPbVeThanh = 0;

struct PB_VeThanh { int nMap, nOX, nOY, nRevId; };
static const PB_VeThanh s_veThanh[] =
{
	{   1, 1591, 3200,  5 },    // Phuong Tuong Phu - Trung Tam (rev 5)
	{  11, 3144, 5073,  5 },    // Thanh Do Phu     - Trung Tam (rev 5)
	{ 162, 1579, 3135, 63 },    // Dai Ly Phu       - Trung Tam (rev 63)
	{  37, 1726, 3100, 23 },    // Bien Kinh Phu    - Trung Tam (rev 23)
	{  78, 1577, 3238, 29 },    // Tuong Duong Phu  - Trung Tam (rev 29)
	{  80, 1758, 3030, 34 },    // Duong Chau Phu   - Trung Tam (rev 34)
	{ 176, 1569, 2938, 69 },    // Lam An Phu       - Bac (rev 69, sat tam nhat)
	{  53, 1622, 3189, 19 },    // Ba Lang Huyen    - THON_TT (rev 19) - thon DUY NHAT giu lai
};
#define PB_SO_VETHANH  (int)(sizeof(s_veThanh) / sizeof(s_veThanh[0]))

int PB_SetVeThanh(int nBat)
{
	if (nBat >= 0)
	{
		s_nPbVeThanh = nBat ? 1 : 0;
		// hen gio so le tinh lai tu dau moi lan BAT (nVeThanhHen per-bot)
		for (int i = 0; i < s_botCount; i++)
			s_bots[i].nVeThanhHen = 0;
		pb_Log("[BotVeThanh] che do ve thanh/thon: %s (%d map dich, chia deu theo"
		       " chi so bot)\n", s_nPbVeThanh ? "BAT" : "TAT", PB_SO_VETHANH);
	}
	return s_nPbVeThanh;
}

int LuaPB_SetVeThanh(Lua_State* L)
{
	int nBat = (Lua_GetTopIndex(L) >= 1) ? (int)Lua_ValueToNumber(L, 1) : -1;
	Lua_PushNumber(L, PB_SetVeThanh(nBat));
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

// ============================================================================
// (20/08) CAT CHANG CHAY THEO LUOI A* - chan "bot chay xuyen map".
//
// Moi lenh di deu la SendCommand(do_run, x, y) roi ENGINE tu dan than bot di.
// Engine doc vat can bang KRegion::GetBarrierMin, ma du lieu do coi VUNG RONG
// NGOAI MAP la "di duoc" (goc benh 19/08) - nen mot dich HOP LE tren luoi A*
// van co the co DUONG DI cat qua vung rong: bot chay tat "ra ngoai map" mot
// doan roi vao lai. Waypoint cua A* la BLOCK to toi 16x32 o (512x1024 MPS)
// nen chang chay co the rat dai, du cho di lac han sang ben kia bo tuong.
//
// Cach chan: lay mau doc DOAN THANG bot se chay, moi 16 MPS (nua o); gap o bi
// luoi chan thi KEO DICH VE diem tot cuoi cung truoc do. Nhu vay engine khong
// bao gio nhan mot lenh doi hoi bang qua o ma luoi A* coi la vat can.
//
// Tra 1 = doan di duoc (dich co the da bi keo ve gan hon);
//     0 = ngay buoc dau tien da bi chan -> noi goi NEN BO QUA nhip nay
//         (dung phat lenh moi) va de bo do ket cua PB_WalkTo tu tim duong lai.
// Luoi chua nap (CellObsSrv tra -1) thi khong cat gi ca - giu nguyen hanh vi cu.
// ============================================================================
static int pb_CatDoan(int nSub, int bx, int by, int* pnX, int* pnY)
{
	if (nSub < 0 || nSub >= MAX_SUBWORLD || bx <= 0 || by <= 0)
		return 1;
	// BAY 2 (phan bien 20/08): PHAI hoi TRANG THAI LUOI, khong hoi o cua bot.
	// CellObsSrv tra -1 cho BA ca gop lam mot - luoi chua nap / MPS <= 0 /
	// NGOAI CUA SO LUOI. Neu lay "-1 tai cho bot" lam dau hieu "chua co luoi"
	// thi dung con DA LOT RA NGOAI MAP lai duoc mien kiem tra - tuc lop chan
	// nay tu tat dung luc can nhat.
	if (!SubWorld[nSub].CoLuoiSrv())
		return 1;                    // ban do nay khong co luoi - giu hanh vi cu
	int dx = *pnX - bx;
	int dy = *pnY - by;
	int ax = (dx >= 0) ? dx : -dx;
	int ay = (dy >= 0) ? dy : -dy;
	// (phan bien 20/08) CHANG QUA DAI: kep so BUOC o 512 se lam buoc lay mau gian
	// ra hon MOT O, ham nhay qua nguyen ca mot vach mong 1 o (vien vung rong,
	// tuong thanh - rat pho bien) va mat tac dung dung o nhung chang de xuyen map
	// NHAT. Thay vi gian buoc, RUT NGAN DICH ve 8192 MPS tren cung tia: bot di het
	// doan da kiem, nhip sau tinh tiep tu cho moi. Buoc mau luon giu 16 MPS.
	{
		const int nDaiMax = 8192;                   // 256 o
		const int nDai = (ax > ay) ? ax : ay;
		if (nDai > nDaiMax)
		{
			dx = (int)((__int64)dx * nDaiMax / nDai);
			dy = (int)((__int64)dy * nDaiMax / nDai);
			*pnX = bx + dx;
			*pnY = by + dy;
			ax = (dx >= 0) ? dx : -dx;
			ay = (dy >= 0) ? dy : -dy;
		}
	}
	int nBuoc = ((ax > ay) ? ax : ay) / 16;
	// BAY 4 (phan bien 20/08): CHI cat chang DAI. Chang ngan khong the "chay
	// xuyen map", ma trong hanh lang hep / goc vach (map 79, 124...) thi block
	// A* ke thuong nam CHEO qua goc tuong - diem mau dau tien da bi chan du A*
	// co duong hop le vong qua. Cat ca chang ngan = bot dung tuong te liet.
	// 16 buoc x 16 MPS = 256 MPS = 8 o.
	if (nBuoc <= 16)
		return 1;
	// Luoi CO nap ma o cua bot lai ngoai cua so = bot DA lot ra ngoai ban do
	// (Mps2Map chia am / ServeJump troi offset / nap lai vao void). Bao chan
	// ngay, tuyet doi khong cho phat lenh chay tiep ra xa.
	if (SubWorld[nSub].CellObsSrv(bx, by) < 0)
		return 0;
	// BAY 1 (phan bien 20/08): buoc lay mau 16 MPS = NUA O nen diem mau dau
	// tien thuong VAN nam trong chinh o cua bot. Bot dung tren o bi luoi son
	// (o goc = vat can tu 18/08, hoac nap lai dung cho cu) se khong bao gio
	// roi di duoc - te liet vinh vien. Bo qua moi diem mau con trong o goc.
	const int nOx0 = bx / 32, nOy0 = by / 32;
	int nGoodX = bx, nGoodY = by, nCo = 0;
	for (int i = 1; i <= nBuoc; i++)
	{
		const int px = bx + (int)((__int64)dx * i / nBuoc);
		const int py = by + (int)((__int64)dy * i / nBuoc);
		if (px / 32 == nOx0 && py / 32 == nOy0)
		{
			nGoodX = px;  nGoodY = py;  nCo = i;
			continue;                // van trong o cua chinh bot - khong xet
		}
		if (SubWorld[nSub].CellObsSrv(px, py) != 0)
			break;                   // dung NGAY TRUOC o bi chan / ngoai luoi
		nGoodX = px;  nGoodY = py;  nCo = i;
	}
	if (nCo >= nBuoc)
		return 1;                    // ca chang thong - giu nguyen dich
	// khong ra noi khoi o xuat phat = coi nhu bi chan (noi goi tu xu ly)
	if (nCo <= 0 || (nGoodX / 32 == nOx0 && nGoodY / 32 == nOy0))
		return 0;
	// (phan bien 20/08) KEP VE TAM O. Diem cat duoc tinh tu VI TRI BOT nen no
	// truot theo bot moi nhip - tren chang bi cat, cong phat lenh o PB_WalkTo se
	// ban lai lien tuc, dung cai bao goi tin ma waypoint dong bang vua diet. Kep
	// ve tam o thi gia tri chi doi khi bot di qua MOT O (32 MPS), tuc nhieu nhat
	// mot goi moi o. O cua diem cat da duoc xac nhan di duoc nen tam o cung vay.
	*pnX = (nGoodX / 32) * 32 + 16;
	*pnY = (nGoodY / 32) * 32 + 16;
	return 1;
}

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
			st.wpX = 0;
			st.lastSendX = st.lastSendY = 0;
			return -1;
		}
		// path[0] LUON la block bot DANG DUNG (FindPath_Block lan nguoc cameFrom tu dich ve
		// xuat phat). Block to toi 16x32 o nen TAM cua no co the cach bot ca tram MPS -
		// nham vao do la buoc GIAT LUI dau tien, dung trieu chung "tien roi giat lui" ma
		// ban tham khao canh bao (KBotCombat.cpp:552-556). Bo qua no ngay tu dau.
		st.nKieuDuong = nRet;    // 1 = duong tron ven, 2 = duong CUT (dich khong toi duoc)
		st.idx = 1;
		st.wpX = 0;   // lo trinh moi -> chot lai waypoint dong bang
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
		st.wpX = 0;   // sang block khac -> chot waypoint moi
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
			st.wpX = 0;
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
		// (20/08 - chu game: "nhin bot di chuyen toi thi bi khung lai 1 nhip roi
		// moi di tiep") WAYPOINT DONG BANG.
		//
		// Truoc day dong nay tinh BlockNearestMps TU VI TRI BOT o MOI NHIP, nen
		// diem den TRUOT theo bot: do tu log 20/08, bot chay 18-20 MPS/khung
		// (~340 MPS/giay), ma cong phat lenh o duoi chi can lech > 48 MPS la ban
		// lai -> khoang 7 goi do_run moi giay cho MOI con dang chay. Client nhan
		// s2c_npcwalk la DUNG LAI DUONG DI cua NPC do tu dau, nen mat thay bot
		// khung mot nhip roi moi chay tiep; dung giua dam 130 bot thi client phai
		// nuot ~900 goi/giay - FPS van cao ma di chuyen cu giat.
		//
		// Nay chot MOT LAN cho moi block roi giu nguyen (xoa o B1 va o cho tien
		// con tro block). Bot khong can toi dung diem do: vong B2 o tren da cho
		// tien con tro NGAY khi bot cham mep block, nen moi block chi con ~1 goi.
		if (st.wpX <= 0)
		{
			int wx = 0, wy = 0;
			if (SubWorld[nSubIdx].BlockNearestMps(st.path[st.idx], bx, by, wx, wy)
			 && wx > 0 && wy > 0)
			{
				st.wpX = wx;
				st.wpY = wy;
			}
		}
		if (st.wpX > 0)
		{
			tx = st.wpX; ty = st.wpY;
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
	// (20/08) CAT CHANG theo luoi truoc khi phat: block A* to toi 512x1024 MPS nen
	// duong thang tu bot toi diem kep trong block ke co the cat qua vung rong ma
	// ENGINE tuong la di duoc -> chinh la canh "bot chay xuyen map". Xem pb_CatDoan.
	if (!pb_CatDoan(nSubIdx, bx, by, &tx, &ty))
	{
		// (phan bien 20/08 vong 2) IM LANG, KHONG bao thua va KHONG dung dong ho
		// rieng. Moi nguong rieng nho hon 3 giay deu tri vi TRUOC hai bo do ket
		// that su chua benh: B5a (3 giay khong nhuc nhich -> tim lai duong CO tinh
		// NPC lam vat can + danh thuc bot bi m_ProcessAI = 0 nuot lenh) va B5b
		// (3 lan tim lai duong moi bo cuoc). B5a do bang VI TRI THAT cua bot nen
		// no VAN chay dung khi ta khong phat lenh - day chinh la dong ho can co.
		// Bao thua som con lam ba nhanh Da Tau TELEPORT bot ngay lan dau (chung
		// SetPos khi PB_WalkTo tra -1) va lam [BotCuu] keo bot ve diem neo - hai
		// canh "bot bay" + "bot gom mot cho" ma dot nay dang di diet.
		return 0;
	}
	// (phan bien 20/08 - PHAI BO NGUONG 48 MPS) Nguong do sinh ra khi diem den con
	// TRUOT theo bot. Nay waypoint da dong bang nen diem den chi doi khi DOI BLOCK -
	// va hai block A* KE NHAU dung chung mot canh, moi rect lai thut vao 16 MPS,
	// nen waypoint moi thuong chi cach waypoint cu 32-64 MPS, tuc DUNG NGAY TREN
	// nguong 48. Khi no roi duoi 48 thi lenh DUY NHAT cua chang moi bi chinh cong
	// nay nuot: bot chay het chang cu, engine DoStand, dung mot nhip, nhip sau
	// bStopped moi mo cong. Tuc la ta se TU TAO ra dung cai "khung mot nhip" dang
	// di chua, o MOI ranh block. So sanh bang "!=" gio khong con nguy co spam.
	const bool bStopped = (Npc[nNpcIdx].m_Doing == do_stand);
	if (tx != st.lastSendX || ty != st.lastSendY)
	{
		Npc[nNpcIdx].SendCommand(do_run, tx, ty);
		st.lastSendX  = tx;
		st.lastSendY  = ty;
		st.uSendTick  = now;
	}
	else if (bStopped && now - st.uSendTick >= (unsigned int)(GAME_FPS / 2))
	{
		// (phan bien 20/08) Bot dung yen tai diem da toi (hoac lenh bi nuot) thi
		// bStopped dung MOI NHIP - de tran se thanh 18 goi/giay/bot cho toi khi
		// B5a tim duong lai sau 3 giay. Gion con 2 lan/giay: van danh thuc duoc
		// bot bi ket ma khong bao goi tin.
		Npc[nNpcIdx].SendCommand(do_run, tx, ty);
		st.uSendTick = now;
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

	// [BotNoi 28/08] bot duong NOI CONG: co y tay khong (nhu nhanh duong quyen duoi) -
	// vu khi cu (ke ca cua nhan vat mau) da duoc thao o tren.
	if (pb_BotNoiThat(nIdx))   // [BotNoi-PHAI] TV/DM khong co chieu noi -> van nhan vu khi
	{
		pb_Log("[BotNoi] %s phai %s: duong NOI CONG, khong nhan vu khi\n",
		       Player[nIdx].m_PlayerName, s_facNpc[nFaction].szTen);
		return;
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

// 9 mon Kim Phong, id AddGoldItem = 177..185 (da kiem chung tren log that: cac mon
// mac vao deu ten "Kim Phong ..."; NPC tan thu hotrotanthu.lua:86 cung phat bo nay
// bang for i=177,185. Chu game noi "176-184" la lech 1 theo cach dem 0).
//
// (19/08 toi #3 - DINH CHINH sau dot soat doi khang) Bang du lieu THAT ma
// AddGoldItem doc la settings\item\GoldItem.txt (TABFILE_GOLDITEM_O, xem
// KBPT_Equipment_Gold::Init), KHONG phai goldequip.txt; id = DONG - 2 vi
// LoadRecord cong nRow += 2 (GoldItem.txt dong 179 = "Kim phong thanh duong khoi",
// cot 53 = 177). KItem.cpp:961 gan pCA->nGoldId = sData.m_nId = COT 53 = 177..185.
// So 36 la COT 54 (m_nSet - ma BO do), KHONG phai nGoldId => phep thu
// GetGoldId()==36 cu KHONG BAO GIO DUNG: bot mac lai ca bo 9 mon MOI LAN LEN CAP
// (26.395 dong log trong 35 phut, mon cu roi vao tui roi bi pb_DonTui xoa).
// Phep thu DUNG: GetGoldId() == chinh id da dung de tao mon do.
static const int s_nKpId[9]  = { 177, 178, 179, 180, 181, 182, 183, 184, 185 };
static const int s_nKpCho[9] = { itempart_head, itempart_body, itempart_amulet,
                                 itempart_ring1, itempart_belt, itempart_cuff,
                                 itempart_pendant, itempart_foot, itempart_ring2 };

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

	// ---- 1b. (20/08 - chu game: "bot cap 80 chua thay nhan duoc cac skill 90",
	// roi chot "CHI duoc nhan ky nang 90 thoi") hoc bo ky nang 90 qua ham lua
	// bot_hoc90(nCurFac) trong hocvocong.lua - ham RIENG cho bot, copy dung vong
	// SKILL90_ARRAY cua show_kynang90 nhung KHONG kem SKILL120AR + chieu 210 nhu
	// ham NPC goc. nCurFac 1-based = m_nCurFaction + 1 (than ham index
	// SKILL90_ARRAY[nCurFac] truc tiep). Goi lai sau nap-bot-cu vo hai:
	// KSkillList::Add chi NANG cap khi cao hon, khong bao gio ha
	// (KSkillList.cpp:434-447).
	if (nLevel >= 80 && !b.nHoc90 && b.nFaction >= 0)
	{
		b.nHoc90 = 1;
		Player[nIdx].ExecuteScript((char*)"\\script\\global\\hocvocong.lua",
		                           (char*)"bot_hoc90",
		                           (int)Player[nIdx].m_cFaction.m_nCurFaction + 1,
		                           false);
		pb_Log("[BotSkill90] %s cap %d hoc bo ky nang 90 - CHI 90 (phai %d)\n",
		       Player[nIdx].m_PlayerName, nLevel,
		       (int)Player[nIdx].m_cFaction.m_nCurFaction + 1);
		b.nAtkSkill = 0;   // chon lai chieu danh - co the co chieu 90 manh hon
	}

	// ---- 2. bo Kim Phong: mon nao chua mac ma du dieu kien thi mac ----
	for (int k = 0; k < 9; k++)
	{
		const int nDangMac = Player[nIdx].m_ItemList.GetEquipment(s_nKpCho[k]);
		if (nDangMac > 0 && Item[nDangMac].GetGoldId() == s_nKpId[k])
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

	// ---- 3b. [BotNoi 28/08] bot duong NOI CONG khong cam vu khi: bot cu con cam
	// (di san truoc khi chia duong, hoac blob mau) thi thao huy mot lan tai day
	// (ham nay chay ca luc login vi gate nTrangBiLevel=0 != cap) roi chon lai chieu
	// -> nWant=-1, bThienNoi (cung nhom dwID le) tu uu tien chieu phep. ----
	if (pb_BotNoiThat(nIdx))   // [BotNoi-PHAI] TV/DM (khong chieu noi) giu vu khi, khong thao
	{
		const int nWn = Player[nIdx].m_ItemList.GetEquipment(itempart_weapon);
		if (nWn > 0)
		{
			Player[nIdx].m_ItemList.RemoveItemIdx(nWn, Item[nWn].GetStackNum());
			b.nAtkSkill = 0;
			pb_Log("[BotNoi] %s cap %d: thao vu khi (duong NOI CONG danh tay khong)\n",
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

// (19/08 dem - chu game: "goi bot ve thanh chia deu... phai tu dong luu ruong o
// map do luon de sau phu ve thi se ve dung thanh da tung bi goi ve")
// THANH NHA DONG: mac dinh s_dtNpc[nLech % 10]; che do VE THANH dap xuong thanh
// nao (co Xa Phu) thi ghi thanh do vao b.nThanhNhaMap - moi lan "phu ve thanh"
// sau nay dung dung thanh ay.
static const PB_DtNpc& pb_ThanhNha(const PB_Bot& b, int nLech)
{
	if (b.nThanhNhaMap > 0)
		for (int i = 0; i < 10; i++)
			if (s_dtNpc[i].nMap == b.nThanhNhaMap)
				return s_dtNpc[i];
	return s_dtNpc[((unsigned)nLech) % 10u];
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

// (19/08 toi #3 - chu game: "bay ban van con bi dinh vao goc ket") O NGOI SAP "TOT".
// pb_ODat chi hoi "o nay dung duoc khong" - mot o tui 1-2 o lot giua khe tuong /
// goc nha van "dung duoc" nen sap chui vao goc ket. O TOT phai du 3 dieu:
//   1. dung duoc (pb_ODuoc: engine trong + luoi A* khong chan) VA khong dap len
//      nguoi/bot khac dang ngoi do (GetBarrierMin bCheckNpc=TRUE);
//   2. THOANG: >= 5/8 o lan can di duoc theo DIA HINH (o tui/goc chi co 1-3 loi);
//      dem bang pb_ODuoc (khong tinh nguoi) de bot ngoi canh nhau khong loai oan nhau;
//   3. NOI DUOC toi chan NPC Da Tau: dung BAN DO LOANG (BFS) tinh san quanh NPC -
//      vuon rao / ban cong kin du thoang may cung khong ai vao mua duoc.
// CO Y KHONG dung FindPathServer o day: khi KHONG co duong (dung ca o tui can bat)
// A* loang het ca thanh phan lien thong cua map thanh (map 1: 777k o) - 200 bot dat
// sap cung mot khung la treo server. BFS cua so co TRAN CUNG PB_SAP_BFS_O o, re va du.

#define PB_SAP_BFS_R   13                       // ban kinh cua so loang (o) - phu het
                                                // vong dat sap 3..12 + 1 o du phong
#define PB_SAP_BFS_W   (PB_SAP_BFS_R * 2 + 1)   // canh cua so (27)
#define PB_SAP_BFS_O   (PB_SAP_BFS_W * PB_SAP_BFS_W)   // 729 o - tran cung

// Loang tu chan NPC (o luoi nTamX,nTamY) ra moi o DI DUOC theo dia hinh (pb_ODuoc,
// khong tinh nguoi - bot ngoi truoc khong duoc phep "cat duong" bot sau) trong cua
// so PB_SAP_BFS_W x PB_SAP_BFS_W. aReach[idx] = 1 neu o toi duoc tu NPC.
// (19/08 toi #4) Tra ve SO O loang duoc - dung lam thuoc do "diem nay co nam trong
// vung lien thong tu te khong" cho viec kiem diem neo bai (pb_LayNeo).
static int pb_SapLoang(int nSubIdx, int nTamX, int nTamY, unsigned char* aReach)
{
	memset(aReach, 0, PB_SAP_BFS_O);
	static short aQx2[PB_SAP_BFS_O], aQy2[PB_SAP_BFS_O];   // hang doi BFS (mot luong)
	int nDau = 0, nDuoi = 0;
	// goc cua so = (nTamX - R, nTamY - R); chi so idx = (y - gocY) * W + (x - gocX)
	aReach[PB_SAP_BFS_R * PB_SAP_BFS_W + PB_SAP_BFS_R] = 1;
	aQx2[nDuoi] = (short)nTamX;  aQy2[nDuoi] = (short)nTamY;  nDuoi++;
	static const int aBx[4] = { 1, -1, 0, 0 };
	static const int aBy[4] = { 0, 0, 1, -1 };
	while (nDau < nDuoi)
	{
		const int x = aQx2[nDau], y = aQy2[nDau];
		nDau++;
		for (int h = 0; h < 4; h++)
		{
			const int x2 = x + aBx[h], y2 = y + aBy[h];
			const int lx = x2 - (nTamX - PB_SAP_BFS_R);
			const int ly = y2 - (nTamY - PB_SAP_BFS_R);
			if (lx < 0 || lx >= PB_SAP_BFS_W || ly < 0 || ly >= PB_SAP_BFS_W)
				continue;
			if (aReach[ly * PB_SAP_BFS_W + lx])
				continue;
			if (!pb_ODuoc(nSubIdx, x2 * 32, y2 * 32))
				continue;
			aReach[ly * PB_SAP_BFS_W + lx] = 1;
			aQx2[nDuoi] = (short)x2;  aQy2[nDuoi] = (short)y2;  nDuoi++;
		}
	}
	// Chan NPC co the chinh no khong qua pb_ODuoc (NPC hay dung tren o dac biet):
	// van loang duoc vi o goc da danh dau 1 truoc khi kiem - 4 o lan can cua no
	// duoc thu binh thuong o vong dau.
	return nDuoi;
}

static int pb_OSapTot(int nSubIdx, int nCellX, int nCellY, int nTamX, int nTamY,
                      const unsigned char* aReach)
{
	const int lx = nCellX - (nTamX - PB_SAP_BFS_R);
	const int ly = nCellY - (nTamY - PB_SAP_BFS_R);
	if (lx < 0 || lx >= PB_SAP_BFS_W || ly < 0 || ly >= PB_SAP_BFS_W)
		return 0;
	if (!aReach[ly * PB_SAP_BFS_W + lx])
		return 0;              // khong noi duoc toi NPC (o tui / vuon rao kin)
	const int nMpsX = nCellX * 32, nMpsY = nCellY * 32;
	if (!pb_ODuoc(nSubIdx, nMpsX, nMpsY))
		return 0;
	int nR = -1, nMX = 0, nMY = 0, nOX = 0, nOY = 0;
	SubWorld[nSubIdx].Mps2Map(nMpsX, nMpsY, &nR, &nMX, &nMY, &nOX, &nOY);
	if (nR < 0)
		return 0;
	if (SubWorld[nSubIdx].m_Region[nR].GetBarrierMin(nMX, nMY, nOX, nOY, TRUE)
	    != Obstacle_NULL)
		return 0;              // co nguoi/bot khac dang chiem o nay
	// [SapRai 28/08] g_nPbNpcChan mac dinh TAT (KRegion.cpp:29 - du an co y cho
	// nguoi/bot dung chong nhau) nen nhanh GetBarrierMin(bCheckNpc=TRUE) o tren
	// thuong KHONG bat duoc "o da co sap" -> ca dan chong mot o. Tu quet danh
	// sach bot sap: con nao (ke ca vua duoc dat trong CUNG khung) da dung/ngoi
	// dung o nay thi o XAU. O(so bot) moi o ung vien, chi chay luc xep cho.
	for (int q3 = 0; q3 < s_botCount; q3++)
	{
		if (!s_bots[q3].nBanSap || s_bots[q3].nPlayerIdx <= 0)
			continue;
		const int nn3 = Player[s_bots[q3].nPlayerIdx].m_nIndex;
		if (nn3 <= 0 || nn3 >= MAX_NPC)
			continue;
		if (Npc[nn3].m_SubWorldIndex != nSubIdx)
			continue;
		int qx3 = 0, qy3 = 0;
		Npc[nn3].GetMpsPos(&qx3, &qy3);
		if (qx3 / 32 == nCellX && qy3 / 32 == nCellY)
			return 0;
	}
	static const int aTx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	static const int aTy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
	int nThoang = 0;
	for (int h = 0; h < 8; h++)
		if (pb_ODuoc(nSubIdx, nMpsX + aTx[h] * 32, nMpsY + aTy[h] * 32))
			nThoang++;
	if (nThoang < 5)
		return 0;              // o tui / khe tuong / goc nha = "goc ket"
	return 1;
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
#define PB_NPC_TKT  2      // (21/08) NPC bao danh phe Tong (song_signup)
#define PB_NPC_TKK  3      // (21/08) NPC bao danh phe Kim  (jin_signup)
#define PB_NPC_TKQ  4      // (21/08) Quan Y trong chien truong (quany)
static int s_npcNho[5][MAX_SUBWORLD];

static int pb_TimNpcNho(int nSub, int nLoaiNpc, const char* szScript, int bx, int by)
{
	if (nSub < 0 || nSub >= MAX_SUBWORLD || nLoaiNpc < 0 || nLoaiNpc > 4)
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

// LEN NGUA khi phai di duong XA (chu game 21/08: "neu di chuyen duong xa thi
// phai len ngua de di chuyen"). Khac pb_CuoiNguaTrongThanh o cho KHONG kep dieu
// kien "dang o thanh thi": trong Tong Kim hai doanh trai cach nhau gan 500 o,
// di bo la het tran chua toi noi. Van gion 5 giay/lan va van doi bot co ngua
// trong o trang bi - khong co thi thoi, khong tu che ra ngua.
static void pb_LenNguaDuongXa(int nIdx, int nNpcIdx, PB_Bot& b, unsigned int now)
{
	if (now - b.nNguaTick < (unsigned int)(GAME_FPS * 5))
		return;
	b.nNguaTick = now;
	// (21/08 - chu game: "phai giong nguoi choi 100%") Chia duong van chua du:
	// cung duong + cung toc do + cung thoi diem thi van la doan tau, chi la nhieu
	// doan tau thay vi mot. Cho ~1/4 bot di BO - nguoi that cung co nguoi khong co
	// ngua hoac quen len. Chot theo CHI SO BOT nen moi con giu nguyen thoi quen ca
	// tran, khong nhap nhay len xuong giua duong (nhin ra ngay).
	//
	// PHAI dat SAU dong gan nNguaTick o tren: dat truoc thi nhanh nay khong cap
	// nhat moc, thanh ra bi goi lai moi khung hinh.
	if ((((int)(&b - s_bots)) & 3) == 0)
	{
		// (21/08 phan bien) PHAI CUONG CHE XUONG: bot vao tran tu thanh thi thuong
		// dang cuoi ngua san (pb_CuoiNguaTrongThanh len ngua cho moi bot o thanh),
		// chi "khong len" thi no giu ngua ca tran. CheckRideHorse(TRUE) = XUONG
		// (KPlayer.cpp:10170: nFlagRide -> m_bRideHorse = FALSE).
		if (Npc[nNpcIdx].m_bRideHorse)
			Player[nIdx].CheckRideHorse(TRUE);
		return;
	}
	if (Npc[nNpcIdx].m_bRideHorse)
		return;
	if (Player[nIdx].m_ItemList.GetEquipment(itempart_horse) <= 0)
		return;
	Player[nIdx].CheckRideHorse(FALSE);        // FALSE = LEN ngua (KPlayer.cpp:9926)
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
	// (28/08 chu game) "phu ve lay TOA DO TRUNG TAM THANH (Than Hanh Phu) roi DI
	// CHUYEN toi NPC" - nhu nguoi choi that dung phu. Dap quanh TRUNG TAM lech
	// 6..15 o theo chi so bot cho khoi don cuc; cac pha co san tu dat toi NPC
	// (DTB_TOI_NPC cuoi ngua, han 600 nhip - xa nhat Dai Ly ~119 o van du; duong
	// Xa Phu co ngan sach 300 giay). Map khong co trong bang thi giu diem cu.
	int nTtX = nha.nX, nTtY = nha.nY;
	pb_ThpDiem(nha.nMap, &nTtX, &nTtY);
	if (nSubT >= 0)
	{
		const int nXaO = 6 + (nLech % 10);
		static const int aGx[4] = { 1, -1, 0, 0 };
		static const int aGy[4] = { 0, 0, 1, -1 };
		const int g2 = nLech & 3;
		if (pb_ODat(nSubT, nTtX + aGx[g2] * nXaO, nTtY + aGy[g2] * nXaO,
		            nLech, 8, &nVx, &nVy))
			nOk = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nVx, nVy) == 1);
		if (!nOk && pb_ODat(nSubT, nTtX, nTtY, nLech, 12, &nVx, &nVy))
			nOk = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nVx, nVy) == 1);
	}
	if (!nOk)
		nOk = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nTtX * 32, nTtY * 32) == 1);
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
		// (19/08 toi #4) neo xich cho map nhiem vu: diem dat chan dau tien sau khi
		// Xa Phu tha (pb_NeoBai doc - chan duoi quai ra "vung trong gia" o day)
		if (b.nDtNeoMap != nMapNv)
		{
			int nNvX = 0, nNvY = 0;
			Npc[nNpcIdx].GetMpsPos(&nNvX, &nNvY);
			b.nDtNeoMap = nMapNv;
			b.nDtNeoX   = nNvX;
			b.nDtNeoY   = nNvY;
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
// 3-5 mon TRANG SUC XANH - do CO THUOC TINH PHU, dung duong quai rot do
// (nhan 0/3/0, lien 0/4/0, boi 0/9/0) - GIA = gia goc x2 nhung SAN 500,
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

// (19/08 toi #4 - chu game: "viet them ham cap nhap ramdon item trang suc cho bot
// bay ban") CHAM HANG SAP: don sach trang suc dang nam trong hanh trang (ca ton kho
// gia-0 sau restart lan hang bay chua ban het) roi bay mot dot hang MOI ngau nhien.
// Duong sinh DO XANH giu NGUYEN khuon da chay that (log 17:50 19/08, 0 dong "bo mon
// khong phai do xanh"); cai moi duy nhat la nDot: g_Random cua engine.lib DONG BANG
// THEO GIAY nen tron nDot vao cac phep gieo de moi dot lam moi ra BO HANG KHAC that
// su (loai, cap, so mon, cap opt) thay vi lap lai dung mot ro hang cu.
// Tra ve so mon da bay duoc len sap.
static int pb_ChamHangSap(int nIdx, int nLech, int nDot, unsigned int nowT)
{
	// Don trang suc cu (ca nPrice > 0 - dot moi la thay ca ro; lan nPrice = 0 -
	// ton kho mat gia sau restart) roi moi bay dot moi. Dung chung pb_DonTrangSucSap
	// voi duong dong sap trong PB_SetBanSap.
	pb_DonTrangSucSap(nIdx);

	int nBay = 0;
	// (19/08 toi #4 - chu game: "item moi bot bay ban chi co vai mon, muon bay
	// nhieu hon de nguoi choi chon mua") 3-5 -> 8-12 mon. (phan bien dot 2 do
	// that:) cua so sap client do theo O TUI nguoi ban (ItemBox 6x10 = 60 o,
	// UiPlayerShop.ini), goi VIEW_ITEM_SYNC tran 60 mon - 8..12 an toan tuyet doi;
	// tui 60 o chi ton ~20 o cho 12 mon trang suc (nhan 1x1, lien 2x1, boi 1x2).
	const int nMuon = 8 + ((nLech + nDot) % 5);
	for (int t2 = 0; t2 < nMuon; t2++)
	{
		static const int aDet[3] = { 3, 4, 9 };   // nhan / lien / boi
		const int nDet = aDet[(nLech + t2 + nDot) % 3];
		const int nLv  = 1 + (((int)g_Random(10) + nLech * 7 + t2 * 3 + nDot * 13) % 10);
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
		const int nCapPhu = 1 + (((int)g_Random(6) + nLech + t2 + nDot) % 6);   // 1..6
		for (int o2 = 0; o2 < 6; o2++)
		{
			// (phan bien dot 2) g_Random dong bang theo giay: khong tron o2/t2 thi
			// ca vong ra CUNG ket qua -> so opt bimodal {1,6}; tron chi so giu
			// xac suat ~1/(2+k) moi buoc nhung bien thien that giua mon/vong
			if (o2 && ((int)g_Random(97) + o2 * 31 + t2 * 13 + nLech) %
			          (2 + (nMay > 10 ? 10 : nMay)) == 0)
				break;
			nMagic[o2] = nCapPhu;
		}
		if (nMagic[0] == 0)
			nMagic[0] = nCapPhu;              // khong co opt nao = do TRANG, bi vut
		// ItemSet.Add KHONG co tham so nPoint => luon nPoint = 0 => Gen_MagicAttrib
		// => do XANH (KItemGenerator nhanh nPoint == 0). AddItemSet2 co nPoint,
		// truyen khac 0 la SetPoint => IsPurple => do TIM (loi ban truoc).
		const int nNew = ItemSet.Add(0, 0, (nLech + t2 + nDot) % 5, nLv, nMay, nDet, 0,
		                             nMagic, g_SubWorldSet.GetGameVersion(), 0,
		                             KItemSet::genXOpt(nMay));
		if (nNew <= 0)
			continue;
		Player[nIdx].m_ItemList.InsertEquipment(nNew, false);
		const int q2 = Player[nIdx].m_ItemList.FindSame(nNew);
		if (q2 <= 0)
		{
			ItemSet.Remove(nNew);
			continue;              // het slot m_Items (rat hiem)
		}
		// (phan bien dot 2) InsertEquipment khi tui DAY khong he bao loi: no nhet
		// mon vao pos_hand va VUT mon dang cam truoc do xuong dat thanh Object
		// cong khai (KItemList.cpp:3923-3989); FindSame tim thay ca do o TAY nen
		// nhanh "tui day" cu (q2 <= 0) gan nhu khong bao gio chay. Mon nam o tay
		// la mon TANG HINH: SendSellItemInfo/GetTradeCount chi dem pos_equiproom.
		// Toi tay = tui da day that -> huy mon nay va DUNG ca dot, khoi xa rac.
		if (Player[nIdx].m_ItemList.m_Items[q2].nPlace != pos_equiproom)
		{
			Player[nIdx].m_ItemList.RemoveItemIdx(nNew, Item[nNew].GetStackNum());
			break;
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
	return nBay;
}

// Sap dang mo ma cu 15 phut thay mot dot hang moi (du chua ban het) - chu game
// 19/08 toi #4. Nguoi dang xem sap luc thay hang chi bi "mua truot" mot nhip
// (TradeBuyItem phia server kiem lai item + gia truoc khi tru tien) - y het
// nguoi that dong/mo lai sap, khong co duong doi/mat do.
#define PB_SAP_LAM_MOI  (GAME_FPS * 900)

static void pb_BanSap(int nIdx, int nNpcIdx, int nSub, PB_Bot& b)
{
	const int nLech = (int)(&b - s_bots);
	const unsigned int nowT = SubWorld[nSub].m_dwCurrentTime;
	const PB_DtNpc& nha = s_dtNpc[nLech % 10];

	// [SapRai 28/08] chu game: "sap chong len nhau, khong rai deu". Xep cho ca khi
	// bot DANG O SAN thanh nha (truoc day bi bo qua -> ngoi ngay dam dong dang dung).
	if (SubWorld[nSub].m_SubWorldID != nha.nMap || !b.nSapChoXong)
	{
		if (nowT - b.nDaTauTick < (unsigned int)(GAME_FPS * 3))
			return;
		b.nDaTauTick = nowT;
		if (pb_TrongNhom(nIdx))
			pb_RoiNhom(nIdx, "ra thanh ngoi ban sap");
		// (19/08 toi chu game) CHI ngoi O TRONG quanh NPC Da Tau.
		const int nSubT = g_SubWorldSet.SearchWorld(nha.nMap);
		int nSx = 0, nSy = 0;
		int bDat = 0;
		int nTamX = nha.nX, nTamY = nha.nY;
		const int bCungMap = (SubWorld[nSub].m_SubWorldID == nha.nMap);
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
			// [SapRai 28/08] duyet TRON VANH DAI moi vong (4 canh x 2r = 8r o/vong,
			// ~600 o cho vanh 3..12) thay vi chi 8 tia (80 o, bot cung nLech%8 khoi
			// dau CUNG tia). Diem khoi dau tren vanh xoay theo chi so bot -> moi con
			// mot huong rieng, toa deu quanh quang truong nhu cho that; pb_OSapTot
			// (da them quet danh sach sap) tranh o co sap khac.
			unsigned char aReach[PB_SAP_BFS_O];
			pb_SapLoang(nSubT, nTamX, nTamY, aReach);
			for (int r3 = 3; r3 <= 12 && !bDat; r3++)
			{
				const int nChuVi = 8 * r3;
				const int nBd = ((nLech % 97) * 13 + r3 * 7) % nChuVi;
				for (int h3 = 0; h3 < nChuVi && !bDat; h3++)
				{
					const int i3 = (nBd + h3) % nChuVi;
					const int nCanh = i3 / (2 * r3);
					const int t3 = i3 % (2 * r3) - r3;   // -r .. r-1
					int cx3 = nTamX, cy3 = nTamY;
					if (nCanh == 0)      { cx3 += r3;  cy3 += t3; }
					else if (nCanh == 1) { cx3 -= r3;  cy3 += t3; }
					else if (nCanh == 2) { cx3 += t3;  cy3 += r3; }
					else                 { cx3 += t3;  cy3 -= r3; }
					if (!pb_OSapTot(nSubT, cx3, cy3, nTamX, nTamY, aReach))
						continue;
					nSx = cx3 * 32;
					nSy = cy3 * 32;
					if (bCungMap)
					{
						Npc[nNpcIdx].SetPos(nSx, nSy);
						bDat = 1;
					}
					else
						bDat = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nSx, nSy) == 1);
				}
			}
			// het vong ma chua dat duoc (quang truong chat kin / NPC dung goc khuat)
			// -> lui ve pb_ODat nhu cu: con hon khong bay duoc sap
			if (!bDat && pb_ODat(nSubT, nTamX, nTamY, nLech, 12, &nSx, &nSy))
			{
				if (bCungMap)
				{
					Npc[nNpcIdx].SetPos(nSx, nSy);
					bDat = 1;
				}
				else
					bDat = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nSx, nSy) == 1);
			}
		}
		if (!bDat)
		{
			if (bCungMap)
				Npc[nNpcIdx].SetPos(nha.nX * 32, nha.nY * 32);
			else
				Npc[nNpcIdx].ChangeWorld(nha.nMap, nha.nX * 32, nha.nY * 32);
		}
		else
			pb_Log("[BotSap] %s ngoi sap thanh %d o(%d,%d)\n",
			       Player[nIdx].m_PlayerName, nha.nMap, nSx / 32, nSy / 32);
		b.nSapChoXong = 1;      // co cho roi (hoac da lui ve diem bang) - khong xep lai
		b.nTargetNpc = 0;
		b.nBanSapNgoi = 0;
		b.walk.Reset();  b.chase.Reset();  b.roam.Reset();
		b.nRoamX = 0;    b.nRoamY = 0;
		b.nBuocRaX = 0;  b.nBuocRaY = 0;
		return;
	}

	// Cham hang khi: (a) lan dau mo sap; (b) HET HANG qua 5 phut; (c) DINH KY
	// PB_SAP_LAM_MOI (15 phut) du chua ban het - chu game 19/08 toi #4: "cap nhat
	// random item trang suc", sap thay hang deu dan cho nguoi choi ghe lai co cai xem.
	// (phan bien 19/08 toi) ky dinh ky SO LE theo chi so bot (+0..59 giay): ca 200
	// sap duoc dat trong CUNG mot khung nen nDaTauThu bang nhau tuyet doi - khong
	// so le thi cu 15 phut ca 200 sap thay hang dong loat trong MOT khung 55ms.
	const int nHetHang = (Player[nIdx].GetTradeCount() == 0);
	if (!b.nBanSapXong
	 || (nHetHang && nowT - b.nDaTauThu >= (unsigned int)(GAME_FPS * 300))
	 || nowT - b.nDaTauThu >= (unsigned int)(PB_SAP_LAM_MOI + (nLech % 60) * GAME_FPS))
	{
		const int nMoLai = b.nBanSapXong;      // 1 = sap dang mo san, chi thay hang
		b.nBanSapXong = 1;
		b.nDaTauThu = nowT;
		pb_NapTenSap();
		const int nBay = pb_ChamHangSap(nIdx, nLech, b.nSapDot, nowT);
		b.nSapDot++;
		// (phan bien 19/08 toi - LOI THAT suyt lot) MO SAP LAI MOI KY CHAM, PHAI
		// nam NGOAI if (!nMoLai): SendSellItemCount (KPlayer.cpp:10371) TU HA
		// m_BaiTan = 0 khi bat ky khach nao bam vao sap da ban het hang. Ban cu
		// set m_BaiTan = 1 vo dieu kien moi ky cham nen sap tu mo lai; neu chi
		// set luc mo lan dau thi sap chay hang la DONG VINH VIEN - bot van ngoi,
		// log van dep, nhung c2sTradeBuy chan !m_BaiTan, khong ai mua duoc nua.
		Npc[nNpcIdx].m_BaiTan = 1;
		if (nBay == 0)
			pb_Log("[BotSapLoi] %s dot %d khong bay duoc mon nao (CMIT thieu to hop"
			       " series/cap hay tui day?) - 5 phut sau tu cham lai\n",
			       Player[nIdx].m_PlayerName, b.nSapDot);
		if (!nMoLai)
		{
			if (s_sapTenSo > 0)
				strncpy(Npc[nNpcIdx].ShopName, s_sapTen[nLech % s_sapTenSo], 31);
			else
				strncpy(Npc[nNpcIdx].ShopName, "Sap tap hoa nho", 31);
			Npc[nNpcIdx].ShopName[31] = 0;
			Npc[nNpcIdx].SendCommand(do_stand);
			b.nBanSapNgoi = 0;             // ep ngoi lai o nhanh duoi
			pb_Log("[BotSap] %s mo sap '%s' tai map %d (%d mon trang suc co opt)\n",
			       Player[nIdx].m_PlayerName, Npc[nNpcIdx].ShopName, nha.nMap, nBay);
		}
		else
			pb_Log("[BotSap] %s lam moi sap dot %d (%s, %d mon trang suc moi)\n",
			       Player[nIdx].m_PlayerName, b.nSapDot,
			       nHetHang ? "het hang" : "dinh ky 15 phut", nBay);
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

// (19/08 dem - chu game: "bot luc len map thi bay thang ra ngoai map... lam lai
// cho bot phai tu di chuyen toi Xa Phu roi len map... toa do moi map da co san
// trong item Than Hanh Phu"). BANG MOI: chi giu cac bai co tuyen CHINH THONG -
// toa do CHEP NGUYEN VAN tu shenxingfu.lua TRAIN_ARRAY1/2 (dong dang waypoint da
// tra qua settings/WayPoint.txt), rieng Hoa Son la tuyen go_HSBattle co san cua
// menu Xa Phu. 9 bai cu toa do tay (170/182/164/79/206/198/181/875/225-227/
// 144/152...) BI LOAI - khong con duong "bay tu che" nao nua.
// (20/08 - bang duoi da duoc THAY bang ban 39 bai day du, xem chu thich ke tiep)
// (20/08 - chu game: "phai gioi han bot len map theo cap khong duoc vuot map
// hien tai; moc 80 co 4 map chu khong phai 2 map") BANG DAY DU 39 BAI.
//
// Ban 19/08 chep tu shenxingfu.lua TRAIN_ARRAY1/2 - HAI bang do CHI la menu phu
// "Trang Tiep / luyen cong tan thu", moi bac chi 2 map. Bang THAT ma nguoi choi
// dung nam o cac bang tab_lv20map .. tab_lv90map trong CUNG tep (shenxingfu.lua
// d.433-679), di qua menu chondiadiem1 -> "Ban do luyen cong tu 20 den 90":
//     moc 20: 2 map | 30: 2 | 40: 2 | 50: 2 | 60: 3 | 70: 3 | 80: 4 | 90: 13
// Bang duoi = HOP cua hai nguon (tab_lvXXmap + TRAIN_ARRAY con thieu), giu dung
// MOC CAP CHINH THONG cua tung map. Toa do chep nguyen van, DA KIEM tung diem
// tren luoi A* (39/39 dap duoc, o trong lien thong >= 194 o trong cua so 27x27).
//
// THU TU BANG NAY = THU TU BANG BOT_BAI trong script/global/station.lua
// (BOT_BAI[i+1] ung voi s_bai[i]). Sua mot ben la phai sua ca hai ben.
static const PB_BaiLuyen s_bai[] =
{
	{ 10,   2, 2605, 3592, "Hoa Son" },                  // go_HSBattle (Xa Phu)
	// ---- moc 20 (3 bai) ----
	{ 20,  19, 3102, 3963, "Kiem Cac Tay Nam" },
	{ 20,   7, 2276, 2825, "Tan Lang tang 1" },
	{ 20,  70, 1608, 3230, "Vu Lang Son" },
	// ---- moc 30 (4 bai) ----
	{ 30, 193, 1938, 2845, "Vu Di Son" },
	{ 30, 170, 1612, 3187, "Tho Phi Dong" },
	{ 30,  90, 1651, 3571, "Phuc Nguu Dong" },
	{ 30,  92, 1632, 3290, "Thuc Cuong Son" },
	// ---- moc 40 (4 bai) ----
	{ 40,  21, 2622, 4502, "Thanh Thanh Son" },
	{ 40, 167, 1575, 3239, "Diem Thuong Son" },
	{ 40,  41, 2078, 2805, "Phuc Nguu Tay" },
	{ 40, 122, 1612, 3323, "Hoang Ha Nguyen Dau" },
	// ---- moc 50 (4 bai) ----
	{ 50, 182, 1777, 2982, "Nghiet Long Dong" },
	{ 50, 164, 1611, 3187, "Thien Tam Thap tang 1" },
	{ 50, 125, 1809, 3208, "Luu Tien Dong" },
	{ 50, 163, 1558, 3199, "Oc Ba Dia Dao" },
	// ---- moc 60 (3 bai) ----
	{ 60,  79, 1600, 3206, "Tuong Duong Mat Dao" },
	{ 60,  56, 1516, 3443, "Hoanh Son Phai" },
	{ 60, 166, 1649, 3231, "Thien Tam Thap tang 3" },
	// ---- moc 70 (3 bai) ----
	{ 70, 319, 1630, 3587, "Lam Du Quan" },
	{ 70, 123, 1702, 3350, "Lao Ho Dong" },
	{ 70, 206, 1603, 3215, "Tan Lang tang 2" },
	// ---- moc 80 (4 bai - dung nhu chu game noi) ----
	{ 80, 224, 1622, 3118, "Sa Mac Dia Bieu" },
	{ 80, 198, 1521, 2947, "Thanh Khe Dong" },
	{ 80, 320, 1147, 3123, "Chan nui Truong Bach" },
	{ 80, 181, 1425, 2999, "Luong Thuy Dong" },
	// ---- moc 90 (13 bai) ----
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
	{ 90,  93, 1529, 3166, "Tien Cuc Dong" },
	{ 90, 124, 1675, 3418, "Can Vien Dong" },
	{ 90, 152, 1672, 3361, "Tuyet Bao Dong tang 8" },
};
#define PB_SO_BAI  (int)(sizeof(s_bai) / sizeof(s_bai[0]))

// (20/08) BO mang s_baiLc: hai bang phai khop THU TU nen anh xa chi la
// "chi so Lua = chi so C + 1". Giu mot bang tra tay chi tao them mot cho de
// lech nhau. Rieng bai 0 (Hoa Son) di tuyen go_HSBattle, khong qua BOT_BAI.
#define PB_BAI_LUA(i)   ((i) + 1)
// (20/08) KHONG con dung: pb_ChonBai nay luon lay bai IT BOT NHAT nen so bot tu
// can bang giua cac ung vien, khong can tran cung. Giu dinh nghia lam moc doc log.
#define PB_BAI_TRAN  120   // tran bot cho MOI bai luyen (gian dam dong 18/08)

// (19/08 toi #3 - chu game: "bot len map luyen cong van di chuyen ra ngoai map,
// map 79 nhieu nhat") DAY XICH BAI. Dieu tra map 79 (mo cache 79_srv.fp + doi
// chieu bot.log): du lieu vat can cua map chi ve TUONG me cung, con vung trong
// ngoai me cung (nhin tren client la "ngoai map") lai ghi = 0 tuc DI DUOC - ca
// engine lan luoi A* deu tin du lieu do nen moi rao 18-19/08 deu cho qua. QUAI
// cung dung/lang thang ra vung ay (engine cho di), pb_FindRoamSpot lai lay VI TRI
// QUAI lam dich roam ([BotHoang] 27 luot toi o 1686,3140 - giua vung trong), bot
// cu the theo quai "ra ngoai map". Khong the phan biet "vung trong gia" voi san
// choi that bang du lieu server, nen ep bang LUAT SAN PHAM: bot luyen o BAI thi
// chi danh/roam trong ban kinh xich quanh diem neo bai (diem neo la toa do da
// kiem chung tay trong s_bai). Xa hon xich = mac ke, quai ngoai do khong ton tai
// voi bot. Dat 2000 MPS (~62 o) de van rong hon le [BotDan] di tan >= 1200 MPS.
//
// (20/08 - chu game: "bot van gom 1 cho khong tan ra deu trong map") tung NOI
// XICH 2000 -> 9600 MPS voi ly le: ba lop chan (luoi A* / pb_CatDoan /
// pb_FindRoamSpot loc o luoi chan) da thay vai tro cua xich chat.
// (20/08 dem - chu game DINH CHINH + do that) SAI: ba lop do chi chan duoc o
// LUOI GHI CHAN, bat luc voi "vung trong gia" ma DU LIEU ghi di duoc (benh map
// 79 - nhin tren client la ngoai map). Xich 300 o trum luon vung ay: 2h log sau
// restart 20:25 co 65 luot [BotCuu] cach neo median 102 o, max 297 o - sat mep
// xich. Loi chu game: "toi keu neu o dam dong tren 20 bot thi tim toa do npc
// duoc add trong map do hop le xa de di chuyen, chu dau keu di chuyen ra ngoai
// map". SUA GOC: san choi cua bot = QUANH CAC TAM CUM DIEM SINH quai da add
// (pb_LayCum - m_OriginX/Y tu du lieu, luon nam trong map that): chon muc tieu,
// diem roam du phong va nga tha khi duoi deu phai cach MOT tam cum <=
// PB_XICH_CUM; ung vien di tan la CHINH tam cum nen van duoc di XA toan map
// (dung dac ta tan deu). Day xich neo duoi day chi con la DUONG LUI cho ban do
// chua co bang cum, tra ve gia tri truoc khi noi.
#define PB_XICH_BAI      2000   // MPS - DUONG LUI (map chua co bang cum): loc theo neo
#define PB_XICH_BAI_THA  2320   // MPS - nga cam khi DANG duoi (them 320 de khong
                                // nha oan muc tieu dang danh do o sat mep xich)
// (phan bien dot 2) MAP NHIEM VU loai 4 xich RONG hon (100 o thay 62,5): quai roi
// cuon rai khap map, xich 2000 quanh diem tha co the thieu quai -> "farm 20 phut
// khong ra cuon" doi nhiem vu lien tuc, mat thanh qua "T4 roam tim quai" dot sang.
#define PB_XICH_NV       3200
#define PB_XICH_NV_THA   3520
// (20/08 dem) XICH CUM: PB_CUM_CACH (1280) la khoang cach toi da tu mot diem
// sinh toi tam cum cua no, cong ~600 MPS quai lang thang quanh diem sinh =>
// 2000 phu du moi quai hop le ma khong voi toi vung trong gia (cach cum that
// thuong hang tram o). Do theo TUNG TRUC (cung phep voi khoi gom cum) cho re.
#define PB_XICH_CUM      2000
#define PB_XICH_CUM_THA  2320

// (19/08 toi #4 - "Bot van dung ngoai map, kiem ky hon") NEO TU SUA. Phap y sau
// restart 19:20: PhamVu342 duoc [BotCuu] tha ve DUNG toa do neo Lao Ho Dong
// (1702,3350) roi 49 giay sau lai "ket dao A*" tai (1710,3345) -> cuu lai ve dung
// cho do - vong lap cuu-tai-cho moi 60 giay, bot dung im giua "vung den" = canh
// "van dung ngoai map" chu game thay (Hoanh Son Phai 21 luot, Truong Bach 11,
// Lao Ho Dong 10 chi trong 17 phut). Goc: toa do neo trong s_bai duoc chon tay
// TU TRUOC khi luoi 19/08 son dac cac region thieu du lieu - mot so neo nay nam
// lot trong / sat canh vung son dac, thanh DAO nho.
// Sua: lan DAU dung toi neo cua moi bai, do DO LIEN THONG bang chinh ban do loang
// BFS cua sap (pb_SapLoang, cua so 27x27): neo loang duoc < PB_NEO_LOANG_MIN o
// thi quet xoan oc buoc 4 o (ban kinh toi 40) tim tam thay the loang du rong,
// cache lai cho ca doi server. Moi noi dung neo (xich, pb_RaBai, [BotCuu]) deu
// di qua pb_LayNeo nen tu huong loi.
#define PB_NEO_LOANG_MIN  60
static int s_aNeoTinh[PB_SO_BAI];             // 0 = chua kiem
static int s_aNeoOX[PB_SO_BAI], s_aNeoOY[PB_SO_BAI];

// (20/08) 1 = bai nay ChangeWorld that bai (may chu chua mo map) -> pb_ChonBai
// KHONG duoc chon lai nua. Khong co co nay thi bai hong luon co 0 bot va vi the
// luon thang o phep "lay bai it bot nhat" - hut ca moc cap vao vong lap song.
static unsigned char s_baiHong[PB_SO_BAI] = { 0 };

static void pb_LayNeo(int nBai, int nSub, int* pnOX, int* pnOY)
{
	if (nBai < 0 || nBai >= PB_SO_BAI)
	{
		*pnOX = 0;  *pnOY = 0;
		return;
	}
	if (!s_aNeoTinh[nBai])
	{
		s_aNeoTinh[nBai] = 1;
		s_aNeoOX[nBai] = s_bai[nBai].nOX;
		s_aNeoOY[nBai] = s_bai[nBai].nOY;
		if (nSub >= 0 && nSub < MAX_SUBWORLD
		 && SubWorld[nSub].m_SubWorldID == s_bai[nBai].nMapId)
		{
			unsigned char aReach[PB_SAP_BFS_O];
			const int nGoc = pb_SapLoang(nSub, s_bai[nBai].nOX, s_bai[nBai].nOY, aReach);
			if (nGoc < PB_NEO_LOANG_MIN)
			{
				static const int aNx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
				static const int aNy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
				int nTot = 0;
				int nChamGoc = 0;
				// VONG 1 (phan bien dot 2): chi nhan ung vien r <= 12 vua loang du
				// rong VUA CHAM DUOC O GOC (hoac 1 trong 4 o lan can goc) trong ban
				// do loang cua chinh no = cung thanh phan lien thong voi neo cu.
				// Thieu dieu kien nay, ung vien dau tien dat >= 60 o rat de la
				// "vung trong gia" (void ghi du-lieu-di-duoc, loang menh mong) va
				// pb_RaBai/[BotCuu] se teleport CA BAI vao void - te hon benh cu.
				for (int r = 4; r <= 12 && !nTot; r += 4)
				{
					for (int h = 0; h < 8 && !nTot; h++)
					{
						const int cx = s_bai[nBai].nOX + aNx[h] * r;
						const int cy = s_bai[nBai].nOY + aNy[h] * r;
						if (!pb_ODuoc(nSub, cx * 32, cy * 32))
							continue;
						const int nL = pb_SapLoang(nSub, cx, cy, aReach);
						if (nL < PB_NEO_LOANG_MIN)
							continue;
						int nCham = 0;
						static const int aGx5[5] = { 0, 1, -1, 0, 0 };
						static const int aGy5[5] = { 0, 0, 0, 1, -1 };
						for (int g5 = 0; g5 < 5 && !nCham; g5++)
						{
							const int lgx = (s_bai[nBai].nOX + aGx5[g5]) - (cx - PB_SAP_BFS_R);
							const int lgy = (s_bai[nBai].nOY + aGy5[g5]) - (cy - PB_SAP_BFS_R);
							if (lgx >= 0 && lgx < PB_SAP_BFS_W
							 && lgy >= 0 && lgy < PB_SAP_BFS_W
							 && aReach[lgy * PB_SAP_BFS_W + lgx])
								nCham = 1;
						}
						if (!nCham)
							continue;
						s_aNeoOX[nBai] = cx;
						s_aNeoOY[nBai] = cy;
						nTot = nL;
						nChamGoc = 1;
					}
				}
				// VONG 2: goc nam trong dao co lap hoan toan (khong ung vien nao
				// cham toi) - danh nhan tam loang-rong GAN NHAT va log ro de chu
				// game DUYET TAY (co the la void; xem hinh fp_view truoc khi tin).
				for (int r = 4; r <= 40 && !nTot; r += 4)
				{
					for (int h = 0; h < 8 && !nTot; h++)
					{
						const int cx = s_bai[nBai].nOX + aNx[h] * r;
						const int cy = s_bai[nBai].nOY + aNy[h] * r;
						if (!pb_ODuoc(nSub, cx * 32, cy * 32))
							continue;
						const int nL = pb_SapLoang(nSub, cx, cy, aReach);
						if (nL < PB_NEO_LOANG_MIN)
							continue;
						s_aNeoOX[nBai] = cx;
						s_aNeoOY[nBai] = cy;
						nTot = nL;
					}
				}
				if (nTot)
					pb_Log("[BotNeo] bai %s: neo goc (%d,%d) loang chi %d o (dao/ket)"
					       " -> DOI neo sang (%d,%d) loang %d o (%s)\n",
					       s_bai[nBai].szTen, s_bai[nBai].nOX, s_bai[nBai].nOY, nGoc,
					       s_aNeoOX[nBai], s_aNeoOY[nBai], nTot,
					       nChamGoc ? "noi duoc voi goc"
					              : "KHONG cham duoc goc - DUYET TAY keo vao void");
				else
					pb_Log("[BotNeo] CANH BAO bai %s: neo (%d,%d) loang %d o va KHONG"
					       " tim duoc tam thay the trong 40 o - kiem lai toa do s_bai\n",
					       s_bai[nBai].szTen, s_bai[nBai].nOX, s_bai[nBai].nOY, nGoc);
			}
		}
		else
			s_aNeoTinh[nBai] = 0;   // goi voi nSub sai map: dung tra goc, dung cache
	}
	*pnOX = s_aNeoOX[nBai];
	*pnOY = s_aNeoOY[nBai];
}

// Tra LOAI NEO + toa do neo (MPS): 1 = neo BAI (xich PB_XICH_BAI), 2 = neo MAP
// NHIEM VU loai 4 (xich PB_XICH_NV rong hon - neo la diem dat chan sau khi Xa Phu
// tha, chan bot duoi quai lang thang ra "vung trong gia" cua map nhiem vu, lo mo
// ra khi nang tran cuon cho loai 4 chay that). 0 = khong xich.
static int pb_NeoBai(const PB_Bot& b, int nSub, int* pnX, int* pnY)
{
	if (nSub < 0 || nSub >= MAX_SUBWORLD)
		return 0;
	if (b.nDtPha == DTB_FARM_NV && b.nDtNeoX > 0
	 && b.nDtNeoMap == SubWorld[nSub].m_SubWorldID)
	{
		*pnX = b.nDtNeoX;
		*pnY = b.nDtNeoY;
		return 2;
	}
	if (b.nBaiIdx < 0 || b.nBaiIdx >= PB_SO_BAI)
		return 0;
	if (SubWorld[nSub].m_SubWorldID != s_bai[b.nBaiIdx].nMapId)
		return 0;
	int nOX = 0, nOY = 0;
	pb_LayNeo(b.nBaiIdx, nSub, &nOX, &nOY);
	*pnX = nOX * 32;
	*pnY = nOY * 32;
	return 1;
}

// MOC CAP cao nhat ma cap nay voi toi. -1 = chua du cap 10.
static int pb_MocCua(int nLevel)
{
	int nMoc = -1;
	for (int i = 0; i < PB_SO_BAI; i++)
		if (nLevel >= s_bai[i].nCapToiThieu && s_bai[i].nCapToiThieu > nMoc)
			nMoc = s_bai[i].nCapToiThieu;
	return nMoc;
}

// Chon bai. Tra chi so trong s_bai, hoac -1 neu chua du cap 10.
// nBoQua = khe bot dang hoi (de KHONG dem chinh no); -1 = khong bo ai.
//
// (20/08 - chu game: "phai gioi han bot len map theo cap khong duoc vuot map
// hien tai") CHI lay bai DUNG MOC cua bot, khong lui bac, khong vuot bac:
//   - vuot bac thi quai qua manh + trai luat game (bai co chan cap trong lua);
//   - lui bac thi bot cap 89 di dam Luu Tien Dong cap 50. Ban 20/08 som tung
//     cho lui 30 cap CONG voi luat "lay bai it bot nhat" - hai thu do cong lai
//     thanh: bac cap nao it dan cu NHAT luon thang, tuc bot cap cao bi hut het
//     xuong bai cap thap. Da bo.
// Chia deu gio dua han vao viec MOI BAC CO NHIEU BAI (bang 39 bai: moc 80 co 4,
// moc 90 co 13) cong voi viec tan deu TRONG map (pb_FindRoamSpot theo diem sinh).
static int pb_ChonBai(int nLevel, int nLech, int nBoQua)
{
	const int nMoc = pb_MocCua(nLevel);
	if (nMoc < 0)
		return -1;

	// (phan bien 20/08) PHAI LOAI BAI HONG. Truoc day mot bai khong vao duoc chi
	// bi bot do bo (nBaiIdx = -1) chu khong bi cam, ma bo dem aDong lai dem "so
	// bot dang GIU bai" - bai hong luon co 0 bot nen no la cuc tieu tuyet doi,
	// tuc luon duoc chon lai: bot phu ve thanh, di bo 5 phut toi Xa Phu, ChangeWorld
	// truot, dat -1, roi chon lai DUNG no - vong lap song, ca moc cap do te liet.
	// Bang 39 bai co 17 map bot CHUA TUNG dat chan nen rui ro nay la that.
	int aTm[PB_SO_BAI], n = 0;
	for (int i = 0; i < PB_SO_BAI; i++)
	{
		if (s_bai[i].nCapToiThieu != nMoc)              continue;
		if (s_baiHong[i])                               continue;
		if (g_SubWorldSet.SearchWorld(s_bai[i].nMapId) < 0) continue;   // may chu chua nap map
		aTm[n++] = i;
	}
	// Ca moc deu hong / chua nap: lui dan xuong cac moc duoi cho toi khi co bai,
	// con hon la de bot dung im vinh vien (day la duong CUU HO, khong phai luat
	// thuong - luat thuong van la dung moc).
	for (int nLui = nMoc - 10; n <= 0 && nLui >= 10; nLui -= 10)
		for (int i = 0; i < PB_SO_BAI; i++)
		{
			if (s_bai[i].nCapToiThieu != nLui)              continue;
			if (s_baiHong[i])                               continue;
			if (g_SubWorldSet.SearchWorld(s_bai[i].nMapId) < 0) continue;
			aTm[n++] = i;
		}
	if (n <= 0)
		return -1;

	// DEM MOT LUOT cho toan bang (truoc day quet lai s_bots cho TUNG ung vien =
	// n x s_botCount phep so moi lan chon; 1000 bot nap cung luc thi day la vai
	// trieu vong lap khong can thiet).
	// (phan bien 20/08) PHAI TRU CHINH MINH: khi bot len cap va chon lai, b.nBaiIdx
	// van dang giu bai CU nen bai do bi cong them 1 la chinh no -> gan nhu lan nao
	// cung thua mot bai khac dang hoa, bot bi day di map moi MOI LAN LEN CAP va
	// mat ca phut phu ve thanh + di bo toi Xa Phu.
	int aDong[PB_SO_BAI];
	for (int i = 0; i < PB_SO_BAI; i++)
		aDong[i] = 0;
	for (int q = 0; q < s_botCount; q++)
	{
		if (q == nBoQua)
			continue;
		// (21/08 phan bien) bot dang trong Tong Kim KHONG con o bai: dem no vao
		// thi suot tran, ca dan con lai se tranh dung nhung bai thuc ra dang
		// trong rong. Khuon giong PB_SetDaTau / PB_SetBanSap.
		if (s_bots[q].nTk)
			continue;
		// (phan bien 20/08) chi dem KHE CON SONG: khe da bi thu hoi / cap lai cho
		// nguoi choi that van giu nBaiIdx cu va se thoi phong dan so cua bai do
		// vinh vien, lam cac bot khac tranh no mai.
		const int p = s_bots[q].nPlayerIdx;
		if (p <= 0 || p >= MAX_PLAYER || Player[p].m_dwID != s_bots[q].dwID)
			continue;
		const int k = s_bots[q].nBaiIdx;
		if (k >= 0 && k < PB_SO_BAI)
			aDong[k]++;
	}

	// Trong cac bai CUNG MOC: lay bai IT BOT NHAT. Hoa nhau thi theo diem boc
	// xoay (tron chi so bot vi g_Random dong bang theo giay - ca loat bot chon
	// trong cung mot khung se ra cung mot so neu khong tron).
	const int nBat = ((int)g_Random(n) + nLech * 7) % n;
	int nChon = -1, nDongChon = 0x7fffffff;
	for (int v = 0; v < n; v++)
	{
		const int nUng = aTm[(nBat + v) % n];
		if (aDong[nUng] < nDongChon)
		{
			nDongChon = aDong[nUng];
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

// (23/08 chu game: "co tuong bot khong chay qua tuong ma dung 2 ben tuong danh
// nhau, trong khi danh khong trung ma dung danh mai")
// "NHIN THAY" muc tieu = doan thang noi hai ben khong cat qua o vat can (lay mau
// ~1 o mot bang pb_ODuoc). Dung trong Tong Kim: tuong thanh chay doc eo giua hai
// doanh trai ma tam nhin 700 MPS thi xuyen tuong -> ca hai dan khoa muc tieu ben
// kia vach, dam chieu khong toi noi; dong ho "10 giay khong sut mau" cam duoc
// con nay thi con KE BEN (cung sau tuong) lai duoc chon - sau tuong co hang chuc
// dich nen xoay vong vo han, hai ben dung hinh doc theo vach dung canh chu game
// chup duoc. Loc ngay tu luc CHON muc tieu thi bot coi nhu chua thay dich va di
// tiep theo lo trinh - A* tu dan vong qua cong thanh roi moi danh.
static int pb_ThayDuoc(int nSub, int x1, int y1, int x2, int y2)
{
	int dx = x2 - x1;  if (dx < 0) dx = -dx;
	int dy = y2 - y1;  if (dy < 0) dy = -dy;
	int nBuoc = ((dx > dy) ? dx : dy) / 32;
	if (nBuoc <= 1)
		return 1;                          // sat canh nhau - khong co cho cho tuong
	if (nBuoc > 24)
		nBuoc = 24;                        // tran cung (tam nhin 700 MPS ~ 22 o)
	for (int s = 1; s < nBuoc; s++)
		if (!pb_ODuoc(nSub, x1 + (x2 - x1) * s / nBuoc, y1 + (y2 - y1) * s / nBuoc))
			return 0;
	return 1;
}

// (20/08 dem) dung ngay duoi day nhung than ham nam o khu "gom cum" phia sau -
// khai bao truoc.
struct PB_CumMap;
static PB_CumMap* pb_LayCum(int nSub, int nRefNpc);
static int pb_GanCum(const PB_CumMap* pC, int x, int y, int nTam);

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

	// (19/08 toi #3) day xich bai: quai dung ngoai ban kinh xich quanh diem neo
	// bai coi nhu khong ton tai (chan tu goc viec bam theo quai lang thang trong
	// "vung trong gia" nhin-nhu-ngoai-map - xem chu thich PB_XICH_BAI).
	// (20/08 dem) co bang cum -> san choi do theo TAM CUM diem sinh (pb_GanCum);
	// xich neo chi con la duong lui. Xem chu thich PB_XICH_CUM.
	int nNeoX = 0, nNeoY = 0;
	const int bNeo = pb_NeoBai(b, nSub, &nNeoX, &nNeoY);
	const int nXich = (bNeo == 2) ? PB_XICH_NV : PB_XICH_BAI;
	// (21/08 - chu game: "van con bot ra ngoai map, keo log ve") BO GATE bNeo:
	// rao cum ap theo BAN DO DANG DUNG. bNeo == 0 (bot dung map KHAC bai duoc
	// gan - vd vua doi bai/len moc, dang cho cua so Xa Phu 60 giay van danh
	// quai o map cu; hoac Da Tau ngoai pha farm) truoc day la KHONG RAO GI CA -
	// lo cuoi de bot duoi quai lang thang ra "vung trong gia" ma khong bo loc
	// nao cham toi. Ban do khong co bang cum (thanh/thon) -> NULL -> nhu cu.
	// (21/08 phan bien) TRONG TONG KIM KHONG RAO CUM. Rao cum sinh ra de chan bot
	// duoi quai lang thang ra "vung trong gia" o bai luyen; trong chien truong no
	// lai loc mat DICH (ke ca nguoi choi that) dang dung ngoai cac cum diem-sinh-
	// quai -> bot khong NHIN THAY de danh. Dung dung cap dieu kien nhu cho
	// kind_player o duoi de khong noi long oan cho bot ngoai tran.
	const int bTrongTK = (b.nTk >= 4 && SubWorld[nSub].m_SubWorldID == PB_TK_MAP);
	const PB_CumMap* pCum = bTrongTK ? NULL : pb_LayCum(nSub, nNpcIdx);

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
			// (21/08 - chu game: "bot danh nguoi choi nhu nguoi choi danh voi
			// nguoi choi") RIENG TRONG TONG KIM thi danh CA NGUOI. Kep chat bang
			// HAI dieu kien: bot da RA TRAN (nTk >= 4) VA dang dung dung map
			// chien truong. Mo tran la tai hoa: KNpcSet::GetRelation (KNpcSet.cpp:
			// 1770-1785) cho hai NGUOI CHOI la dich chi can khac m_CurrentCamp +
			// ca hai bat FightMode + ca hai co co PK (mac dinh BAT) - KHONG he co
			// dieu kien cung map hay cung mission. Bo hai dieu kien nay la bot
			// danh nguoi choi that khac camp o giua thanh, o bai luyen, o moi noi.
			if (Npc[i].m_Kind == kind_player
			 && !(b.nTk >= 4 && SubWorld[nSub].m_SubWorldID == PB_TK_MAP))
				continue;
			if (!(NpcSet.GetRelation(nNpcIdx, i) & relation_enemy)) continue;
			if (pb_BiCam(b, i, now))                         continue;

			int ex = 0, ey = 0;
			Npc[i].GetMpsPos(&ex, &ey);
			const int d = g_GetDistance(bx, by, ex, ey);
			if (d >= nVision)
				continue;
			if (!bTrongTK
			 && (pCum ? !pb_GanCum(pCum, ex, ey, PB_XICH_CUM)
			          : (bNeo && g_GetDistance(nNeoX, nNeoY, ex, ey) > nXich)))
				continue;              // ngoai san choi -> khong nhin thay
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
	// (23/08) trong Tong Kim CHI nhan muc tieu NHIN THAY duoc (khong bi tuong
	// chan giua) - xem chu thich pb_ThayDuoc. Duyet du 8 ung vien bat dau tu vi
	// tri boc rieng cua minh; khong con nao thay duoc thi coi nhu CHUA co dich,
	// bot di tiep theo lo trinh thay vi dung ngoai vach dam chieu vao tuong.
	if (bTrongTK)
	{
		// [TK-DEU 28/08] "gan nhat TUYET DOI" lam moi bot quanh mot khu cung dam dung
		// MOT nan nhan -> tap trung hoa luc xoa 1-2 giay/mang, ~500 mang/phut, ~40%
		// quan so nam trong trai cho hoi sinh (chu game: "ket trong doanh trai").
		// Van la "gan" nhung CHIA trong NHOM 4 GAN NHAT theo chi so bot; het nhom do
		// moi duyet tiep theo khoang cach (aId da sap tang dan).
		const int nChia = (nCand < 4) ? nCand : 4;
		for (int v = 0; v < nCand; v++)
		{
			const int i9 = (v < nChia)
			             ? (int)(((unsigned)nLech + (unsigned)v) % (unsigned)nChia)
			             : v;
			const int i3 = aId[i9];
			int ex3 = 0, ey3 = 0;
			Npc[i3].GetMpsPos(&ex3, &ey3);
			if (pb_ThayDuoc(nSub, bx, by, ex3, ey3))
				return i3;
		}
		return 0;
	}
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
	int nBestNoi = 0;
	// (23/08 chu game: "bot tham gia tong kim toi thay toan he ngoai cong khong
	// co noi cong") NUA DAN thien NOI CONG: bot co dwID le se UU TIEN chieu SAT
	// THUONG PHEP (don magic/cold/fire/lighting/poison) len tren bac khop-vu-khi.
	// Vi sao truoc do ca dan danh vat ly het: bot nao cung duoc phat vu khi
	// ("trang bi theo cap" 19/08), ma bac khop-vu-khi (nRank 2) an dut chieu phep
	// -2 (nRank 1) - vi du Thuy Yen co Vu Da Le Hoa (105, Eqt=1 dao) va Bich Hai
	// Trieu Sinh (111, Eqt=-2) nhung Song Dao vat ly luon thang. Chon theo dwID
	// (chan/le) de on dinh giua cac phien; phai khong co chieu phep hop le (Thieu
	// Lam...) thi bNoi = 0 cho moi ung vien va tu roi ve vat ly nhu cu.
	const int bThienNoi = pb_BotNoi(nIdx);   // [BotNoi 28/08] mot nguon su that voi duong tay-khong
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
		// [MANA 31/08] Chi phi chieu VUOT TRAN tai nguyen cua bot = te liet IM LANG:
		// KNpc::DoSkill (KNpc.cpp:2604-2606) chay `if (!IsPlayer() || Cost(...))`, Cost
		// tra FALSE thi bo ca than lenh va roi thang xuong nhan Exit: -> DoStand()
		// = DUNG YEN, khong mot dong bao loi (nhanh bao het noi luc chi co ban client).
		// CanCastSkill KHONG kiem cost nen bo tham do cung khong bat duoc.
		// Do that 31/08: chieu 303 cap 20 ton 60 mana, nhan vat cap 20 tran chi 45-57
		// -> 1.963/1.963 mau deu thieu, 0 lan ra chieu, bot Duong Mon ket vinh vien.
		// Loai han ung vien khong bao gio tra noi (so voi TRAN, khong so voi hien tai:
		// hien tai con dao dong theo hoi mana / uong thuoc).
		{
			const int nGia = p->GetSkillCost((void*)&Npc[nNpcIdx]);
			if (nGia > 0)
			{
				int nTran = -1;
				switch (p->GetSkillCostType())
				{
				case attrib_mana_v:     nTran = Npc[nNpcIdx].m_CurrentManaMax;    break;
				case attrib_stamina_v:  nTran = Npc[nNpcIdx].m_CurrentStaminaMax; break;
				case attrib_life_v:     nTran = Npc[nNpcIdx].m_CurrentLifeMax;    break;
				default: break;      // loai tai nguyen khac -> khong chan
				}
				if (nTran >= 0 && nGia > nTran)
				{ PB_DIAG(" %d:COST=%d>%d", id, nGia, nTran); continue; }
			}
		}
		// UU TIEN chieu CO DON SAT THUONG (khuon DealsDamage cua ban tham khao,
		// KBotManager.cpp:1247). Lam dang UU TIEN chu khong chan cung: neu ca danh
		// sach deu khong co don sat thuong thi van chon duoc, khong tai dien canh
		// "khong phai nao danh duoc".
		const int nDmg = (p->GetDamageAttribsNum() > 0) ? 1 : 0;
		// (23/08) chieu nay co DON SAT THUONG PHEP khong. KSkills.cpp:2521 dua moi
		// attrib trong khoang (magic_damage_begin, magic_damage_end) vao
		// m_DamageAttribs bat ke chieu Missles hay Melee, nen quet o day bat duoc
		// ca phep dan bay (67 Cuu Thien Cuong Loi: lightingdamage_v) lan chuong
		// can chien. CHI tinh 5 loai don phep that (59, 62..65) - khong tinh
		// attackrating/ignoredefense/seriesdamage cung nam trong khoang do.
		// [SPARSE 31/08] doc theo O; BO doc khoi "don phep" (doc = 0 sat thuong voi quai)
		// nen chieu doc khong con duoc uu tien lam "chieu noi cong".
		const int bNoi = pb_DonPhepThat(p);
		// [303-DOC 30/08] chieu PHEP ma MOI don sat thuong deu la POISON (303 Doc
		// Thach Cot cua DM: duci_gu chi co poisondamage_v): do that 29-30/08 co
		// 199k cu "10 giay khong sut mau" vao quai HP600 con nguyen mau - doc khong
		// bao mon quai tren build nay. LOAI HAN ung vien loai nay: DM cap 20 se roi
		// ve don danh thuong (fallback DM20) va len 30 co chieu that. Chieu VAT LY
		// mang doc phu van giu (con sat thuong vu khi).
		// [SPARSE 31/08] doc lap khoi bNoi: chieu KHONG an theo vu khi (IsPhysical=0)
		// ma don sat thuong DUY NHAT la doc -> vo dung voi quai (303 Doc Thach Cot).
		// 303 con co seriesdamage_p (o [3]) nen phai so theo O DON THAT, khong the
		// "co attrib khac doc" nhu ban cu.
		if (!p->IsPhysical() && pb_DonDoc(p)
		 && !pb_DonPhepThat(p) && !pb_DonVatLy(p))
		{ PB_DIAG(" %d:DOCTHUAN", id); continue; }
		const int nNoi = bThienNoi ? bNoi : 0;
		PB_DIAG(" %d:OK(lv%d,r%d,rq%d,d%d,n%d)", id, lv, nRank, nRq, nDmg, bNoi);

		if (nNoi > nBestNoi
		 || (nNoi == nBestNoi && (nRank > nBestRank
		 || (nRank == nBestRank && nDmg > nBestDmg)
		 || (nRank == nBestRank && nDmg == nBestDmg && nRq > nBestRq)
		 || (nRank == nBestRank && nDmg == nBestDmg && nRq == nBestRq && lv > nBestLv))))
		{
			nBestNoi  = nNoi;
			nBestRank = nRank;
			nBestDmg  = nDmg;
			nBestRq   = nRq;
			nBest     = id;
			nBestLv   = lv;
		}
	}
	// [DM20 30/08] (chu game: "KHONG doi skills.txt; cap thap khong co skill phai
	// thi lay skill danh duoc cap thap hon"). Duong Mon: chieu som nhat rq=30 ->
	// bot cap < 30 khong co ung vien nao qua loc CAP; RemoveAllSkill luc tao bot
	// da xoa ca don danh thuong cua mau, hockynang chi day chieu phai -> nBest=0
	// -> bot dung nhin. Day lai DON DANH THUONG id 1 (rq=0, eqt=-2 moi vu khi,
	// series=-1 khong vuong loc he) roi dung no danh tam - y het nguoi choi thap
	// cap danh don trang; du cap chieu phai thi gate doi-cap tu pick lai chieu xin.
	if (nBest == 0)
	{
		int nCoCb = 0;
		for (int q9 = 1; q9 < MAX_NPCSKILL; q9++)
			if (sl.m_Skills[q9].SkillId == 1)
			{
				nCoCb = 1;
				break;
			}
		if (!nCoCb)
			sl.Add(1, 1);
		if (g_SkillManager.GetSkill(1, 1))
		{
			nBest   = 1;
			nBestLv = 1;
			pb_Log("[BotChon] %s (he %d, vu khi %d) khong co chieu phai du cap"
			       " -> dung DON DANH THUONG (id 1)\n",
			       Player[nIdx].m_PlayerName, (int)Npc[nNpcIdx].m_Series, nWant);
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

	// Chon lai bai khi chua co, hoac khi da DOI MOC CAP (khong phai moi lan len
	// cap: (phan bien 20/08) chon lai moi cap + bot tu dem chinh minh = bot doi
	// bai gan nhu moi lan len cap, va moi lan doi bai la mot chuyen phu ve thanh
	// + di bo toi Xa Phu; bot len cap nhanh se song tren duong chu khong o bai).
	// b.nBaiLevel gio giu MOC (10/20/.../90), khong giu cap.
	if (b.nBaiIdx < 0 || b.nBaiLevel != pb_MocCua(nLevel))
	{
		const int nBaiCu = b.nBaiIdx;
		b.nBaiLevel = pb_MocCua(nLevel);
		// (phan bien 20/08) CA TO DOI DUNG CHUNG BAI CUA DOI TRUONG. pb_ChonBai
		// chia bai theo TUNG bot va con tru chinh minh, nen hai thanh vien cung
		// cap gan nhu chac chan duoc phat HAI bai khac nhau -> thanh vien roi bai,
		// khoi "bam theo doi truong" khong bao gio chay duoc nua (tinh nang to
		// doi + chia exp chet), va bot lac doi truong thi cung khong duoc tan dan.
		int nBaiTruong = -1;
		if (pb_TrongNhom(nIdx) && pb_DoiTruongCua(nIdx) != nIdx)
		{
			const int nT = pb_DoiTruongCua(nIdx);
			if (nT > 0 && nT < MAX_PLAYER && PB_IsBot(nT))
				for (int q = 0; q < s_botCount; q++)
					if (s_bots[q].nPlayerIdx == nT)
					{
						const int k = s_bots[q].nBaiIdx;
						// chi theo khi bai do dung MOC cap cua chinh minh
						if (k >= 0 && k < PB_SO_BAI && !s_baiHong[k]
						 && s_bai[k].nCapToiThieu == b.nBaiLevel)
							nBaiTruong = k;
						break;
					}
		}
		b.nBaiIdx   = (nBaiTruong >= 0) ? nBaiTruong : pb_ChonBai(nLevel, nLech, nLech);
		if (b.nBaiIdx != nBaiCu)
		{
			// (20/08) doi bai (len MOC moi) -> lam lai tu dau duong
			// Xa Phu: "dang o map 20 ma len cap 30 thi phu ve thanh va toi xa
			// phu de len map 30" - dung nguyen loi chu game.
			b.nRaBaiThu = 0;  b.nRaBaiGoi = 0;  b.nDoiMapTick = 0;
		}
	}
	if (b.nBaiIdx < 0)
		return 1;               // chua du cap 10 -> cu danh tai cho

	const PB_BaiLuyen& bai = s_bai[b.nBaiIdx];
	if (SubWorld[nSub].m_SubWorldID == bai.nMapId)
	{
		// (19/08 dem) vua DAP bang duong Xa Phu: buoc ra khoi diem dap chinh
		// thong (nhu vua xuong xe - ca dan cung xuong mot cho thi gian dam dong),
		// chot log roi ve nhip danh quai binh thuong.
		if (b.nRaBaiGoi > 0 || b.nRaBaiThu > 0)
		{
			pb_Log("[BotBai] %s cap %d toi %s (map %d) bang duong XA PHU\n",
			       Player[nIdx].m_PlayerName, nLevel, bai.szTen, bai.nMapId);
			b.nRaBaiThu = 0;  b.nRaBaiGoi = 0;  b.nDoiMapTick = 0;
			int nHx = 0, nHy = 0;
			Npc[nNpcIdx].GetMpsPos(&nHx, &nHy);
			// (20/08 - chu game: "bot van gom 1 cho khong tan ra deu trong map")
			// TAN XA HON NGAY TU DIEM DAP. Script Xa Phu (station.lua botlc_go ->
			// NewWorld) tha MOI con xuong DUNG mot o co dinh, nen ca ~130 bot cua
			// mot bai deu xuat phat tu mot diem; ban cu buoc ra 8-11 o theo 4 huong
			// cheo = ca dam van nam trong mot dia ban kinh 11 o. Nay: 8 huong x 10
			// muc ban kinh (12..66 o) = 80 diem dap rieng, boc theo CHI SO BOT nen
			// khong phu thuoc g_Random (ham nay dong bang theo giay - ca loat bot
			// doi map cung khung se ra cung mot so).
			static const int aRx8[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
			static const int aRy8[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
			const int nHuong = (nLech + (int)g_Random(8)) & 7;
			const int nXaR   = 12 + ((nLech / 8) % 10) * 6;      // 12..66 o
			int nRx = 0, nRy = 0;
			if (pb_ODat(nSub, nHx / 32 + aRx8[nHuong] * nXaR,
			            nHy / 32 + aRy8[nHuong] * nXaR, nLech, 12, &nRx, &nRy))
			{
				b.nBuocRaX = nRx;
				b.nBuocRaY = nRy;
			}
			b.roam.Reset();
		}
		return 1;               // da o dung bai
	}

	// ======================= (19/08 dem) DUONG XA PHU =======================
	// Chu game: "ep bot muon len map luyen cong thi ve thanh roi toi xa phu len
	// lai map bang cap... lam nguoi choi nghi bot cung la nguoi choi - khong nen
	// cho bot bay thang len map". May 2 pha, diem dap luon la toa do CHINH THONG:
	//   (1) KHONG o thanh -> "phu ve thanh nha" (khuon pb_DtVeThanh - nhu dung
	//       Tho Dia Phu; thanh nha dong theo b.nThanhNhaMap neu tung bi goi ve)
	//   (2) O thanh      -> DI BO toi NPC Xa Phu roi goi script station.lua nhu
	//       nguoi choi bam menu (botlc_go / go_HSBattle cho Hoa Son cap 10)
	// (20/08) NGUONG b.nRaBaiThu nay tinh bang GIAY (xem pha 2 ben duoi): 300 =
	// 5 phut di bo trong thanh truoc khi phai dung duong teleport du phong.
	// Moi nga hong deu cong don b.nRaBaiThu; qua nguong (hoac goi script 5 lan
	// khong sang map) thi roi xuong khoi FALLBACK teleport neo-da-kiem ben duoi.
	if (b.nRaBaiThu <= 300 && b.nRaBaiGoi <= 5)
	{
		if (pb_LaThanhThi(SubWorld[nSub].m_SubWorldID))
		{
			// --- pha 2: di bo toi Xa Phu roi "len xe" ---
			int nBx3 = 0, nBy3 = 0;
			Npc[nNpcIdx].GetMpsPos(&nBx3, &nBy3);
			const int nXp = pb_TimNpcNho(nSub, PB_NPC_XP, "xaphu", nBx3, nBy3);
			if (nXp <= 0)
			{
				// (phan bien 20/08) PHAI qua cong giay: cong 20 MOI NHIP thi chi 15
				// nhip (0,8 giay) la vuot tran 300 - ca thanh se roi xuong duong
				// teleport, dung canh "bay" chu game da cam. Nay 20 diem/giay = 15
				// giay tim NPC Xa Phu truoc khi phai dung duong du phong.
				if (now % (unsigned int)GAME_FPS == 0)
					b.nRaBaiThu += 20;
				return 0;
			}
			int nPx = 0, nPy = 0;
			Npc[nXp].GetMpsPos(&nPx, &nPy);
			const __int64 nDdx = (__int64)nBx3 - nPx;
			const __int64 nDdy = (__int64)nBy3 - nPy;
			if (nDdx * nDdx + nDdy * nDdy
			    > (__int64)PB_FAC_ARRIVE_MPS * PB_FAC_ARRIVE_MPS)
			{
				// (20/08) DEM THEO GIAY, KHONG THEO NHIP. Ban cu cong 1 MOI NHIP
				// nen tran 700 chi la 39 GIAY di bo; trong thanh dong (Thanh Do
				// toan duong cheo hep, ca nghin bot chen nhau) di toi NPC Xa Phu
				// thuong lau hon the - do that 20/08: TruongAnh52 dat thu=701 roi
				// bi day sang duong TELEPORT, dung thu chu game da bo. Nay 1 diem
				// = 1 giay, A* thua thi phat them 4 giay.
				const int nWx = PB_WalkTo(nNpcIdx, nPx, nPy, nSub, b.walk,
				                          PB_FAC_ARRIVE_MPS);
				// (phan bien 20/08) THUONG PHAT A*-THUA PHAI NAM NGOAI CONG GIAY:
				// nhot ca hai vao "now % GAME_FPS == 0" thi phat chi trung 1/18 lan
				// -> duong "don nhanh" chet han, bot khong co duong toi Xa Phu se
				// dung tron 300 giay giua thanh. Nay: khong co duong = +1 MOI NHIP
				// (~17 giay la chuyen sang duong du phong), di duoc = 1 diem/giay.
				if (nWx < 0)
					b.nRaBaiThu++;
				else if (now % (unsigned int)GAME_FPS == 0)
					b.nRaBaiThu++;
				return 0;               // dang di bo
			}
			// dung canh Xa Phu: gion 3 giay giua hai lan "bam menu"
			if (now < b.nDoiMapTick)
				return 0;
			b.nDoiMapTick = now + (unsigned int)(GAME_FPS * 3);
			b.nRaBaiGoi++;
			if (pb_TrongNhom(nIdx))
				pb_RoiNhom(nIdx, "len xe di bai luyen");
			pb_Log("[BotXe] %s gap Xa Phu tai thanh %d -> di %s (map %d, chuyen thu %d)\n",
			       Player[nIdx].m_PlayerName, SubWorld[nSub].m_SubWorldID,
			       bai.szTen, bai.nMapId, b.nRaBaiGoi);
			// duong dan PHAI viet THUONG (g_FileName2Id bam phan biet hoa/thuong);
			// ten HAM giu nguyen hoa/thuong cua Lua (khong qua bo bam duong dan)
			if (b.nBaiIdx == 0)
				Player[nIdx].ExecuteScript((char*)"\\script\\global\\station.lua",
				                           (char*)"go_HSBattle", 0, false);
			else
				Player[nIdx].ExecuteScript((char*)"\\script\\global\\station.lua",
				                           (char*)"bot_bai_go", PB_BAI_LUA(b.nBaiIdx), false);
			b.nTargetNpc = 0;
			b.walk.Reset();  b.chase.Reset();  b.roam.Reset();
			b.nRoamX = 0;    b.nRoamY = 0;
			return 0;                   // nhip sau map khop -> nhanh dau tra 1
		}

		// --- pha 1: khong o thanh -> "phu ve thanh nha" (so le cua so 60 giay:
		// ca ngan con cung phu ve mot khung la nghen region sync) ---
		if (b.nDoiMapTick == 0)
			b.nDoiMapTick = now + (unsigned int)((nLech % 60) * GAME_FPS);
		if (now < b.nDoiMapTick)
			return 0;
		b.nDoiMapTick = now + (unsigned int)(GAME_FPS * 10);
		b.nRaBaiThu++;
		if (pb_TrongNhom(nIdx))
			pb_RoiNhom(nIdx, "phu ve thanh doi map luyen");
		const PB_DtNpc& nhaVe = pb_ThanhNha(b, nLech);
		pb_Log("[BotXe] %s phu ve thanh %d tim Xa Phu (muon di %s map %d)\n",
		       Player[nIdx].m_PlayerName, nhaVe.nMap, bai.szTen, bai.nMapId);
		pb_DtVeThanh(nIdx, nNpcIdx, b, nhaVe, nLech);
		return 0;
	}

	// ========= (21/08) KHONG GOI DUOC XA PHU -> DOI THANH KHAC, KHONG TELEPORT =========
	// Chu game: "bot khong goi duoc Xa Phu nen di chuyen qua thanh khac de goi
	// xa phu". Truoc day toi day la teleport thang toi bai ("bay tu che" da bi
	// cam) - do 09:33-09:44 van con 15 luot BO TAY, toan bot ket trong thanh
	// 37/80 khong toi duoc NPC. Nay: qua 300 giay di bo / 5 lan goi script
	// khong sang map -> lay THANH KE TIEP trong 10 thanh co Xa Phu (xoay vong
	// s_dtNpc, tinh tu thanh DANG DUNG), xoa dem, phu sang do bang dung duong
	// "phu ve thanh nha" (pb_DtVeThanh) roi di bo toi Xa Phu lai tu dau.
	// Khong con van cuoi nao khac. Van giu cua so so le 60 giay ben duoi de
	// ca dan khong ChangeWorld cung mot khung.

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

	// "Bai hong" (ban do chua mo tren may chu) truoc day phat hien nho ChangeWorld
	// that bai o day; nay hoi thang g_SubWorldSet.SearchWorld - ket qua nhu cu:
	// cam bai den het phien, lan chon toi bot lay bai khac.
	if (g_SubWorldSet.SearchWorld(bai.nMapId) < 0)
	{
		pb_Log("[BotBai] %s: KHONG VAO DUOC %s (map %d) - ban do co the CHUA MO tren may chu."
		       " CAM bai nay den het phien.\n", Player[nIdx].m_PlayerName, bai.szTen, bai.nMapId);
		if (b.nBaiIdx >= 0 && b.nBaiIdx < PB_SO_BAI)
			s_baiHong[b.nBaiIdx] = 1;
		b.nBaiIdx = -1;
		return 1;
	}
	{
		// thanh ke tiep tinh tu thanh DANG DUNG (neu dang o mot trong 10 thanh),
		// khong thi tu thanh nha hien tai - de chac chan doi sang thanh KHAC.
		int k = pb_DtIdxThanh(SubWorld[nSub].m_SubWorldID);
		if (k < 0)
			k = pb_DtIdxThanh(pb_ThanhNha(b, nLech).nMap);
		if (k < 0)
			k = nLech % 10;
		const PB_DtNpc& nhaMoi = s_dtNpc[(k + 1) % 10];
		pb_Log("[BotXe] %s KHONG goi duoc Xa Phu o map %d (thu=%d, goi=%d)"
		       " -> doi thanh nha sang %d, phu qua do goi lai (muon di %s map %d)\n",
		       Player[nIdx].m_PlayerName, SubWorld[nSub].m_SubWorldID,
		       b.nRaBaiThu, b.nRaBaiGoi, nhaMoi.nMap, bai.szTen, bai.nMapId);
		b.nThanhNhaMap = nhaMoi.nMap;
		b.nRaBaiThu = 0;  b.nRaBaiGoi = 0;  b.nDoiMapTick = 0;
		b.nTargetNpc = 0;
		b.walk.Reset();  b.chase.Reset();  b.roam.Reset();
		b.nRoamX = 0;    b.nRoamY = 0;
		if (pb_TrongNhom(nIdx))
			pb_RoiNhom(nIdx, "doi thanh tim Xa Phu");
		pb_DtVeThanh(nIdx, nNpcIdx, b, nhaMoi, nLech);
		return 0;
	}
}

// ===========================================================================
// DI HOANG TIM QUAI (chong bot gom mot cho)
//
// Bot don cuc mot cho la chuyen chac chan xay ra: ca dam cung ChangeWorld toi CUNG MOT
// diem cua bai luyen, danh sach quai quanh do, roi dung im vi khong con gi de danh.
//
// (20/08 - DA DOI NGUON DICH) Dich KHONG con la vi tri quai dang song nua ma la
// DIEM SINH cua quai (KNpc::m_OriginX/Y, tuc toa do ghi trong tep Npc_S.dat cua
// ban do), gom cum san trong pb_LayCum. Chu thich cu o day ("lay toa do quai
// DANG SONG con dung hon doc tep") la sai o dung cho quan trong: quai lang thang
// quanh diem goc moi nhip AI, nen dich boc duoc thuong la mot con vua di lac -
// bot chay theo trong nhin nhu "di chuyen bay", va hai bot boc hai con quai canh
// nhau thi van don ve cung mot cho. Diem sinh thi co dinh va rai theo dung thiet
// ke bai quai cua nha lam game. Nhanh quet quai dang song van giu lam DU PHONG
// cho ban do chua co bang cum.
//
// PHAI di bang A* (PB_WalkTo): phat thang mot lenh do_run di xa la ket goc ngay - moi
// lenh chi duoc DUNG MOT lan ne vat can (KNpcFindPath.cpp:106-111), vat can thu hai la
// dung im vinh vien. Dung PB_WalkState RIENG (roam) de khong xo lech lo trinh vao phai.
// ===========================================================================
#define PB_ROAM_NEAR      1200     // "gan" = trong chung nay MPS thi khong tinh la diem moi
#define PB_ROAM_MAX_CAND  256      // so ung vien gom toi da (64 cu chan CA vong quet
                                   // region nen moi bot chi thay quai o cac region chi
                                   // so THAP - ca tram con boc chung mot ro; phan bien
                                   // 20/08). Chi con dung cho nhanh du phong.
#define PB_ROAM_RETRY     (GAME_FPS * 5)   // giai cho giua hai lan boc diem

// ===========================================================================
// (20/08 - chu game: "gom 1 cho trong map thi phai kiem tra xung quanh nhieu
// hon 20 bot thi bot khac se tu di chuyen cach xa THEO TOA DO NPC DA ADD trong
// map do chu khong duoc di chuyen bay")
//
// BANG DIEM SINH QUAI THEO BAN DO.
//
// Nguon du lieu = KNpc::m_OriginX / m_OriginY (KNpc.h:402) - toa do ghi trong
// tep Npc_S.dat cua tung region, dat DUNG MOT LAN luc tao NPC (KNpcSet.cpp:515
// AddNpcSet1) va la chinh toa do engine dung de HOI SINH quai (KNpc::Revive,
// KNpc.cpp:8392) va de keo quai ve khi no di lac (KNpcAI::KeepActiveRange,
// KNpcAI.cpp:1182). Dung nghia "toa do npc da add trong map".
//
// KHAC HAN vi tri HIEN TAI (GetMpsPos) ma ban cu dung lam dich: quai di lang
// thang quanh diem goc moi nhip AI (KNpcAI::CommonAction), nen dich boc duoc co
// the la mot con vua di lac ra sat dam dong khac, hoac ra vung rong - dung canh
// "di chuyen bay". Diem sinh thi co dinh, do nha lam game dat, va rai deu theo
// dung thiet ke bai quai.
//
// GOM CUM: duyet MOI npc dich cua ban do - ca m_Region[].m_NpcList LAN
// m_NoneRegionNpcList (quai DANG CHET nam o danh sach thu hai, diem sinh cua no
// van con nguyen ven, bo qua la mat ca nhung bai dang bi farm sach). Nhan mot
// diem sinh lam TAM CUM moi neu no cach MOI tam da nhan >= PB_CUM_CACH. Tam cum
// vi vay LUON la mot toa do co that trong du lieu ban do.
//
// Tinh MOT LAN cho moi ban do roi giu ca doi server: diem sinh khong bao gio
// doi (chi ChangeWorld moi ghi de m_OriginX, ma quai thi khong doi ban do).
// ===========================================================================
#define PB_CUM_MAX    192          // so cum giu moi ban do
#define PB_CUM_CACH   1280         // 40 o - hai cum phai cach nhau it nhat chung nay
#define PB_DAN_XA_MIN 2400         // DI TAN phai di xa it nhat 75 o ("cach xa")

struct PB_CumMap
{
	int nSo;
	int aX[PB_CUM_MAX], aY[PB_CUM_MAX];
	int aDem[PB_CUM_MAX];          // so quai thuoc cum (do "dam quai" cua diem)
};
// (phan bien 20/08) MANG TINH, KHONG cap phat dong: "new" cua MSVC NEM bad_alloc
// chu khong tra NULL, nen nhanh "if (!p)" la ma chet va het bo nho se lam SAP
// GameServer thay vi lui ve nhanh du phong. Mang tinh: 1000 map x 2308 byte =
// ~2,3 MB BSS, khong cap phat, khong ro, khong con tro treo.
static PB_CumMap     s_cum[MAX_SUBWORLD];
static unsigned char s_cumTinh[MAX_SUBWORLD] = { 0 };   // 0 = chua co bang, 1 = co
static unsigned int  s_cumMoc[MAX_SUBWORLD]  = { 0 };   // moc nhip lan tinh gan nhat
// Lam moi 30 phut: quai them bang Lua (boss theo gio / su kien / quai nhiem vu)
// sinh SAU lan quet dau se khong co trong bang neu giu vinh vien.
#define PB_CUM_LAI   (GAME_FPS * 60 * 30)

// Gan mot diem sinh vao cum (tra chi so cum gan nhat trong PB_CUM_CACH, -1 = khong)
static int pb_CumGan(const PB_CumMap* p, int x, int y)
{
	for (int i = 0; i < p->nSo; i++)
	{
		int dx = p->aX[i] - x;  if (dx < 0) dx = -dx;
		int dy = p->aY[i] - y;  if (dy < 0) dy = -dy;
		if (dx <= PB_CUM_CACH && dy <= PB_CUM_CACH)
			return i;
	}
	return -1;
}

static void pb_NapMotNpc(int nSub, PB_CumMap* p, int i, int nRefNpc)
{
	if (i <= 0 || i >= MAX_NPC || i == nRefNpc)  return;
	if (Npc[i].m_dwID == 0)                      return;
	// (phan bien 20/08) TUYET DOI KHONG loc bang NpcSet.GetRelation o day. Bang
	// cum la bang DUNG CHUNG cho ca ban do, ma GenOneRelation (KNpcSet.cpp:126)
	// xet camp TRUOC kind: camp_begin -> ally, cung camp -> ally. Bot duoc rai
	// deu 10 mon phai nen co ca chinh lan ta; neu con hoi dau tien la bot TA
	// PHAI tren mot bai quai ta phai (hoac bot chua vao phai = camp_begin) thi
	// MOI con quai bi coi la dong minh -> bang rong -> ca ban do mat co che tan
	// dan vinh vien. Loc bang chinh THUOC TINH cua NPC: NPCKIND kind_normal = 0
	// la "quai binh thuong" (GameDataDef.h:1369), khong phu thuoc ai dang hoi.
	if (Npc[i].m_Kind != kind_normal)            return;
	const int ox = Npc[i].m_OriginX, oy = Npc[i].m_OriginY;
	if (ox <= 0 || oy <= 0)                   return;
	const int k = pb_CumGan(p, ox, oy);
	if (k >= 0)
	{
		p->aDem[k]++;
		return;
	}
	if (p->nSo >= PB_CUM_MAX)
		return;
	// Tam cum PHAI dung duoc: mot so diem sinh nam sat vach / trong o luoi chan.
	// Nhich quanh do toi 4 o de lay o hop le gan nhat; khong duoc thi bo cum.
	int cx = 0, cy = 0;
	if (!pb_ODat(nSub, ox / 32, oy / 32, p->nSo, 4, &cx, &cy))
		return;
	p->aX[p->nSo]   = cx;
	p->aY[p->nSo]   = cy;
	p->aDem[p->nSo] = 1;
	p->nSo++;
}

static PB_CumMap* pb_LayCum(int nSub, int nRefNpc)
{
	if (nSub < 0 || nSub >= MAX_SUBWORLD || nRefNpc <= 0 || nRefNpc >= MAX_NPC)
		return NULL;
	const unsigned int now = SubWorld[nSub].m_dwCurrentTime;
	if (s_cumTinh[nSub] && now - s_cumMoc[nSub] < (unsigned int)PB_CUM_LAI)
		return &s_cum[nSub];       // bang con tuoi
	const int nTong = SubWorld[nSub].m_nTotalRegion;
	if (nTong <= 0)                // ban do chua nap region
		return s_cumTinh[nSub] ? &s_cum[nSub] : NULL;
	PB_CumMap* p = &s_cum[nSub];
	p->nSo = 0;
	// (1) quai dang song trong cac region
	for (int r = 0; r < nTong; r++)
	{
		KIndexNode* pNode = (KIndexNode*)SubWorld[nSub].m_Region[r].m_NpcList.GetHead();
		while (pNode)
		{
			const int i = pNode->m_nIndex;
			pNode = (KIndexNode*)pNode->GetNext();
			pb_NapMotNpc(nSub, p, i, nRefNpc);
		}
	}
	// (2) quai DANG CHET cho hoi sinh (KNpc::DoRevive day sang VOID_REGION)
	{
		KIndexNode* pNode = (KIndexNode*)SubWorld[nSub].m_NoneRegionNpcList.GetHead();
		while (pNode)
		{
			const int i = pNode->m_nIndex;
			pNode = (KIndexNode*)pNode->GetNext();
			pb_NapMotNpc(nSub, p, i, nRefNpc);
		}
	}
	if (p->nSo <= 0)
	{
		// (phan bien 20/08) KHONG duoc chot "ban do nay khong co quai" o day. Bang
		// rong co the chi la nhat thoi (region chua nap xong, dan bot vua quet sach);
		// chot lai la giet co che tan dan cua ca ban do cho toi luc restart. De
		// s_cumTinh nguyen 0 -> bot ke tiep thu tinh lai, trong luc do chay du phong.
		// Gion log 60 giay/map de khong spam khi ban do that su khong co quai.
		if (now - s_cumMoc[nSub] >= (unsigned int)(GAME_FPS * 60) || s_cumMoc[nSub] == 0)
		{
			s_cumMoc[nSub] = now;
			pb_Log("[BotCum] map %d: chua thay diem sinh quai nao -> tam dung duong cu\n",
			       SubWorld[nSub].m_SubWorldID);
		}
		return NULL;
	}
	int nDamNhat = 0;
	for (int i = 1; i < p->nSo; i++)
		if (p->aDem[i] > p->aDem[nDamNhat]) nDamNhat = i;
	s_cumTinh[nSub] = 1;
	s_cumMoc[nSub]  = now;
	pb_Log("[BotCum] map %d: %d cum diem sinh quai; cum dong nhat %d quai o(%d,%d)\n",
	       SubWorld[nSub].m_SubWorldID, p->nSo, p->aDem[nDamNhat],
	       p->aX[nDamNhat] / 32, p->aY[nDamNhat] / 32);
	return p;
}

// (phan bien 20/08) RAI QUANH TAM CUM theo chi so bot.
// Neu tra ve DUNG tam cum thi moi bot chon cung mot cum se chay toi DUNG MOT
// toa do MPS: bai chi co vai cum nam trong day xich, nen ~130 bot cua mot bai
// chi don thanh vai dong thay vi mot dong - van la "gom mot cho". Luoi 13x13 o
// quanh tam = 169 cho dung rieng cho moi cum, du cho ca ngan bot.
// Diem le PHAI qua pb_ODat de chac chan dung duoc; hong thi giu nguyen tam cum.
static void pb_RaiQuanhCum(int nSub, int nLech, int* pnX, int* pnY)
{
	const int nDx = (nLech % 13) - 6;
	const int nDy = ((nLech / 13) % 13) - 6;
	int rx = 0, ry = 0;
	if (pb_ODat(nSub, *pnX / 32 + nDx, *pnY / 32 + nDy, nLech, 6, &rx, &ry))
	{
		*pnX = rx;
		*pnY = ry;
	}
}

// (20/08 dem) x,y co nam trong "san choi" khong: cach MOT tam cum diem sinh
// (pb_LayCum) toi da nTam theo tung truc - cung phep do tung-truc voi khoi gan
// diem sinh vao cum o tren. Tam cum la m_OriginX/Y cua du lieu nen luon nam
// trong map that; quai lang thang ra vung trong gia se rot khoi luoi nay.
static int pb_GanCum(const PB_CumMap* pC, int x, int y, int nTam)
{
	if (!pC)
		return 0;
	for (int c = 0; c < pC->nSo; c++)
	{
		int dx = x - pC->aX[c]; if (dx < 0) dx = -dx;
		int dy = y - pC->aY[c]; if (dy < 0) dy = -dy;
		if (dx <= nTam && dy <= nTam)
			return 1;
	}
	return 0;
}

// Tim mot con quai XA de lam dich di hoang. Tra 1 va dien toa do, 0 = khong tim duoc.
// bUuTienVang = 1: chon ung vien IT BOT NHAT (dung cho DI TAN gian dan - chu game
// 19/08 toi: "bot van tap trung 1 cho rat dong": boc ngau nhien hay trung diem
// ngay canh dam dong, di tan xong van dong nguyen cho).
static int pb_FindRoamSpot(int nIdx, int nNpcIdx, int nSub, const PB_Bot& b,
                           int nLech, int bUuTienVang, int* pnX, int* pnY)
{
	if (nSub < 0 || nSub >= MAX_SUBWORLD)
		return 0;
	const int nTong = SubWorld[nSub].m_nTotalRegion;
	if (nTong <= 0)
		return 0;

	int bx = 0, by = 0;
	Npc[nNpcIdx].GetMpsPos(&bx, &by);

	// (19/08 toi #3) day xich bai - nhu pb_FindTarget: khong roam toi quai dung
	// ngoai xich (chinh la duong bot "ra ngoai map" tren map 79: [BotHoang] 27
	// luot nham o 1686,3140 giua vung trong gia du lieu bao di duoc).
	int nNeoX = 0, nNeoY = 0;
	const int bNeo = pb_NeoBai(b, nSub, &nNeoX, &nNeoY);
	const int nXich = (bNeo == 2) ? PB_XICH_NV : PB_XICH_BAI;

	// (20/08) DI TAN thi phai di THAT XA ("cach xa" - loi chu game), khong chi
	// nhinh hon tam danh: 1200 MPS = 37 o van nam gon trong cung mot dam.
	const int nGanMin = bUuTienVang ? PB_DAN_XA_MIN : PB_ROAM_NEAR;

	int aX[PB_ROAM_MAX_CAND], aY[PB_ROAM_MAX_CAND];
	int nCand = 0;

	// ---- DUONG CHINH (20/08): ung vien = DIEM SINH NPC da add trong ban do ----
	// Bang cum tinh mot lan cho ca ban do (pb_LayCum). Toa do la m_OriginX/Y tu
	// tep du lieu, khong phai cho quai vua di lac toi, nen bot khong "di bay".
	// (20/08 dem) pC dua ra ngoai khoi de nhanh DU PHONG ben duoi cung loc duoc
	// theo cum khi bang cum co ma moi ung vien chinh deu bi loai (qua gan).
	const PB_CumMap* pC = pb_LayCum(nSub, nNpcIdx);
	{
		if (pC)
		{
			// (phan bien 20/08) NOI LONG DAN moc "phai xa" thay vi rot ngay ve
			// nhanh du phong: nCand == 0 rat hay xay ra chi vi moi cum trong day
			// xich deu gan hon nGanMin (bot dung giua bai), ma nhanh du phong lai
			// bam QUAI DANG SONG - dung hanh vi "di chuyen bay" dot nay dinh bo.
			for (int nNoi = 0; nNoi < 3 && nCand <= 0; nNoi++)
			{
				const int nGan = nGanMin >> nNoi;      // nGanMin, /2, /4
				for (int c = 0; c < pC->nSo && nCand < PB_ROAM_MAX_CAND; c++)
				{
					const int ex = pC->aX[c], ey = pC->aY[c];
					if (g_GetDistance(bx, by, ex, ey) <= nGan)
						continue;
					// (20/08 dem - chu game dinh chinh) tam cum LA toa do NPC da
					// add trong map -> hop le tren TOAN ban do, KHONG loc theo day
					// xich neo nua: "neu o dam dong tren 20 bot thi tim toa do npc
					// duoc add trong map do hop le xa de di chuyen". Chinh phep loc
					// xich cho nay tung ep bot gom quanh neo lam [BotDan] het cho tan.
					if (SubWorld[nSub].CellObsSrv(ex, ey) == 1)
						continue;                 // o bi luoi chan (hiem, van kiem)
					aX[nCand] = ex;
					aY[nCand] = ey;
					nCand++;
				}
			}
		}
	}

	// ---- DU PHONG: ban do chua co bang cum (chua nap region / khong co quai),
	// hoac moi diem sinh deu bi loc het -> quay ve quet quai DANG SONG nhu cu.
	const int bDuPhong = (nCand <= 0);
	for (int r = 0; bDuPhong && r < nTong && nCand < PB_ROAM_MAX_CAND; r++)
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
			if (g_GetDistance(bx, by, ex, ey) <= nGanMin)
				continue;
			if (pC ? !pb_GanCum(pC, ex, ey, PB_XICH_CUM)
			       : (bNeo && g_GetDistance(nNeoX, nNeoY, ex, ey) > nXich))
				continue;              // ngoai san choi -> khong roam toi
			// (20/08) quai dung tren o LUOI CHAN (vung rong ngoai map / vach nui):
			// A* khong bao gio toi duoc no. Ban cu van boc lam dich roi in
			// "[BotHoang] khong tim duoc duong ... boc cho khac" - do that 20/08:
			// 277/838 luot di hoang chet o cua nay, moi luot ton mot lan quet A*
			// toan map VA treo bot PB_ROAM_RETRY = 5 giay cho lan boc sau.
			if (SubWorld[nSub].CellObsSrv(ex, ey) == 1)
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
		if (bUuTienVang || (pb_TrongNhom(nIdx) && pb_DoiTruongCua(nIdx) == nIdx))
		{
			int nTot = -1;
			int nItNhat = 0x7fffffff;
			__int64 nXaNhat = -1;
			// (phan bien 20/08) TRAN 32 UNG VIEN cho vong nay. No la O(nCand x
			// s_botCount) kem GetMpsPos tung cap; voi nCand ~192 va 1000 bot la
			// 192.000 vong MOI LAN goi, ma moi doi truong goi lai mot lan/5 giay
			// -> vai trieu phep/giay, dung kieu lag da phai chua 18/08. Diem bat
			// xoay theo chi so bot nen 32 con dang xet cua moi bot van khac nhau.
			const int nXetToiDa = (nCand > 32) ? 32 : nCand;
			const int nBatC     = ((int)g_Random(nCand) + nLech * 5) % nCand;
			for (int cc = 0; cc < nXetToiDa; cc++)
			{
				const int c = (nBatC + cc) % nCand;
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
				pb_RaiQuanhCum(nSub, nLech, pnX, pnY);
				return 1;
			}
		}
	}

	// Tron them chi so bot: g_Random goi lien tiep trong CUNG MOT khung hay ra giong nhau
	// (canh bao da ghi san o KSimCity.cpp:1326-1328), khong tron thi ca dam keo ve mot cho.
	const int k = ((int)g_Random(nCand) + nLech * 11) % nCand;
	*pnX = aX[k];
	*pnY = aY[k];
	pb_RaiQuanhCum(nSub, nLech, pnX, pnY);
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
		if (!pb_FindRoamSpot(nIdx, nNpcIdx, nSub, b, nLech, 0, &rx, &ry))
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

	// (23/08) TRONG TONG KIM: muc tieu dang giu ma bi TUONG chan giua (mat duong
	// nhin - thuong do mot ben vua chay doc theo vach) -> bo + cam NGAY, khong
	// doi du 10 giay cua dong ho sut mau; de cang lau hai dan cang don dong ve
	// hai ben vach. Gion ~0,5 giay moi lan kiem cho khoi ton 20+ pb_ODuoc/khung.
	if (t && b.nTk >= 4 && SubWorld[nSub].m_SubWorldID == PB_TK_MAP)
	{
		const int nLechF = (int)(&b - s_bots);
		if (((now + (unsigned int)nLechF) % 9u) == 0)
		{
			int bxF = 0, byF = 0, exF = 0, eyF = 0;
			Npc[nNpcIdx].GetMpsPos(&bxF, &byF);
			Npc[t].GetMpsPos(&exF, &eyF);
			if (!pb_ThayDuoc(nSub, bxF, byF, exF, eyF))
			{
				pb_CamMucTieu(b, t, now);
				t = 0;
			}
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
		// (19/08 toi #3) day xich bai: muc tieu (ke ca con dang danh do) lang thang
		// ra ngoai nga tha thi cam roi buong - khong duoi theo quai keo minh vao
		// "vung trong gia" ngoai me cung (map 79). Nga tha rong hon nga chon
		// 320 MPS de khong nha oan con dang danh sat mep xich.
		// (20/08 dem) co bang cum -> nga tha do theo TAM CUM (PB_XICH_CUM_THA);
		// xich neo chi con la duong lui. Xem chu thich PB_XICH_CUM.
		{
			int nNeoX4 = 0, nNeoY4 = 0;
			const int nLoaiNeo = pb_NeoBai(b, nSub, &nNeoX4, &nNeoY4);
			// (21/08) bo gate nLoaiNeo cho nhanh cum - rao theo ban do dang dung
			// (xem chu thich cung ngay o pb_FindTarget).
			// (21/08 phan bien) trong Tong Kim KHONG rao cum - xem chu thich cung
			// ngay o pb_FindTarget. Thieu cho nay thi bot vua bat duoc dich la
			// pb_CamMucTieu cam luon con do roi buong.
			const int bTK4 = (b.nTk >= 4 && SubWorld[nSub].m_SubWorldID == PB_TK_MAP);
			const PB_CumMap* pCum4 = bTK4 ? NULL : pb_LayCum(nSub, nNpcIdx);
			if (!bTK4
			 && (pCum4 ? !pb_GanCum(pCum4, tx2, ty2, PB_XICH_CUM_THA)
			           : (nLoaiNeo
			            && g_GetDistance(nNeoX4, nNeoY4, tx2, ty2)
			              > ((nLoaiNeo == 2) ? PB_XICH_NV_THA : PB_XICH_BAI_THA))))
			{
				static unsigned int s_uXichLog = 0;
				if (now - s_uXichLog >= (unsigned int)(GAME_FPS * 2))
				{
					s_uXichLog = now;
					pb_Log("[BotXich] %s muc tieu %d o(%d,%d) ngoai san choi cum -> buong\n",
					       Player[nIdx].m_PlayerName, t, tx2 / 32, ty2 / 32);
				}
				pb_CamMucTieu(b, t, now);
				b.nTargetNpc = 0;
				b.chase.Reset();
				return 0;
			}
		}
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
					// (20/08) diem NHAM da kiem hop le o tren, nhung DOAN CHAY toi
					// no thi chua: cat theo luoi de khong lao qua vung rong.
					int nCx3 = 0, nCy3 = 0;
					Npc[nNpcIdx].GetMpsPos(&nCx3, &nCy3);
					int nRx3 = nAimThoX, nRy3 = nAimThoY;
					if (pb_CatDoan(nSub, nCx3, nCy3, &nRx3, &nRy3))
						Npc[nNpcIdx].SendCommand(do_run, nRx3, nRy3);
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
				if (pDo->CanCastSkill(nNpcIdx, nP1, nP2) == 0)
				{
					// [CAST-LECH 30/08] 83k cu tu choi/ngay = bot he kiem boc trung
					// "duong quyen" (tay khong) nhung van giu chieu kiem cu: chieu chi
					// duoc chon lai khi DOI CAP, nhanh phat-lai vu khi boc trung NONE
					// cung khong reset -> ket vinh vien. Chieu LECH vu khi dang cam
					// (quy uoc eqt y het pb_PickSkill) -> ep chon lai ngay; tu choi vi
					// ly do khac (mana, the cuoi...) giu nguyen.
					const int eqC = pDo->GetEquipLimit();
					int nWantC = -1;
					{
						const int nWpnC = Player[nIdx].m_ItemList.GetEquipment(itempart_weapon);
						if (nWpnC > 0)
						{
							const int nDetC = Item[nWpnC].GetDetailType();
							const int nParC = Item[nWpnC].GetParticular();
							nWantC = (nParC == HAND_PARTICULAR) ? -1
							       : ((nDetC == 1) ? (nParC + 100) : nParC);
						}
					}
					if (eqC != -2 && eqC != nWantC)
					{
						pb_Log("[BotCast] %s chieu %d LECH vu khi (eqt=%d want=%d)"
						       " -> chon lai chieu theo vu khi hien tai\n",
						       Player[nIdx].m_PlayerName, b.nAtkSkill, eqC, nWantC);
						b.nAtkSkill = 0;
					}
					else if (now - b.nUongTick >= (unsigned int)(GAME_FPS * 5))
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
		}
		if (b.nAtkSkill > 0)   // [CAST-LECH] vua bi ep chon lai -> nhip sau phat chieu moi
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
	// (19/08 toi #4 - chu game: "item bot post len kenh the gioi phai la do XANH
	// co nMagicLevel tu 1-5") - truoc day mang de trong => mon TRANG khong opt.
	// Rai cap phu 1..5 y khuon sap (Gen_MagicAttrib doc toi phan tu 0 dau tien).
	// (phan bien dot 2) g_Random dong bang theo giay + cung nMax cung gia tri:
	// khong tron bien dem thi series == nCapTG - 1 MAI MAI va so opt bimodal
	// {1,6}. s_nLinkDem tang moi lan goi de moi link mot bo mat khac.
	static int s_nLinkDem = 0;
	s_nLinkDem++;
	const int nCapTG = 1 + (((int)g_Random(5) + s_nLinkDem * 3) % 5);   // 1..5
	for (int o3 = 0; o3 < 6; o3++)
	{
		if (o3 && ((int)g_Random(97) + o3 * 31 + s_nLinkDem * 7) % 3 == 0)
			break;
		nMagicTG[o3] = nCapTG;
	}
	const int nDo = ItemSet.Add(0, 0, ((int)g_Random(5) + s_nLinkDem * 2) % 5,
							  1 + (((int)g_Random(8) + s_nLinkDem * 5) % 8), 0,
							  s_aMauDo[nMau][0], s_aMauDo[nMau][1], nMagicTG,
							  g_SubWorldSet.GetGameVersion(), 0);
	if (nDo <= 0 || nDo >= MAX_ITEM)
		return;
	// tu kiem mau: Gen truot to hop (CMIT thieu) ra mon TRANG thi bo link nay
	// (cau rao van gui, chi khong dinh kem do) - khong post do trang len kenh.
	if (Item[nDo].GetColorItem() != green_item)
	{
		ItemSet.Remove(nDo);
		return;
	}

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
// Mot nhip "o thanh" cua che do VE THANH (s_nPbVeThanh): sai map dich -> teleport
// (so le 60s, roi nhom truoc); dung map -> cuoi ngua (10 map thanh thi tu nhan
// qua pb_CuoiNguaTrongThanh, thon di bo) + DI DAO ngau nhien quanh tam <= 18 o
// (chon diem quanh TAM chu khong quanh vi tri hien tai - khoi troi dan ra ria).
static void pb_VeThanh(int nIdx, int nNpcIdx, int nSub, PB_Bot& b, int nLech)
{
	const unsigned int now = SubWorld[nSub].m_dwCurrentTime;
	const PB_VeThanh& vt = s_veThanh[((unsigned)nLech) % (unsigned)PB_SO_VETHANH];

	if (SubWorld[nSub].m_SubWorldID != vt.nMap)
	{
		if (b.nVeThanhHen == 0)
		{
			// cua so so le 60 giay nhu pb_RaBai - 1000 con ChangeWorld cung khung
			// la nghen region sync ("dung thanh rat nhieu" 18/08)
			b.nVeThanhHen = now + (unsigned int)((nLech % 60) * GAME_FPS);
			return;
		}
		if (now < b.nVeThanhHen)
			return;
		b.nVeThanhHen = now + (unsigned int)(GAME_FPS * 10);   // lan thu sau neu truot
		if (pb_TrongNhom(nIdx))
			pb_RoiNhom(nIdx, "ve thanh thi/thon");
		const int nSubT = g_SubWorldSet.SearchWorld(vt.nMap);
		const int nDx = ((nLech % 9) - 4) * 2;
		const int nDy = (((nLech / 9) % 7) - 3) * 3;
		int nRet = 0;
		if (nSubT >= 0)
		{
			int nVx = 0, nVy = 0;
			if (pb_ODat(nSubT, vt.nOX + nDx, vt.nOY + nDy, nLech, 10, &nVx, &nVy))
				nRet = Npc[nNpcIdx].ChangeWorld(vt.nMap, nVx, nVy);
			if (nRet != 1 && pb_ODat(nSubT, vt.nOX, vt.nOY, nLech, 16, &nVx, &nVy))
				nRet = Npc[nNpcIdx].ChangeWorld(vt.nMap, nVx, nVy);
		}
		if (nRet != 1)
			nRet = Npc[nNpcIdx].ChangeWorld(vt.nMap, vt.nOX * 32, vt.nOY * 32);
		if (nRet == 1)
		{
			b.nTargetNpc = 0;
			b.walk.Reset();  b.chase.Reset();  b.roam.Reset();
			b.nRoamX = 0;  b.nRoamY = 0;  b.nRoamTick = 0;
			b.nBuocRaX = 0;  b.nBuocRaY = 0;
			// (19/08 dem - chu game) "goi bot ve thanh chia deu cac map thi phai
			// tu dong LUU RUONG o map do luon de sau phu ve thi se ve dung thanh
			// da tung bi goi ve": dat diem hoi sinh (nhu bam ruong / Tho Dia Phu)
			// + ghi thanh nha DONG (chi khi map co Xa Phu - 10 thanh s_dtNpc;
			// 5 thon van luu ruong de chet hoi sinh tai cho nhung "phu ve" thi
			// dung thanh nha cu vi chua chac thon co Xa Phu).
			Player[nIdx].SetRevivalPos(vt.nMap, vt.nRevId);
			// SetRevivalPos chi ghi LOGIN-pos; CHET hoi sinh doc DEATH-pos
			// (KPlayer::Revive REMOTE :6706) ma death chi duoc chep tu login luc
			// NAP nhan vat (:1376) - tu chep de hieu luc NGAY, khong doi restart.
			{
				POINT sPosRv;
				if (g_SubWorldSet.GetRevivalPosFromId(vt.nMap, vt.nRevId, &sPosRv))
				{
					PLAYER_REVIVAL_POS* pDrv = Player[nIdx].GetDeathRevivalPos();
					pDrv->m_nSubWorldID = vt.nMap;
					pDrv->m_ReviveID    = vt.nRevId;
					pDrv->m_nMpsX       = sPosRv.x;
					pDrv->m_nMpsY       = sPosRv.y;
				}
			}
			if (pb_LaThanhThi(vt.nMap))
				b.nThanhNhaMap = vt.nMap;
			static unsigned int s_uVtLog = 0;
			if (now - s_uVtLog >= (unsigned int)(GAME_FPS / 3))
			{
				s_uVtLog = now;
				pb_Log("[BotVeThanh] %s ve map %d (o %d,%d) + luu ruong (rev %d)\n",
				       Player[nIdx].m_PlayerName, vt.nMap, vt.nOX + nDx, vt.nOY + nDy,
				       vt.nRevId);
			}
		}
		return;
	}

	pb_CuoiNguaTrongThanh(nIdx, nNpcIdx, nSub, b, now);
	if (b.nRoamX == 0 && b.nRoamY == 0)
	{
		if (b.nRoamTick && now - b.nRoamTick < (unsigned int)(GAME_FPS * (8 + (nLech % 13))))
			return;
		b.nRoamTick = now;
		for (int t = 0; t < 6; t++)
		{
			// g_Random DONG BANG THEO GIAY (bay engine.lib): hai lan goi cung khung
			// tra cung so -> cx-lech == cy-lech la ca dan di cheo 45 do + cac bot
			// cung giay ra CUNG diem (dong cuc). Tron nLech + t voi he so nguyen
			// to khac nhau cho tung truc nhu khuon pb_ChamHangSap.
			const int cx = vt.nOX + ((int)g_Random(37) + nLech * 7 + t * 5) % 37 - 18;
			const int cy = vt.nOY + ((int)g_Random(41) + nLech * 13 + t * 11) % 37 - 18;
			if (!pb_ODuoc(nSub, cx * 32, cy * 32))
				continue;
			b.nRoamX = cx * 32;
			b.nRoamY = cy * 32;
			b.roam.Reset();
			break;
		}
		if (b.nRoamX == 0)
			return;
	}
	if (PB_WalkTo(nNpcIdx, b.nRoamX, b.nRoamY, nSub, b.roam, 96) != 0)
	{
		b.nRoamX = 0;  b.nRoamY = 0;   // toi noi / tac duong -> lan sau boc diem khac
		b.roam.Reset();
	}
}


// ===========================================================================
// TONG KIM - BOT TU THAM GIA NHU NGUOI CHOI  (21/08, yeu cau chu game)
//
// TRIET LY: "lam sao nguoi choi vao tham gia tong kim nhu the nao thi bot se
// nhu vay, khong de nguoi choi biet do la bot". Nen KHONG mo phong lai luat
// Tong Kim trong C++ - bot goi CHINH cac ham Lua ma nguoi choi bam:
//   bao danh : mobinhtk.lua go_tong() / go_kim()   (qua song_signup/jin_signup)
//   ra tran  : task02.lua  tong_ratran() / kim_ratran()
//   len bai  : shenxingfu.lua gopos_step3lv80/90() - dung Than Hanh Phu
//   thoat som: bot_tongkim.lua bot_tk_thoat()      - tra lai trang thai cu
// Nho vay bot nhan DUNG camp, dung diem hoi sinh, dung cap bac quan ham, dung
// moc thoi gian, dung thong bao "X gia nhap phe Tong" nhu nguoi that.
//
// BA THU C++ PHAI TU LAM (duong cua nguoi choi CHET voi bot):
//   1. Bot MIEN TOAN BO trap (KNpc.cpp:10068-10079) ma duong chinh thong roi
//      hau doanh ra tran LA MOT CAI TRAP -> C++ tu goi tong_ratran/kim_ratran.
//   2. SetTimer/OnTimer cua Lua CHET voi bot (KPlayer::Active return som khi
//      m_nNetConnectIdx == -1, KPlayer.cpp:371-373) -> dong ho 90 giay day
//      nguoi ra chien tuyen khong ton tai -> C++ tu dem 90 giay.
//   3. Bot khong co client nen khong bam duoc menu -> C++ dat thang TaskTemp
//      TMP_INDEX_NPC roi goi go_tong/go_kim (dung y het luc nguoi choi bam).
// ===========================================================================

static int s_nPbTongKim  = 0;      // cong tac tinh nang (0 = tat)
static int s_nPbTkTran   = 0;      // tran so bot moi tran; 0 = KHONG GIOI HAN
static int s_nPbTkDangMo = 0;      // 1 = dang co tran (do o nhip)
static int s_nPbTkGoiLai = 0;      // co MOT LAN: nhip sau goi quan bo sung
static int s_nTkDemTong  = 0;      // quan so 2 phe, cap nhat 1 giay/lan o pb_TkNhip
static int s_nTkDemKim   = 0;
// (21/08 chay that) moc nhip da cho mot bot cua phe do bam bao danh. Khong co no
// thi ca dan cung bam trong mot nhip, ma MAX_PLAYER_CL = 1 chi cho MOT nguoi moi
// phe vuot len -> so con lai bi tu choi im lang va dot het lan thu.
static unsigned int s_uTkBamTick[3] = { 0, 0, 0 };

// (21/08 do lai) NGUOI CHOI CHI BI GIU TRONG TRAI 10 GIAY, khong phai 90.
// tongratrai.lua:21 cho ra khi GetRestTime() <= (TIME_IN_TRAI - TIME_DELAY_RA_TRAI)*18
// = (90 - 10)*18, ma dong ho bat dau tu 90*18 va giam theo GIO GAME -> qua duoc
// ngay khi troi 10 giay. TIME_IN_TRAI = 90 chi la THOI GIAN TOI DA duoc o trong
// trai (lib_tktc.lua:65-66). Cho 90 giay la bot dung im trong trai gan het pha
// dau tran - dung canh chu game thay.
#define PB_TK_CHO_RATRAN  (GAME_FPS * 12)   // 10 giay cua trap + 2 giay bien
#define PB_TK_DICH_HAN    (GAME_FPS * 45)   // toi da bam mot diem doanh trai dich
#define PB_TK_DICH_XA     3200              // 100 o - diem den phai XA cho dang dung
#define PB_TK_PHA_HAN     (GAME_FPS * 120)  // qua han mot pha -> go co, khoi ket vinh vien

// Tra ve mission Tong Kim NEU TRAN DANG MO (NULL = dong), kem subworld map 379.
// Khuon lay tu LuaIsMission (ScriptFuns.cpp:11602-11620): mission gan vao
// SubWorld cua map 379, khong phai toan cuc.
static KMission* pb_TkMission(int* pnSub)
{
	const int nSub = g_SubWorldSet.SearchWorld(PB_TK_MAP);
	if (nSub < 0 || nSub >= MAX_SUBWORLD)
		return NULL;
	if (pnSub)
		*pnSub = nSub;
	KMission M;
	M.SetMissionId(PB_TK_MISSION);
	return SubWorld[nSub].m_MissionArray.GetData(&M);
}

// Bot nay co du tu cach vao Tong Kim khong. Dung DUNG cac cua chan cua chinh
// script (mobinhtk.lua) de bot khong bao gio bi tu choi IM LANG - moi nhanh tu
// choi ben do deu "return" tran, C++ khong the biet that bai tu gia tri tra ve.
static int pb_TkDuTuCach(const PB_Bot& b)
{
	if (b.nBanSap)                                    return 0;  // chu game: bot ban sap KHONG bi goi
	if (b.nTk)                                        return 0;  // dang trong tran roi
	const int p = b.nPlayerIdx;
	if (p <= 0 || p >= MAX_PLAYER)                    return 0;
	if (Player[p].m_dwID != b.dwID)                   return 0;
	const int n = Player[p].m_nIndex;
	if (n <= 0 || n >= MAX_NPC || Npc[n].m_dwID == 0) return 0;
	if (Npc[n].m_Doing == do_death || Npc[n].m_Doing == do_revive) return 0;
	if (b.nAi != PB_AI_FIGHT)                        return 0;  // (21/08 phan bien)
	                        // chi lay bot DANG LUYEN CONG. Bot con dang tren duong
	                        // vao phai (PB_AI_GOTO_FACTION) hoac chua duoc bat che
	                        // do danh (IDLE / IN_FACTION) ma bi keo di thi viec do
	                        // bi nuot, va luc tra ve pb_TkGoCo se LANG LE bat che
	                        // do danh cho con bot chu game chua he bat.
	if (Npc[n].m_Level < PB_TK_CAP_MIN)               return 0;  // mobinhtk.lua:141
	if (b.nFaction < 0)                               return 0;  // mobinhtk.lua:62 GetFaction()==""
	if (Npc[n].m_CurrentCamp == 4)                    return 0;  // mobinhtk.lua:100
	return 1;
}

// GOI QUAN cho mot tran. Yeu cau chu game, dung nguyen van:
//   "phai set random toan bo bot vao tong kim phai deu giua cac phai (tru cac
//    bot dang ngoi bay sap ban se khong bi goi vao tong kim), con toan bo bot
//    khac thi se tu dong random deu chia deu 2 phe tong kim de tham gia"
//   "phai random bot chu khong keu vao theo thu tu"
static int pb_TkGoiQuan()
{
	int nSub = -1;
	KMission* pM = pb_TkMission(&nSub);
	if (!pM)
		return 0;

	// (1) gom ung vien theo TUNG MON PHAI de con rai deu 10 phai
	static int aPhai[10][PB_MAX_BOTS];
	int aSo[10];
	for (int f = 0; f < 10; f++)
		aSo[f] = 0;
	int nUng = 0;
	for (int i = 0; i < s_botCount; i++)
	{
		if (!pb_TkDuTuCach(s_bots[i]))
			continue;
		int f = s_bots[i].nFaction;
		if (f < 0 || f > 9)
			f = 0;
		if (aSo[f] < PB_MAX_BOTS)
			aPhai[f][aSo[f]++] = i;
		nUng++;
	}
	if (nUng <= 0)
	{
		pb_Log("[BotTK] tran mo nhung KHONG co bot nao du tu cach (can cap >= %d,"
		       " da vao phai, camp != 4, dang luyen cong, khong ban sap)\n", PB_TK_CAP_MIN);
		return 0;
	}

	// (2) XAO TRON tung phai (Fisher-Yates) - "khong keu vao theo thu tu".
	// g_Random DONG BANG THEO GIAY (bay engine.lib da ghi o pb_ChamHangSap): hai
	// lan goi trong CUNG khung tra cung so -> phai tron them chi so vong lap,
	// khong thi hoan vi thanh cong thuc va dan bot luon xep y het nhau moi tran.
	for (int f = 0; f < 10; f++)
		for (int k = aSo[f] - 1; k > 0; k--)
		{
			const int j = ((int)g_Random(k + 1) + k * 29 + f * 7) % (k + 1);
			const int t = aPhai[f][k];  aPhai[f][k] = aPhai[f][j];  aPhai[f][j] = t;
		}

	// (3) rut VONG TRON qua 10 phai -> so bot moi phai chenh nhau toi da 1 con
	int nTong = (int)pM->GetGroupPlayerCount(1);
	int nKim  = (int)pM->GetGroupPlayerCount(2);
	const int nTran = (s_nPbTkTran > 0) ? s_nPbTkTran : nUng;
	int aVet[10];
	for (int f = 0; f < 10; f++)
		aVet[f] = 0;
	// (21/08 phan bien) XAO TRON THU TU DUYET PHAI. Neu duyet 0..9 co dinh thi
	// phe thanh HAM THUAN TUY cua chi so mon phai: vong tron qua 10 phai (SO
	// CHAN) cong luat "vao phe dang it hon" khien phai chi so chan LUON vao Tong,
	// phai le LUON vao Kim, tran nao cung y het - trai voi "random deu".
	int aTt[10];
	for (int f = 0; f < 10; f++)
		aTt[f] = f;
	for (int f = 9; f > 0; f--)
	{
		const int j = ((int)g_Random(f + 1) + f * 17) % (f + 1);
		const int t = aTt[f];  aTt[f] = aTt[j];  aTt[j] = t;
	}
	int nDaGoi = 0;
	int bCon = 1;
	while (bCon && nDaGoi < nTran)
	{
		bCon = 0;
		for (int fi = 0; fi < 10 && nDaGoi < nTran; fi++)
		{
			const int f = aTt[fi];
			if (aVet[f] >= aSo[f])
				continue;
			bCon = 1;
			PB_Bot& b = s_bots[aPhai[f][aVet[f]++]];
			// (4) CHIA DEU 2 PHE: luon vao phe DANG IT HON. Bat buoc chu khong
			// phai lam dep: MAX_PLAYER_CL = 1 (lib_tktc.lua:20) nen boc phe ngau
			// nhien se bi mobinhtk.lua:130-136 tu choi IM LANG khoang mot nua so
			// lan, va bot bi tu choi thi dung im giua map 324 cho het tran.
			b.nTkPhe      = (nTong <= nKim) ? 1 : 2;
			if (b.nTkPhe == 1) nTong++; else nKim++;
			b.nTk         = 1;
			b.nTkTick     = 0;
			b.nTkGoiThu   = 0;
			b.nTkDichX    = 0;  b.nTkDichY = 0;  b.nTkDichTick = 0;
			b.nTkMuaXong  = 0;  b.nTkRaTick = 0;  b.nTkChoRa = 0;
			b.nTkDaBoc = 0;     b.nTkTgX = 0;   b.nTkTgY = 0;  b.nTkTgTick = 0;
			nDaGoi++;
		}
	}
	pb_Log("[BotTK] GOI QUAN: goi %d/%d bot du tu cach (tran bot = %s) -> du kien"
	       " Tong %d / Kim %d\n", nDaGoi, nUng,
	       s_nPbTkTran > 0 ? "co gioi han" : "KHONG GIOI HAN", nTong, nKim);
	return nDaGoi;
}

// ---------------------------------------------------------------------------
// DIEM DEN TRONG TRAN = TOA DO NPC CUA PHE DOI PHUONG
//
// Chu game: "khi ra danh nhau bot se di chuyen theo toa do npc cua phe doi
// phuong (cho bot nhu nguoi choi chay qua doanh trai doi phuong de tim nguoi
// danh)" va "toi noi cho bot lay toa do npc de goi ham di chuyen A* di chuyen
// chu khong phai cho bot vao danh khi co npc".
//
// Lay THANG hai bang toa do CO SAN trong script - chu game da chi ro:
// "toi bao ban lay toa do trong KIMBINH_TOADO va TONGBINH_TOADO roi ma ban da
// lam theo chua". Ban truoc toi tu quet NPC dang song tren map 379 va DO THUC TE
// trong bot.log la:
//     [BotTK] doanh trai phe 1: 0 diem NPC (map 379)      <- 185 lan, lan nao cung 0
// Bang rong => moi bot roi xuong nhanh du phong, ma nhanh do chi co MOT toa do
// cung cho moi phe => ca dan don ve dung mot cho. Chinh la canh chu game nhin thay.
//
// (Chu thich cu o day lap luan rang khong chep bang vao C++ duoc vi "vi tri 2 phe
// dao ngau nhien moi tran". Lap luan do SAI: phep dao la TAT DINH va doc duoc bang
// M_VITRI_TRENDUOI - chinh gia tri ma tep nay VAN DANG dung o nhanh du phong ben
// duoi. Doc song theo camp thi "tu dung o ca hai the tran" that, nhung chi dung khi
// CO NPC de doc; o day khong co con nao.)
//
//   lib_tktc.lua:163  TONGBINH_TOADO -  78 diem, x 1300..1426, y 3318..3499
//   lib_tktc.lua:244  KIMBINH_TOADO  - 139 diem, x 1434..1620, y 3140..3354
// Don vi trong script la O; pb_ODat cung nhan O va TRA VE MPS, con aX/aY cua
// PB_TkDoanh phai la MPS (cho khop g_GetDistance voi GetMpsPos) nen nhan 32.
//
// Phep dao cho 2 phe - chep DUNG phep so sanh cua Lua (lib_tktc.lua:486-499):
//     if (nViTri == 1) then TONGBINH_POS = TONGBINH_TOADO;  KIMBINH_POS = KIMBINH_TOADO
//     else                  TONGBINH_POS = KIMBINH_TOADO;   KIMBINH_POS = TONGBINH_TOADO
// tuc CHI nViTri == 1 la giu nguyen, MOI gia tri khac deu dao. Phai chep dung the
// nay chu khong duoc viet "== 2 thi dao": truoc luc mission kip dat gia tri thi
// GetMissionValue tra 0, ma 0 phai roi vao nhanh DAO.
//
// pb_TkLayDoanh nhan nCampDich = camp DICH, va bang tra ve la noi bot chay TOI:
// bot phe Tong (camp 1) di tim phe Kim => nCampDich = 2 => bang KIMBINH_POS.
// ---------------------------------------------------------------------------
// TONGBINH_TOADO - lib_tktc.lua:163 - 78 diem, x 1300..1426, y 3318..3499 (don vi O)
static const short s_tkTongBinh[78][2] = {
	{1368,3391}, {1369,3387}, {1370,3383}, {1376,3388}, {1381,3384}, {1383,3374},
	{1383,3367}, {1391,3365}, {1396,3363}, {1394,3355}, {1396,3348}, {1405,3346},
	{1403,3357}, {1406,3363}, {1414,3365}, {1422,3372}, {1426,3379}, {1418,3384},
	{1414,3379}, {1412,3396}, {1419,3403}, {1421,3412}, {1416,3420}, {1409,3415},
	{1403,3420}, {1400,3426}, {1403,3433}, {1400,3442}, {1397,3450}, {1398,3457},
	{1391,3462}, {1383,3459}, {1377,3462}, {1381,3472}, {1381,3480}, {1376,3483},
	{1367,3478}, {1366,3478}, {1360,3487}, {1359,3492}, {1351,3497}, {1344,3499},
	{1346,3494}, {1351,3487}, {1354,3482}, {1300,3410}, {1303,3402}, {1308,3396},
	{1306,3388}, {1311,3383}, {1317,3380}, {1319,3371}, {1320,3363}, {1329,3361},
	{1338,3357}, {1344,3352}, {1352,3349}, {1348,3340}, {1335,3350}, {1352,3340},
	{1359,3340}, {1360,3329}, {1368,3328}, {1374,3331}, {1379,3326}, {1385,3320},
	{1394,3319}, {1404,3318}, {1411,3327}, {1407,3336}, {1401,3339}, {1393,3332},
	{1385,3333}, {1388,3350}, {1392,3356}, {1400,3355}, {1407,3361}, {1403,3366},
};

// KIMBINH_TOADO - lib_tktc.lua:244 - 139 diem, x 1434..1620, y 3140..3354 (don vi O)
static const short s_tkKimBinh[139][2] = {
	{1434,3331}, {1436,3320}, {1437,3303}, {1444,3302}, {1447,3314}, {1444,3326},
	{1441,3335}, {1441,3344}, {1444,3353}, {1451,3354}, {1453,3345}, {1452,3336},
	{1458,3333}, {1463,3338}, {1471,3344}, {1477,3342}, {1475,3332}, {1470,3323},
	{1464,3318}, {1461,3308}, {1459,3300}, {1451,3291}, {1450,3281}, {1453,3271},
	{1458,3269}, {1466,3273}, {1476,3275}, {1480,3281}, {1478,3290}, {1477,3297},
	{1482,3299}, {1491,3297}, {1494,3306}, {1491,3314}, {1485,3323}, {1483,3332},
	{1485,3339}, {1493,3340}, {1497,3333}, {1497,3323}, {1499,3312}, {1506,3304},
	{1514,3306}, {1519,3313}, {1517,3323}, {1512,3331}, {1510,3336}, {1518,3334},
	{1523,3328}, {1525,3318}, {1530,3311}, {1534,3320}, {1539,3324}, {1542,3316},
	{1544,3310}, {1549,3317}, {1552,3321}, {1557,3318}, {1558,3309}, {1559,3303},
	{1563,3309}, {1568,3312}, {1571,3306}, {1571,3296}, {1578,3298}, {1581,3304},
	{1587,3300}, {1590,3295}, {1588,3288}, {1585,3282}, {1587,3275}, {1592,3272},
	{1598,3277}, {1602,3282}, {1607,3279}, {1607,3270}, {1606,3261}, {1607,3254},
	{1613,3253}, {1619,3253}, {1620,3249}, {1617,3241}, {1614,3236}, {1606,3243},
	{1609,3248}, {1609,3239}, {1539,3165}, {1535,3157}, {1539,3152}, {1540,3144},
	{1535,3140}, {1530,3145}, {1527,3153}, {1523,3158}, {1518,3159}, {1516,3154},
	{1518,3146}, {1512,3142}, {1507,3147}, {1507,3154}, {1509,3161}, {1509,3169},
	{1505,3172}, {1500,3169}, {1496,3177}, {1496,3186}, {1492,3190}, {1487,3185},
	{1482,3190}, {1485,3197}, {1488,3201}, {1487,3207}, {1482,3207}, {1477,3202},
	{1472,3204}, {1469,3213}, {1474,3217}, {1477,3223}, {1474,3229}, {1469,3228},
	{1462,3229}, {1461,3238}, {1465,3245}, {1470,3252}, {1472,3257}, {1477,3260},
	{1485,3265}, {1490,3272}, {1493,3278}, {1497,3283}, {1501,3279}, {1502,3271},
	{1505,3265}, {1511,3265}, {1513,3261}, {1514,3256}, {1521,3257}, {1524,3257},
	{1523,3249},
};

#define PB_TK_DIEM_MAX 192
struct PB_TkDoanh
{
	int          nSo;
	int          bDaDung;              // da dung bang chua
	int          nViTri;               // the tran da dung bang nay (de dung lai)
	int          nSub;                 // subworld da dung (s_tkDoanh dung chung)
	int          aX[PB_TK_DIEM_MAX];
	int          aY[PB_TK_DIEM_MAX];
};
static PB_TkDoanh s_tkDoanh[3];       // [1] = doanh phe Tong, [2] = doanh phe Kim

// Hai phe DAO CHO nhau moi tran. Tra 1 = DAO, 0 = giu nguyen.
//
// (21/08 phan bien) TACH RIENG ra day vi phep dao nay truoc do bi CHEP LAM BA BAN
// trong tep - va toi chi sua mot ban, hai ban kia van giu khuon cu "(nViTri == 2)".
// Hai khuon do chi lech nhau DUNG TAI nViTri == 0, ma 0 la gia tri CO THAT:
// script\missions\mission01.lua:40 dat SetMission(M_VITRI_TRENDUOI, 0) trong
// EndMission, va KMissionArray::GetMissionValue tra 0 khi o chuoi con rong. Lua
// (lib_tktc.lua:486, mobinhtk.lua:306) viet "if nViTri == 1 then giu nguyen else
// dao" nen 0 PHAI roi vao nhanh dao.
//
// Tu nay moi cho deu goi ham nay - khong con ban sao nao de lech nua.
static int pb_TkDaoTheTran(int nSub)
{
	if (nSub < 0 || nSub >= MAX_SUBWORLD)
		return 0;
	return (SubWorld[nSub].m_MissionArray.GetMissionValue(PB_TK_MVITRI) != 1);
}

// ---------------------------------------------------------------------------
// DIEM TRUNG GIAN - de bot cung phe CHIA NHAU NHIEU DUONG, nhieu huong.
//
// Chu game: "bot da chia ra 2 duong chay nhung toi muon bot phai chia nhau chay
// nhieu duong de giong nguoi choi" / "nen cho bot chay rai ra nhieu toa do random
// de tao cam giac chay nhieu huong giong nguoi choi".
//
// DO THAT tren luoi 379_srv.fp (608x672 o, 110.393/408.576 o di duoc = 27%):
//  - Eo giua hai doanh trai co HAI cua, tong be rong 45 o, va 41/45 o do van cho
//    duong nam trong nguong +5% chi phi. Bot chi bang qua DUNG 5 o. Ca ban do bot
//    chi cham 20,5% so o di duoc. => Dia hinh KHONG phai thu pham, con rat nhieu cho.
//  - Thu pham la BA TANG DON NHAU, deu o phia ta:
//      a. 8 diem SetPos cua tongratrai.lua chi roi vao 7 BLOCK (kimratrai: 6).
//      b. FindPath_Block la A* THUAN TUY, TAT DINH - khong seed, khong nhieu, khong
//         tie-break ngau nhien. Cung (block xuat phat, block dich) => CUNG chuoi
//         block, khong tru ngoai le.
//      c. BlockNearestMps KEP toa do bot vao hinh chu nhat block; gap block 1x1 O
//         thi hinh chu nhat co ve MOT DIEM => ca dan bi ep thanh HANG MOT. Co 9
//         block nam tren 100% so chuyen (cum o(1352..1383, 3378..3408)), trong do
//         1 block 1x1 va 2 block 1x2.
//  - Doi DICH khong cuu duoc than duong: dich da rai that roi (bang KIMBINH sau khi
//    rai cho 1.315 block dich khac nhau) ma van chung mot hanh lang. Muon doi THAN
//    duong chi co ba duong vao: doi BLOCK XUAT PHAT, doi BLOCK DICH, hoac doi luoi.
//    => phai chen CHANG TRUNG GIAN de doi block xuat phat cua chang dai.
//  - Bang chung dut diem: xuat phat tu 9 toa do o suon dong cho 0/36 duong di qua
//    cum choke, va con RE HON (chi phi trung binh 6359 so voi 8868).
//
// KHO DIEM lay tu CHINH SCRIPT (sinh tu dong, khong chep tay): gop moi bang toa do
// chien truong trong lib_tktc.lua + trinhsat.lua. Lay tu script vi moi diem do la
// cho script THAT SU sinh NPC / tha nguoi choi, nen chac chan NAM TRONG dau truong.
//
// VI SAO KHONG boc toa do ngau nhien tuy y tren ban do: dau truong Tong Kim la MOT
// thanh phan lien thong RIENG tren luoi A* (3.343 block / 33.890 o), bi vung ngoai
// bao quanh. Do that ti le mot diem lech roi sang thanh phan KHAC: +-12 o ~6% ;
// +-40 o 14,5% ; +-60 o 25,8% ; +-80 o 54,3%. Moi diem nhu vay bat FindPath_Block
// quet SACH 3.343 block roi tra "duong cut" - ma vong lap do KHONG co bien dem,
// khong tran nut, khong ngat theo thoi gian, va khong log dong nao. 500 bot lam
// vay cung luc la dot CPU khong dau vet.
// ---------------------------------------------------------------------------
#define PB_TK_LECH_O    12                   // bien do rai lech quanh diem kho (O).
                                             // = PB_SAP_BFS_R - 1, vua trong cua so
                                             // BFS 27x27 cua pb_SapLoang. TUYET DOI
                                             // khong nang len cho "nhieu duong hon".
#define PB_TK_TRUNG_HAN (GAME_FPS * 40)      // qua han chang trung gian -> bo, di thang
#define PB_TK_TRUNG_TOI 160                  // ban kinh coi nhu toi diem trung gian (MPS)

// KHO DIEM TRUNG GIAN - gop tu moi bang toa do chien truong trong script Tong Kim.
// Sinh tu dong bang script (khong chep tay). Don vi O.
// Nguon: lib_tktc.lua (COTONG/COKIM/POST_DES_CAMCO/TONGBINH/KIMBINH/HIEUUY/
//         TLINH/PTUONG/DTUONG) + trinhsat.lua (RANDOM_POS_TONG/KIM).
// 264 diem sau khi bo trung | x 1300..1620 (rong 320 o) | y 3140..3499 (cao 359 o)
static const short s_tkKhoDiem[264][2] = {
	{1300,3410}, {1303,3402}, {1306,3388}, {1308,3396}, {1311,3383}, {1313,3433},
	{1314,3433}, {1317,3380}, {1319,3371}, {1320,3363}, {1320,3459}, {1329,3361},
	{1330,3443}, {1331,3444}, {1332,3443}, {1335,3350}, {1338,3357}, {1338,3420},
	{1338,3463}, {1339,3355}, {1340,3456}, {1343,3467}, {1344,3352}, {1344,3499},
	{1345,3430}, {1346,3494}, {1347,3431}, {1348,3340}, {1348,3441}, {1351,3420},
	{1351,3445}, {1351,3487}, {1351,3497}, {1352,3340}, {1352,3349}, {1354,3482},
	{1359,3340}, {1359,3492}, {1360,3329}, {1360,3487}, {1366,3478}, {1367,3478},
	{1368,3328}, {1368,3391}, {1369,3387}, {1370,3383}, {1370,3387}, {1374,3331},
	{1374,3474}, {1376,3388}, {1376,3483}, {1377,3462}, {1379,3326}, {1381,3384},
	{1381,3472}, {1381,3480}, {1383,3367}, {1383,3374}, {1383,3459}, {1385,3320},
	{1385,3333}, {1387,3463}, {1388,3350}, {1391,3365}, {1391,3462}, {1392,3356},
	{1393,3332}, {1394,3319}, {1394,3355}, {1396,3348}, {1396,3363}, {1397,3431},
	{1397,3450}, {1398,3457}, {1400,3355}, {1400,3426}, {1400,3442}, {1401,3339},
	{1403,3355}, {1403,3357}, {1403,3366}, {1403,3420}, {1403,3433}, {1404,3318},
	{1405,3346}, {1406,3363}, {1407,3336}, {1407,3361}, {1409,3415}, {1410,3358},
	{1411,3327}, {1412,3396}, {1414,3365}, {1414,3379}, {1416,3420}, {1417,3384},
	{1418,3384}, {1419,3403}, {1421,3412}, {1422,3372}, {1426,3379}, {1434,3331},
	{1436,3320}, {1437,3303}, {1441,3335}, {1441,3344}, {1444,3302}, {1444,3326},
	{1444,3353}, {1447,3314}, {1450,3281}, {1451,3291}, {1451,3354}, {1452,3336},
	{1453,3271}, {1453,3345}, {1458,3269}, {1458,3333}, {1459,3300}, {1461,3238},
	{1461,3308}, {1462,3229}, {1463,3338}, {1464,3318}, {1465,3245}, {1466,3273},
	{1469,3213}, {1469,3228}, {1470,3229}, {1470,3252}, {1470,3323}, {1471,3344},
	{1472,3204}, {1472,3257}, {1474,3217}, {1474,3229}, {1475,3332}, {1476,3217},
	{1476,3275}, {1477,3202}, {1477,3223}, {1477,3260}, {1477,3297}, {1477,3342},
	{1478,3290}, {1480,3281}, {1482,3190}, {1482,3207}, {1482,3299}, {1483,3332},
	{1485,3197}, {1485,3265}, {1485,3323}, {1485,3339}, {1487,3185}, {1487,3207},
	{1488,3201}, {1489,3190}, {1490,3272}, {1491,3297}, {1491,3314}, {1492,3190},
	{1493,3278}, {1493,3340}, {1494,3306}, {1496,3177}, {1496,3186}, {1497,3283},
	{1497,3323}, {1497,3333}, {1499,3312}, {1500,3169}, {1501,3279}, {1502,3271},
	{1505,3172}, {1505,3265}, {1506,3304}, {1507,3147}, {1507,3154}, {1509,3161},
	{1509,3169}, {1510,3336}, {1511,3265}, {1512,3142}, {1512,3331}, {1513,3261},
	{1514,3256}, {1514,3306}, {1516,3154}, {1517,3323}, {1518,3146}, {1518,3159},
	{1518,3334}, {1519,3313}, {1521,3257}, {1523,3158}, {1523,3249}, {1523,3328},
	{1524,3257}, {1525,3318}, {1527,3153}, {1530,3145}, {1530,3311}, {1533,3320},
	{1534,3320}, {1535,3140}, {1535,3157}, {1539,3152}, {1539,3165}, {1539,3208},
	{1539,3324}, {1540,3144}, {1542,3316}, {1544,3310}, {1548,3207}, {1549,3317},
	{1551,3184}, {1552,3321}, {1554,3177}, {1557,3178}, {1557,3318}, {1558,3206},
	{1558,3309}, {1559,3180}, {1559,3303}, {1563,3223}, {1563,3309}, {1568,3198},
	{1568,3312}, {1570,3198}, {1571,3296}, {1571,3306}, {1572,3195}, {1578,3298},
	{1579,3174}, {1580,3180}, {1580,3185}, {1581,3304}, {1583,3222}, {1584,3292},
	{1585,3217}, {1585,3282}, {1587,3275}, {1587,3300}, {1588,3288}, {1589,3179},
	{1589,3286}, {1590,3295}, {1592,3272}, {1595,3207}, {1598,3277}, {1602,3282},
	{1606,3243}, {1606,3261}, {1607,3254}, {1607,3270}, {1607,3279}, {1609,3239},
	{1609,3248}, {1613,3253}, {1614,3236}, {1617,3241}, {1619,3253}, {1620,3249},
};

// Boc MOT diem trung gian ngau nhien trong kho, rai lech quanh no, va KIEM LIEN
// THONG bang pb_SapLoang.
//
// Dieu kien TIEN LEN: diem boc phai GAN DICH hon cho bot dang dung. Khong co no
// thi bot boc trung mot diem sau lung roi chay nguoc - nhin la biet bot ngay.
//
// Dung pb_SapLoang chu KHONG dung FindPathServer de kiem: chu thich o pb_ODat da
// ghi ro khi KHONG co duong thi A* loang het ca thanh phan lien thong (map 1:
// 777k o) - 200 bot cung mot khung la treo may chu. BFS cua so 27x27 = 729 o co
// TRAN CUNG, re va du.
static int pb_TkBocTrungGian(int nSub, int nBx, int nBy, int nDichX, int nDichY,
                             int nLech, unsigned int now, int* pnMpsX, int* pnMpsY)
{
	const int nKho = (int)(sizeof(s_tkKhoDiem) / sizeof(s_tkKhoDiem[0]));
	// khoang cach hien tai toi dich - diem trung gian phai ngan hon con so nay
	const __int64 dHienTai = (__int64)(nBx - nDichX) * (nBx - nDichX)
	                       + (__int64)(nBy - nDichY) * (nBy - nDichY);

	// tron chi so bot + moc thoi gian + g_Random. LUU Y: Engine/Src/KRandom.cpp
	// trong cay la LCG lanh manh, NHUNG Core KHONG bien dich tep do (Core.vcxproj
	// khong co KRandom.cpp) - g_Random thuc te den tu engine.lib DUNG SAN va DONG
	// BANG THEO GIAY (bat tan tay 18/08: 30 bot sinh cung giay deu gieo = 68). Ai
	// doc KRandom.cpp roi ket luan "no tien moi lan goi" la sai nguon. Thieu hai
	// nguon kia thi trong mot giay ca dan chi khac nhau bang mot cap so cong.
	unsigned int uR = (unsigned int)nLech * 2654435761u
	                ^ (unsigned int)now * 2246822519u
	                ^ ((unsigned int)g_Random(65536) * 3266489917u);
	uR ^= uR >> 13;  uR *= 2654435761u;  uR ^= uR >> 16;

	static unsigned char aReach[PB_SAP_BFS_O];   // MOT LUONG - nhu aQx2/aQy2 cua pb_SapLoang
	int nSoBfs = 0;
	for (int t = 0; t < 6; t++)
	{
		const unsigned int u2 = uR + (unsigned int)t * 2654435761u;
		const int k = (int)(u2 % (unsigned int)nKho);
		const int nOX = (int)s_tkKhoDiem[k][0];
		const int nOY = (int)s_tkKhoDiem[k][1];

		// phai TIEN LEN, khong duoc chay nguoc
		const __int64 dTu = (__int64)(nOX * 32 - nDichX) * (nOX * 32 - nDichX)
		                  + (__int64)(nOY * 32 - nDichY) * (nOY * 32 - nDichY);
		if (dTu >= dHienTai)
			continue;

		// (21/08 phan bien) KIEM TAM TRUOC bang pb_ODuoc: pb_SapLoang danh dau o
		// tam = 1 TRUOC KHI kiem (co y - de NPC dung tren o dac biet van loang duoc)
		// va return nDuoi >= 1 LUON, nen viet "if (!pb_SapLoang(...))" la ma chet.
		// Do that tren 379_srv.fp: 23/264 diem kho (8,7%) nam tren o vat can, 21
		// trong so do loang duoc DUNG MOT O - khong loc thi bot nhan o vat can lam
		// dich vong, A* kep sang block ke ben, PB_WalkTo tra -1 im lang, chang vong
		// bi bo AM THAM ca chu ky 45 giay: dung lai hanh vi hang mot dot nay diet.
		// Khuon dung la dong 4118-4121 (pb_ODuoc truoc, roi so BANG SO voi
		// PB_NEO_LOANG_MIN - 60 o, loai o tui / vung rong gia).
		if (!pb_ODuoc(nSub, nOX * 32, nOY * 32))
			continue;                        // tam la vat can -> bo ung vien
		if (++nSoBfs > 3)
			return 0;                        // chan tran BFS moi lan goi - di thang
		if (pb_SapLoang(nSub, nOX, nOY, aReach) < PB_NEO_LOANG_MIN)
			continue;                        // tui kin / vung rong gia -> bo
		// rai lech quanh diem kho de hai bot boc trung mot diem van khac cho
		const int nBien = PB_TK_LECH_O * 2 + 1;
		for (int j = 0; j < 6; j++)
		{
			const unsigned int u3 = u2 + (unsigned int)j * 2246822519u;
			const int dx = (int)(u3 % (unsigned int)nBien) - PB_TK_LECH_O;
			const int dy = (int)((u3 / (unsigned int)nBien) % (unsigned int)nBien)
			             - PB_TK_LECH_O;
			const int ix = PB_SAP_BFS_R + dx;
			const int iy = PB_SAP_BFS_R + dy;
			if (ix < 0 || ix >= PB_SAP_BFS_W || iy < 0 || iy >= PB_SAP_BFS_W)
				continue;
			if (!aReach[iy * PB_SAP_BFS_W + ix])
				continue;                    // khong noi duoc tu tam -> BO
			*pnMpsX = (nOX + dx) * 32 + 16;
			*pnMpsY = (nOY + dy) * 32 + 16;
			return 1;
		}
		// khong o lech nao noi duoc -> lay chinh tam (da qua pb_ODuoc o tren)
		*pnMpsX = nOX * 32 + 16;
		*pnMpsY = nOY * 32 + 16;
		return 1;
	}
	return 0;                                // het luot -> di thang nhu cu
}

// (21/08 phan bien) Bo tham so "now": than ham khong con dung toi no tu khi cache
// chuyen sang so theo the tran/subworld thay vi moc thoi gian. Giu lai la chu ky
// ham noi doi ve hop dong (nguoi doc tuong bang tu lam moi theo thoi gian).
static const PB_TkDoanh* pb_TkLayDoanh(int nSub, int nCampDich)
{
	if (nCampDich != 1 && nCampDich != 2)
		return NULL;
	if (nSub < 0 || nSub >= MAX_SUBWORLD)
		return NULL;
	PB_TkDoanh* p = &s_tkDoanh[nCampDich];

	// M_VITRI_TRENDUOI nam o cap MANG (KMissionArray) chu khong phai cap mission -
	// khuon lay tu LuaGetMissionValue (ScriptFuns.cpp:10436).
	const int nViTri = SubWorld[nSub].m_MissionArray.GetMissionValue(PB_TK_MVITRI);

	// Bang la HANG TINH nen chi dung lai khi the tran hoac ban do doi. KHONG dat
	// han thoi gian 30 giay nhu ban quet cu: khong co gi de lam moi, ma dung lai
	// moi 30 giay thi 1000 bot lai chep 139 cap so mot cach vo ich.
	if (p->bDaDung && p->nViTri == nViTri && p->nSub == nSub)
		return (p->nSo > 0) ? p : NULL;

	const short (*pBang)[2];
	int nSoBang;
	{
		// nCampDich == 1 -> can TONGBINH_POS ; == 2 -> can KIMBINH_POS
		const int bDao = pb_TkDaoTheTran(nSub);
		const int bLayTong = (nCampDich == 1) ? !bDao : bDao;
		if (bLayTong)
		{
			pBang  = s_tkTongBinh;
			nSoBang = (int)(sizeof(s_tkTongBinh) / sizeof(s_tkTongBinh[0]));
		}
		else
		{
			pBang  = s_tkKimBinh;
			nSoBang = (int)(sizeof(s_tkKimBinh) / sizeof(s_tkKimBinh[0]));
		}
	}
	if (nSoBang > PB_TK_DIEM_MAX)
		nSoBang = PB_TK_DIEM_MAX;

	p->nSo = 0;
	for (int i = 0; i < nSoBang; i++)
	{
		p->aX[p->nSo] = (int)pBang[i][0] * 32;
		p->aY[p->nSo] = (int)pBang[i][1] * 32;
		p->nSo++;
	}
	p->bDaDung = 1;
	p->nViTri  = nViTri;
	p->nSub    = nSub;

	pb_Log("[BotTK] doanh trai phe %d: %d diem tu bang script (the tran %d, map %d)\n",
	       nCampDich, p->nSo, nViTri, SubWorld[nSub].m_SubWorldID);
	return (p->nSo > 0) ? p : NULL;
}

// Tra lai trang thai TRUOC KHI vao Tong Kim + go khoi mission. Goi CHINH ham Lua
// chep tu PlayerEndTongKim (task03.lua:172-216) - xem script\global\bot_tongkim.lua.
// Bat buoc khi bot roi tran SOM: vong don dep cua task03 kep boi
// GetNextPlayer(MS_TONGKIM, idx, 0) nen khong cham toi bot da di khoi tran, va
// bot se mang camp 1/2 + co PK bi khoa ra the gioi thuong -> thanh DICH cua
// nguoi choi that khac camp o giua thanh.
static void pb_TkTraTrangThai(int nIdx)
{
	Player[nIdx].ExecuteScript((char*)"\\script\\global\\bot_tongkim.lua",
	                           (char*)"bot_tk_thoat", 0, false);
}

// Dung Than Hanh Phu len lai bai luyen DUNG CAP (chu game: "cho bot goi script
// item than hanh phu la duoc"). gopos_step3lvXX KHONG kiem item 1271 (kiem do
// chi nam trong GotoMapId), chi kiem CAP roi NewWorld + SetProtectTime - dung
// y het luc nguoi choi bam menu phu.
// Bang tab_lv90map co 25 dong nhung menu goc chi dung 13 dong dau
// (gopos_step2lv90: "for i = 1, 13"), 12 dong sau la map su kien -> bot cung
// chi duoc boc trong 13.
static void pb_TkDungPhu(int nIdx, int nNpcIdx, PB_Bot& b, int nLech)
{
	const int nLevel = Npc[nNpcIdx].m_Level;
	const char* szHam = (nLevel >= 90) ? "gopos_step3lv90" : "gopos_step3lv80";
	const int   nSoBai = (nLevel >= 90) ? 13 : 4;
	const int   nChon  = (((int)g_Random(nSoBai) + nLech * 7) % nSoBai) + 1;  // Lua dem tu 1
	Player[nIdx].ExecuteScript((char*)"\\script\\item\\ib\\shenxingfu.lua",
	                           (char*)szHam, nChon, false);
	pb_Log("[BotTK] %s cap %d het tran -> Than Hanh Phu %s(%d)\n",
	       Player[nIdx].m_PlayerName, nLevel, szHam, nChon);
	b.nTkDichX = 0;  b.nTkDichY = 0;
}

// Go co Tong Kim va tra bot ve nhip song binh thuong.
// PHAI ep nAi ve PB_AI_FIGHT: pb_RaBai / pb_DaTau chi chay trong nhanh do, bot o
// trang thai IDLE se dung im vinh vien tren map vua bi tha xuong.
static void pb_TkGoCo(int nIdx, int nNpcIdx, PB_Bot& b)
{
	// (21/08 phan bien) TU DOC subworld, KHONG nhan tham so: ham nay duoc goi
	// NGAY SAU pb_TkDungPhu - ma phu vua NewWorld bot sang bai luyen. Dung nSub
	// chot tu dau pb_DriveBot thi vong do map ben duoi tra map CU (324), khong
	// khop bang bai nao, b.nBaiIdx = -1 va pb_RaBai bat bot di lai tu dau -
	// chuyen phu thanh vo ich hoan toan.
	const int nSub = Npc[nNpcIdx].m_SubWorldIndex;
	if (nSub < 0 || nSub >= MAX_SUBWORLD)
		return;
	b.nTk = 0;  b.nTkPhe = 0;  b.nTkTick = 0;  b.nTkGoiThu = 0;
	b.nTkDichX = 0;  b.nTkDichY = 0;  b.nTkDichTick = 0;
	b.nTkMuaXong = 0;  b.nTkRaTick = 0;  b.nTkChoRa = 0;
	b.nTkDaBoc = 0;    b.nTkTgX = 0;    b.nTkTgY = 0;    b.nTkTgTick = 0;
	b.nTargetNpc = 0;
	b.nRoamX = 0;  b.nRoamY = 0;
	b.walk.Reset();  b.chase.Reset();  b.roam.Reset();
	b.follow.Reset();  b.nFollowNghiToi = 0;  b.loot.Reset();
	b.nJamTick = 0;  b.nLachToi = 0;
	if (b.nAi != PB_AI_FIGHT)
		b.nAi = PB_AI_FIGHT;
	// bai dang dung chan la bai nao trong bang -> gan lai de xich cum / pb_RaBai
	// khong keo bot di map khac ngay lap tuc. Khong khop thi de -1 cho pb_ChonBai.
	const int nMapNay = SubWorld[nSub].m_SubWorldID;
	int nBai = -1;
	for (int i = 0; i < PB_SO_BAI; i++)
		if (s_bai[i].nMapId == nMapNay && !s_baiHong[i])
		{
			nBai = i;
			break;
		}
	b.nBaiIdx   = nBai;
	b.nBaiLevel = (nBai >= 0) ? s_bai[nBai].nCapToiThieu : 0;
	b.nRaBaiThu = 0;  b.nRaBaiGoi = 0;  b.nDoiMapTick = 0;
}

// ---------------------------------------------------------------------------
// RA KHOI HAU DOANH - di BO toi dung toa do cua trai roi MOI goi kich ban trap.
//
// Chu game 21/08: "khi vao hau doanh phai di chuyen toi mua mau roi se cho di
// chuyen toi toa do trap roi moi goi lai kich ban trap ra trai". Nguoi choi that
// phai di bo qua cua trai; goi kich ban tu giua trai la nhin ra ngay.
//
// Trap nam o TOA DO CO DINH (lib_tktc.lua:648 va :650) trong khi HAI PHE DOI CHO
// cho nhau moi tran (common_tong dung TKPOS_GO_HDOANH[nViTri], common_kim dung
// chi so nguoc lai) -> luon lay cua GAN MINH NHAT, khong lay "cua cua phe minh".
//
// Tra ve:  1 = da qua trap (goi kich ban xong)
//          0 = dang di bo toi cua trai
//         -1 = khong co duong toi cua trai
static int pb_TkRaTrai(int nIdx, int nNpcIdx, int nSub, PB_Bot& b, unsigned int now)
{
	int bx = 0, by = 0;
	Npc[nNpcIdx].GetMpsPos(&bx, &by);
	// DAU vet trap (lib_tktc.lua:648 va :650). AddTrapEx2 (lib_map.lua:364-374)
	// KHONG dat mot o ma rai MOT VET CHEO: for i=0,nDistance -> AddTrap((nX+i),
	// (nY+i)), them hang nY-1. Vet chay tu (nBX,nBY) toi (nBX+10,nBY+10).
	static const int aTx[2] = { 1251, 1661 };   // tongratrai / kimratrai
	static const int aTy[2] = { 3529, 3098 };
	const int nDaiVet = 10;                     // tham so nDistance cua ca hai cua
	// tam vet de do khoang cach (khong dung dau vet - lech nua chieu dai cong)
	const int nTx0 = aTx[0] + nDaiVet / 2, nTy0 = aTy[0] + nDaiVet / 2;
	const int nTx1 = aTx[1] + nDaiVet / 2, nTy1 = aTy[1] + nDaiVet / 2;
	const __int64 dT = (__int64)(bx - nTx0 * 32) * (bx - nTx0 * 32)
	                 + (__int64)(by - nTy0 * 32) * (by - nTy0 * 32);
	const __int64 dK = (__int64)(bx - nTx1 * 32) * (bx - nTx1 * 32)
	                 + (__int64)(by - nTy1 * 32) * (by - nTy1 * 32);
	const int k = (dT <= dK) ? 0 : 1;

	// Diem den = MOT O TREN VET TRAP, chon theo chi so bot de ca dan rai deu 11 o
	// thay vi chen mot goc. O nao khong dung duoc thi pb_ODat tim o trong ke ben.
	const int nLech9 = (int)(&b - s_bots);
	int nOx = 0, nOy = 0;
	{
		const int i9 = nLech9 % (nDaiVet + 1);
		if (!pb_ODat(nSub, aTx[k] + i9, aTy[k] + i9, nLech9, 4, &nOx, &nOy))
		{
			nOx = (aTx[k] + nDaiVet / 2) * 32;
			nOy = (aTy[k] + nDaiVet / 2) * 32;
		}
	}
	pb_LenNguaDuongXa(nIdx, nNpcIdx, b, now);
	// [TKKET3 01/09] bot dung yen >60s (neo dat o dau pha 3): in ket qua tung
	// buoc cua duong ra cua de tim dung cho ket. CHI GHI LOG.
	const int bKet3 = (b.uKet3Tick != 0
	                && now - b.uKet3Tick > (unsigned int)(GAME_FPS * 60));
	const int nW = PB_WalkTo(nNpcIdx, nOx, nOy, nSub, b.walk, PB_FAC_ARRIVE_MPS);
	if (bKet3 && ((now + (unsigned int)nLech9 * 7u) % (unsigned int)(GAME_FPS * 10)) == 0)
		pb_Log("[TkKet3-RA] %s nW=%d o(%d,%d) dich o(%d,%d) k=%d\n",
		       Player[nIdx].m_PlayerName, nW, bx / 32, by / 32, nOx / 32, nOy / 32, k);
	if (nW == 0)
		return 0;                      // dang di bo toi cua trai
	if (nW < 0)
		return -1;                     // khong co duong

	// (23/08 chu game: "khi chua bat dau tong kim thi bot da chay tu hau doanh
	// ra ngoai, con trap ma bao dung gio moi ra duoc thi bot chay xuyen ra")
	// Kich ban trap chi giu MOI NGUOI 10 giay theo dong ho CA NHAN dat luc bao
	// danh (tongratrai.lua:21 doc GetRestTime cua SetTimer(90*18,2) trong
	// common_tong/kim) - no KHONG biet gi ve pha cua TRAN. Nguoi choi thuong bao
	// danh muon nen it lo; bot bao danh ngay tu giay dau -> 10 giay sau ca dan
	// da tran het ra chien truong trong khi tran CHUA chinh thuc bat dau.
	// Nguon "dung gio" lay tu chinh he thong: timerserver.lua:699 dat
	// StartMissionTimer(MS_TONGKIM, 1, TIME_BD_TK*1080) chu thich nguyen van
	// "so phut de chinh thuc bat dau"; het gio task01.lua ontime_tongkim() goi
	// StopMissionTimer(MS_TONGKIM,1) roi loan bao "Tong Kim dai chien chinh thuc
	// bat dau!". Vay: TIMER 1 CON CHAY = con pha bao danh -> dung CHO o cua trai
	// nhu nguoi choi cho truoc cong, CHUA duoc goi kich ban trap.
	// (KTimerTaskFun::Activate tu nap lai chu ky khi no, nhung ontime_tongkim
	// stop ngay dong dau nen sau moc bat dau GetTimerRestTimer(1) giu 0 ca tran.)
	{
		KMission* pM9 = pb_TkMission(NULL);
		if (pM9 && pM9->GetTimerRestTimer(1) > 0)
		{
			// dong ho pha lam moi lien tuc: dung cho o cong la cho HOP LE, khong
			// de PB_TK_PHA_HAN (120 giay) tinh la "ket" roi da bot khoi tran -
			// TIME_BD_TK co ngay duoc noi lai 10 phut nhu chu thich goc.
			b.nTkTick = now;
			// moc rai le sau khi mo cong: moi bot cho them 1..15 giay RIENG theo
			// chi so roi moi buoc qua trap. Khong co no thi luc mo cong ca dan o
			// at qua trap trong cung mot giay - lai "tap trung chay voi nhau".
			// Moc nay dong thoi la nhip kiem tra lai cong (pha 3 chan o
			// "now < b.nTkChoRa" truoc khi goi lai ham nay).
			if (bKet3)
				pb_Log("[TkKet3-CONG] %s cong dong t1=%u -> doi tiep\n",
				       Player[nIdx].m_PlayerName, (unsigned int)pM9->GetTimerRestTimer(1));
			b.nTkChoRa = now + (unsigned int)(GAME_FPS * (1 + ((nLech9 * 13 + 7) % 15)));
			return 0;
		}
	}

	// Da dung o cua trai -> "buoc qua trap": goi dung kich ban ma nguoi choi dam
	// vao. No lam DU: SetPMParam(MS_TONGKIM, idx, 1, 1) (co "dang chien dau" -
	// task02.lua KHONG co, dong do da bi chu thich), SetPos 1 trong 8 diem xuat
	// quan, SetDeathScript theo M_VITRI_TRENDUOI, SetFightState(1),
	// SetProtectTime 3 giay, AddSkillState(963), StopTimer.
	// Cong chan thoi gian dau ham vo hai voi bot: GetRestTime() (KTaskFuns.h:54)
	// tinh bang GIO GAME nen van giam du KPlayer::Active khong chay cho bot.
	if (now - b.nTkRaTick < (unsigned int)(GAME_FPS * 3))
		return 0;                      // gion 3 giay giua hai lan buoc qua trap
	b.nTkRaTick = now;
	// (21/08 phan bien) kich ban trap TU CHOI IM LANG trong 10 giay dau
	// (tongratrai.lua:21-28: nRemain > 0 thi chi Talk roi return, KHONG SetPos).
	// Phai xem bot co THAT SU bi doi cho khong, khong thi ta ghi "da ra tran"
	// trong khi no van dung nguyen o cua trai.
	int nTx9 = 0, nTy9 = 0;
	Npc[nNpcIdx].GetMpsPos(&nTx9, &nTy9);
	// [TKKET3 01/09] dung yen >60s: ghi lai dung gia tri dong ho ma script trap
	// se doc (GetRestTime) + vi tri truoc/sau, de biet no TU CHOI hay SetPos hut.
	int bLogTrap9 = 0;
	if (bKet3 && now - b.uKet3Trap >= (unsigned int)(GAME_FPS * 15))
	{
		b.uKet3Trap = now;
		bLogTrap9 = 1;
		pb_Log("[TkKet3-TRAP] %s goi trap k=%d rest=%u truoc o(%d,%d)\n",
		       Player[nIdx].m_PlayerName, k,
		       (unsigned int)Player[nIdx].m_TimerTask.GetRestTime(),
		       nTx9 / 32, nTy9 / 32);
	}
	BOOL bScr9 = Player[nIdx].ExecuteScript(
	    (char*)((k == 0) ? "\\script\\maps\\tongkim\\trap\\tongratrai.lua"
	                     : "\\script\\maps\\tongkim\\trap\\kimratrai.lua"),
	    (char*)"main", nIdx, false);
	b.walk.Reset();
	{
		int nSx9 = 0, nSy9 = 0;
		Npc[nNpcIdx].GetMpsPos(&nSx9, &nSy9);
		if (nSx9 == nTx9 && nSy9 == nTy9)
		{
			if (bLogTrap9)
				pb_Log("[TkKet3-TRAP2] %s script=%d TU CHOI - dung nguyen o(%d,%d)\n",
				       Player[nIdx].m_PlayerName, (int)bScr9, nSx9 / 32, nSy9 / 32);
			return 0;                  // kich ban tu choi - dung cho, thu lai sau
		}
	}
	return 1;
}

// [TK-SAN10 28/08] (chu game) "toi da 10 bot dinh vi cung 1 muc tieu".
// Bang dem so bot dang san theo npc idx: tang/giam song trong khoi [BotSan],
// dung lai moi giay o pb_TkNhip (don rac bot chet/roi tran khong qua duong thoat).
#define PB_TK_SAN_TRAN  10
static short s_nTkSanDem[MAX_NPC];

// [TK-SAN 28/08] Tim DOI THU (nguoi/bot phe dich) GAN NHAT con SONG tren cung
// ban do. Quet Player[] (toi da MAX_PLAYER, chi cham Npc cua khe dang song) -
// re hon nhieu so voi quet ca bang Npc; linh NPC khong tinh la "doi thu" (da co
// pb_Fight nhat tren duong). Bo xac (do_death/do_revive) va mau <= 0 theo dung
// yeu cau chu game "bo qua muc tieu da chet".
static int pb_TkTimDichGanNhat(int nSub, int nNpcIdx, int nCampDich, int nTuIdx, int nLech, int nTranhGanMps, int* pnX, int* pnY)
{
	int bx = 0, by = 0;
	Npc[nNpcIdx].GetMpsPos(&bx, &by);
	// [TK-TANRA 28/08] (chu game) "bot gom chay dung 1 duong, chua tan ra": lay
	// gan-nhat tuyet doi lam ca dan do ve cung mot tum dich -> mot lan duong.
	// Nay gom TOP-25 ung vien gan nhat roi MOI BOT nhan HANG rieng theo chi so
	// (nLech modulo so ung vien): 250 con/phe chia ~10 con moi dich, 25 dich rai
	// nhieu canh -> nhieu lan duong. Cham mat giua duong van danh gan nhat (pb_Fight).
// [TK-CHIAMAP 28/08] 25 -> 60: 25 ung vien GAN NHAT deu nam trong chinh dam danh
// nhau -> chia hang van quanh mot cum. 60 ung vien phu toi tan canh xa: hang cua
// moi bot rai theo toan pho khoang cach + tran 10 con/dich -> nhieu o danh nhau
// khap map (chu game: "chia ra nhieu doi thu o xa nhau, nhieu nhom danh nhau").
#define PB_TK_SAN_TOPK 60
	// [TK-CHONGCAMP 28/08] (chu game: "bot ket trong doanh trai khong ra duoc")
	// khong san muc tieu con trong VUNG RA QUAN cua phe no: quanh hau doanh R=45 o
	// + quanh cum diem SetPos ra trai R=25 o (tam cum tu RANDOM_POS tongratrai/
	// kimratrai.lua). Truoc do [BotSan] hai phe cam ngay cua dich xay thit nguoi
	// vua buoc ra (chet sau 10-16 giay) -> ca dan quan het vong doi trong trai.
	static const int aZHx[2] = { 1229, 1689 };   // hau doanh (o) - khop aKHx pha 4
	static const int aZHy[2] = { 3561, 3074 };
	const int iTongZ = pb_TkDaoTheTran(nSub);
	const int hz = (nCampDich == 1) ? iTongZ : (1 - iTongZ);
	const int nZCampX = aZHx[hz] * 32, nZCampY = aZHy[hz] * 32;
	const int nZRaX = ((nCampDich == 1) ? 1331 : 1568) * 32;
	const int nZRaY = ((nCampDich == 1) ? 3442 : 3200) * 32;
	int     aC[PB_TK_SAN_TOPK];
	__int64 aD[PB_TK_SAN_TOPK];
	int nCand = 0;
	for (int i = 1; i < MAX_PLAYER; i++)
	{
		const int nn = Player[i].m_nIndex;
		if (nn <= 0 || nn >= MAX_NPC || nn == nNpcIdx)
			continue;
		if (Npc[nn].m_dwID == 0)
			continue;
		if (Npc[nn].m_SubWorldIndex != nSub)
			continue;
		if ((int)Npc[nn].m_CurrentCamp != nCampDich)
			continue;
		if (Npc[nn].m_Doing == do_death || Npc[nn].m_Doing == do_revive)
			continue;              // bo qua muc tieu da chet / dang hoi sinh
		if (Npc[nn].m_CurrentLife <= 0)
			continue;
		if (nn != nTuIdx && s_nTkSanDem[nn] >= PB_TK_SAN_TRAN)
			continue;              // [TK-SAN10] da du 10 con san no -> bo qua
		int ex = 0, ey = 0;
		Npc[nn].GetMpsPos(&ex, &ey);
		{
			int zdx = ex - nZCampX;  if (zdx < 0) zdx = -zdx;
			int zdy = ey - nZCampY;  if (zdy < 0) zdy = -zdy;
			if (zdx <= 45 * 32 && zdy <= 45 * 32)
				continue;          // con trong khu hau doanh phe no - chua duoc san
			zdx = ex - nZRaX;  if (zdx < 0) zdx = -zdx;
			zdy = ey - nZRaY;  if (zdy < 0) zdy = -zdy;
			if (zdx <= 25 * 32 && zdy <= 25 * 32)
				continue;          // vua buoc ra cua trai - cho di han ra ngoai
		}
		const __int64 dx = (__int64)ex - bx;
		const __int64 dy = (__int64)ey - by;
		const __int64 d = dx * dx + dy * dy;
		if (nTranhGanMps > 0 && d < (__int64)nTranhGanMps * nTranhGanMps)
			continue;              // [TK-TAN] ket dam dong quanh minh -> bo muc tieu gan, toa canh khac
		if (nCand < PB_TK_SAN_TOPK)
		{
			int pos = nCand++;
			while (pos > 0 && aD[pos - 1] > d)
			{
				aC[pos] = aC[pos - 1];  aD[pos] = aD[pos - 1];  pos--;
			}
			aC[pos] = nn;  aD[pos] = d;
		}
		else if (d < aD[PB_TK_SAN_TOPK - 1])
		{
			int pos = PB_TK_SAN_TOPK - 1;
			while (pos > 0 && aD[pos - 1] > d)
			{
				aC[pos] = aC[pos - 1];  aD[pos] = aD[pos - 1];  pos--;
			}
			aC[pos] = nn;  aD[pos] = d;
		}
	}
	if (nCand <= 0)
		return 0;
	const int nBest = aC[(unsigned)nLech % (unsigned)nCand];
	Npc[nBest].GetMpsPos(pnX, pnY);
	return nBest;
}

// ---------------------------------------------------------------------------
// MAY TRANG THAI mot bot trong Tong Kim. Goi tu pb_DriveBot, ngay SAU khoi tu
// hoi sinh (de bot chet trong tran van song lai) va TRUOC moi nhanh khac (ban
// sap / ve thanh / Da Tau / ra bai) - bon nhanh do se keo bot ra khoi tran
// trong 3 giay den 5 phut neu duoc chay.
// ---------------------------------------------------------------------------
static void pb_TkLai(int nIdx, int nNpcIdx, int nSub, PB_Bot& b, int nLech)
{
	const unsigned int now = SubWorld[nSub].m_dwCurrentTime;
	const int nMapNay = SubWorld[nSub].m_SubWorldID;
	if (b.nTkTick == 0)
		b.nTkTick = now;

	// --------------------------------------------------------- pha 5: het tran
	// Da duoc task03 (hoac pb_TkTraTrangThai) tha ra ngoai -> dung phu len bai.
	if (b.nTk == 5)
	{
		// Duong ket thuc BINH THUONG: task03 (PlayerEndTongKim) da NewWorld moi
		// nguoi ve map 324 TRUOC khi CloseMission, nen luc nhip thay mission da
		// dong thi bot da o 324 roi -> dung phu ngay.
		// Duong THOAT SOM: pb_TkTraTrangThai go bot khoi mission nhung KHONG
		// dich chuyen no; task03 se khong bao gio cham toi no nua. Neu chi
		// "return" khi con o 379 thi bot dung im tren chien truong VINH VIEN.
		// Nen: cho toi da 15 giay roi tu dung phu di.
		if (nMapNay == PB_TK_MAP
		 && now - b.nTkTick < (unsigned int)(GAME_FPS * 15))
			return;
		// (23/08 chu game: "khi xong tong kim thi 1 so bot khong ra map train ma
		// dung o map bao danh") Do that tran 14:08 hom nay: task03 NewWorld ~336
		// bot ve 324 trong MOT vong lap, roi pha nay lai phu ca dan trong cung
		// giay ke tiep - dung vet "ca dan ChangeWorld cung mot khung la nghen
		// region sync" (bai hoc 18/08 da ghi o pha 1). Con nao ChangeWorld truot
		// thi o lai 324 voi nTk = 0 + nBaiIdx = -1 va dung im vinh vien: map 324
		// khong co Xa Phu, khong thuoc bang bai nao. Sua BA lop:
		//   a. rai le 1..12 giay theo chi so nhu pha 1 - het doi map cung khung;
		//   b. phu xong KIEM da roi map that chua (gopos_step3lvXX khong tra ve
		//      gi - y het bai hoc bao danh) -> truot thi thu lai, phu boc dong
		//      khac cua bang 13 map;
		//   c. 3 lan van truot -> ChangeWorld thang ve thanh nha, chuoi co san
		//      pb_RaBai -> Xa Phu -> bai dung cap tu lo not.
		if (b.nTkDaBoc != 77)
		{
			b.nTkDaBoc  = 77;          // dau "da vao pha 5" (pha 4 chi dung 0/1)
			b.nTkGoiThu = 0;           // muon lai lam bo dem so lan phu truot
		}
		if (now - b.nTkTick < (unsigned int)((1 + (nLech % 12)) * GAME_FPS))
			return;                    // rai le - khong ca dan doi map cung khung
		if (now < b.nTkChoRa)
			return;                    // gion 3 giay giua hai lan phu truot
		{
			const int nSubCu = Npc[nNpcIdx].m_SubWorldIndex;
			pb_TkDungPhu(nIdx, nNpcIdx, b, nLech);
			if (Npc[nNpcIdx].m_SubWorldIndex == nSubCu
			 && (nMapNay == PB_TK_MAP_BD || nMapNay == PB_TK_MAP))
			{
				b.nTkGoiThu++;
				b.nTkChoRa = now + (unsigned int)(GAME_FPS * 3);
				if (b.nTkGoiThu < 3)
					return;            // phu khong an - nhip sau thu lai
				const PB_DtNpc& tn = pb_ThanhNha(b, nLech);
				const int nSubTn = g_SubWorldSet.SearchWorld(tn.nMap);
				int nVx5 = 0, nVy5 = 0;
				if (pb_ODat(nSubTn, tn.nX, tn.nY, nLech, 12, &nVx5, &nVy5))
					Npc[nNpcIdx].ChangeWorld(tn.nMap, nVx5, nVy5);
				pb_Log("[BotTK] %s phu 3 lan khong roi duoc map %d -> ChangeWorld"
				       " ve thanh %d\n", Player[nIdx].m_PlayerName, nMapNay, tn.nMap);
			}
		}
		pb_TkGoCo(nIdx, nNpcIdx, b);
		return;
	}

	// Tran dong giua chung (het gio / chu game tat) -> task03 da tu tra trang
	// thai va NewWorld ve map 324; chi con viec dung phu len bai.
	if (!s_nPbTkDangMo)
	{
		b.nTk = 5;
		b.nTkTick = now;
		return;
	}

	// Ket qua han o mot pha (khong tim thay NPC, script tu choi im lang...) ->
	// tra trang thai roi thoi, KHONG de bot dung im het tran.
	// [TK-KET 28/08] DANG DI CHUYEN (hang doi 250 con/phe don ve Quan Y + cua trai)
	// khong phai "ket": 17:19-17:21 hom nay 85/250 phe Tong xep hang qua 120s bi coi
	// la ket -> bo cuoc mot loat, tran lech han mot phe (0 con Kim dinh). Con nao
	// NHUC NHICH >= 6 o ke tu mau truoc thi lam tuoi dong ho pha; dung im / quanh
	// quan < 6 o van bi cat 120s nhu cu; tran cung 6 x han pha (12') chong treo
	// vinh vien neu bot bi day qua lai lien tuc.
	if (b.nTk >= 1 && b.nTk <= 3)
	{
		if (b.nTkKetPha != b.nTk)
		{
			b.nTkKetPha  = b.nTk;
			b.nTkKetTick = now;
			Npc[nNpcIdx].GetMpsPos(&b.nTkKetX, &b.nTkKetY);
		}
		else if (now - b.nTkKetTick <= (unsigned int)(PB_TK_PHA_HAN * 6))
		{
			int nKx9 = 0, nKy9 = 0;
			Npc[nNpcIdx].GetMpsPos(&nKx9, &nKy9);
			const int nDx9 = (nKx9 - b.nTkKetX) / 32;
			const int nDy9 = (nKy9 - b.nTkKetY) / 32;
			if (nDx9 * nDx9 + nDy9 * nDy9 >= 36)
			{
				b.nTkKetX = nKx9;
				b.nTkKetY = nKy9;
				b.nTkTick = now;               // co tien trien that -> chua phai ket
			}
			// [TK-KET2 28/08] (chu game: "bot phe Tong van thoat khi vao tran") dot
			// 18:03-18:05 con 81 cu KET pha 3 du da co refresh-nhuc-nhich: dau tran
			// 500 con cung xin duong A* (hang doi PathSrv) -> duoi hang doi DUNG IM
			// cho cap duong >120s. TRONG HAU DOANH phe minh la vung an toan: cho o
			// day khong phai "ket" -> lam tuoi dong ho (van chiu tran cung 6x; ra
			// khoi trai ma dung im thi van bi cat nhu cu).
			else if (b.nTk == 3 && nMapNay == PB_TK_MAP)
			{
				static const int aKHx[2] = { 1229, 1689 };   // TKPOS_GO_HDOANH (o)
				static const int aKHy[2] = { 3561, 3074 };
				const int iT9 = pb_TkDaoTheTran(nSub);
				const int h9  = (b.nTkPhe == 1) ? iT9 : (1 - iT9);
				int dxT = nKx9 - aKHx[h9] * 32;  if (dxT < 0) dxT = -dxT;
				int dyT = nKy9 - aKHy[h9] * 32;  if (dyT < 0) dyT = -dyT;
				if (dxT <= 40 * 32 && dyT <= 40 * 32)
					b.nTkTick = now;
			}
		}
	}
	if (now - b.nTkTick > (unsigned int)PB_TK_PHA_HAN && b.nTk < 4)
	{
		pb_Log("[BotTK] %s KET o pha %d qua %d giay (map %d) -> bo cuoc, tra trang thai\n",
		       Player[nIdx].m_PlayerName, b.nTk, PB_TK_PHA_HAN / GAME_FPS, nMapNay);
		pb_TkTraTrangThai(nIdx);
		b.nTk = 5;
		b.nTkTick = now;
		return;
	}

	// ------------------------------------------- pha 1: ve map bao danh 324
	if (b.nTk == 1)
	{
		if (nMapNay == PB_TK_MAP_BD)
		{
			b.nTk = 2;
			b.nTkTick = now;
			return;
		}
		// SO LE: ca dan ChangeWorld cung mot khung la nghen region sync (bai hoc
		// 18/08 "dung thanh rat nhieu"). Cua so bao danh chi 60 GIAY (TIME_BD_TK
		// = 1, lib_tktc.lua:56) nen chi duoc rai trong 12 giay dau, con lai de
		// danh cho khau di bo toi NPC.
		if (now - b.nTkTick < (unsigned int)((nLech % 12) * GAME_FPS))
			return;
		const int nSubBd = g_SubWorldSet.SearchWorld(PB_TK_MAP_BD);
		if (nSubBd < 0)
		{
			pb_Log("[BotTK] map bao danh %d CHUA MO tren may chu -> huy\n", PB_TK_MAP_BD);
			pb_TkGoCo(nIdx, nNpcIdx, b);
			return;
		}
		// dap xuong canh NPC bao danh cua phe minh, moi bot mot o rieng (luoi 9x7
		// nhu pb_RaBai) - ChangeWorld KHONG kiem vat can nen phai loc qua pb_ODat
		// (28/08 chu game) "moi toa do CHINH o Than Hanh Phu": dap DUNG diem
		// battle_transprot (shenxingfu.lua -> lib_tktc.lua:809/:812) tha nguoi choi
		// that: Tong (1541,3178) / Kim (1570,3085). Diem cu bam theo NPC bao danh
		// (startgame.lua:80/:89) lam ca dan dap sat goc ket phia Kim -> don cuc + lag;
		// pha 2 van di bo toi NPC nhu nguoi choi that nen khong doi gi khac.
		const int nOX = (b.nTkPhe == 1) ? 1541 : 1570;   // lib_tktc.lua battle_transprot
		const int nOY = (b.nTkPhe == 1) ? 3178 : 3085;
		const int nDx = ((nLech % 9) - 4) * 2;
		const int nDy = (((nLech / 9) % 7) - 3) * 2;
		int nVx = 0, nVy = 0;
		int nRet = 0;
		if (pb_ODat(nSubBd, nOX + nDx, nOY + nDy, nLech, 10, &nVx, &nVy))
			nRet = Npc[nNpcIdx].ChangeWorld(PB_TK_MAP_BD, nVx, nVy);
		if (nRet != 1 && pb_ODat(nSubBd, nOX, nOY, nLech, 16, &nVx, &nVy))
			nRet = Npc[nNpcIdx].ChangeWorld(PB_TK_MAP_BD, nVx, nVy);
		if (nRet == 1)
		{
			b.nTk = 2;
			b.nTkTick = now;
			b.nTargetNpc = 0;
			b.walk.Reset();  b.chase.Reset();  b.roam.Reset();
			b.follow.Reset();  b.loot.Reset();
		}
		else
			b.nTkTick = now;               // thu lai nhip sau
		return;
	}

	// --------------------------- pha 2: roi nhom + di bo toi NPC bao danh
	if (b.nTk == 2)
	{
		if (nMapNay == PB_TK_MAP)
		{
			b.nTk = 3;                     // script da day sang chien truong
			b.nTkTick = now;
			return;
		}
		if (nMapNay != PB_TK_MAP_BD)
		{
			b.nTk = 1;                     // lac map -> ve lai
			b.nTkTick = now;
			return;
		}
		// Chu game: "khi toi gio tong kim thi bot vao map bao danh thi TU THOAT
		// PATY moi cho di chuyen toi npc bao danh vao tong kim".
		// Ngoai y muon nguoi choi, day con la BAT BUOC ky thuat: khoi "bam theo
		// doi truong" trong pb_DriveBot nam sau khoi nay va co return rieng, va
		// KPlayer::AcceptTeam (KPlayer.cpp:1626) GHI DE m_CurrentCamp cua thanh
		// vien = camp doi truong - hai bot khac phe ma cung nhom se thanh DONG
		// MINH, khong danh nhau duoc.
		if (pb_TrongNhom(nIdx))
		{
			pb_RoiNhom(nIdx, "vao Tong Kim");
			return;                        // nhip sau moi di, cho lenh roi nhom xong
		}
		const char* szNpc = (b.nTkPhe == 1) ? "song_signup" : "jin_signup";
		int bx = 0, by = 0;
		Npc[nNpcIdx].GetMpsPos(&bx, &by);
		// (21/08 phan bien) PHAI qua ban nho: pb_TimNpc quet TOAN BO region cua
		// map moi lan goi, ma khoi nay chay MOI KHUNG cho MOI bot dang cho bao
		// danh - hang tram bot trong cung mot map la O(n^2) khong ai chan.
		// NPC bao danh dung yen suot doi nen quet mot lan la du.
		const int nNpcBd = pb_TimNpcNho(nSub, (b.nTkPhe == 1) ? PB_NPC_TKT : PB_NPC_TKK,
		                                szNpc, bx, by);
		if (nNpcBd <= 0)
		{
			static unsigned int s_uLogNpc = 0;
			if (now - s_uLogNpc >= (unsigned int)(GAME_FPS * 5))
			{
				s_uLogNpc = now;
				pb_Log("[BotTK] KHONG thay NPC bao danh '%s' tren map %d\n", szNpc, nMapNay);
			}
			return;
		}
		int nPx = 0, nPy = 0;
		Npc[nNpcBd].GetMpsPos(&nPx, &nPy);
		const int nW = PB_WalkTo(nNpcIdx, nPx, nPy, nSub, b.walk, PB_FAC_ARRIVE_MPS);
		if (nW == 0)
			return;                        // dang di bo toi NPC nhu nguoi choi
		if (nW < 0)
		{
			// (21/08 - chu game: "phai cho bot di chuyen theo tung buoc nhu
			// nguoi") KHONG dap SetPos nua. Khong co duong thi tinh lai o nhip
			// sau; ket that thi dong ho PB_TK_PHA_HAN se cho bo cuoc.
			b.walk.Reset();
			return;
		}
		// Da dung canh NPC -> "bam menu": dat TaskTemp TMP_INDEX_NPC dung y het
		// main_mobinh (mobinhtk.lua:30) roi goi thang ham nguoi choi bam.
		// go_tong/go_kim tu lo het: kiem cap, kiem camp, kiem can bang quan so,
		// AddMSPlayer, SetCurCamp, SetPunish, SetPKMode, SetRevPos, NewWorld...
		if (now - b.nTkTick < (unsigned int)(GAME_FPS * 3) && b.nTkGoiThu > 0)
			return;                        // gion 3 giay giua hai lan bam
		// (21/08 phan bien) TU XEP HANG theo luat can bang. MAX_PLAYER_CL = 1
		// (lib_tktc.lua:20) nen go_tong tu choi ngay khi nPTong > nPKim va nguoc
		// lai - tu choi IM LANG, khong tra ve gi. Goi quan da chia deu tuyet doi
		// nhung bot TOI NOI theo thu tu ngau nhien nen quan so hai phe van lech
		// tam thoi. Cho luot o day (return TRUOC khi tang nTkGoiThu) thi khong
		// ton lan bam nao, va dan bot phe kia dang toi lien tuc se san bang lai
		// trong vai giay.
		if ((b.nTkPhe == 1 && s_nTkDemTong > s_nTkDemKim)
		 || (b.nTkPhe == 2 && s_nTkDemKim > s_nTkDemTong))
			return;
		// (21/08 chay that) MOI PHE CHI MOT BOT BAM TRONG MOT NHIP. Do that tran
		// 14:58: 429/500 bot bam cung mot luc roi bo cuoc trong 14 giay. Voi khoa
		// nay, moi giay co ~18 con moi phe duoc bam - 500 bot vao het trong ~15
		// giay ma khong con ai dot lan thu vo ich.
		if (s_uTkBamTick[b.nTkPhe] == now)
			return;
		s_uTkBamTick[b.nTkPhe] = now;
		b.nTkTick = now;
		b.nTkGoiThu++;
		Player[nIdx].m_cTask.SetClearVal(PB_TK_TMP_NPC, nNpcBd);
		// (21/08 phan bien - CHI MANG) go_tong/go_kim doc phe bang
		// GetNpcValue(GetTaskTemp(TMP_INDEX_NPC)), tuc lay tu NPC chu KHONG phai
		// tu C++. Nguoi choi bam NPC thi main(sel) dat SetNpcValue(sel, phe)
		// TRUOC khi mo thoai; bot goi thang go_* nen bo qua buoc do. Ma NPC
		// "Kim Binh Bao Danh" (startgame.lua:89) KHONG duoc dat value luc add
		// (m_nNpcParam ZeroMemory o KNpc.cpp:160) -> bot Kim se vao mission voi
		// NHOM 0. AddMSPlayer(...,0) van THANH CONG nen ta khong he biet, nhung:
		// GetMSPlayerCount(...,2) mai mai 0 -> cua can bang tu choi moi bot Tong
		// sau con dau tien; va het tran task03 index TAB_PHE_TONGKIM[0] = nil ->
		// loi Lua giet ca vong don dep -> CloseMission KHONG CHAY -> mission
		// TREO den khi restart may chu.
		// Dat cho CA HAI phe: pb_TimNpc da chon NPC theo dung b.nTkPhe nen gia
		// tri luon khop voi chinh NPC do, khong the lam hong duong nguoi choi.
		if (nNpcBd > 0 && nNpcBd < MAX_NPC && Npc[nNpcBd].m_dwID != 0)
			Npc[nNpcBd].m_nNpcParam[0] = b.nTkPhe;    // = SetNpcValue(nNpcBd, phe)
		if (b.nTkPhe == 1)
			Player[nIdx].ExecuteScript(
			    (char*)"\\script\\startgame\\tinhnang\\tongkim\\song_signup.lua",
			    (char*)"go_tong", 0, false);
		else
			Player[nIdx].ExecuteScript(
			    (char*)"\\script\\startgame\\tinhnang\\tongkim\\jin_signup.lua",
			    (char*)"go_kim", 0, false);
		// go_tong/go_kim KHONG tra ve gi (moi nhanh tu choi deu "return" tran)
		// -> chi co mot cach biet thanh cong: xem bot da sang map 379 chua.
		if (Npc[nNpcIdx].m_SubWorldIndex >= 0
		 && SubWorld[Npc[nNpcIdx].m_SubWorldIndex].m_SubWorldID == PB_TK_MAP)
		{
			// (21/08) cong NGAY vao bo dem, khong cho nhip 1 giay lam moi: trong
			// mot giay co hang chuc con bao danh, cho bo dem cu la ca dan tuong
			// phe minh van dang it hon va cung lao vao.
			if (b.nTkPhe == 1) s_nTkDemTong++; else s_nTkDemKim++;
			b.nTk = 3;
			b.nTkTick = now;
			// cho 5 giay (rai le vai giay theo chi so bot cho khoi ca dan cung
			// buoc mot khung) roi moi di mua thuoc - giong het luc chet hoi sinh
			b.nTkChoRa   = now + (unsigned int)(GAME_FPS * 5 + (nLech % 5) * GAME_FPS);
			b.nTkMuaXong = 0;
			pb_Log("[BotTK] %s bao danh phe %s THANH CONG (lan %d)\n",
			       Player[nIdx].m_PlayerName, (b.nTkPhe == 1) ? "Tong" : "Kim", b.nTkGoiThu);
		}
		else if (b.nTkGoiThu >= 10)
		{
			pb_Log("[BotTK] %s bam bao danh %d lan van khong vao duoc (phe %d) -> bo cuoc\n",
			       Player[nIdx].m_PlayerName, b.nTkGoiThu, b.nTkPhe);
			pb_TkTraTrangThai(nIdx);
			b.nTk = 5;
			b.nTkTick = now;
		}
		return;
	}

	// ------------------------------------- pha 3: hau doanh, cho toi gio ra tran
	if (b.nTk == 3)
	{
		if (nMapNay != PB_TK_MAP)
		{
			// (21/08 phan bien) BI DAY RA SOM trong khi tran VAN DANG MO -> task03
			// chua chay, phai TU tra trang thai. Khong thi bot mang m_CurrentCamp
			// 1/2 + SetPKMode(1,1) khoa co PK ve bai luyen va thanh dich cua nguoi
			// choi that khac camp o giua the gioi.
			pb_TkTraTrangThai(nIdx);
			b.nTk = 5;
			b.nTkTick = now;
			return;
		}
		// [TKKET3 01/09] (chu game: "nhieu bot TK dung yen khong danh") MO XE bot
		// pha 3 dung nguyen mot cho >60s: do 3 tran 31/08 co 45-79 con/tran (15-32%,
		// LUON o phe dong trai DONG 1688,3072) khong bao gio qua cua. Khong dung
		// nTkTick vi vung-an-toan hau doanh (9.38, :8387) lam tuoi no moi tick -
		// phai neo rieng theo toa do. CHI GHI LOG, khong doi hanh vi.
		{
			int nKq3 = 0, nKr3 = 0;
			Npc[nNpcIdx].GetMpsPos(&nKq3, &nKr3);
			int dKx3 = (nKq3 - b.nKet3X) / 32;  if (dKx3 < 0) dKx3 = -dKx3;
			int dKy3 = (nKr3 - b.nKet3Y) / 32;  if (dKy3 < 0) dKy3 = -dKy3;
			if (b.uKet3Tick == 0 || dKx3 >= 2 || dKy3 >= 2)
			{
				b.nKet3X = nKq3;  b.nKet3Y = nKr3;  b.uKet3Tick = now;
			}
			else if (now - b.uKet3Tick > (unsigned int)(GAME_FPS * 60)
			      && now - b.uKet3Log >= (unsigned int)(GAME_FPS * 10))
			{
				b.uKet3Log = now;
				KMission* pMk3 = pb_TkMission(NULL);
				pb_Log("[TkKet3] %s phe %d dung %us o(%d,%d) doing=%d mua=%d choRa=%d"
				       " timer=%u t1=%u\n",
				       Player[nIdx].m_PlayerName, b.nTkPhe,
				       (now - b.uKet3Tick) / GAME_FPS, nKq3 / 32, nKr3 / 32,
				       (int)Npc[nNpcIdx].m_Doing, b.nTkMuaXong,
				       (int)(b.nTkChoRa > now ? (b.nTkChoRa - now) / GAME_FPS : 0),
				       (unsigned int)Player[nIdx].m_TimerTask.GetRestTime(),
				       pMk3 ? (unsigned int)pMk3->GetTimerRestTimer(1) : 0);
			}
		}
		// (21/08 - chu game: "khi chet vao lai hau doanh thi cho thoi gian 5 giay
		// roi di mua mau roi 5 giay sau moi di chuyen toi trap de goi ra ngoai")
		// Nhip nay dung cho CA luot dau (vua bao danh xong) lan moi lan chet hoi
		// sinh, cho dong nhat va nhin ra dang nguoi choi dinh than roi moi di.
		if (now < b.nTkChoRa)
			return;
		// [TK-CUU3 28/08] (chu game: "bot dung yen trong trai khong ra") NewWorld cua
		// script bao danh tha KHONG kiem vat can -> mot so con dap tren O BI CHAN theo
		// luoi server: do_run ma GetDir=0 moi tick = dung yen vinh vien ([BotKet]
		// "bo cuoc: doing=3 procAI=1"). Phat hien la SetPos sang o dat gan nhat
		// (pb_ODat quanh cho dang dung, du phong quanh anchor trai minh).
		if (SubWorld[nSub].CoLuoiSrv())
		{
			int nCx3 = 0, nCy3 = 0;
			Npc[nNpcIdx].GetMpsPos(&nCx3, &nCy3);
			if (SubWorld[nSub].CellObsSrv(nCx3, nCy3) != 0)
			{
				int nVc3 = 0, nVy3 = 0;
				int nOk3 = pb_ODat(nSub, nCx3 / 32, nCy3 / 32, nLech, 8, &nVc3, &nVy3);
				if (!nOk3)
				{
					static const int aCHx[2] = { 1229, 1689 };
					static const int aCHy[2] = { 3561, 3074 };
					const int iTc = pb_TkDaoTheTran(nSub);
					const int hc  = (b.nTkPhe == 1) ? iTc : (1 - iTc);
					nOk3 = pb_ODat(nSub, aCHx[hc], aCHy[hc], nLech, 12, &nVc3, &nVy3);
				}
				if (nOk3)
				{
					pb_Log("[TkCuu3] %s dap tren o CHAN o(%d,%d) trong trai -> SetPos"
					       " o(%d,%d)\n", Player[nIdx].m_PlayerName,
					       nCx3 / 32, nCy3 / 32, nVc3 / 32, nVy3 / 32);
					Npc[nNpcIdx].SetPos(nVc3, nVy3);
					b.walk.Reset();
				}
			}
		}
		// (21/08 - chu game: "phai cho bot mua o npc trong doanh trai") Di BO toi
		// Quan Y roi mua thuoc, y het nguoi choi lam trong luc cho ra tran.
		// Dung ham RIENG bot_tk_muamau (bot_tongkim.lua) chu KHONG dung
		// muamaunhanh cua quany.lua: ham do mua LAP DAY 100% tui, ma pb_DonTui
		// giu lai thuoc nen tui se day VINH VIEN -> khau tra nhiem vu Da Tau doi
		// tui con >= 5 o trong se ket sau tran. Ham rieng chua lai 6 o.
		if (!b.nTkMuaXong)
		{
			pb_LenNguaDuongXa(nIdx, nNpcIdx, b, now);
			// (21/08 phan bien - CA BA lang kinh cung bao) MAP 379 CO HAI QUAN Y:
			// lib_tktc.lua:605 "Tong Quan Y" va :607 "Kim Quan Y", CUNG mot
			// ActionScript, cach nhau ~450 o. Ban nho pb_TimNpcNho chi giu MOT khe
			// cho moi (loai NPC, ban do) nen con bot dau tien chot Quan Y gan NO
			// nhat va MOI bot phe kia deu nhan dung con do -> nua so bot loi xuyen
			// chien truong sang trai DICH. Nen KHONG quet nua: lay thang toa do
			// tu TKPOS_NPC_DIALOG (lib_tktc.lua:574-578, ghi san bang MPS) roi
			// chon theo PHE, hoan theo M_VITRI_TRENDUOI dung khuon common_tong /
			// common_kim (nViTriTong = nViTri, nViTriKim = chi so nguoc lai).
			{
				static const int aQx[2] = { 1249 * 32, 1700 * 32 };
				static const int aQy[2] = { 3557 * 32, 3066 * 32 };
				// (21/08 phan bien) TRUOC DAY viet "(nViTri == 2) ? 1 : 0" - lech
				// voi Lua tai nViTri == 0, luc do bot phe Tong nhan toa do Quan Y
				// cua phe KIM va loi bo xuyen chien truong (~450 o) sang trai dich.
				const int iTong  = pb_TkDaoTheTran(nSub);      // vi tri cua phe Tong
				const int iQ     = (b.nTkPhe == 1) ? iTong : (1 - iTong);
				// [TK-KET 28/08] 250 con/phe cung don ve MOT toa do Quan Y = pheu tac,
				// duoi hang doi vuot 120s. Rai diem den +-8 o theo chi so bot (mua thuoc
				// la ExecuteScript, khong doi dung sat NPC), loc o dat bang pb_ODat.
				int nQtx = aQx[iQ] + ((nLech % 9) - 4) * 2 * 32;
				int nQty = aQy[iQ] + (((nLech / 9) % 7) - 3) * 2 * 32;
				{
					int nQvx = 0, nQvy = 0;
					if (pb_ODat(nSub, nQtx / 32, nQty / 32, nLech, 6, &nQvx, &nQvy))
					{
						nQtx = nQvx;
						nQty = nQvy;
					}
				}
				const int nWq = PB_WalkTo(nNpcIdx, nQtx, nQty, nSub, b.walk,
				                          PB_FAC_ARRIVE_MPS);
				// [TKKET3 01/09] dung yen >60s ngay khau di mua thuoc -> in nWq (CHI LOG)
				if (b.uKet3Tick != 0 && now - b.uKet3Tick > (unsigned int)(GAME_FPS * 60)
				 && ((now + (unsigned int)nLech * 7u) % (unsigned int)(GAME_FPS * 10)) == 0)
					pb_Log("[TkKet3-QY] %s nWq=%d dich o(%d,%d)\n",
					       Player[nIdx].m_PlayerName, nWq, nQtx / 32, nQty / 32);
				if (nWq == 0)
					return;                // dang di bo toi Quan Y cua PHE MINH
				if (nWq > 0)
					Player[nIdx].ExecuteScript((char*)"\\script\\global\\bot_tongkim.lua",
					                           (char*)"bot_tk_muamau", 0, false);
				else
					pb_Log("[TkKet3-MUA] %s KHONG duong toi Quan Y (nWq=%d) -> bo qua mua"
					       " + bo qua StopTimer\n", Player[nIdx].m_PlayerName, nWq);
			}
			b.nTkMuaXong = 1;              // mua roi, hoac khong co Quan Y -> thoi
			b.nTkChoRa   = now + (unsigned int)(GAME_FPS * 5);   // 5 giay roi moi ra cua
			b.walk.Reset();
			return;
		}
		// Cho not 5 giay sau khi mua roi moi di ra cua trai.
		// Dong ho 90 giay cua game chi chay MOT LAN luc bao danh (SetTimer trong
		// common_tong/kim) nen chet hoi sinh khong phai cho lai; rieng luot DAU
		// thi chinh kich ban trap tu chan 10 giay dau, ta khong can dem ho.
		if (now < b.nTkChoRa)
			return;
		// (21/08 chu game) DI BO toi cua trai truoc, toi noi MOI buoc qua trap.
		{
			const int nRa = pb_TkRaTrai(nIdx, nNpcIdx, nSub, b, now);
			if (nRa == 0)
				return;                    // dang di bo toi cua trai
			if (nRa < 0)
			{
				b.walk.Reset();            // khong co duong - tinh lai nhip sau
				return;
			}
		}
		b.nTk = 4;
		b.nTkTick = now;
		b.nTkDichTick = 0;
		// (21/08) Moi luot ra tran la mot duong MOI. Giu nguyen duong ca tran thi
		// bot lap lai dung mot lo trinh hang chuc lan - chinh cai do moi la dau
		// hieu bot. Moi lan chet la mot co hoi doi huong, y het nguoi choi that.
		b.nTkDaBoc = 0;  b.nTkTgX = 0;  b.nTkTgY = 0;  b.nTkTgTick = 0;
		b.walk.Reset();
		pb_Log("[BotTK] %s phe %s qua cua trai RA TRAN\n", Player[nIdx].m_PlayerName,
		       (b.nTkPhe == 1) ? "Tong" : "Kim");
		return;
	}

	// ------------------------------------------------- pha 4: danh nhau trong tran
	if (nMapNay != PB_TK_MAP)
	{
		// het tran (task03 da tra trang thai) HOAC bi day ra som khi tran con mo.
		// Truong hop sau thi task03 chua chay nen phai tu tra - goi thua vo hai:
		// bot_tk_thoat kiem nDataIdx > 0 truoc khi DelMSPlayer, cac SetXxx(0) deu
		// tu khoa lai.
		if (s_nPbTkDangMo)
			pb_TkTraTrangThai(nIdx);
		b.nTk = 5;
		b.nTkTick = now;
		return;
	}

	// (21/08 chay that - chu game: "vao bot khong chay ra khoi trap duoc ma chay
	// lui toi ben trong doanh trai") KET TRONG HAU DOANH.
	// Diem hoi sinh cua nguoi tham gia CHINH LA hau doanh (common_tong/common_kim
	// goi SetTempRevPos(MAP_TK_TC, nXPhucSinh, nYPhucSinh) voi toa do lay tu
	// TKPOS_GO_HDOANH). Bot chet la song lai TRONG TRAI - ma trai chi co mot loi
	// ra la CAI TRAP, va bot thi MIEN TOAN BO TRAP (KNpc.cpp:10068) nen khong
	// bao gio ra duoc nua: no cu tinh duong sang doanh trai dich roi dam vao
	// vach trai, chay lui toi vinh vien - dung canh chu game chup duoc.
	// Sua: thay minh dang o trong ban kinh hau doanh thi GOI LAI kich ban trap
	// ra trai (gion 3 giay), y het nguoi choi di bo qua cua trai lan nua.
	{
		int nHx7 = 0, nHy7 = 0;
		Npc[nNpcIdx].GetMpsPos(&nHx7, &nHy7);
		int bTrongTrai = 0;
		{
			// (21/08 phan bien) CHI nhan dien hau doanh CUA CHINH MINH. Quet ca hai
			// trai thi bot dung o trai DICH cung bi coi la "ket trong trai", bi
			// day ve pha 3 roi chay ra CUA CUA DICH va goi kich ban cua dich -
			// vong lap khong loi ra.
			static const int aHx[2] = { 1229, 1689 };   // TKPOS_GO_HDOANH (o)
			static const int aHy[2] = { 3561, 3074 };
			// (21/08 phan bien) TRUOC DAY viet "(nViTri7 == 2) ? 1 : 0". Tai
			// nViTri == 0 no lech voi bang doanh trai o tren (da dao), nen bot
			// dung DUNG trong hau doanh nha minh lai khong duoc nhan la
			// "bTrongTrai" -> khong chay kich ban ra trai.
			const int iTong7  = pb_TkDaoTheTran(nSub);
			const int h       = (b.nTkPhe == 1) ? iTong7 : (1 - iTong7);
			int dx = nHx7 - aHx[h] * 32;  if (dx < 0) dx = -dx;
			int dy = nHy7 - aHy[h] * 32;  if (dy < 0) dy = -dy;
			if (dx <= 40 * 32 && dy <= 40 * 32)
				bTrongTrai = 1;
		}
		if (bTrongTrai)
		{
			// (21/08 - chu game: "bot chi mua mau 1 lan luc vao tran, toi muon cho
			// bot mua moi lan chet ve lai hau doanh") QUAY VE PHA 3 chu khong ra
			// thang: pha 3 lo TRON GOI "ghe Quan Y mua thuoc -> di bo ra cua trai
			// -> qua trap", dung y het luc moi vao tran.
			b.nTk        = 3;
			b.nTkTick    = now;
			// cho 5 giay roi moi di mua mau (chu game), rai le theo chi so bot
			b.nTkChoRa   = now + (unsigned int)(GAME_FPS * 5 + (nLech % 5) * GAME_FPS);
			b.nTkMuaXong = 0;              // mua binh moi cho luot sau
			b.nTkDichX = 0;  b.nTkDichY = 0;  b.nTkDichTick = 0;
			b.walk.Reset();
			{
				static unsigned int s_uLogRa = 0;
				if (now - s_uLogRa >= (unsigned int)(GAME_FPS * 5))
				{
					s_uLogRa = now;
					pb_Log("[BotTK] %s hoi sinh trong hau doanh o(%d,%d) -> ghe Quan Y"
					       " mua thuoc roi ra cua trai lai\n", Player[nIdx].m_PlayerName,
					       nHx7 / 32, nHy7 / 32);
				}
			}
			return;
		}
	}

	// Co dich trong tam thi danh (pb_Fight tu bat FightMode, tu chon chieu).
	// Bo loc kind_player da duoc mo RIENG cho truong hop nay o pb_FindTarget.
	//
	// GION NHIP TIM MUC TIEU: pb_FindTarget quet 9 vung MOI LAN goi. Ngoai doi
	// thuong 1000 bot rai tren ~16 ban do nen moi vung nhe; trong Tong Kim ca
	// dan don vao MOT map, moi lan quet duyet qua ca ngan nguoi - thanh O(n^2)
	// that su. Dang danh (da co muc tieu) thi van chay moi khung de ra don dung
	// nhip; khau TIM muc tieu moi thi gion lai, rai lech theo chi so bot de
	// khong ca dan cung quet mot khung.
	//
	// (21/08 - chu game: "uu tien dang di chuyen gap doi tuong thi phai dung danh
	// lien") DANG CHAY thi quet DAY GAP BA (6 lan/giay thay vi 2). Voi nhip 2
	// lan/giay, bot chay ~340 MPS/giay luot qua tren 5 O truoc khi kip ngo - no
	// chay xuyen qua mat dich roi moi quay lai.
	const int bTkDangDi = (b.nTkDichX > 0);
	if (b.nTargetNpc > 0
	 || ((now + (unsigned int)nLech) % (bTkDangDi ? 3u : 9u)) == 0)
	{
		const int nMucTruoc = b.nTargetNpc;
		if (pb_Fight(nIdx, nNpcIdx, nSub, b))
		{
			// VUA bat duoc dich GIUA DUONG -> DUNG chuyen di ngay.
			// pb_Fight tu lo phan dung chan: vao tam la SendCommand(do_skill...)
			// (:6078) de len lenh do_run dang chay, vi m_Command chi co MOT khe.
			// Nhung lo trinh b.walk van giu diem den cu - khong xoa thi nhip sau
			// no lai phat do_run keo bot chay tiep giua luc danh.
			if (nMucTruoc <= 0 && b.nTargetNpc > 0)
			{
				b.nTkDichX = 0;  b.nTkDichY = 0;
				b.walk.Reset();
			}
			b.nTkDichTick = 0;             // dang danh -> huy chuyen di
			return;
		}
	}

	// (21/08 - chu game: "uu tien trong tong kim khi dang co doi tuong de danh thi
	// khong goi ham di chuyen toa do moi chi di chuyen toa do khi khong co doi
	// thu") Con muc tieu thi TUYET DOI khong dong toi lo trinh: pb_Fight o tren
	// co the tra 0 trong mot nhip (dang hoi chieu, muc tieu vua khuat tam) ma muc
	// tieu VAN CON - luc do ma boc diem doanh trai moi la bot bo tran dang danh
	// giua chung roi chay di.
	if (b.nTargetNpc > 0)
		return;

	// Het dich gan -> chay qua DOANH TRAI DOI PHUONG tim nguoi danh.
	const int nCampDich = (b.nTkPhe == 1) ? 2 : 1;

	// [TK-SAN 28/08] (chu game) "tu dinh vi doi thu o dau va di chuyen THANG toi
	// danh luon; tren duong gap muc tieu khac thi bat muc tieu GAN NHAT, khong co
	// dinh; bo qua muc tieu chet". Moi ~1,2 giay (so le theo chi so bot, + ngay khi
	// chua co dich) dinh vi lai doi thu gan nhat con song: co -> dat dich = vi tri
	// no, di THANG (khong vong trung gian), doi con khac gan hon / no doi cho > 8 o
	// thi tinh lai duong. Khong thay ai (an nap/chet het) -> roi xuong loi cu:
	// lang thang ve doanh trai dich. Khoi nay chi chay khi KHONG co muc tieu dang
	// danh (b.nTargetNpc da return o tren) nen khong pha luat "dang danh khong
	// dong lo trinh".
	if (((now + (unsigned int)nLech) % 21u) == 0 || b.nTkDichTick == 0)
	{
		// [TK-TAN 28/08] (chu game) ">20 bot cung phe don mot cho thi phai tan ra":
		// dem dong doi cung phe trong 12 o quanh minh; qua 20 -> bo ung vien gan
		// (<20 o), nhan muc tieu canh khac. Chi cham bot CHUA co muc tieu (tuyen
		// sau) - tuyen dau dang danh da return o tren.
		int nTranhGan = 0;
		{
			int bxT = 0, byT = 0;
			Npc[nNpcIdx].GetMpsPos(&bxT, &byT);
			int nDong = 0;
			for (int q8 = 0; q8 < s_botCount; q8++)
			{
				if (s_bots[q8].nTk != 4 || s_bots[q8].nTkPhe != b.nTkPhe)
					continue;
				const int pq = s_bots[q8].nPlayerIdx;
				if (pq <= 0)
					continue;
				const int nq = Player[pq].m_nIndex;
				if (nq <= 0 || nq >= MAX_NPC || Npc[nq].m_SubWorldIndex != nSub)
					continue;
				int qx = 0, qy = 0;
				Npc[nq].GetMpsPos(&qx, &qy);
				int adx = qx - bxT;  if (adx < 0) adx = -adx;
				int ady = qy - byT;  if (ady < 0) ady = -ady;
				if (adx <= 12 * 32 && ady <= 12 * 32)
					nDong++;
			}
			if (nDong > 20)
			{
				// [TK-CHIAMAP] 20 o chi day ra RIA dam dong (dam rong 20-30 o) - van
				// mot cum. 60 o ep tuyen sau nhan dich o vung khac han -> tach o.
				nTranhGan = 60 * 32;
				static unsigned int s_uLogTan = 0;
				if (now - s_uLogTan >= (unsigned int)(GAME_FPS * 3))
				{
					s_uLogTan = now;
					pb_Log("[BotTan] %s phe %d ket dam %d con cung phe trong 12 o"
					       " -> toa sang canh khac\n",
					       Player[nIdx].m_PlayerName, b.nTkPhe, nDong);
				}
			}
		}
		int nSanX = 0, nSanY = 0;
		const int nSan = pb_TkTimDichGanNhat(nSub, nNpcIdx, nCampDich, b.nTkSanIdx, nLech, nTranhGan, &nSanX, &nSanY);
		if (nSan > 0)
		{
			// [TK-KET2 28/08] tiet luu tinh-lai-duong: dich DOI HAN moi reset ngay;
			// dich cu chi doi cho thi toi thieu 3 giay moi re-path mot lan (dau tran
			// 500 con cung xin A* lam nghen hang doi duong -> pha 3 cho duong qua 120s).
			const int bDoiDich = (nSan != b.nTkSanIdx)
			                  || (g_GetDistance(nSanX, nSanY, b.nTkDichX, b.nTkDichY) > 8 * 32
			                   && now - b.nTkTgTick >= (unsigned int)(GAME_FPS * 3));
			if (nSan != b.nTkSanIdx)
			{
				// [TK-SAN10] ke toan song: tha muc tieu cu, giu cho muc tieu moi
				if (b.nTkSanIdx > 0 && b.nTkSanIdx < MAX_NPC && s_nTkSanDem[b.nTkSanIdx] > 0)
					s_nTkSanDem[b.nTkSanIdx]--;
				if (nSan < MAX_NPC)
					s_nTkSanDem[nSan]++;
			}
			b.nTkSanIdx = nSan;
			if (bDoiDich)
			{
				b.nTkDichX = nSanX;
				b.nTkDichY = nSanY;
				b.nTkDaBoc = 1;            // san = di THANG, bo chang vong trung gian
				b.nTkTgX = 0;  b.nTkTgY = 0;
				b.nTkTgTick = now;         // [TK-KET2] moc tiet luu re-path
				b.walk.Reset();
				{
					static unsigned int s_uLogSan = 0;
					if (now - s_uLogSan >= (unsigned int)(GAME_FPS * 2))
					{
						s_uLogSan = now;
						int bxS = 0, byS = 0;
						Npc[nNpcIdx].GetMpsPos(&bxS, &byS);
						pb_Log("[BotSan] %s phe %d duoi %s o(%d,%d) cach %d o\n",
						       Player[nIdx].m_PlayerName, b.nTkPhe,
						       Npc[nSan].Name, nSanX / 32, nSanY / 32,
						       g_GetDistance(bxS, byS, nSanX, nSanY) / 32);
					}
				}
			}
			b.nTkDichTick = now;           // dang san -> khoa nhanh boc doanh trai
		}
		else
		{
			if (b.nTkSanIdx > 0 && b.nTkSanIdx < MAX_NPC && s_nTkSanDem[b.nTkSanIdx] > 0)
				s_nTkSanDem[b.nTkSanIdx]--;   // [TK-SAN10] tha cho
			b.nTkSanIdx = 0;               // khong thay doi thu -> di doanh trai nhu cu
		}
	}
	// (21/08 phan bien) nTkDichX <= 0 VA nTkDichTick != 0 = dang NGHI sau mot lan
	// khong co duong: chi boc lai khi het han, khong boc moi khung.
	if (b.nTkDichTick == 0
	 || now - b.nTkDichTick > (unsigned int)PB_TK_DICH_HAN)
	{
		// (21/08 phan bien) Nhanh "else" cu o day - lay MOT toa do vung xuat quan
		// roi tan trong ban kinh 16 o - DA XOA. Sau khi bang toa do lay tu script
		// (78/139 diem) thi pb_TkLayDoanh chi tra NULL khi camp hoac subworld sai,
		// ma cho goi nay bao dam ca hai deu dung, nen nhanh do la ma chet. Giu lai
		// chi ton cho va de nguoi sua sau tin nham rang no con song - trong khi
		// chinh no la thu pham keo ca dan bot ve mot cho.
		const PB_TkDoanh* pD = pb_TkLayDoanh(nSub, nCampDich);
		if (pD)
		{
			// (21/08 - chu game: "toi muon bot phai random toa do trong danh sach
			// toa do npc phe doi phuong") Tron BA nguon roi mo-dun: chi so bot,
			// MOC THOI GIAN hien tai va g_Random. Phai co moc thoi gian vi
			// g_Random DONG BANG THEO GIAY (bay engine.lib da ghi o pb_ChamHangSap)
			// - thieu no thi trong cung mot giay moi bot chi khac nhau bang mot
			// CAP SO CONG theo chi so, nhin ra ngay la di theo khuon.
			//
			// (21/08 anh chu game: 3 bot chum mot cho) THEM HAI LOP CHONG GOM:
			//  - diem den phai CACH XA cho bot dang dung (PB_TK_DICH_XA): boc
			//    trung diem ngay canh minh thi ca dam quanh quan mot goc doanh
			//    trai, nhin y het "di chung mot toa do".
			//  - quay toi 6 lan de tim diem du xa; het luot thi lay diem cuoi
			//    (con hon dung im).
			int bx8 = 0, by8 = 0;
			Npc[nNpcIdx].GetMpsPos(&bx8, &by8);
			for (int t8 = 0; t8 < 6; t8++)
			{
				unsigned int uR = (unsigned int)nLech * 2654435761u
				                ^ (unsigned int)(now + (unsigned int)t8 * 7919u) * 2246822519u
				                ^ ((unsigned int)g_Random(65536) * 3266489917u);
				uR ^= uR >> 13;  uR *= 2654435761u;  uR ^= uR >> 16;
				const int k = (int)(uR % (unsigned int)pD->nSo);
				b.nTkDichX = pD->aX[k];
				b.nTkDichY = pD->aY[k];
				if (g_GetDistance(bx8, by8, b.nTkDichX, b.nTkDichY) >= PB_TK_DICH_XA)
					break;
			}
			// RAI QUANH diem do theo chi so bot (luoi 13x13 = 169 o rieng): hai
			// bot boc trung MOT diem NPC van dung hai o khac nhau, khong chong len
			// nhau thanh mot cum.
			pb_RaiQuanhCum(nSub, nLech, &b.nTkDichX, &b.nTkDichY);
		}
		b.nTkDichTick = now;
		b.nTkDaBoc = 0;                    // doi dich -> boc diem vong MOI
		b.walk.Reset();
	}
	if (b.nTkDichX <= 0)
		return;                            // dang NGHI sau lan khong co duong (van quet
		                                   // muc tieu o tren), hoac camp/subworld sai

	// ---- CHANG TRUNG GIAN: vong qua mot diem ngau nhien roi moi toi dich ----
	// Day la khau DUY NHAT doi duoc BLOCK XUAT PHAT cua chang dai, tuc khau duy
	// nhat lam A* tra ve mot chuoi block khac. Boc LAI moi khi doi dich (45 giay)
	// nen duong bot di la mot duong gap khuc doi huong vai lan - y het nguoi choi
	// chay lang quang tim nguoi danh, thay vi mot duong thang noi duoi nhau.
	if (!b.nTkDaBoc)
	{
		int bxT = 0, byT = 0;
		Npc[nNpcIdx].GetMpsPos(&bxT, &byT);
		b.nTkDaBoc = 1;
		if (pb_TkBocTrungGian(nSub, bxT, byT, b.nTkDichX, b.nTkDichY,
		                      nLech, now, &b.nTkTgX, &b.nTkTgY))
		{
			b.nTkTgTick = now;
			b.walk.Reset();
			{
				// GION TOAN CUC 1 dong/giay. 1000 bot moi con mot dong la moi dong
				// mot fopen bot.log tren luong game - bai hoc da ghi o pb_Log.
				static unsigned int s_uLogTg = 0;
				if (now - s_uLogTg >= (unsigned int)GAME_FPS)
				{
					s_uLogTg = now;
					pb_Log("[BotLan] %s phe %d -> vong qua o(%d,%d) roi toi o(%d,%d)\n",
					       Player[nIdx].m_PlayerName, b.nTkPhe,
					       b.nTkTgX / 32, b.nTkTgY / 32,
					       b.nTkDichX / 32, b.nTkDichY / 32);
				}
			}
		}
		else
		{
			b.nTkTgX = 0;  b.nTkTgY = 0;   // khong boc duoc -> di thang nhu cu
		}
	}
	if (b.nTkTgX > 0)
	{
		if (now - b.nTkTgTick > (unsigned int)PB_TK_TRUNG_HAN)
		{
			b.nTkTgX = 0;  b.nTkTgY = 0;   // qua han -> thoi, di thang toi dich
			b.walk.Reset();
		}
		else
		{
			pb_LenNguaDuongXa(nIdx, nNpcIdx, b, now);
			const int nWt = PB_WalkTo(nNpcIdx, b.nTkTgX, b.nTkTgY, nSub, b.walk,
			                          PB_TK_TRUNG_TOI);
			if (nWt == 0)
				return;                    // dang tren duong vong
			// toi noi HOAC khong co duong -> bo chang vong, di tiep toi dich
			b.nTkTgX = 0;  b.nTkTgY = 0;
			b.walk.Reset();
		}
	}
	// (21/08 - chu game: "neu di chuyen duong xa thi phai len ngua de di chuyen")
	// Hai doanh trai cach nhau gan 500 o - di bo la het tran chua toi noi.
	pb_LenNguaDuongXa(nIdx, nNpcIdx, b, now);
	const int nW = PB_WalkTo(nNpcIdx, b.nTkDichX, b.nTkDichY, nSub, b.walk, 128);
	if (nW > 0)
	{
		// TOI NOI -> boc diem doanh trai khac o nhip sau
		b.nTkDichTick = 0;
		b.nTkDichX = 0;  b.nTkDichY = 0;
		b.walk.Reset();
	}
	else if (nW < 0)
	{
		// (21/08 phan bien) KHONG CO DUONG -> LUI LAI, KHONG quay vong. Truoc day
		// gop chung voi "toi noi" (nTkDichTick = 0) nen nhip ngay sau lai boc dich
		// moi -> A* lai -> -1 lai: 18 lan/giay cho moi bot ket, ma tu dot nay moi
		// vong con keo them mot BFS 729 o (pb_TkBocTrungGian). 50 bot ket o goc la
		// ~2,6 trieu pb_ODuoc + ~1.800 A* moi giay. Giu nTkDichTick = now de phai
		// cho het PB_TK_DICH_HAN moi boc lai; dich cu bo di de khoi co ai dung.
		b.nTkDichTick = now;
		b.nTkDichX = 0;  b.nTkDichY = 0;
		b.walk.Reset();
		{
			static unsigned int s_uLogCut = 0;
			if (now - s_uLogCut >= (unsigned int)(GAME_FPS * 5))
			{
				s_uLogCut = now;
				pb_Log("[BotLan] %s khong co duong toi dich -> nghi %d giay\n",
				       Player[nIdx].m_PlayerName, PB_TK_DICH_HAN / GAME_FPS);
			}
		}
	}
}

// Nhip Tong Kim: 1 giay/lan, goi canh pb_DemNgoai.
static void pb_TkNhip()
{
	static DWORD s_dwMoc = 0;
	const DWORD dwNow = GetTickCount();
	if (s_dwMoc && dwNow - s_dwMoc < 1000)
		return;
	s_dwMoc = dwNow;

	// [TK-SAN10] dung lai bang dem san-muc-tieu tu dau moi giay: tu don rac khi
	// bot chet / roi tran / doi pha ma khong can moc vao tung duong thoat.
	{
		memset(s_nTkSanDem, 0, sizeof(s_nTkSanDem));
		for (int i9 = 0; i9 < s_botCount; i9++)
			if (s_bots[i9].nTk == 4 && s_bots[i9].nTkSanIdx > 0
			 && s_bots[i9].nTkSanIdx < MAX_NPC)
				s_nTkSanDem[s_bots[i9].nTkSanIdx]++;
	}

	// [TK-CENSUS 28/08] 10 giay/lan: dem bot theo pha + mo xe toi da 3 con pha-3
	// dung lau nhat (chu game: "bot ket trong doanh trai, dung yen") - tran sau
	// doc bot.log la thay dich xac giai phau: o nao, doing gi, procAI, o co bi
	// CHAN theo luoi server khong, dang cho ChoRa hay chua mua thuoc.
	{
		static DWORD s_dwCen = 0;
		if (dwNow - s_dwCen >= 10000)
		{
			s_dwCen = dwNow;
			int nPha[6] = { 0, 0, 0, 0, 0, 0 };
			int nP3Map379 = 0, nP3MapKhac = 0;
			int aMoXe[3];
			int nMoXe = 0;
			for (int i8 = 0; i8 < s_botCount; i8++)
			{
				const int nTk8 = s_bots[i8].nTk;
				if (nTk8 < 0 || nTk8 > 5 || s_bots[i8].nPlayerIdx <= 0)
					continue;
				nPha[nTk8]++;
				if (nTk8 != 3)
					continue;
				const int nn8 = Player[s_bots[i8].nPlayerIdx].m_nIndex;
				if (nn8 <= 0 || nn8 >= MAX_NPC)
					continue;
				const int nSub8 = Npc[nn8].m_SubWorldIndex;
				if (nSub8 >= 0 && SubWorld[nSub8].m_SubWorldID == PB_TK_MAP)
					nP3Map379++;
				else
					nP3MapKhac++;
				if (nMoXe < 3)
					aMoXe[nMoXe++] = i8;
			}
			if (nPha[1] + nPha[2] + nPha[3] + nPha[4] + nPha[5] > 0)
			{
				pb_Log("[TkCensus] pha1=%d pha2=%d pha3=%d(379:%d khac:%d) pha4=%d pha5=%d\n",
				       nPha[1], nPha[2], nPha[3], nP3Map379, nP3MapKhac, nPha[4], nPha[5]);
				for (int m8 = 0; m8 < nMoXe; m8++)
				{
					PB_Bot& b8 = s_bots[aMoXe[m8]];
					const int p8 = b8.nPlayerIdx;
					const int nn8 = Player[p8].m_nIndex;
					if (nn8 <= 0 || nn8 >= MAX_NPC)
						continue;
					const int nSub8 = Npc[nn8].m_SubWorldIndex;
					int x8 = 0, y8 = 0;
					Npc[nn8].GetMpsPos(&x8, &y8);
					const unsigned int now8 = (nSub8 >= 0) ? SubWorld[nSub8].m_dwCurrentTime : 0;
					pb_Log("[TkCensus]  pha3: %s map=%d o(%d,%d) doing=%d procAI=%d obs=%d"
					       " choRa=%d mua=%d\n",
					       Player[p8].m_PlayerName,
					       (nSub8 >= 0) ? SubWorld[nSub8].m_SubWorldID : -1,
					       x8 / 32, y8 / 32, (int)Npc[nn8].m_Doing,
					       (int)(Npc[nn8].IsCanInput() ? 1 : 0),
					       (nSub8 >= 0) ? SubWorld[nSub8].CellObsSrv(x8, y8) : -9,
					       (int)(b8.nTkChoRa > now8 ? (b8.nTkChoRa - now8) / GAME_FPS : 0),
					       b8.nTkMuaXong);
				}
			}
		}
	}

	// Cong tac tat -> moi bot dang trong tran phai duoc TRA TRANG THAI dang
	// hoang, khong duoc bo lung mang camp chien truong ra the gioi.
	if (!s_nPbTongKim)
	{
		if (s_nPbTkDangMo)
		{
			for (int i = 0; i < s_botCount; i++)
			{
				if (!s_bots[i].nTk || s_bots[i].nTk == 5)
					continue;
				const int p = s_bots[i].nPlayerIdx;
				if (p > 0 && p < MAX_PLAYER && Player[p].m_dwID == s_bots[i].dwID)
					pb_TkTraTrangThai(p);
				s_bots[i].nTk = 5;
				s_bots[i].nTkTick = 0;
			}
			pb_Log("[BotTK] cong tac TAT -> tra trang thai cho moi bot dang trong tran\n");
			s_nPbTkDangMo = 0;
		}
		return;
	}

	{
		KMission* pMd = pb_TkMission(NULL);
		if (pMd)
		{
			s_nTkDemTong = (int)pMd->GetGroupPlayerCount(1);
			s_nTkDemKim  = (int)pMd->GetGroupPlayerCount(2);
		}
		else
		{
			s_nTkDemTong = 0;
			s_nTkDemKim  = 0;
		}
	}
	const int bMo = (pb_TkMission(NULL) != NULL) ? 1 : 0;
	if (bMo && !s_nPbTkDangMo)
	{
		s_nPbTkDangMo = 1;
		s_nPbTkGoiLai = 0;
		pb_Log("[BotTK] TRAN MO (mission %d tren map %d) -> goi quan\n",
		       PB_TK_MISSION, PB_TK_MAP);
		pb_TkGoiQuan();
	}
	else if (bMo && s_nPbTkGoiLai)
	{
		// (21/08) GOI QUAN BO SUNG giua tran. Can vi pb_TkGoiQuan chi chay o
		// dung khoanh khac tran chuyen dong->mo: chu game mo Tong Kim de TEST
		// roi moi bat tinh nang thi se khong con bot nao duoc goi. Cung dung khi
		// muon them quan giua tran (bot moi len cap 80, bot vua xong Da Tau...).
		// An toan de goi lai nhieu lan: pb_TkDuTuCach loai ngay bot co b.nTk != 0
		// nen bot dang trong tran khong bi dong den.
		s_nPbTkGoiLai = 0;
		pb_Log("[BotTK] GOI QUAN BO SUNG (lenh bai admin)\n");
		pb_TkGoiQuan();
	}
	else if (s_nPbTkGoiLai)
	{
		s_nPbTkGoiLai = 0;
		pb_Log("[BotTK] KHONG goi duoc: tran Tong Kim CHUA MO (mission %d tren map %d)\n",
		       PB_TK_MISSION, PB_TK_MAP);
	}
	else if (!bMo && s_nPbTkDangMo)
	{
		s_nPbTkDangMo = 0;
		pb_Log("[BotTK] TRAN DONG -> bot tu dung Than Hanh Phu len lai bai luyen\n");
		// task03 da tu tra trang thai + NewWorld ve 324 cho moi nguoi CON trong
		// mission; pha 5 chi con viec dung phu.
		for (int i = 0; i < s_botCount; i++)
			if (s_bots[i].nTk && s_bots[i].nTk != 5)
			{
				s_bots[i].nTk = 5;
				s_bots[i].nTkTick = 0;
			}
	}
}

// Cong tac cho lenh bai admin.
int PB_SetTongKim(int nBat)
{
	if (nBat >= 0)
	{
		s_nPbTongKim = nBat ? 1 : 0;
		if (s_nPbTongKim)
			s_nPbTkGoiLai = 1;   // bat GIUA tran van goi duoc quan (nhip sau)
		pb_Log("[BotTK] tinh nang bot tu tham gia Tong Kim: %s (tran bot = %s)\n",
		       s_nPbTongKim ? "BAT" : "TAT",
		       s_nPbTkTran > 0 ? "co gioi han" : "KHONG GIOI HAN");
	}
	return s_nPbTongKim;
}

// Lenh bai admin: goi quan NGAY (dung khi chu game mo Tong Kim de test).
// Tra ve so bot DU TU CACH tai thoi diem goi; viec goi that chay o nhip ke tiep
// (trong vong 1 giay) de dung chung mot duong voi luc toi gio tu dong.
int PB_TongKimGoi()
{
	if (!s_nPbTongKim)
		s_nPbTongKim = 1;          // tien tay bat luon, dung y chu game
	s_nPbTkGoiLai = 1;
	int nUng = 0;
	for (int i = 0; i < s_botCount; i++)
		if (pb_TkDuTuCach(s_bots[i]))
			nUng++;
	return nUng;
}

int LuaPB_TongKimGoi(Lua_State* L)
{
	Lua_PushNumber(L, PB_TongKimGoi());
	return 1;
}

int LuaPB_SetTongKim(Lua_State* L)
{
	int nBat = (Lua_GetTopIndex(L) >= 1) ? (int)Lua_ValueToNumber(L, 1) : -1;
	Lua_PushNumber(L, PB_SetTongKim(nBat));
	return 1;
}

// Tran so bot moi tran. 0 = KHONG GIOI HAN (chu game 21/08: "se mo gioi han cho
// bot vao tong kim thoai mai khong gioi han 55").
int PB_SetTongKimTran(int nTran)
{
	if (nTran >= 0)
	{
		s_nPbTkTran = nTran;
		pb_Log("[BotTK] tran so bot moi tran = %s\n",
		       nTran > 0 ? "co gioi han" : "KHONG GIOI HAN");
	}
	return s_nPbTkTran;
}

int LuaPB_SetTongKimTran(Lua_State* L)
{
	int nTran = (Lua_GetTopIndex(L) >= 1) ? (int)Lua_ValueToNumber(L, 1) : -1;
	Lua_PushNumber(L, PB_SetTongKimTran(nTran));
	return 1;
}

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

	// (21/08 - chu game: "moi lan bot nhay map thi dung toan bo ham di chuyen")
	// Moi lo trinh deu giu diem den bang TOA DO TUYET DOI cua ban do CU. Sang map
	// moi ma khong xoa thi bot phong thang ve toa do do tren ban do moi - dung
	// bai hoc 20/08 (b.chase giu waypoint chot tu cho vua chet, hoi sinh xong bot
	// chay nguoc ve nua ban do). Xoa MOI lo trinh, khong chi b.walk.
	if (b.nSubTruoc != nSub)
	{
		b.nSubTruoc = nSub;
		b.walk.Reset();   b.chase.Reset();  b.roam.Reset();
		b.follow.Reset(); b.loot.Reset();
		b.nTargetNpc = 0;
		b.nRoamX = 0;  b.nRoamY = 0;  b.nRoamTick = 0;
		b.nFollowNghiToi = 0;
		b.nJamTick = 0;  b.nLachToi = 0;
		b.nTkDichX = 0;  b.nTkDichY = 0;  b.nTkDichTick = 0;
	}

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
	// (21/08 - theo cay tham khao D:\USVOLAM KPlayer.cpp:1288) KICH THEO MAU chu
	// khong chi theo trang thai: ban cu chi vao day khi m_Doing la do_death /
	// do_revive, ma neu bot ket o mot trang thai KHAC voi mau <= 0 thi no nam
	// duoi dat vinh vien - dung canh "chet ve hau doanh bi nam duoi dat khong
	// phuc hoi" chu game gap.
	if (Npc[nNpcIdx].m_Doing == do_death || Npc[nNpcIdx].m_Doing == do_revive
	 || Npc[nNpcIdx].m_CurrentLife <= 0)
	{
		if (b.nChetTuTick == 0)
		{
			b.nChetTuTick = nowAll;
			// (phan bien 20/08) PHAI xoa CA NAM lo trinh, khong chi b.walk.
			// Tu khi waypoint duoc dong bang, mot lo trinh sot lai (thuong la
			// b.chase dang duoi quai) mang theo diem den CHOT TU CHO CHET. Hoi
			// sinh xong, neu bot van nham dung con quai do thi destTile/mapId
			// KHONG DOI -> B1 khong tinh lai duong -> bot phong thang ve phia
			// cho no vua chet, cach ca nua ban do. Ban cu tu khoi vi no kep lai
			// diem den theo vi tri MOI moi nhip; ban nay thi khong.
			b.walk.Reset();
			b.chase.Reset();
			b.roam.Reset();
			b.follow.Reset();
			b.loot.Reset();
			b.nTargetNpc = 0;
			b.nRoamX = 0;  b.nRoamY = 0;
			b.nFollowNghiToi = 0;
			b.nLachToi = 0;
		}
		// Cho 3 giay cho cai chet "kip dien ra" (client con dang ve), roi dung day.
		//
		// (21/08) KHONG con doi m_Doing == do_revive. Cay tham khao (KPlayer.cpp:
		// 1288-1293) kich theo m_CurrentLife < 0 chu khong theo trang thai, va do
		// la ly do ho khong dinh loi "nam duoi dat": bot ket o do_death se KHONG
		// BAO GIO thoa dieu kien do_revive, va lenh hoi sinh khong bao gio chay.
		// Cung theo ho: HA co chien dau sau khi hoi sinh (Npc[].m_FightMode =
		// enumFightNone o KPlayer.cpp:1291).
		// Va THU LAI moi 3 giay: mot lan goi that bai (diem hoi sinh hong, dang
		// doi map...) khong duoc phep lam ket bot den het tran.
		// (23/08 chu game: "bot chet roi ma van thay nam duoi dat vai giay sau moi
		// bien mat nhu tren hinh") Trong Tong Kim rut 3 giay con 1 giay: tran co
		// hang tram bot chet lien tuc, moi xac nam 3 giay la mat dat trai day xac.
		// 1 giay van du cho client ve xong don danh + hoat anh nga xuong, roi
		// Revive dua ve hau doanh (SetTempRevPos) va xac bien mat.
		else if (nowAll - b.nChetTuTick >= (unsigned int)(b.nTk ? GAME_FPS : GAME_FPS * 3))
		{
			b.nChetTuTick = nowAll;      // dat lai moc -> 3 giay nua thu tiep
			Player[nIdx].Revive(REMOTE_REVIVE_TYPE);
			Npc[nNpcIdx].m_FightMode = fight_none;
			pb_Log("[Bot] %s da chet (mau %d, doing %d) -> tu hoi sinh\n",
			       Player[nIdx].m_PlayerName, Npc[nNpcIdx].m_CurrentLife,
			       (int)Npc[nNpcIdx].m_Doing);
		}
		return;                          // dang chet thi khong lam viec gi khac
	}
	b.nChetTuTick = 0;

	// ---------------------------------------------------------------- TONG KIM
	// (21/08) Dat NGAY DAY: sau khoi tu hoi sinh (bot chet trong tran van phai
	// song lai) va TRUOC moi nhanh khac. Bon nhanh ben duoi deu keo bot RA KHOI
	// chien truong neu duoc chay: ban sap lech map (3 giay), ve thanh, Da Tau
	// DTB_TOI_NPC lech map (3 giay), pb_RaBai (0-60 giay so le).
	if (b.nTk)
	{
		// (21/08 chay that - chu game: "bot vao tong kim khong buff skill hay de
		// vong sang nhu khi dang di luyen cong", "tu bom mau day du nhu bot luyen
		// cong vay") Hai ham nay nam o CUOI pb_DriveBot, tuc SAU khoi Tong Kim co
		// "return" - nen bot vao tran la mat sach vong sang/buff va khong bao gio
		// uong thuoc. Goi thang o day de bot Tong Kim song y het bot luyen cong.
		pb_ApplyAuraBuff(nIdx, nNpcIdx, b, nowAll);
		pb_DungVatPham(nIdx, nNpcIdx, b, nowAll, (int)(&b - s_bots));
		pb_TkLai(nIdx, nNpcIdx, nSub, b, (int)(&b - s_bots));
		return;
	}

	// ---------------------------------------------------------------- NGOI BAN SAP
	// (19/08 chieu) bot duoc boc "ra thanh ban sap" (PB_SetBanSap): khong danh
	// quai / khong ra bai - dung sap THAT (m_BaiTan + hang co gia), nguoi choi
	// XEM + MUA qua dung duong TradeBuyItem cua nguoi that.
	if (b.nBanSap)
	{
		pb_BanSap(nIdx, nNpcIdx, nSub, b);
		return;
	}

	// ---------------------------------------------------------------- VE THANH THI
	// (19/08 toi #4) che do trang tri: keu het bot ve 15 map thanh/thon chia deu.
	// Sau nhanh SAP (bot sap giu nguyen sap - dang o thanh san), truoc moi nhanh
	// danh/Da Tau (may trang thai DT tam ngung, KHONG huy nhiem vu; luu dinh ky
	// van chay vi nam o PB_Breathe, khong qua ham nay).
	if (s_nPbVeThanh)
	{
		pb_VeThanh(nIdx, nNpcIdx, nSub, b, (int)(&b - s_bots));
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
				// (19/08 toi #3 - phan bien) 24 -> 48 ung vien (ban kinh 3..8 o):
				// tu khi ung vien phai qua ca luoi A* (CellObsSrv ben duoi), cho
				// dung sat mep vung rong co the BI het 24 o gan; xich bai lai don
				// bot day dac hon nen phai co van thoat xa hon. Vong xet o GAN
				// truoc, chi cham toi 6-8 o khi 3-5 o deu bi.
				// (20/08 - chu game: "cu di chuyen toi roi lui chay bay") LACH VE
				// PHIA DICH, khong lay o TRONG DAU TIEN nua. Ban cu quet 8 huong
				// theo thu tu xoay va nhan o hop le dau tien, nen huong lach hoan
				// toan khong lien quan den noi bot dang muon toi: log 20/08 bat
				// dung chuoi TruongAnh52 o(3185,5087) -> (3182,5087) -> (3185,5084)
				// -> (3185,5087), tuc lach ba nhip roi ve DUNG CHO CU. Nay: gom moi
				// o hop le roi lay o GAN DICH NHAT (dich = lenh do_run cuoi cung cua
				// lo trinh dang chay), nen moi lan lach deu la mot buoc TIEN.
				// (phan bien 20/08) LAY DICH GOC (destTileX/Y = o luoi cua dich lo
				// trinh), KHONG lay lastSendX: tu khi co pb_CatDoan, lastSendX chinh
				// la diem DA BI CAT NGAN - thuong chi cach bot mot o, nen "o gan dich
				// nhat" thoai hoa thanh "o gan chinh minh nhat" va bot lach quanh quan
				// tai cho. destTile la dich THAT cua chuyen di.
				// (phan bien 20/08) PHAI kiem ca mapId: mot lo trinh cu con sot cua
				// BAN DO KHAC (vd b.walk giu dich NPC Da Tau o thanh 37, khong ai
				// Reset khi doi pha) se lam bot lach mai ve mot goc ban do vo nghia.
				const int nMapNay = SubWorld[nSub].m_SubWorldID;
				int nDichX = 0, nDichY = 0;
				if (b.walk.destTileX > 0 && b.walk.mapId == nMapNay)
					{ nDichX = b.walk.destTileX * 32;   nDichY = b.walk.destTileY * 32;   }
				else if (b.roam.destTileX > 0 && b.roam.mapId == nMapNay)
					{ nDichX = b.roam.destTileX * 32;   nDichY = b.roam.destTileY * 32;   }
				else if (b.chase.destTileX > 0 && b.chase.mapId == nMapNay)
					{ nDichX = b.chase.destTileX * 32;  nDichY = b.chase.destTileY * 32;  }
				else if (b.follow.destTileX > 0 && b.follow.mapId == nMapNay)
					{ nDichX = b.follow.destTileX * 32; nDichY = b.follow.destTileY * 32; }
				// (phan bien 20/08) CO RIENG, khong dung nLachX == 0 lam co: cx co
				// the bang 0 that (jx thuoc 96..256 va lach ve huong -X o sat mep tay
				// ban do) - luc do logic "gan dich nhat" thoai hoa thanh "ung vien
				// cuoi cung", hoac bot khong lach du co o hop le.
				int bCoLach = 0;
				int nLachX = 0, nLachY = 0;
				__int64 nLachD = 0;
				for (int h = 0; h < 48; h++)
				{
					// + nLachDem*3 (buoc le): moi lan lach thu huong KHAC truoc,
					// khong con lach A->B roi 8 giay sau lach B->A vinh vien
					const int dir  = (nLech3 + b.nLachDem * 3 + h) & 7;
					const int cach = 3 + (h / 8);            // 3..8 o, gan truoc xa sau
					const int cx = jx + aLx[dir] * cach * 32;
					const int cy = jy + aLy[dir] * cach * 32;
					int nR = -1, nMX = 0, nMY = 0, nOX = 0, nOY = 0;
					SubWorld[nSub].Mps2Map(cx, cy, &nR, &nMX, &nMY, &nOX, &nOY);
					if (nR < 0)
						continue;
					if (SubWorld[nSub].m_Region[nR].GetBarrierMin(nMX, nMY, nOX, nOY, TRUE)
					 != Obstacle_NULL)
						continue;
					// (19/08 toi #3) PHAI kiem ca LUOI A*: cho nay truoc chi hoi engine,
					// ma engine coi vung rong du lieu la "di duoc" -> lach dam dong
					// SetPos nem thang bot vao vung rong/o luoi chan, [BotCuu] sau do
					// phai keo ve (bot.log 17:49-17:53: 3 bot khac nhau cung ket dung
					// o 1477,3023 canh bai Luong Thuy Dong - dau vet cua chinh cua nay).
					if (SubWorld[nSub].CellObsSrv(cx, cy) == 1)
						continue;
					if (nDichX == 0)
					{
						bCoLach = 1;  nLachX = cx;  nLachY = cy;   // khong biet dich -> giu net cu
						break;
					}
					const __int64 ddx = (__int64)cx - nDichX;
					const __int64 ddy = (__int64)cy - nDichY;
					const __int64 dd  = ddx * ddx + ddy * ddy;
					if (!bCoLach || dd < nLachD)
					{
						bCoLach = 1;  nLachD = dd;  nLachX = cx;  nLachY = cy;
					}
				}
				if (bCoLach)
				{
					// (20/08 - chu game: "nhin thay bot dang di chuyen thi keu nhu
					// TOC BIEN toi 1 doan") KHONG teleport nua.
					//
					// SetPos doi cho TUC THOI - do tu log 20/08: quang nhay trung
					// binh 6,4 o = 205 MPS, 51 luot/phut toan the gioi va don dung
					// vao cho dong (dieu kien kich hoat la "6 giay khong nhich noi
					// 3 o"), tuc dung cho nguoi choi dang dung. No con doi region
					// nen client phai go roi ve lai NPC - dung canh "toc bien".
					//
					// Nay PHAT LENH DI BO toi o do: nguoi xem thay bot lach ngang
					// mot buoc nhu nguoi that. O dich da qua GetBarrierMin(bCheckNpc
					// = TRUE) nen khong co ai dung san o do.
					//
					// (21/08 - chu game: "bo van nhay SetPos cua lach thay bang di
					// chuyen") HET VAN CUOI. Ly do giu SetPos truoc day la "JX1 tinh NPC
					// LA TUONG, giua dam dong dac di bo khong nhuc nhich" - tu khi cong
					// tac g_nPbNpcChan tat (nguoi/bot/quai dung chong len nhau) ly do do
					// khong con; do 09:33-09:44 van 129 cu nhay/11 phut = canh "bot toc
					// bien" cho nguoi xem. Nay LUON di bo lach ngang; ket tuong that thi
					// [BotCuu] (map bai) / che do Ve-Thanh (GM) do.
					{
						static unsigned int s_uLachLog = 0;
						if (nowAll - s_uLachLog >= (unsigned int)(GAME_FPS / 3))
						{
							s_uLachLog = nowAll;
							pb_Log("[BotLach] %s map=%d ket dam dong o(%d,%d) -> di bo lach sang o(%d,%d)\n",
							       Player[nIdx].m_PlayerName, nMapNay, jx / 32, jy / 32,
							       nLachX / 32, nLachY / 32);
						}
					}
					{
						// (phan bien 20/08) NHANH DI BO TUYET DOI KHONG DUOC Reset
						// va phai KHOA lo trinh mot lat.
						// m_Command cua KNpc chi co MOT khe: khoi lach nay khong
						// "return", nen ngay trong CUNG NHIP do luong chay tiep
						// xuong pb_RaBai / bam doi truong / pb_Roam, ma ta vua
						// Reset het lo trinh nen PB_WalkTo se tinh A* moi roi phat
						// mot do_run KHAC DE LEN - lenh lach ngang khong bao gio
						// duoc thi hanh.
						Npc[nNpcIdx].SendCommand(do_run, nLachX, nLachY);
						b.nLachToi = nowAll + (unsigned int)GAME_FPS;   // giu 1 giay
					}
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
		// (20/08 - chu game: "bot khi goi ve thanh roi chay len map luyen cong lai
		// cu di chuyen toi roi lui chay bay roi moi toi xa phu") DANG TREN DUONG
		// RA BAI thi KHONG bam theo doi truong. Khoi follow nay nam TRUOC pb_RaBai
		// va co "return;" rieng, nen mot thanh vien cach doi truong hon 200 MPS
		// KHONG BAO GIO xuong toi pb_RaBai: no chay ve phia doi truong, vao trong
		// 200 MPS thi nhip sau moi duoc nhich ve phia NPC Xa Phu, di vai buoc lai
		// vuot 200 MPS va bi keo nguoc - dung canh "di toi roi lui" chu game thay.
		// pb_RaBai co san pb_RoiNhom o ca hai pha, nhung phai chay den do da.
		// (phan bien 20/08) DANG DI BO LACH NGANG -> de yen cho lenh do_run kip
		// thi hanh. Neu chay tiep xuong duoi, PB_WalkTo cua nhanh ke se tinh duong
		// roi phat mot do_run KHAC de len (m_Command chi co MOT khe) va cu lach
		// vua phat coi nhu vut di. Trong luc giu, bot van danh/nhat binh thuong o
		// nhip sau vi han chi 1 giay.
		if (b.nLachToi && nowAll < b.nLachToi)
			return;
		b.nLachToi = 0;

		// (phan bien 20/08) DIEU KIEN PHAI LA "DA O DUNG BAI", khong phai "dang
		// tren duong ra bai": bot chua duoc cap bai (nBaiIdx = -1 luc khoi tao khe,
		// hoac vua bi nhanh KHONG-VAO-DUOC dat lai -1) cung KHONG duoc phep ket
		// trong vong follow - pb_ChonBai chi chay BEN TRONG pb_RaBai, ma pb_RaBai
		// nam SAU khoi nay, nen no se khong bao gio duoc cap bai.
		// Bot duoi cap 10 (chua co bai) van bam theo doi truong nhu cu.
		const int bODungBai = (b.nBaiIdx >= 0 && b.nBaiIdx < PB_SO_BAI
		                    && SubWorld[nSub].m_SubWorldID == s_bai[b.nBaiIdx].nMapId);
		const int bChuaRaBai = (Npc[nNpcIdx].m_Level >= 10 && !bODungBai);
		// (phan bien 20/08) "la thanh vien VA doi truong DANG O CUNG BAN DO".
		// pb_ChonBai chia bai theo TUNG bot, khong biet to doi, nen doi truong va
		// thanh vien rat de o hai map khac nhau. Khoi GIAN DAN ben duoi mien tru
		// moi thanh vien to doi (vi truoc day ho bam theo doi truong); cong voi
		// dieu kien follow moi thi mot thanh vien lac doi truong se KHONG follow
		// ma cung KHONG duoc tan dan -> dam mot cho o diem dap vinh vien.
		int bTheoTruong = 0;
		if (pb_TrongNhom(nIdx) && pb_DoiTruongCua(nIdx) != nIdx)
		{
			const int nCapT = pb_DoiTruongCua(nIdx);
			const int nCapN = (nCapT > 0 && nCapT < MAX_PLAYER) ? Player[nCapT].m_nIndex : 0;
			if (nCapN > 0 && nCapN < MAX_NPC && Npc[nCapN].m_SubWorldIndex == nSub)
				bTheoTruong = 1;
		}
		if (pb_TrongNhom(nIdx) && pb_DoiTruongCua(nIdx) != nIdx && !b.nDaTauChon
		 && !bChuaRaBai)
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
			 && !pb_BotNoiThat(nIdx)   // [BotNoi-PHAI] chi bot noi THAT (phai co chieu noi) moi mien
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

			// -- (20/08) CUU CHUNG: bot nam NGOAI CUA SO LUOI tren BAT KY ban do --
			// Nhanh [BotCuu] ben duoi CHI chay khi bot dang dung dung ban do BAI
			// cua no (can diem neo de tha ve). Nhung bot lot ra ngoai map trong
			// THANH hoac tren duong di thi khong co neo nao ca - va do that 20/08
			// cho thay chung ton tai. Ke tu ban nay pb_CatDoan con CHAN moi lenh
			// di cua bot ngoai luoi (dung, de no khong chay xa them), nen neu
			// khong co duong cuu nay thi no dung im vinh vien.
			// Cach cuu: quet xoan oc quanh cho dung, lay o DAU TIEN ma LUOI bao
			// di duoc (CellObsSrv == 0, khong nhan -1) roi SetPos ve do.
			if (SubWorld[nSub].CoLuoiSrv())
			{
				int nOx = 0, nOy = 0;
				Npc[nNpcIdx].GetMpsPos(&nOx, &nOy);
				if (SubWorld[nSub].CellObsSrv(nOx, nOy) < 0)
				{
					static const int aVx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
					static const int aVy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
					int nVx = 0, nVy = 0;
					for (int r = 2; r <= 128 && !nVx; r += 2)
						for (int h = 0; h < 8 && !nVx; h++)
						{
							const int cx = nOx + aVx[h] * r * 32;
							const int cy = nOy + aVy[h] * r * 32;
							if (cx <= 32 || cy <= 32)                       continue;
							if (SubWorld[nSub].CellObsSrv(cx, cy) != 0)     continue;
							int nRr = -1, nMXr = 0, nMYr = 0, nOXr = 0, nOYr = 0;
							SubWorld[nSub].Mps2Map(cx, cy, &nRr, &nMXr, &nMYr, &nOXr, &nOYr);
							if (nRr < 0)                                    continue;
							if (SubWorld[nSub].m_Region[nRr].GetBarrierMin(nMXr, nMYr, nOXr, nOYr, FALSE)
							    != Obstacle_NULL)                           continue;
							nVx = cx;  nVy = cy;
						}
					if (nVx)
					{
						pb_Log("[BotNgoai] CUU %s map=%d dang o o(%d,%d) NGOAI cua so luoi"
						       " -> keo ve o(%d,%d)\n", Player[nIdx].m_PlayerName,
						       SubWorld[nSub].m_SubWorldID, nOx / 32, nOy / 32,
						       nVx / 32, nVy / 32);
						Npc[nNpcIdx].SetPos(nVx, nVy);
						b.nTargetNpc = 0;
						b.nRoamX = 0;  b.nRoamY = 0;
						b.walk.Reset();  b.chase.Reset();  b.roam.Reset();
						b.follow.Reset();  b.nFollowNghiToi = 0;  b.loot.Reset();
						b.nJamTick = 0;
					}
				}
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
				// (20/08 do that) T1b - BOT NAM NGOAI HAN CUA SO LUOI. T1 doi
				// CellObsSrv == 1, ma o ngoai cua so luoi thi ham tra -1 chu khong
				// phai 1, nen con NGOAI MAP - dung con chu game thay - KHONG BAO GIO
				// duoc T1 cuu. Do bot.log 9 phut sau restart 11:43: 23 luot bot dung
				// o toa do nam ngoai han cua so luoi cua ban do minh dang o.
				if (!nKet && SubWorld[nSub].CoLuoiSrv()
				 && SubWorld[nSub].CellObsSrv(nKcx, nKcy) < 0)
					nKet = 3;
				// T2: 5 lan roam lien tiep khong co duong = ket dao A*
				if (!nKet && b.nAStarThua >= 5)
					nKet = 2;

				if (nKet)
				{
					const PB_BaiLuyen& baiC = s_bai[b.nBaiIdx];
					// (19/08 toi #4) diem ve = NEO DA KIEM lien thong (pb_LayNeo) -
					// neo tho cua Lao Ho Dong nam canh dao luoi lam [BotCuu] tha bot
					// vao dao roi 60 giay sau cuu lai dung cho do, xoay vong vinh vien
					int nNeoCX = baiC.nOX, nNeoCY = baiC.nOY;
					pb_LayNeo(b.nBaiIdx, nSub, &nNeoCX, &nNeoCY);
					// diem ve: dat chan bai + o lech rieng (khuon 9x7 cua pb_RaBai)
					int nVeX = nNeoCX * 32 + ((nLech % 9) - 4) * 2 * 32;
					int nVeY = nNeoCY * 32 + (((nLech / 9) % 7) - 3) * 3 * 32;
					{
						int nRv = -1, nMXv = 0, nMYv = 0, nOXv = 0, nOYv = 0;
						SubWorld[nSub].Mps2Map(nVeX, nVeY, &nRv, &nMXv, &nMYv, &nOXv, &nOYv);
						if (nRv < 0
						 || SubWorld[nSub].m_Region[nRv].GetBarrierMin(nMXv, nMYv, nOXv, nOYv, TRUE)
						    != Obstacle_NULL)
						{
							nVeX = nNeoCX * 32;   // o lech hong -> ve dung diem neo
							nVeY = nNeoCY * 32;
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
						     : (nKet == 3) ? "NGOAI HAN CUA SO LUOI (ra ngoai map that)"
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
						b.nBuocRaX = nNeoCX * 32 + nGocX2 * nXa2;
						b.nBuocRaY = nNeoCY * 32 + nGocY2 * nXa2;
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
			      && !bTheoTruong)   // (20/08) chi mien tru khi doi truong o CUNG map
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
					// (19/08 toi #4) di tan uu tien DIEM VANG BOT nhat trong xich
					if (pb_FindRoamSpot(nIdx, nNpcIdx, nSub, b, nLech, 1, &rx4, &ry4))
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

		// Tieu luon so diem dang ton (nhan vat mau co the da tich san). Moi lan LEN CAP
		// sau do thi pb_TrangBiTheoCap goi lai pb_AllocAttribPoints (dot 19/08 chieu) -
		// chu thich cu ghi "nhanh PB_AI_IN_FACTION ben tren se tieu tiep" la SAI,
		// nhanh do KHONG TON TAI, va do la ly do bot cap 82 chi co 440 HP.
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
// ===========================================================================
// (20/08 - chu game: "van con bi chay ra khoi map, ban keo log ve xem")
// BO DEM BOT NGOAI MAP.
//
// Vi sao phai co: cac dong log san co ([BotLach], [BotKet], [BotDanh]...) KHONG
// in ban do, nen khi doi chieu voi luoi phai doan ban do cua tung bot theo dong
// gan nhat co ghi map - va cach do SAI: do that 20/08 co ba bot dung CHUNG mot
// toa do lai bi gan ba ban do khac nhau. Khong the sua goc bang so lieu doan.
//
// Ham nay dem tu NGUON SU THAT: duyet moi khe bot con song, hoi thang
// Npc[].m_SubWorldIndex + GetMpsPos + CellObsSrv. 1000 bot x mot phep tra mang,
// 30 giay mot lan - khong dang ke.
// ===========================================================================
static void pb_DemNgoai()
{
	static DWORD s_dwMocDem = 0;
	const DWORD dwNow = GetTickCount();
	if (s_dwMocDem && dwNow - s_dwMocDem < 30000)
		return;
	s_dwMocDem = dwNow;

	int nTrong = 0, nChan = 0, nNgoai = 0, nKhongLuoi = 0, nVd = 0;
	int nLac = 0, nVdLac = 0;
	int aMap[64], aNgoai[64], aChan[64], nMap = 0;
	for (int q = 0; q < s_botCount; q++)
	{
		const int p = s_bots[q].nPlayerIdx;
		if (p <= 0 || p >= MAX_PLAYER || Player[p].m_dwID != s_bots[q].dwID)
			continue;
		const int n = Player[p].m_nIndex;
		if (n <= 0 || n >= MAX_NPC)
			continue;
		const int sub = Npc[n].m_SubWorldIndex;
		if (sub < 0 || sub >= MAX_SUBWORLD)
			continue;
		if (!SubWorld[sub].CoLuoiSrv())
		{
			nKhongLuoi++;
			continue;
		}
		int x = 0, y = 0;
		Npc[n].GetMpsPos(&x, &y);
		const int v = SubWorld[sub].CellObsSrv(x, y);
		if (v == 0)
		{
			nTrong++;
			// (21/08 - chu game: "van con bot ra ngoai map ban keo log ve")
			// CENSUS BOT LAC: o dang dung MO tren luoi nen moi bo dem cu deu bao
			// "on", nhung cach MOI tam cum diem sinh cua ban do qua
			// PB_XICH_CUM_THA = dung giua "vung trong gia" nhin-nhu-ngoai-map -
			// dung nhung con chu game thay bang mat ma khong dong log nao goi
			// ten. Chi xet ban do DA co bang cum (khong tu kich build de khoi
			// ton cong o thanh/thon von khong co quai -> khong bao gio bi tinh).
			if (s_cumTinh[sub] && s_cum[sub].nSo > 0)
			{
				const PB_CumMap* pLc = &s_cum[sub];
				int nMinC = 0x7fffffff;
				for (int c2 = 0; c2 < pLc->nSo; c2++)
				{
					int dxc = x - pLc->aX[c2]; if (dxc < 0) dxc = -dxc;
					int dyc = y - pLc->aY[c2]; if (dyc < 0) dyc = -dyc;
					const int dc = (dxc > dyc) ? dxc : dyc;
					if (dc < nMinC) nMinC = dc;
				}
				if (nMinC > PB_XICH_CUM_THA)
				{
					nLac++;
					if (nVdLac < 6)
					{
						nVdLac++;
						pb_Log("[BotLac] VD%d %s map=%d o(%d,%d) cach cum gan nhat"
						       " %d o (ai=%d bai=%d dtPha=%d doing=%d target=%d)\n",
						       nVdLac, Player[p].m_PlayerName,
						       SubWorld[sub].m_SubWorldID, x / 32, y / 32,
						       nMinC / 32, s_bots[q].nAi, s_bots[q].nBaiIdx,
						       s_bots[q].nDtPha, (int)Npc[n].m_Doing,
						       s_bots[q].nTargetNpc);
					}
				}
			}
			continue;
		}
		const int bNgoai = (v < 0);
		if (bNgoai) nNgoai++; else nChan++;
		const int mid = SubWorld[sub].m_SubWorldID;
		int k = -1;
		for (int i = 0; i < nMap; i++)
			if (aMap[i] == mid) { k = i; break; }
		if (k < 0 && nMap < 64)
		{
			k = nMap++;
			aMap[k] = mid;  aNgoai[k] = 0;  aChan[k] = 0;
		}
		if (k >= 0) { if (bNgoai) aNgoai[k]++; else aChan[k]++; }
		if (nVd < 8)
		{
			nVd++;
			pb_Log("[BotNgoai] VD%d %s map=%d o(%d,%d) region(%d,%d) = %s\n", nVd,
			       Player[p].m_PlayerName, mid, x / 32, y / 32, x / 512, y / 1024,
			       bNgoai ? "NGOAI CUA SO LUOI" : "trong O BI CHAN");
		}
	}
	pb_Log("[BotNgoai] tk: %d o trong | %d trong O CHAN | %d NGOAI cua so luoi"
	       " | %d dang o ban do khong co luoi | %d LAC xa moi cum\n",
	       nTrong, nChan, nNgoai, nKhongLuoi, nLac);
	for (int i = 0; i < nMap; i++)
		pb_Log("[BotNgoai]   map %d: %d ngoai luoi, %d trong o chan\n",
		       aMap[i], aNgoai[i], aChan[i]);
}

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
				// (21/08) doi truong dang Tong Kim khong duoc nhan them thanh vien
				// (xem chu thich cung ngay o ve ung vien tu do)
				if (!bChet && !b.nTk && b.nAi == PB_AI_FIGHT
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
		// (21/08) bot dang Tong Kim KHONG duoc ghep nhom: KPlayerTeam::GetInviteReply
		// (KPlayerTeam.cpp:466) GHI DE m_CurrentCamp cua thanh vien = m_Camp GOC
		// cua doi truong -> xoa sach phe Tong/Kim vua SetCurCamp, hai bot khac phe
		// cung nhom thanh DONG MINH khong danh nhau duoc.
		if (bChet || b.nTk || !b.nWantParty || b.nAi != PB_AI_FIGHT
		 || b.nDaTauChon || b.nBanSap)
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
		pb_DemNgoai();      // (20/08) do dem bot ngoai map, 30 giay/lan
		pb_TkNhip();        // (21/08) canh gio Tong Kim + goi quan, 1 giay/lan
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
