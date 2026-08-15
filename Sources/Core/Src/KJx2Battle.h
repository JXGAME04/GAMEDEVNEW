// KJx2Battle.h - DOT E (cong thanh JX2, E4): khung BT_ battle framework toi gian.
// Goc JX2: he battle rieng co UI client (goi tin view/ladder). Client JX1 KHONG
// hieu goi do -> nhom Broad* la no-op CO CHU DICH; du lieu diem/thong ke la THAT:
// type -> PLAYER TASK id (bt_setnormaltask2type: 1->751, 2..50->700+i), moi ghi
// deu vao task nguoi choi + cache theo ten cho bang xep hang (nguoi offline giu
// gia tri cuoi). MOT tran tai mot thoi diem (citywar map 221; arena idle).
#ifndef KJX2BATTLE_H
#define KJX2BATTLE_H

#ifdef _SERVER

typedef struct lua_State Lua_State;

int LuaBT_SetType2Task(Lua_State* L);     // (nType, nTaskId) -> 0 gia tri
int LuaBT_GetData(Lua_State* L);          // (nType) -> so (task cua nguoi goi)
int LuaBT_SetData(Lua_State* L);          // (nType, nVal) -> 0 gia tri
int LuaBT_SetTypeBonus(Lua_State* L);     // (nType, nCamp, nBonus) -> 0
int LuaBT_GetTypeBonus(Lua_State* L);     // (nType, nCamp) -> so
int LuaBT_SetView(Lua_State* L);          // (nType) -> 0 (luu, khong phat)
int LuaBT_SetMissionName(Lua_State* L);   // (sz) -> 0
int LuaBT_SetGameData(Lua_State* L);      // (nKey, nVal) -> 0
int LuaBT_SetRestTime(Lua_State* L);      // (n) -> 0
int LuaBT_SortLadder(Lua_State* L);       // () -> 0 (ta sap khi doc)
int LuaBT_GetTopTenInfo(Lua_State* L);    // (nRank 1..10, nType) -> szName, nValue ; ngoai dai ("",0)
int LuaBT_UpdateMemberCount(Lua_State* L);// () -> 0
int LuaBT_ClearBattle(Lua_State* L);      // () -> 0 (xoa state + dua task ve 0 cho nguoi online)
int LuaBT_BroadView(Lua_State* L);        // no-op (goi UI battle JX2)
int LuaBT_BroadGameData(Lua_State* L);    // no-op
int LuaBT_BroadAllLadder(Lua_State* L);   // no-op
int LuaBT_BroadSelf(Lua_State* L);        // no-op
int LuaBT_ClearPlayerData(Lua_State* L);  // (trap.lua:52) reset task da map cua nguoi goi
int LuaBT_LeaveBattle(Lua_State* L);      // (trap.lua:53, chefu.lua:8) reset + go khoi bang xep hang

#endif // _SERVER
#endif // KJX2BATTLE_H
