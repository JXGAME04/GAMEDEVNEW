// KAuctionServer.cpp - [DAUGIA 04/09] Kho DAU GIA tren MySQL + bo GIU NGUYEN VAT PHAM. Xem KAuctionServer.h.
//
// BANG (tu tao lan dung dau tien, DB = muc [gamedb] cua DataBase.ini - dung cho voi bang `mail`):
//   auction_item
//     id            INT AUTO_INCREMENT
//     atype         TINYINT   1 = bang hoi, 2 = the gioi, 3 = ca nhan (ky gui)      [muc A dung 3]
//     activity      VARBINARY(64)  ten phien (ca nhan de trong)
//     kind          TINYINT   1 = kieu Anh (gia TANG), 2 = kieu Ha Lan (gia GIAM)
//     seller        VARBINARY(32)  ten nguoi ban (byte TCVN3 tho)
//     seller_tong   INT       id bang cua nguoi ban (phien bang hoi)
//     item_name     VARBINARY(64)  ten mon do de hien nhanh
//     item_desc     VARBINARY(64)  "genre,detail,particular,level,series,luck" de loc/hien bieu tuong
//     item_rec      VARCHAR(1024)  BAN GHI HEX cua mon do (giu nguyen thuoc tinh - xem duoi)
//     item_cells    INT       so O hanh trang mon do chiem (kiem cho trong truoc khi giao)
//     currency      TINYINT   1 = Ngan luong, 2 = Xu          [chu chot 04/09]
//     base_price    BIGINT    gia mo ban
//     cur_price     BIGINT    gia hien tai (Ha Lan giam dan / Anh tang dan)
//     guaranteed_price BIGINT gia bao dam (Ha Lan: gia san; Anh: gia mua ngay)
//     buyer         VARBINARY(32)  nguoi mua / nguoi tra gia cao nhat
//     buy_price     BIGINT    gia da chot
//     deposit       BIGINT    phi ky gui da thu cua nguoi ban
//     start_time    INT  end_time INT  next_drop_time INT  drop_left INT
//     state         TINYINT   0 dang ban, 1 da ban (cho giao), 2 het han/luu phach, 3 da xu ly xong
//
// GIU NGUYEN VAT PHAM (quan trong nhat):
//   Hop THU tao lai do bang AddItem -> trang bi bi DOI thuoc tinh ngau nhien. Dau gia KHONG duoc phep the.
//   O day ta luu DUNG bo tham so ma duong luu/nap CSDL cua nhan vat dung (KPlayerDBFuns.cpp:
//   SavePlayerItemList / LoadPlayerItemList) roi tao lai bang CHINH cac ham ItemGen ma duong nap do goi
//   (Gen_ExistEquipment / GetGoldItemByIndex / Gen_Medicine / Gen_Quest / Gen_MagicScript / Gen_Fusion /
//   Gen_StarStone / Gen_TownPortal). Nho vay mon do quay lai y het: cung seed, cung do may man, cung do ben,
//   cung khoa, cung han dung, cung phi phong.
//   NEU KPlayerDBFuns.cpp doi cach nap thi PHAI sua theo o day (hai cho, khong co cach nao khac vi ham nap
//   CSDL nam trong vong lap doc bo dem cua Goddess, khong tach ra duoc ma khong dung den duong dang nhap).
#include "KCore.h"
#include "KWin32.h"
#include "KEngine.h"
#include "KDebug.h"
#include "LuaLib.h"
#include "KAuctionServer.h"

#ifdef _SERVER
#include "KMySQLDB.h"
#include "KItem.h"
#include "KInventory.h"
#include "KItemList.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "GameDataDef.h"
#include "KPlayerDef.h"
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define AUC_REC_VER		1
#define AUC_MAX_ROW		200

//////////////////////////////////////////////////////////////////////
// Ban ghi mot mon do (thu tu KHONG duoc doi - hex da nam trong CSDL)
//////////////////////////////////////////////////////////////////////
struct KAucRec
{
	int nVer;
	int nNature;		// iequipnaturecode
	int nGenre;			// iequipclasscode
	int nDetail;
	int nParticular;
	int nLevel;
	int nSeries;
	int nVersion;		// iequipversion
	int nRandSeed;
	int nParam[MAX_ITEM_MAGICLEVEL];
	int nLuck;
	int nDurability;
	int nGoldId;
	int nStackNum;
	int nEnChance;
	int nPoint;
	int nYear, nMonth, nDay, nHour;
	int nLock;			// ilockbh
	int nHLock;		// igiomokhoa
	int nMantle;		// iiduphong1
	int nUseParam;		// iiduphong2
	int nGlow;			// iiduphong3
	int nMaxOpt;		// iiduphong4
	int nPf[4];			// iiduphong5..8
	int nPrice;			// iiduphong9
	int nRow;			// irow
	int nFusMark;		// iidentify (0x46555331 = co du lieu dung luyen)
	int nFusP[6];		// 6 o dung luyen (P)
	int nFusS[6];		// 6 seed dung luyen
};

#define AUC_REC_INTS	(sizeof(KAucRec) / sizeof(int))

static const char* sArgStr(Lua_State* L, int n)
{
	return (Lua_GetTopIndex(L) >= n && Lua_IsString(L, n)) ? Lua_ValueToString(L, n) : "";
}

static int sArgInt(Lua_State* L, int n)
{
	return (Lua_GetTopIndex(L) >= n && Lua_IsNumber(L, n)) ? (int)Lua_ValueToNumber(L, n) : 0;
}

