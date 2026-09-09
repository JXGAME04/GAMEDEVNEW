// [D3D11 08/09] D3D9 tren D3D11 - thiet bi (IDirect3DDevice9): swapchain, trang thai, ve.
#include "precompile.h"
#include "BaseInclude.h"
#include "D3D9on11.h"
#include "D3D9on11i.h"
#include "Rep3Shaders11_vs.h"
#include "Rep3Shaders11_ps.h"
#include "Rep3LocTG11_vs.h"	// [LOCTG 09/09]
#include "Rep3LocTG11_ps.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>	// [r2] getenv
#include <string.h>

#define R11_SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }
#define R11_RING_SIZE (4 * 1024 * 1024)

CDev11* g_pRep3Dev11 = NULL;
double   g_dRep3PresentMs = 0.0;
unsigned g_uRep3Presents = 0;
unsigned g_uRep3PresentSkip = 0;
unsigned g_uRep3BatchQuads = 0;
unsigned g_uRep3BatchDraws = 0;
static int s_nRep3BuffersUsed = 0;
static int sd_BufferCount_log() { return s_nRep3BuffersUsed; }

double   g_dRep3DrawMs = 0.0;
unsigned g_uRep3Draws = 0;

static double R11Ms(const LARGE_INTEGER& a, const LARGE_INTEGER& b)
{
	static LARGE_INTEGER s_f = { 0 };
	if (!s_f.QuadPart) QueryPerformanceFrequency(&s_f);
	return (double)(b.QuadPart - a.QuadPart) * 1000.0 / (double)s_f.QuadPart;
}

// ---------------------------------------------------------------- ctor / dtor
CDev11::CDev11(CD3D11Shim* pParent, HWND hWnd, const D3DPRESENT_PARAMETERS& pp, DWORD dwBehavior)
{
	m_ref = 1; m_pParent = pParent; m_hWnd = hWnd; m_pp = pp; m_dwBehavior = dwBehavior;
	InitializeCriticalSection(&m_cs);
	m_pDev = NULL; m_pCtx = NULL; m_pSwap = NULL; m_pFactory = NULL; m_pAdapter3 = NULL; m_fl = D3D_FEATURE_LEVEL_10_0;
	m_bTearing = false; m_swapFlags = 0; m_hWaitable = NULL; m_bWaitedThisFrame = false; m_uStillLogged = 0; m_liLastPresent.QuadPart = 0;
	m_pBackTex = NULL; m_pBackRtv = NULL; m_pLastFrame = NULL; m_pStaging = NULL;
	m_pLocCur = NULL; m_pLocCurSrv = NULL; m_pLocHist = NULL; m_pLocHistSrv = NULL; m_pLocVS = NULL; m_pLocPS = NULL; m_pLocCb = NULL; m_dLocLast = 0.0;	// [LOCTG 09/09] m_bbW = pp.BackBufferWidth; m_bbH = pp.BackBufferHeight;
	m_pBackSurf = NULL; m_pRt = NULL; m_bRtBound = false;
	m_pVS = NULL; m_pPS = NULL; m_pVsCb = NULL; m_pPsCb = NULL; m_pRing = NULL; m_ringSize = R11_RING_SIZE; m_ringPos = 0; m_bRingDiscard = true; m_pAtlas = NULL; m_pPalTex = NULL; m_pPalSrv = NULL; m_pDummy = NULL;
	{ const char* e = getenv("REP3_PALLIN"); m_bPalLinForce = (e && atoi(e) != 0); }	// [r2] m_pDss = NULL;
	memset(m_rs, 0, sizeof(m_rs)); memset(m_tss, 0, sizeof(m_tss)); memset(m_ss, 0, sizeof(m_ss)); memset(m_tex, 0, sizeof(m_tex));
	m_fvf = 0; m_pStream = NULL; m_streamOffset = 0; m_streamStride = 0;
	memset(&m_vp, 0, sizeof(m_vp)); m_vp.Width = m_bbW; m_vp.Height = m_bbH; m_vp.MaxZ = 1.0f;
	SetRect(&m_scissor, 0, 0, m_bbW, m_bbH);
	memset(&m_matWorld, 0, sizeof(m_matWorld)); memset(&m_matView, 0, sizeof(m_matView)); memset(&m_matProj, 0, sizeof(m_matProj));
	m_matWorld._11 = m_matWorld._22 = m_matWorld._33 = m_matWorld._44 = 1.0f;
	m_matView = m_matWorld; m_matProj = m_matWorld;
	for (int i = 0; i < 256; i++) { WORD v = (WORD)(i * 257); m_gamma.red[i] = m_gamma.green[i] = m_gamma.blue[i] = v; }
	m_pRecord = NULL; m_bVsDirty = true; m_bPsDirty = true;
	memset(&m_vsCb, 0, sizeof(m_vsCb)); memset(&m_psCb, 0, sizeof(m_psCb));
	memset(m_fmtOk, 0, sizeof(m_fmtOk)); memset(m_fmtRtOk, 0, sizeof(m_fmtRtOk)); memset(m_fmtChecked, 0, sizeof(m_fmtChecked));
	// mac dinh D3D9
	m_rs[D3DRS_ZENABLE] = D3DZB_FALSE; m_rs[D3DRS_FILLMODE] = D3DFILL_SOLID; m_rs[D3DRS_SHADEMODE] = D3DSHADE_GOURAUD;
	m_rs[D3DRS_ZWRITEENABLE] = TRUE; m_rs[D3DRS_ALPHATESTENABLE] = FALSE; m_rs[D3DRS_SRCBLEND] = D3DBLEND_ONE; m_rs[D3DRS_DESTBLEND] = D3DBLEND_ZERO;
	m_rs[D3DRS_CULLMODE] = D3DCULL_CCW; m_rs[D3DRS_ZFUNC] = D3DCMP_LESSEQUAL; m_rs[D3DRS_ALPHAREF] = 0; m_rs[D3DRS_ALPHAFUNC] = D3DCMP_ALWAYS;
	m_rs[D3DRS_ALPHABLENDENABLE] = FALSE; m_rs[D3DRS_BLENDOP] = D3DBLENDOP_ADD; m_rs[D3DRS_COLORWRITEENABLE] = 0xF; m_rs[D3DRS_SCISSORTESTENABLE] = FALSE;
	m_rs[D3DRS_CLIPPING] = TRUE; m_rs[D3DRS_LIGHTING] = TRUE;
	for (int s = 0; s < 8; s++)
	{
		m_tss[s][D3DTSS_COLOROP] = (s == 0) ? D3DTOP_MODULATE : D3DTOP_DISABLE; m_tss[s][D3DTSS_COLORARG1] = D3DTA_TEXTURE; m_tss[s][D3DTSS_COLORARG2] = D3DTA_CURRENT;
		m_tss[s][D3DTSS_ALPHAOP] = (s == 0) ? D3DTOP_SELECTARG1 : D3DTOP_DISABLE; m_tss[s][D3DTSS_ALPHAARG1] = D3DTA_TEXTURE; m_tss[s][D3DTSS_ALPHAARG2] = D3DTA_CURRENT;
		m_tss[s][D3DTSS_TEXCOORDINDEX] = s;
		m_ss[s][D3DSAMP_ADDRESSU] = D3DTADDRESS_WRAP; m_ss[s][D3DSAMP_ADDRESSV] = D3DTADDRESS_WRAP; m_ss[s][D3DSAMP_ADDRESSW] = D3DTADDRESS_WRAP;
		m_ss[s][D3DSAMP_MAGFILTER] = D3DTEXF_POINT; m_ss[s][D3DSAMP_MINFILTER] = D3DTEXF_POINT; m_ss[s][D3DSAMP_MIPFILTER] = D3DTEXF_NONE;
	}
	m_bAppliedValid = false; m_batchVerts = 0; memset(&m_batchState, 0, sizeof(m_batchState)); memset(&m_lastApplied, 0, sizeof(m_lastApplied));
	ResetAppliedState();
}

CDev11::~CDev11()
{
	for (int i = 0; i < 8; i++) R11_SAFE_RELEASE(m_tex[i]);
	R11_SAFE_RELEASE(m_pStream);
	R11_SAFE_RELEASE(m_pRt);
	R11_SAFE_RELEASE(m_pRecord);
	if (m_pBackSurf) { m_pBackSurf->m_pDev = NULL; m_pBackSurf->Release(); m_pBackSurf = NULL; }
	std::map<DWORD, ID3D11InputLayout*>::iterator il; for (il = m_layouts.begin(); il != m_layouts.end(); ++il) if (il->second) il->second->Release();
	std::map<DWORD, ID3D11BlendState*>::iterator ib; for (ib = m_blends.begin(); ib != m_blends.end(); ++ib) if (ib->second) ib->second->Release();
	std::map<DWORD, ID3D11SamplerState*>::iterator is; for (is = m_samplers.begin(); is != m_samplers.end(); ++is) if (is->second) is->second->Release();
	std::map<DWORD, ID3D11RasterizerState*>::iterator ir; for (ir = m_rasters.begin(); ir != m_rasters.end(); ++ir) if (ir->second) ir->second->Release();
	PalRelease();	// [r]
	if (m_pAtlas) { delete m_pAtlas; m_pAtlas = NULL; }
	R11_SAFE_RELEASE(m_pDss); R11_SAFE_RELEASE(m_pDummy); R11_SAFE_RELEASE(m_pRing); R11_SAFE_RELEASE(m_pPsCb); R11_SAFE_RELEASE(m_pVsCb);
	R11_SAFE_RELEASE(m_pPS); R11_SAFE_RELEASE(m_pVS);
	ReleaseSwapBuffers();
	if (m_hWaitable) { CloseHandle(m_hWaitable); m_hWaitable = NULL; }
	if (m_pSwap) { m_pSwap->SetFullscreenState(FALSE, NULL); m_pSwap->Release(); m_pSwap = NULL; }
	if (g_pRep3Dev11 == this) g_pRep3Dev11 = NULL;
	R11_SAFE_RELEASE(m_pAdapter3); R11_SAFE_RELEASE(m_pFactory);
	if (m_pCtx) { m_pCtx->ClearState(); m_pCtx->Flush(); m_pCtx->Release(); m_pCtx = NULL; }
	R11_SAFE_RELEASE(m_pDev);
	DeleteCriticalSection(&m_cs);
}

