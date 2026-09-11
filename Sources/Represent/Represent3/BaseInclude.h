#ifndef __BASEINCLUDE_H__
#define __BASEINCLUDE_H__

#define DEFAULT_SCREENWIDTH		800				// 缺省屏幕宽度
#define DEFAULT_SCREENHEIGHT	600				// 缺省屏幕高度
#define DEFAULT_BITDEPTH		32				// 缺省位深度

#define MAX_TEXTURE_SIZE		1024			// 最大贴图尺寸
#define MIN_TEXTURE_SIZE		8				// 最小贴图尺寸

enum RenderModel
{
	RenderModel2D,
	RenderModel3DOrtho,
	RenderModel3DPerspective,
};

extern HWND		g_hWnd;

extern D3DFORMAT g_16BitFormat;

extern bool		g_bRefRast;						// 是否使用软件3D加速
extern bool		g_bRunWindowed;					// 是否使用窗口模式

extern int32	g_nScreenWidth;					// 显示窗口宽度
extern int32	g_nScreenHeight;				// 显示窗口高度

extern uint32	g_nAntiAliasFSOverSample;		// 多采样级别
extern uint32	g_nBackBufferCount;				// BackBuffer个数
extern bool		g_bForceSwapEffectBlt;			// 是否强制显存交换方式为拷贝
extern bool		g_bForce32Bit;					// 是否强制以32位方式处理ISI_T_BITMAP16资源,暂时不用，都安32位处理

extern int		g_nFogEnable;					// 是否打开雾化效果

extern int		g_nWireframe;					// 是否打开线框模式

extern RenderModel		g_renderModel;				// 是否打开透视模式

extern bool		g_bUse4444Texture;				// spr资源是否使用4444格式贴图，如果否，则使用8888格式

extern bool		g_bNonPow2Conditional;			// 是否允许非二的幂次长宽的贴图

extern int		g_ntest;

