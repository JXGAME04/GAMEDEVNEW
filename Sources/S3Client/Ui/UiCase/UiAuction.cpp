// UiAuction.cpp - [DAUGIA 04/09 A3] cua so DAU GIA phia client. Xem UiAuction.h + KAuctionUiDef.h.
// Khung ma chep tu UiMail.cpp (da chay on): cua so tinh, cac hang la widget co san, cuon bang KWndScrollBar,
// vat pham hien bang KWndObjectBox giu vat pham TAM (GDI_ITEM_CHAT) nen bieu tuong + chu giai la that.
#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "GameDataDef.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "UiAffairItem.h"	// [A6] hop dua vat pham co o gia + nut doi tien
#include "UiAuction.h"
extern iCoreShell* g_pCoreShell;	// nhu UiPartnerCommon.h
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define AUC_MANAGER_INI		"auction\\auction_manager.ini"
#define AUC_ICON_INI		"auction\\auction_icon.ini"
#define AUC_PAGE_TONG_INI	"auction\\auction_page_tong.ini"
#define AUC_PAGE_WORLD_INI	"auction\\auction_page_world.ini"
#define AUC_PAGE_PERS_INI	"auction\\auction_page_personal.ini"
#define AUC_ACT_HEADER_INI	"auction\\auction_activity_header.ini"
#define AUC_ENG_HEADER_INI	"auction\\auction_item_english_header.ini"
#define AUC_DUT_HEADER_INI	"auction\\auction_item_dutch_header.ini"
#define AUC_MINE_HEADER_INI	"auction\\auction_item_personal_myself_header.ini"
#define AUC_ICON_ITEM_INI	"auction\\auction_item_icon.ini"
#define AUC_MEMBER_INI		"auction\\auction_memberlist_info.ini"
#define AUC_MEMBER_ROW_INI	"auction\\auction_member_header.ini"

#define AUCUI_ICON_FRAME_NORMAL	0
#define AUCUI_ICON_FRAME_HOT	2
#define AUCUI_BLINK_FRAMES		9
#define AUCUI_BLINK_TOTAL		(18 * 6)

extern int SCREEN_WIDTH;
extern KUiBase g_UiBase;

static int  s_bAucGameStarted = 0;
static int  s_bAucPendingOpen = 0;
static int  s_nAucPendingIcon = -1;
static int  s_nAucPendingSwitch = 0;

static void sAucLog(const char* szFmt, ...)
{
	if (!szFmt)
		return;
	FILE* f = fopen("jx_mail.log", "a");
	if (!f)
		return;
	time_t t = time(NULL);
	struct tm* p = localtime(&t);
	if (p)
		fprintf(f, "%02d:%02d:%02d [AUC] ", p->tm_hour, p->tm_min, p->tm_sec);
	va_list va;
	va_start(va, szFmt);
	vfprintf(f, szFmt, va);
	va_end(va);
	fprintf(f, "\n");
	fclose(f);
}

static void sSendOp(int nOp, int nParam)
{
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_AUCTION_UI, (unsigned int)nOp, nParam);
}

static void sSendReq(int nOp, int nType, int nId, int nPrice, const char* szName)
{
	KAucUiReq r;
	memset(&r, 0, sizeof(r));
	r.nType = nType;
	r.nId = nId;
	r.nPrice = nPrice;
	if (szName)
	{
		strncpy(r.szName, szName, sizeof(r.szName) - 1);
		r.szName[sizeof(r.szName) - 1] = 0;
	}
	sSendOp(nOp, (int)&r);
}

// [DAUGIA 04/09 A23] chu: "thong tin gia nen rut gon lai thay vi 1000000 thi thanh 100 van".
// 1 van = 10.000 ; 1 uc = 10.000 van = 100.000.000. Duoi mot van thi viet thang so.
// Phan le chi hien khi khac 0 nen gia tron (thuong gap, vi Ngan luong nhap theo van) rat gon.
static void sFmtMoney(char* sz, int nSize, int n)
{
	if (n < 0)
		n = 0;
	// [A32 04/09] chu chot: KHONG dung "øc", cu dem theo v¹n (100 v¹n -> 1000 v¹n -> 10000 v¹n)
	if (n >= 10000)
	{
		int nVan = n / 10000;
		int nLe = n % 10000;
		if (nLe)
			_snprintf(sz, nSize - 1, "%d v¹n %d", nVan, nLe);
		else
			_snprintf(sz, nSize - 1, "%d v¹n", nVan);
	}
	else
		_snprintf(sz, nSize - 1, "%d", n);
	sz[nSize - 1] = 0;
}

// [DAUGIA 04/09 A27] chu: "xu chua co don vi van hien 100". Moi SO TIEN deu phai mang don vi;
// rieng SO LUONG vat pham thi khong (dung sFmtMoney o tren).
//   Xu          : "100 xu" / "12 v¹n xu"
//   Ngan luong  : duoi mot v¹n thi "5000 l­îng", tu mot v¹n tro len thi "100 v¹n"
//                 (chu v¹n da ngu y l­îng, va hang da ghi ro loai tien)
static void sFmtCur(char* sz, int nSize, int n, int nCur)
{
	char szSo[48];
	sFmtMoney(szSo, sizeof(szSo), n);
	if (nCur == AUCUI_CUR_XU)
		_snprintf(sz, nSize - 1, "%s xu", szSo);
	else if (n < 10000)
		_snprintf(sz, nSize - 1, "%s l­îng", szSo);
	else
		_snprintf(sz, nSize - 1, "%s", szSo);
	sz[nSize - 1] = 0;
}

// giay -> "hh:mm:ss" (0 hoac am -> "--:--")
static void sFmtLeft(char* sz, int nSize, int nEndAbs)
{
	int nLeft = nEndAbs - (int)time(NULL);
	if (nEndAbs <= 0 || nLeft <= 0)
	{
		strncpy(sz, "--:--", nSize - 1);
		sz[nSize - 1] = 0;
		return;
	}
	if (nLeft >= 86400)
		_snprintf(sz, nSize - 1, "%dd %02d:%02d", nLeft / 86400, (nLeft % 86400) / 3600, (nLeft % 3600) / 60);
	else
		_snprintf(sz, nSize - 1, "%02d:%02d:%02d", nLeft / 3600, (nLeft % 3600) / 60, nLeft % 60);
	sz[nSize - 1] = 0;
}

//////////////////////////////////////////////////////////////////////
// KUiAuctionScrollWnd
//////////////////////////////////////////////////////////////////////
int KUiAuctionScrollWnd::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if ((uMsg == WND_N_BUTTON_CLICK || uMsg == WND_N_SCORLLBAR_POS_CHANGED) && m_pParentWnd)
		return m_pParentWnd->WndProc(uMsg, uParam, nParam);
	return KWndImage::WndProc(uMsg, uParam, nParam);
}

//////////////////////////////////////////////////////////////////////
// KUiAuctionActRow
//////////////////////////////////////////////////////////////////////
KUiAuctionActRow::KUiAuctionActRow()
{
	m_szName[0] = 0;
}

void KUiAuctionActRow::Build()
{
	AddChild(&m_Selection);
	AddChild(&m_Name);
	AddChild(&m_Click);
}

void KUiAuctionActRow::LoadScheme(KIniFile* pIni)
{
	Init(pIni, "Main");
	m_Selection.Init(pIni, "SelectionImg");
	m_Name.Init(pIni, "ActivityNameLable");
	m_Click.Init(pIni, "ForClickImg");
	ClearRow();
}

