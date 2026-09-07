# -*- coding: utf-8 -*-
# [BOTNOI dot 4b 06/09 - KHAN] Chu game: "bot nem do ra luc doi do thi toi nhat vao hien thi loi item
# roi toi out ra vao lai thi mat do" + "bug lam mat do nguoi choi that?".
#
# GOC (KItemList.cpp): bot mac do bang InsertEquipment + Equip. InsertEquipment khi tui KHONG con
# dai o (mon khoa: thu KHO truoc) thi dat mon vao TAY (pos_hand) va NEM MON DANG O TAY xuong dat
# thanh Object (KItemList.cpp ~4620-4668). Equip (1252) doi nPlace sang pos_equip nhung KHONG XOA
# m_Hand -> m_Hand tro vao mon DA MAC. Lan InsertEquipment sau (vu khi buoc 4, hoac phat lai vu
# khi moi 60 s x 5 lan) -> Remove(m_Hand) = thao va NEM NGUA/VU KHI DA MAC xuong dat; AddKIL
# (pos_hand) tra 0 vi m_Hand != 0 -> mon moi khong vao danh sach -> bot tay khong; m_Hand van
# lech -> moi lan phat lai deu NEM tiep 'Item[m_Hand]' (chi so cu, co the DA CAP LAI cho mon cua
# nguoi khac) => nguoi choi nhat duoc "item loi", out vao mat; nguy co dinh chi so mon nguoi that.
# Do 19:04 06/09: 346 bot tay khong (tat ca vua nhan ngua = tui day), 1531 luot phat lai, 1049
# dang cam=0.
# SUA (KPlayerBot.cpp, ASCII):
#   H17 pb_MacVaoNguoi viet lai: CHI dat vao o tui (CheckCanPlaceInEquipment + AddKIL pos_equiproom),
#       KHONG BAO GIO qua InsertEquipment/tay; het cho -> pb_LamChoTui xoa rac tui roi thu lai;
#       moi that bai co log [BotMac] + yeu cau mon do.
#   H18 pb_DonChoMac: kiem du dieu kien + co cho TRUOC khi huy mon cu (buoc 3 ngua, buoc 4 vu khi).
#   H19 pb_GiveFactionWeapon dung pb_DonChoMac + pb_MacVaoNguoi.
#   H20 sap tao hang: AddKIL vao o tui, khong InsertEquipment.
#   H21 pb_LamChoTui (sau pb_DonTui, cung bo loc giu thuoc/tui mau/phu/do nhiem vu/hang sap).
#   H22 PB_NangMach: kiem dwID khe (khe da cap lai cho nguoi khac thi bo qua).
# AP SAU goi_va_botnoi_dot4_0609.py. Idempotent.
import io, os, sys
ROOT = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_botnoi"
P = os.path.join(ROOT, "Sources", "Core", "Src", "KPlayerBot.cpp")

s = io.open(P, "r", encoding="latin-1", newline="").read()
truoc = sum(1 for c in s if ord(c) > 127)
crlf = "\r\n" in s
n = 0

def ap(ten, cu, moi):
    global s, n
    if crlf:
        cu = cu.replace("\n", "\r\n"); moi = moi.replace("\n", "\r\n")
    if moi in s:
        print("  [=] %s da ap tu truoc" % ten); return
    if s.count(cu) != 1:
        print("LOI: neo %s khop %d cho (can 1)" % (ten, s.count(cu))); sys.exit(1)
    s = s.replace(cu, moi); n += 1
    print("  [+] %s" % ten)

