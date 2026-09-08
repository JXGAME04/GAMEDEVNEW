// [D3D11 08/09] Khai bao noi bo cua lop D3D9-tren-D3D11 (xem D3D9on11.h). Khong include tu ngoai Represent3.
#ifndef __D3D9ON11I_H__
#define __D3D9ON11I_H__

#include <d3d11_1.h>
#include <dxgi1_5.h>
#include <vector>
#include <map>

class CDev11;
class CTex11;
class CSurf11;
class CAtlasPage;
class CAtlasMgr;

void R11Log(const char* fmt, ...);
void R11Stub(const char* szName);			// ghi log MOT lan moi ham chua cai

// ---------------------------------------------------------------- dinh dang
struct R11Fmt
{
	DXGI_FORMAT dxgi;		// dinh dang GPU tuong ung (UNKNOWN = khong co)
	int         bpp;		// byte moi diem (0 = nen block DXT)
	int         blockBytes;	// DXT: byte moi block 4x4
};
R11Fmt R11FormatInfo(D3DFORMAT f);
UINT   R11Pitch(D3DFORMAT f, UINT w);			// pitch CPU theo D3D9 (byte)
UINT   R11Rows(D3DFORMAT f, UINT h);			// so hang du lieu (DXT: (h+3)/4)
void   R11ConvertRowToBgra(D3DFORMAT f, const BYTE* pSrc, DWORD* pDst, UINT w);

// ---------------------------------------------------------------- atlas [D3D11 08/09 d]
class CAtlasPage
{
public:
	ID3D11Texture2D* m_pTex; ID3D11ShaderResourceView* m_pSrv;
	UINT m_binH, m_rows, m_used;
	std::vector<std::vector<std::pair<UINT, UINT> > > m_free;	// moi hang: cac doan trong (x0, x1)
};
class CAtlasMgr
{
public:
	CAtlasMgr(CDev11* pDev);
	~CAtlasMgr();
	static bool Eligible(UINT w, UINT h, DWORD usage, D3DFORMAT fmt, D3DPOOL pool);
	bool Alloc(UINT w, UINT h, CAtlasPage** ppPage, UINT* pX, UINT* pY);
	void Free(CAtlasPage* pPage, UINT x, UINT y, UINT w);
	void ReleaseAll();
	CAtlasPage* NewPage(UINT binH);
	CDev11* m_pDev; std::vector<CAtlasPage*> m_pages; UINT m_pageSize;
};

// ---------------------------------------------------------------- texture
class CTex11 : public IDirect3DTexture9
{
public:
	CTex11(CDev11* pDev, UINT w, UINT h, DWORD usage, D3DFORMAT fmt, D3DPOOL pool);
	virtual ~CTex11();

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
	bool    HasCpu() const { return m_pCpu != NULL; }
	HRESULT EnsureGpu(const BYTE* pInit);				// tao texture GPU (co du lieu khoi tao neu pInit)
	HRESULT UploadRect(const RECT* prc);				// chep vung CPU -> GPU (UpdateSubresource)
	HRESULT PrepareForBind();							// truoc khi SetTexture: tao GPU / day vung ban
	void    ReleaseGpu();
	void    AllocCpu();

	LONG        m_ref;
	CDev11*     m_pDev;
	UINT        m_w, m_h;
	DWORD       m_usage;
	D3DFORMAT   m_fmt;
	D3DPOOL     m_pool;
	UINT        m_pitch;			// pitch CPU
	BYTE*       m_pCpu;				// ban CPU (SYSTEMMEM / MANAGED / SCRATCH); DEFAULT = NULL
	ID3D11Texture2D*          m_pGpu;
	ID3D11ShaderResourceView* m_pSrv;
	ID3D11RenderTargetView*   m_pRtv;
	DXGI_FORMAT m_dxgi;				// dinh dang GPU thuc te
	bool        m_bConvert;			// GPU la BGRA8 doi tu dinh dang 16 bit / 24 bit
	bool        m_bDirty;			// MANAGED: CPU da doi, chua day len GPU
	RECT        m_rcDirty;
	bool        m_bLocked;
	RECT        m_rcLock;
	UINT        m_uGpuBytes;
	CSurf11*    m_pSurf0;			// mat level 0 (khong giu ref; surface giu ref texture)
	bool        m_bVirtual;			// [d] o trong trang atlas (m_pSrv = SRV cua trang, khong so huu)
	CAtlasPage* m_pPage; UINT m_slot, m_ax, m_ay;
};