void KUiAuctionActRow::Fill(const KAucUiActivity* p, int bSelected)
{
	if (!p)
	{
		ClearRow();
		return;
	}
	strncpy(m_szName, p->szName, sizeof(m_szName) - 1);
	m_szName[sizeof(m_szName) - 1] = 0;
	m_Name.SetText(m_szName);
	m_Name.Show();
	m_Click.Show();
	if (bSelected)
		m_Selection.Show();
	else
		m_Selection.Hide();
	Show();
}

void KUiAuctionActRow::ClearRow()
{
	m_szName[0] = 0;
	m_Name.SetText("");
	m_Selection.Hide();
	m_Click.Hide();
	Hide();
}

//////////////////////////////////////////////////////////////////////
// KUiAuctionItemRow
//////////////////////////////////////////////////////////////////////
KUiAuctionItemRow::KUiAuctionItemRow()
{
	memset(&m_Data, 0, sizeof(m_Data));
	m_nOffer = 0;
	m_nTempItemIdx = 0;
}

void KUiAuctionItemRow::Build()
{
	AddChild(&m_Name);
	AddChild(&m_IconBg);
	m_IconBox.SetObjectGenre(CGOG_PLAYERSELLITEM);
	AddChild(&m_IconBox);
	m_IconBox.SetContainerId(0);
	m_IconBox.EnablePickPut(false);
	AddChild(&m_IconCount);
	AddChild(&m_EngT1); AddChild(&m_EngV1);
	AddChild(&m_EngT2); AddChild(&m_EngV2);
	AddChild(&m_EngT3); AddChild(&m_EngV3);
	AddChild(&m_EngT4); AddChild(&m_EngV4);
	AddChild(&m_EngT5); AddChild(&m_EngV5);
	AddChild(&m_EngT6); AddChild(&m_EngV6);
	AddChild(&m_ImgInput);
	AddChild(&m_TxtOffer);
	AddChild(&m_BtnAddPrice);
	AddChild(&m_BtnOffer);
	AddChild(&m_BtnGiveUp);
	AddChild(&m_ImgSuccess);
	AddChild(&m_ImgFail);
	AddChild(&m_DutT1); AddChild(&m_DutV1);
	AddChild(&m_DutT2); AddChild(&m_DutV2);
	AddChild(&m_DutT3); AddChild(&m_DutV3);
	AddChild(&m_DutT4); AddChild(&m_DutV4);
	AddChild(&m_DutT5); AddChild(&m_DutV5);
	AddChild(&m_DutT6); AddChild(&m_DutV6);
	AddChild(&m_BtnBid);
	AddChild(&m_BtnGetBack);
}

void KUiAuctionItemRow::LoadScheme(KIniFile* pEng, KIniFile* pDutch, KIniFile* pMine, KIniFile* pIcon)
{
	// khung + ten lay tu ini kieu Ha Lan (hai ini cung khung 429x118)
	Init(pDutch, "Main");
	m_Name.Init(pDutch, "NameOfItem");
	// bieu tuong vat pham (auction_item_icon.ini: [Main] Left=30 Top=30 la goc cua o)
	int nBaseX = 30, nBaseY = 30, l = 0, t = 0;
	if (pIcon)
	{
		pIcon->GetInteger("Main", "Left", 30, &nBaseX);
		pIcon->GetInteger("Main", "Top", 30, &nBaseY);
		m_IconBg.Init(pIcon, "MailAwardBgSpr");
		m_IconBg.GetPosition(&l, &t);
		m_IconBg.SetPosition(nBaseX + l, nBaseY + t);
		m_IconBox.Init(pIcon, "MailAwardItemSpr");
		m_IconBox.GetPosition(&l, &t);
		m_IconBox.SetPosition(nBaseX + l, nBaseY + t);
		m_IconCount.Init(pIcon, "MailAwardItemCount");
		m_IconCount.GetPosition(&l, &t);
		m_IconCount.SetPosition(nBaseX + l, nBaseY + t);
	}
	// kieu Anh
	m_EngT1.Init(pEng, "GuaranteedPriceTitle");	m_EngV1.Init(pEng, "GuaranteedPrice");
	m_EngT2.Init(pEng, "RangePerOfferTitle");	m_EngV2.Init(pEng, "RangePerOffer");
	m_EngT3.Init(pEng, "CurrencyTypeTitle");	m_EngV3.Init(pEng, "CurrencyType");
	m_EngT4.Init(pEng, "MaxPriceTitle");		m_EngV4.Init(pEng, "MaxPrice");
	m_EngT5.Init(pEng, "SelfPriceTitle");		m_EngV5.Init(pEng, "SelfPrice");
	m_EngT6.Init(pEng, "RemainingTimeTitle");	m_EngV6.Init(pEng, "RemainingTime");
	m_ImgInput.Init(pEng, "imgInPutPrice");
	m_TxtOffer.Init(pEng, "txtJinPai");
	m_BtnAddPrice.Init(pEng, "btnAddPrice");
	m_BtnOffer.Init(pEng, "btnOfferPrice");
	m_BtnGiveUp.Init(pEng, "btnGiveUp");
	m_ImgSuccess.Init(pEng, "imgFinishSuccess");
	m_ImgFail.Init(pEng, "imgFinishFail");
	// kieu Ha Lan
	m_DutT1.Init(pDutch, "CurrentPriceTitle");		m_DutV1.Init(pDutch, "CurrentPrice");
	m_DutT2.Init(pDutch, "BasePriceTitle");			m_DutV2.Init(pDutch, "BasePrice");
	m_DutT3.Init(pDutch, "CurrencyTypeTitle");		m_DutV3.Init(pDutch, "CurrencyType");
	m_DutT4.Init(pDutch, "FreshRemainingTimeTitle");	m_DutV4.Init(pDutch, "FreshRemainingTime");
	m_DutT5.Init(pDutch, "NextPriceTitle");			m_DutV5.Init(pDutch, "NextPrice");
	m_DutT6.Init(pDutch, "TotalRemainingTimeTitle");	m_DutV6.Init(pDutch, "TotalRemainingTime");
	m_BtnBid.Init(pDutch, "btnBid");
	// nut lay lai (ini "cua minh"); neu ini khong co thi dat trung cho nut mua
	if (pMine)
		m_BtnGetBack.Init(pMine, "btnGetBackItem");
	int w = 0, h = 0;
	m_BtnGetBack.GetSize(&w, &h);
	if (w <= 0)
	{
		m_BtnGetBack.Init(pDutch, "btnBid");
	}
	ClearRow();
}

void KUiAuctionItemRow::ShowEnglish(int b)
{
	KWndWindow* a[] = { &m_EngT1, &m_EngV1, &m_EngT2, &m_EngV2, &m_EngT3, &m_EngV3, &m_EngT4, &m_EngV4,
		&m_EngT5, &m_EngV5, &m_EngT6, &m_EngV6, &m_ImgInput, &m_TxtOffer, &m_BtnAddPrice, &m_BtnOffer };
	for (int i = 0; i < (int)(sizeof(a) / sizeof(a[0])); i++)
	{
		if (b) a[i]->Show(); else a[i]->Hide();
	}
	// [DAUGIA 04/09 A18] Nut "Huy dau gia" la NUT CHET nen an han.
	// Ban goc bat nguoi choi tu bam nut do de doi lai tien da tra; ban ta hoan TU DONG qua thu
	// ngay khi bi vuot gia (tot hon goc), nen duong hoan thu cong luon tra that bai va nguoi vua
	// duoc hoan tien bam vao se doc "Dai hiep khong co muc gia nao dang giu o mon nay".
	m_BtnGiveUp.Hide();
	m_ImgSuccess.Hide();
	m_ImgFail.Hide();
}

