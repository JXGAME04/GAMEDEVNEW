//---------------------------------------------------------------------------
// [VATPHAM 12/09] DAI NUT THAO TAC VAT PHAM cho ban Android. Xem chu thich o UiVatPham.h.
// Chi bien dich khi JX_ANDROID (chi nam trong android/CMakeLists.txt) - ban PC khong dinh gi.
//---------------------------------------------------------------------------
#include "KWin32.h"
#ifdef JX_ANDROID
#include "UiVatPham.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "../Elem/MouseHover.h"
#include "../UiBase.h"
#include "UiStoreBox.h"
#include "UiBreakItem.h"
#include "UiPlayerBar.h"
#include "../../../Core/src/coreshell.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "KDebug.h"
#include <string.h>
#include <stdio.h>

extern iCoreShell*		g_pCoreShell;
extern iRepresentShell*	g_pRepresentShell;
extern int				SCREEN_WIDTH;
extern int				SCREEN_HEIGHT;

//	ma nut
enum
{
	VP_DUNG = 1,		// Dung (thuoc, sach) / Trang bi (do mac duoc)
	VP_RAO,				// dinh vat pham vao khung soan chat (rao ban)
	VP_NEM,				// nem bo ra dat
	VP_DINH,			// dinh (khoa) vat pham
	VP_THAODINH,		// thao dinh
	VP_THAO,			// thao trang bi dang mac ra
	VP_TACH,			// tach chong vat pham
	VP_LAYRA,			// lay ra khoi ruong
	VP_CATRUONG,		// cat vao ruong
	VP_CHUYEN,			// chuyen tiep cai bam cu cho cua so chu (mua / ban / them vao / bo ra)
	VP_DONG,
};

#define VP_NUT_RONG		100		// dung co anh nut kho VNKU (android/anh_vatpham_vnku.py: nut ngoc UiAutoNew)
#define VP_NUT_CAO		34
#define VP_NUT_KHE		4
#define VP_NUT_MOI_HANG	3

KUiVatPham*	KUiVatPham::m_pSelf = NULL;

KUiVatPham::KUiVatPham()
{
	memset(&m_Obj, 0, sizeof(m_Obj));
	m_nDataX = m_nDataY = m_nDataW = m_nDataH = 0;
	m_eCont		= (UIOBJECT_CONTAINER)0;
	m_pChu		= NULL;
	m_nGenre	= -1;
	m_bKhoa		= 0;
	m_bChong	= 0;
	m_nSoNut	= 0;
	memset(m_anMaNut, 0, sizeof(m_anMaNut));
	memset(m_szAnhNut, 0, sizeof(m_szAnhNut));
}

KUiVatPham* KUiVatPham::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

bool KUiVatPham::DangMo()
{
	return GetIfVisible() != NULL;
}

void KUiVatPham::Dong()
{
	if (m_pSelf)
		m_pSelf->Hide();
}

void KUiVatPham::Huy()
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

//	Ngan nao duoc dung dai nut nay (cac ngan con lai giu nguyen thao tac cu: o phim tat, ky nang...).
static bool NganDungNut(int nCont)
{
	return (nCont == UOC_ITEM_TAKE_WITH || nCont == UOC_STORE_BOX || nCont == UOC_EQUIPTMENT
		 || nCont == UOC_NPC_SHOP || nCont == UOC_TO_BE_TRADE || nCont == UOC_ITEM_EX);
}

bool KUiVatPham::Mo(const KUiDraggedObject* pItem, UIOBJECT_CONTAINER eCont, int nX, int nY, KWndWindow* pChu)
{
	if (pItem == NULL || pItem->uId == 0 || g_pCoreShell == NULL)
		return false;
	if (!NganDungNut((int)eCont))
		return false;
	//	dang cam vat pham tren tay (keo tha) thi de thao tac cu chay
	if (Wnd_GetDragObj(NULL))
		return false;

	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiVatPham;
		if (m_pSelf == NULL)
			return false;
		m_pSelf->Initialize();
	}
	m_pSelf->m_pChu = pChu;
	m_pSelf->DungDaiNut(pItem, eCont, nX, nY);
	m_pSelf->Show();
	m_pSelf->BringToTop();
	return true;
}

