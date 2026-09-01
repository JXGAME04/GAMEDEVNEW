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
#include <unordered_map>

#define	BUY_SELL_SCALE		4

KItem	Item[MAX_ITEM];
int GetRandomNumber(int nMin, int nMax);

KItem::KItem()
{
	Reset();
}

void KItem::Reset() {
	::memset(&m_CommonAttrib, 0, sizeof(m_CommonAttrib));
	::memset(m_aryBaseAttrib, 0, sizeof(m_aryBaseAttrib));
	::memset(m_aryRequireAttrib, 0, sizeof(m_aryRequireAttrib));
	::memset(m_aryMagicAttrib, 0, sizeof(m_aryMagicAttrib));
	::memset(&m_GeneratorParam, 0, sizeof(m_GeneratorParam));
	m_nCurrentDur = -1;
	nExpPointSec = 0;
	InsuranceCourse = 0;
	m_MaxOptMultiply = 1;
	// [PHI PHONG 2026-08-29] xoa sach du lieu sao/da khi tai su dung o vat pham
	::memset(m_nPfPack, 0, sizeof(m_nPfPack));
	m_bHorseScaleOnly = false;
#ifndef _SERVER
	::memset(&m_Image, 0, sizeof(KRUImage));
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

int KItem::ApplyScalingMethod(float baseValue, int type) const{
	if (type == 111 || type == 135 || type == 246 || type == 247) { //111=toc do di chuyen
		return baseValue;
	}
	switch (g_xMethod) { //use global, change setting will change x method for all items.
	case LOGARITHMIC:
		return std::log(1 + m_MaxOptMultiply) * baseValue;
	case SQUARE_ROOT:
		return std::sqrt(m_MaxOptMultiply) * baseValue;
	case SIGMOID: {
		float k = 0.1f;
		float c = 5.0f;
		return baseValue * (1.0f / (1.0f + std::exp(-k * (m_MaxOptMultiply - c))));
	}
	case EXPONENTIAL_DECAY: {
		float k = 0.5f;
		return baseValue * (1 - std::exp(-k * m_MaxOptMultiply));
	}
	case POLYNOMIAL: {
		float b = 0.8f;
		return baseValue * std::pow(m_MaxOptMultiply, b);
	}
	default:
		return baseValue; // Fallback (no scaling)
	}
}

void KItem::SetScalingMethod(XMethod method) {
	switch (method) {
	default:
		m_xMethod = method;
	}
}

/******************************************************************************
Function: Apply the magic on the item to the NPC
Entry	: pNPC: pointer to NPC, nMagicAcive: number of opened hidden attributes
Exit 	: Magic is applied.
The specific work is completed by the member functions of KNpc.
No member variables of the KItem object itself are modified
******************************************************************************/
// ======== [PHI PHONG 2026-08-29] thuoc tinh Tinh Than Thach kham tren phi phong ========
// Ban Linux: starstone.txt cot 14 = CHI SO 0-BASED cua dong trong magicattrib_ge.txt
// (do that: gia tri 3933 -> dong 1-based 3934, ten bat dau bang 'Tinh Than Thach_'),
// cot 15..24 = gia tri theo CAP SAO CUA LO (1..10).
// Loai thuoc tinh lay o magicattrib_ge cot 5.
// Da do: 34/34 vien co dong magicattrib_ge TRUNG KHIT giua Linux va JX1, va JX1 co
// dung 34 dong da sao -> KHONG phai nan ma thuoc tinh, dung thang so cua ban Linux.
#define PF_STARSTONE_FILE	"\\settings\\item\\starstone.txt"
#define PF_MAGICATTRIB_GE	"\\settings\\item\\magicattrib_ge.txt"
#define PF_MAX_STONE_ID		64

struct KPfStoneInfo
{
	int		nAttribType;
	int		nValue[10];
	char	szName[64];		// ten vien da, cot 1 starstone.txt -- de in ra bang mo ta
};

// Nap 1 lan roi cache. Tra NULL neu vien da khong hop le.
static const KPfStoneInfo* PF_GetStoneInfo(int nStoneId)
{
	static BOOL			s_bLoaded = FALSE;
	static KPfStoneInfo	s_tb[PF_MAX_STONE_ID];
	if (!s_bLoaded)
	{
		s_bLoaded = TRUE;
		memset(s_tb, 0, sizeof(s_tb));
		KTabFile tfS, tfG;
		if (tfS.Load(PF_STARSTONE_FILE) && tfG.Load(PF_MAGICATTRIB_GE))
		{
			int nHS = tfS.GetHeight();
			int nHG = tfG.GetHeight();
			for (int r = 2; r <= nHS; r++)
			{
				int nP = 0, nGe = 0, nType = 0;
				tfS.GetInteger(r, 4, 0, &nP);
				tfS.GetInteger(r, 14, 0, &nGe);
				if (nP <= 0 || nP >= PF_MAX_STONE_ID)
					continue;
				if (nGe <= 0 || nGe + 1 > nHG)
					continue;
				tfG.GetInteger(nGe + 1, 5, 0, &nType);	// 0-based -> dong KTabFile
				if (nType <= 0)
					continue;
				s_tb[nP].nAttribType = nType;
				tfS.GetString(r, 1, "", s_tb[nP].szName, sizeof(s_tb[nP].szName));
				for (int lv = 0; lv < 10; lv++)
					tfS.GetInteger(r, 15 + lv, 0, &s_tb[nP].nValue[lv]);
			}
		}
	}
	if (nStoneId <= 0 || nStoneId >= PF_MAX_STONE_ID)
		return NULL;
	if (s_tb[nStoneId].nAttribType <= 0)
		return NULL;
	return &s_tb[nStoneId];
}

// Ap (bAdd=TRUE) hoac go (bAdd=FALSE) thuoc tinh cua toi da 5 lo kham.
// Lo 0 sao khong cho gi -- dung nhu ban Linux (CheckInlayStarStone chan kham vao lo 0 sao).
void KItem::PF_ModifyStoneAttrib(KNpc* pNPC, BOOL bAdd) const
{
	if (!pNPC)
		return;
	if (GetMaxStoneNum() <= 0)
		return;
	for (int i = 1; i <= PF_MAX_STONE; i++)
	{
		int nStone = GetStoneId(i);
		if (nStone <= 0)
			continue;
		int nLv = GetStoneLevel(i);
		if (nLv < 1 || nLv > 10)
			continue;
		const KPfStoneInfo* p = PF_GetStoneInfo(nStone);
		if (!p)
			continue;
		int nVal = p->nValue[nLv - 1];
		if (nVal == 0)
			continue;
		KItemNormalAttrib sA;
		sA.nAttribType = p->nAttribType;
		sA.nValue[0] = bAdd ? nVal : -nVal;
		sA.nValue[1] = -1;
		sA.nValue[2] = 0;
		pNPC->ModifyAttrib(pNPC->m_Index, (void*)&sA);
	}
}

//////////////////////////////////////////////////////////////////////////
// Tien to so sao dat truoc TEN mon do: "10 sao <ten>".
// Tra chuoi rong neu mon khong phai do co he sao -> moi mon khac giu nguyen.
//////////////////////////////////////////////////////////////////////////
const char* KItem::PF_StarPrefix() const
{
	static char s_sz[32];
	s_sz[0] = 0;
	int nStar = GetStarLevel();
	if (nStar > 0)
		sprintf(s_sz, "%d sao ", nStar);
	return s_sz;
}

//////////////////////////////////////////////////////////////////////////
// Phan rieng cua Phi Phong trong bang mo ta:
//   - diem chuc phuc hien tai / toi da
//   - moi lo kham mot dong: "<cap> sao <ten da>" hoac "<ten> Lo kham trong"
//   - cac dong thuoc tinh do da cong lai (dung g_MagicDesc cho khop cach
//     hanh van voi moi dong khac trong bang)
// Mon khong co lo kham thi ham nay khong in gi ca.
//////////////////////////////////////////////////////////////////////////
void KItem::PF_AppendDesc(char* pszMsg) const
{
	if (!pszMsg)
		return;
	// [VA 31/08i] VIEN DA KHAM (genre 9) tu gioi thieu thuoc tinh cua chinh no.
	// Ban Linux khong lam viec nay - moi vien dung chung mot cau gioi thieu
	// chung chung trong starstone.txt (da doi chieu: cot mo ta trung tung byte
	// giua hai cay), nen nguoi choi khong biet vien nao cho thuoc tinh gi.
	// THEM MOI theo yeu cau chu game 31/08.
	if (m_CommonAttrib.nItemGenre == item_starstone)
	{
#ifndef _SERVER
		const KPfStoneInfo* pDa = PF_GetStoneInfo(m_CommonAttrib.nParticularType);
		if (pDa && pDa->nAttribType > 0)
		{
			KItemNormalAttrib sA;
			memset(&sA, 0, sizeof(sA));
			sA.nAttribType = pDa->nAttribType;
			sA.nValue[1] = -1;
			sA.nValue[2] = 0;
			// dong chinh: gia tri khi kham vao lo DA KICH HOAT (10 sao)
			sA.nValue[0] = pDa->nValue[9];
			char* pszTen = (char*)g_MagicDesc.GetDesc(&sA);
			if (pszTen && pszTen[0])
			{
				strcat(pszMsg, "  \n  ");
				strcat(pszMsg, "<color=200,120,255>");
				strcat(pszMsg, pszTen);
				strcat(pszMsg, " <color>  \n  ");
				// dai gia tri theo cap lo, de nguoi choi uoc luong truoc khi kham
				char szDai[128];
				sprintf(szDai, "<color=Green>LÁ 1 sao: %d  Æ’n  lÁ 10 sao: %d<color>",
					pDa->nValue[0], pDa->nValue[9]);
				strcat(pszMsg, szDai);
				strcat(pszMsg, "  \n  ");
				strcat(pszMsg, "<color=Yellow>Kh∂m vµo lÁ ch≠a k›ch hoπt (0 sao) sœ nªm ngÒ, kh´ng cÈng g◊. <color>");
				strcat(pszMsg, "  \n  <color=255,255,255>");
			}
		}
#endif
		return;
	}
	int nHole = GetMaxStoneNum();
	int nStar = GetStarLevel();
	int nWishMax = GetMaxWishValue();
	if (nHole <= 0 && nStar <= 0 && nWishMax <= 0)
		return;

	char szLine[256];

	// --- diem chuc phuc: G_STR_MANTLESYSTEM_BLESS_VALUE ---
	if (nWishMax > 0)
	{
		strcat(pszMsg, "  \n  ");
		sprintf(szLine, "<color=HBlue>\247\351t ph\270 \256i\323m ch\363c ph\363c %d/%d<color>",
			GetCurWishValue(), nWishMax);
		strcat(pszMsg, szLine);
		strcat(pszMsg, "  \n  ");
	}

	if (nHole <= 0)
		return;

	// --- tung lo kham ---
	// So truoc chu "sao" la CAP CUA LO, khong phai cap cua mon: lo chua co da
	// van co cap rieng (ProcessSecBreakThrough dat cap TRUOC khi kham da).
	int i;
	for (i = 1; i <= nHole && i <= PF_MAX_STONE; i++)
	{
		int nStone = GetStoneId(i);
		int nLv    = GetStoneLevel(i);
		const KPfStoneInfo* p = (nStone > 0) ? PF_GetStoneInfo(nStone) : NULL;
		if (p && p->szName[0])
			sprintf(szLine, "<color=Green>%d sao %s<color>", nLv, p->szName);	// G_STR_COLOR_XING
		else
			sprintf(szLine, "<color=Green>%d sao L\347 kh\266m tr\350ng<color>", nLv);	// G_STR_COLOR_EMPTY_XING
		strcat(pszMsg, szLine);
		strcat(pszMsg, "  \n  ");
	}

	// --- thuoc tinh cua tung vien da: MOI VIEN MOT DONG, khong gop ---
#ifndef _SERVER
	for (i = 1; i <= nHole && i <= PF_MAX_STONE; i++)
	{
		int nStone = GetStoneId(i);
		if (nStone <= 0)
			continue;
		int nLv = GetStoneLevel(i);
		if (nLv < 1 || nLv > 10)
			continue;
		const KPfStoneInfo* p = PF_GetStoneInfo(nStone);
		if (!p || p->nValue[nLv - 1] == 0)
			continue;
		KItemNormalAttrib sA;
		memset(&sA, 0, sizeof(sA));
		sA.nAttribType = p->nAttribType;
		sA.nValue[0] = p->nValue[nLv - 1];
		sA.nValue[1] = -1;
		sA.nValue[2] = 0;
		char* pszInfo = (char*)g_MagicDesc.GetDesc(&sA);
		if (!pszInfo || !pszInfo[0])
			continue;
		// [VA 31/08e] ban chuan to khoi thuoc tinh da mau TIM (thuoc tinh an);
		// dau cach truoc <color> dong de TEncodeText khong nuot dau < khi dong
		// mo ta ket thuc bang so le byte cao (luat RULE 0).
		strcat(pszMsg, "<color=200,120,255>");
		strcat(pszMsg, pszInfo);
		strcat(pszMsg, " <color>  \n  ");
	}
#endif
	strcat(pszMsg, "<color=255,255,255>");
}

// ======== het khoi Tinh Than Thach ========

void KItem::ApplyMagicAttribToNPC(IN KNpc* pNPC, IN int nMagicActive /* = 0 */, IN int nMagicActiveE /* = 0 */) const
{
	_ASSERT(this != NULL);
	_ASSERT(nMagicActive >= 0);

	int nCount = nMagicActive;
	int nCountE = nMagicActiveE;
	int i;

	float fScale = 1.0f;

	if (m_CommonAttrib.nDetailType == equip_horse)
		fScale = m_fHorseScale;
	

	// ----------------- Base attribute -----------------
	for (i = 0; i < sizeof(m_aryBaseAttrib) / sizeof(m_aryBaseAttrib[0]); i++)
	{
		const KItemNormalAttrib* pAttrib = &(m_aryBaseAttrib[i]);
		if (INVALID_ATTRIB != pAttrib->nAttribType)
		{
			if (m_CommonAttrib.nDetailType == equip_horse &&
			pAttrib->nAttribType == 139 && m_bHorseScaleOnly)
			{
				continue;
			}
			KItemNormalAttrib AddAttrib = *pAttrib;

			AddAttrib.nValue[0] = ScaleValue(AddAttrib.nValue[0], fScale);
			AddAttrib.nValue[1] = ScaleValue(AddAttrib.nValue[1], fScale);
			AddAttrib.nValue[2] = ScaleValue(AddAttrib.nValue[2], fScale);

			pNPC->ModifyAttrib(pNPC->m_Index, (void*)&AddAttrib);
		}
	}

	// [PHI PHONG] ap thuoc tinh cua Tinh Than Thach da kham
	PF_ModifyStoneAttrib(pNPC, TRUE);

	// ----------------- Magic attribute -----------------
	for (i = 0; i < sizeof(m_aryMagicAttrib) / sizeof(m_aryMagicAttrib[0]); i++)
	{
		const KItemNormalAttrib* pAttrib = &(m_aryMagicAttrib[i]);

		if (INVALID_ATTRIB != pAttrib->nAttribType)
		{
			if (i >= MAX_ITEM_NORMAL_MAGICATTRIB)
			{
				// Hidden magic (nCountE)
				if (nCountE > 0)
				{
					KItemNormalAttrib ModifiedAttrib;
					ModifiedAttrib.nAttribType = pAttrib->nAttribType;
					for (int j = 0; j < 2; j++)
					{
						int base = ApplyScalingMethod(pAttrib->nValue[j], pAttrib->nAttribType);
						ModifiedAttrib.nValue[j] = ScaleValue(base, fScale);


					}
					ModifiedAttrib.nValue[2] = ScaleValue(pAttrib->nValue[2], fScale);

					pNPC->ModifyAttrib(pNPC->m_Index, (void*)&ModifiedAttrib);
					nCountE--;
				}
			}
			else
			{
				if (i & 1)    // suffix
				{
					if (nCount > 0)
					{
						KItemNormalAttrib ModifiedAttrib;
						ModifiedAttrib.nAttribType = pAttrib->nAttribType;
						for (int j = 0; j < 2; j++)
						{
							int base = ApplyScalingMethod(pAttrib->nValue[j], pAttrib->nAttribType);
							ModifiedAttrib.nValue[j] = ScaleValue(base, fScale);


						}
						ModifiedAttrib.nValue[2] = ScaleValue(pAttrib->nValue[2], fScale);

						pNPC->ModifyAttrib(pNPC->m_Index, (void*)&ModifiedAttrib);
						nCount--;
					}
				}
				else
				{
					KItemNormalAttrib ModifiedAttrib;
					ModifiedAttrib.nAttribType = pAttrib->nAttribType;
					for (int j = 0; j < 2; j++)
					{
						int base = ApplyScalingMethod(pAttrib->nValue[j], pAttrib->nAttribType);
						ModifiedAttrib.nValue[j] = ScaleValue(base, fScale);

					}
					ModifiedAttrib.nValue[2] = ScaleValue(pAttrib->nValue[2], fScale);

					pNPC->ModifyAttrib(pNPC->m_Index, (void*)&ModifiedAttrib);
				}
			}
		}
	}
}




/******************************************************************************
Function: Remove the magic on the item from the NPC
Entry	: pNPC: pointer to NPC, nMagicAcive: number of opened hidden attributes
Exit 	: Magic is applied.
The specific work is completed by the member functions of KNpc.
No member variables of the KItem object itself are modified
******************************************************************************/
void KItem::RemoveMagicAttribFromNPC(IN KNpc* pNPC, IN int nMagicActive /* = 0 */, IN int nMagicActiveE  /* = 0 */) const
{
	_ASSERT(this != NULL);
	_ASSERT(nMagicActive >= 0);

	int nCount = nMagicActive;
	int nCountE = nMagicActiveE;

	// [PHI PHONG] go thuoc tinh cua Tinh Than Thach khi thao trang bi
	PF_ModifyStoneAttrib(pNPC, FALSE);

	float fScale = 1.0f;

	
	fScale = m_fHorseScale;

	/* ------------------------------
	   REMOVE BASE ATTRIBUTES
	   ------------------------------ */
	for (int i = 0; i < sizeof(m_aryBaseAttrib) / sizeof(m_aryBaseAttrib[0]); i++)
	{
		const KItemNormalAttrib* pAttrib = &(m_aryBaseAttrib[i]);
		if (INVALID_ATTRIB != pAttrib->nAttribType)
		{
			if (m_CommonAttrib.nDetailType == equip_horse &&
			pAttrib->nAttribType == 139)
			{
				continue;
			}
			KItemNormalAttrib RemoveAttrib;
			RemoveAttrib.nAttribType = pAttrib->nAttribType;

			RemoveAttrib.nValue[0] = -ScaleValue(pAttrib->nValue[0], fScale);
			RemoveAttrib.nValue[1] = -ScaleValue(pAttrib->nValue[1], fScale);
			RemoveAttrib.nValue[2] = -ScaleValue(pAttrib->nValue[2], fScale);

		//	RemoveAttrib.nValue[0] = -(int)(pAttrib->nValue[0] * fScale);
		//	RemoveAttrib.nValue[1] = -(int)(pAttrib->nValue[1] * fScale);
		//	RemoveAttrib.nValue[2] = -(int)(pAttrib->nValue[2] * fScale);

			pNPC->ModifyAttrib(pNPC->m_Index, (void*)&RemoveAttrib);
		}
	}

	/* ------------------------------
	   REMOVE MAGIC ATTRIBUTES
	   ------------------------------ */
	for (int i = 0; i < sizeof(m_aryMagicAttrib) / sizeof(m_aryMagicAttrib[0]); i++)
	{
		const KItemNormalAttrib* pAttrib = &(m_aryMagicAttrib[i]);
		if (INVALID_ATTRIB == pAttrib->nAttribType)
			continue;

		KItemNormalAttrib RemoveAttrib;
		RemoveAttrib.nAttribType = pAttrib->nAttribType;

		// Hidden magic
		if (i >= MAX_ITEM_NORMAL_MAGICATTRIB)
		{
			if (nCountE > 0)
			{
				for (int j = 0; j < 2; j++)
				{
					int base = ApplyScalingMethod(pAttrib->nValue[j], pAttrib->nAttribType);
					RemoveAttrib.nValue[j] = -ScaleValue(base, fScale);
				}
				RemoveAttrib.nValue[2] = -ScaleValue(pAttrib->nValue[2], fScale);

				pNPC->ModifyAttrib(pNPC->m_Index, (void*)&RemoveAttrib);
				nCountE--;
			}
		}
		else
		{
			// suffix (odd index)
			if (i & 1)
			{
				if (nCount > 0)
				{
					for (int j = 0; j < 2; j++)
					{
						int base = ApplyScalingMethod(pAttrib->nValue[j], pAttrib->nAttribType);
						RemoveAttrib.nValue[j] = -ScaleValue(base, fScale);
					}
					RemoveAttrib.nValue[2] = -ScaleValue(pAttrib->nValue[2], fScale);

					pNPC->ModifyAttrib(pNPC->m_Index, (void*)&RemoveAttrib);
					nCount--;
				}
			}
			else    // prefix
			{
				for (int j = 0; j < 2; j++)
				{
					int base = ApplyScalingMethod(pAttrib->nValue[j], pAttrib->nAttribType);
					RemoveAttrib.nValue[j] = -ScaleValue(base, fScale);
				}
				RemoveAttrib.nValue[2] = -ScaleValue(pAttrib->nValue[2], fScale);

				pNPC->ModifyAttrib(pNPC->m_Index, (void*)&RemoveAttrib);
			}
		}
	}
}



/******************************************************************************
Function: Apply the Nth hidden magic attribute on the item to the NPC
Entry	: pNPC: pointer to NPC
Exit 	: Magic is applied.
			The specific work is completed by the member functions of KNpc.
			No member variables of the KItem object itself are modified
******************************************************************************/
void KItem::ApplyHiddenMagicAttribToNPC(IN KNpc* pNPC, IN int nMagicActive) const
{
	_ASSERT(this != NULL);
	if (nMagicActive <= 0)
		return;

	const KItemNormalAttrib* pAttrib;
	pAttrib = &(m_aryMagicAttrib[(nMagicActive << 1) - 1]);	
	if (-1 != pAttrib->nAttribType)
	{
		KItemNormalAttrib ModifiedAttrib;
		ModifiedAttrib.nAttribType = pAttrib->nAttribType;
		for (int j = 0; j < 2; j++) {
			ModifiedAttrib.nValue[j] = ApplyScalingMethod(pAttrib->nValue[j], pAttrib->nAttribType);
		}
		ModifiedAttrib.nValue[2] = pAttrib->nValue[2];
		pNPC->ModifyAttrib(pNPC->m_Index, (void*)&ModifiedAttrib);
	}
}
/******************************************************************************
Function: Remove the Nth hidden magic attribute on the item from the NPC
Entry	: pNPC: pointer to NPC, nMagicActive: nth magic attribute
Exit 	: Magic removed.
The specific work is completed by the member functions of KNpc.
No member variables of the KItem object itself are modified
******************************************************************************/
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
		for (int j = 0; j < 2; j++) {
			RemoveAttrib.nValue[j] -= ApplyScalingMethod(pAttrib->nValue[j], pAttrib->nAttribType);
		}
		RemoveAttrib.nValue[2] -= pAttrib->nValue[2];
		pNPC->ModifyAttrib(pNPC->m_Index, (void*)&RemoveAttrib);
	}
}
/******************************************************************************
Function: According to the data in the configuration file, assign initial values to each item
Entry	: pData: gives data from the configuration file
Exit 	: Returns non-zero on success, and the following member variables are evaluated:
			m_CommonAttrib,m_aryBaseAttrib,m_aryRequireAttrib
			Returns zero on failure
Description: CBR: Common,Base,Require
******************************************************************************/
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