void KUiAuctionItemRow::ShowDutch(int b)
{
	KWndWindow* a[] = { &m_DutT1, &m_DutV1, &m_DutT2, &m_DutV2, &m_DutT3, &m_DutV3, &m_DutT4, &m_DutV4,
		&m_DutT5, &m_DutV5, &m_DutT6, &m_DutV6, &m_BtnBid };
	for (int i = 0; i < (int)(sizeof(a) / sizeof(a[0])); i++)
	{
		if (b) a[i]->Show(); else a[i]->Hide();
	}
}

void KUiAuctionItemRow::ReleaseItem()
{
	if (m_nTempItemIdx > 0 && g_pCoreShell)
		g_pCoreShell->GetGameData(GDI_ITEM_CHAT, false, m_nTempItemIdx);
	m_nTempItemIdx = 0;
	m_IconBox.HoldObject(CGOG_NOTHING, 0, 0, 0);
	m_IconBox.Hide();
}

void KUiAuctionItemRow::ClearRow()
{
	ReleaseItem();
	memset(&m_Data, 0, sizeof(m_Data));
	m_nOffer = 0;
	ShowEnglish(0);
	ShowDutch(0);
	m_BtnGetBack.Hide();
	m_IconBg.Hide();
	m_IconCount.Hide();
	m_Name.SetText("");
	Hide();
}

void KUiAuctionItemRow::Fill(const KAucUiItem* p)
{
	if (!p || p->nId <= 0)
	{
		ClearRow();
		return;
	}
	int bSameItem = (m_Data.nId == p->nId && m_nTempItemIdx > 0);
	if (!bSameItem)
	{
		ReleaseItem();
		// [DAUGIA 04/09 A18] HANG NAY DA DOI SANG MON KHAC -> phai quen so tien de nghi cu.
		// Truoc day m_nOffer chi duoc NANG len ("if (m_nOffer < nBase)") chu khong bao gio ha,
		// nen mot mon 100 van vua troi di, mon 1 van thay cho, bam la gui thang 100 van.
		// May chu khong cuu duoc: tra THUA van la mot lenh tra gia hop le.
		m_nOffer = 0;
	}
	m_Data = *p;
	char sz[64];
	m_Name.SetText(p->szName);
	// bieu tuong: dung lai vat pham tam tu ChatItem (nhu hop thu)
	// [A35 04/09] o lai rong 58x78 (chu: "o vuong chi co 1x1 chua tra lai nhu truoc") de vien
	// dong khung om dung lay mon ve nguyen co; khung nen chi 26x26 nen nho hon mon - an di.
	m_IconBg.Hide();
	if (!bSameItem && g_pCoreShell && p->Item.m_nID)
	{
		int nIdx = g_pCoreShell->GetGameData(GDI_ITEM_CHAT, true, (int)&p->Item);
		if (nIdx > 0)
		{
			m_nTempItemIdx = nIdx;
			m_IconBox.HoldObject(CGOG_PLAYERSELLITEM, (unsigned int)nIdx, 1, 1);
			m_IconBox.Show();
		}
	}
	else if (bSameItem)
		m_IconBox.Show();
	// [DAUGIA 04/09 D13] giong hop thu: o vat pham that da duoc engine ve san so chong,
	// chi ve nhan so khi KHONG dung duoc o vat pham (khong thi hien hai so chong nhau).
	// [A8 04/09] o vat pham chi ve duoc so <= 255; tren nguong do KAuctionClient dat stack = 1
	if (p->nCount > 1 && (m_nTempItemIdx <= 0 || p->nCount > 255))
	{
		sFmtMoney(sz, sizeof(sz), p->nCount);
		m_IconCount.SetText(sz);
		m_IconCount.Show();
	}
	else
		m_IconCount.Hide();

	if (p->nKind == AUCUI_KIND_ENGLISH)
	{
		ShowDutch(0);
		ShowEnglish(1);
		sFmtCur(sz, sizeof(sz), p->nGuaranteed, p->nCurrency);	m_EngV1.SetText(sz);
		sFmtCur(sz, sizeof(sz), p->nRange, p->nCurrency);		m_EngV2.SetText(sz);
		m_EngV3.SetText(p->szCurrency);
		sFmtCur(sz, sizeof(sz), p->nMax, p->nCurrency);			m_EngV4.SetText(sz);
		// [DAUGIA 04/09 A20e] dong ky gui ca nhan co them GIA MUA NGAY: o thu nam doi nghia.
		// Phien the gioi / bang hoi khong co gia mua ngay (nBuyNow = 0) nen giu nguyen "Gia cua ta".
		if (p->nBuyNow > 0)
		{
			m_EngT5.SetText("Mua ngay");
			sFmtCur(sz, sizeof(sz), p->nBuyNow, p->nCurrency);	m_EngV5.SetText(sz);
		}
		else
		{
			m_EngT5.SetText("Gi¸ cña ta");
			sFmtCur(sz, sizeof(sz), p->nSelf, p->nCurrency);	m_EngV5.SetText(sz);
		}
		// gia minh se tra: cao nhat + buoc (toi thieu = khoi diem)
		int nBase = p->nMax > 0 ? p->nMax + p->nRange : p->nGuaranteed;
		if (m_nOffer < nBase)
			m_nOffer = nBase;
		sFmtCur(sz, sizeof(sz), m_nOffer, p->nCurrency);		m_TxtOffer.SetText(sz);
		// [A20e] mon cua CHINH MINH o the Ca nhan: van phai co nut "Lay lai" nhu dong Ha Lan,
		// khong thi nguoi ban ky gui xong khong con duong nao rut mon ve.
		if (p->bMine)
		{
			m_BtnOffer.Hide();
			m_BtnAddPrice.Hide();
			m_BtnGiveUp.Hide();
			// [A26 04/09] mon cua CHINH MINH thi khong tra gia duoc, nen an ca KHUNG NHAP gia
			// va con so trong do - truoc chi an ba cai nut, khung nhap van nam do trong tron.
			m_ImgInput.Hide();
			m_TxtOffer.Hide();
			if (p->nBuyNow > 0)
				m_BtnGetBack.Show();
			else
				m_BtnGetBack.Hide();
		}
		else
		{
			m_BtnGetBack.Hide();
			// nut phai = MUA NGAY (chi dong ky gui moi co gia mua ngay)
			if (p->nBuyNow > 0)
			{
				m_BtnGiveUp.SetLabel("Mua ngay");
				m_BtnGiveUp.Show();
			}
		}
	}
	else
	{
		ShowEnglish(0);
		ShowDutch(1);
		sFmtCur(sz, sizeof(sz), p->nCur, p->nCurrency);			m_DutV1.SetText(sz);
		sFmtCur(sz, sizeof(sz), p->nGuaranteed, p->nCurrency);	m_DutV2.SetText(sz);
		m_DutV3.SetText(p->szCurrency);
		sFmtCur(sz, sizeof(sz), p->nNext, p->nCurrency);		m_DutV5.SetText(sz);
		if (p->bMine)
		{
			m_BtnBid.Hide();
			m_BtnGetBack.Show();
		}
		else
			m_BtnGetBack.Hide();
	}
	RefreshTime();
	// [A18] mon da ket thuc thi giu nguyen cho nhung an het nut - nguoi choi con doc duoc
	// ten va gia, chi khong bam nham duoc nua (dung tinh than ban goc: danh dau tai cho).
	if (p->bEnded)
	{
		m_BtnBid.Hide();
		m_BtnGetBack.Hide();
		m_BtnOffer.Hide();
		m_BtnAddPrice.Hide();
		m_BtnGiveUp.Hide();
	}
	Show();
}

