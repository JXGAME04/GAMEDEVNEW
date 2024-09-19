#include "KCore.h"
#include "KNpcTemplate.h"
#define MAX_VALUE_LEN 300
#define MIN_DAMAGE_NPC_UPTO 1
#define MAX_DAMAGE_NPC_UPTO 1

#ifdef _SERVER
KItemDropRateTree g_ItemDropRateBinTree;
static KItemDropRate * g_GenItemDropRate(char * szDropIniFile)
{
	if ((!szDropIniFile)|| (!szDropIniFile[0]))
		return NULL;
	
	KIniFile IniFile;
	if (IniFile.Load(szDropIniFile))
	{
		KItemDropRate * pnewDrop = new KItemDropRate;
		IniFile.GetInteger("Main", "Count", 0, &pnewDrop->nCount);
		IniFile.GetInteger("Main", "RandRange", 0, &pnewDrop->nMaxRandRate);
		IniFile.GetInteger("Main", "MagicRate", 0, &pnewDrop->nMagicRate);
		IniFile.GetInteger("Main", "MoneyRate", 20, &pnewDrop->nMoneyRate);
		IniFile.GetInteger("Main", "MoneyScale", 50, &pnewDrop->nMoneyScale);
		IniFile.GetInteger("Main", "MinItemLevelScale", 20, &pnewDrop->nMinItemLevelScale);
		IniFile.GetInteger("Main", "MaxItemLevelScale", 10, &pnewDrop->nMaxItemLevelScale);
		IniFile.GetInteger("Main", "MaxItemLevel", 10, &pnewDrop->nMaxItemLevel);
		IniFile.GetInteger("Main", "MinItemLevel", 1, &pnewDrop->nMinItemLevel);

		if(pnewDrop->nCount <= 0)
		{
			delete pnewDrop;
			return NULL;
		}

		pnewDrop->pItemParam = new KItemDropRate::KItemParam[pnewDrop->nCount];
		char szSection[10];
		
		KItemDropRate::KItemParam * pItemParam = pnewDrop->pItemParam;
		for(int i = 0; i < pnewDrop->nCount; i ++, pItemParam ++)
		{
			sprintf(szSection, "%d", i + 1);
			IniFile.GetInteger(szSection, "Genre", 0, &(pItemParam->nGenre));
			IniFile.GetInteger(szSection, "Detail", 0, &(pItemParam->nDetailType));
			IniFile.GetInteger(szSection, "Particular", 0, &(pItemParam->nParticulType));
			IniFile.GetInteger(szSection, "RandRate", 0, &(pItemParam->nRate));
		}
		return pnewDrop;
	}
	else
	{
		printf("Khong tim thay file Drop: %s\n", szDropIniFile);
		return NULL;
	}

	return NULL;
}

int	operator<(KItemDropRateNode Left, KItemDropRateNode Right)
{
	return strcmp(Left.m_szFileName, Right.m_szFileName);
};

int operator==(KItemDropRateNode Left, KItemDropRateNode Right)
{
	return (strcmp(Left.m_szFileName, Right.m_szFileName) == 0);
};
#endif

