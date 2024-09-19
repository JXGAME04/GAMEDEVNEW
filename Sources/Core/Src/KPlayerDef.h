#ifndef KPLAYERDEF_H
#define KPLAYERDEF_H

#define DBLOADPERTIME_ITEM		10
#define DBLOADPERTIME_SKILL		10//edit by phong kieu fix loi skills khong login vao game mac dinh la 10
#define DBLOADPERTIME_FRIEND	10
#define	DBLOADPERTIME_TASK		10

//#dinh nghia bien trong lua script
#define SCRIPT_PLAYERID		"PlayerId"		
#define SCRIPT_PLAYERINDEX	"PlayerIndex"	
#define SCRIPT_OBJINDEX		"ObjIndex"		
#define SCRIPT_SUBWORLDINDEX		"SubWorld"		
#define SCRIPT_SUBWORLDID	"SubWorldID"		
#define SCRIPT_NPCINDEX		"NpcIndex"
#define SCRIPT_NPCID		"NpcId"	

#ifdef _SERVER
#define	MAX_PLAYER		600			//edit by phong kieu max_player server 1200 account vuot qua 499 loi tai day }TMissionPlayerInfo;
#else
#define	MAX_PLAYER		2	// Index 0 reserved
#endif

#define	CLIENT_PLAYER_INDEX	1

#define		MAX_TEAM_APPLY_LIST		32		

//---------------------------------------------------------------------------
#define	QUEST_BEFORE	0x0000
#define	QUEST_START		0x0001
#define	QUEST_FINISHED	0xffff
//---------------------------------------------------------------------------
#ifndef _SERVER
enum MOUSE_BUTTON
{
	button_left  = 0,
	button_right,
	button_num,
};
#endif

enum SCRIPTACTION  
{
	SCRIPTACTION_UISHOW,			
	SCRIPTACTION_EXESCRIPT,			
};

enum
{
	ATTRIBUTE_STRENGTH = 0,
	ATTRIBUTE_DEXTERITY,
	ATTRIBUTE_VITALITY,
	ATTRIBUTE_ENGERGY,
	ATTRIBUTE_RESET_ALL,
};

enum	
{
	Team_Create_Error_InTeam = 0,	
	Team_Create_Error_SameName,		
	Team_Create_Error_Name,			
	Team_Create_Error_TeamFull,		
	Team_Create_Error_CannotCreate,	
};

enum TEAM_FIGURE
{
	TEAM_CAPTAIN,				
	TEAM_MEMBER,			
};

enum SYNC_STEP
{
	STEP_BASE_INFO = 0,
	STEP_FIGHT_SKILL_LIST,
	//STEP_LIFE_SKILL_LIST,
	STEP_STATE_SKILL_LIST,
	STEP_TASK_LIST,
	STEP_ITEM_LIST,
	//STEP_FRIEND_LIST, //Kh«ng sö dông ®· viÕt bªn s3 relay_friendcfg.ini
	STEP_SYNC_END
};

enum SAVE_STATUS
{
	SAVE_IDLE = 0,
	SAVE_REQUEST,
	SAVE_DOING,
};
#endif
