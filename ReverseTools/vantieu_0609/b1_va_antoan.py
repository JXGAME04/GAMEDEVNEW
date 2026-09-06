# -*- coding: ascii -*-
"""b1_va_antoan.py -- [LMBC 06/09] BUOC B1: ba ban va an toan + noi MAX_NPCPARAM.

Buoc nay CHUA co hanh vi van tieu nao. No chi:
  1. GameDataDef.h  : MAX_NPCPARAM 4 -> 8   (script Linux dung toi chi so 6)
  2. ScriptFuns.cpp : KIEM BIEN cho LuaSetNpcParam / LuaGetNpcParam
     (bat buoc cung commit voi muc 1 -- truoc day khong kiem = cua ghi de bo nho
      tuy y qua SetNpcValue/SetNpcParam, va cay script dang chay CO ghi chi so 4:
      tong_disciple\\npcpoint.lua:129, tong_springfestival\\npcpoint.lua:129,
      shrewmouse.lua:40, festival_shrewmouse.lua:34, doc lai o shrewmouse.lua:37,
      festival_shrewmouse.lua:31, scriptjx2\\tong\\npc\\muren_death.lua:13)
  3. ScriptFuns.cpp : strcpy -> g_StrCpyLen tai 2 diem ghi Name/Owner
     (Owner[32] nam NGAY SAU Name[32] trong KNpc.h:503-504 nen ten dai la tran de;
      ten xe bang Linux "Tieu Xa bang cua [<bang>]<nguoi>" chac chan > 31 byte)
  4. KNpc.cpp       : KIEM BIEN Player[] o hai cong Owner (:4054 va :4769)
     Hai cho sua KHAC NHAU: cho thu hai PHAI giu nguyen 13 ZeroMemory + IgnoreState(TRUE).

Ghi chu ve muc 1: hien cac script tren dang ghi/doc NGOAI MANG (MAX_NPCPARAM=4 nen
chi so hop le la 0..3). Ghi va doc DOI XUNG cung chi so 4, va AddNpcSet1 xoa ca mang
ve 0, nen sau khi noi mang len 8 thi logic script GIU NGUYEN -- chi mat tac dung phu
ngoai y muon len truong nam ke sau mang.

LUAT: tep nguon la ANSI/TCVN3 -> doc/ghi latin-1, CAM dung cong cu Edit/Write.
Chay lai duoc (idempotent). So byte cao moi tep KHONG duoc doi.

dung: python b1_va_antoan.py [<goc worktree>]
"""
import io
import os
import sys

ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_vantieu"
SRC = os.path.join(ROOT, "Sources", "Core", "Src")
MARK = "[LMBC 06/09]"


def rd(p):
    with io.open(p, "r", encoding="latin-1", newline="") as f:
        return f.read()


def wr(p, s):
    with io.open(p, "w", encoding="latin-1", newline="") as f:
        f.write(s)


def hb(s):
    return sum(1 for c in s if ord(c) >= 0x80)


def patch(name, edits, marker=MARK):
    path = os.path.join(SRC, name)
    d = rd(path)
    if marker in d:
        print("  da va tu truoc, bo qua:", name)
        return
    eol = "\r\n" if "\r\n" in d else "\n"
    before = hb(d)
    for old, new in edits:
        assert all(ord(ch) < 0x80 for ch in new), "ma chen phai ASCII thuan: %s" % name
        o = old.replace("\n", eol)
        n = new.replace("\n", eol)
        c = d.count(o)
        assert c == 1, "NEO KHONG DUY NHAT (%d) trong %s:\n%r" % (c, name, old[:160])
        d = d.replace(o, n)
    assert hb(d) == before, "SO BYTE CAO DOI trong %s" % name
    assert marker in d
    # sao luu mot lan
    bak = path + ".truoc_lmbc"
    if not os.path.isfile(bak):
        wr(bak, rd(path))
    wr(path, d)
    print("  da va:", name)


