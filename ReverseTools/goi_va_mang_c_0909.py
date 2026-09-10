# -*- coding: utf-8 -*-
"""goi_va_mang_c_0909.py - [MANG 09/09 c] ring dinh 4 MB -> 16 MB + dem vong ring giua khung / Map max ms; ghi 64 texture RIENG dau tien.

So do ban [MANG b] (18:0x): DrawPrimitives max 30-38 ms o 3 cua so danh tran (khong trung luc mang lon len, tao GPU max 1,9-16 ms).
Nghi: ring 4 MB, lo gop toi 2 MB/lan, danh tran 24 000 quad x 6 dinh x 36 B = 5,2 MB/khung -> DISCARD 2-3 lan/khung,
driver co luc chan (het vung doi ten). Sua: ring 16 MB (1 DISCARD dau khung). Do: 'ring: N vong, Map max X ms' tren [REP3-NAP].
Lo quad con vo 80-350 lan/khung vi texture RIENG <-> atlas (chu? bang UI?): ghi 64 lan tao texture rieng dau (kich thuoc, dinh dang).
"""
import io
import sys

D = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/"
TAG = "[MANG 09/09 c]"
T = "\t"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, sum(1 for c in s if ord(c) >= 0x80), s.count("\n") - s.count("\r\n"), s.count("\r\n")


def ghi(p, s, h0, lf0, crlf0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten, n_mong=1):
    n = s.count(old)
    if n != n_mong:
        print("FAIL neo %s: %d (mong %d)" % (ten, n, n_mong)); sys.exit(1)
    return s.replace(old, new)


# ---------------- D3D9on11Dev.cpp: ring 16 MB + dem
P = D + "D3D9on11Dev.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, "#define R11_RING_SIZE (4 * 1024 * 1024)", "#define R11_RING_SIZE (16 * 1024 * 1024)" + T + "// " + TAG + " 4 -> 16 MB: danh tran 5 MB dinh/khung, lo gop 2 MB -> 4 MB phai DISCARD 2-3 lan/khung", "V ring size")
    s = rep(s, "unsigned g_uRep3BatchDraws = 0;" + NL,
            "unsigned g_uRep3BatchDraws = 0;" + NL + "unsigned g_uRep3RingVong = 0; double g_dRep3RingMapMax = 0.0; unsigned g_uRep3TexRiengTao = 0;" + T + "// " + TAG + NL, "V dem")
    s = rep(s, T + "if (m_bRingDiscard || pos + bytes > m_ringSize) { pos = 0; mapType = D3D11_MAP_WRITE_DISCARD; m_bRingDiscard = false; }" + NL,
            T + "if (m_bRingDiscard || pos + bytes > m_ringSize) { if (!m_bRingDiscard) g_uRep3RingVong++; pos = 0; mapType = D3D11_MAP_WRITE_DISCARD; m_bRingDiscard = false; }" + T + "// " + TAG + " dem vong GIUA khung" + NL, "V vong")
    s = rep(s, T + "HRESULT hr = m_pCtx->Map(m_pRing, 0, mapType, 0, &ms);" + NL,
            T + "LARGE_INTEGER tM0, tM1; QueryPerformanceCounter(&tM0);" + NL +
            T + "HRESULT hr = m_pCtx->Map(m_pRing, 0, mapType, 0, &ms);" + NL +
            T + "QueryPerformanceCounter(&tM1); { const double dM = R11Ms(tM0, tM1); if (dM > g_dRep3RingMapMax) g_dRep3RingMapMax = dM; }" + T + "// " + TAG + NL, "V map ms")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11Dev.cpp")
else:
    print("D3D9on11Dev.cpp da co")

# ---------------- D3D9on11.cpp: ghi texture rieng
P = D + "D3D9on11.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    neo = T + "HRESULT hr = m_pDev->m_pDev->CreateTexture2D(&td, pInit ? &sr : NULL, &m_pGpu);" + NL
    s = rep(s, neo,
            T + "{" + T + "// " + TAG + " ai la texture RIENG (ngoai atlas, cat lo quad 80-350 lan/khung)? ghi 64 lan dau",
            "") if False else s
    them = (T + "{" + T + "// " + TAG + " ai la texture RIENG (ngoai atlas, cat lo quad 80-350 lan/khung)? ghi 64 lan dau" + NL +
            T*2 + "extern unsigned g_uRep3TexRiengTao; static unsigned s_uDaGhi = 0; g_uRep3TexRiengTao++;" + NL +
            T*2 + "if (s_uDaGhi < 64) { s_uDaGhi++; R11Log(\"[D3D11] texture rieng #%u: %ux%u fmt %d usage 0x%X pool %d%s\", s_uDaGhi, m_w, m_h, (int)m_fmt, (unsigned)m_usage, (int)m_pool, bRt ? \" RT\" : \"\"); }" + NL +
            T + "}" + NL)
    s = rep(s, neo, them + neo, "T ghi rieng")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11.cpp")
else:
    print("D3D9on11.cpp da co")

# ---------------- KRepresentShell3.cpp: in [REP3-NAP]
P = D + "KRepresentShell3.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, "khung %.2f ms (max %.1f)\",", "khung %.2f ms (max %.1f) | ring: %u vong, Map max %.1f ms | texture rieng tao %u\",", "K fmt")
    s = rep(s, "g_uRep3VeKhung ? g_dRep3VeKhungTong / g_uRep3VeKhung : 0.0, g_dRep3VeKhungMax)",
            "g_uRep3VeKhung ? g_dRep3VeKhungTong / g_uRep3VeKhung : 0.0, g_dRep3VeKhungMax, g_uRep3RingVong, g_dRep3RingMapMax, g_uRep3TexRiengTao)", "K args")
    s = rep(s, "g_dRep3VeDpTong = 0.0; g_dRep3VeDpMax = 0.0; g_dRep3VeKhungTong = 0.0; g_dRep3VeKhungMax = 0.0;",
            "g_dRep3VeDpTong = 0.0; g_dRep3VeDpMax = 0.0; g_dRep3VeKhungTong = 0.0; g_dRep3VeKhungMax = 0.0; g_uRep3RingVong = 0; g_dRep3RingMapMax = 0.0; g_uRep3TexRiengTao = 0; /* " + TAG + " */", "K reset")
    # khai bao extern gan dau tep (sau dong khai bao g_dRep3VeDpKhung...)
    s = rep(s, "double g_dRep3VeDpKhung = 0.0, g_dRep3VeDpTong = 0.0, g_dRep3VeDpMax = 0.0, g_dRep3VeKhungTong = 0.0, g_dRep3VeKhungMax = 0.0;",
            "double g_dRep3VeDpKhung = 0.0, g_dRep3VeDpTong = 0.0, g_dRep3VeDpMax = 0.0, g_dRep3VeKhungTong = 0.0, g_dRep3VeKhungMax = 0.0;" + NL +
            "extern unsigned g_uRep3RingVong; extern double g_dRep3RingMapMax; extern unsigned g_uRep3TexRiengTao;" + T + "// " + TAG, "K extern")
    ghi(P, s, h0, lf0, crlf0, "KRepresentShell3.cpp")
else:
    print("KRepresentShell3.cpp da co")
print("XONG " + TAG)
