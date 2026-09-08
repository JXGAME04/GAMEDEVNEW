# -*- coding: utf-8 -*-
"""goi_va_d3d11r_palette_0809.py - [D3D11 08/09 r] texture bang mau khong mat mau (xem D3D9on11Pal.cpp).
Sua: D3D9on11i.h (CTex11 m_nPalRow, CAtlasPage m_fmt, Alloc/NewPage theo dinh dang, CDev11 bang mau + PalRow trong dinh),
D3D9on11.h (export), D3D9on11.cpp (A8L8, atlas theo dinh dang), D3D9on11Atlas.cpp (trang R8G8), D3D9on11Dev.cpp (dinh + 4 byte
PALROW, layout, bang mau SRV t2, xa hang thu sau Present), Rep3Shaders11.hlsl (tra bang), TextureRes.cpp/.h (giai ma chi so+alpha,
cap hang bang mau), KRepresentShell3.cpp (ini Rep3Pal + thong ke), BaseInclude.h, vcxproj."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"
def load(name):
    p = ROOT + "\\" + name
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    if name.startswith("D3D9on11") or name.startswith("Rep3Shaders11"): s = s.replace("\r\n", "\n")
    return p, s
def save(p, s, h0=None):
    if h0 is not None and sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao", p); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:70], c, n)); sys.exit(1)
    return s.replace(old, new)

# ---------------- D3D9on11i.h
p, s = load("D3D9on11i.h")
if "m_nPalRow" not in s:
    s = rep(s, "\tbool        m_bVirtual;\t\t\t// [d] o trong trang atlas (m_pSrv = SRV cua trang, khong so huu)\n",
        "\tbool        m_bVirtual;\t\t\t// [d] o trong trang atlas (m_pSrv = SRV cua trang, khong so huu)\n\tint         m_nPalRow;\t\t\t// [r] hang bang mau (-1 = khong phai texture chi so)\n")
    s = rep(s, "\tID3D11Texture2D* m_pTex; ID3D11ShaderResourceView* m_pSrv;\n\tUINT m_binH, m_rows, m_used;\n",
        "\tID3D11Texture2D* m_pTex; ID3D11ShaderResourceView* m_pSrv;\n\tDXGI_FORMAT m_fmt; UINT m_bpp;\t// [r] trang BGRA8 (4) hoac R8G8 (2)\n\tUINT m_binH, m_rows, m_used;\n")
    s = rep(s, "\tbool Alloc(UINT w, UINT h, CAtlasPage** ppPage, UINT* pX, UINT* pY);\n", "\tbool Alloc(UINT w, UINT h, DXGI_FORMAT fmt, CAtlasPage** ppPage, UINT* pX, UINT* pY);\n")
    s = rep(s, "\tCAtlasPage* NewPage(UINT binH);\n", "\tCAtlasPage* NewPage(UINT binH, DXGI_FORMAT fmt);\n")
    s = rep(s, "\tCAtlasMgr*      m_pAtlas;",
        "\t// [r] atlas bang mau (D3D9on11Pal.cpp)\n"
        "\tID3D11Texture2D* m_pPalTex; ID3D11ShaderResourceView* m_pPalSrv; std::vector<int> m_palFree, m_palDeferred;\n"
        "\tbool    PalInit(); void PalRelease(); void PalFrameEnd(); int PalAlloc(const unsigned char* pPal24, int nColors); void PalFree(int row);\n"
        "\tCAtlasMgr*      m_pAtlas;")
    s = rep(s, "class CD3D11Shim;\n\nclass CDev11 : public IDirect3DDevice9\n", "class CD3D11Shim;\nextern CDev11* g_pRep3Dev11;\nextern unsigned g_uRep3PalRows;\n\nclass CDev11 : public IDirect3DDevice9\n")
save(p, s); print("OK D3D9on11i.h")

# ---------------- D3D9on11.h
p, s = load("D3D9on11.h")
if "Rep3_D3D11AllocPalette" not in s:
    s = rep(s, "extern int      g_nRep3Buffers;", "extern int      g_nRep3Pal;          // [Client] Rep3Pal: 1 = texture sprite bang mau 2 B/px (mac dinh khi D3D11), 0 = BGRA8\n"
        "int  Rep3_D3D11PaletteOK();\nint  Rep3_D3D11AllocPalette(const unsigned char* pPal24, int nColors);\t// hang bang mau (-1 = het)\nvoid Rep3_D3D11FreePalette(int nRow);\nvoid Rep3_D3D11TagPalette(IDirect3DTexture9* pTex, int nRow);\n"
        "extern int      g_nRep3Buffers;")
save(p, s); print("OK D3D9on11.h")

# ---------------- D3D9on11.cpp
p, s = load("D3D9on11.cpp")
if "D3DFMT_A8L8" not in s:
    s = rep(s, "\tcase D3DFMT_A8:       r.dxgi = DXGI_FORMAT_A8_UNORM; r.bpp = 1; break;\n", "\tcase D3DFMT_A8:       r.dxgi = DXGI_FORMAT_A8_UNORM; r.bpp = 1; break;\n\tcase D3DFMT_A8L8:     r.dxgi = DXGI_FORMAT_R8G8_UNORM; r.bpp = 2; break;\t// [r] chi so + alpha\n")
    s = rep(s, "\tm_bVirtual = false; m_pPage = NULL; m_slot = 0; m_ax = 0; m_ay = 0;\n", "\tm_bVirtual = false; m_pPage = NULL; m_slot = 0; m_ax = 0; m_ay = 0; m_nPalRow = -1;\n")
    s = rep(s, "\t\tif (!m_pDev->m_pAtlas || !m_pDev->m_pAtlas->Alloc(m_w, m_h, &m_pPage, &m_ax, &m_ay))\n",
        "\t\tDXGI_FORMAT fmtPage = (m_fmt == D3DFMT_A8L8) ? DXGI_FORMAT_R8G8_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM;\t// [r]\n"
        "\t\tif (!m_pDev->m_pAtlas || !m_pDev->m_pAtlas->Alloc(m_w, m_h, fmtPage, &m_pPage, &m_ax, &m_ay))\n")
    s = rep(s, "\t\t\tm_pSrv = m_pPage->m_pSrv; m_dxgi = DXGI_FORMAT_B8G8R8A8_UNORM;\n\t\t\tm_bConvert = (m_fmt != D3DFMT_A8R8G8B8 && m_fmt != D3DFMT_X8R8G8B8);\n\t\t\tm_uGpuBytes = m_w * m_h * 4;",
        "\t\t\tm_pSrv = m_pPage->m_pSrv; m_dxgi = m_pPage->m_fmt;\n\t\t\tm_bConvert = (m_pPage->m_bpp == 4) && (m_fmt != D3DFMT_A8R8G8B8 && m_fmt != D3DFMT_X8R8G8B8);\n\t\t\tm_uGpuBytes = m_w * m_h * m_pPage->m_bpp;")
    s = rep(s, "\t\t\tBYTE* pZero = (BYTE*)calloc(1, (size_t)m_w * m_h * 4);\n\t\t\tif (pZero) { D3D11_BOX bz; bz.left = m_ax; bz.top = m_ay; bz.right = m_ax + m_w; bz.bottom = m_ay + m_h; bz.front = 0; bz.back = 1;\n\t\t\t\tm_pDev->m_pCtx->UpdateSubresource(m_pPage->m_pTex, 0, &bz, pZero, m_w * 4, 0); free(pZero); }\n",
        "\t\t\tBYTE* pZero = (BYTE*)calloc(1, (size_t)m_w * m_h * m_pPage->m_bpp);\n\t\t\tif (pZero) { D3D11_BOX bz; bz.left = m_ax; bz.top = m_ay; bz.right = m_ax + m_w; bz.bottom = m_ay + m_h; bz.front = 0; bz.back = 1;\n\t\t\t\tm_pDev->m_pCtx->UpdateSubresource(m_pPage->m_pTex, 0, &bz, pZero, m_w * m_pPage->m_bpp, 0); free(pZero); }\n")
save(p, s); print("OK D3D9on11.cpp")

# ---------------- D3D9on11Atlas.cpp
p, s = load("D3D9on11Atlas.cpp")
if "DXGI_FORMAT fmt" not in s:
    s = rep(s, "\tcase D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8: case D3DFMT_A4R4G4B4: case D3DFMT_R5G6B5: case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5: return true;\n",
        "\tcase D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8: case D3DFMT_A4R4G4B4: case D3DFMT_R5G6B5: case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5: case D3DFMT_A8L8: return true;\n")
    s = rep(s, "CAtlasPage* CAtlasMgr::NewPage(UINT binH)\n{\n\tD3D11_TEXTURE2D_DESC td; memset(&td, 0, sizeof(td));\n\ttd.Width = m_pageSize; td.Height = m_pageSize; td.MipLevels = 1; td.ArraySize = 1; td.Format = DXGI_FORMAT_B8G8R8A8_UNORM; td.SampleDesc.Count = 1;\n",
        "CAtlasPage* CAtlasMgr::NewPage(UINT binH, DXGI_FORMAT fmt)\n{\n\tUINT bpp = (fmt == DXGI_FORMAT_R8G8_UNORM) ? 2 : 4;\t// [r]\n\tD3D11_TEXTURE2D_DESC td; memset(&td, 0, sizeof(td));\n\ttd.Width = m_pageSize; td.Height = m_pageSize; td.MipLevels = 1; td.ArraySize = 1; td.Format = fmt; td.SampleDesc.Count = 1;\n")
    s = rep(s, "\tBYTE* pZero = (BYTE*)calloc(1, (size_t)m_pageSize * m_pageSize * 4);\t// trang tao kem du lieu 0 (khong de rac)\n\tD3D11_SUBRESOURCE_DATA sr; memset(&sr, 0, sizeof(sr)); sr.pSysMem = pZero; sr.SysMemPitch = m_pageSize * 4;\n",
        "\tBYTE* pZero = (BYTE*)calloc(1, (size_t)m_pageSize * m_pageSize * bpp);\t// trang tao kem du lieu 0 (khong de rac)\n\tD3D11_SUBRESOURCE_DATA sr; memset(&sr, 0, sizeof(sr)); sr.pSysMem = pZero; sr.SysMemPitch = m_pageSize * bpp;\n")
    s = rep(s, "\tp->m_pTex = pTex; p->m_pSrv = pSrv; p->m_binH = binH; p->m_rows = m_pageSize / binH; p->m_used = 0;\n", "\tp->m_pTex = pTex; p->m_pSrv = pSrv; p->m_fmt = fmt; p->m_bpp = bpp; p->m_binH = binH; p->m_rows = m_pageSize / binH; p->m_used = 0;\n")
    s = rep(s, "\tg_uRep3AtlasPages++; g_uRep3AtlasBytes += (unsigned __int64)m_pageSize * m_pageSize * 4;\n\treturn p;\n", "\tg_uRep3AtlasPages++; g_uRep3AtlasBytes += (unsigned __int64)m_pageSize * m_pageSize * bpp;\n\treturn p;\n")
    s = rep(s, "bool CAtlasMgr::Alloc(UINT w, UINT h, CAtlasPage** ppPage, UINT* pX, UINT* pY)\n{\n", "bool CAtlasMgr::Alloc(UINT w, UINT h, DXGI_FORMAT fmt, CAtlasPage** ppPage, UINT* pX, UINT* pY)\n{\n")
    s = rep(s, "\t\t\tif (p->m_binH != binH) continue;\n", "\t\t\tif (p->m_binH != binH || p->m_fmt != fmt) continue;\n")
    s = rep(s, "\t\tif (!NewPage(binH)) return false;\t// lan 2: thu lai voi trang moi\n", "\t\tif (!NewPage(binH, fmt)) return false;\t// lan 2: thu lai voi trang moi\n")
    s = rep(s, "\t\t\tif (m_pages[i] != pPage && m_pages[i]->m_binH == pPage->m_binH && m_pages[i]->m_used == 0) nEmptySameClass++;\n", "\t\t\tif (m_pages[i] != pPage && m_pages[i]->m_binH == pPage->m_binH && m_pages[i]->m_fmt == pPage->m_fmt && m_pages[i]->m_used == 0) nEmptySameClass++;\n")
    s = rep(s, "\t\t\tg_uRep3AtlasBytes -= (unsigned __int64)m_pageSize * m_pageSize * 4;\n", "\t\t\tg_uRep3AtlasBytes -= (unsigned __int64)m_pageSize * m_pageSize * pPage->m_bpp;\n")
save(p, s); print("OK D3D9on11Atlas.cpp")

# ---------------- D3D9on11Dev.cpp
p, s = load("D3D9on11Dev.cpp")
if "PALROW" not in s:
    s = rep(s, "static CDev11* g_pRep3Dev11 = NULL;\n", "CDev11* g_pRep3Dev11 = NULL;\n")
    s = rep(s, "m_bRingDiscard = true; m_pAtlas = NULL; m_pDummy = NULL;", "m_bRingDiscard = true; m_pAtlas = NULL; m_pPalTex = NULL; m_pPalSrv = NULL; m_pDummy = NULL;")
    s = rep(s, "\tif (m_pAtlas) { delete m_pAtlas; m_pAtlas = NULL; }\n", "\tPalRelease();\t// [r]\n\tif (m_pAtlas) { delete m_pAtlas; m_pAtlas = NULL; }\n")
    # layout: them PALROW o cuoi dinh (offset = stride D3D9)
    s = rep(s, "\tD3D11_INPUT_ELEMENT_DESC ie[3]; int n = 0; UINT off = 0;\n", "\tD3D11_INPUT_ELEMENT_DESC ie[4]; int n = 0; UINT off = 0;\n")
    s = rep(s, "\tn++;\n\tID3D11InputLayout* pIL = NULL;\n\tHRESULT hr = m_pDev->CreateInputLayout(ie, n, g_Rep3VS11, sizeof(g_Rep3VS11), &pIL);\n",
        "\tn++;\n\tie[n].SemanticName = \"PALROW\"; ie[n].SemanticIndex = 0; ie[n].Format = DXGI_FORMAT_R32_UINT; ie[n].InputSlot = 0; ie[n].AlignedByteOffset = stride; ie[n].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; ie[n].InstanceDataStepRate = 0; n++;\t// [r] hang bang mau, 4 byte sau dinh D3D9\n"
        "\tID3D11InputLayout* pIL = NULL;\n\tHRESULT hr = m_pDev->CreateInputLayout(ie, n, g_Rep3VS11, sizeof(g_Rep3VS11), &pIL);\n")
    s = rep(s, "\tstd::map<DWORD, ID3D11InputLayout*>::iterator it = m_layouts.find(fvf);\n\tif (it != m_layouts.end()) return it->second;\n",
        "\tDWORD keyIL = (fvf & 0xFFFF) | (stride << 16);\t// [r] layout phu thuoc stride (PALROW o cuoi)\n\tstd::map<DWORD, ID3D11InputLayout*>::iterator it = m_layouts.find(keyIL);\n\tif (it != m_layouts.end()) return it->second;\n")
    s = rep(s, "\tm_layouts[fvf] = pIL;\n\treturn pIL;\n", "\tm_layouts[keyIL] = pIL;\n\treturn pIL;\n")
    # pipe bind: SRV bang mau t2
    s = rep(s, "\t\tm_pCtx->OMSetDepthStencilState(m_pDss, 0);\n\t\tm_bPipeBound = true;\n\t}\n\tconst bool v = m_bAppliedValid;\n",
        "\t\tm_pCtx->OMSetDepthStencilState(m_pDss, 0);\n\t\tif (m_pPalSrv) m_pCtx->PSSetShaderResources(2, 1, &m_pPalSrv);\t// [r]\n\t\tm_bPipeBound = true;\n\t}\n\tconst bool v = m_bAppliedValid;\n")
    # FlushBatch: stride 11 = stride + 4
    s = rep(s, "\tUINT nVerts = m_batchVerts, stride = m_batchState.stride, bytes = nVerts * stride;\n", "\tUINT nVerts = m_batchVerts, stride = m_batchState.stride + 4, bytes = nVerts * stride;\t// [r] +4 byte PALROW moi dinh\n")
    # DrawInternal: quad path
    s = rep(s, "\t\tsize_t base = m_batch.size();\n\t\tm_batch.resize(base + 6 * stride);\n\t\tBYTE* d = &m_batch[base];\n\t\tstatic const int s_idx[6] = { 0, 1, 2, 2, 1, 3 };\n\t\tfor (int i = 0; i < 6; i++) memcpy(d + i * stride, pVerts + s_idx[i] * stride, stride);\n\t\tR11AtlasUv(d, 6, stride, m_fvf, m_tex[0], fPage);\n",
        "\t\tconst UINT s11 = stride + 4;\t// [r] +4 byte PALROW\n\t\tconst UINT uPal = (m_tex[0] && m_tex[0]->m_nPalRow >= 0) ? (UINT)m_tex[0]->m_nPalRow : 0xFFFFu;\n"
        "\t\tsize_t base = m_batch.size();\n\t\tm_batch.resize(base + 6 * s11);\n\t\tBYTE* d = &m_batch[base];\n\t\tstatic const int s_idx[6] = { 0, 1, 2, 2, 1, 3 };\n"
        "\t\tfor (int i = 0; i < 6; i++) { memcpy(d + i * s11, pVerts + s_idx[i] * stride, stride); *(UINT*)(d + i * s11 + stride) = uPal; }\n"
        "\t\tR11AtlasUv(d, 6, s11, m_fvf, m_tex[0], fPage);\n")
    # immediate path: chep sang tmp voi stride 11
    s = rep(s, "\tstd::vector<BYTE> tmp;\n\tif (type == D3DPT_TRIANGLEFAN)\n\t{\n\t\tUINT nTri = nVerts - 2;\n\t\ttmp.resize((size_t)nTri * 3 * stride);\n\t\tfor (UINT i = 0; i < nTri; i++)\n\t\t{\n\t\t\tmemcpy(&tmp[(i * 3 + 0) * stride], pVerts, stride);\n\t\t\tmemcpy(&tmp[(i * 3 + 1) * stride], pVerts + (i + 1) * stride, stride);\n\t\t\tmemcpy(&tmp[(i * 3 + 2) * stride], pVerts + (i + 2) * stride, stride);\n\t\t}\n\t\tpVerts = &tmp[0]; nVerts = nTri * 3; type = D3DPT_TRIANGLELIST;\n\t}\n\telse\n\t{\n\t\ttmp.assign(pVerts, pVerts + (size_t)nVerts * stride);\n\t\tpVerts = &tmp[0];\n\t}\n\tR11AtlasUv(&tmp[0], nVerts, stride, m_fvf, m_tex[0], fPage);\n\tUINT bytes = nVerts * stride;\n",
        "\tstd::vector<BYTE> tmp;\n\tconst UINT s11 = stride + 4;\t// [r] +4 byte PALROW\n\tconst UINT uPal = (m_tex[0] && m_tex[0]->m_nPalRow >= 0) ? (UINT)m_tex[0]->m_nPalRow : 0xFFFFu;\n"
        "\tif (type == D3DPT_TRIANGLEFAN)\n\t{\n\t\tUINT nTri = nVerts - 2;\n\t\ttmp.resize((size_t)nTri * 3 * s11);\n\t\tfor (UINT i = 0; i < nTri; i++)\n\t\t{\n\t\t\tconst UINT src[3] = { 0, i + 1, i + 2 };\n\t\t\tfor (int k = 0; k < 3; k++) { memcpy(&tmp[(i * 3 + k) * s11], pVerts + src[k] * stride, stride); *(UINT*)(&tmp[(i * 3 + k) * s11 + stride]) = uPal; }\n\t\t}\n\t\tnVerts = nTri * 3; type = D3DPT_TRIANGLELIST;\n\t}\n\telse\n\t{\n\t\ttmp.resize((size_t)nVerts * s11);\n\t\tfor (UINT i = 0; i < nVerts; i++) { memcpy(&tmp[i * s11], pVerts + i * stride, stride); *(UINT*)(&tmp[i * s11 + stride]) = uPal; }\n\t}\n"
        "\tR11AtlasUv(&tmp[0], nVerts, s11, m_fvf, m_tex[0], fPage);\n\tUINT bytes = nVerts * s11;\n")
    s = rep(s, "\tUINT pos = 0;\n\tUploadRing(pVerts, bytes, stride, &pos);\n\tif (pos == 0xFFFFFFFF) return D3DERR_INVALIDCALL;\n\tR11Applied a;\n\tComputeApplied(a, pIL, stride);\n\tApplyComputed(a);\n\tID3D11Buffer* bufs[2] = { m_pRing, m_pDummy }; UINT strides[2] = { stride, 16 }; UINT offs[2] = { pos, 0 };\n",
        "\tUINT pos = 0;\n\tUploadRing(&tmp[0], bytes, s11, &pos);\n\tif (pos == 0xFFFFFFFF) return D3DERR_INVALIDCALL;\n\tR11Applied a;\n\tComputeApplied(a, pIL, stride);\n\tApplyComputed(a);\n\tID3D11Buffer* bufs[2] = { m_pRing, m_pDummy }; UINT strides[2] = { s11, 16 }; UINT offs[2] = { pos, 0 };\n")
    # Present: hang bang mau thu tro lai
    s = rep(s, "\tm_bRtBound = false; m_bWaitedThisFrame = false;\n\tm_ringPos = 0; m_bRingDiscard = true;\n", "\tm_bRtBound = false; m_bWaitedThisFrame = false;\n\tm_ringPos = 0; m_bRingDiscard = true;\n\tPalFrameEnd();\t// [r] hang bang mau thu trong khung -> dung lai duoc\n")
save(p, s); print("OK D3D9on11Dev.cpp")

# ---------------- shader
p, s = load("Rep3Shaders11.hlsl")
if "PALROW" not in s:
    s = rep(s, "struct VSIn  { float4 pos : POSITION; float4 col : COLOR0; float2 uv : TEXCOORD0; };\nstruct VSOut { float4 pos : SV_Position; float4 col : COLOR0; float2 uv : TEXCOORD0; };\n",
        "struct VSIn  { float4 pos : POSITION; float4 col : COLOR0; float2 uv : TEXCOORD0; uint palrow : PALROW; };\nstruct VSOut { float4 pos : SV_Position; float4 col : COLOR0; float2 uv : TEXCOORD0; nointerpolation uint palrow : PALROW; };\n")
    s = rep(s, "    o.col = i.col;\n    o.uv = i.uv;\n    return o;\n", "    o.col = i.col;\n    o.uv = i.uv;\n    o.palrow = i.palrow;\n    return o;\n")
    s = rep(s, "Texture2D    g_t1 : register(t1);\nSamplerState g_s1 : register(s1);\n", "Texture2D    g_t1 : register(t1);\nSamplerState g_s1 : register(s1);\nTexture2D    g_pal : register(t2);   // [r] atlas bang mau 256 x N (BGRA8), hang = palrow\n")
    s = rep(s, "        float4 tex0 = (g_st0b.z != 0) ? g_t0.Sample(g_s0, i.uv) : float4(1, 1, 1, 1);\n",
        "        float4 tex0 = (g_st0b.z != 0) ? g_t0.Sample(g_s0, i.uv) : float4(1, 1, 1, 1);\n"
        "        if (i.palrow != 0xFFFFu && g_st0b.z != 0)\n        {   // [r] texture chi so (R8G8): R = chi so bang mau, G = alpha\n"
        "            uint idx = (uint)(tex0.r * 255.0 + 0.5);\n"
        "            float4 c = g_pal.Load(int3(idx, i.palrow, 0));\n"
        "            tex0 = float4(c.rgb, tex0.g);\n        }\n")
save(p, s); print("OK Rep3Shaders11.hlsl")

# ---------------- TextureRes.h / .cpp (latin-1 CRLF)
p, s = load("TextureRes.h"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "m_nPalRow" not in s:
    s = rep(s, "\tWORD*\t\tm_pPal16;", "\tint\t\t\tm_nPalRow;\t\t\t\t\t// [D3D11 08/09 r] hang bang mau trong atlas bang mau (-1 = chua cap)\r\n\tWORD*\t\tm_pPal16;")
save(p, s, h0); print("OK TextureRes.h")

p, s = load("TextureRes.cpp"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "RenderToIndexAlpha" not in s:
    s = rep(s, "// [REP3 03/09 RAM2] giai RLE spr -> A4R4G4B4, CO KIEM BIEN.",
        "// [D3D11 08/09 r] giai RLE spr -> (chi so 8 bit, alpha 8 bit) = D3DFMT_A8L8 (L = chi so o byte thap, A o byte cao). Khong bung mau:\r\n"
        "// mau tra bang trong shader (D3D9on11Pal.cpp) -> y het 8888, VRAM mot nua.\r\n"
        "static void RenderToIndexAlpha(WORD* pDest, BYTE* pSrc, int nSrcLen, int nTotal, int nColors)\r\n{\r\n"
        "\tBYTE*  p    = pSrc;\r\n\tBYTE*  pEnd = pSrc + nSrcLen;\r\n\tWORD*  d    = pDest;\r\n\tWORD*  dEnd = pDest + nTotal;\r\n"
        "\twhile (p + 2 <= pEnd && d < dEnd)\r\n\t{\r\n\t\tint n = *p++;\r\n\t\tint a = *p++;\r\n"
        "\t\tif (a == 0)\r\n\t\t{\r\n\t\t\tfor (int k = 0; k < n && d < dEnd; k++)\r\n\t\t\t\t*d++ = 0;\r\n\t\t}\r\n"
        "\t\telse\r\n\t\t{\r\n\t\t\tfor (int k = 0; k < n && d < dEnd; k++)\r\n\t\t\t{\r\n"
        "\t\t\t\tif (p >= pEnd) { *d++ = 0; continue; }\r\n\t\t\t\tint idx = *p++;\r\n\t\t\t\tif (idx >= nColors) idx = 0;\r\n"
        "\t\t\t\t*d++ = (WORD)(((DWORD)a << 8) | (DWORD)idx);\r\n\t\t\t}\r\n\t\t}\r\n\t}\r\n"
        "\twhile (d < dEnd)\r\n\t\t*d++ = 0;\r\n}\r\n\r\n"
        "// [REP3 03/09 RAM2] giai RLE spr -> A4R4G4B4, CO KIEM BIEN.")
    s = rep(s, "void TextureResSpr::ResetVar()\r\n{\r\n", "void TextureResSpr::ResetVar()\r\n{\r\n\tm_nPalRow\t\t= -1;\t// [D3D11 08/09 r]\r\n")
    s = rep(s, "void TextureResSpr::Release()\r\n{\r\n\tSAFE_DELETE_ARRAY(m_pPal24);\r\n", "void TextureResSpr::Release()\r\n{\r\n\tif (m_nPalRow >= 0) { Rep3_D3D11FreePalette(m_nPalRow); m_nPalRow = -1; }\t// [D3D11 08/09 r] tra hang bang mau\r\n\tSAFE_DELETE_ARRAY(m_pPal24);\r\n")
    s = rep(s, "\tint nBpp = g_nRep3Tex32 ? 4 : 2;\r\n\tD3DFORMAT eFmt = g_nRep3Tex32 ? D3DFMT_A8R8G8B8 : D3DFMT_A4R4G4B4;\r\n",
        "\tint nBpp = g_nRep3Tex32 ? 4 : 2;\r\n\tD3DFORMAT eFmt = g_nRep3Tex32 ? D3DFMT_A8R8G8B8 : D3DFMT_A4R4G4B4;\r\n"
        "\t// [D3D11 08/09 r] bang mau: 2 byte/diem (chi so + alpha), mau tra bang trong shader = y het 8888\r\n"
        "\tbool bPal = false;\r\n"
        "\tif (g_nRep3Pal && g_nRep3ApiOn == 11 && g_nRep3Pool && m_pPal24 && Rep3_D3D11PaletteOK())\r\n"
        "\t{\r\n\t\tif (m_nPalRow < 0)\r\n\t\t\tm_nPalRow = Rep3_D3D11AllocPalette((const unsigned char*)m_pPal24, (int)m_nColors);\r\n"
        "\t\tif (m_nPalRow >= 0) { bPal = true; nBpp = 2; eFmt = D3DFMT_A8L8; }\r\n\t}\r\n")
    s = rep(s, "\tif (g_nRep3Tex32)\r\n\t\tRenderToA8R8G8B8((DWORD*)pTempData, m_pFrameInfo[nFrame].pRawData, m_pFrameInfo[nFrame].nRawDataLen,\r\n",
        "\tif (bPal)\r\n\t\tRenderToIndexAlpha((WORD*)pTempData, m_pFrameInfo[nFrame].pRawData, m_pFrameInfo[nFrame].nRawDataLen, nW * nH, (int)m_nColors);\r\n"
        "\telse if (g_nRep3Tex32)\r\n\t\tRenderToA8R8G8B8((DWORD*)pTempData, m_pFrameInfo[nFrame].pRawData, m_pFrameInfo[nFrame].nRawDataLen,\r\n")
    s = rep(s, "\t\t\tpFill->Release();\r\n\t\t\tti.pTexture = pVram;\r\n\t\t}\r\n", "\t\t\tpFill->Release();\r\n\t\t\tti.pTexture = pVram;\r\n\t\t\tif (bPal) Rep3_D3D11TagPalette(pVram, m_nPalRow);\t// [D3D11 08/09 r]\r\n\t\t}\r\n")
save(p, s, h0); print("OK TextureRes.cpp")

# ---------------- KRepresentShell3.cpp / BaseInclude.h / vcxproj
p, s = load("KRepresentShell3.cpp"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3Pal" not in s:
    s = rep(s, "int  g_nRep3Waitable  = 0;", "int  g_nRep3Pal       = 1;\t// [D3D11 08/09 r] texture sprite bang mau 2 B/px (chi D3D11)\r\nint  g_nRep3Waitable  = 0;")
    s = rep(s, '\tg_nRep3Waitable  = Rep3Ini("Rep3Waitable", 0);\t// [D3D11 08/09 o]\r\n', '\tg_nRep3Waitable  = Rep3Ini("Rep3Waitable", 0);\t// [D3D11 08/09 o]\r\n\tg_nRep3Pal       = Rep3Ini("Rep3Pal", 1);\t// [D3D11 08/09 r]\r\n')
    s = rep(s, "gop %u quad -> %u Draw\",\r\n", "gop %u quad -> %u Draw | pal %u hang\",\r\n")
    s = rep(s, "g_uRep3BatchQuads, g_uRep3BatchDraws);\r\n", "g_uRep3BatchQuads, g_uRep3BatchDraws, g_uRep3PalRows);\r\n")
save(p, s, h0); print("OK KRepresentShell3.cpp")

p, s = load("BaseInclude.h"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3Pal" not in s:
    s = rep(s, "extern int  g_nRep3Waitable;\t// [D3D11 08/09 n]",
        "extern int  g_nRep3Pal;\t// [D3D11 08/09 r]\r\nextern unsigned g_uRep3PalRows;\r\nstruct IDirect3DTexture9;\r\n"
        "int  Rep3_D3D11PaletteOK(); int Rep3_D3D11AllocPalette(const unsigned char* pPal24, int nColors); void Rep3_D3D11FreePalette(int nRow); void Rep3_D3D11TagPalette(IDirect3DTexture9* pTex, int nRow);\r\n"
        "extern int  g_nRep3Waitable;\t// [D3D11 08/09 n]")
save(p, s, h0); print("OK BaseInclude.h")

p, s = load("Represent3.vcxproj"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "D3D9on11Pal.cpp" not in s:
    s = rep(s, '    <ClCompile Include="D3D9on11Atlas.cpp" />\r\n', '    <ClCompile Include="D3D9on11Atlas.cpp" />\r\n    <ClCompile Include="D3D9on11Pal.cpp" />\r\n')
save(p, s, h0); print("OK vcxproj")
print("XONG")
