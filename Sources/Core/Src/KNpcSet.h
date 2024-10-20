#ifndef	KNpcSetH
#define	KNpcSetH
//#include "KFindBinTree.h"
#include "KLinkArray.h"
#include "KNpc.h"
#include "GameDataDef.h"
#include "KNpcGold.h"

#ifndef _SERVER
#define		MAX_NPC_REQUEST	20
#endif

#ifndef _SERVER
#define		MAX_INSTANT_STATE	20
#define		MAX_INSTANT_SOUND	30
class	KInstantSpecial
{
private:
	int		m_nLoadFlag;
	char	m_szSprName[MAX_INSTANT_STATE][FILE_NAME_LENGTH];
	char	m_szSoundName[MAX_INSTANT_SOUND][FILE_NAME_LENGTH];

	KCacheNode	*m_pSoundNode;	
	KWavSound	*m_pWave;		

private:
	void	LoadSprName();
	void	LoadSoundName();

public:
	KInstantSpecial();
	void	GetSprName(int nNo, char *lpszName, int nLength);
	void	PlaySound(int nNo);
};
#endif

typedef struct
{
	DWORD	dwRequestId;
	DWORD	dwRequestTime;
} RequestNpc;

typedef struct
{
#ifndef _SERVER
	int		nStandFrame[2];
	int		nWalkFrame[2];
	int		nRunFrame[2];
#endif
	int		nWalkSpeed;
	int		nRunSpeed;
	int		nAttackFrame;
	int		nHurtFrame;
} PlayerBaseValue;

class KNpcSet
{
public:
	KNpcGoldTemplate	m_cGoldTemplate;					
	PlayerBaseValue		m_cPlayerBaseValue;					

#ifdef _SERVER
	int					m_nPKDamageRate;		
	int					m_nNpcSpecialDamageRate;
	int					m_nFactionPKFactionAddPKValue;	
	int					m_nKillerPKFactionAddPKValue;		
	int					m_nEnmityAddPKValue;				
	int					m_nBeKilledAddPKValue;				
	int					m_nLevelDistance;				
#endif
#ifndef _SERVER
	KInstantSpecial		m_cInstantSpecial;
#endif
private:
	DWORD				m_dwIDCreator;						
	KLinkArray			m_FreeIdx;							
	KLinkArray			m_UseIdx;							
#ifndef _SERVER
	enum
	{
		PATE_CHAT = 0x01,
		PATE_NAME = 0x02,
		PATE_LIFE = 0x04,
		PATE_MANA = 0x08,
	};
	int					m_nShowPateFlag;				
	RequestNpc			m_RequestNpc[MAX_NPC_REQUEST];		
	KLinkArray			m_RequestFreeIdx;					
	KLinkArray			m_RequestUseIdx;					
#endif
public:
	KNpcSet();
	void			Init();
	int				GetNpcCount(int nKind = -1, int nCamp = -1);
	int				SearchName(LPSTR szName);
	int				SearchID(DWORD dwID);
	int				SearchNameID(DWORD dwID);
	int				SearchUUID(DWORD dwID);
	int				SearchNameNpcID(DWORD dwID);
	BOOL			IsNpcExist(int nIdx, DWORD dwId);
	int				AddNpcSet1(int nNpcSettingIdxInfo, int nSeries, int nSubWorld, int nRegion, int nMapX, int nMapY, int nOffX = 0, int nOffY = 0);
	int				AddNpcSet2(int nNpcSettingIdxInfo, int nSeries, int nSubWorld, int nMpsX, int nMpsY);

#ifdef _SERVER
	int				AddNpcSet3(int nSubWorld, void* pNpcInfo);	//Hµm add npc Î server
#endif

