# -*- coding: utf-8 -*-
"""BAN DONG HANH - G4a: giao thuc c2s_partnerop + GDI/GOI/UOC phia Core.

Cac moi (deu co marker [BDH-G4], idempotent):
 1. Headers\KProtocolDef.h        : + c2s_partnerop (CUOI enum c2s, truoc _c2s_begin_relay)
 2. Headers\KPartnerProtocol.h    : MOI - struct PARTNER_OP_DATA + enum PARTNER_OP
 3. Sources\Core\Src\KProtocol.h  : include + prototype SendClientPartnerOp
    (Headers\KProtocol.h cung include neu no co include KDiceProtocol.h)
 4. Sources\Core\Src\KProtocol.cpp: bang size + ham SendClientPartnerOp
 5. KProtocolProcess.h            : khai c2sPartnerOp
 6. KProtocolProcess.cpp          : dang ky + handler
 7. CoreShell.h                   : GOI_PARTNER_OP + GDI_ITEM_IN_PARTNER_BAG (CUOI enum)
 8. CoreShell.cpp                 : case GOI + case GDI + 2 case UOC->pos
 9. KPlayerPartner.h              : PTG_FIGHTMODE + extern Partner_RunTalkScript
10. KPlayerPartner.cpp            : Partner_RunTalkScript + AI doc FIGHTMODE
"""
import io
import os
import shutil

BS = chr(92)
CR = chr(13)
LF = chr(10)
T = chr(9)
E = CR + LF

HDR = r"D:\GAMEDEVNEW\Headers"
SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, s, bak):
    if not os.path.exists(p + bak):
        shutil.copyfile(p, p + bak)
    io.open(p, "w", encoding="latin-1", newline="").write(s)


def patch(p, neo, moi, marker, bak=".truoc_bdh_g4"):
    s = doc(p)
    if marker in s:
        print("  da co:", os.path.basename(p), marker[:40])
        return
    n = s.count(neo)
    assert n == 1, "anchor %d lan trong %s: %r" % (n, p, neo[:60])
    ghi(p, s.replace(neo, moi, 1), bak)
    print("  VA:", os.path.basename(p), marker[:40])


# ---------- 1. KProtocolDef.h ----------
p = os.path.join(HDR, "KProtocolDef.h")
neo = "c2s_diceitem,"
s = doc(p)
if "c2s_partnerop" not in s:
    i = s.find(neo)
    assert i > 0
    j = s.find(LF, i) + 1
    them = T + "c2s_partnerop," + T + T + "// [BDH-G4] lenh cua so ban dong hanh" + E
    ghi(p, s[:j] + them + s[j:], ".truoc_bdh_g4")
    print("  VA: KProtocolDef.h c2s_partnerop")
else:
    print("  da co: KProtocolDef.h c2s_partnerop")

# ---------- 2. KPartnerProtocol.h (MOI) ----------
p = os.path.join(HDR, "KPartnerProtocol.h")
if not os.path.exists(p):
    body = (
        "#ifndef KPARTNERPROTOCOL_H" + E +
        "#define KPARTNERPROTOCOL_H" + E + E +
        "//---------------------------------------------------------------------------" + E +
        "// [BDH-G4] He BAN DONG HANH - goi lenh tu cua so / thanh nhanh client." + E +
        "// Tach tep rieng y het KDiceProtocol.h (doc ghi chu dau tep do: hai ban" + E +
        "// KProtocol.h dung chung include guard nen KHONG duoc nhet struct vao do)." + E +
        "//---------------------------------------------------------------------------" + E + E +
        "#pragma pack(push, enter_partner_protocol)" + E +
        "#pragma pack(1)" + E + E +
        "#define PARTNER_OP_NAME_LEN     32" + E + E +
        "// btOp trong goi c2s_partnerop" + E +
        "enum PARTNER_OP" + E +
        "{" + E +
        "    PARTNER_OP_CALLOUT     = 1,     // goi ra / thu ve (toggle)" + E +
        "    PARTNER_OP_SELECT      = 2,     // nParam = so con 1..3" + E +
        "    PARTNER_OP_TALK        = 3,     // mo doi thoai partner_talk.lua" + E +
        "    PARTNER_OP_ATTACK      = 4,     // che do chu dong cong kich" + E +
        "    PARTNER_OP_FOLLOW      = 5,     // che do chi di theo" + E +
        "    PARTNER_OP_RENAME      = 6,     // szName = ten moi (<=16 byte)" + E +
        "    PARTNER_OP_FORGETSKILL = 7,     // (mo qua doi thoai - nhu TALK)" + E +
        "    PARTNER_OP_DELETE      = 8,     // (mo qua doi thoai - nhu TALK)" + E +
        "};" + E + E +
        "typedef struct" + E +
        "{" + E +
        "    BYTE    ProtocolType;           // c2s_partnerop" + E +
        "    BYTE    btOp;                   // xem PARTNER_OP" + E +
        "    int     nParam;" + E +
        "    char    szName[PARTNER_OP_NAME_LEN];" + E +
        "} PARTNER_OP_DATA;" + E + E +
        "#pragma pack(pop, enter_partner_protocol)" + E + E +
        "#endif  // KPARTNERPROTOCOL_H" + E)
    io.open(p, "w", encoding="latin-1", newline="").write(body)
    print("  MOI: KPartnerProtocol.h")