# ---- H17 + H18: viet lai pb_MacVaoNguoi, them pb_DonChoMac / pb_LogKhongMac / pb_DemDanhSach ----
ap("H17 pb_MacVaoNguoi chi dat vao o tui + pb_DonChoMac",
 '// Tra 1 = da mac len nguoi; 0 = khong du dieu kien / tui day (mon do DA huy sach).\n'
 'static int pb_MacVaoNguoi(int nIdx, int nNew, int nPlace)\n'
 '{\n'
 '\tif (nNew <= 0)\n'
 '\t\treturn 0;\n'
 '\tif (!Player[nIdx].m_ItemList.CanEquip(nNew, -1))\n'
 '\t{\n'
 '\t\tItemSet.Remove(nNew);              // chua chen vao tui - huy thang o kho item\n'
 '\t\treturn 0;\n'
 '\t}\n'
 '\tItem[nNew].LockItem(LOCK_STATE_LOCK);  // khoa nhu vu khi nhap mon - khong lam roi duoc\n'
 '\tPlayer[nIdx].m_ItemList.InsertEquipment(nNew, false);\n'
 '\tconst int q2 = Player[nIdx].m_ItemList.FindSame(nNew);\n'
 '\tif (q2 <= 0)\n'
 '\t{\n'
 '\t\tItemSet.Remove(nNew);              // tui day (InsertEquipment void - kiem lai sau)\n'
 '\t\treturn 0;\n'
 '\t}\n'
 '\tint nRoomX = -1, nRoomY = -1;\n'
 '\tif (Player[nIdx].m_ItemList.m_Items[q2].nPlace == pos_equiproom)\n'
 '\t{\n'
 '\t\tnRoomX = Player[nIdx].m_ItemList.m_Items[q2].nX;\n'
 '\t\tnRoomY = Player[nIdx].m_ItemList.m_Items[q2].nY;\n'
 '\t}\n'
 '\tif (!Player[nIdx].m_ItemList.Equip(nNew, nPlace))\n'
 '\t{\n'
 '\t\tPlayer[nIdx].m_ItemList.RemoveItemIdx(nNew, Item[nNew].GetStackNum());\n'
 '\t\treturn 0;\n'
 '\t}\n'
 '\tif (nRoomX >= 0)\n'
 '\t\tPlayer[nIdx].m_ItemList.m_Room[room_equipment].PickUpItem(\n'
 '\t\t\tnNew, nRoomX, nRoomY, Item[nNew].GetWidth(), Item[nNew].GetHeight());\n'
 '\treturn 1;\n'
 '}\n',
 '// Tra 1 = da mac len nguoi; 0 = khong du dieu kien / tui day (mon do DA huy sach).\n'
 '//\n'
 '// [MAC-TUI 06/09 - KHAN] KHONG dung InsertEquipment nua. Chu game: "bot nem do ra luc doi\n'
 '// do thi toi nhat vao hien thi loi item roi out ra vao lai thi mat do". InsertEquipment\n'
 '// (KItemList.cpp ~4603) khi tui khong con dai o: mon KHOA thi thu kho, roi dat vao TAY\n'
 '// (pos_hand) va NEM MON DANG O TAY xuong dat thanh Object. Equip (KItemList.cpp:1252) doi\n'
 '// nPlace sang pos_equip nhung KHONG XOA m_Hand -> m_Hand tro vao mon DA MAC; lan\n'
 '// InsertEquipment sau: Remove(m_Hand) = THAO + NEM ngua/vu khi da mac xuong dat, AddKIL\n'
 '// (pos_hand) tra 0 vi m_Hand != 0 -> mon moi khong vao danh sach -> bot tay khong;\n'
 '// m_Hand van lech -> moi lan phat lai vu khi (60 s x 5) lai nem tiep Item[m_Hand] (chi so\n'
 '// cu, co the da cap cho mon cua NGUOI KHAC) -> nguoi choi nhat duoc "item loi", mat khi\n'
 '// out. Do 19:04 06/09: 346 bot tay khong (deu vua nhan ngua = tui day), 1531 luot phat lai.\n'
 '// Nay: CHI dat vao O TUI (CheckCanPlaceInEquipment + AddKIL pos_equiproom) -> Equip ->\n'
 '// nha o luoi; het cho thi pb_LamChoTui xoa rac tui roi thu lai; van khong duoc thi HUY\n'
 '// mon moi va bao [BotMac] (mon cu con nguyen vi nguoi goi kiem pb_DonChoMac truoc).\n'
 'static int pb_LamChoTui(int nIdx, int nW, int nH);   // dinh nghia sau pb_DonTui\n'
 '\n'
 'static int pb_DemDanhSach(int nIdx)\n'
 '{\n'
 '\tint nCo = 0;\n'
 '\tfor (int q = 1; q < MAX_PLAYER_ITEM; q++)\n'
 '\t\tif (Player[nIdx].m_ItemList.m_Items[q].nIdx > 0)\n'
 '\t\t\tnCo++;\n'
 '\treturn nCo;\n'
 '}\n'
 '\n'
 '// in yeu cau cua mon (cap / suc manh / than phap / sinh khi / noi cong / he / phai) va chi so bot\n'
 'static void pb_LogKhongMac(int nIdx, int nNew, const char* szViec)\n'
 '{\n'
 '\tchar sz[200];\n'
 '\tint  nD = 0;\n'
 '\tsz[0] = 0;\n'
 '\tfor (int i = 0; i < 8; i++)\n'
 '\t{\n'
 '\t\tKMagicAttrib* p = (KMagicAttrib*)Item[nNew].GetRequirement(i);\n'
 '\t\tif (!p || !p->nAttribType)\n'
 '\t\t\tbreak;\n'
 '\t\tif (nD < (int)sizeof(sz) - 24)\n'
 '\t\t\tnD += _snprintf(sz + nD, sizeof(sz) - nD - 1, " yc%d=%d", p->nAttribType, p->nValue[0]);\n'
 '\t}\n'
 '\tconst int nNpcIdx = Player[nIdx].m_nIndex;\n'
 '\tpb_Log("[BotMac] %s: %s KHONG du dieu kien mac (yeu cau:%s | bot cap %d SM %d TP %d SK %d NC %d he %d)\\n",\n'
 '\t       Player[nIdx].m_PlayerName, szViec, sz[0] ? sz : " ?",\n'
 '\t       (nNpcIdx > 0 && nNpcIdx < MAX_NPC) ? (int)Npc[nNpcIdx].m_Level : 0,\n'
 '\t       Player[nIdx].m_nCurStrength, Player[nIdx].m_nCurDexterity,\n'
 '\t       Player[nIdx].m_nCurVitality, Player[nIdx].m_nCurEngergy,\n'
 '\t       (nNpcIdx > 0 && nNpcIdx < MAX_NPC) ? (int)Npc[nNpcIdx].m_Series : -1);\n'
 '}\n'
 '\n'
 '// Kiem TRUOC khi huy mon cu: du dieu kien mac + co o tui (xoa rac neu can) + danh sach do con\n'
 '// cho. Tra 1 = mac duoc (chua dat gi ca). Tay dang giu rac cua luong cu thi huy (chi khi mon do\n'
 '// THAT SU o pos_hand; m_Hand lech thi chi bao, khong dong vao).\n'
 'static int pb_DonChoMac(int nIdx, int nNew, const char* szViec)\n'
 '{\n'
 '\tif (nNew <= 0)\n'
 '\t\treturn 0;\n'
 '\tKItemList& il = Player[nIdx].m_ItemList;\n'
 '\tif (!il.CanEquip(nNew, -1))\n'
 '\t{\n'
 '\t\tpb_LogKhongMac(nIdx, nNew, szViec);\n'
 '\t\treturn 0;\n'
 '\t}\n'
 '\t{\n'
 '\t\tconst int nTay = il.Hand();\n'
 '\t\tif (nTay > 0 && nTay < MAX_ITEM)\n'
 '\t\t{\n'
 '\t\t\tconst int qT = il.FindSame(nTay);\n'
 '\t\t\tif (qT > 0 && il.m_Items[qT].nPlace == pos_hand)\n'
 '\t\t\t{\n'
 '\t\t\t\tpb_Log("[BotMac] %s: tay dang giu mon %d (rac luong cu) -> huy\\n",\n'
 '\t\t\t\t       Player[nIdx].m_PlayerName, nTay);\n'
 '\t\t\t\til.RemoveItemIdx(nTay, Item[nTay].GetStackNum());\n'
 '\t\t\t}\n'
 '\t\t\telse\n'
 '\t\t\t\tpb_Log("[BotMac] %s: m_Hand=%d LECH (mon khong o tay) - khong dung tay\\n",\n'
 '\t\t\t\t       Player[nIdx].m_PlayerName, nTay);\n'
 '\t\t}\n'
 '\t}\n'
 '\tconst int nW = Item[nNew].GetWidth(), nH = Item[nNew].GetHeight();\n'
 '\tint x = -1, y = -1;\n'
 '\tif (!il.CheckCanPlaceInEquipment(nW, nH, &x, &y) || pb_DemDanhSach(nIdx) >= MAX_PLAYER_ITEM - 2)\n'
 '\t\tpb_LamChoTui(nIdx, nW, nH);\n'
 '\tif (!il.CheckCanPlaceInEquipment(nW, nH, &x, &y))\n'
 '\t{\n'
 '\t\tpb_Log("[BotMac] %s: tui DAY, khong co dai %dx%d cho %s -> giu do cu\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, nW, nH, szViec);\n'
 '\t\treturn 0;\n'
 '\t}\n'
 '\tif (pb_DemDanhSach(nIdx) >= MAX_PLAYER_ITEM - 1)\n'
 '\t{\n'
 '\t\tpb_Log("[BotMac] %s: danh sach do DAY (%d) cho %s -> giu do cu\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, pb_DemDanhSach(nIdx), szViec);\n'
 '\t\treturn 0;\n'
 '\t}\n'
 '\treturn 1;\n'
 '}\n'
 '\n'
 'static int pb_MacVaoNguoi(int nIdx, int nNew, int nPlace)\n'
 '{\n'
 '\tif (nNew <= 0)\n'
 '\t\treturn 0;\n'
 '\tKItemList& il = Player[nIdx].m_ItemList;\n'
 '\tif (!il.CanEquip(nNew, -1))\n'
 '\t{\n'
 '\t\tpb_LogKhongMac(nIdx, nNew, "mac");\n'
 '\t\tItemSet.Remove(nNew);              // chua chen vao tui - huy thang o kho item\n'
 '\t\treturn 0;\n'
 '\t}\n'
 '\tconst int nW = Item[nNew].GetWidth(), nH = Item[nNew].GetHeight();\n'
 '\tint nRoomX = -1, nRoomY = -1;\n'
 '\tif (!il.CheckCanPlaceInEquipment(nW, nH, &nRoomX, &nRoomY))\n'
 '\t{\n'
 '\t\tpb_LamChoTui(nIdx, nW, nH);\n'
 '\t\tif (!il.CheckCanPlaceInEquipment(nW, nH, &nRoomX, &nRoomY))\n'
 '\t\t{\n'
 '\t\t\tpb_Log("[BotMac] %s: tui DAY, khong dat duoc mon %dx%d -> huy mon moi\\n",\n'
 '\t\t\t       Player[nIdx].m_PlayerName, nW, nH);\n'
 '\t\t\tItemSet.Remove(nNew);\n'
 '\t\t\treturn 0;\n'
 '\t\t}\n'
 '\t}\n'
 '\tItem[nNew].LockItem(LOCK_STATE_LOCK);  // khoa nhu vu khi nhap mon - khong lam roi duoc\n'
 '\tif (!il.AddKIL(nNew, pos_equiproom, nRoomX, nRoomY))\n'
 '\t{\n'
 '\t\tpb_Log("[BotMac] %s: AddKIL that bai (danh sach do %d/%d) -> huy mon moi\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, pb_DemDanhSach(nIdx), (int)MAX_PLAYER_ITEM);\n'
 '\t\tItemSet.Remove(nNew);\n'
 '\t\treturn 0;\n'
 '\t}\n'
 '\tif (!il.Equip(nNew, nPlace))\n'
 '\t{\n'
 '\t\tpb_Log("[BotMac] %s: Equip that bai (place %d) -> huy mon moi\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, nPlace);\n'
 '\t\til.RemoveItemIdx(nNew, Item[nNew].GetStackNum());\n'
 '\t\treturn 0;\n'
 '\t}\n'
 '\t// Equip chi doi nPlace sang pos_equip, KHONG nha o luoi (KItemList.cpp:1185) -> nha tay\n'
 '\til.m_Room[room_equipment].PickUpItem(nNew, nRoomX, nRoomY, nW, nH);\n'
 '\treturn 1;\n'
 '}\n')

