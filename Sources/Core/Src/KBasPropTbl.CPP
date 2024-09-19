//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
// Date:	2020.08.14
// Code:	Fong KiÒu
// Desc:    KBasPropTbl.cpp file
//---------------------------------------------------------------------------

#include "KCore.h"
#include "KTabFile.h"
#include "MyAssert.H"
#include "KBasPropTbl.h"

#define		TABFILE_PATH				"\\settings\\item"
#define		TABFILE_MINE				"mine.txt"
#define		TABFILE_TASK				"questkey.txt"
#define		TABFILE_MEDICINE			"potion.txt"
#define		TABFILE_MEDMATERIAL			"medmaterialbase.txt"
#define		TABFILE_MELEEWEAPON			"MeleeWeapon.txt"
#define		TABFILE_RANGEWEAPON			"RangeWeapon.txt"
#define		TABFILE_ARMOR				"Armor.txt"
#define		TABFILE_HELM				"Helm.txt"
#define		TABFILE_BOOT				"Boot.txt"
#define		TABFILE_BELT				"Belt.txt"
#define		TABFILE_AMULET				"Amulet.txt"
#define		TABFILE_RING				"Ring.txt"
#define		TABFILE_CUFF				"Cuff.txt"
#define		TABFILE_PENDANT				"Pendant.txt"
#define		TABFILE_HORSE				"Horse.txt"
#define		TABFILE_TOWNPORTAL			"TownPortal.txt"
#define		TABFILE_MAGICSCRIPT			"Magicscript.txt"
#define		TABFILE_EQUIPMENT_UNIQUE	"unique.txt"
#define		TABFILE_MAGICATTRIB						"magicattrib.txt"
#define		TABFILE_GOLDITEM_O			"GoldItem.txt"
#define		TABFILE_GOLDITEM_N			"goldequip.txt"
#define		TABFILE_MASK				"Mask.txt"	// mat na
#define		TABFILE_MANTLE				"Mantle.txt"//#phi phong

typedef struct tagPROPINFO
{
	int		m_nType;		//PI_VARTYPE_
	union
	{
	char*	m_pszBuf;		
	int*	m_pnData;		
	}m_pData;
	int		m_nBufSize;		
} PROPINFO;
#define		PI_VARTYPE_CHAR		0
#define		PI_VARTYPE_INT		1

char*	TABFILE_EQUIPMENT[] = 
{
	TABFILE_MELEEWEAPON,	//"MeleeWeapon.txt",
	TABFILE_RANGEWEAPON,	//"RangeWeapon.txt",
	TABFILE_ARMOR,			//"Armor.txt",
	TABFILE_HELM,			//"Helm.txt",
	TABFILE_BOOT,			//"Boot.txt",
	TABFILE_BELT,			//"Belt.txt",
	TABFILE_AMULET,			//"Amulet.txt",
	TABFILE_RING,			//"Ring.txt",
	TABFILE_CUFF,			//"Cuff.txt",
	TABFILE_PENDANT,		//"Pendant.txt",
	TABFILE_HORSE,			//"Horse.txt",
	TABFILE_MASK,			//"Mask.txt"	// mat na
	TABFILE_MANTLE,		//"Mantle.txt" #phi phong
};

//int GetRandomNumber(int nMin, int nMax);
BOOL LoadRecord(IN KTabFile* pTF, IN int nRow, IN OUT const PROPINFO* pPI, IN int cbFields)
{
	BOOL bEC = TRUE;
	nRow += 2;

	for (int n = 0; n < cbFields; n++)
	{
		if (PI_VARTYPE_INT == (pPI+n)->m_nType)
		{	
			if (FALSE == pTF->GetInteger(nRow, n+1, -1, (pPI+n)->m_pData.m_pnData))
				{ _ASSERT(FALSE); bEC = FALSE; break; }
		}
		else
		{	
			_ASSERT(PI_VARTYPE_CHAR == (pPI+n)->m_nType);

			if (FALSE == pTF->GetString(nRow, n+1, "", (pPI+n)->m_pData.m_pszBuf, (pPI+n)->m_nBufSize))
				{ _ASSERT(FALSE); bEC = FALSE; break; }
		}
	}
	return bEC;
}

KLibOfBPT::KLibOfBPT()
{
}

KLibOfBPT::~KLibOfBPT()
{
}

