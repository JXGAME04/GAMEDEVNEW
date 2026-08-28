//---------------------------------------------------------------------------
// KItemCompound.cpp - HE LO REN ([hop thanh] / compound), VIET MOI 100% theo
// ban Linux. Xem KItemCompound.h de biet nguon goc tung khai niem.
//
// GHI CHU VE MA HOA: tep nay chi chua ASCII. Moi chuoi hien cho nguoi choi
// deu nam trong kich ban Lua (TCVN3), khong nam o day.
//---------------------------------------------------------------------------

// Core build voi PCH "Use" qua KCore.h - KCore.h PHAI dung dau tien.
#include <stdarg.h>	// [LOREN 28/08] log kiem ghi ra tep
#include "KCore.h"
#include "KWin32.h"

#ifdef _SERVER

#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KLuaScript.h"
#include "KFilePath.h"
#include "KTabFile.h"
#include "KIniFile.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "KNpc.h"
#include "KNpcSet.h"
// KPlayer.h tu keo du chuoi KItem/KInventory/KItemList theo dung thu tu
#include "KPlayerSet.h"
#include "KPlayer.h"
#include "KItem.h"
#include "KItemSet.h"
#include "KItemGenerator.h"
#include "KObjSet.h"
#include "KSortScript.h"
#include "KItemCompound.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

KItemCompound	g_ItemCompound;

// dinh nghia trong ScriptFuns.cpp
extern int GetPlayerIndex(Lua_State* L);

#define COMPOUND_SCRIPT_FILE	"\\settings\\item\\compoundscript.txt"
#define ITEMVALUE_SCRIPT_FILE	"\\settings\\item\\itemvaluescript.txt"

#define COMPOUND_MAX_MATERIAL	32		// tran so o nguyen lieu mot lan lo ren
#define COMPOUND_MAX_DEPTH		8		// tran de quy CalcItemValue

//---------------------------------------------------------------------------
// Tien ich
//---------------------------------------------------------------------------
static void sStrCpyLen(char* szDst, const char* szSrc, int nDstSize)
{
	if (!szDst || nDstSize <= 0)
		return;
	strncpy(szDst, szSrc ? szSrc : "", nDstSize - 1);
	szDst[nDstSize - 1] = 0;
}

// Cat khoang trang hai dau (bang .txt hay co khoang trang thua o cot ten).
static void sTrim(char* sz)
{
	if (!sz)
		return;
	char* p = sz;
	while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
		p++;
	if (p != sz)
		memmove(sz, p, strlen(p) + 1);
	int n = (int)strlen(sz);
	while (n > 0 && (sz[n - 1] == ' ' || sz[n - 1] == '\t' ||
					 sz[n - 1] == '\r' || sz[n - 1] == '\n'))
		sz[--n] = 0;
}

// O trong -> -1 (khop bat ky); nguoc lai -> so.
// Ban Linux thu "bat ky" bang DAU AM chu khong bang mot hang rieng, nen o ghi
// -5 cung la bat ky. Quy o rong ve -1 la khop dung hanh vi do.
static int sCellToRule(KTabFile* pTab, int nRow, const char* szCol)
{
	char szBuf[64] = "";
	pTab->GetString(nRow, (LPSTR)szCol, (LPSTR)"", szBuf, sizeof(szBuf) - 1);
	sTrim(szBuf);
	if (szBuf[0] == 0)
		return KVR_ANY;
	return atoi(szBuf);
}

//---------------------------------------------------------------------------
// Doi pham chat: quy uoc Linux <-> nItemNature cua JX1.
//   Linux 0 thuong        <-> NATURE_NORMAL = 0
//   Linux 1 hoang kim     <-> NATURE_GOLD   = 2
//   Linux 2 kham nam duoc <-> NATURE_VIOLET = 1
//---------------------------------------------------------------------------
int g_QualityLinuxToNature(int nQuality)
{
	switch (nQuality)
	{
	case ITEMQUALITY_NORMAL:		return NATURE_NORMAL;
	case ITEMQUALITY_GOLD:			return NATURE_GOLD;
	case ITEMQUALITY_ENCHASABLE:	return NATURE_VIOLET;
	case ITEMQUALITY_PLATINA:		return NATURE_PLATINA;
	default:						break;
	}
	return NATURE_NORMAL;
}

int g_QualityNatureToLinux(int nNature)
{
	switch (nNature)
	{
	case NATURE_NORMAL:		return ITEMQUALITY_NORMAL;
	case NATURE_GOLD:		return ITEMQUALITY_GOLD;
	case NATURE_VIOLET:		return ITEMQUALITY_ENCHASABLE;
	case NATURE_PLATINA:	return ITEMQUALITY_PLATINA;
	default:				break;
	}
	return ITEMQUALITY_NORMAL;
}

//---------------------------------------------------------------------------
// Duong dan bang theo phien ban vat pham. Ban goc: item_header.lua
//     format( "\\settings\\item\\%03d\\%s", nItemVer, strFileName )
//---------------------------------------------------------------------------
void g_MakeItemFilePath(int nItemVer, const char* szFileName, char* szOut, int nOutSize)
{
	if (!szOut || nOutSize <= 0)
		return;
	if (nItemVer < 0)
		nItemVer = 0;
	_snprintf(szOut, nOutSize - 1, "\\settings\\item\\%03d\\%s",
			  nItemVer, szFileName ? szFileName : "");
	szOut[nOutSize - 1] = 0;
}

//---------------------------------------------------------------------------
// ITEM_GetLatestItemVersion: phien ban vat pham moi nhat may chu dang dung.
// JX1 giu o g_SubWorldSet.GetGameVersion() (khoi tao = ITEM_VERSION).
//---------------------------------------------------------------------------
int g_GetLatestItemVersion()
{
	int nVer = g_SubWorldSet.GetGameVersion();
	if (nVer < 0)
		nVer = 0;
	return nVer;
}

//===========================================================================
//  KFoundryResDemand - dieu kien nguyen lieu (xem KItemCompound.h)
//===========================================================================
KFoundryResDemand	g_FoundryResDemand;

#define FOUNDRY_RES_DEMAND_FILE		"\\settings\\item\\foundryresdemand.ini"

// Ten 21 khoa, DUNG thu tu bang 0x0825EC20 cua ban goc.
static const char* s_szKhoa[FRK_COUNT] =
{
	"CompoundCryolite_Ring", "CompoundCryolite_Necklace", "CompoundCryolite_Pendant",
	"UpgradeCryolite", "UpgradePropMine",
	"Distill_Equip", "Distill_Cryolite", "Distill_OrgMine", "Distill_EnhanceItem",
	"Forge_Equip", "Forge_Cryolite",
	"Enchase_Equip", "Enchase_Cryolite", "Enchase_PropMine", "Enchase_EnhanceItem",
	"CompoundGold_Cryolite", "CompoundGold_Atlas", "CompoundGold_Material",
	"CompoundGold_EnhanceItem",
	"UpgradeFantasyGold", "UpgradeFantasyGoldEssence",
};

// Khoa ma tung COMPOUND_TYPE dung. Do tu bang 9 ham kiem 0x0825EBC0.
// Bon khoa 8/14/17/18 (…_EnhanceItem, CompoundGold_Material) KHONG ham may chu
// nao dung - chung la bo loc cua CLIENT de biet duoc keo gi vao o tu chon.
static const int s_anKhoa0[] = { FRK_CompoundCryolite_Ring, FRK_CompoundCryolite_Pendant, FRK_CompoundCryolite_Necklace };
static const int s_anKhoa1[] = { FRK_UpgradeCryolite };
static const int s_anKhoa2[] = { FRK_UpgradePropMine };
static const int s_anKhoa3[] = { FRK_Distill_Equip, FRK_Distill_Cryolite, FRK_Distill_OrgMine };
static const int s_anKhoa4[] = { FRK_Forge_Equip, FRK_Forge_Cryolite };
static const int s_anKhoa5[] = { FRK_Enchase_Equip, FRK_Enchase_Cryolite, FRK_Enchase_PropMine };
static const int s_anKhoa6[] = { FRK_CompoundGold_Cryolite, FRK_CompoundGold_Atlas };
static const int s_anKhoa7[] = { FRK_UpgradeFantasyGold };
static const int s_anKhoa8[] = { FRK_UpgradeFantasyGoldEssence };

// So O NGUYEN LIEU CHINH, doc THANG tu bang 0x08257F80 cua ban goc:
//   type: 0  1  2  3  4  5  6  7  8
//   so o: 3  3  3  3  2  3  8  3  3
// (Duc do tim chi 2 o = trang bi + Huyen Tinh; Do pho 8 o = Huyen Tinh + do pho
//  + toi 6 nguyen lieu.)
static const int s_anSoO[COMPOUND_TYPE_COUNT] = { 3, 3, 3, 3, 2, 3, 8, 3, 3 };

int KFoundryResDemand::LaySoO(int nCompoundType)
{
	if (nCompoundType < 0 || nCompoundType >= COMPOUND_TYPE_COUNT)
		return 0;
	return s_anSoO[nCompoundType];
}

