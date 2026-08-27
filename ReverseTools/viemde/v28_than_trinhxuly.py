# -*- coding: utf-8 -*-
"""v28 - viet THAN hai trinh xu ly giao thuc xuc xac trong KProtocolProcess.cpp.

Sau v24 hai ham moi duoc KHAI BAO + DANG KY, chua co than -> build bao LNK2019.
Ban va nay bo sung:
  * ben CLIENT: KProtocolProcess::s2cDiceItem  (dat canh s2cSyncBauCuaResult)
  * ben MAY CHU: KProtocolProcess::c2sDiceItem (dat ngay sau c2sBauCua)

Vi tri dat khop voi thu tu dang ky trong cung tep - xem bai hoc o memory
'protocol-add-order' (26/08: chu game bat loi .h lech thu tu .cpp).

KProtocolProcess.cpp la tep CRLF.
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

P = r"D:\GAMEDEVNEW\Sources\Core\Src\KProtocolProcess.cpp"
HAU_TO = ".truoc_xucxac_2608"

# ---------------------------------------------------------------- CLIENT
NEO_CLIENT = (
    "void KProtocolProcess::s2cSyncBauCuaResult(BYTE* pMsg)\r\n"
    "{\r\n"
    "\tBAUCUA_RESULT_SYNC* pBauCuaResultSync = (BAUCUA_RESULT_SYNC*)pMsg;\r\n"
    "\tif (pBauCuaResultSync->ProtocolType != s2c_syncbaucuaresult && "
    "pBauCuaResultSync->ProtocolType != s2c_syncbaucuainfo)\r\n"
    "\t\treturn;\r\n"
    "\tCoreDataChanged(GDCNI_PLAYER_BAUCUA_RESULT_SYNC, (unsigned int)pBauCuaResultSync, "
    "pBauCuaResultSync->ProtocolType);\r\n"
    "}\r\n"
)

THAN_CLIENT = NEO_CLIENT + (
    "\r\n"
    "// He XUC XAC chia do (DICEITEM) - may chu bao client mo/dong/cap nhat mot o.\r\n"
    "// Chi chuyen tiep len lop giao dien; con tro CHI SONG trong loi goi nay nen ben\r\n"
    "// nhan phai chep ra, khong duoc giu lai (y het s2cSyncBauCuaResult).\r\n"
    "void KProtocolProcess::s2cDiceItem(BYTE* pMsg)\r\n"
    "{\r\n"
    "\tDICE_ITEM_SYNC* pSync = (DICE_ITEM_SYNC*)pMsg;\r\n"
    "\tif (pSync->ProtocolType != s2c_diceitem)\r\n"
    "\t\treturn;\r\n"
    "\tCoreDataChanged(GDCNI_DICE_ITEM, (unsigned int)pSync, 0);\r\n"
    "}\r\n"
)

# ---------------------------------------------------------------- MAY CHU
NEO_SERVER = "void KProtocolProcess::c2sSetImage(int nIndex, BYTE* pProtocol)\r\n"

THAN_SERVER = (
    "// He XUC XAC chia do (DICEITEM): nguoi choi bam \"Tham du nhan\" / \"Huy bo nhan\".\r\n"
    "// Do dai goi da duoc CoreServerShell::CheckProtocolSize chan TRUOC khi vao day\r\n"
    "// (tra bang g_nProtocolSize, o c2s_diceitem = sizeof(DICE_CHOICE_DATA)), nen o\r\n"
    "// day chi con phai kiem tinh hop le cua nguoi choi va cua phien.\r\n"
    "void KProtocolProcess::c2sDiceItem(int nIndex, BYTE* pProtocol)\r\n"
    "{\r\n"
    "\tif (!pProtocol)\r\n"
    "\t\treturn;\r\n"
    "\tif (nIndex <= 0 || nIndex >= MAX_PLAYER)\r\n"
    "\t\treturn;\r\n"
    "\tif (Player[nIndex].m_nIndex <= 0 || Player[nIndex].m_nIndex >= MAX_NPC)\r\n"
    "\t\treturn;\r\n"
    "\tif (Npc[Player[nIndex].m_nIndex].m_Kind != kind_player)\r\n"
    "\t\treturn;\r\n"
    "\r\n"
    "\tDICE_CHOICE_DATA* pInfo = (DICE_CHOICE_DATA*)pProtocol;\r\n"
    "\tif (pInfo->ProtocolType != c2s_diceitem)\r\n"
    "\t\treturn;\r\n"
    "\r\n"
    "\tKItemDice* pDice = g_ItemDiceSet.Find(pInfo->m_nDiceId);\r\n"
    "\tif (!pDice)\r\n"
    "\t\treturn;\t// phien da chot hoac ma bia - bo qua im lang\r\n"
    "\r\n"
    "\t// chi nhan dung 2 gia tri; moi thu khac coi la huy bo\r\n"
    "\tint nChoice = (pInfo->m_btChoice == DICE_CHOICE_NEED)\r\n"
    "\t\t? DICE_CHOICE_NEED : DICE_CHOICE_GIVEUP;\r\n"
    "\t// SetChoice tu chan bam hai lan va tu chot phien khi moi nguoi da chon\r\n"
    "\tpDice->SetChoice(nIndex, nChoice);\r\n"
    "}\r\n"
    "\r\n") + NEO_SERVER


def main():
    d = io.open(P, "rb").read().decode("latin-1")

    for nhan, cu, moi in (("CLIENT s2cDiceItem", NEO_CLIENT, THAN_CLIENT),
                          ("MAY CHU c2sDiceItem", NEO_SERVER, THAN_SERVER)):
        if "KProtocolProcess::" + nhan.split()[1] in d:
            print("   = da co than:", nhan)
            continue
        n = d.count(cu)
        if n != 1:
            print("!! MO NEO khong duy nhat (%d lan): %s" % (n, nhan))
            return 2

    goc = d
    for nhan, cu, moi in (("CLIENT s2cDiceItem", NEO_CLIENT, THAN_CLIENT),
                          ("MAY CHU c2sDiceItem", NEO_SERVER, THAN_SERVER)):
        ten = nhan.split()[1]
        if ("KProtocolProcess::" + ten) in d and ("::" + ten + "(") in d and \
           d.count("KProtocolProcess::" + ten) > 1:
            continue
        d = d.replace(cu, moi, 1)
        print("   > da them than:", nhan)

    b = d.encode("latin-1")
    if b.count(b"\n") - b.count(b"\r\n"):
        print("!! sinh ra dong LF le")
        return 2

    bak = P + HAU_TO
    if not os.path.isfile(bak):
        shutil.copy2(P, bak)
    tmp = P + ".tmp"
    io.open(tmp, "wb").write(b)
    os.replace(tmp, P)

    print()
    for ten in ("s2cDiceItem", "c2sDiceItem"):
        print("   %-14s khai bao+dang ky+than: %d lan xuat hien"
              % (ten, d.count(ten)))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