BOOL KLibOfBPT::Init()
{
	KBasicPropertyTable*	paryBPT[] = {	
											&m_BPTMeleeWeapon,
											&m_BPTRangeWeapon,
											&m_BPTArmor,
											&m_BPTHelm,
											&m_BPTBoot,
											&m_BPTBelt,
											&m_BPTAmulet,
											&m_BPTRing,
											&m_BPTCuff,
											&m_BPTPendant,
											&m_BPTHorse,
											&m_BPTMask,	// mat na
											&m_BPTMantle,	//#phi phong
											&m_GoldItem,
											&m_BPTMedicine,
											&m_BPTQuest,
											&m_BPTTownPortal,
											&m_BPTMagicScript,
											&m_BPTMine,
											&m_BPTMagicAttrib,
										};

	const int cbNumOfTables = sizeof(paryBPT)/sizeof(paryBPT[0]);
	for (int i = 0; i < cbNumOfTables; i++)
	{
		if (i < 13)	
		{
			KBPT_Equipment* pTemp = (KBPT_Equipment *)paryBPT[i];
			pTemp->Init(i);
		}
		else if (i == 13)
		{
			KBPT_Equipment_Gold* pGTemp = (KBPT_Equipment_Gold *)paryBPT[i];
			pGTemp->Init();
		}

		if (FALSE == paryBPT[i]->Load())
			{ _ASSERT(FALSE); return FALSE; }
	}

	InitMALib();
    
    InitMAIT();

	return TRUE;
}

BOOL KLibOfBPT::InitMAIT()
{
    int nResult = false;
    int i = 0;
    int nPrefixPostfix = 0;
    int nType = 0;
    int nSeries = 0;
    int nSeriesMin = 0;
    int nSeriesMax = 0;

    int nLevel  = 0;
    int nLevelMin = 0;
    int nLevelMax = 0;

    int nTotalCount = 0;

    for (i = 0; i < m_BPTMagicAttrib.NumOfEntries(); i++)
    {
        KMAGICATTRIB_TABFILE *pItem = (KMAGICATTRIB_TABFILE *)m_BPTMagicAttrib.GetRecord(i);
        if (!pItem)
        {
            _ASSERT(pItem);
            continue;
        }

        pItem->m_nUseFlag = false;

        nPrefixPostfix = pItem->m_nPos;
        _ASSERT((nPrefixPostfix >= 0) && (nPrefixPostfix < MATF_PREFIXPOSFIX));

        for (nType = 0; nType < MATF_CBDR; nType++)
        {
            if ((pItem->m_DropRate[nType]) == 0)
                continue; 
            
            nSeriesMin = nSeriesMax = pItem->m_nClass;
            if ((pItem->m_nClass) == -1)
            {
                nSeriesMin = 0;
                nSeriesMax = MATF_SERIES - 1;
            }
            else
            {
                _ASSERT(((pItem->m_nClass) >= 0) && ((pItem->m_nClass) < MATF_SERIES));
            }

            for (nSeries = nSeriesMin; nSeries <= nSeriesMax; nSeries++)
            {
                nLevelMin = pItem->m_nLevel;
                nLevelMax = MATF_LEVEL;

                for (nLevel = nLevelMin; nLevel <= nLevelMax; nLevel++)
                {  
                    m_CMAIT[nPrefixPostfix][nType][nSeries][nLevel - 1].Insert(i);
                    nTotalCount++;
                }
            }
        }
    }

    //#ifdef _DEBUG
    #if 0
    //g_DebugLog("%s %d", pNpc->Name, pMagic->nValue[0]);
    printf("m_CMAIT \n");
    for (nPrefixPostfix = 0; nPrefixPostfix < MATF_PREFIXPOSFIX; nPrefixPostfix++)
    {
        for (nType = 0; nType < MATF_CBDR; nType++)
        {
            for (nSeries = 0; nSeries < MATF_SERIES; nSeries++)
            {
                for (nLevel = 1; nLevel < (MATF_LEVEL + 1); nLevel++)
                {
                    KBPT_ClassMAIT *pMAITItem = &m_CMAIT[nPrefixPostfix][nType][nSeries][nLevel - 1];

                    char szOutputString[8192];
                    char szStringContent[4096];
                    szStringContent[0] = '\0';

                    for (i = 0; i < pMAITItem->GetCount(); i++)
                    {
                        KMAGICATTRIB_TABFILE *pItem = (KMAGICATTRIB_TABFILE *)m_BPTMagicAttrib.GetRecord(pMAITItem->Get(i));
                        char szTemp[1024];
                        sprintf(szTemp, " %3d(%-8s) ", pMAITItem->Get(i) + 2, pItem->m_szName);

                        strcat(szStringContent, szTemp);
                    }

                    sprintf(szOutputString, 
                        "[Ä§·¨ÊôÐÔ]m_CMAIT[%d][%d][%d][%d]: Count = %3d, %s  \n",
                        nPrefixPostfix,
                        nType,
                        nSeries,
                        nLevel - 1,
                        pMAITItem->GetCount(),
                        szStringContent
                    );
                    printf("%s", szOutputString);
                    //g_DebugLog("%s", szOutputString);
                    OutputDebugString(szOutputString);
                }
            }
        }
    }
    ExitProcess(0); // for redirect to File save

    #endif // _DEBUG

    nResult = true;
//Exit0:
    return nResult;
}