void KUiAuctionItemRow::RefreshTime()
{
	if (m_Data.nId <= 0)
		return;
	char sz[32];
	if (m_Data.nKind == AUCUI_KIND_ENGLISH)
	{
		sFmtLeft(sz, sizeof(sz), m_Data.nRemaining);
		m_EngV6.SetText(sz);
	}
	else
	{
		sFmtLeft(sz, sizeof(sz), m_Data.nRemaining);
		m_DutV4.SetText(sz);
		sFmtLeft(sz, sizeof(sz), m_Data.nTotalRemaining);
		m_DutV6.SetText(sz);
	}
}

//////////////////////////////////////////////////////////////////////
// KUiAuctionMemberRow / KUiAuctionMemberWnd
//////////////////////////////////////////////////////////////////////
KUiAuctionMemberRow::KUiAuctionMemberRow()
{
}

void KUiAuctionMemberRow::Build()
{
	AddChild(&m_Name);
	AddChild(&m_Position);
	AddChild(&m_Level);
}

void KUiAuctionMemberRow::LoadScheme(KIniFile* pIni)
{
	Init(pIni, "Main");
	m_Name.Init(pIni, "RoleName1");
	m_Position.Init(pIni, "Position1");
	m_Level.Init(pIni, "Level1");
	ClearRow();
}

void KUiAuctionMemberRow::Fill(const KAucUiMember* p)
{
	if (!p)
	{
		ClearRow();
		return;
	}
	char sz[32];
	m_Name.SetText(p->szName);
	sFmtMoney(sz, sizeof(sz), p->nFigure);
	m_Position.SetText(sz);
	sFmtMoney(sz, sizeof(sz), p->nLevel);
	m_Level.SetText(sz);
	Show();
}

void KUiAuctionMemberRow::ClearRow()
{
	m_Name.SetText("");
	m_Position.SetText("");
	m_Level.SetText("");
	Hide();
}

KUiAuctionMemberWnd::KUiAuctionMemberWnd()
{
	m_nCount = 0;
	m_nTop = 0;
	m_nRowH = 20;
}

void KUiAuctionMemberWnd::Build()
{
	AddChild(&m_Title);
	AddChild(&m_Col1);
	AddChild(&m_Col2);
	AddChild(&m_Col3);
	AddChild(&m_TotalTitle);
	AddChild(&m_TotalValue);
	AddChild(&m_SalTitle);
	AddChild(&m_SalValue);
	AddChild(&m_ScrollWnd);
	for (int i = 0; i < AUCUI_MEMBER_ROW_COUNT; i++)
	{
		m_Row[i].Build();
		m_ScrollWnd.AddChild(&m_Row[i]);
	}
	m_ScrollWnd.AddChild(&m_Scroll);
	AddChild(&m_Close);
}

void KUiAuctionMemberWnd::LoadScheme(const char* pScheme)
{
	char Buff[256];
	KIniFile Ini, IniRow;
	sprintf(Buff, "%s\\%s", pScheme, AUC_MEMBER_INI);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		m_Title.Init(&Ini, "Title");
		m_Col1.Init(&Ini, "ColumnName1");
		m_Col2.Init(&Ini, "ColumnName2");
		m_Col3.Init(&Ini, "ColumnName3");
		m_TotalTitle.Init(&Ini, "TotalMemberTitle");
		m_TotalValue.Init(&Ini, "TotalMemberValue");
		m_SalTitle.Init(&Ini, "AverageSalaryTitle");
		m_SalValue.Init(&Ini, "AverageSalaryValue");
		m_ScrollWnd.Init(&Ini, "MemberListScroll");
		m_Scroll.Init(&Ini, "MemberListScroll_ScrollBar");
		m_Close.Init(&Ini, "Close");
	}
	sprintf(Buff, "%s\\%s", pScheme, AUC_MEMBER_ROW_INI);
	if (IniRow.Load(Buff))
	{
		int l = 0, t = 0, w = 0, h = 0;
		for (int i = 0; i < AUCUI_MEMBER_ROW_COUNT; i++)
		{
			m_Row[i].LoadScheme(&IniRow);
			m_Row[i].GetPosition(&l, &t);
			m_Row[i].GetSize(&w, &h);
			if (h > 0)
				m_nRowH = h;
			m_Row[i].SetPosition(l, t + i * m_nRowH);
		}
	}
	Hide();
}

int KUiAuctionMemberWnd::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Close)
		{
			Hide();
			return 1;
		}
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_Scroll)
		{
			m_nTop = nParam;
			Refresh();
			return 1;
		}
		break;
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}

void KUiAuctionMemberWnd::AddMember(const KAucUiMember* p)
{
	if (!p || m_nCount >= AUCUI_MAX_MEMBER)
		return;
	m_Member[m_nCount++] = *p;
	Refresh();
}

void KUiAuctionMemberWnd::Clear()
{
	m_nCount = 0;
	m_nTop = 0;
	Refresh();
}

void KUiAuctionMemberWnd::SetSalary(const KAucUiSalary* p)
{
	if (!p)
		return;
	char sz[32];
	sFmtMoney(sz, sizeof(sz), p->nCount);
	m_TotalValue.SetText(sz);
	sFmtMoney(sz, sizeof(sz), p->nSalary);
	m_SalValue.SetText(sz);
}

void KUiAuctionMemberWnd::Refresh()
{
	int nMax = m_nCount - AUCUI_MEMBER_ROW_COUNT;
	if (nMax < 0) nMax = 0;
	if (m_nTop > nMax) m_nTop = nMax;
	if (m_nTop < 0) m_nTop = 0;
	m_Scroll.SetValueRange(0, nMax);
	if (m_Scroll.GetScrollPos() != m_nTop)
		m_Scroll.SetScrollPos(m_nTop);
	for (int i = 0; i < AUCUI_MEMBER_ROW_COUNT; i++)
	{
		int nIdx = m_nTop + i;
		if (nIdx >= 0 && nIdx < m_nCount)
			m_Row[i].Fill(&m_Member[nIdx]);
		else
			m_Row[i].ClearRow();
	}
}

//////////////////////////////////////////////////////////////////////
// KUiAuctionPage
//////////////////////////////////////////////////////////////////////
KUiAuctionPage::KUiAuctionPage()
{
	m_nType = AUCUI_TYPE_PERSONAL;
	m_nActCount = 0;
	m_nActTop = 0;
	m_szSelAct[0] = 0;
	m_nItemCount = 0;
	m_nItemTop = 0;
	m_nActRowH = 25;
	m_nItemRowH = 118;
	m_nBreath = 0;
}

