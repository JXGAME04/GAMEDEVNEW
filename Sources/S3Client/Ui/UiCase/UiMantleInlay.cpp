#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiMantleInlay.h"
#include "UiItem.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"

// Kich thuoc man hinh that (dinh nghia o S3Client.cpp) -- dung de CAN GIUA bang,
// vi [Main] Left/Top trong tep ini la toa do cua ban Linux.
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern iCoreShell*		g_pCoreShell;

// Ten tep bo cuc: lay nguyen tu client VLTK (da chep vao bin\client\Ui\Ui3).
// Chuoi duoi day la ten tep GBK, viet bang escape BAT BUOC theo
// he BAT PHAN cho tep .cpp (khac he thap phan cua .lua) -- xem RULE 0.
#define	SCHEME_INI	"\320\307\263\275\312\257\317\342\307\266.ini"

KUiMantleInlay* KUiMantleInlay::m_pSelf = NULL;

KUiMantleInlay::KUiMantleInlay()
{
	m_szFunc[0] = 0;
}

//--------------------------------------------------------------------------
//	Mo
//--------------------------------------------------------------------------
KUiMantleInlay* KUiMantleInlay::OpenWindow(const char* pszTitle, const char* pszInitString, char* pszFunc)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiMantleInlay;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);

		// mo san tui do de nguoi choi keo da vao
		if (!KUiItem::GetIfVisible())
			KUiItem::OpenWindow();

		// [VA 31/08b] dung tieu de script gui len (truoc day pszTitle bi vut bo)
		if (pszTitle && pszTitle[0])
			m_pSelf->m_Title.SetText(pszTitle);
		m_pSelf->m_Guide.Clear();
		if (pszInitString)
			m_pSelf->m_Guide.AddOneMessage(pszInitString, strlen(pszInitString));
		m_pSelf->UpdateData();		// nap noi dung o ngay khi mo

		m_pSelf->m_szFunc[0] = 0;
		if (pszFunc)
		{
			strncpy(m_pSelf->m_szFunc, pszFunc, sizeof(m_pSelf->m_szFunc) - 1);
			m_pSelf->m_szFunc[sizeof(m_pSelf->m_szFunc) - 1] = 0;
		}

		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
		Wnd_GameSpaceHandleInput(false);
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	Dong -- tra lai do nguoi choi da dat vao (giong hop dat do)
//--------------------------------------------------------------------------
void KUiMantleInlay::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		Wnd_GameSpaceHandleInput(true);
		// [VA 31/08b] tra do ve tui TRUOC khi huy: ban cu gan m_pSelf = NULL roi
		// moi kiem if (m_pSelf) nen OnCancel() khong bao gio chay tren ca ba
		// duong dong (Huy / nut X / ESC deu truyen bDestroy = true) -> do ket
		// lai trong pos_affairitem, phai gap lai NPC moi thay.
		m_pSelf->OnCancel();
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
			m_pSelf->Hide();
	}
}

