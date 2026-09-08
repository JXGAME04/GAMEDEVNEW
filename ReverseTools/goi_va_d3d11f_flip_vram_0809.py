# -*- coding: ascii -*-
"""goi_va_d3d11f_flip_vram_0809.py - [D3D11 08/09 f]
 - "man hinh gon song khi di chuyen" = xe hinh (tearing): swapchain bitblt DISCARD + Present(0) o che do cua so co the bi DWM ghep
   giua chung. Chuyen mac dinh sang FLIP model, KHONG co ALLOW_TEARING (Rep3Tearing=0 mac dinh), interval 0: DWM chi ghep khung
   tron ven, Present khong chan.
 - Ghi VRAM that: dung/ngan sach cua tien trinh (QueryVideoMemoryInfo) vao dong [REP3] thay 'VRAM con 4095'."""
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

# --- D3D9on11.h: extern
p, s = load("D3D9on11.h")
if "g_nRep3Tearing" not in s:
    s = rep(s, "extern int      g_nRep3Flip;", "extern int      g_nRep3Tearing;      // [Client] Rep3Tearing: 1 = cho phep xe hinh (ALLOW_TEARING) khi flip + vsync 0; mac dinh 0\nvoid Rep3_D3D11VramInfo(unsigned* puUsedMB, unsigned* puBudgetMB);   // VRAM tien trinh (0/0 neu khong phai D3D11)\nextern int      g_nRep3Flip;")
save(p, s); print("OK D3D9on11.h")

# --- D3D9on11Dev.cpp
p, s = load("D3D9on11Dev.cpp")
if "g_pRep3Dev11" not in s:
    s = rep(s, "double   g_dRep3PresentMs = 0.0;\n", "static CDev11* g_pRep3Dev11 = NULL;\ndouble   g_dRep3PresentMs = 0.0;\n")
    # tearing chi khi Rep3Tearing=1
    s = rep(s, "\t\tif (SUCCEEDED(pF5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &bAllow, sizeof(bAllow)))) m_bTearing = bAllow != FALSE;\n",
        "\t\tif (SUCCEEDED(pF5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &bAllow, sizeof(bAllow)))) m_bTearing = (bAllow != FALSE) && (g_nRep3Tearing != 0);\n")
    s = rep(s, "\tif (!CreateSwapChain(m_bbW, m_bbH, m_pp.Windowed != FALSE)) return false;\n", "\tg_pRep3Dev11 = this;\n\tif (!CreateSwapChain(m_bbW, m_bbH, m_pp.Windowed != FALSE)) return false;\n")
    s = rep(s, "\tReleaseSwapBuffers();\n\tif (m_pSwap) { m_pSwap->SetFullscreenState(FALSE, NULL); m_pSwap->Release(); m_pSwap = NULL; }\n\tR11_SAFE_RELEASE(m_pAdapter3);",
        "\tReleaseSwapBuffers();\n\tif (m_pSwap) { m_pSwap->SetFullscreenState(FALSE, NULL); m_pSwap->Release(); m_pSwap = NULL; }\n\tif (g_pRep3Dev11 == this) g_pRep3Dev11 = NULL;\n\tR11_SAFE_RELEASE(m_pAdapter3);")
    s = rep(s, "HRESULT CDev11::GetDirect3D(IDirect3D9** ppD3D9)",
        "void Rep3_D3D11VramInfo(unsigned* puUsedMB, unsigned* puBudgetMB)\n{\n"
        "\tif (puUsedMB) *puUsedMB = 0; if (puBudgetMB) *puBudgetMB = 0;\n"
        "\tif (!g_pRep3Dev11 || !g_pRep3Dev11->m_pAdapter3) return;\n"
        "\tDXGI_QUERY_VIDEO_MEMORY_INFO mi;\n"
        "\tif (SUCCEEDED(g_pRep3Dev11->m_pAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &mi)))\n"
        "\t{ if (puUsedMB) *puUsedMB = (unsigned)(mi.CurrentUsage >> 20); if (puBudgetMB) *puBudgetMB = (unsigned)(mi.Budget >> 20); }\n"
        "}\n\n"
        "HRESULT CDev11::GetDirect3D(IDirect3D9** ppD3D9)")
save(p, s); print("OK D3D9on11Dev.cpp")

# --- KRepresentShell3.cpp: Rep3Flip mac dinh 1, Rep3Tearing ini, in VRAM that
p, s = load("KRepresentShell3.cpp"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3Tearing" not in s:
    s = rep(s, "int  g_nRep3Flip      = 0;\t// [D3D11 08/09 b] 0 = swapchain bitblt (Present nhanh nhu D3D9 o che do cua so, mac dinh), 1 = flip model\r\n",
        "int  g_nRep3Flip      = 1;\t// [D3D11 08/09 f] 1 = flip model (DWM ghep khung tron ven, khong xe hinh; mac dinh), 0 = bitblt cu\r\nint  g_nRep3Tearing   = 0;\t// [D3D11 08/09 f] 1 = ALLOW_TEARING khi flip + vsync 0 (xe hinh, do tre thap nhat)\r\n")
    s = rep(s, '\tg_nRep3Flip      = Rep3Ini("Rep3Flip", 0);\t// [D3D11 08/09 b] do harness: flip +0,6 ms/khung Present khi cua so hien\r\n',
        '\tg_nRep3Flip      = Rep3Ini("Rep3Flip", 1);\t// [D3D11 08/09 f] bitblt DISCARD bi DWM ghep giua chung -> "gon song" khi di chuyen\r\n\tg_nRep3Tearing   = Rep3Ini("Rep3Tearing", 0);\t// [D3D11 08/09 f]\r\n')
    s = rep(s, "| gpu tex %u (%u MB, %u trang %u MB) | d3d11:", "| gpu tex %u (%u MB, %u trang %u MB) | vram %u/%u MB | d3d11:")
    s = rep(s, "g_uRep3GpuTexCount, (unsigned)(g_uRep3GpuTexBytes >> 20), g_uRep3AtlasPages, (unsigned)(g_uRep3AtlasBytes >> 20),\r\n",
        "g_uRep3GpuTexCount, (unsigned)(g_uRep3GpuTexBytes >> 20), g_uRep3AtlasPages, (unsigned)(g_uRep3AtlasBytes >> 20), uVramUsed, uVramBudget,\r\n")
    s = rep(s, "\t\t\tuint32 uNodes = 0, uTexMB = 0, uRawMB = 0, uDrawMB = 0, uBudgetMB = 0;\r\n",
        "\t\t\tuint32 uNodes = 0, uTexMB = 0, uRawMB = 0, uDrawMB = 0, uBudgetMB = 0;\r\n\t\t\tunsigned uVramUsed = 0, uVramBudget = 0; Rep3_D3D11VramInfo(&uVramUsed, &uVramBudget);\t// [D3D11 08/09 f]\r\n")
save(p, s, h0); print("OK KRepresentShell3.cpp")

# --- BaseInclude.h: extern + prototype
p, s = load("BaseInclude.h"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3Tearing" not in s:
    s = rep(s, "extern int  g_nRep3Flip;", "extern int  g_nRep3Tearing;     // [D3D11 08/09 f]\r\nvoid Rep3_D3D11VramInfo(unsigned* puUsedMB, unsigned* puBudgetMB);\r\nextern int  g_nRep3Flip;")
save(p, s, h0); print("OK BaseInclude.h")
print("XONG")
