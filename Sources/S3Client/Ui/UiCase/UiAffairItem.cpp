#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiAffairItem.h"
#include "UiItem.h"
#include "UiInformation.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "../../../Engine/src/KDebug.h"
#include "../../../Engine/src/Text.h"	// [BOXMAU 01/09] TEncodeText cho o mo ta
extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI 	"UiGiveItem.ini"

KUiAffairItem* KUiAffairItem::m_pSelf = NULL;

// [BOXMAU 01/09] Chuoi mo ta tu Lua den con THO ("<color=red>" chua phai byte dieu
// khien). Phai TEncodeText truoc khi do vao KWndMessageListBox - y het khuon
// UiCompoundItem.cpp:641. Truoc do nan truong hop '<' dung NGAY SAU chu Viet:
// TEncodeText (Text.cpp:468) coi byte >0x80 la chu 2 byte nen se nuot '<' - chen
// 1 dau cach lam byte-duoi thay the (khuon DTG_FixTagAfterVn UiTaskGuide.cpp:774).
// Sau ma hoa chuoi co the CHUA BYTE 0 (RGB) - phai dung so byte tra ve, cam strlen.
int UiAffair_EncodeDesc(const char* pszSrc, char* pszDst, int nDstCap)
{
	if (!pszSrc || !pszDst || nDstCap <= 0)
		return 0;
	char szTmp[1024];
	size_t i = 0, o = 0;
	size_t nLen = strlen(pszSrc);
	while (i < nLen && o + 3 < sizeof(szTmp))
	{
		unsigned char c = (unsigned char)pszSrc[i];
		if (c > 0x80 && i + 1 < nLen)
		{
			if (pszSrc[i + 1] == '<')
			{
				szTmp[o++] = pszSrc[i];
				szTmp[o++] = ' ';	// byte-duoi gia, hy sinh thay cho '<'
				i++;
			}
			else
			{
				szTmp[o++] = pszSrc[i];
				szTmp[o++] = pszSrc[i + 1];
				i += 2;
			}
		}
		else
		{
			// [BOXMAU 01/09b] byte Viet don le o CUOI chuoi: TEncodeText se DROP
			// (Text.cpp:476 nhanh else chi break) -> mat chu cuoi. Chen 1 dau cach
			// lam byte-duoi gia, cung meo da dung cho '<'.
			szTmp[o++] = pszSrc[i];
			if (c > 0x80 && i + 1 >= nLen)
				szTmp[o++] = ' ';
			i++;
		}
	}
	szTmp[o] = 0;
	int nOut = TEncodeText(szTmp, (int)o);
	if (nOut < 0)
		nOut = 0;
	if (nOut > nDstCap - 1)
		nOut = nDstCap - 1;
	memcpy(pszDst, szTmp, nOut);
	pszDst[nOut] = 0;
	return nOut;
}

KUiAffairItem::KUiAffairItem()
{

}

//--------------------------------------------------------------------------
//	Open
//--------------------------------------------------------------------------
KUiAffairItem* KUiAffairItem::OpenWindow(const char* pszTitle, const char* pszInitString, char* pszFunc1)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiAffairItem;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		
		if(!KUiItem::GetIfVisible())
			KUiItem::OpenWindow();			
		
		if (pszTitle)
			m_pSelf->m_Title.SetText(pszTitle);

		m_pSelf->m_ContentList.Clear();
			
		if (pszInitString)
		{
			// [BOXMAU 01/09] ma hoa the <color> truoc khi do vao khung mo ta
			char szDesc[600];
			int nDescLen = UiAffair_EncodeDesc(pszInitString, szDesc, sizeof(szDesc));
			if (nDescLen > 0)
				m_pSelf->m_ContentList.AddOneMessage(szDesc, nDescLen);
		}
			
		strcpy(m_pSelf->szFunc1, pszFunc1);
		
		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
		Wnd_GameSpaceHandleInput(false);			
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	Close
//--------------------------------------------------------------------------
void KUiAffairItem::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		Wnd_GameSpaceHandleInput(true);
		// [VA 31/08b] OnCancel truoc khi huy: ban cu goi qua con tro NULL sau
		// Destroy (hanh vi khong xac dinh - "chay duoc" chi vi OnCancel khong
		// cham bien thanh vien). Cung khuon voi KUiMantleInlay::CloseWindow.
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

