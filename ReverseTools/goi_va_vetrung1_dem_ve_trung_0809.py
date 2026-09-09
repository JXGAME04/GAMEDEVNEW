# -*- coding: utf-8 -*-
"""goi_va_vetrung1_dem_ve_trung_0809.py - [VETRUNG 08/09] DO, KHONG DOAN.
Chu bao van con "chu ten / danh hieu cua nguoi dung XA bi am do" sau ban [MOCOI]. Gia thuyet dang xet la mot NPC
bi VE HAI LAN trong mot khung (chu ve chong len nhau nen dam va bao hoa ve kenh do). Ban nay DEM that:
 - CoreShell POSSHIFT (chay dung MOT lan moi khung ve) tang g_uPaintFrameSeq.
 - CoreDrawGameObj, nhanh CGOG_NPC + IPOT_RL_INFRONTOF_ALL (dung nhanh ve CHU ten/danh hieu), dem so lan moi chi so
   NPC duoc goi trong CUNG mot khung. Chi so nao duoc goi >= 2 lan = ve trung.
 - Chia GAN (<400 mps) / XA (>=400 mps) theo khoang cach toi nhan vat cua minh, in dong [VETRUNG] vao jx_paint.log
   moi 10 giay khi [Client] PaintLog=1: so khung, so luot ve moi khung, so NPC bi trung moi khung (gan/xa), so lan
   nhieu nhat mot NPC bi ve trong mot khung.
Neu 'trung' = 0 thi gia thuyet ve-hai-lan SAI han va phai tim huong khac; neu > 0 thi biet dung bao nhieu va o dau.
Khong doi mot dong hanh vi ve nao. Tep: Core/Src/CoreShell.cpp, CoreDrawGameObj.cpp (latin-1 CRLF)."""
import io, re, sys

SRC = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src"
NL = "\r\n"


def load(ten):
    p = SRC + "\\" + ten
    return p, io.open(p, "r", encoding="latin-1", newline="").read()


def save(p, s, h0):
    if sum(1 for ch in s if ord(ch) >= 0x80) != h0:
        print("FAIL byte cao", p); sys.exit(1)
    if re.search(r"[^\r]\n", s):
        print("FAIL LF", p); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n:
        print("FAIL neo %r: thay %d, can %d" % (old[:80], c, n)); sys.exit(1)
    return s.replace(old, new)


def hi(s):
    return sum(1 for ch in s if ord(ch) >= 0x80)


# 1. so thu tu khung ve
p, s = load("CoreShell.cpp"); h0 = hi(s)
if "g_uPaintFrameSeq" not in s:
    s = rep(s, "int\t\tg_nPaintAlpha = 0;", "unsigned g_uPaintFrameSeq = 0;\t// [VETRUNG 08/09] tang mot lan moi khung ve (POSSHIFT chay dung mot lan/khung)\r\nint\t\tg_nPaintAlpha = 0;")
    s = rep(s, "\t\tg_nPaintAlpha = nAlpha;\t// [MAU 08/09]" + NL,
        "\t\tg_nPaintAlpha = nAlpha;\t// [MAU 08/09]" + NL +
        "\t\tg_uPaintFrameSeq++;\t// [VETRUNG 08/09]" + NL)
save(p, s, h0); print("OK CoreShell.cpp")

