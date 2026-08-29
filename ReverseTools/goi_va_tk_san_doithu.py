# -*- coding: utf-8 -*-
# [TK-SAN 28/08] Chu game: "bot Tong Kim DI CHUYEN TIM DOI THU de danh, khong di
# chuyen toa do co dinh nua; tu dinh vi doi thu o dau va di chuyen THANG toi danh
# luon; tren duong gap muc tieu khac thi tu bat muc tieu GAN NHAT, khong co dinh;
# bo qua muc tieu da chet".
# Hien trang pha 4: het dich gan -> boc toa do CO DINH trong bang doanh trai dich
# (pb_TkLayDoanh) + vong diem trung gian; bo nham TK xoay vong ung vien (khong
# phai gan nhat).
# Va:
#   H3 pb_TkTimDichGanNhat: quet Player[] (nguoi + bot) cung map, camp DICH,
#      SONG (bo do_death/do_revive/mau<=0) -> gan nhat.
#   H4 [BotSan] trong pha 4: moi ~1,2s (so le theo bot) dinh vi lai; co dich ->
#      dat thang b.nTkDich = vi tri dich, KHONG vong trung gian, doi muc tieu/
#      dich doi >8 o thi Reset duong; khong thay ai -> roi ve loi doanh trai cu.
#      Dang co muc tieu danh (b.nTargetNpc) thi khoi nay khong chay (giu nguyen
#      luat "dang danh khong dong lo trinh").
#   H5 bo nham trong TK: lay muc tieu GAN NHAT nhin thay duoc (aId da sap theo
#      khoang cach) thay vi xoay vong theo chi so bot.
#   H1/H2: truong nTkSanIdx + reset login.
# Chi KPlayerBot.cpp. AP SAU goi_va_tk_ket_hangdoi.py.
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

# ---- H1: truong nTkSanIdx ----
ap("H1 truong nTkSanIdx",
 '\tint          nTkKetPha;                   // [TK-KET 28/08] pha dang do nhuc nhich\n',
 '\tint          nTkSanIdx;                   // [TK-SAN 28/08] npc idx dich dang san\n'
 '\tint          nTkKetPha;                   // [TK-KET 28/08] pha dang do nhuc nhich\n')

# ---- H2: reset login ----
ap("H2 reset login",
 '\t\tb.nTkKetPha = 0;     b.nTkKetTick = 0;  b.nTkKetX = 0;  b.nTkKetY = 0;\n',
 '\t\tb.nTkKetPha = 0;     b.nTkKetTick = 0;  b.nTkKetX = 0;  b.nTkKetY = 0;\n'
 '\t\tb.nTkSanIdx = 0;\n')

# ---- H3: ham quet dich gan nhat (dat truoc pb_TkLai) ----
ap("H3 pb_TkTimDichGanNhat",
 '// ---------------------------------------------------------------------------\n'
 '// MAY TRANG THAI mot bot trong Tong Kim. Goi tu pb_DriveBot, ngay SAU khoi tu\n',
 '// [TK-SAN 28/08] Tim DOI THU (nguoi/bot phe dich) GAN NHAT con SONG tren cung\n'
 '// ban do. Quet Player[] (toi da MAX_PLAYER, chi cham Npc cua khe dang song) -\n'
 '// re hon nhieu so voi quet ca bang Npc; linh NPC khong tinh la "doi thu" (da co\n'
 '// pb_Fight nhat tren duong). Bo xac (do_death/do_revive) va mau <= 0 theo dung\n'
 '// yeu cau chu game "bo qua muc tieu da chet".\n'
 'static int pb_TkTimDichGanNhat(int nSub, int nNpcIdx, int nCampDich, int* pnX, int* pnY)\n'
 '{\n'
 '\tint bx = 0, by = 0;\n'
 '\tNpc[nNpcIdx].GetMpsPos(&bx, &by);\n'
 '\tint nBest = 0;\n'
 '\t__int64 nBestD = -1;\n'
 '\tfor (int i = 1; i < MAX_PLAYER; i++)\n'
 '\t{\n'
 '\t\tconst int nn = Player[i].m_nIndex;\n'
 '\t\tif (nn <= 0 || nn >= MAX_NPC || nn == nNpcIdx)\n'
 '\t\t\tcontinue;\n'
 '\t\tif (Npc[nn].m_dwID == 0)\n'
 '\t\t\tcontinue;\n'
 '\t\tif (Npc[nn].m_SubWorldIndex != nSub)\n'
 '\t\t\tcontinue;\n'
 '\t\tif ((int)Npc[nn].m_CurrentCamp != nCampDich)\n'
 '\t\t\tcontinue;\n'
 '\t\tif (Npc[nn].m_Doing == do_death || Npc[nn].m_Doing == do_revive)\n'
 '\t\t\tcontinue;              // bo qua muc tieu da chet / dang hoi sinh\n'
 '\t\tif (Npc[nn].m_CurrentLife <= 0)\n'
 '\t\t\tcontinue;\n'
 '\t\tint ex = 0, ey = 0;\n'
 '\t\tNpc[nn].GetMpsPos(&ex, &ey);\n'
 '\t\tconst __int64 dx = (__int64)ex - bx;\n'
 '\t\tconst __int64 dy = (__int64)ey - by;\n'
 '\t\tconst __int64 d = dx * dx + dy * dy;\n'
 '\t\tif (nBestD < 0 || d < nBestD)\n'
 '\t\t{\n'
 '\t\t\tnBestD = d;\n'
 '\t\t\tnBest = nn;\n'
 '\t\t\t*pnX = ex;\n'
 '\t\t\t*pnY = ey;\n'
 '\t\t}\n'
 '\t}\n'
 '\treturn nBest;\n'
 '}\n'
 '\n'
 '// ---------------------------------------------------------------------------\n'
 '// MAY TRANG THAI mot bot trong Tong Kim. Goi tu pb_DriveBot, ngay SAU khoi tu\n')

