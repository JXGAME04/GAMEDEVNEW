# -*- coding: utf-8 -*-
# [SapRai 28/08] Chu game: "bot sap bay ban bi CHONG LEN NHAU, khong rai deu ra".
# Ba goc do duoc:
#   1) g_nPbNpcChan mac dinh 0 (KRegion.cpp:29, du an CO Y cho nguoi/bot dung chong
#      nhau) => nhanh GetBarrierMin(bCheckNpc=TRUE) trong pb_OSapTot KHONG BAO GIO
#      thay "o da co sap" -> bot cung nLech%8 chong dung mot o.
#   2) Vong duyet chi di 8 TIA x ban kinh 3..12 (80 o) -> co kiem cung chi xep
#      thanh 8 vet thang, khong "rai deu".
#   3) Bot DANG O SAN thanh nha khi bi boc ban sap: khoi teleport bi bo qua
#      (dieu kien map != nha.nMap) -> ngoi ngay tai cho dang dung (dam dong).
# Va:
#   H5 pb_OSapTot: tu quet s_bots (nBanSap) xem co con nao da ngoi dung o nay
#      (khong dung cong tac toan cuc g_nPbNpcChan - dung cham gameplay).
#   H6 pb_BanSap: duyet TRON VANH DAI (8r o/vong, ~600 o vanh 3..12, diem khoi dau
#      xoay theo chi so bot); bot dang o san thanh nha cung phai xep cho (SetPos
#      cung map); co b.nSapChoXong de khoi lap; log [BotSap] ngoi o(x,y).
#   H1-H4: them truong nSapChoXong + reset o 3 diem.
# AP SAU chuoi bot (botthp -> botnoingoai -> datau_thp). Chi KPlayerBot.cpp.
import io, sys
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerBot.cpp"

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

# ---- H1: truong moi trong PB_Bot ----
ap("H1 truong nSapChoXong",
 '\tint          nBanSapNgoi;                 // 1 = da ngoi xuong (do_sit)\n',
 '\tint          nBanSapNgoi;                 // 1 = da ngoi xuong (do_sit)\n'
 '\tint          nSapChoXong;                 // [SapRai 28/08] 1 = da xep CHO ngoi rieng\n')

# ---- H2: reset khi login/khoi tao bot ----
ap("H2 reset o login",
 '\t\tb.nBanSap = 0;     b.nBanSapXong = 0;  b.nBanSapNgoi = 0;  b.nNguaTick = 0;\n',
 '\t\tb.nBanSap = 0;     b.nBanSapXong = 0;  b.nBanSapNgoi = 0;  b.nNguaTick = 0;\n'
 '\t\tb.nSapChoXong = 0;\n')

# ---- H3: reset khi dong sap cu (PB_SetBanSap) ----
ap("H3 reset khi dong sap",
 '\t\ts_bots[i].nBanSap = 0;\n'
 '\t\ts_bots[i].nBanSapXong = 0;\n',
 '\t\ts_bots[i].nBanSap = 0;\n'
 '\t\ts_bots[i].nBanSapXong = 0;\n'
 '\t\ts_bots[i].nSapChoXong = 0;\n')

# ---- H4: reset khi boc bot moi ----
ap("H4 reset khi boc",
 '\t\ts_bots[aId[i]].nBanSap = 1;\n'
 '\t\tnChon++;\n',
 '\t\ts_bots[aId[i]].nBanSap = 1;\n'
 '\t\ts_bots[aId[i]].nSapChoXong = 0;   // [SapRai] phai xep cho ngoi rieng, ke ca dang o san thanh\n'
 '\t\tnChon++;\n')

