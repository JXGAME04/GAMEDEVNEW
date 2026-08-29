# -*- coding: utf-8 -*-
# [TK-TANRA + TK-KET2 28/08] Hai phan hoi cua chu sau tran 18:0x:
# (1) "rat nhieu bot GOM vao chay dung 1 duong, chua tan ra chia nhieu doi thu"
#     Do that (bot.log 18:06): moi cu san deu tro ve cung tum dich o(1542-1549,
#     3215-3231) - "gan nhat con slot" cua ca dan nam trong MOT cum -> mot lan duong.
#     Va H1/H2: gom TOP-25 ung vien gan nhat, MOI BOT nhan HANG rieng (nLech % so
#     ung vien) -> ~10 con/dich, 25 dich rai nhieu canh -> nhieu lan duong.
# (2) "bot phe Tong van thoat khi vao tran"
#     Do that (bot.log 18:05): 81 cu "KET o pha 3" du ban chay DA co refresh-nhuc-
#     nhich -> dam nay DUNG IM that: dau tran 500 con cung xin duong A* (hang doi
#     PathSrv) -> duoi hang doi cho cap duong >120s khong nhuc nhich.
#     Va H3: TRONG HAU DOANH phe minh = vung an toan, lam tuoi dong ho (van chiu
#     tran cung 6x han pha; ra khoi trai dung im van bi cat). H4/H5: tiet luu
#     re-path cua [BotSan] (doi han muc tieu moi reset ngay; muc tieu cu doi cho
#     thi >=3s moi re-path) -> giam ngay ap luc hang doi duong.
# AP SAU goi_va_tk_san_tran10.py. Chi KPlayerBot.cpp.
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

# ---- H1: thay ruot ham quet: gan-nhat -> top-25 + hang theo bot ----
ap("H1 top-25 + hang rieng",
 'static int pb_TkTimDichGanNhat(int nSub, int nNpcIdx, int nCampDich, int nTuIdx, int* pnX, int* pnY)\n'
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
 '\t\tif (nn != nTuIdx && s_nTkSanDem[nn] >= PB_TK_SAN_TRAN)\n'
 '\t\t\tcontinue;              // [TK-SAN10] da du 10 con san no -> chon con gan ke tiep\n'
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
 '}\n',
 'static int pb_TkTimDichGanNhat(int nSub, int nNpcIdx, int nCampDich, int nTuIdx, int nLech, int* pnX, int* pnY)\n'
 '{\n'
 '\tint bx = 0, by = 0;\n'
 '\tNpc[nNpcIdx].GetMpsPos(&bx, &by);\n'
 '\t// [TK-TANRA 28/08] (chu game) "bot gom chay dung 1 duong, chua tan ra": lay\n'
 '\t// gan-nhat tuyet doi lam ca dan do ve cung mot tum dich -> mot lan duong.\n'
 '\t// Nay gom TOP-25 ung vien gan nhat roi MOI BOT nhan HANG rieng theo chi so\n'
 '\t// (nLech modulo so ung vien): 250 con/phe chia ~10 con moi dich, 25 dich rai\n'
 '\t// nhieu canh -> nhieu lan duong. Cham mat giua duong van danh gan nhat (pb_Fight).\n'
 '#define PB_TK_SAN_TOPK 25\n'
 '\tint     aC[PB_TK_SAN_TOPK];\n'
 '\t__int64 aD[PB_TK_SAN_TOPK];\n'
 '\tint nCand = 0;\n'
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
 '\t\tif (nn != nTuIdx && s_nTkSanDem[nn] >= PB_TK_SAN_TRAN)\n'
 '\t\t\tcontinue;              // [TK-SAN10] da du 10 con san no -> bo qua\n'
 '\t\tint ex = 0, ey = 0;\n'
 '\t\tNpc[nn].GetMpsPos(&ex, &ey);\n'
 '\t\tconst __int64 dx = (__int64)ex - bx;\n'
 '\t\tconst __int64 dy = (__int64)ey - by;\n'
 '\t\tconst __int64 d = dx * dx + dy * dy;\n'
 '\t\tif (nCand < PB_TK_SAN_TOPK)\n'
 '\t\t{\n'
 '\t\t\tint pos = nCand++;\n'
 '\t\t\twhile (pos > 0 && aD[pos - 1] > d)\n'
 '\t\t\t{\n'
 '\t\t\t\taC[pos] = aC[pos - 1];  aD[pos] = aD[pos - 1];  pos--;\n'
 '\t\t\t}\n'
 '\t\t\taC[pos] = nn;  aD[pos] = d;\n'
 '\t\t}\n'
 '\t\telse if (d < aD[PB_TK_SAN_TOPK - 1])\n'
 '\t\t{\n'
 '\t\t\tint pos = PB_TK_SAN_TOPK - 1;\n'
 '\t\t\twhile (pos > 0 && aD[pos - 1] > d)\n'
 '\t\t\t{\n'
 '\t\t\t\taC[pos] = aC[pos - 1];  aD[pos] = aD[pos - 1];  pos--;\n'
 '\t\t\t}\n'
 '\t\t\taC[pos] = nn;  aD[pos] = d;\n'
 '\t\t}\n'
 '\t}\n'
 '\tif (nCand <= 0)\n'
 '\t\treturn 0;\n'
 '\tconst int nBest = aC[(unsigned)nLech % (unsigned)nCand];\n'
 '\tNpc[nBest].GetMpsPos(pnX, pnY);\n'
 '\treturn nBest;\n'
 '}\n')