// NHOM KHOA. Cac khoa trong CUNG mot nhom la LUA CHON THAY THE NHAU: nhom coi
// nhu du khi co MOT nguyen lieu khop bat ky khoa nao trong nhom.
// Vi sao can: Tinh luyen (type 3) lay nguon la TRANG BI hoac KHOANG - ban goc
// de hai thu o hai o khac nhau va cho bo trong mot o. JX1 nhan nguyen lieu qua
// hop trao doi, khong co o trong, nen phai dien ta bang nhom.
struct KNhomKhoa
{
	const int*	pnKhoa;
	int			nKhoa;
};
static const int s_g0a[] = { FRK_CompoundCryolite_Ring };
static const int s_g0b[] = { FRK_CompoundCryolite_Necklace };
static const int s_g0c[] = { FRK_CompoundCryolite_Pendant };
static const int s_g1[]  = { FRK_UpgradeCryolite };
static const int s_g2[]  = { FRK_UpgradePropMine };
// [LOREN 27/08] Ban goc doi CA HAI, khong phai "hoac":
//   magic_distill.lua:30-37 tim TRANG BI (nGenre == 0)
//   magic_distill.lua:38-45 tim NGUYEN KHOANG (6/1/149..154 ban Linux)
//   magic_distill.lua:48    if( nEquipIdx <= 0 or g_nDistillMagicPos <= 0 )
//                               then return RESULT_LACK_RESOURCE
// Gop chung mot nhom la sai: cac khoa cung nhom THAY THE nhau va moi nguyen
// lieu chi phuc vu duoc mot nhom, nen trang bi chiem cho roi thi nguyen
// khoang thanh "o thua" -> s_anKhoaThua[3] = -1 -> RULE_ERROR (ma 8).
static const int s_g3a[] = { FRK_Distill_Equip };
static const int s_g3c[] = { FRK_Distill_OrgMine };
static const int s_g3b[] = { FRK_Distill_Cryolite };
static const int s_g4a[] = { FRK_Forge_Equip };
static const int s_g4b[] = { FRK_Forge_Cryolite };
static const int s_g5a[] = { FRK_Enchase_Equip };
static const int s_g5b[] = { FRK_Enchase_Cryolite };
static const int s_g5c[] = { FRK_Enchase_PropMine };
static const int s_g6a[] = { FRK_CompoundGold_Cryolite };
static const int s_g6b[] = { FRK_CompoundGold_Atlas };
static const int s_g7[]  = { FRK_UpgradeFantasyGold };
static const int s_g8[]  = { FRK_UpgradeFantasyGoldEssence };

static const KNhomKhoa s_aryNhom0[] = { {s_g0a,1}, {s_g0b,1}, {s_g0c,1} };
static const KNhomKhoa s_aryNhom1[] = { {s_g1,1} };
static const KNhomKhoa s_aryNhom2[] = { {s_g2,1} };
static const KNhomKhoa s_aryNhom3[] = { {s_g3a,1}, {s_g3c,1}, {s_g3b,1} };	// trang bi + nguyen khoang + Huyen Tinh
static const KNhomKhoa s_aryNhom4[] = { {s_g4a,1}, {s_g4b,1} };
static const KNhomKhoa s_aryNhom5[] = { {s_g5a,1}, {s_g5b,1}, {s_g5c,1} };
static const KNhomKhoa s_aryNhom6[] = { {s_g6a,1}, {s_g6b,1} };
static const KNhomKhoa s_aryNhom7[] = { {s_g7,1} };
static const KNhomKhoa s_aryNhom8[] = { {s_g8,1} };

// Khoa cho cac o CON LAI (ngoai cac nhom bat buoc). -1 = khong cho mon nao khac.
//   type 1/2/7/8: ba o deu cung mot loai khoang -> o thua van phai la khoang do.
//   type 6 (Do pho): sau o con lai la NGUYEN LIEU. Ban goc khong kiem chung o
//     may chu vi cua so lo ren cua client da chan san bang khoa 17; JX1 khong co
//     cua so do nen phai kiem o day, khong thi nhet gi vao cung duoc.
static const int s_anKhoaThua[COMPOUND_TYPE_COUNT] =
{
	-1,
	FRK_UpgradeCryolite,
	FRK_UpgradePropMine,
	-1,
	-1,
	-1,
	FRK_CompoundGold_Material,
	FRK_UpgradeFantasyGold,
	FRK_UpgradeFantasyGoldEssence,
};

// KHOA CHO O TU CHON (o "tang ty le"). CHAT HON BAN GOC - co y.
// Ban goc KHONG kiem o tu chon o may chu: chi vector o chinh di qua bang 9 ham
// kiem, con vector o tu chon di thang vao bo thuc thi. Bon khoa *_EnhanceItem
// chi la bo loc cua CLIENT, de nguoi choi biet duoc keo gi vao o do.
// JX1 KHONG CO cua so lo ren nen khong co bo loc client nao ca; de trong thi
// nguoi choi nhet mon gia tri cao vao o tu chon de keo ty le len
// (compound_header.lua cong sumItemsVal cua ca hai vector). Nen kiem o day.
static const int s_anKhoaTuChon[COMPOUND_TYPE_COUNT] =
{
	-1,									// 0 hop thanh Huyen Tinh: khong co o tu chon
	-1,									// 1 nang cap khoang
	-1,									// 2 nang cap khoang thuoc tinh
	FRK_Distill_EnhanceItem,			// 3 tinh luyen
	-1,									// 4 duc do tim
	FRK_Enchase_EnhanceItem,			// 5 kham nam
	FRK_CompoundGold_EnhanceItem,		// 6 do pho Hoang Kim
	-1,									// 7 Huyen Kim
	-1,									// 8 Huyen Kim Chi Tinh
};

static int sLayNhom(int nCompoundType, const KNhomKhoa** ppNhom)
{
	struct { const KNhomKhoa* p; int n; } aryBang[COMPOUND_TYPE_COUNT] =
	{
		{ s_aryNhom0, 3 }, { s_aryNhom1, 1 }, { s_aryNhom2, 1 },
		{ s_aryNhom3, 3 }, { s_aryNhom4, 2 }, { s_aryNhom5, 3 },
		{ s_aryNhom6, 2 }, { s_aryNhom7, 1 }, { s_aryNhom8, 1 },
	};
	if (nCompoundType < 0 || nCompoundType >= COMPOUND_TYPE_COUNT)
	{
		if (ppNhom)
			*ppNhom = NULL;
		return 0;
	}
	if (ppNhom)
		*ppNhom = aryBang[nCompoundType].p;
	return aryBang[nCompoundType].n;
}

KFoundryResDemand::KFoundryResDemand()
{
	m_bReady = FALSE;
	memset(m_bCoKhoa, 0, sizeof(m_bCoKhoa));
}

void KFoundryResDemand::Clear()
{
	m_bReady = FALSE;
	memset(m_bCoKhoa, 0, sizeof(m_bCoKhoa));
	for (int i = 0; i < FRK_COUNT; i++)
		m_aryKhoa[i].clear();
}

BOOL KFoundryResDemand::Init(int nScheme)
{
	Clear();

	KIniFile Ini;
	if (!Ini.Load((LPCSTR)FOUNDRY_RES_DEMAND_FILE))
	{
		printf("[LOREN] KHONG nap duoc %s - he lo ren SE KHONG nhan nguyen lieu\n",
			   FOUNDRY_RES_DEMAND_FILE);
		return FALSE;
	}

	char szScheme[64];
	_snprintf(szScheme, sizeof(szScheme) - 1, "ResScheme_%d", nScheme);
	szScheme[sizeof(szScheme) - 1] = 0;
	if (!Ini.IsSectionExist(szScheme))
	{
		printf("[LOREN] %s khong co muc [%s]\n", FOUNDRY_RES_DEMAND_FILE, szScheme);
		return FALSE;
	}

	int nTietDoan = 0;
	for (int k = 0; k < FRK_COUNT; k++)
	{
		char szValue[512];
		szValue[0] = 0;
		if (!Ini.GetString(szScheme, (LPCSTR)s_szKhoa[k], "", szValue, sizeof(szValue)))
			continue;
		if (szValue[0] == 0)
			continue;
		m_bCoKhoa[k] = TRUE;

		// gia tri la danh sach ten loai nguyen lieu, ngan bang dau phay
		char szTam[512];
		strncpy(szTam, szValue, sizeof(szTam) - 1);
		szTam[sizeof(szTam) - 1] = 0;
		char* pSave = szTam;
		while (pSave && *pSave)
		{
			char* pPhay = strchr(pSave, ',');
			if (pPhay)
				*pPhay = 0;
			// cat khoang trang hai dau
			while (*pSave == ' ' || *pSave == '\t')
				pSave++;
			int nLen = (int)strlen(pSave);
			while (nLen > 0 && (pSave[nLen - 1] == ' ' || pSave[nLen - 1] == '\t'))
				pSave[--nLen] = 0;

			if (nLen > 0)
			{
				// CHOT TRAN DEM: KIniFile::IsSectionExist dung char szSection[32]
				// roi g_StrCat KHONG gioi han (KIniFile.cpp:606, KStrBase.cpp:279)
				// => ten tiet doan dai qua 29 ky tu la DAP NGAN XEP GameServer,
				// va duong tran do chinh TEP DU LIEU dieu khien. Chan tai day.
				if ((int)strlen(pSave) > 24)
				{
					printf("[LOREN] ten loai nguyen lieu qua dai, bo qua: %s\n", pSave);
					pSave = pPhay ? (pPhay + 1) : NULL;
					continue;
				}
				// doc cac tiet doan [<ten>_1], [<ten>_2], ... cho toi khi het
				int i = 1;
				for (; i <= 256; i++)
				{
					char szSec[128];
					_snprintf(szSec, sizeof(szSec) - 1, "%s_%d", pSave, i);
					szSec[sizeof(szSec) - 1] = 0;
					if (!Ini.IsSectionExist(szSec))
						break;
					KResDemand D;
					// *** MOI TRUONG MAC DINH -1 = KHONG KIEM ***
					// (ban goc: 0x0814EF0F va cac lenh ke tiep dat -1 truoc khi doc)
					Ini.GetInteger(szSec, (LPCSTR)"Quality",    FRD_BOQUA, &D.nQuality);
					Ini.GetInteger(szSec, (LPCSTR)"Genre",      FRD_BOQUA, &D.nGenre);
					Ini.GetInteger(szSec, (LPCSTR)"DetailType", FRD_BOQUA, &D.nDetailType);
					Ini.GetInteger(szSec, (LPCSTR)"PtcType",    FRD_BOQUA, &D.nPtcType);
					Ini.GetInteger(szSec, (LPCSTR)"Stackable",  FRD_BOQUA, &D.nStackable);
					m_aryKhoa[k].push_back(D);
					nTietDoan++;
				}
				// Cat AM THAM la kieu loi tung lam sap GameServer 26/08: neu
				// cham tran thi phai keu len, khong duoc lang le bo phan con lai.
				if (i > 256)
					printf("[LOREN] *** %s co qua 256 tiet doan, da CAT BOT\n", pSave);
			}
			pSave = pPhay ? (pPhay + 1) : NULL;
		}
	}

	m_bReady = TRUE;
	printf("[LOREN] nap %s so do %d: %d tiet doan nguyen lieu\n",
		   FOUNDRY_RES_DEMAND_FILE, nScheme, nTietDoan);
	return TRUE;
}

