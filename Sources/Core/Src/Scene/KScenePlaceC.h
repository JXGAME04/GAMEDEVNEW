// ***************************************************************************************
// Function: VÏ Backgrounk Map Hoa S¬n
// Copyright : Kingsoft 2002
// Author    : Fong KiÒu
// CreateTime: 2020-11-11
// ---------------------------------------------------------------------------------------
// ***************************************************************************************

#ifndef KSCENEPLACEC_H_HEADER_INCLUDED_C22D00B5
#define KSCENEPLACEC_H_HEADER_INCLUDED_C22D00B5
#include "../../Represent/iRepresent/KRepresentUnit.h"
#include "KScenePlaceRegionC.h"
#include "SceneDataDef.h"
#include "KIpoTree.h"
#include "ScenePlaceMapC.h"

#ifndef _SERVER
#include <KLittleMap.h>
#endif
// »·¾³¹âÊı×éË÷Òı
/*
×Ó£º£¨23£º00£­1£º00£©RGB1
³ó£º£¨1£º00£­3£º00£©RGB1£­RGB2
Òú£º£¨3£º00£­5£º00£©RGB2
Ã®£º£¨5£º00£­7£º00£©RGB2£­RGB3
³½£º£¨7£º00£­9£º00£©RGB3£­RGB4
ËÈ£º£¨9£º00£­11£º00£©RGB4£­RGB5
Îç£º£¨11£º00£­13£º00£©RGB5
Î´£º£¨13£º00£­15£º00£©RGB5£­RGB4
Éê£º£¨15£º00£­17£º00£©RGB4£­RGB6
ÓÏ£º£¨17£º00£­19£º00£©RGB6£­RGB7
Ğç£º£¨19£º00£­21£º00£©RGB7£­RGB1
º¥£º£¨21£º00£­23£º00£©RGB1
*/
#define ENVLIGHT_MIDNIGHT	0	//RGB1£ºÎçÒ¹µÄºÚ°µ
#define ENVLIGHT_DAWN		1	//RGB2£ºÌìÁÁÇ°ÓĞĞ©·¢À¶µÄºÚ°µ
#define ENVLIGHT_MORNING	2	//RGB3£ºÌ«Ñô¸Õ³öÀ´Ê±µÄÁÁ¶È
#define ENVLIGHT_FORENOON	3	//RGB4£ºµØÍ¼µÄÕı³£ÁÁ¶È
#define ENVLIGHT_NOON		4	//RGB5£ºÕıÎçµÄ¸ßÁÁ
#define ENVLIGHT_DUSK		5	//RGB6£ºÌ«ÑôÂäÉ½µÄÍíÏ¼ÁÁ¶È
#define ENVLIGHT_EVENING	6	//RGB7£º°øÍí»ÒÃÉÃÉµÄÁÁ¶È
#define MAX_BACKGROUND_IMAGE 5

class EnvironmentLight
{
private:
	KLColor m_cLight[7];
public:
	EnvironmentLight();
	// ÉèÖÃµÚnIdx¸öÑÕÉ«
	void SetLight(const KLColor &cLight, int nIdx);
	// ÉèÖÃµÚnIdx¸öÑÕÉ«
	void SetLight(BYTE r, BYTE g, BYTE b, int nIdx);
	// ÉèÖÃËùÓĞ7¸öÑÕÉ«
	void SetLight(KLColor *pLight);
	// È¡µÃ¾àÒ»Ìì¿ªÊ¼nMinutes·ÖÖÓÊ±µÄ»·¾³¹âÑÕÉ«
	DWORD GetEnvironmentLight(int nMinutes);
};

//================================================================
//	³¡¾°ÖĞÒ»¸öÇøÓò±»¼ÓÔØÍê±ÏºóµÄ»Øµ÷º¯ÊıµÄº¯ÊıÔ­ĞÍ
//================================================================
//	²ÎÊı: nRegionH, nRegionV ·Ö±ğÎª¼ÓÔØÍê±ÏµÄÇøÓòµÄºáÏòÓë×İÏòË÷Òı
typedef void (*funScenePlaceRegionLoadedCallback)(int nRegionH, int nRegionV);

