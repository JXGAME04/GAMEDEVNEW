// KSimCity.cpp - xem KSimCity.h. Port SimCity (bot nguoi choi gia lap) JX2 -> JX1.
//
// LUU Y: Core build voi PCH "Use" qua KCore.h - moi thu TRUOC dong include nay
// deu bi compiler bo qua, nen KCore.h PHAI dung dau tien (giong KJx2WarInfra.cpp).
#include "KCore.h"
#include "KWin32.h"

#ifdef _SERVER

#include "LuaLib.h"
#include "KNpc.h"           // KNpc, Npc[], MAX_NPC, do_walk, GAME_FPS
#include "KNpcSet.h"        // NpcSet
#include "KSubWorld.h"      // SubWorld[], TestBarrier, MAX_SUBWORLD
#include "KSubWorldSet.h"
#include "KRegion.h"        // KRegion::IsActive
#include "KTabFile.h"       // KTabFile - nap _preset.txt / chat.txt
#include "KPlayerChat.h"    // KPlayerChat::NpcChat - bot noi chuyen
#include "KItemChangeRes.h" // g_ItemChangeRes - dat ngoai hinh mac dinh cho bot
#include "KFaction.h"       // g_Faction.GetCamp - mon phai -> camp
#include "KRandom.h"        // g_Random
#include "KSimCity.h"
#include <string.h>         // strncpy, strcmp, memset
#include <stdio.h>          // sscanf, sprintf

// ============================================================================
// GD1 Fix1 - co danh dau bot giu can bang (xem KSimCity.h)
// ============================================================================

int LuaSC_SetBotFlag(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	int nVal    = (int)Lua_ValueToNumber(L, 2);
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Npc[nNpcIdx].m_btSimCityBot = (BYTE)(nVal ? 1 : 0);
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaSC_GetBotFlag(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, Npc[nNpcIdx].m_btSimCityBot);
	return 1;
}

// ============================================================================
// GD2 - SO BOT + BANG LO TRINH + DI CHUYEN (driver C++ hook vao CoreServerShell::Breathe)
//
// Co che (doc tu ma, KHONG doan):
//   SendCommand(do_walk, mpsX, mpsY) PUBLIC (Goto/RunTo private) = ban-roi-quen:
//   phat 1 lan/chang -> ProcCommand(nAI=1) -> Goto; moi tick engine tu buoc (ServeMove),
//   ne vat can cuc bo (KNpcFindPath::GetDir, men-tuong, KHONG co A* o server).
//   Bot kind_player DUNG giu m_ProcessAI=1 (OnStand KHONG ha) -> SendCommand tac dung NGAY.
//   DoWalk broadcast s2c_npcwalk TRUOC early-return -> CHI phat khi doi chang (chong spam).
//   Bot chi buoc khi region NONG (co nguoi choi gan) - KRegion nong qua AddPlayer, khong AddNpc.
//
// HAI KIEU KET (deu do doc ma, xem chu thich tai cho trong SC_Breathe):
//   Kieu A - m_Doing == do_stand : ServeMove nhanh _SERVER (KNpc.cpp:4422-4433) goi DoStand()
//            cho MOI gia tri GetDir != 1 (ke ca -1). Day la kieu ket thuong gap.
//   Kieu B - m_Doing == do_walk ma vi tri DUNG YEN : ServeMove buoc xong, o moi roi vao
//            region -1 nen KHOI PHUC vi tri cu roi return (KNpc.cpp:4498-4507); m_Doing VAN
//            la do_walk -> lap vinh vien o bien region/map. Bat bang bo dem nStuck.
// ============================================================================

#define SC_MAX_BOTS     4096
#define SC_MAX_ROUTE    64      // so lo trinh (chia se giua nhieu bot)
#define SC_MAX_NODE     320     // so waypoint moi lo trinh (tuyen dai nhat that = 237, du bien)
#define SC_ARRIVE_MPS   48      // toi chang khi trong 48 MPS; phai > buoc/tick (ServeMove kep <=31)
#define SC_MAX_RETRY    3       // so lan ne ngang truoc khi bo chang
#define SC_RETRY_CD     3       // tick cho giua 2 lan ne (~0.17s)
#define SC_STUCK_TICK   9       // do_walk + dung yen >= 9 tick (0.5s) khi region NONG = ket kieu B
#define SC_GIVEUP_STOP  3       // bo cuoc lien tiep 3 chang -> dung han bot (khoi dot CPU/goi tin)
#define SC_SIDE_MPS     96      // do lech ngang khi ne (3 o)

// ---- SC_STEP_MPS: HANG SO QUAN TRONG NHAT CUA CA BAN VA ----
// KHONG bao gio phat thang node xa, ma phat tung buoc ngan huong ve node.
//
// LY DO (doc tu KNpcFindPath.cpp, day la GOC RE THAT cua "dinh goc"):
//   Dong 106-111:  m_nFindTimes++;  if (m_nFindTimes > 1) { m_nFindTimes = 0; return 0; }
//   Truy het cac nhanh:
//     - buoc di duoc (:75-80) chi dat lai m_nFindState = 0, KHONG dung m_nFindTimes;
//     - vao men tuong (:120-121, :137-138, :149-150) dat m_nFindTimer = 0, KHONG dung m_nFindTimes;
//     - MOI nhanh bo cuoc (:156, :166, :194, :219) deu co "//m_nFindTimes = 0;" BI COMMENT;
//     - CHI nhanh DOI DICH (:60-67) moi xoa sach m_nFindTimer/State/Times.
//   => Engine cho DUNG MOT lan ne vat can cho MOI lenh SendCommand. Vat can THU HAI cua cung
//      mot lenh (du cach do 15 o, du con thua huong trong) -> return 0 -> ServeMove nhanh
//      _SERVER (KNpc.cpp:4422-4432) goi DoStand() -> m_Doing = do_stand -> OnWalk khong con
//      duoc goi -> BOT DUNG IM VINH VIEN.
//   Do that: chang preset trung vi 19,9 o (638 MPS) - mot chang xuyen pho chac chan gap hon
//   mot vat can, nhat la vi JX1 tinh NPC LA TUONG khi buoc sang o moi (KSubWorld.cpp:1471-1476
//   truyen bCheckNpc=TRUE -> KRegion.cpp:983-987 tra Obstacle_JumpFly), trong khi JX2 goc TAT
//   lop nay bang IsCheckNpcBarrier=0 (JX1 khong he co cong tac do: grep 0 ket qua).
//   => Lo trinh duoc ve trong the gioi NPC-trong-suot nay chay giua rung tuong NPC.
//
//   Ngan sach men tuong cung nho: MAX_FIND_TIMER=30 khung x 5 MPS/khung (m_CurrentWalkSpeed=5,
//   KNpc.cpp:190 / KNpcSet.cpp:105) = 150 MPS = 4,7 o.
//
// => Phat lai moi 128 MPS (4 o) = nap lai tin dung ne moi 4 o. Chi phi: mot goi s2c_npcwalk
//    moi ~26 khung (1,4 s) cho moi bot DANG DI. Tang so nay = it goi hon nhung de ket hon.
// CANH BAO: diem trung gian PHAI qua TestBarrier truoc khi phat. Neu diem den nam trong tuong
//    thi khi bot vao trong 64 MPS, KNpcFindPath.cpp:100-104 bo cuoc VINH VIEN cho lenh do
//    (CheckDistance tra TRUE khi XA, nen !CheckDistance = GAN).
#define SC_STEP_MPS     128     // do dai mot buoc trung gian (4 o)
#define SC_BOX_HALF     320     // ban kinh mac dinh patrol box (10 o)
#define SC_SNAP_MPS     1024    // node gan nhat xa hon 32 o -> tu choi gan lo trinh

struct SC_Route
{
	int  nCount;
	int  wpX[SC_MAX_NODE];      // toa do MPS (da x32)
	int  wpY[SC_MAX_NODE];
	int  nOwner;                // >0 = patrol box rieng cua bot nay; -1 = tuyen preset dung chung
	char szFile[80];            // khoa khu trung lap (duong dan preset)
	char szPath[40];            // khoa khu trung lap (ten tuyen)
};

