// [GPU 08/09] D3D9 tren SDL_GPU - thiet bi (IDirect3DDevice9): thiet bi/cua so SDL_GPU, pipeline, trang thai, gom lenh ve, Present.
#include "precompile.h"
#include "BaseInclude.h"
#ifdef JX_PLATFORM_SDL
#include "D3D9onGPU.h"
#include "D3D9onGPUi.h"
#include "Rep3ShadersGPU_spv.h"
#include <stddef.h>	// [GOP 11/09] offsetof(RgDrawState, ps)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef JX_ANDROID
// [ANDROID 11/09 HUD] so lieu cho bang do hieu nang trong game (JxPerfHudAndroid.cpp goi Rep3_ThongKeGpu qua GetProcAddress)
static char        s_szRep3GpuDriver[32] = "";
static const char* s_szRep3GpuTrinhChieu = "?";
static unsigned    s_uRep3GpuLenhVe = 0, s_uRep3GpuQuad = 0;
// [DONHIP 12/09] ban do nhip tren may that (JxPerfHudAndroid.cpp goi Rep3_DoNhipDat / Rep3_DoNhipLay qua GetProcAddress)
#ifdef JX_ANDROID
static int      s_nJxChepKhung = 0;		// [DONHIP 12/09 d] Android: KHONG chep swapchain moi khung (Fold 7: fps bang, dien -0,1..-0,8 W); GetFrontBufferData tu bat
#else
static int      s_nJxChepKhung = 1;		// 0 = bo chep swapchain moi khung (ban sao chi de chup man hinh)
#endif
static int      s_nJxKhungBay = 2;		// so khung bay dang dat (SDL mac dinh 2)
static unsigned s_aJxTraHist[400];		// cach giua hai lan tra swapchain, o 0,25 ms (0..100 ms, o cuoi = tran)
static double   s_aJxSo[8];				// xem Rep3_DoNhipLay
static Uint64   s_uJxTraTruoc = 0;
static void JxNhipGhiCho(Uint64 uT0, bool bCoSwap, Uint32 swW, Uint32 swH)
{
	const Uint64 uT1 = SDL_GetPerformanceCounter();
	const double dF = (double)SDL_GetPerformanceFrequency();
	const double dCho = (double)(uT1 - uT0) * 1000.0 / dF;
	s_aJxSo[0] += 1.0;
	if (!bCoSwap) s_aJxSo[1] += 1.0;
	s_aJxSo[2] += dCho;
	if (dCho > s_aJxSo[3]) s_aJxSo[3] = dCho;
	if (dCho > 4.0) s_aJxSo[4] += 1.0;
	if (swW && swH) { s_aJxSo[5] = (double)swW; s_aJxSo[6] = (double)swH; }
	if (!bCoSwap)
		return;
	if (s_uJxTraTruoc)
	{
		const int b = (int)((double)(uT1 - s_uJxTraTruoc) * 4000.0 / dF);
		s_aJxTraHist[b < 0 ? 0 : (b > 399 ? 399 : b)]++;
	}
	s_uJxTraTruoc = uT1;
}
#endif

#ifdef JX_ANDROID
// [VE 11/09] do tung buoc SubmitFrame tren luong ve (cho lenh + swapchain, chep len GPU, ghi lenh render pass, nop) va dem doi trang thai;
// KRepresentShell3.cpp in [VE]/[VE-GOP] moi ky va [VE-GIAT] cho khung cham. Chi cong khung co Present (bPresent).
JxVeDo g_jxVeKhung, g_jxVeTong, g_jxVeMax;
unsigned g_uJxVeKhungSo = 0, g_uJxVe8 = 0, g_uJxVe16 = 0, g_uJxGopVo[8];
static double JxVeMs(Uint64 a, Uint64 b) { return (double)(b - a) * 1000.0 / (double)SDL_GetPerformanceFrequency(); }
static void JxVeCong(const JxVeDo& k)
{
	g_jxVeKhung = k; g_uJxVeKhungSo++;
	g_jxVeTong.dCho += k.dCho; g_jxVeTong.dChep += k.dChep; g_jxVeTong.dGhi += k.dGhi; g_jxVeTong.dNop += k.dNop; g_jxVeTong.dTong += k.dTong;
	g_jxVeTong.uTai += k.uTai; g_jxVeTong.uTaiKB += k.uTaiKB; g_jxVeTong.uRingKB += k.uRingKB; g_jxVeTong.uLenh += k.uLenh; g_jxVeTong.uQuad += k.uQuad; g_jxVeTong.uDinh += k.uDinh; g_jxVeTong.uPass += k.uPass;
	g_jxVeTong.uDoiPipe += k.uDoiPipe; g_jxVeTong.uDoiTex += k.uDoiTex; g_jxVeTong.uDoiVs += k.uDoiVs; g_jxVeTong.uDoiPs += k.uDoiPs; g_jxVeTong.uDoiCat += k.uDoiCat;
	g_jxVeTong.dChepPal += k.dChepPal; g_jxVeTong.dChepTexMap += k.dChepTexMap; g_jxVeTong.dChepTexLenh += k.dChepTexLenh; g_jxVeTong.dChepZero += k.dChepZero; g_jxVeTong.dChepRing += k.dChepRing;	// [VE 11/09 d]
	g_jxVeTong.uPal += k.uPal; g_jxVeTong.uZero += k.uZero; g_jxVeTong.uXferTang += k.uXferTang; if (k.uXferKB > g_jxVeMax.uXferKB) g_jxVeMax.uXferKB = k.uXferKB;
	g_jxVeTong.dChepPalLenh += k.dChepPalLenh; if (k.dChepPalLenh > g_jxVeMax.dChepPalLenh) g_jxVeMax.dChepPalLenh = k.dChepPalLenh;	// [PALBUF 11/09]
	if (k.dChepPal > g_jxVeMax.dChepPal) g_jxVeMax.dChepPal = k.dChepPal; if (k.dChepTexMap > g_jxVeMax.dChepTexMap) g_jxVeMax.dChepTexMap = k.dChepTexMap; if (k.dChepTexLenh > g_jxVeMax.dChepTexLenh) g_jxVeMax.dChepTexLenh = k.dChepTexLenh;
	if (k.dChepZero > g_jxVeMax.dChepZero) g_jxVeMax.dChepZero = k.dChepZero; if (k.dChepRing > g_jxVeMax.dChepRing) g_jxVeMax.dChepRing = k.dChepRing;
	if (k.dCho > g_jxVeMax.dCho) g_jxVeMax.dCho = k.dCho; if (k.dChep > g_jxVeMax.dChep) g_jxVeMax.dChep = k.dChep; if (k.dGhi > g_jxVeMax.dGhi) g_jxVeMax.dGhi = k.dGhi;
	if (k.dNop > g_jxVeMax.dNop) g_jxVeMax.dNop = k.dNop; if (k.dTong > g_jxVeMax.dTong) g_jxVeMax.dTong = k.dTong;
	if (k.uTaiKB > g_jxVeMax.uTaiKB) g_jxVeMax.uTaiKB = k.uTaiKB; if (k.uRingKB > g_jxVeMax.uRingKB) g_jxVeMax.uRingKB = k.uRingKB;
	if (k.uLenh > g_jxVeMax.uLenh) g_jxVeMax.uLenh = k.uLenh; if (k.uQuad > g_jxVeMax.uQuad) g_jxVeMax.uQuad = k.uQuad; if (k.uDoiTex > g_jxVeMax.uDoiTex) g_jxVeMax.uDoiTex = k.uDoiTex;
	{ const double dViec = k.dTong - k.dCho; if (dViec > 16.0) g_uJxVe16++; else if (dViec > 8.0) g_uJxVe8++; }	// [VE 11/09 c] khong ke cho swapchain (vblank)
}
// ly do quad KHONG gop duoc vao lenh truoc, kiem theo thu tu: 0 stride khac, 1 khong lien tiep trong ring (hoac lenh truoc la clear/target),
// 2 pipeline, 3 texture 0, 4 texture 1 / sampler, 5 uniform vs, 6 uniform ps, 7 cat / viewport
static void JxGopVo(const RgCmd& L, const RgDrawState& st, UINT stride, UINT ringOff, UINT s2)
{
	int k;
	if (L.type != RGCMD_DRAW) k = 1;
	else if (L.stride != stride) k = 0;
	else if (L.ringOff + L.nVerts * s2 != ringOff) k = 1;
	else if (L.st.pPipe != st.pPipe) k = 2;
	else if (L.st.pTex[0] != st.pTex[0]) k = 3;
	else if (L.st.pTex[1] != st.pTex[1] || L.st.pSamp[0] != st.pSamp[0] || L.st.pSamp[1] != st.pSamp[1]) k = 4;
	else if (memcmp(&L.st.vs, &st.vs, sizeof(st.vs)) != 0) k = 5;
	else if (memcmp(&L.st.ps, &st.ps, sizeof(st.ps)) != 0) k = 6;
	else k = 7;
	g_uJxGopVo[k]++;
}
static int s_nJxEpTrinhChieu = 0;	// [BKG 11/09] 1 = khung ke tiep PHAI trinh chieu (be mat / cua so doi) du giong khung truoc; Present dat lai 0
// [D1 11/09] swapchain theo KHUNG LOGIC: hint JX_SWAPCHAIN_W/H cho SDL (android/va_sdl3_d1.py) = backbuffer x Rep3SwapchainLogic/100, khong vuot cua so.
// HWC/DPU phong len man (khong them pass GPU, GPU to it diem hon 3-4,4 lan); Letterbox() doc kich thuoc that tu acquire nen ti le tu ve 1.
// Bang cua so (may ao 1040x604) hoac Rep3SwapchainLogic=0 -> hint 0 = SDL nhu cu. Goi TRUOC SDL_ClaimWindowForGPUDevice va trong Reset.
static UINT s_uJxD1W = 0, s_uJxD1H = 0;	// kich thuoc swapchain nhan duoc lan gan nhat (ghi [D1] khi doi)
static void JxDatHintSwapchain(SDL_Window* pWin, UINT bbW, UINT bbH)
{
	char szW[16], szH[16];
	int pw = 0, ph = 0; if (pWin) SDL_GetWindowSizeInPixels(pWin, &pw, &ph);
	UINT w = 0, h = 0;
	if (g_nJxSwapchainLogic > 0 && bbW && bbH)
	{
		w = (UINT)((unsigned long long)bbW * (unsigned)g_nJxSwapchainLogic / 100u); h = (UINT)((unsigned long long)bbH * (unsigned)g_nJxSwapchainLogic / 100u);
		if (pw > 0 && w > (UINT)pw) w = (UINT)pw; if (ph > 0 && h > (UINT)ph) h = (UINT)ph;
		if (pw > 0 && ph > 0 && w == (UINT)pw && h == (UINT)ph) w = h = 0;	// bang cua so: khong can hint
	}
	snprintf(szW, sizeof(szW), "%u", w); snprintf(szH, sizeof(szH), "%u", h);
	SDL_SetHint("JX_SWAPCHAIN_W", szW); SDL_SetHint("JX_SWAPCHAIN_H", szH);
	RgLog("[D1] hint swapchain %ux%u (backbuffer %ux%u, cua so %dx%d px, Rep3SwapchainLogic=%d)", w, h, bbW, bbH, pw, ph, g_nJxSwapchainLogic);
}
#endif

#define RG_PAL_ROWS 8192
#define JX_PS_MAX 4096	// [GOP 11/09] so to hop trang thai tang texture toi da trong mot khung (chi so 12 bit trong o PALROW)