static __int64 sArgI64(Lua_State* L, int n)
{
	return (Lua_GetTopIndex(L) >= n && Lua_IsNumber(L, n)) ? (__int64)Lua_ValueToNumber(L, n) : 0;
}

static void sRecToHex(const KAucRec* p, char* szOut, int nSize)
{
	const int* pi = (const int*)p;
	int nNeed = (int)AUC_REC_INTS * 8 + 1;
	if (!szOut || nSize < nNeed)
	{
		if (szOut && nSize > 0)
			szOut[0] = 0;
		return;
	}
	for (int i = 0; i < (int)AUC_REC_INTS; i++)
		sprintf(szOut + i * 8, "%08X", (unsigned)pi[i]);
	szOut[AUC_REC_INTS * 8] = 0;
}

static bool sHexToRec(const char* szHex, KAucRec* p)
{
	if (!szHex || !p)
		return false;
	int nLen = (int)strlen(szHex);
	if (nLen != (int)AUC_REC_INTS * 8)
		return false;
	int* pi = (int*)p;
	char szTmp[9];
	szTmp[8] = 0;
	for (int i = 0; i < (int)AUC_REC_INTS; i++)
	{
		memcpy(szTmp, szHex + i * 8, 8);
		unsigned u = 0;
		if (sscanf(szTmp, "%08X", &u) != 1)
			return false;
		pi[i] = (int)u;
	}
	return (p->nVer == AUC_REC_VER);
}

// [DAUGIA 04/09 B1] Loai nao sRecToItem tao lai duoc? PHAI kiem NGAY o duong ky gui, khong thi
// mon bi xoa khoi tui roi luc tra lai moi phat hien khong dung duoc -> mat vinh vien.
// Danh sach nay phai khop y het nhanh switch cua sRecToItem ben duoi.
static bool sCanRebuild(int nGenre)
{
	switch (nGenre)
	{
	case item_equip:
	case item_medicine:
	case item_task:
	case item_townportal:
	case item_magicscript:
	case item_fusion:
	case item_starstone:
		return true;
	default:
		return false;	// item_mine, item_materials, item_brokenequip... ban goc cung khong nap lai duoc
	}
}

// Lay ban ghi tu mot mon do dang co trong Item[] (khuon KPlayer::SavePlayerItemList)
static bool sItemToRec(int nItemIdx, KAucRec* p)
{
	if (nItemIdx <= 0 || nItemIdx >= MAX_ITEM || !p)
		return false;
	if (!sCanRebuild(Item[nItemIdx].m_CommonAttrib.nItemGenre))
		return false;
	KItem& it = Item[nItemIdx];
	memset(p, 0, sizeof(KAucRec));
	p->nVer = AUC_REC_VER;
	p->nNature = it.m_CommonAttrib.nItemNature;
	p->nGenre = it.m_CommonAttrib.nItemGenre;
	p->nDetail = it.m_CommonAttrib.nDetailType;
	p->nParticular = it.m_CommonAttrib.nParticularType;
	p->nLevel = it.m_CommonAttrib.nLevel;
	p->nSeries = it.m_CommonAttrib.nSeries;
	p->nVersion = it.GetItemParam()->nVersion;
	p->nRandSeed = (int)it.GetItemParam()->uRandomSeed;
	memcpy(p->nParam, it.GetItemParam()->nGeneratorLevel, sizeof(p->nParam));
	p->nLuck = it.GetItemParam()->nLuck;
	p->nDurability = it.GetDurability();
	p->nGoldId = it.GetGoldId();
	p->nStackNum = it.GetStackNum();
	p->nEnChance = it.GetEnChance();
	p->nPoint = it.IsPurple();
	p->nYear = it.GetTime()->bYear;
	p->nMonth = it.GetTime()->bMonth;
	p->nDay = it.GetTime()->bDay;
	p->nHour = it.GetTime()->bHour;
	p->nLock = it.GetPlayerItemLock();
	p->nHLock = it.GetPlayerItemHLock();
	p->nMantle = it.GetMantle();
	p->nUseParam = it.GetParam();
	p->nGlow = it.GetItemGlowLight();
	p->nMaxOpt = it.GetMaxOptMultiply();
	for (int k = 0; k < 4; k++)
		p->nPf[k] = it.GetPfPack(k);
	p->nPrice = (int)it.m_CommonAttrib.uPrice;
	p->nRow = it.m_CommonAttrib.nRow;
	// dung luyen: giu nguyen 6 o + 6 seed neu mon do co
	p->nFusMark = 0;
	for (int f = 0; f < 6; f++)
	{
		int nP = it.GetFusionP(f);
		unsigned uS = it.GetFusionSeed(f);
		p->nFusP[f] = nP;
		p->nFusS[f] = (int)uS;
		if (nP || uS)
			p->nFusMark = 0x46555331;
	}
	return true;
}

