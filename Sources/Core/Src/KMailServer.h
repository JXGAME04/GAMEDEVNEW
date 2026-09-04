// KMailServer.h - [MAIL 03/09] kho THU tren MySQL (bang `mail`) cho may chu. Xem KMailServer.cpp.
#ifndef KMAILSERVER_H
#define KMAILSERVER_H

typedef struct lua_State Lua_State;

#ifdef _SERVER
// [MAIL 04/09 D10] loi gui thu cho code C++ khac (KTongJX2.cpp phat luong bang hoi).
// Tra id thu (0 = loi). nExpireSec <= 0 => han mac dinh. szAward theo dinh dang cot `award` (vd "money:50000").
int Mail_Send(const char* szRole, const char* szSender, const char* szTitle, const char* szContent,
	const char* szAward, int nAwardCount, int nExpireSec, const char* szSource);
int LuaMailDB_Ready(Lua_State* L);      // () -> 1 neu MySQL san sang va bang da co
int LuaMailDB_Send(Lua_State* L);       // (szRole, szSender, szTitle, szContent, szAward, nAwardCount, nExpireSec, szSource) -> id (0 = loi)
int LuaMailDB_Headers(Lua_State* L);    // (szRole, nMinId, nMax) -> bang {[i] = {id, sender, title, state, send, expire, award_count}}
int LuaMailDB_Get(Lua_State* L);        // (szRole, nId) -> bang {id, sender, title, content, award, award_count, state, send, expire} | nil
int LuaMailDB_SetState(Lua_State* L);   // (szRole, nId, nState, nBelow) -> 1 neu doi duoc (state < nBelow)
int LuaMailDB_Delete(Lua_State* L);     // (szRole, nId) -> 1/0 (state = 4)
int LuaMailDB_Count(Lua_State* L);      // (szRole) -> so thu con song
int LuaMailDB_PollNew(Lua_State* L);    // (nAfterId, nMax) -> bang {[i] = {id, role}} thu state 0 moi (web admin ghi thang bang)
int LuaMailDB_MaxId(Lua_State* L);      // () -> id lon nhat
int LuaMailDB_Sweep(Lua_State* L);      // () -> danh dau thu het han (bat dong bo)
#endif

#endif // KMAILSERVER_H