static SDL_GPUBlendFactor RgBlendFactor(DWORD d3d)
{
	switch (d3d)
	{
	case D3DBLEND_ZERO: return SDL_GPU_BLENDFACTOR_ZERO;
	case D3DBLEND_ONE: return SDL_GPU_BLENDFACTOR_ONE;
	case D3DBLEND_SRCCOLOR: return SDL_GPU_BLENDFACTOR_SRC_COLOR;
	case D3DBLEND_INVSRCCOLOR: return SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_COLOR;
	case D3DBLEND_SRCALPHA: return SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	case D3DBLEND_INVSRCALPHA: return SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	case D3DBLEND_DESTALPHA: return SDL_GPU_BLENDFACTOR_DST_ALPHA;
	case D3DBLEND_INVDESTALPHA: return SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_ALPHA;
	case D3DBLEND_DESTCOLOR: return SDL_GPU_BLENDFACTOR_DST_COLOR;
	case D3DBLEND_INVDESTCOLOR: return SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_COLOR;
	case D3DBLEND_SRCALPHASAT: return SDL_GPU_BLENDFACTOR_SRC_ALPHA_SATURATE;
	case D3DBLEND_BLENDFACTOR: return SDL_GPU_BLENDFACTOR_CONSTANT_COLOR;
	case D3DBLEND_INVBLENDFACTOR: return SDL_GPU_BLENDFACTOR_ONE_MINUS_CONSTANT_COLOR;
	default: return SDL_GPU_BLENDFACTOR_ONE;
	}
}
static SDL_GPUBlendOp RgBlendOp(DWORD d3d)
{
	switch (d3d)
	{
	case D3DBLENDOP_SUBTRACT: return SDL_GPU_BLENDOP_SUBTRACT;
	case D3DBLENDOP_REVSUBTRACT: return SDL_GPU_BLENDOP_REVERSE_SUBTRACT;
	case D3DBLENDOP_MIN: return SDL_GPU_BLENDOP_MIN;
	case D3DBLENDOP_MAX: return SDL_GPU_BLENDOP_MAX;
	default: return SDL_GPU_BLENDOP_ADD;
	}
}
static SDL_GPUSamplerAddressMode RgAddress(DWORD d3d)
{
	switch (d3d)
	{
	case D3DTADDRESS_MIRROR: case D3DTADDRESS_MIRRORONCE: return SDL_GPU_SAMPLERADDRESSMODE_MIRRORED_REPEAT;
	case D3DTADDRESS_CLAMP: case D3DTADDRESS_BORDER: return SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	default: return SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
	}
}
// D3D9 (hang-chinh): out = a * b
static void RgMatMul(float* out, const D3DMATRIX& a, const D3DMATRIX& b)
{
	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++)
		out[i * 4 + j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
}
static UINT RgVertexCount(D3DPRIMITIVETYPE t, UINT n)
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
// kich thuoc dinh theo FVF (Represent3: XYZRHW|DIFFUSE|TEX1 = 28, XYZ|DIFFUSE|TEX1 = 24)
static UINT RgFvfStride(DWORD fvf, UINT* pPosBytes, UINT* pColOff, UINT* pUvOff)
{
	DWORD pt = fvf & D3DFVF_POSITION_MASK;
	UINT pos = (pt == D3DFVF_XYZRHW) ? 16 : 12;
	if (pt == D3DFVF_XYZB1) pos += 4; else if (pt == D3DFVF_XYZB2) pos += 8; else if (pt == D3DFVF_XYZB3) pos += 12; else if (pt == D3DFVF_XYZB4) pos += 16; else if (pt == D3DFVF_XYZB5) pos += 20;
	UINT off = pos;
	if (fvf & D3DFVF_NORMAL) off += 12;
	if (fvf & D3DFVF_PSIZE) off += 4;
	UINT colOff = 0xFFFFFFFF, uvOff = 0xFFFFFFFF;
	if (fvf & D3DFVF_DIFFUSE) { colOff = off; off += 4; }
	if (fvf & D3DFVF_SPECULAR) off += 4;
	UINT nTex = (fvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
	if (nTex >= 1) { uvOff = off; off += 8 * nTex; }
	if (pPosBytes) *pPosBytes = pos; if (pColOff) *pColOff = colOff; if (pUvOff) *pUvOff = uvOff;
	return off;
}

// ---------------------------------------------------------------- ctor / dtor
CDevGpu::CDevGpu(CGpuShim* pParent, HWND hWnd, const D3DPRESENT_PARAMETERS& pp, DWORD dwBehavior)
{
	m_ref = 1; m_pParent = pParent; m_hWnd = hWnd; m_pWin = NULL; m_pp = pp; m_dwBehavior = dwBehavior;
	m_pMutex = SDL_CreateMutex();
	m_pGpu = NULL; m_swapFmt = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM; m_bbW = pp.BackBufferWidth; m_bbH = pp.BackBufferHeight;
	m_pBackSurf = NULL; m_pRtTex = NULL; m_pRtSurf = NULL; m_pLastFrame = NULL; m_lastW = m_lastH = 0;
	m_pVS = NULL; m_pFS = NULL; m_pDummy = NULL; m_pWhite = NULL;
	m_pRingGpu = NULL; m_ringGpuSize = 0; m_pRingXfer = NULL; m_ringXferSize = 0; m_pTexXfer = NULL; m_texXferSize = 0;
#ifdef JX_ANDROID
	m_pJxZeroXfer = NULL; m_jxZeroSize = 0; m_jxZeroDaXoa = 0;	// [VE 11/09 d]
	m_bJxCoKhungTruoc = false; m_bJxKhungCoFlush = false; m_uJxTrinhChieuLuc = 0; m_uJxGiongLienTiep = 0; m_pJxPalBuf = NULL;	// [BKG 11/09] [PALBUF 11/09]
	m_pJxPsBuf = NULL; memset(&m_jxPsCuoi, 0, sizeof(m_jxPsCuoi)); m_uJxPsCuoi = 0xFFFFFFFFu; m_uJxPsStageOff = 0xFFFFFFFFu;	// [GOP 11/09]
#endif
	m_bFrameOpen = false;
	m_pAtlas = NULL; m_uCpuBoSo = 0; m_uCpuBoThuLai = 0; m_uCpuBoBytes = 0;	// [GPU 11/09 ATLAS] [GPU 11/09 BOCPU]
	m_pPalTex = NULL; { const char* e = getenv("REP3_PALLIN"); m_bPalLinForce = (e && atoi(e) != 0); }
	memset(m_rs, 0, sizeof(m_rs)); memset(m_tss, 0, sizeof(m_tss)); memset(m_ss, 0, sizeof(m_ss)); memset(m_tex, 0, sizeof(m_tex));
	m_fvf = 0; m_pStream = NULL; m_streamOffset = 0; m_streamStride = 0;
	memset(&m_vp, 0, sizeof(m_vp)); m_vp.Width = m_bbW; m_vp.Height = m_bbH; m_vp.MaxZ = 1.0f;
	SetRect(&m_scissor, 0, 0, (int)m_bbW, (int)m_bbH);
	memset(&m_matWorld, 0, sizeof(m_matWorld)); m_matWorld._11 = m_matWorld._22 = m_matWorld._33 = m_matWorld._44 = 1.0f;
	m_matView = m_matWorld; m_matProj = m_matWorld;
	for (int i = 0; i < 256; i++) { WORD v = (WORD)(i * 257); m_gamma.red[i] = m_gamma.green[i] = m_gamma.blue[i] = v; }
	m_pRecord = NULL; m_bVsDirty = true; memset(&m_vsCb, 0, sizeof(m_vsCb));
	m_uFrames = m_uDrawCmds = m_uQuads = m_uUploads = 0; m_uTexBytes = 0;
	// mac dinh D3D9
	m_rs[D3DRS_ZENABLE] = D3DZB_FALSE; m_rs[D3DRS_FILLMODE] = D3DFILL_SOLID; m_rs[D3DRS_SHADEMODE] = D3DSHADE_GOURAUD;
	m_rs[D3DRS_ZWRITEENABLE] = TRUE; m_rs[D3DRS_ALPHATESTENABLE] = FALSE; m_rs[D3DRS_SRCBLEND] = D3DBLEND_ONE; m_rs[D3DRS_DESTBLEND] = D3DBLEND_ZERO;
	m_rs[D3DRS_CULLMODE] = D3DCULL_CCW; m_rs[D3DRS_ZFUNC] = D3DCMP_LESSEQUAL; m_rs[D3DRS_ALPHAREF] = 0; m_rs[D3DRS_ALPHAFUNC] = D3DCMP_ALWAYS;
	m_rs[D3DRS_ALPHABLENDENABLE] = FALSE; m_rs[D3DRS_BLENDOP] = D3DBLENDOP_ADD; m_rs[D3DRS_COLORWRITEENABLE] = 0xF; m_rs[D3DRS_SCISSORTESTENABLE] = FALSE;
	{ float f = 1.0f; m_rs[D3DRS_POINTSIZE] = *(DWORD*)&f; }
	for (int s = 0; s < 8; s++)
	{
		m_tss[s][D3DTSS_COLOROP] = (s == 0) ? D3DTOP_MODULATE : D3DTOP_DISABLE; m_tss[s][D3DTSS_COLORARG1] = D3DTA_TEXTURE; m_tss[s][D3DTSS_COLORARG2] = D3DTA_CURRENT;
		m_tss[s][D3DTSS_ALPHAOP] = (s == 0) ? D3DTOP_SELECTARG1 : D3DTOP_DISABLE; m_tss[s][D3DTSS_ALPHAARG1] = D3DTA_TEXTURE; m_tss[s][D3DTSS_ALPHAARG2] = D3DTA_CURRENT;
		m_ss[s][D3DSAMP_ADDRESSU] = D3DTADDRESS_WRAP; m_ss[s][D3DSAMP_ADDRESSV] = D3DTADDRESS_WRAP; m_ss[s][D3DSAMP_MAGFILTER] = D3DTEXF_POINT; m_ss[s][D3DSAMP_MINFILTER] = D3DTEXF_POINT; m_ss[s][D3DSAMP_MIPFILTER] = D3DTEXF_NONE;
	}
	pParent->AddRef();
}

CDevGpu::~CDevGpu()
{
	if (m_pGpu) SDL_WaitForGPUIdle(m_pGpu);
	FrameReset();
	if (m_pAtlas) { m_pAtlas->ReleaseAll(); delete m_pAtlas; m_pAtlas = NULL; }	// [GPU 11/09 ATLAS] (sau FrameReset: cho tra sau khung da xu ly)
	for (int s = 0; s < 8; s++) if (m_tex[s]) { m_tex[s]->Release(); m_tex[s] = NULL; }
	if (m_pStream) { m_pStream->Release(); m_pStream = NULL; }
	if (m_pRtSurf) { m_pRtSurf->Release(); m_pRtSurf = NULL; }
	if (m_pRtTex) { m_pRtTex->Release(); m_pRtTex = NULL; }
	if (m_pBackSurf) { m_pBackSurf->m_pDev = NULL; m_pBackSurf->Release(); m_pBackSurf = NULL; }
	if (m_pRecord) { m_pRecord->Release(); m_pRecord = NULL; }
	PalRelease();
	if (m_pGpu)
	{
		for (std::map<unsigned long long, SDL_GPUGraphicsPipeline*>::iterator it = m_pipes.begin(); it != m_pipes.end(); ++it) SDL_ReleaseGPUGraphicsPipeline(m_pGpu, it->second);
		for (std::map<DWORD, SDL_GPUSampler*>::iterator it = m_samplers.begin(); it != m_samplers.end(); ++it) SDL_ReleaseGPUSampler(m_pGpu, it->second);
		if (m_pLastFrame) SDL_ReleaseGPUTexture(m_pGpu, m_pLastFrame);
		if (m_pWhite) SDL_ReleaseGPUTexture(m_pGpu, m_pWhite);
		if (m_pDummy) SDL_ReleaseGPUBuffer(m_pGpu, m_pDummy);
		if (m_pRingGpu) SDL_ReleaseGPUBuffer(m_pGpu, m_pRingGpu);
		if (m_pRingXfer) SDL_ReleaseGPUTransferBuffer(m_pGpu, m_pRingXfer);
		if (m_pTexXfer) SDL_ReleaseGPUTransferBuffer(m_pGpu, m_pTexXfer);
#ifdef JX_ANDROID
		if (m_pJxZeroXfer) SDL_ReleaseGPUTransferBuffer(m_pGpu, m_pJxZeroXfer);	// [VE 11/09 d]
#endif
		if (m_pVS) SDL_ReleaseGPUShader(m_pGpu, m_pVS);
		if (m_pFS) SDL_ReleaseGPUShader(m_pGpu, m_pFS);
		for (size_t i = 0; i < m_release.size(); i++) SDL_ReleaseGPUTexture(m_pGpu, m_release[i]);
		m_release.clear();
		if (m_pWin) SDL_ReleaseWindowFromGPUDevice(m_pGpu, m_pWin);
		SDL_DestroyGPUDevice(m_pGpu); m_pGpu = NULL;
	}
	if (g_pRep3DevGpu == this) g_pRep3DevGpu = NULL;
	if (m_pMutex) SDL_DestroyMutex(m_pMutex);
	if (m_pParent) m_pParent->Release();
}

// tai mot lan (khoi tao): buffer/texture nho, dong bo
static bool RgUploadOnce(SDL_GPUDevice* dev, SDL_GPUBuffer* pBuf, SDL_GPUTexture* pTex, UINT w, UINT h, const void* pData, UINT bytes)
{
	SDL_GPUTransferBufferCreateInfo ti; memset(&ti, 0, sizeof(ti)); ti.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD; ti.size = bytes;
	SDL_GPUTransferBuffer* pX = SDL_CreateGPUTransferBuffer(dev, &ti);
	if (!pX) return false;
	void* p = SDL_MapGPUTransferBuffer(dev, pX, false);
	if (!p) { SDL_ReleaseGPUTransferBuffer(dev, pX); return false; }
	memcpy(p, pData, bytes); SDL_UnmapGPUTransferBuffer(dev, pX);
	SDL_GPUCommandBuffer* cb = SDL_AcquireGPUCommandBuffer(dev);
	if (!cb) { SDL_ReleaseGPUTransferBuffer(dev, pX); return false; }
	SDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cb);
	if (pBuf) { SDL_GPUTransferBufferLocation src = { pX, 0 }; SDL_GPUBufferRegion dst = { pBuf, 0, bytes }; SDL_UploadToGPUBuffer(cp, &src, &dst, false); }
	if (pTex) { SDL_GPUTextureTransferInfo src; memset(&src, 0, sizeof(src)); src.transfer_buffer = pX; SDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = pTex; dst.w = w; dst.h = h; dst.d = 1; SDL_UploadToGPUTexture(cp, &src, &dst, false); }
	SDL_EndGPUCopyPass(cp);
	SDL_GPUFence* f = SDL_SubmitGPUCommandBufferAndAcquireFence(cb);
	if (f) { SDL_WaitForGPUFences(dev, true, &f, 1); SDL_ReleaseGPUFence(dev, f); }
	SDL_ReleaseGPUTransferBuffer(dev, pX);
	return true;
}

bool CDevGpu::Init()
{
	// cua so SDL ung voi HWND
	int nWin = 0;
	SDL_Window** wins = SDL_GetWindows(&nWin);
	if (wins)
	{
		for (int i = 0; i < nWin && !m_pWin; i++)
		{
			HWND h = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(wins[i]), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
			if (h == m_hWnd) m_pWin = wins[i];
		}
		if (!m_pWin && nWin == 1) m_pWin = wins[0];
		SDL_free(wins);
	}
	if (!m_pWin) { RgLog("khong tim thay SDL_Window cho HWND %p (%d cua so)", (void*)m_hWnd, nWin); return false; }
	const char* e = getenv("REP3_GPU_DEBUG");
	m_pGpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, (e && atoi(e) != 0), NULL);
	if (!m_pGpu) { RgLog("SDL_CreateGPUDevice(SPIRV) that bai: %s", SDL_GetError()); return false; }
#ifdef JX_ANDROID
	JxDatHintSwapchain(m_pWin, m_bbW, m_bbH);	// [D1 11/09] truoc khi SDL tao swapchain lan dau
#endif
	if (!SDL_ClaimWindowForGPUDevice(m_pGpu, m_pWin)) { RgLog("ClaimWindowForGPUDevice that bai: %s", SDL_GetError()); return false; }
	ApplyWindowMode();
	SDL_GPUPresentMode pm = SDL_GPU_PRESENTMODE_VSYNC;
	if (m_pp.PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE && SDL_WindowSupportsGPUPresentMode(m_pGpu, m_pWin, SDL_GPU_PRESENTMODE_IMMEDIATE)) pm = SDL_GPU_PRESENTMODE_IMMEDIATE;
#ifdef JX_ANDROID
	if (pm == SDL_GPU_PRESENTMODE_VSYNC && m_pp.PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE && g_nRep3GpuMailbox && SDL_WindowSupportsGPUPresentMode(m_pGpu, m_pWin, SDL_GPU_PRESENTMODE_MAILBOX))
		pm = SDL_GPU_PRESENTMODE_MAILBOX;	// [ANDROID 11/09 MAILBOX] IMMEDIATE khong co (LDPlayer / dien thoai): MAILBOX khong cho vblank, khong xe hinh; Rep3GpuMailbox=0 de tat
#endif
	SDL_SetGPUSwapchainParameters(m_pGpu, m_pWin, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, pm);
	m_swapFmt = SDL_GetGPUSwapchainTextureFormat(m_pGpu, m_pWin);
	if (!CreateShaders()) return false;
#ifdef JX_ANDROID
	if (g_nJxPalBuffer && !PalInit()) return false;	// [PALBUF 11/09] tao storage buffer bang mau ngay: shader kieu buffer can bind no truoc lenh ve dau tien
	if (g_nJxPsBuffer)
	{	// [GOP 11/09] bang to hop trang thai tang texture (80 byte moi muc)
		SDL_GPUBufferCreateInfo bi; memset(&bi, 0, sizeof(bi)); bi.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ; bi.size = (Uint32)(JX_PS_MAX * sizeof(RgPsCb));
		m_pJxPsBuf = SDL_CreateGPUBuffer(m_pGpu, &bi);
		if (!m_pJxPsBuf) { RgLog("bang ps: CreateGPUBuffer %u KB that bai: %s", (unsigned)(JX_PS_MAX * sizeof(RgPsCb) >> 10), SDL_GetError()); return false; }
		RgLog("bang trang thai tang texture: storage buffer %d muc x %u byte (%u KB) " "[GOP 11/09]", JX_PS_MAX, (unsigned)sizeof(RgPsCb), (unsigned)(JX_PS_MAX * sizeof(RgPsCb) >> 10));
	}
#endif
	{	// dinh gia (mau trang, uv 0)
		struct { DWORD c; float u, v; float pad; } dummy = { 0xFFFFFFFF, 0.0f, 0.0f, 0.0f };
		SDL_GPUBufferCreateInfo bi; memset(&bi, 0, sizeof(bi)); bi.usage = SDL_GPU_BUFFERUSAGE_VERTEX; bi.size = sizeof(dummy);
		m_pDummy = SDL_CreateGPUBuffer(m_pGpu, &bi);
		if (!m_pDummy || !RgUploadOnce(m_pGpu, m_pDummy, NULL, 0, 0, &dummy, sizeof(dummy))) { RgLog("buffer dinh gia that bai: %s", SDL_GetError()); return false; }
	}
	{	// texture trang 1x1 (stage khong texture / bang mau chua co)
		DWORD white = 0xFFFFFFFF;
		SDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci)); ci.type = SDL_GPU_TEXTURETYPE_2D; ci.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
#ifdef JX_ANDROID
		if (g_nJxAtlasMang) ci.type = SDL_GPU_TEXTURETYPE_2D_ARRAY;	// [MANG 11/09] shader dung sampler2DArray
#endif
		ci.width = 1; ci.height = 1; ci.layer_count_or_depth = 1; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;
		m_pWhite = SDL_CreateGPUTexture(m_pGpu, &ci);
		if (!m_pWhite || !RgUploadOnce(m_pGpu, NULL, m_pWhite, 1, 1, &white, 4)) { RgLog("texture trang that bai: %s", SDL_GetError()); return false; }
	}
	m_pBackSurf = new CSurfGpu(this, RGSURF_BACKBUFFER, NULL, m_bbW, m_bbH, D3DFMT_X8R8G8B8);
	if (g_nRep3AtlasGpu) m_pAtlas = new CAtlasMgrGpu(this);	// [GPU 11/09 ATLAS]
	RgLog("atlas: %s | bo ban CPU sau khi tai len: %s", m_pAtlas ? "BAT (trang 1024x1024, texture DEFAULT <= 512 khong RT; Rep3AtlasGpu=0 de tat)" : "tat", g_nRep3GpuBoBanCpu ? "BAT (Rep3GpuBoBanCpu=0 de tat)" : "tat");
	g_pRep3DevGpu = this;
#ifdef JX_ANDROID
	{ const char* d = SDL_GetGPUDeviceDriver(m_pGpu); strncpy(s_szRep3GpuDriver, d ? d : "?", sizeof(s_szRep3GpuDriver) - 1); }	// [ANDROID 11/09 HUD]
	s_szRep3GpuTrinhChieu = pm == SDL_GPU_PRESENTMODE_IMMEDIATE ? "ngay" : (pm == SDL_GPU_PRESENTMODE_MAILBOX ? "mailbox" : "vsync");
