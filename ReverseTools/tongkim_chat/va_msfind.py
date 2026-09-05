# -*- coding: ascii -*-
# [MSFIND 05/09] (phuong an 3) KMissionArray::FindById + thay 27 cho "KMission X; X.SetMissionId(id); ...GetData(&X)"
# [TKDIEM 05/09] (phuong an 2) LuaUpdateBattleBoxAll: phat bang diem cho ca tran bang MOT loi goi Lua.
# Chi chen ASCII; doc/ghi latin-1 (nguon TCVN3); kiem so byte cao khong doi; kiem dem so cho thay = 27.
import io, os, re, sys

ROOT = r"D:\GAMEDEVNEW_wt_mail\Sources\Core\Src"
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

# ---------------- 1. KMissionArray.h: FindById
p = os.path.join(ROOT, "KMissionArray.h"); s = rd(p); h0 = hi(s); nl = nl_of(s)
old = T + "unsigned long FindSame(T *pMission)" + nl
new = L(nl,
    T + "// [MSFIND 05/09] Tra nhiem vu theo id KHONG dung KMission tam. Truoc: moi ham Lua nhiem vu viet",
    T + "// 'KMission Mission; Mission.SetMissionId(id); GetData(&Mission)' -> constructor KLinkArrayTemplate cap phat",
    T + "// 2 x new KLinkNode[] cho mang nguoi (MAX_PLAYER) + NPC (5000) + timer, chen toan bo o free, roi delete[]:",
    T + "// ~210 KB stack + 101 KB heap + ~6.500 vong moi loi goi (~40 us). Do 05/09: vong bang diem TK 2.400 loi goi",
    T + "// /lan chet = ~90 ms -> may chu bao hoa (ban giao BANGIAO_TONGKIM_CHAT_DIEM_0409.md muc 13f/13g).",
    T + "// FindSame chi so GetMissionId() va khong ghi nguoc -> ham nay tuong duong 100%.",
    T + "T* FindById(unsigned long ulMissionId)",
    T + "{",
    T*2 + "int nIdx = 0;",
    T*2 + "while(1)",
    T*2 + "{",
    T*3 + "nIdx = m_UseIdx.GetNext(nIdx);",
    T*3 + "if (!nIdx)",
    T*4 + "break;",
    T*3 + "if (m_Data[nIdx].GetMissionId() == ulMissionId)",
    T*4 + "return &m_Data[nIdx];",
    T*2 + "}",
    T*2 + "return NULL;",
    T + "};") + old
s = rep1(s, old, new)
assert hi(s) == h0
wr(p, s); print("KMissionArray.h ok")

# ---------------- 2. thay 27 cho
pat = re.compile(
    r"(?P<ind>[ \t]*)KMission[ \t]+(?P<v>\w+);(?P<nl>\r?\n)"
    r"[ \t]*(?P=v)\.SetMissionId\((?P<id>[^;\r\n]+)\);(?P=nl)"
    r"(?P<ind2>[ \t]*)(?P<lhs>KMission[ \t]*\*[ \t]*\w+[ \t]*=[ \t]*|return[ \t]+)"
    r"(?P<arr>SubWorld\[[^\]\r\n]+\]\.m_MissionArray)\.GetData\(&(?P=v)\);")
total = 0
for fn, expect in [("ScriptFuns.cpp", 25), ("KJx2WarInfra.cpp", 1), ("KPlayerBot.cpp", 1)]:
    p = os.path.join(ROOT, fn); s = rd(p); h0 = hi(s)
    def sub(m):
        return "%s%s%s.FindById(%s);%s// [MSFIND 05/09] tra theo id, khong dung KMission tam" % (
            m.group("ind2"), m.group("lhs"), m.group("arr"), m.group("id").strip(), T)
    s2, n = pat.subn(sub, s)
    assert n == expect, (fn, n, expect)
    assert len(re.findall(r"^[ \t]*KMission[ \t]+\w+;", s2, re.M)) == 0, (fn, "con KMission tam")
    assert hi(s2) == h0
    wr(p, s2); total += n; print(fn, "thay", n, "cho")
assert total == 27, total

