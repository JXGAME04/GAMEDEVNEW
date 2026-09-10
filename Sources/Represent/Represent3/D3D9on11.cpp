// [D3D11 08/09] D3D9 tren D3D11 - phan tai nguyen: dinh dang, texture, surface, vertex buffer, state block.
// Xem D3D9on11.h. Thiet bi + IDirect3D9 o D3D9on11Dev.cpp.
#include "precompile.h"
#include "BaseInclude.h"
#include "D3D9on11.h"
#include "D3D9on11i.h"
#include <stdio.h>
#include <string.h>
#include <set>
#include <string>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

unsigned          g_uRep3GpuTexCount = 0;
unsigned __int64  g_uRep3GpuTexBytes = 0;
int               g_nRep3D3D11FL = 0;

void R11Log(const char* fmt, ...)
{
	char sz[1024];
	va_list ap;
	va_start(ap, fmt);
	_vsnprintf(sz, sizeof(sz) - 1, fmt, ap);
	va_end(ap);
	sz[sizeof(sz) - 1] = 0;
	Rep3Log("[D3D11] %s", sz);
}

void R11Stub(const char* szName)
{
	static std::set<std::string> s_logged;
	if (s_logged.insert(szName).second)
		R11Log("CHUA CAI: %s (goi lan dau)", szName);
}

// ---------------------------------------------------------------- dinh dang
R11Fmt R11FormatInfo(D3DFORMAT f)
{
	R11Fmt r = { DXGI_FORMAT_UNKNOWN, 0, 0 };
	switch (f)
	{
	case D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8: r.dxgi = DXGI_FORMAT_B8G8R8A8_UNORM; r.bpp = 4; break;
	case D3DFMT_A8B8G8R8: case D3DFMT_X8B8G8R8: r.dxgi = DXGI_FORMAT_R8G8B8A8_UNORM; r.bpp = 4; break;
	case D3DFMT_A4R4G4B4: r.dxgi = DXGI_FORMAT_B4G4R4A4_UNORM; r.bpp = 2; break;
	case D3DFMT_R5G6B5:   r.dxgi = DXGI_FORMAT_B5G6R5_UNORM;   r.bpp = 2; break;
	case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5: r.dxgi = DXGI_FORMAT_B5G5R5A1_UNORM; r.bpp = 2; break;
	case D3DFMT_R8G8B8:   r.dxgi = DXGI_FORMAT_UNKNOWN; r.bpp = 3; break;	// luon doi sang BGRA8
	case D3DFMT_A8:       r.dxgi = DXGI_FORMAT_A8_UNORM; r.bpp = 1; break;
	case D3DFMT_A8L8:     r.dxgi = DXGI_FORMAT_R8G8_UNORM; r.bpp = 2; break;	// [r] chi so + alpha
	case D3DFMT_L8:       r.dxgi = DXGI_FORMAT_R8_UNORM; r.bpp = 1; break;
	case D3DFMT_P8:       r.dxgi = DXGI_FORMAT_UNKNOWN; r.bpp = 1; break;
	case D3DFMT_DXT1:     r.dxgi = DXGI_FORMAT_BC1_UNORM; r.bpp = 0; r.blockBytes = 8; break;
	case D3DFMT_DXT3:     r.dxgi = DXGI_FORMAT_BC2_UNORM; r.bpp = 0; r.blockBytes = 16; break;
	case D3DFMT_DXT5:     r.dxgi = DXGI_FORMAT_BC3_UNORM; r.bpp = 0; r.blockBytes = 16; break;
	default: break;
	}
	return r;
}

UINT R11Pitch(D3DFORMAT f, UINT w)
{
	R11Fmt r = R11FormatInfo(f);
	if (r.blockBytes) return ((w + 3) / 4) * r.blockBytes;
	return w * (r.bpp ? r.bpp : 4);
}

UINT R11Rows(D3DFORMAT f, UINT h)
{
	R11Fmt r = R11FormatInfo(f);
	if (r.blockBytes) return (h + 3) / 4;
	return h;
}

