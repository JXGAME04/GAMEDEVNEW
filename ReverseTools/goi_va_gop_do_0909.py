# -*- coding: utf-8 -*-
"""goi_va_gop_do_0909.py - [GOP 09/09 do] dem LY DO lo quad bi vo (moi lan FlushBatch) va loai lenh ve ngay (khong gop).

So do (jx_rep3.log 16:4x, danh tran): moi khung 6 300 quad -> 4 500 Draw (1,3-1,5 quad/Draw) + ~2 100 lenh ve ngay.
Lo quad vo khi R11Applied khac (srv[2], blend, sampler, PS-cb st0/st1/alphatest, VS-cb, layout, vp/scissor/raster) hoac day.
Muc dich DUY NHAT: biet ly do nao chiem da so de chon cach gop (atlas mang trang neu la 'doi trang', khac neu la blend/PS).
In moi 30 s tren dong [GOP] ngay sau dong [REP3].
"""
import io
import sys

NL = "\r\n"
T = "\t"
D = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/"
TAG = "[GOP 09/09 do]"


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    # D3D9on11*.cpp dung LF thuan; KRepresentShell3.cpp dung CRLF -> tra ve kieu xuong dong de dung dung
    return s, sum(1 for c in s if ord(c) >= 0x80), s.count("\n") - s.count("\r\n"), s.count("\r\n")


def ghi(p, s, h0, lf0, crlf0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


# kiem cau truc PS-cb dung nhu gia dinh
h, _, _, _ = doc(D + "D3D9on11i.h")
if "struct R11PsCb { int st0[4]; int st0b[4]; int st1[4]; int st1b[4]; float at[4]; };" not in h:
    print("FAIL R11PsCb khac gia dinh"); sys.exit(1)

# ---------------- D3D9on11Dev.cpp ----------------
P = D + "D3D9on11Dev.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, "unsigned g_uRep3BatchDraws = 0;" + NL,
            "unsigned g_uRep3BatchDraws = 0;" + NL +
            "unsigned g_uRep3GopVo[12] = { 0 };" + T + "// " + TAG + " ly do vo lo quad: 0 doi trang atlas, 1 texture rieng, 2 srv1, 3 blend, 4 sampler, 5 ps st0, 6 ps st1, 7 alphatest, 8 vs, 9 layout, 10 vp/scissor/raster, 11 day" + NL +
            "unsigned g_uRep3VeNgay[4] = { 0 };" + T + "// " + TAG + " lenh ve ngay (khong gop): 0 fan, 1 list, 2 strip (khong phai quad), 3 khac" + NL,
            "dem")
    old = T*2 + "if (m_batchVerts && (memcmp(&a, &m_batchState, sizeof(a)) != 0 || m_batch.size() + 6 * stride > 2 * 1024 * 1024))" + NL + T*3 + "FlushBatch();" + NL
    new = NL.join([
        T*2 + "if (m_batchVerts && (memcmp(&a, &m_batchState, sizeof(a)) != 0 || m_batch.size() + 6 * stride > 2 * 1024 * 1024))",
        T*2 + "{",
        T*3 + "const R11Applied& b = m_batchState; int nLy = 11;" + T + "// " + TAG + " ly do dau tien theo thu tu uu tien",
        T*3 + "if (a.srv[0] != b.srv[0]) nLy = (m_tex[0] && m_tex[0]->m_bVirtual) ? 0 : 1;",
        T*3 + "else if (a.srv[1] != b.srv[1]) nLy = 2;",
        T*3 + "else if (a.pBlend != b.pBlend) nLy = 3;",
        T*3 + "else if (a.pSamp[0] != b.pSamp[0] || a.pSamp[1] != b.pSamp[1]) nLy = 4;",
        T*3 + "else if (memcmp(a.ps.st0, b.ps.st0, sizeof(a.ps.st0)) != 0 || memcmp(a.ps.st0b, b.ps.st0b, sizeof(a.ps.st0b)) != 0) nLy = 5;",
        T*3 + "else if (memcmp(a.ps.st1, b.ps.st1, sizeof(a.ps.st1)) != 0 || memcmp(a.ps.st1b, b.ps.st1b, sizeof(a.ps.st1b)) != 0) nLy = 6;",
        T*3 + "else if (memcmp(a.ps.at, b.ps.at, sizeof(a.ps.at)) != 0) nLy = 7;",
        T*3 + "else if (memcmp(&a.vs, &b.vs, sizeof(a.vs)) != 0) nLy = 8;",
        T*3 + "else if (a.pIL != b.pIL || a.stride != b.stride) nLy = 9;",
        T*3 + "else if (a.pRaster != b.pRaster || a.bScissor != b.bScissor || memcmp(&a.rcScissor, &b.rcScissor, sizeof(RECT)) != 0 || memcmp(&a.vp, &b.vp, sizeof(a.vp)) != 0) nLy = 10;",
        T*3 + "g_uRep3GopVo[nLy]++;",
        T*3 + "FlushBatch();",
        T*2 + "}",
        "",
    ])
    s = rep(s, old, new, "vo lo")
    old = T + "FlushIfPending();" + NL + T + "// ---- lenh khac: ve ngay" + NL
    s = rep(s, old, old + T + "g_uRep3VeNgay[type == D3DPT_TRIANGLEFAN ? 0 : (type == D3DPT_TRIANGLELIST ? 1 : (type == D3DPT_TRIANGLESTRIP ? 2 : 3))]++;" + T + "// " + TAG + NL, "ve ngay")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11Dev.cpp")
else:
    print("D3D9on11Dev.cpp da co")

# ---------------- KRepresentShell3.cpp ----------------
P = D + "KRepresentShell3.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    neo = "g_uRep3BatchQuads, g_uRep3BatchDraws, g_uRep3PalRows);" + NL
    them = NL.join([
        T*3 + "{" + T + "// " + TAG,
        T*4 + "extern unsigned g_uRep3GopVo[12]; extern unsigned g_uRep3VeNgay[4];",
        T*4 + "Rep3Log(\"[GOP] vo lo quad: doi trang atlas %u, texture rieng %u, srv1 %u, blend %u, sampler %u, ps st0 %u, ps st1 %u, alphatest %u, vs %u, layout %u, vp/scissor %u, day %u | ve ngay: fan %u, list %u, strip %u, khac %u\",",
        T*5 + "g_uRep3GopVo[0], g_uRep3GopVo[1], g_uRep3GopVo[2], g_uRep3GopVo[3], g_uRep3GopVo[4], g_uRep3GopVo[5], g_uRep3GopVo[6], g_uRep3GopVo[7], g_uRep3GopVo[8], g_uRep3GopVo[9], g_uRep3GopVo[10], g_uRep3GopVo[11],",
        T*5 + "g_uRep3VeNgay[0], g_uRep3VeNgay[1], g_uRep3VeNgay[2], g_uRep3VeNgay[3]);",
        T*4 + "memset(g_uRep3GopVo, 0, sizeof(g_uRep3GopVo)); memset(g_uRep3VeNgay, 0, sizeof(g_uRep3VeNgay));",
        T*3 + "}",
        "",
    ])
    s = rep(s, neo, neo + them, "in [GOP]")
    ghi(P, s, h0, lf0, crlf0, "KRepresentShell3.cpp")
else:
    print("KRepresentShell3.cpp da co")
print("XONG " + TAG)
