# -*- coding: utf-8 -*-
# [BotNoi-BAT 06/09] Chu game: "tim hieu phan chia bot theo noi ngoai - hien tai da so bot
# la ngoai cong; do phan add vu khi cho bot - bot khong co vu khi la noi cong nhung hien tai
# da so bot la co vu khi nen khong danh skill noi; rieng bot he kim Thien Vuong thi toan bo
# la ngoai cong phai co vu khi; rieng bot he moc Duong Mon toan bo phai co vu khi moi danh
# duoc; nen luu y 2 phai do tranh xoa vu khi nham dan toi danh khong duoc ky nang".
#
# GOC (do KPlayerBot.cpp + bot.log 05-06/09):
#   1. PB_BAT_DUONG_NOI = 0 tu [NOI-HOAN 31/08] -> pb_BotNoiThat() luon 0 -> duong NOI
#      (thao vu khi, khong phat lai) CHUA BAO GIO chay; [BotNoi] = 0 dong.
#   2. Nhanh phat-lai vu khi (10 s, gion 60 s, tran 5 lan) boc lai pool ngau nhien cho MOI
#      bot tay khong -> bot boc trung "duong quyen" (PB_WPN_NONE) cung bi ep cam vu khi
#      (05/09: 31 dong "duong QUYEN" nhung 69 dong "tay khong giua doi -> phat lai").
#   3. 1000 bot dang chay deu la bot cu nap lai (nGaveWeapon=1, vu khi trong blob).
#   => [BotDiem] 05-06/09: 1395/1395 lan chia diem deu la mau NGOAI (SM=2 TP=1 SK=2) hoac
#      Duong Mon (TP=3 SK=2); 0 lan mau NOI (SM=1 TP=0 SK=4) / Vo Dang NC=3.
#
# SUA (3 hunk, chi KPlayerBot.cpp):
#   H1  PB_BAT_DUONG_NOI 0 -> 1 + pb_PhaiLuonCamVuKhi(): Thien Vuong (1) + Duong Mon (2)
#       chan CUNG. BAT BUOC vi do lai (quet_noi.py, cap 110): DM co chieu 90 id 351
#       (eqt -2, co don phep) -> bo quet du lieu se cho DM cap >= 80 di duong noi = thao
#       vu khi = khong danh duoc - dung loi chu game canh bao.
#   H2  pb_BotNoiThat(): kiem phai truoc khi quet chieu.
#   H3  nhanh phat-lai vu khi: xet !pb_BotNoiThat SAU cung (chi quet chieu khi da tay khong).
#   H4  log [BotNoi] thao vu khi ghi them ten phai (nghiem thu: khong duoc co TV/DM).
# Idempotent. Duong dan = tham so 1 (mac dinh cay worktree botnoi-0609).
import io, sys
P = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW_wt_botnoi\Sources\Core\Src\KPlayerBot.cpp"

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

ap("H1 bat cong tac + pb_PhaiLuonCamVuKhi",
 '// => TAM NGAT, giu nguyen hanh vi dang chay. Doi 0 -> 1 khi muon test rieng.\n'
 '#define PB_BAT_DUONG_NOI  0\n',
 '// => TAM NGAT, giu nguyen hanh vi dang chay. Doi 0 -> 1 khi muon test rieng.\n'
 '// [BotNoi-BAT 06/09] Chu game: "hien tai da so bot la ngoai cong ... bot khong co vu khi\n'
 '// la noi cong nhung hien tai da so bot la co vu khi nen khong danh skill noi" -> BAT.\n'
 '// Do lai (ReverseTools/quet_noi.py: skills.txt x SKILLNORMAL + SKILL90, cap 110):\n'
 '//   co chieu noi tay khong: TL 271/318 (eqt -1 = PHAI tay khong), NM 80/82/91/380,\n'
 '//   TY 102/113/111/337, CB 122/128/357, TN 145/138/148/362, VD 153/164/165/365,\n'
 '//   CL 179/182/375 (eqt -2); Ngu Doc chi co don DOC (bi loai 303-DOC) -> giu vu khi;\n'
 '//   Hoa Son / Vu Hon / Tieu Dao khong co chieu tay khong nao -> giu vu khi.\n'
 '//   DUONG MON: chieu 90 id 351 (eqt -2) CO don phep -> neu chi dua vao bo quet thi DM\n'
 '//   cap >= 80 se bi thao vu khi = khong danh duoc (dung loi chu game canh bao)\n'
 '//   -> chan CUNG bang pb_PhaiLuonCamVuKhi, khong phu thuoc du lieu.\n'
 '#define PB_BAT_DUONG_NOI  1\n'
 '\n'
 '// [BotNoi-BAT 06/09] Phai TOAN ky nang ngoai cong / tam xa CAN vu khi. Chu game: "bot he\n'
 '// kim Thien Vuong toan bo la ngoai cong phai co vu khi; bot he moc Duong Mon toan bo phai\n'
 '// co vu khi moi danh duoc; luu y 2 phai do tranh xoa vu khi nham". Chan cung o day: moi\n'
 '// noi quyet dinh thao / khong phat vu khi deu di qua pb_BotNoiThat -> ham nay.\n'
 'static int pb_PhaiLuonCamVuKhi(int nFaction)\n'
 '{\n'
 '\treturn nFaction == 1 || nFaction == 2;   // 1 Thien Vuong, 2 Duong Mon\n'
 '}\n')