// doi mot hang sang B8G8R8A8 (DWORD 0xAARRGGBB)
void R11ConvertRowToBgra(D3DFORMAT f, const BYTE* pSrc, DWORD* pDst, UINT w)
{
	UINT x;
	switch (f)
	{
	case D3DFMT_A4R4G4B4:
		for (x = 0; x < w; x++) { WORD v = ((const WORD*)pSrc)[x];
			DWORD a = (v >> 12) & 15, r = (v >> 8) & 15, g = (v >> 4) & 15, b = v & 15;
			pDst[x] = ((a * 17) << 24) | ((r * 17) << 16) | ((g * 17) << 8) | (b * 17); }
		break;
	case D3DFMT_R5G6B5:
		for (x = 0; x < w; x++) { WORD v = ((const WORD*)pSrc)[x];
			DWORD r = (v >> 11) & 31, g = (v >> 5) & 63, b = v & 31;
			pDst[x] = 0xFF000000 | (((r << 3) | (r >> 2)) << 16) | (((g << 2) | (g >> 4)) << 8) | ((b << 3) | (b >> 2)); }
		break;
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		for (x = 0; x < w; x++) { WORD v = ((const WORD*)pSrc)[x];
			DWORD r = (v >> 10) & 31, g = (v >> 5) & 31, b = v & 31;
			DWORD a = (f == D3DFMT_A1R5G5B5) ? ((v & 0x8000) ? 0xFF : 0) : 0xFF;
			pDst[x] = (a << 24) | (((r << 3) | (r >> 2)) << 16) | (((g << 3) | (g >> 2)) << 8) | ((b << 3) | (b >> 2)); }
		break;
	case D3DFMT_R8G8B8:
		for (x = 0; x < w; x++) pDst[x] = 0xFF000000 | (pSrc[x * 3 + 2] << 16) | (pSrc[x * 3 + 1] << 8) | pSrc[x * 3];
		break;
	case D3DFMT_P8:
	case D3DFMT_L8:
		for (x = 0; x < w; x++) pDst[x] = 0xFF000000 | (pSrc[x] << 16) | (pSrc[x] << 8) | pSrc[x];
		break;
	case D3DFMT_A8:
		for (x = 0; x < w; x++) pDst[x] = ((DWORD)pSrc[x] << 24) | 0x00FFFFFF;
		break;
	default:	// 32 bit: chep
		memcpy(pDst, pSrc, w * 4);
		break;
	}
}

// ---------------------------------------------------------------- CTex11
CTex11::CTex11(CDev11* pDev, UINT w, UINT h, DWORD usage, D3DFORMAT fmt, D3DPOOL pool)
{
	m_ref = 1; m_pDev = pDev; m_w = w; m_h = h; m_usage = usage; m_fmt = fmt; m_pool = pool;
	m_pitch = R11Pitch(fmt, w);
	m_pCpu = NULL; m_pGpu = NULL; m_pSrv = NULL; m_pRtv = NULL;
	m_dxgi = DXGI_FORMAT_UNKNOWN; m_bConvert = false; m_bDirty = false; m_bLocked = false; m_uGpuBytes = 0; m_pSurf0 = NULL;
	m_bVirtual = false; m_pPage = NULL; m_slot = 0; m_ax = 0; m_ay = 0; m_nPalRow = -1;
	SetRect(&m_rcDirty, 0, 0, 0, 0); SetRect(&m_rcLock, 0, 0, 0, 0);
	pDev->AddRef();
}

CTex11::~CTex11()
{
	ReleaseGpu();
	if (m_pCpu) { free(m_pCpu); m_pCpu = NULL; }
	if (m_pDev) m_pDev->Release();
}

void CTex11::AllocCpu()
{
	if (m_pCpu) return;
	UINT rows = R11Rows(m_fmt, m_h);
	m_pCpu = (BYTE*)calloc(1, (size_t)m_pitch * rows + 16);
}

void CTex11::ReleaseGpu()
{
	if (m_pDev && (m_pSrv || m_pPage)) m_pDev->FlushIfPending();	// [j] SRV cua texture/trang co the nam trong lo dang cho
	if (m_bVirtual)
	{
		if (m_pPage && m_pDev->m_pAtlas) m_pDev->m_pAtlas->Free(m_pPage, m_ax, m_ay, m_w);
		m_pPage = NULL; m_pSrv = NULL;	// SRV cua trang, khong so huu
		if (m_uGpuBytes) { if (g_uRep3GpuTexCount) g_uRep3GpuTexCount--; g_uRep3GpuTexBytes -= m_uGpuBytes; m_uGpuBytes = 0; }
		return;
	}
	if (m_pRtv) { m_pRtv->Release(); m_pRtv = NULL; }
	if (m_pSrv) { m_pSrv->Release(); m_pSrv = NULL; }
	if (m_pGpu)
	{
		m_pGpu->Release(); m_pGpu = NULL;
		if (g_uRep3GpuTexCount) g_uRep3GpuTexCount--;
		g_uRep3GpuTexBytes -= m_uGpuBytes; m_uGpuBytes = 0;
	}
}

