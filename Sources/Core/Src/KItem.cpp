#include "KCore.h"
#include "MyAssert.H"
#include "KTabFile.h"
#include "KNpc.h"
#include "KItem.h"
#include "KItemSet.h"
#ifndef _STANDALONE
#include "../../../lib/S3DBInterface.h"
#else
#include "S3DBInterface.h"
#endif

#ifndef _SERVER
#include "ImgRef.h"
#include "KPlayer.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "KMagicDesc.h"
#include <time.h>
#include "../../Engine/Src/Text.h"
#endif

#define	BUY_SELL_SCALE		4

KItem	Item[MAX_ITEM];
int GetRandomNumber(int nMin, int nMax);

KItem::KItem()
{
	::memset(&m_CommonAttrib,    0, sizeof(m_CommonAttrib));
	::memset(m_aryBaseAttrib,    0, sizeof(m_aryBaseAttrib));
	::memset(m_aryRequireAttrib, 0, sizeof(m_aryRequireAttrib));
	::memset(m_aryMagicAttrib,   0, sizeof(m_aryMagicAttrib));
	::memset(&m_GeneratorParam,	 0, sizeof(m_GeneratorParam));
	m_nCurrentDur = -1;
	nExpPointSec = 0;
#ifndef _SERVER
	::memset(&m_Image,   0, sizeof(KRUImage));
#endif
	m_nIndex = 0;
}

KItem::~KItem()
{
}

void* KItem::GetRequirement(IN int nReq)
{
	int i = sizeof(m_aryRequireAttrib)/sizeof(m_aryRequireAttrib[0]);
	if (nReq >= i)
		return NULL;

	return &m_aryRequireAttrib[nReq];
}

void KItem::ApplyMagicAttribToNPC(IN KNpc* pNPC, IN int nMagicActive /* = 0 */) const
{
	_ASSERT(this != NULL);
	_ASSERT(nMagicActive >= 0);

	int nCount = nMagicActive;
	int i;

	for (i = 0; i < sizeof(m_aryBaseAttrib)/sizeof(m_aryBaseAttrib[0]); i++)
	{
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_aryBaseAttrib[i]);
		if (INVALID_ATTRIB != pAttrib->nAttribType)
		{
			pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
		}
	}
	for (i = 0; i < sizeof(m_aryMagicAttrib)/sizeof(m_aryMagicAttrib[0]); i++)
	{
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_aryMagicAttrib[i]);

		if (INVALID_ATTRIB != pAttrib->nAttribType)
		{
			if (i & 1)						
			{
				if (nCount > 0)
				{
					pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
					nCount--;
				}
			}
			else
			{
				pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
			}
		}
	}
}

void KItem::RemoveMagicAttribFromNPC(IN KNpc* pNPC, IN int nMagicActive /* = 0 */) const
{
	_ASSERT(this != NULL);
	_ASSERT(nMagicActive >= 0);

	int nCount = nMagicActive;
	int	i;
	
	// ª˘¥° Ù–‘µ˜’˚NPC
	for (i = 0; i < sizeof(m_aryBaseAttrib)/sizeof(m_aryBaseAttrib[0]); i++)
	{
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_aryBaseAttrib[i]);
		if (INVALID_ATTRIB != pAttrib->nAttribType)
		{
			KItemNormalAttrib RemoveAttrib;
			RemoveAttrib.nAttribType = pAttrib->nAttribType;
			RemoveAttrib.nValue[0] = -pAttrib->nValue[0];
			RemoveAttrib.nValue[1] = -pAttrib->nValue[1];
			RemoveAttrib.nValue[2] = -pAttrib->nValue[2];
			pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
		}
	}

	for (i = 0; i < sizeof(m_aryMagicAttrib)/sizeof(m_aryMagicAttrib[0]); i++)
	{
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_aryMagicAttrib[i]);

		if (INVALID_ATTRIB != pAttrib->nAttribType)		// TODO: Œ™ -1 ∂®“Â“ª∏ˆ≥£¡ø?
		{
			KItemNormalAttrib RemoveAttrib;
			if (i & 1)						// Œ™∆Ê ˝£¨ «∫Û◊∫£®i¥”¡„ø™ º£©
			{
				if (nCount > 0)
				{
					RemoveAttrib.nAttribType = pAttrib->nAttribType;
					RemoveAttrib.nValue[0] = -pAttrib->nValue[0];
					RemoveAttrib.nValue[1] = -pAttrib->nValue[1];
					RemoveAttrib.nValue[2] = -pAttrib->nValue[2];
					pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
					nCount--;
				}
			}
			else
			{
				RemoveAttrib.nAttribType = pAttrib->nAttribType;
				RemoveAttrib.nValue[0] = -pAttrib->nValue[0];
				RemoveAttrib.nValue[1] = -pAttrib->nValue[1];
				RemoveAttrib.nValue[2] = -pAttrib->nValue[2];
				pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
			}
		}
	}
}

void KItem::ApplyHiddenMagicAttribToNPC(IN KNpc* pNPC, IN int nMagicActive) const
{
	_ASSERT(this != NULL);
	if (nMagicActive <= 0)
		return;

	const KItemNormalAttrib* pAttrib;
	pAttrib = &(m_aryMagicAttrib[(nMagicActive << 1) - 1]);	
	if (-1 != pAttrib->nAttribType)
	{
		pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
	}
}

void KItem::RemoveHiddenMagicAttribFromNPC(IN KNpc* pNPC, IN int nMagicActive) const
{
	_ASSERT(this != NULL);
	if (nMagicActive <= 0)
		return;

	const KItemNormalAttrib* pAttrib;
	pAttrib = &(m_aryMagicAttrib[(nMagicActive << 1) - 1]);	
	if (-1 != pAttrib->nAttribType)
	{
		KItemNormalAttrib RemoveAttrib;
		RemoveAttrib.nAttribType = pAttrib->nAttribType;
		RemoveAttrib.nValue[0] = -pAttrib->nValue[0];
		RemoveAttrib.nValue[1] = -pAttrib->nValue[1];
		RemoveAttrib.nValue[2] = -pAttrib->nValue[2];
		pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
	}
}

BOOL KItem::SetAttrib_CBR(IN const KBASICPROP_EQUIPMENT* pData)
{
	_ASSERT(pData != NULL);
	
	BOOL bEC = FALSE;
	if (pData)
	{
		//SetAttrib_Common(pData);
		*this = *pData;		
		SetAttrib_Base(pData->m_aryPropBasic);
		SetAttrib_Req(pData->m_aryPropReq);
		bEC = TRUE;
	}
	return bEC;
}

BOOL KItem::SetAttrib_CBR(IN const KBASICPROP_EQUIPMENT_GOLD* pData)
{
	_ASSERT(pData != NULL);
	
	BOOL bEC = FALSE;
	if (pData)
	{
		//SetAttrib_Common(pData);
		*this = *pData;		
		SetAttrib_Base(pData->m_aryPropBasic);
		SetAttrib_Req(pData->m_aryPropReq);
		bEC = TRUE;
	}
	return bEC;
}

