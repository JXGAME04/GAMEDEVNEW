# -*- coding: utf-8 -*-
# [TKKET3 01/09] Chu game: "bot Tong Kim danh dong thi nhieu bot DUNG YEN khong danh
# - kiem tra ky lay log de fix".
# Do 3 tran 31/08 (bao danh 20:50 / 22:50 / 23:46, moi tran 500 con):
#   - 79 / 45 / 73 con bao danh xong KHONG BAO GIO qua cua trai ("RA TRAN" = 0 lan),
#     LUON thuoc phe dong TRAI DONG (1688,3072) - tran 1+3 la Tong, tran 2 la Kim;
#     phe dong trai TAY (1242,3549): 0 con ket o ca 3 tran.
#   - Census bat qua tang: dung nguyen MOT o hang chuc phut ngay VET TRAP cua ra
#     (NguyenHieu9 o(1666,3102) 8+ phut, doing=1 procAI=1 obs=0 choRa=0 mua=1),
#     va ca o diem dap (1242,3549)/(1688,3072) + canh Quan Y (1692-1699,306x).
#   - Giao 2 tran cung the tran chi 10/79 trung ten => KHONG ben theo con, la yeu
#     to DONG (dua tranh) moi tran.
#   - Da loai bang ma + log: A* fail lien tuc (22 dong [BotA*] ca toi), nhanh cho
#     cong (choRa luon =0), dong ho ca nhan (deadline theo m_nLoopRate tu can),
#     loi script (ScriptError.log = 0 dong trap), nRemain (>80s chi 10s dau).
#   - Con MOT buoc khong quan sat duoc: moi tick pha 3 ket thuc o dau (PB_WalkTo
#     tra gi / cong dong / ExecuteScript trap co SetPos duoc khong).
# Bo nay CHI DAT LOG - khong doi mot dong hanh vi. Nhan moi:
#   [TkKet3]       censu 10s/con: bot pha 3 dung nguyen 1 cho >60s (doing/mua/choRa
#                  /timer ca nhan/t1 cong)
#   [TkKet3-QY]    dung yen >60s ngay khau di den Quan Y: in nWq (10s/con)
#   [TkKet3-MUA]   PB_WalkTo toi Quan Y tra -1 -> bo qua mua + bo qua StopTimer
#                  (duong "mua=1 rom" - luon in, hiem)
#   [TkKet3-RA]    dung yen >60s o khau ra cua: in nW cua PB_WalkTo (10s/con)
#   [TkKet3-CONG]  dung yen >60s ma qua duoc walk nhung cong dong (t1>0)
#   [TkKet3-TRAP]  dung yen >60s: goi script trap, in GetRestTime TRUOC khi goi
#                  (dung gia tri script doc) + vi tri (15s/con)
#   [TkKet3-TRAP2] sau cu goi tren: script tra ve gi + co bi TU CHOI (khong doi cho)
# Chi KPlayerBot.cpp. AP SAU chuoi: ... -> goi_va_303_docthuan -> damageattrib_thua
# -> loc_noiluc -> noi_hoan -> TKKET3 (bo nay).
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

# ---- H1: them truong theo doi vao PB_Bot (server-only, khong dinh giao thuc) ----
ap("H1 truong PB_Bot",
 '\tunsigned int nTkRaTick;                   // moc goi lai trap ra trai\n',
 '\tunsigned int nTkRaTick;                   // moc goi lai trap ra trai\n'
 '\tunsigned int uKet3Tick;                   // [TKKET3 01/09] dung yen mot cho o pha 3 tu luc nao (CHI de log)\n'
 '\tint          nKet3X;                      // [TKKET3] neo toa do do dung-yen\n'
 '\tint          nKet3Y;\n'
 '\tunsigned int uKet3Log;                    // [TKKET3] tiet che dong [TkKet3] 10 giay/con\n'
 '\tunsigned int uKet3Trap;                   // [TKKET3] tiet che dong [TkKet3-TRAP] 15 giay/con\n')