# ---- H5: pb_OSapTot tu quet sap khac ----
ap("H5 OSapTot quet bot sap",
 '\tif (SubWorld[nSubIdx].m_Region[nR].GetBarrierMin(nMX, nMY, nOX, nOY, TRUE)\n'
 '\t    != Obstacle_NULL)\n'
 '\t\treturn 0;              // co nguoi/bot khac dang chiem o nay\n',
 '\tif (SubWorld[nSubIdx].m_Region[nR].GetBarrierMin(nMX, nMY, nOX, nOY, TRUE)\n'
 '\t    != Obstacle_NULL)\n'
 '\t\treturn 0;              // co nguoi/bot khac dang chiem o nay\n'
 '\t// [SapRai 28/08] g_nPbNpcChan mac dinh TAT (KRegion.cpp:29 - du an co y cho\n'
 '\t// nguoi/bot dung chong nhau) nen nhanh GetBarrierMin(bCheckNpc=TRUE) o tren\n'
 '\t// thuong KHONG bat duoc "o da co sap" -> ca dan chong mot o. Tu quet danh\n'
 '\t// sach bot sap: con nao (ke ca vua duoc dat trong CUNG khung) da dung/ngoi\n'
 '\t// dung o nay thi o XAU. O(so bot) moi o ung vien, chi chay luc xep cho.\n'
 '\tfor (int q3 = 0; q3 < s_botCount; q3++)\n'
 '\t{\n'
 '\t\tif (!s_bots[q3].nBanSap || s_bots[q3].nPlayerIdx <= 0)\n'
 '\t\t\tcontinue;\n'
 '\t\tconst int nn3 = Player[s_bots[q3].nPlayerIdx].m_nIndex;\n'
 '\t\tif (nn3 <= 0 || nn3 >= MAX_NPC)\n'
 '\t\t\tcontinue;\n'
 '\t\tif (Npc[nn3].m_SubWorldIndex != nSubIdx)\n'
 '\t\t\tcontinue;\n'
 '\t\tint qx3 = 0, qy3 = 0;\n'
 '\t\tNpc[nn3].GetMpsPos(&qx3, &qy3);\n'
 '\t\tif (qx3 / 32 == nCellX && qy3 / 32 == nCellY)\n'
 '\t\t\treturn 0;\n'
 '\t}\n')

