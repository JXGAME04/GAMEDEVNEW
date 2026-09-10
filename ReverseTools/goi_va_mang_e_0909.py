# -*- coding: utf-8 -*-
"""goi_va_mang_e_0909.py - [MANG 09/09 e] cull tren CPU cho lenh 2D (XYZRHW) trong bo gop -> chu (CULL_CCW) gop chung sprite (CULL_NONE).

So do 18:3x (ban MANG d): 92 % lo quad vo o muc 'vp/scissor/raster' = doi rasterizer state: KFont3 dat CULLMODE = CCW cho chu
(state block), sprite dung CULL_NONE; moi nhan ten / dong chat / so sat thuong cat lo mot lan (200-640 lan/khung).
Sua (y het ket qua): voi dinh XYZRHW va cull != NONE, tinh chieu quay tung tam giac tren CPU (tich cheo, y huong xuong):
CCW -> bo tam giac co cr < 0 (nguoc chieu kim dong ho), CW -> bo cr > 0, cr == 0 -> bo (khong co dien tich); roi dung raster
CULL_NONE cho lo -> chu va sprite cung trang thai. Dem 'cull cpu: giu N bo M' de kiem (chu dang hien = tat ca CW -> bo ~ 0).
"""
import io
import sys

D = "D:/GAMEDEVNEW_wt_delta/Sources/Represent/Represent3/"
TAG = "[MANG 09/09 e]"
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


# ---------------- D3D9on11i.h
P = D + "D3D9on11i.h"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    n = s.count("GetRasterState();")
    if n != 1:
        print("FAIL H GetRasterState: %d" % n); sys.exit(1)
    i = s.find("GetRasterState();"); j = s.find("\n", i) + 1
    s = s[:j] + T + "ID3D11RasterizerState* GetRasterStateCull(DWORD cull);" + T + "// " + TAG + " raster voi cull chi dinh (lo 2D dung CULL_NONE, cull tren CPU)" + NL + s[j:]
    ghi(P, s, h0, lf0, crlf0, "D3D9on11i.h")
else:
    print("D3D9on11i.h da co")

