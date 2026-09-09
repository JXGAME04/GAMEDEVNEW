// [GPU 08/09] D3D9 tren SDL_GPU - doi tuong IDirect3D9 (che do man hinh, caps, tao thiet bi), dinh dang, log, API bang mau.
#include "precompile.h"
#include "BaseInclude.h"
#ifdef JX_PLATFORM_SDL
#include "D3D9onGPU.h"
#include "D3D9onGPUi.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <set>
#include <string>

CDevGpu* g_pRep3DevGpu = NULL;

void RgLog(const char* fmt, ...)
{
	char sz[1024];
	va_list ap;
	va_start(ap, fmt);
	_vsnprintf(sz, sizeof(sz) - 1, fmt, ap);
	va_end(ap);
	sz[sizeof(sz) - 1] = 0;
	Rep3Log("[GPU] %s", sz);
}

void RgStub(const char* szName)
{
	static std::set<std::string> s_logged;
	if (s_logged.insert(szName).second)
		RgLog("CHUA CAI: %s (goi lan dau)", szName);
}

// ---------------------------------------------------------------- dinh dang
RgFmt RgFormatInfo(D3DFORMAT f)
{
	RgFmt r = { SDL_GPU_TEXTUREFORMAT_INVALID, 0, false };
	switch (f)
	{
	case D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8: r.gpu = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM; r.bpp = 4; break;
	case D3DFMT_A8B8G8R8: case D3DFMT_X8B8G8R8: r.gpu = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM; r.bpp = 4; break;
	// 16 bit: CPU giu nguyen, tai len BGRA8 (thu tu kenh 4444/1555 khac nhau giua Vulkan va D3D -> doi cho chac)
	case D3DFMT_A4R4G4B4: case D3DFMT_R5G6B5: case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5: r.gpu = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM; r.bpp = 2; r.bConvert = true; break;
	case D3DFMT_R8G8B8: r.gpu = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM; r.bpp = 3; r.bConvert = true; break;
	case D3DFMT_A8:     r.gpu = SDL_GPU_TEXTUREFORMAT_A8_UNORM; r.bpp = 1; break;
	case D3DFMT_A8L8:   r.gpu = SDL_GPU_TEXTUREFORMAT_R8G8_UNORM; r.bpp = 2; break;	// chi so (byte thap) + alpha (byte cao) cho bang mau
	case D3DFMT_L8:     r.gpu = SDL_GPU_TEXTUREFORMAT_R8_UNORM; r.bpp = 1; break;
	case D3DFMT_P8:     r.gpu = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM; r.bpp = 1; r.bConvert = true; break;
	default: break;
	}
	return r;
}

UINT RgPitch(D3DFORMAT f, UINT w)
{
	RgFmt r = RgFormatInfo(f);
	return w * (r.bpp ? r.bpp : 4);
}

UINT RgGpuBpp(SDL_GPUTextureFormat f)
{
	switch (f)
	{
	case SDL_GPU_TEXTUREFORMAT_A8_UNORM: case SDL_GPU_TEXTUREFORMAT_R8_UNORM: return 1;
	case SDL_GPU_TEXTUREFORMAT_R8G8_UNORM: case SDL_GPU_TEXTUREFORMAT_B5G6R5_UNORM: case SDL_GPU_TEXTUREFORMAT_B5G5R5A1_UNORM: case SDL_GPU_TEXTUREFORMAT_B4G4R4A4_UNORM: return 2;
	default: return 4;
	}
}

// doi mot hang sang B8G8R8A8 (DWORD 0xAARRGGBB) - nhu R11ConvertRowToBgra
void RgConvertRowToBgra(D3DFORMAT f, const BYTE* pSrc, DWORD* pDst, UINT w)
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
	case D3DFMT_A8L8:
		for (x = 0; x < w; x++) { BYTE l = pSrc[x * 2], a = pSrc[x * 2 + 1]; pDst[x] = ((DWORD)a << 24) | (l << 16) | (l << 8) | l; }
		break;
	case D3DFMT_A8:
		for (x = 0; x < w; x++) pDst[x] = ((DWORD)pSrc[x] << 24) | 0x00FFFFFF;
		break;
	default:	// 32 bit: chep
		memcpy(pDst, pSrc, w * 4);
		break;
	}
}