// Tao lai mon do tu ban ghi (khuon KPlayer::LoadPlayerItemList - xem chu thich dau tep).
// Tra ve chi so trong Item[], 0 neu that bai. KHONG dat vao tui - nguoi goi tu dat.
static int sRecToItem(const KAucRec* p)
{
	if (!p)
		return 0;
	KItem NewItem;
	ZeroMemory(&NewItem, sizeof(KItem));
	NewItem.m_CommonAttrib.nItemNature = p->nNature;
	NewItem.m_CommonAttrib.nItemGenre = p->nGenre;
	NewItem.m_CommonAttrib.nDetailType = p->nDetail;
	NewItem.m_CommonAttrib.nParticularType = p->nParticular;
	NewItem.m_CommonAttrib.nLevel = p->nLevel;
	NewItem.m_CommonAttrib.nSeries = p->nSeries;
	NewItem.m_CommonAttrib.nStackNum = p->nStackNum;
	NewItem.m_CommonAttrib.nEnChance = p->nEnChance;
	NewItem.m_CommonAttrib.nPoint = p->nPoint;
	NewItem.m_CommonAttrib.nRow = p->nRow;
	NewItem.m_CommonAttrib.nGoldId = p->nGoldId;
	memset(NewItem.m_GeneratorParam.nGeneratorLevel, 0, sizeof(NewItem.m_GeneratorParam.nGeneratorLevel));
	memcpy(NewItem.m_GeneratorParam.nGeneratorLevel, p->nParam, sizeof(NewItem.m_GeneratorParam.nGeneratorLevel));
	NewItem.m_GeneratorParam.nVersion = p->nVersion;
	NewItem.m_GeneratorParam.uRandomSeed = (unsigned)p->nRandSeed;
	NewItem.m_GeneratorParam.nLuck = p->nLuck;

	BOOL bOk = FALSE;
	switch (p->nGenre)
	{
	case item_equip:
		if (!p->nGoldId)
		{
			bOk = ItemGen.Gen_ExistEquipment(
				NewItem.m_CommonAttrib.nItemNature,
				NewItem.m_CommonAttrib.nItemNature >= NATURE_GOLD ? NewItem.m_CommonAttrib.nRow : NewItem.m_CommonAttrib.nDetailType,
				NewItem.m_CommonAttrib.nParticularType,
				NewItem.m_CommonAttrib.nSeries,
				NewItem.m_CommonAttrib.nLevel,
				NewItem.m_GeneratorParam.nGeneratorLevel,
				NewItem.m_GeneratorParam.nLuck,
				NewItem.m_GeneratorParam.nVersion,
				&NewItem);
		}
		else
		{
			bOk = ItemGen.GetGoldItemByIndex(NewItem.m_CommonAttrib.nGoldId, &NewItem,
				NewItem.m_GeneratorParam.nGeneratorLevel,
				NewItem.m_CommonAttrib.nSeries,
				NewItem.m_CommonAttrib.nEnChance);
		}
		NewItem.SetMaxOptMultiply(p->nMaxOpt);
		break;
	case item_medicine:
		bOk = ItemGen.Gen_Medicine(NewItem.m_CommonAttrib.nDetailType, NewItem.m_CommonAttrib.nParticularType,
			NewItem.m_CommonAttrib.nLevel, NewItem.m_GeneratorParam.nVersion, &NewItem, NewItem.m_CommonAttrib.nStackNum);
		break;
	case item_task:
		bOk = ItemGen.Gen_Quest(NewItem.m_CommonAttrib.nDetailType, &NewItem, NewItem.m_CommonAttrib.nStackNum);
		break;
	case item_townportal:
		bOk = ItemGen.Gen_TownPortal(&NewItem);
		break;
	case item_magicscript:
		bOk = ItemGen.Gen_MagicScript(NewItem.m_CommonAttrib.nDetailType, NewItem.m_CommonAttrib.nParticularType,
			&NewItem, NewItem.m_CommonAttrib.nLevel, NewItem.m_CommonAttrib.nSeries,
			NewItem.m_GeneratorParam.nLuck, NewItem.m_CommonAttrib.nStackNum);
		break;
	case item_fusion:
		bOk = ItemGen.Gen_Fusion(NewItem.m_CommonAttrib.nParticularType, &NewItem, NewItem.m_CommonAttrib.nStackNum);
		break;
	case item_starstone:
		bOk = ItemGen.Gen_StarStone(NewItem.m_CommonAttrib.nParticularType, &NewItem, NewItem.m_CommonAttrib.nStackNum);
		break;
	default:
		// item_mine / item_materials: ban goc cung khong tao lai duoc -> tu choi ky gui
		bOk = FALSE;
		break;
	}
	if (!bOk)
		return 0;

	NewItem.SetDurability(p->nDurability);
	NewItem.SetParam(p->nUseParam);
	NewItem.SetItemGlowLight(p->nGlow);
	NewItem.SetPlayerItemLock(p->nLock);
	NewItem.SetPlayerItemHLock(p->nHLock);
	NewItem.SetExpTime(p->nYear, (BYTE)p->nMonth, (BYTE)p->nDay, (BYTE)p->nHour);
	NewItem.m_CommonAttrib.uPrice = (unsigned)p->nPrice;
	NewItem.SetMantle(p->nMantle);
	for (int k = 0; k < 4; k++)
		NewItem.SetPfPack(k, p->nPf[k]);
	NewItem.ClearFusion();
	if (p->nFusMark == 0x46555331)
	{
		for (int f = 0; f < 6; f++)
		{
			if (p->nFusP[f] || p->nFusS[f])
				NewItem.SetFusion(f, p->nFusP[f], (unsigned)p->nFusS[f]);
		}
	}
	return ItemSet.AddI(&NewItem);
}

