#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiSmelt.h"
#include "UiAffairItem.h"	// UiAffair_EncodeDesc (ma hoa <color>/<enter>, chong nuot the sau byte le TCVN3)
#include "UiItem.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern iCoreShell*		g_pCoreShell;

#define	SMELT_INI_MAIN		"smeltsystem\\smeltsystem.ini"
#define	SMELT_INI_SMELT		"smeltsystem\\smelt.ini"
#define	SMELT_INI_UNSMELT	"smeltsystem\\distill.ini"
#define	SMELT_EFFECT_FRAMES	25		// nhu MAX_SPR_FRAME cua lo ren (UiCompoundItem.cpp)

KUiSmelt* KUiSmelt::m_pSelf = NULL;

// ======================= TRANG =======================
KUiSmeltPage::KUiSmeltPage()
{
	m_nMode = 0;
	m_szIni[0] = 0;
	m_szRule[0] = 0;
	m_nEffect = 0;
}

void KUiSmeltPage::Initialize()
{
	AddChild(&m_Guide);
	AddChild(&m_Scroll);
	AddChild(&m_Pos1);
	AddChild(&m_Pos2);
	AddChild(&m_BtnDo);
	AddChild(&m_BtnCancel);
	AddChild(&m_BoxEquip);
	AddChild(&m_BoxMat);
	AddChild(&m_Effect);
	m_BoxEquip.SetContainerId((int)UOC_AFFAIR_ITEM);
	m_BoxMat.SetContainerId((int)UOC_AFFAIR_ITEM);
	m_Guide.SetScrollbar(&m_Scroll);
}

void KUiSmeltPage::LoadScheme(const char* pScheme, const char* pIni, int nMode, const char* pszRule)
{
	char		Buff[256];
	KIniFile	Ini;
	m_nMode = nMode;
	strncpy(m_szIni, pIni, sizeof(m_szIni) - 1);
	m_szIni[sizeof(m_szIni) - 1] = 0;
	m_szRule[0] = 0;
	if (pszRule)
	{
		strncpy(m_szRule, pszRule, sizeof(m_szRule) - 1);
		m_szRule[sizeof(m_szRule) - 1] = 0;
	}
	sprintf(Buff, "%s\\%s", pScheme, pIni);
	if (!Ini.Load(Buff))
		return;
	KWndImage::Init(&Ini, "Main");
	m_BoxEquip.Init(&Ini, "BigBox");
	m_BoxEquip.EnablePickPut(true);
	m_BoxMat.Init(&Ini, "SmallBox");
	m_BoxMat.EnablePickPut(true);
	m_Guide.Init(&Ini, "GuideList");
	m_Scroll.Init(&Ini, "GuideList_Scroll");
	m_BtnDo.Init(&Ini, nMode == 0 ? "SmeltBtn" : "UnSmeltBtn");
	m_BtnCancel.Init(&Ini, "CancelBtn");
	m_Effect.Init(&Ini, "EquipEffect");
	m_Effect.Hide();

	int nX = 0, nY = 0, nColor = 0xFFFFFF;
	if (Ini.GetString("TextColor", "Font", "", Buff, sizeof(Buff)) && Buff[0])
		nColor = (::GetColor(Buff) & 0xFFFFFF);
	Ini.GetInteger2("EquipPos", "Pos", &nX, &nY);
	m_Pos1.SetPosition(nX - 24, nY - 4);
	m_Pos1.SetTextColor(nColor);
	m_Pos1.BringToTop();
	m_Pos1.SetText("Trang bÞ");
	Ini.GetInteger2(nMode == 0 ? "SteelPos" : "CoalPos", "Pos", &nX, &nY);
	m_Pos2.SetPosition(nX - 28, nY - 4);
	m_Pos2.SetTextColor(nColor);
	m_Pos2.BringToTop();
	m_Pos2.SetText(nMode == 0 ? "V¨n C­¬ng" : "HuyÒn Háa Than");
	HienRuleInfo();
}

void KUiSmeltPage::ThemGuide(const char* pszText, bool bClear)
{
	if (bClear)
		m_Guide.Clear();
	if (!pszText || !pszText[0])
		return;
	char szDesc[1400];
	int nLen = UiAffair_EncodeDesc(pszText, szDesc, sizeof(szDesc));
	if (nLen > 0)
		m_Guide.AddOneMessage(szDesc, nLen);
}

void KUiSmeltPage::HienRuleInfo()
{
	ThemGuide(m_szRule, true);
}

void KUiSmeltPage::HienReturnInfo(int nKey)
{
	char		Buff[256];
	char		szKey[16];
	char		szText[512];
	KIniFile	Ini;
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	sprintf(Buff, "%s\\%s", Scheme, m_szIni);
	if (!Ini.Load(Buff))
		return;
	sprintf(szKey, "%d", nKey);
	szText[0] = 0;
	Ini.GetString("ReturnInfo", szKey, "", szText, sizeof(szText));
	ThemGuide(szText, true);
}