// doi mot hang B8G8R8A8 ve dinh dang D3D9 (doc lai render target)
void RgConvertRowFromBgra(D3DFORMAT f, const DWORD* pSrc, BYTE* pDst, UINT w)
{
	UINT x;
	switch (f)
	{
	case D3DFMT_A4R4G4B4:
		for (x = 0; x < w; x++) { DWORD c = pSrc[x]; ((WORD*)pDst)[x] = (WORD)((((c >> 24) & 0xFF) >> 4) << 12 | (((c >> 16) & 0xFF) >> 4) << 8 | (((c >> 8) & 0xFF) >> 4) << 4 | ((c & 0xFF) >> 4)); }
		break;
	case D3DFMT_R5G6B5:
		for (x = 0; x < w; x++) { DWORD c = pSrc[x]; ((WORD*)pDst)[x] = (WORD)((((c >> 16) & 0xFF) >> 3) << 11 | (((c >> 8) & 0xFF) >> 2) << 5 | ((c & 0xFF) >> 3)); }
		break;
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		for (x = 0; x < w; x++) { DWORD c = pSrc[x]; ((WORD*)pDst)[x] = (WORD)((((c >> 24) & 0x80) ? 0x8000 : 0) | (((c >> 16) & 0xFF) >> 3) << 10 | (((c >> 8) & 0xFF) >> 3) << 5 | ((c & 0xFF) >> 3)); }
		break;
	case D3DFMT_R8G8B8:
		for (x = 0; x < w; x++) { DWORD c = pSrc[x]; pDst[x * 3] = (BYTE)c; pDst[x * 3 + 1] = (BYTE)(c >> 8); pDst[x * 3 + 2] = (BYTE)(c >> 16); }
		break;
	case D3DFMT_A8L8:
		for (x = 0; x < w; x++) { DWORD c = pSrc[x]; pDst[x * 2] = (BYTE)(c >> 16); pDst[x * 2 + 1] = (BYTE)(c >> 24); }
		break;
	case D3DFMT_L8: case D3DFMT_P8:
		for (x = 0; x < w; x++) pDst[x] = (BYTE)(pSrc[x] >> 16);
		break;
	case D3DFMT_A8:
		for (x = 0; x < w; x++) pDst[x] = (BYTE)(pSrc[x] >> 24);
		break;
	default:
		memcpy(pDst, pSrc, w * 4);
		break;
	}
}

// ---------------------------------------------------------------- CGpuShim
static void RgAddMode(std::vector<D3DDISPLAYMODE>& v, UINT w, UINT h, UINT hz)
{
	for (size_t i = 0; i < v.size(); i++) if (v[i].Width == w && v[i].Height == h) return;
	D3DDISPLAYMODE m; m.Width = w; m.Height = h; m.RefreshRate = hz; m.Format = D3DFMT_X8R8G8B8;
	v.push_back(m);
}

CGpuShim::CGpuShim()
{
	m_ref = 1; memset(&m_desktop, 0, sizeof(m_desktop)); m_uMaxTex = 8192;
	strcpy(m_szDriver, "SDL_GPU");
}

CGpuShim::~CGpuShim() {}

bool CGpuShim::Init()
{
	if (!SDL_WasInit(SDL_INIT_VIDEO)) { RgLog("SDL video chua khoi tao (can chay trong GameSDL.exe)"); return false; }
	SDL_DisplayID did = SDL_GetPrimaryDisplay();
	const SDL_DisplayMode* dm = did ? SDL_GetDesktopDisplayMode(did) : NULL;
	m_desktop.Width = dm ? (UINT)dm->w : 1280; m_desktop.Height = dm ? (UINT)dm->h : 720;
	m_desktop.RefreshRate = (dm && dm->refresh_rate > 0.0f) ? (UINT)(dm->refresh_rate + 0.5f) : 60; m_desktop.Format = D3DFMT_X8R8G8B8;
	int n = 0;
	SDL_DisplayMode** modes = did ? SDL_GetFullscreenDisplayModes(did, &n) : NULL;
	if (modes)
	{
		for (int i = 0; i < n; i++)
			if (modes[i]->w >= 640 && modes[i]->h >= 480)
				RgAddMode(m_modes, (UINT)modes[i]->w, (UINT)modes[i]->h, modes[i]->refresh_rate > 0.0f ? (UINT)(modes[i]->refresh_rate + 0.5f) : m_desktop.RefreshRate);
		SDL_free(modes);
	}
	static const UINT s_std[][2] = { {640, 480}, {800, 600}, {1024, 768}, {1280, 720}, {1280, 800}, {1280, 1024}, {1366, 768}, {1440, 900}, {1600, 900}, {1680, 1050}, {1920, 1080}, {2560, 1440} };
	for (int i = 0; i < (int)(sizeof(s_std) / sizeof(s_std[0])); i++) RgAddMode(m_modes, s_std[i][0], s_std[i][1], m_desktop.RefreshRate);
	RgAddMode(m_modes, m_desktop.Width, m_desktop.Height, m_desktop.RefreshRate);
	RgLog("man hinh %ux%u@%u | %u che do | SDL %d.%d.%d", m_desktop.Width, m_desktop.Height, m_desktop.RefreshRate, (unsigned)m_modes.size(),
		SDL_VERSIONNUM_MAJOR(SDL_GetVersion()), SDL_VERSIONNUM_MINOR(SDL_GetVersion()), SDL_VERSIONNUM_MICRO(SDL_GetVersion()));
	return true;
}

