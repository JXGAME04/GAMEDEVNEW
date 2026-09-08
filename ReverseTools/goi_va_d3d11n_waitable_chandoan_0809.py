# -*- coding: ascii -*-
"""goi_va_d3d11n_waitable_chandoan_0809.py - [D3D11 08/09 n]
 (1) Doi tuong cho khung (DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT, Rep3Waitable=1 mac dinh): dau moi khung cho toi khi
     hang trinh chieu con cho (toi da Rep3Latency khung) -> Present luon co buffer, khong bao gio WAS_STILL_DRAWING; cho = nhu D3D9 day hang.
 (2) Chan doan: 12 lan dau Present tra WAS_STILL_DRAWING/OCCLUDED ghi log kem ms tu Present truoc; luc tao swapchain ghi style cua so,
     so cua so con, ex-style (GDI/child window co the pha flip model)."""
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
if "m_hWaitable" not in s:
    s = rep(s, "\tbool            m_bTearing;\n\tUINT            m_swapFlags;\n", "\tbool            m_bTearing;\n\tUINT            m_swapFlags;\n\tHANDLE          m_hWaitable;			// [n] doi tuong cho khung (NULL = khong dung)\n\tunsigned        m_uStillLogged;\n\tLARGE_INTEGER   m_liLastPresent;\n")
save(p, s); print("OK D3D9on11i.h")

p, s = load("D3D9on11.h")
if "g_nRep3Waitable" not in s:
    s = rep(s, "extern int      g_nRep3Buffers;", "extern int      g_nRep3Waitable;     // [Client] Rep3Waitable: 1 = dung doi tuong cho khung cua DXGI (mac dinh), 0 = tat\nextern int      g_nRep3Buffers;")
save(p, s); print("OK D3D9on11.h")

