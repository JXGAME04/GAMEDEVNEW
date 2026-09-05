# -*- coding: ascii -*-
# [TKINFO 05/09] ScriptFuns.cpp: UpdateBattleInfo / UpdateBattleInfoAll (kind 7 dau + kind 8 hang, tu m_MissionLadder + timer).
import io, os

P = r"D:\GAMEDEVNEW_wt_mail\Sources\Core\Src\ScriptFuns.cpp"
T = "\t"

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) >= 128)
def nl_of(s): return "\r\n" if "\r\n" in s else "\n"
def L(nl, *lines): return nl.join(lines) + nl
def rep1(s, old, new):
    if s.count(old) != 1:
        old2 = old.replace("\n", "\r\n"); new2 = new.replace("\n", "\r\n")
        assert s.count(old2) == 1, ("anchor", old[:70], s.count(old), s.count(old2))
        old, new = old2, new2
    assert all(ord(c) < 128 for c in new)
    return s.replace(old, new)

s = rd(P); h0 = hi(s); nl = nl_of(s)
anchor = "// [TKDIEM 05/09] UpdateBattleBoxAll(nMissionId, nTong, nKim, nKind) -> so nguoi da gui." + nl
new = L(nl,
    "// [TKINFO 05/09] Cua so \"thong tin tran\" Tong Kim kieu Lien Dau Bang 2.0 (client KUiTongKimInfo) - kenh S2C_BATTLE_BOX co san:",
    "//   kind 7 = \"phase|rest_giay|tong|kim\"  (phase: 1 bao danh, 2 chien dau, 3 ket thuc; rest = timer nTimerId cua nhiem vu / 18)",
    "//   kind 8 = \"n;hang|ten|phe|diem;...\"    (top nRows tu m_MissionLadder - da xep theo tham so ladder, cap nhat trong",
    "//                                          KMission::SetPlayerParam moi lan cong diem; ten cat 12 ky tu de vua 127 byte)",
    "// UpdateBattleInfo(nMissionId, nPhase, nTimerId, nTong, nKim, nRows)    -> gui cho PlayerIndex hien tai (ra trai / vao tran)",
    "// UpdateBattleInfoAll(nMissionId, nPhase, nTimerId, nTong, nKim, nRows) -> gui ca tran (dieu kien nhu UpdateBattleBoxAll)",
    "static void sTkInfoBuild(KMission* pMission, int nPhase, int nTimerId, int nTong, int nKim, int nRows, char* szHead, char* szRows)",
    "{",
    T + "int nRest = 0;",
    T + "if (nTimerId > 0)",
    T*2 + "nRest = (int)(pMission->GetTimerRestTimer((unsigned char)nTimerId) / 18);\t// khung -> giay (18 khung/giay)",
    T + "sprintf(szHead, \"%d|%d|%d|%d\", nPhase, nRest, nTong, nKim);",
    T + "if (nRows > MISSION_STATNUM)",
    T*2 + "nRows = MISSION_STATNUM;",
    T + "int nHave = 0;",
    T + "for (int i = 0; i < nRows; i++)",
    T*2 + "if (pMission->m_MissionLadder[i].Name[0])",
    T*3 + "nHave++;",
    T + "int nLen = sprintf(szRows, \"%d\", nHave);",
    T + "int nLadder = pMission->GetMissionLadderParam();",
    T + "if (nLadder < 0 || nLadder >= MAX_MISSION_PARAM)",
    T*2 + "nLadder = 0;",
    T + "for (int i = 0; i < nRows; i++)",
    T + "{",
    T*2 + "TMissionLadderInfo& Info = pMission->m_MissionLadder[i];",
    T*2 + "if (!Info.Name[0])",
    T*3 + "continue;",
    T*2 + "char szName[16];",
    T*2 + "strncpy(szName, Info.Name, 12);",
    T*2 + "szName[12] = 0;",
    T*2 + "char szTmp[64];",
    T*2 + "int n = sprintf(szTmp, \";%d|%s|%d|%d\", i + 1, szName, (int)Info.ucGroup, Info.nParam[nLadder]);",
    T*2 + "if (nLen + n >= 127)",
    T*3 + "break;",
    T*2 + "memcpy(szRows + nLen, szTmp, n);",
    T*2 + "nLen += n;",
    T*2 + "szRows[nLen] = 0;",
    T + "}",
    "}",
    "",
    "static int sTkInfoSend(int nPlayerIndex, const char* szHead, const char* szRows)",
    "{",
    T + "if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER || !g_pServer)",
    T*2 + "return 0;",
    T + "if (Player[nPlayerIndex].m_nNetConnectIdx == -1)",
    T*2 + "return 0;",
    T + "S2C_BATTLE_BOX NetCommand;",
    T + "NetCommand.ProtocolType = s2c_battlebox;",
    T + "NetCommand.nType = 7;",
    T + "strcpy(NetCommand.szBattleDesc, szHead);",
    T + "g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(S2C_BATTLE_BOX));",
    T + "NetCommand.nType = 8;",
    T + "strcpy(NetCommand.szBattleDesc, szRows);",
    T + "g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(S2C_BATTLE_BOX));",
    T + "return 1;",
    "}",
    "",
    "static KMission* sTkInfoArgs(Lua_State* L, int& nPhase, int& nTimerId, int& nTong, int& nKim, int& nRows)",
    "{",
    T + "if (Lua_GetTopIndex(L) < 6)",
    T*2 + "return NULL;",
    T + "int nMissionId = (int)Lua_ValueToNumber(L, 1);",
    T + "nPhase = (int)Lua_ValueToNumber(L, 2);",
    T + "nTimerId = (int)Lua_ValueToNumber(L, 3);",
    T + "nTong = (int)Lua_ValueToNumber(L, 4);",
    T + "nKim = (int)Lua_ValueToNumber(L, 5);",
    T + "nRows = (int)Lua_ValueToNumber(L, 6);",
    T + "int nSubWorldIndex = GetSubWorldIndex(L);",
    T + "if (nMissionId < 0 || nSubWorldIndex < 0 || !g_pServer)",
    T*2 + "return NULL;",
    T + "if (nRows < 0) nRows = 0;",
    T + "return SubWorld[nSubWorldIndex].m_MissionArray.FindById(nMissionId);",
    "}",
    "",
    "int LuaUpdateBattleInfo(Lua_State* L)",
    "{",
    T + "int nPhase, nTimerId, nTong, nKim, nRows;",
    T + "KMission* pMission = sTkInfoArgs(L, nPhase, nTimerId, nTong, nKim, nRows);",
    T + "int nPlayerIndex = GetPlayerIndex(L);",
    T + "if (!pMission || nPlayerIndex <= 0)",
    T + "{",
    T*2 + "Lua_PushNumber(L, 0);",
    T*2 + "return 1;",
    T + "}",
    T + "char szHead[128], szRows[160];",
    T + "sTkInfoBuild(pMission, nPhase, nTimerId, nTong, nKim, nRows, szHead, szRows);",
    T + "Lua_PushNumber(L, sTkInfoSend(nPlayerIndex, szHead, szRows));",
    T + "return 1;",
    "}",
    "",
    "int LuaUpdateBattleInfoAll(Lua_State* L)",
    "{",
    T + "int nPhase, nTimerId, nTong, nKim, nRows;",
    T + "KMission* pMission = sTkInfoArgs(L, nPhase, nTimerId, nTong, nKim, nRows);",
    T + "if (!pMission)",
    T + "{",
    T*2 + "Lua_PushNumber(L, 0);",
    T*2 + "return 1;",
    T + "}",
    T + "char szHead[128], szRows[160];",
    T + "sTkInfoBuild(pMission, nPhase, nTimerId, nTong, nKim, nRows, szHead, szRows);",
    T + "int nCount = 0;",
    T + "int nIdx = 0;",
    T + "while (1)",
    T + "{",
    T*2 + "nIdx = pMission->m_MissionPlayer.m_UseIdx.GetNext(nIdx);",
    T*2 + "if (!nIdx)",
    T*3 + "break;",
    T*2 + "TMissionPlayerInfo& Info = pMission->m_MissionPlayer.m_Data[nIdx];",
    T*2 + "if (Info.m_nParam[MISSION_PARAM_AVAILABLE] != MISSION_AVAILABLE_VALUE)",
    T*3 + "continue;",
    T*2 + "int nPlayerIndex = (int)Info.m_ulPlayerIndex;",
    T*2 + "if (nPlayerIndex <= 0 || nPlayerIndex >= MAX_PLAYER)",
    T*3 + "continue;",
    T*2 + "if (Player[nPlayerIndex].m_dwID != Info.m_ulPlayerID)",
    T*3 + "continue;",
    T*2 + "nCount += sTkInfoSend(nPlayerIndex, szHead, szRows);",
    T + "}",
    T + "Lua_PushNumber(L, nCount);",
    T + "return 1;",
    "}",
    "") + anchor
s = rep1(s, anchor, new)
old = T*2 + '{"UpdateBattleBoxAll",' + T + "LuaUpdateBattleBoxAll}," + T + "// [TKDIEM 05/09] phat bang diem ca tran, 1 loi goi" + nl
new = old + T*2 + '{"UpdateBattleInfo",' + T + "LuaUpdateBattleInfo}," + T + "// [TKINFO 05/09] thong tin tran cho 1 nguoi (kind 7+8)" + nl \
          + T*2 + '{"UpdateBattleInfoAll",' + T + "LuaUpdateBattleInfoAll}," + T + "// [TKINFO 05/09] thong tin tran ca tran" + nl
s = rep1(s, old, new)
assert hi(s) == h0
wr(P, s); print("ScriptFuns.cpp: UpdateBattleInfo(All) ok")
