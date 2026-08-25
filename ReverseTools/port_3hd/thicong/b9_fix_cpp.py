# -*- coding: ascii -*-
"""B9 - VA MA C++ theo ket qua phan bien (A1_cpp.md).

N1  ITEM_GetExpiredTime tra SO PHUT + dung GetExpireTime() (tra 0 khi da het han).
    Ban goc 0x08154540 tra GIA TRI THO, khong phep tinh nao. Cho goi
    activitysys\\activity.lua:314-322 dua thang vao Time2Tm (epoch Unix that)
    => phai cong goc 1451581200. Sua: tra (nExpireTime + 1451581200), doc TRUC TIEP
    truong (khong qua getter) de khong mat gia tri da het han.

N9  ITEM_SetExpiredTime(idx, 0): ban goc (test esi,esi / je -> 0) = "KHONG dat han".
    Ban port hien coi 0 la "0 phut" => HET HAN NGAY. droptemplet.lua:129 chi kiem
    'if ... then' ma Lua coi 0 la true => co the truyen 0.

N7  GetRoomItems VUT BO tham so nRoomType (composeex.lua co 15 khoang) => sai khoang.

N4  AddStatData ghi bin\\server\\log_game\\ - THU MUC KHONG TON TAI (log_game la thu
    muc SCRIPT; thu muc log that la logs\\) => fopen hong lap lai moi loi goi.

N5  IniFile_Save dung KIniFile::Save ma chinh du an da cam (KJx2SharedStore.cpp:2-3
    "cat trang file... KHONG dung cho du lieu song") + KMemStack MAX_CHUNK=10 =>
    ~9 lan luu la sap. Hien 0 loi goi => doi thanh KHONG LAM GI + ghi log ro rang,
    de neu sau nay co ai noi day thi thay canh bao chu khong sap may chu.

N8  BT_GetBattleParam ban goc tra CHUOI (0x081C6A3A call lua_pushstring), port dang
    day so 0 => battlehead.lua getNpcInfo(str) tach sai. Doi thanh day chuoi rong.
"""
import io, os, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KJx2WarInfra.cpp"
d = io.open(P, encoding="latin-1", newline="").read()
n = 0


def sub(old, new, tag):
    global d, n
    c = d.count(old)
    if c != 1:
        print("  !! %-8s KHONG KHOP (%d)" % (tag, c))
        return
    d = d.replace(old, new)
    n += 1
    print("  OK %s" % tag)


# ---- N1 ----
sub(
    "\tint nExpire = Item[nItemIdx].GetExpireTime();\t// getter KItem.h:345 (tra 0 khi het/khong han)\r\n"
    "\tif (nExpire <= 0)\r\n\t{\r\n\t\tLua_PushNumber(L, 0);\r\n\t\treturn 1;\r\n\t}\r\n"
    "\tLua_PushNumber(L, (nExpire - KSG_GetCurSec()) / 60);\r\n",
    "\t// [VA 25/08 - N1] ban goc 0x08154540 tra GIA TRI THO (khong phep tinh nao) va\r\n"
    "\t// cho goi (activitysys\\activity.lua:314-322) dua thang vao Time2Tm = epoch Unix\r\n"
    "\t// that => phai cong goc 1451581200 cua JX1 (KItem.cpp:2629). Doc TRUC TIEP truong,\r\n"
    "\t// KHONG qua GetExpireTime() vi getter tra 0 khi vat pham DA het han.\r\n"
    "\tint nExpire = Item[nItemIdx].m_CommonAttrib.nExpireTime;\r\n"
    "\tif (nExpire <= 0)\r\n\t{\r\n\t\tLua_PushNumber(L, 0);\r\n\t\treturn 1;\r\n\t}\r\n"
    "\tLua_PushNumber(L, (double)nExpire + 1451581200.0);\r\n",
    "N1")

# ---- N9 ----
sub(
    "\tint nExpireSec;\t\t// giay theo goc 1451581200\r\n\tif (dVal >= 20000000.0)",
    "\t// [VA 25/08 - N9] ban goc: tham so 2 == 0 nghia la KHONG DAT HAN (test esi,esi/je),\r\n"
    "\t// KHONG phai 'het han ngay'. droptemplet.lua:129 chi kiem 'if .. then' ma Lua coi\r\n"
    "\t// 0 la true nen gia tri 0 CO THE toi day.\r\n"
    "\tif (dVal == 0.0)\r\n"
    "\t{\r\n"
    "\t\tItem[nItemIdx].SetExpireTime(0);\r\n"
    "\t\tLua_PushNumber(L, 1);\r\n"
    "\t\treturn 1;\r\n"
    "\t}\r\n"
    "\tint nExpireSec;\t\t// giay theo goc 1451581200\r\n\tif (dVal >= 20000000.0)",
    "N9")

