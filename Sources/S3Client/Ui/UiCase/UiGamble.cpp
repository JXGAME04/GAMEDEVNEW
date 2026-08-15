// -------------------------------------------------------------------------
//	ÎÄ¼₫Ăû		£º	UiGamble.cpp
//	´´½¨Ơß		£º	ÂÀ¹đ»ª, Wooy(Wu yue)
//	´´½¨Ê±¼ä	£º	2002-9-16 11:58:57
//	¹¦ÄÜĂèÊö	£º	
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiGamble.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/CoreObjGenreDef.h"
#include "../../../core/src/coreshell.h"

#include "../UiBase.h"
#include "UiInformation.h"
#include <crtdbg.h>

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI_GAMBLE		"UiGamble.ini"
#define	STR_WAIT_TRADING		"Chê x¸c nhËn"
#define	STR_OTHER_OK			"§èi ph­¬ng ®· kh\xe3\x61"
#define	STR_OTHER_NOT_OK		"§èi ph­¬ng ch­a kh\xe3\x61"

KUiGamble* KUiGamble::m_pSelf = NULL;

#define	SEND_MONEY_CHANGE_MSG_DELAY		16

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´̣¿ª´°¿Ú£¬·µ»ØÎ¨̉»µÄ̉»¸öÀà¶ÔÏóÊµÀư
//--------------------------------------------------------------------------
KUiGamble* KUiGamble::OpenWindow(KUiPlayerItem* pOther)
{
	if (g_UiBase.GetStatus() != UIS_S_IDLE)
		return NULL;
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiGamble;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		if (pOther)
			m_pSelf->m_OtherData = *pOther;
		m_pSelf->m_OtherName.SetText(pOther->Name);
		m_pSelf->UpdateTakewithItems();
		m_pSelf->UpdateOperData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
		g_UiBase.SetStatus(UIS_S_TRADE_PLAYER);
	}
	return m_pSelf;
}

