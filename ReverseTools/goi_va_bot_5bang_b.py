# -*- coding: utf-8 -*-
r"""goi_va_bot_5bang_b.py - [BOTBANG5b 02/09] DOT b: sua theo PHAN BIEN (tac tu doc lap) tren ban goi_va_bot_5bang.py.

CHAN 1+2: pb_DongBoBang (10 s/con) Clear() ho so bang khi ban sao relay chua co bang/thanh vien - ma relay
          chi phat bang MOI o tick 30 giay (JX2_TimerTick) va thanh vien MOI o tick TimeLong 750 giay
          (sJX2_BroadcastTong) -> bot vua lap bang / vua duoc duyet bi xoa oan, roi bi tuyen lai vo han.
          -> AN HAN 15 phut (uBangMatTu, ms) truoc khi Clear; dem thanh vien qua m_cTong cua bot
          (pb_BbDemBot) thay vi chi mapMember cua ban sao.
CHAN 3:   Tong Kim 17:50 hang ngay keo bang chu bot (cap cao nhat) di -> mat cua so dau thau 18h00-18h55.
          -> pb_TkDuTuCach loai bot dang lam viec bang (nBangPha) va bang chu bang bot khi dang bat CTC.
CAO 1:    sau restart s_bbLoaded = 0 / s_nPbBangCTC = 0 -> khong duyet don, khong dau thau.
          -> pb_BangNhip tu nap; co CTC luu \settings\simcity\botbang_ctc.txt.
CAO 2:    bang NGUOI THAT trung ten trong botbang.txt bi chiem dung (tuyen bot vao, nap tien vao).
          -> nTao 4 = "bang nguoi that": doi bang chu la bot (pb_BbLeaderIdx) moi coi la bang bot.
VUA:      phi thau < SignUpFee (max voi KJx2CityWar_GetSetting(0)); duyet don khong back-off (nghi 30 s khi
          ma != 0); PB_BangTT co the > 512 byte (cat 360); nap lai botbang.txt khi bot dang di (tu choi).
THAP:     uLogTick tron ms/khung (dua het ve ms); cho relay moi khung chay lai PB_WalkTo (nghi 1 giay);
          ten bang chua '/' '|' vo menu (tu choi luc nap).
Chuoi tai ap: goi_va_bot_5bang.py -> goi_va_bot_5bang_b.py. Idempotent (dau [BOTBANG5b). --thu = chi kiem neo.
"""
import io
import os
import re
import shutil
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import vn

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"
RUN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
MIR_SCRIPT = r"D:\GAMEDEVNEW\serverscript_jx2\jx1_edits"
MARK = "[BOTBANG5b"
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
        # '\\n' (regex cho backslash-n LITERAL trong chuoi C/Lua) phai giu nguyen; chi '\n' (xuong dong) doi CRLF
        PH = "\x00LIT\x00"
        p = pat.replace("\\\\n", PH)
        p = p.replace("[^\n]", "[^\r\n]").replace("[^\\n]", "[^\\r\\n]")
        p = p.replace("\n", "\r?\n").replace("\\n", "\\r?\\n")
        p = p.replace(PH, "\\\\n")
        found = len(re.findall(p, self.s, flags))
        if found != count:
            print("  NEO KHONG KHOP (%d thay vi %d): %s" % (found, count, pat[:72].replace("\n", "|")))
            self.err += 1
            return

        def _thay(m):
            # KHONG dung m.expand(): no doi '\n' '\t' trong chuoi thay the thanh ky tu that -> pha
            # literal \n trong chuoi C/Lua (da dinh 02/09). Chi mo rong \1..\9.
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