# ---------------- 3. LuaUpdateBattleBoxAll + dang ky
p = os.path.join(ROOT, "ScriptFuns.cpp"); s = rd(p); h0 = hi(s); nl = nl_of(s)
old = L(nl,
    T*2 + "g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(S2C_BATTLE_BOX));",
    T + "return 0;",
    "}")
new = old + L(nl,
    "",
    "// [TKDIEM 05/09] UpdateBattleBoxAll(nMissionId, nTong, nKim, nKind) -> so nguoi da gui.",
    "// Phat bang diem \"tong|kim|diem_rieng\" (S2C_BATTLE_BOX) cho MOI nguoi con trong tran bang MOT loi goi Lua,",
    "// duyet thang m_UseIdx nhu KMission::Msg2All. Thay vong Lua 600 o trong lib_tktc.lua TK_GuiDiemPhe",
    "// (MSDIdx2PIdx + PIdx2MSDIdx + GetPMParam + UpdateBattleBox = 2.400 loi goi Lua-C moi lan chet; do 05/09 ~90 ms/lan",
    "// -> may chu bao hoa 55-78 ms/tick trong Tong Kim; xem BANGIAO_TONGKIM_CHAT_DIEM_0409.md muc 13f).",
    "// Dieu kien gui = dung dieu kien vong cu: o con dung (MISSION_PARAM_AVAILABLE), nguoi con noi, va chi so nguoi",
    "// van thuoc dung nguoi do (m_dwID == m_ulPlayerID; o cu cua nguoi da roi bi bo qua).",
    "int LuaUpdateBattleBoxAll(Lua_State* L)",
    "{",
    T + "int nCount = 0;",
    T + "if (Lua_GetTopIndex(L) < 4)",
    T + "{",
    T*2 + "Lua_PushNumber(L, 0);",
    T*2 + "return 1;",
    T + "}",
    T + "int nMissionId = (int)Lua_ValueToNumber(L, 1);",
    T + "int nTong = (int)Lua_ValueToNumber(L, 2);",
    T + "int nKim = (int)Lua_ValueToNumber(L, 3);",
    T + "BYTE nKind = (BYTE)Lua_ValueToNumber(L, 4);",
    T + "int nSubWorldIndex = GetSubWorldIndex(L);",
    T + "if (nMissionId < 0 || nSubWorldIndex < 0 || !g_pServer)",
    T + "{",
    T*2 + "Lua_PushNumber(L, 0);",
    T*2 + "return 1;",
    T + "}",
    T + "KMission* pMission = SubWorld[nSubWorldIndex].m_MissionArray.FindById(nMissionId);",
    T + "if (!pMission)",
    T + "{",
    T*2 + "Lua_PushNumber(L, 0);",
    T*2 + "return 1;",
    T + "}",
    T + "S2C_BATTLE_BOX NetCommand;",
    T + "NetCommand.ProtocolType = s2c_battlebox;",
    T + "NetCommand.nType = nKind;",
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
    T*2 + "if (Player[nPlayerIndex].m_nNetConnectIdx == -1)",
    T*3 + "continue;",
    T*2 + "if (Player[nPlayerIndex].m_dwID != Info.m_ulPlayerID)",
    T*3 + "continue;",
    T*2 + "sprintf(NetCommand.szBattleDesc, \"%d|%d|%d\", nTong, nKim, Info.m_nParam[6]);",
    T*2 + "g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &NetCommand, sizeof(S2C_BATTLE_BOX));",
    T*2 + "nCount++;",
    T + "}",
    T + "Lua_PushNumber(L, nCount);",
    T + "return 1;",
    "}")
s = rep1(s, old, new)
old = T*2 + '{"UpdateBattleBox",' + T + "LuaUpdateBattleBox}," + nl
new = old + T*2 + '{"UpdateBattleBoxAll",' + T + "LuaUpdateBattleBoxAll}," + T + "// [TKDIEM 05/09] phat bang diem ca tran, 1 loi goi" + nl
s = rep1(s, old, new)
assert hi(s) == h0
wr(p, s); print("ScriptFuns.cpp: UpdateBattleBoxAll + dang ky ok")
print("XONG")