void KUiVatPham::Initialize()
{
	int i;

	SetSize(VP_NUT_MOI_HANG * VP_NUT_RONG + (VP_NUT_MOI_HANG - 1) * VP_NUT_KHE, VP_NUT_CAO);
	m_Style &= ~WND_S_VISIBLE;
	for (i = 0; i < VP_MAX_NUT; i++)
	{
		m_Nut[i].SetSize(VP_NUT_RONG, VP_NUT_CAO);
		m_Nut[i].SetText("");
		m_Nut[i].Hide();
		AddChild(&m_Nut[i]);
	}
	Wnd_AddWindow(this, WL_TOPMOST);
}

//	Them mot nut: chu da ve san trong anh (kho VNKU); KWndPureTextBtn chi lo bat cham.
void KUiVatPham::ThemNut(int nMa, const char* pszTen)
{
	if (m_nSoNut >= VP_MAX_NUT)
		return;
	_snprintf(m_szAnhNut[m_nSoNut], sizeof(m_szAnhNut[0]) - 1, "\\spr\\ui3\\uivatpham\\nut_vp_%s.spr", pszTen);
	m_szAnhNut[m_nSoNut][sizeof(m_szAnhNut[0]) - 1] = 0;
	m_Nut[m_nSoNut].Show();
	m_anMaNut[m_nSoNut] = nMa;
	m_nSoNut++;
}

bool KUiVatPham::LayObj(KUiObjAtContRegion* pObj)
{
	if (pObj == NULL)
		return false;
	pObj->Obj			= m_Obj;
	pObj->Region.h		= m_nDataX;
	pObj->Region.v		= m_nDataY;
	pObj->Region.Width	= m_nDataW;
	pObj->Region.Height	= m_nDataH;
	pObj->nContainer	= (int)m_eCont;
	return true;
}

//---------------------------------------------------------------------------
//	Dung dai nut theo NGU CANH roi moi theo LOAI vat pham (thu tu cua ban tham khao USVOLAM),
//	roi dat ngay DUOI khung chu giai vat pham co san cua game.
//---------------------------------------------------------------------------
void KUiVatPham::DungDaiNut(const KUiDraggedObject* pItem, UIOBJECT_CONTAINER eCont, int nX, int nY)
{
	int	i, nHang, nRong, nCao;
	int	nKL = 0, nKT = 0, nKW = 0, nKH = 0;

	m_Obj.uGenre	= pItem->uGenre;
	m_Obj.uId		= pItem->uId;
	m_nDataX		= pItem->DataX;
	m_nDataY		= pItem->DataY;
	m_nDataW		= pItem->DataW;
	m_nDataH		= pItem->DataH;
	m_eCont			= eCont;
	m_nSoNut		= 0;
	for (i = 0; i < VP_MAX_NUT; i++)
		m_Nut[i].Hide();

	m_nGenre = g_pCoreShell->GetGenreItem2(m_Obj.uId);

	//	trang thai khoa (da dinh) + co xep chong khong: GDI_GET_ITEM_PARAM.
	//	Quy uoc "da dinh" lay dung nhu loi: m_Lock > 0 || m_HLock > 0 || m_Lock == LOCK_STATE_FOREVER.
	{
		ChatItem CItem;
		memset(&CItem, 0, sizeof(CItem));
		m_bKhoa  = 0;
		m_bChong = 0;
		if (g_pCoreShell->GetGameData(GDI_GET_ITEM_PARAM, (KUPARAM)&CItem, m_Obj.uId))
		{
			if (CItem.m_Lock > 0 || CItem.m_HLock > 0 || CItem.m_Lock == LOCK_STATE_FOREVER)
				m_bKhoa = 1;
			if (CItem.m_bStack > 1)
				m_bChong = 1;
		}
	}

	//	--- nut theo ngu canh ---
	if (KUiStoreBox::GetIfVisible())
	{
		if (eCont == UOC_STORE_BOX)
			ThemNut(VP_LAYRA, "lay_ra");
		else if (eCont == UOC_ITEM_TAKE_WITH)
			ThemNut(VP_CATRUONG, "cat_ruong");
		ThemNut(VP_RAO, "rao");
	}
	else if (eCont == UOC_EQUIPTMENT)
	{
		ThemNut(VP_THAO, "thao");
		ThemNut(VP_RAO,  "rao");
	}
	else if (eCont == UOC_NPC_SHOP || eCont == UOC_TO_BE_TRADE || eCont == UOC_ITEM_EX)
	{
		ThemNut(VP_CHUYEN, "chon");
		ThemNut(VP_RAO, "rao");
	}
	else	// hanh trang
	{
		ThemNut(VP_DUNG, (m_nGenre == item_equip) ? "trang_bi" : "dung");
		ThemNut(VP_RAO, "rao");
		if (!m_bKhoa)					// do da dinh thi loi khong cho nem
			ThemNut(VP_NEM, "nem");
		if (m_nGenre == item_equip)
			ThemNut(m_bKhoa ? VP_THAODINH : VP_DINH, m_bKhoa ? "thao_dinh" : "dinh");
		if (m_bChong)					// chi mon xep chong moi tach duoc
			ThemNut(VP_TACH, "tach");
	}
	ThemNut(VP_DONG, "dong");

	//	--- kich thuoc dai nut ---
	nHang = (m_nSoNut + VP_NUT_MOI_HANG - 1) / VP_NUT_MOI_HANG;
	if (nHang < 1)
		nHang = 1;
	nRong = (m_nSoNut < VP_NUT_MOI_HANG ? m_nSoNut : VP_NUT_MOI_HANG) * (VP_NUT_RONG + VP_NUT_KHE) - VP_NUT_KHE;
	nCao  = nHang * (VP_NUT_CAO + VP_NUT_KHE) - VP_NUT_KHE;
	SetSize(nRong, nCao);
	XepNut();

	//	--- dat NGAY DUOI khung chu giai vat pham co san (chu: "them nut duoi bang hien thong tin item") ---
	if (g_MouseOver.JxLayKhung(&nKL, &nKT, &nKW, &nKH) && nKW > 0)
	{
		nX = nKL + (nKW - nRong) / 2;
		nY = nKT + nKH + 2;
	}
	else
	{
		nX -= nRong / 2;
		nY += 16;
	}
	if (nX < 2)
		nX = 2;
	if (nX + nRong > SCREEN_WIDTH - 2)
		nX = SCREEN_WIDTH - 2 - nRong;
	if (nY < 2)
		nY = 2;
	if (nY + nCao > SCREEN_HEIGHT - 2)
		nY = SCREEN_HEIGHT - 2 - nCao;
	SetPosition(nX, nY);
}