//////////////////////////////////////////////////////////////////////
// Ham Lua - vat pham
//////////////////////////////////////////////////////////////////////
// AUC_ItemToRec(nItemIdx) -> szHex, szTen, szMoTa ("g,d,p,l,s,k"), nCells, nStack, nHetHan
int LuaAUC_ItemToRec(Lua_State* L)
{
	int nItemIdx = sArgInt(L, 1);
	KAucRec rec;
	if (!sItemToRec(nItemIdx, &rec))
	{
		Lua_PushString(L, (char*)"");
		return 1;
	}
	char szHex[AUC_REC_INTS * 8 + 8];
	sRecToHex(&rec, szHex, sizeof(szHex));
	char szDesc[64];
	sprintf(szDesc, "%d,%d,%d,%d,%d,%d", rec.nGenre, rec.nDetail, rec.nParticular, rec.nLevel, rec.nSeries, rec.nLuck);
	Lua_PushString(L, szHex);
	Lua_PushString(L, Item[nItemIdx].GetName());
	Lua_PushString(L, szDesc);
	Lua_PushNumber(L, (double)(Item[nItemIdx].GetWidth() * Item[nItemIdx].GetHeight()));
	// [B1] so luong THO cua chong: ben Lua GetItemStackCount bi kep theo tran chong nen xoa thieu
	Lua_PushNumber(L, (double)Item[nItemIdx].GetStackNum());
	// [A15 04/09] HAN DUNG that su = m_CommonAttrib.nExpireTime (moc tuyet doi; GetExpireTime tra 0
	// khi khong co hoac da qua). Ban va truoc dung ham Lua GetItemLife la SAI hoan toan: ham do nhan
	// MA SU KIEN chu khong phai chi so vat pham, va tra -1 khi khong tim thay -> chan sach moi ky gui.
	// Luu y nExpirePoint (dem nguoc tu luc nhan) KHONG tinh la han co dinh nen khong chan.
	Lua_PushNumber(L, (double)Item[nItemIdx].GetExpireTime());
	return 6;
}

// AUC_RecName(szHex) -> szTen  (tao tam mot mon do, doc ten roi tra khe ve)
int LuaAUC_RecName(Lua_State* L)
{
	KAucRec rec;
	if (!sHexToRec(sArgStr(L, 1), &rec))
	{
		Lua_PushString(L, (char*)"");
		return 1;
	}
	int nIdx = sRecToItem(&rec);
	if (nIdx <= 0)
	{
		Lua_PushString(L, (char*)"");
		return 1;
	}
	Lua_PushString(L, Item[nIdx].GetName());
	ItemSet.Remove(nIdx);
	return 1;
}

// [DAUGIA 04/09 A16] AUC_RecDesc(szHex) -> szInfo, nStack
// szInfo = danh sach so cach nhau bang dau phay, DU de client dung lai DUNG bo mat cua mon:
//   genre, detail, particular, level, series, luck, nature, goldid, enchance,
//   MAX_ITEM_MAGICLEVEL muc phu van, hat giong ngau nhien
// Truong 'detail' da ma hoa san theo luat hoang kim (khuon KItemDice::FillItemDesc):
// hang nature >= NATURE_GOLD thi dung nRow chu KHONG phai nDetailType, vi Gen_ExistEquipment
// nhan nRow lam chi so dong goldequip.txt (KItemGenerator.CPP case NATURE_GOLD).
// KHONG tao vat pham tam nhu AUC_RecName/AUC_RecCells - chi giai hex, nen goi thoai mai.
int LuaAUC_RecDesc(Lua_State* L)
{
	KAucRec rec;
	if (!sHexToRec(sArgStr(L, 1), &rec))
	{
		Lua_PushString(L, (char*)"");
		Lua_PushNumber(L, 1);
		return 2;
	}
	int nDet = (rec.nNature >= NATURE_GOLD) ? rec.nRow : rec.nDetail;
	char szInfo[512];	// [A25] ~46 so
	int nLen = _snprintf(szInfo, sizeof(szInfo) - 1, "%d,%d,%d,%d,%d,%d,%d,%d,%d",
		rec.nGenre, nDet, rec.nParticular, rec.nLevel, rec.nSeries, rec.nLuck,
		rec.nNature, rec.nGoldId, rec.nEnChance);
	if (nLen < 0)
		nLen = 0;
	szInfo[nLen] = 0;
	for (int i = 0; i < MAX_ITEM_MAGICLEVEL; i++)
	{
		int n = _snprintf(szInfo + nLen, sizeof(szInfo) - 1 - nLen, ",%d", rec.nParam[i]);
		if (n <= 0)
			break;
		nLen += n;
		szInfo[nLen] = 0;
	}
	{
		int n = _snprintf(szInfo + nLen, sizeof(szInfo) - 1 - nLen, ",%u", (unsigned)rec.nRandSeed);
		if (n > 0)
		{
			nLen += n;
			szInfo[nLen] = 0;
		}
	}
	// [A25 04/09] Chu bao ten trang bi hoa DO va thieu van cuong / phi phong.
	// Ten do la vi DO BEN ve 0 (anh chup: "Do ben: 0 / 80") - mon dung lai bi coi la do hong.
	// Gui not nhung truong ChatItem co cho chua; ban ghi trong kho da giu san tat ca.
	{
		int nAdd[6];
		nAdd[0] = rec.nVersion;
		nAdd[1] = rec.nDurability;
		nAdd[2] = rec.nMaxOpt;
		nAdd[3] = rec.nPoint;
		nAdd[4] = rec.nLock;
		nAdd[5] = rec.nHLock;
		for (int k = 0; k < 6; k++)
		{
			int n = _snprintf(szInfo + nLen, sizeof(szInfo) - 1 - nLen, ",%d", nAdd[k]);
			if (n <= 0)
				break;
			nLen += n;
			szInfo[nLen] = 0;
		}
		for (int k2 = 0; k2 < 4; k2++)	// phi phong da ep
		{
			int n = _snprintf(szInfo + nLen, sizeof(szInfo) - 1 - nLen, ",%d", rec.nPf[k2]);
			if (n <= 0)
				break;
			nLen += n;
			szInfo[nLen] = 0;
		}
		for (int k3 = 0; k3 < 6; k3++)	// van cuong: 6 o
		{
			int n = _snprintf(szInfo + nLen, sizeof(szInfo) - 1 - nLen, ",%d", rec.nFusP[k3]);
			if (n <= 0)
				break;
			nLen += n;
			szInfo[nLen] = 0;
		}
		for (int k4 = 0; k4 < 6; k4++)	// van cuong: 6 hat giong
		{
			int n = _snprintf(szInfo + nLen, sizeof(szInfo) - 1 - nLen, ",%u", (unsigned)rec.nFusS[k4]);
			if (n <= 0)
				break;
			nLen += n;
			szInfo[nLen] = 0;
		}
	}
	Lua_PushString(L, szInfo);
	Lua_PushNumber(L, (double)(rec.nStackNum > 0 ? rec.nStackNum : 1));
	return 2;
}

