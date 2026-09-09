// [GPU 08/09] D3D9 tren SDL_GPU - noi bo. Chi khi JX_PLATFORM_SDL.
//
// Mo hinh:
//  - Moi lenh D3D9 (SetRenderState/SetTexture/Draw...) chi ghi trang thai; Draw ghi mot RgCmd (trang thai da tinh + dinh trong ring CPU).
//    Quad (TRIANGLESTRIP 4 dinh) cung trang thai va lien tiep -> gop vao mot lenh (nhu D3D9on11 [j]).
//  - Present: 1 command buffer = [copy pass: tai ring dinh + moi texture ban] [render pass tung render target: phat lai RgCmd] [copy khung
//    vua ve sang m_pLastFrame de chup man hinh] -> submit.
//  - Texture: luon giu ban CPU (dinh dang D3D9). GPU = "phien ban" SDL_GPUTexture; neu texture da duoc lenh ve trong khung tham chieu roi
//    ma lai bi sua (LockRect/UnlockRect) -> tao phien ban moi (ban cu tra sau Present) de lenh cu van thay noi dung cu.
//  - Dinh dang 16 bit (4444/1555/565) va 24 bit: CPU giu nguyen, tai len BGRA8 (doi khi tai) - chac chan dung thu tu kenh tren moi backend;
//    A8L8 (chi so + alpha) -> R8G8 cho bang mau; A8R8G8B8/X8R8G8B8 -> B8G8R8A8 truc tiep.
#pragma once
#ifdef JX_PLATFORM_SDL

#include <d3d9.h>
#include <SDL3/SDL.h>
#include <vector>
#include <map>

class CDevGpu; class CTexGpu; class CSurfGpu; class CVBGpu; class CSBGpu; class CGpuShim;

void RgLog(const char* fmt, ...);
void RgStub(const char* szName);			// ghi log MOT lan moi ham chua cai

// ---------------------------------------------------------------- dinh dang
struct RgFmt
{
	SDL_GPUTextureFormat gpu;	// dinh dang GPU dung de tai len (BGRA8 khi doi)
	int  bpp;					// byte moi diem CPU theo D3D9 (0 = khong ho tro)
	bool bConvert;				// tai len phai doi tung hang sang BGRA8
};
RgFmt RgFormatInfo(D3DFORMAT f);
UINT  RgPitch(D3DFORMAT f, UINT w);				// pitch CPU theo D3D9 (byte)
UINT  RgGpuBpp(SDL_GPUTextureFormat f);
void  RgConvertRowToBgra(D3DFORMAT f, const BYTE* pSrc, DWORD* pDst, UINT w);

// ---------------------------------------------------------------- texture
class CTexGpu : public IDirect3DTexture9
{
public:
	CTexGpu(CDevGpu* pDev, UINT w, UINT h, DWORD usage, D3DFORMAT fmt, D3DPOOL pool);
	virtual ~CTexGpu();