struct SC_Bot
{
	int   nNpcIdx;             // slot Npc[]
	DWORD dwID;                // chot danh tinh (chong reuse slot)
	int   nRouteId;            // chi so vao s_routes[]; -1 = chua co lo trinh / da dung han
	int   nLeg;                // chang hien tai
	int   nDir;                // +1 / -1 (khi khong khep vong)
	int   bLoop;               // 1 = khep vong (%nCount); 0 = quay dau
	int   nLastX, nLastY;      // vi tri MPS lan truoc (biet bot co nhuc nhich khong)
	int   nRetry;              // so lan da ne ngang cho chang nay
	int   nRetryCd;            // tick con phai cho truoc khi ne lan nua
	int   nStuck;              // so tick lien tiep do_walk ma dung yen (ket kieu B)
	int   nGiveUp;             // so chang lien tiep phai bo cuoc
	int   nSide;               // +1 / -1 : ben ne ngang lan toi (doi ben moi lan)
	int   nTgtX, nTgtY;        // dich DA PHAT (diem trung gian hoac chinh node) - xem SC_STEP_MPS
};

static SC_Route s_routes[SC_MAX_ROUTE];
static int      s_routeCount   = 0;
static SC_Bot   s_bots[SC_MAX_BOTS];
static int      s_botCount     = 0;
static int      g_bSimCityMove = 0;   // cong tac driver di chuyen

// ---- GD3: bot noi chuyen ----
// NpcChat KHONG tu kep do dai; tran that nam o CLIENT: KPlayerChat.cpp:673 szBuf[256] va
// KProtocolProcess.cpp:1105 tmpMsg[256]. Chot 64 byte (giong ScriptFuns.cpp:6104) la LA CHAN
// CHONG SAP CLIENT, phai giu. chat.txt that: cau dai nhat 56 byte.
#define SC_MAX_CHAT    512     // so cau thoai giu trong bo nho
#define SC_CHAT_LEN    68      // 64 byte + NUL, du bien

static char s_chat[SC_MAX_CHAT][SC_CHAT_LEN];
static int  s_chatCount  = 0;
static int  s_chatChance = 0;   // n / 1000 moi giay moi bot; 0 = tat
static int  s_tick       = 0;   // dem tick de coi nhip 1 giay (GAME_FPS = 18)
static char s_chatFile[80] = "";  // khoa chong nap trung (LoadChat cong don)
static char s_chatType[40] = "";

// ============================================================================
// Tien ich hinh hoc / ban do
// ============================================================================

// Khoang cach binh phuong bang __int64.
// LY DO PHAI 64-BIT (do that tren du lieu da ship): 183_ngudoc_preset.txt cho dist2 = 2,642e9
// va 154_thuyyen_preset.txt cho 2,019e9, deu VUOT INT_MAX = 2,147e9. Tran ra so AM se lot
// qua phep so "<= SC_ARRIVE_MPS^2" -> bot tuong TOI DICH moi tick -> tien chang moi tick ->
// SendCommand -> DoWalk broadcast s2c_npcwalk 18 goi/giay/bot. Day chinh la trieu chung "lag".
static __int64 sc_Dist2(int x1, int y1, int x2, int y2)
{
	__int64 dx = (__int64)x1 - (__int64)x2;
	__int64 dy = (__int64)y1 - (__int64)y2;
	return dx * dx + dy * dy;
}

// O (x,y) co di duoc khong. TestBarrier tra 0xff khi ngoai vung nap (KSubWorld.cpp:1268-1286).
static int sc_Walkable(int nSubWorld, int nMpsX, int nMpsY)
{
	if (nSubWorld < 0 || nSubWorld >= MAX_SUBWORLD)
		return 0;
	if (nMpsX <= 0 || nMpsY <= 0)
		return 0;
	return SubWorld[nSubWorld].TestBarrier(nMpsX, nMpsY) == 0;
}

// Region cua bot co NONG khong (m_nActive chi tang o KSubWorld::AddPlayer = nguoi choi that).
// Region NGUOI => KSubWorld::Activate bo qua NPC (KSubWorld.cpp:900-905) => ProcCommand khong
// chay => bot nam yen o do_stand. KHONG duoc doc trang thai do_stand do la "engine bo cuoc".
static int sc_RegionHot(int i)
{
	int sw = Npc[i].m_SubWorldIndex;
	int ri = Npc[i].m_RegionIndex;
	if (sw < 0 || sw >= MAX_SUBWORLD || ri < 0)
		return 0;
	if (ri >= SubWorld[sw].m_nTotalRegion)
		return 0;
	return SubWorld[sw].m_Region[ri].IsActive() ? 1 : 0;
}

// Bot dang bat dong HOP LE (bi khong che / doi server) - khong duoc tinh la ket.
//   m_StunState  : ProcessState tra TRUE -> Activate return TRUOC ProcStatus (KNpc.cpp:576-580)
//   m_FreezeState: dong bang tick chan/le (KNpc.cpp:1161-1168)
//   m_FrozenAction: SendCommand NUOT IM LANG moi do_walk (KNpc.cpp:4647-4661)
//   m_CurrentWalkSpeed <= 0 : ServeMove return ngay, giu nguyen do_walk (KNpc.cpp:4369-4370)
static int sc_Immobilized(int i)
{
	KNpc* p = &Npc[i];
	if (p->m_StunState.nTime    > 0) return 1;
	if (p->m_FreezeState.nTime  > 0) return 1;
	if (p->m_FrozenAction.nTime > 0) return 1;
	if (p->m_CurrentWalkSpeed  <= 0) return 1;
	if (p->m_bExchangeServer)        return 1;
	return 0;
}

// ---- quan ly so bot ----
static int sc_Find(int nNpcIdx)
{
	for (int r = 0; r < s_botCount; r++)
		if (s_bots[r].nNpcIdx == nNpcIdx)
			return r;
	return -1;
}

static void sc_Register(int nNpcIdx)
{
	int r = sc_Find(nNpcIdx);
	if (r < 0)
	{
		if (s_botCount >= SC_MAX_BOTS)
			return;
		r = s_botCount++;
	}
	memset(&s_bots[r], 0, sizeof(SC_Bot));
	s_bots[r].nNpcIdx  = nNpcIdx;
	s_bots[r].dwID     = Npc[nNpcIdx].m_dwID;
	s_bots[r].nRouteId = -1;
	s_bots[r].nDir     = 1;
	s_bots[r].nSide    = 1;
}

static void sc_Deregister(int nNpcIdx)
{
	int r = sc_Find(nNpcIdx);
	if (r < 0)
		return;
	s_bots[r] = s_bots[--s_botCount];   // nen mang
}

// tien chang (khep vong / quay dau), kep an toan chi so
static void sc_NextLeg(SC_Bot& b, SC_Route& rt)
{
	b.nLeg += b.nDir;
	if (b.bLoop)
	{
		if (b.nLeg >= rt.nCount) b.nLeg = 0;
		if (b.nLeg < 0)          b.nLeg = rt.nCount - 1;
	}
	else
	{
		if (b.nLeg >= rt.nCount) { b.nLeg = rt.nCount - 2; b.nDir = -1; }
		else if (b.nLeg < 0)     { b.nLeg = 1;             b.nDir = 1;  }
	}
	if (b.nLeg < 0)          b.nLeg = 0;
	if (b.nLeg >= rt.nCount) b.nLeg = 0;
}

// Phat MOT BUOC NGAN huong ve node cua chang hien tai (khong bao gio phat thang node xa).
// Moi lan phat = mot dich MOI = engine nap lai tin dung ne vat can (KNpcFindPath.cpp:60-67).
// Xem khoi chu thich SC_STEP_MPS de biet vi sao day la mau chot.
static void sc_IssueStep(SC_Bot& b, SC_Route& rt, int i)
{
	if (b.nLeg < 0)          b.nLeg = 0;
	if (b.nLeg >= rt.nCount) b.nLeg = 0;

	int tx = rt.wpX[b.nLeg];
	int ty = rt.wpY[b.nLeg];
	int nx, ny;
	Npc[i].GetMpsPos(&nx, &ny);
	int sw = Npc[i].m_SubWorldIndex;

	int dx = tx, dy = ty;               // mac dinh: node con gan, di thang toi luon

	int vx = tx - nx, vy = ty - ny;
	int ax = (vx < 0) ? -vx : vx;
	int ay = (vy < 0) ? -vy : vy;
	int len = (ax > ay) ? (ax + ay * 7 / 16) : (ay + ax * 7 / 16);   // xap xi bat giac

	if (len > SC_STEP_MPS)
	{
		int fx = (int)((__int64)vx * SC_STEP_MPS / len);
		int fy = (int)((__int64)vy * SC_STEP_MPS / len);
		int px = (int)((__int64)(-vy) * 32 / len);    // 1 o lech ngang
		int py = (int)((__int64)( vx) * 32 / len);

		int cx[3], cy[3];
		cx[0] = nx + fx;        cy[0] = ny + fy;      // dung tren tia toi node
		cx[1] = nx + fx + px;   cy[1] = ny + fy + py; // lech 1 o de vuot mep
		cx[2] = nx + fx - px;   cy[2] = ny + fy - py;

		dx = 0;
		for (int k = 0; k < 3; k++)
		{
			if (!sc_Walkable(sw, cx[k], cy[k]))
				continue;
			dx = cx[k];
			dy = cy[k];
			break;
		}
		if (dx <= 0)                    // khong diem trung gian nao di duoc -> pho mac node that
		{
			dx = tx;
			dy = ty;
		}
	}

	if (dx > 0 && dy > 0)
		Npc[i].SendCommand(do_walk, dx, dy);
	b.nTgtX    = dx;
	b.nTgtY    = dy;
	b.nRetry   = 0;
	b.nRetryCd = 0;
	b.nStuck   = 0;
}