BOOL KItem::SetAttrib_CBR(IN const KBASICPROP_EQUIPMENT_GOLD2* pData)
{
	_ASSERT(pData != NULL);

	BOOL bEC = FALSE;
	if (pData)
	{
		*this = *pData;		// Operator overloading
		SetAttrib_Base(pData->m_aryPropBasic);
		SetAttrib_Req(pData->m_aryPropReq);
		bEC = TRUE;
	}
	return bEC;
}

BOOL KItem::SetAttrib_CBR(IN const KBASICPROP_EQUIPMENT_PLATINA* pData)
{
	_ASSERT(pData != NULL);

	BOOL bEC = FALSE;
	if (pData)
	{
		*this = *pData;		// Operator overloading
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

/******************************************************************************
Function: According to the incoming data, assign an initial value to the magic attribute of the item
Entry	: pMA: Give data
Exit 	: Returns non-zero on success, and the following member variables are evaluated:
			m_aryMagicAttrib
			Returns zero on failure
******************************************************************************/
BOOL KItem::SetAttrib_MA(IN const KItemNormalAttrib* pMA)
{
	if (NULL == pMA)
	{
		_ASSERT(FALSE); return FALSE;
	}
	for (int i = 0; i < sizeof(m_aryMagicAttrib) / sizeof(m_aryMagicAttrib[0]); i++)
	{
		m_aryMagicAttrib[i] = pMA[i];
		if (m_aryMagicAttrib[i].nAttribType == magic_indestructible_b)
		{
			SetDurability(-1);
		}
	}
	return TRUE;
}

/******************************************************************************
Function: According to the incoming data, assign an initial value to the magic attribute of the item
Entry	: pMA: Give data
Exit 	: Returns non-zero on success, and the following member variables are evaluated:
		m_aryMagicAttrib
		Returns zero on failure
******************************************************************************/
BOOL KItem::SetAttrib_MA(IN const KMACP* pMA)
{
	if (NULL == pMA)
	{
		_ASSERT(FALSE); return FALSE;
	}

	for (int i = 0; i < sizeof(m_aryMagicAttrib) / sizeof(m_aryMagicAttrib[0]); i++)
	{
		const KMACP* pSrc;
		KItemNormalAttrib* pDst;
		pSrc = &(pMA[i]);
		pDst = &(m_aryMagicAttrib[i]);

		pDst->nAttribType = pSrc->nPropKind;
		pDst->nValue[0] = ::GetRandomNumber(pSrc->aryRange[0].nMin, pSrc->aryRange[0].nMax);
		pDst->nValue[1] = ::GetRandomNumber(pSrc->aryRange[1].nMin, pSrc->aryRange[1].nMax);
		pDst->nValue[2] = ::GetRandomNumber(pSrc->aryRange[2].nMin, pSrc->aryRange[2].nMax);
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

// [PHI PHONG 2026-08-29] nap ban ghi Tinh Than Thach vao vat pham.
void KItem::operator = (const KBASICPROP_STARSTONE& sData)
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->nItemGenre		 = sData.m_nItemGenre;
	pCA->nDetailType	 = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticurType;
	pCA->nObjIdx		 = sData.m_nObjIdx;
	pCA->nWidth			 = sData.m_nWidth;
	pCA->nHeight		 = sData.m_nHeight;
	pCA->nPrice			 = sData.m_nPrice;
	pCA->nLevel			 = sData.m_nLevel > 0 ? sData.m_nLevel : 1;
	pCA->nSeries		 = -1;
	pCA->nSet			 = 0;
	pCA->nSetId			 = 0;
	pCA->nSetNum		 = 0;
	pCA->nGoldId		 = 0;
	pCA->nStackNum		 = 1;
	pCA->nEnChance		 = 0;
	pCA->nPoint			 = 0;
	pCA->bStack			 = sData.m_bStack;
	pCA->nMaxStack		 = sData.m_nMaxStack > 0 ? sData.m_nMaxStack : 1;
	::strcpy(pCA->szItemName, sData.m_szName);
	pCA->LimitTime.bYear = 0;
	pCA->LimitTime.bMonth = 0;
	pCA->LimitTime.bDay = 0;
	pCA->LimitTime.bHour = 0;
	pCA->uPrice = 0;
	// [VA 31/08e] ve sinh khe tai su dung: AddItemSet2 KHONG zero m_CommonAttrib,
	// truong nao khong gan o day la giu RAC cua mon truoc do trong cung khe.
	pCA->szScript[0]	 = 0;
	pCA->nPickExecute	 = 0;
	pCA->nIsSell		 = 1;
	pCA->nIsTrade		 = 1;
#ifndef _SERVER
	// [VA 31/08e] khoi CLIENT bi bo sot khi port 29/08 (moi operator= khac deu
	// co): khong chep ten anh + mo ta va khong khoi tao m_Image -> vien da nhan
	// ve VO HINH, chuot phai khong co thong tin. Server khong dung cac truong
	// nay nen khong ai thay tu 29/08.
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


void KItem::operator = (const KBASICPROP_EQUIPMENT_PLATINA& sData)
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->bTemp = FALSE;
	pCA->BackLocal.Release();
	pCA->nItemNature = NATURE_PLATINA;
	pCA->nItemGenre = sData.m_nItemGenre;
	pCA->nDetailType = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx = sData.m_nObjIdx;
	pCA->nPrice = sData.m_nPrice;
	pCA->nNewPrice = sData.m_nPrice;
	pCA->bNewArrival = FALSE;
	pCA->nLevel = sData.m_nLevel;
	pCA->nSeries = sData.m_nSeries;
	pCA->bShortKey = FALSE;
	pCA->nWidth = sData.m_nWidth;
	pCA->nHeight = sData.m_nHeight;
	pCA->nStackNum = 1;
	pCA->nMaxStack = 1;
	pCA->nExpirePoint = 0;
	pCA->nRow = -1;
	pCA->nGroup = sData.m_nGroup;
	pCA->nSetID = sData.m_nSetID;
	pCA->nNeedToActive1 = sData.m_nNeedToActive1;
	pCA->nNeedToActive2 = sData.m_nNeedToActive2;
	pCA->nMantle = 0;
	pCA->uFlash = 0;
	pCA->nUpgradeLvl = 0;
	pCA->nPhysicVal = 0;
	pCA->nMagicVal = 0;
	pCA->bLockSell = FALSE;
	pCA->bLockTrade = FALSE;
	pCA->bLockDrop = FALSE;
	pCA->nParam = -1;
	pCA->nFortune = 0;
	pCA->nExpireTime = 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName, sData.m_szName);
	::memset(pCA->szScript, 0, sizeof(pCA->szScript));
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro, sData.m_szIntro);
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_EQUIPMENT_GOLD2& sData)
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->bTemp = FALSE;
	pCA->BackLocal.Release();
	pCA->nItemNature = NATURE_GOLD;
	pCA->nItemGenre = sData.m_nItemGenre;
	pCA->nDetailType = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx = sData.m_nObjIdx;
	pCA->nPrice = sData.m_nPrice;
	pCA->nNewPrice = sData.m_nPrice;
	pCA->bNewArrival = FALSE;
	pCA->nLevel = sData.m_nLevel;
	pCA->nSeries = sData.m_nSeries;
	pCA->bShortKey = FALSE;
	pCA->nWidth = sData.m_nWidth;
	pCA->nHeight = sData.m_nHeight;
	pCA->nStackNum = 1;
	pCA->nMaxStack = 1;
	pCA->nExpirePoint = 0;
	pCA->nRow = -1;
	pCA->nGroup = sData.m_nGroup;
	pCA->nSet = sData.m_nGroup; //goldequip min number items in set to active >=1
	pCA->nSetNum = sData.m_nNeedToActive2; //goldequip number items in set >=1
	pCA->nSetID = sData.m_nSetID;
	pCA->nSetIDNo = sData.m_nSetIDNo;
	//pCA->nNeedToActive1 = sData.m_nNeedToActive1;
	//pCA->nNeedToActive2 = sData.m_nNeedToActive2;
	pCA->nMantle = 0;
	pCA->uFlash = 0;
	pCA->nUpgradeLvl = 0;
	pCA->nPhysicVal = 0;
	pCA->nMagicVal = 0;
	pCA->bLockSell = FALSE;
	pCA->bLockTrade = FALSE;
	pCA->bLockDrop = FALSE;
	pCA->nParam = -1;
	pCA->nFortune = 0;
	pCA->nExpireTime = 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName, sData.m_szName);
	::memset(pCA->szScript, 0, sizeof(pCA->szScript));
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro, sData.m_szIntro);
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}