void KUiVatPham::XepNut()
{
	int i, nCot, nHang;

	for (i = 0; i < m_nSoNut; i++)
	{
		nCot  = i % VP_NUT_MOI_HANG;
		nHang = i / VP_NUT_MOI_HANG;
		m_Nut[i].SetPosition(nCot * (VP_NUT_RONG + VP_NUT_KHE), nHang * (VP_NUT_CAO + VP_NUT_KHE));
	}
}

void KUiVatPham::PaintWindow()
{
	KRUImage	a;
	int			i, bx, by;

	if (!IsVisible() || g_pRepresentShell == NULL)
		return;

	memset(&a, 0, sizeof(a));
	a.nType = ISI_T_SPR;
	a.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	a.Color.Color_dw = 0xffffffff;
	a.nISPosition = IMAGE_IS_POSITION_INIT;
	a.nFrame = 0;
	for (i = 0; i < m_nSoNut; i++)
	{
		bx = by = 0;
		m_Nut[i].GetAbsolutePos(&bx, &by);
		//	PHAI xoa lai ma anh cho tung nut: Represent nho ma anh vao uImage/nISPosition ngay lan ve dau,
		//	dung lai ca cau truc thi moi nut deu ra ANH CUA NUT DAU TIEN.
		a.uImage = 0;
		a.nISPosition = IMAGE_IS_POSITION_INIT;
		strncpy(a.szImage, m_szAnhNut[i], sizeof(a.szImage) - 1);
		a.szImage[sizeof(a.szImage) - 1] = 0;
		a.oPosition.nX = bx;
		a.oPosition.nY = by;
		g_pRepresentShell->DrawPrimitives(1, &a, RU_T_IMAGE, true);
	}
	KWndWindow::PaintWindow();
}