class KWeather;


#define KMAX_PREV_LOAD_POS_NUM  40
typedef struct _KPrevLoadPosItem
{
    int     m_nNum;
    POINT   m_Pos[KMAX_PREV_LOAD_POS_NUM];

} KPrevLoadPosItem;


#define MAX_PREV_LOAD_FILE_COUNT    1024
#define PREV_LOAD_FILE_MIN_SIZE     (32 * 1024)

typedef struct _KPrevLoadFileNameAndFrameAndFrame
{
    char szName[MAX_RESOURCE_FILE_NAME_LEN];
    int  nFrame;
} KPrevLoadFileNameAndFrame;



//typedef struct _KPrevLoadItem
//{
//    POINT
//
//} KPrevLoadItem;


//##ModelId=3DB8F8B40128
//##Documentation
//## ³¡¾°µÄÒ»¸öµØÍ¼¡£
class KScenePlaceC
{
  public:
	//##ModelId=3DBE3B53008C
	//##Documentation
	//## ¹¹Ôìº¯Êı¡£
	//## º¯ÊıÌåÒ»¶¨°üº¬´ËÓï¾äm_szPlaceRootPath[0] = 0
	KScenePlaceC();

	//##ModelId=3DD17A770383
	//##Documentation
	//## call Terminate
	~KScenePlaceC();

	//##ModelId=3DCAA6A703DB
	//##Documentation
	//## ³õÊ¼»¯
	bool Initialize();

	//##ModelId=3DCD58AC00BC
	//##Documentation
	//## ½áÊø¶ÔÏó¹¦ÄÜ¡£ÊÍ·Å¶ÔÏóµÄÈ«²¿Êı¾İÓë¶¯Ì¬¹¹ÔìµÄ×ÊÔ´¡£
	void Terminate();

	//##ModelId=3DCAA64C01DA
	//##Documentation
	//## ¸ü»»/¼ÓÔØÖ¸¶¨µÄ³¡¾°µØÍ¼¡£
	bool OpenPlace(
		//##Documentation
		//## ³¡¾°µØÍ¼Ë÷Òı
		int nPlaceIndex);

	//##ModelId=3DCAAE3703A6
	//##Documentation
	//## ¹Ø±Õ³¡¾°µØÍ¼¡£
	void ClosePlace();

	//##ModelId=3DCD7F0A0071
	//##Documentation
	//## »æÖÆ³¡¾°
	void Paint();

	//##ModelId=3DBCE7B70358
	//##Documentation
	//## ÉèÖÃ³¡¾°µØÍ¼µÄ½¹µã
	void SetFocusPosition(
		//##Documentation
		//## ³¡¾°½¹µã×ø±êµÄx·ÖÁ¿
		int nX, 
		//##Documentation
		//## ³¡¾°½¹µã×ø±êµÄy·ÖÁ¿
		int nY, 
		//##Documentation
		//## ³¡¾°½¹µã×ø±êµÄz·ÖÁ¿
		int nZ);

	void GetFocusPosition(
		//##Documentation
		//## ³¡¾°½¹µã×ø±êµÄx·ÖÁ¿
		int& nX, 
		//##Documentation
		//## ³¡¾°½¹µã×ø±êµÄy·ÖÁ¿
		int& nY, 
		//##Documentation
		//## ³¡¾°½¹µã×ø±êµÄz·ÖÁ¿
		int& nZ);

//----¹ØÓÚ²»Á¥ÊôÓÚ³¡¾°µØÍ¼µÄ¶ÔÏóÓë³¡¾°¹ØÏµµÄ²Ù×÷----

