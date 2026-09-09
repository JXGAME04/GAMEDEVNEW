// [GPU 08/09] d3d9mini.h - tap con cua d3d9types.h / d3d9.h / d3d9caps.h / d3dx9 (vector, ma tran) ma Represent3 + lop D3D9onGPU dang dung,
// de bien dich Represent3 KHONG can Windows SDK / DirectX (Android, hoac kiem tren Windows voi /DJX_D3D9MINI).
// Gia tri enum/co/GUID giu dung nhu D3D9. Vtable cac giao dien giu dung thu tu d3d9.h (D3D9onGPUi.h cai vtable nay).
// Chi dung khi JX_D3D9MINI. Kieu Win32 co ban (DWORD/UINT/BOOL/HRESULT/RECT/HWND/GUID...) lay tu windows.h (Windows) hoac KPosixCompat.h (POSIX).
#pragma once
#ifdef JX_D3D9MINI

#include <string.h>
#include <math.h>

// Windows (kiem tren PC): chan cac header DirectX that neu bi keo vao sau (dsound.h/ddraw.h/d3d9.h...) bang chinh guard cua chung
#define _d3d9TYPES_H_
#define _D3D9_H_		// guard that cua d3d9.h (SDK: _D3D9_H_)
#define _d3d9CAPS_H		// guard that cua d3d9caps.h (SDK: _d3d9CAPS_H, khong co _ cuoi)
#define __D3DX9_H__
#define __D3DX9MATH_H__
#define __D3DX9CORE_H__
#define __D3DX9TEX_H__
#define D3DVECTOR_DEFINED
#define D3DCOLORVALUE_DEFINED
#define D3DRECT_DEFINED
#define D3DMATRIX_DEFINED

#ifndef _WIN32
	#include "../../Engine/Src/Platform/KPosixCompat.h"
	#ifndef __stdcall
		#define __stdcall
	#endif
	#ifndef CONST
		#define CONST const
	#endif
	#ifndef _RGNDATA_MINI
	#define _RGNDATA_MINI
	typedef struct _RGNDATAHEADER { DWORD dwSize; DWORD iType; DWORD nCount; DWORD nRgnSize; RECT rcBound; } RGNDATAHEADER;
	typedef struct _RGNDATA { RGNDATAHEADER rdh; char Buffer[1]; } RGNDATA;
	#ifndef _LUID_MINI
	#define _LUID_MINI
	typedef struct _LUID { DWORD LowPart; LONG HighPart; } LUID;
	#endif
	#endif
#endif

// ---------------------------------------------------------------- ma loi
#ifndef D3D_OK
#define D3D_OK                          0
#endif
#define _FACD3D                         0x876
#define MAKE_D3DHRESULT(code)           ((HRESULT)(0x80000000 | (_FACD3D << 16) | (code)))
#define D3DERR_WRONGTEXTUREFORMAT       MAKE_D3DHRESULT(2072)
#define D3DERR_UNSUPPORTEDCOLOROPERATION MAKE_D3DHRESULT(2073)
#define D3DERR_TOOMANYOPERATIONS        MAKE_D3DHRESULT(2077)
#define D3DERR_CONFLICTINGRENDERSTATE   MAKE_D3DHRESULT(2081)
#define D3DERR_UNSUPPORTEDTEXTUREFILTER MAKE_D3DHRESULT(2082)
#define D3DERR_DRIVERINTERNALERROR      MAKE_D3DHRESULT(2087)
#define D3DERR_NOTFOUND                 MAKE_D3DHRESULT(2150)
#define D3DERR_MOREDATA                 MAKE_D3DHRESULT(2151)
#define D3DERR_DEVICELOST               MAKE_D3DHRESULT(2152)
#define D3DERR_DEVICENOTRESET           MAKE_D3DHRESULT(2153)
#define D3DERR_NOTAVAILABLE             MAKE_D3DHRESULT(2154)
#define D3DERR_OUTOFVIDEOMEMORY         MAKE_D3DHRESULT(380)
#define D3DERR_INVALIDDEVICE            MAKE_D3DHRESULT(2155)
#define D3DERR_INVALIDCALL              MAKE_D3DHRESULT(2156)
#define D3DERR_DRIVERINVALIDCALL        MAKE_D3DHRESULT(2157)
#define D3DERR_WASSTILLDRAWING          MAKE_D3DHRESULT(540)
#ifndef E_FAIL
#define E_FAIL          ((HRESULT)0x80004005L)
#endif
#ifndef E_POINTER
#define E_POINTER       ((HRESULT)0x80004003L)
#endif
#ifndef E_NOINTERFACE
#define E_NOINTERFACE   ((HRESULT)0x80004002L)
#endif
#ifndef E_OUTOFMEMORY
#define E_OUTOFMEMORY   ((HRESULT)0x8007000EL)
#endif
#ifndef S_OK
#define S_OK            ((HRESULT)0L)
#endif
#ifndef SUCCEEDED
#define SUCCEEDED(hr)   (((HRESULT)(hr)) >= 0)
#define FAILED(hr)      (((HRESULT)(hr)) < 0)
#endif

#define D3D_SDK_VERSION                 32
#define D3DADAPTER_DEFAULT              0
#define MAX_DEVICE_IDENTIFIER_STRING    512

typedef DWORD D3DCOLOR;
#define D3DCOLOR_ARGB(a,r,g,b)  ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define D3DCOLOR_RGBA(r,g,b,a)  D3DCOLOR_ARGB(a,r,g,b)
#define D3DCOLOR_XRGB(r,g,b)    D3DCOLOR_ARGB(0xff,r,g,b)
#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24))
#endif

// ---------------------------------------------------------------- enum (gia tri D3D9)
typedef enum _D3DFORMAT
{
	D3DFMT_UNKNOWN = 0,
	D3DFMT_R8G8B8 = 20, D3DFMT_A8R8G8B8 = 21, D3DFMT_X8R8G8B8 = 22, D3DFMT_R5G6B5 = 23, D3DFMT_X1R5G5B5 = 24, D3DFMT_A1R5G5B5 = 25,
	D3DFMT_A4R4G4B4 = 26, D3DFMT_R3G3B2 = 27, D3DFMT_A8 = 28, D3DFMT_A8R3G3B2 = 29, D3DFMT_X4R4G4B4 = 30, D3DFMT_A2B10G10R10 = 31,
	D3DFMT_A8B8G8R8 = 32, D3DFMT_X8B8G8R8 = 33, D3DFMT_G16R16 = 34, D3DFMT_A2R10G10B10 = 35, D3DFMT_A16B16G16R16 = 36,
	D3DFMT_A8P8 = 40, D3DFMT_P8 = 41, D3DFMT_L8 = 50, D3DFMT_A8L8 = 51, D3DFMT_A4L4 = 52,
	D3DFMT_V8U8 = 60, D3DFMT_L6V5U5 = 61, D3DFMT_X8L8V8U8 = 62, D3DFMT_Q8W8V8U8 = 63, D3DFMT_V16U16 = 64, D3DFMT_A2W10V10U10 = 67,
	D3DFMT_UYVY = MAKEFOURCC('U', 'Y', 'V', 'Y'), D3DFMT_YUY2 = MAKEFOURCC('Y', 'U', 'Y', '2'),
	D3DFMT_DXT1 = MAKEFOURCC('D', 'X', 'T', '1'), D3DFMT_DXT2 = MAKEFOURCC('D', 'X', 'T', '2'), D3DFMT_DXT3 = MAKEFOURCC('D', 'X', 'T', '3'),
	D3DFMT_DXT4 = MAKEFOURCC('D', 'X', 'T', '4'), D3DFMT_DXT5 = MAKEFOURCC('D', 'X', 'T', '5'),
	D3DFMT_D16_LOCKABLE = 70, D3DFMT_D32 = 71, D3DFMT_D15S1 = 73, D3DFMT_D24S8 = 75, D3DFMT_D24X8 = 77, D3DFMT_D24X4S4 = 79, D3DFMT_D16 = 80,
	D3DFMT_D32F_LOCKABLE = 82, D3DFMT_D24FS8 = 83, D3DFMT_L16 = 81, D3DFMT_VERTEXDATA = 100, D3DFMT_INDEX16 = 101, D3DFMT_INDEX32 = 102,
	D3DFMT_FORCE_DWORD = 0x7fffffff
} D3DFORMAT;

