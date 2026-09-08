# -*- coding: ascii -*-
"""goi_va_d3d11c_ring_atlas_0809.py - [D3D11 08/09 c+d]
 c) ring dinh: DISCARD dau moi khung (sau Present) - truoc day NO_OVERWRITE de len dinh GPU con doc -> o den / giat den.
 d) atlas: texture sprite nho (DEFAULT, <=512, khong RT) = o trong trang 1024x1024 BGRA8 (D3D9on11Atlas.cpp); uv nhan/dich khi chep dinh.
Cac tep D3D9on11*.cpp/.h la tep cua toi (ASCII, LF). KRepresentShell3.cpp/BaseInclude.h/vcxproj: latin-1, CRLF."""
import io, re, sys
ROOT = r"D:\GAMEDEVNEW_wt_delta\Sources\Represent\Represent3"

def load(name):
    p = ROOT + "\\" + name
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    if name.startswith("D3D9on11"):
        s = s.replace("\r\n", "\n")     # tep cua toi: Edit tool da doi sang CRLF, chuan hoa ve LF de neo khop
    return p, s
def save(p, s, h0=None):
    if h0 is not None and sum(1 for ch in s if ord(ch) >= 0x80) != h0: print("FAIL byte cao", p); sys.exit(1)
    io.open(p, "w", encoding="latin-1", newline="").write(s)
def rep(s, old, new, n=1):
    c = s.count(old)
    if c != n: print("FAIL neo %r: thay %d, can %d" % (old[:70], c, n)); sys.exit(1)
    return s.replace(old, new)
T = "\t"; N = "\n"

# ---------------- D3D9on11i.h
p, s = load("D3D9on11i.h")
if "class CAtlasMgr" not in s:
    s = rep(s, "class CDev11;\nclass CTex11;\nclass CSurf11;\n",
        "class CDev11;\nclass CTex11;\nclass CSurf11;\nclass CAtlasPage;\nclass CAtlasMgr;\n")
    s = rep(s, "// ---------------------------------------------------------------- texture\nclass CTex11 : public IDirect3DTexture9\n",
        "// ---------------------------------------------------------------- atlas [D3D11 08/09 d]\n"
        "class CAtlasPage\n{\npublic:\n"
        "\tID3D11Texture2D* m_pTex; ID3D11ShaderResourceView* m_pSrv;\n"
        "\tUINT m_binW, m_binH, m_cols, m_rows, m_used;\n"
        "\tstd::vector<UINT> m_free;\n};\n"
        "class CAtlasMgr\n{\npublic:\n"
        "\tCAtlasMgr(CDev11* pDev);\n\t~CAtlasMgr();\n"
        "\tstatic bool Eligible(UINT w, UINT h, DWORD usage, D3DFORMAT fmt, D3DPOOL pool);\n"
        "\tbool Alloc(UINT w, UINT h, CAtlasPage** ppPage, UINT* pSlot, UINT* pX, UINT* pY);\n"
        "\tvoid Free(CAtlasPage* pPage, UINT slot);\n"
        "\tvoid ReleaseAll();\n"
        "\tCAtlasPage* NewPage(UINT binW, UINT binH);\n"
        "\tCDev11* m_pDev; std::vector<CAtlasPage*> m_pages; UINT m_pageSize;\n};\n\n"
        "// ---------------------------------------------------------------- texture\nclass CTex11 : public IDirect3DTexture9\n")
    s = rep(s, "\tUINT        m_uGpuBytes;\n\tCSurf11*    m_pSurf0;			// mat level 0 (khong giu ref; surface giu ref texture)\n",
        "\tUINT        m_uGpuBytes;\n\tCSurf11*    m_pSurf0;			// mat level 0 (khong giu ref; surface giu ref texture)\n"
        "\tbool        m_bVirtual;			// [d] o trong trang atlas (m_pSrv = SRV cua trang, khong so huu)\n"
        "\tCAtlasPage* m_pPage; UINT m_slot, m_ax, m_ay;\n")
    s = rep(s, "\tbool            m_bRingDiscard;", "\tbool            m_bRingDiscard;\n\tCAtlasMgr*      m_pAtlas;				// [d] NULL = tat")
save(p, s); print("OK D3D9on11i.h")

