# -*- coding: utf-8 -*-
# [BOTNHOM-NGUOI c + BOTBANG c + TKKET4 + TKCHET 01/09 toi] Chu game (nguyen van, phien wauto-34):
#  * "toi muon nguoi choi moi party cho bot thi bot nao chua co party se tu vao party cua
#    nguoi choi (khong can bot nhan tin chat mat cho nguoi choi)"
#  * "khi bot vao party nguoi choi thi bot phai theo sau nguoi choi (doi truong party) neu bot
#    dang lam da tau thi khi xong nhiem vu tu dong roi nhom de ve tra nhiem vu"
#  * "khi nguoi choi phu ve thanh hoac thon thi se khong giai tan party - chi khi nguoi choi len
#    map luyen cong khac hoac la vao map su kien hoat dong moi tu giai tan party"
#  * "bot tu dong xin vao bang hoi cua nguoi choi - dieu kien: nguoi choi la BANG CHU nhan tin
#    chat mat cho bot voi tu khoa: vao bang / vo bang / tham gia bang hoi / vao bang hoat dong /
#    vao bang ko / vo bang k -> bot se ve thon Ba Lang Huyen tu toi NPC mon phai cua minh roi
#    XUAT SU xong, tu he thong bang hoi bot se xin vao dung bang hoi cua nguoi choi do"
#  * "tat ca phai lam theo thu tu tung buoc mot chu khong duoc bo buoc"
#  * "keo them log vua di xong tran tong kim de kiem tra tinh trang bot dung yen khong danh"
#  * "bot tham gia tong kim xong thi khong di luyen cong - lam nhiem vu da tau ma dung trong map
#    bao danh"
#
# DO TRAN 17:50-18:20 01/09 (CoreServer 3730dc63, bo log [TkKet3] dang song):
#  - 81 bot ket, 100% trai DONG, 6.160 dong [TkKet3-RA] deu nW=-1, chi 5 dich: (1665,3106)
#    (1666,3107) (1666,3109) (1667,3110) (1674,3108); KHONG mot dong [BotA*] nao tren map 379
#    => A* khong "loi", ma PB_WalkTo roi vao nhanh DUONG CUT im lang (FindPathServer tra 2).
#    Doc luoi Maps\379_srv.fp: hanh lang trap kimratrai (1661+i,3098+i) la KHE TUONG, cac o trap
#    i>=6 la vat can tren luoi; pb_ODat quet xoan oc (chi hoi "o trong + luoi khong chan") nen
#    chon o TRONG o BEN KIA TUONG -> khong bao gio toi. 152 dong [BotTrap] id=1975C4C8
#    (kimratrai) + 224 EA163CE9 (tongratrai) = bot CO dap dung trap ra trai nhung bi "mien".
#  - 13 bot sau tran: "da chet (mau 0, doing 1=do_stand)" moi giay x 991 lan/17 phut: task03
#    NewWorld het tran dung luc dang chet -> KNpc::ChangeWorld DoStand() (KNpc.cpp:10368) xoa
#    do_death/do_revive; KPlayer::Revive (KPlayer.cpp:6944) doi DUNG do_revive -> nuot mai =
#    "dung trong map bao danh".
#
# Chi KPlayerBot.cpp (20 hunk). AP SAU chuoi: ... -> bot_bang_nhom -> bot_bang_nhom_b ->
# tkket3_moxe -> (bo nay). Chay: python goi_va_bot_nhom_bang_tk_c.py [--thu]  (--thu: chi kiem neo)
import io, sys
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerBot.cpp"
THU = ("--thu" in sys.argv)

s = io.open(P, "r", encoding="latin-1", newline="").read()
truoc = sum(1 for c in s if ord(c) > 127)
crlf = "\r\n" in s
n = 0
loi = 0

def ap(ten, cu, moi):
    global s, n, loi
    if crlf:
        cu = cu.replace("\n", "\r\n"); moi = moi.replace("\n", "\r\n")
    if moi in s:
        print("  [=] %s da ap tu truoc" % ten); return
    c = s.count(cu)
    if c != 1:
        print("LOI: neo %s khop %d cho (can 1)" % (ten, c)); loi += 1
        if not THU:
            sys.exit(1)
        return
    if THU:
        print("  [ok] %s (neo khop 1)" % ten); return
    s = s.replace(cu, moi); n += 1
    print("  [+] %s" % ten)

# ============================================================ H1 truong PB_Bot
ap("H1a uNhomNguoiLac",
 '\tint          nNhomNguoiSub;               // [b] subworld cua nguoi moi LUC vao nhom (-1 = khong)\n',
 '\tint          nNhomNguoiSub;               // [b] subworld cua nguoi moi LUC vao nhom (-1 = khong)\n'
 '\tunsigned int uNhomNguoiLac;               // [c] moc BOT lac khoi map nhom nguoi that (0 = khong)\n')

ap("H1b nTkRaXoay",
 '\tunsigned int uKet3Tick;                   // [TKKET3 01/09] dung yen mot cho o pha 3 tu luc nao (CHI de log)\n',
 '\tint          nTkRaXoay;                   // [TKKET4 01/09] xoay o trap dich khi PB_WalkTo thua (0 = theo chi so bot)\n'
 '\tunsigned int uKet3Tick;                   // [TKKET3 01/09] dung yen mot cho o pha 3 tu luc nao (CHI de log)\n')

# ============================================================ H2 khoi tao
ap("H2 khoi tao truong moi",
 '\t\tb.nPmCamToi = 0;  b.nPmLapHash = 0;\n'
 '\t\tb.nNhomNguoiIdx = 0;  b.dwNhomNguoiID = 0;  b.nNhomNguoiSub = -1;\n',
 '\t\tb.nPmCamToi = 0;  b.nPmLapHash = 0;\n'
 '\t\tb.nNhomNguoiIdx = 0;  b.dwNhomNguoiID = 0;  b.nNhomNguoiSub = -1;\n'
 '\t\tb.uNhomNguoiLac = 0;  b.nTkRaXoay = 0;\n')

# ============================================================ H3 khai bao truoc
ap("H3 khai bao truoc helper nhom nguoi that",
 '\tpb_Log("[BotNhom] %s %s (%s)\\n", Player[nIdx].m_PlayerName,\n'
 '\t       bTruong ? "GIAI TAN nhom" : "roi nhom", szLyDo);\n'
 '}\n',
 '\tpb_Log("[BotNhom] %s %s (%s)\\n", Player[nIdx].m_PlayerName,\n'
 '\t       bTruong ? "GIAI TAN nhom" : "roi nhom", szLyDo);\n'
 '}\n'
 '\n'
 '// [BOTNHOM-NGUOI c 01/09] khai bao truoc - than nam sau pb_GuiChatMat (tim "pb_RoiNhomNguoi(int")\n'
 '// va sau pb_LaThanhThi (tim "pb_LaThanhThon(int").\n'
 'static int  pb_NhomNguoiSong(int nIdx, const PB_Bot& b);\n'
 'static void pb_RoiNhomNguoi(int nIdx, PB_Bot& b, const char* szPm, const char* szLyDo);\n'
 'static int  pb_LaThanhThon(int nMapId);\n')

# ============================================================ H4 pb_LaThanhThon
ap("H4 pb_LaThanhThon",
 'static int pb_LaThanhThi(int nMapId)\n'
 '{\n'
 '\tfor (int i = 0; i < 10; i++)\n'
 '\t\tif (s_dtNpc[i].nMap == nMapId)\n'
 '\t\t\treturn 1;\n'
 '\treturn 0;\n'
 '}\n',
 'static int pb_LaThanhThi(int nMapId)\n'
 '{\n'
 '\tfor (int i = 0; i < 10; i++)\n'
 '\t\tif (s_dtNpc[i].nMap == nMapId)\n'
 '\t\t\treturn 1;\n'
 '\treturn 0;\n'
 '}\n'
 '\n'
 '// [BOTNHOM-NGUOI c 01/09] thanh thi HOAC thon/tran - noi nguoi choi "phu ve" (Tho Dia Phu /\n'
 '// Than Hanh Phu) chu KHONG phai map luyen cong hay map hoat dong: 10 thanh co NPC Da Tau +\n'
 '// cac thon trong THON_TT_MP_ARRAY (item\\ib\\shenxingfu.lua:27-32: Giang Tan Thon 20, Ba Lang\n'
 '// Huyen 53, Vinh Lac Tran 99, Chu Tien Tran 100, Dao Huong Thon 101, Long Mon Tran 121).\n'
 'static int pb_LaThanhThon(int nMapId)\n'
 '{\n'
 '\tstatic const int aThon[] = { 20, 53, 99, 100, 101, 121 };\n'
 '\tif (pb_LaThanhThi(nMapId))\n'
 '\t\treturn 1;\n'
 '\tfor (int i = 0; i < (int)(sizeof(aThon) / sizeof(aThon[0])); i++)\n'
 '\t\tif (aThon[i] == nMapId)\n'
 '\t\t\treturn 1;\n'
 '\treturn 0;\n'
 '}\n')

