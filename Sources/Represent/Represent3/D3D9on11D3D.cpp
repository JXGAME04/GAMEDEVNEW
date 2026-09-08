// [D3D11 08/09] D3D9 tren D3D11 - doi tuong IDirect3D9 (liet ke adapter/che do, caps, tao thiet bi).
#include "precompile.h"
#include "BaseInclude.h"
#include "D3D9on11.h"
#include "D3D9on11i.h"
#include <stdio.h>
#include <string.h>

CD3D11Shim::CD3D11Shim()
{
	m_ref = 1; m_pFactory = NULL; m_pAdapter = NULL;
	memset(&m_adapterDesc, 0, sizeof(m_adapterDesc)); memset(&m_desktop, 0, sizeof(m_desktop));
	m_flProbe = (D3D_FEATURE_LEVEL)0; m_uMaxTex = 8192;
}

CD3D11Shim::~CD3D11Shim()
{
	if (m_pAdapter) { m_pAdapter->Release(); m_pAdapter = NULL; }
	if (m_pFactory) { m_pFactory->Release(); m_pFactory = NULL; }
}

static void R11AddMode(std::vector<D3DDISPLAYMODE>& v, UINT w, UINT h, UINT hz)
{
	for (size_t i = 0; i < v.size(); i++) if (v[i].Width == w && v[i].Height == h) return;
	D3DDISPLAYMODE m; m.Width = w; m.Height = h; m.RefreshRate = hz; m.Format = D3DFMT_X8R8G8B8;
	v.push_back(m);
}

bool CD3D11Shim::Init()
{
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory2), (void**)&m_pFactory);
	if (FAILED(hr) || !m_pFactory) { R11Log("CreateDXGIFactory1(IDXGIFactory2) that bai 0x%08X (can Windows 8 tro len)", (unsigned)hr); return false; }
	hr = m_pFactory->EnumAdapters1(0, &m_pAdapter);
	if (FAILED(hr) || !m_pAdapter) { R11Log("EnumAdapters1(0) that bai 0x%08X", (unsigned)hr); return false; }
	m_pAdapter->GetDesc1(&m_adapterDesc);
	D3D_FEATURE_LEVEL fls[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
	hr = D3D11CreateDevice(m_pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT, fls, 4, D3D11_SDK_VERSION, NULL, &m_flProbe, NULL);
	if (FAILED(hr)) { R11Log("D3D11CreateDevice (do feature level) that bai 0x%08X", (unsigned)hr); return false; }
	m_uMaxTex = (m_flProbe >= D3D_FEATURE_LEVEL_11_0) ? 16384 : 8192;
	// che do man hinh hien tai
	DEVMODEA dm; memset(&dm, 0, sizeof(dm)); dm.dmSize = sizeof(dm);
	if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm))
	{ m_desktop.Width = dm.dmPelsWidth; m_desktop.Height = dm.dmPelsHeight; m_desktop.RefreshRate = dm.dmDisplayFrequency; }
	else
	{ m_desktop.Width = GetSystemMetrics(SM_CXSCREEN); m_desktop.Height = GetSystemMetrics(SM_CYSCREEN); m_desktop.RefreshRate = 60; }
	m_desktop.Format = D3DFMT_X8R8G8B8;
	// danh sach che do tu output 0
	IDXGIOutput* pOut = NULL;
	if (SUCCEEDED(m_pAdapter->EnumOutputs(0, &pOut)) && pOut)
	{
		UINT n = 0;
		if (SUCCEEDED(pOut->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, 0, &n, NULL)) && n > 0)
		{
			std::vector<DXGI_MODE_DESC> md(n);
			if (SUCCEEDED(pOut->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, 0, &n, &md[0])))
				for (UINT i = 0; i < n; i++)
				{
					UINT hz = md[i].RefreshRate.Denominator ? (md[i].RefreshRate.Numerator + md[i].RefreshRate.Denominator / 2) / md[i].RefreshRate.Denominator : 60;
					if (md[i].Width >= 640 && md[i].Height >= 480) R11AddMode(m_modes, md[i].Width, md[i].Height, hz);
				}
		}
		pOut->Release();
	}
	static const UINT s_std[][2] = { {640, 480}, {800, 600}, {1024, 768}, {1280, 720}, {1280, 800}, {1280, 1024}, {1366, 768}, {1440, 900}, {1600, 900}, {1680, 1050}, {1920, 1080} };
	for (int i = 0; i < (int)(sizeof(s_std) / sizeof(s_std[0])); i++) R11AddMode(m_modes, s_std[i][0], s_std[i][1], m_desktop.RefreshRate ? m_desktop.RefreshRate : 60);
	R11AddMode(m_modes, m_desktop.Width, m_desktop.Height, m_desktop.RefreshRate ? m_desktop.RefreshRate : 60);
	char szDesc[256] = "";
	WideCharToMultiByte(CP_ACP, 0, m_adapterDesc.Description, -1, szDesc, sizeof(szDesc) - 1, NULL, NULL);
	R11Log("adapter: %s | VRAM %u MB | feature level 0x%X | %u che do | man hinh %ux%u@%u", szDesc, (unsigned)(m_adapterDesc.DedicatedVideoMemory >> 20), (unsigned)m_flProbe, (unsigned)m_modes.size(), m_desktop.Width, m_desktop.Height, m_desktop.RefreshRate);
	return true;
}