# ---------------- D3D9on11.cpp (CTex11)
p, s = load("D3D9on11.cpp")
if "m_bVirtual = false" not in s:
    s = rep(s, "\tm_dxgi = DXGI_FORMAT_UNKNOWN; m_bConvert = false; m_bDirty = false; m_bLocked = false; m_uGpuBytes = 0; m_pSurf0 = NULL;\n",
        "\tm_dxgi = DXGI_FORMAT_UNKNOWN; m_bConvert = false; m_bDirty = false; m_bLocked = false; m_uGpuBytes = 0; m_pSurf0 = NULL;\n"
        "\tm_bVirtual = false; m_pPage = NULL; m_slot = 0; m_ax = 0; m_ay = 0;\n")
    # ReleaseGpu: texture ao
    s = rep(s, "void CTex11::ReleaseGpu()\n{\n\tif (m_pRtv) { m_pRtv->Release(); m_pRtv = NULL; }\n",
        "void CTex11::ReleaseGpu()\n{\n"
        "\tif (m_bVirtual)\n\t{\n"
        "\t\tif (m_pPage && m_pDev->m_pAtlas) m_pDev->m_pAtlas->Free(m_pPage, m_slot);\n"
        "\t\tm_pPage = NULL; m_pSrv = NULL;\t// SRV cua trang, khong so huu\n"
        "\t\tif (m_uGpuBytes) { if (g_uRep3GpuTexCount) g_uRep3GpuTexCount--; g_uRep3GpuTexBytes -= m_uGpuBytes; m_uGpuBytes = 0; }\n"
        "\t\treturn;\n\t}\n"
        "\tif (m_pRtv) { m_pRtv->Release(); m_pRtv = NULL; }\n")
    # EnsureGpu: nhanh ao
    s = rep(s, "HRESULT CTex11::EnsureGpu(const BYTE* pInit)\n{\n\tif (m_pGpu) return D3D_OK;\n",
        "HRESULT CTex11::EnsureGpu(const BYTE* pInit)\n{\n\tif (m_pGpu) return D3D_OK;\n"
        "\tif (m_bVirtual)\n\t{\n"
        "\t\tif (m_pPage) return D3D_OK;\n"
        "\t\tif (!m_pDev->m_pAtlas || !m_pDev->m_pAtlas->Alloc(m_w, m_h, &m_pPage, &m_slot, &m_ax, &m_ay))\n"
        "\t\t{\n\t\t\tm_bVirtual = false;\t// het cach: texture rieng\n\t\t}\n"
        "\t\telse\n\t\t{\n"
        "\t\t\tm_pSrv = m_pPage->m_pSrv; m_dxgi = DXGI_FORMAT_B8G8R8A8_UNORM;\n"
        "\t\t\tm_bConvert = (m_fmt != D3DFMT_A8R8G8B8 && m_fmt != D3DFMT_X8R8G8B8);\n"
        "\t\t\tm_uGpuBytes = m_w * m_h * 4; g_uRep3GpuTexCount++; g_uRep3GpuTexBytes += m_uGpuBytes;\n"
        "\t\t\tm_bDirty = false;\n"
        "\t\t\tif (pInit) { BYTE* pSave = m_pCpu; m_pCpu = (BYTE*)pInit; HRESULT hrU = UploadRect(NULL); m_pCpu = pSave; return hrU; }\n"
        "\t\t\t// khong co du lieu: xoa o (tranh rac cua texture cu)\n"
        "\t\t\tBYTE* pZero = (BYTE*)calloc(1, (size_t)m_w * m_h * 4);\n"
        "\t\t\tif (pZero) { D3D11_BOX bz; bz.left = m_ax; bz.top = m_ay; bz.right = m_ax + m_w; bz.bottom = m_ay + m_h; bz.front = 0; bz.back = 1;\n"
        "\t\t\t\tm_pDev->m_pCtx->UpdateSubresource(m_pPage->m_pTex, 0, &bz, pZero, m_w * 4, 0); free(pZero); }\n"
        "\t\t\treturn D3D_OK;\n\t\t}\n\t}\n")
    # UploadRect: ao -> box dich theo o
    s = rep(s, "\tif (!m_pCpu) return D3DERR_INVALIDCALL;\n\tif (!m_pGpu) return EnsureGpu(m_pCpu);\n\tRECT rc = { 0, 0, (LONG)m_w, (LONG)m_h };\n",
        "\tif (!m_pCpu) return D3DERR_INVALIDCALL;\n"
        "\tif (m_bVirtual && !m_pPage) return EnsureGpu(m_pCpu);\n"
        "\tif (!m_bVirtual && !m_pGpu) return EnsureGpu(m_pCpu);\n"
        "\tRECT rc = { 0, 0, (LONG)m_w, (LONG)m_h };\n")
    s = rep(s, "\tD3D11_BOX box; box.left = rc.left; box.top = rc.top; box.right = rc.right; box.bottom = rc.bottom; box.front = 0; box.back = 1;\n\tUINT w = rc.right - rc.left, h = rc.bottom - rc.top;\n",
        "\tUINT ox = m_bVirtual ? m_ax : 0, oy = m_bVirtual ? m_ay : 0;\n"
        "\tID3D11Texture2D* pDst = m_bVirtual ? m_pPage->m_pTex : m_pGpu;\n"
        "\tD3D11_BOX box; box.left = ox + rc.left; box.top = oy + rc.top; box.right = ox + rc.right; box.bottom = oy + rc.bottom; box.front = 0; box.back = 1;\n"
        "\tUINT w = rc.right - rc.left, h = rc.bottom - rc.top;\n")
    s = rep(s, "\t\tm_pDev->m_pCtx->UpdateSubresource(m_pGpu, 0, &box, pConv, w * 4, 0);\n\t\tfree(pConv);\n\t}\n\telse\n\t\tm_pDev->m_pCtx->UpdateSubresource(m_pGpu, 0, &box, m_pCpu + rc.top * m_pitch + rc.left * fi.bpp, m_pitch, 0);\n",
        "\t\tm_pDev->m_pCtx->UpdateSubresource(pDst, 0, &box, pConv, w * 4, 0);\n\t\tfree(pConv);\n\t}\n\telse\n\t\tm_pDev->m_pCtx->UpdateSubresource(pDst, 0, &box, m_pCpu + rc.top * m_pitch + rc.left * fi.bpp, m_pitch, 0);\n")
    # PrepareForBind: ao chua co trang
    s = rep(s, "HRESULT CTex11::PrepareForBind()\n{\n\tif (!m_pGpu)\n",
        "HRESULT CTex11::PrepareForBind()\n{\n"
        "\tif (m_bVirtual)\n\t{\n"
        "\t\tif (!m_pPage) { HRESULT hr = EnsureGpu(m_pCpu); m_bDirty = false; return hr; }\n"
        "\t\tif (m_bDirty && m_pCpu) { HRESULT hr = UploadRect(&m_rcDirty); m_bDirty = false; return hr; }\n"
        "\t\treturn D3D_OK;\n\t}\n"
        "\tif (!m_pGpu)\n")
    # UnlockRect: 'if (m_pGpu)' -> co trang
    s = rep(s, "\tm_bDirty = true;\n\tif (m_pGpu)\n\t{\n\t\tm_pDev->Lock();\n\t\tUploadRect(&m_rcDirty);\n",
        "\tm_bDirty = true;\n\tif (m_pGpu || (m_bVirtual && m_pPage))\n\t{\n\t\tm_pDev->Lock();\n\t\tUploadRect(&m_rcDirty);\n")