// AUC_RecCells(szHex) -> so O hanh trang
int LuaAUC_RecCells(Lua_State* L)
{
	KAucRec rec;
	if (!sHexToRec(sArgStr(L, 1), &rec))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nIdx = sRecToItem(&rec);
	if (nIdx <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nCells = Item[nIdx].GetWidth() * Item[nIdx].GetHeight();
	ItemSet.Remove(nIdx);
	Lua_PushNumber(L, (double)nCells);
	return 1;
}

// AUC_GiveRec(szHex) -> nItemIdx (0 = loi / het cho). Tao lai DUNG mon do va dat vao tui nguoi choi hien tai.
int LuaAUC_GiveRec(Lua_State* L)
{
	int nPlayerIdx = 0;
	{
		lua_getglobal(L, SCRIPT_PLAYERINDEX);
		if (lua_isnumber(L, -1))
			nPlayerIdx = (int)lua_tonumber(L, -1);
		lua_settop(L, -2);
	}
	KAucRec rec;
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER || !sHexToRec(sArgStr(L, 1), &rec))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nIdx = sRecToItem(&rec);
	if (nIdx <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int x = 0, y = 0;
	if (!Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y))
	{
		ItemSet.Remove(nIdx);	// het cho: KHONG vut xuong dat, de nguoi goi bao loi va giu lai trong kho
		Lua_PushNumber(L, 0);
		return 1;
	}
	// [DAUGIA 04/09 B1] PHAI kiem ket qua AddKIL: truoc day bo qua nen khi dat that bai thi mon
	// nam mo coi trong ItemSet ma Lua van tuong da giao xong -> hop thu danh dau da nhan, mat do.
	if (Player[nPlayerIdx].m_ItemList.AddKIL(nIdx, pos_equiproom, x, y, false, true) <= 0)
	{
		ItemSet.Remove(nIdx);
		Lua_PushNumber(L, 0);
		return 1;
	}
	Lua_PushNumber(L, (double)nIdx);
	return 1;
}

// [DAUGIA 04/09 B1] AUC_CanGiveRec(szHex) -> 1 neu THUC SU dat duoc vao tui bay gio.
// Khac CalcFreeItemCellCount: ham do dem o ROI RAC, con dat do doi mot KHOI LIEN TUC WxH.
// Tui con 8 o roi rac van khong dat noi mot thanh vu khi 2x3.
int LuaAUC_CanGiveRec(Lua_State* L)
{
	int nPlayerIdx = 0;
	{
		lua_getglobal(L, SCRIPT_PLAYERINDEX);
		if (lua_isnumber(L, -1))
			nPlayerIdx = (int)lua_tonumber(L, -1);
		lua_settop(L, -2);
	}
	KAucRec rec;
	if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER || !sHexToRec(sArgStr(L, 1), &rec))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nIdx = sRecToItem(&rec);
	if (nIdx <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int x = 0, y = 0;
	int bOk = Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
		Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y) ? 1 : 0;
	ItemSet.Remove(nIdx);	// chi la phep thu - tra khe ve ngay
	Lua_PushNumber(L, bOk);
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Bang auction_item
//////////////////////////////////////////////////////////////////////
static bool s_bTableOk = false;

static bool sEnsureTable()
{
	if (!g_MySQLDB.IsReady())
		return false;
	if (s_bTableOk)
		return true;
	const char* szDDL =
		"CREATE TABLE IF NOT EXISTS auction_item ("
		" id INT AUTO_INCREMENT PRIMARY KEY,"
		" atype TINYINT NOT NULL DEFAULT 3,"
		" activity VARBINARY(64) NOT NULL DEFAULT '',"
		" kind TINYINT NOT NULL DEFAULT 2,"
		" seller VARBINARY(32) NOT NULL,"
		" seller_tong INT NOT NULL DEFAULT 0,"
		" item_name VARBINARY(64) NOT NULL DEFAULT '',"
		" item_desc VARBINARY(64) NOT NULL DEFAULT '',"
		" item_rec VARCHAR(1024) NOT NULL DEFAULT '',"
		" item_cells INT NOT NULL DEFAULT 1,"
		" currency TINYINT NOT NULL DEFAULT 1,"
		" base_price BIGINT NOT NULL DEFAULT 0,"
		" cur_price BIGINT NOT NULL DEFAULT 0,"
		" guaranteed_price BIGINT NOT NULL DEFAULT 0,"
		" buyer VARBINARY(32) NOT NULL DEFAULT '',"
		" buy_price BIGINT NOT NULL DEFAULT 0,"
		" deposit BIGINT NOT NULL DEFAULT 0,"
		" start_time INT NOT NULL DEFAULT 0,"
		" end_time INT NOT NULL DEFAULT 0,"
		" next_drop_time INT NOT NULL DEFAULT 0,"
		" drop_left INT NOT NULL DEFAULT 0,"
		" state TINYINT NOT NULL DEFAULT 0,"
		" KEY idx_state_type (state, atype),"
		" KEY idx_seller (seller, state),"
		" KEY idx_end (state, end_time)"
		") ENGINE=InnoDB DEFAULT CHARSET=latin1";
	s_bTableOk = g_MySQLDB.Exec(szDDL, 0, 0);
	if (!s_bTableOk)
		g_DebugLog((LPSTR)"[DAUGIA] khong tao duoc bang auction_item");
	return s_bTableOk;
}

