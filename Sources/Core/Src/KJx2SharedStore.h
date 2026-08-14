// KJx2SharedStore.h - DOT E (cong thanh JX2): kho chia se GIUA CAC LUA STATE.
// Engine ta nap moi file .lua mot Lua state rieng (KSortScript.cpp) nen moi kho
// dung chung (ObjBuffer / Ladder / GlbValue / League...) BAT BUOC nam o C++.
// Dac ta ham theo DIEUTRA_CONGTHANH_BINARY.md (dich nguoc jx_linux_y 14/08/2026):
//  - OB_*      : pool buffer 4096B/handle, grow tu do, FIFO (spec objbuffer_head.lua)
//  - Ladder_*  : bang xep hang top-10/id, id > 10000, persist file
//  - GlbValue  : kho int toan cuc (league_cityinfo dung lam khoa, id 840-846), RAM
// Struct o day la struct SACH cua cay ta - KHONG sao offset binary Linux 32-bit.
#ifndef KJX2SHAREDSTORE_H
#define KJX2SHAREDSTORE_H

#ifdef _SERVER

typedef struct lua_State Lua_State;

// ---- ObjBuffer (13 ham - thieu 1 la ObjBuffer:PushObject/PopObject chet) ----
int LuaOB_Create(Lua_State* L);      // -> handle (>0), 0 = fail
int LuaOB_Release(Lua_State* L);     // (h) -> 1/0
int LuaOB_IsEmpty(Lua_State* L);     // (h) -> 1 khi rong/handle xau, 0 khi con du lieu
int LuaOB_Clear(Lua_State* L);       // (h) -> 1/0
int LuaOB_Append(Lua_State* L);      // (hDst,hSrc) noi noi dung src vao duoi dst -> 1/0
int LuaOB_Copy(Lua_State* L);        // (hDst,hSrc) dst := src (ca con tro doc/ghi) -> 1/0
int LuaOB_PushByte(Lua_State* L);    // (h,n) -> 1/0
int LuaOB_PopByte(Lua_State* L);     // (h) -> byte; HET du lieu -> KHONG tra gi (nil)
int LuaOB_PushInt(Lua_State* L);
int LuaOB_PopInt(Lua_State* L);
int LuaOB_PushDouble(Lua_State* L);
int LuaOB_PopDouble(Lua_State* L);
int LuaOB_PushString(Lua_State* L);
int LuaOB_PopString(Lua_State* L);

// ---- Ladder (goc gui goi 52B len relay; ta 1 GS -> store tai cho + persist) ----
int LuaLadder_NewLadder(Lua_State* L);     // (id>10000, szName, nValue[,nType][,nSect][,nGender]) -> 0 gia tri
int LuaLadder_ClearLadder(Lua_State* L);   // (id>10000) -> 0 gia tri
int LuaLadder_GetLadderInfo(Lua_State* L); // (id, nRank 1..10) -> szName,nValue,nSect,nGender ; o trong -> ("",0,-1,0) KHONG nil

// ---- GlbValue ----
int LuaSetGlbValue(Lua_State* L);    // (id, nValue) -> 1
int LuaGetGlbValue(Lua_State* L);    // (id) -> nValue (0 neu chua dat)

#endif // _SERVER
#endif // KJX2SHAREDSTORE_H