typedef enum _D3DPOOL { D3DPOOL_DEFAULT = 0, D3DPOOL_MANAGED = 1, D3DPOOL_SYSTEMMEM = 2, D3DPOOL_SCRATCH = 3, D3DPOOL_FORCE_DWORD = 0x7fffffff } D3DPOOL;
typedef enum _D3DRESOURCETYPE { D3DRTYPE_SURFACE = 1, D3DRTYPE_VOLUME = 2, D3DRTYPE_TEXTURE = 3, D3DRTYPE_VOLUMETEXTURE = 4, D3DRTYPE_CUBETEXTURE = 5, D3DRTYPE_VERTEXBUFFER = 6, D3DRTYPE_INDEXBUFFER = 7, D3DRTYPE_FORCE_DWORD = 0x7fffffff } D3DRESOURCETYPE;
typedef enum _D3DMULTISAMPLE_TYPE { D3DMULTISAMPLE_NONE = 0, D3DMULTISAMPLE_NONMASKABLE = 1, D3DMULTISAMPLE_2_SAMPLES = 2, D3DMULTISAMPLE_4_SAMPLES = 4, D3DMULTISAMPLE_FORCE_DWORD = 0x7fffffff } D3DMULTISAMPLE_TYPE;
typedef enum _D3DSWAPEFFECT { D3DSWAPEFFECT_DISCARD = 1, D3DSWAPEFFECT_FLIP = 2, D3DSWAPEFFECT_COPY = 3, D3DSWAPEFFECT_OVERLAY = 4, D3DSWAPEFFECT_FLIPEX = 5, D3DSWAPEFFECT_FORCE_DWORD = 0x7fffffff } D3DSWAPEFFECT;
typedef enum _D3DDEVTYPE { D3DDEVTYPE_HAL = 1, D3DDEVTYPE_REF = 2, D3DDEVTYPE_SW = 3, D3DDEVTYPE_NULLREF = 4, D3DDEVTYPE_FORCE_DWORD = 0x7fffffff } D3DDEVTYPE;
typedef enum _D3DBACKBUFFER_TYPE { D3DBACKBUFFER_TYPE_MONO = 0, D3DBACKBUFFER_TYPE_LEFT = 1, D3DBACKBUFFER_TYPE_RIGHT = 2, D3DBACKBUFFER_TYPE_FORCE_DWORD = 0x7fffffff } D3DBACKBUFFER_TYPE;
typedef enum _D3DPRIMITIVETYPE { D3DPT_POINTLIST = 1, D3DPT_LINELIST = 2, D3DPT_LINESTRIP = 3, D3DPT_TRIANGLELIST = 4, D3DPT_TRIANGLESTRIP = 5, D3DPT_TRIANGLEFAN = 6, D3DPT_FORCE_DWORD = 0x7fffffff } D3DPRIMITIVETYPE;
typedef enum _D3DTRANSFORMSTATETYPE { D3DTS_VIEW = 2, D3DTS_PROJECTION = 3, D3DTS_TEXTURE0 = 16, D3DTS_WORLD = 256, D3DTS_FORCE_DWORD = 0x7fffffff } D3DTRANSFORMSTATETYPE;
#define D3DTS_WORLDMATRIX(index) (D3DTRANSFORMSTATETYPE)(index + 256)
typedef enum _D3DSCANLINEORDERING { D3DSCANLINEORDERING_UNKNOWN = 0, D3DSCANLINEORDERING_PROGRESSIVE = 1, D3DSCANLINEORDERING_INTERLACED = 2 } D3DSCANLINEORDERING;
typedef enum _D3DDISPLAYROTATION { D3DDISPLAYROTATION_IDENTITY = 1, D3DDISPLAYROTATION_90 = 2, D3DDISPLAYROTATION_180 = 3, D3DDISPLAYROTATION_270 = 4 } D3DDISPLAYROTATION;
typedef enum _D3DCOMPOSERECTSOP { D3DCOMPOSERECTS_COPY = 1, D3DCOMPOSERECTS_OR = 2, D3DCOMPOSERECTS_AND = 3, D3DCOMPOSERECTS_NEG = 4 } D3DCOMPOSERECTSOP;
typedef enum _D3DSTATEBLOCKTYPE { D3DSBT_ALL = 1, D3DSBT_PIXELSTATE = 2, D3DSBT_VERTEXSTATE = 3, D3DSBT_FORCE_DWORD = 0x7fffffff } D3DSTATEBLOCKTYPE;
typedef enum _D3DQUERYTYPE { D3DQUERYTYPE_VCACHE = 4, D3DQUERYTYPE_EVENT = 8, D3DQUERYTYPE_OCCLUSION = 9 } D3DQUERYTYPE;
typedef enum _D3DLIGHTTYPE { D3DLIGHT_POINT = 1, D3DLIGHT_SPOT = 2, D3DLIGHT_DIRECTIONAL = 3, D3DLIGHT_FORCE_DWORD = 0x7fffffff } D3DLIGHTTYPE;

typedef enum _D3DRENDERSTATETYPE
{
	D3DRS_ZENABLE = 7, D3DRS_FILLMODE = 8, D3DRS_SHADEMODE = 9, D3DRS_ZWRITEENABLE = 14, D3DRS_ALPHATESTENABLE = 15, D3DRS_LASTPIXEL = 16,
	D3DRS_SRCBLEND = 19, D3DRS_DESTBLEND = 20, D3DRS_CULLMODE = 22, D3DRS_ZFUNC = 23, D3DRS_ALPHAREF = 24, D3DRS_ALPHAFUNC = 25, D3DRS_DITHERENABLE = 26,
	D3DRS_ALPHABLENDENABLE = 27, D3DRS_FOGENABLE = 28, D3DRS_SPECULARENABLE = 29, D3DRS_FOGCOLOR = 34, D3DRS_FOGTABLEMODE = 35, D3DRS_FOGSTART = 36,
	D3DRS_FOGEND = 37, D3DRS_FOGDENSITY = 38, D3DRS_RANGEFOGENABLE = 48, D3DRS_STENCILENABLE = 52, D3DRS_STENCILFAIL = 53, D3DRS_STENCILZFAIL = 54,
	D3DRS_STENCILPASS = 55, D3DRS_STENCILFUNC = 56, D3DRS_STENCILREF = 57, D3DRS_STENCILMASK = 58, D3DRS_STENCILWRITEMASK = 59, D3DRS_TEXTUREFACTOR = 60,
	D3DRS_WRAP0 = 128, D3DRS_CLIPPING = 136, D3DRS_LIGHTING = 137, D3DRS_AMBIENT = 139, D3DRS_FOGVERTEXMODE = 140, D3DRS_COLORVERTEX = 141,
	D3DRS_LOCALVIEWER = 142, D3DRS_NORMALIZENORMALS = 143, D3DRS_DIFFUSEMATERIALSOURCE = 145, D3DRS_SPECULARMATERIALSOURCE = 146,
	D3DRS_AMBIENTMATERIALSOURCE = 147, D3DRS_EMISSIVEMATERIALSOURCE = 148, D3DRS_VERTEXBLEND = 151, D3DRS_CLIPPLANEENABLE = 152, D3DRS_POINTSIZE = 154,
	D3DRS_POINTSIZE_MIN = 155, D3DRS_POINTSPRITEENABLE = 156, D3DRS_POINTSCALEENABLE = 157, D3DRS_MULTISAMPLEANTIALIAS = 161, D3DRS_MULTISAMPLEMASK = 162,
	D3DRS_PATCHEDGESTYLE = 163, D3DRS_DEBUGMONITORTOKEN = 165, D3DRS_POINTSIZE_MAX = 166, D3DRS_INDEXEDVERTEXBLENDENABLE = 167, D3DRS_COLORWRITEENABLE = 168,
	D3DRS_TWEENFACTOR = 170, D3DRS_BLENDOP = 171, D3DRS_POSITIONDEGREE = 172, D3DRS_NORMALDEGREE = 173, D3DRS_SCISSORTESTENABLE = 174,
	D3DRS_SLOPESCALEDEPTHBIAS = 175, D3DRS_ANTIALIASEDLINEENABLE = 176, D3DRS_MINTESSELLATIONLEVEL = 178, D3DRS_MAXTESSELLATIONLEVEL = 179,
	D3DRS_TWOSIDEDSTENCILMODE = 185, D3DRS_COLORWRITEENABLE1 = 190, D3DRS_COLORWRITEENABLE2 = 191, D3DRS_COLORWRITEENABLE3 = 192, D3DRS_BLENDFACTOR = 193,
	D3DRS_SRGBWRITEENABLE = 194, D3DRS_DEPTHBIAS = 195, D3DRS_SEPARATEALPHABLENDENABLE = 206, D3DRS_SRCBLENDALPHA = 207, D3DRS_DESTBLENDALPHA = 208,
	D3DRS_BLENDOPALPHA = 209, D3DRS_FORCE_DWORD = 0x7fffffff
} D3DRENDERSTATETYPE;

typedef enum _D3DTEXTURESTAGESTATETYPE
{
	D3DTSS_COLOROP = 1, D3DTSS_COLORARG1 = 2, D3DTSS_COLORARG2 = 3, D3DTSS_ALPHAOP = 4, D3DTSS_ALPHAARG1 = 5, D3DTSS_ALPHAARG2 = 6,
	D3DTSS_BUMPENVMAT00 = 7, D3DTSS_BUMPENVMAT01 = 8, D3DTSS_BUMPENVMAT10 = 9, D3DTSS_BUMPENVMAT11 = 10, D3DTSS_TEXCOORDINDEX = 11,
	D3DTSS_BUMPENVLSCALE = 22, D3DTSS_BUMPENVLOFFSET = 23, D3DTSS_TEXTURETRANSFORMFLAGS = 24, D3DTSS_COLORARG0 = 26, D3DTSS_ALPHAARG0 = 27,
	D3DTSS_RESULTARG = 28, D3DTSS_CONSTANT = 32, D3DTSS_FORCE_DWORD = 0x7fffffff
} D3DTEXTURESTAGESTATETYPE;

typedef enum _D3DSAMPLERSTATETYPE
{
	D3DSAMP_ADDRESSU = 1, D3DSAMP_ADDRESSV = 2, D3DSAMP_ADDRESSW = 3, D3DSAMP_BORDERCOLOR = 4, D3DSAMP_MAGFILTER = 5, D3DSAMP_MINFILTER = 6,
	D3DSAMP_MIPFILTER = 7, D3DSAMP_MIPMAPLODBIAS = 8, D3DSAMP_MAXMIPLEVEL = 9, D3DSAMP_MAXANISOTROPY = 10, D3DSAMP_SRGBTEXTURE = 11,
	D3DSAMP_ELEMENTINDEX = 12, D3DSAMP_DMAPOFFSET = 13, D3DSAMP_FORCE_DWORD = 0x7fffffff
} D3DSAMPLERSTATETYPE;

