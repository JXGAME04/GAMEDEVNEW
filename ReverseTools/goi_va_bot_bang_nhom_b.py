# -*- coding: utf-8 -*-
# [BOTBANG/BOTNHOM-NGUOI b 01/09] Vong phan bien (workflow 5 tac tu) tren ban da thi cong
# goi_va_bot_bang_nhom.py - 8 mieng va nho, cung tep KPlayerBot.cpp:
#  H1/H2  PB_Bot them nNhomNguoiSub (subworld nguoi moi luc vao nhom) - bao "doi map" theo
#         NGUOI CHOI, khong bao sai khi chinh bot hoi sinh o map khac.
#  H3     PB_MoiVaoNhom: tu choi them khi dang giua nhiem vu Da Tau (nDtPha != DTB_NGHI)
#         hoac dang di xin bang (nBangPha); KHONG ep SetCanTeamFlag (map cam to doi bot
#         cung phai chiu); log tu choi gion 3 giay; luu nNhomNguoiSub.
#  H4     canh doi map: so subworld HIEN TAI cua nguoi moi voi luc vao nhom.
#  H5     pb_TkDuTuCach: bot dang trong nhom nguoi that KHONG bi goi quan Tong Kim.
#  H6     PM ru bang: tu khoa 'bang' PHAI kem vao/vo bang/gia nh/xin/moi/ru/join/bang h
#         (nguoi go khong dau: 'cap bang nhau', 'bang gia' khong con dinh); tu choi lich su
#         khi dang ban sap / giua nhiem vu Da Tau; bo ca cau dang cho (nPmDenHan) de cau
#         'Ok! Minh ve NPC...' chac chan duoc gui.
#  H7     ChangeWorld ve NPC so le theo chi so bot (3..5 giay) - tranh ca loat cung khung.
#  H8     roi nhom nguoi that de di xin bang -> nhan mat bao truoc.
# AP SAU goi_va_bot_bang_nhom.py. Idempotent.
import io, sys
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerBot.cpp"
s = io.open(P, "r", encoding="latin-1", newline="").read()
truoc = sum(1 for c in s if ord(c) > 127)
n = 0
def ap(ten, cu, moi):
    global s, n
    if moi in s:
        print("  [=] %s da ap tu truoc" % ten); return
    if s.count(cu) != 1:
        print("LOI: neo %s khop %d cho (can 1)" % (ten, s.count(cu))); sys.exit(1)
    s = s.replace(cu, moi); n += 1
    print("  [+] %s" % ten)

ap("H1 truong nNhomNguoiSub",
 '\tDWORD        dwNhomNguoiID;               // chot danh tinh nguoi moi\n',
 '\tDWORD        dwNhomNguoiID;               // chot danh tinh nguoi moi\n'
 '\tint          nNhomNguoiSub;               // [b] subworld cua nguoi moi LUC vao nhom (-1 = khong)\n')

ap("H2 khoi tao nNhomNguoiSub",
 '\t\tb.nPmCamToi = 0;  b.nPmLapHash = 0;\n'
 '\t\tb.nNhomNguoiIdx = 0;  b.dwNhomNguoiID = 0;\n',
 '\t\tb.nPmCamToi = 0;  b.nPmLapHash = 0;\n'
 '\t\tb.nNhomNguoiIdx = 0;  b.dwNhomNguoiID = 0;  b.nNhomNguoiSub = -1;\n')

