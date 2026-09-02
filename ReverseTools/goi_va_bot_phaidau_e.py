# -*- coding: utf-8 -*-
# [BOTPHAI-DAU 02/09, dot e] Chu game: "khi vao paty thi bot chua hien icon theo mon phai cua bot
# o mini paty".
#
# LAN NGUOC DUONG MA (khong doan):
#   1. Icon mon phai o mini party do CLIENT ve, lay tu KTeam::GetMemberInfo (KPlayerTeam.cpp:871
#      va :890, khoi #ifndef _SERVER):
#          pList[nNum].nFaction = Npc[nNpcIdx].nFirstFaction;
#   2. Npc[].nFirstFaction phia client duoc dien tu goi PLAYER_SYNC (KProtocolProcess.cpp:2796
#      va :2894).
#   3. May chu dien truong do trong KNpc::SendSyncData / SendSyncDataToNearRegion
#      (KNpc.cpp:6086 va :6303):
#          PlayerSync.nFirstFaction = (BYTE)Player[m_nPlayerIdx].GetFirstAddFaction();
#      = KPlayer.h:849 -> m_cFaction.m_nFirstAddFaction. (Nhanh m_btSimCityBot ben duoi la cua
#      bot SimCity - bot KPlayer KHONG di duong do.)
#   4. m_nFirstAddFaction CHI duoc dat o MOT cho: KPlayerFaction::AddFaction (KPlayerFaction.cpp:88)
#          m_nAddTimes++;  if (m_nAddTimes == 1) m_nFirstAddFaction = nFactionID;
#      va m_nAddTimes duoc NAP TU BLOB (KPlayerDBFuns.cpp:376 = BaseInfo.ijoincount) roi luu lai
#      (:1016).
#   => BOT nhan ban tu NHAN VAT MAU da tung vao phai (ijoincount >= 1), nen khi bot chay
#      gianhapmonphai -> SetFaction -> KPlayer::AddFaction (KPlayer.cpp:4249) -> AddFaction cua
#      lop faction, m_nAddTimes thanh 2, 3... KHONG BAO GIO bang 1 => m_nFirstAddFaction giu
#      nguyen gia tri cua nhan vat mau (-1 = chua vao phai, hoac phai CUA MAU). Bot vi vay gui
#      xuong client nFirstFaction = 255 (BYTE cua -1) => mini party khong ve duoc icon nao.
#
# VA (chi cham bot, KHONG dong vao duong nguoi choi that): ep m_nFirstAddFaction = phai HIEN TAI
# cua bot khi hai gia tri lech. Dung y nghia "phai dau tien da gia nhap" cua mot nhan vat chi
# tung vao dung mot phai. Loi keo them (dung huong): item magic_requiremenpai (KItemList.cpp:1204)
# va bang thanh vien bang hoi JX2 (KTongJX2.cpp:2428) cung doc chinh truong nay.
#
# LUU Y hien thi: KPlayer::SendFactionData (KPlayer.cpp:1922) chi gui cho CHINH chu (m_nNetConnectIdx),
# bot khong co client nen la no-op; nguoi choi quanh do nhan gia tri moi o lan dong bo vung ke tiep
# (PLAYER_SYNC) - y het nguoi choi that doi phai. Vi vay sua SOM (luc nap bot / vua vao phai xong)
# moi la cho dung, va gia tri duoc luu vao blob nen chi phai sua mot lan.
#
# Chi KPlayerBot.cpp. AP SAU: ... -> bot_nhom_bang_tk_c -> bot_bang_mau_d -> (bo nay).
# Chay: python goi_va_bot_phaidau_e.py [--thu]
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

# ---------------------------------------------------------------- E1: than ham
ap("E1 than pb_SuaPhaiDau",
 '// ===========================================================================\n'
 '// [BOTBANG-MAU 01/09 toi] (chu game) "bot da vao bang thanh cong se co MAU RIENG THEO BANG".\n',
 '// ===========================================================================\n'
 '// [BOTPHAI-DAU 02/09] (chu game) "khi vao paty thi bot chua hien icon theo mon phai cua bot o\n'
 '// mini paty".\n'
 '//\n'
 '// Icon do CLIENT ve tu KTeam::GetMemberInfo (KPlayerTeam.cpp:871/:890, khoi #ifndef _SERVER):\n'
 '//   pList[].nFaction = Npc[].nFirstFaction   <- PLAYER_SYNC (KProtocolProcess.cpp:2796/:2894)\n'
 '//   <- may chu dien o KNpc::SendSyncData (KNpc.cpp:6086/:6303):\n'
 '//      PlayerSync.nFirstFaction = Player[].GetFirstAddFaction() = m_cFaction.m_nFirstAddFaction\n'
 '//\n'
 '// m_nFirstAddFaction chi duoc dat o DUNG MOT cho - KPlayerFaction::AddFaction (:88):\n'
 '//      m_nAddTimes++;  if (m_nAddTimes == 1) m_nFirstAddFaction = nFactionID;\n'
 '// ma m_nAddTimes NAP TU BLOB (KPlayerDBFuns.cpp:376 = BaseInfo.ijoincount). Bot nhan ban tu\n'
 '// nhan vat mau DA tung vao phai nen ijoincount >= 1 => luc bot chay gianhapmonphai, m_nAddTimes\n'
 '// thanh 2/3/... khong bao gio bang 1 => m_nFirstAddFaction giu nguyen gia tri cua nhan vat mau\n'
 '// (-1 "chua vao phai", hoac phai cua mau) => bot gui xuong client nFirstFaction = 255 va mini\n'
 '// party khong co icon nao de ve.\n'
 '//\n'
 '// Sua: ep m_nFirstAddFaction = phai HIEN TAI cua bot. Dung nghia voi mot nhan vat chi tung vao\n'
 '// mot phai, va keo theo dung huong hai cho khac cung doc truong nay: item magic_requiremenpai\n'
 '// (KItemList.cpp:1204) + bang thanh vien bang hoi JX2 (KTongJX2.cpp:2428).\n'
 '// KHONG dong vao duong nguoi choi that (ham nay chi chay trong nhip bot).\n'
 '// ===========================================================================\n'
 'static void pb_SuaPhaiDau(int nIdx, int nNpcIdx, unsigned int now, int nLech)\n'
 '{\n'
 '\tif (((now + (unsigned int)nLech * 3u) % (unsigned int)(GAME_FPS * 10)) != 0)\n'
 '\t\treturn;                            // 10 giay/con, le pha voi pb_DongBoBang\n'
 '\tKPlayerFaction& f = Player[nIdx].m_cFaction;\n'
 '\tconst int nCur = f.m_nCurFaction;\n'
 '\tif (nCur < 0 || nCur >= MAX_FACTION)\n'
 '\t\treturn;                            // bot chua vao phai - khong co gi de dong bo\n'
 '\tif (f.m_nFirstAddFaction == nCur)\n'
 '\t\treturn;\n'
 '\tpb_Log("[BotPhai] %s sua PHAI DAU (first faction) %d -> %d de client ve duoc icon mon phai"\n'
 '\t       " (mini party / bang hoi)\\n", Player[nIdx].m_PlayerName,\n'
 '\t       f.m_nFirstAddFaction, nCur);\n'
 '\tf.m_nFirstAddFaction = nCur;\n'
 '\tif (f.m_nAddTimes <= 0)\n'
 '\t\tf.m_nAddTimes = 1;                 // giu bat bien cua KPlayerFaction (da vao phai = >= 1)\n'
 '}\n'
 '\n'
 '// ===========================================================================\n'
 '// [BOTBANG-MAU 01/09 toi] (chu game) "bot da vao bang thanh cong se co MAU RIENG THEO BANG".\n')

