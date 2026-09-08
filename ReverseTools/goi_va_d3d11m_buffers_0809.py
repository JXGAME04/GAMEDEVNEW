# -*- coding: ascii -*-
"""goi_va_d3d11m_buffers_0809.py - [D3D11 08/09 m] so backbuffer swapchain flip = [Client] Rep3Buffers (mac dinh 3, kep 2..4).
Do 12:49 trong game: 2 buffer + DO_NOT_WAIT -> bo dung 31 khung/s (DWM giu buffer 2 vsync -> 1 khung / 2 vsync); harness khong tai hien.
3 buffer: app luon co buffer trong de ve khung ke tiep trong khi DWM con giu khung truoc."""
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
if "g_nRep3Buffers" not in s:
    s = rep(s, "extern int      g_nRep3NoWait;", "extern int      g_nRep3Buffers;      // [Client] Rep3Buffers: so backbuffer flip (2..4), mac dinh 3\nextern int      g_nRep3NoWait;")
save(p, s); print("OK D3D9on11.h")

p, s = load("D3D9on11Dev.cpp")
if "g_nRep3Buffers" not in s:
    s = rep(s, "\tsd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; sd.BufferCount = 2; sd.Scaling = DXGI_SCALING_STRETCH;\n",
        "\tsd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; sd.BufferCount = (g_nRep3Buffers < 2) ? 2 : ((g_nRep3Buffers > 4) ? 4 : g_nRep3Buffers); sd.Scaling = DXGI_SCALING_STRETCH;\t// [m]\n")
    s = rep(s, "| do tre trinh chieu %d khung, khong cho %d | gop lenh %d\", m_pAtlas ?", "| %d buffer, do tre trinh chieu %d khung, khong cho %d | gop lenh %d\", m_pAtlas ?")
    s = rep(s, "\"tat\", g_nRep3Latency, g_nRep3NoWait, g_nRep3Batch);", "\"tat\", (int)sd_BufferCount_log(), g_nRep3Latency, g_nRep3NoWait, g_nRep3Batch);")
    # ham nho tra so buffer da dung (doc lai tu swapchain)
    s = rep(s, "bool CDev11::CreateSwapChain(UINT w, UINT h, bool bWindowed)\n{\n",
        "static int s_nRep3BuffersUsed = 0;\nstatic int sd_BufferCount_log() { return s_nRep3BuffersUsed; }\n\nbool CDev11::CreateSwapChain(UINT w, UINT h, bool bWindowed)\n{\n")
    s = rep(s, "\tHRESULT hr = m_pFactory->CreateSwapChainForHwnd(m_pDev, m_hWnd, &sd, NULL, NULL, &m_pSwap);\n\tif (FAILED(hr))\n\t{\n\t\t// may cu: thu FLIP_SEQUENTIAL roi DISCARD thuong\n",
        "\ts_nRep3BuffersUsed = (int)sd.BufferCount;\n\tHRESULT hr = m_pFactory->CreateSwapChainForHwnd(m_pDev, m_hWnd, &sd, NULL, NULL, &m_pSwap);\n\tif (FAILED(hr))\n\t{\n\t\t// may cu: thu FLIP_SEQUENTIAL roi DISCARD thuong\n")
save(p, s); print("OK D3D9on11Dev.cpp")

p, s = load("KRepresentShell3.cpp"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3Buffers" not in s:
    s = rep(s, "int  g_nRep3NoWait    = 0;", "int  g_nRep3Buffers   = 3;\t// [D3D11 08/09 m] so backbuffer flip\r\nint  g_nRep3NoWait    = 0;")
    s = rep(s, '\tg_nRep3NoWait    = Rep3Ini("Rep3NoWait", 0);\t// [D3D11 08/09 l]\r\n', '\tg_nRep3NoWait    = Rep3Ini("Rep3NoWait", 0);\t// [D3D11 08/09 l]\r\n\tg_nRep3Buffers   = Rep3Ini("Rep3Buffers", 3);\t// [D3D11 08/09 m]\r\n')
save(p, s, h0); print("OK KRepresentShell3.cpp")

p, s = load("BaseInclude.h"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3Buffers" not in s:
    s = rep(s, "extern int  g_nRep3NoWait;\t// [D3D11 08/09 l]", "extern int  g_nRep3Buffers;\t// [D3D11 08/09 m]\r\nextern int  g_nRep3NoWait;\t// [D3D11 08/09 l]")
save(p, s, h0); print("OK BaseInclude.h")
print("XONG")