ap("H3a PB_MoiVaoNhom tu choi them + gion log",
 '\tif (pB->nTk || pB->nBanSap\n'
 '\t || Npc[nNpcB].m_Doing == do_death || Npc[nNpcB].m_Doing == do_revive)\n'
 '\t{\n'
 '\t\tpb_Log("[BotNhomNguoi] %s tu choi loi moi cua %s (dang Tong Kim / ban sap / chet)\\n",\n'
 '\t\t       Player[nBot].m_PlayerName, Player[nMoi].m_PlayerName);\n'
 '\t\treturn 0;\n'
 '\t}\n',
 '\tstatic unsigned int s_uTcLog = 0;         // [b] gion log tu choi 3 giay (nguoi bam lien tuc)\n'
 '\tconst unsigned int uNowTc = GetTickCount();\n'
 '\tif (pB->nTk || pB->nBanSap || pB->nBangPha || pB->nDtPha != DTB_NGHI\n'
 '\t || Npc[nNpcB].m_Doing == do_death || Npc[nNpcB].m_Doing == do_revive)\n'
 '\t{\n'
 '\t\tif (uNowTc - s_uTcLog >= 3000)\n'
 '\t\t{\n'
 '\t\t\ts_uTcLog = uNowTc;\n'
 '\t\t\tpb_Log("[BotNhomNguoi] %s tu choi loi moi cua %s (TK=%d sap=%d bang=%d DaTau=%d chet=%d)\\n",\n'
 '\t\t\t       Player[nBot].m_PlayerName, Player[nMoi].m_PlayerName, pB->nTk, pB->nBanSap,\n'
 '\t\t\t       pB->nBangPha, (int)(pB->nDtPha != DTB_NGHI),\n'
 '\t\t\t       (int)(Npc[nNpcB].m_Doing == do_death || Npc[nNpcB].m_Doing == do_revive));\n'
 '\t\t}\n'
 '\t\treturn 0;\n'
 '\t}\n')

ap("H3b khong ep SetCanTeamFlag",
 '\tPlayer[nBot].m_cTeam.SetCanTeamFlag(nBot, TRUE);\n'
 '\tta.m_nInviteList[ta.m_nListPos] = nBot;\n',
 '\tif (!Player[nBot].m_cTeam.GetCanTeamFlag())\n'
 '\t\treturn 0;                          // [b] map cam to doi (SetCreateTeam(0)) -> de duong cu bao\n'
 '\tta.m_nInviteList[ta.m_nListPos] = nBot;\n')

ap("H3c luu subworld nguoi moi",
 '\tpB->nNhomNguoiIdx = nMoi;\n'
 '\tpB->dwNhomNguoiID = Player[nMoi].m_dwID;\n',
 '\tpB->nNhomNguoiIdx = nMoi;\n'
 '\tpB->dwNhomNguoiID = Player[nMoi].m_dwID;\n'
 '\t{\n'
 '\t\tconst int nNpcM = Player[nMoi].m_nIndex;\n'
 '\t\tpB->nNhomNguoiSub = (nNpcM > 0 && nNpcM < MAX_NPC) ? Npc[nNpcM].m_SubWorldIndex : -1;\n'
 '\t}\n')

ap("H4a so map nguoi moi voi luc vao nhom",
 '\t\t\tif (nNpcM > 0 && nNpcM < MAX_NPC && Npc[nNpcM].m_SubWorldIndex != nSub)\n',
 '\t\t\t// [b] so voi subworld cua NGUOI MOI luc vao nhom - chinh bot hoi sinh/di map khac\n'
 '\t\t\t// khong tinh la "nguoi choi doi map"\n'
 '\t\t\tif (nNpcM > 0 && nNpcM < MAX_NPC && b.nNhomNguoiSub >= 0\n'
 '\t\t\t && Npc[nNpcM].m_SubWorldIndex != b.nNhomNguoiSub)\n')

ap("H4b xoa nNhomNguoiSub khi thoi canh",
 '\t\tif (bXoa)\n'
 '\t\t{\n'
 '\t\t\tb.nNhomNguoiIdx = 0;  b.dwNhomNguoiID = 0;\n'
 '\t\t}\n',
 '\t\tif (bXoa)\n'
 '\t\t{\n'
 '\t\t\tb.nNhomNguoiIdx = 0;  b.dwNhomNguoiID = 0;  b.nNhomNguoiSub = -1;\n'
 '\t\t}\n')