	// IUnknown
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj);
	virtual ULONG   __stdcall AddRef();
	virtual ULONG   __stdcall Release();
	// IDirect3DResource9
	virtual HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice);
	virtual HRESULT __stdcall SetPrivateData(REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags) { return D3D_OK; }
	virtual HRESULT __stdcall GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) { return D3DERR_NOTFOUND; }
	virtual HRESULT __stdcall FreePrivateData(REFGUID refguid) { return D3D_OK; }
	virtual DWORD   __stdcall SetPriority(DWORD PriorityNew) { return 0; }
	virtual DWORD   __stdcall GetPriority() { return 0; }
	virtual void    __stdcall PreLoad() {}
	virtual D3DRESOURCETYPE __stdcall GetType() { return D3DRTYPE_TEXTURE; }
	// IDirect3DBaseTexture9
	virtual DWORD   __stdcall SetLOD(DWORD LODNew) { return 0; }
	virtual DWORD   __stdcall GetLOD() { return 0; }
	virtual DWORD   __stdcall GetLevelCount() { return 1; }
	virtual HRESULT __stdcall SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType) { return D3D_OK; }
	virtual D3DTEXTUREFILTERTYPE __stdcall GetAutoGenFilterType() { return D3DTEXF_NONE; }
	virtual void    __stdcall GenerateMipSubLevels() {}
	// IDirect3DTexture9
	virtual HRESULT __stdcall GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc);
	virtual HRESULT __stdcall GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel);
	virtual HRESULT __stdcall LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags);
	virtual HRESULT __stdcall UnlockRect(UINT Level);
	virtual HRESULT __stdcall AddDirtyRect(CONST RECT* pDirtyRect) { return D3D_OK; }

	// noi bo
	SDL_GPUTexture* PrepareForBind();		// truoc khi ve: dam bao phien ban GPU dung noi dung CPU; NULL neu khong tao duoc
	SDL_GPUTexture* PrepareAsTarget();		// truoc khi lam render target: phien ban co COLOR_TARGET, chua du lieu CPU
	void  ReleaseGpu();
	void  MarkUsed() { m_bUsedThisFrame = true; }
	void  FrameEnd()  { m_bUsedThisFrame = false; }
	bool  NewVersion(bool bTarget);			// tao SDL_GPUTexture moi (ban cu vao danh sach tra sau Present)
	void  QueueUpload(const RECT* prc);		// chep CPU (vung prc) vao staging cua khung + ghi lenh tai

	LONG        m_ref;
	CDevGpu*    m_pDev;
	UINT        m_w, m_h;
	DWORD       m_usage;
	D3DFORMAT   m_fmt;
	D3DPOOL     m_pool;
	UINT        m_pitch;			// pitch CPU
	BYTE*       m_pCpu;				// ban CPU (dinh dang D3D9); render target thuan: NULL
	RgFmt       m_fi;
	SDL_GPUTexture* m_pGpu;			// phien ban hien tai
	SDL_GPUTextureFormat m_gpuFmt;	// dinh dang phien ban hien tai (target: BGRA8)
	bool        m_bGpuTarget;		// phien ban hien tai co COLOR_TARGET
	bool        m_bGpuNewer;		// da ve len GPU sau lan tai CPU cuoi -> Lock phai doc lai
	bool        m_bGpuHasData;		// phien ban hien tai da co du lieu (tai len / da ve)
	bool        m_bDirty;			// CPU doi, chua tai
	RECT        m_rcDirty;
	bool        m_bLocked;
	bool        m_bLockRO;
	RECT        m_rcLock;
	bool        m_bUsedThisFrame;	// lenh ve trong khung nay da tham chieu phien ban hien tai
	UINT        m_uGpuBytes;
	CSurfGpu*   m_pSurf0;			// mat level 0 (khong giu ref; surface giu ref texture)
	int         m_nPalRow;			// hang bang mau (-1 = khong phai texture chi so)
};

// ---------------------------------------------------------------- surface
enum RgSurfKind { RGSURF_TEX = 0, RGSURF_BACKBUFFER = 1, RGSURF_OFFSCREEN = 2 };
class CSurfGpu : public IDirect3DSurface9
{
public:
	CSurfGpu(CDevGpu* pDev, RgSurfKind kind, CTexGpu* pTex, UINT w, UINT h, D3DFORMAT fmt);
	virtual ~CSurfGpu();
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj);
	virtual ULONG   __stdcall AddRef();
	virtual ULONG   __stdcall Release();
	virtual HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice);
	virtual HRESULT __stdcall SetPrivateData(REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags) { return D3D_OK; }
	virtual HRESULT __stdcall GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) { return D3DERR_NOTFOUND; }
	virtual HRESULT __stdcall FreePrivateData(REFGUID refguid) { return D3D_OK; }
	virtual DWORD   __stdcall SetPriority(DWORD PriorityNew) { return 0; }
	virtual DWORD   __stdcall GetPriority() { return 0; }
	virtual void    __stdcall PreLoad() {}
	virtual D3DRESOURCETYPE __stdcall GetType() { return D3DRTYPE_SURFACE; }
	virtual HRESULT __stdcall GetContainer(REFIID riid, void** ppContainer);
	virtual HRESULT __stdcall GetDesc(D3DSURFACE_DESC* pDesc);
	virtual HRESULT __stdcall LockRect(D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags);
	virtual HRESULT __stdcall UnlockRect();
	virtual HRESULT __stdcall GetDC(HDC* phdc) { RgStub("Surface::GetDC"); return D3DERR_INVALIDCALL; }
	virtual HRESULT __stdcall ReleaseDC(HDC hdc) { return D3DERR_INVALIDCALL; }

	LONG        m_ref;
	CDevGpu*    m_pDev;
	RgSurfKind  m_kind;
	CTexGpu*    m_pTex;				// RGSURF_TEX: giu ref
	UINT        m_w, m_h;
	D3DFORMAT   m_fmt;
	BYTE*       m_pCpu;				// RGSURF_OFFSCREEN
	UINT        m_pitch;
};