# ---- H4: khoi san dich trong pha 4 ----
ap("H4 khoi [BotSan]",
 '\t// Het dich gan -> chay qua DOANH TRAI DOI PHUONG tim nguoi danh.\n'
 '\tconst int nCampDich = (b.nTkPhe == 1) ? 2 : 1;\n',
 '\t// Het dich gan -> chay qua DOANH TRAI DOI PHUONG tim nguoi danh.\n'
 '\tconst int nCampDich = (b.nTkPhe == 1) ? 2 : 1;\n'
 '\n'
 '\t// [TK-SAN 28/08] (chu game) "tu dinh vi doi thu o dau va di chuyen THANG toi\n'
 '\t// danh luon; tren duong gap muc tieu khac thi bat muc tieu GAN NHAT, khong co\n'
 '\t// dinh; bo qua muc tieu chet". Moi ~1,2 giay (so le theo chi so bot, + ngay khi\n'
 '\t// chua co dich) dinh vi lai doi thu gan nhat con song: co -> dat dich = vi tri\n'
 '\t// no, di THANG (khong vong trung gian), doi con khac gan hon / no doi cho > 8 o\n'
 '\t// thi tinh lai duong. Khong thay ai (an nap/chet het) -> roi xuong loi cu:\n'
 '\t// lang thang ve doanh trai dich. Khoi nay chi chay khi KHONG co muc tieu dang\n'
 '\t// danh (b.nTargetNpc da return o tren) nen khong pha luat "dang danh khong\n'
 '\t// dong lo trinh".\n'
 '\tif (((now + (unsigned int)nLech) % 21u) == 0 || b.nTkDichTick == 0)\n'
 '\t{\n'
 '\t\tint nSanX = 0, nSanY = 0;\n'
 '\t\tconst int nSan = pb_TkTimDichGanNhat(nSub, nNpcIdx, nCampDich, &nSanX, &nSanY);\n'
 '\t\tif (nSan > 0)\n'
 '\t\t{\n'
 '\t\t\tconst int bDoiDich = (nSan != b.nTkSanIdx)\n'
 '\t\t\t                  || (g_GetDistance(nSanX, nSanY, b.nTkDichX, b.nTkDichY) > 8 * 32);\n'
 '\t\t\tb.nTkSanIdx = nSan;\n'
 '\t\t\tif (bDoiDich)\n'
 '\t\t\t{\n'
 '\t\t\t\tb.nTkDichX = nSanX;\n'
 '\t\t\t\tb.nTkDichY = nSanY;\n'
 '\t\t\t\tb.nTkDaBoc = 1;            // san = di THANG, bo chang vong trung gian\n'
 '\t\t\t\tb.nTkTgX = 0;  b.nTkTgY = 0;\n'
 '\t\t\t\tb.walk.Reset();\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tstatic unsigned int s_uLogSan = 0;\n'
 '\t\t\t\t\tif (now - s_uLogSan >= (unsigned int)(GAME_FPS * 2))\n'
 '\t\t\t\t\t{\n'
 '\t\t\t\t\t\ts_uLogSan = now;\n'
 '\t\t\t\t\t\tint bxS = 0, byS = 0;\n'
 '\t\t\t\t\t\tNpc[nNpcIdx].GetMpsPos(&bxS, &byS);\n'
 '\t\t\t\t\t\tpb_Log("[BotSan] %s phe %d duoi %s o(%d,%d) cach %d o\\n",\n'
 '\t\t\t\t\t\t       Player[nIdx].m_PlayerName, b.nTkPhe,\n'
 '\t\t\t\t\t\t       Npc[nSan].Name, nSanX / 32, nSanY / 32,\n'
 '\t\t\t\t\t\t       g_GetDistance(bxS, byS, nSanX, nSanY) / 32);\n'
 '\t\t\t\t\t}\n'
 '\t\t\t\t}\n'
 '\t\t\t}\n'
 '\t\t\tb.nTkDichTick = now;           // dang san -> khoa nhanh boc doanh trai\n'
 '\t\t}\n'
 '\t\telse\n'
 '\t\t\tb.nTkSanIdx = 0;               // khong thay doi thu -> di doanh trai nhu cu\n'
 '\t}\n')

# ---- H5: bo nham TK lay gan nhat nhin thay duoc ----
ap("H5 nham gan nhat trong TK",
 '\tif (bTrongTK)\n'
 '\t{\n'
 '\t\tfor (int v = 0; v < nCand; v++)\n'
 '\t\t{\n'
 '\t\t\tconst int i3 = aId[((unsigned)nLech + (unsigned)v) % (unsigned)nCand];\n',
 '\tif (bTrongTK)\n'
 '\t{\n'
 '\t\t// [TK-SAN 28/08] (chu game) "bat muc tieu GAN NHAT": aId da sap tang dan theo\n'
 '\t\t// khoang cach -> duyet tu dau la gan-nhat-nhin-thay-duoc. Bo loi xoay vong\n'
 '\t\t// theo chi so bot (chong dan don cuc 23/08) vi trai y chu game.\n'
 '\t\tfor (int v = 0; v < nCand; v++)\n'
 '\t\t{\n'
 '\t\t\tconst int i3 = aId[(unsigned)v % (unsigned)nCand];\n',)

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