KUiMantleInlay* KUiMantleInlay::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	Khoi tao
//--------------------------------------------------------------------------
void KUiMantleInlay::Initialize()
{
	// HAI LOP nhu ban goc: cua so ngoai (vien/nut dong/tieu de) chua RUOT m_Pad.
	// Moi thu con lai la con cua m_Pad, nen toa do trong tep ruot (von la
	// toa do TUONG DOI voi ruot) dung nguyen khong phai chinh.
	AddChild(&m_Pad);
	AddChild(&m_Close);
	AddChild(&m_Title);
	AddChild(&m_TabStone);

	// Thu tu AddChild = thu tu ve: khung truoc, o dat do sau cung, de bieu
	// tuong vat pham khong bi anh khung de len.
	int i;
	for (i = 0; i < PF_UI_IMGBOTTOM; i++)
		m_Pad.AddChild(&m_ImgBottom[i]);
	for (i = 0; i < PF_UI_IMGUP; i++)
		m_Pad.AddChild(&m_ImgUp[i]);
	m_Pad.AddChild(&m_Guide);
	m_Pad.AddChild(&m_GuideScroll);
	m_Pad.AddChild(&m_OkBtn);
	m_Pad.AddChild(&m_CancelBtn);
	for (i = 0; i < PF_UI_SLOT_COUNT; i++)
		m_Pad.AddChild(&m_Obj[i]);
	for (i = 0; i < PF_UI_INPUT_COUNT; i++)
		m_Pad.AddChild(&m_Nhan[i]);		// nhan ve sau cung cho noi len tren

	m_Guide.SetScrollbar(&m_GuideScroll);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	// Ca 11 o dung CHUNG khoang chua cua hop dat do -> phia server thu do bang
	// dung duong cu (KJx2WarInfra_GiveBoxCollect), khong phai them gi.
	for (int j = 0; j < PF_UI_SLOT_COUNT; j++)
		m_Obj[j].SetContainerId((int)UOC_AFFAIR_ITEM);

	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	Nap bo cuc goc cua ban Linux
//--------------------------------------------------------------------------
// Chi so obj_N cua 6 o NHAN DO, xep theo dung thu tu ma server cho doi:
//   phan tu 0..4 -> Region.h 0..4 = Tinh Than Thach (tbItemIdx[1..5])
//   phan tu 5    -> Region.h 5    = Phi Phong       (tbItemIdx[6])
// obj_13 nam chinh giua hoa van ngoi sao nen dung lam o Phi Phong.
static const int PF_UI_INPUT[PF_UI_INPUT_COUNT] = { 10, 11, 12, 0, 1, 13 };

void KUiMantleInlay::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[256];
		char		Sect[32];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			// --- lop NGOAI ---
			{
				char		BuffOut[256];
				KIniFile	IniOut;
				sprintf(BuffOut, "%s\\\320\307\263\275\312\257\317\342\307\266\275\347\303\346.ini", pScheme);
				if (IniOut.Load(BuffOut))
				{
					m_pSelf->Init(&IniOut, "Main");			// anh VIEN cua bang
					m_pSelf->m_Close.Init(&IniOut, "CloseBtn");
					m_pSelf->m_Title.Init(&IniOut, "txtTitle");
					m_pSelf->m_TabStone.Init(&IniOut, "AddStarstoneBtn");
					m_pSelf->m_TabStone.CheckButton(TRUE);

					// Chu Viet dai hon chu Trung goc nen tran ra ngoai be rong cua
					// tep ini (tieu de 112, the 64). Noi rong cho vua, giu nguyen chu.
					int nWndW = 0, nWndH = 0;
					m_pSelf->GetSize(&nWndW, &nWndH);
					m_pSelf->m_Title.SetPosition((nWndW - 220) / 2, 7);
					m_pSelf->m_Title.SetSize(220, 18);
					m_pSelf->m_TabStone.SetSize(112, 21);

					// Tep ini goc dung PositionType=1 de tu can, nhung KWndWindow::Init
					// cua JX1 KHONG doc khoa do -> phai can giua bang tay.
					if (nWndW > 0 && nWndH > 0)
						m_pSelf->SetPosition((SCREEN_WIDTH - nWndW) / 2,
							(SCREEN_HEIGHT - nWndH) / 2);
				}
			}
			// --- RUOT: dat ben trong lop ngoai theo toa do cua chinh no ---
			m_pSelf->m_Pad.Init(&Ini, "Main");
			m_pSelf->m_Guide.Init(&Ini, "GuideList");
			m_pSelf->m_GuideScroll.Init(&Ini, "GuideList_Scroll");
			m_pSelf->m_OkBtn.Init(&Ini, "CompoundBtn");
			m_pSelf->m_CancelBtn.Init(&Ini, "CancleBtn");
			int i;
			// khung cua tung o -- thieu cai nay la nhin trong tron, khong thay o nao
			for (i = 0; i < PF_UI_IMGBOTTOM; i++)
			{
				sprintf(Sect, "imgBottom_%d", i);
				m_pSelf->m_ImgBottom[i].Init(&Ini, Sect);
			}
			for (i = 0; i < PF_UI_IMGUP; i++)
			{
				sprintf(Sect, "imgUp_%d", i);
				m_pSelf->m_ImgUp[i].Init(&Ini, Sect);
			}
			for (i = 0; i < PF_UI_SLOT_COUNT; i++)
			{
				sprintf(Sect, "obj_%d", i);
				m_pSelf->m_Obj[i].Init(&Ini, Sect);
				m_pSelf->m_Obj[i].EnablePickPut(false);
			}
			// Chi 6 o duoi day nhan tha do; obj_2..obj_9 la lo cua hoa van, de xem thoi.
			for (i = 0; i < PF_UI_INPUT_COUNT; i++)
				m_pSelf->m_Obj[PF_UI_INPUT[i]].EnablePickPut(true);

			// --- nhan ghi ro o nao bo gi (khuon KUiCompoundOne + muc [TextColor]) ---
			{
				char szMau[64] = "";
				int  nMau = 0xFFFFFFFF;
				if (Ini.GetString("TextColor", "Font", "", szMau, sizeof(szMau)))
					nMau = (::GetColor(szMau) & 0xFFFFFF);
				for (i = 0; i < PF_UI_INPUT_COUNT; i++)
				{
					int nX = 0, nY = 0, nW = 0, nH = 0;
					sprintf(Sect, "obj_%d", PF_UI_INPUT[i]);
					Ini.GetInteger(Sect, "Left", 0, &nX);
					Ini.GetInteger(Sect, "Top", 0, &nY);
					Ini.GetInteger(Sect, "Width", 24, &nW);
					Ini.GetInteger(Sect, "Height", 24, &nH);
					// 48px khong du cho "Tinh Than Thach" -> cat chu. Cho 96px va
					// can giua theo o.
					// [CHONLO 31/08] danh so o dung so lo server nhan (Region.h = i)
					// va kep x vao long m_Pad de nhan khong tran ra ngoai vien trai /
					// de len khung huong dan (obj_10 cu ra x=-24, obj_12 de GuideList).
					int nNhanW = 60;
					int nLx = nX + nW / 2 - nNhanW / 2;
					int nPadW = 0, nPadH = 0;
					m_pSelf->m_Pad.GetSize(&nPadW, &nPadH);
					if (nPadW > nNhanW && nLx > nPadW - nNhanW)
						nLx = nPadW - nNhanW;
					if (nLx < 0)
						nLx = 0;
					char szNhan[32];
					sprintf(szNhan, "L\347 kh\266m %d", i + 1);
					m_pSelf->m_Nhan[i].SetPosition(nLx, nY + nH + 1);
					m_pSelf->m_Nhan[i].SetSize(nNhanW, 14);
					m_pSelf->m_Nhan[i].SetTextColor(nMau);
					m_pSelf->m_Nhan[i].SetText(i == PF_UI_INPUT_COUNT - 1
						? "Phi Phong" : szNhan);
					m_pSelf->m_Nhan[i].BringToTop();
				}
			}
			// KHONG can giua tay nua: Left/Top cua lop NGOAI (132,100) da la toa do
			// man hinh that, con (4,49) cua ruot la toa do TUONG DOI trong lop ngoai.
		}
	}
}