typedef enum _D3DTEXTUREOP
{
	D3DTOP_DISABLE = 1, D3DTOP_SELECTARG1 = 2, D3DTOP_SELECTARG2 = 3, D3DTOP_MODULATE = 4, D3DTOP_MODULATE2X = 5, D3DTOP_MODULATE4X = 6,
	D3DTOP_ADD = 7, D3DTOP_ADDSIGNED = 8, D3DTOP_ADDSIGNED2X = 9, D3DTOP_SUBTRACT = 10, D3DTOP_ADDSMOOTH = 11, D3DTOP_BLENDDIFFUSEALPHA = 12,
	D3DTOP_BLENDTEXTUREALPHA = 13, D3DTOP_BLENDFACTORALPHA = 14, D3DTOP_BLENDTEXTUREALPHAPM = 15, D3DTOP_BLENDCURRENTALPHA = 16, D3DTOP_PREMODULATE = 17,
	D3DTOP_MODULATEALPHA_ADDCOLOR = 18, D3DTOP_MODULATECOLOR_ADDALPHA = 19, D3DTOP_MODULATEINVALPHA_ADDCOLOR = 20, D3DTOP_MODULATEINVCOLOR_ADDALPHA = 21,
	D3DTOP_BUMPENVMAP = 22, D3DTOP_BUMPENVMAPLUMINANCE = 23, D3DTOP_DOTPRODUCT3 = 24, D3DTOP_MULTIPLYADD = 25, D3DTOP_LERP = 26, D3DTOP_FORCE_DWORD = 0x7fffffff
} D3DTEXTUREOP;
#define D3DTA_SELECTMASK        0x0000000f
#define D3DTA_DIFFUSE           0x00000000
#define D3DTA_CURRENT           0x00000001
#define D3DTA_TEXTURE           0x00000002
#define D3DTA_TFACTOR           0x00000003
#define D3DTA_SPECULAR          0x00000004
#define D3DTA_TEMP              0x00000005
#define D3DTA_CONSTANT          0x00000006
#define D3DTA_COMPLEMENT        0x00000010
#define D3DTA_ALPHAREPLICATE    0x00000020
typedef enum _D3DTEXTUREFILTERTYPE { D3DTEXF_NONE = 0, D3DTEXF_POINT = 1, D3DTEXF_LINEAR = 2, D3DTEXF_ANISOTROPIC = 3, D3DTEXF_PYRAMIDALQUAD = 6, D3DTEXF_GAUSSIANQUAD = 7, D3DTEXF_FORCE_DWORD = 0x7fffffff } D3DTEXTUREFILTERTYPE;
typedef enum _D3DTEXTUREADDRESS { D3DTADDRESS_WRAP = 1, D3DTADDRESS_MIRROR = 2, D3DTADDRESS_CLAMP = 3, D3DTADDRESS_BORDER = 4, D3DTADDRESS_MIRRORONCE = 5, D3DTADDRESS_FORCE_DWORD = 0x7fffffff } D3DTEXTUREADDRESS;
typedef enum _D3DTEXTURETRANSFORMFLAGS { D3DTTFF_DISABLE = 0, D3DTTFF_COUNT1 = 1, D3DTTFF_COUNT2 = 2, D3DTTFF_COUNT3 = 3, D3DTTFF_COUNT4 = 4, D3DTTFF_PROJECTED = 256, D3DTTFF_FORCE_DWORD = 0x7fffffff } D3DTEXTURETRANSFORMFLAGS;
typedef enum _D3DBLEND
{
	D3DBLEND_ZERO = 1, D3DBLEND_ONE = 2, D3DBLEND_SRCCOLOR = 3, D3DBLEND_INVSRCCOLOR = 4, D3DBLEND_SRCALPHA = 5, D3DBLEND_INVSRCALPHA = 6,
	D3DBLEND_DESTALPHA = 7, D3DBLEND_INVDESTALPHA = 8, D3DBLEND_DESTCOLOR = 9, D3DBLEND_INVDESTCOLOR = 10, D3DBLEND_SRCALPHASAT = 11,
	D3DBLEND_BOTHSRCALPHA = 12, D3DBLEND_BOTHINVSRCALPHA = 13, D3DBLEND_BLENDFACTOR = 14, D3DBLEND_INVBLENDFACTOR = 15, D3DBLEND_FORCE_DWORD = 0x7fffffff
} D3DBLEND;
typedef enum _D3DBLENDOP { D3DBLENDOP_ADD = 1, D3DBLENDOP_SUBTRACT = 2, D3DBLENDOP_REVSUBTRACT = 3, D3DBLENDOP_MIN = 4, D3DBLENDOP_MAX = 5, D3DBLENDOP_FORCE_DWORD = 0x7fffffff } D3DBLENDOP;
typedef enum _D3DCULL { D3DCULL_NONE = 1, D3DCULL_CW = 2, D3DCULL_CCW = 3, D3DCULL_FORCE_DWORD = 0x7fffffff } D3DCULL;
typedef enum _D3DFILLMODE { D3DFILL_POINT = 1, D3DFILL_WIREFRAME = 2, D3DFILL_SOLID = 3, D3DFILL_FORCE_DWORD = 0x7fffffff } D3DFILLMODE;
typedef enum _D3DSHADEMODE { D3DSHADE_FLAT = 1, D3DSHADE_GOURAUD = 2, D3DSHADE_PHONG = 3, D3DSHADE_FORCE_DWORD = 0x7fffffff } D3DSHADEMODE;
typedef enum _D3DCMPFUNC { D3DCMP_NEVER = 1, D3DCMP_LESS = 2, D3DCMP_EQUAL = 3, D3DCMP_LESSEQUAL = 4, D3DCMP_GREATER = 5, D3DCMP_NOTEQUAL = 6, D3DCMP_GREATEREQUAL = 7, D3DCMP_ALWAYS = 8, D3DCMP_FORCE_DWORD = 0x7fffffff } D3DCMPFUNC;
typedef enum _D3DZBUFFERTYPE { D3DZB_FALSE = 0, D3DZB_TRUE = 1, D3DZB_USEW = 2, D3DZB_FORCE_DWORD = 0x7fffffff } D3DZBUFFERTYPE;
typedef enum _D3DFOGMODE { D3DFOG_NONE = 0, D3DFOG_EXP = 1, D3DFOG_EXP2 = 2, D3DFOG_LINEAR = 3, D3DFOG_FORCE_DWORD = 0x7fffffff } D3DFOGMODE;
typedef enum _D3DMATERIALCOLORSOURCE { D3DMCS_MATERIAL = 0, D3DMCS_COLOR1 = 1, D3DMCS_COLOR2 = 2, D3DMCS_FORCE_DWORD = 0x7fffffff } D3DMATERIALCOLORSOURCE;
typedef enum _D3DVERTEXBLENDFLAGS { D3DVBF_DISABLE = 0, D3DVBF_1WEIGHTS = 1, D3DVBF_2WEIGHTS = 2, D3DVBF_3WEIGHTS = 3, D3DVBF_TWEENING = 255, D3DVBF_0WEIGHTS = 256, D3DVBF_FORCE_DWORD = 0x7fffffff } D3DVERTEXBLENDFLAGS;

