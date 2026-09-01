#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiMantleWash.h"
#include "UiAffairItem.h"	// UiAffair_EncodeDesc (ma hoa <color>/<enter>)
#include "UiItem.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern iCoreShell*		g_pCoreShell;

// 3 tep bo cuc rut tu pak client VLTK (chu Viet TCVN3 giu nguyen byte, spr doi sang ASCII)
#define	WASH_INI_MAIN		"mantleimplicit_main.ini"
#define	WASH_INI_SHIFT		"mantleimplicit_pageshift.ini"
#define	WASH_INI_ACTIVATE	"mantleimplicit_pageactivate.ini"

KUiMantleWash* KUiMantleWash::m_pSelf = NULL;

// Doc khoa Text cua mot section (co the dai > 256) roi ma hoa the, do vao listbox.
static void sNapMoTa(KIniFile* pIni, const char* pSection, KWndMessageListBox& Box)
{
	char szRaw[1024];
	szRaw[0] = 0;
	Box.Clear();
	if (!pIni->GetString(pSection, "Text", "", szRaw, sizeof(szRaw)))
		return;
	if (!szRaw[0])
		return;
	char szDesc[1200];
	int nLen = UiAffair_EncodeDesc(szRaw, szDesc, sizeof(szDesc));
	if (nLen > 0)
		Box.AddOneMessage(szDesc, nLen);
}

// Dat mot day thanh/chu theo template (Left/Top/StepY) - khuon VLTK.
static void sDatTheoTemplate(KIniFile* pIni, const char* pSection, KWndWindow* pWnd, int nIdx)
{
	int nL = 0, nT = 0, nStep = 22;
	pIni->GetInteger(pSection, "Left", 0, &nL);
	pIni->GetInteger(pSection, "Top", 0, &nT);
	pIni->GetInteger(pSection, "StepY", 22, &nStep);
	pWnd->SetPosition(nL, nT + nIdx * nStep);
}

// ======================= TRANG 1 =======================
KUiMantleWashPageShift::KUiMantleWashPageShift()
{
}

void KUiMantleWashPageShift::Initialize()
{
	AddChild(&m_ImgBg);
	int i;
	for (i = 0; i < PF_WASH_LINE; i++)
		AddChild(&m_BarBefore[i]);
	for (i = 0; i < PF_WASH_LINE; i++)
		AddChild(&m_BarAfter[i]);
	AddChild(&m_ImgArrow);
	AddChild(&m_ImgItemFrame);
	AddChild(&m_DescTitle);
	AddChild(&m_Desc);
	AddChild(&m_TxtBeforeHdr);
	AddChild(&m_TxtAfterHdr);
	for (i = 0; i < PF_WASH_LINE; i++)
		AddChild(&m_TxtBefore[i]);
	for (i = 0; i < PF_WASH_LINE; i++)
		AddChild(&m_TxtAfter[i]);
	AddChild(&m_TxtConsume);
	AddChild(&m_BtnKeep);
	AddChild(&m_BtnApply);
	AddChild(&m_BtnWash);
	AddChild(&m_ItemSlot);
	m_ItemSlot.SetContainerId((int)UOC_AFFAIR_ITEM);
}

