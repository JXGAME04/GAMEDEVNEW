# -*- coding: utf-8 -*-
"""goi_va_mang_b_0909.py - [MANG 09/09 b] (1) alpha test theo DINH (khong con la ly do vo lo: 70 % so lan vo sau [MANG]),
(2) hai mang atlas gan co dinh t3 (R8G8) / t4 (BGRA8), nguon chon theo dinh -> doi dinh dang khong vo lo (8 %),
(3) mang lon len KHONG kem du lieu khoi tao (bo khung 20-57 ms moi lan lon), lon dan x1,5.

So do 17:3x-17:4x (13 cua so, ban MANG): vo lo alphatest 25,2 M (70 %), ps st0 4,0 M (11 %), texture rieng 3,7 M (10 %),
doi mang BGRA8<->R8G8 2,8 M (8 %). DrawPrimitives max 34,8 / 56,6 ms dung luc 'atlas mang R8G8: N lop'.
Dinh them 8 byte (uint2 PALROW): x = hang bang mau (16) | lop (9) <<16 | nguon (2) <<25 [0 t0 rieng, 1 t3 R8G8, 2 t4 BGRA8]
| alpha test bat (1) <<27 | (ham-1) (3) <<28; y = alpha ref (8). PS ban MANG doc tu dinh; ban cu (Rep3AtlasMang=0) doc cb nhu truoc.
Stage 1: nguon/lop trong g_at.xy (cb, hiem doi). Ket qua tung diem anh y het.
"""
import io
import os
import subprocess
import sys

D = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/"
TAG = "[MANG 09/09 b]"
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