p, s = load("D3D9on11Dev.cpp")
if "m_hWaitable" not in s:
    s = rep(s, "\tm_bTearing = false; m_swapFlags = 0;\n", "\tm_bTearing = false; m_swapFlags = 0; m_hWaitable = NULL; m_uStillLogged = 0; m_liLastPresent.QuadPart = 0;\n")
    # dtor: dong handle
    s = rep(s, "\tReleaseSwapBuffers();\n\tif (m_pSwap) { m_pSwap->SetFullscreenState(FALSE, NULL); m_pSwap->Release(); m_pSwap = NULL; }\n\tif (g_pRep3Dev11 == this) g_pRep3Dev11 = NULL;\n",
        "\tReleaseSwapBuffers();\n\tif (m_hWaitable) { CloseHandle(m_hWaitable); m_hWaitable = NULL; }\n\tif (m_pSwap) { m_pSwap->SetFullscreenState(FALSE, NULL); m_pSwap->Release(); m_pSwap = NULL; }\n\tif (g_pRep3Dev11 == this) g_pRep3Dev11 = NULL;\n")
    # CreateSwapChain: dong handle cu, co flip + waitable
    s = rep(s, "\tReleaseSwapBuffers();\n\tif (m_pSwap) { m_pSwap->SetFullscreenState(FALSE, NULL); m_pSwap->Release(); m_pSwap = NULL; }\n\tif (w == 0) w = 1; if (h == 0) h = 1;\n",
        "\tReleaseSwapBuffers();\n\tif (m_hWaitable) { CloseHandle(m_hWaitable); m_hWaitable = NULL; }\n\tif (m_pSwap) { m_pSwap->SetFullscreenState(FALSE, NULL); m_pSwap->Release(); m_pSwap = NULL; }\n\tif (w == 0) w = 1; if (h == 0) h = 1;\n")
    s = rep(s, "\tm_swapFlags = m_bTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;\n", "\tm_swapFlags = m_bTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;\n\tif (g_nRep3Flip && g_nRep3Waitable) m_swapFlags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;\t// [n]\n")
    # sau khi tao swapchain: lay handle + SetMaximumFrameLatency tren swapchain; ghi chan doan cua so
    s = rep(s, "\tm_pFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);\n",
        "\tm_pFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);\n"
        "\tif (m_swapFlags & DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT)\n\t{\t// [n]\n"
        "\t\tIDXGISwapChain2* pSc2 = NULL;\n"
        "\t\tif (SUCCEEDED(m_pSwap->QueryInterface(__uuidof(IDXGISwapChain2), (void**)&pSc2)) && pSc2)\n"
        "\t\t{\n\t\t\tpSc2->SetMaximumFrameLatency(g_nRep3Latency < 1 ? 1 : (g_nRep3Latency > 16 ? 16 : g_nRep3Latency));\n"
        "\t\t\tm_hWaitable = pSc2->GetFrameLatencyWaitableObject();\n\t\t\tpSc2->Release();\n\t\t}\n"
        "\t}\n"
        "\t{\t// [n] chan doan cua so\n"
        "\t\tstruct R11Cnt { static BOOL CALLBACK Enum(HWND, LPARAM lp) { (*(int*)lp)++; return TRUE; } };\n"
        "\t\tint nChild = 0; EnumChildWindows(m_hWnd, R11Cnt::Enum, (LPARAM)&nChild);\n"
        "\t\tRECT rcC = { 0, 0, 0, 0 }; GetClientRect(m_hWnd, &rcC);\n"
        "\t\tR11Log(\"cua so: style 0x%08X exstyle 0x%08X con %d client %dx%d | swapchain %ux%u flags 0x%X waitable=%d\", (unsigned)GetWindowLongA(m_hWnd, GWL_STYLE), (unsigned)GetWindowLongA(m_hWnd, GWL_EXSTYLE), nChild, (int)(rcC.right - rcC.left), (int)(rcC.bottom - rcC.top), w, h, (unsigned)m_swapFlags, m_hWaitable ? 1 : 0);\n"
        "\t}\n")
    # dau khung: cho doi tuong (truoc khi gan render target lan dau sau Present)
    s = rep(s, "void CDev11::BindRenderTarget()\n{\n\tif (m_bRtBound) return;\n",
        "void CDev11::BindRenderTarget()\n{\n\tif (m_bRtBound) return;\n\tif (m_hWaitable) WaitForSingleObjectEx(m_hWaitable, 1000, TRUE);\t// [n] cho toi khi hang trinh chieu con cho (nhu D3D9 day hang)\n")
    # Present: chan doan
    s = rep(s, "\tHRESULT hr = m_pSwap->Present(interval, flags);\n\tif (hr == DXGI_ERROR_WAS_STILL_DRAWING) { g_uRep3PresentSkip++; hr = S_OK; }\n",
        "\tHRESULT hr = m_pSwap->Present(interval, flags);\n"
        "\tif (hr != S_OK && m_uStillLogged < 12)\n\t{\t// [n] chan doan: ma tra ve + ms tu Present truoc\n"
        "\t\tLARGE_INTEGER tq; QueryPerformanceCounter(&tq);\n"
        "\t\tR11Log(\"Present tra 0x%08X, %.2f ms sau Present truoc, flags 0x%X\", (unsigned)hr, m_liLastPresent.QuadPart ? R11Ms(m_liLastPresent, tq) : 0.0, (unsigned)flags);\n"
        "\t\tm_uStillLogged++;\n\t}\n"
        "\tQueryPerformanceCounter(&m_liLastPresent);\n"
        "\tif (hr == DXGI_ERROR_WAS_STILL_DRAWING) { g_uRep3PresentSkip++; hr = S_OK; }\n")
    # ResizeBuffers phai giu co
    s = rep(s, "\thr = m_pSwap->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, m_swapFlags);\n\tif (FAILED(hr)) { R11Log(\"Reset: ResizeBuffers %ux%u that bai 0x%08X\", w, h, (unsigned)hr); Unlock(); return D3DERR_INVALIDCALL; }\n",
        "\thr = m_pSwap->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, m_swapFlags);\n\tif (FAILED(hr)) { R11Log(\"Reset: ResizeBuffers %ux%u that bai 0x%08X\", w, h, (unsigned)hr); Unlock(); return D3DERR_INVALIDCALL; }\n"
        "\tif (m_hWaitable) WaitForSingleObjectEx(m_hWaitable, 1000, TRUE);\t// [n] sau resize, cho lan dau\n")
save(p, s); print("OK D3D9on11Dev.cpp")

p, s = load("KRepresentShell3.cpp"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3Waitable" not in s:
    s = rep(s, "int  g_nRep3Buffers   = 3;", "int  g_nRep3Waitable  = 1;\t// [D3D11 08/09 n] doi tuong cho khung DXGI\r\nint  g_nRep3Buffers   = 3;")
    s = rep(s, '\tg_nRep3Buffers   = Rep3Ini("Rep3Buffers", 3);\t// [D3D11 08/09 m]\r\n', '\tg_nRep3Buffers   = Rep3Ini("Rep3Buffers", 3);\t// [D3D11 08/09 m]\r\n\tg_nRep3Waitable  = Rep3Ini("Rep3Waitable", 1);\t// [D3D11 08/09 n]\r\n')
save(p, s, h0); print("OK KRepresentShell3.cpp")

p, s = load("BaseInclude.h"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3Waitable" not in s:
    s = rep(s, "extern int  g_nRep3Buffers;\t// [D3D11 08/09 m]", "extern int  g_nRep3Waitable;\t// [D3D11 08/09 n]\r\nextern int  g_nRep3Buffers;\t// [D3D11 08/09 m]")
save(p, s, h0); print("OK BaseInclude.h")
print("XONG")
