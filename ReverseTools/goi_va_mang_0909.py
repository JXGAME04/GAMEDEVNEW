# -*- coding: utf-8 -*-
"""goi_va_mang_0909.py - [MANG 09/09] Atlas = Texture2DArray: doi trang atlas KHONG lam vo lo quad nua.

SO DO ([GOP] 17:0x-17:2x, 28 cua so co danh tran): 95,9 % lo quad vo vi DOI TRANG ATLAS (srv0), 2,2 % tham so shader stage 0,
1,8 % texture rieng; moi khung danh tran 6 300 quad -> 4 500 Draw (1,3-1,5 quad/Draw) + 1 100-1 800 lenh TRIANGLELIST ve ngay (chu KFont3).
Sua (khong doi mot diem anh):
  (1) moi dinh dang trang (BGRA8 / R8G8) mot Texture2DArray; trang = mot lop; mang lon len +8 lop (chep lop cu bang GPU);
      SRV cua mang dung chung cho moi trang -> R11Applied.srv[0] khong doi giua cac trang -> lo tiep tuc.
  (2) lop ghi vao 16 bit cao cua PALROW moi dinh (16 bit thap = hang bang mau, 0xFFFF = khong); stage 1 lay lop tu g_st1b.w.
  (3) shader PS ban MANG (Texture2DArray t0/t1, Sample float3(uv, lop)); moi SRV texture rieng cung la view mang 1 lop.
  (4) TRIANGLELIST (chu) vao cung bo gop (cung trang thai -> cung Draw).
  Lui: [Client] Rep3AtlasMang=0 -> trang rieng + PS cu y nhu truoc.
Can fxc (Windows Kits 10) de dich lai Rep3Shaders11.hlsl -> Rep3Shaders11_ps.h (cu) + Rep3Shaders11_psmang.h (moi).
"""
import io
import os
import subprocess
import sys

D = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/"
TAG = "[MANG 09/09]"
FXC = r"C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\fxc.exe"
T = "\t"


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


def het_dong(s, moc):
    """chen sau dong chua moc (tra ve vi tri ngay sau ky tu xuong dong)"""
    if s.count(moc) != 1:
        print("FAIL moc %s: %d" % (moc[:40], s.count(moc))); sys.exit(1)
    i = s.find(moc); j = s.find("\n", i)
    return j + 1


# ====================================================================== 1. HLSL
P = D + "Rep3Shaders11.hlsl"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, "Texture2D    g_t0 : register(t0);" + NL,
            NL.join(["#ifdef MANG",
                     "Texture2DArray g_t0 : register(t0);   // " + TAG + " atlas = mang trang; lop = 16 bit cao cua PALROW",
                     "#else",
                     "Texture2D    g_t0 : register(t0);",
                     "#endif", ""]), "hlsl t0")
    s = rep(s, "Texture2D    g_t1 : register(t1);" + NL,
            NL.join(["#ifdef MANG",
                     "Texture2DArray g_t1 : register(t1);   // " + TAG + " lop cua stage 1 = g_st1b.w",
                     "#else",
                     "Texture2D    g_t1 : register(t1);",
                     "#endif", ""]), "hlsl t1")
    j = het_dong(s, "Texture2D    g_pal : register(t2);")
    s = s[:j] + NL.join([
        "#ifdef MANG",
        "#define T0_SAMPLE(uv)  g_t0.Sample(g_s0, float3(uv, lop))",
        "#define T0_LOAD(p)     g_t0.Load(int4(p, (int)lop, 0))",
        "#define T0_DIM(d)      { float3 d3; g_t0.GetDimensions(d3.x, d3.y, d3.z); d = d3.xy; }",
        "#define T1_SAMPLE(uv)  g_t1.Sample(g_s1, float3(uv, (float)g_st1b.w))",
        "#else",
        "#define T0_SAMPLE(uv)  g_t0.Sample(g_s0, uv)",
        "#define T0_LOAD(p)     g_t0.Load(int3(p, 0))",
        "#define T0_DIM(d)      g_t0.GetDimensions(d.x, d.y)",
        "#define T1_SAMPLE(uv)  g_t1.Sample(g_s1, uv)",
        "#endif", ""]) + s[j:]
    s = rep(s, "float4 PS(VSOut i) : SV_Target" + NL + "{" + NL,
            "float4 PS(VSOut i) : SV_Target" + NL + "{" + NL + "    uint row = i.palrow & 0xFFFFu; float lop = (float)(i.palrow >> 16);   // " + TAG + NL, "hlsl PS dau")
    s = rep(s, "g_t0.Sample(g_s0, i.uv)", "T0_SAMPLE(i.uv)", "hlsl sample t0")
    s = rep(s, "if (i.palrow != 0xFFFFu && g_st0b.z != 0)", "if (row != 0xFFFFu && g_st0b.z != 0)", "hlsl row")
    s = rep(s, "float2 dim; g_t0.GetDimensions(dim.x, dim.y);", "float2 dim; T0_DIM(dim);", "hlsl dim")
    for v in ["clamp(p0, int2(0, 0), mx)", "clamp(p0 + int2(1, 0), int2(0, 0), mx)", "clamp(p0 + int2(0, 1), int2(0, 0), mx)", "clamp(p0 + int2(1, 1), int2(0, 0), mx)"]:
        s = rep(s, "g_t0.Load(int3(" + v + ", 0))", "T0_LOAD(" + v + ")", "hlsl load " + v[:14])
    s = rep(s, ", i.palrow)", ", row)", "hlsl palrow->row", 5)
    s = rep(s, "g_t1.Sample(g_s1, i.uv)", "T1_SAMPLE(i.uv)", "hlsl sample t1")
    ghi(P, s, h0, lf0, crlf0, "Rep3Shaders11.hlsl")