else:
    print("  da co: KPartnerProtocol.h")

# ---------- 3. include + prototype ----------
for kp in [os.path.join(SRC, "KProtocol.h"), os.path.join(HDR, "KProtocol.h")]:
    s = doc(kp)
    if "KDiceProtocol.h" in s and "KPartnerProtocol.h" not in s:
        neo = '#include "KDiceProtocol.h"'
        assert s.count(neo) == 1, kp
        ghi(kp, s.replace(neo, neo + E + '#include "KPartnerProtocol.h"' +
                          T + "// [BDH-G4]", 1), ".truoc_bdh_g4")
        print("  VA include:", kp)
    else:
        print("  include da co / khong can:", os.path.basename(kp))

patch(os.path.join(SRC, "KProtocol.h"),
      "void SendClientDiceItem(int nDiceId, int nChoice);",
      "void SendClientDiceItem(int nDiceId, int nChoice);" + E +
      "void SendClientPartnerOp(int nOp, int nParam, const char* szName);" +
      T + "// [BDH-G4]",
      "SendClientPartnerOp(int nOp")

# ---------- 4. KProtocol.cpp ----------
patch(os.path.join(SRC, "KProtocol.cpp"),
      T + "sizeof(DICE_CHOICE_DATA),				// c2s_diceitem",
      T + "sizeof(DICE_CHOICE_DATA),				// c2s_diceitem" + E +
      T + "sizeof(PARTNER_OP_DATA),				// c2s_partnerop [BDH-G4]",
      "PARTNER_OP_DATA),")

send_fn = (
    "// [BDH-G4] Cua so / thanh nhanh Ban Dong Hanh gui lenh len may chu." + E +
    "void SendClientPartnerOp(int nOp, int nParam, const char* szName)" + E +
    "{" + E +
    T + "PARTNER_OP_DATA" + T + "Data;" + E + E +
    T + "memset(&Data, 0, sizeof(Data));" + E +
    T + "Data.ProtocolType = (BYTE)c2s_partnerop;" + E +
    T + "Data.btOp = (BYTE)nOp;" + E +
    T + "Data.nParam = nParam;" + E +
    T + "if (szName)" + E +
    T + T + "strncpy(Data.szName, szName, PARTNER_OP_NAME_LEN - 1);" + E +
    T + "if (g_pClient)" + E +
    T + T + "g_pClient->SendPackToServer((BYTE*)&Data, sizeof(PARTNER_OP_DATA));" + E +
    "}" + E + E)
patch(os.path.join(SRC, "KProtocol.cpp"),
      "void SendClientCPSetImageCmd(int ID)",
      send_fn + "void SendClientCPSetImageCmd(int ID)",
      "void SendClientPartnerOp(")

# ---------- 5. KProtocolProcess.h ----------
patch(os.path.join(SRC, "KProtocolProcess.h"),
      T + "void 	c2sDiceItem(int nIndex, BYTE* pProtocol);",
      T + "void 	c2sDiceItem(int nIndex, BYTE* pProtocol);" + E +
      T + "void 	c2sPartnerOp(int nIndex, BYTE* pProtocol);	// [BDH-G4]",
      "c2sPartnerOp(int nIndex")