KUiGamble* KUiGamble::GambleResetWindow(KUiPlayerItem* pOther)
{
	if (g_UiBase.GetStatus() != UIS_S_TRADE_PLAYER)
		return NULL;
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiGamble;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		//UiSoundPlay(UI_SI_WND_OPENCLOSE);
		if (pOther) {
			m_pSelf->m_OtherData = *pOther;
			m_pSelf->m_OtherName.SetText(pOther->Name);
		}
		m_pSelf->UpdateTakewithItems();
		m_pSelf->OnResetMoney();
		m_pSelf->UpdateOperData();
		m_pSelf->BringToTop();
		m_pSelf->Clear();
		m_pSelf->Show();
		g_UiBase.SetStatus(UIS_S_TRADE_PLAYER);
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÈç¹û´°¿ÚƠư±»ÏÔÊ¾£¬Ộ·µ»ØÊµÀưÖ¸Ơë
//--------------------------------------------------------------------------
KUiGamble* KUiGamble::GetIfVisible()
{
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±Ơ´°¿Ú£¬Í¬Ê±¿É̉ÔÑ¡ÔñÊÇ·ñÉ¾³ư¶ÔÏóÊµÀư
//--------------------------------------------------------------------------
void KUiGamble::CloseWindow()
{
	if (m_pSelf)
	{
		g_UiBase.SetStatus(UIS_S_IDLE);
		delete(m_pSelf);
		m_pSelf = NULL;
	}
}

KUiGamble::KUiGamble()
{
	m_nSelfTradeMoney = 0;
	m_bAdjustingMoney = 0;
	m_nTakewithMoney  = 0;
	memset(&m_OtherData, 0, sizeof(KUiPlayerItem));
	m_uWaitTradeMsgColor = 0xffffff;
	m_uOtherLockMsgColor = 0xffffff;
	m_uOtherNotLockMsgColor =0xffffff;
	m_bPick = -1;
}

KUiGamble::~KUiGamble()
{
}

//³ơÊ¼»¯
void KUiGamble::Initialize()
{
	AddChild(&m_TakewithMoney);
	AddChild(&m_TakewithItemsBox);
	AddChild(&m_SelfItemsBox);
	AddChild(&m_SelfMoney);
	AddChild(&m_Ok);
	AddChild(&m_Cancel);
	AddChild(&m_Trade);
	AddChild(&m_AddMoney);
	AddChild(&m_ReduceMoney);
	AddChild(&m_OtherName);
	AddChild(&m_OtherItemsBox);
	AddChild(&m_OtherMoney);
	AddChild(&m_Info);
	AddChild(&m_Scissors);
	AddChild(&m_Rock);
	AddChild(&m_Paper);
	AddChild(&m_SelfPick);
	AddChild(&m_Result);

	m_TakewithItemsBox.SetContainerId((int)UOC_ITEM_TAKE_WITH);
	m_SelfItemsBox.SetContainerId((int)UOC_TO_BE_GAMBLE);
	m_OtherItemsBox.SetContainerId((int)UOC_OTHER_TO_BE_GAMBLE);

	m_SelfMoney.SetIntText(0);
	m_OtherMoney.SetIntText(0);
	m_TakewithMoney.SetIntText(0);
	m_Info.SetText("", 0);
	m_OtherName.SetText("", 0);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_Trade.Enable(false);
	m_OtherItemsBox.EnablePickPut(false);
	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë½çĂæ·½°¸
//--------------------------------------------------------------------------
void KUiGamble::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_GAMBLE);
		if (Ini.Load(Buff))
		{
			m_pSelf->LoadScheme(&Ini);
		}
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë½çĂæ·½°¸
//--------------------------------------------------------------------------
void KUiGamble::LoadScheme(KIniFile* pIni)
{
	//_ASSERT(pIni);
	Init(pIni, "Main");
	m_TakewithItemsBox.Init(pIni, "TakewithItemsBox");
	m_TakewithMoney	.Init(pIni, "TakewithMoney");
	m_SelfItemsBox  .Init(pIni, "SelfItemsBox");
	m_SelfMoney		.Init(pIni, "SelfMoney");
	m_Ok			.Init(pIni, "OkBtn");
	m_Cancel		.Init(pIni, "CancelBtn");
	m_Trade			.Init(pIni, "TradeBtn");
	m_AddMoney		.Init(pIni, "AddMoney");
	m_ReduceMoney	.Init(pIni, "ReduceMoney");
	m_OtherName		.Init(pIni, "OtherName");
	m_OtherItemsBox .Init(pIni, "OtherItemsBox");
	m_OtherMoney	.Init(pIni, "OtherMoney");
	m_Info			.Init(pIni, "InfoText");
	m_OtherName		.SetText(m_OtherData.Name);
	m_Scissors		.Init(pIni, "SissorsBtn");
	m_Rock			.Init(pIni, "RockBtn");
	m_Paper			.Init(pIni, "PaperBtn");
	m_SelfPick		.Init(pIni, "SelfPickBtn");
	m_Result		.Init(pIni, "ResultBtn");

	char	szColor[16];
	pIni->GetString("InfoText", "WaitTradeMsgColor", "255,255,0", szColor, sizeof(szColor));
	m_uWaitTradeMsgColor = (::GetColor(szColor) & 0xffffff);
	pIni->GetString("InfoText", "LockMsgColor", "255,253,0", szColor, sizeof(szColor));
	m_uOtherLockMsgColor = (::GetColor(szColor) & 0xffffff);
	pIni->GetString("InfoText", "UnlockMsgColor", "255,0,0", szColor, sizeof(szColor));
	m_uOtherNotLockMsgColor = (::GetColor(szColor) & 0xffffff);
	
	m_SelfItemsBox.EnableTracePutPos(true);
	m_TakewithItemsBox.EnableTracePutPos(true);
}

void KUiGamble::UpdateTakewithItems()
{
	m_TakewithItemsBox.Clear();

	m_nTakewithMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	//m_TakewithMoney.SetIntText(m_nTakewithMoney);
	m_TakewithMoney.SetMoneyText(m_nTakewithMoney);

	m_nSelfTradeMoney = 0;
	m_SelfMoney.SetIntText(m_nSelfTradeMoney);

	KUiObjAtRegion* pObjs = NULL;
	int nCount = g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, 0, 0);
	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, (unsigned int)pObjs, nCount);
		for (int i = 0; i < nCount; i++)
		{
			KUiDraggedObject no;
			no.uGenre = pObjs[i].Obj.uGenre;
			no.uId = pObjs[i].Obj.uId;
			no.DataX = pObjs[i].Region.h;
			no.DataY = pObjs[i].Region.v;
			no.DataW = pObjs[i].Region.Width;
			no.DataH = pObjs[i].Region.Height;
			m_TakewithItemsBox.AddObject(&no, 1);
		}
		free(pObjs);
		pObjs = NULL;
	}
}