void KUiAuctionPage::Build()
{
	AddChild(&m_Title);
	AddChild(&m_GoldTitle);
	AddChild(&m_GoldValue);
	AddChild(&m_SilverTitle);
	AddChild(&m_SilverValue);
	AddChild(&m_BtnTip);
	AddChild(&m_PageText);
	AddChild(&m_BtnPrev);
	AddChild(&m_BtnNext);
	AddChild(&m_ActScrollWnd);
	int i;
	for (i = 0; i < AUCUI_ACT_ROW_COUNT; i++)
	{
		m_ActRow[i].Build();
		m_ActScrollWnd.AddChild(&m_ActRow[i]);
	}
	m_ActScrollWnd.AddChild(&m_ActScroll);
	AddChild(&m_ItemScrollWnd);
	for (i = 0; i < AUCUI_ROW_COUNT; i++)
	{
		m_ItemRow[i].Build();
		m_ItemScrollWnd.AddChild(&m_ItemRow[i]);
	}
	m_ItemScrollWnd.AddChild(&m_ItemScroll);
}

void KUiAuctionPage::LoadScheme(const char* pScheme, int nType)
{
	char Buff[256];
	KIniFile Ini, IniAct, IniEng, IniDut, IniMine, IniIcon;
	m_nType = nType;
	const char* szPage = (nType == AUCUI_TYPE_TONG) ? AUC_PAGE_TONG_INI :
		(nType == AUCUI_TYPE_WORLD) ? AUC_PAGE_WORLD_INI : AUC_PAGE_PERS_INI;
	sprintf(Buff, "%s\\%s", pScheme, szPage);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		m_Title.Init(&Ini, "Title");
		m_GoldTitle.Init(&Ini, "BindingGoldTitle");
		m_GoldValue.Init(&Ini, "BindingGoldValue");
		m_SilverTitle.Init(&Ini, "BindingSilverTitle");
		m_SilverValue.Init(&Ini, "BindingSilverValue");
		m_BtnTip.Init(&Ini, (nType == AUCUI_TYPE_TONG) ? "seeMemberList" :
			(nType == AUCUI_TYPE_WORLD) ? "AuctionWorldTip" : "AuctionPersonalTip");
		m_PageText.Init(&Ini, "CurrentPage");
		m_BtnPrev.Init(&Ini, "btnPrevPage");
		m_BtnNext.Init(&Ini, "btnNextPage");
		m_ActScrollWnd.Init(&Ini, "ActivityListScroll");
		m_ActScroll.Init(&Ini, "ActivityListScroll_ScrollBar");
		m_ItemScrollWnd.Init(&Ini, "ItemListScroll");
		m_ItemScroll.Init(&Ini, "ItemListScroll_ScrollBar");
		// [DAUGIA 04/09 A4] nut goc phai dung tren CA BA trang: bam la mo luong dat ban
		// (ca nhan = ky gui, the gioi = GM mo phien, bang hoi = menu bang chu / xem thanh vien).
		m_BtnTip.Show();
	}
	else
		sAucLog("[UI] KHONG nap duoc %s", Buff);
	int i, l = 0, t = 0, w = 0, h = 0;
	sprintf(Buff, "%s\\%s", pScheme, AUC_ACT_HEADER_INI);
	if (IniAct.Load(Buff))
	{
		for (i = 0; i < AUCUI_ACT_ROW_COUNT; i++)
		{
			m_ActRow[i].LoadScheme(&IniAct);
			m_ActRow[i].GetPosition(&l, &t);
			m_ActRow[i].GetSize(&w, &h);
			if (h > 0)
				m_nActRowH = h;
			m_ActRow[i].SetPosition(l, t + i * m_nActRowH);
		}
	}
	sprintf(Buff, "%s\\%s", pScheme, AUC_ENG_HEADER_INI);
	IniEng.Load(Buff);
	sprintf(Buff, "%s\\%s", pScheme, AUC_DUT_HEADER_INI);
	int bDut = IniDut.Load(Buff);
	sprintf(Buff, "%s\\%s", pScheme, AUC_MINE_HEADER_INI);
	int bMine = IniMine.Load(Buff);
	sprintf(Buff, "%s\\%s", pScheme, AUC_ICON_ITEM_INI);
	int bIcon = IniIcon.Load(Buff);
	if (bDut)
	{
		for (i = 0; i < AUCUI_ROW_COUNT; i++)
		{
			m_ItemRow[i].LoadScheme(&IniEng, &IniDut, bMine ? &IniMine : NULL, bIcon ? &IniIcon : NULL);
			m_ItemRow[i].GetPosition(&l, &t);
			m_ItemRow[i].GetSize(&w, &h);
			if (h > 0)
				m_nItemRowH = h;
			m_ItemRow[i].SetPosition(l, t + i * m_nItemRowH);
		}
	}
	RefreshAct();
	RefreshItem();
}

int KUiAuctionPage::ActRowOf(KWndWindow* pWnd)
{
	for (int i = 0; i < AUCUI_ACT_ROW_COUNT; i++)
		if (pWnd == (KWndWindow*)&m_ActRow[i].m_Click)
			return i;
	return -1;
}

int KUiAuctionPage::ItemRowOf(KWndWindow* pWnd, int* pnBtn)
{
	for (int i = 0; i < AUCUI_ROW_COUNT; i++)
	{
		KUiAuctionItemRow& r = m_ItemRow[i];
		if (pWnd == (KWndWindow*)&r.m_BtnAddPrice) { *pnBtn = 1; return i; }
		if (pWnd == (KWndWindow*)&r.m_BtnOffer)    { *pnBtn = 2; return i; }
		if (pWnd == (KWndWindow*)&r.m_BtnGiveUp)   { *pnBtn = 3; return i; }
		if (pWnd == (KWndWindow*)&r.m_BtnBid)      { *pnBtn = 4; return i; }
		if (pWnd == (KWndWindow*)&r.m_BtnGetBack)  { *pnBtn = 5; return i; }
	}
	return -1;
}

int KUiAuctionPage::FindItem(int nId)
{
	for (int i = 0; i < m_nItemCount; i++)
		if (m_Item[i].nId == nId)
			return i;
	return -1;
}

int KUiAuctionPage::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		{
			int nRow = ActRowOf((KWndWindow*)uParam);
			if (nRow >= 0)
			{
				int nIdx = m_nActTop + nRow;
				if (nIdx >= 0 && nIdx < m_nActCount)
				{
					strncpy(m_szSelAct, m_Act[nIdx].szName, sizeof(m_szSelAct) - 1);
					m_szSelAct[sizeof(m_szSelAct) - 1] = 0;
					RefreshAct();
					sSendReq(AUCUI_OP_SELECT_ACTIVITY, m_nType, 0, 0, m_szSelAct);
				}
				return 1;
			}
			int nBtn = 0;
			nRow = ItemRowOf((KWndWindow*)uParam, &nBtn);
			if (nRow >= 0)
			{
				KUiAuctionItemRow& r = m_ItemRow[nRow];
				if (r.m_Data.nId <= 0)
					return 1;
				char sz[32];
				switch (nBtn)
				{
				case 1:	// + buoc gia
					r.m_nOffer += (r.m_Data.nRange > 0 ? r.m_Data.nRange : 1);
					sFmtCur(sz, sizeof(sz), r.m_nOffer, r.m_Data.nCurrency);
					r.m_TxtOffer.SetText(sz);
					break;
				case 2:
					sSendReq(AUCUI_OP_OFFER_ENGLISH, m_nType, r.m_Data.nId, r.m_nOffer, NULL);
					break;
				case 3:
					// [A20e] nut phai cua dong kieu Anh: co gia mua ngay thi la MUA NGAY,
					// khong thi la nut "Huy dau gia" cu (A18 da an han vi no la nut chet).
					if (r.m_Data.nBuyNow > 0)
						sSendReq(AUCUI_OP_OFFER_DUTCH, m_nType, r.m_Data.nId, r.m_Data.nBuyNow, NULL);
					break;
				case 4:
					sSendReq(AUCUI_OP_OFFER_DUTCH, m_nType, r.m_Data.nId, r.m_Data.nCur, NULL);
					break;
				case 5:
					sSendReq(AUCUI_OP_GET_BACK, m_nType, r.m_Data.nId, 0, NULL);
					break;
				}
				return 1;
			}
			if (uParam == (unsigned int)(KWndWindow*)&m_BtnPrev)
			{
				sSendOp(AUCUI_OP_PREV_PAGE, 0);
				return 1;
			}
			if (uParam == (unsigned int)(KWndWindow*)&m_BtnNext)
			{
				sSendOp(AUCUI_OP_NEXT_PAGE, 0);
				return 1;
			}
			if (uParam == (unsigned int)(KWndWindow*)&m_BtnTip)
			{
				// [DAUGIA 04/09 A4] khong con NPC: nut nay mo thang luong dat ban tren may chu
				sSendOp(AUCUI_OP_PUT_ON, m_nType);
				return 1;
			}
		}
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_ActScroll)
		{
			m_nActTop = nParam;
			RefreshAct();
			return 1;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_ItemScroll)
		{
			m_nItemTop = nParam;
			RefreshItem();
			return 1;
		}
		break;
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}