# ---- H6: pb_BanSap - xep cho tron vanh dai + ap dung ca khi dang o san thanh ----
ap("H6 xep cho tron vanh dai",
 '\tif (SubWorld[nSub].m_SubWorldID != nha.nMap)\n'
 '\t{\n'
 '\t\tif (nowT - b.nDaTauTick < (unsigned int)(GAME_FPS * 3))\n'
 '\t\t\treturn;\n'
 '\t\tb.nDaTauTick = nowT;\n'
 '\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t\tpb_RoiNhom(nIdx, "ra thanh ngoi ban sap");\n'
 '\t\t// (19/08 toi chu game) CHI ngoi O TRONG quanh NPC Da Tau - truoc day dap\n'
 '\t\t// theo luoi 12x8 cung nhac nen bot ngoi vao GOC LAG / trong bui truc.\n'
 '\t\tconst int nSubT = g_SubWorldSet.SearchWorld(nha.nMap);\n'
 '\t\tint nSx = 0, nSy = 0;\n'
 '\t\tint bDat = 0;\n'
 '\t\tint nTamX = nha.nX, nTamY = nha.nY;\n'
 '\t\tif (nSubT >= 0)\n'
 '\t\t{\n'
 '\t\t\t// tam = NPC Da Tau THAT tren ban do (toa do bang chi la duong lui)\n'
 '\t\t\tconst int nDtNpc2 = pb_TimNpcNho(nSubT, PB_NPC_DT, "seasonnpc",\n'
 '\t\t\t                                 nha.nX * 32, nha.nY * 32);\n'
 '\t\t\tif (nDtNpc2 > 0)\n'
 '\t\t\t{\n'
 '\t\t\t\tint tx2 = 0, ty2 = 0;\n'
 '\t\t\t\tNpc[nDtNpc2].GetMpsPos(&tx2, &ty2);\n'
 '\t\t\t\tnTamX = tx2 / 32;\n'
 '\t\t\t\tnTamY = ty2 / 32;\n'
 '\t\t\t}\n'
 '\t\t\t// (19/08 toi #3) duyet VONG TRON quanh NPC ban kinh 3..12 o, moi bot khoi\n'
 '\t\t\t// dau mot goc rieng theo chi so; CHI nhan O TOT (pb_OSapTot: noi duoc\n'
 '\t\t\t// toi NPC theo ban do loang + thoang >= 5/8 lan can + chua ai ngoi).\n'
 '\t\t\t// Truoc day pb_ODat nhan ca o tui trong khe tuong -> "sap dinh goc ket".\n'
 '\t\t\tstatic const int aQx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };\n'
 '\t\t\tstatic const int aQy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };\n'
 '\t\t\tunsigned char aReach[PB_SAP_BFS_O];\n'
 '\t\t\tpb_SapLoang(nSubT, nTamX, nTamY, aReach);\n'
 '\t\t\tfor (int r3 = 3; r3 <= 12 && !bDat; r3++)\n'
 '\t\t\t{\n'
 '\t\t\t\tfor (int h3 = 0; h3 < 8 && !bDat; h3++)\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tconst int d3 = (nLech + h3) & 7;\n'
 '\t\t\t\t\tconst int cx3 = nTamX + aQx[d3] * r3;\n'
 '\t\t\t\t\tconst int cy3 = nTamY + aQy[d3] * r3;\n'
 '\t\t\t\t\tif (!pb_OSapTot(nSubT, cx3, cy3, nTamX, nTamY, aReach))\n'
 '\t\t\t\t\t\tcontinue;\n'
 '\t\t\t\t\tnSx = cx3 * 32;\n'
 '\t\t\t\t\tnSy = cy3 * 32;\n'
 '\t\t\t\t\tbDat = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nSx, nSy) == 1);\n'
 '\t\t\t\t}\n'
 '\t\t\t}\n'
 '\t\t\t// het vong ma chua dat duoc (quang truong chat kin / NPC dung goc khuat)\n'
 '\t\t\t// -> lui ve pb_ODat nhu cu: con hon khong bay duoc sap\n'
 '\t\t\tif (!bDat && pb_ODat(nSubT, nTamX, nTamY, nLech, 12, &nSx, &nSy))\n'
 '\t\t\t\tbDat = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nSx, nSy) == 1);\n'
 '\t\t}\n'
 '\t\tif (!bDat)\n'
 '\t\t\tNpc[nNpcIdx].ChangeWorld(nha.nMap, nha.nX * 32, nha.nY * 32);\n'
 '\t\tb.nTargetNpc = 0;\n'
 '\t\tb.nBanSapNgoi = 0;\n'
 '\t\tb.walk.Reset();  b.chase.Reset();  b.roam.Reset();\n'
 '\t\tb.nRoamX = 0;    b.nRoamY = 0;\n'
 '\t\tb.nBuocRaX = 0;  b.nBuocRaY = 0;\n'
 '\t\treturn;\n'
 '\t}\n',
 '\t// [SapRai 28/08] chu game: "sap chong len nhau, khong rai deu". Xep cho ca khi\n'
 '\t// bot DANG O SAN thanh nha (truoc day bi bo qua -> ngoi ngay dam dong dang dung).\n'
 '\tif (SubWorld[nSub].m_SubWorldID != nha.nMap || !b.nSapChoXong)\n'
 '\t{\n'
 '\t\tif (nowT - b.nDaTauTick < (unsigned int)(GAME_FPS * 3))\n'
 '\t\t\treturn;\n'
 '\t\tb.nDaTauTick = nowT;\n'
 '\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t\tpb_RoiNhom(nIdx, "ra thanh ngoi ban sap");\n'
 '\t\t// (19/08 toi chu game) CHI ngoi O TRONG quanh NPC Da Tau.\n'
 '\t\tconst int nSubT = g_SubWorldSet.SearchWorld(nha.nMap);\n'
 '\t\tint nSx = 0, nSy = 0;\n'
 '\t\tint bDat = 0;\n'
 '\t\tint nTamX = nha.nX, nTamY = nha.nY;\n'
 '\t\tconst int bCungMap = (SubWorld[nSub].m_SubWorldID == nha.nMap);\n'
 '\t\tif (nSubT >= 0)\n'
 '\t\t{\n'
 '\t\t\t// tam = NPC Da Tau THAT tren ban do (toa do bang chi la duong lui)\n'
 '\t\t\tconst int nDtNpc2 = pb_TimNpcNho(nSubT, PB_NPC_DT, "seasonnpc",\n'
 '\t\t\t                                 nha.nX * 32, nha.nY * 32);\n'
 '\t\t\tif (nDtNpc2 > 0)\n'
 '\t\t\t{\n'
 '\t\t\t\tint tx2 = 0, ty2 = 0;\n'
 '\t\t\t\tNpc[nDtNpc2].GetMpsPos(&tx2, &ty2);\n'
 '\t\t\t\tnTamX = tx2 / 32;\n'
 '\t\t\t\tnTamY = ty2 / 32;\n'
 '\t\t\t}\n'
 '\t\t\t// [SapRai 28/08] duyet TRON VANH DAI moi vong (4 canh x 2r = 8r o/vong,\n'
 '\t\t\t// ~600 o cho vanh 3..12) thay vi chi 8 tia (80 o, bot cung nLech%8 khoi\n'
 '\t\t\t// dau CUNG tia). Diem khoi dau tren vanh xoay theo chi so bot -> moi con\n'
 '\t\t\t// mot huong rieng, toa deu quanh quang truong nhu cho that; pb_OSapTot\n'
 '\t\t\t// (da them quet danh sach sap) tranh o co sap khac.\n'
 '\t\t\tunsigned char aReach[PB_SAP_BFS_O];\n'
 '\t\t\tpb_SapLoang(nSubT, nTamX, nTamY, aReach);\n'
 '\t\t\tfor (int r3 = 3; r3 <= 12 && !bDat; r3++)\n'
 '\t\t\t{\n'
 '\t\t\t\tconst int nChuVi = 8 * r3;\n'
 '\t\t\t\tconst int nBd = ((nLech % 97) * 13 + r3 * 7) % nChuVi;\n'
 '\t\t\t\tfor (int h3 = 0; h3 < nChuVi && !bDat; h3++)\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tconst int i3 = (nBd + h3) % nChuVi;\n'
 '\t\t\t\t\tconst int nCanh = i3 / (2 * r3);\n'
 '\t\t\t\t\tconst int t3 = i3 % (2 * r3) - r3;   // -r .. r-1\n'
 '\t\t\t\t\tint cx3 = nTamX, cy3 = nTamY;\n'
 '\t\t\t\t\tif (nCanh == 0)      { cx3 += r3;  cy3 += t3; }\n'
 '\t\t\t\t\telse if (nCanh == 1) { cx3 -= r3;  cy3 += t3; }\n'
 '\t\t\t\t\telse if (nCanh == 2) { cx3 += t3;  cy3 += r3; }\n'
 '\t\t\t\t\telse                 { cx3 += t3;  cy3 -= r3; }\n'
 '\t\t\t\t\tif (!pb_OSapTot(nSubT, cx3, cy3, nTamX, nTamY, aReach))\n'
 '\t\t\t\t\t\tcontinue;\n'
 '\t\t\t\t\tnSx = cx3 * 32;\n'
 '\t\t\t\t\tnSy = cy3 * 32;\n'
 '\t\t\t\t\tif (bCungMap)\n'
 '\t\t\t\t\t{\n'
 '\t\t\t\t\t\tNpc[nNpcIdx].SetPos(nSx, nSy);\n'
 '\t\t\t\t\t\tbDat = 1;\n'
 '\t\t\t\t\t}\n'
 '\t\t\t\t\telse\n'
 '\t\t\t\t\t\tbDat = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nSx, nSy) == 1);\n'
 '\t\t\t\t}\n'
 '\t\t\t}\n'
 '\t\t\t// het vong ma chua dat duoc (quang truong chat kin / NPC dung goc khuat)\n'
 '\t\t\t// -> lui ve pb_ODat nhu cu: con hon khong bay duoc sap\n'
 '\t\t\tif (!bDat && pb_ODat(nSubT, nTamX, nTamY, nLech, 12, &nSx, &nSy))\n'
 '\t\t\t{\n'
 '\t\t\t\tif (bCungMap)\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tNpc[nNpcIdx].SetPos(nSx, nSy);\n'
 '\t\t\t\t\tbDat = 1;\n'
 '\t\t\t\t}\n'
 '\t\t\t\telse\n'
 '\t\t\t\t\tbDat = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nSx, nSy) == 1);\n'
 '\t\t\t}\n'
 '\t\t}\n'
 '\t\tif (!bDat)\n'
 '\t\t{\n'
 '\t\t\tif (bCungMap)\n'
 '\t\t\t\tNpc[nNpcIdx].SetPos(nha.nX * 32, nha.nY * 32);\n'
 '\t\t\telse\n'
 '\t\t\t\tNpc[nNpcIdx].ChangeWorld(nha.nMap, nha.nX * 32, nha.nY * 32);\n'
 '\t\t}\n'
 '\t\telse\n'
 '\t\t\tpb_Log("[BotSap] %s ngoi sap thanh %d o(%d,%d)\\n",\n'
 '\t\t\t       Player[nIdx].m_PlayerName, nha.nMap, nSx / 32, nSy / 32);\n'
 '\t\tb.nSapChoXong = 1;      // co cho roi (hoac da lui ve diem bang) - khong xep lai\n'
 '\t\tb.nTargetNpc = 0;\n'
 '\t\tb.nBanSapNgoi = 0;\n'
 '\t\tb.walk.Reset();  b.chase.Reset();  b.roam.Reset();\n'
 '\t\tb.nRoamX = 0;    b.nRoamY = 0;\n'
 '\t\tb.nBuocRaX = 0;  b.nBuocRaY = 0;\n'
 '\t\treturn;\n'
 '\t}\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