BOOL KItem::SetAttrib_Base(const KEQCP_BASIC* pBasic)
{
	for (int i = 0;
		 i < sizeof(m_aryBaseAttrib)/sizeof(m_aryBaseAttrib[0]); i++)
	{
		KItemNormalAttrib* pDst;
		const KEQCP_BASIC* pSrc;
		pDst = &(m_aryBaseAttrib[i]);
		pSrc = &(pBasic[i]);
		pDst->nAttribType = pSrc->nType;
		pDst->nValue[0] = ::GetRandomNumber(pSrc->sRange.nMin, pSrc->sRange.nMax);
		pDst->nValue[1] = 0;	// RESERVED
		pDst->nValue[2] = 0;	// RESERVED
		if (pDst->nAttribType == magic_durability_v) //#kh´ng th” ph∏ hu˚
			SetDurability(pDst->nValue[0]);
	}
	if (m_nCurrentDur == 0)	
		m_nCurrentDur = -1;
	return TRUE;
}

BOOL KItem::SetAttrib_Req(const KEQCP_REQ* pReq)
{
	for (int i = 0;
		 i < sizeof(m_aryRequireAttrib)/sizeof(m_aryRequireAttrib[0]); i++)
	{
		KItemNormalAttrib* pDst;
		pDst = &(m_aryRequireAttrib[i]);
		pDst->nAttribType = pReq[i].nType;
		pDst->nValue[0] = pReq[i].nPara;
		pDst->nValue[1] = 0;	// RESERVED
		pDst->nValue[2] = 0;	// RESERVED
	}
	return TRUE;
}

BOOL KItem::SetAttrib_MA(IN const KItemNormalAttrib* pMA)
{
	if (NULL == pMA)
		{ _ASSERT(FALSE); return FALSE; }

	for (int i = 0; i < sizeof(m_aryMagicAttrib) / sizeof(m_aryMagicAttrib[0]); i++)
	{
		m_aryMagicAttrib[i] = pMA[i];
		
		if (m_aryMagicAttrib[i].nAttribType == magic_indestructible_b) //#kh´ng th” ph∏ hu˚
		{
			SetDurability(-1);
		}
	}
	return TRUE;
}

BOOL KItem::SetAttrib_MA(IN const KMACP* pMA)
{
	if (NULL == pMA)
		{ _ASSERT(FALSE); return FALSE; }

	for (int i = 0; i < sizeof(m_aryMagicAttrib) / sizeof(m_aryMagicAttrib[0]); i++)
	{
		const KMACP* pSrc;
		KItemNormalAttrib* pDst;
		pSrc = &(pMA[i]);
		pDst = &(m_aryMagicAttrib[i]);

		pDst->nAttribType = pSrc->nPropKind;
		pDst->nValue[0] =  ::GetRandomNumber(pSrc->aryRange[0].nMin, pSrc->aryRange[0].nMax);
		pDst->nValue[1] =  ::GetRandomNumber(pSrc->aryRange[1].nMin, pSrc->aryRange[1].nMax);
		pDst->nValue[2] =  ::GetRandomNumber(pSrc->aryRange[2].nMin, pSrc->aryRange[2].nMax);
	}
	return TRUE;
}

/*
BOOL KItem::SetAttrib_MA(IN const int* pMA)
{
	if (NULL == pMA)
	{ _ASSERT(FALSE); return FALSE; }
	
	KTabFile MagicTab;
	MagicTab.Load(GOLD_EQUIP_MAGIC_FILE);
	for (int i = 0; i < sizeof(m_aryMagicAttrib) / sizeof(m_aryMagicAttrib[0]); i++)
	{
		const int* pSrc;
		KItemNormalAttrib* pDst;
		pSrc = &(pMA[i]);
		pDst = &(m_aryMagicAttrib[i]);
		
		int nType,nLow,nHigh;
		MagicTab.GetInteger(*pSrc + 1,"Type",0,&nType);
		pDst->nAttribType = nType;
		MagicTab.GetInteger(*pSrc + 1,"Low1",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"High1",0,&nHigh);
		pDst->nValue[0] =  ::GetRandomNumber(nLow, nHigh);
		MagicTab.GetInteger(*pSrc + 1,"Low2",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"High2",0,&nHigh);
		pDst->nValue[1] =  ::GetRandomNumber(nLow, nHigh);
		MagicTab.GetInteger(*pSrc + 1,"Low3",0,&nLow);
		MagicTab.GetInteger(*pSrc + 1,"High3",0,&nHigh);
		pDst->nValue[2] =  ::GetRandomNumber(nLow, nHigh);
	}
	MagicTab.Clear();
	return TRUE;
}*/

void KItem::operator = (const KBASICPROP_EQUIPMENT& sData)
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->bStack			 = 0;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nSeries		 = sData.m_nSeries;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nBigSet		 = 0;
	pCA->nGoldId		 = 0;
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	::strcpy(pCA->szScript,	   "");
	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->uPrice = 0; 

#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);

	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_MEDMATERIAL& sData)
{
	KItemCommonAttrib* pCA = &(m_CommonAttrib);
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->bStack			 = sData.m_bStack;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nSeries		 = sData.m_nSeries;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nBigSet		 = 0;
	pCA->nGoldId		 = 0;
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	::strcpy(pCA->szScript,	   "");
	
	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->uPrice = 0;
#ifndef _SERVER

	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	KItemNormalAttrib* pBA = m_aryBaseAttrib;
	pBA[0].nAttribType = sData.m_nAttrib1_Type;
	pBA[0].nValue[0]   = sData.m_nAttrib1_Para;
	pBA[1].nAttribType = sData.m_nAttrib2_Type;
	pBA[1].nValue[0]   = sData.m_nAttrib2_Para;
	pBA[2].nAttribType = sData.m_nAttrib3_Type;
	pBA[2].nValue[0]   = sData.m_nAttrib3_Para;
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_MINE& sData)
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->bStack			 = sData.m_bStack;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nBigSet		 = sData.m_nDelet;
	pCA->nGoldId		 = 0;
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	::strcpy(pCA->szScript,	   sData.m_szScript);
	
	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->uPrice = 0;
#ifndef _SERVER

	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));	
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_QUEST& sData)
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = 0;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nPriceXu		 = sData.m_nPriceXu;
	pCA->nLevel			 = 1;
	pCA->nSeries		 = -1;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nBigSet		 = sData.m_nDelet;
	pCA->nGoldId		 = 0;
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	pCA->nIsSell		= sData.m_nIsSell;
	pCA->nIsTrade		= sData.m_nIsTrade;
	pCA->bStack			 = sData.m_bStack;
	::strcpy(pCA->szItemName,  sData.m_szName);
	::strcpy(pCA->szScript,	   sData.m_szScript);
	
	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->uPrice = 0;
#ifndef _SERVER

	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryBaseAttrib));
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryBaseAttrib));
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_TOWNPORTAL& sData)
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = 0;
	pCA->nParticularType = 0;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->bStack			 = 0;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nLevel			 = 1;
	pCA->nSeries		 = -1;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nBigSet		 = 0;
	pCA->nGoldId		 = 0;
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	::strcpy(pCA->szScript,	   "");
	
	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->uPrice = 0;
