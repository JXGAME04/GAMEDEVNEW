# -*- coding: utf-8 -*-
"""C37 - GOC THAT cua "toi gio ra NPC thi khong danh duoc" (chu bao 2 lan).

Da soat toan bo Phong Lang Do: MOI lenh dat co / chuyen che do KHOP LINUX 100%
(SetFightState x5 trong mission.lua + x1 fld_head, SetCurCamp, ForbidEnmity,
DisabledUseTownP, SetRevPos, SetLogoutRV, AddMSPlayer, SetTaskTemp - 0 lech).
=> Loi KHONG o script ma o ENGINE:

  KNpc::SetFightMode (KNpc.cpp:10290) CHI dat m_FightMode PHIA SERVER, khong bao
  cho client mot chu nao. Khi mission goi SetFightState(1) luc thuyen roi ben:
     - SERVER: nguoi choi = dang chien dau  -> chap nhan don danh
     - CLIENT: van tuong m_FightMode = 0 (chua rut vu khi)
       -> KNpcSet::GetRelation (client, KNpcSet.cpp:1458) ep relation_none cho cap
          (kind_player fight_none <-> kind_normal)
       -> SearchNpcAt loc theo relation_enemy => KHONG CHON DUOC QUAI = "khong danh duoc"
  (Cung mot lop mask da gay loi C12 "boss danh tang hinh".)

VA: sau khi doi fight mode cho NGUOI CHOI THAT, gui lai goi dong bo cho CHINH
client do bang duong CO SAN KNpc::SendSyncData(nNetConnectIdx) - goi PLAYER_SYNC
da mang co 0x02 (m_btSomeFlag) va client da xu ly san (KProtocolProcess.cpp:2384)
KHONG loai tru ban than. Khong them protocol moi, khong doi kich thuoc goi.
"""
import io, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

P = r"D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp"
T = "\t"
d = io.open(P, encoding="latin-1", newline="").read()
NL = "\r\n" if "\r\n" in d else "\n"
if "C37" in d:
    print("da va roi"); raise SystemExit

old = NL.join([
    "int LuaSetFightState(Lua_State* L)",
    "{",
    T + "int nPlayerIndex = GetPlayerIndex(L);",
    T + "if (nPlayerIndex <= 0) return 0;",
    "",
    T + "if (Player[nPlayerIndex].m_nIndex <= 0) return 0;",
    T + "Npc[Player[nPlayerIndex].m_nIndex].SetFightMode(Lua_ValueToNumber(L, 1) != 0);",
])
assert old in d, "anchor LuaSetFightState"
new = NL.join([
    "int LuaSetFightState(Lua_State* L)",
    "{",
    T + "int nPlayerIndex = GetPlayerIndex(L);",
    T + "if (nPlayerIndex <= 0) return 0;",
    "",
    T + "if (Player[nPlayerIndex].m_nIndex <= 0) return 0;",
    T + "Npc[Player[nPlayerIndex].m_nIndex].SetFightMode(Lua_ValueToNumber(L, 1) != 0);",
    "#ifdef _SERVER",
    T + "// [3HD 25/08 C37] BAO CHO CLIENT trang thai chien dau vua doi. Neu khong,",
    T + "// client van tuong minh CHUA RUT VU KHI => GetRelation (client) ep",
    T + "// relation_none cho cap (nguoi choi fight_none <-> quai) => SearchNpcAt",
    T + "// khong tra ve muc tieu nao = \"toi gio ra NPC ma khong danh duoc\".",
    T + "// Dung duong CO SAN: PLAYER_SYNC mang co 0x02 (m_btSomeFlag), client da",
    T + "// xu ly san va KHONG loai tru ban than (KProtocolProcess.cpp:2384).",
    T + "if (Player[nPlayerIndex].m_nNetConnectIdx >= 0)",
    T + T + "Npc[Player[nPlayerIndex].m_nIndex].SendSyncData(Player[nPlayerIndex].m_nNetConnectIdx);",
    "#endif",
])
d = d.replace(old, new)
io.open(P, "w", encoding="latin-1", newline="").write(d)
print("da va LuaSetFightState: gui PLAYER_SYNC cho chinh client sau khi doi fight mode")
