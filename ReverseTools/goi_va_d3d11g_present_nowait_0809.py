# -*- coding: ascii -*-
"""goi_va_d3d11g_present_nowait_0809.py - [D3D11 08/09 g] Present khong chan: flip model + Present(0) khong ALLOW_TEARING bi DXGI
xep hang toi 3 khung roi CHAN (do 12:19: present TB 6-16 ms, fps ghim 60, luong game doi trong Present). Nay:
 - IDXGIDevice1::SetMaximumFrameLatency(1) (toi da 1 khung cho),
 - Present(0, DXGI_PRESENT_DO_NOT_WAIT): hang day -> DXGI_ERROR_WAS_STILL_DRAWING -> BO khung nay (khong ve lai, khong cho), dem 'bo'.
 => nhu D3D9 cua so: khong chan, khong xe hinh, hien thi 60 Hz, logic 63 fps."""
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
if "g_uRep3PresentSkip" not in s:
    s = rep(s, "extern unsigned g_uRep3Presents;     // so lan Present\n", "extern unsigned g_uRep3Presents;     // so lan Present\nextern unsigned g_uRep3PresentSkip;  // so khung bo vi hang trinh chieu day (DO_NOT_WAIT)\n")
save(p, s); print("OK D3D9on11.h")

p, s = load("D3D9on11Dev.cpp")
if "g_uRep3PresentSkip" not in s:
    s = rep(s, "unsigned g_uRep3Presents = 0;\n", "unsigned g_uRep3Presents = 0;\nunsigned g_uRep3PresentSkip = 0;\n")
    # sau khi tao swapchain flip: do tre toi da 1 khung
    s = rep(s, "\tm_pFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);\n",
        "\tm_pFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);\n"
        "\t{\t// [D3D11 08/09 g] toi da 1 khung cho trinh chieu -> Present(DO_NOT_WAIT) bo khung thua thay vi chan\n"
        "\t\tIDXGIDevice1* pDev1 = NULL;\n"
        "\t\tif (SUCCEEDED(m_pDev->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pDev1)) && pDev1) { pDev1->SetMaximumFrameLatency(1); pDev1->Release(); }\n"
        "\t}\n")
    s = rep(s, "\tif (interval == 0 && m_bTearing && !bFull) flags |= DXGI_PRESENT_ALLOW_TEARING;\n\tHRESULT hr = m_pSwap->Present(interval, flags);\n",
        "\tif (interval == 0 && m_bTearing && !bFull) flags |= DXGI_PRESENT_ALLOW_TEARING;\n"
        "\tif (interval == 0 && g_nRep3Flip) flags |= DXGI_PRESENT_DO_NOT_WAIT;\t// [D3D11 08/09 g] hang day -> bo khung, khong chan\n"
        "\tHRESULT hr = m_pSwap->Present(interval, flags);\n"
        "\tif (hr == DXGI_ERROR_WAS_STILL_DRAWING) { g_uRep3PresentSkip++; hr = S_OK; }\n")
save(p, s); print("OK D3D9on11Dev.cpp")

p, s = load("KRepresentShell3.cpp"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_uRep3PresentSkip" not in s:
    s = rep(s, "| d3d11: present TB %.2f ms, ve %u lenh %.1f us/lenh\",", "| d3d11: present TB %.2f ms bo %u, ve %u lenh %.1f us/lenh\",")
    s = rep(s, "g_uRep3Presents ? g_dRep3PresentMs / g_uRep3Presents : 0.0, g_uRep3Draws,", "g_uRep3Presents ? g_dRep3PresentMs / g_uRep3Presents : 0.0, g_uRep3PresentSkip, g_uRep3Draws,")
    s = rep(s, "g_dRep3PresentMs = 0.0; g_uRep3Presents = 0; g_dRep3DrawMs = 0.0; g_uRep3Draws = 0;", "g_dRep3PresentMs = 0.0; g_uRep3Presents = 0; g_uRep3PresentSkip = 0; g_dRep3DrawMs = 0.0; g_uRep3Draws = 0;")
save(p, s, h0); print("OK KRepresentShell3.cpp")

p, s = load("BaseInclude.h"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_uRep3PresentSkip" not in s:
    s = rep(s, "extern double g_dRep3PresentMs; extern unsigned g_uRep3Presents;", "extern double g_dRep3PresentMs; extern unsigned g_uRep3Presents; extern unsigned g_uRep3PresentSkip;")
save(p, s, h0); print("OK BaseInclude.h")
print("XONG")