# ---- H2: censu dung-yen dau pha 3 (truoc moi cua cho) ----
ap("H2 censu dung yen pha 3",
 '\t\t// (21/08 - chu game: "khi chet vao lai hau doanh thi cho thoi gian 5 giay\n'
 '\t\t// roi di mua mau roi 5 giay sau moi di chuyen toi trap de goi ra ngoai")\n',
 '\t\t// [TKKET3 01/09] (chu game: "nhieu bot TK dung yen khong danh") MO XE bot\n'
 '\t\t// pha 3 dung nguyen mot cho >60s: do 3 tran 31/08 co 45-79 con/tran (15-32%,\n'
 '\t\t// LUON o phe dong trai DONG 1688,3072) khong bao gio qua cua. Khong dung\n'
 '\t\t// nTkTick vi vung-an-toan hau doanh (9.38, :8387) lam tuoi no moi tick -\n'
 '\t\t// phai neo rieng theo toa do. CHI GHI LOG, khong doi hanh vi.\n'
 '\t\t{\n'
 '\t\t\tint nKq3 = 0, nKr3 = 0;\n'
 '\t\t\tNpc[nNpcIdx].GetMpsPos(&nKq3, &nKr3);\n'
 '\t\t\tint dKx3 = (nKq3 - b.nKet3X) / 32;  if (dKx3 < 0) dKx3 = -dKx3;\n'
 '\t\t\tint dKy3 = (nKr3 - b.nKet3Y) / 32;  if (dKy3 < 0) dKy3 = -dKy3;\n'
 '\t\t\tif (b.uKet3Tick == 0 || dKx3 >= 2 || dKy3 >= 2)\n'
 '\t\t\t{\n'
 '\t\t\t\tb.nKet3X = nKq3;  b.nKet3Y = nKr3;  b.uKet3Tick = now;\n'
 '\t\t\t}\n'
 '\t\t\telse if (now - b.uKet3Tick > (unsigned int)(GAME_FPS * 60)\n'
 '\t\t\t      && now - b.uKet3Log >= (unsigned int)(GAME_FPS * 10))\n'
 '\t\t\t{\n'
 '\t\t\t\tb.uKet3Log = now;\n'
 '\t\t\t\tKMission* pMk3 = pb_TkMission(NULL);\n'
 '\t\t\t\tpb_Log("[TkKet3] %s phe %d dung %us o(%d,%d) doing=%d mua=%d choRa=%d"\n'
 '\t\t\t\t       " timer=%u t1=%u\\n",\n'
 '\t\t\t\t       Player[nIdx].m_PlayerName, b.nTkPhe,\n'
 '\t\t\t\t       (now - b.uKet3Tick) / GAME_FPS, nKq3 / 32, nKr3 / 32,\n'
 '\t\t\t\t       (int)Npc[nNpcIdx].m_Doing, b.nTkMuaXong,\n'
 '\t\t\t\t       (int)(b.nTkChoRa > now ? (b.nTkChoRa - now) / GAME_FPS : 0),\n'
 '\t\t\t\t       (unsigned int)Player[nIdx].m_TimerTask.GetRestTime(),\n'
 '\t\t\t\t       pMk3 ? (unsigned int)pMk3->GetTimerRestTimer(1) : 0);\n'
 '\t\t\t}\n'
 '\t\t}\n'
 '\t\t// (21/08 - chu game: "khi chet vao lai hau doanh thi cho thoi gian 5 giay\n'
 '\t\t// roi di mua mau roi 5 giay sau moi di chuyen toi trap de goi ra ngoai")\n')