// ---------------------------------------------------------------- vertex buffer (chi CPU; Draw chep vao ring)
class CVBGpu : public IDirect3DVertexBuffer9
{
public:
	CVBGpu(CDevGpu* pDev, UINT len, DWORD usage, DWORD fvf, D3DPOOL pool);
	virtual ~CVBGpu();
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj);
	virtual ULONG   __stdcall AddRef();
	virtual ULONG   __stdcall Release();
	virtual HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice);
	virtual HRESULT __stdcall SetPrivateData(REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags) { return D3D_OK; }
	virtual HRESULT __stdcall GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) { return D3DERR_NOTFOUND; }
	virtual HRESULT __stdcall FreePrivateData(REFGUID refguid) { return D3D_OK; }
	virtual DWORD   __stdcall SetPriority(DWORD PriorityNew) { return 0; }
	virtual DWORD   __stdcall GetPriority() { return 0; }
	virtual void    __stdcall PreLoad() {}
	virtual D3DRESOURCETYPE __stdcall GetType() { return D3DRTYPE_VERTEXBUFFER; }
	virtual HRESULT __stdcall Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags);
	virtual HRESULT __stdcall Unlock() { return D3D_OK; }
	virtual HRESULT __stdcall GetDesc(D3DVERTEXBUFFER_DESC* pDesc);

	LONG    m_ref;
	CDevGpu* m_pDev;
	UINT    m_len;
	DWORD   m_usage, m_fvf;
	D3DPOOL m_pool;
	BYTE*   m_pCpu;
};

// ---------------------------------------------------------------- state block
enum { RGSB_RS = 0x00000000, RGSB_TSS = 0x01000000, RGSB_SS = 0x02000000, RGSB_KINDMASK = 0xFF000000 };
class CSBGpu : public IDirect3DStateBlock9
{
public:
	CSBGpu(CDevGpu* pDev);
	virtual ~CSBGpu();
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj);
	virtual ULONG   __stdcall AddRef();
	virtual ULONG   __stdcall Release();
	virtual HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice);
	virtual HRESULT __stdcall Capture();
	virtual HRESULT __stdcall Apply();
	void Record(DWORD key, DWORD value);

	LONG    m_ref;
	CDevGpu* m_pDev;
	std::vector<std::pair<DWORD, DWORD> > m_entries;
};

// ---------------------------------------------------------------- lenh ve trong khung
struct RgVsCb { float vp[4]; float wvp[16]; float flags[4]; };									// = VSCB trong Rep3ShadersGPU.vert
struct RgPsCb { int st0[4]; int st0b[4]; int st1[4]; int st1b[4]; float at[4]; };				// = PSCB trong Rep3ShadersGPU.frag

// trang thai da tinh cho mot lenh ve (so sanh memcmp => khoa gop lenh); moi truong dang con tro/so, khong padding rac (memset 0)
struct RgDrawState
{
	SDL_GPUGraphicsPipeline* pPipe;
	SDL_GPUSampler* pSamp[2];
	SDL_GPUTexture* pTex[2];
	UINT bScissor;
	RECT rcScissor;
	D3DVIEWPORT9 vp;
	RgVsCb vs;
	RgPsCb ps;
};

enum RgCmdType { RGCMD_DRAW = 0, RGCMD_CLEAR = 1, RGCMD_TARGET = 2 };
struct RgCmd
{
	int             type;
	RgDrawState     st;				// RGCMD_DRAW
	UINT            ringOff;		// byte dau trong ring
	UINT            nVerts;
	UINT            stride;
	D3DCOLOR        clearColor;		// RGCMD_CLEAR
	SDL_GPUTexture* pTarget;		// RGCMD_TARGET: NULL = backbuffer
};

struct RgTexUpload { SDL_GPUTexture* pTex; UINT x, y, w, h; UINT stageOff; UINT bytes; };	// tai tu staging texture cua khung