save(p, s); print("OK D3D9on11.cpp")

# ---------------- D3D9on11Dev.cpp
p, s = load("D3D9on11Dev.cpp")
if "m_bRingDiscard = true; m_pDummy" not in s:
    s = rep(s, "m_ringSize = R11_RING_SIZE; m_ringPos = 0; m_pDummy = NULL;", "m_ringSize = R11_RING_SIZE; m_ringPos = 0; m_bRingDiscard = true; m_pDummy = NULL;")
    s = rep(s, "\tm_bRtBound = false;\n\tm_ringPos = 0;\n\tQueryPerformanceCounter(&t1);", "\tm_bRtBound = false;\n\tm_ringPos = 0; m_bRingDiscard = true;\n\tQueryPerformanceCounter(&t1);")
    s = rep(s, "\tif (pos + bytes > m_ringSize) { pos = 0; mapType = D3D11_MAP_WRITE_DISCARD; }",
        "\t// [D3D11 08/09 c] dau moi khung (sau Present) hoac het ring: DISCARD de driver cap vung moi. Truoc day sau Present dat m_ringPos = 0\n"
        "\t// roi ghi NO_OVERWRITE de len dinh GPU con dang doc cua khung truoc -> o den, manh rac, giat den khi di chuyen / qua map.\n"
        "\tif (m_bRingDiscard || pos + bytes > m_ringSize) { pos = 0; mapType = D3D11_MAP_WRITE_DISCARD; m_bRingDiscard = false; }")