void	KNpcTemplate::InitNpcBaseData(int nNpcTemplateId)
{
	if (nNpcTemplateId < 0 ) return;
	int nNpcTempRow = nNpcTemplateId + 2;
	m_NpcSettingIdx = nNpcTemplateId;

	g_NpcSetting.GetString(nNpcTempRow, "Name", "", Name, sizeof(Name));
	g_NpcSetting.GetInteger(nNpcTempRow, "Kind", 0, (int *)&m_Kind);
	g_NpcSetting.GetInteger(nNpcTempRow, "Camp", 0, &m_Camp);
	g_NpcSetting.GetInteger(nNpcTempRow, "Series", 0, &m_Series);
	g_NpcSetting.GetInteger(nNpcTempRow, "HeadImage",	0, &m_HeadImage);
	g_NpcSetting.GetInteger(nNpcTempRow, "ClientOnly",	0, &m_bClientOnly);
	g_NpcSetting.GetInteger(nNpcTempRow, "CorpseIdx",	0, &m_CorpseSettingIdx);
	g_NpcSetting.GetInteger(nNpcTempRow, "DeathFrame",	12, &m_DeathFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "WalkFrame",	15, &m_WalkFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "RunFrame",	15, &m_RunFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "HurtFrame",	10, &m_HurtFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "WalkSpeed",	5, &m_WalkSpeed);
	g_NpcSetting.GetInteger(nNpcTempRow, "AttackSpeed",	20, &m_AttackFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "CastSpeed",	20, &m_CastFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "RunSpeed",	10, &m_RunSpeed);
	g_NpcSetting.GetInteger(nNpcTempRow, "StandFrame",	15, &m_StandFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "StandFrame1", 15, &m_StandFrame1);
	g_NpcSetting.GetInteger(nNpcTempRow, "Stature",		0,  &m_nStature);

#ifdef _SERVER	
	g_NpcSetting.GetInteger(nNpcTempRow, "AIMode",	0, &m_AiMode);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam1",	0, &m_AiParam[0]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam2",	0, &m_AiParam[1]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam3",	0, &m_AiParam[2]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam4",	0, &m_AiParam[3]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam5",	0, &m_AiParam[4]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam6",	0, &m_AiParam[5]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam7",	0, &m_AiParam[6]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam8",	0, &m_AiParam[7]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam9",	0, &m_AiParam[8]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam10",	5, &m_AiParam[9]);
	g_NpcSetting.GetInteger(nNpcTempRow, "FireResistMax",	0, &m_FireResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "ColdResistMax",	0, &m_ColdResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "LightResistMax",	0, &m_LightResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "PoisonResistMax",	0, &m_PoisonResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "PhysicsResistMax",	0, &m_PhysicsResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "ActiveRadius", 30, &m_ActiveRadius);		//B¸n kÝnh ho¹t ®éng
	g_NpcSetting.GetInteger(nNpcTempRow, "VisionRadius", 40, &m_VisionRadius);
	g_NpcSetting.GetInteger(nNpcTempRow, "Treasure", 1, &m_Treasure);

	char szDropFile[128];
	g_NpcSetting.GetString(nNpcTempRow, "DropRateFile", "", szDropFile, sizeof(szDropFile));
	strlwr(szDropFile);
	strcpy(m_szDropRateFile, szDropFile);
	KItemDropRateNode DropNode;
	strcpy(DropNode.m_szFileName, szDropFile);
	if (g_ItemDropRateBinTree.Find(DropNode))
	{
		m_pItemDropRate = DropNode.m_pItemDropRate;
	}
	else
	{
		KItemDropRateNode newDropNode;
		strcpy(newDropNode.m_szFileName,szDropFile);
		newDropNode.m_pItemDropRate = g_GenItemDropRate(szDropFile);
		g_ItemDropRateBinTree.Insert(newDropNode);
		m_pItemDropRate = newDropNode.m_pItemDropRate;
	}
	
	int nAIMaxTime = 0;
	g_NpcSetting.GetInteger(nNpcTempRow, "AIMaxTime", 25, (int*)&nAIMaxTime);
	m_AIMAXTime = (BYTE)nAIMaxTime;
	g_NpcSetting.GetInteger(nNpcTempRow, "HitRecover", 0, &m_HitRecover);
	g_NpcSetting.GetInteger(nNpcTempRow, "ReviveFrame", 2400, &m_ReviveFrame);	//Thêi gian håi sinh
	m_ReviveFrame = m_ReviveFrame * 6; //T¨ng thêi gian phôc sinh chuÈn VNG qu¸i 9x 2 phót
	char szLevelScript[MAX_PATH];
	g_NpcSetting.GetString(nNpcTempRow, "LevelScript", "", szLevelScript, MAX_PATH);
	if (!szLevelScript[0])
		m_dwLevelSettingScript = 0;
	else
	{
#ifdef WIN32
		_strlwr(szLevelScript);
#else
		for (int nl = 0; szLevelScript[nl]; nl++)
			if (szLevelScript[nl] >= 'A' && szLevelScript[nl] <= 'Z')
				szLevelScript[nl] += 'a' - 'A';
#endif
		m_dwLevelSettingScript = g_FileName2Id(szLevelScript);
	}
