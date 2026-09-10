# -*- coding: utf-8 -*-
"""goi_va_mang_f_0909.py - [MANG 09/09 f] atlas theo KHOI co dinh (R8G8: 32 lop = 64 MB, BGRA8: 4 lop = 16 MB, toi da 16 khoi/dinh dang)
thay mot mang lon dan x1,5; khoi rong tra VRAM (giu 1 khoi rong/dinh dang); ring dinh 16 -> 12 MB.

So do 19:4x: VRAM dinh 574 MB cap cho 384 MB anh; sau tran anh con 183 MB ma VRAM van 608 MB (mang khong co); mang chu 8 lop 32 MB
cho 3 texture; ring 16 MB -> driver giu vai ban ~30-40 MB RAM (khung nang nhat can 8,9 MB).
Dinh: truong 9 bit 'lop' = khoi (4 bit) << 5 | lop trong khoi (5 bit). Shader: 16 Texture2DArray moi dinh dang (t3..t18 R8G8,
t19..t34 BGRA8), chon khoi bang switch tren gia tri theo dinh; SampleLevel(..., 0) (texture 1 mip -> y het Sample).
Khong chep mang khi lon, khong cap du, khong doi diem anh. Lui: Rep3AtlasMang=0.
"""
import io
import os
import subprocess
import sys

D = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/"
TAG = "[MANG 09/09 f]"
FXC = r"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\fxc.exe"
T = "\t"
N_KHOI = 16


def doc(p):
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    return s, sum(1 for c in s if ord(c) >= 0x80), s.count("\n") - s.count("\r\n"), s.count("\r\n")


def ghi(p, s, h0, lf0, crlf0, ten):
    ok = (s.count("\n") - s.count("\r\n") == lf0) if crlf0 else (s.count("\r\n") == 0)
    if not ok or sum(1 for c in s if ord(c) >= 0x80) != h0 or "\ufffd" in s:
        print("FAIL ma hoa " + ten); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("OK " + ten)


def rep(s, old, new, ten, n_mong=1):
    n = s.count(old)
    if n != n_mong:
        print("FAIL neo %s: %d (mong %d)" % (ten, n, n_mong)); sys.exit(1)
    return s.replace(old, new)


def rep_khoang(s, dau, cuoi, new, ten):
    if s.count(dau) != 1:
        print("FAIL neo %s: dau %d" % (ten, s.count(dau))); sys.exit(1)
    a = s.find(dau); b = s.find(cuoi, a + len(dau))
    if b < 0:
        print("FAIL neo %s: khong thay cuoi" % ten); sys.exit(1)
    return s[:a] + new + s[b + len(cuoi):]