HRESULT CD3D11Shim::QueryInterface(REFIID riid, void** ppvObj)
{
	if (!ppvObj) return E_POINTER;
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDirect3D9)) { *ppvObj = this; AddRef(); return S_OK; }
	*ppvObj = NULL; return E_NOINTERFACE;
}
ULONG CD3D11Shim::AddRef() { return (ULONG)InterlockedIncrement(&m_ref); }
ULONG CD3D11Shim::Release() { LONG r = InterlockedDecrement(&m_ref); if (r <= 0) { delete this; return 0; } return (ULONG)r; }

HRESULT CD3D11Shim::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier)
{
	if (!pIdentifier) return E_POINTER;
	if (Adapter != 0) return D3DERR_INVALIDCALL;
	memset(pIdentifier, 0, sizeof(*pIdentifier));
	strncpy(pIdentifier->Driver, "d3d11.dll", MAX_DEVICE_IDENTIFIER_STRING - 1);
	WideCharToMultiByte(CP_ACP, 0, m_adapterDesc.Description, -1, pIdentifier->Description, MAX_DEVICE_IDENTIFIER_STRING - 1, NULL, NULL);
	strncpy(pIdentifier->DeviceName, "\\\\.\\DISPLAY1", 31);
	pIdentifier->VendorId = m_adapterDesc.VendorId; pIdentifier->DeviceId = m_adapterDesc.DeviceId;
	pIdentifier->SubSysId = m_adapterDesc.SubSysId; pIdentifier->Revision = m_adapterDesc.Revision;
	return D3D_OK;
}

UINT CD3D11Shim::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format)
{
	if (Adapter != 0) return 0;
	if (Format == D3DFMT_X8R8G8B8 || Format == D3DFMT_A8R8G8B8) return (UINT)m_modes.size();
	return 0;
}

HRESULT CD3D11Shim::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode)
{
	if (!pMode) return E_POINTER;
	if (Adapter != 0 || (Format != D3DFMT_X8R8G8B8 && Format != D3DFMT_A8R8G8B8) || Mode >= m_modes.size()) return D3DERR_INVALIDCALL;
	*pMode = m_modes[Mode];
	return D3D_OK;
}

HRESULT CD3D11Shim::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode)
{
	if (!pMode) return E_POINTER;
	if (Adapter != 0) return D3DERR_INVALIDCALL;
	*pMode = m_desktop;
	return D3D_OK;
}

HRESULT CD3D11Shim::CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed)
{
	if (Adapter != 0 || DevType != D3DDEVTYPE_HAL) return D3DERR_NOTAVAILABLE;
	bool bA = (AdapterFormat == D3DFMT_X8R8G8B8 || AdapterFormat == D3DFMT_A8R8G8B8);
	bool bB = (BackBufferFormat == D3DFMT_UNKNOWN || BackBufferFormat == D3DFMT_X8R8G8B8 || BackBufferFormat == D3DFMT_A8R8G8B8);
	return (bA && bB) ? D3D_OK : D3DERR_NOTAVAILABLE;
}

HRESULT CD3D11Shim::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
	if (Adapter != 0) return D3DERR_INVALIDCALL;
	if (Usage & D3DUSAGE_DEPTHSTENCIL) return D3DERR_NOTAVAILABLE;
	R11Fmt fi = R11FormatInfo(CheckFormat);
	return (fi.bpp || fi.blockBytes) ? D3D_OK : D3DERR_NOTAVAILABLE;
}

HRESULT CD3D11Shim::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels)
{
	if (pQualityLevels) *pQualityLevels = 1;
	return (MultiSampleType == D3DMULTISAMPLE_NONE) ? D3D_OK : D3DERR_NOTAVAILABLE;
}

HRESULT CD3D11Shim::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps)
{
	if (!pCaps) return E_POINTER;
	if (Adapter != 0) return D3DERR_INVALIDCALL;
	FillCapsStatic(pCaps, m_uMaxTex);
	pCaps->DeviceType = DeviceType;
	return D3D_OK;
}

HMONITOR CD3D11Shim::GetAdapterMonitor(UINT Adapter)
{
	POINT pt = { 0, 0 };
	return MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
}

void CD3D11Shim::FillCapsStatic(D3DCAPS9* pCaps, UINT uMaxTex)
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

HRESULT CD3D11Shim::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
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
	CDev11* pDev = new CDev11(this, hWnd, pp, BehaviorFlags);
	if (!pDev->Init()) { pDev->Release(); return D3DERR_NOTAVAILABLE; }
	*ppReturnedDeviceInterface = pDev;
	return D3D_OK;
}

IDirect3D9* Rep3_CreateD3D9on11()
{
	CD3D11Shim* p = new CD3D11Shim();
	if (!p->Init()) { p->Release(); return NULL; }
	return p;
}