# ============================================================ H5 than helper
ap("H5 than pb_NhomNguoiSong / pb_RoiNhomNguoi",
 'int PB_MoiVaoNhom(int nMoi, int nBot)\n'
 '{\n',
 '// [BOTNHOM-NGUOI c] nguoi that da moi con online VA con CHUNG nhom voi bot?\n'
 'static int pb_NhomNguoiSong(int nIdx, const PB_Bot& b)\n'
 '{\n'
 '\tconst int nM = b.nNhomNguoiIdx;\n'
 '\tif (nM <= 0 || nM >= MAX_PLAYER || Player[nM].m_dwID != b.dwNhomNguoiID || Player[nM].m_nIndex <= 0)\n'
 '\t\treturn 0;\n'
 '\tif (!pb_TrongNhom(nIdx) || !Player[nM].m_cTeam.m_nFlag\n'
 '\t || Player[nM].m_cTeam.m_nID != Player[nIdx].m_cTeam.m_nID)\n'
 '\t\treturn 0;\n'
 '\treturn 1;\n'
 '}\n'
 '\n'
 '// [BOTNHOM-NGUOI c] roi nhom: neu la nhom NGUOI THAT thi NHAN MAT bao truoc (nguoi do con online)\n'
 '// roi moi roi; xoa dau vet de moi nhanh khac thoi coi bot la "dang o nhom nguoi that". Nhom toan\n'
 '// bot (nNhomNguoiIdx = 0) thi chi roi nhu pb_RoiNhom. Dung MOT ham cho moi nhanh tu doi map cua\n'
 '// bot (Xa Phu / Da Tau / Tong Kim / ve thanh / xin bang) de khong nhanh nao roi nhom im lang.\n'
 'static void pb_RoiNhomNguoi(int nIdx, PB_Bot& b, const char* szPm, const char* szLyDo)\n'
 '{\n'
 '\tconst int nM = b.nNhomNguoiIdx;\n'
 '\tif (szPm && szPm[0] && nM > 0 && nM < MAX_PLAYER\n'
 '\t && Player[nM].m_dwID == b.dwNhomNguoiID && Player[nM].m_nIndex > 0 && pb_TrongNhom(nIdx))\n'
 '\t\tpb_GuiChatMat(nM, nIdx, szPm, (int)strlen(szPm));\n'
 '\tif (pb_TrongNhom(nIdx))\n'
 '\t\tpb_RoiNhom(nIdx, szLyDo);\n'
 '\tb.nNhomNguoiIdx = 0;  b.dwNhomNguoiID = 0;  b.nNhomNguoiSub = -1;  b.uNhomNguoiLac = 0;\n'
 '}\n'
 '\n'
 'int PB_MoiVaoNhom(int nMoi, int nBot)\n'
 '{\n')

# ============================================================ H6 PB_MoiVaoNhom
ap("H6a dieu kien tu choi (Da Tau dang FARM van vao)",
 '\tif (pB->nTk || pB->nBanSap || pB->nBangPha || pB->nDtPha != DTB_NGHI\n'
 '\t || Npc[nNpcB].m_Doing == do_death || Npc[nNpcB].m_Doing == do_revive)\n'
 '\t{\n'
 '\t\tif (uNowTc - s_uTcLog >= 3000)\n'
 '\t\t{\n'
 '\t\t\ts_uTcLog = uNowTc;\n'
 '\t\t\tpb_Log("[BotNhomNguoi] %s tu choi loi moi cua %s (TK=%d sap=%d bang=%d DaTau=%d chet=%d)\\n",\n'
 '\t\t\t       Player[nBot].m_PlayerName, Player[nMoi].m_PlayerName, pB->nTk, pB->nBanSap,\n'
 '\t\t\t       pB->nBangPha, (int)(pB->nDtPha != DTB_NGHI),\n',
 '\t// [c 01/09] Da Tau: chi tu choi khi dang DI (toi NPC / doi thoai / loc / Xa Phu / ve tra /\n'
 '\t// thuong); dang FARM (loai 4 tren map nhiem vu, loai 5/6 vua luyen vua lam) van vao nhom -\n'
 '\t// xong nhiem vu se tu roi nhom di tra (chu game: "bot dang lam da tau thi khi xong nhiem vu\n'
 '\t// tu dong roi nhom de ve tra nhiem vu").\n'
 '\tconst int bDtDangDi = (pB->nDtPha != DTB_NGHI && pB->nDtPha != DTB_FARM_NV\n'
 '\t                    && pB->nDtPha != DTB_FARM_BAI) ? 1 : 0;\n'
 '\tif (pB->nTk || pB->nBanSap || pB->nBangPha || bDtDangDi\n'
 '\t || Npc[nNpcB].m_Doing == do_death || Npc[nNpcB].m_Doing == do_revive)\n'
 '\t{\n'
 '\t\tif (uNowTc - s_uTcLog >= 3000)\n'
 '\t\t{\n'
 '\t\t\ts_uTcLog = uNowTc;\n'
 '\t\t\tpb_Log("[BotNhomNguoi] %s tu choi loi moi cua %s (TK=%d sap=%d bang=%d DaTau=%d chet=%d)\\n",\n'
 '\t\t\t       Player[nBot].m_PlayerName, Player[nMoi].m_PlayerName, pB->nTk, pB->nBanSap,\n'
 '\t\t\t       pB->nBangPha, bDtDangDi,\n')

ap("H6b co to doi mac dinh",
 '\t// "bam dong y" ho: ghi vao danh sach moi roi GetInviteReply(.., 1) (KPlayerTeam.cpp:414)\n'
 '\tif (!Player[nBot].m_cTeam.GetCanTeamFlag())\n'
 '\t\treturn 0;                          // [b] map cam to doi (SetCreateTeam(0)) -> de duong cu bao\n',
 '\t// "bam dong y" ho: ghi vao danh sach moi roi GetInviteReply(.., 1) (KPlayerTeam.cpp:414)\n'
 '\t// [c] co to doi cua bot mac dinh = 0 (Player[] la mang tinh, server khong co ctor KPlayerTeam;\n'
 '\t// chi pb_GhepNhom moi bat) -> bot chua tung ghep nhom bot bi "khong the moi" oan. Map cam to\n'
 '\t// doi that (CreateTeam_OFF cua newworldscript_default.lua) la map lien dau/hoat dong - bot o\n'
 '\t// do da bi chan boi nTk/nBanSap o tren, nen o day cu bat cho bot.\n'
 '\tif (!Player[nBot].m_cTeam.GetCanTeamFlag())\n'
 '\t\tPlayer[nBot].m_cTeam.SetCanTeamFlag(nBot, TRUE);\n')

ap("H6c vao nhom xong: coi map nhom la bai",
 '\t{\n'
 '\t\tconst int nNpcM = Player[nMoi].m_nIndex;\n'
 '\t\tpB->nNhomNguoiSub = (nNpcM > 0 && nNpcM < MAX_NPC) ? Npc[nNpcM].m_SubWorldIndex : -1;\n'
 '\t}\n',
 '\t{\n'
 '\t\tconst int nNpcM = Player[nMoi].m_nIndex;\n'
 '\t\tpB->nNhomNguoiSub = (nNpcM > 0 && nNpcM < MAX_NPC) ? Npc[nNpcM].m_SubWorldIndex : -1;\n'
 '\t}\n'
 '\tpB->uNhomNguoiLac = 0;\n'
 '\t// [c] (chu game: "bot phai theo sau nguoi choi") coi map dang dung la BAI cua bot neu no co\n'
 '\t// trong bang bai: pb_RaBai khong keo di map khac, va khi lac (hoi sinh o thanh) thi duong Xa\n'
 '\t// Phu dua LAI dung map nay. Map khong co trong bang (map nhiem vu...) thi giu nguyen - lac qua\n'
 '\t// 5 phut se tu roi nhom (pb_DriveBot).\n'
 '\tif (pB->nNhomNguoiSub >= 0)\n'
 '\t{\n'
 '\t\tconst int nMapM = SubWorld[pB->nNhomNguoiSub].m_SubWorldID;\n'
 '\t\tfor (int i = 0; i < PB_SO_BAI; i++)\n'
 '\t\t\tif (s_bai[i].nMapId == nMapM && !s_baiHong[i])\n'
 '\t\t\t{\n'
 '\t\t\t\tpB->nBaiIdx   = i;\n'
 '\t\t\t\tpB->nBaiLevel = pb_MocCua(Npc[nNpcB].m_Level);\n'
 '\t\t\t\tbreak;\n'
 '\t\t\t}\n'
 '\t}\n')

