//---------------------------------------------------------------------------
// [ANDROID 08/09] Thay cac tep D3D9on11*.cpp (lop D3D9 tren D3D11 - chi Windows) tren POSIX: Represent3 van goi
// Rep3_CreateD3D9on11 (Rep3Api=11 -> khong co, tra NULL), API bang mau Rep3_D3D11*Palette (dispatch thang sang thiet bi SDL_GPU),
// Rep3_D3D11VramInfo (uoc luong cua SDL_GPU), va cac bo dem thong ke g_uRep3*/g_dRep3* (D3D9on11Dev.cpp dinh nghia).
// Chi bien dich khi JX_POSIX (CMake Android).
//---------------------------------------------------------------------------
#ifdef JX_POSIX
#include "precompile.h"
#include "D3D9onGPU.h"

double           g_dRep3DrawMs = 0.0;
double           g_dRep3PresentMs = 0.0;
unsigned         g_uRep3Presents = 0;
unsigned         g_uRep3PresentSkip = 0;
unsigned         g_uRep3Draws = 0;
unsigned         g_uRep3BatchQuads = 0;
unsigned         g_uRep3BatchDraws = 0;
unsigned         g_uRep3AtlasPages = 0;
unsigned __int64 g_uRep3AtlasBytes = 0;
unsigned         g_uRep3GpuTexCount = 0;
unsigned __int64 g_uRep3GpuTexBytes = 0;
unsigned         g_uRep3PalRows = 0;
/* [ANDROID 11/09 MANG] bo dem moi cua [GOP do] / [MANG a-e] - dinh nghia trong D3D9on11Dev.cpp (chi Windows) */
unsigned         g_uRep3GopVo[12] = { 0 };
unsigned         g_uRep3VeNgay[4] = { 0 };
unsigned         g_uRep3CullGiu = 0;
unsigned         g_uRep3CullBo = 0;
unsigned         g_uRep3RingVong = 0;
unsigned         g_uRep3TexRiengTao = 0;
double           g_dRep3RingMapMax = 0.0;

IDirect3D9* Rep3_CreateD3D9on11() { return NULL; }
int  Rep3_D3D11PaletteOK() { return Rep3_GpuPaletteOK() ? 1 : 0; }
int  Rep3_D3D11AllocPalette(const unsigned char* pPal24, int nColors) { return Rep3_GpuPaletteOK() ? Rep3_GpuAllocPalette(pPal24, nColors) : -1; }
void Rep3_D3D11FreePalette(int nRow) { if (Rep3_GpuPaletteOK()) Rep3_GpuFreePalette(nRow); }
void Rep3_D3D11TagPalette(IDirect3DTexture9* pTex, int nRow) { if (Rep3_GpuPaletteOK()) Rep3_GpuTagPalette(pTex, nRow); }
void Rep3_D3D11VramInfo(unsigned* puUsedMB, unsigned* puBudgetMB) { Rep3_GpuVramInfo(puUsedMB, puBudgetMB); }
#endif // JX_POSIX
