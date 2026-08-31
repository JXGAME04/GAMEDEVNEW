//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
// File:	KItemGenerator.CPP
// Date:	2020.08.26
// Code:	Fong KiÒu
//---------------------------------------------------------------------------
#include "KEngine.h"
#include "KCore.h"
#include "MyAssert.H"
#include "KItem.h"
#include "KPlayer.h"
#include "KSubWorldSet.h"
#include "KItemGenerator.h"
#include "CoreUseNameDef.h"	// [PHI PHONG] TABFILE_GOLDITEM_FULL_N / PLATINA_EQUIP_FILE

// ======== [PHI PHONG 2026-08-29] doc 3 cot cuoi cua bang trang bi vang / bach kim ========
// Cot 60 = co the tang sao | 61 = so lo kham Tinh Than Thach | 62 = tran diem chuc phuc.
// Ban Linux nap thang 3 so nay len vat pham (jx_linux_y: item+0x2A8 so lo, +0x2AC tran
// chuc phuc). JX1 KHONG doc chung: KBPT_Equipment_Gold2::LoadRecord anh xa cot THEO VI TRI
// va trong danh sach do da co 2 muc bi chu thich (m_nNeedToActive1/2) roi lai chen
// m_aryMagicAttribs[6],[7] sau, nen them 3 muc vao duoi la cuoc vao mot anh xa da lech.
// Vi vay doc rieng o day cho CO LAP: hong cung chi hong phi phong.
// Quy uoc dong: mirror dung ::LoadRecord (KBasPropTbl.CPP:84) -> KTabFile row = nIndex + 2.
// Toan bang 5940 dong chi co 5 dong mang gia tri (5374..5378) nen nap 1 lan roi cache.
// goldequip.txt  (62 cot): nhan tieu de DUNG vi tri.
#define PF_GCOL_CANUPSTAR	60
#define PF_GCOL_MAXSTONE	61
#define PF_GCOL_MAXWISH		62
#define PF_GOLD_WIDTH		62
// platinaequip.txt (71 cot): LECH 1 O so voi goldequip. Bang nay co THEM mot cot
// KHONG TEN o vi tri 68 (gia tri 6400/200/600/24...) ma dong tieu de khong dat ten,
// nen tu do tro di nhan bi day sang trai 1 o. Doc theo nhan la SAI.
// Bang chung (khop 100%, 5 dong x 3 cot) -- cung mot mon do o hai bang:
//   gold 5374..5378 cot 60/61/62 = (1,1,4) (1,2,6) (1,3,8) (1,4,14) (1,5,_)
//   plat 5300..5304 cot 69/70/71 = (1,1,4) (1,2,6) (1,3,8) (1,4,13) (1,5,_)
// va dong 4835..4839 ("Phi phong Vo Cuc" 1..5 o kham) co cot 70 = 1,2,3,4,5.
// Doc 68/69/70 thi MaxStone thanh hang so 1 con MaxWish thanh 1..5: vo nghia.
#define PF_PCOL_CANUPSTAR	69
#define PF_PCOL_MAXSTONE	70
#define PF_PCOL_MAXWISH		71
#define PF_PLAT_WIDTH		71

struct KPfRowInfo
{
	int	nCanUpStar;
	int	nMaxStone;
	int	nMaxWish;
};

static BOOL PF_GetRowInfo(BOOL bPlatina, int nIndex, KPfRowInfo* pOut)
{
	static BOOL			s_bLoaded[2] = { FALSE, FALSE };
	static KPfRowInfo*	s_pTab[2]    = { NULL, NULL };
	static int			s_nRows[2]   = { 0, 0 };

	int t = bPlatina ? 1 : 0;
	if (!s_bLoaded[t])
	{
		s_bLoaded[t] = TRUE;
		KTabFile tf;
		if (tf.Load(bPlatina ? PLATINA_EQUIP_FILE : TABFILE_GOLDITEM_FULL_N))
		{
			// Moi bang mot bo cot rieng -- xem khoi #define o tren.
			const int nColStar  = bPlatina ? PF_PCOL_CANUPSTAR : PF_GCOL_CANUPSTAR;
			const int nColStone = bPlatina ? PF_PCOL_MAXSTONE  : PF_GCOL_MAXSTONE;
			const int nColWish  = bPlatina ? PF_PCOL_MAXWISH   : PF_GCOL_MAXWISH;
			const int nNeedWid  = bPlatina ? PF_PLAT_WIDTH     : PF_GOLD_WIDTH;
			int nH = tf.GetHeight();
			// Doi DU be rong: bang thieu cot la doc trung vao cot khac (da dinh mot lan).
			if (nH > 0 && tf.GetWidth() >= nNeedWid)
			{
				s_nRows[t] = nH;
				s_pTab[t] = new KPfRowInfo[nH + 2];
				memset(s_pTab[t], 0, sizeof(KPfRowInfo) * (nH + 2));
				for (int r = 1; r <= nH; r++)
				{
					int a = 0, b = 0, c = 0;
					tf.GetInteger(r, nColStar,  0, &a);
					tf.GetInteger(r, nColStone, 0, &b);
					tf.GetInteger(r, nColWish,  0, &c);
					s_pTab[t][r].nCanUpStar = a;
					s_pTab[t][r].nMaxStone  = b;
					s_pTab[t][r].nMaxWish   = c;
				}
			}
		}
	}

	if (!pOut)
		return FALSE;
	pOut->nCanUpStar = 0;
	pOut->nMaxStone  = 0;
	pOut->nMaxWish   = 0;

	int nRow = nIndex + 2;	// mirror ::LoadRecord
	if (!s_pTab[t] || nRow < 1 || nRow > s_nRows[t])
		return FALSE;
	*pOut = s_pTab[t][nRow];
	return TRUE;
}

// Gan 3 so tren vao vat pham vua sinh. Chi dung cho trang bi CO cot 61 > 0
// (thuc te chi 5 dong phi phong), cac mon khac giu nguyen 0 nhu truoc.
static void PF_ApplyRowInfo(BOOL bPlatina, int nIndex, KItem* pItem)
{
	if (!pItem)
		return;
	KPfRowInfo sInfo;
	if (!PF_GetRowInfo(bPlatina, nIndex, &sInfo))
		return;
	if (sInfo.nMaxStone <= 0 && sInfo.nMaxWish <= 0 && sInfo.nCanUpStar <= 0)
		return;
	pItem->SetMaxStoneNum(sInfo.nMaxStone);
	pItem->SetMaxWishValue(sInfo.nMaxWish);
	pItem->SetCanUpStar(sInfo.nCanUpStar ? TRUE : FALSE);
}
// ======== het khoi doc cot phi phong ========

KItemGenerator	ItemGen;			

KItemGenerator::KItemGenerator()
{
}

KItemGenerator::~KItemGenerator()
{
}
/**********************************************************************************
Function: Data initialization. Read data from tab file
******************************************************************************/
BOOL KItemGenerator::Init()
{
	if (!m_BPTLib.Init())		//This call loads all initial properties from several tab files.
		return FALSE;

	for (int i = equip_meleeweapon; i < equip_detailnum - equip_meleeweapon; i++)
	{
		switch(i)
		{
		case equip_meleeweapon:
			m_EquipNumOfEntries[i] = m_BPTLib.GetMeleeWeaponRecordNumber();
			break;
		case equip_rangeweapon:
			m_EquipNumOfEntries[i] = m_BPTLib.GetRangeWeaponRecordNumber();
			break;
		case equip_armor:
			m_EquipNumOfEntries[i] = m_BPTLib.GetArmorRecordNumber();
			break;
		case equip_helm:
			m_EquipNumOfEntries[i] = m_BPTLib.GetHelmRecordNumber();
			break;
		case equip_boots:
			m_EquipNumOfEntries[i] = m_BPTLib.GetBootRecordNumber();
			break;
		case equip_belt:
			m_EquipNumOfEntries[i] = m_BPTLib.GetBeltRecordNumber();
			break;
		case equip_cuff:
			m_EquipNumOfEntries[i] = m_BPTLib.GetCuffRecordNumber();
			break;
		case equip_amulet:
			m_EquipNumOfEntries[i] = m_BPTLib.GetAmuletRecordNumber();
			break;
		case equip_ring:
			m_EquipNumOfEntries[i] = m_BPTLib.GetRingRecordNumber();
			break;
		case equip_pendant:
			m_EquipNumOfEntries[i] = m_BPTLib.GetPendantRecordNumber();
			break;
		case equip_horse:
			m_EquipNumOfEntries[i] = m_BPTLib.GetHorseRecordNumber();
			break;
		case equip_mask:	// mat na
			m_EquipNumOfEntries[i] = m_BPTLib.GetMaskRecordNumber();
			break;
		case equip_mantle: // phi phong
			m_EquipNumOfEntries[i] = m_BPTLib.GetMantleRecordNumber();
			break;
		case equip_signet:
			m_EquipNumOfEntries[i] = m_BPTLib.GetSignetRecordNumber();
			break;
		case equip_shipin:
			m_EquipNumOfEntries[i] = m_BPTLib.GetShipinRecordNumber();
			break;
		case equip_hoods:
			m_EquipNumOfEntries[i] = m_BPTLib.GetHoodsRecordNumber();
			break;
		case equip_cloak:
			m_EquipNumOfEntries[i] = m_BPTLib.GetCloakRecordNumber();
			break;
		}
	}
	m_MedNumOfEntries = m_BPTLib.GetMedicineRecordNumber();
	return TRUE;
/*
	m_BPTLib.Init();
	int naryMALevel[] = {1,2,3,4,5,6};
	KItem theItem;
	Gen_Equipment(1, 1, 0, 5, naryMALevel, 45, &theItem);
	return TRUE;
*/
}

BOOL KItemGenerator::Gen_Medicine(IN int nDetailType, IN int nParticularType,
								  IN int nLevel,
								  IN int nVersion,
								  IN OUT KItem* pItem,
								  IN int nStackNum)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);

	BOOL bEC = FALSE;
	
#ifdef _SERVER
	pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck = 0;
#else
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	int nGameVersion = pItem->m_GeneratorParam.nVersion;
#endif

	const int i = nDetailType * 5 + nLevel - 1;

	const KBASICPROP_MEDICINE* pMed = NULL;
	pMed = m_BPTLib.GetMedicineRecord(i);
	if (NULL == pMed)
	{
		_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pMed;
	pItem->SetStackNum(nStackNum);
	pItem->m_CommonAttrib.nParticularType = nParticularType; //edit by Fong KiÒu míi thªm vµo skill shortcut
#ifndef _SERVER
	if(nDetailType == 13 && nParticularType > 0) //item skill
	{
		int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(nParticularType);
		if(nLevel > 0)
			nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nParticularType);

		ISkill * pISkill = g_SkillManager.GetSkill(nParticularType, nLevel);
		if(pISkill)
			strcpy(pItem->m_Image.szImage, pISkill->GetSkillIcon()); //VÏ l¹i icon item skill lÊy icon tõ skill qua
	}
#endif
	return TRUE;
}

