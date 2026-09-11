/*****************************************************************************************
//	剑侠引擎，界面窗口体系结构的最基本窗口对象
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-9
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

//============窗口风格================
#define WND_S_VISIBLE				0x80000000	//可见
#define WND_S_DISABLED				0x40000000	//不可操作
#define	WND_S_TOPMOST				0x20000000	//置顶窗口
#define	WND_S_MOVEALBE				0x10000000	//可被拖动
#define	WND_S_DISABLE				0x04000000	//窗口不可被操作
#define	WND_S_TOBEDESTROY			0x02000000	//window is to be destroy
#define	WND_S_MOVE_WITH_R_EDGE		0x01000000	//跟随父窗口右下边界的变化移动窗口
#define	WND_S_MOVE_WITH_B_EDGE		0x00800000	//跟随父窗口右下边界的变化移动窗口
#define	WND_S_SIZE_WITH_R_EDGE		0x00400000	//跟随父窗口大小的变化调整窗口大小
#define	WND_S_SIZE_WITH_B_EDGE		0x00200000	//跟随父窗口大小的变化调整窗口大小
#define	WND_S_SIZE_WITH_ALL_CHILD	0x00100000	//以子窗口的范围测试的集合作为自己的范围测试的集合

// [UITOADO] o giao dien bi nguoi choi "xoa" (an han). Bit nay TRONG:
// khong lop nao trong S3Client\Ui dung 0x08000000, va Show()/Hide() chi
// dung WND_S_VISIBLE nen ma game khong the bat lai duoc.
#define	WND_S_UITOADO_AN			0x08000000

extern	int WND_SHOW_DEBUG_FRAME_TEXT;

#ifdef JX_ANDROID
// [ANDROID 09/09 NEO] Chinh sach neo cua so khi man hinh khac khung ve chuan 1024x768.
// Mang tu ban JX1 Mobile cua chu (USVOLAM, "dot UI-1/UI-2").
enum FitFlags
{
	FIT_NONE    = 0,
	FIT_LEFT    = 1 << 0,	// giu nguyen Left (bam le trai)
	FIT_RIGHT   = 1 << 1,	// bam le phai
	FIT_HCENTER = 1 << 2,	// can giua ngang
	FIT_TOP     = 1 << 3,	// giu nguyen Top (bam le tren)
	FIT_BOTTOM  = 1 << 4,	// bam le duoi
	FIT_VCENTER = 1 << 5,	// can giua doc
	FIT_NOFIT   = 1 << 7,	// cua so tu dat cho, dung dung toi
};
#endif

class KIniFile;

class KWndWindow
{
protected:
	//----窗口参数----
	unsigned int m_Style;
#ifdef JX_ANDROID
	unsigned char	m_FitFlags;		// [ANDROID 09/09 NEO] chinh sach neo (0 = mac dinh: dich vao giua)
	unsigned char	m_bNeedFit;		// [ANDROID 09/09 NEO] Init vua dat lai khung -> con mot luot can
#endif			//窗口风格,见Wnd.h中窗口风格的定义
	int			m_Left;				//左上角X坐标，相对于父窗口
	int			m_Top;				//左上角Y坐标，相对于父窗口
	int			m_Width;			//宽度
	int			m_Height;			//高度
	int			m_nAbsoluteLeft;	//窗口左上角的绝对坐标x
	int			m_nAbsoluteTop;		//窗口左上角的绝对坐标y
#ifdef _DEBUG
	char		m_Caption[32];		//标题文字
#endif

	// [UITOADO] ten muc ini ma cua so nay duoc Init tu do - dinh danh ben
	// de luu / ap lai toa do trong UserData\UiToaDo.ini
	char		m_szMucIni[64];
	// [UITOADO] ti le do nguoi choi dat, phan nghin (1000 = 100%).
	// m_nUiGocW/H chup kich thuoc goc DUNG MOT LAN de ap lai bao nhieu
	// lan cung ra cung ket qua (khong nhan don).
	int			m_nUiTiLe;
	int			m_nUiGocW;
	int			m_nUiGocH;
#ifdef JX_ANDROID
	// [PHONGBANG 14/09] phong ca CAY CON cua mot bang theo man hinh: vi tri goc (tuong doi cha) va co chu goc chup mot lan;
	// m_nUiPhongLech = bang da dich de giu tam (bo cuc ghi vi tri o co GOC = vi tri that + lech)
	int			m_nUiGocLeft;
	int			m_nUiGocTop;
	int			m_bUiGocViTri;
	int			m_nUiGocFont;
	int			m_nUiPhongCay;		// ti le cay dang ap (1000 = chua)
	int			m_nUiPhongLechX;
	int			m_nUiPhongLechY;
	int			m_nUiPhongDatX;	// [c] vi tri PhongBang da dat lan truoc (de biet co ai doi chua)
	int			m_nUiPhongDatY;
	int			m_nUiFontDat;	// [d] co chu chinh UiPhongChu da dat (khac -> co chu vua doc lai tu ini, chup goc lai)
	int			m_bUiViTriTuBang;	// [g] vi tri vua dat tu bang bo cuc (= vi tri o co goc) -> PhongBang dich giu tam
#endif

	int			m_bMoving;
	int			m_nLastMouseHoldPosX;
	int			m_nLastMouseHoldPosY;

	//----与其他窗口的级连关系----
	KWndWindow*	m_pPreviousWnd;		//前一个同级窗口
	KWndWindow*	m_pNextWnd;			//后一个同级窗口
	KWndWindow*	m_pFirstChild;		//第一个子窗口
	KWndWindow*	m_pParentWnd;		//父窗口

public:
	KWndWindow();
	virtual	~KWndWindow();

	void	Destroy();
	virtual void UpdateData(){}

	//====可重载的函数====
	// flying add this function
	int			CloseWindow(bool bDestory){return 0;};	// [CLANG 08/09] bo virtual: khong lop nao override khong-static, khong goi ao; 123 lop KUi* khai bao static CloseWindow(bool) che no (clang/GCC cam static trung ham ao lop cha)

	virtual int		Init(KIniFile* pIniFile, const char* pSection);//初始化
	virtual int		WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam);//窗口函数
	virtual int		PtInWindow(int x, int y);				//判断一个点是否在窗口范围内,传入的是绝对坐标

	virtual void	PaintWindow();							//窗体绘制
	void			PaintDebugInfo();

	virtual void	SetSize(int nWidth, int nHeight);		//设置窗口大小

	//====窗口行为操作====
	void			BringToTop();							//把窗口置顶
	void			GetPosition(int* pLeft, int* pTop);		//获取窗口位置，相对坐标
	void			SetPosition(int nLeft, int nTop);		//设置窗口位置，相对坐标
	void			GetAbsolutePos(int* pLeft, int* pTop);	//获取窗口位置，绝对坐标
	void			GetSize(int* pWidth, int* pHeight);		//获取窗口大小
	void			SetCursorAbove();						//使鼠标指针以移动到悬浮在此窗口中的位置上
	void			GetAllChildLayoutRect(RECT* pRect);		//取得包含所有子窗口分布区域的最小区域

	virtual void	Show();									//显示窗口
	virtual void	Hide();									//隐藏窗口
	int				IsVisible();							//判断窗口是否被显示
	virtual	void	Enable(int bEnable);					//禁止或者允许使窗口被操作
//	void			SetStyle(DWORD	dwStyle);				//修改窗口风格
	void			AddChild(KWndWindow* pChild);			//添加子窗口
	void			AddBrother(KWndWindow* pBrother);		//添加兄弟窗口

	virtual KWndWindow*	TopChildFromPoint(int x, int y);	//得到处于指定坐标位置的最上层窗口，传入的坐标为绝对坐标

	KWndWindow*		GetPreWnd() const { return m_pPreviousWnd; }	//得到前一个兄弟窗口
#ifdef JX_ANDROID
	// [ANDROID 09/09 NEO] dat chinh sach neo cho cua so nay (xem enum FitFlags)
	void			SetFitFlags(unsigned char nFlags) { m_FitFlags = nFlags; }
	// [ANDROID 09/09 NEO] dat lai mot cua so GOC tu khung ve chuan 1024x768 sang man hinh that
	virtual void	FitToScreen();
	void			ComputeFit(int nRefL, int nRefT, int* pOutL, int* pOutT);
	// [SUAGD 13/09] dat ti le nhung giu TAM o (phong to / thu nho quanh tam -> o bam mep khong troi ra ngoai)
	void			UiDatTiLeQuanhTam(int nTiLe);
	// [PHONGBANG 14/09] phong ca cay con (vi tri, co, chu, luoi vat pham) theo ti le phan nghin; ap lai bao nhieu lan cung
	// ra mot ket qua (tinh tu goc). Lop chu ghi de UiLayFont / UiDatFont; lop can tinh lai so do sau khi doi co ghi de UiPhongRieng.
	void			UiPhongCay(int nTiLe);
	int				UiLayPhongCay() const { return m_nUiPhongCay; }
	void			UiGhiPhongCay(int nTiLe) { m_nUiPhongCay = nTiLe; }
	int				UiLayGocW() const { return (m_nUiGocW || m_nUiGocH) ? m_nUiGocW : m_Width; }
	int				UiLayGocH() const { return (m_nUiGocW || m_nUiGocH) ? m_nUiGocH : m_Height; }
	void			UiChupGocCo() { if (m_nUiGocW == 0 && m_nUiGocH == 0) { m_nUiGocW = m_Width; m_nUiGocH = m_Height; } }
	void			UiChupGocViTri() { if (!m_bUiGocViTri) { m_nUiGocLeft = m_Left; m_nUiGocTop = m_Top; m_bUiGocViTri = 1; } }
	void			UiDatGocViTri(int nLeft, int nTop) { m_nUiGocLeft = nLeft; m_nUiGocTop = nTop; m_bUiGocViTri = 1; }
	void			UiDatViTriTuGoc(int nTiLe) { if (m_bUiGocViTri) SetPosition(m_nUiGocLeft * nTiLe / 1000, m_nUiGocTop * nTiLe / 1000); }
	// [e] co chu la MA font trong [FontList] uibasepublicsetting.ini (10, 12, 13=12, 14, 16; font.pak chi co fs12/14/16):
	// ma la thi OutputText bo ve -> chon ma gan nhat. Muon chu to hon 16 phai them tep .fnt + ma vao [FontList].
	static int	UiChonMaFont(int f) { return (f >= 15) ? 16 : (f >= 13) ? 14 : (f >= 11) ? 12 : 10; }
	void			UiPhongChu(int nTiLe) { int f = UiLayFont(); if (f > 0) { if (!m_nUiGocFont || f != m_nUiFontDat) m_nUiGocFont = f; f = UiChonMaFont(m_nUiGocFont * nTiLe / 1000); UiDatFont(f); m_nUiFontDat = f; } }
	void			UiPhongChuCay(int nTiLe);	// [e] chi chu, ca cay (widget vua Init lai: Init doc Font= SAU KWndWindow::Init)
	void			UiDatPhongLech(int nX, int nY) { m_nUiPhongLechX = nX; m_nUiPhongLechY = nY; }
	int				UiLayPhongLechX() const { return m_nUiPhongLechX; }
	int				UiLayPhongLechY() const { return m_nUiPhongLechY; }
	void			UiGhiPhongDat(int nX, int nY) { m_nUiPhongDatX = nX; m_nUiPhongDatY = nY; }
	int				UiPhongDaDat(int nX, int nY) const { return m_nUiPhongDatX == nX && m_nUiPhongDatY == nY; }
	void			UiDanhDauViTriBang() { m_bUiViTriTuBang = 1; }
	int				UiLayViTriTuBang() const { return m_bUiViTriTuBang; }
	void			UiXoaViTriTuBang() { m_bUiViTriTuBang = 0; }
	// [c] o vua Init lai theo toa do thiet ke (trang Auto doi tab): xoa goc de chup lai, roi UiPhongLai phong ca cay tu dau
	void			UiXoaGoc() { m_nUiGocW = 0; m_nUiGocH = 0; m_bUiGocViTri = 0; m_nUiGocFont = 0; m_nUiTiLe = 1000; m_nUiPhongCay = 1000; }
	void			UiPhongLai(int nTiLe);
	virtual int	UiLayFont() const { return 0; }
	virtual void	UiDatFont(int nFont) {}
	virtual void	UiPhongRieng(int nTiLe) {}
#endif
	KWndWindow*		GetNextWnd() const { return m_pNextWnd; }		//得到后一个兄弟窗口
	KWndWindow*		GetParent() const { return m_pParentWnd; }		//得到父窗口
	KWndWindow*		GetFirstChild() const { return m_pFirstChild; }	//得到第一个子窗口
	KWndWindow*		GetOwner();								//获得不再有父窗口的祖先窗口

	void			Paint();								//绘制
	void			LetMeBreathe();							//让窗口活动

	void			SplitSmaleFamily();						//把自己（及子窗口）从窗口树里面里面分离出来
	void			LeaveAlone();							//世间再无窗在我左右，一无牵连

	//----属性设置，获取----
#ifdef _DEBUG
	void			SetCaption(char* pszCaption);
#endif
	// [UITOADO] ten muc ini dat cho cua so nay (rong = chua Init tu ini)
	const char*		GetMucIni() const { return m_szMucIni; }
	// [UITOADO] dat lai ti le (phan nghin), kep trong 300..3000
	void			UiDatTiLe(int nTiLe);
	int				UiLayTiLe() const { return m_nUiTiLe; }
	// [UITOADO] an han / hien lai o nay
	void			UiDatAn(int bAn);
	int				UiDangAn() const { return (m_Style & WND_S_UITOADO_AN) != 0; }
	// [UITOADO] goi ngay sau khi toa do bi doi tu ben ngoai, de lop con
	// (KWndMovingImage) chup lai vi tri goc cua hoat hinh mo cua so
	virtual void	UiNhoViTri() {}
#ifdef JX_ANDROID
	//	[UITOADO 14/09 LOPPHU] hau to ghep vao ten lop khi tao khoa bo cuc (ban do nho: che do to / to mo rong / khong anh
	//	dung bo khoa rieng vi ini moi che do khac han). NULL / rong = khong doi.
	virtual const char* UiTenLopPhu() const { return NULL; }
#endif
	int				IsDisable() { return (m_Style & WND_S_DISABLE); }
	int				GetStyle() { return m_Style; }
	int				SetStyle(unsigned int nStyle)
	{
		m_Style = nStyle;
		return m_Style; 
	}
	
protected:
	void			Clone(KWndWindow* pCopy);

private:
	virtual void	Breathe() {}							//窗口的持续行为
	void			OnLBtnDown(int x, int y);				//响应鼠标左键按下的操作，传入的坐标为绝对坐标
	void			OnMoveWnd();							//响应鼠标左键按下移动的操作，传入的坐标为绝对坐标

private:
	void			AbsoluteMove(int dx, int dy);			//绝对坐标的调整
};

//把字符串表示的颜色信息转为数值表示
unsigned int	GetColor(const char* pString);
//把数值表示的颜色信息转为字符串表示
const char* GetColorString(unsigned int nColor);
