# -*- coding: utf-8 -*-
r"""v37 - NUT BAM PHIA MAY CHU cho cua so lo ren (UILOREN).

Mau song la kham do xanh (TrembleItem):
  client SendUiCmdScript(5, "TrembleItem")
    -> KProtocolProcess::UiCommandScript case 5
    -> ExecuteScript(m_dwTrembleItemId, m_szTaskExcuteFun, "")
  m_dwTrembleItemId duoc LuaOpenTrembleItem luu tu Npc[...].m_ActionScriptID.

Goi va nay lam y het cho cua so lo ren, dung case 7 (doc that: case 1..6 da co
chu, case 7 chua ai dung):

  1. KPlayer.h    : them DWORD m_dwCompoundItemId + char m_szCompoundFun[64]
                    ngay canh m_dwTrembleItemId (khong dung TRoleData - chi la
                    thanh vien runtime cua KPlayer, khong len day/khong luu DB).
  2. KPlayer.cpp  : reset 2 truong moi tai dung cho m_dwTrembleItemId = 0
                    (khe player duoc tai su dung - khong reset la ro id cu).
  3. ScriptFuns.cpp LuaOpenCompoundItem: bo sung theo khuon LuaOpenTrembleItem
                    (luu script id hien tai + ten ham callback neu co doi so);
                    van gui op 1 byte s2c_opencompounditem nhu cu.
  4. ScriptFuns.cpp LuaEndCompoundItem (moi): xoa id. Op mo cua so chi co 1 byte
                    (khong co m_nType nhu OPEN_TREMBLEITEM) nen khong co goi
                    dong rieng - chi xoa phia may chu.
                    Dang ky "EndCompoundItem" ngay duoi "OpenCompoundItem".
  5. ScriptFuns.cpp LuaGetIdItem: mo case 4..9 (dang bi chu thich) tra chi so
                    item toan cuc trong 6 phong qua getter GetCompOneItem/...
                    (cac getter nay do v36 khai trong KItemList.h - PHAI ap v36
                    truoc khi build). nSocket kep theo kich thuoc mang that
                    8/8/8/11/2/11 (chi so lon nhat 7/7/7/10/1/10) + chan am.
  6. KProtocolProcess.cpp UiCommandScript: them case 7 TRUOC default - chi cho
                    goi 6 ham trong DANH SACH TRANG {"LR_UI_MotOre", ...}.
                    Khac tremble: KHONG xoa m_dwCompoundItemId sau moi lan goi
                    (cua so lo ren bam nhieu lan; EndCompoundItem/logout xoa).
                    Kem 1 dong "break;" cho case 5: truoc day case 5 roi thang
                    xuong default nhung default chi break nen hanh vi KHONG doi;
                    phai chan de case 5 khoi lot vao case 7 moi.

Cap doi voi: ui_loren/compound_ui.lua (6 ham callback + LR_UI_Mo).

MAC DINH DIEN TAP - chi ghi khi co --ghi. Sao luu <tep>.truoc_uiloren truoc
khi ghi lan dau. Moi mieng va idempotent: chuoi THAY da co thi bo qua; moc neo
phai dung 1 lan, khac la BAO LOI va khong ghi tep do.
"""
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

GOC = r"D:\GAMEDEVNEW\Sources\Core\Src"
KPLAYER_H = os.path.join(GOC, "KPlayer.h")
KPLAYER_CPP = os.path.join(GOC, "KPlayer.cpp")
SCRIPTFUNS = os.path.join(GOC, "ScriptFuns.cpp")
KPROTOPROC = os.path.join(GOC, "KProtocolProcess.cpp")
KITEMLIST_H = os.path.join(GOC, "KItemList.h")
DUOI = ".truoc_uiloren"


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def dem_dong(t):
    crlf = t.count("\r\n")
    return crlf, t.count("\n") - crlf


def theo_eol(t, s):
    crlf, lf = dem_dong(t)
    s = s.replace("\r\n", "\n")
    if crlf > lf:
        s = s.replace("\n", "\r\n")
    return s