	//##ModelId=3DCAA6B90196
	//##Documentation
	//## Ìí¼ÓÔËĞĞÊ±¶ÔÏó(´æÔÚÓÚ³¡¾°ÖĞµ«ÊÇ²»Á¥ÊôÓÚ³¡¾°µØÍ¼µÄ¶ÔÏó)µ½³¡¾°ÖĞ¡£
	//## ·µ»ØÌí¼Ó¶ÔÏóÌí¼Óµ½³¡¾°ÖĞ»ñµÃµÄ±ê¼Ç¡£Èç¹û·µ»Ø0Öµ±íÊ¾Ìí¼Ó¶ÔÏóÊ§°Ü¡£»òÕß¶ÔÏóÃ»ÓĞ²»ÔÚ³¡¾°ÂäÔÚ³¡¾°µ±Ç°µÄ´¦ÀíÇøÓòÖĞ£¬Ìí¼Ó²Ù×÷±»ºöÂÔ¡£
	unsigned int AddObject(
		//##Documentation
		//## Òª¼ÓÈëµÄ¶ÔÏóÀàÊô¡£
		unsigned int uGenre, 
		//##Documentation
		//## Òª¼ÓÈëµÄ¶ÔÏóµÄid
		int nId, 
		//##Documentation
		//## ¼ÓÈëµÄÎ»ÖÃ×ø±ê¡£
		int x, int y, int z,
		int eLayerParam = IPOT_RL_OBJECT);

	//##ModelId=3DCAA7000085
	//##Documentation
	//## ÒÆ¶¯´æÔÚÓÚ³¡¾°ÖĞµ«ÊÇ²»Á¥ÊôÓÚ³¡¾°µØÍ¼µÄ¶ÔÏóÔÚ³¡¾°ÖĞµÄÎ»ÖÃ¡£
	//## Èç¹û¶ÔÏóÒÆ¶¯³É¹¦Ôò·µ»Ø¶ÔÏóÔÚ³¡¾°µØÍ¼ÖĞµÄ±ê¼ÇÊıÖµ£¬Îª·Ç0Öµ£»Èç¹ûÊ§°ÜÔò·µ»Ø0£»Èç¹û¶ÔÏóÒÆ³öÁË³¡¾°µ±Ç°´¦ÀíµÄÇøÓòÒ²·µ»Ø0¡£
	unsigned int MoveObject(
		//##Documentation
		//## ÒªÒÆ¶¯µÄ¶ÔÏóÀàÊô¡£
		unsigned int uGenre, 
		//##Documentation
		//## ÒªÒÆ¶¯µÄ¶ÔÏóµÄid
		int nId, 
		//##Documentation
		//## ¶ÔÏóµÄÄ¿µÄ×ø±ê¡£
		int x, int y, int z, 		 
		//##Documentation
		//## ÒÆ¶¯Ç°¶ÔÏóÔÚ³¡¾°ÖĞµÄ±ê¼ÇÊıÖµ¡£Èç¹û¶ÔÏóÊÇĞÂ¼ÓÈë£¬Ôò´«Èë0Öµ¡£
		unsigned int& uRtoid,
		int eLayerParam = IPOT_RL_OBJECT);

	//##ModelId=3DCAA70603E3
	//##Documentation
	//## È¥³ı´æÔÚÓÚ³¡¾°ÖĞµ«ÊÇ²»Á¥ÊôÓÚ³¡¾°µØÍ¼µÄ¶ÔÏó
	void RemoveObject(
		//##Documentation
		//## ÒªÈ¥³ıµÄ¶ÔÏóÀàÊô¡£
		unsigned int uGenre, 
		//##Documentation
		//## ÒªÈ¥³ıµÄ¶ÔÏóµÄid
		int nId, 
		//##Documentation
		//## ¶ÔÏó±»Çå³ıÇ°ÔÚ³¡¾°ÖĞµÄ±ê¼ÇÊıÖµ¡£
		unsigned int& uRtoid);