# ---- H2: callsite them nLech ----
ap("H2 callsite nLech",
 '\t\tconst int nSan = pb_TkTimDichGanNhat(nSub, nNpcIdx, nCampDich, b.nTkSanIdx, &nSanX, &nSanY);\n',
 '\t\tconst int nSan = pb_TkTimDichGanNhat(nSub, nNpcIdx, nCampDich, b.nTkSanIdx, nLech, &nSanX, &nSanY);\n')

# ---- H3: hau doanh = vung an toan cho dong ho KET ----
ap("H3 hau doanh an toan",
 '\t\t\tif (nDx9 * nDx9 + nDy9 * nDy9 >= 36)\n'
 '\t\t\t{\n'
 '\t\t\t\tb.nTkKetX = nKx9;\n'
 '\t\t\t\tb.nTkKetY = nKy9;\n'
 '\t\t\t\tb.nTkTick = now;               // co tien trien that -> chua phai ket\n'
 '\t\t\t}\n',
 '\t\t\tif (nDx9 * nDx9 + nDy9 * nDy9 >= 36)\n'
 '\t\t\t{\n'
 '\t\t\t\tb.nTkKetX = nKx9;\n'
 '\t\t\t\tb.nTkKetY = nKy9;\n'
 '\t\t\t\tb.nTkTick = now;               // co tien trien that -> chua phai ket\n'
 '\t\t\t}\n'
 '\t\t\t// [TK-KET2 28/08] (chu game: "bot phe Tong van thoat khi vao tran") dot\n'
 '\t\t\t// 18:03-18:05 con 81 cu KET pha 3 du da co refresh-nhuc-nhich: dau tran\n'
 '\t\t\t// 500 con cung xin duong A* (hang doi PathSrv) -> duoi hang doi DUNG IM\n'
 '\t\t\t// cho cap duong >120s. TRONG HAU DOANH phe minh la vung an toan: cho o\n'
 '\t\t\t// day khong phai "ket" -> lam tuoi dong ho (van chiu tran cung 6x; ra\n'
 '\t\t\t// khoi trai ma dung im thi van bi cat nhu cu).\n'
 '\t\t\telse if (b.nTk == 3 && nMapNay == PB_TK_MAP)\n'
 '\t\t\t{\n'
 '\t\t\t\tstatic const int aKHx[2] = { 1229, 1689 };   // TKPOS_GO_HDOANH (o)\n'
 '\t\t\t\tstatic const int aKHy[2] = { 3561, 3074 };\n'
 '\t\t\t\tconst int iT9 = pb_TkDaoTheTran(nSub);\n'
 '\t\t\t\tconst int h9  = (b.nTkPhe == 1) ? iT9 : (1 - iT9);\n'
 '\t\t\t\tint dxT = nKx9 - aKHx[h9] * 32;  if (dxT < 0) dxT = -dxT;\n'
 '\t\t\t\tint dyT = nKy9 - aKHy[h9] * 32;  if (dyT < 0) dyT = -dyT;\n'
 '\t\t\t\tif (dxT <= 40 * 32 && dyT <= 40 * 32)\n'
 '\t\t\t\t\tb.nTkTick = now;\n'
 '\t\t\t}\n')

# ---- H4: tiet luu re-path cua [BotSan] ----
ap("H4 tiet luu repath",
 '\t\t\tconst int bDoiDich = (nSan != b.nTkSanIdx)\n'
 '\t\t\t                  || (g_GetDistance(nSanX, nSanY, b.nTkDichX, b.nTkDichY) > 8 * 32);\n',
 '\t\t\t// [TK-KET2 28/08] tiet luu tinh-lai-duong: dich DOI HAN moi reset ngay;\n'
 '\t\t\t// dich cu chi doi cho thi toi thieu 3 giay moi re-path mot lan (dau tran\n'
 '\t\t\t// 500 con cung xin A* lam nghen hang doi duong -> pha 3 cho duong qua 120s).\n'
 '\t\t\tconst int bDoiDich = (nSan != b.nTkSanIdx)\n'
 '\t\t\t                  || (g_GetDistance(nSanX, nSanY, b.nTkDichX, b.nTkDichY) > 8 * 32\n'
 '\t\t\t                   && now - b.nTkTgTick >= (unsigned int)(GAME_FPS * 3));\n')

# ---- H5: ghi moc repath ----
ap("H5 ghi moc repath",
 '\t\t\t\tb.nTkDaBoc = 1;            // san = di THANG, bo chang vong trung gian\n'
 '\t\t\t\tb.nTkTgX = 0;  b.nTkTgY = 0;\n'
 '\t\t\t\tb.walk.Reset();\n',
 '\t\t\t\tb.nTkDaBoc = 1;            // san = di THANG, bo chang vong trung gian\n'
 '\t\t\t\tb.nTkTgX = 0;  b.nTkTgY = 0;\n'
 '\t\t\t\tb.nTkTgTick = now;         // [TK-KET2] moc tiet luu re-path\n'
 '\t\t\t\tb.walk.Reset();\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