// [LOREN 27/08] LOG CHAN DOAN duong kiem nguyen lieu.
// Chi IN, khong doi logic. Goi tu Check()/CheckTuChon() khi TU CHOI, de biet
// chinh xac mon nao truot thay vi doan. Go bo bang w3_log_kiemnguyenlieu.py --go
// [LOREN 28/08] log kiem ghi ra tep - ghi ra tep de doc lai duoc (printf chi ra console).
static void sLoRenGhiLog(const char* szDinhDang, ...)
{
	FILE* f = fopen("loren_kiem.log", "a");
	if (!f)
		return;
	va_list ap;
	va_start(ap, szDinhDang);
	vfprintf(f, szDinhDang, ap);
	va_end(ap);
	fclose(f);
}

static void sLoRenInMon(const char* szNhan, int nThuTu, int nItemIdx)
{
	if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
	{
		sLoRenGhiLog("[LOREN-KIEM]   %s o%d: chi so vat pham KHONG HOP LE (%d)\n",
			   szNhan, nThuTu, nItemIdx);
		return;
	}
	KItem* p = &Item[nItemIdx];
	sLoRenGhiLog("[LOREN-KIEM]   %s o%d: genre=%d detail=%d ptc=%d cap=%d he=%d chong=%d nature=%d\n",
		   szNhan, nThuTu, p->GetGenre(), p->GetDetailType(), p->GetParticular(),
		   p->m_CommonAttrib.nLevel, p->m_CommonAttrib.nSeries,
		   p->GetStackNum(), p->m_CommonAttrib.nItemNature);
}

BOOL KFoundryResDemand::KhopTietDoan(const KResDemand& D, int nItemIdx)
{
	if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
		return FALSE;
	KItem* pItem = &Item[nItemIdx];

	if (D.nGenre != FRD_BOQUA && D.nGenre != pItem->GetGenre())
		return FALSE;
	if (D.nDetailType != FRD_BOQUA && D.nDetailType != pItem->GetDetailType())
		return FALSE;
	if (D.nPtcType != FRD_BOQUA && D.nPtcType != pItem->GetParticular())
		return FALSE;
	if (D.nQuality != FRD_BOQUA)
	{
		// ini ghi theo he pham chat cua JX1 (da nan bang v30), so thang.
		if (D.nQuality != pItem->m_CommonAttrib.nItemNature)
			return FALSE;
	}

	// Chot chan vat pham DANG XEP CHONG (ban goc 0x0814ED29..0x0814ED6E).
	// Ban goc chi cho moi o cua so lo ren mot don vi; mon dang chong chi duoc
	// nhan khi tiet doan ghi ro Stackable=1 (chi cac muc ConsumeItem_* co).
	// Thieu chot nay thi:
	//   - gia tri nguyen lieu nhan theo ca chong (KItemCompound.cpp) trong khi
	//     ban goc tinh mot don vi  => bom gia tri
	//   - ore_upgrade.lua:97 cho xac suat AM khi tong gia tri qua lon
	//     => luon that bai ma van huy ca chong, thiet cho nguoi choi
	if (D.nStackable != 1 && pItem->GetStackNum() >= 2)
		return FALSE;

	return TRUE;
}

BOOL KFoundryResDemand::KhopMotKhoa(int nItemIdx, int nKey) const
{
	if (nKey < 0 || nKey >= FRK_COUNT)
		return FALSE;
	for (size_t i = 0; i < m_aryKhoa[nKey].size(); i++)
	{
		if (KhopTietDoan(m_aryKhoa[nKey][i], nItemIdx))
			return TRUE;
	}
	return FALSE;
}

int KFoundryResDemand::Check(int nCompoundType, const int* pnItem, int nCount) const
{
	if (!m_bReady)
		return FOUNDRY_RESULT_UNKNOWN;

	const KNhomKhoa* pNhom = NULL;
	int nSoNhom = sLayNhom(nCompoundType, &pNhom);
	if (nSoNhom <= 0 || pNhom == NULL)
		return FOUNDRY_RESULT_UNKNOWN;

	// (1) SO O. Ban goc doc so o cua tung thao tac tu bang 0x08257F80 va tu
	//     choi ngay khi goi tin bao nhieu hon. Thieu chan nay thi nguoi choi
	//     nhet toi COMPOUND_MAX_MATERIAL (32) mon vao mot lan duc de bom tong
	//     gia tri nguyen lieu len, keo xac suat ra mon dat len 1,0.
	int nSoO = LaySoO(nCompoundType);
	if (nCount <= 0 || nCount > nSoO)
		return FOUNDRY_RESULT_LACK_RESOURCE;

	// (2) Ghep tung NHOM BAT BUOC voi mot nguyen lieu rieng. Moi nguyen lieu
	//     chi phuc vu duoc mot nhom (danh dau da dung) - khong thi mot vien
	//     Huyen Tinh vua dong vai nguon vua dong vai phi.
	BOOL abDaDung[COMPOUND_MAX_MATERIAL];
	memset(abDaDung, 0, sizeof(abDaDung));
	for (int g = 0; g < nSoNhom; g++)
	{
		BOOL bDu = FALSE;
		for (int k = 0; k < pNhom[g].nKhoa && !bDu; k++)
		{
			int nKey = pNhom[g].pnKhoa[k];
			if (!m_bCoKhoa[nKey])
				continue;				// so do khong khai khoa nay
			for (int i = 0; i < nCount; i++)
			{
				if (abDaDung[i])
					continue;
				if (KhopMotKhoa(pnItem[i], nKey))
				{
					abDaDung[i] = TRUE;
					bDu = TRUE;
					break;
				}
			}
		}
		if (!bDu)
		{
			// [LOREN 27/08] LOG CHAN DOAN duong kiem nguyen lieu
			sLoRenGhiLog("[LOREN-KIEM] type=%d O CHINH n=%d -> thieu NHOM BAT BUOC thu %d (ma 4)\n",
				   nCompoundType, nCount, g);
			for (int j = 0; j < nCount; j++)
				sLoRenInMon("CHINH", j, pnItem[j]);
			return FOUNDRY_RESULT_LACK_RESOURCE;
		}
	}

	// (3) Nguyen lieu con thua phai khop khoa danh cho o thua; khoa -1 nghia la
	//     thao tac nay khong nhan them mon nao.
	int nKhoaThua = s_anKhoaThua[nCompoundType];
	for (int i = 0; i < nCount; i++)
	{
		if (abDaDung[i])
			continue;
		if (nKhoaThua < 0 || !m_bCoKhoa[nKhoaThua]
			|| !KhopMotKhoa(pnItem[i], nKhoaThua))
			return FOUNDRY_RESULT_RULE_ERROR;
	}

	return FOUNDRY_RESULT_SUCCEED;
}

// Kiem VECTOR O TU CHON. Xem giai thich o s_anKhoaTuChon: day la cho CHAT HON
// ban goc, bu lai viec JX1 khong co cua so lo ren de loc phia client.
int KFoundryResDemand::CheckTuChon(int nCompoundType, const int* pnItem, int nCount) const
{
	if (!m_bReady)
		return FOUNDRY_RESULT_UNKNOWN;
	if (nCount <= 0)
		return FOUNDRY_RESULT_SUCCEED;			// khong bo gi vao = hop le
	if (nCompoundType < 0 || nCompoundType >= COMPOUND_TYPE_COUNT)
		return FOUNDRY_RESULT_UNKNOWN;

	int nKhoa = s_anKhoaTuChon[nCompoundType];
	if (nKhoa < 0 || !m_bCoKhoa[nKhoa])
		return FOUNDRY_RESULT_RULE_ERROR;		// thao tac nay khong co o tu chon

	// [LOREN 27/08] TRUOC DAY LAY NHAM: dung `LaySoO()` - ham tra SO O CHINH
	// (Kham nam = 3) - lam tran cua vector O TU CHON. Chi can bo qua 3 mon phu
	// la bi tu choi, du giao dien co han 8 o phu va mon bo vao hoan toan hop le.
	// So o phu THAT, doc tu chinh giao dien va kich ban dang chay:
	//   Lay.ini / Khamnam co Consume1..8            -> 8 o
	//   compound_ui.lua: LR_UI_Lam(7,11,3,3) va (9,11,3,5)
	//       => 11 o, o 0..2 chinh, o 3..10 PHU     -> dung 8 o
	// Ba thao tac co o tu chon (Trich lay 3, Kham nam 5, Do pho 6) deu 8 o.
	const int nSoOTuChon = 8;
	if (nCount > nSoOTuChon)
		return FOUNDRY_RESULT_RULE_ERROR;

	for (int i = 0; i < nCount; i++)
	{
		if (!KhopMotKhoa(pnItem[i], nKhoa))
		{
			// [LOREN 27/08] LOG CHAN DOAN duong kiem nguyen lieu
			sLoRenGhiLog("[LOREN-KIEM] type=%d O TU CHON n=%d -> tu choi (ma 8) tai o thu %d\n",
				   nCompoundType, nCount, i);
			sLoRenGhiLog("[LOREN-KIEM]   khoa so %d: co=%d, %d tiet doan\n",
				   nKhoa, (int)m_bCoKhoa[nKhoa], (int)m_aryKhoa[nKhoa].size());
			for (int j = 0; j < nCount; j++)
				sLoRenInMon("TUCHON", j, pnItem[j]);
			return FOUNDRY_RESULT_RULE_ERROR;
		}
	}
	return FOUNDRY_RESULT_SUCCEED;
}

//---------------------------------------------------------------------------
KItemCompound::KItemCompound()
{
	m_bReady = FALSE;
	m_nCalcDepth = 0;
	memset(m_szCompoundScript, 0, sizeof(m_szCompoundScript));
}

KItemCompound::~KItemCompound()
{
}

void KItemCompound::Clear()
{
	m_bReady = FALSE;
	m_nCalcDepth = 0;
	memset(m_szCompoundScript, 0, sizeof(m_szCompoundScript));
	m_vRules.clear();
}