void KUiMantleWashPageShift::LoadScheme(const char* pScheme)
{
	char		Buff[256];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, WASH_INI_SHIFT);
	if (!Ini.Load(Buff))
		return;
	Init(&Ini, "Main");
	m_ImgBg.Init(&Ini, "ImgBg");
	m_DescTitle.Init(&Ini, "DescTitle");
	m_Desc.Init(&Ini, "Desc");
	sNapMoTa(&Ini, "Desc", m_Desc);
	m_TxtBeforeHdr.Init(&Ini, "TxtAttribBefore");
	m_TxtAfterHdr.Init(&Ini, "TxtAttribAfter");
	m_ImgArrow.Init(&Ini, "ImgArrow");
	m_ImgItemFrame.Init(&Ini, "ImgItemFrame");
	m_ItemSlot.Init(&Ini, "ObjBoxItem");
	m_ItemSlot.EnablePickPut(true);
	m_TxtConsume.Init(&Ini, "TxtConsume");
	m_BtnKeep.Init(&Ini, "BtnRemain");
	m_BtnApply.Init(&Ini, "BtnApply");
	m_BtnWash.Init(&Ini, "BtnKeepOn");

	// day thanh + chu theo template; chu dat TUONG DOI so voi thanh (Left/Top cua _Txt)
	int nTxL = 0, nTxT = 0;
	int i;
	Ini.GetInteger("AttribBefore_ItemTemplate_Txt", "Left", 0, &nTxL);
	Ini.GetInteger("AttribBefore_ItemTemplate_Txt", "Top", 0, &nTxT);
	for (i = 0; i < PF_WASH_LINE; i++)
	{
		m_BarBefore[i].Init(&Ini, "AttribBefore_ItemTemplate");
		sDatTheoTemplate(&Ini, "AttribBefore_ItemTemplate", &m_BarBefore[i], i);
		m_TxtBefore[i].Init(&Ini, "AttribBefore_ItemTemplate_Txt");
		int x = 0, y = 0;
		m_BarBefore[i].GetPosition(&x, &y);
		m_TxtBefore[i].SetPosition(x + nTxL + 3, y + nTxT);
		m_TxtBefore[i].SetText("");
	}
	Ini.GetInteger("AttribAfter_ItemTemplate_Txt", "Left", 0, &nTxL);
	Ini.GetInteger("AttribAfter_ItemTemplate_Txt", "Top", 0, &nTxT);
	for (i = 0; i < PF_WASH_LINE; i++)
	{
		m_BarAfter[i].Init(&Ini, "AttribAfter_ItemTemplate");
		sDatTheoTemplate(&Ini, "AttribAfter_ItemTemplate", &m_BarAfter[i], i);
		m_TxtAfter[i].Init(&Ini, "AttribAfter_ItemTemplate_Txt");
		int x = 0, y = 0;
		m_BarAfter[i].GetPosition(&x, &y);
		m_TxtAfter[i].SetPosition(x + nTxL + 3, y + nTxT);
		m_TxtAfter[i].SetText("");
	}
}

void KUiMantleWashPageShift::VeHaiCot(const char szTruoc[PF_WASH_LINE][128], const char szSau[PF_WASH_LINE][128])
{
	for (int i = 0; i < PF_WASH_LINE; i++)
	{
		m_TxtBefore[i].SetText(szTruoc[i]);
		m_TxtAfter[i].SetText(szSau[i]);
	}
}

int KUiMantleWashPageShift::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_ITEM_PICKDROP)
	{
		if (KUiMantleWash::Instance())
			KUiMantleWash::Instance()->OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		return 0;
	}
	// nut trong trang: KWndPage chuyen WND_N_BUTTON_CLICK len cha (KUiMantleWash)
	return KWndPage::WndProc(uMsg, uParam, nParam);
}

// ======================= TRANG 2 =======================
KUiMantleWashPageActivate::KUiMantleWashPageActivate()
{
}

void KUiMantleWashPageActivate::Initialize()
{
	AddChild(&m_ImgBg);
	int i;
	for (i = 0; i < PF_WASH_LINE; i++)
		AddChild(&m_AttrBgInactive[i]);
	for (i = 0; i < PF_WASH_LINE; i++)
		AddChild(&m_AttrBgActive[i]);
	AddChild(&m_ImgItemFrame);
	AddChild(&m_DescTitle);
	AddChild(&m_Desc);
	for (i = 0; i < PF_WASH_LINE; i++)
		AddChild(&m_AttrTxt[i]);
	AddChild(&m_TxtActivate);
	AddChild(&m_TxtConsume);
	AddChild(&m_BtnActivate);
	AddChild(&m_ItemSlot);
	m_ItemSlot.SetContainerId((int)UOC_AFFAIR_ITEM);
}