// Node GAN NHAT trong lo trinh. Tra chi so, ghi khoang cach binh phuong ra *pDist2.
//
// VI SAO PHAI CO (loi B1 - nghi can chinh cua "di theo lo trinh thi DINH GOC"):
//   Ban cu dat nLeg = 0 vo dieu kien, tuc ban bot toi node DAU TIEN THEO THU TU TEP.
//   Do that tren du lieu: 1_phuongtuong node[0] cach cho admin dung toi 1009 o (~32.000 MPS),
//   37_bienkinh toi 1155 o. Server KHONG co A* (KJXPathFinder.h:12 la #ifndef _SERVER), chi con
//   bo men-tuong 30 tick -> duong thang cat ngang ca thanh chac chan dam nha va ket.
//   Cac node trong preset la BUOC DI TREN DO THI (do that: 1_phuongtuong 63/63 cap node lien
//   tiep deu la canh trong _nodes.txt) nen node k -> k+1 chi bao dam di duoc KHI xuat phat tu
//   node k. Bam vao node gan nhat = khoi phuc bat bien do thi.
static int sc_NearestLeg(SC_Route& rt, int nx, int ny, __int64* pDist2)
{
	int     nBest  = 0;
	__int64 nBestD = -1;
	for (int k = 0; k < rt.nCount; k++)
	{
		__int64 d = sc_Dist2(nx, ny, rt.wpX[k], rt.wpY[k]);
		if (nBestD < 0 || d < nBestD)
		{
			nBestD = d;
			nBest  = k;
		}
	}
	if (pDist2)
		*pDist2 = (nBestD < 0) ? 0 : nBestD;
	return nBest;
}

// Bam bot vao node gan nhat roi phat buoc dau.
static void sc_SnapToNearest(SC_Bot& b, SC_Route& rt, int i)
{
	int nx, ny;
	Npc[i].GetMpsPos(&nx, &ny);
	b.nLeg = sc_NearestLeg(rt, nx, ny, NULL);
	sc_IssueStep(b, rt, i);
}

// Phat 1 dich NE NGANG (vuong goc voi tia cur->node) de pha the ket kieu A.
//
// VI SAO PHAI VUONG GOC, KHONG PHAI TRUNG DIEM:
//   g_GetDirIndex (KMath.h:109-149) chi phu thuoc TI SO (dx,dy). Trung diem nam tren dung tia
//   cur->node nen cho nWantDir Y HET -> CheckBarrier(x,y) (KNpcFindPath.cpp:74) cho ket qua y
//   het -> toan bo vong do ne 112-155 tai hien dung that bai cu. GetDir la ham TAT DINH cua
//   (vi tri, dich, ban do tinh): cung dau vao = cung dau ra. Chi doi HUONG moi doi duoc phep thu.
// Tra 1 neu da phat duoc mot diem ne di duoc.
static int sc_TryDetour(SC_Bot& b, int i, int nx, int ny, int tx, int ty)
{
	int sw = Npc[i].m_SubWorldIndex;
	int vx = tx - nx;
	int vy = ty - ny;

	// chuan hoa xap xi kieu bat giac: len ~= max + min*7/16 (sai so < 4%, khong can sqrt/float)
	int ax  = (vx < 0) ? -vx : vx;
	int ay  = (vy < 0) ? -vy : vy;
	int len = (ax > ay) ? (ax + ay * 7 / 16) : (ay + ax * 7 / 16);
	if (len < 1)
		return 0;

	int fx = (int)((__int64)vx * SC_SIDE_MPS / len);   // don vi tien theo huong node
	int fy = (int)((__int64)vy * SC_SIDE_MPS / len);
	int px = (int)((__int64)(-vy) * SC_SIDE_MPS / len);  // don vi vuong goc
	int py = (int)((__int64)( vx) * SC_SIDE_MPS / len);

	// thu 4 ung vien: cheo-truoc va ngang-thuan, moi ben mot lan (doi ben moi lan goi)
	int s = (b.nSide >= 0) ? 1 : -1;
	int cx[4], cy[4];
	cx[0] = nx + fx + px * s;      cy[0] = ny + fy + py * s;
	cx[1] = nx + fx - px * s;      cy[1] = ny + fy - py * s;
	cx[2] = nx + px * s * 2;       cy[2] = ny + py * s * 2;
	cx[3] = nx - px * s * 2;       cy[3] = ny - py * s * 2;

	for (int k = 0; k < 4; k++)
	{
		if (!sc_Walkable(sw, cx[k], cy[k]))
			continue;
		Npc[i].SendCommand(do_walk, cx[k], cy[k]);
		b.nTgtX = cx[k];            // diem ne tro thanh dich hien tai; toi noi se phat buoc ke
		b.nTgtY = cy[k];
		b.nSide = -s;               // lan sau doi ben
		return 1;
	}
	b.nSide = -s;
	return 0;
}

// ============================================================================
// GD1 Fix2 - sinh / xoa bot kind_player mang ngoai hinh nguoi choi (sentinel -1/-2)
// ============================================================================

