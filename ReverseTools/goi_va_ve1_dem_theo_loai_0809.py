# -*- coding: utf-8 -*-
"""goi_va_ve1_dem_theo_loai_0809.py - [VE 08/09 a] DO TRUOC cho viec #2: dem so don vi ve MOI KHUNG theo loai anh (theo tien to ten:
spr\\npcres = NPC, spr\\skill = chieu, spr\\ui* = UI, maps = nen/vat the ban do, '*' = anh tao san (nen vung/ban do nho), khac) ngay tai
TextureResMgr::GetImage khi dang ve. In vao dong [REP3-NAP]: '| ve/khung: npc N skill N ui N map N tao N khac N (khung K)'."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"
def load(name):
    p = ROOT + "\\" + name
    return p, io.open(p, "r", encoding="latin-1", newline="").read()
def save(p, s, h0):
    if sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao", p); sys.exit(1)
    if re.search(r"[^\r]\n", s): print("FAIL LF", p); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:90], c, n)); sys.exit(1)
    return s.replace(old, new)
def hi(s): return sum(1 for ch in s if ord(ch) >= 0x80)

p, s = load("BaseInclude.h"); h0 = hi(s)
if "g_uRep3VeLoai" not in s:
    s = rep(s, "void   Rep3NapCong(Rep3NapDo& d, double ms);\r\n",
        "void   Rep3NapCong(Rep3NapDo& d, double ms);\r\n"
        "extern unsigned g_uRep3VeLoai[6];\t// [VE 08/09 a] don vi ve theo loai: 0 npc, 1 skill, 2 ui, 3 maps, 4 anh tao, 5 khac\r\n"
        "extern unsigned g_uRep3VeKhung;\t\t// so khung ve trong ky\r\n"
        "void   Rep3VeDem(const char* pszImage);\r\n")
save(p, s, h0); print("OK BaseInclude.h")

p, s = load("KRepresentShell3.cpp"); h0 = hi(s)
if "g_uRep3VeLoai" not in s:
    s = rep(s, "double Rep3NapMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b)\r\n{\r\n",
        "unsigned g_uRep3VeLoai[6] = {0, 0, 0, 0, 0, 0}; unsigned g_uRep3VeKhung = 0;\t// [VE 08/09 a]\r\n"
        "void Rep3VeDem(const char* p)\r\n{\r\n"
        "\tif (!p) { g_uRep3VeLoai[5]++; return; }\r\n"
        "\tif (*p == '\\\\' || *p == '/') p++;\r\n"
        "\tif (*p == '*' || *p == '_') { g_uRep3VeLoai[4]++; return; }\r\n"
        "\tif (_strnicmp(p, \"spr\\\\npcres\", 10) == 0 || _strnicmp(p, \"spr/npcres\", 10) == 0) { g_uRep3VeLoai[0]++; return; }\r\n"
        "\tif (_strnicmp(p, \"spr\\\\skill\", 9) == 0 || _strnicmp(p, \"spr/skill\", 9) == 0) { g_uRep3VeLoai[1]++; return; }\r\n"
        "\tif (_strnicmp(p, \"spr\\\\ui\", 6) == 0 || _strnicmp(p, \"spr/ui\", 6) == 0) { g_uRep3VeLoai[2]++; return; }\r\n"
        "\tif (_strnicmp(p, \"maps\", 4) == 0) { g_uRep3VeLoai[3]++; return; }\r\n"
        "\tg_uRep3VeLoai[5]++;\r\n}\r\n"
        "double Rep3NapMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b)\r\n{\r\n")
    s = rep(s, "\t{\t// [NAP 08/09 a] tong ms nap trong khung nay -> max / dem khung nang\r\n",
        "\tg_uRep3VeKhung++;\t// [VE 08/09 a]\r\n\t{\t// [NAP 08/09 a] tong ms nap trong khung nay -> max / dem khung nang\r\n")
    s = rep(s, "khung co nap >5 ms: %u, >16 ms: %u, max %.1f ms/khung | nen: giao %u xong %u hong %u bo_ve %u\",\t// [NAP 08/09 a/b]\r\n",
        "khung co nap >5 ms: %u, >16 ms: %u, max %.1f ms/khung | nen: giao %u xong %u hong %u bo_ve %u | ve/khung: npc %.0f skill %.0f ui %.0f map %.0f tao %.0f khac %.0f (khung %u)\",\t// [NAP 08/09 a/b] [VE 08/09 a]\r\n")
    s = rep(s, "\t\t\t\tm_TextureResMgr.m_nNapNenGui, m_TextureResMgr.m_nNapNenXong, m_TextureResMgr.m_nNapNenHong, m_TextureResMgr.m_nNapNenBoVe);\r\n",
        "\t\t\t\tm_TextureResMgr.m_nNapNenGui, m_TextureResMgr.m_nNapNenXong, m_TextureResMgr.m_nNapNenHong, m_TextureResMgr.m_nNapNenBoVe,\r\n"
        "\t\t\t\tg_uRep3VeKhung ? (double)g_uRep3VeLoai[0] / g_uRep3VeKhung : 0.0, g_uRep3VeKhung ? (double)g_uRep3VeLoai[1] / g_uRep3VeKhung : 0.0, g_uRep3VeKhung ? (double)g_uRep3VeLoai[2] / g_uRep3VeKhung : 0.0,\r\n"
        "\t\t\t\tg_uRep3VeKhung ? (double)g_uRep3VeLoai[3] / g_uRep3VeKhung : 0.0, g_uRep3VeKhung ? (double)g_uRep3VeLoai[4] / g_uRep3VeKhung : 0.0, g_uRep3VeKhung ? (double)g_uRep3VeLoai[5] / g_uRep3VeKhung : 0.0, g_uRep3VeKhung);\r\n"
        "\t\t\tmemset(g_uRep3VeLoai, 0, sizeof(g_uRep3VeLoai)); g_uRep3VeKhung = 0;\r\n")
save(p, s, h0); print("OK KRepresentShell3.cpp")

p, s = load("TextureResMgr.cpp"); h0 = hi(s)
if "Rep3VeDem" not in s:
    s = rep(s, "    KAutoCriticalSection AutoLock(m_ImageProcessLock);\r\n\r\n\tTextureRes* pObject = NULL;\r\n\tif ((nImagePosition = FindImage(uImage, nImagePosition)) >= 0)\r\n",
        "    KAutoCriticalSection AutoLock(m_ImageProcessLock);\r\n"
        "\tif (m_bVeDangDien) Rep3VeDem(pszImage);\t// [VE 08/09 a] dem don vi ve theo loai\r\n\r\n"
        "\tTextureRes* pObject = NULL;\r\n\tif ((nImagePosition = FindImage(uImage, nImagePosition)) >= 0)\r\n")
save(p, s, h0); print("OK TextureResMgr.cpp")
print("XONG VE a")
