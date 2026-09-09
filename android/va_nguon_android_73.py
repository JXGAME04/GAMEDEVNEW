# -*- coding: utf-8 -*-
#
# [GPU 11/09 ATLAS/BOCPU] Lop D3D9-tren-SDL_GPU (mobile, Rep3Api=100) - giam lag Tong Kim, buoc 2 (GPU + RAM):
#  (1) ATLAS: gom texture sprite nho (POOL_DEFAULT, <= 512 px) vao trang 1024x1024 nhu lop D3D11 tren PC
#      (D3D9on11Atlas.cpp, [D3D11 08/09 d/i]) - lop SDL_GPU truoc day KHONG co: moi khung sprite / moi o cat la MOT
#      SDL_GPUTexture rieng (vkCreateImage + cap bo nho + upload + rang buoc rieng) va MOT lenh ve rieng. Tong Kim
#      = hang tram nguoi x nhieu khung -> hang nghin texture moi moi giay tren Vulkan (LDPlayer dich Vulkan sang may
#      that cang cham). Trang atlas: vai tram doi tuong GPU thay vi hang van, quad ke nhau cung trang -> gop lenh.
#  (2) BOCPU: texture POOL_DEFAULT (sprite) sau khi da tai len GPU thi BO ban CPU (truoc: lop nay giu ban CPU cua
#      MOI texture -> RAM = 2 x cache texture; dien thoai khong co VRAM rieng). LockRect sau nay (hiem: sprite chi
#      ghi mot lan) doc lai tu GPU (ReadbackRegion, dong bo).
#  (3) Ngan sach cache texture (TextureResMgr::SetBudget) tren Android: kep <= RAM/3 ke ca khi [Client] Rep3CacheMB ep
#      (config Android dang de 1500 MB chep tu PC 32 GB RAM); config Android chuyen Rep3CacheMB=0 (tu tinh).
#  Cong tac [Client] Rep3AtlasGpu / Rep3GpuBoBanCpu: mac dinh 1 tren Android, 0 tren Windows (GameSDL.exe khong doi).
#  Chi trong tep JX_PLATFORM_SDL (D3D9onGPU*) + rao JX_PLATFORM_SDL/JX_POSIX -> ban PC (D3D11/D3D9) khong doi.

import io
import os

os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
DAU = "[GPU 11/09 ATLAS]"
DAU2 = "[GPU 11/09 BOCPU]"