#endif
	RgLog("thiet bi: driver %s, backbuffer %ux%u, swapchain fmt %d, trinh chieu %s, windowed=%d", SDL_GetGPUDeviceDriver(m_pGpu), m_bbW, m_bbH, (int)m_swapFmt,
		pm == SDL_GPU_PRESENTMODE_IMMEDIATE ? "ngay" : (pm == SDL_GPU_PRESENTMODE_MAILBOX ? "mailbox" : "vsync"), (int)(m_pp.Windowed != FALSE));
	return true;
}

// [GPU 08/09 khung ao] toan man hinh (desktop, khong doi che do) theo pp.Windowed; cua so theo backbuffer. Swapchain khac backbuffer -> letterbox.
void CDevGpu::ApplyWindowMode()
{
#ifdef JX_ANDROID
	// [ANDROID 09/09 DPG] Dien thoai KHONG co che do cua so: cua so da toan man hinh tu KSdlApp::Init.
	// Vi config.ini de FullScreen=0 nen ham nay tung goi SDL_SetWindowFullscreen(false), lam Android HIEN LAI
	// thanh trang thai ngay sau khung dau tien: cua so tut 1040x604 -> 1040x568, khung ve 604 bi ep xuong 568
	// (co 6 %, chu mo). Do la muc 3.1 "con lam" cua BANGIAO_ANDROID_PHA4_0809.md.
	// (Ghi chep pha 4 con canh: goi ham nay voi false sau khi be mat da co con co the lam Activity tao lai.)
	int pwA = 0, phA = 0; SDL_GetWindowSizeInPixels(m_pWin, &pwA, &phA);
	RgLog("cua so: toan man hinh (Android), %dx%d px (backbuffer %ux%u)", pwA, phA, m_bbW, m_bbH);
	return;
#else
	const bool bFull = (m_pp.Windowed == FALSE);
	if (!SDL_SetWindowFullscreen(m_pWin, bFull)) RgLog("SetWindowFullscreen(%d) that bai: %s", (int)bFull, SDL_GetError());
	if (!bFull)
	{
		int w = 0, h = 0; SDL_GetWindowSize(m_pWin, &w, &h);
		if (w != (int)m_bbW || h != (int)m_bbH)
		{
			SDL_SetWindowSize(m_pWin, (int)m_bbW, (int)m_bbH);
			SDL_SetWindowPosition(m_pWin, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		}
	}
	SDL_SyncWindow(m_pWin);
	int pw = 0, ph = 0; SDL_GetWindowSizeInPixels(m_pWin, &pw, &ph);
	RgLog("cua so: %s, %dx%d px (backbuffer %ux%u)", bFull ? "toan man hinh" : "cua so", pw, ph, m_bbW, m_bbH);
#endif
}

void CDevGpu::Letterbox(UINT swW, UINT swH, float* pScale, float* pOffX, float* pOffY)
{
	float sc = 1.0f, ox = 0.0f, oy = 0.0f;
	if (swW && swH && m_bbW && m_bbH && (swW != m_bbW || swH != m_bbH))
	{
		float sx = (float)swW / (float)m_bbW, sy = (float)swH / (float)m_bbH;
		sc = (sx < sy) ? sx : sy;
		ox = ((float)swW - (float)m_bbW * sc) * 0.5f; oy = ((float)swH - (float)m_bbH * sc) * 0.5f;
	}
	*pScale = sc; *pOffX = ox; *pOffY = oy;
}

bool CDevGpu::CreateShaders()
{
	SDL_GPUShaderCreateInfo si; memset(&si, 0, sizeof(si));
	si.code = g_Rep3GpuVS; si.code_size = sizeof(g_Rep3GpuVS); si.entrypoint = "main"; si.format = SDL_GPU_SHADERFORMAT_SPIRV; si.stage = SDL_GPU_SHADERSTAGE_VERTEX; si.num_uniform_buffers = 1;
	m_pVS = SDL_CreateGPUShader(m_pGpu, &si);
	if (!m_pVS) { RgLog("CreateGPUShader VS that bai: %s", SDL_GetError()); return false; }
	memset(&si, 0, sizeof(si));
	si.code = g_Rep3GpuFS; si.code_size = sizeof(g_Rep3GpuFS); si.entrypoint = "main"; si.format = SDL_GPU_SHADERFORMAT_SPIRV; si.stage = SDL_GPU_SHADERSTAGE_FRAGMENT; si.num_samplers = 3; si.num_uniform_buffers = 1;
#ifdef JX_ANDROID
	if (g_nJxPalBuffer)
	{	// [PALBUF 11/09] bang mau = storage buffer: shader chi co 2 sampler (t0, t1), buffer o set 2 binding 2 (SDL: sau cac sampler)
		si.code = g_Rep3GpuFSPalBuf; si.code_size = sizeof(g_Rep3GpuFSPalBuf); si.num_samplers = 2; si.num_storage_buffers = 1;
		if (g_nJxPsBuffer)
		{	// [GOP 11/09] them bang trang thai tang texture (set 2, binding 3); khong con uniform cua fragment
			si.code = g_Rep3GpuFSPalPs; si.code_size = sizeof(g_Rep3GpuFSPalPs); si.num_storage_buffers = 2; si.num_uniform_buffers = 0;
			if (g_nJxAtlasMang) { si.code = g_Rep3GpuFSPalPsMang; si.code_size = sizeof(g_Rep3GpuFSPalPsMang); }	// [MANG 11/09] sampler2DArray, lop lay tu dinh
		}
	}
#endif
	m_pFS = SDL_CreateGPUShader(m_pGpu, &si);
	if (!m_pFS) { RgLog("CreateGPUShader FS that bai: %s", SDL_GetError()); return false; }
	return true;
}

// pipeline theo (fvf, topo, blend, cull, fill, dinh dang target)
SDL_GPUGraphicsPipeline* CDevGpu::GetPipeline(DWORD fvf, SDL_GPUPrimitiveType topo, SDL_GPUTextureFormat rtFmt)
{
	UINT posBytes = 0, colOff = 0, uvOff = 0;
	const UINT stride = RgFvfStride(fvf, &posBytes, &colOff, &uvOff);
	const DWORD blendOn = m_rs[D3DRS_ALPHABLENDENABLE] ? 1 : 0;
	unsigned long long key = (unsigned long long)(fvf & 0xFFF)
		| ((unsigned long long)(topo & 7) << 12)
		| ((unsigned long long)(m_rs[D3DRS_SRCBLEND] & 31) << 16) | ((unsigned long long)(m_rs[D3DRS_DESTBLEND] & 31) << 21) | ((unsigned long long)(m_rs[D3DRS_BLENDOP] & 7) << 26)
		| ((unsigned long long)blendOn << 29) | ((unsigned long long)(m_rs[D3DRS_COLORWRITEENABLE] & 15) << 30)
		| ((unsigned long long)(m_rs[D3DRS_CULLMODE] & 3) << 34) | ((unsigned long long)(m_rs[D3DRS_FILLMODE] & 3) << 36)
		| ((unsigned long long)(rtFmt & 0xFF) << 40) | ((unsigned long long)(stride & 0xFF) << 48);
	std::map<unsigned long long, SDL_GPUGraphicsPipeline*>::iterator it = m_pipes.find(key);
	if (it != m_pipes.end()) return it->second;

	SDL_GPUVertexBufferDescription vb[2]; memset(vb, 0, sizeof(vb));
	vb[0].slot = 0; vb[0].pitch = stride + 4; vb[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
	vb[1].slot = 1; vb[1].pitch = 16; vb[1].input_rate = SDL_GPU_VERTEXINPUTRATE_INSTANCE;
	SDL_GPUVertexAttribute va[4]; memset(va, 0, sizeof(va));
	va[0].location = 0; va[0].buffer_slot = 0; va[0].format = ((fvf & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW) ? SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4 : SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3; va[0].offset = 0;
	va[1].location = 1; va[1].format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM;
	if (colOff != 0xFFFFFFFF) { va[1].buffer_slot = 0; va[1].offset = colOff; } else { va[1].buffer_slot = 1; va[1].offset = 0; }
	va[2].location = 2; va[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
	if (uvOff != 0xFFFFFFFF) { va[2].buffer_slot = 0; va[2].offset = uvOff; } else { va[2].buffer_slot = 1; va[2].offset = 4; }
	va[3].location = 3; va[3].buffer_slot = 0; va[3].format = SDL_GPU_VERTEXELEMENTFORMAT_UINT; va[3].offset = stride;

	SDL_GPUColorTargetDescription ct; memset(&ct, 0, sizeof(ct));
	ct.format = rtFmt;
	ct.blend_state.enable_blend = blendOn != 0;
	ct.blend_state.src_color_blendfactor = RgBlendFactor(m_rs[D3DRS_SRCBLEND]); ct.blend_state.dst_color_blendfactor = RgBlendFactor(m_rs[D3DRS_DESTBLEND]);
	ct.blend_state.color_blend_op = RgBlendOp(m_rs[D3DRS_BLENDOP]);
	ct.blend_state.src_alpha_blendfactor = ct.blend_state.src_color_blendfactor; ct.blend_state.dst_alpha_blendfactor = ct.blend_state.dst_color_blendfactor;
	ct.blend_state.alpha_blend_op = ct.blend_state.color_blend_op;
	ct.blend_state.enable_color_write_mask = true; ct.blend_state.color_write_mask = (SDL_GPUColorComponentFlags)(m_rs[D3DRS_COLORWRITEENABLE] & 15);

	SDL_GPUGraphicsPipelineCreateInfo pi; memset(&pi, 0, sizeof(pi));
	pi.vertex_shader = m_pVS; pi.fragment_shader = m_pFS;
	pi.vertex_input_state.vertex_buffer_descriptions = vb; pi.vertex_input_state.num_vertex_buffers = 2;
	pi.vertex_input_state.vertex_attributes = va; pi.vertex_input_state.num_vertex_attributes = 4;
	pi.primitive_type = topo;
	pi.rasterizer_state.fill_mode = (m_rs[D3DRS_FILLMODE] == D3DFILL_WIREFRAME) ? SDL_GPU_FILLMODE_LINE : SDL_GPU_FILLMODE_FILL;
	switch (m_rs[D3DRS_CULLMODE])
	{
	case D3DCULL_CW:  pi.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_BACK; pi.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE; break;
	case D3DCULL_CCW: pi.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_BACK; pi.rasterizer_state.front_face = SDL_GPU_FRONTFACE_CLOCKWISE; break;
	default:          pi.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE; pi.rasterizer_state.front_face = SDL_GPU_FRONTFACE_CLOCKWISE; break;
	}
	pi.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
	pi.target_info.color_target_descriptions = &ct; pi.target_info.num_color_targets = 1;
	SDL_GPUGraphicsPipeline* p = SDL_CreateGPUGraphicsPipeline(m_pGpu, &pi);
	if (!p) RgLog("CreateGraphicsPipeline (fvf 0x%X topo %d blend %u/%u/%u cull %u rt %d) that bai: %s", (unsigned)fvf, (int)topo, (unsigned)m_rs[D3DRS_SRCBLEND], (unsigned)m_rs[D3DRS_DESTBLEND], (unsigned)blendOn, (unsigned)m_rs[D3DRS_CULLMODE], (int)rtFmt, SDL_GetError());
	m_pipes[key] = p;	// NULL cung luu de khong thu lai moi khung
	return p;
}

SDL_GPUSampler* CDevGpu::GetSampler(UINT stage)
{
	const DWORD* ss = m_ss[stage];
	DWORD key = (ss[D3DSAMP_MAGFILTER] & 7) | ((ss[D3DSAMP_MINFILTER] & 7) << 3) | ((ss[D3DSAMP_ADDRESSU] & 7) << 6) | ((ss[D3DSAMP_ADDRESSV] & 7) << 9);
	std::map<DWORD, SDL_GPUSampler*>::iterator it = m_samplers.find(key);
	if (it != m_samplers.end()) return it->second;
	SDL_GPUSamplerCreateInfo si; memset(&si, 0, sizeof(si));
	si.min_filter = ((ss[D3DSAMP_MINFILTER] & 7) >= D3DTEXF_LINEAR) ? SDL_GPU_FILTER_LINEAR : SDL_GPU_FILTER_NEAREST;
	si.mag_filter = ((ss[D3DSAMP_MAGFILTER] & 7) >= D3DTEXF_LINEAR) ? SDL_GPU_FILTER_LINEAR : SDL_GPU_FILTER_NEAREST;
	si.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
	si.address_mode_u = RgAddress(ss[D3DSAMP_ADDRESSU]); si.address_mode_v = RgAddress(ss[D3DSAMP_ADDRESSV]); si.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
	si.max_lod = 1000.0f;
	SDL_GPUSampler* p = SDL_CreateGPUSampler(m_pGpu, &si);
	if (!p) RgLog("CreateGPUSampler that bai: %s", SDL_GetError());
	m_samplers[key] = p;
	return p;
}

// ---------------------------------------------------------------- IUnknown / thong tin
HRESULT CDevGpu::QueryInterface(REFIID riid, void** ppvObj)
{
	if (!ppvObj) return E_POINTER;
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IDirect3DDevice9)) { *ppvObj = this; AddRef(); return S_OK; }
	*ppvObj = NULL; return E_NOINTERFACE;
}
ULONG CDevGpu::AddRef() { return (ULONG)InterlockedIncrement(&m_ref); }
ULONG CDevGpu::Release() { LONG r = InterlockedDecrement(&m_ref); if (r <= 0) { delete this; return 0; } return (ULONG)r; }
HRESULT CDevGpu::TestCooperativeLevel() { return D3D_OK; }
UINT CDevGpu::GetAvailableTextureMem()
{
	unsigned __int64 budget = 1024ull << 20;
	return (UINT)((m_uTexBytes < budget) ? (budget - m_uTexBytes) : (16ull << 20));
}
HRESULT CDevGpu::GetDirect3D(IDirect3D9** ppD3D9) { if (!ppD3D9) return E_POINTER; *ppD3D9 = m_pParent; m_pParent->AddRef(); return D3D_OK; }
HRESULT CDevGpu::GetDeviceCaps(D3DCAPS9* pCaps) { if (!pCaps) return E_POINTER; FillCaps(pCaps); return D3D_OK; }
void CDevGpu::FillCaps(D3DCAPS9* pCaps) { m_pParent->FillCapsStatic(pCaps, 8192); }
HRESULT CDevGpu::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
	if (!pMode) return E_POINTER;
	pMode->Width = m_bbW; pMode->Height = m_bbH; pMode->RefreshRate = 60; pMode->Format = D3DFMT_X8R8G8B8;
	return D3D_OK;
}
HRESULT CDevGpu::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters)
{
	if (!pParameters) return E_POINTER;
	pParameters->AdapterOrdinal = 0; pParameters->DeviceType = D3DDEVTYPE_HAL; pParameters->hFocusWindow = m_hWnd; pParameters->BehaviorFlags = m_dwBehavior;
	return D3D_OK;
}
HRESULT CDevGpu::Reset(D3DPRESENT_PARAMETERS* pp)
{
	if (!pp) return D3DERR_INVALIDCALL;
	Lock();
	if (m_bFrameOpen || !m_cmds.empty()) SubmitFrame(false);
#ifdef JX_ANDROID
	m_bJxCoKhungTruoc = false; s_nJxEpTrinhChieu = 1;	// [BKG 11/09] backbuffer / swapchain doi -> khung ke tiep phai trinh chieu
#endif
	m_pp = *pp;
	if (m_pp.BackBufferWidth == 0 || m_pp.BackBufferHeight == 0) { RECT rc; GetClientRect(m_hWnd, &rc); m_pp.BackBufferWidth = rc.right - rc.left; m_pp.BackBufferHeight = rc.bottom - rc.top; }
	m_bbW = m_pp.BackBufferWidth; m_bbH = m_pp.BackBufferHeight;
	if (m_pBackSurf) { m_pBackSurf->m_w = m_bbW; m_pBackSurf->m_h = m_bbH; }
	ApplyWindowMode();
#ifdef JX_ANDROID
	JxDatHintSwapchain(m_pWin, m_bbW, m_bbH);	// [D1 11/09] backbuffer doi (gap/mo) -> SDL_SetGPUSwapchainParameters ben duoi dung lai swapchain voi hint moi
#endif
	SDL_GPUPresentMode pm = SDL_GPU_PRESENTMODE_VSYNC;
	if (m_pp.PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE && SDL_WindowSupportsGPUPresentMode(m_pGpu, m_pWin, SDL_GPU_PRESENTMODE_IMMEDIATE)) pm = SDL_GPU_PRESENTMODE_IMMEDIATE;
#ifdef JX_ANDROID
	if (pm == SDL_GPU_PRESENTMODE_VSYNC && m_pp.PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE && g_nRep3GpuMailbox && SDL_WindowSupportsGPUPresentMode(m_pGpu, m_pWin, SDL_GPU_PRESENTMODE_MAILBOX))
		pm = SDL_GPU_PRESENTMODE_MAILBOX;	// [ANDROID 11/09 MAILBOX] IMMEDIATE khong co (LDPlayer / dien thoai): MAILBOX khong cho vblank, khong xe hinh; Rep3GpuMailbox=0 de tat
#endif
	SDL_SetGPUSwapchainParameters(m_pGpu, m_pWin, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, pm);
	if (m_pLastFrame) { DeferRelease(m_pLastFrame); m_pLastFrame = NULL; m_lastW = m_lastH = 0; }
	memset(&m_vp, 0, sizeof(m_vp)); m_vp.Width = m_bbW; m_vp.Height = m_bbH; m_vp.MaxZ = 1.0f; m_bVsDirty = true;
	SetRect(&m_scissor, 0, 0, (int)m_bbW, (int)m_bbH);
#ifdef JX_ANDROID
	s_szRep3GpuTrinhChieu = pm == SDL_GPU_PRESENTMODE_IMMEDIATE ? "ngay" : (pm == SDL_GPU_PRESENTMODE_MAILBOX ? "mailbox" : "vsync");	// [ANDROID 11/09 HUD]
#endif
	RgLog("Reset: %ux%u windowed=%d vsync=%d", m_bbW, m_bbH, (int)(m_pp.Windowed != FALSE), (int)(pm == SDL_GPU_PRESENTMODE_VSYNC));
	Unlock();
	return D3D_OK;
}
HRESULT CDevGpu::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
	if (!ppBackBuffer) return E_POINTER;
	if (!m_pBackSurf) return D3DERR_INVALIDCALL;
	m_pBackSurf->AddRef(); *ppBackBuffer = m_pBackSurf;
	return D3D_OK;
}
HRESULT CDevGpu::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
	if (!pRasterStatus) return E_POINTER;
	pRasterStatus->InVBlank = FALSE; pRasterStatus->ScanLine = 0;
	return D3D_OK;
}
void CDevGpu::SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp) { if (pRamp) m_gamma = *pRamp; }