HRESULT CGpuShim::QueryInterface(REFIID riid, void** ppvObj)
{
	if (!ppvObj) return E_POINTER;
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDirect3D9)) { *ppvObj = this; AddRef(); return S_OK; }
	*ppvObj = NULL; return E_NOINTERFACE;
}
ULONG CGpuShim::AddRef() { return (ULONG)InterlockedIncrement(&m_ref); }
ULONG CGpuShim::Release() { LONG r = InterlockedDecrement(&m_ref); if (r <= 0) { delete this; return 0; } return (ULONG)r; }

HRESULT CGpuShim::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier)
{
	if (!pIdentifier) return E_POINTER;
	if (Adapter != 0) return D3DERR_INVALIDCALL;
	memset(pIdentifier, 0, sizeof(*pIdentifier));
	strncpy(pIdentifier->Driver, "SDL3.dll", MAX_DEVICE_IDENTIFIER_STRING - 1);
	strncpy(pIdentifier->Description, "SDL_GPU (Vulkan)", MAX_DEVICE_IDENTIFIER_STRING - 1);
	strncpy(pIdentifier->DeviceName, "\\\\.\\DISPLAY1", 31);
	return D3D_OK;
}

UINT CGpuShim::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format)
{
	if (Adapter != 0) return 0;
	if (Format == D3DFMT_X8R8G8B8 || Format == D3DFMT_A8R8G8B8) return (UINT)m_modes.size();
	return 0;
}

HRESULT CGpuShim::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
{
	if (!pMode) return E_POINTER;
	if (Adapter != 0 || (Format != D3DFMT_X8R8G8B8 && Format != D3DFMT_A8R8G8B8) || Mode >= m_modes.size()) return D3DERR_INVALIDCALL;
	*pMode = m_modes[Mode];
	return D3D_OK;
}

HRESULT CGpuShim::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode)
{
	if (!pMode) return E_POINTER;
	if (Adapter != 0) return D3DERR_INVALIDCALL;
	*pMode = m_desktop;
	return D3D_OK;
}

HRESULT CGpuShim::CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed)
{
	if (Adapter != 0 || DevType != D3DDEVTYPE_HAL) return D3DERR_NOTAVAILABLE;
	bool bA = (AdapterFormat == D3DFMT_X8R8G8B8 || AdapterFormat == D3DFMT_A8R8G8B8);
	bool bB = (BackBufferFormat == D3DFMT_UNKNOWN || BackBufferFormat == D3DFMT_X8R8G8B8 || BackBufferFormat == D3DFMT_A8R8G8B8);
	return (bA && bB) ? D3D_OK : D3DERR_NOTAVAILABLE;
}

HRESULT CGpuShim::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
	if (Adapter != 0) return D3DERR_INVALIDCALL;
	if (Usage & D3DUSAGE_DEPTHSTENCIL) return D3DERR_NOTAVAILABLE;
	RgFmt fi = RgFormatInfo(CheckFormat);
	return fi.bpp ? D3D_OK : D3DERR_NOTAVAILABLE;
}

HRESULT CGpuShim::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)
{
	if (pQualityLevels) *pQualityLevels = 1;
	return (MultiSampleType == D3DMULTISAMPLE_NONE) ? D3D_OK : D3DERR_NOTAVAILABLE;
}