# ====================================================================== 1. HLSL
P = D + "Rep3Shaders11.hlsl"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, "float2 uv : TEXCOORD0; uint palrow : PALROW; };", "float2 uv : TEXCOORD0; uint2 palrow : PALROW; };", "hlsl VSIn")
    s = rep(s, "nointerpolation uint palrow : PALROW; };", "nointerpolation uint2 palrow : PALROW; };   // " + TAG + " x = hang | lop | nguon | alpha test, y = alpha ref", "hlsl VSOut")
    # mang atlas t3/t4 (MANG)
    j = s.find("Texture2D    g_pal : register(t2);"); j = s.find("\n", j) + 1
    s = s[:j] + NL.join([
        "#ifdef MANG",
        "Texture2DArray g_a8  : register(t3);  // " + TAG + " atlas R8G8 (bang mau), gan co dinh",
        "Texture2DArray g_a32 : register(t4);  // " + TAG + " atlas BGRA8, gan co dinh",
        "// nguon texture stage 0 theo dinh: 0 = t0 (texture rieng, view mang 1 lop), 1 = t3, 2 = t4",
        "float4 T0Sample(uint src, float lop, float2 uv)",
        "{",
        "    if (src == 1u) return g_a8.Sample(g_s0, float3(uv, lop));",
        "    if (src == 2u) return g_a32.Sample(g_s0, float3(uv, lop));",
        "    return g_t0.Sample(g_s0, float3(uv, 0.0));",
        "}",
        "float4 T0Load(uint src, float lop, int2 p)",
        "{",
        "    if (src == 1u) return g_a8.Load(int4(p, (int)lop, 0));",
        "    if (src == 2u) return g_a32.Load(int4(p, (int)lop, 0));",
        "    return g_t0.Load(int4(p, 0, 0));",
        "}",
        "float2 T0Dim(uint src)",
        "{",
        "    float3 d;",
        "    if (src == 1u) g_a8.GetDimensions(d.x, d.y, d.z);",
        "    else if (src == 2u) g_a32.GetDimensions(d.x, d.y, d.z);",
        "    else g_t0.GetDimensions(d.x, d.y, d.z);",
        "    return d.xy;",
        "}",
        "float4 T1Sample(float2 uv)",
        "{   // stage 1: nguon = g_at.x, lop = g_at.y (cb, hiem doi)",
        "    uint src = (uint)g_at.x; float lop = g_at.y;",
        "    if (src == 1u) return g_a8.Sample(g_s1, float3(uv, lop));",
        "    if (src == 2u) return g_a32.Sample(g_s1, float3(uv, lop));",
        "    return g_t1.Sample(g_s1, float3(uv, 0.0));",
        "}",
        "#endif", ""]) + s[j:]
    old = NL.join([
        "#ifdef MANG",
        "#define T0_SAMPLE(uv)  g_t0.Sample(g_s0, float3(uv, lop))",
        "#define T0_LOAD(p)     g_t0.Load(int4(p, (int)lop, 0))",
        "#define T0_DIM(d)      { float3 d3; g_t0.GetDimensions(d3.x, d3.y, d3.z); d = d3.xy; }",
        "#define T1_SAMPLE(uv)  g_t1.Sample(g_s1, float3(uv, (float)g_st1b.w))",
        "#else", ""])
    new = NL.join([
        "#ifdef MANG",
        "#define T0_SAMPLE(uv)  T0Sample(src, lop, uv)",
        "#define T0_LOAD(p)     T0Load(src, lop, p)",
        "#define T0_DIM(d)      d = T0Dim(src)",
        "#define T1_SAMPLE(uv)  T1Sample(uv)",
        "#else", ""])
    s = rep(s, old, new, "hlsl macro")
    s = rep(s, "    uint row = i.palrow & 0xFFFFu; float lop = (float)(i.palrow >> 16);   // [MANG 09/09]" + NL,
            NL.join([
                "    uint row = i.palrow.x & 0xFFFFu;   // [MANG 09/09] hang bang mau",
                "#ifdef MANG",
                "    float lop = (float)((i.palrow.x >> 16) & 0x1FFu); uint src = (i.palrow.x >> 25) & 3u;   // " + TAG + " lop + nguon theo dinh",
                "#endif", ""]), "hlsl PS dau")
    old = NL.join([
        "    if (g_at.x > 0.5)",
        "    {",
        "        // so sanh tren alpha 8 bit nhu phan cung D3D9",
        "        int a8 = (int)floor(cur.a * 255.0 + 0.5);",
        "        int r8 = (int)g_at.z;",
        "        int f = (int)g_at.y;", ""])
    new = NL.join([
        "#ifdef MANG",
        "    if (((i.palrow.x >> 27) & 1u) != 0u)   // " + TAG + " alpha test theo dinh: bat | ham | ref",
        "    {",
        "        int a8 = (int)floor(cur.a * 255.0 + 0.5);",
        "        int r8 = (int)(i.palrow.y & 0xFFu);",
        "        int f = (int)((i.palrow.x >> 28) & 7u) + 1;",
        "#else",
        "    if (g_at.x > 0.5)",
        "    {",
        "        // so sanh tren alpha 8 bit nhu phan cung D3D9",
        "        int a8 = (int)floor(cur.a * 255.0 + 0.5);",
        "        int r8 = (int)g_at.z;",
        "        int f = (int)g_at.y;",
        "#endif", ""])
    s = rep(s, old, new, "hlsl alpha test")
    ghi(P, s, h0, lf0, crlf0, "Rep3Shaders11.hlsl")
else:
    print("Rep3Shaders11.hlsl da co")

if not os.path.exists(FXC):
    print("FAIL khong thay fxc"); sys.exit(1)