BOOL KItemGenerator::Get_SizeItem(IN int nItemGenre, IN int nDetailType, IN int nParticularType, IN int nLevel, IN OUT int* nWidth, IN OUT int* nHeight)
{
	BOOL bCheck = FALSE;
	*nWidth = 0;
	*nHeight = 0;

	switch(nItemGenre)
	{
		case item_equip:		
			{
				const KBASICPROP_EQUIPMENT* pEqu = NULL;
				const int i = nParticularType * 10 + nLevel - 1;
				
				switch(nDetailType)
				{
					case equip_meleeweapon:
						pEqu = m_BPTLib.GetMeleeWeaponRecord(i);
						break;
					case equip_rangeweapon:
						pEqu = m_BPTLib.GetRangeWeaponRecord(i);
						break;
					case equip_armor:
						pEqu = m_BPTLib.GetArmorRecord(i);
						break;
					case equip_helm:
						pEqu = m_BPTLib.GetHelmRecord(i);
						break;
					case equip_boots:
						pEqu = m_BPTLib.GetBootRecord(i);
						break;
					case equip_belt:
						pEqu = m_BPTLib.GetBeltRecord(i);
						break;
					case equip_amulet:
						pEqu = m_BPTLib.GetAmuletRecord(i);
						break;
					case equip_ring:
						pEqu = m_BPTLib.GetRingRecord(i);
						break;
					case equip_cuff:
						pEqu = m_BPTLib.GetCuffRecord(i);
						break;
					case equip_pendant:
						pEqu = m_BPTLib.GetPendantRecord(i);
						break;
					case equip_horse:
						pEqu = m_BPTLib.GetHorseRecord(i);
						break;
					case equip_mask: // MASK
						pEqu = m_BPTLib.GetMaskRecord(i);
						break;		
					case equip_mantle:
						pEqu = m_BPTLib.GetMantleRecord(i);
						break;
					case equip_signet:
						pEqu = m_BPTLib.GetSignetRecord(i);
						break;
					case equip_shipin:
						pEqu = m_BPTLib.GetShipinRecord(i);
						break;
					case equip_hoods:
						pEqu = m_BPTLib.GetHoodsRecord(i);
						break;
					case equip_cloak:
						pEqu = m_BPTLib.GetCloakRecord(i);
						break;
					default:
						break;
				}

				if (pEqu != NULL)
				{
					*nWidth = pEqu->m_nWidth;
					*nHeight = pEqu->m_nHeight;
					bCheck = TRUE;
				}

			}
			break;
		case item_medicine:		
			{
				const int i = nDetailType * 5 + nLevel - 1;	
				const KBASICPROP_MEDICINE* pMed = NULL;
				pMed = m_BPTLib.GetMedicineRecord(i);

				if (pMed != NULL)
				{
					*nWidth = pMed->m_nWidth;
					*nHeight = pMed->m_nHeight;
					bCheck = TRUE;
				}
			}
			break;
		case item_mine:			
			break;
		case item_materials:		
			break;
		case item_task:				
			{
				const KBASICPROP_QUEST* pQuest = NULL;
				pQuest = m_BPTLib.GetQuestRecord(nDetailType);

				if (pQuest != NULL)
				{
					*nWidth = pQuest->m_nWidth;
					*nHeight = pQuest->m_nHeight;
					bCheck = TRUE;
				}
			}
			break;
		case item_townportal:
			{
				const KBASICPROP_TOWNPORTAL* pPortal = NULL;
				pPortal = m_BPTLib.GetTownPortalRecord(0);

				if (pPortal != NULL)
				{
					*nWidth = pPortal->m_nWidth;
					*nHeight = pPortal->m_nHeight;
					bCheck = TRUE;
				}

			}
			break;
		default:
			break;
	}
	return bCheck;
}

BOOL KItemGenerator::Gen_Equipment(IN int nDetailType,
								   IN int nParticularType,
								   IN int nSeriesReq,
								   IN int nLevel,
								   IN const int* pnaryMALevel,
								   IN int nLucky,
								   IN int nVersion,
								   IN OUT KItem* pItem, 
								   IN int nEnChance,
								   IN int nPoint)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);

	BOOL bEC = FALSE;
#ifdef _SERVER	// ·þÎñÆ÷°æ±¾
	pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * MAX_ITEM_MAGICATTRIB);
	else
		ZeroMemory(pItem->m_GeneratorParam.nGeneratorLevel, sizeof(int) * MAX_ITEM_MAGICATTRIB);
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck = nLucky;
#else
	pItem->m_GeneratorParam.nLuck = nLucky;//edit by phong kieu fix khong get dc nluck t¹i client dÉn ®Õn sai option ®å xanh chatitem
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * MAX_ITEM_MAGICATTRIB);
#endif
	int nGameVersion = pItem->m_GeneratorParam.nVersion;

	int i = 0;
	if (nDetailType > equip_horse)
		i = nParticularType;
	else
		i = nParticularType * 10 + nLevel - 1;
	const KBASICPROP_EQUIPMENT* pEqu = NULL;
	switch(nDetailType)
	{
		case equip_meleeweapon:
			pEqu = m_BPTLib.GetMeleeWeaponRecord(i);
			break;
		case equip_rangeweapon:
			pEqu = m_BPTLib.GetRangeWeaponRecord(i);
			break;
		case equip_armor:
			pEqu = m_BPTLib.GetArmorRecord(i);
			break;
		case equip_helm:
			pEqu = m_BPTLib.GetHelmRecord(i);
			break;
		case equip_boots:
			pEqu = m_BPTLib.GetBootRecord(i);
			break;
		case equip_belt:
			pEqu = m_BPTLib.GetBeltRecord(i);
			break;
		case equip_amulet:
			pEqu = m_BPTLib.GetAmuletRecord(i);
			break;
		case equip_ring:
			pEqu = m_BPTLib.GetRingRecord(i);
			break;
		case equip_cuff:
			pEqu = m_BPTLib.GetCuffRecord(i);
			break;
		case equip_pendant:
			pEqu = m_BPTLib.GetPendantRecord(i);
			break;
		case equip_horse:
			pEqu = m_BPTLib.GetHorseRecord(i);
			break;
		case equip_mask:	// mat na
			pEqu = m_BPTLib.GetMaskRecord(i);
			break;
		case equip_mantle:
			pEqu = m_BPTLib.GetMantleRecord(i);
			break;
		default:
			return bEC;
			break;
	}
	if (NULL == pEqu)
	{
		_ASSERT(FALSE); 
		return bEC; 
	}

	pItem->SetAttrib_CBR(pEqu);
	pItem->SetSeries(nSeriesReq);

	if (nPoint == 0)
	{
		if (NULL == pnaryMALevel)
			return TRUE;
		KItemNormalAttrib	sMA[MAX_ITEM_MAGICATTRIB];
		bEC = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);
		if (bEC)
			pItem->SetAttrib_MA(sMA);
	}
	else
	{
		KItemNormalAttrib sMA[MAX_ITEM_MAGICATTRIB];
		ZeroMemory(sMA,sizeof(sMA));
		KTabFile tabMagicAttribe;
		tabMagicAttribe.Load("\\Settings\\Item\\magicattrib.txt");

		if (nLucky > 10)
		{
			nLucky = 10;
		}
		else if (nLucky < 0)
		{
			nLucky = 0;
		}

		int nType,nMax,nMin;
		for (int j = 0;j < MAX_ITEM_MAGICATTRIB;j++)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[j] > 0 && j < nPoint)
			{
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],5,0,&nType);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],6,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],7,0,&nMin);
				sMA[j].nAttribType = nType;
				sMA[j].nValue[0] = nMax + ((nMin - nMax) * nLucky / 10);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],8,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],9,0,&nMin);
				sMA[j].nValue[1] = nMax + ((nMin - nMax) * nLucky / 10);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],10,0,&nMax);
				tabMagicAttribe.GetInteger(pItem->m_GeneratorParam.nGeneratorLevel[j],11,0,&nMin);
				sMA[j].nValue[2] = nMax + ((nMin - nMax) * nLucky / 10);
				continue;
			}
			sMA[j].nAttribType = 0;
			sMA[j].nValue[0] = 0;
			sMA[j].nValue[1] = 0;
			sMA[j].nValue[2] = 0;
		}
		pItem->SetAttrib_MA(sMA);
		pItem->SetPoint(nPoint);
		
		tabMagicAttribe.Clear();
	}
	
	pItem->EnChance(nEnChance);
	
	return bEC;
}
/*
BOOL KItemGenerator::Gen_MagicAttrib(
									 int nType, const int* pnaryMALevel, int nSeriesReq, int nLucky,
									 KItemNormalAttrib* pnaryMA, int nGameVersion)
{
    int nResult = false;

    KBPT_ClassMAIT  SelectedMagicTable;	

    KMAGICATTRIB_TABFILE *pMagicAttrTable[6] = { NULL, NULL, NULL, NULL, NULL, NULL };

    int i = 0;

    _ASSERT(pnaryMALevel);
    _ASSERT(pnaryMA);

    for (i = 0; i < 6; i++) 
    {
        if (pnaryMALevel[i] == 0)
        	break;

        SelectedMagicTable.Clear();

        const KBPT_ClassMAIT *pCMITItem = m_BPTLib.GetCMIT(
            1 - (i & 1),        
            nType,              
            nSeriesReq,          
            pnaryMALevel[i]     
        );

        _ASSERT(pCMITItem);
        if (!pCMITItem)
        {
            g_DebugLog(
                "[GenMagicAttrib] GetCMIT Error, nPrefixPostfix = %d, nType = %d, nSeries = %d, nLevel = %d", 
                1 - (i & 1), nType, nSeriesReq, pnaryMALevel[i]
            );
            break;
        }

        int nCMITItemCount = pCMITItem->GetCount();
        int j = 0;
        //int nDecide = GetRandomNumber(0, 100 - 1) / (1 + nLucky * 20 / 100);
		int nDecide = (100 * g_Random(1000000)) / (10 * nLucky + 100);
        for (j = 0; j < nCMITItemCount; j++)
        {
            int nMAIndex = pCMITItem->Get(j);
            KMAGICATTRIB_TABFILE *pMAItem = (KMAGICATTRIB_TABFILE *)m_BPTLib.GetMARecord(nMAIndex);
            if (!pMAItem)
            {
                _ASSERT(pMAItem);
                continue;
            }

            if (pMAItem->m_nUseFlag)
                continue;
			
            if ((pMAItem->m_DropRate[nType]) <= nDecide)
                continue;
            
            int k = 0;
            for (k = 0; k < i; k++)
            {
				//if (!pMagicAttrTable[k])
				//{
					//printf("Da xac dinh duoc loi RollBack 3\n");
				//	continue;
				//}

                _ASSERT(pMagicAttrTable[k]);

                if ((pMagicAttrTable[k]->m_MagicAttrib.nPropKind) == (pMAItem->m_MagicAttrib.nPropKind))
                    break;
            }

            if (k < i)
                continue;

            SelectedMagicTable.Insert(nMAIndex);
        }

        int nSelectedCount = SelectedMagicTable.GetCount();

        if (nSelectedCount == 0)
        {   
            break;
        }
        
        int nLuckyItemIndex = GetRandomNumber(0, nSelectedCount - 1);
        KMAGICATTRIB_TABFILE *pMAItem = (KMAGICATTRIB_TABFILE *)m_BPTLib.GetMARecord(
             SelectedMagicTable.Get(nLuckyItemIndex)
        );

        pMagicAttrTable[i] = pMAItem;

        _ASSERT(pMAItem);
        _ASSERT(!(pMAItem->m_nUseFlag));     

        pMAItem->m_nUseFlag = true;
		
        pnaryMA[i].nAttribType = pMAItem->m_MagicAttrib.nPropKind;
        pnaryMA[i].nValue[0] = GetRandomNumber(
            pMAItem->m_MagicAttrib.aryRange[0].nMin, 
            pMAItem->m_MagicAttrib.aryRange[0].nMax
        );
        pnaryMA[i].nValue[1] = GetRandomNumber(
            pMAItem->m_MagicAttrib.aryRange[1].nMin, 
            pMAItem->m_MagicAttrib.aryRange[1].nMax
        );
        pnaryMA[i].nValue[2] = GetRandomNumber(
            pMAItem->m_MagicAttrib.aryRange[2].nMin, 
            pMAItem->m_MagicAttrib.aryRange[2].nMax
        );
		g_DebugLog(
			"[GenMagicAttrib] pnaryMA[%d]: (%d:%s), nAttribType = %d, nValue[0] = %d, nValue[1] = %d, nValue[2] = %d,", 
			i,
			SelectedMagicTable.Get(nLuckyItemIndex),
			pMAItem->m_szName,
			pnaryMA[i].nAttribType,
			pnaryMA[i].nValue[0],
			pnaryMA[i].nValue[1],
			pnaryMA[i].nValue[2]
		);

    } 

    for (NULL; i < 6; i++)
    {
        pnaryMA[i].nAttribType = 0;
        pnaryMA[i].nValue[0] = 0;
        pnaryMA[i].nValue[1] = 0;
        pnaryMA[i].nValue[2] = 0;
    }

    for (i = 0; i < 6; i++)
    {
        if (!pMagicAttrTable[i])
            break;

        _ASSERT(pMagicAttrTable[i]->m_nUseFlag);   

        pMagicAttrTable[i]->m_nUseFlag = false;    
    }

    nResult = true;
//Exit0:
	return nResult;
}
*/
// [LOREN] Dich CHI SO DONG cua bang magicattriblevel.txt thanh mot thuoc
// tinh cu the. Ban Linux ghi chi so dong vao arynMagLvl, ban JX1 thi cho
// rang do la cap 1..10 - hai quy uoc khac han nhau.
// Bang nam o  \settings\item\%03d\magicattriblevel.txt, cot:
//   0 NAME  1 PREFIX  2 SERIES  3 LEVEL  4 MAGIC_ID
//   5 MAG_P1_MIN  6 MAG_P1_MAX  7 MAG_P2_MIN  8 MAG_P2_MAX
//   9 MAG_P3_MIN 10 MAG_P3_MAX 11 BASE_VALUE
// Nap mot lan cho moi phien ban roi giu lai.
#define LOREN_MAX_ITEM_VER 8
static KTabFile   s_LoRenMagTab[LOREN_MAX_ITEM_VER];
static BOOL       s_LoRenMagTabDaNap[LOREN_MAX_ITEM_VER] = { FALSE };