# 2. dem ve trung trong nhanh ve chu
p, s = load("CoreDrawGameObj.cpp"); h0 = hi(s)
if "g_uVeTrungXa" not in s:
    khoi = NL.join([
    "// [VETRUNG 08/09] DEM ve trung: mot chi so NPC duoc goi bao nhieu lan trong CUNG mot khung o nhanh ve CHU.",
    "// Gia thuyet dang kiem: nut canh du -> NPC ve hai lan -> chu chong len nhau nen dam va ngA ve kenh do.",
    "extern int g_nCorePaintLog;",
    "extern unsigned g_uPaintFrameSeq;",
    "static unsigned s_uVTSeq = 0xFFFFFFFF;",
    "static unsigned char s_abyVTLan[MAX_NPC];",
    "static unsigned g_uVeTrungGan = 0, g_uVeTrungXa = 0, g_uVTKhung = 0, g_uVTLuot = 0, g_uVTMax = 0;",
    "static void VeTrungGhi(unsigned uId)",
    "{",
    "\tif (uId >= MAX_NPC) return;",
    "\tif (s_uVTSeq != g_uPaintFrameSeq)",
    "\t{",
    "\t\ts_uVTSeq = g_uPaintFrameSeq;",
    "\t\tmemset(s_abyVTLan, 0, sizeof(s_abyVTLan));",
    "\t\tg_uVTKhung++;",
    "\t}",
    "\tg_uVTLuot++;",
    "\tif (s_abyVTLan[uId] < 255) s_abyVTLan[uId]++;",
    "\tif (s_abyVTLan[uId] > g_uVTMax) g_uVTMax = s_abyVTLan[uId];",
    "\tif (s_abyVTLan[uId] == 2)",
    "\t{\t// lan thu hai trong cung khung = ve trung; xet khoang cach toi nhan vat cua minh",
    "\t\tint nX = 0, nY = 0, nMeX = 0, nMeY = 0;",
    "\t\tNpc[uId].GetDrawPos(&nX, &nY);",
    "\t\tNpc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetDrawPos(&nMeX, &nMeY);",
    "\t\tconst int ddx = nX - nMeX, ddy = (nY - nMeY) * 2;",
    "\t\tif ((double)ddx * ddx + (double)ddy * ddy < 400.0 * 400.0) g_uVeTrungGan++; else g_uVeTrungXa++;",
    "\t}",
    "}",
    "static void VeTrungInDong()",
    "{",
    "\tstatic DWORD s_dwLan = 0;",
    "\tconst DWORD dwNow = GetTickCount();",
    "\tif (s_dwLan == 0) { s_dwLan = dwNow; return; }",
    "\tif (dwNow - s_dwLan < 10000 || g_uVTKhung == 0) return;",
    "\ts_dwLan = dwNow;",
    "\tFILE* pLog = fopen(\"jx_paint.log\", \"a\");",
    "\tif (pLog)",
    "\t{",
    "\t\tfprintf(pLog, \"[VETRUNG] t=%u khung=%u | luot ve chu %.1f/khung | TRUNG gan %.2f xa %.2f moi khung | mot npc ve toi da %u lan/khung\\n\",",
    "\t\t\tdwNow, g_uVTKhung, (double)g_uVTLuot / g_uVTKhung,",
    "\t\t\t(double)g_uVeTrungGan / g_uVTKhung, (double)g_uVeTrungXa / g_uVTKhung, g_uVTMax);",
    "\t\tfclose(pLog);",
    "\t}",
    "\tg_uVeTrungGan = g_uVeTrungXa = g_uVTKhung = g_uVTLuot = g_uVTMax = 0;",
    "}",
    "void\tCoreDrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam)",
    "{",
    ""])
    s = rep(s, "void\tCoreDrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam)" + NL + "{" + NL, khoi)
    neo = ("\t\t\tif ((nParam & IPOT_RL_INFRONTOF_ALL) == IPOT_RL_INFRONTOF_ALL)" + NL +
           "\t\t\t{" + NL +
           "\t\t\t\tNpc[uId].PaintBlood(nHeight / 2);" + NL)
    moi = ("\t\t\tif ((nParam & IPOT_RL_INFRONTOF_ALL) == IPOT_RL_INFRONTOF_ALL)" + NL +
           "\t\t\t{" + NL +
           "\t\t\t\tif (g_nCorePaintLog > 0) { VeTrungGhi(uId); VeTrungInDong(); }\t// [VETRUNG 08/09]" + NL +
           "\t\t\t\tNpc[uId].PaintBlood(nHeight / 2);" + NL)
    s = rep(s, neo, moi)
save(p, s, h0); print("OK CoreDrawGameObj.cpp")
print("XONG VETRUNG")