KUiAffairItem*	KUiAffairItem::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	Khoi tao
//--------------------------------------------------------------------------
void KUiAffairItem::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_ContentList);
	AddChild(&m_ContentScroll);	
	AddChild(&m_OkBtn);
	AddChild(&m_CancelBtn);
	AddChild(&m_ItemBox);
	
	m_ContentList.SetScrollbar(&m_ContentScroll);	
	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_ItemBox.SetContainerId((int)UOC_AFFAIR_ITEM);
	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	Load
//--------------------------------------------------------------------------
void KUiAffairItem::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_Title.Init(&Ini, "Title");
			m_pSelf->m_ContentList.Init(&Ini, "ContentList");
			m_pSelf->m_ContentScroll.Init(&Ini, "ContentScroll");			
			m_pSelf->m_OkBtn.Init(&Ini, "Assemble");
			m_pSelf->m_CancelBtn.Init(&Ini, "Close");
			m_pSelf->m_ItemBox.Init(&Ini, "Items");
			m_pSelf->m_ItemBox.EnableTracePutPos(true);
			m_pSelf->m_ItemBox.EnablePickPut(true);
		}
	}
}

//--------------------------------------------------------------------------
//	Su kien
//--------------------------------------------------------------------------
int KUiAffairItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_OkBtn)
			OnOk();
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
			CloseWindow(true);
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
		if (uParam == (unsigned int)(KWndWindow*)&m_ContentScroll)
			m_ContentList.SetFirstShowLine(nParam);
		break;			
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}
//--------------------------------------------------------------------------
//	Ok
//--------------------------------------------------------------------------
void KUiAffairItem::OnOk()
{
	if (g_pCoreShell)
	{
		g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 1, (unsigned int)m_pSelf->szFunc1);
	}
}

//--------------------------------------------------------------------------
//	Cancel
//--------------------------------------------------------------------------
void KUiAffairItem::OnCancel()
{
	if (g_pCoreShell)
	{
		int nCount = g_pCoreShell->GetGameData(GDI_AFFAIR_ITEM, 0, 0);		
		if (nCount)	
			g_pCoreShell->OperationRequest(GOI_RECOVERY_BOX_COMMAND, pos_affairitem, 0);	
	}	
}

void KUiAffairItem::UpdateItem( KUiObjAtRegion* pItem, int bAdd )
{
	if (pItem)
	{
		bool open = false;
		if (bAdd == 2) {
			open = true;
			bAdd -= 1;
		}
		if (open)
			UiSoundPlay(UI_SI_PICKPUT_ITEM);
		if (pItem->Obj.uGenre != CGOG_MONEY)
		{
			KUiDraggedObject Obj;
			Obj.uGenre = pItem->Obj.uGenre;
			Obj.uId = pItem->Obj.uId;
			Obj.DataX = pItem->Region.h;
			Obj.DataY = pItem->Region.v;
			Obj.DataW = pItem->Region.Width;
			Obj.DataH = pItem->Region.Height;
			if (bAdd)
				m_ItemBox.AddObject(&Obj, 1);
			else
				m_ItemBox.RemoveObject(&Obj);
			if (!open)
				UiSoundPlayItem(Obj.uId);
		}
	}
	else
		UpdateData();
}

void KUiAffairItem::UpdateData()
{
	m_ItemBox.Clear();

	KUiObjAtRegion* pObjs = NULL;

	int nCount = g_pCoreShell->GetGameData(GDI_AFFAIR_ITEM, 0, 0);
	if (nCount <= 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_AFFAIR_ITEM, (unsigned int)pObjs, nCount);
		for (int i = 0; i < nCount; i++)
			UpdateItem(&pObjs[i], 2);
		free(pObjs);
		pObjs = NULL;
	}
}

void KUiAffairItem::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	if (!g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM) && 
		!g_UiBase.IsOperationEnable(UIS_O_TRADE_ITEM))
		return;
	KUiObjAtContRegion	Pick, Drop;
	KUiDraggedObject	Obj;
	
	UISYS_STATUS eStatus = g_UiBase.GetStatus();
	if (pPickPos)
	{
		if(!pPickPos->pWnd) return;		
		((KWndObjectMatrix*)(pPickPos->pWnd))->GetObject(
			Obj, pPickPos->h, pPickPos->v);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = Obj.DataX;
		Pick.Region.v = Obj.DataY;
		Pick.eContainer = UOC_AFFAIR_ITEM;
	}
	
	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.h = pDropPos->h;
		Drop.Region.v = pDropPos->v;
		Drop.eContainer = UOC_AFFAIR_ITEM;	
	}
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}