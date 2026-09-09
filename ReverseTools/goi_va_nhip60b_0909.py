# -*- coding: utf-8 -*-
"""goi_va_nhip60b_0909.py - [NHIP60 b] buoc theo SO KHUNG CHAN thay vi theo dong ho.

Chu thu 481cc62b (chi con nhip the gioi 60): "ten nhan vat van con nhay khi di chuyen nhung mau do am hon
nhieu". Mau do => mo hinh dung (buoc tho hon, mat bam kem hon). Nhay => 60 khong chia het 143: theo dong ho
ra buoc xen ke 3 khung (20,97 ms) - 2 khung (13,98 ms), chu manh lo nhip lech.
Sua: do khoang cach khung ve (EMA), K = lam tron(paintHz / NhipTheGioi), POSSHIFT chay MOI K KHUNG.
143 Hz: Nhip 60 -> K=2 (71,5 Hz deu), 48 -> K=3 (47,7 Hz), 40 -> K=4. In K vao dong [INTERP].
"""
import io
import sys

NL = "\r\n"
T = "\t"
BS = chr(92)
P = r"D:\GAMEDEVNEW_wt_delta\Sources\Core\Src\CoreShell.cpp"
s = io.open(P, "r", encoding="latin-1", newline="").read()
h0 = sum(1 for c in s if ord(c) >= 0x80)


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


if "[NHIP60 b]" in s:
    print("da va roi"); sys.exit(0)

# 1) bien in K
s = rep(s, "static int      s_nIpLechMax = 0, s_nIpLechNguoi = 0;",
        "static int      s_nIpLechMax = 0, s_nIpLechNguoi = 0;" + NL +
        "int             g_nIpBuocK = 1;" + T + "// [NHIP60 b] so khung ve moi buoc the gioi (1 = moi khung)",
        "bien K")
s = rep(s, '" | lech VE-TICK: npc max %d, nguoi choi max %d | vuot bien vung %u' + BS + 'n",',
        '" | lech VE-TICK: npc max %d, nguoi choi max %d | vuot bien vung %u | buoc the gioi moi %d khung' + BS + 'n",',
        "format")
s = rep(s, "(double)s_uIpNpc / s_uIpFrame, s_uIpNpcMax, s_nIpLechMax, s_nIpLechNguoi, s_uIpVuot);",
        "(double)s_uIpNpc / s_uIpFrame, s_uIpNpcMax, s_nIpLechMax, s_nIpLechNguoi, s_uIpVuot, g_nIpBuocK);",
        "args")

# 2) thay khoi dong ho bang khoi dem khung
old = NL.join([
    T*3 + "static double s_dKeTiep = 0.0;",
])
new = NL.join([
    T*3 + "static double s_dKhungMs = 0.0;" + T + "// [NHIP60 b] khoang cach khung ve trung binh (EMA)",
    T*3 + "static DWORD  s_dwKhungTruoc = 0;",
    T*3 + "static int    s_nDemKhung = 0;",
])
s = rep(s, old, new, "static")

old = NL.join([
    T*3 + "if (s_nNhipTG > 0)",
    T*3 + "{",
    T*4 + "const double dNow = (double)timeGetTime();",
    T*4 + "if (s_dKeTiep == 0.0 || dNow - s_dKeTiep > 250.0 || dNow < s_dKeTiep - 250.0)",
    T*5 + "s_dKeTiep = dNow;" + T + "// lan dau / treo lau / dong ho quay vong: neo lai",
    T*4 + "if (dNow < s_dKeTiep)",
    T*5 + "break;" + T + "// chua toi luot: giu vi tri ve cua khung truoc (nRet giu 1)",
    T*4 + "s_dKeTiep += 1000.0 / (double)s_nNhipTG;" + T + "// tich luy, khong neo vao khung => trung binh dung nhip",
    T*3 + "}",
])
new = NL.join([
    T*3 + "if (s_nNhipTG > 0)",
    T*3 + "{",
    T*4 + "// [NHIP60 b] Buoc theo SO KHUNG CHAN: K = lam tron(paintHz / NhipTheGioi). Theo dong ho thi 60 tren",
    T*4 + "// man 143 Hz ra buoc xen ke 3 khung - 2 khung (20,97 / 13,98 ms) => chu manh 'nhay' (chu bao).",
    T*4 + "// 143 Hz: 60 -> K=2 (71,5 Hz deu), 48 -> K=3 (47,7), 40 -> K=4 (35,8).",
    T*4 + "const DWORD dwNow = timeGetTime();",
    T*4 + "if (s_dwKhungTruoc)",
    T*4 + "{",
    T*5 + "const double dt = (double)(dwNow - s_dwKhungTruoc);",
    T*5 + "if (dt > 0.0 && dt < 100.0)",
    T*6 + "s_dKhungMs = (s_dKhungMs > 0.0) ? (s_dKhungMs * 0.95 + dt * 0.05) : dt;",
    T*4 + "}",
    T*4 + "s_dwKhungTruoc = dwNow;",
    T*4 + "int nK = 1;",
    T*4 + "if (s_dKhungMs > 0.0)",
    T*4 + "{",
    T*5 + "nK = (int)(1000.0 / (s_dKhungMs * (double)s_nNhipTG) + 0.5);",
    T*5 + "if (nK < 1) nK = 1;",
    T*5 + "if (nK > 8) nK = 8;",
    T*4 + "}",
    T*4 + "g_nIpBuocK = nK;",
    T*4 + "if (++s_nDemKhung < nK)",
    T*5 + "break;" + T + "// chua toi luot: giu vi tri ve cua khung truoc (nRet giu 1)",
    T*4 + "s_nDemKhung = 0;",
    T*3 + "}",
])
s = rep(s, old, new, "khoi gate")

if sum(1 for c in s if ord(c) >= 0x80) != h0 or s.count("\n") != s.count("\r\n") or "\ufffd" in s:
    print("FAIL ma hoa"); sys.exit(1)
io.open(P, "w", encoding="latin-1", newline="").write(s)
print("OK CoreShell.cpp [NHIP60 b] (byte cao %d giu nguyen)" % h0)