bool CDev11::Init()
{
	UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	if (getenv("REP3_D3D11_DEBUG")) flags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL fls[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
	HRESULT hr = D3D11CreateDevice(m_pParent->m_pAdapter, m_pParent->m_pAdapter ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE, NULL, flags, fls, 4, D3D11_SDK_VERSION, &m_pDev, &m_fl, &m_pCtx);
	if (FAILED(hr) && (flags & D3D11_CREATE_DEVICE_DEBUG))
	{
		flags &= ~D3D11_CREATE_DEVICE_DEBUG;
		hr = D3D11CreateDevice(m_pParent->m_pAdapter, m_pParent->m_pAdapter ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE, NULL, flags, fls, 4, D3D11_SDK_VERSION, &m_pDev, &m_fl, &m_pCtx);
	}
	if (FAILED(hr) || !m_pDev) { R11Log("D3D11CreateDevice that bai 0x%08X", (unsigned)hr); return false; }
	g_nRep3D3D11FL = (int)m_fl;
	IDXGIDevice* pDxgiDev = NULL;
	if (SUCCEEDED(m_pDev->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDxgiDev)) && pDxgiDev)
	{
		IDXGIAdapter* pAd = NULL;
		if (SUCCEEDED(pDxgiDev->GetAdapter(&pAd)) && pAd)
		{
			pAd->QueryInterface(__uuidof(IDXGIAdapter3), (void**)&m_pAdapter3);
			pAd->GetParent(__uuidof(IDXGIFactory2), (void**)&m_pFactory);
			pAd->Release();
		}
		pDxgiDev->Release();
	}
	if (!m_pFactory) { R11Log("khong lay duoc IDXGIFactory2 (can Windows 8 tro len)"); return false; }
	IDXGIFactory5* pF5 = NULL;
	if (SUCCEEDED(m_pFactory->QueryInterface(__uuidof(IDXGIFactory5), (void**)&pF5)) && pF5)
	{
		BOOL bAllow = FALSE;
		if (SUCCEEDED(pF5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &bAllow, sizeof(bAllow)))) m_bTearing = (bAllow != FALSE) && (g_nRep3Tearing != 0);
		pF5->Release();
	}
	g_pRep3Dev11 = this;
	if (!CreateSwapChain(m_bbW, m_bbH, m_pp.Windowed != FALSE)) return false;
	if (!CreatePipelineObjects()) return false;
	if (g_nRep3Atlas) m_pAtlas = new CAtlasMgr(this);
	R11Log("thiet bi: feature level 0x%X, %ux%u, windowed=%d, vsync=%d, tearing=%d", (unsigned)m_fl, m_bbW, m_bbH, (int)(m_pp.Windowed != FALSE), (int)(m_pp.PresentationInterval != D3DPRESENT_INTERVAL_IMMEDIATE), (int)m_bTearing);
	R11Log("atlas: %s | %d buffer, do tre trinh chieu %d khung, khong cho %d | gop lenh %d", m_pAtlas ? "BAT (trang 1024x1024 BGRA8, texture <= 512 khong RT)" : "tat", (int)sd_BufferCount_log(), g_nRep3Latency, g_nRep3NoWait, g_nRep3Batch);
	return true;
}

bool CDev11::CreateSwapChain(UINT w, UINT h, bool bWindowed)
{
	ReleaseSwapBuffers();
	if (m_hWaitable) { CloseHandle(m_hWaitable); m_hWaitable = NULL; }
	if (m_pSwap) { m_pSwap->SetFullscreenState(FALSE, NULL); m_pSwap->Release(); m_pSwap = NULL; }
	if (w == 0) w = 1; if (h == 0) h = 1;
	m_bbW = w; m_bbH = h;
	m_swapFlags = m_bTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	if (g_nRep3Flip && g_nRep3Waitable) m_swapFlags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;	// [n]
	DXGI_SWAP_CHAIN_DESC1 sd; memset(&sd, 0, sizeof(sd));
	sd.Width = w; sd.Height = h; sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM; sd.SampleDesc.Count = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; sd.BufferCount = (g_nRep3Buffers < 2) ? 2 : ((g_nRep3Buffers > 4) ? 4 : g_nRep3Buffers); sd.Scaling = DXGI_SCALING_STRETCH;	// [m]
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE; sd.Flags = m_swapFlags;
	if (!g_nRep3Flip)	// kieu bitblt cu: Present = chep vao be mat DWM, khong xep hang khung
	{
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; sd.BufferCount = 1; sd.Flags = 0; m_swapFlags = 0; m_bTearing = false;
	}
	s_nRep3BuffersUsed = (int)sd.BufferCount;
	HRESULT hr = m_pFactory->CreateSwapChainForHwnd(m_pDev, m_hWnd, &sd, NULL, NULL, &m_pSwap);
	if (FAILED(hr))
	{
		// may cu: thu FLIP_SEQUENTIAL roi DISCARD thuong
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; sd.Flags = 0; m_swapFlags = 0; m_bTearing = false;
		hr = m_pFactory->CreateSwapChainForHwnd(m_pDev, m_hWnd, &sd, NULL, NULL, &m_pSwap);
		if (FAILED(hr))
		{
			sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; sd.BufferCount = 1;
			hr = m_pFactory->CreateSwapChainForHwnd(m_pDev, m_hWnd, &sd, NULL, NULL, &m_pSwap);
		}
		if (FAILED(hr)) { R11Log("CreateSwapChainForHwnd that bai 0x%08X", (unsigned)hr); return false; }
	}
	m_pFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
	if (m_swapFlags & DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT)
	{	// [n]
		IDXGISwapChain2* pSc2 = NULL;
		if (SUCCEEDED(m_pSwap->QueryInterface(__uuidof(IDXGISwapChain2), (void**)&pSc2)) && pSc2)
		{
			pSc2->SetMaximumFrameLatency(g_nRep3Latency < 1 ? 3 : (g_nRep3Latency > 16 ? 16 : g_nRep3Latency));
			m_hWaitable = pSc2->GetFrameLatencyWaitableObject();
			pSc2->Release();
		}
	}
	{	// [n] chan doan cua so
		struct R11Cnt { static BOOL CALLBACK Enum(HWND, LPARAM lp) { (*(int*)lp)++; return TRUE; } };
		int nChild = 0; EnumChildWindows(m_hWnd, R11Cnt::Enum, (LPARAM)&nChild);
		RECT rcC = { 0, 0, 0, 0 }; GetClientRect(m_hWnd, &rcC);
		R11Log("cua so: style 0x%08X exstyle 0x%08X con %d client %dx%d | swapchain %ux%u flags 0x%X waitable=%d", (unsigned)GetWindowLongA(m_hWnd, GWL_STYLE), (unsigned)GetWindowLongA(m_hWnd, GWL_EXSTYLE), nChild, (int)(rcC.right - rcC.left), (int)(rcC.bottom - rcC.top), w, h, (unsigned)m_swapFlags, m_hWaitable ? 1 : 0);
	}
	{	// [D3D11 08/09 g] toi da 1 khung cho trinh chieu -> Present(DO_NOT_WAIT) bo khung thua thay vi chan
		IDXGIDevice1* pDev1 = NULL;
		if (g_nRep3Latency > 0 && SUCCEEDED(m_pDev->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pDev1)) && pDev1) { pDev1->SetMaximumFrameLatency(g_nRep3Latency > 16 ? 16 : g_nRep3Latency); pDev1->Release(); }	// [o] 0 = khong dong (mac dinh DXGI, Present tu chan)
	}
	if (!bWindowed)
	{
		hr = m_pSwap->SetFullscreenState(TRUE, NULL);
		if (FAILED(hr)) R11Log("SetFullscreenState(TRUE) that bai 0x%08X -> chay cua so", (unsigned)hr);
		else { m_pSwap->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, m_swapFlags); }
	}
	return AcquireBackBuffer();
}

void CDev11::ReleaseSwapBuffers()
{
	if (m_pCtx) { ID3D11RenderTargetView* pNull = NULL; m_pCtx->OMSetRenderTargets(1, &pNull, NULL); }
	R11_SAFE_RELEASE(m_pBackRtv); R11_SAFE_RELEASE(m_pBackTex); R11_SAFE_RELEASE(m_pLastFrame); R11_SAFE_RELEASE(m_pStaging);
	LocRelease();	// [LOCTG 09/09] texture theo kich thuoc back buffer -> tao lai khi doi
	m_bRtBound = false;
}

bool CDev11::AcquireBackBuffer()
{
	HRESULT hr = m_pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_pBackTex);
	if (FAILED(hr) || !m_pBackTex) { R11Log("GetBuffer that bai 0x%08X", (unsigned)hr); return false; }
	hr = m_pDev->CreateRenderTargetView(m_pBackTex, NULL, &m_pBackRtv);
	if (FAILED(hr)) { R11Log("CreateRenderTargetView backbuffer that bai 0x%08X", (unsigned)hr); return false; }
	if (!m_pBackSurf) m_pBackSurf = new CSurf11(this, R11SURF_BACKBUFFER, NULL, m_bbW, m_bbH, D3DFMT_X8R8G8B8);
	else { m_pBackSurf->m_w = m_bbW; m_pBackSurf->m_h = m_bbH; }
	m_bRtBound = false;
	return true;
}