void KUiAuctionPage::Breathe()
{
	if (!IsVisible())
		return;
	if (++m_nBreath >= 18)
	{
		m_nBreath = 0;
		for (int i = 0; i < AUCUI_ROW_COUNT; i++)
			m_ItemRow[i].RefreshTime();
	}
}

void KUiAuctionPage::AddActivity(const KAucUiActivity* p)
{
	if (!p)
		return;
	for (int i = 0; i < m_nActCount; i++)
	{
		if (strcmp(m_Act[i].szName, p->szName) == 0)
		{
			m_Act[i] = *p;
			RefreshAct();
			return;
		}
	}
	if (m_nActCount >= AUCUI_MAX_ACTIVITY)
		return;
	m_Act[m_nActCount++] = *p;
	RefreshAct();
}

void KUiAuctionPage::DelActivity(const char* szName)
{
	if (!szName)
		return;
	for (int i = 0; i < m_nActCount; i++)
	{
		if (strcmp(m_Act[i].szName, szName) == 0)
		{
			for (int j = i + 1; j < m_nActCount; j++)
				m_Act[j - 1] = m_Act[j];
			m_nActCount--;
			if (strcmp(m_szSelAct, szName) == 0)
				m_szSelAct[0] = 0;
			RefreshAct();
			return;
		}
	}
}

void KUiAuctionPage::ClearActivity()
{
	m_nActCount = 0;
	m_nActTop = 0;
	m_szSelAct[0] = 0;
	RefreshAct();
}

void KUiAuctionPage::SelectActivity(const char* szName)
{
	if (!szName)
		return;
	strncpy(m_szSelAct, szName, sizeof(m_szSelAct) - 1);
	m_szSelAct[sizeof(m_szSelAct) - 1] = 0;
	RefreshAct();
}

void KUiAuctionPage::RefreshAct()
{
	int nMax = m_nActCount - AUCUI_ACT_ROW_COUNT;
	if (nMax < 0) nMax = 0;
	if (m_nActTop > nMax) m_nActTop = nMax;
	if (m_nActTop < 0) m_nActTop = 0;
	m_ActScroll.SetValueRange(0, nMax);
	if (m_ActScroll.GetScrollPos() != m_nActTop)
		m_ActScroll.SetScrollPos(m_nActTop);
	for (int i = 0; i < AUCUI_ACT_ROW_COUNT; i++)
	{
		int nIdx = m_nActTop + i;
		if (nIdx >= 0 && nIdx < m_nActCount)
			m_ActRow[i].Fill(&m_Act[nIdx], strcmp(m_Act[nIdx].szName, m_szSelAct) == 0);
		else
			m_ActRow[i].ClearRow();
	}
}

void KUiAuctionPage::AddItem(const KAucUiItem* p)
{
	if (!p)
		return;
	int nIdx = FindItem(p->nId);
	if (nIdx >= 0)
		m_Item[nIdx] = *p;
	else
	{
		if (m_nItemCount >= AUCUI_MAX_ITEM)
			return;
		m_Item[m_nItemCount++] = *p;
	}
	RefreshItem();
}

// SetItem: chi doi cac so (gia / thoi gian), giu vat pham + ten
void KUiAuctionPage::SetItem(const KAucUiItem* p)
{
	if (!p)
		return;
	int nIdx = FindItem(p->nId);
	if (nIdx < 0)
		return;
	KAucUiItem& d = m_Item[nIdx];
	d.nKind = p->nKind;
	d.nStartTime = p->nStartTime;
	d.nCurrency = p->nCurrency;
	d.bMine = p->bMine;
	d.nGuaranteed = p->nGuaranteed;
	d.nRange = p->nRange;
	d.nMax = p->nMax;
	d.nSelf = p->nSelf;
	d.nRemaining = p->nRemaining;
	d.nCur = p->nCur;
	d.nNext = p->nNext;
	d.nTotalRemaining = p->nTotalRemaining;
	if (p->szCurrency[0])
		strcpy(d.szCurrency, p->szCurrency);
	RefreshItem();
}

// [DAUGIA 04/09 A18] KHONG don danh sach duoi tay nguoi dang bam.
// Ban goc co y khong lam viec nay: hai dong "xoa mon" va "nap lai trang" bi chinh tac gia
// goc chu thich bo (uiauction_house.lua:559 va :568), chi danh dau mon do tai cho.
// Dich mot bac = mon dang nam duoi con tro chuot doi thanh mon khac giua chung -> mua nham.
// Cung voi B2 (so tien de nghi cu) thi thanh bo doi: bam mot cai la mua mon la voi gia mon cu.
void KUiAuctionPage::EndItem(int nId)
{
	int nIdx = FindItem(nId);
	if (nIdx < 0)
		return;
	// [A33 04/09] chu: "item duoc mua roi thi khong cap nhat lien, phai tat bang mo lai moi bien".
	// Danh dau tai cho lam hang chet trong Y HET hang dang ban (ten, gia, bieu tuong van ve).
	// Nay XOA TRANG o do: Fill() mo dau bang "if (!p || p->nId <= 0) { ClearRow(); return; }"
	// nen hang trong ngay, va ClearRow nha o vat pham tam nen khong ro GDI_ITEM_CHAT.
	// m_nItemCount KHONG doi -> hai hang kia khong dich mot ly -> khong the bam nham mon.
	memset(&m_Item[nIdx], 0, sizeof(m_Item[nIdx]));
	RefreshItem();
}

void KUiAuctionPage::ClearItem()
{
	m_nItemCount = 0;
	m_nItemTop = 0;
	RefreshItem();
}

void KUiAuctionPage::SetPageText(const char* sz)
{
	m_PageText.SetText(sz ? sz : "");
}

void KUiAuctionPage::SetMoney(int nMoney, int nXu)
{
	char sz[32];
	sFmtMoney(sz, sizeof(sz), nMoney);
	m_SilverValue.SetText(sz);
	sFmtMoney(sz, sizeof(sz), nXu);
	m_GoldValue.SetText(sz);
}