#else
	g_NpcSetting.GetInteger(nNpcTempRow, "ArmorType", 0, &m_ArmorType);
	g_NpcSetting.GetInteger(nNpcTempRow, "HelmType", 0, &m_HelmType);
	g_NpcSetting.GetInteger(nNpcTempRow, "WeaponType", 0, &m_WeaponType);
	g_NpcSetting.GetInteger(nNpcTempRow, "HorseType", -1, &m_HorseType);
	g_NpcSetting.GetInteger(nNpcTempRow, "RideHorse",0, &m_bRideHorse);
	g_NpcSetting.GetString(nNpcTempRow, "ActionScript", "", ActionScript, sizeof(ActionScript));
	g_NpcSetting.GetString(nNpcTempRow, "LevelScript", "", m_szLevelSettingScript, 100);
#endif

#ifdef _SERVER				
	int nParam;
	int nParam2;
	g_NpcSetting.GetInteger(nNpcTempRow, "Skill1",	0, &nParam);
	g_NpcSetting.GetInteger(nNpcTempRow, "Level1", 0, &nParam2);
	if (nParam && nParam2)
		m_SkillList.SetNpcSkill(1, nParam, nParam2);

	g_NpcSetting.GetInteger(nNpcTempRow, "Skill2",	0, &nParam);
	g_NpcSetting.GetInteger(nNpcTempRow, "Level2", 0, &nParam2);
	if (nParam && nParam2)
		m_SkillList.SetNpcSkill(2, nParam, nParam2);

	g_NpcSetting.GetInteger(nNpcTempRow, "Skill3",	0, &nParam);
	g_NpcSetting.GetInteger(nNpcTempRow, "Level3", 0, &nParam2);
	if (nParam && nParam2)
		m_SkillList.SetNpcSkill(3, nParam, nParam2);

	g_NpcSetting.GetInteger(nNpcTempRow, "Skill4",	0, &nParam);
	g_NpcSetting.GetInteger(nNpcTempRow, "Level4", 0, &nParam2);
	if (nParam && nParam2)
		m_SkillList.SetNpcSkill(4, nParam, nParam2);

	g_NpcSetting.GetInteger(nNpcTempRow, "ExpParam", 1, &nParam);
	m_Experience = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "LifeParam", 100, &nParam);
	m_LifeMax= nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "LifeReplenish", 0, &nParam);
	m_LifeReplenish = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "ARParam", 1, &nParam);	
	m_AttackRating = nParam;
	if (m_AttackRating == 0) 
		m_AttackRating = 100;

	g_NpcSetting.GetInteger(nNpcTempRow, "DefenseParam", 1, &nParam);
	m_Defend = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "MinDamageParam", 1, &nParam);
	m_PhysicsDamage.nValue[0] = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "MaxDamageParam", 1, &nParam);
	m_PhysicsDamage.nValue[2] = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "RedLum", 0, &nParam);
	m_RedLum = nParam;
	
	g_NpcSetting.GetInteger(nNpcTempRow, "GreenLum", 0, &nParam);
	m_GreenLum = nParam;
	
	g_NpcSetting.GetInteger(nNpcTempRow, "BlueLum", 0, &nParam);
	m_BlueLum = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "FireResist", 0, &nParam);
	m_FireResist = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "ColdResist", 0, &nParam);
	m_ColdResist = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "LightResist", 0, &nParam);
	m_LightResist = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "PoisonResist", 0, &nParam);
	m_PoisonResist = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "PhysicsResist", 0, &nParam);
	m_PhysicsResist = nParam;
#endif
}

#ifdef _SERVER
int KNpcTemplate::GenNpcSeries(int n1, int K, int M, int T, int H, int TH)//Gen NPC HÖ theo tØ lÖ
{
	if(n1 == 1)
	{
		int K1 = g_Random(K);
		int M1 = g_Random(M);
		int T1 = g_Random(T);
		int H1 = g_Random(H);
		int TH1 = g_Random(TH);
		int m1 = max(K1,M1);
		int m2 = max(m1,T1);
		int m3 = max(m2,H1);
		int m4 = max(m3,TH1);
		if(K1 == m4)
		{
			return 0;
		}
		else if(M1 == m4)
		{
			return 1;
		}
		else if(T1 == m4)
		{
			return 2;
		}
		else if(H1 == m4)
		{
			return 3;
		}
		else if(TH1 == m4)
		{
			return 4;
		}
	}
	return 0;
}