BOOL KItemCompound::Init()
{
	Clear();
	BOOL b1 = LoadCompoundScript();
	BOOL b2 = LoadItemValueScript();
	m_bReady = (b1 && b2);
	if (!m_bReady)
		printf("[LOREN] Init THAT BAI (compoundscript=%d itemvaluescript=%d)\n",
			   (int)b1, (int)b2);
	else
		printf("[LOREN] Init xong: %d kich ban, %d luat gia tri\n",
			   (int)COMPOUND_TYPE_COUNT, (int)m_vRules.size());
	return m_bReady;
}

//---------------------------------------------------------------------------
// settings\item\compoundscript.txt
//     COMPOUND_TYPE   COMPOUND_SCRIPT   REMARK
// Dong 1 la tieu de; du lieu bat dau tu dong 2 (quy uoc KTabFile cua JX1).
//---------------------------------------------------------------------------
BOOL KItemCompound::LoadCompoundScript()
{
	KTabFile Tab;
	if (!Tab.Load((LPSTR)COMPOUND_SCRIPT_FILE))
	{
		printf("[LOREN] khong nap duoc %s\n", COMPOUND_SCRIPT_FILE);
		return FALSE;
	}
	int nRows = Tab.GetHeight();
	int nFound = 0;
	for (int r = 2; r <= nRows; r++)
	{
		char szType[32] = "";
		char szPath[256] = "";
		Tab.GetString(r, (LPSTR)"COMPOUND_TYPE", (LPSTR)"", szType, sizeof(szType) - 1);
		Tab.GetString(r, (LPSTR)"COMPOUND_SCRIPT", (LPSTR)"", szPath, sizeof(szPath) - 1);
		sTrim(szType);
		sTrim(szPath);
		if (szType[0] == 0 || szPath[0] == 0)
			continue;
		int nType = atoi(szType);
		if (nType < 0 || nType >= COMPOUND_TYPE_COUNT)
		{
			printf("[LOREN] compoundscript.txt dong %d: COMPOUND_TYPE=%d ngoai dai 0..%d\n",
				   r, nType, COMPOUND_TYPE_COUNT - 1);
			continue;
		}
		sStrCpyLen(m_szCompoundScript[nType], szPath, sizeof(m_szCompoundScript[nType]));
		nFound++;
	}
	return (nFound > 0);
}

//---------------------------------------------------------------------------
// settings\item\itemvaluescript.txt
//   ITEM_NAME ITEM_QUALITY ITEM_GENRE ITEM_DETAILTYPE ITEM_PARTICULAR
//   ITEM_LEVEL ITEM_SERIES ITEMVALUE ITEMVALUE_SCRIPT REMARK
// O trong o cac cot dieu kien = khop bat ky.
//---------------------------------------------------------------------------
BOOL KItemCompound::LoadItemValueScript()
{
	KTabFile Tab;
	if (!Tab.Load((LPSTR)ITEMVALUE_SCRIPT_FILE))
	{
		printf("[LOREN] khong nap duoc %s\n", ITEMVALUE_SCRIPT_FILE);
		return FALSE;
	}
	int nRows = Tab.GetHeight();
	for (int r = 2; r <= nRows; r++)
	{
		KItemValueRule Rule;
		memset(&Rule, 0, sizeof(Rule));
		Rule.nQuality		= sCellToRule(&Tab, r, "ITEM_QUALITY");
		Rule.nGenre			= sCellToRule(&Tab, r, "ITEM_GENRE");
		Rule.nDetailType	= sCellToRule(&Tab, r, "ITEM_DETAILTYPE");
		Rule.nParticular	= sCellToRule(&Tab, r, "ITEM_PARTICULAR");
		Rule.nLevel			= sCellToRule(&Tab, r, "ITEM_LEVEL");
		Rule.nSeries		= sCellToRule(&Tab, r, "ITEM_SERIES");

		char szVal[64] = "";
		Tab.GetString(r, (LPSTR)"ITEMVALUE", (LPSTR)"", szVal, sizeof(szVal) - 1);
		sTrim(szVal);
		Rule.bHasFixedValue = (szVal[0] != 0);
		Rule.dFixedValue = Rule.bHasFixedValue ? atof(szVal) : 0.0;

		Tab.GetString(r, (LPSTR)"ITEMVALUE_SCRIPT", (LPSTR)"",
					  Rule.szScript, sizeof(Rule.szScript) - 1);
		sTrim(Rule.szScript);

		// GIU MOI DONG, dung thu tu tep (bo nhi phan Linux 0x081538AD chi
		// push_back tuan tu, khong loc). Loc bot o day se lam sai luat "so
		// cung khop CUOI CUNG thang" o duoi.
		m_vRules.push_back(Rule);
	}
	return (!m_vRules.empty());
}

//---------------------------------------------------------------------------
// Mot dong co khop mot mo ta vat pham khong.
// Phep thu cua ban Linux: cot AM = bat ky (0x081539A1..0x081539D1 test/js).
//---------------------------------------------------------------------------
BOOL KItemCompound::KhopLuat(const KItemValueRule& R, int nQuality, int nGenre,
							 int nDetailType, int nParticular, int nLevel, int nSeries)
{
	if (R.nQuality    >= 0 && R.nQuality    != nQuality)		return FALSE;
	if (R.nGenre      >= 0 && R.nGenre      != nGenre)			return FALSE;
	if (R.nDetailType >= 0 && R.nDetailType != nDetailType)		return FALSE;
	if (R.nParticular >= 0 && R.nParticular != nParticular)		return FALSE;
	if (R.nLevel      >= 0 && R.nLevel      != nLevel)			return FALSE;
	if (R.nSeries     >= 0 && R.nSeries     != nSeries)			return FALSE;
	return TRUE;
}

const char* KItemCompound::GetCompoundScript(int nCompoundType) const
{
	if (nCompoundType < 0 || nCompoundType >= COMPOUND_TYPE_COUNT)
		return NULL;
	if (m_szCompoundScript[nCompoundType][0] == 0)
		return NULL;
	return m_szCompoundScript[nCompoundType];
}

//---------------------------------------------------------------------------
// Day mot mang so nguyen thanh bang Lua {1..n} tren ngan xep cua state.
//---------------------------------------------------------------------------
static void sPushIntArray(Lua_State* L, const int* pn, int nCount)
{
	Lua_NewTable(L);
	for (int i = 0; i < nCount; i++)
	{
		Lua_PushNumber(L, (double)pn[i]);
		Lua_RawSetI(L, -2, i + 1);
	}
}

// Doc bang Lua {1..nMax} ra mang C. Tra ve so phan tu doc duoc.
static int sReadIntArray(Lua_State* L, int nStackIdx, int* pnOut, int nMax)
{
	int nCount = 0;
	if (!Lua_IsTable(L, nStackIdx))
		return 0;
	for (int i = 0; i < nMax; i++)
	{
		Lua_RawGetI(L, nStackIdx, i + 1);
		if (Lua_IsNumber(L, -1))
		{
			pnOut[i] = (int)Lua_ValueToNumber(L, -1);
			nCount = i + 1;
		}
		else
			pnOut[i] = 0;
		Lua_Pop(L, 1);
	}
	return nCount;
}

// Doc bang Lua {1..n} DAY DAC ra mang C: dung ngay o phan tu dau khong phai
// so. Khac sReadIntArray o cho no tra ve SO PHAN TU THAT, dung cho danh sach
// nguyen lieu do kich ban gom lai (do dai khong biet truoc).
static int sReadIntArrayDem(Lua_State* L, int nStackIdx, int* pnOut, int nMax)
{
	int nCount = 0;
	if (!Lua_IsTable(L, nStackIdx))
		return 0;
	for (int i = 0; i < nMax; i++)
	{
		Lua_RawGetI(L, nStackIdx, i + 1);
		if (!Lua_IsNumber(L, -1))
		{
			Lua_Pop(L, 1);
			break;
		}
		pnOut[nCount++] = (int)Lua_ValueToNumber(L, -1);
		Lua_Pop(L, 1);
	}
	return nCount;
}

//---------------------------------------------------------------------------
// Bom ba bien toan cuc chuan cua JX1 vao mot state truoc khi goi ham.
// Giong het KPlayer::ExecuteScript - thieu buoc nay thi Say/Pay/GetName
// trong kich ban se khong biet minh dang phuc vu ai.
//---------------------------------------------------------------------------
static void sSetPlayerGlobals(KLuaScript* pScript, int nPlayerIdx)
{
	if (!pScript || !pScript->m_LuaState)
		return;
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return;
	KPlayer* pPlayer = &Player[nPlayerIdx];

	Lua_PushNumber(pScript->m_LuaState, nPlayerIdx);
	pScript->SetGlobalName((LPSTR)SCRIPT_PLAYERINDEX);

	Lua_PushNumber(pScript->m_LuaState, (double)pPlayer->m_dwID);
	pScript->SetGlobalName((LPSTR)SCRIPT_PLAYERID);

	int nSubWorld = 0;
	if (pPlayer->m_nIndex > 0 && pPlayer->m_nIndex < MAX_NPC)
		nSubWorld = Npc[pPlayer->m_nIndex].m_SubWorldIndex;
	Lua_PushNumber(pScript->m_LuaState, nSubWorld);
	pScript->SetGlobalName((LPSTR)SCRIPT_SUBWORLDINDEX);
}

