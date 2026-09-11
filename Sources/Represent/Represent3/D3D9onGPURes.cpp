// [GPU 08/09] D3D9 tren SDL_GPU - tai nguyen: texture (ban CPU + phien ban GPU), surface, vertex buffer (chi CPU), state block.
#include "precompile.h"
#include "BaseInclude.h"
#ifdef JX_PLATFORM_SDL
#include "D3D9onGPU.h"
#include "D3D9onGPUi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void RgConvertRowFromBgra(D3DFORMAT f, const DWORD* pSrc, BYTE* pDst, UINT w);

// ---------------------------------------------------------------- CTexGpu
CTexGpu::CTexGpu(CDevGpu* pDev, UINT w, UINT h, DWORD usage, D3DFORMAT fmt, D3DPOOL pool)
{
	m_ref = 1; m_pDev = pDev; m_w = w; m_h = h; m_usage = usage; m_fmt = fmt; m_pool = pool;
	m_fi = RgFormatInfo(fmt);
	m_pitch = RgPitch(fmt, w);
	m_pCpu = NULL; m_pGpu = NULL; m_gpuFmt = m_fi.gpu; m_bGpuTarget = false; m_bGpuHasData = false; m_bGpuNewer = false;
	m_bDirty = false; m_bLocked = false; m_bLockRO = false; m_bUsedThisFrame = false; m_uGpuBytes = 0; m_pSurf0 = NULL; m_nPalRow = -1;
	m_bVirtual = false; m_pPage = NULL; m_ax = m_ay = 0; m_bCpuBo = false;	// [GPU 11/09 ATLAS] [GPU 11/09 BOCPU]
	SetRect(&m_rcDirty, 0, 0, 0, 0); SetRect(&m_rcLock, 0, 0, 0, 0);
	if (!(usage & D3DUSAGE_RENDERTARGET))
		m_pCpu = (BYTE*)calloc((size_t)m_pitch * h, 1);	// render target: khong ban CPU, cap khi Lock
	pDev->AddRef();
}

CTexGpu::~CTexGpu()
{
	if (m_pDev && m_bUsedThisFrame) m_pDev->UntouchTex(this);	// [GPU 11/09 ATLAS] dang trong m_touched cua khung -> rut ra (tranh con tro treo o FrameReset)
	if (m_bVirtual && m_pPage && m_pDev)
	{	// [GPU 11/09 ATLAS] tra cho trong trang SAU khung (lenh ve trong khung co the con tham chieu)
		m_pDev->DeferAtlasFree(m_pPage, m_ax, m_ay, m_w);
		m_pDev->m_uTexBytes -= m_uGpuBytes; g_uRep3GpuTexCount--; g_uRep3GpuTexBytes -= m_uGpuBytes;
		m_pPage = NULL; m_uGpuBytes = 0;
	}
	ReleaseGpu();
	if (m_pCpu) { free(m_pCpu); m_pCpu = NULL; }
	if (m_pDev) m_pDev->Release();
}

void CTexGpu::ReleaseGpu()
{
	if (m_pGpu)
	{
		m_pDev->DeferRelease(m_pGpu);
		m_pDev->m_uTexBytes -= m_uGpuBytes;
		g_uRep3GpuTexCount--; g_uRep3GpuTexBytes -= m_uGpuBytes;
		m_pGpu = NULL; m_uGpuBytes = 0; m_bGpuHasData = false; m_bGpuNewer = false;
	}
}

bool CTexGpu::NewVersion(bool bTarget)
{
	SDL_GPUTextureFormat gf = bTarget ? SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM : m_fi.gpu;	// render target luon BGRA8 (pipeline mot dinh dang)
	if (gf == SDL_GPU_TEXTUREFORMAT_INVALID) return false;
	SDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci));
#ifdef JX_ANDROID
	ci.type = g_nJxAtlasMang ? SDL_GPU_TEXTURETYPE_2D_ARRAY : SDL_GPU_TEXTURETYPE_2D; ci.format = gf;	// [MANG 11/09] shader dung sampler2DArray -> MOI texture phai la mang (rieng = 1 lop)
#else
	ci.type = SDL_GPU_TEXTURETYPE_2D; ci.format = gf;
#endif
	ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | (bTarget ? SDL_GPU_TEXTUREUSAGE_COLOR_TARGET : 0);
	ci.width = m_w; ci.height = m_h; ci.layer_count_or_depth = 1; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;
	SDL_GPUTexture* p = SDL_CreateGPUTexture(m_pDev->m_pGpu, &ci);
	if (!p) { RgLog("CreateGPUTexture %ux%u fmt %d rt=%d that bai: %s", m_w, m_h, (int)gf, (int)bTarget, SDL_GetError()); return false; }
	ReleaseGpu();
	m_pGpu = p; m_gpuFmt = gf; m_bGpuTarget = bTarget; m_bGpuHasData = false; m_bUsedThisFrame = false; m_bGpuNewer = false;
	m_uGpuBytes = m_w * m_h * RgGpuBpp(gf);
	m_pDev->m_uTexBytes += m_uGpuBytes;
	g_uRep3GpuTexCount++; g_uRep3GpuTexBytes += m_uGpuBytes;
	return true;
}