# ============================================================ H7 canh nhom moi nhip
ap("H7 canh doi map: thanh/thon KHONG giai tan + bot lac 5 phut",
 '\t\telse\n'
 '\t\t{\n'
 '\t\t\tconst int nNpcM = Player[nM].m_nIndex;\n'
 '\t\t\t// [b] so voi subworld cua NGUOI MOI luc vao nhom - chinh bot hoi sinh/di map khac\n'
 '\t\t\t// khong tinh la "nguoi choi doi map"\n'
 '\t\t\tif (nNpcM > 0 && nNpcM < MAX_NPC && b.nNhomNguoiSub >= 0\n'
 '\t\t\t && Npc[nNpcM].m_SubWorldIndex != b.nNhomNguoiSub)\n'
 '\t\t\t{\n'
 '\t\t\t\tpb_RoiNhom(nIdx, "nguoi choi da doi map");\n'
 '\t\t\t\tconst char* szBye = "Ban doi map roi, minh roi nhom nhe.";\n'
 '\t\t\t\tpb_GuiChatMat(nM, nIdx, szBye, (int)strlen(szBye));\n'
 '\t\t\t\tbXoa = 1;\n'
 '\t\t\t}\n'
 '\t\t}\n',
 '\t\telse\n'
 '\t\t{\n'
 '\t\t\tconst int nNpcM = Player[nM].m_nIndex;\n'
 '\t\t\tconst int nSubM = (nNpcM > 0 && nNpcM < MAX_NPC) ? Npc[nNpcM].m_SubWorldIndex : -1;\n'
 '\t\t\t// [b] so voi subworld cua NGUOI MOI luc vao nhom - chinh bot hoi sinh/di map khac\n'
 '\t\t\t// khong tinh la "nguoi choi doi map"\n'
 '\t\t\t// [c 01/09] (chu game) nguoi choi PHU VE THANH/THON thi KHONG giai tan; chi khi ho\n'
 '\t\t\t// sang MAP LUYEN CONG KHAC hoac MAP HOAT DONG moi tu roi nhom. Ve thanh roi quay lai\n'
 '\t\t\t// dung map cu thi bot lai bam theo nhu thuong.\n'
 '\t\t\tif (nSubM >= 0 && b.nNhomNguoiSub >= 0 && nSubM != b.nNhomNguoiSub\n'
 '\t\t\t && !pb_LaThanhThon(SubWorld[nSubM].m_SubWorldID))\n'
 '\t\t\t{\n'
 '\t\t\t\tpb_Log("[BotNhomNguoi] %s: nguoi choi %s sang map %d (khong phai thanh/thon) -> roi nhom\\n",\n'
 '\t\t\t\t       Player[nIdx].m_PlayerName, Player[nM].m_PlayerName, SubWorld[nSubM].m_SubWorldID);\n'
 '\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "Ban di map khac roi, minh roi nhom nhe.", "nguoi choi da sang map khac");\n'
 '\t\t\t}\n'
 '\t\t\t// [c] chinh BOT lac khoi map nhom (hoi sinh o thanh, doi bai...) qua 5 phut ma nguoi choi\n'
 '\t\t\t// van o map cu -> thoi, roi nhom cho ho khoi vuong (duong Xa Phu ve lai bai co the mat\n'
 '\t\t\t// vai phut: so le 60s + di bo trong thanh toi 300s, nen khong cat som hon).\n'
 '\t\t\telse if (nSub != b.nNhomNguoiSub && nSubM == b.nNhomNguoiSub && !b.nTk && !b.nBangPha)\n'
 '\t\t\t{\n'
 '\t\t\t\tif (b.uNhomNguoiLac == 0)\n'
 '\t\t\t\t\tb.uNhomNguoiLac = nowAll;\n'
 '\t\t\t\telse if (nowAll - b.uNhomNguoiLac > (unsigned int)(GAME_FPS * 300))\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tpb_Log("[BotNhomNguoi] %s lac khoi map nhom qua 5 phut (bot map %d, nguoi choi map %d) -> roi nhom\\n",\n'
 '\t\t\t\t\t       Player[nIdx].m_PlayerName, SubWorld[nSub].m_SubWorldID, SubWorld[nSubM].m_SubWorldID);\n'
 '\t\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "Minh lac mat ban roi, minh roi nhom nhe.", "bot lac khoi map nhom qua 5 phut");\n'
 '\t\t\t\t}\n'
 '\t\t\t}\n'
 '\t\t\telse\n'
 '\t\t\t\tb.uNhomNguoiLac = 0;\n'
 '\t\t}\n')

# ============================================================ H8 bam theo doi truong NGUOI THAT
ap("H8 bam theo doi truong la nguoi that",
 '\t\tif (pb_TrongNhom(nIdx) && pb_DoiTruongCua(nIdx) != nIdx && !b.nDaTauChon\n'
 '\t\t && !bChuaRaBai)\n'
 '\t\t{\n'
 '\t\t\tconst int nCap    = pb_DoiTruongCua(nIdx);\n'
 '\t\t\tconst int nCapNpc = (nCap > 0) ? Player[nCap].m_nIndex : 0;\n'
 '\t\t\tif (nCap > 0 && PB_IsBot(nCap) && nCapNpc > 0 && nCapNpc < MAX_NPC\n'
 '\t\t\t && Npc[nCapNpc].m_SubWorldIndex == nSub\n'
 '\t\t\t && Npc[nCapNpc].m_Doing != do_death)\n'
 '\t\t\t{\n',
 '\t\t// [BOTNHOM-NGUOI c 01/09] (chu game) "khi bot vao party nguoi choi thi bot phai theo sau\n'
 '\t\t// nguoi choi (doi truong party)": doi truong la NGUOI THAT cung duoc bam theo y nhu doi\n'
 '\t\t// truong bot; khong can dung bai, Da Tau dang FARM van theo (dang DI lam Da Tau thi de\n'
 '\t\t// pb_DaTau lo - no tu roi nhom co bao). Doi truong o map khac (phu ve thanh) thi bot cu\n'
 '\t\t// luyen tai cho doi ho ve.\n'
 '\t\tconst int bTheoNguoi = (b.nNhomNguoiIdx > 0 && pb_NhomNguoiSong(nIdx, b)\n'
 '\t\t                     && (b.nDtPha == DTB_NGHI || b.nDtPha == DTB_FARM_NV\n'
 '\t\t                      || b.nDtPha == DTB_FARM_BAI)) ? 1 : 0;\n'
 '\t\tif (pb_TrongNhom(nIdx) && pb_DoiTruongCua(nIdx) != nIdx\n'
 '\t\t && (bTheoNguoi || (!b.nDaTauChon && !bChuaRaBai)))\n'
 '\t\t{\n'
 '\t\t\tconst int nCap    = pb_DoiTruongCua(nIdx);\n'
 '\t\t\tconst int nCapNpc = (nCap > 0) ? Player[nCap].m_nIndex : 0;\n'
 '\t\t\tif (nCap > 0 && (PB_IsBot(nCap) || bTheoNguoi) && nCapNpc > 0 && nCapNpc < MAX_NPC\n'
 '\t\t\t && Npc[nCapNpc].m_SubWorldIndex == nSub\n'
 '\t\t\t && Npc[nCapNpc].m_Doing != do_death)\n'
 '\t\t\t{\n')

# ============================================================ H9 pb_RaBai
ap("H9a pb_RaBai: dang o map nhom nguoi that = dung bai",
 'static int pb_RaBai(int nIdx, int nNpcIdx, int nSub, PB_Bot& b, int nLech)\n'
 '{\n'
 '\tconst unsigned int now = SubWorld[nSub].m_dwCurrentTime;\n'
 '\tconst int nLevel = Npc[nNpcIdx].m_Level;\n',
 'static int pb_RaBai(int nIdx, int nNpcIdx, int nSub, PB_Bot& b, int nLech)\n'
 '{\n'
 '\tconst unsigned int now = SubWorld[nSub].m_dwCurrentTime;\n'
 '\tconst int nLevel = Npc[nNpcIdx].m_Level;\n'
 '\n'
 '\t// [BOTNHOM-NGUOI c 01/09] dang o nhom NGUOI THAT va dang dung DUNG map cua nhom -> coi nhu\n'
 '\t// dung bai: khong chon lai bai, khong doi map (chu game: "bot phai theo sau nguoi choi";\n'
 '\t// nguoi choi phu ve thanh thi bot van o lai map nay cho). Lac sang map khac thi cac nhanh\n'
 '\t// duoi van dua bot ve bai (= map nhom, dat luc vao nhom) ma KHONG roi nhom.\n'
 '\tif (b.nNhomNguoiIdx > 0 && b.nNhomNguoiSub == nSub && pb_NhomNguoiSong(nIdx, b))\n'
 '\t\treturn 1;\n')

