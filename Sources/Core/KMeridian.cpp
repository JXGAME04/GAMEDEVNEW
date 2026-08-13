#include	"KCore.h"
#include "KMeridian.h"

KMeridian::KMeridian()
{
	memset(m_abActiveMeridianLevel, 0, sizeof(m_abActiveMeridianLevel));
}

void KMeridian::Init(int nPlayerIdx)
{
	m_nPlayerIndex = nPlayerIdx;
	Reset();
}

void KMeridian::Reset()
{
	memset(m_abActiveMeridianLevel, 0, sizeof(m_abActiveMeridianLevel));
}

KMeridian::~KMeridian()
{
	Reset();
}

BYTE* KMeridian::getMeridian()
{
	return m_abActiveMeridianLevel;
}

bool KMeridian::setMeridian(BYTE* meridian)
{
	if (memcmp(m_abActiveMeridianLevel, meridian, sizeof(m_abActiveMeridianLevel)) == 0)
		return false;
	memcpy(m_abActiveMeridianLevel, meridian, sizeof(m_abActiveMeridianLevel));
	return true;
}
BOOL KMeridian::canEnhance(int type, int level)
{
	if (m_abActiveMeridianLevel[type] + 1 != level) //chi duoc nang tung cap mot
		return false;
	return true;
}
//return dif level
int KMeridian::setMeridian(int nMeridianType, int nMeridianLevel)
{
	if (nMeridianType >= MAX_MERIDIAN || nMeridianLevel > MAX_MERIDIAN_LEVEL
		|| nMeridianType < 0 || nMeridianLevel < 0)
		return 0;
	int ret = nMeridianLevel - m_abActiveMeridianLevel[nMeridianType];
	m_abActiveMeridianLevel[nMeridianType] = (BYTE)nMeridianLevel;
	
	return ret;
}

KMeridianManager MeridianManager;

KMeridianManager::KMeridianManager()
{
}

void KMeridianManager::Init()
{
	MagicTab.Load(GOLD_EQUIP_MAGIC_FILE_N);
}

KMeridianManager::~KMeridianManager()
{
	MagicTab.Clear();
}

void KMeridianManager::ApplyMaridianToNPC(IN KNpc* pNPC, IN int nMaridianType, IN int nMaridianLevel, IN int levelDiff /*=0*/)
{
	_ASSERT(this != NULL);
	if (nMaridianType <= 0 || nMaridianType > MAX_MERIDIAN 
		|| nMaridianLevel <= 0 
		|| nMaridianLevel > MAX_MERIDIAN_LEVEL)
		return;
	if (levelDiff < 0)
		return;
	KMagicAttrib pDst;
	//get maridian from g_MeridiantSetting
	int magicType;
	int nType, nValue;
	int i = nMaridianLevel;
	for (i = nMaridianLevel - levelDiff + 1; i <= nMaridianLevel; i++) {
		g_MeridiantSetting.GetInteger((nMaridianType-1) * MAX_MERIDIAN_LEVEL + i + 1, 7, 1, &magicType); //Col7="Magic attribute ID_1"


		MagicTab.GetInteger(magicType + 1, 5, 0, &nType);
		pDst.nAttribType = nType;
		MagicTab.GetInteger(magicType + 1, 6, 0, &nValue);
		pDst.nValue[0] = pDst.nValue[1] = pDst.nValue[2] = nValue;
		pDst.nValue[2] = 99887786; //cryptic value to identify maridian

		pNPC->ModifyAttrib(pNPC->m_Index, &pDst);
	}
}

//apply to NPC from init
void KMeridianManager::ApplyMaridianToNPC(IN KNpc* pNPC, IN BYTE* baMeridian)
{
	int i = 0;
	for (i = 0; i < MAX_MERIDIAN; i++) {
		ApplyMaridianToNPC(pNPC, i+1, (BYTE)baMeridian[i], (BYTE)baMeridian[i]);
	}
}
//remove maridian from current level to nMaridianLevel = levelDiff
//levelDiff <= 0
void KMeridianManager::RemoveMaridianFromNPC(IN KNpc* pNPC, IN int nMaridianType, IN int nMaridianLevel, IN int levelDiff /*=0*/)
{
	_ASSERT(this != NULL);
	if (nMaridianType <= 0 || nMaridianType > MAX_MERIDIAN
		|| nMaridianLevel < 0
		|| nMaridianLevel > MAX_MERIDIAN_LEVEL)
		return;

	KMagicAttrib pDst;
	//get maridian from g_MeridiantSetting
	int magicType;
	int nType, nValue;
	int i = nMaridianLevel;
	for (i = nMaridianLevel - levelDiff; i > nMaridianLevel; i--) {
		g_MeridiantSetting.GetInteger((nMaridianType - 1) * MAX_MERIDIAN_LEVEL + i + 1, 7, 1, &magicType); //Col7="Magic attribute ID_1"


		MagicTab.GetInteger(magicType + 1, 5, 0, &nType);
		pDst.nAttribType = nType;
		MagicTab.GetInteger(magicType + 1, 6, 0, &nValue);
		pDst.nValue[0] = pDst.nValue[1] = pDst.nValue[2] = -nValue;

		pNPC->ModifyAttrib(pNPC->m_Index, &pDst);
	}
}
