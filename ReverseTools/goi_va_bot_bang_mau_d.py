# -*- coding: utf-8 -*-
# [BOTBANG-MAU + BOTBANG-CAP90 + BOTNHOM-IMLANG 01/09 toi, dot d] Chu game (nguyen van):
#  * "Bot da vao bang thanh cong se co mau rieng theo bang"
#  * "(them gioi han bot tren cap 90 nguoi choi nhan chat mat moi ve xuat su - xin vao bang)"
#  * "khi bot vao paty va roi paty khong can nhan chat mat cho nguoi choi - ban hay tat cai do di"
#
# CO SO (doc ma, khong doan):
#  - "Mau" cua nguoi choi = m_CurrentCamp cua KNpc. Bang hoi dat mau nay: KPlayerTong::AddTong
#    (KPlayerTong.cpp:571) va ::Login (:1364) deu gan Npc.m_Camp = m_nCamp va Npc.m_CurrentCamp =
#    m_nCamp (chi khi KHONG o to doi). Camp cua bang = KTongJX2Tong::btCamp (1 chinh / 2 ta /
#    3 trung lap; camp 4 = tu do, la dieu kien BAT BUOC de xin vao bang - chinh la ly do phai
#    xuat su truoc).
#  - Nguoi xung quanh thay doi mau nho KNpc::SetCurrentCamp (KNpc.cpp:475) BROADCAST goi
#    s2c_npcchgcurcamp cho 9 vung. AddTong gan THANG m_CurrentCamp nen KHONG phat goi -
#    nguoi dang dung canh khong thay doi mau cho toi khi dong bo lai vung.
#  - Ten bang tren dau: KNpc::SendSyncData (KNpc.cpp:5966) lay tu chinh
#    Player[].m_cTong.GetTongName() - bot la KPlayer THAT nen chi can dien m_cTong la du.
#  - Sau RESTART: blob bot CO luu dwTongID (KPlayerDBFuns.cpp:989) va nap lai qua
#    DBSetTongNameID (:357, dat m_nFlag = 1) NHUNG bot khong qua KPlayerTong::Login (duong
#    dang nhap cua nguoi that, SGDI_TONG_LOGIN tu relay) nen ten bang + camp + chuc vu TRONG
#    => bot mat mau bang sau moi lan khoi dong lai. Ban sao g_TongJX2 (dong bo tu relay) co
#    du: FindTong(id)->btCamp/szName + FindMember(...)->btFigure.
#  => Them mot khau DONG BO NHE 10 giay/con: doc ban sao g_TongJX2, dien m_cTong, dat camp,
#     phat SetCurrentCamp khi lech. Khong them goi tin, khong doi cau truc.
#
# HE QUA TICH CUC (dinh chinh ghi chu dot c): bot vao bang xong co camp 1/2/3 -> mobinhtk.lua:100
# ("Chu do khong the vao Tong Kim", chan camp 4) KHONG con chan nua => bot vao bang VAN di duoc
# Tong Kim. Chi bot da xuat su ma CHUA duoc duyet don moi tam thoi mang camp 4.
#
# Chi KPlayerBot.cpp. AP SAU: ... -> bot_nhom_bang_tk_c -> (bo nay).
# Chay: python goi_va_bot_bang_mau_d.py [--thu]   (--thu = chi kiem neo, khong ghi)
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

# ===================================================================== A. TAT PM TO DOI
# A1: bo cau "Ok, minh vao nhom nhe!" khi bot vao nhom
ap("A1 bo PM khi vao nhom",
 '\tpb_Log("[BotNhomNguoi] %s vao nhom cua nguoi choi %s (%d thanh vien)\\n",\n'
 '\t       Player[nBot].m_PlayerName, Player[nMoi].m_PlayerName, g_Team[ta.m_nID].m_nMemNum);\n'
 '\t{\n'
 '\t\tconst char* szOk = "Ok, minh vao nhom nhe!";\n'
 '\t\tpb_GuiChatMat(nMoi, nBot, szOk, (int)strlen(szOk));\n'
 '\t}\n'
 '\treturn 1;\n',
 '\t// [d 01/09] (chu game: "khi bot vao paty va roi paty khong can nhan chat mat cho nguoi\n'
 '\t// choi - tat cai do di") KHONG gui PM khi vao nhom nua; chi ghi bot.log.\n'
 '\tpb_Log("[BotNhomNguoi] %s vao nhom cua nguoi choi %s (%d thanh vien)\\n",\n'
 '\t       Player[nBot].m_PlayerName, Player[nMoi].m_PlayerName, g_Team[ta.m_nID].m_nMemNum);\n'
 '\treturn 1;\n')