ap("H9b pb_RaBai len xe khong roi nhom nguoi that",
 '\t\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t\t\tpb_RoiNhom(nIdx, "len xe di bai luyen");\n',
 '\t\t\tif (pb_TrongNhom(nIdx) && !b.nNhomNguoiIdx)   // [c] nhom nguoi that: dang ve LAI map nhom, giu nhom\n'
 '\t\t\t\tpb_RoiNhom(nIdx, "len xe di bai luyen");\n')

ap("H9c pb_RaBai phu ve thanh khong roi nhom nguoi that",
 '\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t\tpb_RoiNhom(nIdx, "phu ve thanh doi map luyen");\n',
 '\t\tif (pb_TrongNhom(nIdx) && !b.nNhomNguoiIdx)   // [c] nhom nguoi that: giu nhom, dang tren duong ve lai\n'
 '\t\t\tpb_RoiNhom(nIdx, "phu ve thanh doi map luyen");\n')

ap("H9d pb_RaBai doi thanh khong roi nhom nguoi that",
 '\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t\tpb_RoiNhom(nIdx, "doi thanh tim Xa Phu");\n',
 '\t\tif (pb_TrongNhom(nIdx) && !b.nNhomNguoiIdx)   // [c] nhom nguoi that: giu nhom\n'
 '\t\t\tpb_RoiNhom(nIdx, "doi thanh tim Xa Phu");\n')

# ============================================================ H10 Da Tau
ap("H10a Da Tau toi NPC: roi nhom co bao",
 '\t\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t\t\tpb_RoiNhom(nIdx, "di lam nhiem vu Da Tau");\n'
 '\t\t\tif (!pb_DtVeThanh(nIdx, nNpcIdx, b, nha, nLech))\n'
 '\t\t\t{\n'
 '\t\t\t\tpb_Log("[BotDT] %s KHONG toi duoc thanh %d - nghi 10 phut\\n",\n',
 '\t\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "Minh di lam nhiem vu Da Tau day, ban choi vui nhe.", "di lam nhiem vu Da Tau");\n'
 '\t\t\tif (!pb_DtVeThanh(nIdx, nNpcIdx, b, nha, nLech))\n'
 '\t\t\t{\n'
 '\t\t\t\tpb_Log("[BotDT] %s KHONG toi duoc thanh %d - nghi 10 phut\\n",\n')

ap("H10b Da Tau toi Xa Phu: roi nhom co bao",
 '\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t\tpb_RoiNhom(nIdx, "di lam nhiem vu Da Tau");\n'
 '\t\tif (SubWorld[nSub].m_SubWorldID != nha.nMap)\n',
 '\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t\tpb_RoiNhomNguoi(nIdx, b, "Minh di lam nhiem vu Da Tau day, ban choi vui nhe.", "di lam nhiem vu Da Tau");\n'
 '\t\tif (SubWorld[nSub].m_SubWorldID != nha.nMap)\n')

ap("H10c Da Tau ve tra (khac map): roi nhom co bao",
 '\t\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t\t\tpb_RoiNhom(nIdx, "ve tra nhiem vu Da Tau");\n',
 '\t\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "Minh xong nhiem vu Da Tau roi, roi nhom di tra nhiem vu nhe.", "ve tra nhiem vu Da Tau");\n')

ap("H10d Da Tau du cuon -> roi nhom nguoi that ngay",
 '\t\t\tpb_Log("[BotDT] %s DU %d/%d cuon -> ve NPC tra nhiem vu\\n",\n'
 '\t\t\t       Player[nIdx].m_PlayerName, nCo, nCan);\n'
 '\t\t\tb.nDtPha    = DTB_VE_TRA;\n',
 '\t\t\tpb_Log("[BotDT] %s DU %d/%d cuon -> ve NPC tra nhiem vu\\n",\n'
 '\t\t\t       Player[nIdx].m_PlayerName, nCo, nCan);\n'
 '\t\t\t// [c 01/09] (chu game) "xong nhiem vu tu dong roi nhom de ve tra nhiem vu"\n'
 '\t\t\tif (b.nNhomNguoiIdx)\n'
 '\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "Minh xong nhiem vu Da Tau roi, roi nhom di tra nhiem vu nhe.", "xong nhiem vu Da Tau - di tra");\n'
 '\t\t\tb.nDtPha    = DTB_VE_TRA;\n')

# ============================================================ H11 boc Da Tau / ban sap tranh bot dang o nhom nguoi that
ap("H11a PB_SetDaTau khong boc bot dang o nhom nguoi that",
 '\t\tif (s_bots[i].nTk)\n'
 '\t\t\tcontinue;\n'
 '\t\tif (!s_bots[i].nDaTauChon)\n'
 '\t\t\taId[nSong++] = i;          // ung vien co the them\n',
 '\t\tif (s_bots[i].nTk)\n'
 '\t\t\tcontinue;\n'
 '\t\tif (s_bots[i].nNhomNguoiIdx)\n'
 '\t\t\tcontinue;                  // [c] dang o nhom nguoi that - khong boc di lam Da Tau moi\n'
 '\t\tif (!s_bots[i].nDaTauChon)\n'
 '\t\t\taId[nSong++] = i;          // ung vien co the them\n')

ap("H11b PB_SetBanSap khong boc bot dang o nhom nguoi that",
 '\t\t\t                           // bot dang danh tran khong bi bat ban sap\n'
 '\t\taId[nSong++] = i;\n'
 '\t}\n',
 '\t\t\t                           // bot dang danh tran khong bi bat ban sap\n'
 '\t\tif (s_bots[i].nNhomNguoiIdx)\n'
 '\t\t\tcontinue;                  // [c] dang o nhom nguoi that - khong keo ra thanh ban sap\n'
 '\t\taId[nSong++] = i;\n'
 '\t}\n')

# ============================================================ H12 ve thanh (GM)
ap("H12 pb_VeThanh roi nhom co bao",
 '\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t\tpb_RoiNhom(nIdx, "ve thanh thi/thon");\n',
 '\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t\tpb_RoiNhomNguoi(nIdx, b, "Minh phai ve thanh, roi nhom nhe.", "ve thanh thi/thon");\n')

# ============================================================ H13 Tong Kim pha 2
ap("H13 TK pha 2 roi nhom co bao",
 '\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t{\n'
 '\t\t\tpb_RoiNhom(nIdx, "vao Tong Kim");\n'
 '\t\t\treturn;                        // nhip sau moi di, cho lenh roi nhom xong\n'
 '\t\t}\n',
 '\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t{\n'
 '\t\t\tpb_RoiNhomNguoi(nIdx, b, "Minh di Tong Kim day, roi nhom nhe.", "vao Tong Kim");\n'
 '\t\t\treturn;                        // nhip sau moi di, cho lenh roi nhom xong\n'
 '\t\t}\n')

# ============================================================ H14 xin vao bang: thu tu tung buoc + XUAT SU
ap("H14a define PB_TASK_XUATSU",
 '// [BOTBANG] may trang thai xin vao bang. Tra 1 = dang ban (caller return), 0 = xong/huy.\n'
 'static int pb_XinVaoBang(',
 '// [BOTBANG c 01/09] "da xuat su" = TASK_DUNGCHUNG2 (header\\taskid.lua:26 = 4134, o [9] "xuat su");\n'
 '// factionhead.lua:32 xuatsu(): SetCamp(4)+SetCurCamp(4)+LeaveTeam()+SetTask(TASK_DUNGCHUNG2,1);\n'
 '// menu "Xuat su xuong nui/xuatsu" chi hien khi cap >= 60 va task = 0 (thieulam.lua:18-20).\n'
 '#define PB_TASK_XUATSU   4134\n'
 '#define PB_CAP_XUATSU    60\n'
 '\n'
 '// [BOTBANG] may trang thai xin vao bang. Tra 1 = dang ban (caller return), 0 = xong/huy.\n'
 'static int pb_XinVaoBang(')