# ---------------------------------------------------------------- KPlayer.h
# Moc neo CHI phan ASCII cua dong 702 (phan chu thich TCVN3 "kham nam" nam sau
# dau ';' duoc giu nguyen, khong cham toi).
VA_KPLAYER_H = [
    ("member moi",
     "\n\tDWORD\t\t\tm_dwTrembleItemId;",
     "\n\tDWORD\t\t\tm_dwCompoundItemId;\t\t// [UILOREN] script id giu cua so lo ren (khuon m_dwTrembleItemId)"
     "\n\tchar\t\t\tm_szCompoundFun[64];\t// [UILOREN] ten ham callback do OpenCompoundItem dang ky"
     "\n\tDWORD\t\t\tm_dwTrembleItemId;"),
]

# -------------------------------------------------------------- KPlayer.cpp
VA_KPLAYER_CPP = [
    ("reset khi login",
     "\n\tm_dwTrembleItemId = 0;\n",
     "\n\tm_dwTrembleItemId = 0;\n"
     "\tm_dwCompoundItemId = 0;\t\t// [UILOREN] reset khi khe player tai su dung\n"
     "\tm_szCompoundFun[0] = 0;\t\t// [UILOREN]\n"),
]

# ------------------------------------------------------------ ScriptFuns.cpp
THAY_SF_OPEN = (
    "\t// [UILOREN] luu script id dang chay de nut bam tren cua so goi nguoc ve\n"
    "\t// (khuon LuaOpenTrembleItem). Doi so 1 (neu co) la ten ham callback.\n"
    "\tPlayer[nPlayerIndex].m_dwCompoundItemId = Npc[Player[nPlayerIndex].m_nIndex].m_ActionScriptID;\n"
    "\tPlayer[nPlayerIndex].m_szCompoundFun[0] = 0;\n"
    "\tif (Lua_GetTopIndex(L) >= 1 && Lua_IsString(L, 1))\n"
    "\t{\n"
    "\t\tstrncpy(Player[nPlayerIndex].m_szCompoundFun, Lua_ValueToString(L, 1), sizeof(Player[nPlayerIndex].m_szCompoundFun) - 1);\n"
    "\t\tPlayer[nPlayerIndex].m_szCompoundFun[sizeof(Player[nPlayerIndex].m_szCompoundFun) - 1] = 0;\n"
    "\t}\n"
    "\n"
    "\tBYTE\tNetCommand = (BYTE)s2c_opencompounditem;")

THAY_SF_END = (
    "\n"
    "// [UILOREN] dong phien lo ren: xoa script id dang giu. Op mo cua so\n"
    "// s2c_opencompounditem chi co 1 byte (khong co m_nType nhu tremble) nen\n"
    "// khong co goi dong rieng phia client - chi xoa id phia may chu.\n"
    "int LuaEndCompoundItem(Lua_State* L)\n"
    "{\n"
    "\tint nPlayerIndex = GetPlayerIndex(L);\n"
    "\tif (nPlayerIndex <= 0)\n"
    "\t\treturn 0;\n"
    "\n"
    "\tPlayer[nPlayerIndex].m_dwCompoundItemId = 0;\n"
    "\tPlayer[nPlayerIndex].m_szCompoundFun[0] = 0;\n"
    "\treturn 0;\n"
    "}\n"
    "\n"
    "int LuaGetIdItem(Lua_State* L)\n"
    "{")


def lam_case(n_case, clamp_cu, getter, clamp_moi, duoi):
    """Mieng va cho 1 case cua LuaGetIdItem.

    duoi: cases 5..9 cua ban goc co MOT DAU CACH thua sau '));' - phai giu
    dung tung byte trong moc neo (doc that bang cat -A ngay 27/08).
    """
    tim = ("\tcase %d:\n"
           "\t\tif (nSocket > %d)\n"
           "\t\t{\n"
           "\t\t\tnSocket = %d;\n"
           "\t\t}\n"
           "\t\t//Lua_PushNumber(L,Player[nPlayerIndex].m_ItemList.%s(nSocket));%s\n"
           "\t\tbreak;" % (n_case, clamp_cu, clamp_cu, getter, duoi))
    thay = ("\tcase %d:\n"
            "\t\tif (nSocket < 0)\n"
            "\t\t{\n"
            "\t\t\tnSocket = 0;\n"
            "\t\t}\n"
            "\t\tif (nSocket > %d)\n"
            "\t\t{\n"
            "\t\t\tnSocket = %d;\n"
            "\t\t}\n"
            "\t\tLua_PushNumber(L, Player[nPlayerIndex].m_ItemList.%s(nSocket));\t// [UILOREN] 0 = o rong\n"
            "\t\tbreak;" % (n_case, clamp_moi, clamp_moi, getter))
    return ("GetIdItem case %d" % n_case, tim, thay)