// chep CPU (vung prc, NULL = toan bo) vao staging cua khung + ghi lenh tai len phien ban hien tai
void CTexGpu::QueueUpload(const RECT* prc)
{
	SDL_GPUTexture* pDst = GpuTex();	// [GPU 11/09 ATLAS] texture ao: tai vao trang tai (m_ax, m_ay)
	if (!m_pCpu || !pDst) return;
	RECT rc; if (prc) rc = *prc; else SetRect(&rc, 0, 0, (int)m_w, (int)m_h);
	if (rc.left < 0) rc.left = 0; if (rc.top < 0) rc.top = 0; if (rc.right > (int)m_w) rc.right = (int)m_w; if (rc.bottom > (int)m_h) rc.bottom = (int)m_h;
	if (rc.right <= rc.left || rc.bottom <= rc.top) return;
	const UINT rw = (UINT)(rc.right - rc.left), rh = (UINT)(rc.bottom - rc.top);
	const SDL_GPUTextureFormat gf = m_bVirtual ? m_pPage->m_fmt : m_gpuFmt;	// [GPU 11/09 ATLAS]
	const bool bConv = (gf != m_fi.gpu) || m_fi.bConvert;
	const UINT gbpp = RgGpuBpp(gf);
	const UINT bytes = rw * rh * gbpp;
	std::vector<BYTE>& st = m_pDev->m_texStage;
	const UINT off = ((UINT)st.size() + 15) & ~15u;
	st.resize(off + bytes);
	for (UINT y = 0; y < rh; y++)
	{
		const BYTE* pSrc = m_pCpu + (size_t)(rc.top + (int)y) * m_pitch + (size_t)rc.left * m_fi.bpp;
		BYTE* pDst = &st[off + y * rw * gbpp];
		if (bConv) RgConvertRowToBgra(m_fmt, pSrc, (DWORD*)pDst, rw);
		else memcpy(pDst, pSrc, rw * gbpp);
	}
	RgTexUpload u = { pDst, (UINT)rc.left + (m_bVirtual ? m_ax : 0), (UINT)rc.top + (m_bVirtual ? m_ay : 0), rw, rh, off, bytes, JxLop() };	// [GPU 11/09 ATLAS] [MANG 11/09] lop
	m_pDev->QueueTexUpload(u);
	m_bGpuHasData = true;
}

SDL_GPUTexture* CTexGpu::PrepareForBind()
{
	if (m_bVirtual)
	{	// [GPU 11/09 ATLAS] texture ao: o trong trang atlas
		if (!m_pPage)
		{
			CAtlasMgrGpu* pA = m_pDev->m_pAtlas;
			if (!pA || !pA->Alloc(m_w, m_h, m_fi.gpu, &m_pPage, &m_ax, &m_ay))
				m_bVirtual = false;	// het cach (khong tao duoc trang): texture rieng nhu cu
			else
			{
				m_uGpuBytes = m_w * m_h * m_pPage->m_bpp; m_pDev->m_uTexBytes += m_uGpuBytes; g_uRep3GpuTexCount++; g_uRep3GpuTexBytes += m_uGpuBytes;
				if (m_pCpu) QueueUpload(NULL); else m_pDev->QueueZeroUpload(m_pPage->m_pTex, m_ax, m_ay, m_w, m_h, m_pPage->m_bpp, m_pPage->m_nLop);	// [MANG 11/09] lop
				m_bDirty = false; m_bGpuHasData = true;
			}
		}
		if (m_bVirtual)
		{
			if (m_bDirty)
			{
				if (m_bUsedThisFrame && m_pCpu)
				{	// lenh ve dau khung da tham chieu cho cu -> xin cho MOI trong trang (cho cu tra sau khung), tai toan bo
					CAtlasPageGpu* pNew = NULL; UINT x = 0, y = 0;
					if (m_pDev->m_pAtlas && m_pDev->m_pAtlas->Alloc(m_w, m_h, m_fi.gpu, &pNew, &x, &y))
					{ m_pDev->DeferAtlasFree(m_pPage, m_ax, m_ay, m_w); m_pPage = pNew; m_ax = x; m_ay = y; QueueUpload(NULL); }
					else QueueUpload(&m_rcDirty);	// khong xin duoc: tai de len cho cu (lenh truoc trong khung thay noi dung moi - hiem)
				}
				else QueueUpload(&m_rcDirty);
				m_bDirty = false;
			}
			m_pDev->TouchTex(this);
			m_bUsedThisFrame = true;
			return m_pPage->m_pTex;
		}
	}
	if (!m_pGpu)
	{
		if (m_bCpuBo && !m_pCpu) { m_pCpu = (BYTE*)calloc((size_t)m_pitch * m_h, 1); if (m_pCpu) ThuLaiCpu(); }	// [GPU 11/09 BOCPU] (khong xay ra voi sprite: m_pGpu chi mat khi NewVersion)
		if (!NewVersion(false)) return NULL;
		if (m_pCpu) QueueUpload(NULL);
		m_bDirty = false;
	}
	else if (m_bDirty)
	{
		if (m_bUsedThisFrame)
		{	// lenh ve dau khung da tham chieu phien ban nay -> phien ban moi, tai toan bo
			if (!NewVersion(m_bGpuTarget)) return NULL;
			QueueUpload(NULL);
		}
		else
			QueueUpload(&m_rcDirty);
		m_bDirty = false;
	}
	m_pDev->TouchTex(this);
	m_bUsedThisFrame = true;
	return m_pGpu;
}

SDL_GPUTexture* CTexGpu::PrepareAsTarget()
{
	if (m_bVirtual) BoAtlas();	// [GPU 11/09 ATLAS] render target khong o trong trang
	if (m_bCpuBo && !m_pCpu) { m_pCpu = (BYTE*)calloc((size_t)m_pitch * m_h, 1); if (m_pCpu) ThuLaiCpu(); }	// [GPU 11/09 BOCPU]
	if (!m_pGpu || !m_bGpuTarget)
	{
		const bool bKeep = (m_pGpu != NULL && m_bGpuNewer);	// noi dung GPU moi hon CPU: khong tai de lai (hiem; chi khi da la target)
		if (!NewVersion(true)) return NULL;
		if (m_pCpu && !bKeep) QueueUpload(NULL);
		m_bDirty = false;
	}
	else if (m_bDirty)
	{
		if (m_bUsedThisFrame) { if (!NewVersion(true)) return NULL; QueueUpload(NULL); }
		else QueueUpload(&m_rcDirty);
		m_bDirty = false;
	}
	m_bGpuNewer = true; m_bGpuHasData = true;
	m_pDev->TouchTex(this);
	m_bUsedThisFrame = true;
	return m_pGpu;
}