// SC_AddBot(nSex, nLevel, nSubWorldIdx, nMpsX, nMpsY, nSeries [, szName] [, nFaction] [, nLifeMax])
int LuaSC_AddBot(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 6)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nSex         = (int)Lua_ValueToNumber(L, 1);
	int nLevel       = (int)Lua_ValueToNumber(L, 2);
	int nSubWorldIdx = (int)Lua_ValueToNumber(L, 3);
	int nMpsX        = (int)Lua_ValueToNumber(L, 4);
	int nMpsY        = (int)Lua_ValueToNumber(L, 5);
	int nSeries      = (int)Lua_ValueToNumber(L, 6);

	if (nLevel < 1)   nLevel = 1;
	if (nLevel > 127) nLevel = 127;
	if (nSeries < 0 || nSeries > 5) nSeries = 0;

	// ---- TEN BOT: kiem TRUOC khi sinh, chu game da chot CAM TRUNG TUYET DOI ----
	//   NpcSet.SearchName (KNpcSet.cpp:242-258) quet m_UseIdx = MOI NPC dang song, ke ca
	//   KNpc cua nguoi choi that, va tra khop DAU TIEN - khong phan biet bot hay nguoi.
	//   Ten trung se lam: (a) hook PM nuot tin nhan gui cho nguoi that, (b) loi moi bang hoi
	//   (CoreShell.cpp:9330) va theo-sau-theo-ten (KPlayerAuto.cpp:1016) di nham sang bot,
	//   (c) bong thoai gan nham doi tuong (CoreShell.cpp:8458-8473).
	char szName[32];
	szName[0] = 0;
	if (Lua_GetTopIndex(L) >= 7 && Lua_IsString(L, 7))
	{
		char* pName = (char*)lua_tostring(L, 7);
		if (pName && pName[0])
		{
			strncpy(szName, pName, sizeof(szName) - 1);
			szName[sizeof(szName) - 1] = 0;
		}
	}
	if (szName[0] && NpcSet.SearchName(szName) != 0)
	{
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, -1);        // -1 = ten da co nguoi dung
		return 2;
	}

	int nTid = nSex ? PLAYER_FEMALE_NPCTEMPLATEID : PLAYER_MALE_NPCTEMPLATEID;

	int nNpcIdx = NpcSet.AddNpcSet2(MAKELONG(nLevel, nTid), nSeries, nSubWorldIdx, nMpsX, nMpsY);
	if (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	KNpc* p = &Npc[nNpcIdx];

	p->m_btSimCityBot = 1;
	p->m_Level        = nLevel;
	p->m_Series       = nSeries;
	p->m_nSex         = nSex;

	// ---- DON RAC HEAP (bat buoc truoc khi lop thong tin doc cac truong nay) ----
	//   Server cap phat Npc bang "new KNpc[MAX_NPC]" (KNpc.cpp:111) = heap KHONG zero-init.
	//   Lenh ZeroMemory cua m_szTongName/m_szTongTitle/m_nFigure nam trong khoi #ifndef _SERVER
	//   cua KNpc::Init (KNpc.cpp:164-176) nen TREN SERVER KHONG BAO GIO CHAY; con
	//   ZeroMemory(MateName) o KNpc.cpp:232 thi BI CHU THICH.
	//   => 4 truong nay la 32 byte rac KHONG ket thuc NUL. Khi lop thong tin sao chep chung
	//      vao goi sync, strcpy se doc tran qua truong ke tiep. Phai don o day.
	memset(p->m_szTongName,  0, sizeof(p->m_szTongName));
	memset(p->m_szTongTitle, 0, sizeof(p->m_szTongTitle));
	memset(p->MateName,      0, sizeof(p->MateName));
	p->m_nFigure  = 0;
	p->m_Recruit  = 0;

	// ---- NGOAI HINH mac dinh (khuon duy nhat trong cay nguon: KPlayerDBFuns.cpp:439-443) ----
	//   KNpc::Init dat Helm/Armor/Weapon = 1 (KNpc.cpp:205-207) = do "tap su", trong ky cuc.
	//   g_ItemChangeRes.Init() CO chay tren server (KCore.cpp:236, ngoai khoi #ifndef _SERVER).
	p->m_WeaponType = g_ItemChangeRes.GetWeaponRes(0, 0, 0);
	p->m_ArmorType  = g_ItemChangeRes.GetArmorRes(0, 0);
	p->m_HelmType   = g_ItemChangeRes.GetHelmRes(0, 0);
	p->m_HorseType  = g_ItemChangeRes.GetHorseRes(0, 0);
	p->m_bRideHorse = FALSE;

	// TEN: KNpc::Init CO xoa Name (KNpc.cpp:219 ZeroMemory(Name, 32)) nen o tai su dung KHONG
	// mang ten chu cu. Nhung van phai dat ten: ten rong thi client khong ve bong thoai
	// (CoreShell.cpp:8458-8473 tim nguoi noi THEO TEN).
	strncpy(p->Name, szName, sizeof(p->Name) - 1);
	p->Name[sizeof(p->Name) - 1] = 0;
	if (!p->Name[0])
	{
		sprintf(p->Name, "SC%d", nNpcIdx);          // duy nhat trong Npc[] theo chi so o
		if (NpcSet.SearchName(p->Name) != nNpcIdx)  // cuc hiem: nguoi that dat trung "SCxxx"
			sprintf(p->Name, "SC%dx", nNpcIdx);
	}

	if (Lua_GetTopIndex(L) >= 8)
	{
		// m_CurrentCamp la CHI SO MANG cua m_RelationTable[..][camp_num][camp_num][..]
		// (KNpcSet.cpp:1663-1664, 1739-1740) voi camp_num = 9 (GameDataDef.h:495).
		// Mot dong Lua go nham la doc ngoai mang trong ham nong nhat cua engine -> phai kep.
		int nFaction = (int)Lua_ValueToNumber(L, 8);
		if (nFaction < 0 || nFaction >= camp_num)
			nFaction = camp_justice;      // dan thanh thi = chinh phai; camp_free(4) = "mau do sat thu"
		p->m_Camp        = nFaction;
		p->m_CurrentCamp = nFaction;
	}
	else
	{
		p->m_Camp        = camp_justice;  // KNpc::Init de camp_free (KNpc.cpp:144-145) - sai y do
		p->m_CurrentCamp = camp_justice;
	}
	if (Lua_GetTopIndex(L) >= 9)
	{
		int nLifeMax = (int)Lua_ValueToNumber(L, 9);
		if (nLifeMax > 0)
			p->m_LifeMax = nLifeMax;
	}

	p->RestoreNpcBaseInfo();          // fill m_Current* tu base (con kind_normal)
	p->m_Kind = kind_player;          // flip CUOI CUNG
	p->SetProcessAI(1);               // bot phai co AI de di duoc; xac dinh, khong phu thuoc lich su slot

	sc_Register(nNpcIdx);             // vao so de driver di chuyen (chua co lo trinh)

	Lua_PushNumber(L, nNpcIdx);
	return 1;
}

int LuaSC_DelBot(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int i = (int)Lua_ValueToNumber(L, 1);
	if (i <= 0 || i >= MAX_NPC || !Npc[i].m_btSimCityBot)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (Npc[i].m_SubWorldIndex >= 0 && Npc[i].m_RegionIndex >= 0)
	{
		SubWorld[Npc[i].m_SubWorldIndex].m_Region[Npc[i].m_RegionIndex].RemoveNpc(i);
		SubWorld[Npc[i].m_SubWorldIndex].m_Region[Npc[i].m_RegionIndex].DecRef(Npc[i].m_MapX, Npc[i].m_MapY, obj_npc);
		NpcSet.Remove(i);
		sc_Deregister(i);
		Lua_PushNumber(L, 1);
		return 1;
	}
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaSC_ClearBots(Lua_State* L)
{
	int nCount = 0;
	for (int i = 1; i < MAX_NPC; i++)
	{
		if (!Npc[i].m_btSimCityBot)
			continue;
		if (Npc[i].m_SubWorldIndex >= 0 && Npc[i].m_RegionIndex >= 0)
		{
			SubWorld[Npc[i].m_SubWorldIndex].m_Region[Npc[i].m_RegionIndex].RemoveNpc(i);
			SubWorld[Npc[i].m_SubWorldIndex].m_Region[Npc[i].m_RegionIndex].DecRef(Npc[i].m_MapX, Npc[i].m_MapY, obj_npc);
			NpcSet.Remove(i);
			nCount++;
		}
	}
	s_botCount   = 0;                 // xoa so bot
	s_routeCount = 0;                 // xoa bang lo trinh (khong con bot nao tham chieu)
	Lua_PushNumber(L, nCount);
	return 1;
}

// ============================================================================
// GD2 - dieu khien di chuyen
// ============================================================================

int LuaSC_MoveOn(Lua_State* L)
{
	g_bSimCityMove = 1;
	Lua_PushNumber(L, 1);
	return 1;
}

int LuaSC_MoveOff(Lua_State* L)
{
	g_bSimCityMove = 0;
	Lua_PushNumber(L, 1);
	return 1;
}

// SC_Goto(nNpcIdx, nMpsX, nMpsY) -> phat 1 lenh di toi (MPS). Test tay 1 chang.
int LuaSC_Goto(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int i     = (int)Lua_ValueToNumber(L, 1);
	int nMpsX = (int)Lua_ValueToNumber(L, 2);
	int nMpsY = (int)Lua_ValueToNumber(L, 3);
	if (i <= 0 || i >= MAX_NPC || !Npc[i].m_btSimCityBot || nMpsX <= 0 || nMpsY <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	Npc[i].SendCommand(do_walk, nMpsX, nMpsY);
	Lua_PushNumber(L, 1);
	return 1;
}

// SC_LoadPreset(szPresetPath, szPathName) -> nRouteId (>=0) hoac -1.
//   Doc file _preset.txt (2 cot: PathName, node_name "X_Y"), gom node cua szPathName theo thu tu.
//   szPresetPath vd "/settings/simcity/maps/thanhthi/1_phuongtuong_preset.txt" (giai theo cwd server).
//   PHAI dung gach xuoi "/" - Lua 4.0 nuot escape "\" ("\t" -> TAB). g_GetFullPath nhan ca hai.
int LuaSC_LoadPreset(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 || !Lua_IsString(L, 1) || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, -1);
		return 1;
	}
	char* szFile = (char*)lua_tostring(L, 1);
	char* szPath = (char*)lua_tostring(L, 2);
	if (!szFile || !szFile[0] || !szPath || !szPath[0])
	{
		Lua_PushNumber(L, -1);
		return 1;
	}

	// KHU TRUNG LAP (loi B4): menu admin goi LoadPreset MOI LAN chon tuyen. Khong tra lai rid
	// cu thi 64 lan bam menu la can SC_MAX_ROUTE roi tra -1 vinh vien.
	for (int k = 0; k < s_routeCount; k++)
	{
		if (s_routes[k].nOwner < 0
			&& strcmp(s_routes[k].szFile, szFile) == 0
			&& strcmp(s_routes[k].szPath, szPath) == 0)
		{
			Lua_PushNumber(L, k);
			return 1;
		}
	}

	if (s_routeCount >= SC_MAX_ROUTE)
	{
		Lua_PushNumber(L, -1);
		return 1;
	}

	KTabFile tab;
	if (!tab.Load((LPSTR)szFile))
	{
		Lua_PushNumber(L, -1);
		return 1;
	}

	SC_Route& rt = s_routes[s_routeCount];
	rt.nCount = 0;
	rt.nOwner = -1;
	strncpy(rt.szFile, szFile, sizeof(rt.szFile) - 1); rt.szFile[sizeof(rt.szFile) - 1] = 0;
	strncpy(rt.szPath, szPath, sizeof(rt.szPath) - 1); rt.szPath[sizeof(rt.szPath) - 1] = 0;

	int hgt = tab.GetHeight();
	char szPn[64], szNn[64];
	for (int row = 2; row <= hgt && rt.nCount < SC_MAX_NODE; row++)   // row 1 = header
	{
		tab.GetString(row, 1, (LPSTR)"", szPn, sizeof(szPn));
		if (strcmp(szPn, szPath) != 0)
			continue;
		tab.GetString(row, 2, (LPSTR)"", szNn, sizeof(szNn));
		int gx = 0, gy = 0;
		if (sscanf(szNn, "%d_%d", &gx, &gy) != 2 || gx <= 0 || gy <= 0)
			continue;

		int mx = gx * 32;   // o luoi -> MPS
		int my = gy * 32;

		// LOC NODE QUA GAN (loi B3): do that tren bo preset co 59 chang ngan hon SC_ARRIVE_MPS,
		// ke ca node TRUNG KHIT (10000_tongkim_nguyensoai_preset.txt tuyen huong2trai co
		// 1570_3199 -> 1570_3199, dai 0). Chang ngan hon ban kinh toi dich = "toi ngay lap tuc"
		// -> tien chang + SendCommand + broadcast moi tick. Bo di vo hai vi node bi bo nam
		// TRONG ban kinh toi dich cua node truoc.
		if (rt.nCount > 0)
		{
			__int64 d = sc_Dist2(mx, my, rt.wpX[rt.nCount - 1], rt.wpY[rt.nCount - 1]);
			if (d < (__int64)SC_ARRIVE_MPS * SC_ARRIVE_MPS)
				continue;
		}

		rt.wpX[rt.nCount] = mx;
		rt.wpY[rt.nCount] = my;
		rt.nCount++;
	}

	if (rt.nCount < 2)
	{
		Lua_PushNumber(L, -1);
		return 1;
	}
	Lua_PushNumber(L, s_routeCount);
	s_routeCount++;
	return 1;
}