	void Breathe();
	void SetRepresentAreaSize(int nWidth, int nHeight);
	void ProjectDistToSpaceDist(int& nXDistance, int& nYDistance);
	void ViewPortCoordToSpaceCoord(
		int& nX,	//´«Èë£ºÊÓÍ¼/»æÍ¼Éè±¸×ø±êµÄxÁ¿£¬´«³ö£º¿Õ¼ä×ø±êµÄxÁ¿
		int& nY,	//´«Èë£ºÊÓÍ¼/»æÍ¼Éè±¸×ø±êµÄyÁ¿£¬´«³ö£º¿Õ¼ä×ø±êµÄyÁ¿
		int  nZ		//£¨ÆÚÍû£©µÃµ½µÄ¿Õ¼ä×ø±êµÄzÁ¿
		);
	void GetRegionLeftTopPos(int nRegionX, int nRegionY, int& nLeft, int& nTop);
	long GetObstacleInfo(int nX, int nY);
	long GetObstacleInfoMin(int nX, int nY, int nOffX, int nOffY);
	void SetObstacleInfoMin(long nValue, int nX, int nY, int nOffX, int nOffY);
	void RepresentShellReset();
	void	SetRegionLoadedCallback(funScenePlaceRegionLoadedCallback pfunCallback);
	void	SetHightLightSpecialObject(int nRegionX, int nRegionY, int nBioIndex);
	void	UnsetHightLightSpecialObject();
	void	GetSceneNameAndFocus(char* pszName, int& nId, int& nX, int& nY);
	void	GetSceneNameAndFocusOften(char* pszName, int& nId, int& nX, int& nY);
	void SetAmbient(DWORD dwAmbient);
	void SetCurrentTime(DWORD dwCurrentTime);
	void SetEnvironmentLight(int idx, BYTE r, BYTE g, BYTE b);
	void EnableBioLights(bool bEnable);
	void EnableDynamicLights(bool bEnable);
	void ChangeWeather(int nWeatherID);
	int GetMapInfo(KSceneMapInfo* pInfo);
	void PaintPUBGCircle(int nX, int nY, int Radius);
	void SetMapParam(unsigned int uShowElems, int nSize);
	void SetMapFocusPositionOffset(int nOffsetX, int nOffsetY);
	void  PaintMap(int nX, int nY);
	void PaintMapPoint(int nX, int nY);
	void FollowMapMove(int nbEnable);
	void EnableWeather(int nbEnable);
	void LoadGround(KIniFile *pIni); //add by phong kiÒu h×nh nÒn hoa s¬n
	BOOL PaintBackGround(); //add by phong kiÒu h×nh nÒn hoa s¬n
	BOOL OnDirectMap(int nX, int nY);
	void		DoDirectMap(int nX, int nY);
	void		DirectFindPos(int nX, int nY, BOOL bSync, BOOL bPaintLine);
	void		LoadSymbol(int nSubWorldID);//#maptraffic
	bool		bFlagMode;	 
	BYTE	 bPaintMode; 
	
#ifndef _SERVER
	KLittleMap *GetLittleMap();
	KScenePlaceMapC *GetKScenePlaceMapC();
#endif
	void FlagOnTarget(int x, int y)
	{
		m_Map.FlagOnTarget(x, y);
	}
	void FlagOnCoord(int x, int y)
	{
		m_Map.FlagOnCoord(x, y);
	}
	void RemoveFlag()
	{
		m_Map.RemoveFlag();
	}
	void SetFlagImage(const char* szImg, int nFlagOffset)
	{
		m_Map.SetFlagImage(szImg, nFlagOffset);
	}
	int GetCurFlagPos(unsigned int uYPos, int nPosParam)
	{
		return m_Map.GetCurFlagPos(uYPos, nPosParam);
	}
private:

	//##ModelId=3DCE68BB0238
	//##Documentation
	//## ¸Ä±ä¼ÓÔØ·¶Î§¡£
	void ChangeLoadArea();

	//##ModelId=3DBF946D0053
	//##Documentation
	//## ÏÈ¶ÔÈ«²¿Àë¿ª´¦Àí·¶Î§µÄÇøÓò¶ÔÏó£¬µ÷ÓÃFreePrerender·½·¨£¬ÊÍ·ÅÕâĞ©ÇøÓò¶ÔÏóÔ¤äÖÈ¾ºÃµÄ´óÕÅµØ±íÍ¼£¬È»ºóÉèÖÃÔ¤´¦ÀíĞÅºÅ¡£
	void ChangeProcessArea();