ap("H2 pb_BotNoiThat kiem phai truoc khi quet",
 '#if PB_BAT_DUONG_NOI\n'
 '\tif (!pb_BotNoi(nIdx))\n'
 '\t\treturn 0;\n'
 '\tconst int nNpcIdx = Player[nIdx].m_nIndex;\n',
 '#if PB_BAT_DUONG_NOI\n'
 '\tif (!pb_BotNoi(nIdx))\n'
 '\t\treturn 0;\n'
 '\t// [BotNoi-BAT 06/09] Thien Vuong / Duong Mon: KHONG BAO GIO di duong noi, bat ke\n'
 '\t// bo quet chieu noi ra gi (DM co 351 eqt -2 mang don phep o cap 80).\n'
 '\tif (pb_PhaiLuonCamVuKhi((int)Player[nIdx].m_cFaction.m_nCurFaction))\n'
 '\t\treturn 0;\n'
 '\tconst int nNpcIdx = Player[nIdx].m_nIndex;\n')

ap("H3 phat-lai vu khi: quet chieu sau cung",
 '\t\t\tif (b.nGaveWeapon && b.nFaction >= 0\n'
 '\t\t\t && !pb_BotNoiThat(nIdx)   // [BotNoi-PHAI] chi bot noi THAT (phai co chieu noi) moi mien\n'
 '\t\t\t && Player[nIdx].m_ItemList.GetWeaponType() < 0\n'
 '\t\t\t && b.nVuKhiThu < 5\n'
 '\t\t\t && nowAll - b.nVuKhiTick >= (unsigned int)(GAME_FPS * 60))\n',
 '\t\t\tif (b.nGaveWeapon && b.nFaction >= 0\n'
 '\t\t\t && Player[nIdx].m_ItemList.GetWeaponType() < 0\n'
 '\t\t\t && b.nVuKhiThu < 5\n'
 '\t\t\t && nowAll - b.nVuKhiTick >= (unsigned int)(GAME_FPS * 60)\n'
 '\t\t\t // [BotNoi-PHAI] chi bot noi THAT (phai co chieu noi) moi mien phat lai;\n'
 '\t\t\t // [BotNoi-BAT 06/09] xet SAU CUNG: bo quet chieu chi chay khi bot da tay khong\n'
 '\t\t\t && !pb_BotNoiThat(nIdx))\n')

ap("H4 log [BotNoi] thao vu khi kem ten phai",
 '\t\t\tpb_Log("[BotNoi] %s cap %d: thao vu khi (duong NOI CONG danh tay khong)\\n",\n'
 '\t\t\t       Player[nIdx].m_PlayerName, nLevel);\n',
 '\t\t\tpb_Log("[BotNoi] %s phai %s cap %d: thao vu khi (duong NOI CONG danh tay khong)\\n",\n'
 '\t\t\t       Player[nIdx].m_PlayerName,\n'
 '\t\t\t       (b.nFaction >= 0 && b.nFaction < MAX_FACTION) ? s_facNpc[b.nFaction].szTen : "?",\n'
 '\t\t\t       nLevel);\n')

if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
sau = sum(1 for c in s if ord(c) > 127)
print("da ap %d hunk; high-byte truoc=%d sau=%d %s" % (n, truoc, sau, "OK" if truoc == sau else "LECH!"))