//¶Ô·½±ä¸üËæÉíÎïÆ·
void KUiGamble::OnChangedTakewithItem(KUiObjAtRegion* pObj, int bAdd)
{
	if (pObj)
	{
		//UiSoundPlay(UI_SI_PICKPUT_ITEM);
		KUiDraggedObject Obj;
		Obj.uGenre = pObj->Obj.uGenre;
		Obj.uId = pObj->Obj.uId;
		Obj.DataX = pObj->Region.h;
		Obj.DataY = pObj->Region.v;
		Obj.DataW = pObj->Region.Width;
		Obj.DataH = pObj->Region.Height;
		if (bAdd)
			m_TakewithItemsBox.AddObject(&Obj, 1);
		else
			m_TakewithItemsBox.RemoveObject(&Obj);
		UiSoundPlayItem(Obj.uId);
	}
	else
		UpdateTakewithItems();
}

void KUiGamble::setResult(BYTE choose)
{
	if(choose == 3)
		m_Result.SetFrame((int)choose);
	if(choose == 1)
		m_Result.SetFrame(2);
	if (choose == 2)
		m_Result.SetFrame(1);
}

//±ä¸ü½»̉×ÎïÆ·
void KUiGamble::OnSelfChangedItem(KUiObjAtRegion* pObj, int bAdd)
{
	_ASSERT(pObj);
	if (pObj->Obj.uGenre == CGOG_MONEY)
	{
		m_nSelfTradeMoney = pObj->Obj.uId;
		m_SelfMoney.SetIntText(pObj->Obj.uId);
	}
	else
	{
		//UiSoundPlay(UI_SI_PICKPUT_ITEM);
		KUiDraggedObject	obj;
		obj.uGenre = pObj->Obj.uGenre;
		obj.uId = pObj->Obj.uId;
		obj.DataX = pObj->Region.h;
		obj.DataY = pObj->Region.v;
		obj.DataW = pObj->Region.Width;
		obj.DataH = pObj->Region.Height;
		if (bAdd)
			m_SelfItemsBox.AddObject(&obj, 1);
		else
			m_SelfItemsBox.RemoveObject(&obj);
		UiSoundPlayItem(obj.uId);
	}
}

// -------------------------------------------------------------------------
// ¹¦ÄÜ	: ¶Ô·½±ä¸ü½»̉×ÎïÆ·
// -------------------------------------------------------------------------
void KUiGamble::OnOppositeChangedItem(KUiObjAtRegion* pObj, int bAdd)
{
	_ASSERT(pObj);
	if (pObj->Obj.uGenre == CGOG_MONEY)
	{
		//m_OtherMoney.SetIntText(pObj->Obj.uId);
		m_OtherMoney.SetMoneyText(pObj->Obj.uId); 
	}
	else
	{
		KUiDraggedObject	obj;
		obj.uGenre = pObj->Obj.uGenre;
		obj.uId = pObj->Obj.uId;
		obj.DataX = pObj->Region.h;
		obj.DataY = pObj->Region.v;
		obj.DataW = pObj->Region.Width;
		obj.DataH = pObj->Region.Height;
		if (bAdd)
			m_OtherItemsBox.AddObject(&obj, 1);
		else
			m_OtherItemsBox.RemoveObject(&obj);
	}
}