HRESULT CTex11::EnsureGpu(const BYTE* pInit)
{
	if (m_pGpu) return D3D_OK;
	if (m_bVirtual)
	{
		if (m_pPage) return D3D_OK;
		DXGI_FORMAT fmtPage = (m_fmt == D3DFMT_A8L8) ? DXGI_FORMAT_R8G8_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM;	// [r]
		if (!m_pDev->m_pAtlas || !m_pDev->m_pAtlas->Alloc(m_w, m_h, fmtPage, &m_pPage, &m_ax, &m_ay))
		{
			m_bVirtual = false;	// het cach: texture rieng
		}
		else
		{
			m_pSrv = m_pPage->m_pSrv; m_dxgi = m_pPage->m_fmt;
			m_bConvert = (m_pPage->m_bpp == 4) && (m_fmt != D3DFMT_A8R8G8B8 && m_fmt != D3DFMT_X8R8G8B8);
			m_uGpuBytes = m_w * m_h * m_pPage->m_bpp; g_uRep3GpuTexCount++; g_uRep3GpuTexBytes += m_uGpuBytes;
			m_bDirty = false;
			if (pInit) { BYTE* pSave = m_pCpu; m_pCpu = (BYTE*)pInit; HRESULT hrU = UploadRect(NULL); m_pCpu = pSave; return hrU; }
			// khong co du lieu: xoa o (tranh rac cua texture cu)
			BYTE* pZero = (BYTE*)calloc(1, (size_t)m_w * m_h * m_pPage->m_bpp);
			if (pZero) { D3D11_BOX bz; bz.left = m_ax; bz.top = m_ay; bz.right = m_ax + m_w; bz.bottom = m_ay + m_h; bz.front = 0; bz.back = 1;
				m_pDev->m_pCtx->UpdateSubresource(m_pPage->m_pTex, m_pPage->m_lop, &bz, pZero, m_w * m_pPage->m_bpp, 0); free(pZero); }	// [MANG 09/09] lop cua trang
			return D3D_OK;
		}
	}
	bool bRt = (m_usage & D3DUSAGE_RENDERTARGET) != 0;
	R11Fmt fi = R11FormatInfo(m_fmt);
	bool bNative = (fi.dxgi != DXGI_FORMAT_UNKNOWN) && m_pDev->FormatTexOK(m_fmt, bRt);
	m_dxgi = bNative ? fi.dxgi : DXGI_FORMAT_B8G8R8A8_UNORM;
	m_bConvert = !bNative;
	if (m_bConvert && fi.blockBytes)
		return D3DERR_NOTAVAILABLE;			// DXT khong doi duoc tren CPU o day

	D3D11_TEXTURE2D_DESC td; memset(&td, 0, sizeof(td));
	td.Width = m_w; td.Height = m_h; td.MipLevels = 1; td.ArraySize = 1; td.Format = m_dxgi; td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE | (bRt ? D3D11_BIND_RENDER_TARGET : 0);

	D3D11_SUBRESOURCE_DATA sr; memset(&sr, 0, sizeof(sr));
	BYTE* pConv = NULL;
	if (pInit)
	{
		if (m_bConvert)
		{
			pConv = (BYTE*)malloc((size_t)m_w * m_h * 4);
			if (!pConv) return E_OUTOFMEMORY;
			for (UINT y = 0; y < m_h; y++)
				R11ConvertRowToBgra(m_fmt, pInit + y * m_pitch, (DWORD*)(pConv + y * m_w * 4), m_w);
			sr.pSysMem = pConv; sr.SysMemPitch = m_w * 4;
		}
		else
		{
			sr.pSysMem = pInit; sr.SysMemPitch = m_pitch;
		}
	}
	HRESULT hr = m_pDev->m_pDev->CreateTexture2D(&td, pInit ? &sr : NULL, &m_pGpu);
	if (pConv) free(pConv);
	if (FAILED(hr) || !m_pGpu)
	{
		R11Log("CreateTexture2D %ux%u fmt %d (dxgi %d) rt=%d that bai 0x%08X", m_w, m_h, (int)m_fmt, (int)m_dxgi, (int)bRt, (unsigned)hr);
		m_pGpu = NULL;
		return D3DERR_OUTOFVIDEOMEMORY;
	}
	if (g_nRep3AtlasMang)
	{	// [MANG 09/09] shader khai bao Texture2DArray -> SRV texture rieng cung la view MANG 1 lop
		D3D11_SHADER_RESOURCE_VIEW_DESC vd; memset(&vd, 0, sizeof(vd));
		vd.Format = m_dxgi; vd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY; vd.Texture2DArray.MostDetailedMip = 0; vd.Texture2DArray.MipLevels = 1; vd.Texture2DArray.FirstArraySlice = 0; vd.Texture2DArray.ArraySize = 1;
		hr = m_pDev->m_pDev->CreateShaderResourceView(m_pGpu, &vd, &m_pSrv);
	}
	else
		hr = m_pDev->m_pDev->CreateShaderResourceView(m_pGpu, NULL, &m_pSrv);
	if (FAILED(hr)) { R11Log("CreateShaderResourceView that bai 0x%08X", (unsigned)hr); ReleaseGpu(); return D3DERR_OUTOFVIDEOMEMORY; }
	if (bRt)
	{
		hr = m_pDev->m_pDev->CreateRenderTargetView(m_pGpu, NULL, &m_pRtv);
		if (FAILED(hr)) { R11Log("CreateRenderTargetView that bai 0x%08X", (unsigned)hr); ReleaseGpu(); return D3DERR_OUTOFVIDEOMEMORY; }
	}
	m_uGpuBytes = (m_bConvert ? m_w * 4 : m_pitch) * R11Rows(m_fmt, m_h);
	g_uRep3GpuTexCount++;
	g_uRep3GpuTexBytes += m_uGpuBytes;
	m_bDirty = false;
	return D3D_OK;
}

