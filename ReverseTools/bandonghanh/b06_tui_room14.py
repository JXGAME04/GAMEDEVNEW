# -*- coding: utf-8 -*-
"""BAN DONG HANH - G3: tui dong hanh (room_partnerbag / pos_partnerbag = 25).

15 moi va theo khuon pos_affairitem (danh sach diem sua: BANGIAO muc 3):
  GameDataDef.h : room + pos + kich thuoc + UOC
  KItemList.cpp : Init / AddKIL / Remove / eContainer x3 / ExchangeItem / FindNumberInAll / ClearAll
  KCore.cpp     : PositionToRoom
  KPlayerPartner.h/.cpp : Partner_BagCellActive (gate o theo cap tui)
Idempotent (nhan [BDH-G3]) + neo duy nhat + thich ung CRLF/LF + sao luu .truoc_bdh_g3.
"""
import io
import os
import shutil

SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"
NHAN = "[BDH-G3]"
CR = chr(13)
LF = chr(10)


def doc(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()


def ghi(p, s):
    io.open(p, "w", encoding="latin-1", newline="").write(s)


DA_SUA = {}


def va(ten, neo, moi):
    p = os.path.join(SRC, ten)
    s = DA_SUA.get(p) or doc(p)
    if moi in s:
        print("  da co:", ten)
        DA_SUA[p] = s
        return
    n = s.count(neo)
    if n != 1:
        neo2 = neo.replace(CR, "")
        moi2 = moi.replace(CR, "")
        if s.count(neo2) == 1:
            neo, moi = neo2, moi2
        else:
            raise AssertionError((ten, n, neo[:70]))
    if p not in DA_SUA and not os.path.exists(p + ".truoc_bdh_g3"):
        shutil.copyfile(p, p + ".truoc_bdh_g3")
    DA_SUA[p] = s.replace(neo, moi, 1)
    print("  OK:", ten)


E = CR + LF
T = chr(9)

# ============================= GameDataDef.h =============================
va("GameDataDef.h",
   T + "room_affairitem," + T + "//12" + E + T + "room_num,",
   T + "room_affairitem," + T + "//12" + E +
   T + "room_partnerbag," + T + "//13 " + NHAN + " tui ban dong hanh (Linux room_partnerbag=14)" + E +
   T + "room_num,")

va("GameDataDef.h",
   T + "pos_equipback," + T + T + "// equip 2" + E + T + "pos_num,",
   T + "pos_equipback," + T + T + "// equip 2" + E +
   T + "pos_partnerbag," + T + "// =25 " + NHAN + " tui ban dong hanh (them CUOI - giu gia tri cu)" + E +
   T + "pos_num,")

va("GameDataDef.h",
   "#define" + T + T + "AFFAIRITEM_ROOM_WIDTH" + T + T + "6",
   "// " + NHAN + " tui ban dong hanh 6x10 (partner_bag.ini Level_10=6,10; o mo theo cap tui)" + E +
   "#define" + T + T + "PARTNERBAG_ROOM_WIDTH" + T + T + "6" + E +
   "#define" + T + T + "PARTNERBAG_ROOM_HEIGHT" + T + T + "10" + E +
   "#define" + T + T + "AFFAIRITEM_ROOM_WIDTH" + T + T + "6")

va("GameDataDef.h",
   T + "UOC_GAMBLE_OTHER,",
   T + "UOC_GAMBLE_OTHER,\t\t//On the mutual gambling panel, others use it for gambling" + E +
   T + "UOC_PARTNER_BAG," + T + "// " + NHAN + " tui ban dong hanh")
# xoa dong comment cu bi lap (neo tren nuot comment goc) - dung neo phu:
# (comment goc nam SAU UOC_GAMBLE_OTHER, da chep lai o dong moi -> xoa ban goc)

# ============================= KItemList.cpp =============================
# 1. Init
va("KItemList.cpp",
   T + "m_Room[room_affairitem].Init(AFFAIRITEM_ROOM_WIDTH, AFFAIRITEM_ROOM_HEIGHT);",
   T + "m_Room[room_affairitem].Init(AFFAIRITEM_ROOM_WIDTH, AFFAIRITEM_ROOM_HEIGHT);" + E +
   T + "m_Room[room_partnerbag].Init(PARTNERBAG_ROOM_WIDTH, PARTNERBAG_ROOM_HEIGHT);" + T + "// " + NHAN)

# 2. AddKIL (khong gate - duong nap DB/sync phai luon nhan)
va("KItemList.cpp",
   T + "case pos_affairitem:" + E +
   T + T + "if (!m_Room[room_affairitem].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))" + E +
   T + T + T + "return 0;" + E +
   T + T + "m_Items[i].nPlace = pos_affairitem;" + E +
   T + T + "m_Items[i].nX = nX;" + E +
   T + T + "m_Items[i].nY = nY;" + T + T + E +
   T + T + "break;",
   T + "case pos_affairitem:" + E +
   T + T + "if (!m_Room[room_affairitem].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))" + E +
   T + T + T + "return 0;" + E +
   T + T + "m_Items[i].nPlace = pos_affairitem;" + E +
   T + T + "m_Items[i].nX = nX;" + E +
   T + T + "m_Items[i].nY = nY;" + T + T + E +
   T + T + "break;" + E +
   T + "case pos_partnerbag:" + T + "// " + NHAN + " tui ban dong hanh" + E +
   T + T + "if (!m_Room[room_partnerbag].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))" + E +
   T + T + T + "return 0;" + E +
   T + T + "m_Items[i].nPlace = pos_partnerbag;" + E +
   T + T + "m_Items[i].nX = nX;" + E +
   T + T + "m_Items[i].nY = nY;" + E +
   T + T + "break;")

# 3. Remove/PickUp
va("KItemList.cpp",
   T + "case pos_affairitem:" + E +
   T + T + "m_Room[room_affairitem].PickUpItem(" + E +
   T + T + T + "nGameIdx," + E +
   T + T + T + "m_Items[nIdx].nX," + E +
   T + T + T + "m_Items[nIdx].nY," + E +
   T + T + T + "Item[m_Items[nIdx].nIdx].GetWidth()," + E +
   T + T + T + "Item[m_Items[nIdx].nIdx].GetHeight());" + E +
   T + T + "break;",
   T + "case pos_affairitem:" + E +
   T + T + "m_Room[room_affairitem].PickUpItem(" + E +
   T + T + T + "nGameIdx," + E +
   T + T + T + "m_Items[nIdx].nX," + E +
   T + T + T + "m_Items[nIdx].nY," + E +
   T + T + T + "Item[m_Items[nIdx].nIdx].GetWidth()," + E +
   T + T + T + "Item[m_Items[nIdx].nIdx].GetHeight());" + E +
   T + T + "break;" + E +
   T + "case pos_partnerbag:" + T + "// " + NHAN + E +
   T + T + "m_Room[room_partnerbag].PickUpItem(" + E +
   T + T + T + "nGameIdx," + E +
   T + T + T + "m_Items[nIdx].nX," + E +
   T + T + T + "m_Items[nIdx].nY," + E +
   T + T + T + "Item[m_Items[nIdx].nIdx].GetWidth()," + E +
   T + T + T + "Item[m_Items[nIdx].nIdx].GetHeight());" + E +
   T + T + "break;")

# 4. eContainer (don item, :851)
va("KItemList.cpp",
   T + "case pos_affairitem:" + E +
   T + T + "pInfo.Region.h = m_Items[nIdx].nX;" + E +
   T + T + "pInfo.Region.v = m_Items[nIdx].nY;" + E +
   T + T + "pInfo.eContainer = UOC_AFFAIR_ITEM;" + E +
   T + T + "break;",
   T + "case pos_affairitem:" + E +
   T + T + "pInfo.Region.h = m_Items[nIdx].nX;" + E +
   T + T + "pInfo.Region.v = m_Items[nIdx].nY;" + E +
   T + T + "pInfo.eContainer = UOC_AFFAIR_ITEM;" + E +
   T + T + "break;" + E +
   T + "case pos_partnerbag:" + T + "// " + NHAN + E +
   T + T + "pInfo.Region.h = m_Items[nIdx].nX;" + E +
   T + T + "pInfo.Region.v = m_Items[nIdx].nY;" + E +
   T + T + "pInfo.eContainer = UOC_PARTNER_BAG;" + E +
   T + T + "break;")

# 5. eContainer (tim cho, :539)
va("KItemList.cpp",
   T + "case pos_affairitem:" + E +
   T + T + "pInfo.Region.h = nX;" + E +
   T + T + "pInfo.Region.v = nY;" + E +
   T + T + "pInfo.eContainer = UOC_AFFAIR_ITEM;" + E +
   T + T + "break;",
   T + "case pos_affairitem:" + E +
   T + T + "pInfo.Region.h = nX;" + E +
   T + T + "pInfo.Region.v = nY;" + E +
   T + T + "pInfo.eContainer = UOC_AFFAIR_ITEM;" + E +
   T + T + "break;" + E +
   T + "case pos_partnerbag:" + T + "// " + NHAN + E +
   T + T + "pInfo.Region.h = nX;" + E +
   T + T + "pInfo.Region.v = nY;" + E +
   T + T + "pInfo.eContainer = UOC_PARTNER_BAG;" + E +
   T + T + "break;")

# 6. eContainer cap (ExchangeItem sync, :3869)
va("KItemList.cpp",
   T + T + "case pos_affairitem:" + E +
   T + T + T + "pInfo1.Region.h = SrcPos->nX;" + E +
   T + T + T + "pInfo1.Region.v = SrcPos->nY;" + E +
   T + T + T + "pInfo2.Region.h = DesPos->nX;" + E +
   T + T + T + "pInfo2.Region.v = DesPos->nY;" + E +
   T + T + T + "pInfo1.eContainer = UOC_AFFAIR_ITEM;" + E +
   T + T + T + "pInfo2.eContainer = UOC_AFFAIR_ITEM;" + E +
   T + T + T + "break;",
   T + T + "case pos_affairitem:" + E +
   T + T + T + "pInfo1.Region.h = SrcPos->nX;" + E +
   T + T + T + "pInfo1.Region.v = SrcPos->nY;" + E +
   T + T + T + "pInfo2.Region.h = DesPos->nX;" + E +
   T + T + T + "pInfo2.Region.v = DesPos->nY;" + E +
   T + T + T + "pInfo1.eContainer = UOC_AFFAIR_ITEM;" + E +
   T + T + T + "pInfo2.eContainer = UOC_AFFAIR_ITEM;" + E +
   T + T + T + "break;" + E +
   T + T + "case pos_partnerbag:" + T + "// " + NHAN + E +
   T + T + T + "pInfo1.Region.h = SrcPos->nX;" + E +
   T + T + T + "pInfo1.Region.v = SrcPos->nY;" + E +
   T + T + T + "pInfo2.Region.h = DesPos->nX;" + E +
   T + T + T + "pInfo2.Region.v = DesPos->nY;" + E +
   T + T + T + "pInfo1.eContainer = UOC_PARTNER_BAG;" + E +
   T + T + T + "pInfo2.eContainer = UOC_PARTNER_BAG;" + E +
   T + T + T + "break;")

# 7. FindNumberInAll (antihack)
va("KItemList.cpp",
   T + "nNumberArray += m_Room[room_affairitem].FindNumberArrayItem(nIdx);",
   T + "nNumberArray += m_Room[room_affairitem].FindNumberArrayItem(nIdx);" + E +
   T + "nNumberArray += m_Room[room_partnerbag].FindNumberArrayItem(nIdx);" + T + "// " + NHAN)

# 8. ClearAll
va("KItemList.cpp",
   T + "ClearRoom(room_immediacy);" + E + T + "ClearRoom(room_affairitem);",
   T + "ClearRoom(room_immediacy);" + E + T + "ClearRoom(room_affairitem);" + E +
   T + "ClearRoom(room_partnerbag);" + T + "// " + NHAN)

# 9. ExchangeItem: case moi theo khuon pos_affairitem + gate cap tui (server)
va("KItemList.cpp",
   T + "case pos_tremble:" + T + "//--------------------------- kh",
   T + "case pos_partnerbag:" + T + "//---------------- " + NHAN + " tui ban dong hanh ----------------" + E +
   "#ifdef _SERVER" + E +
   T + T + "// o dich phai nam trong vung da kich hoat theo cap tui (partner_bag.ini)" + E +
   T + T + "if (m_Hand && !Partner_BagCellActive(m_PlayerIdx, DesPos->nX, DesPos->nY))" + E +
   T + T + T + "return;" + E +
   "#endif" + E +
   T + T + "nEquipIdx1 = m_Room[room_partnerbag].FindItem(SrcPos->nX, SrcPos->nY);" + E +
   T + T + "if (nEquipIdx1 < 0)" + E +
   T + T + T + "return;" + E +
   T + T + "if (nEquipIdx1)" + E +
   T + T + "{" + E +
   T + T + T + "if (!m_Room[room_partnerbag].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))" + E +
   T + T + T + T + "return;" + E +
   T + T + "}" + E +
   T + T + "if (m_Hand)" + E +
   T + T + "{" + E +
   T + T + T + "if (Item[m_Hand].CanStack(nEquipIdx1, m_Hand))" + E +
   T + T + T + "{" + E +
   "#ifdef _SERVER" + E +
   T + T + T + T + "g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));" + E +
   "#endif" + E +
   T + T + T + T + "if (Item[nEquipIdx1].Stack(Item[m_Hand].GetStackNum()))" + E +
   T + T + T + T + "{" + E +
   T + T + T + T + T + "Item[m_Hand].Remove();" + E +
   T + T + T + T + T + "Remove(m_Hand);" + E +
   T + T + T + T + "}" + E +
   T + T + T + T + "else" + E +
   T + T + T + T + "{" + E +
   T + T + T + T + T + "Item[m_Hand].SetStackNum(Item[nEquipIdx1].GetStackNum() - Def_MAX_STACK_TIENDONG);" + E +
   T + T + T + T + T + "Item[nEquipIdx1].SetStackNum(Def_MAX_STACK_TIENDONG);" + E +
   T + T + T + T + "}" + E +
   T + T + T + T + "m_Room[room_partnerbag].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());" + E +
   T + T + T + T + "return;" + E +
   T + T + T + "}" + E +
   T + T + "}" + E +
   T + T + "if (m_Hand)" + E +
   T + T + "{" + E +
   T + T + T + "if (m_Room[room_partnerbag].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))" + E +
   T + T + T + "{" + E +
   T + T + T + T + "int nListIdx = FindSame(m_Hand);" + E +
   T + T + T + T + "m_Items[nListIdx].nPlace = pos_partnerbag;" + E +
   T + T + T + T + "m_Items[nListIdx].nX = DesPos->nX;" + E +
   T + T + T + T + "m_Items[nListIdx].nY = DesPos->nY;" + E +
   T + T + T + T + "m_Hand = nEquipIdx1;" + E +
   T + T + T + T + "if (nEquipIdx1)" + E +
   T + T + T + T + T + "m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;" + E +
   "#ifdef _SERVER" + E +
   T + T + T + T + "g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));" + E +
   "#endif" + E +
   T + T + T + "}" + E +
   T + T + T + "else" + E +
   T + T + T + "{" + E +
   T + T + T + T + "m_Room[room_partnerbag].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());" + E +
   T + T + T + "}" + E +
   T + T + "}" + E +
   T + T + "else" + E +
   T + T + "{" + E +
   T + T + T + "int nListIdx = FindSame(nEquipIdx1);" + E +
   T + T + T + "if (nEquipIdx1 && nListIdx)" + E +
   T + T + T + "{" + E +
   T + T + T + T + "m_Items[nListIdx].nPlace = pos_hand;" + E +
   T + T + T + T + "m_Hand = nEquipIdx1;" + E +
   T + T + T + "}" + E +
   "#ifdef _SERVER" + E +
   T + T + T + "g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));" + E +
   "#endif" + E +
   T + T + "}" + E +
   T + T + "break;" + E +
   E +
   T + "case pos_tremble:" + T + "//--------------------------- kh")

# ============================= KCore.cpp =============================
va("KCore.cpp",
   T + T + "case pos_affairitem:" + E + T + T + T + "return room_affairitem;",
   T + T + "case pos_affairitem:" + E + T + T + T + "return room_affairitem;" + E +
   T + T + "case pos_partnerbag:" + T + "// " + NHAN + E +
   T + T + T + "return room_partnerbag;")

for p, s in DA_SUA.items():
    ghi(p, s)
print("XONG b06 (%d tep)" % len(DA_SUA))
