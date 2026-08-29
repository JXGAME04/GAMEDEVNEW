# -*- coding: utf-8 -*-
# [BotTHP 28/08] Chu game: "moi toa do CHINH o Than Hanh Phu" (shenxingfu.lua):
#   H1: diem dap bao danh TK cua bot = DUNG diem battle_transprot (lib_tktc.lua:809/:812)
#       tha nguoi choi that: Tong (1541,3178) / Kim (1570,3085). Diem cu bam NPC
#       (startgame.lua:80/:89) lam ca dan dap sat goc ket phia Kim -> don cuc + lag.
#   H2: goi bot vao game ma dung O BI CHAN / NGOAI LUOI ("goc ket") -> SetPos ve diem
#       chuan Than Hanh Phu cua map do (bang THON_TT_MP_ARRAY chep nguyen van), moi con
#       mot o qua pb_ODat. Vi tri luu HOP LE thi giu nguyen (luat 18/08 cap>=20).
# Idempotent, latin-1, CRLF-aware. Chi dung KPlayerBot.cpp (server-only).
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

# ---------------- H0: prototype pb_ODat (dinh nghia o :2744, sau diem chen H2) ----------------
ap("H0 prototype pb_ODat",
 'static void pb_Log(const char* szFmt, ...)\n',
 '// [BotTHP 28/08] khai bao truoc: dinh nghia nam duoi (tim "static int pb_ODat"),\n'
 '// ma khoi gac goc-ket trong PB_OnRoleData (nam tren) can goi.\n'
 'static int pb_ODat(int nSubIdx, int nOX, int nOY, int nLech, int nRMax,\n'
 '                   int* pnX, int* pnY);\n'
 '\n'
 'static void pb_Log(const char* szFmt, ...)\n')

# ---------------- H1: diem dap bao danh = diem Than Hanh Phu ----------------
ap("H1 diem dap bao danh THP",
 '\t\tconst int nOX = (b.nTkPhe == 1) ? 1550 : 1555;   // startgame.lua:80 / :89\n'
 '\t\tconst int nOY = (b.nTkPhe == 1) ? 3179 : 3082;\n',
 '\t\t// (28/08 chu game) "moi toa do CHINH o Than Hanh Phu": dap DUNG diem\n'
 '\t\t// battle_transprot (shenxingfu.lua -> lib_tktc.lua:809/:812) tha nguoi choi\n'
 '\t\t// that: Tong (1541,3178) / Kim (1570,3085). Diem cu bam theo NPC bao danh\n'
 '\t\t// (startgame.lua:80/:89) lam ca dan dap sat goc ket phia Kim -> don cuc + lag;\n'
 '\t\t// pha 2 van di bo toi NPC nhu nguoi choi that nen khong doi gi khac.\n'
 '\t\tconst int nOX = (b.nTkPhe == 1) ? 1541 : 1570;   // lib_tktc.lua battle_transprot\n'
 '\t\tconst int nOY = (b.nTkPhe == 1) ? 3178 : 3085;\n')