	//##ModelId=3DBFA1460230
	//##Documentation
	//## ³¡¾°´¦Àí·¶Î§ÄÚµÄÇøÓò¶ÔÏóµÄÔ¤´¦Àí¡£
	void Preprocess();

	//##ModelId=3DCCBD7B0239
	//##Documentation
	//## Çå³ı/ÊÍ·ÅÔ¤´¦Àí²úÉúµÄÊı¾İÓëĞÅÏ¢¡£
	void ClearPreprocess(int bIncludeRto);

	//##ModelId=3DBDBC7200B4
	//##Documentation
	//## ÉèÖÃĞÂµÄÒª¼ÓÔØµÄÇøÓò¡£
	void SetRegionsToLoad();

	//##ModelId=3DCB6BC90345
	//##Documentation
	//## ¼ÓÔØÇøÓòÊı¾İµÄ¹ı³Ì£¬¾ßÌåÄÚÈİ¼û»î¶¯Í¼£º
	//## Use Case View/³¡¾°/¿Í»§¶Ë/³¡¾°µØÍ¼¼ÓÔØ/State/Activity Model/¼ÓÔØÇøÓòµÄ»úÖÆ ÖĞ×ÓÏß³Ì¿Õ¼äµÄÖ´ĞĞÄÚÈİ
	void LoadProcess();

	//##ModelId=3DCCD131018C
	//##Documentation
	//## ¼ÓÔØÏß³ÌµÄÖ´ĞĞÈë¿Úº¯Êı¡£
	//## ÒÔpParam×÷ÎªKScenePlaceC¶ÔÏóµÄÖ¸Õë£¬µ÷ÓÃËüµÄLoadAndPreprocess·½·¨¡£
	//## ·µ»ØÖµ¹Ì¶¨Îª0¡£
	static DWORD WINAPI LoadThreadEntrance(void* pParam);

	void PrerenderGround(bool bForce);

	void ARegionLoaded(KScenePlaceRegionC* pRegion);
	
	KRUImage* GetFreeGroundImage();

	KScenePlaceRegionC*	GetLoadedRegion(int h, int v);

	void SetLoadingStatus(bool bLoading);

    void PreLoadProcess();

private:

	//##ModelId=3DCAC1D103D7
	//##Documentation
	//## KScenePlaceCÔËĞĞ²ÎÊı
	enum SP_WORKING_PARAM { 
		//##Documentation
		//## (¶¯Ì¬)¼ÓÔØ·¶Î§£¬×İ/ºáÏòÔÚÖĞĞÄÇøÓòËÄ²à»¹ÒªÑÓÉì³öµÄ·¶Î§¡£(µ¥Î»£ºÇøÓò)
		//## ÆäÈ¡ÖµÀ´×Ô(¼ÓÔØ·¶Î§×İ/ºáÏò¿çÔ½µÄÇøÓòµÄÊıÄ¿-1)/2
		SPWP_LOAD_EXTEND_RANGE = 3,
		//##Documentation
		//## ×î¶à¼ÓÔØµÄÇøÓòµÄÊıÄ¿
		SPWP_MAX_NUM_REGIONS = 49,
		//##Documentation
		//## ×î¶àÄÜ¹»½øÈë´¦Àí·¶Î§µÄÇøÓòµÄÊıÄ¿
		SPWP_NUM_REGIONS_IN_PROCESS_AREA = 49,
		//##Documentation
		//## ½¹µãÒÆ¶¯ºáÏò»òÕß×İÏò¿çÔ½¶àÉÙ¸öÇøÓòÖ®ºóÖØÉè¶¯Ì¬¼ÓÔØ·¶Î§
		SPWP_TRIGGER_RANGE = 1,
		//## ½¹µãÒÆ¶¯ºáÏò»òÕß×İÏòÍ»È»¿çÔ½¶àÉÙ¸öÇøÓò»á´¥¶¯½øÈë¼ÓÔØÖĞ×´Ì¬
		SPWP_TRIGGER_LOADING_RANGE = 1,
		//##Documentation
		//## ´¦ÀíµÄ¿ç¶È
		SPWP_PROCESS_RANGE = 6,
		//##Documentation
		//## ±íÊ¾Ò»¸öÒ£Ô¶µÄ×ø±êÖµ£¬Ô¶ÀëÓĞĞ§µÄ×ø±ê·¶Î§¡£
	    SPWP_FARAWAY_COORD = -2147476129,