VA_SCRIPTFUNS = [
    ("OpenCompoundItem luu id",
     "\tBYTE\tNetCommand = (BYTE)s2c_opencompounditem;",
     THAY_SF_OPEN),
    ("LuaEndCompoundItem",
     "\nint LuaGetIdItem(Lua_State* L)\n{",
     THAY_SF_END),
    ("dang ky EndCompoundItem",
     "\t{\"OpenCompoundItem\", LuaOpenCompoundItem},",
     "\t{\"OpenCompoundItem\", LuaOpenCompoundItem},\n"
     "\t{\"EndCompoundItem\", LuaEndCompoundItem},"),
    # kich thuoc mang that (v36): CompOne/Two/Three=8, Distill=11, Forge=2,
    # Enchase=11  ->  chi so lon nhat 7/7/7/10/1/10
    lam_case(4, 3, "GetCompOneItem", 7, ""),
    lam_case(5, 3, "GetCompTwoItem", 7, " "),
    lam_case(6, 3, "GetCompThreeItem", 7, " "),
    lam_case(7, 11, "GetDistillItem", 10, " "),
    lam_case(8, 2, "GetForgeItem", 1, " "),
    lam_case(9, 11, "GetEnchaseItem", 10, " "),
]

# ------------------------------------------------------- KProtocolProcess.cpp
TIM_KP = (
    "\t\t\t\tPlayer[nIndex].ExecuteScript(Player[nIndex].m_dwTrembleItemId, Player[nIndex].m_szTaskExcuteFun, \"\");\n"
    "\t\t\t\tPlayer[nIndex].m_dwTrembleItemId = 0;\n"
    "\t\t\t}\n"
    "\t\tdefault:")

THAY_KP = (
    "\t\t\t\tPlayer[nIndex].ExecuteScript(Player[nIndex].m_dwTrembleItemId, Player[nIndex].m_szTaskExcuteFun, \"\");\n"
    "\t\t\t\tPlayer[nIndex].m_dwTrembleItemId = 0;\n"
    "\t\t\t}\n"
    "\t\t\t// [UILOREN] break nay khong doi hanh vi cu (case 5 von roi xuong\n"
    "\t\t\t// default, ma default chi break) - chi chan khoi lot vao case 7 moi.\n"
    "\t\t\tbreak;\n"
    "\t\tcase 7:\t// [UILOREN] nut bam cua so lo ren - chi cho ham trong danh sach trang\n"
    "\t\t\tif (Player[nIndex].m_dwCompoundItemId > 0)\n"
    "\t\t\t{\n"
    "\t\t\t\tstatic const char* s_szLRFuns[] =\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\t\"LR_UI_MotOre\", \"LR_UI_HaiOre\", \"LR_UI_BaOre\",\n"
    "\t\t\t\t\t\"LR_UI_Distill\", \"LR_UI_Forge\", \"LR_UI_Enchase\",\n"
    "\t\t\t\t};\n"
    "\t\t\t\tchar szFun[sizeof(pUiCmd->szFunc) + 1];\n"
    "\t\t\t\tmemcpy(szFun, pUiCmd->szFunc, sizeof(pUiCmd->szFunc));\n"
    "\t\t\t\tszFun[sizeof(pUiCmd->szFunc)] = 0;\t// chan chuoi khong ket thuc tu client\n"
    "\t\t\t\tfor (int nLR = 0; nLR < (int)(sizeof(s_szLRFuns) / sizeof(s_szLRFuns[0])); nLR++)\n"
    "\t\t\t\t{\n"
    "\t\t\t\t\tif (!strcmp(szFun, s_szLRFuns[nLR]))\n"
    "\t\t\t\t\t{\n"
    "\t\t\t\t\t\t// khong xoa m_dwCompoundItemId: cua so bam nhieu lan,\n"
    "\t\t\t\t\t\t// EndCompoundItem hoac reset login se xoa\n"
    "\t\t\t\t\t\tPlayer[nIndex].ExecuteScript(Player[nIndex].m_dwCompoundItemId, szFun, \"\");\n"
    "\t\t\t\t\t\tbreak;\n"
    "\t\t\t\t\t}\n"
    "\t\t\t\t}\n"
    "\t\t\t}\n"
    "\t\t\tbreak;\n"
    "\t\tdefault:")