void KUiAuctionPage::RefreshItem()
{
	int nMax = m_nItemCount - AUCUI_ROW_COUNT;
	if (nMax < 0) nMax = 0;
	if (m_nItemTop > nMax) m_nItemTop = nMax;
	if (m_nItemTop < 0) m_nItemTop = 0;
	m_ItemScroll.SetValueRange(0, nMax);
	if (m_ItemScroll.GetScrollPos() != m_nItemTop)
		m_ItemScroll.SetScrollPos(m_nItemTop);
	for (int i = 0; i < AUCUI_ROW_COUNT; i++)
	{
		int nIdx = m_nItemTop + i;
		if (nIdx >= 0 && nIdx < m_nItemCount)
			m_ItemRow[i].Fill(&m_Item[nIdx]);
		else
			m_ItemRow[i].ClearRow();
	}
}

//////////////////////////////////////////////////////////////////////
// KUiAuctionManager
//////////////////////////////////////////////////////////////////////
KUiAuctionManager* KUiAuctionManager::ms_pSelf = NULL;

KUiAuctionManager::KUiAuctionManager()
{
	m_szScheme[0] = 0;
}

KUiAuctionManager::~KUiAuctionManager()
{
	ms_pSelf = NULL;
}

KUiAuctionManager* KUiAuctionManager::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiAuctionManager;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->BringToTop();
		ms_pSelf->Show();
		ms_pSelf->m_Members.Hide();
		sAucLog("[UI] OpenWindow visible=%d", ms_pSelf->IsVisible() ? 1 : 0);
	}
	return ms_pSelf;
}

KUiAuctionManager* KUiAuctionManager::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}

void KUiAuctionManager::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		if (ms_pSelf->IsVisible())
			UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->m_Members.Hide();
		ms_pSelf->Hide();
		sSendOp(AUCUI_OP_CLOSE, 0);
		if (bDestory)
		{
			ms_pSelf->m_Page.ClearItem();
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}

void KUiAuctionManager::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_TabTong);
	AddChild(&m_TabWorld);
	AddChild(&m_TabPersonal);
	AddChild(&m_Close);
	m_Page.Build();
	AddChild(&m_Page);
	m_Members.Build();
	AddChild(&m_Members);
	g_UiBase.GetCurSchemePath(m_szScheme, sizeof(m_szScheme));
	LoadScheme(m_szScheme);
	Wnd_AddWindow(this);
}

void KUiAuctionManager::LoadScheme(const char* pScheme)
{
	if (!ms_pSelf)
		return;
	char Buff[256];
	KIniFile Ini;
	sprintf(Buff, "%s\\%s", pScheme, AUC_MANAGER_INI);
	if (Ini.Load(Buff))
	{
		ms_pSelf->Init(&Ini, "Main");
		ms_pSelf->m_Title.Init(&Ini, "Title");
		ms_pSelf->m_TabTong.Init(&Ini, "AuctionTong");
		ms_pSelf->m_TabWorld.Init(&Ini, "AuctionWorld");
		ms_pSelf->m_TabPersonal.Init(&Ini, "AuctionPersonal");
		ms_pSelf->m_Close.Init(&Ini, "Close");
	}
	else
		sAucLog("[UI] KHONG nap duoc %s", Buff);
	ms_pSelf->m_Page.LoadScheme(pScheme, ms_pSelf->m_Page.m_nType);
	ms_pSelf->m_Members.LoadScheme(pScheme);
	ms_pSelf->Switch(ms_pSelf->m_Page.m_nType);
}

void KUiAuctionManager::Switch(int nType)
{
	if (nType < AUCUI_TYPE_TONG || nType > AUCUI_TYPE_PERSONAL)
		nType = AUCUI_TYPE_PERSONAL;
	m_TabTong.CheckButton(nType == AUCUI_TYPE_TONG);
	m_TabWorld.CheckButton(nType == AUCUI_TYPE_WORLD);
	m_TabPersonal.CheckButton(nType == AUCUI_TYPE_PERSONAL);
	if (m_Page.m_nType != nType)
	{
		m_Page.ClearItem();
		m_Page.ClearActivity();
		m_Page.LoadScheme(m_szScheme, nType);
	}
	m_Members.Hide();
	m_Page.Show();
}

int KUiAuctionManager::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Close)
		{
			CloseWindow(false);
			return 1;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_TabTong)
		{
			sSendOp(AUCUI_OP_PAGE_BTN, AUCUI_TYPE_TONG);
			return 1;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_TabWorld)
		{
			sSendOp(AUCUI_OP_PAGE_BTN, AUCUI_TYPE_WORLD);
			return 1;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_TabPersonal)
		{
			sSendOp(AUCUI_OP_PAGE_BTN, AUCUI_TYPE_PERSONAL);
			return 1;
		}
		break;
	default:
		break;
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}

//////////////////////////////////////////////////////////////////////
// KUiAuctionIcon
//////////////////////////////////////////////////////////////////////
KUiAuctionIcon* KUiAuctionIcon::ms_pSelf = NULL;

KUiAuctionIcon::KUiAuctionIcon()
{
	m_bBlink = 0;
	m_nFrame = 0;
	m_nBlinkLeft = 0;
}

void KUiAuctionIcon::Initialize()
{
	AddChild(&m_Btn);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this);
}

void KUiAuctionIcon::LoadScheme(const char* pScheme)
{
	if (!ms_pSelf)
		return;
	char Buff[256];
	KIniFile Ini;
	sprintf(Buff, "%s\\%s", pScheme, AUC_ICON_INI);
	if (Ini.Load(Buff))
	{
		ms_pSelf->Init(&Ini, "Main");
		ms_pSelf->m_Btn.Init(&Ini, "AuctionBtn");
		int nX = 0, nY = 0;
		ms_pSelf->GetPosition(&nX, &nY);
		// dat ngay DUOI bieu tuong thu (thu: SCREEN_WIDTH-30, y 296): cung cot, cach 26
		if (SCREEN_WIDTH == 1024)
			ms_pSelf->SetPosition(SCREEN_WIDTH - 30, nY);
		sAucLog("[UI] bieu tuong dau gia tai (%d,%d)", nX, nY);
	}
	else
		sAucLog("[UI] KHONG nap duoc %s", Buff);
}

void KUiAuctionIcon::SetVisible(int bVisible)
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiAuctionIcon;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (!ms_pSelf)
		return;
	ms_pSelf->m_Btn.Show();
	ms_pSelf->Show();
	if (!ms_pSelf->m_bBlink)
		ms_pSelf->m_Btn.SetFrame(bVisible ? AUCUI_ICON_FRAME_HOT : AUCUI_ICON_FRAME_NORMAL);
}

void KUiAuctionIcon::Blink()
{
	SetVisible(1);
	if (ms_pSelf)
	{
		ms_pSelf->m_bBlink = 1;
		ms_pSelf->m_nFrame = 0;
		ms_pSelf->m_nBlinkLeft = AUCUI_BLINK_TOTAL;
	}
}

void KUiAuctionIcon::Breathe()
{
	if (!m_bBlink)
		return;
	m_nFrame++;
	if (m_nBlinkLeft > 0)
		m_nBlinkLeft--;
	if (m_nBlinkLeft <= 0)
	{
		m_bBlink = 0;
		m_Btn.SetFrame(AUCUI_ICON_FRAME_NORMAL);
		return;
	}
	if (m_nFrame >= AUCUI_BLINK_FRAMES)
	{
		m_nFrame = 0;
		m_Btn.SetFrame(m_Btn.GetCurrentFrame() == AUCUI_ICON_FRAME_HOT ? AUCUI_ICON_FRAME_NORMAL : AUCUI_ICON_FRAME_HOT);
	}
}