else:
    print("Rep3Shaders11.hlsl da co")

# dich shader
if not os.path.exists(FXC):
    print("FAIL khong thay fxc: " + FXC); sys.exit(1)
for args, ten in [(["/T", "ps_4_0", "/E", "PS", "/Fh", "Rep3Shaders11_ps.h", "/Vn", "g_Rep3PS11", "Rep3Shaders11.hlsl"], "PS cu"),
                  (["/T", "ps_4_0", "/E", "PS", "/D", "MANG=1", "/Fh", "Rep3Shaders11_psmang.h", "/Vn", "g_Rep3PS11Mang", "Rep3Shaders11.hlsl"], "PS MANG")]:
    r = subprocess.run([FXC, "/nologo"] + args, cwd=D, capture_output=True, text=True)
    if r.returncode != 0:
        print("FAIL fxc %s:\n%s\n%s" % (ten, r.stdout, r.stderr)); sys.exit(1)
    print("OK fxc " + ten + (" " + r.stdout.strip() if r.stdout.strip() else ""))

# ====================================================================== 2. D3D9on11i.h
P = D + "D3D9on11i.h"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, "class CAtlasPage" + NL + "{" + NL + "public:" + NL + T + "ID3D11Texture2D* m_pTex; ID3D11ShaderResourceView* m_pSrv;" + NL,
            NL.join([
                "struct CAtlasMang" + T + "// " + TAG + " mot Texture2DArray cho mot dinh dang; moi trang atlas = mot lop (slice)",
                "{",
                T + "ID3D11Texture2D* m_pTex; ID3D11ShaderResourceView* m_pSrv; DXGI_FORMAT m_fmt; UINT m_bpp;",
                T + "UINT m_nLop, m_nDung; std::vector<UINT> m_lopTrong;" + T + "// so lop da cap, so lop da phat (ke tiep), lop da tra lai",
                "};",
                "class CAtlasPage",
                "{",
                "public:",
                T + "ID3D11Texture2D* m_pTex; ID3D11ShaderResourceView* m_pSrv;" + T + "// che do mang: = cua mang (doi khi mang lon len); che do cu: cua trang",
                T + "CAtlasMang* m_pMang; UINT m_lop;" + T + "// " + TAG + " mang + chi so lop; che do cu: NULL, 0",
                ""]), "H CAtlasPage")
    s = rep(s, T + "CDev11* m_pDev; std::vector<CAtlasPage*> m_pages; UINT m_pageSize;" + NL + "};",
            T + "CDev11* m_pDev; std::vector<CAtlasPage*> m_pages; UINT m_pageSize;" + NL +
            T + "CAtlasMang* MangLay(DXGI_FORMAT fmt, UINT* pLop);" + T + "// " + TAG + " lay mot lop trong (tao/lon mang khi can)" + NL +
            T + "void MangXoa();" + NL +
            T + "std::vector<CAtlasMang*> m_mang;" + NL + "};", "H CAtlasMgr")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11i.h")
else:
    print("D3D9on11i.h da co")