# ====================================================================== 1. HLSL
P = D + "Rep3Shaders11.hlsl"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    dau = "#ifdef MANG" + NL + "Texture2DArray g_a8  : register(t3);"
    cuoi = "    return g_t1.Sample(g_s1, float3(uv, 0.0));" + NL + "}" + NL + "#endif" + NL
    L = ["#ifdef MANG",
         "// " + TAG + " atlas theo KHOI: 16 mang/dinh dang, chon khoi theo dinh (4 bit cao cua truong lop), lop trong khoi = 5 bit thap"]
    for k in range(N_KHOI): L.append("Texture2DArray g_a8_%d : register(t%d);" % (k, 3 + k))
    for k in range(N_KHOI): L.append("Texture2DArray g_a32_%d : register(t%d);" % (k, 3 + N_KHOI + k))
    for ten, pre in [("A8", "g_a8_"), ("A32", "g_a32_")]:
        L.append("float4 %sSample(uint k, float3 p)" % ten); L.append("{"); L.append("    switch (k)"); L.append("    {")
        for k in range(N_KHOI): L.append("    case %d: return %s%d.SampleLevel(g_s0, p, 0);" % (k, pre, k))
        L.append("    default: return float4(0, 0, 0, 0);"); L.append("    }"); L.append("}")
        L.append("float4 %sSample1(uint k, float3 p)" % ten); L.append("{"); L.append("    switch (k)"); L.append("    {")
        for k in range(N_KHOI): L.append("    case %d: return %s%d.SampleLevel(g_s1, p, 0);" % (k, pre, k))
        L.append("    default: return float4(0, 0, 0, 0);"); L.append("    }"); L.append("}")
        L.append("float4 %sLoad(uint k, int4 p)" % ten); L.append("{"); L.append("    switch (k)"); L.append("    {")
        for k in range(N_KHOI): L.append("    case %d: return %s%d.Load(p);" % (k, pre, k))
        L.append("    default: return float4(0, 0, 0, 0);"); L.append("    }"); L.append("}")
        L.append("float2 %sDim(uint k)" % ten); L.append("{"); L.append("    float3 d = float3(1, 1, 1);"); L.append("    switch (k)"); L.append("    {")
        for k in range(N_KHOI): L.append("    case %d: %s%d.GetDimensions(d.x, d.y, d.z); break;" % (k, pre, k))
        L.append("    default: break;"); L.append("    }"); L.append("    return d.xy;"); L.append("}")
    L += [
        "// nguon texture stage 0 theo dinh: 0 = t0 (texture rieng, view mang 1 lop), 1 = khoi R8G8, 2 = khoi BGRA8; lop = khoi<<5 | lop",
        "float4 T0Sample(uint src, uint lop, float2 uv)",
        "{",
        "    float3 p = float3(uv, (float)(lop & 31u));",
        "    if (src == 1u) return A8Sample((lop >> 5) & 15u, p);",
        "    if (src == 2u) return A32Sample((lop >> 5) & 15u, p);",
        "    return g_t0.SampleLevel(g_s0, float3(uv, 0.0), 0);",
        "}",
        "float4 T0Load(uint src, uint lop, int2 p)",
        "{",
        "    int4 q = int4(p, (int)(lop & 31u), 0);",
        "    if (src == 1u) return A8Load((lop >> 5) & 15u, q);",
        "    if (src == 2u) return A32Load((lop >> 5) & 15u, q);",
        "    return g_t0.Load(int4(p, 0, 0));",
        "}",
        "float2 T0Dim(uint src, uint lop)",
        "{",
        "    if (src == 1u) return A8Dim((lop >> 5) & 15u);",
        "    if (src == 2u) return A32Dim((lop >> 5) & 15u);",
        "    float3 d; g_t0.GetDimensions(d.x, d.y, d.z); return d.xy;",
        "}",
        "float4 T1Sample(float2 uv)",
        "{   // stage 1: nguon = g_at.x, lop (khoi<<5|lop) = g_at.y (cb, hiem doi)",
        "    uint src = (uint)g_at.x; uint lop = (uint)g_at.y;",
        "    float3 p = float3(uv, (float)(lop & 31u));",
        "    if (src == 1u) return A8Sample1((lop >> 5) & 15u, p);",
        "    if (src == 2u) return A32Sample1((lop >> 5) & 15u, p);",
        "    return g_t1.SampleLevel(g_s1, float3(uv, 0.0), 0);",
        "}",
        "#endif", ""]
    s = rep_khoang(s, dau, cuoi, NL.join(L), "hlsl khoi")
    s = rep(s, "#define T0_DIM(d)      d = T0Dim(src)", "#define T0_DIM(d)      d = T0Dim(src, lop)", "hlsl macro dim")
    s = rep(s, "    float lop = (float)((i.palrow.x >> 16) & 0x1FFu); uint src = (i.palrow.x >> 25) & 3u;   // [MANG 09/09 b] lop + nguon theo dinh",
            "    uint lop = (i.palrow.x >> 16) & 0x1FFu; uint src = (i.palrow.x >> 25) & 3u;   // [MANG 09/09 b] lop (khoi<<5|lop, " + TAG + ") + nguon theo dinh", "hlsl PS dau")
    ghi(P, s, h0, lf0, crlf0, "Rep3Shaders11.hlsl")
else:
    print("Rep3Shaders11.hlsl da co")

if not os.path.exists(FXC):
    print("FAIL khong thay fxc"); sys.exit(1)
for args, ten in [(["/T", "ps_4_0", "/E", "PS", "/Fh", "Rep3Shaders11_ps.h", "/Vn", "g_Rep3PS11", "Rep3Shaders11.hlsl"], "PS cu"),
                  (["/T", "ps_4_0", "/E", "PS", "/D", "MANG=1", "/Fh", "Rep3Shaders11_psmang.h", "/Vn", "g_Rep3PS11Mang", "Rep3Shaders11.hlsl"], "PS MANG")]:
    r = subprocess.run([FXC, "/nologo"] + args, cwd=D, capture_output=True, text=True)
    if r.returncode != 0:
        print("FAIL fxc %s:\n%s\n%s" % (ten, r.stdout, r.stderr)); sys.exit(1)
    print("OK fxc " + ten)