ap("H14b buoc 1: roi to doi truoc moi buoc khac",
 '\t// NPC mon phai CUA MINH; chua ro phai (blob mau) thi lay Thieu Lam - ca 10 NPC deu o\n'
 '\t// map 53 canh nhau nen "ve NPC mon phai" van dung nghia.\n'
 '\tint nFac = b.nFaction;\n',
 '\t// [c 01/09] BUOC 1 (chu game: "lam theo thu tu tung buoc mot"): roi to doi - bao nguoi choi neu\n'
 '\t// la nhom nguoi that - roi moi ve Ba Lang Huyen / di bo toi NPC / xuat su / nop don.\n'
 '\tif (pb_TrongNhom(nIdx))\n'
 '\t\tpb_RoiNhomNguoi(nIdx, b, "Minh roi nhom di xin vao bang, xong viec quay lai nhe.", "ve NPC mon phai xin vao bang");\n'
 '\t// NPC mon phai CUA MINH; chua ro phai (blob mau) thi lay Thieu Lam - ca 10 NPC deu o\n'
 '\t// map 53 canh nhau nen "ve NPC mon phai" van dung nghia.\n'
 '\tint nFac = b.nFaction;\n')

ap("H14c bo khoi roi nhom cu trong nhanh khac map",
 '\t\tb.nBangDoiMapTick = now;\n'
 '\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t{\n'
 '\t\t\tif (b.nNhomNguoiIdx > 0 && b.nNhomNguoiIdx < MAX_PLAYER\n'
 '\t\t\t && Player[b.nNhomNguoiIdx].m_dwID == b.dwNhomNguoiID && Player[b.nNhomNguoiIdx].m_nIndex > 0)\n'
 '\t\t\t{\n'
 '\t\t\t\tconst char* szRoi = "Minh roi nhom di xin vao bang, xong viec quay lai nhe.";\n'
 '\t\t\t\tpb_GuiChatMat(b.nNhomNguoiIdx, nIdx, szRoi, (int)strlen(szRoi));\n'
 '\t\t\t}\n'
 '\t\t\tpb_RoiNhom(nIdx, "ve NPC mon phai xin vao bang");\n'
 '\t\t}\n'
 '\t\tb.nNhomNguoiIdx = 0;  b.dwNhomNguoiID = 0;  b.nNhomNguoiSub = -1;\n'
 '\t\tint vx = 0, vy = 0;\n',
 '\t\tb.nBangDoiMapTick = now;\n'
 '\t\tint vx = 0, vy = 0;\n')

ap("H14d buoc XUAT SU tai NPC mon phai roi moi nop don",
 '\t// ---- TOI NOI: nop don xin vao DUNG bang cua nguoi ru ----\n'
 '\tNpc[nNpcIdx].SendCommand(do_stand);\n'
 '\tTONG_JX2OP_COMMAND sCmd;\n',
 '\t// ---- TOI NOI ----\n'
 '\tNpc[nNpcIdx].SendCommand(do_stand);\n'
 '\t// [c 01/09] BUOC 4 (chu game): "toi NPC mon phai cua minh roi XUAT SU xong, tu he thong bang\n'
 '\t// hoi xin vao" - goi dung ham \'xuatsu\' cua chinh script NPC mon phai (menu "Xuat su xuong\n'
 '\t// nui/xuatsu"): factionhead.lua:32 SetCamp(4)+SetCurCamp(4)+LeaveTeam()+SetTask(4134,1).\n'
 '\t// Theo dung luat cua menu: cap >= 60 va chua xuat su. Xuat su xong nghi 2 giay roi nop don\n'
 '\t// (tung buoc mot, khong lam hai viec trong cung mot nhip).\n'
 '\t// LUU Y (luat san co cua may chu): camp 4 bi mobinhtk.lua:100/:217 tu choi ("Chu do khong\n'
 '\t// the vao Tong Kim") -> bot da xuat su/vao bang se KHONG duoc goi di Tong Kim nua\n'
 '\t// (pb_TkDuTuCach da chan camp 4 tu 21/08).\n'
 '\tif ((int)Player[nIdx].m_cTask.GetSaveVal(PB_TASK_XUATSU) == 0)\n'
 '\t{\n'
 '\t\tif (Npc[nNpcIdx].m_Level < PB_CAP_XUATSU)\n'
 '\t\t{\n'
 '\t\t\tpb_Log("[BotBang] %s cap %d chua du %d de xuat su -> HUY xin vao bang %u\\n",\n'
 '\t\t\t       Player[nIdx].m_PlayerName, Npc[nNpcIdx].m_Level, PB_CAP_XUATSU, (unsigned int)b.dwBangID);\n'
 '\t\t\tif (bNguoiCon)\n'
 '\t\t\t{\n'
 '\t\t\t\tconst char* szCap = "Minh chua du cap 60 de xuat su nen chua xin vao bang duoc, thong cam nhe.";\n'
 '\t\t\t\tpb_GuiChatMat(nAi, nIdx, szCap, (int)strlen(szCap));\n'
 '\t\t\t}\n'
 '\t\t\tb.nBangPha = 0;  b.dwBangID = 0;  b.nBangNguoiIdx = 0;  b.dwBangNguoiID = 0;\n'
 '\t\t\tb.nBangTick = 0;  b.nBangThu = 0;  b.nBangNghiToi = 0;  b.nBangDoiMapTick = 0;\n'
 '\t\t\tb.walk.Reset();\n'
 '\t\t\treturn 0;\n'
 '\t\t}\n'
 '\t\tPlayer[nIdx].ExecuteScript((char*)s_facNpc[nFac].szScript, (char*)"xuatsu", 0, false);\n'
 '\t\tif ((int)Player[nIdx].m_cTask.GetSaveVal(PB_TASK_XUATSU) != 1)\n'
 '\t\t{\n'
 '\t\t\tpb_Log("[BotBang] %s goi \'xuatsu\' cua %s KHONG an (task %d = %d) -> HUY\\n",\n'
 '\t\t\t       Player[nIdx].m_PlayerName, s_facNpc[nFac].szTen, PB_TASK_XUATSU,\n'
 '\t\t\t       (int)Player[nIdx].m_cTask.GetSaveVal(PB_TASK_XUATSU));\n'
 '\t\t\tif (bNguoiCon)\n'
 '\t\t\t{\n'
 '\t\t\t\tconst char* szXs = "Minh xuat su khong duoc, de khi khac xin vao bang nhe.";\n'
 '\t\t\t\tpb_GuiChatMat(nAi, nIdx, szXs, (int)strlen(szXs));\n'
 '\t\t\t}\n'
 '\t\t\tb.nBangPha = 0;  b.dwBangID = 0;  b.nBangNguoiIdx = 0;  b.dwBangNguoiID = 0;\n'
 '\t\t\tb.nBangTick = 0;  b.nBangThu = 0;  b.nBangNghiToi = 0;  b.nBangDoiMapTick = 0;\n'
 '\t\t\tb.walk.Reset();\n'
 '\t\t\treturn 0;\n'
 '\t\t}\n'
 '\t\tpb_Log("[BotBang] %s da XUAT SU tai NPC %s (camp -> %d) - 2 giay nua nop don vao bang %u\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, s_facNpc[nFac].szTen,\n'
 '\t\t       (int)Npc[nNpcIdx].m_CurrentCamp, (unsigned int)b.dwBangID);\n'
 '\t\tb.nBangNghiToi = now + (unsigned int)(GAME_FPS * 2);\n'
 '\t\treturn 1;\n'
 '\t}\n'
 '\t// ---- BUOC 5: nop don xin vao DUNG bang cua nguoi ru ----\n'
 '\tTONG_JX2OP_COMMAND sCmd;\n')