HRESULT CTexGpu::QueryInterface(REFIID riid, void** ppvObj)
{
	if (!ppvObj) return E_POINTER;
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDirect3DResource9) || IsEqualGUID(riid, IID_IDirect3DBaseTexture9) || IsEqualGUID(riid, IID_IDirect3DTexture9)) { *ppvObj = this; AddRef(); return S_OK; }
	*ppvObj = NULL; return E_NOINTERFACE;
}
ULONG CTexGpu::AddRef() { return (ULONG)InterlockedIncrement(&m_ref); }
ULONG CTexGpu::Release() { LONG r = InterlockedDecrement(&m_ref); if (r <= 0) { delete this; return 0; } return (ULONG)r; }
HRESULT CTexGpu::GetDevice(IDirect3DDevice9** ppDevice) { if (!ppDevice) return E_POINTER; *ppDevice = m_pDev; m_pDev->AddRef(); return D3D_OK; }

HRESULT CTexGpu::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
{
	if (!pDesc || Level != 0) return D3DERR_INVALIDCALL;
	memset(pDesc, 0, sizeof(*pDesc));
	pDesc->Format = m_fmt; pDesc->Type = D3DRTYPE_TEXTURE; pDesc->Usage = m_usage; pDesc->Pool = m_pool;
	pDesc->MultiSampleType = D3DMULTISAMPLE_NONE; pDesc->Width = m_w; pDesc->Height = m_h;
	return D3D_OK;
}

HRESULT CTexGpu::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel)
{
	if (!ppSurfaceLevel || Level != 0) return D3DERR_INVALIDCALL;
	if (!m_pSurf0) m_pSurf0 = new CSurfGpu(m_pDev, RGSURF_TEX, this, m_w, m_h, m_fmt);
	else m_pSurf0->AddRef();
	*ppSurfaceLevel = m_pSurf0;
	return D3D_OK;
}

HRESULT CTexGpu::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)
{
	if (!pLockedRect || Level != 0) return D3DERR_INVALIDCALL;
	if (m_bLocked) return D3DERR_INVALIDCALL;
	if (!m_pCpu)
	{
		m_pCpu = (BYTE*)calloc((size_t)m_pitch * m_h, 1);
		if (!m_pCpu) return E_OUTOFMEMORY;
		if (m_bCpuBo) ThuLaiCpu();	// [GPU 11/09 BOCPU] ban CPU da bo sau khi tai len: doc lai tu GPU (hiem; sprite chi ghi mot lan)
	}
	if (m_bGpuNewer && m_pGpu)
	{	// render target da ve tren GPU: doc lai ve CPU (dong bo, hiem)
		if (m_gpuFmt == m_fi.gpu && !m_fi.bConvert)
			m_pDev->ReadbackTexture(m_pGpu, m_w, m_h, m_pCpu, m_pitch);
		else
		{
			std::vector<BYTE> tmp((size_t)m_w * m_h * 4);
			if (m_pDev->ReadbackTexture(m_pGpu, m_w, m_h, &tmp[0], m_w * 4))
				for (UINT y = 0; y < m_h; y++) RgConvertRowFromBgra(m_fmt, (const DWORD*)&tmp[(size_t)y * m_w * 4], m_pCpu + (size_t)y * m_pitch, m_w);
		}
		m_bGpuNewer = false;
	}
	RECT rc; if (pRect) rc = *pRect; else SetRect(&rc, 0, 0, (int)m_w, (int)m_h);
	if (rc.left < 0) rc.left = 0; if (rc.top < 0) rc.top = 0; if (rc.right > (int)m_w) rc.right = (int)m_w; if (rc.bottom > (int)m_h) rc.bottom = (int)m_h;
	m_rcLock = rc; m_bLocked = true; m_bLockRO = (Flags & D3DLOCK_READONLY) != 0;
	pLockedRect->Pitch = (INT)m_pitch;
	pLockedRect->pBits = m_pCpu + (size_t)rc.top * m_pitch + (size_t)rc.left * m_fi.bpp;
	return D3D_OK;
}

HRESULT CTexGpu::UnlockRect(UINT Level)
{
	if (Level != 0 || !m_bLocked) return D3DERR_INVALIDCALL;
	m_bLocked = false;
	if (!m_bLockRO)
	{
		if (m_bDirty) UnionRect(&m_rcDirty, &m_rcDirty, &m_rcLock);
		else { m_rcDirty = m_rcLock; m_bDirty = true; }
	}
	return D3D_OK;
}

// [GPU 11/09 ATLAS] texture ao -> texture rieng (truoc khi lam render target). Noi dung: ban CPU (doc lai tu trang neu da bo).
void CTexGpu::BoAtlas()
{
	if (!m_bVirtual) return;
	if (m_pPage)
	{
		if (!m_pCpu) { m_pCpu = (BYTE*)calloc((size_t)m_pitch * m_h, 1); if (m_pCpu) { m_bCpuBo = true; ThuLaiCpu(); } }
		else if (m_bCpuBo) ThuLaiCpu();
		m_pDev->DeferAtlasFree(m_pPage, m_ax, m_ay, m_w);
		m_pDev->m_uTexBytes -= m_uGpuBytes; g_uRep3GpuTexCount--; g_uRep3GpuTexBytes -= m_uGpuBytes; m_uGpuBytes = 0;
		m_pPage = NULL;
	}
	m_bVirtual = false; m_bGpuHasData = false;
	if (m_pCpu) { SetRect(&m_rcDirty, 0, 0, (int)m_w, (int)m_h); m_bDirty = true; }
}