def doc(p):
    return io.open(p, encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def thay(s, cu, moi, ten, so=1):
    nl = "\r\n" if s.count("\r\n") > s.count("\n") // 2 else "\n"
    c = nl.join(cu)
    if s.count(c) == so:
        return s.replace(c, nl.join(moi))
    raise SystemExit("khong tim thay dung %d cho: %s" % (so, ten))


# ============================================================ D3D9onGPUi.h
P = "Sources/Represent/Represent3/D3D9onGPUi.h"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    s = thay(s, ["class CDevGpu; class CTexGpu; class CSurfGpu; class CVBGpu; class CSBGpu; class CGpuShim;"],
             ["class CDevGpu; class CTexGpu; class CSurfGpu; class CVBGpu; class CSBGpu; class CGpuShim; class CAtlasPageGpu; class CAtlasMgrGpu;"],
             "forward decl")
    s = thay(s, ["// ---------------------------------------------------------------- texture"], [
        "// ---------------------------------------------------------------- atlas %s (mang CAtlasMgr cua D3D9on11Atlas.cpp sang SDL_GPU)" % DAU,
        "// Gom texture sprite nho (POOL_DEFAULT, <= 512, khong RT/DYNAMIC) vao trang 1024x1024 cung dinh dang GPU (R8G8 bang mau /",
        "// BGRA8). Xep theo KE: trang thuoc mot lop chieu cao (16..512), moi hang cao H, anh chiem mot doan rong w (first-fit).",
        "// Texture ao (CTexGpu::m_bVirtual): m_pPage + (m_ax, m_ay); uv nhan/dich khi chep dinh vao ring (RgAtlasUv).",
        "// Tra cho: SAU khung (CDevGpu::m_atlasFrees, xu ly o FrameReset) vi lenh ve trong khung con tham chieu trang.",
        "class CAtlasPageGpu",
        "{",
        "public:",
        "\tCAtlasPageGpu() : m_pTex(NULL), m_fmt(SDL_GPU_TEXTUREFORMAT_INVALID), m_bpp(0), m_binH(0), m_rows(0), m_used(0) {}",
        "\tSDL_GPUTexture* m_pTex; SDL_GPUTextureFormat m_fmt; UINT m_bpp; UINT m_binH, m_rows, m_used;",
        "\tstd::vector<std::vector<std::pair<UINT, UINT> > > m_free;\t// moi hang: cac doan trong [x0, x1)",
        "};",
        "class CAtlasMgrGpu",
        "{",
        "public:",
        "\tCAtlasMgrGpu(CDevGpu* pDev);",
        "\t~CAtlasMgrGpu();",
        "\tstatic bool Eligible(UINT w, UINT h, DWORD usage, D3DFORMAT fmt, D3DPOOL pool);",
        "\tbool Alloc(UINT w, UINT h, SDL_GPUTextureFormat fmt, CAtlasPageGpu** ppPage, UINT* pX, UINT* pY);",
        "\tvoid Free(CAtlasPageGpu* pPage, UINT x, UINT y, UINT w);",
        "\tvoid ReleaseAll();\t\t\t// huy thiet bi: tra trang ngay",
        "\tCAtlasPageGpu* NewPage(UINT binH, SDL_GPUTextureFormat fmt);",
        "\tCDevGpu* m_pDev; std::vector<CAtlasPageGpu*> m_pages; UINT m_pageSize;",
        "};",
        "struct RgAtlasFree { CAtlasPageGpu* pPage; UINT x, y, w; };",
        "",
        "// ---------------------------------------------------------------- texture",
    ], "atlas classes")
    s = thay(s, ["\tvoid  FrameEnd()  { m_bUsedThisFrame = false; }"], [
        "\tvoid  FrameEnd()  { m_bUsedThisFrame = false; }",
        "\tvoid  BoAtlas();\t\t\t\t\t\t// %s texture ao -> texture rieng (truoc khi lam render target)" % DAU,
        "\tbool  ThuLaiCpu();\t\t\t\t\t\t// %s ban CPU da bo: doc lai tu GPU (dong bo, hiem)" % DAU2,
        "\tSDL_GPUTexture* GpuTex() const { return m_bVirtual ? (m_pPage ? m_pPage->m_pTex : NULL) : m_pGpu; }",
    ], "CTexGpu methods")
    s = thay(s, ["\tint         m_nPalRow;\t\t\t// hang bang mau (-1 = khong phai texture chi so)"], [
        "\tint         m_nPalRow;\t\t\t// hang bang mau (-1 = khong phai texture chi so)",
        "\tbool        m_bVirtual;\t\t\t// %s o trong trang atlas (m_pPage), khong co m_pGpu rieng" % DAU,
        "\tCAtlasPageGpu* m_pPage; UINT m_ax, m_ay;",
        "\tbool        m_bCpuBo;\t\t\t// %s ban CPU da bo sau khi tai len (LockRect phai doc lai tu GPU)" % DAU2,
    ], "CTexGpu fields")
    s = thay(s, ["\tvoid    TouchTex(CTexGpu* p);"], [
        "\tvoid    TouchTex(CTexGpu* p);",
        "\tvoid    UntouchTex(CTexGpu* p);\t\t\t\t\t// %s texture bi huy giua khung: rut khoi m_touched" % DAU,
        "\tvoid    DeferAtlasFree(CAtlasPageGpu* pPage, UINT x, UINT y, UINT w) { RgAtlasFree f = { pPage, x, y, w }; m_atlasFrees.push_back(f); }",
        "\tvoid    QueueZeroUpload(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp);\t// tai vung 0 (trang moi / o chua co du lieu)",
        "\tbool    ReadbackRegion(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, BYTE* pDst, UINT dstPitch);\t// %s" % DAU2,
    ], "CDevGpu methods")
    s = thay(s, ["\tstd::vector<CTexGpu*> m_touched;\t\t\t\t// texture co lenh ve tham chieu trong khung"], [
        "\tstd::vector<CTexGpu*> m_touched;\t\t\t\t// texture co lenh ve tham chieu trong khung",
        "\tCAtlasMgrGpu*   m_pAtlas;\t\t\t\t\t\t// %s NULL = tat (Rep3AtlasGpu=0)" % DAU,
        "\tstd::vector<RgAtlasFree> m_atlasFrees;\t\t\t// cho trong trang tra SAU khung",
    ], "CDevGpu fields")
    s = thay(s, ["\tunsigned __int64 m_uTexBytes;"], [
        "\tunsigned __int64 m_uTexBytes;",
        "\tunsigned        m_uCpuBoSo, m_uCpuBoThuLai; unsigned __int64 m_uCpuBoBytes;\t// %s so texture da bo ban CPU / phai doc lai / byte da bo" % DAU2,
    ], "CDevGpu stats")
    ghi(P, s)
    print("da va:", P)

# ============================================================ D3D9onGPURes.cpp
P = "Sources/Represent/Represent3/D3D9onGPURes.cpp"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    s = thay(s, ["\tm_bDirty = false; m_bLocked = false; m_bLockRO = false; m_bUsedThisFrame = false; m_uGpuBytes = 0; m_pSurf0 = NULL; m_nPalRow = -1;"], [
        "\tm_bDirty = false; m_bLocked = false; m_bLockRO = false; m_bUsedThisFrame = false; m_uGpuBytes = 0; m_pSurf0 = NULL; m_nPalRow = -1;",
        "\tm_bVirtual = false; m_pPage = NULL; m_ax = m_ay = 0; m_bCpuBo = false;\t// %s %s" % (DAU, DAU2),
    ], "ctor")
    s = thay(s, [
        "CTexGpu::~CTexGpu()",
        "{",
        "\tReleaseGpu();",
    ], [
        "CTexGpu::~CTexGpu()",
        "{",
        "\tif (m_pDev && m_bUsedThisFrame) m_pDev->UntouchTex(this);\t// %s dang trong m_touched cua khung -> rut ra (tranh con tro treo o FrameReset)" % DAU,
        "\tif (m_bVirtual && m_pPage && m_pDev)",
        "\t{\t// %s tra cho trong trang SAU khung (lenh ve trong khung co the con tham chieu)" % DAU,
        "\t\tm_pDev->DeferAtlasFree(m_pPage, m_ax, m_ay, m_w);",
        "\t\tm_pDev->m_uTexBytes -= m_uGpuBytes; g_uRep3GpuTexCount--; g_uRep3GpuTexBytes -= m_uGpuBytes;",
        "\t\tm_pPage = NULL; m_uGpuBytes = 0;",
        "\t}",
        "\tReleaseGpu();",
    ], "dtor")
    s = thay(s, ["\tif (!m_pCpu || !m_pGpu) return;"], [
        "\tSDL_GPUTexture* pDst = GpuTex();\t// %s texture ao: tai vao trang tai (m_ax, m_ay)" % DAU,
        "\tif (!m_pCpu || !pDst) return;",
    ], "QueueUpload dst")
    s = thay(s, [
        "\tconst bool bConv = (m_gpuFmt != m_fi.gpu) || m_fi.bConvert;",
        "\tconst UINT gbpp = RgGpuBpp(m_gpuFmt);",
    ], [
        "\tconst SDL_GPUTextureFormat gf = m_bVirtual ? m_pPage->m_fmt : m_gpuFmt;\t// %s" % DAU,
        "\tconst bool bConv = (gf != m_fi.gpu) || m_fi.bConvert;",
        "\tconst UINT gbpp = RgGpuBpp(gf);",
    ], "QueueUpload fmt")
    s = thay(s, ["\tRgTexUpload u = { m_pGpu, (UINT)rc.left, (UINT)rc.top, rw, rh, off, bytes };"], [
        "\tRgTexUpload u = { pDst, (UINT)rc.left + (m_bVirtual ? m_ax : 0), (UINT)rc.top + (m_bVirtual ? m_ay : 0), rw, rh, off, bytes };\t// %s" % DAU,
    ], "QueueUpload upload")
    s = thay(s, [
        "SDL_GPUTexture* CTexGpu::PrepareForBind()",
        "{",
        "\tif (!m_pGpu)",
        "\t{",
        "\t\tif (!NewVersion(false)) return NULL;",
    ], [
        "SDL_GPUTexture* CTexGpu::PrepareForBind()",
        "{",
        "\tif (m_bVirtual)",
        "\t{\t// %s texture ao: o trong trang atlas" % DAU,
        "\t\tif (!m_pPage)",
        "\t\t{",
        "\t\t\tCAtlasMgrGpu* pA = m_pDev->m_pAtlas;",
        "\t\t\tif (!pA || !pA->Alloc(m_w, m_h, m_fi.gpu, &m_pPage, &m_ax, &m_ay))",
        "\t\t\t\tm_bVirtual = false;\t// het cach (khong tao duoc trang): texture rieng nhu cu",
        "\t\t\telse",
        "\t\t\t{",
        "\t\t\t\tm_uGpuBytes = m_w * m_h * m_pPage->m_bpp; m_pDev->m_uTexBytes += m_uGpuBytes; g_uRep3GpuTexCount++; g_uRep3GpuTexBytes += m_uGpuBytes;",
        "\t\t\t\tif (m_pCpu) QueueUpload(NULL); else m_pDev->QueueZeroUpload(m_pPage->m_pTex, m_ax, m_ay, m_w, m_h, m_pPage->m_bpp);",
        "\t\t\t\tm_bDirty = false; m_bGpuHasData = true;",
        "\t\t\t}",
        "\t\t}",
        "\t\tif (m_bVirtual)",
        "\t\t{",
        "\t\t\tif (m_bDirty)",
        "\t\t\t{",
        "\t\t\t\tif (m_bUsedThisFrame && m_pCpu)",
        "\t\t\t\t{\t// lenh ve dau khung da tham chieu cho cu -> xin cho MOI trong trang (cho cu tra sau khung), tai toan bo",
        "\t\t\t\t\tCAtlasPageGpu* pNew = NULL; UINT x = 0, y = 0;",
        "\t\t\t\t\tif (m_pDev->m_pAtlas && m_pDev->m_pAtlas->Alloc(m_w, m_h, m_fi.gpu, &pNew, &x, &y))",
        "\t\t\t\t\t{ m_pDev->DeferAtlasFree(m_pPage, m_ax, m_ay, m_w); m_pPage = pNew; m_ax = x; m_ay = y; QueueUpload(NULL); }",
        "\t\t\t\t\telse QueueUpload(&m_rcDirty);\t// khong xin duoc: tai de len cho cu (lenh truoc trong khung thay noi dung moi - hiem)",
        "\t\t\t\t}",
        "\t\t\t\telse QueueUpload(&m_rcDirty);",
        "\t\t\t\tm_bDirty = false;",
        "\t\t\t}",
        "\t\t\tm_pDev->TouchTex(this);",
        "\t\t\tm_bUsedThisFrame = true;",
        "\t\t\treturn m_pPage->m_pTex;",
        "\t\t}",
        "\t}",
        "\tif (!m_pGpu)",
        "\t{",
        "\t\tif (m_bCpuBo && !m_pCpu) { m_pCpu = (BYTE*)calloc((size_t)m_pitch * m_h, 1); if (m_pCpu) ThuLaiCpu(); }\t// %s (khong xay ra voi sprite: m_pGpu chi mat khi NewVersion)" % DAU2,
        "\t\tif (!NewVersion(false)) return NULL;",
    ], "PrepareForBind")
    s = thay(s, [
        "SDL_GPUTexture* CTexGpu::PrepareAsTarget()",
        "{",
    ], [
        "SDL_GPUTexture* CTexGpu::PrepareAsTarget()",
        "{",
        "\tif (m_bVirtual) BoAtlas();\t// %s render target khong o trong trang" % DAU,
        "\tif (m_bCpuBo && !m_pCpu) { m_pCpu = (BYTE*)calloc((size_t)m_pitch * m_h, 1); if (m_pCpu) ThuLaiCpu(); }\t// %s" % DAU2,
    ], "PrepareAsTarget")
    s = thay(s, [
        "\tif (!m_pCpu)",
        "\t{",
        "\t\tm_pCpu = (BYTE*)calloc((size_t)m_pitch * m_h, 1);",
        "\t\tif (!m_pCpu) return E_OUTOFMEMORY;",
        "\t}",
        "\tif (m_bGpuNewer && m_pGpu)",
    ], [
        "\tif (!m_pCpu)",
        "\t{",
        "\t\tm_pCpu = (BYTE*)calloc((size_t)m_pitch * m_h, 1);",
        "\t\tif (!m_pCpu) return E_OUTOFMEMORY;",
        "\t\tif (m_bCpuBo) ThuLaiCpu();\t// %s ban CPU da bo sau khi tai len: doc lai tu GPU (hiem; sprite chi ghi mot lan)" % DAU2,
        "\t}",
        "\tif (m_bGpuNewer && m_pGpu)",
    ], "LockRect")
    s = thay(s, ["// ---------------------------------------------------------------- CSurfGpu"], [
        "// %s texture ao -> texture rieng (truoc khi lam render target). Noi dung: ban CPU (doc lai tu trang neu da bo)." % DAU,
        "void CTexGpu::BoAtlas()",
        "{",
        "\tif (!m_bVirtual) return;",
        "\tif (m_pPage)",
        "\t{",
        "\t\tif (!m_pCpu) { m_pCpu = (BYTE*)calloc((size_t)m_pitch * m_h, 1); if (m_pCpu) { m_bCpuBo = true; ThuLaiCpu(); } }",
        "\t\telse if (m_bCpuBo) ThuLaiCpu();",
        "\t\tm_pDev->DeferAtlasFree(m_pPage, m_ax, m_ay, m_w);",
        "\t\tm_pDev->m_uTexBytes -= m_uGpuBytes; g_uRep3GpuTexCount--; g_uRep3GpuTexBytes -= m_uGpuBytes; m_uGpuBytes = 0;",
        "\t\tm_pPage = NULL;",
        "\t}",
        "\tm_bVirtual = false; m_bGpuHasData = false;",
        "\tif (m_pCpu) { SetRect(&m_rcDirty, 0, 0, (int)m_w, (int)m_h); m_bDirty = true; }",
        "}",
        "",
        "// %s ban CPU da bo: doc lai tu GPU (texture rieng hoac vung trong trang). Dong bo - chi cho duong hiem (LockRect sprite)." % DAU2,
        "bool CTexGpu::ThuLaiCpu()",
        "{",
        "\t// m_bCpuBo giu = true trong luc doc (ReadbackRegion -> SubmitFrame(false) -> FrameReset khong bo ban CPU vua cap), xong moi ha",
        "\tSDL_GPUTexture* pSrc = GpuTex();",
        "\tif (!pSrc || !m_pCpu || !m_bGpuHasData) { m_bCpuBo = false; return false; }",
        "\tconst SDL_GPUTextureFormat gf = m_bVirtual ? m_pPage->m_fmt : m_gpuFmt;",
        "\tconst UINT gbpp = RgGpuBpp(gf), ox = m_bVirtual ? m_ax : 0, oy = m_bVirtual ? m_ay : 0;",
        "\tif (gbpp == 0) { m_bCpuBo = false; return false; }",
        "\tstd::vector<BYTE> tmp((size_t)m_w * m_h * gbpp);",
        "\tif (!m_pDev->ReadbackRegion(pSrc, ox, oy, m_w, m_h, gbpp, &tmp[0], m_w * gbpp)) { m_bCpuBo = false; return false; }",
        "\tconst bool bConv = (gf != m_fi.gpu) || m_fi.bConvert;",
        "\tfor (UINT y = 0; y < m_h; y++)",
        "\t{",
        "\t\tconst BYTE* ps = &tmp[(size_t)y * m_w * gbpp]; BYTE* pd = m_pCpu + (size_t)y * m_pitch;",
        "\t\tif (bConv) RgConvertRowFromBgra(m_fmt, (const DWORD*)ps, pd, m_w); else memcpy(pd, ps, (size_t)m_w * m_fi.bpp);",
        "\t}",
        "\tm_bCpuBo = false;",
        "\tm_pDev->m_uCpuBoThuLai++;",
        "\treturn true;",
        "}",
        "",
        "// ---------------------------------------------------------------- atlas %s (thuat toan y het CAtlasMgr cua D3D9on11Atlas.cpp)" % DAU,
        "static UINT RgAtlasBin(UINT v)",
        "{",
        "\tstatic const UINT s_bins[] = { 16, 24, 32, 48, 64, 96, 128, 192, 256, 384, 512 };",
        "\tfor (int i = 0; i < (int)(sizeof(s_bins) / sizeof(s_bins[0])); i++)",
        "\t\tif (v <= s_bins[i]) return s_bins[i];",
        "\treturn 512;",
        "}",
        "",
        "CAtlasMgrGpu::CAtlasMgrGpu(CDevGpu* pDev) { m_pDev = pDev; m_pageSize = 1024; }",
        "CAtlasMgrGpu::~CAtlasMgrGpu() { ReleaseAll(); }",
        "",
        "void CAtlasMgrGpu::ReleaseAll()",
        "{",
        "\tfor (size_t i = 0; i < m_pages.size(); i++)",
        "\t{",
        "\t\tCAtlasPageGpu* p = m_pages[i];",
        "\t\tif (p->m_pTex && m_pDev->m_pGpu) SDL_ReleaseGPUTexture(m_pDev->m_pGpu, p->m_pTex);",
        "\t\tdelete p;",
        "\t}",
        "\tm_pages.clear();",
        "\tg_uRep3AtlasPages = 0; g_uRep3AtlasBytes = 0;",
        "}",
        "",
        "bool CAtlasMgrGpu::Eligible(UINT w, UINT h, DWORD usage, D3DFORMAT fmt, D3DPOOL pool)",
        "{",
        "\tif (pool != D3DPOOL_DEFAULT) return false;",
        "\tif (usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DYNAMIC | D3DUSAGE_DEPTHSTENCIL)) return false;",
        "\tif (w == 0 || h == 0 || w > 512 || h > 512) return false;",
        "\tswitch (fmt)",
        "\t{",
        "\tcase D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8: case D3DFMT_A4R4G4B4: case D3DFMT_R5G6B5: case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5: case D3DFMT_A8L8: return true;",
        "\tdefault: return false;",
        "\t}",
        "}",
        "",
        "CAtlasPageGpu* CAtlasMgrGpu::NewPage(UINT binH, SDL_GPUTextureFormat fmt)",
        "{",
        "\tconst UINT bpp = RgGpuBpp(fmt);",
        "\tif (bpp == 0 || !m_pDev->m_pGpu) return NULL;",
        "\tSDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci));",
        "\tci.type = SDL_GPU_TEXTURETYPE_2D; ci.format = fmt; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;",
        "\tci.width = m_pageSize; ci.height = m_pageSize; ci.layer_count_or_depth = 1; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;",
        "\tSDL_GPUTexture* pTex = SDL_CreateGPUTexture(m_pDev->m_pGpu, &ci);",
        "\tif (!pTex) { RgLog(\"atlas: CreateGPUTexture trang %ux%u fmt %d that bai: %s\", m_pageSize, m_pageSize, (int)fmt, SDL_GetError()); return NULL; }",
        "\tm_pDev->QueueZeroUpload(pTex, 0, 0, m_pageSize, m_pageSize, bpp);\t// trang moi = 0 (khong de rac; vien o khi loc tuyen tinh)",
        "\tCAtlasPageGpu* p = new CAtlasPageGpu();",
        "\tp->m_pTex = pTex; p->m_fmt = fmt; p->m_bpp = bpp; p->m_binH = binH; p->m_rows = m_pageSize / binH; p->m_used = 0;",
        "\tp->m_free.resize(p->m_rows);",
        "\tfor (UINT r = 0; r < p->m_rows; r++) p->m_free[r].push_back(std::make_pair(0u, m_pageSize));\t// ca hang trong",
        "\tm_pages.push_back(p);",
        "\tg_uRep3AtlasPages++; g_uRep3AtlasBytes += (unsigned __int64)m_pageSize * m_pageSize * bpp;",
        "\treturn p;",
        "}",
        "",
        "bool CAtlasMgrGpu::Alloc(UINT w, UINT h, SDL_GPUTextureFormat fmt, CAtlasPageGpu** ppPage, UINT* pX, UINT* pY)",
        "{",
        "\tUINT binH = RgAtlasBin(h);",
        "\tif (h > binH || w > m_pageSize) return false;",
        "\tfor (int lan = 0; lan < 2; lan++)",
        "\t{",
        "\t\tfor (size_t i = 0; i < m_pages.size(); i++)",
        "\t\t{",
        "\t\t\tCAtlasPageGpu* p = m_pages[i];",
        "\t\t\tif (p->m_binH != binH || p->m_fmt != fmt) continue;",
        "\t\t\tfor (UINT r = 0; r < p->m_rows; r++)",
        "\t\t\t{",
        "\t\t\t\tstd::vector<std::pair<UINT, UINT> >& fr = p->m_free[r];",
        "\t\t\t\tfor (size_t k = 0; k < fr.size(); k++)",
        "\t\t\t\t{",
        "\t\t\t\t\tif (fr[k].second - fr[k].first < w) continue;",
        "\t\t\t\t\tUINT x = fr[k].first;",
        "\t\t\t\t\tfr[k].first += w;",
        "\t\t\t\t\tif (fr[k].first >= fr[k].second) fr.erase(fr.begin() + k);",
        "\t\t\t\t\tp->m_used++;",
        "\t\t\t\t\t*ppPage = p; *pX = x; *pY = r * binH;",
        "\t\t\t\t\treturn true;",
        "\t\t\t\t}",
        "\t\t\t}",
        "\t\t}",
        "\t\tif (!NewPage(binH, fmt)) return false;\t// lan 2: thu lai voi trang moi",
        "\t}",
        "\treturn false;",
        "}",
        "",
        "void CAtlasMgrGpu::Free(CAtlasPageGpu* pPage, UINT x, UINT y, UINT w)",
        "{",
        "\tif (!pPage || pPage->m_binH == 0) return;",
        "\tUINT r = y / pPage->m_binH;",
        "\tif (r >= pPage->m_rows) return;",
        "\tstd::vector<std::pair<UINT, UINT> >& fr = pPage->m_free[r];",
        "\tUINT x0 = x, x1 = x + w;",
        "\tsize_t k = 0;",
        "\twhile (k < fr.size() && fr[k].first < x0) k++;",
        "\tfr.insert(fr.begin() + k, std::make_pair(x0, x1));",
        "\tif (k + 1 < fr.size() && fr[k].second == fr[k + 1].first) { fr[k].second = fr[k + 1].second; fr.erase(fr.begin() + k + 1); }",
        "\tif (k > 0 && fr[k - 1].second == fr[k].first) { fr[k - 1].second = fr[k].second; fr.erase(fr.begin() + k); }",
        "\tif (pPage->m_used) pPage->m_used--;",
        "\tif (pPage->m_used == 0)",
        "\t{",
        "\t\t// giu toi da MOT trang rong moi lop chieu cao; trang rong thu hai thi tra lai GPU (sau khung: DeferRelease)",
        "\t\tint nEmptySameClass = 0;",
        "\t\tfor (size_t i = 0; i < m_pages.size(); i++)",
        "\t\t\tif (m_pages[i] != pPage && m_pages[i]->m_binH == pPage->m_binH && m_pages[i]->m_fmt == pPage->m_fmt && m_pages[i]->m_used == 0) nEmptySameClass++;",
        "\t\tif (nEmptySameClass >= 1)",
        "\t\t{",
        "\t\t\tfor (size_t i = 0; i < m_pages.size(); i++)",
        "\t\t\t\tif (m_pages[i] == pPage) { m_pages.erase(m_pages.begin() + i); break; }",
        "\t\t\tif (pPage->m_pTex) m_pDev->DeferRelease(pPage->m_pTex);",
        "\t\t\tif (g_uRep3AtlasPages) g_uRep3AtlasPages--;",
        "\t\t\tg_uRep3AtlasBytes -= (unsigned __int64)m_pageSize * m_pageSize * pPage->m_bpp;",
        "\t\t\tdelete pPage;",
        "\t\t}",
        "\t}",
        "}",
        "",
        "// ---------------------------------------------------------------- CSurfGpu",
    ], "atlas impl")
    ghi(P, s)
    print("da va:", P)

# ============================================================ D3D9onGPUDev.cpp
P = "Sources/Represent/Represent3/D3D9onGPUDev.cpp"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    s = thay(s, [
        "\tm_bFrameOpen = false;",
        "\tm_pPalTex = NULL; { const char* e = getenv(\"REP3_PALLIN\"); m_bPalLinForce = (e && atoi(e) != 0); }",
    ], [
        "\tm_bFrameOpen = false;",
        "\tm_pAtlas = NULL; m_uCpuBoSo = 0; m_uCpuBoThuLai = 0; m_uCpuBoBytes = 0;\t// %s %s" % (DAU, DAU2),
        "\tm_pPalTex = NULL; { const char* e = getenv(\"REP3_PALLIN\"); m_bPalLinForce = (e && atoi(e) != 0); }",
    ], "ctor")
    s = thay(s, [
        "\tif (m_pGpu) SDL_WaitForGPUIdle(m_pGpu);",
        "\tFrameReset();",
    ], [
        "\tif (m_pGpu) SDL_WaitForGPUIdle(m_pGpu);",
        "\tFrameReset();",
        "\tif (m_pAtlas) { m_pAtlas->ReleaseAll(); delete m_pAtlas; m_pAtlas = NULL; }\t// %s (sau FrameReset: cho tra sau khung da xu ly)" % DAU,
    ], "dtor")
    s = thay(s, ["\tm_pBackSurf = new CSurfGpu(this, RGSURF_BACKBUFFER, NULL, m_bbW, m_bbH, D3DFMT_X8R8G8B8);"], [
        "\tm_pBackSurf = new CSurfGpu(this, RGSURF_BACKBUFFER, NULL, m_bbW, m_bbH, D3DFMT_X8R8G8B8);",
        "\tif (g_nRep3AtlasGpu) m_pAtlas = new CAtlasMgrGpu(this);\t// %s" % DAU,
        "\tRgLog(\"atlas: %s | bo ban CPU sau khi tai len: %s\", m_pAtlas ? \"BAT (trang 1024x1024, texture DEFAULT <= 512 khong RT; Rep3AtlasGpu=0 de tat)\" : \"tat\", g_nRep3GpuBoBanCpu ? \"BAT (Rep3GpuBoBanCpu=0 de tat)\" : \"tat\");",
    ], "Init")
    s = thay(s, ["\tCTexGpu* pTex = new CTexGpu(this, Width, Height, Usage, Format, Pool);"], [
        "\tCTexGpu* pTex = new CTexGpu(this, Width, Height, Usage, Format, Pool);",
        "\tif (m_pAtlas && CAtlasMgrGpu::Eligible(Width, Height, Usage, Format, Pool)) pTex->m_bVirtual = true;\t// %s" % DAU,
    ], "CreateTexture")
    s = thay(s, [
        "void CDevGpu::TouchTex(CTexGpu* p)",
        "{",
        "\tif (!p->m_bUsedThisFrame) m_touched.push_back(p);",
        "}",
    ], [
        "void CDevGpu::TouchTex(CTexGpu* p)",
        "{",
        "\tif (!p->m_bUsedThisFrame) m_touched.push_back(p);",
        "}",
        "",
        "// %s texture bi huy giua khung (dang trong m_touched) -> rut ra; lenh ve da ghi van giu SDL_GPUTexture (tra sau khung)" % DAU,
        "void CDevGpu::UntouchTex(CTexGpu* p)",
        "{",
        "\tfor (size_t i = m_touched.size(); i > 0; i--)",
        "\t\tif (m_touched[i - 1] == p) { m_touched.erase(m_touched.begin() + (i - 1)); return; }",
        "}",
        "",
        "// %s ghi lenh tai mot vung toan 0 (trang moi, o chua co du lieu CPU)" % DAU,
        "void CDevGpu::QueueZeroUpload(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp)",
        "{",
        "\tif (!pTex || !w || !h || !bpp) return;",
        "\tconst UINT bytes = w * h * bpp;",
        "\tconst UINT off = ((UINT)m_texStage.size() + 15) & ~15u;",
        "\tm_texStage.resize((size_t)off + bytes, 0);",
        "\tRgTexUpload u = { pTex, x, y, w, h, off, bytes };",
        "\tm_texUploads.push_back(u);",
        "}",
        "",
        "// %s doc lai mot vung texture GPU ve CPU (dong bo): nhu ReadbackTexture nhung co goc (x, y) va byte/diem" % DAU2,
        "bool CDevGpu::ReadbackRegion(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, BYTE* pDst, UINT dstPitch)",
        "{",
        "\tif (!pTex || !pDst || !w || !h || !bpp) return false;",
        "\tif (!m_cmds.empty() || !m_texUploads.empty()) SubmitFrame(false);",
        "\tconst UINT bytes = w * h * bpp;",
        "\tSDL_GPUTransferBufferCreateInfo ti; memset(&ti, 0, sizeof(ti)); ti.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD; ti.size = bytes;",
        "\tSDL_GPUTransferBuffer* pX = SDL_CreateGPUTransferBuffer(m_pGpu, &ti);",
        "\tif (!pX) return false;",
        "\tSDL_GPUCommandBuffer* cb = SDL_AcquireGPUCommandBuffer(m_pGpu);",
        "\tif (!cb) { SDL_ReleaseGPUTransferBuffer(m_pGpu, pX); return false; }",
        "\tSDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cb);",
        "\tSDL_GPUTextureRegion src; memset(&src, 0, sizeof(src)); src.texture = pTex; src.x = x; src.y = y; src.w = w; src.h = h; src.d = 1;",
        "\tSDL_GPUTextureTransferInfo dst; memset(&dst, 0, sizeof(dst)); dst.transfer_buffer = pX; dst.pixels_per_row = w; dst.rows_per_layer = h;",
        "\tSDL_DownloadFromGPUTexture(cp, &src, &dst);",
        "\tSDL_EndGPUCopyPass(cp);",
        "\tSDL_GPUFence* f = SDL_SubmitGPUCommandBufferAndAcquireFence(cb);",
        "\tif (f) { SDL_WaitForGPUFences(m_pGpu, true, &f, 1); SDL_ReleaseGPUFence(m_pGpu, f); }",
        "\tbool ok = false;",
        "\tconst BYTE* p = (const BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, pX, false);",
        "\tif (p) { for (UINT r = 0; r < h; r++) memcpy(pDst + (size_t)r * dstPitch, p + (size_t)r * w * bpp, (size_t)w * bpp); SDL_UnmapGPUTransferBuffer(m_pGpu, pX); ok = true; }",
        "\tSDL_ReleaseGPUTransferBuffer(m_pGpu, pX);",
        "\treturn ok;",
        "}",
    ], "TouchTex helpers")
    # DrawInternal: uv atlas
    s = thay(s, [
        "HRESULT CDevGpu::DrawInternal(D3DPRIMITIVETYPE type, const BYTE* pVerts, UINT nVerts, UINT stride)",
        "{",
    ], [
        "// %s uv cua texture ao (stage 0) -> uv trong trang atlas, sua tai cho tren dinh da chep vao ring (nhu R11AtlasUv)" % DAU,
        "static void RgAtlasUv(BYTE* pV, UINT nVerts, UINT strideRing, DWORD fvf, CTexGpu* pTex, float fPage)",
        "{",
        "\tif (!pTex || !pTex->m_bVirtual || !pTex->m_pPage) return;",
        "\tUINT uvOff = 0xFFFFFFFF; RgFvfStride(fvf, NULL, NULL, &uvOff);",
        "\tif (uvOff == 0xFFFFFFFF) return;",
        "\tconst float sx = (float)pTex->m_w / fPage, sy = (float)pTex->m_h / fPage, ox = (float)pTex->m_ax / fPage, oy = (float)pTex->m_ay / fPage;",
        "\tfor (UINT i = 0; i < nVerts; i++) { float* uv = (float*)(pV + i * strideRing + uvOff); uv[0] = uv[0] * sx + ox; uv[1] = uv[1] * sy + oy; }",
        "}",
        "",
        "HRESULT CDevGpu::DrawInternal(D3DPRIMITIVETYPE type, const BYTE* pVerts, UINT nVerts, UINT stride)",
        "{",
    ], "RgAtlasUv")
    s = thay(s, ["\tRgDrawState st; ComputeState(st, topo);"], [
        "\tRgDrawState st; ComputeState(st, topo);\t// (PrepareForBind o day: texture ao da co cho trong trang truoc khi doi uv)",
        "\tconst float fPage = m_pAtlas ? (float)m_pAtlas->m_pageSize : 1024.0f;\t// %s" % DAU,
    ], "DrawInternal fPage")
    s = thay(s, [
        "\t\tfor (int i = 0; i < 6; i++) { memcpy(d + i * s2, pVerts + s_idx[i] * stride, stride); *(UINT*)(d + i * s2 + stride) = uPal; }",
        "\t\tnOut = 6;",
    ], [
        "\t\tfor (int i = 0; i < 6; i++) { memcpy(d + i * s2, pVerts + s_idx[i] * stride, stride); *(UINT*)(d + i * s2 + stride) = uPal; }",
        "\t\tRgAtlasUv(d, 6, s2, m_fvf, m_tex[0], fPage);\t// %s" % DAU,
        "\t\tnOut = 6;",
    ], "DrawInternal quad uv")
    s = thay(s, ["\t\tnOut = nTri * 3;"], [
        "\t\tnOut = nTri * 3;",
        "\t\tRgAtlasUv(d, nOut, s2, m_fvf, m_tex[0], fPage);\t// %s" % DAU,
    ], "DrawInternal fan uv")
    s = thay(s, [
        "\t\tfor (UINT i = 0; i < nVerts; i++) { memcpy(d + i * s2, pVerts + i * stride, stride); *(UINT*)(d + i * s2 + stride) = uPal; }",
        "\t}",
        "\tRgCmd c; memset(&c, 0, sizeof(c)); c.type = RGCMD_DRAW;",
    ], [
        "\t\tfor (UINT i = 0; i < nVerts; i++) { memcpy(d + i * s2, pVerts + i * stride, stride); *(UINT*)(d + i * s2 + stride) = uPal; }",
        "\t\tRgAtlasUv(d, nVerts, s2, m_fvf, m_tex[0], fPage);\t// %s" % DAU,
        "\t}",
        "\tRgCmd c; memset(&c, 0, sizeof(c)); c.type = RGCMD_DRAW;",
    ], "DrawInternal list uv")
    # FrameReset
    s = thay(s, [
        "\tfor (size_t i = 0; i < m_touched.size(); i++) m_touched[i]->FrameEnd();",
        "\tm_touched.clear();",
    ], [
        "\tfor (size_t i = 0; i < m_touched.size(); i++)",
        "\t{",
        "\t\tCTexGpu* p = m_touched[i];",
        "\t\tp->FrameEnd();",
        "\t\t// %s texture DEFAULT (sprite) da tai len xong (lenh tai vua submit): bo ban CPU - RAM = 1x cache thay vi 2x." % DAU2,
        "\t\t// Khong bo: render target, DYNAMIC, dang doi (dirty), dang khoa, MANAGED/SYSTEMMEM (engine con ghi tiep).",
        "\t\t// (!m_bCpuBo: dang doc lai tu GPU trong ThuLaiCpu - SubmitFrame(false) giua khung goi vao day - thi khong duoc bo lan nua)",
        "\t\tif (g_nRep3GpuBoBanCpu && p->m_pCpu && !p->m_bCpuBo && !p->m_bDirty && !p->m_bLocked && p->m_bGpuHasData && !p->m_bGpuTarget && !p->m_bGpuNewer",
        "\t\t\t&& p->m_pool == D3DPOOL_DEFAULT && !(p->m_usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DYNAMIC)) && p->GpuTex())",
        "\t\t{",
        "\t\t\tm_uCpuBoBytes += (unsigned __int64)p->m_pitch * p->m_h; m_uCpuBoSo++;",
        "\t\t\tfree(p->m_pCpu); p->m_pCpu = NULL; p->m_bCpuBo = true;",
        "\t\t}",
        "\t}",
        "\tm_touched.clear();",
        "\tif (m_pAtlas)\t// %s tra cho trong trang sau khi lenh ve da submit (trang rong -> DeferRelease -> tra ngay duoi day)" % DAU,
        "\t\tfor (size_t i = 0; i < m_atlasFrees.size(); i++) m_pAtlas->Free(m_atlasFrees[i].pPage, m_atlasFrees[i].x, m_atlasFrees[i].y, m_atlasFrees[i].w);",
        "\tm_atlasFrees.clear();",
    ], "FrameReset")
    s = thay(s, [
        "\t\tRgLog(\"khung %u: lenh ve %u, quad %u, tai texture %u, pipeline %u, texture GPU %u (%u MB)\", m_uFrames, m_uDrawCmds, m_uQuads, m_uUploads, (unsigned)m_pipes.size(), g_uRep3GpuTexCount, (unsigned)(g_uRep3GpuTexBytes >> 20));",
    ], [
        "\t\tRgLog(\"khung %u: lenh ve %u, quad %u, tai texture %u, pipeline %u, texture GPU %u (%u MB) | atlas %u trang (%u MB) | bo ban CPU %u texture (%u MB), doc lai %u\", m_uFrames, m_uDrawCmds, m_uQuads, m_uUploads, (unsigned)m_pipes.size(), g_uRep3GpuTexCount, (unsigned)(g_uRep3GpuTexBytes >> 20),",
        "\t\t\tg_uRep3AtlasPages, (unsigned)(g_uRep3AtlasBytes >> 20), m_uCpuBoSo, (unsigned)(m_uCpuBoBytes >> 20), m_uCpuBoThuLai);\t// %s %s" % (DAU, DAU2),
    ], "Present log")
    ghi(P, s)
    print("da va:", P)

# ============================================================ BaseInclude.h : khai bao cong tac
P = "Sources/Represent/Represent3/BaseInclude.h"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    s = thay(s, ["extern unsigned g_uRep3AtlasPages; extern unsigned __int64 g_uRep3AtlasBytes;"], [
        "extern unsigned g_uRep3AtlasPages; extern unsigned __int64 g_uRep3AtlasBytes;",
        "#ifdef JX_PLATFORM_SDL",
        "extern int  g_nRep3AtlasGpu;\t// %s [Client] Rep3AtlasGpu: atlas cho lop SDL_GPU (Rep3Api=100); mac dinh 1 Android, 0 Windows" % DAU,
        "extern int  g_nRep3GpuBoBanCpu;\t// %s [Client] Rep3GpuBoBanCpu: bo ban CPU cua texture DEFAULT sau khi tai len; mac dinh 1 Android, 0 Windows" % DAU2,
        "#endif",
    ], "BaseInclude")
    ghi(P, s)
    print("da va:", P)

# ============================================================ KRepresentShell3.cpp : dinh nghia + doc ini
P = "Sources/Represent/Represent3/KRepresentShell3.cpp"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    s = thay(s, ["int  g_nRep3Pal       = 1;\t// [D3D11 08/09 r] texture sprite bang mau 2 B/px (chi D3D11)"], [
        "#ifdef JX_PLATFORM_SDL",
        "#ifdef JX_ANDROID",
        "int g_nRep3AtlasGpu = 1;\t// %s Android: mac dinh BAT" % DAU,
        "int g_nRep3GpuBoBanCpu = 1;\t// %s Android: mac dinh BAT" % DAU2,
        "#else",
        "int g_nRep3AtlasGpu = 0;\t// %s GameSDL.exe tren Windows: mac dinh TAT (khong doi hanh vi PC); bat bang [Client] Rep3AtlasGpu=1" % DAU,
        "int g_nRep3GpuBoBanCpu = 0;\t// %s nhu tren" % DAU2,
        "#endif",
        "#endif",
        "int  g_nRep3Pal       = 1;\t// [D3D11 08/09 r] texture sprite bang mau 2 B/px (chi D3D11)",
    ], "shell globals")
    s = thay(s, ["\tg_nRep3Pal       = Rep3Ini(\"Rep3Pal\", 1);\t// [D3D11 08/09 r]"], [
        "\tg_nRep3Pal       = Rep3Ini(\"Rep3Pal\", 1);\t// [D3D11 08/09 r]",
        "#ifdef JX_PLATFORM_SDL",
        "\tg_nRep3AtlasGpu    = Rep3Ini(\"Rep3AtlasGpu\", g_nRep3AtlasGpu) ? 1 : 0;\t// %s" % DAU,
        "\tg_nRep3GpuBoBanCpu = Rep3Ini(\"Rep3GpuBoBanCpu\", g_nRep3GpuBoBanCpu) ? 1 : 0;\t// %s" % DAU2,
        "#endif",
    ], "shell ini")
    ghi(P, s)
    print("da va:", P)

# ============================================================ TextureResMgr.cpp : ngan sach tren Android + he so atlas
P = "Sources/Represent/Represent3/TextureResMgr.cpp"
s = doc(P)
if DAU in s:
    print("da va roi, bo qua:", P)
else:
    s = thay(s, [
        "\t\tuBudgetMB = (unsigned __int64)g_nRep3CacheMB;",
        "\tm_nBalanceNum = (int32)(uBudgetMB * 1024 * 1024);",
    ], [
        "\t\tuBudgetMB = (unsigned __int64)g_nRep3CacheMB;",
        "#ifdef JX_POSIX",
        "\t{\t// [ANDROID 11/09 CACHE] dien thoai: texture nam trong RAM chung CPU/GPU (khong co VRAM rieng), he thong giet app khi",
        "\t\t// tien trinh qua lon -> kep <= RAM/3 KE CA khi [Client] Rep3CacheMB ep (config Android tung chep 1500 MB tu PC 32 GB).",
        "\t\tif (g_nRep3CacheMB <= 0)",
        "\t\t{\t// tu tinh: RAM/8 kep [128, 512] MB (PC: RAM/16 kep 60..1024 - dien thoai it RAM hon nhieu, ma texture la thu an nhieu nhat;",
        "\t\t\t// ban CPU cua texture da bo (BOCPU) nen so nay ~ RAM that texture chiem). CapBudgetByVram con kep theo 'VRAM' ao cua SDL_GPU.",
        "\t\t\tuBudgetMB = uPhysMB / 8;",
        "\t\t\tif (uBudgetMB < 128) uBudgetMB = 128;",
        "\t\t\tif (uBudgetMB > 512) uBudgetMB = 512;",
        "\t\t}",
        "\t\tunsigned __int64 uTranMB = uPhysMB / 3;",
        "\t\tif (uTranMB < 128) uTranMB = 128;",
        "\t\tif (uBudgetMB > uTranMB)",
        "\t\t{",
        "\t\t\tRep3Log(\"[REP3] cache texture: %llu MB (Rep3CacheMB=%d) qua lon so voi RAM %llu MB cua may -> kep %llu MB\", uBudgetMB, g_nRep3CacheMB, uPhysMB, uTranMB);",
        "\t\t\tuBudgetMB = uTranMB;",
        "\t\t}",
        "\t}",
        "#endif",
        "\tm_nBalanceNum = (int32)(uBudgetMB * 1024 * 1024);",
    ], "SetBudget")
    s = thay(s, ["\tunsigned __int64 uHeSo10 = (g_nRep3ApiOn == 11 && g_nRep3Atlas) ? 13 : 10;"], [
        "#ifdef JX_PLATFORM_SDL",
        "\tunsigned __int64 uHeSo10 = ((g_nRep3ApiOn == 11 && g_nRep3Atlas) || (g_nRep3ApiOn == 100 && g_nRep3AtlasGpu)) ? 13 : 10;\t// %s trang atlas SDL_GPU cung ton them ~1,3x" % DAU,
        "#else",
        "\tunsigned __int64 uHeSo10 = (g_nRep3ApiOn == 11 && g_nRep3Atlas) ? 13 : 10;",
        "#endif",
    ], "CapBudgetByVram")
    ghi(P, s)
    print("da va:", P)

# ============================================================ config.ini Android
P = "android/du_lieu_ghi_de/config.ini"
s = doc(P)
if "Rep3AtlasGpu" in s:
    print("da va roi, bo qua:", P)
else:
    s = thay(s, ["Rep3CacheMB=1500"], [
        "; [ANDROID 11/09 CACHE] 1500 la so cua PC 32 GB RAM. Dien thoai / may ao: de 0 = tu tinh (RAM/16, kep them theo 'VRAM' ao cua",
        "; SDL_GPU). Tren Android tu tinh = RAM/8 kep 128..512 MB; con kep <= RAM/3 ke ca khi ep so o day. Nho hon = giai ma lai nhieu hon (lag khi dong nguoi);",
        "; lon hon = de bi he thong giet app vi thieu RAM.",
        "Rep3CacheMB=0",
        "; [ANDROID 11/09 ATLAS] 1 = gom texture sprite nho vao trang 1024x1024 tren lop SDL_GPU (nhu D3D11 tren PC). 0 = moi khung mot texture (cu).",
        "Rep3AtlasGpu=1",
        "; [ANDROID 11/09 BOCPU] 1 = bo ban CPU cua texture sprite sau khi da tai len GPU (RAM = 1x cache thay vi 2x). 0 = giu nhu cu.",
        "Rep3GpuBoBanCpu=1",
    ], "config")
    ghi(P, s)
    print("da va:", P)