BOOL KLibOfBPT::InitMALib()
{
	BOOL bEC = FALSE;

    int naryMACount[2][MATF_CBDR];	// µÚiÖÖ×°±¸¹²ÓÐnaryMACount[0][i]ÖÖ¿ÉÄÜµÄÇ°×º
	m_BPTMagicAttrib.GetMACount((int*)naryMACount);

	for (int i = 0; i < MATF_CBDR - 1; i++)	// -1 because of horse
	{
		_ASSERT(naryMACount[0][i] > 0);	// ²»¿ÉÄÜÃ»ÓÐÒ»¸öÄ§·¨Ç°×ºÊÊÓÃÓÚ¸Ã×°±¸
		_ASSERT(naryMACount[1][i] > 0);	// ²»¿ÉÄÜÃ»ÓÐÒ»¸öÄ§·¨ºó×ºÊÊÓÃÓÚ¸Ã×°±¸
		if (FALSE == m_CMAT[0][i].GetMemory(naryMACount[0][i]))
			return bEC;
		if (FALSE == m_CMAT[1][i].GetMemory(naryMACount[1][i]))
			return bEC;
	}

	const int nNumOfMA = m_BPTMagicAttrib.NumOfEntries();	// Ä§·¨ÊôÐÔµÄ×ÜÊý
	_ASSERT(nNumOfMA > 0);
	int m, n;		// µÚmÏîÄ§·¨ÊôÐÔÊÊÓÃÓÚµÚnÖÖ×°±¸
	for (m = 0; m < nNumOfMA; m++)				// ±éÀúËùÓÐÄ§·¨ÊôÐÔ
	{
		const KMAGICATTRIB_TABFILE* pRec;
		pRec = m_BPTMagicAttrib.GetRecord(m);		
		for (n = 0; n < MATF_CBDR; n++)			// ¹²ÓÐMATF_CBDRÖÖ×°±¸´øÄ§·¨
		{	// È·ÈÏÄ§·¨ÊôÐÔÊÊÓÃÓÚÄÄÐ©×°±¸
			if (0 != pRec->m_DropRate[n])
			{	// ÔËÐÐÖÁ´Ë, ËµÃ÷µÚmÏîÄ§·¨ÊôÐÔÊÊÓÃÓÚµÚnÖÖ×°±¸
				int nPos;
				nPos = (0 == pRec->m_nPos) ? 1 : 0;
				m_CMAT[nPos][n].Set(m);
			}
		}
	}
	bEC = TRUE;
	return bEC;
}

// flying add here
const KBASICPROP_EQUIPMENT_GOLD* KLibOfBPT::GetGoldItemRecord(IN int nIndex) const
{
	return (KBASICPROP_EQUIPMENT_GOLD*)m_GoldItem.GetRecord(nIndex);
}

const int KLibOfBPT::GetGoldItemNumber() const
{
	return m_GoldItem.GetRecordCount();
}

const KBPT_ClassifiedMAT* KLibOfBPT::GetCMAT(int nPos, int i) const
{
	_ASSERT(this != NULL);

	if (nPos != 0 && nPos != 1)
		{ _ASSERT(FALSE); return NULL; }
	if (i < 0 || i >= MATF_CBDR)
		{ _ASSERT(FALSE); return NULL; }
	return &(m_CMAT[nPos][i]);
}

// Add by Freeway Chen in 2003.5.30
const KBPT_ClassMAIT*       KLibOfBPT::GetCMIT(IN int nPrefixPostfix, IN int nType, IN int nSeries, int nLevel) const
{
    _ASSERT((nPrefixPostfix >= 0) && (nPrefixPostfix < MATF_PREFIXPOSFIX));
    if (!((nPrefixPostfix >= 0) && (nPrefixPostfix < MATF_PREFIXPOSFIX)))
        return NULL;

    _ASSERT((nType >= 0) && (nType < MATF_CBDR));
    if (!((nType >= 0) && (nType < MATF_CBDR)))
        return NULL;

    _ASSERT((nSeries >= 0) && (nSeries < MATF_SERIES));
    if (!((nSeries >= 0) && (nSeries < MATF_SERIES)))
        return NULL;

    _ASSERT(((nLevel - 1) >= 0) && ((nLevel - 1) < MATF_LEVEL));    // nLevel is from 1..MATF_LEVEL
    if (!(((nLevel - 1) >= 0) && ((nLevel - 1) < MATF_LEVEL)))
        return NULL;

    return &m_CMAIT[nPrefixPostfix][nType][nSeries][nLevel - 1];
}

const KMAGICATTRIB_TABFILE* KLibOfBPT::GetMARecord(IN int i) const
{
	return m_BPTMagicAttrib.GetRecord(i);
}

const int KLibOfBPT::GetMARecordNumber() const
{
	return m_BPTMagicAttrib.NumOfEntries();
}

const KBASICPROP_EQUIPMENT* KLibOfBPT::GetMeleeWeaponRecord(IN int i) const
{
	return m_BPTMeleeWeapon.GetRecord(i);
}