HRESULT CTex11::UploadRect(const RECT* prc)
{
	if (!m_pCpu) return D3DERR_INVALIDCALL;
	m_pDev->FlushIfPending();	// [j] lenh dang cho co the dang doc texture nay
	if (m_bVirtual && !m_pPage) return EnsureGpu(m_pCpu);
	if (!m_bVirtual && !m_pGpu) return EnsureGpu(m_pCpu);
	RECT rc = { 0, 0, (LONG)m_w, (LONG)m_h };
	if (prc)
	{
		rc = *prc;
		if (rc.left < 0) rc.left = 0; if (rc.top < 0) rc.top = 0;
		if (rc.right > (LONG)m_w) rc.right = m_w; if (rc.bottom > (LONG)m_h) rc.bottom = m_h;
		if (rc.right <= rc.left || rc.bottom <= rc.top) return D3D_OK;
	}
	R11Fmt fi = R11FormatInfo(m_fmt);
	if (fi.blockBytes)	// DXT: day ca texture
	{
		m_pDev->m_pCtx->UpdateSubresource(m_pGpu, 0, NULL, m_pCpu, m_pitch, 0);
		return D3D_OK;
	}
	UINT ox = m_bVirtual ? m_ax : 0, oy = m_bVirtual ? m_ay : 0;
	ID3D11Texture2D* pDst = m_bVirtual ? m_pPage->m_pTex : m_pGpu;
	const UINT uSub = m_bVirtual ? m_pPage->m_lop : 0;	// [MANG 09/09] lop cua trang trong mang (che do cu = 0)
	D3D11_BOX box; box.left = ox + rc.left; box.top = oy + rc.top; box.right = ox + rc.right; box.bottom = oy + rc.bottom; box.front = 0; box.back = 1;
	UINT w = rc.right - rc.left, h = rc.bottom - rc.top;
	if (m_bConvert)
	{
		BYTE* pConv = (BYTE*)malloc((size_t)w * h * 4);
		if (!pConv) return E_OUTOFMEMORY;
		for (UINT y = 0; y < h; y++)
			R11ConvertRowToBgra(m_fmt, m_pCpu + (rc.top + y) * m_pitch + rc.left * fi.bpp, (DWORD*)(pConv + y * w * 4), w);
		m_pDev->m_pCtx->UpdateSubresource(pDst, uSub, &box, pConv, w * 4, 0);
		free(pConv);
	}
	else
		m_pDev->m_pCtx->UpdateSubresource(pDst, uSub, &box, m_pCpu + rc.top * m_pitch + rc.left * fi.bpp, m_pitch, 0);
	return D3D_OK;
}