// ---------------------------------------------------------------- tai nguyen
HRESULT CDevGpu::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
	if (!ppTexture) return E_POINTER;
	*ppTexture = NULL;
	if (Width == 0 || Height == 0) return D3DERR_INVALIDCALL;
	RgFmt fi = RgFormatInfo(Format);
	if (fi.bpp == 0) { RgLog("CreateTexture: dinh dang %d khong ho tro", (int)Format); return D3DERR_NOTAVAILABLE; }
	CTexGpu* pTex = new CTexGpu(this, Width, Height, Usage, Format, Pool);
	if (m_pAtlas && CAtlasMgrGpu::Eligible(Width, Height, Usage, Format, Pool)) pTex->m_bVirtual = true;	// [GPU 11/09 ATLAS]
	if (!(Usage & D3DUSAGE_RENDERTARGET) && !pTex->m_pCpu) { pTex->Release(); return E_OUTOFMEMORY; }
	*ppTexture = pTex;
	return D3D_OK;
}
HRESULT CDevGpu::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
	if (!ppVertexBuffer) return E_POINTER;
	CVBGpu* p = new CVBGpu(this, Length, Usage, FVF, Pool);
	if (!p->m_pCpu) { p->Release(); return E_OUTOFMEMORY; }
	*ppVertexBuffer = p;
	return D3D_OK;
}
HRESULT CDevGpu::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
	if (!ppSurface) return E_POINTER;
	CSurfGpu* p = new CSurfGpu(this, RGSURF_OFFSCREEN, NULL, Width, Height, Format);
	if (!p->m_pCpu) { p->Release(); return E_OUTOFMEMORY; }
	*ppSurface = p;
	return D3D_OK;
}

// chep CPU giua hai texture (nhu D3D9on11)
HRESULT CDevGpu::UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint)
{
	CSurfGpu* pS = (CSurfGpu*)pSourceSurface; CSurfGpu* pD = (CSurfGpu*)pDestinationSurface;
	if (!pS || !pD) return D3DERR_INVALIDCALL;
	D3DLOCKED_RECT ls, ld;
	RECT rs; if (pSourceRect) rs = *pSourceRect; else SetRect(&rs, 0, 0, (int)pS->m_w, (int)pS->m_h);
	POINT pt = { 0, 0 }; if (pDestPoint) pt = *pDestPoint;
	RECT rd; SetRect(&rd, pt.x, pt.y, pt.x + (rs.right - rs.left), pt.y + (rs.bottom - rs.top));
	if (FAILED(pS->LockRect(&ls, &rs, D3DLOCK_READONLY))) return D3DERR_INVALIDCALL;
	if (FAILED(pD->LockRect(&ld, &rd, 0))) { pS->UnlockRect(); return D3DERR_INVALIDCALL; }
	RgFmt fs = RgFormatInfo(pS->m_fmt), fd = RgFormatInfo(pD->m_fmt);
	const UINT w = (UINT)(rs.right - rs.left), h = (UINT)(rs.bottom - rs.top);
	for (UINT y = 0; y < h; y++)
	{
		const BYTE* src = (const BYTE*)ls.pBits + (size_t)y * ls.Pitch; BYTE* dst = (BYTE*)ld.pBits + (size_t)y * ld.Pitch;
		if (pS->m_fmt == pD->m_fmt) memcpy(dst, src, (size_t)w * fs.bpp);
		else { std::vector<DWORD> tmp(w); RgConvertRowToBgra(pS->m_fmt, src, &tmp[0], w); extern void RgConvertRowFromBgra(D3DFORMAT, const DWORD*, BYTE*, UINT); RgConvertRowFromBgra(pD->m_fmt, &tmp[0], dst, w); }
	}
	pD->UnlockRect(); pS->UnlockRect();
	return D3D_OK;
}
HRESULT CDevGpu::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
	CTexGpu* pS = (CTexGpu*)pSourceTexture; CTexGpu* pD = (CTexGpu*)pDestinationTexture;
	if (!pS || !pD) return D3DERR_INVALIDCALL;
	IDirect3DSurface9* s = NULL; IDirect3DSurface9* d = NULL;
	pS->GetSurfaceLevel(0, &s); pD->GetSurfaceLevel(0, &d);
	HRESULT hr = UpdateSurface(s, NULL, d, NULL);
	if (s) s->Release(); if (d) d->Release();
	return hr;
}
HRESULT CDevGpu::ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
{
	CSurfGpu* p = (CSurfGpu*)pSurface;
	if (!p) return D3DERR_INVALIDCALL;
	if (p->m_kind == RGSURF_BACKBUFFER) return Clear(0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0);
	D3DLOCKED_RECT lr; RECT rc; if (pRect) rc = *pRect; else SetRect(&rc, 0, 0, (int)p->m_w, (int)p->m_h);
	if (FAILED(p->LockRect(&lr, &rc, 0))) return D3DERR_INVALIDCALL;
	RgFmt fi = RgFormatInfo(p->m_fmt);
	const UINT w = (UINT)(rc.right - rc.left), h = (UINT)(rc.bottom - rc.top);
	std::vector<DWORD> row(w, (DWORD)color); std::vector<BYTE> conv((size_t)w * (fi.bpp ? fi.bpp : 4));
	extern void RgConvertRowFromBgra(D3DFORMAT, const DWORD*, BYTE*, UINT);
	RgConvertRowFromBgra(p->m_fmt, &row[0], &conv[0], w);
	for (UINT y = 0; y < h; y++) memcpy((BYTE*)lr.pBits + (size_t)y * lr.Pitch, &conv[0], conv.size());
	p->UnlockRect();
	return D3D_OK;
}

// doc lai texture GPU ve CPU (dong bo) - phat lenh dang cho truoc
bool CDevGpu::ReadbackTexture(SDL_GPUTexture* pTex, UINT w, UINT h, BYTE* pDst, UINT dstPitch)
{
	if (!pTex || !pDst) return false;
	if (!m_cmds.empty() || !m_texUploads.empty()) SubmitFrame(false);
	const UINT bytes = w * h * 4;
	SDL_GPUTransferBufferCreateInfo ti; memset(&ti, 0, sizeof(ti)); ti.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD; ti.size = bytes;
	SDL_GPUTransferBuffer* pX = SDL_CreateGPUTransferBuffer(m_pGpu, &ti);
	if (!pX) return false;
	SDL_GPUCommandBuffer* cb = SDL_AcquireGPUCommandBuffer(m_pGpu);
	if (!cb) { SDL_ReleaseGPUTransferBuffer(m_pGpu, pX); return false; }
	SDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cb);
	SDL_GPUTextureRegion src; memset(&src, 0, sizeof(src)); src.texture = pTex; src.w = w; src.h = h; src.d = 1;
	SDL_GPUTextureTransferInfo dst; memset(&dst, 0, sizeof(dst)); dst.transfer_buffer = pX; dst.pixels_per_row = w; dst.rows_per_layer = h;
	SDL_DownloadFromGPUTexture(cp, &src, &dst);
	SDL_EndGPUCopyPass(cp);
	SDL_GPUFence* f = SDL_SubmitGPUCommandBufferAndAcquireFence(cb);
	if (f) { SDL_WaitForGPUFences(m_pGpu, true, &f, 1); SDL_ReleaseGPUFence(m_pGpu, f); }
	bool ok = false;
	const BYTE* p = (const BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, pX, false);
	if (p) { for (UINT y = 0; y < h; y++) memcpy(pDst + (size_t)y * dstPitch, p + (size_t)y * w * 4, (size_t)w * 4); SDL_UnmapGPUTransferBuffer(m_pGpu, pX); ok = true; }
	SDL_ReleaseGPUTransferBuffer(m_pGpu, pX);
	return ok;
}

HRESULT CDevGpu::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
	CSurfGpu* pRt = (CSurfGpu*)pRenderTarget; CSurfGpu* pD = (CSurfGpu*)pDestSurface;
	if (!pRt || !pD) return D3DERR_INVALIDCALL;
	if (pRt->m_kind == RGSURF_BACKBUFFER) return GetFrontBufferData(0, pDestSurface);
	if (pRt->m_kind != RGSURF_TEX || !pRt->m_pTex) return D3DERR_INVALIDCALL;
	CTexGpu* pT = pRt->m_pTex;
	if (!pT->m_pGpu) return D3DERR_INVALIDCALL;
	std::vector<BYTE> tmp((size_t)pT->m_w * pT->m_h * 4);
	if (pT->m_gpuFmt != SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM) { RgStub("GetRenderTargetData(fmt != BGRA8)"); return D3DERR_INVALIDCALL; }
	if (!ReadbackTexture(pT->m_pGpu, pT->m_w, pT->m_h, &tmp[0], pT->m_w * 4)) return D3DERR_INVALIDCALL;
	D3DLOCKED_RECT lr; if (FAILED(pD->LockRect(&lr, NULL, 0))) return D3DERR_INVALIDCALL;
	const UINT w = (pT->m_w < pD->m_w) ? pT->m_w : pD->m_w, h = (pT->m_h < pD->m_h) ? pT->m_h : pD->m_h;
	extern void RgConvertRowFromBgra(D3DFORMAT, const DWORD*, BYTE*, UINT);
	for (UINT y = 0; y < h; y++) RgConvertRowFromBgra(pD->m_fmt, (const DWORD*)&tmp[(size_t)y * pT->m_w * 4], (BYTE*)lr.pBits + (size_t)y * lr.Pitch, w);
	pD->UnlockRect();
	return D3D_OK;
}

HRESULT CDevGpu::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
	CSurfGpu* pD = (CSurfGpu*)pDestSurface;
	if (!pD) return D3DERR_INVALIDCALL;
	if (!m_pLastFrame)
	{
#ifdef JX_ANDROID
		if (!s_nJxChepKhung) { s_nJxChepKhung = 1; RgLog("GetFrontBufferData: bat chep khung tu khung sau (Android mac dinh khong chep)"); }	// [DONHIP 12/09 d]
#endif
		RgLog("GetFrontBufferData: chua co khung nao (chup lai o khung sau)"); return D3DERR_INVALIDCALL;
	}
	std::vector<BYTE> tmp((size_t)m_lastW * m_lastH * 4);
	if (!ReadbackTexture(m_pLastFrame, m_lastW, m_lastH, &tmp[0], m_lastW * 4)) return D3DERR_INVALIDCALL;
	D3DLOCKED_RECT lr; if (FAILED(pD->LockRect(&lr, NULL, 0))) return D3DERR_INVALIDCALL;
	const UINT w = (m_lastW < pD->m_w) ? m_lastW : pD->m_w, h = (m_lastH < pD->m_h) ? m_lastH : pD->m_h;
	extern void RgConvertRowFromBgra(D3DFORMAT, const DWORD*, BYTE*, UINT);
	for (UINT y = 0; y < h; y++) RgConvertRowFromBgra(pD->m_fmt, (const DWORD*)&tmp[(size_t)y * m_lastW * 4], (BYTE*)lr.pBits + (size_t)y * lr.Pitch, w);
	pD->UnlockRect();
	return D3D_OK;
}

// ---------------------------------------------------------------- render target / clear
HRESULT CDevGpu::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	if (RenderTargetIndex != 0) return D3DERR_INVALIDCALL;
	CSurfGpu* p = (CSurfGpu*)pRenderTarget;
	Lock();
	CTexGpu* pNewTex = NULL; SDL_GPUTexture* pTarget = NULL;
	if (p && p->m_kind == RGSURF_TEX && p->m_pTex)
	{
		pNewTex = p->m_pTex;
		pTarget = pNewTex->PrepareAsTarget();
		if (!pTarget) { Unlock(); return D3DERR_INVALIDCALL; }
	}
	else if (p && p->m_kind == RGSURF_OFFSCREEN) { Unlock(); RgStub("SetRenderTarget(offscreen)"); return D3DERR_INVALIDCALL; }
	if (pNewTex) pNewTex->AddRef();
	if (m_pRtTex) m_pRtTex->Release();
	m_pRtTex = pNewTex;
	if (p) p->AddRef();
	if (m_pRtSurf) m_pRtSurf->Release();
	m_pRtSurf = (p && p->m_kind == RGSURF_TEX) ? p : NULL;
	RgCmd c; memset(&c, 0, sizeof(c)); c.type = RGCMD_TARGET; c.pTarget = pTarget;
	m_cmds.push_back(c);
	// D3D9: doi render target -> viewport = ca target
	memset(&m_vp, 0, sizeof(m_vp)); m_vp.Width = pNewTex ? pNewTex->m_w : m_bbW; m_vp.Height = pNewTex ? pNewTex->m_h : m_bbH; m_vp.MaxZ = 1.0f; m_bVsDirty = true;
	Unlock();
	return D3D_OK;
}
HRESULT CDevGpu::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
	if (!ppRenderTarget) return E_POINTER;
	if (RenderTargetIndex != 0) return D3DERR_INVALIDCALL;
	CSurfGpu* p = m_pRtSurf ? m_pRtSurf : m_pBackSurf;
	if (!p) return D3DERR_INVALIDCALL;
	p->AddRef(); *ppRenderTarget = p;
	return D3D_OK;
}
HRESULT CDevGpu::Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	if (!(Flags & D3DCLEAR_TARGET)) return D3D_OK;
	Lock();
	if (!m_cmds.empty() && m_cmds.back().type == RGCMD_CLEAR) m_cmds.back().clearColor = Color;
	else { RgCmd c; memset(&c, 0, sizeof(c)); c.type = RGCMD_CLEAR; c.clearColor = Color; m_cmds.push_back(c); }
	Unlock();
	return D3D_OK;
}