# A2: pb_RoiNhomNguoi - bo tham so szPm, khong gui PM nua
ap("A2 pb_RoiNhomNguoi bo PM",
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
 '}\n',
 '// [BOTNHOM-NGUOI c] roi nhom + xoa dau vet de moi nhanh khac thoi coi bot la "dang o nhom nguoi\n'
 '// that". Dung MOT ham cho moi nhanh tu doi map cua bot (Xa Phu / Da Tau / Tong Kim / ve thanh /\n'
 '// xin bang).\n'
 '// [d 01/09] (chu game: "khi bot vao paty va roi paty khong can nhan chat mat cho nguoi choi -\n'
 '// tat cai do di") DA BO khau nhan mat bao truoc khi roi; ly do chi con di vao bot.log.\n'
 'static void pb_RoiNhomNguoi(int nIdx, PB_Bot& b, const char* szLyDo)\n'
 '{\n'
 '\tif (pb_TrongNhom(nIdx))\n'
 '\t\tpb_RoiNhom(nIdx, szLyDo);\n'
 '\tb.nNhomNguoiIdx = 0;  b.dwNhomNguoiID = 0;  b.nNhomNguoiSub = -1;  b.uNhomNguoiLac = 0;\n'
 '}\n')

ap("A3 khai bao truoc pb_RoiNhomNguoi",
 'static void pb_RoiNhomNguoi(int nIdx, PB_Bot& b, const char* szPm, const char* szLyDo);\n',
 'static void pb_RoiNhomNguoi(int nIdx, PB_Bot& b, const char* szLyDo);\n')

# A4..A11: cac cho goi - bo tham so PM
ap("A4 goi: nguoi choi sang map khac",
 '\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "Ban di map khac roi, minh roi nhom nhe.", "nguoi choi da sang map khac");\n',
 '\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "nguoi choi da sang map khac");\n')

ap("A5 goi: bot lac 5 phut",
 '\t\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "Minh lac mat ban roi, minh roi nhom nhe.", "bot lac khoi map nhom qua 5 phut");\n',
 '\t\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "bot lac khoi map nhom qua 5 phut");\n')

ap("A6 goi: Da Tau toi NPC",
 '\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "Minh di lam nhiem vu Da Tau day, ban choi vui nhe.", "di lam nhiem vu Da Tau");\n'
 '\t\t\tif (!pb_DtVeThanh(nIdx, nNpcIdx, b, nha, nLech))\n',
 '\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "di lam nhiem vu Da Tau");\n'
 '\t\t\tif (!pb_DtVeThanh(nIdx, nNpcIdx, b, nha, nLech))\n')

ap("A7 goi: Da Tau toi Xa Phu",
 '\t\t\tpb_RoiNhomNguoi(nIdx, b, "Minh di lam nhiem vu Da Tau day, ban choi vui nhe.", "di lam nhiem vu Da Tau");\n'
 '\t\tif (SubWorld[nSub].m_SubWorldID != nha.nMap)\n',
 '\t\t\tpb_RoiNhomNguoi(nIdx, b, "di lam nhiem vu Da Tau");\n'
 '\t\tif (SubWorld[nSub].m_SubWorldID != nha.nMap)\n')

ap("A8 goi: Da Tau ve tra (khac map)",
 '\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "Minh xong nhiem vu Da Tau roi, roi nhom di tra nhiem vu nhe.", "ve tra nhiem vu Da Tau");\n',
 '\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "ve tra nhiem vu Da Tau");\n')

ap("A9 goi: Da Tau du cuon",
 '\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "Minh xong nhiem vu Da Tau roi, roi nhom di tra nhiem vu nhe.", "xong nhiem vu Da Tau - di tra");\n',
 '\t\t\t\tpb_RoiNhomNguoi(nIdx, b, "xong nhiem vu Da Tau - di tra");\n')

ap("A10 goi: ve thanh",
 '\t\t\tpb_RoiNhomNguoi(nIdx, b, "Minh phai ve thanh, roi nhom nhe.", "ve thanh thi/thon");\n',
 '\t\t\tpb_RoiNhomNguoi(nIdx, b, "ve thanh thi/thon");\n')

