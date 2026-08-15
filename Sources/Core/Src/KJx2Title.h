// KJx2Title.h - DOT E (cong thanh JX2, E4): he danh hieu DOC LAP voi quan ham
// Tong Kim JX1 (Get/SetPlayerTitle tren Npc - KHONG dung).
// Bang: \settings\playertitle_jx2.txt (doi ten tu playertitle.txt goc - ten goc
// DE PlayerTitle.txt quan ham TK, Windows khong phan biet hoa thuong).
// State per-player theo TEN nhan vat (RAM; active-id do script goc tu persist
// qua task 1122 - titlefuncs.lua; Thai Thu duoc E6 re-grant tu mirror luc boot;
// danh hieu event khac KHONG song qua restart - gioi han ghi nhan trong ke hoach).
#ifndef KJX2TITLE_H
#define KJX2TITLE_H

#ifdef _SERVER

typedef struct lua_State Lua_State;

int LuaTitle_AddTitle(Lua_State* L);      // (nId>0, nTimeType 0/1/2, nTime) >=3 doi -> 0 gia tri
                                          //  0 = vinh vien; 1 = FRAME (giay x18); 2 = moc MMDDHHMM
int LuaTitle_ActiveTitle(Lua_State* L);   // (nId; 0 = tat het) -> 1 so; tat cu (go aura/ext) roi bat moi
int LuaTitle_RemoveTitle(Lua_State* L);   // (nId) -> 0 gia tri
int LuaTitle_GetTitleInfo(Lua_State* L);  // (nId) -> nTimeType, nTime ; khong so huu -> (0,0)
int LuaTitle_GetTitleName(Lua_State* L);  // (nId) -> chuoi ; khong co -> ""
int LuaTitle_GetActiveTitle(Lua_State* L);// () -> nId / 0
int LuaTitle_GetTitleTab(Lua_State* L);   // () -> BANG id / nil (0 gia tri) khi rong

// cho E6 (cap/thu Thai Thu tu C++ khi doi chu thanh) - theo TEN nhan vat
void KJx2Title_GrantByName(const char* szPlayerName, int nTitleId);
void KJx2Title_RevokeByName(const char* szPlayerName, int nTitleId);

#endif // _SERVER
#endif // KJX2TITLE_H