// ---------------------------------------------------------------- surface
enum R11SurfKind { R11SURF_TEX = 0, R11SURF_BACKBUFFER = 1, R11SURF_OFFSCREEN = 2 };

class CSurf11 : public IDirect3DSurface9
{
public:
	CSurf11(CDev11* pDev, R11SurfKind kind, CTex11* pTex, UINT w, UINT h, D3DFORMAT fmt);
	virtual ~CSurf11();

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
	virtual HRESULT __stdcall GetDC(HDC* phdc) { R11Stub("Surface::GetDC"); return D3DERR_INVALIDCALL; }
	virtual HRESULT __stdcall ReleaseDC(HDC hdc) { return D3DERR_INVALIDCALL; }

	LONG        m_ref;
	CDev11*     m_pDev;
	R11SurfKind m_kind;
	CTex11*     m_pTex;				// R11SURF_TEX: giu ref
	UINT        m_w, m_h;
	D3DFORMAT   m_fmt;
	BYTE*       m_pCpu;				// R11SURF_OFFSCREEN
	UINT        m_pitch;
};

// ---------------------------------------------------------------- vertex buffer
class CVB11 : public IDirect3DVertexBuffer9
{
public:
	CVB11(CDev11* pDev, UINT len, DWORD usage, DWORD fvf, D3DPOOL pool);
	virtual ~CVB11();

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
	CDev11* m_pDev;
	UINT    m_len;
	DWORD   m_usage, m_fvf;
	D3DPOOL m_pool;
	BYTE*   m_pCpu;
};

// ---------------------------------------------------------------- state block
enum { R11SB_RS = 0x00000000, R11SB_TSS = 0x01000000, R11SB_SS = 0x02000000, R11SB_KINDMASK = 0xFF000000 };

class CSB11 : public IDirect3DStateBlock9
{
public:
	CSB11(CDev11* pDev);
	virtual ~CSB11();
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj);
	virtual ULONG   __stdcall AddRef();
	virtual ULONG   __stdcall Release();
	virtual HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice);
	virtual HRESULT __stdcall Capture();
	virtual HRESULT __stdcall Apply();

	void Record(DWORD key, DWORD value);

	LONG    m_ref;
	CDev11* m_pDev;
	std::vector<std::pair<DWORD, DWORD> > m_entries;
};

// ---------------------------------------------------------------- device
struct R11VsCb { float vp[4]; float wvp[16]; float flags[4]; };
struct R11PsCb { int st0[4]; int st0b[4]; int st1[4]; int st1b[4]; float at[4]; };
// [j] trang thai D3D11 da tinh cho mot lenh ve (so sanh memcmp => khoa gop lenh)
struct R11Applied
{
	ID3D11ShaderResourceView* srv[2];
	ID3D11BlendState* pBlend;
	ID3D11SamplerState* pSamp[2];
	ID3D11RasterizerState* pRaster;
	ID3D11InputLayout* pIL;
	UINT stride;
	UINT bScissor;
	RECT rcScissor;
	D3DVIEWPORT9 vp;
	R11VsCb vs;
	R11PsCb ps;
};

class CD3D11Shim;

class CDev11 : public IDirect3DDevice9
{
public:
	CDev11(CD3D11Shim* pParent, HWND hWnd, const D3DPRESENT_PARAMETERS& pp, DWORD dwBehavior);
	virtual ~CDev11();
	bool    Init();