# ====================================================================== 2. D3D9on11i.h
P = D + "D3D9on11i.h"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, T + "UINT m_nLop, m_nDung; std::vector<UINT> m_lopTrong;" + T + "// so lop da cap, so lop da phat (ke tiep), lop da tra lai",
            T + "UINT m_nLop, m_nDung; std::vector<UINT> m_lopTrong;" + T + "// so lop cua khoi, so lop da phat (ke tiep), lop da tra lai" + NL +
            T + "UINT m_nKhoi, m_nSuDung;" + T + "// " + TAG + " chi so khoi trong dinh dang (0..15, = khe t3+k / t19+k), so lop dang dung", "H khoi")
    s = rep(s, T + "void GanMang();" + T + "// [MANG 09/09 b] gan mang R8G8 -> t3, BGRA8 -> t4" + NL,
            T + "void GanMang();" + T + "// [MANG 09/09 b] gan khoi R8G8 -> t3..t18, BGRA8 -> t19..t34 (" + TAG + ")" + NL +
            T + "void KhoiXoaNeuTrong(CAtlasMang* pM);" + T + "// " + TAG + " khoi khong con lop dung -> tra VRAM (giu 1 khoi rong / dinh dang)" + NL, "H KhoiXoa")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11i.h")
else:
    print("D3D9on11i.h da co")