// ---------------------------------------------------------------- device
class CDevGpu : public IDirect3DDevice9
{
public:
	CDevGpu(CGpuShim* pParent, HWND hWnd, const D3DPRESENT_PARAMETERS& pp, DWORD dwBehavior);
	virtual ~CDevGpu();
	bool Init();

	// IUnknown
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj);
	virtual ULONG   __stdcall AddRef();
	virtual ULONG   __stdcall Release();
	// IDirect3DDevice9
	virtual HRESULT __stdcall TestCooperativeLevel();
	virtual UINT    __stdcall GetAvailableTextureMem();
	virtual HRESULT __stdcall EvictManagedResources() { return D3D_OK; }
	virtual HRESULT __stdcall GetDirect3D(IDirect3D9** ppD3D9);
	virtual HRESULT __stdcall GetDeviceCaps(D3DCAPS9* pCaps);
	virtual HRESULT __stdcall GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode);
	virtual HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters);
	virtual HRESULT __stdcall SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap) { return D3D_OK; }
	virtual void    __stdcall SetCursorPosition(int X, int Y, DWORD Flags) {}
	virtual BOOL    __stdcall ShowCursor(BOOL bShow) { return FALSE; }
	virtual HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain) { RgStub("CreateAdditionalSwapChain"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) { RgStub("GetSwapChain"); return D3DERR_NOTAVAILABLE; }
	virtual UINT    __stdcall GetNumberOfSwapChains() { return 1; }
	virtual HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);
	virtual HRESULT __stdcall Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
	virtual HRESULT __stdcall GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer);
	virtual HRESULT __stdcall GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus);
	virtual HRESULT __stdcall SetDialogBoxMode(BOOL bEnableDialogs) { return D3D_OK; }
	virtual void    __stdcall SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp);
	virtual void    __stdcall GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) { if (pRamp) *pRamp = m_gamma; }
	virtual HRESULT __stdcall CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle);
	virtual HRESULT __stdcall CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle) { RgStub("CreateVolumeTexture"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle) { RgStub("CreateCubeTexture"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle);
	virtual HRESULT __stdcall CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) { RgStub("CreateIndexBuffer"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) { RgStub("CreateRenderTarget"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) { RgStub("CreateDepthStencilSurface"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint);
	virtual HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture);
	virtual HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface);
	virtual HRESULT __stdcall GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface);
	virtual HRESULT __stdcall StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) { RgStub("StretchRect"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color);
	virtual HRESULT __stdcall CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
	virtual HRESULT __stdcall SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget);
	virtual HRESULT __stdcall GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget);
	virtual HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) { return D3D_OK; }
	virtual HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) { if (ppZStencilSurface) *ppZStencilSurface = NULL; return D3DERR_NOTFOUND; }
	virtual HRESULT __stdcall BeginScene() { return D3D_OK; }
	virtual HRESULT __stdcall EndScene() { return D3D_OK; }
	virtual HRESULT __stdcall Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
	virtual HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix);
	virtual HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix);
	virtual HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*) { RgStub("MultiplyTransform"); return D3D_OK; }
	virtual HRESULT __stdcall SetViewport(CONST D3DVIEWPORT9* pViewport) { if (!pViewport) return D3DERR_INVALIDCALL; m_vp = *pViewport; m_bVsDirty = true; return D3D_OK; }
	virtual HRESULT __stdcall GetViewport(D3DVIEWPORT9* pViewport) { if (!pViewport) return D3DERR_INVALIDCALL; *pViewport = m_vp; return D3D_OK; }
	virtual HRESULT __stdcall SetMaterial(CONST D3DMATERIAL9* pMaterial) { return D3D_OK; }
	virtual HRESULT __stdcall GetMaterial(D3DMATERIAL9* pMaterial) { return D3D_OK; }
	virtual HRESULT __stdcall SetLight(DWORD Index, CONST D3DLIGHT9*) { return D3D_OK; }
	virtual HRESULT __stdcall GetLight(DWORD Index, D3DLIGHT9*) { return D3D_OK; }
	virtual HRESULT __stdcall LightEnable(DWORD Index, BOOL Enable) { return D3D_OK; }
	virtual HRESULT __stdcall GetLightEnable(DWORD Index, BOOL* pEnable) { if (pEnable) *pEnable = FALSE; return D3D_OK; }
	virtual HRESULT __stdcall SetClipPlane(DWORD Index, CONST float* pPlane) { return D3D_OK; }
	virtual HRESULT __stdcall GetClipPlane(DWORD Index, float* pPlane) { return D3D_OK; }
	virtual HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
	virtual HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) { if (!pValue || (DWORD)State >= 256) return D3DERR_INVALIDCALL; *pValue = m_rs[State]; return D3D_OK; }
	virtual HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB);
	virtual HRESULT __stdcall BeginStateBlock() { if (m_pRecord) return D3DERR_INVALIDCALL; m_pRecord = new CSBGpu(this); return D3D_OK; }
	virtual HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9** ppSB) { if (!ppSB) return E_POINTER; if (!m_pRecord) return D3DERR_INVALIDCALL; *ppSB = m_pRecord; m_pRecord = NULL; return D3D_OK; }
	virtual HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus) { return D3D_OK; }
	virtual HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9* pClipStatus) { return D3D_OK; }
	virtual HRESULT __stdcall GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture);
	virtual HRESULT __stdcall SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture);
	virtual HRESULT __stdcall GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) { if (!pValue || Stage >= 8 || (DWORD)Type >= 33) return D3DERR_INVALIDCALL; *pValue = m_tss[Stage][Type]; return D3D_OK; }
	virtual HRESULT __stdcall SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
	virtual HRESULT __stdcall GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) { if (!pValue || Sampler >= 8 || (DWORD)Type >= 14) return D3DERR_INVALIDCALL; *pValue = m_ss[Sampler][Type]; return D3D_OK; }
	virtual HRESULT __stdcall SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
	virtual HRESULT __stdcall ValidateDevice(DWORD* pNumPasses) { if (pNumPasses) *pNumPasses = 1; return D3D_OK; }
	virtual HRESULT __stdcall SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries) { return D3D_OK; }
	virtual HRESULT __stdcall GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) { return D3D_OK; }
	virtual HRESULT __stdcall SetCurrentTexturePalette(UINT PaletteNumber) { return D3D_OK; }
	virtual HRESULT __stdcall GetCurrentTexturePalette(UINT* PaletteNumber) { if (PaletteNumber) *PaletteNumber = 0; return D3D_OK; }
	virtual HRESULT __stdcall SetScissorRect(CONST RECT* pRect) { if (pRect) m_scissor = *pRect; return D3D_OK; }
	virtual HRESULT __stdcall GetScissorRect(RECT* pRect) { if (pRect) *pRect = m_scissor; return D3D_OK; }
	virtual HRESULT __stdcall SetSoftwareVertexProcessing(BOOL bSoftware) { return D3D_OK; }
	virtual BOOL    __stdcall GetSoftwareVertexProcessing() { return FALSE; }
	virtual HRESULT __stdcall SetNPatchMode(float nSegments) { return D3D_OK; }
	virtual float   __stdcall GetNPatchMode() { return 0.0f; }
	virtual HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
	virtual HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) { RgStub("DrawIndexedPrimitive"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
	virtual HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) { RgStub("DrawIndexedPrimitiveUP"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) { RgStub("ProcessVertices"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) { RgStub("CreateVertexDeclaration"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) { return D3D_OK; }
	virtual HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) { if (ppDecl) *ppDecl = NULL; return D3D_OK; }
	virtual HRESULT __stdcall SetFVF(DWORD FVF) { m_fvf = FVF; return D3D_OK; }
	virtual HRESULT __stdcall GetFVF(DWORD* pFVF) { if (!pFVF) return E_POINTER; *pFVF = m_fvf; return D3D_OK; }
	virtual HRESULT __stdcall CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader) { RgStub("CreateVertexShader"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall SetVertexShader(IDirect3DVertexShader9* pShader) { return D3D_OK; }
	virtual HRESULT __stdcall GetVertexShader(IDirect3DVertexShader9** ppShader) { if (ppShader) *ppShader = NULL; return D3D_OK; }
	virtual HRESULT __stdcall SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) { return D3D_OK; }
	virtual HRESULT __stdcall GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) { return D3D_OK; }
	virtual HRESULT __stdcall SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) { return D3D_OK; }
	virtual HRESULT __stdcall GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) { return D3D_OK; }
	virtual HRESULT __stdcall SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) { return D3D_OK; }
	virtual HRESULT __stdcall GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) { return D3D_OK; }
	virtual HRESULT __stdcall SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride);
	virtual HRESULT __stdcall GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride);
	virtual HRESULT __stdcall SetStreamSourceFreq(UINT StreamNumber, UINT Setting) { return D3D_OK; }
	virtual HRESULT __stdcall GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting) { if (pSetting) *pSetting = 1; return D3D_OK; }
	virtual HRESULT __stdcall SetIndices(IDirect3DIndexBuffer9* pIndexData) { return D3D_OK; }
	virtual HRESULT __stdcall GetIndices(IDirect3DIndexBuffer9** ppIndexData) { if (ppIndexData) *ppIndexData = NULL; return D3D_OK; }
	virtual HRESULT __stdcall CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader) { RgStub("CreatePixelShader"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9* pShader) { return D3D_OK; }
	virtual HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9** ppShader) { if (ppShader) *ppShader = NULL; return D3D_OK; }
	virtual HRESULT __stdcall SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) { return D3D_OK; }
	virtual HRESULT __stdcall GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) { return D3D_OK; }
	virtual HRESULT __stdcall SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) { return D3D_OK; }
	virtual HRESULT __stdcall GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) { return D3D_OK; }
	virtual HRESULT __stdcall SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) { return D3D_OK; }
	virtual HRESULT __stdcall GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) { return D3D_OK; }
	virtual HRESULT __stdcall DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo) { RgStub("DrawRectPatch"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo) { RgStub("DrawTriPatch"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall DeletePatch(UINT Handle) { return D3D_OK; }
	virtual HRESULT __stdcall CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) { RgStub("CreateQuery"); return D3DERR_NOTAVAILABLE; }

	// ---- noi bo
	void    Lock()   { SDL_LockMutex(m_pMutex); }
	void    Unlock() { SDL_UnlockMutex(m_pMutex); }
	bool    CreateShaders();
	SDL_GPUGraphicsPipeline* GetPipeline(DWORD fvf, SDL_GPUPrimitiveType topo, SDL_GPUTextureFormat rtFmt);
	SDL_GPUSampler* GetSampler(UINT stage);
	void    ComputeState(RgDrawState& st, SDL_GPUPrimitiveType topo);
	HRESULT DrawInternal(D3DPRIMITIVETYPE type, const BYTE* pVerts, UINT nVerts, UINT stride);
	void    SetStateInternal(DWORD key, DWORD value);
	DWORD   GetStateInternal(DWORD key);
	void    FillCaps(D3DCAPS9* pCaps);
	bool    SubmitFrame(bool bPresent);						// phat lai lenh cua khung (Present hoac can doc lai)
	void    FrameReset();										// don ring/lenh/phien ban cu sau submit
	SDL_GPUTexture* CurrentTargetTex() const { return m_pRtTex ? m_pRtTex->m_pGpu : NULL; }
	SDL_GPUTextureFormat CurrentTargetFmt() const { return m_pRtTex ? SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM : m_swapFmt; }
	UINT    StageTexData(const BYTE* pData, UINT bytes);		// chep vao staging texture cua khung; tra offset
	void    QueueTexUpload(const RgTexUpload& u) { m_texUploads.push_back(u); }
	void    DeferRelease(SDL_GPUTexture* p) { if (p) m_release.push_back(p); }
	void    TouchTex(CTexGpu* p);
	bool    ReadbackTexture(SDL_GPUTexture* pTex, UINT w, UINT h, BYTE* pDst, UINT dstPitch);	// dong bo (chup man hinh)
	// bang mau
	bool    PalInit(); void PalRelease(); void PalFrameEnd(); int PalAlloc(const unsigned char* pPal24, int nColors); void PalFree(int row);

	LONG            m_ref;
	SDL_Mutex*      m_pMutex;
	CGpuShim*       m_pParent;
	HWND            m_hWnd;
	SDL_Window*     m_pWin;
	D3DPRESENT_PARAMETERS m_pp;
	DWORD           m_dwBehavior;
	SDL_GPUDevice*  m_pGpu;
	SDL_GPUTextureFormat m_swapFmt;
	UINT            m_bbW, m_bbH;
	CSurfGpu*       m_pBackSurf;
	CTexGpu*        m_pRtTex;			// render target hien tai (NULL = backbuffer); giu ref
	CSurfGpu*       m_pRtSurf;			// surface dang dat lam render target (giu ref)
	SDL_GPUTexture* m_pLastFrame;		// ban sao khung vua Present (chup man hinh)
	UINT            m_lastW, m_lastH;
	// shader / pipeline / sampler
	SDL_GPUShader*  m_pVS;
	SDL_GPUShader*  m_pFS;
	std::map<unsigned long long, SDL_GPUGraphicsPipeline*> m_pipes;
	std::map<DWORD, SDL_GPUSampler*> m_samplers;
	SDL_GPUBuffer*  m_pDummy;			// 1 dinh gia (mau trang, uv 0) theo instance
	SDL_GPUTexture* m_pWhite;			// texture 1x1 trang cho stage khong texture
	// ring dinh + lenh cua khung
	std::vector<BYTE>   m_ring;
	SDL_GPUBuffer*      m_pRingGpu; UINT m_ringGpuSize;
	SDL_GPUTransferBuffer* m_pRingXfer; UINT m_ringXferSize;
	std::vector<BYTE>   m_texStage;
	SDL_GPUTransferBuffer* m_pTexXfer; UINT m_texXferSize;
	std::vector<RgTexUpload> m_texUploads;
	std::vector<RgCmd>  m_cmds;
	std::vector<SDL_GPUTexture*> m_release;		// phien ban cu, tra sau submit
	std::vector<CTexGpu*> m_touched;				// texture co lenh ve tham chieu trong khung
	bool            m_bFrameOpen;
	// bang mau
	SDL_GPUTexture* m_pPalTex; std::vector<int> m_palFree, m_palDeferred; std::vector<std::pair<int, std::vector<DWORD> > > m_palPending;
	bool            m_bPalLinForce;
	// trang thai D3D9
	DWORD           m_rs[256];
	DWORD           m_tss[8][33];
	DWORD           m_ss[8][14];
	CTexGpu*        m_tex[8];
	DWORD           m_fvf;
	CVBGpu*         m_pStream;
	UINT            m_streamOffset, m_streamStride;
	D3DVIEWPORT9    m_vp;
	RECT            m_scissor;
	D3DMATRIX       m_matWorld, m_matView, m_matProj;
	D3DGAMMARAMP    m_gamma;
	CSBGpu*         m_pRecord;
	bool            m_bVsDirty;
	RgVsCb          m_vsCb;
	// thong ke
	unsigned        m_uFrames, m_uDrawCmds, m_uQuads, m_uUploads;
	unsigned __int64 m_uTexBytes;
};

// ---------------------------------------------------------------- IDirect3D9
class CGpuShim : public IDirect3D9
{
public:
	CGpuShim();
	virtual ~CGpuShim();
	bool Init();
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj);
	virtual ULONG   __stdcall AddRef();
	virtual ULONG   __stdcall Release();
	virtual HRESULT __stdcall RegisterSoftwareDevice(void* pInitializeFunction) { return D3DERR_NOTAVAILABLE; }
	virtual UINT    __stdcall GetAdapterCount() { return 1; }
	virtual HRESULT __stdcall GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier);
	virtual UINT    __stdcall GetAdapterModeCount(UINT Adapter, D3DFORMAT Format);
	virtual HRESULT __stdcall EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode);
	virtual HRESULT __stdcall GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode);
	virtual HRESULT __stdcall CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed);
	virtual HRESULT __stdcall CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat);
	virtual HRESULT __stdcall CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels);
	virtual HRESULT __stdcall CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) { return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) { return D3D_OK; }
	virtual HRESULT __stdcall GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps);
	virtual HMONITOR __stdcall GetAdapterMonitor(UINT Adapter);
	virtual HRESULT __stdcall CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);
	void FillCapsStatic(D3DCAPS9* pCaps, UINT uMaxTex);

	LONG m_ref;
	std::vector<D3DDISPLAYMODE> m_modes;
	D3DDISPLAYMODE m_desktop;
	UINT m_uMaxTex;
	char m_szDriver[64];
};

extern CDevGpu* g_pRep3DevGpu;

#endif // JX_PLATFORM_SDL