bool CDev11::CreatePipelineObjects()
{
	HRESULT hr = m_pDev->CreateVertexShader(g_Rep3VS11, sizeof(g_Rep3VS11), NULL, &m_pVS);
	if (FAILED(hr)) { R11Log("CreateVertexShader that bai 0x%08X", (unsigned)hr); return false; }
	hr = m_pDev->CreatePixelShader(g_Rep3PS11, sizeof(g_Rep3PS11), NULL, &m_pPS);
	if (FAILED(hr)) { R11Log("CreatePixelShader that bai 0x%08X", (unsigned)hr); return false; }
	D3D11_BUFFER_DESC bd; memset(&bd, 0, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC; bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.ByteWidth = sizeof(R11VsCb); if (FAILED(m_pDev->CreateBuffer(&bd, NULL, &m_pVsCb))) return false;
	bd.ByteWidth = sizeof(R11PsCb); if (FAILED(m_pDev->CreateBuffer(&bd, NULL, &m_pPsCb))) return false;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; bd.ByteWidth = m_ringSize; if (FAILED(m_pDev->CreateBuffer(&bd, NULL, &m_pRing))) return false;
	{
		struct { DWORD c; float u, v; float pad; } dummy = { 0xFFFFFFFF, 0.0f, 0.0f, 0.0f };
		D3D11_BUFFER_DESC dd; memset(&dd, 0, sizeof(dd)); dd.Usage = D3D11_USAGE_IMMUTABLE; dd.BindFlags = D3D11_BIND_VERTEX_BUFFER; dd.ByteWidth = sizeof(dummy);
		D3D11_SUBRESOURCE_DATA sr; memset(&sr, 0, sizeof(sr)); sr.pSysMem = &dummy;
		if (FAILED(m_pDev->CreateBuffer(&dd, &sr, &m_pDummy))) return false;
	}
	D3D11_DEPTH_STENCIL_DESC dsd; memset(&dsd, 0, sizeof(dsd)); dsd.DepthEnable = FALSE; dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; dsd.DepthFunc = D3D11_COMPARISON_ALWAYS;
	if (FAILED(m_pDev->CreateDepthStencilState(&dsd, &m_pDss))) return false;
	m_pCtx->VSSetShader(m_pVS, NULL, 0); m_pCtx->PSSetShader(m_pPS, NULL, 0);
	m_pCtx->VSSetConstantBuffers(0, 1, &m_pVsCb); m_pCtx->PSSetConstantBuffers(0, 1, &m_pPsCb);
	m_pCtx->OMSetDepthStencilState(m_pDss, 0);
	return true;
}

bool CDev11::FormatTexOK(D3DFORMAT f, bool bRenderTarget)
{
	int i = (int)f;
	if (i < 0 || i >= 256) return false;
	if (!m_fmtChecked[i])
	{
		m_fmtChecked[i] = true;
		R11Fmt fi = R11FormatInfo(f);
		UINT sup = 0;
		if (fi.dxgi != DXGI_FORMAT_UNKNOWN && SUCCEEDED(m_pDev->CheckFormatSupport(fi.dxgi, &sup)))
		{
			m_fmtOk[i]   = (sup & D3D11_FORMAT_SUPPORT_TEXTURE2D) && (sup & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE);
			m_fmtRtOk[i] = m_fmtOk[i] && (sup & D3D11_FORMAT_SUPPORT_RENDER_TARGET);
		}
		R11Log("dinh dang D3D9 %d -> dxgi %d: texture %s, render target %s", i, (int)fi.dxgi, m_fmtOk[i] ? "goc" : "doi BGRA8", m_fmtRtOk[i] ? "goc" : "doi BGRA8");
	}
	return bRenderTarget ? m_fmtRtOk[i] : m_fmtOk[i];
}

// ---------------------------------------------------------------- IUnknown
HRESULT CDev11::QueryInterface(REFIID riid, void** ppvObj)
{
	if (!ppvObj) return E_POINTER;
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDirect3DDevice9)) { *ppvObj = this; AddRef(); return S_OK; }
	*ppvObj = NULL; return E_NOINTERFACE;
}
ULONG CDev11::AddRef() { return (ULONG)InterlockedIncrement(&m_ref); }
ULONG CDev11::Release() { LONG r = InterlockedDecrement(&m_ref); if (r <= 0) { delete this; return 0; } return (ULONG)r; }

// ---------------------------------------------------------------- thiet bi
HRESULT CDev11::TestCooperativeLevel() { return D3D_OK; }

UINT CDev11::GetAvailableTextureMem()
{
	if (m_pAdapter3)
	{
		DXGI_QUERY_VIDEO_MEMORY_INFO mi;
		if (SUCCEEDED(m_pAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &mi)))
		{
			UINT64 free = (mi.Budget > mi.CurrentUsage) ? (mi.Budget - mi.CurrentUsage) : 0;
			return (free > 0xFFFFFFFFULL) ? 0xFFFFFFFFu : (UINT)free;
		}
	}
	return 1024u * 1024u * 1024u;
}

void Rep3_D3D11VramInfo(unsigned* puUsedMB, unsigned* puBudgetMB)
{
	if (puUsedMB) *puUsedMB = 0; if (puBudgetMB) *puBudgetMB = 0;
	if (!g_pRep3Dev11 || !g_pRep3Dev11->m_pAdapter3) return;
	DXGI_QUERY_VIDEO_MEMORY_INFO mi;
	if (SUCCEEDED(g_pRep3Dev11->m_pAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &mi)))
	{ if (puUsedMB) *puUsedMB = (unsigned)(mi.CurrentUsage >> 20); if (puBudgetMB) *puBudgetMB = (unsigned)(mi.Budget >> 20); }
}

HRESULT CDev11::GetDirect3D(IDirect3D9** ppD3D9) { if (!ppD3D9) return E_POINTER; *ppD3D9 = m_pParent; m_pParent->AddRef(); return D3D_OK; }
HRESULT CDev11::GetDeviceCaps(D3DCAPS9* pCaps) { if (!pCaps) return E_POINTER; FillCaps(pCaps); return D3D_OK; }
void CDev11::FillCaps(D3DCAPS9* pCaps) { m_pParent->FillCapsStatic(pCaps, (m_fl >= D3D_FEATURE_LEVEL_11_0) ? 16384 : 8192); }

HRESULT CDev11::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
	if (!pMode) return E_POINTER;
	*pMode = m_pParent->m_desktop;
	return D3D_OK;
}

HRESULT CDev11::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters)
{
	if (!pParameters) return E_POINTER;
	pParameters->AdapterOrdinal = 0; pParameters->DeviceType = D3DDEVTYPE_HAL; pParameters->hFocusWindow = m_hWnd; pParameters->BehaviorFlags = m_dwBehavior;
	return D3D_OK;
}

HRESULT CDev11::Reset(D3DPRESENT_PARAMETERS* pp)
{
	if (!pp) return D3DERR_INVALIDCALL;
	Lock();
	FlushIfPending();
	m_pp = *pp;
	UINT w = pp->BackBufferWidth ? pp->BackBufferWidth : m_bbW, h = pp->BackBufferHeight ? pp->BackBufferHeight : m_bbH;
	bool bWin = pp->Windowed != FALSE;
	BOOL bCurFull = FALSE; m_pSwap->GetFullscreenState(&bCurFull, NULL);
	HRESULT hr = D3D_OK;
	ReleaseSwapBuffers();
	if (!bWin && !bCurFull) { hr = m_pSwap->SetFullscreenState(TRUE, NULL); if (FAILED(hr)) R11Log("Reset: SetFullscreenState(TRUE) 0x%08X", (unsigned)hr); }
	if (bWin && bCurFull) m_pSwap->SetFullscreenState(FALSE, NULL);
	hr = m_pSwap->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, m_swapFlags);
	if (FAILED(hr)) { R11Log("Reset: ResizeBuffers %ux%u that bai 0x%08X", w, h, (unsigned)hr); Unlock(); return D3DERR_INVALIDCALL; }
	m_bWaitedThisFrame = false;	// [o] sau resize
	m_bbW = w; m_bbH = h;
	if (!AcquireBackBuffer()) { Unlock(); return D3DERR_INVALIDCALL; }
	m_vp.X = 0; m_vp.Y = 0; m_vp.Width = w; m_vp.Height = h; m_vp.MinZ = 0.0f; m_vp.MaxZ = 1.0f; m_bVsDirty = true;
	R11_SAFE_RELEASE(m_pRt);
	R11Log("Reset %ux%u windowed=%d", w, h, (int)bWin);
	Unlock();
	return D3D_OK;
}

void CDev11::UpdateLastFrame()
{
	if (!m_pBackTex) return;
	if (!m_pLastFrame)
	{
		D3D11_TEXTURE2D_DESC td; m_pBackTex->GetDesc(&td);
		td.BindFlags = 0; td.Usage = D3D11_USAGE_DEFAULT; td.CPUAccessFlags = 0; td.MiscFlags = 0;
		if (FAILED(m_pDev->CreateTexture2D(&td, NULL, &m_pLastFrame))) { m_pLastFrame = NULL; return; }
	}
	m_pCtx->CopyResource(m_pLastFrame, m_pBackTex);
}

// [LOCTG 09/09] Bo loc thoi gian luc trinh khung: back = lerp(back, lich_su, a), a = exp(-dt/tau).
// Xem dau tep Rep3LocTG11.hlsl. Pass rieng (shader rieng, khong vertex buffer); xong lam mat hieu luc
// cache trang thai cua lop de lan ve sau gan lai het (ApplyComputed).
extern unsigned g_uRep3LocKhung;	// [ANDROID 10/09 LOCTG] dinh nghia da doi sang KRepresentShell3.cpp (bien dich o moi nen); tep nay chi co tren Windows
void CDev11::LocRelease()
{
	R11_SAFE_RELEASE(m_pLocCurSrv); R11_SAFE_RELEASE(m_pLocCur);
	R11_SAFE_RELEASE(m_pLocHistSrv); R11_SAFE_RELEASE(m_pLocHist);
	m_dLocLast = 0.0;
}

