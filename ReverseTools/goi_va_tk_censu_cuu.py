# -*- coding: utf-8 -*-
# [TK-CENSUS + TK-CUU3 28/08] Chu game: "bot ket tai doanh trai nhieu, truoc no di
# toi gan trap roi goi script ra ngoai, gio DUNG YEN".
# Do 3 tran: ty le bao-danh-xong-KHONG-bao-gio-ra = 25% (17:19, ban TRUOC moi va TK
# chieu nay) / 12% (18:03) / 16% (19:05) -> loi TON DONG tu truoc, 500 bot lam lo ro
# (60-140 con/tran). Manh moi cung: [BotKet] "bo cuoc sau 3 lan: doing=3 procAI=1"
# = NPC dang do_run ma khong nhuc nhich => dung tren O BI CHAN theo luoi server
# (NewWorld cua script bao danh tha khong kiem vat can), GetDir=0 moi tick.
# Va 2 mieng:
#   H1 [TK-CENSUS] trong pb_TkNhip (10 giay/lan khi co tran mo): dem bot theo pha
#      + rieng pha 3 dem theo map; mo xe toi da 3 con pha-3 o lau: ten, map, o,
#      doing, procAI, obs (CellObsSrv), ChoRa-delta, MuaXong -> tran sau doc log la
#      biet dich xac giai phau ket.
#   H2 [TK-CUU3] dau pha 3 (sau gate ChoRa): dung tren o CHAN theo luoi server
#      (CellObsSrv != 0) trong khu hau doanh -> SetPos sang o dat gan do (pb_ODat
#      quanh vi tri roi anchor trai) - dung khuon [BotTHP]. Log dem duoc.
# Chi KPlayerBot.cpp. AP SAU goi_va_tk_chia_hoaluc.py.
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

