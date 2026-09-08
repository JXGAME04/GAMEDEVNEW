# -*- coding: ascii -*-
"""goi_va_d3d11b_flip_thongke_0809.py - [D3D11 08/09 b] them ini Rep3Flip + thong ke Present/ve vao dong [REP3] (KRepresentShell3.cpp)."""
import io, re, sys
p = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3\KRepresentShell3.cpp"
def hb(s): return sum(1 for ch in s if ord(ch) >= 0x80)
s = io.open(p, "r", encoding="latin-1", newline="").read(); h0 = hb(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:60], c, n)); sys.exit(1)
    return s.replace(old, new)
s = rep(s, "int  g_nRep3ApiOn     = 9;\t// [D3D11 08/09]\r\n", "int  g_nRep3ApiOn     = 9;\t// [D3D11 08/09]\r\nint  g_nRep3Flip      = 1;\t// [D3D11 08/09 b] 1 = swapchain flip, 0 = bitblt cu\r\n")
s = rep(s, '\tg_nRep3Api       = Rep3Ini("Rep3Api", 9);\t// [D3D11 08/09]\r\n', '\tg_nRep3Api       = Rep3Ini("Rep3Api", 9);\t// [D3D11 08/09]\r\n\tg_nRep3Flip      = Rep3Ini("Rep3Flip", 1);\t// [D3D11 08/09 b]\r\n')
s = rep(s, 'giai_ma %u khung %.1f ms | gpu tex %u (%u MB)",', 'giai_ma %u khung %.1f ms | gpu tex %u (%u MB) | d3d11: present TB %.2f ms, ve %u lenh %.1f us/lenh",')
s = rep(s, "g_uRep3FxGiaiMa, g_dRep3FxGiaiMaMs, g_uRep3GpuTexCount, (unsigned)(g_uRep3GpuTexBytes >> 20));",
    "g_uRep3FxGiaiMa, g_dRep3FxGiaiMaMs, g_uRep3GpuTexCount, (unsigned)(g_uRep3GpuTexBytes >> 20),\r\n"
    "\t\t\t\tg_uRep3Presents ? g_dRep3PresentMs / g_uRep3Presents : 0.0, g_uRep3Draws, g_uRep3Draws ? g_dRep3DrawMs * 1000.0 / g_uRep3Draws : 0.0);\r\n"
    "\t\t\tg_dRep3PresentMs = 0.0; g_uRep3Presents = 0; g_dRep3DrawMs = 0.0; g_uRep3Draws = 0;")
if hb(s) != h0 or re.search(r"[^\r]\n", s): print("FAIL byte cao / LF"); sys.exit(1)
io.open(p, "w", encoding="latin-1", newline="").write(s)
print("OK KRepresentShell3.cpp: Rep3Flip + thong ke present/ve")