HRESULT CTex11::PrepareForBind()
{
	if (m_bVirtual)
	{
		if (!m_pPage) { HRESULT hr = EnsureGpu(m_pCpu); m_bDirty = false; return hr; }
		m_pSrv = m_pPage->m_pSrv;	// [MANG 09/09] SRV cua mang co the da doi khi mang lon len
		if (m_bDirty && m_pCpu) { HRESULT hr = UploadRect(&m_rcDirty); m_bDirty = false; return hr; }
		return D3D_OK;
	}
	if (!m_pGpu)
	{
		if (m_pCpu) { HRESULT hr = EnsureGpu(m_pCpu); m_bDirty = false; return hr; }
		return EnsureGpu(NULL);		// DEFAULT chua bao gio duoc UpdateTexture: texture rong
	}
	if (m_bDirty && m_pCpu)
	{
		HRESULT hr = UploadRect(&m_rcDirty);
		m_bDirty = false;
		return hr;
	}
	return D3D_OK;
}

HRESULT CTex11::QueryInterface(REFIID riid, void** ppvObj)
{
	if (!ppvObj) return E_POINTER;
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDirect3DResource9) || IsEqualGUID(riid, IID_IDirect3DBaseTexture9) || IsEqualGUID(riid, IID_IDirect3DTexture9))
	{ *ppvObj = this; AddRef(); return S_OK; }
	*ppvObj = NULL; return E_NOINTERFACE;
}
ULONG CTex11::AddRef() { return (ULONG)InterlockedIncrement(&m_ref); }
ULONG CTex11::Release() { LONG r = InterlockedDecrement(&m_ref); if (r <= 0) { delete this; return 0; } return (ULONG)r; }
HRESULT CTex11::GetDevice(IDirect3DDevice9** ppDevice) { if (!ppDevice) return E_POINTER; *ppDevice = m_pDev; m_pDev->AddRef(); return D3D_OK; }

HRESULT CTex11::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
{
	if (!pDesc || Level != 0) return D3DERR_INVALIDCALL;
	memset(pDesc, 0, sizeof(*pDesc));
	pDesc->Format = m_fmt; pDesc->Type = D3DRTYPE_SURFACE; pDesc->Usage = m_usage; pDesc->Pool = m_pool;
	pDesc->MultiSampleType = D3DMULTISAMPLE_NONE; pDesc->Width = m_w; pDesc->Height = m_h;
	return D3D_OK;
}

HRESULT CTex11::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel)
{
	if (!ppSurfaceLevel || Level != 0) return D3DERR_INVALIDCALL;
	if (!m_pSurf0)
		m_pSurf0 = new CSurf11(m_pDev, R11SURF_TEX, this, m_w, m_h, m_fmt);	// ctor giu 1 ref texture, ref surface = 1 (cua ta)
	else
		m_pSurf0->AddRef();
	*ppSurfaceLevel = m_pSurf0;
	return D3D_OK;
}

HRESULT CTex11::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)
{
	if (!pLockedRect || Level != 0) return D3DERR_INVALIDCALL;
	if (m_bLocked) return D3DERR_INVALIDCALL;
	if (!m_pCpu)
	{
		if (m_pool == D3DPOOL_DEFAULT && !(m_usage & D3DUSAGE_DYNAMIC))
		{
			R11Stub("Texture::LockRect tren POOL_DEFAULT");
			return D3DERR_INVALIDCALL;
		}
		AllocCpu();
		if (!m_pCpu) return E_OUTOFMEMORY;
	}
	R11Fmt fi = R11FormatInfo(m_fmt);
	RECT rc = { 0, 0, (LONG)m_w, (LONG)m_h };
	if (pRect) rc = *pRect;
	m_rcLock = rc;
	m_bLocked = true;
	pLockedRect->Pitch = (INT)m_pitch;
	if (fi.blockBytes)
		pLockedRect->pBits = m_pCpu + (rc.top / 4) * m_pitch + (rc.left / 4) * fi.blockBytes;
	else
		pLockedRect->pBits = m_pCpu + rc.top * m_pitch + rc.left * fi.bpp;
	if (Flags & D3DLOCK_READONLY) m_rcLock.right = m_rcLock.left;	// khong ban
	return D3D_OK;
}