# ---------------------------------------------------------------------------
# 1. GameDataDef.h : MAX_NPCPARAM 4 -> 8
# ---------------------------------------------------------------------------
GAMEDATADEF = [
    ("#define\t\tMAX_NPCPARAM\t\t\t4\n",
     "// [LMBC 06/09] 4 -> 8: script Long Mon Tieu Cuc (ban Linux) dung toi chi so 6.\n"
     "// Cay script dang chay CUNG da ghi/doc chi so 4 (tong_disciple, tong_springfestival,\n"
     "// muren_death) tuc la dang ghi NGOAI MANG; noi mang bien chung thanh o that.\n"
     "// KNpc.h chi duoc project Core dung (Engine/Src/LuaFuns.cpp chet vi USEOLD khong dinh nghia)\n"
     "// nen chi can Rebuild ca hai cau hinh cua Core, khong phai swap dong bo nhi phan khac.\n"
     "// PHAI di kem kiem bien o LuaSetNpcParam/LuaGetNpcParam (ScriptFuns.cpp).\n"
     "#define\t\tMAX_NPCPARAM\t\t\t8\n"),
]

# ---------------------------------------------------------------------------
# 2+3. ScriptFuns.cpp
# ---------------------------------------------------------------------------
SCRIPTFUNS = [
    # kiem bien LuaSetNpcParam
    ("\tif (nParamNum > 2)\n"
     "\t\tNpc[nNpcIndex].m_nNpcParam[(int)Lua_ValueToNumber(L, 2)] = (int)Lua_ValueToNumber(L, 3);\n"
     "\telse\n"
     "\t\tNpc[nNpcIndex].m_nNpcParam[0] = (int)Lua_ValueToNumber(L, 2);\n",
     "\tif (nParamNum > 2)\n"
     "\t{\n"
     "\t\t// [LMBC 06/09] truoc day KHONG kiem chi so: script goi SetNpcParam(idx, N, v)\n"
     "\t\t// voi N tuy y se ghi de bo nho ke sau mang. SetNpcValue la BI DANH cua chinh\n"
     "\t\t// ham nay (bang dang ky :15297) nen sua o day bit ca hai duong.\n"
     "\t\tint nP = (int)Lua_ValueToNumber(L, 2);\n"
     "\t\tif (nP < 0 || nP >= MAX_NPCPARAM)\n"
     "\t\t\treturn 0;\n"
     "\t\tNpc[nNpcIndex].m_nNpcParam[nP] = (int)Lua_ValueToNumber(L, 3);\n"
     "\t}\n"
     "\telse\n"
     "\t\tNpc[nNpcIndex].m_nNpcParam[0] = (int)Lua_ValueToNumber(L, 2);\n"),

    # kiem bien LuaGetNpcParam
    ("\tif (nParamNum > 1)\n"
     "\t\tLua_PushNumber(L, Npc[nNpcIndex].m_nNpcParam[(int)Lua_ValueToNumber(L, 2)]);\n"
     "\telse\n"
     "\t\tLua_PushNumber(L, Npc[nNpcIndex].m_nNpcParam[0]);\n",
     "\tif (nParamNum > 1)\n"
     "\t{\n"
     "\t\t// [LMBC 06/09] xem chu thich o LuaSetNpcParam. Tra 0 khi chi so ngoai bien\n"
     "\t\t// (KHONG tra nil) vi script goc so sanh thang ket qua voi so.\n"
     "\t\tint nP = (int)Lua_ValueToNumber(L, 2);\n"
     "\t\tif (nP < 0 || nP >= MAX_NPCPARAM)\n"
     "\t\t{\n"
     "\t\t\tLua_PushNumber(L, 0);\n"
     "\t\t\treturn 1;\n"
     "\t\t}\n"
     "\t\tLua_PushNumber(L, Npc[nNpcIndex].m_nNpcParam[nP]);\n"
     "\t}\n"
     "\telse\n"
     "\t\tLua_PushNumber(L, Npc[nNpcIndex].m_nNpcParam[0]);\n"),

    # strcpy Owner -> g_StrCpyLen
    ("\tstrcpy(Npc[nNpcIndex].Owner, (char*)Lua_ValueToString(L, 2));\n",
     "\t// [LMBC 06/09] strcpy -> g_StrCpyLen: Owner[32] nam ngay sau Name[32] (KNpc.h:503-504),\n"
     "\t// ten dai hon 31 byte se tran de sang truong ke.\n"
     "\tg_StrCpyLen(Npc[nNpcIndex].Owner, (char*)Lua_ValueToString(L, 2), sizeof(Npc[nNpcIndex].Owner));\n"),

    # strcpy Name -> g_StrCpyLen  (KHONG dung nhanh GetString o tren, no da co sizeof)
    ("\telse if (Lua_IsString(L, 2))\n"
     "\t\tstrcpy(Npc[nNpcIndex].Name, (char*)Lua_ValueToString(L, 2));\n",
     "\telse if (Lua_IsString(L, 2))\n"
     "\t\t// [LMBC 06/09] strcpy -> g_StrCpyLen: ten xe tieu bang cua ban Linux\n"
     "\t\t// (\"Tieu Xa bang cua [<ten bang>]<ten nguoi>\") chac chan vuot Name[32].\n"
     "\t\tg_StrCpyLen(Npc[nNpcIndex].Name, (char*)Lua_ValueToString(L, 2), sizeof(Npc[nNpcIndex].Name));\n"),
]