//---------------------------------------------------------------------------
// ITEM_CalcItemValue dang 10 doi.
// Dich nguoc tu compound_header.lua : funitem_calcItemValue()
//     ITEM_CalcItemValue( ver, quality, genre, detail, particular, level,
//                         series, luck, arynMagLvl, aryMagic )
// C++ tra bang itemvaluescript.txt roi:
//     - co cot ITEMVALUE  -> tra thang so do
//     - co ITEMVALUE_SCRIPT -> goi CalcItemValue(...) trong kich ban do
//---------------------------------------------------------------------------
double KItemCompound::CalcItemValueByInfo(int nVer, int nQuality, int nGenre, int nDetailType,
										  int nParticular, int nLevel, int nSeries, int nLuck,
										  const int* pnMagLvl, int nMagLvlCount,
										  const int* pnMagic, int nMagicCount,
										  const char* szParam)
{
	if (!m_bReady)
		return 0.0;

	// QUY TAC QUET (dich nguoc tu bo nhi phan Linux):
	//   - dong co ITEMVALUE (so cung)      : ghi de ket qua roi QUET TIEP
	//                                        (0x081539F5) => so cung khop
	//                                        CUOI CUNG thang.
	//   - dong co ITEMVALUE_SCRIPT         : goi kich ban va TRA VE NGAY
	//                                        (0x08153C18) => kich ban khop
	//                                        DAU TIEN thang tuyet doi.
	double dValue = 0.0;
	const KItemValueRule* pScriptRule = NULL;

	for (size_t i = 0; i < m_vRules.size(); i++)
	{
		const KItemValueRule& R = m_vRules[i];
		if (!KhopLuat(R, nQuality, nGenre, nDetailType, nParticular, nLevel, nSeries))
			continue;
		if (R.szScript[0])
		{
			pScriptRule = &R;
			break;
		}
		if (R.bHasFixedValue)
			dValue = R.dFixedValue;
	}

	if (!pScriptRule)
		return dValue;

	// Chan de quy: kich ban gia tri co the goi lai ITEM_CalcItemValue.
	if (m_nCalcDepth >= COMPOUND_MAX_DEPTH)
	{
		printf("[LOREN] CalcItemValue: de quy qua %d tang tai %s\n",
			   COMPOUND_MAX_DEPTH, pScriptRule->szScript);
		return 0.0;
	}

	KLuaScript* pScript = (KLuaScript*)g_GetScript(pScriptRule->szScript);
	if (!pScript || !pScript->m_LuaState)
	{
		printf("[LOREN] CalcItemValue: khong tim thay kich ban %s\n",
			   pScriptRule->szScript);
		return 0.0;
	}

	Lua_State* L = pScript->m_LuaState;
	dValue = 0.0;

	m_nCalcDepth++;
	int nTopIndex = 0;
	pScript->SafeCallBegin(&nTopIndex);

	// Ban Linux tach theo nGenre (0x08153A6D): CHI trang bi (genre 0) moi
	// duoc nhan hai bang; vat pham khac nhan hai nil. Giu dung the de kich
	// ban gia tri khong hieu nham "co mang rong" thay vi "khong co mang".
	BOOL bCoMang = (nGenre == item_equip);

	// Dung bang TRUOC khi CallFunction day ham len ngan xep, roi truyen bang
	// chi so TUYET DOI qua dinh dang 'v' (bo ky tu dinh dang cua JX1 la
	// n/d/s/N/v/f/p - KHONG co 't' nhu ban Linux).
	int nIdxMagLvl = 0;
	int nIdxMagic = 0;
	if (bCoMang)
	{
		if (pnMagLvl && nMagLvlCount > 0)
			sPushIntArray(L, pnMagLvl, nMagLvlCount);
		else
			Lua_PushNil(L);
		nIdxMagLvl = Lua_GetTopIndex(L);

		if (pnMagic && nMagicCount > 0)
			sPushIntArray(L, pnMagic, nMagicCount);
		else
			Lua_PushNil(L);
		nIdxMagic = Lua_GetTopIndex(L);
	}

	BOOL bGoi;
	if (bCoMang)
		bGoi = pScript->CallFunction((LPSTR)"CalcItemValue", 1, (LPSTR)"ddddddddvvs",
									 nVer, nQuality, nGenre, nDetailType, nParticular,
									 nLevel, nSeries, nLuck,
									 nIdxMagLvl, nIdxMagic,
									 szParam ? szParam : "");
	else
		bGoi = pScript->CallFunction((LPSTR)"CalcItemValue", 1, (LPSTR)"ddddddddNNs",
									 nVer, nQuality, nGenre, nDetailType, nParticular,
									 nLevel, nSeries, nLuck,
									 szParam ? szParam : "");

	if (bGoi)
		pScript->GetValuesFromStack((char*)"n", &dValue);

	pScript->SafeCallEnd(nTopIndex);
	m_nCalcDepth--;

	// KHONG kep am: funitem_calcItemValue cua atlas.lua tra -1 de bao "khong
	// che tao duoc mon nay"; kep ve 0 la nuot mat tin hieu do.
	return dValue;
}

//---------------------------------------------------------------------------
// ITEM_CalcItemValue dang 2 doi: doc thang tu vat pham co that.
// aryMagic la mang PHANG 24 phan tu (6 thuoc tinh x 4 so: id, v0, v1, v2) -
// dung khuon ma equip_normal.lua doc:  for i = 1, 6*4, 4 do ... aryMagic[i]
//---------------------------------------------------------------------------
double KItemCompound::CalcItemValueByIndex(int nItemIdx, const char* szParam)
{
	// Ban Linux tra -1 (0x08153D81 fld1/fchs) khi chi so vat pham sai, KHONG
	// tra 0 - de kich ban phan biet "khong tinh duoc" voi "gia tri bang 0".
	if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
		return -1.0;

	KItem* pItem = &Item[nItemIdx];

	int nMagLvl[6];
	for (int i = 0; i < 6; i++)
		nMagLvl[i] = (i < MAX_ITEM_MAGICLEVEL) ? pItem->m_GeneratorParam.nGeneratorLevel[i] : 0;

	int nMagic[24];
	memset(nMagic, 0, sizeof(nMagic));
	for (int k = 0; k < 6 && k < MAX_ITEM_MAGICATTRIB; k++)
	{
		nMagic[k * 4 + 0] = pItem->m_aryMagicAttrib[k].nAttribType;
		nMagic[k * 4 + 1] = pItem->m_aryMagicAttrib[k].nValue[0];
		nMagic[k * 4 + 2] = pItem->m_aryMagicAttrib[k].nValue[1];
		nMagic[k * 4 + 3] = pItem->m_aryMagicAttrib[k].nValue[2];
	}

	double dValue = CalcItemValueByInfo(
		pItem->m_GeneratorParam.nVersion,
		g_QualityNatureToLinux(pItem->m_CommonAttrib.nItemNature),
		pItem->GetGenre(),
		pItem->GetDetailType(),
		pItem->GetParticular(),
		pItem->GetLevel(),
		pItem->GetSeries(),
		pItem->m_GeneratorParam.nLuck,
		nMagLvl, 6,
		nMagic, 24,
		szParam);

	// NHAN THEO SO LUONG CHONG (Linux 0x08153F21-0x08153F6A):
	//   khong xep chong          -> n = 1
	//   co xep chong             -> m = (max > 0 ? max : 1);
	//                               n = (cur <= m ? cur : 1)
	// Day la ly do vi sao removeItems() cua ban goc phai xoa CA CHONG: nguoi
	// choi tra gia bang ca chong thi cung phai mat ca chong.
	// Kep GIONG HET GetItemStackCount (KJx2WarInfra.cpp:311-316), vi do
	// la con so ma compound_header.lua dung de xoa nguyen lieu. Hai ben
	// lech nhau la nguoi choi mat nhieu hon phan duoc tinh.
	int nSoLuong = pItem->GetStackNum();
	if (nSoLuong < 1)
		nSoLuong = 1;
	{
		int nMax = pItem->GetMaxStackNum();
		if (nMax >= 1 && nSoLuong > nMax)
			nSoLuong = nMax;
	}
	return dValue * nSoLuong;
}