	void			Remove(int nIdx);
	void			RemoveAll();
	NPC_RELATION	GetRelation(int nIdx1, int nIdx2);
	int				GetNearestNpc(int nMapX, int nMapY, int nId, int nRelation);
	static int		GetDistance(int nIdx1, int nIdx2);
	static int		GetDistanceSquare(int nIdx1, int nIdx2);
	int				GetNextIdx(int nIdx);
	void			ClearActivateFlagOfAllNpc();
	void			LoadPlayerBaseValue(LPSTR szFile);
	int				GetPlayerWalkSpeed() { return m_cPlayerBaseValue.nWalkSpeed; };
	int				GetPlayerRunSpeed() { return m_cPlayerBaseValue.nRunSpeed; };
	int				GetPlayerAttackFrame() { return m_cPlayerBaseValue.nAttackFrame; };
	int				GetPlayerHurtFrame() { return m_cPlayerBaseValue.nHurtFrame; };
	int				GetDistanceMps(int nRx1, int nRy1, int nRx2, int nRy2);
#ifndef _SERVER
	int				GetPlayerStandFrame(BOOL bMale) 
	{ 
		if (bMale)
			return m_cPlayerBaseValue.nStandFrame[0];
		else
			return m_cPlayerBaseValue.nStandFrame[1];
	};
	int				GetPlayerWalkFrame(BOOL bMale)
	{
		if (bMale)
			return m_cPlayerBaseValue.nWalkFrame[0];
		else
			return m_cPlayerBaseValue.nWalkFrame[1];
	};
	int				GetPlayerRunFrame(BOOL bMale)
	{
		if (bMale)
			return m_cPlayerBaseValue.nRunFrame[0];
		else
			return m_cPlayerBaseValue.nRunFrame[1];
	};
	BOOL			IsNpcRequestExist(DWORD	dwID);
	BOOL			InsertNpcRequest(DWORD dwID);
	void			RemoveNpcRequest(DWORD dwID);
	int				GetRequestIndex(DWORD dwID);
	int				AddClientNpc(int nTemplateID, int nRegionX, int nRegionY, int nMpsX, int nMpsY, int nNo);
	void			InsertNpcToRegion(int nRegionIdx);
	int				SearchClientID(KClientNpcID sClientID);
	int				SearchNpcAt(int nX, int nY, int nRelation, int nRange);
	void			CheckBalance();
	int				GetAroundPlayerForTeamInvite(KUiPlayerItem *pList, int nCount);	
	void			GetAroundOpenCaptain(int nCamp);		
	int				GetAroundPlayer(KUiPlayerItem *pList, int nCount);	
	int				GetAroundNpcMonster(KUiPlayerItem *pList, int nCount, int nRange);//Add by phong ki“u using fkauto
	int				GetAroundPlayerMonster(KUiPlayerItem* pList, int nCount, int nRange);//Add by phong ki“u using fkauto
	int				GetAroundNpcBossMonster(KUiPlayerItem* pList, int nCount, int nRange);//Add by phong ki“u using fkauto
	void			SetShowNameFlag(BOOL bFlag);
	BOOL			CheckShowName();
	void			SetShowChatFlag(BOOL bFlag);
	BOOL			CheckShowChat();
	void			SetShowLifeFlag(BOOL bFlag);
	BOOL			CheckShowLife();
	void			SetShowManaFlag(BOOL bFlag);
	BOOL			CheckShowMana();
	int				AutoGetNpcNear(int nX, int nY, int nRelation, int nRange, int ArrayLag[], int nSize, BOOL m_bFightCheckBox, BOOL m_bFightSelfDef);
#endif
#ifdef _SERVER
	BOOL			SyncNpc(DWORD dwID, int nClientIdx);
#endif
private:
	void			SetID(int m_nIndex);
	int				FindFree();
    // Add By Freeway Chen in 2003.7.14
private:
    // [NPC.m_Kind]NPC.m_Kind][NPC.m_CurrentCamp][NPC.m_CurrentCamp]
	unsigned char m_RelationTable[kind_num][kind_num][camp_num][camp_num][fight_num][fight_num]; // fix by lzlsky301
    // 
    int GenRelationTable();
	NPC_RELATION GenOneRelation(NPCKIND Kind1, NPCKIND Kind2, NPCCAMP Camp1, NPCCAMP Camp2, NPCFIGHT Fight1, NPCFIGHT Fight2); // fix by lzlsky301
};

// modify by Freeway Chen in 2003.7.14
// »∑∂®¡Ω∏ˆNPC÷Æº‰µƒ’Ω∂∑πÿœµ

extern KNpcSet NpcSet;
#endif