// co
#define D3DUSAGE_RENDERTARGET           0x00000001L
#define D3DUSAGE_DEPTHSTENCIL           0x00000002L
#define D3DUSAGE_DYNAMIC                0x00000200L
#define D3DUSAGE_WRITEONLY              0x00000008L
#define D3DUSAGE_SOFTWAREPROCESSING     0x00000010L
#define D3DUSAGE_AUTOGENMIPMAP          0x00000400L
#define D3DLOCK_READONLY                0x00000010L
#define D3DLOCK_DISCARD                 0x00002000L
#define D3DLOCK_NOOVERWRITE             0x00001000L
#define D3DLOCK_NOSYSLOCK               0x00000800L
#define D3DCLEAR_TARGET                 0x00000001l
#define D3DCLEAR_ZBUFFER                0x00000002l
#define D3DCLEAR_STENCIL                0x00000004l
#define D3DPRESENT_INTERVAL_DEFAULT     0x00000000L
#define D3DPRESENT_INTERVAL_ONE         0x00000001L
#define D3DPRESENT_INTERVAL_IMMEDIATE   0x80000000L
#define D3DPRESENT_RATE_DEFAULT         0x00000000
#define D3DPRESENTFLAG_LOCKABLE_BACKBUFFER 0x00000001
#define D3DPRESENT_DONOTWAIT            0x00000001L
#define D3DCREATE_FPU_PRESERVE          0x00000002L
#define D3DCREATE_MULTITHREADED         0x00000004L
#define D3DCREATE_PUREDEVICE            0x00000010L
#define D3DCREATE_SOFTWARE_VERTEXPROCESSING 0x00000020L
#define D3DCREATE_HARDWARE_VERTEXPROCESSING 0x00000040L
#define D3DCREATE_MIXED_VERTEXPROCESSING    0x00000080L
#define D3DSGR_NO_CALIBRATION           0x00000000L
#define D3DSGR_CALIBRATE                0x00000001L
#define D3DFVF_RESERVED0        0x001
#define D3DFVF_POSITION_MASK    0x400E
#define D3DFVF_XYZ              0x002
#define D3DFVF_XYZRHW           0x004
#define D3DFVF_XYZB1            0x006
#define D3DFVF_XYZB2            0x008
#define D3DFVF_XYZB3            0x00a
#define D3DFVF_XYZB4            0x00c
#define D3DFVF_XYZB5            0x00e
#define D3DFVF_XYZW             0x4002
#define D3DFVF_NORMAL           0x010
#define D3DFVF_PSIZE            0x020
#define D3DFVF_DIFFUSE          0x040
#define D3DFVF_SPECULAR         0x080
#define D3DFVF_TEXCOUNT_MASK    0xf00
#define D3DFVF_TEXCOUNT_SHIFT   8
#define D3DFVF_TEX0             0x000
#define D3DFVF_TEX1             0x100
#define D3DFVF_TEXTUREFORMAT2 0         // hai gia tri toa do texture (mac dinh)
#define D3DFVF_TEXTUREFORMAT1 3
#define D3DFVF_TEXTUREFORMAT3 1
#define D3DFVF_TEXTUREFORMAT4 2
#define D3DFVF_TEXCOORDSIZE3(CoordIndex) (D3DFVF_TEXTUREFORMAT3 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE2(CoordIndex) (D3DFVF_TEXTUREFORMAT2)
#define D3DFVF_TEXCOORDSIZE4(CoordIndex) (D3DFVF_TEXTUREFORMAT4 << (CoordIndex*2 + 16))
#define D3DFVF_TEXCOORDSIZE1(CoordIndex) (D3DFVF_TEXTUREFORMAT1 << (CoordIndex*2 + 16))
#define D3DFVF_TEX2             0x200
#define D3DFVF_LASTBETA_UBYTE4  0x1000
#define D3DFVF_LASTBETA_D3DCOLOR 0x8000
#define D3DVS_VERSION(_Major,_Minor) (0xFFFE0000|((_Major)<<8)|(_Minor))
#define D3DPS_VERSION(_Major,_Minor) (0xFFFF0000|((_Major)<<8)|(_Minor))
// caps (d3d9caps.h) - chi cac co Represent3 va shim dung
#define D3DCAPS2_FULLSCREENGAMMA        0x00020000L
#define D3DCAPS2_CANCALIBRATEGAMMA      0x00100000L
#define D3DCAPS2_DYNAMICTEXTURES        0x20000000L
#define D3DCAPS2_CANAUTOGENMIPMAP       0x40000000L
#define D3DDEVCAPS_TEXTUREVIDEOMEMORY   0x00000200L
#define D3DDEVCAPS_TLVERTEXVIDEOMEMORY  0x00000080L
#define D3DDEVCAPS_DRAWPRIMTLVERTEX     0x00000400L
#define D3DDEVCAPS_CANRENDERAFTERFLIP   0x00000800L
#define D3DDEVCAPS_DRAWPRIMITIVES2      0x00002000L
#define D3DDEVCAPS_DRAWPRIMITIVES2EX    0x00008000L
#define D3DDEVCAPS_HWTRANSFORMANDLIGHT  0x00010000L
#define D3DDEVCAPS_HWRASTERIZATION      0x00080000L
#define D3DDEVCAPS_PUREDEVICE           0x00100000L
#define D3DPMISCCAPS_MASKZ              0x00000002L
#define D3DPMISCCAPS_CULLNONE           0x00000010L
#define D3DPMISCCAPS_CULLCW             0x00000020L
#define D3DPMISCCAPS_CULLCCW            0x00000040L
#define D3DPMISCCAPS_COLORWRITEENABLE   0x00000080L
#define D3DPMISCCAPS_BLENDOP            0x00000800L
#define D3DPRASTERCAPS_ZTEST            0x00000010L
#define D3DPRASTERCAPS_SCISSORTEST      0x01000000L
#define D3DPSHADECAPS_COLORGOURAUDRGB   0x00000008L
#define D3DPSHADECAPS_ALPHAGOURAUDBLEND 0x00004000L
#define D3DPTEXTURECAPS_PERSPECTIVE     0x00000001L
#define D3DPTEXTURECAPS_POW2            0x00000002L
#define D3DPTEXTURECAPS_ALPHA           0x00000004L
#define D3DPTEXTURECAPS_SQUAREONLY      0x00000020L
#define D3DPTEXTURECAPS_NONPOW2CONDITIONAL 0x00000100L
#define D3DPTEXTURECAPS_MIPMAP          0x00004000L
#define D3DPTFILTERCAPS_MINFPOINT       0x00000100L
#define D3DPTFILTERCAPS_MINFLINEAR      0x00000200L
#define D3DPTFILTERCAPS_MIPFPOINT       0x00010000L
#define D3DPTFILTERCAPS_MIPFLINEAR      0x00020000L
#define D3DPTFILTERCAPS_MAGFPOINT       0x01000000L
#define D3DPTFILTERCAPS_MAGFLINEAR      0x02000000L
#define D3DPTADDRESSCAPS_WRAP           0x00000001L
#define D3DPTADDRESSCAPS_MIRROR         0x00000002L
#define D3DPTADDRESSCAPS_CLAMP          0x00000004L
#define D3DPTADDRESSCAPS_BORDER         0x00000008L
#define D3DPTADDRESSCAPS_INDEPENDENTUV  0x00000010L
#define D3DLINECAPS_TEXTURE             0x00000001L
#define D3DLINECAPS_ZTEST               0x00000002L
#define D3DLINECAPS_BLEND               0x00000004L
#define D3DLINECAPS_ALPHACMP            0x00000008L
#define D3DVTXPCAPS_TEXGEN              0x00000001L
#define D3DVTXPCAPS_MATERIALSOURCE7     0x00000002L
#define D3DVTXPCAPS_DIRECTIONALLIGHTS   0x00000008L
#define D3DVTXPCAPS_POSITIONALLIGHTS    0x00000010L
#define D3DVTXPCAPS_LOCALVIEWER         0x00000020L
#define D3DTEXOPCAPS_DISABLE            0x00000001L
#define D3DTEXOPCAPS_SELECTARG1         0x00000002L
#define D3DTEXOPCAPS_SELECTARG2         0x00000004L
#define D3DTEXOPCAPS_MODULATE           0x00000008L
#define D3DTEXOPCAPS_MODULATE2X         0x00000010L
#define D3DTEXOPCAPS_MODULATE4X         0x00000020L
#define D3DTEXOPCAPS_ADD                0x00000040L
#define D3DTEXOPCAPS_ADDSIGNED          0x00000080L
#define D3DTEXOPCAPS_ADDSIGNED2X        0x00000100L
#define D3DTEXOPCAPS_SUBTRACT           0x00000200L
#define D3DTEXOPCAPS_ADDSMOOTH          0x00000400L
#define D3DTEXOPCAPS_BLENDDIFFUSEALPHA  0x00000800L
#define D3DTEXOPCAPS_BLENDTEXTUREALPHA  0x00001000L

// ---------------------------------------------------------------- struct
typedef struct _D3DVECTOR { float x, y, z; } D3DVECTOR;
typedef struct _D3DCOLORVALUE { float r, g, b, a; } D3DCOLORVALUE;
typedef struct _D3DRECT { LONG x1, y1, x2, y2; } D3DRECT;
typedef struct _D3DMATRIX
{
	union
	{
		struct { float _11, _12, _13, _14; float _21, _22, _23, _24; float _31, _32, _33, _34; float _41, _42, _43, _44; };
		float m[4][4];
	};
} D3DMATRIX;
typedef struct _D3DVIEWPORT9 { DWORD X, Y, Width, Height; float MinZ, MaxZ; } D3DVIEWPORT9;
typedef struct _D3DMATERIAL9 { D3DCOLORVALUE Diffuse, Ambient, Specular, Emissive; float Power; } D3DMATERIAL9;
typedef struct _D3DLIGHT9 { D3DLIGHTTYPE Type; D3DCOLORVALUE Diffuse, Specular, Ambient; D3DVECTOR Position, Direction; float Range, Falloff, Attenuation0, Attenuation1, Attenuation2, Theta, Phi; } D3DLIGHT9;
typedef struct _D3DCLIPSTATUS9 { DWORD ClipUnion, ClipIntersection; } D3DCLIPSTATUS9;
typedef struct _D3DLOCKED_RECT { INT Pitch; void* pBits; } D3DLOCKED_RECT;
typedef struct _D3DSURFACE_DESC { D3DFORMAT Format; D3DRESOURCETYPE Type; DWORD Usage; D3DPOOL Pool; D3DMULTISAMPLE_TYPE MultiSampleType; DWORD MultiSampleQuality; UINT Width, Height; } D3DSURFACE_DESC;
typedef struct _D3DVERTEXBUFFER_DESC { D3DFORMAT Format; D3DRESOURCETYPE Type; DWORD Usage; D3DPOOL Pool; UINT Size; DWORD FVF; } D3DVERTEXBUFFER_DESC;
typedef struct _D3DDISPLAYMODE { UINT Width, Height, RefreshRate; D3DFORMAT Format; } D3DDISPLAYMODE;
typedef struct D3DDISPLAYMODEEX { UINT Size, Width, Height, RefreshRate; D3DFORMAT Format; D3DSCANLINEORDERING ScanLineOrdering; } D3DDISPLAYMODEEX;
typedef struct D3DDISPLAYMODEFILTER { UINT Size; D3DFORMAT Format; D3DSCANLINEORDERING ScanLineOrdering; } D3DDISPLAYMODEFILTER;
typedef struct _D3DPRESENT_PARAMETERS_
{
	UINT BackBufferWidth, BackBufferHeight; D3DFORMAT BackBufferFormat; UINT BackBufferCount; D3DMULTISAMPLE_TYPE MultiSampleType; DWORD MultiSampleQuality;
	D3DSWAPEFFECT SwapEffect; HWND hDeviceWindow; BOOL Windowed; BOOL EnableAutoDepthStencil; D3DFORMAT AutoDepthStencilFormat; DWORD Flags;
	UINT FullScreen_RefreshRateInHz; UINT PresentationInterval;
} D3DPRESENT_PARAMETERS;
typedef struct _D3DGAMMARAMP { WORD red[256]; WORD green[256]; WORD blue[256]; } D3DGAMMARAMP;
typedef struct _D3DRASTER_STATUS { BOOL InVBlank; UINT ScanLine; } D3DRASTER_STATUS;
typedef struct _D3DDEVICE_CREATION_PARAMETERS { UINT AdapterOrdinal; D3DDEVTYPE DeviceType; HWND hFocusWindow; DWORD BehaviorFlags; } D3DDEVICE_CREATION_PARAMETERS;
typedef struct _D3DADAPTER_IDENTIFIER9
{
	char Driver[MAX_DEVICE_IDENTIFIER_STRING]; char Description[MAX_DEVICE_IDENTIFIER_STRING]; char DeviceName[32];
	unsigned long long DriverVersion; DWORD VendorId, DeviceId, SubSysId, Revision; GUID DeviceIdentifier; DWORD WHQLLevel;
} D3DADAPTER_IDENTIFIER9;
typedef struct _D3DVERTEXELEMENT9 { WORD Stream, Offset; BYTE Type, Method, Usage, UsageIndex; } D3DVERTEXELEMENT9;
typedef struct _D3DRECTPATCH_INFO { UINT StartVertexOffsetWidth, StartVertexOffsetHeight, Width, Height, Stride; DWORD Basis; DWORD Degree; } D3DRECTPATCH_INFO;
typedef struct _D3DTRIPATCH_INFO { UINT StartVertexOffset, NumVertices; DWORD Basis; DWORD Degree; } D3DTRIPATCH_INFO;
typedef struct _D3DVSHADERCAPS2_0 { DWORD Caps; INT DynamicFlowControlDepth; INT NumTemps; INT StaticFlowControlDepth; } D3DVSHADERCAPS2_0;
typedef struct _D3DPSHADERCAPS2_0 { DWORD Caps; INT DynamicFlowControlDepth; INT NumTemps; INT StaticFlowControlDepth; INT NumInstructionSlots; } D3DPSHADERCAPS2_0;
typedef struct _D3DCAPS9
{
	D3DDEVTYPE DeviceType; UINT AdapterOrdinal; DWORD Caps, Caps2, Caps3, PresentationIntervals, CursorCaps, DevCaps, PrimitiveMiscCaps, RasterCaps,
	ZCmpCaps, SrcBlendCaps, DestBlendCaps, AlphaCmpCaps, ShadeCaps, TextureCaps, TextureFilterCaps, CubeTextureFilterCaps, VolumeTextureFilterCaps,
	TextureAddressCaps, VolumeTextureAddressCaps, LineCaps, MaxTextureWidth, MaxTextureHeight, MaxVolumeExtent, MaxTextureRepeat, MaxTextureAspectRatio,
	MaxAnisotropy; float MaxVertexW, GuardBandLeft, GuardBandTop, GuardBandRight, GuardBandBottom, ExtentsAdjust; DWORD StencilCaps, FVFCaps, TextureOpCaps,
	MaxTextureBlendStages, MaxSimultaneousTextures, VertexProcessingCaps, MaxActiveLights, MaxUserClipPlanes, MaxVertexBlendMatrices, MaxVertexBlendMatrixIndex;
	float MaxPointSize; DWORD MaxPrimitiveCount, MaxVertexIndex, MaxStreams, MaxStreamStride, VertexShaderVersion, MaxVertexShaderConst, PixelShaderVersion;
	float PixelShader1xMaxValue; DWORD DevCaps2; float MaxNpatchTessellationLevel; DWORD Reserved5; UINT MasterAdapterOrdinal, AdapterOrdinalInGroup,
	NumberOfAdaptersInGroup; DWORD DeclTypes, NumSimultaneousRTs, StretchRectFilterCaps; D3DVSHADERCAPS2_0 VS20Caps; D3DPSHADERCAPS2_0 PS20Caps;
	DWORD VertexTextureFilterCaps, MaxVShaderInstructionsExecuted, MaxPShaderInstructionsExecuted, MaxVertexShader30InstructionSlots, MaxPixelShader30InstructionSlots;
} D3DCAPS9;