//---------------------------------------------------------------------------
// Duong vao chinh cua he lo ren.
// Goi ham Lua:  Compound( arynNecessaryItemIdx, arynAlternativeItemIdx, bPreview )
// Nhan ve:      nResultDesItemIdx, nResult
//---------------------------------------------------------------------------
int KItemCompound::Compound(int nPlayerIdx, int nCompoundType,
							const int* pnNec, int nNecCount,
							const int* pnAlt, int nAltCount,
							BOOL bPreview, int* pnResultItemIdx)
{
	if (pnResultItemIdx)
		*pnResultItemIdx = -1;

	if (!m_bReady)
		return FOUNDRY_RESULT_UNKNOWN;
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
		return FOUNDRY_RESULT_UNKNOWN;
	if (Player[nPlayerIdx].m_nIndex <= 0 || Player[nPlayerIdx].m_nIndex >= MAX_NPC)
		return FOUNDRY_RESULT_UNKNOWN;

	const char* szScript = GetCompoundScript(nCompoundType);
	if (!szScript)
	{
		printf("[LOREN] Compound: COMPOUND_TYPE=%d chua co kich ban\n", nCompoundType);
		return FOUNDRY_RESULT_UNKNOWN;
	}

	KLuaScript* pScript = (KLuaScript*)g_GetScript(szScript);
	if (!pScript || !pScript->m_LuaState)
	{
		printf("[LOREN] Compound: khong tim thay kich ban %s\n", szScript);
		return FOUNDRY_RESULT_UNKNOWN;
	}

	if (nNecCount < 0) nNecCount = 0;
	if (nAltCount < 0) nAltCount = 0;
	if (nNecCount > COMPOUND_MAX_MATERIAL) nNecCount = COMPOUND_MAX_MATERIAL;
	if (nAltCount > COMPOUND_MAX_MATERIAL) nAltCount = COMPOUND_MAX_MATERIAL;

	//-----------------------------------------------------------------------
	// LOC CHI SO VAT PHAM - phai lam TRUOC khi dua xuong kich ban.
	//
	// Kich ban chi nhan mot mang so nguyen roi tu tinh gia tri bang mang TOAN
	// CUC Item[]. Neu khong loc thi co hai duong an gian ngay khi noi goi tin:
	//   (a) gui chi so vat pham CUA NGUOI KHAC - gia tri duoc cong vao nhung
	//       RemoveItemIdx chi duyet tui cua chinh minh nen xoa hut IM LANG.
	//   (b) gui CUNG MOT chi so hai lan - gia tri cong doi, lan xoa thu hai
	//       khong con gi de xoa.
	// Hai chot: phai nam trong tui cua chinh nguoi choi, va khong trung nhau.
	//-----------------------------------------------------------------------
	int anNec[COMPOUND_MAX_MATERIAL];
	int anAlt[COMPOUND_MAX_MATERIAL];
	int nNec = 0;
	int nAlt = 0;
	{
		KItemList* pList = &Player[nPlayerIdx].m_ItemList;
		int anDaCo[COMPOUND_MAX_MATERIAL * 2];
		int nDaCo = 0;
		for (int nVong = 0; nVong < 2; nVong++)
		{
			const int* pnNguon = nVong ? pnAlt : pnNec;
			int nSo = nVong ? nAltCount : nNecCount;
			int* pnDich = nVong ? anAlt : anNec;
			int* pnDem = nVong ? &nAlt : &nNec;
			for (int k = 0; k < nSo; k++)
			{
				int nIdx = pnNguon ? pnNguon[k] : 0;
				if (nIdx <= 0 || nIdx >= MAX_ITEM)
					continue;
				if (pList->FindSame(nIdx) == 0)		// khong phai do cua minh
					continue;
				// [UILOREN 27/08] SKIP LOCKED ITEMS.
				// The consume path (removeItems -> RemoveItemByIndex ->
				// KItemList::RemoveItemIdx) never reads the lock flag, so a locked
				// item dropped into a forge slot would be destroyed for good.
				// Same guard the trade / drop paths use: KBuySell.cpp:366-368 and
				// CoreShell.cpp:3659-3660. The -2 / -3 literals are
				// LOCK_STATE_FOREVER / LOCK_STATE_CHARACTER (GameDataDef.h:292-293),
				// written as numbers the way every other call site here does.
				// Dropping the item from the list (instead of failing outright) is
				// the safe direction: a required material that vanishes makes
				// g_FoundryResDemand::Check refuse the craft, and nothing is eaten.
				if (Item[nIdx].GetPlayerItemLock() > 0 ||
					Item[nIdx].GetPlayerItemHLock() > 0 ||
					Item[nIdx].GetPlayerItemLock() == -2 ||
					Item[nIdx].GetPlayerItemLock() == -3)
					continue;
				int t = 0;
				for (t = 0; t < nDaCo; t++)
					if (anDaCo[t] == nIdx)
						break;
				if (t < nDaCo)						// da gui chi so nay roi
					continue;
				anDaCo[nDaCo++] = nIdx;
				pnDich[(*pnDem)++] = nIdx;
			}
		}
	}
	pnNec = anNec;
	nNecCount = nNec;
	pnAlt = anAlt;
	nAltCount = nAlt;

	//-----------------------------------------------------------------------
	// CHOT DIEU KIEN NGUYEN LIEU - phai chay TRUOC khi goi kich ban.
	//
	// Kich ban Lua cua ban goc CO Y khong kiem nguyen lieu: xuanjing_compound
	// chi co "function verifySrcItems() return RESULT_SUCCEED end", equip_compound
	// chi lay mon dau tien co nGenre == 0. Toan bo viec loc nam o day.
	// Bo qua doan nay thi he lo ren nhan bat ky vat pham gi lam nguyen lieu.
	//
	// Chi kiem NGUYEN LIEU BAT BUOC. Nguyen lieu TU CHON (o "tang xac suat")
	// ban goc khong kiem o may chu: bon khoa …_EnhanceItem / CompoundGold_Material
	// khong mot ham kiem nao cua may chu dung toi - chung la bo loc cua client.
	//-----------------------------------------------------------------------
	if (g_FoundryResDemand.IsReady())
	{
		int nCheck = g_FoundryResDemand.Check(nCompoundType, pnNec, nNecCount);
		if (nCheck != FOUNDRY_RESULT_SUCCEED)
			return nCheck;
		nCheck = g_FoundryResDemand.CheckTuChon(nCompoundType, pnAlt, nAltCount);
		if (nCheck != FOUNDRY_RESULT_SUCCEED)
			return nCheck;
	}
	else
	{
		// Chua nap duoc ini thi TU CHOI, khong chay tiep. Chay tiep dong nghia
		// voi mo cua cho moi vat pham vao lo ren.
		printf("[LOREN] Compound: chua nap duoc foundryresdemand.ini, tu choi\n");
		return FOUNDRY_RESULT_UNKNOWN;
	}

	Lua_State* L = pScript->m_LuaState;

	sSetPlayerGlobals(pScript, nPlayerIdx);

	int nResultIdx = -1;
	int nResult = FOUNDRY_RESULT_UNKNOWN;

	int nTopIndex = 0;
	pScript->SafeCallBegin(&nTopIndex);

	sPushIntArray(L, pnNec, nNecCount);
	int nIdxNec = Lua_GetTopIndex(L);
	sPushIntArray(L, pnAlt, nAltCount);
	int nIdxAlt = Lua_GetTopIndex(L);

	if (pScript->CallFunction((LPSTR)"Compound", 2, (LPSTR)"vvd",
							  nIdxNec, nIdxAlt, (int)(bPreview ? 1 : 0)))
	{
		pScript->GetValuesFromStack((char*)"dd", &nResultIdx, &nResult);
	}

	pScript->SafeCallEnd(nTopIndex);

	if (pnResultItemIdx)
		*pnResultItemIdx = nResultIdx;
	return nResult;
}

//===========================================================================
//  CAC HAM LUA MOI - dang ky trong ScriptFuns.cpp
//===========================================================================

//---------------------------------------------------------------------------
// ITEM_CalcItemValue - hai dang goi:
//   (nItemIdx, strParam)
//   (nVer, nQuality, nGenre, nDetail, nParticular, nLevel, nSeries, nLuck,
//    arynMagLvl, aryMagic [, strParam])
//---------------------------------------------------------------------------
int LuaCmp_ITEM_CalcItemValue(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	double dValue = 0.0;

	if (nParamNum <= 2)
	{
		if (Lua_IsNumber(L, 1))
		{
			int nItemIdx = (int)Lua_ValueToNumber(L, 1);
			const char* szParam = (nParamNum >= 2 && Lua_IsString(L, 2))
								  ? Lua_ValueToString(L, 2) : "";
			dValue = g_ItemCompound.CalcItemValueByIndex(nItemIdx, szParam);
		}
	}
	else if (nParamNum >= 8)
	{
		int nVer		= (int)Lua_ValueToNumber(L, 1);
		int nQuality	= (int)Lua_ValueToNumber(L, 2);
		int nGenre		= (int)Lua_ValueToNumber(L, 3);
		int nDetailType	= (int)Lua_ValueToNumber(L, 4);
		int nParticular	= (int)Lua_ValueToNumber(L, 5);
		int nLevel		= (int)Lua_ValueToNumber(L, 6);
		int nSeries		= (int)Lua_ValueToNumber(L, 7);
		int nLuck		= (int)Lua_ValueToNumber(L, 8);

		int nMagLvl[6];
		int nMagic[24];
		memset(nMagLvl, 0, sizeof(nMagLvl));
		memset(nMagic, 0, sizeof(nMagic));
		int nMagLvlCount = 0;
		int nMagicCount = 0;

		if (nParamNum >= 9 && Lua_IsTable(L, 9))
			nMagLvlCount = sReadIntArray(L, 9, nMagLvl, 6) ? 6 : 0;
		if (nParamNum >= 10 && Lua_IsTable(L, 10))
			nMagicCount = sReadIntArray(L, 10, nMagic, 24) ? 24 : 0;

		const char* szParam = (nParamNum >= 11 && Lua_IsString(L, 11))
							  ? Lua_ValueToString(L, 11) : "";

		dValue = g_ItemCompound.CalcItemValueByInfo(
			nVer, nQuality, nGenre, nDetailType, nParticular, nLevel, nSeries, nLuck,
			nMagLvlCount ? nMagLvl : NULL, nMagLvlCount,
			nMagicCount ? nMagic : NULL, nMagicCount,
			szParam);
	}

	Lua_PushNumber(L, dValue);
	return 1;
}

//---------------------------------------------------------------------------
// FoundryCompound( nCompoundType, bangNguyenLieuChinh, bangNguyenLieuPhu,
//                  bPreview )
//   -> chi so vat pham sinh ra, ma ket qua (FOUNDRY_RESULT_TYPE)
//
// Day la cua vao cua he lo ren tu phia kich ban. Chinh no goi
// KItemCompound::Compound, tuc goi ham Compound() cua kich ban tuong ung
// trong bang compoundscript.txt.
//
// Hai bang truyen vao la mang chi so vat pham {1..n}. Kich ban NPC gom chung
// tu hop giao vat pham (GiveItemUI + GetGiveItemUnit) - kenh nay JX1 da co
// san va dang chay that, nen khong phai dung them giao thuc rieng.
// Chi so trung lap va do khong phai cua nguoi choi deu bi loc o Compound().
//---------------------------------------------------------------------------
int LuaCmp_FoundryCompound(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	int nParamNum = Lua_GetTopIndex(L);
	if (nPlayerIndex <= 0 || nParamNum < 1 || !Lua_IsNumber(L, 1))
	{
		Lua_PushNumber(L, -1);
		Lua_PushNumber(L, FOUNDRY_RESULT_UNKNOWN);
		return 2;
	}

	int nType = (int)Lua_ValueToNumber(L, 1);

	int anNec[COMPOUND_MAX_MATERIAL];
	int anAlt[COMPOUND_MAX_MATERIAL];
	memset(anNec, 0, sizeof(anNec));
	memset(anAlt, 0, sizeof(anAlt));
	int nNecCount = 0;
	int nAltCount = 0;

	if (nParamNum >= 2 && Lua_IsTable(L, 2))
		nNecCount = sReadIntArrayDem(L, 2, anNec, COMPOUND_MAX_MATERIAL);
	if (nParamNum >= 3 && Lua_IsTable(L, 3))
		nAltCount = sReadIntArrayDem(L, 3, anAlt, COMPOUND_MAX_MATERIAL);

	BOOL bPreview = FALSE;
	if (nParamNum >= 4 && Lua_IsNumber(L, 4))
		bPreview = ((int)Lua_ValueToNumber(L, 4) != 0);

	int nResultIdx = -1;
	int nResult = g_ItemCompound.Compound(nPlayerIndex, nType,
										  anNec, nNecCount,
										  anAlt, nAltCount,
										  bPreview, &nResultIdx);

	Lua_PushNumber(L, nResultIdx);
	Lua_PushNumber(L, nResult);
	return 2;
}

//---------------------------------------------------------------------------
// ITEM_GetItemVersion(nItemIdx) -> nVersion cua vat pham
//---------------------------------------------------------------------------
int LuaCmp_ITEM_GetItemVersion(Lua_State* L)
{
	int nVer = 0;
	if (Lua_GetTopIndex(L) >= 1 && Lua_IsNumber(L, 1))
	{
		int nItemIdx = (int)Lua_ValueToNumber(L, 1);
		if (nItemIdx > 0 && nItemIdx < MAX_ITEM)
			nVer = Item[nItemIdx].m_GeneratorParam.nVersion;
	}
	Lua_PushNumber(L, nVer);
	return 1;
}