# ---- H19a: khai bao truoc (pb_GiveFactionWeapon nam TRUOC pb_MacVaoNguoi trong tep) ----
ap("H19a khai bao truoc pb_DonChoMac/pb_MacVaoNguoi",
 '#define PB_VK_SO_DONG  6      // tran dong thuoc tinh do xanh cua du an\n',
 '#define PB_VK_SO_DONG  6      // tran dong thuoc tinh do xanh cua du an\n'
 '// [MAC-TUI 06/09] khai bao truoc: dinh nghia nam sau pb_TrangBiTheoCap, pb_GiveFactionWeapon can\n'
 'static int pb_DonChoMac(int nIdx, int nNew, const char* szViec);\n'
 'static int pb_MacVaoNguoi(int nIdx, int nNew, int nPlace);\n')

# ---- H19: pb_GiveFactionWeapon dung duong an toan ----
ap("H19 pb_GiveFactionWeapon: pb_DonChoMac + pb_MacVaoNguoi",
 '\tItem[nNew].LockItem(LOCK_STATE_LOCK);              // khoa lai de bot khong lam roi\n'
 '\tPlayer[nIdx].m_ItemList.InsertEquipment(nNew, false);\n'
 '\t// Ghi lai o luoi tui ma InsertEquipment vua chiem: Equip chi doi nPlace sang\n'
 '\t// pos_equip chu KHONG nha o luoi (KItemList.cpp:1185 - duong client that equip\n'
 '\t// tu pos_hand nen engine chua bao gio lo). Khong nha thi vu khi 1x3/1x4 chiem\n'
 '\t// chet 3-4 o tui suot doi bot (phan bien 18/08).\n'
 '\tint nWpnRoomX = -1, nWpnRoomY = -1;\n'
 '\t{\n'
 '\t\tconst int q2 = Player[nIdx].m_ItemList.FindSame(nNew);\n'
 '\t\tif (q2 > 0 && Player[nIdx].m_ItemList.m_Items[q2].nPlace == pos_equiproom)\n'
 '\t\t{\n'
 '\t\t\tnWpnRoomX = Player[nIdx].m_ItemList.m_Items[q2].nX;\n'
 '\t\t\tnWpnRoomY = Player[nIdx].m_ItemList.m_Items[q2].nY;\n'
 '\t\t}\n'
 '\t}\n'
 '\t// Equip o cay nay chi co HAI tham so (KItemList.h:104); ban tham khao co tham so thu ba\n'
 '\t// bUpdateSkin - dung chep nguyen chu ky sang.\n'
 '\tPlayer[nIdx].m_ItemList.Equip(nNew, -1);\n'
 '\tif (nWpnRoomX >= 0\n'
 '\t && Player[nIdx].m_ItemList.GetEquipment(itempart_weapon) == nNew)\n'
 '\t\tPlayer[nIdx].m_ItemList.m_Room[room_equipment].PickUpItem(\n'
 '\t\t\tnNew, nWpnRoomX, nWpnRoomY, Item[nNew].GetWidth(), Item[nNew].GetHeight());\n'
 '\n'
 '\tpb_Log("[BotVuKhi] %s phai %s: detail=%d parti=%d he=%d, dang cam=%d\\n",\n',
 '\t// [MAC-TUI 06/09] khong InsertEquipment nua (nem do o tay xuong dat) - kiem cho roi dat\n'
 '\t// vao o tui, Equip, nha o luoi (pb_MacVaoNguoi). Vu khi cu da thao o tren.\n'
 '\tif (!pb_DonChoMac(nIdx, nNew, "vu khi nhap mon"))\n'
 '\t{\n'
 '\t\tItemSet.Remove(nNew);\n'\
 '\t\tpb_Log("[BotVuKhi] %s phai %s: khong mac duoc vu khi nhap mon (xem [BotMac] tren)\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, s_facNpc[nFaction].szTen);\n'
 '\t\treturn;\n'
 '\t}\n'
 '\tpb_MacVaoNguoi(nIdx, nNew, -1);\n'
 '\n'
 '\tpb_Log("[BotVuKhi] %s phai %s: detail=%d parti=%d he=%d, dang cam=%d\\n",\n')

