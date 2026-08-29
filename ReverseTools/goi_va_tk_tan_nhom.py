# -*- coding: utf-8 -*-
# [TK-TAN 28/08] Chu game: "bot tim doi thu se GOM 1 nhom rat dong -> phai quet:
# tren 20 bot cung phe o tai cho thi phai TAN RA de danh, khong thi nguoi sau don
# lai phia xa khong danh duoc nguoi truoc".
# Thiet ke: luc SEEK (chi chay khi bot CHUA co muc tieu = chinh ke dang xep hang
# sau), dem dong doi cung phe trong 12 o quanh minh; > 20 -> bo qua moi ung vien
# gan (< 20 o) trong pb_TkTimDichGanNhat -> nhan muc tieu o canh khac va di khoi
# dam dong. Tuyen dau dang danh (b.nTargetNpc > 0) da return truoc do - khong dung.
# Chi phi: chi ~24 bot seek moi khung x quet s_botCount = ~24k phep/khung.
# AP SAU goi_va_tk_censu_cuu.py. Chi KPlayerBot.cpp.
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

# ---- H1: them tham so nTranhGanMps vao ham quet ----
ap("H1 chu ky + loc gan",
 'static int pb_TkTimDichGanNhat(int nSub, int nNpcIdx, int nCampDich, int nTuIdx, int nLech, int* pnX, int* pnY)\n',
 'static int pb_TkTimDichGanNhat(int nSub, int nNpcIdx, int nCampDich, int nTuIdx, int nLech, int nTranhGanMps, int* pnX, int* pnY)\n')

ap("H1b diem loc theo d",
 '\t\tconst __int64 d = dx * dx + dy * dy;\n'
 '\t\tif (nCand < PB_TK_SAN_TOPK)\n',
 '\t\tconst __int64 d = dx * dx + dy * dy;\n'
 '\t\tif (nTranhGanMps > 0 && d < (__int64)nTranhGanMps * nTranhGanMps)\n'
 '\t\t\tcontinue;              // [TK-TAN] ket dam dong quanh minh -> bo muc tieu gan, toa canh khac\n'
 '\t\tif (nCand < PB_TK_SAN_TOPK)\n')

# ---- H2: callsite dem dam dong + truyen nguong ----
ap("H2 dem dam dong",
 '\t\tint nSanX = 0, nSanY = 0;\n'
 '\t\tconst int nSan = pb_TkTimDichGanNhat(nSub, nNpcIdx, nCampDich, b.nTkSanIdx, nLech, &nSanX, &nSanY);\n',
 '\t\t// [TK-TAN 28/08] (chu game) ">20 bot cung phe don mot cho thi phai tan ra":\n'
 '\t\t// dem dong doi cung phe trong 12 o quanh minh; qua 20 -> bo ung vien gan\n'
 '\t\t// (<20 o), nhan muc tieu canh khac. Chi cham bot CHUA co muc tieu (tuyen\n'
 '\t\t// sau) - tuyen dau dang danh da return o tren.\n'
 '\t\tint nTranhGan = 0;\n'
 '\t\t{\n'
 '\t\t\tint bxT = 0, byT = 0;\n'
 '\t\t\tNpc[nNpcIdx].GetMpsPos(&bxT, &byT);\n'
 '\t\t\tint nDong = 0;\n'
 '\t\t\tfor (int q8 = 0; q8 < s_botCount; q8++)\n'
 '\t\t\t{\n'
 '\t\t\t\tif (s_bots[q8].nTk != 4 || s_bots[q8].nTkPhe != b.nTkPhe)\n'
 '\t\t\t\t\tcontinue;\n'
 '\t\t\t\tconst int pq = s_bots[q8].nPlayerIdx;\n'
 '\t\t\t\tif (pq <= 0)\n'
 '\t\t\t\t\tcontinue;\n'
 '\t\t\t\tconst int nq = Player[pq].m_nIndex;\n'
 '\t\t\t\tif (nq <= 0 || nq >= MAX_NPC || Npc[nq].m_SubWorldIndex != nSub)\n'
 '\t\t\t\t\tcontinue;\n'
 '\t\t\t\tint qx = 0, qy = 0;\n'
 '\t\t\t\tNpc[nq].GetMpsPos(&qx, &qy);\n'
 '\t\t\t\tint adx = qx - bxT;  if (adx < 0) adx = -adx;\n'
 '\t\t\t\tint ady = qy - byT;  if (ady < 0) ady = -ady;\n'
 '\t\t\t\tif (adx <= 12 * 32 && ady <= 12 * 32)\n'
 '\t\t\t\t\tnDong++;\n'
 '\t\t\t}\n'
 '\t\t\tif (nDong > 20)\n'
 '\t\t\t{\n'
 '\t\t\t\tnTranhGan = 20 * 32;\n'
 '\t\t\t\tstatic unsigned int s_uLogTan = 0;\n'
 '\t\t\t\tif (now - s_uLogTan >= (unsigned int)(GAME_FPS * 3))\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\ts_uLogTan = now;\n'
 '\t\t\t\t\tpb_Log("[BotTan] %s phe %d ket dam %d con cung phe trong 12 o"\n'
 '\t\t\t\t\t       " -> toa sang canh khac\\n",\n'
 '\t\t\t\t\t       Player[nIdx].m_PlayerName, b.nTkPhe, nDong);\n'
 '\t\t\t\t}\n'
 '\t\t\t}\n'
 '\t\t}\n'
 '\t\tint nSanX = 0, nSanY = 0;\n'
 '\t\tconst int nSan = pb_TkTimDichGanNhat(nSub, nNpcIdx, nCampDich, b.nTkSanIdx, nLech, nTranhGan, &nSanX, &nSanY);\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
