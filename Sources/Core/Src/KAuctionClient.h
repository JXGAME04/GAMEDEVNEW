// KAuctionClient.h - [DAUGIA 04/09 A3] cau noi Lua <-> cua so DAU GIA phia client. Xem KAuctionClient.cpp / KAuctionUiDef.h.
#ifndef KAUCTIONCLIENT_H
#define KAUCTIONCLIENT_H

typedef struct lua_State Lua_State;

#ifndef _SERVER
// 21 ham 2.0 ma \script\ui\uiauction_house.lua goi
int LuaAuc_OpenAuctionWindow(Lua_State* L);
int LuaAuc_CloseAuctionWindow(Lua_State* L);
int LuaAuc_SwitchAuctionWindow(Lua_State* L);
int LuaAuc_AuctionUiIsOpen(Lua_State* L);
int LuaAuc_NewAuctionEventArrival(Lua_State* L);
int LuaAuc_AuctionAddActivity(Lua_State* L);
int LuaAuc_AuctionDelActivity(Lua_State* L);
int LuaAuc_AuctionClearActivityList(Lua_State* L);
int LuaAuc_AuctionOnActivitySelect(Lua_State* L);
int LuaAuc_AuctionAddEnglishItem(Lua_State* L);
int LuaAuc_AuctionAddDutchItem(Lua_State* L);
int LuaAuc_AuctionSetEnglishItem(Lua_State* L);
int LuaAuc_AuctionSetDutchItem(Lua_State* L);
int LuaAuc_AuctionEndItem(Lua_State* L);
int LuaAuc_AuctionClearItemList(Lua_State* L);
int LuaAuc_AuctionAddActivityMember(Lua_State* L);
int LuaAuc_AuctionClearMemberList(Lua_State* L);
int LuaAuc_AuctionSetSalaryAndCount(Lua_State* L);
int LuaAuc_AuctionSetCurrentPageTxt(Lua_State* L);
int LuaAuc_AuctionClearAll(Lua_State* L);
int LuaAuc_AuctionPutOnMode(Lua_State* L);	// [A6] bat che do ky gui cho hop dua vat pham
int LuaAuc_SetAuctionIconVisible(Lua_State* L);
// ham phu cho script client
int LuaAuc_AuctionSetMoney(Lua_State* L);		// (nNganLuong, nXu) hien o tab ca nhan
int LuaAuc_AuctionConfirm(Lua_State* L);		// (szText, szYesFunc) hop Co/Khong dung lai hop cua thu
int LuaAuc_GetLocalTime(Lua_State* L);			// () -> time_t
int LuaAuc_PopBlackTips(Lua_State* L);			// (szMsg) -> thong bao he thong

// CoreShell.cpp: case GOI_AUCTION_UI -> AuctionUi_OnRequest(uParam, nParam)
void AuctionUi_OnRequest(unsigned int uParam, int nParam);
int  AuctionUi_IsOpen();
void AuctionUi_SetOpen(int bOpen);
#endif

#endif // KAUCTIONCLIENT_H