#ifndef _SERVER

	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryBaseAttrib));
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryBaseAttrib));
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_MEDICINE& sData)
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->bStack			 = sData.m_bStack;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nSeries		 = sData.m_nSeries;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nBigSet		 = 0;
	pCA->nGoldId		 = 0;
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	::strcpy(pCA->szScript,	   "");
	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->uPrice = 0;
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	KItemNormalAttrib* pBA = m_aryBaseAttrib;
	pBA[0].nAttribType = sData.m_aryAttrib[0].nAttrib;
	pBA[0].nValue[0]   = sData.m_aryAttrib[0].nValue;
	pBA[0].nValue[1]   = sData.m_aryAttrib[0].nTime;
	pBA[1].nAttribType = sData.m_aryAttrib[1].nAttrib;
	pBA[1].nValue[0]   = sData.m_aryAttrib[1].nValue;
	pBA[1].nValue[1]   = sData.m_aryAttrib[1].nTime;
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif

}

void KItem::operator = (const KBASICPROP_MAGICSCRIPT& sData)
{
	KItemCommonAttrib* pCA	= &(m_CommonAttrib);
	pCA->nItemNature		= NATURE_NORMAL;
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType			= sData.m_nDetailType;
	pCA->nParticularType		= sData.m_nParticurType;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nNewPrice			= sData.m_nPrice;
	pCA->bNewArrival		= FALSE;
	pCA->nLevel				= 0;
	pCA->nSeries			= series_num;
	pCA->bShortKey			= sData.m_bShortKey;	//b· xuËng ´ ph›m tæt
	pCA->bStack			 = sData.m_nMaxStack > 0 ? TRUE : FALSE;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= sData.m_nMaxStack;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nBigSet		 = 0;
	pCA->nGoldId		 = 0;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	::strcpy(pCA->szScript,  sData.m_szScript);
	pCA->nPickExecute = sData.m_nPickExecute;//add by phong ki“u item sˆ dÙng ngay
	pCA->nParam				= -1;
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));	
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}
/*
void KItem::operator = (const KBASICPROP_EQUIPMENT_UNIQUE& sData)
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nSeries		 = sData.m_nSeries;
	pCA->bStack			 = 0;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nBigSet		 = 0;
	pCA->nGoldId		 = 0;
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	::strcpy(pCA->szScript,	   "");
	
	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
#ifndef _SERVER
	pCA->uPrice = 0; 

	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);

	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}*/

//flying add this overloaded operator to generate a gold item.
void KItem::operator = (const KBASICPROP_EQUIPMENT_GOLD& sData)
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;	
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nLevel			 = sData.m_nLevel;
	pCA->nSeries		 = sData.m_nSeries;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nSet			 = sData.m_nSet;
	pCA->nSetId			 = sData.m_nSetId;
	pCA->nSetNum		 = sData.m_nSetNum;
	pCA->nBigSet		 = sData.m_nUpSet;
	pCA->nGoldId		 = sData.m_nId;
	pCA->bStack			 = 0;
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	::strcpy(pCA->szItemName,  sData.m_szName);
	::strcpy(pCA->szScript,	   "");
	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->nGlowLight = 0;
	pCA->uPrice =  0;
#ifndef _SERVER

	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}
/*
BOOL KItem::Gen_Equipment_Unique(const KBASICPROP_EQUIPMENT* pEqu,
								 const KBASICPROP_EQUIPMENT_UNIQUE* pUni)
{
	_ASSERT(this != NULL);
	_ASSERT(pEqu != NULL);
	_ASSERT(pUni != NULL);

	if (NULL == pEqu || NULL == pUni)
		{ _ASSERT(FALSE); return FALSE; }

	*this = *pUni;
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->bStack  = pEqu->m_bStack;
	pCA->nWidth  = pEqu->m_nWidth;
	pCA->nHeight = pEqu->m_nHeight;

	SetAttrib_Base(pEqu->m_aryPropBasic);		
	SetAttrib_Req(pUni->m_aryPropReq);			
	SetAttrib_MA(pUni->m_aryMagicAttribs);		

	return TRUE;
}*/

void KItem::Remove()
{
	m_nIndex = 0;
}

BOOL KItem::SetBaseAttrib(IN const KItemNormalAttrib* pAttrib)
{
	if (!pAttrib)
		return FALSE;

	for (int i = 0; i < sizeof(m_aryBaseAttrib) / sizeof(m_aryBaseAttrib[0]); i++)
	{
		m_aryBaseAttrib[i] = pAttrib[i];
	}
	return TRUE;
}

BOOL KItem::SetRequireAttrib(IN const KItemNormalAttrib* pAttrib)
{
	if (!pAttrib)
		return FALSE;

	for (int i = 0; i < sizeof(m_aryRequireAttrib) / sizeof(m_aryRequireAttrib[0]); i++)
	{
		m_aryRequireAttrib[i] = pAttrib[i];
	}
	return TRUE;
}

BOOL KItem::SetMagicAttrib(IN const KItemNormalAttrib* pAttrib)
{
	return SetAttrib_MA(pAttrib);
}

int KItem::Abrade(IN const int nAbradeP, IN const int nRange)//#mµi mﬂn
{
	if (m_nCurrentDur == -1 || nRange == 0)	
		return -1;

	if(nAbradeP > 0)
		m_nCurrentDur -= GetMaxDurability() * nAbradeP / MAX_PERCENT;

	if(m_nCurrentDur > 0)
	{
		if (g_Random(nRange) == 0) //#mµi mﬂn theo tham sË nRange
			m_nCurrentDur--;
	}
	if (m_nCurrentDur <= 0)
		m_nCurrentDur = 0;

	return m_nCurrentDur;
}

#ifndef _SERVER
void KItem::PaintItem(int nX, int nY, bool bResize/* = false*/, bool bPaintStack/* = false*/, unsigned int sidx /* = 0*/)
{
	bool ispos_immediacy = false;
	if (bResize && (m_CommonAttrib.nWidth * m_CommonAttrib.nHeight > 1))
	{
		strcpy(m_Image.szImage, RESIZEITEM_SPR);
	}
	//
	m_Image.oPosition.nX = nX;
	m_Image.oPosition.nY = nY;
	//
	if(GetDurability() == 0)
	{
		m_CommonAttrib.nWidth = 1;
		m_CommonAttrib.nHeight = 1;
		strcpy(m_Image.szImage, BROKEN_ITEM_SPR); //CÙc sæt cuc sat
		m_Image.oPosition.nX = nX - 1; //Vœ lπi vﬁ tr› Icon
		m_Image.oPosition.nY = nY - 2;
	}
	//
	if(sidx > 0)
	{
		int nPos = Player[CLIENT_PLAYER_INDEX].SearchItemPosByItemIdx(sidx);
		if(nPos == pos_immediacy && !IsFkItemSkill()) //Vœ lπi vﬁ tr› Icon cho nh˜ng item kh´ng ph∂i lµ skill
		{
			m_Image.oPosition.nX = nX + 5;
			m_Image.oPosition.nY = nY + 5;
		}
		if(nPos == pos_immediacy)
		{
			ispos_immediacy = true;
		}
	}
	m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, TRUE);
	
	if (IsStack() && bPaintStack && !ispos_immediacy) //Kh´ng vœ sË l≠Óng item trong ´ ph›m tæt
	{
		int nNum = GetStackNum();
		if (nNum >= 1 && nNum < 10000)
		{
			int nFontSize = 12;
			char szNum[5];
			int nLen = sprintf(szNum, "%d", nNum);
			szNum[4] = 0;
			g_pRepresent->OutputText(nFontSize, szNum, KRF_ZERO_END,
				nX + (m_CommonAttrib.nWidth * 27) - nLen * (nFontSize ) / 2,
				nY + (m_CommonAttrib.nHeight) + nFontSize + 1, 0xFFFFFF00);
		}
	}
}