static KTabFile* sLoRenBangThuocTinh(int nVersion)
{
	if (nVersion < 0 || nVersion >= LOREN_MAX_ITEM_VER)
		return NULL;
	if (!s_LoRenMagTabDaNap[nVersion])
	{
		char szPath[MAX_PATH];
		_snprintf(szPath, sizeof(szPath) - 1,
			  "\\settings\\item\\%03d\\magicattriblevel.txt", nVersion);
		szPath[sizeof(szPath) - 1] = 0;
		s_LoRenMagTab[nVersion].Load(szPath);
		s_LoRenMagTabDaNap[nVersion] = TRUE;
	}
	if (s_LoRenMagTab[nVersion].GetHeight() < 2)
		return NULL;
	return &s_LoRenMagTab[nVersion];
}

// Tra ve TRUE neu dich duoc. nRow dem tu 1 (dong tieu de la dong 1 cua
// KTabFile nen dong du lieu thu nRow nam o nRow + 1).
static BOOL sLoRenDichThuocTinh(int nVersion, int nRow, KItemNormalAttrib* pOut)
{
	if (!pOut || nRow <= 0)
		return FALSE;
	KTabFile* pTab = sLoRenBangThuocTinh(nVersion);
	if (!pTab)
		return FALSE;
	if (nRow + 1 > pTab->GetHeight())
		return FALSE;

	int nMagicId = 0;
	pTab->GetInteger(nRow + 1, (LPSTR)"MAGIC_ID", 0, &nMagicId);
	if (nMagicId <= 0)
		return FALSE;

	const char* szMin[3] = { "MAG_P1_MIN", "MAG_P2_MIN", "MAG_P3_MIN" };
	const char* szMax[3] = { "MAG_P1_MAX", "MAG_P2_MAX", "MAG_P3_MAX" };
	pOut->nAttribType = nMagicId;
	for (int k = 0; k < 3; k++)
	{
		int nMin = 0, nMax = 0;
		pTab->GetInteger(nRow + 1, (LPSTR)szMin[k], 0, &nMin);
		pTab->GetInteger(nRow + 1, (LPSTR)szMax[k], 0, &nMax);
		// dai am (vd -1,-1) nghia la o nay khong dung - giu nguyen so do
		if (nMin < 0 || nMax < nMin)
			pOut->nValue[k] = nMin;
		else if (nMax == nMin)
			pOut->nValue[k] = nMin;
		else
			pOut->nValue[k] = GetRandomNumber(nMin, nMax);
	}
	return TRUE;
}

