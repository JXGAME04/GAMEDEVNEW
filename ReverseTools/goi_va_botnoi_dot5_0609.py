# -*- coding: utf-8 -*-
# [BOTNOI dot 5 06/09] Sau restart 20:45/20:57 (ban 7e41f1b7 co dot 4b): 459 bot van "tui DAY" ->
# 1.056 dong [BotMac] "giu do cu", 0 dong "lam cho tui" = pb_LamChoTui KHONG xoa duoc mon nao.
# GOC: bot nhat binh thuoc roi (item_medicine) va pb_DonTui GIU MOI binh thuoc -> [BotUong] "con 47..51
# binh" = 47-51 O tui toan binh (moi chong 1 o) -> tui day vinh vien, khong con dai 1x3 / 2x3 cho vu khi /
# ngua. Bot chi uong binh DAU TIEN tim thay (pb_Uong), nen giu vai chong la du; binh nhat lai lien tuc.
# SUA (KPlayerBot.cpp):
#   H24 pb_LamChoTui: sau vong xoa rac, van chua co cho -> xoa bot binh thuoc, giu PB_BINH_GIU_KHAN (4)
#       chong lon nhat (uu tien giu binh HP: detail 0), roi xoa tiep neu can; log so xoa.
#   H25 pb_DonTui (don dinh ky): tran binh thuoc PB_BINH_GIU (12) chong - qua thi xoa chong nho nhat,
#       de tui khong day lai.
#   H26 pb_DonChoMac: khi that bai ghi 1 dong thong ke tui (so o trong, so mon theo genre) - toi da 60
#       dong/phien de con biet tui chua gi.
# AP SAU goi_va_botnoi_dot4c_0609.py. Idempotent. Chi ASCII.
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

# ---- H24: pb_LamChoTui vong 2 xoa bot binh thuoc ----
ap("H24 pb_LamChoTui: vong 2 xoa binh thuoc thua",
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
 '}\n',
 '\t\tint nSo = Item[g].GetStackNum();\n'
 '\t\tif (nSo < 1)\n'
 '\t\t\tnSo = 1;\n'
 '\t\tif (il.RemoveItemIdx(g, nSo))\n'
 '\t\t\tnXoa++;\n'
 '\t}\n'
 '\t// [TUI-BINH 06/09 dot 5] Van chua co cho: tui bot day BINH THUOC nhat duoc (pb_Uong "con 47..51\n'
 '\t// binh" = 47-51 chong, moi chong 1 o; pb_DonTui truoc gio giu het). Bot chi uong binh DAU TIEN\n'
 '\t// tim thay nen giu PB_BINH_GIU_KHAN chong lon nhat (uu tien binh HP detail 0) la du; con lai xoa\n'
 '\t// dan cho toi khi co dai o. Binh nhat lai lien tuc ngoai bai nen khong thiet.\n'
 '\tint nXoaBinh = 0;\n'
 '\tif (!(il.CheckCanPlaceInEquipment(nW, nH, &x, &y) && pb_DemDanhSach(nIdx) < MAX_PLAYER_ITEM - 2))\n'
 '\t\tnXoaBinh = pb_XoaBinhThua(nIdx, PB_BINH_GIU_KHAN, nW, nH);\n'\
 '\tif (nXoa || nXoaBinh)\n'
 '\t\tpb_Log("[BotXoaDo] %s lam cho tui: xoa %d mon rac + %d chong binh de dat mon %dx%d\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, nXoa, nXoaBinh, nW, nH);\n'
 '\treturn nXoa + nXoaBinh;\n'
 '}\n')