void CDev11::LocThoiGian()
{
	if (!m_pBackTex || !m_pBackRtv || !m_pDev || !m_pCtx)
		return;
	LARGE_INTEGER q, f; QueryPerformanceCounter(&q); QueryPerformanceFrequency(&f);
	const double dNow = (double)q.QuadPart * 1000.0 / (double)f.QuadPart;
	const double dt = (m_dLocLast > 0.0) ? (dNow - m_dLocLast) : 1000.0;
	m_dLocLast = dNow;
	if (!m_pLocVS)
	{
		if (FAILED(m_pDev->CreateVertexShader(g_Rep3LocVS11, sizeof(g_Rep3LocVS11), NULL, &m_pLocVS))) { m_pLocVS = NULL; g_nRep3LocMs = 0; R11Log("[LOCTG] VS that bai -> tat"); return; }
		if (FAILED(m_pDev->CreatePixelShader(g_Rep3LocPS11, sizeof(g_Rep3LocPS11), NULL, &m_pLocPS))) { m_pLocPS = NULL; g_nRep3LocMs = 0; R11Log("[LOCTG] PS that bai -> tat"); return; }
		D3D11_BUFFER_DESC bd; memset(&bd, 0, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC; bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; bd.ByteWidth = 16;
		if (FAILED(m_pDev->CreateBuffer(&bd, NULL, &m_pLocCb))) { m_pLocCb = NULL; g_nRep3LocMs = 0; R11Log("[LOCTG] cbuffer that bai -> tat"); return; }
	}
	if (!m_pLocCur || !m_pLocHist)
	{
		D3D11_TEXTURE2D_DESC td; m_pBackTex->GetDesc(&td);
		td.BindFlags = D3D11_BIND_SHADER_RESOURCE; td.Usage = D3D11_USAGE_DEFAULT; td.CPUAccessFlags = 0; td.MiscFlags = 0; td.SampleDesc.Count = 1; td.SampleDesc.Quality = 0;
		if (!m_pLocCur && (FAILED(m_pDev->CreateTexture2D(&td, NULL, &m_pLocCur)) || FAILED(m_pDev->CreateShaderResourceView(m_pLocCur, NULL, &m_pLocCurSrv))))
		{ LocRelease(); g_nRep3LocMs = 0; R11Log("[LOCTG] texture that bai -> tat"); return; }
		if (!m_pLocHist && (FAILED(m_pDev->CreateTexture2D(&td, NULL, &m_pLocHist)) || FAILED(m_pDev->CreateShaderResourceView(m_pLocHist, NULL, &m_pLocHistSrv))))
		{ LocRelease(); g_nRep3LocMs = 0; R11Log("[LOCTG] texture lich su that bai -> tat"); return; }
		m_pCtx->CopyResource(m_pLocHist, m_pBackTex);	// lich su ban dau = chinh khung nay
		R11Log("[LOCTG] bat: tau = %d ms, %ux%u", g_nRep3LocMs, (unsigned)td.Width, (unsigned)td.Height);
	}
	// trong so lich su; dt lon (khung dau / treo / doi map) -> khong tron, chi cap nhat lich su
	float a = 0.0f;
	if (dt > 0.0 && dt < 100.0)
		a = (float)exp(-dt / (double)g_nRep3LocMs);
	if (a > 0.9f) a = 0.9f;
	if (a <= 0.001f) { m_pCtx->CopyResource(m_pLocHist, m_pBackTex); return; }
	m_pCtx->CopyResource(m_pLocCur, m_pBackTex);
	D3D11_MAPPED_SUBRESOURCE ms;
	if (SUCCEEDED(m_pCtx->Map(m_pLocCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) { float v[4] = { a, 0.0f, 0.0f, 0.0f }; memcpy(ms.pData, v, 16); m_pCtx->Unmap(m_pLocCb, 0); }
	ID3D11ShaderResourceView* srv[2] = { m_pLocCurSrv, m_pLocHistSrv };
	ID3D11RenderTargetView* rtv = m_pBackRtv;
	m_pCtx->OMSetRenderTargets(1, &rtv, NULL);
	D3D11_VIEWPORT vp; vp.TopLeftX = 0.0f; vp.TopLeftY = 0.0f; vp.Width = (float)m_bbW; vp.Height = (float)m_bbH; vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
	m_pCtx->RSSetViewports(1, &vp);
	m_pCtx->RSSetState(NULL);
	float bf[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; m_pCtx->OMSetBlendState(NULL, bf, 0xFFFFFFFF);
	m_pCtx->OMSetDepthStencilState(NULL, 0);
	m_pCtx->IASetInputLayout(NULL);
	m_pCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pCtx->VSSetShader(m_pLocVS, NULL, 0); m_pCtx->PSSetShader(m_pLocPS, NULL, 0);
	m_pCtx->PSSetConstantBuffers(0, 1, &m_pLocCb);
	m_pCtx->PSSetShaderResources(0, 2, srv);
	m_pCtx->Draw(3, 0);
	ID3D11ShaderResourceView* nul[2] = { NULL, NULL }; m_pCtx->PSSetShaderResources(0, 2, nul);
	m_pCtx->CopyResource(m_pLocHist, m_pBackTex);	// lich su = ket qua da tron
	// cache trang thai cua lop khong con dung -> lan ve sau gan lai het
	m_bAppliedValid = false; m_bPipeBound = false; m_bRtBound = false;
	m_lastSrv[0] = m_lastSrv[1] = (ID3D11ShaderResourceView*)1; m_lastIL = NULL; m_lastTopo = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	memset(&m_lastVp, 0xFF, sizeof(m_lastVp));
	g_uRep3LocKhung++;
}

HRESULT CDev11::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	Lock();
	FlushIfPending();
	LARGE_INTEGER t0, t1; QueryPerformanceCounter(&t0);
	if (g_nRep3LocMs > 0)
		LocThoiGian();	// [LOCTG 09/09] tron voi lich su TRUOC khi chup / trinh
	UpdateLastFrame();
	UINT interval = (m_pp.PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE) ? 0 : 1;
	UINT flags = 0;
	BOOL bFull = FALSE; m_pSwap->GetFullscreenState(&bFull, NULL);
	if (interval == 0 && m_bTearing && !bFull) flags |= DXGI_PRESENT_ALLOW_TEARING;
	if (interval == 0 && g_nRep3Flip && g_nRep3NoWait) flags |= DXGI_PRESENT_DO_NOT_WAIT;	// [D3D11 08/09 l] chi khi Rep3NoWait=1: hang day -> bo khung (game ve theo dot -> giat)
	HRESULT hr = m_pSwap->Present(interval, flags);
	if (hr != S_OK && m_uStillLogged < 12)
	{	// [n] chan doan: ma tra ve + ms tu Present truoc
		LARGE_INTEGER tq; QueryPerformanceCounter(&tq);
		R11Log("Present tra 0x%08X, %.2f ms sau Present truoc, flags 0x%X", (unsigned)hr, m_liLastPresent.QuadPart ? R11Ms(m_liLastPresent, tq) : 0.0, (unsigned)flags);
		m_uStillLogged++;
	}
	QueryPerformanceCounter(&m_liLastPresent);
	if (hr == DXGI_ERROR_WAS_STILL_DRAWING) { g_uRep3PresentSkip++; hr = S_OK; }
	m_bRtBound = false; m_bWaitedThisFrame = false;
	m_ringPos = 0; m_bRingDiscard = true;
	PalFrameEnd();	// [r] hang bang mau thu trong khung -> dung lai duoc
	QueryPerformanceCounter(&t1); g_dRep3PresentMs += R11Ms(t0, t1); g_uRep3Presents++;
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		static bool s_bLogged = false;
		if (!s_bLogged) { s_bLogged = true; R11Log("Present: thiet bi bi go (0x%08X, ly do 0x%08X)", (unsigned)hr, (unsigned)m_pDev->GetDeviceRemovedReason()); }
	}
	Unlock();
	return D3D_OK;
}

HRESULT CDev11::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
	if (!ppBackBuffer) return E_POINTER;
	*ppBackBuffer = m_pBackSurf; m_pBackSurf->AddRef();
	return D3D_OK;
}

HRESULT CDev11::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
	if (!pRasterStatus) return E_POINTER;
	pRasterStatus->InVBlank = FALSE; pRasterStatus->ScanLine = 0;
	return D3D_OK;
}

void CDev11::SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)
{
	if (pRamp) m_gamma = *pRamp;
	static bool s_bLogged = false;
	if (!s_bLogged) { s_bLogged = true; R11Log("SetGammaRamp: bo qua (D3D11 chi chinh gamma o toan man hinh)"); }
}
void CDev11::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) { if (pRamp) *pRamp = m_gamma; }

HRESULT CDev11::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
	if (!ppTexture) return E_POINTER;
	*ppTexture = NULL;
	if (Width == 0 || Height == 0) return D3DERR_INVALIDCALL;
	R11Fmt fi = R11FormatInfo(Format);
	if (fi.bpp == 0 && fi.blockBytes == 0) { R11Log("CreateTexture: dinh dang %d khong ho tro", (int)Format); return D3DERR_NOTAVAILABLE; }
	CTex11* pTex = new CTex11(this, Width, Height, Usage, Format, Pool);
	if (m_pAtlas && CAtlasMgr::Eligible(Width, Height, Usage, Format, Pool)) pTex->m_bVirtual = true;	// [d]
	Lock();
	HRESULT hr = D3D_OK;
	if (Pool == D3DPOOL_SYSTEMMEM || Pool == D3DPOOL_SCRATCH || Pool == D3DPOOL_MANAGED || (Usage & D3DUSAGE_DYNAMIC))
	{
		pTex->AllocCpu();
		if (!pTex->m_pCpu) hr = E_OUTOFMEMORY;
	}
	if (SUCCEEDED(hr) && Pool == D3DPOOL_DEFAULT && (Usage & D3DUSAGE_RENDERTARGET))
		hr = pTex->EnsureGpu(NULL);
	Unlock();
	if (FAILED(hr)) { pTex->Release(); return hr; }
	*ppTexture = pTex;
	return D3D_OK;
}

HRESULT CDev11::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	if (!ppVertexBuffer) return E_POINTER;
	CVB11* p = new CVB11(this, Length, Usage, FVF, Pool);
	if (!p->m_pCpu) { p->Release(); return E_OUTOFMEMORY; }
	*ppVertexBuffer = p;
	return D3D_OK;
}

HRESULT CDev11::UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint)
{
	CSurf11* pS = (CSurf11*)pSourceSurface; CSurf11* pD = (CSurf11*)pDestinationSurface;
	if (!pS || !pD || !pS->m_pTex || !pD->m_pTex) { R11Stub("UpdateSurface (khong phai texture)"); return D3DERR_INVALIDCALL; }
	return UpdateTexture(pS->m_pTex, pD->m_pTex);
}

HRESULT CDev11::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	CTex11* pS = (CTex11*)pSourceTexture; CTex11* pD = (CTex11*)pDestinationTexture;
	if (!pS || !pD || !pS->m_pCpu) return D3DERR_INVALIDCALL;
	if (pS->m_w != pD->m_w || pS->m_h != pD->m_h || pS->m_fmt != pD->m_fmt) { R11Log("UpdateTexture: kich thuoc/dinh dang lech"); return D3DERR_INVALIDCALL; }
	Lock();
	FlushIfPending();
	HRESULT hr;
	if (pD->m_pCpu)
	{
		memcpy(pD->m_pCpu, pS->m_pCpu, (size_t)pD->m_pitch * R11Rows(pD->m_fmt, pD->m_h));
		pD->m_bDirty = true; SetRect(&pD->m_rcDirty, 0, 0, pD->m_w, pD->m_h);
		hr = pD->m_pGpu ? pD->UploadRect(NULL) : D3D_OK;
		if (pD->m_pGpu) pD->m_bDirty = false;
	}
	else if (pD->m_bVirtual)
	{
		if (!pD->m_pPage) hr = pD->EnsureGpu(pS->m_pCpu);
		else { BYTE* pSave = pD->m_pCpu; pD->m_pCpu = pS->m_pCpu; hr = pD->UploadRect(NULL); pD->m_pCpu = pSave; }
	}
	else if (!pD->m_pGpu)
		hr = pD->EnsureGpu(pS->m_pCpu);				// duong chinh: tao texture GPU kem du lieu (khong ban sao RAM)
	else
	{
		BYTE* pSave = pD->m_pCpu; pD->m_pCpu = pS->m_pCpu;
		hr = pD->UploadRect(NULL);
		pD->m_pCpu = pSave;
	}
	Unlock();
	return hr;
}