# ====================================================================== 3. D3D9on11Atlas.cpp
P = D + "D3D9on11Atlas.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep_khoang(s, "void CAtlasMgr::ReleaseAll()" + NL + "{", NL + "}" + NL, NL.join([
        "void CAtlasMgr::ReleaseAll()",
        "{",
        T + "for (size_t i = 0; i < m_pages.size(); i++)",
        T + "{",
        T*2 + "CAtlasPage* p = m_pages[i];",
        T*2 + "if (!p->m_pMang)" + T + "// " + TAG + " che do mang: texture/SRV thuoc mang, xoa o MangXoa",
        T*2 + "{",
        T*3 + "if (p->m_pSrv) p->m_pSrv->Release();",
        T*3 + "if (p->m_pTex) p->m_pTex->Release();",
        T*2 + "}",
        T*2 + "delete p;",
        T + "}",
        T + "m_pages.clear();",
        T + "MangXoa();",
        T + "g_uRep3AtlasPages = 0; g_uRep3AtlasBytes = 0;",
        "}", ""]), "A ReleaseAll")
    s = rep_khoang(s, "CAtlasPage* CAtlasMgr::NewPage(UINT binH, DXGI_FORMAT fmt)" + NL + "{", NL + "}" + NL, NL.join([
        "// " + TAG + " lay mot lop trong cua mang theo dinh dang; het lop -> mang lon hon (+8 lop): chep cac lop cu bang GPU, doi SRV",
        "CAtlasMang* CAtlasMgr::MangLay(DXGI_FORMAT fmt, UINT* pLop)",
        "{",
        T + "CAtlasMang* pM = NULL;",
        T + "for (size_t i = 0; i < m_mang.size(); i++) if (m_mang[i]->m_fmt == fmt) { pM = m_mang[i]; break; }",
        T + "if (!pM)",
        T + "{",
        T*2 + "pM = new CAtlasMang(); pM->m_pTex = NULL; pM->m_pSrv = NULL; pM->m_fmt = fmt; pM->m_bpp = (fmt == DXGI_FORMAT_R8G8_UNORM) ? 2 : 4; pM->m_nLop = 0; pM->m_nDung = 0;",
        T*2 + "m_mang.push_back(pM);",
        T + "}",
        T + "if (!pM->m_lopTrong.empty()) { *pLop = pM->m_lopTrong.back(); pM->m_lopTrong.pop_back(); return pM; }",
        T + "if (pM->m_nDung >= pM->m_nLop)",
        T + "{",
        T*2 + "const UINT nMoi = pM->m_nLop + 8;",
        T*2 + "if (nMoi > 512) { R11Log(\"atlas mang %s: qua 512 lop\", pM->m_bpp == 2 ? \"R8G8\" : \"BGRA8\"); return NULL; }",
        T*2 + "m_pDev->FlushIfPending();" + T + "// lo dang cho con tham chieu SRV cu",
        T*2 + "D3D11_TEXTURE2D_DESC td; memset(&td, 0, sizeof(td));",
        T*2 + "td.Width = m_pageSize; td.Height = m_pageSize; td.MipLevels = 1; td.ArraySize = nMoi; td.Format = fmt; td.SampleDesc.Count = 1;",
        T*2 + "td.Usage = D3D11_USAGE_DEFAULT; td.BindFlags = D3D11_BIND_SHADER_RESOURCE;",
        T*2 + "const size_t uLop = (size_t)m_pageSize * m_pageSize * pM->m_bpp;",
        T*2 + "BYTE* pZero = (BYTE*)calloc(1, uLop);" + T + "// moi lop khoi tao 0 (khong de rac)",
        T*2 + "std::vector<D3D11_SUBRESOURCE_DATA> sr(nMoi);",
        T*2 + "for (UINT k = 0; k < nMoi; k++) { memset(&sr[k], 0, sizeof(sr[k])); sr[k].pSysMem = pZero; sr[k].SysMemPitch = m_pageSize * pM->m_bpp; }",
        T*2 + "ID3D11Texture2D* pTex = NULL;",
        T*2 + "HRESULT hr = m_pDev->m_pDev->CreateTexture2D(&td, pZero ? &sr[0] : NULL, &pTex);",
        T*2 + "if (pZero) free(pZero);",
        T*2 + "if (FAILED(hr) || !pTex) { R11Log(\"atlas mang: CreateTexture2D %u lop that bai 0x%08X\", nMoi, (unsigned)hr); return NULL; }",
        T*2 + "D3D11_SHADER_RESOURCE_VIEW_DESC vd; memset(&vd, 0, sizeof(vd));",
        T*2 + "vd.Format = fmt; vd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY; vd.Texture2DArray.MostDetailedMip = 0; vd.Texture2DArray.MipLevels = 1; vd.Texture2DArray.FirstArraySlice = 0; vd.Texture2DArray.ArraySize = nMoi;",
        T*2 + "ID3D11ShaderResourceView* pSrv = NULL;",
        T*2 + "hr = m_pDev->m_pDev->CreateShaderResourceView(pTex, &vd, &pSrv);",
        T*2 + "if (FAILED(hr) || !pSrv) { pTex->Release(); R11Log(\"atlas mang: SRV that bai 0x%08X\", (unsigned)hr); return NULL; }",
        T*2 + "if (pM->m_pTex)",
        T*2 + "{",
        T*3 + "for (UINT k = 0; k < pM->m_nLop; k++) m_pDev->m_pCtx->CopySubresourceRegion(pTex, k, 0, 0, 0, pM->m_pTex, k, NULL);",
        T*3 + "pM->m_pSrv->Release(); pM->m_pTex->Release();",
        T*2 + "}",
        T*2 + "pM->m_pTex = pTex; pM->m_pSrv = pSrv; pM->m_nLop = nMoi;",
        T*2 + "for (size_t i = 0; i < m_pages.size(); i++) if (m_pages[i]->m_pMang == pM) { m_pages[i]->m_pTex = pTex; m_pages[i]->m_pSrv = pSrv; }",
        T*2 + "m_pDev->m_bAppliedValid = false;" + T + "// SRV cu da huy, dia chi co the trung SRV moi -> ep gan lai",
        T*2 + "g_uRep3AtlasBytes = 0; for (size_t i = 0; i < m_mang.size(); i++) g_uRep3AtlasBytes += (unsigned __int64)m_mang[i]->m_nLop * m_pageSize * m_pageSize * m_mang[i]->m_bpp;",
        T*2 + "R11Log(\"atlas mang %s: %u lop (%u MB)\", pM->m_bpp == 2 ? \"R8G8\" : \"BGRA8\", nMoi, (unsigned)(((unsigned __int64)nMoi * uLop) >> 20));",
        T + "}",
        T + "*pLop = pM->m_nDung++;",
        T + "return pM;",
        "}",
        "",
        "void CAtlasMgr::MangXoa()",
        "{",
        T + "for (size_t i = 0; i < m_mang.size(); i++) { if (m_mang[i]->m_pSrv) m_mang[i]->m_pSrv->Release(); if (m_mang[i]->m_pTex) m_mang[i]->m_pTex->Release(); delete m_mang[i]; }",
        T + "m_mang.clear();",
        "}",
        "",
        "CAtlasPage* CAtlasMgr::NewPage(UINT binH, DXGI_FORMAT fmt)",
        "{",
        T + "UINT bpp = (fmt == DXGI_FORMAT_R8G8_UNORM) ? 2 : 4;" + T + "// [r]",
        T + "ID3D11Texture2D* pTex = NULL; ID3D11ShaderResourceView* pSrv = NULL; CAtlasMang* pM = NULL; UINT lop = 0;",
        T + "if (g_nRep3AtlasMang)",
        T + "{" + T + "// " + TAG + " trang = mot lop cua mang theo dinh dang; xoa lop ve 0 (lop tra lai co the con anh cu)",
        T*2 + "pM = MangLay(fmt, &lop);",
        T*2 + "if (!pM) return NULL;",
        T*2 + "pTex = pM->m_pTex; pSrv = pM->m_pSrv;",
        T*2 + "BYTE* pZero = (BYTE*)calloc(1, (size_t)m_pageSize * m_pageSize * bpp);",
        T*2 + "if (pZero) { m_pDev->m_pCtx->UpdateSubresource(pTex, lop, NULL, pZero, m_pageSize * bpp, 0); free(pZero); }",
        T + "}",
        T + "else",
        T + "{",
        T*2 + "D3D11_TEXTURE2D_DESC td; memset(&td, 0, sizeof(td));",
        T*2 + "td.Width = m_pageSize; td.Height = m_pageSize; td.MipLevels = 1; td.ArraySize = 1; td.Format = fmt; td.SampleDesc.Count = 1;",
        T*2 + "td.Usage = D3D11_USAGE_DEFAULT; td.BindFlags = D3D11_BIND_SHADER_RESOURCE;",
        T*2 + "BYTE* pZero = (BYTE*)calloc(1, (size_t)m_pageSize * m_pageSize * bpp);" + T + "// trang tao kem du lieu 0 (khong de rac)",
        T*2 + "D3D11_SUBRESOURCE_DATA sr; memset(&sr, 0, sizeof(sr)); sr.pSysMem = pZero; sr.SysMemPitch = m_pageSize * bpp;",
        T*2 + "HRESULT hr = m_pDev->m_pDev->CreateTexture2D(&td, pZero ? &sr : NULL, &pTex);",
        T*2 + "if (pZero) free(pZero);",
        T*2 + "if (FAILED(hr) || !pTex) { R11Log(\"atlas: CreateTexture2D trang %u that bai 0x%08X\", m_pageSize, (unsigned)hr); return NULL; }",
        T*2 + "hr = m_pDev->m_pDev->CreateShaderResourceView(pTex, NULL, &pSrv);",
        T*2 + "if (FAILED(hr)) { pTex->Release(); R11Log(\"atlas: CreateShaderResourceView trang that bai 0x%08X\", (unsigned)hr); return NULL; }",
        T*2 + "g_uRep3AtlasBytes += (unsigned __int64)m_pageSize * m_pageSize * bpp;",
        T + "}",
        T + "CAtlasPage* p = new CAtlasPage();",
        T + "p->m_pTex = pTex; p->m_pSrv = pSrv; p->m_pMang = pM; p->m_lop = lop; p->m_fmt = fmt; p->m_bpp = bpp; p->m_binH = binH; p->m_rows = m_pageSize / binH; p->m_used = 0;",
        T + "p->m_free.resize(p->m_rows);",
        T + "for (UINT r = 0; r < p->m_rows; r++) p->m_free[r].push_back(std::make_pair(0u, m_pageSize));" + T + "// ca hang trong",
        T + "m_pages.push_back(p);",
        T + "g_uRep3AtlasPages++;",
        T + "return p;",
        "}", ""]), "A NewPage")
    old = NL.join([
        T*2 + "if (nEmptySameClass >= 1)",
        T*2 + "{",
        T*3 + "m_pDev->FlushIfPending();",
        T*3 + "for (size_t i = 0; i < m_pages.size(); i++)",
        T*4 + "if (m_pages[i] == pPage) { m_pages.erase(m_pages.begin() + i); break; }",
        T*3 + "if (pPage->m_pSrv) pPage->m_pSrv->Release();",
        T*3 + "if (pPage->m_pTex) pPage->m_pTex->Release();",
        T*3 + "delete pPage;",
        T*3 + "if (g_uRep3AtlasPages) g_uRep3AtlasPages--;",
        T*3 + "g_uRep3AtlasBytes -= (unsigned __int64)m_pageSize * m_pageSize * pPage->m_bpp;",
        T*2 + "}", ""])
    new = NL.join([
        T*2 + "if (nEmptySameClass >= 1)",
        T*2 + "{",
        T*3 + "m_pDev->FlushIfPending();",
        T*3 + "for (size_t i = 0; i < m_pages.size(); i++)",
        T*4 + "if (m_pages[i] == pPage) { m_pages.erase(m_pages.begin() + i); break; }",
        T*3 + "if (pPage->m_pMang)" + T + "// " + TAG + " tra lop cho mang (VRAM da cap giu nguyen, dung lai cho trang sau)",
        T*4 + "pPage->m_pMang->m_lopTrong.push_back(pPage->m_lop);",
        T*3 + "else",
        T*3 + "{",
        T*4 + "if (pPage->m_pSrv) pPage->m_pSrv->Release();",
        T*4 + "if (pPage->m_pTex) pPage->m_pTex->Release();",
        T*4 + "g_uRep3AtlasBytes -= (unsigned __int64)m_pageSize * m_pageSize * pPage->m_bpp;",
        T*3 + "}",
        T*3 + "if (g_uRep3AtlasPages) g_uRep3AtlasPages--;",
        T*3 + "delete pPage;",
        T*2 + "}", ""])
    s = rep(s, old, new, "A Free")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11Atlas.cpp")