void KNpcTemplate::GenNpcDropRate(KNpc * npc, char* szDropFile)
{
	strlwr(szDropFile);
	KItemDropRateNode DropNode;
	strcpy(DropNode.m_szFileName, szDropFile);
	if (g_ItemDropRateBinTree.Find(DropNode))
	{
		npc->m_pDropRate = DropNode.m_pItemDropRate;
	}
	else
	{
		KItemDropRateNode newDropNode;
		strcpy(newDropNode.m_szFileName,szDropFile);
		newDropNode.m_pItemDropRate = g_GenItemDropRate(szDropFile);
		g_ItemDropRateBinTree.Insert(newDropNode);
		npc->m_pDropRate = newDropNode.m_pItemDropRate;
	}
}
#endif

void KNpcTemplate::InitNpcLevelData(KTabFile * pKindFile, int nNpcTemplateId, KLuaScript * pLevelScript, int nLevel, int nSeries)
{
	if (nNpcTemplateId < 0 || nLevel <= 0 || !pLevelScript || !pLevelScript->m_szScriptName[0])
	{
		printf("[Error] KNpcTemplate InitNpcLevelData\n");
		return;
	}
	int nNpcTempRow = nNpcTemplateId + 2;
	int	 nTopIndex = 0;
	m_nLevel = nLevel;

	pLevelScript->SafeCallBegin(&nTopIndex);
	{
		m_NpcSettingIdx = nNpcTemplateId;
#ifdef _SERVER				

		char szValue1[MAX_VALUE_LEN];
		char szValue2[MAX_VALUE_LEN];
		g_NpcSetting.GetString(nNpcTempRow, "Skill1",	"", szValue1, MAX_VALUE_LEN);
		g_NpcSetting.GetString(nNpcTempRow, "Level1", "", szValue2, MAX_VALUE_LEN);
		if (szValue1[0] && szValue2[0])
		{
			int SkillID = GetNpcLevelDataFromScript(pLevelScript, "Skill1", nSeries, nLevel, szValue2);
			int SkillLevel = GetNpcLevelDataFromScript(pLevelScript, "Level1", nSeries, nLevel, szValue2);
			if(SkillID == 0) SkillID = SkillString2Id(szValue1);
			m_SkillList.SetNpcSkill(1, SkillID, SkillLevel);
		}
		
		g_NpcSetting.GetString(nNpcTempRow, "Skill2",	"", szValue1, MAX_VALUE_LEN);
		g_NpcSetting.GetString(nNpcTempRow, "Level2", "", szValue2, MAX_VALUE_LEN);
		if (szValue1[0] && szValue2[0])
		{
			int SkillID = GetNpcLevelDataFromScript(pLevelScript, "Skill2", nSeries, nLevel, szValue2);
			int SkillLevel = GetNpcLevelDataFromScript(pLevelScript, "Level2", nSeries, nLevel, szValue2);
			if(SkillID == 0) SkillID = SkillString2Id(szValue1);
			m_SkillList.SetNpcSkill(2, SkillID, SkillLevel); 
		}			

		g_NpcSetting.GetString(nNpcTempRow, "Skill3",	"", szValue1, MAX_VALUE_LEN);
		g_NpcSetting.GetString(nNpcTempRow, "Level3", "", szValue2, MAX_VALUE_LEN);
		if (szValue1[0] && szValue2[0])
		{
			int SkillID = GetNpcLevelDataFromScript(pLevelScript, "Skill3", nSeries, nLevel, szValue2);
			int SkillLevel = GetNpcLevelDataFromScript(pLevelScript, "Level3", nSeries, nLevel, szValue2);
			if(SkillID == 0) SkillID = SkillString2Id(szValue1);
			m_SkillList.SetNpcSkill(3, SkillID, SkillLevel); 
		} 

		g_NpcSetting.GetString(nNpcTempRow, "Skill4",	"", szValue1, MAX_VALUE_LEN);
		g_NpcSetting.GetString(nNpcTempRow, "Level4", "", szValue2, MAX_VALUE_LEN);
		if (szValue1[0] && szValue2[0])
		{
			int SkillID = GetNpcLevelDataFromScript(pLevelScript, "Skill4", nSeries, nLevel, szValue2);
			int SkillLevel = GetNpcLevelDataFromScript(pLevelScript, "Level4", nSeries, nLevel, szValue2);
			if(SkillID == 0) SkillID = SkillString2Id(szValue1);
			m_SkillList.SetNpcSkill(4, SkillID, SkillLevel); 
		}
		
		g_NpcSetting.GetString(nNpcTempRow, "AuraSkillId",	"", szValue1, MAX_VALUE_LEN);
		g_NpcSetting.GetString(nNpcTempRow, "AuraSkillLevel", "", szValue2, MAX_VALUE_LEN);
		if (szValue1[0] && szValue2[0])
		{
			int SkillID = SkillString2Id(szValue1);
			int SkillLevel = GetNpcLevelDataFromScript(pLevelScript, "AuraSkillLevel", nSeries, nLevel, szValue2);
			//m_SkillList.SetNpcSkill(5, SkillID, SkillLevel); //t¹m thêi bá skill
		}

		float nParam1 = 0;
		float nParam2 = 0;
		float nParam3 = 0;
		float nParam = 1;
		
		g_NpcSetting.GetFloat(nNpcTempRow, "ExpParam", 1, &nParam);
		g_NpcSetting.GetFloat(nNpcTempRow, "ExpParam1", 0, &nParam1);
		g_NpcSetting.GetFloat(nNpcTempRow, "ExpParam2", 0, &nParam2);
		g_NpcSetting.GetFloat(nNpcTempRow, "ExpParam3", 0, &nParam3);
		m_Experience = nParam * GetNpcLevelDataFromScript(pLevelScript, "Exp", nSeries, nLevel, nParam1, nParam2, nParam3) / 100;
		if(m_Experience == 0) m_Experience = 100;
		
		g_NpcSetting.GetFloat(nNpcTempRow, "LifeParam", 1, &nParam);
		g_NpcSetting.GetFloat(nNpcTempRow, "LifeParam1", 0, &nParam1);
		g_NpcSetting.GetFloat(nNpcTempRow, "LifeParam2", 0, &nParam2);
		g_NpcSetting.GetFloat(nNpcTempRow, "LifeParam3", 0, &nParam3);
		m_LifeMax = nParam *  GetNpcLevelDataFromScript(pLevelScript, "Life", nSeries, nLevel, nParam1, nParam2, nParam3) / 100;
		if (m_LifeMax == 0) m_LifeMax = 100;
		
		g_NpcSetting.GetString(nNpcTempRow, "LifeReplenish", "", szValue1, MAX_VALUE_LEN);
		if(szValue1[0]) m_LifeReplenish = GetNpcLevelDataFromScript(pLevelScript, "LifeReplenish", nSeries, nLevel, szValue1);
		if(m_LifeReplenish == 0) m_LifeReplenish = 0; //Phôc håi sinh lùc, phuc hoi sinh luc, phsl

		g_NpcSetting.GetFloat(nNpcTempRow, "ARParam", 1, &nParam);
		g_NpcSetting.GetFloat(nNpcTempRow, "ARParam1", 0, &nParam1);
		g_NpcSetting.GetFloat(nNpcTempRow, "ARParam2", 0, &nParam2);
		g_NpcSetting.GetFloat(nNpcTempRow, "ARParam3", 0, &nParam3);
		m_AttackRating = nParam * GetNpcLevelDataFromScript(pLevelScript, "AR", nSeries, nLevel, nParam1, nParam2, nParam3) / 100;
		if (m_AttackRating == 0) m_AttackRating = 100;

		g_NpcSetting.GetFloat(nNpcTempRow, "DefenseParam", 1, &nParam); //phong thu
		g_NpcSetting.GetFloat(nNpcTempRow, "DefenseParam1", 0, &nParam1);
		g_NpcSetting.GetFloat(nNpcTempRow, "DefenseParam2", 0, &nParam2);
		g_NpcSetting.GetFloat(nNpcTempRow, "DefenseParam3", 0, &nParam3);
		m_Defend = nParam * GetNpcLevelDataFromScript(pLevelScript, "Defense", nSeries, nLevel, nParam1, nParam2, nParam3) / 100;
		if(m_Defend == 0) m_Defend = 10;

		g_NpcSetting.GetFloat(nNpcTempRow, "MinDamageParam", 1, &nParam);
		g_NpcSetting.GetFloat(nNpcTempRow, "MinDamageParam1", 0, &nParam1);
		g_NpcSetting.GetFloat(nNpcTempRow, "MinDamageParam2", 0, &nParam2);
		g_NpcSetting.GetFloat(nNpcTempRow, "MinDamageParam3", 0, &nParam3);
		m_PhysicsDamage.nValue[0] = nParam * MIN_DAMAGE_NPC_UPTO * GetNpcLevelDataFromScript(pLevelScript, "MinDamage", nSeries, nLevel, nParam1, nParam2, nParam3) / 100;
		if(m_PhysicsDamage.nValue[0] == 0) m_PhysicsDamage.nValue[0] = 100 * MIN_DAMAGE_NPC_UPTO;

		g_NpcSetting.GetFloat(nNpcTempRow, "MaxDamageParam", 1, &nParam);
		g_NpcSetting.GetFloat(nNpcTempRow, "MaxDamageParam1", 0, &nParam1);
		g_NpcSetting.GetFloat(nNpcTempRow, "MaxDamageParam2", 0, &nParam2);
		g_NpcSetting.GetFloat(nNpcTempRow, "MaxDamageParam3", 0, &nParam3);
		m_PhysicsDamage.nValue[2] = nParam * MAX_DAMAGE_NPC_UPTO * GetNpcLevelDataFromScript(pLevelScript, "MaxDamage", nSeries, nLevel, nParam1, nParam2, nParam3) / 100;
		if(m_PhysicsDamage.nValue[2] == 0) m_PhysicsDamage.nValue[2] = 100 * MAX_DAMAGE_NPC_UPTO;

		g_NpcSetting.GetString(nNpcTempRow, "RedLum", "", szValue1, MAX_VALUE_LEN);
		if(szValue1[0])	m_RedLum = GetNpcLevelDataFromScript(pLevelScript, "RedLum", nSeries, nLevel, szValue1);
		
		g_NpcSetting.GetString(nNpcTempRow, "GreenLum", "", szValue1, MAX_VALUE_LEN);
		if(szValue1[0]) m_GreenLum = GetNpcLevelDataFromScript(pLevelScript, "GreenLum", nSeries, nLevel, szValue1);
		
		g_NpcSetting.GetString(nNpcTempRow, "BlueLum", "", szValue1, MAX_VALUE_LEN);
		if(szValue1[0]) m_BlueLum = GetNpcLevelDataFromScript(pLevelScript, "BlueLum", nSeries, nLevel, szValue1);
		
		g_NpcSetting.GetString(nNpcTempRow, "FireResist", "", szValue1, MAX_VALUE_LEN);
		if(szValue1[0]) m_FireResist = GetNpcLevelDataFromScript(pLevelScript, "FireResist", nSeries, nLevel, szValue1);
		if(m_FireResist ==0) m_FireResist = 10;

		g_NpcSetting.GetString(nNpcTempRow, "ColdResist", "", szValue1, MAX_VALUE_LEN);
		if(szValue1[0]) m_ColdResist = GetNpcLevelDataFromScript(pLevelScript, "ColdResist", nSeries, nLevel, szValue1);
		if(m_ColdResist ==0) m_ColdResist = 10;

		g_NpcSetting.GetString(nNpcTempRow, "LightResist", "", szValue1, MAX_VALUE_LEN);
		if(szValue1[0]) m_LightResist = GetNpcLevelDataFromScript(pLevelScript, "LightResist", nSeries, nLevel, szValue1);
		if(m_LightResist ==0) m_LightResist = 10;
		
		g_NpcSetting.GetString(nNpcTempRow, "PoisonResist", "", szValue1, MAX_VALUE_LEN);
		if(szValue1[0]) m_PoisonResist = GetNpcLevelDataFromScript(pLevelScript, "PoisonResist", nSeries, nLevel, szValue1);
		if(m_PoisonResist ==0) m_PoisonResist = 10;

		g_NpcSetting.GetString(nNpcTempRow, "PhysicsResist", "", szValue1, MAX_VALUE_LEN);
		if(szValue1[0]) m_PhysicsResist = GetNpcLevelDataFromScript(pLevelScript, "PhysicsResist", nSeries, nLevel, szValue1);
		if(m_PhysicsResist ==0) m_PhysicsResist = 10;
		
		g_NpcSetting.GetString(nNpcTempRow, "AIMode", "2", szValue1, MAX_VALUE_LEN);
		if(szValue1[0])
		{
			if(strlen(szValue1) < 3) sprintf(szValue1, "%s|0", szValue1);
			m_AiMode = GetNpcLevelDataFromScript(pLevelScript, "AIMode", nSeries, nLevel, szValue1);
			if(m_AiMode == 0) m_AiMode = atoi(szValue1);
		}

		g_NpcSetting.GetString(nNpcTempRow, "AIParam1", "12", szValue1, MAX_VALUE_LEN);
		if(szValue1[0])
		{
			if(strlen(szValue1) < 3) sprintf(szValue1, "%s|0", szValue1);
			m_AiParam[0] = GetNpcLevelDataFromScript(pLevelScript, "AIParam1", nSeries, nLevel, szValue1);
			if(m_AiParam[0] == 0) m_AiParam[0] = atoi(szValue1);
		}

		g_NpcSetting.GetString(nNpcTempRow, "AIParam2", "12", szValue1, MAX_VALUE_LEN);
		if(szValue1[0])
		{
			if(strlen(szValue1) < 3) sprintf(szValue1, "%s|0", szValue1);
			m_AiParam[1] = GetNpcLevelDataFromScript(pLevelScript, "AIParam2", nSeries, nLevel, szValue1);
			if(m_AiParam[1] == 0) m_AiParam[1] = atoi(szValue1);
		}

		g_NpcSetting.GetString(nNpcTempRow, "AIParam3", "12", szValue1, MAX_VALUE_LEN);
		if(szValue1[0])
		{
			if(strlen(szValue1) < 3) sprintf(szValue1, "%s|0", szValue1);
			m_AiParam[2] = GetNpcLevelDataFromScript(pLevelScript, "AIParam3", nSeries, nLevel, szValue1);
			if(m_AiParam[2] == 0) m_AiParam[2] = atoi(szValue1);
		}

		g_NpcSetting.GetString(nNpcTempRow, "AIParam4", "12", szValue1, MAX_VALUE_LEN);
		if(szValue1[0])
		{
			if(strlen(szValue1) < 3) sprintf(szValue1, "%s|0", szValue1);
			m_AiParam[3] = GetNpcLevelDataFromScript(pLevelScript, "AIParam4", nSeries, nLevel, szValue1);
			if(m_AiParam[3] == 0) m_AiParam[3] = atoi(szValue1);
		}
		g_NpcSetting.GetString(nNpcTempRow, "AIParam5", "12", szValue1, MAX_VALUE_LEN);
		if(szValue1[0])
		{
			if(strlen(szValue1) < 3) sprintf(szValue1, "%s|0", szValue1);
			m_AiParam[4] = GetNpcLevelDataFromScript(pLevelScript, "AIParam5", nSeries, nLevel, szValue1);
			if(m_AiParam[4] == 0) m_AiParam[4] = atoi(szValue1);
		}
		g_NpcSetting.GetString(nNpcTempRow, "AIParam6", "12", szValue1, MAX_VALUE_LEN);
		if(szValue1[0])
		{
			if(strlen(szValue1) < 3) sprintf(szValue1, "%s|0", szValue1);
			m_AiParam[5] = GetNpcLevelDataFromScript(pLevelScript, "AIParam6", nSeries, nLevel, szValue1);
			if(m_AiParam[5] == 0) m_AiParam[5] = atoi(szValue1);
		}
		g_NpcSetting.GetString(nNpcTempRow, "AIParam7", "12", szValue1, MAX_VALUE_LEN);
		if(szValue1[0])
		{
			if(strlen(szValue1) < 3) sprintf(szValue1, "%s|0", szValue1);
			m_AiParam[6] = GetNpcLevelDataFromScript(pLevelScript, "AIParam7", nSeries, nLevel, szValue1);
			if(m_AiParam[6] == 0) m_AiParam[6] = atoi(szValue1);
		}
		
#endif
#ifndef _SERVER
		int nParam1 = 0;
		int nParam2 = 0;
		int nParam3 = 0;
		int nParam = 1;
		g_NpcSetting.GetInteger(nNpcTempRow, "LifeParam", 1, &nParam);
		g_NpcSetting.GetInteger(nNpcTempRow, "LifeParam1", 0, &nParam1);
		g_NpcSetting.GetInteger(nNpcTempRow, "LifeParam2", 0, &nParam2);
		g_NpcSetting.GetInteger(nNpcTempRow, "LifeParam3", 0, &nParam3);
		m_LifeMax = nParam *  GetNpcLevelDataFromScript(pLevelScript, "Life", nSeries, nLevel, nParam1, nParam2, nParam3) / 100;
		if (m_LifeMax == 0) m_LifeMax = 100;
#endif
	}
	pLevelScript->SafeCallEnd(nTopIndex);
}