// ---------------------------------------------------------------- GUID
#ifndef _WIN32
#endif
#define JX_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) static const GUID name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }
JX_DEFINE_GUID(IID_IDirect3D9,             0x81bdcbca, 0x64d4, 0x426d, 0xae, 0x8d, 0xad, 0x01, 0x47, 0xf4, 0x27, 0x5c);
JX_DEFINE_GUID(IID_IDirect3DDevice9,       0xd0223b96, 0xbf7a, 0x43fd, 0x92, 0xbd, 0xa4, 0x3b, 0x0d, 0x82, 0xb9, 0xeb);
JX_DEFINE_GUID(IID_IDirect3DResource9,     0x05eec05d, 0x8f7d, 0x4362, 0xb9, 0x99, 0xd1, 0xba, 0xf3, 0x57, 0xc7, 0x04);
JX_DEFINE_GUID(IID_IDirect3DBaseTexture9,  0x580ca87e, 0x1d3c, 0x4d54, 0x99, 0x1d, 0xb7, 0xd3, 0xe3, 0xc2, 0x98, 0xce);
JX_DEFINE_GUID(IID_IDirect3DTexture9,      0x85c31227, 0x3de5, 0x4f00, 0x9b, 0x3a, 0xf1, 0x1a, 0xc3, 0x8c, 0x18, 0xb5);
JX_DEFINE_GUID(IID_IDirect3DSurface9,      0x0cfbaf3a, 0x9ff6, 0x429a, 0x99, 0xb3, 0xa2, 0x79, 0x6a, 0xf8, 0xb8, 0x9b);
JX_DEFINE_GUID(IID_IDirect3DVertexBuffer9, 0xb64bb1b5, 0xfd70, 0x4df6, 0xbf, 0x91, 0x19, 0xd0, 0xa1, 0x24, 0x55, 0xe3);
JX_DEFINE_GUID(IID_IDirect3DStateBlock9,   0xb07c4fe5, 0x310d, 0x4ba8, 0xa2, 0x3c, 0x4f, 0x0f, 0x20, 0x6f, 0x21, 0x8b);

// ---------------------------------------------------------------- giao dien (thu tu vtable nhu d3d9.h)
/* IUnknown: KPosixCompat.h (POSIX) / unknwn.h (Windows) */
struct IDirect3D9; struct IDirect3DDevice9; struct IDirect3DResource9; struct IDirect3DBaseTexture9; struct IDirect3DTexture9; struct IDirect3DSurface9;
struct IDirect3DVertexBuffer9; struct IDirect3DIndexBuffer9; struct IDirect3DStateBlock9; struct IDirect3DSwapChain9; struct IDirect3DVolumeTexture9;
struct IDirect3DCubeTexture9; struct IDirect3DVertexDeclaration9; struct IDirect3DVertexShader9; struct IDirect3DPixelShader9; struct IDirect3DQuery9;
struct IDirect3D9Ex; struct IDirect3DDevice9Ex;
typedef IDirect3D9* LPDIRECT3D9; typedef IDirect3DDevice9* LPDIRECT3DDEVICE9; typedef IDirect3DTexture9* LPDIRECT3DTEXTURE9; typedef IDirect3DSurface9* LPDIRECT3DSURFACE9;
typedef IDirect3DVertexBuffer9* LPDIRECT3DVERTEXBUFFER9; typedef IDirect3D9Ex* LPDIRECT3D9EX;
typedef IDirect3DStateBlock9* LPDIRECT3DSTATEBLOCK9; typedef IDirect3DBaseTexture9* LPDIRECT3DBASETEXTURE9; typedef IDirect3DIndexBuffer9* LPDIRECT3DINDEXBUFFER9;
typedef IDirect3DDevice9Ex* LPDIRECT3DDEVICE9EX; typedef IDirect3DSwapChain9* LPDIRECT3DSWAPCHAIN9;