# ---- H1: census 10s trong pb_TkNhip (ngay sau khoi rebuild bang dem san) ----
ap("H1 census 10s",
 '\t// [TK-SAN10] dung lai bang dem san-muc-tieu tu dau moi giay: tu don rac khi\n'
 '\t// bot chet / roi tran / doi pha ma khong can moc vao tung duong thoat.\n'
 '\t{\n'
 '\t\tmemset(s_nTkSanDem, 0, sizeof(s_nTkSanDem));\n'
 '\t\tfor (int i9 = 0; i9 < s_botCount; i9++)\n'
 '\t\t\tif (s_bots[i9].nTk == 4 && s_bots[i9].nTkSanIdx > 0\n'
 '\t\t\t && s_bots[i9].nTkSanIdx < MAX_NPC)\n'
 '\t\t\t\ts_nTkSanDem[s_bots[i9].nTkSanIdx]++;\n'
 '\t}\n',
 '\t// [TK-SAN10] dung lai bang dem san-muc-tieu tu dau moi giay: tu don rac khi\n'
 '\t// bot chet / roi tran / doi pha ma khong can moc vao tung duong thoat.\n'
 '\t{\n'
 '\t\tmemset(s_nTkSanDem, 0, sizeof(s_nTkSanDem));\n'
 '\t\tfor (int i9 = 0; i9 < s_botCount; i9++)\n'
 '\t\t\tif (s_bots[i9].nTk == 4 && s_bots[i9].nTkSanIdx > 0\n'
 '\t\t\t && s_bots[i9].nTkSanIdx < MAX_NPC)\n'
 '\t\t\t\ts_nTkSanDem[s_bots[i9].nTkSanIdx]++;\n'
 '\t}\n'
 '\n'
 '\t// [TK-CENSUS 28/08] 10 giay/lan: dem bot theo pha + mo xe toi da 3 con pha-3\n'
 '\t// dung lau nhat (chu game: "bot ket trong doanh trai, dung yen") - tran sau\n'
 '\t// doc bot.log la thay dich xac giai phau: o nao, doing gi, procAI, o co bi\n'
 '\t// CHAN theo luoi server khong, dang cho ChoRa hay chua mua thuoc.\n'
 '\t{\n'
 '\t\tstatic DWORD s_dwCen = 0;\n'
 '\t\tif (dwNow - s_dwCen >= 10000)\n'
 '\t\t{\n'
 '\t\t\ts_dwCen = dwNow;\n'
 '\t\t\tint nPha[6] = { 0, 0, 0, 0, 0, 0 };\n'
 '\t\t\tint nP3Map379 = 0, nP3MapKhac = 0;\n'
 '\t\t\tint aMoXe[3];\n'
 '\t\t\tint nMoXe = 0;\n'
 '\t\t\tfor (int i8 = 0; i8 < s_botCount; i8++)\n'
 '\t\t\t{\n'
 '\t\t\t\tconst int nTk8 = s_bots[i8].nTk;\n'
 '\t\t\t\tif (nTk8 < 0 || nTk8 > 5 || s_bots[i8].nPlayerIdx <= 0)\n'
 '\t\t\t\t\tcontinue;\n'
 '\t\t\t\tnPha[nTk8]++;\n'
 '\t\t\t\tif (nTk8 != 3)\n'
 '\t\t\t\t\tcontinue;\n'
 '\t\t\t\tconst int nn8 = Player[s_bots[i8].nPlayerIdx].m_nIndex;\n'
 '\t\t\t\tif (nn8 <= 0 || nn8 >= MAX_NPC)\n'
 '\t\t\t\t\tcontinue;\n'
 '\t\t\t\tconst int nSub8 = Npc[nn8].m_SubWorldIndex;\n'
 '\t\t\t\tif (nSub8 >= 0 && SubWorld[nSub8].m_SubWorldID == PB_TK_MAP)\n'
 '\t\t\t\t\tnP3Map379++;\n'
 '\t\t\t\telse\n'
 '\t\t\t\t\tnP3MapKhac++;\n'
 '\t\t\t\tif (nMoXe < 3)\n'
 '\t\t\t\t\taMoXe[nMoXe++] = i8;\n'
 '\t\t\t}\n'
 '\t\t\tif (nPha[1] + nPha[2] + nPha[3] + nPha[4] + nPha[5] > 0)\n'
 '\t\t\t{\n'
 '\t\t\t\tpb_Log("[TkCensus] pha1=%d pha2=%d pha3=%d(379:%d khac:%d) pha4=%d pha5=%d\\n",\n'
 '\t\t\t\t       nPha[1], nPha[2], nPha[3], nP3Map379, nP3MapKhac, nPha[4], nPha[5]);\n'
 '\t\t\t\tfor (int m8 = 0; m8 < nMoXe; m8++)\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tPB_Bot& b8 = s_bots[aMoXe[m8]];\n'
 '\t\t\t\t\tconst int p8 = b8.nPlayerIdx;\n'
 '\t\t\t\t\tconst int nn8 = Player[p8].m_nIndex;\n'
 '\t\t\t\t\tif (nn8 <= 0 || nn8 >= MAX_NPC)\n'
 '\t\t\t\t\t\tcontinue;\n'
 '\t\t\t\t\tconst int nSub8 = Npc[nn8].m_SubWorldIndex;\n'
 '\t\t\t\t\tint x8 = 0, y8 = 0;\n'
 '\t\t\t\t\tNpc[nn8].GetMpsPos(&x8, &y8);\n'
 '\t\t\t\t\tconst unsigned int now8 = (nSub8 >= 0) ? SubWorld[nSub8].m_dwCurrentTime : 0;\n'
 '\t\t\t\t\tpb_Log("[TkCensus]  pha3: %s map=%d o(%d,%d) doing=%d procAI=%d obs=%d"\n'
 '\t\t\t\t\t       " choRa=%d mua=%d\\n",\n'
 '\t\t\t\t\t       Player[p8].m_PlayerName,\n'
 '\t\t\t\t\t       (nSub8 >= 0) ? SubWorld[nSub8].m_SubWorldID : -1,\n'
 '\t\t\t\t\t       x8 / 32, y8 / 32, (int)Npc[nn8].m_Doing,\n'
 '\t\t\t\t\t       (int)(Npc[nn8].IsCanInput() ? 1 : 0),\n'
 '\t\t\t\t\t       (nSub8 >= 0) ? SubWorld[nSub8].CellObsSrv(x8, y8) : -9,\n'
 '\t\t\t\t\t       (int)(b8.nTkChoRa > now8 ? (b8.nTkChoRa - now8) / GAME_FPS : 0),\n'
 '\t\t\t\t\t       b8.nTkMuaXong);\n'
 '\t\t\t\t}\n'
 '\t\t\t}\n'
 '\t\t}\n'
 '\t}\n')