void KItem::GetDesc(char* pszMsg, bool bShowPrice, int nPriceScale, int nActiveAttrib)
{
	int i = 0;
	char    TextLevel[256];
	char	szColor[item_number][32] = 
	{
		"",
		"<color=White>",
		"",
		"",
		"<color=Yellow>",
	};
	
	if (m_CommonAttrib.nItemGenre == item_equip)
	{
		if (m_CommonAttrib.nGoldId)
		{
			strcpy(szColor[item_equip], "<color=Yellow>");
		}
		else if (m_CommonAttrib.nPoint > 0)
		{
			strcpy(szColor[item_equip], "<color=Purple>");
		}
		else if (m_aryMagicAttrib[0].nAttribType)
		{
			strcpy(szColor[item_equip], "<color=Blue>");
		}
		else
		{
			strcpy(szColor[item_equip], "<color=White>");
		}
	}
	
	if (m_CommonAttrib.nEnChance)
	{
		char sItemName[256];
		sprintf(sItemName,"%s + %d",m_CommonAttrib.szItemName,m_CommonAttrib.nEnChance);
		strcpy(pszMsg, szColor[m_CommonAttrib.nItemGenre]);
		if (m_CommonAttrib.nItemGenre == 0)
		{
			int        LevelItem = m_CommonAttrib.nLevel;

			if(LevelItem > 10)
			{
				if (LevelItem < 100)
				{
					if (LevelItem%10 == 0)
					{
						sprintf(TextLevel, "%s [C p %d]", sItemName, LevelItem/(LevelItem/10));
					}
					else
					{
						sprintf(TextLevel, "%s [C p %d]", sItemName, LevelItem%10);
					}
				}
				else if (LevelItem < 1000)
				{
					if (LevelItem%100 == 0)
					{
						sprintf(TextLevel, "%s [C p %d]", sItemName, LevelItem/(LevelItem/100));
					}
					else
					{
						sprintf(TextLevel, "%s [C p %d]", sItemName, LevelItem%100);
					}
				}
			}
			else
			{
				sprintf(TextLevel, "%s [C p %d]", sItemName, LevelItem);
			}
			strcat(pszMsg, TextLevel);
		}
		else
		{
			strcat(pszMsg, sItemName);
		}
	}
	else
	{
		strcpy(pszMsg, szColor[m_CommonAttrib.nItemGenre]);
		if (m_CommonAttrib.nItemGenre == 0)
		{
			int        LevelItem = m_CommonAttrib.nLevel;

			if(LevelItem > 10)
			{
				if (LevelItem < 100)
				{
					if (LevelItem%10 == 0)
					{
						sprintf(TextLevel, "%s [C p %d]", m_CommonAttrib.szItemName, LevelItem/(LevelItem/10));
					}
					else
					{
						sprintf(TextLevel, "%s [C p %d]", m_CommonAttrib.szItemName, LevelItem%10);
					}
				}
				else if (LevelItem < 1000)
				{
					if (LevelItem%100 == 0)
					{
						sprintf(TextLevel, "%s [C p %d]", m_CommonAttrib.szItemName, LevelItem/(LevelItem/100));
					}
					else
					{
						sprintf(TextLevel, "%s [C p %d]", m_CommonAttrib.szItemName, LevelItem%100);
					}
				}
			}
			else
			{
				sprintf(TextLevel, "%s [C p %d]", m_CommonAttrib.szItemName, LevelItem);
			}
			strcat(pszMsg, TextLevel);
		}
		else
		{
			strcat(pszMsg, "<color=White>");
			strcat(pszMsg, m_CommonAttrib.szItemName);
		}
	}
	//
	if (m_CommonAttrib.nItemGenre == item_magicscript && m_CommonAttrib.nParticularType == 146) //Huyen tinh khoang thach
	{
		sprintf(TextLevel, " [C p %d]", m_CommonAttrib.nLevel);
		strcat(pszMsg, TextLevel);
	}
	//
	if (m_CommonAttrib.nItemGenre == item_magicscript && (m_CommonAttrib.nParticularType == 1083 || m_CommonAttrib.nParticularType == 1084)) //HÂi thµnh phÔ
	{
		strcat(pszMsg, "  \n  ");
		char NumUse[128];
		sprintf(NumUse, " Cﬂn lπi <color=red> %d <color> l«n sˆ dÙng ", m_CommonAttrib.nParam);
		strcat(pszMsg, NumUse);
	}
	//
	strcat(pszMsg, "  \n  ");

	if(InsuranceCourse == -2)//#kho∏ v‹nh vi‘n
	{
		strcat(pszMsg, "<color=Green>VÀt ph»m nµy ÆuÓc kho∏ v‹nh vi‘n");
		strcat(pszMsg, "\r\n");
	}

	if(InsuranceCourse > 0)//#hπn sˆ dÙng
	{
		if(InsuranceHourCourse > 0)
		{
			time_t baygio = time(0);
			int s_conlai = InsuranceHourCourse - baygio;
			if( s_conlai > 0)
			{
				DWORD p_conlai = s_conlai / 60;
				DWORD g_conlai = p_conlai / 60;
				if(g_conlai > 1)
				{
					char szPrice2[32];
					sprintf(szPrice2, "<color=Green>ThÍi gian chÍ mÎ kho∏ : %d giÍ", g_conlai);
					strcat(pszMsg, szPrice2);
					strcat(pszMsg, "  \n  ");
				}
				else if(p_conlai > 1)
				{
					char szPrice2[32];
					sprintf(szPrice2, "<color=Green>ThÍi gian chÍ mÎ kho∏ : %d phÛt", p_conlai);
					strcat(pszMsg, szPrice2);
					strcat(pszMsg, "  \n  ");
				}
			}
			else
			{
				strcat(pszMsg, "<color=Green>Hoµn t t thÍi gian chÍ mÎ kho∏.\r\n");
			}
		}
		else
		{
			strcat(pszMsg, "<color=Green>VÀt ph»m nµy ÆuÓc kho∏ b∂o hi”m");
			strcat(pszMsg, "\r\n");
		}
	}
	
	/*
	if (bShowPrice && nPriceScale > 0)
	{
		char szPrice[32];
		if (m_CommonAttrib.nPrice / nPriceScale < 10000)
			sprintf(szPrice, "<color=White>Gi∏: %d l≠Óng<color>", m_CommonAttrib.nPrice / nPriceScale);
		else
		{
			if (m_CommonAttrib.nPrice / nPriceScale%10000 == 0)
				sprintf(szPrice, "<color=White>Gi∏: %d vπn l≠Óng<color>",  m_CommonAttrib.nPrice / nPriceScale /10000);
			else
				sprintf(szPrice, "<color=White>Gi∏: %d vπn %d l≠Óng<color>",  m_CommonAttrib.nPrice / nPriceScale/10000,  m_CommonAttrib.nPrice / nPriceScale%10000);
		}
		strcat(pszMsg, szPrice);
		strcat(pszMsg, "  \n  ");
	}*/
	
	char szPriceColor[moneyunit_num][32] = 
	{
		"<color=255,255,255>",
		"<color=255,90,0>",
		"<color=255,219,74>",
		"<color=255,219,74>",
		"<color=0,255,0>",
		"<color=0,255,0>",
		"<color=255,90,0>",
	};
	
	if (bShowPrice)
	{
		int nPrice = 0;
		char pszTemp2[128];
		char pszTemp[128];
		
		nPrice = GetCurPrice();
		strcat(pszMsg, szPriceColor[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nMoneyUnit]);
		sprintf(pszTemp2, "%d", Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nMoneyUnit);	
		
		strcat(pszMsg, "  \n  ");
		strcpy(pszTemp, "Gi∏ c∂: ");
		strcat(pszMsg, pszTemp);

		g_GameSetting.GetString("MoneyUnit", pszTemp2, "", pszTemp, sizeof(pszTemp));
		sprintf(pszTemp2, "%d %s <color=255,255,255> ", nPrice, pszTemp);
		strcat(pszMsg, pszTemp2);	
		strcat(pszMsg, "  \n  ");
	}

	int nfkSerial = m_CommonAttrib.nSeries;
	if(m_CommonAttrib.nItemGenre == item_magicscript)
	{
		int nPar = m_CommonAttrib.nParticularType;
		if(nPar == 398 || nPar == 399) //s∏t thÒ l÷nh, s∏t thÒ gi∂n
			nfkSerial = m_CommonAttrib.nSeries;
		else
			nfkSerial = series_nil;
	}
	
	switch(nfkSerial)
	{
	case series_metal:
		strcat(pszMsg, "  <color=White>ThuÈc t›nh NgÚ hµnh: <color=Metal>Kim  ");
		break;
	case series_wood:
		strcat(pszMsg, "  <color=White>ThuÈc t›nh NgÚ hµnh: <color=Wood>MÈc  ");
		break;
	case series_water:
		strcat(pszMsg, "  <color=White>ThuÈc t›nh NgÚ hµnh: <color=Water>ThÒy  ");
		break;
	case series_fire:
		strcat(pszMsg, "  <color=White>ThuÈc t›nh NgÚ hµnh: <color=Fire>H·a  ");
		break;
	case series_earth:
		strcat(pszMsg, "  <color=White>ThuÈc t›nh NgÚ hµnh: <color=Earth>ThÊ  ");
		break;
	}

	strcat(pszMsg, "  \n  ");
	strcat(pszMsg, " <color=White> ");
	char szIntroEnter[256] = "";
	TEnterTextFromCharArray(m_CommonAttrib.szIntro, szIntroEnter, 40); //50 k˝ t˘ enter xuËng //add by phong ki“u enter xuËng dﬂng
	strcat(pszMsg, szIntroEnter);//strcat(pszMsg, m_CommonAttrib.szIntro); 
	strcat(pszMsg, "  \n  ");

	if (m_CommonAttrib.nItemGenre == item_magicscript &&  m_CommonAttrib.nParticularType >= 199 &&  m_CommonAttrib.nParticularType <= 204)
	{
		char szIntor[64] = "!";
		KTabFile MagicTab;
		MagicTab.Load("\\Settings\\Item\\magicattriblevel_index.txt");
		char szTmp[8];
		sprintf(szTmp,"%d",m_aryBaseAttrib[0].nValue[0]);

		MagicTab.GetString(szTmp,"DESC","",szIntor,64);
		strcat(pszMsg, " <color=Fire>ThuÈc t›nh: ");
		strcat(pszMsg, szIntor);
		strcat(pszMsg, "  \n  ");
		MagicTab.GetString(szTmp,"FIT_EQUIP","",szIntor,64);
		strcat(pszMsg, " <color=White>Y™u c«u trang bﬁ:<color> <color=Green>");
		strcat(pszMsg, szIntor);
		strcat(pszMsg, "  \n  ");
		strcat(pszMsg, "  \n  ");
	}

	if (m_CommonAttrib.nItemGenre == item_magicscript && (m_CommonAttrib.nParticularType >= 199 && m_CommonAttrib.nParticularType <= 204))
	{
		char szLevel[32];
		sprintf(szLevel, " <color=White>Ph»m ch t thuÈc t›nh:<color> <color=Blue>%d<color>", m_CommonAttrib.nLevel);
		strcat(pszMsg, szLevel);
		strcat(pszMsg, "  \n  ");
	}

	for (i = 0; i < 7; i++)
	{
		if (!m_aryBaseAttrib[i].nAttribType)
		{
			continue;
		}
		if (m_aryBaseAttrib[i].nAttribType == magic_durability_v) //#kh´ng th” ph∏ hu˚
		{
			char	szDurInfo[32];
			if (m_nCurrentDur == -1)
			{
				sprintf(szDurInfo, "<color=Yellow>Kh´ng th” ph∏ hu˚");
			}
			else if(m_nCurrentDur == 0)
			{
				sprintf(szDurInfo, "<color=Red>Trang bﬁ h·ng");
			}
			else
			{
				sprintf(szDurInfo, "ßÈ b“n: %3d / %3d", GetDurability(), GetMaxDurability());
			}
			strcat(pszMsg, szDurInfo);
		}
		else
		{
			char* pszInfo = (char *)g_MagicDesc.GetDesc(&m_aryBaseAttrib[i]);
			if (!pszInfo || !pszInfo[0])
				continue;
			strcat(pszMsg, pszInfo);
		}
		strcat(pszMsg, "  \n  ");
	}
	
	for (i = 0; i < 6; i++)
	{
		if (!m_aryRequireAttrib[i].nAttribType)
		{
			continue;
		}
		char* pszInfo = (char *)g_MagicDesc.GetDesc(&m_aryRequireAttrib[i]);
		if (!pszInfo || !pszInfo[0])
			continue;
		if (Player[CLIENT_PLAYER_INDEX].m_ItemList.EnoughAttrib(&m_aryRequireAttrib[i]))
		{
			strcat(pszMsg, "<color=White>");
		}
		else
		{
			strcat(pszMsg, "<color=Red>");
		}
		strcat(pszMsg, pszInfo);
		strcat(pszMsg, "  \n  ");
	}

	for (i = 0; i < 6; i++)
	{
		if (!m_aryMagicAttrib[i].nAttribType)
		{
			if (i < m_CommonAttrib.nPoint)
			{
				strcat(pszMsg, "<color=Yellow>Ch≠a kh∂m nπm<color=white>");
				strcat(pszMsg, "  \n  ");
			}
			continue;
		}
		char* pszInfo = (char *)g_MagicDesc.GetDesc(&m_aryMagicAttrib[i]);
		if (!pszInfo || !pszInfo[0])
		continue;
		if ((i & 1) == 0)
		{
			if (m_CommonAttrib.nGoldId > 0)
				strcat(pszMsg, "<color=HYellow>");
			else if (m_CommonAttrib.nPoint)
				strcat(pszMsg, "<color=Purple>");
			else
				strcat(pszMsg, "<color=HBlue>");
		}
		else
		{
			if ((i>>1) < nActiveAttrib)
			{
				if (m_CommonAttrib.nGoldId > 0)
					strcat(pszMsg, "<color=HYellow>");
				else if (m_CommonAttrib.nPoint)
					strcat(pszMsg, "<color=Purple>");
				else
					strcat(pszMsg, "<color=HBlue>");
			}
			else
			{
				if (m_CommonAttrib.nGoldId > 0)
					strcat(pszMsg, "<color=DYellow>");
				else if (m_CommonAttrib.nPoint)
					strcat(pszMsg, "<color=APurple>");
				else
					strcat(pszMsg, "<color=DBlue>");
			}
		}
		strcat(pszMsg, pszInfo);
		strcat(pszMsg, "  \n  ");
	}
    PlayerItem m_pItems = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[Player[CLIENT_PLAYER_INDEX].m_ItemList.FindSame((DWORD)m_dwID)];
	if (m_aryMagicAttrib[0].nAttribType && 
		m_CommonAttrib.nItemGenre == item_equip && 
		m_CommonAttrib.nDetailType < equip_horse )
	{
    if (m_pItems.nPlace == pos_equip)//m_CommonAttrib.nGoldId > 0)
		{
        char szBuff[64], szBuffer[64];
        char szColor[64];
		char pszKeyName[64];
        sprintf(pszKeyName, "NeedSr%d", m_CommonAttrib.nSeries);
        g_GameSetting.GetString("ActiveEquip", pszKeyName, "", szBuff, sizeof(szBuff));
					
					switch(m_CommonAttrib.nSeries)
					{
					case series_metal:
                strcpy(szColor, "<color=Earth>");
						break;
					case series_wood:
                strcpy(szColor, "<color=Water>");
						break;
					case series_water:
                strcpy(szColor, "<color=Metal>");
						break;
					case series_fire:
                strcpy(szColor, "<color=Wood>");
						break;
					case series_earth:
                strcpy(szColor, "<color=Fire>");
						break;
            default:
                strcpy(szColor, "<color=Yellow>");
						break;
					}
					
        switch(m_CommonAttrib.nDetailType)
					{
            case equip_meleeweapon:
            case equip_rangeweapon:
            case equip_armor:
            case equip_ring:
			case equip_amulet:
			case equip_boots:
			case equip_belt:
			case equip_helm:
			case equip_cuff:
			case equip_pendant:
				{
                // ph©n bi÷t ring 1 vµ ring 2
                if (m_pItems.nX == itempart_ring1)
                    sprintf(pszKeyName, "NeedEq%d1", m_CommonAttrib.nDetailType);
                else if (m_pItems.nX == itempart_ring2)
                    sprintf(pszKeyName, "NeedEq%d2", m_CommonAttrib.nDetailType);
                else
                    sprintf(pszKeyName, "NeedEq%d", m_CommonAttrib.nDetailType);

                if (pszKeyName[0] != '\0')
					{
                    strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color>");
                    strcat(pszMsg, szColor );
                    strcat(pszMsg, szBuff);
                    strcat(pszMsg, "<color=Yellow> cÒa ");
                    g_GameSetting.GetString("ActiveEquip", pszKeyName, "", szBuffer, sizeof(szBuffer));
                    strcat(pszMsg, szBuffer);
                    strcat(pszMsg, " Æ” k›ch t›nh ©m<color>\n");
					}
					break;
				}
            default:
                pszKeyName[0] = '\0';
				break;		
			}
		}
}
/*
	if (GetAttribType() || m_CommonAttrib.nPoint)
		{
			switch(m_CommonAttrib.nDetailType)
			{
			case equip_meleeweapon:
				{
					switch(m_CommonAttrib.nSeries)
					{
					case series_metal:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Earth>ThÊ <color>");
						break;
					case series_wood:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Water>ThÒy <color>");
						break;
					case series_water:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Metal>Kim <color>");
						break;
					case series_fire:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Wood>MÈc <color>");
						break;
					case series_earth:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Fire>H·a <color>");
						break;
					}
					strcat(pszMsg, "cÒa d©y chuy“n vµ y phÙc Æ” k›ch t›nh ©m<color> \n");
					break;
					
				}
			case equip_rangeweapon:
				{
					switch(m_CommonAttrib.nSeries)
					{
					case series_metal:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Earth>ThÊ <color>");
						break;
					case series_wood:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Water>ThÒy <color>");
						break;
					case series_water:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Metal>Kim <color>");
						break;
					case series_fire:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Wood>MÈc <color>");
						break;
					case series_earth:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Fire>H·a <color>");
						break;
					}
					strcat(pszMsg, "cÒa d©y chuy“n vµ y phÙc Æ” k›ch t›nh ©m<color>\n");
					break;
					
				}
			case  equip_armor:
				{
					switch(m_CommonAttrib.nSeries)
					{
					case series_metal:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Earth>ThÊ <color>");
						break;
					case series_wood:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Water>ThÒy <color>");
						break;
					case series_water:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Metal>Kim <color>");
						break;
					case series_fire:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Wood>MÈc <color>");
						break;
					case series_earth:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Fire>H·a <color>");
						break;
					}
					strcat(pszMsg, "cÒa nh…n (d≠Ìi) vµ thæt l≠ng Æ” k›ch t›nh ©m<color>\n");
					break;
					
				}
			case equip_ring:
				{
					switch(m_CommonAttrib.nSeries)
					{
					case series_metal:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Earth>ThÊ <color>");
						break;
					case series_wood:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Water>ThÒy <color>");
						break;
					case series_water:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Metal>Kim <color>");
						break;
					case series_fire:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Wood>MÈc <color>");
						break;
					case series_earth:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Fire>H·a <color>");
						break;
					}
                        strcat(pszMsg, "cÒa n„n vµ vÚ kh› Æ” k›ch t›nh ©m<color> \n");
					break;
					
				}
			case equip_amulet:
				{
					switch(m_CommonAttrib.nSeries)
					{
					case series_metal:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Earth>ThÊ <color>");
						break;
					case series_wood:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Water>ThÒy <color>");
						break;
					case series_water:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Metal>Kim <color>");
						break;
					case series_fire:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Wood>MÈc <color>");
						break;
					case series_earth:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Fire>H·a <color>");
						break;
					}
					strcat(pszMsg, "cÒa nh…n (d≠Ìi) vµ thæt l≠ng Æ” k›ch t›nh ©m<color> \n");
					break;
				}
			case equip_boots:
				{
					switch(m_CommonAttrib.nSeries)
					{
					case series_metal:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Earth>ThÊ <color>");
						break;
					case series_wood:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Water>ThÒy <color>");
						break;
					case series_water:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Metal>Kim <color>");
						break;
					case series_fire:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Wood>MÈc <color>");
						break;
					case series_earth:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Fire>H·a <color>");
						break;
					}
					strcat(pszMsg, "cÒa n„n vµ vÚ kh› Æ” k›ch t›nh ©m<color> \n");
					break;
					
				}
			case equip_belt:
				{
					switch(m_CommonAttrib.nSeries)
					{
					case series_metal:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Earth>ThÊ <color>");
						break;
					case series_wood:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Water>ThÒy <color>");
						break;
					case series_water:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Metal>Kim <color>");
						break;
					case series_fire:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Wood>MÈc <color>");
						break;
					case series_earth:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Fire>H·a <color>");
						break;
					}	
					strcat(pszMsg, "cÒa ng‰c bÈi vµ bao tay Æ” k›ch t›nh ©m<color> \n");
					break;
					
				}
			case equip_helm:
				{
					switch(m_CommonAttrib.nSeries)
					{
					case series_metal:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Earth>ThÊ <color>");
						break;
					case series_wood:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Water>ThÒy <color>");
						break;
					case series_water:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Metal>Kim <color>");
						break;
					case series_fire:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Wood>MÈc <color>");
						break;
					case series_earth:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Fire>H·a <color>");
						break;
					}		
					strcat(pszMsg, "cÒa d©y chuy“n vµ y phÙc Æ” k›ch t›nh ©m<color>\n");
					break;
				}
			case equip_cuff:
				{
					switch(m_CommonAttrib.nSeries)
					{
					case series_metal:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Earth>ThÊ <color>");
						break;
					case series_wood:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Water>ThÒy <color>");
						break;
					case series_water:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Metal>Kim <color>");
						break;
					case series_fire:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Wood>MÈc <color>");
						break;
					case series_earth:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Fire>H·a <color>");
						break;
					}	
					strcat(pszMsg, "cÒa gi«y vµ nh…n (tr™n) Æ” k›ch hoπt ©m<color> \n");
					break;
				}
			case equip_pendant:
				{
					switch(m_CommonAttrib.nSeries)
					{
					case series_metal:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Earth>ThÊ <color>");
						break;
					case series_wood:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Water>ThÒy <color>");
						break;
					case series_water:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Metal>Kim <color>");
						break;
					case series_fire:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Wood>MÈc <color>");
						break;
					case series_earth:
						strcat(pszMsg, "\n<color=Yellow> C«n h÷ <color=Fire>H·a <color>");
						break;
					}
					strcat(pszMsg, "cÒa nh…n (tr™n) vµ giµy Æ” k›ch t›nh ©m<color>\n");
					break;
				}
				strcat(pszMsg, "\n");
				break;		
			}
		}
	}*/
	if (m_CommonAttrib.uPrice > 0)
	{
		strcat(pszMsg, "  \n  ");
		strcat(pszMsg, "<color=Yellow>");
		char sOrice[256];
		if (m_CommonAttrib.uPrice < 10000)
		{
			sprintf(sOrice, "<color=White>Gi∏ ni™m y’t:<color> %d l≠Óng", m_CommonAttrib.uPrice);
		}
		else
		{
			if (m_CommonAttrib.uPrice%10000 == 0)
			{
				sprintf(sOrice, "<color=White>Gi∏ ni™m y’t:<color> %d vπn l≠Óng",  m_CommonAttrib.uPrice/10000);
			}
			else
			{
				sprintf(sOrice, "<color=White>Gi∏ ni™m y’t:<color> %d vπn %d l≠Óng",  m_CommonAttrib.uPrice/10000,  m_CommonAttrib.uPrice%10000);
			}
		}
		strcat(pszMsg, sOrice);
		strcat(pszMsg, "  \n  ");
		strcat(pszMsg, "  \n  ");
	}

	strcat(pszMsg, "  \n  ");
	
	if (m_CommonAttrib.LimitTime.bYear)
	{
		char sTmp[128];
        const long thoigianhet = m_CommonAttrib.LimitTime.bYear;
		if(thoigianhet > 0)
		{
       
            time_t timeValue = static_cast<time_t>(thoigianhet);
            tm ltm = {};
            localtime_s(&ltm, &timeValue);
    
            int nam = 1900 + ltm.tm_year;
            int thang = 1 + ltm.tm_mon;
            int ngay = ltm.tm_mday;
            int gio = ltm.tm_hour;
            int phut = ltm.tm_min;
            int giay = ltm.tm_sec;
    
            if (time(0) < timeValue)
			{
                snprintf(sTmp, sizeof(sTmp), "<color=fire>ThÍi hπn sˆ dÙng: %02d:%02d:%02d %02d-%02d-%d<color>", gio, phut, giay, ngay, thang, nam);
			}
			else
			{
                snprintf(sTmp, sizeof(sTmp), "<color=red>H’t hπn sˆ dÙng vÀt ph»m sœ bﬁ hÒy");
			}
            
            // Use strncat for safer concatenation
            strncat(pszMsg, "  \n  ", sizeof(pszMsg) - strlen(pszMsg) - 1);
            strncat(pszMsg, sTmp, sizeof(pszMsg) - strlen(pszMsg) - 1);
		}
	}

	if(GetExpirePoint() > 0)
	{
		char sTmp[128];
		int totalSeconds = GetExpirePoint();
	    int days = totalSeconds / (60 * 60 * 24);      
	    int hours = (totalSeconds % (60 * 60 * 24)) / (60 * 60);
		sprintf(sTmp, "<color=fire>ThÍi hπn sˆ dÙng: %d Ngµy %d GiÍ . <color>", days, hours);
		strcat(pszMsg, "  \n  ");
		strcat(pszMsg,sTmp);
	}

	if (m_CommonAttrib.nGoldId)
	{
		strcat(pszMsg, " \n ");
		KTabFile GoldTab;

		char szGoldName[64];
		GoldTab.Load(TABFILE_GOLDITEM_FULL_O);
		for (int k = 0;k < m_CommonAttrib.nSetNum;k++)
		{
			GoldTab.GetString(m_CommonAttrib.nGoldId + 3 - m_CommonAttrib.nSetId + k, "Name", "", szGoldName, sizeof(szGoldName));
			int ncolor = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetGoldColor(m_CommonAttrib.nSet, k + 1);
			if (m_CommonAttrib.nSetId == k + 1)
			{
				strcat(pszMsg, "<color=AYellow>");
			}
			else if (ncolor == 2)
			{
				strcat(pszMsg, "<color=Green>");
			}
			else if (ncolor == 1)
			{
				strcat(pszMsg, "<color=HGreen>");
			}
			else if (ncolor == 0)
			{
				strcat(pszMsg, "<color=HGreen>");
			}
			strcat(pszMsg, szGoldName);
			strcat(pszMsg, " \n ");
		}
	}
}
#endif

