/*****************************************************************************************
//	界面声音
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-5-15
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "UiSoundSetting.h"
#include "../../core/src/coreshell.h"
#include <GameDataDef.h>
extern iCoreShell*		g_pCoreShell;

#define	SETTING_FILE	"\\Settings\\SoundList.txt"

static char s_SoundFileName[UI_SOUND_COUNT][80] = 
{
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	""
};

//载入设定
bool UiSoundLoadSetting()
{
	KIniFile	Setting;
	if (!Setting.Load(SETTING_FILE))
		return false;
	char	szSection[8];
	for (int i = 0; i < (int)UI_SOUND_COUNT; i++)
	{
		itoa(i, szSection, 10);
		Setting.GetString("Ui", szSection, "", s_SoundFileName[i], sizeof(s_SoundFileName[i]));
	}
	return true;
}

void UiSoundPlayItem(int idx)
{
	ChatItem CItem;
	int soundIdx = UI_SI_PICKPUT_ITEM;
	CItem.m_btDetail = 99; //flag for call GDI_GET_ITEM_PARAM from UiSoundPlayItem
	if (g_pCoreShell->GetGameData(GDI_GET_ITEM_PARAM, (unsigned int)&CItem, idx)) {
		if (CItem.m_dwRandomSeed) {	
			if (CItem.m_btGenre == 0) //equip
				soundIdx = CItem.m_btDetail + 6;
			if (CItem.m_btGenre == 1) //medicine
				soundIdx = UI_SI_ITEM_MEDICINE;
		}
	}
		
	if (CItem.m_dwRandomSeed)
		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_PLAY_SOUND, (unsigned int)(&s_SoundFileName[soundIdx]), 0);
}

//播放指定的声音
void UiSoundPlay(UI_SOUND_INDEX eIndex)
{
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_PLAY_SOUND, (unsigned int)(&s_SoundFileName[eIndex]), 0);
}

//播放指定的声音
void UiSoundPlay(char* SoundFileName)
{
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_PLAY_SOUND, (unsigned int)(SoundFileName), 0);
}
