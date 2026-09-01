#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiMantleWash.h"
#include "UiAffairItem.h"	// [BOXMAU 01/09] UiAffair_EncodeDesc
#include "UiItem.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern iCoreShell*		g_pCoreShell;

// Ten tep bo cuc (ASCII, tu tao - da chep box tay luyen VLTK vao Ui\Ui3\mantlewash.ini)
#define	WASH_SCHEME_INI	"mantlewash.ini"

KUiMantleWash* KUiMantleWash::m_pSelf = NULL;

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

		if (pszTitle && pszTitle[0])
			m_pSelf->m_Title.SetText(pszTitle);
		m_pSelf->m_Desc.Clear();
		if (pszInitString && pszInitString[0])
		{
			// [BOXMAU 01/09] ma hoa the <color> truoc khi do vao khung mo ta
			char szDesc[600];
			int nDescLen = UiAffair_EncodeDesc(pszInitString, szDesc, sizeof(szDesc));
			if (nDescLen > 0)
				m_pSelf->m_Desc.AddOneMessage(szDesc, nDescLen);
		}

		m_pSelf->m_szFunc[0] = 0;
		if (pszFunc)
		{
			strncpy(m_pSelf->m_szFunc, pszFunc, sizeof(m_pSelf->m_szFunc) - 1);
			m_pSelf->m_szFunc[sizeof(m_pSelf->m_szFunc) - 1] = 0;
		}
		m_pSelf->m_bDaTay = false;
		for (int i = 0; i < PF_WASH_LINE; i++)
			m_pSelf->m_szTruoc[i][0] = 0;

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
	// Bo cuc don lop (khac box kham 2 lop): moi thanh phan la con truc tiep cua Main.
	// Thu tu AddChild = thu tu ve: nen/khung truoc, o dat do + chu sau cung.
	AddChild(&m_ImgBg);
	AddChild(&m_ImgArrow);
	AddChild(&m_ImgItemFrame);
	AddChild(&m_Close);
	AddChild(&m_Title);
	AddChild(&m_Desc);
	AddChild(&m_TxtBeforeHdr);
	AddChild(&m_TxtAfterHdr);
	int i;
	for (i = 0; i < PF_WASH_LINE; i++)
		AddChild(&m_TxtBefore[i]);
	for (i = 0; i < PF_WASH_LINE; i++)
		AddChild(&m_TxtAfter[i]);
	AddChild(&m_TxtConsume);
	AddChild(&m_BtnWash);
	AddChild(&m_BtnKeep);
	AddChild(&m_BtnApply);
	AddChild(&m_ItemSlot);		// o dat do ve tren cung

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_ItemSlot.SetContainerId((int)UOC_AFFAIR_ITEM);

	Wnd_AddWindow(this);
}

void KUiMantleWash::LoadScheme(const char* pScheme)
{
	if (!m_pSelf)
		return;
	char		Buff[256];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, WASH_SCHEME_INI);
	if (!Ini.Load(Buff))
		return;

	m_pSelf->Init(&Ini, "Main");
	m_pSelf->m_ImgBg.Init(&Ini, "ImgBg");
	m_pSelf->m_ImgArrow.Init(&Ini, "ImgArrow");
	m_pSelf->m_ImgItemFrame.Init(&Ini, "ImgItemFrame");
	m_pSelf->m_Title.Init(&Ini, "DescTitle");
	m_pSelf->m_Desc.Init(&Ini, "Desc");
	m_pSelf->m_TxtBeforeHdr.Init(&Ini, "TxtAttribBefore");
	m_pSelf->m_TxtAfterHdr.Init(&Ini, "TxtAttribAfter");
	m_pSelf->m_TxtConsume.Init(&Ini, "TxtConsume");
	m_pSelf->m_ItemSlot.Init(&Ini, "ObjBoxItem");
	m_pSelf->m_ItemSlot.EnablePickPut(true);
	m_pSelf->m_BtnWash.Init(&Ini, "BtnKeepOn");
	m_pSelf->m_BtnKeep.Init(&Ini, "BtnRemain");
	m_pSelf->m_BtnApply.Init(&Ini, "BtnApply");
	m_pSelf->m_Close.Init(&Ini, "CloseBtn");

	// 2 dong an moi cot: dat theo o goc cua template + buoc StepY (giong VLTK).
	int nBx = 0, nBy = 0, nStepB = 22, nBw = 160, nBh = 18;
	int nAx = 0, nAy = 0, nStepA = 22, nAw = 160, nAh = 18;
	Ini.GetInteger("AttribBefore_ItemTemplate", "Left", 9, &nBx);
	Ini.GetInteger("AttribBefore_ItemTemplate", "Top", 39, &nBy);
	Ini.GetInteger("AttribBefore_ItemTemplate", "StepY", 22, &nStepB);
	Ini.GetInteger("AttribBefore_ItemTemplate", "Width", 160, &nBw);
	Ini.GetInteger("AttribAfter_ItemTemplate", "Left", 185, &nAx);
	Ini.GetInteger("AttribAfter_ItemTemplate", "Top", 39, &nAy);
	Ini.GetInteger("AttribAfter_ItemTemplate", "StepY", 22, &nStepA);
	Ini.GetInteger("AttribAfter_ItemTemplate", "Width", 160, &nAw);
	int i;
	for (i = 0; i < PF_WASH_LINE; i++)
	{
		m_pSelf->m_TxtBefore[i].SetPosition(nBx + 4, nBy + i * nStepB);
		m_pSelf->m_TxtBefore[i].SetSize(nBw, nBh);
		m_pSelf->m_TxtBefore[i].SetTextColor((::GetColor("Gray") & 0xFFFFFF));
		m_pSelf->m_TxtBefore[i].SetText("");
		m_pSelf->m_TxtAfter[i].SetPosition(nAx + 4, nAy + i * nStepA);
		m_pSelf->m_TxtAfter[i].SetSize(nAw, nAh);
		m_pSelf->m_TxtAfter[i].SetTextColor((::GetColor("HGreen") & 0xFFFFFF));
		m_pSelf->m_TxtAfter[i].SetText("");
	}

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
	int nRet = 0;
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_BtnWash)
			OnWash();
		else if (uParam == (unsigned int)(KWndWindow*)&m_BtnKeep)
			OnKeepOld();
		else if (uParam == (unsigned int)(KWndWindow*)&m_BtnApply)
			OnApplyNew();
		else if (uParam == (unsigned int)(KWndWindow*)&m_Close)
			CloseWindow(true);
		break;
	case WM_KEYDOWN:
		if (uParam == VK_ESCAPE)
		{
			CloseWindow(true);
			nRet = 1;
		}
		break;
	case WND_N_ITEM_PICKDROP:
		OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		break;
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