		SPWP_REPRESENT_RECT_WINDAGE_X = 140,
		SPWP_REPRESENT_RECT_WINDAGE_T = 90,
		SPWP_REPRESENT_RECT_WINDAGE_B = 150,
		//##Documentation
		//##ÄÜ´¥·¢Ô¤ÏÈ´¦ÀíÓëÔ¤äÖÈ¾µÄÒÑ¼ÓÔØÇøÓòÊ÷Ä¿
		SPWP_PROCESS_PRERENDER_REGION_COUNTER_TRIGGER = 25,
		//##Documentation
		//##ÎŞ¼ÓÁ¿ÏÔÊ¾µÄ¶ÔÏóµÄ±êÖ¾
		SPWP_NO_HL_SPECAIL_OBJECT = -1,
		//##Documentation
		//##ÇĞ»»µØÍ¼¼ÓÔØµÄ³¬Ê±Ê±ÏŞ(µ¥Î»£ººÁÃë)
        SPWP_SWITCH_SCENE_TIMEOUT = 30000,
		//##Documentation
		//##rto¶ÔÏóµÄ´óÔ¼Ò»°ë¿í/¸ß¶È
        SPWP_RTO_HALF_RANGE = 80,
		//Ã»ÓĞ³¡¾°
		SPWP_NO_SCENE = -1,
	};

private:

	//³¡¾°ÖĞÒ»¸öÇøÓò±»¼ÓÔØÍê±ÏºóµÄ»Øµ÷º¯ÊıµÄÖ¸Õë¡£
	funScenePlaceRegionLoadedCallback m_pfunRegionLoadedCallback;

	//##ModelId=3DD2CA84021C
	//##Documentation
	//## ±êÖ¾¶ÔÏóÊÇ·ñÒÑ¾­³É¹¦Ö´µØ±»³õÊ¼»¯ÁË¡£
	bool	m_bInited;

	//## ÕıÔÚ¼ÓÔØÖĞ
	bool	m_bLoading;

	bool	m_bEnableWeather;

	//ÊÇ·ñ¸úËæµØÍ¼µÄÒÆ¶¯¶øÒÆ¶¯
	bool	m_bFollowWithMap;

	//Ô­Ê¼µÄ½¹µã×ø±ê£¬´ËÖµÖ»ÔÚm_bFollowWithMapÎªÕæÖµÊ±ÓĞĞ§
	POINT	m_OrigFocusPosition;

	//µØÍ¼½¹µã×ø±êµÄÆ«ÒÆ
	POINT	m_MapFocusOffset;

	//##ModelId=3DCE5BF203D6
	//##Documentation
	//## ½¹µã×ø±ê,´ËµãÔÚz=0Æ½ÃæÉÏ¡£
	POINT m_FocusPosition;

	//##ModelId=3DCD42F60221
	//##Documentation
	//## ½¹µãËùÔÚµÄÇøÓòµÄÇøÓò×İºáË÷Òı×ø±ê
	POINT m_FocusRegion;

	//##ModelId=3DD3B98002E1
	//##Documentation
	//## µ±Ç°µÄ½¹µãËùÔÚµÄÇøÓòµÄË÷ÒıÓë¾ÉµÄ½¹µãËùÔÚµÄÇøÓòµÄË÷ÒıÖµ¼ÓµÄ²îÖµ¡£
	//## ôßÀÛ¼Æ½¹µãÒÆ¶¯×İÏòÓëºáÏò¿çÔ½'ÇøÓò'µÄÊıÄ¿¡£
	SIZE m_FocusMoveOffset;

	//##ModelId=3DCB792F02D7
	//##Documentation
	//## ´¥·¢¼ÓÔØÇøÓòµÄĞÅºÅ£¨¼ò³Æ¼ÓÔØĞÅºÅ£©
	HANDLE m_hLoadRegionEvent;