HELPER_TRUOC_NAP = r'''// [BOTBANG5b 02/09] co "dau thau theo gio" giu qua restart: \settings\simcity\botbang_ctc.txt ("1"/"0")
static void pb_BbDuongCoCTC(char* szOut, int nMax)
{
	char szRoot[MAX_PATH];
	szRoot[0] = 0;
	g_GetRootPath(szRoot);
	int nLen = (int)strlen(szRoot);
	while (nLen > 0 && (szRoot[nLen - 1] == '\\' || szRoot[nLen - 1] == '/'))
		szRoot[--nLen] = 0;
	_snprintf(szOut, nMax - 1, "%s\\settings\\simcity\\botbang_ctc.txt", szRoot);
	szOut[nMax - 1] = 0;
}

static int pb_BbDocCoCTC()
{
	char szPath[MAX_PATH * 2];
	pb_BbDuongCoCTC(szPath, sizeof(szPath));
	FILE* f = fopen(szPath, "r");
	if (!f)
		return 0;
	int n = 0;
	if (fscanf(f, "%d", &n) != 1)
		n = 0;
	fclose(f);
	return n ? 1 : 0;
}

static void pb_BbGhiCoCTC(int nBat)
{
	char szPath[MAX_PATH * 2];
	pb_BbDuongCoCTC(szPath, sizeof(szPath));
	FILE* f = fopen(szPath, "w");
	if (!f)
	{
		pb_Log("[BotBang5] khong ghi duoc %s\n", szPath);
		return;
	}
	fprintf(f, "%d\n", nBat ? 1 : 0);
	fclose(f);
}

'''

HELPER_SAU_XOAVIEC = r'''// [BOTBANG5b 02/09] (phan bien CHAN 2) thanh vien MOI chi vao ban sao relay sau tick TimeLong 750 giay,
// nhung SSOI_TONG_ADD dat m_cTong cua bot NGAY khi duyet -> dem theo m_cTong de chien dich tuyen khong
// giao thua bot va "TUYEN XONG" dung luc.
static int pb_BbDemBot(DWORD dwTongID)
{
	int n = 0;
	for (int i = 0; i < s_botCount; i++)
	{
		const int p = s_bots[i].nPlayerIdx;
		if (p <= 0 || p >= MAX_PLAYER || Player[p].m_dwID != s_bots[i].dwID)
			continue;
		if (Player[p].m_cTong.m_nFlag && Player[p].m_cTong.GetTongNameID() == dwTongID)
			n++;
	}
	return n;
}

static int pb_BbSoNguoi(const PB_BotBang& bb)
{
	KTongJX2Tong* pT = g_TongJX2.FindTong(bb.dwTongID);
	const int a = pT ? (int)pT->mapMember.size() : 0;
	const int c = pb_BbDemBot(bb.dwTongID);
	return (a > c) ? a : c;
}

// bot nay la BANG CHU cua mot bang bot -> khong keo di Tong Kim khi dang bat dau thau theo gio
// (phan bien CHAN 3: Tong Kim 17:50 hang ngay nuot cua so 18h00-18h55)
static int pb_BbLaBangChu(const PB_Bot& b)
{
	const int p = b.nPlayerIdx;
	if (p <= 0 || p >= MAX_PLAYER || Player[p].m_dwID != b.dwID || !Player[p].m_cTong.m_nFlag)
		return 0;
	const DWORD dwId = Player[p].m_cTong.GetTongNameID();
	for (int k = 0; k < s_bbCount; k++)
		if (s_bb[k].dwTongID == dwId)
			return (Player[p].m_cTong.m_nFigure == enumTONG_FIGURE_MASTER || pb_BbLeaderIdx(s_bb[k]) == p) ? 1 : 0;
	return 0;
}

'''