// ---------------------------------------------------------------- trang thai
HRESULT CDevGpu::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	if (!pMatrix) return D3DERR_INVALIDCALL;
	if (State == D3DTS_WORLD) m_matWorld = *pMatrix; else if (State == D3DTS_VIEW) m_matView = *pMatrix; else if (State == D3DTS_PROJECTION) m_matProj = *pMatrix; else return D3D_OK;
	m_bVsDirty = true;
	return D3D_OK;
}
HRESULT CDevGpu::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
	if (!pMatrix) return D3DERR_INVALIDCALL;
	if (State == D3DTS_WORLD) *pMatrix = m_matWorld; else if (State == D3DTS_VIEW) *pMatrix = m_matView; else if (State == D3DTS_PROJECTION) *pMatrix = m_matProj; else memset(pMatrix, 0, sizeof(*pMatrix));
	return D3D_OK;
}
void CDevGpu::SetStateInternal(DWORD key, DWORD value)
{
	DWORD kind = key & RGSB_KINDMASK; DWORD stage = (key >> 16) & 0xFF; DWORD type = key & 0xFFFF;
	if (kind == RGSB_RS) { if (type < 256) m_rs[type] = value; }
	else if (kind == RGSB_TSS) { if (stage < 8 && type < 33) m_tss[stage][type] = value; }
	else if (kind == RGSB_SS) { if (stage < 8 && type < 14) m_ss[stage][type] = value; }
}
DWORD CDevGpu::GetStateInternal(DWORD key)
{
	DWORD kind = key & RGSB_KINDMASK; DWORD stage = (key >> 16) & 0xFF; DWORD type = key & 0xFFFF;
	if (kind == RGSB_RS) return (type < 256) ? m_rs[type] : 0;
	if (kind == RGSB_TSS) return (stage < 8 && type < 33) ? m_tss[stage][type] : 0;
	if (kind == RGSB_SS) return (stage < 8 && type < 14) ? m_ss[stage][type] : 0;
	return 0;
}
HRESULT CDevGpu::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	if ((DWORD)State >= 256) return D3DERR_INVALIDCALL;
	if (m_pRecord) { m_pRecord->Record(RGSB_RS | (DWORD)State, Value); return D3D_OK; }
	m_rs[State] = Value;
	return D3D_OK;
}
HRESULT CDevGpu::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	if (Stage >= 8 || (DWORD)Type >= 33) return D3DERR_INVALIDCALL;
	if (m_pRecord) { m_pRecord->Record(RGSB_TSS | (Stage << 16) | (DWORD)Type, Value); return D3D_OK; }
	m_tss[Stage][Type] = Value;
	return D3D_OK;
}
HRESULT CDevGpu::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
	if (Sampler >= 8 || (DWORD)Type >= 14) return D3DERR_INVALIDCALL;
	if (m_pRecord) { m_pRecord->Record(RGSB_SS | (Sampler << 16) | (DWORD)Type, Value); return D3D_OK; }
	m_ss[Sampler][Type] = Value;
	return D3D_OK;
}
HRESULT CDevGpu::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
	if (!ppSB) return E_POINTER;
	CSBGpu* p = new CSBGpu(this);
	for (DWORD i = 0; i < 256; i++) p->Record(RGSB_RS | i, m_rs[i]);
	for (DWORD s = 0; s < 8; s++) { for (DWORD t = 1; t < 33; t++) p->Record(RGSB_TSS | (s << 16) | t, m_tss[s][t]); for (DWORD t = 1; t < 14; t++) p->Record(RGSB_SS | (s << 16) | t, m_ss[s][t]); }
	*ppSB = p;
	return D3D_OK;
}
HRESULT CDevGpu::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
	if (Stage >= 8) return D3DERR_INVALIDCALL;
	CTexGpu* p = (CTexGpu*)pTexture;
	if (m_tex[Stage] == p) return D3D_OK;
	if (p) p->AddRef();
	if (m_tex[Stage]) m_tex[Stage]->Release();
	m_tex[Stage] = p;
	return D3D_OK;
}
HRESULT CDevGpu::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{
	if (!ppTexture || Stage >= 8) return D3DERR_INVALIDCALL;
	*ppTexture = m_tex[Stage]; if (m_tex[Stage]) m_tex[Stage]->AddRef();
	return D3D_OK;
}
HRESULT CDevGpu::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
	if (StreamNumber != 0) return D3DERR_INVALIDCALL;
	CVBGpu* p = (CVBGpu*)pStreamData;
	if (p) p->AddRef();
	if (m_pStream) m_pStream->Release();
	m_pStream = p; m_streamOffset = OffsetInBytes; m_streamStride = Stride;
	return D3D_OK;
}
HRESULT CDevGpu::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride)
{
	if (StreamNumber != 0 || !ppStreamData) return D3DERR_INVALIDCALL;
	*ppStreamData = m_pStream; if (m_pStream) m_pStream->AddRef();
	if (pOffsetInBytes) *pOffsetInBytes = m_streamOffset; if (pStride) *pStride = m_streamStride;
	return D3D_OK;
}

void CDevGpu::TouchTex(CTexGpu* p)
{
	if (!p->m_bUsedThisFrame) m_touched.push_back(p);
}

// [GPU 11/09 ATLAS] texture bi huy giua khung (dang trong m_touched) -> rut ra; lenh ve da ghi van giu SDL_GPUTexture (tra sau khung)
void CDevGpu::UntouchTex(CTexGpu* p)
{
	for (size_t i = m_touched.size(); i > 0; i--)
		if (m_touched[i - 1] == p) { m_touched.erase(m_touched.begin() + (i - 1)); return; }
}

// [GPU 11/09 ATLAS] ghi lenh tai mot vung toan 0 (trang moi, o chua co du lieu CPU)
void CDevGpu::QueueZeroUpload(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, UINT layer)
{
	if (!pTex || !w || !h || !bpp) return;
#ifdef JX_ANDROID
	{	// [VE 11/09 d] tai tu bo dem 0 co dinh (SubmitFrame): khong memset/memcpy vao staging, staging khong phinh 2-4 MB moi trang atlas moi
		RgTexUpload u = { pTex, x, y, w, h, 0, w * h * bpp, layer };	// [MANG 11/09]
		m_jxZeroUploads.push_back(u);
		return;
	}
#endif
	const UINT bytes = w * h * bpp;
	const UINT off = ((UINT)m_texStage.size() + 15) & ~15u;
	m_texStage.resize((size_t)off + bytes, 0);
	RgTexUpload u = { pTex, x, y, w, h, off, bytes, layer };
	m_texUploads.push_back(u);
}

// [GPU 11/09 BOCPU] doc lai mot vung texture GPU ve CPU (dong bo): nhu ReadbackTexture nhung co goc (x, y) va byte/diem
bool CDevGpu::ReadbackRegion(SDL_GPUTexture* pTex, UINT x, UINT y, UINT w, UINT h, UINT bpp, BYTE* pDst, UINT dstPitch, UINT layer)
{
	if (!pTex || !pDst || !w || !h || !bpp) return false;
	if (!m_cmds.empty() || !m_texUploads.empty()) SubmitFrame(false);
	const UINT bytes = w * h * bpp;
	SDL_GPUTransferBufferCreateInfo ti; memset(&ti, 0, sizeof(ti)); ti.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD; ti.size = bytes;
	SDL_GPUTransferBuffer* pX = SDL_CreateGPUTransferBuffer(m_pGpu, &ti);
	if (!pX) return false;
	SDL_GPUCommandBuffer* cb = SDL_AcquireGPUCommandBuffer(m_pGpu);
	if (!cb) { SDL_ReleaseGPUTransferBuffer(m_pGpu, pX); return false; }
	SDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cb);
	SDL_GPUTextureRegion src; memset(&src, 0, sizeof(src)); src.texture = pTex; src.layer = layer; src.x = x; src.y = y; src.w = w; src.h = h; src.d = 1;	// [MANG 11/09] lop
	SDL_GPUTextureTransferInfo dst; memset(&dst, 0, sizeof(dst)); dst.transfer_buffer = pX; dst.pixels_per_row = w; dst.rows_per_layer = h;
	SDL_DownloadFromGPUTexture(cp, &src, &dst);
	SDL_EndGPUCopyPass(cp);
	SDL_GPUFence* f = SDL_SubmitGPUCommandBufferAndAcquireFence(cb);
	if (f) { SDL_WaitForGPUFences(m_pGpu, true, &f, 1); SDL_ReleaseGPUFence(m_pGpu, f); }
	bool ok = false;
	const BYTE* p = (const BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, pX, false);
	if (p) { for (UINT r = 0; r < h; r++) memcpy(pDst + (size_t)r * dstPitch, p + (size_t)r * w * bpp, (size_t)w * bpp); SDL_UnmapGPUTransferBuffer(m_pGpu, pX); ok = true; }
	SDL_ReleaseGPUTransferBuffer(m_pGpu, pX);
	return ok;
}

// trang thai SDL_GPU + uniform cho lenh ve hien tai
void CDevGpu::ComputeState(RgDrawState& st, SDL_GPUPrimitiveType topo)
{
	memset(&st, 0, sizeof(st));
	st.pPipe = GetPipeline(m_fvf, topo, CurrentTargetFmt());
	st.pSamp[0] = GetSampler(0); st.pSamp[1] = GetSampler(1);
	bool bound[2] = { false, false };
	for (int s = 0; s < 2; s++)
	{
		SDL_GPUTexture* t = NULL;
#ifdef JX_ANDROID
		// [MANG 11/09] o PALROW chi cho MOT chi so lop (dung cho tang 0): texture cua tang 1 phai ra khoi atlas de lop cua no luon = 0
		if (g_nJxAtlasMang && s == 1 && m_tex[1] && m_tex[1] != m_pRtTex && m_tex[1]->m_bVirtual) m_tex[1]->BoAtlas();
#endif
		if (m_tex[s] && m_tex[s] != m_pRtTex) t = m_tex[s]->PrepareForBind();
		if (t) bound[s] = true;
		st.pTex[s] = t ? t : m_pWhite;
	}
	st.bScissor = m_rs[D3DRS_SCISSORTESTENABLE] ? 1 : 0; st.rcScissor = m_scissor;
	st.vp = m_vp;
	const bool bRhw = ((m_fvf & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW);
	if (m_bVsDirty || (m_vsCb.flags[0] > 0.5f) != bRhw)
	{
		m_vsCb.vp[0] = (float)(m_vp.Width ? m_vp.Width : 1); m_vsCb.vp[1] = (float)(m_vp.Height ? m_vp.Height : 1); m_vsCb.vp[2] = (float)m_vp.X; m_vsCb.vp[3] = (float)m_vp.Y;
		D3DMATRIX wv; float t[16]; RgMatMul(t, m_matWorld, m_matView); memcpy(&wv, t, sizeof(t)); RgMatMul(m_vsCb.wvp, wv, m_matProj);
		m_vsCb.flags[0] = bRhw ? 1.0f : 0.0f;
		m_bVsDirty = false;
	}
	{ DWORD ps = m_rs[D3DRS_POINTSIZE]; float f = *(float*)&ps; m_vsCb.flags[1] = (f > 0.0f && f < 256.0f) ? f : 1.0f; }
	st.vs = m_vsCb;
	RgPsCb& cb = st.ps;
	cb.st0[0] = (int)m_tss[0][D3DTSS_COLOROP]; cb.st0[1] = (int)m_tss[0][D3DTSS_COLORARG1]; cb.st0[2] = (int)m_tss[0][D3DTSS_COLORARG2]; cb.st0[3] = (int)m_tss[0][D3DTSS_ALPHAOP];
	cb.st0b[0] = (int)m_tss[0][D3DTSS_ALPHAARG1]; cb.st0b[1] = (int)m_tss[0][D3DTSS_ALPHAARG2]; cb.st0b[2] = bound[0] ? 1 : 0;
	cb.st0b[3] = (m_bPalLinForce || (m_ss[0][D3DSAMP_MAGFILTER] & 7) >= D3DTEXF_LINEAR || (m_ss[0][D3DSAMP_MINFILTER] & 7) >= D3DTEXF_LINEAR) ? 1 : 0;
	cb.st1[0] = (int)m_tss[1][D3DTSS_COLOROP]; cb.st1[1] = (int)m_tss[1][D3DTSS_COLORARG1]; cb.st1[2] = (int)m_tss[1][D3DTSS_COLORARG2]; cb.st1[3] = (int)m_tss[1][D3DTSS_ALPHAOP];
	cb.st1b[0] = (int)m_tss[1][D3DTSS_ALPHAARG1]; cb.st1b[1] = (int)m_tss[1][D3DTSS_ALPHAARG2]; cb.st1b[2] = bound[1] ? 1 : 0; cb.st1b[3] = 0;
	cb.at[0] = m_rs[D3DRS_ALPHATESTENABLE] ? 1.0f : 0.0f; cb.at[1] = (float)(m_rs[D3DRS_ALPHAFUNC] & 15); cb.at[2] = (float)(m_rs[D3DRS_ALPHAREF] & 255); cb.at[3] = 0.0f;
}

// ---------------------------------------------------------------- ve
// [GPU 11/09 ATLAS] uv cua texture ao (stage 0) -> uv trong trang atlas, sua tai cho tren dinh da chep vao ring (nhu R11AtlasUv)
static void RgAtlasUv(BYTE* pV, UINT nVerts, UINT strideRing, DWORD fvf, CTexGpu* pTex, float fPage)
{
	if (!pTex || !pTex->m_bVirtual || !pTex->m_pPage) return;
	UINT uvOff = 0xFFFFFFFF; RgFvfStride(fvf, NULL, NULL, &uvOff);
	if (uvOff == 0xFFFFFFFF) return;
	const float sx = (float)pTex->m_w / fPage, sy = (float)pTex->m_h / fPage, ox = (float)pTex->m_ax / fPage, oy = (float)pTex->m_ay / fPage;
	for (UINT i = 0; i < nVerts; i++) { float* uv = (float*)(pV + i * strideRing + uvOff); uv[0] = uv[0] * sx + ox; uv[1] = uv[1] * sy + oy; }
}

HRESULT CDevGpu::DrawInternal(D3DPRIMITIVETYPE type, const BYTE* pVerts, UINT nVerts, UINT stride)
{
	if (!pVerts || nVerts == 0 || stride == 0) return D3DERR_INVALIDCALL;
	const UINT s2 = stride + 4;	// + PALROW
	UINT uPal = (m_tex[0] && m_tex[0]->m_nPalRow >= 0) ? (UINT)m_tex[0]->m_nPalRow : 0xFFFFu;
	SDL_GPUPrimitiveType topo = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
	if (type == D3DPT_POINTLIST) topo = SDL_GPU_PRIMITIVETYPE_POINTLIST;
	else if (type == D3DPT_LINELIST) topo = SDL_GPU_PRIMITIVETYPE_LINELIST;
	else if (type == D3DPT_LINESTRIP) topo = SDL_GPU_PRIMITIVETYPE_LINESTRIP;
	else if (type == D3DPT_TRIANGLESTRIP && nVerts != 4) topo = SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP;
	RgDrawState st; ComputeState(st, topo);	// (PrepareForBind o day: texture ao da co cho trong trang truoc khi doi uv)
#ifdef JX_ANDROID
	if (g_nJxPsBuffer)
	{	// [GOP 11/09] o PALROW: bit 0..12 hang bang mau (0x1FFF = khong co), 13..24 chi so to hop ps, 25..30 danh cho lop atlas (buoc sau)
		const UINT uRow = (m_tex[0] && m_tex[0]->m_nPalRow >= 0) ? ((UINT)m_tex[0]->m_nPalRow & 0x1FFFu) : 0x1FFFu;
		uPal = uRow | ((JxPsIdx(st.ps) & 0xFFFu) << 13);
		if (g_nJxAtlasMang && m_tex[0]) uPal |= ((m_tex[0]->JxLop() & 0x3Fu) << 25);	// [MANG 11/09] bit 25..30 = lop trong texture mang
	}
#endif
	const float fPage = m_pAtlas ? (float)m_pAtlas->m_pageSize : 1024.0f;	// [GPU 11/09 ATLAS]
	if (!st.pPipe) return D3DERR_INVALIDCALL;
	// dinh -> ring
	UINT ringOff = (UINT)m_ring.size();
#ifdef JX_ANDROID
	if (g_nJxBindRing)
	{	// [GOP 11/09] can len boi (stride+4) de first_vertex = ringOff / (stride+4) chia het (vai byte dem moi khi doi stride)
		const UINT du = ringOff % s2;
		if (du) { ringOff += s2 - du; m_ring.resize(ringOff); }
	}
#endif
	UINT nOut = nVerts;
	if (type == D3DPT_TRIANGLESTRIP && nVerts == 4)
	{	// quad -> 6 dinh triangle list de gop lo
		static const int s_idx[6] = { 0, 1, 2, 2, 1, 3 };
		m_ring.resize(ringOff + 6 * s2);
		BYTE* d = &m_ring[ringOff];
		for (int i = 0; i < 6; i++) { memcpy(d + i * s2, pVerts + s_idx[i] * stride, stride); *(UINT*)(d + i * s2 + stride) = uPal; }
		RgAtlasUv(d, 6, s2, m_fvf, m_tex[0], fPage);	// [GPU 11/09 ATLAS]
		nOut = 6;
		m_uQuads++;
		// gop vao lenh truoc neu cung trang thai va lien tiep
		if (!m_cmds.empty())
		{
			RgCmd& L = m_cmds.back();
#ifdef JX_ANDROID
			// [GOP 11/09] ps theo dinh -> khong so phan ps khi gop (hai quad chi khac trang thai tang texture van gop duoc)
			const size_t nSo = g_nJxPsBuffer ? offsetof(RgDrawState, ps) : sizeof(st);
#else
			const size_t nSo = sizeof(st);
#endif
			if (L.type == RGCMD_DRAW && L.stride == stride && L.ringOff + L.nVerts * s2 == ringOff && memcmp(&L.st, &st, nSo) == 0)
			{ L.nVerts += 6; return D3D_OK; }
#ifdef JX_ANDROID
			JxGopVo(L, st, stride, ringOff, s2);	// [VE 11/09] vi sao khong gop
#endif
		}
	}
	else if (type == D3DPT_TRIANGLEFAN)
	{
		const UINT nTri = nVerts - 2;
		m_ring.resize(ringOff + (size_t)nTri * 3 * s2);
		BYTE* d = &m_ring[ringOff];
		for (UINT i = 0; i < nTri; i++)
		{
			const UINT src[3] = { 0, i + 1, i + 2 };
			for (int k = 0; k < 3; k++) { memcpy(d + (i * 3 + k) * s2, pVerts + src[k] * stride, stride); *(UINT*)(d + (i * 3 + k) * s2 + stride) = uPal; }
		}
		nOut = nTri * 3;
		RgAtlasUv(d, nOut, s2, m_fvf, m_tex[0], fPage);	// [GPU 11/09 ATLAS]
	}
	else
	{
		m_ring.resize(ringOff + (size_t)nVerts * s2);
		BYTE* d = &m_ring[ringOff];
		for (UINT i = 0; i < nVerts; i++) { memcpy(d + i * s2, pVerts + i * stride, stride); *(UINT*)(d + i * s2 + stride) = uPal; }
		RgAtlasUv(d, nVerts, s2, m_fvf, m_tex[0], fPage);	// [GPU 11/09 ATLAS]
	}
	RgCmd c; memset(&c, 0, sizeof(c)); c.type = RGCMD_DRAW; c.st = st; c.ringOff = ringOff; c.nVerts = nOut; c.stride = stride;
	m_cmds.push_back(c);
	m_uDrawCmds++;
	return D3D_OK;
}
HRESULT CDevGpu::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	if (!m_pStream || !m_pStream->m_pCpu || m_streamStride == 0) return D3DERR_INVALIDCALL;
	UINT nVerts = RgVertexCount(PrimitiveType, PrimitiveCount);
	UINT off = m_streamOffset + StartVertex * m_streamStride;
	if (nVerts == 0 || off + nVerts * m_streamStride > m_pStream->m_len) return D3DERR_INVALIDCALL;
	Lock();
	HRESULT hr = DrawInternal(PrimitiveType, m_pStream->m_pCpu + off, nVerts, m_streamStride);
	Unlock();
	return hr;
}
HRESULT CDevGpu::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	UINT nVerts = RgVertexCount(PrimitiveType, PrimitiveCount);
	if (nVerts == 0 || !pVertexStreamZeroData) return D3DERR_INVALIDCALL;
	Lock();
	HRESULT hr = DrawInternal(PrimitiveType, (const BYTE*)pVertexStreamZeroData, nVerts, VertexStreamZeroStride);
	Unlock();
	return hr;
}

