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

// trang thai thanh - [LOI DAI CN 21/08] dung DU 6 trang thai cua ban goc
// (R +0x05 / G +0x05; mirror jx2citywar.txt 'V 2'; mirror cu 1->4, 2->5)
#define JX2CW_STATE_NORMAL		0	// roi (co the vo chu / co chu)
#define JX2CW_STATE_SIGNUP		1	// 18h-19h dang mo bao danh loi dai
#define JX2CW_STATE_BRACKET		2	// 19h da chot >= 2 bang, co bang dau, cho StartArena
#define JX2CW_STATE_ARENA		3	// 20h loi dai dang dien ra
#define JX2CW_STATE_WARDECIDED	4	// da co khieu chien gia - mai 20h cong thanh
#define JX2CW_STATE_ATWAR		5	// dang cong thanh (HaveBeginWar = 1)

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
int LuaIsSigningUp(Lua_State* L);         // (1..7) -> 1 khi state == SIGNUP (truoc: league 508)
int LuaNumOfSignUpTongs(Lua_State* L);    // (1..7) -> so bang bao danh loi dai (state 1..3)
int LuaGetSignUpTongName(Lua_State* L);   // (1..7, nIndex 0-based) -> ten bang bao danh ; fail -> ""
int LuaDisabledChatCity(Lua_State* L);    // stub (0 call site) -> 1
int LuaIsDisabledChatCity(Lua_State* L);  // stub (0 call site) -> 0
int LuaCTC_JX2_SetCityState(Lua_State* L);// (1..7, nState 0..5) - admin/ep trang thai -> 1/0

// nhom ARENA - [LOI DAI CN 21/08] dung lai ban CN goc (dich nguoc relay + GS,
// DACTA_LOIDAI_BANGHOI_CN_WORKFLOW.md): bao danh dau thau -> bang dau loai truc
// tiep <= 16 bang -> vo dich = khieu chien gia. 4 pha = 4 API Lua trung ten relay.
int LuaStartSignUp(Lua_State* L);         // (1..7) -> 1/0 : 18h xoa du lieu cu, state 1
int LuaEndSignUp(Lua_State* L);           // (1..7) -> 1/0 : 19h 0 bang->0, 1->challenger, >=2->bracket
int LuaStartArena(Lua_State* L);          // (1..7) -> 1/0 : 20h state 3, timer 18 mo mission 9
int LuaStartCityWar(Lua_State* L);        // (1..7) -> 1/0 : 20h hom sau; vo chu -> chiem luon; co chu -> state 5
int LuaSignUpCityWarArena(Lua_State* L);  // (1..7, nFee) - bang chu bao danh (10 dieu kien), tru quy bang
int LuaIsArenaBegin(Lua_State* L);        // (0..7) -> 1 khi cap nArenaID cua thanh dang dau chua co ket qua
int LuaGetArenaBothSides(Lua_State* L);   // (0..7) -> ten A, ten B / "",""
int LuaGetArenaCityArea(Lua_State* L);    // (nArenaID) -> thanh dang dau loi dai / 0
int LuaGetArenaLevel(Lua_State* L);       // () -> vong dang cho (1..) ; 0 gia tri khi khong co
int LuaGetArenaTargetCity(Lua_State* L);  // () -> thanh dang dau / 0
int LuaGetArenaTotalLevel(Lua_State* L);  // () -> log2(size bracket) cua thanh dang dau
int LuaGetArenaTotalLevelByCity(Lua_State* L); // (1..7) -> log2(size bracket)
int LuaGetArenaSchedule(Lua_State* L);    // (1..7) -> lich vong hien tai (chuoi)
int LuaGetArenaInfoByCity(Lua_State* L);  // (1..7[,nLevel]) -> moi ket qua da co
int LuaNotifyArenaResult(Lua_State* L);   // (nArenaID, bSide1Win) -> 0 gia tri; het cay -> challenger
int LuaGetCityWarTongCamp(Lua_State* L);  // (szTong) -> 1 thu / 2 cong / nil

// ArenaCredits (E4): goc = task value id 3179 > MAX_TASK 3000 cua ta (im lang
// vo hieu) -> remap id 2894 (da quet: khong script nao dung lam task value)
int LuaGetArenaCredits(Lua_State* L);     // () -> so
int LuaSetArenaCredits(Lua_State* L);     // (n)
int LuaAddArenaCredits(Lua_State* L);     // (n)
int LuaReduceArenaCredits(Lua_State* L);  // (n)

// E7 (khong protocol moi): thue qua thoai NPC quan thanh
int LuaCTC_JX2_SetTax(Lua_State* L);      // (1..7, nTax) -> 0 OK/1 quyen/2 gio/3 da dat/4 sai
int LuaCTC_JX2_GetTax(Lua_State* L);      // (1..7) -> thue hien tai

// mot lan sau khi map nap xong: nap config + mirror roi ghi vao KSubWorld
void KJx2CityWar_Breathe();
// [CITYINFO 21/08] day 7 thanh xuong client (UI_CITYINFO) - luc nap nhan vat, khi mo ban do, khi doi
void KJx2CityWar_SyncToPlayer(int nPlayerIndex);
void KJx2CityWar_SyncToAll();

// cho ScriptFuns (de-hardcode 2 setter map 78) + E7 (thue): tra FALSE khi city sai
BOOL KJx2CityWar_SetTaxByCity(int nCityID, int nTax);          // clamp 0..MaxExchangeTax
BOOL KJx2CityWar_SetOwnerByCity(int nCityID, const char* szOwnerRaw); // ten THO (khong dem); rong = xoa
int  KJx2CityWar_GetSetting(int nIdx);    // 0 SignUpFee 1 MinTongLevel 2 MaxExchangeTax 3 MaxPriceParam
                                          // 4 MinTongCrowNumber 5 StartSetTaxTime 6 EndSetTaxTime
                                          // 7 WarCycleValue 8 SupplyLineBuildScale

// [BOTBANG5 02/09] cho bot bang chu (KPlayerBot.cpp): dau thau bang chi so nguoi choi (10 cua y het Lua),
// tra 0 OK / 1..10 ly do (pb_BbTenKq); trang thai thanh JX2CW_STATE_* (-1 = sai id)
int KJx2CityWar_SignUpArenaC(int nPlayerIdx, int nCity, int nFee);
int KJx2CityWar_GetState(int nCity);

#endif // _SERVER
#endif // KJX2CITYWAR_H