else:
    print("D3D9on11Atlas.cpp da co")

# ====================================================================== 4. D3D9on11.cpp (CTex11)
P = D + "D3D9on11.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, "m_pDev->m_pCtx->UpdateSubresource(m_pPage->m_pTex, 0, &bz, pZero, m_w * m_pPage->m_bpp, 0); free(pZero); }",
            "m_pDev->m_pCtx->UpdateSubresource(m_pPage->m_pTex, m_pPage->m_lop, &bz, pZero, m_w * m_pPage->m_bpp, 0); free(pZero); }" + T + "// " + TAG + " lop cua trang", "T zero")
    s = rep(s, T + "ID3D11Texture2D* pDst = m_bVirtual ? m_pPage->m_pTex : m_pGpu;" + NL,
            T + "ID3D11Texture2D* pDst = m_bVirtual ? m_pPage->m_pTex : m_pGpu;" + NL +
            T + "const UINT uSub = m_bVirtual ? m_pPage->m_lop : 0;" + T + "// " + TAG + " lop cua trang trong mang (che do cu = 0)" + NL, "T uSub")
    s = rep(s, "m_pDev->m_pCtx->UpdateSubresource(pDst, 0, &box, pConv, w * 4, 0);", "m_pDev->m_pCtx->UpdateSubresource(pDst, uSub, &box, pConv, w * 4, 0);", "T upload conv")
    s = rep(s, "m_pDev->m_pCtx->UpdateSubresource(pDst, 0, &box, m_pCpu + rc.top * m_pitch + rc.left * fi.bpp, m_pitch, 0);",
            "m_pDev->m_pCtx->UpdateSubresource(pDst, uSub, &box, m_pCpu + rc.top * m_pitch + rc.left * fi.bpp, m_pitch, 0);", "T upload")
    s = rep(s, T + "hr = m_pDev->m_pDev->CreateShaderResourceView(m_pGpu, NULL, &m_pSrv);" + NL, NL.join([
        T + "if (g_nRep3AtlasMang)",
        T + "{" + T + "// " + TAG + " shader khai bao Texture2DArray -> SRV texture rieng cung la view MANG 1 lop",
        T*2 + "D3D11_SHADER_RESOURCE_VIEW_DESC vd; memset(&vd, 0, sizeof(vd));",
        T*2 + "vd.Format = m_dxgi; vd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY; vd.Texture2DArray.MostDetailedMip = 0; vd.Texture2DArray.MipLevels = 1; vd.Texture2DArray.FirstArraySlice = 0; vd.Texture2DArray.ArraySize = 1;",
        T*2 + "hr = m_pDev->m_pDev->CreateShaderResourceView(m_pGpu, &vd, &m_pSrv);",
        T + "}",
        T + "else",
        T*2 + "hr = m_pDev->m_pDev->CreateShaderResourceView(m_pGpu, NULL, &m_pSrv);",
        ""]), "T SRV rieng")
    s = rep(s, T*2 + "if (m_bDirty && m_pCpu) { HRESULT hr = UploadRect(&m_rcDirty); m_bDirty = false; return hr; }" + NL + T*2 + "return D3D_OK;" + NL,
            T*2 + "m_pSrv = m_pPage->m_pSrv;" + T + "// " + TAG + " SRV cua mang co the da doi khi mang lon len" + NL +
            T*2 + "if (m_bDirty && m_pCpu) { HRESULT hr = UploadRect(&m_rcDirty); m_bDirty = false; return hr; }" + NL + T*2 + "return D3D_OK;" + NL, "T PrepareForBind")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11.cpp")