if "m_pAtlas = NULL" not in s:
    s = rep(s, " m_bRingDiscard = true; m_pDummy = NULL;", " m_bRingDiscard = true; m_pAtlas = NULL; m_pDummy = NULL;")
    # dtor: atlas sau khi thu texture stage, truoc doi tuong pipeline
    s = rep(s, "\tR11_SAFE_RELEASE(m_pDss); R11_SAFE_RELEASE(m_pDummy);", "\tif (m_pAtlas) { delete m_pAtlas; m_pAtlas = NULL; }\n\tR11_SAFE_RELEASE(m_pDss); R11_SAFE_RELEASE(m_pDummy);")
    # Init: tao atlas
    s = rep(s, "\tif (!CreatePipelineObjects()) return false;\n", "\tif (!CreatePipelineObjects()) return false;\n\tif (g_nRep3Atlas) m_pAtlas = new CAtlasMgr(this);\n")
    s = rep(s, "(int)(m_pp.PresentationInterval != D3DPRESENT_INTERVAL_IMMEDIATE), (int)m_bTearing);",
        "(int)(m_pp.PresentationInterval != D3DPRESENT_INTERVAL_IMMEDIATE), (int)m_bTearing);\n\tR11Log(\"atlas: %s\", m_pAtlas ? \"BAT (trang 1024x1024 BGRA8, texture <= 512 khong RT)\" : \"tat\");")
    # CreateTexture: danh dau ao
    s = rep(s, "\tCTex11* pTex = new CTex11(this, Width, Height, Usage, Format, Pool);\n\tLock();\n\tHRESULT hr = D3D_OK;\n",
        "\tCTex11* pTex = new CTex11(this, Width, Height, Usage, Format, Pool);\n"
        "\tif (m_pAtlas && CAtlasMgr::Eligible(Width, Height, Usage, Format, Pool)) pTex->m_bVirtual = true;\t// [d]\n"
        "\tLock();\n\tHRESULT hr = D3D_OK;\n")
    # UpdateTexture: dst ao (khong GPU rieng) -> EnsureGpu(src)
    s = rep(s, "\telse if (!pD->m_pGpu)\n\t\thr = pD->EnsureGpu(pS->m_pCpu);				// duong chinh: tao texture GPU kem du lieu (khong ban sao RAM)\n",
        "\telse if (pD->m_bVirtual)\n"
        "\t{\n\t\tif (!pD->m_pPage) hr = pD->EnsureGpu(pS->m_pCpu);\n"
        "\t\telse { BYTE* pSave = pD->m_pCpu; pD->m_pCpu = pS->m_pCpu; hr = pD->UploadRect(NULL); pD->m_pCpu = pSave; }\n\t}\n"
        "\telse if (!pD->m_pGpu)\n\t\thr = pD->EnsureGpu(pS->m_pCpu);				// duong chinh: tao texture GPU kem du lieu (khong ban sao RAM)\n")
    # DrawInternal: uv nhan/dich khi texture stage 0 la ao
    s = rep(s, "\tmemcpy((BYTE*)ms.pData + pos, pVerts, bytes);\n\tm_pCtx->Unmap(m_pRing, 0);\n",
        "\tmemcpy((BYTE*)ms.pData + pos, pVerts, bytes);\n"
        "\tif (m_tex[0] && m_tex[0]->m_bVirtual && m_tex[0]->m_pPage && ((m_fvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT) >= 1)\n"
        "\t{\t// [d] uv cua texture ao -> uv trong trang\n"
        "\t\tUINT uvOff = ((m_fvf & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW) ? 16 : 12;\n"
        "\t\tDWORD pt = m_fvf & D3DFVF_POSITION_MASK;\n"
        "\t\tif (pt == D3DFVF_XYZB1) uvOff += 4; else if (pt == D3DFVF_XYZB2) uvOff += 8; else if (pt == D3DFVF_XYZB3) uvOff += 12; else if (pt == D3DFVF_XYZB4) uvOff += 16; else if (pt == D3DFVF_XYZB5) uvOff += 20;\n"
        "\t\tif (m_fvf & D3DFVF_NORMAL) uvOff += 12; if (m_fvf & D3DFVF_PSIZE) uvOff += 4; if (m_fvf & D3DFVF_DIFFUSE) uvOff += 4; if (m_fvf & D3DFVF_SPECULAR) uvOff += 4;\n"
        "\t\tconst float fPage = (float)m_pAtlas->m_pageSize;\n"
        "\t\tconst float sx = (float)m_tex[0]->m_w / fPage, sy = (float)m_tex[0]->m_h / fPage, ox = (float)m_tex[0]->m_ax / fPage, oy = (float)m_tex[0]->m_ay / fPage;\n"
        "\t\tBYTE* pV = (BYTE*)ms.pData + pos;\n"
        "\t\tfor (UINT i = 0; i < nVerts; i++) { float* uv = (float*)(pV + i * stride + uvOff); uv[0] = uv[0] * sx + ox; uv[1] = uv[1] * sy + oy; }\n"
        "\t}\n"
        "\tm_pCtx->Unmap(m_pRing, 0);\n")
