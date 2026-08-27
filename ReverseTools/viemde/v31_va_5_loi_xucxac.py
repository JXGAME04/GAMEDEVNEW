# -*- coding: utf-8 -*-
"""v31 - va 5 loi THAT trong ma xuc xac vua viet (do phan bien bat duoc, da tu kiem lai).

1. PLAYERINDEX TREO (nang nhat). `Finish()` truyen nPlayerIndex = m_nWinner, ma khi
   KHONG AI NHAN thi m_nWinner == 0 -> `CallScript` bo qua ca khoi dat PlayerIndex
   -> kich ban chay voi PlayerIndex CON SOT tu loi goi truoc. `saizi.lua:35` doc
   `YDBZ_sdl_getTaskByte(YDBZ_ITEM_YANDILING,1)` NGAY TRUOC vong lap, tuc doc nham
   du lieu cua nguoi khac.
   Sua: Finish tu chon nguoi con song dau tien khi khong co nguoi thang; va CallScript
   LUON dat PlayerIndex (dat 0 khi that su khong co ai) chu khong bao gio de treo.

2. THIEU SCRIPT_SUBWORLDINDEX. Moi loi goi kich ban khac trong cay deu dat bien nay
   (KPlayer.cpp:6975, :7397; KMission.cpp:89; ScriptFuns.cpp:11287). Thieu thi ham
   nao cua kich ban dung SubWorld se doc nham.

3. VUT GIA TRI TRA CUA CallFunction. Kich ban no la hong IM LANG - khong co dau vet
   nao trong nhat ky. Them canh bao.

4. `_NAME_LEN` trong KItemDice.h chi ton tai o KProtocol.h. Hien chi bien dich duoc
   NHO PCH keo KCore.h -> KProtocol.h vao truoc. Doi sang MAX_DICE_NAME_LEN cua
   chinh KDiceProtocol.h de header tu dung duoc.

5. `g_ItemDiceSet.Init()` khong ai goi. Ma van chay vi bien toan cuc duoc zero-init
   va Alloc() co chot `if (m_nNextId <= 0) m_nNextId = 1;`, nhung de nguyen la dua
   vao may man. Goi tuong minh canh g_TeamSet.Init() trong KCore.cpp.

KItemDice.h/.cpp va KCore.cpp deu CRLF.
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CAY = r"D:\GAMEDEVNEW\Sources\Core\Src"
HAU_TO = ".truoc_xucxac_2608"
F_H = os.path.join(CAY, "KItemDice.h")
F_C = os.path.join(CAY, "KItemDice.cpp")
F_K = os.path.join(CAY, "KCore.cpp")


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def ghi(p, s):
    bak = p + HAU_TO
    if not os.path.isfile(bak):
        shutil.copy2(p, bak)
    b = s.encode("latin-1")
    goc = io.open(bak, "rb").read()
    if (b.count(b"\n") - b.count(b"\r\n")) > (goc.count(b"\n") - goc.count(b"\r\n")):
        raise SystemExit("!! %s: sinh them dong LF le" % p)
    tmp = p + ".tmp"
    io.open(tmp, "wb").write(b)
    os.replace(tmp, p)


VA = []

# ---- (4) _NAME_LEN -> MAX_DICE_NAME_LEN
VA.append((F_H,
           "    char    szName[_NAME_LEN];      // chep lai de con hien ten khi ho thoat\r\n",
           "    char    szName[MAX_DICE_NAME_LEN];  // chep lai de con hien ten khi ho thoat\r\n"
           "                                        // (dung hang cua KDiceProtocol.h, KHONG dung\r\n"
           "                                        //  _NAME_LEN - hang do chi co o KProtocol.h)\r\n",
           "KItemDice.h: bo phu thuoc _NAME_LEN"))

# ---- (1) Finish: chon nguoi con song khi khong co nguoi thang
VA.append((F_C,
           "    // goi kich ban chot: szDoneFunc(nId, nWinner, nNumber).\r\n",
           "    // Chon NGU CANH nguoi choi cho loi goi kich ban. Khi khong ai nhan thi\r\n"
           "    // m_nWinner = 0; neu cu truyen 0 thi CallScript se KHONG dat PlayerIndex va\r\n"
           "    // kich ban chay voi PlayerIndex CON SOT tu loi goi truoc (saizi.lua doc\r\n"
           "    // YDBZ_sdl_getTaskByte ngay truoc vong lap -> doc nham nguoi khac).\r\n"
           "    int nCtx = m_nWinner;\r\n"
           "    if (nCtx <= 0)\r\n"
           "    {\r\n"
           "        for (int i = 0; i < m_nPlayerCount; i++)\r\n"
           "        {\r\n"
           "            if (sDicePlayerAlive(m_Players[i].nPlayerIndex, m_Players[i].dwPlayerId))\r\n"
           "            {\r\n"
           "                nCtx = m_Players[i].nPlayerIndex;\r\n"
           "                break;\r\n"
           "            }\r\n"
           "        }\r\n"
           "    }\r\n"
           "\r\n"
           "    // goi kich ban chot: szDoneFunc(nId, nWinner, nNumber).\r\n",
           "KItemDice.cpp: Finish chon ngu canh nguoi choi khi khong co nguoi thang"))

VA.append((F_C,
           '        CallScript(m_szDoneFunc, "ddd", m_nWinner, m_nId, m_nWinner, m_nWinNumber);\r\n',
           '        CallScript(m_szDoneFunc, "ddd", nCtx, m_nId, m_nWinner, m_nWinNumber);\r\n',
           "KItemDice.cpp: Finish truyen nCtx thay cho m_nWinner"))

# ---- (1)+(2)+(3) CallScript
VA.append((F_C,
           "    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)\r\n"
           "    {\r\n"
           "        Lua_PushNumber(pScript->m_LuaState, nPlayerIndex);\r\n"
           "        pScript->SetGlobalName(SCRIPT_PLAYERINDEX);\r\n"
           "        Lua_PushNumber(pScript->m_LuaState, Player[nPlayerIndex].m_dwID);\r\n"
           "        pScript->SetGlobalName(SCRIPT_PLAYERID);\r\n"
           "    }\r\n"
           "\r\n"
           "    int nTopIndex = 0;\r\n"
           "    pScript->SafeCallBegin(&nTopIndex);\r\n"
           "    if (strcmp(szTypes, \"ddd\") == 0)\r\n"
           "        pScript->CallFunction((char*)szFunc, 0, (char*)szTypes, nA, nB, nC);\r\n"
           "    else\r\n"
           "        pScript->CallFunction((char*)szFunc, 0, (char*)szTypes, nA, nB);\r\n"
           "    pScript->SafeCallEnd(nTopIndex);\r\n",

           "    // LUON dat ngu canh, KE CA khi khong xac dinh duoc nguoi choi: de treo thi\r\n"
           "    // kich ban se dung PlayerIndex CON SOT tu loi goi truoc do.\r\n"
           "    if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)\r\n"
           "    {\r\n"
           "        Lua_PushNumber(pScript->m_LuaState, nPlayerIndex);\r\n"
           "        pScript->SetGlobalName(SCRIPT_PLAYERINDEX);\r\n"
           "        Lua_PushNumber(pScript->m_LuaState, Player[nPlayerIndex].m_dwID);\r\n"
           "        pScript->SetGlobalName(SCRIPT_PLAYERID);\r\n"
           "        // moi loi goi kich ban khac trong cay deu dat ca bien nay\r\n"
           "        // (KPlayer.cpp:6975/:7397, KMission.cpp:89, ScriptFuns.cpp:11287)\r\n"
           "        if (Player[nPlayerIndex].m_nIndex > 0 && Player[nPlayerIndex].m_nIndex < MAX_NPC)\r\n"
           "        {\r\n"
           "            Lua_PushNumber(pScript->m_LuaState,\r\n"
           "                           Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex);\r\n"
           "            pScript->SetGlobalName(SCRIPT_SUBWORLDINDEX);\r\n"
           "        }\r\n"
           "    }\r\n"
           "    else\r\n"
           "    {\r\n"
           "        Lua_PushNumber(pScript->m_LuaState, 0);\r\n"
           "        pScript->SetGlobalName(SCRIPT_PLAYERINDEX);\r\n"
           "        Lua_PushNumber(pScript->m_LuaState, 0);\r\n"
           "        pScript->SetGlobalName(SCRIPT_PLAYERID);\r\n"
           "    }\r\n"
           "\r\n"
           "    int nTopIndex = 0;\r\n"
           "    BOOL bOk;\r\n"
           "    pScript->SafeCallBegin(&nTopIndex);\r\n"
           "    if (strcmp(szTypes, \"ddd\") == 0)\r\n"
           "        bOk = pScript->CallFunction((char*)szFunc, 0, (char*)szTypes, nA, nB, nC);\r\n"
           "    else\r\n"
           "        bOk = pScript->CallFunction((char*)szFunc, 0, (char*)szTypes, nA, nB);\r\n"
           "    pScript->SafeCallEnd(nTopIndex);\r\n"
           "    if (!bOk)\r\n"
           "        g_DebugLog((LPSTR)\"KItemDice: kich ban [%.40s] ham [%.30s] chay hong\",\r\n"
           "                   szLow, (char*)szFunc);\r\n",
           "KItemDice.cpp: CallScript luon dat ngu canh + SubWorld + bat loi kich ban"))

# ---- (5) goi Init
VA.append((F_K,
           "\tg_TeamSet.Init();\r\n",
           "\tg_TeamSet.Init();\r\n"
           "\tg_ItemDiceSet.Init();\t// 26/08: he xuc xac chia do (DICEITEM)\r\n",
           "KCore.cpp: goi g_ItemDiceSet.Init()"))


def main():
    theo_tep = {}
    for p, cu, moi, nhan in VA:
        theo_tep.setdefault(p, []).append((cu, moi, nhan))

    noidung = {}
    for p, ds in theo_tep.items():
        d = doc(p)
        for cu, moi, nhan in ds:
            if moi in d:
                print("   = da co san:", nhan)
                continue
            if d.count(cu) != 1:
                print("!! MO NEO khong duy nhat (%d lan): %s" % (d.count(cu), nhan))
                return 2
        noidung[p] = d
    print("moi mo neo deu duy nhat - bat dau sua\n")

    for p, ds in theo_tep.items():
        d = noidung[p]
        for cu, moi, nhan in ds:
            if moi in d:
                continue
            d = d.replace(cu, moi, 1)
            print("   > " + nhan)
        ghi(p, d)

    print()
    d = doc(F_C)
    print("   CallScript dat SubWorld      : %s" % ("CO" if "SCRIPT_SUBWORLDINDEX" in d else "KHONG"))
    print("   CallScript bat loi kich ban  : %s" % ("CO" if "chay hong" in d else "KHONG"))
    print("   Finish dung nCtx             : %s" % ("CO" if '"ddd", nCtx' in d else "KHONG"))
    print("   _NAME_LEN con trong header   : %s" % ("CON" if "_NAME_LEN" in doc(F_H) else "het"))
    print("   KCore.cpp goi Init           : %s"
          % ("CO" if "g_ItemDiceSet.Init()" in doc(F_K) else "KHONG"))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