HRESULT CDev11::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
	CSurf11* pRt = (CSurf11*)pRenderTarget; CSurf11* pDst = (CSurf11*)pDestSurface;
	if (!pRt || !pDst) return D3DERR_INVALIDCALL;
	ID3D11Texture2D* pSrcTex = (pRt->m_kind == R11SURF_BACKBUFFER) ? m_pBackTex : (pRt->m_pTex ? pRt->m_pTex->m_pGpu : NULL);
	BYTE* pOut = pDst->m_pTex ? pDst->m_pTex->m_pCpu : pDst->m_pCpu;
	UINT outPitch = pDst->m_pTex ? pDst->m_pTex->m_pitch : pDst->m_pitch;
	D3DFORMAT outFmt = pDst->m_pTex ? pDst->m_pTex->m_fmt : pDst->m_fmt;
	if (!pSrcTex || !pOut) return D3DERR_INVALIDCALL;
	if (outFmt != D3DFMT_A8R8G8B8 && outFmt != D3DFMT_X8R8G8B8) { R11Stub("GetRenderTargetData dinh dang dich khong 32 bit"); return D3DERR_INVALIDCALL; }
	Lock();
	FlushIfPending();
	D3D11_TEXTURE2D_DESC td; pSrcTex->GetDesc(&td);
	td.BindFlags = 0; td.Usage = D3D11_USAGE_STAGING; td.CPUAccessFlags = D3D11_CPU_ACCESS_READ; td.MiscFlags = 0;
	ID3D11Texture2D* pSt = NULL;
	HRESULT hr = m_pDev->CreateTexture2D(&td, NULL, &pSt);
	if (SUCCEEDED(hr))
	{
		m_pCtx->CopyResource(pSt, pSrcTex);
		D3D11_MAPPED_SUBRESOURCE ms;
		if (SUCCEEDED(m_pCtx->Map(pSt, 0, D3D11_MAP_READ, 0, &ms)))
		{
			UINT w = min(td.Width, pDst->m_w), h = min(td.Height, pDst->m_h);
			for (UINT y = 0; y < h; y++) memcpy(pOut + y * outPitch, (BYTE*)ms.pData + y * ms.RowPitch, w * 4);
			m_pCtx->Unmap(pSt, 0);
		}
		pSt->Release();
	}
	Unlock();
	return SUCCEEDED(hr) ? D3D_OK : D3DERR_INVALIDCALL;
}

HRESULT CDev11::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
	CSurf11* pDst = (CSurf11*)pDestSurface;
	if (!pDst || pDst->m_kind != R11SURF_OFFSCREEN || !pDst->m_pCpu || !m_pLastFrame) return D3DERR_INVALIDCALL;
	Lock();
	FlushIfPending();
	if (!m_pStaging)
	{
		D3D11_TEXTURE2D_DESC td; m_pLastFrame->GetDesc(&td);
		td.BindFlags = 0; td.Usage = D3D11_USAGE_STAGING; td.CPUAccessFlags = D3D11_CPU_ACCESS_READ; td.MiscFlags = 0;
		if (FAILED(m_pDev->CreateTexture2D(&td, NULL, &m_pStaging))) { m_pStaging = NULL; Unlock(); return D3DERR_INVALIDCALL; }
	}
	m_pCtx->CopyResource(m_pStaging, m_pLastFrame);
	D3D11_MAPPED_SUBRESOURCE ms;
	if (FAILED(m_pCtx->Map(m_pStaging, 0, D3D11_MAP_READ, 0, &ms))) { Unlock(); return D3DERR_INVALIDCALL; }
	memset(pDst->m_pCpu, 0, (size_t)pDst->m_pitch * pDst->m_h);
	// vi tri vung client tren man hinh (nhu D3D9: du lieu la ca man hinh)
	POINT pt = { 0, 0 };
	BOOL bFull = FALSE; m_pSwap->GetFullscreenState(&bFull, NULL);
	if (!bFull) ClientToScreen(m_hWnd, &pt);
	for (UINT y = 0; y < m_bbH; y++)
	{
		LONG dy = pt.y + (LONG)y; if (dy < 0 || dy >= (LONG)pDst->m_h) continue;
		LONG x0 = pt.x, x1 = pt.x + (LONG)m_bbW; LONG sx = 0;
		if (x0 < 0) { sx = -x0; x0 = 0; } if (x1 > (LONG)pDst->m_w) x1 = pDst->m_w;
		if (x1 <= x0) continue;
		memcpy(pDst->m_pCpu + dy * pDst->m_pitch + x0 * 4, (BYTE*)ms.pData + y * ms.RowPitch + sx * 4, (x1 - x0) * 4);
	}
	m_pCtx->Unmap(m_pStaging, 0);
	Unlock();
	return D3D_OK;
}

HRESULT CDev11::ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
{
	CSurf11* pS = (CSurf11*)pSurface;
	if (!pS) return D3DERR_INVALIDCALL;
	ID3D11RenderTargetView* pRtv = (pS->m_kind == R11SURF_BACKBUFFER) ? m_pBackRtv : (pS->m_pTex ? pS->m_pTex->m_pRtv : NULL);
	if (!pRtv) { R11Stub("ColorFill khong phai render target"); return D3DERR_INVALIDCALL; }
	float c[4] = { ((color >> 16) & 255) / 255.0f, ((color >> 8) & 255) / 255.0f, (color & 255) / 255.0f, ((color >> 24) & 255) / 255.0f };
	Lock(); m_pCtx->ClearRenderTargetView(pRtv, c); Unlock();
	return D3D_OK;
}

HRESULT CDev11::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	if (!ppSurface) return E_POINTER;
	CSurf11* p = new CSurf11(this, R11SURF_OFFSCREEN, NULL, Width, Height, Format);
	if (!p->m_pCpu) { p->Release(); return E_OUTOFMEMORY; }
	*ppSurface = p;
	return D3D_OK;
}

HRESULT CDev11::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	if (RenderTargetIndex != 0) return D3DERR_INVALIDCALL;
	CSurf11* pS = (CSurf11*)pRenderTarget;
	if (!pS) return D3DERR_INVALIDCALL;
	Lock();
	FlushIfPending();
	R11_SAFE_RELEASE(m_pRt);
	UINT w = m_bbW, h = m_bbH;
	if (pS->m_kind == R11SURF_TEX && pS->m_pTex)
	{
		if (!pS->m_pTex->m_pRtv) { pS->m_pTex->m_usage |= D3DUSAGE_RENDERTARGET; pS->m_pTex->ReleaseGpu(); pS->m_pTex->EnsureGpu(pS->m_pTex->m_pCpu); }
		if (!pS->m_pTex->m_pRtv) { Unlock(); return D3DERR_INVALIDCALL; }
		m_pRt = pS; m_pRt->AddRef();
		w = pS->m_pTex->m_w; h = pS->m_pTex->m_h;
	}
	m_vp.X = 0; m_vp.Y = 0; m_vp.Width = w; m_vp.Height = h; m_vp.MinZ = 0.0f; m_vp.MaxZ = 1.0f;
	m_bVsDirty = true; m_bRtBound = false;
	Unlock();
	return D3D_OK;
}

HRESULT CDev11::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
	if (!ppRenderTarget) return E_POINTER;
	if (RenderTargetIndex != 0) return D3DERR_INVALIDCALL;
	CSurf11* p = m_pRt ? m_pRt : m_pBackSurf;
	*ppRenderTarget = p; p->AddRef();
	return D3D_OK;
}

void CDev11::BindRenderTarget()
{
	if (m_bRtBound) return;
	if (m_hWaitable && !m_bWaitedThisFrame) { WaitForSingleObjectEx(m_hWaitable, 1000, TRUE); m_bWaitedThisFrame = true; }	// [o] MOT lan moi khung (moi lan cho tieu mot suat)
	ID3D11RenderTargetView* pRtv = (m_pRt && m_pRt->m_pTex) ? m_pRt->m_pTex->m_pRtv : m_pBackRtv;
	ID3D11ShaderResourceView* pNull[2] = { NULL, NULL };
	m_pCtx->PSSetShaderResources(0, 2, pNull);		// tranh texture vua la RT vua la nguon
	m_pCtx->OMSetRenderTargets(1, &pRtv, NULL);
	m_bRtBound = true;
}

HRESULT CDev11::BeginScene() { return D3D_OK; }
HRESULT CDev11::EndScene() { return D3D_OK; }

HRESULT CDev11::Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	if (!(Flags & D3DCLEAR_TARGET)) return D3D_OK;
	Lock();
	FlushIfPending();
	BindRenderTarget();
	ID3D11RenderTargetView* pRtv = (m_pRt && m_pRt->m_pTex) ? m_pRt->m_pTex->m_pRtv : m_pBackRtv;
	float c[4] = { ((Color >> 16) & 255) / 255.0f, ((Color >> 8) & 255) / 255.0f, (Color & 255) / 255.0f, ((Color >> 24) & 255) / 255.0f };
	if (pRtv) m_pCtx->ClearRenderTargetView(pRtv, c);
	Unlock();
	return D3D_OK;
}

HRESULT CDev11::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	if (!pMatrix) return D3DERR_INVALIDCALL;
	if (State == D3DTS_VIEW) m_matView = *pMatrix; else if (State == D3DTS_PROJECTION) m_matProj = *pMatrix; else if (State == D3DTS_WORLD) m_matWorld = *pMatrix; else return D3D_OK;
	m_bVsDirty = true;
	return D3D_OK;
}
HRESULT CDev11::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
	if (!pMatrix) return D3DERR_INVALIDCALL;
	if (State == D3DTS_VIEW) *pMatrix = m_matView; else if (State == D3DTS_PROJECTION) *pMatrix = m_matProj; else *pMatrix = m_matWorld;
	return D3D_OK;
}
HRESULT CDev11::SetViewport(CONST D3DVIEWPORT9* pViewport) { if (!pViewport) return D3DERR_INVALIDCALL; m_vp = *pViewport; m_bVsDirty = true; return D3D_OK; }
HRESULT CDev11::GetViewport(D3DVIEWPORT9* pViewport) { if (!pViewport) return D3DERR_INVALIDCALL; *pViewport = m_vp; return D3D_OK; }
HRESULT CDev11::SetScissorRect(CONST RECT* pRect) { if (pRect) m_scissor = *pRect; return D3D_OK; }
HRESULT CDev11::GetScissorRect(RECT* pRect) { if (pRect) *pRect = m_scissor; return D3D_OK; }