def va_kplayerbot():
    p = os.path.join(SRC, "KPlayerBot.cpp")
    h = Hunks(p)
    if MARK in h.s:
        print("KPlayerBot.cpp: DA AP")
        return True
    # B1 truong an han trong PB_Bot
    h.ins_after(r"^\tint\s+nBangBotIdx;\s+// chi so bang bot trong s_bb \(-1 = khong\)\n",
                "\tunsigned int uBangMatTu;                // [BOTBANG5b] GetTickCount ms tu luc ban sao relay KHONG thay bang/thanh vien (an han 15 phut)\n")
    # B2 reset luc sinh bot (chi dong bat dau bang nBangLoai o khoi PB_OnRoleData)
    h.ins_after(r"^\t\tb\.nBangLoai = 0;  b\.nBangBotIdx = -1;\n", "\t\tb.uBangMatTu = 0;\n")
    # B3 pb_DongBoBang: an han 15 phut truoc khi Clear
    h.sub(r"^\tif \(!pT \|\| !pMe\)\n\t\{\n\t\t// bang giai tan / bot bi duoi luc offline\. Xoa ho so bang; CO Y khong dong vao camp\n"
          r"\t\t// \(bot con mang camp mon phai hoac camp tu do sau xuat su - doi o day la doi gameplay\)\.\n"
          r"\t\tpb_Log\(\"\[BotBang\] %s khong con la thanh vien bang %u -> xoa ho so bang\\n\",\n"
          r"\t\t       Player\[nIdx\]\.m_PlayerName, \(unsigned int\)dwId\);\n"
          r"\t\tt\.Clear\(\);\n\t\treturn;\n\t\}\n",
          "\tif (!pT || !pMe)\n\t{\n"
          "\t\t// bang giai tan / bot bi duoi luc offline. Xoa ho so bang; CO Y khong dong vao camp\n"
          "\t\t// (bot con mang camp mon phai hoac camp tu do sau xuat su - doi o day la doi gameplay).\n"
          "\t\t// [BOTBANG5b 02/09] AN HAN 15 PHUT (phan bien CHAN 1-2): ban sao relay chi nhan bang MOI o tick\n"
          "\t\t// 30 giay (JX2_TimerTick) va thanh vien MOI o tick TimeLong 750 giay (sJX2_BroadcastTong) - bot\n"
          "\t\t// vua lap bang / vua duoc duyet ma xoa ngay la mat bang oan roi bi tuyen lai vo han.\n"
          "\t\tconst unsigned int uMs = (unsigned int)GetTickCount();\n"
          "\t\tif (b.uBangMatTu == 0)\n"
          "\t\t{\n"
          "\t\t\tb.uBangMatTu = uMs ? uMs : 1;\n"
          "\t\t\treturn;\n"
          "\t\t}\n"
          "\t\tif (uMs - b.uBangMatTu < 900000u)\n"
          "\t\t\treturn;\n"
          "\t\tpb_Log(\"[BotBang] %s khong con la thanh vien bang %u (ban sao relay khong thay qua 15 phut) -> xoa ho so bang\\n\",\n"
          "\t\t       Player[nIdx].m_PlayerName, (unsigned int)dwId);\n"
          "\t\tt.Clear();\n"
          "\t\tb.uBangMatTu = 0;\n"
          "\t\treturn;\n\t}\n"
          "\tb.uBangMatTu = 0;\n")
    # B4 truong back-off duyet don
    h.ins_after(r"^\tunsigned int uLogTick;        // gion log\n",
                "\tunsigned int uDuyetNghi;      // [BOTBANG5b] ms: duyet don tra ma khac 0 -> nghi 30 giay (khoi spam)\n")
    # B5 helper: co CTC (truoc pb_BbNap) + dem bot / bang chu (sau pb_BbXoaViec)
    h.ins_before(r"^static void pb_BbNap\(int bLai\)\n\{\n", HELPER_TRUOC_NAP)
    h.ins_before(r"^// relay da tra ve KPlayerTong::Create cho bot bang chu \(m_cTong\.m_nFlag = 1\)\n", HELPER_SAU_XOAVIEC)
    # B6 pb_BbNap: ten cam '/' '|', nTao de pb_BangNhip phan loai, doc co CTC
    h.sub(r"^\t\tbb\.dwTongID = g_FileName2Id\(bb\.szTen\);\n\t\tbb\.nTao = g_TongJX2\.FindTong\(bb\.dwTongID\) \? 2 : 0;\n",
          "\t\tif (strchr(bb.szTen, '/') || strchr(bb.szTen, '|'))\n"
          "\t\t{\n"
          "\t\t\tpb_Log(\"[BotBang5] %s dong %d: ten bang chua '/' hoac '|' (vo nhan menu Say) -> bo\\n\", PB_BB_FILE, row);\n"
          "\t\t\tcontinue;\n"
          "\t\t}\n"
          "\t\tbb.dwTongID = g_FileName2Id(bb.szTen);\n"
          "\t\tbb.nTao = 0;                       // [BOTBANG5b] pb_BangNhip phan loai: 2 = bang bot (bang chu la bot) / 4 = bang NGUOI THAT trung ten\n")
    h.ins_after(r"^\tpb_Log\(\"\[BotBang5\] nap %d bang bot tu %s\\n\", s_bbCount, PB_BB_FILE\);\n",
                "\ts_nPbBangCTC = pb_BbDocCoCTC();   // [BOTBANG5b] co dau thau theo gio giu qua restart\n"
                "\tif (s_nPbBangCTC)\n"
                "\t\tpb_Log(\"[BotBang5] dau thau cong thanh theo gio: BAT (theo botbang_ctc.txt)\\n\");\n")
    # B7 cho relay: nghi 1 giay giua hai lan kiem (khoi PB_WalkTo moi khung)
    h.sub(r"^\t\treturn 1;                          // dang cho relay\n",
          "\t\tb.nBangNghiToi = now + (unsigned int)GAME_FPS;   // [BOTBANG5b] 1 giay/lan kiem, khoi PB_WalkTo + do_stand moi khung\n"
          "\t\treturn 1;                          // dang cho relay\n")
    # B8 pb_BangThau: uLogTick theo ms (thong nhat voi pb_BangNhip)
    h.sub(r"^\t\tif \(now - bb\.uLogTick > \(unsigned int\)\(GAME_FPS \* 30\)\)\n\t\t\{\n\t\t\tbb\.uLogTick = now;\n",
          "\t\tif ((unsigned int)GetTickCount() - bb.uLogTick > 30000)   // [BOTBANG5b] ms, cung don vi voi pb_BangNhip\n\t\t{\n\t\t\tbb.uLogTick = (unsigned int)GetTickCount();\n")
    # B9 phi thau >= SignUpFee
    h.ins_after(r"^extern int     KJx2CityWar_GetState\(int nCity\);\n",
                "extern int     KJx2CityWar_GetSetting(int nIdx);   // [BOTBANG5b] 0 = SignUpFee (citywar.ini)\n")
    h.sub(r"^\tconst int nKq = KJx2CityWar_SignUpArenaC\(nIdx, bb\.nThanh, bb\.nPhi\);\n",
          "\tint nPhi = bb.nPhi;                                      // [BOTBANG5b] phi >= SignUpFee citywar.ini (phan bien VUA 1)\n"
          "\tif (nPhi < KJx2CityWar_GetSetting(0))\n"
          "\t\tnPhi = KJx2CityWar_GetSetting(0);\n"
          "\tconst int nKq = KJx2CityWar_SignUpArenaC(nIdx, bb.nThanh, nPhi);\n")
    h.sub(r"Player\[nIdx\]\.m_PlayerName, bb\.nThanh, bb\.szTen, bb\.nPhi, nKq, pb_BbTenKq\(nKq\), KTongJX2_GetMoneyC\(bb\.dwTongID\)\);",
          "Player[nIdx].m_PlayerName, bb.nThanh, bb.szTen, nPhi, nKq, pb_BbTenKq(nKq), KTongJX2_GetMoneyC(bb.dwTongID));")
    # B10 pb_BangNhip: tu nap sau restart
    h.sub(r"^\tif \(!s_bbLoaded \|\| s_botCount <= 0\)\n\t\treturn;\n",
          "\tif (!s_bbLoaded)\n\t\tpb_BbNap(0);                       // [BOTBANG5b] tu nap sau restart - khong cho admin mo menu (phan bien CAO 1)\n\tif (s_botCount <= 0)\n\t\treturn;\n")
    # B11 phan loai bang bot / bang nguoi that trung ten
    h.sub(r"^\t\tif \(!pT\)\n\t\t\tcontinue;\n\t\tif \(bb\.nTao != 2\)\n\t\t\{\n\t\t\tbb\.nTao = 2;\n"
          r"\t\t\tpb_Log\(\"\[BotBang5\] bang '%s' \(id %u\) da co tren GS: %d thanh vien\\n\", bb\.szTen, \(unsigned int\)bb\.dwTongID, \(int\)pT->mapMember\.size\(\)\);\n\t\t\}\n",
          "\t\tif (!pT)\n\t\t\tcontinue;\n"
          "\t\t// [BOTBANG5b] (phan bien CAO 2) co tren GS nhung bang chu KHONG phai bot dang song = bang NGUOI THAT\n"
          "\t\t// trung ten (hoac bang chu bot chua nap sau restart) -> khong tuyen / khong nap quy / khong dau thau\n"
          "\t\tif (pb_BbLeaderIdx(bb) <= 0)\n"
          "\t\t{\n"
          "\t\t\tif (bb.nTao != 4)\n"
          "\t\t\t{\n"
          "\t\t\t\tbb.nTao = 4;\n"
          "\t\t\t\tpb_Log(\"[BotBang5] bang '%s' (id %u) co tren GS nhung bang chu khong phai bot dang song -> coi la bang NGUOI THAT trung ten, KHONG dung (doi ten trong botbang.txt / cho bot bang chu nap)\\n\", bb.szTen, (unsigned int)bb.dwTongID);\n"
          "\t\t\t}\n"
          "\t\t\tcontinue;\n"
          "\t\t}\n"
          "\t\tif (bb.nTao != 2)\n\t\t{\n\t\t\tbb.nTao = 2;\n"
          "\t\t\tpb_Log(\"[BotBang5] bang '%s' (id %u) da co tren GS, bang chu la bot: %d thanh vien (ban sao relay)\\n\", bb.szTen, (unsigned int)bb.dwTongID, (int)pT->mapMember.size());\n\t\t}\n")
    # B12 duyet don: back-off 30 giay khi ma khac 0
    h.sub(r"^\t\tif \(pT->btApplyCount > 0\)\n",
          "\t\tif (pT->btApplyCount > 0 && (!bb.uDuyetNghi || (int)(dwNow - bb.uDuyetNghi) >= 0))\n")
    h.ins_after(r"^\t\t\t\t       bBot \? \"\" : \" \(khong phai bot\)\", nKq, \(int\)pT->btApplyCount, \(int\)pT->mapMember\.size\(\)\);\n",
                "\t\t\t\tbb.uDuyetNghi = (nKq == 0) ? 0 : (dwNow + 30000);   // [BOTBANG5b] ma 2 (ban sao chua co bang chu) / 3... -> nghi 30 giay (phan bien VUA 2)\n")
    # B13 dem thanh vien qua m_cTong cua bot
    h.sub(r"^\t\t\tconst int nCo = \(int\)pT->mapMember\.size\(\);\n",
          "\t\t\tint nCo = (int)pT->mapMember.size();\n"
          "\t\t\t{\n"
          "\t\t\t\tconst int nBot = pb_BbDemBot(bb.dwTongID);   // [BOTBANG5b] thanh vien MOI chi vao ban sao sau tick 750 giay\n"
          "\t\t\t\tif (nBot > nCo)\n"
          "\t\t\t\t\tnCo = nBot;\n"
          "\t\t\t}\n")
    # B14 PB_BangTao: bang trung ten cua nguoi that
    h.sub(r"^\t\tif \(g_TongJX2\.FindTong\(bb\.dwTongID\)\)\n\t\t\{\n\t\t\tbb\.nTao = 2;\n\t\t\tcontinue;\n\t\t\}\n",
          "\t\tif (g_TongJX2.FindTong(bb.dwTongID))\n\t\t{\n"
          "\t\t\t// [BOTBANG5b] da co: bang bot (bang chu la bot) -> bo qua; bang NGUOI THAT trung ten -> khong dung ten nay\n"
          "\t\t\tif (pb_BbLeaderIdx(bb) > 0)\n"
          "\t\t\t\tbb.nTao = 2;\n"
          "\t\t\telse\n"
          "\t\t\t{\n"
          "\t\t\t\tbb.nTao = 4;\n"
          "\t\t\t\tpb_Log(\"[BotBang5] tao bang '%s': ten da co tren GS ma bang chu khong phai bot -> bang NGUOI THAT trung ten, doi ten trong botbang.txt\\n\", bb.szTen);\n"
          "\t\t\t}\n"
          "\t\t\tcontinue;\n\t\t}\n")
    # B15 PB_BangTuyen: doi nTao == 2
    h.sub(r"^\tif \(!g_TongJX2\.FindTong\(bb\.dwTongID\)\)\n\t\{\n\t\tpb_Log\(\"\[BotBang5\] tuyen cho '%s': bang chua co tren GS \(chua tao / relay chua dong bo\)\\n\", bb\.szTen\);\n\t\treturn -2;\n\t\}\n",
          "\tif (bb.nTao != 2 || !g_TongJX2.FindTong(bb.dwTongID))\n\t{\n"
          "\t\tpb_Log(\"[BotBang5] tuyen cho '%s': chua phai bang bot san sang (nTao %d) - chua tao / relay chua dong bo (<= 30 giay) / bang nguoi that trung ten\\n\", bb.szTen, bb.nTao);\n"
          "\t\treturn -2;\n\t}\n")
    # B16 PB_BangNap: chi nap vao bang bot da xac nhan
    h.sub(r"^\tif \(nStt >= 1 && nStt <= s_bbCount\)\n\t\tdwID = s_bb\[nStt - 1\]\.dwTongID;\n",
          "\tif (nStt >= 1 && nStt <= s_bbCount)\n\t{\n"
          "\t\tif (s_bb[nStt - 1].nTao != 2)\n"
          "\t\t\treturn 0;                      // [BOTBANG5b] khong nap vao bang chua tao / bang nguoi that trung ten\n"
          "\t\tdwID = s_bb[nStt - 1].dwTongID;\n\t}\n")
    # B17 PB_BangCTC: luu co
    h.ins_after(r"^\t\ts_nPbBangCTC = nBat \? 1 : 0;\n",
                "\t\tpb_BbGhiCoCTC(s_nPbBangCTC);   // [BOTBANG5b] giu qua restart\n")
    # B18 nap lai botbang.txt: tu choi khi bot dang di
    h.sub(r"^\tpb_BbNap\(pb_LuaSoNguyen\(L, 1, 0\)\);\n\tLua_PushNumber\(L, s_bbCount\);\n",
          "\tconst int bLai = pb_LuaSoNguyen(L, 1, 0);\n"
          "\tif (bLai)\n"
          "\t{\n"
          "\t\t// [BOTBANG5b] memset s_bb khi bot dang mang nBangBotIdx = lech chi so -> tu choi\n"
          "\t\tfor (int i = 0; i < s_botCount; i++)\n"
          "\t\t\tif (s_bots[i].nBangPha && s_bots[i].nBangBotIdx >= 0)\n"
          "\t\t\t{\n"
          "\t\t\t\tpb_Log(\"[BotBang5] KHONG nap lai botbang.txt: con bot dang di lam viec bang (%s) - cho xong\\n\",\n"
          "\t\t\t\t       Player[s_bots[i].nPlayerIdx].m_PlayerName);\n"
          "\t\t\t\tLua_PushNumber(L, -1);\n"
          "\t\t\t\treturn 1;\n"
          "\t\t\t}\n"
          "\t}\n"
          "\tpb_BbNap(bLai);\n\tLua_PushNumber(L, s_bbCount);\n")
    # B19 PB_BangTT: ngan hon 512 byte + nTao 4
    h.sub(r'^\tstatic const char\* s_szTao\[4\] = \{ "chua tao", "dang tao", "co", "LOI tao" \};\n',
          '\tstatic const char* s_szTao[5] = { "chua tao", "dang tao", "co", "LOI tao", "BANG NGUOI!" };   // [BOTBANG5b]\n')
    h.sub(r"^\tfor \(int k = 0; k < s_bbCount && nLen < 900; k\+\+\)\n",
          "\tfor (int k = 0; k < s_bbCount && nLen < 360; k++)   // [BOTBANG5b] Say/SayEx tran 512 byte ke ca nhan\n")
    h.sub(r'^\t\tnLen \+= sprintf\(sz \+ nLen, "%d\.%s %s %dng %I64dtr T%d %s%s\\n",\n',
          '\t\tnLen += sprintf(sz + nLen, "%d.%s %s %dng %I64dtr T%d %.10s%s\\n",\n')
    h.sub(r"^\t\t                bb\.nStt, bb\.szTen, s_szTao\[\(bb\.nTao >= 0 && bb\.nTao <= 3\) \? bb\.nTao : 0\],\n",
          "\t\t                bb.nStt, bb.szTen, s_szTao[(bb.nTao >= 0 && bb.nTao <= 4) ? bb.nTao : 0],\n")
    h.sub(r"^\t\t                pT \? \(int\)pT->mapMember\.size\(\) : 0,\n",
          "\t\t                pb_BbSoNguoi(bb),\n")
    # B20 pb_TkDuTuCach: khong keo bot dang lam viec bang / bang chu bang bot
    h.ins_after(r"^\tif \(b\.nTk\)                                        return 0;  // dang trong tran roi\n",
                "\tif (b.nBangPha)                                   return 0;  // [BOTBANG5b] dang lap bang / xin bang / di dau thau\n"
                "\tif (s_nPbBangCTC && pb_BbLaBangChu(b))            return 0;  // [BOTBANG5b] bang chu bang bot: TK 17:50 nuot cua so dau thau 18h (phan bien CHAN 3)\n")
    return h.done("KPlayerBot.cpp")