// [GPU 11/09 BOCPU] ban CPU da bo: doc lai tu GPU (texture rieng hoac vung trong trang). Dong bo - chi cho duong hiem (LockRect sprite).
bool CTexGpu::ThuLaiCpu()
{
	// m_bCpuBo giu = true trong luc doc (ReadbackRegion -> SubmitFrame(false) -> FrameReset khong bo ban CPU vua cap), xong moi ha
	SDL_GPUTexture* pSrc = GpuTex();
	if (!pSrc || !m_pCpu || !m_bGpuHasData) { m_bCpuBo = false; return false; }
	const SDL_GPUTextureFormat gf = m_bVirtual ? m_pPage->m_fmt : m_gpuFmt;
	const UINT gbpp = RgGpuBpp(gf), ox = m_bVirtual ? m_ax : 0, oy = m_bVirtual ? m_ay : 0;
	if (gbpp == 0) { m_bCpuBo = false; return false; }
	std::vector<BYTE> tmp((size_t)m_w * m_h * gbpp);
	if (!m_pDev->ReadbackRegion(pSrc, ox, oy, m_w, m_h, gbpp, &tmp[0], m_w * gbpp, JxLop())) { m_bCpuBo = false; return false; }	// [MANG 11/09] dung lop cua trang
	const bool bConv = (gf != m_fi.gpu) || m_fi.bConvert;
	for (UINT y = 0; y < m_h; y++)
	{
		const BYTE* ps = &tmp[(size_t)y * m_w * gbpp]; BYTE* pd = m_pCpu + (size_t)y * m_pitch;
		if (bConv) RgConvertRowFromBgra(m_fmt, (const DWORD*)ps, pd, m_w); else memcpy(pd, ps, (size_t)m_w * m_fi.bpp);
	}
	m_bCpuBo = false;
	m_pDev->m_uCpuBoThuLai++;
	return true;
}

// ---------------------------------------------------------------- atlas [GPU 11/09 ATLAS] (thuat toan y het CAtlasMgr cua D3D9on11Atlas.cpp)
static UINT RgAtlasBin(UINT v)
{
	static const UINT s_bins[] = { 16, 24, 32, 48, 64, 96, 128, 192, 256, 384, 512 };
	for (int i = 0; i < (int)(sizeof(s_bins) / sizeof(s_bins[0])); i++)
		if (v <= s_bins[i]) return s_bins[i];
	return 512;
}

CAtlasMgrGpu::CAtlasMgrGpu(CDevGpu* pDev)
{
	m_pDev = pDev; m_pageSize = 1024;
#ifdef JX_ANDROID
	if (g_nJxAtlasTrang == 2048 || g_nJxAtlasTrang == 4096) m_pageSize = (UINT)g_nJxAtlasTrang;	// [VE 11/09 e] [Client] Rep3AtlasTrang
#endif
}
CAtlasMgrGpu::~CAtlasMgrGpu() { ReleaseAll(); }

void CAtlasMgrGpu::ReleaseAll()
{
	for (size_t i = 0; i < m_pages.size(); i++)
	{
		CAtlasPageGpu* p = m_pages[i];
#ifdef JX_ANDROID
		if (p->m_pTex && m_pDev->m_pGpu && !g_nJxAtlasMang) SDL_ReleaseGPUTexture(m_pDev->m_pGpu, p->m_pTex);	// [MANG 11/09] texture cum huy o duoi, khong huy theo tung trang
#else
		if (p->m_pTex && m_pDev->m_pGpu) SDL_ReleaseGPUTexture(m_pDev->m_pGpu, p->m_pTex);
#endif
		delete p;
	}
	m_pages.clear();
#ifdef JX_ANDROID
	for (size_t i = 0; i < m_jxCum.size(); i++)	// [MANG 11/09] huy texture cua tung cum
		if (m_jxCum[i].pTex && m_pDev->m_pGpu) SDL_ReleaseGPUTexture(m_pDev->m_pGpu, m_jxCum[i].pTex);
	m_jxCum.clear(); g_uJxAtlasCum = 0;
#endif
	g_uRep3AtlasPages = 0; g_uRep3AtlasBytes = 0;
}

bool CAtlasMgrGpu::Eligible(UINT w, UINT h, DWORD usage, D3DFORMAT fmt, D3DPOOL pool)
{
	if (pool != D3DPOOL_DEFAULT) return false;
	if (usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DYNAMIC | D3DUSAGE_DEPTHSTENCIL)) return false;
	if (w == 0 || h == 0 || w > 512 || h > 512) return false;
	switch (fmt)
	{
	case D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8: case D3DFMT_A4R4G4B4: case D3DFMT_R5G6B5: case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5: case D3DFMT_A8L8: return true;
	default: return false;
	}
}