int KUiAuctionIcon::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_BUTTON_CLICK && uParam == (unsigned int)(KWndWindow*)&m_Btn)
	{
		m_bBlink = 0;
		m_Btn.Show();
		m_Btn.SetFrame(AUCUI_ICON_FRAME_NORMAL);
		KUiAuctionManager* p = KUiAuctionManager::GetIfVisible();
		sSendOp(AUCUI_OP_ICON_CLICK, p ? 0 : 1);
		return 1;
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}

void KUiAuctionIcon::Release()
{
	if (ms_pSelf)
	{
		ms_pSelf->Destroy();
		ms_pSelf = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// Core -> UI
//////////////////////////////////////////////////////////////////////
static KUiAuctionManager* sEnsureManager()
{
	if (KUiAuctionManager::GetSelf() == NULL)
		KUiAuctionManager::OpenWindow();
	return KUiAuctionManager::GetSelf();
}

void KUiAuction_OnCoreCmd(unsigned int uCmd, int nParam)
{
	KUiAuctionManager* p;
	if (!s_bAucGameStarted)
	{
		switch (uCmd)
		{
		case AUCUI_CMD_OPEN:
			s_bAucPendingOpen = 1;
			return;
		case AUCUI_CMD_SWITCH:
			s_nAucPendingSwitch = nParam;
			return;
		case AUCUI_CMD_ICON_VISIBLE:
			s_nAucPendingIcon = nParam;
			return;
		case AUCUI_CMD_NEW_EVENT:
			s_nAucPendingIcon = 1;
			return;
		default:
			break;
		}
	}
	switch (uCmd)
	{
	case AUCUI_CMD_OPEN:
		KUiAuctionManager::OpenWindow();
		break;
	case AUCUI_CMD_CLOSE:
		KUiAuctionManager::CloseWindow(false);
		break;
	case AUCUI_CMD_SWITCH:
		if ((p = sEnsureManager()) != NULL)
			p->Switch(nParam);
		break;
	case AUCUI_CMD_NEW_EVENT:
		KUiAuctionIcon::Blink();
		break;
	case AUCUI_CMD_ICON_VISIBLE:
		KUiAuctionIcon::SetVisible(nParam);
		break;
	case AUCUI_CMD_ADD_ACTIVITY:
		if ((p = sEnsureManager()) != NULL)
			p->m_Page.AddActivity((const KAucUiActivity*)nParam);
		break;
	case AUCUI_CMD_DEL_ACTIVITY:
		if ((p = KUiAuctionManager::GetSelf()) != NULL && nParam)
			p->m_Page.DelActivity(((const KAucUiActivity*)nParam)->szName);
		break;
	case AUCUI_CMD_CLEAR_ACTIVITY:
		if ((p = KUiAuctionManager::GetSelf()) != NULL)
			p->m_Page.ClearActivity();
		break;
	case AUCUI_CMD_SELECT_ACTIVITY:
		if ((p = KUiAuctionManager::GetSelf()) != NULL && nParam)
			p->m_Page.SelectActivity(((const KAucUiActivity*)nParam)->szName);
		break;
	case AUCUI_CMD_ADD_ITEM:
		if ((p = sEnsureManager()) != NULL)
			p->m_Page.AddItem((const KAucUiItem*)nParam);
		break;
	case AUCUI_CMD_SET_ITEM:
		if ((p = KUiAuctionManager::GetSelf()) != NULL)
			p->m_Page.SetItem((const KAucUiItem*)nParam);
		break;
	case AUCUI_CMD_END_ITEM:
		if ((p = KUiAuctionManager::GetSelf()) != NULL)
			p->m_Page.EndItem(nParam);
		break;
	case AUCUI_CMD_CLEAR_ITEM:
		if ((p = KUiAuctionManager::GetSelf()) != NULL)
			p->m_Page.ClearItem();
		break;
	case AUCUI_CMD_ADD_MEMBER:
		if ((p = KUiAuctionManager::GetSelf()) != NULL)
			p->m_Members.AddMember((const KAucUiMember*)nParam);
		break;
	case AUCUI_CMD_CLEAR_MEMBER:
		if ((p = KUiAuctionManager::GetSelf()) != NULL)
			p->m_Members.Clear();
		break;
	case AUCUI_CMD_SET_SALARY:
		// [DAUGIA 04/09 A4] may chu gui danh sach thanh vien = nguoi choi vua bam nut o trang Bang hoi
		// (khong phai bang chu) -> mo luon bang thanh vien.
		if ((p = KUiAuctionManager::GetSelf()) != NULL)
		{
			p->m_Members.SetSalary((const KAucUiSalary*)nParam);
			p->m_Members.BringToTop();
			p->m_Members.Show();
		}
		break;
	case AUCUI_CMD_SET_PAGE_TXT:
		if ((p = KUiAuctionManager::GetSelf()) != NULL)
			p->m_Page.SetPageText((const char*)nParam);
		break;
	case AUCUI_CMD_CLEAR_ALL:
		if ((p = KUiAuctionManager::GetSelf()) != NULL)
		{
			p->m_Page.ClearItem();
			p->m_Page.ClearActivity();
			p->m_Members.Clear();
		}
		break;
	case AUCUI_CMD_PUTON_MODE:
		// [A6] chu 04/09: gop lam MOT hop - hop dua vat pham co them o gia + nut doi loai tien
		// [A28 04/09] tri 2 = DONG hop (ky gui xong). Dung 2 thay vi them mot lenh moi de khong
		// phai doi KAuctionUiDef.h - chen them tri vao giua enum la lech het ba tep nhi phan.
		// Dong bang dung duong CloseWindow(true) cho an toan: no chay ca OnCancel, ma OnCancel
		// ban lenh THU HOI vat pham trong hop - luc nay may chu da lay mon nen la lenh rong,
		// con neu vi ly do gi mon van con trong hop thi no duoc tra ve hanh trang thay vi ket lai.
		if (nParam == 2)
			KUiAffairItem::CloseWindow(true);
		else
			KUiAffairItem::SetAuctionMode(nParam);
		break;
	case AUCUI_CMD_SET_MONEY:
		if ((p = KUiAuctionManager::GetSelf()) != NULL && nParam)
			p->m_Page.SetMoney(((const KAucUiSalary*)nParam)->nCount, ((const KAucUiSalary*)nParam)->nSalary);
		break;
	default:
		break;
	}
}

void KUiAuction_OnGameStart()
{
	s_bAucGameStarted = 1;
	KUiAuctionIcon::SetVisible(s_nAucPendingIcon > 0 ? 1 : 0);
	if (s_bAucPendingOpen)
	{
		KUiAuctionManager::OpenWindow();
		if (s_nAucPendingSwitch)
			KUiAuctionManager::GetSelf()->Switch(s_nAucPendingSwitch);
	}
	sAucLog("[UI] GAME_START: icon=%d mo=%d", s_nAucPendingIcon, s_bAucPendingOpen);
	s_bAucPendingOpen = 0;
	s_nAucPendingIcon = -1;
	s_nAucPendingSwitch = 0;
}

void KUiAuction_OnGameExit()
{
	s_bAucGameStarted = 0;
	s_bAucPendingOpen = 0;
	s_nAucPendingIcon = -1;
	s_nAucPendingSwitch = 0;
	KUiAuctionManager::CloseWindow(true);
	KUiAuctionIcon::Release();
	sSendOp(AUCUI_OP_RESET, 0);
	sAucLog("[UI] EXIT_GAME: da don cua so dau gia");
}
