# -*- coding: ascii -*-
"""goi_va_d3d11l_nowait_tuychon_0809.py - [D3D11 08/09 l] Present: mac dinh CHO nhu D3D9 (hang 3 khung, day thi doi vsync), khong bo khung.
Do 12:38/12:49: DO_NOT_WAIT lam bo ~31 khung/s trong game (game ve theo tung dot nhieu khung sat nhau; harness nhip deu thi 0) ->
hinh cap nhat ~32 lan/s = giat. Hang doi 3 khung trai deu cac khung cua mot dot len cac vsync ke tiep = muot nhu D3D9.
[Client] Rep3NoWait=1 de bat lai kieu bo khung (khong chan CPU)."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"
def load(name):
    p = ROOT + "\\" + name
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    if name.startswith("D3D9on11"): s = s.replace("\r\n", "\n")
    return p, s
def save(p, s, h0=None):
    if h0 is not None and sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao", p); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:70], c, n)); sys.exit(1)
    return s.replace(old, new)

p, s = load("D3D9on11.h")
if "g_nRep3NoWait" not in s:
    s = rep(s, "extern int      g_nRep3Latency;", "extern int      g_nRep3NoWait;       // [Client] Rep3NoWait: 1 = Present khong cho (hang day thi bo khung); mac dinh 0 = cho nhu D3D9\nextern int      g_nRep3Latency;")
save(p, s); print("OK D3D9on11.h")

p, s = load("D3D9on11Dev.cpp")
if "g_nRep3NoWait" not in s:
    s = rep(s, "\tif (interval == 0 && g_nRep3Flip) flags |= DXGI_PRESENT_DO_NOT_WAIT;\t// [D3D11 08/09 g] hang day -> bo khung, khong chan\n",
        "\tif (interval == 0 && g_nRep3Flip && g_nRep3NoWait) flags |= DXGI_PRESENT_DO_NOT_WAIT;\t// [D3D11 08/09 l] chi khi Rep3NoWait=1: hang day -> bo khung (game ve theo dot -> giat)\n")
    s = rep(s, "| do tre trinh chieu %d khung | gop lenh %d\", m_pAtlas ?", "| do tre trinh chieu %d khung, khong cho %d | gop lenh %d\", m_pAtlas ?")
    s = rep(s, "\"tat\", g_nRep3Latency, g_nRep3Batch);", "\"tat\", g_nRep3Latency, g_nRep3NoWait, g_nRep3Batch);")
save(p, s); print("OK D3D9on11Dev.cpp")

p, s = load("KRepresentShell3.cpp"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3NoWait" not in s:
    s = rep(s, "int  g_nRep3Latency   = 3;", "int  g_nRep3NoWait    = 0;\t// [D3D11 08/09 l] 0 = Present cho nhu D3D9 (khong bo khung)\r\nint  g_nRep3Latency   = 3;")
    s = rep(s, '\tg_nRep3Latency   = Rep3Ini("Rep3Latency", 3);\t// [D3D11 08/09 k]\r\n', '\tg_nRep3Latency   = Rep3Ini("Rep3Latency", 3);\t// [D3D11 08/09 k]\r\n\tg_nRep3NoWait    = Rep3Ini("Rep3NoWait", 0);\t// [D3D11 08/09 l]\r\n')
save(p, s, h0); print("OK KRepresentShell3.cpp")

p, s = load("BaseInclude.h"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3NoWait" not in s:
    s = rep(s, "extern int  g_nRep3Latency;\t// [D3D11 08/09 k]", "extern int  g_nRep3NoWait;\t// [D3D11 08/09 l]\r\nextern int  g_nRep3Latency;\t// [D3D11 08/09 k]")
save(p, s, h0); print("OK BaseInclude.h")
print("XONG")