CAtlasPageGpu* CAtlasMgrGpu::NewPage(UINT binH, SDL_GPUTextureFormat fmt)
{
	const UINT bpp = RgGpuBpp(fmt);
	if (bpp == 0 || !m_pDev->m_pGpu) return NULL;
	SDL_GPUTexture* pTex = NULL; UINT uLop = 0;
#ifdef JX_ANDROID
	if (g_nJxAtlasMang)
	{	// [MANG 11/09] trang = mot LOP trong texture mang cua cum (hai trang cung cum -> cung texture0 -> gop duoc lenh ve)
		if (!JxCapLop(fmt, bpp, &pTex, &uLop)) return NULL;
	}
	else
#endif
	{
		SDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci));
		ci.type = SDL_GPU_TEXTURETYPE_2D; ci.format = fmt; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
		ci.width = m_pageSize; ci.height = m_pageSize; ci.layer_count_or_depth = 1; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;
		pTex = SDL_CreateGPUTexture(m_pDev->m_pGpu, &ci);
		if (!pTex) { RgLog("atlas: CreateGPUTexture trang %ux%u fmt %d that bai: %s", m_pageSize, m_pageSize, (int)fmt, SDL_GetError()); return NULL; }
	}
	m_pDev->QueueZeroUpload(pTex, 0, 0, m_pageSize, m_pageSize, bpp, uLop);	// trang moi = 0 (khong de rac; vien o khi loc tuyen tinh)
	CAtlasPageGpu* p = new CAtlasPageGpu();
	p->m_nLop = uLop;	// [MANG 11/09]
#ifdef JX_ANDROID
	p->m_yTiep = 0;
	if (g_nJxAtlasKe) { p->m_pTex = pTex; p->m_fmt = fmt; p->m_bpp = bpp; p->m_binH = 0; p->m_rows = 0; p->m_used = 0; }	// [VE 11/09 e] trang xep ke: chua co hang
	else
	{
#endif
	p->m_pTex = pTex; p->m_fmt = fmt; p->m_bpp = bpp; p->m_binH = binH; p->m_rows = m_pageSize / binH; p->m_used = 0;
	p->m_free.resize(p->m_rows);
	for (UINT r = 0; r < p->m_rows; r++) p->m_free[r].push_back(std::make_pair(0u, m_pageSize));	// ca hang trong
#ifdef JX_ANDROID
	}
#endif
	m_pages.push_back(p);
	g_uRep3AtlasPages++; g_uRep3AtlasBytes += (unsigned __int64)m_pageSize * m_pageSize * bpp;
	return p;
}

bool CAtlasMgrGpu::Alloc(UINT w, UINT h, SDL_GPUTextureFormat fmt, CAtlasPageGpu** ppPage, UINT* pX, UINT* pY)
{
#ifdef JX_ANDROID
	if (g_nJxAtlasKe) return JxAllocKe(w, h, fmt, ppPage, pX, pY);	// [VE 11/09 e]
#endif
	UINT binH = RgAtlasBin(h);
	if (h > binH || w > m_pageSize) return false;
	for (int lan = 0; lan < 2; lan++)
	{
		for (size_t i = 0; i < m_pages.size(); i++)
		{
			CAtlasPageGpu* p = m_pages[i];
			if (p->m_binH != binH || p->m_fmt != fmt) continue;
			for (UINT r = 0; r < p->m_rows; r++)
			{
				std::vector<std::pair<UINT, UINT> >& fr = p->m_free[r];
				for (size_t k = 0; k < fr.size(); k++)
				{
					if (fr[k].second - fr[k].first < w) continue;
					UINT x = fr[k].first;
					fr[k].first += w;
					if (fr[k].first >= fr[k].second) fr.erase(fr.begin() + k);
					p->m_used++;
					*ppPage = p; *pX = x; *pY = r * binH;
					return true;
				}
			}
		}
		if (!NewPage(binH, fmt)) return false;	// lan 2: thu lai voi trang moi
	}
	return false;
}

void CAtlasMgrGpu::Free(CAtlasPageGpu* pPage, UINT x, UINT y, UINT w)
{
#ifdef JX_ANDROID
	if (g_nJxAtlasKe) { JxFreeKe(pPage, x, y, w); return; }	// [VE 11/09 e]
#endif
	if (!pPage || pPage->m_binH == 0) return;
	UINT r = y / pPage->m_binH;
	if (r >= pPage->m_rows) return;
	std::vector<std::pair<UINT, UINT> >& fr = pPage->m_free[r];
	UINT x0 = x, x1 = x + w;
	size_t k = 0;
	while (k < fr.size() && fr[k].first < x0) k++;
	fr.insert(fr.begin() + k, std::make_pair(x0, x1));
	if (k + 1 < fr.size() && fr[k].second == fr[k + 1].first) { fr[k].second = fr[k + 1].second; fr.erase(fr.begin() + k + 1); }
	if (k > 0 && fr[k - 1].second == fr[k].first) { fr[k - 1].second = fr[k].second; fr.erase(fr.begin() + k); }
	if (pPage->m_used) pPage->m_used--;
	if (pPage->m_used == 0)
	{
		// giu toi da MOT trang rong moi lop chieu cao; trang rong thu hai thi tra lai GPU (sau khung: DeferRelease)
		int nEmptySameClass = 0;
		for (size_t i = 0; i < m_pages.size(); i++)
			if (m_pages[i] != pPage && m_pages[i]->m_binH == pPage->m_binH && m_pages[i]->m_fmt == pPage->m_fmt && m_pages[i]->m_used == 0) nEmptySameClass++;
		if (nEmptySameClass >= 1)
		{
			for (size_t i = 0; i < m_pages.size(); i++)
				if (m_pages[i] == pPage) { m_pages.erase(m_pages.begin() + i); break; }
#ifdef JX_ANDROID
			if (g_nJxAtlasMang) JxTraLop(pPage);	// [MANG 11/09] texture la cua CUM (dung chung): chi tra lop, khong huy
			else if (pPage->m_pTex) m_pDev->DeferRelease(pPage->m_pTex);
#else
			if (pPage->m_pTex) m_pDev->DeferRelease(pPage->m_pTex);
#endif
			if (g_uRep3AtlasPages) g_uRep3AtlasPages--;
			g_uRep3AtlasBytes -= (unsigned __int64)m_pageSize * m_pageSize * pPage->m_bpp;
			delete pPage;
		}
	}
}