// SC_SetBotRoute(nNpcIdx, nRouteId [, bLoop=1]) -> 1/0, kem khoang cach (o) toi node gan nhat.
int LuaSC_SetBotRoute(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int i     = (int)Lua_ValueToNumber(L, 1);
	int nRid  = (int)Lua_ValueToNumber(L, 2);
	int bLoop = (Lua_GetTopIndex(L) >= 3) ? (int)Lua_ValueToNumber(L, 3) : 1;
	if (i <= 0 || i >= MAX_NPC || !Npc[i].m_btSimCityBot || nRid < 0 || nRid >= s_routeCount)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int r = sc_Find(i);
	if (r < 0)
	{
		sc_Register(i);
		r = sc_Find(i);
		if (r < 0)
		{
			Lua_PushNumber(L, 0);
			return 1;
		}
	}

	SC_Route& rt = s_routes[nRid];
	int nx, ny;
	Npc[i].GetMpsPos(&nx, &ny);

	// FIX B1: bam vao node GAN NHAT thay vi nLeg = 0 (xem chu thich sc_NearestLeg).
	__int64 dNear = 0;
	int     nLeg  = sc_NearestLeg(rt, nx, ny, &dNear);

	// node gan nhat con qua xa -> tu choi, bao cho GM biet thay vi ban bot di xuyen thanh
	if (dNear > (__int64)SC_SNAP_MPS * SC_SNAP_MPS)
	{
		int ddx = nx - rt.wpX[nLeg]; if (ddx < 0) ddx = -ddx;
		int ddy = ny - rt.wpY[nLeg]; if (ddy < 0) ddy = -ddy;
		// xap xi bat giac (sai so < 4%), khong can sqrt; doi MPS -> o luoi
		int nCell = ((ddx > ddy) ? (ddx + ddy * 7 / 16) : (ddy + ddx * 7 / 16)) / 32;
		Lua_PushNumber(L, 0);
		Lua_PushNumber(L, nCell);
		return 2;
	}

	SC_Bot& b = s_bots[r];
	b.nRouteId = nRid;
	b.nLeg     = nLeg;
	b.nDir     = 1;
	b.bLoop    = bLoop ? 1 : 0;
	b.nGiveUp  = 0;
	b.nSide    = 1;
	b.nLastX   = nx;
	b.nLastY   = ny;
	sc_IssueStep(b, rt, i);
	Lua_PushNumber(L, 1);
	return 1;
}