HRESULT CGpuShim::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps)
{
	if (!pCaps) return E_POINTER;
	if (Adapter != 0) return D3DERR_INVALIDCALL;
	FillCapsStatic(pCaps, m_uMaxTex);
	pCaps->DeviceType = DeviceType;
	return D3D_OK;
}

HMONITOR CGpuShim::GetAdapterMonitor(UINT Adapter)
{
	POINT pt = { 0, 0 };
	return MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
}

// giong CD3D11Shim::FillCapsStatic (Represent3 chi hoi vai co)
void CGpuShim::FillCapsStatic(D3DCAPS9* pCaps, UINT uMaxTex)
{
	memset(pCaps, 0, sizeof(*pCaps));
	pCaps->DeviceType = D3DDEVTYPE_HAL;
	pCaps->AdapterOrdinal = 0;
	pCaps->Caps2 = D3DCAPS2_DYNAMICTEXTURES | D3DCAPS2_FULLSCREENGAMMA | D3DCAPS2_CANAUTOGENMIPMAP;
	pCaps->PresentationIntervals = D3DPRESENT_INTERVAL_ONE | D3DPRESENT_INTERVAL_IMMEDIATE;
	pCaps->DevCaps = D3DDEVCAPS_HWTRANSFORMANDLIGHT | D3DDEVCAPS_HWRASTERIZATION | D3DDEVCAPS_PUREDEVICE | D3DDEVCAPS_DRAWPRIMTLVERTEX
		| D3DDEVCAPS_TEXTUREVIDEOMEMORY | D3DDEVCAPS_CANRENDERAFTERFLIP | D3DDEVCAPS_DRAWPRIMITIVES2 | D3DDEVCAPS_DRAWPRIMITIVES2EX | D3DDEVCAPS_TLVERTEXVIDEOMEMORY;
	pCaps->PrimitiveMiscCaps = D3DPMISCCAPS_CULLNONE | D3DPMISCCAPS_CULLCW | D3DPMISCCAPS_CULLCCW | D3DPMISCCAPS_BLENDOP | D3DPMISCCAPS_MASKZ | D3DPMISCCAPS_COLORWRITEENABLE;
	pCaps->RasterCaps = D3DPRASTERCAPS_SCISSORTEST | D3DPRASTERCAPS_ZTEST;
	pCaps->ZCmpCaps = 0xFF; pCaps->SrcBlendCaps = 0x1FFF; pCaps->DestBlendCaps = 0x1FFF; pCaps->AlphaCmpCaps = 0xFF;
	pCaps->ShadeCaps = D3DPSHADECAPS_COLORGOURAUDRGB | D3DPSHADECAPS_ALPHAGOURAUDBLEND;
	pCaps->TextureCaps = D3DPTEXTURECAPS_ALPHA | D3DPTEXTURECAPS_MIPMAP | D3DPTEXTURECAPS_PERSPECTIVE;	// khong POW2 -> NPOT day du
	pCaps->TextureFilterCaps = D3DPTFILTERCAPS_MAGFPOINT | D3DPTFILTERCAPS_MAGFLINEAR | D3DPTFILTERCAPS_MINFPOINT | D3DPTFILTERCAPS_MINFLINEAR | D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFLINEAR;
	pCaps->TextureAddressCaps = D3DPTADDRESSCAPS_WRAP | D3DPTADDRESSCAPS_CLAMP | D3DPTADDRESSCAPS_MIRROR | D3DPTADDRESSCAPS_BORDER | D3DPTADDRESSCAPS_INDEPENDENTUV;
	pCaps->LineCaps = D3DLINECAPS_TEXTURE | D3DLINECAPS_ZTEST | D3DLINECAPS_BLEND | D3DLINECAPS_ALPHACMP;
	pCaps->MaxTextureWidth = uMaxTex; pCaps->MaxTextureHeight = uMaxTex; pCaps->MaxTextureRepeat = 8192; pCaps->MaxTextureAspectRatio = 0; pCaps->MaxAnisotropy = 1;
	pCaps->MaxVertexW = 1.0e10f; pCaps->GuardBandLeft = -32768.0f; pCaps->GuardBandTop = -32768.0f; pCaps->GuardBandRight = 32768.0f; pCaps->GuardBandBottom = 32768.0f;
	pCaps->MaxTextureBlendStages = 8; pCaps->MaxSimultaneousTextures = 8;
	pCaps->VertexProcessingCaps = D3DVTXPCAPS_TEXGEN | D3DVTXPCAPS_MATERIALSOURCE7 | D3DVTXPCAPS_DIRECTIONALLIGHTS | D3DVTXPCAPS_POSITIONALLIGHTS | D3DVTXPCAPS_LOCALVIEWER;
	pCaps->MaxActiveLights = 8; pCaps->MaxUserClipPlanes = 6; pCaps->MaxVertexBlendMatrices = 4; pCaps->MaxVertexBlendMatrixIndex = 0;
	pCaps->MaxPointSize = 256.0f; pCaps->MaxPrimitiveCount = 0xFFFFF; pCaps->MaxVertexIndex = 0xFFFFF; pCaps->MaxStreams = 16; pCaps->MaxStreamStride = 255;
	pCaps->VertexShaderVersion = D3DVS_VERSION(3, 0); pCaps->MaxVertexShaderConst = 256; pCaps->PixelShaderVersion = D3DPS_VERSION(3, 0); pCaps->PixelShader1xMaxValue = 8.0f;
	pCaps->TextureOpCaps = D3DTEXOPCAPS_DISABLE | D3DTEXOPCAPS_SELECTARG1 | D3DTEXOPCAPS_SELECTARG2 | D3DTEXOPCAPS_MODULATE | D3DTEXOPCAPS_MODULATE2X | D3DTEXOPCAPS_MODULATE4X
		| D3DTEXOPCAPS_ADD | D3DTEXOPCAPS_ADDSIGNED | D3DTEXOPCAPS_ADDSIGNED2X | D3DTEXOPCAPS_SUBTRACT | D3DTEXOPCAPS_ADDSMOOTH | D3DTEXOPCAPS_BLENDDIFFUSEALPHA | D3DTEXOPCAPS_BLENDTEXTUREALPHA;
	pCaps->NumSimultaneousRTs = 1;
	pCaps->DevCaps2 = 0;
}

