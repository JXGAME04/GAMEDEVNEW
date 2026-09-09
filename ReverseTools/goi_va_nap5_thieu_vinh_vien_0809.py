# -*- coding: utf-8 -*-
"""goi_va_nap5_thieu_vinh_vien_0809.py - [NAP 08/09 e] Tep thieu san (khong co trong pak/dia) van duoc luong nen thu nap lai moi 10 s
suot phien (do 11 phut: 334 luot thu vo ich). Sua: ResNode dem so lan nap hong (m_nLanHong); tu lan hong thu 3 tro di, nhip thu lai
gian ra 10 phut (khong bo han: neu tep duoc chep them vao thi 10 phut sau van thay). Chi Represent3, khong doi giao dien."""
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

p, s = load("TextureResMgr.h"); h0 = hi(s)
if "m_nLanHong" not in s:
    s = rep(s, "\tResNode() : m_bDangNap(false) {}\t// [NAP 08/09 b]\r\n\tbool\t\tm_bDangNap;",
        "\tResNode() : m_bDangNap(false), m_nLanHong(0) {}\t// [NAP 08/09 b/e]\r\n"
        "\tunsigned char\tm_nLanHong;\t\t\t// [NAP 08/09 e] so lan nap hong lien tiep: >= 3 -> thu lai moi 10 phut thay vi 10 giay\r\n"
        "\tbool\t\tm_bDangNap;")
save(p, s, h0); print("OK TextureResMgr.h")

p, s = load("TextureResMgr.cpp"); h0 = hi(s)
if "m_nLanHong" not in s:
    s = rep(s, "#define REP3_RELOAD_COOLDOWN\t10000\r\n",
        "#define REP3_RELOAD_COOLDOWN\t10000\r\n#define REP3_RELOAD_COOLDOWN_LAU\t600000\t// [NAP 08/09 e] sau 3 lan hong: 10 phut\r\n")
    s = rep(s, "\t\t\t\tif ((tmNow - m_TextureResList[nImagePosition].m_nRetryTime) >= REP3_RELOAD_COOLDOWN)\r\n",
        "\t\t\t\tif ((tmNow - m_TextureResList[nImagePosition].m_nRetryTime) >= (m_TextureResList[nImagePosition].m_nLanHong >= 3 ? (uint32)REP3_RELOAD_COOLDOWN_LAU : (uint32)REP3_RELOAD_COOLDOWN))\t// [NAP 08/09 e]\r\n")
    # thu lai dong bo that bai
    s = rep(s, "\t\t\t\t\t\tRep3Log(\"[REP3] LoadImage OK sau khi that bai: %s\", pszImage);\r\n\t\t\t\t\t}\r\n\t\t\t\t\telse\r\n\t\t\t\t\t\tRep3LogLoadFail(pszImage, nType);\r\n",
        "\t\t\t\t\t\tRep3Log(\"[REP3] LoadImage OK sau khi that bai: %s\", pszImage);\r\n\t\t\t\t\t\tm_TextureResList[nImagePosition].m_nLanHong = 0;\r\n\t\t\t\t\t}\r\n\t\t\t\t\telse\r\n\t\t\t\t\t{\r\n"
        "\t\t\t\t\t\tif (m_TextureResList[nImagePosition].m_nLanHong < 255) m_TextureResList[nImagePosition].m_nLanHong++;\t// [NAP 08/09 e]\r\n"
        "\t\t\t\t\t\tRep3LogLoadFail(pszImage, nType);\r\n\t\t\t\t\t}\r\n")
    # nap lan dau (dong bo) that bai -> node moi
    s = rep(s, "\t\tnode.m_nRetryTime = GetTickCount();\t// [REP3 03/09 LAG]\r\n\t\tnode.m_nType = nType;\r\n",
        "\t\tnode.m_nRetryTime = GetTickCount();\t// [REP3 03/09 LAG]\r\n\t\tnode.m_nLanHong = (!bNapNen && !pObject) ? 1 : 0;\t// [NAP 08/09 e]\r\n\t\tnode.m_nType = nType;\r\n")
    # luong nen that bai / thanh cong
    s = rep(s, "\t\t\tnode.m_pTextureRes = NULL;\r\n\t\t\tnode.m_nRetryTime = GetTickCount();\r\n\t\t\tm_nNapNenHong++;\r\n",
        "\t\t\tnode.m_pTextureRes = NULL;\r\n\t\t\tnode.m_nRetryTime = GetTickCount();\r\n\t\t\tif (node.m_nLanHong < 255) node.m_nLanHong++;\t// [NAP 08/09 e]\r\n\t\t\tm_nNapNenHong++;\r\n")
    s = rep(s, "\t\t\tm_nNapNenXong++;\r\n\t\t}\r\n\t\telse\r\n\t\t{\r\n\t\t\tnode.m_pTextureRes = NULL;\r\n",
        "\t\t\tm_nNapNenXong++;\r\n\t\t\tnode.m_nLanHong = 0;\t// [NAP 08/09 e]\r\n\t\t}\r\n\t\telse\r\n\t\t{\r\n\t\t\tnode.m_pTextureRes = NULL;\r\n")
save(p, s, h0); print("OK TextureResMgr.cpp")
print("XONG NAP e")
