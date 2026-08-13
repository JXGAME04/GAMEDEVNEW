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
BOOL KItemGenerator::Gen_MagicAttrib(
	int nType, const int* pnaryMALevel, int nSeriesReq, int nLucky,
	KItemNormalAttrib* pnaryMA, int nGameVersion
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
		for (i = 0; i < MAX_ITEM_MAGICLEVEL; i++)
		{
			// Èç¹û·¢ÏÖ¼¶±ðÎªÁã£¬±íÊ¾²»ÐèÒª¼ÌÐø´¦ÀíÁË£¬ÍË³ö
			if (pnaryMALevel[i] == 0)
				break;
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
				int k = 0;
				for (k = 0; k < i; k++)
				{
					_ASSERT(pMagicAttrTable[k]);

					if ((pMagicAttrTable[k]->m_MagicAttrib.nPropKind) == (pMAItem->m_MagicAttrib.nPropKind))
						break;
				}

				// Èç¹û·¢ÏÖ ¡°ÊôÐÔµ÷ÕûÀà±ð¡±ÏàÍ¬£¬ËµÃ÷ÒÑ¾­ÓÐÒ»¸öÏàÍ¬µÄÖµ±»Ñ¡ÖÐ£¬ÄÇÃ´È¡ÏÂÒ»¸ö
				if (k < i)
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
		if (!pMagicAttrTable[i])
			break;

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
		bEC = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);
		if (bEC)
			pItem->SetAttrib_MA(sMA);

		pItem->SetNature(nItemNature);
		return bEC;
	}
	break;
	}
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

	if(pMagicScript->m_nMaxStack) // xö lý xÕp chång nÕu lµ vËt phÈm xÕp chång kh«ng ph©n biÖt ngò hµnh xep chong
		pItem->SetSeries(-1);

	pItem->SetLevel(nLevel);

	if(pMagicScript->m_nMaxStack) // xö lý xÕp chång nÕu lµ vËt phÈm xÕp chång kh«ng ph©n biÖt level hµnh xep chong
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
		pItem->SetNeedToActive1(ActiveEquip->nActiveCount1);
		pItem->SetNeedToActive2(ActiveEquip->nActiveCount2);
		if (NULL == pnaryMALevel)
			return TRUE;

		KItemNormalAttrib	sMA[MAX_ITEM_MAGICATTRIB];
		bEC = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);
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
	pItem->SetNeedToActive1(ActiveEquip->nActiveCount1);
	pItem->SetNeedToActive2(ActiveEquip->nActiveCount2);
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