// KMailClient.h - [MAIL 03/09] cau noi Lua <-> cua so THU phia client. Xem KMailClient.cpp / KMailUiDef.h.
#ifndef KMAILCLIENT_H
#define KMAILCLIENT_H

typedef struct lua_State Lua_State;

#ifndef _SERVER
// 15 ham 2.0 ma \script\ui\uimail.lua goi + 3 ham phu (FormatTime2String, MailConfirm, Msg2Player)
int LuaMail_AddMailHeader(Lua_State* L);
int LuaMail_SetMailHeader(Lua_State* L);
int LuaMail_DeleteOneMail(Lua_State* L);
int LuaMail_CleanMailAll(Lua_State* L);
int LuaMail_CleanMailList(Lua_State* L);
int LuaMail_CleanMailDetail(Lua_State* L);
int LuaMail_UpdateMailCount(Lua_State* L);
int LuaMail_NewMailUIEventArrival(Lua_State* L);
int LuaMail_SwitchMailManager(Lua_State* L);
int LuaMail_OpenMailWindow(Lua_State* L);
int LuaMail_SetMailIconVisible(Lua_State* L);
int LuaMail_SelectMail(Lua_State* L);
int LuaMail_SetFilterText(Lua_State* L);
int LuaMail_SetMailBntStatus(Lua_State* L);
int LuaMail_UpdateMailDetail(Lua_State* L);
int LuaMail_FormatTime2String(Lua_State* L);
int LuaMail_MailConfirm(Lua_State* L);
int LuaMail_Msg2Player(Lua_State* L);

// CoreShell.cpp: case GOI_MAIL_UI
void MailUi_OnRequest(unsigned int uParam, int nParam);
#endif

#endif // KMAILCLIENT_H
