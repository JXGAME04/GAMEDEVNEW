/*****************************************************************************************
//	Copyright : Kingsoft 2003
//	Author	:   Fong Ki“u
//	CreateTime:	2020-1-10
*****************************************************************************************/
#include "KWin32.h"

#include "Windows.h"
#include "TextCtrlCmd.h"
#include "../../Core/src/MsgGenreDef.h"
#include "../Ui/ShortcutKey.h"

BOOL TextMsgFilter(const char* pText, int nLen)
{
	BOOL bHandled = false;

	if (pText[0] == TEXT_CTRL_CMD_PREFIX)
	{
		bHandled = KShortcutKeyCentre::ExcuteScript(pText + 1);
	}
	else if (pText[0] == TEXT_CTRL_CHAT_PREFIX)
	{
		bHandled = KShortcutKeyCentre::TranslateExcuteScript(pText + 1);
	}
	return bHandled;
}