ap("A11 goi: vao Tong Kim",
 '\t\t\tpb_RoiNhomNguoi(nIdx, b, "Minh di Tong Kim day, roi nhom nhe.", "vao Tong Kim");\n',
 '\t\t\tpb_RoiNhomNguoi(nIdx, b, "vao Tong Kim");\n')

ap("A12 goi: di xin vao bang",
 '\t\tpb_RoiNhomNguoi(nIdx, b, "Minh roi nhom di xin vao bang, xong viec quay lai nhe.", "ve NPC mon phai xin vao bang");\n',
 '\t\tpb_RoiNhomNguoi(nIdx, b, "ve NPC mon phai xin vao bang");\n')

# ===================================================================== B. GIOI HAN CAP 90
ap("B1 nguong cap 90 khi nhan loi ru vao bang",
 '#define PB_TASK_XUATSU   4134\n'
 '#define PB_CAP_XUATSU    60\n',
 '#define PB_TASK_XUATSU   4134\n'
 '#define PB_CAP_XUATSU    60\n'
 '// [d 01/09] (chu game) "them gioi han bot TREN CAP 90 nguoi choi nhan chat mat moi ve xuat su -\n'
 '// xin vao bang". Nguong nay CHAT HON luat cua chinh game (client doi cap >= 60 de gia nhap,\n'
 '// KPlayerTong.cpp:196) va duoc kiem ngay o khau nhan PM, truoc khi bot roi cho dang lam.\n'
 '#define PB_CAP_VAOBANG   90\n')

ap("B2 kiem cap trong PB_WhisperReply",
 '\t\telse if (!bBangChu)\n'
 '\t\t\tsprintf(szTraLoi, "Ban khong phai bang chu bang %s, nho bang chu nhan cho minh nhe.", pTg->szName);\n'
 '\t\telse if (pB->nBangPha)\n',
 '\t\telse if (!bBangChu)\n'
 '\t\t\tsprintf(szTraLoi, "Ban khong phai bang chu bang %s, nho bang chu nhan cho minh nhe.", pTg->szName);\n'
 '\t\telse if (nCapBot < PB_CAP_VAOBANG)\n'
 '\t\t\tsprintf(szTraLoi, "Minh moi cap %d, phai tu cap %d tro len minh moi dam xuat su xin vao bang.",\n'
 '\t\t\t        nCapBot, PB_CAP_VAOBANG);\n'
 '\t\telse if (pB->nBangPha)\n')

ap("B3 doc cap bot trong PB_WhisperReply",
 '\tif (pB && (strstr(szLow, "vao bang") || strstr(szLow, "vo bang") || strstr(szLow, "tham gia bang")))\n'
 '\t{\n'
 '\t\tbBangHoi = 1;\n',
 '\tif (pB && (strstr(szLow, "vao bang") || strstr(szLow, "vo bang") || strstr(szLow, "tham gia bang")))\n'
 '\t{\n'
 '\t\tbBangHoi = 1;\n'
 '\t\tconst int nNpcW = Player[nBot].m_nIndex;\n'
 '\t\tconst int nCapBot = (nNpcW > 0 && nNpcW < MAX_NPC) ? Npc[nNpcW].m_Level : 0;\n')