else:
    print("D3D9on11.cpp da co")

# ====================================================================== 5. D3D9on11Dev.cpp
P = D + "D3D9on11Dev.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, "#include \"Rep3Shaders11_ps.h\"" + NL, "#include \"Rep3Shaders11_ps.h\"" + NL + "#include \"Rep3Shaders11_psmang.h\"" + T + "// " + TAG + " PS voi atlas Texture2DArray" + NL, "V include")
    s = rep(s, T + "hr = m_pDev->CreatePixelShader(g_Rep3PS11, sizeof(g_Rep3PS11), NULL, &m_pPS);" + NL,
            T + "hr = g_nRep3AtlasMang ? m_pDev->CreatePixelShader(g_Rep3PS11Mang, sizeof(g_Rep3PS11Mang), NULL, &m_pPS) : m_pDev->CreatePixelShader(g_Rep3PS11, sizeof(g_Rep3PS11), NULL, &m_pPS);" + T + "// " + TAG + NL, "V PS")
    s = rep(s, "m_pAtlas ? \"BAT (trang 1024x1024 BGRA8, texture <= 512 khong RT)\" : \"tat\"",
            "m_pAtlas ? (g_nRep3AtlasMang ? \"BAT, MANG Texture2DArray (trang 1024x1024, texture <= 512 khong RT)\" : \"BAT (trang 1024x1024 BGRA8, texture <= 512 khong RT)\") : \"tat\"", "V log")
    s = rep(s, "HRESULT CDev11::DrawInternal(D3DPRIMITIVETYPE type, const BYTE* pVerts, UINT nVerts, UINT stride)" + NL + "{", NL.join([
        "// " + TAG + " 4 byte PALROW moi dinh: 16 bit thap = hang bang mau (0xFFFF = khong), 16 bit cao = lop cua trang trong mang atlas",
        "static inline UINT R11PalLop(CTex11* pTex)",
        "{",
        T + "UINT u = (pTex && pTex->m_nPalRow >= 0) ? (UINT)pTex->m_nPalRow : 0xFFFFu;",
        T + "if (pTex && pTex->m_bVirtual && pTex->m_pPage) u |= (pTex->m_pPage->m_lop << 16);",
        T + "return u;",
        "}",
        "",
        "HRESULT CDev11::DrawInternal(D3DPRIMITIVETYPE type, const BYTE* pVerts, UINT nVerts, UINT stride)",
        "{"]), "V R11PalLop")
    dau = T + "// ---- [j] quad (strip 4 dinh): gop" + NL
    cuoi = T*2 + "g_uRep3BatchQuads++;" + NL + T*2 + "return D3D_OK;" + NL + T + "}" + NL
    new = NL.join([
        T + "// ---- [j] quad (strip 4 dinh): gop; " + TAG + " them TRIANGLELIST (chu KFont3: 2 tam giac moi ky tu) vao cung lo",
        T + "const bool bQuad = (type == D3DPT_TRIANGLESTRIP && nVerts == 4);",
        T + "const bool bList = (type == D3DPT_TRIANGLELIST && (nVerts % 3) == 0 && nVerts <= 3072);",
        T + "if (g_nRep3Batch && (bQuad || bList))",
        T + "{",
        T*2 + "R11Applied a;",
        T*2 + "ComputeApplied(a, pIL, stride);",
        T*2 + "const UINT s11 = stride + 4;" + T + "// [r] +4 byte PALROW",
        T*2 + "const UINT nThem = bQuad ? 6 : nVerts;",
        T*2 + "if (m_batchVerts && (memcmp(&a, &m_batchState, sizeof(a)) != 0 || m_batch.size() + (size_t)nThem * s11 > 2 * 1024 * 1024))",
        T*2 + "{",
        T*3 + "const R11Applied& b = m_batchState; int nLy = 11;" + T + "// [GOP 09/09 do] ly do dau tien theo thu tu uu tien",
        T*3 + "if (a.srv[0] != b.srv[0]) nLy = (m_tex[0] && m_tex[0]->m_bVirtual) ? 0 : 1;",
        T*3 + "else if (a.srv[1] != b.srv[1]) nLy = 2;",
        T*3 + "else if (a.pBlend != b.pBlend) nLy = 3;",
        T*3 + "else if (a.pSamp[0] != b.pSamp[0] || a.pSamp[1] != b.pSamp[1]) nLy = 4;",
        T*3 + "else if (memcmp(a.ps.st0, b.ps.st0, sizeof(a.ps.st0)) != 0 || memcmp(a.ps.st0b, b.ps.st0b, sizeof(a.ps.st0b)) != 0) nLy = 5;",
        T*3 + "else if (memcmp(a.ps.st1, b.ps.st1, sizeof(a.ps.st1)) != 0 || memcmp(a.ps.st1b, b.ps.st1b, sizeof(a.ps.st1b)) != 0) nLy = 6;",
        T*3 + "else if (memcmp(a.ps.at, b.ps.at, sizeof(a.ps.at)) != 0) nLy = 7;",
        T*3 + "else if (memcmp(&a.vs, &b.vs, sizeof(a.vs)) != 0) nLy = 8;",
        T*3 + "else if (a.pIL != b.pIL || a.stride != b.stride) nLy = 9;",
        T*3 + "else if (a.pRaster != b.pRaster || a.bScissor != b.bScissor || memcmp(&a.rcScissor, &b.rcScissor, sizeof(RECT)) != 0 || memcmp(&a.vp, &b.vp, sizeof(a.vp)) != 0) nLy = 10;",
        T*3 + "g_uRep3GopVo[nLy]++;",
        T*3 + "FlushBatch();",
        T*2 + "}",
        T*2 + "if (!m_batchVerts) m_batchState = a;",
        T*2 + "const UINT uPal = R11PalLop(m_tex[0]);",
        T*2 + "size_t base = m_batch.size();",
        T*2 + "m_batch.resize(base + (size_t)nThem * s11);",
        T*2 + "BYTE* d = &m_batch[base];",
        T*2 + "if (bQuad)",
        T*2 + "{",
        T*3 + "static const int s_idx[6] = { 0, 1, 2, 2, 1, 3 };",
        T*3 + "for (int i = 0; i < 6; i++) { memcpy(d + i * s11, pVerts + s_idx[i] * stride, stride); *(UINT*)(d + i * s11 + stride) = uPal; }",
        T*2 + "}",
        T*2 + "else",
        T*3 + "for (UINT i = 0; i < nVerts; i++) { memcpy(d + i * s11, pVerts + i * stride, stride); *(UINT*)(d + i * s11 + stride) = uPal; }",
        T*2 + "R11AtlasUv(d, nThem, s11, m_fvf, m_tex[0], fPage);",
        T*2 + "m_batchVerts += nThem;",
        T*2 + "g_uRep3BatchQuads += bQuad ? 1 : (nVerts / 6);" + T + "// thong ke theo 'quad' (2 tam giac)",
        T*2 + "return D3D_OK;",
        T + "}",
        ""])
    s = rep_khoang(s, dau, cuoi, new, "V lo quad")
    s = rep(s, T + "const UINT uPal = (m_tex[0] && m_tex[0]->m_nPalRow >= 0) ? (UINT)m_tex[0]->m_nPalRow : 0xFFFFu;" + NL,
            T + "const UINT uPal = R11PalLop(m_tex[0]);" + T + "// " + TAG + NL, "V uPal ve ngay")
    s = rep(s, "cb.st1b[3] = 0;", "cb.st1b[3] = (m_tex[1] && m_tex[1]->m_bVirtual && m_tex[1]->m_pPage) ? (int)m_tex[1]->m_pPage->m_lop : 0;" + T + "// " + TAG + " lop cua texture stage 1", "V st1b")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11Dev.cpp")