# ---- H3: khau Quan Y - in nWq khi dang ket + danh dau duong "mua=1 rom" ----
ap("H3 quan y nWq + mua rom",
 '\t\t\t\tconst int nWq = PB_WalkTo(nNpcIdx, nQtx, nQty, nSub, b.walk,\n'
 '\t\t\t\t                          PB_FAC_ARRIVE_MPS);\n'
 '\t\t\t\tif (nWq == 0)\n'
 '\t\t\t\t\treturn;                // dang di bo toi Quan Y cua PHE MINH\n'
 '\t\t\t\tif (nWq > 0)\n'
 '\t\t\t\t\tPlayer[nIdx].ExecuteScript((char*)"\\\\script\\\\global\\\\bot_tongkim.lua",\n'
 '\t\t\t\t\t                           (char*)"bot_tk_muamau", 0, false);\n',
 '\t\t\t\tconst int nWq = PB_WalkTo(nNpcIdx, nQtx, nQty, nSub, b.walk,\n'
 '\t\t\t\t                          PB_FAC_ARRIVE_MPS);\n'
 '\t\t\t\t// [TKKET3 01/09] dung yen >60s ngay khau di mua thuoc -> in nWq (CHI LOG)\n'
 '\t\t\t\tif (b.uKet3Tick != 0 && now - b.uKet3Tick > (unsigned int)(GAME_FPS * 60)\n'
 '\t\t\t\t && ((now + (unsigned int)nLech * 7u) % (unsigned int)(GAME_FPS * 10)) == 0)\n'
 '\t\t\t\t\tpb_Log("[TkKet3-QY] %s nWq=%d dich o(%d,%d)\\n",\n'
 '\t\t\t\t\t       Player[nIdx].m_PlayerName, nWq, nQtx / 32, nQty / 32);\n'
 '\t\t\t\tif (nWq == 0)\n'
 '\t\t\t\t\treturn;                // dang di bo toi Quan Y cua PHE MINH\n'
 '\t\t\t\tif (nWq > 0)\n'
 '\t\t\t\t\tPlayer[nIdx].ExecuteScript((char*)"\\\\script\\\\global\\\\bot_tongkim.lua",\n'
 '\t\t\t\t\t                           (char*)"bot_tk_muamau", 0, false);\n'
 '\t\t\t\telse\n'
 '\t\t\t\t\tpb_Log("[TkKet3-MUA] %s KHONG duong toi Quan Y (nWq=%d) -> bo qua mua"\n'
 '\t\t\t\t\t       " + bo qua StopTimer\\n", Player[nIdx].m_PlayerName, nWq);\n')

# ---- H4a: pb_TkRaTrai - co bKet3 + in ket qua PB_WalkTo ra cua ----
ap("H4a bKet3 + nW ra cua",
 '\tpb_LenNguaDuongXa(nIdx, nNpcIdx, b, now);\n'
 '\tconst int nW = PB_WalkTo(nNpcIdx, nOx, nOy, nSub, b.walk, PB_FAC_ARRIVE_MPS);\n'
 '\tif (nW == 0)\n'
 '\t\treturn 0;                      // dang di bo toi cua trai\n'
 '\tif (nW < 0)\n'
 '\t\treturn -1;                     // khong co duong\n',
 '\tpb_LenNguaDuongXa(nIdx, nNpcIdx, b, now);\n'
 '\t// [TKKET3 01/09] bot dung yen >60s (neo dat o dau pha 3): in ket qua tung\n'
 '\t// buoc cua duong ra cua de tim dung cho ket. CHI GHI LOG.\n'
 '\tconst int bKet3 = (b.uKet3Tick != 0\n'
 '\t                && now - b.uKet3Tick > (unsigned int)(GAME_FPS * 60));\n'
 '\tconst int nW = PB_WalkTo(nNpcIdx, nOx, nOy, nSub, b.walk, PB_FAC_ARRIVE_MPS);\n'
 '\tif (bKet3 && ((now + (unsigned int)nLech9 * 7u) % (unsigned int)(GAME_FPS * 10)) == 0)\n'
 '\t\tpb_Log("[TkKet3-RA] %s nW=%d o(%d,%d) dich o(%d,%d) k=%d\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, nW, bx / 32, by / 32, nOx / 32, nOy / 32, k);\n'
 '\tif (nW == 0)\n'
 '\t\treturn 0;                      // dang di bo toi cua trai\n'
 '\tif (nW < 0)\n'
 '\t\treturn -1;                     // khong co duong\n')

# ---- H4b: nhanh cong dong (t1 > 0) ----
ap("H4b cong dong",
 '\t\t\tb.nTkChoRa = now + (unsigned int)(GAME_FPS * (1 + ((nLech9 * 13 + 7) % 15)));\n'
 '\t\t\treturn 0;\n',
 '\t\t\tif (bKet3)\n'
 '\t\t\t\tpb_Log("[TkKet3-CONG] %s cong dong t1=%u -> doi tiep\\n",\n'
 '\t\t\t\t       Player[nIdx].m_PlayerName, (unsigned int)pM9->GetTimerRestTimer(1));\n'
 '\t\t\tb.nTkChoRa = now + (unsigned int)(GAME_FPS * (1 + ((nLech9 * 13 + 7) % 15)));\n'
 '\t\t\treturn 0;\n')

