# -*- coding: utf-8 -*-
r"""goi_va_bot_5bang.py - [BOTBANG5 02/09] NAM BANG BOT: tao bang - tuyen 100-150 bot/bang - dung
18h ngay bao danh dau thau cong thanh + lenh bai admin nap quy bang.

Chu game 02/09 chieu: "lam 1 - 6; 5 ten bat ky tuy ban; giu nguyen luat 5 ngay; quy thi hay viet
lenh bai admin toi se tu nap vao bang toi muon; hay lam va phan bien va note day du lai".

Vá 4 tệp C++ (latin-1, giữ CRLF, không thêm byte cao) + 1 script Lua (TCVN3 qua vn()) + 2 tệp dữ liệu:
  1. KPlayerBot.cpp   : truong nBangLoai/nBangBotIdx; pb_XinVaoBang loai 1 (TAO BANG); khoi [BOTBANG5]
                        (bang cau hinh, pb_BangThau, pb_BangNhip, PB_Bang*, LuaPB_Bang*); hook pb_DriveBot + PB_Breathe
  2. KTongJX2.cpp     : KTongJX2_SendCreateC - Core tu gui STONG_CREATE_COMMAND len relay (y het KSOServer.cpp:4327)
  3. KJx2CityWar.cpp/h: sSignUpArena(L) -> sSignUpArena(nPlayerIndex) tra ma; export SignUpArenaC + GetState
  4. ScriptFuns.cpp   : dang ky 7 ham Lua PB_Bang*
  5. simcity_admin.lua: menu "Bang hoi bot" (tao / tuyen / dau thau / nap quy)
  6. settings\simcity\botbang.txt (5 ten) + bin\multiserver\relay_config.ini tongcap 30 -> 160
Idempotent: tep da co dau [BOTBANG5 thi bo qua. --thu = chi kiem neo, khong ghi.
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn  # tieng Viet -> chuoi latin-1 mang byte TCVN3

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"
RUN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
MIR_SCRIPT = r"D:\GAMEDEVNEW\serverscript_jx2\jx1_edits"
MIR_DATA = r"D:\GAMEDEVNEW\serverdata_jx2\settings\simcity"
MARK = "[BOTBANG5"
THU = "--thu" in sys.argv


def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def wr(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


class Hunks(object):
    def __init__(self, path):
        self.path = path
        self.s = rd(path)
        self.crlf = "\r\n" in self.s
        self.n = 0
        self.err = 0

    def nl(self, t):
        return t.replace("\n", "\r\n") if self.crlf else t

    def sub(self, pat, repl, count=1, flags=re.M):
        """regex pat (xuong dong viet bang \\n THAT hoac chuoi raw '\\n') -> repl; phai khop DUNG count lan.
        Tep CRLF: moi \\n trong pat thanh \\r?\\n; [^\\n] thanh [^\\r\\n]. repl chuoi duoc expand (\\1...)."""
        p = pat.replace("[^\n]", "[^\r\n]").replace("[^\\n]", "[^\\r\\n]")
        p = p.replace("\n", "\r?\n").replace("\\n", "\\r?\\n")
        found = len(re.findall(p, self.s, flags))
        if found != count:
            print("  NEO KHONG KHOP (%d thay vi %d): %s" % (found, count, pat[:70].replace("\n", "|")))
            self.err += 1
            return
        # nhom bat (\1) mang san \r\n cua tep -> bo \r truoc khi nl() (da dinh: \r\r\n = 16 CR le, 02/09)
        def _thay(m):
            # KHONG dung m.expand(): no doi '\n' '\t' trong chuoi thay the thanh ky tu that -> pha
            # literal \n trong chuoi C/Lua. Chi mo rong \1..\9.
            t = repl(m) if callable(repl) else re.sub(r"\\(\d)", lambda g: m.group(int(g.group(1))), repl)
            return self.nl(t.replace("\r\n", "\n"))
        self.s = re.sub(p, _thay, self.s, count=0, flags=flags)
        self.n += found

    def ins_before(self, anchor_pat, block):
        self.sub(anchor_pat, lambda m: block + m.group(0).replace("\r\n", "\n"))

    def ins_after(self, anchor_pat, block):
        self.sub(anchor_pat, lambda m: m.group(0).replace("\r\n", "\n") + block)

    def done(self, name):
        if self.err:
            print("%s: %d neo hong -> KHONG GHI" % (name, self.err))
            return False
        if THU:
            print("%s: %d hunk OK (thu)" % (name, self.n))
            return True
        s0 = rd(self.path)
        if hb(s0) != hb(self.s):
            print("%s: SO BYTE CAO DOI %d -> %d -> KHONG GHI" % (name, hb(s0), hb(self.s)))
            return False
        wr(self.path, self.s)
        print("%s: AP %d hunk, high-byte %d, %d -> %d byte" % (name, self.n, hb(self.s), len(s0), len(self.s)))
        return True


# ============================================================================ KPlayerBot.cpp
KPB_KHOI = r'''
// =====================================================================================
// [BOTBANG5 02/09] NAM BANG BOT (chu game: "tao 5 bang cho bot, chi bot vao 5 bang nay, moi bang
// 100-150 bot; tao bang - tuyen thanh vien - dung ngay dung gio dang ky cong thanh").
// Cau hinh: \settings\simcity\botbang.txt (STT TEN CAMP THANH CAPMIN SONGUOI PHI, ten TCVN3 <= 12
// byte = defTONG_NAME_MAX_LENGTH). Khong dung giao thuc moi:
//  - TAO BANG: bot bang chu di dung luong xin bang (pb_XinVaoBang B1-B4: roi nhom, ve map 53, toi
//    NPC mon phai, XUAT SU) roi Core gui STONG_CREATE_COMMAND len relay (KTongJX2_SendCreateC - y het
//    GameServer.exe KSOServer.cpp:4327 lam cho nguoi that). Relay tra ve enumS2C_TONG_CREATE_SUCCESS
//    -> SSOI_TONG_CREATE -> KPlayerTong::Create nhu thuong. Khong qua CheckCreateCondition vi bot
//    co lead level 1 (KPlayer.cpp:175) - moi thu khac (cap, tien, item 195) relay khong kiem.
//  - TUYEN: ung vien di pb_XinVaoBang (nBangLoai 2, khong PM) -> nop don (hang doi GS 64);
//    bang chu bot duyet bang chinh COP_ACCEPT_APPLY (DoClientOpBody) 1 don/giay/bang; don cua
//    NGUOI THAT -> COP_REFUSE_APPLY (bang chi co bot). Relay tongcap phai >= muc tieu.
//  - DAU THAU: 18h00-18h55 ngay bao danh cua thanh muc tieu (TB_CTC6 timerserver_ctc.lua), state
//    SIGNUP: bang chu bot toi NPC Su Gia Cong Thanh (map 53) roi KJx2CityWar_SignUpArenaC (10 cua y
//    goc: bang chu, >= 37 nguoi, field 6 >= 18, quy >= phi...). Quy do chu game nap (PB_BangNap).
// =====================================================================================
extern void    KTongJX2_SendCreateC(int nPlayerIdx, int nCamp, const char* pszTongName);
extern __int64 KTongJX2_GetMoneyC(DWORD dwTongID);
extern void    KTongJX2_AddMoneyC(DWORD dwTongID, __int64 nDelta);
extern int     KJx2CityWar_SignUpArenaC(int nPlayerIdx, int nCity, int nFee);
extern int     KJx2CityWar_GetState(int nCity);

#define PB_BB_MAX          8
#define PB_BB_FILE         "\\settings\\simcity\\botbang.txt"
#define PB_BB_MAP_SUGIA    53                 // NPC Su Gia Cong Thanh (citywar_boot.lua:16)
#define PB_BB_SUGIA_X      1625
#define PB_BB_SUGIA_Y      3170
#define PB_BB_SONG_SONG    8                  // so bot cung luc dang di xin vao MOT bang
#define PB_BB_TAO_HAN_MS   60000              // cho relay tra loi lenh tao bang
#define PB_BB_STATE_SIGNUP 1                  // = JX2CW_STATE_SIGNUP (KJx2CityWar.h)

struct PB_BotBang
{
	int          nStt;
	char         szTen[32];       // ten bang (byte TCVN3)
	int          nCamp;           // 1 chinh / 2 ta / 3 trung lap
	int          nThanh;          // thanh muc tieu 1..7 (0 = khong dau thau)
	int          nCapMin;         // cap toi thieu bot vao bang
	int          nSoNguoi;        // muc tieu so thanh vien (mac dinh cho PB_BangTuyen)
	int          nPhi;            // phi dau thau (luong)
	DWORD        dwTongID;        // g_FileName2Id(szTen)
	int          nTao;            // 0 chua co; 1 dang tao; 2 da co tren GS; 3 tao that bai
	int          nTaoLeaderIdx;   // bot duoc giao lam bang chu (luc dang tao)
	DWORD        dwTaoLeaderID;
	unsigned int uTaoGui;         // GetTickCount luc gui STONG_CREATE_COMMAND (0 = chua)
	int          nTaoThu;
	int          nTuyenMuc;       // > 0: chien dich tuyen dang chay, muc tieu so thanh vien
	unsigned int uTuyenTick;
	int          nTuyenDaGiao;
	int          nThauNgay;       // yyyymmdd da xu ly dau thau (khong lap trong ngay)
	int          nThauKq;
	unsigned int uThauNghi;       // GetTickCount: thau that bai / dang di -> nghi toi moc nay
	unsigned int uLogTick;        // gion log
};
static PB_BotBang s_bb[PB_BB_MAX];
static int        s_bbCount = 0;
static int        s_bbLoaded = 0;
static int        s_nPbBangCTC = 0;       // cong tac dau thau theo gio
// thu BAO DANH (%w, 0 = CN) cua tung thanh 1..7 = cot 1 TB_CTC6 (timerserver_ctc.lua:10)
static const int  s_nBbLich[7] = { 3, 1, 2, 5, 4, 0, 6 };

static void pb_BbNap(int bLai)
{
	if (s_bbLoaded && !bLai)
		return;
	KTabFile tab;
	memset(s_bb, 0, sizeof(s_bb));
	s_bbCount = 0;
	s_bbLoaded = 1;
	if (!tab.Load((LPSTR)PB_BB_FILE))
	{
		pb_Log("[BotBang5] KHONG doc duoc %s\n", PB_BB_FILE);
		return;
	}
	const int nH = tab.GetHeight();
	for (int row = 2; row <= nH && s_bbCount < PB_BB_MAX; row++)   // row 1 = tieu de
	{
		PB_BotBang& bb = s_bb[s_bbCount];
		tab.GetInteger(row, 1, 0, &bb.nStt);
		tab.GetString(row, 2, (LPSTR)"", bb.szTen, sizeof(bb.szTen));
		tab.GetInteger(row, 3, 3, &bb.nCamp);
		tab.GetInteger(row, 4, 0, &bb.nThanh);
		tab.GetInteger(row, 5, 60, &bb.nCapMin);
		tab.GetInteger(row, 6, 120, &bb.nSoNguoi);
		tab.GetInteger(row, 7, 1000000, &bb.nPhi);
		bb.szTen[31] = 0;
		if (!bb.szTen[0] || (int)strlen(bb.szTen) > defTONG_NAME_MAX_LENGTH)
		{
			pb_Log("[BotBang5] %s dong %d: ten bang rong hoac dai qua %d byte -> bo\n", PB_BB_FILE, row, defTONG_NAME_MAX_LENGTH);
			continue;
		}
		if (bb.nCamp < 1 || bb.nCamp > 3)  bb.nCamp = 3;
		if (bb.nThanh < 0 || bb.nThanh > 7) bb.nThanh = 0;
		if (bb.nCapMin < 60)               bb.nCapMin = 60;      // xuat su doi cap >= 60 (thieulam.lua:19)
		if (bb.nPhi < 1000000)             bb.nPhi = 1000000;    // citywar.ini SignUpFee
		bb.dwTongID = g_FileName2Id(bb.szTen);
		bb.nTao = g_TongJX2.FindTong(bb.dwTongID) ? 2 : 0;
		s_bbCount++;
	}
	pb_Log("[BotBang5] nap %d bang bot tu %s\n", s_bbCount, PB_BB_FILE);
}

// bot du tu cach vao bang bot / lam bang chu bang bot
static int pb_BbUngVien(const PB_Bot& b, int nCapMin)
{
	if (b.nBanSap || b.nNhomNguoiIdx || b.nTk || b.nBangPha)   return 0;
	if (b.nDtPha != DTB_NGHI)                                   return 0;  // dang lam Da Tau
	if (b.nAi != PB_AI_FIGHT || b.nFaction < 0)                 return 0;
	const int p = b.nPlayerIdx;
	if (p <= 0 || p >= MAX_PLAYER || Player[p].m_dwID != b.dwID) return 0;
	if (Player[p].m_cTong.m_nFlag)                              return 0;  // da co bang
	const int n = Player[p].m_nIndex;
	if (n <= 0 || n >= MAX_NPC || Npc[n].m_dwID == 0)           return 0;
	if (Npc[n].m_Doing == do_death || Npc[n].m_Doing == do_revive) return 0;
	if (Npc[n].m_Level < nCapMin)                               return 0;
	return 1;
}

static PB_Bot* pb_BbBotCuaPlayer(int nPlayerIdx)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return NULL;
	for (int i = 0; i < s_botCount; i++)
		if (s_bots[i].nPlayerIdx == nPlayerIdx && Player[nPlayerIdx].m_dwID == s_bots[i].dwID)
			return &s_bots[i];
	return NULL;
}

// bang chu (btFigure 0) cua bang bot -> Player idx bot (0 = chua dong bo / bang chu khong phai bot)
static int pb_BbLeaderIdx(const PB_BotBang& bb)
{
	KTongJX2Tong* pT = g_TongJX2.FindTong(bb.dwTongID);
	if (!pT)
		return 0;
	for (std::map<DWORD, KTongJX2Member>::iterator it = pT->mapMember.begin(); it != pT->mapMember.end(); ++it)
		if (it->second.btFigure == 0)
			return pb_FindBotByName(it->second.szName);
	return 0;
}

static void pb_BbXoaViec(PB_Bot& b)
{
	b.nBangPha = 0;  b.dwBangID = 0;  b.nBangNguoiIdx = 0;  b.dwBangNguoiID = 0;
	b.nBangTick = 0;  b.nBangThu = 0;  b.nBangNghiToi = 0;  b.nBangDoiMapTick = 0;
	b.nBangLoai = 0;  b.nBangBotIdx = -1;
	b.walk.Reset();
}

// relay da tra ve KPlayerTong::Create cho bot bang chu (m_cTong.m_nFlag = 1)
static void pb_BbTaoXong(int nIdx, PB_Bot& b)
{
	const int k = b.nBangBotIdx;
	if (k >= 0 && k < s_bbCount)
	{
		PB_BotBang& bb = s_bb[k];
		if (Player[nIdx].m_cTong.GetTongNameID() == bb.dwTongID)
		{
			bb.nTao = 2;
			pb_Log("[BotBang5] TAO BANG XONG: %s la bang chu bang '%s' (id %u, camp %d) - cho relay dong bo JX2 (<= 30 giay) roi moi tuyen duoc\n",
			       Player[nIdx].m_PlayerName, bb.szTen, (unsigned int)bb.dwTongID, (int)Player[nIdx].m_cTong.m_nCamp);
		}
		else
		{
			bb.nTao = 3;
			pb_Log("[BotBang5] %s co bang KHAC (id %u) trong luc dang tao bang '%s' -> bo\n",
			       Player[nIdx].m_PlayerName, (unsigned int)Player[nIdx].m_cTong.GetTongNameID(), bb.szTen);
		}
	}
	pb_BbXoaViec(b);
}

'''

KPB_B5 = r'''	// [BOTBANG5 02/09] nBangLoai 1: da XUAT SU -> gui lenh TAO BANG len relay roi cho KPlayerTong::Create
	// (bat o dau ham: m_nFlag = 1 -> pb_BbTaoXong). Relay tu choi (ten trung / bot da la bang chu noi
	// khac) thi chi bao client (bot khong co) -> qua PB_BB_TAO_HAN_MS coi nhu THAT BAI.
	if (b.nBangLoai == 1)
	{
		const int k = b.nBangBotIdx;
		if (k < 0 || k >= s_bbCount)
		{
			pb_BbXoaViec(b);
			return 0;
		}
		PB_BotBang& bb = s_bb[k];
		const unsigned int uMs = (unsigned int)GetTickCount();
		if (bb.uTaoGui == 0)
		{
			KTongJX2_SendCreateC(nIdx, bb.nCamp, bb.szTen);
			bb.uTaoGui = uMs ? uMs : 1;
			pb_Log("[BotBang5] %s (cap %d, camp %d) gui lenh TAO BANG '%s' (camp %d) len relay - cho tra loi\n",
			       Player[nIdx].m_PlayerName, Npc[nNpcIdx].m_Level, (int)Npc[nNpcIdx].m_CurrentCamp, bb.szTen, bb.nCamp);
			return 1;
		}
		if (uMs - bb.uTaoGui > (unsigned int)PB_BB_TAO_HAN_MS)
		{
			bb.nTao = 3;
			pb_Log("[BotBang5] %s: relay KHONG tao bang '%s' sau %d giay (ten trung? bot da la bang chu noi khac? relay tat?) -> THAT BAI\n",
			       Player[nIdx].m_PlayerName, bb.szTen, PB_BB_TAO_HAN_MS / 1000);
			pb_BbXoaViec(b);
			return 0;
		}
		return 1;                          // dang cho relay
	}
'''

KPB_SAU = r'''// [BOTBANG5 02/09] --------------------------------------------------------------------------
static const char* pb_BbTenKq(int nKq)
{
	switch (nKq)
	{
	case 0:  return "OK - da bao danh dau thau";
	case 1:  return "bot chua co bang (m_cTong trong)";
	case 2:  return "bang khong co tren GS";
	case 3:  return "bot khong phai bang chu";
	case 4:  return "thanh khong o pha bao danh (18h-19h ngay bao danh)";
	case 5:  return "bang da bao danh (thanh nay hoac thanh khac)";
	case 6:  return "bang dang la khieu chien gia";
	case 7:  return "bang dang la chu mot thanh";
	case 8:  return "bang chua du 37 nguoi (MinTongCrowNumber)";
	case 9:  return "cap bang (field 6 = kinh nghiem) chua du 18 - quy chua qua 1 trieu du 750 giay?";
	case 10: return "quy bang khong du phi";
	default: return "ma la";
	}
}

// Bang chu bot di DAU THAU tai NPC Su Gia Cong Thanh (map 53). Tra 1 = dang ban, 0 = xong/huy.
// Khuon pb_XinVaoBang: ChangeWorld so le, PB_WalkTo, A* thua 5 lan -> SetPos canh NPC.
static int pb_BangThau(int nIdx, int nNpcIdx, int nSub, PB_Bot& b, unsigned int now, int nLech)
{
	if (b.nBangTick == 0)
		b.nBangTick = now;
	const int k = b.nBangBotIdx;
	if (k < 0 || k >= s_bbCount || now - b.nBangTick > (unsigned int)(GAME_FPS * 300))
	{
		pb_Log("[BotBang5] %s HUY di dau thau (bang %d / het han 5 phut)\n", Player[nIdx].m_PlayerName, k);
		pb_BbXoaViec(b);
		return 0;
	}
	PB_BotBang& bb = s_bb[k];
	if (pb_TrongNhom(nIdx))
		pb_RoiNhomNguoi(nIdx, b, "di dau thau cong thanh");
	const int nSubSg = g_SubWorldSet.SearchWorld(PB_BB_MAP_SUGIA);
	if (nSubSg < 0 || nSubSg >= MAX_SUBWORLD)
		return 1;
	const int nSg = pb_TimNpc(nSubSg, "sugia_congthanh", PB_BB_SUGIA_X * 32, PB_BB_SUGIA_Y * 32);
	if (nSg <= 0)
	{
		if (now - bb.uLogTick > (unsigned int)(GAME_FPS * 30))
		{
			bb.uLogTick = now;
			pb_Log("[BotBang5] %s: KHONG thay NPC Su Gia Cong Thanh (sugia_congthanh) tren map %d\n",
			       Player[nIdx].m_PlayerName, PB_BB_MAP_SUGIA);
		}
		return 1;
	}
	int nx = 0, ny = 0;
	Npc[nSg].GetMpsPos(&nx, &ny);
	if (nSubSg != nSub)
	{
		if (b.nBangDoiMapTick && now - b.nBangDoiMapTick < (unsigned int)(GAME_FPS * (3 + (nLech % 3))))
			return 1;
		b.nBangDoiMapTick = now;
		int vx = 0, vy = 0;
		if (pb_ODat(nSubSg, nx / 32, ny / 32, nLech, 8, &vx, &vy))
		{
			const int nRet = Npc[nNpcIdx].ChangeWorld(SubWorld[nSubSg].m_SubWorldID, vx, vy);
			pb_Log("[BotBang5] %s dich chuyen ve map %d canh NPC Su Gia de dau thau thanh %d (ret=%d)\n",
			       Player[nIdx].m_PlayerName, PB_BB_MAP_SUGIA, bb.nThanh, nRet);
		}
		b.walk.Reset();
		return 1;
	}
	if (b.nBangNghiToi && now < b.nBangNghiToi)
		return 1;
	const int nW = PB_WalkTo(nNpcIdx, nx, ny, nSub, b.walk, PB_FAC_ARRIVE_MPS);
	if (nW == 0)
		return 1;
	if (nW < 0)
	{
		b.walk.Reset();
		b.nBangNghiToi = now + (unsigned int)PB_FAC_RETRY_TICK;
		if (++b.nBangThu >= PB_FAC_MAX_RETRY)
		{
			int dx3 = 0, dy3 = 0;
			if (pb_ODat(nSub, nx / 32, ny / 32, nLech, 6, &dx3, &dy3))
				Npc[nNpcIdx].SetPos(dx3, dy3);
			b.nBangThu = 0;
		}
		return 1;
	}
	Npc[nNpcIdx].SendCommand(do_stand);
	const int nKq = KJx2CityWar_SignUpArenaC(nIdx, bb.nThanh, bb.nPhi);
	time_t tNow = time(NULL);
	struct tm* pTm = localtime(&tNow);
	const int nToday = pTm ? ((pTm->tm_year + 1900) * 10000 + (pTm->tm_mon + 1) * 100 + pTm->tm_mday) : 0;
	bb.nThauKq = nKq;
	if (nKq == 0 || nKq == 5 || nKq == 6 || nKq == 7)
		bb.nThauNgay = nToday;                                   // xong / khong con gi de thu hom nay
	else
		bb.uThauNghi = (unsigned int)GetTickCount() + 60000;    // thieu quy... -> 60 giay sau thu lai neu con trong cua so
	pb_Log("[BotBang5] %s DAU THAU thanh %d cho bang '%s' phi %d: ket qua %d (%s), quy bang %I64d\n",
	       Player[nIdx].m_PlayerName, bb.nThanh, bb.szTen, bb.nPhi, nKq, pb_BbTenKq(nKq), KTongJX2_GetMoneyC(bb.dwTongID));
	pb_BbXoaViec(b);
	return 0;
}

// Nhip 1 giay: (1) bang chu bot duyet don, (2) chien dich tuyen, (3) 18h dau thau theo lich.
static void pb_BangNhip()
{
	static DWORD s_dwMoc = 0;
	const DWORD dwNow = GetTickCount();
	if (s_dwMoc && dwNow - s_dwMoc < 1000)
		return;
	s_dwMoc = dwNow;
	if (!s_bbLoaded || s_botCount <= 0)
		return;
	static int s_nXoay = 0;
	for (int k = 0; k < s_bbCount; k++)
	{
		PB_BotBang& bb = s_bb[k];
		KTongJX2Tong* pT = g_TongJX2.FindTong(bb.dwTongID);
		// dang tao ma bot bang chu da bo cuoc (HUY trong pb_XinVaoBang) va bang van chua co -> that bai
		if (bb.nTao == 1 && !pT)
		{
			PB_Bot* pL = pb_BbBotCuaPlayer(bb.nTaoLeaderIdx);
			if (!pL || pL->dwID != bb.dwTaoLeaderID || (pL->nBangPha == 0 && !Player[bb.nTaoLeaderIdx].m_cTong.m_nFlag))
			{
				bb.nTao = 3;
				pb_Log("[BotBang5] tao bang '%s': bot bang chu bo cuoc giua chung (xem [BotBang] HUY) -> THAT BAI, bam Tao lai\n", bb.szTen);
			}
		}
		if (!pT)
			continue;
		if (bb.nTao != 2)
		{
			bb.nTao = 2;
			pb_Log("[BotBang5] bang '%s' (id %u) da co tren GS: %d thanh vien\n", bb.szTen, (unsigned int)bb.dwTongID, (int)pT->mapMember.size());
		}
		// (1) duyet don: 1 don / giay / bang. Don cua NGUOI THAT -> tu choi (bang chi co bot - chu game).
		if (pT->btApplyCount > 0)
		{
			const int nL = pb_BbLeaderIdx(bb);
			if (nL > 0)
			{
				char szAi[32];
				memcpy(szAi, pT->szApplyName[0], 32);
				szAi[31] = 0;
				const DWORD dwAi = pT->dwApplyID[0];
				const int bBot = (pb_FindBotByName(szAi) > 0) ? 1 : 0;
				TONG_JX2OP_COMMAND sCmd;
				memset(&sCmd, 0, sizeof(sCmd));
				sCmd.m_btOp = bBot ? defTONG_JX2_COP_ACCEPT_APPLY : defTONG_JX2_COP_REFUSE_APPLY;
				sCmd.m_dwTarget = dwAi;
				int nKq = g_TongJX2.DoClientOpBody(nL, &sCmd);
				if (nKq == 5 && bBot)
				{
					// nguoi xin dang online ma DA co bang khac: ACCEPT tra 5 KHONG xoa don -> don ket
					// o dau hang mai. Tu choi de xoa.
					sCmd.m_btOp = defTONG_JX2_COP_REFUSE_APPLY;
					nKq = g_TongJX2.DoClientOpBody(nL, &sCmd);
				}
				pb_Log("[BotBang5] bang '%s': bang chu %s %s don cua %s%s (ket qua %d, con %d don, %d thanh vien)\n",
				       bb.szTen, Player[nL].m_PlayerName, bBot ? "DUYET" : "TU CHOI", szAi,
				       bBot ? "" : " (khong phai bot)", nKq, (int)pT->btApplyCount, (int)pT->mapMember.size());
			}
			else if (dwNow - bb.uLogTick > 30000)
			{
				bb.uLogTick = dwNow;
				pb_Log("[BotBang5] bang '%s': co %d don nhung khong thay bang chu la bot dang song -> chua duyet\n",
				       bb.szTen, (int)pT->btApplyCount);
			}
		}
		// (2) chien dich tuyen
		if (bb.nTuyenMuc > 0)
		{
			const int nCo = (int)pT->mapMember.size();
			if (nCo >= bb.nTuyenMuc)
			{
				pb_Log("[BotBang5] bang '%s': TUYEN XONG %d/%d thanh vien (da giao %d bot)\n", bb.szTen, nCo, bb.nTuyenMuc, bb.nTuyenDaGiao);
				bb.nTuyenMuc = 0;
				continue;
			}
			if (bb.nTuyenDaGiao >= bb.nTuyenMuc + 30)
			{
				pb_Log("[BotBang5] bang '%s': da giao %d bot ma moi co %d/%d thanh vien -> DUNG (kiem tongcap relay_config.ini + dong [BotBang] HUY, roi Tuyen lai)\n",
				       bb.szTen, bb.nTuyenDaGiao, nCo, bb.nTuyenMuc);
				bb.nTuyenMuc = 0;
				continue;
			}
			int nDang = 0;
			for (int i = 0; i < s_botCount; i++)
				if (s_bots[i].nBangPha == 1 && s_bots[i].nBangLoai == 2 && s_bots[i].nBangBotIdx == k)
					nDang++;
			if (nDang >= PB_BB_SONG_SONG)
				continue;
			if (bb.uTuyenTick && dwNow - bb.uTuyenTick < 2000)
				continue;
			int nChon = -1;
			for (int j = 0; j < s_botCount; j++)
			{
				const int i = (s_nXoay + j) % s_botCount;
				if (pb_BbUngVien(s_bots[i], bb.nCapMin))
				{
					nChon = i;
					break;
				}
			}
			s_nXoay = (nChon >= 0) ? (nChon + 1) % s_botCount : (s_nXoay + 1) % s_botCount;
			if (nChon < 0)
			{
				if (dwNow - bb.uLogTick > 60000)
				{
					bb.uLogTick = dwNow;
					pb_Log("[BotBang5] bang '%s': het bot du tu cach (cap >= %d, chua bang, dang luyen cong, khong sap/TK/Da Tau/nhom nguoi) - moi co %d/%d\n",
					       bb.szTen, bb.nCapMin, nCo, bb.nTuyenMuc);
				}
				continue;
			}
			PB_Bot& b = s_bots[nChon];
			b.nBangPha = 1;  b.nBangLoai = 2;  b.nBangBotIdx = k;  b.dwBangID = bb.dwTongID;
			b.nBangNguoiIdx = 0;  b.dwBangNguoiID = 0;
			b.nBangTick = 0;  b.nBangThu = 0;  b.nBangNghiToi = 0;  b.nBangDoiMapTick = 0;
			bb.nTuyenDaGiao++;
			bb.uTuyenTick = dwNow;
			pb_Log("[BotBang5] bang '%s': giao %s (cap %d) di xin vao bang (%d/%d, dang di %d)\n",
			       bb.szTen, Player[b.nPlayerIdx].m_PlayerName, Npc[Player[b.nPlayerIdx].m_nIndex].m_Level,
			       nCo, bb.nTuyenMuc, nDang + 1);
		}
	}
	// (3) dau thau theo gio: 18h00-18h55 ngay bao danh cua thanh muc tieu, state SIGNUP; kiem 20 giay/lan
	static DWORD s_dwMocThau = 0;
	if (!s_nPbBangCTC || dwNow - s_dwMocThau < 20000)
		return;
	s_dwMocThau = dwNow;
	time_t tNow = time(NULL);
	struct tm* pTm = localtime(&tNow);
	if (!pTm)
		return;
	const int nToday = (pTm->tm_year + 1900) * 10000 + (pTm->tm_mon + 1) * 100 + pTm->tm_mday;
	if (pTm->tm_hour != 18 || pTm->tm_min > 55)
		return;
	for (int k = 0; k < s_bbCount; k++)
	{
		PB_BotBang& bb = s_bb[k];
		if (bb.nTao != 2 || bb.nThanh < 1 || bb.nThanh > 7 || bb.nThauNgay == nToday)
			continue;
		if (s_nBbLich[bb.nThanh - 1] != pTm->tm_wday)
			continue;
		if (bb.uThauNghi && dwNow < bb.uThauNghi)
			continue;
		if (KJx2CityWar_GetState(bb.nThanh) != PB_BB_STATE_SIGNUP)
			continue;
		const int nL = pb_BbLeaderIdx(bb);
		PB_Bot* pL = (nL > 0) ? pb_BbBotCuaPlayer(nL) : NULL;
		if (!pL)
		{
			if (dwNow - bb.uLogTick > 60000)
			{
				bb.uLogTick = dwNow;
				pb_Log("[BotBang5] 18h: bang '%s' khong co bang chu bot dang song -> khong dau thau thanh %d\n", bb.szTen, bb.nThanh);
			}
			continue;
		}
		if (pL->nBangPha || pL->nTk || pL->nBanSap)
			continue;                       // dang ban (Tong Kim...) -> 20 giay sau thu lai
		pL->nBangPha = 2;  pL->nBangLoai = 3;  pL->nBangBotIdx = k;  pL->dwBangID = bb.dwTongID;
		pL->nBangNguoiIdx = 0;  pL->dwBangNguoiID = 0;
		pL->nBangTick = 0;  pL->nBangThu = 0;  pL->nBangNghiToi = 0;  pL->nBangDoiMapTick = 0;
		pL->walk.Reset();
		bb.uThauNghi = dwNow + 120000;      // bot dang di - khong giao lai trong 2 phut
		pb_Log("[BotBang5] 18h%02d: bang chu %s cua '%s' di dau thau thanh %d (phi %d, quy %I64d, %d thanh vien)\n",
		       pTm->tm_min, Player[nL].m_PlayerName, bb.szTen, bb.nThanh, bb.nPhi,
		       KTongJX2_GetMoneyC(bb.dwTongID), (int)(g_TongJX2.FindTong(bb.dwTongID) ? g_TongJX2.FindTong(bb.dwTongID)->mapMember.size() : 0));
	}
}

// ---- lenh (lenh bai admin: simcity_admin.lua PB_BangMenu) ----
int PB_BangTao(int nSo)
{
	pb_BbNap(0);
	int nGiao = 0;
	for (int k = 0; k < s_bbCount && nGiao < nSo; k++)
	{
		PB_BotBang& bb = s_bb[k];
		if (g_TongJX2.FindTong(bb.dwTongID))
		{
			bb.nTao = 2;
			continue;
		}
		if (bb.nTao == 1)
			continue;                       // dang tao
		int nBest = -1, nBestLv = -1;
		for (int i = 0; i < s_botCount; i++)
		{
			if (!pb_BbUngVien(s_bots[i], bb.nCapMin))
				continue;
			const int lv = Npc[Player[s_bots[i].nPlayerIdx].m_nIndex].m_Level;
			if (lv > nBestLv)
			{
				nBestLv = lv;
				nBest = i;
			}
		}
		if (nBest < 0)
		{
			pb_Log("[BotBang5] tao bang '%s': KHONG co bot du tu cach lam bang chu (cap >= %d, chua bang, dang luyen cong)\n", bb.szTen, bb.nCapMin);
			continue;
		}
		PB_Bot& b = s_bots[nBest];
		b.nBangPha = 1;  b.nBangLoai = 1;  b.nBangBotIdx = k;  b.dwBangID = bb.dwTongID;
		b.nBangNguoiIdx = 0;  b.dwBangNguoiID = 0;
		b.nBangTick = 0;  b.nBangThu = 0;  b.nBangNghiToi = 0;  b.nBangDoiMapTick = 0;
		bb.nTao = 1;
		bb.nTaoLeaderIdx = b.nPlayerIdx;
		bb.dwTaoLeaderID = b.dwID;
		bb.uTaoGui = 0;
		bb.nTaoThu++;
		nGiao++;
		pb_Log("[BotBang5] TAO BANG '%s' (camp %d, thanh %d): giao %s (cap %d) ve NPC mon phai xuat su roi lap bang (lan %d)\n",
		       bb.szTen, bb.nCamp, bb.nThanh, Player[b.nPlayerIdx].m_PlayerName, nBestLv, bb.nTaoThu);
	}
	return nGiao;
}

int PB_BangTuyen(int nStt, int nMuc)
{
	pb_BbNap(0);
	const int k = nStt - 1;
	if (k < 0 || k >= s_bbCount)
		return -1;
	PB_BotBang& bb = s_bb[k];
	if (!g_TongJX2.FindTong(bb.dwTongID))
	{
		pb_Log("[BotBang5] tuyen cho '%s': bang chua co tren GS (chua tao / relay chua dong bo)\n", bb.szTen);
		return -2;
	}
	if (nMuc <= 0)
		nMuc = bb.nSoNguoi;
	bb.nTuyenMuc = nMuc;
	bb.nTuyenDaGiao = 0;
	bb.uTuyenTick = 0;
	pb_Log("[BotBang5] bang '%s': BAT DAU TUYEN toi %d thanh vien (cap >= %d, %d bot/luot)\n", bb.szTen, nMuc, bb.nCapMin, PB_BB_SONG_SONG);
	return nMuc;
}

int PB_BangCTC(int nBat)
{
	pb_BbNap(0);
	if (nBat >= 0)
	{
		s_nPbBangCTC = nBat ? 1 : 0;
		pb_Log("[BotBang5] dau thau cong thanh theo gio (18h00-18h55 ngay bao danh cua thanh muc tieu): %s\n", s_nPbBangCTC ? "BAT" : "TAT");
	}
	return s_nPbBangCTC;
}

// nStt 1..n = bang bot; nStt 0 = bang co NameID dwTongIDKhac (bang cua chinh admin - Lua GetTongName())
int PB_BangNap(int nStt, int nLuong, unsigned int dwTongIDKhac)
{
	pb_BbNap(0);
	if (nLuong <= 0 || nLuong > 2000000000)
		return 0;
	DWORD dwID = 0;
	if (nStt >= 1 && nStt <= s_bbCount)
		dwID = s_bb[nStt - 1].dwTongID;
	else
		dwID = (DWORD)dwTongIDKhac;
	KTongJX2Tong* pT = dwID ? g_TongJX2.FindTong(dwID) : NULL;
	if (!pT)
		return 0;
	const __int64 nTruoc = KTongJX2_GetMoneyC(dwID);
	KTongJX2_AddMoneyC(dwID, (__int64)nLuong);
	pb_Log("[BotBang5] ADMIN NAP QUY %d luong vao bang '%s' (id %u), quy truoc khi nap %I64d (relay dong bo lai sau vai giay)\n",
	       nLuong, pT->szName, (unsigned int)dwID, nTruoc);
	return 1;
}

static int pb_LuaSoNguyen(Lua_State* L, int nArg, int nMacDinh)
{
	if (Lua_GetTopIndex(L) >= nArg && Lua_IsNumber(L, nArg))
		return (int)Lua_ValueToNumber(L, nArg);
	return nMacDinh;
}

int LuaPB_BangTao(Lua_State* L)
{
	Lua_PushNumber(L, PB_BangTao(pb_LuaSoNguyen(L, 1, PB_BB_MAX)));
	return 1;
}

int LuaPB_BangTuyen(Lua_State* L)
{
	Lua_PushNumber(L, PB_BangTuyen(pb_LuaSoNguyen(L, 1, 0), pb_LuaSoNguyen(L, 2, 0)));
	return 1;
}

int LuaPB_BangCTC(Lua_State* L)
{
	Lua_PushNumber(L, PB_BangCTC(pb_LuaSoNguyen(L, 1, -1)));
	return 1;
}

int LuaPB_BangNap(Lua_State* L)
{
	unsigned int dwKhac = 0;
	if (Lua_GetTopIndex(L) >= 3 && Lua_IsNumber(L, 3))
		dwKhac = (unsigned int)(double)Lua_ValueToNumber(L, 3);
	Lua_PushNumber(L, PB_BangNap(pb_LuaSoNguyen(L, 1, 0), pb_LuaSoNguyen(L, 2, 0), dwKhac));
	return 1;
}

// PB_BangSo([1 = nap lai botbang.txt]) -> so bang bot
int LuaPB_BangSo(Lua_State* L)
{
	pb_BbNap(pb_LuaSoNguyen(L, 1, 0));
	Lua_PushNumber(L, s_bbCount);
	return 1;
}

int LuaPB_BangTen(Lua_State* L)
{
	pb_BbNap(0);
	const int k = pb_LuaSoNguyen(L, 1, 0);
	Lua_PushString(L, (k >= 1 && k <= s_bbCount) ? s_bb[k - 1].szTen : (char*)"");
	return 1;
}

// PB_BangTT() -> chuoi trang thai (ngan: Say/SayEx tran 512 byte)
int LuaPB_BangTT(Lua_State* L)
{
	pb_BbNap(0);
	static const char* s_szTao[4] = { "chua tao", "dang tao", "co", "LOI tao" };
	char sz[1024];
	int nLen = 0;
	sz[0] = 0;
	for (int k = 0; k < s_bbCount && nLen < 900; k++)
	{
		PB_BotBang& bb = s_bb[k];
		KTongJX2Tong* pT = g_TongJX2.FindTong(bb.dwTongID);
		const int nL = pb_BbLeaderIdx(bb);
		nLen += sprintf(sz + nLen, "%d.%s %s %dng %I64dtr T%d %s%s\n",
		                bb.nStt, bb.szTen, s_szTao[(bb.nTao >= 0 && bb.nTao <= 3) ? bb.nTao : 0],
		                pT ? (int)pT->mapMember.size() : 0,
		                pT ? KTongJX2_GetMoneyC(bb.dwTongID) / 1000000 : (__int64)0,
		                bb.nThanh, nL > 0 ? Player[nL].m_PlayerName : "-",
		                bb.nTuyenMuc > 0 ? " (tuyen)" : "");
	}
	Lua_PushString(L, sz);
	return 1;
}

'''


def va_kplayerbot():
    p = os.path.join(SRC, "KPlayerBot.cpp")
    h = Hunks(p)
    if MARK in h.s:
        print("KPlayerBot.cpp: DA AP")
        return True
    # H1 truong moi trong PB_Bot
    h.ins_after(r"^\tint\s+nBangThu;[^\n]*// so lan A\* thua lien tiep\n",
                "\tint          nBangLoai;                  // [BOTBANG5 02/09] 0 = xin bang theo PM; 1 = TAO bang bot; 2 = xin vao bang bot; 3 = di dau thau\n"
                "\tint          nBangBotIdx;                // chi so bang bot trong s_bb (-1 = khong)\n")
    # H2 reset luc sinh bot
    h.ins_after(r"^\t\tb\.nBangTick = 0;\s+b\.nBangNghiToi = 0;\s+b\.nBangDoiMapTick = 0;\s+b\.nBangThu = 0;[ \t]*\n",
                "\t\tb.nBangLoai = 0;  b.nBangBotIdx = -1;\n")
    # H3 4 cho reset trong pb_XinVaoBang
    h.sub(r"b\.nBangTick = 0;\s+b\.nBangThu = 0;\s+b\.nBangNghiToi = 0;\s+b\.nBangDoiMapTick = 0;",
          "b.nBangTick = 0;  b.nBangThu = 0;  b.nBangNghiToi = 0;  b.nBangDoiMapTick = 0;  b.nBangLoai = 0;  b.nBangBotIdx = -1;",
          count=4)
    # H4 PB_WhisperReply (luong PM cu) -> loai 0
    h.sub(r"pB->nBangTick = 0;\s+pB->nBangThu = 0;\s+pB->nBangNghiToi = 0;\s+pB->nBangDoiMapTick = 0;",
          "pB->nBangTick = 0;  pB->nBangThu = 0;  pB->nBangNghiToi = 0;  pB->nBangDoiMapTick = 0;  pB->nBangLoai = 0;  pB->nBangBotIdx = -1;",
          count=1)
    # H5 khoi cau hinh + helper, dat TRUOC pb_XinVaoBang
    h.ins_before(r'^// \[BOTBANG c 01/09\] "da xuat su" = TASK_DUNGCHUNG2 \(header\\taskid\.lua:26 = 4134, o \[9\] "xuat su"\);\n',
                 KPB_KHOI)
    # H6 dau pb_XinVaoBang: loai 1 khong coi "bang mat", m_nFlag = 1 la thanh cong
    h.sub(r"^\tconst int bDaCoBang = \(Player\[nIdx\]\.m_cTong\.m_nFlag != 0\);\n"
          r"\tconst int bBangMat  = \(g_TongJX2\.FindTong\(b\.dwBangID\) == NULL\);\n",
          "\tconst int bDaCoBang = (Player[nIdx].m_cTong.m_nFlag != 0);\n"
          "\t// [BOTBANG5 02/09] nBangLoai 1 = bot dang TAO bang: bang chua ton tai tren GS (khong phai \"bang mat\"),\n"
          "\t// va khi relay tra ve KPlayerTong::Create thi m_nFlag = 1 chinh la THANH CONG.\n"
          "\tif (b.nBangLoai == 1 && bDaCoBang)\n"
          "\t{\n"
          "\t\tpb_BbTaoXong(nIdx, b);\n"
          "\t\treturn 0;\n"
          "\t}\n"
          "\tconst int bBangMat  = (b.nBangLoai != 1) && (g_TongJX2.FindTong(b.dwBangID) == NULL);\n")
    # H7 truoc BUOC 5
    h.ins_before(r"^\t// ---- BUOC 5: nop don xin vao DUNG bang cua nguoi ru ----\n", KPB_B5)
    # H8 sau pb_XinVaoBang (truoc PB_WhisperReply)
    h.ins_before(r"^int PB_WhisperReply\(const PB_WHISPER\* p\)\n\{\n", KPB_SAU)
    # H9 hook pb_DriveBot
    h.sub(r"^\tif \(b\.nBangPha\)\n\t\{\n\t\tif \(pb_XinVaoBang\(nIdx, nNpcIdx, nSub, b, nowAll, \(int\)\(&b - s_bots\)\)\)\n\t\t\treturn;\n\t\}\n",
          "\tif (b.nBangPha)\n\t{\n"
          "\t\t// [BOTBANG5 02/09] nBangPha 2 = bang chu bot di dau thau cong thanh (NPC Su Gia, map 53)\n"
          "\t\tif (b.nBangPha == 2 ? pb_BangThau(nIdx, nNpcIdx, nSub, b, nowAll, (int)(&b - s_bots))\n"
          "\t\t                    : pb_XinVaoBang(nIdx, nNpcIdx, nSub, b, nowAll, (int)(&b - s_bots)))\n"
          "\t\t\treturn;\n\t}\n")
    # H10 PB_Breathe
    h.ins_after(r"^\t\tpb_TkNhip\(\);[^\n]*\n",
                "\t\tpb_BangNhip();      // [BOTBANG5 02/09] duyet don / chien dich tuyen / 18h dau thau, 1 giay/lan\n")
    return h.done("KPlayerBot.cpp")


# ============================================================================ KTongJX2.cpp
KTJ_SEND = r'''// [BOTBANG5 02/09] Bot lap bang: Core tu gui STONG_CREATE_COMMAND len relay y het GameServer.exe
// KSOServer.cpp:4327-4341 lam cho nguoi that (ke ca m_wLength cat sau m_szBuffer - m_btSex nam sau
// buffer nen relay doc ngoai goi, la quirk goc, giu nguyen de hai duong giong nhau). Duong tra ve
// dung nguyen: relay enumS2C_TONG_CREATE_SUCCESS -> GS.exe SSOI_TONG_CREATE -> KPlayerTong::Create.
// KHONG qua CheckCreateCondition (bot lead level 1); relay chi kiem ten <= 31 byte, ten bang / ten
// bang chu chua ai dung (KTongSet.cpp CTongSet::Create).
void KTongJX2_SendCreateC(int nPlayerIdx, int nCamp, const char* pszTongName)
{
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER || !pszTongName || !pszTongName[0])
		return;
	if (Player[nPlayerIdx].m_nIndex <= 0 || Player[nPlayerIdx].m_dwID == 0)
		return;
	const int nTL = (int)strlen(pszTongName);
	const int nPL = (int)strlen(Player[nPlayerIdx].m_PlayerName);
	if (nTL > defTONG_NAME_MAX_LENGTH || nPL <= 0 || nTL + nPL > 63)
		return;
	STONG_CREATE_COMMAND sCreate;
	memset(&sCreate, 0, sizeof(sCreate));
	sCreate.ProtocolFamily = pf_tong;
	sCreate.ProtocolID = enumC2S_TONG_CREATE;
	sCreate.m_btCamp = (BYTE)nCamp;
	sCreate.m_btSex = (BYTE)Npc[Player[nPlayerIdx].m_nIndex].m_nSex;
	sCreate.m_dwParam = (DWORD)nPlayerIdx;
	sCreate.m_dwPlayerNameID = g_FileName2Id(Player[nPlayerIdx].m_PlayerName);
	sCreate.m_btPlayerNameLength = (BYTE)nPL;
	sCreate.m_btTongNameLength = (BYTE)nTL;
	memcpy(sCreate.m_szBuffer, pszTongName, nTL);
	memcpy(&sCreate.m_szBuffer[nTL], Player[nPlayerIdx].m_PlayerName, nPL);
	sCreate.m_wLength = (WORD)(sizeof(STONG_CREATE_COMMAND) - sizeof(sCreate.m_szBuffer) + nTL + nPL);
	g_NewProtocolProcess.PushMsgInTong((const void*)&sCreate, (int)sCreate.m_wLength);
}

'''


def va_ktongjx2():
    p = os.path.join(SRC, "KTongJX2.cpp")
    h = Hunks(p)
    if MARK in h.s:
        print("KTongJX2.cpp: DA AP")
        return True
    h.ins_before(r"^// Bao cho bang chu / nguoi co quyen thu nhan \(1901\) dang online biet co don moi\.\n", KTJ_SEND)
    return h.done("KTongJX2.cpp")


# ============================================================================ KJx2CityWar.cpp / .h
CW_EXPORT = r'''// [BOTBANG5 02/09] cho bot bang chu (KPlayerBot.cpp pb_BangThau): dau thau theo chi so nguoi choi,
// cung 10 cua va cung thong bao nhu nguoi bam menu Su Gia; tra 0 OK / 1..10 ly do. Trang thai thanh
// de bot chi di khi dang mo bao danh (JX2CW_STATE_SIGNUP).
int KJx2CityWar_SignUpArenaC(int nPlayerIdx, int nCity, int nFee)
{
	sEnsureStore();
	if (nCity < 1 || nCity > 7)
		return 11;
	return sSignUpArena(nPlayerIdx, nCity, nFee);
}

int KJx2CityWar_GetState(int nCity)
{
	sEnsureStore();
	if (nCity < 1 || nCity > 7)
		return -1;
	return s_Cities[nCity].nState;
}

'''


def va_citywar():
    p = os.path.join(SRC, "KJx2CityWar.cpp")
    h = Hunks(p)
    if MARK in h.s:
        print("KJx2CityWar.cpp: DA AP")
    else:
        h.sub(r"^static void sSignUpArena\(Lua_State\* L, int c, int nFee\)\n\{\n\tint nPlayerIndex = GetPlayerIndex\(L\);\n\tif \(nPlayerIndex <= 0\)\n\t\treturn;\n",
              "static int sSignUpArena(int nPlayerIndex, int c, int nFee)	// [BOTBANG5 02/09] tra ma (0 OK) - ca Lua lan bot dung\n{\n\tif (nPlayerIndex <= 0)\n\t\treturn 1;\n")
        h.sub(r"^\t\treturn;(\t\t// \(1\) khong bang -> goc bo qua im lang)\n", "\t\treturn 1;\\1\n")
        h.sub(r"^(\t\tsMsgPlayer\(nPlayerIndex, CWS_WARN_TONGERROR\);\n\t\t)return;\n", "\\1return 2;\n")
        h.sub(r"^(\t\tsMsgPlayer\(nPlayerIndex, CWS_WARN_NOTTONGLEADER\);\n\t\t)return;\n", "\\1return 3;\n")
        h.sub(r"^(\t\tsMsgPlayer\(nPlayerIndex, CWS_WARN_SIGNUPTIMEOUT\);\n\t\t)return;\n", "\\1return 4;\n")
        h.sub(r"^(\t\t\t\t\tsMsgPlayer\(nPlayerIndex, CWS_WARN_ALREADYSIGNUP\);\n\t\t\t\t\t)return;\n", "\\1return 5;\n")
        h.sub(r"^(\t\t\tsprintf\(szMsg, CWS_WARN_BECHALLENGER, s_Cities\[i\]\.szAreaName\);\n\t\t\tsMsgPlayer\(nPlayerIndex, szMsg\);\n\t\t\t)return;\n", "\\1return 6;\n")
        h.sub(r"^(\t\t\tsprintf\(szMsg, CWS_WARN_BECITYOWNER, s_Cities\[i\]\.szAreaName\);\n\t\t\tsMsgPlayer\(nPlayerIndex, szMsg\);\n\t\t\t)return;\n", "\\1return 7;\n")
        h.sub(r"^(\t\tsprintf\(szMsg, CWS_WARN_TOOFEWCROWS, s_nSettings\[4\]\);\n\t\tsMsgPlayer\(nPlayerIndex, szMsg\);\n\t\t)return;\n", "\\1return 8;\n")
        h.sub(r"^(\t\tsprintf\(szMsg, CWS_WARN_LEVELLOW, s_nSettings\[1\]\);\n\t\tsMsgPlayer\(nPlayerIndex, szMsg\);\n\t\t)return;\n", "\\1return 9;\n")
        h.sub(r"^(\t\tsprintf\(szMsg, CWS_WARN_NOTENOUGHFEE, nFee\);\n\t\tsMsgPlayer\(nPlayerIndex, szMsg\);\n\t\t)return;\n", "\\1return 10;\n")
        h.sub(r"^(\t\tszTong, c, nFee, Player\[nPlayerIndex\]\.m_PlayerName\);\n\tsSaveMirror\(\);\n)\}\n", "\\1\treturn 0;\n}\n")
        h.sub(r"^\t\tsSignUpArena\(L, c, \(int\)Lua_ValueToNumber\(L, 2\)\);\n",
              "\t\tsSignUpArena(GetPlayerIndex(L), c, (int)Lua_ValueToNumber(L, 2));\n")
        h.ins_before(r"^//////////////////////////////////////////////////////////////////////\n// 14 ham Lua phia GS \(muc 4 dac ta\) - thay stub E4\n", CW_EXPORT)
        if not h.done("KJx2CityWar.cpp"):
            return False
    ph = os.path.join(SRC, "KJx2CityWar.h")
    hh = Hunks(ph)
    if MARK in hh.s:
        print("KJx2CityWar.h: DA AP")
        return True
    hh.ins_before(r"^#endif // _SERVER\n#endif // KJX2CITYWAR_H\n",
                  "// [BOTBANG5 02/09] cho bot bang chu (KPlayerBot.cpp): dau thau bang chi so nguoi choi (10 cua y het Lua),\n"
                  "// tra 0 OK / 1..10 ly do (pb_BbTenKq); trang thai thanh JX2CW_STATE_* (-1 = sai id)\n"
                  "int KJx2CityWar_SignUpArenaC(int nPlayerIdx, int nCity, int nFee);\n"
                  "int KJx2CityWar_GetState(int nCity);\n\n")
    return hh.done("KJx2CityWar.h")


# ============================================================================ ScriptFuns.cpp
def va_scriptfuns():
    p = os.path.join(SRC, "ScriptFuns.cpp")
    h = Hunks(p)
    if MARK in h.s:
        print("ScriptFuns.cpp: DA AP")
        return True
    h.ins_after(r"^extern int LuaPB_TongKimGoi\(Lua_State\* L\);\n",
                "// [BOTBANG5 02/09] 5 bang bot: tao bang - tuyen - dau thau cong thanh - nap quy (KPlayerBot.cpp)\n"
                "extern int LuaPB_BangTao(Lua_State* L);\n"
                "extern int LuaPB_BangTuyen(Lua_State* L);\n"
                "extern int LuaPB_BangCTC(Lua_State* L);\n"
                "extern int LuaPB_BangNap(Lua_State* L);\n"
                "extern int LuaPB_BangSo(Lua_State* L);\n"
                "extern int LuaPB_BangTen(Lua_State* L);\n"
                "extern int LuaPB_BangTT(Lua_State* L);\n")
    h.ins_after(r'^\t\{"PB_TongKimGoi",[^\n]*\n',
                '\t{"PB_BangTao",		LuaPB_BangTao},		// [BOTBANG5 02/09] (n) giao n bot cap cao nhat di lap n bang bot dau (botbang.txt)\n'
                '\t{"PB_BangTuyen",	LuaPB_BangTuyen},	// (stt, muc) chien dich tuyen bot vao bang stt toi muc nguoi (0 = SONGUOI trong tep)\n'
                '\t{"PB_BangCTC",		LuaPB_BangCTC},		// (1/0/-1) bat/tat/doc: 18h ngay bao danh bang chu bot di dau thau thanh muc tieu\n'
                '\t{"PB_BangNap",		LuaPB_BangNap},		// (stt, luong[, tongid]) admin nap quy: stt 1..n bang bot, 0 = bang co tongid\n'
                '\t{"PB_BangSo",		LuaPB_BangSo},		// ([1 = nap lai tep]) -> so bang bot\n'
                '\t{"PB_BangTen",		LuaPB_BangTen},		// (stt) -> ten bang\n'
                '\t{"PB_BangTT",		LuaPB_BangTT},		// () -> chuoi trang thai\n')
    return h.done("ScriptFuns.cpp")


# ============================================================================ simcity_admin.lua
LUA_KHOI = r'''-- ================= BANG HOI BOT (02/09) =================
-- 5 bang toan bot (settings\simcity\botbang.txt): tao bang - tuyen thanh vien - dung 18h ngay bao danh
-- dau thau cong thanh - admin nap quy. Ham C (KPlayerBot.cpp): PB_BangTao / PB_BangTuyen / PB_BangCTC /
-- PB_BangNap / PB_BangSo / PB_BangTen / PB_BangTT. Xem bot.log muc [BotBang5].
function PB_BangMenu()
	local nBat = PB_BangCTC(-1)
	local sBat = "Tắt"
	if nBat == 1 then
		sBat = "Bật"
	end
	SayEx({format("<color=yellow>Bang hội bot<color>\nĐấu thầu công thành theo giờ (18h ngày báo danh): <color=green>%s<color>", sBat),
	"Xem trạng thái các bang bot/PB_BangXem",
	"Tạo 5 bang (bot cấp cao nhất làm bang chủ)/PB_BangTaoGo",
	"Tuyển thành viên/PB_BangTuyenMenu",
	"Bật đấu thầu theo giờ/#PB_BangCTCBat(1)",
	"Tắt đấu thầu theo giờ/#PB_BangCTCBat(0)",
	"Nạp quỹ bang/PB_BangNapMenu",
	"Nạp lại botbang.txt/PB_BangNapLai",
	"Quay lại menu bot/PB_Menu",
	SC_END_SAY})
end

function PB_BangXem()
	local s = PB_BangTT()
	if (s == nil or s == "") then
		s = "(chưa nạp được botbang.txt)"
	end
	SayEx({"<color=yellow>Bang bot: stt.tên trạng thái người quỹ(triệu) thành bang chủ<color>\n"..s,
	"Quay lại/PB_BangMenu",
	SC_END_SAY})
end

function PB_BangTaoGo()
	local n = PB_BangTao(5)
	Msg2Player(format("Đã giao %d bot đi lập bang (bot về NPC môn phái xuất sư rồi lập bang; xem bot.log [BotBang5]).", n))
	PB_BangMenu()
end

function PB_BangTuyenMenu()
	local t = {"<color=yellow>Tuyển thành viên<color>\nBot đủ cấp, chưa bang, đang luyện công sẽ lần lượt về xin vào; bang chủ bot tự duyệt."}
	local n = PB_BangSo()
	for i = 1, n do
		tinsert(t, format("%s: tuyển theo botbang.txt/#PB_BangTuyenGo(%d)", PB_BangTen(i), i))
	end
	tinsert(t, "Quay lại/PB_BangMenu")
	tinsert(t, SC_END_SAY)
	SayEx(t)
end

function PB_BangTuyenGo(k)
	local n = PB_BangTuyen(k, 0)
	if (n > 0) then
		Msg2Player(format("Bang %s: bắt đầu tuyển tới %d thành viên (xem bot.log [BotBang5]).", PB_BangTen(k), n))
	elseif (n == -2) then
		Msg2Player("Bang này chưa có trên máy chủ (chưa tạo, hoặc relay chưa đồng bộ - chờ 30 giây rồi thử lại).")
	else
		Msg2Player("Số thứ tự bang không hợp lệ.")
	end
	PB_BangMenu()
end

function PB_BangCTCBat(n)
	PB_BangCTC(n)
	if (n == 1) then
		Msg2Player("Đã bật: 18h00-18h55 ngày báo danh của thành mục tiêu, bang chủ bot tới Sứ Giả Công Thành đấu thầu (cần đủ 37 người, quỹ > 1 triệu đủ 750 giây, quỹ đủ phí).")
	else
		Msg2Player("Đã tắt đấu thầu theo giờ.")
	end
	PB_BangMenu()
end

function PB_BangNapLai()
	local n = PB_BangSo(1)
	Msg2Player(format("Đã nạp lại botbang.txt: %d bang (trạng thái chiến dịch tuyển bị đặt lại).", n))
	PB_BangMenu()
end

function PB_BangNapMenu()
	local t = {"<color=yellow>Nạp quỹ bang<color>\nChọn bang nhận tiền (tiền vào ngân quỹ qua relay, vài giây sau mới hiện)."}
	local n = PB_BangSo()
	for i = 1, n do
		tinsert(t, format("%s/#PB_BangNapChon(%d)", PB_BangTen(i), i))
	end
	tinsert(t, "Bang của tôi/#PB_BangNapChon(0)")
	tinsert(t, "Quay lại/PB_BangMenu")
	tinsert(t, SC_END_SAY)
	SayEx(t)
end

function PB_BangNapChon(k)
	PB_BangNapK = k
	local sTen = "bang của tôi"
	if (k > 0) then
		sTen = PB_BangTen(k)
	end
	SayEx({format("<color=yellow>Nạp quỹ cho %s<color>\nChọn số tiền", sTen),
	"1 triệu lượng/#PB_BangNapGo(1000000)",
	"5 triệu lượng/#PB_BangNapGo(5000000)",
	"20 triệu lượng/#PB_BangNapGo(20000000)",
	"100 triệu lượng/#PB_BangNapGo(100000000)",
	"Quay lại/PB_BangNapMenu",
	SC_END_SAY})
end

function PB_BangNapGo(nLuong)
	local k = PB_BangNapK or 0
	local nTongID = 0
	if (k == 0) then
		local szTen, nID = GetTongName()
		if (nID == nil or nID == 0) then
			Msg2Player("Bạn chưa có bang hội.")
			PB_BangNapMenu()
			return
		end
		nTongID = nID
	end
	local r = PB_BangNap(k, nLuong, nTongID)
	if (r == 1) then
		Msg2Player(format("Đã gửi lệnh nạp %d lượng vào ngân quỹ (bot.log [BotBang5]). Mở lại Xem trạng thái sau vài giây.", nLuong))
	else
		Msg2Player("Không nạp được: bang chưa có trên máy chủ hoặc số tiền không hợp lệ.")
	end
	PB_BangMenu()
end

'''


def va_lua():
    p = os.path.join(RUN, "server", "script", "item", "simcity_admin.lua")
    h = Hunks(p)
    if "BANG HOI BOT (02/09)" in h.s:
        print("simcity_admin.lua: DA AP")
    else:
        h.ins_before("^\t" + re.escape(vn('"Quay lại SimCity/SC_Menu",')) + "\n",
                     "\t" + vn('"Bang hội bot/PB_BangMenu",') + "\n")
        h.ins_before(r"^-- ================= TONG KIM \(21/08\) =================\n", vn(LUA_KHOI))
        s0 = rd(p)
        if h.err:
            print("simcity_admin.lua: neo hong")
            return False
        if THU:
            print("simcity_admin.lua: %d hunk OK (thu)" % h.n)
        else:
            wr(p, h.s)
            print("simcity_admin.lua: AP %d hunk, high-byte %d -> %d" % (h.n, hb(s0), hb(h.s)))
    if not THU:
        shutil.copyfile(p, os.path.join(MIR_SCRIPT, "simcity_admin.lua"))
        print("MIRROR simcity_admin.lua")
    return True


# ============================================================================ du lieu
def va_dulieu():
    # botbang.txt: STT TEN CAMP THANH CAPMIN SONGUOI PHI (thanh: 5 TD-T5, 4 BK-T6, 7 LA-T7, 6 DC-CN, 2 TDo-T2)
    rows = [
        ("1", "Thanh Long", "1", "5", "60", "120", "1000000"),
        ("2", "Bạch Hổ",    "2", "4", "60", "120", "1000000"),
        ("3", "Chu Tước",   "1", "7", "60", "120", "1000000"),
        ("4", "Huyền Vũ",   "2", "6", "60", "120", "1000000"),
        ("5", "Kỳ Lân",     "3", "2", "60", "120", "1000000"),
    ]
    lines = ["STT\tTEN\tCAMP\tTHANH\tCAPMIN\tSONGUOI\tPHI"]
    for r in rows:
        lines.append("\t".join(r))
    txt = vn("\r\n".join(lines) + "\r\n")
    for r in rows:
        assert len(vn(r[1])) <= 12, r[1]
    p = os.path.join(RUN, "server", "settings", "simcity", "botbang.txt")
    if THU:
        print("botbang.txt: OK (thu), %d bang" % len(rows))
    else:
        if os.path.isfile(p):
            print("botbang.txt: DA CO, giu nguyen (xoa tep de sinh lai)")
        else:
            wr(p, txt)
            print("botbang.txt: TAO %s" % p)
        if not os.path.isdir(MIR_DATA):
            os.makedirs(MIR_DATA)
        shutil.copyfile(p, os.path.join(MIR_DATA, "botbang.txt"))
    # relay_config.ini tongcap 30 -> 160 (relay doc luc tao/nap bang -> can restart S3Relay)
    p2 = os.path.join(RUN, "multiserver", "relay_config.ini")
    s = rd(p2)
    if re.search(r"^tongcap\s*=\s*160\s*$", s, re.M):
        print("relay_config.ini: tongcap = 160 DA CO")
    elif len(re.findall(r"^tongcap\s*=\s*30\s*$", s, re.M)) == 1:
        if not THU:
            wr(p2, re.sub(r"^(tongcap\s*=\s*)30(\s*)$", r"\g<1>160\2", s, count=1, flags=re.M))
        print("relay_config.ini: tongcap 30 -> 160 %s" % ("(thu)" if THU else "AP"))
    else:
        print("relay_config.ini: KHONG thay 'tongcap = 30' -> tu sua tay")
    return True


def main():
    ok = True
    ok = va_kplayerbot() and ok
    ok = va_ktongjx2() and ok
    ok = va_citywar() and ok
    ok = va_scriptfuns() and ok
    ok = va_lua() and ok
    ok = va_dulieu() and ok
    print("KET QUA: %s" % ("OK" if ok else "CO LOI"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