void KUiSmeltPage::HienThongTinMon(unsigned uId)
{
	if (!g_pCoreShell || uId == 0)
		return;
	char szBuf[1100];
	szBuf[0] = 0;
	int nCo = g_pCoreShell->GetGameData(GDI_FUSION_INFO, uId, (int)szBuf);
	m_Guide.Clear();
	ThemGuide(m_szRule, false);
	if (nCo > 0 && szBuf[0])
		ThemGuide(szBuf, false);
}

void KUiSmeltPage::BatHieuUng()
{
	m_Effect.SetFrame(0);
	m_Effect.Show();
	m_nEffect = 1;
}

void KUiSmeltPage::Breathe()
{
	if (m_nEffect != 1)
		return;
	if (m_Effect.GetCurrentFrame() >= SMELT_EFFECT_FRAMES)
	{
		m_Effect.SetFrame(0);
		m_Effect.Hide();
		m_nEffect = 0;
	}
	else
		m_Effect.NextFrame();
}

int KUiSmeltPage::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_ITEM_PICKDROP)
	{
		if (KUiSmelt::Instance())
			KUiSmelt::Instance()->OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		return 0;
	}
	if (uMsg == WND_N_SCORLLBAR_POS_CHANGED)
	{
		if (uParam == (unsigned int)(KWndWindow*)&m_Scroll)
			m_Guide.SetFirstShowLine(nParam);
		return 0;
	}
	// nut trong trang: KWndPage chuyen WND_N_BUTTON_CLICK len cha (KUiSmelt)
	return KWndPage::WndProc(uMsg, uParam, nParam);
}

// ======================= KHUNG CHA =======================
KUiSmelt::KUiSmelt()
{
	m_szFunc[0] = 0;
	m_uIdEquip = 0;
	m_uIdMat = 0;
}

KUiSmelt* KUiSmelt::OpenWindow(const char* pszTitle, const char* pszInitString, char* pszFunc)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiSmelt;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		if (!KUiItem::GetIfVisible())
			KUiItem::OpenWindow();
		m_pSelf->m_szFunc[0] = 0;
		if (pszFunc)
		{
			strncpy(m_pSelf->m_szFunc, pszFunc, sizeof(m_pSelf->m_szFunc) - 1);
			m_pSelf->m_szFunc[sizeof(m_pSelf->m_szFunc) - 1] = 0;
		}
		// server MO LAI box sau moi lan nop (doSmeltBox/doUnSmeltBox) -> chi ve trang 1 khi box dang AN
		if (!m_pSelf->IsVisible())
		{
			m_pSelf->ActivePage(0);
			m_pSelf->m_Page[0].HienRuleInfo();
			m_pSelf->m_Page[1].HienRuleInfo();
		}
		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
		Wnd_GameSpaceHandleInput(false);
	}
	return m_pSelf;
}

void KUiSmelt::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		Wnd_GameSpaceHandleInput(true);
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

KUiSmelt* KUiSmelt::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

void KUiSmelt::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_Close);
	AddChild(&m_TabBtn[0]);
	AddChild(&m_TabBtn[1]);
	m_Page[0].Initialize();
	AddPage(&m_Page[0], &m_TabBtn[0]);
	m_Page[1].Initialize();
	AddPage(&m_Page[1], &m_TabBtn[1]);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

void KUiSmelt::LoadScheme(const char* pScheme)
{
	if (!m_pSelf)
		return;
	char		Buff[256];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SMELT_INI_MAIN);
	if (!Ini.Load(Buff))
		return;

	m_pSelf->Init(&Ini, "Main");
	m_pSelf->m_Title.Init(&Ini, "txtTitle");
	m_pSelf->m_Close.Init(&Ini, "CloseBtn");
	m_pSelf->m_TabBtn[0].Init(&Ini, "SmeltBtn");
	m_pSelf->m_TabBtn[1].Init(&Ini, "UnSmeltBtn");
	char szRule[1024];
	szRule[0] = 0;
	Ini.GetString("RuleInfo", "Smelt", "", szRule, sizeof(szRule));
	m_pSelf->m_Page[0].LoadScheme(pScheme, SMELT_INI_SMELT, 0, szRule);
	szRule[0] = 0;
	Ini.GetString("RuleInfo", "UnSmelt", "", szRule, sizeof(szRule));
	m_pSelf->m_Page[1].LoadScheme(pScheme, SMELT_INI_UNSMELT, 1, szRule);

	if (SCREEN_WIDTH > 0 && SCREEN_HEIGHT > 0)
	{
		int nW = 0, nH = 0;
		m_pSelf->GetSize(&nW, &nH);
		if (nW > 0 && nH > 0)
			m_pSelf->SetPosition((SCREEN_WIDTH - nW) / 2, (SCREEN_HEIGHT - nH) / 2);
	}
}

void KUiSmelt::Breathe()
{
	m_Page[0].Breathe();
	m_Page[1].Breathe();
	// KWndWindow::Breathe la virtual PRIVATE (rong) - khong goi len lop cha
}