struct KAucRow
{
	int nId, nType, nKind, nCurrency, nCells, nStart, nEnd, nNextDrop, nDropLeft, nState, nTong;
	__int64 nBase, nCur, nGuar, nBuyPrice, nDeposit;
	std::string sActivity, sSeller, sName, sDesc, sRec, sBuyer;
};

static std::string sCol(const KDBRow& row, int c)
{
	if (c < 0 || c >= row.nCol || !row.pVal[c] || row.pLen[c] <= 0)
		return std::string();
	return std::string(row.pVal[c], row.pLen[c]);
}

static __int64 sColI64(const KDBRow& row, int c)
{
	std::string s = sCol(row, c);
	return s.empty() ? 0 : _atoi64(s.c_str());
}

static int sColInt(const KDBRow& row, int c)
{
	return (int)sColI64(row, c);
}

// thu tu cot dung chung cho moi truy van (dung sai la lech het)
#define AUC_COLS \
	" id, atype, activity, kind, seller, seller_tong, item_name, item_desc, item_rec, item_cells," \
	" currency, base_price, cur_price, guaranteed_price, buyer, buy_price, deposit," \
	" start_time, end_time, next_drop_time, drop_left, state"

static bool _RowAuc(const KDBRow& row, void* p)
{
	std::vector<KAucRow>* pv = (std::vector<KAucRow>*)p;
	KAucRow r;
	r.nId = sColInt(row, 0);
	r.nType = sColInt(row, 1);
	r.sActivity = sCol(row, 2);
	r.nKind = sColInt(row, 3);
	r.sSeller = sCol(row, 4);
	r.nTong = sColInt(row, 5);
	r.sName = sCol(row, 6);
	r.sDesc = sCol(row, 7);
	r.sRec = sCol(row, 8);
	r.nCells = sColInt(row, 9);
	r.nCurrency = sColInt(row, 10);
	r.nBase = sColI64(row, 11);
	r.nCur = sColI64(row, 12);
	r.nGuar = sColI64(row, 13);
	r.sBuyer = sCol(row, 14);
	r.nBuyPrice = sColI64(row, 15);
	r.nDeposit = sColI64(row, 16);
	r.nStart = sColInt(row, 17);
	r.nEnd = sColInt(row, 18);
	r.nNextDrop = sColInt(row, 19);
	r.nDropLeft = sColInt(row, 20);
	r.nState = sColInt(row, 21);
	pv->push_back(r);
	return true;
}

static void sPushRow(Lua_State* L, const KAucRow& r)
{
	Lua_NewTable(L);
	int t = Lua_GetTopIndex(L);
	Lua_PushString(L, (char*)"id");        Lua_PushNumber(L, (double)r.nId);        Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"atype");     Lua_PushNumber(L, (double)r.nType);      Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"activity");  Lua_PushString(L, (char*)r.sActivity.c_str()); Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"kind");      Lua_PushNumber(L, (double)r.nKind);      Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"seller");    Lua_PushString(L, (char*)r.sSeller.c_str());   Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"tong");      Lua_PushNumber(L, (double)r.nTong);      Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"name");      Lua_PushString(L, (char*)r.sName.c_str());     Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"desc");      Lua_PushString(L, (char*)r.sDesc.c_str());     Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"rec");       Lua_PushString(L, (char*)r.sRec.c_str());      Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"cells");     Lua_PushNumber(L, (double)r.nCells);     Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"currency");  Lua_PushNumber(L, (double)r.nCurrency);  Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"base");      Lua_PushNumber(L, (double)r.nBase);      Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"cur");       Lua_PushNumber(L, (double)r.nCur);       Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"guar");      Lua_PushNumber(L, (double)r.nGuar);      Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"buyer");     Lua_PushString(L, (char*)r.sBuyer.c_str());    Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"buyprice");  Lua_PushNumber(L, (double)r.nBuyPrice);  Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"deposit");   Lua_PushNumber(L, (double)r.nDeposit);   Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"start");     Lua_PushNumber(L, (double)r.nStart);     Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"endtime");   Lua_PushNumber(L, (double)r.nEnd);       Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"nextdrop");  Lua_PushNumber(L, (double)r.nNextDrop);  Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"dropleft");  Lua_PushNumber(L, (double)r.nDropLeft);  Lua_SetTable(L, t);
	Lua_PushString(L, (char*)"state");     Lua_PushNumber(L, (double)r.nState);     Lua_SetTable(L, t);
}

int LuaAUC_Ready(Lua_State* L)
{
	Lua_PushNumber(L, sEnsureTable() ? 1 : 0);
	return 1;
}