void KUiMantleWashPageActivate::LoadScheme(const char* pScheme)
{
	char		Buff[256];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, WASH_INI_ACTIVATE);
	if (!Ini.Load(Buff))
		return;
	Init(&Ini, "Main");
	m_ImgBg.Init(&Ini, "ImgBg");
	m_DescTitle.Init(&Ini, "DescTitle");
	m_Desc.Init(&Ini, "Desc");
	sNapMoTa(&Ini, "Desc", m_Desc);
	m_ImgItemFrame.Init(&Ini, "ImgItemFrame");
	m_ItemSlot.Init(&Ini, "ObjBoxItem");
	m_ItemSlot.EnablePickPut(true);
	m_TxtActivate.Init(&Ini, "TxtActivate");
	m_TxtConsume.Init(&Ini, "TxtConsume");
	m_BtnActivate.Init(&Ini, "BtnActivate");
	// JX1: dong an luon hieu luc, khong co co che kich hoat co thoi han -> tat nut
	m_BtnActivate.Enable(FALSE);

	int nTxL = 0, nTxT = 0;
	Ini.GetInteger("Attrib_Template_Desc", "Left", 0, &nTxL);
	Ini.GetInteger("Attrib_Template_Desc", "Top", 0, &nTxT);
	for (int i = 0; i < PF_WASH_LINE; i++)
	{
		m_AttrBgInactive[i].Init(&Ini, "Attrib_Template_Bg");
		sDatTheoTemplate(&Ini, "Attrib_Template", &m_AttrBgInactive[i], i);
		m_AttrBgActive[i].Init(&Ini, "Attrib_Template_BgActive");
		sDatTheoTemplate(&Ini, "Attrib_Template", &m_AttrBgActive[i], i);
		m_AttrBgActive[i].Hide();
		m_AttrTxt[i].Init(&Ini, "Attrib_Template_Desc");
		int x = 0, y = 0;
		m_AttrBgInactive[i].GetPosition(&x, &y);
		m_AttrTxt[i].SetPosition(x + nTxL, y + nTxT);
		m_AttrTxt[i].SetText("");
	}
}

void KUiMantleWashPageActivate::VeDongAn(const char szDong[PF_WASH_LINE][128], int nCo)
{
	for (int i = 0; i < PF_WASH_LINE; i++)
	{
		bool bCo = (i < nCo && szDong[i][0] != 0);
		m_AttrTxt[i].SetText(bCo ? szDong[i] : "");
		// co dong -> nen "da kich hoat" + chu xanh; khong -> nen "chua kich hoat" + chu xam
		if (bCo)
		{
			m_AttrBgActive[i].Show();
			m_AttrBgInactive[i].Hide();
			m_AttrTxt[i].SetTextColor((::GetColor("HGreen") & 0xFFFFFF));
		}
		else
		{
			m_AttrBgActive[i].Hide();
			m_AttrBgInactive[i].Show();
			m_AttrTxt[i].SetTextColor((::GetColor("Gray") & 0xFFFFFF));
		}
	}
}

int KUiMantleWashPageActivate::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_ITEM_PICKDROP)
	{
		if (KUiMantleWash::Instance())
			KUiMantleWash::Instance()->OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		return 0;
	}
	return KWndPage::WndProc(uMsg, uParam, nParam);
}

// ======================= KHUNG CHA =======================
KUiMantleWash::KUiMantleWash()
{
	m_szFunc[0] = 0;
	m_bDaTay = false;
	for (int i = 0; i < PF_WASH_LINE; i++)
		m_szTruoc[i][0] = 0;
}