// -------------------------------------------------------------------------
// The other party agrees to the transaction
// -------------------------------------------------------------------------
void KUiGamble::UpdateOperData()
{
	int bLock = g_pCoreShell->GetGameData(GDI_GAMBLE_OPER_DATA, UGOD_IS_LOCKED, 0);
	m_Ok.CheckButton(bLock);

	if (g_pCoreShell->GetGameData(GDI_GAMBLE_OPER_DATA, UGOD_IS_GAMBLING, 0))
	{
		m_Trade.Enable(true);
		m_Trade.CheckButton(true);
		m_Info.SetText(STR_WAIT_TRADING, strlen(STR_WAIT_TRADING));
		//m_Info.SetTextColor(m_uWaitTradeMsgColor);
	}
	else
	{
		int bOtherLock = g_pCoreShell->GetGameData(GDI_GAMBLE_OPER_DATA, UGOD_IS_OTHER_LOCKED, 0);
		m_Trade.CheckButton(false);
		m_Trade.Enable(bLock && bOtherLock);
		m_Info.SetText(bOtherLock ? STR_OTHER_OK : STR_OTHER_NOT_OK, -1);
		//m_Info.SetTextColor(bOtherLock ? m_uOtherLockMsgColor : m_uOtherNotLockMsgColor);
	}

	m_SelfItemsBox.Enable(!bLock);
	m_SelfMoney.Enable(!bLock);
	m_TakewithItemsBox.Enable(!bLock);
	m_AddMoney.Enable(!bLock);
	m_ReduceMoney.Enable(!bLock);
	m_SelfMoney.Enable(!bLock);
}

// -------------------------------------------------------------------------
// ¹¦ÄÜ	: ´°¿Úº¯Êư
// -------------------------------------------------------------------------
int	KUiGamble::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Cancel)
			OnCancelTrade();
		else if (uParam == (unsigned int)(KWndWindow*)&m_Ok) {
			OnOk(nParam);
			m_Result.SetFrame(0);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_Trade)
			OnTrade();
		else if (uParam == (unsigned int)(KWndWindow*)&m_AddMoney)
		{
			OnAdjustMoney(true);
			m_bAdjustingMoney = 1;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ReduceMoney)
		{
			OnAdjustMoney(false);
			m_bAdjustingMoney = 1;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_Rock) {
			m_bPick = enumGAMBLE_Rock;
			m_Paper.SetFrame(0);
			m_Scissors.SetFrame(0);
			m_SelfPick.SetFrame(2);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_Paper) {
			m_bPick = enumGAMBLE_Paper;
			m_Rock.SetFrame(0);
			m_Scissors.SetFrame(0);
			m_SelfPick.SetFrame(1);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_Scissors) {
			m_bPick = enumGAMBLE_Scissors;
			m_Rock.SetFrame(0);
			m_Paper.SetFrame(0);
			m_SelfPick.SetFrame(3);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_Result || uParam == (unsigned int)(KWndWindow*)&m_SelfPick) {
			m_SelfPick.SetFrame(0);
			m_Result.SetFrame(0);
		}
		break;
	case WND_N_BUTTON_HOLD:
		if (uParam == (unsigned int)(KWndWindow*)&m_AddMoney)
		{			
			OnAdjustMoney(true);
			m_bAdjustingMoney = 1;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ReduceMoney)
		{
			OnAdjustMoney(false);
			m_bAdjustingMoney = 1;
		}
		break;
	case WND_N_EDIT_CHANGE:
		OnModififyMoney();
		break;
	case WND_N_ITEM_PICKDROP:
		OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiGamble::Breathe()
{
	if (m_bAdjustingMoney)
	{
		if ((++m_bAdjustingMoney) == SEND_MONEY_CHANGE_MSG_DELAY)
			OnAdjustedMoney();
	}
}

void KUiGamble::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	KUiObjAtContRegion	Pick, Drop;
	KUiDraggedObject	Obj;

	if (pPickPos)
	{
		if (pPickPos->pWnd == (KWndWindow*)&m_SelfItemsBox)
			Pick.eContainer = UOC_TO_BE_GAMBLE;
		else if (pPickPos->pWnd == (KWndWindow*)&m_TakewithItemsBox)
			Pick.eContainer = UOC_ITEM_TAKE_WITH;
		else
			return;
		((KWndObjectMatrix*)(pPickPos->pWnd))->GetObject(
			Obj, pPickPos->h, pPickPos->v);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = Obj.DataX;
		Pick.Region.v = Obj.DataY;
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
		if (pPickPos)
			Drop.eContainer = Pick.eContainer;
		else if (pDropPos->pWnd == (KWndWindow*)&m_SelfItemsBox)
			Drop.eContainer = UOC_TO_BE_GAMBLE;
		else if (pDropPos->pWnd == (KWndWindow*)&m_TakewithItemsBox)
			Drop.eContainer = UOC_ITEM_TAKE_WITH;
		else
			return;
	}
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}