# ============================================================ H15 nhan mat: tu khoa + bang chu
ap("H15 PB_WhisperReply: tu khoa chu game + phai la bang chu",
 '\tif (pB && strstr(szLow, "bang")\n'
 '\t && (strstr(szLow, "vao") || strstr(szLow, "vo bang") || strstr(szLow, "gia nh")\n'
 '\t  || strstr(szLow, "xin") || strstr(szLow, "moi") || strstr(szLow, "ru ")\n'
 '\t  || strstr(szLow, "join") || strstr(szLow, "bang h")))\n'
 '\t{\n'
 '\t\tbBangHoi = 1;\n'
 '\t\tconst DWORD dwNg = Player[p->nSenderIdx].m_cTong.m_nFlag\n'
 '\t\t                 ? Player[p->nSenderIdx].m_cTong.GetTongNameID() : 0;\n'
 '\t\tconst KTongJX2Tong* pTg = dwNg ? g_TongJX2.FindTong(dwNg) : NULL;\n'
 '\t\tif (Player[nBot].m_cTong.m_nFlag)\n'
 '\t\t\tsprintf(szTraLoi, "Minh co bang hoi roi, cam on ban nhe.");\n'
 '\t\telse if (!pTg)\n'
 '\t\t\tsprintf(szTraLoi, "Ban chua co bang hoi ma, ru minh vao dau?");\n'
 '\t\telse if (pB->nBangPha)\n'
 '\t\t\tsprintf(szTraLoi, "Minh dang tren duong ve NPC mon phai xin vao bang roi, cho chut.");\n'
 '\t\telse if (pB->nBanSap)\n'
 '\t\t\tsprintf(szTraLoi, "Minh dang ngoi ban sap, de khi khac minh xin vao bang nhe.");\n'
 '\t\telse if (pB->nDtPha != DTB_NGHI)\n'
 '\t\t\tsprintf(szTraLoi, "Minh dang lam nhiem vu Da Tau, xong viec minh xin vao bang sau nhe.");\n'
 '\t\telse\n'
 '\t\t{\n'
 '\t\t\tpB->nBangPha = 1;  pB->dwBangID = dwNg;\n'
 '\t\t\tpB->nBangNguoiIdx = p->nSenderIdx;  pB->dwBangNguoiID = Player[p->nSenderIdx].m_dwID;\n'
 '\t\t\tpB->nBangTick = 0;  pB->nBangThu = 0;  pB->nBangNghiToi = 0;  pB->nBangDoiMapTick = 0;\n'
 '\t\t\tsprintf(szTraLoi, "Ok! Minh ve NPC mon phai xin vao bang %s ngay day.", pTg->szName);\n',
 '\t// [c 01/09] (chu game) tu khoa: "vao bang" / "vo bang" / "tham gia bang hoi" / "vao bang hoat\n'
 '\t// dong" / "vao bang ko" / "vo bang k" -> chua "vao bang", "vo bang" hoac "tham gia bang"; nguoi\n'
 '\t// nhan PHAI la BANG CHU (btFigure 0 trong so thanh vien JX2) cua bang do. Da Tau dang lam thi\n'
 '\t// van nhan (pb_XinVaoBang chan pb_DaTau trong luc di; xong viec may trang thai Da Tau tu\n'
 '\t// dong bo lai theo course).\n'
 '\tif (pB && (strstr(szLow, "vao bang") || strstr(szLow, "vo bang") || strstr(szLow, "tham gia bang")))\n'
 '\t{\n'
 '\t\tbBangHoi = 1;\n'
 '\t\tconst DWORD dwNg = Player[p->nSenderIdx].m_cTong.m_nFlag\n'
 '\t\t                 ? Player[p->nSenderIdx].m_cTong.GetTongNameID() : 0;\n'
 '\t\tKTongJX2Tong* pTg = dwNg ? g_TongJX2.FindTong(dwNg) : NULL;\n'
 '\t\tint bBangChu = 0;\n'
 '\t\tif (pTg)\n'
 '\t\t{\n'
 '\t\t\tKTongJX2Member* pMe = g_TongJX2.FindMember(pTg,\n'
 '\t\t\t                          g_FileName2Id((LPSTR)Player[p->nSenderIdx].m_PlayerName));\n'
 '\t\t\tbBangChu = (pMe && pMe->btFigure == 0) ? 1 : 0;\n'
 '\t\t}\n'
 '\t\tif (Player[nBot].m_cTong.m_nFlag)\n'
 '\t\t\tsprintf(szTraLoi, "Minh co bang hoi roi, cam on ban nhe.");\n'
 '\t\telse if (!pTg)\n'
 '\t\t\tsprintf(szTraLoi, "Ban chua co bang hoi ma, ru minh vao dau?");\n'
 '\t\telse if (!bBangChu)\n'
 '\t\t\tsprintf(szTraLoi, "Ban khong phai bang chu bang %s, nho bang chu nhan cho minh nhe.", pTg->szName);\n'
 '\t\telse if (pB->nBangPha)\n'
 '\t\t\tsprintf(szTraLoi, "Minh dang tren duong ve NPC mon phai xin vao bang roi, cho chut.");\n'
 '\t\telse if (pB->nBanSap)\n'
 '\t\t\tsprintf(szTraLoi, "Minh dang ngoi ban sap, de khi khac minh xin vao bang nhe.");\n'
 '\t\telse\n'
 '\t\t{\n'
 '\t\t\tpB->nBangPha = 1;  pB->dwBangID = dwNg;\n'
 '\t\t\tpB->nBangNguoiIdx = p->nSenderIdx;  pB->dwBangNguoiID = Player[p->nSenderIdx].m_dwID;\n'
 '\t\t\tpB->nBangTick = 0;  pB->nBangThu = 0;  pB->nBangNghiToi = 0;  pB->nBangDoiMapTick = 0;\n'
 '\t\t\tsprintf(szTraLoi, "Ok! Minh ve Ba Lang Huyen gap NPC mon phai xuat su roi xin vao bang %s ngay day.", pTg->szName);\n')