// ---------------------------------------------------------------- CSurfGpu
#ifdef JX_ANDROID
// [VE 11/09 e] atlas xep KE theo dinh dang: trang chi phan biet dinh dang; ke (hang) cao = bin cua khung, mo dan tu y = 0 den het trang.
// Khung giai ma gan nhau (cung NPC / cung dam dong) roi vao cung trang -> lenh ve lien tiep cung texture0 -> gop duoc (Fold 7 09:31:
// 86 % quad khong gop la do doi trang). Ke cuoi trang rong -> thu lai (m_yTiep lui); trang rong -> xep lai tu dau; giu mot trang rong/dinh dang.
bool CAtlasMgrGpu::JxAllocKe(UINT w, UINT h, SDL_GPUTextureFormat fmt, CAtlasPageGpu** ppPage, UINT* pX, UINT* pY)
{
	const UINT binH = RgAtlasBin(h);
	if (h > binH || w > m_pageSize) return false;
	for (int lan = 0; lan < 2; lan++)
	{
		for (size_t i = 0; i < m_pages.size(); i++)
		{
			CAtlasPageGpu* p = m_pages[i];
			if (p->m_fmt != fmt) continue;
			for (size_t k = 0; k < p->m_ke.size(); k++)
			{	// ke da co cung chieu cao: first-fit
				CAtlasPageGpu::JxKe& ke = p->m_ke[k];
				if (ke.h != binH) continue;
				for (size_t d = 0; d < ke.free.size(); d++)
				{
					if (ke.free[d].second - ke.free[d].first < w) continue;
					const UINT x = ke.free[d].first;
					ke.free[d].first += w;
					if (ke.free[d].first >= ke.free[d].second) ke.free.erase(ke.free.begin() + d);
					ke.used++; p->m_used++;
					*ppPage = p; *pX = x; *pY = ke.y;
					return true;
				}
			}
			if (p->m_yTiep + binH <= m_pageSize)
			{	// mo ke moi
				CAtlasPageGpu::JxKe ke; ke.y = p->m_yTiep; ke.h = binH; ke.used = 1;
				if (w < m_pageSize) ke.free.push_back(std::make_pair(w, m_pageSize));
				p->m_yTiep += binH; p->m_ke.push_back(ke); p->m_used++;
				*ppPage = p; *pX = 0; *pY = ke.y;
				return true;
			}
		}
		if (!NewPage(0, fmt)) return false;	// lan 2: thu lai voi trang moi
	}
	return false;
}

void CAtlasMgrGpu::JxFreeKe(CAtlasPageGpu* pPage, UINT x, UINT y, UINT w)
{
	if (!pPage) return;
	for (size_t k = 0; k < pPage->m_ke.size(); k++)
	{
		CAtlasPageGpu::JxKe& ke = pPage->m_ke[k];
		if (ke.y != y) continue;
		std::vector<std::pair<UINT, UINT> >& fr = ke.free;
		const UINT x0 = x, x1 = x + w;
		size_t d = 0;
		while (d < fr.size() && fr[d].first < x0) d++;
		fr.insert(fr.begin() + d, std::make_pair(x0, x1));
		if (d + 1 < fr.size() && fr[d].second == fr[d + 1].first) { fr[d].second = fr[d + 1].second; fr.erase(fr.begin() + d + 1); }
		if (d > 0 && fr[d - 1].second == fr[d].first) { fr[d - 1].second = fr[d].second; fr.erase(fr.begin() + d); }
		if (ke.used) ke.used--;
		break;
	}
	while (!pPage->m_ke.empty() && pPage->m_ke.back().used == 0)
	{	// ke cuoi trang rong: thu lai de mo ke cao khac
		pPage->m_yTiep = pPage->m_ke.back().y;
		pPage->m_ke.pop_back();
	}
	if (pPage->m_used) pPage->m_used--;
	if (pPage->m_used == 0)
	{
		pPage->m_ke.clear(); pPage->m_yTiep = 0;
		int nEmptySameFmt = 0;
		for (size_t i = 0; i < m_pages.size(); i++)
			if (m_pages[i] != pPage && m_pages[i]->m_fmt == pPage->m_fmt && m_pages[i]->m_used == 0) nEmptySameFmt++;
		if (nEmptySameFmt >= 1)
		{	// giu toi da MOT trang rong moi dinh dang; trang rong thu hai tra lai GPU (sau khung: DeferRelease)
			for (size_t i = 0; i < m_pages.size(); i++)
				if (m_pages[i] == pPage) { m_pages.erase(m_pages.begin() + i); break; }
#ifdef JX_ANDROID
			if (g_nJxAtlasMang) JxTraLop(pPage);	// [MANG 11/09] texture la cua CUM (dung chung): chi tra lop, khong huy
			else if (pPage->m_pTex) m_pDev->DeferRelease(pPage->m_pTex);
#else
			if (pPage->m_pTex) m_pDev->DeferRelease(pPage->m_pTex);
#endif
			if (g_uRep3AtlasPages) g_uRep3AtlasPages--;
			g_uRep3AtlasBytes -= (unsigned __int64)m_pageSize * m_pageSize * pPage->m_bpp;
			delete pPage;
		}
	}
}
#endif