# ---------- 6. KProtocolProcess.cpp ----------
patch(os.path.join(SRC, "KProtocolProcess.cpp"),
      T + "ProcessFunc[c2s_diceitem] = &KProtocolProcess::c2sDiceItem;",
      T + "ProcessFunc[c2s_diceitem] = &KProtocolProcess::c2sDiceItem;" + E +
      T + "ProcessFunc[c2s_partnerop] = &KProtocolProcess::c2sPartnerOp;	// [BDH-G4]",
      "ProcessFunc[c2s_partnerop]")

handler = (
    "// [BDH-G4] Ban Dong Hanh: client bam nut tren thanh nhanh / cua so." + E +
    "// Do dai goi da duoc CheckProtocolSize chan truoc (g_nProtocolSize)." + E +
    "void KProtocolProcess::c2sPartnerOp(int nIndex, BYTE* pProtocol)" + E +
    "{" + E +
    "#ifdef _SERVER" + E +
    T + "if (!pProtocol)" + E + T + T + "return;" + E +
    T + "if (nIndex <= 0 || nIndex >= MAX_PLAYER)" + E + T + T + "return;" + E +
    T + "if (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)" + E +
    T + T + "return;" + E +
    T + "if (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)" + E +
    T + T + "return;" + E + E +
    T + "PARTNER_OP_DATA* pInfo = (PARTNER_OP_DATA*)pProtocol;" + E +
    T + "if (pInfo->ProtocolType != c2s_partnerop)" + E + T + T + "return;" + E + E +
    T + "KPartnerSys* pSys = &Player[nIndex].m_cPartner;" + E +
    T + "if (pSys->GetG(PTG_VERSION) <= 0 || pSys->Count() <= 0)" + E +
    T + T + "return;" + T + "// chua co dong hanh nao - nut chua co tac dung" + E + E +
    T + "switch (pInfo->btOp)" + E +
    T + "{" + E +
    T + "case PARTNER_OP_CALLOUT:" + E +
    T + T + "pSys->CallOut(pSys->IsCallOut() ? 0 : 1);" + E +
    T + T + "break;" + E +
    T + "case PARTNER_OP_SELECT:" + E +
    T + T + "if (pInfo->nParam >= 1 && pInfo->nParam <= PARTNER_MAX_COUNT)" + E +
    T + T + T + "pSys->SetCurPartner(pInfo->nParam);" + E +
    T + T + "break;" + E +
    T + "case PARTNER_OP_ATTACK:" + E +
    T + T + "pSys->SetG(PTG_FIGHTMODE, 0);" + T + "// 0 = chu dong danh (mac dinh)" + E +
    T + T + "break;" + E +
    T + "case PARTNER_OP_FOLLOW:" + E +
    T + T + "pSys->SetG(PTG_FIGHTMODE, 1);" + T + "// 1 = chi di theo, khong danh" + E +
    T + T + "break;" + E +
    T + "case PARTNER_OP_RENAME:" + E +
    T + "{" + E +
    T + T + "char szTen[PARTNER_NAME_LEN + 1];" + E +
    T + T + "memset(szTen, 0, sizeof(szTen));" + E +
    T + T + "strncpy(szTen, pInfo->szName, PARTNER_NAME_LEN);" + E +
    T + T + "for (int c = 0; szTen[c]; c++)" + E +
    T + T + T + "if ((unsigned char)szTen[c] < 32)" + E +
    T + T + T + T + "szTen[c] = ' ';" + T + "// chan ky tu dieu khien" + E +
    T + T + "if (szTen[0])" + E +
    T + T + T + "pSys->SetName(pSys->GetCur(), szTen);" + E +
    T + "}" + E +
    T + T + "break;" + E +
    T + "case PARTNER_OP_TALK:" + E +
    T + "case PARTNER_OP_FORGETSKILL:" + T + "// ban goc xu qua menu doi thoai" + E +
    T + "case PARTNER_OP_DELETE:" + E +
    T + "default:" + E +
    T + T + "Partner_RunTalkScript(nIndex);" + E +
    T + T + "break;" + E +
    T + "}" + E +
    "#endif" + E +
    "}" + E + E)
patch(os.path.join(SRC, "KProtocolProcess.cpp"),
      "void KProtocolProcess::c2sSetImage(int nIndex, BYTE* pProtocol)",
      handler + "void KProtocolProcess::c2sSetImage(int nIndex, BYTE* pProtocol)",
      "c2sPartnerOp(int nIndex, BYTE* pProtocol)" + E + "{")