# ---- H4c: cu goi script trap - in GetRestTime truoc goi + ket qua sau goi ----
ap("H4c goi trap + tu choi",
 '\tint nTx9 = 0, nTy9 = 0;\n'
 '\tNpc[nNpcIdx].GetMpsPos(&nTx9, &nTy9);\n'
 '\tPlayer[nIdx].ExecuteScript(\n'
 '\t    (char*)((k == 0) ? "\\\\script\\\\maps\\\\tongkim\\\\trap\\\\tongratrai.lua"\n'
 '\t                     : "\\\\script\\\\maps\\\\tongkim\\\\trap\\\\kimratrai.lua"),\n'
 '\t    (char*)"main", nIdx, false);\n'
 '\tb.walk.Reset();\n'
 '\t{\n'
 '\t\tint nSx9 = 0, nSy9 = 0;\n'
 '\t\tNpc[nNpcIdx].GetMpsPos(&nSx9, &nSy9);\n'
 '\t\tif (nSx9 == nTx9 && nSy9 == nTy9)\n'
 '\t\t\treturn 0;                  // kich ban tu choi - dung cho, thu lai sau\n'
 '\t}\n'
 '\treturn 1;\n',
 '\tint nTx9 = 0, nTy9 = 0;\n'
 '\tNpc[nNpcIdx].GetMpsPos(&nTx9, &nTy9);\n'
 '\t// [TKKET3 01/09] dung yen >60s: ghi lai dung gia tri dong ho ma script trap\n'
 '\t// se doc (GetRestTime) + vi tri truoc/sau, de biet no TU CHOI hay SetPos hut.\n'
 '\tint bLogTrap9 = 0;\n'
 '\tif (bKet3 && now - b.uKet3Trap >= (unsigned int)(GAME_FPS * 15))\n'
 '\t{\n'
 '\t\tb.uKet3Trap = now;\n'
 '\t\tbLogTrap9 = 1;\n'
 '\t\tpb_Log("[TkKet3-TRAP] %s goi trap k=%d rest=%u truoc o(%d,%d)\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, k,\n'
 '\t\t       (unsigned int)Player[nIdx].m_TimerTask.GetRestTime(),\n'
 '\t\t       nTx9 / 32, nTy9 / 32);\n'
 '\t}\n'
 '\tBOOL bScr9 = Player[nIdx].ExecuteScript(\n'
 '\t    (char*)((k == 0) ? "\\\\script\\\\maps\\\\tongkim\\\\trap\\\\tongratrai.lua"\n'
 '\t                     : "\\\\script\\\\maps\\\\tongkim\\\\trap\\\\kimratrai.lua"),\n'
 '\t    (char*)"main", nIdx, false);\n'
 '\tb.walk.Reset();\n'
 '\t{\n'
 '\t\tint nSx9 = 0, nSy9 = 0;\n'
 '\t\tNpc[nNpcIdx].GetMpsPos(&nSx9, &nSy9);\n'
 '\t\tif (nSx9 == nTx9 && nSy9 == nTy9)\n'
 '\t\t{\n'
 '\t\t\tif (bLogTrap9)\n'
 '\t\t\t\tpb_Log("[TkKet3-TRAP2] %s script=%d TU CHOI - dung nguyen o(%d,%d)\\n",\n'
 '\t\t\t\t       Player[nIdx].m_PlayerName, (int)bScr9, nSx9 / 32, nSy9 / 32);\n'
 '\t\t\treturn 0;                  // kich ban tu choi - dung cho, thu lai sau\n'
 '\t\t}\n'
 '\t}\n'
 '\treturn 1;\n')

io.open(P, "w", encoding="latin-1", newline="").write(s)
sau = sum(1 for c in s if ord(c) > 127)
print("Xong: %d hunk moi. high-byte truoc=%d sau=%d (%s)"
      % (n, truoc, sau, "OK" if truoc == sau else "LECH!"))
