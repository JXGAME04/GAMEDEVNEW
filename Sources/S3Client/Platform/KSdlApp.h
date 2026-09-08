//---------------------------------------------------------------------------
// [SDL 08/09] Lop nen SDL3 cho client (phuong an mobile, pha 2 - lat 2a "Game64-SDL tren Windows").
// KSdlApp thay KWin32App o hai viec: tao cua so (SDL_CreateWindow, lay HWND cho Represent3) va vong lap
// (SDL_PollEvent -> dich sang WM_* -> dua vao MsgProc/HandleInput cu). Moi thu khac (KWnd, Core, mang, tep,
// am thanh) giu nguyen o lat nay. Chi bien dich khi JX_PLATFORM_SDL (cau hinh ReleaseSDL|x64, GameSDL.exe);
// ban PC thuong khong dung tep nay.
//---------------------------------------------------------------------------
#ifndef KSdlApp_H
#define KSdlApp_H
#ifdef JX_PLATFORM_SDL

struct SDL_Window;
union  SDL_Event;

class KSdlApp : public KWin32App
{
public:
	KSdlApp();
	virtual BOOL	Init(HINSTANCE hInstance, char* AppName = "JXWC");
	virtual void	Run();
	virtual void	ShowMouse(BOOL bShow);
	SDL_Window*		GetWindow() { return m_pWindow; }
	// Goi tu moc thong diep Windows cua SDL (WM_COPYDATA, khay, TaskbarCreated) - can public
	LRESULT			RawWinMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return MsgProc(hWnd, uMsg, wParam, lParam); }
protected:
	bool			TranslateEvent(const SDL_Event& ev);	// false = thoat vong lap
	SDL_Window*		m_pWindow;
	unsigned int	m_uMsgQuit;		// dem WM_CLOSE da xu ly (chan doan)
};

#endif // JX_PLATFORM_SDL
#endif
