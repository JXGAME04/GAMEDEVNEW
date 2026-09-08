# -*- coding: ascii -*-
"""goi_va_d3d11o_ve_f_0809.py - [D3D11 08/09 o] Chu 13:2x: ban [n] (doi tuong cho khung) "lag giat khong choi duoc".
 - Loi: BindRenderTarget cho doi tuong MOI LAN gan render target (Clear, SetRenderTarget ban do nho...) -> moi lan cho tieu mot
   suat khung -> chan nhieu lan trong mot khung. Sua: cho TOI DA MOT LAN moi khung (co m_bWaitedThisFrame, dat lai o Present).
 - Mac dinh quay ve cau hinh trinh chieu cua ban [f] 12:19 (chu choi khong keu giat): flip discard, 2 buffer, KHONG goi
   SetMaximumFrameLatency (Rep3Latency=0), KHONG doi tuong cho (Rep3Waitable=0), KHONG DO_NOT_WAIT (Rep3NoWait=0):
   Present(0,0) tu CHAN khi hang day (do 12:19: present 6-16 ms = doi vsync, fps 60, khong bo khung).
   Quan sat: tu khi goi SetMaximumFrameLatency (ban g..m) Present tra WAS_STILL_DRAWING 31 lan/s thay vi chan."""
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

p, s = load("D3D9on11i.h")
if "m_bWaitedThisFrame" not in s:
    s = rep(s, "\tHANDLE          m_hWaitable;			// [n] doi tuong cho khung (NULL = khong dung)\n", "\tHANDLE          m_hWaitable;			// [n] doi tuong cho khung (NULL = khong dung)\n\tbool            m_bWaitedThisFrame;		// [o] da cho doi tuong trong khung nay\n")
save(p, s); print("OK D3D9on11i.h")

p, s = load("D3D9on11Dev.cpp")
if "m_bWaitedThisFrame" not in s:
    s = rep(s, "m_hWaitable = NULL; m_uStillLogged = 0; m_liLastPresent.QuadPart = 0;\n", "m_hWaitable = NULL; m_bWaitedThisFrame = false; m_uStillLogged = 0; m_liLastPresent.QuadPart = 0;\n")
    # SetMaximumFrameLatency (device) chi khi Rep3Latency > 0
    s = rep(s, "\t\tif (SUCCEEDED(m_pDev->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pDev1)) && pDev1) { pDev1->SetMaximumFrameLatency(g_nRep3Latency < 1 ? 1 : (g_nRep3Latency > 16 ? 16 : g_nRep3Latency)); pDev1->Release(); }\n",
        "\t\tif (g_nRep3Latency > 0 && SUCCEEDED(m_pDev->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pDev1)) && pDev1) { pDev1->SetMaximumFrameLatency(g_nRep3Latency > 16 ? 16 : g_nRep3Latency); pDev1->Release(); }\t// [o] 0 = khong dong (mac dinh DXGI, Present tu chan)\n")
    s = rep(s, "\t\t{\n\t\t\tpSc2->SetMaximumFrameLatency(g_nRep3Latency < 1 ? 1 : (g_nRep3Latency > 16 ? 16 : g_nRep3Latency));\n", "\t\t{\n\t\t\tpSc2->SetMaximumFrameLatency(g_nRep3Latency < 1 ? 3 : (g_nRep3Latency > 16 ? 16 : g_nRep3Latency));\n")
    # cho toi da mot lan moi khung
    s = rep(s, "\tif (m_hWaitable) WaitForSingleObjectEx(m_hWaitable, 1000, TRUE);\t// [n] cho toi khi hang trinh chieu con cho (nhu D3D9 day hang)\n",
        "\tif (m_hWaitable && !m_bWaitedThisFrame) { WaitForSingleObjectEx(m_hWaitable, 1000, TRUE); m_bWaitedThisFrame = true; }\t// [o] MOT lan moi khung (moi lan cho tieu mot suat)\n")
    s = rep(s, "\tm_bRtBound = false;\n\tm_ringPos = 0; m_bRingDiscard = true;\n\tQueryPerformanceCounter(&t1);", "\tm_bRtBound = false; m_bWaitedThisFrame = false;\n\tm_ringPos = 0; m_bRingDiscard = true;\n\tQueryPerformanceCounter(&t1);")
    s = rep(s, "\tif (m_hWaitable) WaitForSingleObjectEx(m_hWaitable, 1000, TRUE);\t// [n] sau resize, cho lan dau\n", "\tm_bWaitedThisFrame = false;\t// [o] sau resize\n")
save(p, s); print("OK D3D9on11Dev.cpp")

p, s = load("KRepresentShell3.cpp"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if 'Rep3Ini("Rep3Latency", 0)' not in s:
    s = rep(s, "int  g_nRep3Latency   = 3;\t// [D3D11 08/09 k] so khung toi da cho trinh chieu", "int  g_nRep3Latency   = 0;\t// [D3D11 08/09 o] 0 = khong dong SetMaximumFrameLatency (nhu ban f: Present tu chan khi hang day)")
    s = rep(s, '\tg_nRep3Latency   = Rep3Ini("Rep3Latency", 3);\t// [D3D11 08/09 k]\r\n', '\tg_nRep3Latency   = Rep3Ini("Rep3Latency", 0);\t// [D3D11 08/09 o]\r\n')
    s = rep(s, "int  g_nRep3Waitable  = 1;\t// [D3D11 08/09 n] doi tuong cho khung DXGI", "int  g_nRep3Waitable  = 0;\t// [D3D11 08/09 o] 0 = khong dung doi tuong cho (ban n giat)")
    s = rep(s, '\tg_nRep3Waitable  = Rep3Ini("Rep3Waitable", 1);\t// [D3D11 08/09 n]\r\n', '\tg_nRep3Waitable  = Rep3Ini("Rep3Waitable", 0);\t// [D3D11 08/09 o]\r\n')
    s = rep(s, "int  g_nRep3Buffers   = 3;\t// [D3D11 08/09 m] so backbuffer flip", "int  g_nRep3Buffers   = 2;\t// [D3D11 08/09 o] 2 nhu ban f")
    s = rep(s, '\tg_nRep3Buffers   = Rep3Ini("Rep3Buffers", 3);\t// [D3D11 08/09 m]\r\n', '\tg_nRep3Buffers   = Rep3Ini("Rep3Buffers", 2);\t// [D3D11 08/09 o]\r\n')
save(p, s, h0); print("OK KRepresentShell3.cpp")
print("XONG")