// ---------------------------------------------------------------- khung: phat lai lenh
static void RgEnsureXfer(SDL_GPUDevice* dev, SDL_GPUTransferBuffer** pp, UINT* pSize, UINT need, SDL_GPUTransferBufferUsage usage)
{
	if (*pp && *pSize >= need) return;
	UINT sz = *pSize ? *pSize : (1u << 20); while (sz < need) sz <<= 1;
	if (*pp) SDL_ReleaseGPUTransferBuffer(dev, *pp);
	SDL_GPUTransferBufferCreateInfo ti; memset(&ti, 0, sizeof(ti)); ti.usage = usage; ti.size = sz;
	*pp = SDL_CreateGPUTransferBuffer(dev, &ti); *pSize = *pp ? sz : 0;
}

#ifdef JX_ANDROID
// [GOP 11/09] Chi so to hop trang thai tang texture (RgPsCb) trong bang cua khung: hai lenh ve chi khac ps thi VAN gop duoc vi ps
// di theo dinh (12 bit trong o PALROW) chu khong phai uniform cua lenh. Bang gui len storage buffer mot lan moi khung.
// Nho o cuoi (ps doi 837 lan tren 1 818 lenh -> phan lon lenh dung lai ps ngay truoc) roi moi tra bang bam; bam va cham
// van kiem lai bang memcmp nen khong the tra nham to hop.
UINT CDevGpu::JxPsIdx(const RgPsCb& ps)
{
	if (m_uJxPsCuoi != 0xFFFFFFFFu && memcmp(&m_jxPsCuoi, &ps, sizeof(ps)) == 0) return m_uJxPsCuoi;
	unsigned long long h = 1469598103934665603ULL;	// FNV-1a
	const BYTE* pb = (const BYTE*)&ps;
	for (size_t i = 0; i < sizeof(ps); i++) { h ^= pb[i]; h *= 1099511628211ULL; }
	UINT idx = 0xFFFFFFFFu;
	std::map<unsigned long long, UINT>::iterator it = m_jxPsMap.find(h);
	if (it != m_jxPsMap.end() && it->second < m_jxPsBang.size() && memcmp(&m_jxPsBang[it->second], &ps, sizeof(ps)) == 0)
		idx = it->second;
	if (idx == 0xFFFFFFFFu)
	{
		if (m_jxPsBang.size() < JX_PS_MAX)
		{
			idx = (UINT)m_jxPsBang.size(); m_jxPsBang.push_back(ps);
			if (it == m_jxPsMap.end()) m_jxPsMap[h] = idx;	// va cham bam: giu muc dau, muc sau van dung nhung tra bang memcmp
			if (m_jxPsBang.size() > g_uJxPsBangMax) g_uJxPsBangMax = (unsigned)m_jxPsBang.size();
		}
		else { idx = JX_PS_MAX - 1; m_jxPsBang[idx] = ps; g_uJxPsTran++; }	// tran (chua gap: canh dong nhat vai chuc to hop)
	}
	m_jxPsCuoi = ps; m_uJxPsCuoi = idx;
	return idx;
}
#endif

bool CDevGpu::SubmitFrame(bool bPresent)
{
#ifdef JX_ANDROID
	if (!bPresent) m_bJxKhungCoFlush = true;	// [BKG 11/09] flush giua khung (doc lai / doi khung bay): khung nay khong dung lam "khung truoc" de so
	JxVeDo jxK; memset(&jxK, 0, sizeof(jxK)); const Uint64 uJxK0 = SDL_GetPerformanceCounter(); Uint64 uJxK1 = uJxK0;	// [VE 11/09]
#endif
	SDL_GPUCommandBuffer* cb = SDL_AcquireGPUCommandBuffer(m_pGpu);
	if (!cb) { RgLog("AcquireGPUCommandBuffer that bai: %s", SDL_GetError()); FrameReset(); return false; }
	SDL_GPUTexture* pSwap = NULL; Uint32 swW = 0, swH = 0;
	if (bPresent)
	{
#ifdef JX_ANDROID
		const Uint64 uJxT0 = SDL_GetPerformanceCounter();	// [DONHIP 12/09] do thoi gian cho swapchain (vblank / khung bay / dung lai swapchain)
#endif
		if (!SDL_WaitAndAcquireGPUSwapchainTexture(cb, m_pWin, &pSwap, &swW, &swH)) { if (m_uFrames < 3) RgLog("AcquireSwapchainTexture that bai: %s", SDL_GetError()); pSwap = NULL; }
#ifdef JX_ANDROID
		JxNhipGhiCho(uJxT0, pSwap != NULL, swW, swH);	// [DONHIP 12/09]
		if (pSwap && (swW != s_uJxD1W || swH != s_uJxD1H))
		{	// [D1 11/09] swapchain doi kich thuoc (lan dau / gap-mo / doi hint): ghi de doi chieu backbuffer, cua so, extent SDL bao
			int pw = 0, ph = 0; SDL_GetWindowSizeInPixels(m_pWin, &pw, &ph);
			const char* t = SDL_GetHint("JX_SWAPCHAIN_THAT"); const char* e = SDL_GetHint("JX_SWAPCHAIN_EXTENT");
			RgLog("[D1] swapchain %ux%u | backbuffer %ux%u | cua so %dx%d px | Rep3SwapchainLogic=%d | SDL: tao %s; extent %s", swW, swH, m_bbW, m_bbH, pw, ph, g_nJxSwapchainLogic, t ? t : "-", e ? e : "-");
			s_uJxD1W = swW; s_uJxD1H = swH;
		}
#endif
	}
#ifdef JX_ANDROID
	uJxK1 = SDL_GetPerformanceCounter(); jxK.dCho = JxVeMs(uJxK0, uJxK1);	// [VE 11/09] cho lenh + swapchain
	jxK.uTai = (unsigned)m_texUploads.size(); jxK.uTaiKB = (unsigned)(m_texStage.size() >> 10); jxK.uRingKB = (unsigned)(m_ring.size() >> 10);
#endif
	// ---- copy pass: bang mau, texture, ring dinh
	{
		SDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cb);
#ifdef JX_ANDROID
		// [VE 11/09 f] vung 0 (trang atlas moi / o chua co ban CPU) PHAI ghi lenh TRUOC noi dung texture: trang moi va anh dau tien tren no
		// nam trong cung khung; [VE 11/09 d] tung dat khoi nay SAU tex -> lenh to 0 ghi de anh vua tai (chu thay spr an hien, map loi, ngua mat dau duoi).
		if (!m_jxZeroUploads.empty())
		{	// [VE 11/09 d] vung 0 (trang atlas moi, o chua co ban CPU): tai tu bo dem 0 co dinh, chi memset mot lan khi tao/phinh
			const Uint64 uZ0 = SDL_GetPerformanceCounter();
			const UINT needZ = 2u << 20;	// [VE 11/09 e] bo dem 0 co dinh 2 MiB (khong qua SMALL_ALLOCATION_THRESHOLD cua SDL); vung lon hon tai theo dai
			const UINT uZeroTruoc = m_jxZeroSize;
			RgEnsureXfer(m_pGpu, &m_pJxZeroXfer, &m_jxZeroSize, needZ, SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD);
			if (m_jxZeroSize != uZeroTruoc) m_jxZeroDaXoa = 0;
			if (m_pJxZeroXfer && m_jxZeroDaXoa < m_jxZeroSize)
			{
				BYTE* pz = (BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, m_pJxZeroXfer, false);
				if (pz) { memset(pz, 0, m_jxZeroSize); SDL_UnmapGPUTransferBuffer(m_pGpu, m_pJxZeroXfer); m_jxZeroDaXoa = m_jxZeroSize; }
			}
			if (m_pJxZeroXfer && m_jxZeroDaXoa >= needZ)
			{
				for (size_t i = 0; i < m_jxZeroUploads.size(); i++)
				{
					const RgTexUpload& u = m_jxZeroUploads[i];
					const UINT bppZ = u.bytes / (u.w * u.h); UINT hDai = needZ / (u.w * (bppZ ? bppZ : 4)); if (hDai == 0) hDai = 1;	// [VE 11/09 e] so hang moi dai
					for (UINT y0 = 0; y0 < u.h; y0 += hDai)
					{
						const UINT hh = (u.h - y0 < hDai) ? (u.h - y0) : hDai;
						SDL_GPUTextureTransferInfo src; memset(&src, 0, sizeof(src)); src.transfer_buffer = m_pJxZeroXfer; src.offset = 0; src.pixels_per_row = u.w; src.rows_per_layer = hh;
						SDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = u.pTex; dst.layer = u.layer; dst.x = u.x; dst.y = u.y + y0; dst.w = u.w; dst.h = hh; dst.d = 1;	// [MANG 11/09] lop
						SDL_UploadToGPUTexture(cp, &src, &dst, false);
					}
				}
				m_uUploads += (unsigned)m_jxZeroUploads.size(); jxK.uZero = (unsigned)m_jxZeroUploads.size();
			}
			else RgLog("bo dem 0 (%u B) that bai: %s", needZ, SDL_GetError());
			jxK.dChepZero = JxVeMs(uZ0, SDL_GetPerformanceCounter());
		}
#endif
#ifdef JX_ANDROID
		m_uJxPsStageOff = 0xFFFFFFFFu;
		if (m_pJxPsBuf && !m_jxPsBang.empty())
		{	// [GOP 11/09] bang ps di chung staging cua khung (nhu bang mau)
			const UINT off = ((UINT)m_texStage.size() + 15) & ~15u;
			const UINT bytes = (UINT)(m_jxPsBang.size() * sizeof(RgPsCb));
			m_texStage.resize((size_t)off + bytes);
			memcpy(&m_texStage[off], &m_jxPsBang[0], bytes);
			m_uJxPsStageOff = off;
		}
		size_t uJxTexTruocPal = m_texUploads.size(); Uint64 uJxPalT0 = 0;	// [PALBUF 11/09] muc tu day tro di trong m_texUploads la hang bang mau (kieu texture cu) -> do rieng
		if (!m_palPending.empty() && (m_pPalTex || m_pJxPalBuf))
		{	// [VE 11/09 d] bang mau di chung staging + transfer buffer co dinh (truoc: tao/huy mot transfer buffer rieng moi khung co bang mau moi
			// -> SDL cap/giai phong khoi bo nho 16 MB (vkAllocateMemory) -> chep 40-60 ms tren Fold 7)
			const Uint64 uP0 = SDL_GetPerformanceCounter();
			for (size_t i = 0; i < m_palPending.size(); i++)
			{
				const UINT off = ((UINT)m_texStage.size() + 15) & ~15u;
				m_texStage.resize((size_t)off + 1024);
				memcpy(&m_texStage[off], &m_palPending[i].second[0], 1024);
				if (m_pJxPalBuf) m_jxPalUploads.push_back(std::make_pair((UINT)m_palPending[i].first, off));	// [PALBUF 11/09] -> storage buffer (tai o duoi)
				else { RgTexUpload u = { m_pPalTex, 0, (UINT)m_palPending[i].first, 256, 1, off, 1024 }; m_texUploads.push_back(u); }
			}
			jxK.uPal = (unsigned)m_palPending.size(); m_palPending.clear();
			jxK.dChepPal = JxVeMs(uP0, SDL_GetPerformanceCounter());
		}
		if (0)
#endif
		if (!m_palPending.empty() && m_pPalTex)
		{
			const UINT bytes = (UINT)m_palPending.size() * 256 * 4;
			SDL_GPUTransferBufferCreateInfo ti; memset(&ti, 0, sizeof(ti)); ti.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD; ti.size = bytes;
			SDL_GPUTransferBuffer* pX = SDL_CreateGPUTransferBuffer(m_pGpu, &ti);
			BYTE* p = pX ? (BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, pX, false) : NULL;
			if (p)
			{
				for (size_t i = 0; i < m_palPending.size(); i++) memcpy(p + i * 1024, &m_palPending[i].second[0], 1024);
				SDL_UnmapGPUTransferBuffer(m_pGpu, pX);
				for (size_t i = 0; i < m_palPending.size(); i++)
				{
					SDL_GPUTextureTransferInfo src; memset(&src, 0, sizeof(src)); src.transfer_buffer = pX; src.offset = (Uint32)(i * 1024); src.pixels_per_row = 256; src.rows_per_layer = 1;
					SDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = m_pPalTex; dst.y = (Uint32)m_palPending[i].first; dst.w = 256; dst.h = 1; dst.d = 1;
					SDL_UploadToGPUTexture(cp, &src, &dst, false);
				}
			}
			if (pX) SDL_ReleaseGPUTransferBuffer(m_pGpu, pX);
			m_palPending.clear();
		}
#ifdef JX_ANDROID
		if ((!m_texUploads.empty() || !m_jxPalUploads.empty() || m_uJxPsStageOff != 0xFFFFFFFFu) && !m_texStage.empty())	// [PALBUF 11/09] hang bang mau (storage buffer) cung tai tu staging nay; [GOP 11/09] ca bang ps
#else
		if (!m_texUploads.empty() && !m_texStage.empty())
#endif
		{
#ifdef JX_ANDROID
			const Uint64 uT0 = SDL_GetPerformanceCounter(); const UINT uXferTruoc = m_texXferSize;	// [VE 11/09 d]
#endif
			RgEnsureXfer(m_pGpu, &m_pTexXfer, &m_texXferSize, (UINT)m_texStage.size(), SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD);
#ifdef JX_ANDROID
			if (m_texXferSize != uXferTruoc) jxK.uXferTang++;
#endif
			BYTE* p = m_pTexXfer ? (BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, m_pTexXfer, true) : NULL;
			if (p)
			{
				memcpy(p, &m_texStage[0], m_texStage.size());
				SDL_UnmapGPUTransferBuffer(m_pGpu, m_pTexXfer);
#ifdef JX_ANDROID
				{ const Uint64 u = SDL_GetPerformanceCounter(); jxK.dChepTexMap = JxVeMs(uT0, u); }	// [VE 11/09 d]
#endif
				for (size_t i = 0; i < m_texUploads.size(); i++)
				{
#ifdef JX_ANDROID
					if (i == uJxTexTruocPal) uJxPalT0 = SDL_GetPerformanceCounter();	// [PALBUF 11/09] tu day la hang bang mau kieu texture cu
#endif
					const RgTexUpload& u = m_texUploads[i];
					SDL_GPUTextureTransferInfo src; memset(&src, 0, sizeof(src)); src.transfer_buffer = m_pTexXfer; src.offset = u.stageOff; src.pixels_per_row = u.w; src.rows_per_layer = u.h;
					SDL_GPUTextureRegion dst; memset(&dst, 0, sizeof(dst)); dst.texture = u.pTex; dst.layer = u.layer; dst.x = u.x; dst.y = u.y; dst.w = u.w; dst.h = u.h; dst.d = 1;	// [MANG 11/09] lop
					SDL_UploadToGPUTexture(cp, &src, &dst, false);
				}
				m_uUploads += (unsigned)m_texUploads.size();
#ifdef JX_ANDROID
				jxK.dChepTexLenh = JxVeMs(uT0, SDL_GetPerformanceCounter()) - jxK.dChepTexMap;	// [VE 11/09 d] lenh tai (SDL_UploadToGPUTexture)
				if (uJxPalT0) jxK.dChepPalLenh = JxVeMs(uJxPalT0, SDL_GetPerformanceCounter());	// [PALBUF 11/09] phan hang bang mau (kieu texture cu)
				if (!m_jxPalUploads.empty() && m_pJxPalBuf)
				{	// [PALBUF 11/09] hang bang mau -> storage buffer bang lenh copy buffer (khong dinh layout/tile cua anh 256x8192: het chep 17-100 ms moi khi co sprite moi)
					const Uint64 uPl0 = SDL_GetPerformanceCounter();
					for (size_t i = 0; i < m_jxPalUploads.size(); i++)
					{
						SDL_GPUTransferBufferLocation src = { m_pTexXfer, m_jxPalUploads[i].second }; SDL_GPUBufferRegion dst = { m_pJxPalBuf, m_jxPalUploads[i].first * 1024u, 1024u };
						SDL_UploadToGPUBuffer(cp, &src, &dst, false);
					}
					m_uUploads += (unsigned)m_jxPalUploads.size();
					jxK.dChepPalLenh = JxVeMs(uPl0, SDL_GetPerformanceCounter());
				}
				if (m_pJxPsBuf && !m_jxPsBang.empty() && m_uJxPsStageOff != 0xFFFFFFFFu)
				{	// [GOP 11/09] ca bang to hop trang thai tang texture cua khung (vai chuc muc x 80 byte)
					SDL_GPUTransferBufferLocation src = { m_pTexXfer, m_uJxPsStageOff }; SDL_GPUBufferRegion dst = { m_pJxPsBuf, 0, (Uint32)(m_jxPsBang.size() * sizeof(RgPsCb)) };
					SDL_UploadToGPUBuffer(cp, &src, &dst, false);
					m_uUploads++;
				}
#endif
			}
			else RgLog("map transfer texture (%u B) that bai: %s", (unsigned)m_texStage.size(), SDL_GetError());
		}
#ifdef JX_ANDROID
		jxK.uXferKB = m_texXferSize >> 10;
#endif
		if (!m_ring.empty())
		{
#ifdef JX_ANDROID
			const Uint64 uR0 = SDL_GetPerformanceCounter();	// [VE 11/09 d]
#endif
			const UINT need = (UINT)m_ring.size();
			RgEnsureXfer(m_pGpu, &m_pRingXfer, &m_ringXferSize, need, SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD);
			if (!m_pRingGpu || m_ringGpuSize < need)
			{
				UINT sz = m_ringGpuSize ? m_ringGpuSize : (1u << 20); while (sz < need) sz <<= 1;
				if (m_pRingGpu) SDL_ReleaseGPUBuffer(m_pGpu, m_pRingGpu);
				SDL_GPUBufferCreateInfo bi; memset(&bi, 0, sizeof(bi)); bi.usage = SDL_GPU_BUFFERUSAGE_VERTEX; bi.size = sz;
				m_pRingGpu = SDL_CreateGPUBuffer(m_pGpu, &bi); m_ringGpuSize = m_pRingGpu ? sz : 0;
			}
			BYTE* p = (m_pRingXfer && m_pRingGpu) ? (BYTE*)SDL_MapGPUTransferBuffer(m_pGpu, m_pRingXfer, true) : NULL;
			if (p)
			{
				memcpy(p, &m_ring[0], need);
				SDL_UnmapGPUTransferBuffer(m_pGpu, m_pRingXfer);
				SDL_GPUTransferBufferLocation src = { m_pRingXfer, 0 }; SDL_GPUBufferRegion dst = { m_pRingGpu, 0, need };
				SDL_UploadToGPUBuffer(cp, &src, &dst, true);
			}
			else RgLog("map transfer ring (%u B) that bai: %s", need, SDL_GetError());
#ifdef JX_ANDROID
			jxK.dChepRing = JxVeMs(uR0, SDL_GetPerformanceCounter());	// [VE 11/09 d]
#endif
		}
		SDL_EndGPUCopyPass(cp);
	}