int KItem::GetMaxDurability()
{
	for (int i = 0; i < 7; i++)
	{
		if (m_aryBaseAttrib[i].nAttribType == magic_durability_v) //#kh´ng th” ph∏ hu˚
		{
			return m_aryBaseAttrib[i].nValue[0];
		}
	}
	return -1;
}

int KItem::GetTotalMagicLevel()
{
	int nRet = 0;
	for (int i = 0; i < 6; i++)
	{
		nRet += m_GeneratorParam.nGeneratorLevel[i];
	}
	return nRet;
}

int KItem::GetRepairPrice()
{
	if (ItemSet.m_sRepairParam.nMagicScale == 0)
		return 0;

	if (GetGenre() != item_equip)
		return 0;

	if (m_nCurrentDur == -1)
		return 0;

	int nMaxDur = GetMaxDurability();
	int nSumMagic = GetTotalMagicLevel();

	if (nMaxDur <= 0)
		return 0;

	if(GetGoldId())//neu do hoang kim gia sua gap 5 lan
		return (m_CommonAttrib.nPrice * ItemSet.m_sRepairParam.nPriceScale / 100 * (nMaxDur - m_nCurrentDur) / nMaxDur * (ItemSet.m_sRepairParam.nMagicScale + nSumMagic) / ItemSet.m_sRepairParam.nMagicScale) * 5;

	return m_CommonAttrib.nPrice * ItemSet.m_sRepairParam.nPriceScale / 100 * (nMaxDur - m_nCurrentDur) / nMaxDur * (ItemSet.m_sRepairParam.nMagicScale + nSumMagic) / ItemSet.m_sRepairParam.nMagicScale;
}

