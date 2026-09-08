# -*- coding: ascii -*-
"""goi_va_d3d11k_latency_0809.py - [D3D11 08/09 k] [Client] Rep3Latency = so khung toi da cho trinh chieu (SetMaximumFrameLatency),
mac dinh 3 (= mac dinh DXGI). Do 12:38 (1) va 12:49 (2): deu bo ~31 khung/s -> DWM cua so flip model can 3 de nhan 1 khung moi vsync."""
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
if "g_nRep3Latency" not in s:
    s = rep(s, "extern int      g_nRep3Flip;", "extern int      g_nRep3Latency;      // [Client] Rep3Latency: so khung toi da cho trinh chieu (1..3), mac dinh 3\nextern int      g_nRep3Flip;")
save(p, s); print("OK D3D9on11.h")

p, s = load("D3D9on11Dev.cpp")
if "g_nRep3Latency" not in s:
    s = rep(s, "{ pDev1->SetMaximumFrameLatency(2); pDev1->Release(); }", "{ pDev1->SetMaximumFrameLatency(g_nRep3Latency < 1 ? 1 : (g_nRep3Latency > 16 ? 16 : g_nRep3Latency)); pDev1->Release(); }")
    s = rep(s, "\tR11Log(\"atlas: %s\", m_pAtlas ? \"BAT (trang 1024x1024 BGRA8, texture <= 512 khong RT)\" : \"tat\");", "\tR11Log(\"atlas: %s | do tre trinh chieu %d khung | gop lenh %d\", m_pAtlas ? \"BAT (trang 1024x1024 BGRA8, texture <= 512 khong RT)\" : \"tat\", g_nRep3Latency, g_nRep3Batch);")
save(p, s); print("OK D3D9on11Dev.cpp")

p, s = load("KRepresentShell3.cpp"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3Latency" not in s:
    s = rep(s, "int  g_nRep3Batch     = 1;", "int  g_nRep3Latency   = 3;\t// [D3D11 08/09 k] so khung toi da cho trinh chieu\r\nint  g_nRep3Batch     = 1;")
    s = rep(s, '\tg_nRep3Batch     = Rep3Ini("Rep3Batch", 1);\t// [D3D11 08/09 j]\r\n', '\tg_nRep3Batch     = Rep3Ini("Rep3Batch", 1);\t// [D3D11 08/09 j]\r\n\tg_nRep3Latency   = Rep3Ini("Rep3Latency", 3);\t// [D3D11 08/09 k]\r\n')
save(p, s, h0); print("OK KRepresentShell3.cpp")

p, s = load("BaseInclude.h"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3Latency" not in s:
    s = rep(s, "extern int  g_nRep3Batch;", "extern int  g_nRep3Latency;\t// [D3D11 08/09 k]\r\nextern int  g_nRep3Batch;")
save(p, s, h0); print("OK BaseInclude.h")
print("XONG")