int KUiSmelt::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Close)
		{
			CloseWindow(true);
			return 0;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_Page[0].m_BtnDo)
		{
			OnDo(0);
			return 0;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_Page[1].m_BtnDo)
		{
			OnDo(1);
			return 0;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_Page[0].m_BtnCancel ||
			uParam == (unsigned int)(KWndWindow*)&m_Page[1].m_BtnCancel)
		{
			OnCancel();	// Huy = thu hoi do ve tui, box van mo
			return 0;
		}
		// con lai: 2 nut the -> KWndPageSet tu doi trang
		return KWndPageSet::WndProc(uMsg, uParam, nParam);
	case WM_KEYDOWN:
		if (uParam == VK_ESCAPE)
		{
			CloseWindow(true);
			return 1;
		}
		break;
	case WND_N_ITEM_PICKDROP:
		OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		return 0;
	default:
		break;
	}
	return KWndPageSet::WndProc(uMsg, uParam, nParam);
}

// Nut "Dung luyen" (nMode 0) / "Tinh che" (nMode 1): kiem so bo 2 o roi gui ham nop tuong ung
void KUiSmelt::OnDo(int nMode)
{
	if (nMode < 0 || nMode > 1)
		return;
	KUiSmeltPage& pg = m_Page[nMode];
	if (m_uIdEquip == 0 || m_uIdMat == 0)
	{
		pg.HienReturnInfo(nMode == 0 ? 4 : 19);	// "Hay bo vao trang bi ... va 1 ..."
		return;
	}
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 1,
			(unsigned int)(nMode == 0 ? "doSmeltBox" : "doUnSmeltBox"));
	pg.BatHieuUng();
}

void KUiSmelt::OnCancel()
{
	if (g_pCoreShell)
	{
		int nCount = g_pCoreShell->GetGameData(GDI_AFFAIR_ITEM, 0, 0);
		if (nCount)
			g_pCoreShell->OperationRequest(GOI_RECOVERY_BOX_COMMAND, pos_affairitem, 0);
	}
}

void KUiSmelt::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	KUiObjAtContRegion	Drop, Pick;
	KUiDraggedObject	Obj;
	KWndWindow* pWnd = NULL;

	if (pPickPos)
	{
		if (!pPickPos->pWnd)
			return;
		pWnd = pPickPos->pWnd;
		((KWndObjectBox*)(pPickPos->pWnd))->GetObject(Obj);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.v = 0;
		Pick.Region.h = 0;
		Pick.eContainer = UOC_AFFAIR_ITEM;
	}
	if (pDropPos)
	{
		if (pDropPos->pWnd)
			pWnd = pDropPos->pWnd;
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.v = 0;
		Drop.Region.h = 0;
		Drop.eContainer = UOC_AFFAIR_ITEM;
	}
	// Region.h = vai tro cua o (server doc bang GetGiveItemSlot): 0 trang bi | 1 nguyen lieu
	int nSlot = 0;
	for (int i = 0; i < 2; i++)
	{
		if (pWnd == (KWndWindow*)&m_Page[i].m_BoxEquip)
			nSlot = 0;
		else if (pWnd == (KWndWindow*)&m_Page[i].m_BoxMat)
			nSlot = 1;
	}
	Pick.Region.h = nSlot;
	Drop.Region.h = nSlot;
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
			pPickPos ? (unsigned int)&Pick : 0,
			pDropPos ? (int)&Drop : 0);
}

void KUiSmelt::UpdateData()
{
	int i;
	for (i = 0; i < 2; i++)
	{
		m_Page[i].m_BoxEquip.Celar();	// ten ham goc viet sai chinh ta, giu nguyen
		m_Page[i].m_BoxMat.Celar();
	}
	m_uIdEquip = 0;
	m_uIdMat = 0;
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

// Them (bAdd) / bot mot mon khoi o. Region.h = vai tro o da ghi luc tha.
void KUiSmelt::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (!pItem)
	{
		UpdateData();
		return;
	}
	if (pItem->Obj.uGenre == CGOG_MONEY)
		return;
	if (pItem->Region.v != 0)
		return;	// hang duoi khoang chua (phien give-box khac de lai): an
	int nSlot = pItem->Region.h;
	if (nSlot < 0 || nSlot > 1)
		return;
	int i;
	for (i = 0; i < 2; i++)
	{
		KWndObjectBox& Box = (nSlot == 0) ? m_Page[i].m_BoxEquip : m_Page[i].m_BoxMat;
		if (bAdd)
			Box.HoldObject(pItem->Obj.uGenre, pItem->Obj.uId, pItem->Region.Width, pItem->Region.Height);
		else
			Box.Celar();
	}
	if (nSlot == 0)
		m_uIdEquip = bAdd ? pItem->Obj.uId : 0;
	else
		m_uIdMat = bAdd ? pItem->Obj.uId : 0;
	// bang huong dan: mon vua dat vao -> thong tin mon; lay ra -> luat
	for (i = 0; i < 2; i++)
	{
		if (bAdd)
			m_Page[i].HienThongTinMon(pItem->Obj.uId);
		else if (m_uIdEquip)
			m_Page[i].HienThongTinMon(m_uIdEquip);
		else
			m_Page[i].HienRuleInfo();
	}
}