# ---- H19b/c: buoc 3 (ngua) va buoc 4 (vu khi) kiem pb_DonChoMac truoc khi huy mon cu ----
ap("H19b buoc 3 ngua: pb_DonChoMac",
 '\t\t\tif (nNew > 0 && !Player[nIdx].m_ItemList.CanEquip(nNew, -1))\n'
 '\t\t\t\tItemSet.Remove(nNew);\n'
 '\t\t\telse if (nNew > 0)\n'
 '\t\t\t{\n'
 '\t\t\t\tif (nNgua > 0)\n',
 '\t\t\tif (nNew > 0 && !pb_DonChoMac(nIdx, nNew, "ngua"))   // [MAC-TUI] kiem cho TRUOC khi huy ngua cu\n'
 '\t\t\t\tItemSet.Remove(nNew);\n'
 '\t\t\telse if (nNew > 0)\n'
 '\t\t\t{\n'
 '\t\t\t\tif (nNgua > 0)\n')

ap("H19c buoc 4 vu khi: pb_DonChoMac",
 '\t\t\tif (nNew > 0 && !Player[nIdx].m_ItemList.CanEquip(nNew, -1))\n'
 '\t\t\t\tItemSet.Remove(nNew);\n'
 '\t\t\telse if (nNew > 0)\n'
 '\t\t\t{\n'
 '\t\t\t\tPlayer[nIdx].m_ItemList.RemoveItemIdx(nW, Item[nW].GetStackNum());\n',
 '\t\t\tif (nNew > 0 && !pb_DonChoMac(nIdx, nNew, "vu khi cap 10"))   // [MAC-TUI] kiem cho TRUOC khi huy vu khi cu\n'
 '\t\t\t\tItemSet.Remove(nNew);\n'
 '\t\t\telse if (nNew > 0)\n'
 '\t\t\t{\n'
 '\t\t\t\tPlayer[nIdx].m_ItemList.RemoveItemIdx(nW, Item[nW].GetStackNum());\n')