KUiMantleWash* KUiMantleWash::OpenWindow(const char* pszTitle, const char* pszInitString, char* pszFunc)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiMantleWash;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		if (!KUiItem::GetIfVisible())
			KUiItem::OpenWindow();

		// Tieu de + mo ta lay tu ini (dung chu VLTK); tham so Lua chi giu ten ham nop.
		m_pSelf->m_szFunc[0] = 0;
		if (pszFunc)
		{
			strncpy(m_pSelf->m_szFunc, pszFunc, sizeof(m_pSelf->m_szFunc) - 1);
			m_pSelf->m_szFunc[sizeof(m_pSelf->m_szFunc) - 1] = 0;
		}
		// [PB 01/09] server MO LAI box sau moi lan roll (PF_MoLaiWashBox) -> chi xoa snapshot
		// cot TRUOC khi box dang AN (phien moi); mo lai giua phien thi GIU de so sanh truoc/sau.
		if (!m_pSelf->IsVisible())
		{
			m_pSelf->m_bDaTay = false;
			for (int i = 0; i < PF_WASH_LINE; i++)
				m_pSelf->m_szTruoc[i][0] = 0;
			m_pSelf->ActivePage(0);
		}
		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
		Wnd_GameSpaceHandleInput(false);
	}
	return m_pSelf;
}

void KUiMantleWash::CloseWindow(bool bDestroy)
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

KUiMantleWash* KUiMantleWash::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

void KUiMantleWash::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_Close);
	AddChild(&m_TabBtn[0]);
	AddChild(&m_TabBtn[1]);

	m_PageShift.Initialize();
	AddPage(&m_PageShift, &m_TabBtn[0]);
	m_PageActivate.Initialize();
	AddPage(&m_PageActivate, &m_TabBtn[1]);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

void KUiMantleWash::LoadScheme(const char* pScheme)
{
	if (!m_pSelf)
		return;
	char		Buff[256];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, WASH_INI_MAIN);
	if (!Ini.Load(Buff))
		return;

	m_pSelf->Init(&Ini, "Main");
	m_pSelf->m_Title.Init(&Ini, "Title");
	m_pSelf->m_Close.Init(&Ini, "Close");
	m_pSelf->m_TabBtn[0].Init(&Ini, "PageButton_0");
	m_pSelf->m_TabBtn[1].Init(&Ini, "PageButton_1");
	m_pSelf->m_PageShift.LoadScheme(pScheme);
	m_pSelf->m_PageActivate.LoadScheme(pScheme);

	if (SCREEN_WIDTH > 0 && SCREEN_HEIGHT > 0)
	{
		int nW = 0, nH = 0;
		m_pSelf->GetSize(&nW, &nH);
		if (nW > 0 && nH > 0)
			m_pSelf->SetPosition((SCREEN_WIDTH - nW) / 2, (SCREEN_HEIGHT - nH) / 2);
	}
}

int KUiMantleWash::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Close)
		{
			CloseWindow(true);
			return 0;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_PageShift.m_BtnWash)
		{
			OnWash();
			return 0;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_PageShift.m_BtnKeep)
		{
			OnKeepOld();
			return 0;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_PageShift.m_BtnApply)
		{
			OnApplyNew();
			return 0;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_PageActivate.m_BtnActivate)
			return 0;	// JX1 khong co kich hoat co thoi han
		// con lai: 2 nut tab -> KWndPageSet tu doi trang
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

// "Tay luyen": chup dong an HIEN TAI lam cot TRUOC, roi goi ham Lua roll.
void KUiMantleWash::OnWash()
{
	char szDong[PF_WASH_LINE][128];
	int nCo = LayDongAn(m_PageShift.m_ItemSlot, szDong);
	for (int i = 0; i < PF_WASH_LINE; i++)
	{
		if (i < nCo)
		{
			strncpy(m_szTruoc[i], szDong[i], sizeof(m_szTruoc[i]) - 1);
			m_szTruoc[i][sizeof(m_szTruoc[i]) - 1] = 0;
		}
		else
			m_szTruoc[i][0] = 0;
	}
	m_bDaTay = true;
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 1, (unsigned int)m_szFunc);
}

void KUiMantleWash::OnKeepOld()
{
	// [PB 01/09] giu nguyen -> server khoi phuc dong cu = dong hien tai -> xoa snapshot
	m_bDaTay = false;
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 1, (unsigned int)"doWashKeep");
}

void KUiMantleWash::OnApplyNew()
{
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 1, (unsigned int)"doWashApply");
	CloseWindow(true);
}