//--------------------------------------------------------------------------
//	Su kien
//--------------------------------------------------------------------------
int KUiMantleInlay::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_OkBtn)
			OnOk();
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
			CloseWindow(true);
		else if (uParam == (unsigned int)(KWndWindow*)&m_Close)
			CloseWindow(true);		// nut X cua lop ngoai
		else if (uParam == (unsigned int)(KWndWindow*)&m_TabStone)
			m_TabStone.CheckButton(TRUE);	// [VA 31/08b] the duy nhat -> giu CHECKED
		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
		{
			OnOk();
			nRet = 1;
		}
		else if (uParam == VK_ESCAPE)
		{
			CloseWindow(true);
			nRet = 1;
		}
		break;
	case WND_N_ITEM_PICKDROP:
		OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_GuideScroll)
			m_Guide.SetFirstShowLine(nParam);
		break;
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	Bam kham -> chay ham Lua ben server (doMantleMosaicStoneBox)
//--------------------------------------------------------------------------
void KUiMantleInlay::OnOk()
{
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 1, (unsigned int)m_pSelf->m_szFunc);
}

//--------------------------------------------------------------------------
//	Huy -> tra do ve tui
//--------------------------------------------------------------------------
void KUiMantleInlay::OnCancel()
{
	if (g_pCoreShell)
	{
		int nCount = g_pCoreShell->GetGameData(GDI_AFFAIR_ITEM, 0, 0);
		if (nCount)
			g_pCoreShell->OperationRequest(GOI_RECOVERY_BOX_COMMAND, pos_affairitem, 0);
	}
}