#ifdef JX_ANDROID
	{ const Uint64 u = SDL_GetPerformanceCounter(); jxK.dChep = JxVeMs(uJxK1, u); uJxK1 = u; }	// [VE 11/09] chep len GPU
#endif
	// ---- render pass
	SDL_GPURenderPass* pass = NULL;
	SDL_GPUTexture* pCur = pSwap; UINT curW = swW, curH = swH; bool bCurSwap = true;
	bool bPendingClear = false; D3DCOLOR clearColor = 0;
	RgDrawState last; bool bLast = false;
	float lbScale = 1.0f, lbOffX = 0.0f, lbOffY = 0.0f; Letterbox(swW, swH, &lbScale, &lbOffX, &lbOffY);	// [GPU 08/09 khung ao]
	for (size_t i = 0; i < m_cmds.size(); i++)
	{
		const RgCmd& c = m_cmds[i];
		if (c.type == RGCMD_TARGET)
		{
			if (pass) { SDL_EndGPURenderPass(pass); pass = NULL; }
			if (c.pTarget) { pCur = c.pTarget; bCurSwap = false; curW = curH = 0; for (size_t k = 0; k < m_touched.size(); k++) if (m_touched[k]->m_pGpu == c.pTarget) { curW = m_touched[k]->m_w; curH = m_touched[k]->m_h; break; } }
			else { pCur = pSwap; bCurSwap = true; curW = swW; curH = swH; }
			bPendingClear = false;
			continue;
		}
		if (c.type == RGCMD_CLEAR)
		{
			if (pass) { SDL_EndGPURenderPass(pass); pass = NULL; }
			bPendingClear = true; clearColor = c.clearColor;
			continue;
		}
		if (!pCur) continue;	// khong co swapchain (cua so thu nho)
		if (!pass)
		{
			SDL_GPUColorTargetInfo ci; memset(&ci, 0, sizeof(ci));
			ci.texture = pCur; ci.load_op = bPendingClear ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD; ci.store_op = SDL_GPU_STOREOP_STORE;
			ci.clear_color.a = ((clearColor >> 24) & 0xFF) / 255.0f; ci.clear_color.r = ((clearColor >> 16) & 0xFF) / 255.0f; ci.clear_color.g = ((clearColor >> 8) & 0xFF) / 255.0f; ci.clear_color.b = (clearColor & 0xFF) / 255.0f;
			pass = SDL_BeginGPURenderPass(cb, &ci, 1, NULL);
			bPendingClear = false; bLast = false;
#ifdef JX_ANDROID
			jxK.uPass++;	// [VE 11/09]
#endif
			if (!pass) { RgLog("BeginGPURenderPass that bai: %s", SDL_GetError()); break; }
			SDL_GPUBufferBinding bd = { m_pDummy, 0 }; SDL_BindGPUVertexBuffers(pass, 1, &bd, 1);
#ifdef JX_ANDROID
			if (g_nJxBindRing && m_pRingGpu)
			{	// [GOP 11/09] ring dinh bind MOT lan moi pass; lenh ve dung first_vertex (ringOff da can theo stride+4)
				SDL_GPUBufferBinding bdR = { m_pRingGpu, 0 }; SDL_BindGPUVertexBuffers(pass, 0, &bdR, 1);
			}
			if (g_nJxPalBuffer && m_pJxPalBuf)
			{	// [PALBUF 11/09] bang mau (set 2, binding 2, sau 2 sampler); [GOP 11/09] bang ps (binding 3) - mot lan moi pass
				SDL_GPUBuffer* aBuf[2] = { m_pJxPalBuf, m_pJxPsBuf };
				SDL_BindGPUFragmentStorageBuffers(pass, 0, aBuf, (m_pJxPsBuf && g_nJxPsBuffer) ? 2 : 1);
			}
#endif
		}
		const RgDrawState& st = c.st;
		if (!bLast || st.pPipe != last.pPipe)
		{
			SDL_BindGPUGraphicsPipeline(pass, st.pPipe);
#ifdef JX_ANDROID
			jxK.uDoiPipe++;	// [VE 11/09]
#endif
		}
		if (!bLast || memcmp(&st.vp, &last.vp, sizeof(st.vp)) != 0)
		{
#ifdef JX_ANDROID
			jxK.uDoiCat++;	// [VE 11/09] (viewport)
#endif
			SDL_GPUViewport vp; vp.x = (float)st.vp.X; vp.y = (float)st.vp.Y; vp.w = (float)st.vp.Width; vp.h = (float)st.vp.Height; vp.min_depth = 0.0f; vp.max_depth = 1.0f;
			if (vp.w <= 0.0f) vp.w = (float)m_bbW; if (vp.h <= 0.0f) vp.h = (float)m_bbH;
			if (bCurSwap) { vp.x = lbOffX + vp.x * lbScale; vp.y = lbOffY + vp.y * lbScale; vp.w *= lbScale; vp.h *= lbScale; }	// [GPU 08/09 khung ao]
			SDL_SetGPUViewport(pass, &vp);
		}
		if (!bLast || st.bScissor != last.bScissor || memcmp(&st.rcScissor, &last.rcScissor, sizeof(RECT)) != 0)
		{
#ifdef JX_ANDROID
			jxK.uDoiCat++;	// [VE 11/09] (scissor)
#endif
			SDL_Rect r;
			if (st.bScissor) { r.x = st.rcScissor.left; r.y = st.rcScissor.top; r.w = st.rcScissor.right - st.rcScissor.left; r.h = st.rcScissor.bottom - st.rcScissor.top; if (r.w < 0) r.w = 0; if (r.h < 0) r.h = 0;
				if (bCurSwap) { r.x = (int)(lbOffX + r.x * lbScale); r.y = (int)(lbOffY + r.y * lbScale); r.w = (int)(r.w * lbScale + 0.5f); r.h = (int)(r.h * lbScale + 0.5f); } }	// [GPU 08/09 khung ao]
			else { r.x = 0; r.y = 0; r.w = (int)curW; r.h = (int)curH; }
			SDL_SetGPUScissor(pass, &r);
		}
		if (!bLast || st.pTex[0] != last.pTex[0] || st.pTex[1] != last.pTex[1] || st.pSamp[0] != last.pSamp[0] || st.pSamp[1] != last.pSamp[1])
		{
#ifdef JX_ANDROID
			jxK.uDoiTex++;	// [VE 11/09]
#endif
			SDL_GPUTextureSamplerBinding tb[3];
			tb[0].texture = st.pTex[0]; tb[0].sampler = st.pSamp[0]; tb[1].texture = st.pTex[1]; tb[1].sampler = st.pSamp[1];
			tb[2].texture = m_pPalTex ? m_pPalTex : m_pWhite; tb[2].sampler = st.pSamp[0];
#ifdef JX_ANDROID
			SDL_BindGPUFragmentSamplers(pass, 0, tb, g_nJxPalBuffer ? 2 : 3);	// [PALBUF 11/09] kieu buffer: shader chi khai 2 sampler
#else
			SDL_BindGPUFragmentSamplers(pass, 0, tb, 3);
#endif
		}
		if (!bLast || memcmp(&st.vs, &last.vs, sizeof(st.vs)) != 0)
		{
			SDL_PushGPUVertexUniformData(cb, 0, &st.vs, sizeof(st.vs));
#ifdef JX_ANDROID
			jxK.uDoiVs++;	// [VE 11/09]
#endif
		}
#ifdef JX_ANDROID
		if (!g_nJxPsBuffer && (!bLast || memcmp(&st.ps, &last.ps, sizeof(st.ps)) != 0))	// [GOP 11/09] ps theo dinh: shader doc tu bang, khong day uniform
#else
		if (!bLast || memcmp(&st.ps, &last.ps, sizeof(st.ps)) != 0)
#endif
		{
			SDL_PushGPUFragmentUniformData(cb, 0, &st.ps, sizeof(st.ps));
#ifdef JX_ANDROID
			jxK.uDoiPs++;	// [VE 11/09]
#endif
		}
#ifdef JX_ANDROID
		if (g_nJxBindRing) SDL_DrawGPUPrimitives(pass, c.nVerts, 1, c.ringOff / (c.stride + 4), 0);	// [GOP 11/09] ring da bind dau pass
		else
#endif
		{
			SDL_GPUBufferBinding vb = { m_pRingGpu, c.ringOff };
			SDL_BindGPUVertexBuffers(pass, 0, &vb, 1);
			SDL_DrawGPUPrimitives(pass, c.nVerts, 1, 0, 0);
		}
#ifdef JX_ANDROID
		jxK.uLenh++; jxK.uDinh += c.nVerts;	// [VE 11/09]
#endif
		last = st; bLast = true;
	}
	if (bPendingClear && pCur)
	{	// Clear cuoi khung khong co lenh ve theo sau
		if (pass) { SDL_EndGPURenderPass(pass); pass = NULL; }
		SDL_GPUColorTargetInfo ci; memset(&ci, 0, sizeof(ci));
		ci.texture = pCur; ci.load_op = SDL_GPU_LOADOP_CLEAR; ci.store_op = SDL_GPU_STOREOP_STORE;
		ci.clear_color.a = ((clearColor >> 24) & 0xFF) / 255.0f; ci.clear_color.r = ((clearColor >> 16) & 0xFF) / 255.0f; ci.clear_color.g = ((clearColor >> 8) & 0xFF) / 255.0f; ci.clear_color.b = (clearColor & 0xFF) / 255.0f;
		pass = SDL_BeginGPURenderPass(cb, &ci, 1, NULL);
	}
	if (pass) { SDL_EndGPURenderPass(pass); pass = NULL; }
#ifdef JX_ANDROID
	{ const Uint64 u = SDL_GetPerformanceCounter(); jxK.dGhi = JxVeMs(uJxK1, u); uJxK1 = u; }	// [VE 11/09] ghi lenh render pass
#endif
	// ---- ban sao khung de chup man hinh
#ifdef JX_ANDROID
	if (pSwap && swW && swH && s_nJxChepKhung)	// [DONHIP 12/09] Rep3_DoNhipDat(0, ..) tat de do (swapchain SDL tren Android khong co TRANSFER_SRC)
#else
	if (pSwap && swW && swH)
#endif
	{
		if (!m_pLastFrame || m_lastW != swW || m_lastH != swH)
		{
			if (m_pLastFrame) DeferRelease(m_pLastFrame);
			SDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci)); ci.type = SDL_GPU_TEXTURETYPE_2D; ci.format = m_swapFmt; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
			ci.width = swW; ci.height = swH; ci.layer_count_or_depth = 1; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;
			m_pLastFrame = SDL_CreateGPUTexture(m_pGpu, &ci); m_lastW = swW; m_lastH = swH;
		}
		if (m_pLastFrame)
		{
			SDL_GPUCopyPass* cp = SDL_BeginGPUCopyPass(cb);
			SDL_GPUTextureLocation s; memset(&s, 0, sizeof(s)); s.texture = pSwap; SDL_GPUTextureLocation d; memset(&d, 0, sizeof(d)); d.texture = m_pLastFrame;
			SDL_CopyGPUTextureToTexture(cp, &s, &d, swW, swH, 1, false);
			SDL_EndGPUCopyPass(cp);
		}
	}
	if (!SDL_SubmitGPUCommandBuffer(cb)) RgLog("SubmitGPUCommandBuffer that bai: %s", SDL_GetError());
