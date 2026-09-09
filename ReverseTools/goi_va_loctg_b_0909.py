# -*- coding: utf-8 -*-
"""goi_va_loctg_b_0909.py - [LOCTG b] bo loc CHON LOC: chi lam cham buoc DI LEN TU VUNG TOI.

Chu (11:xx): "da do hon nhung ... mau bi toi lai khi di chuyen" + "khong co phuong an nao tot hon a?".
Bo loc doi xung lam cham MOI chuyen doi => vet phia sau (buoc xuong bi cham) va toi (buoc len bi cham).
Tam VA chi cham o buoc di len tu vung toi. => Shader: tung kenh, a_eff = a * w(h) * [c > h],
w = saturate(1 - h * k), k = 255 / Rep3LocToi. Buoc xuong va buoc len tu vung sang qua tuc thi.
  [Client] Rep3LocKieu = 1 (chon loc, mac dinh) | 0 = doi xung nhu ban dau
  [Client] Rep3LocToi  = 128 (muc sang 0..255 ma tren do buoc len KHONG bi lam cham; nho hon = it can thiep)
Chi doi Represent3 (hang so shader + 2 khoa). Ba tep D3D9on11* la LF thuan - giu dung EOL.
"""
import io
import sys

R = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"
F_D = R + r"\D3D9on11Dev.cpp"
F_H = R + r"\D3D9on11.h"
F_S = R + r"\KRepresentShell3.cpp"


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, s.count("\r\n"), s.count("\n") - s.count("\r\n"), sum(1 for c in s if ord(c) >= 0x80)


def ghi(p, s, crlf0, lf0, h0, ten):
    eol = "\r\n" if crlf0 > 0 else "\n"
    if crlf0 == 0:
        s = s.replace("\r\n", "\n")
    ok = (s.count("\r\n") == 0) if crlf0 == 0 else (s.count("\n") - s.count("\r\n") == lf0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "�" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK %s (%s)" % (ten, "LF" if crlf0 == 0 else "CRLF"))


def rep(s, old, new, ten):
    n = s.count(old)
    if n != 1:
        print("FAIL neo %s: %d" % (ten, n)); sys.exit(1)
    return s.replace(old, new)


NL = "\r\n"   # soan bang CRLF; ghi() doi ve LF neu tep la LF

# ---- D3D9on11.h: extern
s, c0, l0, h0 = doc(F_H)
if "g_nRep3LocKieu" not in s:
    s = rep(s, "extern int      g_nRep3LocMs;",
            "extern int      g_nRep3LocKieu;      // [LOCTG b] 1 = chon loc (chi lam cham buoc len tu vung toi), 0 = doi xung" + NL +
            "extern float    g_fRep3LocK;         // [LOCTG b] k = 255 / Rep3LocToi" + NL +
            "extern int      g_nRep3LocMs;", "H extern")
    ghi(F_H, s, c0, l0, h0, "D3D9on11.h")
else:
    print("D3D9on11.h da co")

# ---- D3D9on11Dev.cpp: ghi them k va kieu vao cbuffer
s, c0, l0, h0 = doc(F_D)
if "g_fRep3LocK" not in s:
    s = rep(s, "float v[4] = { a, 0.0f, 0.0f, 0.0f };",
            "float v[4] = { a, g_fRep3LocK, (float)g_nRep3LocKieu, 0.0f };", "D cbuffer")
    ghi(F_D, s, c0, l0, h0, "D3D9on11Dev.cpp")
else:
    print("D3D9on11Dev.cpp da co")

# ---- KRepresentShell3.cpp: bien + config + log
s, c0, l0, h0 = doc(F_S)
if "g_nRep3LocKieu" not in s:
    s = rep(s, "int  g_nRep3LocMs     = 8;",
            "int  g_nRep3LocKieu   = 1;" + "\t" + "// [LOCTG b] 1 = chon loc, 0 = doi xung" + NL +
            "float g_fRep3LocK     = 2.0f;" + "\t" + "// [LOCTG b] 255 / Rep3LocToi" + NL +
            "int  g_nRep3LocMs     = 8;", "S bien")
    s = rep(s, "\tif (g_nRep3LocMs > 100) g_nRep3LocMs = 100;",
            "\tif (g_nRep3LocMs > 100) g_nRep3LocMs = 100;" + NL +
            "\tg_nRep3LocKieu   = Rep3Ini(\"Rep3LocKieu\", 1) ? 1 : 0;" + "\t" + "// [LOCTG b]" + NL +
            "\t{ int nToi = Rep3Ini(\"Rep3LocToi\", 128); if (nToi < 8) nToi = 8; if (nToi > 255) nToi = 255; g_fRep3LocK = 255.0f / (float)nToi; }",
            "S config")
    s = rep(s, "Rep3Log(\"[LOCTG] tau=%d ms | %u khung da tron\", g_nRep3LocMs, g_uRep3LocKhung);",
            "Rep3Log(\"[LOCTG] tau=%d ms kieu=%d toi=%d | %u khung da tron\", g_nRep3LocMs, g_nRep3LocKieu, (int)(255.0f / g_fRep3LocK + 0.5f), g_uRep3LocKhung);",
            "S log")
    ghi(F_S, s, c0, l0, h0, "KRepresentShell3.cpp")
else:
    print("KRepresentShell3.cpp da co")
print("XONG LOCTG b")