// [REP3 03/09] cong tac doc tu [Client] config.ini (KRepresentShell3.cpp)
extern int  g_nRep3Flat;        // 1 = ve phang 2D nhu Represent2 (mac dinh), 0 = ortho 3D cu
extern int  g_nRep3Composite;   // 1 = ghep nhan vat len texture roi ve (cach cu), 0 = ve tung phan nhu Represent2
extern int  g_nRep3Tex32;       // 1 = texture sprite A8R8G8B8 (dung mau palette), 0 = A4R4G4B4
extern int  g_nRep3Npot;        // 1 = dung texture khong luy thua 2 neu card ho tro (theo 2.0)
extern int  g_nRep3Vsync;       // 0 = Present ngay (client tu dieu nhip PaintFps), 1 = cho vsync
extern int  g_nRep3CacheMB;     // 0 = tu tinh theo RAM, >0 = ngan sach cache texture (MB)
extern int  g_nRep3Log;         // 1 = ghi jx_rep3.log
extern int  g_nRep3Pool;       // [REP3 03/09 RAM] 1 = texture sprite o D3DPOOL_DEFAULT (chi VRAM, khong ban sao RAM), 0 = MANAGED cu
// [NAP 08/09 a] do thoi gian NAP tai nguyen tren luong ve (tep spr, jpeg, rut khung, giai ma, tao GPU)
struct Rep3NapDo { unsigned n; double ms; double max; };
extern Rep3NapDo g_napSpr, g_napJpeg, g_napKhung, g_napGiaiMa, g_napGpu;
extern double g_dRep3NapKhung, g_dRep3NapKhungMax; extern unsigned g_uRep3NapKhung5, g_uRep3NapKhung16;
void   Rep3NapCong(Rep3NapDo& d, double ms);
extern unsigned g_uRep3VeLoai[6];	// [VE 08/09 a] don vi ve theo loai: 0 npc, 1 skill, 2 ui, 3 maps, 4 anh tao, 5 khac
extern unsigned g_uRep3VeKhung;		// so khung ve trong ky
void   Rep3VeDem(const char* pszImage);
double Rep3NapMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b);
#ifdef JX_ANDROID
// [VE 11/09] Android: (A) do tung buoc trinh chieu CDevGpu::SubmitFrame; (B) nap KHUNG sprite o luong nen theo ngan sach.
//   In jx_rep3.log: [VE] + [VE-GOP] + [VE-NAP] moi ky Rep3StatSec (KRepresentShell3.cpp JxVeKyIn), [VE-GIAT] khung cham (JxVeGiatGhi).
struct JxVeDo { double dCho, dChep, dGhi, dNop, dTong, dChepPal, dChepTexMap, dChepTexLenh, dChepZero, dChepRing; unsigned uTai, uTaiKB, uRingKB, uLenh, uQuad, uDinh, uPass, uDoiPipe, uDoiTex, uDoiVs, uDoiPs, uDoiCat, uPal, uZero, uXferTang, uXferKB; double dChepPalLenh; };	// [VE 11/09 d] + tung buoc chep
extern JxVeDo g_jxVeKhung, g_jxVeTong, g_jxVeMax;		// khung vua submit / cong don ky / max trong ky (D3D9onGPUDev.cpp ghi; KRepresentShell3.cpp doc + dat lai)
extern unsigned g_uJxVeKhungSo, g_uJxVe8, g_uJxVe16, g_uJxGopVo[8];	// so khung trong ky, khung SubmitFrame > 8 / > 16 ms, ly do quad khong gop (xem JxGopVo)
class TextureResSpr; class TextureResMgr;
extern TextureResMgr* g_pJxTexMgr;	// bo quan ly texture duy nhat (TextureResMgr ctor gan)
struct JxKhungViec { TextureResSpr* pSpr; int nFrame; int nBpp; int eFmt; int bPal; int nNguon; unsigned uLuc; };	// viec cho luong nen (nNguon 0 = dang ve can, 1 = nap truoc)
struct JxKhungXong { TextureResSpr* pSpr; int nFrame; int nW, nH, nOffX, nOffY; BYTE* pDiem; int nBpp; int eFmt; int bPal; int bHong; unsigned uLuc; };	// ket qua: khung da giai ma
extern int g_nJxNapKhungNen, g_nJxNapKhungMs, g_nJxNapKhungTruoc, g_nJxNapKhungApMs, g_nJxVeGiatMs;	// [Client] NapKhungNen / NapKhungMs / NapKhungTruoc / NapKhungApMs / VeGiatMs
extern int g_nJxAnhBoVeNen;	// 1 = GetImage vua tra NULL vi khung dang nap o luong nen (Rep3AnhNullGhi bo qua, khong tinh la anh thieu)
extern int g_nJxHoiKhongDe;	// [VE 11/09 d] [Client] NapHoiKhongDe: 1 = hoi kich thuoc sprite NPC dang nap o luong nen -> tra 'chua co' (khong nap dong bo de len)
extern unsigned g_uJxNapKhungRong, g_uJxHoiTre;	// [VE 11/09 d] khung rong/hong o luong nen (khong giao lai); so lan hoi NPC dang nap tra 'chua co'
extern int g_nJxAtlasKe, g_nJxAtlasTrang;	// [VE 11/09 e] [Client] Rep3AtlasKe: 1 = atlas xep ke theo dinh dang (mot trang nhieu hang bin); Rep3AtlasTrang: co trang 1024/2048/4096
extern int g_nJxPalBuffer;	// [PALBUF 11/09] [Client] Rep3PalBuffer: 1 = bang mau trong storage buffer (tai hang bang lenh copy buffer, khong copy tung hang vao texture 256x8192)
extern int g_nJxBoKhungGiong, g_nJxBoKhungGiongMs;	// [BKG 11/09] [Client] Rep3BoKhungGiong: 1 = khung giong het khung vua trinh chieu -> khong trinh chieu; 0 = chi dem; -1 = tat. Rep3BoKhungGiongMs = toi da ms giua hai lan trinh chieu
extern int g_nJxSwapchainLogic;	// [D1 11/09] [Client] Rep3SwapchainLogic: swapchain = backbuffer x %/100 (100 = khung logic, HWC phong len man; 0 = cua so nhu cu)
extern int g_nJxPsBuffer, g_nJxBindRing;	// [GOP 11/09] [Client] Rep3PsBuffer: trang thai tang texture qua storage buffer + chi so theo dinh (gop duoc quad khac ps);
										// Rep3BindRing: bind ring dinh mot lan moi render pass, lenh ve dung first_vertex