# ---------------- H2: gac "goc ket" luc goi bot vao game ----------------
ap("H2 gac goc ket khi login",
 '\tPlayer[nIdx].LaunchPlayer2(true);\n',
 '\tPlayer[nIdx].LaunchPlayer2(true);\n'
 '\n'
 '\t// [BotTHP 28/08] Chu game: "goi bot ra nhieu con dung trong GOC KET" + "moi toa do\n'
 '\t// CHINH o Than Hanh Phu". Bot vao game dung vi tri luu/mau (luat 18/08 giu nguyen),\n'
 '\t// NHUNG neu o dang dung la O BI CHAN hoac NGOAI cua so luoi (goc ket - [BotLach]\n'
 '\t// lac khong thoat, [BotCuu]/T1 chi phu map bai) thi keo ve diem chuan Than Hanh Phu\n'
 '\t// cua map do (bang THON_TT_MP_ARRAY, shenxingfu.lua:18-48), moi con mot o rieng.\n'
 '\t// Map khong co trong bang (bai luyen cong...) thi giu nguyen - he cuu bai lo.\n'
 '\t{\n'
 '\t\tstatic const int aThp[][3] = {   // {mapid, oX, oY} - chep nguyen van shenxingfu.lua\n'
 '\t\t\t{1,1591,3170},{78,1565,3219},{11,3124,5117},{162,1576,3133},{37,1722,3081},\n'
 '\t\t\t{80,1756,3006},{176,1573,2933},{20,3552,6194},{53,1622,3189},{99,1628,3203},\n'
 '\t\t\t{101,1688,3154},{100,1628,3203},{121,1951,4509},{153,1605,3220},{174,1573,3203},\n'
 '\t\t\t{54,1650,3169},{175,1673,3168},{103,1668,3131},{59,1650,3169},{183,1417,3540},\n'
 '\t\t\t{25,4046,5170},{13,1899,4978},{154,382,1369},{115,1523,3780},{45,1603,3191},\n'
 '\t\t\t{81,1690,3169},{131,1526,3152},\n'
 '\t\t};\n'
 '\t\tconst int nNpcThp = Player[nIdx].m_nIndex;\n'
 '\t\tif (nNpcThp > 0 && nNpcThp < MAX_NPC)\n'
 '\t\t{\n'
 '\t\t\tconst int nSubThp = Npc[nNpcThp].m_SubWorldIndex;\n'
 '\t\t\tif (nSubThp >= 0 && SubWorld[nSubThp].CoLuoiSrv())\n'
 '\t\t\t{\n'
 '\t\t\t\tint nTx = 0, nTy = 0;\n'
 '\t\t\t\tNpc[nNpcThp].GetMpsPos(&nTx, &nTy);\n'
 '\t\t\t\tif (SubWorld[nSubThp].CellObsSrv(nTx, nTy) != 0)\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tconst int nMapThp = SubWorld[nSubThp].m_SubWorldID;\n'
 '\t\t\t\t\tint nVao = -1;\n'
 '\t\t\t\t\tfor (int t = 0; t < (int)(sizeof(aThp) / sizeof(aThp[0])); t++)\n'
 '\t\t\t\t\t\tif (aThp[t][0] == nMapThp) { nVao = t; break; }\n'
 '\t\t\t\t\tif (nVao >= 0)\n'
 '\t\t\t\t\t{\n'
 '\t\t\t\t\t\t// khuon rai 9x7 + loc o dat cua pha bao danh TK\n'
 '\t\t\t\t\t\tconst int nLechT = s_botCount;\n'
 '\t\t\t\t\t\tconst int nDxT = ((nLechT % 9) - 4) * 2;\n'
 '\t\t\t\t\t\tconst int nDyT = (((nLechT / 9) % 7) - 3) * 2;\n'
 '\t\t\t\t\t\tint nVx = 0, nVy = 0;\n'
 '\t\t\t\t\t\tint nOkT = pb_ODat(nSubThp, aThp[nVao][1] + nDxT, aThp[nVao][2] + nDyT,\n'
 '\t\t\t\t\t\t                   nLechT, 10, &nVx, &nVy);\n'
 '\t\t\t\t\t\tif (!nOkT)\n'
 '\t\t\t\t\t\t\tnOkT = pb_ODat(nSubThp, aThp[nVao][1], aThp[nVao][2], nLechT, 16, &nVx, &nVy);\n'
 '\t\t\t\t\t\tif (nOkT)\n'
 '\t\t\t\t\t\t{\n'
 '\t\t\t\t\t\t\tpb_Log("[BotTHP] %s map=%d vao game o o(%d,%d) BI CHAN/ngoai luoi"\n'
 '\t\t\t\t\t\t\t       " -> ve diem Than Hanh Phu o(%d,%d)\\n",\n'
 '\t\t\t\t\t\t\t       Player[nIdx].m_PlayerName, nMapThp, nTx / 32, nTy / 32,\n'
 '\t\t\t\t\t\t\t       nVx / 32, nVy / 32);\n'
 '\t\t\t\t\t\t\tNpc[nNpcThp].SetPos(nVx, nVy);\n'
 '\t\t\t\t\t\t}\n'
 '\t\t\t\t\t}\n'
 '\t\t\t\t\telse\n'
 '\t\t\t\t\t\tpb_Log("[BotTHP] %s map=%d o(%d,%d) BI CHAN nhung map khong co trong"\n'
 '\t\t\t\t\t\t       " bang Than Hanh Phu - giu nguyen cho he cuu bai\\n",\n'
 '\t\t\t\t\t\t       Player[nIdx].m_PlayerName, nMapThp, nTx / 32, nTy / 32);\n'
 '\t\t\t\t}\n'
 '\t\t\t}\n'
 '\t\t}\n'
 '\t}\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
