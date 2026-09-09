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
	SetRect(&m_rcDirty, 0, 0, 0, 0); SetRect(&m_rcLock, 0, 0, 0, 0);
	if (!(usage & D3DUSAGE_RENDERTARGET))
		m_pCpu = (BYTE*)calloc((size_t)m_pitch * h, 1);	// render target: khong ban CPU, cap khi Lock
	pDev->AddRef();
}

CTexGpu::~CTexGpu()
{
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
	ci.type = SDL_GPU_TEXTURETYPE_2D; ci.format = gf;
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
	if (!m_pCpu || !m_pGpu) return;
	RECT rc; if (prc) rc = *prc; else SetRect(&rc, 0, 0, (int)m_w, (int)m_h);
	if (rc.left < 0) rc.left = 0; if (rc.top < 0) rc.top = 0; if (rc.right > (int)m_w) rc.right = (int)m_w; if (rc.bottom > (int)m_h) rc.bottom = (int)m_h;
	if (rc.right <= rc.left || rc.bottom <= rc.top) return;
	const UINT rw = (UINT)(rc.right - rc.left), rh = (UINT)(rc.bottom - rc.top);
	const bool bConv = (m_gpuFmt != m_fi.gpu) || m_fi.bConvert;
	const UINT gbpp = RgGpuBpp(m_gpuFmt);
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
	RgTexUpload u = { m_pGpu, (UINT)rc.left, (UINT)rc.top, rw, rh, off, bytes };
	m_pDev->QueueTexUpload(u);
	m_bGpuHasData = true;
}

SDL_GPUTexture* CTexGpu::PrepareForBind()
{
	if (!m_pGpu)
	{
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

// ---------------------------------------------------------------- CSurfGpu
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