# ====================================================================== 3. D3D9on11Atlas.cpp
P = D + "D3D9on11Atlas.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    dau = "// [MANG 09/09] lay mot lop trong cua mang theo dinh dang; het lop -> mang lon hon (+8 lop): chep cac lop cu bang GPU, doi SRV" + NL
    cuoi = T + "m_pDev->m_pCtx->PSSetShaderResources(3, 2, v);" + NL + "}" + NL
    new = NL.join([
        "// " + TAG + " atlas theo KHOI co dinh: R8G8 32 lop (64 MB), BGRA8 4 lop (16 MB), toi da 16 khoi moi dinh dang (khe t3+k / t19+k).",
        "// Khong chep mang khi lon, khong cap du; khoi khong con lop dung -> tra VRAM (giu lai 1 khoi rong moi dinh dang de khoi cap/tra lien tuc).",
        "#define R11_KHOI_MAX 16",
        "static UINT R11KhoiLop(DXGI_FORMAT fmt) { return (fmt == DXGI_FORMAT_R8G8_UNORM) ? 32u : 4u; }",
        "CAtlasMang* CAtlasMgr::MangLay(DXGI_FORMAT fmt, UINT* pLop)",
        "{",
        T + "for (size_t i = 0; i < m_mang.size(); i++)",
        T + "{",
        T*2 + "CAtlasMang* pM = m_mang[i];",
        T*2 + "if (pM->m_fmt != fmt) continue;",
        T*2 + "if (!pM->m_lopTrong.empty()) { *pLop = pM->m_lopTrong.back(); pM->m_lopTrong.pop_back(); return pM; }",
        T*2 + "if (pM->m_nDung < pM->m_nLop) { *pLop = pM->m_nDung++; return pM; }",
        T + "}",
        T + "// het cho: tao khoi moi voi chi so nho nhat chua dung",
        T + "bool aDung[R11_KHOI_MAX]; memset(aDung, 0, sizeof(aDung));",
        T + "for (size_t i = 0; i < m_mang.size(); i++) if (m_mang[i]->m_fmt == fmt && m_mang[i]->m_nKhoi < R11_KHOI_MAX) aDung[m_mang[i]->m_nKhoi] = true;",
        T + "UINT nKhoi = R11_KHOI_MAX; for (UINT k = 0; k < R11_KHOI_MAX; k++) if (!aDung[k]) { nKhoi = k; break; }",
        T + "if (nKhoi >= R11_KHOI_MAX) { R11Log(\"atlas khoi %s: da du 16 khoi, texture moi dung texture rieng\", (fmt == DXGI_FORMAT_R8G8_UNORM) ? \"R8G8\" : \"BGRA8\"); return NULL; }",
        T + "const UINT bpp = (fmt == DXGI_FORMAT_R8G8_UNORM) ? 2 : 4, nLop = R11KhoiLop(fmt);",
        T + "D3D11_TEXTURE2D_DESC td; memset(&td, 0, sizeof(td));",
        T + "td.Width = m_pageSize; td.Height = m_pageSize; td.MipLevels = 1; td.ArraySize = nLop; td.Format = fmt; td.SampleDesc.Count = 1;",
        T + "td.Usage = D3D11_USAGE_DEFAULT; td.BindFlags = D3D11_BIND_SHADER_RESOURCE;",
        T + "ID3D11Texture2D* pTex = NULL;",
        T + "HRESULT hr = m_pDev->m_pDev->CreateTexture2D(&td, NULL, &pTex);" + T + "// khong kem du lieu: lop duoc xoa 0 luc NewPage",
        T + "if (FAILED(hr) || !pTex) { R11Log(\"atlas khoi: CreateTexture2D %u lop that bai 0x%08X\", nLop, (unsigned)hr); return NULL; }",
        T + "D3D11_SHADER_RESOURCE_VIEW_DESC vd; memset(&vd, 0, sizeof(vd));",
        T + "vd.Format = fmt; vd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY; vd.Texture2DArray.MostDetailedMip = 0; vd.Texture2DArray.MipLevels = 1; vd.Texture2DArray.FirstArraySlice = 0; vd.Texture2DArray.ArraySize = nLop;",
        T + "ID3D11ShaderResourceView* pSrv = NULL;",
        T + "hr = m_pDev->m_pDev->CreateShaderResourceView(pTex, &vd, &pSrv);",
        T + "if (FAILED(hr) || !pSrv) { pTex->Release(); R11Log(\"atlas khoi: SRV that bai 0x%08X\", (unsigned)hr); return NULL; }",
        T + "CAtlasMang* pM = new CAtlasMang(); pM->m_pTex = pTex; pM->m_pSrv = pSrv; pM->m_fmt = fmt; pM->m_bpp = bpp; pM->m_nLop = nLop; pM->m_nDung = 0; pM->m_nKhoi = nKhoi; pM->m_nSuDung = 0;",
        T + "m_mang.push_back(pM);",
        T + "m_pDev->FlushIfPending(); m_pDev->m_bAppliedValid = false; m_pDev->m_bPipeBound = false;" + T + "// gan lai bo khe t3..t34",
        T + "g_uRep3AtlasBytes = 0; for (size_t i = 0; i < m_mang.size(); i++) g_uRep3AtlasBytes += (unsigned __int64)m_mang[i]->m_nLop * m_pageSize * m_pageSize * m_mang[i]->m_bpp;",
        T + "R11Log(\"atlas khoi %s #%u: %u lop (%u MB), tong %u MB\", bpp == 2 ? \"R8G8\" : \"BGRA8\", nKhoi, nLop, (unsigned)(((unsigned __int64)nLop * m_pageSize * m_pageSize * bpp) >> 20), (unsigned)(g_uRep3AtlasBytes >> 20));",
        T + "*pLop = pM->m_nDung++;",
        T + "return pM;",
        "}",
        "",
        "void CAtlasMgr::KhoiXoaNeuTrong(CAtlasMang* pM)",
        "{",
        T + "if (!pM || pM->m_nSuDung != 0) return;",
        T + "int nRongKhac = 0;",
        T + "for (size_t i = 0; i < m_mang.size(); i++) if (m_mang[i] != pM && m_mang[i]->m_fmt == pM->m_fmt && m_mang[i]->m_nSuDung == 0) nRongKhac++;",
        T + "if (nRongKhac == 0) return;" + T + "// giu lai mot khoi rong moi dinh dang",
        T + "m_pDev->FlushIfPending();",
        T + "for (size_t i = 0; i < m_pages.size(); i++) if (m_pages[i]->m_pMang == pM) return;" + T + "// con trang tro vao (khong nen xay ra khi m_nSuDung == 0)",
        T + "for (size_t i = 0; i < m_mang.size(); i++) if (m_mang[i] == pM) { m_mang.erase(m_mang.begin() + i); break; }",
        T + "if (pM->m_pSrv) pM->m_pSrv->Release();",
        T + "if (pM->m_pTex) pM->m_pTex->Release();",
        T + "R11Log(\"atlas khoi %s #%u: tra VRAM (%u MB)\", pM->m_bpp == 2 ? \"R8G8\" : \"BGRA8\", pM->m_nKhoi, (unsigned)(((unsigned __int64)pM->m_nLop * m_pageSize * m_pageSize * pM->m_bpp) >> 20));",
        T + "delete pM;",
        T + "m_pDev->m_bAppliedValid = false; m_pDev->m_bPipeBound = false;",
        T + "g_uRep3AtlasBytes = 0; for (size_t i = 0; i < m_mang.size(); i++) g_uRep3AtlasBytes += (unsigned __int64)m_mang[i]->m_nLop * m_pageSize * m_pageSize * m_mang[i]->m_bpp;",
        "}",
        "",
        "// [MANG 09/09 b] gan cac khoi atlas co dinh: t3..t18 = R8G8 #0..15, t19..t34 = BGRA8 #0..15 (" + TAG + ")",
        "void CAtlasMgr::GanMang()",
        "{",
        T + "ID3D11ShaderResourceView* v[2 * R11_KHOI_MAX]; memset(v, 0, sizeof(v));",
        T + "for (size_t i = 0; i < m_mang.size(); i++)",
        T + "{",
        T*2 + "const UINT k = m_mang[i]->m_nKhoi; if (k >= R11_KHOI_MAX) continue;",
        T*2 + "v[(m_mang[i]->m_bpp == 2 ? 0 : R11_KHOI_MAX) + k] = m_mang[i]->m_pSrv;",
        T + "}",
        T + "m_pDev->m_pCtx->PSSetShaderResources(3, 2 * R11_KHOI_MAX, v);",
        "}",
        ""])
    s = rep_khoang(s, dau, cuoi, new, "A MangLay/GanMang")
    s = rep(s, T*2 + "pM = MangLay(fmt, &lop);" + NL + T*2 + "if (!pM) return NULL;" + NL,
            T*2 + "pM = MangLay(fmt, &lop);" + NL + T*2 + "if (!pM) return NULL;" + NL + T*2 + "pM->m_nSuDung++;" + T + "// " + TAG + NL, "A NewPage dem")
    s = rep(s, T*3 + "if (pPage->m_pMang)" + T + "// [MANG 09/09] tra lop cho mang (VRAM da cap giu nguyen, dung lai cho trang sau)" + NL +
            T*4 + "pPage->m_pMang->m_lopTrong.push_back(pPage->m_lop);" + NL,
            T*3 + "if (pPage->m_pMang)" + T + "// [MANG 09/09] tra lop cho khoi; khoi trong -> tra VRAM (" + TAG + ")" + NL +
            T*3 + "{" + NL +
            T*4 + "pPage->m_pMang->m_lopTrong.push_back(pPage->m_lop);" + NL +
            T*4 + "if (pPage->m_pMang->m_nSuDung) pPage->m_pMang->m_nSuDung--;" + NL +
            T*4 + "CAtlasMang* pM = pPage->m_pMang; pPage->m_pMang = NULL;" + NL +
            T*4 + "for (size_t i = 0; i < m_pages.size(); i++) if (m_pages[i] == pPage) { m_pages.erase(m_pages.begin() + i); break; }" + NL +
            T*4 + "KhoiXoaNeuTrong(pM);" + NL +
            T*3 + "}" + NL, "A Free")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11Atlas.cpp")