for args, ten in [(["/T", "vs_4_0", "/E", "VS", "/Fh", "Rep3Shaders11_vs.h", "/Vn", "g_Rep3VS11", "Rep3Shaders11.hlsl"], "VS"),
                  (["/T", "ps_4_0", "/E", "PS", "/Fh", "Rep3Shaders11_ps.h", "/Vn", "g_Rep3PS11", "Rep3Shaders11.hlsl"], "PS cu"),
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
    s = rep(s, T + "void MangXoa();" + NL, T + "void MangXoa();" + NL + T + "void GanMang();" + T + "// " + TAG + " gan mang R8G8 -> t3, BGRA8 -> t4" + NL, "H GanMang")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11i.h")
else:
    print("D3D9on11i.h da co")

# ====================================================================== 3. D3D9on11Atlas.cpp
P = D + "D3D9on11Atlas.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, T*2 + "const UINT nMoi = pM->m_nLop + 8;" + NL + T*2 + "if (nMoi > 512) { R11Log(\"atlas mang %s: qua 512 lop\", pM->m_bpp == 2 ? \"R8G8\" : \"BGRA8\"); return NULL; }" + NL,
            T*2 + "UINT nMoi = pM->m_nLop + (pM->m_nLop < 16 ? 8 : pM->m_nLop / 2); if (nMoi > 512) nMoi = 512;" + T + "// " + TAG + " lon dan x1,5 (it lan lon hon)" + NL +
            T*2 + "if (nMoi <= pM->m_nLop) { R11Log(\"atlas mang %s: qua 512 lop\", pM->m_bpp == 2 ? \"R8G8\" : \"BGRA8\"); return NULL; }" + NL, "A lon dan")
    old = NL.join([
        T*2 + "const size_t uLop = (size_t)m_pageSize * m_pageSize * pM->m_bpp;",
        T*2 + "BYTE* pZero = (BYTE*)calloc(1, uLop);" + T + "// moi lop khoi tao 0 (khong de rac)",
        T*2 + "std::vector<D3D11_SUBRESOURCE_DATA> sr(nMoi);",
        T*2 + "for (UINT k = 0; k < nMoi; k++) { memset(&sr[k], 0, sizeof(sr[k])); sr[k].pSysMem = pZero; sr[k].SysMemPitch = m_pageSize * pM->m_bpp; }",
        T*2 + "ID3D11Texture2D* pTex = NULL;",
        T*2 + "HRESULT hr = m_pDev->m_pDev->CreateTexture2D(&td, pZero ? &sr[0] : NULL, &pTex);",
        T*2 + "if (pZero) free(pZero);", ""])
    new = NL.join([
        T*2 + "const size_t uLop = (size_t)m_pageSize * m_pageSize * pM->m_bpp;",
        T*2 + "ID3D11Texture2D* pTex = NULL;",
        T*2 + "HRESULT hr = m_pDev->m_pDev->CreateTexture2D(&td, NULL, &pTex);" + T + "// " + TAG + " KHONG kem du lieu khoi tao (tung lop duoc xoa 0 luc NewPage) -> khong con khung 20-57 ms", ""])
    s = rep(s, old, new, "A khong khoi tao")
    s = rep(s, T*2 + "m_pDev->m_bAppliedValid = false;" + T + "// SRV cu da huy, dia chi co the trung SRV moi -> ep gan lai" + NL,
            T*2 + "m_pDev->m_bAppliedValid = false; m_pDev->m_bPipeBound = false;" + T + "// SRV cu da huy -> ep gan lai trang thai va mang t3/t4 (" + TAG + ")" + NL, "A pipe")
    s = rep(s, "void CAtlasMgr::MangXoa()" + NL + "{", NL.join([
        "// " + TAG + " gan hai mang atlas co dinh: t3 = R8G8 (bang mau), t4 = BGRA8; nguon chon theo dinh trong shader",
        "void CAtlasMgr::GanMang()",
        "{",
        T + "ID3D11ShaderResourceView* v[2] = { NULL, NULL };",
        T + "for (size_t i = 0; i < m_mang.size(); i++) { if (m_mang[i]->m_bpp == 2) v[0] = m_mang[i]->m_pSrv; else v[1] = m_mang[i]->m_pSrv; }",
        T + "m_pDev->m_pCtx->PSSetShaderResources(3, 2, v);",
        "}",
        "",
        "void CAtlasMgr::MangXoa()",
        "{"]), "A GanMang")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11Atlas.cpp")
else:
    print("D3D9on11Atlas.cpp da co")

# ====================================================================== 4. D3D9on11Dev.cpp
P = D + "D3D9on11Dev.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    # (a) layout: PALROW 8 byte
    s = rep(s, "ie[n].SemanticName = \"PALROW\"; ie[n].SemanticIndex = 0; ie[n].Format = DXGI_FORMAT_R32_UINT;",
            "ie[n].SemanticName = \"PALROW\"; ie[n].SemanticIndex = 0; ie[n].Format = DXGI_FORMAT_R32G32_UINT; /* " + TAG + " 8 byte */", "V IL")
    # (b) FlushBatch stride
    s = rep(s, "UINT nVerts = m_batchVerts, stride = m_batchState.stride + 4, bytes = nVerts * stride;",
            "UINT nVerts = m_batchVerts, stride = m_batchState.stride + 8, bytes = nVerts * stride;	/* " + TAG + " +8 */", "V flush stride")
    # (c) ham dong goi dinh them
    s = rep_khoang(s, "static inline UINT R11PalLop(CTex11* pTex)", T + "return u;" + NL + "}" + NL, NL.join([
        "// " + TAG + " 8 byte them moi dinh (uint2 PALROW): x = hang bang mau (16 bit, 0xFFFF = khong) | lop (9) << 16 | nguon (2) << 25",
        "// [0 = t0 texture rieng, 1 = t3 atlas R8G8, 2 = t4 atlas BGRA8] | alpha test bat (1) << 27 | (ham - 1) (3) << 28; y = alpha ref (8 bit)",
        "static inline void R11DinhThem(CTex11* pTex, DWORD dwAtBat, DWORD dwAtHam, DWORD dwAtRef, UINT* pX, UINT* pY)",
        "{",
        T + "UINT x = (pTex && pTex->m_nPalRow >= 0) ? (UINT)pTex->m_nPalRow : 0xFFFFu;",
        T + "if (pTex && pTex->m_bVirtual && pTex->m_pPage) x |= ((pTex->m_pPage->m_lop & 0x1FFu) << 16) | ((pTex->m_pPage->m_bpp == 2 ? 1u : 2u) << 25);",
        T + "if (dwAtBat) x |= (1u << 27) | ((((dwAtHam & 15u) + 7u) & 7u) << 28);",
        T + "*pX = x; *pY = (UINT)(dwAtRef & 255u);",
        "}",
        ""]), "V R11DinhThem")
    s = rep(s, "const UINT uPal = R11PalLop(m_tex[0]);",
            "UINT uX = 0, uY = 0; R11DinhThem(m_tex[0], m_rs[D3DRS_ALPHATESTENABLE], m_rs[D3DRS_ALPHAFUNC], m_rs[D3DRS_ALPHAREF], &uX, &uY);", "V uPal", 2)
    s = rep(s, "const UINT s11 = stride + 4;" + T + "// [r] +4 byte PALROW", "const UINT s11 = stride + 8;" + T + "// [r] +4 byte PALROW, " + TAG + " +4 nua", "V s11", 2)
    s = rep(s, "*(UINT*)(d + i * s11 + stride) = uPal; }", "{ UINT* q = (UINT*)(d + i * s11 + stride); q[0] = uX; q[1] = uY; } }", "V ghi dinh lo", 2)
    s = rep(s, "*(UINT*)(&tmp[(i * 3 + k) * s11 + stride]) = uPal; }", "{ UINT* q = (UINT*)(&tmp[(i * 3 + k) * s11 + stride]); q[0] = uX; q[1] = uY; } }", "V ghi dinh fan")
    s = rep(s, "*(UINT*)(&tmp[i * s11 + stride]) = uPal; }", "{ UINT* q = (UINT*)(&tmp[i * s11 + stride]); q[0] = uX; q[1] = uY; } }", "V ghi dinh khac")
    # (d) ComputeApplied: atlas khong qua t0/t1; co texture = theo m_tex; alpha test ra khoi cb (MANG)
    old = NL.join([
        T + "for (int s = 0; s < 2; s++)",
        T + "{",
        T*2 + "if (m_tex[s]) { m_tex[s]->PrepareForBind(); a.srv[s] = m_tex[s]->m_pSrv; }",
        T*2 + "if (m_pRt && m_pRt->m_pTex && m_tex[s] == m_pRt->m_pTex) a.srv[s] = NULL;",
        T + "}", ""])
    new = NL.join([
        T + "int nBound[2] = { 0, 0 }; int nNguon1 = 0, nLop1 = 0;" + T + "// " + TAG,
        T + "for (int s = 0; s < 2; s++)",
        T + "{",
        T*2 + "if (m_tex[s]) { m_tex[s]->PrepareForBind(); a.srv[s] = m_tex[s]->m_pSrv; nBound[s] = 1; }",
        T*2 + "if (m_pRt && m_pRt->m_pTex && m_tex[s] == m_pRt->m_pTex) { a.srv[s] = NULL; nBound[s] = 0; }",
        T*2 + "if (g_nRep3AtlasMang && m_tex[s] && m_tex[s]->m_bVirtual && m_tex[s]->m_pPage)",
        T*2 + "{" + T + "// " + TAG + " atlas gan co dinh o t3/t4 -> khong qua t0/t1 (srv = NULL -> doi trang/dinh dang khong vo lo)",
        T*3 + "a.srv[s] = NULL;",
        T*3 + "if (s == 1) { nNguon1 = (m_tex[1]->m_pPage->m_bpp == 2) ? 1 : 2; nLop1 = (int)m_tex[1]->m_pPage->m_lop; }",
        T*2 + "}",
        T + "}", ""])
    s = rep(s, old, new, "V ComputeApplied srv")
    s = rep(s, "cb.st0b[2] = a.srv[0] ? 1 : 0;", "cb.st0b[2] = nBound[0];", "V bound0")
    s = rep(s, "cb.st1b[2] = a.srv[1] ? 1 : 0;", "cb.st1b[2] = nBound[1];", "V bound1")
    s = rep(s, T + "cb.at[0] = m_rs[D3DRS_ALPHATESTENABLE] ? 1.0f : 0.0f; cb.at[1] = (float)(m_rs[D3DRS_ALPHAFUNC] & 15); cb.at[2] = (float)(m_rs[D3DRS_ALPHAREF] & 255); cb.at[3] = 0.0f;" + NL,
            NL.join([
                T + "if (g_nRep3AtlasMang) { cb.at[0] = (float)nNguon1; cb.at[1] = (float)nLop1; cb.at[2] = 0.0f; cb.at[3] = 0.0f; }" + T + "// " + TAG + " alpha test theo dinh; g_at = nguon/lop stage 1",
                T + "else { cb.at[0] = m_rs[D3DRS_ALPHATESTENABLE] ? 1.0f : 0.0f; cb.at[1] = (float)(m_rs[D3DRS_ALPHAFUNC] & 15); cb.at[2] = (float)(m_rs[D3DRS_ALPHAREF] & 255); cb.at[3] = 0.0f; }",
                ""]), "V cb.at")
    # (e) ApplyComputed: gan mang t3/t4 cung luc gan bang mau
    s = rep(s, T*2 + "if (m_pPalSrv) m_pCtx->PSSetShaderResources(2, 1, &m_pPalSrv);" + T + "// [r]" + NL,
            T*2 + "if (m_pPalSrv) m_pCtx->PSSetShaderResources(2, 1, &m_pPalSrv);" + T + "// [r]" + NL +
            T*2 + "if (m_pAtlas && g_nRep3AtlasMang) m_pAtlas->GanMang();" + T + "// " + TAG + " t3 = atlas R8G8, t4 = atlas BGRA8" + NL, "V GanMang")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11Dev.cpp")
else:
    print("D3D9on11Dev.cpp da co")
print("XONG " + TAG)