void KItem::Remove()
{
	m_nIndex = 0;
	Reset();
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
//------------------------------------------------ ------------------
// Wear, the return value indicates the remaining durability
//------------------------------------------------ ------------------
int KItem::Abrade(IN const int nAbradeP, IN const int nRange)//#mµi mﬂn
{
	if (m_nCurrentDur == -1 || nRange == 0)	// Never wear out
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
		//m_CommonAttrib.nWidth = 1;
		//m_CommonAttrib.nHeight = 1;
		//strcpy(m_Image.szImage, BROKEN_ITEM_SPR); //CÙc sæt cuc sat
		//m_Image.oPosition.nX = nX - 1; //Vœ lπi vﬁ tr› Icon
		//m_Image.oPosition.nY = nY - 2;
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
			strcat(pszMsg, PF_StarPrefix());	// "10 sao " neu la do co he sao
			strcat(pszMsg, TextLevel);
		}
		else
		{
			strcat(pszMsg, PF_StarPrefix());
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
			strcat(pszMsg, PF_StarPrefix());
			strcat(pszMsg, TextLevel);
		}
		else
		{
			strcat(pszMsg, "<color=White>");
			strcat(pszMsg, PF_StarPrefix());
			strcat(pszMsg, m_CommonAttrib.szItemName);
		}
	}
	//
	if (m_CommonAttrib.nItemGenre == item_magicscript && m_CommonAttrib.nParticularType == 146) //Huyen tinh khoang thach
	{
		sprintf(TextLevel, " [C p %d]", m_CommonAttrib.nLevel);
		strcat(pszMsg, TextLevel);
	}
	//if (m_MaxOptMultiply >= 1) {
		//sprintf(TextLevel, " [x%d ThuÈc T›nh]", m_MaxOptMultiply);
		//strcat(pszMsg, TextLevel);
	//}
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

	if(InsuranceCourse == LOCK_STATE_FOREVER)//#kho∏ v‹nh vi‘n
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
					char szPrice2[64];
					sprintf(szPrice2, "<color=Green>ThÍi gian chÍ mÎ kho∏ : %d giÍ", g_conlai);
					strcat(pszMsg, szPrice2);
					strcat(pszMsg, "  \n  ");
				}
				else if(p_conlai > 1)
				{
					char szPrice2[64];
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
		char szPrice[64];
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
		char pszTemp2[256];
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
		// [LOREN 27/08] Vien khoang O AN co ngu hanh that va PHAI hien ra:
		// ban goc in dong ngu hanh cho chung (prop_ore.lua:21-23 voi day
		// khoang thuoc tinh, primitive_ore.lua:14-16 voi day nguyen khoang),
		// vi o AN (pos chan) bat buoc cung ngu hanh voi trang bi
		// (equip_enchase.lua:67, magic_distill.lua:51). O HIEN thi khong hien.
		// JX1 lech -1 so voi Linux nen parity DAO:
		//   nguyen khoang    148..153 -> o AN la ptc LE   (149,151,153)
		//   khoang thuoc tinh 199..204 -> o AN la ptc CHAN (200,202,204)
		BOOL bKhoangAn =
			(nPar >= 148 && nPar <= 153 && (nPar % 2) == 1) ||
			(nPar >= 199 && nPar <= 204 && (nPar % 2) == 0);
		if(nPar == 398 || nPar == 399) //s∏t thÒ l÷nh, s∏t thÒ gi∂n
			nfkSerial = m_CommonAttrib.nSeries;
		else if(bKhoangAn)
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
		strcat(pszMsg, "  <color=White>ThuÈc t›nh NgÚ hµnh: <color=Earth>ThÊ ");
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
		// [LOREN 27/08] 8 byte KHONG du: "%d" cua mot int co the dai 11 ky tu
		// ("-2147483648") + NUL = 12. Khi m_aryBaseAttrib[0].nValue[0] mang so
		// la (khe vat pham dung lai, xem KItemSet::AddItemSet2) thi sprintf pha
		// khung stack ngay tai day -> client sap CUNG, bo bat loi khong kip ghi.
		char szTmp[16];
		// [LOREN 27/08] Ban goc doc MAGIC_ID cua vien khoang bang
		// GetItemParam(idx,1) = m_GeneratorParam.nGeneratorLevel[0]
		// (ScriptFuns.cpp:5811; equip_enchase.lua:68, prop_ore.lua:32), va
		// duong ghi cung ghi vao do (KItemCompound.cpp:1364).
		// m_aryBaseAttrib[0].nValue[0] KHONG AI GHI (bi ZeroMemory o
		// KItem.cpp:890) nen tra bang luon truot -> dong "Thuoc tinh:" rong.
		sprintf(szTmp,"%d",m_GeneratorParam.nGeneratorLevel[0]);

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

	// [LOREN 28/08] chu giai + xem truoc Do pho.
	// Chu game: "item do pho thieu noi dung can nhung nguyen lieu nao pham
	// chat bao nhieu". Cong thuc doc tu atlas_compound.txt - LAY DONG DAU
	// TIEN khop ma do pho, dung dong ma isAtlas (atlas.lua:110) dung khi ghep
	// that, nen tooltip va luat kiem luon khop nhau.
	// Dai 238..390 = dai ma do pho that sau khi nan bang (132 ma).
	if (m_CommonAttrib.nItemGenre == item_magicscript && m_CommonAttrib.nDetailType == 1 &&
		m_CommonAttrib.nParticularType >= 238 && m_CommonAttrib.nParticularType <= 390)
	{
		KTabFile AtlasTab;
		if (AtlasTab.Load("\\Settings\\Item\\atlas_compound.txt"))
		{
			int nHang = 0;
			for (int r = 2; r <= AtlasTab.GetHeight(); r++)
			{
				int nG = -1, nD = -1, nP = -1;
				AtlasTab.GetInteger(r, (LPSTR)"ATLAS_GENRE", -1, &nG);
				AtlasTab.GetInteger(r, (LPSTR)"ATLAS_DETAILTYPE", -1, &nD);
				AtlasTab.GetInteger(r, (LPSTR)"ATLAS_PARTICULAR", -1, &nP);
				if (nG == 6 && nD == 1 && nP == m_CommonAttrib.nParticularType)
				{
					nHang = r;
					break;
				}
			}
			if (nHang >= 2)
			{
				KTabFile MagicIdx;
				BOOL bMagic = MagicIdx.Load("\\Settings\\Item\\magicattriblevel_index.txt");
				strcat(pszMsg, " <color=Fire>Nguy™n li÷u c«n:  \n  ");
				for (int k = 1; k <= 6; k++)
				{
					char szCot[32];
					char szTenNl[64];
					_snprintf(szCot, sizeof(szCot) - 1, "%d_NAME", k);
					szCot[sizeof(szCot) - 1] = 0;
					szTenNl[0] = 0;
					AtlasTab.GetString(nHang, szCot, (LPSTR)"", szTenNl, sizeof(szTenNl));
					if (szTenNl[0] == 0)
						continue;
					int nCap = -1, nMg = -1;
					_snprintf(szCot, sizeof(szCot) - 1, "%d_LEVEL", k);
					szCot[sizeof(szCot) - 1] = 0;
					AtlasTab.GetInteger(nHang, szCot, -1, &nCap);
					_snprintf(szCot, sizeof(szCot) - 1, "%d_MAGIC_ID", k);
					szCot[sizeof(szCot) - 1] = 0;
					AtlasTab.GetInteger(nHang, szCot, -1, &nMg);
					char szDescNl[64];
					szDescNl[0] = 0;
					if (bMagic && nMg > 0)
					{
						char szKey[16];
						_snprintf(szKey, sizeof(szKey) - 1, "%d", nMg);
						szKey[sizeof(szKey) - 1] = 0;
						MagicIdx.GetString(szKey, (LPSTR)"DESC", (LPSTR)"", szDescNl, sizeof(szDescNl));
					}
					char szDongNl[200];
					if (nCap > 0 && szDescNl[0])
						_snprintf(szDongNl, sizeof(szDongNl) - 1, " <color=Green>%s (c p %d) - %s  \n  ", szTenNl, nCap, szDescNl);
					else if (nCap > 0)
						_snprintf(szDongNl, sizeof(szDongNl) - 1, " <color=Green>%s (c p %d)  \n  ", szTenNl, nCap);
					else
						_snprintf(szDongNl, sizeof(szDongNl) - 1, " <color=Green>%s  \n  ", szTenNl);
					szDongNl[sizeof(szDongNl) - 1] = 0;
					strcat(pszMsg, szDongNl);
				}
				strcat(pszMsg, " <color=White>KÃm: Huy“n Tinh Kho∏ng Thπch + 100000 l≠Óng  \n  ");
			}
		}
	}

	if (m_CommonAttrib.nItemGenre == item_magicscript && (m_CommonAttrib.nParticularType >= 199 && m_CommonAttrib.nParticularType <= 204))
	{
		char szLevel[128];
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
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_aryMagicAttrib[i]);
		KItemNormalAttrib ModifiedAttrib = *pAttrib;
		for (int j = 0; j < 2; j++) {
			ModifiedAttrib.nValue[j] = ApplyScalingMethod(pAttrib->nValue[j], pAttrib->nAttribType);
		}
		ModifiedAttrib.nValue[2] = pAttrib->nValue[2];
		char* pszInfo = (char *)g_MagicDesc.GetDesc((void*) & ModifiedAttrib);
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
	PF_AppendDesc(pszMsg);	// diem chuc phuc + tung lo kham + thuoc tinh da
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
int KItem::GetQuality()
{
	if (GetGenre() == item_equip)
	{
		if (m_nCurrentDur == 0)
			return equip_damage;
		else
		{
			switch (m_CommonAttrib.nItemNature)
			{
			case NATURE_GOLD:
				return equip_gold;
			case NATURE_PLATINA:
				return equip_platina;
			case NATURE_VIOLET:
				return equip_violet;
			}
		}
		if (m_CommonAttrib.nPoint > 0)
			return NATURE_VIOLET;
		if (m_aryMagicAttrib[0].nAttribType)
			return equip_magic;
		if (m_CommonAttrib.nGoldId)
		{
			return equip_gold;
		}
	}
	return equip_normal;
}
void KItem::GetDesc(char* pszMsg, bool bShowPrice, bool bPriceScale, int nActiveAttrib, int nGoldActiveAttrib)
{
	if (m_CommonAttrib.nGoldId || m_CommonAttrib.nPoint > 0 || m_CommonAttrib.nItemGenre > item_equip)
		return GetDesc(pszMsg, bShowPrice, 1, nActiveAttrib); //call old getdesc
	char pszKeyName[16];
	char pszTemp[256];
	char pszTemp2[256];

	memset(pszKeyName, 0, sizeof(pszKeyName));
	memset(pszTemp, 0, sizeof(pszTemp));
	memset(pszTemp2, 0, sizeof(pszTemp2));

	switch (this->GetQuality())
	{
	case equip_normal:
		strcat(pszMsg, "<color=255,255,255>");
		break;
	case equip_magic:
		strcat(pszMsg, "<color=100,100,255>");
		break;
	case equip_damage:
		strcat(pszMsg, "<color=255,0,66>");
		break;
	case equip_violet:
		strcat(pszMsg, "<color=170,30,255>");
		break;
	case equip_gold:
		strcat(pszMsg, "<color=255,255,0>");
		break;
	case equip_platina:
		strcat(pszMsg, "<bclr=100,100,255><color=255,255,0>");
		break;
	}
	strcat(pszMsg, PF_StarPrefix());	// "10 sao " neu la do co he sao
	strcat(pszMsg, m_CommonAttrib.szItemName);

	if (m_CommonAttrib.nItemGenre == item_equip)
	{
		if (m_CommonAttrib.nItemNature == NATURE_PLATINA)
		{
			sprintf(pszTemp, " +%d", m_CommonAttrib.nLevel);
		}
		else if (m_CommonAttrib.nDetailType <= equip_horse)
		{
			if (m_CommonAttrib.nLevel > MAX_ITEM_LEVEL)
			{
				if (m_CommonAttrib.nLevel < MAX_ITEM_LEVEL * 10)
				{
					if (m_CommonAttrib.nLevel % MAX_ITEM_LEVEL == 0)
						sprintf(pszTemp, " [c p %d]", m_CommonAttrib.nLevel / (m_CommonAttrib.nLevel / MAX_ITEM_LEVEL));
					else
						sprintf(pszTemp, " [c p %d]", m_CommonAttrib.nLevel % MAX_ITEM_LEVEL);
				}
				else if (m_CommonAttrib.nLevel < MAX_ITEM_LEVEL * 100)
				{
					if (m_CommonAttrib.nLevel % 100 == 0)
						sprintf(pszTemp, " [c p %d]", m_CommonAttrib.nLevel / (m_CommonAttrib.nLevel / MAX_ITEM_LEVEL * 10));
					else
						sprintf(pszTemp, " [c p %d]", m_CommonAttrib.nLevel % MAX_ITEM_LEVEL * 10);
				}
			}
			else if (m_CommonAttrib.nLevel > MIN_ITEM_LEVEL)
				sprintf(pszTemp, " [c p %d]", m_CommonAttrib.nLevel);
		}
		if (pszTemp[0])
			strcat(pszMsg, pszTemp);
	}
	if (m_MaxOptMultiply >= 1) {
		sprintf(pszTemp, "[x%d ThuÈc T›nh]", m_MaxOptMultiply);
		strcat(pszMsg, pszTemp);
	}
	strcat(pszMsg, "<bclr=0,0,0><color>");
	//strcat(pszMsg, "  \n  ");
	//if (InsuranceCourse == -2)//#kho∏ v‹nh vi‘n
	//{
	//	strcat(pszMsg, "<color=Green>VÀt ph»m nµy ÆuÓc kho∏ v‹nh vi‘n");
	//	strcat(pszMsg, "\r\n");
	//}

	//if (InsuranceCourse > 0)//#hπn sˆ dÙng
	//{
	//	if (InsuranceHourCourse > 0)
	//	{
	//		time_t baygio = time(0);
	//		int s_conlai = InsuranceHourCourse - baygio;
	//		if (s_conlai > 0)
	//		{
	//			DWORD p_conlai = s_conlai / 60;
	//			DWORD g_conlai = p_conlai / 60;
	//			if (g_conlai > 1)
	//			{
	//				char szPrice2[64];
	//				sprintf(szPrice2, "<color=Green>ThÍi gian chÍ mÎ kho∏ : %d giÍ", g_conlai);
	//				strcat(pszMsg, szPrice2);
	//				strcat(pszMsg, "  \n  ");
	//			}
	//			else if (p_conlai > 1)
	//			{
	//				char szPrice2[64];
	//				sprintf(szPrice2, "<color=Green>ThÍi gian chÍ mÎ kho∏ : %d phÛt", p_conlai);
	//				strcat(pszMsg, szPrice2);
	//				strcat(pszMsg, "  \n  ");
	//			}
	//		}
	//		else
	//		{
	//			strcat(pszMsg, "<color=Green>Hoµn t t thÍi gian chÍ mÎ kho∏.\r\n");
	//		}
	//	}
	//	else
	//	{
	//		strcat(pszMsg, "<color=Green>VÀt ph»m nµy ÆuÓc kho∏ b∂o hi”m");
	//		strcat(pszMsg, "\r\n");
	//	}
	//}

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
		if (bPriceScale)
		{
			nPrice = GetSalePrice();
			strcat(pszMsg, szPriceColor[moneyunit_money]);
			sprintf(pszTemp2, "%d", moneyunit_money);
		}
		else
		{
			nPrice = GetCurPrice();
			strcat(pszMsg, szPriceColor[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nMoneyUnit]);
			sprintf(pszTemp2, "%d", Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nMoneyUnit);
		}
		strcat(pszMsg, "\n");
		strcpy(pszTemp, "Gi c: ");
		strcat(pszMsg, pszTemp);

		g_GameSetting.GetString("MoneyUnit", pszTemp2, "", pszTemp, sizeof(pszTemp));
		sprintf(pszTemp2, "%d %s<color=255,255,255>", nPrice, pszTemp);
		strcat(pszMsg, pszTemp2);
	}

	pszTemp2[0] = 0;
	switch (m_CommonAttrib.LockItem.nState)
	{
	case LOCK_STATE_CHARACTER:
		sprintf(pszTemp2, "<color=0,255,0>VÀt ph»m Æ›nh kÃm theo nh©n vÀt<color>");
		break;
	case LOCK_STATE_FOREVER:
		sprintf(pszTemp2, "<color=0,255,0>VÀt ph»m nµy Æ∑ kh„a b∂o hi”m v‹nh vi‘n<color>");
		break;
	case LOCK_STATE_LOCK:
		sprintf(pszTemp2, "<color=0,255,0>VÀt ph»m nµy Æ∑ kh„a b∂o hi”m<color>");
		break;
	case LOCK_STATE_UNLOCK:
        if (m_CommonAttrib.LockItem.dwLockTime == 0) {
    			strcpy(pszTemp2, "<color=0,255,0>VÀt ph»m nµy Æ∑ kh„a tπm thÍi<color>");
    		}
    	else if (m_CommonAttrib.LockItem.dwLockTime > KSG_GetCurSec())
		{
			time_t nowtime = m_CommonAttrib.LockItem.dwLockTime;
			struct tm* timeinfo = localtime(&nowtime);
			strcpy(pszTemp, "<color=0,255,0>ThÍi gian m kh„a: %H:%M:%S %d-%m-%Y<color>");
			strftime(pszTemp2, sizeof(pszTemp2), pszTemp, timeinfo);
		}
		break;
	}
	if (pszTemp2[0])
	{
		strcat(pszMsg, "\n");
		strcat(pszMsg, pszTemp2);
	}

	sprintf(pszKeyName, "%d", m_CommonAttrib.nSeries);
	g_GameSetting.GetString("Elements", pszKeyName, "", pszTemp, sizeof(pszTemp));
	if (pszTemp[0])
	{
		strcat(pszMsg, "\n");
		strcat(pszMsg, pszTemp);
	}

	if (m_CommonAttrib.szIntro[0])
	{
		char szIntro[SZBUFLEN_512];

		int offset = 0, nL = 0, nS = 0;
		int nStrL = sprintf(szIntro, "%s", m_CommonAttrib.szIntro);
		while (szIntro[offset] != 0)
		{
			if (szIntro[offset] == '<')
			{
				if (szIntro[offset + 1] == 'e' &&
					szIntro[offset + 2] == 'n' &&
					szIntro[offset + 3] == 't' &&
					szIntro[offset + 4] == 'e' &&
					szIntro[offset + 5] == 'r' &&
					szIntro[offset + 6] == '>')
					nL = 0;
				if (szIntro[offset + 1] == 'c' &&
					szIntro[offset + 2] == 'o' &&
					szIntro[offset + 3] == 'l' &&
					szIntro[offset + 4] == 'o' &&
					szIntro[offset + 5] == 'r')
				{
					if (szIntro[offset + 6] == '>')
						nL -= 6;
					else
					{
						int k;
						for (k = 0; k < 12; k++)
						{
							if (szIntro[offset + 6 + k] == '>')
								break;
						}
						nL -= 6 + k;
					}
				}
				if (szIntro[offset + 1] == 'b' &&
					szIntro[offset + 2] == 'c' &&
					szIntro[offset + 3] == 'l' &&
					szIntro[offset + 4] == 'r')
				{
					if (szIntro[offset + 5] == '>')
						nL -= 5;
					else
					{
						if (szIntro[offset + 5] == '=')
						{
							int k;
							for (k = 0; k < 12; k++)
							{
								if (szIntro[offset + 5 + k] == '>')
									break;
							}
							nL -= 5 + k;
						}
					}
				}
			}

			if (nL == 32 && (offset + 7) < nStrL)
			{
				while (offset + nS < nStrL && szIntro[offset + nS] < 0 || (szIntro[offset + nS] > 32 && szIntro[offset + nS] < 126))
					nS++;
				if (nS >= 6)
				{
					memmove(&szIntro[offset + nS + 6], &szIntro[offset + nS], nStrL - offset + nS);
					memcpy(&szIntro[offset + nS], "<enter>", 7);
					offset += 7 + nS;
					nStrL += 6;
				}
				else
				{
					memmove(&szIntro[offset + 7], &szIntro[offset], nStrL - offset + 1);
					memcpy(&szIntro[offset + nS], "<enter>", 7);
					offset += 7 + nS;
					nStrL += 7;
				}
				nL = 0;
				nS = 0;
			}
			offset++;
			nL++;
		}
		if (strlen(szIntro) > (SZBUFLEN_1 - 1))
			szIntro[SZBUFLEN_1 - 1] = 0;

		strcat(pszMsg, "\n");
		strcat(pszMsg, szIntro);
	}
	if (m_CommonAttrib.nItemGenre == item_equip)
	{
		/*if (m_aryBaseAttrib[0].nAttribType > 0 ||
			m_aryRequireAttrib[0].nAttribType > 0 ||
			m_aryMagicAttrib[0].nAttribType > 0)*/
		strcat(pszMsg, "\n\n");
	}
	else
		strcat(pszMsg, "\n");

	if (m_CommonAttrib.nItemGenre == item_magicscript && (m_CommonAttrib.nParticularType >= 199 && m_CommonAttrib.nParticularType <= 204))
	{
		if (m_GeneratorParam.nLuck)
		{
			KTabFile MagicTab;

			MagicTab.Load(MAGICATTRIB_LVINDEX_FILE);

			sprintf(pszKeyName, "%d", m_GeneratorParam.nLuck);

			MagicTab.GetString(pszKeyName, "DESC", "", pszTemp, sizeof(pszTemp));
			strcat(pszMsg, "<color=100,100,255>ThuÈc t›nh: ");
			strcat(pszMsg, pszTemp);
			strcat(pszMsg, "\n");
			MagicTab.GetString(pszKeyName, "FIT_EQUIP", "", pszTemp, sizeof(pszTemp));
			strcat(pszMsg, "<color=255,219,74>Loπi trang b c th kh∂m nπm: ");
			strcat(pszMsg, pszTemp);
			strcat(pszMsg, "<color=255,255,255>");
			strcat(pszMsg, "\n");
		}

		if (m_CommonAttrib.nLevel)
		{
			sprintf(pszTemp, "<color=100,100,255>Ph»m ch t thuÈc t›nh: <color=255,255,0>%d<color=255,255,255>", m_CommonAttrib.nLevel);
			strcat(pszMsg, pszTemp);
		}
	}

	bool bActiveAllAttrib = true;
	int i = 0;
	for (i = 0; i < 7; i++)
	{
		if (!m_aryBaseAttrib[i].nAttribType)
		{
			continue;
		}
		if (m_aryBaseAttrib[i].nAttribType == magic_durability_v)
		{
			if (GetDurability() == -1)
				sprintf(pszTemp, "<color=255,255,0>Kh´ng th” ph∏ hÒy<color=255,255,255>");
			else
				sprintf(pszTemp, "ßÈ b“n: %3d / %3d", GetDurability(), GetMaxDurability());
			strcat(pszMsg, pszTemp);
		}
		else
		{
			char* pszTemp2 = (char*)g_MagicDesc.GetDesc(&m_aryBaseAttrib[i]);
			if (!pszTemp2 || !pszTemp2[0])
				continue;
			strcat(pszMsg, pszTemp2);
		}
		strcat(pszMsg, "\n");
	}
	for (i = 0; i < 6; i++)
	{
		if (!m_aryRequireAttrib[i].nAttribType)
			continue;

		char* pszTemp2 = (char*)g_MagicDesc.GetDesc(&m_aryRequireAttrib[i]);
		if (!pszTemp2 || !pszTemp2[0])
			continue;

		if (Player[CLIENT_PLAYER_INDEX].m_ItemList.EnoughAttrib(&m_aryRequireAttrib[i]))
			strcat(pszMsg, "<color=255,255,255>");
		else
			strcat(pszMsg, "<color=255,0,0>");

		strcat(pszMsg, pszTemp2);
		strcat(pszMsg, "\n");
	}

	for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
	{
		if (!m_aryMagicAttrib[i].nAttribType)
		{
			if (m_CommonAttrib.nItemNature == NATURE_VIOLET)
			{
				// [LOREN] Truoc day dem bang nLuck (may man cua mon do) - khong
				// lien quan gi toi so o. Trang bi do lo ren duc ra chep nLuck tu
				// mon goc. Dem dung cai danh dau o trong: nGeneratorLevel == -1.
				if (i < MAX_ITEM_MAGICLEVEL && m_GeneratorParam.nGeneratorLevel[i] == -1)
				{
					if (i == MAX_ITEM_NORMAL_MAGICATTRIB)
						strcat(pszMsg, "\n");
					strcat(pszMsg, "<color=255,255,0>Ch≠a kh∂m nπm<color=255,255,255>");
					strcat(pszMsg, "\n");
				}
			}
			continue;
		}
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_aryMagicAttrib[i]);
		KItemNormalAttrib ModifiedAttrib = *pAttrib;
		for (int j = 0; j < 2; j++) {
			ModifiedAttrib.nValue[j] = ApplyScalingMethod(pAttrib->nValue[j], pAttrib->nAttribType);
		}
		ModifiedAttrib.nValue[2] = pAttrib->nValue[2];
		char* pszTemp2 = (char*)g_MagicDesc.GetDesc(&ModifiedAttrib);
		if (!pszTemp2 || !pszTemp2[0])
			continue;
		if (i == MAX_ITEM_NORMAL_MAGICATTRIB && m_aryMagicAttrib[i].nAttribType)
			strcat(pszMsg, "\n");

		if (i < MAX_ITEM_NORMAL_MAGICATTRIB)
		{
			if ((i & 1) == 0)
			{
				switch (m_CommonAttrib.nItemNature)
				{
				case NATURE_GOLD:
				case NATURE_PLATINA:
					strcat(pszMsg, "<color=255,255,0>");
					break;
				case NATURE_VIOLET:
					strcat(pszMsg, "<color=170,30,255>");
					break;
				default:
					strcat(pszMsg, "<color=99,101,255>");
					break;
				}
			}
			else
			{
				if ((i >> 1) < nActiveAttrib)
				{
					switch (m_CommonAttrib.nItemNature)
					{
					case NATURE_GOLD:
					case NATURE_PLATINA:
						strcat(pszMsg, "<color=255,255,0>");
						break;
					case NATURE_VIOLET:
						strcat(pszMsg, "<color=170,30,255>");
						break;
					default:
						strcat(pszMsg, "<color=100,100,255>");
						break;
					}
				}
				else
				{
					switch (m_CommonAttrib.nItemNature)
					{
					case NATURE_GOLD:
					case NATURE_PLATINA:
						strcat(pszMsg, "<color=123,125,90>");
						break;
					case NATURE_VIOLET:
						strcat(pszMsg, "<color=170,135,184>");
						break;
					default:
						strcat(pszMsg, "<color=120,120,120>");
						break;
					}
					bActiveAllAttrib = false;
				}
			}
		}
		else
		{
			if (nGoldActiveAttrib)
			{
				switch (m_CommonAttrib.nItemNature)
				{
				case NATURE_GOLD:
					strcat(pszMsg, "<color=255,0,255>");
					break;
				case NATURE_PLATINA:
					strcat(pszMsg, "<color=255,94,0>");
					break;
				case NATURE_VIOLET:
					strcat(pszMsg, "<color=170,30,255>");
					break;
				default:
					strcat(pszMsg, "<color=100,100,255>");
					break;
				}
				nGoldActiveAttrib--;
			}
			else
			{
				switch (m_CommonAttrib.nItemNature)
				{
				case NATURE_GOLD:
					strcat(pszMsg, "<color=169,19,215>");
					break;
				case NATURE_PLATINA:
					strcat(pszMsg, "<color=189,70,2>");
					break;
				case NATURE_VIOLET:
					strcat(pszMsg, "<color=170,135,184>");
					break;
				default:
					strcat(pszMsg, "<color=120,120,120>");
					break;
				}
			}
		}
		strcat(pszMsg, pszTemp2);
		strcat(pszMsg, "\n");
		strcat(pszMsg, "<color=255,255,255>");
	}
	PF_AppendDesc(pszMsg);	// diem chuc phuc + tung lo kham + thuoc tinh da
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
			if (m_CommonAttrib.uPrice % 10000 == 0)
			{
				sprintf(sOrice, "<color=White>Gi∏ ni™m y’t:<color> %d vπn l≠Óng", m_CommonAttrib.uPrice / 10000);
			}
			else
			{
				sprintf(sOrice, "<color=White>Gi∏ ni™m y’t:<color> %d vπn %d l≠Óng", m_CommonAttrib.uPrice / 10000, m_CommonAttrib.uPrice % 10000);
			}
		}
		strcat(pszMsg, sOrice);
		strcat(pszMsg, "  \n  ");
		strcat(pszMsg, "  \n  ");
	}
	if (m_CommonAttrib.LimitTime.bYear)
	{
		char sTmp[128];
		const long thoigianhet = m_CommonAttrib.LimitTime.bYear;
		if (thoigianhet > 0)
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
				snprintf(sTmp, sizeof(sTmp), "<color=red>H’t hπn sˆ dÙng vÀt ph»m sœ b~ hÒy");
			}

			// Use strncat for safer concatenation
			strncat(pszMsg, "  \n  ", sizeof(pszMsg) - strlen(pszMsg) - 1);
			strncat(pszMsg, sTmp, sizeof(pszMsg) - strlen(pszMsg) - 1);
		}
	}
	if (m_CommonAttrib.nExpireTime > KSG_GetCurSec())
	{
		time_t nowtime = m_CommonAttrib.nExpireTime + 1451581200;
		struct tm* timeinfo = localtime(&nowtime);
		strcpy(pszTemp, "<color=255,90,0>ThÍi hπn s dÙng: %H:%M:%S %d-%m-%Y<color>");
		strftime(pszTemp2, sizeof(pszTemp2), pszTemp, timeinfo);
		strcat(pszMsg, "\n");
		strcat(pszMsg, pszTemp2);
		strcat(pszMsg, "\n");
	}

	if (m_CommonAttrib.nExpirePoint)
	{
		time_t nowtime = m_CommonAttrib.nExpirePoint + (KSG_GetCurSec() + 1451581200);
		struct tm* timeinfo = localtime(&nowtime);
		strcpy(pszTemp, "<color=255,90,0>ThÍi hπn s dÙng: %H:%M:%S %d-%m-%Y<color>");
		strftime(pszTemp2, sizeof(pszTemp2), pszTemp, timeinfo);
		strcat(pszMsg, "\n");
		strcat(pszMsg, pszTemp2);
		strcat(pszMsg, "\n");
	}
	PlayerItem m_pItems = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[Player[CLIENT_PLAYER_INDEX].m_ItemList.FindSame(m_dwID)];

	if (!bActiveAllAttrib &&
		m_aryMagicAttrib[0].nAttribType &&
		m_CommonAttrib.nItemGenre == item_equip &&
		m_CommonAttrib.nDetailType < equip_horse
		)
	{
		if (m_pItems.nPlace == pos_equip)
		{
			char szBuff[32], szBuffer[32];

			sprintf(pszKeyName, "NeedSr%d", m_CommonAttrib.nSeries);
			g_GameSetting.GetString("ActiveEquip", pszKeyName, "", szBuff, sizeof(szBuff));
			if (m_pItems.nX == itempart_ring1)
				sprintf(pszKeyName, "NeedEq%d1", m_CommonAttrib.nDetailType);
			else if (m_pItems.nX == itempart_ring2)
				sprintf(pszKeyName, "NeedEq%d2", m_CommonAttrib.nDetailType);
			else
				sprintf(pszKeyName, "NeedEq%d", m_CommonAttrib.nDetailType);
			g_GameSetting.GetString("ActiveEquip", pszKeyName, "", szBuffer, sizeof(szBuffer));

			sprintf(pszTemp, "<color=Yellow>C«n h÷ %s cÒa %s Æ” k›ch hoπt thuÈc t›nh ©m<color>", szBuff, szBuffer);
			strcat(pszMsg, "\n");
			strcat(pszMsg, pszTemp);
			strcat(pszMsg, "\n<color=255,255,255>");
		}
	}
	if (m_CommonAttrib.nItemNature == NATURE_GOLD)
	{
		BOOL bFlag = FALSE;
		KTabFile TabFile;

		if (m_CommonAttrib.nItemNature == NATURE_GOLD)
			bFlag = TabFile.Load(TABFILE_GOLDITEM_FULL_N);
		else
			bFlag = TabFile.Load(PLATINA_EQUIP_FILE);
		if (bFlag)
		{
			if (m_CommonAttrib.nDetailType < itempart_horse)
				strcat(pszMsg, "\n");

			int i, j, k;
			if (m_CommonAttrib.nSetIDNo != -1) {
				for (i = 0; i < 10; i++)
				{
					TabFile.GetInteger(m_CommonAttrib.nRow + 3 - m_CommonAttrib.nSetIDNo + i, 53, 0, &j); //get group -> j, Group column 53
					if (j == m_CommonAttrib.nGroup)
					{
						TabFile.GetString(m_CommonAttrib.nRow + 3 - m_CommonAttrib.nSetIDNo + i, 1, "", pszTemp, sizeof(pszTemp)); //Name column 1
						bFlag = FALSE;
						if (m_pItems.nPlace > pos_hand) //Check all item in equip that in the set or not
						{
							for (k = 0; k < MAX_PLAYER_ITEM; k++)
							{
								if ((Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nIdx].GetGroup() == m_CommonAttrib.nGroup) &&
									(Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nIdx].GetSetID() == m_CommonAttrib.nSetID) &&
									(Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nIdx].GetSetIDNo() == i + 1))
								{
									if (Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nPlace == pos_equip) { //priority in equip
										bFlag = TRUE;
										break;
									}
									else {
										bFlag = TRUE; //have item but not equip
									}
								}
							}
							if (bFlag)
							{
								if (Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nPlace == pos_equip)
									strcat(pszMsg, "<color=247,195,90>");  //active in equip
								else
									strcat(pszMsg, "<color=0,255,0>");		//in another room
							}
							else
								strcat(pszMsg, "<color=35,172,35>");		//Not exist
						}
						else
							strcat(pszMsg, "<color=0,201,0>");

						strcat(pszMsg, pszTemp);
						int len = strlen(pszMsg);
						strcat(pszMsg, "\n<color=255,255,255>");
					}
				}
			}
			else { //Item without setID
				for (i = -10; i < 10; i++)
				{
					TabFile.GetInteger(m_CommonAttrib.nRow + 3 + i, 53, 0, &j); //get group -> j, Group column 53
					if (j == m_CommonAttrib.nGroup)
					{
						TabFile.GetString(m_CommonAttrib.nRow + 3 + i, 1, "", pszTemp, sizeof(pszTemp)); //Name column 1
						if (strncmp(m_CommonAttrib.szItemName, pszTemp, 7) != 0) //temp fix
							continue;
						bFlag = FALSE;
						if (m_pItems.nPlace > pos_hand) //Check all item in equip that in the set or not
						{
							for (k = 0; k < MAX_PLAYER_ITEM; k++)
							{
								if (Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nIdx].GetGroup() == m_CommonAttrib.nGroup &&
									strcmp(Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nIdx].m_CommonAttrib.szItemName, pszTemp) == 0)
								{
									if (Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nPlace == pos_equip) { //priority in equip
										bFlag = TRUE;
										break;
									}
									else {
										bFlag = TRUE; //have item but not equip
									}
								}
							}
							if (bFlag)
							{
								if (Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nPlace == pos_equip)
									strcat(pszMsg, "<color=247,195,90>");  //active in equip
								else
									strcat(pszMsg, "<color=0,255,0>");		//in another room
							}
							else
								strcat(pszMsg, "<color=35,172,35>"); //Khong co
						}
						else
							strcat(pszMsg, "<color=0,201,0>"); //error

						strcat(pszMsg, pszTemp);
						int len = strlen(pszMsg);
						strcat(pszMsg, "\n<color=255,255,255>");
					}
				}
			}
		}
	}
	else if (m_CommonAttrib.nItemNature == NATURE_PLATINA)
	{
		BOOL bFlag = FALSE;
		KTabFile TabFile;

		if (m_CommonAttrib.nItemNature == NATURE_GOLD)
			bFlag = TabFile.Load(TABFILE_GOLDITEM_FULL_N);
		else
			bFlag = TabFile.Load(PLATINA_EQUIP_FILE);
		if (bFlag)
		{
			if (m_CommonAttrib.nDetailType < itempart_horse)
				strcat(pszMsg, "\n");

			int i, j, k;
			if (m_CommonAttrib.nSetIDNo >= 1) {
				for (i = -10; i < 10; i++)
				{
					TabFile.GetInteger(m_CommonAttrib.nRow + 3 - m_CommonAttrib.nSetNum + i, 59, 0, &j); //get group -> j, Group column 59
					if (j == m_CommonAttrib.nGroup)
					{
						TabFile.GetString(m_CommonAttrib.nRow + 3 + i, 1, "", pszTemp, sizeof(pszTemp)); //Name column 1
						bFlag = FALSE;
						if (m_pItems.nPlace > pos_hand) //Check all item in equip that in the set or not
						{
							for (k = 0; k < MAX_PLAYER_ITEM; k++)
							{
								if ((Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nIdx].GetGroup() == m_CommonAttrib.nGroup) &&
									(Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nIdx].GetSetID() == m_CommonAttrib.nSetID) &&
									(Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nIdx].GetSetIDNo() == i + 1))
								{
									bFlag = TRUE; //have item but not equip
									break;
								}
								else if(Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nIdx].GetGroup() == m_CommonAttrib.nGroup
									&& Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nIdx].GetObjIdx() != m_CommonAttrib.nObjIdx)
								{
									bFlag = TRUE; //have item but not equip
									break;
								}
							}
							if (bFlag)
							{
								if (Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nPlace == pos_equip)
									strcat(pszMsg, "<color=247,195,90>");  //active in equip
								else
									strcat(pszMsg, "<color=0,255,0>");		//in another room
							}
							else
								strcat(pszMsg, "<color=35,172,35>");		//Not exist
						}
						else
							strcat(pszMsg, "<color=0,201,0>");

						strcat(pszMsg, pszTemp);
						int len = strlen(pszMsg);
						strcat(pszMsg, "\n<color=255,255,255>");
					}
				}
			}
			else { //Item without setID
				for (i = -10; i < 10; i++)
				{
					TabFile.GetInteger(m_CommonAttrib.nRow + 3 + i, 59, 0, &j); //get group -> j, Group column 53
					if (j == m_CommonAttrib.nGroup)
					{
						TabFile.GetString(m_CommonAttrib.nRow + 3 + i, 1, "", pszTemp, sizeof(pszTemp)); //Name column 1
						if (strncmp(m_CommonAttrib.szItemName, pszTemp, 10) != 0) //temp fix
							continue;
						bFlag = FALSE;
						if (m_pItems.nPlace > pos_hand) //Check all item in equip that in the set or not
						{
							for (k = 0; k < MAX_PLAYER_ITEM; k++)
							{
								if (Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nIdx].GetGroup() == m_CommonAttrib.nGroup &&
									strcmp(Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nIdx].m_CommonAttrib.szItemName, pszTemp) == 0)
								{
									if (Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nPlace == pos_equip) { //priority in equip
										bFlag = TRUE;
										break;
									}
									else {
										bFlag = TRUE; //have item but not equip
									}
								}
							}
							if (bFlag)
							{
								if (Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[k].nPlace == pos_equip)
									strcat(pszMsg, "<color=247,195,90>");  //active in equip
								else
									strcat(pszMsg, "<color=0,255,0>");		//in another room
							}
							else
								strcat(pszMsg, "<color=35,172,35>"); //Khong co
						}
						else
							strcat(pszMsg, "<color=0,201,0>"); //error

						strcat(pszMsg, pszTemp);
						int len = strlen(pszMsg);
						strcat(pszMsg, "\n<color=255,255,255>");
					}
				}
			}
		}
		}
	KLuaScript* pScript = NULL;
	g_SetFilePath("\\");
	//Load Lua script
	KLuaScript Script;
	Script.Init();
	if (Script.Load(m_CommonAttrib.szScript))
	{
		pScript = &Script;
		int nSafeIndex = 1;
		nSafeIndex = pScript->SafeCallBegin();

		pScript->CallFunction("GetDesc", 1, "d", Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchID(GetID()));
		if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
		{
			strcat(pszMsg, "<color=255,255,255>\n");
			strcat(pszMsg, (char*)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
			strcat(pszMsg, "\n<color=255,255,255>");
		}
		// [VA5LOI 29/08] bo dong `nSafeIndex = SafeCallBegin();` o day: no ghi
		// de nSafeIndex bang dinh ngan xep HIEN TAI (da co ket qua GetDesc
		// tren do), khien SafeCallEnd khoi phuc ve chinh no = KHONG don gi.
		// Dinh dung phai la cai lay o SafeCallBegin() truoc CallFunction.
		pScript->SafeCallEnd(nSafeIndex);
	}

	if (m_CommonAttrib.nFortune)
	{
		sprintf(pszTemp2, "<color=255,255,0>Tr s tµi ph binh gi∏p:<color> <color=0,255,0>%d<color>", m_CommonAttrib.nFortune);

		strcat(pszMsg, "\n");
		strcat(pszMsg, pszTemp2);
		strcat(pszMsg, "\n<color=255,255,255>");
	}

	if ((/*Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PTrade.nTrade || */bShowPrice) &&
		m_CommonAttrib.nTradePrice)
	{
		strcat(pszMsg, "\n");
		strcat(pszMsg, "<color=255,255,255>Gi ni™m y’t: <color=255,255,0>");
		if (m_CommonAttrib.nTradePrice < MONEY_FLOOR)
			sprintf(pszTemp, "%d l≠Óng", m_CommonAttrib.nTradePrice);
		else if ((m_CommonAttrib.nTradePrice % MONEY_FLOOR) == 0)
			sprintf(pszTemp, "%d vπn l≠Óng", m_CommonAttrib.nTradePrice / MONEY_FLOOR);
		else
			sprintf(pszTemp, "%d vπn %d l≠Óng", m_CommonAttrib.nTradePrice / MONEY_FLOOR, m_CommonAttrib.nTradePrice % MONEY_FLOOR);
		strcat(pszMsg, pszTemp);
	}
	strcat(pszMsg, "");
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

// [LOREN] doi ung cua GetMaxDurability - ghi vao chinh o base attrib do.
BOOL KItem::SetMaxDurability(IN const int nDur)
{
	for (int i = 0; i < 7; i++)
	{
		if (m_aryBaseAttrib[i].nAttribType == magic_durability_v)
		{
			m_aryBaseAttrib[i].nValue[0] = nDur;
			if (m_nCurrentDur > nDur)
				m_nCurrentDur = nDur;
			return TRUE;
		}
	}
	return FALSE;
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
		return (m_CommonAttrib.nPrice * ItemSet.m_sRepairParam.nPriceScale / 100 * (nMaxDur - m_nCurrentDur) / nMaxDur * (ItemSet.m_sRepairParam.nMagicScale + nSumMagic) / ItemSet.m_sRepairParam.nMagicScale) * 2;

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

//---------------------------------------------------------------------------
// [LOREN] CHIN CO XEP CHONG - doc tu settings\item\magicscript_stack.txt
//
// Ban goc co DUNG MOT ham quyet dinh xep chong (0x08065A70, 5 noi goi). No so
// 6 truong VO DIEU KIEN, va 9 truong CO DIEU KIEN theo co: cung dang cap,
// cung ngu hanh, cung may man, cung MagicLevel1..6.
//
// *** MO CLIENT VLTK MOI RA DUNG SO LIEU ***
// Bang cua may chu Linux co 543 dong / 32 dong bat co, nhung ban VIET HOA
// CHAY THAT (client VLTK, update04.pak) chi co 247 dong / DUNG BON dong bat co,
// va ca bon deu la nguyen lieu lo ren:
//     Huyen Tinh Khoang Thach (JX1 6/1/146)  -> doi CUNG DANG CAP
//     Khong Tuoc Nguyen Thach (JX1 6/1/149)  -> doi CUNG NGU HANH
//     Phu Dung  Nguyen Thach  (JX1 6/1/151)  -> doi CUNG NGU HANH
//     Chung Nhu Nguyen Thach  (JX1 6/1/153)  -> doi CUNG NGU HANH
// 28 dong con lai cua ban Linux (banh kem, nen, thiep hoa hong, hop trang suc...)
// KHONG co trong ban Viet hoa - khong dua vao.
// Rieng ba mon NGUYEN KHOANG (148/150/152) KHONG doi cung ngu hanh; chi ba mon
// NGUYEN THACH moi doi. Truoc day toi gop ca sau, la chat hon ban goc.
//
// Chi so cot 1 cua bang la so dong cua magicscript.txt; ma particular = chi so
// tru 1 (bang magicscript cua JX1 co record index dung bang particular - xem
// KItemGenerator::Gen_MagicScript goi GetMagicScript(nParticularType)).
//---------------------------------------------------------------------------
#define MAGICSCRIPT_STACK_FILE	"\\settings\\item\\magicscript_stack.txt"

struct KCoXepChong
{
	int		nParticular;
	int		bCungCap;
	int		bCungNguHanh;
	int		bCungMayMan;
	int		bCungML[MAX_ITEM_MAGICLEVEL];
};

static KCoXepChong	s_aryCoXepChong[64];
static int			s_nCoXepChong = 0;
static BOOL			s_bDaNapCoXepChong = FALSE;

static void sNapCoXepChong()
{
	s_bDaNapCoXepChong = TRUE;
	s_nCoXepChong = 0;

	KTabFile Tab;
	if (!Tab.Load((LPSTR)MAGICSCRIPT_STACK_FILE))
		return;						// khong co tep = khong co co nao, chay nhu cu

	int nRow = Tab.GetHeight();
	for (int r = 2; r <= nRow; r++)	// dong 1 la tieu de
	{
		if (s_nCoXepChong >= (int)(sizeof(s_aryCoXepChong) / sizeof(s_aryCoXepChong[0])))
			break;
		int nIdx = 0;
		Tab.GetInteger(r, 1, 0, &nIdx);
		if (nIdx <= 0)
			continue;
		KCoXepChong C;
		memset(&C, 0, sizeof(C));
		C.nParticular = nIdx - 1;	// chi so dong -> ma particular
		Tab.GetInteger(r, 3, 0, &C.bCungCap);
		Tab.GetInteger(r, 4, 0, &C.bCungNguHanh);
		Tab.GetInteger(r, 5, 0, &C.bCungMayMan);
		for (int j = 0; j < MAX_ITEM_MAGICLEVEL && j < 6; j++)
			Tab.GetInteger(r, 6 + j, 0, &C.bCungML[j]);
		// dong khong co co nao thi khong can nho
		int nTong = C.bCungCap + C.bCungNguHanh + C.bCungMayMan;
		for (int j = 0; j < MAX_ITEM_MAGICLEVEL && j < 6; j++)
			nTong += C.bCungML[j];
		if (nTong <= 0)
			continue;
		s_aryCoXepChong[s_nCoXepChong++] = C;
	}
	// KTabFile khong co UnLoad; ham huy tu don.
}

// Chi vat pham bang magicscript (genre 6) moi co chin cot co nay; cac bang
// khac (questkey 11 cot, questkey_stack 2 cot) khong he co - do da o ca hai ben.
static const KCoXepChong* sTimCoXepChong(int nGenre, int nDetailType, int nPtc)
{
	if (!s_bDaNapCoXepChong)
		sNapCoXepChong();
	if (nGenre != item_magicscript || nDetailType != 1)
		return NULL;
	for (int i = 0; i < s_nCoXepChong; i++)
	{
		if (s_aryCoXepChong[i].nParticular == nPtc)
			return &s_aryCoXepChong[i];
	}
	return NULL;
}

// Phep so co dieu kien, dung CHUNG cho ca hai duong xep chong cua JX1
// (KItem::CanStack khi keo tha, KInventory::FindSameItemToSort khi nhap tui).
// Ban goc cung chi co mot ham (0x08065A70) cho ca hai.
BOOL g_HopCoXepChong(int nIdxA, int nIdxB)
{
	if (nIdxA <= 0 || nIdxA >= MAX_ITEM || nIdxB <= 0 || nIdxB >= MAX_ITEM)
		return TRUE;
	KItem* pA = &Item[nIdxA];
	KItem* pB = &Item[nIdxB];
	const KCoXepChong* pCo = sTimCoXepChong(pA->GetGenre(), pA->GetDetailType(),
											pA->GetParticular());
	if (!pCo)
		return TRUE;				// khong khai co nao = giu nguyen hanh vi cu
	if (pCo->bCungCap && pA->GetLevel() != pB->GetLevel())
		return FALSE;
	if (pCo->bCungNguHanh && pA->GetSeries() != pB->GetSeries())
		return FALSE;
	if (pCo->bCungMayMan
		&& pA->m_GeneratorParam.nLuck != pB->m_GeneratorParam.nLuck)
		return FALSE;
	for (int j = 0; j < MAX_ITEM_MAGICLEVEL && j < 6; j++)
	{
		if (pCo->bCungML[j]
			&& pA->m_GeneratorParam.nGeneratorLevel[j]
			   != pB->m_GeneratorParam.nGeneratorLevel[j])
			return FALSE;
	}
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
			// [LOREN] Chin phep so CO DIEU KIEN cua ban goc (0x08065A70).
			// Dung CHUNG mot ham voi KInventory::FindSameItemToSort - ban goc
			// cung chi co mot ham quyet dinh xep chong cho ca hai duong.
			if (!g_HopCoXepChong(nOldIdx, Dest))
				return FALSE;
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

BOOL KItem::CanShortKey()
{
	if (m_CommonAttrib.nWidth != 1 || m_CommonAttrib.nHeight != 1)
		return FALSE;

	return m_CommonAttrib.bShortKey;
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
	if (GetGoldId() || GetNature() >= NATURE_GOLD)
	{
		return gold_item;
	}
	// [LOREN 27/08] Trang bi do LO REN duc ra danh dau bang nItemNature =
	// NATURE_VIOLET chu khong phai nPoint (do tim kieu cu). Thieu nhanh nay
	// thi GetNatureItem tra ve normal_item va KWndObjectMatrix::PaintWindow
	// (WndObjContainer.cpp:560) khong ve vong sang tim.
	// KHONG sua IsPurple(): no ghi vao CSDL qua KPlayerDBFuns.cpp:1061.
	else if (GetNature() == NATURE_VIOLET)
	{
		return purple_item;
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
	if (GetGoldId() || GetNature() == NATURE_GOLD)
	{
		return gold_item;
	}
	if (GetNature() == NATURE_PLATINA)
	{
		return platinum_item;
	}
	// [LOREN 27/08] Trang bi do LO REN duc ra danh dau bang nItemNature =
	// NATURE_VIOLET chu khong phai nPoint (do tim kieu cu). Thieu nhanh nay
	// thi GetNatureItem tra ve normal_item va KWndObjectMatrix::PaintWindow
	// (WndObjContainer.cpp:560) khong ve vong sang tim.
	// KHONG sua IsPurple(): no ghi vao CSDL qua KPlayerDBFuns.cpp:1061.
	else if (GetNature() == NATURE_VIOLET)
	{
		return purple_item;
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