BOOL KItemGenerator::Gen_MagicAttrib(
	int nType, const int* pnaryMALevel, int nSeriesReq, int nLucky,
	KItemNormalAttrib* pnaryMA, int nGameVersion, int nItemNature
)
{
	int nResult = false;

	KBPT_ClassMAIT  SelectedMagicTable;
	// ¼ÇÂ¼Ã¿Ò»¸ö¼ÇÂ¼Ñ¡ÖÐµÄÄ§·¨ÊôÐÔ£¬Ö÷ÒªÊÇÎªÁË×îºó½«Ê¹ÓÃ±êÖ¾Çå³ý
	KMAGICATTRIB_TABFILE* pMagicAttrTable[MAX_ITEM_MAGICLEVEL] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

	int i = 0;

	_ASSERT(pnaryMALevel);
	_ASSERT(pnaryMA);

	if (pnaryMALevel[MAX_ITEM_MAGICATTRIB])
	{
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			if (pnaryMALevel[i] <= 0)
				break;
			pnaryMA[i].nAttribType = pnaryMALevel[i];
			pnaryMA[i].nValue[0] = LOWORD(pnaryMALevel[i + MAX_ITEM_MAGICATTRIB]);
			pnaryMA[i].nValue[1] = -1;
			pnaryMA[i].nValue[2] = HIWORD(pnaryMALevel[i + MAX_ITEM_MAGICATTRIB]);
		}
	}
	else
	{
		// [LOREN] Xoa sach TRUOC khi sinh.
		// Ban goc dua vao vong don o cuoi ham de xoa phan con lai, nhung
		// vong do chi chay khi vong duoi day `break` som. Khi co o kham
		// nam rong (-1) thi vong duoi day di het MAX_ITEM_MAGICLEVEL,
		// vong don thanh lenh rong -> mang sMA[] chua khoi tao cua
		// Gen_ExistEquipment se ro rac vao trang bi.
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			pnaryMA[i].nAttribType = 0;
			pnaryMA[i].nValue[0] = 0;
			pnaryMA[i].nValue[1] = 0;
			pnaryMA[i].nValue[2] = 0;
		}

		// [LOREN 27/08] NHAN DIEN CHI SO DONG THEO CA MANG.
		// Truoc day xet TUNG O: `pnaryMALevel[i] > MATF_LEVEL`. Phep doan do
		// HONG khi chi so dong roi vao 1..10 - va co that: bang
		// magicattriblevel_index.txt cho MAGIC_ID 126 ("Tang sat thuong vat ly
		// he Ngoai cong") dai chi so dong dung bang 1..10. O do bi hieu nham la
		// "cap 10" cua trang bi thuong -> di duong GetCMIT -> SINH THUOC TINH
		// NGAU NHIEN, nen kham vien da nay vao lai hien ra mot dong khac han.
		// (Do that: trang bi <29,236,10,340,-1,0>, o thu 3 mang chi so dong 10.)
		//
		// Trang bi tim dung chi so dong cho CA SAU O, nen xet CA MANG mot lan:
		// chi can mot o vuot MATF_LEVEL hoac bang -1 (o kham nam rong - rieng
		// trang bi tim moi co) thi ca mang la chi so dong. Nho vay o mang gia tri
		// nho (1..10) cung duoc hieu dung.
		// Van uu tien nItemNature khi duong goi co truyen (KItemSet::Add); duong
		// dong bo ve client (AddItemSet2 -> Gen_Equipment ban 10 tham so) khong
		// mang pham chat xuong nen moi phai suy tu chinh mang.
		BOOL bChiSoDong = (nItemNature == NATURE_VIOLET);
		if (!bChiSoDong)
		{
			for (int k = 0; k < MAX_ITEM_MAGICATTRIB; k++)
			{
				if (pnaryMALevel[k] > MATF_LEVEL || pnaryMALevel[k] == -1)
				{
					bChiSoDong = TRUE;
					break;
				}
			}
		}
		
		for (i = 0; i < MAX_ITEM_MAGICLEVEL; i++)
		{
			// Èç¹û·¢ÏÖ¼¶±ðÎªÁã£¬±íÊ¾²»ÐèÒª¼ÌÐø´¦ÀíÁË£¬ÍË³ö
			if (pnaryMALevel[i] == 0)
				break;
			// [LOREN] chan cung: pnaryMA[] chi co MAX_ITEM_MAGICATTRIB
			// o, trong khi vong nay chay toi MAX_ITEM_MAGICLEVEL. Truoc
			// day khong no vi vong luon break som; nay co nhanh bo qua
			// o rong nen phai chan.
			if (i >= MAX_ITEM_MAGICATTRIB)
				break;
			// [LOREN] ma thu ba cua ban Linux: -1 = O KHAM NAM RONG.
			// Giu khe (de arynMagLvl[j] va m_aryMagicAttrib[j] cung
			// chi so) nhung khong sinh thuoc tinh. Truoc day -1 roi
			// vao GetCMIT -> NULL -> break -> cat ngang danh sach.
			if (pnaryMALevel[i] < 0)
				continue;
			// [LOREN] Trang bi tim: con so la CHI SO DONG cua bang thuoc tinh,
			// da dac ta du ca thuoc tinh lan cap lan dai gia tri. Ghi thang,
			// khong qua GetCMIT (GetCMIT chi nhan cap 1..10 nen se tra NULL).
			// Duong dong bo tu may chu ve client khong mang pham chat xuong
			// (KItemSet::AddItemSet2 -> Gen_Equipment ban 10 tham so), nen nhan
			// dien them bang CHINH GIA TRI: cap cua JX1 chi chay 1..MATF_LEVEL,
			// con so lon hon the chac chan la chi so dong cua bang thuoc tinh.
			if (bChiSoDong)
			{
				if (i < MAX_ITEM_MAGICATTRIB &&
					sLoRenDichThuocTinh(nGameVersion, pnaryMALevel[i], &pnaryMA[i]))
					continue;
				// khong tra duoc thi bo qua o nay, dung cat ca danh sach
				continue;
			}
			SelectedMagicTable.Clear();

			// ¸ù¾ÝÇ°×ººó×º¡¢ÎïÆ·ÀàÐÍ¡¢ÎåÐÐÐèÇó¡¢ÐèÇó¼¶±ð½«ËùÓÐ·ûºÏÌõ¼þµÄÄ§·¨ÊôÐÔÏîÈ¡³öÀ´
			const KBPT_ClassMAIT* pCMITItem = m_BPTLib.GetCMIT(
				1 - (i & 1),        // Ç°×ººó×º
				nType,              // ÎïÆ·ÀàÐÍ
				nSeriesReq,         // ÎåÐÐÐèÇó 
				pnaryMALevel[i]     // ÐèÇó¼¶±ð
			);

			_ASSERT(pCMITItem);
			if (!pCMITItem)
				break;

			// ±éÀú·ûºÏÌõ¼þµÄÄ§·¨ÊôÐÔ£¬¸ù¾Ý
			// 1.ÊÇ·ñÒÑ¾­Ê¹ÓÃ£¬
			// 2.ÔËÆøÖµ
			// 3.ÊÇ·ñÓÐÏàÍ¬µÄ¡°ÊôÐÔµ÷ÕûÀà±ð¡±
			// È¡³öÏàÓ¦µÄÏî·ÅÔÚSelectedMagicTableÖÐ
			int nCMITItemCount = pCMITItem->GetCount();
			int j = 0;
			int nDecide = GetRandomNumber(0, 100 - 1) / (1 + nLucky * 20 / 100);

			for (j = 0; j < nCMITItemCount; j++)
			{
				// ¸ù¾ÝË÷ÒýÖµ£¬È¡³öÏàÓ¦µÄÏî
				int nMAIndex = pCMITItem->Get(j);
				KMAGICATTRIB_TABFILE* pMAItem = (KMAGICATTRIB_TABFILE*)m_BPTLib.GetMARecord(nMAIndex);
				if (!pMAItem)
				{
					_ASSERT(pMAItem);
					continue;
				}

				// Èç¹ûÒÑ¾­Ê¹ÓÃ£¬Ìø¹ýÈ¡ÏÂÒ»¸ö
				if (pMAItem->m_nUseFlag)
					continue;

				// Èç¹ûÔËÆø²»×ã£¬È¡ÏÂÒ»¸ö
				if ((pMAItem->m_DropRate[nType]) <= nDecide)
					continue;

				if (pMAItem->m_MagicAttrib.nPropKind == 135)
				{
					if (GetRandomNumber(0, 79) != 0)
						continue;
				}

				if (pMAItem->m_MagicAttrib.nPropKind == 114)
				{
					if (GetRandomNumber(0, 15) != 0)
						continue;
				}

				if (pMAItem->m_MagicAttrib.nPropKind == 115)
				{
					if (GetRandomNumber(0, 12) != 0)
						continue;
				}
				if (pMAItem->m_MagicAttrib.nPropKind == 136)
				{
					if (GetRandomNumber(0, 19) != 0)
						continue;
				}
				if (pMAItem->m_MagicAttrib.nPropKind == 137)
				{
					if (GetRandomNumber(0, 14) != 0)
						continue;
				}
				if (pMAItem->m_MagicAttrib.nPropKind == 106)
				{
					if (GetRandomNumber(0, 14) != 0)
						continue;
				}
				if (pMAItem->m_MagicAttrib.nPropKind == 104) // phong thu vat ly				
				{
					if (GetRandomNumber(0, 14) != 0)
						continue;
				}

				if (pMAItem->m_MagicAttrib.nPropKind == 106)
				{
					if (nLucky < 11 && nMAIndex > 237)
						continue;

					if (nLucky < 51 && nMAIndex > 238)
						continue;

					if (nLucky < 81 && nMAIndex > 239)
						continue;

				}

				if (pMAItem->m_MagicAttrib.nPropKind == 114)
				{
					if (nLucky < 4 && nMAIndex > 54)
						continue;

					if (nLucky < 11 && nMAIndex > 55)
						continue;

					if (nLucky < 21 && nMAIndex > 56)
						continue;

					if (nLucky < 31 && nMAIndex > 57)
						continue;

					if (nLucky < 51 && nMAIndex > 58)
						continue;

					if (nLucky < 61 && nMAIndex > 59)
						continue;

					if (nLucky < 71 && nMAIndex > 60)
						continue;

					if (nLucky < 81 && nMAIndex > 61)
						continue;

					if (nLucky < 91 && nMAIndex > 62)
						continue;

					if (nLucky < 111 && nMAIndex > 63)
						continue;

				}
				if (pMAItem->m_MagicAttrib.nPropKind == 104)
				{
					if (nLucky < 11 && nMAIndex > 180)
						continue;

					if (nLucky < 31 && nMAIndex > 181)
						continue;

					if (nLucky < 51 && nMAIndex > 182)
						continue;

					if (nLucky < 61 && nMAIndex > 183)
						continue;

					if (nLucky < 81 && nMAIndex > 184)
						continue;

				}
				if (pMAItem->m_MagicAttrib.nPropKind == 135)
				{
					if (nLucky < 4 && nMAIndex > 224)
						continue;

					if (nLucky < 11 && nMAIndex > 225)
						continue;

					if (nLucky < 21 && nMAIndex > 226)
						continue;

					if (nLucky < 31 && nMAIndex > 227)
						continue;

					if (nLucky < 51 && nMAIndex > 228)
						continue;

					if (nLucky < 61 && nMAIndex > 229)
						continue;

					if (nLucky < 71 && nMAIndex > 230)
						continue;

					if (nLucky < 81 && nMAIndex > 231)
						continue;

					if (nLucky < 91 && nMAIndex > 232)
						continue;

					if (nLucky < 111 && nMAIndex > 233)
						continue;

				}
				// ¼ì²éÊÇ·ñÒÑ¾­ÓÐÏàÍ¬µÄ¡°ÊôÐÔµ÷ÕûÀà±ð¡±
				// [LOREN] khe NULL = o kham nam rong, bo qua chu khong deref.
				// Dung co rieng vi `k < i` khong con nghia sau khi bo qua.
				int k = 0;
				BOOL bTrungThuocTinh = FALSE;
				for (k = 0; k < i; k++)
				{
					if (!pMagicAttrTable[k])
						continue;

					if ((pMagicAttrTable[k]->m_MagicAttrib.nPropKind) == (pMAItem->m_MagicAttrib.nPropKind))
					{
						bTrungThuocTinh = TRUE;
						break;
					}
				}

				// Èç¹û·¢ÏÖ ¡°ÊôÐÔµ÷ÕûÀà±ð¡±ÏàÍ¬£¬ËµÃ÷ÒÑ¾­ÓÐÒ»¸öÏàÍ¬µÄÖµ±»Ñ¡ÖÐ£¬ÄÇÃ´È¡ÏÂÒ»¸ö
				if (bTrungThuocTinh)
					continue;

				SelectedMagicTable.Insert(nMAIndex);
			}

			int nSelectedCount = SelectedMagicTable.GetCount();

			if (nSelectedCount == 0)
			{
				// Èç¹ûÃ»ÓÐÂú×ãÌõ¼þµÄÄ§·¨ÊôÐÔ£¬¾ÍÍË³ö
				break;
			}

			// Èç¹û·¢ÏÖÂú×ãÌõ¼þµÄÄ§·¨ÊôÐÔ£¬¾ÍÉèÖÃ
			int nLuckyItemIndex = GetRandomNumber(0, nSelectedCount - 1);
			KMAGICATTRIB_TABFILE* pMAItem = (KMAGICATTRIB_TABFILE*)m_BPTLib.GetMARecord(
				SelectedMagicTable.Get(nLuckyItemIndex)
			);

			// ±£´æÂú×ãÌõ¼þµÄÄ§·¨ÊôÐÔ£¬ÒÔ±ãÔÚ×îºóµÄÊ±ºò½«Ê¹ÓÃ±êÖ¾Çå³ý
			pMagicAttrTable[i] = pMAItem;

			_ASSERT(pMAItem);
			_ASSERT(!(pMAItem->m_nUseFlag));     // È·¶¨»¹Ã»ÓÐÊ¹ÓÃ

			// ÉèÖÃÊ¹ÓÃ±êÖ¾
			pMAItem->m_nUseFlag = true;

			pnaryMA[i].nAttribType = pMAItem->m_MagicAttrib.nPropKind;
			pnaryMA[i].nValue[0] = GetRandomNumber(
				pMAItem->m_MagicAttrib.aryRange[0].nMin,
				pMAItem->m_MagicAttrib.aryRange[0].nMax
			);
			pnaryMA[i].nValue[1] = GetRandomNumber(
				pMAItem->m_MagicAttrib.aryRange[1].nMin,
				pMAItem->m_MagicAttrib.aryRange[1].nMax
			);
			pnaryMA[i].nValue[2] = GetRandomNumber(
				pMAItem->m_MagicAttrib.aryRange[2].nMin,
				pMAItem->m_MagicAttrib.aryRange[2].nMax
			);

		} // for ±éÀúÐèÒªÈ¡»ØµÄÄ§·¨ÊôÐÔ±í
	}
	for (NULL; i < MAX_ITEM_MAGICATTRIB; i++)
	{
		// Çå¿ÕÊ£ÏÂµÄÏî
		pnaryMA[i].nAttribType = 0;
		pnaryMA[i].nValue[0] = 0;
		pnaryMA[i].nValue[1] = 0;
		pnaryMA[i].nValue[2] = 0;
	}

	for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
	{
		// [LOREN] khe NULL = o kham nam rong. Truoc day `break` o day
		// lam cac thuoc tinh PHIA SAU giu co "dang dung" vinh vien,
		// khien moi lan sinh trang bi ve sau deu bo qua chung.
		if (!pMagicAttrTable[i])
			continue;

		_ASSERT(pMagicAttrTable[i]->m_nUseFlag);   // È·¶¨ÒÑ¾­Ê¹ÓÃ

		pMagicAttrTable[i]->m_nUseFlag = false;    // ÉèÖÃÎªÃ»ÓÃÊ¹ÓÃ
	}

	nResult = true;
	//Exit0:
	return nResult;
}
BOOL KItemGenerator::GMA_GetAvaliableMA(int nType)
{
	for (int i = 0; i < 2; i++)		// 0: Ç°×º, 1: ºó×º
	{
		const KBPT_ClassifiedMAT* pCMAT;
		pCMAT = m_BPTLib.GetCMAT(i, nType);
		if (pCMAT == NULL)
		{
			_ASSERT(FALSE); return FALSE;
		}
		m_GMA_nCount[i] = NUMOFCMA;
		pCMAT->GetAll((int*)m_GMA_naryMA[i], &(m_GMA_nCount[i]));
	}
	return TRUE;
}

BOOL KItemGenerator::GMA_GetLevelAndDropRate(int nType)
{
	for (int i = 0; i < 2; i++)
	{
		for (int n = 0; n < m_GMA_nCount[i]; n++)
		{
			const KMAGICATTRIB_TABFILE* pMATF;
			pMATF = GetMARecord(m_GMA_naryMA[i][n]);
			if (pMATF == NULL)
			{
				_ASSERT(FALSE); return FALSE;
			}
			m_GMA_naryLevel[i][n] = pMATF->m_nLevel;
			m_GMA_naryDropRate[i][n] = pMATF->m_DropRate[nType];		
		}
	}
	return TRUE;
}