BOOL KItem::CanBeRepaired()
{
	if (GetGenre() != item_equip)
		return FALSE;

	if (m_nCurrentDur == -1 || m_nCurrentDur == 0) //#do ben bang 0 trang bi hong khong the sua bang tien van || do ben = -1 khong bi mai mon
		return FALSE;

	int nMaxDur = GetMaxDurability();
	if (m_nCurrentDur == nMaxDur)
		return FALSE;

	return TRUE;
}

BOOL KItem::CanStack()
{
	if (m_CommonAttrib.bStack)
	{
		if (m_CommonAttrib.nItemGenre != 1)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL KItem::CanStack( int nOldIdx, int Dest)
{
	if (m_CommonAttrib.bStack)
	{
		if (m_CommonAttrib.nItemGenre == Item[nOldIdx].GetGenre()
			&& m_CommonAttrib.nDetailType == Item[nOldIdx].GetDetailType()
			&& m_CommonAttrib.nParticularType == Item[nOldIdx].GetParticular()
			&& m_CommonAttrib.nSeries == Item[nOldIdx].GetSeries()
			&& m_CommonAttrib.nItemGenre != 1
			&& m_CommonAttrib.LimitTime.bYear == Item[nOldIdx].GetTime()->bYear
			&& Item[nOldIdx].GetPlayerItemLock() == Item[Dest].GetPlayerItemLock() 
			&& Item[nOldIdx].GetPlayerItemHLock() == Item[Dest].GetPlayerItemHLock()
			&& Item[Dest].GetStackNum() < Item[Dest].GetMaxStackNum()
			&& Item[nOldIdx].GetStackNum() < Item[nOldIdx].GetMaxStackNum())
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

int KItem::Stack( int nIdx )
{
	m_CommonAttrib.nStackNum += nIdx;
	
	if (m_CommonAttrib.nStackNum > Def_MAX_STACK_TIENDONG)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int KItem::EnChance( int nEnChance /*= 1*/ )
{
		if (nEnChance < 10)
		{
			m_CommonAttrib.nEnChance = nEnChance;
			for (int i = 0;i < 6;i++)
			{
				if (m_aryMagicAttrib[i].nAttribType)
				{
					if (m_aryMagicAttrib[i].nValue[0] >= 5)
					{
						m_aryMagicAttrib[i].nValue[0] += m_aryMagicAttrib[i].nValue[0] * nEnChance / 18;
					}
				}
			}
		}
		else
		{
			m_CommonAttrib.nEnChance = 9;
			for (int i = 0;i < 6;i++)
			{
				if (m_aryMagicAttrib[i].nAttribType)
				{
					if (m_aryMagicAttrib[i].nValue[0] >= 5)
					{
						m_aryMagicAttrib[i].nValue[0] += m_aryMagicAttrib[i].nValue[0] * nEnChance / 18;
					}
				}
			}
		}
	
	return 1;
}

int KItem::IsPurple()
{
	return m_CommonAttrib.nPoint;
}

void KItem::SetExpTime( int bYear,BYTE bMonth,BYTE bDay,BYTE bHour )
{
	if (bYear)
	{
		//while (bHour > 24)
		//{
		//	bDay++;
		//	bHour -= 24;
		//}
		//while (bDay > 30)
		//{
		//	if (((bMonth <= 7) && !(bMonth % 2)) || ((bMonth > 7) && !(bMonth % 2)))
		//	{
		//		bMonth++;
		//		bDay -= 30;
		//	}
		//	else if (((bMonth <= 7) && !(bMonth % 2) && bDay > 31 )||((bMonth > 7) && (bMonth % 2) && bDay > 31))
		//	{
		//		bMonth++;
		//		bDay -= 31;
		//	}
		//	else if (((bMonth <= 7) && !(bMonth % 2) && bDay == 31 )||((bMonth > 7) && (bMonth % 2) && bDay == 31))
		//	{
		//		break;
		//	}
		//}
		//while (bMonth > 12)
		//{
		//	bYear++;
		//	bMonth -= 12;
		//}
		m_CommonAttrib.LimitTime.bYear = bYear;
		//m_CommonAttrib.LimitTime.bMonth = bMonth;
		//m_CommonAttrib.LimitTime.bDay = bDay;
		//m_CommonAttrib.LimitTime.bHour = bHour;
	}
}

BOOL KItem::HaveMaigc( int nAttribe,int nValue1Min,int nValue1Max,int nValue2Min,int nValue2Max,int nValue3Min,int nValue3Max )
{
	for (int i = 0;i < 6;i++)
	{
		if (m_aryMagicAttrib[i].nAttribType == nAttribe)
		{
			if (nValue1Min != -1)
			{
				if (m_aryMagicAttrib[i].nValue[0] < nValue1Min)
					return FALSE;
			}
			if (nValue1Max != -1)
			{
				if (m_aryMagicAttrib[i].nValue[0] > nValue1Max)
					return FALSE;
			}
			if (nValue2Min != -1)
			{
				if (m_aryMagicAttrib[i].nValue[1] < nValue2Min)
					return FALSE;
			}
			if (nValue2Max != -1)
			{
				if (m_aryMagicAttrib[i].nValue[1] > nValue2Max)
					return FALSE;
			}
			if (nValue3Min != -1)
			{
				if (m_aryMagicAttrib[i].nValue[2] < nValue3Min)
					return FALSE;
			}
			if (nValue3Max != -1)
			{
				if (m_aryMagicAttrib[i].nValue[2] > nValue3Max)
					return FALSE;
			}
			return TRUE;
		}
	}
	return FALSE;
}

int KItem::GetSalePrice()
{
	if(GetPlayerItemLock())
		return 0;

	return (m_CommonAttrib.nPrice/BUY_SELL_SCALE)*m_CommonAttrib.nStackNum;
}

BYTE KItem::GetKind()
{
	if (GetGoldId())
	{
		return gold_item;
	}
	else if (IsPurple())
	{
		return purple_item;
	}
	else if (m_aryMagicAttrib[0].nAttribType)
	{
		return green_item;
	}
	else
	{
		return normal_item;
	}
}

int KItem::GetColorItem()
{
	if (GetGoldId())
	{
		return gold_item;
	}
	else if (IsPurple())
	{
		return purple_item;
	}
	else if (m_aryMagicAttrib[0].nAttribType)
	{
		return green_item;
	}
	else
	{
		return normal_item;
	}
}

int	KItem::CheckSubStrInText(char* pstart,char* strItemInfo, int nLen, int sublen)
{

		for(int z=0; z< nLen; z++)
		{
			if(strItemInfo[z] == pstart[0])
			{
				BOOL checkname = TRUE;
				for(int t=0; t< sublen; t++)
				{
					if(strItemInfo[z+t] != pstart[t])
						checkname = FALSE;
				}
				if(checkname == TRUE)
				{
					return z;
				}
			}
		}
		return -1;
}