extern unsigned g_uJxPsBangMax, g_uJxPsTran;	// [GOP 11/09] so muc bang ps lon nhat trong ky / so lan tran bang
extern int g_nJxAtlasMang, g_nJxAtlasLop, g_nJxAtlasCumMB;	// [MANG 11/09] [Client] Rep3AtlasMang: nhieu trang atlas trong MOT texture mang 2D (lop di theo dinh) -> gop duoc quad khac trang
extern unsigned g_uJxAtlasCum;	// [MANG 11/09] so cum dang song
extern unsigned g_uJxKhungGiongBo, g_uJxKhungGiongCoTai, g_uJxKhungGiongEp, g_uJxKhungGiongDem, g_uJxKhungGiongChuoiMax, g_uJxKhungTrinhChieu;	// [BKG 11/09] dem trong ky ([VE-BKG])
extern unsigned g_uJxNapKhungBoVe, g_uJxNapKhungBoVeKhung, g_uJxNapKhungDongBo, g_uJxNapKhungGiao, g_uJxNapKhungTruocSo, g_uJxNapKhungXong, g_uJxNapKhungHong, g_uJxNapKhungBo, g_uJxNapKhungChoMax;
extern double g_dJxNapKhungTre, g_dJxNapKhungTreMax, g_dJxNapNenBan, g_dJxNapKhungAp, g_dJxNapKhungApMax; extern unsigned g_uJxNapKhungApKhung;
extern Rep3NapDo g_jxNapNgoaiVe;	// nap dong bo NGOAI luc ve (hoi kich thuoc / alpha tu logic) trong ky
#endif
extern int  g_nRep3NapNen;	// [NAP 08/09 b] [Client] Rep3NapNen: 1 = nap sprite/jpeg o luong nen (mac dinh), 0 = nap ngay tren luong ve nhu cu
extern int  g_nRep3StatSec;    // [REP3 03/09 RAM] chu ky ghi thong ke RAM/VRAM/cache vao jx_rep3.log (giay), 0 = tat
extern bool g_bNpotOK;          // card + thiet bi da qua thu NPOT
extern int  g_nMaxTexW, g_nMaxTexH;
extern int  g_nRep3Api;         // [D3D11 08/09] [Client] Rep3Api: 9 = D3D9 (mac dinh), 11 = D3D9 tren D3D11
extern int  g_nRep3ApiOn;       // API thuc chay sau D3D_Shell::Create
extern unsigned g_uRep3GpuTexCount;          // [D3D11 08/09] so texture GPU dang song
extern unsigned __int64 g_uRep3GpuTexBytes;  // [D3D11 08/09] byte texture GPU
extern int  g_nRep3Atlas;       // [D3D11 08/09 d] gom texture nho vao trang atlas
extern unsigned g_uRep3AtlasPages; extern unsigned __int64 g_uRep3AtlasBytes;
#ifdef JX_PLATFORM_SDL
extern int  g_nRep3AtlasGpu;	// [GPU 11/09 ATLAS] [Client] Rep3AtlasGpu: atlas cho lop SDL_GPU (Rep3Api=100); mac dinh 1 Android, 0 Windows
extern int  g_nRep3GpuBoBanCpu;	// [GPU 11/09 BOCPU] [Client] Rep3GpuBoBanCpu: bo ban CPU cua texture DEFAULT sau khi tai len; mac dinh 1 Android, 0 Windows
extern int  g_nRep3GpuMailbox;	// [ANDROID 11/09 MAILBOX] [Client] Rep3GpuMailbox: dung MAILBOX khi IMMEDIATE khong co; mac dinh 1 Android, 0 Windows
#endif
extern int  g_nRep3Pal;	// [D3D11 08/09 r]
extern unsigned g_uRep3PalRows;
struct IDirect3DTexture9;
int  Rep3_D3D11PaletteOK(); int Rep3_D3D11AllocPalette(const unsigned char* pPal24, int nColors); void Rep3_D3D11FreePalette(int nRow); void Rep3_D3D11TagPalette(IDirect3DTexture9* pTex, int nRow);
extern int  g_nRep3Waitable;	// [D3D11 08/09 n]
extern int  g_nRep3Buffers;	// [D3D11 08/09 m]
extern int  g_nRep3NoWait;	// [D3D11 08/09 l]
extern int  g_nRep3Latency;	// [D3D11 08/09 k]
extern int  g_nRep3Batch; extern unsigned g_uRep3BatchQuads; extern unsigned g_uRep3BatchDraws; extern int g_nRep3AtlasMang; /* [MANG 09/09] */	// [D3D11 08/09 j]
extern int  g_nRep3Tearing;     // [D3D11 08/09 f]
void Rep3_D3D11VramInfo(unsigned* puUsedMB, unsigned* puBudgetMB);
extern int  g_nRep3Flip;        // [D3D11 08/09 b] 1 = swapchain flip, 0 = bitblt cu
extern double g_dRep3PresentMs; extern unsigned g_uRep3Presents; extern unsigned g_uRep3PresentSkip; extern double g_dRep3DrawMs; extern unsigned g_uRep3Draws;	// [D3D11 08/09 b] thong ke
void Rep3Log(const char* fmt, ...);

#define YESNO(X) (X ? "Yes" : "No")

//#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#endif