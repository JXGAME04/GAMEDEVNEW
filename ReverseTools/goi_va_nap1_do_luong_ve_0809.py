# -*- coding: utf-8 -*-
"""goi_va_nap1_do_luong_ve_0809.py - [NAP 08/09 a] DO TRUOC KHI SUA (viec #1 KEHOACH_D3D11_TIEP_0809.md): tach thoi gian NAP tai nguyen
tren luong ve thanh 5 phan (tep spr = pak+giai nen, jpeg nen, rut khung tu pak, giai ma RLE, tao/day GPU) + so khung ve co nap > 5 ms
/ > 16 ms va max ms nap trong mot khung. In moi Rep3StatSec giay: dong [REP3-NAP]. Kem #0: mac dinh Rep3Api=11 (tu lui D3D9 khi may
khong du: CD3D11Shim::Init da do IDXGIFactory2 + feature level).
Tep: BaseInclude.h, KRepresentShell3.cpp, TextureRes.cpp, TextureResMgr.cpp (latin-1 CRLF)."""
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
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:80], c, n)); sys.exit(1)
    return s.replace(old, new)
def hi(s): return sum(1 for ch in s if ord(ch) >= 0x80)

# ---------------- BaseInclude.h: cau truc + extern + ham cong
p, s = load("BaseInclude.h"); h0 = hi(s)
if "Rep3NapDo" not in s:
    s = rep(s, "extern int  g_nRep3StatSec;",
        "// [NAP 08/09 a] do thoi gian NAP tai nguyen tren luong ve (tep spr, jpeg, rut khung, giai ma, tao GPU)\r\n"
        "struct Rep3NapDo { unsigned n; double ms; double max; };\r\n"
        "extern Rep3NapDo g_napSpr, g_napJpeg, g_napKhung, g_napGiaiMa, g_napGpu;\r\n"
        "extern double g_dRep3NapKhung, g_dRep3NapKhungMax; extern unsigned g_uRep3NapKhung5, g_uRep3NapKhung16;\r\n"
        "void   Rep3NapCong(Rep3NapDo& d, double ms);\r\n"
        "double Rep3NapMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b);\r\n"
        "extern int  g_nRep3StatSec;")
save(p, s, h0); print("OK BaseInclude.h")

# ---------------- KRepresentShell3.cpp: bien, ham, RepresentEnd, in thong ke, mac dinh Rep3Api=11
p, s = load("KRepresentShell3.cpp"); h0 = hi(s)
if "g_napSpr" not in s:
    s = rep(s, "double   g_dRep3FxGiaiMaMs = 0.0;\t// tong ms giai ma + tao texture\r\n",
        "double   g_dRep3FxGiaiMaMs = 0.0;\t// tong ms giai ma + tao texture\r\n"
        "// [NAP 08/09 a] do NAP tren luong ve, in [REP3-NAP] moi Rep3StatSec giay\r\n"
        "Rep3NapDo g_napSpr = {0, 0, 0}, g_napJpeg = {0, 0, 0}, g_napKhung = {0, 0, 0}, g_napGiaiMa = {0, 0, 0}, g_napGpu = {0, 0, 0};\r\n"
        "double g_dRep3NapKhung = 0.0, g_dRep3NapKhungMax = 0.0; unsigned g_uRep3NapKhung5 = 0, g_uRep3NapKhung16 = 0;\r\n"
        "void Rep3NapCong(Rep3NapDo& d, double ms) { d.n++; d.ms += ms; if (ms > d.max) d.max = ms; g_dRep3NapKhung += ms; }\r\n"
        "double Rep3NapMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b)\r\n{\r\n"
        "\tstatic LARGE_INTEGER s_liTanSo = {0};\r\n"
        "\tif (!s_liTanSo.QuadPart) QueryPerformanceFrequency(&s_liTanSo);\r\n"
        "\treturn s_liTanSo.QuadPart ? (1000.0 * (double)(b.QuadPart - a.QuadPart) / (double)s_liTanSo.QuadPart) : 0.0;\r\n}\r\n")
    s = rep(s, "void KRepresentShell3::RepresentEnd()\r\n{\r\n",
        "void KRepresentShell3::RepresentEnd()\r\n{\r\n"
        "\t{\t// [NAP 08/09 a] tong ms nap trong khung nay -> max / dem khung nang\r\n"
        "\t\tif (g_dRep3NapKhung > g_dRep3NapKhungMax) g_dRep3NapKhungMax = g_dRep3NapKhung;\r\n"
        "\t\tif (g_dRep3NapKhung > 16.0) g_uRep3NapKhung16++; else if (g_dRep3NapKhung > 5.0) g_uRep3NapKhung5++;\r\n"
        "\t\tg_dRep3NapKhung = 0.0;\r\n\t}\r\n")
    s = rep(s, "\t\t\tg_uRep3FxTexNull = 0; g_uRep3FxAnhNull = 0; g_uRep3FxTaoHong = 0; g_uRep3FxKhungKhongTex = 0; g_uRep3FxGiaiMa = 0; g_dRep3FxGiaiMaMs = 0.0;\r\n",
        "\t\t\tg_uRep3FxTexNull = 0; g_uRep3FxAnhNull = 0; g_uRep3FxTaoHong = 0; g_uRep3FxKhungKhongTex = 0; g_uRep3FxGiaiMa = 0; g_dRep3FxGiaiMaMs = 0.0;\r\n"
        "\t\t\tRep3Log(\"[REP3-NAP] %ds tren luong ve: tep spr %u lan %.1f ms (max %.1f) | jpeg %u lan %.1f ms (max %.1f) | rut khung %u lan %.1f ms (max %.2f) | giai ma %u %.1f ms (max %.2f) | tao GPU %u %.1f ms (max %.2f) | khung co nap >5 ms: %u, >16 ms: %u, max %.1f ms/khung\",\t// [NAP 08/09 a]\r\n"
        "\t\t\t\tg_nRep3StatSec, g_napSpr.n, g_napSpr.ms, g_napSpr.max, g_napJpeg.n, g_napJpeg.ms, g_napJpeg.max, g_napKhung.n, g_napKhung.ms, g_napKhung.max,\r\n"
        "\t\t\t\tg_napGiaiMa.n, g_napGiaiMa.ms, g_napGiaiMa.max, g_napGpu.n, g_napGpu.ms, g_napGpu.max, g_uRep3NapKhung5, g_uRep3NapKhung16, g_dRep3NapKhungMax);\r\n"
        "\t\t\tmemset(&g_napSpr, 0, sizeof(g_napSpr)); memset(&g_napJpeg, 0, sizeof(g_napJpeg)); memset(&g_napKhung, 0, sizeof(g_napKhung)); memset(&g_napGiaiMa, 0, sizeof(g_napGiaiMa)); memset(&g_napGpu, 0, sizeof(g_napGpu));\r\n"
        "\t\t\tg_dRep3NapKhungMax = 0.0; g_uRep3NapKhung5 = 0; g_uRep3NapKhung16 = 0;\r\n")