# ---- H24b: pb_XoaBinhThua + hang so, dat TRUOC pb_DonTui ----
ap("H24b pb_XoaBinhThua truoc pb_DonTui",
 'static void pb_DonTui(int nIdx, PB_Bot& b, unsigned int now)\n'
 '{\n'
 '\tif (b.nDonTuiTick && now - b.nDonTuiTick < (unsigned int)PB_DONTUI_GAP)\n'
 '\t\treturn;\n',
 '// [TUI-BINH 06/09 dot 5] Xoa bot chong binh thuoc trong TUI, giu nGiu chong "tot nhat" (binh HP\n'
 '// detail 0 truoc, roi chong lon truoc). nW/nH > 0: dung ngay khi da co dai o nW x nH. Tra so chong xoa.\n'
 '#define PB_BINH_GIU       12     // don dinh ky: tran chong binh trong tui\n'
 '#define PB_BINH_GIU_KHAN  4      // luc can cho mac do: giu it hon\n'
 'static int pb_XoaBinhThua(int nIdx, int nGiu, int nW, int nH)\n'
 '{\n'
 '\tKItemList& il = Player[nIdx].m_ItemList;\n'
 '\tint aQ[MAX_PLAYER_ITEM];\n'
 '\tint nBinh = 0;\n'
 '\tfor (int q = 1; q < MAX_PLAYER_ITEM; q++)\n'
 '\t{\n'
 '\t\tconst int g = il.m_Items[q].nIdx;\n'
 '\t\tif (g <= 0 || g >= MAX_ITEM || il.m_Items[q].nPlace != pos_equiproom)\n'
 '\t\t\tcontinue;\n'
 '\t\tif (il.m_Items[q].nPrice > 0 || Item[g].GetGenre() != item_medicine)\n'
 '\t\t\tcontinue;\n'
 '\t\taQ[nBinh++] = q;\n'
 '\t}\n'
 '\tif (nBinh <= nGiu)\n'
 '\t\treturn 0;\n'
 '\t// sap xep: binh HP (detail 0) truoc, chong lon truoc -> phan DUOI danh sach bi xoa\n'
 '\tfor (int i = 1; i < nBinh; i++)\n'
 '\t{\n'
 '\t\tconst int qi = aQ[i];\n'
 '\t\tint j = i - 1;\n'
 '\t\twhile (j >= 0)\n'
 '\t\t{\n'
 '\t\t\tconst int gj = il.m_Items[aQ[j]].nIdx, gi = il.m_Items[qi].nIdx;\n'
 '\t\t\tconst int kj = (Item[gj].GetDetailType() == 0 ? 1000000 : 0) + Item[gj].GetStackNum();\n'
 '\t\t\tconst int ki = (Item[gi].GetDetailType() == 0 ? 1000000 : 0) + Item[gi].GetStackNum();\n'
 '\t\t\tif (kj >= ki)\n'
 '\t\t\t\tbreak;\n'
 '\t\t\taQ[j + 1] = aQ[j];\n'
 '\t\t\tj--;\n'
 '\t\t}\n'
 '\t\taQ[j + 1] = qi;\n'
 '\t}\n'
 '\tint nXoa = 0;\n'
 '\tint x = -1, y = -1;\n'
 '\tfor (int i = nBinh - 1; i >= nGiu; i--)\n'
 '\t{\n'
 '\t\tif (nW > 0 && nH > 0 && il.CheckCanPlaceInEquipment(nW, nH, &x, &y)\n'
 '\t\t && pb_DemDanhSach(nIdx) < MAX_PLAYER_ITEM - 2)\n'
 '\t\t\tbreak;\n'
 '\t\tconst int g = il.m_Items[aQ[i]].nIdx;\n'
 '\t\tif (g <= 0 || g >= MAX_ITEM)\n'
 '\t\t\tcontinue;\n'
 '\t\tint nSo = Item[g].GetStackNum();\n'
 '\t\tif (nSo < 1)\n'
 '\t\t\tnSo = 1;\n'
 '\t\tif (il.RemoveItemIdx(g, nSo))\n'
 '\t\t\tnXoa++;\n'
 '\t}\n'
 '\treturn nXoa;\n'
 '}\n'
 '\n'
 'static void pb_DonTui(int nIdx, PB_Bot& b, unsigned int now)\n'
 '{\n'
 '\tif (b.nDonTuiTick && now - b.nDonTuiTick < (unsigned int)PB_DONTUI_GAP)\n'
 '\t\treturn;\n')