const int KLibOfBPT::GetMeleeWeaponRecordNumber() const
{
	return m_BPTMeleeWeapon.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetRangeWeaponRecord(IN int i) const
{
	return m_BPTRangeWeapon.GetRecord(i);
}

const int KLibOfBPT::GetRangeWeaponRecordNumber() const
{
	return m_BPTRangeWeapon.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetArmorRecord(IN int i) const
{
	return m_BPTArmor.GetRecord(i);
}

const int KLibOfBPT::GetArmorRecordNumber() const
{
	return m_BPTArmor.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetHelmRecord(IN int i) const
{
	return m_BPTHelm.GetRecord(i);
}

const int KLibOfBPT::GetHelmRecordNumber() const
{
	return m_BPTHelm.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetBootRecord(IN int i) const
{
	return m_BPTBoot.GetRecord(i);
}

const int KLibOfBPT::GetBootRecordNumber() const
{
	return m_BPTBoot.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetBeltRecord(IN int i) const
{
	return m_BPTBelt.GetRecord(i);
}

const int KLibOfBPT::GetBeltRecordNumber() const
{
	return m_BPTBelt.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetAmuletRecord(IN int i) const
{
	return m_BPTAmulet.GetRecord(i);
}

const int KLibOfBPT::GetAmuletRecordNumber() const
{
	return m_BPTAmulet.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetRingRecord(IN int i) const
{
	return m_BPTRing.GetRecord(i);
}

const int KLibOfBPT::GetRingRecordNumber() const
{
	return m_BPTRing.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetCuffRecord(IN int i) const
{
	return m_BPTCuff.GetRecord(i);
}

const int KLibOfBPT::GetCuffRecordNumber() const
{
	return m_BPTCuff.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetPendantRecord(IN int i) const
{
	return m_BPTPendant.GetRecord(i);
}

const int KLibOfBPT::GetPendantRecordNumber() const
{
	return m_BPTPendant.NumOfEntries();
}

const KBASICPROP_EQUIPMENT* KLibOfBPT::GetHorseRecord(IN int i) const
{
	return m_BPTHorse.GetRecord(i);
}

const int KLibOfBPT::GetHorseRecordNumber() const
{
	return m_BPTHorse.NumOfEntries();
}

const KBASICPROP_MEDICINE* KLibOfBPT::GetMedicineRecord(IN int i) const
{
	return m_BPTMedicine.GetRecord(i);
}

const int KLibOfBPT::GetMedicineRecordNumber() const
{
	return m_BPTMedicine.NumOfEntries();
}

const KBASICPROP_QUEST* KLibOfBPT::GetQuestRecord(IN int i) const
{
	return m_BPTQuest.GetRecord(i);
}

const int KLibOfBPT::GetQuestRecordNumber() const
{
	return m_BPTQuest.NumOfEntries();
}

const KBASICPROP_TOWNPORTAL* KLibOfBPT::GetTownPortalRecord(IN int i) const
{
	return m_BPTTownPortal.GetRecord(i);
}

const int KLibOfBPT::GetTownPortalRecordNumber() const
{
	return m_BPTTownPortal.NumOfEntries();
}

const KBASICPROP_MAGICSCRIPT* KLibOfBPT::GetMagicScript(IN int i) const
{
	return m_BPTMagicScript.GetRecord(i);
}

const int KLibOfBPT::GetMagicScriptRecordNumber() const
{
	return m_BPTMagicScript.NumOfEntries();
}

const KBASICPROP_EQUIPMENT* KLibOfBPT::GetMaskRecord(IN int i) const // mat na
{
	return m_BPTMask.GetRecord(i);
}

const int KLibOfBPT::GetMaskRecordNumber() const
{
	return m_BPTMask.NumOfEntries();
}

const KBASICPROP_EQUIPMENT* KLibOfBPT::GetMantleRecord(IN int i) const
{
	return m_BPTMantle.GetRecord(i);
}

const int KLibOfBPT::GetMantleRecordNumber() const
{
	return m_BPTMantle.NumOfEntries();
}

const int KLibOfBPT::GetMineRecordNumber() const
{
	return m_BPTMine.NumOfEntries();
}

const KBASICPROP_MINE* KLibOfBPT::GetMine(IN int i) const
{
	return m_BPTMine.GetRecord(i);
}

KBasicPropertyTable::KBasicPropertyTable()
{
	m_pBuf = NULL;
	m_nNumOfEntries = 0;
	m_nSizeOfEntry = 0;
	m_szTabFile[0] = 0;
}

KBasicPropertyTable::~KBasicPropertyTable()
{
	ReleaseMemory();
}

void KBasicPropertyTable::SetCount(int cbCount)
{
	_ASSERT(cbCount>0);
	_ASSERT(0==m_nNumOfEntries);	// this function is supposed to be called only once
	m_nNumOfEntries = cbCount;
}

BOOL KBasicPropertyTable::GetMemory()
{
	_ASSERT(NULL == m_pBuf);
	_ASSERT(m_nNumOfEntries > 0 && m_nSizeOfEntry > 0);

	BOOL bEC = FALSE;
	const int nMemSize = m_nSizeOfEntry * m_nNumOfEntries;
	void* pBuf = new BYTE[nMemSize];
	_ASSERT(pBuf != NULL);
	if (pBuf != NULL)
	{
		m_pBuf = pBuf;
		bEC = TRUE;
	}
	return bEC;
}

void KBasicPropertyTable::ReleaseMemory()
{
	if (m_pBuf)
	{
		delete []m_pBuf;
		m_pBuf = NULL;
		m_nNumOfEntries = 0;
	}
}

/******************************************************************************
m_szTabFile
m_nNumOfEntries
******************************************************************************/
BOOL KBasicPropertyTable::Load()
{
	BOOL bEC = FALSE;
	KTabFile	theLoader;
	//g_SetRootPath(NULL);
//	::g_SetFilePath(TABFILE_PATH);
	char	szFileName[FILE_NAME_LENGTH];
	g_UnitePathAndName(TABFILE_PATH, m_szTabFile, szFileName);
	if (FALSE == theLoader.Load(szFileName))
		{ _ASSERT(FALSE); return bEC; }

	const int cbItems = theLoader.GetHeight() - 1;	
	if (cbItems <= 0)							
		{ _ASSERT(FALSE); return bEC; }
	SetCount(cbItems);

	if (FALSE == GetMemory())
		{ _ASSERT(FALSE); return bEC; }

	int i;
	for (i = 0; i < cbItems; i++)
	{
		if (FALSE == LoadRecord(i, &theLoader))
			{ _ASSERT(FALSE); return bEC; }
	}

	bEC = TRUE;
	return bEC;
}

KBPT_Mine::KBPT_Mine()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_MINE);
	::strcpy(m_szTabFile, TABFILE_MINE);
}

KBPT_Mine::~KBPT_Mine()
{
}

BOOL KBPT_Mine::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	KBASICPROP_MINE* pBuf = (KBASICPROP_MINE*)m_pBuf;
	pBuf = pBuf + i;	// ¶ÁÈëµÄÊôÐÔ¼ÇÔÚ pBuf ËùÖ¸½á¹¹ÖÐ
	const PROPINFO	aryPI[] =
	{
/*ver1****************************************************************
		{ PI_VARTYPE_CHAR,			pBuf->m_szSerialNum, sizeof(pBuf->m_szSerialNum)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nStyle), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nClass), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImage1, sizeof(pBuf->m_szImage1)},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImage2, sizeof(pBuf->m_szImage2)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_bRepeatable), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSort), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nLevel), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nValue), 0},
*********************************************************************/
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nParticularType), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSeries), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nLevel), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_bStack), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szScript, sizeof(pBuf->m_szScript)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_Magic[0]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_Magic[1]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_Magic[2]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_Magic[3]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_Magic[4]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDelet), 0},
	};

	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

const KBASICPROP_MINE* KBPT_Mine::GetRecord(int i) const
{
	_ASSERT(this != NULL);

	if (-1 == i)
		i = ::GetRandomNumber(0, m_nNumOfEntries-1);

	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_MINE*)m_pBuf) + i) : NULL;
}