HRESULT CTex11::UnlockRect(UINT Level)
{
	if (Level != 0 || !m_bLocked) return D3DERR_INVALIDCALL;
	m_bLocked = false;
	if (m_rcLock.right <= m_rcLock.left) return D3D_OK;		// READONLY
	if (m_pool == D3DPOOL_SYSTEMMEM || m_pool == D3DPOOL_SCRATCH)
		return D3D_OK;										// chi la nguon cho UpdateTexture
	// MANAGED / DYNAMIC: gop vung ban, day len khi bind (hoac ngay neu GPU da co)
	if (m_bDirty) UnionRect(&m_rcDirty, &m_rcDirty, &m_rcLock); else m_rcDirty = m_rcLock;
	m_bDirty = true;
	if (m_pGpu || (m_bVirtual && m_pPage))
	{
		m_pDev->Lock();
		UploadRect(&m_rcDirty);
		m_pDev->Unlock();
		m_bDirty = false;
	}
	return D3D_OK;
}

// ---------------------------------------------------------------- CSurf11
CSurf11::CSurf11(CDev11* pDev, R11SurfKind kind, CTex11* pTex, UINT w, UINT h, D3DFORMAT fmt)
{
	m_ref = 1; m_pDev = pDev; m_kind = kind; m_pTex = pTex; m_w = w; m_h = h; m_fmt = fmt; m_pCpu = NULL;
	m_pitch = R11Pitch(fmt, w);
	if (m_pTex) m_pTex->AddRef();
	if (kind == R11SURF_OFFSCREEN) m_pCpu = (BYTE*)calloc(1, (size_t)m_pitch * R11Rows(fmt, h) + 16);
	pDev->AddRef();
}

CSurf11::~CSurf11()
{
	if (m_pTex) { m_pTex->m_pSurf0 = NULL; m_pTex->Release(); m_pTex = NULL; }
	if (m_pCpu) free(m_pCpu);
	if (m_pDev) m_pDev->Release();
}

HRESULT CSurf11::QueryInterface(REFIID riid, void** ppvObj)
{
	if (!ppvObj) return E_POINTER;
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDirect3DResource9) || IsEqualGUID(riid, IID_IDirect3DSurface9))
	{ *ppvObj = this; AddRef(); return S_OK; }
	*ppvObj = NULL; return E_NOINTERFACE;
}
ULONG CSurf11::AddRef() { return (ULONG)InterlockedIncrement(&m_ref); }
ULONG CSurf11::Release() { LONG r = InterlockedDecrement(&m_ref); if (r <= 0) { delete this; return 0; } return (ULONG)r; }
HRESULT CSurf11::GetDevice(IDirect3DDevice9** ppDevice) { if (!ppDevice) return E_POINTER; *ppDevice = m_pDev; m_pDev->AddRef(); return D3D_OK; }

HRESULT CSurf11::GetContainer(REFIID riid, void** ppContainer)
{
	if (!ppContainer) return E_POINTER;
	if (m_pTex) return m_pTex->QueryInterface(riid, ppContainer);
	return m_pDev->QueryInterface(riid, ppContainer);
}

HRESULT CSurf11::GetDesc(D3DSURFACE_DESC* pDesc)
{
	if (!pDesc) return E_POINTER;
	if (m_pTex) return m_pTex->GetLevelDesc(0, pDesc);
	memset(pDesc, 0, sizeof(*pDesc));
	pDesc->Format = m_fmt; pDesc->Type = D3DRTYPE_SURFACE;
	pDesc->Usage = (m_kind == R11SURF_BACKBUFFER) ? D3DUSAGE_RENDERTARGET : 0;
	pDesc->Pool = (m_kind == R11SURF_BACKBUFFER) ? D3DPOOL_DEFAULT : D3DPOOL_SYSTEMMEM;
	pDesc->MultiSampleType = D3DMULTISAMPLE_NONE; pDesc->Width = m_w; pDesc->Height = m_h;
	return D3D_OK;
}

HRESULT CSurf11::LockRect(D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)
{
	if (!pLockedRect) return E_POINTER;
	if (m_pTex) return m_pTex->LockRect(0, pLockedRect, pRect, Flags);
	if (m_kind != R11SURF_OFFSCREEN || !m_pCpu) { R11Stub("Surface::LockRect backbuffer"); return D3DERR_INVALIDCALL; }
	R11Fmt fi = R11FormatInfo(m_fmt);
	RECT rc = { 0, 0, (LONG)m_w, (LONG)m_h };
	if (pRect) rc = *pRect;
	pLockedRect->Pitch = (INT)m_pitch;
	pLockedRect->pBits = m_pCpu + rc.top * m_pitch + rc.left * (fi.bpp ? fi.bpp : 4);
	return D3D_OK;
}

