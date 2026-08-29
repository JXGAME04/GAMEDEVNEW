# -*- coding: utf-8 -*-
# [TK-KET 28/08] Chu game: "500 bot vao Tong Kim, vai phut sau ~100 con TU THOAT,
# chi thoat MOT phe". Do that (bot.log 17:19-17:21): 250+250 bao danh cung luc;
# 85/250 phe TONG dinh "KET o pha 3 qua 120 giay -> bo cuoc" (0 con phe Kim),
# ca 85 chua tung "RA TRAN". Goc: pha 3 don 250 con/phe ve MOT toa do Quan Y roi
# MOT cua trai (pheu); dong ho PB_TK_PHA_HAN khong duoc lam tuoi suot doan di bo
# nen DUOI HANG DOI (co tien nhung cham) bi coi la ket -> cat oan nguyen mot vet
# cua phe co trai chat hon.
# Va: H3 con nao NHUC NHICH >= 6 o thi lam tuoi dong ho pha (ket that - dung im /
#     quanh quan <6 o - van bi cat 120s; tran cung 6 x han pha = 12' chong treo);
#     H4 rai diem den Quan Y +-8 o theo chi so bot (bot_tk_muamau la ExecuteScript,
#     khong doi dung sat NPC) de pha pheu tu goc.
# Chi KPlayerBot.cpp. AP SAU chuoi bot hien co.
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

# ---- H1: 4 truong theo doi nhuc nhich ----
ap("H1 truong nTkKet*",
 '\tint          nTkGoiThu;                   // so lan da goi script bao danh\n',
 '\tint          nTkGoiThu;                   // so lan da goi script bao danh\n'
 '\tint          nTkKetPha;                   // [TK-KET 28/08] pha dang do nhuc nhich\n'
 '\tunsigned int nTkKetTick;                  // moc vao pha (tran cung 6 x han pha)\n'
 '\tint          nTkKetX;                     // mau vi tri lan truoc (mps)\n'
 '\tint          nTkKetY;\n')

# ---- H2: reset o login ----
ap("H2 reset login",
 '\t\tb.nVuKhiTick = 0;    b.nVuKhiThu = 0;                     // rac cu -> phai xoa\n',
 '\t\tb.nVuKhiTick = 0;    b.nVuKhiThu = 0;                     // rac cu -> phai xoa\n'
 '\t\tb.nTkKetPha = 0;     b.nTkKetTick = 0;  b.nTkKetX = 0;  b.nTkKetY = 0;\n')

# ---- H3: lam tuoi dong ho khi con nhuc nhich ----
ap("H3 nhuc nhich = chua ket",
 '\tif (now - b.nTkTick > (unsigned int)PB_TK_PHA_HAN && b.nTk < 4)\n'
 '\t{\n'
 '\t\tpb_Log("[BotTK] %s KET o pha %d qua %d giay (map %d) -> bo cuoc, tra trang thai\\n",\n',
 '\t// [TK-KET 28/08] DANG DI CHUYEN (hang doi 250 con/phe don ve Quan Y + cua trai)\n'
 '\t// khong phai "ket": 17:19-17:21 hom nay 85/250 phe Tong xep hang qua 120s bi coi\n'
 '\t// la ket -> bo cuoc mot loat, tran lech han mot phe (0 con Kim dinh). Con nao\n'
 '\t// NHUC NHICH >= 6 o ke tu mau truoc thi lam tuoi dong ho pha; dung im / quanh\n'
 '\t// quan < 6 o van bi cat 120s nhu cu; tran cung 6 x han pha (12\') chong treo\n'
 '\t// vinh vien neu bot bi day qua lai lien tuc.\n'
 '\tif (b.nTk >= 1 && b.nTk <= 3)\n'
 '\t{\n'
 '\t\tif (b.nTkKetPha != b.nTk)\n'
 '\t\t{\n'
 '\t\t\tb.nTkKetPha  = b.nTk;\n'
 '\t\t\tb.nTkKetTick = now;\n'
 '\t\t\tNpc[nNpcIdx].GetMpsPos(&b.nTkKetX, &b.nTkKetY);\n'
 '\t\t}\n'
 '\t\telse if (now - b.nTkKetTick <= (unsigned int)(PB_TK_PHA_HAN * 6))\n'
 '\t\t{\n'
 '\t\t\tint nKx9 = 0, nKy9 = 0;\n'
 '\t\t\tNpc[nNpcIdx].GetMpsPos(&nKx9, &nKy9);\n'
 '\t\t\tconst int nDx9 = (nKx9 - b.nTkKetX) / 32;\n'
 '\t\t\tconst int nDy9 = (nKy9 - b.nTkKetY) / 32;\n'
 '\t\t\tif (nDx9 * nDx9 + nDy9 * nDy9 >= 36)\n'
 '\t\t\t{\n'
 '\t\t\t\tb.nTkKetX = nKx9;\n'
 '\t\t\t\tb.nTkKetY = nKy9;\n'
 '\t\t\t\tb.nTkTick = now;               // co tien trien that -> chua phai ket\n'
 '\t\t\t}\n'
 '\t\t}\n'
 '\t}\n'
 '\tif (now - b.nTkTick > (unsigned int)PB_TK_PHA_HAN && b.nTk < 4)\n'
 '\t{\n'
 '\t\tpb_Log("[BotTK] %s KET o pha %d qua %d giay (map %d) -> bo cuoc, tra trang thai\\n",\n')

# ---- H4: rai diem den Quan Y ----
ap("H4 rai diem Quan Y",
 '\t\t\t\tconst int iQ     = (b.nTkPhe == 1) ? iTong : (1 - iTong);\n'
 '\t\t\t\tconst int nWq = PB_WalkTo(nNpcIdx, aQx[iQ], aQy[iQ], nSub, b.walk,\n'
 '\t\t\t\t                          PB_FAC_ARRIVE_MPS);\n',
 '\t\t\t\tconst int iQ     = (b.nTkPhe == 1) ? iTong : (1 - iTong);\n'
 '\t\t\t\t// [TK-KET 28/08] 250 con/phe cung don ve MOT toa do Quan Y = pheu tac,\n'
 '\t\t\t\t// duoi hang doi vuot 120s. Rai diem den +-8 o theo chi so bot (mua thuoc\n'
 '\t\t\t\t// la ExecuteScript, khong doi dung sat NPC), loc o dat bang pb_ODat.\n'
 '\t\t\t\tint nQtx = aQx[iQ] + ((nLech % 9) - 4) * 2 * 32;\n'
 '\t\t\t\tint nQty = aQy[iQ] + (((nLech / 9) % 7) - 3) * 2 * 32;\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tint nQvx = 0, nQvy = 0;\n'
 '\t\t\t\t\tif (pb_ODat(nSub, nQtx / 32, nQty / 32, nLech, 6, &nQvx, &nQvy))\n'
 '\t\t\t\t\t{\n'
 '\t\t\t\t\t\tnQtx = nQvx;\n'
 '\t\t\t\t\t\tnQty = nQvy;\n'
 '\t\t\t\t\t}\n'
 '\t\t\t\t}\n'
 '\t\t\t\tconst int nWq = PB_WalkTo(nNpcIdx, nQtx, nQty, nSub, b.walk,\n'
 '\t\t\t\t                          PB_FAC_ARRIVE_MPS);\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