//---------------------------------------------------------------------------
// ITEM_GetLatestItemVersion() -> phien ban vat pham moi nhat
//---------------------------------------------------------------------------
int LuaCmp_ITEM_GetLatestItemVersion(Lua_State* L)
{
	Lua_PushNumber(L, g_GetLatestItemVersion());
	return 1;
}

//---------------------------------------------------------------------------
// AddItemEx( nItemVer, nRandSeed, nQuality, nGenre, nDetailType, nParticular,
//            nLevel, nSeries, nLuck, nMagLvl1..nMagLvl6 )
// Dich nguoc tu compound_header.lua : addItemByInfo().
// Tra ve chi so vat pham moi, 0 neu that bai.
//
// nQuality theo QUY UOC LINUX - doi sang nItemNature cua JX1 truoc khi sinh.
// nMagLvl = -1 nghia la O KHAM NAM RONG (giu khe, chua co thuoc tinh).
//---------------------------------------------------------------------------
int LuaCmp_AddItemEx(Lua_State* L)
{
	int nPlayerIndex = GetPlayerIndex(L);
	if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 9)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int		nItemVer	= (int)Lua_ValueToNumber(L, 1);
	UINT	uRandSeed	= (UINT)Lua_ValueToNumber(L, 2);
	int		nQuality	= (int)Lua_ValueToNumber(L, 3);
	int		nGenre		= (int)Lua_ValueToNumber(L, 4);
	int		nDetailType	= (int)Lua_ValueToNumber(L, 5);
	int		nParticular	= (int)Lua_ValueToNumber(L, 6);
	int		nLevel		= (int)Lua_ValueToNumber(L, 7);
	int		nSeries		= (int)Lua_ValueToNumber(L, 8);
	int		nLuck		= (int)Lua_ValueToNumber(L, 9);

	int nMagicLevel[MAX_ITEM_MAGICLEVEL];
	memset(nMagicLevel, 0, sizeof(nMagicLevel));
	for (int i = 0; i < 6; i++)
	{
		int nArg = 10 + i;
		if (nParamNum >= nArg && Lua_IsNumber(L, nArg))
		{
			int nLv = (int)Lua_ValueToNumber(L, nArg);
			// chi ba ma hop le: > 0 cap thuoc tinh, -1 o kham nam rong, 0 het
			if (nLv < -1)
				nLv = -1;
			nMagicLevel[i] = nLv;
		}
	}

	//-----------------------------------------------------------------------
	// KEP BIEN. AddItemEx duoc dang ky TOAN CUC nen moi kich ban deu goi duoc;
	// mot o sai trong bang du lieu la duc bay ra do khong ton tai.
	// AddItemSet2 nem _ASSERT(0) khi gap nGenre la, con Gen_* thi doc thang
	// bang tra theo chi so - sai la doc ngoai bang.
	//-----------------------------------------------------------------------
	if (nGenre < item_equip || nGenre > item_magicscript)
	{
		printf("[LOREN] AddItemEx: nGenre=%d ngoai dai 0..%d, tu choi\n",
			   nGenre, (int)item_magicscript);
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (nQuality != ITEMQUALITY_NORMAL && nQuality != ITEMQUALITY_GOLD &&
		nQuality != ITEMQUALITY_ENCHASABLE && nQuality != ITEMQUALITY_PLATINA)
	{
		printf("[LOREN] AddItemEx: nQuality=%d khong hop le, tu choi\n", nQuality);
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (nDetailType < 0 || nParticular < 0 || nLevel < 0)
	{
		printf("[LOREN] AddItemEx: ma am (%d/%d/%d), tu choi\n",
			   nDetailType, nParticular, nLevel);
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nNature = g_QualityLinuxToNature(nQuality);

	int nIndex = 0;
	if (nGenre == item_equip)
	{
		nIndex = ItemSet.Add(nNature, nGenre, nSeries, nLevel, nLuck,
							 nDetailType, nParticular, nMagicLevel,
							 nItemVer, uRandSeed,
							 ItemSet.genXOpt(nLuck));
	}
	else
	{
		nIndex = ItemSet.AddItemSet2(nGenre, nSeries, nLevel, nLuck,
									 nDetailType, nParticular, nMagicLevel,
									 nItemVer, uRandSeed, 1, 0, 0,
									 0, 0, 0, 0, 0, 0,
									 ItemSet.genXOpt(nLuck));
	}

	if (nIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	//-----------------------------------------------------------------------
	// KHOI PHUC THAM SO SINH.
	//
	// Ca hai duong tao vat pham cua JX1 deu XOA hoac GHI DE tham so sinh vua
	// dat, nen phai dat lai o day - neu khong thi hai co che cot loi cua ban
	// Linux khong the chay:
	//
	//  (1) Gen_Equipment nhanh mac dinh (KItemGenerator.cpp) tu lay hat ngau
	//      nhien moi + phien ban may chu -> mat uRandSeed/nItemVer ma kich ban
	//      truyen vao. Ma "ep do tim" truyen lai hat cua chinh mon do goc de
	//      mon duc ra GIU NGUYEN chi so ngau nhien. Chi Gen_ExistEquipment
	//      ton trong hat, nen phai goi lai no.
	//
	//  (2) Gen_MagicScript / Gen_Quest deu ZeroMemory ca khoi m_GeneratorParam
	//      va khong bao gio ghi nGeneratorLevel[]. Ma phep cua khoang thach
	//      (atlas.lua doc bang GetItemParam(idx,1) = nGeneratorLevel[0]) nam
	//      dung o do -> khong dat lai la 100% cong thuc Do pho truot.
	//-----------------------------------------------------------------------
	KItem* pNew = &Item[nIndex];
	pNew->m_GeneratorParam.nVersion = nItemVer;
	pNew->m_GeneratorParam.uRandomSeed = uRandSeed;
	pNew->m_GeneratorParam.nLuck = nLuck;
	memcpy(pNew->m_GeneratorParam.nGeneratorLevel, nMagicLevel,
		   sizeof(int) * MAX_ITEM_MAGICLEVEL);

	// [LOREN] Khoang thuoc tinh: JX1 va ban Linux canh HAI CUA khac nhau
	// cho cung mot thu. JX1 doc ma phep o m_GeneratorParam.nLuck
	// (KItem.cpp:2372, de ve dong "Thuoc tinh:" trong chu giai), con kich
	// ban lo ren doc GetItemParam(idx,1) = nGeneratorLevel[0].
	// Ghi vao ca hai thi ca hai phia deu dung, khoi phai chon ben.
	if (nGenre == item_magicscript && nDetailType == 1 &&
		nParticular >= 199 && nParticular <= 204 && nMagicLevel[0] > 0)
	{
		pNew->m_GeneratorParam.nLuck = nMagicLevel[0];
	}

	if (nGenre == item_equip && uRandSeed != 0)
	{
		// Hat khac 0 = "tai sinh dung mon nay". Ban Linux cung re nhanh theo
		// dung dieu kien nay (0x0811F46C test eax,eax / 0x0811F50A jne).
		ItemGen.Gen_ExistEquipment(nNature, nDetailType, nParticular, nSeries,
								   nLevel, nMagicLevel, nLuck, nItemVer, pNew);
		pNew->m_GeneratorParam.nVersion = nItemVer;
		pNew->m_GeneratorParam.uRandomSeed = uRandSeed;
	}

	// Dat vao hanh trang; het cho thi tha xuong dat nhu LuaAddItem.
	int x = 0, y = 0;
	if (Player[nPlayerIndex].m_ItemList.CheckCanPlaceInEquipment(
			Item[nIndex].GetWidth(), Item[nIndex].GetHeight(), &x, &y))
	{
		Player[nPlayerIndex].m_ItemList.AddKIL(nIndex, pos_equiproom, x, y, false, true);
	}
	else
	{
		int nIdx = Player[nPlayerIndex].m_ItemList.Hand();
		if (nIdx)
		{
			Player[nPlayerIndex].m_ItemList.Remove(nIdx);

			KMapPos sMapPos;
			KObjItemInfo sInfo;
			memset(&sInfo, 0, sizeof(sInfo));
			Player[nPlayerIndex].GetAboutPos(&sMapPos);

			sInfo.m_nItemID			= nIdx;
			sInfo.m_nItemWidth		= Item[nIdx].GetWidth();
			sInfo.m_nItemHeight		= Item[nIdx].GetHeight();
			sInfo.m_nMoneyNum		= 0;
			char szNameTemp[OBJ_NAME_LENGHT];
			if (Item[nIdx].GetGenre() != item_equip && Item[nIdx].GetStackNum() > 1)
			{
				_snprintf(szNameTemp, sizeof(szNameTemp) - 1, "%s x %d",
						  Item[nIdx].GetName(), Item[nIdx].GetStackNum());
				szNameTemp[sizeof(szNameTemp) - 1] = 0;
				sStrCpyLen(sInfo.m_szName, szNameTemp, sizeof(sInfo.m_szName));
			}
			else
				sStrCpyLen(sInfo.m_szName, Item[nIdx].GetName(), sizeof(sInfo.m_szName));
			sInfo.m_nColorID		= Item[nIdx].GetColorItem();
			sInfo.m_nGenre			= Item[nIdx].GetGenre();
			sInfo.m_nDetailType		= Item[nIdx].GetDetailType();
			sInfo.m_nParticularType	= Item[nIdx].GetParticular();
			sInfo.m_nMovieFlag		= 1;
			sInfo.m_nSoundFlag		= 1;
			sInfo.m_dwNpcId1		= 0;

			int nObj = ObjSet.Add(Item[nIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIdx].GetGenre() == item_task)
					Object[nObj].SetEntireBelong(nPlayerIndex);
				else
					Object[nObj].SetItemBelong(nPlayerIndex);
			}
		}
		Player[nPlayerIndex].m_ItemList.AddKIL(nIndex, pos_hand, 0, 0);
	}

	Lua_PushNumber(L, nIndex);
	return 1;
}

//---------------------------------------------------------------------------
// TabFile_Search( strTabName, colNameOrIdx, value [, nStartRow] )
// Dich nguoc tu ba cho goi that trong ban Linux:
//   ore.lua:              TabFile_Search( path, "LEVEL", nLevel )
//   equip_gold.lua:       TabFile_Search( path, "INDEX", n, nSearchRow + 1 )
//   magicattriblevel.lua: TabFile_Search( path, 2, nMagicAttrib, 3 )
// Tra ve chi so DONG tim duoc (>= nStartRow), 0 neu khong thay.
// nStartRow mac dinh 2 (dong 1 la tieu de).
//---------------------------------------------------------------------------
// mo trong ScriptFuns.cpp (s_mapTabFiles/sGetTabFileByName la static)
extern KTabFile* g_GetLuaTabFile(Lua_State* L, int nArg);

int LuaCmp_TabFile_Search(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	KTabFile* pTab = g_GetLuaTabFile(L, 1);
	if (nParamNum < 3 || !pTab || !pTab->GetHeight())
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	// cot: co the la ten hoac chi so
	int nColumn = 0;
	if (Lua_IsNumber(L, 2))
		nColumn = (int)Lua_ValueToNumber(L, 2);
	else if (Lua_IsString(L, 2))
	{
		char szColumn[64];
		sStrCpyLen(szColumn, Lua_ValueToString(L, 2), sizeof(szColumn));
		nColumn = pTab->FindColumn(szColumn);
	}
	if (nColumn <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	// gia tri can tim: so sanh theo CHUOI da cat khoang trang neu doi la
	// chuoi, theo SO neu doi la so (bang .txt hay ghi "5" va " 5")
	bool bNumeric = (Lua_IsNumber(L, 2 + 1) != 0);
	double dWanted = 0.0;
	char szWanted[128] = "";
	if (bNumeric)
		dWanted = Lua_ValueToNumber(L, 3);
	else
	{
		sStrCpyLen(szWanted, Lua_ValueToString(L, 3), sizeof(szWanted));
		sTrim(szWanted);
	}

	int nStartRow = 2;
	if (nParamNum >= 4 && Lua_IsNumber(L, 4))
		nStartRow = (int)Lua_ValueToNumber(L, 4);
	if (nStartRow < 2)
		nStartRow = 2;

	int nRows = pTab->GetHeight();
	for (int r = nStartRow; r <= nRows; r++)
	{
		char szCell[128] = "";
		pTab->GetString(r, nColumn, (LPSTR)"", szCell, sizeof(szCell) - 1);
		sTrim(szCell);
		if (bNumeric)
		{
			if (szCell[0] == 0)
				continue;
			if (atof(szCell) == dWanted)
			{
				Lua_PushNumber(L, r);
				return 1;
			}
		}
		else
		{
			if (strcmp(szCell, szWanted) == 0)
			{
				Lua_PushNumber(L, r);
				return 1;
			}
		}
	}

	Lua_PushNumber(L, 0);
	return 1;
}

//---------------------------------------------------------------------------
// WriteCompoundLog( strText ) - nhat ky rieng cua he lo ren.
// Ban Linux ghi vao ./Logs/KSG_CompoundLog*.txt; giu dung ten thu muc Logs.
//---------------------------------------------------------------------------
int LuaCmp_WriteCompoundLog(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 1 || !Lua_IsString(L, 1))
		return 0;

	const char* szText = Lua_ValueToString(L, 1);
	if (!szText)
		return 0;

	time_t t = time(0);
	struct tm* pTm = localtime(&t);
	char szFile[256];
	if (pTm)
		_snprintf(szFile, sizeof(szFile) - 1, "Logs\\KSG_CompoundLog_%04d%02d%02d.txt",
				  pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
	else
		sStrCpyLen(szFile, "Logs\\KSG_CompoundLog.txt", sizeof(szFile));
	szFile[sizeof(szFile) - 1] = 0;

	FILE* fp = fopen(szFile, "a+");
	if (!fp)
		return 0;
	fputs(szText, fp);
	// ban goc tu dat \r\n o mot so dong, khong o dong khac - them dau dong
	// moi neu chuoi chua ket thuc bang xuong dong, de tep khong dinh lien.
	int nLen = (int)strlen(szText);
	if (nLen == 0 || szText[nLen - 1] != '\n')
		fputs("\r\n", fp);
	fclose(fp);
	return 0;
}

//---------------------------------------------------------------------------
// Do ben vat pham - boc lai ba ham co san cua KItem (KItem.h:282/289/296).
//   GetMaxDurability( nItemIdx )
//   GetCurDurability( nItemIdx )
//   SetCurDurability( nItemIdx, nValue )
//   SetMaxDurability( nItemIdx, nValue )
//---------------------------------------------------------------------------
static KItem* sGetItemArg(Lua_State* L, int nArg)
{
	if (Lua_GetTopIndex(L) < nArg || !Lua_IsNumber(L, nArg))
		return NULL;
	int nItemIdx = (int)Lua_ValueToNumber(L, nArg);
	if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM)
		return NULL;
	return &Item[nItemIdx];
}

int LuaCmp_GetMaxDurability(Lua_State* L)
{
	KItem* p = sGetItemArg(L, 1);
	Lua_PushNumber(L, p ? p->GetMaxDurability() : 0);
	return 1;
}

int LuaCmp_GetCurDurability(Lua_State* L)
{
	KItem* p = sGetItemArg(L, 1);
	Lua_PushNumber(L, p ? p->GetDurability() : 0);
	return 1;
}

int LuaCmp_SetCurDurability(Lua_State* L)
{
	KItem* p = sGetItemArg(L, 1);
	if (p && Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
	{
		int nValue = (int)Lua_ValueToNumber(L, 2);
		if (nValue < 0)
			nValue = 0;
		p->SetDurability(nValue);
		Lua_PushNumber(L, 1);
		return 1;
	}
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaCmp_SetMaxDurability(Lua_State* L)
{
	KItem* p = sGetItemArg(L, 1);
	if (p && Lua_GetTopIndex(L) >= 2 && Lua_IsNumber(L, 2))
	{
		int nValue = (int)Lua_ValueToNumber(L, 2);
		if (nValue < 0)
			nValue = 0;
		p->SetMaxDurability(nValue);
		Lua_PushNumber(L, 1);
		return 1;
	}
	Lua_PushNumber(L, 0);
	return 1;
}

//---------------------------------------------------------------------------
// GetItemQuality( nItemIdx ) -> pham chat theo QUY UOC LINUX (0/1/2).
//
// Truoc day JX1 tra cung 0 (ban vo cua dot 3 hoat dong, chi dung de ghi log).
// He lo ren SONG bang gia tri nay: equip_compound sinh do pham chat 2, atlas
// sinh do pham chat 1, va bang itemvaluescript.txt tra theo ITEM_QUALITY.
// Nguon that = m_CommonAttrib.nItemNature (co luu xuong CSDL: iequipnaturecode).
//---------------------------------------------------------------------------
int LuaCmp_GetItemQuality(Lua_State* L)
{
	KItem* p = sGetItemArg(L, 1);
	Lua_PushNumber(L, p ? g_QualityNatureToLinux(p->m_CommonAttrib.nItemNature)
						: ITEMQUALITY_NORMAL);
	return 1;
}

//---------------------------------------------------------------------------
// BA HAM DI KEM GetItemQuality.
//
// Truoc day GetItemQuality luon tra 0 nen MOI nhanh "pham chat khac 0" trong
// kich ban dang chay deu la nhanh CHET. Bat GetItemQuality len la nhung nhanh
// do song lai - va chung goi ba ham duoi day, von cung dang la ban vo. De ba
// ham nay tra 0 thi bat GetItemQuality se lam HONG cac cho dang chay:
//     script\lib\composeex.lua:142        ghep do
//     script\lib\log.lua:39               ghi nhat ky
//     script\missions\yandibaozang\yandiduihuan.lua:135   doi thuong Viem De
//     script\event\...\qianqiu_yinglie\head.lua:67
// nen phai lam that ca ba cung luc.
//
// Cho luu: trang bi Hoang Kim va Bach Kim deu giu SO DONG bang tra o
// m_CommonAttrib.nRow (KItemGenerator.cpp:1201/1208 SetRow(nDetailType)).
// Kich ban JX2 dem tu 1 (log.lua:39 lay GetGlodEqIndex() - 1 lam DetailType)
// nen tra ve nRow + 1.
//---------------------------------------------------------------------------
int LuaCmp_GetGlodEqIndex(Lua_State* L)
{
	KItem* p = sGetItemArg(L, 1);
	int nIdx = 0;
	if (p && p->m_CommonAttrib.nItemNature >= NATURE_GOLD)
		nIdx = p->GetRow() + 1;
	Lua_PushNumber(L, nIdx);
	return 1;
}

int LuaCmp_GetPlatinaEquipIndex(Lua_State* L)
{
	KItem* p = sGetItemArg(L, 1);
	int nIdx = 0;
	if (p && p->m_CommonAttrib.nItemNature == NATURE_PLATINA)
		nIdx = p->GetRow() + 1;
	Lua_PushNumber(L, nIdx);
	return 1;
}

int LuaCmp_GetPlatinaLevel(Lua_State* L)
{
	KItem* p = sGetItemArg(L, 1);
	if (p && p->m_CommonAttrib.nItemNature == NATURE_PLATINA)
	{
		// UpgradePlatinaEquip (KItemGenerator.cpp:1930) dung chinh GetLevel()
		// lam cap bach kim, dat lai 0 khi vua nang tu Hoang Kim len.
		Lua_PushNumber(L, p->GetLevel());
		return 1;
	}
	// khong phai bach kim -> nil, dung nhu ban vo cu (kich ban tostring())
	Lua_PushNil(L);
	return 1;
}

//---------------------------------------------------------------------------
// GetItemBindState( nItemIdx ) - doi ung voi SetItemBindState da co san.
//---------------------------------------------------------------------------
int LuaCmp_GetItemBindState(Lua_State* L)
{
	KItem* p = sGetItemArg(L, 1);
	Lua_PushNumber(L, p ? p->GetPlayerItemLock() : 0);
	return 1;
}

#endif	// _SERVER
