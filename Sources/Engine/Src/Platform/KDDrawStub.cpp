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

#include "Kime.h"

ENGINE_API KDirectDraw* g_pDirectDraw = NULL;
// KDDraw.cpp cung la noi giu kich thuoc cua so ban dau (S3Client goi SetEngineResolution truoc KSdlApp::Init)
int WND_INIT_WIDTH = 800;
int WND_INIT_HEIGHT = 600;
ENGINE_API void SetEngineResolution(int width, int height) { WND_INIT_WIDTH = width; WND_INIT_HEIGHT = height; }
// Kime.cpp (IME Windows) bi loai: KWin32App::MsgProc chi hoi g_pIme khi khac NULL
ENGINE_API KIme* g_pIme = NULL;
int KIme::WndMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { (void)hWnd; (void)uMsg; (void)wParam; (void)lParam; return 0; }
KIme::KIme() { m_bNoLanguageChange = false; m_bIsWorking = false; m_bCloseFlag = false; m_bCloseFlagForCanChange = false; m_dwConversion = 0; m_dwSentence = 0; m_bCloseIMESimHotKey = false; m_nCaretX = 0; m_nCaretY = 0; }
KIme::~KIme() {}
void KIme::ResetIme(void) {}
void KIme::EnableLanguageChange() {}
void KIme::DisableLanguageChange() {}
void KIme::CloseIME() {}
void KIme::OpenIME() {}
BOOL KIme::IsIme() { return FALSE; }
void KIme::TurnOff() {}
void KIme::TurnOn() {}
void KIme::SetCaretPos(int X, int Y) { m_nCaretX = X; m_nCaretY = Y; }

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