void KUiMantleWash::OnCancel()
{
	if (g_pCoreShell)
	{
		int nCount = g_pCoreShell->GetGameData(GDI_AFFAIR_ITEM, 0, 0);
		if (nCount)
			g_pCoreShell->OperationRequest(GOI_RECOVERY_BOX_COMMAND, pos_affairitem, 0);
	}
}

void KUiMantleWash::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	KUiObjAtContRegion	Drop, Pick;
	KUiDraggedObject	Obj;

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
		Pick.Region.h = 0;
		Pick.eContainer = UOC_AFFAIR_ITEM;
	}
	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.v = 0;
		Drop.Region.h = 0;
		Drop.eContainer = UOC_AFFAIR_ITEM;
	}
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
			pPickPos ? (unsigned int)&Pick : 0,
			pDropPos ? (int)&Drop : 0);
}

void KUiMantleWash::UpdateData()
{
	m_PageShift.m_ItemSlot.Celar();		// ten ham goc viet sai chinh ta, giu nguyen
	m_PageActivate.m_ItemSlot.Celar();
	if (!g_pCoreShell)
		return;
	int nCount = g_pCoreShell->GetGameData(GDI_AFFAIR_ITEM, 0, 0);
	if (nCount <= 0)
	{
		VeLai();
		return;
	}
	KUiObjAtRegion* pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount);
	if (!pObjs)
		return;
	g_pCoreShell->GetGameData(GDI_AFFAIR_ITEM, (unsigned int)pObjs, nCount);
	for (int i = 0; i < nCount; i++)
		UpdateItem(&pObjs[i], 1);
	free(pObjs);
}

void KUiMantleWash::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (!pItem)
	{
		UpdateData();
		return;
	}
	if (pItem->Obj.uGenre == CGOG_MONEY)
		return;
	if (pItem->Region.v != 0)
		return;
	if (bAdd)
	{
		m_PageShift.m_ItemSlot.HoldObject(pItem->Obj.uGenre, pItem->Obj.uId, pItem->Region.Width, pItem->Region.Height);
		m_PageActivate.m_ItemSlot.HoldObject(pItem->Obj.uGenre, pItem->Obj.uId, pItem->Region.Width, pItem->Region.Height);
	}
	else
	{
		m_PageShift.m_ItemSlot.Celar();
		m_PageActivate.m_ItemSlot.Celar();
	}
	VeLai();
}

int KUiMantleWash::LayDongAn(KWndObjectBox& Slot, char szDong[PF_WASH_LINE][128])
{
	for (int i = 0; i < PF_WASH_LINE; i++)
		szDong[i][0] = 0;
	if (!g_pCoreShell)
		return 0;
	KUiDraggedObject Obj;
	Slot.GetObject(Obj);
	if (Obj.uId == 0)
		return 0;
	char szBuf[512];
	szBuf[0] = 0;
	int nCo = g_pCoreShell->GetGameData(GDI_MANTLE_HIDDEN_DESC, (unsigned int)Obj.uId, (int)szBuf);
	if (nCo <= 0)
		return 0;
	int nLine = 0;
	char* p = szBuf;
	while (p && *p && nLine < PF_WASH_LINE)
	{
		char* q = strchr(p, '\n');
		int nLen = q ? (int)(q - p) : (int)strlen(p);
		if (nLen > 127)
			nLen = 127;
		memcpy(szDong[nLine], p, nLen);
		szDong[nLine][nLen] = 0;
		nLine++;
		if (!q)
			break;
		p = q + 1;
	}
	return nLine;
}

// Ve lai ca 2 trang tu item dang trong o.
void KUiMantleWash::VeLai()
{
	char szNay[PF_WASH_LINE][128];
	int nCo = LayDongAn(m_PageShift.m_ItemSlot, szNay);
	// cot PHAI = dong hien tai; cot TRAI = snapshot neu da tay, khong thi = hien tai
	if (m_bDaTay)
		m_PageShift.VeHaiCot(m_szTruoc, szNay);
	else
		m_PageShift.VeHaiCot(szNay, szNay);
	m_PageActivate.VeDongAn(szNay, nCo);
}