else:
    print("D3D9on11Atlas.cpp da co")

# ====================================================================== 4. D3D9on11Dev.cpp
P = D + "D3D9on11Dev.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, "#define R11_RING_SIZE (16 * 1024 * 1024)", "#define R11_RING_SIZE (12 * 1024 * 1024)" + T + "/* " + TAG + " 16 -> 12 MB: khung nang nhat do duoc 8,9 MB */", "V ring")
    s = rep(s, "unsigned g_uRep3CullGiu = 0, g_uRep3CullBo = 0;",
            "unsigned g_uRep3CullGiu = 0, g_uRep3CullBo = 0;" + NL +
            "// " + TAG + " truong 'lop' 9 bit theo dinh = khoi (4 bit) << 5 | lop trong khoi (5 bit)" + NL +
            "static inline UINT R11LopGoi(const CAtlasPage* p) { return (((p->m_pMang ? p->m_pMang->m_nKhoi : 0u) & 15u) << 5) | (p->m_lop & 31u); }", "V LopGoi")
    s = rep(s, "if (pTex && pTex->m_bVirtual && pTex->m_pPage) x |= ((pTex->m_pPage->m_lop & 0x1FFu) << 16) | ((pTex->m_pPage->m_bpp == 2 ? 1u : 2u) << 25);",
            "if (pTex && pTex->m_bVirtual && pTex->m_pPage) x |= (R11LopGoi(pTex->m_pPage) << 16) | ((pTex->m_pPage->m_bpp == 2 ? 1u : 2u) << 25);" + T + "// " + TAG, "V DinhThem")
    s = rep(s, "if (s == 1) { nNguon1 = (m_tex[1]->m_pPage->m_bpp == 2) ? 1 : 2; nLop1 = (int)m_tex[1]->m_pPage->m_lop; }",
            "if (s == 1) { nNguon1 = (m_tex[1]->m_pPage->m_bpp == 2) ? 1 : 2; nLop1 = (int)R11LopGoi(m_tex[1]->m_pPage); }", "V st1 lop")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11Dev.cpp")
else:
    print("D3D9on11Dev.cpp da co")
print("XONG " + TAG)