// ---------------------------------------------------------------- trang thai
void CDev11::SetStateInternal(DWORD key, DWORD value)
{
	DWORD kind = key & R11SB_KINDMASK; DWORD stage = (key >> 16) & 0xFF; DWORD type = key & 0xFFFF;
	if (kind == R11SB_RS) { if (type < 256) { m_rs[type] = value; m_bPsDirty = true; } }
	else if (kind == R11SB_TSS) { if (stage < 8 && type < 33) { m_tss[stage][type] = value; m_bPsDirty = true; } }
	else if (kind == R11SB_SS) { if (stage < 8 && type < 14) m_ss[stage][type] = value; }
}
DWORD CDev11::GetStateInternal(DWORD key)
{
	DWORD kind = key & R11SB_KINDMASK; DWORD stage = (key >> 16) & 0xFF; DWORD type = key & 0xFFFF;
	if (kind == R11SB_RS) return (type < 256) ? m_rs[type] : 0;
	if (kind == R11SB_TSS) return (stage < 8 && type < 33) ? m_tss[stage][type] : 0;
	if (kind == R11SB_SS) return (stage < 8 && type < 14) ? m_ss[stage][type] : 0;
	return 0;
}
HRESULT CDev11::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	if ((DWORD)State >= 256) return D3DERR_INVALIDCALL;
	if (m_pRecord) { m_pRecord->Record(R11SB_RS | (DWORD)State, Value); return D3D_OK; }
	if (m_rs[State] != Value) { m_rs[State] = Value; m_bPsDirty = true; }
	return D3D_OK;
}
HRESULT CDev11::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) { if (!pValue || (DWORD)State >= 256) return D3DERR_INVALIDCALL; *pValue = m_rs[State]; return D3D_OK; }
HRESULT CDev11::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	if (Stage >= 8 || (DWORD)Type >= 33) return D3DERR_INVALIDCALL;
	if (m_pRecord) { m_pRecord->Record(R11SB_TSS | (Stage << 16) | (DWORD)Type, Value); return D3D_OK; }
	if (m_tss[Stage][Type] != Value) { m_tss[Stage][Type] = Value; m_bPsDirty = true; }
	return D3D_OK;
}
HRESULT CDev11::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) { if (!pValue || Stage >= 8 || (DWORD)Type >= 33) return D3DERR_INVALIDCALL; *pValue = m_tss[Stage][Type]; return D3D_OK; }
HRESULT CDev11::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	if (Sampler >= 8 || (DWORD)Type >= 14) return D3DERR_INVALIDCALL;
	if (m_pRecord) { m_pRecord->Record(R11SB_SS | (Sampler << 16) | (DWORD)Type, Value); return D3D_OK; }
	m_ss[Sampler][Type] = Value;
	return D3D_OK;
}
HRESULT CDev11::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) { if (!pValue || Sampler >= 8 || (DWORD)Type >= 14) return D3DERR_INVALIDCALL; *pValue = m_ss[Sampler][Type]; return D3D_OK; }

HRESULT CDev11::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
	if (!ppSB) return E_POINTER;
	CSB11* p = new CSB11(this);
	for (DWORD i = 0; i < 256; i++) p->Record(R11SB_RS | i, m_rs[i]);
	for (DWORD s = 0; s < 8; s++) { for (DWORD t = 1; t < 33; t++) p->Record(R11SB_TSS | (s << 16) | t, m_tss[s][t]); for (DWORD t = 1; t < 14; t++) p->Record(R11SB_SS | (s << 16) | t, m_ss[s][t]); }
	*ppSB = p;
	return D3D_OK;
}
HRESULT CDev11::BeginStateBlock() { if (m_pRecord) return D3DERR_INVALIDCALL; m_pRecord = new CSB11(this); return D3D_OK; }
HRESULT CDev11::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
	if (!ppSB) return E_POINTER;
	if (!m_pRecord) return D3DERR_INVALIDCALL;
	*ppSB = m_pRecord; m_pRecord = NULL;
	return D3D_OK;
}

HRESULT CDev11::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
	if (Stage >= 8) return D3DERR_INVALIDCALL;
	CTex11* p = (CTex11*)pTexture;
	if (m_tex[Stage] == p) return D3D_OK;
	if (p) p->AddRef();
	if (m_tex[Stage]) m_tex[Stage]->Release();
	m_tex[Stage] = p;
	m_bPsDirty = true;
	return D3D_OK;
}
HRESULT CDev11::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{
	if (!ppTexture || Stage >= 8) return D3DERR_INVALIDCALL;
	*ppTexture = m_tex[Stage]; if (m_tex[Stage]) m_tex[Stage]->AddRef();
	return D3D_OK;
}
HRESULT CDev11::SetFVF(DWORD FVF) { m_fvf = FVF; return D3D_OK; }
HRESULT CDev11::GetFVF(DWORD* pFVF) { if (!pFVF) return E_POINTER; *pFVF = m_fvf; return D3D_OK; }
HRESULT CDev11::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	if (StreamNumber != 0) return D3DERR_INVALIDCALL;
	CVB11* p = (CVB11*)pStreamData;
	if (p) p->AddRef();
	if (m_pStream) m_pStream->Release();
	m_pStream = p; m_streamOffset = OffsetInBytes; m_streamStride = Stride;
	return D3D_OK;
}
HRESULT CDev11::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)
{
	if (StreamNumber != 0 || !ppStreamData) return D3DERR_INVALIDCALL;
	*ppStreamData = m_pStream; if (m_pStream) m_pStream->AddRef();
	if (pOffsetInBytes) *pOffsetInBytes = m_streamOffset; if (pStride) *pStride = m_streamStride;
	return D3D_OK;
}

// ---------------------------------------------------------------- doi tuong trang thai D3D11
ID3D11InputLayout* CDev11::GetInputLayout(DWORD fvf, UINT stride)
{
	DWORD keyIL = (fvf & 0xFFFF) | (stride << 16);	// [r] layout phu thuoc stride (PALROW o cuoi)
	std::map<DWORD, ID3D11InputLayout*>::iterator it = m_layouts.find(keyIL);
	if (it != m_layouts.end()) return it->second;
	D3D11_INPUT_ELEMENT_DESC ie[4]; int n = 0; UINT off = 0;
	DWORD posType = fvf & D3DFVF_POSITION_MASK;
	bool bRhw = (posType == D3DFVF_XYZRHW);
	ie[n].SemanticName = "POSITION"; ie[n].SemanticIndex = 0; ie[n].Format = bRhw ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R32G32B32_FLOAT;
	ie[n].InputSlot = 0; ie[n].AlignedByteOffset = 0; ie[n].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; ie[n].InstanceDataStepRate = 0; n++;
	off = bRhw ? 16 : 12;
	if (posType == D3DFVF_XYZB1) off += 4; else if (posType == D3DFVF_XYZB2) off += 8; else if (posType == D3DFVF_XYZB3) off += 12; else if (posType == D3DFVF_XYZB4) off += 16; else if (posType == D3DFVF_XYZB5) off += 20;
	if (fvf & D3DFVF_NORMAL) off += 12;
	if (fvf & D3DFVF_PSIZE) off += 4;
	ie[n].SemanticName = "COLOR"; ie[n].SemanticIndex = 0; ie[n].Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	if (fvf & D3DFVF_DIFFUSE) { ie[n].InputSlot = 0; ie[n].AlignedByteOffset = off; ie[n].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; ie[n].InstanceDataStepRate = 0; off += 4; }
	else { ie[n].InputSlot = 1; ie[n].AlignedByteOffset = 0; ie[n].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA; ie[n].InstanceDataStepRate = 0; }
	n++;
	if (fvf & D3DFVF_SPECULAR) off += 4;
	ie[n].SemanticName = "TEXCOORD"; ie[n].SemanticIndex = 0; ie[n].Format = DXGI_FORMAT_R32G32_FLOAT;
	if (((fvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT) >= 1) { ie[n].InputSlot = 0; ie[n].AlignedByteOffset = off; ie[n].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; ie[n].InstanceDataStepRate = 0; }
	else { ie[n].InputSlot = 1; ie[n].AlignedByteOffset = 4; ie[n].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA; ie[n].InstanceDataStepRate = 0; }
	n++;
	ie[n].SemanticName = "PALROW"; ie[n].SemanticIndex = 0; ie[n].Format = DXGI_FORMAT_R32_UINT; ie[n].InputSlot = 0; ie[n].AlignedByteOffset = stride; ie[n].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; ie[n].InstanceDataStepRate = 0; n++;	// [r] hang bang mau, 4 byte sau dinh D3D9
	ID3D11InputLayout* pIL = NULL;
	HRESULT hr = m_pDev->CreateInputLayout(ie, n, g_Rep3VS11, sizeof(g_Rep3VS11), &pIL);
	if (FAILED(hr)) { R11Log("CreateInputLayout fvf 0x%X that bai 0x%08X", (unsigned)fvf, (unsigned)hr); pIL = NULL; }
	m_layouts[keyIL] = pIL;
	return pIL;
}

ID3D11BlendState* CDev11::GetBlendState()
{
	DWORD en = m_rs[D3DRS_ALPHABLENDENABLE] ? 1 : 0, src = m_rs[D3DRS_SRCBLEND] & 31, dst = m_rs[D3DRS_DESTBLEND] & 31, op = m_rs[D3DRS_BLENDOP] & 7, wm = m_rs[D3DRS_COLORWRITEENABLE] & 15;
	DWORD key = en | (src << 1) | (dst << 6) | (op << 11) | (wm << 14);
	std::map<DWORD, ID3D11BlendState*>::iterator it = m_blends.find(key);
	if (it != m_blends.end()) return it->second;
	D3D11_BLEND_DESC bd; memset(&bd, 0, sizeof(bd));
	bd.RenderTarget[0].BlendEnable = en ? TRUE : FALSE;
	bd.RenderTarget[0].SrcBlend = (D3D11_BLEND)(src ? src : 2); bd.RenderTarget[0].DestBlend = (D3D11_BLEND)(dst ? dst : 1);
	bd.RenderTarget[0].BlendOp = (D3D11_BLEND_OP)(op ? op : 1);
	// alpha: giong D3D9 khi khong co SEPARATEALPHABLEND (dung cung he so; he so mau chuyen sang alpha)
	D3D11_BLEND sa = bd.RenderTarget[0].SrcBlend, da = bd.RenderTarget[0].DestBlend;
	if (sa == D3D11_BLEND_SRC_COLOR) sa = D3D11_BLEND_SRC_ALPHA; else if (sa == D3D11_BLEND_INV_SRC_COLOR) sa = D3D11_BLEND_INV_SRC_ALPHA; else if (sa == D3D11_BLEND_DEST_COLOR) sa = D3D11_BLEND_DEST_ALPHA; else if (sa == D3D11_BLEND_INV_DEST_COLOR) sa = D3D11_BLEND_INV_DEST_ALPHA;
	if (da == D3D11_BLEND_SRC_COLOR) da = D3D11_BLEND_SRC_ALPHA; else if (da == D3D11_BLEND_INV_SRC_COLOR) da = D3D11_BLEND_INV_SRC_ALPHA; else if (da == D3D11_BLEND_DEST_COLOR) da = D3D11_BLEND_DEST_ALPHA; else if (da == D3D11_BLEND_INV_DEST_COLOR) da = D3D11_BLEND_INV_DEST_ALPHA;
	bd.RenderTarget[0].SrcBlendAlpha = sa; bd.RenderTarget[0].DestBlendAlpha = da; bd.RenderTarget[0].BlendOpAlpha = bd.RenderTarget[0].BlendOp;
	bd.RenderTarget[0].RenderTargetWriteMask = (UINT8)wm;
	ID3D11BlendState* p = NULL;
	if (FAILED(m_pDev->CreateBlendState(&bd, &p))) { R11Log("CreateBlendState key 0x%X that bai", (unsigned)key); p = NULL; }
	m_blends[key] = p;
	return p;
}

ID3D11SamplerState* CDev11::GetSamplerState(UINT stage)
{
	DWORD mn = m_ss[stage][D3DSAMP_MINFILTER] & 7, mg = m_ss[stage][D3DSAMP_MAGFILTER] & 7, mp = m_ss[stage][D3DSAMP_MIPFILTER] & 7;
	DWORD au = m_ss[stage][D3DSAMP_ADDRESSU] & 7, av = m_ss[stage][D3DSAMP_ADDRESSV] & 7;
	DWORD key = mn | (mg << 3) | (mp << 6) | (au << 9) | (av << 12);
	std::map<DWORD, ID3D11SamplerState*>::iterator it = m_samplers.find(key);
	if (it != m_samplers.end()) return it->second;
	D3D11_SAMPLER_DESC sd; memset(&sd, 0, sizeof(sd));
	bool bMinL = mn >= D3DTEXF_LINEAR, bMagL = mg >= D3DTEXF_LINEAR, bMipL = mp >= D3DTEXF_LINEAR;
	if (bMinL && bMagL) sd.Filter = bMipL ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	else if (bMinL && !bMagL) sd.Filter = bMipL ? D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR : D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
	else if (!bMinL && bMagL) sd.Filter = bMipL ? D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR : D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
	else sd.Filter = bMipL ? D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
	sd.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)(au ? au : 1); sd.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)(av ? av : 1); sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.MaxAnisotropy = 1; sd.ComparisonFunc = D3D11_COMPARISON_NEVER; sd.MinLOD = 0; sd.MaxLOD = D3D11_FLOAT32_MAX;
	ID3D11SamplerState* p = NULL;
	if (FAILED(m_pDev->CreateSamplerState(&sd, &p))) { R11Log("CreateSamplerState key 0x%X that bai", (unsigned)key); p = NULL; }
	m_samplers[key] = p;
	return p;
}