if 'Rep3Ini("Rep3Api", 11)' not in s:
    s = rep(s, "int  g_nRep3Api       = 9;\t// [D3D11 08/09]", "int  g_nRep3Api       = 11;\t// [D3D11 08/09] [NAP 08/09 #0] mac dinh 11: CD3D11Shim::Init do IDXGIFactory2 + feature level, khong du -> tu lui D3D9")
    s = rep(s, '\tg_nRep3Api       = Rep3Ini("Rep3Api", 9);\t// [D3D11 08/09]', '\tg_nRep3Api       = Rep3Ini("Rep3Api", 11);\t// [D3D11 08/09] [NAP 08/09 #0] mac dinh 11, tu lui D3D9 khi may khong du')
save(p, s, h0); print("OK KRepresentShell3.cpp")

# ---------------- TextureResMgr.cpp: thoi gian nap tep spr / jpeg
p, s = load("TextureResMgr.cpp"); h0 = hi(s)
if "g_napSpr" not in s:
    s = rep(s, "\t\tpRet = new TextureResSpr;\r\n\t\tif (!pRet)\r\n\t\t\tbreak;\r\n\t\tif (!pRet->LoadImage((LPSTR)pszImageFile, nType))\r\n\t\t{\r\n",
        "\t\tpRet = new TextureResSpr;\r\n\t\tif (!pRet)\r\n\t\t\tbreak;\r\n"
        "\t\t{\tLARGE_INTEGER liA, liB; QueryPerformanceCounter(&liA);\t// [NAP 08/09 a] tep spr: pak + giai nen\r\n"
        "\t\t\tbool bNap = pRet->LoadImage((LPSTR)pszImageFile, nType);\r\n"
        "\t\t\tQueryPerformanceCounter(&liB); Rep3NapCong(g_napSpr, Rep3NapMs(liA, liB));\r\n"
        "\t\tif (!bNap)\r\n\t\t{\r\n")
    # dong ket khoi: sau 'break;\r\n\t\t}\r\n\t\tbreak;\r\n\tcase ISI_T_BITMAP16:' can them '}' dong khoi { lay thoi gian
    s = rep(s, "\t\t\tdelete pRet;\r\n\t\t\tpRet = NULL;\r\n\t\t\tbreak;\r\n\t\t}\r\n\t\tbreak;\r\n\tcase ISI_T_BITMAP16:\r\n",
        "\t\t\tdelete pRet;\r\n\t\t\tpRet = NULL;\r\n\t\t\tbreak;\r\n\t\t}\r\n\t\t}\r\n\t\tbreak;\r\n\tcase ISI_T_BITMAP16:\r\n")
    s = rep(s, "\t\t\tpRet = new TextureResBmp;\r\n\t\t\tif (!pRet)\r\n\t\t\t\tbreak;\r\n\t\t\tif (!pRet->LoadImage((LPSTR)pszImageFile, nType))\r\n\t\t\t{\r\n",
        "\t\t\tpRet = new TextureResBmp;\r\n\t\t\tif (!pRet)\r\n\t\t\t\tbreak;\r\n"
        "\t\t\tLARGE_INTEGER liA, liB; QueryPerformanceCounter(&liA);\t// [NAP 08/09 a] jpeg nen\r\n"
        "\t\t\tbool bNap = pRet->LoadImage((LPSTR)pszImageFile, nType);\r\n"
        "\t\t\tQueryPerformanceCounter(&liB); Rep3NapCong(g_napJpeg, Rep3NapMs(liA, liB));\r\n"
        "\t\t\tif (!bNap)\r\n\t\t\t{\r\n")
