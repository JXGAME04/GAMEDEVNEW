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

#include "../../../core/src/KAuctionUiDef.h"	// [DAUGIA 04/09 A6] o gia + nut doi tien trong chinh hop nay
#define	SCHEME_INI 	"UiGiveItem.ini"

// [A7] cua so co the CHUA duoc tao khi script bat che do -> nho lai, ap dung luc OpenWindow
static int s_bAucPending = 0;

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
	// [A7] ap dung che do ky gui da nho truoc khi cua so ton tai
	if (s_bAucPending)
		SetAuctionMode(1);
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
	// [A6] che do ky gui: an san, chi hien khi KUiAffairItem::SetAuctionMode(1)
	AddChild(&m_AucLabel);
	AddChild(&m_AucPrice);
	AddChild(&m_AucCur);
	AddChild(&m_AucUnit);
	// [A20 04/09] chu chot: nguoi ban nhap HAI gia - mua ngay va co ban (khoi diem)
	AddChild(&m_AucLabel2);
	AddChild(&m_AucBase);
	AddChild(&m_AucUnit2);
	m_bAucMode = 0;
	m_nAucCur = AUCUI_CUR_MONEY;
	m_AucLabel.Hide();
	m_AucPrice.Hide();
	m_AucCur.Hide();
	m_AucUnit.Hide();
	m_AucLabel2.Hide();
	m_AucBase.Hide();
	m_AucUnit2.Hide();
	
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
			// [A6] ba dieu khien cua che do ky gui (muc moi trong UiGiveItem.ini; thieu muc thi rong 0 -> khong hien)
			m_pSelf->m_AucLabel.Init(&Ini, "AucPriceLabel");
			m_pSelf->m_AucPrice.Init(&Ini, "AucPriceEdit");
			m_pSelf->m_AucCur.Init(&Ini, "AucCurBtn");
			m_pSelf->m_AucUnit.Init(&Ini, "AucUnitLabel");
			m_pSelf->m_AucLabel2.Init(&Ini, "AucBaseLabel");
			m_pSelf->m_AucBase.Init(&Ini, "AucBaseEdit");
			m_pSelf->m_AucUnit2.Init(&Ini, "AucBaseUnit");
			m_pSelf->m_ItemBox.EnableTracePutPos(true);
			m_pSelf->m_ItemBox.EnablePickPut(true);
		}
	}
}

// [DAUGIA 04/09 A6] Chu 04/09: gop lam MOT hop - hop dua vat pham nay khi ky gui dau gia se hien
// them o nhap GIA va nut doi LOAI TIEN. Bat truoc khi may chu mo hop; tat khi hop dong.
// [A23 04/09] chu bao "van bi nut ngan luong": nhan "Ngan luong" DAI HON anh cua nut.
// Nut dung sprite "nut nho bon chu" nen chi ve vua bon chu Han; noi Width trong ini KHONG
// lam anh rong ra (dot A7 da thu). Cach dung la rut nhan cho vua anh.
static const char* sAucCurName(int nCur)
{
	// TCVN3 tho (cung bang ma voi phan con lai cua client)
	return (nCur == AUCUI_CUR_XU) ? "Xu" : "L­îng";
}

// [A7] don vi hien sau o nhap: Ngan luong tinh theo VAN (x10.000), Xu tinh thang
static const char* sAucUnitName(int nCur)
{
	return (nCur == AUCUI_CUR_XU) ? "Xu" : "v¹n";
}

static int sAucMultiplier(int nCur)
{
	return (nCur == AUCUI_CUR_XU) ? 1 : 10000;
}