# ---- H25: pb_DonTui dinh ky cung cat binh thua ----
ap("H25 pb_DonTui: tran binh thuoc",
 '\tif (nXoa)\n'
 '\t\tpb_Log("[BotXoaDo] %s don tui: xoa %d mon, giu binh thuoc + tui mau + phu + tien thao lo\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, nXoa);\n'
 '}\n',
 '\t// [TUI-BINH 06/09 dot 5] tran chong binh thuoc - de tui khong day lai boi binh nhat duoc\n'
 '\tconst int nXoaBinh = pb_XoaBinhThua(nIdx, PB_BINH_GIU, 0, 0);\n'
 '\tif (nXoa || nXoaBinh)\n'
 '\t\tpb_Log("[BotXoaDo] %s don tui: xoa %d mon rac + %d chong binh thua (giu %d chong), giu tui mau + phu + tien thao lo\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, nXoa, nXoaBinh, (int)PB_BINH_GIU);\n'
 '}\n')

# ---- H26: thong ke tui khi that bai ----
ap("H26 pb_DonChoMac: thong ke tui khi that bai",
 '\tif (!il.CheckCanPlaceInEquipment(nW, nH, &x, &y))\n'
 '\t{\n'
 '\t\tpb_Log("[BotMac] %s: tui DAY, khong co dai %dx%d cho %s -> giu do cu\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, nW, nH, szViec);\n'
 '\t\treturn 0;\n'
 '\t}\n',
 '\tif (!il.CheckCanPlaceInEquipment(nW, nH, &x, &y))\n'
 '\t{\n'
 '\t\t// [TUI-BINH dot 5] thong ke tui theo genre (toi da 60 dong/phien) de biet tui chua gi\n'
 '\t\tstatic int s_nThongKeTui = 0;\n'
 '\t\tchar szTk[160];\n'
 '\t\tszTk[0] = 0;\n'
 '\t\tif (s_nThongKeTui < 60)\n'
 '\t\t{\n'
 '\t\t\tint aGe[16];\n'
 '\t\t\tint nTong = 0, nGia = 0;\n'
 '\t\t\tZeroMemory(aGe, sizeof(aGe));\n'
 '\t\t\tfor (int q = 1; q < MAX_PLAYER_ITEM; q++)\n'
 '\t\t\t{\n'
 '\t\t\t\tconst int g = il.m_Items[q].nIdx;\n'
 '\t\t\t\tif (g <= 0 || g >= MAX_ITEM || il.m_Items[q].nPlace != pos_equiproom)\n'
 '\t\t\t\t\tcontinue;\n'
 '\t\t\t\tconst int ge = Item[g].GetGenre();\n'
 '\t\t\t\tif (ge >= 0 && ge < 16) aGe[ge]++;\n'
 '\t\t\t\tnTong++;\n'
 '\t\t\t\tif (il.m_Items[q].nPrice > 0) nGia++;\n'
 '\t\t\t}\n'
 '\t\t\t_snprintf(szTk, sizeof(szTk) - 1,\n'
 '\t\t\t          " | tui %d mon (equip %d thuoc %d mine %d task %d phu %d magic %d, dang bay %d)",\n'
 '\t\t\t          nTong, aGe[item_equip], aGe[item_medicine], aGe[item_mine], aGe[item_task],\n'
 '\t\t\t          aGe[item_townportal], aGe[item_magicscript], nGia);\n'
 '\t\t\tszTk[sizeof(szTk) - 1] = 0;\n'
 '\t\t\ts_nThongKeTui++;\n'
 '\t\t}\n'
 '\t\tpb_Log("[BotMac] %s: tui DAY, khong co dai %dx%d cho %s -> giu do cu%s\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, nW, nH, szViec, szTk);\n'
 '\t\treturn 0;\n'
 '\t}\n')

if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
sau = sum(1 for c in s if ord(c) > 127)
print("da ap %d hunk; high-byte truoc=%d sau=%d %s" % (n, truoc, sau, "OK" if truoc == sau else "LECH!"))