ID3D11RasterizerState* CDev11::GetRasterState()
{
	DWORD cull = m_rs[D3DRS_CULLMODE] & 3, fill = m_rs[D3DRS_FILLMODE] & 3, sc = m_rs[D3DRS_SCISSORTESTENABLE] ? 1 : 0;
	DWORD key = cull | (fill << 2) | (sc << 4);
	std::map<DWORD, ID3D11RasterizerState*>::iterator it = m_rasters.find(key);
	if (it != m_rasters.end()) return it->second;
	D3D11_RASTERIZER_DESC rd; memset(&rd, 0, sizeof(rd));
	rd.FillMode = (fill == D3DFILL_WIREFRAME) ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	rd.CullMode = (cull == D3DCULL_CW) ? D3D11_CULL_FRONT : ((cull == D3DCULL_CCW) ? D3D11_CULL_BACK : D3D11_CULL_NONE);
	rd.FrontCounterClockwise = FALSE; rd.DepthClipEnable = FALSE; rd.ScissorEnable = sc ? TRUE : FALSE; rd.MultisampleEnable = FALSE;
	ID3D11RasterizerState* p = NULL;
	if (FAILED(m_pDev->CreateRasterizerState(&rd, &p))) { R11Log("CreateRasterizerState key 0x%X that bai", (unsigned)key); p = NULL; }
	m_rasters[key] = p;
	return p;
}

void CDev11::ResetAppliedState()
{
	m_lastBlendKey = 0xFFFFFFFF; m_lastRasterKey = 0xFFFFFFFF; m_lastSampKey[0] = m_lastSampKey[1] = 0xFFFFFFFF;
	m_lastSrv[0] = m_lastSrv[1] = (ID3D11ShaderResourceView*)1; m_lastIL = NULL; m_lastTopo = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	memset(&m_lastVp, 0xFF, sizeof(m_lastVp)); m_bPipeBound = false;
	m_bAppliedValid = false;
}