// Bam "Tay luyen": chup dong an HIEN TAI lam cot TRUOC, roi goi ham Lua roll.
void KUiMantleWash::OnWash()
{
	// chup 2 dong hien tai (truoc khi roll) -> cot trai
	char szDong[PF_WASH_LINE][128];
	int nCo = LayDongAn(szDong);
	int i;
	for (i = 0; i < PF_WASH_LINE; i++)
	{
		if (i < nCo)
			strncpy(m_szTruoc[i], szDong[i], sizeof(m_szTruoc[i]) - 1), m_szTruoc[i][sizeof(m_szTruoc[i]) - 1] = 0;
		else
			m_szTruoc[i][0] = 0;
	}
	m_bDaTay = true;
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 1, (unsigned int)m_szFunc);
}

// "Giu nguyen": khoi phuc dong cu (Lua doWashKeep)
void KUiMantleWash::OnKeepOld()
{
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_ADD_UI_CMD_SCRIPT, 1, (unsigned int)"doWashKeep");
}

// "Ap dung": giu dong moi (da ghi) -> chi dong box
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
	m_ItemSlot.Celar();		// ten ham goc viet sai chinh ta, giu nguyen
	if (!g_pCoreShell)
		return;
	int nCount = g_pCoreShell->GetGameData(GDI_AFFAIR_ITEM, 0, 0);
	if (nCount <= 0)
	{
		VeHaiCot();
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
		m_ItemSlot.HoldObject(pItem->Obj.uGenre, pItem->Obj.uId, pItem->Region.Width, pItem->Region.Height);
	else
		m_ItemSlot.Celar();
	VeHaiCot();
}

// Doc 2 dong an (khe 6-7) cua phi phong dang trong o -> text. Tra so dong.
int KUiMantleWash::LayDongAn(char szDong[PF_WASH_LINE][128])
{
	for (int i = 0; i < PF_WASH_LINE; i++)
		szDong[i][0] = 0;
	if (!g_pCoreShell)
		return 0;
	KUiDraggedObject Obj;
	m_ItemSlot.GetObject(Obj);
	if (Obj.uId == 0)
		return 0;
	char szBuf[512];
	szBuf[0] = 0;
	int nCo = g_pCoreShell->GetGameData(GDI_MANTLE_HIDDEN_DESC, (unsigned int)Obj.uId, (int)szBuf);
	if (nCo <= 0)
		return 0;
	// tach theo '\n'
	int nLine = 0;
	char* p = szBuf;
	while (p && *p && nLine < PF_WASH_LINE)
	{
		char* q = strchr(p, '\n');
		int nLen;
		if (q)
			nLen = (int)(q - p);
		else
			nLen = (int)strlen(p);
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

void KUiMantleWash::VeHaiCot()
{
	char szNay[PF_WASH_LINE][128];
	int nCo = LayDongAn(szNay);
	int i;
	// cot PHAI = dong HIEN TAI cua item
	for (i = 0; i < PF_WASH_LINE; i++)
		m_TxtAfter[i].SetText(i < nCo ? szNay[i] : "");
	// cot TRAI: neu da tay it nhat 1 lan thi hien snapshot truoc; chua tay thi = hien tai
	if (m_bDaTay)
	{
		for (i = 0; i < PF_WASH_LINE; i++)
			m_TxtBefore[i].SetText(m_szTruoc[i]);
	}
	else
	{
		for (i = 0; i < PF_WASH_LINE; i++)
			m_TxtBefore[i].SetText(i < nCo ? szNay[i] : "");
	}
}
