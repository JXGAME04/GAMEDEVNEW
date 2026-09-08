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
double Rep3NapMs(const LARGE_INTEGER& a, const LARGE_INTEGER& b);
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
extern int  g_nRep3Pal;	// [D3D11 08/09 r]
extern unsigned g_uRep3PalRows;
struct IDirect3DTexture9;
int  Rep3_D3D11PaletteOK(); int Rep3_D3D11AllocPalette(const unsigned char* pPal24, int nColors); void Rep3_D3D11FreePalette(int nRow); void Rep3_D3D11TagPalette(IDirect3DTexture9* pTex, int nRow);
extern int  g_nRep3Waitable;	// [D3D11 08/09 n]
extern int  g_nRep3Buffers;	// [D3D11 08/09 m]
extern int  g_nRep3NoWait;	// [D3D11 08/09 l]
extern int  g_nRep3Latency;	// [D3D11 08/09 k]
extern int  g_nRep3Batch; extern unsigned g_uRep3BatchQuads; extern unsigned g_uRep3BatchDraws;	// [D3D11 08/09 j]
extern int  g_nRep3Tearing;     // [D3D11 08/09 f]
void Rep3_D3D11VramInfo(unsigned* puUsedMB, unsigned* puBudgetMB);
extern int  g_nRep3Flip;        // [D3D11 08/09 b] 1 = swapchain flip, 0 = bitblt cu
extern double g_dRep3PresentMs; extern unsigned g_uRep3Presents; extern unsigned g_uRep3PresentSkip; extern double g_dRep3DrawMs; extern unsigned g_uRep3Draws;	// [D3D11 08/09 b] thong ke
void Rep3Log(const char* fmt, ...);

#define YESNO(X) (X ? "Yes" : "No")

//#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#endif