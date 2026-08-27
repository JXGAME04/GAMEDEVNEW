# -*- coding: utf-8 -*-
"""v24 - chen giao thuc he XUC XAC (DICEITEM) vao dung thu tu luoi.

Chu game dan: "chen giao thuc phai dong bo theo thu tu tranh viec loi".
Da kiem bang so truoc khi sua (xem BANGIAO_VIEMDE_2608.md muc 11):

  c2s_diceitem = 172  (ngay sau c2s_baucua = 171)     -> o bang chi so 107
  s2c_diceitem = 214  (ngay sau s2c_removeallitem=213)-> o bang chi so 149

BAY DA GO: nua client cua g_nProtocolSize co MOT DONG CHET
`-1,  //s2c_dynamic_structure` (KProtocol.cpp:168) trong khi thanh vien enum do
DA BI CHU THICH (KProtocolDef.h:205) va trinh xu ly cung da bi chu thich
(KProtocolProcess.cpp:234). No dang CHIEM DUNG o chi so 149 - dung o ma
s2c_diceitem se nhan. Neu chi noi them vao cuoi thi:
    g_nProtocolSize[214-64-1] = [149] = -1  (bi hieu la goi DO DAI THAY DOI)
    phan tu moi roi vao [150] va KHONG BAO GIO duoc doc
=> Ban va nay THAY THE dong chet do bang muc cua s2c_diceitem, tra bang ve
   dung 1:1 voi enum (150 thanh vien / 150 phan tu).

Moi tep deu thuan CRLF va thut dau dong bang TAB - ban va giu nguyen ca hai.
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

CAY = r"D:\GAMEDEVNEW"
HAU_TO = ".truoc_xucxac_2608"

F_DEF = os.path.join(CAY, "Headers", "KProtocolDef.h")
F_H = os.path.join(CAY, "Sources", "Core", "Src", "KProtocol.h")
F_CPP = os.path.join(CAY, "Sources", "Core", "Src", "KProtocol.cpp")
F_PH = os.path.join(CAY, "Sources", "Core", "Src", "KProtocolProcess.h")
F_PC = os.path.join(CAY, "Sources", "Core", "Src", "KProtocolProcess.cpp")


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def ghi(p, s):
    """ghi an toan: sao luu -> tep tam -> doi cho. Giu nguyen byte."""
    bak = p + HAU_TO
    if not os.path.isfile(bak):
        shutil.copy2(p, bak)
    tmp = p + ".tmp"
    io.open(tmp, "wb").write(s.encode("latin-1"))
    os.replace(tmp, p)


def kiem_eol(p, s):
    b = s.encode("latin-1")
    le = b.count(b"\n") - b.count(b"\r\n")
    if le:
        raise SystemExit("!! %s co %d dong LF don - phai thuan CRLF" % (p, le))


VA = []


def them(p, cu, moi, nhan, sl=1):
    VA.append((p, cu, moi, nhan, sl))


# ------------------------------------------------------------------ 1. enum
them(F_DEF,
     "\ts2c_removeallitem,\r\n",
     "\ts2c_removeallitem,\r\n\ts2c_diceitem,\t\t\t// mo/cap nhat/dong o xuc xac chia do\r\n",
     "KProtocolDef.h: s2c_diceitem = 214 (ngay sau s2c_removeallitem)")

them(F_DEF,
     "\tc2s_baucua,\r\n",
     "\tc2s_baucua,\r\n\tc2s_diceitem,\t\t\t// nguoi choi chon: tham du nhan / huy bo\r\n",
     "KProtocolDef.h: c2s_diceitem = 172 (ngay sau c2s_baucua)")

# ------------------------------------------------------- 2. cau truc goi
CAU_TRUC = (
    "} VIEW_ITEM_SYNC;\r\n"
    "\r\n"
    "// ---- he XUC XAC chia do (DICEITEM) ----\r\n"
    "// Dung lai SViewSellItemInfo: day la ban mo ta vat pham \"khong so huu\" san\r\n"
    "// co cua JX1 (cua so mua ban dung chinh no), du de client ve bieu tuong va\r\n"
    "// chu giai ma khong phai them dinh dang moi.\r\n"
    "typedef struct\r\n"
    "{\r\n"
    "\tBYTE\t\t\t\tProtocolType;\t// s2c_diceitem\r\n"
    "\tBYTE\t\t\t\tm_btAction;\t\t// 0 = mo o, 1 = dong o, 2 = bao diem vua gieo\r\n"
    "\tint\t\t\t\t\tm_nDiceId;\t\t// ma phien xuc xac\r\n"
    "\tint\t\t\t\t\tm_nTimeLeft;\t// so giay con lai\r\n"
    "\tint\t\t\t\t\tm_nNumber;\t\t// diem vua gieo (dung khi m_btAction = 2)\r\n"
    "\tSViewSellItemInfo\tm_sItem;\t\t// mo ta vat pham dem ra chia\r\n"
    "} DICE_ITEM_SYNC;\r\n"
    "\r\n"
    "typedef struct\r\n"
    "{\r\n"
    "\tBYTE\tProtocolType;\t\t\t// c2s_diceitem\r\n"
    "\tint\t\tm_nDiceId;\t\t\t\t// ma phien xuc xac\r\n"
    "\tBYTE\tm_btChoice;\t\t\t\t// 0 = huy bo nhan, 1 = tham du nhan\r\n"
    "} DICE_CHOICE_DATA;\r\n"
)
them(F_H, "} VIEW_ITEM_SYNC;\r\n", CAU_TRUC,
     "KProtocol.h: DICE_ITEM_SYNC + DICE_CHOICE_DATA (sau VIEW_ITEM_SYNC)")

# --------------------------------------------------- 3. bang kich thuoc
# THAY THE dong chet - xem giai thich dau tep
them(F_CPP,
     "\t-1,\t\t\t\t//s2c_dynamic_structure\r\n",
     "\tsizeof(DICE_ITEM_SYNC),\t\t\t\t// s2c_diceitem\r\n",
     "KProtocol.cpp: THAY dong chet '-1 //s2c_dynamic_structure' bang o s2c_diceitem (chi so 149)")

them(F_CPP,
     "\tsizeof(BAUCUA_DATA),\t\t\t\t\t// c2s_baucua\r\n",
     "\tsizeof(BAUCUA_DATA),\t\t\t\t\t// c2s_baucua\r\n"
     "\tsizeof(DICE_CHOICE_DATA),\t\t\t\t// c2s_diceitem\r\n",
     "KProtocol.cpp: o c2s_diceitem (chi so 107, sau c2s_baucua)")

# ------------------------------------------------ 4. khai bao trinh xu ly
them(F_PH, "\tvoid\ts2cSyncBauCuaResult(BYTE* pMsg);\r\n",
     "\tvoid\ts2cSyncBauCuaResult(BYTE* pMsg);\r\n"
     "\tvoid\ts2cDiceItem(BYTE* pMsg);\r\n",
     "KProtocolProcess.h: khai bao s2cDiceItem")

them(F_PH, "\tvoid \tc2sBauCua(int nIndex, BYTE* pProtocol);\r\n",
     "\tvoid \tc2sBauCua(int nIndex, BYTE* pProtocol);\r\n"
     "\tvoid \tc2sDiceItem(int nIndex, BYTE* pProtocol);\r\n",
     "KProtocolProcess.h: khai bao c2sDiceItem")

# ------------------------------------------------------ 5. dang ky
them(F_PC,
     "\tProcessFunc[s2c_removeallitem] = &KProtocolProcess::s2cRemoveAllItem;\r\n",
     "\tProcessFunc[s2c_removeallitem] = &KProtocolProcess::s2cRemoveAllItem;\r\n"
     "\tProcessFunc[s2c_diceitem] = &KProtocolProcess::s2cDiceItem;\r\n",
     "KProtocolProcess.cpp: dang ky s2c_diceitem")

them(F_PC,
     "\tProcessFunc[c2s_baucua] = &KProtocolProcess::c2sBauCua;\r\n",
     "\tProcessFunc[c2s_baucua] = &KProtocolProcess::c2sBauCua;\r\n"
     "\tProcessFunc[c2s_diceitem] = &KProtocolProcess::c2sDiceItem;\r\n",
     "KProtocolProcess.cpp: dang ky c2s_diceitem")


def main():
    # gom theo tep
    theo_tep = {}
    for p, cu, moi, nhan, sl in VA:
        theo_tep.setdefault(p, []).append((cu, moi, nhan, sl))

    # kiem TRUOC khi sua bat cu gi
    noidung = {}
    for p, ds in theo_tep.items():
        if not os.path.isfile(p):
            print("!! khong co tep:", p)
            return 2
        d = doc(p)
        for cu, moi, nhan, sl in ds:
            n = d.count(cu)
            if n != sl:
                print("!! MO NEO KHONG DUNG (%d lan, can %d): %s" % (n, sl, nhan))
                print("   trong", p)
                return 2
            if moi in d:
                print("   = da co san, bo qua:", nhan)
        noidung[p] = d
    print("moi mo neo deu duy nhat - bat dau sua\n")

    for p, ds in theo_tep.items():
        d = noidung[p]
        for cu, moi, nhan, sl in ds:
            if moi in d:
                continue
            d = d.replace(cu, moi, 1)
            print("   > " + nhan)
        kiem_eol(p, d)
        ghi(p, d)
    print("\nda sua %d tep (sao luu duoi '%s')" % (len(theo_tep), HAU_TO))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
