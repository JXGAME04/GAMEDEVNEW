# -*- coding: utf-8 -*-
# [NOI-HOAN 31/08] CHAN RUI RO do workflow phan bien phat hien (truoc khi chu restart):
# Ban va [SPARSE 31/08] sua phep doc mang thua -> HOI SINH pb_CoChieuNoiTayKhong
# (truoc do luon tra 0, nen duong noi/ngoai 28/08 NAM IM: "[BotNoi]" = 0 dong tren
# toan bo 115 MB bot.log). Dau ra cua no gac khoi HUY VU KHI trong pb_TrangBiTheoCap
# (RemoveItemIdx(vu khi) - KPlayerBot.cpp ~2820-2831), va pb_GiveFactionWeapon cung
# return som => bot bi thao vu khi VINH VIEN.
# Do (skills.txt x factionhead.lua SKILLNORMAL, cap bot 110): 8/10 phai co chieu phep
# tay khong hop le (TL 271 / NM 80,82,91 / TY 102,113,111 / CB 122,128 / TN 145,138,148
# / VD 153,164,165 / CL 179,182); Duong Mon + Thien Vuong an toan. Voi pb_BotNoi =
# dwID&1 (~50%) => ~400/1000 bot bi huy vu khi NGAY lan pb_TrangBiTheoCap dau tien sau
# restart (gate nTrangBiLevel = 0 luc login).
# => Chu game CHUA duyet doi hanh vi nay, va no khong lien quan gi den loi "bot Duong
#    Mon dung yen" dang sua. TAM NGAT bang cong tac bien dich, GIU NGUYEN hanh vi da
#    chay tu 28/08. Doi 0 -> 1 khi chu muon test rieng duong noi cong.
# Chi KPlayerBot.cpp. AP SAU goi_va_damageattrib_thua.py.
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

ap("H1 cong tac PB_BAT_DUONG_NOI",
 '// duong NOI THAT SU = dwID le VA phai co chieu noi tay khong dung duoc ngay bay gio\n'
 'static int pb_BotNoiThat(int nIdx)\n'
 '{\n'
 '\tif (!pb_BotNoi(nIdx))\n'
 '\t\treturn 0;\n'
 '\tconst int nNpcIdx = Player[nIdx].m_nIndex;\n'
 '\tif (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)\n'
 '\t\treturn 0;\n'
 '\treturn pb_CoChieuNoiTayKhong(nNpcIdx);\n'
 '}\n',
 '// [NOI-HOAN 31/08] Duong NOI CONG (28/08) tren thuc te NAM IM tu dau: ban cu quet\n'
 '// "a < GetDamageAttribsNum()" tren MANG THUA nen pb_CoChieuNoiTayKhong luon tra 0\n'
 '// ("[BotNoi]" = 0 dong tren toan bo 115 MB bot.log). Ban va [SPARSE 31/08] lam no\n'
 '// SONG LAI - ma dau ra cua no gac khoi HUY VU KHI trong pb_TrangBiTheoCap\n'
 '// (RemoveItemIdx) va chan pb_GiveFactionWeapon phat lai.\n'
 '// Do that (skills.txt x factionhead.lua SKILLNORMAL, cap bot 110): 8/10 phai co\n'
 '// chieu phep tay khong hop le (TL 271 / NM 80,82,91 / TY 102,113,111 / CB 122,128 /\n'
 '// TN 145,138,148 / VD 153,164,165 / CL 179,182). Voi pb_BotNoi = dwID&1 (~50%%) thi\n'
 '// ~400/1000 bot se bi huy vu khi NGAY lan pb_TrangBiTheoCap dau sau restart.\n'
 '// Chu game CHUA duyet, va viec do KHONG lien quan loi "bot Duong Mon dung yen".\n'
 '// => TAM NGAT, giu nguyen hanh vi dang chay. Doi 0 -> 1 khi muon test rieng.\n'
 '#define PB_BAT_DUONG_NOI  0\n'
 '\n'
 '// duong NOI THAT SU = dwID le VA phai co chieu noi tay khong dung duoc ngay bay gio\n'
 'static int pb_BotNoiThat(int nIdx)\n'
 '{\n'
 '#if PB_BAT_DUONG_NOI\n'
 '\tif (!pb_BotNoi(nIdx))\n'
 '\t\treturn 0;\n'
 '\tconst int nNpcIdx = Player[nIdx].m_nIndex;\n'
 '\tif (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)\n'
 '\t\treturn 0;\n'
 '\treturn pb_CoChieuNoiTayKhong(nNpcIdx);\n'
 '#else\n'
 '\t(void)nIdx;   // [NOI-HOAN 31/08] xem ghi chu ngay tren\n'
 '\treturn 0;\n'
 '#endif\n'
 '}\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