# ---------------------------------------------------------------- E2: goi moi nhip
ap("E2 goi pb_SuaPhaiDau moi nhip",
 '\t// [BOTBANG-MAU 01/09] dong bo bang + mau theo bang (10 giay/con, so le). Dat truoc moi\n'
 '\t// nhanh return de bot dang Tong Kim / ban sap / Da Tau cung giu dung ho so bang.\n'
 '\tpb_DongBoBang(nIdx, nNpcIdx, b, nowAll, (int)(&b - s_bots));\n',
 '\t// [BOTBANG-MAU 01/09] dong bo bang + mau theo bang (10 giay/con, so le). Dat truoc moi\n'
 '\t// nhanh return de bot dang Tong Kim / ban sap / Da Tau cung giu dung ho so bang.\n'
 '\tpb_DongBoBang(nIdx, nNpcIdx, b, nowAll, (int)(&b - s_bots));\n'
 '\t// [BOTPHAI-DAU 02/09] icon mon phai o mini party doc "phai dau tien" - bot nhan ban co\n'
 '\t// ijoincount >= 1 nen khong bao gio duoc dat. Sua o day, gia tri se di theo goi PLAYER_SYNC.\n'
 '\tpb_SuaPhaiDau(nIdx, nNpcIdx, nowAll, (int)(&b - s_bots));\n')

# ---------------------------------------------------------------- E3: sua ngay khi vua vao phai
ap("E3 sua ngay sau khi vao phai xong",
 '\t\tpb_Log("[Bot] %s da vao %s (phai %d, he %d)\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, s_facNpc[b.nFaction].szTen,\n'
 '\t\t       b.nFaction, Npc[nNpcIdx].m_Series);\n',
 '\t\tpb_Log("[Bot] %s da vao %s (phai %d, he %d)\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, s_facNpc[b.nFaction].szTen,\n'
 '\t\t       b.nFaction, Npc[nNpcIdx].m_Series);\n'
 '\t\t// [BOTPHAI-DAU 02/09] chot "phai dau tien" NGAY: KPlayerFaction::AddFaction chi dat no\n'
 '\t\t// khi m_nAddTimes == 1, ma bot nhan ban tu nhan vat mau da co ijoincount >= 1 nen dieu\n'
 '\t\t// kien do khong bao gio dung -> mini party khong co icon mon phai (xem pb_SuaPhaiDau).\n'
 '\t\tif (Player[nIdx].m_cFaction.m_nFirstAddFaction != b.nFaction)\n'
 '\t\t{\n'
 '\t\t\tpb_Log("[BotPhai] %s sua PHAI DAU (first faction) %d -> %d ngay khi vao phai\\n",\n'
 '\t\t\t       Player[nIdx].m_PlayerName,\n'
 '\t\t\t       Player[nIdx].m_cFaction.m_nFirstAddFaction, b.nFaction);\n'
 '\t\t\tPlayer[nIdx].m_cFaction.m_nFirstAddFaction = b.nFaction;\n'
 '\t\t\tif (Player[nIdx].m_cFaction.m_nAddTimes <= 0)\n'
 '\t\t\t\tPlayer[nIdx].m_cFaction.m_nAddTimes = 1;\n'
 '\t\t}\n')

if THU:
    print("KIEM NEO XONG: %d loi" % loi)
    sys.exit(1 if loi else 0)

io.open(P, "w", encoding="latin-1", newline="").write(s)
sau = sum(1 for c in s if ord(c) > 127)
print("Xong: %d hunk moi. high-byte truoc=%d sau=%d (%s)"
      % (n, truoc, sau, "OK" if truoc == sau else "LECH!"))