ap("H5 pb_TkDuTuCach mien bot trong nhom nguoi that",
 '\tif (b.nBanSap)                                    return 0;  // chu game: bot ban sap KHONG bi goi\n',
 '\tif (b.nBanSap)                                    return 0;  // chu game: bot ban sap KHONG bi goi\n'
 '\tif (b.nNhomNguoiIdx)                              return 0;  // [BOTNHOM-NGUOI b] dang trong nhom nguoi that: khong keo di TK\n')

ap("H6a tu khoa ru bang chat hon",
 '\tif (pB && strstr(szLow, "bang"))\n'
 '\t{\n'
 '\t\tbBangHoi = 1;\n',
 '\t// [b] phan bien: "bang" tran voi nguoi go khong dau (cap bang nhau, bang gia, bang chu la\n'
 '\t// ai) -> phai kem mot tu y "ru vao" (vao / vo bang / gia nh / xin / moi / ru / join) hoac\n'
 '\t// chinh cum "bang h(oi)".\n'
 '\tif (pB && strstr(szLow, "bang")\n'
 '\t && (strstr(szLow, "vao") || strstr(szLow, "vo bang") || strstr(szLow, "gia nh")\n'
 '\t  || strstr(szLow, "xin") || strstr(szLow, "moi") || strstr(szLow, "ru ")\n'
 '\t  || strstr(szLow, "join") || strstr(szLow, "bang h")))\n'
 '\t{\n'
 '\t\tbBangHoi = 1;\n')

ap("H6b tu choi khi ban sap / giua Da Tau",
 '\t\telse if (pB->nBangPha)\n'
 '\t\t\tsprintf(szTraLoi, "Minh dang tren duong ve NPC mon phai xin vao bang roi, cho chut.");\n'
 '\t\telse\n'
 '\t\t{\n',
 '\t\telse if (pB->nBangPha)\n'
 '\t\t\tsprintf(szTraLoi, "Minh dang tren duong ve NPC mon phai xin vao bang roi, cho chut.");\n'
 '\t\telse if (pB->nBanSap)\n'
 '\t\t\tsprintf(szTraLoi, "Minh dang ngoi ban sap, de khi khac minh xin vao bang nhe.");\n'
 '\t\telse if (pB->nDtPha != DTB_NGHI)\n'
 '\t\t\tsprintf(szTraLoi, "Minh dang lam nhiem vu Da Tau, xong viec minh xin vao bang sau nhe.");\n'
 '\t\telse\n'
 '\t\t{\n')

ap("H6c bo cau dang cho de cau bang duoc gui",
 '\t\tpB->nPmCamToi = 0;                 // cau nay quan trong - khong "lam ngo" theo han cam\n',
 '\t\tpB->nPmCamToi = 0;                 // cau nay quan trong - khong "lam ngo" theo han cam\n'
 '\t\tpB->nPmDenHan = 0;                 // [b] bo ca cau dang cho, uu tien cau nay\n')

ap("H7 ChangeWorld so le theo chi so bot",
 '\t\tif (b.nBangDoiMapTick && now - b.nBangDoiMapTick < (unsigned int)(GAME_FPS * 3))\n',
 '\t\tif (b.nBangDoiMapTick && now - b.nBangDoiMapTick < (unsigned int)(GAME_FPS * (3 + (nLech % 3))))\n')

ap("H8 bao nguoi choi truoc khi roi nhom di xin bang",
 '\t\tif (pb_TrongNhom(nIdx))\n'
 '\t\t\tpb_RoiNhom(nIdx, "ve NPC mon phai xin vao bang");\n'
 '\t\tb.nNhomNguoiIdx = 0;  b.dwNhomNguoiID = 0;\n',
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
 '\t\tb.nNhomNguoiIdx = 0;  b.dwNhomNguoiID = 0;  b.nNhomNguoiSub = -1;\n')

io.open(P, "w", encoding="latin-1", newline="").write(s)
sau = sum(1 for c in s if ord(c) > 127)
print("Xong: %d hunk moi. high-byte %d -> %d (%s)" % (n, truoc, sau, "OK" if truoc == sau else "LECH!"))