# ---------------------------------------------------------------------------
# 4. KNpc.cpp : kiem bien Player[] o hai cong Owner
#    Neo lay theo ma ASCII quanh, tranh phan chu thich TCVN3.
# ---------------------------------------------------------------------------
KNPC = [
    # cong #1 (:4054) - chi return FALSE
    ("\t\tif(strcmp(Player[Npc[nAttacker].m_nPlayerIdx].m_PlayerName,Owner) == 0)",
     "\t\t// [LMBC 06/09] KIEM BIEN: quai / bay / dan deu co m_nPlayerIdx == 0 -> doc\n"
     "\t\t// Player[0] la khe CHUA KHOI TAO. Xe tieu cu it roi thanh nen chua lo, nhung\n"
     "\t\t// xe Long Mon di khap the gioi thi moi cu danh cua quai deu di qua day.\n"
     "\t\tint nLMBCAtkP = (nAttacker > 0 && nAttacker < MAX_NPC) ? Npc[nAttacker].m_nPlayerIdx : 0;\n"
     "\t\tif (nLMBCAtkP <= 0 || nLMBCAtkP >= MAX_PLAYER)\n"
     "\t\t{\n"
     "\t\t}\n"
     "\t\telse if(strcmp(Player[nLMBCAtkP].m_PlayerName,Owner) == 0)"),
    ("\t\tif(Player[Npc[nAttacker].m_nPlayerIdx].m_cPK.GetNormalPKState() == 0)",
     "\t\telse if(Player[nLMBCAtkP].m_cPK.GetNormalPKState() == 0)"),

    # cong #2 (:4769) - GIU NGUYEN 13 ZeroMemory + IgnoreState
    ("\t\tif (strcmp(Player[Npc[nLauncher].m_nPlayerIdx].m_PlayerName, Owner) == 0)",
     "\t\t// [LMBC 06/09] KIEM BIEN nhu cong tren. LUU Y: nhanh thu hai o day CO 13\n"
     "\t\t// ZeroMemory + IgnoreState(TRUE) truoc return, KHAC han cong :4054 - giu nguyen.\n"
     "\t\tint nLMBCLauP = (nLauncher > 0 && nLauncher < MAX_NPC) ? Npc[nLauncher].m_nPlayerIdx : 0;\n"
     "\t\tif (nLMBCLauP <= 0 || nLMBCLauP >= MAX_PLAYER)\n"
     "\t\t{\n"
     "\t\t}\n"
     "\t\telse if (strcmp(Player[nLMBCLauP].m_PlayerName, Owner) == 0)"),
    ("\t\tif (Player[Npc[nLauncher].m_nPlayerIdx].m_cPK.GetNormalPKState() == 0)",
     "\t\telse if (Player[nLMBCLauP].m_cPK.GetNormalPKState() == 0)"),
]


def main():
    assert os.path.isdir(SRC), SRC
    print("goc:", ROOT)
    patch("GameDataDef.h", GAMEDATADEF)
    patch("ScriptFuns.cpp", SCRIPTFUNS)
    patch("KNpc.cpp", KNPC)
    print("XONG B1")


if __name__ == "__main__":
    main()
