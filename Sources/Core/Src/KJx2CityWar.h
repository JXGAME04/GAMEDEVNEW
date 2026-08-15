// KJx2CityWar.h - DOT E (cong thanh JX2): state 7 thanh + nhom ham CITY engine-side.
// Ban goc Linux: mang co dinh 7 phan tu (index 1..7), state 0..5; nhanh VN chi dung
// 3 trang thai (thuong / da co khieu chien gia / dang danh). Struct SACH cua cay ta -
// KHONG sao offset binary 32-bit (DIEUTRA_CONGTHANH_BINARY.md muc 1).
// Config: \settings\citywar.ini ([CityArea] AreaNameNN/AreaIncludesNN + [CitySettings]).
// Persist mirror: \settings\jx2citywar.txt (tmp + MoveFileEx nhu jx2league.txt).
// Dong bo ra KSubWorld (m_CityOwnTong dem " %s " / m_CityTax) cho MOI instance
// cung map id - SearchWorld chi tra instance dau nen phai tu quet.
#ifndef KJX2CITYWAR_H
#define KJX2CITYWAR_H

#ifdef _SERVER

typedef struct lua_State Lua_State;

// trang thai thanh (nhanh VN dung 3)
#define JX2CW_STATE_NORMAL		0	// binh thuong (co the vo chu / co chu)
#define JX2CW_STATE_WARDECIDED	1	// 19h da chot khieu chien gia - mai danh
#define JX2CW_STATE_ATWAR		2	// 20h dang danh (HaveBeginWar = 1)

// nhom CITY (call site: citybulletin.lua, citywar_function.lua, citywar_city\head.lua,
// citywar_global\timer.lua, infocenter_head.lua, station.lua)
int LuaGetCityOwner(Lua_State* L);        // (1..7) -> szOwnerTong, szMaster ; fail -> "",""
int LuaGetCityWarBothSides(Lua_State* L); // (1..7) -> szChallenger(CONG), szOwner(THU) ; ca 2 non-empty moi tra
int LuaGetCityAreaName(Lua_State* L);     // (1..7) -> ten thanh (TCVN3 tu ini) ; sai id -> ""
int LuaGetCityArea(Lua_State* L);         // () -> 0..7 theo MAP cua state goi (global "SubWorld")
int LuaGetCitySummary(Lua_State* L);      // (1..7) -> chuoi mo ta (toi gian, E6 lam giau)
int LuaGetAllCitySummary(Lua_State* L);   // goc gui goi 0xAC client JX2 - ta no-op 0 gia tri
int LuaSyncCitySummary(Lua_State* L);     // nhu tren - no-op
int LuaOpenCityManageUI(Lua_State* L);    // goc gui 0xA3 - no-op toi E7 (UI rieng)
int LuaHaveBeginWar(Lua_State* L);        // (1..7) -> 0/1 (PHAI la so - script so ~= 0)
int LuaNotifyWarResult(Lua_State* L);     // (1..7, bAttackerWin) -> 0 gia tri; cong thang: doi chu;
                                          // MOI nhanh: clear challenger + state ve thuong
int LuaAppointViceroy(Lua_State* L);      // (szCityName, szTongName) - TEN THANH, khong phai id
int LuaAppointChallenger(Lua_State* L);   // (szCityName, szTongName) -> state WARDECIDED
int LuaIsSigningUp(Lua_State* L);         // (1..7) -> 1 khi league 508 task 1 == 1
int LuaNumOfSignUpTongs(Lua_State* L);    // (1..7) -> so bang da nop lenh (member league 508)
int LuaGetSignUpTongName(Lua_State* L);   // (1..7, nIndex 0-based) -> ten bang ; fail -> ""
int LuaDisabledChatCity(Lua_State* L);    // stub (0 call site) -> 1
int LuaIsDisabledChatCity(Lua_State* L);  // stub (0 call site) -> 0
int LuaCTC_JX2_SetCityState(Lua_State* L);// (1..7, nState 0..2) - ha tang cho tick 5 pha (E6) -> 1/0

// nhom ARENA (E4) - nhanh VN loi dai CHET (binary goc khong dang ky
// SignUpCityWarArena): dang ky DU de script nap/thoai khong loi, tra "chua co"
int LuaIsArenaBegin(Lua_State* L);        // (0..7) -> 0
int LuaGetArenaBothSides(Lua_State* L);   // (0..7) -> "",""
int LuaGetArenaCityArea(Lua_State* L);    // (nArenaID) -> 0
int LuaGetArenaLevel(Lua_State* L);       // () -> 0 (chua/xong)
int LuaGetArenaTargetCity(Lua_State* L);  // () -> 0
int LuaGetArenaTotalLevel(Lua_State* L);  // () -> 0
int LuaGetArenaTotalLevelByCity(Lua_State* L); // (1..7) -> 0
int LuaGetArenaSchedule(Lua_State* L);    // (1..7) -> ""
int LuaGetArenaInfoByCity(Lua_State* L);  // (1..7[,nLevel]) -> ""
int LuaNotifyArenaResult(Lua_State* L);   // (nArenaID, bSide1Win) -> 0 gia tri (nuot)
int LuaGetCityWarTongCamp(Lua_State* L);  // (szTong) -> 1 thu / 2 cong / nil

// ArenaCredits (E4): goc = task value id 3179 > MAX_TASK 3000 cua ta (im lang
// vo hieu) -> remap id 2894 (da quet: khong script nao dung lam task value)
int LuaGetArenaCredits(Lua_State* L);     // () -> so
int LuaSetArenaCredits(Lua_State* L);     // (n)
int LuaAddArenaCredits(Lua_State* L);     // (n)
int LuaReduceArenaCredits(Lua_State* L);  // (n)

// mot lan sau khi map nap xong: nap config + mirror roi ghi vao KSubWorld
void KJx2CityWar_Breathe();

// cho ScriptFuns (de-hardcode 2 setter map 78) + E7 (thue): tra FALSE khi city sai
BOOL KJx2CityWar_SetTaxByCity(int nCityID, int nTax);          // clamp 0..MaxExchangeTax
BOOL KJx2CityWar_SetOwnerByCity(int nCityID, const char* szOwnerRaw); // ten THO (khong dem); rong = xoa
int  KJx2CityWar_GetSetting(int nIdx);    // 0 SignUpFee 1 MinTongLevel 2 MaxExchangeTax 3 MaxPriceParam
                                          // 4 MinTongCrowNumber 5 StartSetTaxTime 6 EndSetTaxTime
                                          // 7 WarCycleValue 8 SupplyLineBuildScale

#endif // _SERVER
#endif // KJX2CITYWAR_H