void KItemGenerator::GMA_ChooseMA(int nPos, int nLevel, int nLucky, KItemNormalAttrib* pINA)
{
	_ASSERT(pINA != NULL);

	pINA->nAttribType = 0;
	pINA->nValue[0] = 0;
	pINA->nValue[1] = 0;
	pINA->nValue[2] = 0;

	int nDropRate = ::GetRandomNumber(0, 100) - nLucky;
	int nCount = GMA_GetCandidateMA(nPos, nLevel, nDropRate);
	if (nCount > 0)
	{
		int nFinal = ::GetRandomNumber(0, nCount);
		int nMAi = m_GMA_naryCandidateMA[nFinal];
		int nMA = m_GMA_naryMA[nPos][nMAi];
		if (nMA == -1)	
		{
			pINA->nAttribType = 0;
			return;
		}
		m_GMA_naryMA[nPos][nMAi] = -1;	
		const KMAGICATTRIB_TABFILE* pMATF = GetMARecord(nMA);
		if (NULL != pMATF)
		{
			const KMACP* pMACP = &(pMATF->m_MagicAttrib);
			pINA->nAttribType = pMACP->nPropKind;
			pINA->nValue[0] = ::GetRandomNumber(pMACP->aryRange[0].nMin, pMACP->aryRange[0].nMax);
			pINA->nValue[1] = ::GetRandomNumber(pMACP->aryRange[1].nMin, pMACP->aryRange[1].nMax);
			pINA->nValue[2] = ::GetRandomNumber(pMACP->aryRange[2].nMin, pMACP->aryRange[2].nMax);
		}
	}
}

const KMAGICATTRIB_TABFILE* KItemGenerator::GetMARecord(int i) const
{
	return m_BPTLib.GetMARecord(i);
}

int KItemGenerator::GMA_GetCandidateMA(int nPos, int nLevel, int nDropRate)
{
	int nCount = 0;
	for (int i = 0; i < m_GMA_nCount[nPos]; i++)
	{
		if (m_GMA_naryMA[nPos][i] != -1 &&		
			m_GMA_naryLevel[nPos][i] == nLevel &&
			m_GMA_naryDropRate[nPos][i] >= nDropRate)
		{
			m_GMA_naryCandidateMA[nCount++] = i;
		}
	}
	return nCount;
}

BOOL KItemGenerator::GetMedicineCommonAttrib(IN int nDetailType, IN int nLevel, IN OUT KItem* pItem)
{
	if (NULL == pItem)
	{
		KASSERT(FALSE);
		return FALSE;
	}

	const int i = nDetailType * 5 + nLevel - 1;

	const KBASICPROP_MEDICINE* pMed = NULL;
	pMed = m_BPTLib.GetMedicineRecord(i);
	if (NULL == pMed)
	{
		_ASSERT(FALSE);
		return FALSE;
	}
	*pItem = *pMed;
	return TRUE;
}

BOOL KItemGenerator::GetGoldItemByRandom(IN int nCondition, OUT KItem * pItem)
{
	BOOL bResult = FALSE;
	const KBASICPROP_EQUIPMENT_GOLD* pArray[MAX_ITEM];
	int i = 0;
	int j = 0;
	int nIndex = 0;

	::memset(pArray, 0, sizeof(KBASICPROP_EQUIPMENT_GOLD *) * MAX_ITEM);

	for (i = 0; i < MAX_ITEM; i++)
	{
		const KBASICPROP_EQUIPMENT_GOLD* pTemp = m_BPTLib.GetGoldItemRecord(i);
		if (pTemp == 0)
			break;
/*		if (pTemp->m_nRarity < nCondition)*/
			pArray[j] = pTemp;
	}

	if (j == 0)
		goto Exit0;			 
	nIndex = GetRandomNumber(0, j);	
	*pItem = *pArray[nIndex];
Exit0:
	return FALSE;
}

BOOL KItemGenerator::GetGoldItemByIndex(IN int nIndex, OUT KItem * pItem, IN const int* pnay, IN int nSeries, IN int nEnChance)
{
	int i = 0;
	int j = 0;
	int nCount = 0;

#ifdef _SERVER	
	pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();
	if (pnay)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnay, sizeof(int) * 6);
	else
	{
		for (i = 0; i < MAX_ITEM_MAGICATTRIB;i++)
		{
			pItem->m_GeneratorParam.nGeneratorLevel[i] = ::GetRandomNumber(0,10);
		}
	}
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck = 10;
#else
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	if (pnay)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnay, sizeof(int) * 6);
	else
	{
		for (i = 0; i < MAX_ITEM_MAGICATTRIB;i++)
		{
			pItem->m_GeneratorParam.nGeneratorLevel[i] = ::GetRandomNumber(0,10);
		}
	}
#endif

	nCount = m_BPTLib.GetGoldItemNumber();
	const KBASICPROP_EQUIPMENT_GOLD* pTemp = m_BPTLib.GetGoldItemRecord(nIndex);
	if (nIndex >= nCount)
		return FALSE;
	if (pTemp == NULL)
		return FALSE;
	pItem->SetAttrib_CBR((KBASICPROP_EQUIPMENT_GOLD*)pTemp);
	if (nSeries > 4)
	{
		pItem->SetSeries(::GetRandomNumber(0,4));
	} 
	else
	{
		pItem->SetSeries(nSeries);
	}

	KItemNormalAttrib	sMA[MAX_ITEM_MAGICATTRIB];
	KTabFile MagicTab;
	MagicTab.Load(GOLD_EQUIP_MAGIC_FILE_O);
	
	for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
	{
		const int* pSrc;
		KItemNormalAttrib* pDst;
		pSrc = &(pTemp->m_aryMagicAttribs[i]);
		pDst = &(sMA[i]);
		
		int nType,nLow,nHigh;
		MagicTab.GetInteger(*pSrc + 1, 5,0,&nType);
		pDst->nAttribType = nType;
		MagicTab.GetInteger(*pSrc + 1, 6,0,&nLow);
		MagicTab.GetInteger(*pSrc + 1, 7,0,&nHigh);
		pDst->nValue[0] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		MagicTab.GetInteger(*pSrc + 1, 8,0,&nLow);
		MagicTab.GetInteger(*pSrc + 1, 9,0,&nHigh);
		pDst->nValue[1] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		MagicTab.GetInteger(*pSrc + 1, 10,0,&nLow);
		MagicTab.GetInteger(*pSrc + 1, 11,0,&nHigh);
		pDst->nValue[2] =  nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / 10);
		if (sMA[i].nAttribType <= 0)
		{
			sMA[i].nValue[0] = 0;
			sMA[i].nValue[1] = 0;
			sMA[i].nValue[2] = 0;
		}
	}
	MagicTab.Clear();
    for (NULL; i < MAX_ITEM_MAGICATTRIB; i++)
    {
        sMA[i].nAttribType = 0;
        sMA[i].nValue[0] = 0;
        sMA[i].nValue[1] = 0;
        sMA[i].nValue[2] = 0;
    }
	pItem->SetAttrib_MA(sMA);
	pItem->EnChance(nEnChance);

	return TRUE;
}

BOOL KItemGenerator::GetEquipmentCommonAttrib(IN int nDetailType, IN int nParticularType, IN int nLevel, IN int nSeries, IN OUT KItem* pItem)
{
	if (NULL == pItem)
	{
		KASSERT(FALSE);
		return FALSE;
	}

	const int i = nParticularType * 10 + nLevel - 1;

	const KBASICPROP_EQUIPMENT* pEqu = NULL;
	switch(nDetailType)
	{
	case equip_meleeweapon:
		pEqu = m_BPTLib.GetMeleeWeaponRecord(i);
		break;
	case equip_rangeweapon:
		pEqu = m_BPTLib.GetRangeWeaponRecord(i);
		break;
	case equip_armor:
		pEqu = m_BPTLib.GetArmorRecord(i);
		break;
	case equip_helm:
		pEqu = m_BPTLib.GetHelmRecord(i);
		break;
	case equip_boots:
		pEqu = m_BPTLib.GetBootRecord(i);
		break;
	case equip_belt:
		pEqu = m_BPTLib.GetBeltRecord(i);
		break;
	case equip_amulet:
		pEqu = m_BPTLib.GetAmuletRecord(i);
		break;
	case equip_ring:
		pEqu = m_BPTLib.GetRingRecord(i);
		break;
	case equip_cuff:
		pEqu = m_BPTLib.GetCuffRecord(i);
		break;
	case equip_pendant:
		pEqu = m_BPTLib.GetPendantRecord(i);
		break;
	case equip_horse:
		pEqu = m_BPTLib.GetHorseRecord(i);
		break;
	case equip_mask:	// mat na
		pEqu = m_BPTLib.GetMaskRecord(i);
		break;
	case equip_mantle:
		pEqu = m_BPTLib.GetMantleRecord(i);
		break;
	default:
		break;
	}
	if (NULL == pEqu)
	{
		_ASSERT(FALSE); return FALSE;
	}

	pItem->SetAttrib_CBR(pEqu);
	pItem->SetSeries(nSeries);
	return TRUE;
}