# ---- H2: tu cuu bot pha 3 dung tren o chan ----
ap("H2 cuu o chan trong trai",
 '\t\t// (21/08 - chu game: "khi chet vao lai hau doanh thi cho thoi gian 5 giay\n'
 '\t\t// roi di mua mau roi 5 giay sau moi di chuyen toi trap de goi ra ngoai")\n'
 '\t\t// Nhip nay dung cho CA luot dau (vua bao danh xong) lan moi lan chet hoi\n'
 '\t\t// sinh, cho dong nhat va nhin ra dang nguoi choi dinh than roi moi di.\n'
 '\t\tif (now < b.nTkChoRa)\n'
 '\t\t\treturn;\n',
 '\t\t// (21/08 - chu game: "khi chet vao lai hau doanh thi cho thoi gian 5 giay\n'
 '\t\t// roi di mua mau roi 5 giay sau moi di chuyen toi trap de goi ra ngoai")\n'
 '\t\t// Nhip nay dung cho CA luot dau (vua bao danh xong) lan moi lan chet hoi\n'
 '\t\t// sinh, cho dong nhat va nhin ra dang nguoi choi dinh than roi moi di.\n'
 '\t\tif (now < b.nTkChoRa)\n'
 '\t\t\treturn;\n'
 '\t\t// [TK-CUU3 28/08] (chu game: "bot dung yen trong trai khong ra") NewWorld cua\n'
 '\t\t// script bao danh tha KHONG kiem vat can -> mot so con dap tren O BI CHAN theo\n'
 '\t\t// luoi server: do_run ma GetDir=0 moi tick = dung yen vinh vien ([BotKet]\n'
 '\t\t// "bo cuoc: doing=3 procAI=1"). Phat hien la SetPos sang o dat gan nhat\n'
 '\t\t// (pb_ODat quanh cho dang dung, du phong quanh anchor trai minh).\n'
 '\t\tif (SubWorld[nSub].CoLuoiSrv())\n'
 '\t\t{\n'
 '\t\t\tint nCx3 = 0, nCy3 = 0;\n'
 '\t\t\tNpc[nNpcIdx].GetMpsPos(&nCx3, &nCy3);\n'
 '\t\t\tif (SubWorld[nSub].CellObsSrv(nCx3, nCy3) != 0)\n'
 '\t\t\t{\n'
 '\t\t\t\tint nVc3 = 0, nVy3 = 0;\n'
 '\t\t\t\tint nOk3 = pb_ODat(nSub, nCx3 / 32, nCy3 / 32, nLech, 8, &nVc3, &nVy3);\n'
 '\t\t\t\tif (!nOk3)\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tstatic const int aCHx[2] = { 1229, 1689 };\n'
 '\t\t\t\t\tstatic const int aCHy[2] = { 3561, 3074 };\n'
 '\t\t\t\t\tconst int iTc = pb_TkDaoTheTran(nSub);\n'
 '\t\t\t\t\tconst int hc  = (b.nTkPhe == 1) ? iTc : (1 - iTc);\n'
 '\t\t\t\t\tnOk3 = pb_ODat(nSub, aCHx[hc], aCHy[hc], nLech, 12, &nVc3, &nVy3);\n'
 '\t\t\t\t}\n'
 '\t\t\t\tif (nOk3)\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tpb_Log("[TkCuu3] %s dap tren o CHAN o(%d,%d) trong trai -> SetPos"\n'
 '\t\t\t\t\t       " o(%d,%d)\\n", Player[nIdx].m_PlayerName,\n'
 '\t\t\t\t\t       nCx3 / 32, nCy3 / 32, nVc3 / 32, nVy3 / 32);\n'
 '\t\t\t\t\tNpc[nNpcIdx].SetPos(nVc3, nVy3);\n'
 '\t\t\t\t\tb.walk.Reset();\n'
 '\t\t\t\t}\n'
 '\t\t\t}\n'
 '\t\t}\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