// AUC_PutOn(nType, szActivity, nKind, szSeller, nTong, szName, szDesc, szRec, nCells, nCurrency,
//           nBase, nCur, nGuar, nDeposit, nStart, nEnd, nNextDrop, nDropLeft) -> id
int LuaAUC_PutOn(Lua_State* L)
{
	if (!sEnsureTable())
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KDBParam p[18];
	p[0] = KDBParam::I(sArgInt(L, 1));
	p[1] = KDBParam::S(sArgStr(L, 2));
	p[2] = KDBParam::I(sArgInt(L, 3));
	p[3] = KDBParam::S(sArgStr(L, 4));
	p[4] = KDBParam::I(sArgInt(L, 5));
	p[5] = KDBParam::S(sArgStr(L, 6));
	p[6] = KDBParam::S(sArgStr(L, 7));
	p[7] = KDBParam::S(sArgStr(L, 8));
	p[8] = KDBParam::I(sArgInt(L, 9));
	p[9] = KDBParam::I(sArgInt(L, 10));
	p[10] = KDBParam::I(sArgI64(L, 11));
	p[11] = KDBParam::I(sArgI64(L, 12));
	p[12] = KDBParam::I(sArgI64(L, 13));
	p[13] = KDBParam::I(sArgI64(L, 14));
	p[14] = KDBParam::I(sArgInt(L, 15));
	p[15] = KDBParam::I(sArgInt(L, 16));
	p[16] = KDBParam::I(sArgInt(L, 17));
	p[17] = KDBParam::I(sArgInt(L, 18));
	__int64 nId = 0;
	bool bOk = g_MySQLDB.Exec(
		"INSERT INTO auction_item (atype, activity, kind, seller, seller_tong, item_name, item_desc, item_rec,"
		" item_cells, currency, base_price, cur_price, guaranteed_price, deposit, start_time, end_time,"
		" next_drop_time, drop_left, state)"
		" VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, 0)", p, 18, 0, &nId);
	Lua_PushNumber(L, bOk ? (double)nId : 0);
	return 1;
}

// AUC_List(nType, nMax, nAfterId) -> bang {[i] = {...}} cac muc dang ban (state 0)
int LuaAUC_List(Lua_State* L)
{
	int nType = sArgInt(L, 1);
	int nMax = sArgInt(L, 2);
	int nAfter = sArgInt(L, 3);
	if (nMax <= 0 || nMax > AUC_MAX_ROW)
		nMax = 20;
	Lua_NewTable(L);
	if (!sEnsureTable())
		return 1;
	int tb = Lua_GetTopIndex(L);
	std::vector<KAucRow> rows;
	KDBParam p[3];
	p[0] = KDBParam::I(nType);
	p[1] = KDBParam::I(nAfter);
	p[2] = KDBParam::I(nMax);
	if (!g_MySQLDB.Query("SELECT" AUC_COLS " FROM auction_item"
		" WHERE state=0 AND atype=? AND id>? ORDER BY id LIMIT ?", p, 3, _RowAuc, &rows))
		return 1;
	for (size_t i = 0; i < rows.size(); i++)
	{
		Lua_PushNumber(L, (double)(i + 1));
		sPushRow(L, rows[i]);
		Lua_SetTable(L, tb);
	}
	return 1;
}

// AUC_Get(nId) -> bang mot muc / nil
int LuaAUC_Get(Lua_State* L)
{
	int nId = sArgInt(L, 1);
	if (nId <= 0 || !sEnsureTable())
		return 0;
	std::vector<KAucRow> rows;
	KDBParam p[1];
	p[0] = KDBParam::I(nId);
	if (!g_MySQLDB.Query("SELECT" AUC_COLS " FROM auction_item WHERE id=?", p, 1, _RowAuc, &rows) || rows.empty())
		return 0;
	sPushRow(L, rows[0]);
	return 1;
}

// AUC_Buy(nId, szBuyer, nPrice) -> 1/0. Nguyen tu: chi doi duoc khi state van la 0.
int LuaAUC_Buy(Lua_State* L)
{
	int nId = sArgInt(L, 1);
	const char* szBuyer = sArgStr(L, 2);
	__int64 nPrice = sArgI64(L, 3);
	if (nId <= 0 || !szBuyer[0] || !sEnsureTable())
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KDBParam p[3];
	p[0] = KDBParam::S(szBuyer);
	p[1] = KDBParam::I(nPrice);
	p[2] = KDBParam::I(nId);
	__int64 nAffected = 0;
	bool bOk = g_MySQLDB.Exec(
		"UPDATE auction_item SET state=1, buyer=?, buy_price=? WHERE id=? AND state=0", p, 3, &nAffected);
	Lua_PushNumber(L, (bOk && nAffected > 0) ? 1 : 0);
	return 1;
}

// AUC_SetState(nId, nState, nBelow) -> 1/0 (chi doi khi state < nBelow)
int LuaAUC_SetState(Lua_State* L)
{
	int nId = sArgInt(L, 1);
	int nState = sArgInt(L, 2);
	int nBelow = sArgInt(L, 3);
	if (nId <= 0 || !sEnsureTable())
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KDBParam p[3];
	p[0] = KDBParam::I(nState);
	p[1] = KDBParam::I(nId);
	p[2] = KDBParam::I(nBelow);
	__int64 nAffected = 0;
	bool bOk = g_MySQLDB.Exec("UPDATE auction_item SET state=? WHERE id=? AND state<?", p, 3, &nAffected);
	Lua_PushNumber(L, (bOk && nAffected > 0) ? 1 : 0);
	return 1;
}

