# -*- coding: utf-8 -*-
# [DT-THP 28/08] Chu game: "khi bot ve tra nhiem vu Da Tau tuy thanh thi lay TOA DO
# TRUNG TAM THANH do (co san o Than Hanh Phu) de PHU VE, roi DI CHUYEN toi NPC Da Tau".
#   H1 : nang bang toa do trung tam (THON_TT_MP_ARRAY shenxingfu.lua) len CAP FILE
#        (s_aThpDiem + pb_ThpDiem) - mot nguon su that.
#   H2a-c: khoi [BotTHP] login (goi_va_botthp_toado.py) doi sang dung pb_ThpDiem,
#        bo bang cuc bo aThp.
#   H3 : pb_DtVeThanh dap quanh TRUNG TAM thanh (Than Hanh Phu) thay vi canh NPC;
#        cac pha co san (DTB_TOI_NPC cuoi ngua di bo / duong Xa Phu 300s) tu dat toi
#        NPC. Anh huong ca 5 diem goi (tra/nhan Da Tau, tim Xa Phu, doi thanh) -
#        deu la "phu ve" nen ve trung tam la dung hanh vi nguoi choi that.
# AP SAU goi_va_botthp_toado.py + goi_va_botnoingoai.py (neo dua vao output cua chung).
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

# ---- H1: bang + helper cap file (sau pb_BotNoi cua goi_va_botnoingoai) ----
ap("H1 bang s_aThpDiem + pb_ThpDiem",
 'static int pb_BotNoi(int nIdx)\n'
 '{\n'
 '\treturn (int)(Player[nIdx].m_dwID & 1);\n'
 '}\n',
 'static int pb_BotNoi(int nIdx)\n'
 '{\n'
 '\treturn (int)(Player[nIdx].m_dwID & 1);\n'
 '}\n'
 '\n'
 '// [DT-THP 28/08] Toa do TRUNG TAM thanh/thon cua Than Hanh Phu - chep nguyen van\n'
 '// THON_TT_MP_ARRAY (script/item/ib/shenxingfu.lua:18-48). Chu game: "moi toa do\n'
 '// chinh o Than Hanh Phu". Dung cho: gac goc-ket luc goi bot vao game (PB_OnRoleData)\n'
 '// va diem dap "phu ve thanh" (pb_DtVeThanh).\n'
 'static const int s_aThpDiem[][3] = {   // {mapid, oX, oY}\n'
 '\t{1,1591,3170},{78,1565,3219},{11,3124,5117},{162,1576,3133},{37,1722,3081},\n'
 '\t{80,1756,3006},{176,1573,2933},{20,3552,6194},{53,1622,3189},{99,1628,3203},\n'
 '\t{101,1688,3154},{100,1628,3203},{121,1951,4509},{153,1605,3220},{174,1573,3203},\n'
 '\t{54,1650,3169},{175,1673,3168},{103,1668,3131},{59,1650,3169},{183,1417,3540},\n'
 '\t{25,4046,5170},{13,1899,4978},{154,382,1369},{115,1523,3780},{45,1603,3191},\n'
 '\t{81,1690,3169},{131,1526,3152},\n'
 '};\n'
 'static int pb_ThpDiem(int nMapId, int* pnX, int* pnY)\n'
 '{\n'
 '\tfor (int i = 0; i < (int)(sizeof(s_aThpDiem) / sizeof(s_aThpDiem[0])); i++)\n'
 '\t\tif (s_aThpDiem[i][0] == nMapId)\n'
 '\t\t{\n'
 '\t\t\t*pnX = s_aThpDiem[i][1];\n'
 '\t\t\t*pnY = s_aThpDiem[i][2];\n'
 '\t\t\treturn 1;\n'
 '\t\t}\n'
 '\treturn 0;\n'
 '}\n')

# ---- H2a: bo bang cuc bo trong khoi [BotTHP] login ----
ap("H2a bo bang cuc bo aThp",
 '\t{\n'
 '\t\tstatic const int aThp[][3] = {   // {mapid, oX, oY} - chep nguyen van shenxingfu.lua\n'
 '\t\t\t{1,1591,3170},{78,1565,3219},{11,3124,5117},{162,1576,3133},{37,1722,3081},\n'
 '\t\t\t{80,1756,3006},{176,1573,2933},{20,3552,6194},{53,1622,3189},{99,1628,3203},\n'
 '\t\t\t{101,1688,3154},{100,1628,3203},{121,1951,4509},{153,1605,3220},{174,1573,3203},\n'
 '\t\t\t{54,1650,3169},{175,1673,3168},{103,1668,3131},{59,1650,3169},{183,1417,3540},\n'
 '\t\t\t{25,4046,5170},{13,1899,4978},{154,382,1369},{115,1523,3780},{45,1603,3191},\n'
 '\t\t\t{81,1690,3169},{131,1526,3152},\n'
 '\t\t};\n'
 '\t\tconst int nNpcThp = Player[nIdx].m_nIndex;\n',
 '\t{\n'
 '\t\t// bang toa do dung chung s_aThpDiem/pb_ThpDiem (nang len cap file 28/08 chieu)\n'
 '\t\tconst int nNpcThp = Player[nIdx].m_nIndex;\n')

