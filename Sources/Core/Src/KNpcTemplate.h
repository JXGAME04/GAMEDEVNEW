#ifndef	_KNPCTEMPLATE_H
#define	_KNPCTEMPLATE_H

#ifdef _SERVER
class KItemDropRateNode
{
public:
	KItemDropRate * m_pItemDropRate;
	char m_szFileName[128];
	KItemDropRateNode()
	{
		m_pItemDropRate = NULL;
	}
	~KItemDropRateNode()
	{

	}
};
#endif

#ifdef _SERVER
typedef	BinSTree<KItemDropRateNode> KItemDropRateTree;
extern KItemDropRateTree g_ItemDropRateBinTree;
#endif

#include "KCore.h"
#include "KNpc.h"

class KNpcTemplate
{
public:
private:
public:
	char	Name[32];
	//char	DescName[32];//edit by phong kieu khai bao ngu hanh quai
	DWORD	m_Kind;
	int		m_Camp;
	int		m_Series;
	int		m_HeadImage;
	int		m_bClientOnly;
	int		m_CorpseSettingIdx;
	int		m_DeathFrame;
	int		m_WalkFrame;
	int		m_StandFrame;
	int		m_StandFrame1;
	int		m_RunFrame;
	int		m_HurtFrame;
	int		m_WalkSpeed;
	int		m_AttackFrame;
	int		m_CastFrame;
	int		m_RunSpeed;
	int		m_LifeMax;

#ifdef _SERVER	
	DWORD	m_dwLevelSettingScript;
	int		m_Treasure;
	int		m_AiMode;
	int		m_AiParam[MAX_AI_PARAM - 1];
	int		m_FireResistMax;
	int		m_ColdResistMax;
	int		m_LightResistMax;
	int		m_PoisonResistMax;
	int		m_PhysicsResistMax;
	int		m_ActiveRadius;		//B¸n kÝnh ho¹t ®éng
	int		m_VisionRadius;
	BYTE	m_AIMAXTime;
	int		m_HitRecover;
	int		m_ReviveFrame;		//Thêi gian håi sinh
	int		m_Experience;
	int		m_LifeReplenish;
	int		m_AttackRating;
	int		m_Defend;
	KMagicAttrib		m_PhysicsDamage;
	int		m_RedLum;
	int		m_GreenLum;
	int		m_BlueLum;
	int		m_FireResist;
	int		m_ColdResist;
	int		m_LightResist;
	int		m_PoisonResist;
	int		m_PhysicsResist;
	KSkillList m_SkillList;
	KItemDropRate *m_pItemDropRate;
	char	m_szDropRateFile[128];
	
#endif
	
#ifndef _SERVER	
	int		m_ArmorType;
	int		m_HelmType;
	int		m_WeaponType;
	int		m_HorseType;
	int		m_bRideHorse;
	char	ActionScript[32];
	char	m_szLevelSettingScript[100];
#endif
	
	int		m_NpcSettingIdx;
	int		m_nSkillID[4];
	int		m_nSkillLevel[4];
	BOOL	m_bHaveLoadedFromTemplate;
	int		m_nStature;	
	int		m_nLevel;
	
public:
	void	InitNpcBaseData(int nNpcTemplateId);
	void	InitNpcLevelData(KTabFile * pKindFile, int nNpcTemplateId, KLuaScript * pLevelScript, int nLevel, int nSeries);
	static int		SkillString2Id(char * szSkillString);
	static int		GetNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName,int nSerial, int nLevel, char * szParam);
	static int		GetNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName,int nSerial, int nLevel, double nParam1, double nParam2, double nParam3);
	static void	   GenNpcDropRate(KNpc * npc, char* szDropFile);
	static int	   GenNpcSeries(int n1, int K, int M, int T, int H, int TH);//Gen NPC HÖ theo tØ lÖ
	KNpcTemplate(){
		memset(Name, 0, sizeof(Name));

		// Initialize public member variables
		m_Kind = 0;
		m_Camp = 0;
		m_Series = 0;
		m_HeadImage = 0;
		m_bClientOnly = 0;
		m_CorpseSettingIdx = 0;
		m_DeathFrame = 0;
		m_WalkFrame = 0;
		m_StandFrame = 0;
		m_StandFrame1 = 0;
		m_RunFrame = 0;
		m_HurtFrame = 0;
		m_WalkSpeed = 0;
		m_AttackFrame = 0;
		m_CastFrame = 0;
		m_RunSpeed = 0;
		m_LifeMax = 0;

#ifdef _SERVER
		m_dwLevelSettingScript = 0;
		m_Treasure = 0;
		m_AiMode = 0;
		memset(m_AiParam, 0, sizeof(m_AiParam));
		m_FireResistMax = 0;
		m_ColdResistMax = 0;
		m_LightResistMax = 0;
		m_PoisonResistMax = 0;
		m_PhysicsResistMax = 0;
		m_ActiveRadius = 0;
		m_VisionRadius = 0;
		m_AIMAXTime = 0;
		m_HitRecover = 0;
		m_ReviveFrame = 0;
		m_Experience = 0;
		m_LifeReplenish = 0;
		m_AttackRating = 0;
		m_Defend = 0;
		m_RedLum = 0;
		m_GreenLum = 0;
		m_BlueLum = 0;
		m_FireResist = 0;
		m_ColdResist = 0;
		m_LightResist = 0;
		m_PoisonResist = 0;
		m_PhysicsResist = 0;
		m_pItemDropRate = nullptr;
		memset(m_szDropRateFile, 0, sizeof(m_szDropRateFile));
#endif

#ifndef _SERVER
		m_ArmorType = 0;
		m_HelmType = 0;
		m_WeaponType = 0;
		m_HorseType = 0;
		m_bRideHorse = 0;
		memset(ActionScript, 0, sizeof(ActionScript));
		memset(m_szLevelSettingScript, 0, sizeof(m_szLevelSettingScript));
#endif

		m_NpcSettingIdx = 0;
		memset(m_nSkillID, 0, sizeof(m_nSkillID));
		memset(m_nSkillLevel, 0, sizeof(m_nSkillLevel));
		m_bHaveLoadedFromTemplate = FALSE;
		m_nStature = 0;
		m_nLevel = 0;
	};
#ifdef _SERVER
	static KItemDropRate* UpdateDropRate(const char* pszDropRateFile);

#endif
};
extern KNpcTemplate	* g_pNpcTemplate[MAX_NPCSTYLE][MAX_NPC_LEVEL][MAX_NPC_SERIES];
#endif