VA_KPROTOPROC = [
    ("UiCommandScript case 7", TIM_KP, THAY_KP),
]

BO_VA = [
    (KPLAYER_H, VA_KPLAYER_H),
    (KPLAYER_CPP, VA_KPLAYER_CPP),
    (SCRIPTFUNS, VA_SCRIPTFUNS),
    (KPROTOPROC, VA_KPROTOPROC),
]


def va_mot_tep(duong, cap, ghi):
    ten = os.path.basename(duong)
    goc = doc(duong)
    t = goc
    loi = 0
    doi = 0
    for nhan, tim, thay in cap:
        a = theo_eol(t, tim)
        b = theo_eol(t, thay)
        if b in t:
            print("  BO QUA  %-28s da co san" % nhan)
            continue
        n = t.count(a)
        if n != 1:
            print("  *** LOI %-28s moc neo xuat hien %d lan (phai dung 1)" % (nhan, n))
            loi += 1
            continue
        t = t.replace(a, b, 1)
        doi += 1
        print("  OK      %-28s" % nhan)
    if loi:
        print("  >> %s: CO LOI MOC NEO - KHONG GHI tep nay" % ten)
        return loi, 0
    if doi == 0:
        print("  >> %s: khong co gi de doi" % ten)
        return 0, 0
    c0, l0 = dem_dong(goc)
    c1, l1 = dem_dong(t)
    if min(c0, l0) == 0 and min(c1, l1) > 0:
        print("  *** LOI %s: sinh them loai xuong dong la (CRLF %d->%d, LF %d->%d) - KHONG GHI"
              % (ten, c0, c1, l0, l1))
        return 1, 0
    if ghi:
        sao = duong + DUOI
        if not os.path.isfile(sao):
            io.open(sao, "wb").write(goc.encode("latin-1"))
        io.open(duong, "wb").write(t.encode("latin-1"))
        print("  >> DA GHI %s (%d mieng; sao luu %s)" % (ten, doi, os.path.basename(sao)))
    else:
        print("  >> DIEN TAP %s: %d mieng se duoc va (CRLF %d->%d, LF %d->%d)"
              % (ten, doi, c0, c1, l0, l1))
    return 0, doi


def main():
    ghi = "--ghi" in sys.argv
    print("v37 NUT BAM PHIA MAY CHU (UILOREN)  %s" % ("[GHI THAT]" if ghi else "[DIEN TAP]"))
    print("=" * 78)

    for p in (KPLAYER_H, KPLAYER_CPP, SCRIPTFUNS, KPROTOPROC):
        if not os.path.isfile(p):
            print("  *** THIEU TEP: %s" % p)
            return 1

    # canh bao phu thuoc v36: getter 6 phong phai co that trong KItemList.h
    # truoc khi BUILD (khong chan va - chi nhac).
    kil = doc(KITEMLIST_H)
    thieu = [g for g in ("GetCompOneItem", "GetCompTwoItem", "GetCompThreeItem",
                         "GetDistillItem", "GetForgeItem", "GetEnchaseItem")
             if g not in kil]
    if thieu:
        print("  !! CANH BAO: KItemList.h CHUA co getter %s" % ", ".join(thieu))
        print("     -> phai ap v36 (khai 6 getter) truoc khi build, khong thi C2039.")
    else:
        print("  getter 6 phong da co trong KItemList.h (v36 da ap)")

    tong_loi = 0
    tong_doi = 0
    for duong, cap in BO_VA:
        print("-" * 78)
        print("  TEP: %s" % duong)
        loi, doi = va_mot_tep(duong, cap, ghi)
        tong_loi += loi
        tong_doi += doi

    print("=" * 78)
    if tong_loi:
        print("  CO %d MUC LOI - kiem lai moc neo (khong tep loi nao duoc ghi)" % tong_loi)
        return 1
    print("  %s - tong %d mieng va" % ("DA GHI XONG" if ghi else "DIEN TAP DAT", tong_doi))
    if not ghi:
        print("  (chay lai voi --ghi de ghi that; ghi xong can build lai CoreServer)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