#ifdef JX_ANDROID
	{ const Uint64 u = SDL_GetPerformanceCounter(); jxK.dNop = JxVeMs(uJxK1, u); jxK.dTong = JxVeMs(uJxK0, u); jxK.uQuad = m_uQuads; if (bPresent) JxVeCong(jxK); }	// [VE 11/09] nop
#endif
	FrameReset();
	return true;
}

void CDevGpu::FrameReset()
{
	m_ring.clear(); m_texStage.clear(); m_texUploads.clear(); m_cmds.clear();
#ifdef JX_ANDROID
	m_jxZeroUploads.clear();	// [VE 11/09 d]
	m_jxPalUploads.clear();	// [PALBUF 11/09]
	m_jxPsBang.clear(); m_jxPsMap.clear(); m_uJxPsCuoi = 0xFFFFFFFFu; m_uJxPsStageOff = 0xFFFFFFFFu;	// [GOP 11/09] bang ps theo tung khung
#endif
	for (size_t i = 0; i < m_touched.size(); i++)
	{
		CTexGpu* p = m_touched[i];
		p->FrameEnd();
		// [GPU 11/09 BOCPU] texture DEFAULT (sprite) da tai len xong (lenh tai vua submit): bo ban CPU - RAM = 1x cache thay vi 2x.
		// Khong bo: render target, DYNAMIC, dang doi (dirty), dang khoa, MANAGED/SYSTEMMEM (engine con ghi tiep).
		// (!m_bCpuBo: dang doc lai tu GPU trong ThuLaiCpu - SubmitFrame(false) giua khung goi vao day - thi khong duoc bo lan nua)
		if (g_nRep3GpuBoBanCpu && p->m_pCpu && !p->m_bCpuBo && !p->m_bDirty && !p->m_bLocked && p->m_bGpuHasData && !p->m_bGpuTarget && !p->m_bGpuNewer
			&& p->m_pool == D3DPOOL_DEFAULT && !(p->m_usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DYNAMIC)) && p->GpuTex())
		{
			m_uCpuBoBytes += (unsigned __int64)p->m_pitch * p->m_h; m_uCpuBoSo++;
			free(p->m_pCpu); p->m_pCpu = NULL; p->m_bCpuBo = true;
		}
	}
	m_touched.clear();
	if (m_pAtlas)	// [GPU 11/09 ATLAS] tra cho trong trang sau khi lenh ve da submit (trang rong -> DeferRelease -> tra ngay duoi day)
		for (size_t i = 0; i < m_atlasFrees.size(); i++) m_pAtlas->Free(m_atlasFrees[i].pPage, m_atlasFrees[i].x, m_atlasFrees[i].y, m_atlasFrees[i].w);
	m_atlasFrees.clear();
	if (m_pGpu) for (size_t i = 0; i < m_release.size(); i++) SDL_ReleaseGPUTexture(m_pGpu, m_release[i]);
	m_release.clear();
	PalFrameEnd();
	m_bFrameOpen = false;
}

#ifdef JX_ANDROID
// [BKG 11/09] Khung nay (danh sach lenh + ring dinh) giong HET khung vua trinh chieu, khong co texture / bang mau / vung 0 cho tai, khong bi flush
// giua khung, chua qua Rep3BoKhungGiongMs va khong bi ep (cua so / be mat doi) -> khong SubmitFrame: man hinh dang hien dung khung do (pixel y het),
// bot ghi lenh + nop + toan bo viec GPU cua khung. Canh yen (logic 18 tick/s, chi noi suy vat dang di chuyen) phan lon khung giong nhau.
// RgCmd/RgDrawState duoc memset 0 khi tao nen so bang memcmp la dung. Tra true = da bo (chi don khung).
bool CDevGpu::JxBoKhungGiong()
{
	if (g_nJxBoKhungGiong < 0) return false;
	const Uint64 uNay = SDL_GetPerformanceCounter();
	const bool bCoTai = !m_texUploads.empty() || !m_palPending.empty() || !m_jxZeroUploads.empty() || !m_jxPalUploads.empty();
	bool bGiong = m_bJxCoKhungTruoc && !m_bJxKhungCoFlush && m_cmds.size() == m_jxCmdsTruoc.size() && m_ring.size() == m_jxRingTruoc.size();
	if (bGiong && !m_cmds.empty() && memcmp(&m_cmds[0], &m_jxCmdsTruoc[0], m_cmds.size() * sizeof(RgCmd)) != 0) bGiong = false;
	if (bGiong && !m_ring.empty() && memcmp(&m_ring[0], &m_jxRingTruoc[0], m_ring.size()) != 0) bGiong = false;
	bool bBo = false;
	if (bGiong)
	{
		if (bCoTai) g_uJxKhungGiongCoTai++;
		else if (s_nJxEpTrinhChieu) g_uJxKhungGiongEp++;
		else if (g_nJxBoKhungGiongMs > 0 && JxVeMs(m_uJxTrinhChieuLuc, uNay) >= (double)g_nJxBoKhungGiongMs) g_uJxKhungGiongEp++;
		else if (g_nJxBoKhungGiong > 0) bBo = true;
		else g_uJxKhungGiongDem++;
	}
	if (bBo)
	{
		g_uJxKhungGiongBo++; m_uJxGiongLienTiep++; if (m_uJxGiongLienTiep > g_uJxKhungGiongChuoiMax) g_uJxKhungGiongChuoiMax = m_uJxGiongLienTiep;
		FrameReset();	// bo lenh + ring cua khung nay (y het khung dang hien); texture cham / cho trong atlas / bang mau don nhu thuong (khong co gi cho tai)
		return true;
	}
	// se trinh chieu: luu lenh + dinh lam "khung truoc" (truoc khi SubmitFrame -> FrameReset xoa); khung co flush giua chung thi khong luu
	if (m_bJxKhungCoFlush) { m_bJxCoKhungTruoc = false; m_bJxKhungCoFlush = false; }
	else { m_jxCmdsTruoc = m_cmds; m_jxRingTruoc = m_ring; m_bJxCoKhungTruoc = true; }
	m_uJxTrinhChieuLuc = uNay; m_uJxGiongLienTiep = 0; s_nJxEpTrinhChieu = 0; g_uJxKhungTrinhChieu++;
	return false;
}
#endif

HRESULT CDevGpu::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	Lock();
#ifdef JX_ANDROID
	if (!JxBoKhungGiong()) SubmitFrame(true);	// [BKG 11/09] khung giong het khung vua trinh chieu -> khong trinh chieu (man hinh giu nguyen)
#else
	SubmitFrame(true);
#endif
	m_uFrames++;
	if (m_uFrames == 1 || (m_uFrames % 1800) == 0)
		RgLog("khung %u: lenh ve %u, quad %u, tai texture %u, pipeline %u, texture GPU %u (%u MB) | atlas %u trang (%u MB) | bo ban CPU %u texture (%u MB), doc lai %u", m_uFrames, m_uDrawCmds, m_uQuads, m_uUploads, (unsigned)m_pipes.size(), g_uRep3GpuTexCount, (unsigned)(g_uRep3GpuTexBytes >> 20),
			g_uRep3AtlasPages, (unsigned)(g_uRep3AtlasBytes >> 20), m_uCpuBoSo, (unsigned)(m_uCpuBoBytes >> 20), m_uCpuBoThuLai);	// [GPU 11/09 ATLAS] [GPU 11/09 BOCPU]
#ifdef JX_ANDROID
	s_uRep3GpuLenhVe = m_uDrawCmds; s_uRep3GpuQuad = m_uQuads;	// [ANDROID 11/09 HUD]
#endif
	m_uDrawCmds = m_uQuads = m_uUploads = 0;
	Unlock();
	return D3D_OK;
}

// ---------------------------------------------------------------- bang mau (nhu D3D9on11Pal)
bool CDevGpu::PalInit()
{
#ifdef JX_ANDROID
	if (m_pJxPalBuf) return true;
	if (g_nJxPalBuffer && !m_pPalTex)
	{	// [PALBUF 11/09] bang mau = storage buffer RG_PAL_ROWS hang x 1 KB: hang tai bang SDL_UploadToGPUBuffer (copy buffer thuong), shader doc g_palBuf[row*256+idx]
		SDL_GPUBufferCreateInfo bi; memset(&bi, 0, sizeof(bi)); bi.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ; bi.size = (Uint32)RG_PAL_ROWS * 1024u;
		m_pJxPalBuf = SDL_CreateGPUBuffer(m_pGpu, &bi);
		if (!m_pJxPalBuf) { RgLog("bang mau: CreateGPUBuffer %d KB that bai: %s", RG_PAL_ROWS, SDL_GetError()); return false; }
		m_palFree.reserve(RG_PAL_ROWS);
		for (int i = RG_PAL_ROWS - 1; i >= 0; i--) m_palFree.push_back(i);
		RgLog("bang mau: storage buffer %d hang x 256 mau BGRA8 (%d MB) [PALBUF 11/09]", RG_PAL_ROWS, RG_PAL_ROWS / 1024);
		return true;
	}
#endif
	if (m_pPalTex) return true;
	SDL_GPUTextureCreateInfo ci; memset(&ci, 0, sizeof(ci)); ci.type = SDL_GPU_TEXTURETYPE_2D; ci.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM; ci.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
	ci.width = 256; ci.height = RG_PAL_ROWS; ci.layer_count_or_depth = 1; ci.num_levels = 1; ci.sample_count = SDL_GPU_SAMPLECOUNT_1;
	m_pPalTex = SDL_CreateGPUTexture(m_pGpu, &ci);
	if (!m_pPalTex) { RgLog("bang mau: CreateGPUTexture 256x%d that bai: %s", RG_PAL_ROWS, SDL_GetError()); return false; }
	m_palFree.reserve(RG_PAL_ROWS);
	for (int i = RG_PAL_ROWS - 1; i >= 0; i--) m_palFree.push_back(i);
	RgLog("bang mau: atlas 256x%d BGRA8 (%d MB)", RG_PAL_ROWS, RG_PAL_ROWS / 1024);
	return true;
}
void CDevGpu::PalRelease()
{
	if (m_pPalTex && m_pGpu) SDL_ReleaseGPUTexture(m_pGpu, m_pPalTex);
	m_pPalTex = NULL; m_palFree.clear(); m_palDeferred.clear(); m_palPending.clear();
#ifdef JX_ANDROID
	if (m_pJxPalBuf && m_pGpu) SDL_ReleaseGPUBuffer(m_pGpu, m_pJxPalBuf);
	m_pJxPalBuf = NULL; m_jxPalUploads.clear();	// [PALBUF 11/09]
	if (m_pJxPsBuf && m_pGpu) SDL_ReleaseGPUBuffer(m_pGpu, m_pJxPsBuf);
	m_pJxPsBuf = NULL; m_jxPsBang.clear(); m_jxPsMap.clear(); m_uJxPsCuoi = 0xFFFFFFFFu;	// [GOP 11/09]
#endif
}
void CDevGpu::PalFrameEnd()
{
	for (size_t i = 0; i < m_palDeferred.size(); i++) m_palFree.push_back(m_palDeferred[i]);
	m_palDeferred.clear();
}
int CDevGpu::PalAlloc(const unsigned char* pPal24, int nColors)
{
	if (!pPal24 || nColors <= 0) return -1;
	Lock();
	if (!PalInit() || m_palFree.empty()) { Unlock(); return -1; }
	int row = m_palFree.back(); m_palFree.pop_back();
	std::vector<DWORD> rgb(256);
	if (nColors > 256) nColors = 256;
	for (int i = 0; i < 256; i++)
		rgb[i] = (i < nColors) ? (0xFF000000u | ((DWORD)pPal24[i * 3 + 0] << 16) | ((DWORD)pPal24[i * 3 + 1] << 8) | (DWORD)pPal24[i * 3 + 2]) : 0xFF000000u;
	m_palPending.push_back(std::make_pair(row, rgb));
	g_uRep3PalRows++;
	Unlock();
	return row;
}
void CDevGpu::PalFree(int row)
{
	if (row < 0 || row >= RG_PAL_ROWS) return;
	Lock();
	m_palDeferred.push_back(row);
	if (g_uRep3PalRows) g_uRep3PalRows--;
	Unlock();
}

#endif // JX_PLATFORM_SDL

#ifdef JX_ANDROID
// [ANDROID 11/09 HUD] chuoi cho bang do hieu nang: driver | texture GPU | atlas | trinh chieu | lenh ve khung truoc. Tra do dai chuoi.
extern "C" int Rep3_ThongKeGpu(char* sz, int n)
{
	if (!sz || n < 8) return 0;
	snprintf(sz, (size_t)n, "GPU %s   texture %u MB (%u)   atlas %u trang   %s   lÖnh vÏ %u, quad %u",
		s_szRep3GpuDriver[0] ? s_szRep3GpuDriver : "?", (unsigned)(g_uRep3GpuTexBytes >> 20), g_uRep3GpuTexCount, g_uRep3AtlasPages,
		s_szRep3GpuTrinhChieu, s_uRep3GpuLenhVe, s_uRep3GpuQuad);
	sz[n - 1] = 0;
	return (int)strlen(sz);
}

// [BKG 11/09] cua so / be mat doi (xoay, gap-mo, quay lai app, vung an toan, tieu diem): khung ke tiep PHAI trinh chieu du giong khung truoc.
// KSdlApp::TranslateEvent goi qua GetModuleHandle/GetProcAddress (lop tuong thich) nhu Rep3_DoNhipDat.
extern "C" void Rep3_JxEpTrinhChieu() { s_nJxEpTrinhChieu = 1; }

// [DONHIP 12/09] ban do nhip: bat/tat chep swapchain moi khung + so khung bay (SDL_SetGPUAllowedFramesInFlight 1..3). -1 = giu nguyen.
// Goi tu luong chinh giua hai khung. Doi so khung bay lam SDL cho het hang lenh va dung lai swapchain -> chi goi khi so thay doi.
extern "C" void Rep3_DoNhipDat(int nChepKhung, int nKhungBay)
{
	if (nChepKhung >= 0)
		s_nJxChepKhung = nChepKhung ? 1 : 0;
	CDevGpu* d = g_pRep3DevGpu;
	if (nKhungBay >= 1 && nKhungBay <= 3 && nKhungBay != s_nJxKhungBay && d && d->m_pGpu)
	{
		d->Lock();
		if (d->m_bFrameOpen || !d->m_cmds.empty() || !d->m_texUploads.empty()) d->SubmitFrame(false);
		if (SDL_SetGPUAllowedFramesInFlight(d->m_pGpu, (Uint32)nKhungBay)) s_nJxKhungBay = nKhungBay;
		else RgLog("[DONHIP] SetGPUAllowedFramesInFlight(%d) that bai: %s", nKhungBay, SDL_GetError());
		s_uJxTraTruoc = 0;
		d->Unlock();
	}
}
// [DONHIP 12/09] so lieu tu lan lay truoc roi dat lai. pHist[0..nBins) = cach giua hai lan tra swapchain (o 0,25 ms).
// pSo: 0 so lan lay swapchain, 1 lan khong co swapchain, 2 tong ms cho, 3 cho lau nhat (ms), 4 so lan cho > 4 ms,
//      5 rong swapchain, 6 cao swapchain, 7 so khung bay dang dat. Tra so o bieu do da chep.
extern "C" int Rep3_DoNhipLay(unsigned* pHist, int nBins, double* pSo, int nSo)
{
	const int n = (nBins < 400) ? nBins : 400;
	if (pHist) for (int i = 0; i < n; i++) pHist[i] = s_aJxTraHist[i];
	s_aJxSo[7] = (double)s_nJxKhungBay;
	if (pSo) for (int i = 0; i < nSo && i < 8; i++) pSo[i] = s_aJxSo[i];
	memset(s_aJxTraHist, 0, sizeof(s_aJxTraHist));
	const double w = s_aJxSo[5], h = s_aJxSo[6];
	memset(s_aJxSo, 0, sizeof(s_aJxSo));
	s_aJxSo[5] = w; s_aJxSo[6] = h;
	s_uJxTraTruoc = 0;
	return n;
}
#endif