# ---- H20: sap tao hang khong qua InsertEquipment ----
ap("H20 sap: AddKIL vao o tui",
 '\t\tPlayer[nIdx].m_ItemList.InsertEquipment(nNew, false);\n'
 '\t\tconst int q2 = Player[nIdx].m_ItemList.FindSame(nNew);\n'
 '\t\tif (q2 <= 0)\n'
 '\t\t{\n'
 '\t\t\tItemSet.Remove(nNew);\n'
 '\t\t\tcontinue;              // het slot m_Items (rat hiem)\n'
 '\t\t}\n',
 '\t\t// [MAC-TUI 06/09] khong InsertEquipment (tui day -> nhet vao TAY va NEM mon o tay\n'
 '\t\t// xuong dat): tim o tui truoc, het o thi dung ca dot.\n'
 '\t\t{\n'
 '\t\t\tint sx = -1, sy = -1;\n'
 '\t\t\tif (!Player[nIdx].m_ItemList.CheckCanPlaceInEquipment(\n'
 '\t\t\t        Item[nNew].GetWidth(), Item[nNew].GetHeight(), &sx, &sy))\n'
 '\t\t\t{\n'
 '\t\t\t\tItemSet.Remove(nNew);\n'
 '\t\t\t\tbreak;                 // tui day that -> dung ca dot, khoi xa rac\n'
 '\t\t\t}\n'
 '\t\t\tif (!Player[nIdx].m_ItemList.AddKIL(nNew, pos_equiproom, sx, sy))\n'
 '\t\t\t{\n'
 '\t\t\t\tItemSet.Remove(nNew);\n'
 '\t\t\t\tcontinue;              // het slot m_Items (rat hiem)\n'
 '\t\t\t}\n'
 '\t\t}\n'
 '\t\tconst int q2 = Player[nIdx].m_ItemList.FindSame(nNew);\n'
 '\t\tif (q2 <= 0)\n'
 '\t\t{\n'
 '\t\t\tItemSet.Remove(nNew);\n'
 '\t\t\tcontinue;              // het slot m_Items (rat hiem)\n'
 '\t\t}\n')