//---------------------------------------------------------------------------
//	Lam theo nut vua bam. Moi lenh deu la lenh SAN CO cua ban PC (OperationRequest).
//---------------------------------------------------------------------------
void KUiVatPham::LamNut(int nMa)
{
	KUiObjAtContRegion	Obj;
	unsigned int		uPr[2];

	if (g_pCoreShell == NULL || m_Obj.uId == 0)
	{
		Dong();
		return;
	}
	LayObj(&Obj);

	switch (nMa)
	{
	case VP_DUNG:		// dung / mac trang bi (giong KUiItem)
		if (m_nGenre == item_equip)
			Obj.Region.Width = (g_pCoreShell->GetGameData(GDI_EQUIPMENT_SETNUM, 0, 0) == 1) ? pos_equip : pos_equipback;
		g_pCoreShell->OperationRequest(GOI_USE_ITEM, (KUPARAM)(&Obj), UOC_ITEM_TAKE_WITH);
		break;

	case VP_THAO:		// thao trang bi dang mac ra (giong KUiStatus: dich = hanh trang)
		Obj.Region.Width = pos_equiproom;
		g_pCoreShell->OperationRequest(GOI_USE_ITEM, (KUPARAM)(&Obj), UOC_EQUIPTMENT);
		break;

	case VP_LAYRA:		// ruong -> hanh trang (uPr[1] = NGUON, nParam = DICH)
		uPr[0] = m_Obj.uId;
		uPr[1] = pos_repositoryroom;
		g_pCoreShell->OperationRequest(GOI_EXCHANGEITEM, (KUPARAM)&uPr, pos_equiproom);
		break;

	case VP_CATRUONG:	// hanh trang -> ruong
		uPr[0] = m_Obj.uId;
		uPr[1] = pos_equiproom;
		g_pCoreShell->OperationRequest(GOI_EXCHANGEITEM, (KUPARAM)&uPr, pos_repositoryroom);
		break;

	case VP_NEM:		// nem bo (loi tu chan do khoa / vang bac)
		g_pCoreShell->OperationRequest(GDI_THROW_ALL_ITEM, (KUPARAM)(&Obj), 0);
		break;

	case VP_DINH:
		g_pCoreShell->OperationRequest(GOI_LOCK_PLAYER_ITEM, (KUPARAM)(&Obj), 1);
		break;

	case VP_THAODINH:
		g_pCoreShell->OperationRequest(GOI_LOCK_PLAYER_ITEM, (KUPARAM)(&Obj), 0);
		break;

	case VP_TACH:
		{
			KUiItemBuySelInfo	Price;
			memset(&Price, 0, sizeof(Price));
			KUiBreakItem::OpenWindow(&Obj, &Price, 100);
		}
		break;

	case VP_RAO:		// dinh vat pham vao khung soan chat (nhu Ctrl + bam ben PC)
		{
			ChatItem CItem;
			memset(&CItem, 0, sizeof(CItem));
			if (g_pCoreShell->GetGameData(GDI_GET_ITEM_PARAM, (KUPARAM)&CItem, m_Obj.uId))
				KUiPlayerBar::SetChatItem(CItem, m_Obj.uId);
		}
		break;

	case VP_CHUYEN:		// cua hang / giao dich: tra ve dung cai bam cu cua cua so chu
		if (m_pChu)
		{
			KUiDraggedObject o;
			memset(&o, 0, sizeof(o));
			o.uGenre = m_Obj.uGenre;
			o.uId    = m_Obj.uId;
			o.DataX  = m_nDataX;
			o.DataY  = m_nDataY;
			o.DataW  = m_nDataW;
			o.DataH  = m_nDataH;
			m_pChu->WndProc(WND_N_LEFT_CLICK_ITEM, (KUPARAM)&o, 0);
		}
		break;

	default:
		break;
	}
	Dong();
}

int KUiVatPham::WndProc(unsigned int uMsg, KUPARAM uParam, KNPARAM nParam)
{
	int i;

	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		for (i = 0; i < m_nSoNut; i++)
		{
			if ((KWndWindow*)uParam == (KWndWindow*)&m_Nut[i])
			{
				LamNut(m_anMaNut[i]);
				return 0;
			}
		}
		break;
	default:
		break;
	}
	return KWndWindow::WndProc(uMsg, uParam, nParam);
}
#endif	// JX_ANDROID