// SC_PatrolBox(nNpcIdx [, nHalfMps]) -> gan lo trinh tuan tra hinh vuong quanh vi tri hien tai.
//
// FIX B5 (nghi can chinh cua "di tuan tra thi DINH GOC"): ban cu dung 4 goc cx +/- nHalf ma
// KHONG he kiem TestBarrier lan bien ban do. Trong thanh, hop 10 o gan nhu chac chan co goc
// nam trong nha/ngoai region -> GetDir tra != 1 mai -> DoStand -> ket vinh vien.
// Nay: thu thu nho dan ban kinh, chon co lon nhat co du 2 goc DI DUOC tro len.
int LuaSC_PatrolBox(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int i = (int)Lua_ValueToNumber(L, 1);
	if (i <= 0 || i >= MAX_NPC || !Npc[i].m_btSimCityBot)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nHalf = (Lua_GetTopIndex(L) >= 2) ? (int)Lua_ValueToNumber(L, 2) : SC_BOX_HALF;
	if (nHalf < 64)   nHalf = 64;
	if (nHalf > 2048) nHalf = 2048;

	int r = sc_Find(i);
	if (r < 0)
	{
		sc_Register(i);
		r = sc_Find(i);
		if (r < 0)
		{
			Lua_PushNumber(L, 0);
			return 1;
		}
	}

	int sw = Npc[i].m_SubWorldIndex;
	int cx, cy;
	Npc[i].GetMpsPos(&cx, &cy);

	// tim co hop lon nhat ma con >= 2 goc di duoc
	int nBestHalf = 0, nBestOk = 0;
	int nTry[4];
	nTry[0] = nHalf; nTry[1] = nHalf * 3 / 4; nTry[2] = nHalf / 2; nTry[3] = nHalf / 4;
	for (int t = 0; t < 4; t++)
	{
		int h = nTry[t];
		if (h < 64)
			continue;
		int nOk = 0;
		if (sc_Walkable(sw, cx - h, cy - h)) nOk++;
		if (sc_Walkable(sw, cx + h, cy - h)) nOk++;
		if (sc_Walkable(sw, cx + h, cy + h)) nOk++;
		if (sc_Walkable(sw, cx - h, cy + h)) nOk++;
		if (nOk > nBestOk || (nOk == nBestOk && nOk >= 2 && nBestHalf == 0))
		{
			nBestOk   = nOk;
			nBestHalf = h;
		}
		if (nOk == 4)
			break;      // du 4 goc o ban kinh lon nhat -> dung luon
	}
	if (nBestOk < 2)
	{
		Lua_PushNumber(L, 0);   // cho nay qua chat, khong dung noi hop tuan tra
		return 1;
	}

	// tai dung o lo trinh cu cua CHINH bot nay (khoi can SC_MAX_ROUTE moi lan bam menu)
	int nRid = -1;
	for (int k = 0; k < s_routeCount; k++)
	{
		if (s_routes[k].nOwner == i)
		{
			nRid = k;
			break;
		}
	}
	if (nRid < 0)
	{
		if (s_routeCount >= SC_MAX_ROUTE)
		{
			Lua_PushNumber(L, 0);
			return 1;
		}
		nRid = s_routeCount++;
	}

	SC_Route& rt = s_routes[nRid];
	rt.nOwner    = i;
	rt.szFile[0] = 0;
	rt.szPath[0] = 0;
	rt.nCount    = 0;

	int gx[4], gy[4];
	gx[0] = cx - nBestHalf; gy[0] = cy - nBestHalf;
	gx[1] = cx + nBestHalf; gy[1] = cy - nBestHalf;
	gx[2] = cx + nBestHalf; gy[2] = cy + nBestHalf;
	gx[3] = cx - nBestHalf; gy[3] = cy + nBestHalf;
	for (int k = 0; k < 4; k++)
	{
		if (!sc_Walkable(sw, gx[k], gy[k]))
			continue;
		rt.wpX[rt.nCount] = gx[k];
		rt.wpY[rt.nCount] = gy[k];
		rt.nCount++;
	}
	if (rt.nCount < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	SC_Bot& b = s_bots[r];
	b.nRouteId = nRid;
	b.nDir     = 1;
	b.bLoop    = 1;
	b.nGiveUp  = 0;
	b.nSide    = 1;
	b.nLastX   = cx;
	b.nLastY   = cy;
	sc_SnapToNearest(b, rt, i);

	Lua_PushNumber(L, 1);
	return 1;
}

// ============================================================================
// GD4 - LOP THONG TIN BOT (ngoai trang / mon phai / danh hieu / bang hoi / chi so)
//
// NGUYEN LY CHUNG: moi truong duoi day DA TON TAI tren KNpc va DA duoc hai ham dong bo
// KNpc::SendSyncData (s2c_syncplayer) va KNpc::NormalSync (s2c_syncplayermin) doc san.
// Cac ham nay chi GHI GIA TRI - khong them truong moi, khong them goi tin, khong doi
// kich thuoc goi (PLAYER_SYNC 222 byte / PLAYER_NORMAL_SYNC 229 byte giu nguyen).
// Nhanh doc cho bot da duoc chen trong KNpc.cpp (tim "Port SimCity: nguon du lieu cho BOT").
// ============================================================================

// chot chung: chi so phai tro toi mot BOT con song
static KNpc* sc_Bot(Lua_State* L, int nArg)
{
	if (Lua_GetTopIndex(L) < nArg)
		return NULL;
	int i = (int)Lua_ValueToNumber(L, nArg);
	if (i <= 0 || i >= MAX_NPC || !Npc[i].m_btSimCityBot || !Npc[i].m_Index)
		return NULL;
	return &Npc[i];
}

// kep ve mien cua BYTE - cac truong ngoai trang deu la BYTE trong goi tin
// (KProtocol.h:28/31/33/38 va 426-431). Dat >= 256 se bi cat am tham.
static int sc_Byte(int v)
{
	if (v < 0)   return 0;
	if (v > 255) return 255;
	return v;
}

// doc tham so tuy chon; khong truyen thi giu nguyen gia tri cu
static int sc_Opt(Lua_State* L, int nArg, int nCur)
{
	if (Lua_GetTopIndex(L) < nArg)
		return nCur;
	return (int)Lua_ValueToNumber(L, nArg);
}

// SC_SetBotLook(idx, nHelm, nArmor, nWeapon [, nHorse] [, nMantle] [, nMantleLevel] [, nMask])
//   Tuong duong ChangeNpcFeature cua ban goc JX2 (plugins/pngoaitrang.lua) - ban goc goi
//   ChangeNpcFeature(idx, 0, 0, sentinel, helm, armor, weapon, horse) roi SetNpcRideHorse.
//   O JX1 ham Lua ChangeNpcFeature chi la stub rong (ScriptFuns.cpp) nen ta ghi thang truong.
//   nHorse < 0 = khong cuoi ngua (ham sync tu ep HorseType = -1).
int LuaSC_SetBotLook(Lua_State* L)
{
	KNpc* p = sc_Bot(L, 1);
	if (!p || Lua_GetTopIndex(L) < 4)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	p->m_HelmType   = sc_Byte((int)Lua_ValueToNumber(L, 2));
	p->m_ArmorType  = sc_Byte((int)Lua_ValueToNumber(L, 3));
	p->m_WeaponType = sc_Byte((int)Lua_ValueToNumber(L, 4));

	int nHorse = sc_Opt(L, 5, -1);
	if (nHorse >= 0)
	{
		p->m_HorseType  = sc_Byte(nHorse);
		p->m_bRideHorse = TRUE;
	}
	else
	{
		p->m_HorseType  = 0;
		p->m_bRideHorse = FALSE;
	}

	p->m_MantleType   = sc_Byte(sc_Opt(L, 6, p->m_MantleType));
	p->m_byMantleLevel = (BYTE)sc_Byte(sc_Opt(L, 7, p->m_byMantleLevel));
	p->m_MaskType     = sc_Opt(L, 8, p->m_MaskType);   // MaskType la int trong goi, khong can kep

	Lua_PushNumber(L, 1);
	return 1;
}

// ---- kho ngoai trang HOP LE cua JX1 (do tu chinh bang g_ItemChangeRes) ----
//
// Ban goc JX2 (plugins/pngoaitrang.lua) dung bang trang gan cung:
//   HELM_WL = {2,5,8,11,14,17,20}; WEAPON_WL = {20,23,24,26,28,30}; HORSE_WL = {0,1,2,5,9,10,11,12}
//   ARMOR_WL_NAM = {2,8,11,14,20,23,29,32,35,41}; ARMOR_WL_NU = {8,14,19,20,29,35,40,41}
// KHONG bung nguyen sang JX1: do la chi so trong \settings\npcres_simple\*.txt cua JX2,
// ma JX1 KHONG co thu muc do va dung bo bang khac (CHANGERES_*_FILE, KItemChangeRes.cpp:54-72).
// Bung nham = bot mac do khong ton tai -> mat mon do (CRESINFO::GetName co kiem bien nen
// khong sap client, nhung nhin nhu coi truong).
//
// Thay vao do DO THAT: g_ItemChangeRes.GetXxxRes(nParti, nLevel) tra ve gia tri tu KTabFile;
// dong ngoai bang thi GetInteger tra mac dinh 2 => ham tra 0. Nen chi can quet va giu cac
// gia tri > 0 la co dung tap ngoai trang MA BAN JX1 NAY THUC SU CO.
#define SC_MAX_RES  128

struct SC_ResPool
{
	int nCount;
	int v[SC_MAX_RES];
};

static SC_ResPool s_resHelm, s_resArmor, s_resWeapon, s_resHorse;
static int        s_resReady = 0;

static void sc_PoolAdd(SC_ResPool& pool, int nVal)
{
	if (nVal <= 0 || nVal > 255 || pool.nCount >= SC_MAX_RES)
		return;
	for (int k = 0; k < pool.nCount; k++)
		if (pool.v[k] == nVal)
			return;                 // khu trung
	pool.v[pool.nCount++] = nVal;
}

static void sc_BuildResPools()
{
	if (s_resReady)
		return;
	s_resReady = 1;
	s_resHelm.nCount = s_resArmor.nCount = s_resWeapon.nCount = s_resHorse.nCount = 0;

	// nRow = nParti * 10 + nLevel + 2 (KItemChangeRes.cpp:87, 107, 120, 132)
	for (int nParti = 0; nParti < 32; nParti++)
	{
		for (int nLevel = 1; nLevel <= 9; nLevel++)
		{
			sc_PoolAdd(s_resHelm,   g_ItemChangeRes.GetHelmRes(nParti, nLevel));
			sc_PoolAdd(s_resArmor,  g_ItemChangeRes.GetArmorRes(nParti, nLevel));
			sc_PoolAdd(s_resHorse,  g_ItemChangeRes.GetHorseRes(nParti, nLevel));
			sc_PoolAdd(s_resWeapon, g_ItemChangeRes.GetWeaponRes(equip_meleeweapon, nParti, nLevel));
		}
	}
}

// SC_DressBot(idx [, bRideHorse=0]) -> 1/0. Tuong duong SimCityNgoaiTrang:makeup cua ban goc.
int LuaSC_DressBot(Lua_State* L)
{
	KNpc* p = sc_Bot(L, 1);
	if (!p)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	sc_BuildResPools();

	if (s_resHelm.nCount > 0)
		p->m_HelmType = s_resHelm.v[g_Random(s_resHelm.nCount)];
	if (s_resArmor.nCount > 0)
		p->m_ArmorType = s_resArmor.v[g_Random(s_resArmor.nCount)];
	if (s_resWeapon.nCount > 0)
		p->m_WeaponType = s_resWeapon.v[g_Random(s_resWeapon.nCount)];

	int bHorse = (Lua_GetTopIndex(L) >= 2) ? (int)Lua_ValueToNumber(L, 2) : 0;
	if (bHorse && s_resHorse.nCount > 0)
	{
		p->m_HorseType  = s_resHorse.v[g_Random(s_resHorse.nCount)];
		p->m_bRideHorse = TRUE;
	}
	else
	{
		p->m_HorseType  = 0;
		p->m_bRideHorse = FALSE;
	}

	Lua_PushNumber(L, 1);
	// tra them so mon do moi kho tim duoc - de GM biet du lieu res co nap duoc khong
	Lua_PushNumber(L, s_resHelm.nCount);
	Lua_PushNumber(L, s_resArmor.nCount);
	Lua_PushNumber(L, s_resWeapon.nCount);
	Lua_PushNumber(L, s_resHorse.nCount);
	return 5;
}

// SC_SetBotFaction(idx, nFaction 0..9) -> 1/0. Dat mon phai + camp tuong ung.
//   MON PHAI KHAC HE (ngu hanh): KItemList.cpp:994-999 doc m_cFaction.m_nFirstAddFaction cho
//   magic_requiremenpai, con KItemList.cpp:1000-1005 doc m_Series cho magic_requireseries.
//   Anh xa: KFaction::GetID(nSeries, nNo) = nSeries * FACTIONS_PRR_SERIES + nNo => 5 he x 2 phai.
int LuaSC_SetBotFaction(Lua_State* L)
{
	KNpc* p = sc_Bot(L, 1);
	if (!p || Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nFaction = (int)Lua_ValueToNumber(L, 2);
	if (nFaction < 0 || nFaction >= MAX_FACTION)
	{
		p->nFirstFaction = (BYTE)-1;      // chua vao phai; UpdateGameTitle chi in "Lv:x"
		Lua_PushNumber(L, 1);
		return 1;
	}
	p->nFirstFaction = (BYTE)nFaction;

	// camp phai theo phai, giong nguoi that (KPlayer.cpp:4017). KNpc::Init de camp_free
	// (KNpc.cpp:144-145) = "mau do sat thu" - sai hoan toan y do "dan thanh thi".
	int nCamp = g_Faction.GetCamp(nFaction);
	if (nCamp < 0 || nCamp >= camp_num)
		nCamp = camp_justice;
	p->m_Camp        = nCamp;
	p->m_CurrentCamp = nCamp;

	Lua_PushNumber(L, 1);
	return 1;
}

// SC_SetBotInfo(idx, nPKValue, nRepute, nFuYuan, nReBorn [, nRankInWorld] [, nImagePlayer])
//   Cac o so trong bang "Tin tuc". Deu la truong san co cua KNpc (KNpc.h:231-236).
int LuaSC_SetBotInfo(Lua_State* L)
{
	KNpc* p = sc_Bot(L, 1);
	if (!p || Lua_GetTopIndex(L) < 5)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	p->nPKValue     = (BYTE)sc_Byte((int)Lua_ValueToNumber(L, 2));
	p->nRepute      = (int)Lua_ValueToNumber(L, 3);
	p->nFuYuan      = (int)Lua_ValueToNumber(L, 4);
	p->nReBorn      = (BYTE)sc_Byte((int)Lua_ValueToNumber(L, 5));
	p->nRankInWorld = sc_Opt(L, 6, p->nRankInWorld);
	p->m_ImagePlayer = (BYTE)sc_Byte(sc_Opt(L, 7, p->m_ImagePlayer));
	Lua_PushNumber(L, 1);
	return 1;
}

// SC_SetBotTitle(idx, nRankId [, nRankBattleId] [, nPlayerTitle] [, nHonorId])
int LuaSC_SetBotTitle(Lua_State* L)
{
	KNpc* p = sc_Bot(L, 1);
	if (!p || Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	p->m_btRankId       = (BYTE)sc_Byte((int)Lua_ValueToNumber(L, 2));
	p->m_btRankBattleId = (DWORD)sc_Opt(L, 3, p->m_btRankBattleId);
	p->m_btPlayerTitle  = (DWORD)sc_Opt(L, 4, p->m_btPlayerTitle);
	p->m_btHonorId      = (BYTE)sc_Byte(sc_Opt(L, 5, p->m_btHonorId));
	Lua_PushNumber(L, 1);
	return 1;
}

// SC_SetBotTong(idx, szTongName [, szTongTitle] [, nFigure] [, nRecruit])
//   Truyen chuoi rong de xoa. LUU Y: 3 truong nay tung la RAC HEAP tren server
//   (ZeroMemory cua chung nam trong khoi #ifndef _SERVER cua KNpc::Init) - SC_AddBot da don.
int LuaSC_SetBotTong(Lua_State* L)
{
	KNpc* p = sc_Bot(L, 1);
	if (!p || Lua_GetTopIndex(L) < 2 || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	char* pName = (char*)lua_tostring(L, 2);
	memset(p->m_szTongName, 0, sizeof(p->m_szTongName));
	if (pName)
		strncpy(p->m_szTongName, pName, sizeof(p->m_szTongName) - 1);

	memset(p->m_szTongTitle, 0, sizeof(p->m_szTongTitle));
	if (Lua_GetTopIndex(L) >= 3 && Lua_IsString(L, 3))
	{
		char* pTitle = (char*)lua_tostring(L, 3);
		if (pTitle)
			strncpy(p->m_szTongTitle, pTitle, sizeof(p->m_szTongTitle) - 1);
	}
	p->m_nFigure = sc_Opt(L, 4, 0);
	p->m_Recruit = sc_Opt(L, 5, 0);
	Lua_PushNumber(L, 1);
	return 1;
}

// SC_SetBotMate(idx, szMateName) - ten phu the. Chuoi rong = doc than.
int LuaSC_SetBotMate(Lua_State* L)
{
	KNpc* p = sc_Bot(L, 1);
	if (!p || Lua_GetTopIndex(L) < 2 || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	char* pMate = (char*)lua_tostring(L, 2);
	memset(p->MateName, 0, sizeof(p->MateName));
	if (pMate)
		strncpy(p->MateName, pMate, sizeof(p->MateName) - 1);
	Lua_PushNumber(L, 1);
	return 1;
}

// ============================================================================
// GD3 - bot noi chuyen
// ============================================================================

// SC_LoadChat(szChatPath, szType) -> so cau da nap. 0 = that bai.
//   Doc chat.txt (2 cot: Type, Chat), gom cac cau co Type = szType.
//   Vi du: SC_LoadChat("/settings/simcity/chat.txt", "general")
int LuaSC_LoadChat(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 || !Lua_IsString(L, 1) || !Lua_IsString(L, 2))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	char* szFile = (char*)lua_tostring(L, 1);
	char* szType = (char*)lua_tostring(L, 2);
	if (!szFile || !szFile[0] || !szType || !szType[0])
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	// LoadChat CONG DON. Nap lai dung tep+type se nhan doi kho cau (loi C1) - chan tai day
	// thay vi dua vao co SC_ChatLoaded ben Lua (co do mat khi nap lai script).
	if (s_chatCount > 0 && strcmp(s_chatFile, szFile) == 0 && strcmp(s_chatType, szType) == 0)
	{
		Lua_PushNumber(L, s_chatCount);
		return 1;
	}

	KTabFile tab;
	if (!tab.Load((LPSTR)szFile))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int hgt = tab.GetHeight();
	char szTy[64];
	for (int row = 2; row <= hgt && s_chatCount < SC_MAX_CHAT; row++)   // row 1 = header
	{
		tab.GetString(row, 1, (LPSTR)"", szTy, sizeof(szTy));
		if (strcmp(szTy, szType) != 0)
			continue;
		tab.GetString(row, 2, (LPSTR)"", s_chat[s_chatCount], SC_CHAT_LEN);
		// LOC NGAY LUC NAP (loi B5-chat): dem SC_CHAT_LEN = 68 nen cau 65..67 byte VAO duoc kho
		// nhung se bi chan "nLen <= 64" luc noi -> moi lan roll trung o do la mot lan bot cam
		// khong ro nguyen nhan. Loc o day thi kho cau chi chua cau noi duoc.
		int n = (int)strlen(s_chat[s_chatCount]);
		if (n > 0 && n <= 64)
			s_chatCount++;
		else
			s_chat[s_chatCount][0] = 0;
	}

	strncpy(s_chatFile, szFile, sizeof(s_chatFile) - 1); s_chatFile[sizeof(s_chatFile) - 1] = 0;
	strncpy(s_chatType, szType, sizeof(s_chatType) - 1); s_chatType[sizeof(s_chatType) - 1] = 0;

	Lua_PushNumber(L, s_chatCount);
	return 1;
}

// SC_ChatChance(n) -> n. n/1000 co hoi moi giay moi bot se noi 1 cau. 0 = tat.
int LuaSC_ChatChance(Lua_State* L)
{
	int n = 0;
	if (Lua_GetTopIndex(L) >= 1)
		n = (int)Lua_ValueToNumber(L, 1);
	if (n < 0)    n = 0;
	if (n > 1000) n = 1000;
	s_chatChance = n;
	Lua_PushNumber(L, s_chatChance);
	return 1;
}

// SC_ClearChat() -> xoa kho cau thoai (goi truoc khi doi sang Type khac)
int LuaSC_ClearChat(Lua_State* L)
{
	s_chatCount   = 0;
	s_chatFile[0] = 0;
	s_chatType[0] = 0;
	Lua_PushNumber(L, 1);
	return 1;
}

// ============================================================================
// SC_Breathe() - goi moi tick tu CoreServerShell::Breathe.
// ============================================================================
void SC_Breathe()
{
	s_tick++;

	int bChatOn = (s_chatChance > 0 && s_chatCount > 0);
	if (!g_bSimCityMove && !bChatOn)
		return;

	int w = 0;
	for (int r = 0; r < s_botCount; r++)
	{
		SC_Bot b = s_bots[r];
		int i = b.nNpcIdx;

		// chot bot con song + dung danh tinh; entry hong -> khong copy sang w (bi nen)
		if (i <= 0 || i >= MAX_NPC
			|| !Npc[i].m_btSimCityBot
			|| !Npc[i].m_Index
			|| !Npc[i].IsMatch(b.dwID)
			|| Npc[i].m_RegionIndex < 0
			|| Npc[i].m_SubWorldIndex < 0)
			continue;

		// ---- GD3: bot noi chuyen ----
		// Nhip RAI DEU theo chi so NPC thay vi mot nhip chung: (s_tick + i) % GAME_FPS.
		// Ly do: nhip chung lam TAT CA bot roll cung mot tick, ma bo dem nhip cua server la
		// "cong bu" (KSOServer.cpp:1076-1080) nen sau moi lan lag, MainLoop chay lien tuc va
		// don cuc ca chum tin nhan vao vai ms. Rai deu thi moi tick chi ~1/18 so bot roll.
		// Ep unsigned de khong bao gio ra so am khi s_tick tran (~3,8 nam uptime).
		if (bChatOn && ((DWORD)(s_tick + i) % (DWORD)GAME_FPS) == 0
			&& Npc[i].m_CurrentLife > 0 && Npc[i].m_Doing != do_death
			&& (int)g_Random(1000) < s_chatChance)
		{
			// g_Random la LCG rac o bit thap (KRandom.cpp:32-43: s = s*3877+29573, roi % nMax)
			// nen g_Random(n) di het n gia tri theo mot THU TU CO DINH = phat lai danh sach.
			// Tron them chi so NPC de moi bot doc kho cau tu mot vi tri khac nhau.
			int nPick = (int)((g_Random(s_chatCount) + i * 37) % s_chatCount);
			char* pMsg = s_chat[nPick];
			int   nLen = (int)strlen(pMsg);
			if (nLen > 0 && nLen <= 64)     // la chan chong tran buffer client (xem SC_MAX_CHAT)
				KPlayerChat::NpcChat(i, pMsg, nLen, FALSE);
		}

		if (!g_bSimCityMove || b.nRouteId < 0 || b.nRouteId >= s_routeCount)
		{
			s_bots[w++] = b;
			continue;
		}

		SC_Route& rt = s_routes[b.nRouteId];
		if (rt.nCount < 2)
		{
			s_bots[w++] = b;
			continue;
		}

		if (b.nLeg < 0)          b.nLeg = 0;
		if (b.nLeg >= rt.nCount) b.nLeg = 0;

		int nx, ny;
		Npc[i].GetMpsPos(&nx, &ny);

		// ---- CONG CHAN 1: region NGUOI ----
		// Region nguoi thi KSubWorld::Activate khong goi Activate cho NPC (KSubWorld.cpp:900-905)
		// nen ProcCommand khong bao gio tieu thu lenh -> bot nam o do_stand. Khong duoc doc
		// trang thai do la "engine bo cuoc", neu khong se dot het retry roi tien chang lien tuc:
		// bo vang vai phut la bo dem chang chay vong quanh lo trinh nhieu lan du bot chua he
		// nhuc nhich, va khi co nguoi toi thi bot khoi dong tu mot chang bat ky.
		// ---- CONG CHAN 2: bot bat dong HOP LE (choang / dong bang / khoa dong tac / speed 0)
		if (!sc_RegionHot(i) || sc_Immobilized(i))
		{
			b.nLastX = nx;
			b.nLastY = ny;
			s_bots[w++] = b;
			continue;
		}

		int tx = rt.wpX[b.nLeg];
		int ty = rt.wpY[b.nLeg];

		__int64 nArrive2 = (__int64)SC_ARRIVE_MPS * SC_ARRIVE_MPS;

		if (sc_Dist2(nx, ny, tx, ty) <= nArrive2)
		{
			// toi NODE -> sang chang ke
			sc_NextLeg(b, rt);
			sc_IssueStep(b, rt, i);
			b.nGiveUp = 0;
		}
		else
		{
			int bMoved = (nx != b.nLastX || ny != b.nLastY);
			if (bMoved)
			{
				b.nRetry = 0;
				b.nStuck = 0;
			}
			else if (Npc[i].m_Doing == do_walk)
			{
				b.nStuck++;     // ket kieu B: buoc bi khoi phuc o bien region, m_Doing giu do_walk
			}

			if (sc_Dist2(nx, ny, b.nTgtX, b.nTgtY) <= nArrive2)
			{
				// Toi DIEM TRUNG GIAN (chua toi node). Phat buoc ke = NAP LAI TIN DUNG NE.
				// Day la duong chinh cua ca ban va: bot di het chang dai bang nhieu lenh ngan,
				// moi lenh duoc engine cho mot lan ne vat can (xem khoi SC_STEP_MPS).
				sc_IssueStep(b, rt, i);
				b.nGiveUp = 0;
			}
			else if (b.nRetryCd > 0)
			{
				b.nRetryCd--;
			}
			else if (Npc[i].m_Doing == do_stand || b.nStuck >= SC_STUCK_TICK)
			{
				// Ket THAT: chua toi diem trung gian ma engine da dung bot lai.
				if (b.nRetry < SC_MAX_RETRY)
				{
					b.nRetry++;
					b.nStuck   = 0;
					b.nRetryCd = SC_RETRY_CD;
					// Ne NGANG (vuong goc) - xem sc_TryDetour: chi doi HUONG moi doi duoc phep
					// thu cua GetDir. Khong tim duoc huong ne thi it ra phat lai buoc (reset).
					if (!sc_TryDetour(b, i, nx, ny, tx, ty))
						sc_IssueStep(b, rt, i);
				}
				else
				{
					// BO CUOC. KHONG duoc tien chang mu quang: node preset la buoc di tren do thi
					// (do that: 13.984/13.984 chang preset deu la canh trong _nodes.txt), bot dang
					// o ngoai do thi nen chang ke con kho hon chang nay.
					// Bam lai node GAN NHAT; neu node gan nhat chinh la node dang ket thi moi bo.
					b.nGiveUp++;
					if (b.nGiveUp >= SC_GIVEUP_STOP)
					{
						b.nRouteId = -1;         // dung han - khoi dot CPU va goi tin vo ich
					}
					else
					{
						int nNear = sc_NearestLeg(rt, nx, ny, NULL);
						if (nNear == b.nLeg)
							sc_NextLeg(b, rt);
						else
							b.nLeg = nNear;
						sc_IssueStep(b, rt, i);
					}
				}
			}
		}

		b.nLastX = nx;
		b.nLastY = ny;
		s_bots[w++] = b;
	}
	s_botCount = w;
}

#endif // _SERVER