# ---- H2b: thay lookup aThp bang pb_ThpDiem ----
ap("H2b lookup qua pb_ThpDiem",
 '\t\t\t\t\tconst int nMapThp = SubWorld[nSubThp].m_SubWorldID;\n'
 '\t\t\t\t\tint nVao = -1;\n'
 '\t\t\t\t\tfor (int t = 0; t < (int)(sizeof(aThp) / sizeof(aThp[0])); t++)\n'
 '\t\t\t\t\t\tif (aThp[t][0] == nMapThp) { nVao = t; break; }\n'
 '\t\t\t\t\tif (nVao >= 0)\n',
 '\t\t\t\t\tconst int nMapThp = SubWorld[nSubThp].m_SubWorldID;\n'
 '\t\t\t\t\tint nThpX = 0, nThpY = 0;\n'
 '\t\t\t\t\tif (pb_ThpDiem(nMapThp, &nThpX, &nThpY))\n')

# ---- H2c: hai cho doc toa do ----
ap("H2c doc toa do qua nThpX/nThpY",
 '\t\t\t\t\t\tint nOkT = pb_ODat(nSubThp, aThp[nVao][1] + nDxT, aThp[nVao][2] + nDyT,\n'
 '\t\t\t\t\t\t                   nLechT, 10, &nVx, &nVy);\n'
 '\t\t\t\t\t\tif (!nOkT)\n'
 '\t\t\t\t\t\t\tnOkT = pb_ODat(nSubThp, aThp[nVao][1], aThp[nVao][2], nLechT, 16, &nVx, &nVy);\n',
 '\t\t\t\t\t\tint nOkT = pb_ODat(nSubThp, nThpX + nDxT, nThpY + nDyT,\n'
 '\t\t\t\t\t\t                   nLechT, 10, &nVx, &nVy);\n'
 '\t\t\t\t\t\tif (!nOkT)\n'
 '\t\t\t\t\t\t\tnOkT = pb_ODat(nSubThp, nThpX, nThpY, nLechT, 16, &nVx, &nVy);\n')

# ---- H3: pb_DtVeThanh dap TRUNG TAM thanh ----
ap("H3 DtVeThanh dap trung tam THP",
 '\tconst int nSubT = g_SubWorldSet.SearchWorld(nha.nMap);\n'
 '\tint nVx = 0, nVy = 0;\n'
 '\tint nOk = 0;\n'
 '\tif (nSubT >= 0)\n'
 '\t{\n'
 '\t\t// dap xuong cach NPC 6..15 o (nhu vua tu cong thanh buoc vao), o TRONG\n'
 '\t\tconst int nXaO = 6 + (nLech % 10);\n'
 '\t\tstatic const int aGx[4] = { 1, -1, 0, 0 };\n'
 '\t\tstatic const int aGy[4] = { 0, 0, 1, -1 };\n'
 '\t\tconst int g2 = nLech & 3;\n'
 '\t\tif (pb_ODat(nSubT, nha.nX + aGx[g2] * nXaO, nha.nY + aGy[g2] * nXaO,\n'
 '\t\t            nLech, 8, &nVx, &nVy))\n'
 '\t\t\tnOk = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nVx, nVy) == 1);\n'
 '\t\tif (!nOk && pb_ODat(nSubT, nha.nX, nha.nY, nLech, 12, &nVx, &nVy))\n'
 '\t\t\tnOk = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nVx, nVy) == 1);\n'
 '\t}\n'
 '\tif (!nOk)\n'
 '\t\tnOk = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nha.nX * 32, nha.nY * 32) == 1);\n',
 '\tconst int nSubT = g_SubWorldSet.SearchWorld(nha.nMap);\n'
 '\tint nVx = 0, nVy = 0;\n'
 '\tint nOk = 0;\n'
 '\t// (28/08 chu game) "phu ve lay TOA DO TRUNG TAM THANH (Than Hanh Phu) roi DI\n'
 '\t// CHUYEN toi NPC" - nhu nguoi choi that dung phu. Dap quanh TRUNG TAM lech\n'
 '\t// 6..15 o theo chi so bot cho khoi don cuc; cac pha co san tu dat toi NPC\n'
 '\t// (DTB_TOI_NPC cuoi ngua, han 600 nhip - xa nhat Dai Ly ~119 o van du; duong\n'
 '\t// Xa Phu co ngan sach 300 giay). Map khong co trong bang thi giu diem cu.\n'
 '\tint nTtX = nha.nX, nTtY = nha.nY;\n'
 '\tpb_ThpDiem(nha.nMap, &nTtX, &nTtY);\n'
 '\tif (nSubT >= 0)\n'
 '\t{\n'
 '\t\tconst int nXaO = 6 + (nLech % 10);\n'
 '\t\tstatic const int aGx[4] = { 1, -1, 0, 0 };\n'
 '\t\tstatic const int aGy[4] = { 0, 0, 1, -1 };\n'
 '\t\tconst int g2 = nLech & 3;\n'
 '\t\tif (pb_ODat(nSubT, nTtX + aGx[g2] * nXaO, nTtY + aGy[g2] * nXaO,\n'
 '\t\t            nLech, 8, &nVx, &nVy))\n'
 '\t\t\tnOk = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nVx, nVy) == 1);\n'
 '\t\tif (!nOk && pb_ODat(nSubT, nTtX, nTtY, nLech, 12, &nVx, &nVy))\n'
 '\t\t\tnOk = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nVx, nVy) == 1);\n'
 '\t}\n'
 '\tif (!nOk)\n'
 '\t\tnOk = (Npc[nNpcIdx].ChangeWorld(nha.nMap, nTtX * 32, nTtY * 32) == 1);\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
