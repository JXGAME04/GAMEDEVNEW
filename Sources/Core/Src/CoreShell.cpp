/*****************************************************************************************
//	Copyright : Kingsoft 2002
//	Author	:   Fong KiÒu
//	CreateTime:	2020-9-12
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "KCore.h"
#include "GameDataDef.h"
#include "CoreShell.h"
#include "CoreDrawGameObj.h"
#include "ImgRef.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KItemSet.h"
#include "KItemList.h"
#include "KSubWorldSet.h"
#include "KProtocolProcess.h"
#include "KItemGenerator.h"
#include "KNpcResList.h"
#include "Scene/KScenePlaceC.h"
#include "kskills.h"
#include "GameDataDef.h"
#include "MsgGenreDef.h"
#include "KOption.h"
#include "KSubWorld.h"
#include "KViewItem.h"
#include "KTongProtocol.h"
#include "malloc.h"
#include "KMagicDesc.h"
#include "KBuySell.h"
#include "KThiefSkill.h"
#include "KObjSet.h"
#include "KTaskFuns.h"
#include "KSellItem.h"

#include <cmath>
#include "KJXPathFinder.h"
#include "KMath.h"
#include "KDaTauCap.h"
#include "KDaTauTables.h"
#include "KDaTauSpots.h"

#define	NPC_TRADE_BOX_WIDTH		6
#define	NPC_TRADE_BOX_HEIGHT	10
#define	MAX_TRADE_ITEM_WIDTH	2
#define	MAX_TRADE_ITEM_HEIGHT	4
#define	MAX_GAMBLE_ITEM_WIDTH	2
#define	MAX_GAMBLE_ITEM_HEIGHT	4

IClientCallback* l_pDataChangedNotifyFunc = 0;
int KNpc::g_DrawVision;
int KNpc::g_DrawVisionSkill;
class KCoreShell : public iCoreShell
{
public:
	int	 GetProtocolSize(BYTE byProtocol);
	int	 Debug(unsigned int uDataId, unsigned int uParam, int nParam);
	int	 OperationRequest(unsigned int uOper, unsigned int uParam, int nParam);
	void ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam);
	int	 FindSelectNPC(int x, int y, int nRelation, bool bSelect, void* pReturn, int& nKind);
	int FindSelectObject(int x, int y, bool bSelect, int& nObjectIdx, int& nKind);
	int FindSpecialNPC(char* Name, void* pReturn, int& nKind);
	int ChatSpecialPlayer(void* pPlayer, const char* pMsgBuff, unsigned short nMsgLength);
	void ApplyAddTeam(void* pPlayer);
	void TradeApplyStart(void* pPlayer);
	void GambleApplyStart(void* pPlayer);
	int UseSkill(int x, int y, int nSkillID);
	int UseSkillCastB(int x, int y, int nSkillID, int nNpcIdx);
	int LockSomeoneUseSkill(int nTargetIndex, int nSkillID);
	int LockSomeoneAction(int nTargetIndex);
	int LockObjectAction(int nTargetIndex);
	void GotoWhere(int x, int y, int mode);	//mode 0 is auto, 1 is walk, 2 is run
	void Goto(int nDir, int mode);	//nDir 0~63, mode 0 is auto, 1 is walk, 2 is run
	void Turn(int nDir);	//nDir 0 is left, 1 is right, 2 is back
	int ThrowAwayItem();
	int GetNPCRelation(int nIndex);
	int GetNPCBAITAN(int nIndex);
	int	GetNPCBAITAN2();
	int GetGenreItem2(unsigned int uId);
	int GetPriceSell2(unsigned int uId);
	int GetTypeItem(unsigned int uId);
	int GetStallState();
	int GetPriceSell(unsigned int uId);
	int GetNatureItem(unsigned int uItemId, unsigned int uGenre);
	int GetGenreItem(unsigned int uItemId, unsigned int uGenre);
	int	SceneMapOperation(unsigned int uOper, unsigned int uParam, int nParam);
	int	TongOperation(unsigned int uOper, unsigned int uParam, int nParam);
	int TeamOperation(unsigned int uOper, unsigned int uParam, int nParam);
	int	 GetGameData(unsigned int uDataId, unsigned int uParam, int nParam);
	void DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam);
	void DrawGameSpace();
	DWORD GetPing();
	//void SendPing();
	int	 SetCallDataChangedNofify(IClientCallback* pNotifyFunc);
	void NetMsgCallbackFunc(void* pMsgData);
	void SetRepresentShell(struct iRepresentShell* pRepresent);
	void SetMusicInterface(void* pMusicInterface);
	void SetRepresentAreaSize(int nWidth, int nHeight);
	int  Breathe();
	void Release();	
	void SetClient(LPVOID pClient);
	void SendNewDataToServer(void* pData, int nLength);
	int	GetOwnValue(int nMoneyUnit);
	int GetDataSuperShop(int nSaleId, unsigned int uParam, int nParam);
	int GetObjAtCountRegionInSuperShop(int nSaleId, unsigned int uParam, int nParam);
	int GetDataDynamicShop(int nSaleId, unsigned int uParam, int nParam);
	int GetItemIdxNpcShop(char* szItemName);//fkauto
	BOOL GetAutoFlag();
	BOOL GetFightFlag();
	void SetActiveAutoPlay(BOOL nActive = FALSE);
	void SetMoveMap(int nType, int nPos, int nValue);
	void SetSortItem(int nType, int nPos, int nValue);
	void FkAutoSetFillterMagic(int nType, int nPos, int nValue);
	int FindSkillInfo(int nType, int nIndex);
	void GetSkillName(int nSkillId, char* szSkillName);
	BOOL GetSkillData(int nSkillId, int *nLevel);
	BOOL GetFlagMode();
	void SetFlagMode(bool nIndex);
	void DirectFindPos(unsigned int uParam, int nParam, BOOL bSync, BOOL bPaintLine);
	BYTE GetPaintMode();
	void SetPaintMode(BYTE nIndex);
	int AutoPlayOperation(unsigned int uOper, unsigned int uParam, int nParam);//fkauto
	//BOOL AutoMove();
	void ClearPathFinder();
	void GotoWhereDirect(int x, int y, int mode);	//mode 0 is auto, 1 is walk, 2 is run
	int	 CheckMapLoiDai();
};

struct sStation
{
	int x;
	int y;
};
sStation s53MedList[] =	//thuoc
{
	{51150, 102700},
};
sStation s53ShopList[] = //tap hoa
{
	{51205, 101480},
};
sStation s53MoveList[] = //xa phu
{
	{50517, 103568},
};
sStation s53Center = {51892, 101854}; //khong can quan tam center
//-----------
sStation s20MedList[] =
{
	{111034, 197262},
};
sStation s20ShopList[] =
{
	{107837, 200166},
};
sStation s20MoveList[] =
{
	{110670, 195540},
};
sStation s20Center = {113435, 198520};
//-----------
sStation s99MedList[] =
{
	{51054, 103276},
};
sStation s99ShopList[] =
{
	{51405, 105034},
};
sStation s99MoveList[] =
{
	{52171, 105754},
	{51440, 101176},
};
sStation s99Center = {52159, 102392};
//-----------
sStation s101MedList[] =
{
	{53734, 102290},
};
sStation s101ShopList[] =
{
	{52455, 100606},
};
sStation s101MoveList[] =
{
	{51942, 99132},
};
sStation s101Center = {54111, 100866};
//-----------
sStation s100MedList[] =
{
	{53038, 99966},
};
sStation s100ShopList[] =
{
	{52512, 100036},
};
sStation s100MoveList[] =
{
	{53707, 99260},
	{51543, 99112},
	{51307, 101368},
	{55022, 102942},
};
sStation s100Center = {52759, 100768};
//-----------
sStation s121MedList[] =
{
	{61906, 145656},
};
sStation s121ShopList[] =
{
	{61266, 144794},
};
sStation s121MoveList[] =
{
	{61578, 141662},
	{63385, 146996},
};
sStation s121Center = {62496, 144256};
//-----------
sStation s153MedList[] =
{
	{51224, 102696},
};
sStation s153ShopList[] =
{
	{52311, 103142},
};
sStation s153MoveList[] =
{
	{52181, 101696},
	{53928, 103822},
};
sStation s153Center = {52180, 103726};
//-----------
sStation s174MedList[] =
{
	{50323, 104126},
};
sStation s174ShopList[] =
{
	{50127, 102480},
};
sStation s174MoveList[] =
{
	{52306, 102274},
};
sStation s174Center = {51571, 102806};
//-----------
sStation s01MedList[] =
{
	{51280, 102082},
};
sStation s01ShopList[] =
{
	{49902, 102572},
};
sStation s01MoveList[] =
{
	{48596, 103334},
	{49892, 99436},
	{52694, 101276},
	{52640, 104736},
};
sStation s01Center = {51059, 102406};
//-----------
sStation s78MedList[] =
{
	{51584, 103800},
};
sStation s78ShopList[] =
{
	{52140, 104334},
};
sStation s78MoveList[] =
{
	{54146, 103434},
	{51138, 108056},
	{45996, 102746},
	{48309, 100412},
};
sStation s78Center = {50499, 103440};
//-----------
sStation s11MedList[] =
{
	{100535, 164300},
};
sStation s11ShopList[] =
{
	{98992, 164338},
};
sStation s11MoveList[] =
{
	{102323, 166194},
	{96480, 162976},
	{96886, 158658},
	{102176, 166080},
};
sStation s11Center = {100425, 162088};
//-----------
sStation s162MedList[] =
{
	{47983, 102686},
};
sStation s162ShopList[] =
{
	{49138, 102416},
};
sStation s162MoveList[] =
{
	{47032, 104606},
	{53366, 100000},
	{54355, 104858},
};
sStation s162Center = {50818, 100638};
//-----------
sStation s37MedList[] =
{
	{56804, 98886},
};
sStation s37ShopList[] =
{
	{57170, 99270},
};
sStation s37MoveList[] =
{
	{54561, 103124},
	{52176, 101964},
	{51024, 95796},
	{59569, 93640},
};
sStation s37Center = {55757, 98198};
//-----------
sStation s80MedList[] =
{
	{56778, 98524},
};
sStation s80ShopList[] =
{
	{54484, 96666},
};
sStation s80MoveList[] =
{
	{53478, 95684},
	{58383, 98012},
	{55200, 102592},
	{50970, 102168},
};
sStation s80Center = {56569, 97092};
//-----------
sStation s176MedList[] =
{
	{51725, 95238},
	{49423, 94814},
	{53938, 106180},
	{47228, 107542},
};
sStation s176ShopList[] =
{
	{42868, 101256},
};
sStation s176MoveList[] =
{
	{43279, 96462},
	{43889, 106572},
	{54287, 105342},
	{51250, 93178},
};
sStation s176Center = {50416, 94524};
//-----------
typedef std::vector<sStation>	StationVector;
typedef std::map<int, StationVector> MapStation;
static MapStation g_MedicineStation;
static MapStation g_ShopStation;
static MapStation g_MoveStation;
typedef std::map<int, sStation> MapOneStation;
static MapOneStation g_CenterStation;
static int g_GoMapID[] = 
{
	875,
	322,
	321,
	75,
	225,
	226,
	227,
	336,
	340,
	144,
	93,
	124,
	152,
	224,
	198,
	320,
	181,
	319,
	123,
	206,
	79,
	56,
	166,
	182,
	164,
	21,
	167,
	193,
	170,
	19,
	7,
};

struct BOTTLE_CTRL_MAP
{
	int				nDetail;
	int				nLevel;
};

static BOTTLE_CTRL_MAP g_LifeBottle[] =
{
	{0, 1},
	{0, 2},
	{0, 3},
	{0, 4},
	{0, 5},
	{2, 1},
	{2, 2},
	{2, 3},
	{2, 4},
	{2, 5},
};

static BOTTLE_CTRL_MAP g_ManaBottle[] =
{
	{1, 1},
	{1, 2},
	{1, 3},
	{1, 4},
	{1, 5},
	{2, 1},
	{2, 2},
	{2, 3},
	{2, 4},
	{2, 5},
};

static BOTTLE_CTRL_MAP g_PoisonBottle[] =
{
	{4, 1},
	{4, 2},
	{4, 3},
	{4, 4},
	{4, 5},
};

static KCoreShell	g_CoreShell;

CORE_API void g_InitCore(char * nParmName);
#ifndef _STANDALONE
extern "C" __declspec(dllexport)
#endif
iCoreShell* CoreGetShell(char * nParmName)
{
	g_InitCore(nParmName);
	//add auto station pos
	int count,i;
	count = sizeof(s53MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[53].push_back(s53MedList[i]);
	}
	count = sizeof(s53ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[53].push_back(s53ShopList[i]);
	}
	count = sizeof(s53MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[53].push_back(s53MoveList[i]);
	}
	g_CenterStation[53] = s53Center;
	//---------
	count = sizeof(s20MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[20].push_back(s20MedList[i]);
	}
	count = sizeof(s20ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[20].push_back(s20ShopList[i]);
	}
	count = sizeof(s20MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[20].push_back(s20MoveList[i]);
	}
	g_CenterStation[20] = s20Center;
	//---------
	count = sizeof(s99MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[99].push_back(s99MedList[i]);
	}
	count = sizeof(s99ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[99].push_back(s99ShopList[i]);
	}
	count = sizeof(s99MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[99].push_back(s99MoveList[i]);
	}
	g_CenterStation[99] = s99Center;
	//---------
	count = sizeof(s101MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[101].push_back(s101MedList[i]);
	}
	count = sizeof(s101ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[101].push_back(s101ShopList[i]);
	}
	count = sizeof(s101MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[101].push_back(s101MoveList[i]);
	}
	g_CenterStation[101] = s101Center;
	//---------
	count = sizeof(s100MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[100].push_back(s100MedList[i]);
	}
	count = sizeof(s100ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[100].push_back(s100ShopList[i]);
	}
	count = sizeof(s100MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[100].push_back(s100MoveList[i]);
	}
	g_CenterStation[100] = s100Center;
	//---------
	count = sizeof(s121MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[121].push_back(s121MedList[i]);
	}
	count = sizeof(s121ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[121].push_back(s121ShopList[i]);
	}
	count = sizeof(s121MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[121].push_back(s121MoveList[i]);
	}
	g_CenterStation[121] = s121Center;
	//---------
	count = sizeof(s153MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[153].push_back(s153MedList[i]);
	}
	count = sizeof(s153ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[153].push_back(s153ShopList[i]);
	}
	count = sizeof(s153MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[153].push_back(s153MoveList[i]);
	}
	g_CenterStation[153] = s153Center;
	//---------
	count = sizeof(s174MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[174].push_back(s174MedList[i]);
	}
	count = sizeof(s174ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[174].push_back(s174ShopList[i]);
	}
	count = sizeof(s174MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[174].push_back(s174MoveList[i]);
	}
	g_CenterStation[174] = s174Center;
	//---------
	count = sizeof(s01MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[1].push_back(s01MedList[i]);
	}
	count = sizeof(s01ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[1].push_back(s01ShopList[i]);
	}
	count = sizeof(s01MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[1].push_back(s01MoveList[i]);
	}
	g_CenterStation[1] = s01Center;
	//---------
	count = sizeof(s78MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[78].push_back(s78MedList[i]);
	}
	count = sizeof(s78ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[78].push_back(s78ShopList[i]);
	}
	count = sizeof(s78MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[78].push_back(s78MoveList[i]);
	}
	g_CenterStation[78] = s78Center;
	//---------
	count = sizeof(s11MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[11].push_back(s11MedList[i]);
	}
	count = sizeof(s11ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[11].push_back(s11ShopList[i]);
	}
	count = sizeof(s11MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[11].push_back(s11MoveList[i]);
	}
	g_CenterStation[11] = s11Center;
	//---------
	count = sizeof(s162MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[162].push_back(s162MedList[i]);
	}
	count = sizeof(s162ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[162].push_back(s162ShopList[i]);
	}
	count = sizeof(s162MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[162].push_back(s162MoveList[i]);
	}
	g_CenterStation[162] = s162Center;
	//---------
	count = sizeof(s37MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[37].push_back(s37MedList[i]);
	}
	count = sizeof(s37ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[37].push_back(s37ShopList[i]);
	}
	count = sizeof(s37MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[37].push_back(s37MoveList[i]);
	}
	g_CenterStation[37] = s37Center;
	//---------
	count = sizeof(s80MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[80].push_back(s80MedList[i]);
	}
	count = sizeof(s80ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[80].push_back(s80ShopList[i]);
	}
	count = sizeof(s80MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[80].push_back(s80MoveList[i]);
	}
	g_CenterStation[80] = s80Center;
	//---------
	count = sizeof(s176MedList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MedicineStation[176].push_back(s176MedList[i]);
	}
	count = sizeof(s176ShopList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_ShopStation[176].push_back(s176ShopList[i]);
	}
	count = sizeof(s176MoveList) / sizeof(sStation);
	for (i = 0; i < count; ++i)
	{
		g_MoveStation[176].push_back(s176MoveList[i]);
	}
	g_CenterStation[176] = s176Center;
	////////////
	return &g_CoreShell;
}

int CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam)
{
	if (l_pDataChangedNotifyFunc)
		return l_pDataChangedNotifyFunc->CoreDataChanged(uDataId, uParam, nParam);
	return 0;
}

void SendDataToTool(const void * const pData, const size_t &datalength)
{
	if (l_pDataChangedNotifyFunc)
		l_pDataChangedNotifyFunc->SendDataToTool(pData, datalength);
}

void KCoreShell::Release()
{
	g_ReleaseCore();
}

void KCoreShell::NetMsgCallbackFunc(void* pMsgData)
{
	g_ProtocolProcess.ProcessNetMsg((BYTE *)pMsgData);
}

int	KCoreShell::SetCallDataChangedNofify(IClientCallback* pNotifyFunc)
{
	l_pDataChangedNotifyFunc = pNotifyFunc;
	return true;
}

BOOL KCoreShell::GetSkillData(int nSkillId, int *nLevel)
{
	int nList = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(nSkillId);
	if (nList > 0)
	{
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(nSkillId) > 0)
		{
			*nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nSkillId);
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetAddLevelIndex(nList) > 0)
				return TRUE;
			else
				return FALSE;
		}
	}
	return FALSE;
}

BOOL KCoreShell::GetFlagMode()
{
	return g_ScenePlace.bFlagMode;
}

void KCoreShell::SetFlagMode(bool bFlag)
{
	g_ScenePlace.bFlagMode = bFlag;
}

void KCoreShell::DirectFindPos(unsigned int uParam, int nParam, BOOL bSync, BOOL bPaintLine)
{
	g_ScenePlace.DirectFindPos(uParam, nParam, bSync, bPaintLine);
}

BYTE KCoreShell::GetPaintMode()
{
	return g_ScenePlace.bPaintMode;
}

void KCoreShell::SetPaintMode(BYTE bFlag)
{
	g_ScenePlace.DirectFindPos(0, 0, FALSE, FALSE);
	g_ScenePlace.bPaintMode = bFlag;
}

int	KCoreShell::GetGameData(unsigned int uDataId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uDataId)
	{
	case GDI_TASK_SAVE_VALUE:	// [TaskGuide] ban sao task value (dong bo qua UI_TASKVALUE)
		nRet = (int)Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal((int)uParam);
		break;
	case GDI_PLAYER_REPUTE_VALUE:	// [TaskGuide] danh vong hien tai
		if (Player[CLIENT_PLAYER_INDEX].m_nIndex > 0)
			nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].nRepute;
		break;
	case GDI_PLAYER_IS_MALE:
		{
			int nIndex = 0;
			if (nParam == 0)
				nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			else
				nIndex = NpcSet.SearchID(nParam);

			if (nIndex)
				nRet = (Npc[nIndex].m_NpcSettingIdx == PLAYER_MALE_NPCTEMPLATEID);
			else
				nRet = 1;	
		}
		break;
	case GDI_REPAIR_ITEM_PRICE:
		if (uParam)
		{
			KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
			KItem*	pItem = NULL;

			switch(pObj->Obj.uGenre)
			{
			case CGOG_ITEM:
				{
					if (pObj->Obj.uId > 0)
					{
						pItem = &Item[pObj->Obj.uId];
					}
				}
				break;
			default:
				break;
			}

			if (!pItem)
				break;

			KUiItemBuySelInfo *pInfo = (KUiItemBuySelInfo *)nParam;
		
			if (pObj->eContainer == UOC_NPC_SHOP)
				break;
			pInfo->nCurPrice = pItem->GetRepairPrice();
			strcpy(pInfo->szItemName, pItem->GetName());
			nRet = pItem->CanBeRepaired();
			if(!nRet)//#thong bao khong the sua trang bi hong
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_NO_REPAIR_MONEY);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}
		}
		else
		{
			nParam = 0;
			nRet = 0;
		}
		break;
	case GDI_TRADE_ITEM_PRICE:
		if (uParam)
		{
			KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
			KItem*	pItem = NULL;

			switch(pObj->Obj.uGenre)
			{
			case CGOG_PLAYERSELLITEM:
				{
					if (pObj->Obj.uId > 0)
					{
						KUiItemBuySelInfo *pInfo = (KUiItemBuySelInfo *)nParam;
						pInfo->nPrice = Item[pObj->Obj.uId].GetSetPrice();
						pInfo->nCurPrice = Item[pObj->Obj.uId].GetSetPrice();
						strcpy(pInfo->szItemName, Item[pObj->Obj.uId].GetName());
						return 1;
					}
				}
				break;
			case CGOG_ITEM:
				{
					if (pObj->Obj.uId > 0)
					{
						pItem = &Item[pObj->Obj.uId];
					}
				}
				break;
			case CGOG_NPCSELLITEM:
				{
					int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];
					if (nBuyIdx != -1)
					{
						int nIndex = BuySell.GetItemIndex(nBuyIdx, pObj->Obj.uId);
						if (nIndex >= 0)
							pItem = BuySell.GetItem(nIndex);
					}
				}
				break;
			default:
				break;
			}

			if (!pItem)
				break;

			KUiItemBuySelInfo *pInfo = (KUiItemBuySelInfo *)nParam;
			pInfo->nItemNature = pItem->GetNature();
			pInfo->nOldPrice = pItem->GetOrgPrice();		
			if (pObj->eContainer == UOC_NPC_SHOP)
				pInfo->nCurPrice = pItem->GetCurPrice();
			else
				pInfo->nCurPrice = pItem->GetSalePrice();
			strcpy(pInfo->szItemName, pItem->GetName());
			pInfo->bNewArrival = pItem->GetNewArrival();
			pInfo->nMoneyUnit = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nMoneyUnit;
			nRet = 1;
		}
		else
		{
			nParam = 0;
			nRet = 0;
		}
		break;

	case GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO:
	case GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO:
		if (nParam && uParam)
		{
			KUiObjAtContRegion* pObj = (KUiObjAtContRegion *)uParam;
			char* pszDescript = (char *)nParam;
			pszDescript[0] = 0;
			switch(pObj->Obj.uGenre)
			{
			case CGOG_PLAYERSELLITEM:
				{
					//Item[pObj->Obj.uId].GetDesc(pszDescript, true, BUY_SELL_SCALE);
					Item[pObj->Obj.uId].GetDesc(pszDescript, true, true, 0);
				}
				break;
			case CGOG_ITEM:
				{
					if (pObj->eContainer == UOC_EQUIPTMENT)
					{
						int nActive = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetActiveAttribNum(pObj->Obj.uId);
						Item[pObj->Obj.uId].GetDesc(pszDescript, true, true, nActive);
					}
					else
					{
						int money_unit = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nMoneyUnit;
						int item_index = pObj->Obj.uId;
						int item_place = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[item_index].nPlace;
						if(money_unit == moneyunit_money && item_place == pos_equiproom)//#chØ hiÖn thØ gi¸ b¸n b»ng tiÒn v¹n
						{
							Item[pObj->Obj.uId].GetDesc(pszDescript, true, true, 0);
						}
						else
						{
							Item[pObj->Obj.uId].GetDesc(pszDescript, false, false);
						}
					}
				}
				break;			
			case CGOG_NPCSELLITEM:
				{
					int nIdx = -1;
					if (-1 == Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop])
						break;
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop], pObj->Obj.uId);

					KItem* pItem = NULL;
					if (nIdx < 0)
						break;
					pItem = BuySell.GetItem(nIdx);

					if (!pItem)
						break;
					pItem->GetDesc(pszDescript, true, false);
				}
				break;
			}
		}
		break;
	case GDI_GAME_OBJ_DESC:
		if (nParam && uParam)
		{
			KUiObjAtContRegion* pObj = (KUiObjAtContRegion *)uParam;
			char* pszDescript = (char *)nParam;
			pszDescript[0] = 0;
			switch(pObj->Obj.uGenre)
			{
			case CGOG_ITEM:
				{
					if (pObj->eContainer == UOC_EQUIPTMENT)
					{
						int nActive = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetActiveAttribNum(pObj->Obj.uId);
						int nGoldActive = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetGoldActiveAttribNum(pObj->Obj.uId);
						if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetIfActive()) //check if set is active
						//if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetIfActive(pObj->Obj.uId)) //check if item is active
						{
							nActive = 4;
						}
						//Item[pObj->Obj.uId].GetDesc(pszDescript, false, 1, nActive);
						Item[pObj->Obj.uId].GetDesc(pszDescript, false, false, nActive, nGoldActive);
					}
					else
					{
						if(Item[pObj->Obj.uId].IsFkItemSkill() == true)
						{
							int nSkillId = Item[pObj->Obj.uId].GetParticular();
							int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(nSkillId);
							if(nLevel > 0)
								nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nSkillId);
							KSkill::GetDesc(nSkillId, nLevel, pszDescript, Player[CLIENT_PLAYER_INDEX].m_nIndex, false);
						}
						else
							Item[pObj->Obj.uId].GetDesc(pszDescript, false, false);
					}

					if(Item[pObj->Obj.uId].GetPlayerItemIsHoureOpen())//xu ly vat pham cho mo khoa bao hiem
					{
						PLAYER_LOCK_UNLOCK_ITEM lockCmd;
						lockCmd.ProtocolType = c2s_plockitem;
						lockCmd.itemIdx = Item[pObj->Obj.uId].GetID();
						lockCmd.islock = 0;
						if (g_pClient)
							g_pClient->SendPackToServer((BYTE*)&lockCmd, sizeof(PLAYER_LOCK_UNLOCK_ITEM));
					}

					if(Item[pObj->Obj.uId].GetTimeYearIsExp())//xu ly vat pham het han su dung
					{
						REMOVE_ITEM_YEAR_EXP lockCmd;
						lockCmd.ProtocolType = c2s_itemyearexp;
						lockCmd.itemIdx = Item[pObj->Obj.uId].GetID();
						if (g_pClient)
							g_pClient->SendPackToServer((BYTE*)&lockCmd, sizeof(REMOVE_ITEM_YEAR_EXP));
					}
				}
				break;
			case CGOG_SKILL:
			case CGOG_SKILL_FIGHT:
			case CGOG_SKILL_LIVE:
			case CGOG_SKILL_SHORTCUT:
				{
					int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLevel(pObj->Obj.uId);
					if(nLevel > 0)
						nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(pObj->Obj.uId);
					_ASSERT(nLevel >= 0);
					if (pObj->Obj.uId >0)
					{						
						ISkill * pISkill = g_SkillManager.GetSkill(pObj->Obj.uId, 1);
						if (!pISkill)
							break;
						eSkillStyle eStyle = (eSkillStyle) pISkill->GetSkillStyle();
						
						switch(eStyle)
						{
						case SKILL_SS_Missles:			
						case SKILL_SS_Melee:
						case SKILL_SS_InitiativeNpcState:	
						case SKILL_SS_PassivityNpcState:		
							{
								int nList = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.FindSame(pObj->Obj.uId);
									KSkill::GetDesc(
									pObj->Obj.uId,
									nLevel,
									pszDescript,
									Player[CLIENT_PLAYER_INDEX].m_nIndex,
									(pObj->Obj.uGenre == CGOG_SKILL_SHORTCUT)?false:true
									);
							}
							break;
							
						case SKILL_SS_Thief:
							{
								((KThiefSkill *)pISkill)->GetDesc(
									pObj->Obj.uId,
									nLevel,
									pszDescript,
									Player[CLIENT_PLAYER_INDEX].m_nIndex,
									(pObj->Obj.uGenre == CGOG_SKILL_SHORTCUT)?false:true
								);
			
							}break;
							
						}
					}
				}
				break;
			case CGOG_PLAYER_FACE:
				break;
			case CGOG_NPCSELLITEM:
				{
					int nIdx = -1;
					if (-1 == Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop])
						break;
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop], pObj->Obj.uId);

					KItem* pItem = NULL;
					if (nIdx < 0)
						break;
					pItem = BuySell.GetItem(nIdx);

					if (!pItem)
						break;
					pItem->GetDesc(pszDescript, false, false);
				}
				break;
			case CGOG_IME_ITEM:
				{
					if (pObj->Obj.uId <= 0)
						return 0;
					Item[pObj->Obj.uId].GetDesc(pszDescript, false, false);
				}
				break;
			case CGOG_PLAYERSELLITEM:
				{
					Item[pObj->Obj.uId].GetDesc(pszDescript, false, false);
				}
				break;
			}
		}
		break;

	case GDI_PLAYER_BASE_INFO:
		if (uParam)
		{
			KUiPlayerBaseInfo* pInfo = (KUiPlayerBaseInfo*)uParam;
			int nIndex = 0;
			if (nParam == 0)
			{
				nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
				pInfo->nCurFaction = Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nCurFaction;
				pInfo->nFirstAddFaction = Player[CLIENT_PLAYER_INDEX].m_cFaction.m_nFirstAddFaction;
				pInfo->nCurTong = Player[CLIENT_PLAYER_INDEX].m_cTong.GetTongNameID();
				pInfo->nMissionGroup = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nMissionGroup;
			}
			else
			{
				nIndex = NpcSet.SearchID(nParam);
				pInfo->nCurFaction = -1;
				pInfo->nCurTong = 0;
				pInfo->nMissionGroup = -1;
			}
			if (nIndex)
			{
				strcpy(pInfo->Name, Npc[nIndex].Name);
				//to do:no implements in this version
				pInfo->Agname[0] = 0;
				pInfo->Title[0] = 0;
				strcpy(pInfo->szMateName, Npc[nIndex].MateName);//#MateName
				if (Npc[nIndex].m_btRankId)
				{
					char szRankId[5];
					itoa(Npc[nIndex].m_btRankId, szRankId, 10);
					g_RankTabSetting.GetString(szRankId, "RANKSTR", "", pInfo->Title, 32);
				}
				pInfo->nRankInWorld = Player[CLIENT_PLAYER_INDEX].m_nWorldStat; //xÕp h¹ng thÕ giíi
				pInfo->nRankInWorld = Npc[nIndex].nRankInWorld;		//tnxh
				pInfo->nRepute = Npc[nIndex].nRepute; // danh vong
				pInfo->nFuYuan = Npc[nIndex].nFuYuan;// phuc duyen
				pInfo->nPKValue = Npc[nIndex].nPKValue; // PK
				pInfo->nReBorn = Npc[nIndex].nReBorn; // trung sinh
			//	pInfo->nReBorn = Player[nIndex].m_cReBorn.GetReBornCurLevel(); // trung sinh
			}
		}
		break;

	case GDI_PLAYER_RT_INFO:
		if (uParam)
		{
			KUiPlayerRuntimeInfo* pInfo = (KUiPlayerRuntimeInfo*)uParam;
			pInfo->nLifeFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLifeMax;		
			pInfo->nLife = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentLife;				
			pInfo->nManaFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentManaMax;	
			pInfo->nMana = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentMana;				
			pInfo->nStaminaFull = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStaminaMax;
			pInfo->nStamina = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentStamina;		
			pInfo->nAngryFull = 0;		
			pInfo->nAngry = 0;			
			pInfo->nExperienceFull = Player[CLIENT_PLAYER_INDEX].m_nNextLevelExp;		
			pInfo->nExperience = Player[CLIENT_PLAYER_INDEX].m_nExp;					
			pInfo->nCurLevelExperience = Player[CLIENT_PLAYER_INDEX].m_nNextLevelExp;
			pInfo->byActionDisable = 0;
			pInfo->byAction = PA_NONE;

			if (Player[CLIENT_PLAYER_INDEX].m_RunStatus)
				pInfo->byAction |= PA_RUN;

			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing == do_sit)
				pInfo->byAction |= PA_SIT;
			
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
				pInfo->byAction |= PA_RIDE;

			pInfo->wReserved = 0;
		}
		break;

	case GDI_PLAYER_RT_ATTRIBUTE:
		if (uParam)
		{
			KUiPlayerAttribute* pInfo = (KUiPlayerAttribute*)uParam;
			KNpc*	pNpc = &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex];
			pInfo->nMoney = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment);				
			pInfo->nBARemainPoint = Player[CLIENT_PLAYER_INDEX].m_nAttributePoint;					
			pInfo->nStrength = Player[CLIENT_PLAYER_INDEX].m_nCurStrength;								
			pInfo->nDexterity = Player[CLIENT_PLAYER_INDEX].m_nCurDexterity;								
			pInfo->nVitality = Player[CLIENT_PLAYER_INDEX].m_nCurVitality;								
			pInfo->nEnergy = Player[CLIENT_PLAYER_INDEX].m_nCurEngergy;									
			pInfo->nPKValue = Player[CLIENT_PLAYER_INDEX].m_cPK.GetPKValue();	//tnpk
			//pInfo->nRepute = Player[CLIENT_PLAYER_INDEX].m_cRepute.GetReputeValue(); // danh vong
			pInfo->nRepute = pNpc->nRepute;
			//pInfo->nFuYuan = Player[CLIENT_PLAYER_INDEX].m_cFuYuan.GetFuYuanValue(); // phuc duyen
			pInfo->nFuYuan = pNpc->nFuYuan;
			pInfo->nReBorn = Player[CLIENT_PLAYER_INDEX].m_cReBorn.GetReBornNum(); // trung sinh
			pInfo->nRankInWorld = Player[CLIENT_PLAYER_INDEX].m_nWorldStat; //xÕp h¹ng thÕ giíi

			Player[CLIENT_PLAYER_INDEX].GetEchoDamage(&pInfo->nKillMIN, &pInfo->nKillMAX, 0);				
			Player[CLIENT_PLAYER_INDEX].GetEchoDamage(&pInfo->nRightKillMin , &pInfo->nRightKillMax, 1);
			pInfo->nAttack = pNpc->m_CurrentAttackRating;				
			pInfo->nDefence = pNpc->m_CurrentDefend;					
			pInfo->nMoveSpeed = pNpc->m_CurrentRunSpeed;				
			pInfo->nAttackSpeed = pNpc->m_CurrentAttackSpeed;			
			pInfo->nCastSpeed = pNpc->m_CurrentCastSpeed;
			memcpy(pInfo->bMeridianLevel, Player[CLIENT_PLAYER_INDEX].m_cMeridian.getMeridian(), sizeof(pInfo->bMeridianLevel));

			if (pNpc->m_CurrentPhysicsResistMax >= pNpc->m_CurrentPhysicsResist)
			{
				pInfo->nPhyDef = pNpc->m_CurrentPhysicsResist;
				pInfo->nPhyDefPlus = 0;
			}
			else
			{
				pInfo->nPhyDef = pNpc->m_CurrentPhysicsResistMax;
				pInfo->nPhyDefPlus = (pNpc->m_CurrentPhysicsResist - pNpc->m_CurrentPhysicsResistMax) / RESIST_PLUS_SCALE;
			}

			if (pNpc->m_CurrentColdResistMax >= pNpc->m_CurrentColdResist)
			{
				pInfo->nCoolDef = pNpc->m_CurrentColdResist;
				pInfo->nCoolDefPlus = 0;
			}
			else
			{
				pInfo->nCoolDef = pNpc->m_CurrentColdResistMax;
				pInfo->nCoolDefPlus = (pNpc->m_CurrentColdResist - pNpc->m_CurrentColdResistMax) / RESIST_PLUS_SCALE;
			}

			if (pNpc->m_CurrentLightResistMax >= pNpc->m_CurrentLightResist)
			{
				pInfo->nLightDef = pNpc->m_CurrentLightResist;
				pInfo->nLightDefPlus = 0;
			}
			else
			{
				pInfo->nLightDef = pNpc->m_CurrentLightResistMax;
				pInfo->nLightDefPlus = (pNpc->m_CurrentLightResist - pNpc->m_CurrentLightResistMax) / RESIST_PLUS_SCALE;
			}

			if (pNpc->m_CurrentFireResistMax >= pNpc->m_CurrentFireResist)
			{
				pInfo->nFireDef = pNpc->m_CurrentFireResist;
				pInfo->nFireDefPlus = 0;
			}
			else
			{
				pInfo->nFireDef = pNpc->m_CurrentFireResistMax;
				pInfo->nFireDefPlus = (pNpc->m_CurrentFireResist - pNpc->m_CurrentFireResistMax) / RESIST_PLUS_SCALE;
			}

			if (pNpc->m_CurrentPoisonResistMax >= pNpc->m_CurrentPoisonResist)
			{
				pInfo->nPoisonDef = pNpc->m_CurrentPoisonResist;
				pInfo->nPoisonDefPlus = 0;
			}
			else
			{
				pInfo->nPoisonDef = pNpc->m_CurrentPoisonResistMax;
				pInfo->nPoisonDefPlus = (pNpc->m_CurrentPoisonResist - pNpc->m_CurrentPoisonResistMax) / RESIST_PLUS_SCALE;
			}
			pInfo->nLevel = pNpc->m_Level;

			//edit by phong kieu ngu hanh
			memset(pInfo->StatusDesc, 0, sizeof(pInfo->StatusDesc));
			switch(pNpc->m_Series)
			{
			case series_water:
				strcpy(pInfo->StatusDesc, "HÖ Thñy");
				break;
			case series_wood:
				strcpy(pInfo->StatusDesc, "HÖ Méc");
				break;
			case series_metal:
				strcpy(pInfo->StatusDesc, "HÖ Kim");
				break;
			case series_fire:
				strcpy(pInfo->StatusDesc, "HÖ Háa");
				break;
			case series_earth:
				strcpy(pInfo->StatusDesc, "HÖ Thæ");
				break;
			}
		}
		break;

	case GDI_PLAYER_IMMED_ITEMSKILL:
		if (uParam)
		{
			KUiPlayerImmedItemSkill* pInfo = (KUiPlayerImmedItemSkill*)uParam;
			memset(pInfo,0,sizeof(KUiPlayerImmedItemSkill));
			pInfo->IMmediaSkill[0].uGenre	= CGOG_SKILL_SHORTCUT;
			pInfo->IMmediaSkill[0].uId		= Player[CLIENT_PLAYER_INDEX].GetLeftSkill();
			pInfo->IMmediaSkill[1].uGenre	= CGOG_SKILL_SHORTCUT;
			pInfo->IMmediaSkill[1].uId		= Player[CLIENT_PLAYER_INDEX].GetRightSkill();

			for (int i = 0; i < MAX_IMMEDIACY_ITEM; i++)
			{
				pInfo->ImmediaItem[i].uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Room[room_immediacy].FindItem(i, 0);
				if (pInfo->ImmediaItem[i].uId > 0)
				{
					pInfo->ImmediaItem[i].uGenre = CGOG_ITEM;
				}
				else
				{
					pInfo->ImmediaItem[i].uGenre = CGOG_NOTHING;
				}
			}
		}
		break;

	case GDI_TRADE_PLAYER_ITEM_COUNT:
		nRet = 0;
		SendClientCmdGetCount(uParam);
		break;

	case GDI_TRADE_PLAYER_ITEM:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtContRegion* pInfo = (KUiObjAtContRegion*)uParam;
			
			for (int i = 0; i < 60; i++)
			{
				if (g_cSellItem.m_sItem[i].nIdx)
				{
					pInfo->Obj.uGenre = CGOG_PLAYERSELLITEM;
					pInfo->Obj.uId = g_cSellItem.m_sItem[i].nIdx;
					
					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = g_cSellItem.m_sItem[i].nX;
					pInfo->Region.v = g_cSellItem.m_sItem[i].nY;
					pInfo->nContainer = g_cSellItem.m_sItem[i].nPrice;
					nCount++;
					pInfo++;
				}		
			}
			
			nRet = nCount;
		} 
		else
		{
			nRet = g_cSellItem.GetCount();
		}
		break;

	case GDI_PLAYER_IS_BAITAN:
		nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan;
		break;
	case GDI_AFFAIR_ITEM:		// pos tra vat pham nhiem vu
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_affairitem)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_affairitem)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_affairitem)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_affairitem)
					nCount++;
			}
			nRet = nCount;
		}
		break;

	case GDI_PLAYER_HOLD_MONEY:	
		nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment);
		break;

	case GDI_PLAYER_HOLD_FKCOIN:	//add by phong kiÒu sè xu ng­êi trªn player
		nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_XU);
		break;

	case GDI_GET_PLAYERNPC_INDEX:
		nRet = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		if (uParam)
		{
			UINT* pP = (UINT*)uParam;
			*pP = Player[CLIENT_PLAYER_INDEX].m_dwID;
		}
		break;

	case GDI_ITEM_TAKEN_WITH:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproom)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproom)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproom)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproom)
					nCount++;
			}
			nRet = nCount;
		}
		break;

	case GDI_EQUIPMENT:
		nRet = 0;
		if (uParam)
		{

			if (nParam == 1)
				break;

			int PartConvert[itempart_num] = 
			{
				UIEP_HEAD,		UIEP_BODY,
				UIEP_WAIST,		UIEP_HAND,
				UIEP_FOOT,		UIEP_FINESSE,
				UIEP_NECK,		UIEP_FINGER1,
				UIEP_FINGER2,	UIEP_WAIST_DECOR,
				UIEP_HORSE,		UIEP_MASK,	// mat na
				UIEP_FIFONG, 	UIEP_SIGNET,
				UIEP_SHIPIN,	UIEP_HOODS,
				UIEP_CLOAK,
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < itempart_num; i++)
			{
				pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(i);
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
				
					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = 0;
					pInfo->Region.v = PartConvert[i];
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;
	case GDI_EQUIPMENT_SETNUM:
		nRet = Player[CLIENT_PLAYER_INDEX].m_nActiveEquipNum;
		break;
	case GDI_PARADE_EQUIPMENT:
		nRet = 0;
		if (uParam)
		{

			if (nParam == 1)
				break;

			int PartConvert[itempart_num] = 
			{
				UIEP_HEAD,		UIEP_BODY,
				UIEP_WAIST,		UIEP_HAND,
				UIEP_FOOT,		UIEP_FINESSE,
				UIEP_NECK,		UIEP_FINGER1,
				UIEP_FINGER2,	UIEP_WAIST_DECOR,
				UIEP_HORSE,		UIEP_MASK,
				UIEP_FIFONG,	UIEP_SIGNET,
				UIEP_SHIPIN,	UIEP_HOODS,
				UIEP_CLOAK,
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < itempart_num; i++)
			{
				pInfo->Obj.uId = g_cViewItem.m_sItem[i].nIdx;
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
				
					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = 0;
					pInfo->Region.v = PartConvert[i];
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;
		
	case GDI_TRADE_OPER_DATA:
		if (uParam == UTOD_IS_LOCKED)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock;
		else if (uParam == UTOD_IS_TRADING)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState;
		else if (uParam == UTOD_IS_OTHER_LOCKED)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock;
		else if (uParam == UTOD_IS_WILLING)
		{
			nRet = (Player[CLIENT_PLAYER_INDEX].m_cMenuState.m_nState == PLAYER_MENU_STATE_TRADEOPEN);
		}
		break;
	case GDI_GAMBLE_OPER_DATA:
		if (uParam == UTOD_IS_LOCKED)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock;
		else if (uParam == UGOD_IS_GAMBLING)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState;
		else if (uParam == UTOD_IS_OTHER_LOCKED)
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock;

		break;

	case GDI_LIVE_SKILL_BASE:
		if (uParam)
		{
			KUiPlayerLiveSkillBase* pInfo = (KUiPlayerLiveSkillBase*)uParam;
			//to do:no implements in this version;
			pInfo->nLiveExperience = 0 ;
			pInfo->nRemainPoint = 0 ;
			pInfo->nLiveExperienceFull = 0 ;
			memset(pInfo,0,sizeof(KUiPlayerLiveSkillBase));
		}
		break;

	case GDI_LIVE_SKILLS:
		if (uParam)
		{
			//to do:no implements in this version;
			KUiSkillData* pInfo = (KUiSkillData*)uParam;
			memset(pInfo,0,sizeof(KUiSkillData)*10);
		}
		break;

	case GDI_FIGHT_SKILL_POINT:
		//to do:no implements in this version;
		nRet = Player[CLIENT_PLAYER_INDEX].m_nSkillPoint;
		break;

	case GDI_FIGHT_SKILLS:
		if (uParam)
		{
			KUiSkillData* pSkills = (KUiSkillData*)uParam;
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetSkillSortList(pSkills);
		}
		break;

	case GDI_LEFT_ENABLE_SKILLS:
		{
			KUiSkillData * pSkills = (KUiSkillData*)uParam;
			int nCount = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetLeftSkillSortList(pSkills);
			return nCount;
		}
		break;

	case GDI_RIGHT_ENABLE_SKILLS:
		{
			KUiSkillData * pSkills = (KUiSkillData*)uParam;
			int nCount = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetRightSkillSortList(pSkills);
			return nCount;
		}
		break;

	case GDI_CUSTOM_SKILLS:
		if (uParam)
		{
			//to do: no implements in this version;
			KUiSkillData* pSkills = (KUiSkillData*)uParam;
			memset(pSkills,0,sizeof(KUiSkillData)*5);
		}
		break;

	case GDI_NEARBY_PLAYER_LIST:
		nRet = NpcSet.GetAroundPlayer((KUiPlayerItem*)uParam, nParam);
		break;

	case GDI_ITEM_EQUIP_ROOM_LIST:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cAuto.AllItemNameEquipRoom((FKUiObjectName*)uParam, nParam);
		break;

	case GDI_NEARBY_IDLE_PLAYER_LIST:
		nRet = NpcSet.GetAroundPlayerForTeamInvite((KUiPlayerItem*)uParam, nParam);
		break;

	case GDI_PLAYER_LEADERSHIP:
		if (uParam)
		{
			KUiPlayerLeaderShip* pInfo = (KUiPlayerLeaderShip*)uParam ;
			pInfo->nLeaderShipExperience = Player[CLIENT_PLAYER_INDEX].m_dwLeadExp ;		
			//to do: waiting for...;
			pInfo->nLeaderShipExperienceFull = Player[CLIENT_PLAYER_INDEX].m_dwNextLevelLeadExp;
			pInfo->nLeaderShipLevel = Player[CLIENT_PLAYER_INDEX].m_dwLeadLevel ;			
		}
		break;

	case GDI_ITEM_IN_ENVIRO_PROP:
		{
			if (!nParam)
			{
				KUiGameObject *pObj = (KUiGameObject *)uParam;
				if (pObj->uGenre != CGOG_ITEM && pObj->uGenre != CGOG_NPCSELLITEM)
					break;

				KItem* pItem = NULL;

				if (pObj->uGenre == CGOG_ITEM && pObj->uId > 0 && pObj->uId < MAX_ITEM)
				{
					pItem = &Item[pObj->uId];
				}
				else if (pObj->uGenre == CGOG_NPCSELLITEM)
				{
					int nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop], pObj->uId);
					if(nIdx < 0) //#can kiem tra
						break;
					pItem = BuySell.GetItem(nIdx);
				}

				_ASSERT(pItem);
				if (!pItem || pItem->GetGenre() != item_equip)
					break;

				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CanEquip(pItem))
				{
					nRet = IIEP_NORMAL;
				}
				else
				{
					nRet = IIEP_NOT_USEABLE;
				}
			}
			else
			{
				KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
				if (pObj->Obj.uGenre != CGOG_ITEM || pObj->Obj.uId >= MAX_ITEM)
					break;

				int PartConvert[itempart_num] = 
				{ 
					itempart_head,		itempart_weapon,
					itempart_amulet,	itempart_cuff,
					itempart_body,		itempart_belt,
					itempart_ring1,		itempart_ring2,
					itempart_pendant,	itempart_foot,
					itempart_horse, itempart_mask,
					itempart_mantle,	itempart_signet,
					itempart_shipin,	itempart_hoods,
					itempart_cloak,
				};

				_ASSERT(pObj->eContainer < itempart_num);
				if (pObj->eContainer >= itempart_num || pObj->eContainer < 0)
					break;

				if (Item[pObj->Obj.uId].GetGenre() != item_equip)
					break;

				int nPlace = PartConvert[pObj->eContainer];

				if (Player[CLIENT_PLAYER_INDEX].m_ItemList.CanEquip(pObj->Obj.uId, nPlace))
				{
					nRet = IIEP_NORMAL;
				}
				else
				{
					nRet = IIEP_NOT_USEABLE;
				}
			}
		}
		break;

	case GDI_IMMEDIATEITEM_NUM:
		if (uParam >= 0 && uParam < IMMEDIACY_ROOM_WIDTH)
			nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetSameDetailItemNum(uParam);
		break;

	case GDI_TRADE_NPC_ITEM:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			int nPage = 0;
			int nIndex = 0;
			KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
			int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];

			if (nBuyIdx == -1)
				break;
			if (nBuyIdx >= BuySell.GetHeight())
				break;
			if (!BuySell.m_pShopRoom)
				break;
			BuySell.m_pShopRoom->Clear();
			for (int i = 0; i < BuySell.GetWidth(); i++)
			{
				nIndex = BuySell.GetItemIndex(nBuyIdx, i);
				KItem* pItem = BuySell.GetItem(nIndex);
				
				if (nIndex >= 0 && pItem)
				{
					// Set pInfo->Obj.uGenre
					pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
					// Set pInfo->Obj.uId
					pInfo->Obj.uId = i;

					POINT	Pos;
					if (BuySell.m_pShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos))
					{

						BuySell.m_pShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
					}
					else
					{
						nPage++;
						BuySell.m_pShopRoom->Clear();

						BuySell.m_pShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos);
						BuySell.m_pShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
					}
					pInfo->Region.h = Pos.x;
					pInfo->Region.v = Pos.y;
					pInfo->Region.Width = pItem->GetWidth();
					pInfo->Region.Height = pItem->GetHeight();
					pInfo->nContainer = nPage;
					nCount++;
					pInfo++;
				}
			}			
			nRet = nCount;
		}
		else
		{
			int nCount = 0;
			int nIndex = 0;
			int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];
			if (nBuyIdx == -1)
				break;
			if (nBuyIdx >= BuySell.GetHeight())
				break;
			for (int i = 0; i < BuySell.GetWidth(); i++)
			{
				nIndex = BuySell.GetItemIndex(nBuyIdx, i);
				KItem* pItem = BuySell.GetItem(nIndex);
				
				if (nIndex >= 0 && pItem)
				{
					nCount++;
				}
			}
			nRet = nCount;
		}
		break;
	case GDI_CHAT_SEND_CHANNEL_LIST:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.SendSelectChannelList((KUiChatChannel*)uParam, nParam);
		break;
	case GDI_CHAT_RECEIVE_CHANNEL_LIST:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.SendTakeChannelList((KUiChatChannel*)uParam, nParam);
		break;
	case GDI_CHAT_CURRENT_SEND_CHANNEL:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.GetCurChannel((KUiChatChannel*)uParam);
		break;
	case GDI_CHAT_GROUP_INFO:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.IGetTeamInfo(nParam, (KUiChatGroupInfo*)uParam);
		break;

	case GDI_CHAT_FRIENDS_IN_AGROUP:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cChat.IGetTeamFriendInfo(nParam, (KUiPlayerItem*)uParam);
		break;
	case GDI_ITEM_IN_STORE_BOX:
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_repository);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;

			nCount++;

			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_repositoryroom)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_repositoryroom)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_EX_BOX1:	// mo rong ruong 1
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_exbox1);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox1room)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox1room)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;

			nCount++;

			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox1room)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox1room)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_EX_BOX2:	// mo rong ruong 2
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_exbox2);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox2room)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox2room)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;

			nCount++;

			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox2room)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox2room)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_IN_EX_BOX3:	// mo rong ruong 3
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_exbox3);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox3room)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox3room)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;

			nCount++;

			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_exbox3room)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_exbox3room)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_ITEM_EX:	// mo rong hanh trang
		nRet = 0;
		if (uParam)
		{
			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			pInfo->Obj.uGenre = CGOG_MONEY;
			pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipmentex);
			nCount++;
			pInfo++;
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproomex)
			{
				pInfo->Obj.uGenre = CGOG_ITEM;
				pInfo->Obj.uId = pItem->nIdx;
				pInfo->Region.h = pItem->nX;
				pInfo->Region.v = pItem->nY;
				pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
				pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
				nCount++;
				pInfo++;
			}
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproomex)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
					pInfo->Obj.uId = pItem->nIdx;		
					pInfo->Region.h = pItem->nX;
					pInfo->Region.v = pItem->nY;
					pInfo->Region.Width = Item[pItem->nIdx].GetWidth();
					pInfo->Region.Height = Item[pItem->nIdx].GetHeight();
					nCount++;
					pInfo++;
				}
				if (nCount > nParam)
					break;
			}
			nRet = nCount;
		}
		else
		{
			int nCount = 0;

			nCount++;

			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == pos_equiproomex)
				nCount++;
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == pos_equiproomex)
					nCount++;
			}
			nRet = nCount;
		}
		break;
	case GDI_PK_SETTING:					
		nRet = Player[CLIENT_PLAYER_INDEX].m_cPK.GetNormalPKState();
		break;
	case GDI_SHOW_PLAYERS_NAME:			
		nRet = NpcSet.CheckShowName();
		break;
	case GDI_SHOW_PLAYERS_LIFE:			
		nRet = NpcSet.CheckShowLife();
		break;
	case GDI_SHOW_PLAYERS_MANA:			
		nRet = NpcSet.CheckShowMana();
		break;
	case GDI_SHOW_OBJ_NAME:            //hien ten obj edit by phong kieu
        nRet = ObjSet.CheckShowName();
		break;
	case GDI_IS_CHEST_UNLOCKED:			//Kiem tra xem nguoi choi da khoa ruong lai hay chua
		nRet = Player[CLIENT_PLAYER_INDEX].m_CUnlocked;
		break;
	case GDI_IS_CHECK_IMAGE:			//Kiem tra hinh anh nguoi choi da chon
		nRet = Player[CLIENT_PLAYER_INDEX].m_ImagePlayer;
		break;
	case GDI_ITEM_CHAT:			//tao ra itemchat
		{
			if(uParam)
			{
				if(nParam)
				{	
					ChatItem* m_sInfo = (ChatItem*)nParam;
					//Create ItemSet
					if(m_sInfo->m_nID == 0)
						return 0;
					int nMagicParam[MAX_ITEM_MAGICLEVEL], nItemIdx=0;
					for (int j = 0; j < MAX_ITEM_MAGICLEVEL; j++)
						nMagicParam[j] = m_sInfo->m_btMagicLevel[j];
					//
					if (m_sInfo->m_nNature >= NATURE_GOLD) {
						//IN int nItemNature, int nItemGenre, int nSeries,
						//int nLevel, int nLuck, int nDetailType/*=-1*/,
						//	int nParticularType/*=-1*/, int* pnMagicLevel, int nVersion/*=0*/, UINT nRandomSeed
						nItemIdx = ItemSet.Add(m_sInfo->m_nNature,
							m_sInfo->m_btGenre,
							m_sInfo->m_btSeries,
							m_sInfo->m_btLevel,
							m_sInfo->m_btLuck,
							m_sInfo->m_btDetail,
							m_sInfo->m_btParticur,
							nMagicParam,
							m_sInfo->m_wVersion,
							m_sInfo->m_dwRandomSeed);
					}
					else if (m_sInfo->m_nGoldId)
					{
							nItemIdx = ItemSet.AddGoldItem(
							m_sInfo->m_nGoldId,
							nMagicParam,
							m_sInfo->m_btSeries,
							m_sInfo->m_bEnChance
							);
					}
					else
					{
							nItemIdx = ItemSet.AddItemSet2(
							m_sInfo->m_btGenre,
							m_sInfo->m_btSeries,
							m_sInfo->m_btLevel,
							m_sInfo->m_btLuck,
							m_sInfo->m_btDetail,
							m_sInfo->m_btParticur,
							nMagicParam,
							m_sInfo->m_wVersion,
							m_sInfo->m_dwRandomSeed,
							m_sInfo->m_bStack,
							m_sInfo->m_bEnChance,
							m_sInfo->m_bPoint //®å tÝm sö dông ®iÓm nµy tèi ®a 6 dßng
							);						
					}
					if (nItemIdx > 0)
					{
						Item[nItemIdx].SetID(m_sInfo->m_nID);
						Item[nItemIdx].SetPrice(m_sInfo->m_uPrice);
						Item[nItemIdx].SetExpTime(m_sInfo->m_YearExp,0,0,0);
						Item[nItemIdx].SetPlayerItemLock(m_sInfo->m_Lock);
						Item[nItemIdx].SetPlayerItemHLock(m_sInfo->m_HLock);
						Item[nItemIdx].SetDurability(m_sInfo->m_nDurability);
						Item[nItemIdx].SetMaxOptMultiply(m_sInfo->m_nMaxOptMultiply);
						nRet = nItemIdx;
					}
				}
			}
			else
			{
				ItemSet.Remove(nParam);
			}
		}
		break;
	case GDI_ITEM_NAME:			//lay ten item
		if(uParam)
		{
			char* pzName = (char*)uParam;
			if(nParam)
				strcpy(pzName, Item[nParam].GetName());
		}
		break;
	case GDI_GET_ITEM_PARAM:			//lay thong so cua item
		if(uParam && nParam)
		{
			DWORD nIdx = nParam;
			ChatItem *pInfo = (ChatItem *)uParam;
			pInfo->m_uPrice = 0;
			pInfo->m_bX = 0;
			pInfo->m_bY = 0;
			pInfo->m_nIdx = 0;
			pInfo->m_nID			= Item[nIdx].GetID();
			pInfo->m_btGenre		= Item[nIdx].GetGenre();
			if(Item[nIdx].GetNature() >= NATURE_GOLD && pInfo->m_btDetail != 99) //99 is flag indicate that called from SouldPlay
				pInfo->m_btDetail = Item[nIdx].GetRow();
			else
				pInfo->m_btDetail = Item[nIdx].GetDetailType();
			pInfo->m_btParticur		= Item[nIdx].GetParticular();
			pInfo->m_btSeries		= Item[nIdx].GetSeries();
			pInfo->m_btLevel		= Item[nIdx].GetLevel();
			pInfo->m_bStack			= Item[nIdx].GetStackNum();
			pInfo->m_bEnChance		= Item[nIdx].GetEnChance();
			pInfo->m_nGoldId		= Item[nIdx].GetGoldId();
			pInfo->m_btLuck			= Item[nIdx].m_GeneratorParam.nLuck;
			pInfo->m_dwRandomSeed	= Item[nIdx].m_GeneratorParam.uRandomSeed;
			pInfo->m_wVersion		= Item[nIdx].m_GeneratorParam.nVersion;
			pInfo->m_YearExp		= Item[nIdx].GetTime()->bYear;
			pInfo->m_Lock			= Item[nIdx].GetPlayerItemLock();
			pInfo->m_HLock			= Item[nIdx].GetPlayerItemHLock();
			pInfo->m_nNature		= Item[nIdx].GetNature();
			pInfo->m_nMaxOptMultiply = Item[nIdx].GetMaxOptMultiply();
			memset(pInfo->m_btMagicLevel, 0, sizeof(pInfo->m_btMagicLevel));
			for (int i = 0; i < MAX_ITEM_MAGICLEVEL; i++)
			{
				if (Item[nIdx].m_GeneratorParam.nGeneratorLevel[i] > 10) {
					pInfo->m_btMagicLevel[i] = Item[nIdx].m_GeneratorParam.nGeneratorLevel[i];
					pInfo->m_btMagicLevel[i+ MAX_ITEM_MAGICATTRIB] = (short)Item[nIdx].m_aryMagicAttrib[i].nValue[0];
				}
				else if (pInfo->m_btMagicLevel[i] == 0) //not set
					pInfo->m_btMagicLevel[i] = Item[nIdx].m_GeneratorParam.nGeneratorLevel[i];
			}
			pInfo->m_bPoint = Item[nIdx].IsPurple();
			pInfo->m_nDurability = Item[nIdx].GetDurability();
			nRet = 1;
		}
		break;
	case GDI_CHAT_ITEM_DESC:
		if (nParam && uParam)
		{
			char* pszDescript = (char *)nParam;
			pszDescript[0] = 0;
			Item[uParam].GetDesc(pszDescript, false, false);
		}
		break;
	case GDI_IS_TONG_MASTER:
		if (uParam)
		{
			char* pszPlayerName = (char *)uParam;
			Player[CLIENT_PLAYER_INDEX].m_cTong.GetTongMasterName(pszPlayerName);
		}
		break;
	case GDI_MISSION_SELFDATA: //add by phong kiÒu Tèng Kim Battle
		if (uParam)
		{
			TMissionLadderSelfInfo *pRankData = (TMissionLadderSelfInfo *)uParam;
			*pRankData = Player[CLIENT_PLAYER_INDEX].m_MissionData;
		}
		break;	
	case GDI_MISSION_RANKDATA:
		if (uParam)
		{
			TMissionLadderInfo *pRankData = (TMissionLadderInfo *)uParam;
			*pRankData = Player[CLIENT_PLAYER_INDEX].m_MissionRank[nParam];
		}
		break;	
	case GDI_TREMBLE_ITEM: 
		nRet = 0;
		if (uParam)
		{
			if (nParam == 1)
				break;

			int PartTrembleConvert[tremblepart_num] = 
			{
				UIEP_BLUEITEM,	UIEP_GEMLEVEL,
				UIEP_GEMSPIRIT,	UIEP_GEMMETAL,
				UIEP_GEMWOOD,	UIEP_GEMWATER,
				UIEP_GEMFIRE,	UIEP_GEMEARTH,
			};

			int nCount = 0;
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;

			for (int i = 0; i < tremblepart_num; i++)
			{
				pInfo->Obj.uId = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetTrembleItem(i);
				if (pInfo->Obj.uId)
				{
					pInfo->Obj.uGenre = CGOG_ITEM;
				
					pInfo->Region.Width = Item[pInfo->Obj.uId].GetWidth();
					pInfo->Region.Height = Item[pInfo->Obj.uId].GetHeight();
					pInfo->Region.h = 0;
					pInfo->Region.v = PartTrembleConvert[i];
				}
				else
				{
					pInfo->Obj.uGenre = CGOG_NOTHING;
				}
				nCount++;
				pInfo++;
			}
			nRet = nCount;
		}
		break;
	case GDI_CHAT_ITEM_IMAGE:
		if (nParam && uParam)
		{
			char* pszImage = (char *)uParam;
			pszImage[0] = 0;
			strcpy(pszImage, Item[nParam].GetImageName());
			if(pszImage[0]) 
				nRet = 1;
		}
		break;
	case GDI_NPC_STATE_SKILL:
		nRet = 0;
		if (uParam)
		{
			KStateNode* pNode;
			pNode = (KStateNode *)Npc[CLIENT_PLAYER_INDEX].m_StateSkillList.GetTail();
			KStateTempNode* pInfo = (KStateTempNode *)uParam;
			
			while(pNode)
			{
				if (pNode->m_SkillID > 0 && pNode->m_SkillID < MAX_SKILL && pNode->m_Level > 0 && pNode->m_Level < MAX_SKILLLEVEL)
				{
					KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(pNode->m_SkillID, pNode->m_Level);
					if (pOrdinSkill)
					{
						if(pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles || pOrdinSkill->GetSkillStyle() == SKILL_SS_InitiativeNpcState)
						{
							pInfo->nSkillId = pNode->m_SkillID;
							pInfo->nLeftTime = pNode->m_LeftTime;
							pInfo++;
						}
					}
				}
				pNode = (KStateNode *)pNode->GetPrev();
			}	
		}
		else
		{
			int nCount = 0;
			KStateNode* pNode;
			pNode = (KStateNode *)Npc[CLIENT_PLAYER_INDEX].m_StateSkillList.GetTail();
			while(pNode)
			{
				if (pNode->m_SkillID > 0 && pNode->m_SkillID < MAX_SKILL && pNode->m_Level > 0 && pNode->m_Level < MAX_SKILLLEVEL)
				{
					KSkill * pOrdinSkill = (KSkill *)g_SkillManager.GetSkill(pNode->m_SkillID, pNode->m_Level);
					if (pOrdinSkill)
					{
						if(pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles || pOrdinSkill->GetSkillStyle() == SKILL_SS_InitiativeNpcState)
						{
							nCount++;
						}
					}
				}
				pNode = (KStateNode *)pNode->GetPrev();
			}
			nRet = nCount;
		}
		break;	
	case GDI_EXBOX_ID: 				// truyen id mo rong ruong
		{	
			int nExBoxId = 0;
			if (nExBoxId < 0)
				return 0;
			nExBoxId = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
				return nExBoxId;
			break;
		}
#ifndef _SERVER
	case NPC_OI_TARGET_INFO:
	{
		int idx = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx;
		int idx_hover = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
		if (idx_hover)
			idx = idx_hover;
		if (idx) {
			//having target
			KUiTargetDetailInfo* pTargetInfo = (KUiTargetDetailInfo*)uParam;
			strcpy_s(pTargetInfo->sTargetName, sizeof(pTargetInfo->sTargetName), Npc[idx].Name);
			pTargetInfo->Series = Npc[idx].m_Series;
			pTargetInfo->nLifePercent = (Npc[idx].m_CurrentLife * 100 / max(Npc[idx].m_CurrentLifeMax, 1));
			nRet = 1;
		}
		break;
	}
	case GDI_PLAYER_MERIDIAN:
	{
		BYTE* tmp = Player[CLIENT_PLAYER_INDEX].m_cMeridian.getMeridian();
		memcpy((void *)uParam, tmp, nParam);
		nRet = 1;
		break;
	}
	case GDI_ITEM_EQUIP_SAME_GERNE:
	{
		int itemIdx = nParam;
		int i = 0;
		nRet = -1;
		for (i = 0; i < itempart_num; i++)
		{
			if (Item[itemIdx].GetGenre() == item_equip) {
				if (Item[itemIdx].GetDetailType() == Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(i)].GetDetailType()) {
					nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(i);
					break;
				}
			}
		}
		break;
	}
	}
#endif
	return nRet;
}

// PaintFps interpolation state: paint clock runs faster than the 18-fps logic clock.
// GOI_PROCFRAME_BREATHE (once per logic tick) writes these, GOI_PROCFRAME_POSSHIFT
// (once per paint frame) reads them. Client only - this file is excluded from server builds.
#define	PAINT_INTERP_SNAP_DIST	64	// pixels per tick; any bigger jump is a teleport, never interpolated
#define	MISSLE_INTERP_SNAP_DIST	512	// missiles fly far per tick; only a real respawn/retarget exceeds this
static POINT	s_InterpFrom[MAX_NPC];
static POINT	s_InterpTo[MAX_NPC];
static DWORD	s_InterpNpcID[MAX_NPC];
static BYTE	s_InterpValid[MAX_NPC];	// 1 = snapshot hop le (ClientOnly npc co m_dwID = 0 van hop le)
BOOL	g_bPaintInterpFocus = FALSE;
// Probe gia cap Lock/Unlock surface - dinh nghia trong Engine (KCanvas.cpp).
// Khai bao thang (dung ENGINE_API tu KWin32.h, KHONG extern "C" de khop ten
// C++ da mangle) thay vi include KCanvas.h - tranh keo DirectDraw vao Core.
ENGINE_API void g_SetCanvasLockProbe(int nOn);
ENGINE_API void g_GetCanvasLockStats(unsigned int* pnCount, unsigned int* pnMicroSec, int bReset);
int	g_nCorePaintLog = 0;	// mirror of [Client] PaintLog for Core-side probes (set via GOI_PROCFRAME_BREATHE nParam)	// TRUE = POSSHIFT drives the camera each paint frame; the logic tick must not touch the focus

// ==================== AUTO DA TAU (18/08/2026) ====================
// May trang thai lam nhiem vu Da Tau, dieu khien bang NOI DUNG HOI THOAI
// (client KHONG doc duoc task value >=256 - TASK_VALUE_SYNC.nTaskId la BYTE).
// Du lieu bang + marker TCVN3: KDaTauTables.h (sinh boi gen_datau_tables.py).
// Hook chup goi tin: KDaTauCap.h (ghi tu KPlayer.cpp / KProtocolProcess.cpp).
// Duoc goi moi tick tu ExtAutoLoop (S3Client.cpp) qua ATYPE_DATAU.
// Tra ve: 0 = tha may (auto thuong chay); 1 = dang lam viec o thanh
// (ExtAutoLoop bo MOVE/RETURN); 2 = dang farm map nhiem vu (bo MOVE/RETURN,
// bo qua bSkipGoldboss vi Mat Chi chi roi tu boss).

enum DTPHASE
{
	DTP_IDLE = 0,
	DTP_GOTONPC,		// di den NPC Da Tau (template 108) + mo thoai
	DTP_WAITDLG,		// doi va phan loai hoi thoai
	DTP_EXEC,			// quyet dinh viec theo loai nhiem vu da parse
	DTP_GOSHOP,			// T1: di den tiem tap hoa
	DTP_SHOPTALK,		// T1: mo thoai tiem + chon giao dich
	DTP_BUY,			// T1: mua item trong cua so shop
	DTP_GOXAFU,			// T4: di den xa phu
	DTP_XAFUTALK_DONE,	// T4: da chon godatau, cho chuyen map
	DTP_FARM,			// T4: danh quai nhat cuon (engaged=2) / T5exp: tha may cay exp
	DTP_RETURN,			// ve thanh Da Tau (Tho Dia Phu)
	DTP_GIVEBOX,		// T1/2/3: dat item vao o nop + OK
	DTP_REWARD,			// bam ruong thuong
	DTP_USEPD,			// T5 phuc duyen: dung item -> thu tra -> lap
	DTP_HOLD,			// treo (ket / loai tat / du 40 / loi) - tha may
	DTP_SELLJUNK,		// tui day: (20/08) TU ban rac tai cho, xong lam tiep
	DTP_MUASAP,			// (20/08) T2 thieu do: di cho xem sap nguoi ban (that + bot) de mua
	DTP_CITYHOP,		// (20/08) dang nho Xa Phu cho qua thanh khac tim sap
	DTP_YIELD,			// (21/08) bi dua ve thanh giua chung / het rac: nhuong may cho Hau can don xong roi lam tiep
};

// y dinh hien tai khi mo thoai NPC Da Tau (ea.nDTStep)
enum DTINTENT
{
	DTI_NONE = 0,		// gap NPC binh thuong (nhan/xem nhiem vu)
	DTI_TURNIN,			// den de tra nhiem vu (chon option 2)
	DTI_CANCEL,			// den de huy nhiem vu (chon option 3)
	DTI_CANCELWAIT,		// da chon huy, cho hop xac nhan
	DTI_TURNWAIT,		// da chon tra, cho ket qua (give-box / thuong / fail)
};

static const char* DT_FIN3[3] = { "finish_exp", "finish_money", "quest_random" };
static const char* DT_FIN4[3] = { "finish_point", "finish_lucky", "finish_item" };
static double g_dDTPrevExp = -1.0;	// theo doi exp lan truoc (T5 exp; 1 client 1 nhan vat)
static double g_dDTExpGain = 0.0;	// exp tich luy duoc tu luc nhan T5 exp

static void DT_Msg(int nPlayerIdx, const char* szMsg)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	UINT uNow = timeGetTime();
	if (ea.uDTStatusTime > uNow)
		return;
	ea.uDTStatusTime = uNow + 1200;
	try
	{
		l_pDataChangedNotifyFunc->ChannelMessageArrival(0, "[D· TÈu]", (char*)szMsg, strlen(szMsg), TRUE);
	}
	catch (...) {}
}

static int DT_Today()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	return ((int)st.wYear % 100) * 10000 + (int)st.wMonth * 100 + (int)st.wDay;
}

static bool DT_Has(const char* s, const char* m)
{
	return s && m && m[0] && strstr(s, m) != NULL;
}

// khop ten item/tiem/map co ranh gioi: ky tu ngay truoc ten phai la '>' (het the <color=yellow>)
// -> tranh ten ngan trung khop vao ten dai hon (VD "Gioi Chi (Kim)" trong "Hoang Ngoc Gioi Chi (Kim)")
static bool DT_HasName(const char* s, const char* m)
{
	if (!s || !m || !m[0])
		return false;
	size_t nLen = strlen(m);
	const char* p = s;
	while ((p = strstr(p, m)) != NULL)
	{
		if (p > s && *(p - 1) == '>')
			return true;
		p += nLen;
	}
	return false;
}

// so nguyen dau tien sau marker (bo qua the <color=...>)
static int DT_NumAfter(const char* s, const char* mark)
{
	const char* p = strstr(s, mark);
	if (!p)
		return -1;
	p += strlen(mark);
	while (*p && (*p < '0' || *p > '9'))
		++p;
	if (!*p)
		return -1;
	return atoi(p);
}

// tach "cau hoi|a1|a2..." TAI CHO, tra so dap an; szBuf tro cau hoi
static int DT_Split(char* szBuf, char* apAns[], int nMax)
{
	int n = 0;
	char* p = strstr(szBuf, "|");
	while (p && n < nMax)
	{
		*p++ = 0;
		apAns[n++] = p;
		p = strstr(p, "|");
	}
	return n;
}

static int DT_FindAns(char* apAns[], int nAns, const char* szMark)
{
	for (int i = 0; i < nAns; ++i)
		if (strstr(apAns[i], szMark))
			return i;
	return -1;
}

static int DT_Hold(int nPlayerIdx, const char* szWhy, UINT uCurTime, UINT uMs)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	char szNghi[512];
	ea.uDTStatusTime = 0;
	ea.nDTPhaseBack = 0;	// (r5d) treo = bo tour, dung quay lai pha di cho cu
	// treo co thoi han -> tu ghi chu "(tam nghi N phut)" mau xam cuoi cau
	if (uMs >= 60000u && strlen(szWhy) < 440)
	{
		sprintf(szNghi, "%s <color=Gray>(t¹m nghØ %u phót)", szWhy, uMs / 60000u);
		DT_Msg(nPlayerIdx, szNghi);
	}
	else
		DT_Msg(nPlayerIdx, szWhy);
	ea.nDTPhase = DTP_HOLD;
	ea.uDTHoldUntil = uMs ? (uCurTime + uMs) : 0;
	ea.nDTHoldFreeze = 0;	// hold loi/treo: NHA MAY cho auto thuong chay tiep
	ea.nDTBackXaFu = 0;	// (r4 - PB R6) hold = mach gay - xoa co "ve chi de di lai Xa Phu"
	ea.nDTEngaged = 0;
	return 0;
}

// tim NPC kind_dialoger theo template (nRadius mps; 0 = ca vung nhin thay)
static int DT_FindNpcTpl(int nPlayerIdx, int nTpl, int nRadius)
{
	int nX, nY, dX, dY;
	Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nX, &nY);
	int nIdx = 0;
	int nFirst = 0;
	while (nIdx = NpcSet.GetNextIdx(nIdx))
	{
		if (Npc[nIdx].m_Kind != kind_dialoger)
			continue;
		if (Npc[nIdx].m_RegionIndex < 0)
			continue;
		if (Npc[nIdx].m_NpcSettingIdx != nTpl)
			continue;
		Npc[nIdx].GetMpsPos(&dX, &dY);
		if (nRadius > 0 && g_GetDistance(nX, nY, dX, dY) > nRadius)
			continue;
		if (strcmp(Npc[nIdx].Name, "D· TÈu") == 0)
			return nIdx;	// dung ten 'Da Tau' (TCVN3) - chac chan nhat
		if (!nFirst)
			nFirst = nIdx;
	}
	return nFirst;
}

// tim NPC kind_dialoger co ten (thuong hoa ASCII) chua chuoi con, gan (nX,nY)
static int DT_FindNpcName(int nPlayerIdx, const char* szSub, int nAtX, int nAtY, int nRadius)
{
	int dX, dY;
	char szBuff[36];
	int nIdx = 0;
	while (nIdx = NpcSet.GetNextIdx(nIdx))
	{
		if (Npc[nIdx].m_Kind != kind_dialoger)
			continue;
		if (Npc[nIdx].m_RegionIndex < 0)
			continue;
		Npc[nIdx].GetMpsPos(&dX, &dY);
		if (g_GetDistance(nAtX, nAtY, dX, dY) > nRadius)
			continue;
		g_StrCpyLen(szBuff, Npc[nIdx].Name, sizeof(szBuff));
		g_StrLower(szBuff);
		if (strstr(szBuff, szSub))
			return nIdx;
	}
	return 0;
}

// [DaTau] len ngua khi di duong (khuon dung nguyen case PA_RIDE - CoreShell.cpp:9614:
// phai co ngua o o trang bi, khong dang ngoi, va het gian TIME_RIDE).
static void DT_Ride(int nPlayerIdx)
{
	const int nSelf = Player[nPlayerIdx].m_nIndex;
	if (nSelf <= 0)
		return;
	if (Npc[nSelf].m_bRideHorse)
		return;
	if (Player[nPlayerIdx].m_ItemList.GetEquipment(itempart_horse) <= 0)
		return;
	if (Npc[nSelf].m_Doing == do_sit)
		return;
	if (GetTickCount() - Npc[nSelf].m_TimeHorse < TIME_RIDE)
		return;
	Npc[nSelf].m_TimeHorse = GetTickCount();
	SendClientCmdRide(FALSE);
}

// di bo trong map; tra 1 khi da toi gan (nNear mps)
static int DT_WalkTo(int nPlayerIdx, int nX, int nY, int nNear, UINT uCurTime)
{
	int px, py;
	Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&px, &py);
	if (g_GetDistance(px, py, nX, nY) <= nNear)
		return 1;
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	// di duong (ve thanh tra nhiem vu / toi NPC / toi tiem / toi Xa Phu) thi LEN NGUA;
	// rieng pha farm thi khong - dang danh quai (yeu cau chu game 19/08).
	if (ea.nDTPhase != DTP_FARM)
		DT_Ride(nPlayerIdx);
	if (ea.uDTPath < uCurTime)
	{
		ea.uDTPath = uCurTime + 2500;
		// HaveTarget(int&,int&) XUAT target hien tai ra tham so - dua thang dich den vao
		// se bi ghi de thanh (0,0) => FindPath tu choi => dung yen tai cho.
		// Lam nhu ATYPE_MOVE (bAroundPoint): hung ra bien nhap, so voi dich roi moi path.
		int tx = 0, ty = 0;
		if (!SubWorld[0].HaveTarget(tx, ty) || tx != nX || ty != nY)
		{
			g_ScenePlace.RemoveFlag();
			SubWorld[0].FindPath(nX, nY);
		}
	}
	return 0;
}

//---------------------------------------------------------------------------
// [TaskGuide] Bam vao nhiem vu loai 4 (Dia do chi / Mat chi) tren bang Chi nam
// nhiem vu (F11) -> tu chay den NPC Xa Phu roi mo thoai; nguoi choi chon muc
// 'Den noi lam nhiem vu da tau' (godatau) de len map. Chay DOC LAP voi engine
// WAuto (tick tu KCoreShell::Breathe, client-only); dung lai do nghe da kiem
// chung cua engine Da Tau: g_MoveStation / DT_FindNpcName / DT_WalkTo / DialogNpc.
//---------------------------------------------------------------------------
static void DT_Answer(int nPlayerIdx, int nIdx);	// dinh nghia o duoi (dung chung voi engine)

static int	g_nTGXaFuOn = 0;
static UINT	g_uTGXaFuNext = 0;
static int	g_nTGXaFuTry = 0;
static int	g_nTGXaFuMap = 0;
static int	g_nTGXaFuPhase = 0;		// 1 = dang di, 2 = da mo thoai - cho chon godatau
static UINT	g_uTGXaFuDlgSeen = 0;	// uDlgSeq cua g_sDTCap da xem

static void TG_XaFuStop(const char* szMsg)
{
	if (g_nTGXaFuOn && szMsg)
		DT_Msg(CLIENT_PLAYER_INDEX, szMsg);
	g_nTGXaFuOn = 0;
}

// bam lan dau = bat dan duong; dang chay bam lai = huy. Tra 1 neu vua bat.
static int TG_XaFuStart()
{
	int nPlayerIdx = CLIENT_PLAYER_INDEX;
	if (g_nTGXaFuOn)
	{
		TG_XaFuStop("<color=Cyan>[ChØ nam] §· hñy dÉn ®­êng.");
		return 0;
	}
	if (Player[nPlayerIdx].m_nIndex <= 0)
		return 0;
	if (Player[nPlayerIdx].m_sExtAuto.nDTEngaged)
	{
		DT_Msg(nPlayerIdx, "<color=Yellow>[ChØ nam] Auto D· TÈu ®ang ch¹y - ®Ó auto tù lo viÖc di chuyÓn.");
		return 0;
	}
	int nMap = SubWorld[0].m_SubWorldID;
	MapStation::iterator it = g_MoveStation.find(nMap);
	if (it == g_MoveStation.end() || it->second.empty())
	{
		DT_Msg(nPlayerIdx, "<color=Yellow>[ChØ nam] Kh«ng t×m thÊy Xa Phu ë thµnh nµy.");
		return 0;
	}
	g_nTGXaFuOn = 1;
	g_nTGXaFuMap = nMap;
	g_nTGXaFuTry = 0;
	g_uTGXaFuNext = 0;
	g_nTGXaFuPhase = 1;
	g_uTGXaFuDlgSeen = g_sDTCap.uDlgSeq;
	DT_Msg(nPlayerIdx, "<color=Cyan>[ChØ nam] §ang tù ch¹y ®Õn Xa Phu - bÊm l¹i vµo dßng nhiÖm vô ®Ó hñy.");
	return 1;
}

static void TG_XaFuTick()
{
	if (!g_nTGXaFuOn)
		return;
	int nPlayerIdx = CLIENT_PLAYER_INDEX;
	if (Player[nPlayerIdx].m_nIndex <= 0)
	{
		g_nTGXaFuOn = 0;
		return;
	}
	UINT uCur = timeGetTime();
	if (uCur < g_uTGXaFuNext)
		return;
	g_uTGXaFuNext = uCur + 400;
	// da chuyen map (Xa Phu cho di / nguoi choi tu di) -> xong viec, tat im lang
	if (SubWorld[0].m_SubWorldID != g_nTGXaFuMap)
	{
		g_nTGXaFuOn = 0;
		return;
	}
	if (++g_nTGXaFuTry > 90)	// ~36 giay
	{
		TG_XaFuStop("<color=Yellow>[ChØ nam] §i qu¸ l©u - dõng dÉn ®­êng.");
		return;
	}
	MapStation::iterator it = g_MoveStation.find(g_nTGXaFuMap);
	if (it == g_MoveStation.end() || it->second.empty())
	{
		TG_XaFuStop("<color=Yellow>[ChØ nam] Kh«ng t×m thÊy Xa Phu ë thµnh nµy.");
		return;
	}
	sStation& s = it->second[0];
	int nIdx = DT_FindNpcName(nPlayerIdx, "xa phu", s.x, s.y, 400);
	if (nIdx)
	{
		int nX, nY, dX, dY;
		Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nX, &nY);
		Npc[nIdx].GetMpsPos(&dX, &dY);
		if (g_GetDistance(nX, nY, dX, dY) <= 128)
		{
			if (g_nTGXaFuPhase == 1)
			{
				// vua den noi: mo thoai va chuyen sang pha tu chon muc di map
				g_nTGXaFuPhase = 2;
				g_nTGXaFuTry = 0;
				g_uTGXaFuDlgSeen = g_sDTCap.uDlgSeq;
				Player[nPlayerIdx].DialogNpc(nIdx);
				DT_Msg(nPlayerIdx, "<color=Cyan>[ChØ nam] §· gÆp Xa Phu - ®ang tù chän môc ®i map nhiÖm vô...");
				return;
			}
			// pha 2: doi thoai Xa Phu ve roi tu chon muc 'Den noi lam nhiem vu da tau'
			if (g_sDTCap.uDlgSeq != g_uTGXaFuDlgSeen)
			{
				g_uTGXaFuDlgSeen = g_sDTCap.uDlgSeq;
				char szBuf[2048];
				char* apAns[16];
				g_StrCpyLen(szBuf, g_sDTCap.szDlg, sizeof(szBuf));
				int nAns = DT_Split(szBuf, apAns, 16);
				int nOpt = DT_FindAns(apAns, nAns, DTM_OPT_GODATAU);
				if (nOpt >= 0)
				{
					DT_Answer(nPlayerIdx, nOpt);
					TG_XaFuStop("<color=Cyan>[ChØ nam] §· chän môc ®i tíi map nhiÖm vô - chê chuyÓn map.");
					return;
				}
				TG_XaFuStop("<color=Yellow>[ChØ nam] Tho¹i kh«ng cã môc ®i map - h·y tù chän trong khung tho¹i.");
				return;
			}
			if ((g_nTGXaFuTry % 8) == 0)
				Player[nPlayerIdx].DialogNpc(nIdx);	// ~3.2s chua co thoai -> go lai
			return;
		}
		if (g_nTGXaFuPhase == 2)
			g_nTGXaFuPhase = 1;	// bi keo ra xa khi dang cho thoai -> di lai
		DT_WalkTo(nPlayerIdx, dX, dY, 96, uCur);
		return;
	}
	DT_WalkTo(nPlayerIdx, s.x, s.y, 200, uCur);
}

// [DaTau] tim quai con SONG da sync NGOAI tam danh de chay toi (T4 di tim quai).
// Client chi thay quai da sync (~2 man hinh); ngoai tam nay DTP_FARM dao 8 huong quanh neo.
static int DT_FindFarMob(int nPlayerIdx, const autoData* pAp, int* pnX, int* pnY)
{
	int nSelf = Player[nPlayerIdx].m_nIndex;
	int px, py;
	Npc[nSelf].GetMpsPos(&px, &py);
	int nVision = pAp->nVision;
	if (nVision < 100)
		nVision = 100;
	else if (nVision > 1200)
		nVision = 1200;
	int nBest = 0, nBestDist = 0x7fffffff;
	int nIdx = 0;
	while (nIdx = NpcSet.GetNextIdx(nIdx))
	{
		if (nIdx == nSelf)
			continue;
		if (Npc[nIdx].m_Kind == kind_player || Npc[nIdx].m_Kind == kind_dialoger)
			continue;
		if (Npc[nIdx].m_RegionIndex < 0)
			continue;
		if (Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)
			continue;
		if (!(NpcSet.GetRelation(nSelf, nIdx) == relation_enemy))
			continue;
		int ex, ey;
		Npc[nIdx].GetMpsPos(&ex, &ey);
		int nDist = g_GetDistance(px, py, ex, ey);
		if (nDist <= nVision)
			continue;	// gan roi - FIGHT tu bat
		if (nDist < nBestDist)
		{
			nBestDist = nDist;
			nBest = nIdx;
		}
	}
	if (!nBest)
		return 0;
	Npc[nBest].GetMpsPos(pnX, pnY);
	return 1;
}

// [DaTau] BANG TOA DO QUAI CAC MAP NHIEM VU - nap tu tep van ban luc chay.
// Tep: <thu muc game>\settings\datau_toado.txt, moi dong "MapID X Y SoQuai"
// (X/Y = MPS tuyet doi), dong bat dau bang '#' hoac ';' la chu thich. Sinh boi
// ReverseTools/gen_datau_spots.py doc file add NPC cua may chu trong pak.
// Sua tep la doi duoc cho danh quai - KHONG phai dung lai DLL. Thieu tep thi
// dung bang nhung san trong KDaTauSpots.h (chi 14 map dang bat).
// 204 map x 24 cum = 4896 dong nen 4096 la THIEU (bi cat am tham). De 8192.
#define DT_MAX_SPOTFILE 8192
static DTSpotRow g_aDTSpotFile[DT_MAX_SPOTFILE];
static int g_nDTSpotFile = -1;	// -1 = chua nap lan nao

static void DT_LoadSpotFile()
{
	if (g_nDTSpotFile >= 0)
		return;				// da nap (hoac da biet la khong co tep)
	g_nDTSpotFile = 0;
	FILE* f = fopen("settings\\datau_toado.txt", "rb");
	if (!f)
		return;
	char szD[256];
	while (g_nDTSpotFile < DT_MAX_SPOTFILE && fgets(szD, sizeof(szD), f))
	{
		const char* s = szD;
		// Notepad Windows luu UTF-8 co dau BOM EF BB BF - bo qua cho khoi hong dong dau
		if ((unsigned char)s[0] == 0xEF && (unsigned char)s[1] == 0xBB
		 && (unsigned char)s[2] == 0xBF)
			s += 3;
		while (*s == ' ' || *s == '\t')
			++s;
		if (*s == '#' || *s == ';' || *s == '\r' || *s == '\n' || *s == 0)
			continue;
		int nMap = 0, nX = 0, nY = 0, nNum = 0;
		if (sscanf(s, "%d %d %d %d", &nMap, &nX, &nY, &nNum) < 3)
			continue;
		if (nMap <= 0 || nX <= 0 || nY <= 0)
			continue;
		g_aDTSpotFile[g_nDTSpotFile].nMapId = nMap;
		g_aDTSpotFile[g_nDTSpotFile].nX = nX;
		g_aDTSpotFile[g_nDTSpotFile].nY = nY;
		g_aDTSpotFile[g_nDTSpotFile].nNum = nNum;
		++g_nDTSpotFile;
	}
	if (g_nDTSpotFile >= DT_MAX_SPOTFILE)
		g_DebugLog("[DaTau] datau_toado.txt qua %d dong - da cat bot, tang DT_MAX_SPOTFILE\n",
		           DT_MAX_SPOTFILE);
	fclose(f);
}

// [DaTau] cum quai THAT cua map nhiem vu (KDaTauSpots.h - sinh tu file add NPC
// cua may chu trong pak). Tra so cum cua map va dien toa do cum thu nIdx.
// VI SAO CAN: neo nhiem vu chi la cho Xa Phu tha xuong - o map 53/80/226 no nam
// NGOAI han vung co quai, dao quanh neo la khong bao gio gap quai.
static int DT_SpotOf(int nMap, int nIdx, int* pnX, int* pnY)
{
	DT_LoadSpotFile();
	const DTSpotRow* pBang = (g_nDTSpotFile > 0) ? g_aDTSpotFile : g_DTSpot;
	const int nTong = (g_nDTSpotFile > 0) ? g_nDTSpotFile : g_nDTSpotCount;
	if (nIdx < 0)
		nIdx = -nIdx;
	int nNum = 0;
	int i;
	for (i = 0; i < nTong; ++i)
		if (pBang[i].nMapId == nMap)
			++nNum;
	if (nNum <= 0)
		return 0;
	int nCan = nIdx % nNum;
	for (i = 0; i < nTong; ++i)
	{
		if (pBang[i].nMapId != nMap)
			continue;
		if (nCan-- > 0)
			continue;
		*pnX = pBang[i].nX;
		*pnY = pBang[i].nY;
		return nNum;
	}
	return 0;
}

// tra loi hoi thoai dang mo theo index 0-based (dong khung roi gui - nhu luong mua thuoc)
static void DT_Answer(int nPlayerIdx, int nIdx)
{
	if (g_bUISelLastSelCount == 0)
		return;
	CoreDataChanged(GDCNI_UI_ACT, 1, 0);
	PLAYER_SELECTUI_COMMAND command;
	command.nSelectIndex = nIdx;
	Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_SELECTDIALOG);
}

// item khop luat: magic>0 -> (G/D/P neu >=0) + magic co value trong [mn,mx];
// magic==0 -> 5 truong chinh xac (lvl/five == -1 nghia la bo qua)
// (r5 - nguoi dung bao) tra nhiem vu TRUOT: server chi Say loi, KHONG dong hop
// giao -> mon da bo vao hop nam ket o pos_affairitem, moi bo quet tui/ruong
// khong thay nen auto tuong thieu do va DI MUA THEM. Ham nay gui goi
// RECOVERY_BOX (dung goi nut Huy that gui - xem case GOI_RECOVERY_BOX_COMMAND).
// (PB r5b) server KPlayer::RecoveryBox khi tui thieu cho se nhet mon vao TAY
// (pos_hand - tang hinh voi moi bo quet DT), va lenh thu hoi KE TIEP lam mon
// dang cam bi NEM XUONG DAT = mat vinh vien. Nen: moi lan goi chi thu 1 MON,
// va client tu kiem CheckCanPlaceInEquipment truoc khi gui - thieu cho thi GIU
// mon trong hop (an toan, lay lai o lan FAILREQ sau khi tui da don).
// Tra ve: 1 = da gui thu hoi 1 mon; 0 = hop trong; -1 = co mon nhung tui
// khong du cho dat -> KHONG gui.
static int DT_ThuHoiBox(int nPlayerIdx)
{
	PlayerItem* pIt = Player[nPlayerIdx].m_ItemList.GetFirstItem();
	while (pIt)
	{
		if (pIt->nPlace == pos_affairitem && pIt->nIdx > 0)
		{
			int nX3, nY3;
			if (!Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
				Item[pIt->nIdx].GetWidth(), Item[pIt->nIdx].GetHeight(), &nX3, &nY3))
				return -1;
			SendClientRecoveryBox(Item[pIt->nIdx].GetID(),
				Item[pIt->nIdx].GetWidth(), Item[pIt->nIdx].GetHeight());
			return 1;
		}
		pIt = Player[nPlayerIdx].m_ItemList.GetNextItem();
	}
	return 0;
}

static bool DT_MatchRule(int nItemIdx, int g, int d, int p, int lvl, int five, int magic, int mn, int mx)
{
	if (nItemIdx <= 0)
		return false;
	if (magic > 0)
	{
		if (g >= 0 && Item[nItemIdx].GetGenre() != g)
			return false;
		if (d >= 0 && Item[nItemIdx].GetDetailType() != d)
			return false;
		if (p >= 0 && Item[nItemIdx].GetParticular() != p)
			return false;
		// (r5 - phan bien) server tasklink chi kiem 6 O DAU (for i=1,6 trong
		// seasonnpc.lua, ca loai 2 lan loai 3) - o 7/8 la dong an hoang kim/kham,
		// server KHONG doc. Kep 6 de khoi mua/giu/tra nham mon chi khop o o 7/8
		// (tra truot vinh vien + mat tien mua).
		for (int i = 0; i < 6; ++i)
		{
			if (Item[nItemIdx].m_aryMagicAttrib[i].nAttribType == magic
			&& Item[nItemIdx].m_aryMagicAttrib[i].nValue[0] >= mn
			&& Item[nItemIdx].m_aryMagicAttrib[i].nValue[0] <= mx)
				return true;
		}
		return false;
	}
	if (Item[nItemIdx].GetGenre() != g)
		return false;
	if (Item[nItemIdx].GetDetailType() != d)
		return false;
	if (Item[nItemIdx].GetParticular() != p)
		return false;
	if (lvl >= 0 && Item[nItemIdx].GetLevel() != lvl)
		return false;
	if (five >= 0 && Item[nItemIdx].GetSeries() != five)
		return false;
	return true;
}

// item bi khoa / do quy (tim-do se BI TIEU HUY khi nop -> tranh dung nham do xin)
static bool DT_ItemProtected(int nItemIdx, bool bWillLose)
{
	if (Item[nItemIdx].GetPlayerItemLock() > 0 || Item[nItemIdx].GetPlayerItemHLock() > 0
	|| Item[nItemIdx].GetPlayerItemLock() == -2)
		return true;
	if (bWillLose && Item[nItemIdx].GetGenre() == item_equip
	&& Item[nItemIdx].GetColorItem() > green_item)
		return true;
	return false;
}

// tim item theo luat trong tui (+ ruong neu bBox); tra idx, *pnPos = pos_* dang chua
static int DT_FindItemRule(int nPlayerIdx, bool bBox, bool bWillLose,
	int g, int d, int p, int lvl, int five, int magic, int mn, int mx, int* pnPos)
{
	PlayerItem* pIt = Player[nPlayerIdx].m_ItemList.GetFirstItem();
	int nBest = 0, nBestPos = 0;
	while (pIt && pIt->nIdx > 0)
	{
		int nPos = pIt->nPlace;
		bool bOk = (nPos == pos_equiproom);
		if (!bOk && bBox)
			bOk = (nPos == pos_repositoryroom || nPos == pos_exbox1room
				|| nPos == pos_exbox2room || nPos == pos_exbox3room);
		if (bOk && DT_MatchRule(pIt->nIdx, g, d, p, lvl, five, magic, mn, mx)
		&& !DT_ItemProtected(pIt->nIdx, bWillLose))
		{
			if (nPos == pos_equiproom)
			{
				*pnPos = nPos;
				return pIt->nIdx;	// uu tien do co san trong tui
			}
			if (!nBest)
			{
				nBest = pIt->nIdx;
				nBestPos = nPos;
			}
		}
		pIt = Player[nPlayerIdx].m_ItemList.GetNextItem();
	}
	*pnPos = nBestPos;
	return nBest;
}

static bool DT_GetItemPos(int nPlayerIdx, int nItemIdx, ItemPos* pPos)
{
	PlayerItem* pIt = Player[nPlayerIdx].m_ItemList.GetFirstItem();
	while (pIt && pIt->nIdx > 0)
	{
		if (pIt->nIdx == nItemIdx)
		{
			pPos->nPlace = pIt->nPlace;
			pPos->nX = pIt->nX;
			pPos->nY = pIt->nY;
			return true;
		}
		pIt = Player[nPlayerIdx].m_ItemList.GetNextItem();
	}
	return false;
}

// keo item tu ruong ve tui - server chi can m_CUnlocked, KHONG can dung gan ruong
// (mo phong case GOI_EXCHANGEITEM: c2s_dynamic_structure / c2sdnmbr_exchangeitem)
static void DT_BoxToBag(int nItemIdx, int nSrcPos)
{
	char szPack[16];
	DYNAMIC_COMMAND* pCmd = (DYNAMIC_COMMAND*)&szPack[0];
	pCmd->ProtocolType = c2s_dynamic_structure;
	pCmd->nBranch = c2sdnmbr_exchangeitem;
	pCmd->m_wLength = sizeof(DYNAMIC_COMMAND) - 1 + 2 * sizeof(BYTE) + sizeof(int);
	BYTE* pPos = (BYTE*)(pCmd + 1);
	*pPos = (BYTE)nSrcPos;
	++pPos;
	*pPos = (BYTE)pos_equiproom;
	++pPos;
	*(int*)pPos = Item[nItemIdx].GetID();
	g_pClient->SendPackToServer((BYTE*)pCmd, pCmd->m_wLength + 1);
}

// (20/08) mot goi = MOT CU CLICK trong 1 o do (Down == Up). Server KItemList::ExchangeItem
// TU CHOI goi co Down != Up (KItemList.cpp:2227) nen "keo" do phai la 2 cu click:
// click cho item (nhac len tay) roi click o dich (dat xuong) - dung khuon KPlayer.cpp:3281.
static void DT_ClickItem(int nPlace, int nX, int nY)
{
	PLAYER_MOVE_ITEM_COMMAND sMove;
	sMove.ProtocolType = c2s_playermoveitem;
	sMove.m_btDownPos = nPlace;
	sMove.m_btDownX = nX;
	sMove.m_btDownY = nY;
	sMove.m_btUpPos = nPlace;
	sMove.m_btUpX = nX;
	sMove.m_btUpY = nY;
	if (g_pClient)
		g_pClient->SendPackToServer((BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_COMMAND));
}

// (20/08) dung phu ve thanh: uu tien phu VO HAN (6,1,437 - khong ton), roi Tho Dia Phu,
// roi Hoi thanh phu (6,1,1083/1084). KHONG tu mua nua (yeu cau nguoi dung 19/08 toi).
static int DT_UsePortal(int nPlayerIdx)
{
	if (Player[nPlayerIdx].m_ItemList.AutoUseItem(6, 1, 437, nPlayerIdx))
		return 1;
	if (Player[nPlayerIdx].m_ItemList.AutoUseItem(item_townportal, 0, 0, nPlayerIdx))
		return 1;
	if (Player[nPlayerIdx].m_ItemList.AutoUseItem(6, 1, 1083, nPlayerIdx))
		return 1;
	if (Player[nPlayerIdx].m_ItemList.AutoUseItem(6, 1, 1084, nPlayerIdx))
		return 1;
	return 0;
}

static bool DT_LaPhuVe(int nItemIdx)
{
	if (Item[nItemIdx].GetGenre() == item_townportal)
		return true;
	if (Item[nItemIdx].GetGenre() == 6 && Item[nItemIdx].GetDetailType() == 1)
	{
		int p = Item[nItemIdx].GetParticular();
		return (p == 437 || p == 1083 || p == 1084);
	}
	return false;
}

static int DT_HasPortalInBag(int nPlayerIdx)
{
	PlayerItem* pIt = Player[nPlayerIdx].m_ItemList.GetFirstItem();
	while (pIt && pIt->nIdx > 0)
	{
		if (pIt->nPlace == pos_equiproom && DT_LaPhuVe(pIt->nIdx))
			return pIt->nIdx;
		pIt = Player[nPlayerIdx].m_ItemList.GetNextItem();
	}
	return 0;
}

// ==== (21/08) trang thai phu cua 1 client (1 nhan vat) ====
static DWORD g_dwDTKhoePend = 0;	// ID mon KHOE (loai 3) da dat vao hop giao, dang cho ket qua tra
static DWORD g_dwDTKhoeId = 0;		// ID mon khoe da tra XONG (server hoan lai) - can cat vao ruong
static int   g_nDTKhoeTry = 0;		// so nhip da thu cat mon khoe
static UINT  g_uDTTickT = 0;		// moc nhip DT_Process gan nhat (cong TP tui day doc de biet DT con song)
static int   g_nDTSellNeed = 8;		// DTP_SELLJUNK: du bao nhieu o trong thi thoi ban
static int   g_nDTSellMin = 5;		// DTP_SELLJUNK: het rac ma >= so nay thi van lam tiep duoc
static UINT  g_uDTYieldT = 0;		// lan cuoi nhuong may cho Hau can (DTP_YIELD)
static int   g_nDTYieldMap = 0;		// map luc nhuong
#define DT_YIELD_GAP (3u * 60u * 1000u)	// (PB F5) toi thieu giua 2 lan nhuong: du de chan ping-pong,
										// du ngan de TP tien/do hong/thuoc lap lai van duoc Hau can xu ly
static UINT  g_uDTSellPortalT = 0;	// lan cuoi SELLJUNK dung phu ve (het rac, nho Hau can)

// (20/08 - F11 nguoi dung) item DAT YEU CAU nhiem vu Da Tau dang lam (loai 1/2/3):
// TUYET DOI khong duoc ban. Goi tu may ban rac (ca Hau can lan DTP_SELLJUNK).
static bool DT_IsQuestItem(int nPlayerIdx, const autoData* pAp, int nItemIdx)
{
	if (!pAp || pAp->bDaTau != 1 || nItemIdx <= 0)
		return false;
	// (21/08) mon "khoe" vua tra xong dang cho cat ruong (DT_CatKhoe): cung cam ban
	if (g_dwDTKhoeId || g_dwDTKhoePend)
	{
		const DWORD dwK = Item[nItemIdx].GetID();
		if (dwK == g_dwDTKhoeId || dwK == g_dwDTKhoePend)
			return true;
	}
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	if (ea.nDTQType < 1 || ea.nDTQType > 3)
		return false;
	if (nItemIdx == ea.nDTItemIdx)
		return true;
	for (int k = 0; k < ea.nDTCandNum && k < 8; ++k)
	{
		const int c = ea.nDTCand[k];
		if (ea.nDTQType == 1)
		{
			const DTBuyRow& r = g_DTBuy[c];
			if (DT_MatchRule(nItemIdx, r.nGenre, r.nDetail, r.nParticular, r.nLevel, r.nFive, 0, 0, 0))
				return true;
		}
		else if (ea.nDTQType == 2)
		{
			const DTFindRow& r = g_DTFind[c];
			if (r.nMagic > 0)
			{
				if (DT_MatchRule(nItemIdx, r.nGenre, r.nDetail, r.nParticular, -1, -1, r.nMagic, r.nMin, r.nMax))
					return true;
			}
			else if (DT_MatchRule(nItemIdx, r.nGenre, r.nDetail, r.nParticular, r.nLevel, r.nFive, 0, 0, 0))
				return true;
		}
		else
		{
			const DTShowRow& r = g_DTShow[c];
			if (DT_MatchRule(nItemIdx, -1, -1, -1, -1, -1, r.nMagic, r.nMin, r.nMax))
				return true;
		}
	}
	return false;
}

// mo khoa ruong bang mat khau WAuto (tab Hau can); tra true khi da mo
static bool DT_EnsureUnlock(int nPlayerIdx, const autoData* pAp, UINT uCurTime)
{
	if (Player[nPlayerIdx].m_CUnlocked)
		return true;
	if (!pAp->szBoxPass[0])
		return false;
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	if (ea.uDTNext <= uCurTime + 250)
	{
		SendClientCPUnlockCmd(atoi(pAp->szBoxPass));
		ea.uDTNext = uCurTime + 1200;
	}
	return false;
}

// (21/08) cat item tu TUI vao ruong - nguoc voi DT_BoxToBag, cung goi c2sdnmbr_exchangeitem
// (server KProtocolProcess.cpp:5073: doi nSrcPos == pos_equiproom, m_CUnlocked, KHONG fight-mode,
// ruong dich phai con cho - thieu la bo qua im lang).
static void DT_BagToBox(int nItemIdx, int nDstPos)
{
	char szPack[16];
	DYNAMIC_COMMAND* pCmd = (DYNAMIC_COMMAND*)&szPack[0];
	pCmd->ProtocolType = c2s_dynamic_structure;
	pCmd->nBranch = c2sdnmbr_exchangeitem;
	pCmd->m_wLength = sizeof(DYNAMIC_COMMAND) - 1 + 2 * sizeof(BYTE) + sizeof(int);
	BYTE* pPos = (BYTE*)(pCmd + 1);
	*pPos = (BYTE)pos_equiproom;
	++pPos;
	*pPos = (BYTE)nDstPos;
	++pPos;
	*(int*)pPos = Item[nItemIdx].GetID();
	g_pClient->SendPackToServer((BYTE*)pCmd, pCmd->m_wLength + 1);
}

// (21/08) chon ruong con cho cho mon w x h, theo o "cat vao" (nSelStore) cua tab Hau can -
// dung thu tu cua may Hau can buoc 5: ruong chinh -> mo rong 1 -> (nSelStore>=1) mo rong 2
// -> (nSelStore>=2) mo rong 3. Tra pos_* ruong dich, 0 = het cho.
static int DT_ChestRoomFor(int nPlayerIdx, const autoData* pAp, int w, int h)
{
	int x, y;
	KItemList& il = Player[nPlayerIdx].m_ItemList;
	const int nEx = Npc[Player[nPlayerIdx].m_nIndex].m_ExBoxId;
	if (il.CheckCanPlaceInEquipment(w, h, &x, &y, room_repository))
		return pos_repositoryroom;
	if (nEx >= 1 && il.CheckCanPlaceInEquipment(w, h, &x, &y, room_exbox1))
		return pos_exbox1room;
	if (pAp->nSelStore >= 1 && nEx >= 2 && il.CheckCanPlaceInEquipment(w, h, &x, &y, room_exbox2))
		return pos_exbox2room;
	if (pAp->nSelStore >= 2 && nEx >= 3 && il.CheckCanPlaceInEquipment(w, h, &x, &y, room_exbox3))
		return pos_exbox3room;
	return 0;
}

// (21/08) thay ruong thuong trong luc dang tra loai 3 = tra XONG -> chot mon khoe can cat.
static void DT_KhoeXong(int nPlayerIdx)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	if (ea.nDTQType == 3 && g_dwDTKhoePend)
	{
		g_dwDTKhoeId = g_dwDTKhoePend;
		g_nDTKhoeTry = 0;
	}
	g_dwDTKhoePend = 0;
}

// (21/08 - nguoi dung: "lam nhiem vu khoe do xong thay vi ban thi gui mon do vao ruong")
// Mon khoe (loai 3) duoc server HOAN LAI sau khi tra xong (seasonnpc.lua Task_Accept_03 ->
// EndGiveBox) - truoc day nam lai trong tui roi bi may ban rac (Hau can / DTP_SELLJUNK)
// ban mat. Nay: ghi ID luc bam OK hop giao (g_dwDTKhoePend), thay ruong thuong (= tra
// XONG) thi chot (g_dwDTKhoeId), va o dau DTP_GOTONPC cat vao ruong (can ruong dang mo
// hoac mat khau ruong tab Hau can). Server CU (chua restart, chua co EndGiveBox) de mon
// ket trong hop giao -> thu hoi ve tui truoc roi moi cat. Tra 1 = dang ban, 0 = khong co viec.
static int DT_CatKhoe(int nPlayerIdx, const autoData* pAp, UINT uCurTime)
{
	if (!g_dwDTKhoeId)
		return 0;
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	if (ea.nDTStep == DTI_TURNIN)
		return 0;	// (PB F2) dang tren duong di TRA nhiem vu ke - de sau, khong cat nham mon sap nop
	int nIdx = 0, nPlace = -1;
	PlayerItem* pIt = Player[nPlayerIdx].m_ItemList.GetFirstItem();
	while (pIt && pIt->nIdx > 0)
	{
		if (Item[pIt->nIdx].GetID() == g_dwDTKhoeId)
		{
			nIdx = pIt->nIdx;
			nPlace = pIt->nPlace;
			break;
		}
		pIt = Player[nPlayerIdx].m_ItemList.GetNextItem();
	}
	if (nIdx && nIdx == ea.nDTItemIdx)
	{
		// (PB F2) chinh mon nay vua duoc chot cho nhiem vu khoe KE TIEP - giu trong tui,
		// tra xong lan nua se cat
		g_dwDTKhoeId = 0;
		g_nDTKhoeTry = 0;
		return 0;
	}
	if (!nIdx || nPlace == pos_repositoryroom || nPlace == pos_exbox1room
	 || nPlace == pos_exbox2room || nPlace == pos_exbox3room)
	{
		// da nam trong ruong (minh cat xong / Hau can cat ho) hoac khong con -> xong viec
		if (nIdx && g_nDTKhoeTry > 0)
			DT_Msg(nPlayerIdx, "<color=Cyan>§· cÊt mãn khoe vµo r­¬ng - lÇn sau gÆp nhiÖm vô khoe sÏ lÊy ra dïng l¹i.");
		g_dwDTKhoeId = 0;
		g_nDTKhoeTry = 0;
		return 0;
	}
	if (Npc[Player[nPlayerIdx].m_nIndex].m_FightMode)
		return 0;	// ngoai thanh server khong cho dung ruong - de luc ve thanh
	if (++g_nDTKhoeTry > 12)
	{
		DT_Msg(nPlayerIdx, "<color=Yellow>Kh«ng cÊt ®­îc mãn khoe vµo r­¬ng (r­¬ng khãa/®Çy?) - ®Ó l¹i trong tói.");
		g_dwDTKhoeId = 0;
		g_nDTKhoeTry = 0;
		return 0;
	}
	if (nPlace == pos_affairitem)
	{
		// server cu: mon con ket trong hop giao - thu hoi ve tui truoc (moi lan 1 mon)
		if (DT_ThuHoiBox(nPlayerIdx) < 0)
			DT_Msg(nPlayerIdx, "<color=Yellow>Mãn khoe cßn kÑt trong hép giao mµ tói thiÕu chç - sÏ lÊy sau.");
		ea.uDTNext = uCurTime + 1200;
		return 1;
	}
	if (nPlace != pos_equiproom)
	{
		g_dwDTKhoeId = 0;	// tren tay / cho la - thoi
		g_nDTKhoeTry = 0;
		return 0;
	}
	if (!Player[nPlayerIdx].m_CUnlocked && !pAp->szBoxPass[0])
	{
		DT_Msg(nPlayerIdx, "<color=Yellow>R­¬ng ®ang khãa mµ ch­a cã mËt khÈu ë tab HËu cÇn - kh«ng cÊt ®­îc mãn khoe, ®Ó l¹i trong tói.");
		g_dwDTKhoeId = 0;
		g_nDTKhoeTry = 0;
		return 0;
	}
	if (!DT_EnsureUnlock(nPlayerIdx, pAp, uCurTime))
		return 1;	// dang go mat khau ruong
	const int nDst = DT_ChestRoomFor(nPlayerIdx, pAp, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
	if (!nDst)
	{
		DT_Msg(nPlayerIdx, "<color=Yellow>R­¬ng ®Çy - kh«ng cÊt ®­îc mãn khoe, ®Ó l¹i trong tói.");
		g_dwDTKhoeId = 0;
		g_nDTKhoeTry = 0;
		return 0;
	}
	if (g_nDTKhoeTry == 1)
		DT_Msg(nPlayerIdx, "<color=Cyan>Khoe xong - cÊt mãn khoe vµo r­¬ng ®Ó lÇn sau dïng l¹i...");
	DT_BagToBox(nIdx, nDst);
	ea.uDTNext = uCurTime + 1200;
	return 1;
}

// (20/08) keo PHU VE tu ruong ve tui (chi can mat khau ruong). 1 = dang xu ly, 0 = chiu.
static int DT_PortalPull(int nPlayerIdx, const autoData* pAp, UINT uCurTime)
{
	// (r3) ruong khong dat mat khau thi o mat khau WAuto de trong - nhung ruong
	// co the DANG MO (m_CUnlocked) va van keo duoc. Chi chiu khi vua khoa vua
	// khong co mat khau.
	if (!Player[nPlayerIdx].m_CUnlocked && !pAp->szBoxPass[0])
		return 0;
	// (PB V07) server TU CHOI moi thao tac ruong khi m_FightMode=1 (ExchangeItem
	// KItemList.cpp:2250 + c2sdnmbr_exchangeitem KProtocolProcess.cpp:5096 deu gate)
	// -> ngoai thanh KHONG keo duoc, dung gui goi vo ich roi tuong "dang xu ly".
	if (Npc[Player[nPlayerIdx].m_nIndex].m_FightMode)
		return 0;
	int nBest = 0, nPos = 0;
	PlayerItem* pIt = Player[nPlayerIdx].m_ItemList.GetFirstItem();
	while (pIt && pIt->nIdx > 0)
	{
		if ((pIt->nPlace == pos_repositoryroom || pIt->nPlace == pos_exbox1room
		  || pIt->nPlace == pos_exbox2room || pIt->nPlace == pos_exbox3room)
		 && DT_LaPhuVe(pIt->nIdx))
		{
			nBest = pIt->nIdx;
			nPos = pIt->nPlace;
			break;
		}
		pIt = Player[nPlayerIdx].m_ItemList.GetNextItem();
	}
	if (!nBest)
		return 0;
	if (!DT_EnsureUnlock(nPlayerIdx, pAp, uCurTime))
		return 1;	// dang go mat khau - cho tick sau
	if (Player[nPlayerIdx].m_ItemList.CalcFreeItemCellCount(1, 1, room_equipment) < 1)
		return 0;
	DT_BoxToBag(nBest, nPos);
	return 1;
}

// ==== (20/08) DI CHO MUA DO SAP - trang thai rieng cua 1 client (1 nhan vat) ====
static int   g_nDTSapMask = 0;		// bit i = da xem het thanh g_DTNpc[i]
static DWORD g_aDTSapDone[96];		// dwID cac sap DA XEM trong thanh hien tai
static int   g_nDTSapDone = 0;
static DWORD g_dwDTSapCur = 0;		// sap dang mo xem
static UINT  g_uDTSapWait = 0;		// han cho sap tra loi (bot trang tri im lang)
static int   g_nDTSapWpt = 0;		// diem tu tap ke tiep trong thanh
static UINT  g_uDTSapWptT = 0;
static UINT  g_uDTSapDwell = 0;
static UINT  g_uDTSapHopT = 0;		// han cho MOT luot nho Xa Phu qua thanh (150s)
static UINT  g_uDTSapFresh = 0;		// (PB V11) lan cuoi xin du lieu sap (mua phai dua tren snapshot <3.5s)
static int   g_nDTSapXem = 0;		// (r3) so sap DA XEM trong thanh hien tai (thong ke)
static int   g_nDTSapCam = 0;		// (r3) so sap khong phan hoi (bot trang tri / ruong khoa)
static int   g_nDTSapBuyTry = 0;	// (PB V12) so lenh mua da gui cho sap hien tai
static int   g_nDTSapRut = 0;		// (PB V15) so lan da rut tien ruong cho sap hien tai
static UINT  g_uDTThpT = 0;		// (r5) Than Hanh Phu cho luot nhay nay: 0=chua thu,
									// 1=khong co/that bai (di Xa Phu), >1=thoi diem da dung phu
static DWORD g_dwDTSapProbe = 0;	// (r5c) sap dang tham do so mon (needcount)
static UINT  g_uDTSapProbeT = 0;	// han cho tra loi tham do
static UINT  g_uDTSapCntSeen = 0;	// moc uCntSeq luc gui tham do
static int   g_nDTSapProbeTry = 0;	// so lan tham do ung vien hien tai
static DWORD g_dwDTSapOkId = 0;		// ung vien DA qua tham do (sap that, co hang)
static int   g_nDTSapDsMap = -1;	// (r5e) danh ba sap: map da hoi (-1 = chua hoi)
static int   g_nDTSapDs = -1;		// so muc (-1 = chua co/cho tra loi)
static int   g_nDTSapDsCur = 0;		// dang di toi muc nao
static UINT  g_uDTSapDsT = 0;		// han cho server tra loi
static UINT  g_uDTSapDsFresh = 0;	// luc hoi lan cuoi (90s hoi lai)
static UINT  g_uDTSapDsSeen = 0;	// seq [SapMap] da doc
static DWORD g_aDTSapDsId[12];		// danh ba: id / toa do MPS
static int   g_nDTSapDsTry = 0;		// (r5f) so lan da hoi danh ba o map nay
static UINT  g_uDTSapDsItemT = 0;	// (r5h) han di toi MOT muc danh ba (45 giay)
static int   g_nDTSapDsItemIdx = -1;	// (r5i) muc danh ba dang tinh gio
static DWORD g_aDTSapXa[16];		// (r5i) sap KHONG TOI DUOC bang duong dai:
static int   g_nDTSapXa = 0;		// chi chan duong danh ba, KHONG chan quet gan
static int   g_aDTSapDsX[12];
static int   g_aDTSapDsY[12];

// ten thanh trong menu "Nhung thanh thi da di qua" cua Xa Phu (settings\Station.txt)
struct DTSapTown { int nMapId; const char* szMenu; };
static const DTSapTown g_aDTSapTown[10] =
{
	{ 1,   "Ph­îng T­êng" },
	{ 11,  "Thµnh §«" },	// (r5b) bo " Phu": menu khu cua THP chi ghi "Thanh Do Trung Tam/Dong/..." -> co " Phu" la strstr truot, ket 12s
	{ 162, "§¹i Lý" },
	{ 37,  "BiÖn Kinh" },
	{ 78,  "T­¬ng D­¬ng" },
	{ 80,  "D­¬ng Ch©u" },
	{ 176, "L©m An" },
	{ 20,  "Giang T©n" },
	{ 121, "Long M«n" },
	{ 53,  "Ba L¨ng" },	// (r5) rut ngan: THP viet "Huy\326n" HOA - "Ba L\250ng" khop ca 2 menu
};
// muc menu 1 cua Xa Phu: "Nhung thanh thi da di qua" (xaphu.lua:16)
#define DTM_SAP_THANHTHI "thµnh thÞ ®· ®i qua"

static const char* DT_SapTownMenu(int nMapId)
{
	for (int i = 0; i < 10; ++i)
		if (g_aDTSapTown[i].nMapId == nMapId)
			return g_aDTSapTown[i].szMenu;
	return NULL;
}

static int DT_SapTownIndex(int nMapId)
{
	for (int i = 0; i < g_nDTNpcCount && i < 30; ++i)
		if (g_DTNpc[i].nMapId == nMapId)
			return i;
	return -1;
}

// thanh ke tiep CHUA xem (co ten trong menu xa phu); -1 = het
static int DT_SapNextTown(int nCurMap)
{
	for (int i = 0; i < g_nDTNpcCount && i < 30; ++i)
	{
		if (g_DTNpc[i].nMapId == nCurMap)
			continue;
		if (g_nDTSapMask & (1 << i))
			continue;
		if (!DT_SapTownMenu(g_DTNpc[i].nMapId))
			continue;
		return i;
	}
	return -1;
}

static bool DT_SapDaXem(DWORD dwId)
{
	for (int i = 0; i < g_nDTSapDone; ++i)
		if (g_aDTSapDone[i] == dwId)
			return true;
	return false;
}

// (r5h - phan bien vong 3) cac bien danh ba la static trong DLL nen song qua
// ca chuyen di cho / doi nhan vat. Vong Da Tau moi vao lai DUNG thanh cu ma
// danh ba con "da tieu thu het" thi khoi di toi bi bo qua, waypoint cung bi
// chan -> bo nguyen mot thanh. Goi ham nay o MOI loi vao DTP_MUASAP.
// (r5i) danh sach RIENG cho muc danh ba khong bo toi duoc: dung DT_SapGhiXem
// se lam sap do mu ca voi VONG QUET GAN (cung dung g_aDTSapDone de loc) - bot
// dung sat ben sap con hang cung khong mo xem. Danh sach nay CHI chan duong
// danh ba (di bo toi toa do), quet can canh van thay binh thuong.
static int DT_SapDsDaBo(DWORD dwId)
{
	for (int i = 0; i < g_nDTSapXa; ++i)
		if (g_aDTSapXa[i] == dwId)
			return 1;
	return 0;
}

static void DT_SapDsGhiBo(DWORD dwId)
{
	if (g_nDTSapXa < 16 && !DT_SapDsDaBo(dwId))
		g_aDTSapXa[g_nDTSapXa++] = dwId;
}

static void DT_SapDsReset()
{
	g_nDTSapXa = 0;
	g_nDTSapDsItemIdx = -1;
	g_nDTSapDsMap = -1;
	g_nDTSapDs = -1;
	g_nDTSapDsCur = 0;
	g_nDTSapDsTry = 0;
	g_uDTSapDsFresh = 0;
	g_uDTSapDsT = 0;
	g_uDTSapDsItemT = 0;
	g_dwDTSapOkId = 0;
	g_dwDTSapProbe = 0;
	// PHAI nuot seq hien tai (khong dat 0) - khong thi goi [SapMap] sot lai cua
	// chuyen truoc se bi phan tich lai thanh danh ba cu ngay nhip dau.
	g_uDTSapDsSeen = g_sDTCap.uSapMapSeq;
}

static void DT_SapGhiXem(DWORD dwId)
{
	if (g_nDTSapDone < 96 && !DT_SapDaXem(dwId))
		g_aDTSapDone[g_nDTSapDone++] = dwId;
}

// (r5c) nR co nam trong 3x3 region quanh nSelfR khong - dung tam voi
// KPlayer::FindAroundPlayer (server chi tra loi tham do/mo xem trong tam nay).
static int DT_SapKeVung(int nSelfR, int nR)
{
	if (nSelfR < 0 || nR < 0)
		return 0;
	if (nR == nSelfR)
		return 1;
	for (int i = 0; i < 8; ++i)
		if (SubWorld[0].m_Region[nSelfR].m_nConnectRegion[i] == nR)
			return 1;
	return 0;
}

// dong cua so xem sap (UI + bo dem) - khong can gui gi len server (view la stateless)
static void DT_SapDong(int nPlayerIdx)
{
	CoreDataChanged(GDCNI_CLOSE_BAITAN, 0, 0);
	CoreDataChanged(GDCNI_UI_ACT, 3, 0);	// dong luon cua so tui do UI sap mo kem
	g_cSellItem.DeleteAll();
	Npc[Player[nPlayerIdx].m_nIndex].SetMenuState(PLAYER_MENU_STATE_NORMAL);
	g_dwDTSapCur = 0;
}

// di lan luot cac diem tu tap trong thanh (trung tam / tap hoa / xa phu / Da Tau)
// de sap quanh do duoc dong bo vao tam nhin. 1 = con diem dang di, 0 = het.
static int DT_SapWaypoint(int nPlayerIdx, int nMap, UINT uCurTime)
{
	int aX[8], aY[8], n = 0;
	std::map<int, sStation>::iterator itC = g_CenterStation.find(nMap);
	if (itC != g_CenterStation.end())
	{
		aX[n] = itC->second.x;
		aY[n] = itC->second.y;
		++n;
	}
	std::map<int, StationVector>::iterator it = g_ShopStation.find(nMap);
	if (it != g_ShopStation.end())
		for (int q = 0; q < (int)it->second.size() && q < 2 && n < 8; ++q)
		{
			aX[n] = it->second[q].x;
			aY[n] = it->second[q].y;
			++n;
		}
	it = g_MoveStation.find(nMap);
	if (it != g_MoveStation.end() && !it->second.empty() && n < 8)
	{
		aX[n] = it->second[0].x;
		aY[n] = it->second[0].y;
		++n;
	}
	for (int r = 0; r < g_nDTNpcCount && n < 8; ++r)
		if (g_DTNpc[r].nMapId == nMap)
		{
			aX[n] = g_DTNpc[r].nX * 32;
			aY[n] = g_DTNpc[r].nY * 32;
			++n;
			break;
		}
	if (g_nDTSapWpt >= n)
		return 0;
	if (!g_uDTSapWptT)
	{
		g_uDTSapWptT = uCurTime + 45000;
		// (r5d) bao ro dang DI TUAN cac diem tu tap (trung tam/tiem/xa phu) de
		// quet sap quanh do - khong phai chay nham toi NPC chuc nang.
		char szTuan[160];
		sprintf(szTuan, "<color=Gray>\247i tu\307n \256i\323m t\364 t\313p %d/%d t\327m s\271p quanh \256\343...",
			g_nDTSapWpt + 1, n);
		DT_Msg(nPlayerIdx, szTuan);
	}
	int nToi = DT_WalkTo(nPlayerIdx, aX[g_nDTSapWpt], aY[g_nDTSapWpt], 250, uCurTime);
	if (nToi && !g_uDTSapDwell)
	{
		g_uDTSapDwell = uCurTime + 2500;	// dung lai 2.5s cho sap quanh day sync ve
		return 1;
	}
	if ((nToi && uCurTime > g_uDTSapDwell) || uCurTime > g_uDTSapWptT)
	{
		++g_nDTSapWpt;
		g_uDTSapWptT = 0;
		g_uDTSapDwell = 0;
	}
	return 1;
}

// phan tich cau nhiem vu (course 1) -> loai + ung vien dong bang + yeu cau
static void DT_ParseQuest(int nPlayerIdx, const char* szQ)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	ea.nDTPhaseBack = 0;	// (r5d) nhiem vu moi = tour cu het y nghia
	g_dwDTKhoePend = 0;	// (21/08) hoi thoai nhiem vu hien lai = lan tra truoc khong xong
	ea.nDTQType = 0;
	ea.nDTCandNum = 0;
	ea.nDTCandCur = 0;
	ea.nDTReqNum = 0;
	ea.nDTMapId = 0;
	ea.nDTBook = 0;
	ea.nDTStatType = 0;
	int i, k;
	if (DT_Has(szQ, DTM_T1_MUA))
	{
		ea.nDTQType = 1;
		for (i = 0; i < g_nDTBuyCount && ea.nDTCandNum < 8; ++i)
		{
			if (!DT_HasName(szQ, g_DTBuy[i].szShop) || !DT_HasName(szQ, g_DTBuy[i].szItem))
				continue;
			bool bDup = false;
			for (k = 0; k < ea.nDTCandNum; ++k)
			{
				const DTBuyRow& a = g_DTBuy[ea.nDTCand[k]];
				const DTBuyRow& b = g_DTBuy[i];
				if (a.nGenre == b.nGenre && a.nDetail == b.nDetail && a.nParticular == b.nParticular
				&& a.nLevel == b.nLevel && a.nFive == b.nFive)
					bDup = true;
			}
			if (!bDup)
				ea.nDTCand[ea.nDTCandNum++] = i;
		}
		return;
	}
	if (DT_Has(szQ, DTM_T6_MANH))
	{
		ea.nDTQType = 6;
		ea.nDTReqNum = DT_NumAfter(szQ, DTM_T4_TIMGIUP);
		return;
	}
	if (DT_Has(szQ, DTM_T4_QUYEN))
	{
		ea.nDTQType = 4;
		ea.nDTBook = DT_Has(szQ, DTM_T4_DIADO) ? 1 : (DT_Has(szQ, DTM_T4_MATCHI) ? 2 : 0);
		ea.nDTReqNum = DT_NumAfter(szQ, DTM_T4_TIMGIUP);
		for (i = 0; i < g_nDTQuestMapCount; ++i)
		{
			if (DT_HasName(szQ, g_DTQuestMap[i].szName))
			{
				ea.nDTMapId = g_DTQuestMap[i].nMapId;
				ea.nDTAnchorX = g_DTQuestMap[i].nX * 32;
				ea.nDTAnchorY = g_DTQuestMap[i].nY * 32;
				break;
			}
		}
		return;
	}
	if (DT_Has(szQ, DTM_T23_TIM))
	{
		int nMin = DT_NumAfter(szQ, DTM_MIN_MARK);
		int nMax = DT_NumAfter(szQ, DTM_MAX_MARK);
		if (DT_Has(szQ, DTM_T3_XEMXONG))
		{
			ea.nDTQType = 3;
			for (i = 0; i < g_nDTShowCount && ea.nDTCandNum < 8; ++i)
				if (g_DTShow[i].nMin == nMin && g_DTShow[i].nMax == nMax
				&& DT_Has(szQ, g_DTShow[i].szMagic))
					ea.nDTCand[ea.nDTCandNum++] = i;
			return;
		}
		ea.nDTQType = 2;
		if (nMin >= 0 && nMax >= 0)
		{
			for (i = 0; i < g_nDTFindCount && ea.nDTCandNum < 8; ++i)
				if (g_DTFind[i].nMagic > 0 && g_DTFind[i].nMin == nMin && g_DTFind[i].nMax == nMax
				&& DT_HasName(szQ, g_DTFind[i].szItem) && DT_Has(szQ, g_DTFind[i].szMagic))
					ea.nDTCand[ea.nDTCandNum++] = i;
		}
		else
		{
			for (i = 0; i < g_nDTFindCount && ea.nDTCandNum < 8; ++i)
			{
				if (g_DTFind[i].nMagic != 0 || !DT_HasName(szQ, g_DTFind[i].szItem))
					continue;
				bool bDup = false;
				for (k = 0; k < ea.nDTCandNum; ++k)
				{
					const DTFindRow& a = g_DTFind[ea.nDTCand[k]];
					const DTFindRow& b = g_DTFind[i];
					if (a.nGenre == b.nGenre && a.nDetail == b.nDetail && a.nParticular == b.nParticular
					&& a.nLevel == b.nLevel && a.nFive == b.nFive)
						bDup = true;
				}
				if (!bDup)
					ea.nDTCand[ea.nDTCandNum++] = i;
			}
		}
		return;
	}
	if (DT_Has(szQ, DTM_T5_NANGCAP))
	{
		ea.nDTQType = 5;
		if (DT_Has(szQ, DTM_T5_EXP))
		{
			ea.nDTStatType = 2;
			ea.nDTReqNum = DT_NumAfter(szQ, DTM_T5_EXP);
		}
		else if (DT_Has(szQ, DTM_T5_DANHVONG))
		{
			ea.nDTStatType = 3;
			ea.nDTReqNum = DT_NumAfter(szQ, DTM_T5_DANHVONG);
		}
		else if (DT_Has(szQ, DTM_T5_PHUCDUYEN))
		{
			ea.nDTStatType = 4;
			ea.nDTReqNum = DT_NumAfter(szQ, DTM_T5_PHUCDUYEN);
		}
		else if (DT_Has(szQ, DTM_T5_TONGKIM))
		{
			ea.nDTStatType = 6;
			ea.nDTReqNum = DT_NumAfter(szQ, DTM_T5_TONGKIM);
		}
		else if (DT_Has(szQ, DTM_T5_PK))
		{
			ea.nDTStatType = 5;
			ea.nDTReqNum = DT_NumAfter(szQ, DTM_T5_PK);
		}
		return;
	}
}

// tim item khop 1 ung vien hien tai (T1/T2/T3); tra idx item, *pnPos vi tri
static int DT_FindCandItem(int nPlayerIdx, const autoData* pAp, int* pnPos)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	bool bBox = pAp->bDTUseBox != 0;
	if (ea.nDTCandCur >= ea.nDTCandNum)
		return 0;
	int c = ea.nDTCand[ea.nDTCandCur];
	if (ea.nDTQType == 1)
	{
		const DTBuyRow& r = g_DTBuy[c];
		return DT_FindItemRule(nPlayerIdx, bBox, true, r.nGenre, r.nDetail, r.nParticular,
			r.nLevel, r.nFive, 0, 0, 0, pnPos);
	}
	if (ea.nDTQType == 2)
	{
		const DTFindRow& r = g_DTFind[c];
		if (r.nMagic > 0)
			return DT_FindItemRule(nPlayerIdx, bBox, true, r.nGenre, r.nDetail, r.nParticular,
				-1, -1, r.nMagic, r.nMin, r.nMax, pnPos);
		return DT_FindItemRule(nPlayerIdx, bBox, true, r.nGenre, r.nDetail, r.nParticular,
			r.nLevel, r.nFive, 0, 0, 0, pnPos);
	}
	if (ea.nDTQType == 3)
	{
		const DTShowRow& r = g_DTShow[c];
		return DT_FindItemRule(nPlayerIdx, bBox, false, -1, -1, -1, -1, -1,
			r.nMagic, r.nMin, r.nMax, pnPos);
	}
	return 0;
}

// xu ly "ket" / loai bi tat: huy hay treo theo cau hinh
static int DT_Skip(int nPlayerIdx, const autoData* pAp, UINT uCurTime, const char* szWhy)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	ea.nDTPhaseBack = 0;	// (r5d) bo qua nhiem vu = bo tour dang do
	// (19/08 - chu game chot) 'Khi bo qua = Huy nhiem vu' CHI ap cho loai nhiem vu
	// nguoi choi da TAT trong tab Da Tau. Loai dang BAT ma ket thi TREO, tuyet doi
	// khong huy - huy la mat cong lam do / mat luot trong chuoi.
	const int nLoaiNay = ea.nDTQType;
	const bool bLoaiBiTat = (nLoaiNay >= 1 && nLoaiNay <= 6
	                      && !pAp->bDTType[nLoaiNay - 1]);
	if (pAp->nDTSkipMode == 1 && bLoaiBiTat)
	{
		char szHuy[512];
		if (strlen(szWhy) < 440)
		{
			sprintf(szHuy, "%s <color=Gray>(hñy nhiÖm vô - lo¹i nµy ®ang t¾t)", szWhy);
			DT_Msg(nPlayerIdx, szHuy);
		}
		else
			DT_Msg(nPlayerIdx, szWhy);
		ea.nDTStep = DTI_CANCEL;
		ea.nDTPhase = DTP_GOTONPC;
		ea.nDTRetry = 0;
		ea.nDTEngaged = 1;
		return 1;
	}
	return DT_Hold(nPlayerIdx, szWhy, uCurTime, 15 * 60 * 1000);
}

// (r3) sau khi don tui xong: quay lai DUNG mach dang lam do - dang tra do thi ra
// NPC (giu DTI_TURNIN), dang can di lai map thi ra Xa Phu, con lai ve IDLE.
static void DT_SellResume(ExtAuto& ea)
{
	if (ea.nDTBackXaFu)
	{
		ea.nDTBackXaFu = 0;
		ea.nDTPhase = DTP_GOXAFU;
	}
	else if (ea.nDTStep == DTI_TURNIN)
		ea.nDTPhase = DTP_GOTONPC;
	else
		ea.nDTPhase = DTP_IDLE;
	ea.nDTRetry = 0;
}

// (21/08) vao pha ban rac tai cho - dung chung cho MOI loi vao (DT_BagRelease / vua ve thanh /
// bi dua ve thanh luc farm). Nguong thoat mac dinh 8 o, toi thieu 5 (= server doi khi tra);
// hoi thoai "can it nhat N o" cua moc set se nang nguong SAU khi goi ham nay.
static void DT_SellStart(int nPlayerIdx, UINT uCurTime)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	ea.nDTPhase = DTP_SELLJUNK;
	ea.uDTHoldUntil = uCurTime + 4u * 60u * 1000u;	// han an toan 4 phut
	ea.nDTRetry = 0;		// so lenh ban da gui
	// (PB S3) moc so o trong de vao nDTShopTry - nDTItemIdx phai giu nguyen nghia
	// "item nhiem vu da chot" cho DT_IsQuestItem, khong duoc muon lam bien dem.
	ea.nDTShopTry = Player[nPlayerIdx].m_ItemList.CalcFreeItemCellCount(1, 1, room_equipment);
	g_nDTSellNeed = 8;
	g_nDTSellMin = 5;
}

// (21/08) nhuong may cho HAU CAN (tab Hau can, o "Ve thanh") lam viec thanh theo cau hinh
// cua no: ban rac / mo ruong / rut tien / sua do / cat do / mua thuoc / giu tien (buoc 0-8);
// toi buoc 9 (sap ra Xa Phu len map luyen cong) thi Da Tau lay lai may va tu di tiep
// (DTP_YIELD). Chi goi khi pAp->bReturn bat va dang o thanh.
static int DT_Yield(int nPlayerIdx, UINT uCurTime, const char* szWhy)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	DT_Msg(nPlayerIdx, szWhy);
	g_uDTYieldT = uCurTime;
	g_nDTYieldMap = SubWorld[0].m_SubWorldID;
	ea.nHomeStep = 0;	// Hau can chay lai tu dau (may DT cam lai nen no chua chay nhip nao)
	ea.nSubStep = 0;
	ea.nDTBackXaFu = 0;	// sau khi nhuong, EXEC/GOTONPC tu quyet dinh duong di
	ea.nDTPhase = DTP_YIELD;
	ea.uDTHoldUntil = uCurTime + 5u * 60u * 1000u;	// han chot: Hau can ket o dau thi lay lai may
	ea.nDTEngaged = 0;
	return 0;
}

// (20/08 - loi "phu ve khong ban ma di thang xa phu roi phu ve lap mai") tui day:
// truoc day THA MAY cho may Hau can - nhung Hau can chay theo cau hinh RIENG cua no
// (co the khong bat Ban do, xong viec lai tu quay ve bai) => vong lap phu ve vo tan.
// Nay Da Tau TU ban rac TAI CHO (server cho ban khong can dung canh tiem - chinh may
// Hau can van ban ngay buoc 1 truoc khi di dau). Tieu chi ban + bo loc giu do lay
// dung bo cua tab Hau can, KEM la chan khong ban item dat yeu cau nhiem vu (F11).
static int DT_BagRelease(int nPlayerIdx, const autoData* pAp, UINT uCurTime, const char* szWhy)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	DT_Msg(nPlayerIdx, szWhy);
	ea.uDTStatusTime = 0;
	DT_Msg(nPlayerIdx, "<color=Yellow>Tói ®Çy - auto tù b¸n bít trang bÞ tr¾ng/xanh ngay t¹i chç...");
	DT_SellStart(nPlayerIdx, uCurTime);
	ea.nDTEngaged = 1;
	return 1;
}

// ================== MAY CHINH ==================
static int DT_Process(int nPlayerIdx, const autoData* pAp, UINT uCurTime)
{
	ExtAuto& ea = Player[nPlayerIdx].m_sExtAuto;
	KDaTauCapture& cap = g_sDTCap;
	g_uDTTickT = uCurTime;	// (21/08) dau nhip "DT con song" cho cong TP tui day (ATYPE_TP_FULLITEM)
	int nMap = SubWorld[0].m_SubWorldID;
	int nToday = DT_Today();
	int i, idx;

	// sang ngay moi -> mo lai neu dang nghi vi du 40
	if (ea.nDTDoneDay && ea.nDTDoneDay != nToday)
	{
		ea.nDTDoneDay = 0;
		if (ea.nDTPhase == DTP_HOLD)
		{
			ea.nDTPhase = DTP_IDLE;
			ea.uDTHoldUntil = 0;
		}
	}

	if (ea.nDTPhase == DTP_HOLD)
	{
		if (ea.uDTHoldUntil && uCurTime > ea.uDTHoldUntil)
		{
			// (r3) hold "du 40" (freeze): chi ra hoi lai NPC neu DANG o thanh co Da
			// Tau; dang treo o map luyen cong thi KHONG keo ve (cho nua dem hoac
			// luot ve thanh tu nhien cua Hau can) - het canh yo-yo moi 60 phut.
			if (ea.nDTHoldFreeze)
			{
				int nOThanhH = 0;
				for (i = 0; i < g_nDTNpcCount; ++i)
					if (g_DTNpc[i].nMapId == nMap)
					{
						nOThanhH = 1;
						break;
					}
				if (!nOThanhH)
				{
					// (r4 - PB R1) chi la KIEM VI TRI, khong phai chu ky hoi NPC: kiem lai
					// moi 60 GIAY de luot ve thanh tu nhien nao cua Hau can cung bat duoc
					// probe (60 phut lam co che hoi-lai gan nhu khong bao gio trung thanh).
					ea.uDTHoldUntil = uCurTime + 60u * 1000u;
					ea.nDTEngaged = 0;
					return 0;
				}
			}
			ea.nDTPhase = DTP_IDLE;
			ea.uDTHoldUntil = 0;
		}
		else
		{
			// (20/08) dang treo T2 (tim) / T3 (khoe) ma trong luc auto thuong chay lai
			// NHAT duoc do dat yeu cau -> tu phat hien va quay ve tra ngay.
			if ((ea.nDTQType == 2 || ea.nDTQType == 3) && ea.nDTCandNum > 0
			 && !ea.nDTHoldFreeze && ea.uDTPath < uCurTime)
			{
				ea.uDTPath = uCurTime + 5000;	// quet 5 giay/lan
				int nPosH = 0;
				// (PB V10) CHI nhan do vua NHAT vao TUI. Do nam trong ruong da duoc EXEC
				// xu ly tu luc nhan nhiem vu; nhan ca ruong o day se lap vo han HOLD<->EXEC
				// khi mat khau ruong sai/trong (EXEC keo that bai roi Hold, Hold lai thay).
				int nCoH = DT_FindCandItem(nPlayerIdx, pAp, &nPosH);
				if (nCoH && nPosH == pos_equiproom)
				{
					DT_Msg(nPlayerIdx, "<color=Green>§· cã ®å ®¹t yªu cÇu nhiÖm vô - quay vÒ gÆp D· TÈu tr¶ ngay!");
					ea.uDTHoldUntil = 0;
					ea.uDTPath = 0;
					ea.nDTStep = DTI_NONE;
					ea.nDTPhase = DTP_EXEC;	// EXEC tu lo: keo tu ruong / di tra
					ea.nDTRetry = 0;
					ea.nDTEngaged = 1;
					return 1;
				}
			}
			// (r3) MOI hold deu NHA MAY - ke ca "du 40": auto thuong + Hau can dua
			// nhan vat ve nhip binh thuong (ban rac, mua do, len map treo).
			ea.nDTEngaged = 0;
			return 0;
		}
	}

	// (20/08) tui day -> TU ban rac tai cho; du cho trong thi quay lai lam tiep ngay.
	// Neu gui vai lenh ban ma so o trong khong nhuc nhich (map cam ban?) va dang o
	// ngoai thanh thi phu ve thanh ban tiep - het duong thi treo nho nguoi don.
	if (ea.nDTPhase == DTP_SELLJUNK)
	{
		ea.nDTEngaged = (ea.nDTQType == 4 && nMap == ea.nDTMapId) ? 2 : 1;
		int nTrong = Player[nPlayerIdx].m_ItemList.CalcFreeItemCellCount(1, 1, room_equipment);
		if (nTrong >= g_nDTSellNeed)
		{
			DT_Msg(nPlayerIdx, "<color=Cyan>Tói ®· cã chç trèng - quay l¹i lµm D· TÈu.");
			DT_SellResume(ea);
			ea.uDTHoldUntil = 0;
		}
		else if (!pAp->bSellItem)
		{
			// (PB V09) TON TRONG cong tac "Ban vat pham" tab Hau can: nguoi choi tat
			// thi Da Tau tuyet doi khong tu ban do - treo nho nguoi don tui.
			ea.uDTHoldUntil = 0;
			return DT_Hold(nPlayerIdx, "<color=Yellow>Tói ®Çy - h·y dän tói, hoÆc bËt « 'B¸n vËt phÈm' ë tab HËu cÇn ®Ó auto tù b¸n trang bÞ tr¾ng/xanh.", uCurTime, 15 * 60 * 1000);
		}
		else if (uCurTime > ea.uDTHoldUntil)
		{
			ea.uDTHoldUntil = 0;
			if (nTrong >= g_nDTSellMin)
			{
				DT_SellResume(ea);	// du o de tra nhiem vu - lam tiep dung mach
				return 1;
			}
			return DT_Hold(nPlayerIdx, "<color=Yellow>B¸n r¸c xong vÉn ch­a ®ñ « trèng - h·y dän bít tói gióp auto.", uCurTime, 15 * 60 * 1000);
		}
		else if (ea.uDTNext <= uCurTime)
		{
			ea.uDTNext = uCurTime + 700;
			// gui >=4 lenh ban ma so o trong khong tang + dang ngoai thanh (fight mode):
			// co the map nay khong cho ban - phu ve thanh roi ban tiep
			int nOThanh = 0;
			for (i = 0; i < g_nDTNpcCount; ++i)
				if (g_DTNpc[i].nMapId == nMap)
				{
					nOThanh = 1;
					break;
				}
			if (ea.nDTRetry >= 4 && nTrong <= ea.nDTShopTry && !nOThanh
			 && Npc[Player[nPlayerIdx].m_nIndex].m_FightMode)
			{
				if (DT_UsePortal(nPlayerIdx) || DT_PortalPull(nPlayerIdx, pAp, uCurTime))
				{
					if (ea.nDTQType == 4 && ea.nDTMapId > 0)
						ea.nDTBackXaFu = 1;	// (21/08) ban xong o thanh thi ra Xa Phu di lai, khoi ghe NPC
					ea.nDTRetry = 0;
					ea.uDTNext = uCurTime + 3000;
					return 1;
				}
			}
			// chon 1 mon rac dem ban: trang bi trang/xanh, khong khoa, ton trong bo loc
			// giu do cua tab Hau can, TRU item dat yeu cau nhiem vu (DT_IsQuestItem)
			int nBan = 0;
			for (int bi = 0; bi < EQUIPMENT_ROOM_HEIGHT && !nBan; ++bi)
				for (int bj = 0; bj < EQUIPMENT_ROOM_WIDTH; ++bj)
				{
					int nIdx2 = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(bj, bi);
					if (nIdx2 <= 0)
						continue;
					if (Item[nIdx2].GetGenre() != item_equip)
						continue;
					if (Item[nIdx2].GetColorItem() > green_item)
						continue;
					if (Item[nIdx2].GetPlayerItemLock() > 0 || Item[nIdx2].GetPlayerItemHLock() > 0
					 || Item[nIdx2].GetPlayerItemLock() == -2)
						continue;
					if (DT_IsQuestItem(nPlayerIdx, pAp, nIdx2))
						continue;
					int nDet = Item[nIdx2].GetDetailType();
					if (!pAp->bSellHorse && nDet >= equip_horse)
						continue;
					if (pAp->bSaveRing && (nDet == equip_ring || nDet == equip_amulet || nDet == equip_pendant)
					 && Item[nIdx2].GetLevel() > pAp->nSRLevel)
						continue;
					// bo loc giu do theo dong ma (nhu may Hau can): +all skill (139) luon giu
					if (!pAp->nSelSell && pAp->nFtMaCount)
					{
						bool bGiu = false;
						for (int bk = 0; bk < pAp->nFtMaCount && !bGiu; ++bk)
							for (int bm = 0; bm < 6; ++bm)
							{
								if (Item[nIdx2].m_aryMagicAttrib[bm].nAttribType == 139)
								{
									bGiu = true;
									break;
								}
								if (Item[nIdx2].m_aryMagicAttrib[bm].nAttribType == 0)
									break;
								if (pAp->nFtMagic[bk][0] == Item[nIdx2].m_aryMagicAttrib[bm].nAttribType
								 && (pAp->nFtMagic[bk][0] == magic_indestructible_b
								  || Item[nIdx2].m_aryMagicAttrib[bm].nValue[0] >= pAp->nFtMagic[bk][1]))
								{
									bGiu = true;
									break;
								}
							}
						if (bGiu)
							continue;
					}
					nBan = nIdx2;
					break;
				}
			if (nBan)
			{
				SendClientCmdSell(Item[nBan].GetID());
				++ea.nDTRetry;
				return ea.nDTEngaged;
			}
			// het rac ma van thieu cho
			if (nTrong >= g_nDTSellMin)
			{
				ea.uDTHoldUntil = 0;
				DT_SellResume(ea);
				return 1;
			}
			// (21/08 - "phu ve khong ban, toi xa phu, len map, phu ve lai") con do khong ban
			// duoc (nguyen lieu/do khoa/do tim...): nho may HAU CAN don theo cau hinh cua no
			// (cat ruong/ban/mua thuoc) roi lam tiep - dang ngoai thanh thi phu ve truoc. Moi
			// DT_YIELD_GAP nhuong 1 lan; nhuong roi van chat thi treo nho nguoi don.
			if (pAp->bReturn && uCurTime - g_uDTYieldT > DT_YIELD_GAP)
			{
				if (nOThanh)
					return DT_Yield(nPlayerIdx, uCurTime, "<color=Yellow>HÕt r¸c ®Ó b¸n mµ tói vÉn chËt - ®Ó HËu cÇn cÊt r­¬ng/dän tói råi lµm tiÕp.");
				if (Npc[Player[nPlayerIdx].m_nIndex].m_FightMode)
				{
					// moi 15 giay dung phu 1 lan (Tho Dia Phu TIEU HAO - dang cho chuyen map thi dung lap)
					if (uCurTime - g_uDTSellPortalT > 15000u && DT_UsePortal(nPlayerIdx))
					{
						g_uDTSellPortalT = uCurTime;
						if (ea.nDTQType == 4 && ea.nDTMapId > 0)
							ea.nDTBackXaFu = 1;
						DT_Msg(nPlayerIdx, "<color=Yellow>HÕt r¸c ®Ó b¸n mµ tói vÉn chËt - phï vÒ thµnh nhê HËu cÇn dän tói råi quay l¹i.");
						ea.uDTNext = uCurTime + 3000;
						return ea.nDTEngaged;
					}
					if (uCurTime - g_uDTSellPortalT <= 15000u)
					{
						ea.uDTNext = uCurTime + 1000;	// dang cho phu chuyen map
						return ea.nDTEngaged;
					}
				}
			}
			ea.uDTHoldUntil = 0;
			return DT_Hold(nPlayerIdx, "<color=Yellow>B¸n hÕt r¸c mµ vÉn ch­a ®ñ « trèng - h·y dän bít tói gióp auto.", uCurTime, 15 * 60 * 1000);
		}
		return ea.nDTEngaged;
	}

	if (Npc[Player[nPlayerIdx].m_nIndex].m_Doing == do_death
	|| Npc[Player[nPlayerIdx].m_nIndex].m_Doing == do_revive)
		return 0;
	if (Player[nPlayerIdx].CheckTrading())
		return 0;

	// pha FARM cua T5-exp tha may hoan toan (auto thuong cay theo cau hinh nguoi choi)
	if (ea.nDTPhase == DTP_FARM && ea.nDTQType == 5 && ea.nDTStatType == 2)
	{
		double dCur = Player[nPlayerIdx].m_nExp;
		if (g_dDTPrevExp >= 0 && dCur > g_dDTPrevExp)
		{
			g_dDTExpGain += dCur - g_dDTPrevExp;
			ea.uDTFarmStall = uCurTime;	// co tien do exp -> reset stall
		}
		g_dDTPrevExp = dCur;
		if (ea.uDTFarmStall && uCurTime - ea.uDTFarmStall > 20u * 60u * 1000u)
			return DT_Hold(nPlayerIdx, "<color=Red>Cµy kinh nghiÖm qu¸ l©u kh«ng tiÕn triÓn.", uCurTime, 15 * 60 * 1000);
		if (g_dDTExpGain >= (double)ea.nDTReqNum)
		{
			DT_Msg(nPlayerIdx, "<color=Green>§· ®ñ kinh nghiÖm - quay vÒ tr¶ nhiÖm vô.");
			ea.nDTStep = DTI_TURNIN;
			ea.nDTPhase = DTP_RETURN;
			ea.nDTEngaged = 1;
			return 1;
		}
		ea.nDTEngaged = 0;
		return 0;
	}

	if (ea.uDTNext > uCurTime)
		return ea.nDTEngaged;
	ea.uDTNext = uCurTime + 250;

	// [Ruong thuong] Da Tau tra thuong bang HAI cua so lien tiep (exp/bac roi
	// diem/may man/vat pham). Cua so thu hai thuong bat ra khi may DA ROI pha
	// WAITDLG -> phai bat o MOI PHA, khong thi no nam nguyen tren man hinh va
	// may di nhan nhiem vu moi (loi nguoi dung bao 19/08).
	// (PB V13) dang giua 2 buoc "dat item vao hop -> bam OK" (GIVEBOX, co +100) thi
	// nhuong 1 nhip cho OK di truoc - khong thi item nam ket trong hop, may quen mat.
	if (cap.uFinSeq != ea.uDTFinSeen && ea.nDTPhase != DTP_REWARD
	 && !(ea.nDTPhase == DTP_GIVEBOX && ea.nDTStep == DTI_TURNWAIT + 100))
	{
		// (r5d - nguoi dung bao "tu chay ve Da Tau giua luc tim sap") cua so
		// thuong THU 2 cua nhiem vu truoc hay toi TRE khi may DA sang di cho -
		// nho pha lai de bam thuong xong quay ve dung cho, khong lam lai tour.
		ea.nDTPhaseBack = (ea.nDTPhase == DTP_MUASAP || ea.nDTPhase == DTP_CITYHOP)
			? ea.nDTPhase : 0;
		DT_KhoeXong(nPlayerIdx);	// (21/08) loai 3 co thuong = tra XONG, mon khoe duoc hoan -> cat ruong
		ea.nDTPhase = DTP_REWARD;
		ea.nDTRetry = 0;
	}

	switch (ea.nDTPhase)
	{
	case DTP_IDLE:
	{
		ea.nDTEngaged = 0;
		// Dang giu nhiem vu loai 4 va DUNG tren map nhiem vu (vd vua het mot lan treo):
		// lam tiep tai cho. CHI duoc phu ve khi DU cuon (yeu cau chu game 19/08).
		if (ea.nDTQType == 4 && ea.nDTMapId > 0 && nMap == ea.nDTMapId
		 && ea.nDTProg < ea.nDTReqNum)
		{
			ea.nDTPhase = DTP_FARM;
			ea.nDTEngaged = 2;
			ea.uDTFarmStall = uCurTime;
			return 2;
		}
		for (i = 0; i < g_nDTNpcCount; ++i)
			if (g_DTNpc[i].nMapId == nMap)
				break;
		if (i >= g_nDTNpcCount)
		{
			DT_Msg(nPlayerIdx, "<color=Cyan>Kh«ng ë thµnh cã D· TÈu - dïng Thæ §Þa Phï vÒ thµnh.");
			ea.nDTPhase = DTP_RETURN;
			ea.nDTStep = DTI_NONE;
			ea.nDTEngaged = 1;
			return 1;
		}
		DT_Msg(nPlayerIdx, "<color=Cyan>§ang ®Õn chç NPC D· TÈu ®Ó nhËn / tr¶ nhiÖm vô.");
		ea.nDTPhase = DTP_GOTONPC;
		ea.nDTStep = DTI_NONE;
		ea.nDTRetry = 0;
		ea.nDTEngaged = 1;
		return 1;
	}

	case DTP_RETURN:
	{
		ea.nDTEngaged = 1;
		for (i = 0; i < g_nDTNpcCount; ++i)
		{
			if (g_DTNpc[i].nMapId == nMap)
			{
				// (r3 - "phu ve la ban rac lien") vua dat chan ve thanh: bat o "Ban vat
				// pham" + tui coi it cho trong -> ban rac TRUOC roi moi lam viec tiep
				// (DT_SellResume se dua ve dung mach: NPC / Xa Phu).
				if (pAp->bSellItem
				 && Player[nPlayerIdx].m_ItemList.CalcFreeItemCellCount(1, 1, room_equipment) < 10)
				{
					DT_SellStart(nPlayerIdx, uCurTime);
					DT_Msg(nPlayerIdx, "<color=Cyan>VÒ thµnh - b¸n bít r¸c tr­íc råi lµm tiÕp...");
					return 1;
				}
				if (ea.nDTBackXaFu)
				{
					// ve thanh chi de DI LAI Xa Phu (len nham map luc truoc)
					ea.nDTBackXaFu = 0;
					ea.nDTPhase = DTP_GOXAFU;
					ea.nDTRetry = 0;
					DT_Msg(nPlayerIdx, "<color=Cyan>§· vÒ thµnh - ra Xa Phu ®i tiÕp tíi map nhiÖm vô.");
					return 1;
				}
				ea.nDTPhase = DTP_GOTONPC;
				ea.nDTRetry = 0;
				return 1;
			}
		}
		// khong o thanh: dung phu ve (can fight mode - map luyen cong/godatau deu bat).
		// (20/08) uu tien phu VO HAN (6,1,437) -> Tho Dia Phu -> Hoi thanh phu; het thi
		// keo tu ruong; TUYET DOI khong tu mua nua (yeu cau nguoi dung).
		if (!Npc[Player[nPlayerIdx].m_nIndex].m_FightMode)
			return DT_Hold(nPlayerIdx, "<color=Red>KÑt: kh«ng ë thµnh mµ kh«ng dïng ®­îc Thæ §Þa Phï.", uCurTime, 10 * 60 * 1000);
		if (++ea.nDTRetry > 8)
			return DT_Hold(nPlayerIdx, "<color=Red>KÑt: kh«ng vÒ ®­îc thµnh (hÕt phï vÒ?).", uCurTime, 10 * 60 * 1000);
		if (!DT_UsePortal(nPlayerIdx))
		{
			if (!DT_PortalPull(nPlayerIdx, pAp, uCurTime))
				return DT_Hold(nPlayerIdx, "<color=Yellow>HÕt phï vÒ trong tói (phï ë r­¬ng kh«ng lÊy ®­îc khi ngoµi thµnh) - bá Thæ §Þa Phï / phï v« h¹n vµo tói gióp auto.", uCurTime, 10 * 60 * 1000);
		}
		ea.uDTNext = uCurTime + 3000;
		return 1;
	}

	case DTP_GOTONPC:
	{
		ea.nDTEngaged = 1;
		// (21/08) mon "khoe" vua tra xong (server hoan lai) -> cat vao ruong truoc (yeu cau
		// nguoi dung: "khoe do xong thay vi ban thi gui mon do vao ruong").
		if (DT_CatKhoe(nPlayerIdx, pAp, uCurTime))
			return 1;
		const DTNpcRow* pRow = NULL;
		for (i = 0; i < g_nDTNpcCount; ++i)
			if (g_DTNpc[i].nMapId == nMap)
			{
				pRow = &g_DTNpc[i];
				break;
			}
		if (!pRow)
		{
			ea.nDTPhase = DTP_RETURN;
			ea.nDTRetry = 0;
			return 1;
		}
		int nIdx = DT_FindNpcTpl(nPlayerIdx, 108, 0);
		if (nIdx)
		{
			int nX, nY, dX, dY;
			Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nX, &nY);
			Npc[nIdx].GetMpsPos(&dX, &dY);
			if (g_GetDistance(nX, nY, dX, dY) <= 128)
			{
				ea.uDTDlgSeen = cap.uDlgSeq;
				ea.uDTTalkSeen = cap.uTalkSeq;
				ea.uDTFinSeen = cap.uFinSeq;
				ea.uDTBoxSeen = cap.uBoxSeq;
				Player[nPlayerIdx].DialogNpc(nIdx);
				ea.nDTPhase = DTP_WAITDLG;
				ea.uDTNext = uCurTime + 700;
				return 1;
			}
			DT_WalkTo(nPlayerIdx, dX, dY, 96, uCurTime);
			return 1;
		}
		if (DT_WalkTo(nPlayerIdx, pRow->nX * 32, pRow->nY * 32, 160, uCurTime))
		{
			if (++ea.nDTRetry > 20)
				return DT_Hold(nPlayerIdx, "<color=Red>Kh«ng thÊy NPC D· TÈu ë täa ®é ®· ®Þnh.", uCurTime, 5 * 60 * 1000);
		}
		return 1;
	}

	case DTP_WAITDLG:
	{
		ea.nDTEngaged = 1;
		// cua so 3 ruong (course 2 hoac vua tra xong) - KHONG an uFinSeq o day:
		// chinh DTP_REWARD moi tieu thu no de biet phai bam nut nao.
		if (cap.uFinSeq != ea.uDTFinSeen)
		{
			DT_KhoeXong(nPlayerIdx);	// (21/08) loai 3: co thuong = tra XONG -> chot mon khoe de cat ruong
			ea.nDTPhase = DTP_REWARD;
			ea.nDTRetry = 0;
			ea.uDTNext = uCurTime + 700;
			return 1;
		}
		// give-box (tra loai 1/2/3)
		if (cap.uBoxSeq != ea.uDTBoxSeen)
		{
			ea.uDTBoxSeen = cap.uBoxSeq;
			ea.nDTPhase = DTP_GIVEBOX;
			ea.nDTRetry = 0;
			ea.uDTNext = uCurTime + 500;
			return 1;
		}
		if (cap.uTalkSeq != ea.uDTTalkSeen)
		{
			ea.uDTTalkSeen = cap.uTalkSeq;
			if (DT_Has(cap.szTalk, DTM_MSG_PUNISH))
				return DT_Hold(nPlayerIdx, "<color=Orange>BÞ D· TÈu ph¹t v× hñy nhiÒu - chê hÕt ph¹t sÏ lµm tiÕp.", uCurTime, 11 * 60 * 1000);
		}
		if (cap.uDlgSeq == ea.uDTDlgSeen)
		{
			// moi vong ~250ms; 16 vong (~4s) khong co phan hoi -> go lai; 60 vong -> treo
			if (++ea.nDTRetry > 60)
				return DT_Hold(nPlayerIdx, "<color=Red>NPC kh«ng tr¶ lêi héi tho¹i.", uCurTime, 5 * 60 * 1000);
			if ((ea.nDTRetry % 16) == 0)
			{
				ea.nDTPhase = DTP_GOTONPC;
				ea.uDTNext = uCurTime + 900;
			}
			return 1;
		}
		ea.uDTDlgSeen = cap.uDlgSeq;
		ea.nDTRetry = 0;
		// gia dinh se nhan dang duoc -> nDTUnknown=0; nhanh unknown cuoi ham khoi phuc+tang.
		// KHONG reset vo dieu kien o day (se tai lap bug DT-1: dialog la mo lai reset mai).
		int nUnkSave = ea.nDTUnknown;
		ea.nDTUnknown = 0;

		char szBuf[2048];
		char* apAns[16];
		g_StrCpyLen(szBuf, cap.szDlg, sizeof(szBuf));
		int nAns = DT_Split(szBuf, apAns, 16);
		const char* szQ = szBuf;

		// het 40 nhiem vu / ngay
		if (DT_Has(szQ, DTM_MSG_LIMIT))
		{
			const int bLanDauNgay = (ea.nDTDoneDay != nToday);
			ea.nDTDoneDay = nToday;
			// (20/08) treo co han: server reset la chay tiep ngay.
			DT_Hold(nPlayerIdx, "<color=AYellow>TuyÖt! §· ®ñ 40 nhiÖm vô D· TÈu h«m nay - nghØ, qua ngµy auto tù ch¹y l¹i.", uCurTime, 60 * 60 * 1000);
			ea.nDTHoldFreeze = 1;	// danh dau hold "du 40" (han che quet/thu lai)
			// (r3) NHA MAY + danh thuc Hau can. (r4 - PB R4) CHI danh thuc khi may da
			// xong/chet (nHomeStep>12) hoac lan dau cham tran trong ngay - probe moi
			// gio khong duoc de trang thai GIUA CHUYEN cua Hau can (rut tien lai...).
			if (bLanDauNgay || ea.nHomeStep > 12)
			{
				ea.nHomeStep = 0;
				ea.nSubStep = 0;
			}
			ea.nDTEngaged = 0;
			return 0;
		}
		// tui day
		if (DT_Has(szQ, DTM_MSG_BAGFULL))
		{
			// (21/08) moc set doi NHIEU hon 5 o ("Hanh trang da day, can it nhat N o trong" -
			// seasonnpc.lua:463, N toi 5+24): doc N de ban cho du, khong thi SELLJUNK du 8 o
			// thoat ngay -> NPC lai bao day -> lap vo tan. Ngoai moc N = 5 nhu cu.
			char szDay[160];
			const int nCanO = DT_NumAfter(szQ, DTM_MSG_BAGFULL);
			const int nCanOk = (nCanO > 5 && nCanO <= 40) ? nCanO : 5;	// tui 60 o, server hien toi 29
			sprintf(szDay, "<color=Yellow>Tói ®Çy (cÇn Ýt nhÊt %d « trèng) - auto dän bít råi ch¹y tiÕp.", nCanOk);
			const int nRet5 = DT_BagRelease(nPlayerIdx, pAp, uCurTime, szDay);
			if (nCanOk > 5)
			{
				g_nDTSellMin = nCanOk;
				g_nDTSellNeed = nCanOk + 2;
			}
			return nRet5;
		}
		// tra nhiem vu that bai (chua du yeu cau)
		if (DT_Has(szQ, DTM_MSG_FAILREQ) || DT_Has(szQ, DTM_MSG_FAILSHXT))
		{
			g_dwDTKhoePend = 0;	// (21/08) tra truot = mon khoe chua xong, chua cat
			if (ea.nDTQType == 3)
				g_dwDTKhoeId = 0;	// (phong cua so thuong TRE cua nhiem vu truoc chot nham mon dang can tra)
			// (r5) thu do ket trong hop giao ve tui truoc (server khong dong hop
			// khi tu choi) - khong thu thi moi bo quet se tuong MAT do.
			// (PB r5b) moi lan 1 mon + kiem cho; -1 = tui thieu cho -> giu trong hop.
			int nThu5 = DT_ThuHoiBox(nPlayerIdx);
			if (nThu5 < 0)
				DT_Msg(nPlayerIdx, "<color=Yellow>§å trong hép giao ch­a lÊy ra ®­îc (tói thiÕu chç) - sÏ tù lÊy sau khi dän tói.");
			else if (nThu5 > 0)
				DT_Msg(nPlayerIdx, "<color=Cyan>Tr¶ tr­ît - thu l¹i ®å trong hép giao vÒ tói.");
			// (r3) loai 4 tra som ma chua du (tin nhan troi) -> quay lai map danh tiep,
			// dung de roi xuong skip/treo lam mat nhiem vu dang lam do.
			if (ea.nDTQType == 4 && ea.nDTMapId > 0)
			{
				DT_Msg(nPlayerIdx, "<color=Cyan>Ch­a ®ñ m¶nh - quay l¹i map ®¸nh tiÕp...");
				ea.nDTStep = DTI_NONE;
				ea.nDTPhase = DTP_EXEC;
				ea.uDTNext = uCurTime + 800;
				return 1;
			}
			if (ea.nDTQType == 5 && ea.nDTStatType == 4)
			{
				// phuc duyen: dung them item roi thu lai
				ea.nDTPhase = DTP_USEPD;
				ea.uDTNext = uCurTime + 800;
				return 1;
			}
			if ((ea.nDTQType == 1 || ea.nDTQType == 2 || ea.nDTQType == 3)
			&& ea.nDTCandCur + 1 < ea.nDTCandNum)
			{
				++ea.nDTCandCur;	// thu ung vien ke (ten trung nhau khac level/he)
				ea.nDTStep = DTI_NONE;
				ea.nDTPhase = DTP_GOTONPC;
				ea.uDTNext = uCurTime + 800;
				return 1;
			}
			if (ea.nDTQType == 5 && ea.nDTStatType == 2 && ea.nDTStep == DTI_TURNIN)
			{
				// exp chua du (mat baseline sau crash) - cay tiep
				g_dDTExpGain = 0;
				g_dDTPrevExp = -1;
				ea.uDTFarmStall = uCurTime;
				ea.nDTPhase = DTP_FARM;
				ea.nDTEngaged = 0;
				DT_Msg(nPlayerIdx, "<color=Cyan>Ch­a ®ñ kinh nghiÖm - cµy tiÕp...");
				return 0;
			}
			return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Orange>Kh«ng ®¸p øng ®­îc nhiÖm vô nµy.");
		}
		// hop xac nhan huy - het luot (3 lua chon, co option 100 manh)
		if ((idx = DT_FindAns(apAns, nAns, DTM_OPT_CANCEL2)) >= 0)
		{
			if (pAp->nDTCancelMode == 2)
			{
				DT_Answer(nPlayerIdx, idx);
				ea.nDTStep = DTI_CANCELWAIT;
				ea.uDTNext = uCurTime + 900;
				return 1;
			}
			if (pAp->nDTCancelMode == 1
			&& (idx = DT_FindAns(apAns, nAns, DTM_OPT_NORMALCANCEL)) >= 0)
			{
				DT_Answer(nPlayerIdx, idx);
				ea.nDTStep = DTI_CANCELWAIT;
				ea.uDTNext = uCurTime + 900;
				return 1;
			}
			DT_Answer(nPlayerIdx, nAns - 1);	// "de ta suy nghi lai" - thoat
			return DT_Hold(nPlayerIdx, "<color=Orange>HÕt l­ît hñy nhiÖm vô.", uCurTime, 15 * 60 * 1000);
		}
		// hop xac nhan huy (con luot / xac nhan lan 2)
		if ((idx = DT_FindAns(apAns, nAns, DTM_OPT_CANCEL1A)) >= 0
		|| (idx = DT_FindAns(apAns, nAns, DTM_OPT_CANCEL1B)) >= 0)
		{
			DT_Answer(nPlayerIdx, idx);
			ea.nDTStep = DTI_NONE;
			ea.nDTQType = 0;
			ea.nDTPhase = DTP_GOTONPC;	// nhiem vu moi se duoc chia, quay lai noi chuyen
			ea.uDTNext = uCurTime + 1200;
			return 1;
		}
		// menu xa phu
		if ((idx = DT_FindAns(apAns, nAns, DTM_OPT_GODATAU)) >= 0)
		{
			DT_Answer(nPlayerIdx, idx);
			ea.nDTPhase = DTP_XAFUTALK_DONE;
			ea.uDTNext = uCurTime + 1500;
			return 1;
		}
		// gioi thieu lan dau (course 0) -> nhan
		if ((idx = DT_FindAns(apAns, nAns, DTM_OPT_CONFIRM)) >= 0)
		{
			DT_Answer(nPlayerIdx, idx);
			ea.nDTPhase = DTP_GOTONPC;
			ea.uDTNext = uCurTime + 1200;
			return 1;
		}
		// course 3 -> nhan nhiem vu tiep
		if ((idx = DT_FindAns(apAns, nAns, DTM_OPT_TASKPROCESS)) >= 0)
		{
			DT_Answer(nPlayerIdx, idx);
			ea.nDTStep = DTI_NONE;
			ea.nDTQType = 0;
			ea.nDTPhase = DTP_GOTONPC;
			ea.uDTNext = uCurTime + 1200;
			return 1;
		}
		// hop thoai chinh course 1
		if ((idx = DT_FindAns(apAns, nAns, DTM_OPT_ACCEPT)) >= 0)
		{
			// giu con tro ung vien neu van la nhiem vu cu (dang thu ung vien ke)
			int nPrevType = ea.nDTQType;
			int nPrevCand0 = (ea.nDTCandNum > 0) ? ea.nDTCand[0] : -1;
			int nPrevCur = ea.nDTCandCur;
			DT_ParseQuest(nPlayerIdx, szQ);
			if (ea.nDTQType == nPrevType && ea.nDTCandNum > 0
			&& ea.nDTCand[0] == nPrevCand0 && nPrevCur < ea.nDTCandNum)
				ea.nDTCandCur = nPrevCur;
			else
			{
				ea.nDTLBTry = 0;	// (20/08) nhiem vu MOI: cho phep dung lenh bai lai
				ea.nDTShopTry = 0;
				// (r4) baseline tien do CHI khi nhiem vu moi (PB R3)
				ea.nDTProg = -1;
				ea.uDTMsgSeen = cap.uMsgSeq;
				ea.uDTTienSeen = cap.uTienSeq;
				ea.nDTHaveBase = 0;	// cho phep "ve thu tra som" lai
			}
			if (ea.nDTQType <= 0)
				return DT_Hold(nPlayerIdx, "<color=Red>Kh«ng hiÓu néi dung nhiÖm vô (l¹) - h·y b¸o l¹i lçi nµy.", uCurTime, 10 * 60 * 1000);
			// loai bi nguoi choi tat?
			if (!pAp->bDTType[ea.nDTQType - 1])
			{
				// (20/08) bat o "Dung Lenh bai hoan thanh": co item (6,1,4818) trong tui
				// thi dung ngay (server hoan thanh + bung ruong thuong - uFinSeq bat duoc);
				// nam trong ruong thi keo ve (chi can mat khau). Dung 2 lan ma khong thay
				// thuong (hoi thoai loai-tat lai hien) coi nhu server chua co script -> thoi.
				if (pAp->bDTLenhBai == 1 && ea.nDTLBTry < 2)
				{
					int nLBPos = 0;
					int nLB = DT_FindItemRule(nPlayerIdx,
						(Player[nPlayerIdx].m_CUnlocked || pAp->szBoxPass[0]) != 0, false,
						6, 1, 4818, -1, -1, 0, 0, 0, &nLBPos);
					if (nLB && nLBPos == pos_equiproom)
					{
						// (PB V19) server doi >= 5 o trong moi phat thuong lenh bai (Talk tu
						// choi thi client khong doc duoc) - kiem truoc, thieu thi don tui da.
						if (Player[nPlayerIdx].m_ItemList.CalcFreeItemCellCount(1, 1, room_equipment) < 5)
							return DT_BagRelease(nPlayerIdx, pAp, uCurTime, "<color=Yellow>CÇn Ýt nhÊt 5 « trèng ®Ó nhËn th­ëng lÖnh bµi - dän tói tr­íc.");
						CoreDataChanged(GDCNI_UI_ACT, 1, 0);
						++ea.nDTLBTry;
						Player[nPlayerIdx].m_ItemList.AutoUseItem(6, 1, 4818, nPlayerIdx);
						DT_Msg(nPlayerIdx, "<color=AYellow>Dïng LÖnh bµi hoµn thµnh D· TÈu - chê nhËn th­ëng...");
						ea.nDTStep = DTI_NONE;
						ea.nDTPhase = DTP_WAITDLG;
						ea.uDTNext = uCurTime + 1800;
						return 1;
					}
					if (nLB && nLBPos != pos_equiproom && ea.nDTShopTry < 12)
					{
						++ea.nDTShopTry;
						if (DT_EnsureUnlock(nPlayerIdx, pAp, uCurTime))
						{
							if (Player[nPlayerIdx].m_ItemList.CalcFreeItemCellCount(
								Item[nLB].GetWidth(), Item[nLB].GetHeight(), room_equipment) >= 1)
								DT_BoxToBag(nLB, nLBPos);
						}
						ea.uDTNext = uCurTime + 1100;
						return 1;	// tick sau item ve tui se dung
					}
				}
				else if (pAp->bDTLenhBai == 1 && ea.nDTLBTry >= 2)
					DT_Msg(nPlayerIdx, "<color=Red>LÖnh bµi hoµn thµnh kh«ng cã t¸c dông (server ch­a cËp nhËt script?) - lµm theo cÊu h×nh.");
				int nCanIdx = DT_FindAns(apAns, nAns, DTM_OPT_CANCELCONF);
				if (pAp->nDTSkipMode == 1 && nCanIdx >= 0)
				{
					DT_Answer(nPlayerIdx, nCanIdx);
					ea.nDTStep = DTI_CANCELWAIT;
					ea.uDTNext = uCurTime + 900;
					return 1;
				}
				return DT_Hold(nPlayerIdx, "<color=Yellow>Lo¹i nhiÖm vô nµy ®ang <color=Red>t¾t<color> trong tab D· TÈu - treo chê.", uCurTime, 15 * 60 * 1000);
			}
			// den de tra?
			if (ea.nDTStep == DTI_TURNIN)
			{
				if (Player[nPlayerIdx].m_ItemList.CalcFreeItemCellCount(1, 1, room_equipment) < 5)
					return DT_BagRelease(nPlayerIdx, pAp, uCurTime, "<color=Yellow>CÇn Ýt nhÊt 5 « trèng ®Ó tr¶ nhiÖm vô - h·y dän tói.");
				DT_Answer(nPlayerIdx, idx);
				ea.nDTStep = DTI_TURNWAIT;
				ea.uDTNext = uCurTime + 900;
				return 1;
			}
			// muon huy (tu DT_Skip)?
			if (ea.nDTStep == DTI_CANCEL)
			{
				int nCanIdx = DT_FindAns(apAns, nAns, DTM_OPT_CANCELCONF);
				if (nCanIdx >= 0)
				{
					DT_Answer(nPlayerIdx, nCanIdx);
					ea.nDTStep = DTI_CANCELWAIT;
					ea.uDTNext = uCurTime + 900;
					return 1;
				}
				return DT_Hold(nPlayerIdx, "<color=Red>Kh«ng thÊy nót hñy trong héi tho¹i.", uCurTime, 10 * 60 * 1000);
			}
			// nhiem vu moi -> dong khung thoai (khong can tra loi - nhiem vu da duoc gan)
			CoreDataChanged(GDCNI_UI_ACT, 1, 0);
			ea.nDTPhase = DTP_EXEC;
			ea.uDTNext = uCurTime + 300;
			return 1;
		}
		// hop thoai la (khong khop marker nao). Khoi phuc bo dem cu roi tang - KHONG bi
		// reset boi new-dialog nen chot nay khong bi vo hieu khi NPC lien tuc tra hoi thoai la (DT-1).
		ea.nDTUnknown = nUnkSave + 1;
		if (ea.nDTUnknown > 6)
			return DT_Hold(nPlayerIdx, "<color=Red>Héi tho¹i kh«ng nhËn d¹ng ®­îc (server ®æi lêi tho¹i?).", uCurTime, 5 * 60 * 1000);
		CoreDataChanged(GDCNI_UI_ACT, 1, 0);
		ea.nDTPhase = DTP_GOTONPC;
		ea.uDTNext = uCurTime + 900;
		return 1;
	}

	case DTP_EXEC:
	{
		ea.nDTEngaged = 1;
		switch (ea.nDTQType)
		{
		case 1:
		case 2:
		case 3:
		{
			if (ea.nDTCandNum <= 0)
				return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Red>Kh«ng khíp ®­îc dßng b¶ng d÷ liÖu nhiÖm vô.");
			int nPos = 0;
			int nItem = DT_FindCandItem(nPlayerIdx, pAp, &nPos);
			if (!nItem && ea.nDTCandCur + 1 < ea.nDTCandNum)
			{
				// thu cac ung vien khac truoc khi bo cuoc
				for (i = ea.nDTCandCur + 1; i < ea.nDTCandNum; ++i)
				{
					ea.nDTCandCur = i;
					nItem = DT_FindCandItem(nPlayerIdx, pAp, &nPos);
					if (nItem)
						break;
				}
				if (!nItem)
					ea.nDTCandCur = 0;
			}
			if (nItem)
			{
				if (nPos != pos_equiproom)
				{
					// keo tu ruong ve tui
					if (!pAp->bDTUseBox)
						return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Yellow>§å cÇn tr¶ ®ang n»m trong r­¬ng - h·y bËt « <color=White>LÊy ®å/tiÒn tõ r­¬ng<color> ë tab D· TÈu.");
					if (!DT_EnsureUnlock(nPlayerIdx, pAp, uCurTime))
					{
						if (++ea.nDTRetry > 8)
							return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Yellow>Kh«ng më khãa ®­îc r­¬ng - kiÓm tra mËt khÈu r­¬ng ë tab HËu cÇn.");
						return 1;
					}
					if (Player[nPlayerIdx].m_ItemList.CalcFreeItemCellCount(
						Item[nItem].GetWidth(), Item[nItem].GetHeight(), room_equipment) < 1)
						return DT_BagRelease(nPlayerIdx, pAp, uCurTime, "<color=Yellow>Tói kh«ng ®ñ chç ®Ó lÊy ®å tõ r­¬ng - h·y dän tói.");
					DT_BoxToBag(nItem, nPos);
					ea.uDTNext = uCurTime + 1200;
					return 1;	// tick sau se thay item trong tui
				}
				// co do trong tui -> di tra
				ea.nDTItemIdx = nItem;
				ea.nDTStep = DTI_TURNIN;
				ea.nDTPhase = DTP_GOTONPC;
				ea.nDTRetry = 0;
				return 1;
			}
			// chua co do
			if (ea.nDTQType == 1)
			{
				// mua: chi ho tro tiem tap hoa (detail 5/6/8); vu khi/ngua -> ket
				const DTBuyRow& r = g_DTBuy[ea.nDTCand[ea.nDTCandCur]];
				if (r.nDetail == 5 || r.nDetail == 6 || r.nDetail == 8)
				{
					ea.nDTPhase = DTP_GOSHOP;
					ea.nDTShopTry = 0;
					ea.nDTRetry = 0;
					return 1;
				}
				return DT_Skip(nPlayerIdx, pAp, uCurTime,
					"<color=Orange>NhiÖm vô cÇn vò khÝ/ngùa - h·y bá s½n mãn ®ã vµo r­¬ng (xem tªn trong nhiÖm vô).");
			}
			if (ea.nDTQType == 2)
			{
				// (20/08) bat o "Mua o sap": di cho xem sap nguoi ban (that + bot) de mua
				if (pAp->bDTMuaSap == 1)
				{
					g_nDTSapMask = 0;
					g_nDTSapDone = 0;
					g_dwDTSapCur = 0;
					g_nDTSapWpt = 0;
					g_uDTSapWptT = 0;
					g_uDTSapDwell = 0;
					g_nDTSapXem = 0;
					g_nDTSapCam = 0;
					DT_SapDsReset();	// (r5h) chuyen di cho moi - danh ba cu het hieu luc
					ea.nDTPhase = DTP_MUASAP;
					ea.nDTRetry = 0;
					ea.uDTHoldUntil = uCurTime + 25u * 60u * 1000u;	// han toan cuoc di cho
					DT_Msg(nPlayerIdx, "<color=Cyan>Kh«ng cã ®å cÇn t×m trong tói/r­¬ng - ®i xem s¹p ng­êi b¸n ®Ó mua...");
					return 1;
				}
				return DT_Skip(nPlayerIdx, pAp, uCurTime,
					"<color=Orange>Kh«ng cã ®å cÇn t×m trong tói/r­¬ng - nªn tÝch tr÷ s½n trang søc.");
			}
			// (r5 - nguoi dung) loai 3 "Tim trang bi (khoe)" cung DI CHO mua o sap nhu
			// loai 2: quet sap thanh nay, het thi Xa Phu qua du 10 thanh/thon, tim het
			// ma khong co MOI treo. (Do khoe tra xong duoc hoan lai - mua cang loi.)
			if (ea.nDTQType == 3 && pAp->bDTMuaSap == 1)
			{
				g_nDTSapMask = 0;
				g_nDTSapDone = 0;
				g_dwDTSapCur = 0;
				g_nDTSapWpt = 0;
				g_uDTSapWptT = 0;
				g_uDTSapDwell = 0;
				g_nDTSapXem = 0;
				g_nDTSapCam = 0;
				DT_SapDsReset();	// (r5h) chuyen di cho moi - danh ba cu het hieu luc
				ea.nDTPhase = DTP_MUASAP;
				ea.nDTRetry = 0;
				ea.uDTHoldUntil = uCurTime + 25u * 60u * 1000u;
				DT_Msg(nPlayerIdx, "<color=Cyan>Kh«ng cã trang bÞ cÇn khoe trong tói/r­¬ng - ®i xem s¹p ng­êi b¸n ®Ó mua...");
				return 1;
			}
			return DT_Skip(nPlayerIdx, pAp, uCurTime,
				"<color=Orange>Kh«ng cã trang bÞ cÇn t×m (khoe) phï hîp trong tói/r­¬ng.");
		}
		case 4:
		{
			if (ea.nDTMapId <= 0 || ea.nDTReqNum <= 0)
				return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Red>Kh«ng ®äc ®­îc tªn map / sè l­îng cña nhiÖm vô lo¹i 4 (®¸nh qu¸i).");
			if (!pAp->bFight)
				return DT_Hold(nPlayerIdx, "<color=Yellow>NhiÖm vô ®¸nh qu¸i: h·y bËt « <color=White>§¸nh qu¸i<color> ë tab ChiÕn ®Êu.", uCurTime, 10 * 60 * 1000);
			// (r4 - PB R3) KHONG xoa nDTProg/uDTMsgSeen o day nua: quay lai CUNG nhiem
			// vu (chet/teleport/tra som truot) phai giu tien do; baseline chi dat khi
			// nhan nhiem vu MOI (nhanh else cua WAITDLG).
			// (20/08) chuan bi phu ve: khong tu mua nua - thieu thi keo tu ruong,
			// khong co luon thi CHI NHAC roi van di lam (luc ve se xu ly tiep)
			if (!DT_HasPortalInBag(nPlayerIdx))
			{
				// (r4 - PB R8) nap tran: keo/mo khoa qua 10 nhip khong xong thi di lam
				// luon (ve se xu ly tiep) - khong dung ruc o day vi pass sai.
				if (++ea.nDTRetry <= 10 && DT_PortalPull(nPlayerIdx, pAp, uCurTime))
				{
					ea.uDTNext = uCurTime + 1200;
					return 1;	// dang keo phu tu ruong ve tui
				}
				DT_Msg(nPlayerIdx, "<color=Yellow>Ch­a cã phï vÒ thµnh trong tói - nªn bá s½n Thæ §Þa Phï hoÆc phï v« h¹n vµo tói/r­¬ng.");
			}
			ea.uDTFarmStall = uCurTime;
			if (nMap == ea.nDTMapId)
			{
				ea.nDTPhase = DTP_FARM;
				ea.nDTEngaged = 2;
				return 2;
			}
			ea.nDTPhase = DTP_GOXAFU;
			ea.nDTXaFuTry = 0;
			ea.nDTRetry = 0;
			return 1;
		}
		case 5:
		{
			if (ea.nDTStatType == 2)
			{
				g_dDTPrevExp = Player[nPlayerIdx].m_nExp;
				g_dDTExpGain = 0;
				ea.uDTFarmStall = uCurTime;
				DT_Msg(nPlayerIdx, "<color=Cyan>NhiÖm vô kinh nghiÖm: th¶ cho auto th­êng cµy, ®ñ sÏ tù vÒ tr¶.");
				ea.nDTPhase = DTP_FARM;
				ea.nDTEngaged = 0;
				return 0;
			}
			if (ea.nDTStatType == 4)
			{
				ea.nDTUsedPD = 0;
				ea.nDTRetry = 0;	// (r4) moc cho nap mo-khoa trong USEPD
				ea.nDTFuYuanPrev = Npc[Player[nPlayerIdx].m_nIndex].nFuYuan;
				ea.nDTPhase = DTP_USEPD;
				return 1;
			}
			// danh vong / PK / Tong Kim: thu tra 1 lan (nho da du), fail se skip
			ea.nDTStep = DTI_TURNIN;
			ea.nDTPhase = DTP_GOTONPC;
			return 1;
		}
		case 6:
		{
			ea.nDTStep = DTI_TURNIN;	// thu tra; thieu manh -> fail -> skip
			ea.nDTPhase = DTP_GOTONPC;
			return 1;
		}
		}
		return DT_Hold(nPlayerIdx, "<color=Red>Lo¹i nhiÖm vô l¹ - auto ch­a hç trî.", uCurTime, 10 * 60 * 1000);
	}

	case DTP_GOSHOP:
	{
		ea.nDTEngaged = 1;
		std::map<int, StationVector>::iterator it = g_ShopStation.find(nMap);
		if (it == g_ShopStation.end() || it->second.empty())
			return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Red>Thµnh nµy ch­a cã täa ®é tiÖm t¹p hãa.");
		// du tien chua? (uoc luong 100k; thieu thi rut)
		if (Player[nPlayerIdx].m_ItemList.GetEquipmentMoney() < 100000 && pAp->bDTUseBox
		&& pAp->nDTWDMoney > 0)
		{
			if (DT_EnsureUnlock(nPlayerIdx, pAp, uCurTime))
			{
				Player[nPlayerIdx].m_ItemList.ExchangeMoney(room_repository, room_equipment,
					pAp->nDTWDMoney * 10000);
				ea.uDTNext = uCurTime + 1000;
			}
		}
		sStation& s = it->second[0];
		if (DT_WalkTo(nPlayerIdx, s.x, s.y, 250, uCurTime))
		{
			ea.nDTPhase = DTP_SHOPTALK;
			ea.nDTRetry = 0;
		}
		return 1;
	}

	case DTP_SHOPTALK:
	{
		ea.nDTEngaged = 1;
		// cua so shop da mo?
		if (CoreDataChanged(GDCNI_UI_ACT, 2, 0))
		{
			ea.nDTPhase = DTP_BUY;
			ea.nDTRetry = 0;
			return 1;
		}
		// hoi thoai dang mo -> chon option giao dich (thu lan luot)
		if (cap.uDlgSeq != ea.uDTDlgSeen)
		{
			ea.uDTDlgSeen = cap.uDlgSeq;
			char szBuf[2048];
			char* apAns[16];
			g_StrCpyLen(szBuf, cap.szDlg, sizeof(szBuf));
			int nAns = DT_Split(szBuf, apAns, 16);
			int nPick = ea.nDTShopTry;
			if (nPick >= nAns)
				return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Orange>TiÖm kh«ng cã môc giao dÞch phï hîp.");
			DT_Answer(nPlayerIdx, nPick);
			++ea.nDTShopTry;
			ea.uDTNext = uCurTime + 900;
			return 1;
		}
		// tim chu tiem tap hoa gan diem shop va mo thoai
		std::map<int, StationVector>::iterator it = g_ShopStation.find(nMap);
		if (it == g_ShopStation.end() || it->second.empty())
			return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Red>MÊt täa ®é tiÖm t¹p hãa.");
		sStation& s = it->second[0];
		int nIdx = DT_FindNpcName(nPlayerIdx, "t¹p h", s.x, s.y, 300);
		if (!nIdx)
			nIdx = DT_FindNpcName(nPlayerIdx, "t¹p h", s.x, s.y, 600);
		if (nIdx)
		{
			int nX, nY, dX, dY;
			Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nX, &nY);
			Npc[nIdx].GetMpsPos(&dX, &dY);
			if (g_GetDistance(nX, nY, dX, dY) <= 128)
			{
				ea.uDTDlgSeen = cap.uDlgSeq;
				Player[nPlayerIdx].DialogNpc(nIdx);
				ea.uDTNext = uCurTime + 800;
				if (++ea.nDTRetry > 10)
					return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Orange>Kh«ng më ®­îc cöa sæ tiÖm t¹p hãa.");
				return 1;
			}
			DT_WalkTo(nPlayerIdx, dX, dY, 96, uCurTime);
			return 1;
		}
		if (++ea.nDTRetry > 10)
			return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Orange>Kh«ng thÊy chñ tiÖm t¹p hãa.");
		return 1;
	}

	case DTP_BUY:
	{
		ea.nDTEngaged = 1;
		if (!CoreDataChanged(GDCNI_UI_ACT, 2, 0))
		{
			ea.nDTPhase = DTP_SHOPTALK;
			return 1;
		}
		const DTBuyRow& r = g_DTBuy[ea.nDTCand[ea.nDTCandCur]];
		// da mua duoc chua?
		int nPos = 0;
		int nItem = DT_FindItemRule(nPlayerIdx, false, true, r.nGenre, r.nDetail, r.nParticular,
			r.nLevel, r.nFive, 0, 0, 0, &nPos);
		if (nItem)
		{
			CoreDataChanged(GDCNI_UI_ACT, 3, 0);	// dong shop
			ea.nDTItemIdx = nItem;
			ea.nDTStep = DTI_TURNIN;
			ea.nDTPhase = DTP_GOTONPC;
			ea.nDTRetry = 0;
			return 1;
		}
		for (int b = 0; b < BuySell.GetWidth(); ++b)
		{
			KItem* pItem = BuySell.GetItem(BuySell.GetItemIndex(
				Player[nPlayerIdx].m_BuyInfo.m_nShopIdx[Player[nPlayerIdx].m_BuyInfo.m_nCurShop], b));
			if (!pItem)
				break;
			if (pItem->GetGenre() == r.nGenre && pItem->GetDetailType() == r.nDetail
			&& pItem->GetParticular() == r.nParticular && pItem->GetLevel() == r.nLevel
			&& pItem->GetSeries() == r.nFive)
			{
				if (Player[nPlayerIdx].m_ItemList.GetEquipmentMoney() < pItem->GetPrice())
				{
					if (pAp->bDTUseBox && pAp->nDTWDMoney > 0 && DT_EnsureUnlock(nPlayerIdx, pAp, uCurTime))
					{
						Player[nPlayerIdx].m_ItemList.ExchangeMoney(room_repository, room_equipment,
							pAp->nDTWDMoney * 10000);
						ea.uDTNext = uCurTime + 1000;
						return 1;
					}
					CoreDataChanged(GDCNI_UI_ACT, 3, 0);
					return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Yellow>Kh«ng ®ñ tiÒn mua ®å nhiÖm vô.");
				}
				int x, y;
				if (!Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
					pItem->GetWidth(), pItem->GetHeight(), &x, &y))
				{
					CoreDataChanged(GDCNI_UI_ACT, 3, 0);
					return DT_BagRelease(nPlayerIdx, pAp, uCurTime, "<color=Yellow>Tói ®Çy, kh«ng mua ®­îc ®å.");
				}
				SendClientCmdBuy(Player[nPlayerIdx].m_BuyInfo.m_nCurShop, b, 1, 0);
				ea.uDTNext = uCurTime + 1200;
				return 1;
			}
		}
		// shop nay khong co mon can -> dong, thu option khac cua tiem
		CoreDataChanged(GDCNI_UI_ACT, 3, 0);
		if (ea.nDTShopTry >= 4)
			return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Orange>TiÖm kh«ng b¸n mãn cÇn mua.");
		ea.nDTPhase = DTP_SHOPTALK;
		ea.uDTNext = uCurTime + 700;
		return 1;
	}

	case DTP_GOXAFU:
	{
		ea.nDTEngaged = 1;
		std::map<int, StationVector>::iterator it = g_MoveStation.find(nMap);
		if (it == g_MoveStation.end() || it->second.empty())
		{
			// Dang o map KHONG co Xa Phu (len nham map nhiem vu / bi keo di dau do):
			// dung Tho Dia Phu ve thanh roi ra Xa Phu di lai cho dung map.
			if (ea.nDTMapId > 0 && ea.nDTQType == 4)
			{
				DT_Msg(nPlayerIdx, "<color=Cyan>Kh«ng ph¶i map nhiÖm vô - dïng Thæ §Þa Phï vÒ thµnh ®i l¹i.");
				ea.nDTBackXaFu = 1;
				ea.nDTPhase = DTP_RETURN;
				ea.nDTRetry = 0;
				return 1;
			}
			return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Red>Thµnh nµy ch­a cã täa ®é Xa Phu.");
		}
		sStation& s = it->second[0];
		int nIdx = DT_FindNpcName(nPlayerIdx, "xa phu", s.x, s.y, 400);
		if (nIdx)
		{
			int nX, nY, dX, dY;
			Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nX, &nY);
			Npc[nIdx].GetMpsPos(&dX, &dY);
			if (g_GetDistance(nX, nY, dX, dY) <= 128)
			{
				ea.uDTDlgSeen = cap.uDlgSeq;
				Player[nPlayerIdx].DialogNpc(nIdx);
				ea.nDTPhase = DTP_WAITDLG;	// menu xa phu se duoc nhan dang (OPT_GODATAU)
				ea.uDTNext = uCurTime + 800;
				return 1;
			}
			DT_WalkTo(nPlayerIdx, dX, dY, 96, uCurTime);
			return 1;
		}
		if (DT_WalkTo(nPlayerIdx, s.x, s.y, 200, uCurTime))
		{
			if (++ea.nDTRetry > 15)
				return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Orange>Kh«ng thÊy Xa Phu.");
		}
		return 1;
	}

	case DTP_XAFUTALK_DONE:
	{
		// da chon "den noi lam nhiem vu da tau" - cho chuyen map
		ea.nDTEngaged = 1;
		if (nMap == ea.nDTMapId)
		{
			ea.nDTPhase = DTP_FARM;
			ea.nDTEngaged = 2;
			ea.nDTRetry = 0;
			ea.nDTXaFuTry = 0;
			ea.uDTFarmStall = uCurTime;	// ARM watchdog T4 (0 se TAT han - DT-3)
			ea.nDTRoamStep = 0;
			ea.uDTRoamNext = 0;	// bat dau tu cum quai gan neo nhat
			return 2;
		}
		// Xa Phu da cho RA KHOI THANH (map hien tai khong co tram xe): TIN MAY CHU -
		// day chinh la map nhiem vu. Id map doc tu VAN BAN nhiem vu co the lech (map
		// nhieu tang), lay id THAT lam chuan roi farm luon - KHONG phu ve giua chung.
		if (g_MoveStation.find(nMap) == g_MoveStation.end())
		{
			DT_Msg(nPlayerIdx, "<color=Green>§· tíi map nhiÖm vô - b¾t ®Çu ®¸nh qu¸i nhÆt cuén.");
			ea.nDTMapId = nMap;
			ea.nDTPhase = DTP_FARM;
			ea.nDTEngaged = 2;
			ea.nDTRetry = 0;
			ea.nDTXaFuTry = 0;
			ea.uDTFarmStall = uCurTime;
			ea.nDTRoamStep = 0;
			ea.uDTRoamNext = 0;
			return 2;
		}
		if (++ea.nDTRetry > 12)
		{
			// van dung trong thanh: xa phu khong chuyen (server chua thay nhiem vu?)
			if (++ea.nDTXaFuTry > 5)
				return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Red>Xa Phu kh«ng chë ®i map nhiÖm vô (kiÓm tra l¹i nhiÖm vô).");
			ea.nDTPhase = DTP_GOXAFU;
			ea.nDTRetry = 0;
		}
		ea.uDTNext = uCurTime + 700;
		return 1;
	}

	case DTP_FARM:
	{
		// (T5-exp da xu ly o tren; day la T4)
		// (r4 - PB R3) DOC TIN TRUOC MOI THU - ke ca khi vua bi bat ra khoi map,
		// tin chot "du manh" van phai duoc tieu hoa truoc khi doi pha.
		// Kenh RIENG cho tin tien do (PB R2 - spam thong bao toan server khong de duoc):
		if (cap.uTienSeq != ea.uDTTienSeen)
		{
			ea.uDTTienSeen = cap.uTienSeq;
			if (DT_Has(cap.szTien, DTM_MSG_TAM_PRE) && DT_Has(cap.szTien, DTM_MSG_TONGCONG))
			{
				int nT = DT_NumAfter(cap.szTien, DTM_MSG_TONGCONG);
				if (nT > ea.nDTProg)
				{
					ea.nDTProg = nT;
					ea.uDTFarmStall = uCurTime;
				}
			}
		}
		// vong khe chung (du phong; PB R7: clamp dung la >4/-4 - 4 khe giu du 4 tin)
		while (cap.uMsgSeq != ea.uDTMsgSeen)
		{
			if (cap.uMsgSeq - ea.uDTMsgSeen > 4)
				ea.uDTMsgSeen = cap.uMsgSeq - 4;	// qua cu, khe da bi de - nhay toi
			++ea.uDTMsgSeen;
			const char* szDTM = cap.aMsg[ea.uDTMsgSeen & 3];
			if (DT_Has(szDTM, DTM_MSG_TAM_PRE) && DT_Has(szDTM, DTM_MSG_TONGCONG))
			{
				int n = DT_NumAfter(szDTM, DTM_MSG_TONGCONG);
				if (n > ea.nDTProg)
				{
					ea.nDTProg = n;
					ea.uDTFarmStall = uCurTime;	// co tien do -> reset dong ho stall
				}
			}
		}
		if (nMap != ea.nDTMapId)
		{
			// bi ra khoi map (chet / teleport / TP mau-thuoc-tien-do hong cua tab Hau can) -
			// quay lai (tin da doc xong o tren). (21/08 - "phu ve khong ban ma cu toi xa phu
			// roi len lai map") dang o THANH co Da Tau thi: nhuong Hau can don theo cau hinh
			// (o "Ve thanh" bat) hoac it nhat ban rac (o "Ban vat pham" bat) TRUOC roi moi
			// ra Xa Phu - khong thi vua len map TP tui day lai bat ve, lap vo tan.
			int nOThanhF = 0;
			for (i = 0; i < g_nDTNpcCount; ++i)
				if (g_DTNpc[i].nMapId == nMap)
				{
					nOThanhF = 1;
					break;
				}
			if (nOThanhF && pAp->bReturn && uCurTime - g_uDTYieldT > DT_YIELD_GAP)
				return DT_Yield(nPlayerIdx, uCurTime, "<color=Cyan>BÞ ®­a vÒ thµnh gi÷a lóc ®¸nh qu¸i - ®Ó HËu cÇn b¸n/cÊt ®å, mua thuèc xong sÏ ra Xa Phu ®i l¹i.");
			if (nOThanhF && pAp->bSellItem
			 && Player[nPlayerIdx].m_ItemList.CalcFreeItemCellCount(1, 1, room_equipment) < 10)
			{
				DT_SellStart(nPlayerIdx, uCurTime);
				ea.nDTBackXaFu = 1;	// ban xong thi ra Xa Phu di lai (DT_SellResume)
				DT_Msg(nPlayerIdx, "<color=Cyan>BÞ ®­a vÒ thµnh gi÷a lóc ®¸nh qu¸i - b¸n bít r¸c råi ra Xa Phu ®i l¹i.");
				return 1;
			}
			ea.nDTPhase = DTP_EXEC;
			ea.nDTEngaged = 1;
			return 1;
		}
		ea.nDTEngaged = 2;
		// (r3) dung o (can-1) qua 4 phut: co the tin CHOT da troi TRUOC khi co vong
		// 4 khe (server ngung tha manh khi du) - ve thu tra MOT lan; truot thi
		// FAILREQ tu quay lai danh tiep (nDTHaveBase = da thu, moi nhiem vu 1 lan).
		if (!ea.nDTHaveBase && ea.nDTReqNum > 1 && ea.nDTProg == ea.nDTReqNum - 1
		 && ea.uDTFarmStall && uCurTime - ea.uDTFarmStall > 4u * 60u * 1000u)
		{
			ea.nDTHaveBase = 1;
			DT_Msg(nPlayerIdx, "<color=Cyan>Cã thÓ ®· ®ñ m¶nh (tin nh¾n tr«i) - vÒ thö tr¶ nhiÖm vô...");
			ea.nDTStep = DTI_TURNIN;
			ea.nDTPhase = DTP_RETURN;
			ea.nDTRetry = 0;
			ea.nDTEngaged = 1;
			return 1;
		}
		if (ea.uDTFarmStall && uCurTime - ea.uDTFarmStall > 20u * 60u * 1000u)
			return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Orange>§¸nh qu¸i qu¸ l©u kh«ng tiÕn triÓn.");
		if (ea.nDTProg >= ea.nDTReqNum && ea.nDTReqNum > 0)
		{
			DT_Msg(nPlayerIdx, "<color=Green>§· nhÆt ®ñ sè cuén - quay vÒ tr¶ nhiÖm vô.");
			ea.nDTStep = DTI_TURNIN;
			ea.nDTPhase = DTP_RETURN;
			ea.nDTRetry = 0;
			ea.nDTEngaged = 1;
			return 1;
		}
		// (21/08 - "dang lam nhiem vu, tui day, phu ve khong ban, toi xa phu, len map, phu ve
		// lai") FARM truoc day KHONG kiem tui: day thi TP "ve thanh khi tui day" (tab Hau
		// can) bat nhan vat ve, may tuong bi da khoi map -> ra Xa Phu di lai khong ban -> lap;
		// khong bat TP thi cuon roi khong nhat duoc -> 20 phut "khong tien trien". Nay <5 o
		// trong la TU ban rac tai cho (DTP_SELLJUNK), du cho lai danh tiep.
		if (Player[nPlayerIdx].m_ItemList.CalcFreeItemCellCount(1, 1, room_equipment) < 5)
		{
			if (pAp->bSellItem)
				return DT_BagRelease(nPlayerIdx, pAp, uCurTime, "<color=Yellow>Tói gÇn ®Çy khi ®ang ®¸nh qu¸i - b¸n bít r¸c t¹i chç råi ®¸nh tiÕp.");
			// (PB F4) nguoi choi TAT "Ban vat pham": khong ep vao SELLJUNK (= treo 15 phut) -
			// bat "Ve thanh" thi phu ve, toi thanh FARM se nhuong Hau can cat ruong (DTP_YIELD);
			// khong co gi de don thi treo co loi, khong lap mu.
			if (pAp->bReturn && uCurTime - g_uDTYieldT > DT_YIELD_GAP)
			{
				if (uCurTime - g_uDTSellPortalT <= 15000u)
				{
					ea.uDTNext = uCurTime + 1000;	// dang cho phu chuyen map
					return 2;
				}
				if (DT_UsePortal(nPlayerIdx))
				{
					g_uDTSellPortalT = uCurTime;
					DT_Msg(nPlayerIdx, "<color=Yellow>Tói gÇn ®Çy (kh«ng bËt 'B¸n vËt phÈm') - phï vÒ thµnh nhê HËu cÇn cÊt ®å råi quay l¹i.");
					ea.uDTNext = uCurTime + 1000;
					return 2;
				}
			}
			return DT_Hold(nPlayerIdx, "<color=Yellow>Tói gÇn ®Çy mµ auto kh«ng tù dän ®­îc - bËt 'B¸n vËt phÈm' hoÆc 'VÒ thµnh' (+ phï vÒ trong tói) ë tab HËu cÇn, hoÆc dän tói gióp auto.", uCurTime, 15 * 60 * 1000);
		}
		// DANH QUAI + DI TIM QUAI THEO BANG CUM THAT (ATYPE_FIGHT dung nCurMoveRet==3)
		int nX, nY;
		Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nX, &nY);
		ea.nCurMoveRet = 3;
		// moc quet cua FIGHT luon la CHO DANG DUNG -> gap gi danh nay, ke ca doc duong
		ea.nTempX = nX;
		ea.nTempY = nY;
		if (ea.uNpcID)
		{
			// dang danh: nhuong quyen di chuyen cho FIGHT (huy duong dang di neu con)
			int tx = 0, ty = 0;
			if (SubWorld[0].HaveTarget(tx, ty))
				SubWorld[0].StopPath();
			ea.uDTRoamNext = uCurTime + 60000;	// con viec lam - gia han doi cum
		}
		else
		{
			int rx = 0, ry = 0;
			if (DT_FindFarMob(nPlayerIdx, pAp, &rx, &ry))
			{
				// co quai da dong bo (ngoai tam danh) -> chay toi luon
				DT_WalkTo(nPlayerIdx, rx, ry, 300, uCurTime);
				ea.uDTRoamNext = uCurTime + 60000;
			}
			else
			{
				// (20/08) UU TIEN TAB DI CHUYEN: nguoi choi da cau hinh "quanh diem" hoac
				// "theo toa do" cho DUNG map nhiem vu nay thi di theo do, bo bang cum quai.
				const bool bTabDC = (pAp->nMoveMapId == nMap)
					&& ((pAp->bMoveCoord && pAp->nCoordCount > 0)
					 || (pAp->bAroundPoint && pAp->nPointX > 0 && pAp->nPointY > 0));
				int nSx = 0, nSy = 0;
				if (bTabDC)
				{
					if (pAp->bMoveCoord && pAp->nCoordCount > 0)
					{
						int nK = ea.nDTRoamStep % pAp->nCoordCount;
						if (nK < 0)
							nK = 0;
						if (!ea.uDTRoamNext)
							ea.uDTRoamNext = uCurTime + 60000;
						if (DT_WalkTo(nPlayerIdx, pAp->sMoveCoord[nK].x, pAp->sMoveCoord[nK].y, 300, uCurTime)
						 || uCurTime > ea.uDTRoamNext)
						{
							++ea.nDTRoamStep;
							ea.uDTRoamNext = uCurTime + 60000;
						}
					}
					else
					{
						// quanh diem cua tab Di chuyen: xa thi keo ve diem, gan thi dao 8 huong
						int nBk = pAp->nVision;
						if (nBk < 600)
							nBk = 600;
						else if (nBk > 1800)
							nBk = 1800;
						if (g_GetDistance(nX, nY, pAp->nPointX, pAp->nPointY) > nBk + 1200)
							DT_WalkTo(nPlayerIdx, pAp->nPointX, pAp->nPointY, 400, uCurTime);
						else if (ea.uDTRoamNext < uCurTime)
						{
							ea.uDTRoamNext = uCurTime + 1500;
							static const int nDTDirDC[8][2] =
								{ {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1} };
							int nKk = ea.nDTRoamStep & 7;
							if (DT_WalkTo(nPlayerIdx, pAp->nPointX + nDTDirDC[nKk][0] * nBk,
								pAp->nPointY + nDTDirDC[nKk][1] * nBk, 350, uCurTime))
								++ea.nDTRoamStep;
						}
					}
				}
				else if (DT_SpotOf(nMap, ea.nDTRoamStep, &nSx, &nSy))
				{
					// khong thay quai nao trong tam dong bo -> di den CUM QUAI that
					if (!ea.uDTRoamNext)
						ea.uDTRoamNext = uCurTime + 60000;
					if (DT_WalkTo(nPlayerIdx, nSx, nSy, 400, uCurTime)
					 || uCurTime > ea.uDTRoamNext)
					{
						// toi noi ma van vang, hoac di mai khong toi -> cum ke tiep
						++ea.nDTRoamStep;
						ea.uDTRoamNext = uCurTime + 60000;
					}
				}
				else if (g_GetDistance(nX, nY, ea.nDTAnchorX, ea.nDTAnchorY) > 3600)
					DT_WalkTo(nPlayerIdx, ea.nDTAnchorX, ea.nDTAnchorY, 800, uCurTime);
				else if (ea.uDTRoamNext < uCurTime)
				{
					// map la khong co trong bang: van dao 8 huong quanh neo
					ea.uDTRoamNext = uCurTime + 1500;
					static const int nDTDir8[8][2] =
						{ {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1} };
					int k = ea.nDTRoamStep & 7;
					if (DT_WalkTo(nPlayerIdx, ea.nDTAnchorX + nDTDir8[k][0] * 1800,
						ea.nDTAnchorY + nDTDir8[k][1] * 1800, 350, uCurTime))
						++ea.nDTRoamStep;
				}
			}
		}
		// nhat cuon roi tren dat (genre 6) ke ca khi bo loc nhat cua nguoi choi bo qua
		int nObj = ObjSet.GetNext(0);
		while (nObj)
		{
			if (Object[nObj].m_nKind == Obj_Kind_Item && Object[nObj].m_nGenre == 6)
			{
				int dX, dY;
				Object[nObj].GetMpsPos(&dX, &dY);
				if (g_GetDistance(nX, nY, dX, dY) < 500)
				{
					Player[nPlayerIdx].CheckObject(nObj);
					break;
				}
			}
			nObj = ObjSet.GetNext(nObj);
		}
		ea.uDTNext = uCurTime + 400;
		return 2;
	}

	case DTP_GIVEBOX:
	{
		ea.nDTEngaged = 1;
		if (!cap.nBoxOpen)
		{
			// hop da dong (server xu ly xong?) - quay lai nghe ket qua
			if (ea.nDTStep == DTI_TURNWAIT + 100)
				ea.nDTStep = DTI_TURNWAIT;	// (PB S4) xoa co "da dat item" cho hop lan sau
			ea.nDTPhase = DTP_WAITDLG;
			ea.uDTNext = uCurTime + 600;
			return 1;
		}
		// T3 co the chua chot item (khong qua DTP_EXEC lai) - bao dam co item
		int nItem = ea.nDTItemIdx;
		ItemPos sSrc;
		if (!nItem || !DT_GetItemPos(nPlayerIdx, nItem, &sSrc) || sSrc.nPlace != pos_equiproom)
		{
			int nPos = 0;
			nItem = DT_FindCandItem(nPlayerIdx, pAp, &nPos);
			if (!nItem || nPos != pos_equiproom || !DT_GetItemPos(nPlayerIdx, nItem, &sSrc))
			{
				SendUiCmdScript(1, cap.szBoxFunc);	// bam OK rong de dong (server bao loi nhe)
				ea.nDTPhase = DTP_WAITDLG;
				ea.uDTNext = uCurTime + 900;
				return 1;
			}
			ea.nDTItemIdx = nItem;
		}
		if (ea.nDTStep != DTI_TURNWAIT + 100)	// dung nDTStep lam co "da dat item"
		{
			// (20/08 - loi "co do dung yeu cau ma khong bo vao hop giao") MoveItem 1 goi
			// Down=tui / Up=hop bi server TU CHOI ngay (ExchangeItem doi Down == Up,
			// KItemList.cpp:2227) nen item KHONG BAO GIO vao hop. Lam nhu nguoi keo that
			// (khuon KPlayer.cpp:3281): click nhac item len tay -> click o hop dat xuong
			// -> click lai cho cu (dat truot thi item tu quay ve tui, khong ket tren tay).
			DT_ClickItem(sSrc.nPlace, sSrc.nX, sSrc.nY);
			DT_ClickItem(pos_affairitem, 0, 0);
			DT_ClickItem(sSrc.nPlace, sSrc.nX, sSrc.nY);
			// (21/08 - PB F1) nho ID mon khoe NGAY o nhip dat vao hop: nhip sau item da o
			// pos_affairitem -> guard tren di nhanh "bam OK rong" (chinh la nut OK), khong
			// toi duoc doan duoi; nItem o day la DUY NHAT mon dang dat. Tra XONG thi cat ruong.
			if (ea.nDTQType == 3)
				g_dwDTKhoePend = Item[nItem].GetID();
			ea.nDTStep = DTI_TURNWAIT + 100;
			ea.uDTNext = uCurTime + 900;
			return 1;
		}
		SendUiCmdScript(1, cap.szBoxFunc);
		ea.nDTStep = DTI_TURNWAIT;
		ea.nDTItemIdx = 0;
		ea.nDTPhase = DTP_WAITDLG;
		ea.uDTNext = uCurTime + 1000;
		return 1;
	}

	case DTP_REWARD:
	{
		ea.nDTEngaged = 1;
		if (cap.uFinSeq != ea.uDTFinSeen)
		{
			// Cua so thuong CHUA BAM. seasonnpc.lua (ham Prise) gan 3 phan thuong vao 3
			// nut cua DUNG mot cua so; bam nut khong duoc anh xa thi Prise_Chon KHONG bao
			// loi ma MO LAI cua so (:1327-1333). Nen: lan dau bam o nguoi choi chon, moi
			// lan cua so mo lai thi thu NUT KE, het 3 nut nhom nay thi thu nhom kia.
			ea.uDTFinSeen = cap.uFinSeq;
			ea.nDTRetry = 0;
			const int nThu = ea.nDTRwTry++;
			if (nThu < 6)
			{
				const int nRw1 = (pAp->nDTReward1 >= 0 && pAp->nDTReward1 <= 2) ? pAp->nDTReward1 : 0;
				const int nRw2 = (pAp->nDTReward2 >= 0 && pAp->nDTReward2 <= 2) ? pAp->nDTReward2 : 2;
				const bool bNhomA = (cap.nFinType <= 4) ? (nThu < 3) : (nThu >= 3);
				if (nThu == 0)
					DT_Msg(nPlayerIdx, "<color=AYellow>R­¬ng th­ëng ®· më - auto bÊm chän phÇn th­ëng...");
				// (20/08) bam nut THAT tren cua so thuong qua UI (GDCNI_UI_ACT 7 ->
				// KUiDaTau/KUiDaTau1::AutoPick): di dung duong click nguoi choi nen
				// cua so TU DONG sau khi chon - het canh "nhan roi ma bang van hien".
				// Tra 0 = cua so nhom do khong mo (3 lan thu cheo nhom): UI da an
				// cua so thua, gui thang script nhu ban cu de van nhan duoc thuong.
				if (bNhomA)
				{
					const int nNut = (nRw1 + nThu) % 3;
					if (CoreDataChanged(GDCNI_UI_ACT, 7, 30 + nNut) == 0)
						SendUiCmdScript(3, (char*)DT_FIN3[nNut]);
				}
				else
				{
					const int nNut = (nRw2 + nThu) % 3;
					if (CoreDataChanged(GDCNI_UI_ACT, 7, 40 + nNut) == 0)
						SendUiCmdScript(4, (char*)DT_FIN4[nNut]);
				}
				ea.uDTNext = uCurTime + 800;
				return 1;
			}
			// da thu ca 6 nut ma cua so van mo lai - thoi, di nhan nhiem vu ke
			DT_Msg(nPlayerIdx, "<color=Red>Kh«ng chän ®­îc r­¬ng th­ëng (®· thö ®ñ 6 nót) - bá qua.");
			ea.nDTRwTry = 0;
			ea.nDTStep = DTI_NONE;
			// (r5d) bi cuop pha giua luc di cho -> quay lai pha dang do (GIU nhiem vu)
			if (ea.nDTPhaseBack == DTP_MUASAP || ea.nDTPhaseBack == DTP_CITYHOP)
			{
				ea.nDTPhase = ea.nDTPhaseBack;
				ea.nDTPhaseBack = 0;
				ea.nDTRetry = 0;
				ea.uDTNext = uCurTime + 600;
				return 1;
			}
			ea.nDTQType = 0;
			ea.nDTItemIdx = 0;
			ea.nDTPhase = DTP_GOTONPC;
			ea.nDTRetry = 0;
			ea.uDTNext = uCurTime + 1200;
			return 1;
		}
		// da bam xong cua so vua roi - nan o day ~3 giay xem con cua so thu hai khong
		if (++ea.nDTRetry <= 12)
		{
			ea.uDTNext = uCurTime + 250;
			return 1;
		}
		DT_Msg(nPlayerIdx, "<color=AYellow>§· nhËn th­ëng xong - ®i nhËn nhiÖm vô kÕ tiÕp!");
		ea.nDTRwTry = 0;
		ea.nDTStep = DTI_NONE;
		// (r5d) bi cuop pha giua luc di cho -> quay lai pha dang do (GIU nhiem vu)
		if (ea.nDTPhaseBack == DTP_MUASAP || ea.nDTPhaseBack == DTP_CITYHOP)
		{
			ea.nDTPhase = ea.nDTPhaseBack;
			ea.nDTPhaseBack = 0;
			ea.nDTRetry = 0;
			ea.uDTNext = uCurTime + 600;
			return 1;
		}
		ea.nDTQType = 0;
		ea.nDTItemIdx = 0;
		ea.nDTPhase = DTP_GOTONPC;	// noi chuyen tiep de nhan nhiem vu ke (course 3)
		ea.nDTRetry = 0;
		ea.uDTNext = uCurTime + 1200;
		return 1;
	}

	case DTP_YIELD:
	{
		// (21/08) dang nhuong may cho Hau can lam viec thanh (xem DT_Yield). Buoc 9 cua no
		// = sap ra Xa Phu len map luyen cong -> lay lai may TRUOC do; het han / roi thanh /
		// nguoi tat o "Ve thanh" cung lay lai. Loai 4 dang ve tra thi toi NPC; loai khac
		// qua EXEC de tim lai do (Hau can co the da cat mon nhiem vu vao ruong - EXEC keo ra).
		const int nXongHC = (ea.nHomeStep >= 9);
		// (fight-mode cua minh co the cap nhat TRE hon id map vai nhip sau khi chuyen map
		// -> chi tin co nay sau 3 giay ke tu luc nhuong)
		if (nXongHC || uCurTime > ea.uDTHoldUntil || nMap != g_nDTYieldMap || !pAp->bReturn
		 || (Npc[Player[nPlayerIdx].m_nIndex].m_FightMode && uCurTime - g_uDTYieldT > 3000u))
		{
			DT_Msg(nPlayerIdx, nXongHC ? "<color=Cyan>HËu cÇn dän xong - D· TÈu lµm tiÕp."
			                           : "<color=Cyan>Kh«ng chê HËu cÇn n÷a - D· TÈu lµm tiÕp.");
			ea.uDTHoldUntil = 0;
			if (ea.nDTQType == 4 && ea.nDTStep == DTI_TURNIN)
				ea.nDTPhase = DTP_GOTONPC;
			else if (ea.nDTQType >= 1 && ea.nDTQType <= 6)
			{
				ea.nDTStep = DTI_NONE;
				ea.nDTPhase = DTP_EXEC;
			}
			else
				ea.nDTPhase = DTP_IDLE;
			ea.nDTRetry = 0;
			ea.nDTEngaged = 1;
			return 1;
		}
		ea.nDTEngaged = 0;
		return 0;
	}

	case DTP_MUASAP:
	{
		// (20/08) T2 thieu do: quet sap trong thanh, khong co thi nho Xa Phu qua
		// thanh khac (du 10 thanh/thon co Da Tau). Sap = nguoi that + bot (cung
		// protocol); sap TRANG TRI SimCity khong tra loi -> co timeout moi sap.
		ea.nDTEngaged = 1;
		if (uCurTime > ea.uDTHoldUntil)
		{
			DT_SapDong(nPlayerIdx);
			return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Orange>§i chî qu¸ l©u mµ ch­a mua ®­îc mãn cÇn t×m.");
		}
		// mua xong chua? (item ve tui la xong viec di cho)
		{
			int nPosM = 0;
			int nCo = DT_FindCandItem(nPlayerIdx, pAp, &nPosM);
			if (!nCo && ea.nDTCandCur + 1 < ea.nDTCandNum)
			{
				for (i = 0; i < ea.nDTCandNum; ++i)
				{
					ea.nDTCandCur = i;
					nCo = DT_FindCandItem(nPlayerIdx, pAp, &nPosM);
					if (nCo)
						break;
				}
				if (!nCo)
					ea.nDTCandCur = 0;
			}
			if (nCo && nPosM == pos_equiproom)
			{
				DT_SapDong(nPlayerIdx);
				DT_Msg(nPlayerIdx, "<color=Green>§· mua ®­îc ®å cÇn t×m ë s¹p - quay vÒ tr¶ nhiÖm vô!");
				ea.nDTItemIdx = nCo;
				ea.nDTStep = DTI_TURNIN;
				ea.nDTPhase = DTP_GOTONPC;
				ea.nDTRetry = 0;
				return 1;
			}
		}
		// dang mo mot sap - doi hang / loc / mua
		if (g_dwDTSapCur)
		{
			if (g_cSellItem.m_nId > 0)
			{
				int nBuy = 0;
				int nGia = 0x7fffffff;
				for (int k = 0; k < g_cSellItem.m_nId && k < 60; ++k)
				{
					int nIt = g_cSellItem.m_sItem[k].nIdx;
					if (nIt <= 0)
						continue;
					int nP = g_cSellItem.m_sItem[k].nPrice;
					if (nP <= 0)
						continue;
					bool bKhop = false;
					for (int c2 = 0; c2 < ea.nDTCandNum && !bKhop; ++c2)
					{
						if (ea.nDTQType == 3)
						{
							// (r5) loai 3 "khoe": chi can DUNG DONG MA trong khoang yeu cau
							const DTShowRow& rS = g_DTShow[ea.nDTCand[c2]];
							bKhop = DT_MatchRule(nIt, -1, -1, -1, -1, -1, rS.nMagic, rS.nMin, rS.nMax);
							continue;
						}
						const DTFindRow& r = g_DTFind[ea.nDTCand[c2]];
						if (r.nMagic > 0)
							bKhop = DT_MatchRule(nIt, r.nGenre, r.nDetail, r.nParticular, -1, -1, r.nMagic, r.nMin, r.nMax);
						else
							bKhop = DT_MatchRule(nIt, r.nGenre, r.nDetail, r.nParticular, r.nLevel, r.nFive, 0, 0, 0);
					}
					if (!bKhop)
						continue;
					if (nP < nGia)
					{
						nGia = nP;
						nBuy = nIt;
					}
				}
				// (r5 - phan bien) tran MAC DINH khi nguoi dung chua dat: loai 3
				// (khoe - do trang/xanh re) 30 van de khoi bi sap gia cao hut mau;
				// loai khac giu 200 van. Dat tay o "tran mua" thi theo nguoi dung.
				int nTran = (pAp->nDTMaxMua > 0 && pAp->nDTMaxMua < 100000)
					? pAp->nDTMaxMua : (ea.nDTQType == 3 ? 30 : 200);
				if (nBuy && nGia > nTran * 10000)
				{
					DT_Msg(nPlayerIdx, "<color=Yellow>S¹p cã mãn cÇn nh­ng gi¸ v­ît trÇn cho phÐp - bá qua s¹p nµy.");
					nBuy = 0;
				}
				if (nBuy && Player[nPlayerIdx].m_ItemList.GetEquipmentMoney() < nGia)
				{
					// (PB V15/V20) rut tien tu ruong DUNG cach: cho mo khoa xong (khong coi
					// "dang mo khoa" la that bai), kep theo so du ruong (ExchangeMoney fail
					// im lang khi rut qua so du), toi da 3 lan cho mot sap.
					if (pAp->bDTUseBox && pAp->nDTWDMoney > 0 && g_nDTSapRut < 3
					 && (Player[nPlayerIdx].m_CUnlocked || pAp->szBoxPass[0]))
					{
						if (!DT_EnsureUnlock(nPlayerIdx, pAp, uCurTime))
							return 1;	// dang go mat khau ruong - cho tick sau
						int nRut = pAp->nDTWDMoney * 10000;
						int nCoBox = Player[nPlayerIdx].m_ItemList.GetMoney(room_repository);
						if (nRut > nCoBox)
							nRut = nCoBox;
						if (nRut > 0)
						{
							++g_nDTSapRut;
							Player[nPlayerIdx].m_ItemList.ExchangeMoney(room_repository, room_equipment, nRut);
							ea.uDTNext = uCurTime + 1000;
							return 1;
						}
					}
					DT_Msg(nPlayerIdx, "<color=Yellow>ThÊy mãn cÇn ë s¹p mµ kh«ng ®ñ tiÒn mua.");
					nBuy = 0;
				}
				if (nBuy)
				{
					// server KHONG kiem o dich (dat sai la mat tien + mat mon - 
					// KProtocolProcess.cpp:6073 bo qua return AddKIL) -> tu tim o hop le
					ItemPos sVi;
					if (!Player[nPlayerIdx].m_ItemList.SearchPosition(
						Item[nBuy].GetWidth(), Item[nBuy].GetHeight(), &sVi)
					 || sVi.nPlace != pos_equiproom)
					{
						DT_SapDong(nPlayerIdx);
						return DT_BagRelease(nPlayerIdx, pAp, uCurTime,
							"<color=Yellow>Tói kh«ng cßn « trèng ®Ó nhËn ®å mua ë s¹p.");
					}
					// (PB V11) gia server tinh o THOI DIEM MUA theo m_Idx - snapshot cu qua
					// 3.5s thi xin cap nhat lai roi moi mua (thu hep cua so doi gia lua auto).
					if (uCurTime - g_uDTSapFresh > 3500)
					{
						g_uDTSapFresh = uCurTime;
						g_cSellItem.UpdateItem(g_dwDTSapCur);
						ea.uDTNext = uCurTime + 700;
						return 1;
					}
					// (PB V12) mua truot khong co ACK (mon bi nguoi khac hot) - qua 3 lan gui
					// ma item khong ve tui thi bo sap nay, dung spam.
					if (g_nDTSapBuyTry >= 3)
					{
						DT_SapGhiXem(g_dwDTSapCur);
						DT_SapDong(nPlayerIdx);
						ea.uDTNext = uCurTime + 400;
						return 1;
					}
					int nBanIdx = NpcSet.SearchID(g_dwDTSapCur);
					if (nBanIdx > 0 && Npc[nBanIdx].m_BaiTan)
					{
						++g_nDTSapBuyTry;
						SendClientCmdPlayerBuy(g_cSellItem.FindIdx(nBuy), g_dwDTSapCur,
							pos_equiproom, sVi.nX, sVi.nY);
						ea.uDTNext = uCurTime + 1600;
						return 1;	// vong sau kiem "mua xong chua" o dau case
					}
				}
				// sap nay khong co mon phu hop (hoac khong mua duoc)
				DT_SapGhiXem(g_dwDTSapCur);
				DT_SapDong(nPlayerIdx);
				ea.uDTNext = uCurTime + 400;
				return 1;
			}
			if (uCurTime > g_uDTSapWait)
			{
				// sap khong tra loi (bot trang tri SimCity / nguoi ban dang khoa ruong)
				++g_nDTSapCam;
				DT_SapGhiXem(g_dwDTSapCur);
				DT_SapDong(nPlayerIdx);
			}
			return 1;
		}
		// tim sap gan nhat CHUA xem trong tam dong bo
		{
			int nSelf2 = Player[nPlayerIdx].m_nIndex;
			int nPx, nPy;
			Npc[nSelf2].GetMpsPos(&nPx, &nPy);
			int nGan = 0;
			int nGanD = 0x7fffffff;
			int nQ = 0;
			while (nQ = NpcSet.GetNextIdx(nQ))
			{
				if (nQ == nSelf2)
					continue;
				// (r5 - nguoi dung bao) slot NPC tai dung co the SOT co m_BaiTan cu
				// -> NPC chuc nang/thoai bi quet nham. Loc lai kind_player (nguoi
				// that + bot sap deu la player - test that da mua duoc o sap).
				if (Npc[nQ].m_Kind != kind_player)
					continue;
				if (!Npc[nQ].m_BaiTan)
					continue;
				if (Npc[nQ].m_RegionIndex < 0)
					continue;
				if (DT_SapDaXem(Npc[nQ].m_dwID))
					continue;
				int nEx, nEy;
				Npc[nQ].GetMpsPos(&nEx, &nEy);
				int nD = g_GetDistance(nPx, nPy, nEx, nEy);
				if (nD < nGanD)
				{
					nGanD = nD;
					nGan = nQ;
				}
			}
			if (nGan)
			{
				int nEx, nEy;
				Npc[nGan].GetMpsPos(&nEx, &nEy);
				// (r5c - nguoi dung bao "toi npc chuc nang") dan SimCity cung la
				// kind_player + m_BaiTan=1 (sap trang tri) nen loc kind khong an.
				// THAM DO truoc khi di: goi hoi SO MON - server chi tra loi cho sap
				// co PLAYER that; im lang 2 lan (da ke vung) = sap gia, bo khong di.
				const DWORD dwUV = Npc[nGan].m_dwID;
				if (dwUV != g_dwDTSapOkId)
				{
					const int nSelfR = Npc[Player[nPlayerIdx].m_nIndex].m_RegionIndex;
					if (!DT_SapKeVung(nSelfR, Npc[nGan].m_RegionIndex))
					{
						// xa qua server chua "thay" - di lai gan roi hoi
						DT_WalkTo(nPlayerIdx, nEx, nEy, 280, uCurTime);
						return 1;
					}
					if (g_dwDTSapProbe != dwUV)
					{
						g_dwDTSapProbe = dwUV;
						g_nDTSapProbeTry = 1;
						g_uDTSapProbeT = uCurTime + 1300;
						g_uDTSapCntSeen = g_sDTCap.uCntSeq;
						SendClientCmdGetCount(dwUV);
						ea.uDTNext = uCurTime + 250;
						return 1;
					}
					if (g_sDTCap.uCntSeq != g_uDTSapCntSeen && g_sDTCap.dwCntId == dwUV)
					{
						if (g_sDTCap.nCnt <= 0)
						{
							DT_SapGhiXem(dwUV);	// sap that nhung HET HANG - khoi mo
							g_dwDTSapProbe = 0;
							ea.uDTNext = uCurTime + 200;
							return 1;
						}
						g_dwDTSapOkId = dwUV;	// sap that co hang - di toi mo xem
						g_dwDTSapProbe = 0;
					}
					else if (uCurTime < g_uDTSapProbeT)
					{
						ea.uDTNext = uCurTime + 200;
						return 1;
					}
					else if (g_nDTSapProbeTry < 2)
					{
						++g_nDTSapProbeTry;	// goi co the rot - hoi lai lan 2
						g_uDTSapProbeT = uCurTime + 1300;
						g_uDTSapCntSeen = g_sDTCap.uCntSeq;
						SendClientCmdGetCount(dwUV);
						ea.uDTNext = uCurTime + 250;
						return 1;
					}
					else
					{
						DT_SapGhiXem(dwUV);	// 2 lan im lang = sap trang tri
						++g_nDTSapCam;
						g_dwDTSapProbe = 0;
						char szBoQua[200];
						sprintf(szBoQua, "<color=Gray>B\341 qua s\271p trang tr\335 \"%s\".",
							Npc[nGan].ShopName[0] ? Npc[nGan].ShopName : Npc[nGan].Name);
						DT_Msg(nPlayerIdx, szBoQua);
						ea.uDTNext = uCurTime + 200;
						return 1;
					}
				}
				if (g_GetDistance(nPx, nPy, nEx, nEy) > 320)
				{
					DT_WalkTo(nPlayerIdx, nEx, nEy, 280, uCurTime);
					return 1;
				}
				// server chi doi cung/canh region (~1 man hinh) - du gan roi, mo xem
				{
					char szXemSap[160];
					sprintf(szXemSap, "<color=Cyan>Xem s¹p \"%s\" ...",
						Npc[nGan].ShopName[0] ? Npc[nGan].ShopName : Npc[nGan].Name);
					DT_Msg(nPlayerIdx, szXemSap);
				}
				++g_nDTSapXem;
				g_dwDTSapCur = Npc[nGan].m_dwID;
				g_uDTSapWait = uCurTime + 2500;
				g_uDTSapFresh = uCurTime;
				g_nDTSapBuyTry = 0;
				g_nDTSapRut = 0;
				g_cSellItem.DeleteAll();
				g_cSellItem.ApplyViewItem(g_dwDTSapCur);
				ea.uDTNext = uCurTime + 300;
				return 1;
			}
		}
		// (r5e - nguoi dung) DANH BA SAP: hoi server vi tri MOI sap trong map
		// (goi needcount cu, dwId dac biet) - biet ngay cho nao co sap ke ca
		// nguoi choi bay cho la, khoi long vong. Server cu chua restart -> im
		// lang 1.8s roi roi xuong di tuan nhu truoc.
		// (r5f - phan bien) "-1" truoc day la trang thai CHET: mat goi tra loi
		// (server vua restart / goi roi) la khong bao gio hoi lai tren map do.
		// Nay cho hoi lai toi da 3 lan, va lam moi bo dem sau 30 giay.
		if (g_nDTSapDs < 0 && g_nDTSapDsMap == nMap
		 && uCurTime > g_uDTSapDsFresh + 30000)
			g_nDTSapDsTry = 0;
		// (r5g - phan bien vong 2) BA sua o dieu kien nay:
		// - nhanh hoi lai chay TRUOC khoi phan tich va ghi de g_uDTSapDsSeen ben
		//   duoi, nen tra loi ve MUON hon han cho se bi vut vinh vien (bo dem
		//   don, seq da bi danh dau da doc) -> con tra loi CHUA DOC thi DUNG hoi
		//   lai, de nhip nay phan tich no.
		// - bo dem Try phai dem so lan hoi LIEN TIEP KHONG hoi am: nhanh doi map
		//   va nhanh lam moi 90 giay (da co danh ba = kenh dang song) nap lai ve 0,
		//   khong thi o lau mot thanh la can sach ngan sach.
		if (g_nDTSapDsMap != nMap
		 || (g_nDTSapDs >= 0 && uCurTime > g_uDTSapDsFresh + 90000)
		 || (g_nDTSapDs < 0 && uCurTime > g_uDTSapDsT && g_nDTSapDsTry < 3
		  && g_sDTCap.uSapMapSeq == g_uDTSapDsSeen
		  && uCurTime > g_uDTSapDsFresh + 5200))	// (r5h) phai VUOT cooldown 5
					// giay ben server, khong thi goi hoi lai bi vut im lang
		{
			if (g_nDTSapDsMap != nMap)
			{
				g_nDTSapXa = 0;		// (r5i) thanh khac - danh sach khong-toi-duoc het nghia
				g_nDTSapDsItemIdx = -1;
			}
			if (g_nDTSapDsMap != nMap || g_nDTSapDs >= 0)
				g_nDTSapDsTry = 0;
			++g_nDTSapDsTry;
			g_nDTSapDsMap = nMap;
			g_nDTSapDs = -1;
			g_nDTSapDsCur = 0;
			g_uDTSapDsFresh = uCurTime;
			// han cho: lan DAU 1,8 giay; cac lan hoi lai phai > 5 giay (cooldown
			// chong spam ben server) khong thi goi hoi lai bi vut im lang.
			g_uDTSapDsT = uCurTime + 1800;	// han "coi nhu mat goi" + han dung im
			g_uDTSapDsSeen = g_sDTCap.uSapMapSeq;
			SendClientCmdGetCount(DATAU_SAPMAP_ID);
			ea.uDTNext = uCurTime + 300;
			return 1;
		}
		if (g_sDTCap.uSapMapSeq != g_uDTSapDsSeen)
		{
			g_uDTSapDsSeen = g_sDTCap.uSapMapSeq;
			g_nDTSapDsTry = 0;	// (r5g) co hoi am = kenh song, nap lai ngan sach
			// (r5h) g_uDTSapWptT/g_uDTSapDwell la MOC TUYET DOI: roi di tuan sang
			// che do danh ba ma de moc treo thi lan tuan sau se DOT thang diem tu
			// tap dau tien (het han ngay nhip dau, chua di toi).
			g_uDTSapWptT = 0;
			g_uDTSapDwell = 0;
			g_uDTSapDsItemT = 0;
			g_nDTSapDs = 0;
			g_nDTSapDsCur = 0;
			const char* pDs = g_sDTCap.szSapMap + 8;
			unsigned int uId5; int nX5, nY5;
			while (g_nDTSapDs < 12 && sscanf(pDs, " %u:%d:%d", &uId5, &nX5, &nY5) == 3)
			{
				g_aDTSapDsId[g_nDTSapDs] = uId5;
				g_aDTSapDsX[g_nDTSapDs] = nX5 * 32;
				g_aDTSapDsY[g_nDTSapDs] = nY5 * 32;
				++g_nDTSapDs;
				const char* pKe = strchr(pDs + 1, ' ');
				if (!pKe)
					break;
				pDs = pKe;
			}
			if (g_nDTSapDs > 0)
			{
				char szDs2[160];
				sprintf(szDs2, "<color=Cyan>Server b\270o %d s\271p trong th\265nh - ch\271y th\274ng t\355i t\365ng s\271p.", g_nDTSapDs);
				DT_Msg(nPlayerIdx, szDs2);
			}
			else
				DT_Msg(nPlayerIdx, "<color=Gray>Th\265nh n\265y kh\253ng c\343 s\271p th\313t n\265o (server x\270c nh\313n) - qua th\265nh k\325.");
		}
		// (r5g) chi dung im cho o LAN HOI DAU (1,8 giay); cac lan hoi lai cho 6
		// giay nen phai vua di tuan vua cho, khong dung bot lai 5,4 giay/chu ky.
		if (g_nDTSapDs < 0 && g_nDTSapDsTry <= 1 && uCurTime < g_uDTSapDsT)
		{
			ea.uDTNext = uCurTime + 300;
			return 1;	// cho danh ba ve
		}
		if (g_nDTSapDs > 0)
		{
			while (g_nDTSapDsCur < g_nDTSapDs
			 && (DT_SapDaXem(g_aDTSapDsId[g_nDTSapDsCur])
			  || DT_SapDsDaBo(g_aDTSapDsId[g_nDTSapDsCur])))
				++g_nDTSapDsCur;
			if (g_nDTSapDsCur < g_nDTSapDs)
			{
				// (r5i - phan bien vong 4) HAI dong xoa moc tuan TRUOC day nam O DAY
				// (ngoai nhanh nay) nen khi danh ba DA CAN, moi nhip deu xoa roi goi
				// DT_SapWaypoint: han 45s nap lai lien tuc va dwell luon 0 => bot ket
				// VINH VIEN o diem tuan 1 + spam chat. Nay chi xoa khi THUC SU dang
				// di theo danh ba.
				g_uDTSapWptT = 0;
				g_uDTSapDwell = 0;
				const int nDs = g_nDTSapDsCur;
				// (r5i) doi muc = nap lai han 45 giay. Truoc day muc hoan tat qua
				// duong "sap that" khong xoa han nen muc ke thua han da can va bi bo
				// OAN khi chua di mot buoc nao.
				if (g_nDTSapDsItemIdx != nDs)
				{
					g_nDTSapDsItemIdx = nDs;
					g_uDTSapDsItemT = 0;
				}
				int nPx5, nPy5;
				Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nPx5, &nPy5);
				if (g_GetDistance(nPx5, nPy5, g_aDTSapDsX[nDs], g_aDTSapDsY[nDs]) < 300)
				{
					int nIdx5 = NpcSet.SearchID(g_aDTSapDsId[nDs]);
					if (nIdx5 <= 0 || !Npc[nIdx5].m_BaiTan)
					{
						DT_SapGhiXem(g_aDTSapDsId[nDs]);	// sap da don/doi cho
						++g_nDTSapDsCur;
						g_uDTSapDsItemT = 0;
					}
					// con sap that o day: vong quet phia tren tu tham do + mo xem
					ea.uDTNext = uCurTime + 400;
					return 1;
				}
				// (r5h - phan bien vong 3) HAN 45 GIAY cho MOI muc: toa do co the
				// khong bo toi duoc (NPC lam tuong, o cua, hoac map KHONG CO LUOI
				// DUONG - FindPath tra -1 nen bot dung im vinh vien). Het gio thi
				// GHI XEM (khong chi ++Cur - vi lam moi 90 giay dat Cur ve 0) roi
				// qua muc ke, khong de mot sap lam ket ca thanh 25 phut.
				if (!g_uDTSapDsItemT)
					g_uDTSapDsItemT = uCurTime + 45000;
				if (uCurTime > g_uDTSapDsItemT)
				{
					// (r5i) ghi vao danh sach RIENG - KHONG dung DT_SapGhiXem, khong thi
					// vong quet gan cung mu luon (bot dung sat ben van khong mo xem).
					DT_SapDsGhiBo(g_aDTSapDsId[nDs]);
					++g_nDTSapDsCur;
					g_uDTSapDsItemT = 0;
					DT_Msg(nPlayerIdx, "<color=Gray>Kh\253ng \256\325n \256\255\356c ch\347 s\271p n\265y - b\341 qua, \256i s\271p k\325.");
					ea.uDTNext = uCurTime + 300;
					return 1;
				}
				DT_WalkTo(nPlayerIdx, g_aDTSapDsX[nDs], g_aDTSapDsY[nDs], 250, uCurTime);
				return 1;
			}
			// het danh ba -> KHONG dung o day: van di tuan cac diem tu tap de quet
			// not sap ngoai 12 muc server tra ve (thanh dong co the co nhieu hon).
		}
		// (r5h - phan bien vong 3) TRUOC day chi di tuan khi danh ba THAT BAI, ma
		// server chi tra toi da 12 sap -> thanh dong bi tuyen bo "xong" sau 12 sap,
		// do phu THAP HON ban truoc khi co danh ba. Nay: danh ba di truoc (dia chi
		// chinh xac), het danh ba thi tour diem tu tap quet not, het CA HAI moi qua
		// thanh ke.
		if (DT_SapWaypoint(nPlayerIdx, nMap, uCurTime))
			return 1;
		// het thanh nay -> danh dau roi qua thanh ke
		{
			int nTi = DT_SapTownIndex(nMap);
			if (nTi >= 0)
				g_nDTSapMask |= (1 << nTi);
			int nKe = DT_SapNextTown(nMap);
			if (nKe < 0)
			{
				DT_SapDong(nPlayerIdx);
				return DT_Skip(nPlayerIdx, pAp, uCurTime,
					"<color=Orange>§· xem s¹p kh¾p 10 thµnh/th«n mµ kh«ng cã mãn cÇn t×m.");
			}
			ea.nDTPhase = DTP_CITYHOP;
			ea.nDTShopTry = nKe;	// chi so thanh dich trong g_DTNpc
			ea.nDTRetry = 0;
			g_uDTSapHopT = 0;
			{
				// (r3) thong ke de chan doan: xem duoc bao nhieu sap, may sap "cam"
				char szTKSap[220];
				sprintf(szTKSap, "<color=Cyan>Thµnh nµy: xem %d s¹p (%d kh«ng ph¶n håi) - kh«ng cã mãn cÇn, nhê Xa Phu qua thµnh kÕ...",
					g_nDTSapXem, g_nDTSapCam);
				DT_Msg(nPlayerIdx, szTKSap);
			}
			return 1;
		}
	}

	case DTP_CITYHOP:
	{
		// (20/08) qua thanh khac bang menu Xa Phu "Nhung thanh thi da di qua"
		ea.nDTEngaged = 1;
		if (uCurTime > ea.uDTHoldUntil)
		{
			DT_SapDong(nPlayerIdx);
			return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Orange>§i chî qu¸ l©u mµ ch­a mua ®­îc mãn cÇn t×m.");
		}
		if (ea.nDTShopTry < 0 || ea.nDTShopTry >= g_nDTNpcCount)
		{
			ea.nDTPhase = DTP_MUASAP;
			return 1;
		}
		const int nDestMap = g_DTNpc[ea.nDTShopTry].nMapId;
		if (nMap == nDestMap)
		{
			// toi noi - quet thanh moi tu dau
			g_nDTSapWpt = 0;
			g_uDTSapWptT = 0;
			g_uDTSapDwell = 0;
			g_nDTSapDone = 0;
			g_uDTSapHopT = 0;
			g_uDTThpT = 0;
			g_nDTSapXem = 0;
			g_nDTSapCam = 0;
			ea.nDTPhase = DTP_MUASAP;
			ea.nDTRetry = 0;
			ea.uDTNext = uCurTime + 900;
			return 1;
		}
		if (!g_uDTSapHopT)
		{
			g_uDTSapHopT = uCurTime + 150000;	// 150s cho mot luot nhay thanh
			g_uDTThpT = 0;	// (r5) luot nhay moi - duoc thu Than Hanh Phu lai
		}
		if (uCurTime > g_uDTSapHopT)
		{
			// khong qua duoc thanh nay (khong co tien?) - bo, thu thanh khac
			g_nDTSapMask |= (1 << ea.nDTShopTry);
			g_uDTSapHopT = 0;
			int nKe2 = DT_SapNextTown(nMap);
			if (nKe2 < 0)
			{
				DT_SapDong(nPlayerIdx);
				return DT_Skip(nPlayerIdx, pAp, uCurTime,
					"<color=Orange>Kh«ng nhê Xa Phu qua thµnh kh¸c ®­îc (thiÕu tiÒn?).");
			}
			ea.nDTShopTry = nKe2;
			return 1;
		}
		std::map<int, StationVector>::iterator itXa = g_MoveStation.find(nMap);
		if (itXa == g_MoveStation.end() || itXa->second.empty())
		{
			// dang o map la (khong co Xa Phu) - phu ve thanh truoc
			if (!Npc[Player[nPlayerIdx].m_nIndex].m_FightMode
			 || (!DT_UsePortal(nPlayerIdx) && !DT_PortalPull(nPlayerIdx, pAp, uCurTime)))
			{
				DT_SapDong(nPlayerIdx);
				return DT_Skip(nPlayerIdx, pAp, uCurTime,
					"<color=Red>L¹c map kh«ng cã Xa Phu mµ kh«ng dïng ®­îc phï vÒ thµnh.");
			}
			ea.uDTNext = uCurTime + 3000;
			return 1;
		}
		// co hoi thoai moi cua Xa Phu?
		if (cap.uDlgSeq != ea.uDTDlgSeen)
		{
			ea.uDTDlgSeen = cap.uDlgSeq;
			char szBuf2[2048];
			char* apAns2[16];
			g_StrCpyLen(szBuf2, cap.szDlg, sizeof(szBuf2));
			int nAns2 = DT_Split(szBuf2, apAns2, 16);
			// menu 2: dong mang ten thanh dich (kem "[N luong]")
			const char* szTen = DT_SapTownMenu(nDestMap);
			int idx2;
			if (szTen && (idx2 = DT_FindAns(apAns2, nAns2, szTen)) >= 0)
			{
				DT_Answer(nPlayerIdx, idx2);
				ea.uDTNext = uCurTime + 1500;
				return 1;	// server thu tien + NewWorld; vong sau nMap == nDestMap
			}
			// menu 1: muc "Nhung thanh thi da di qua"
			if ((idx2 = DT_FindAns(apAns2, nAns2, DTM_SAP_THANHTHI)) >= 0)
			{
				DT_Answer(nPlayerIdx, idx2);
				ea.uDTNext = uCurTime + 900;
				return 1;
			}
			// (r5) chuoi thoai Than Hanh Phu (shenxingfu.lua): menu chinh -> muc
			// "Su dung thuat than hanh di den noi chi dinh" -> "Thanh thi"/"Thon
			// trang" -> danh sach ten thanh + menu khu (CA HAI deu khop boi szTen
			// o tren). 6 thanh co "Trung Tam" hang dau nen tu vao trung tam; RIENG
			// Lam An KHONG co Trung Tam -> vao hang dau "Lam An Nam". CAM doi sang
			// khop needle "Trung Tam": Lam An se truot het handler va ket 12s
			// (nhu bug " Phu" cua Thanh Do). Thon dich chuyen thang, khong menu khu.
			if ((idx2 = DT_FindAns(apAns2, nAns2, "thuËt thÇn hµnh")) >= 0)
			{
				DT_Answer(nPlayerIdx, idx2);
				ea.uDTNext = uCurTime + 900;
				return 1;
			}
			if (g_uDTThpT > 1)
			{
				const int bThon5 = (nDestMap == 20 || nDestMap == 121 || nDestMap == 53);
				idx2 = DT_FindAns(apAns2, nAns2, bThon5 ? "Th«n trang" : "Thµnh thÞ");
				if (idx2 >= 0)
				{
					DT_Answer(nPlayerIdx, idx2);
					ea.uDTNext = uCurTime + 900;
					return 1;
				}
			}
			// hoi thoai khong lien quan - dong roi mo lai
			CoreDataChanged(GDCNI_UI_ACT, 1, 0);
		}
		// (r5 - nguoi dung) co Than Hanh Phu (6,1,1271) trong tui thi dung no dich
		// chuyen thang (mien phi, khoi chay bo toi Xa Phu). Thu 1 lan moi luot
		// nhay; 12 giay chua doi map thi roi xuong duong Xa Phu nhu cu.
		if (g_uDTThpT == 0)
		{
			if (Player[nPlayerIdx].m_ItemList.AutoUseItem(6, 1, 1271, nPlayerIdx))
			{
				g_uDTThpT = (uCurTime > 1) ? uCurTime : 2;
				DT_Msg(nPlayerIdx, "<color=Cyan>Dïng ThÇn Hµnh Phï dÞch chuyÓn tíi thµnh kÕ tiÕp...");
				ea.uDTNext = uCurTime + 1200;
				return 1;
			}
			g_uDTThpT = 1;	// khong co phu trong tui - di Xa Phu
			DT_Msg(nPlayerIdx, "<color=Gray>Kh\253ng c\343 Th\307n H\265nh Ph\357 trong t\363i (r\255\254ng kh\253ng t\335nh) - \256i nh\352 Xa Phu.");
		}
		else if (g_uDTThpT > 1)
		{
			if (uCurTime < g_uDTThpT + 12000)
			{
				ea.uDTNext = uCurTime + 700;	// dang di bang thoai THP (khoi tren xu ly)
				return 1;
			}
			g_uDTThpT = 1;	// het 12s van chua toi - roi xuong Xa Phu
			DT_Msg(nPlayerIdx, "<color=Yellow>ThÇn Hµnh Phï kh«ng ®­a ®i ®­îc - ch¹y tíi Xa Phu vËy.");
		}
		// di den Xa Phu + mo thoai (khuon DTP_GOXAFU)
		sStation& sXa = itXa->second[0];
		int nXaIdx = DT_FindNpcName(nPlayerIdx, "xa phu", sXa.x, sXa.y, 400);
		if (nXaIdx)
		{
			int nX2, nY2, dX2, dY2;
			Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(&nX2, &nY2);
			Npc[nXaIdx].GetMpsPos(&dX2, &dY2);
			if (g_GetDistance(nX2, nY2, dX2, dY2) <= 128)
			{
				ea.uDTDlgSeen = cap.uDlgSeq;
				Player[nPlayerIdx].DialogNpc(nXaIdx);
				ea.uDTNext = uCurTime + 800;
				return 1;
			}
			DT_WalkTo(nPlayerIdx, dX2, dY2, 96, uCurTime);
			return 1;
		}
		DT_WalkTo(nPlayerIdx, sXa.x, sXa.y, 200, uCurTime);
		return 1;
	}

	case DTP_USEPD:
	{
		ea.nDTEngaged = 1;
		if (ea.nDTUsedPD > 40)
			return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Orange>Dïng qu¸ nhiÒu Phóc Duyªn Lé mµ vÉn ch­a ®ñ ®iÓm.");
		// item khong tac dung? (2 lan dung ma fuyuan khong doi)
		int nFY = Npc[Player[nPlayerIdx].m_nIndex].nFuYuan;
		if (ea.nDTUsedPD >= 2 && nFY <= ea.nDTFuYuanPrev)
			return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Red>Phóc Duyªn Lé kh«ng cã t¸c dông (server thiÕu script?).");
		// dung 1 item tu tui; khong co -> keo tu ruong; khong co nua -> thu tra roi skip
		bool bUsed = false;
		for (i = 0; i < 3 && !bUsed; ++i)
		{
			if (Player[nPlayerIdx].m_ItemList.AutoUseItem(6, 1, 121 + i, nPlayerIdx))
				bUsed = true;
		}
		if (bUsed)
		{
			++ea.nDTUsedPD;
			ea.nDTFuYuanPrev = nFY;
			ea.nDTStep = DTI_TURNIN;
			ea.nDTPhase = DTP_GOTONPC;
			ea.uDTNext = uCurTime + 1500;
			return 1;
		}
		// (20/08) phuc duyen: kiem RUONG khong phu thuoc o "Lay do/tien tu ruong".
		// (r3) ruong DA MO (m_CUnlocked - vd ruong khong dat mat khau) cung keo duoc,
		// khong bat buoc phai go mat khau trong WAuto nua.
		if (Player[nPlayerIdx].m_CUnlocked || pAp->szBoxPass[0])
		{
			for (i = 0; i < 3; ++i)
			{
				int nPos = 0;
				int nItem = DT_FindItemRule(nPlayerIdx, true, false, 6, 1, 121 + i, -1, -1, 0, 0, 0, &nPos);
				if (nItem && nPos != pos_equiproom)
				{
					if (!DT_EnsureUnlock(nPlayerIdx, pAp, uCurTime))
					{
						// (r4 - PB R8) mo khoa mai khong duoc (pass sai?) - dung lap vo han
						if (++ea.nDTRetry > 12)
						{
							DT_Msg(nPlayerIdx, "<color=Yellow>Kh«ng më khãa ®­îc r­¬ng (kiÓm tra mËt khÈu ë tab HËu cÇn).");
							break;	// roi xuong thu tra / bao het
						}
						return 1;
					}
					if (Player[nPlayerIdx].m_ItemList.CalcFreeItemCellCount(
						Item[nItem].GetWidth(), Item[nItem].GetHeight(), room_equipment) >= 1)
					{
						DT_BoxToBag(nItem, nPos);
						ea.uDTNext = uCurTime + 1200;
						return 1;
					}
				}
			}
		}
		else
		{
			// (r3) ruong khoa + khong co mat khau: neu ruong CO Phuc Duyen Lo thi noi
			// ro cho nguoi choi biet phai lam gi (truoc day im lang roi bao "het" - sai).
			for (i = 0; i < 3; ++i)
			{
				int nPosPD = 0;
				if (DT_FindItemRule(nPlayerIdx, true, false, 6, 1, 121 + i, -1, -1, 0, 0, 0, &nPosPD)
				 && nPosPD != pos_equiproom)
				{
					DT_Msg(nPlayerIdx, "<color=Yellow>R­¬ng cã Phóc Duyªn Lé nh­ng ®ang khãa - nhËp mËt khÈu r­¬ng ë tab HËu cÇn.");
					break;
				}
			}
		}
		if (ea.nDTUsedPD == 0)
		{
			// chua co item nao - thu tra 1 lan (nho da du fuyuan)
			ea.nDTStep = DTI_TURNIN;
			ea.nDTPhase = DTP_GOTONPC;
			++ea.nDTUsedPD;
			return 1;
		}
		return DT_Skip(nPlayerIdx, pAp, uCurTime, "<color=Orange>HÕt Phóc Duyªn Lé mµ vÉn ch­a ®ñ ®iÓm.");
	}

	default:
		ea.nDTPhase = DTP_IDLE;
		return ea.nDTEngaged;
	}
}
// ==================== HET AUTO DA TAU ====================


int	KCoreShell::OperationRequest(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 1;
	switch(uOper)
	{
	case GOI_TASKGUIDE_GOTO_XAFU:	// [TaskGuide] bang F11: nhiem vu loai 4 -> tu chay den Xa Phu
		nRet = TG_XaFuStart();
		break;
	case GOI_CP_UNLOCK:						//open ruong
		SendClientCPUnlockCmd(uParam);
		break;
	case GOI_CP_LOCK:						//khoa ruong
		SendClientCPLockCmd();
		break;
	case GOI_CP_SWITCH_EQUIPSET:			//request switch equip set
		SendClientSwitchEquipSetCmd(nParam);
		break;
	case GOI_CP_CHANGE:						//doi mk ruong
		SendClientCPChangeCmd(uParam, nParam);
		break;
	case GOI_CP_RESET:						//reset mk ruong
		SendClientCPResetCmd(nParam);
		break;
	case GOI_CP_SET_IMAGE_PLAYER:
		SendClientCPSetImageCmd(uParam);
		break;
	case GOI_TRADE_PLAYER_BUY:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			if (CGOG_PLAYERSELLITEM != pObject1->Obj.uGenre)
				break;			
			
			int nWidth, nHeight;
			ItemPos	Pos;
			
			nWidth = Item[pObject1->Obj.uId].GetWidth();
			nHeight = Item[pObject1->Obj.uId].GetHeight();
			if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(nWidth, nHeight, &Pos))
			{
				nRet = 0;
				break;
			}
			if (Pos.nPlace != pos_equiproom)
			{
				nRet = 0;
				
				KSystemMessage	sMsg;
				
				strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
			}
			
			if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < Item[pObject1->Obj.uId].GetSetPrice())
			{
				nRet = 0;
				KSystemMessage	sMsg;
				
				strcpy(sMsg.szMessage, MSG_SHOP_NO_MONEY);
				sMsg.eType = SMT_SYSTEM;
				sMsg.byConfirmType = SMCT_CLICK;
				sMsg.byPriority = 1;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				break;
				
			}
			
			nRet = 1;
			SendClientCmdPlayerBuy(g_cSellItem.FindIdx(pObject1->Obj.uId), nParam, pos_equiproom, Pos.nX, Pos.nY);
		}
		break;

	case GOI_VIEW_PLAYERSELLITEM_END:
		{
			g_cSellItem.DeleteAll();
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetMenuState(PLAYER_MENU_STATE_NORMAL);
		}
		break;
	case GOI_VIEW_PLAYERSELLITEM:
		if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan)
		{
			g_cSellItem.ApplyViewItem(uParam);
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetMenuState(PLAYER_MENU_STATE_TRADING);
		}
		break;
	case GDI_SET_TRADE_ITEM:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{
				if (!Player[CLIENT_PLAYER_INDEX].m_CUnlocked)
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, "B¹n h·y më khãa b¶o vÖ míi cã thÓ thùc hiÖn ®­îc !");
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				if(Item[nIdx].GetPlayerItemLock() > 0 || Item[nIdx].GetPlayerItemHLock() >0 || Item[nIdx].GetPlayerItemLock() == -2)
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_TRADE_TASK_ITEM);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				Item[nIdx].SetPrice(nParam);
				SendClientCmdSetPrice(Item[nIdx].GetID(),nParam);
			}
		}
		break;
	case GDI_PLAYER_TRADE:
		{
			char* sShopName = (char *)uParam;
			if (!Player[CLIENT_PLAYER_INDEX].m_CUnlocked)
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, "B¹n h·y më khãa b¶o vÖ míi cã thÓ thùc hiÖn ®­îc !");
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return 0;
			}

			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, "§ang c­ëi ngùa kh«ng thÓ bµy b¸n!");
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return 0;
			}
			
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode)
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, "Kh«ng thÓ bµy b¸n ë khu vùc chiÕn ®Êu!");
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				return 0;
			}
			
			if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan == 0)
			{
				int nCount = 0;//#check neu khong co item dinh gia thong bao
				PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
				if(pItem && pItem->nIdx && Item[pItem->nIdx].m_CommonAttrib.uPrice && pItem->nPlace == pos_equiproom)
				{
					nCount++;
				}
				while(pItem)
				{
					pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
					if(pItem && pItem->nIdx && Item[pItem->nIdx].m_CommonAttrib.uPrice && pItem->nPlace == pos_equiproom)
					{
						nCount++;
					}
				}
				if(nCount)
				{
					if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
					{
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_sit);
						SendClientCmdSit(TRUE);
					}
					SendClientCmdStartTrade(1, sShopName);
				}
				else
				{
					//thong bao chua dinh gia vat pham
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, "Ch­a ®Þnh gi¸ vËt phÈm !!! ");
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
			}
			else
				SendClientCmdStartTrade(0, "");
		}
		break;
	case GOI_QUERY_RANK_INFORMATION:
		SendClientCmdQueryLadder(uParam);
		break;
	//uParam = (const char*)pszFileName
	case GOI_PLAY_SOUND:
		if (uParam)
		{
			static KCacheNode* pSndNode = NULL;
			KWavSound* pSound = NULL;
			pSndNode	= (KCacheNode*)g_SoundCache.GetNode((char *)uParam, (KCacheNode * )pSndNode);
			pSound		= (KWavSound*)pSndNode->m_lpData;
			if (pSound)
			{
				if (pSound->IsPlaying())
					break;
				pSound->Play(0, -10000 + Option.GetSndVolume() * 100, 0);
			}
		}
		break;
	case GOI_PLAYER_RENASCENCE:
		{/*
			int nReviveType;
			if (nParam)	// bBackTown
			{
				nReviveType = REMOTE_REVIVE_TYPE;
			}
			else
			{
				nReviveType = LOCAL_REVIVE_TYPE;
			}*/
			SendClientCmdRevive();
		}
		break;
	case GOI_MONEY_INOUT_STORE_BOX:
		{
			BOOL	bIn = (BOOL)uParam;
			int		nMoney = nParam;
			int		nSrcRoom, nDesRoom;


			if (bIn)
			{
				nSrcRoom = room_equipment;
				nDesRoom = room_repository;
			}
			else
			{
				nDesRoom = room_equipment;
				nSrcRoom = room_repository;
			}
			Player[CLIENT_PLAYER_INDEX].m_ItemList.ExchangeMoney(nSrcRoom, nDesRoom, nMoney);
		}
		break;
	case GOI_LIXIAN:
		SendClientCmdLiXian();
		break;
	case GOI_EXIT_GAME:
		g_SubWorldSet.Close();
		g_ScenePlace.ClosePlace();
		//Player[CLIENT_PLAYER_INDEX].m_cAuto.FkAutoMapSet_StepOne(); //fkauto
		break;
	case GOI_GAMESPACE_DISCONNECTED:
		//Player[CLIENT_PLAYER_INDEX].m_cAuto.FkAutoMapSet_StepOne(); //fkauto
		Player[CLIENT_PLAYER_INDEX].m_sExtAuto.nHomeStep = 0;
		Player[CLIENT_PLAYER_INDEX].m_sExtAuto.nSubStep = 0;
		g_SubWorldSet.Close();
		break;
	case GOI_TRADE_NPC_BUY:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			if (pObject1->Obj.uGenre != CGOG_NPCSELLITEM)
				break;
			//
			if (nParam <= 0)
				break;
			//
			SendClientCmdBuy(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop, pObject1->Obj.uId, nParam, 0);
		}
		break;
	case GOI_SUPPERSHOP_TRADE_NPC_BUY:
		{
			FKGioHang* pGioHang = (FKGioHang*)uParam;

			if (nParam <= 0)
				break;
			
			for(int i = 0; i < MAX_ITEM_SPC; i ++)
			{
				if(pGioHang->m_ListCount[i] > 0)
				{
					SendClientCmdBuy(pGioHang->m_ListShopId[i], pGioHang->m_ListItemInfo[i].Obj.uId, pGioHang->m_ListCount[i], 1);
				}
			}
		}
		break;
	case GOI_TRADE_NPC_SELL:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			//
			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;
			//
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{
				if (//Item[nIdx].GetGenre() == item_task || //Vat pham task event Item khong giao dich
					Item[nIdx].GetPlayerItemLock() > 0 
					|| Item[nIdx].GetPlayerItemHLock() > 0 
					|| Item[nIdx].GetPlayerItemLock() == -2)
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_TRADE_TASK_ITEM);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				SendClientCmdSell(Item[nIdx].GetID());
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	case GOI_ADD_UI_CMD_SCRIPT:
		if(uParam && nParam)
		{
			SendUiCmdScript(uParam, (char*)nParam);
		}
		break;
	case GDI_THROW_ALL_ITEM:
		if(uParam)
		{
			KUiObjAtContRegion *pObj = (KUiObjAtContRegion *)uParam;
			Player[CLIENT_PLAYER_INDEX].ThrowAllItem(pObj->Obj.uId, pObj->Obj.uGenre, 0);
		}
		break;
	case GOI_RECOVER_ITEM:
		if(uParam)
		{
			BOOL bExistId = FALSE;
			switch(uParam)
			{	
				int i;
				/*case pos_give:
					if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetGiveItem())
					{
						bExistId = TRUE;
					}
					break;*/
				case pos_tremble:
					{
						for(i = 0; i < tremblepart_num; i++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetTrembleItem(i))
							{
								bExistId = TRUE;
							}
						}
					}
					break;
				/*case pos_compone:
					{
						for(i = 0; i < compoundpart_num; i++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetCompOneItem(i))
							{
								bExistId = TRUE;
							}
						}
					}
					break;
				case pos_comptwo:
					{
						for(i = 0; i < compoundpart_num; i++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetCompTwoItem(i))
							{
								bExistId = TRUE;
							}
						}
					}
					break;
				case pos_compthree:
					{
						for(i = 0; i < compoundpart_num; i++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetCompThreeItem(i))
							{
								bExistId = TRUE;
							}
						}
					}
					break;
				case pos_distill:
					{
						for(i = 0; i < outinpart_num; i++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetDistillItem(i))
							{
								bExistId = TRUE;
							}
						}
					}
					break;
				case pos_forge:
					{
						for(i = 0; i < forgepart_num; i++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetForgeItem(i))
							{
								bExistId = TRUE;
							}
						}
					}
					break;
				case pos_enchase:
					{
						for(i = 0; i < outinpart_num; i++)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEnchaseItem(i))
							{
								bExistId = TRUE;
							}
						}
					}
					break;*/
				default:
					break;
			}
			if(!bExistId)
			{
				break;
			}
			Player[CLIENT_PLAYER_INDEX].m_ItemList.RecoverItem(uParam);
			SendClientCmdRecoverItem(uParam);
		}
		break;
	case GOI_GET_CITY_OWN_TONG:
		SendClientCmdGetCityOwnTong();
		break;
	case GOI_RECOVERY_BOX_COMMAND:
		if (uParam)
		{
			DWORD dwID;
			int nW, nH;		
			PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();
			if (pItem && pItem->nPlace == uParam)
			{
				dwID = Item[pItem->nIdx].GetID();
				nW = Item[pItem->nIdx].GetWidth();
				nH = Item[pItem->nIdx].GetHeight();
				
				SendClientRecoveryBox(dwID, nW, nH);
			}
			
			while(pItem)
			{
				pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();
				if (pItem && pItem->nPlace == uParam)
				{
					dwID = Item[pItem->nIdx].GetID();
					nW = Item[pItem->nIdx].GetWidth();
					nH = Item[pItem->nIdx].GetHeight();
									
					SendClientRecoveryBox(dwID, nW, nH);	
				}
			}
		}	
		break;
	case GOI_INPUT_INFO:
		{
			KUiInPutBoxCmd* pInPutCmd = (KUiInPutBoxCmd*)nParam;
			SendClientCmdInputBox(uParam,pInPutCmd->nNum,(char*)pInPutCmd->szAction,(char*)pInPutCmd->nValue);
		}
		break;
	case GOI_NPC_ITEM_BREAK:																		//tach vat pham xep chong
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;
			//
			KUiBreakItemOption* pBreakOption = (KUiBreakItemOption*)nParam;
			//
			int nIdx = pObject1->Obj.uId;
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{
				
				if (Item[nIdx].GetStackNum() > pBreakOption->num)
				{
					//Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - pBreakOption->num);
					SendClientCmdBreak(Item[nIdx].GetID(), pBreakOption->num, pBreakOption->isbreakall);
				}
				else
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_BREAK_ITEM_NOT);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	case GOI_TRADE_NPC_REPAIR:
		{
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;

			if (CGOG_ITEM != pObject1->Obj.uGenre)
				break;
			int nIdx = pObject1->Obj.uId;	//Player[CLIENT_PLAYER_INDEX].m_ItemList.Hand();
			if (nIdx > 0 && nIdx < MAX_ITEM)
			{		
				if (Item[nIdx].GetGenre() != item_equip) //vat pham khong the sua
				{
					return 0;
				}
				else if (Item[nIdx].GetDurability() == -1 || Item[nIdx].GetMaxDurability() == -1 || Item[nIdx].GetDurability() == Item[nIdx].GetMaxDurability()) //#chua bi giam sut do ben
				{
					return 0;
				}
				else if(Item[nIdx].GetDurability() == 0)//#do ben trang bi bang 0 trang bi hong khong sua bang tien van
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_NO_REPAIR_MONEY);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				else if (Item[nIdx].GetRepairPrice() <= Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney())
				{
					SendClientCmdRepair(Item[nIdx].GetID());
				}
				else
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_SHOP_NO_MONEY);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
					return 0;
				}
				
				return 1;
			}
			else
			{
				return 0;
			}
		}
		break;
	case GOI_SWITCH_OBJECT:
		{
			ItemPos	P1, P2;
			int PartConvert[itempart_num] = 
			{ 
				itempart_head,		itempart_weapon,
				itempart_amulet,	itempart_cuff,
				itempart_body,		itempart_belt,
				itempart_ring1,		itempart_ring2,
				itempart_pendant,	itempart_foot,
				itempart_horse, itempart_mask,
				itempart_mantle,	itempart_signet,
				itempart_shipin,	itempart_hoods,
				itempart_cloak,
			};
			int PartTrembleConvert[tremblepart_num] = 
			{
				tremblepart_item,	tremblepart_gemlevel,
				tremblepart_gemspirit,	tremblepart_gemmetal,
				tremblepart_gemwood,	tremblepart_gemwater,
				tremblepart_gemfire,	tremblepart_gemearth,
			};
			KUiObjAtContRegion* pObject1 = (KUiObjAtContRegion*)uParam;
			KUiObjAtContRegion* pObject2 = (KUiObjAtContRegion*)nParam;
			
			if (!pObject1 && !pObject2)
				break;
			
			if (pObject1)
			{
				switch(pObject1->eContainer)
				{
				case UOC_STORE_BOX:
					P1.nPlace = pos_repositoryroom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_EX_BOX1:	// mo rong ruong 1
					P1.nPlace = pos_exbox1room;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_EX_BOX2:	// mo rong ruong 2
					P1.nPlace = pos_exbox2room;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_EX_BOX3:	// mo rong ruong 3
					P1.nPlace = pos_exbox3room;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_ITEM_EX:	// mo rong hanh trang 
					P1.nPlace = pos_equiproomex;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_IMMEDIA_ITEM:
					P1.nPlace = pos_immediacy;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_IMMEDIA_SKILL:
					P1.nPlace = pos_immediacyskill;
					P1.nX = pObject1->Obj.uGenre;
					P1.nY = pObject1->Obj.uId;
					break;
				case UOC_SKILL_TAKE_WITH:
					P1.nPlace = pos_skilltakewith;
					P1.nX = pObject1->Obj.uGenre;
					P1.nY =pObject1->Obj.uId;
					break;
				case UOC_ITEM_TAKE_WITH:
					P1.nPlace = pos_equiproom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_EQUIPTMENT:
					{
						// TODO
						if (pObject1->Region.h == 1)
							break;
						P1.nPlace = pos_equip;
						P1.nX = PartConvert[pObject1->Region.v];
					}
					break;
				case UOC_TO_BE_TRADE:
					P1.nPlace = pos_traderoom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_TO_BE_GAMBLE:
					P1.nPlace = pos_gambleroom;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_AFFAIR_ITEM: // pos tra vat pham nhiem vu
					P1.nPlace = pos_affairitem;
					P1.nX = pObject1->Region.h;
					P1.nY = pObject1->Region.v;
					break;
				case UOC_TREMBLE_ITEM: 
					{
						if (pObject1->Region.h == 1)
							break;
						P1.nPlace = pos_tremble;
						P1.nX = PartTrembleConvert[pObject1->Region.v];
					}
					break;
				case UOC_NPC_SHOP:
					if (CGOG_NPCSELLITEM != pObject1->Obj.uGenre)
						break;

					int nIdx = 0;
					KItem* pItem = NULL;
					
					nIdx = BuySell.GetItemIndex(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop, pObject1->Obj.uId);
					pItem = BuySell.GetItem(nIdx);
					
					int nWidth, nHeight;
					ItemPos	Pos;
					
					nWidth = pItem->GetWidth();
					nHeight = pItem->GetHeight();
					if (!Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(nWidth, nHeight, &Pos))
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					if (Pos.nPlace != pos_equiproom)
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy(sMsg.szMessage, MSG_SHOP_NO_ROOM);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					if (Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipmentMoney() < pItem->GetPrice())
					{
						nRet = 0;
						KSystemMessage	sMsg;
						
						strcpy(sMsg.szMessage, MSG_SHOP_NO_MONEY);
						sMsg.eType = SMT_SYSTEM;
						sMsg.byConfirmType = SMCT_CLICK;
						sMsg.byPriority = 1;
						sMsg.byParamSize = 0;
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
						break;
					}
					
					SendClientCmdBuy(Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop, pObject1->Obj.uId, nParam, 0);
					break;
				}
			}
			
			if (pObject2)
			{
				switch(pObject2->eContainer)
				{
				case UOC_STORE_BOX:
					P2.nPlace = pos_repositoryroom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_EX_BOX1:	// mo rong ruong 1
					P2.nPlace = pos_exbox1room;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_EX_BOX2:	// mo rong ruong 2
					P2.nPlace = pos_exbox2room;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_EX_BOX3:	// mo rong ruong 3
					P2.nPlace = pos_exbox3room;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_ITEM_EX:	// mo rong hanh trang
					P2.nPlace = pos_equiproomex;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_IMMEDIA_ITEM:
					P2.nPlace = pos_immediacy;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_IMMEDIA_SKILL:
					P2.nPlace = pos_immediacyskill;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_SKILL_TAKE_WITH:
					P2.nPlace = pos_skilltakewith;
					P2.nX = pObject1->Obj.uGenre;
					P2.nY =pObject1->Obj.uId;
					break;
				case UOC_ITEM_TAKE_WITH:
					P2.nPlace = pos_equiproom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_EQUIPTMENT:
					{
						if (pObject2->Region.h == 1)
							break;
						P2.nPlace = pos_equip;
						P2.nX = PartConvert[pObject2->Region.v];
					}
					break;
				case UOC_TO_BE_TRADE:
					P2.nPlace = pos_traderoom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_TO_BE_GAMBLE:
					P2.nPlace = pos_gambleroom;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_AFFAIR_ITEM: // pos tra vat pham nhiem vu
					P2.nPlace = pos_affairitem;
					P2.nX = pObject2->Region.h;
					P2.nY = pObject2->Region.v;
					break;
				case UOC_TREMBLE_ITEM: 
					{
						if (pObject2->Region.h == 1)
							break;
						P2.nPlace = pos_tremble;
						P2.nX = PartTrembleConvert[pObject2->Region.v];
					}
					break;
				case UOC_NPC_SHOP:
					break;
				}
			}
			if (!pObject1)
			{
				memcpy(&P1, &P2, sizeof(P1));
			}
			if (!pObject2)
			{
				memcpy(&P2, &P1, sizeof(P1));
			}
			if(P1.nPlace == pos_equiproom)
			{
				Player[CLIENT_PLAYER_INDEX].m_sExtAuto.uFtNextTime = timeGetTime() + 5000;
			}
			Player[CLIENT_PLAYER_INDEX].MoveItem(P1, P2);
		}
		break;

	case GOI_INFORMATION_CONFIRM_NOTIFY:
	{
		PLAYER_SELECTUI_COMMAND command;
		command.nSelectIndex = 0;
		Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_TALKDIALOG);
		break;
	}
	
	case GOI_QUESTION_CHOOSE:
		if (g_bUISelLastSelCount == 0 )
			break;	
		{
			PLAYER_SELECTUI_COMMAND command;
			command.nSelectIndex = nParam;
			Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_SELECTDIALOG);
		}
		break;

	case GOI_SAY_NEW: //Say New
		if (g_bUISelLastSelCount == 0 )
			break;	
		{
			PLAYER_SELECTUI_COMMAND command;
			command.nSelectIndex = nParam;
			Player[CLIENT_PLAYER_INDEX].OnSelectFromUI(&command, UI_SAYNEW);
		}
		break;

	case GOI_LOCK_PLAYER_ITEM:
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			if(pInfo->Obj.uId)
			{
				int lock = nParam;
				SendClientLockPlayerItemCmd(pInfo->Obj.uId, lock);
			}
		}
		break;
	case GOI_EXCHANGEITEM:
		{
		/*if(uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			int nPlace = nParam;
			ItemPos	Pos;
			switch(nPlace)
			{
			case UOC_ITEM_TAKE_WITH:
				Pos.nPlace = pos_equiproom;
				break;
			default:
				Pos.nPlace = -1;
				break;
			}

			Pos.nX = pInfo->Region.h;
			Pos.nY = pInfo->Region.v;

			if ((pInfo->Obj.uGenre == CGOG_ITEM || pInfo->Obj.uGenre == CGOG_IME_ITEM) && pInfo->Obj.uId > 0 && Pos.nPlace != -1)
			{
				Player[CLIENT_PLAYER_INDEX].ApplyAutoMoveItem(pInfo->Obj.uId, Pos);					
			}
		}*/

			UINT* pParam = (UINT*)uParam;
			int nItemID = *pParam;
			int nSrcPos = *(pParam+1);
			if(nItemID <= 0 || nItemID >= MAX_ITEM)
				break;
			if(nParam <= 0)
				break;
			char szPack[16];
			DYNAMIC_COMMAND* pCmd = (DYNAMIC_COMMAND*)&szPack[0];
			pCmd->ProtocolType = c2s_dynamic_structure;
			pCmd->nBranch = c2sdnmbr_exchangeitem;
			pCmd->m_wLength = sizeof(DYNAMIC_COMMAND) - 1 + 2*sizeof(BYTE) + sizeof(int);
			BYTE* pPos = (BYTE*)(pCmd+1);
			*pPos = nSrcPos;
			pPos++;
			*pPos = nParam;
			pPos++;
			*(int*)pPos = Item[nItemID].GetID();
			if (g_pClient)
				g_pClient->SendPackToServer(pCmd, pCmd->m_wLength + 1);
		}
		break;
	case GOI_USE_ITEM:
		//to do: waiting for...
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			int nPlace = nParam;
			ItemPos	Pos;
			switch(nPlace)
			{
			case UOC_ITEM_TAKE_WITH:
				Pos.nPlace = pos_equiproom;
				break;
			case UOC_IMMEDIA_ITEM:
				Pos.nPlace = pos_immediacy;
				break;
			case UOC_EQUIPTMENT:
				if(Player[CLIENT_PLAYER_INDEX].m_nActiveEquipNum == 1)
				Pos.nPlace = pos_equip;
				else
				Pos.nPlace = pos_equipback;
				break;
			default:
				Pos.nPlace = -1;
				break;
			}

			Pos.nX = pInfo->Region.h;
			Pos.nY = pInfo->Region.v;

			if ((pInfo->Obj.uGenre == CGOG_ITEM || pInfo->Obj.uGenre == CGOG_IME_ITEM) && pInfo->Obj.uId > 0 && Pos.nPlace != -1)
			{
				if(Pos.nPlace == pos_immediacy && Item[pInfo->Obj.uId].IsFkItemSkill())
					CoreDataChanged(GDCNI_USE_SHORCUT_SKILL, Item[pInfo->Obj.uId].GetParticular(), 0);
				else
					Player[CLIENT_PLAYER_INDEX].ApplyUseItem(pInfo->Obj.uId, Pos, pInfo->Region.Width);
			}
		}
		break;

	case GOI_WEAR_EQUIP:
		//to do: waiting for...
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
		}
		break;

	case GOI_USE_SKILL:	
		if (uParam)
		{
			//to do:wating for...
			KUiGameObject* pInfo = (KUiGameObject*)uParam;
		}
		break;

	case GOI_SET_IMMDIA_SKILL:
		if (uParam)
		{
			KUiGameObject* pSkill = (KUiGameObject*)uParam;
			if (nParam == 0)
				//to do : modify;
			{
				if ( (int)pSkill->uId > 0 )
					Player[CLIENT_PLAYER_INDEX].SetLeftSkill((int)pSkill->uId);
				else if ((int)(pSkill->uId) == -1) 
				{
					int nDetailType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponType();
					int nParticularType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponParticular();
					
					if (nDetailType == 0)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nMeleeWeaponSkill[nParticularType]);
					}
					else if (nDetailType == 1)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nRangeWeaponSkill[nParticularType]);
					}
					else if (nDetailType == -1)
					{
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(g_nHandSkill);
					}
					
				}
			}
			else if (nParam == 1)
			{
				if ((int)pSkill->uId > 0)
					Player[CLIENT_PLAYER_INDEX].SetRightSkill((int)pSkill->uId);
				else if ((int)(pSkill->uId) == -1)
				{
					int nDetailType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponType();
					int nParticularType = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetWeaponParticular();
					
					if (nDetailType == 0)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nMeleeWeaponSkill[nParticularType]);
					}
					else if (nDetailType == 1)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nRangeWeaponSkill[nParticularType]);
					}
					else if (nDetailType == -1)
					{
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(g_nHandSkill);
					}
				}
			}
		}
		break;

	case GOI_TONE_UP_SKILL:
		Player[CLIENT_PLAYER_INDEX].ApplyAddSkillLevel((int)nParam, 1);
		break;

	case GOI_TONE_UP_ATTRIBUTE:
		int numpoint2;
		numpoint2 = nParam;
		if(numpoint2 <= 0 || numpoint2 >= 256)
			break;
		switch (uParam)
		{
		case UIPA_STRENGTH:		
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(0, numpoint2);//Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(0, 1);
			break;
		case UIPA_DEXTERITY:	
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(1, numpoint2);//Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(1, 1);
			break;		
		case UIPA_VITALITY:		
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(2, numpoint2);//Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(2, 1);
			break;
		case UIPA_ENERGY:		
			Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(3, numpoint2);//Player[CLIENT_PLAYER_INDEX].ApplyAddBaseAttribute(3, 1);
			break;		
		}
		break;

	case GOI_TRADE_INVITE_RESPONSE:
		if (uParam)
		{
			KTrade::ReplyInvite(((KUiPlayerItem*)uParam)->nIndex, nParam);
		}
		break;
	case GOI_GAMBLE_INVITE_RESPONSE:
		if (uParam)
		{
			KTrade::ReplyGambleInvite(((KUiPlayerItem*)uParam)->nIndex, nParam);
		}
		break;
	case GOI_TRADE_DESIRE_ITEM:
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*) uParam;
			if (pInfo->Obj.uGenre != CGOG_MONEY)
				break;
			Player[CLIENT_PLAYER_INDEX].TradeMoveMoney(pInfo->Obj.uId);
		}
		break;
	case GOI_GAMBLE_DESIRE_ITEM:
		if (uParam)
		{
			KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;
			if (pInfo->Obj.uGenre != CGOG_MONEY)
				break;
			Player[CLIENT_PLAYER_INDEX].GambleMoveMoney(pInfo->Obj.uId);
		}
		break;
	case GOI_TRADE_WILLING: //giao dich giao dÞch
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMenuState() == PLAYER_MENU_STATE_TRADEOPEN)
		{
			Player[CLIENT_PLAYER_INDEX].TradeApplyClose();
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].TradeApplyOpen((char*)uParam, nParam);
		}
		break;
	case GOI_GAMBLE_WILLING: //giao dich giao dÞch
		//if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMenuState() == PLAYER_MENU_STATE_TRADEOPEN)
		//{
		//	Player[CLIENT_PLAYER_INDEX].TradeApplyClose();
		//}
		//else
		//{
		//	Player[CLIENT_PLAYER_INDEX].TradeApplyOpen((char*)uParam, nParam);
		//}
		break;
	case GOI_TRADE_LOCK:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock)
			Player[CLIENT_PLAYER_INDEX].TradeApplyLock(0);
		else
			Player[CLIENT_PLAYER_INDEX].TradeApplyLock(1);
		break;
	case GOI_GAMBLE_LOCK:
		if (!Player[CLIENT_PLAYER_INDEX].CheckTrading())
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock)
			Player[CLIENT_PLAYER_INDEX].GambleApplyLock(0);
		else
			Player[CLIENT_PLAYER_INDEX].GambleApplyLock(1);
		break;
	case GOI_TRADE:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock != 1 || Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock != 1)
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState == 0)
		{
			Player[CLIENT_PLAYER_INDEX].TradeDecision(1);		
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].TradeDecision(2);		
		}
		break;
	case GOI_GAMBLE:
		if (!Player[CLIENT_PLAYER_INDEX].CheckTrading())
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeLock != 1 || Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeDestLock != 1)
			break;
		if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState == 0)
		{
			Player[CLIENT_PLAYER_INDEX].GambleDecision(1, nParam);
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].GambleDecision(2);
		}
		break;
	case GOI_TRADE_CANCEL:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			break;
		Player[CLIENT_PLAYER_INDEX].TradeDecision(0);		
		break;
	case GOI_GAMBLE_CANCEL:
		if (!Player[CLIENT_PLAYER_INDEX].CheckTrading())
			break;
		Player[CLIENT_PLAYER_INDEX].GambleDecision(0);
		break;
	case GOI_AUTOPLAY_ACTION:
		{
			nRet = 0;
			int nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			int nPlayerIdx = CLIENT_PLAYER_INDEX;
			UINT uCurTime = timeGetTime();
			switch(uParam)
			{
				case ATYPE_PUMPLIFE:
				{
					int* pValue = (int*)nParam;
					if(Player[nPlayerIdx].m_sExtAuto.uLTime1 < uCurTime)
					{
						if(Npc[nNpcIdx].m_CurrentLife < Npc[nNpcIdx].m_CurrentLifeMax
						 && Npc[nNpcIdx].m_CurrentLife < pValue[0])
						{
							Player[nPlayerIdx].m_sExtAuto.uLTime1 = uCurTime + pValue[2];
							for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
							for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
							{
								int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
								if(nIdx > 0)
								{
									int nGenre = Item[nIdx].GetGenre();
									int nDetail = Item[nIdx].GetDetailType();
									if(nGenre == item_medicine
									&& (nDetail == 0 || nDetail == 2
									|| (nDetail == 8 && Item[nIdx].GetLevel() == 4)))
									{
										ItemPos	Pos;
										Pos.nPlace = pos_equiproom;
										Pos.nX = j;
										Pos.nY = i;
										Player[nPlayerIdx].ApplyUseItem(nIdx, Pos);
										return 1;
									}
								}
							}
						}
					}
					if(Player[nPlayerIdx].m_sExtAuto.uLTime2 < uCurTime)
					{
						if(Npc[nNpcIdx].m_CurrentLife < Npc[nNpcIdx].m_CurrentLifeMax
						 && Npc[nNpcIdx].m_CurrentLife < pValue[1])
						{
							Player[nPlayerIdx].m_sExtAuto.uLTime2 = uCurTime + pValue[2];
							for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
							for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
							{
								int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
								if(nIdx > 0)
								{
									int nGenre = Item[nIdx].GetGenre();
									int nDetail = Item[nIdx].GetDetailType();
									if(nGenre == item_medicine
									&& (nDetail == 0 || nDetail == 2))
									{
										ItemPos	Pos;
										Pos.nPlace = pos_equiproom;
										Pos.nX = j;
										Pos.nY = i;
										Player[nPlayerIdx].ApplyUseItem(nIdx, Pos);
										return 1;
									}
								}
							}
						}
					}
					break;
				}
				case ATYPE_PUMPMANA:
				{
					int* pValue = (int*)nParam;
					if(Player[nPlayerIdx].m_sExtAuto.uMTime1 < uCurTime)
					{
						if(Npc[nNpcIdx].m_CurrentMana < Npc[nNpcIdx].m_CurrentManaMax
						 && Npc[nNpcIdx].m_CurrentMana < pValue[0])
						{
							Player[nPlayerIdx].m_sExtAuto.uMTime1 = uCurTime + pValue[2];
							for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
							for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
							{
								int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
								if(nIdx > 0)
								{
									int nGenre = Item[nIdx].GetGenre();
									int nDetail = Item[nIdx].GetDetailType();
									if(nGenre == item_medicine
									&& (nDetail == 1 || nDetail == 2))
									{
										ItemPos	Pos;
										Pos.nPlace = pos_equiproom;
										Pos.nX = j;
										Pos.nY = i;
										Player[nPlayerIdx].ApplyUseItem(nIdx, Pos);
										return 1;
									}
								}
							}
						}
					}
					if(Player[nPlayerIdx].m_sExtAuto.uMTime2 < uCurTime)
					{
						if(Npc[nNpcIdx].m_CurrentMana < Npc[nNpcIdx].m_CurrentManaMax
						 && Npc[nNpcIdx].m_CurrentMana < pValue[1])
						{
							Player[nPlayerIdx].m_sExtAuto.uMTime2 = uCurTime + pValue[2];
							for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
							for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
							{
								int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
								if(nIdx > 0)
								{
									int nGenre = Item[nIdx].GetGenre();
									int nDetail = Item[nIdx].GetDetailType();
									if(nGenre == item_medicine
									&& (nDetail == 1 || nDetail == 2))
									{
										ItemPos	Pos;
										Pos.nPlace = pos_equiproom;
										Pos.nX = j;
										Pos.nY = i;
										Player[nPlayerIdx].ApplyUseItem(nIdx, Pos);
										return 1;
									}
								}
							}
						}
					}
					break;
				}
				case ATYPE_TP_CHECKLIFE:
				case ATYPE_TP_CHECKMANA:
				case ATYPE_TP_LIFEGONE:
				case ATYPE_TP_MANAGONE:
				case ATYPE_TP_FULLITEM:
				case ATYPE_TP_FULLMONEY:
				case ATYPE_TP_DMGITEM:
				{
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					// (21/08 DaTau) may Da Tau dang CAM LAI (farm / ban rac / ve thanh...) thi tu lo
					// chuyen tui day (DTP_FARM <5 o -> DTP_SELLJUNK ban tai cho, het cach moi phu ve
					// + nhuong Hau can). TP "ve thanh khi tui day" bat giua luc FARM lam may tuong bi
					// da khoi map -> ra Xa Phu di lai khong ban -> "phu ve - xa phu - len map - phu
					// ve" lap vo tan (loi nguoi dung 21/08). Chi chan khi DT_Process vua chay trong
					// nhip nay (tat o Da Tau / treo / nhuong may = TP chay lai binh thuong).
					if(uParam == ATYPE_TP_FULLITEM && Player[nPlayerIdx].m_sExtAuto.nDTEngaged
					&& uCurTime - g_uDTTickT < 1500)
						return 0;

					static int aMap[42] = {387,388,389,390,391,392,393,394,395,375,386,416,511,995,44,197,208,209,210,211,213,223,
														341,342,175,337,338,339,379,324,481,482,483,484,485,486,487,488,489,399,397,396};
					for(int i=0;i<42;++i)
					{
						if(SubWorld[0].m_SubWorldID == aMap[i])
							return 0;
					}
					bool bUseTP = false;
					if(uParam == ATYPE_TP_CHECKLIFE && Npc[nNpcIdx].m_CurrentLife < nParam)
						bUseTP = true;
					else if(uParam == ATYPE_TP_CHECKMANA && Npc[nNpcIdx].m_CurrentMana < nParam)
						bUseTP = true;
					else if(uParam == ATYPE_TP_LIFEGONE)
					{
						bool bFound = false;
						for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
						{
							for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
							{
								int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
								if(nIdx > 0)
								{
									int nGenre = Item[nIdx].GetGenre();
									int nDetail = Item[nIdx].GetDetailType();
									if(nGenre == item_medicine
									&& (nDetail == 0 || nDetail == 2))
									{
										bFound = true;
										break;
									}
								}
							}
							if(bFound)
								break;
						}
						if(!bFound)
							bUseTP = true;
					}
					else if(uParam == ATYPE_TP_MANAGONE)
					{
						bool bFound = false;
						for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
						{
							for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
							{
								int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
								if(nIdx > 0)
								{
									int nGenre = Item[nIdx].GetGenre();
									int nDetail = Item[nIdx].GetDetailType();
									if(nGenre == item_medicine
									&& (nDetail == 1 || nDetail == 2))
									{
										bFound = true;
										break;
									}
								}
							}
							if(bFound)
								break;
						}
						if(!bFound)
							bUseTP = true;
					}
					else if(uParam == ATYPE_TP_FULLITEM)
					{
						int x, y;
						if(nParam == 0)
						{
							if(!Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(1, 1, &x, &y))
								bUseTP = true;
						}
						else if(nParam == 1)
						{
							if(!Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(2, 2, &x, &y))
								bUseTP = true;
						}
						else if(nParam == 2)
						{
							if(!Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(2, 3, &x, &y))
								bUseTP = true;
						}
						else if(nParam == 3)
						{
							if(!Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(2, 4, &x, &y))
								bUseTP = true;
						}
					}
					else if(uParam == ATYPE_TP_FULLMONEY)
					{
						if(Player[nPlayerIdx].m_ItemList.GetMoney(room_equipment) > nParam*10000)
							bUseTP = true;
					}
					else if(uParam == ATYPE_TP_DMGITEM)
					{
						if(nParam > 0)
						{
							for (int k = 0; k < itempart_horse; ++k)
							{
								int nIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(k);
								if(nIdx > 0)
								{
									int nDur = Item[nIdx].GetDurability();
									if(nDur > 0 && nDur < nParam)
									{
										bUseTP = true;
										break;
									}
								}
							}
						}
					}
					if(bUseTP)
					{
						for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
						for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
						{
							int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
							if(nIdx > 0)
							{
								int nGenre = Item[nIdx].GetGenre();
								int nDetail = Item[nIdx].GetDetailType();
								int nPart = Item[nIdx].GetParticular();
								if(nGenre == item_townportal
								|| (nGenre == item_magicscript && nDetail == 1 && (nPart == 437 || nPart == 1083 || nPart == 1084)))
								{
									Player[nPlayerIdx].m_sExtAuto.bJustTP = TRUE;
									ItemPos	Pos;
									Pos.nPlace = pos_equiproom;
									Pos.nX = j;
									Pos.nY = i;
									Player[nPlayerIdx].ApplyUseItem(nIdx, Pos);
									return 1;
								}
							}
						}
					}
					break;
				}
				case ATYPE_CLEAR:
				{
					g_ScenePlace.RemoveFlag();
					Player[nPlayerIdx].m_mAutoExcludeNpcID.clear();
					Player[nPlayerIdx].m_mAutoIDObj.clear();
					Player[nPlayerIdx].m_mAutoIDTeam.clear();
					Player[nPlayerIdx].m_mAutoTeamRecv.clear();
					Player[nPlayerIdx].m_vAutoTeamKick.clear();
					memset(&Player[nPlayerIdx].m_sExtAuto, 0, sizeof(ExtAuto));
					Player[nPlayerIdx].m_sExtAuto.uChatTime = uCurTime + 5*1000;
					if(!nParam && nNpcIdx > 0)
					{
						int nX, nY;
						Npc[nNpcIdx].GetMpsPos(&nX, &nY);
						Npc[nNpcIdx].SendCommand(do_run, nX, nY);
						SendClientCmdRun(nX, nY);
					}
					break;
				}
				case ATYPE_CHECKTIME:
				{
					if(!Player[nPlayerIdx].m_sExtAuto.uUnFightTime)
					{
						Player[nPlayerIdx].m_sExtAuto.uUnFightTime = uCurTime;
						if(nNpcIdx > 0)
							Player[nPlayerIdx].m_sExtAuto.bPrevFightState = Npc[nNpcIdx].m_FightMode;
					}
					else
					{
						if(nNpcIdx > 0)
						{
							Player[nPlayerIdx].m_sExtAuto.bJustDis = FALSE;
							if(Player[nPlayerIdx].m_sExtAuto.bPrevFightState != Npc[nNpcIdx].m_FightMode)
							{
								Player[nPlayerIdx].m_sExtAuto.bPrevFightState = Npc[nNpcIdx].m_FightMode;
								Player[nPlayerIdx].m_sExtAuto.uUnFightTime = uCurTime;
								if(Player[nPlayerIdx].m_sExtAuto.bPrevFightState)
								{
									CoreDataChanged(GDCNI_UI_ACT, 1, 0);
									Player[nPlayerIdx].m_sExtAuto.bJustTP = FALSE;
									Player[nPlayerIdx].m_sExtAuto.nTempX = 0;
									Player[nPlayerIdx].m_sExtAuto.nTempY = 0;
									Player[nPlayerIdx].m_sExtAuto.bReachDes = FALSE;
									Player[nPlayerIdx].m_sExtAuto.uTJustMove = uCurTime + 8000;
								}
								else
								{
									Player[nPlayerIdx].m_sExtAuto.nHomeStep = 0;
									Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
								}
							}
						}
						else if(!Player[nPlayerIdx].m_sExtAuto.bJustDis)
						{
							Player[nPlayerIdx].m_sExtAuto.bJustDis = TRUE;
							Player[nPlayerIdx].m_sExtAuto.uUnFightTime = uCurTime;
						}
					}
					for (std::map<UINT,UINT>::iterator it = Player[nPlayerIdx].m_mAutoExcludeNpcID.begin();
						it != Player[nPlayerIdx].m_mAutoExcludeNpcID.end();)
					{
						UINT uTime = it->second;
						if(uTime < uCurTime)
						{
							Player[nPlayerIdx].m_mAutoExcludeNpcID.erase(it++);
						}
						else
						{
							++it;
						}
					}
					break;
				}
				case ATYPE_TP_EXIT:
				{
					if(Player[nPlayerIdx].m_sExtAuto.bJustTP && !Player[nPlayerIdx].m_sExtAuto.bPrevFightState)
						return 1;
					break;
				}
				case ATYPE_DISEXIT:
				{
					if(Player[nPlayerIdx].m_sExtAuto.bJustDis
					|| (nNpcIdx > 0 && !Player[nPlayerIdx].m_sExtAuto.bPrevFightState))
					{
						if(uCurTime - Player[nPlayerIdx].m_sExtAuto.uUnFightTime > 14*60*1000)
							return 1;
					}
					break;
				}
				case ATYPE_CANCHAT:
				{
					if(Player[nPlayerIdx].m_sExtAuto.uChatTime < uCurTime)
					{
						if(nParam == 0)
							Player[nPlayerIdx].m_sExtAuto.uChatTime = uCurTime + 12*1000;
						else
							Player[nPlayerIdx].m_sExtAuto.uChatTime = uCurTime + 62*1000;
						return 1;
					}
					break;
				}
				case ATYPE_EATLIFEFULL:
				{
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					int x, y;
					if(!Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(2, 3, &x, &y))
					{
						for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
						for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
						{
							int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
							if(nIdx > 0)
							{
								int nGenre = Item[nIdx].GetGenre();
								int nDetail = Item[nIdx].GetDetailType();
								if(nGenre == item_medicine
								&& (nDetail == 0 || nDetail == 1 || nDetail == 2 || nDetail == 4))
								{
									ItemPos	Pos;
									Pos.nPlace = pos_equiproom;
									Pos.nX = j;
									Pos.nY = i;
									Player[nPlayerIdx].ApplyUseItem(nIdx, Pos);
									return 1;
								}
							}
						}
					}
					break;
				}
				case ATYPE_EATPOISON:
				{
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					if(Player[nPlayerIdx].m_sExtAuto.uPoisonTime < uCurTime)
					{
						Player[nPlayerIdx].m_sExtAuto.uPoisonTime = uCurTime + 500;
						if(Npc[nNpcIdx].m_PoisonState.nTime > 0)
						{
						for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
						for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
						{
							int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
							if(nIdx > 0)
							{
								int nGenre = Item[nIdx].GetGenre();
								int nDetail = Item[nIdx].GetDetailType();
								if(nGenre == item_medicine && nDetail == 4)
								{
									ItemPos	Pos;
									Pos.nPlace = pos_equiproom;
									Pos.nX = j;
									Pos.nY = i;
									Player[nPlayerIdx].ApplyUseItem(nIdx, Pos);
									return 1;
								}
							}
						}}
					}
					break;
				}
				case ATYPE_EATEXPX2:
				{
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					if(Player[nPlayerIdx].m_sExtAuto.uExp2Time >= uCurTime)
						return 0;
					Player[nPlayerIdx].m_sExtAuto.uExp2Time = uCurTime + 5000;
					KStateNode* pNode = (KStateNode *)Npc[nNpcIdx].m_StateSkillList.GetHead();
					while(pNode)
					{
						if(pNode->m_SkillID == 440)
							return 0;
						pNode = (KStateNode *)pNode->GetNext();
					}
					for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
					for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
					{
						int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
						if(nIdx > 0)
						{
							int nGenre = Item[nIdx].GetGenre();
							int nDetail = Item[nIdx].GetDetailType();
							int nPart = Item[nIdx].GetParticular();
							if(nGenre == item_magicscript && nDetail == 1
							&& (nPart == 71 || nPart == 70 || nPart == 1182))
							{
								ItemPos	Pos;
								Pos.nPlace = pos_equiproom;
								Pos.nX = j;
								Pos.nY = i;
								Player[nPlayerIdx].ApplyUseItem(nIdx, Pos);
								return 1;
							}
						}
					}
					break;
				}
				case ATYPE_EATSKILLX2:
				{
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					if(Player[nPlayerIdx].m_sExtAuto.uSkillExp2Time >= uCurTime)
						return 0;
					Player[nPlayerIdx].m_sExtAuto.uSkillExp2Time = uCurTime + 6000;
					KStateNode* pNode = (KStateNode *)Npc[nNpcIdx].m_StateSkillList.GetHead();
					while(pNode)
					{
						if(pNode->m_SkillID == 1555)
							return 0;
						pNode = (KStateNode *)pNode->GetNext();
					}
					for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
					for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
					{
						int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
						if(nIdx > 0)
						{
							int nGenre = Item[nIdx].GetGenre();
							int nDetail = Item[nIdx].GetDetailType();
							int nPart = Item[nIdx].GetParticular();
							if(nGenre == item_magicscript && nDetail == 1
							&& (nPart == 4822 || nPart == 4823))
							{
								ItemPos	Pos;
								Pos.nPlace = pos_equiproom;
								Pos.nX = j;
								Pos.nY = i;
								Player[nPlayerIdx].ApplyUseItem(nIdx, Pos);
								return 1;
							}
						}
					}
					break;
				}
				case ATYPE_BASEBUFF:
				{
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					if(Player[nPlayerIdx].m_sExtAuto.uBaseBuffTime >= uCurTime)
						return 0;
					Player[nPlayerIdx].m_sExtAuto.uBaseBuffTime = uCurTime + 500;
					int nSkillIdx = Npc[nNpcIdx].m_SkillList.FindSame(93);
					if(!nSkillIdx)
						return 0;
					int* pValue = (int*)nParam;
					if((Npc[nNpcIdx].m_CurrentLife < Npc[nNpcIdx].m_CurrentLifeMax)
					&& (Npc[nNpcIdx].m_CurrentLife <= Npc[nNpcIdx].m_CurrentLifeMax - pValue[0]))
					{//Nga my buff minh truoc
						Npc[nNpcIdx].SendCommand(do_skill, 93, -1, nNpcIdx);
						SendClientCmdSkill(93, -1, Npc[nNpcIdx].m_dwID);
						return 1;
					}
					if(pValue[1]) //buff pt sau
					{
						KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(93,
											Npc[nNpcIdx].m_SkillList.m_Skills[nSkillIdx].SkillLevel);
						if(!pSkill || !Player[nPlayerIdx].m_cTeam.m_nFlag)
							return 0;
						int nX, nY, dX, dY;
						Npc[nNpcIdx].GetMpsPos(&nX, &nY);
						int nIndex = NpcSet.SearchID(g_Team[0].m_nCaptain);
						if(nIndex > 0 && Npc[nIndex].m_RegionIndex >= 0) //buff doi truong
						{
							Npc[nIndex].GetMpsPos(&dX, &dY);
							if(g_GetDistance(nX, nY, dX, dY) < pSkill->GetAttackRadius())
							{
								if((Npc[nIndex].m_CurrentLife < Npc[nIndex].m_CurrentLifeMax)
								&& (Npc[nIndex].m_CurrentLife <= Npc[nIndex].m_CurrentLifeMax - pValue[0]))
								{
									Npc[nNpcIdx].SendCommand(do_skill, 93, -1, nIndex);
									SendClientCmdSkill(93, -1, Npc[nIndex].m_dwID);
									return 1;
								}
							}
						}
						for (int i = 0; i < MAX_TEAM_MEMBER; ++i) //buff mem
						{
							nIndex = NpcSet.SearchID(g_Team[0].m_nMember[i]);
							if(nIndex > 0 && Npc[nIndex].m_RegionIndex >= 0)
							{
								Npc[nIndex].GetMpsPos(&dX, &dY);
								if(g_GetDistance(nX, nY, dX, dY) < pSkill->GetAttackRadius())
								{
									if((Npc[nIndex].m_CurrentLife < Npc[nIndex].m_CurrentLifeMax)
									&& (Npc[nIndex].m_CurrentLife <= Npc[nIndex].m_CurrentLifeMax - pValue[0]))
									{
										Npc[nNpcIdx].SendCommand(do_skill, 93, -1, nIndex);
										SendClientCmdSkill(93, -1, Npc[nIndex].m_dwID);
										return 1;
									}
								}
							}
						}
					}
					break;
				}
				case ATYPE_CLBUFF:
				{
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					if(Player[nPlayerIdx].m_sExtAuto.uCLBuffTime >= uCurTime)
						return 0;
					Player[nPlayerIdx].m_sExtAuto.uCLBuffTime = uCurTime + 500;
					int nX, nY;
					Npc[nNpcIdx].GetMpsPos(&nX, &nY);
					static int arID[3] = {171,173,178};
					for(int k=0;k<3;++k)
					{
						int nSkillIdx = Npc[nNpcIdx].m_SkillList.FindSame(arID[k]);
						if(!nSkillIdx)
							continue;
						KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(arID[k],
											Npc[nNpcIdx].m_SkillList.m_Skills[nSkillIdx].SkillLevel);
						if(!pSkill)
							continue;
						int nStateId = pSkill->GetStateSpecailId();
						if(!nStateId)
							continue;
						int nSkillRad = pSkill->GetAttackRadius();
						if(nParam)	//camp
						{
							int nIdx = 0;
							int x,y;
							while(nIdx = NpcSet.GetNextIdx(nIdx))
							{
								if(nIdx == nNpcIdx || !Npc[nIdx].m_dwID || Npc[nIdx].m_RegionIndex < 0
								|| Npc[nIdx].m_Doing == do_death || Npc[nIdx].m_Doing == do_revive)
									continue;
								if(NpcSet.GetRelation(nNpcIdx, nIdx) == relation_enemy)
									continue;
								if(Npc[nIdx].m_Kind != kind_player)
									continue;
								if(Npc[nIdx].m_CurrentCamp != Npc[nNpcIdx].m_CurrentCamp)
									continue;
								Npc[nIdx].GetMpsPos(&x, &y);
								int nDist = g_GetDistance(nX, nY, x, y);
								if(nDist < nSkillRad)
								{
									bool bCastExist = false;
									for(int s=0;s<MAX_SKILL_STATE;++s)
									{
										if(!Npc[nIdx].m_btStateInfo[s])
											break;
										if(Npc[nIdx].m_btStateInfo[s] == (BYTE)nStateId)
										{
											bCastExist = true;
											break;
										}
									}
									if(bCastExist)
										continue;
									Npc[nNpcIdx].SendCommand(do_skill, arID[k], -1, nIdx);
									SendClientCmdSkill(arID[k], -1, Npc[nIdx].m_dwID);
									return 1;
								}
							}
						}
						else	//team
						{
							int dX, dY;
							int nIndex = NpcSet.SearchID(g_Team[0].m_nCaptain);
							if(nIndex > 0 && Npc[nIndex].m_RegionIndex >= 0) //buff doi truong
							{
								Npc[nIndex].GetMpsPos(&dX, &dY);
								if(g_GetDistance(nX, nY, dX, dY) < nSkillRad)
								{
									bool bCastExist = false;
									for(int s=0;s<MAX_SKILL_STATE;++s)
									{
										if(!Npc[nIndex].m_btStateInfo[s])
											break;
										if(Npc[nIndex].m_btStateInfo[s] == (BYTE)nStateId)
										{
											bCastExist = true;
											break;
										}
									}
									if(!bCastExist)
									{
										Npc[nNpcIdx].SendCommand(do_skill, arID[k], -1, nIndex);
										SendClientCmdSkill(arID[k], -1, Npc[nIndex].m_dwID);
										return 1;
									}
								}
							}
							for (int i = 0; i < MAX_TEAM_MEMBER; ++i) //buff mem
							{
								nIndex = NpcSet.SearchID(g_Team[0].m_nMember[i]);
								if(nIndex > 0 && Npc[nIndex].m_RegionIndex >= 0)
								{
									Npc[nIndex].GetMpsPos(&dX, &dY);
									if(g_GetDistance(nX, nY, dX, dY) < nSkillRad)
									{
										bool bCastExist = false;
										for(int s=0;s<MAX_SKILL_STATE;++s)
										{
											if(!Npc[nIndex].m_btStateInfo[s])
												break;
											if(Npc[nIndex].m_btStateInfo[s] == (BYTE)nStateId)
											{
												bCastExist = true;
												break;
											}
										}
										if(!bCastExist)
										{
											Npc[nNpcIdx].SendCommand(do_skill, arID[k], -1, nIndex);
											SendClientCmdSkill(arID[k], -1, Npc[nIndex].m_dwID);
											return 1;
										}
									}
								}
							}
						}
					}
					break;
				}
				case ATYPE_OPENBAG:
				{
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					if(Player[nPlayerIdx].m_sExtAuto.uOpenBagTime >= uCurTime)
						return 0;
					Player[nPlayerIdx].m_sExtAuto.uOpenBagTime = uCurTime + 3000;
					if(Player[nPlayerIdx].m_ItemList.CountCommonItem(0, item_medicine, 2) <= 6)
					{
					for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
					for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
					{
						int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
						if(nIdx > 0)
						{
							int nGenre = Item[nIdx].GetGenre();
							int nDetail = Item[nIdx].GetDetailType();
							int nPart = Item[nIdx].GetParticular();
							if(nGenre == item_magicscript && nDetail == 1 && nPart == 4813)
							{
								ItemPos	Pos;
								Pos.nPlace = pos_equiproom;
								Pos.nX = j;
								Pos.nY = i;
								Player[nPlayerIdx].ApplyUseItem(nIdx, Pos);
								return 1;
							}
						}
					}}
					break;
				}
				case ATYPE_SUPPORTBUFF:
				{
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					KStateNode* pNode = (KStateNode *)Npc[nNpcIdx].m_StateSkillList.GetHead();
					while(pNode)
					{
						if(pNode->m_SkillID == nParam)
							return 0;
						pNode = (KStateNode *)pNode->GetNext();
					}
					int nSkillIdx = Npc[nNpcIdx].m_SkillList.FindSame(nParam);
					if(!nSkillIdx)
						return 0;
					if(Npc[nNpcIdx].m_SkillList.m_Skills[nSkillIdx].NextCastTime > SubWorld[0].m_dwCurrentTime)
						return 0;
					Npc[nNpcIdx].SendCommand(do_skill, nParam, -1, nNpcIdx);
					SendClientCmdSkill(nParam, -1, Npc[nNpcIdx].m_dwID);
					return 1;
				}
				case ATYPE_LEFTSKILL:
				{
					if(Player[nPlayerIdx].GetLeftSkill() == nParam)
						return 0;
					KUiGameObject	Skill;
					Skill.uGenre = CGOG_SKILL_FIGHT;
					Skill.uId = nParam;
					OperationRequest(GOI_SET_IMMDIA_SKILL,
						(unsigned int)&Skill, 0);
					return 1;
				}
				case ATYPE_RIGHTSKILL:
				{
					if(Player[nPlayerIdx].GetRightSkill() == nParam)
						return 0;
					KUiGameObject	Skill;
					Skill.uGenre = CGOG_SKILL_FIGHT;
					Skill.uId = nParam;
					OperationRequest(GOI_SET_IMMDIA_SKILL,
						(unsigned int)&Skill, 1);
					return 1;
				}
				case ATYPE_CHANGEAURA:
				{
					int nTime = g_SubWorldSet.GetGameTime();
					if(Player[nPlayerIdx].m_sExtAuto.nAuraTime > nTime)
						return 0;
					Player[nPlayerIdx].m_sExtAuto.nAuraTime = nTime + 10;
					int* pValue = (int*)nParam;
					Player[nPlayerIdx].m_sExtAuto.bChangeAura = !Player[nPlayerIdx].m_sExtAuto.bChangeAura;
					if(Player[nPlayerIdx].m_sExtAuto.bChangeAura)
					{
						if(pValue[0] <= 0 || Player[nPlayerIdx].GetRightSkill() == pValue[0])
							return 0;
						KUiGameObject	Skill;
						Skill.uGenre = CGOG_SKILL_FIGHT;
						Skill.uId = pValue[0];
						OperationRequest(GOI_SET_IMMDIA_SKILL,
							(unsigned int)&Skill, 1);
					}
					else
					{
						if(pValue[1] <= 0 || Player[nPlayerIdx].GetRightSkill() == pValue[1])
							return 0;
						KUiGameObject	Skill;
						Skill.uGenre = CGOG_SKILL_FIGHT;
						Skill.uId = pValue[1];
						OperationRequest(GOI_SET_IMMDIA_SKILL,
							(unsigned int)&Skill, 1);
					}
					return 1;
				}
				case ATYPE_FIGHT:
				{
					if(!Player[nPlayerIdx].m_sExtAuto.bPrevFightState)
						return 0;
					const autoData* pApData = (autoData*)nParam;
					int nX, nY, x, y;
					BOOL bNewFound = FALSE;
					Npc[nNpcIdx].GetMpsPos(&nX, &nY);
					int nTGNpcIdx = 0;
					if(Player[nPlayerIdx].m_sExtAuto.uNpcID)
					{
						nTGNpcIdx = NpcSet.SearchID(Player[nPlayerIdx].m_sExtAuto.uNpcID);
						if(!nTGNpcIdx || Npc[nTGNpcIdx].m_RegionIndex < 0
						|| Npc[nTGNpcIdx].m_Doing == do_death || Npc[nTGNpcIdx].m_Doing == do_revive
							|| !(NpcSet.GetRelation(nNpcIdx, nTGNpcIdx) == relation_enemy))
							Player[nPlayerIdx].m_sExtAuto.uNpcID = 0;
					}
					if(!Player[nPlayerIdx].m_sExtAuto.uNpcID)
					{
						int Ox = 0,Oy = 0;
						if(Player[nPlayerIdx].m_sExtAuto.nCurMoveRet == 1)
						{
							Ox = pApData->nPointX;
							Oy = pApData->nPointY;
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nCurMoveRet == 2)
						{
							Ox = pApData->sMoveCoord[Player[nPlayerIdx].m_sExtAuto.nCoordStep].x;
							Oy = pApData->sMoveCoord[Player[nPlayerIdx].m_sExtAuto.nCoordStep].y;
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nCurMoveRet == 3)
						{
							Ox = Player[nPlayerIdx].m_sExtAuto.nTempX;
							Oy = Player[nPlayerIdx].m_sExtAuto.nTempY;
						}
						nTGNpcIdx = Player[nPlayerIdx].FindTargetNpc(
						pApData->nVision, pApData->bFightBack, pApData->nFBVision,
						pApData->nSelBoss, TRUE, NULL, pApData->bMoveFollow, Ox, Oy);
						if(!nTGNpcIdx)
						{
							if(Player[nPlayerIdx].m_sExtAuto.nCurMoveRet == 2)
							{
								++Player[nPlayerIdx].m_sExtAuto.nCoordStep;
								if(Player[nPlayerIdx].m_sExtAuto.nCoordStep >= pApData->nCoordCount)
									Player[nPlayerIdx].m_sExtAuto.nCoordStep = 0;
								Player[nPlayerIdx].m_sExtAuto.bReachDes = FALSE;
							}
							Player[nPlayerIdx].m_sExtAuto.nTempX = 0;
							Player[nPlayerIdx].m_sExtAuto.nTempY = 0;
							return 0;
						}
						Player[nPlayerIdx].m_sExtAuto.uNpcID = Npc[nTGNpcIdx].m_dwID;
						bNewFound = TRUE;
					}
					if(pApData->bSkipGoldboss)
					{
						if(Npc[nTGNpcIdx].m_Type == boss_gold)
						{
							Player[nPlayerIdx].
								m_mAutoExcludeNpcID[Player[nPlayerIdx].m_sExtAuto.uNpcID]
								= uCurTime + 30000;
							Player[nPlayerIdx].m_sExtAuto.uNpcID = 0;
							return 0;
						}
					}
					if(pApData->nSelFHorse == 1)
					{
						if(Player[nPlayerIdx].m_sExtAuto.uHorseTime < uCurTime)
						{
							Player[nPlayerIdx].m_sExtAuto.uHorseTime = uCurTime + 2000;
							if(!Npc[nNpcIdx].m_bRideHorse)
							{
								OperationRequest(GOI_PLAYER_ACTION, PA_RIDE, 0);
							}
						}
					}
					else if(pApData->nSelFHorse == 2)
					{
						if(Player[nPlayerIdx].m_sExtAuto.uHorseTime < uCurTime)
						{
							Player[nPlayerIdx].m_sExtAuto.uHorseTime = uCurTime + 2000;
							if(Npc[nNpcIdx].m_bRideHorse)
							{
								OperationRequest(GOI_PLAYER_ACTION, PA_RIDE, 0);
							}
						}
					}
					Npc[nTGNpcIdx].GetMpsPos(&x, &y);
					int nDist = g_GetDistance(nX, nY, x, y);
					if(Npc[nTGNpcIdx].m_Kind != kind_player)
					{
						if(bNewFound)
						{
							int nSpeed = Npc[nNpcIdx].m_CurrentRunSpeed;
							if(nSpeed <= 0)
								nSpeed = 10;
							Player[nPlayerIdx].m_sExtAuto.uFDelayTime =
								(UINT)(nDist/(float)nSpeed)*56 + 2500;
							Player[nPlayerIdx].m_sExtAuto.uFDelayTime += uCurTime;
							Player[nPlayerIdx].m_sExtAuto.nOldLife = Npc[nTGNpcIdx].m_CurrentLife;
						}
						else if(Player[nPlayerIdx].m_sExtAuto.uFDelayTime < uCurTime)
						{
							if(Player[nPlayerIdx].m_sExtAuto.nOldLife == Npc[nTGNpcIdx].m_CurrentLife)
							{
								Player[nPlayerIdx].
									m_mAutoExcludeNpcID[Player[nPlayerIdx].m_sExtAuto.uNpcID]
									= uCurTime + 30000;
								Player[nPlayerIdx].m_sExtAuto.uNpcID = 0;
								return 0;
							}
							else
							{
								Player[nPlayerIdx].m_sExtAuto.uFDelayTime = uCurTime + 2500;
								Player[nPlayerIdx].m_sExtAuto.nOldLife = Npc[nTGNpcIdx].m_CurrentLife;
							}
						}
					}
					int nMainSkill = Player[nPlayerIdx].GetLeftSkill();
					if(pApData->nSkillIdC)
					{
						if(Player[nPlayerIdx].m_sExtAuto.uChSkillTime < uCurTime)
						{
							Player[nPlayerIdx].m_sExtAuto.uChSkillTime = uCurTime + 1000*pApData->nSkillCSec;
							Player[nPlayerIdx].m_sExtAuto.bChSkill = !Player[nPlayerIdx].m_sExtAuto.bChSkill;
						}
						if(Player[nPlayerIdx].m_sExtAuto.bChSkill)
						{
							int nIdx = Npc[nNpcIdx].m_SkillList.FindSame(pApData->nSkillIdC);
							if(nIdx && Npc[nNpcIdx].m_SkillList.m_Skills[nIdx].NextCastTime
									<= SubWorld[0].m_dwCurrentTime)
								nMainSkill = pApData->nSkillIdC;
						}
					}
					bool bChecked = false;
					if(pApData->nSkillIdB && Npc[nTGNpcIdx].m_Kind == kind_normal && Npc[nTGNpcIdx].m_Type != boss_none)
						nMainSkill = pApData->nSkillIdB;
					if(pApData->nSkillIdLS)
					{
						int nAPerc = pApData->nSLSPerc;
						if(nAPerc < 0)
							nAPerc = 0;
						else if(nAPerc > 100)
							nAPerc = 100;
						int nPPercent = (int)((double)Npc[nNpcIdx].m_CurrentLife
										*100.0/Npc[nNpcIdx].m_CurrentLifeMax);
						if(nPPercent < nAPerc)
						{
							bChecked = true;
							nMainSkill = pApData->nSkillIdLS;
						}
					}
					if(!bChecked && pApData->nSkillIdMS)
					{
						int nAPerc = pApData->nSMSPerc;
						if(nAPerc < 0)
							nAPerc = 0;
						else if(nAPerc > 100)
							nAPerc = 100;
						int nPPercent = (int)((double)Npc[nNpcIdx].m_CurrentMana
										*100.0/Npc[nNpcIdx].m_CurrentManaMax);
						if(nPPercent < nAPerc)
						{
							bChecked = true;
							nMainSkill = pApData->nSkillIdMS;
						}
					}
					if(Npc[nTGNpcIdx].m_Kind == kind_player && pApData->bFightBack)
					{
						if(pApData->nSelFBack == 1)
						{
							for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
							for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
							{
								int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
								if(nIdx > 0)
								{
									int nGenre = Item[nIdx].GetGenre();
									int nDetail = Item[nIdx].GetDetailType();
									int nPart = Item[nIdx].GetParticular();
									if(nGenre == item_townportal
									|| (nGenre == item_magicscript && nDetail == 1 && (nPart == 437 || nPart == 1083 || nPart == 1084)))
									{
										Player[nPlayerIdx].m_sExtAuto.bJustTP = TRUE;
										ItemPos	Pos;
										Pos.nPlace = pos_equiproom;
										Pos.nX = j;
										Pos.nY = i;
										Player[nPlayerIdx].ApplyUseItem(nIdx, Pos);
										return 1;
									}
								}
							}
						}
						else if(pApData->nSelFBack == 2)
							return 2;
						if(pApData->nSkillIdP)
							nMainSkill = pApData->nSkillIdP;
					}
					int nSkillIdx = Npc[nNpcIdx].m_SkillList.FindSame(nMainSkill);
					if(!nSkillIdx)
						return 0;
					KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(nMainSkill,
										Npc[nNpcIdx].m_SkillList.m_Skills[nSkillIdx].SkillLevel);
					if(!pSkill)
						return 0;
					if(pApData->nSelFHorse == 0)
					{
						if(pSkill->IsNeedDownHorse())
						{
							if(Npc[nNpcIdx].m_bRideHorse)
								OperationRequest(GOI_PLAYER_ACTION, PA_RIDE, 0);
						}
						else
						{
							if(!Npc[nNpcIdx].m_bRideHorse)
								OperationRequest(GOI_PLAYER_ACTION, PA_RIDE, 0);
						}
					}
					int nSkillRadius = pSkill->GetAttackRadius();
					if(pApData->bApproach)
					{
						int nNearDist = pApData->nNearDist;
						if(nNearDist < 75)
							nNearDist = 75;
						if(nSkillRadius > nNearDist)
							nSkillRadius = nNearDist;
					}
					g_ScenePlace.RemoveFlag();
					if(nDist < nSkillRadius)
					{
						Npc[nNpcIdx].SendCommand(do_skill, nMainSkill, -1, nTGNpcIdx);
						SendClientCmdSkill(nMainSkill, -1, Npc[nTGNpcIdx].m_dwID);
					}
					else
					{
						if (!Player[nPlayerIdx].m_RunStatus)
						{
							Npc[nNpcIdx].SendCommand(do_walk, x, y);
							SendClientCmdWalk(x, y);
						}
						else
						{
							Npc[nNpcIdx].SendCommand(do_run, x, y);
							SendClientCmdRun(x, y);
						}
					}
					return 1;
				}
				case ATYPE_RESETNPCID:
				{
					Player[nPlayerIdx].m_sExtAuto.uNpcID = 0;
					return 0;
				}
				case ATYPE_ISFIGHTMODE:
				{
					return Player[nPlayerIdx].m_sExtAuto.bPrevFightState;
				}
				case ATYPE_DRAWVISION:
				{
					KNpc::g_DrawVision = nParam;
					if(KNpc::g_DrawVision)
					{
						KNpc::g_DrawVisionSkill = KNpc::g_DrawVision;
						int nMainSkill = Player[nPlayerIdx].GetLeftSkill();
						int nSkillIdx = Npc[nNpcIdx].m_SkillList.FindSame(nMainSkill);
						if(!nSkillIdx)
							return 0;
						KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(nMainSkill,
										Npc[nNpcIdx].m_SkillList.m_Skills[nSkillIdx].SkillLevel);
						if(!pSkill)
							return 0;
						KNpc::g_DrawVisionSkill = pSkill->GetAttackRadius();
					}
					return 0;
				}
				case ATYPE_PKFIGHT:
				{
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					const autoData* pApData = (autoData*)nParam;
					int nX, nY, x, y;
					BOOL bNewFound = FALSE;
					Npc[nNpcIdx].GetMpsPos(&nX, &nY);
					int nTGNpcIdx = 0;
					if(Player[nPlayerIdx].m_sExtAuto.uNpcID)
					{
						nTGNpcIdx = NpcSet.SearchID(Player[nPlayerIdx].m_sExtAuto.uNpcID);
						if(!nTGNpcIdx || Npc[nTGNpcIdx].m_RegionIndex < 0
						|| Npc[nTGNpcIdx].m_Doing == do_death || Npc[nTGNpcIdx].m_Doing == do_revive
							|| !(NpcSet.GetRelation(nNpcIdx, nTGNpcIdx) == relation_enemy))
							Player[nPlayerIdx].m_sExtAuto.uNpcID = 0;
					}
					if(!Player[nPlayerIdx].m_sExtAuto.uNpcID)
					{
						if(pApData->nPriority)
						nTGNpcIdx = Player[nPlayerIdx].FindTargetNpc(
						pApData->nPKVision, pApData->bPKPlayer,
						pApData->nPKVision, 0, pApData->bPKNpc, &pApData->nSerPy[0]);
						else
						nTGNpcIdx = Player[nPlayerIdx].FindTargetNpc(
						pApData->nPKVision, pApData->bPKPlayer,
						pApData->nPKVision, 0, pApData->bPKNpc);
						if(!nTGNpcIdx)
							return 0;
						Player[nPlayerIdx].m_sExtAuto.uNpcID = Npc[nTGNpcIdx].m_dwID;
						bNewFound = TRUE;
					}
					if(pApData->bPKDownHorse)
					{
						if(Player[nPlayerIdx].m_sExtAuto.uHorseTime < uCurTime)
						{
							Player[nPlayerIdx].m_sExtAuto.uHorseTime = uCurTime + 2000;
							if(Npc[nNpcIdx].m_bRideHorse)
							{
								OperationRequest(GOI_PLAYER_ACTION, PA_RIDE, 0);
							}
						}
					}
					Npc[nTGNpcIdx].GetMpsPos(&x, &y);
					int nDist = g_GetDistance(nX, nY, x, y);
					int nMainSkill = Player[nPlayerIdx].GetLeftSkill();
					if(pApData->nSkillIdC)
					{
						if(Player[nPlayerIdx].m_sExtAuto.uChSkillTime < uCurTime)
						{
							Player[nPlayerIdx].m_sExtAuto.uChSkillTime = uCurTime + 1000*pApData->nSkillCSec;
							Player[nPlayerIdx].m_sExtAuto.bChSkill = !Player[nPlayerIdx].m_sExtAuto.bChSkill;
						}
						if(Player[nPlayerIdx].m_sExtAuto.bChSkill)
						{
							int nIdx = Npc[nNpcIdx].m_SkillList.FindSame(pApData->nSkillIdC);
							if(nIdx && Npc[nNpcIdx].m_SkillList.m_Skills[nIdx].NextCastTime
									<= SubWorld[0].m_dwCurrentTime)
								nMainSkill = pApData->nSkillIdC;
						}
					}
					bool bCastState = false;
					if(pApData->nSkillIdCS1)
					{
						KSkill* pStateSkill = (KSkill*)g_SkillManager.GetSkill(pApData->nSkillIdCS1, 1);
						if(pStateSkill)
						{
							int nStateId = pStateSkill->GetStateSpecailId();
							if(nStateId)
							{
								bool bCastExist = false;
								for(int s=0;s<MAX_SKILL_STATE;++s)
								{
									if(!Npc[nTGNpcIdx].m_btStateInfo[s])
										break;
									if(Npc[nTGNpcIdx].m_btStateInfo[s] == (BYTE)nStateId)
									{
										bCastExist = true;
										break;
									}
								}
								if(!bCastExist)
								{
									bCastState = true;
									nMainSkill = pApData->nSkillIdCS1;
								}
							}
						}
					}
					if(pApData->nSkillIdCS2 && !bCastState)
					{
						KSkill* pStateSkill = (KSkill*)g_SkillManager.GetSkill(pApData->nSkillIdCS2, 1);
						if(pStateSkill)
						{
							int nStateId = pStateSkill->GetStateSpecailId();
							if(nStateId)
							{
								bool bCastExist = false;
								for(int s=0;s<MAX_SKILL_STATE;++s)
								{
									if(!Npc[nTGNpcIdx].m_btStateInfo[s])
										break;
									if(Npc[nTGNpcIdx].m_btStateInfo[s] == (BYTE)nStateId)
									{
										bCastExist = true;
										break;
									}
								}
								if(!bCastExist)
								{
									bCastState = true;
									nMainSkill = pApData->nSkillIdCS2;
								}
							}
						}
					}
					if(pApData->nSkillIdCS3 && !bCastState)
					{
						KSkill* pStateSkill = (KSkill*)g_SkillManager.GetSkill(pApData->nSkillIdCS3, 1);
						if(pStateSkill)
						{
							int nStateId = pStateSkill->GetStateSpecailId();
							if(nStateId)
							{
								bool bCastExist = false;
								for(int s=0;s<MAX_SKILL_STATE;++s)
								{
									if(!Npc[nTGNpcIdx].m_btStateInfo[s])
										break;
									if(Npc[nTGNpcIdx].m_btStateInfo[s] == (BYTE)nStateId)
									{
										bCastExist = true;
										break;
									}
								}
								if(!bCastExist)
								{
									bCastState = true;
									nMainSkill = pApData->nSkillIdCS3;
								}
							}
						}
					}
					int nSkillIdx = Npc[nNpcIdx].m_SkillList.FindSame(nMainSkill);
					if(!nSkillIdx)
						return 0;
					KSkill* pSkill = (KSkill*)g_SkillManager.GetSkill(nMainSkill,
										Npc[nNpcIdx].m_SkillList.m_Skills[nSkillIdx].SkillLevel);
					if(!pSkill)
						return 0;
					g_ScenePlace.RemoveFlag();
					int nSkillRadius = pSkill->GetAttackRadius();
					if(pApData->bPKFollowTG)
					{
						if(pApData->bPKAppr && !bCastState)
						{
							int nNearDist = pApData->nPKNearDist;
							if(nNearDist < 75)
								nNearDist = 75;
							if(nSkillRadius > nNearDist)
								nSkillRadius = nNearDist;
						}
						if(nDist < nSkillRadius)
						{
							Npc[nNpcIdx].SendCommand(do_skill, nMainSkill, -1, nTGNpcIdx);
							SendClientCmdSkill(nMainSkill, -1, Npc[nTGNpcIdx].m_dwID);
						}
						else
						{
							if (!Player[nPlayerIdx].m_RunStatus)
							{
								Npc[nNpcIdx].SendCommand(do_walk, x, y);
								SendClientCmdWalk(x, y);
							}
							else
							{
								Npc[nNpcIdx].SendCommand(do_run, x, y);
								SendClientCmdRun(x, y);
							}
						}
					}
					else
					{
						if(nDist <= nSkillRadius)
						{
							Npc[nNpcIdx].SendCommand(do_skill, nMainSkill, -1, nTGNpcIdx);
							SendClientCmdSkill(nMainSkill, -1, Npc[nTGNpcIdx].m_dwID);
						}
						else
						{
							if(bCastState)
								return 0;
							int nOverDist = nDist - nSkillRadius;
							nOverDist += nSkillRadius/2;
							int nDir = g_GetDirIndex(x, y, nX, nY);
							x = x + ((nOverDist * g_DirCos(nDir, 64)) >> 10);
							y = y + ((nOverDist * g_DirSin(nDir, 64)) >> 10);
							Npc[nNpcIdx].SendCommand(do_skill, nMainSkill, x, y);
							SendClientCmdSkill(nMainSkill, x, y);
						}
					}
					return 1;
				}
				case ATYPE_PICKUPSET:
				{
					Player[nPlayerIdx].m_sExtAuto.bLBObjDown = nParam;
					for (std::map<int,ExtAutoObjTime>::iterator itt = Player[nPlayerIdx].m_mAutoIDObj.begin();
						itt != Player[nPlayerIdx].m_mAutoIDObj.end();)
					{
						ExtAutoObjTime s = itt->second;
						if(uCurTime - s.nTotalTime >= 3*60000) //3 minutes
						{
							Player[nPlayerIdx].m_mAutoIDObj.erase(itt++);
						}
						else
						{
							++itt;
						}
					}
					break;
				}
				case ATYPE_GETITEMNAME:
				{
					char* pName = (char*)nParam;
					std::map<std::string, int> mapName;
					for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
					for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
					{
						int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
						if(nIdx > 0)
						{
							std::string str = Item[nIdx].GetName();
							if(mapName.find(str) == mapName.end())
							{
								mapName[str] = 1;
								strcpy(pName, Item[nIdx].GetName());
								pName += 80;
								++nRet;
							}
						}
					}
					break;
				}
				case ATYPE_PICKUP:
				{
					const autoData* pApData = (autoData*)nParam;
					if(!pApData->bPickUp)
						return 0;
					if(!Npc[nNpcIdx].m_FightMode && !pApData->bCityPick)
						return 0;
					int nGameLoop = g_SubWorldSet.GetGameTime();
					if(Player[nPlayerIdx].m_sExtAuto.nCurObjLoop == nGameLoop)
						return 0;
					Player[nPlayerIdx].m_sExtAuto.nCurObjLoop = nGameLoop;
					int i = ObjSet.GetNext(0);
					int nX,nY,dX,dY;
					Npc[nNpcIdx].GetMpsPos(&nX, &nY);
					int nVision = 200;
					while(i)
					{
						if(Object[i].m_nID > 0 &&
						(Object[i].m_nKind == Obj_Kind_Item
						|| Object[i].m_nKind == Obj_Kind_Money))
						{
							int nExist = -1;
							for (std::map<int,ExtAutoObjTime>::iterator it = Player[nPlayerIdx].m_mAutoIDObj.begin();
							it != Player[nPlayerIdx].m_mAutoIDObj.end();++it)
							{
								ExtAutoObjTime& s = it->second;
								if((s.bItem && Object[i].m_nKind == Obj_Kind_Item
								&& Object[i].m_nItemDataID == s.nID)
								||
								(!s.bItem && Object[i].m_nKind == Obj_Kind_Money
								&& Object[i].m_nID == s.nID)
								)
								{
									nExist = it->first;
									break;
								}
							}
							if(nExist >= 0)
							{
								ExtAutoObjTime& s = Player[nPlayerIdx].m_mAutoIDObj[nExist];
								if(s.nChecked >= 3 || s.nPickTime > uCurTime)
								{
									i = ObjSet.GetNext(i);
									continue;
								}
							}
							if(pApData->bNoPick && pApData->nNOPCount)
							{
								bool bCont = false;
								for(int c=0;c < pApData->nNOPCount;++c)
								{
									if(!strcmp(Object[i].m_szName, pApData->szNOPName[c]))
									{
										bCont = true;
										break;
									}
								}
								if(bCont)
								{
									i = ObjSet.GetNext(i);
									continue;
								}
							}
							if(Object[i].m_nKind == Obj_Kind_Item)
							{
								if(pApData->nPickType == 1) //®Æc phÈm
								{
									if(Object[i].m_nGenre == item_equip)
									{
										i = ObjSet.GetNext(i);
										continue;
									}
								}
								else if(pApData->nPickType == 2) //®å mµu
								{
									if(Object[i].m_nGenre == item_equip
									&& !Object[i].m_nColorID)
									{
										i = ObjSet.GetNext(i);
										continue;
									}
								}
								else if(pApData->nPickType == 3) //1 « mµu
								{
									if(Object[i].m_nItemWidth != 1 || Object[i].m_nItemHeight != 1)
									{
										i = ObjSet.GetNext(i);
										continue;
									}
									if(Object[i].m_nGenre == item_equip
									&& !Object[i].m_nColorID)
									{
										i = ObjSet.GetNext(i);
										continue;
									}
								}
								else if(pApData->nPickType == 4) //1-4 « mµu
								{
									if(Object[i].m_nItemWidth > 2 || Object[i].m_nItemHeight > 2)
									{
										i = ObjSet.GetNext(i);
										continue;
									}
									if(Object[i].m_nGenre == item_equip
									&& !Object[i].m_nColorID)
									{
										i = ObjSet.GetNext(i);
										continue;
									}
								}
								else if(pApData->nPickType == 5) //tiÒn
								{
									i = ObjSet.GetNext(i);
									continue;
								}
							}
							Object[i].GetMpsPos(&dX,&dY);
							if(g_GetDistance(nX, nY, dX, dY) < nVision)
							{
								int x, y;
								if(Object[i].m_nKind == Obj_Kind_Money
									|| Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
										Object[i].m_nItemWidth, Object[i].m_nItemHeight, &x, &y))
								{
									if(nExist < 0)
									{
										ExtAutoObjTime s;
										s.nTotalTime = uCurTime;
										s.nChecked = 1;
										s.nPickTime = uCurTime + 120;
										if(Object[i].m_nKind == Obj_Kind_Item)
											s.nID = Object[i].m_nItemDataID;
										else
											s.nID = Object[i].m_nID;
										s.bItem = (Object[i].m_nKind == Obj_Kind_Item);
										Player[nPlayerIdx].m_mAutoIDObj[Player[nPlayerIdx].m_sExtAuto.umObjIncId++] = s;
									}
									else
									{
										ExtAutoObjTime& s = Player[nPlayerIdx].m_mAutoIDObj[nExist];
										s.nChecked++;
										s.nPickTime = uCurTime + 120;
									}
									Player[nPlayerIdx].CheckObject(i);
									nRet = 1;
									break;
								}
							}
						}
						i = ObjSet.GetNext(i);
					}
					if(pApData->bFollowPick && !pApData->bOnPK && Npc[nNpcIdx].m_FightMode
					&& !Player[nPlayerIdx].m_sExtAuto.bLBObjDown)
					{
						if(pApData->bMoveFollow)
						{
							bool bFoundFol = false;
							int nIdx = 0;
							while (nIdx = NpcSet.GetNextIdx(nIdx))
							{
								if (Npc[nIdx].m_Kind != kind_player)
									continue;
								if (nIdx == Player[nPlayerIdx].m_nIndex)
									continue;
								if (Npc[nIdx].m_RegionIndex < 0)
									continue;
								if(!strcmp(pApData->szFollName, Npc[nIdx].Name))
								{
									bFoundFol = true;
									break;
								}
							}
							if(bFoundFol)
								break;
						}
						int nFollowObj = 0;
						if(Player[nPlayerIdx].m_sExtAuto.nCurObjID)
						{
							nFollowObj = ObjSet.FindID(Player[nPlayerIdx].m_sExtAuto.nCurObjID);
							if(nFollowObj > 0)
							{
								int nExist = -1;
								for (std::map<int,ExtAutoObjTime>::iterator it = Player[nPlayerIdx].m_mAutoIDObj.begin();
								it != Player[nPlayerIdx].m_mAutoIDObj.end();++it)
								{
									ExtAutoObjTime& s = it->second;
									if((s.bItem && Object[nFollowObj].m_nKind == Obj_Kind_Item
									&& Object[nFollowObj].m_nItemDataID == s.nID)
									||
									(!s.bItem && Object[nFollowObj].m_nKind == Obj_Kind_Money
									&& Object[nFollowObj].m_nID == s.nID)
									)
									{
										nExist = it->first;
										break;
									}
								}
								if(nExist >= 0)
								{
									ExtAutoObjTime& s = Player[nPlayerIdx].m_mAutoIDObj[nExist];
									if(s.nChecked >= 3)
									{
										nFollowObj = 0;
										Player[nPlayerIdx].m_sExtAuto.nCurObjID = 0;
									}
								}
								if(nFollowObj > 0)
								{
									int x, y;
									if(!(Object[nFollowObj].m_nKind == Obj_Kind_Money
											|| Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
											Object[nFollowObj].m_nItemWidth, Object[nFollowObj].m_nItemHeight, &x, &y)))
									{
										nFollowObj = 0;
										Player[nPlayerIdx].m_sExtAuto.nCurObjID = 0;
									}
								}
							}
						}
						nVision = pApData->nPickVision;
						if(nVision < 200)
							nVision = 200;
						else if(nVision > 800)
							nVision = 800;
						if(!nFollowObj)
						{
							i = ObjSet.GetNext(0);
							while(i)
							{
								if(Object[i].m_nID > 0 &&
								(Object[i].m_nKind == Obj_Kind_Item
								|| Object[i].m_nKind == Obj_Kind_Money))
								{
									int x, y;
									if(!(Object[i].m_nKind == Obj_Kind_Money
											|| Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
											Object[i].m_nItemWidth, Object[i].m_nItemHeight, &x, &y)))
									{
										i = ObjSet.GetNext(i);
										continue;
									}
									int nExist = -1;
									for (std::map<int,ExtAutoObjTime>::iterator it = Player[nPlayerIdx].m_mAutoIDObj.begin();
									it != Player[nPlayerIdx].m_mAutoIDObj.end();++it)
									{
										ExtAutoObjTime& s = it->second;
										if((s.bItem && Object[i].m_nKind == Obj_Kind_Item
										&& Object[i].m_nItemDataID == s.nID)
										||
										(!s.bItem && Object[i].m_nKind == Obj_Kind_Money
										&& Object[i].m_nID == s.nID)
										)
										{
											nExist = it->first;
											break;
										}
									}
									if(nExist >= 0)
									{
										ExtAutoObjTime& s = Player[nPlayerIdx].m_mAutoIDObj[nExist];
										if(s.nChecked >= 3)
										{
											i = ObjSet.GetNext(i);
											continue;
										}
									}
									if(pApData->bNoPick && pApData->nNOPCount)
									{
										bool bCont = false;
										for(int c=0;c < pApData->nNOPCount;++c)
										{
											if(!strcmp(Object[i].m_szName, pApData->szNOPName[c]))
											{
												bCont = true;
												break;
											}
										}
										if(bCont)
										{
											i = ObjSet.GetNext(i);
											continue;
										}
									}
									if(Object[i].m_nKind == Obj_Kind_Item)
									{
										if(pApData->nPickType == 1) //®Æc phÈm
										{
											if(Object[i].m_nGenre == item_equip)
											{
												i = ObjSet.GetNext(i);
												continue;
											}
										}
										else if(pApData->nPickType == 2) //®å mµu
										{
											if(Object[i].m_nGenre == item_equip
											&& !Object[i].m_nColorID)
											{
												i = ObjSet.GetNext(i);
												continue;
											}
										}
										else if(pApData->nPickType == 3) //1 « mµu
										{
											if(Object[i].m_nItemWidth != 1 || Object[i].m_nItemHeight != 1)
											{
												i = ObjSet.GetNext(i);
												continue;
											}
											if(Object[i].m_nGenre == item_equip
											&& !Object[i].m_nColorID)
											{
												i = ObjSet.GetNext(i);
												continue;
											}
										}
										else if(pApData->nPickType == 4) //1-4 « mµu
										{
											if(Object[i].m_nItemWidth > 2 || Object[i].m_nItemHeight > 2)
											{
												i = ObjSet.GetNext(i);
												continue;
											}
											if(Object[i].m_nGenre == item_equip
											&& !Object[i].m_nColorID)
											{
												i = ObjSet.GetNext(i);
												continue;
											}
										}
										else if(pApData->nPickType == 5) //tiÒn
										{
											i = ObjSet.GetNext(i);
											continue;
										}
									}
									Object[i].GetMpsPos(&dX,&dY);
									if(g_GetDistance(nX, nY, dX, dY) < nVision)
									{
										if(Player[nPlayerIdx].m_sExtAuto.nCurMoveRet > 0)
										{
											int Ox = 0,Oy = 0;
											if(Player[nPlayerIdx].m_sExtAuto.nCurMoveRet == 1)
											{
												Ox = pApData->nPointX;
												Oy = pApData->nPointY;
											}
											else if(Player[nPlayerIdx].m_sExtAuto.nCurMoveRet == 2)
											{
												Ox = pApData->sMoveCoord[Player[nPlayerIdx].m_sExtAuto.nCoordStep].x;
												Oy = pApData->sMoveCoord[Player[nPlayerIdx].m_sExtAuto.nCoordStep].y;
											}
											else if(Player[nPlayerIdx].m_sExtAuto.nCurMoveRet == 3)
											{
												Ox = Player[nPlayerIdx].m_sExtAuto.nTempX;
												Oy = Player[nPlayerIdx].m_sExtAuto.nTempY;
											}
											int nFVision = pApData->nVision;
											if(nFVision < 100)
												nFVision = 100;
											else if(nFVision > 1200)
												nFVision = 1200;
											int nDist = g_GetDistance(Ox, Oy, dX, dY);
											if(nDist < nFVision)
											{
												nFollowObj = i;
												Player[nPlayerIdx].m_sExtAuto.nCurObjID = Object[i].m_nID;
												break;
											}
										}
										else
										{
											nFollowObj = i;
											Player[nPlayerIdx].m_sExtAuto.nCurObjID = Object[i].m_nID;
											break;
										}
									}
								}
								i = ObjSet.GetNext(i);
							}
						}
						if(nFollowObj)
						{
							g_ScenePlace.RemoveFlag();
							Object[nFollowObj].GetMpsPos(&dX,&dY);
							if (!Player[nPlayerIdx].m_RunStatus)
							{
								Npc[nNpcIdx].SendCommand(do_walk, dX, dY);
								SendClientCmdWalk(dX, dY);
							}
							else
							{
								Npc[nNpcIdx].SendCommand(do_run, dX, dY);
								SendClientCmdRun(dX, dY);
							}
							nRet = 2;
						}
					}
					break;
				}
				case ATYPE_FILTER:
				{
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					if(Player[nPlayerIdx].m_sExtAuto.uFtNextTime >= uCurTime)
						return 0;
					Player[nPlayerIdx].m_sExtAuto.uFtNextTime = uCurTime + 300;
					const autoData* pApData = (autoData*)nParam;
					if(!pApData->bFilter && !pApData->bPrize && !pApData->bLevel && !pApData->bSaveRing)
						return 0;
					int nIdx = Player[nPlayerIdx].m_ItemList.Hand();
					if(nIdx)
					{
						bool bThrow = true;
						if(Item[nIdx].GetGenre() != item_equip)
							bThrow = false;
						if(bThrow && Item[nIdx].GetColorItem() > green_item)
							bThrow = false;
						if(bThrow && Item[nIdx].GetDetailType() >= equip_horse)
							bThrow = false;
						if(bThrow && (Item[nIdx].GetPlayerItemLock() > 0 
							|| Item[nIdx].GetPlayerItemHLock() > 0 
							|| Item[nIdx].GetPlayerItemLock() == -2))
							bThrow = false;
						if(pApData->bPrize && bThrow)
						{
							int nPrize = Item[nIdx].GetSalePrice();
							if(nPrize > pApData->nPrize)
								bThrow = false;
						}
						if(pApData->bLevel && bThrow)
						{
							if(Item[nIdx].GetLevel() > pApData->nLevel)
								bThrow = false;
						}
						if(pApData->bSaveRing && bThrow)
						{
							if((Item[nIdx].GetDetailType() == equip_ring
							|| Item[nIdx].GetDetailType() == equip_amulet
							|| Item[nIdx].GetDetailType() == equip_pendant)
							&& Item[nIdx].GetLevel() > pApData->nSRLevel)
								bThrow = false;
						}
						if(pApData->bFilter && pApData->nFtMaCount && bThrow)
						{
							for(int i=0;i<pApData->nFtMaCount;++i)
							{
								if(!bThrow)
									break;
								for(int m=0;m<6;++m)
								{
									if(Item[nIdx].m_aryMagicAttrib[m].nAttribType == 139)
									{
										bThrow = false;
										break;
									}
									if(Item[nIdx].m_aryMagicAttrib[m].nAttribType == 0)
										break;
									if(pApData->nFtMagic[i][0] == Item[nIdx].m_aryMagicAttrib[m].nAttribType)
									{
										if(pApData->nFtMagic[i][0] == magic_indestructible_b
										|| Item[nIdx].m_aryMagicAttrib[m].nValue[0] >= pApData->nFtMagic[i][1])
										{
											bThrow = false;
											break;
										}
									}
								}
							}
						}
						if(bThrow)
						{
							int nExist = -1;
							UINT uID = Item[nIdx].GetID();
							for (std::map<int,ExtAutoObjTime>::iterator it = Player[nPlayerIdx].m_mAutoIDObj.begin();
							it != Player[nPlayerIdx].m_mAutoIDObj.end();++it)
							{
								ExtAutoObjTime& s = it->second;
								if(s.bItem && uID == s.nID)
								{
									nExist = it->first;
									break;
								}
							}
							if(nExist < 0)
							{
								ExtAutoObjTime s;
								s.nTotalTime = uCurTime;
								s.nChecked = 3;
								s.nPickTime = uCurTime + 120;
								s.nID = uID;
								s.bItem = 1;
								Player[nPlayerIdx].m_mAutoIDObj[Player[nPlayerIdx].m_sExtAuto.umObjIncId++] = s;
							}
							else
							{
								ExtAutoObjTime& s = Player[nPlayerIdx].m_mAutoIDObj[nExist];
								s.nTotalTime = uCurTime;
								s.nChecked = 3;
							}
							Player[nPlayerIdx].ThrowAwayItem();
							return 1;
						}
						int x, y;
						if(Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
										Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y))
						{
							ItemPos	P1, P2;
							P1.nPlace = P2.nPlace = pos_equiproom;
							P1.nX = P2.nX = x;
							P1.nY = P2.nY = y;
							Player[nPlayerIdx].MoveItem(P1, P2);
							return 1;
						}
					}
					else
					{
						for(int h=0;h<EQUIPMENT_ROOM_HEIGHT;++h)
						for(int w=0;w<EQUIPMENT_ROOM_WIDTH;++w)
						{
							nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(w, h);
							if(nIdx > 0)
							{
								if(Item[nIdx].GetGenre() != item_equip)
									continue;
								if(Item[nIdx].GetColorItem() > green_item)
									continue;
								if(Item[nIdx].GetDetailType() >= equip_horse)
									continue;
								if(Item[nIdx].GetPlayerItemLock() > 0 
									|| Item[nIdx].GetPlayerItemHLock() > 0 
									|| Item[nIdx].GetPlayerItemLock() == -2)
									continue;
								if(pApData->bPrize)
								{
									int nPrize = Item[nIdx].GetSalePrice();
									if(nPrize > pApData->nPrize)
										continue;
								}
								if(pApData->bLevel)
								{
									if(Item[nIdx].GetLevel() > pApData->nLevel)
										continue;
								}
								if(pApData->bSaveRing)
								{
									if((Item[nIdx].GetDetailType() == equip_ring
									|| Item[nIdx].GetDetailType() == equip_amulet
									|| Item[nIdx].GetDetailType() == equip_pendant)
									&& Item[nIdx].GetLevel() > pApData->nSRLevel)
										continue;
								}
								bool bPick = true;
								if(pApData->bFilter && pApData->nFtMaCount)
								{
									for(int i=0;i<pApData->nFtMaCount;++i)
									{
										if(!bPick)
											break;
										for(int m=0;m<6;++m)
										{
											if(Item[nIdx].m_aryMagicAttrib[m].nAttribType == 139)
											{
												bPick = false;
												break;
											}
											if(Item[nIdx].m_aryMagicAttrib[m].nAttribType == 0)
												break;
											if(pApData->nFtMagic[i][0] == Item[nIdx].m_aryMagicAttrib[m].nAttribType)
											{
												if(pApData->nFtMagic[i][0] == magic_indestructible_b
												|| Item[nIdx].m_aryMagicAttrib[m].nValue[0] >= pApData->nFtMagic[i][1])
												{
													bPick = false;
													break;
												}
											}
										}
									}
								}
								if(bPick && pApData->bSaveRing && pApData->bSellItem
								&& !pApData->bFilter && !pApData->bPrize && !pApData->bLevel)
									bPick = false;
								if(bPick)
								{
									ItemPos	P1, P2;
									P1.nPlace = P2.nPlace = pos_equiproom;
									P1.nX = P2.nX = w;
									P1.nY = P2.nY = h;
									Player[nPlayerIdx].MoveItem(P1, P2);
									return 1;
								}
							}
						}
					}
					break;
				}
				case ATYPE_ARRANGEITEM:
				{
					if(!Player[nPlayerIdx].m_sExtAuto.bPrevFightState)
						return 0;
					if(Player[nPlayerIdx].m_sExtAuto.uARTimeItem > uCurTime)
						return 0;
					Player[nPlayerIdx].m_sExtAuto.uARTimeItem = uCurTime + 20000;
					DYNAMIC_COMMAND sCmd;
					sCmd.ProtocolType = c2s_dynamic_structure;
					sCmd.nBranch = c2sdnmbr_arrangeitem;
					sCmd.m_wLength = sizeof(DYNAMIC_COMMAND) - 1;
					if(g_pClient)
					g_pClient->SendPackToServer(&sCmd, sCmd.m_wLength + 1);
					return 1;
				}
				case ATYPE_ARRANGEBOX:
				{
					if(Player[nPlayerIdx].m_sExtAuto.bPrevFightState)
						return 0;
					if(Player[nPlayerIdx].m_sExtAuto.uARTimeBox > uCurTime)
						return 0;
					Player[nPlayerIdx].m_sExtAuto.uARTimeBox = uCurTime + 60000;
					DYNAMIC_COMMAND sCmd;
					sCmd.ProtocolType = c2s_dynamic_structure;
					sCmd.nBranch = c2sdnmbr_arrangebox;
					sCmd.m_wLength = sizeof(DYNAMIC_COMMAND) - 1;
					if(g_pClient)
					g_pClient->SendPackToServer(&sCmd, sCmd.m_wLength + 1);
					return 1;
				}
				case ATYPE_GETAROUNDNAME:
				{
					char* pName = (char*)nParam;
					int nIdx = 0;
					while (nIdx = NpcSet.GetNextIdx(nIdx))
					{
						if (Npc[nIdx].m_Kind != kind_player)
							continue;
						if (nIdx == Player[nPlayerIdx].m_nIndex)
							continue;
						if (Npc[nIdx].m_RegionIndex < 0)
							continue;
						strcpy(pName, Npc[nIdx].Name);
						pName += 32;
						nRet++;
						if(nRet >= 100)
							break;
					}
					break;
				}
				case ATYPE_PTPROC:
				{
					if(Player[nPlayerIdx].m_sExtAuto.uTNextProc > uCurTime)
						return 0;
					Player[nPlayerIdx].m_sExtAuto.uTNextProc = uCurTime + 400;
					for (std::map<UINT,UINT>::iterator it = Player[nPlayerIdx].m_mAutoIDTeam.begin();
						it != Player[nPlayerIdx].m_mAutoIDTeam.end();)
					{
						UINT uTime = it->second;
						if(uCurTime - uTime >= 15*1000) //15s
						{
							Player[nPlayerIdx].m_mAutoIDTeam.erase(it++);
						}
						else
						{
							++it;
						}
					}
					for (std::map<int, ExtAutoTeamRecv>::iterator itt = Player[nPlayerIdx].m_mAutoTeamRecv.begin();
						itt != Player[nPlayerIdx].m_mAutoTeamRecv.end();)
					{
						ExtAutoTeamRecv& s = itt->second;
						if(uCurTime - s.uTime >= 1500) //1.5s
						{
							Player[nPlayerIdx].m_mAutoTeamRecv.erase(itt++);
						}
						else
						{
							++itt;
						}
					}
					if(!Player[nPlayerIdx].m_vAutoTeamKick.empty())
					{
						UINT uNpcID = Player[nPlayerIdx].m_vAutoTeamKick.back();
						Player[nPlayerIdx].m_vAutoTeamKick.pop_back();
						Player[nPlayerIdx].TeamKickMember(uNpcID);
						return 1;
					}
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					const autoData* pApData = (autoData*)nParam;
					int nLeavePtMem = pApData->nLeavePtMem;
					UINT nLeavePtMin = pApData->nLeavePtMin;
					UINT nRemovePtMin = pApData->nRemovePtMin;
					if(nLeavePtMem < 2)
						nLeavePtMem = 2;
					else if(nLeavePtMem > 8)
						nLeavePtMem = 8;
					if(nLeavePtMin < 1)
						nLeavePtMin = 1;
					if(nRemovePtMin < 1)
						nRemovePtMin = 1;
					if(Player[nPlayerIdx].m_cTeam.m_nFlag)
					{
						if(!Player[nPlayerIdx].m_sExtAuto.uTNextLeave)
							Player[nPlayerIdx].m_sExtAuto.uTNextLeave = uCurTime + nLeavePtMin*60000;
						if(!Player[nPlayerIdx].m_sExtAuto.uTNextRemove)
							Player[nPlayerIdx].m_sExtAuto.uTNextRemove = uCurTime + nRemovePtMin*60000;
					}
					if(pApData->bLeavePt)
					{
						if(Player[nPlayerIdx].m_sExtAuto.uTNextLeave < uCurTime)
						{
							Player[nPlayerIdx].m_sExtAuto.uTNextLeave = uCurTime + nLeavePtMin*60000;
							if(!Player[nPlayerIdx].m_cTeam.m_nFlag)
								return 0;
							int nCount = 0;
							int nIdx = NpcSet.SearchID(g_Team[0].m_nCaptain);
							if(nIdx > 0 && Npc[nIdx].m_RegionIndex >= 0)
								++nCount;
							for (int i = 0; i < MAX_TEAM_MEMBER; ++i)
							{
								if(g_Team[0].m_nMember[i] > 0)
								{
									nIdx = NpcSet.SearchID(g_Team[0].m_nMember[i]);
									if(nIdx > 0 && Npc[nIdx].m_RegionIndex >= 0)
									{
										++nCount;
									}
								}
							}
							if(nCount < nLeavePtMem)
							{
								Player[nPlayerIdx].LeaveTeam();
								return 1;
							}
						}
					}
					if(pApData->bRemovePt)
					{
						if(Player[nPlayerIdx].m_sExtAuto.uTNextRemove < uCurTime)
						{
							Player[nPlayerIdx].m_sExtAuto.uTNextRemove = uCurTime + nRemovePtMin*60000;
							if(!Player[nPlayerIdx].m_cTeam.m_nFlag
							|| (int)Npc[nNpcIdx].m_dwID != g_Team[0].m_nCaptain)
								return 0;
							for (int i = 0; i < MAX_TEAM_MEMBER; ++i)
							{
								if(g_Team[0].m_nMember[i] > 0)
								{
									int nIdx = NpcSet.SearchID(g_Team[0].m_nMember[i]);
									if(!nIdx || Npc[nIdx].m_RegionIndex < 0)
									{
										Player[nPlayerIdx].m_vAutoTeamKick.push_back(g_Team[0].m_nMember[i]);
									}
								}
							}
						}
					}
					break;
				}
				case ATYPE_PTINVITE:
				{
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					if(Player[nPlayerIdx].m_sExtAuto.uTNextInvite > uCurTime)
						return 0;
					Player[nPlayerIdx].m_sExtAuto.uTNextInvite = uCurTime + 600;
					const autoData* pApData = (autoData*)nParam;
					if(Player[nPlayerIdx].m_cTeam.m_nFlag)
					{
						if((int)Npc[nNpcIdx].m_dwID != g_Team[0].m_nCaptain)
							return 0;
						bool bFull = true;
						for (int i = 0; i < MAX_TEAM_MEMBER; ++i)
						{
							if(g_Team[0].m_nMember[i] <= 0)
							{
								bFull = false;
								break;
							}
						}
						if(bFull)
							return 0;
					}
					UINT nLeavePtMin = pApData->nLeavePtMin;
					UINT nRemovePtMin = pApData->nRemovePtMin;
					if(nLeavePtMin < 1)
						nLeavePtMin = 1;
					if(nRemovePtMin < 1)
						nRemovePtMin = 1;
					int nIdx = 0;
					while (nIdx = NpcSet.GetNextIdx(nIdx))
					{
						if (Npc[nIdx].m_Kind != kind_player)
							continue;
						if (nIdx == Player[nPlayerIdx].m_nIndex)
							continue;
						if (Npc[nIdx].m_RegionIndex < 0)
							continue;
						if(Player[nPlayerIdx].m_mAutoIDTeam.find(Npc[nIdx].m_dwID)
							!= Player[nPlayerIdx].m_mAutoIDTeam.end())
							continue;
						if(pApData->bJoinPtByList && pApData->nIJPtCount)
						{	//chØ mêi cã tªn trong list
							bool bNamefound = false;
							for(int i=0;i<pApData->nIJPtCount;++i)
							{
								if(!strcmp(Npc[nIdx].Name, pApData->szIJPtName[i]))
								{
									bNamefound = true;
									break;
								}
							}
							if(!bNamefound)
								continue;
						}
						if(!Player[nPlayerIdx].m_cTeam.m_nFlag)
						{
							Player[nPlayerIdx].m_sExtAuto.uTNextLeave = uCurTime + nLeavePtMin*60000;
							Player[nPlayerIdx].m_sExtAuto.uTNextRemove = uCurTime + nRemovePtMin*60000;
							Player[nPlayerIdx].ApplyCreateTeam();
						}
						else
						{
							bool bFoundInTeam = false;
							for (int i = 0; i < MAX_TEAM_MEMBER; ++i)
							{
								if((int)Npc[nIdx].m_dwID == g_Team[0].m_nMember[i])
								{
									bFoundInTeam = true;
									break;
								}
							}
							if(bFoundInTeam)
								continue;
						}
						Player[nPlayerIdx].TeamInviteAdd(Npc[nIdx].m_dwID);
						Player[nPlayerIdx].m_mAutoIDTeam[Npc[nIdx].m_dwID] = uCurTime;
						nRet = 1;
						break;
					}
					break;
				}
				case ATYPE_PTJOIN:
				{
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					if(Player[nPlayerIdx].m_sExtAuto.uTNextJoin > uCurTime)
						return 0;
					Player[nPlayerIdx].m_sExtAuto.uTNextJoin = uCurTime + 500;
					const autoData* pApData = (autoData*)nParam;
					if(Player[nPlayerIdx].m_cTeam.m_nFlag)
						return 0;
					UINT nLeavePtMin = pApData->nLeavePtMin;
					if(nLeavePtMin < 1)
						nLeavePtMin = 1;
					for (std::map<int, ExtAutoTeamRecv>::iterator it
							= Player[nPlayerIdx].m_mAutoTeamRecv.begin();
						it != Player[nPlayerIdx].m_mAutoTeamRecv.end();)
					{	//t×m trong list ®· mêi
						ExtAutoTeamRecv& s = it->second;
						int nIdx = 0;
						while (nIdx = NpcSet.GetNextIdx(nIdx))
						{
							if (Npc[nIdx].m_Kind != kind_player)
								continue;
							if (nIdx == Player[nPlayerIdx].m_nIndex)
								continue;
							if (Npc[nIdx].m_RegionIndex < 0)
								continue;
							if(pApData->bJoinPtByList && pApData->nIJPtCount)
							{	//nÕu nhËn theo list th× tra cã tªn
								bool bNamefound = false;
								for(int i=0;i<pApData->nIJPtCount;++i)
								{
									if(!strcmp(Npc[nIdx].Name, pApData->szIJPtName[i]))
									{
										bNamefound = true;
										break;
									}
								}
								if(!bNamefound)
									continue;
							}
							if(!strcmp(Npc[nIdx].Name, s.szName))
								break;
						}
						if(nIdx)
						{
							Player[nPlayerIdx].m_sExtAuto.uTNextLeave = uCurTime + nLeavePtMin*60000;
							Player[nPlayerIdx].m_cTeam.ReplyInvite(it->first, 1);
							Player[nPlayerIdx].m_mAutoTeamRecv.erase(it++);
							return 1;
						}
						else
						{
							Player[nPlayerIdx].m_mAutoTeamRecv.erase(it++);
						}
					}
					break;
				}
				case ATYPE_REPAIRF:
				{
					if(!Npc[nNpcIdx].m_FightMode)
						return 0;
					if(Player[nPlayerIdx].m_sExtAuto.uTNextRepair > uCurTime)
						return 0;
					Player[nPlayerIdx].m_sExtAuto.uTNextRepair = uCurTime + 1800;
					int nRepairIdx = 0;
					for (int k = 0; k < itempart_horse; ++k)
					{
						int nIdx = Player[nPlayerIdx].m_ItemList.GetEquipment(k);
						if(nIdx > 0)
						{
							int nDur = Item[nIdx].GetDurability();
							int nMaxDur = Item[nIdx].GetMaxDurability();
							if(nDur > 0 && nMaxDur > 0 && nDur < nMaxDur)
							{
								nRepairIdx = nIdx;
								break;
							}
						}
					}
					if(nRepairIdx)
					{
						int nMoney = Player[nPlayerIdx].m_ItemList.GetMoney(room_equipment);
						int nRepair = Item[nRepairIdx].GetRepairPrice();
						if(nMoney >= nRepair)
						{
							SendClientCmdRepair(Item[nRepairIdx].GetID());
							return 1;
						}
					}
					break;
				}
				case ATYPE_RETURN:
				{
					if(Player[nPlayerIdx].CheckTrading() || Npc[Player[nPlayerIdx].m_nIndex].m_BaiTan)
						return 0;
					if(Player[nPlayerIdx].m_sExtAuto.uTNextReturn > uCurTime)
						return 0;
					//g_DebugLog("homest [%d]",Player[nPlayerIdx].m_sExtAuto.nHomeStep);
					Player[nPlayerIdx].m_sExtAuto.uTNextReturn = uCurTime + 300;
					if(Player[nPlayerIdx].m_sExtAuto.uHorseTime < uCurTime)
					{
						Player[nPlayerIdx].m_sExtAuto.uHorseTime = uCurTime + 2000;
						if(!Npc[nNpcIdx].m_bRideHorse)
						{
							OperationRequest(GOI_PLAYER_ACTION, PA_RIDE, 0);
						}
					}
					const autoData* pApData = (autoData*)nParam;
					if(Player[nPlayerIdx].m_sExtAuto.nHomeStep == 0)
					{	//cÊt hoÆc qu¨ng mãn trªn tay nÕu cã
						g_ScenePlace.RemoveFlag();
						++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
						int nIdx = Player[nPlayerIdx].m_ItemList.Hand();
						if(nIdx > 0)
						{
							int x, y;
							if(Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
											Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y))
							{
								ItemPos	P1, P2;
								P1.nPlace = P2.nPlace = pos_equiproom;
								P1.nX = P2.nX = x;
								P1.nY = P2.nY = y;
								Player[nPlayerIdx].MoveItem(P1, P2);
							}
							else
								Player[nPlayerIdx].ThrowAwayItem();
							return 1;
						}
					}
					else if(Player[nPlayerIdx].m_sExtAuto.nHomeStep == 1)
					{	//b¸n r¸c
						if(pApData->bSellItem)
						{
							int nSelIdx = 0;
							for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
							{
								if(nSelIdx)
									break;
								for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
								{
									int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
									if(nIdx > 0)
									{
										if(Item[nIdx].GetGenre() != item_equip)
											continue;
										if(Item[nIdx].GetColorItem() > green_item)
											continue;
										if(Item[nIdx].GetPlayerItemLock() > 0 
											|| Item[nIdx].GetPlayerItemHLock() > 0 
											|| Item[nIdx].GetPlayerItemLock() == -2)
											continue;
										// (20/08) item dat yeu cau nhiem vu Da Tau dang lam: CAM ban
										if(DT_IsQuestItem(nPlayerIdx, pApData, nIdx))
											continue;
										int nDetail = Item[nIdx].GetDetailType();
										if(!pApData->bSellHorse)
										{
											if(nDetail >= equip_horse)
												continue;
											nSelIdx = nIdx;
										}
										else if(nDetail == equip_horse || nDetail == equip_mask)
										{
											nSelIdx = nIdx;
											break;
										}
										else if(nDetail < equip_horse)
										{
											nSelIdx = nIdx;
										}
										if(nSelIdx)
										{
											if(pApData->bSaveRing)
											{
												if((nDetail == equip_ring
												|| nDetail == equip_amulet
												|| nDetail == equip_pendant)
												&& Item[nSelIdx].GetLevel() > pApData->nSRLevel)
												{
													nSelIdx = 0;
													continue;
												}
											}
											if(!pApData->nSelSell && pApData->nFtMaCount)
											{
												bool bSave = false;
												for(int k=0;k<pApData->nFtMaCount;++k)
												{
													if(bSave)
														break;
													for(int m=0;m<6;++m)
													{
														if(Item[nSelIdx].m_aryMagicAttrib[m].nAttribType == 139)
														{
															bSave = true;
															break;
														}
														if(Item[nSelIdx].m_aryMagicAttrib[m].nAttribType == 0)
															break;
														if(pApData->nFtMagic[k][0] == Item[nSelIdx].m_aryMagicAttrib[m].nAttribType)
														{
															if(pApData->nFtMagic[k][0] == magic_indestructible_b
															|| Item[nSelIdx].m_aryMagicAttrib[m].nValue[0] >= pApData->nFtMagic[k][1])
															{
																g_DebugLog("ma[%d][%d]", pApData->nFtMagic[k][0],pApData->nFtMagic[k][1]);
																bSave = true;
																break;
															}
														}
													}
												}
												if(bSave)
												{
													nSelIdx = 0;
													continue;
												}
											}
										}
									}
								}
							}
							if(nSelIdx)
							{
								SendClientCmdSell(Item[nSelIdx].GetID());
								return 1;
							}
							else
								++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
						}
						else
							++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
					}
					else if(Player[nPlayerIdx].m_sExtAuto.nHomeStep == 2)
					{	//mËt khÈu
						if(!Player[nPlayerIdx].m_CUnlocked)
						{
							// (r4 - PB R5) khong co mat khau thi dung spam mo khoa 300ms/lan
							// va dung im - bo qua cac buoc can ruong, di tiep
							if(!pApData->szBoxPass[0])
							{
								++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
								return 1;
							}
							SendClientCPUnlockCmd(atoi(pApData->szBoxPass));
							return 1;
						}
						++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
					}
					else if(Player[nPlayerIdx].m_sExtAuto.nHomeStep == 3)
					{	//rót tiÒn
						if(pApData->bWithdraw && pApData->nWDMoney)
						{
							int nWDMoney = pApData->nWDMoney*10000;
							int nMoney = Player[nPlayerIdx].m_ItemList.GetMoney(room_repository);
							if(nMoney < nWDMoney)
								nWDMoney = nMoney;
							OperationRequest(GOI_MONEY_INOUT_STORE_BOX, false, nWDMoney);
							++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
							return 1;
						}
						else
							++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
					}
					else if(Player[nPlayerIdx].m_sExtAuto.nHomeStep == 4)
					{	//söa ®å trong thµnh
						int nRepairIdx = 0;
						if(pApData->bRepair)
						{
							for (int k = 0; k < itempart_horse; ++k)
							{
								int nIdx = Player[nPlayerIdx].m_ItemList.GetEquipment(k);
								if(nIdx > 0)
								{
									int nDur = Item[nIdx].GetDurability();
									int nMaxDur = Item[nIdx].GetMaxDurability();
									if(nDur > 0 && nMaxDur > 0 && nDur < nMaxDur)
									{
										nRepairIdx = nIdx;
										break;
									}
								}
							}
						}
						if(nRepairIdx)
						{
							int nMoney = Player[nPlayerIdx].m_ItemList.GetMoney(room_equipment);
							int nRepair = Item[nRepairIdx].GetRepairPrice();
							if(nMoney < nRepair)
							{
								++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
								return 0;
							}
							SendClientCmdRepair(Item[nRepairIdx].GetID());
							return 1;
						}
						else
							++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
					}
					else if(Player[nPlayerIdx].m_sExtAuto.nHomeStep == 5)
					{	//cÊt ®å
						if(pApData->bSaveItem && Player[nPlayerIdx].m_CUnlocked)
						{
							int nSaveIdx = 0, nDstPos = 0;
							for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
							{
								if(nSaveIdx)
									break;
								for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
								{
									int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
									if(nIdx > 0)
									{
										if(Item[nIdx].GetGenre() != item_equip)
											continue;
										// (21/08 - PB F3) item dat yeu cau nhiem vu Da Tau dang lam: de trong tui
										// (cat vao ruong la may DT khong thay / khong lay ra duoc khi tat o lay-tu-ruong)
										if(DT_IsQuestItem(nPlayerIdx, pApData, nIdx))
											continue;
										int x, y;
										if(pApData->nSelStore == 0)
										{
											if(Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
												Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y, room_repository))
											{
												nDstPos = pos_repositoryroom;
												nSaveIdx = nIdx;
												break;
											}
											else if(Npc[Player[nPlayerIdx].m_nIndex].m_ExBoxId >= 1 && Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
												Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y, room_exbox1))
											{
												nDstPos = pos_exbox1room;
												nSaveIdx = nIdx;
												break;
											}
										}
										else if(pApData->nSelStore == 1)
										{
											if(Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
												Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y, room_repository))
											{
												nDstPos = pos_repositoryroom;
												nSaveIdx = nIdx;
												break;
											}
											else if(Npc[Player[nPlayerIdx].m_nIndex].m_ExBoxId >= 1 && Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
												Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y, room_exbox1))
											{
												nDstPos = pos_exbox1room;
												nSaveIdx = nIdx;
												break;
											}
											else if(Npc[Player[nPlayerIdx].m_nIndex].m_ExBoxId >= 2 && Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
												Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y, room_exbox2))
											{
												nDstPos = pos_exbox2room;
												nSaveIdx = nIdx;
												break;
											}
										}
										else
										{
											if(Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
												Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y, room_repository))
											{
												nDstPos = pos_repositoryroom;
												nSaveIdx = nIdx;
												break;
											}
											else if(Npc[Player[nPlayerIdx].m_nIndex].m_ExBoxId >= 1 && Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
												Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y, room_exbox1))
											{
												nDstPos = pos_exbox1room;
												nSaveIdx = nIdx;
												break;
											}
											else if(Npc[Player[nPlayerIdx].m_nIndex].m_ExBoxId >= 2 && Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
												Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y, room_exbox2))
											{
												nDstPos = pos_exbox2room;
												nSaveIdx = nIdx;
												break;
											}
											else if(Npc[Player[nPlayerIdx].m_nIndex].m_ExBoxId >= 3 && Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(
												Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &x, &y, room_exbox3))
											{
												nDstPos = pos_exbox3room;
												nSaveIdx = nIdx;
												break;
											}
										}
									}
								}
							}
							if(nSaveIdx)
							{
								unsigned int uSrcPr[2];
								uSrcPr[0] = nSaveIdx;
								uSrcPr[1] = pos_equiproom;
								OperationRequest(GOI_EXCHANGEITEM,
								(unsigned int)&uSrcPr, nDstPos);
								return 1;
							}
							else
								++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
						}
						else
							++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
					}
					else if(Player[nPlayerIdx].m_sExtAuto.nHomeStep == 6)
					{	//mua thuèc
						MapStation::iterator it = g_MedicineStation.find(SubWorld[0].m_SubWorldID);
						if(it == g_MedicineStation.end())
						{
							Player[nPlayerIdx].m_sExtAuto.nHomeStep = 100;
							return 0;
						}
						if(!pApData->bBuyLife && !pApData->bBuyMana && !pApData->bBuyPois)
						{
							Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
							++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
							return 0;
						}
						int nX,nY;
						Npc[nNpcIdx].GetMpsPos(&nX, &nY);
						StationVector& v = ( *it ).second;
						if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 0)
						{	//t×m shop
							int i,j;
							int nLNum = 0, nMNum = 0, nPNum = 0;
							for( i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
							for( j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
							{
								int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
								if(nIdx > 0)
								{
									if(Item[nIdx].GetGenre() == item_medicine
									&& Item[nIdx].GetDetailType() == g_LifeBottle[pApData->nBuyLifeSel].nDetail
									&& Item[nIdx].GetLevel() == g_LifeBottle[pApData->nBuyLifeSel].nLevel)
									{
										++nLNum;
									}
									else if(Item[nIdx].GetGenre() == item_medicine
									&& Item[nIdx].GetDetailType() == g_ManaBottle[pApData->nBuyManaSel].nDetail
									&& Item[nIdx].GetLevel() == g_ManaBottle[pApData->nBuyManaSel].nLevel)
									{
										++nMNum;
									}
									else if(Item[nIdx].GetGenre() == item_medicine
									&& Item[nIdx].GetDetailType() == g_PoisonBottle[pApData->nBuyPoisSel].nDetail
									&& Item[nIdx].GetLevel() == g_PoisonBottle[pApData->nBuyPoisSel].nLevel)
									{
										++nPNum;
									}
								}
							}
							if((!pApData->bBuyLife || nLNum >= pApData->nBLNum)
							&& (!pApData->bBuyMana || nMNum >= pApData->nBMNum)
							&& (!pApData->bBuyPois || nPNum >= pApData->nBPNum))
							{
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
								++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
								return 0;
							}
							int nDist = -1, nPos = 0;
							for( i=0;i < (int)v.size();++i)
							{
								int nCurDist = g_GetDistance(nX, nY, v[i].x, v[i].y);
								if(nDist < 0)
									nDist = nCurDist;
								if(nCurDist < nDist)
								{
									nPos = i;
									nDist = nCurDist;
								}
							}
							if(SubWorld[0].FindPath(v[nPos].x, v[nPos].y) > 0)
							{
								Player[nPlayerIdx].m_sExtAuto.nCurShop = nPos;
								Player[nPlayerIdx].m_sExtAuto.uSyncTime = 0;
								Player[nPlayerIdx].m_sExtAuto.nSubStep += 2;
							}
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 1)
						{	//trªn ®­êng quay l¹i trung t©m
							int x,y;
							if(!SubWorld[0].HaveTarget(x, y))
							{
								sStation& c = g_CenterStation[SubWorld[0].m_SubWorldID];
								SubWorld[0].FindPath(c.x, c.y);
							}
							else
							{
								sStation& c = g_CenterStation[SubWorld[0].m_SubWorldID];
								if(c.x != x || c.y != y)
								{
									g_ScenePlace.RemoveFlag();
									Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
									return 0;
								}
							}
							if(Player[nPlayerIdx].m_sExtAuto.uSyncTime < uCurTime)
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
							return 0;
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 2)
						{	//trªn ®­êng ®Õn shop
							int x,y;
							if(!SubWorld[0].HaveTarget(x, y))
							{
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
								return 0;
							}
							else
							{
								if(v[Player[nPlayerIdx].m_sExtAuto.nCurShop].x != x
								|| v[Player[nPlayerIdx].m_sExtAuto.nCurShop].y != y)
								{
									g_ScenePlace.RemoveFlag();
									Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
									return 0;
								}
							}
							int nPos = Player[nPlayerIdx].m_sExtAuto.nCurShop;
							int nDist = g_GetDistance(nX, nY, v[nPos].x, v[nPos].y);
							if(nDist < 300)
							{
								if(CoreDataChanged(GDCNI_UI_ACT, 0, 0))
								{
									++Player[nPlayerIdx].m_sExtAuto.nSubStep;
									return 0;
								}
								if(!Player[nPlayerIdx].m_sExtAuto.uSyncTime)
								{
									Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 5000;
									return 0;
								}
								else if(Player[nPlayerIdx].m_sExtAuto.uSyncTime < uCurTime)
								{
									sStation& c = g_CenterStation[SubWorld[0].m_SubWorldID];
									if(SubWorld[0].FindPath(c.x, c.y) > 0)
									{
										Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 5000;
										Player[nPlayerIdx].m_sExtAuto.nSubStep = 1;
									}
									return 0;
								}
								int nIdx = 0;
								int dX,dY;
								char szBuff[32];
								while (nIdx = NpcSet.GetNextIdx(nIdx))
								{
									if (Npc[nIdx].m_Kind != kind_dialoger)
										continue;
									if (Npc[nIdx].m_RegionIndex < 0)
										continue;
									Npc[nIdx].GetMpsPos(&dX, &dY);
									if(g_GetDistance(nX, nY, dX, dY) < 128)
									{
										strcpy(szBuff, Npc[nIdx].Name);
										g_StrLower(szBuff);
										if(strstr(szBuff, "d­îc") || strstr(szBuff, "thuèc")
										|| strstr(szBuff, "thÇn y"))
										{
											Player[nPlayerIdx].DialogNpc(nIdx);
											return 1;
										}
									}
								}
							}
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 3)
						{	//lùa chän khung dialog
							if(!CoreDataChanged(GDCNI_UI_ACT, 0, 0))
							{
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
								return 0;
							}
							CoreDataChanged(GDCNI_UI_ACT, 1, 0);
							OperationRequest(GOI_QUESTION_CHOOSE, 0, 0);
							++Player[nPlayerIdx].m_sExtAuto.nSubStep;
							return 1;
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 4)
						{	//mua m¸u
							if(!CoreDataChanged(GDCNI_UI_ACT, 2, 0))
							{
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
								return 0;
							}
							if(!pApData->bBuyLife || pApData->nBLNum <= 0)
							{
								++Player[nPlayerIdx].m_sExtAuto.nSubStep;
								return 0;
							}
							int nBuyNum = 0;
							for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
							for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
							{
								int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
								if(nIdx > 0)
								{
									if(Item[nIdx].GetGenre() == item_medicine
									&& Item[nIdx].GetDetailType() == g_LifeBottle[pApData->nBuyLifeSel].nDetail
									&& Item[nIdx].GetLevel() == g_LifeBottle[pApData->nBuyLifeSel].nLevel)
									{
										++nBuyNum;
									}
								}
							}
							if(nBuyNum >= pApData->nBLNum)
							{
								++Player[nPlayerIdx].m_sExtAuto.nSubStep;
								return 0;
							}
							for(int b = 0; b < BuySell.GetWidth(); ++b)
							{
								KItem* pItem = BuySell.GetItem(BuySell.GetItemIndex(Player[nPlayerIdx].m_BuyInfo.m_nShopIdx[Player[nPlayerIdx].m_BuyInfo.m_nCurShop], b));
								if(!pItem)
									break;
								if(pItem->GetGenre() == item_medicine
								&& pItem->GetDetailType() == g_LifeBottle[pApData->nBuyLifeSel].nDetail
								&& pItem->GetLevel() == g_LifeBottle[pApData->nBuyLifeSel].nLevel)
								{
									if (Player[nPlayerIdx].m_ItemList.GetEquipmentMoney() < pItem->GetPrice())
									{
										++Player[nPlayerIdx].m_sExtAuto.nSubStep;
										return 0;
									}
									int x,y;
									if(!Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(1, 1, &x, &y))
									{
										++Player[nPlayerIdx].m_sExtAuto.nSubStep;
										return 0;
									}
									SendClientCmdBuy(Player[nPlayerIdx].m_BuyInfo.m_nCurShop, b, 1, 0);
									return 1;
								}
							}
							++Player[nPlayerIdx].m_sExtAuto.nSubStep;
							return 0;
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 5)
						{	//mua mana
							if(!CoreDataChanged(GDCNI_UI_ACT, 2, 0))
							{
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
								return 0;
							}
							if(!pApData->bBuyMana || pApData->nBMNum <= 0)
							{
								++Player[nPlayerIdx].m_sExtAuto.nSubStep;
								return 0;
							}
							int nBuyNum = 0;
							for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
							for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
							{
								int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
								if(nIdx > 0)
								{
									if(Item[nIdx].GetGenre() == item_medicine
									&& Item[nIdx].GetDetailType() == g_ManaBottle[pApData->nBuyManaSel].nDetail
									&& Item[nIdx].GetLevel() == g_ManaBottle[pApData->nBuyManaSel].nLevel)
									{
										++nBuyNum;
									}
								}
							}
							if(nBuyNum >= pApData->nBMNum)
							{
								++Player[nPlayerIdx].m_sExtAuto.nSubStep;
								return 0;
							}
							for(int b = 0; b < BuySell.GetWidth(); ++b)
							{
								KItem* pItem = BuySell.GetItem(BuySell.GetItemIndex(Player[nPlayerIdx].m_BuyInfo.m_nShopIdx[Player[nPlayerIdx].m_BuyInfo.m_nCurShop], b));
								if(!pItem)
									break;
								if(pItem->GetGenre() == item_medicine
								&& pItem->GetDetailType() == g_ManaBottle[pApData->nBuyManaSel].nDetail
								&& pItem->GetLevel() == g_ManaBottle[pApData->nBuyManaSel].nLevel)
								{
									if (Player[nPlayerIdx].m_ItemList.GetEquipmentMoney() < pItem->GetPrice())
									{
										++Player[nPlayerIdx].m_sExtAuto.nSubStep;
										return 0;
									}
									int x,y;
									if(!Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(1, 1, &x, &y))
									{
										++Player[nPlayerIdx].m_sExtAuto.nSubStep;
										return 0;
									}
									SendClientCmdBuy(Player[nPlayerIdx].m_BuyInfo.m_nCurShop, b, 1, 0);
									return 1;
								}
							}
							++Player[nPlayerIdx].m_sExtAuto.nSubStep;
							return 0;
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 6)
						{	//mua gi¶i ®éc
							if(!CoreDataChanged(GDCNI_UI_ACT, 2, 0))
							{
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
								return 0;
							}
							if(!pApData->bBuyPois || pApData->nBPNum <= 0)
							{
								++Player[nPlayerIdx].m_sExtAuto.nSubStep;
								return 0;
							}
							int nBuyNum = 0;
							for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
							for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
							{
								int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
								if(nIdx > 0)
								{
									if(Item[nIdx].GetGenre() == item_medicine
									&& Item[nIdx].GetDetailType() == g_PoisonBottle[pApData->nBuyPoisSel].nDetail
									&& Item[nIdx].GetLevel() == g_PoisonBottle[pApData->nBuyPoisSel].nLevel)
									{
										++nBuyNum;
									}
								}
							}
							if(nBuyNum >= pApData->nBPNum)
							{
								++Player[nPlayerIdx].m_sExtAuto.nSubStep;
								return 0;
							}
							for(int b = 0; b < BuySell.GetWidth(); ++b)
							{
								KItem* pItem = BuySell.GetItem(BuySell.GetItemIndex(Player[nPlayerIdx].m_BuyInfo.m_nShopIdx[Player[nPlayerIdx].m_BuyInfo.m_nCurShop], b));
								if(!pItem)
									break;
								if(pItem->GetGenre() == item_medicine
								&& pItem->GetDetailType() == g_PoisonBottle[pApData->nBuyPoisSel].nDetail
								&& pItem->GetLevel() == g_PoisonBottle[pApData->nBuyPoisSel].nLevel)
								{
									if (Player[nPlayerIdx].m_ItemList.GetEquipmentMoney() < pItem->GetPrice())
									{
										++Player[nPlayerIdx].m_sExtAuto.nSubStep;
										return 0;
									}
									int x,y;
									if(!Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(1, 1, &x, &y))
									{
										++Player[nPlayerIdx].m_sExtAuto.nSubStep;
										return 0;
									}
									SendClientCmdBuy(Player[nPlayerIdx].m_BuyInfo.m_nCurShop, b, 1, 0);
									return 1;
								}
							}
							++Player[nPlayerIdx].m_sExtAuto.nSubStep;
							return 0;
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 7)
						{
							CoreDataChanged(GDCNI_UI_ACT, 3, 0);
							Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
							++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
						}
					}
					else if(Player[nPlayerIdx].m_sExtAuto.nHomeStep == 7)
					{	//mua phï
						MapStation::iterator it = g_ShopStation.find(SubWorld[0].m_SubWorldID);
						if(it == g_ShopStation.end())
						{
							Player[nPlayerIdx].m_sExtAuto.nHomeStep = 100;
							return 0;
						}
						if(!pApData->bBuyTP)
						{
							Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
							++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
							return 0;
						}
						int nX,nY;
						Npc[nNpcIdx].GetMpsPos(&nX, &nY);
						StationVector& v = ( *it ).second;
						if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 0)
						{	//t×m shop
							int i,j;
							int nTPNum = 0;
							for( i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
							for( j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
							{
								int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
								if(nIdx > 0)
								{
									if(Item[nIdx].GetGenre() == item_townportal)
									{
										++nTPNum;
									}
								}
							}
							if(!pApData->bBuyTP || nTPNum >= pApData->nBTPNum)
							{
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
								++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
								return 0;
							}
							int nDist = -1, nPos = 0;
							for( i=0;i < (int)v.size();++i)
							{
								int nCurDist = g_GetDistance(nX, nY, v[i].x, v[i].y);
								if(nDist < 0)
									nDist = nCurDist;
								if(nCurDist < nDist)
								{
									nPos = i;
									nDist = nCurDist;
								}
							}
							if(SubWorld[0].FindPath(v[nPos].x, v[nPos].y) > 0)
							{
								Player[nPlayerIdx].m_sExtAuto.nCurShop = nPos;
								Player[nPlayerIdx].m_sExtAuto.uSyncTime = 0;
								Player[nPlayerIdx].m_sExtAuto.nSubStep += 2;
							}
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 1)
						{	//trªn ®­êng quay l¹i trung t©m
							int x,y;
							if(!SubWorld[0].HaveTarget(x, y))
							{
								sStation& c = g_CenterStation[SubWorld[0].m_SubWorldID];
								SubWorld[0].FindPath(c.x, c.y);
							}
							else
							{
								sStation& c = g_CenterStation[SubWorld[0].m_SubWorldID];
								if(c.x != x || c.y != y)
								{
									g_ScenePlace.RemoveFlag();
									Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
									return 0;
								}
							}
							if(Player[nPlayerIdx].m_sExtAuto.uSyncTime < uCurTime)
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
							return 0;
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 2)
						{	//trªn ®­êng ®Õn shop
							int x,y;
							if(!SubWorld[0].HaveTarget(x, y))
							{
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
								return 0;
							}
							else
							{
								if(v[Player[nPlayerIdx].m_sExtAuto.nCurShop].x != x
								|| v[Player[nPlayerIdx].m_sExtAuto.nCurShop].y != y)
								{
									g_ScenePlace.RemoveFlag();
									Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
									return 0;
								}
							}
							int nPos = Player[nPlayerIdx].m_sExtAuto.nCurShop;
							int nDist = g_GetDistance(nX, nY, v[nPos].x, v[nPos].y);
							if(nDist < 300)
							{
								if(CoreDataChanged(GDCNI_UI_ACT, 0, 0))
								{
									++Player[nPlayerIdx].m_sExtAuto.nSubStep;
									return 0;
								}
								if(!Player[nPlayerIdx].m_sExtAuto.uSyncTime)
								{
									Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 5000;
									return 0;
								}
								else if(Player[nPlayerIdx].m_sExtAuto.uSyncTime < uCurTime)
								{
									sStation& c = g_CenterStation[SubWorld[0].m_SubWorldID];
									if(SubWorld[0].FindPath(c.x, c.y) > 0)
									{
										Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 5000;
										Player[nPlayerIdx].m_sExtAuto.nSubStep = 1;
									}
									return 0;
								}
								int nIdx = 0;
								int dX,dY;
								char szBuff[32];
								while (nIdx = NpcSet.GetNextIdx(nIdx))
								{
									if (Npc[nIdx].m_Kind != kind_dialoger)
										continue;
									if (Npc[nIdx].m_RegionIndex < 0)
										continue;
									Npc[nIdx].GetMpsPos(&dX, &dY);
									if(g_GetDistance(nX, nY, dX, dY) < 128)
									{
										strcpy(szBuff, Npc[nIdx].Name);
										g_StrLower(szBuff);
										if(strstr(szBuff, "t¹p h"))
										{
											Player[nPlayerIdx].DialogNpc(nIdx);
											return 1;
										}
									}
								}
							}
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 3)
						{	//lùa chän khung dialog
							if(!CoreDataChanged(GDCNI_UI_ACT, 0, 0))
							{
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
								return 0;
							}
							CoreDataChanged(GDCNI_UI_ACT, 1, 0);
							OperationRequest(GOI_QUESTION_CHOOSE, 0, 0);
							++Player[nPlayerIdx].m_sExtAuto.nSubStep;
							return 1;
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 4)
						{	//mua phï
							if(!CoreDataChanged(GDCNI_UI_ACT, 2, 0))
							{
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
								return 0;
							}
							if(!pApData->bBuyTP || pApData->nBTPNum <= 0)
							{
								++Player[nPlayerIdx].m_sExtAuto.nSubStep;
								return 0;
							}
							int nBuyNum = 0;
							for(int i=0;i<EQUIPMENT_ROOM_HEIGHT;++i)
							for(int j=0;j<EQUIPMENT_ROOM_WIDTH;++j)
							{
								int nIdx = Player[nPlayerIdx].m_ItemList.m_Room[room_equipment].FindItem(j, i);
								if(nIdx > 0)
								{
									if(Item[nIdx].GetGenre() == item_townportal)
									{
										++nBuyNum;
									}
								}
							}
							if(nBuyNum >= pApData->nBTPNum)
							{
								++Player[nPlayerIdx].m_sExtAuto.nSubStep;
								return 0;
							}
							for(int b = 0; b < BuySell.GetWidth(); ++b)
							{
								KItem* pItem = BuySell.GetItem(BuySell.GetItemIndex(Player[nPlayerIdx].m_BuyInfo.m_nShopIdx[Player[nPlayerIdx].m_BuyInfo.m_nCurShop], b));
								if(!pItem)
									break;
								if(pItem->GetGenre() == item_townportal)
								{
									if (Player[nPlayerIdx].m_ItemList.GetEquipmentMoney() < pItem->GetPrice())
									{
										++Player[nPlayerIdx].m_sExtAuto.nSubStep;
										return 0;
									}
									int x,y;
									if(!Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(1, 1, &x, &y))
									{
										++Player[nPlayerIdx].m_sExtAuto.nSubStep;
										return 0;
									}
									SendClientCmdBuy(Player[nPlayerIdx].m_BuyInfo.m_nCurShop, b, 1, 0);
									return 1;
								}
							}
							++Player[nPlayerIdx].m_sExtAuto.nSubStep;
							return 0;
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 5)
						{
							CoreDataChanged(GDCNI_UI_ACT, 3, 0);
							Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
							++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
						}
					}
					else if(Player[nPlayerIdx].m_sExtAuto.nHomeStep == 8)
					{	//gi÷ tiÒn
						int nCurMoney = Player[nPlayerIdx].m_ItemList.GetEquipmentMoney();
						if(!pApData->bHoldMoney || pApData->nHoldMoneyNum <= 0)
						{
							if(nCurMoney > 0)
							{
								OperationRequest(GOI_MONEY_INOUT_STORE_BOX, true, nCurMoney);
								nRet = 1;
							}
						}
						else
						{
							int nNeed = pApData->nHoldMoneyNum*10000;
							nCurMoney -= nNeed;
							if(nCurMoney > 0)
							{
								OperationRequest(GOI_MONEY_INOUT_STORE_BOX, true, nCurMoney);
								nRet = 1;
							}
							else if(nCurMoney < 0)
							{
								OperationRequest(GOI_MONEY_INOUT_STORE_BOX, false, -nCurMoney);
								nRet = 1;
							}
						}
						++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
					}
					else if(Player[nPlayerIdx].m_sExtAuto.nHomeStep == 9)
					{	//®i xa phu
						MapStation::iterator it = g_MoveStation.find(SubWorld[0].m_SubWorldID);
						if(it == g_MoveStation.end())
						{
							Player[nPlayerIdx].m_sExtAuto.nHomeStep = 100;
							return 0;
						}
						if(!pApData->bGoStation)
						{
							Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
							++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
							Player[nPlayerIdx].m_sExtAuto.uSyncTime = 0;
							return 0;
						}
						int nX,nY;
						Npc[nNpcIdx].GetMpsPos(&nX, &nY);
						StationVector& v = ( *it ).second;
						if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 0)
						{	//t×m shop
							int nDist = -1, nPos = 0;
							for(int i=0;i < (int)v.size();++i)
							{
								int nCurDist = g_GetDistance(nX, nY, v[i].x, v[i].y);
								if(nDist < 0)
									nDist = nCurDist;
								if(nCurDist < nDist)
								{
									nPos = i;
									nDist = nCurDist;
								}
							}
							if(SubWorld[0].FindPath(v[nPos].x, v[nPos].y) > 0)
							{
								Player[nPlayerIdx].m_sExtAuto.nCurShop = nPos;
								Player[nPlayerIdx].m_sExtAuto.uSyncTime = 0;
								Player[nPlayerIdx].m_sExtAuto.nSubStep += 2;
							}
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 1)
						{	//trªn ®­êng quay l¹i trung t©m
							int x,y;
							if(!SubWorld[0].HaveTarget(x, y))
							{
								sStation& c = g_CenterStation[SubWorld[0].m_SubWorldID];
								SubWorld[0].FindPath(c.x, c.y);
							}
							else
							{
								sStation& c = g_CenterStation[SubWorld[0].m_SubWorldID];
								if(c.x != x || c.y != y)
								{
									g_ScenePlace.RemoveFlag();
									Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
									return 0;
								}
							}
							if(Player[nPlayerIdx].m_sExtAuto.uSyncTime < uCurTime)
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
							return 0;
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 2)
						{	//trªn ®­êng ®Õn shop
							int x,y;
							if(!SubWorld[0].HaveTarget(x, y))
							{
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
								return 0;
							}
							else
							{
								if(v[Player[nPlayerIdx].m_sExtAuto.nCurShop].x != x
								|| v[Player[nPlayerIdx].m_sExtAuto.nCurShop].y != y)
								{
									g_ScenePlace.RemoveFlag();
									Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
									return 0;
								}
							}
							int nPos = Player[nPlayerIdx].m_sExtAuto.nCurShop;
							int nDist = g_GetDistance(nX, nY, v[nPos].x, v[nPos].y);
							if(nDist < 300)
							{
								if(CoreDataChanged(GDCNI_UI_ACT, 0, 0))
								{
									++Player[nPlayerIdx].m_sExtAuto.nSubStep;
									return 0;
								}
								if(!Player[nPlayerIdx].m_sExtAuto.uSyncTime)
								{
									Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 5000;
									return 0;
								}
								else if(Player[nPlayerIdx].m_sExtAuto.uSyncTime < uCurTime)
								{
									sStation& c = g_CenterStation[SubWorld[0].m_SubWorldID];
									if(SubWorld[0].FindPath(c.x, c.y) > 0)
									{
										Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 5000;
										Player[nPlayerIdx].m_sExtAuto.nSubStep = 1;
									}
									return 0;
								}
								int nIdx = 0;
								int dX,dY;
								char szBuff[32];
								while (nIdx = NpcSet.GetNextIdx(nIdx))
								{
									if (Npc[nIdx].m_Kind != kind_dialoger)
										continue;
									if (Npc[nIdx].m_RegionIndex < 0)
										continue;
									Npc[nIdx].GetMpsPos(&dX, &dY);
									if(g_GetDistance(nX, nY, dX, dY) < 128)
									{
										strcpy(szBuff, Npc[nIdx].Name);
										g_StrLower(szBuff);
										if(strstr(szBuff, "xa phu"))
										{
											Player[nPlayerIdx].DialogNpc(nIdx);
											return 1;
										}
									}
								}
							}
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 3)
						{	//lùa chän khung dialog
							if(!CoreDataChanged(GDCNI_UI_ACT, 0, 0))
							{
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
								return 0;
							}
							char szBuff[256];
							for(int i=0;i < CoreDataChanged(GDCNI_UI_ACT, 4, 0);++i)
							{
								CoreDataChanged(GDCNI_UI_ACT, 5, i);
								CoreDataChanged(GDCNI_UI_ACT, 6, (int)&szBuff);
								g_StrLower(szBuff);
								if((pApData->nSelStation == 0 && strstr(szBuff, "l¹i"))
								|| (pApData->nSelStation == 4 && strstr(szBuff, "n¬i lµm")))
								{
									CoreDataChanged(GDCNI_UI_ACT, 1, 0);
									OperationRequest(GOI_QUESTION_CHOOSE, 0, i);
									Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
									++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
									Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 2000;
									return 1;
								}
								else if(pApData->nSelStation > 0 && pApData->nSelStation < 4
								&& strstr(szBuff, "n¬i ®·"))
								{
									CoreDataChanged(GDCNI_UI_ACT, 1, 0);
									OperationRequest(GOI_QUESTION_CHOOSE, 0, i);
									++Player[nPlayerIdx].m_sExtAuto.nSubStep;
									Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 2000;
									return 1;
								}
							}
							CoreDataChanged(GDCNI_UI_ACT, 1, 0);
							Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
							return 0;
						}
						else if(Player[nPlayerIdx].m_sExtAuto.nSubStep == 4)
						{	//lùa chän n¬i ®· ®i qua
							if(CoreDataChanged(GDCNI_UI_ACT, 0, 0))
							{
								int nSel = pApData->nSelStation - 1;
								if(nSel >= 0 && nSel < CoreDataChanged(GDCNI_UI_ACT, 4, 0))
								{
									CoreDataChanged(GDCNI_UI_ACT, 1, 0);
									OperationRequest(GOI_QUESTION_CHOOSE, 0, nSel);
									Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
									++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
									Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 2000;
									return 1;
								}
								else
								{
									CoreDataChanged(GDCNI_UI_ACT, 1, 0);
									Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
									++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
									Player[nPlayerIdx].m_sExtAuto.uSyncTime = 0;
									return 0;
								}
							}
							if(Player[nPlayerIdx].m_sExtAuto.uSyncTime < uCurTime)
							{
								CoreDataChanged(GDCNI_UI_ACT, 1, 0);
								Player[nPlayerIdx].m_sExtAuto.nSubStep = 0;
								++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
								Player[nPlayerIdx].m_sExtAuto.uSyncTime = 0;
							}
						}
					}
					else if(Player[nPlayerIdx].m_sExtAuto.nHomeStep == 10)
					{	//®i b¶n ®å b»ng thÇn hµnh phï
						if(g_MoveStation.find(SubWorld[0].m_SubWorldID) == g_MoveStation.end())
						{
							Player[nPlayerIdx].m_sExtAuto.nHomeStep = 100;
							return 0;
						}
						if(!pApData->bGoMap)
						{
							++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
							Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 3000;
							return 0;
						}
						if(Player[nPlayerIdx].m_sExtAuto.uSyncTime > uCurTime)
							return 0;
						Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 1000;
						//di chuyÓn b»ng THP
						char szPack[16];
						DYNAMIC_COMMAND* pCmd = (DYNAMIC_COMMAND*)&szPack[0];
						pCmd->ProtocolType = c2s_dynamic_structure;
						pCmd->nBranch = c2sdnmbr_movemapid;
						pCmd->m_wLength = sizeof(DYNAMIC_COMMAND) - 1 + sizeof(int);
						int* pMapID = (int*)(pCmd+1);
						*pMapID = g_GoMapID[pApData->nSelMap];
						if (g_pClient)
							g_pClient->SendPackToServer(pCmd, pCmd->m_wLength + 1);
						++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
						Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 3000;
						return 1;
					}
					else if(Player[nPlayerIdx].m_sExtAuto.nHomeStep == 11)
					{	//check cã ®øng gÇn tr¹m
						if(Player[nPlayerIdx].m_sExtAuto.uSyncTime > uCurTime)
							return 0;
						MapStation::iterator it = g_MedicineStation.find(SubWorld[0].m_SubWorldID);
						if(it == g_MedicineStation.end())
						{
							Player[nPlayerIdx].m_sExtAuto.nHomeStep = 100;
							return 0;
						}
						StationVector v = ( *it ).second;
						int nX,nY;
						Npc[nNpcIdx].GetMpsPos(&nX, &nY);
						int i;
						for( i=0;i < (int)v.size();++i)
						{
							if(g_GetDistance(nX, nY, v[i].x, v[i].y) < 200)
							{
								sStation& c = g_CenterStation[SubWorld[0].m_SubWorldID];
								SubWorld[0].FindPath(c.x, c.y);
								Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 5000 + g_Random(10)*1000;
								++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
								return 1;
							}
						}
						it = g_ShopStation.find(SubWorld[0].m_SubWorldID);
						v = ( *it ).second;
						for( i=0;i < (int)v.size();++i)
						{
							if(g_GetDistance(nX, nY, v[i].x, v[i].y) < 200)
							{
								sStation& c = g_CenterStation[SubWorld[0].m_SubWorldID];
								SubWorld[0].FindPath(c.x, c.y);
								Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 5000 + g_Random(10)*1000;
								++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
								return 1;
							}
						}
						it = g_MoveStation.find(SubWorld[0].m_SubWorldID);
						v = ( *it ).second;
						for( i=0;i < (int)v.size();++i)
						{
							if(g_GetDistance(nX, nY, v[i].x, v[i].y) < 200)
							{
								sStation& c = g_CenterStation[SubWorld[0].m_SubWorldID];
								SubWorld[0].FindPath(c.x, c.y);
								Player[nPlayerIdx].m_sExtAuto.uSyncTime = uCurTime + 5000 + g_Random(10)*1000;
								++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
								return 1;
							}
						}
						Player[nPlayerIdx].m_sExtAuto.nHomeStep += 2;
					}
					else if(Player[nPlayerIdx].m_sExtAuto.nHomeStep == 12)
					{	//®ang ch¹y vÒ trung t©m
						int x,y;
						if(!SubWorld[0].HaveTarget(x, y))
						{
							sStation& c = g_CenterStation[SubWorld[0].m_SubWorldID];
							SubWorld[0].FindPath(c.x, c.y);
						}
						else
						{
							sStation& c = g_CenterStation[SubWorld[0].m_SubWorldID];
							if(c.x != x || c.y != y)
							{
								g_ScenePlace.RemoveFlag();
								SubWorld[0].FindPath(c.x, c.y);
							}
						}
						if(Player[nPlayerIdx].m_sExtAuto.uSyncTime > uCurTime)
							return 0;
						g_ScenePlace.RemoveFlag();
						++Player[nPlayerIdx].m_sExtAuto.nHomeStep;
					}
					break;
				}
				case ATYPE_RESETMOVE:
				{
					Player[nPlayerIdx].m_sExtAuto.uTFollMove2 = 0;
					break;
				}
				case ATYPE_MOVE:
				{
					Player[nPlayerIdx].m_sExtAuto.nCurMoveRet = 0;
					if(!Player[nPlayerIdx].m_sExtAuto.bPrevFightState)
						return 0;
					const autoData* pApData = (autoData*)nParam;
					if(!pApData->bMoveFollow && !pApData->bAroundPoint && !pApData->bMoveCoord)
						return 0;
					if(pApData->bMoveFollow)
					{
						if(Player[nPlayerIdx].m_sExtAuto.uTFollMove2 > uCurTime)
							return 0;
						int nIdx = 0;
						while (nIdx = NpcSet.GetNextIdx(nIdx))
						{
							if (Npc[nIdx].m_Kind != kind_player)
								continue;
							if (nIdx == Player[nPlayerIdx].m_nIndex)
								continue;
							if (Npc[nIdx].m_RegionIndex < 0)
								continue;
							if(!strcmp(pApData->szFollName, Npc[nIdx].Name))
							{
								int nDist = NpcSet.GetDistance(nNpcIdx, nIdx);
								if((!pApData->bFight && nDist > 75) || (nDist > 75
								&& (Player[nPlayerIdx].m_sExtAuto.uTFollMove1 > uCurTime
								|| nDist > pApData->nFollowDist)))
								{
									g_ScenePlace.RemoveFlag();
									if(pApData->bMoveUpHorse && !Npc[nNpcIdx].m_bRideHorse)
									{
										if(!pApData->bFight || Player[nPlayerIdx].FindTargetNpc(
											pApData->nVision, pApData->bFightBack, pApData->nFBVision,
											pApData->nSelBoss) <= 0)
										OperationRequest(GOI_PLAYER_ACTION, PA_RIDE, 0);
									}
									int x,y;
									Npc[nIdx].GetMpsPos(&x, &y);
									if (!Player[nPlayerIdx].m_RunStatus)
									{
										Npc[nNpcIdx].SendCommand(do_walk, x, y);
										SendClientCmdWalk(x, y);
									}
									else
									{
										Npc[nNpcIdx].SendCommand(do_run, x, y);
										SendClientCmdRun(x, y);
									}
									if(nDist > pApData->nFollowDist)
										Player[nPlayerIdx].m_sExtAuto.uTFollMove1 = uCurTime + nDist;
									Player[nPlayerIdx].m_sExtAuto.uNpcID = 0;
									return 1;
								}
								else
								{
									Player[nPlayerIdx].m_mAutoExcludeNpcID.clear();
									if(pApData->bFight)
									Player[nPlayerIdx].m_sExtAuto.uTFollMove2 = uCurTime + 1000;
									else
									Player[nPlayerIdx].m_sExtAuto.uTFollMove2 = 0;
								}
								return 0;
							}
						}
					}
					if(pApData->bAroundPoint)
					{
						if(pApData->nMoveMapId == SubWorld[0].m_SubWorldID
						&& pApData->nPointX > 0 && pApData->nPointY > 0)
						{
							Player[nPlayerIdx].m_sExtAuto.nCurMoveRet = 1;
							int x,y;
							Npc[nNpcIdx].GetMpsPos(&x, &y);
							int nVision = pApData->nVision;
							if(nVision < 100)
								nVision = 100;
							else if(nVision > 1200)
								nVision = 1200;
							int nDist = g_GetDistance(x, y, pApData->nPointX, pApData->nPointY);
							if(nDist >= nVision || nDist > 75)
							{
								if(nDist >= nVision)
								{
									Player[nPlayerIdx].m_sExtAuto.uTOutMove = uCurTime + 1000;
									Player[nPlayerIdx].m_sExtAuto.uNpcID = 0;
								}
								else if(Player[nPlayerIdx].m_sExtAuto.uTOutMove < uCurTime)
								{
									g_ScenePlace.RemoveFlag();
									return 0;
								}
								if(!SubWorld[0].HaveTarget(x, y))
								{
									SubWorld[0].FindPath(pApData->nPointX, pApData->nPointY);
								}
								else
								{
									if(pApData->nPointX != x || pApData->nPointY != y)
									{
										g_ScenePlace.RemoveFlag();
										SubWorld[0].FindPath(pApData->nPointX, pApData->nPointY);
									}
								}
								return 1;
							}
							else
								g_ScenePlace.RemoveFlag();
							return 0;
						}
					}
					if(pApData->bMoveCoord)
					{
						if(pApData->nCoordCount > 0 && pApData->nMoveMapId == SubWorld[0].m_SubWorldID)
						{
							Player[nPlayerIdx].m_sExtAuto.nCurMoveRet = 2;
							int nX,nY;
							Npc[nNpcIdx].GetMpsPos(&nX, &nY);
							int nVision = pApData->nVision;
							if(nVision < 100)
								nVision = 100;
							else if(nVision > 1200)
								nVision = 1200;
							if(Player[nPlayerIdx].m_sExtAuto.nCoordStep >= pApData->nCoordCount)
								Player[nPlayerIdx].m_sExtAuto.nCoordStep = 0;
							int nDist = g_GetDistance(nX, nY,
								pApData->sMoveCoord[Player[nPlayerIdx].m_sExtAuto.nCoordStep].x,
								pApData->sMoveCoord[Player[nPlayerIdx].m_sExtAuto.nCoordStep].y);
							if(pApData->bEncircle
							&& Player[nPlayerIdx].m_sExtAuto.uTEncircle > uCurTime)
							{
								g_ScenePlace.RemoveFlag();
								int x = Player[nPlayerIdx].m_sExtAuto.sEncircle[8].x;
								int y = Player[nPlayerIdx].m_sExtAuto.sEncircle[8].y;
								int nVS = pApData->nVision;
								if(nVS < 600)
									nVS = 600;
								int nTGNpcIdx = Player[nPlayerIdx].FindTargetNpc(
									nVS, pApData->bFightBack, pApData->nFBVision,
									pApData->nSelBoss, TRUE, NULL, pApData->bMoveFollow, x, y);
								if(nTGNpcIdx > 0)
								{
									UINT i = Player[nPlayerIdx].m_sExtAuto.nCurEncircle;
									x = Player[nPlayerIdx].m_sExtAuto.sEncircle[i].x;
									y = Player[nPlayerIdx].m_sExtAuto.sEncircle[i].y;
									UINT uRemain = 9000 - (Player[nPlayerIdx].m_sExtAuto.uTEncircle - uCurTime);
									if(g_GetDistance(nX, nY, x, y) < 64
									|| uRemain > (i+1)*1000)
									{
										++i;
										if(i >= 9)
										{
											Player[nPlayerIdx].m_sExtAuto.uTEncircle = 0;
											return 1;
										}
										Player[nPlayerIdx].m_sExtAuto.nCurEncircle = i;
										x = Player[nPlayerIdx].m_sExtAuto.sEncircle[i].x;
										y = Player[nPlayerIdx].m_sExtAuto.sEncircle[i].y;
									}
									if (!Player[nPlayerIdx].m_RunStatus)
									{
										Npc[nNpcIdx].SendCommand(do_walk, x, y);
										SendClientCmdWalk(x, y);
									}
									else
									{
										Npc[nNpcIdx].SendCommand(do_run, x, y);
										SendClientCmdRun(x, y);
									}
									return 1;
								}
								else
								{
									Player[nPlayerIdx].m_sExtAuto.uTEncircle = 0;
									return 1;
								}
							}
							if(nDist >= nVision || nDist > 75)
							{
								if(!(pApData->bMoveKillMons && pApData->bFight)
								|| !Player[nPlayerIdx].m_sExtAuto.nTempX)
								{
									if(nDist >= nVision)
									{
										Player[nPlayerIdx].m_sExtAuto.uTOutMove = uCurTime + 1000;
										Player[nPlayerIdx].m_sExtAuto.uNpcID = 0;
									}
									else if(Player[nPlayerIdx].m_sExtAuto.uTOutMove < uCurTime)
									{
										g_ScenePlace.RemoveFlag();
										if(!pApData->bFight)
										{
											++Player[nPlayerIdx].m_sExtAuto.nCoordStep;
											if(Player[nPlayerIdx].m_sExtAuto.nCoordStep >= pApData->nCoordCount)
												Player[nPlayerIdx].m_sExtAuto.nCoordStep = 0;
											Player[nPlayerIdx].m_sExtAuto.nTempX = 0;
											Player[nPlayerIdx].m_sExtAuto.nTempY = 0;
											Player[nPlayerIdx].m_sExtAuto.bReachDes = FALSE;
										}
										return 0;
									}
									int x,y;
									if(!SubWorld[0].HaveTarget(x, y))
									{
										SubWorld[0].FindPath(
										pApData->sMoveCoord[Player[nPlayerIdx].m_sExtAuto.nCoordStep].x,
										pApData->sMoveCoord[Player[nPlayerIdx].m_sExtAuto.nCoordStep].y);
										if(Npc[nNpcIdx].m_CurrentRunSpeed <= 6)
											Player[nPlayerIdx].m_sExtAuto.uTJustMove = uCurTime + 10000;
										else if(Npc[nNpcIdx].m_CurrentRunSpeed <= 10)
											Player[nPlayerIdx].m_sExtAuto.uTJustMove = uCurTime + 8000;
										else if(Npc[nNpcIdx].m_CurrentRunSpeed <= 20)
											Player[nPlayerIdx].m_sExtAuto.uTJustMove = uCurTime + 6000;
										else
											Player[nPlayerIdx].m_sExtAuto.uTJustMove = uCurTime + 4000;
									}
									else
									{
										if(pApData->sMoveCoord[Player[nPlayerIdx].m_sExtAuto.nCoordStep].x != x
										|| pApData->sMoveCoord[Player[nPlayerIdx].m_sExtAuto.nCoordStep].y != y)
										{
											g_ScenePlace.RemoveFlag();
											SubWorld[0].FindPath(
											pApData->sMoveCoord[Player[nPlayerIdx].m_sExtAuto.nCoordStep].x,
											pApData->sMoveCoord[Player[nPlayerIdx].m_sExtAuto.nCoordStep].y);
											if(Npc[nNpcIdx].m_CurrentRunSpeed <= 6)
												Player[nPlayerIdx].m_sExtAuto.uTJustMove = uCurTime + 10000;
											else if(Npc[nNpcIdx].m_CurrentRunSpeed <= 10)
												Player[nPlayerIdx].m_sExtAuto.uTJustMove = uCurTime + 8000;
											else if(Npc[nNpcIdx].m_CurrentRunSpeed <= 20)
												Player[nPlayerIdx].m_sExtAuto.uTJustMove = uCurTime + 6000;
											else
												Player[nPlayerIdx].m_sExtAuto.uTJustMove = uCurTime + 4000;
										}
									}
								}
								if(pApData->bMoveKillMons && pApData->bFight && nDist >= nVision)
								{
									if(!Player[nPlayerIdx].m_sExtAuto.nTempX)
									{
										if(Player[nPlayerIdx].m_sExtAuto.uTJustMove < uCurTime)
										{
											int nTGNpcIdx = Player[nPlayerIdx].FindTargetNpc(
											pApData->nVision, pApData->bFightBack, pApData->nFBVision,
											pApData->nSelBoss);
											if(nTGNpcIdx > 0)
											{
												g_ScenePlace.RemoveFlag();
												Player[nPlayerIdx].m_sExtAuto.nCurMoveRet = 3;
												Player[nPlayerIdx].m_sExtAuto.nTempX = nX;
												Player[nPlayerIdx].m_sExtAuto.nTempY = nY;
												return 0;
											}
										}
									}
									else
									{
										Player[nPlayerIdx].m_sExtAuto.nCurMoveRet = 3;
										return 0;
									}
								}
								else
								{
									Player[nPlayerIdx].m_sExtAuto.nTempX = 0;
									Player[nPlayerIdx].m_sExtAuto.nTempY = 0;
								}
								if(pApData->bMoveUpHorse)
								{
									if(nDist > 1500 && !Npc[nNpcIdx].m_bRideHorse)
										OperationRequest(GOI_PLAYER_ACTION, PA_RIDE, 0);
								}
								return 1;
							}
							else
							{
								g_ScenePlace.RemoveFlag();
								if(!pApData->bFight)
								{
									++Player[nPlayerIdx].m_sExtAuto.nCoordStep;
									if(Player[nPlayerIdx].m_sExtAuto.nCoordStep >= pApData->nCoordCount)
										Player[nPlayerIdx].m_sExtAuto.nCoordStep = 0;
									Player[nPlayerIdx].m_sExtAuto.nTempX = 0;
									Player[nPlayerIdx].m_sExtAuto.nTempY = 0;
									Player[nPlayerIdx].m_sExtAuto.bReachDes = FALSE;
								}
								else if(pApData->bEncircle
								&& !Player[nPlayerIdx].m_sExtAuto.bReachDes)
								{
									int x = pApData->sMoveCoord[Player[nPlayerIdx].m_sExtAuto.nCoordStep].x;
									int y = pApData->sMoveCoord[Player[nPlayerIdx].m_sExtAuto.nCoordStep].y;
									int nTGNpcIdx = Player[nPlayerIdx].FindTargetNpc(
									pApData->nVision, pApData->bFightBack, pApData->nFBVision,
									pApData->nSelBoss, TRUE, NULL, pApData->bMoveFollow, x, y);
									if(nTGNpcIdx > 0)
									{
										Player[nPlayerIdx].m_sExtAuto.bReachDes = TRUE;
										Player[nPlayerIdx].m_sExtAuto.nCurEncircle = 0;
										Player[nPlayerIdx].m_sExtAuto.uTEncircle = uCurTime + 9000;
										int i;
										for(i = 0; i < 4; ++i)
										{
											Player[nPlayerIdx].m_sExtAuto.sEncircle[i].x =
												x + ((500 * g_DirCos(i*15, 64)) >> 10);
											Player[nPlayerIdx].m_sExtAuto.sEncircle[i].y =
												y + ((500 * g_DirSin(i*15, 64)) >> 10);
										}
										for(i = 0; i < 4; ++i)
										{
											Player[nPlayerIdx].m_sExtAuto.sEncircle[i+4].x =
												x + ((250 * g_DirCos(i*15, 64)) >> 10);
											Player[nPlayerIdx].m_sExtAuto.sEncircle[i+4].y =
												y + ((250 * g_DirSin(i*15, 64)) >> 10);
										}
										Player[nPlayerIdx].m_sExtAuto.sEncircle[8].x = x;
										Player[nPlayerIdx].m_sExtAuto.sEncircle[8].y = y;
										return 1;
									}
									Player[nPlayerIdx].m_sExtAuto.sEncircle[8].x = x;
									Player[nPlayerIdx].m_sExtAuto.sEncircle[8].y = y;
									return 1;
								}
							}
							return 0;
						}
					}
					break;
				}
				case ATYPE_DATAU:
				{
					return DT_Process(nPlayerIdx, (const autoData*)nParam, uCurTime);
				}
				case ATYPE_SETSELSV1:
				{
					PlayerSet.m_nSelSvGroup = nParam;
					break;
				}
				case ATYPE_SETSELSV2:
				{
					PlayerSet.m_nSelServer = nParam;
					break;
				}
				case ATYPE_SETACC:
				{
					strcpy(PlayerSet.m_szAccount, (char*)nParam);
					break;
				}
				case ATYPE_SETPASS:
				{
					strcpy(PlayerSet.m_szPassword, (char*)nParam);
					for (int i = 0; i < strlen(PlayerSet.m_szPassword); ++i)
					{
						if(PlayerSet.m_szPassword[i] != -1)
						PlayerSet.m_szPassword[i] = ~PlayerSet.m_szPassword[i];
					}
					break;
				}
			}
		}
		break;

	case GOI_SEND_MSG:
		if (uParam && nParam)
			Player[CLIENT_PLAYER_INDEX].SendChat((KUiMsgParam*)nParam, (char*)uParam);
		break;
	case GOI_SET_SEND_CHAT_CHANNEL:	
		if (uParam)
		{
			KUiChatChannel* pChannelInfo = (KUiChatChannel*)uParam;// pChannelInfo
			Player[CLIENT_PLAYER_INDEX].m_cChat.SetCurChannel(pChannelInfo->nChannelNo, pChannelInfo->uChannelId, pChannelInfo->nChannelIndex);
		}
		break;

	case GOI_SET_SEND_WHISPER_CHANNEL:
		if (uParam)
		{
			if (((KUiPlayerItem*)uParam)->nIndex)
			{
				int	nIdx = NpcSet.SearchName(((KUiPlayerItem*)uParam)->Name);
				if (nIdx > 0)
				{
					Player[CLIENT_PLAYER_INDEX].m_cChat.SetCurChannel(CHAT_CUR_CHANNEL_SCREENSINGLE, Npc[nIdx].m_dwID, 0);
				}
				else
				{
					KSystemMessage	sMsg;
					sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_NOT_NEAR, ((KUiPlayerItem*)uParam)->Name);
					sMsg.eType = SMT_NORMAL;
					sMsg.byConfirmType = SMCT_NONE;
					sMsg.byPriority = 0;
					sMsg.byParamSize = 0;
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
				}
			}
			else
			{
				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_NOT_ONLINE, ((KUiPlayerItem*)uParam)->Name);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}
		}
		break;

	case GOI_SET_RECEIVE_CHAT_CHANNEL:
		if (uParam)
		{
			KUiChatChannel* pChannelInfo = (KUiChatChannel*)uParam;// pChannelInfo
			if (nParam)
			{
				Player[CLIENT_PLAYER_INDEX].m_cChat.AddChannel(pChannelInfo->nChannelNo);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cChat.SubChannel(pChannelInfo->nChannelNo);
			}
		}
		break;

	case GOI_CHAT_GROUP_NEW:
		Player[CLIENT_PLAYER_INDEX].m_cChat.AddTeam(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name, (char*)uParam);
		CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
		break;

	case GOI_CHAT_GROUP_RENAME:
		Player[CLIENT_PLAYER_INDEX].m_cChat.RenameTeam(nParam, (char*)uParam, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
		CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
		break;

	case GOI_CHAT_GROUP_DELETE:
		Player[CLIENT_PLAYER_INDEX].m_cChat.DeleteTeam(nParam, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);
		CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
		break;

	case GOI_CHAT_FRIEND_ADD:
		{
			if (g_pClient)
			{
				size_t pckgsize = sizeof(tagExtendProtoHeader) + sizeof(ASK_ADDFRIEND_CMD);

				tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
				pExHeader->ProtocolType = c2s_extendfriend;
				pExHeader->wLength = pckgsize - 1;

				ASK_ADDFRIEND_CMD* pAafCmd = (ASK_ADDFRIEND_CMD*)(pExHeader + 1);
				pAafCmd->ProtocolFamily = pf_friend;
				pAafCmd->ProtocolID = friend_c2c_askaddfriend;
				pAafCmd->pckgid = -1;
				strncpy(pAafCmd->dstrole, ((KUiPlayerItem*)uParam)->Name, _NAME_LEN);
				g_pClient->SendPackToServer(pExHeader, pckgsize);
		
				//Player[CLIENT_PLAYER_INDEX].m_cChat.ApplyAddFriend(((KUiPlayerItem*)uParam)->uId);

				KSystemMessage	sMsg;
				sprintf(sMsg.szMessage, MSG_CHAT_APPLY_ADD_FRIEND, ((KUiPlayerItem*)uParam)->Name);
				sMsg.eType = SMT_NORMAL;
				sMsg.byConfirmType = SMCT_NONE;
				sMsg.byPriority = 0;
				sMsg.byParamSize = 0;
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
			}
		}
		break;

	case GOI_CHAT_FRIEND_DELETE:
		{
			Player[CLIENT_PLAYER_INDEX].m_cChat.ApplyDeleteFriend(((KUiPlayerItem*)uParam)->uId, nParam, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].Name);

			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHAT_DELETE_FRIEND, ((KUiPlayerItem*)uParam)->Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;

	case GOI_CHAT_FRIEND_MOVE:
		{
			KUiPlayerItem	*pPlayer = (KUiPlayerItem*)uParam;
			int		nSrcTeamNo = Player[CLIENT_PLAYER_INDEX].m_cChat.GetTeamNo(pPlayer->uId);
			if (nSrcTeamNo == -1)
			{
				break;
			}
			if (Player[CLIENT_PLAYER_INDEX].m_cChat.OneFriendChangeTeam(pPlayer->uId, nSrcTeamNo, nParam))
			{	
				CoreDataChanged(GDCNI_CHAT_GROUP, 0, 0);
			}
			else
			{

			}
		}
		break;

	case GOI_CHAT_WORDS_TO_FRIEND:
		{
			char	szSentence[MAX_SENTENCE_LENGTH];
			KUiChatMessage	*pMsg = (KUiChatMessage*)uParam;
			KUiMsgParam	sMsg;

			sMsg.eGenre = MSG_G_CHAT;
			sMsg.szName[0] = 0;
			memset(sMsg.cChatPrefix, 0, sizeof(sMsg.cChatPrefix));
			sMsg.cChatPrefixLen = sizeof(unsigned int);
			sMsg.cChatPrefix[0] = ((char*)(&pMsg->uColor))[3];
			sMsg.cChatPrefix[1] = ((char*)(&pMsg->uColor))[2];
			sMsg.cChatPrefix[2] = ((char*)(&pMsg->uColor))[1];
			sMsg.cChatPrefix[3] = ((char*)(&pMsg->uColor))[0];

			if (pMsg->nContentLen >= MAX_SENTENCE_LENGTH)
			{
				memcpy(szSentence, pMsg->szContent, MAX_SENTENCE_LENGTH - 1);
				szSentence[MAX_SENTENCE_LENGTH - 1] = 0;
				sMsg.nMsgLength = MAX_SENTENCE_LENGTH - 1;
			}
			else
			{
				memcpy(szSentence, pMsg->szContent, pMsg->nContentLen);
				szSentence[pMsg->nContentLen] = 0;
				sMsg.nMsgLength = pMsg->nContentLen;
			}

			Player[CLIENT_PLAYER_INDEX].m_cChat.QQSendSentence(
				((KUiPlayerItem*)nParam)->uId,
				((KUiPlayerItem*)nParam)->nIndex, &sMsg, szSentence);
		}
		break;

	case GOI_CHAT_FRIEND_INVITE:
		if (uParam)
		{
			if (g_pClient)
			{
				size_t pckgsize = sizeof(tagExtendProtoHeader) + sizeof(REP_ADDFRIEND_CMD);

				tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
				pExHeader->ProtocolType = c2s_extendfriend;
				pExHeader->wLength = pckgsize - 1;
				
				REP_ADDFRIEND_CMD* pRafCmd = (REP_ADDFRIEND_CMD*)(pExHeader + 1);
				pRafCmd->ProtocolFamily = pf_friend;
				pRafCmd->ProtocolID = friend_c2c_repaddfriend;
				pRafCmd->pckgid = -1;
				strncpy(pRafCmd->dstrole, ((KUiPlayerItem*)uParam)->Name, _NAME_LEN);
				pRafCmd->answer = nParam ? answerAgree : answerDisagree;
				g_pClient->SendPackToServer(pExHeader, pckgsize);

			}
		}
		break;

	case GOI_OPTION_SETTING:			
		if (uParam == OPTION_DYNALIGHT)
		{
			g_ScenePlace.EnableDynamicLights(nParam != 0);
			if (g_pRepresent)
				g_pRepresent->SetOption(DYNAMICLIGHT, nParam != 0);
		}
		else if (uParam == OPTION_PERSPECTIVE)
		{
			if (g_pRepresent)
				g_pRepresent->SetOption(PERSPECTIVE, nParam != 0);
		}
		else if (uParam == OPTION_MUSIC_VALUE)
			Option.SetMusicVolume(nParam);
		else if (uParam == OPTION_SOUND_VALUE)
			Option.SetSndVolume(nParam);
		else if (uParam == OPTION_BRIGHTNESS)
			Option.SetGamma(nParam);
		else if (uParam == OPTION_WEATHER)
			g_ScenePlace.EnableWeather(nParam);

		else if(uParam == OPTION_QUALITY_GIAMPLAYER)//add by phong kiÒu
		{
			Option.SetLow(LowPlayer, nParam);
		}
		else if(uParam == OPTION_QUALITY_MATNPC)
		{
			Option.SetLow(LowNpc, nParam);
		}
		else if(uParam == OPTION_QUALITY_MATPLAYER)
		{
			Option.SetLow(LowEstPlayer, nParam);
		}
		else if(uParam == OPTION_QUALITY_GIAMSKILL)
		{
			Option.SetLow(LowEstMissle, nParam);
		}
	
		break;

	case GOI_VIEW_PLAYERITEM:
		{
			g_cViewItem.ApplyViewEquip(uParam);
		}
		break;
	case GOI_VIEW_PLAYERITEM_END:
		g_cViewItem.DeleteAll();
		break;
	case GOI_AUTO_PLAY: 
		{ 
			switch(uParam) 
			{ 
			case 0:
				Player[CLIENT_PLAYER_INDEX].m_bBuffSkill[0] = nParam;
				break;
			case 1:
				Player[CLIENT_PLAYER_INDEX].m_bBuffSkill[1] = nParam;
				break;
			case 2:
				Player[CLIENT_PLAYER_INDEX].m_bBuffSkill[2] = nParam;
				break;
			case 3:
				Player[CLIENT_PLAYER_INDEX].m_bFollowPeople = nParam;
				break;
			case 4:	
				strcpy(Player[CLIENT_PLAYER_INDEX].m_FollowPeopleName,(char*)nParam);
				Player[CLIENT_PLAYER_INDEX].m_FollowPeopleIdx = NpcSet.SearchName(Player[CLIENT_PLAYER_INDEX].m_FollowPeopleName);
				break;
			case 5:
				Player[CLIENT_PLAYER_INDEX].m_AutoAttack = nParam;
				break;
			case 6:
				Player[CLIENT_PLAYER_INDEX].m_bAttackAround = nParam;
				if(nParam)
				{
					Player[CLIENT_PLAYER_INDEX].SendInfoAuto();
				}
				break;
			case 7:
				Player[CLIENT_PLAYER_INDEX].m_RadiusAuto = nParam;
				break;
			case 8:
				Player[CLIENT_PLAYER_INDEX].m_btDurabilityItem = nParam;
				break;
			case 9:
				Player[CLIENT_PLAYER_INDEX].m_btDurabilityValue = nParam;
				break;
			case 10:
				Player[CLIENT_PLAYER_INDEX].m_AuraSkill[0] = nParam;
				break;
			case 11:
				Player[CLIENT_PLAYER_INDEX].m_AuraSkill[1] = nParam;
				break;
			case 12:
				Player[CLIENT_PLAYER_INDEX].m_bActiveSwitchAura = nParam;
				break;
			case 13:
				Player[CLIENT_PLAYER_INDEX].m_ArrayStateSkill[0] = nParam;
				break;
			case 14:
				Player[CLIENT_PLAYER_INDEX].m_AutoLifeReplenish = nParam; //auto buff
				break;
			case 15:	
				Player[CLIENT_PLAYER_INDEX].m_AutoLifeReplenishP = nParam; //buff theo phÇn tr¨m m¸u
				break;
			case 16:
				Player[CLIENT_PLAYER_INDEX].m_AutoMove = nParam;
				break;
			case 17:
				Player[CLIENT_PLAYER_INDEX].m_bPickItem = nParam;
				break;
			case 18:
				Player[CLIENT_PLAYER_INDEX].m_btPickUpKind = nParam;
				break;
			case 19:
				Player[CLIENT_PLAYER_INDEX].m_bFilterEquipment = nParam;
				break;
			case 20://gi÷ trang søc
				{
					Player[CLIENT_PLAYER_INDEX].m_SaveRAP = nParam;
					Player[CLIENT_PLAYER_INDEX].m_bSaveJewelry = nParam;
					break;
				}
			case 21:
				Player[CLIENT_PLAYER_INDEX].m_EatLife = nParam;
				break;
			case 22:
				Player[CLIENT_PLAYER_INDEX].m_EatMana = nParam;
				break;
			case 23:	
				Player[CLIENT_PLAYER_INDEX].m_LifeAutoV = nParam;
				break;
			case 24:	
				Player[CLIENT_PLAYER_INDEX].m_LifeTimeUse = nParam;
				break;
			case 25:	
				Player[CLIENT_PLAYER_INDEX].m_ManaAutoV = nParam;
				break;
			case 26:	
				Player[CLIENT_PLAYER_INDEX].m_ManaTimeUse = nParam;
				break;
			case 27:
				Player[CLIENT_PLAYER_INDEX].m_TPLife = nParam;
				break;
			case 28:
				Player[CLIENT_PLAYER_INDEX].m_TPMana = nParam;
				break;
			case 29:	
				Player[CLIENT_PLAYER_INDEX].m_TPLifeV = nParam;
				break;
			case 30:	
				Player[CLIENT_PLAYER_INDEX].m_TPManaV = nParam;
				break;
			case 31:
				Player[CLIENT_PLAYER_INDEX].m_TPNotMedicineBlood = nParam;
				break;
			case 32:
				Player[CLIENT_PLAYER_INDEX].m_TPNotMedicineMana = nParam;
				break;
			case 33:
				Player[CLIENT_PLAYER_INDEX].m_TPHightMoney = nParam;
				break;
			case 34:	
				Player[CLIENT_PLAYER_INDEX].m_TPHightMoneyV = nParam;
				break;
			case 35:
				Player[CLIENT_PLAYER_INDEX].m_AutoAntiPoison = nParam;
				break;
			case 36:
				Player[CLIENT_PLAYER_INDEX].m_AutoTTL = nParam;
				break;
			case 37:
				Player[CLIENT_PLAYER_INDEX].m_bActiveAutoParty = nParam;	//kÝch ho¹t tÝnh n¨ng tæ ®éi
				break;
			case 38:
				Player[CLIENT_PLAYER_INDEX].m_bAutoParty = nParam;	//tù ®éng tæ ®éi
				break;
			case 39:
				Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bAutoAccecpt = nParam;	//tù ®éng nhËn lêi mêi
				break;
			case 40:
				Player[CLIENT_PLAYER_INDEX].m_DistanceAuto = nParam;
				break;
			case 41:
				Player[CLIENT_PLAYER_INDEX].m_bFightDistance = nParam;
				break;
			case 42:
				Player[CLIENT_PLAYER_INDEX].m_ArrayStateSkill[1] = nParam;
				break;
			case 43:
				Player[CLIENT_PLAYER_INDEX].m_ArrayStateSkill[2] = nParam;
				break;
			case 44:
				Player[CLIENT_PLAYER_INDEX].m_bAttackPeople = !nParam;
				Player[CLIENT_PLAYER_INDEX].m_bAttackNpc = nParam;
				break;
			case 45:
				Player[CLIENT_PLAYER_INDEX].m_bAttackPeople = nParam;
				Player[CLIENT_PLAYER_INDEX].m_bAttackNpc = !nParam;
				break;
			case 46:
				Player[CLIENT_PLAYER_INDEX].m_bAttackPeople = nParam;
				Player[CLIENT_PLAYER_INDEX].m_bAttackNpc = nParam;
				break;
			case 47:
				if(nParam)
				{
					Player[CLIENT_PLAYER_INDEX].GetAutoQDXY((char *)nParam);
					break;
				}
			case eSortItem:
				{
					Player[CLIENT_PLAYER_INDEX].m_bSortEquipment_Active = nParam;//Set tu xep do
					break;
				}
			case eBuyItem:
				{
					Player[CLIENT_PLAYER_INDEX].m_bBuyEquip = nParam;//Set tu mua do
					break;
				}
			case eInventoryIM:
				{
					Player[CLIENT_PLAYER_INDEX].m_bInventoryMoney = nParam;//Set tu cat item money vao ruong
					Player[CLIENT_PLAYER_INDEX].m_bInventoryItem = nParam;
					break;
				}
			case eRepairEquip:
				{
					Player[CLIENT_PLAYER_INDEX].m_bRepairEquip = nParam;
					break;
				}
			case eReturnPortal:
				{
					Player[CLIENT_PLAYER_INDEX].m_bReturnPortal = nParam;
					break;
				}
			case eAutoTuiDuocPham:
				{
					Player[CLIENT_PLAYER_INDEX].m_Auto_TuiDuocPham = nParam;
					break;
				}
			case eABanItem:
				{
					Player[CLIENT_PLAYER_INDEX].m_Auto_BanItem = nParam;
					break;
				}
			/*case ePickInFightState://nhÆt ®å trong thµnh
				{
					Player[CLIENT_PLAYER_INDEX].m_Auto_PickInFightState = nParam;
					break;
				}*/
			case eAutoRightSkill: //sö dông chiªu bªn ph¶i
				{
					Player[CLIENT_PLAYER_INDEX].m_Auto_SkillRight = nParam;
					break;
				}
			case eAutoPTAll://pt tÊt c¶
				{
					Player[CLIENT_PLAYER_INDEX].m_bAutoAccecptAll = nParam;
					Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bAutoAccecptAll = nParam;
					break;
				}
			default:
				break; 
			} 
		} 
		break;
	case GOI_SUPERSHOP:
		{
			PLAYER_COMMAND	sSS;
			sSS.ProtocolType = c2s_playercommand;
			sSS.m_wMsgID = enumC2S_PLAYERCOMMAND_ID_SUPERSHOP;
			sSS.m_lpBuf = 0;
			sSS.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
			g_pClient->SendPackToServer((BYTE*)&sSS, sSS.m_wLength + 1);
		}
		break;
	case GOI_PLAYER_ACTIONCHAT:
		SendClientActionChatCmd((char *)uParam);
		break;
	case GOI_PLAYER_ACTION:
		{
			switch(uParam)
			{
			case PA_RUN:
				Player[CLIENT_PLAYER_INDEX].m_RunStatus = !Player[CLIENT_PLAYER_INDEX].m_RunStatus;
				break;
			case PA_RIDE:// len xuong ngua
				if(Player[CLIENT_PLAYER_INDEX].m_ItemList.GetEquipment(itempart_horse) <= 0)
					break;
				if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
				{
				   if(GetTickCount() - Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TimeHorse >= TIME_RIDE)
					{	
					   if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
					   {
						   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TimeHorse = GetTickCount();
						   SendClientCmdRide(FALSE);// len xuong ngua
					   }
					   else
					   {
						   Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TimeHorse = GetTickCount();
						   SendClientCmdRide(TRUE);// len xuong ngua
					   }
					}
					else
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMT_NORMAL;
						Msg.eType = SMCT_NONE;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						sprintf(Msg.szMessage, "B¹n qu¸ mÖt mái, kh«ng thÓ tiÕp tôc lªn xuèng ngùa!");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					 }
				}
				else
				{
					KSystemMessage	Msg;
					Msg.byConfirmType = SMT_NORMAL;
					Msg.eType = SMCT_NONE;
					Msg.byPriority = 1;
					Msg.byParamSize = 0;
					sprintf(Msg.szMessage, "B¹n ®ang ngåi thiÒn kh«ng thÓ lªn ngùa");
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
				}
				break;
			case PW_NOT_SAME: // mat khau ko trung nhau
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "MËt khÈu x¸c thùc kh«ng gièng nhau");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case PW_NOT_LONG: // mat khau ko du do dai
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "MËt khÈu ph¶i ®ñ 6 ch÷ sè");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case PW_ACCEPTED: // mat khau dc chap nhan
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "MËt khÈu ®­îc chÊp nhËn");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case HT_CN:
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "Chøc n¨ng nµy ®ang ®­îc hoµn thiÖn");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;

			case CN_GH:
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "Ch­a më khãa r­¬ng. Kh«ng thÓ thùc hiÖn thao t¸c nµy !");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
				break;
			case EX_BOX:
				{	
					int nExbox = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
					if(nExbox == 0)
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "B¹n ch­a më réng r­¬ng ®Õn gÆp ThÈm Cöu t¹i Ba L¨ng HuyÖn 188/198");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
					else 
						CoreDataChanged(GDCNI_OPEN_EX_BOX, NULL, NULL);
				}
				break;
			case EX_BOX2:
				{	
					int nExbox = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
					if(nExbox == 1)
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "B¹n ch­a më réng r­¬ng 2 ®Õn gÆp ThÈm Cöu t¹i Ba L¨ng HuyÖn 188/198");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
					else if(nExbox == 1 || nExbox == 2 || nExbox == 3)
							CoreDataChanged(GDCNI_OPEN_EX_BOX2, NULL, NULL);
				}
				break;
			case EX_BOX3:
				{	
					int nExbox = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExBoxId;
					if(nExbox == 2 || nExbox == 1)
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "B¹n ch­a më réng r­¬ng 3 ®Õn gÆp ThÈm Cöu t¹i Ba L¨ng HuyÖn 188/198");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
					else if(nExbox == 1 || nExbox == 2 || nExbox == 3)
							CoreDataChanged(GDCNI_OPEN_EX_BOX3, NULL, NULL);
				}
				break;
			case ITEMEX:
				{	
					int nItemEX = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_ExItemId;
					if(nItemEX == 0)
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.eType = SMT_NORMAL;
						Msg.byPriority = 1;
						Msg.byParamSize = 0;
						strcpy(Msg.szMessage, "B¹n ch­a mua më réng hµnh trang ë ThÈm Cöu");
						CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
					}
					else if(nItemEX >= 1)
							CoreDataChanged(GDCNI_OPEN_ITEMEX, NULL, NULL);
				}
				break;

			case PA_SIT:
				if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_bRideHorse)
				{
					if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Doing != do_sit)
					{
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_sit);
						SendClientCmdSit(TRUE);
					}
					else
					{
						Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_stand);
						SendClientCmdSit(FALSE);
					}
				}
				else
				{
					KSystemMessage	Msg;
					Msg.byConfirmType = SMCT_CLICK;
					Msg.eType = SMT_PLAYER;
					Msg.byPriority = 1;
					Msg.byParamSize = 0;
					strcpy(Msg.szMessage, "Trong lóc ®i ngùa kh«ng thÓ thùc hiÖn");//edit by phong kieu dang tren ngua khong the ngoi
					CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
				}
				break;
			}
		}
		break;
	case GOI_PK_SETTING:		
		{
			if(!Player[CLIENT_PLAYER_INDEX].m_cPK.GetLockPKState())
			{
				if(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode == enumPKNormal) //Neu dang o trong thanh
				{
					Player[CLIENT_PLAYER_INDEX].m_cPK.ApplySetNormalPKState(nParam, FALSE);
				}
				else
				{
					if(nParam != enumPKNormal)
					{
						Player[CLIENT_PLAYER_INDEX].m_cPK.ApplySetNormalPKState(nParam, FALSE);
					}
					else
					{
						if(GetTickCount() - Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TimeSWPK >= TIME_PK)
						{
							Player[CLIENT_PLAYER_INDEX].m_cPK.ApplySetNormalPKState(nParam, FALSE);
							Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_TimeSWPK = GetTickCount();
						}
						else
						{
							KSystemMessage	Msg;
							Msg.byConfirmType = SMT_NORMAL;
							Msg.eType = SMCT_NONE;
							Msg.byPriority = 1;
							Msg.byParamSize = 0;
							sprintf(Msg.szMessage, "PK luyÖn c«ng chuyÓn sang phi luyÖn c«ng mÊt 3 phót !");
							CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
						}
					}
				}
			}
			else
			{
				KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.eType = SMT_NORMAL;
				Msg.byPriority = 1;
				Msg.byParamSize = 0;
				strcpy(Msg.szMessage, "Khu vùc nµy kh«ng cho phÐp ®æi PK!");
				CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&Msg, 0);
			}
		}
		break;

	case GOI_REVENGE_SOMEONE:
		if (uParam)
		{
			KUiPlayerItem	*pTarget = (KUiPlayerItem*)uParam;
			Player[CLIENT_PLAYER_INDEX].m_cPK.ApplyEnmityPK(pTarget->uId);
		}
		break;

	case GOI_FOLLOW_SOMEONE:
		if (uParam)
		{
			KUiPlayerItem	*pTarget = (KUiPlayerItem*)uParam;
			if (Npc[pTarget->nIndex].m_Kind == kind_player)
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPeopleIdx = pTarget->nIndex;
		}
		break;

	case GOI_SHOW_PLAYERS_NAME:
		NpcSet.SetShowNameFlag(nParam);
		break;

	case GOI_SHOW_PLAYERS_LIFE:
		NpcSet.SetShowLifeFlag(nParam);
		break;

	case GOI_SHOW_PLAYERS_MANA:
		NpcSet.SetShowManaFlag(nParam);
		break;

	case GOI_SHOW_OBJ_NAME:
        ObjSet.SetShowNameFlag(nParam); // hien ten obj duoi dat edit by phong kieu
        break;

	case GOI_DATAU:
		//SendClientDaTau(uParam);
		break;

	case GOI_DATAU1:
		//SendClientDaTau1(uParam);
		break;
	case GOI_DRAW_TARGET_INFO:
	{
		if (uParam && nParam)
		{
			KUiPlayerItem* m_pPlayersList = (KUiPlayerItem*)uParam;
			KUiPlayerPaintTeamMNG* nPainTMG = (KUiPlayerPaintTeamMNG*)nParam;
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].PaintTargetInfo(m_pPlayersList, nPainTMG);
		}
	}
	break;
	case GOI_SET_PLAYER_MERIDIAN:
	{
		if (uParam)
		{
			SendClientSetMeridian((char*)uParam);
		}
	}
	break;
	case GOI_BAUCUA:
	{
		if (uParam)
		{
			SendClientBaucua((char*)uParam);
		}
	}
	break;
	case GOI_MASKFEATURE:
	{
		PLAYER_COMMAND	sMF;
		sMF.ProtocolType = c2s_playercommand;
		sMF.m_wMsgID = enumC2S_PLAYERCOMMAND_ID_MASKFEATURE;
		sMF.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		sMF.m_lpBuf = 0;
		g_pClient->SendPackToServer((BYTE*)&sMF, sMF.m_wLength + 1);
	}
	break;
	case GOI_PROCFRAME_BREATHE:
	// PaintFps interpolation: called once right after each logic tick.
	// Shift the current tick position into "from", store the new tick position in "to".
	// uParam = 1 when POSSHIFT will drive the drawing (PaintInterp on).
	{
		g_bPaintInterpFocus = (uParam != 0);
		g_nCorePaintLog = nParam;
		g_SetCanvasLockProbe(nParam > 0 ? 1 : 0);	// do gia cap Lock/Unlock surface
		if (uParam == 0)
			break;	// PaintInterp off: snapshot would never be consumed
		int	nIdx = 0;
		while (nIdx = NpcSet.GetNextIdx(nIdx))
		{
			if (Npc[nIdx].m_RegionIndex < 0)
			{
				s_InterpValid[nIdx] = 0;
				continue;
			}
			int		nMpsX, nMpsY;
			SubWorld[0].Map2Mps(Npc[nIdx].m_RegionIndex, Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, Npc[nIdx].m_OffX, Npc[nIdx].m_OffY, &nMpsX, &nMpsY);
			if (s_InterpValid[nIdx] && s_InterpNpcID[nIdx] == Npc[nIdx].m_dwID)
			{
				s_InterpFrom[nIdx] = s_InterpTo[nIdx];
			}
			else
			{
				// slot moi hoac doi chu: chua co lich su, khong noi suy khung nay
				s_InterpFrom[nIdx].x = nMpsX;
				s_InterpFrom[nIdx].y = nMpsY;
				s_InterpNpcID[nIdx] = Npc[nIdx].m_dwID;
				s_InterpValid[nIdx] = 1;
			}
			s_InterpTo[nIdx].x = nMpsX;
			s_InterpTo[nIdx].y = nMpsY;
			int	nJumpX = s_InterpTo[nIdx].x - s_InterpFrom[nIdx].x;
			int	nJumpY = s_InterpTo[nIdx].y - s_InterpFrom[nIdx].y;
			if (nJumpX < 0)
				nJumpX = -nJumpX;
			if (nJumpY < 0)
				nJumpY = -nJumpY;
			if (nJumpX > PAINT_INTERP_SNAP_DIST || nJumpY > PAINT_INTERP_SNAP_DIST)
			{
				// teleport / yank: never smear across the map
				s_InterpFrom[nIdx] = s_InterpTo[nIdx];
			}
		}
	}
	break;
	case GOI_PROCFRAME_POSSHIFT:
		// PaintFps interpolation: called once per paint frame, right before UiPaint.
		// uParam = alpha in [0..1000]: 0 = previous tick position, 1000 = current tick position.
		// (nParam is unused, the alpha scale is fixed at 1000).
		// Moves ONLY the drawn position (KNpcRes / scene tree / camera focus);
		// logic coordinates (m_MapX/m_MapY/m_OffX/m_OffY) are never touched here.
	{
		int	nAlpha = (int)uParam;
		if (nAlpha < 0)
			nAlpha = 0;
		if (nAlpha > 1000)
			nAlpha = 1000;
		int	nPlayerNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		int	nIdx = 0;
		while (nIdx = NpcSet.GetNextIdx(nIdx))
		{
			if (Npc[nIdx].m_RegionIndex < 0 ||
				!s_InterpValid[nIdx] || s_InterpNpcID[nIdx] != Npc[nIdx].m_dwID)
				continue;
			BOOL bIsPlayer = (nIdx == nPlayerNpcIdx);
			if (!bIsPlayer &&
				s_InterpFrom[nIdx].x == s_InterpTo[nIdx].x &&
				s_InterpFrom[nIdx].y == s_InterpTo[nIdx].y)
				continue;	// dung yen: tick da dat vi tri roi, khoi ton cong scene
			int	nDrawX = s_InterpFrom[nIdx].x + (s_InterpTo[nIdx].x - s_InterpFrom[nIdx].x) * nAlpha / 1000;
			int	nDrawY = s_InterpFrom[nIdx].y + (s_InterpTo[nIdx].y - s_InterpFrom[nIdx].y) * nAlpha / 1000;
			if (!bIsPlayer)
			{
				// NPC thuong: chi doi vi tri VE. Goi SetPos se keo theo MoveObject ->
				// PluckRto/AddLeafPoint + new/delete den, nam trong critical section dung
				// chung voi luong nap canh; dong nguoi x 60 khung/giay se gay giut du FPS
				// van cao. Sprite doc thang m_nXpos nen bo cay canh o day khong anh huong hinh.
				Npc[nIdx].GetNpcRes()->SetDrawPos(nDrawX, nDrawY, Npc[nIdx].m_Height);
				continue;
			}
			int	nFocusX0 = 0, nFocusY0 = 0, nFocusZ0 = 0;
			g_ScenePlace.GetFocusPosition(nFocusX0, nFocusY0, nFocusZ0);
			Npc[nIdx].GetNpcRes()->SetPos(nIdx, nDrawX, nDrawY, Npc[nIdx].m_Height, TRUE);
			{
				// Single writer: while interpolation is on, only POSSHIFT moves the camera
				// (the logic tick and KSubWorld::LoadMap are muted through g_bPaintInterpFocus),
				// so the focus follows the interpolated position with no backward snap.
				// A region border crossing now happens at paint time and tears the draw tree
				// down (ClearPreprocess/Fell), so rebuild it in the same frame. Measure the
				// focus BEFORE and AFTER SetPos: every early-return inside SetFocusPosition
				// (map drag mode, place not open, unchanged value) is then immune.
				int	nFocusX1, nFocusY1, nFocusZ1;
				g_ScenePlace.GetFocusPosition(nFocusX1, nFocusY1, nFocusZ1);
				if (nFocusX0 / KScenePlaceRegionC::RWPP_AREGION_WIDTH != nFocusX1 / KScenePlaceRegionC::RWPP_AREGION_WIDTH ||
					nFocusY0 / KScenePlaceRegionC::RWPP_AREGION_HEIGHT != nFocusY1 / KScenePlaceRegionC::RWPP_AREGION_HEIGHT)
				{
					g_ScenePlace.Breathe();	// focus really crossed a region border: rebuild the draw tree in the same frame
					nRet = 2;	// report the crossing to the caller (frame-time probe)
				}
			}
		}
		// Ky nang / am khi: chi doi VI TRI VE, KHONG dung cay canh (MoveObject).
		// Nut cay chi dung de sap xep truoc-sau va cat canh - do chinh xac theo tick
		// la du, nen bo qua giup tranh chi phi PluckRto/AddLeafPoint moi khung ve.
		// Missle[] co MAX_MISSLE phan tu => chi so hop le 0..MAX_MISSLE-1.
		// Dung '<=' la cham Missle[MAX_MISSLE], GHI RA NGOAI MANG -> hong bo nho.
		for (int nMsl = 1; nMsl < MAX_MISSLE; nMsl++)
		{
			if (Missle[nMsl].m_nMissleId <= 0 || !Missle[nMsl].m_bInterpValid)
				continue;
			int	nMdx = Missle[nMsl].m_nTickX - Missle[nMsl].m_nPrevX;
			int	nMdy = Missle[nMsl].m_nTickY - Missle[nMsl].m_nPrevY;
			int	nMdz = Missle[nMsl].m_nTickZ - Missle[nMsl].m_nPrevZ;
			if (nMdx == 0 && nMdy == 0 && nMdz == 0)
				continue;	// dung yen (dang cho / bam nguoi): tick da dat dung cho
			if (nMdx > MISSLE_INTERP_SNAP_DIST || nMdx < -MISSLE_INTERP_SNAP_DIST ||
				nMdy > MISSLE_INTERP_SNAP_DIST || nMdy < -MISSLE_INTERP_SNAP_DIST)
				continue;	// nhay xa (moi sinh / doi muc tieu): ve thang tai vi tri tick
			Missle[nMsl].m_nDrawX = Missle[nMsl].m_nPrevX + nMdx * nAlpha / 1000;
			Missle[nMsl].m_nDrawY = Missle[nMsl].m_nPrevY + nMdy * nAlpha / 1000;
			Missle[nMsl].m_nDrawZ = Missle[nMsl].m_nPrevZ + nMdz * nAlpha / 1000;
		}
	}
	break;
	default:
		nRet = 0;
		break;
	}

	return nRet;
}

void KCoreShell::ProcessInput(unsigned int uMsg, unsigned int uParam, int nParam)
{
	Player[CLIENT_PLAYER_INDEX].ProcessInputMsg(uMsg, uParam, nParam);
}

int KCoreShell::CheckMapLoiDai()
{
		int Map = SubWorld[Npc[CLIENT_PLAYER_INDEX].m_SubWorldIndex].m_SubWorldID;
		if (Map != 209)
		return true;
		
	return false;
}


int KCoreShell::FindSelectNPC(int x, int y, int nRelation, bool bSelect, void* pReturn, int& nKind)
{
	Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, nRelation);
	int nT = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();

	if (!bSelect)
		Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);

	if (nT > 0)
	{
		if (pReturn)
		{
			KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
			strncpy(p->Name, Npc[nT].Name, 32);
			p->nIndex = Npc[nT].m_Index;
			p->uId = Npc[nT].m_dwID;
			p->nData = Npc[nT].GetMenuState();
		}
		nKind = Npc[nT].m_Kind;
		return true;
	}
	return false;
}

int KCoreShell::FindSelectObject(int x, int y, bool bSelect, int& nObjectIdx, int& nKind)
{
	Player[CLIENT_PLAYER_INDEX].FindSelectObject(x, y);
	int nT = Player[CLIENT_PLAYER_INDEX].GetTargetObj();
	
	if (!bSelect)
		Player[CLIENT_PLAYER_INDEX].SetTargetObj(0);

	if (nT > 0)
	{
		nObjectIdx = nT;
		nKind = Object[nT].m_nKind;
		return true;
	}
	return false;
}

int KCoreShell::FindSpecialNPC(char* Name, void* pReturn, int& nKind)
{
	if (Name == NULL || Name[0] == 0)
		return false;
	for (int nT = 0; nT < MAX_NPC; nT++)
	{
		if	(strcmp(Npc[nT].Name, Name) == 0)
		{
			if (pReturn)
			{
				KUiPlayerItem* p = (KUiPlayerItem*)pReturn;
				strncpy(p->Name, Npc[nT].Name, 32);
				p->nIndex = Npc[nT].m_Index;
				p->uId = Npc[nT].m_dwID;
				p->nData = Npc[nT].GetMenuState();
			}
			nKind = Npc[nT].m_Kind;
			return true;
		}
	}
	return false;
}

int KCoreShell::ChatSpecialPlayer(void* pPlayer, const char* pMsgBuff, unsigned short nMsgLength)
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC)
		{
			int nTalker = p->nIndex;
			if (Npc[nTalker].m_dwID == p->uId)
			{
				char * pszCheck1 = NULL;
				char * pszCheck2 = NULL;
				unsigned int uMsgLen1 = 0;
				unsigned int uMsgLen2 = 0;
				int nCount = 0;
				int i = 0;
				int j = 0;
				pszCheck1 = (char*)pMsgBuff; //bat dau tu ky tu dau tien cua msg
				BOOL bOk = FALSE;
				while(i < nMsgLength)
				{
					uMsgLen2 = 0;
					if(*pszCheck1 == '[') //khi tim thay ky tu "["
					{
						pszCheck2 = (char*)(pMsgBuff + i);
						nCount = 0;
						j = i;
						while(j < nMsgLength)
						{
							uMsgLen2++;
							if(*pszCheck2 == ']' && nCount == NUM_INFO_ITEM_CHAT) {bOk = TRUE; break;}
							//[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26] // doc tu tu se hieu thoi, cai nay de, phan gia tri nhat thi chi co cho nay
							if(*pszCheck2 == ',') nCount++;
							pszCheck2++;
							j++;
						}
					}
					pszCheck1++;
					if(bOk) {uMsgLen1 = i; break;}
					i++;
				}
				if(bOk)
				{
					return true;
				}

				if(nMsgLength >= 128)	//edit by phong kieu xu ly input lon hon 90 ky tu
				{
					Npc[nTalker].SetChatInfo(p->Name, "JX GiangHoKy", 15);
					return true;
				}
				Npc[nTalker].SetChatInfo(p->Name, pMsgBuff, nMsgLength);
				return true;
			}
		}
	}

	return false;
}

void KCoreShell::ApplyAddTeam(void* pPlayer)
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC && !Player[CLIENT_PLAYER_INDEX].CheckTrading())
		{
			Player[CLIENT_PLAYER_INDEX].ApplyAddTeam(p->nIndex);
		}
	}
}

void KCoreShell::TradeApplyStart(void* pPlayer)
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC && !Player[CLIENT_PLAYER_INDEX].CheckTrading())
		{
			Player[CLIENT_PLAYER_INDEX].TradeApplyStart(p->nIndex);
		}
	}
}

void KCoreShell::GambleApplyStart(void* pPlayer)
{
	KUiPlayerItem* p = (KUiPlayerItem*)pPlayer;
	if (p)
	{
		if (p->nIndex >= 0 && p->nIndex < MAX_NPC && !Player[CLIENT_PLAYER_INDEX].CheckTrading())
		{
			Player[CLIENT_PLAYER_INDEX].GambleApplyStart(p->nIndex);
		}
	}
}

int KCoreShell::UseSkillCastB(int x, int y, int nSkillID, int nNpcIdx)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;

	int nX = x;
	int nY = y;
	int nZ = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (Npc[nIndex].IsCanInput())
	{
		int nIdx = 0;
		nIdx = Npc[nIndex].m_SkillList.FindSame(nSkillID);
		g_DebugLog("[skill]Active");
		Npc[nIndex].SetActiveSkill(nIdx);
	}
	else
	{
		g_DebugLog("[skill]return");
		return 0;
	}

	if (Npc[nIndex].m_ActiveSkillID > 0)
	{
		ISkill * pISkill =  g_SkillManager.GetSkill(Npc[nIndex].m_ActiveSkillID, 1);
		if (!pISkill) 
            return 0;

		if (pISkill->IsAura())
			return 0;

		int nTargetIdx = nNpcIdx;
		if (pISkill->IsTargetOnly() && !nTargetIdx)
        {
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}
		if (nIndex == nTargetIdx)
		{
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}
		if ((!Npc[nIndex].m_SkillList.CanCast(Npc[nIndex].m_ActiveSkillID, SubWorld[Npc[nIndex].m_SubWorldIndex].m_dwCurrentTime))
			||
			(!Npc[nIndex].Cost(pISkill->GetSkillCostType(), pISkill->GetSkillCost(&Npc[nIndex]), TRUE))
			)
		{
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}
		if (!nTargetIdx)
		{
			Npc[nIndex].SendCommand(do_skill, Npc[nIndex].m_ActiveSkillID, nX, nY);
			// Send to Server
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, nX, nY);
		}
		else
		{
			if (pISkill->IsTargetOnly())
			{
				int distance = NpcSet.GetDistance(nIndex , nTargetIdx);
				if (distance > pISkill->GetAttackRadius())
				{
					Player[CLIENT_PLAYER_INDEX].SetTargetNpc(nTargetIdx);
					return 0;
				}
			}

			if (nIndex == nTargetIdx && pISkill->GetSkillStyle() == SKILL_SS_Missles) 
				return 0;
			Npc[nIndex].SendCommand(do_skill, Npc[nIndex].m_ActiveSkillID, -1, nTargetIdx);
			// Send to Server		
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, -1, Npc[nTargetIdx].m_dwID);
		}
	}
	Npc[nIndex].m_nPeopleIdx = 0;
	return 1;
}

int KCoreShell::UseSkill(int x, int y, int nSkillID)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;
	
	int nX = x;
	int nY = y;
	int nZ = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (Npc[nIndex].IsCanInput())
	{
		int nIdx = 0;
		nIdx = Npc[nIndex].m_SkillList.FindSame(nSkillID);
		g_DebugLog("[skill]Active");
		Npc[nIndex].SetActiveSkill(nIdx);
	}
	else
	{
		g_DebugLog("[skill]return");
		return 0;
	}

	if (Npc[nIndex].m_ActiveSkillID > 0)
	{
		ISkill * pISkill =  g_SkillManager.GetSkill(Npc[nIndex].m_ActiveSkillID, 1);
		if (!pISkill) 
            return 0;

		if (pISkill->IsAura())
			return 0;

		int nAttackRange = pISkill->GetAttackRadius();

		int nTargetIdx = 0;
		
		if (pISkill->IsTargetAlly())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, relation_ally);
			if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
			}
		}

		if (pISkill->IsTargetEnemy())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectNpc(x, y, relation_enemy);
			if (Player[CLIENT_PLAYER_INDEX].GetTargetNpc())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetNpc();
			}
		}

		if (pISkill->IsTargetObj())
		{
			Player[CLIENT_PLAYER_INDEX].FindSelectObject(x, y);
			if (Player[CLIENT_PLAYER_INDEX].GetTargetObj())
			{
				nTargetIdx = Player[CLIENT_PLAYER_INDEX].GetTargetObj();
			}
		}

		if (pISkill->IsTargetOnly() && !nTargetIdx)
        {
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}
		
		if (nIndex == nTargetIdx)
		{
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}

		if ((!Npc[nIndex].m_SkillList.CanCast(Npc[nIndex].m_ActiveSkillID, SubWorld[Npc[nIndex].m_SubWorldIndex].m_dwCurrentTime))
			||
			(!Npc[nIndex].Cost(pISkill->GetSkillCostType(), pISkill->GetSkillCost(&Npc[nIndex]), TRUE))
			)
		{
			Npc[nIndex].m_nPeopleIdx = 0;
			Player[CLIENT_PLAYER_INDEX].SetTargetNpc(0);
			return 0;
		}

		if (!nTargetIdx)
		{
			Npc[nIndex].SendCommand(do_skill, Npc[nIndex].m_ActiveSkillID, nX, nY);
			// Send to Server
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, nX, nY);
		}
		else
		{
			if (pISkill->IsTargetOnly())
			{
				int distance = NpcSet.GetDistance(nIndex , nTargetIdx);
				if (distance > pISkill->GetAttackRadius())
				{
					Player[CLIENT_PLAYER_INDEX].SetTargetNpc(nTargetIdx);
					return 0;
				}
			}

			if (nIndex == nTargetIdx && pISkill->GetSkillStyle() == SKILL_SS_Missles) 
				return 0;
			Npc[nIndex].SendCommand(do_skill, Npc[nIndex].m_ActiveSkillID, -1, nTargetIdx);
			// Send to Server		
			SendClientCmdSkill(Npc[nIndex].m_ActiveSkillID, -1, Npc[nTargetIdx].m_dwID);
		}
	}
	Npc[nIndex].m_nPeopleIdx = 0;
	return 1;
}

int KCoreShell::LockSomeoneUseSkill(int nTargetIndex, int nSkillID)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;
	
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex == nIndex)
		return 0;

	if (Npc[nIndex].IsCanInput())
	{
		int nIdx = 0;
		
		nIdx = Npc[nIndex].m_SkillList.FindSame(nSkillID);
		g_DebugLog("[skill]Active");

		Npc[nIndex].SetActiveSkill(nIdx);
	}
	else
	{
		g_DebugLog("[skill]return");
		return 0;
	}

	int nRelation = NpcSet.GetRelation(nIndex, nTargetIndex);
	if (nRelation == relation_enemy)
	{
		Npc[nIndex].m_nPeopleIdx = nTargetIndex;
		return 1;
	}

	return 0;
}

int KCoreShell::LockSomeoneAction(int nTargetIndex)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;
	
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex == nIndex)
		return 0;
	if (nTargetIndex <= 0 || nTargetIndex >= MAX_NPC)	
	{
		Npc[nIndex].m_nPeopleIdx = 0;
		return 1;
	}

	int nRelation = NpcSet.GetRelation(nIndex, nTargetIndex);
	if (nRelation != relation_enemy)
	{
		Npc[nIndex].m_nPeopleIdx = nTargetIndex;
		return 1;
	}

	return 0;
}

int KCoreShell::LockObjectAction(int nTargetIndex)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return 0;
	
	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	if (nTargetIndex <= 0)	
		Npc[nIndex].m_nObjectIdx = 0;
	else
		Npc[nIndex].m_nObjectIdx = nTargetIndex;

	return 1;
}

void KCoreShell::GotoWhere(int x, int y, int mode)
{
	if (mode < 0 || mode > 2)
		return;

	if (Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
	{
		int bRun = false;

		if ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) ||
			mode == 2)
			bRun = true;

		int nX = x;
		int nY = y;
		int nZ = 0;
		g_ScenePlace.ViewPortCoordToSpaceCoord(nX, nY, nZ);
		int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

		if (!bRun)
		{
			Npc[nIndex].SendCommand(do_walk, nX, nY);
			// Send to Server
			SendClientCmdWalk(nX, nY);
		}
		else
		{
			Npc[nIndex].SendCommand(do_run, nX, nY);
			// Send to Server
			SendClientCmdRun(nX, nY);
		}
		Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
	}
}

void KCoreShell::Goto(int nDir, int mode)
{
	if (nDir < 0 || nDir > 63)
		return;

	if (mode < 0 || mode > 2)
		return;

	int bRun = false;

	if ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) ||
		mode == 2)
		bRun = true;

	int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

	int nSpeed;
	if (bRun)
		nSpeed = Npc[nIndex].m_CurrentRunSpeed;
	else
		nSpeed = Npc[nIndex].m_CurrentWalkSpeed;

	Player[CLIENT_PLAYER_INDEX].Walk(nDir, nSpeed);

	Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
}

void KCoreShell::Turn(int nDir)
{
	if (nDir < 0 || nDir > 3)
		return;

	if (nDir == 0)
		Player[CLIENT_PLAYER_INDEX].TurnLeft();
	else if (nDir == 1)
		Player[CLIENT_PLAYER_INDEX].TurnRight();
	else
		Player[CLIENT_PLAYER_INDEX].TurnBack();
}

int KCoreShell::ThrowAwayItem()
{
	if(Player[CLIENT_PLAYER_INDEX].m_sExtAuto.uUnFightTime)
	{
	int nPlayerIdx = CLIENT_PLAYER_INDEX;
	int nIdx = Player[nPlayerIdx].m_ItemList.Hand();
	if(nIdx > 0)
	{
		int nExist = -1;
		UINT uID = Item[nIdx].GetID();
		for (std::map<int,ExtAutoObjTime>::iterator it = Player[nPlayerIdx].m_mAutoIDObj.begin();
		it != Player[nPlayerIdx].m_mAutoIDObj.end();++it)
		{
			ExtAutoObjTime& s = it->second;
			if(s.bItem && uID == s.nID)
			{
				nExist = it->first;
				break;
			}
		}
		UINT uCurTime = timeGetTime();
		if(nExist < 0)
		{
			ExtAutoObjTime s;
			s.nTotalTime = uCurTime;
			s.nChecked = 3;
			s.nPickTime = uCurTime + 120;
			s.nID = uID;
			s.bItem = 1;
			Player[nPlayerIdx].m_mAutoIDObj[Player[nPlayerIdx].m_sExtAuto.umObjIncId++] = s;
		}
		else
		{
			ExtAutoObjTime& s = Player[nPlayerIdx].m_mAutoIDObj[nExist];
			s.nTotalTime = uCurTime;
			s.nChecked = 3;
		}
	}}
	return Player[CLIENT_PLAYER_INDEX].ThrowAwayItem();
}

int KCoreShell::GetNPCRelation(int nIndex)
{
	return NpcSet.GetRelation(Player[CLIENT_PLAYER_INDEX].m_nIndex, nIndex);
}

void KCoreShell::DrawGameObj(unsigned int uObjGenre, unsigned int uId, int x, int y, int Width, int Height, int nParam)
{
	if (g_pRepresent)
		CoreDrawGameObj(uObjGenre, uId, x, y, Width, Height, nParam);
}

#include "../../Represent/iRepresent/iRepresentshell.h"

#ifndef _SERVER
// ============== slow-STRETCH probe (active only when PaintLog=1) ==============
// [SPIKE]/[PDET] only catch a single pass >= 25ms. A slow STRETCH (paint fps
// sagging to 40-55, or logic ticks stretching for a few seconds) is invisible
// to them. This aggregates PER SECOND and logs only abnormal seconds, plus a
// [TICK] line for any single logic tick >= 40ms split into its 3 phases
// (net = MessageLoop, world = MainLoop, scene = ScenePlace Breathe which
// contains the switch-scene wait and Preprocess).
static DWORD s_ProbeSec = 0;
static int   s_SecPaint = 0;
static int   s_SecTick = 0;
static DWORD s_SecTickSum = 0, s_SecTickMax = 0;
static DWORD s_SecNetSum = 0, s_SecNetMax = 0;
static DWORD s_SecWorldSum = 0, s_SecWorldMax = 0;
static DWORD s_SecSceneSum = 0, s_SecSceneMax = 0;
static DWORD s_SecPaintSum = 0, s_SecPaintMax = 0;

static void CoreProbeRollSecond(DWORD dwNow)
{
	DWORD dwSec = dwNow / 1000;
	if (s_ProbeSec == 0)
		s_ProbeSec = dwSec;
	if (dwSec == s_ProbeSec)
		return;
	// abnormal second: painted below ~55fps, starved ticks, or heavy ticking.
	// s_SecPaint > 0 filters loading screens / minimized window noise.
	if (s_SecPaint > 0 &&
		(s_SecPaint < 55 || s_SecTick < 16 || s_SecTickMax >= 30 || s_SecTickSum >= 250 ||
		 s_SecPaintSum >= 300))
	{
		FILE* pLog = fopen("jx_paint.log", "a");
		if (pLog)
		{
			// Gia thuc cua khau khoa surface: so cap trong giay vua roi va tong thoi gian (us).
			unsigned int	nLockCnt = 0, nLockUs = 0;
			g_GetCanvasLockStats(&nLockCnt, &nLockUs, 1);
			fprintf(pLog, "[SEC] pid=%u t=%u painted=%d draw=%u/%u lock=%u/%uus ticks=%d ticksum=%u tickmax=%u net=%u/%u world=%u/%u scene=%u/%u\n",
				GetCurrentProcessId(), s_ProbeSec * 1000, s_SecPaint, s_SecPaintSum, s_SecPaintMax,
				nLockCnt, nLockUs, s_SecTick, s_SecTickSum, s_SecTickMax,
				s_SecNetSum, s_SecNetMax, s_SecWorldSum, s_SecWorldMax, s_SecSceneSum, s_SecSceneMax);
			fclose(pLog);
		}
	}
	// Reset bo dem khoa surface MOI GIAY (khong chi khi ghi log): neu khong, giay
	// binh thuong khong ghi se cong don sang giay sau => dong log vo ly kieu
	// lock=13448702/2097571us (2,1 giay chi phi trong mot giay).
	g_GetCanvasLockStats(NULL, NULL, 1);
	s_ProbeSec = dwSec;
	s_SecPaint = 0;
	s_SecPaintSum = 0; s_SecPaintMax = 0;
	s_SecTick = 0;
	s_SecTickSum = 0; s_SecTickMax = 0;
	s_SecNetSum = 0; s_SecNetMax = 0;
	s_SecWorldSum = 0; s_SecWorldMax = 0;
	s_SecSceneSum = 0; s_SecSceneMax = 0;
}

static void CoreProbeTick(DWORD dwStart, DWORD dwNet, DWORD dwWorld, DWORD dwScene)
{
	DWORD dwTotal = dwNet + dwWorld + dwScene;
	s_SecTick++;
	s_SecTickSum += dwTotal;
	if (dwTotal > s_SecTickMax) s_SecTickMax = dwTotal;
	s_SecNetSum += dwNet;     if (dwNet > s_SecNetMax) s_SecNetMax = dwNet;
	s_SecWorldSum += dwWorld; if (dwWorld > s_SecWorldMax) s_SecWorldMax = dwWorld;
	s_SecSceneSum += dwScene; if (dwScene > s_SecSceneMax) s_SecSceneMax = dwScene;
	if (dwTotal >= 40)
	{
		FILE* pLog = fopen("jx_paint.log", "a");
		if (pLog)
		{
			fprintf(pLog, "[TICK] pid=%u t=%u total=%u net=%u world=%u scene=%u\n",
				GetCurrentProcessId(), dwStart, dwTotal, dwNet, dwWorld, dwScene);
			fclose(pLog);
		}
	}
	CoreProbeRollSecond(dwStart + dwTotal);
}
#endif

void KCoreShell::DrawGameSpace()
{
#ifndef _SERVER
	if (g_nCorePaintLog > 0)
	{
		DWORD dwDrawT0 = timeGetTime();
		s_SecPaint++;
		if (g_pRepresent)
		{
			g_ScenePlace.Paint();
			Player[CLIENT_PLAYER_INDEX].DrawSelectInfo();
		}
		DWORD dwDrawMs = timeGetTime() - dwDrawT0;
		s_SecPaintSum += dwDrawMs;
		if (dwDrawMs > s_SecPaintMax) s_SecPaintMax = dwDrawMs;
		CoreProbeRollSecond(dwDrawT0 + dwDrawMs);
		return;
	}
#endif
	if (g_pRepresent)
	{
		g_ScenePlace.Paint();
		//SubWorld[0].Paint(); //xem obs debug
		Player[CLIENT_PLAYER_INDEX].DrawSelectInfo();
	}
}

void KCoreShell::SetRepresentShell(struct iRepresentShell* pRepresent)
{
	g_pRepresent = pRepresent;
	g_ScenePlace.RepresentShellReset();
	if (g_pAdjustColorTab && g_ulAdjustColorCount && g_pRepresent)
		g_pRepresent->SetAdjustColorList(g_pAdjustColorTab, g_ulAdjustColorCount);
}

void KCoreShell::SetMusicInterface(void* pMusicInterface)
{
	g_pMusic = (KMusic*)pMusicInterface;
	Option.SetMusicVolume(Option.GetMusicVolume());
}

int KCoreShell::Breathe()
{
#ifndef _SERVER
	if (g_nCorePaintLog > 0)
	{
		DWORD dwTickT0 = timeGetTime();
		g_SubWorldSet.MessageLoop();
		DWORD dwTickT1 = timeGetTime();
		g_SubWorldSet.MainLoop();
		TG_XaFuTick();	// [TaskGuide] dan duong den Xa Phu (chi chay khi dang bat)
		DWORD dwTickT2 = timeGetTime();
		g_ScenePlace.Breathe();
		CoreProbeTick(dwTickT0, dwTickT1 - dwTickT0, dwTickT2 - dwTickT1, timeGetTime() - dwTickT2);
		return true;
	}
#endif
	g_SubWorldSet.MessageLoop();
	g_SubWorldSet.MainLoop();
	TG_XaFuTick();	// [TaskGuide] dan duong den Xa Phu (chi chay khi dang bat)
	g_ScenePlace.Breathe();
	return true;
}

int KCoreShell::FindSkillInfo(int nType, int nIndex)
{
	if(nType == 0)
		return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetNextSkillState(nIndex);
	else if(nType == 1)
		return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetNextSkillFight(nIndex);
	else if(nType == 2)
		return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetNextSkillAura(nIndex);
	else if(nType == 3)
		return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetNextAllSkill(nIndex);
	return 0;
}

void KCoreShell::GetSkillName(int nSkillId, char* szSkillName)
{
	if (nSkillId)
	{
		ISkill * pISkill = g_SkillManager.GetSkill(nSkillId, 1);
		if(pISkill) //fix by phong kiÒu
			strcpy(szSkillName, pISkill->GetSkillName());
	}
}

BOOL KCoreShell::GetAutoFlag()
{
	return Player[CLIENT_PLAYER_INDEX].GetAutoFlag();
	return TRUE;
}

BOOL KCoreShell::GetFightFlag()
{
	return Player[CLIENT_PLAYER_INDEX].GetFightFlag();
}

void KCoreShell::SetMoveMap(int nType, int nPos, int nValue)
{
	switch(nType)
	{
		case GAUTO_AUTO_MOVEMPSID: //to¹ ®é train
			if (nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL)
				//Player[CLIENT_PLAYER_INDEX].m_MoveMps[nPos][0] = nValue;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_MoveMpsTrain[nPos][0] = nValue;
			break;
		case GAUTO_AUTO_MOVEMPSX:
			if (nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL)
				//Player[CLIENT_PLAYER_INDEX].m_MoveMps[nPos][1] = nValue;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_MoveMpsTrain[nPos][1] = nValue;
			break;
		case GAUTO_AUTO_MOVEMPSY:
			if (nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL)
				//Player[CLIENT_PLAYER_INDEX].m_MoveMps[nPos][2] = nValue;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_MoveMpsTrain[nPos][2] = nValue;
			break;
		case GAUTO_AUTO_PT_PLAYERTEAM://qu¶n lý tæ ®éi
			{
				if(nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL &&  nValue)
				{
					char * nPlayerName = (char*)nValue;
					strcpy(Player[CLIENT_PLAYER_INDEX].m_AutoPT_PlayerList[nPos], nPlayerName);
					strcpy(Player[CLIENT_PLAYER_INDEX].m_cTeam.m_AutoPT_PlayerList[nPos], nPlayerName);
					strcpy(Player[CLIENT_PLAYER_INDEX].m_cAuto.m_AutoPT_PlayerList[nPos], nPlayerName);
				}
				break;
			}
		case GAUTO_AUTO_BLACK_ITEM:
			{
				if(nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL &&  nValue)
				{
					char * nItemName = (char*)nValue;
					strcpy(Player[CLIENT_PLAYER_INDEX].m_cAuto.m_BlackItemList[nPos], nItemName);
				}
				break;
			}
		case GAUTO_AUTO_MOVETPSID: //to¹ ®é ra b·i
			if (nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL)
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_MoveMpsMap[nPos][0] = nValue;
			break;
		case GAUTO_AUTO_MOVETPSX:
			if (nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL)
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_MoveMpsMap[nPos][1] = nValue;
			break;
		case GAUTO_AUTO_MOVETPSY:
			if (nPos >= 0 && nPos < defMAX_AUTO_MOVEMPSL)
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_MoveMpsMap[nPos][2] = nValue;
			break;
	}
}

void KCoreShell::SetSortItem(int nType, int nValue, int nPos)
{
	int nIndex = (int)g_MagicDesc.String2MagicID((char*)nType);
	if (nIndex >= magic_skill_begin && nIndex < magic_normal_end)
	{
		Player[CLIENT_PLAYER_INDEX].m_FilterMagic[nPos][0] = nIndex;
		Player[CLIENT_PLAYER_INDEX].m_FilterMagic[nPos][1] = nValue;
	}
	else
	{
		Player[CLIENT_PLAYER_INDEX].m_FilterMagic[nPos][0] = 0;
		Player[CLIENT_PLAYER_INDEX].m_FilterMagic[nPos][1] = 0;
	}
}

void KCoreShell::FkAutoSetFillterMagic(int nType, int nValue, int nPos)//fkauto
{
	int nIndex = (int)g_MagicDesc.String2MagicID((char*)nType);
	if (nIndex >= magic_skill_begin && nIndex < magic_normal_end)
	{
		Player[CLIENT_PLAYER_INDEX].m_cAuto.m_FilterMagic[nPos][0] = nIndex;
		Player[CLIENT_PLAYER_INDEX].m_cAuto.m_FilterMagic[nPos][1] = nValue;
	}
	else
	{
		Player[CLIENT_PLAYER_INDEX].m_cAuto.m_FilterMagic[nPos][0] = 0;
		Player[CLIENT_PLAYER_INDEX].m_cAuto.m_FilterMagic[nPos][1] = 0;
	}
}

void KCoreShell::SetActiveAutoPlay(BOOL nActive)
{
	/*Player[CLIENT_PLAYER_INDEX].SendInfoAuto();
	if (nActive)
		SendClientCmdAutoPlay(1,1);
	else
		SendClientCmdAutoPlay(1,0);	*/
}

int KCoreShell::GetProtocolSize(BYTE byProtocol)
{
	if (byProtocol <= s2c_clientbegin || byProtocol >= s2c_end)
		return -1;
	return g_nProtocolSize[byProtocol - s2c_clientbegin - 1];
}

#ifdef SWORDONLINE_SHOW_DBUG_INFO
extern int		g_bShowObstacle;
extern bool		g_bShowGameInfo;	
#endif
int KCoreShell::Debug(unsigned int uDataId, unsigned int uParam, int nParam)
{
#ifdef SWORDONLINE_SHOW_DBUG_INFO
	switch(uDataId)
	{
	case DEBUG_SHOWINFO:
		Player[CLIENT_PLAYER_INDEX].m_DebugMode = !Player[CLIENT_PLAYER_INDEX].m_DebugMode;
		g_bShowGameInfo = !g_bShowGameInfo;
		break;
	case DEBUG_SHOWOBSTACLE:
		g_bShowObstacle = !g_bShowObstacle;
		break;
	}
#endif
	return 0;
}

DWORD KCoreShell::GetPing()
{
	return g_SubWorldSet.GetPing();
}

//void KCoreShell::SendPing()
//{
//	SendClientCmdPing();
//}

void KCoreShell::SetRepresentAreaSize(int nWidth, int nHeight)
{
	g_ScenePlace.SetRepresentAreaSize(nWidth, nHeight);
}

void KCoreShell::SetClient(LPVOID pClient)
{
	g_SetClient(pClient);
}

void KCoreShell::SendNewDataToServer(void* pData, int nLength)
{
	if (g_pClient)
		g_pClient->SendPackToServer(pData, nLength);
}

int	KCoreShell::SceneMapOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uOper)
	{
	case GSMOI_SCENE_TIME_INFO:
		if (uParam)
		{
			KUiSceneTimeInfo* pInfo = (KUiSceneTimeInfo*)uParam;
			g_ScenePlace.GetSceneNameAndFocus(pInfo->szSceneName, pInfo->nSceneId,
				pInfo->nScenePos0, pInfo->nScenePos1);
			pInfo->nGameSpaceTime = (SubWorld[0].m_dwCurrentTime / 100) % 1440;
		}
		break;
	case GSMOI_SCENE_TIME_INFO_OFTEN:
		if (uParam)
		{
			KUiSceneTimeInfoOften* pInfo = (KUiSceneTimeInfoOften*)uParam;
			g_ScenePlace.GetSceneNameAndFocusOften(pInfo->szSceneName, pInfo->nSceneId,
				pInfo->nScenePos0, pInfo->nScenePos1);
			pInfo->nGameSpaceTime = (SubWorld[0].m_dwCurrentTime / 100) % 1440;
		}
		break;
	case GSMOI_SCENE_MAP_INFO:
		nRet = g_ScenePlace.GetMapInfo((KSceneMapInfo*)uParam);
		break;
	case GSMOI_IS_SCENE_MAP_SHOWING:
		g_ScenePlace.SetMapParam(uParam, nParam);
		break;
	case GSMOI_PAINT_SCENE_MAP:
		g_ScenePlace.PaintMap(uParam, nParam);
		break;
	case GSMOI_SCENE_MAP_FOCUS_OFFSET:
		g_ScenePlace.SetMapFocusPositionOffset((int)uParam, nParam);
		break;
	case GSMOI_SCENE_FOLLOW_WITH_MAP:	
		g_ScenePlace.FollowMapMove(nParam);
		break;
	case GSMOI_IS_SCENE_DIRECT_MAP:
		nRet = g_ScenePlace.OnDirectMap((int)uParam, nParam);
		break;
	case GSMOI_IS_SCENE_DO_DIRECT_MAP:
		g_ScenePlace.DoDirectMap((int)uParam, nParam);
		break;
	case GSMOI_SCENE_MAP_FLAG_ON_TARGET:
		g_ScenePlace.FlagOnTarget(uParam, nParam);
		break;
	case GSMOI_IS_SCENE_MAP_FLAGIMG:
		g_ScenePlace.SetFlagImage((char*)uParam, nParam);
		break;
	case GSMOI_SCENE_MAP_REMOVE_FLAG:
		g_ScenePlace.RemoveFlag();
		break;
	case GSMOI_SCENE_MAP_GET_FLAGPOS:
		nRet = g_ScenePlace.GetCurFlagPos(uParam, nParam);
		break;
	case GSMOI_SCENE_MAP_TG_COORD:
		g_ScenePlace.FlagOnCoord(uParam, nParam);
		break;
	}
	return nRet;
}

int	KCoreShell::TongOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
	// ==== JX2 port: cua so bang hoi kieu JX2 ====
	if (uOper == GTOI_TONG_JX2_VIEW)
	{
		if (Player[CLIENT_PLAYER_INDEX].m_cTong.GetTongNameID() == 0 &&
			uParam != defTONG_JX2_PAGE_TONGLIST && uParam != defTONG_JX2_PAGE_OTHERZM)
			return 0;	// chua vao bang -> chi xem danh sach bang / chieu mo bang khac
		Player[CLIENT_PLAYER_INDEX].m_cTong.JX2_RequestView((int)uParam, nParam);
		return 1;
	}
	if (uOper == GTOI_TONG_JX2_OP && uParam)
	{
		KUiTongJX2Op* pOp = (KUiTongJX2Op*)uParam;
		Player[CLIENT_PLAYER_INDEX].m_cTong.JX2_SendOp(pOp->nOp, pOp->dwTarget,
			pOp->nParam1, pOp->nParam2, pOp->szText);
		return 1;
	}
	int nRet = 0;
	switch(uOper)
	{
	case GTOI_TONG_CREATE:		
		Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyCreateTong(nParam, (char *)uParam);
		break;
	case GTOI_TONG_IS_RECRUIT:		
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
			nRet = Npc[pItem->nIndex].m_nTongFlag;
		}
		break;
	case GTOI_TONG_GET_RECRUIT:
		// Co tuyen nguoi la thuoc tinh cua BANG cua chinh minh, khong can
		// tham so. Truoc day case nay boc trong if (uParam) nen cua so bang
		// hoi JX2 (goi voi uParam = 0) LUON nhan 0 = "dang dong tuyen":
		// nhan nut bao sai va khong bao gio dong lai duoc.
		nRet = Player[CLIENT_PLAYER_INDEX].m_cTong.GetRecruit();
		break;
	case GTOI_TONG_RECRUIT:     
		if (uParam)
		{
			Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeRecruit(1);
			//Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeLevel(10);
		}
		else
		{
			Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeRecruit(0);
			//Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeLevel(9);
		}
		break;
	case GTOI_TONG_ACTION:         
		{
    		KTongOperationParam *Oper = (KTongOperationParam *)uParam;
		    KTongMemberItem *TargetInfo = (KTongMemberItem *)nParam;
		    KUiPlayerItem TargetPlayer;
		    int nKind;

		    switch(Oper->eOper)
		    {
		    case TONG_ACTION_DISMISS:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyKick(Oper->nData[0], Oper->nData[1], TargetInfo->Name);
    			break;
		    case TONG_ACTION_ASSIGN:
    			Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInstate(Oper->nData[0], Oper->nData[1], Oper->nData[2], Oper->nData[3], TargetInfo->Name);
			    break;
		    case TONG_ACTION_DEMISE:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyChangeMaster(Oper->nData[0], Oper->nData[1], TargetInfo->Name);
    			break;
		    case TONG_ACTION_LEAVE:
    			Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyLeave();
    			break;
		    case TONG_ACTION_RECRUIT:
    			break;
		    case TONG_ACTION_APPLY:
				if(!FindSpecialNPC(TargetInfo->Name, &TargetPlayer, nKind))
				{
    				break;
				}
    			Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyAddTong(TargetPlayer.uId);
			    break;
			case TONG_ACTION_CHANGE_TITLE:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyChangeTitle(Oper->nData[0], Oper->nData[1], TargetInfo->Name, Oper->szPassword);
				break;
			case TONG_ACTION_CHANGE_MALE_TITLE:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyChangeSexTitle(Oper->szPassword, 0);
				break;
			case TONG_ACTION_CHANGE_FEMALE_TITLE:
				{
					Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyChangeSexTitle(Oper->szPassword, 1);
					g_DebugLog("gia tri title nu truyen len: %s", Oper->szPassword);
				}
   				break;
			case TONG_ACTION_CHANGE_CAMP_JUSTIE:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeCamp(1);	
   				break;
			case TONG_ACTION_CHANGE_CAMP_EVIL:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeCamp(2);
   				break;
			case TONG_ACTION_CHANGE_CAMP_BALANCE:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeCamp(3);
   				break;
			case TONG_ACTION_CHANGE_WAYEDIT:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeWayEdit(Oper->lpszParam);
				break;
			case TONG_ACTION_CHANGE_NEXTTARGET:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyTongChangeNextTarget(Oper->lpszParam);
				break;
		    }
		}
		break;

	case GTOI_TONG_JOIN_REPLY:
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
			Player[CLIENT_PLAYER_INDEX].m_cTong.AcceptMember(pItem->nIndex, g_FileName2Id(pItem->Name), nParam);
		}
		break;

	case GTOI_REQUEST_PLAYER_TONG:	
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
			if (pItem->nIndex == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			{
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_TONG_HEAD,
					pItem->nIndex, 0, 0);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_TONG_HEAD,
					pItem->nIndex, 0, 0);
			}
		}
		break;
	case GTOI_REQUEST_TONG_DATA:     
		if (uParam)
		{
			if (Player[CLIENT_PLAYER_INDEX].m_cTong.CheckIn() == 0)
				break;

			KUiGameObjectWithName	*pObj = (KUiGameObjectWithName*)uParam;
			char	szTongName[32];
			DWORD	dwTongNameID;

			szTongName[0] = 0;
			Player[CLIENT_PLAYER_INDEX].m_cTong.GetTongName(szTongName);
			if (!szTongName[0])
				break;

			dwTongNameID = g_FileName2Id(pObj->szName);
			if (dwTongNameID != Player[CLIENT_PLAYER_INDEX].m_cTong.GetTongNameID())
				break;

			switch (pObj->nData)
			{
			case enumTONG_FIGURE_MASTER:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_TONG_HEAD,
					Player[CLIENT_PLAYER_INDEX].m_nIndex, 0, 0);
				break;
			case enumTONG_FIGURE_DIRECTOR:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_TONG_HEAD,
					Player[CLIENT_PLAYER_INDEX].m_nIndex, 0, 0);
				break;
			case enumTONG_FIGURE_MANAGER:
				if (!Player[CLIENT_PLAYER_INDEX].m_cTong.CanGetManagerInfo(dwTongNameID))
					break;
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_MANAGER,
					dwTongNameID, nParam, defTONG_ONE_PAGE_MAX_NUM);
				break;
			case enumTONG_FIGURE_MEMBER:
				if (!Player[CLIENT_PLAYER_INDEX].m_cTong.CanGetMemberInfo(dwTongNameID))
					break;
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyInfo(
					enumTONG_APPLY_INFO_ID_MEMBER,
					dwTongNameID, nParam, defTONG_ONE_PAGE_MAX_NUM);
				break;
			}
		}
		break;
	case GTOI_TONG_MONEY_ACTION:
		if (uParam)
		{
			KUiPlayerItem	*pItem = (KUiPlayerItem*)uParam;
			switch(nParam)
			{
 			case TONG_ACTION_SAVE:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplySaveMoney(pItem->nData);
				break;
			case TONG_ACTION_GET:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplyGetMoney(pItem->nData);
				break;
			case TONG_ACTION_SND:
				Player[CLIENT_PLAYER_INDEX].m_cTong.ApplySndMoney(pItem->nData);
				break;
			}
		}
		break;
	}
	return nRet;
}

int KCoreShell::TeamOperation(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uOper)
	{
	case TEAM_OI_GD_INFO:		
		if (uParam)
		{
			KUiPlayerTeam* pTeam = (KUiPlayerTeam*)uParam;
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetInfo(pTeam);
		}
		break;
	case TEAM_OI_GD_MEMBER_LIST:
		nRet = g_Team[0].GetMemberInfo((KUiPlayerItem *)uParam, nParam);
		break;
	case TEAM_OI_GD_REFUSE_INVITE_STATUS:
		nRet = Player[CLIENT_PLAYER_INDEX].m_cTeam.GetAutoRefuseState();
		break;
	case TEAM_OI_COLLECT_NEARBY_LIST:
		NpcSet.GetAroundOpenCaptain(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp);
		break;
	case TEAM_OI_APPLY:			
		if (uParam)
			Player[CLIENT_PLAYER_INDEX].ApplyAddTeam(((KUiTeamItem*)uParam)->Leader.nIndex);
		break;
	case TEAM_OI_CREATE:		
		Player[CLIENT_PLAYER_INDEX].ApplyCreateTeam();
		break;
	case TEAM_OI_APPOINT:		
		Player[CLIENT_PLAYER_INDEX].ApplyTeamChangeCaptain(((KUiPlayerItem*)uParam)->uId);		
		break;
	case TEAM_OI_INVITE:		
		if (uParam)
		{
			Player[CLIENT_PLAYER_INDEX].TeamInviteAdd(((KUiPlayerItem*)uParam)->uId);

			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_TEAM_SEND_INVITE, ((KUiPlayerItem*)uParam)->Name);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		break;
	case TEAM_OI_KICK:			
		Player[CLIENT_PLAYER_INDEX].TeamKickMember(((KUiPlayerItem*)uParam)->uId);
		break;
	case TEAM_OI_LEAVE:			
		Player[CLIENT_PLAYER_INDEX].LeaveTeam();
		break;
	case TEAM_OI_CLOSE:			
		Player[CLIENT_PLAYER_INDEX].ApplyTeamOpenClose(nParam);
		break;
	case TEAM_OI_REFUSE_INVITE:		
		Player[CLIENT_PLAYER_INDEX].m_cTeam.SetAutoRefuseInvite(nParam);//0 t¾t tù ®éng tõ chèi lêi mêi//1 bËt tù ®éng tõ chèi lêi mêi
		break;
	case TEAM_OI_APPLY_RESPONSE:		
		if (uParam)
		{
			if (nParam)
			{
				Player[CLIENT_PLAYER_INDEX].AcceptTeamMember(((KUiPlayerItem*)uParam)->uId);
			}
			else
			{
				Player[CLIENT_PLAYER_INDEX].m_cTeam.DeleteOneFromApplyList(((KUiPlayerItem*)uParam)->uId);
				//Player[CLIENT_PLAYER_INDEX].m_cTeam.UpdateInterface();
			}
		}
		break;
	case TEAM_OI_INVITE_RESPONSE:
		if (uParam)
			Player[CLIENT_PLAYER_INDEX].m_cTeam.ReplyInvite(((KUiPlayerItem*)uParam)->nIndex, nParam);
		break;
	case TEAM_OI_GET_NPC_MAP_POS:
		{
			if (uParam && nParam)
			{
				KUiPlayerItem *m_pPlayersList = (KUiPlayerItem*)uParam;
			KUiPlayerPaintTeamMNG* nPainTMG = (KUiPlayerPaintTeamMNG*)nParam;
			Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].PaintTeamMNG(m_pPlayersList, nPainTMG);
			}
		}
		break;
	}
	return nRet;
}

int KCoreShell::GetNPCBAITAN2()
{
	return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan;
}

int KCoreShell::GetNPCBAITAN(int nIndex)
{
	int nID = NpcSet.SearchID(nIndex);
	return Npc[nID].m_BaiTan;
}

int KCoreShell::GetPriceSell(unsigned int uId)
{
	return Item[uId].GetCurPrice();
}

int KCoreShell::GetStallState()
{
	return Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_BaiTan;
}

int KCoreShell::GetGenreItem2(unsigned int uId)
{
	return Item[uId].GetGenre();
}

int KCoreShell::GetPriceSell2(unsigned int uId)
{
	return Item[uId].GetSetPrice();
}

int KCoreShell::GetTypeItem(unsigned int uId)
{
	return Item[uId].GetColorItem();
}

int KCoreShell::GetNatureItem(unsigned int uItemId, unsigned int uGenre /* = CGOG_ITEM*/)
{
	int nIndex = uItemId;
	KItem*	pItem = NULL;
	if (uGenre == CGOG_ITEM)
		return Item[nIndex].GetColorItem();
	else if (uGenre == CGOG_NPCSELLITEM)
	{
		int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];
		if (nBuyIdx != -1)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, uItemId);
			if (nIndex >= 0)
			{
				pItem = BuySell.GetItem(nIndex);
				return pItem->GetColorItem();
			}
		}
	}
	return Item[nIndex].GetColorItem();
}

int KCoreShell::GetGenreItem(unsigned int uItemId, unsigned int uGenre /* = CGOG_ITEM*/)
{
	int nIndex = uItemId;
	KItem*	pItem = NULL;
	if (uGenre == CGOG_ITEM)
		return Item[nIndex].GetGenre();
	else if (uGenre == CGOG_NPCSELLITEM)
	{
		int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];
		if (nBuyIdx != -1)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, uItemId);
			if (nIndex >= 0)
			{
				pItem = BuySell.GetItem(nIndex);
				return pItem->GetGenre();
			}
		}
	}
	return Item[nIndex].GetGenre();
}

int KCoreShell::GetObjAtCountRegionInSuperShop(int nSaleId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[nSaleId];
	if (nBuyIdx == -1)
		return 0;
	if (nBuyIdx >= BuySell.GetHeight())
		return 0;
	int nIndex = 0;
	if(uParam)
	{
		int nPage = 0;
		char* pInfo = (char *)uParam;
		//KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
		if(nParam < BuySell.GetWidth())
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, nParam);
			KItem* pItem = BuySell.GetItem(nIndex);
			if (nIndex >= 0 && pItem)
			{
				strcpy(pInfo, pItem->GetImageName());
				//pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
				//pInfo->Obj.uId = nParam;
				//pInfo->Region.h = 0;
				//pInfo->Region.v = 0;
				//pInfo->Region.Width = pItem->GetWidth();
				//pInfo->Region.Height = pItem->GetHeight();
				//pInfo->nContainer = nPage;
				return 1;
			}
		}
	}
	return nRet;
}

int KCoreShell::GetDataSuperShop(int nSaleId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[nSaleId];
	if (nBuyIdx == -1)
		return 0;
	if (nBuyIdx >= BuySell.GetHeight())
		return 0;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop = nSaleId;
	int nIndex = 0;
	int nCount = 0;
	if (uParam)
	{
		int nPage = 0;
		KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItem(nIndex);
			
			if (nIndex >= 0 && pItem)
			{
				pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
				pInfo->Obj.uId = i;
				pInfo->Region.h = 0;
				pInfo->Region.v = 0;
				pInfo->Region.Width = pItem->GetWidth();
				pInfo->Region.Height = pItem->GetHeight();
				pInfo->nContainer = nPage;
				nCount++;
				pInfo++;
				if (nCount %15 == 0)
					nPage ++;
			}
		}			
		nRet = nPage;
	}
	else
	{
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItem(nIndex);
			
			if (nIndex >= 0 && pItem)
			{
				nCount++;
			}
		}
		nRet = nCount;
	}
	return nRet;
}

int KCoreShell::GetItemIdxNpcShop(char* szItemName)//fkauto
{
	int nIndex = -1;
	int curShop = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop;
	int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[curShop];
	if (nBuyIdx == -1)
		return -1;
	if (nBuyIdx >= BuySell.GetHeight())
		return -1;
	for (int i = 0; i < BuySell.GetWidth(); i++)
	{
		nIndex = BuySell.GetItemIndex(nBuyIdx, i);
		KItem* pItem = BuySell.GetItem(nIndex);
		if (nIndex >= 0 && pItem)
		{
			if(strcmp(pItem->GetName(), szItemName) == 0)
				return i;
		}
	}
	return -1;
}

int KCoreShell::GetDataDynamicShop(int nSaleId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	int	nBuyIdx = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[nSaleId];
	if (nBuyIdx == -1)
		return 0;
	if (nBuyIdx >= BuySell.GetHeight())
		return 0;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop = nSaleId;
	int nIndex = 0;
	int nCount = 0;
	if (uParam)
	{
		int nPage = 0;
		KUiObjAtContRegion* pInfo = (KUiObjAtContRegion *)uParam;
		if (!BuySell.m_pSShopRoom)
			return 0;
		BuySell.m_pSShopRoom->Clear();
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItem(nIndex);
			
			if (nIndex >= 0 && pItem)
			{
				pInfo->Obj.uGenre = CGOG_NPCSELLITEM;
				pInfo->Obj.uId = i;
				POINT	Pos;
				if (BuySell.m_pSShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos))
				{
					BuySell.m_pSShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
				}
				else
				{
					nPage++;
					BuySell.m_pSShopRoom->Clear();
					BuySell.m_pSShopRoom->FindRoom(pItem->GetWidth(), pItem->GetHeight(), &Pos);
					BuySell.m_pSShopRoom->PlaceItem(Pos.x, Pos.y, nIndex + 1, pItem->GetWidth(), pItem->GetHeight());
				}
				pInfo->Region.h = Pos.x;
				pInfo->Region.v = Pos.y;
				pInfo->Region.Width = pItem->GetWidth();
				pInfo->Region.Height = pItem->GetHeight();
				pInfo->nContainer = nPage;
				nCount++;
				pInfo++;
			}
		}			
		nRet = nPage;
	}
	else
	{
		for (int i = 0; i < BuySell.GetWidth(); i++)
		{
			nIndex = BuySell.GetItemIndex(nBuyIdx, i);
			KItem* pItem = BuySell.GetItem(nIndex);
			
			if (nIndex >= 0 && pItem)
			{
				nCount++;
			}
		}
		nRet = nCount;
	}
	return nRet;
}

int KCoreShell::GetOwnValue(int nMoneyUnit)
{
	int nRet = 0;
	switch (nMoneyUnit)
	{
		case moneyunit_money://tien van
			nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetMoney(room_equipment);
			break;
		case moneyunit_extpoint://tien xu
			//nRet = Player[CLIENT_PLAYER_INDEX].m_ItemList.CountCommonItem(0, 4, Def_ID_XU_TIENDONG);
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_XU);
			break;
		case moneyunit_fuyuan://phuc duyen
			nRet = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].nFuYuan;
			break;
		case moneyunit_repute://danh vong
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE);
			break;
		case moneyunit_accum: //tong kim
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_ACCUM);
			break;
		case moneyunit_honor: //lien dau
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_HONOR);
			break;
		case moneyunit_respect: //c«ng tr¹ng uy danh
			nRet = Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(TASKVALUE_STATTASK_RESPECT);
			break;
		default:
			break;
	}
	return nRet;
}

int KCoreShell::AutoPlayOperation(unsigned int uOper, unsigned int uParam, int nParam)//fkauto
{
	int nRet = 0, i = 0;
	switch(uOper)
	{	
		case AUTOPLAY_OI_ACTIVE:	//-----------chiÕn ®Êu--------------
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.InitFkAuto(uParam);
			}
			break;
		case AUTOPLAY_OI_PAUSE:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.PauseFkAuto(uParam);
			}
			break;
		case AUTOPLAY_OI_FIGHT_B: //chiÕn ®Êu
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFightCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_FIGHT_V:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nFightRange = uParam;
			}
			break;
		case AUTOPLAY_OI_FIGHT_S:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFightSelect = uParam;
			}
			break;
		case AUTOPLAY_OI_DISTANCE_B: //tiÕp cËn
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFightDistance = uParam;
			}
			break;
		case AUTOPLAY_OI_DISTANCE_V:
			{
				int nLeftSkillID = Player[CLIENT_PLAYER_INDEX].GetLeftSkill();
				int nLevel = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_SkillList.GetCurrentLevel(nLeftSkillID);
				//GetAttackRadius
				ISkill* pISkill = g_SkillManager.GetSkill(nLeftSkillID, nLevel);
				if(!pISkill)
					break;
				int nRange = pISkill->GetAttackRadius()*0.75;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nFightDistance = min(uParam, nRange);
			}
			break;
		case AUTOPLAY_OI_DISTANCE_S:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFightDistanceSelect = uParam;
			}
			break;
		case AUTOPLAY_OI_SELFDEF_B://tù vÖ
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFightSelfDef = uParam;
			}
			break;
		case AUTOPLAY_OI_SELFDEF_V:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nFightSelfDefValue = uParam;
			}
			break;
		case AUTOPLAY_OI_SELFDEF_S:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFightSelfDefSelect = uParam;
			}
			break;
		case AUTOPLAY_OI_SUPPORT_SKILL:
			{
				switch(uParam)
				{
					case 1:
						Player[CLIENT_PLAYER_INDEX].m_cAuto.m_ArrayStateSkill[0] = nParam;
						break;
					case 2:
						Player[CLIENT_PLAYER_INDEX].m_cAuto.m_ArrayStateSkill[1] = nParam;
						break;
					case 3:
						Player[CLIENT_PLAYER_INDEX].m_cAuto.m_ArrayStateSkill[2] = nParam;
						break;
					case 4:
						Player[CLIENT_PLAYER_INDEX].m_cAuto.m_LeftSkillID = nParam;
						Player[CLIENT_PLAYER_INDEX].SetLeftSkill(nParam);//set chiªu tay tr¸i
						break;
					case 5:
						Player[CLIENT_PLAYER_INDEX].m_cAuto.m_RightSkillID = nParam;
						Player[CLIENT_PLAYER_INDEX].SetRightSkill(nParam);//set chiªu tay ph¶i
						break;
					case 6:
						Player[CLIENT_PLAYER_INDEX].m_cAuto.m_SkillSelfDefID = nParam;
						break;
					case 7:
						Player[CLIENT_PLAYER_INDEX].m_cAuto.m_SkillBossID = nParam;
						break;						
				}
			}
			break;
		case AUTOPLAY_OI_SHORTKEY:
			{
				if(uParam == 1)
				{
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_ShortcutKeySelect1 = nParam;
				}
				else
				{
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_ShortcutKeySelect2 = nParam;
				}
			}
			break;
		case AUTOPLAY_OI_SHORTCUT_EDIT:
			{
				if(uParam == 1)
				{
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nShortcutEditBox1 = nParam;
				}
				else
				{
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nShortcutEditBox2 = nParam;
				}
			}
			break;
		case AUTOPLAY_OI_SHORTCUT:
			{
				if(uParam == 1)
				{
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_ShortcutOptSelect1 = nParam;
				}
				else
				{
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_ShortcutOptSelect2 = nParam;
				}
			}
			break;
		case AUTOPLAY_OI_USKILL_RIGHT_B:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bSkillRightCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_NR_DO_SKILL_B:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bNRDoSkillCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_HP_B://b¬m sl //-----------phôc håi---------------
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bReHPCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_HP_1:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nReHPEditBox1 = uParam;
			}
			break;
		case AUTOPLAY_OI_HP_2:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nReHPEditBox2 = uParam;
			}
			break;
		case AUTOPLAY_OI_HP_3:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nReHPEditBox3 = uParam;
			}
			break;
		case AUTOPLAY_OI_MP_B://b¬m nl
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bReMPCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_MP_1:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nReMPEditBox1 = uParam;
			}
			break;
		case AUTOPLAY_OI_MP_2:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nReMPEditBox2 = uParam;
			}
			break;
		case AUTOPLAY_OI_MP_3:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nReMPEditBox3 = uParam;
			}
			break;
		case AUTOPLAY_OI_TP_1://tdp sl <
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTownHPVCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nTownHPVEditBox = nParam;
			}
			break;
		case AUTOPLAY_OI_TP_2://tdp nl <
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTownMPVCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nTownMPVEditBox = nParam;
			}
			break;
		case AUTOPLAY_OI_TP_3://tdp het binh sl
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTownHPNCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_TP_4://tdp het binh nl
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTownMPNCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_TP_5:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTownFCellCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nTownFCellTxtSelect = nParam;
			}
			break;
		case AUTOPLAY_OI_TP_6:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTowMoneyCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nTownMoneyEditBox = nParam * 10000;//®¬n vÞ tÝnh v¹n l­îng
			}
			break;
		case AUTOPLAY_OI_TP_7:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTownAbradeCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nTownAbradeEditBox = nParam;
			}
			break;
		case AUTOPLAY_OI_USEM_FCELL:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bUseHPCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_ANTI_TOXIC:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bUseAntiToxicCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_UX2ITEM:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bUseExpCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_UX2SKILL:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bUseSkillCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_NMBUFF:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bBuffCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nBuffEditBox = nParam;
			}
			break;
		case AUTOPLAY_OI_BUFF_TEAM:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bBuffTeamCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_OPENBAGHP:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bOpenBagHPCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nOpenBagHPEditBox = nParam;
			}
			break;
		case AUTOPLAY_OI_RING_TDP:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_RingTDPCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_1:	//-----------nhÆt ®å---------------
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bAutoPickCheckBox = uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nAutoPickEditBox = nParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_2:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bPickAllCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_3:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bPickSpeCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_4:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bNoneFightCheckBox = uParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_5:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nPickSelTxtSelect = uParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_6:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bPriceCellCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nPriceCellEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_7:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bLevelCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nLevelEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_8:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bRiAmPeCheckBox= uParam;//gi÷ trang søc
			}
			break;
		case AUTOPLAY_OI_OBJ_9:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nRiEditBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nAmPeEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_10:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bSortBagCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nSortBagEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_11:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bNPickBackLCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_OBJ_12:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFillterItemCheckBox = uParam; //läc ®å
			}
		case AUTOPLAY_OI_OBJ_13:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bKeepPurpleCheckBox= uParam; //nhÆt ®å tÝm
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nKeepPurpleEditBox= nParam; // sè dßng
			}
			break;
		case AUTOPLAY_OI_MOVE_1: //-- di chuyÓn
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFollowCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nFollowEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_2:
			{
				strcpy(Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nFollowNameTxt, (char*)uParam);
			}
			break;
		case AUTOPLAY_OI_MOVE_3://quanh ®iÓm
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bLoopPosCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nLoopPosRangEBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_4:
			{
				if(Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bLoopPosCheckBox)
				{
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nXLoopPosTxt = uParam;
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nYLoopPosTxt = nParam;
				}
				else
				{
					int nX = 0; int nY = 0;
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nX, &nY);
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nXLoopPosTxt = nX;
					Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nYLoopPosTxt = nY;
				}
			}
			break;
		case AUTOPLAY_OI_MOVE_5: //theo to¹ ®é
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bConstPosCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_6:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bMoveMapsCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nMoveMapID= nParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_7:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bIRMoveCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_8://®¸nh qu¸i trªn ®­êng ®i
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFMORCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_9:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bDamnMonterCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_10:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bTalkAnnyCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_11:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFlCaptainCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MOVE_12:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bFlAnnyPTCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MAP_1://--b¶n ®å--
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bReturnCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MAP_2:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bSellCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MAP_3:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bRepairCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MAP_4:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bQuickRepairCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MAP_5:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bGetMoneyCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MAP_6:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nGetMoneyEditBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nPwdMoneyEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MAP_7:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bBuyHPCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nBuyHPEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MAP_8: //tªn m¸u
			{
				if(uParam)
					strcpy(Player[CLIENT_PLAYER_INDEX].m_cAuto.m_BuyHPTxtSelect, (char*)uParam);
			}
			break;
		case AUTOPLAY_OI_MAP_9:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bBuyMPCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nBuyMPEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MAP_10://tªn mana
			{
				if(uParam)
					strcpy(Player[CLIENT_PLAYER_INDEX].m_cAuto.m_BuyMPTxtSelect, (char*)uParam);
			}
			break;
		case AUTOPLAY_OI_MAP_11:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bBuyToxicCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nBuyToxicEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MAP_12:
			{
				if(uParam)
					strcpy(Player[CLIENT_PLAYER_INDEX].m_cAuto.m_BuyToxicTxtSelect, (char*)uParam);
			}
			break;
		case AUTOPLAY_OI_MAP_13:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bBuyTownCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nBuyTownEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MAP_14:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bMapRunPosCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nMapTxtId= nParam;
			}
			break;
		case AUTOPLAY_OI_MAP_15:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bKeepMoneyCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nKeepMoneyEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_MAP_16:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bGetFYCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_MAP_17:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bGoFarAwayCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_GoFarAwayTxtSelect= nParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_1://---tæ ®éi---
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bAutoCTeamCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_2: //lu«n lµm nhãm tr­ëng
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bAlwayLeaderCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_3: //tù ®éng PT tÊt c¶
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bPTAllCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_4: //tõ chèi mäi lêi mêi
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bDecAllInviCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cTeam.SetAutoRefuseInvite(uParam);//0 t¾t tù ®éng tõ chèi lêi mêi//1 bËt tù ®éng tõ chèi lêi mêi
			}
			break;
		case AUTOPLAY_OI_TEAM_5:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bLeaveTeamCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_6:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nLeaveTeamEditBox1= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nLeaveTeamEditBox2= nParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_7:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bKickTeamCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_nKickTeamEditBox= nParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_8:
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bSCountTeamCheckBox= uParam;
			}
			break;
		case AUTOPLAY_OI_TEAM_9://PT cïng bang
			{
				Player[CLIENT_PLAYER_INDEX].m_cAuto.m_bPTTongCheckBox= uParam;
				Player[CLIENT_PLAYER_INDEX].m_cTeam.m_bPTTongCheckBox = uParam;
			}
			break;
	}
	return nRet;
}

void KCoreShell::GotoWhereDirect(int x, int y, int mode)
{
	if (Player[CLIENT_PLAYER_INDEX].CheckTrading())
		return;

	if (mode < 0 || mode > 2)
		return;

	if (Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames >= defMAX_PLAYER_SEND_MOVE_FRAME)
	{
		int bRun = false;

		if ((mode == 0 && Player[CLIENT_PLAYER_INDEX].m_RunStatus) ||
			mode == 2)
			bRun = true;

		int nX = x;
		int nY = y;
		int nZ = 0;

		int nIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;

		if (!bRun)
		{
			Npc[nIndex].SendCommand(do_walk, nX, nY);
			// Send to Server
			SendClientCmdWalk(nX, nY);
		}
		else
		{
			Npc[nIndex].SendCommand(do_run, nX, nY);
			// Send to Server
			SendClientCmdRun(nX, nY);
		}
		Player[CLIENT_PLAYER_INDEX].m_nSendMoveFrames = 0;
	}
}
bool CloseToTarget(const FindPathNode& point1, const FindPathNode& point2, int delta)
{
	int distance = sqrt(pow(point2.x - point1.x, 2) + pow(point2.y - point1.y, 2));
	g_DebugLog("Distance %d\n", distance);
	return distance <= delta;
}

#ifndef _SERVER
/*
BOOL KCoreShell::AutoMove()
{
    FindPathNode nextPoint;
    FindPathNode currentPoint;

    int dX, dY;
    INT nCurX, nCurY;
    Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nCurX, &nCurY);
    BOOL nRet = FALSE;
	int delta = 32;
   // if (!GetPaintMode())
      // return nRet;
    if (!Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.empty())
    {
		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoMoveTemp.x == nCurX && Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoMoveTemp.y == nCurY) //in lag point, can not move
        {
            Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].AutoMoveStuckCount += 1;
            if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].AutoMoveStuckCount > 10) {
				srand(static_cast<unsigned>(time(NULL)));
				dX = rand() % 201 - 100; // [-100, 100]
				dY = rand() % 201 - 100;
				GotoWhereDirect(nCurX + dX, nCurY + dY, 0);
                Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].AutoMoveStuckCount = 0;
            }
        }
        else {
            Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoMoveTemp.x = nCurX;
            Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_AutoMoveTemp.y = nCurY;
            Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].AutoMoveStuckCount = 0;
        }
        // Go to the next point in auto move path
        nextPoint = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.front();

        currentPoint.x = nCurX;
        currentPoint.y = nCurY;
        dX = currentPoint.x & 0x1F;
        dY = currentPoint.y & 0x1F;
		//nextPoint.x = nextPoint.x + 0x10 - dX; // Anti-LAG
		//nextPoint.y = nextPoint.y + 0x10 - dY; // Anti-LAG

		if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.size() == 1) { // 1 point to move, the target point
			g_DebugLog("1 target point left, running to %d, %d from Current %d, %d\n", nextPoint.x, nextPoint.y, currentPoint.x, currentPoint.y);
			GotoWhereDirect(nextPoint.x, nextPoint.y, 0);
			if (CloseToTarget(currentPoint, nextPoint, delta))
				Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.erase(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.begin()); //remove all point in path find
			return false;
		}
        if (g_JXPathFinder.GetNextStep(currentPoint, nextPoint, 2) == emKNEXTSTEP_RESULT_SUCCESS) 
        {
            // Go to next point in m_PathFind
            if (Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.empty())
                return FALSE;

            nextPoint = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.front();
            if (CloseToTarget(currentPoint, nextPoint, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentRunSpeed * 7) || (g_JXPathFinder.getStand().x == currentPoint.x && g_JXPathFinder.getStand().y == currentPoint.y)) {
                Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.erase(Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PathFind.begin());
            }
            g_DebugLog("Running to %d, %d from Current %d, %d\n", nextPoint.x, nextPoint.y, currentPoint.x, currentPoint.y);
            GotoWhereDirect(nextPoint.x, nextPoint.y, 0);
        }
        else if (g_JXPathFinder.GetNextStep(currentPoint, nextPoint, 1) == emKNEXTSTEP_RESULT_ARRIVAL)
        {
            GotoWhereDirect(nextPoint.x, nextPoint.y, 0);
            return FALSE; // Still moving
        }
        return TRUE; // Auto moving
    }
    else
        return FALSE; // Nothing in auto move path
}

void KCoreShell::ClearPathFinder() {
	g_ScenePlace.bFlagMode = FALSE;
	g_ScenePlace.bPaintMode = FALSE;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].ResetPathFind();
}*/

#endif