HRESULT CSurf11::UnlockRect()
{
	if (m_pTex) return m_pTex->UnlockRect(0);
	return D3D_OK;
}

// ---------------------------------------------------------------- CVB11
CVB11::CVB11(CDev11* pDev, UINT len, DWORD usage, DWORD fvf, D3DPOOL pool)
{
	m_ref = 1; m_pDev = pDev; m_len = len; m_usage = usage; m_fvf = fvf; m_pool = pool;
	m_pCpu = (BYTE*)calloc(1, (size_t)len + 64);
	pDev->AddRef();
}
CVB11::~CVB11() { if (m_pCpu) free(m_pCpu); if (m_pDev) m_pDev->Release(); }

HRESULT CVB11::QueryInterface(REFIID riid, void** ppvObj)
{
	if (!ppvObj) return E_POINTER;
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDirect3DResource9) || IsEqualGUID(riid, IID_IDirect3DVertexBuffer9))
	{ *ppvObj = this; AddRef(); return S_OK; }
	*ppvObj = NULL; return E_NOINTERFACE;
}
ULONG CVB11::AddRef() { return (ULONG)InterlockedIncrement(&m_ref); }
ULONG CVB11::Release() { LONG r = InterlockedDecrement(&m_ref); if (r <= 0) { delete this; return 0; } return (ULONG)r; }
HRESULT CVB11::GetDevice(IDirect3DDevice9** ppDevice) { if (!ppDevice) return E_POINTER; *ppDevice = m_pDev; m_pDev->AddRef(); return D3D_OK; }

HRESULT CVB11::Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags)
{
	if (!ppbData || !m_pCpu) return D3DERR_INVALIDCALL;
	if (OffsetToLock >= m_len) return D3DERR_INVALIDCALL;
	*ppbData = m_pCpu + OffsetToLock;
	return D3D_OK;
}

HRESULT CVB11::GetDesc(D3DVERTEXBUFFER_DESC* pDesc)
{
	if (!pDesc) return E_POINTER;
	pDesc->Format = D3DFMT_VERTEXDATA; pDesc->Type = D3DRTYPE_VERTEXBUFFER; pDesc->Usage = m_usage; pDesc->Pool = m_pool; pDesc->Size = m_len; pDesc->FVF = m_fvf;
	return D3D_OK;
}

// ---------------------------------------------------------------- CSB11
CSB11::CSB11(CDev11* pDev) { m_ref = 1; m_pDev = pDev; pDev->AddRef(); }
CSB11::~CSB11() { if (m_pDev) m_pDev->Release(); }

HRESULT CSB11::QueryInterface(REFIID riid, void** ppvObj)
{
	if (!ppvObj) return E_POINTER;
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDirect3DStateBlock9)) { *ppvObj = this; AddRef(); return S_OK; }
	*ppvObj = NULL; return E_NOINTERFACE;
}
ULONG CSB11::AddRef() { return (ULONG)InterlockedIncrement(&m_ref); }
ULONG CSB11::Release() { LONG r = InterlockedDecrement(&m_ref); if (r <= 0) { delete this; return 0; } return (ULONG)r; }
HRESULT CSB11::GetDevice(IDirect3DDevice9** ppDevice) { if (!ppDevice) return E_POINTER; *ppDevice = m_pDev; m_pDev->AddRef(); return D3D_OK; }

void CSB11::Record(DWORD key, DWORD value)
{
	for (size_t i = 0; i < m_entries.size(); i++)
		if (m_entries[i].first == key) { m_entries[i].second = value; return; }
	m_entries.push_back(std::make_pair(key, value));
}

HRESULT CSB11::Capture()
{
	for (size_t i = 0; i < m_entries.size(); i++)
		m_entries[i].second = m_pDev->GetStateInternal(m_entries[i].first);
	return D3D_OK;
}

HRESULT CSB11::Apply()
{
	for (size_t i = 0; i < m_entries.size(); i++)
		m_pDev->SetStateInternal(m_entries[i].first, m_entries[i].second);
	return D3D_OK;
}