#ifdef JX_ANDROID
// [MANG 11/09] Cap mot LOP cho trang atlas: tim cum cung dinh dang con lop (lop da tra truoc, roi lop chua dung), het thi tao cum moi.
// So lop moi cum tang dan (2, 4, 8...) va khong vuot ngan sach byte Rep3AtlasCumMB -> khong cap 64 MB ngay khi vao map.
bool CAtlasMgrGpu::JxCapLop(SDL_GPUTextureFormat fmt, UINT bpp, SDL_GPUTexture** ppTex, UINT* pLop)
{
	for (size_t i = 0; i < m_jxCum.size(); i++)
	{
		JxCum& c = m_jxCum[i];
		if (c.fmt != fmt) continue;
		if (!c.lopTrong.empty()) { *ppTex = c.pTex; *pLop = c.lopTrong.back(); c.lopTrong.pop_back(); return true; }
		if (c.nLopTiep < c.nLop) { *ppTex = c.pTex; *pLop = c.nLopTiep++; return true; }
	}
	UINT nLop = 2;
	for (size_t i = 0; i < m_jxCum.size(); i++) if (m_jxCum[i].fmt == fmt) nLop *= 2;	// cum sau nhieu lop hon cum truoc
	if ((int)nLop > g_nJxAtlasLop) nLop = (UINT)g_nJxAtlasLop;
	{
		const unsigned __int64 uMotLop = (unsigned __int64)m_pageSize * m_pageSize * bpp;
		const unsigned __int64 uNganSach = (unsigned __int64)g_nJxAtlasCumMB << 20;
		UINT nToiDa = (UINT)(uMotLop ? (uNganSach / uMotLop) : 1); if (nToiDa < 1) nToiDa = 1;
		if (nLop > nToiDa) nLop = nToiDa;
	}
	SDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci));
	ci.type = SDL_GPU_TEXTURETYPE_2D_ARRAY; ci.format = fmt; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
	ci.width = m_pageSize; ci.height = m_pageSize; ci.layer_count_or_depth = nLop; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;
	SDL_GPUTexture* pTex = SDL_CreateGPUTexture(m_pDev->m_pGpu, &ci);
	if (!pTex)
	{
		RgLog("atlas mang: CreateGPUTexture %ux%u x %u lop fmt %d that bai: %s", m_pageSize, m_pageSize, nLop, (int)fmt, SDL_GetError());
		return false;
	}
	JxCum c; c.pTex = pTex; c.fmt = fmt; c.bpp = bpp; c.nLop = nLop; c.nLopTiep = 1;
	m_jxCum.push_back(c);
	g_uJxAtlasCum = (unsigned)m_jxCum.size();
	RgLog("[MANG] cum atlas moi: %ux%u x %u lop fmt %d (%u MB), tong %u cum", m_pageSize, m_pageSize, nLop, (int)fmt,
		(unsigned)(((unsigned __int64)m_pageSize * m_pageSize * bpp * nLop) >> 20), (unsigned)m_jxCum.size());
	*ppTex = pTex; *pLop = 0;
	return true;
}

void CAtlasMgrGpu::JxTraLop(CAtlasPageGpu* pPage)
{
	if (!pPage || !pPage->m_pTex) return;
	for (size_t i = 0; i < m_jxCum.size(); i++)
		if (m_jxCum[i].pTex == pPage->m_pTex) { m_jxCum[i].lopTrong.push_back(pPage->m_nLop); return; }
}
#endif

CSurfGpu::CSurfGpu(CDevGpu* pDev, RgSurfKind kind, CTexGpu* pTex, UINT w, UINT h, D3DFORMAT fmt)
{
	m_ref = 1; m_pDev = pDev; m_kind = kind; m_pTex = pTex; m_w = w; m_h = h; m_fmt = fmt; m_pCpu = NULL; m_pitch = RgPitch(fmt, w);
	if (kind == RGSURF_TEX && pTex) pTex->AddRef();
	if (kind == RGSURF_OFFSCREEN) m_pCpu = (BYTE*)calloc((size_t)m_pitch * h, 1);
	pDev->AddRef();
}
CSurfGpu::~CSurfGpu()
{
	if (m_kind == RGSURF_TEX && m_pTex) { m_pTex->m_pSurf0 = NULL; m_pTex->Release(); }
	if (m_pCpu) free(m_pCpu);
	if (m_pDev) m_pDev->Release();
}
HRESULT CSurfGpu::QueryInterface(REFIID riid, void** ppvObj)
{
	if (!ppvObj) return E_POINTER;
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDirect3DResource9) || IsEqualGUID(riid, IID_IDirect3DSurface9)) { *ppvObj = this; AddRef(); return S_OK; }
	*ppvObj = NULL; return E_NOINTERFACE;
}
ULONG CSurfGpu::AddRef() { return (ULONG)InterlockedIncrement(&m_ref); }
ULONG CSurfGpu::Release() { LONG r = InterlockedDecrement(&m_ref); if (r <= 0) { delete this; return 0; } return (ULONG)r; }
HRESULT CSurfGpu::GetDevice(IDirect3DDevice9** ppDevice) { if (!ppDevice) return E_POINTER; *ppDevice = m_pDev; m_pDev->AddRef(); return D3D_OK; }
HRESULT CSurfGpu::GetContainer(REFIID riid, void** ppContainer)
{
	if (!ppContainer) return E_POINTER;
	if (m_kind == RGSURF_TEX && m_pTex) { *ppContainer = m_pTex; m_pTex->AddRef(); return D3D_OK; }
	*ppContainer = NULL; return E_NOINTERFACE;
}
HRESULT CSurfGpu::GetDesc(D3DSURFACE_DESC* pDesc)
{
	if (!pDesc) return E_POINTER;
	memset(pDesc, 0, sizeof(*pDesc));
	pDesc->Format = m_fmt; pDesc->Type = D3DRTYPE_SURFACE; pDesc->MultiSampleType = D3DMULTISAMPLE_NONE; pDesc->Width = m_w; pDesc->Height = m_h;
	if (m_kind == RGSURF_TEX && m_pTex) { pDesc->Usage = m_pTex->m_usage; pDesc->Pool = m_pTex->m_pool; }
	else if (m_kind == RGSURF_BACKBUFFER) { pDesc->Usage = D3DUSAGE_RENDERTARGET; pDesc->Pool = D3DPOOL_DEFAULT; }
	else pDesc->Pool = D3DPOOL_SYSTEMMEM;
	return D3D_OK;
}
HRESULT CSurfGpu::LockRect(D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)
{
	if (!pLockedRect) return E_POINTER;
	if (m_kind == RGSURF_TEX && m_pTex) return m_pTex->LockRect(0, pLockedRect, pRect, Flags);
	if (m_kind == RGSURF_OFFSCREEN && m_pCpu)
	{
		RECT rc; if (pRect) rc = *pRect; else SetRect(&rc, 0, 0, (int)m_w, (int)m_h);
		RgFmt fi = RgFormatInfo(m_fmt);
		pLockedRect->Pitch = (INT)m_pitch; pLockedRect->pBits = m_pCpu + (size_t)rc.top * m_pitch + (size_t)rc.left * (fi.bpp ? fi.bpp : 4);
		return D3D_OK;
	}
	RgStub("Surface::LockRect(backbuffer)");
	return D3DERR_INVALIDCALL;
}
HRESULT CSurfGpu::UnlockRect()
{
	if (m_kind == RGSURF_TEX && m_pTex) return m_pTex->UnlockRect(0);
	return D3D_OK;
}