	//## ÇĞ»»µØÍ¼¼ÓÔØ½áÊøµÄĞÅºÅ
	HANDLE m_hSwitchLoadFinishedEvent;

	//##ModelId=3DCD478001BA
	//##Documentation
	//## ´¥·¢Ô¤´¦Àí¼ÆËãµÄĞÅºÅ(¼ò³ÆÔ¤´¦ÀíĞÅºÅ)
	int	m_bPreprocessEvent;

	//##ModelId=3DCB83F30160
	//##Documentation
	//## ¾ÅÇøÓòÊı¾İ´¦Àí/Óû´¦ÀíÁÙ½çÇø¡£
	CRITICAL_SECTION m_ProcessCritical;

	//##ModelId=3DCAB77A01E9
	//##Documentation
	//## ¼ÓÔØ³¡¾°ÇøÓòĞŞ¸Ä¹Ø¼üÊı¾İµÄÁÙ½çÇø¶ÔÏó¡£
	CRITICAL_SECTION m_LoadCritical;

	//##ModelId=3DCB84480342
	//##Documentation
	//## ·ÃÎÊm_RegionListAdjustCriticalµÄÁÙ½çÇø¡£
	CRITICAL_SECTION m_RegionListAdjustCritical;


	//##ModelId=3DB907ED02B7
	//##Documentation
	//## £¨¹Ì¶¨£©Ò»¶¨ÊıÄ¿µÄRegion¶ÔÏóÊµÀı¡£
	KScenePlaceRegionC m_RegionObjs[SPWP_MAX_NUM_REGIONS];
	
	KRUImage m_pBackGroundImages[MAX_BACKGROUND_IMAGE];
	BOOL	m_bBackGroundImages;
	int		m_nBackGroundImages;
	RECT	BGArea[MAX_BACKGROUND_IMAGE];

	//##ModelId=3DDBC73803A4
	//##Documentation
	//## ½øÈë´¦Àí·¶Î§µÄÒÔ¼ÓÔØÇøÓòµÄÖ¸Õë¡£
	KScenePlaceRegionC* m_pInProcessAreaRegions[SPWP_NUM_REGIONS_IN_PROCESS_AREA];

	KRUImage	m_RegionGroundImages[SPWP_NUM_REGIONS_IN_PROCESS_AREA];
	int			m_nNumGroundImagesAvailable;

	//##ModelId=3DCAB33A01CE
	//##Documentation
	//## ³¡¾°µØÍ¼ÎÄ¼şµÄ¸ùÄ¿Â¼£¨ôß³¡¾°µØÍ¼¸ùÎÄ¼şËùÔÚµÄÄ¿Â¼£©¡£
	//## KScenePlaceC½èÖúm_szPlaceRootPath[0]ÊÇ·ñÎª0À´×÷ÎªÊÇ·ñ´ò¿ª³¡¾°µØÍ¼µÄÅĞ¶ÏÒÀ¾İ¡£
	char m_szPlaceRootPath[80];
	char m_szSceneName[32];
	int	 m_nSceneId;
	//##ModelId=3DD2E1410028
	//##Documentation
	//## m_pRegionsÁĞ±íµÄË÷Òı£¬Èç¹ûm_nFirstToLoadIndexµÄÖµÔÚÓĞĞ§·¶Î§ÄÚ£¬Ôò±íÊ¾m_pRegionsÁĞ±íÓÚm_nFirstToLoadIndexË÷ÒıÖ®Ç°µÄÔªËØÎªÒÑ¾­¼ÓÔØÁËËùĞèÊı¾İµÄÇøÓò¶ÔÏóµÄÖ¸Õë£¬Ê£ÓÚµÄÔªËØÎª´ı¼ÓÔØÊı¾İµÄÇøÓò¶ÔÏóµÄÖ¸Õë¡£Èç¹ûm_nFirstToLoadIndexµÄÖµ²»ÔÚÓĞĞ§·¶Î§ÄÚÔò±íÊ¾ÁĞ±íÖĞÈ«²¿Ö¸ÕëËù±íÊöµÄÇøÓò¶ÔÏó¶¼ÒÑ¾­¼ÓÔØÁËËùĞèµÄÊı¾İ¡£
	int m_nFirstToLoadIndex;