// Khuon lay tu KUiCompoundOne::OnItemPickDrop (panel dung KWndObjectBox, khac
// hop dat do von dung KWndObjectMatrix). Khoang chua van la UOC_AFFAIR_ITEM de
// duong thu do ben server khong phai doi.
void KUiMantleInlay::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	KUiObjAtContRegion	Drop, Pick;
	KUiDraggedObject	Obj;
	KWndWindow*			pWnd = NULL;

	if (pPickPos)
	{
		if (!pPickPos->pWnd)
			return;
		((KWndObjectBox*)(pPickPos->pWnd))->GetObject(Obj);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.v = 0;
		Pick.eContainer = UOC_AFFAIR_ITEM;
		pWnd = pPickPos->pWnd;
	}
	else if (pDropPos)
	{
		pWnd = pDropPos->pWnd;
	}
	else
		return;

	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.v = 0;
		Drop.eContainer = UOC_AFFAIR_ITEM;
	}

	// Region.h phai la 0..5 theo dung vai tro ma server cho doi, KHONG phai
	// chi so obj_N -- bo nham la server tra "Hay dat vao Phi Phong can kham".
	for (int i = 0; i < PF_UI_INPUT_COUNT; i++)
	{
		if (pWnd == (KWndWindow*)&m_Obj[PF_UI_INPUT[i]])
		{
			Drop.Region.h = Pick.Region.h = i;
			break;
		}
	}
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
			pPickPos ? (unsigned int)&Pick : 0,
			pDropPos ? (int)&Drop : 0);
}

//--------------------------------------------------------------------------
//	Nap lai TOAN BO noi dung cac o tu khoang chua UOC_AFFAIR_ITEM.
//	Truoc day ham nay de RONG -> mon do bo vao khong bao gio duoc ve.
//	Khuon lay tu KUiAffairItem::UpdateData.
//--------------------------------------------------------------------------
void KUiMantleInlay::UpdateData()
{
	int i;
	for (i = 0; i < PF_UI_SLOT_COUNT; i++)
		m_Obj[i].Celar();		// ten ham goc viet sai chinh ta, giu nguyen

	if (!g_pCoreShell)
		return;
	int nCount = g_pCoreShell->GetGameData(GDI_AFFAIR_ITEM, 0, 0);
	if (nCount <= 0)
		return;

	KUiObjAtRegion* pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount);
	if (!pObjs)
		return;
	g_pCoreShell->GetGameData(GDI_AFFAIR_ITEM, (unsigned int)pObjs, nCount);
	for (i = 0; i < nCount; i++)
		UpdateItem(&pObjs[i], 1);
	free(pObjs);
}

//--------------------------------------------------------------------------
//	Them (bAdd != 0) hoac bot mot mon khoi o. Region.h chinh la CHI SO O ma
//	OnItemPickDrop da ghi vao luc tha do.
//--------------------------------------------------------------------------
void KUiMantleInlay::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (!pItem)
	{
		UpdateData();
		return;
	}
	if (pItem->Obj.uGenre == CGOG_MONEY)
		return;
	// [VA 31/08b] mon o hang duoi (v != 0) la do phien give-box khac de lai:
	// panel nay pick/drop deu gui v = 0 nen ve no ra chi tao "bong ma" khong
	// keo ra duoc / hai mon gop mot o. An di; nguoi choi don bang nut Huy
	// (thu hoi tat ca) hoac hop dat do thuong.
	if (pItem->Region.v != 0)
		return;
	int nSlot = pItem->Region.h;
	if (nSlot < 0 || nSlot >= PF_UI_INPUT_COUNT)
		return;
	nSlot = PF_UI_INPUT[nSlot];		// Region.h -> chi so obj_N
	if (bAdd)
		m_Obj[nSlot].HoldObject(pItem->Obj.uGenre, pItem->Obj.uId,
			pItem->Region.Width, pItem->Region.Height);
	else
		m_Obj[nSlot].Celar();
}