int KNpcTemplate::GetNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName, int nSerial, int nLevel, char * szParam)
{
	int nTopIndex = 0;
	int nReturn = 0;
	if (szParam == NULL|| szParam[0] == 0 || strlen(szParam) < 3)
	{
		return 0;
	}
	pScript->SafeCallBegin(&nTopIndex);
	pScript->CallFunction("GetNpcLevelData", 1, "ddss", nSerial, nLevel, szDataName, szParam);//GetNpcLevelData function name trong script
	nTopIndex = Lua_GetTopIndex(pScript->m_LuaState);
	nReturn = (int) Lua_ValueToNumber(pScript->m_LuaState, nTopIndex);
	pScript->SafeCallEnd(nTopIndex);
	return nReturn;
}

int KNpcTemplate::GetNpcLevelDataFromScript(KLuaScript * pScript, char * szDataName,int nSerial, int nLevel, double nParam1, double nParam2, double nParam3)
{
	int nTopIndex = 0;
	int nReturn = 0;
	pScript->SafeCallBegin(&nTopIndex);
	pScript->CallFunction("GetNpcKeyData", 1, "ddsnnn", nSerial, nLevel, szDataName, nParam1, nParam2, nParam3);//GetNpcKeyData function name trong script
	nTopIndex = Lua_GetTopIndex(pScript->m_LuaState);
	nReturn = (int) Lua_ValueToNumber(pScript->m_LuaState, nTopIndex);
	pScript->SafeCallEnd(nTopIndex);
	return nReturn;
}

int KNpcTemplate::SkillString2Id(char * szSkillString)
{
	if (!szSkillString[0]) return 0;
	int nSkillNum = g_OrdinSkillsSetting.GetHeight() - 1;
	char szSkillName[100];
	for (int i = 0 ;  i < nSkillNum; i ++)
	{
		g_OrdinSkillsSetting.GetString(i + 2, "SkillName", "", szSkillName, sizeof(szSkillName));
		if (g_StrCmp(szSkillString, szSkillName))
		{
			int nSkillId = 0;
			g_OrdinSkillsSetting.GetInteger(i + 2, "SkillId", 0, &nSkillId);
			printf("Tim thay skillname= %s voi id= %d \n",szSkillString,nSkillId);
			return nSkillId;
		}
	}
	try
	{
		int i_skillid;
		sscanf(szSkillString, "%d", &i_skillid); // Using sscanf
		return i_skillid;
	} 
	catch(...)
	{
		printf("Error convert szSkillString to i_skillid edit by phong kieu \n");
		return 0;
	}
	return 0;
}



