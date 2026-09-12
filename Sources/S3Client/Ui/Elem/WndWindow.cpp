/*****************************************************************************************
//	剑侠引擎，界面窗口体系结构的最基本窗口对象
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-9
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "WndWindow.h"
#include "shlwapi.h"
#include "Wnds.h"
#ifdef JX_MOBILE
void UiToaDo_ApChoO(KWndWindow* pWnd);	// UiToaDo.cpp, [UITOADO 12/09 NEO b]
void UiToaDo_ApChoOInit(KWndWindow* pWnd);	// [PHONGBANG 14/09 d] nhu tren nhung KHONG phong bang (con chua Init)
void UiToaDo_PhongBangKhiHien(KWndWindow* pWnd);	// [PHONGBANG 14/09 f] cua so goc vua Show(): bang tinh nang phong ca cay theo man hinh
#endif
#include "MouseHover.h"
#include "UiToaDo.h"	// [UITOADO]

#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
extern iRepresentShell*	g_pRepresentShell;


int WND_SHOW_DEBUG_FRAME_TEXT = false;

#define	MOSE_HOVER_TIME_VALVE		1000	

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndWindow::KWndWindow()
{
#ifdef _DEBUG
	m_Caption[0]	= 0;
	m_Caption[31]	= 0;
#endif
	m_Left			= 0;
	m_Top			= 0;
	m_Width			= 0;
	m_Height		= 0;
	m_nAbsoluteLeft = 0;
	m_nAbsoluteTop	= 0;

	m_szMucIni[0]	= 0;			// [UITOADO]
	m_nUiTiLe		= 1000;			// [UITOADO] 1000 = 100%, chua doi
	m_nUiGocW		= 0;
	m_nUiGocH		= 0;
#ifdef JX_MOBILE
	m_nUiGocLeft	= 0;		// [PHONGBANG 14/09]
	m_nUiGocTop		= 0;
	m_bUiGocViTri	= 0;
	m_nUiGocFont	= 0;
	m_nUiPhongCay	= 1000;
	m_nUiPhongLechX	= 0;
	m_nUiPhongLechY	= 0;
	m_nUiPhongDatX	= -100000;	// [c]
	m_nUiPhongDatY	= -100000;
	m_nUiFontDat	= 0;
	m_bUiViTriTuBang	= 0;
#endif

	m_bMoving = false;
	m_nLastMouseHoldPosX = m_nLastMouseHoldPosY = 0;

	m_pPreviousWnd	= NULL;
	m_pNextWnd		= NULL;
	m_pFirstChild	= NULL;
	m_pParentWnd	= NULL;
	m_Style			= WND_S_VISIBLE;
#ifdef JX_MOBILE
	m_FitFlags = 0;		// [ANDROID 09/09 NEO]
	m_bNeedFit = 0;
#endif
}

//--------------------------------------------------------------------------
//	功能：分构函数
//--------------------------------------------------------------------------
KWndWindow::~KWndWindow()
{
	LeaveAlone();
	Wnd_OnWindowDelete(this);
	g_MouseOver.OnWndClosed((void*)this);
	g_MouseOverCompare.OnWndClosed((void*)this);
}

//--------------------------------------------------------------------------
//	功能：把窗口移动到最前面
//--------------------------------------------------------------------------

#ifdef JX_MOBILE
//--------------------------------------------------------------------------
// [ANDROID 09/09 NEO] TU CAN CHINH GIAO DIEN THEO MAN HINH
//
// Moi tep .ini giao dien duoc ve theo KHUNG CHUAN 1024x768. Tren dien thoai khung ve la thu khac
// (vi du 1040x604, 1188x616, 1370x616) nen cua so nao neo goc tren-trai se nam sai cho, con cua so
// le phai / le duoi thi hoac loi ra ngoai hoac de ho mot mang.
//
// Cach lam (mang tu ban JX1 Mobile cua chu, "dot UI-1/UI-2"):
//   mac dinh              : dich ca khung 1024 vao GIUA man hinh -> giu nguyen bo cuc tuong doi
//   FIT_LEFT / FIT_TOP    : giu nguyen goc (bam le trai / le tren)
//   FIT_RIGHT / FIT_BOTTOM: bam le phai / le duoi
//   FIT_HCENTER/VCENTER   : can giua that (theo be ngang thuc cua cua so)
//   LEFT|RIGHT, TOP|BOTTOM: keo cang cho day man hinh
//   FIT_NOFIT             : cua so tu dat cho (hop thoai tu can giua) - khong dung toi
//
// Moi cua so chi can MOT LAN: KWndWindow::Init dat m_bNeedFit, Wnd_RenderWindows quet mot luot.
//--------------------------------------------------------------------------
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

void KWndWindow::ComputeFit(int nRefL, int nRefT, int* pOutL, int* pOutT)
{
	const int nRefW = 1024;		// khung ve chuan ma cac tep .ini duoc ve theo
	const int nRefH = 768;
	int nFlags = m_FitFlags & ~FIT_NOFIT;
	int nDX = SCREEN_WIDTH - nRefW;
	int nDY = SCREEN_HEIGHT - nRefH;

	int eW = m_Width, eH = m_Height;
	if ((m_Style & WND_S_SIZE_WITH_ALL_CHILD) || m_Width <= 0 || m_Height <= 0)
	{
		RECT rcAll;		// cua so "rong" (chi chua o con) phai lay be ngang that cua dam con
		GetAllChildLayoutRect(&rcAll);
		if (rcAll.right > rcAll.left) eW = rcAll.right - rcAll.left;
		if (rcAll.bottom > rcAll.top) eH = rcAll.bottom - rcAll.top;
	}

	// Khong dat FitFlags thi TU SUY RA neo theo cho cua so nam trong khung chuan:
	//   nam o mot phan ba dau  -> bam le tren / le trai
	//   nam o mot phan ba cuoi -> bam le duoi / le phai
	//   nam o giua             -> dich vao giua
	// KHAC ban USVOLAM (ho mac dinh luon "dich ca khung vao giua"): ho chi bat he neo khi man hinh
	// RONG VA CAO hon khung chuan. Man hinh dien thoai thi nguoc lai - rong hon nhung THAP hon
	// nhieu (604 / 616 so voi 768), dich vao giua se keo thanh cong cu o day man hinh LEN ~82 diem
	// anh (da nhin thay tan mat). Tu suy ra neo thi thanh tren o tren, thanh duoi o duoi, hop thoai
	// van o giua - dung cho moi co man hinh ma khong phai danh dau tung cua so.
	int nGiuaX = nRefL + (eW > 0 ? eW / 2 : 0);
	int nGiuaY = nRefT + (eH > 0 ? eH / 2 : 0);

	int nNewL;
	if ((nFlags & FIT_LEFT) && (nFlags & FIT_RIGHT)) nNewL = nRefL;			// keo cang: giu goc
	else if (nFlags & FIT_RIGHT)   nNewL = nRefL + nDX;						// bam le phai
	else if (nFlags & FIT_LEFT)    nNewL = nRefL;							// bam le trai
	else if (nFlags & FIT_HCENTER) nNewL = (SCREEN_WIDTH - eW) / 2;			// can giua that
	else if (nGiuaX < nRefW / 3)   nNewL = nRefL;							// tu suy: nua trai -> giu goc
	else if (nGiuaX > nRefW * 2 / 3) nNewL = nRefL + nDX;					// tu suy: nua phai -> bam le phai
	else                           nNewL = nRefL + nDX / 2;					// tu suy: giua -> dich vao giua

	int nNewT;
	if ((nFlags & FIT_TOP) && (nFlags & FIT_BOTTOM)) nNewT = nRefT;
	else if (nFlags & FIT_BOTTOM)  nNewT = nRefT + nDY;						// bam le duoi
	else if (nFlags & FIT_TOP)     nNewT = nRefT;							// bam le tren
	else if (nFlags & FIT_VCENTER) nNewT = (SCREEN_HEIGHT - eH) / 2;
	else if (nGiuaY < nRefH / 3)   nNewT = nRefT;							// tu suy: phan tren -> giu goc
	else if (nGiuaY > nRefH * 2 / 3) nNewT = nRefT + nDY;					// tu suy: phan duoi -> bam le duoi
	else                           nNewT = nRefT + nDY / 2;

	if (nNewL < 0) nNewL = 0;
	if (nNewT < 0) nNewT = 0;
	*pOutL = nNewL;
	*pOutT = nNewT;
}

void KWndWindow::FitToScreen()
{
	if (!m_bNeedFit)
		return;
	m_bNeedFit = 0;
	if (m_pParentWnd != NULL)		// chi cua so GOC (phong xa)
		return;
	// CHI neo cua so nao TU DANG KY (SetFitFlags). Da thu ap cho MOI cua so mot luot (nhu ban
	// USVOLAM) va DO thay HONG: ban nay da tu chinh san nhieu cua so theo SCREEN_WIDTH/HEIGHT ngay
	// trong ma (vi du UiPlayerBar co nhanh rieng cho 1024 va goi SetSize(SCREEN_WIDTH, ...)), nen neo
	// lai tu khung chuan 1024x768 la CHINH HAI LAN: thanh cong cu duoi day bi keo len ~82 diem anh va
	// khung trang tri lac cho. Vi vay de opt-in: cua so moi cua ban mobile goi SetFitFlags, cua so cu
	// giu nguyen duong da chay.
	if (m_FitFlags == 0)
		return;
	if (m_FitFlags & FIT_NOFIT)		// cua so tu dat cho
		return;
	if (SCREEN_WIDTH == 1024 && SCREEN_HEIGHT == 768)	// dung khung chuan: khong phai dich gi
		return;

	int nFlags = m_FitFlags & ~FIT_NOFIT;
	if (((nFlags & FIT_LEFT) && (nFlags & FIT_RIGHT)) ||
		((nFlags & FIT_TOP) && (nFlags & FIT_BOTTOM)))
	{
		int nNewW = ((nFlags & FIT_LEFT) && (nFlags & FIT_RIGHT)) ? SCREEN_WIDTH - m_Left : m_Width;
		int nNewH = ((nFlags & FIT_TOP) && (nFlags & FIT_BOTTOM)) ? SCREEN_HEIGHT - m_Top : m_Height;
		if (nNewW != m_Width || nNewH != m_Height)
			SetSize(nNewW, nNewH);
	}

	int nNewL, nNewT;
	ComputeFit(m_Left, m_Top, &nNewL, &nNewT);
	if (nNewL != m_Left || nNewT != m_Top)
		SetPosition(nNewL, nNewT);
}
#endif	// JX_MOBILE

void KWndWindow::BringToTop()
{
	if (m_pNextWnd)
	{
		KWndWindow*	pLast = m_pNextWnd;
		while(pLast->m_pNextWnd)
			pLast = pLast->m_pNextWnd;

		pLast->m_pNextWnd = this;
		m_pNextWnd->m_pPreviousWnd = m_pPreviousWnd;
		if (m_pPreviousWnd)
			m_pPreviousWnd->m_pNextWnd = m_pNextWnd;
		else if (m_pParentWnd && m_pParentWnd->m_pFirstChild == this)
			m_pParentWnd->m_pFirstChild = m_pNextWnd;
		m_pNextWnd = NULL;
		m_pPreviousWnd = pLast;
	}
}

//--------------------------------------------------------------------------
//	功能：获取窗口位置
//--------------------------------------------------------------------------
void KWndWindow::GetPosition(int* pLeft, int* pTop)
{
	if (pLeft)
		*pLeft = m_Left;
	if (pTop)
		*pTop = m_Top;
}

//--------------------------------------------------------------------------
//	功能：设置窗口位置（左上角相对于父窗口左上角位置的位移）
//--------------------------------------------------------------------------
void KWndWindow::SetPosition(int nLeft, int nTop)
{
	AbsoluteMove(nLeft - m_Left, nTop - m_Top);
	m_Left = nLeft;
	m_Top  = nTop;
}

//--------------------------------------------------------------------------
//	功能：获取窗口位置，绝对坐标
//--------------------------------------------------------------------------
void KWndWindow::GetAbsolutePos(int* pLeft, int* pTop)
{
	if (pLeft)
		*pLeft = m_nAbsoluteLeft;
	if (pTop)
		*pTop = m_nAbsoluteTop;
}

//--------------------------------------------------------------------------
//	功能：获取窗口大小
//--------------------------------------------------------------------------
void KWndWindow::GetSize(int* pWidth, int* pHeight)
{
	if (pWidth)
		*pWidth = m_Width;
	if (pHeight)
		*pHeight = m_Height;
}

//--------------------------------------------------------------------------
//	功能：设置窗口大小
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//	[UITOADO] dat lai ti le cua o nay (phan nghin, 1000 = 100%)
//--------------------------------------------------------------------------
void KWndWindow::UiDatTiLe(int nTiLe)
{
	if (nTiLe < 300)
		nTiLe = 300;
	if (nTiLe > 3000)
		nTiLe = 3000;

	// chup kich thuoc goc dung mot lan, sau do luon tinh TU GOC
	// => ap lai bao nhieu lan cung ra mot ket qua, khong nhan don
	if (m_nUiGocW == 0 && m_nUiGocH == 0)
	{
		m_nUiGocW = m_Width;
		m_nUiGocH = m_Height;
	}
	m_nUiTiLe = nTiLe;
	SetSize(m_nUiGocW * nTiLe / 1000, m_nUiGocH * nTiLe / 1000);
}

#ifdef JX_MOBILE
//--------------------------------------------------------------------------
//	[SUAGD 13/09] dat ti le nhung giu TAM o: UiDatTiLe neo goc tren-trai nen icon bam mep phai / duoi
//	phong to la troi ra ngoai; trinh chinh cho nguoi choi dung ham nay.
//--------------------------------------------------------------------------
void KWndWindow::UiDatTiLeQuanhTam(int nTiLe)
{
	int nW0 = m_Width, nH0 = m_Height;

	UiDatTiLe(nTiLe);
	if (m_Width != nW0 || m_Height != nH0)
		SetPosition(m_Left - (m_Width - nW0) / 2, m_Top - (m_Height - nH0) / 2);
}
//--------------------------------------------------------------------------
//	[PHONGBANG 14/09] Chu: 'cac o item trong hanh trang - ruong - Auto - cac bang tinh nang ... giu nguyen hien tai, chi
//	phong to theo kich thuoc moi dien thoai'. UiDatTiLe chi phong MOT cua so (con van o cho cu, chu van nho) nen bang bi
//	danh SUAGD_CO_KHONGPHONG. Ham nay phong ca CAY: chup goc mot lan (W/H qua m_nUiGocW/H, vi tri tuong doi cha, co chu)
//	TRUOC khi cha doi co (SetSize cua cha keo o con co WND_S_*_WITH_R/B_EDGE), roi tinh tu goc -> ap lai bao nhieu lan
//	cung ra mot ket qua. Moi o: chu truoc (UiDatFont) -> co (UiDatTiLe, anh gian that qua m_nUiTiLe) -> UiPhongRieng
//	(luoi vat pham tinh lai o); cha truoc con, con dat lai vi tri tuong doi tu goc. Vi tri cua CHINH bang do bo cuc dat.
//--------------------------------------------------------------------------
static void UiPhongChupGoc(KWndWindow* p)
{
	for (KWndWindow* c = p->GetFirstChild(); c; c = c->GetNextWnd())
	{
		c->UiChupGocViTri();
		c->UiChupGocCo();
		UiPhongChupGoc(c);
	}
}
static void UiPhongMotO(KWndWindow* p, int nTiLe)
{
	p->UiPhongChu(nTiLe);
	p->UiDatTiLe(nTiLe);
	p->UiPhongRieng(nTiLe);
	p->UiGhiPhongCay(nTiLe);
	for (KWndWindow* c = p->GetFirstChild(); c; c = c->GetNextWnd())
	{
		c->UiDatViTriTuGoc(nTiLe);
		UiPhongMotO(c, nTiLe);
	}
}
static void UiPhongXoaGoc(KWndWindow* p)
{
	p->UiXoaGoc();
	for (KWndWindow* c = p->GetFirstChild(); c; c = c->GetNextWnd())
		UiPhongXoaGoc(c);
}
void KWndWindow::UiPhongCay(int nTiLe)
{
	if (nTiLe < 500)
		nTiLe = 500;
	if (nTiLe > 3000)
		nTiLe = 3000;
	UiChupGocCo();
	UiPhongChupGoc(this);
	UiPhongMotO(this, nTiLe);
}
//	[c] o (va ca cay con) vua duoc Init lai theo toa do thiet ke trong luc bang dang phong (trang Auto doi tab):
//	xoa goc da chup, chup lai tu so do thiet ke, dat lai vi tri tuong doi cha roi phong ca cay nhu lan dau.
//	[KHOPO 14/09] tim o dau tien trong cay doi duoc k (luoi vat pham) - cac luoi trong mot bang cung co o nen mot k cho ca bang
static int UiKhopTiLeDe(KWndWindow* p, int nTiLe, int* pbCo)
{
	for (KWndWindow* c = p->GetFirstChild(); c && !*pbCo; c = c->GetNextWnd())
	{
		int k = c->UiKhopTiLe(nTiLe);
		if (k != nTiLe)
		{
			*pbCo = 1;
			return k;
		}
		k = UiKhopTiLeDe(c, nTiLe, pbCo);
		if (*pbCo)
			return k;
	}
	return nTiLe;
}
int KWndWindow::UiKhopTiLeCay(int nTiLe)
{
	int bCo = 0;
	int k = UiKhopTiLe(nTiLe);
	if (k != nTiLe)
		return k;
	return UiKhopTiLeDe(this, nTiLe, &bCo);
}
void KWndWindow::UiPhongChuCay(int nTiLe)
{
	UiPhongChu(nTiLe);
	for (KWndWindow* c = GetFirstChild(); c; c = c->GetNextWnd())
		c->UiPhongChuCay(nTiLe);
}
void KWndWindow::UiPhongLai(int nTiLe)
{
	UiPhongXoaGoc(this);
	UiChupGocViTri();
	UiDatViTriTuGoc(nTiLe);
	UiPhongCay(nTiLe);
}
#endif

//--------------------------------------------------------------------------
//	[UITOADO] an han / hien lai o nay
//--------------------------------------------------------------------------
void KWndWindow::UiDatAn(int bAn)
{
	if (bAn)
	{
		m_Style |= WND_S_UITOADO_AN;
		// khong di qua Hide() nen phai tu don chuot / tieu diem dang giu
		if (Wnd_GetFocusWnd() == this)
			Wnd_SetFocusWnd(NULL);
	}
	else
		m_Style &= ~WND_S_UITOADO_AN;
}

void KWndWindow::SetSize(int nWidth, int nHeight)
{
	if (nWidth < 0)
		nWidth = 0;
	if (nHeight < 0)
		nHeight = 0;
	int nDX = nWidth - m_Width;
	int nDY = nHeight - m_Height;
	if (nDX == 0 && nDY == 0)
		return;
	m_Width = nWidth;
	m_Height = nHeight;

	KWndWindow* pChild = m_pFirstChild;
	while(pChild)
	{
		int	x, y, w, h;
		pChild->GetPosition(&x, &y);
		pChild->GetSize(&w, &h);
		if (nDX)
		{
			if (pChild->m_Style & WND_S_SIZE_WITH_R_EDGE)
				pChild->SetSize(w + nDX, h);
			if (pChild->m_Style & WND_S_MOVE_WITH_R_EDGE)
				pChild->SetPosition(x + nDX, y);
		}
		if (nDY)
		{
			if (pChild->m_Style & WND_S_SIZE_WITH_B_EDGE)
				pChild->SetSize(w, h + nDY);
			if (pChild->m_Style & WND_S_MOVE_WITH_B_EDGE)
				pChild->SetPosition(x, y + nDY);
		}
		pChild = pChild->m_pNextWnd;
	};

	//to be check.!!!!!
	//一些窗口类可能会有根据窗口大小计算出一些变量保存下来供后继运算使用，
	//目前没有提供大小改变的通知消息，可能那些窗口的行为会有未知的结果。
	//但是目前情况来看那样的窗口一般不会被调用SetSize。
}

//--------------------------------------------------------------------------
//	功能：绝对坐标的调整
//--------------------------------------------------------------------------
void KWndWindow::AbsoluteMove(int dx, int dy)
{
	m_nAbsoluteLeft += dx;
	m_nAbsoluteTop  += dy;
	KWndWindow* pChild = m_pFirstChild;
	while(pChild)
	{
		pChild->AbsoluteMove(dx, dy);
		pChild = pChild->m_pNextWnd;
	}
}

//--------------------------------------------------------------------------
//	功能：显示窗口
//--------------------------------------------------------------------------
void KWndWindow::Show()
{
	m_Style |= WND_S_VISIBLE;
#ifdef JX_MOBILE
	if (m_pParentWnd == NULL)	// [PHONGBANG 14/09 f] cua so goc (anh em cua goc lop) hien len = da nap xong ca cay -> phong theo man hinh
		UiToaDo_PhongBangKhiHien(this);
#endif
}

//--------------------------------------------------------------------------
//	功能：隐藏窗口
//--------------------------------------------------------------------------
void KWndWindow::Hide()
{
	m_Style &= ~WND_S_VISIBLE;
	g_MouseOver.OnWndClosed((void*)this);
	g_MouseOverCompare.OnWndClosed((void*)this);
	KWndWindow* pFocus = Wnd_GetFocusWnd();
	while(pFocus)
	{
		if (pFocus != this)
			pFocus = pFocus->m_pParentWnd;
		else
		{
			Wnd_SetFocusWnd(NULL);
			break;
		}
	}
	Wnd_ReleaseExclusive(this);
}

//--------------------------------------------------------------------------
//	功能：判断窗口是否被显示
//--------------------------------------------------------------------------
int KWndWindow::IsVisible()
{
	return (m_Style & WND_S_VISIBLE);
}

//--------------------------------------------------------------------------
//	功能：禁止或者允许使窗口被操作
//--------------------------------------------------------------------------
void KWndWindow::Enable(int bEnable)
{
	if (bEnable)
		m_Style &= ~WND_S_DISABLE;
	else
		m_Style |= WND_S_DISABLE;
}

void KWndWindow::Destroy()
{
	m_Style |= WND_S_TOBEDESTROY;
}

void KWndWindow::Clone(KWndWindow* pCopy)
{
	if (pCopy)
	{
		pCopy->m_Width	= m_Width;
		pCopy->m_Height = m_Height;
		pCopy->m_Style = m_Style;
		pCopy->SetPosition(m_Left, m_Top);
	}
}

//--------------------------------------------------------------------------
//	功能：初始化窗口
//--------------------------------------------------------------------------
int KWndWindow::Init(KIniFile* pIniFile, const char* pSection)
{
	int		nValue1, nValue2;

	if (pIniFile && pSection)
	{
#ifdef _DEBUG
		strncpy(m_Caption, pSection, 32);
#endif
		// [UITOADO] nho ten muc ini de lam dinh danh khi luu / ap toa do
		strncpy(m_szMucIni, pSection, sizeof(m_szMucIni) - 1);
		m_szMucIni[sizeof(m_szMucIni) - 1] = 0;
		pIniFile->GetInteger(pSection, "Width", 0, &m_Width);
		pIniFile->GetInteger(pSection, "Height",0, &m_Height);
		pIniFile->GetInteger(pSection, "Left",  0, &nValue1);
		pIniFile->GetInteger(pSection, "Top",   0, &nValue2);
		SetPosition(nValue1, nValue2);		
#ifdef JX_MOBILE
		UiXoaGoc();	// [PHONGBANG 14/09 d] vua doc lai so do thiet ke -> bo goc da chup (Init lai luc bang dang phong: trang Auto)
		UiToaDo_ApChoOInit(this);	// [UITOADO 12/09 NEO b] o con Init sau khi goc dang ky -> ap toa do nguoi choi / bo cuc mac dinh ngay
#endif
		pIniFile->GetInteger(pSection, "Disable",  0, &nValue1);
		pIniFile->GetInteger(pSection, "Moveable", 0, &nValue2);
		if (nValue1)
			m_Style |= WND_S_DISABLE;
		else
			m_Style &= ~WND_S_DISABLE;
		if (nValue2)
			m_Style |= WND_S_MOVEALBE;
		else
			m_Style &= ~WND_S_MOVEALBE;

		nValue1 = nValue2 = 0;
		pIniFile->GetInteger2(pSection, "FollowMove", &nValue1, &nValue2);
		if (nValue1)
			m_Style |= WND_S_MOVE_WITH_R_EDGE;
		else
			m_Style &= ~WND_S_MOVE_WITH_R_EDGE;
		if (nValue2)
			m_Style |= WND_S_MOVE_WITH_B_EDGE;
		else
			m_Style &= ~WND_S_MOVE_WITH_B_EDGE;

		nValue1 = nValue2 = 0;
		pIniFile->GetInteger2(pSection, "FollowSize", &nValue1, &nValue2);
		if (nValue1)
			m_Style |= WND_S_SIZE_WITH_R_EDGE;
		else
			m_Style &= ~WND_S_SIZE_WITH_R_EDGE;
		if (nValue2)
			m_Style |= WND_S_SIZE_WITH_B_EDGE;
		else
			m_Style &= ~WND_S_SIZE_WITH_B_EDGE;
		pIniFile->GetInteger(pSection, "DummyWnd", 0, &nValue1);
		if (nValue1 == 0)
			m_Style &= ~WND_S_SIZE_WITH_ALL_CHILD;
		else
			m_Style |= WND_S_SIZE_WITH_ALL_CHILD;
		
#ifdef JX_MOBILE
		m_bNeedFit = 1;	// [ANDROID 09/09 NEO] khung vua dat tu ini -> con mot luot can lai
#endif
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：判断一个点是否落在窗口内,传入的是绝对坐标
//--------------------------------------------------------------------------
int KWndWindow::PtInWindow(int x, int y)
{
	int nRet = 0;
	// [UITOADO] o da bi "xoa" thi khong bat chuot nua - tru khi dang o
	// che do sua giao dien, luc do van phai bam duoc de bat hien lai
#ifdef JX_MOBILE
	if ((m_Style & WND_S_UITOADO_AN) && !UiToaDo_HienOAn())	// [SUAGD 13/09 e] chi khi trinh chinh dang "hien o da giau"
		return 0;
#else
	if ((m_Style & WND_S_UITOADO_AN) && !UiToaDo_DangSua())
		return 0;
#endif
	if (m_Style & WND_S_VISIBLE)
	{
		if ((m_Style & WND_S_SIZE_WITH_ALL_CHILD) == 0)
		{
			nRet = (x >= m_nAbsoluteLeft && y >= m_nAbsoluteTop &&
				x < m_nAbsoluteLeft + m_Width && y < m_nAbsoluteTop + m_Height);
		}
		else
		{
			KWndWindow* pChild = m_pFirstChild;
			while(pChild)
			{
				if (pChild->PtInWindow(x, y))
				{
					nRet = 1;
					break;
				}
				pChild = pChild->m_pNextWnd;
			};
		}
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：绘制窗口（包括子窗口与后继兄弟窗口）
//--------------------------------------------------------------------------
void KWndWindow::Paint()
{
	// [UITOADO] o da bi "xoa" thi khong ve nua (ca o con cua no) - tru khi
	// dang o che do sua giao dien, luc do van ve de con thay ma hien lai
#ifdef JX_MOBILE
	if ((m_Style & WND_S_VISIBLE) &&
		((m_Style & WND_S_UITOADO_AN) == 0 || UiToaDo_HienOAn()))	// [SUAGD 13/09 e] o da giau: chi ve khi dang "hien o da giau"
#else
	if ((m_Style & WND_S_VISIBLE) &&
		((m_Style & WND_S_UITOADO_AN) == 0 || UiToaDo_DangSua()))
#endif
	{
		PaintWindow();
		if (m_pFirstChild)
			m_pFirstChild->Paint();
	}
	if (m_pNextWnd)
		m_pNextWnd->Paint();
}

//--------------------------------------------------------------------------
//	功能：让窗口活动
//--------------------------------------------------------------------------
void KWndWindow::LetMeBreathe()
{
	if (m_pNextWnd)
		m_pNextWnd->LetMeBreathe();

	if (m_Style & WND_S_TOBEDESTROY)
		delete this;
	else if ((m_Style & (WND_S_VISIBLE | WND_S_DISABLE)) == WND_S_VISIBLE)
		Breathe();
}

//--------------------------------------------------------------------------
//	功能：添加子窗口
//--------------------------------------------------------------------------
void KWndWindow::AddChild(KWndWindow* pChild)
{
	if (pChild)
	{
		pChild->SplitSmaleFamily();

		//调整绝对坐标
		pChild->AbsoluteMove(m_nAbsoluteLeft + pChild->m_Left - pChild->m_nAbsoluteLeft,
			m_nAbsoluteTop + pChild->m_Top - pChild->m_nAbsoluteTop);

		//调整级连关系
		pChild->m_pParentWnd = this;
		if (m_pFirstChild == NULL)
			m_pFirstChild = pChild;
		else
		{
			KWndWindow* pBrother = m_pFirstChild;
			while(pBrother->m_pNextWnd)
				pBrother = pBrother->m_pNextWnd;
			pBrother->m_pNextWnd = pChild;
			pChild->m_pPreviousWnd = pBrother;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：添加子窗口
//--------------------------------------------------------------------------
void KWndWindow::AddBrother(KWndWindow* pBrother)
{
	if (pBrother)
	{
		pBrother->SplitSmaleFamily();

		//调整绝对坐标
		if (m_pParentWnd)
		{
			pBrother->AbsoluteMove(m_pParentWnd->m_nAbsoluteLeft + pBrother->m_Left - pBrother->m_nAbsoluteLeft,
				m_pParentWnd->m_nAbsoluteTop + pBrother->m_Top - pBrother->m_nAbsoluteTop);
		}

		//调整级连关系
		pBrother->m_pParentWnd = m_pParentWnd;
		if (m_pNextWnd == NULL)
		{
			m_pNextWnd = pBrother;
			pBrother->m_pPreviousWnd = this;
		}
		else
		{
			KWndWindow* pWnd = m_pNextWnd;
			while(pWnd->m_pNextWnd)
				pWnd = pWnd->m_pNextWnd;
			pWnd->m_pNextWnd = pBrother;
			pBrother->m_pPreviousWnd = pWnd;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：世间再无窗在我左右，一无牵连
//--------------------------------------------------------------------------
void KWndWindow::LeaveAlone()
{
	SplitSmaleFamily();
	while (m_pFirstChild)
	{
		m_pFirstChild->m_pParentWnd = NULL;
		m_pFirstChild = m_pFirstChild->m_pNextWnd;
	}
}

//--------------------------------------------------------------------------
//	功能：（抛父弃兄唯留子，成立小家庭），把自己（及子窗口）从窗口树里面里面分离出来
//--------------------------------------------------------------------------
void KWndWindow::SplitSmaleFamily()
{
	if (IsVisible())
	{
		KWndWindow::Hide();
		KWndWindow::Show();
	}
	if (m_pPreviousWnd)
		m_pPreviousWnd->m_pNextWnd = m_pNextWnd;
	else if (m_pParentWnd)
		m_pParentWnd->m_pFirstChild = m_pNextWnd;
	if (m_pNextWnd)
		m_pNextWnd->m_pPreviousWnd = m_pPreviousWnd;
	m_pPreviousWnd = NULL;
	m_pNextWnd     = NULL;
	m_pParentWnd   = NULL;
}

//--------------------------------------------------------------------------
//	功能：窗口函数（处理消息）
//--------------------------------------------------------------------------
int KWndWindow::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		OnLBtnDown(LOWORD(nParam), HIWORD(nParam));
		nRet = 1;
		break;
	case WM_RBUTTONDOWN:
		if (m_pParentWnd)
		{
			m_pParentWnd->WndProc(WND_M_POPUPMENU, (KUPARAM)(KWndWindow*)this, nParam);
			nRet = 1;
		}
		break;
	case WM_MOUSEMOVE:
		if (m_bMoving)
		{
			OnMoveWnd();
			nRet = 1;
		}
		break;
	case WM_LBUTTONUP:
		if (m_bMoving)
		{
			Wnd_ReleaseCapture();
			m_bMoving = false;
			if (m_pParentWnd)
			{
				m_pParentWnd->WndProc(WND_N_CHILD_MOVE, (KUPARAM)(KWndWindow*)this, 0);
			}
		}
		nRet = 1;
		break;
	case WND_M_CAPTURECHANGED:
		m_bMoving = false;
		nRet = 1;
		break;
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：响应鼠标左键按下的操作
//--------------------------------------------------------------------------
void KWndWindow::OnLBtnDown(int x, int y)
{
	if (m_Style & WND_S_MOVEALBE)
	{
		m_bMoving = TRUE;
		m_nLastMouseHoldPosX = x;
		m_nLastMouseHoldPosY = y;
		Wnd_SetCapture(this);
	}
}

//--------------------------------------------------------------------------
//	功能：响应鼠标左键按下移动的操作
//--------------------------------------------------------------------------
void KWndWindow::OnMoveWnd()
{
	int x, y;
	Wnd_GetCursorPos(&x, &y);
	if (x != m_nLastMouseHoldPosX || y != m_nLastMouseHoldPosY)
	{
		x -= m_nLastMouseHoldPosX;
		y -= m_nLastMouseHoldPosY;
		SetPosition(x + m_Left, y + m_Top);
		m_nLastMouseHoldPosX += x;
		m_nLastMouseHoldPosY += y;

		x = ((x & 0xffff) | ((y & 0xffff) << 16));
		if (x && m_pParentWnd)
		{
			m_pParentWnd->WndProc(WND_N_CHILD_MOVE, (KUPARAM)(KWndWindow*)this, x);
		}
	}
}

//--------------------------------------------------------------------------
//	功能：得到处于指定坐标位置的最上层窗口，传入的坐标为绝对坐标
//--------------------------------------------------------------------------
KWndWindow* KWndWindow::TopChildFromPoint(int x, int y)
{
	KWndWindow* pLastMatch = NULL;
	if (PtInWindow(x, y))
	{
		pLastMatch = this;
		KWndWindow*	pWnd = m_pFirstChild;
		while(pWnd)
		{
			//一系列同级的兄弟窗口，要从最上面（链表最末端的）开始判断
			while(pWnd->m_pNextWnd)
				pWnd = pWnd->m_pNextWnd;
			while(pWnd)
			{
				if (pWnd->PtInWindow(x, y) && !pWnd->IsDisable())
				{
					pLastMatch = pWnd;
					pWnd = pLastMatch->m_pFirstChild;
					break;
				}
				pWnd = pWnd->m_pPreviousWnd;
			}
		}
	}	
	return pLastMatch;
}

//--------------------------------------------------------------------------
//	功能：获得最顶层的父窗口
//--------------------------------------------------------------------------
KWndWindow* KWndWindow::GetOwner()
{
	KWndWindow* pWnd = this;
	while(pWnd->m_pParentWnd)
		pWnd = pWnd->m_pParentWnd;
	return pWnd;
}

//--------------------------------------------------------------------------
//	功能：设置窗口标题
//--------------------------------------------------------------------------
#ifdef _DEBUG
void KWndWindow::SetCaption(char* pszCaption)
{
	if (pszCaption)
		strncpy(m_Caption, pszCaption, 31);
}
#endif

void KWndWindow::PaintDebugInfo()
{
	char	szInfo[128];
	szInfo[0] = 0;
	int nInfoLen = 0;

	int nOld = WND_SHOW_DEBUG_FRAME_TEXT;
	WND_SHOW_DEBUG_FRAME_TEXT = true;
	KWndWindow::PaintWindow();
	WND_SHOW_DEBUG_FRAME_TEXT = nOld;

#ifdef _DEBUG
	sprintf(szInfo, "Name:%s, Pos:%d,%d,Size:%d,%d", m_Caption,
		m_nAbsoluteLeft, m_nAbsoluteTop, m_Width, m_Height);
	nInfoLen = strlen(szInfo);
#else
	sprintf(szInfo, "Pos:%d,%d,Size:%d,%d", m_nAbsoluteLeft, m_nAbsoluteTop, m_Width, m_Height);
	nInfoLen = strlen(szInfo);
#endif
	if (m_Style & WND_S_SIZE_WITH_ALL_CHILD)
	{
		RECT	rc;
		GetAllChildLayoutRect(&rc);
		sprintf(&szInfo[nInfoLen], ",Rect:%d,%d-%d,%d", rc.left, rc.top,
			rc.right, rc.bottom);
		nInfoLen = strlen(szInfo);
	}
	int x, y, w, h;
	Wnd_GetCursorPos(&x, &y);
	Wnd_GetScreenSize(w, h);
	if (x + nInfoLen * 6  + 24 > w)
		x = w - nInfoLen * 6 - 24;
	if (y + 25 > h)
		y = h - 13;
	else
		y += 12;
	g_pRepresentShell->OutputText(12, szInfo, nInfoLen, x, y, 0xFFFF0000,
		0, TEXT_IN_SINGLE_PLANE_COORD, 0xffffffff);
}


//--------------------------------------------------------------------------
//	功能：窗体绘制
//--------------------------------------------------------------------------
void KWndWindow::PaintWindow()
{
	if (m_bMoving)
		OnMoveWnd();

	if (g_pRepresentShell && WND_SHOW_DEBUG_FRAME_TEXT)
	{
		KRULine		Line[4];

		for(int i = 0; i < 4; i++)
		{
			Line[i].Color.Color_dw = 0xff0000ff;
			Line[i].oPosition.nX = Line[i].oEndPos.nX = m_nAbsoluteLeft;
			Line[i].oPosition.nY = Line[i].oEndPos.nY = m_nAbsoluteTop;
		}

		Line[0].oEndPos.nX += m_Width;
		Line[1].oPosition.nY += m_Height;
		Line[1].oEndPos.nX += m_Width;
		Line[1].oEndPos.nY += m_Height;
		Line[2].oEndPos.nY += m_Height;
		Line[3].oPosition.nX += m_Width;
		Line[3].oEndPos.nX += m_Width;
		Line[3].oEndPos.nY += m_Height;

		g_pRepresentShell->DrawPrimitives(4, Line, RU_T_LINE, true);

		if (m_Style & WND_S_SIZE_WITH_ALL_CHILD)
		{
			for(int i = 0; i < 4; i++)
				Line[i].Color.Color_dw = 0xffff0000;
			RECT	rc;
			GetAllChildLayoutRect(&rc);
			Line[0].oPosition.nX = Line[1].oPosition.nX = rc.left;
			Line[0].oEndPos.nX = Line[1].oEndPos.nX = rc.right;
			Line[0].oPosition.nY = Line[0].oEndPos.nY = rc.top;
			Line[1].oPosition.nY = Line[1].oEndPos.nY = rc.bottom;

			Line[0].oPosition.nY = Line[1].oPosition.nY = rc.top;
			Line[0].oEndPos.nY = Line[1].oEndPos.nY = rc.bottom;
			Line[0].oPosition.nX = Line[0].oEndPos.nX = rc.left;
			Line[1].oPosition.nX = Line[1].oEndPos.nX = rc.right;

			g_pRepresentShell->DrawPrimitives(4, Line, RU_T_LINE, true);
		}

#ifdef _DEBUG
		g_pRepresentShell->OutputText(12, m_Caption, -1, m_nAbsoluteLeft, m_nAbsoluteTop, 0xFFFFFFFF);
#endif
	}
}

//--------------------------------------------------------------------------
//	功能：把字符串表示的颜色信息转为数值表示
//--------------------------------------------------------------------------
unsigned int GetColor(LPCTSTR pString)
{
	if (pString == NULL)
		return 0;

	unsigned int Color = 0xFF000000;

	char Buf[16] = "";
	int  i = 0;
	int  n = 0;
	while (pString[i] != ',')
	{
		if (pString[i] == 0 || n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	
	Buf[n] = 0;
	Color += ((atoi(Buf) & 0xFF) << 16);
	n = 0;
	i++;
	while (pString[i] != ',')
	{
		if (pString[i] == 0 || n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	Buf[n] = 0;
	Color += ((atoi(Buf) & 0xFF) << 8);
	n = 0;
	i++;
	while (pString[i] != 0)
	{
		if (n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	Buf[n] = 0;
	Color += (atoi(Buf) & 0xFF);
	return Color;
}

const char* GetColorString(unsigned int nColor)
{
	static char szColor[12];
	KRColor c;
	c.Color_dw = nColor;
	sprintf(szColor, "%d,%d,%d", c.Color_b.r, c.Color_b.g, c.Color_b.b);
	szColor[11] = 0;

	return szColor;
}

//使鼠标指针以移动到悬浮在此窗口中的位置上
void KWndWindow::SetCursorAbove()
{
	Wnd_SetCursorPos(m_nAbsoluteLeft + m_Width / 2,
		m_nAbsoluteTop + m_Height / 2);
}

//取得包含所有子窗口分布区域的最小区域
void KWndWindow::GetAllChildLayoutRect(RECT* pRect)
{
	if (pRect == NULL)
		return;
	if (m_pFirstChild == NULL)
	{
		pRect->left = pRect->right = m_nAbsoluteLeft;
		pRect->top = pRect->bottom = m_nAbsoluteTop;
	}
	else
	{
		m_pFirstChild->GetAbsolutePos((int*)&pRect->left, (int*)&pRect->top);
		pRect->right = pRect->left;
		pRect->bottom = pRect->top;

		KWndWindow* pChild = m_pFirstChild;
		do
		{
			RECT	rc;
			pChild->GetAllChildLayoutRect(&rc);
			int		x, y, w, h;
			pChild->GetAbsolutePos(&x, &y);
			pChild->GetSize(&w, &h);

			if (rc.left > x)
				rc.left = x;
			if (rc.top > y)
				rc.top = y;
			if (rc.right < x + w)
				rc.right = x + w;
			if (rc.bottom < y + h)
				rc.bottom = y + h;

			if (pRect->left > rc.left)
				pRect->left = rc.left;
			if (pRect->top > rc.top)
				pRect->top = rc.top;
			if (pRect->right < rc.right)
				pRect->right = rc.right;
			if (pRect->bottom < rc.bottom)
				pRect->bottom = rc.bottom;
		}while(pChild = pChild->m_pNextWnd);
	}
}
