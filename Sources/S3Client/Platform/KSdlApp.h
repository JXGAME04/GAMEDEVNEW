//---------------------------------------------------------------------------
// [SDL 08/09] Lop nen SDL3 cho client (phuong an mobile, pha 2 - lat 2a "Game64-SDL tren Windows").
// KSdlApp thay KWin32App o hai viec: tao cua so (SDL_CreateWindow, lay HWND cho Represent3) va vong lap
// (SDL_PollEvent -> dich sang WM_* -> dua vao MsgProc/HandleInput cu). Moi thu khac (KWnd, Core, mang, tep,
// am thanh) giu nguyen o lat nay. Chi bien dich khi JX_PLATFORM_SDL (cau hinh ReleaseSDL|x64, GameSDL.exe);
// ban PC thuong khong dung tep nay. KHONG doi KWin32App (Engine.dll giu nguyen): hover chuot tu lam lai o day.
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
	// che ten (khong virtual) SetMouseHoverTime cua KWin32App: KMyApp::GameInit goi SetMouseHoverTime(400) -> toi day
	void			SetMouseHoverTime(unsigned int uHoverTime);
	SDL_Window*		GetWindow() { return m_pWindow; }
	// Goi tu moc thong diep Windows cua SDL (WM_COPYDATA, khay, TaskbarCreated) - can public
	LRESULT			RawWinMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return MsgProc(hWnd, uMsg, wParam, lParam); }
protected:
	bool			TranslateEvent(const SDL_Event& ev);	// false = thoat vong lap
	void			GhiChuot(WPARAM wParam, LPARAM lParam);	// bookkeeping cho hover (ban sao cua MsgProc, vi cua KWin32App la private)
	void			SinhHover();							// ban sao KWin32App::GenerateMsgHoverMsg (private ben Engine)
	SDL_Window*		m_pWindow;
	unsigned int	m_uMsgQuit;			// dem WM_CLOSE da xu ly (chan doan)
	unsigned int	m_uHoverSetting;	// ms; 0 = tat
	unsigned int	m_uHoverStart;		// 0 none, 1 vua co su kien chuot, 2 da gui hover, >=3 moc thoi gian bat dau dung yen
	unsigned int	m_uHoverLastStatus;
	int				m_nHoverLastPos;
	char			m_cHoverCounter;
};

#endif // JX_PLATFORM_SDL
#endif