save(p, s); print("OK D3D9on11Dev.cpp")

# ---------------- D3D9on11.h: extern
p, s = load("D3D9on11.h")
if "g_nRep3Atlas" not in s:
    s = rep(s, "extern int      g_nRep3Flip;", "extern int      g_nRep3Atlas;        // [Client] Rep3Atlas: 1 = gom texture nho vao trang (mac dinh), 0 = tat\nextern unsigned g_uRep3AtlasPages;   // so trang atlas\nextern unsigned __int64 g_uRep3AtlasBytes;\nextern int      g_nRep3Flip;")
save(p, s); print("OK D3D9on11.h")

# ---------------- KRepresentShell3.cpp (latin-1, CRLF)
p, s = load("KRepresentShell3.cpp"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3Atlas" not in s:
    s = rep(s, "int  g_nRep3Flip      = 0;", "int  g_nRep3Atlas     = 1;\t// [D3D11 08/09 d] gom texture nho vao trang atlas (chi khi Rep3Api=11)\r\nint  g_nRep3Flip      = 0;")
    s = rep(s, '\tg_nRep3Flip      = Rep3Ini("Rep3Flip", 0);', '\tg_nRep3Atlas     = Rep3Ini("Rep3Atlas", 1);\t// [D3D11 08/09 d]\r\n\tg_nRep3Flip      = Rep3Ini("Rep3Flip", 0);')
    s = rep(s, "| gpu tex %u (%u MB) | d3d11:", "| gpu tex %u (%u MB, %u trang %u MB) | d3d11:")
    s = rep(s, "g_uRep3GpuTexCount, (unsigned)(g_uRep3GpuTexBytes >> 20),\r\n", "g_uRep3GpuTexCount, (unsigned)(g_uRep3GpuTexBytes >> 20), g_uRep3AtlasPages, (unsigned)(g_uRep3AtlasBytes >> 20),\r\n")
save(p, s, h0); print("OK KRepresentShell3.cpp")

# ---------------- BaseInclude.h
p, s = load("BaseInclude.h"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "g_nRep3Atlas" not in s:
    s = rep(s, "extern int  g_nRep3Flip;", "extern int  g_nRep3Atlas;       // [D3D11 08/09 d] gom texture nho vao trang atlas\r\nextern unsigned g_uRep3AtlasPages; extern unsigned __int64 g_uRep3AtlasBytes;\r\nextern int  g_nRep3Flip;")
save(p, s, h0); print("OK BaseInclude.h")

# ---------------- vcxproj
p, s = load("Represent3.vcxproj"); h0 = sum(1 for ch in s if ord(ch) >= 0x80)
if "D3D9on11Atlas.cpp" not in s:
    s = rep(s, '    <ClCompile Include="D3D9on11D3D.cpp" />\r\n', '    <ClCompile Include="D3D9on11D3D.cpp" />\r\n    <ClCompile Include="D3D9on11Atlas.cpp" />\r\n')
save(p, s, h0); print("OK vcxproj")
print("XONG")
