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
	// [KM 27/08b] truoc day KHONG kiem bien: goi voi type ngoai 0..MAX_MERIDIAN-1
	// la doc ngoai mang. Chan lai.
	if (type < 0 || type >= MAX_MERIDIAN || level < 0 || level > MAX_MERIDIAN_LEVEL)
		return false;
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


// [KM 27/08b] THUONG TRON BO khi mot kinh mach dat cap toi da (32).
// Ban chuan cap bang ky nang 2106..2113; so lay nguyen tu bang SKILLS cua
// \script\skill\special\qiyingdantian.lua (client VLTK). Chi mach 1..8 co.
static const int KM_L32_LOAI[9] = {
	0,
	magic_five_elements_enhance_v,	// 1 Doc mach   - Cuong Hoa Ngu Hanh
	magic_five_elements_resist_v,	// 2 Nham mach  - Nhuoc Hoa Ngu Hanh
	magic_lifemax_yan_v,			// 3 Xung mach  - Sinh Luc Toi Da (duong)
	magic_physicsres_yan_p,			// 4 Doi mach   - Phong thu vat ly (duong)
	magic_coldres_yan_p,			// 5 Am duy     - Bang Phong (duong)
	magic_fireres_yan_p,			// 6 Duong duy  - Hoa Phong (duong)
	magic_lightingres_yan_p,		// 7 Am khieu   - Loi Phong (duong)
	magic_poisonres_yan_p,			// 8 Duong khieu- Doc Phong (duong)
};
static const int KM_L32_TRI[9] = { 0, 200, 200, 15000, 5, 5, 5, 5, 5 };

// bAp = 1 cong vao, 0 tru ra
static void KM_ThuongCap32(KNpc* pNPC, int nMach, int bAp)
{
	if (pNPC == NULL || nMach < 1 || nMach > 8)
		return;
	KMagicAttrib Dst;
	Dst.nAttribType = KM_L32_LOAI[nMach];
	int nTri = bAp ? KM_L32_TRI[nMach] : -KM_L32_TRI[nMach];
	Dst.nValue[0] = Dst.nValue[1] = Dst.nValue[2] = nTri;
	pNPC->ModifyAttrib(pNPC->m_Index, &Dst);
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
	// [KM 27/08b] vua cham cap toi da -> cong thuong tron bo cua mach nay
	if (nMaridianLevel >= MAX_MERIDIAN_LEVEL && levelDiff >= 1)
		KM_ThuongCap32(pNPC, nMaridianType, 1);
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
	// [KM 27/08b] tut khoi cap toi da -> thu hoi thuong tron bo
	if (nMaridianLevel - levelDiff >= MAX_MERIDIAN_LEVEL && nMaridianLevel < MAX_MERIDIAN_LEVEL)
		KM_ThuongCap32(pNPC, nMaridianType, 0);
}