// [j] tinh trang thai D3D11 cho lenh ve hien tai (khong goi context)
void CDev11::ComputeApplied(R11Applied& a, ID3D11InputLayout* pIL, UINT stride)
{
	memset(&a, 0, sizeof(a));
	a.pIL = pIL; a.stride = stride;
	a.vp = m_vp;
	a.bScissor = m_rs[D3DRS_SCISSORTESTENABLE] ? 1 : 0; a.rcScissor = m_scissor;
	a.pRaster = GetRasterState();
	a.pBlend = GetBlendState();
	a.pSamp[0] = GetSamplerState(0); a.pSamp[1] = GetSamplerState(1);
	for (int s = 0; s < 2; s++)
	{
		if (m_tex[s]) { m_tex[s]->PrepareForBind(); a.srv[s] = m_tex[s]->m_pSrv; }
		if (m_pRt && m_pRt->m_pTex && m_tex[s] == m_pRt->m_pTex) a.srv[s] = NULL;
	}
	bool bRhw = ((m_fvf & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW);
	if (m_bVsDirty || (m_vsCb.flags[0] > 0.5f) != bRhw)
	{
		m_vsCb.vp[0] = (float)(m_vp.Width ? m_vp.Width : 1); m_vsCb.vp[1] = (float)(m_vp.Height ? m_vp.Height : 1); m_vsCb.vp[2] = (float)m_vp.X; m_vsCb.vp[3] = (float)m_vp.Y;
		D3DXMATRIX wv, wvp;
		D3DXMatrixMultiply(&wv, (const D3DXMATRIX*)&m_matWorld, (const D3DXMATRIX*)&m_matView);
		D3DXMatrixMultiply(&wvp, &wv, (const D3DXMATRIX*)&m_matProj);
		memcpy(m_vsCb.wvp, &wvp, sizeof(float) * 16);
		m_vsCb.flags[0] = bRhw ? 1.0f : 0.0f;
		m_bVsDirty = false;
	}
	a.vs = m_vsCb;
	R11PsCb& cb = a.ps;
	cb.st0[0] = (int)m_tss[0][D3DTSS_COLOROP]; cb.st0[1] = (int)m_tss[0][D3DTSS_COLORARG1]; cb.st0[2] = (int)m_tss[0][D3DTSS_COLORARG2]; cb.st0[3] = (int)m_tss[0][D3DTSS_ALPHAOP];
	cb.st0b[0] = (int)m_tss[0][D3DTSS_ALPHAARG1]; cb.st0b[1] = (int)m_tss[0][D3DTSS_ALPHAARG2]; cb.st0b[2] = a.srv[0] ? 1 : 0;
	cb.st0b[3] = (m_bPalLinForce || (m_ss[0][D3DSAMP_MAGFILTER] & 7) >= D3DTEXF_LINEAR || (m_ss[0][D3DSAMP_MINFILTER] & 7) >= D3DTEXF_LINEAR) ? 1 : 0;	// [r2] loc tuyen tinh stage 0 -> shader tu noi suy texture bang mau
	cb.st1[0] = (int)m_tss[1][D3DTSS_COLOROP]; cb.st1[1] = (int)m_tss[1][D3DTSS_COLORARG1]; cb.st1[2] = (int)m_tss[1][D3DTSS_COLORARG2]; cb.st1[3] = (int)m_tss[1][D3DTSS_ALPHAOP];
	cb.st1b[0] = (int)m_tss[1][D3DTSS_ALPHAARG1]; cb.st1b[1] = (int)m_tss[1][D3DTSS_ALPHAARG2]; cb.st1b[2] = a.srv[1] ? 1 : 0; cb.st1b[3] = 0;
	cb.at[0] = m_rs[D3DRS_ALPHATESTENABLE] ? 1.0f : 0.0f; cb.at[1] = (float)(m_rs[D3DRS_ALPHAFUNC] & 15); cb.at[2] = (float)(m_rs[D3DRS_ALPHAREF] & 255); cb.at[3] = 0.0f;
}

// [j] gan len context nhung gi khac voi lan gan truoc
void CDev11::ApplyComputed(const R11Applied& a)
{
	if (!m_bRtBound) { BindRenderTarget(); m_bAppliedValid = false; m_bPipeBound = false; }
	if (!m_bPipeBound)
	{
		m_pCtx->VSSetShader(m_pVS, NULL, 0); m_pCtx->PSSetShader(m_pPS, NULL, 0);
		m_pCtx->VSSetConstantBuffers(0, 1, &m_pVsCb); m_pCtx->PSSetConstantBuffers(0, 1, &m_pPsCb);
		m_pCtx->OMSetDepthStencilState(m_pDss, 0);
		if (m_pPalSrv) m_pCtx->PSSetShaderResources(2, 1, &m_pPalSrv);	// [r]
		m_bPipeBound = true;
	}
	const bool v = m_bAppliedValid;
	const R11Applied& L = m_lastApplied;
	if (!v || memcmp(&a.vp, &L.vp, sizeof(a.vp)) != 0)
	{
		D3D11_VIEWPORT vp; vp.TopLeftX = (float)a.vp.X; vp.TopLeftY = (float)a.vp.Y; vp.Width = (float)a.vp.Width; vp.Height = (float)a.vp.Height; vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
		m_pCtx->RSSetViewports(1, &vp);
	}
	if (!v || a.pRaster != L.pRaster || a.bScissor != L.bScissor || memcmp(&a.rcScissor, &L.rcScissor, sizeof(RECT)) != 0)
	{
		if (a.bScissor) { D3D11_RECT r = { a.rcScissor.left, a.rcScissor.top, a.rcScissor.right, a.rcScissor.bottom }; m_pCtx->RSSetScissorRects(1, &r); }
		m_pCtx->RSSetState(a.pRaster);
	}
	if (!v || a.pBlend != L.pBlend) { float bf[4] = { 0, 0, 0, 0 }; m_pCtx->OMSetBlendState(a.pBlend, bf, 0xFFFFFFFF); }
	if (!v || a.pSamp[0] != L.pSamp[0] || a.pSamp[1] != L.pSamp[1]) m_pCtx->PSSetSamplers(0, 2, a.pSamp);
	if (!v || a.srv[0] != L.srv[0] || a.srv[1] != L.srv[1]) m_pCtx->PSSetShaderResources(0, 2, a.srv);
	if (!v || memcmp(&a.vs, &L.vs, sizeof(a.vs)) != 0)
	{
		D3D11_MAPPED_SUBRESOURCE ms;
		if (SUCCEEDED(m_pCtx->Map(m_pVsCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) { memcpy(ms.pData, &a.vs, sizeof(a.vs)); m_pCtx->Unmap(m_pVsCb, 0); }
	}
	if (!v || memcmp(&a.ps, &L.ps, sizeof(a.ps)) != 0)
	{
		D3D11_MAPPED_SUBRESOURCE ms;
		if (SUCCEEDED(m_pCtx->Map(m_pPsCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) { memcpy(ms.pData, &a.ps, sizeof(a.ps)); m_pCtx->Unmap(m_pPsCb, 0); }
	}
	if (!v || a.pIL != L.pIL) m_pCtx->IASetInputLayout(a.pIL);
	m_lastApplied = a; m_bAppliedValid = true;
}

void CDev11::ApplyState()
{
	R11Applied a;
	ComputeApplied(a, m_lastIL, m_streamStride);
	ApplyComputed(a);
}

// chep dinh vao ring; tra vi tri byte
void CDev11::UploadRing(const BYTE* pData, UINT bytes, UINT stride, UINT* pPos)
{
	UINT pos = m_ringPos;
	if (stride && (pos % stride)) pos += stride - (pos % stride);
	D3D11_MAP mapType = D3D11_MAP_WRITE_NO_OVERWRITE;
	// [D3D11 08/09 c] dau moi khung (sau Present) hoac het ring: DISCARD de driver cap vung moi.
	if (m_bRingDiscard || pos + bytes > m_ringSize) { pos = 0; mapType = D3D11_MAP_WRITE_DISCARD; m_bRingDiscard = false; }
	D3D11_MAPPED_SUBRESOURCE ms;
	HRESULT hr = m_pCtx->Map(m_pRing, 0, mapType, 0, &ms);
	if (FAILED(hr)) { R11Log("Map ring that bai 0x%08X", (unsigned)hr); *pPos = 0xFFFFFFFF; return; }
	memcpy((BYTE*)ms.pData + pos, pData, bytes);
	m_pCtx->Unmap(m_pRing, 0);
	m_ringPos = pos + bytes;
	*pPos = pos;
}

// [j] ve lo quad dang cho bang MOT Draw voi trang thai da chup luc dua lenh
void CDev11::FlushBatch()
{
	if (!m_batchVerts) return;
	Lock();
	UINT nVerts = m_batchVerts, stride = m_batchState.stride + 4, bytes = nVerts * stride;	// [r] +4 byte PALROW moi dinh
	m_batchVerts = 0;
	if (bytes > m_ringSize) { m_batch.clear(); Unlock(); return; }
	UINT pos = 0;
	UploadRing(&m_batch[0], bytes, stride, &pos);
	m_batch.clear();
	if (pos == 0xFFFFFFFF) { Unlock(); return; }
	ApplyComputed(m_batchState);
	ID3D11Buffer* bufs[2] = { m_pRing, m_pDummy }; UINT strides[2] = { stride, 16 }; UINT offs[2] = { pos, 0 };
	m_pCtx->IASetVertexBuffers(0, 2, bufs, strides, offs);
	if (m_lastTopo != D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) { m_pCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); m_lastTopo = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; }
	m_pCtx->Draw(nVerts, 0);
	g_uRep3BatchDraws++;
	Unlock();
}

static UINT R11VertexCount(D3DPRIMITIVETYPE t, UINT n)
{
	switch (t)
	{
	case D3DPT_POINTLIST: return n;
	case D3DPT_LINELIST: return n * 2;
	case D3DPT_LINESTRIP: return n + 1;
	case D3DPT_TRIANGLELIST: return n * 3;
	case D3DPT_TRIANGLESTRIP: return n + 2;
	case D3DPT_TRIANGLEFAN: return n + 2;
	}
	return 0;
}

struct R11DrawTimer
{
	LARGE_INTEGER t0;
	R11DrawTimer() { QueryPerformanceCounter(&t0); }
	~R11DrawTimer() { LARGE_INTEGER t1; QueryPerformanceCounter(&t1); g_dRep3DrawMs += R11Ms(t0, t1); g_uRep3Draws++; }
};

// uv cua texture ao (stage 0) -> uv trong trang atlas, sua tai cho tren dinh da chep
static void R11AtlasUv(BYTE* pV, UINT nVerts, UINT stride, DWORD fvf, CTex11* pTex, float fPage)
{
	if (!pTex || !pTex->m_bVirtual || !pTex->m_pPage || ((fvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT) < 1) return;
	UINT uvOff = ((fvf & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW) ? 16 : 12;
	DWORD pt = fvf & D3DFVF_POSITION_MASK;
	if (pt == D3DFVF_XYZB1) uvOff += 4; else if (pt == D3DFVF_XYZB2) uvOff += 8; else if (pt == D3DFVF_XYZB3) uvOff += 12; else if (pt == D3DFVF_XYZB4) uvOff += 16; else if (pt == D3DFVF_XYZB5) uvOff += 20;
	if (fvf & D3DFVF_NORMAL) uvOff += 12; if (fvf & D3DFVF_PSIZE) uvOff += 4; if (fvf & D3DFVF_DIFFUSE) uvOff += 4; if (fvf & D3DFVF_SPECULAR) uvOff += 4;
	const float sx = (float)pTex->m_w / fPage, sy = (float)pTex->m_h / fPage, ox = (float)pTex->m_ax / fPage, oy = (float)pTex->m_ay / fPage;
	for (UINT i = 0; i < nVerts; i++) { float* uv = (float*)(pV + i * stride + uvOff); uv[0] = uv[0] * sx + ox; uv[1] = uv[1] * sy + oy; }
}

HRESULT CDev11::DrawInternal(D3DPRIMITIVETYPE type, const BYTE* pVerts, UINT nVerts, UINT stride)
{
	R11DrawTimer timer;
	if (!pVerts || nVerts == 0 || stride == 0) return D3DERR_INVALIDCALL;
	ID3D11InputLayout* pIL = GetInputLayout(m_fvf, stride);
	if (!pIL) return D3DERR_INVALIDCALL;
	const float fPage = m_pAtlas ? (float)m_pAtlas->m_pageSize : 1024.0f;
	// ---- [j] quad (strip 4 dinh): gop
	if (g_nRep3Batch && type == D3DPT_TRIANGLESTRIP && nVerts == 4)
	{
		R11Applied a;
		ComputeApplied(a, pIL, stride);
		if (m_batchVerts && (memcmp(&a, &m_batchState, sizeof(a)) != 0 || m_batch.size() + 6 * stride > 2 * 1024 * 1024))
			FlushBatch();
		if (!m_batchVerts) m_batchState = a;
		const UINT s11 = stride + 4;	// [r] +4 byte PALROW
		const UINT uPal = (m_tex[0] && m_tex[0]->m_nPalRow >= 0) ? (UINT)m_tex[0]->m_nPalRow : 0xFFFFu;
		size_t base = m_batch.size();
		m_batch.resize(base + 6 * s11);
		BYTE* d = &m_batch[base];
		static const int s_idx[6] = { 0, 1, 2, 2, 1, 3 };
		for (int i = 0; i < 6; i++) { memcpy(d + i * s11, pVerts + s_idx[i] * stride, stride); *(UINT*)(d + i * s11 + stride) = uPal; }
		R11AtlasUv(d, 6, s11, m_fvf, m_tex[0], fPage);
		m_batchVerts += 6;
		g_uRep3BatchQuads++;
		return D3D_OK;
	}
	FlushIfPending();
	// ---- lenh khac: ve ngay
	std::vector<BYTE> tmp;
	const UINT s11 = stride + 4;	// [r] +4 byte PALROW
	const UINT uPal = (m_tex[0] && m_tex[0]->m_nPalRow >= 0) ? (UINT)m_tex[0]->m_nPalRow : 0xFFFFu;
	if (type == D3DPT_TRIANGLEFAN)
	{
		UINT nTri = nVerts - 2;
		tmp.resize((size_t)nTri * 3 * s11);
		for (UINT i = 0; i < nTri; i++)
		{
			const UINT src[3] = { 0, i + 1, i + 2 };
			for (int k = 0; k < 3; k++) { memcpy(&tmp[(i * 3 + k) * s11], pVerts + src[k] * stride, stride); *(UINT*)(&tmp[(i * 3 + k) * s11 + stride]) = uPal; }
		}
		nVerts = nTri * 3; type = D3DPT_TRIANGLELIST;
	}
	else
	{
		tmp.resize((size_t)nVerts * s11);
		for (UINT i = 0; i < nVerts; i++) { memcpy(&tmp[i * s11], pVerts + i * stride, stride); *(UINT*)(&tmp[i * s11 + stride]) = uPal; }
	}
	R11AtlasUv(&tmp[0], nVerts, s11, m_fvf, m_tex[0], fPage);
	UINT bytes = nVerts * s11;
	if (bytes > m_ringSize) { R11Log("DrawInternal: %u byte vuot ring", bytes); return D3DERR_INVALIDCALL; }
	UINT pos = 0;
	UploadRing(&tmp[0], bytes, s11, &pos);
	if (pos == 0xFFFFFFFF) return D3DERR_INVALIDCALL;
	R11Applied a;
	ComputeApplied(a, pIL, stride);
	ApplyComputed(a);
	ID3D11Buffer* bufs[2] = { m_pRing, m_pDummy }; UINT strides[2] = { s11, 16 }; UINT offs[2] = { pos, 0 };
	m_pCtx->IASetVertexBuffers(0, 2, bufs, strides, offs);
	D3D11_PRIMITIVE_TOPOLOGY topo = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	switch (type)
	{
	case D3DPT_POINTLIST: topo = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST; break;
	case D3DPT_LINELIST: topo = D3D11_PRIMITIVE_TOPOLOGY_LINELIST; break;
	case D3DPT_LINESTRIP: topo = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
	case D3DPT_TRIANGLESTRIP: topo = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
	default: break;
	}
	if (topo != m_lastTopo) { m_pCtx->IASetPrimitiveTopology(topo); m_lastTopo = topo; }
	m_pCtx->Draw(nVerts, 0);
	return D3D_OK;
}

HRESULT CDev11::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	if (!m_pStream || !m_pStream->m_pCpu || m_streamStride == 0) return D3DERR_INVALIDCALL;
	UINT nVerts = R11VertexCount(PrimitiveType, PrimitiveCount);
	UINT off = m_streamOffset + StartVertex * m_streamStride;
	if (nVerts == 0 || off + nVerts * m_streamStride > m_pStream->m_len) return D3DERR_INVALIDCALL;
	Lock();
	HRESULT hr = DrawInternal(PrimitiveType, m_pStream->m_pCpu + off, nVerts, m_streamStride);
	Unlock();
	return hr;
}

HRESULT CDev11::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	UINT nVerts = R11VertexCount(PrimitiveType, PrimitiveCount);
	if (nVerts == 0 || !pVertexStreamZeroData) return D3DERR_INVALIDCALL;
	Lock();
	HRESULT hr = DrawInternal(PrimitiveType, (const BYTE*)pVertexStreamZeroData, nVerts, VertexStreamZeroStride);
	Unlock();
	return hr;
}
