//---------------------------------------------------------------------------
// [ANDROID 08/09] Stub KDirectDraw cho ban khong co DirectX (JX_NO_DIRECTX): KDDraw.cpp bi loai khoi ban Android,
// nhung khai bao lop (KDDraw.h) va con tro g_pDirectDraw van duoc KCanvas/KBitmap16/KColors/KDrawBase/KPalette/... tham chieu
// (moi cho goi deu kiem g_pDirectDraw != NULL, ma tren SDL/Represent3 khong ai tao KDirectDraw -> luon NULL).
// Tep nay chi de LINK duoc; khong ham nao duoc goi luc chay. Chi bien dich khi JX_POSIX (CMake Android).
//---------------------------------------------------------------------------
#ifdef JX_POSIX
#include "KWin32.h"
#include "KMemBase.h"
#include "KDDraw.h"

ENGINE_API KDirectDraw* g_pDirectDraw = NULL;

KDirectDraw::KDirectDraw()
{
	m_lpDirectDraw = NULL; m_lpDDSPrimary = NULL; m_lpDDSBackBuf = NULL; m_lpClipper = NULL;
	m_dwScreenMode = 0; m_dwScreenWidth = 0; m_dwScreenHeight = 0; m_dwScreenPitch = 0;
	m_dwRGBBitCount = 16; m_dwRGBBitMask16 = 0; m_dwRGBBitMask32 = 0;
	g_pDirectDraw = this;
}
KDirectDraw::~KDirectDraw() { g_pDirectDraw = NULL; }
void KDirectDraw::Mode(BOOL bFullScreen, int nWidth, int nHeight) { (void)bFullScreen; m_dwScreenWidth = nWidth; m_dwScreenHeight = nHeight; }
BOOL KDirectDraw::Init() { return FALSE; }
void KDirectDraw::Exit() {}
void KDirectDraw::WaitForVerticalBlankBegin() {}
void KDirectDraw::WaitForVerticalBlankEnd() {}
BOOL KDirectDraw::RestoreSurface() { return TRUE; }
void KDirectDraw::SetClipperHWnd(HWND hWnd) { (void)hWnd; }
void KDirectDraw::FillBackBuffer(DWORD dwColor) { (void)dwColor; }
void KDirectDraw::UpdateScreenZoom(LPRECT lpRect) { (void)lpRect; }
LPDIRECTDRAWSURFACE KDirectDraw::CreateSurface(int nWidth, int nHeight) { (void)nWidth; (void)nHeight; return NULL; }
void KDirectDraw::BltToFrontBuffer(LPDIRECTDRAWSURFACE pSurface, RECT* pDestRect, RECT* pSrcRect) { (void)pSurface; (void)pDestRect; (void)pSrcRect; }
void KDirectDraw::BltToBackBuffer(LPDIRECTDRAWSURFACE pSurface, RECT* pDestRect, RECT* pSrcRect) { (void)pSurface; (void)pDestRect; (void)pSrcRect; }
BOOL KDirectDraw::CreateDirectDraw() { return FALSE; }
BOOL KDirectDraw::CreateClipper() { return FALSE; }
BOOL KDirectDraw::CreateSurface() { return FALSE; }
BOOL KDirectDraw::GetSurfaceDesc() { return FALSE; }
BOOL KDirectDraw::GetDisplayMode() { return FALSE; }
BOOL KDirectDraw::SetDisplayMode() { return FALSE; }
void KDirectDraw::SetWindowStyle() {}
#endif // JX_POSIX