# KProtocolProcess.cpp can include KPlayerPartner.h (PTG_*, KPartnerSys, extern)
s = doc(os.path.join(SRC, "KProtocolProcess.cpp"))
if '#include "KPlayerPartner.h"' not in s:
    neo = '#include "KProtocolProcess.h"'
    assert s.count(neo) == 1
    ghi(os.path.join(SRC, "KProtocolProcess.cpp"),
        s.replace(neo, neo + E + '#include "KPlayerPartner.h"' + T + "// [BDH-G4]", 1),
        ".truoc_bdh_g4")
    print("  VA include KPlayerPartner.h vao KProtocolProcess.cpp")
else:
    print("  da co include KPlayerPartner.h")

# ---------- 7. CoreShell.h ----------
patch(os.path.join(SRC, "CoreShell.h"),
      T + "GOI_DICE_CHOICE," + E,
      T + "GOI_DICE_CHOICE," + E + E +
      T + "// [BDH-G4] cua so Ban Dong Hanh: uParam = PARTNER_OP_*, nParam = tham so." + E +
      T + "// Op RENAME: nParam = (int)(const char*) ten moi. Dat CUOI enum." + E +
      T + "GOI_PARTNER_OP," + E,
      "GOI_PARTNER_OP,")

patch(os.path.join(SRC, "CoreShell.h"),
      T + "GDI_ENCHASE_ITEM,		//Lo ren - kham nam trang bi" + E + "};",
      T + "GDI_ENCHASE_ITEM,		//Lo ren - kham nam trang bi" + E +
      T + "GDI_ITEM_IN_PARTNER_BAG,	// [BDH-G4] tui ban dong hanh (them CUOI)" + E + "};",
      "GDI_ITEM_IN_PARTNER_BAG,")

# ---------- 8. CoreShell.cpp ----------
p = os.path.join(SRC, "CoreShell.cpp")

goi_case = (
    T + "case GOI_PARTNER_OP:	// [BDH-G4] cua so ban dong hanh" + E +
    T + "{" + E +
    T + T + "SendClientPartnerOp((int)uParam, nParam," + E +
    T + T + T + "((int)uParam == PARTNER_OP_RENAME) ? (const char*)nParam : NULL);" + E +
    T + "}" + E +
    T + "break;" + E)
patch(p,
      T + "case GOI_MASKFEATURE:" + E,
      goi_case + T + "case GOI_MASKFEATURE:" + E,
      "case GOI_PARTNER_OP:")

gdi_case = (
    T + "case GDI_ITEM_IN_PARTNER_BAG:	// [BDH-G4] tui ban dong hanh" + E +
    T + T + "nRet = 0;" + E +
    T + T + "{" + E +
    T + T + T + "int nCount = 0;" + E +
    T + T + T + "KUiObjAtRegion* pInfo = (KUiObjAtRegion*)uParam;" + E +
    T + T + T + "PlayerItem* pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetFirstItem();" + E +
    T + T + T + "while (pItem)" + E +
    T + T + T + "{" + E +
    T + T + T + T + "if (pItem->nPlace == pos_partnerbag)" + E +
    T + T + T + T + "{" + E +
    T + T + T + T + T + "if (pInfo)" + E +
    T + T + T + T + T + "{" + E +
    T + T + T + T + T + T + "pInfo->Obj.uGenre = CGOG_ITEM;" + E +
    T + T + T + T + T + T + "pInfo->Obj.uId = pItem->nIdx;" + E +
    T + T + T + T + T + T + "pInfo->Region.h = pItem->nX;" + E +
    T + T + T + T + T + T + "pInfo->Region.v = pItem->nY;" + E +
    T + T + T + T + T + T + "pInfo->Region.Width = Item[pItem->nIdx].GetWidth();" + E +
    T + T + T + T + T + T + "pInfo->Region.Height = Item[pItem->nIdx].GetHeight();" + E +
    T + T + T + T + T + T + "pInfo++;" + E +
    T + T + T + T + T + "}" + E +
    T + T + T + T + T + "nCount++;" + E +
    T + T + T + T + T + "if (uParam && nCount > nParam)" + E +
    T + T + T + T + T + T + "break;" + E +
    T + T + T + T + "}" + E +
    T + T + T + T + "pItem = Player[CLIENT_PLAYER_INDEX].m_ItemList.GetNextItem();" + E +
    T + T + T + "}" + E +
    T + T + T + "nRet = nCount;" + E +
    T + T + "}" + E +
    T + T + "break;" + E)