# ============================================================ H16 TKKET4: ra cua trai
ap("H16 pb_TkRaTrai: dich = O TRAP noi duoc + sat vet = toi noi + xoay khi thua",
 '\t// Diem den = MOT O TREN VET TRAP, chon theo chi so bot de ca dan rai deu 11 o\n'
 '\t// thay vi chen mot goc. O nao khong dung duoc thi pb_ODat tim o trong ke ben.\n'
 '\tconst int nLech9 = (int)(&b - s_bots);\n'
 '\tint nOx = 0, nOy = 0;\n'
 '\t{\n'
 '\t\tconst int i9 = nLech9 % (nDaiVet + 1);\n'
 '\t\tif (!pb_ODat(nSub, aTx[k] + i9, aTy[k] + i9, nLech9, 4, &nOx, &nOy))\n'
 '\t\t{\n'
 '\t\t\tnOx = (aTx[k] + nDaiVet / 2) * 32;\n'
 '\t\t\tnOy = (aTy[k] + nDaiVet / 2) * 32;\n'
 '\t\t}\n'
 '\t}\n'
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
 '\t\treturn -1;                     // khong co duong\n',
 '\tconst int nLech9 = (int)(&b - s_bots);\n'
 '\t// [TKKET3 01/09] bot dung yen >60s (neo dat o dau pha 3): in ket qua tung\n'
 '\t// buoc cua duong ra cua de tim dung cho ket. CHI GHI LOG.\n'
 '\tconst int bKet3 = (b.uKet3Tick != 0\n'
 '\t                && now - b.uKet3Tick > (unsigned int)(GAME_FPS * 60));\n'
 '\t// [TKKET4 01/09] GOC "nhieu bot TK dung yen khong danh" (chu game). Do tran 17:50-18:20:\n'
 '\t// 81 con, 100% trai DONG, 6.160 dong [TkKet3-RA] deu nW=-1 nhung KHONG mot dong [BotA*] nao\n'
 '\t// tren map 379 -> A* khong loi, ma PB_WalkTo roi vao nhanh DUONG CUT im lang (FindPathServer\n'
 '\t// tra 2). Doc luoi Maps\\379_srv.fp: hanh lang trap kimratrai (1661+i,3098+i) la KHE TUONG,\n'
 '\t// cac o trap i>=6 la VAT CAN tren luoi; pb_ODat quet xoan oc quanh o trap (chi hoi "engine\n'
 '\t// trong + luoi khong chan") nen chon o TRONG o BEN KIA TUONG (1665,3106 / 1666,3107 /\n'
 '\t// 1667,3110 / 1674,3108...) -> A* dan toi block gan nhat roi -1 moi nhip -> bot dung sat cua\n'
 '\t// vinh vien, 12 phut sau bi cat "KET pha 3" va roi tran. Sua 3 lop:\n'
 '\t//  (1) dung trong 2 o quanh BAT KY o trap nao cua vet (ca hang nY va nY-1 - lib_map.lua:366\n'
 '\t//      AddTrapEx2) = DA TOI CUA -> xuong thang khau "buoc qua trap" (nguoi choi dam trap la bi\n'
 '\t//      hut; bot mien trap nen tu goi kich ban);\n'
 '\t//  (2) dich = CHINH O TRAP: luoi bao di duoc (CellObsSrv == 0) VA A* NOI duoc tu cho bot\n'
 '\t//      (FindPathServer tra 1 = duong tron), KHONG qua pb_ODat nua; chi so xoay theo\n'
 '\t//      b.nTkRaXoay de ca dan rai deu 22 o;\n'
 '\t//  (3) PB_WalkTo thua -> nhip sau xoay sang o trap khac (b.nTkRaXoay++).\n'
 '\t// Kem: PB_TrapLog cho bot pha 3 dap dung trap ra trai -> chay kich ban nhu nguoi choi.\n'
 '\t{\n'
 '\t\tconst int nBotX9 = bx / 32, nBotY9 = by / 32;\n'
 '\t\tint nGanVet = 999;\n'
 '\t\tfor (int i = 0; i <= nDaiVet; i++)\n'
 '\t\t\tfor (int r = 0; r < 2; r++)\n'
 '\t\t\t{\n'
 '\t\t\t\tint dcx = nBotX9 - (aTx[k] + i);       if (dcx < 0) dcx = -dcx;\n'
 '\t\t\t\tint dcy = nBotY9 - (aTy[k] + i - r);   if (dcy < 0) dcy = -dcy;\n'
 '\t\t\t\tconst int dc = (dcx > dcy) ? dcx : dcy;\n'
 '\t\t\t\tif (dc < nGanVet)\n'
 '\t\t\t\t\tnGanVet = dc;\n'
 '\t\t\t}\n'
 '\t\tif (nGanVet > 2)\n'
 '\t\t{\n'
 '\t\t\tint nOx = 0, nOy = 0, nChon = 0;\n'
 '\t\t\tstatic std::vector<int> s_vTmp9;   // vong lap game mot luong - dung lai, khong cap phat moi nhip\n'
 '\t\t\tconst int nSoO = (nDaiVet + 1) * 2;\n'
 '\t\t\tfor (int t = 0; t < nSoO && !nChon; t++)\n'
 '\t\t\t{\n'
 '\t\t\t\tconst int j  = (nLech9 + b.nTkRaXoay + t) % nSoO;\n'
 '\t\t\t\tconst int cx = aTx[k] + (j % (nDaiVet + 1));\n'
 '\t\t\t\tconst int cy = aTy[k] + (j % (nDaiVet + 1)) - (j / (nDaiVet + 1));\n'
 '\t\t\t\tconst int mx = cx * 32 + 16, my = cy * 32 + 16;\n'
 '\t\t\t\tif (SubWorld[nSub].CellObsSrv(mx, my) != 0)\n'
 '\t\t\t\t\tcontinue;                  // o trap nam trong tuong (luoi) - bo\n'
 '\t\t\t\tif (SubWorld[nSub].FindPathServer(bx, by, mx, my, s_vTmp9, false) != 1)\n'
 '\t\t\t\t\tcontinue;                  // khong noi duoc tu cho bot (ben kia tuong) - bo\n'
 '\t\t\t\tnOx = mx;  nOy = my;  nChon = 1;\n'
 '\t\t\t}\n'
 '\t\t\tif (!nChon)\n'
 '\t\t\t{\n'
 '\t\t\t\tnOx = (aTx[k] + nDaiVet / 2) * 32;\n'
 '\t\t\t\tnOy = (aTy[k] + nDaiVet / 2) * 32;\n'
 '\t\t\t}\n'
 '\t\t\tpb_LenNguaDuongXa(nIdx, nNpcIdx, b, now);\n'
 '\t\t\tconst int nW = PB_WalkTo(nNpcIdx, nOx, nOy, nSub, b.walk, PB_FAC_ARRIVE_MPS);\n'
 '\t\t\tif (bKet3 && ((now + (unsigned int)nLech9 * 7u) % (unsigned int)(GAME_FPS * 10)) == 0)\n'
 '\t\t\t\tpb_Log("[TkKet3-RA] %s nW=%d o(%d,%d) dich o(%d,%d) k=%d xoay=%d noi=%d\\n",\n'
 '\t\t\t\t       Player[nIdx].m_PlayerName, nW, bx / 32, by / 32, nOx / 32, nOy / 32, k,\n'
 '\t\t\t\t       b.nTkRaXoay, nChon);\n'
 '\t\t\tif (nW == 0)\n'
 '\t\t\t\treturn 0;                  // dang di bo toi cua trai\n'
 '\t\t\tif (nW < 0)\n'
 '\t\t\t{\n'
 '\t\t\t\tb.nTkRaXoay++;             // nhip sau thu o trap khac\n'
 '\t\t\t\tb.walk.Reset();\n'
 '\t\t\t\treturn -1;                 // khong co duong\n'
 '\t\t\t}\n'
 '\t\t\t// nW > 0: da toi sat o trap (<= 3 o) -> xuong khau "buoc qua trap" ben duoi\n'
 '\t\t}\n'
 '\t}\n')

# ============================================================ H17 PB_TrapLog: bot pha 3 dap trap ra trai
ap("H17 PB_TrapLog: chay kich ban trap ra trai cho bot pha 3",
 'void PB_TrapLog(int nPlayerIdx, unsigned long dwScriptId, int nMapX, int nMapY)\n'
 '{\n'
 '\tpb_Log("[BotTrap] %s dap trap id=%08X tai o(%d,%d) -> BO QUA (bot mien trap)\\n",\n',
 'void PB_TrapLog(int nPlayerIdx, unsigned long dwScriptId, int nMapX, int nMapY)\n'
 '{\n'
 '\t// [TKKET4 01/09] bot Tong Kim PHA 3 (trong hau doanh cho ra tran) dap DUNG trap RA TRAI\n'
 '\t// (tongratrai / kimratrai) -> chay kich ban y het nguoi choi (KNpc::CheckTrap goi ham nay\n'
 '\t// ngay truoc cho no "mien" bot; nguoi choi thi ExecuteScript(id, "main", idx) cung cho do).\n'
 '\t// Kich ban tu lo: 10 giay dau chi Talk + return, sau do SetPos ra chien truong, SetFightState,\n'
 '\t// SetDeathScript... - pb_TkLai pha 3 thay bot da o ngoai trai thi sang pha 4. Do tran 17:50:\n'
 '\t// 152 dong kimratrai (1975C4C8) + 224 tongratrai (EA163CE9) bi "BO QUA" - trong do co ca\n'
 '\t// nhung con sau do dung ket sat cua 12 phut.\n'
 '\tif (nPlayerIdx > 0 && nPlayerIdx < MAX_PLAYER)\n'
 '\t{\n'
 '\t\tPB_Bot* pB = NULL;\n'
 '\t\tfor (int i = 0; i < s_botCount; i++)\n'
 '\t\t\tif (s_bots[i].nPlayerIdx == nPlayerIdx && s_bots[i].dwID == Player[nPlayerIdx].m_dwID)\n'
 '\t\t\t{\n'
 '\t\t\t\tpB = &s_bots[i];\n'
 '\t\t\t\tbreak;\n'
 '\t\t\t}\n'
 '\t\tif (pB && pB->nTk == 3)\n'
 '\t\t{\n'
 '\t\t\tstatic unsigned long s_dwTrapRa[2] = { 0, 0 };\n'
 '\t\t\tif (!s_dwTrapRa[0])\n'
 '\t\t\t{\n'
 '\t\t\t\ts_dwTrapRa[0] = (unsigned long)g_FileName2Id((LPSTR)"\\\\script\\\\maps\\\\tongkim\\\\trap\\\\tongratrai.lua");\n'
 '\t\t\t\ts_dwTrapRa[1] = (unsigned long)g_FileName2Id((LPSTR)"\\\\script\\\\maps\\\\tongkim\\\\trap\\\\kimratrai.lua");\n'
 '\t\t\t}\n'
 '\t\t\tif (dwScriptId == s_dwTrapRa[0] || dwScriptId == s_dwTrapRa[1])\n'
 '\t\t\t{\n'
 '\t\t\t\tconst int nNpcB = Player[nPlayerIdx].m_nIndex;\n'
 '\t\t\t\tint nTx = 0, nTy = 0, nSx = 0, nSy = 0;\n'
 '\t\t\t\tif (nNpcB > 0 && nNpcB < MAX_NPC)\n'
 '\t\t\t\t\tNpc[nNpcB].GetMpsPos(&nTx, &nTy);\n'
 '\t\t\t\tPlayer[nPlayerIdx].ExecuteScript((DWORD)dwScriptId, (char*)"main", nPlayerIdx);\n'
 '\t\t\t\tif (nNpcB > 0 && nNpcB < MAX_NPC)\n'
 '\t\t\t\t\tNpc[nNpcB].GetMpsPos(&nSx, &nSy);\n'
 '\t\t\t\tpB->walk.Reset();\n'
 '\t\t\t\tpb_Log("[BotTK] %s dap trap ra trai %08X tai o(%d,%d) -> chay kich ban nhu nguoi choi: %s\\n",\n'
 '\t\t\t\t       Player[nPlayerIdx].m_PlayerName, (unsigned int)dwScriptId, nTx / 32, nTy / 32,\n'
 '\t\t\t\t       (nSx != nTx || nSy != nTy) ? "DA RA TRAN" : "kich ban tu choi (chua toi gio) - dung cho");\n'
 '\t\t\t\treturn;\n'
 '\t\t\t}\n'
 '\t\t}\n'
 '\t}\n'
 '\tpb_Log("[BotTrap] %s dap trap id=%08X tai o(%d,%d) -> BO QUA (bot mien trap)\\n",\n')