void KUiGamble::OnAdjustMoney(bool bAdd)
{
	if (bAdd)
	{
		if (m_nTakewithMoney)
		{
			m_nTakewithMoney--;
			m_nSelfTradeMoney++;
		}
	}
	else if (m_nSelfTradeMoney)
	{
		m_nSelfTradeMoney--;
		m_nTakewithMoney++;
	}
	m_SelfMoney.SetIntText(m_nSelfTradeMoney);
	//m_TakewithMoney.SetIntText(m_nTakewithMoney);
	m_TakewithMoney.SetMoneyText(m_nTakewithMoney);
}

void KUiGamble::OnAdjustedMoney()
{
	OnModififyMoney();
	m_bAdjustingMoney = 0;
	if (g_pCoreShell)
	{
		KUiObjAtRegion	Obj = { 0 };
		Obj.Obj.uGenre = CGOG_MONEY;
		Obj.Obj.uId = m_nSelfTradeMoney;
		g_pCoreShell->OperationRequest(GOI_GAMBLE_DESIRE_ITEM,
			(unsigned int)&Obj, 0);
	}
}

void KUiGamble::OnModififyMoney()
{
	m_bAdjustingMoney = 1;
	int nNewSelfTradeMoney = m_SelfMoney.GetIntNumber();
	if (nNewSelfTradeMoney < 0)
		nNewSelfTradeMoney = 0;
	if (nNewSelfTradeMoney > m_nTakewithMoney + m_nSelfTradeMoney)
	{
		nNewSelfTradeMoney = m_nTakewithMoney + m_nSelfTradeMoney;
		m_SelfMoney.SetIntText(nNewSelfTradeMoney);
	}

	m_nTakewithMoney -= nNewSelfTradeMoney - m_nSelfTradeMoney;
	m_nSelfTradeMoney = nNewSelfTradeMoney;
	//m_TakewithMoney.SetIntText(m_nTakewithMoney);
	m_TakewithMoney.SetMoneyText(m_nTakewithMoney);
}

void KUiGamble::OnResetMoney()
{
	m_bAdjustingMoney = 1;
	int nNewSelfTradeMoney = 0;
	m_SelfMoney.SetIntText(nNewSelfTradeMoney);

	m_bAdjustingMoney = 0;
	if (g_pCoreShell)
	{
		KUiObjAtRegion	Obj = { 0 };
		Obj.Obj.uGenre = CGOG_MONEY;
		Obj.Obj.uId = m_nSelfTradeMoney;
		g_pCoreShell->OperationRequest(GOI_GAMBLE_DESIRE_ITEM,
			(unsigned int)&Obj, 0);
	}
}

//ḮÓ¦µă»÷¿ªÊ¼½»̉×
void KUiGamble::OnTrade()
{
	g_pCoreShell->OperationRequest(GOI_GAMBLE, 0, m_bPick);
	UpdateOperData();
}

//ḮÓ¦µă»÷ÓĐ½»̉×̉âỊ̈
void KUiGamble::OnOk(int bChecked)
{
	OnAdjustedMoney();
	g_pCoreShell->OperationRequest(GOI_GAMBLE_LOCK, 0, !!bChecked);
	UpdateOperData();
}

//Çå³ưÄÚÈƯ
void KUiGamble::Clear()
{
	m_SelfItemsBox.Clear();
	//m_nTakewithMoney += m_nSelfTradeMoney;
	m_nSelfTradeMoney = 0;
	m_SelfMoney.SetText("0");
	//m_TakewithMoney.SetIntText(m_nTakewithMoney);
	m_OtherItemsBox.Clear();
	m_OtherMoney.SetText("0");
	m_Info.SetText("");
	m_Trade.Enable(false);
	UpdateOperData();
}

void KUiGamble::OnCancelTrade()
{
	g_pCoreShell->OperationRequest(GOI_GAMBLE_CANCEL, 0, 0);
	CloseWindow();
}	

// -------------------------------------------------------------------------
// ¹¦ÄÜ	: ½»̉×½áÊø
// ²ÎÊư : bTraded±íÊ¾ÊÇ·ñ½øĐĐÁË½»̉×£¬Èç¹ûÈ¡0Öµ±íÊ¾¶Ô·½Àë¿ª½»̉×£¨½»̉×±»È¡Ïû£©
// -------------------------------------------------------------------------
void KUiGamble::UpdateTradeEnd(int bTraded)
{
	CloseWindow();
}