BOOL KItemGenerator::Gen_ExistEquipment(IN int nItemNature,
										IN int nDetailType,
										IN int nParticularType,
										IN int nSeriesReq,
										IN int nLevel,
										IN const int* pnaryMALevel,
										IN int nLucky,
										IN int nVersion,
										IN OUT KItem* pItem
)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);

	pItem->m_CommonAttrib.nItemNature = nItemNature;
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	switch (nItemNature)
	{
	case NATURE_GOLD:
	{
		this->Gen_GoldEquipment(nDetailType, pnaryMALevel, nLucky, pItem);
		pItem->SetRow(nDetailType);
		return TRUE;
	}
	break;
	case NATURE_PLATINA:
	{
		this->Gen_PlatinaEquipment(nDetailType, pnaryMALevel, nLucky, pItem, nLevel);
		pItem->SetRow(nDetailType);
		pItem->SetLevel(nLevel);
		return TRUE;
	}
	break;
	default:
	{
		BOOL bEC = FALSE;

		g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
		if (pnaryMALevel)
			memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
		int nGameVersion = pItem->m_GeneratorParam.nVersion;

		// ¸ù¾ÝÈë¿Ú²ÎÊý, È·¶¨×°±¸µÄ»ù±¾Êý¾Ý
		int i;
		if (nDetailType > equip_horse)
			i = nParticularType;
		else
			i = nParticularType * 10 + nLevel - 1;// ±¾¹«Ê½ÓÉ²ß»®¶¨Òå
		// Ïê¼ûSPE 2002Äê8ÔÂ31ÈÕ7:40
		// email: »ù´¡µÀ¾ßÊýÖµ±í
		// µÈ¼¶Êý¾Ý´Ó1¿ªÊ¼£¬ËùÒÔÒª¼õ»ØÀ´
		const KBASICPROP_EQUIPMENT* pEqu = NULL;
		switch (nDetailType)
		{
		case equip_meleeweapon:
			pEqu = m_BPTLib.GetMeleeWeaponRecord(i);
			break;
		case equip_rangeweapon:
			pEqu = m_BPTLib.GetRangeWeaponRecord(i);
			break;
		case equip_armor:
			pEqu = m_BPTLib.GetArmorRecord(i);
			break;
		case equip_helm:
			pEqu = m_BPTLib.GetHelmRecord(i);
			break;
		case equip_boots:
			pEqu = m_BPTLib.GetBootRecord(i);
			break;
		case equip_belt:
			pEqu = m_BPTLib.GetBeltRecord(i);
			break;
		case equip_amulet:
			pEqu = m_BPTLib.GetAmuletRecord(i);
			break;
		case equip_ring:
			pEqu = m_BPTLib.GetRingRecord(i);
			break;
		case equip_cuff:
			pEqu = m_BPTLib.GetCuffRecord(i);
			break;
		case equip_pendant:
			pEqu = m_BPTLib.GetPendantRecord(i);
			break;
		case equip_horse:
			pEqu = m_BPTLib.GetHorseRecord(i);
			break;
		case equip_mask:
			pEqu = m_BPTLib.GetMaskRecord(i);
			break;
		case equip_mantle:
			pEqu = m_BPTLib.GetMantleRecord(i);
			break;
		case equip_signet:
			pEqu = m_BPTLib.GetSignetRecord(i);
			break;
		case equip_shipin:
			pEqu = m_BPTLib.GetShipinRecord(i);
			break;
		case equip_hoods:
			pEqu = m_BPTLib.GetHoodsRecord(i);
			break;
		case equip_cloak:
			pEqu = m_BPTLib.GetCloakRecord(i);
			break;
		default:
			return bEC;
			break;
		}
		if (NULL == pEqu)
		{
			_ASSERT(FALSE); return bEC;
		}
		// ÔËÐÐÖÁ´ËµÄÖ±½ÓÔ­Òò: Ö»ÓÐnÖÖ×°±¸, ¶øÉÏÃæ i µÄÖµÔÚ[0,n-1]Ö®Íâ
		// ¼ì²é3µã: nParticularType ÓÐÎó?
		//			nLevel ÓÐÎó?
		//			Ô­Ê¼µÄtab fileÓÐÎóµ¼ÖÂ m_BPTLib.m_BPTEquipment Ëù
		//			¹ÜÀíµÄÊý¾ÝÓÐÎÊÌâ?
		pItem->SetAttrib_CBR(pEqu);
		if (nSeriesReq < series_metal)
			pItem->SetSeries(::GetRandomNumber(series_metal, series_earth));
		else
			pItem->SetSeries(nSeriesReq);
		const KSUITE_ACTIVE_COUNT* ActiveEquip = m_BPTLib.GetExtActiveSuiteRecord(pItem->GetGroup());
		if (ActiveEquip) {
			pItem->SetNeedToActive1(ActiveEquip->nActiveCount1);
			pItem->SetNeedToActive2(ActiveEquip->nActiveCount2);
		}
		else {
			pItem->SetNeedToActive1(-1);
			pItem->SetNeedToActive2(-1);
		}
		if (NULL == pnaryMALevel)
			return TRUE;
		KItemNormalAttrib	sMA[MAX_ITEM_MAGICATTRIB];	// µÀ¾ßµÄÄ§·¨ÊôÐÔ
		bEC = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion, nItemNature);
		if (bEC)
			pItem->SetAttrib_MA(sMA);

		pItem->SetNature(nItemNature);
		return bEC;
	}
	break;
	}
}

// [PHI PHONG 2026-08-29] sinh Tinh Than Thach theo ParticularType (1..34).
BOOL KItemGenerator::Gen_StarStone(IN int nParticular, IN OUT KItem* pItem, IN int nStackNum)
{
	_ASSERT(pItem != NULL);
	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	const KBASICPROP_STARSTONE* p = m_BPTLib.GetStarStoneRecord(nParticular);
	if (NULL == p)
		return FALSE;
	*pItem = *p;
	pItem->SetStackNum(nStackNum > 0 ? nStackNum : 1);
	return TRUE;
}

BOOL KItemGenerator::Gen_Quest(IN int nDetailType, IN OUT KItem* pItem,IN int nStackNum)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);

	BOOL bEC = FALSE;
	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	
	const KBASICPROP_QUEST* pQuest = NULL;
	pQuest = m_BPTLib.GetQuestRecord(nDetailType);
	if (NULL == pQuest)
	{
		_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pQuest;
	pItem->SetStackNum(nStackNum);
	return TRUE;
}

BOOL KItemGenerator::Gen_Mine(IN int nDetailType, IN OUT KItem* pItem,IN int nStackNum,IN int nLevel,IN int nSerises,IN int nMagicA)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);
	
	BOOL bEC = FALSE;
	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	
	const KBASICPROP_MINE* pQuest = NULL;
	pQuest = m_BPTLib.GetMine(nDetailType);
	if (NULL == pQuest)
	{
		_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pQuest;
	pItem->SetSeries(nSerises);
	pItem->SetStackNum(nStackNum);
	pItem->SetLevel(nLevel);
	if (nMagicA && pQuest->m_nParticularType >= 199 && pQuest->m_nParticularType <= 204)
	{
		pItem->m_GeneratorParam.nLuck = nMagicA;
		KEQCP_BASIC sBAS[6];
		ZeroMemory(sBAS,sizeof(sBAS));
		sBAS[0].nType = 0;
		sBAS[0].sRange.nMin = nMagicA;
		sBAS[0].sRange.nMax = nMagicA;
		pItem->SetAttrib_Base(sBAS);
	}
	return TRUE;
}

// [LOREN] Nguyen lieu cua he lo ren: chung co CAP va NGU HANH that, va
// khoang thuoc tinh con mang MA PHEP o nGeneratorLevel[0].
// Ban Linux dien ta bang mot cot rieng trong magicscript (cot 17
// "co khai niem cap"); bang cua JX1 khong co cot do nen phai liet ke.
//   6/1/146        Huyen Tinh Khoang Thach
//   6/1/199..204   sau khoang thuoc tinh
//   6/1/3769,3770  Huyen Kim va Huyen Kim Chi Tinh
static BOOL sLaNguyenLieuLoRen(int nDetailType, int nParticularType)
{
	if (nDetailType != 1)
		return FALSE;
	if (nParticularType == 146)
		return TRUE;
	// sau NGUYEN KHOANG - nguyen lieu cua thao tac tinh luyen thuoc tinh.
	// Chung cung nMaxStack = 50 nen cung dinh phep ep, va magic_distill
	// doc ngu hanh cua chinh chung; thieu day la ca kham nam lan Do pho
	// deu khong co nguyen lieu.
	if (nParticularType >= 148 && nParticularType <= 153)
		return TRUE;
	if (nParticularType >= 199 && nParticularType <= 204)
		return TRUE;
	if (nParticularType == 3769 || nParticularType == 3770)
		return TRUE;
	return FALSE;
}

BOOL KItemGenerator::Gen_MagicScript(IN int nDetailType, IN int nParticularType, IN OUT KItem* pItem, IN int nLevel, IN int nSeries, IN int nLuck, IN int nStackNum)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);
	
	BOOL bEC = FALSE;
	// [LOREN] Ma phep cua khoang nam o nGeneratorLevel[0]; xoa la mat.
	KItemGeneratorParam sGiuParam = pItem->m_GeneratorParam;
	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	if (sLaNguyenLieuLoRen(nDetailType, nParticularType))
		pItem->m_GeneratorParam = sGiuParam;
	
	const KBASICPROP_MAGICSCRIPT* pMagicScript = NULL;
	const int i = nParticularType;
	pMagicScript = m_BPTLib.GetMagicScript(i);
	if (NULL == pMagicScript)
	{
		_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pMagicScript;
	// [LOREN] Nguyen lieu lo ren: ngu hanh am nghia la CHUA CO, phai giu
	// nguyen. Boc ngau nhien o day thi so do se bi ghi xuong CSDL
	// (KPlayerDBFuns) va khong hoi lai duoc; moi lan dang nhap lai boc
	// mot so khac. Viec gan ngu hanh that de buoc di tru CSDL lam.
	if (nSeries < series_metal)
	{
		if (sLaNguyenLieuLoRen(nDetailType, nParticularType))
			pItem->SetSeries(nSeries);
		else
			pItem->SetSeries(::GetRandomNumber(series_metal, series_earth));
	}
	else
		pItem->SetSeries(nSeries);

	if(pMagicScript->m_nMaxStack) // xö lý xÕp chång nÕu lµ vËt phÈm xÕp chång kh«ng ph©n biÖt ngò hµnh xep chong
		if (!sLaNguyenLieuLoRen(nDetailType, nParticularType))
			pItem->SetSeries(-1);

	pItem->SetLevel(nLevel);

	if(pMagicScript->m_nMaxStack) // xö lý xÕp chång nÕu lµ vËt phÈm xÕp chång kh«ng ph©n biÖt level hµnh xep chong
		if (!sLaNguyenLieuLoRen(nDetailType, nParticularType))
			pItem->SetLevel(10);

	pItem->m_GeneratorParam.nLuck = nLuck;
	pItem->m_CommonAttrib.nGoldId = 0;
	pItem->SetStackNum(nStackNum);
	return TRUE;
}

BOOL KItemGenerator::Gen_TownPortal(IN OUT KItem* pItem)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);

	BOOL bEC = FALSE;
	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	
	const KBASICPROP_TOWNPORTAL* pPortal = NULL;
	pPortal = m_BPTLib.GetTownPortalRecord(0);
	if (NULL == pPortal)
	{
		_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pPortal;
	return TRUE;
}

