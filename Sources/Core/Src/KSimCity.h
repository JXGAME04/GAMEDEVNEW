// KSimCity.h - Port SimCity (bot nguoi choi gia lap) tu JX2 Linux -> cay nguon JX1.
// GD1 Fix 1: co danh dau bot + cong giu can bang (Gate 4 cua cay nguon).
//
// Bot SimCity mang m_Kind = kind_player de co ngoai hinh nguoi choi day du
// (trang bi / ten bang / danh hieu qua PLAYER_NORMAL_SYNC). Nhung dieu do keo theo
// 4 tac dung phu DOI CAN BANG neu khong chan:
//   1. KNpc::DoDeath          - IsPlayer() && !FightMode -> bat tu (ve 1 mau)
//   2. exp skill 90/120       - nguoi choi danh bot nhan/mat exp skill
//   3. KNpc::DeathCalcPKValue - giet bot lam nguoi choi len diem PK
//   4. OnDamage script        - bot dinh vao Player[0] (o nhap dung chung)
// Co m_btSimCityBot lam bot MIEN NHIEM ca 4 (xem guard trong KNpc.cpp), giu
// hanh vi nguoi-choi-that va quai-that y het cu.
//
// Server-only, boc #ifdef _SERVER giong KJx2WarInfra.h. Xem PORT_SIMCITY_JX1_KEHOACH.md.
#ifndef KSIMCITY_H
#define KSIMCITY_H

#ifdef _SERVER

typedef struct lua_State Lua_State;

int LuaSC_SetBotFlag(Lua_State* L);   // (nNpcIdx, 0/1) -> 1 ok / 0 that bai
int LuaSC_GetBotFlag(Lua_State* L);   // (nNpcIdx)     -> 0/1

// GD1 Fix2 - sinh / xoa bot kind_player mang ngoai hinh nguoi choi (sentinel -1/-2)
int LuaSC_AddBot(Lua_State* L);       // (nSex,nLevel,nSubWorldIdx,nMpsX,nMpsY,nSeries[,szName][,nFaction][,nLifeMax]) -> nNpcIdx / 0
int LuaSC_DelBot(Lua_State* L);       // (nNpcIdx) -> 1/0 (khong dung LuaDelNpc - no bo qua kind_player)
int LuaSC_ClearBots(Lua_State* L);    // () -> so bot da xoa

// GD2 - di chuyen bot theo lo trinh
int LuaSC_MoveOn(Lua_State* L);       // () bat driver di chuyen
int LuaSC_MoveOff(Lua_State* L);      // () tat driver
int LuaSC_Goto(Lua_State* L);         // (nNpcIdx, nMpsX, nMpsY) phat 1 chang (test tay)
int LuaSC_PatrolBox(Lua_State* L);    // (nNpcIdx [, nHalfMps]) gan lo trinh tuan tra hinh vuong
int LuaSC_LoadPreset(Lua_State* L);   // (szPresetPath, szPathName) -> nRouteId / -1 (nap lo trinh tu file)
int LuaSC_SetBotRoute(Lua_State* L);  // (nNpcIdx, nRouteId [, bLoop=1]) -> 1/0

// GD3 - bot noi chuyen
int LuaSC_LoadChat(Lua_State* L);     // (szChatPath, szType) -> so cau da nap (cong don)
int LuaSC_ChatChance(Lua_State* L);   // (n) n/1000 moi giay moi bot; 0 = tat
int LuaSC_ClearChat(Lua_State* L);    // () xoa kho cau thoai

// GD4 - lop thong tin bot. Moi truong deu CO SAN tren KNpc va DA nam trong 2 goi sync
// s2c_syncplayer / s2c_syncplayermin -> khong them goi tin, khong doi kich thuoc goi.
int LuaSC_SetBotLook(Lua_State* L);    // (idx, nHelm, nArmor, nWeapon [, nHorse] [, nMantle] [, nMantleLevel] [, nMask])
int LuaSC_DressBot(Lua_State* L);      // (idx [, bRideHorse]) trang diem ngau nhien tu kho res THAT cua JX1
int LuaSC_SetBotFaction(Lua_State* L); // (idx, nFaction 0..9) dat mon phai + camp tuong ung
int LuaSC_SetBotInfo(Lua_State* L);    // (idx, nPKValue, nRepute, nFuYuan, nReBorn [, nRankInWorld] [, nImagePlayer])
int LuaSC_SetBotTitle(Lua_State* L);   // (idx, nRankId [, nRankBattleId] [, nPlayerTitle] [, nHonorId])
int LuaSC_SetBotTong(Lua_State* L);    // (idx, szTongName [, szTongTitle] [, nFigure] [, nRecruit])
int LuaSC_SetBotMate(Lua_State* L);    // (idx, szMateName)

void SC_Breathe();                    // goi moi tick tu CoreServerShell::Breathe (KHONG phai ham Lua)

#endif // _SERVER
#endif // KSIMCITY_H