HRESULT CGpuShim::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
	if (!ppReturnedDeviceInterface || !pPresentationParameters) return D3DERR_INVALIDCALL;
	*ppReturnedDeviceInterface = NULL;
	if (Adapter != 0 || DeviceType != D3DDEVTYPE_HAL) return D3DERR_NOTAVAILABLE;
	HWND hWnd = pPresentationParameters->hDeviceWindow ? pPresentationParameters->hDeviceWindow : hFocusWindow;
	if (!hWnd) return D3DERR_INVALIDCALL;
	D3DPRESENT_PARAMETERS pp = *pPresentationParameters;
	if (pp.BackBufferWidth == 0 || pp.BackBufferHeight == 0)
	{
		RECT rc; GetClientRect(hWnd, &rc);
		pp.BackBufferWidth = rc.right - rc.left; pp.BackBufferHeight = rc.bottom - rc.top;
	}
	CDevGpu* pDev = new CDevGpu(this, hWnd, pp, BehaviorFlags);
	if (!pDev->Init()) { pDev->Release(); return D3DERR_NOTAVAILABLE; }
	*ppReturnedDeviceInterface = pDev;
	return D3D_OK;
}

IDirect3D9* Rep3_CreateD3D9onGPU()
{
	CGpuShim* p = new CGpuShim();
	if (!p->Init()) { p->Release(); return NULL; }
	return p;
}

void Rep3_GpuVramInfo(unsigned* puUsedMB, unsigned* puBudgetMB)
{
	if (puUsedMB) *puUsedMB = g_pRep3DevGpu ? (unsigned)(g_pRep3DevGpu->m_uTexBytes >> 20) : 0;
	if (puBudgetMB) *puBudgetMB = g_pRep3DevGpu ? 1024 : 0;
}

int  Rep3_GpuPaletteOK() { return (g_pRep3DevGpu && g_pRep3DevGpu->m_pGpu) ? 1 : 0; }
int  Rep3_GpuAllocPalette(const unsigned char* pPal24, int nColors) { return g_pRep3DevGpu ? g_pRep3DevGpu->PalAlloc(pPal24, nColors) : -1; }
void Rep3_GpuFreePalette(int nRow) { if (g_pRep3DevGpu) g_pRep3DevGpu->PalFree(nRow); }
void Rep3_GpuTagPalette(IDirect3DTexture9* pTex, int nRow) { if (pTex) ((CTexGpu*)pTex)->m_nPalRow = nRow; }

#endif // JX_PLATFORM_SDL