struct IDirect3DResource9 : public IUnknown
{
	virtual HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice) = 0;
	virtual HRESULT __stdcall SetPrivateData(REFGUID refguid, CONST void* pData, DWORD SizeOfData, DWORD Flags) = 0;
	virtual HRESULT __stdcall GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) = 0;
	virtual HRESULT __stdcall FreePrivateData(REFGUID refguid) = 0;
	virtual DWORD   __stdcall SetPriority(DWORD PriorityNew) = 0;
	virtual DWORD   __stdcall GetPriority() = 0;
	virtual void    __stdcall PreLoad() = 0;
	virtual D3DRESOURCETYPE __stdcall GetType() = 0;
};
struct IDirect3DBaseTexture9 : public IDirect3DResource9
{
	virtual DWORD   __stdcall SetLOD(DWORD LODNew) = 0;
	virtual DWORD   __stdcall GetLOD() = 0;
	virtual DWORD   __stdcall GetLevelCount() = 0;
	virtual HRESULT __stdcall SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType) = 0;
	virtual D3DTEXTUREFILTERTYPE __stdcall GetAutoGenFilterType() = 0;
	virtual void    __stdcall GenerateMipSubLevels() = 0;
};
struct IDirect3DTexture9 : public IDirect3DBaseTexture9
{
	virtual HRESULT __stdcall GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc) = 0;
	virtual HRESULT __stdcall GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel) = 0;
	virtual HRESULT __stdcall LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) = 0;
	virtual HRESULT __stdcall UnlockRect(UINT Level) = 0;
	virtual HRESULT __stdcall AddDirtyRect(CONST RECT* pDirtyRect) = 0;
};
struct IDirect3DSurface9 : public IDirect3DResource9
{
	virtual HRESULT __stdcall GetContainer(REFIID riid, void** ppContainer) = 0;
	virtual HRESULT __stdcall GetDesc(D3DSURFACE_DESC* pDesc) = 0;
	virtual HRESULT __stdcall LockRect(D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) = 0;
	virtual HRESULT __stdcall UnlockRect() = 0;
	virtual HRESULT __stdcall GetDC(HDC* phdc) = 0;
	virtual HRESULT __stdcall ReleaseDC(HDC hdc) = 0;
};
struct IDirect3DVertexBuffer9 : public IDirect3DResource9
{
	virtual HRESULT __stdcall Lock(UINT OffsetToLock, UINT SizeToLock, void** ppbData, DWORD Flags) = 0;
	virtual HRESULT __stdcall Unlock() = 0;
	virtual HRESULT __stdcall GetDesc(D3DVERTEXBUFFER_DESC* pDesc) = 0;
};
struct IDirect3DStateBlock9 : public IUnknown
{
	virtual HRESULT __stdcall GetDevice(IDirect3DDevice9** ppDevice) = 0;
	virtual HRESULT __stdcall Capture() = 0;
	virtual HRESULT __stdcall Apply() = 0;
};
struct IDirect3D9 : public IUnknown
{
	virtual HRESULT __stdcall RegisterSoftwareDevice(void* pInitializeFunction) = 0;
	virtual UINT    __stdcall GetAdapterCount() = 0;
	virtual HRESULT __stdcall GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) = 0;
	virtual UINT    __stdcall GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) = 0;
	virtual HRESULT __stdcall EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) = 0;
	virtual HRESULT __stdcall GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) = 0;
	virtual HRESULT __stdcall CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) = 0;
	virtual HRESULT __stdcall CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) = 0;
	virtual HRESULT __stdcall CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) = 0;
	virtual HRESULT __stdcall CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) = 0;
	virtual HRESULT __stdcall CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) = 0;
	virtual HRESULT __stdcall GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) = 0;
	virtual HMONITOR __stdcall GetAdapterMonitor(UINT Adapter) = 0;
	virtual HRESULT __stdcall CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface) = 0;
};
struct IDirect3DDevice9 : public IUnknown
{
	virtual HRESULT __stdcall TestCooperativeLevel() = 0;
	virtual UINT    __stdcall GetAvailableTextureMem() = 0;
	virtual HRESULT __stdcall EvictManagedResources() = 0;
	virtual HRESULT __stdcall GetDirect3D(IDirect3D9** ppD3D9) = 0;
	virtual HRESULT __stdcall GetDeviceCaps(D3DCAPS9* pCaps) = 0;
	virtual HRESULT __stdcall GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) = 0;
	virtual HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters) = 0;
	virtual HRESULT __stdcall SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap) = 0;
	virtual void    __stdcall SetCursorPosition(int X, int Y, DWORD Flags) = 0;
	virtual BOOL    __stdcall ShowCursor(BOOL bShow) = 0;
	virtual HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain) = 0;
	virtual HRESULT __stdcall GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) = 0;
	virtual UINT    __stdcall GetNumberOfSwapChains() = 0;
	virtual HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) = 0;
	virtual HRESULT __stdcall Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) = 0;
	virtual HRESULT __stdcall GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) = 0;
	virtual HRESULT __stdcall GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) = 0;
	virtual HRESULT __stdcall SetDialogBoxMode(BOOL bEnableDialogs) = 0;
	virtual void    __stdcall SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp) = 0;
	virtual void    __stdcall GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) = 0;
	virtual HRESULT __stdcall CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) = 0;
	virtual HRESULT __stdcall CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle) = 0;
	virtual HRESULT __stdcall CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle) = 0;
	virtual HRESULT __stdcall CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle) = 0;
	virtual HRESULT __stdcall CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) = 0;
	virtual HRESULT __stdcall CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) = 0;
	virtual HRESULT __stdcall CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) = 0;
	virtual HRESULT __stdcall UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint) = 0;
	virtual HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture) = 0;
	virtual HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface) = 0;
	virtual HRESULT __stdcall GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) = 0;
	virtual HRESULT __stdcall StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) = 0;
	virtual HRESULT __stdcall ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color) = 0;
	virtual HRESULT __stdcall CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) = 0;
	virtual HRESULT __stdcall SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) = 0;
	virtual HRESULT __stdcall GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget) = 0;
	virtual HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) = 0;
	virtual HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) = 0;
	virtual HRESULT __stdcall BeginScene() = 0;
	virtual HRESULT __stdcall EndScene() = 0;
	virtual HRESULT __stdcall Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) = 0;
	virtual HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix) = 0;
	virtual HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) = 0;
	virtual HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*) = 0;
	virtual HRESULT __stdcall SetViewport(CONST D3DVIEWPORT9* pViewport) = 0;
	virtual HRESULT __stdcall GetViewport(D3DVIEWPORT9* pViewport) = 0;
	virtual HRESULT __stdcall SetMaterial(CONST D3DMATERIAL9* pMaterial) = 0;
	virtual HRESULT __stdcall GetMaterial(D3DMATERIAL9* pMaterial) = 0;
	virtual HRESULT __stdcall SetLight(DWORD Index, CONST D3DLIGHT9*) = 0;
	virtual HRESULT __stdcall GetLight(DWORD Index, D3DLIGHT9*) = 0;
	virtual HRESULT __stdcall LightEnable(DWORD Index, BOOL Enable) = 0;
	virtual HRESULT __stdcall GetLightEnable(DWORD Index, BOOL* pEnable) = 0;
	virtual HRESULT __stdcall SetClipPlane(DWORD Index, CONST float* pPlane) = 0;
	virtual HRESULT __stdcall GetClipPlane(DWORD Index, float* pPlane) = 0;
	virtual HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) = 0;
	virtual HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) = 0;
	virtual HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB) = 0;
	virtual HRESULT __stdcall BeginStateBlock() = 0;
	virtual HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9** ppSB) = 0;
	virtual HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus) = 0;
	virtual HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9* pClipStatus) = 0;
	virtual HRESULT __stdcall GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture) = 0;
	virtual HRESULT __stdcall SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) = 0;
	virtual HRESULT __stdcall GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) = 0;
	virtual HRESULT __stdcall SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) = 0;
	virtual HRESULT __stdcall GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) = 0;
	virtual HRESULT __stdcall SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) = 0;
	virtual HRESULT __stdcall ValidateDevice(DWORD* pNumPasses) = 0;
	virtual HRESULT __stdcall SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries) = 0;
	virtual HRESULT __stdcall GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) = 0;
	virtual HRESULT __stdcall SetCurrentTexturePalette(UINT PaletteNumber) = 0;
	virtual HRESULT __stdcall GetCurrentTexturePalette(UINT* PaletteNumber) = 0;
	virtual HRESULT __stdcall SetScissorRect(CONST RECT* pRect) = 0;
	virtual HRESULT __stdcall GetScissorRect(RECT* pRect) = 0;
	virtual HRESULT __stdcall SetSoftwareVertexProcessing(BOOL bSoftware) = 0;
	virtual BOOL    __stdcall GetSoftwareVertexProcessing() = 0;
	virtual HRESULT __stdcall SetNPatchMode(float nSegments) = 0;
	virtual float   __stdcall GetNPatchMode() = 0;
	virtual HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) = 0;
	virtual HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) = 0;
	virtual HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) = 0;
	virtual HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) = 0;
	virtual HRESULT __stdcall ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) = 0;
	virtual HRESULT __stdcall CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) = 0;
	virtual HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) = 0;
	virtual HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) = 0;
	virtual HRESULT __stdcall SetFVF(DWORD FVF) = 0;
	virtual HRESULT __stdcall GetFVF(DWORD* pFVF) = 0;
	virtual HRESULT __stdcall CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader) = 0;
	virtual HRESULT __stdcall SetVertexShader(IDirect3DVertexShader9* pShader) = 0;
	virtual HRESULT __stdcall GetVertexShader(IDirect3DVertexShader9** ppShader) = 0;
	virtual HRESULT __stdcall SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) = 0;
	virtual HRESULT __stdcall GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) = 0;
	virtual HRESULT __stdcall SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) = 0;
	virtual HRESULT __stdcall GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) = 0;
	virtual HRESULT __stdcall SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) = 0;
	virtual HRESULT __stdcall GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) = 0;
	virtual HRESULT __stdcall SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride) = 0;
	virtual HRESULT __stdcall GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride) = 0;
	virtual HRESULT __stdcall SetStreamSourceFreq(UINT StreamNumber, UINT Setting) = 0;
	virtual HRESULT __stdcall GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting) = 0;
	virtual HRESULT __stdcall SetIndices(IDirect3DIndexBuffer9* pIndexData) = 0;
	virtual HRESULT __stdcall GetIndices(IDirect3DIndexBuffer9** ppIndexData) = 0;
	virtual HRESULT __stdcall CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader) = 0;
	virtual HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9* pShader) = 0;
	virtual HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9** ppShader) = 0;
	virtual HRESULT __stdcall SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) = 0;
	virtual HRESULT __stdcall GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) = 0;
	virtual HRESULT __stdcall SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) = 0;
	virtual HRESULT __stdcall GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) = 0;
	virtual HRESULT __stdcall SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) = 0;
	virtual HRESULT __stdcall GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) = 0;
	virtual HRESULT __stdcall DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo) = 0;
	virtual HRESULT __stdcall DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo) = 0;
	virtual HRESULT __stdcall DeletePatch(UINT Handle) = 0;
	virtual HRESULT __stdcall CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) = 0;
};
// D3D9Ex (Vista+): D3D_Shell.cpp/D3D_Device.cpp dung khi g_nRep3ExOn; thu tu vtable dung nhu d3d9.h
struct IDirect3D9Ex : public IDirect3D9
{
	virtual UINT    __stdcall GetAdapterModeCountEx(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter) = 0;
	virtual HRESULT __stdcall EnumAdapterModesEx(UINT Adapter, CONST D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode) = 0;
	virtual HRESULT __stdcall GetAdapterDisplayModeEx(UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation) = 0;
	virtual HRESULT __stdcall CreateDeviceEx(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface) = 0;
	virtual HRESULT __stdcall GetAdapterLUID(UINT Adapter, LUID* pLUID) = 0;
};
struct IDirect3DDevice9Ex : public IDirect3DDevice9
{
	virtual HRESULT __stdcall SetConvolutionMonoKernel(UINT width, UINT height, float* rows, float* columns) = 0;
	virtual HRESULT __stdcall ComposeRects(IDirect3DSurface9* pSrc, IDirect3DSurface9* pDst, IDirect3DVertexBuffer9* pSrcRectDescs, UINT NumRects, IDirect3DVertexBuffer9* pDstRectDescs, D3DCOMPOSERECTSOP Operation, int Xoffset, int Yoffset) = 0;
	virtual HRESULT __stdcall PresentEx(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion, DWORD dwFlags) = 0;
	virtual HRESULT __stdcall GetGPUThreadPriority(INT* pPriority) = 0;
	virtual HRESULT __stdcall SetGPUThreadPriority(INT Priority) = 0;
	virtual HRESULT __stdcall WaitForVBlank(UINT iSwapChain) = 0;
	virtual HRESULT __stdcall CheckResourceResidency(IDirect3DResource9** pResourceArray, UINT32 NumResources) = 0;
	virtual HRESULT __stdcall SetMaximumFrameLatency(UINT MaxLatency) = 0;
	virtual HRESULT __stdcall GetMaximumFrameLatency(UINT* pMaxLatency) = 0;
	virtual HRESULT __stdcall CheckDeviceState(HWND hDestinationWindow) = 0;
	virtual HRESULT __stdcall CreateRenderTargetEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) = 0;
	virtual HRESULT __stdcall CreateOffscreenPlainSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) = 0;
	virtual HRESULT __stdcall CreateDepthStencilSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) = 0;
	virtual HRESULT __stdcall ResetEx(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode) = 0;
	virtual HRESULT __stdcall GetDisplayModeEx(UINT iSwapChain, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation) = 0;
};
// khong co D3D9 that o day: tra NULL / that bai -> D3D_Shell lui ve / bat buoc Rep3Api=100
inline IDirect3D9* Direct3DCreate9(UINT) { return (IDirect3D9*)0; }
inline HRESULT Direct3DCreate9Ex(UINT, IDirect3D9Ex** pp) { if (pp) *pp = (IDirect3D9Ex*)0; return E_FAIL; }