# ---------------- D3D9on11Dev.cpp
P = D + "D3D9on11Dev.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, "ID3D11RasterizerState* CDev11::GetRasterState()" + NL + "{" + NL + T + "DWORD cull = m_rs[D3DRS_CULLMODE] & 3, fill = m_rs[D3DRS_FILLMODE] & 3, sc = m_rs[D3DRS_SCISSORTESTENABLE] ? 1 : 0;" + NL,
            "ID3D11RasterizerState* CDev11::GetRasterState() { return GetRasterStateCull(m_rs[D3DRS_CULLMODE] & 3); }" + NL +
            "ID3D11RasterizerState* CDev11::GetRasterStateCull(DWORD cull)" + T + "// " + TAG + NL + "{" + NL +
            T + "DWORD fill = m_rs[D3DRS_FILLMODE] & 3, sc = m_rs[D3DRS_SCISSORTESTENABLE] ? 1 : 0;" + NL, "V GetRasterState")
    s = rep(s, "unsigned g_uRep3RingVong = 0; double g_dRep3RingMapMax = 0.0; unsigned g_uRep3TexRiengTao = 0;",
            "unsigned g_uRep3RingVong = 0; double g_dRep3RingMapMax = 0.0; unsigned g_uRep3TexRiengTao = 0;" + NL +
            "unsigned g_uRep3CullGiu = 0, g_uRep3CullBo = 0;" + T + "// " + TAG + " tam giac 2D giu / bo khi cull tren CPU", "V dem")
    dau = T + "// ---- [j] quad (strip 4 dinh): gop; [MANG 09/09] them TRIANGLELIST (chu KFont3: 2 tam giac moi ky tu) vao cung lo" + NL
    cuoi = T*2 + "g_uRep3BatchQuads += bQuad ? 1 : (nVerts / 6);" + T + "// thong ke theo 'quad' (2 tam giac)" + NL + T*2 + "return D3D_OK;" + NL + T + "}" + NL
    new = NL.join([
        T + "// ---- [j] quad (strip 4 dinh): gop; [MANG 09/09] them TRIANGLELIST (chu KFont3: 2 tam giac moi ky tu) vao cung lo",
        T + "const bool bQuad = (type == D3DPT_TRIANGLESTRIP && nVerts == 4);",
        T + "const bool bList = (type == D3DPT_TRIANGLELIST && (nVerts % 3) == 0 && nVerts <= 3072);",
        T + "if (g_nRep3Batch && (bQuad || bList))",
        T + "{",
        T*2 + "const bool bRhw = ((m_fvf & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW);",
        T*2 + "const DWORD dwCull = m_rs[D3DRS_CULLMODE] & 3;",
        T*2 + "const bool bCullCpu = bRhw && (dwCull == D3DCULL_CW || dwCull == D3DCULL_CCW);" + T + "// " + TAG + " 2D: cull tren CPU, lo dung CULL_NONE (chu CCW gop chung sprite NONE)",
        T*2 + "R11Applied a;",
        T*2 + "ComputeApplied(a, pIL, stride);",
        T*2 + "if (bCullCpu) a.pRaster = GetRasterStateCull(D3DCULL_NONE);",
        T*2 + "const UINT s11 = stride + 8;" + T + "// [r] +4 byte PALROW, [MANG 09/09 b] +4 nua",
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
        T*2 + "UINT uX = 0, uY = 0; R11DinhThem(m_tex[0], m_rs[D3DRS_ALPHATESTENABLE], m_rs[D3DRS_ALPHAFUNC], m_rs[D3DRS_ALPHAREF], m_tss[0], (m_tex[0] && !(m_pRt && m_pRt->m_pTex && m_tex[0] == m_pRt->m_pTex)) ? 1 : 0, &uX, &uY);",
        T*2 + "size_t base = m_batch.size();",
        T*2 + "m_batch.resize(base + (size_t)nThem * s11);",
        T*2 + "BYTE* d = &m_batch[base];",
        T*2 + "static const int s_idx[6] = { 0, 1, 2, 2, 1, 3 };",
        T*2 + "const UINT nTri = nThem / 3; UINT nGhi = 0;",
        T*2 + "for (UINT t = 0; t < nTri; t++)",
        T*2 + "{",
        T*3 + "const UINT i0 = bQuad ? (UINT)s_idx[t * 3] : t * 3, i1 = bQuad ? (UINT)s_idx[t * 3 + 1] : t * 3 + 1, i2 = bQuad ? (UINT)s_idx[t * 3 + 2] : t * 3 + 2;",
        T*3 + "if (bCullCpu)",
        T*3 + "{" + T + "// tich cheo tren man hinh (y huong xuong): cr > 0 = thuan chieu kim dong ho (mat truoc D3D9)",
        T*4 + "const float* p0 = (const float*)(pVerts + i0 * stride); const float* p1 = (const float*)(pVerts + i1 * stride); const float* p2 = (const float*)(pVerts + i2 * stride);",
        T*4 + "const float cr = (p1[0] - p0[0]) * (p2[1] - p0[1]) - (p1[1] - p0[1]) * (p2[0] - p0[0]);",
        T*4 + "if (cr == 0.0f || (dwCull == D3DCULL_CCW ? (cr < 0.0f) : (cr > 0.0f))) { g_uRep3CullBo++; continue; }",
        T*4 + "g_uRep3CullGiu++;",
        T*3 + "}",
        T*3 + "BYTE* q = d + (size_t)nGhi * s11;",
        T*3 + "memcpy(q, pVerts + i0 * stride, stride); *(UINT*)(q + stride) = uX; *(UINT*)(q + stride + 4) = uY;",
        T*3 + "memcpy(q + s11, pVerts + i1 * stride, stride); *(UINT*)(q + s11 + stride) = uX; *(UINT*)(q + s11 + stride + 4) = uY;",
        T*3 + "memcpy(q + 2 * s11, pVerts + i2 * stride, stride); *(UINT*)(q + 2 * s11 + stride) = uX; *(UINT*)(q + 2 * s11 + stride + 4) = uY;",
        T*3 + "nGhi += 3;",
        T*2 + "}",
        T*2 + "if (nGhi != nThem) m_batch.resize(base + (size_t)nGhi * s11);",
        T*2 + "if (nGhi == 0) return D3D_OK;",
        T*2 + "R11AtlasUv(&m_batch[base], nGhi, s11, m_fvf, m_tex[0], fPage);",
        T*2 + "m_batchVerts += nGhi;",
        T*2 + "g_uRep3BatchQuads += bQuad ? 1 : (nVerts / 6);" + T + "// thong ke theo 'quad' (2 tam giac)",
        T*2 + "return D3D_OK;",
        T + "}",
        ""])
    s = rep_khoang(s, dau, cuoi, new, "V lo quad")
    ghi(P, s, h0, lf0, crlf0, "D3D9on11Dev.cpp")
else:
    print("D3D9on11Dev.cpp da co")

# ---------------- KRepresentShell3.cpp: in cull cpu
P = D + "KRepresentShell3.cpp"
s, h0, lf0, crlf0 = doc(P)
NL = "\r\n" if crlf0 else "\n"
if TAG not in s:
    s = rep(s, "extern unsigned g_uRep3GopVo[12]; extern unsigned g_uRep3VeNgay[4];",
            "extern unsigned g_uRep3GopVo[12]; extern unsigned g_uRep3VeNgay[4]; extern unsigned g_uRep3CullGiu, g_uRep3CullBo; /* " + TAG + " */", "K extern")
    s = rep(s, "| ve ngay: fan %u, list %u, strip %u, khac %u\",", "| ve ngay: fan %u, list %u, strip %u, khac %u | cull cpu: giu %u bo %u\",", "K fmt")
    s = rep(s, "g_uRep3VeNgay[0], g_uRep3VeNgay[1], g_uRep3VeNgay[2], g_uRep3VeNgay[3]);", "g_uRep3VeNgay[0], g_uRep3VeNgay[1], g_uRep3VeNgay[2], g_uRep3VeNgay[3], g_uRep3CullGiu, g_uRep3CullBo);", "K args")
    s = rep(s, "memset(g_uRep3GopVo, 0, sizeof(g_uRep3GopVo)); memset(g_uRep3VeNgay, 0, sizeof(g_uRep3VeNgay));",
            "memset(g_uRep3GopVo, 0, sizeof(g_uRep3GopVo)); memset(g_uRep3VeNgay, 0, sizeof(g_uRep3VeNgay)); g_uRep3CullGiu = g_uRep3CullBo = 0;", "K reset")
    ghi(P, s, h0, lf0, crlf0, "KRepresentShell3.cpp")
else:
    print("KRepresentShell3.cpp da co")
print("XONG " + TAG)