patch(p,
      T + "case GDI_ITEM_IN_EX_BOX1:	// mo rong ruong 1" + E,
      gdi_case + T + "case GDI_ITEM_IN_EX_BOX1:	// mo rong ruong 1" + E,
      "case GDI_ITEM_IN_PARTNER_BAG:")

# 2 case UOC -> pos (P1 va P2)
for v in ("P1", "P2"):
    ob = "pObject1" if v == "P1" else "pObject2"
    neo = (T*4 + "case UOC_EX_BOX1:	// mo rong ruong 1" + E +
           T*5 + v + ".nPlace = pos_exbox1room;")
    moi = (T*4 + "case UOC_PARTNER_BAG:	// [BDH-G4] tui ban dong hanh" + E +
           T*5 + v + ".nPlace = pos_partnerbag;" + E +
           T*5 + v + ".nX = " + ob + "->Region.h;" + E +
           T*5 + v + ".nY = " + ob + "->Region.v;" + E +
           T*5 + "break;" + E + neo)
    patch(p, neo, moi, "case UOC_PARTNER_BAG:" + E + T*5 + v)

# ---------- 9. KPlayerPartner.h ----------
patch(os.path.join(SRC, "KPlayerPartner.h"),
      "#define PTG_CALLOUT_SWITCH       (PARTNER_TASK_BASE + 5)   // 1 = map cam goi (PARTNER_OFF)",
      "#define PTG_CALLOUT_SWITCH       (PARTNER_TASK_BASE + 5)   // 1 = map cam goi (PARTNER_OFF)" + E +
      "#define PTG_FIGHTMODE            (PARTNER_TASK_BASE + 6)   // [BDH-G4] 0 = chu dong danh, 1 = chi theo",
      "PTG_FIGHTMODE")

patch(os.path.join(SRC, "KPlayerPartner.h"),
      "void Partner_OnNpcDeath(int nNpcIdx);",
      "void Partner_OnNpcDeath(int nNpcIdx);" + E +
      "void Partner_RunTalkScript(int nPlayerIdx);   // [BDH-G4] mo doi thoai partner_talk.lua",
      "Partner_RunTalkScript")

# ---------- 10. KPlayerPartner.cpp ----------
talk_fn = (
    "//---------------------------------------------------------------------------" + E +
    "// [BDH-G4] mo doi thoai chinh (partner_talk.lua main) trong ngu canh player." + E +
    "// Dung CallFunction truc tiep de KHONG pha m_ActionScriptID (bay ExecuteScript2)." + E +
    "//---------------------------------------------------------------------------" + E +
    "void Partner_RunTalkScript(int nPlayerIdx)" + E +
    "{" + E +
    T + "if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER) return;" + E +
    T + "KLuaScript* pScript = (KLuaScript*)g_GetScript(\"" + BS*2 + "script" + BS*2 +
    "partner" + BS*2 + "partner_talk.lua\");" + E +
    T + "if (!pScript || !pScript->m_LuaState) return;" + E +
    T + "Lua_PushNumber(pScript->m_LuaState, nPlayerIdx);" + E +
    T + "pScript->SetGlobalName((LPSTR)SCRIPT_PLAYERINDEX);" + E +
    T + "pScript->CallFunction((LPSTR)\"main\", 0, (LPSTR)\"\");" + E +
    "}" + E + E)
patch(os.path.join(SRC, "KPlayerPartner.cpp"),
      "void Partner_ProcessAI(int nNpcIdx)" + E + "{",
      talk_fn + "void Partner_ProcessAI(int nNpcIdx)" + E + "{",
      "void Partner_RunTalkScript(int nPlayerIdx)")

# AI: che do "chi di theo" (nut Follow tren thanh nhanh)
patch(os.path.join(SRC, "KPlayerPartner.cpp"),
      T + "int nTarget = 0;" + E +
      T + "if (pOwnerNpc->m_FightMode)" + E,
      T + "int nTarget = 0;" + E +
      T + "if (pOwnerNpc->m_FightMode && pSys->GetG(PTG_FIGHTMODE) != 1)" +
      T + "// [BDH-G4] 1 = chi theo" + E,
      "pSys->GetG(PTG_FIGHTMODE) != 1")

print("XONG b09")