const KBASICPROP_MINE* KBPT_Mine::FindRecord(IN int nType,IN int nLevel) const
{
	const KBASICPROP_MINE* pData = NULL;
	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_MINE* pMed;
		pMed = GetRecord(i);
		_ASSERT(NULL != pMed);
		if (nType == pMed->m_nDetailType && nLevel == pMed->m_nLevel)
		{
			//_ASSERT(nType == pMed->m_nAttrib1_Type);
			pData = pMed;
			break;
		}
	}
	return pData;
}

KBPT_MagicScript::KBPT_MagicScript()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_MAGICSCRIPT);
	::strcpy(m_szTabFile, TABFILE_MAGICSCRIPT);
}

KBPT_MagicScript::~KBPT_MagicScript()
{
}

BOOL KBPT_MagicScript::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	KBASICPROP_MAGICSCRIPT* pBuf = (KBASICPROP_MAGICSCRIPT*)m_pBuf;
	pBuf = pBuf + i;
	const PROPINFO aryPI[] = //load theo thø tù cét trong file settings/item/filename.txt
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nParticurType), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_CHAR,			pBuf->m_szScript, sizeof(pBuf->m_szScript)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_bShortKey), 0},	//bá xuèng « phÝm t¾t
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nMaxStack), 0},	//xÕp chång tèi ®a
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPickExecute), 0},//add by phong kiÒu item sö dông ngay
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nParam), 0},//sè lÇn sö dông item 
	};

	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

const KBASICPROP_MAGICSCRIPT* KBPT_MagicScript::GetRecord(IN int i) const
{
	_ASSERT(this != NULL);

	if (-1 == i)
		i = ::GetRandomNumber(0, m_nNumOfEntries-1);

	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_MAGICSCRIPT*)m_pBuf) + i) : NULL;
}

const KBASICPROP_MAGICSCRIPT* KBPT_MagicScript::FindRecord(IN int nType) const
{
	const KBASICPROP_MAGICSCRIPT* pData = NULL;
	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_MAGICSCRIPT* pMed;
		pMed = GetRecord(i);
		_ASSERT(NULL != pMed);
		if (nType == pMed->m_nParticurType)
		{
			pData = pMed;
			break;
		}
	}
	return pData;
}

KBPT_TownPortal::KBPT_TownPortal()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_TOWNPORTAL);
	::strcpy(m_szTabFile, TABFILE_TOWNPORTAL);
}