// ---------------------------------------------------------------- d3dx9 tap con
#define D3DPCMPCAPS_NEVER        0x00000001
#define D3DPCMPCAPS_LESS         0x00000002
#define D3DPCMPCAPS_EQUAL        0x00000004
#define D3DPCMPCAPS_LESSEQUAL    0x00000008
#define D3DPCMPCAPS_GREATER      0x00000010
#define D3DPCMPCAPS_NOTEQUAL     0x00000020
#define D3DPCMPCAPS_GREATEREQUAL 0x00000040
#define D3DPCMPCAPS_ALWAYS       0x00000080
#define D3DX_PI            ((float)3.141592654f)
#define D3DXToRadian(degree) ((degree) * (D3DX_PI / 180.0f))
#define D3DXToDegree(radian) ((radian) * (180.0f / D3DX_PI))
#define D3DX_FILTER_NONE   (1 << 0)
#define D3DX_DEFAULT       ((UINT)-1)
struct D3DXVECTOR3 : public D3DVECTOR
{
	D3DXVECTOR3() {}
	D3DXVECTOR3(float fx, float fy, float fz) { x = fx; y = fy; z = fz; }
	D3DXVECTOR3(const D3DVECTOR& v) { x = v.x; y = v.y; z = v.z; }
	operator float*() { return &x; }
	operator const float*() const { return &x; }
	D3DXVECTOR3& operator+=(const D3DXVECTOR3& v) { x += v.x; y += v.y; z += v.z; return *this; }
	D3DXVECTOR3& operator-=(const D3DXVECTOR3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	D3DXVECTOR3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
	D3DXVECTOR3& operator/=(float f) { float r = 1.0f / f; x *= r; y *= r; z *= r; return *this; }
	D3DXVECTOR3 operator+() const { return *this; }
	D3DXVECTOR3 operator-() const { return D3DXVECTOR3(-x, -y, -z); }
	D3DXVECTOR3 operator+(const D3DXVECTOR3& v) const { return D3DXVECTOR3(x + v.x, y + v.y, z + v.z); }
	D3DXVECTOR3 operator-(const D3DXVECTOR3& v) const { return D3DXVECTOR3(x - v.x, y - v.y, z - v.z); }
	D3DXVECTOR3 operator*(float f) const { return D3DXVECTOR3(x * f, y * f, z * f); }
	D3DXVECTOR3 operator/(float f) const { float r = 1.0f / f; return D3DXVECTOR3(x * r, y * r, z * r); }
	friend D3DXVECTOR3 operator*(float f, const D3DXVECTOR3& v) { return D3DXVECTOR3(v.x * f, v.y * f, v.z * f); }
	bool operator==(const D3DXVECTOR3& v) const { return x == v.x && y == v.y && z == v.z; }
	bool operator!=(const D3DXVECTOR3& v) const { return !(*this == v); }
};
struct D3DXVECTOR4
{
	float x, y, z, w;
	D3DXVECTOR4() {}
	D3DXVECTOR4(float fx, float fy, float fz, float fw) { x = fx; y = fy; z = fz; w = fw; }
	operator float*() { return &x; }
	operator const float*() const { return &x; }
	D3DXVECTOR4& operator+=(const D3DXVECTOR4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	D3DXVECTOR4& operator-=(const D3DXVECTOR4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	D3DXVECTOR4& operator*=(float f) { x *= f; y *= f; z *= f; w *= f; return *this; }
	D3DXVECTOR4 operator-() const { return D3DXVECTOR4(-x, -y, -z, -w); }
	D3DXVECTOR4 operator+(const D3DXVECTOR4& v) const { return D3DXVECTOR4(x + v.x, y + v.y, z + v.z, w + v.w); }
	D3DXVECTOR4 operator-(const D3DXVECTOR4& v) const { return D3DXVECTOR4(x - v.x, y - v.y, z - v.z, w - v.w); }
	D3DXVECTOR4 operator*(float f) const { return D3DXVECTOR4(x * f, y * f, z * f, w * f); }
	friend D3DXVECTOR4 operator*(float f, const D3DXVECTOR4& v) { return D3DXVECTOR4(v.x * f, v.y * f, v.z * f, v.w * f); }
	bool operator==(const D3DXVECTOR4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
	bool operator!=(const D3DXVECTOR4& v) const { return !(*this == v); }
};
struct D3DXMATRIX;
D3DXMATRIX* D3DXMatrixMultiply(D3DXMATRIX* pOut, const D3DXMATRIX* pM1, const D3DXMATRIX* pM2);
struct D3DXMATRIX : public D3DMATRIX
{
	D3DXMATRIX() {}
	D3DXMATRIX(const D3DMATRIX& mat) { memcpy(m, mat.m, sizeof(m)); }
	D3DXMATRIX(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
	{ _11 = m11; _12 = m12; _13 = m13; _14 = m14; _21 = m21; _22 = m22; _23 = m23; _24 = m24; _31 = m31; _32 = m32; _33 = m33; _34 = m34; _41 = m41; _42 = m42; _43 = m43; _44 = m44; }
	float& operator()(UINT r, UINT c) { return m[r][c]; }
	float operator()(UINT r, UINT c) const { return m[r][c]; }
	operator float*() { return &_11; }
	operator const float*() const { return &_11; }
	D3DXMATRIX operator*(const D3DXMATRIX& b) const { D3DXMATRIX r; D3DXMatrixMultiply(&r, this, &b); return r; }
	D3DXMATRIX& operator*=(const D3DXMATRIX& b) { D3DXMATRIX r; D3DXMatrixMultiply(&r, this, &b); *this = r; return *this; }
};
inline D3DXMATRIX* D3DXMatrixIdentity(D3DXMATRIX* pOut)
{
	memset(pOut->m, 0, sizeof(pOut->m)); pOut->_11 = pOut->_22 = pOut->_33 = pOut->_44 = 1.0f; return pOut;
}
inline D3DXMATRIX* D3DXMatrixMultiply(D3DXMATRIX* pOut, const D3DXMATRIX* pM1, const D3DXMATRIX* pM2)
{
	D3DXMATRIX r;
	for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++)
		r.m[i][j] = pM1->m[i][0] * pM2->m[0][j] + pM1->m[i][1] * pM2->m[1][j] + pM1->m[i][2] * pM2->m[2][j] + pM1->m[i][3] * pM2->m[3][j];
	*pOut = r; return pOut;
}
inline D3DXMATRIX* D3DXMatrixTranspose(D3DXMATRIX* pOut, const D3DXMATRIX* pM)
{
	D3DXMATRIX r; for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) r.m[i][j] = pM->m[j][i]; *pOut = r; return pOut;
}
// nghich dao 4x4 tong quat (nhu D3DXMatrixInverse; tra NULL neu suy bien)
inline D3DXMATRIX* D3DXMatrixInverse(D3DXMATRIX* pOut, float* pDeterminant, const D3DXMATRIX* pM)
{
	const float* a = &pM->_11; float inv[16];
	inv[0] = a[5] * a[10] * a[15] - a[5] * a[11] * a[14] - a[9] * a[6] * a[15] + a[9] * a[7] * a[14] + a[13] * a[6] * a[11] - a[13] * a[7] * a[10];
	inv[4] = -a[4] * a[10] * a[15] + a[4] * a[11] * a[14] + a[8] * a[6] * a[15] - a[8] * a[7] * a[14] - a[12] * a[6] * a[11] + a[12] * a[7] * a[10];
	inv[8] = a[4] * a[9] * a[15] - a[4] * a[11] * a[13] - a[8] * a[5] * a[15] + a[8] * a[7] * a[13] + a[12] * a[5] * a[11] - a[12] * a[7] * a[9];
	inv[12] = -a[4] * a[9] * a[14] + a[4] * a[10] * a[13] + a[8] * a[5] * a[14] - a[8] * a[6] * a[13] - a[12] * a[5] * a[10] + a[12] * a[6] * a[9];
	inv[1] = -a[1] * a[10] * a[15] + a[1] * a[11] * a[14] + a[9] * a[2] * a[15] - a[9] * a[3] * a[14] - a[13] * a[2] * a[11] + a[13] * a[3] * a[10];
	inv[5] = a[0] * a[10] * a[15] - a[0] * a[11] * a[14] - a[8] * a[2] * a[15] + a[8] * a[3] * a[14] + a[12] * a[2] * a[11] - a[12] * a[3] * a[10];
	inv[9] = -a[0] * a[9] * a[15] + a[0] * a[11] * a[13] + a[8] * a[1] * a[15] - a[8] * a[3] * a[13] - a[12] * a[1] * a[11] + a[12] * a[3] * a[9];
	inv[13] = a[0] * a[9] * a[14] - a[0] * a[10] * a[13] - a[8] * a[1] * a[14] + a[8] * a[2] * a[13] + a[12] * a[1] * a[10] - a[12] * a[2] * a[9];
	inv[2] = a[1] * a[6] * a[15] - a[1] * a[7] * a[14] - a[5] * a[2] * a[15] + a[5] * a[3] * a[14] + a[13] * a[2] * a[7] - a[13] * a[3] * a[6];
	inv[6] = -a[0] * a[6] * a[15] + a[0] * a[7] * a[14] + a[4] * a[2] * a[15] - a[4] * a[3] * a[14] - a[12] * a[2] * a[7] + a[12] * a[3] * a[6];
	inv[10] = a[0] * a[5] * a[15] - a[0] * a[7] * a[13] - a[4] * a[1] * a[15] + a[4] * a[3] * a[13] + a[12] * a[1] * a[7] - a[12] * a[3] * a[5];
	inv[14] = -a[0] * a[5] * a[14] + a[0] * a[6] * a[13] + a[4] * a[1] * a[14] - a[4] * a[2] * a[13] - a[12] * a[1] * a[6] + a[12] * a[2] * a[5];
	inv[3] = -a[1] * a[6] * a[11] + a[1] * a[7] * a[10] + a[5] * a[2] * a[11] - a[5] * a[3] * a[10] - a[9] * a[2] * a[7] + a[9] * a[3] * a[6];
	inv[7] = a[0] * a[6] * a[11] - a[0] * a[7] * a[10] - a[4] * a[2] * a[11] + a[4] * a[3] * a[10] + a[8] * a[2] * a[7] - a[8] * a[3] * a[6];
	inv[11] = -a[0] * a[5] * a[11] + a[0] * a[7] * a[9] + a[4] * a[1] * a[11] - a[4] * a[3] * a[9] - a[8] * a[1] * a[7] + a[8] * a[3] * a[5];
	inv[15] = a[0] * a[5] * a[10] - a[0] * a[6] * a[9] - a[4] * a[1] * a[10] + a[4] * a[2] * a[9] + a[8] * a[1] * a[6] - a[8] * a[2] * a[5];
	float det = a[0] * inv[0] + a[1] * inv[4] + a[2] * inv[8] + a[3] * inv[12];
	if (pDeterminant) *pDeterminant = det;
	if (det == 0.0f) return (D3DXMATRIX*)0;
	float r = 1.0f / det; float* o = &pOut->_11;
	for (int i = 0; i < 16; i++) o[i] = inv[i] * r;
	return pOut;
}
inline D3DXVECTOR3* D3DXVec3Normalize(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV)
{
	float l = sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);
	if (l > 0.0f) { float r = 1.0f / l; *pOut = D3DXVECTOR3(pV->x * r, pV->y * r, pV->z * r); } else *pOut = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	return pOut;
}
inline float D3DXVec3Dot(const D3DXVECTOR3* a, const D3DXVECTOR3* b) { return a->x * b->x + a->y * b->y + a->z * b->z; }
inline D3DXVECTOR3* D3DXVec3Cross(D3DXVECTOR3* pOut, const D3DXVECTOR3* a, const D3DXVECTOR3* b)
{
	D3DXVECTOR3 r(a->y * b->z - a->z * b->y, a->z * b->x - a->x * b->z, a->x * b->y - a->y * b->x); *pOut = r; return pOut;
}
inline D3DXMATRIX* D3DXMatrixLookAtLH(D3DXMATRIX* pOut, const D3DXVECTOR3* pEye, const D3DXVECTOR3* pAt, const D3DXVECTOR3* pUp)
{
	D3DXVECTOR3 z = *pAt - *pEye; D3DXVec3Normalize(&z, &z);
	D3DXVECTOR3 x; D3DXVec3Cross(&x, pUp, &z); D3DXVec3Normalize(&x, &x);
	D3DXVECTOR3 y; D3DXVec3Cross(&y, &z, &x);
	*pOut = D3DXMATRIX(x.x, y.x, z.x, 0.0f, x.y, y.y, z.y, 0.0f, x.z, y.z, z.z, 0.0f, -D3DXVec3Dot(&x, pEye), -D3DXVec3Dot(&y, pEye), -D3DXVec3Dot(&z, pEye), 1.0f);
	return pOut;
}
inline D3DXMATRIX* D3DXMatrixOrthoLH(D3DXMATRIX* pOut, float w, float h, float zn, float zf)
{
	D3DXMatrixIdentity(pOut); pOut->_11 = 2.0f / w; pOut->_22 = 2.0f / h; pOut->_33 = 1.0f / (zf - zn); pOut->_43 = -zn / (zf - zn); return pOut;
}
inline D3DXMATRIX* D3DXMatrixPerspectiveFovLH(D3DXMATRIX* pOut, float fovy, float aspect, float zn, float zf)
{
	float ys = 1.0f / tanf(fovy * 0.5f), xs = ys / aspect;
	memset(pOut->m, 0, sizeof(pOut->m)); pOut->_11 = xs; pOut->_22 = ys; pOut->_33 = zf / (zf - zn); pOut->_34 = 1.0f; pOut->_43 = -zn * zf / (zf - zn);
	return pOut;
}
inline D3DXVECTOR3* D3DXVec3TransformCoord(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV, const D3DXMATRIX* pM)
{
	float x = pV->x * pM->_11 + pV->y * pM->_21 + pV->z * pM->_31 + pM->_41;
	float y = pV->x * pM->_12 + pV->y * pM->_22 + pV->z * pM->_32 + pM->_42;
	float z = pV->x * pM->_13 + pV->y * pM->_23 + pV->z * pM->_33 + pM->_43;
	float w = pV->x * pM->_14 + pV->y * pM->_24 + pV->z * pM->_34 + pM->_44;
	if (w == 0.0f) w = 1.0f;
	*pOut = D3DXVECTOR3(x / w, y / w, z / w); return pOut;
}
inline D3DXVECTOR3* D3DXVec3Project(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV, const D3DVIEWPORT9* pViewport, const D3DXMATRIX* pProjection, const D3DXMATRIX* pView, const D3DXMATRIX* pWorld)
{
	D3DXMATRIX m; D3DXMatrixIdentity(&m);
	if (pWorld) m = m * (*pWorld);
	if (pView) m = m * (*pView);
	if (pProjection) m = m * (*pProjection);
	D3DXVECTOR3 c; D3DXVec3TransformCoord(&c, pV, &m);
	D3DXVECTOR3 r(c.x, c.y, c.z);
	if (pViewport)
	{
		r.x = (float)pViewport->X + (1.0f + c.x) * (float)pViewport->Width * 0.5f;
		r.y = (float)pViewport->Y + (1.0f - c.y) * (float)pViewport->Height * 0.5f;
		r.z = pViewport->MinZ + c.z * (pViewport->MaxZ - pViewport->MinZ);
	}
	*pOut = r; return pOut;
}
inline D3DXVECTOR3* D3DXVec3Unproject(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV, const D3DVIEWPORT9* pViewport, const D3DXMATRIX* pProjection, const D3DXMATRIX* pView, const D3DXMATRIX* pWorld)
{
	D3DXMATRIX m; D3DXMatrixIdentity(&m);
	if (pWorld) m = m * (*pWorld);
	if (pView) m = m * (*pView);
	if (pProjection) m = m * (*pProjection);
	D3DXMATRIX inv; if (!D3DXMatrixInverse(&inv, (float*)0, &m)) D3DXMatrixIdentity(&inv);
	D3DXVECTOR3 n(pV->x, pV->y, pV->z);
	if (pViewport)
	{
		n.x = (pV->x - (float)pViewport->X) / (float)pViewport->Width * 2.0f - 1.0f;
		n.y = 1.0f - (pV->y - (float)pViewport->Y) / (float)pViewport->Height * 2.0f;
		n.z = (pViewport->MaxZ != pViewport->MinZ) ? (pV->z - pViewport->MinZ) / (pViewport->MaxZ - pViewport->MinZ) : pV->z;
	}
	return D3DXVec3TransformCoord(pOut, &n, &inv);
}
// tai texture/surface: qua thiet bi (CPU), khong loc, khong doi kich thuoc
inline HRESULT D3DXLoadSurfaceFromSurface(IDirect3DSurface9* pDestSurface, CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, IDirect3DSurface9* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey)
{
	if (!pDestSurface || !pSrcSurface) return D3DERR_INVALIDCALL;
	IDirect3DDevice9* pDev = (IDirect3DDevice9*)0;
	if (FAILED(pDestSurface->GetDevice(&pDev)) || !pDev) return D3DERR_INVALIDCALL;
	POINT pt = { 0, 0 }; if (pDestRect) { pt.x = pDestRect->left; pt.y = pDestRect->top; }
	HRESULT hr = pDev->UpdateSurface(pSrcSurface, pSrcRect, pDestSurface, &pt);
	pDev->Release();
	return hr;
}
inline HRESULT D3DXCreateTexture(IDirect3DDevice9* pDevice, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture)
{
	if (!pDevice) return D3DERR_INVALIDCALL;
	return pDevice->CreateTexture(Width, Height, MipLevels ? MipLevels : 1, Usage, Format, Pool, ppTexture, (HANDLE*)0);
}

#endif // JX_D3D9MINI