/**********************************************************************************
Function: Generate random equipment
Entry: nDetailType: Specific category, such as melee weapons, hidden weapons, etc.
nParticularType: Detailed category
nSeriesReq: Five elements attributes
nLevel: Level
pnaryMALevel: Magic attribute level array [6]
nLucky: Luck value
Exit: Returns non-zero when successful, and the relevant data is given in the object pointed to by pItem
Returns zero when failed
******************************************************************************/
BOOL KItemGenerator::Gen_Equipment(IN int nItemNature,
	IN int nDetailType,
	IN int nParticularType,
	IN int nSeriesReq,
	IN int nLevel,
	IN const int* pnaryMALevel,
	IN int nLucky,
	IN int nVersion,
	IN OUT KItem* pItem)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);

	switch (nItemNature)
	{
	case NATURE_GOLD:
	{
		this->Gen_GoldEquipment(nDetailType, pnaryMALevel, nLucky, pItem);
		pItem->SetRow(nDetailType);
		return TRUE;
	}
	break;
	case NATURE_PLATINA:
	{
		this->Gen_PlatinaEquipment(nDetailType, pnaryMALevel, nLucky, pItem, nLevel);
		pItem->SetRow(nDetailType);
		pItem->SetLevel(nLevel);
		return TRUE;
	}
	break;
	default:
	{
		BOOL bEC = FALSE;
#ifdef _SERVER	// Server version
		pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();
		if (pnaryMALevel)
			memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
		else
			ZeroMemory(pItem->m_GeneratorParam.nGeneratorLevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
		pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
#else
		g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
		if (pnaryMALevel)
			memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
#endif
		pItem->m_GeneratorParam.nLuck = nLucky;
		int nGameVersion = pItem->m_GeneratorParam.nVersion;

		// According to the input parameters, determine the basic data of the equipment
		int i;
		if (nDetailType > equip_horse)
			i = nParticularType;
		else
			i = nParticularType * 10 + nLevel - 1;	// This formula is defined by the planner
													// For details, see SPE August 31, 2002 7:40
													// email: Basic item value table
													// Level data starts at 1, so it needs to be subtracted back
		const KBASICPROP_EQUIPMENT* pEqu = NULL;
		switch (nDetailType)
		{
		case equip_meleeweapon:
			pEqu = m_BPTLib.GetMeleeWeaponRecord(i);
			break;
		case equip_rangeweapon:
			pEqu = m_BPTLib.GetRangeWeaponRecord(i);
			break;
		case equip_armor:
			pEqu = m_BPTLib.GetArmorRecord(i);
			break;
		case equip_helm:
			pEqu = m_BPTLib.GetHelmRecord(i);
			break;
		case equip_boots:
			pEqu = m_BPTLib.GetBootRecord(i);
			break;
		case equip_belt:
			pEqu = m_BPTLib.GetBeltRecord(i);
			break;
		case equip_amulet:
			pEqu = m_BPTLib.GetAmuletRecord(i);
			break;
		case equip_ring:
			pEqu = m_BPTLib.GetRingRecord(i);
			break;
		case equip_cuff:
			pEqu = m_BPTLib.GetCuffRecord(i);
			break;
		case equip_pendant:
			pEqu = m_BPTLib.GetPendantRecord(i);
			break;
		case equip_horse:
			pEqu = m_BPTLib.GetHorseRecord(i);
			break;
		case equip_mask:
			pEqu = m_BPTLib.GetMaskRecord(i);
			break;
		case equip_mantle:
			pEqu = m_BPTLib.GetMantleRecord(i);
			break;
		case equip_signet:
			pEqu = m_BPTLib.GetSignetRecord(i);
			break;
		case equip_shipin:
			pEqu = m_BPTLib.GetShipinRecord(i);
			break;
		case equip_hoods:
			pEqu = m_BPTLib.GetHoodsRecord(i);
			break;
		case equip_cloak:
			pEqu = m_BPTLib.GetCloakRecord(i);
			break;
		default:
			return bEC;
			break;
		}
		if (NULL == pEqu)
		{
			_ASSERT(FALSE); return bEC;
		}
		// Direct reason for running to this point: there are only n types of equipment, and the value of i above is outside [0,n-1]
		// Check 3 points: nParticularType is wrong?
		// nLevel is wrong?
		// The original tab file is wrong, causing the data managed by m_BPTLib.m_BPTEquipment to be wrong?
		pItem->SetAttrib_CBR(pEqu);
		if (nSeriesReq < series_metal)
			pItem->SetSeries(::GetRandomNumber(series_metal, series_earth));
		else
			pItem->SetSeries(nSeriesReq);
		const KSUITE_ACTIVE_COUNT* ActiveEquip = m_BPTLib.GetExtActiveSuiteRecord(pItem->GetGroup());
		// [VA 29/08 - dump 17:48 va 17:50] GetExtActiveSuiteRecord tra NULL khi
		// bang ext_suite rong, va cho nay doc thang ActiveEquip->... -> sap
		// 0xC0000005 doc dia chi 0x4 (chinh la lenh CoreServer.dll+0x16982A).
		// Hai cho goi con lai (dong 1533 va 1922) VON DA kiem NULL; lay -1 cho
		// giong khuon dong 1533 vi cung la duong trang bi thuong.
		if (ActiveEquip)
		{
			pItem->SetNeedToActive1(ActiveEquip->nActiveCount1);
			pItem->SetNeedToActive2(ActiveEquip->nActiveCount2);
		}
		else
		{
			pItem->SetNeedToActive1(-1);
			pItem->SetNeedToActive2(-1);
		}
		if (NULL == pnaryMALevel)
			return TRUE;

		KItemNormalAttrib	sMA[MAX_ITEM_MAGICATTRIB];
		bEC = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion, nItemNature);
		if (bEC)
			pItem->SetAttrib_MA(sMA);

		pItem->SetNature(nItemNature);
		return bEC;
	}
	break;
	}
}

BOOL KItemGenerator::Gen_GoldEquipment(IN int nIndex, IN const int* pnaryMALevel, IN int nLucky, OUT KItem* pItem)
{
	int i = 0;
	int j = 0;
	int nCount = 0;

	int nLuckMin = (int)LOWORD(nLucky);
	int nLuckMax = (int)HIWORD(nLucky);

	if (nLuckMin > MAX_ITEM_LUCK)
		nLuckMin = MAX_ITEM_LUCK;

	if (nLuckMin > nLuckMax)
		nLuckMin = nLuckMax;

	if (nLuckMax > MAX_ITEM_LUCK)
		nLuckMax = MAX_ITEM_LUCK;

#ifdef _SERVER	// ·þÎñÆ÷°æ±¾
	pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
#else
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
#endif
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
	else
	{
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			pItem->m_GeneratorParam.nGeneratorLevel[i] = nLuckMin == nLuckMax ? nLuckMax : GetRandomNumber(nLuckMin, nLuckMax);
			pItem->m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB] = 0;
		}
	}
	pItem->m_GeneratorParam.nLuck = nLucky;

	nCount = m_BPTLib.GetGoldEquipNumber();
	const KBASICPROP_EQUIPMENT_GOLD2* pTemp = m_BPTLib.GetGoldEquipRecord(nIndex);
	if (nIndex >= nCount)
		return FALSE;
	if (pTemp == NULL)
		return FALSE;
	pItem->SetAttrib_CBR((KBASICPROP_EQUIPMENT_GOLD2*)pTemp);
	PF_ApplyRowInfo(FALSE, nIndex, pItem);	// [PHI PHONG] nap cot 60/61/62
	int setNum = m_BPTLib.GetActiveSuiteCount(pTemp->m_nGroup); //get number items in set
	pItem->SetSetNum(setNum); //return nActiveCount1 => setNum
	//pItem->SetSetId(setNum);

	const KSUITE_ACTIVE_COUNT* ActiveEquip = m_BPTLib.GetExtActiveSuiteRecord(pTemp->m_nGroup);
	
	if (ActiveEquip != NULL) {
		pItem->SetNeedToActive1(ActiveEquip->nActiveCount1);
		pItem->SetNeedToActive2(ActiveEquip->nActiveCount2);
	}
	else {
		pItem->SetNeedToActive1(0);
		pItem->SetNeedToActive2(0);
	}
	

	KItemNormalAttrib	sMA[MAX_ITEM_MAGICATTRIB];

	if (pItem->m_GeneratorParam.nGeneratorLevel[MAX_ITEM_MAGICATTRIB]) //custom magic already set 
	{
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[i] <= 0)
				break;
			sMA[i].nAttribType = pItem->m_GeneratorParam.nGeneratorLevel[i];
			sMA[i].nValue[0] = LOWORD(pItem->m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB]);
			sMA[i].nValue[1] = -1;
			sMA[i].nValue[2] = HIWORD(pItem->m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB]);
		}
	}
	else // load default value from config file
	{
		KTabFile MagicTab;
		if (!MagicTab.Load(GOLD_EQUIP_MAGIC_FILE_N))
			return FALSE;

		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			const int* pSrc = &(pTemp->m_aryMagicAttribs[i]);
			KItemNormalAttrib* pDst = &(sMA[i]);

			int nType, nLow, nHigh;
			MagicTab.GetInteger(*pSrc + 1, 5, 0, &nType);
			pDst->nAttribType = nType;

			for (int j = 0; j < 3; ++j)
			{
				int colLow = 6 + j * 2;
				int colHigh = colLow + 1;

				MagicTab.GetInteger(*pSrc + 1, colLow, 0, &nLow);
				MagicTab.GetInteger(*pSrc + 1, colHigh, 0, &nHigh);

				if (nHigh > nLow)
					pDst->nValue[j] = nLow + GetRandomNumber(0, nHigh - nLow);
				else
					pDst->nValue[j] = nLow;
			}

			if (pDst->nAttribType <= 0)
			{
				pDst->nValue[0] = 0;
				pDst->nValue[1] = 0;
				pDst->nValue[2] = 0;
			}
		}
		MagicTab.Clear();
	}
	for (NULL; i < MAX_ITEM_MAGICATTRIB; i++)
	{
		// Çå¿ÕÊ£ÏÂµÄÏî
		sMA[i].nAttribType = 0;
		sMA[i].nValue[0] = 0;
		sMA[i].nValue[1] = 0;
		sMA[i].nValue[2] = 0;
	}
	pItem->SetAttrib_MA(sMA);

	return TRUE;
}
int GetMagicIdxWithHighestRate(const KPLATINA_MAGICRATE* pMagicRate) {
	if (!pMagicRate) return -1; // Return an invalid index if pointer is null

	int highestRate = pMagicRate->m_nRate1;
	int highestIdx = pMagicRate->m_nMagicIdx1;

	if (pMagicRate->m_nRate2 > highestRate) {
		highestRate = pMagicRate->m_nRate2;
		highestIdx = pMagicRate->m_nMagicIdx2;
	}

	if (pMagicRate->m_nRate3 > highestRate) {
		highestRate = pMagicRate->m_nRate3;
		highestIdx = pMagicRate->m_nMagicIdx3;
	}

	return highestIdx;
}