save(p, s, h0); print("OK TextureResMgr.cpp")

# ---------------- TextureRes.cpp: rut khung, giai ma, tao GPU
p, s = load("TextureRes.cpp"); h0 = hi(s)
if "g_napKhung" not in s:
    s = rep(s, "\t\tSPRFRAME *pFrame = (SPRFRAME *)SprGetFrame((SPRHEAD*)m_pHeader, nFrame);\r\n",
        "\t\tLARGE_INTEGER liK0, liK1; QueryPerformanceCounter(&liK0);\t// [NAP 08/09 a] rut khung tu pak\r\n"
        "\t\tSPRFRAME *pFrame = (SPRFRAME *)SprGetFrame((SPRHEAD*)m_pHeader, nFrame);\r\n"
        "\t\tQueryPerformanceCounter(&liK1); Rep3NapCong(g_napKhung, Rep3NapMs(liK0, liK1));\r\n")
    s = rep(s, "\tLARGE_INTEGER liFx0, liFx1;\t// [FX 07/09] do giai ma dong bo tren luong ve\r\n\tQueryPerformanceCounter(&liFx0);\r\n",
        "\tLARGE_INTEGER liFx0, liFx1, liFxDec;\t// [FX 07/09] do giai ma dong bo tren luong ve; [NAP 08/09 a] liFxDec = xong giai ma RLE\r\n\tQueryPerformanceCounter(&liFx0); liFxDec = liFx0;\r\n")
    s = rep(s, "\t\t\t\t\t\tnW * nH, m_pPal16, (int)m_nColors);\t// [REP3 03/09 RAM2] ban co kiem bien\r\n\r\n\tfor(i=0; i<m_pFrameInfo[nFrame].nTexNum; i++)\r\n",
        "\t\t\t\t\t\tnW * nH, m_pPal16, (int)m_nColors);\t// [REP3 03/09 RAM2] ban co kiem bien\r\n"
        "\tQueryPerformanceCounter(&liFxDec);\t// [NAP 08/09 a]\r\n\r\n\tfor(i=0; i<m_pFrameInfo[nFrame].nTexNum; i++)\r\n")
    s = rep(s, "\tQueryPerformanceCounter(&liFx1);\t// [FX 07/09]\r\n\tg_uRep3FxGiaiMa++;\r\n\tg_dRep3FxGiaiMaMs += Rep3FxMs(liFx0, liFx1);\r\n\treturn;\r\n",
        "\tQueryPerformanceCounter(&liFx1);\t// [FX 07/09]\r\n\tg_uRep3FxGiaiMa++;\r\n\tg_dRep3FxGiaiMaMs += Rep3FxMs(liFx0, liFx1);\r\n"
        "\tRep3NapCong(g_napGiaiMa, Rep3FxMs(liFx0, liFxDec)); Rep3NapCong(g_napGpu, Rep3FxMs(liFxDec, liFx1));\t// [NAP 08/09 a]\r\n\treturn;\r\n")
    s = rep(s, "\tQueryPerformanceCounter(&liFx1);\t// [FX 07/09] truoc day hong im lang\r\n\tg_uRep3FxTaoHong++;\r\n\tg_dRep3FxGiaiMaMs += Rep3FxMs(liFx0, liFx1);\r\n",
        "\tQueryPerformanceCounter(&liFx1);\t// [FX 07/09] truoc day hong im lang\r\n\tg_uRep3FxTaoHong++;\r\n\tg_dRep3FxGiaiMaMs += Rep3FxMs(liFx0, liFx1);\r\n"
        "\tRep3NapCong(g_napGpu, Rep3FxMs(liFx0, liFx1));\t// [NAP 08/09 a] nhanh hong: tinh ca vao tao GPU\r\n")
save(p, s, h0); print("OK TextureRes.cpp")
print("XONG NAP a")