// AUC_Sweep(nNow, nMax) -> bang cac muc HET HAN (state 0, end_time <= nNow) de script tra lai nguoi ban
int LuaAUC_Sweep(Lua_State* L)
{
	int nNow = sArgInt(L, 1);
	int nMax = sArgInt(L, 2);
	if (nNow <= 0)
		nNow = (int)time(NULL);
	if (nMax <= 0 || nMax > AUC_MAX_ROW)
		nMax = 20;
	Lua_NewTable(L);
	if (!sEnsureTable())
		return 1;
	int tb = Lua_GetTopIndex(L);
	std::vector<KAucRow> rows;
	KDBParam p[2];
	p[0] = KDBParam::I(nNow);
	p[1] = KDBParam::I(nMax);
	if (!g_MySQLDB.Query("SELECT" AUC_COLS " FROM auction_item"
		" WHERE state=0 AND end_time>0 AND end_time<=? ORDER BY id LIMIT ?", p, 2, _RowAuc, &rows))
		return 1;
	for (size_t i = 0; i < rows.size(); i++)
	{
		Lua_PushNumber(L, (double)(i + 1));
		sPushRow(L, rows[i]);
		Lua_SetTable(L, tb);
	}
	return 1;
}

// AUC_CountSeller(szSeller) -> so mon dang ky gui (state 0)
int LuaAUC_CountSeller(Lua_State* L)
{
	const char* szSeller = sArgStr(L, 1);
	Lua_PushNumber(L, 0);
	if (!szSeller[0] || !sEnsureTable())
		return 1;
	std::vector<KAucRow> rows;
	KDBParam p[1];
	p[0] = KDBParam::S(szSeller);
	struct KCnt { __int64 n; } c;
	c.n = 0;
	struct Local
	{
		static bool Row(const KDBRow& row, void* p)
		{
			((KCnt*)p)->n = (row.nCol > 0 && row.pLen[0] > 0) ? _atoi64(row.pVal[0]) : 0;
			return true;
		}
	};
	g_MySQLDB.Query("SELECT COUNT(*) FROM auction_item WHERE seller=? AND state=0", p, 1, Local::Row, &c);
	lua_settop(L, -2);
	Lua_PushNumber(L, (double)c.n);
	return 1;
}

// AUC_SetPrice(nId, nCur, nNextDrop, nDropLeft, nEnd) -> 1/0 (chi doi khi con dang ban)
int LuaAUC_SetPrice(Lua_State* L)
{
	int nId = sArgInt(L, 1);
	if (nId <= 0 || !sEnsureTable())
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KDBParam p[5];
	p[0] = KDBParam::I(sArgI64(L, 2));
	p[1] = KDBParam::I(sArgInt(L, 3));
	p[2] = KDBParam::I(sArgInt(L, 4));
	p[3] = KDBParam::I(sArgInt(L, 5));
	p[4] = KDBParam::I(nId);
	__int64 nAffected = 0;
	bool bOk = g_MySQLDB.Exec(
		"UPDATE auction_item SET cur_price=?, next_drop_time=?, drop_left=?, end_time=? WHERE id=? AND state=0", p, 5, &nAffected);
	Lua_PushNumber(L, (bOk && nAffected > 0) ? 1 : 0);
	return 1;
}

// AUC_Bid(nId, szBuyer, nPrice, nNewEnd) -> 1/0. Nguyen tu: chi khi state 0 va gia moi CAO HON gia hien tai.
int LuaAUC_Bid(Lua_State* L)
{
	int nId = sArgInt(L, 1);
	const char* szBuyer = sArgStr(L, 2);
	__int64 nPrice = sArgI64(L, 3);
	int nEnd = sArgInt(L, 4);
	if (nId <= 0 || !szBuyer[0] || nPrice <= 0 || !sEnsureTable())
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KDBParam p[5];
	p[0] = KDBParam::S(szBuyer);
	p[1] = KDBParam::I(nPrice);
	p[2] = KDBParam::I(nEnd);
	p[3] = KDBParam::I(nId);
	p[4] = KDBParam::I(nPrice);
	__int64 nAffected = 0;
	bool bOk = g_MySQLDB.Exec(
		"UPDATE auction_item SET buyer=?, cur_price=?, buy_price=cur_price, end_time=? WHERE id=? AND state=0 AND cur_price<?", p, 5, &nAffected);
	Lua_PushNumber(L, (bOk && nAffected > 0) ? 1 : 0);
	return 1;
}

// [DAUGIA 04/09 B1] AUC_Rollback(nId): tra dong ve DANG BAN va XOA nguoi mua.
// AUC_SetState chi lui state, de nguyen buyer/buy_price -> nguoi ban khong rut lai duoc (AUC_OnRequestGetBack
// tu choi vi buyer khac rong) va dong kieu Anh khi het gio se giao mon cho nguoi CHUA TRA dong nao.
int LuaAUC_Rollback(Lua_State* L)
{
	int nId = sArgInt(L, 1);
	if (nId <= 0 || !sEnsureTable())
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	KDBParam p[1];
	p[0] = KDBParam::I(nId);
	__int64 nAffected = 0;
	bool bOk = g_MySQLDB.Exec(
		"UPDATE auction_item SET state=0, buyer='', buy_price=0 WHERE id=? AND state=1", p, 1, &nAffected);
	Lua_PushNumber(L, (bOk && nAffected > 0) ? 1 : 0);
	return 1;
}

#endif // _SERVER