# ---- H21: pb_LamChoTui sau pb_DonTui ----
ap("H21 pb_LamChoTui",
 '\tif (nXoa)\n'
 '\t\tpb_Log("[BotXoaDo] %s don tui: xoa %d mon, giu binh thuoc + tui mau + phu + tien thao lo\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, nXoa);\n'
 '}\n',
 '\tif (nXoa)\n'
 '\t\tpb_Log("[BotXoaDo] %s don tui: xoa %d mon, giu binh thuoc + tui mau + phu + tien thao lo\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, nXoa);\n'
 '}\n'
 '\n'
 '// [MAC-TUI 06/09] Lam cho trong tui de dat mon nW x nH: xoa tung mon RAC trong tui (cung bo loc\n'
 '// giu cua pb_DonTui: thuoc, do nhiem vu, Tho Dia Phu, phu giu, hang dang bay sap) cho toi khi\n'
 '// co dai o hoac het rac. Khong co nhip cho (pb_DonTui gion 12 s), goi ngay khi can mac do.\n'
 '// Tra so mon da xoa.\n'
 'static int pb_LamChoTui(int nIdx, int nW, int nH)\n'
 '{\n'
 '\tKItemList& il = Player[nIdx].m_ItemList;\n'
 '\tint nXoa = 0;\n'
 '\tint x = -1, y = -1;\n'
 '\tfor (int q = 1; q < MAX_PLAYER_ITEM && nXoa < 60; q++)\n'
 '\t{\n'
 '\t\tif (il.CheckCanPlaceInEquipment(nW, nH, &x, &y) && pb_DemDanhSach(nIdx) < MAX_PLAYER_ITEM - 2)\n'
 '\t\t\tbreak;\n'
 '\t\tconst int g = il.m_Items[q].nIdx;\n'
 '\t\tif (g <= 0 || g >= MAX_ITEM)\n'
 '\t\t\tcontinue;\n'
 '\t\tif (il.m_Items[q].nPlace != pos_equiproom)\n'
 '\t\t\tcontinue;\n'
 '\t\tif (il.m_Items[q].nPrice > 0)\n'
 '\t\t\tcontinue;\n'
 '\t\tconst int nGe = Item[g].GetGenre();\n'
 '\t\tif (nGe == item_medicine || nGe == item_task || nGe == item_townportal)\n'
 '\t\t\tcontinue;\n'
 '\t\tif (nGe == item_mine && Item[g].GetDetailType() == 1 && Item[g].GetParticular() == 4)\n'
 '\t\t\tcontinue;\n'
 '\t\tif (nGe == item_magicscript && Item[g].GetDetailType() == 1)\n'
 '\t\t{\n'
 '\t\t\tconst int nPa = Item[g].GetParticular();\n'
 '\t\t\tint bGiu = 0;\n'
 '\t\t\tfor (int h = 0; h < (int)(sizeof(s_nGiuPhu) / sizeof(s_nGiuPhu[0])); h++)\n'
 '\t\t\t\tif (s_nGiuPhu[h] == nPa) { bGiu = 1; break; }\n'
 '\t\t\tif (bGiu)\n'
 '\t\t\t\tcontinue;\n'
 '\t\t}\n'
 '\t\tint nSo = Item[g].GetStackNum();\n'
 '\t\tif (nSo < 1)\n'
 '\t\t\tnSo = 1;\n'
 '\t\tif (il.RemoveItemIdx(g, nSo))\n'
 '\t\t\tnXoa++;\n'
 '\t}\n'
 '\tif (nXoa)\n'
 '\t\tpb_Log("[BotXoaDo] %s lam cho tui: xoa %d mon de dat mon %dx%d\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, nXoa, nW, nH);\n'
 '\treturn nXoa;\n'
 '}\n')

# ---- H22: PB_NangMach kiem dwID khe ----
ap("H22 PB_NangMach kiem dwID",
 '\t\tconst int nIdx = b.nPlayerIdx;\n'
 '\t\tif (nIdx <= 0 || nIdx >= MAX_PLAYER || Player[nIdx].m_nIndex <= 0)\n'
 '\t\t\tcontinue;\n'
 '\t\tint nDoi = 0;\n'
 '\t\tif (nMach == 0)\n',
 '\t\tconst int nIdx = b.nPlayerIdx;\n'
 '\t\tif (nIdx <= 0 || nIdx >= MAX_PLAYER || Player[nIdx].m_nIndex <= 0)\n'
 '\t\t\tcontinue;\n'
 '\t\tif (Player[nIdx].m_dwID == 0 || Player[nIdx].m_dwID != b.dwID)\n'
 '\t\t\tcontinue;                // khe da cap lai cho nguoi khac (nhu pb_DriveBot)\n'
 '\t\tint nDoi = 0;\n'
 '\t\tif (nMach == 0)\n')

if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
sau = sum(1 for c in s if ord(c) > 127)
print("da ap %d hunk; high-byte truoc=%d sau=%d %s" % (n, truoc, sau, "OK" if truoc == sau else "LECH!"))
