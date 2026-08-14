// KJx2League.h - DOT E (cong thanh JX2): he LEAGUE engine-side + GLOBAL mission timer.
// Ban goc: relay giu DB league, GS giu mirror; Apply* = goi len relay (ASYNC) roi relay
// callback ve GS. Ta 1 GS -> ghi thang store C++ va GOI CALLBACK DONG BO ngay trong
// cung loi goi (spec: phien dac ta LG_ 14/08 - khong script nao phu thuoc "chua thay ngay").
// Dac ta chi tiet tung ham (arity, gia tri tra, call site): xem ket qua phien dac ta
// trong KEHOACH_CONGTHANH_DOTE.md + DIEUTRA_CONGTHANH_BINARY.md.
//
// Hai KHONG GIAN HANDLE (deu la so):
//  - handle obj TAM (LG_CreateLeagueObj/LGM_CreateMemberObj): "form" dien roi Apply;
//    cap tu 1.000.000/2.000.000 de KHONG dung so voi lid (infocenter_head.lua:687
//    truyen nham lid vao LG_AddMemberToObj - phai im lang bo qua, khong crash).
//  - lid league DA DANG KY (LG_GetLeagueObj/GetFirst/GetNext...): cap tu 1.
// League khoa kep (nType, szName); member khoa = ten (chuoi TCVN3); league-task va
// member-task la 2 khong gian task TACH BIET. Moi ham Get* tra 0/""/(", 0) khi fail,
// KHONG tra nil (tru khi thieu tham so).
#ifndef KJX2LEAGUE_H
#define KJX2LEAGUE_H

#ifdef _SERVER

typedef struct lua_State Lua_State;

// vong doi obj tam + dang ky
int LuaLG_CreateLeagueObj(Lua_State* L);   // () -> handle tam (>0) / 0
int LuaLG_FreeLeagueObj(Lua_State* L);     // (h) -> 0 gia tri
int LuaLG_SetLeagueInfo(Lua_State* L);     // (h, nType, szName) -> 1
int LuaLG_AddMemberToObj(Lua_State* L);    // (hLeague, hMember) -> 1/0 ; h la lid -> bo qua
int LuaLG_ApplyAddLeague(Lua_State* L);    // (h [,szCbScript,szCbFunc]) -> 1/0 + cb dong bo
int LuaLG_ApplyRemoveLeague(Lua_State* L); // (nType, szName) -> 1/0

// truy van (dong bo tren store)
int LuaLG_GetLeagueObj(Lua_State* L);      // (nType, szName) -> lid / 0
int LuaLG_GetLeagueObjByRole(Lua_State* L);// (nType, szMemberName) -> lid / 0
int LuaLG_GetFirstLeague(Lua_State* L);    // (nType) -> lid / 0
int LuaLG_GetNextLeague(Lua_State* L);     // (nType, lid) -> lid ke / 0
int LuaLG_GetLeagueInfo(Lua_State* L);     // (lid) -> szName, nCreateTime, nMemberCount
int LuaLG_GetLeagueCreateTime(Lua_State* L);// (lid) -> nCreateTime (0 call site - du ten)
int LuaLG_GetMemberCount(Lua_State* L);    // (lid) -> n (0 neu lid xau)
int LuaLG_GetMemberInfo(Lua_State* L);     // (lid, nIndex 0-based) -> szName, nJob ; ngoai dai -> ("",0) KHONG nil
int LuaLG_GetMemberObj(Lua_State* L);      // 0 call site - stub 0
int LuaLG_GetMemberJoinTime(Lua_State* L); // 0 call site - stub 0

// task league (2 khong gian: league vs member)
int LuaLG_GetLeagueTask(Lua_State* L);         // (lid,nTask) HOAC (nType,szName,nTask) -> so / 0
int LuaLG_ApplySetLeagueTask(Lua_State* L);    // (nType,szName,nTask,nVal [,cbS,cbF]) -> 1/0
int LuaLG_ApplyAppendLeagueTask(Lua_State* L); // (nType,szName,nTask,nDelta [,cbS,cbF]) -> 1/0
int LuaLG_GetMemberTask(Lua_State* L);         // (nType,szName,szMember,nTask) LUON 4 -> so / 0
int LuaLG_ApplySetMemberTask(Lua_State* L);    // (nType,szName,szMember,nTask,nVal) 5 -> 1/0
int LuaLG_ApplyAppendMemberTask(Lua_State* L); // (nType,szName,szMember,nTask,nDelta [,cbS,cbF]) 5/7 -> 1/0

// member obj
int LuaLGM_CreateMemberObj(Lua_State* L);  // () -> handle tam
int LuaLGM_SetMemberInfo(Lua_State* L);    // (h, szMember, nJob, nType, szLgName) 5 - obj TU MANG dia chi league dich
int LuaLGM_ApplyAddMember(Lua_State* L);   // (h [,cbS,cbF]) -> 1 = OK (infocenter kiem ==1)
int LuaLGM_ApplyRemoveMember(Lua_State* L);// (nType,szName,szMember,cbS,cbF,n6) LUON 6 (n6 luon 0, bo qua)
int LuaLGM_FreeMemberObj(Lua_State* L);    // (h) -> 0 gia tri

// GS->relay chay script: ta thuc thi CUC BO dong bo trong state cua file dich
int LuaLG_ApplyDoScript(Lua_State* L);     // (nType,szName,szMember,szScript,szFunc,szParam,cbS,cbF) LUON 8

// GLOBAL mission + timer (nhip 5' cua cong thanh; autoexec goc: OpenGlbMission(8))
int LuaOpenGlbMission(Lua_State* L);       // (nMissionId) -> goi InitMission() cua script missions.txt[id]
int LuaStartGlbMSTimer(Lua_State* L);      // (nMissionId, nTimerId, nIntervalFrames) - LAP VO HAN, 18 frame = 1s
int LuaStopGlbMSTimer(Lua_State* L);       // (nMissionId, nTimerId)
void KJx2GlbMission_Breathe();             // goi tu CoreServerShell::Breathe moi tick (tu gioi han theo thoi gian)

// helper C cho cac module engine khac (KJx2CityWar: IsSigningUp/NumOfSignUpTongs/
// GetSignUpTongName doc league 508) - cung store voi cac ham Lua o tren
int         KJx2League_GetLeagueTaskC(int nType, const char* szName, int nTaskId);  // 0 khi khong co
int         KJx2League_GetMemberCountC(int nType, const char* szName);              // 0 khi khong co
const char* KJx2League_GetMemberNameC(int nType, const char* szName, int nIndex);   // "" khi ngoai dai

#endif // _SERVER
#endif // KJX2LEAGUE_H