BOOL KItemGenerator::Gen_PlatinaEquipment(IN int nIndex, IN const int* pnaryMALevel, IN int nLucky, OUT KItem* pItem, IN int nLevel)
{
	int i = 0;
	int j = 0;
	int nCount = 0;
	const float SCALE_PER_LEVEL = 0.05f;

	int nLuckMin = (int)LOWORD(nLucky);
	int nLuckMax = (int)HIWORD(nLucky);

	if (nLuckMin > MAX_ITEM_LUCK)
		nLuckMin = MAX_ITEM_LUCK;

	if (nLuckMin > nLuckMax)
		nLuckMin = nLuckMax;

	if (nLuckMax > MAX_ITEM_LUCK)
		nLuckMax = MAX_ITEM_LUCK;

#ifdef _SERVER	// ·þÎñÆ÷°æ±¾
	pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
#else
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
#endif
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
	else
	{
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			pItem->m_GeneratorParam.nGeneratorLevel[i] = nLuckMin == nLuckMax ? nLuckMax : GetRandomNumber(nLuckMin, nLuckMax);
			pItem->m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB] = 0;
		}
	}
	pItem->m_GeneratorParam.nLuck = nLucky;

	nCount = m_BPTLib.GetPlatinaEquipNumber();
	KBASICPROP_EQUIPMENT_PLATINA* pTemp = (KBASICPROP_EQUIPMENT_PLATINA*)m_BPTLib.GetPlatinaEquipRecord(nIndex);
	if (nIndex >= nCount)
		return FALSE;
	if (pTemp == NULL)
		return FALSE;
	//magic_durability_v,						//31
	if (nLevel > 5) {
		for (i = 0; i < 7; i++)
			if (pTemp->m_aryPropBasic[i].nType == magic_durability_v) {
				pTemp->m_aryPropBasic[i].sRange.nMin *= 10;
				pTemp->m_aryPropBasic[i].sRange.nMax *= 10;
			}
	}
	pItem->SetAttrib_CBR((KBASICPROP_EQUIPMENT_PLATINA*)pTemp); //Set Base & Require Attributes
	PF_ApplyRowInfo(TRUE, nIndex, pItem);	// [PHI PHONG] nap cot 60/61/62
	//reset durability
	if (nLevel > 5) {
		for (i = 0; i < 7; i++)
			if (pTemp->m_aryPropBasic[i].nType == magic_durability_v) {
				pTemp->m_aryPropBasic[i].sRange.nMin /= 10;
				pTemp->m_aryPropBasic[i].sRange.nMax /= 10;
			}
	}
	int setNum = m_BPTLib.GetActiveSuiteCount(pTemp->m_nGroup); //get number items in set
	pItem->SetSetNum(setNum); //return nActiveCount1 => setNum
	pItem->SetSetId(pTemp->m_nGroup);
	//pItem->SetSetId(setNum);

	const KSUITE_ACTIVE_COUNT* ActiveEquip = m_BPTLib.GetExtActiveSuiteRecord(pTemp->m_nGroup);
	// [VA 29/08] cung lo hong nhu dong 1856; lay 0 cho giong khuon dong 1922
	// (Gen_GoldEquipment) vi cung la duong trang bi cao cap.
	if (ActiveEquip)
	{
		pItem->SetNeedToActive1(ActiveEquip->nActiveCount1);
		pItem->SetNeedToActive2(ActiveEquip->nActiveCount2);
	}
	else
	{
		pItem->SetNeedToActive1(0);
		pItem->SetNeedToActive2(0);
	}
	KItemNormalAttrib	sMA[MAX_ITEM_MAGICATTRIB];

	if (pItem->m_GeneratorParam.nGeneratorLevel[MAX_ITEM_MAGICATTRIB])
	{
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[i] <= 0)
				break;
			sMA[i].nAttribType = pItem->m_GeneratorParam.nGeneratorLevel[i];
			sMA[i].nValue[0] = LOWORD(pItem->m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB]);
			sMA[i].nValue[1] = -1;
			sMA[i].nValue[2] = HIWORD(pItem->m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB]);
		}
	}
	else
	{
		KTabFile MagicTab;
		if (!MagicTab.Load(GOLD_EQUIP_MAGIC_FILE_N))
			return FALSE;

		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			const int* pSrc = &(pTemp->m_aryMagicAttribs_0[i]);
			KItemNormalAttrib* pDst = &(sMA[i]);

			int nType;
			if (!MagicTab.GetInteger(*pSrc + 1, 5, 0, &nType) || nType <= 0)
				break;
			pDst->nAttribType = nType;

			for (int j = 0; j < 3; ++j)
			{
				int colLow = 6 + j * 2;
				int colHigh = colLow + 1;
				int nLow = 0, nHigh = 0;

				MagicTab.GetInteger(*pSrc + 1, colLow, 0, &nLow);
				MagicTab.GetInteger(*pSrc + 1, colHigh, 0, &nHigh);

				// --- 1) Roll Gold value once ---
				int goldVal = (nHigh > nLow)
					? nLow + GetRandomNumber(0, nHigh - nLow)
					: nLow;

				if (nLevel == 0)
				{
					// exact Gold behavior at L0
					pDst->nValue[j] = goldVal;
				}
				else
				{
					// --- 2) Compute its ratio in [nLow..nHigh] ---
					float ratio = (nHigh > nLow)
						? (float)(goldVal - nLow) / (float)(nHigh - nLow)
						: 0.0f;

					float scale = 1.0f;
					float scaledMin = nLow * scale;
					float scaledMax = nHigh * scale;
					// --- 3) Scale min/max by level (exponential) ---
					if (nType != magic_attackspeed_v && nType != magic_castspeed_v && nType != magic_allskill_v) {
						scale = 1.0f + SCALE_PER_LEVEL * nLevel;
						scaledMin = nLow * scale;
						scaledMax = nHigh * scale;
					}

					// --- 4) Map ratio to [scaledMin..scaledMax] and round ---
					float fVal = scaledMin + ratio * (scaledMax - scaledMin);
					pDst->nValue[j] = (int)(fVal + 0.5f);
				}
			}
		}
		MagicTab.Clear();
	}

	for (NULL; i < MAX_ITEM_MAGICATTRIB; i++)
	{
		// Çå¿ÕÊ£ÏÂµÄÏî
		sMA[i].nAttribType = 0;
		sMA[i].nValue[0] = 0;
		sMA[i].nValue[1] = 0;
		sMA[i].nValue[2] = 0;
	}
	i = 6;
	// chinh level xuat hien 2 dong thuoc tinh 7 8
	if (nLevel >= 6) {
		const KPLATINA_MAGICRATE* pMagicRate = m_BPTLib.GetPlatinaMagicRateFindRecord(nIndex, nLevel-1, 1); //skill no 1
		if (pMagicRate) {
			int tmpRate = GetMagicIdxWithHighestRate(pMagicRate)-1;
			const KPLATINA_MAGICATTR* pMagicAttr = m_BPTLib.GetPlatinaMagicAttrRecord(tmpRate);
			if (pMagicAttr) {
				sMA[i].nAttribType = pMagicAttr->m_nAttributeType;
				sMA[i].nValue[0] = pMagicAttr->m_nValue1;
				sMA[i].nValue[1] = pMagicAttr->m_nValue2;
				sMA[i].nValue[2] = pMagicAttr->m_nValue3;
				i++;
			}
		}
	}
	if (nLevel >= 6) {
		const KPLATINA_MAGICRATE* pMagicRate = m_BPTLib.GetPlatinaMagicRateFindRecord(nIndex, nLevel-1, 2);
		if (pMagicRate) {
			int tmpRate = GetMagicIdxWithHighestRate(pMagicRate) - 1;
			const KPLATINA_MAGICATTR* pMagicAttr = m_BPTLib.GetPlatinaMagicAttrRecord(tmpRate);
			if (pMagicAttr) {
				sMA[i].nAttribType = pMagicAttr->m_nAttributeType;
				sMA[i].nValue[0] = pMagicAttr->m_nValue1;
				sMA[i].nValue[1] = pMagicAttr->m_nValue2;
				sMA[i].nValue[2] = pMagicAttr->m_nValue3;
				i++;
			}
		}
	}

	pItem->SetAttrib_MA(sMA);

	return TRUE;
}

BOOL KItemGenerator::UpgradePlatinaEquip(int nVersion, KItem* pItem) {
	int i = 0;
	KItemNormalAttrib	sMA[MAX_ITEM_MAGICATTRIB];
	const float SCALE_PER_LEVEL = 0.05f;
	int nLevel = 0;
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	if (pItem->GetNature() == NATURE_GOLD) {
		pItem->SetNature(NATURE_PLATINA);
		pItem->SetLevel(0); // reset level to 0 when upgrading from gold to platina
	}
	else {
		nLevel = pItem->GetLevel() + 1;
		if (nLevel > 10)
			return FALSE;
		pItem->SetLevel(nLevel);
	}
	//recalc all item magic
	KBASICPROP_EQUIPMENT_PLATINA* pTemp = (KBASICPROP_EQUIPMENT_PLATINA*)m_BPTLib.GetPlatinaEquipRecord(pItem->GetIndex());
	if (pTemp == NULL)
		return FALSE;
	//magic_durability_v,						//31
	if (nLevel > 5) {
		for (i = 0; i < 7; i++)
			if (pItem->m_aryBaseAttrib[i].nAttribType == magic_durability_v) {
				pItem->m_aryBaseAttrib[i].nValue[0] *= 10;
			}
	}

	if (pItem->m_GeneratorParam.nGeneratorLevel[MAX_ITEM_MAGICATTRIB])
	{
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[i] <= 0)
				break;
			sMA[i].nAttribType = pItem->m_GeneratorParam.nGeneratorLevel[i];
			sMA[i].nValue[0] = LOWORD(pItem->m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB]);
			sMA[i].nValue[1] = -1;
			sMA[i].nValue[2] = HIWORD(pItem->m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB]);
		}
	}
	else
	{
		KTabFile MagicTab;
		if (!MagicTab.Load(GOLD_EQUIP_MAGIC_FILE_N))
			return FALSE;

		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			const int* pSrc = &(pTemp->m_aryMagicAttribs_0[i]);
			KItemNormalAttrib* pDst = &sMA[i];

			int nType;
			if (!MagicTab.GetInteger(*pSrc + 1, 5, 0, &nType) || nType <= 0)
				break;
			pDst->nAttribType = nType;

			for (int j = 0; j < 3; ++j)
			{
				int colLow = 6 + j * 2;
				int colHigh = colLow + 1;
				int nLow = 0, nHigh = 0;

				MagicTab.GetInteger(*pSrc + 1, colLow, 0, &nLow);
				MagicTab.GetInteger(*pSrc + 1, colHigh, 0, &nHigh);

				// --- 1) Roll Gold value once ---
				int goldVal = (nHigh > nLow)
					? nLow + GetRandomNumber(0, nHigh - nLow)
					: nLow;

				if (nLevel == 0)
				{
					// exact Gold behavior at L0
					pDst->nValue[j] = goldVal;
				}
				else
				{
					// --- 2) Compute its ratio in [nLow..nHigh] ---
					float ratio = (nHigh > nLow)
						? (float)(goldVal - nLow) / (float)(nHigh - nLow)
						: 0.0f;

					float scale = 1.0f;
					float scaledMin = nLow * scale;
					float scaledMax = nHigh * scale;
					// --- 3) Scale min/max by level (exponential) ---
					if (nType != magic_attackspeed_v && nType != magic_castspeed_v && nType != magic_allskill_v) {
						scale = 1.0f + SCALE_PER_LEVEL * nLevel;
						scaledMin = nLow * scale;
						scaledMax = nHigh * scale;
					}
					// --- 4) Map ratio to [scaledMin..scaledMax] and round ---
					float fVal = scaledMin + ratio * (scaledMax - scaledMin);
					pDst->nValue[j] = (int)(fVal + 0.5f);
				}
			}
		}
		MagicTab.Clear();
	}

	for (NULL; i < MAX_ITEM_MAGICATTRIB; i++)
	{
		// Çå¿ÕÊ£ÏÂµÄÏî
		sMA[i].nAttribType = 0;
		sMA[i].nValue[0] = 0;
		sMA[i].nValue[1] = 0;
		sMA[i].nValue[2] = 0;
	}
	i = 6;
	// chinh level xuat hien 2 dong thuoc tinh 7 8
	if (nLevel >= 6) {
		const KPLATINA_MAGICRATE* pMagicRate = m_BPTLib.GetPlatinaMagicRateFindRecord(pItem->GetIndex(), nLevel-1, 1); //skill no 1
		if (pMagicRate) {
			int tmpRate = GetMagicIdxWithHighestRate(pMagicRate) - 1;
			const KPLATINA_MAGICATTR* pMagicAttr = m_BPTLib.GetPlatinaMagicAttrRecord(tmpRate);
			if (pMagicAttr) {
				sMA[i].nAttribType = pMagicAttr->m_nAttributeType;
				sMA[i].nValue[0] = pMagicAttr->m_nValue1;
				sMA[i].nValue[1] = pMagicAttr->m_nValue2;
				sMA[i].nValue[2] = pMagicAttr->m_nValue3;
				i++;
			}
		}
	}
	if (nLevel >= 6) {
		const KPLATINA_MAGICRATE* pMagicRate = m_BPTLib.GetPlatinaMagicRateFindRecord(pItem->GetIndex(), nLevel-1, 2);
		if (pMagicRate) {
			int tmpRate = GetMagicIdxWithHighestRate(pMagicRate) - 1;
			const KPLATINA_MAGICATTR* pMagicAttr = m_BPTLib.GetPlatinaMagicAttrRecord(tmpRate);
			if (pMagicAttr) {
				sMA[i].nAttribType = pMagicAttr->m_nAttributeType;
				sMA[i].nValue[0] = pMagicAttr->m_nValue1;
				sMA[i].nValue[1] = pMagicAttr->m_nValue2;
				sMA[i].nValue[2] = pMagicAttr->m_nValue3;
				i++;
			}
		}
	}
	pItem->SetAttrib_MA(sMA);

	return TRUE;
}