// ---------------------------------------------------------------- CVBGpu
CVBGpu::CVBGpu(CDevGpu* pDev, UINT len, DWORD usage, DWORD fvf, D3DPOOL pool)
{
	m_ref = 1; m_pDev = pDev; m_len = len; m_usage = usage; m_fvf = fvf; m_pool = pool;
	m_pCpu = (BYTE*)calloc(len ? len : 1, 1);
	pDev->AddRef();
}
CVBGpu::~CVBGpu() { if (m_pCpu) free(m_pCpu); if (m_pDev) m_pDev->Release(); }
HRESULT CVBGpu::QueryInterface(REFIID riid, void** ppvObj)
{
	if (!ppvObj) return E_POINTER;
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDirect3DResource9) || IsEqualGUID(riid, IID_IDirect3DVertexBuffer9)) { *ppvObj = this; AddRef(); return S_OK; }
	*ppvObj = NULL; return E_NOINTERFACE;
}
ULONG CVBGpu::AddRef() { return (ULONG)InterlockedIncrement(&m_ref); }
ULONG CVBGpu::Release() { LONG r = InterlockedDecrement(&m_ref); if (r <= 0) { delete this; return 0; } return (ULONG)r; }
HRESULT CVBGpu::GetDevice(IDirect3DDevice9** ppDevice) { if (!ppDevice) return E_POINTER; *ppDevice = m_pDev; m_pDev->AddRef(); return D3D_OK; }
HRESULT CVBGpu::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
	if (!ppbData) return E_POINTER;
	if (OffsetToLock >= m_len || (SizeToLock && OffsetToLock + SizeToLock > m_len)) return D3DERR_INVALIDCALL;
	*ppbData = m_pCpu + OffsetToLock;
	return D3D_OK;
}
HRESULT CVBGpu::GetDesc(D3DVERTEXBUFFER_DESC* pDesc)
{
	if (!pDesc) return E_POINTER;
	pDesc->Format = D3DFMT_VERTEXDATA; pDesc->Type = D3DRTYPE_VERTEXBUFFER; pDesc->Usage = m_usage; pDesc->Pool = m_pool; pDesc->Size = m_len; pDesc->FVF = m_fvf;
	return D3D_OK;
}

// ---------------------------------------------------------------- CSBGpu
CSBGpu::CSBGpu(CDevGpu* pDev) { m_ref = 1; m_pDev = pDev; pDev->AddRef(); }
CSBGpu::~CSBGpu() { if (m_pDev) m_pDev->Release(); }
HRESULT CSBGpu::QueryInterface(REFIID riid, void** ppvObj)
{
	if (!ppvObj) return E_POINTER;
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDirect3DStateBlock9)) { *ppvObj = this; AddRef(); return S_OK; }
	*ppvObj = NULL; return E_NOINTERFACE;
}
ULONG CSBGpu::AddRef() { return (ULONG)InterlockedIncrement(&m_ref); }
ULONG CSBGpu::Release() { LONG r = InterlockedDecrement(&m_ref); if (r <= 0) { delete this; return 0; } return (ULONG)r; }
HRESULT CSBGpu::GetDevice(IDirect3DDevice9** ppDevice) { if (!ppDevice) return E_POINTER; *ppDevice = m_pDev; m_pDev->AddRef(); return D3D_OK; }
void CSBGpu::Record(DWORD key, DWORD value)
{
	for (size_t i = 0; i < m_entries.size(); i++) if (m_entries[i].first == key) { m_entries[i].second = value; return; }
	m_entries.push_back(std::make_pair(key, value));
}
HRESULT CSBGpu::Capture()
{
	for (size_t i = 0; i < m_entries.size(); i++) m_entries[i].second = m_pDev->GetStateInternal(m_entries[i].first);
	return D3D_OK;
}
HRESULT CSBGpu::Apply()
{
	for (size_t i = 0; i < m_entries.size(); i++) m_pDev->SetStateInternal(m_entries[i].first, m_entries[i].second);
	return D3D_OK;
}

#endif // JX_PLATFORM_SDL