# ===================================================================== C. MAU THEO BANG
ap("C1 than pb_DongBoBang",
 '// [BOTBANG c 01/09] "da xuat su" = TASK_DUNGCHUNG2',
 '// ===========================================================================\n'
 '// [BOTBANG-MAU 01/09 toi] (chu game) "bot da vao bang thanh cong se co MAU RIENG THEO BANG".\n'
 '//\n'
 '// "Mau" = m_CurrentCamp cua KNpc; bang hoi dat no = camp CUA BANG (KTongJX2Tong::btCamp,\n'
 '// 1 chinh / 2 ta / 3 trung lap). Duong cua nguoi that: relay bao ve -> SSOI_TONG_ADD ->\n'
 '// KPlayerTong::AddTong (KPlayerTong.cpp:571) gan Npc.m_Camp + m_CurrentCamp; luc dang nhap\n'
 '// lai thi SGDI_TONG_LOGIN -> KPlayerTong::Login (:1364) lam y het.\n'
 '//\n'
 '// HAI LO HONG voi bot, ham nay va CA HAI (KHONG them goi tin, khong doi cau truc):\n'
 '//  1. AddTong gan THANG m_CurrentCamp nen KHONG phat goi - nguoi dang dung canh bot khong\n'
 '//     thay doi mau. Ta goi KNpc::SetCurrentCamp (KNpc.cpp:475) = dung ham BROADCAST\n'
 '//     s2c_npcchgcurcamp cho 9 vung ma engine dung cho moi thay doi mau khac.\n'
 '//  2. Sau RESTART bot KHONG qua duong Login (khong co bat tay voi relay): blob chi tra lai\n'
 '//     dwTongID (KPlayerDBFuns.cpp:357 DBSetTongNameID, co dat m_nFlag = 1) con TEN BANG,\n'
 '//     CAMP, CHUC VU deu trong -> bot mat mau bang va mat ten bang tren dau. Ban sao\n'
 '//     g_TongJX2 (relay dong bo ve) co du: doc ra roi dien lai m_cTong.\n'
 '//\n'
 '// Ten bang tren dau khong can lam gi them: KNpc::SendSyncData (KNpc.cpp:5966) lay thang tu\n'
 '// Player[].m_cTong.GetTongName() - bot la KPlayer THAT nen dien m_cTong la du.\n'
 '//\n'
 '// KHONG dong mau khi: dang Tong Kim (script dat camp phe Tong/Kim - de nguyen), dang o to doi\n'
 '// (KPlayerTong::Login cung chi dat CurrentCamp khi "!m_cTeam.m_nFlag" - AcceptTeam ghi de camp\n'
 '// thanh vien bang camp doi truong).\n'
 '// ===========================================================================\n'
 'static void pb_DongBoBang(int nIdx, int nNpcIdx, PB_Bot& b, unsigned int now, int nLech)\n'
 '{\n'
 '\tif (((now + (unsigned int)nLech) % (unsigned int)(GAME_FPS * 10)) != 0)\n'
 '\t\treturn;                            // 10 giay/con, so le theo chi so bot\n'
 '\tKPlayerTong& t = Player[nIdx].m_cTong;\n'
 '\tconst DWORD dwId = t.GetTongNameID();\n'
 '\tif (dwId == 0)\n'
 '\t\treturn;                            // bot khong o bang nao\n'
 '\tif (!g_TongJX2.m_bSynced)\n'
 '\t\treturn;                            // ban sao tu relay chua ve - chua ket luan duoc gi\n'
 '\tKTongJX2Tong* pT = g_TongJX2.FindTong(dwId);\n'
 '\tKTongJX2Member* pMe = pT ? g_TongJX2.FindMember(pT,\n'
 '\t                          g_FileName2Id((LPSTR)Player[nIdx].m_PlayerName)) : NULL;\n'
 '\tif (!pT || !pMe)\n'
 '\t{\n'
 '\t\t// bang giai tan / bot bi duoi luc offline. Xoa ho so bang; CO Y khong dong vao camp\n'
 '\t\t// (bot con mang camp mon phai hoac camp tu do sau xuat su - doi o day la doi gameplay).\n'
 '\t\tpb_Log("[BotBang] %s khong con la thanh vien bang %u -> xoa ho so bang\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, (unsigned int)dwId);\n'
 '\t\tt.Clear();\n'
 '\t\treturn;\n'
 '\t}\n'
 '\tconst int nCamp = (int)pT->btCamp;\n'
 '\tconst int bMoi = (!t.m_nFlag || t.m_nCamp != nCamp || t.m_szName[0] == 0\n'
 '\t               || strcmp(t.m_szName, pT->szName) != 0\n'
 '\t               || t.m_nFigure != (int)pMe->btFigure);\n'
 '\tif (bMoi)\n'
 '\t{\n'
 '\t\tt.m_nFlag   = 1;\n'
 '\t\tt.m_nFigure = (int)pMe->btFigure;\n'
 '\t\tt.m_nCamp   = nCamp;\n'
 '\t\tstrncpy(t.m_szName, pT->szName, sizeof(t.m_szName) - 1);\n'
 '\t\tt.m_szName[sizeof(t.m_szName) - 1] = 0;\n'
 '\t\t// ten bang chu (client hien o bang thanh vien) - thanh vien co btFigure 0\n'
 '\t\t{\n'
 '\t\t\tstd::map<DWORD, KTongJX2Member>::iterator it;\n'
 '\t\t\tfor (it = pT->mapMember.begin(); it != pT->mapMember.end(); ++it)\n'
 '\t\t\t\tif (it->second.btFigure == 0)\n'
 '\t\t\t\t{\n'
 '\t\t\t\t\tstrncpy(t.m_szMasterName, it->second.szName, sizeof(t.m_szMasterName) - 1);\n'
 '\t\t\t\t\tt.m_szMasterName[sizeof(t.m_szMasterName) - 1] = 0;\n'
 '\t\t\t\t\tbreak;\n'
 '\t\t\t\t}\n'
 '\t\t}\n'
 '\t\tpb_Log("[BotBang] %s dong bo bang \'%s\' (id %u, camp %d, chuc vu %d) tu ban sao relay\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, t.m_szName, (unsigned int)dwId, nCamp, t.m_nFigure);\n'
 '\t}\n'
 '\tif (Npc[nNpcIdx].m_Camp != nCamp)\n'
 '\t\tNpc[nNpcIdx].m_Camp = nCamp;       // y het AddTong / Login\n'
 '\t// MAU hien tren dau: chi dat khi khong dang Tong Kim va khong o to doi (dung luat cua\n'
 '\t// KPlayerTong::Login). SetCurrentCamp phat goi cho moi nguoi quanh do thay doi mau ngay.\n'
 '\tif (!b.nTk && !Player[nIdx].m_cTeam.m_nFlag && Npc[nNpcIdx].m_CurrentCamp != nCamp)\n'
 '\t{\n'
 '\t\tpb_Log("[BotBang] %s doi mau theo bang \'%s\': camp %d -> %d\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, t.m_szName,\n'
 '\t\t       (int)Npc[nNpcIdx].m_CurrentCamp, nCamp);\n'
 '\t\tNpc[nNpcIdx].SetCurrentCamp(nCamp);\n'
 '\t}\n'
 '}\n'
 '\n'
 '// [BOTBANG c 01/09] "da xuat su" = TASK_DUNGCHUNG2')