KBPT_TownPortal::~KBPT_TownPortal()
{
}

BOOL KBPT_TownPortal::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	KBASICPROP_TOWNPORTAL* pBuf = (KBASICPROP_TOWNPORTAL*)m_pBuf;
	pBuf = pBuf + i;			// ¶ÁÈëµÄÊôÐÔ¼ÇÔÚ pBuf ËùÖ¸½á¹¹ÖÐ
	const PROPINFO aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
	};

	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

const KBASICPROP_TOWNPORTAL* KBPT_TownPortal::GetRecord(IN int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_TOWNPORTAL*)m_pBuf) + i) : NULL;
}

KBPT_Quest::KBPT_Quest()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_QUEST);
	::strcpy(m_szTabFile, TABFILE_TASK);
}

KBPT_Quest::~KBPT_Quest()
{
}

BOOL KBPT_Quest::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	KBASICPROP_QUEST* pBuf = (KBASICPROP_QUEST*)m_pBuf;
	pBuf = pBuf + i;			// ¶ÁÈëµÄÊôÐÔ¼ÇÔÚ pBuf ËùÖ¸½á¹¹ÖÐ
	const PROPINFO aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_bShortKey), 0}, //bá xuèng « phÝm t¾t
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_bStack), 0},
	};

	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

const KBASICPROP_QUEST* KBPT_Quest::GetRecord(IN int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_QUEST*)m_pBuf) + i) : NULL;
}

const KBASICPROP_QUEST* KBPT_Quest::FindRecord(IN int nDetailType) const
{
	const KBASICPROP_QUEST* pData = NULL;
	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_QUEST* pQst;
		pQst = GetRecord(i);
		_ASSERT(NULL != pQst);
		if (nDetailType == pQst->m_nDetailType)
		{
//			_ASSERT(nType == pMed->m_nAttrib1_Type);
			pData = pQst;
			break;
		}
	}
	return pData;
}

KBPT_Medicine::KBPT_Medicine()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_MEDICINE);
	::strcpy(m_szTabFile, TABFILE_MEDICINE);
}

KBPT_Medicine::~KBPT_Medicine()
{

}

BOOL KBPT_Medicine::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	KBASICPROP_MEDICINE* pBuf = (KBASICPROP_MEDICINE*)m_pBuf;
	pBuf = pBuf + i;
	const PROPINFO	aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nParticularType), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSeries), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nLevel), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_bStack), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryAttrib[0].nAttrib), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryAttrib[0].nValue), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryAttrib[0].nTime), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryAttrib[1].nAttrib), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryAttrib[1].nValue), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryAttrib[1].nTime), 0},
	};

	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

const KBASICPROP_MEDICINE* KBPT_Medicine::GetRecord(int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_MEDICINE*)m_pBuf) + i) : NULL;
}

const KBASICPROP_MEDICINE* KBPT_Medicine::FindRecord(IN int nType, IN int nLevel) const
{
	const KBASICPROP_MEDICINE* pData = NULL;

	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_MEDICINE* pMed;
		pMed = GetRecord(i);
		_ASSERT(NULL != pMed);
		if (nType == pMed->m_nDetailType && nLevel == pMed->m_nLevel)
		{
//			_ASSERT(nType == pMed->m_nAttrib1_Type);
			pData = pMed;
			break;
		}
	}
	return pData;
}

KBPT_MedMaterial::KBPT_MedMaterial()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_MEDMATERIAL);
	::strcpy(m_szTabFile, TABFILE_MEDMATERIAL);
}

KBPT_MedMaterial::~KBPT_MedMaterial()
{

}

BOOL KBPT_MedMaterial::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	KBASICPROP_MEDMATERIAL* pBuf = (KBASICPROP_MEDMATERIAL*)m_pBuf;
	pBuf = pBuf + i;	// ¶ÁÈëµÄÊôÐÔ¼ÇÔÚ pBuf ËùÖ¸½á¹¹ÖÐ
	const PROPINFO	aryPI[] =
	{
/*ver1****************************************************************
		{ PI_VARTYPE_CHAR,			pBuf->m_szSerialNum, sizeof(pBuf->m_szSerialNum)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nStyle), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImage1, sizeof(pBuf->m_szImage1)},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImage2, sizeof(pBuf->m_szImage2)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_bRepeatable), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSort1), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSort2), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nLevel), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nValue), 0},
*********************************************************************/
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nParticularType), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSeries), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nLevel), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_bStack), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nAttrib1_Type), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nAttrib1_Para), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nAttrib2_Type), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nAttrib2_Para), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nAttrib3_Type), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nAttrib3_Para), 0},
	};

	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

const KBASICPROP_MEDMATERIAL* KBPT_MedMaterial::GetRecord(int i) const
{
	_ASSERT(this != NULL);

	if (-1 == i)
		i = ::GetRandomNumber(0, m_nNumOfEntries-1);

	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_MEDMATERIAL*)m_pBuf) + i) : NULL;
}