def va_lua():
    p = os.path.join(RUN, "server", "script", "item", "simcity_admin.lua")
    h = Hunks(p)
    if "BOTBANG5b" in h.s:
        print("simcity_admin.lua: DA AP")
    else:
        h.sub("^" + re.escape(vn('\t\ttinsert(t, format("%s: tuyển theo botbang.txt/#PB_BangTuyenGo(%d)", PB_BangTen(i), i))')) + "\n",
              vn('\t\ttinsert(t, format("%s/#PB_BangTuyenGo(%d)", PB_BangTen(i), i))') + "\n")
        h.sub("^" + re.escape(vn('\tlocal t = {"<color=yellow>Tuyển thành viên<color>\\nBot đủ cấp, chưa bang, đang luyện công sẽ lần lượt về xin vào; bang chủ bot tự duyệt."}')) + "\n",
              vn('\tlocal t = {"<color=yellow>Tuyển thành viên<color>\\nChọn bang (mục tiêu theo botbang.txt; bot đủ cấp, chưa bang tự về xin)"}') + "\n")
        h.sub("^" + re.escape(vn('\tlocal t = {"<color=yellow>Nạp quỹ bang<color>\\nChọn bang nhận tiền (tiền vào ngân quỹ qua relay, vài giây sau mới hiện)."}')) + "\n",
              vn('\tlocal t = {"<color=yellow>Nạp quỹ bang<color>\\nChọn bang nhận tiền"}') + "\n")
        h.sub("^" + re.escape(vn('\tSayEx({"<color=yellow>Bang bot: stt.tên trạng thái người quỹ(triệu) thành bang chủ<color>\\n"..s,')) + "\n",
              vn('\tSayEx({"<color=yellow>Bang bot (stt.tên tt người quỹ thành chủ)<color>\\n"..s,') + "\n")
        h.sub("^" + re.escape(vn('\tlocal n = PB_BangSo(1)\n\tMsg2Player(format("Đã nạp lại botbang.txt: %d bang (trạng thái chiến dịch tuyển bị đặt lại).", n))')) + "\n",
              vn('\tlocal n = PB_BangSo(1)\n\t-- [BOTBANG5b] -1 = con bot dang di lam viec bang\n\tif (n < 0) then\n\t\tMsg2Player("Chưa nạp lại được: còn bot đang đi lập bang / xin bang / đấu thầu. Thử lại sau.")\n\telse\n\t\tMsg2Player(format("Đã nạp lại botbang.txt: %d bang (chiến dịch tuyển bị đặt lại).", n))\n\tend') + "\n")
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


def main():
    ok = va_kplayerbot()
    ok = va_lua() and ok
    print("KET QUA: %s" % ("OK" if ok else "CO LOI"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