# ============================================================ H18 pha 3: da o ngoai trai -> pha 4 + reset xoay
ap("H18a pha 3: da o ngoai trai (trap hook) -> RA TRAN",
 '\t\t// [TKKET3 01/09] (chu game: "nhieu bot TK dung yen khong danh") MO XE bot\n'
 '\t\t// pha 3 dung nguyen mot cho >60s: do 3 tran 31/08 co 45-79 con/tran (15-32%,\n',
 '\t\t// [TKKET4 01/09] kich ban trap ra trai (PB_TrapLog) vua day bot ra NGOAI trai -> sang pha 4\n'
 '\t\t// ngay, khong di tim cua nua. Nhan dien "ngoai trai" y het pha 4 (hop 40 o quanh\n'
 '\t\t// TKPOS_GO_HDOANH cua phe minh).\n'
 '\t\t{\n'
 '\t\t\tint nHx3 = 0, nHy3 = 0;\n'
 '\t\t\tNpc[nNpcIdx].GetMpsPos(&nHx3, &nHy3);\n'
 '\t\t\tstatic const int aHx3[2] = { 1229, 1689 };   // TKPOS_GO_HDOANH (o)\n'
 '\t\t\tstatic const int aHy3[2] = { 3561, 3074 };\n'
 '\t\t\tconst int iT3 = pb_TkDaoTheTran(nSub);\n'
 '\t\t\tconst int h3  = (b.nTkPhe == 1) ? iT3 : (1 - iT3);\n'
 '\t\t\tint dx3 = nHx3 - aHx3[h3] * 32;  if (dx3 < 0) dx3 = -dx3;\n'
 '\t\t\tint dy3 = nHy3 - aHy3[h3] * 32;  if (dy3 < 0) dy3 = -dy3;\n'
 '\t\t\tif (dx3 > 40 * 32 || dy3 > 40 * 32)\n'
 '\t\t\t{\n'
 '\t\t\t\tb.nTk = 4;\n'
 '\t\t\t\tb.nTkTick = now;\n'
 '\t\t\t\tb.nTkDichTick = 0;\n'
 '\t\t\t\tb.nTkDaBoc = 0;  b.nTkTgX = 0;  b.nTkTgY = 0;  b.nTkTgTick = 0;\n'
 '\t\t\t\tb.nTkRaXoay = 0;\n'
 '\t\t\t\tb.walk.Reset();\n'
 '\t\t\t\tpb_Log("[BotTK] %s phe %s da o ngoai trai o(%d,%d) (qua trap nhu nguoi choi) -> RA TRAN\\n",\n'
 '\t\t\t\t       Player[nIdx].m_PlayerName, (b.nTkPhe == 1) ? "Tong" : "Kim", nHx3 / 32, nHy3 / 32);\n'
 '\t\t\t\treturn;\n'
 '\t\t\t}\n'
 '\t\t}\n'
 '\t\t// [TKKET3 01/09] (chu game: "nhieu bot TK dung yen khong danh") MO XE bot\n'
 '\t\t// pha 3 dung nguyen mot cho >60s: do 3 tran 31/08 co 45-79 con/tran (15-32%,\n')

ap("H18b reset xoay khi bao danh xong",
 '\t\t\tb.nTkChoRa   = now + (unsigned int)(GAME_FPS * 5 + (nLech % 5) * GAME_FPS);\n'
 '\t\t\tb.nTkMuaXong = 0;\n'
 '\t\t\tpb_Log("[BotTK] %s bao danh phe %s THANH CONG (lan %d)\\n",\n',
 '\t\t\tb.nTkChoRa   = now + (unsigned int)(GAME_FPS * 5 + (nLech % 5) * GAME_FPS);\n'
 '\t\t\tb.nTkMuaXong = 0;\n'
 '\t\t\tb.nTkRaXoay  = 0;              // [TKKET4] xoay o trap dich tu dau\n'
 '\t\t\tpb_Log("[BotTK] %s bao danh phe %s THANH CONG (lan %d)\\n",\n')

ap("H18c reset xoay khi hoi sinh ve pha 3",
 '\t\t\tb.nTkMuaXong = 0;              // mua binh moi cho luot sau\n',
 '\t\t\tb.nTkMuaXong = 0;              // mua binh moi cho luot sau\n'
 '\t\t\tb.nTkRaXoay  = 0;              // [TKKET4] xoay o trap dich tu dau\n')

ap("H18d reset xoay khi ra tran (duong goi kich ban)",
 '\t\tb.nTk = 4;\n'
 '\t\tb.nTkTick = now;\n'
 '\t\tb.nTkDichTick = 0;\n'
 '\t\t// (21/08) Moi luot ra tran la mot duong MOI.',
 '\t\tb.nTk = 4;\n'
 '\t\tb.nTkTick = now;\n'
 '\t\tb.nTkDichTick = 0;\n'
 '\t\tb.nTkRaXoay = 0;               // [TKKET4]\n'
 '\t\t// (21/08) Moi luot ra tran la mot duong MOI.')

ap("H18e reset xoay trong pb_TkGoCo",
 '\tb.nTkMuaXong = 0;  b.nTkRaTick = 0;  b.nTkChoRa = 0;\n',
 '\tb.nTkMuaXong = 0;  b.nTkRaTick = 0;  b.nTkChoRa = 0;\n'
 '\tb.nTkRaXoay = 0;\n')

# ============================================================ H19 xac dung thang -> ep ve do_revive
ap("H19 xac dung thang sau tran (mau 0 + do_stand) -> hoi sinh duoc",
 '\t\telse if (nowAll - b.nChetTuTick >= (unsigned int)(b.nTk ? GAME_FPS : GAME_FPS * 3))\n'
 '\t\t{\n'
 '\t\t\tb.nChetTuTick = nowAll;      // dat lai moc -> 3 giay nua thu tiep\n'
 '\t\t\tPlayer[nIdx].Revive(REMOTE_REVIVE_TYPE);\n',
 '\t\telse if (nowAll - b.nChetTuTick >= (unsigned int)(b.nTk ? GAME_FPS : GAME_FPS * 3))\n'
 '\t\t{\n'
 '\t\t\tb.nChetTuTick = nowAll;      // dat lai moc -> 3 giay nua thu tiep\n'
 '\t\t\t// [TKCHET 01/09] (chu game: "bot tham gia Tong Kim xong thi dung trong map bao danh")\n'
 '\t\t\t// Do tran 17:50: 13 con mau 0 nhung m_Doing = do_stand, moi giay goi Revive suot 17\n'
 '\t\t\t// phut khong an. Goc: task03 het tran NewWorld ca dan ve 324 dung luc bot dang chet -\n'
 '\t\t\t// KNpc::ChangeWorld goi DoStand() (KNpc.cpp:10368) xoa do_death/do_revive, ma\n'
 '\t\t\t// KPlayer::Revive (KPlayer.cpp:6944) doi DUNG do_revive moi hoi sinh -> xac dung thang\n'
 '\t\t\t// giua map bao danh vinh vien. Dua ve dung trang thai "chet cho hoi sinh" (chinh la\n'
 '\t\t\t// trang thai OnDeath dat o KNpc.cpp:2316 khi het hoat anh chet) roi hoi sinh nhu thuong.\n'
 '\t\t\tif (Npc[nNpcIdx].m_CurrentLife <= 0\n'
 '\t\t\t && Npc[nNpcIdx].m_Doing != do_death && Npc[nNpcIdx].m_Doing != do_revive)\n'
 '\t\t\t{\n'
 '\t\t\t\tpb_Log("[Bot] %s XAC DUNG THANG (mau %d, doing %d, map %d) -> ep ve do_revive de hoi sinh\\n",\n'
 '\t\t\t\t       Player[nIdx].m_PlayerName, Npc[nNpcIdx].m_CurrentLife, (int)Npc[nNpcIdx].m_Doing,\n'
 '\t\t\t\t       SubWorld[nSub].m_SubWorldID);\n'
 '\t\t\t\tNpc[nNpcIdx].m_Doing = do_revive;\n'
 '\t\t\t}\n'
 '\t\t\tPlayer[nIdx].Revive(REMOTE_REVIVE_TYPE);\n')

if THU:
    print("KIEM NEO XONG: %d loi" % loi)
    sys.exit(1 if loi else 0)

io.open(P, "w", encoding="latin-1", newline="").write(s)
sau = sum(1 for c in s if ord(c) > 127)
print("Xong: %d hunk moi. high-byte truoc=%d sau=%d (%s)"
      % (n, truoc, sau, "OK" if truoc == sau else "LECH!"))