	//##ModelId=3DD2E1410064
	//##Documentation
	//## ÇøÓò¶ÔÏóÖ¸ÕëÁĞ±í¡£
	//## ÁĞ±íÖĞµÄÃ¿¸öÔªËØÄÚÈİ¶¼ÊÇÒ»¸öÇøÓò¶ÔÏóµÄÖ¸Õë£¬ÕâĞ©Ö¸Õë·Ö±ğ¾ÍÊÇÖ¸Ïòm_RegionObjsÖĞµÄÒ»¸öÔªËØ£¬ÊÇÒ»Ò»¶ÔÓ¦µÄ¹ØÏµ¡£ÇøÓò¶ÔÏóÖ¸ÕëÁĞ±íÖĞ¸öÔªËØµÄË³Ğò»á¸ù¾İÔËËã·¢Éú±ä»¯¡£
	KScenePlaceRegionC* m_pRegions[SPWP_MAX_NUM_REGIONS];

	//##ModelId=3DD3A6140179
	//##Documentation
	//## Ö´ĞĞ¼ÓÔØÇøÓòÊı¾İÓëÔ¤´¦ÀíµÄÏß³ÌµÄ¾ä±ú¡£
	HANDLE m_hLoadAndPreprocessThread;

	//##ModelId=3DD960150394
	//##Documentation
	//## ´¦ÓÚ´¦Àí·¶Î§ÄÚµÄ²¿·Ö¶ÔÏóµÄ¼¯ºÏ¡£
	KIpoTree m_ObjectsTree;

	RECT m_RepresentArea;
	SIZE m_RepresentExactHalfSize;
	
	long m_bRenderGround;

	int		m_nHLSpecialObjectRegionX, m_nHLSpecialObjectRegionY;
	int		m_nHLSpecialObjectBioIndex;

	KBuildinObj**	m_pObjsAbove;
	unsigned int	m_nNumObjsAbove;

	// µ±Ç°Ê±¼ä£¬Ò»Ìì¿ªÊ¼µÄ·ÖÖÓÊı
	int m_nCurrentTime;

	// ´¦Àí»·¾³¹âµÄ¶ÔÏó
	EnvironmentLight m_EnLight;
	// ÌìÆø¶ÔÏóÖ¸Õë
	KWeather *m_pWeather;
	//Ğ¡µØÍ¼
	KScenePlaceMapC	m_Map;

private:
	//##ModelId=3DDB39BA029B
	//##Documentation
	//## ¼ÆËã¼ÓÔØÇøÓòµÄÓÅÏÈ´ÎĞòµÄ±í
	static int m_PRIIdxTable[SPWP_MAX_NUM_REGIONS];

	//##ModelId=3DDB39150334
	//##Documentation
	//## ¼ÆËã¼ÓÔØ·¶Î§ÄÚ³¡¾°ÇøÓòµÄË÷Òı×ø±êÓÃµÄ±í
	static POINT m_RangePosTable[SPWP_MAX_NUM_REGIONS];

	// ¸ù¾İÍæ¼ÒÇ°½øµÄ·½Ïò£¬ĞèÒª¼ÓÔØµÄÏà¶ÔÍæ¼Òµ±Ç°RegionµÄÆ«ÒÆ
    static const KPrevLoadPosItem m_PrevLoadPosOffset[3][3];

	// ¸ù¾İÍæ¼ÒÇ°½øµÄ·½Ïò£¬ĞèÒª¼ÓÔØµÄRegion
    KPrevLoadPosItem    m_PreLoadPosItem;

    int                 m_nPrevLoadFileCount;
    KPrevLoadFileNameAndFrame   m_PrevLoadFileNameAndFrames[MAX_PREV_LOAD_FILE_COUNT];

};

extern KScenePlaceC	g_ScenePlace;
#endif /* KSCENEPLACEC_H_HEADER_INCLUDED_C22D00B5 */