KBPT_Equipment::KBPT_Equipment()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_EQUIPMENT);
}

KBPT_Equipment::~KBPT_Equipment()
{
}

void KBPT_Equipment::Init(IN int i)
{
	::strcpy(m_szTabFile, TABFILE_EQUIPMENT[i]);
}

BOOL KBPT_Equipment::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	KBASICPROP_EQUIPMENT* pBuf = (KBASICPROP_EQUIPMENT*)m_pBuf;
	pBuf = pBuf + i;	// ¶ÁÈëµÄÊôÐÔ¼ÇÔÚ pBuf ËùÖ¸½á¹¹ÖÐ
	const PROPINFO	aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nParticularType), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSeries), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nLevel), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_bStack), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[0].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[0].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[1].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[1].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[2].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[2].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[3].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[3].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[4].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[4].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[5].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[5].nPara), 0},
	};

	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

const KBASICPROP_EQUIPMENT* KBPT_Equipment::GetRecord(int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_EQUIPMENT*)m_pBuf) + i) : NULL;
}

const KBASICPROP_EQUIPMENT* KBPT_Equipment::FindRecord( IN int nDetailType, IN int nParticularType, IN int nLevel) const
{
	_ASSERT(this != NULL);
	
	const KBASICPROP_EQUIPMENT* pData = NULL;

	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_EQUIPMENT* pEqu;
		pEqu = GetRecord(i);
		_ASSERT(NULL != pEqu);
		if (nDetailType == pEqu->m_nDetailType &&
			nParticularType == pEqu->m_nParticularType &&
			nLevel == pEqu->m_nLevel)
		{
			pData = pEqu;
			break;
		}
	}
	return pData;
}

KBPT_Equipment_Gold::KBPT_Equipment_Gold()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_EQUIPMENT_GOLD);
}

KBPT_Equipment_Gold::~KBPT_Equipment_Gold()
{
	return;
}

BOOL KBPT_Equipment_Gold::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	KBASICPROP_EQUIPMENT_GOLD* pBuf = (KBASICPROP_EQUIPMENT_GOLD*)m_pBuf;
	pBuf = pBuf + i;
	const PROPINFO	aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nParticularType), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSeries), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nLevel), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_bStack), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].sRange.nMax), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[0].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[0].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[1].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[1].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[2].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[2].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[3].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[3].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[4].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[4].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[5].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[5].nPara), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs[0]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs[1]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs[2]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs[3]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs[4]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs[5]), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nId), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSet), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSetNum), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nUpSet), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSetId), 0},
	};

	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

const KBASICPROP_EQUIPMENT_GOLD* KBPT_Equipment_Gold::GetRecord(int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_EQUIPMENT_GOLD*)m_pBuf) + i) : NULL;
}

const KBASICPROP_EQUIPMENT_GOLD*
		KBPT_Equipment_Gold::FindRecord(IN int nDetailType,
										  IN int nParticularType,
										  IN int nLevel) const
{
	_ASSERT(this != NULL);
	
	const KBASICPROP_EQUIPMENT_GOLD* pData = NULL;
	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_EQUIPMENT_GOLD* pEqu;
		pEqu = GetRecord(i);
		_ASSERT(NULL != pEqu);
		if (NULL == pEqu)
			continue;
		if (nDetailType == pEqu->m_nDetailType &&
			nParticularType == pEqu->m_nParticularType &&
			nLevel == pEqu->m_nLevel)
		{
			pData = pEqu;
			break;
		}
	}
	return pData;
}

void KBPT_Equipment_Gold::Init()
{
	::strcpy(m_szTabFile, TABFILE_GOLDITEM_O);
}

KBPT_MagicAttrib_TF::KBPT_MagicAttrib_TF()
{
	m_nSizeOfEntry = sizeof(KMAGICATTRIB_TABFILE);
	::strcpy(m_szTabFile, TABFILE_MAGICATTRIB);
	::memset(&m_naryMACount, 0, sizeof(m_naryMACount));
}

KBPT_MagicAttrib_TF::~KBPT_MagicAttrib_TF()
{

}

BOOL KBPT_MagicAttrib_TF::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	KMAGICATTRIB_TABFILE* pBuf = (KMAGICATTRIB_TABFILE*)m_pBuf;
	pBuf = pBuf + i;	// ¶ÁÈëµÄÊôÐÔ¼ÇÔÚ pBuf ËùÖ¸½á¹¹ÖÐ
	const PROPINFO	aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPos), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nClass), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nLevel), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_MagicAttrib.nPropKind), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_MagicAttrib.aryRange[0].nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_MagicAttrib.aryRange[0].nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_MagicAttrib.aryRange[1].nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_MagicAttrib.aryRange[1].nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_MagicAttrib.aryRange[2].nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_MagicAttrib.aryRange[2].nMax), 0},

		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[0]), 0},	//Vò khÝ cËn chiÕn
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[1]), 0},	//Vò khÝ tÇm xa
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[2]), 0},	//Y phôc
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[3]), 0},	//Giíi chØ
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[4]), 0},	//D©y chuyÒn
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[5]), 0},	//Ngoa
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[6]), 0},	//Yªu ®¸i
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[7]), 0},	//Mò
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[8]), 0},	//Bao Tay
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[9]), 0},	//Ngäc Béi
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[10]), 0}	//Ngùa
	};
	
	if (FALSE == ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0])))
		return FALSE;

    pBuf->m_nUseFlag = false;       

	for (int n = 0; n < MATF_CBDR; n++)
	{
		if (0 != pBuf->m_DropRate[n])
		{
			int nPos;
			nPos = (0 == pBuf->m_nPos) ? 1 : 0;
			m_naryMACount[nPos][n]++;
		}
	}
	return TRUE;
}

