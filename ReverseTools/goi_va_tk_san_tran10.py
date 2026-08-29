# -*- coding: utf-8 -*-
# [TK-SAN10 28/08] Chu game: "toi da 10 bot dinh vi cung 1 muc tieu".
# Bang dem s_nTkSanDem[idx] = so bot dang san muc tieu do:
#   - tang/giam SONG ngay khi bot doi/tha muc tieu trong khoi [BotSan];
#   - DUNG LAI moi giay trong pb_TkNhip (quet s_bots) de tu don rac bot chet/
#     roi tran/doi pha - khong can moc vao moi duong thoat.
# pb_TkTimDichGanNhat bo qua ung vien da du 10 con san (tru muc tieu HIEN TAI
# cua chinh bot - giu cho, khong tu evict) -> ap luc tu dan sang con gan ke tiep.
# AP SAU goi_va_tk_san_doithu.py. Chi KPlayerBot.cpp.
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

# ---- H1: bang dem + nguong, dat truoc ham quet ----
ap("H1 bang dem s_nTkSanDem",
 '// [TK-SAN 28/08] Tim DOI THU (nguoi/bot phe dich) GAN NHAT con SONG tren cung\n',
 '// [TK-SAN10 28/08] (chu game) "toi da 10 bot dinh vi cung 1 muc tieu".\n'
 '// Bang dem so bot dang san theo npc idx: tang/giam song trong khoi [BotSan],\n'
 '// dung lai moi giay o pb_TkNhip (don rac bot chet/roi tran khong qua duong thoat).\n'
 '#define PB_TK_SAN_TRAN  10\n'
 'static short s_nTkSanDem[MAX_NPC];\n'
 '\n'
 '// [TK-SAN 28/08] Tim DOI THU (nguoi/bot phe dich) GAN NHAT con SONG tren cung\n')

# ---- H2: helper nhan them nTuIdx + bo qua ung vien da du 10 ----
ap("H2 helper loc tran 10",
 'static int pb_TkTimDichGanNhat(int nSub, int nNpcIdx, int nCampDich, int* pnX, int* pnY)\n',
 'static int pb_TkTimDichGanNhat(int nSub, int nNpcIdx, int nCampDich, int nTuIdx, int* pnX, int* pnY)\n')

ap("H2b diem loc trong vong quet",
 '\t\tif (Npc[nn].m_CurrentLife <= 0)\n'
 '\t\t\tcontinue;\n'
 '\t\tint ex = 0, ey = 0;\n',
 '\t\tif (Npc[nn].m_CurrentLife <= 0)\n'
 '\t\t\tcontinue;\n'
 '\t\tif (nn != nTuIdx && s_nTkSanDem[nn] >= PB_TK_SAN_TRAN)\n'
 '\t\t\tcontinue;              // [TK-SAN10] da du 10 con san no -> chon con gan ke tiep\n'
 '\t\tint ex = 0, ey = 0;\n')

# ---- H3: call site + ke toan tang/giam song ----
ap("H3 call site + ke toan",
 '\t\tint nSanX = 0, nSanY = 0;\n'
 '\t\tconst int nSan = pb_TkTimDichGanNhat(nSub, nNpcIdx, nCampDich, &nSanX, &nSanY);\n'
 '\t\tif (nSan > 0)\n'
 '\t\t{\n'
 '\t\t\tconst int bDoiDich = (nSan != b.nTkSanIdx)\n'
 '\t\t\t                  || (g_GetDistance(nSanX, nSanY, b.nTkDichX, b.nTkDichY) > 8 * 32);\n'
 '\t\t\tb.nTkSanIdx = nSan;\n',
 '\t\tint nSanX = 0, nSanY = 0;\n'
 '\t\tconst int nSan = pb_TkTimDichGanNhat(nSub, nNpcIdx, nCampDich, b.nTkSanIdx, &nSanX, &nSanY);\n'
 '\t\tif (nSan > 0)\n'
 '\t\t{\n'
 '\t\t\tconst int bDoiDich = (nSan != b.nTkSanIdx)\n'
 '\t\t\t                  || (g_GetDistance(nSanX, nSanY, b.nTkDichX, b.nTkDichY) > 8 * 32);\n'
 '\t\t\tif (nSan != b.nTkSanIdx)\n'
 '\t\t\t{\n'
 '\t\t\t\t// [TK-SAN10] ke toan song: tha muc tieu cu, giu cho muc tieu moi\n'
 '\t\t\t\tif (b.nTkSanIdx > 0 && b.nTkSanIdx < MAX_NPC && s_nTkSanDem[b.nTkSanIdx] > 0)\n'
 '\t\t\t\t\ts_nTkSanDem[b.nTkSanIdx]--;\n'
 '\t\t\t\tif (nSan < MAX_NPC)\n'
 '\t\t\t\t\ts_nTkSanDem[nSan]++;\n'
 '\t\t\t}\n'
 '\t\t\tb.nTkSanIdx = nSan;\n')

ap("H3b tha muc tieu khi khong thay ai",
 '\t\telse\n'
 '\t\t\tb.nTkSanIdx = 0;               // khong thay doi thu -> di doanh trai nhu cu\n',
 '\t\telse\n'
 '\t\t{\n'
 '\t\t\tif (b.nTkSanIdx > 0 && b.nTkSanIdx < MAX_NPC && s_nTkSanDem[b.nTkSanIdx] > 0)\n'
 '\t\t\t\ts_nTkSanDem[b.nTkSanIdx]--;   // [TK-SAN10] tha cho\n'
 '\t\t\tb.nTkSanIdx = 0;               // khong thay doi thu -> di doanh trai nhu cu\n'
 '\t\t}\n')

# ---- H4: dung lai bang dem moi giay trong pb_TkNhip ----
ap("H4 rebuild 1Hz",
 'static void pb_TkNhip()\n'
 '{\n'
 '\tstatic DWORD s_dwMoc = 0;\n'
 '\tconst DWORD dwNow = GetTickCount();\n'
 '\tif (s_dwMoc && dwNow - s_dwMoc < 1000)\n'
 '\t\treturn;\n'
 '\ts_dwMoc = dwNow;\n',
 'static void pb_TkNhip()\n'
 '{\n'
 '\tstatic DWORD s_dwMoc = 0;\n'
 '\tconst DWORD dwNow = GetTickCount();\n'
 '\tif (s_dwMoc && dwNow - s_dwMoc < 1000)\n'
 '\t\treturn;\n'
 '\ts_dwMoc = dwNow;\n'
 '\n'
 '\t// [TK-SAN10] dung lai bang dem san-muc-tieu tu dau moi giay: tu don rac khi\n'
 '\t// bot chet / roi tran / doi pha ma khong can moc vao tung duong thoat.\n'
 '\t{\n'
 '\t\tmemset(s_nTkSanDem, 0, sizeof(s_nTkSanDem));\n'
 '\t\tfor (int i9 = 0; i9 < s_botCount; i9++)\n'
 '\t\t\tif (s_bots[i9].nTk == 4 && s_bots[i9].nTkSanIdx > 0\n'
 '\t\t\t && s_bots[i9].nTkSanIdx < MAX_NPC)\n'
 '\t\t\t\ts_nTkSanDem[s_bots[i9].nTkSanIdx]++;\n'
 '\t}\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