	// IUnknown
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj);
	virtual ULONG   __stdcall AddRef();
	virtual ULONG   __stdcall Release();
	// IDirect3DDevice9 (thu tu y het d3d9.h)
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
	virtual HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain) { R11Stub("CreateAdditionalSwapChain"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) { R11Stub("GetSwapChain"); return D3DERR_NOTAVAILABLE; }
	virtual UINT    __stdcall GetNumberOfSwapChains() { return 1; }
	virtual HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);
	virtual HRESULT __stdcall Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
	virtual HRESULT __stdcall GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer);
	virtual HRESULT __stdcall GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus);
	virtual HRESULT __stdcall SetDialogBoxMode(BOOL bEnableDialogs) { return D3D_OK; }
	virtual void    __stdcall SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp);
	virtual void    __stdcall GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp);
	virtual HRESULT __stdcall CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle);
	virtual HRESULT __stdcall CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle) { R11Stub("CreateVolumeTexture"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle) { R11Stub("CreateCubeTexture"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle);
	virtual HRESULT __stdcall CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) { R11Stub("CreateIndexBuffer"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) { R11Stub("CreateRenderTarget"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) { R11Stub("CreateDepthStencilSurface"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint);
	virtual HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture);
	virtual HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface);
	virtual HRESULT __stdcall GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface);
	virtual HRESULT __stdcall StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) { R11Stub("StretchRect"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color);
	virtual HRESULT __stdcall CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle);
	virtual HRESULT __stdcall SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget);
	virtual HRESULT __stdcall GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget);
	virtual HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) { return D3D_OK; }
	virtual HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) { if (ppZStencilSurface) *ppZStencilSurface = NULL; return D3DERR_NOTFOUND; }
	virtual HRESULT __stdcall BeginScene();
	virtual HRESULT __stdcall EndScene();
	virtual HRESULT __stdcall Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil);
	virtual HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix);
	virtual HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix);
	virtual HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*) { R11Stub("MultiplyTransform"); return D3D_OK; }
	virtual HRESULT __stdcall SetViewport(CONST D3DVIEWPORT9* pViewport);
	virtual HRESULT __stdcall GetViewport(D3DVIEWPORT9* pViewport);
	virtual HRESULT __stdcall SetMaterial(CONST D3DMATERIAL9* pMaterial) { return D3D_OK; }
	virtual HRESULT __stdcall GetMaterial(D3DMATERIAL9* pMaterial) { return D3D_OK; }
	virtual HRESULT __stdcall SetLight(DWORD Index, CONST D3DLIGHT9*) { return D3D_OK; }
	virtual HRESULT __stdcall GetLight(DWORD Index, D3DLIGHT9*) { return D3D_OK; }
	virtual HRESULT __stdcall LightEnable(DWORD Index, BOOL Enable) { return D3D_OK; }
	virtual HRESULT __stdcall GetLightEnable(DWORD Index, BOOL* pEnable) { if (pEnable) *pEnable = FALSE; return D3D_OK; }
	virtual HRESULT __stdcall SetClipPlane(DWORD Index, CONST float* pPlane) { return D3D_OK; }
	virtual HRESULT __stdcall GetClipPlane(DWORD Index, float* pPlane) { return D3D_OK; }
	virtual HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
	virtual HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue);
	virtual HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB);
	virtual HRESULT __stdcall BeginStateBlock();
	virtual HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9** ppSB);
	virtual HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus) { return D3D_OK; }
	virtual HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9* pClipStatus) { return D3D_OK; }
	virtual HRESULT __stdcall GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture);
	virtual HRESULT __stdcall SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture);
	virtual HRESULT __stdcall GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue);
	virtual HRESULT __stdcall SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
	virtual HRESULT __stdcall GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue);
	virtual HRESULT __stdcall SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
	virtual HRESULT __stdcall ValidateDevice(DWORD* pNumPasses) { if (pNumPasses) *pNumPasses = 1; return D3D_OK; }
	virtual HRESULT __stdcall SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries) { return D3D_OK; }
	virtual HRESULT __stdcall GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) { return D3D_OK; }
	virtual HRESULT __stdcall SetCurrentTexturePalette(UINT PaletteNumber) { return D3D_OK; }
	virtual HRESULT __stdcall GetCurrentTexturePalette(UINT* PaletteNumber) { if (PaletteNumber) *PaletteNumber = 0; return D3D_OK; }
	virtual HRESULT __stdcall SetScissorRect(CONST RECT* pRect);
	virtual HRESULT __stdcall GetScissorRect(RECT* pRect);
	virtual HRESULT __stdcall SetSoftwareVertexProcessing(BOOL bSoftware) { return D3D_OK; }
	virtual BOOL    __stdcall GetSoftwareVertexProcessing() { return FALSE; }
	virtual HRESULT __stdcall SetNPatchMode(float nSegments) { return D3D_OK; }
	virtual float   __stdcall GetNPatchMode() { return 0.0f; }
	virtual HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
	virtual HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) { R11Stub("DrawIndexedPrimitive"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
	virtual HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) { R11Stub("DrawIndexedPrimitiveUP"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) { R11Stub("ProcessVertices"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) { R11Stub("CreateVertexDeclaration"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) { return D3D_OK; }
	virtual HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) { if (ppDecl) *ppDecl = NULL; return D3D_OK; }
	virtual HRESULT __stdcall SetFVF(DWORD FVF);
	virtual HRESULT __stdcall GetFVF(DWORD* pFVF);
	virtual HRESULT __stdcall CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader) { R11Stub("CreateVertexShader"); return D3DERR_NOTAVAILABLE; }
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
	virtual HRESULT __stdcall CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader) { R11Stub("CreatePixelShader"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9* pShader) { return D3D_OK; }
	virtual HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9** ppShader) { if (ppShader) *ppShader = NULL; return D3D_OK; }
	virtual HRESULT __stdcall SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) { return D3D_OK; }
	virtual HRESULT __stdcall GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) { return D3D_OK; }
	virtual HRESULT __stdcall SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) { return D3D_OK; }
	virtual HRESULT __stdcall GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) { return D3D_OK; }
	virtual HRESULT __stdcall SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) { return D3D_OK; }
	virtual HRESULT __stdcall GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) { return D3D_OK; }
	virtual HRESULT __stdcall DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo) { R11Stub("DrawRectPatch"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo) { R11Stub("DrawTriPatch"); return D3DERR_NOTAVAILABLE; }
	virtual HRESULT __stdcall DeletePatch(UINT Handle) { return D3D_OK; }
	virtual HRESULT __stdcall CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) { R11Stub("CreateQuery"); return D3DERR_NOTAVAILABLE; }

	// ---- noi bo
	void    Lock()   { EnterCriticalSection(&m_cs); }
	void    Unlock() { LeaveCriticalSection(&m_cs); }
	bool    CreateSwapChain(UINT w, UINT h, bool bWindowed);
	void    ReleaseSwapBuffers();
	bool    AcquireBackBuffer();
	bool    CreatePipelineObjects();
	void    BindRenderTarget();
	void    ApplyState();
	HRESULT DrawInternal(D3DPRIMITIVETYPE type, const BYTE* pVerts, UINT nVerts, UINT stride);
	ID3D11InputLayout*    GetInputLayout(DWORD fvf, UINT stride);
	ID3D11BlendState*     GetBlendState();
	ID3D11SamplerState*   GetSamplerState(UINT stage);
	ID3D11RasterizerState* GetRasterState();
	void    FillCaps(D3DCAPS9* pCaps);
	void    SetStateInternal(DWORD key, DWORD value);
	DWORD   GetStateInternal(DWORD key);
	bool    FormatTexOK(D3DFORMAT f, bool bRenderTarget);
	void    UpdateLastFrame();

	LONG            m_ref;
	CRITICAL_SECTION m_cs;
	CD3D11Shim*     m_pParent;
	HWND            m_hWnd;
	D3DPRESENT_PARAMETERS m_pp;
	DWORD           m_dwBehavior;
	ID3D11Device*          m_pDev;
	ID3D11DeviceContext*   m_pCtx;
	IDXGISwapChain1*       m_pSwap;
	IDXGIFactory2*         m_pFactory;
	IDXGIAdapter3*         m_pAdapter3;
	D3D_FEATURE_LEVEL      m_fl;
	bool            m_bTearing;
	UINT            m_swapFlags;
	HANDLE          m_hWaitable;			// [n] doi tuong cho khung (NULL = khong dung)
	unsigned        m_uStillLogged;
	LARGE_INTEGER   m_liLastPresent;
	ID3D11Texture2D*        m_pBackTex;
	ID3D11RenderTargetView* m_pBackRtv;
	ID3D11Texture2D*        m_pLastFrame;	// ban sao khung vua Present (chup man hinh)
	ID3D11Texture2D*        m_pStaging;		// doc lai khung ve CPU
	UINT            m_bbW, m_bbH;
	CSurf11*        m_pBackSurf;
	CSurf11*        m_pRt;					// render target hien tai (NULL = backbuffer)
	bool            m_bRtBound;
	// shader / layout / buffer
	ID3D11VertexShader*     m_pVS;
	ID3D11PixelShader*      m_pPS;
	ID3D11Buffer*           m_pVsCb;
	ID3D11Buffer*           m_pPsCb;
	ID3D11Buffer*           m_pRing;		// vertex ring (dynamic)
	UINT            m_ringSize, m_ringPos;
	bool            m_bRingDiscard;
	CAtlasMgr*      m_pAtlas;				// [d] NULL = tat			// dau khung: Map DISCARD (GPU co the con doc dinh cua khung truoc - KHONG duoc ghi de NO_OVERWRITE)
	ID3D11Buffer*           m_pDummy;		// mau trang + uv 0 cho FVF thieu thanh phan
	ID3D11DepthStencilState* m_pDss;
	std::map<DWORD, ID3D11InputLayout*>     m_layouts;
	std::map<DWORD, ID3D11BlendState*>      m_blends;
	std::map<DWORD, ID3D11SamplerState*>    m_samplers;
	std::map<DWORD, ID3D11RasterizerState*> m_rasters;
	// trang thai D3D9
	DWORD           m_rs[256];
	DWORD           m_tss[8][33];
	DWORD           m_ss[8][14];
	CTex11*         m_tex[8];
	DWORD           m_fvf;
	CVB11*          m_pStream;
	UINT            m_streamOffset, m_streamStride;
	D3DVIEWPORT9    m_vp;
	RECT            m_scissor;
	D3DMATRIX       m_matWorld, m_matView, m_matProj;
	D3DGAMMARAMP    m_gamma;
	CSB11*          m_pRecord;				// dang ghi state block
	bool            m_bVsDirty, m_bPsDirty;
	R11VsCb         m_vsCb;
	R11PsCb         m_psCb;
	bool            m_fmtOk[256];
	bool            m_fmtRtOk[256];
	bool            m_fmtChecked[256];
	// trang thai D3D11 da ap (tranh goi lai moi lan ve)
	DWORD           m_lastBlendKey, m_lastRasterKey, m_lastSampKey[2];
	ID3D11ShaderResourceView* m_lastSrv[2];
	ID3D11InputLayout* m_lastIL;
	D3D11_PRIMITIVE_TOPOLOGY m_lastTopo;
	D3DVIEWPORT9    m_lastVp;
	bool            m_bPipeBound;			// shader + CB + depth da gan sau Present/Reset
	void            ResetAppliedState();
	// [j] gop lenh ve
	R11Applied      m_lastApplied; bool m_bAppliedValid;
	std::vector<BYTE> m_batch; UINT m_batchVerts; R11Applied m_batchState;
	void            ComputeApplied(R11Applied& a, ID3D11InputLayout* pIL, UINT stride);
	void            ApplyComputed(const R11Applied& a);
	void            UploadRing(const BYTE* pData, UINT bytes, UINT stride, UINT* pPos);
	void            FlushBatch();
	void            FlushIfPending() { if (m_batchVerts) FlushBatch(); }
};

// ---------------------------------------------------------------- IDirect3D9
class CD3D11Shim : public IDirect3D9
{
public:
	CD3D11Shim();
	virtual ~CD3D11Shim();
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
	virtual HRESULT __stdcall CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) { return D3D_OK; }
	virtual HRESULT __stdcall CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) { return D3D_OK; }
	virtual HRESULT __stdcall GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps);
	virtual HMONITOR __stdcall GetAdapterMonitor(UINT Adapter);
	virtual HRESULT __stdcall CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface);

	void FillCapsStatic(D3DCAPS9* pCaps, UINT uMaxTex);

	LONG                m_ref;
	IDXGIFactory2*      m_pFactory;
	IDXGIAdapter1*      m_pAdapter;
	DXGI_ADAPTER_DESC1  m_adapterDesc;
	std::vector<D3DDISPLAYMODE> m_modes;
	D3DDISPLAYMODE      m_desktop;
	D3D_FEATURE_LEVEL   m_flProbe;
	UINT                m_uMaxTex;
};

#endif
