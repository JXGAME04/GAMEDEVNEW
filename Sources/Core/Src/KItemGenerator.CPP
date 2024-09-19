//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
// File:	KItemGenerator.CPP
// Date:	2020.08.26
// Code:	Fong Ki“u
//---------------------------------------------------------------------------
#include "KEngine.h"
#include "KCore.h"
#include "MyAssert.H"
#include "KItem.h"
#include "KPlayer.h"
#include "KSubWorldSet.h"
#include "KItemGenerator.h"

KItemGenerator	ItemGen;			

KItemGenerator::KItemGenerator()
{
}

KItemGenerator::~KItemGenerator()
{
}

BOOL KItemGenerator::Init()
{
	if (!m_BPTLib.Init())		// ¥Àµ˜”√¥”»Ù∏…µƒtab file÷–‘ÿ»ÎÀ˘”–µƒ≥ı º Ù–‘
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
	pItem->m_CommonAttrib.nParticularType = nParticularType; //edit by Fong Ki“u mÌi th™m vµo skill shortcut
#ifndef _SERVER
	if(nDetailType == 13 && nParticularType > 0) //item skill
	{
		int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(nParticularType);
		if(nLevel > 0)
			nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nParticularType);

		ISkill * pISkill = g_SkillManager.GetSkill(nParticularType, nLevel);
		if(pISkill)
			strcpy(pItem->m_Image.szImage, pISkill->GetSkillIcon()); //Vœ lπi icon item skill l y icon tı skill qua
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
#ifdef _SERVER	// ∑˛ŒÒ∆˜∞Ê±æ
	pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * 6);
	else
		ZeroMemory(pItem->m_GeneratorParam.nGeneratorLevel, sizeof(int) * 6);
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck = nLucky;
#else
	pItem->m_GeneratorParam.nLuck = nLucky;//edit by phong kieu fix khong get dc nluck tπi client d…n Æ’n sai option ÆÂ xanh chatitem
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * 6);
#endif
	int nGameVersion = pItem->m_GeneratorParam.nVersion;

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
		_ASSERT(FALSE); 
		return bEC; 
	}

	pItem->SetAttrib_CBR(pEqu);
	pItem->SetSeries(nSeriesReq);

	if (nPoint == 0)
	{
		if (NULL == pnaryMALevel)
			return TRUE;
		KItemNormalAttrib	sMA[6];	
		bEC = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);
		if (bEC)
			pItem->SetAttrib_MA(sMA);
	}
	else
	{
		KItemNormalAttrib sMA[6];
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
		for (int j = 0;j < 6;j++)
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

BOOL KItemGenerator::GMA_GetAvaliableMA(int nType)
{
	for (int i = 0; i < 2; i++)		// 0: «∞◊∫, 1: ∫Û◊∫
	{
		const KBPT_ClassifiedMAT* pCMAT;
		pCMAT = m_BPTLib.GetCMAT(i, nType);
		if (pCMAT == NULL)
			{ _ASSERT(FALSE); return FALSE; }
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
				{ _ASSERT(FALSE); return FALSE; }
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
	MagicTab.Load(GOLD_EQUIP_MAGIC_FILE_N);
	
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
		{ _ASSERT(FALSE); return FALSE; }

	pItem->SetAttrib_CBR(pEqu);
	pItem->SetSeries(nSeries);
	return TRUE;
}

BOOL KItemGenerator::Gen_ExistEquipment(IN int nDetailType,
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

	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	if (pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * 6);
	int nGameVersion = pItem->m_GeneratorParam.nVersion;

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
		_ASSERT(FALSE); 
		return bEC; 
	}

	pItem->SetAttrib_CBR(pEqu);
	pItem->SetSeries(nSeriesReq);

	if (nPoint == 0)
	{
		if (NULL == pnaryMALevel)
		return TRUE;
		KItemNormalAttrib	sMA[6];	
		bEC = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);
		if (bEC)
			pItem->SetAttrib_MA(sMA);
	}
	else
	{
		KItemNormalAttrib sMA[6];
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
		pItem->m_GeneratorParam.nLuck = nLucky;
		int nType,nMax,nMin;
		for (int j = 0;j < 6;j++)
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

BOOL KItemGenerator::Gen_MagicScript(IN int nDetailType, IN int nParticularType, IN OUT KItem* pItem, IN int nLevel, IN int nSeries, IN int nLuck, IN int nStackNum)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);
	
	BOOL bEC = FALSE;
	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	
	const KBASICPROP_MAGICSCRIPT* pMagicScript = NULL;
	const int i = nParticularType;
	pMagicScript = m_BPTLib.GetMagicScript(i);
	if (NULL == pMagicScript)
	{
		_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pMagicScript;
	if (nSeries < series_metal)
		pItem->SetSeries(::GetRandomNumber(series_metal, series_earth));
	else
		pItem->SetSeries(nSeries);

	if(pMagicScript->m_nMaxStack) // xˆ l˝ x’p chÂng n’u lµ vÀt ph»m x’p chÂng kh´ng ph©n bi÷t ngÚ hµnh xep chong
		pItem->SetSeries(-1);

	pItem->SetLevel(nLevel);

	if(pMagicScript->m_nMaxStack) // xˆ l˝ x’p chÂng n’u lµ vÀt ph»m x’p chÂng kh´ng ph©n bi÷t level hµnh xep chong
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