else:
    print("D3D9on11Dev.cpp da co")

# ====================================================================== 6. KRepresentShell3.cpp + BaseInclude.h (khoa cau hinh)
P = D + "KRepresentShell3.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    j = het_dong(s, "int  g_nRep3Batch     = 1;")
    s = s[:j] + "int  g_nRep3AtlasMang = 1;" + T + "// " + TAG + " atlas = Texture2DArray (doi trang khong vo lo quad); 0 = trang rieng + PS cu" + NL + s[j:]
    j = het_dong(s, T + "g_nRep3Batch     = Rep3Ini(\"Rep3Batch\", 1);")
    s = s[:j] + T + "g_nRep3AtlasMang = Rep3Ini(\"Rep3AtlasMang\", 1);" + T + "// " + TAG + NL + s[j:]
    ghi(P, s, h0, lf0, crlf0, "KRepresentShell3.cpp")
else:
    print("KRepresentShell3.cpp da co")

P = D + "BaseInclude.h"
s, h0, lf0, crlf0 = doc(P)
if TAG not in s:
    s = rep(s, "extern int  g_nRep3Batch; extern unsigned g_uRep3BatchQuads; extern unsigned g_uRep3BatchDraws;",
            "extern int  g_nRep3Batch; extern unsigned g_uRep3BatchQuads; extern unsigned g_uRep3BatchDraws; extern int g_nRep3AtlasMang; /* " + TAG + " */", "B extern")
    ghi(P, s, h0, lf0, crlf0, "BaseInclude.h")
else:
    print("BaseInclude.h da co")

# ra soat: con cho nao dung m_pPage->m_pTex voi subresource 0?
for f in ["D3D9on11.cpp", "D3D9on11Dev.cpp", "D3D9on11Atlas.cpp"]:
    t = io.open(D + f, "r", encoding="latin-1", newline="").read().replace("\r\n", "\n").split("\n")
    for i, l in enumerate(t):
        if "m_pPage->m_pTex" in l and "m_lop" not in l and "pDst" not in l:
            print("  RA SOAT %s:%d: %s" % (f, i + 1, l.strip()[:150]))
print("XONG " + TAG)