const KMAGICATTRIB_TABFILE* KBPT_MagicAttrib_TF::GetRecord(int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KMAGICATTRIB_TABFILE*)m_pBuf) + i) : NULL;
}

void KBPT_MagicAttrib_TF::GetMACount(int* pnCount) const
{
	_ASSERT(this != NULL);
	_ASSERT(pnCount != NULL);
	::memcpy(pnCount, m_naryMACount, sizeof(m_naryMACount));
}

KBPT_ClassMAIT::KBPT_ClassMAIT()
{
	m_pnTable = NULL;
	m_nSize = 0;
	m_nNumOfValidData = 0;
}

KBPT_ClassMAIT::~KBPT_ClassMAIT()
{
    m_nSize = 0;
	m_nNumOfValidData = 0;

	if (m_pnTable)
    {
        delete []m_pnTable;
        m_pnTable = NULL;
    }
}

BOOL KBPT_ClassMAIT::Clear()
{
    m_nNumOfValidData = 0; 
    return true;   
}

int KBPT_ClassMAIT::Get(int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfValidData);
    _ASSERT(m_pnTable);
	return m_pnTable[i];
}

BOOL KBPT_ClassMAIT::Insert(int nItemIndex)
{
    int nResult = false;

	_ASSERT(this != NULL);
	_ASSERT(nItemIndex >= 0);	// nData 

    if (!m_pnTable)
    {
        m_pnTable = new int [4];
        if (!m_pnTable)
            goto Exit0;

        m_nNumOfValidData = 0;
        m_nSize = 4;
    }

    if (m_nNumOfValidData >= m_nSize)
    {
        int *pnaryTemp = new int [m_nSize + 8];
        if (!pnaryTemp)
            goto Exit0;

        memcpy(pnaryTemp, m_pnTable, m_nNumOfValidData * sizeof(int));

        m_nSize += 8;
        delete []m_pnTable;
        m_pnTable = pnaryTemp;
        pnaryTemp = NULL;
    }

	m_pnTable[m_nNumOfValidData++] = nItemIndex;

    nResult = true;
Exit0:
    return nResult;
}

KBPT_ClassifiedMAT::KBPT_ClassifiedMAT()
{
	m_pnTable = NULL;
	m_nSize = 0;
	m_nNumOfValidData = 0;
}

KBPT_ClassifiedMAT::~KBPT_ClassifiedMAT()
{
	ReleaseMemory();
}

BOOL KBPT_ClassifiedMAT::GetMemory(int nCount)
{
	_ASSERT(this != NULL);
	_ASSERT(nCount > 0);
	_ASSERT(NULL == m_pnTable);
	_ASSERT(0 == m_nSize);

	BOOL bEC = FALSE;
	int* pnBuf = new int[nCount];
	if (pnBuf)
	{
		m_pnTable = pnBuf;
		m_nSize = nCount;
		bEC = TRUE;
	}
	return bEC;
}

void KBPT_ClassifiedMAT::ReleaseMemory()
{
	if (m_pnTable)
	{
		delete []m_pnTable;
		m_pnTable = NULL;
		m_nSize = 0;
	}
}

BOOL KBPT_ClassifiedMAT::Set(int nData)
{
	_ASSERT(this != NULL);
	_ASSERT(nData >= 0);	// nData ÎªÊý×éÏÂ±ê
	_ASSERT(m_nNumOfValidData >= 0 && m_nNumOfValidData < m_nSize);
	m_pnTable[m_nNumOfValidData++] = nData;
	return TRUE;
}

int KBPT_ClassifiedMAT::Get(int nIndex) const
{
	_ASSERT(this != NULL);
	_ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return (nIndex >= 0 && nIndex < m_nSize) ? m_pnTable[nIndex] : 0;
}

BOOL KBPT_ClassifiedMAT::GetAll(int* pnaryBuf, int* pnCount) const
{
	_ASSERT(this != NULL);
	_ASSERT(pnaryBuf != NULL);
	_ASSERT(pnCount != NULL);

	BOOL bEC = FALSE;
	if (pnaryBuf && pnCount && *pnCount > 0)
	{
		int nLength = *pnCount;
		if(m_nSize< nLength) nLength = m_nSize;
		::memcpy(pnaryBuf, m_pnTable, nLength*sizeof(int));
		*pnCount = nLength;
		bEC = TRUE;
	}
	return bEC;
}
