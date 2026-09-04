// KChienLenhClient.h - [CL 04/09 DOT2] cau noi Lua <-> cua so CHIEN LENH phia client. Xem KChienLenhClient.cpp / KChienLenhUiDef.h.
#ifndef KCHIENLENHCLIENT_H
#define KCHIENLENHCLIENT_H

typedef struct lua_State Lua_State;

#ifndef _SERVER
// ham C++ ma \script\ui\uichienlenh.lua goi (may chu day du lieu xuong, Lua chuyen sang UI)
int LuaCLUi_SetIconVisible(Lua_State* L);	// (0/1)
int LuaCLUi_Open(Lua_State* L);				// ()
int LuaCLUi_Close(Lua_State* L);			// ()
int LuaCLUi_IsOpen(Lua_State* L);			// () -> 0/1
int LuaCLUi_Clear(Lua_State* L);			// () xoa moc + nhiem vu truoc khi day lai
int LuaCLUi_SetInfo(Lua_State* L);			// (score, cap, vip, gotlow, gotvip, somoc, lvscore, close, dangmo, vipprice)
int LuaCLUi_SetAward(Lua_State* L);			// (idx, need, cap, branch, count, szName, szItemInfo6so)
int LuaCLUi_SetMission(Lua_State* L);		// (id, kind, score, target, prog, state, szTitle, szTips)
int LuaCLUi_Refresh(Lua_State* L);			// () ve lai
int LuaCLUi_Msg(Lua_State* L);				// (szMsg) thong bao ngan

// CoreShell.cpp: case GOI_CHIENLENH_UI -> ChienLenhUi_OnRequest(uParam, nParam)
void ChienLenhUi_OnRequest(unsigned int uParam, int nParam);
#endif

#endif // KCHIENLENHCLIENT_H