//--------------------------------------------------------------------------
//	Su kien
//--------------------------------------------------------------------------
int KUiAffairItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	// [A6] nut doi loai tien cua che do ky gui
	if (uMsg == WND_N_BUTTON_CLICK && uParam == (unsigned int)(KWndWindow*)&m_AucCur)
	{
		m_nAucCur = (m_nAucCur == AUCUI_CUR_XU) ? AUCUI_CUR_MONEY : AUCUI_CUR_XU;
		m_AucCur.SetLabel(sAucCurName(m_nAucCur));
		m_AucUnit.SetText(sAucUnitName(m_nAucCur));
		m_AucUnit2.SetText(sAucUnitName(m_nAucCur));	// [A20] doi don vi ca o gia co ban
		return 1;
	}
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
void KUiAffairItem::SetAuctionMode(int bOn)
{
	if (!m_pSelf)
	{
		// [A7] chua co cua so: nho lai, OpenWindow se ap dung (truoc day bam lan dau khong hien o gia)
		s_bAucPending = bOn ? 1 : 0;
		return;
	}
	s_bAucPending = 0;
	m_pSelf->m_bAucMode = bOn ? 1 : 0;
	m_pSelf->m_nAucCur = AUCUI_CUR_MONEY;
	if (bOn)
	{
		m_pSelf->m_AucPrice.SetText("1");	// [A7] co san so 1 de biet cho go
		m_pSelf->m_AucCur.SetLabel(sAucCurName(m_pSelf->m_nAucCur));
		m_pSelf->m_AucUnit.SetText(sAucUnitName(m_pSelf->m_nAucCur));
		m_pSelf->m_AucUnit.Show();
		m_pSelf->m_AucLabel.Show();
		m_pSelf->m_AucPrice.Show();
		m_pSelf->m_AucCur.Show();
		// [A20] o gia CO BAN (khoi diem) - phai thap hon gia mua ngay
		m_pSelf->m_AucBase.SetText("1");
		m_pSelf->m_AucUnit2.SetText(sAucUnitName(m_pSelf->m_nAucCur));
		m_pSelf->m_AucLabel2.Show();
		m_pSelf->m_AucBase.Show();
		m_pSelf->m_AucUnit2.Show();
	}
	else
	{
		m_pSelf->m_AucLabel.Hide();
		m_pSelf->m_AucPrice.Hide();
		m_pSelf->m_AucCur.Hide();
		m_pSelf->m_AucUnit.Hide();
		m_pSelf->m_AucLabel2.Hide();
		m_pSelf->m_AucBase.Hide();
		m_pSelf->m_AucUnit2.Hide();
	}
}

void KUiAffairItem::OnOk()
{
	// [A6] che do ky gui: bao GIA + LOAI TIEN qua kenh dau gia TRUOC khi gui OK cua hop
	// (cung mot duong truyen nen may chu nhan dung thu tu; khong doi cau truc goi nao).
	if (m_bAucMode && g_pCoreShell)
	{
		char szBuf[32];
		szBuf[0] = 0;
		m_AucPrice.GetText(szBuf, sizeof(szBuf), true);
		KAucUiReq r;
		memset(&r, 0, sizeof(r));
		// [A7] Ngan luong nhap theo VAN (x10.000), Xu nhap thang
		double dP = (double)atoi(szBuf) * (double)sAucMultiplier(m_nAucCur);
		if (dP > 2000000000.0)
			dP = 2000000000.0;
		r.nPrice = (int)dP;
		r.nType = m_nAucCur;
		// [A20] gia CO BAN di theo truong nId (duong nay bo trong) -> khong doi kich thuoc goi
		szBuf[0] = 0;
		m_AucBase.GetText(szBuf, sizeof(szBuf), true);
		double dB = (double)atoi(szBuf) * (double)sAucMultiplier(m_nAucCur);
		if (dB > 2000000000.0)
			dB = 2000000000.0;
		r.nId = (int)dB;
		g_pCoreShell->OperationRequest(GOI_AUCTION_UI, (unsigned int)AUCUI_OP_SET_PRICE, (int)&r);
		m_bAucMode = 0;
	}
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
	m_bAucMode = 0;
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