# ---- N7 ----
sub(
    "\tint nCount = 0;\r\n\tstatic const int nRooms[2] = { room_equipment, room_equipmentex };",
    "\tint nCount = 0;\r\n"
    "\t// [VA 25/08 - N7] ton trong tham so nRoomType (composeex.lua co nhieu khoang).\r\n"
    "\t// Mac dinh (thieu tham so hoac <=0) = khay hanh trang + phan mo rong, y nhu truoc.\r\n"
    "\tint nRoomArg = (Lua_GetTopIndex(L) >= 1 && Lua_IsNumber(L, 1)) ? (int)Lua_ValueToNumber(L, 1) : -1;\r\n"
    "\tint nRoomsOne[1] = { nRoomArg };\r\n"
    "\tstatic const int nRoomsDef[2] = { room_equipment, room_equipmentex };\r\n"
    "\tconst int* pRooms = nRoomsDef;\r\n"
    "\tint nRoomN = 2;\r\n"
    "\tif (nRoomArg >= 0 && nRoomArg < room_num)\r\n"
    "\t{\r\n\t\tpRooms = nRoomsOne;\r\n\t\tnRoomN = 1;\r\n\t}\r\n"
    "\tstatic const int nRooms[2] = { room_equipment, room_equipmentex };",
    "N7a")
sub("\tfor (int r = 0; r < 2; r++)\r\n\t{\r\n\t\tKInventory* pRoom = &Player[nPlayerIndex].m_ItemList.m_Room[nRooms[r]];",
    "\tfor (int r = 0; r < nRoomN; r++)\r\n\t{\r\n\t\tKInventory* pRoom = &Player[nPlayerIndex].m_ItemList.m_Room[pRooms[r]];",
    "N7b")

# ---- N4 ----
sub('\t\tstrcat(szPath, "\\\\log_game\\\\hd3_statdata.log");',
    '\t\t// [VA 25/08 - N4] bin\\server\\log_game KHONG TON TAI (log_game la thu muc SCRIPT);\r\n'
    '\t\t// thu muc log that cua may chu la logs\\.\r\n'
    '\t\tstrcat(szPath, "\\\\logs\\\\hd3_statdata.log");',
    "N4")

# ---- N8 ----
sub('int LuaHD3_BT_GetBattleParam(Lua_State* L)\r\n{\r\n\tsHD3_LogOnce("BT_GetBattleParam");\r\n\tLua_PushNumber(L, 0);\r\n\treturn 1;\r\n}',
    'int LuaHD3_BT_GetBattleParam(Lua_State* L)\r\n{\r\n'
    '\t// [VA 25/08 - N8] ban goc 0x081C69B0 tra CHUOI (lua_pushstring), khong phai so.\r\n'
    '\t// battlehead.lua:631/639 goi getNpcInfo(str) - day so se tach sai.\r\n'
    '\tsHD3_LogOnce("BT_GetBattleParam");\r\n\tLua_PushString(L, "");\r\n\treturn 1;\r\n}',
    "N8")

io.open(P, "w", encoding="latin-1", newline="").write(d)

# ---- N5: IniFile_Save trong ScriptFuns.cpp ----
P2 = r"D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp"
d2 = io.open(P2, encoding="latin-1", newline="").read()
old5 = "\tLua_PushNumber(L, p->Ini.Save(szTo) ? 1 : 0);\n\treturn 1;"
new5 = ("\t// [VA 25/08 - N5] KIniFile::Save bi chinh du an CAM cho du lieu song\n"
        "\t// (KJx2SharedStore.cpp:2-3 \"cat trang file\"), va KMemStack MAX_CHUNK=10\n"
        "\t// lam ~9 lan luu la g_MessageBox + sprintf(NULL) => SAP may chu.\n"
        "\t// Hien KHONG co loi goi that nao trong 3 hoat dong. De an toan: khong ghi,\n"
        "\t// chi bao ro - neu sau nay co duong goi that se thay canh bao thay vi sap.\n"
        "\tg_DebugLog((LPSTR)\"[3HD] IniFile_Save CHUA HIEN THUC (tep %.120s) - xem BANGIAO_3HOATDONG_2508.md\", szTo);\n"
        "\tLua_PushNumber(L, 0);\n\treturn 1;")
if old5 in d2:
    d2 = d2.replace(old5, new5)
    io.open(P2, "w", encoding="latin-1", newline="").write(d2)
    print("  OK N5")
else:
    print("  !! N5 KHONG KHOP (%d)" % d2.count(old5))

print()
print("Da sua %d cho trong KJx2WarInfra.cpp" % n)