ap("C2 goi pb_DongBoBang moi nhip",
 '\t// ap CHE DO cho bot den muon (sinh / vao phai xong SAU khi chu game bam lenh)\n',
 '\t// [BOTBANG-MAU 01/09] dong bo bang + mau theo bang (10 giay/con, so le). Dat truoc moi\n'
 '\t// nhanh return de bot dang Tong Kim / ban sap / Da Tau cung giu dung ho so bang.\n'
 '\tpb_DongBoBang(nIdx, nNpcIdx, b, nowAll, (int)(&b - s_bots));\n'
 '\n'
 '\t// ap CHE DO cho bot den muon (sinh / vao phai xong SAU khi chu game bam lenh)\n')

# ===================================================================== D. dinh chinh ghi chu dot c
ap("D1 dinh chinh ghi chu camp 4 / Tong Kim",
 '\t// LUU Y (luat san co cua may chu): camp 4 bi mobinhtk.lua:100/:217 tu choi ("Chu do khong\n'
 '\t// the vao Tong Kim") -> bot da xuat su/vao bang se KHONG duoc goi di Tong Kim nua\n'
 '\t// (pb_TkDuTuCach da chan camp 4 tu 21/08).\n',
 '\t// LUU Y (luat san co cua may chu): xuat su dat camp 4, ma camp 4 bi mobinhtk.lua:100/:217\n'
 '\t// tu choi ("Chu do khong the vao Tong Kim"). [d 01/09 dinh chinh] Day chi la trang thai\n'
 '\t// TAM: vao bang xong AddTong dat camp = camp CUA BANG (1/2/3) nen bot lai du tu cach di\n'
 '\t// Tong Kim; chi bot da xuat su ma don CHUA duoc duyet moi tam thoi mang camp 4.\n')

if THU:
    print("KIEM NEO XONG: %d loi" % loi)
    sys.exit(1 if loi else 0)

io.open(P, "w", encoding="latin-1", newline="").write(s)
sau = sum(1 for c in s if ord(c) > 127)
print("Xong: %d hunk moi. high-byte truoc=%d sau=%d (%s)"
      % (n, truoc, sau, "OK" if truoc == sau else "LECH!"))
