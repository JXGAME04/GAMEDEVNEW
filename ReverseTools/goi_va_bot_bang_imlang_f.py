# -*- coding: utf-8 -*-
# [BOTBANG-IMLANG 02/09, dot f] Chu game (nguyen van):
#   "va bot tu vao bang hoi cung vay cu am tham vao thoi ko can chat mat lai"
#
# => GO TOAN BO nhan mat lien quan bang hoi: ca 4 cau ket qua trong pb_XinVaoBang (huy / chua du
#    cap / xuat su khong an / ket qua nop don) LAN cau tra loi trong PB_WhisperReply. Bot nhan
#    duoc tu khoa thi LAM IM LANG, khong dap lai mot chu nao. Ly do chi di vao bot.log.
#
# Sau bo nay, duong bot -> nguoi choi con lai chi la:
#   * PB_WhisperReply / pb_XuLyPmCho: tra loi khi nguoi choi HOI (chao hoi, cap may, o dau...) -
#     tinh nang chu game giao 18/08, KHONG dong toi o day.
#   * pb_Chat: noi chuyen kenh/gan, khong phai nhan mat.
#
# Chi KPlayerBot.cpp. AP SAU: ... -> bot_bang_mau_d -> bot_phaidau_e -> (bo nay).
# Chay: python goi_va_bot_bang_imlang_f.py [--thu]
import io, sys
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KPlayerBot.cpp"
THU = ("--thu" in sys.argv)

s = io.open(P, "r", encoding="latin-1", newline="").read()
truoc = sum(1 for c in s if ord(c) > 127)
crlf = "\r\n" in s
n = 0
loi = 0

# Bo nay co nhieu hunk XOA (moi = "" hoac = mot doan von da co san) nen KHONG duoc dung
# "moi in s" lam dau hieu da-ap - se DUONG TINH GIA va bo qua het. Dung tham so `bo`:
# mot chuoi chac chan BIEN MAT sau khi ap (thuong la chinh cau nhan mat bi go).
def ap(ten, cu, moi, bo=None):
    global s, n, loi
    if crlf:
        cu = cu.replace("\n", "\r\n"); moi = moi.replace("\n", "\r\n")
    c = s.count(cu)
    if c == 1:
        if THU:
            print("  [ok] %s (neo khop 1)" % ten); return
        s = s.replace(cu, moi); n += 1
        print("  [+] %s" % ten); return
    if c == 0 and ((bo is not None and bo not in s) or (bo is None and moi and moi in s)):
        print("  [=] %s da ap tu truoc" % ten); return
    print("LOI: neo %s khop %d cho (can 1)" % (ten, c)); loi += 1
    if not THU:
        sys.exit(1)

# ------------------------------------------------ F1: bo PM khi HUY (het han / bang mat)
ap("F1 bo PM huy xin bang",
 '\t\tif (bNguoiCon && !bDaCoBang)\n'
 '\t\t{\n'
 '\t\t\tconst char* szHuy = "Minh khong toi duoc NPC mon phai, de khi khac nhe.";\n'
 '\t\t\tpb_GuiChatMat(nAi, nIdx, szHuy, (int)strlen(szHuy));\n'
 '\t\t}\n'
 '\t\tb.nBangPha = 0;',
 '\t\tb.nBangPha = 0;',
 "Minh khong toi duoc NPC mon phai")

# ------------------------------------------------ F2: bo PM khi chua du cap xuat su
ap("F2 bo PM chua du cap xuat su",
 '\t\t\tif (bNguoiCon)\n'
 '\t\t\t{\n'
 '\t\t\t\tconst char* szCap = "Minh chua du cap 60 de xuat su nen chua xin vao bang duoc, thong cam nhe.";\n'
 '\t\t\t\tpb_GuiChatMat(nAi, nIdx, szCap, (int)strlen(szCap));\n'
 '\t\t\t}\n',
 '',
 "Minh chua du cap 60 de xuat su")

# ------------------------------------------------ F3: bo PM khi xuat su khong an
ap("F3 bo PM xuat su khong an",
 '\t\t\tif (bNguoiCon)\n'
 '\t\t\t{\n'
 '\t\t\t\tconst char* szXs = "Minh xuat su khong duoc, de khi khac xin vao bang nhe.";\n'
 '\t\t\t\tpb_GuiChatMat(nAi, nIdx, szXs, (int)strlen(szXs));\n'
 '\t\t\t}\n',
 '',
 "Minh xuat su khong duoc")

# ------------------------------------------------ F4: bo PM bao ket qua nop don
ap("F4 bo PM ket qua nop don",
 '\tconst KTongJX2Tong* pT = g_TongJX2.FindTong(b.dwBangID);\n'
 '\tconst char* szTen = (pT && pT->szName[0]) ? pT->szName : "cua ban";\n'
 '\tchar szMsg[200];\n'
 '\tif (nKq == 7)\n'
 '\t\tsprintf(szMsg, "Minh da gia nhap bang %s roi, cam on ban!", szTen);\n'
 '\telse if (nKq == 0)\n'
 '\t\tsprintf(szMsg, "Minh da nop don xin vao bang %s, ban nho nhac bang chu duyet nhe.", szTen);\n'
 '\telse if (nKq == 12)\n'
 '\t\tsprintf(szMsg, "Minh da nop don vao bang %s tu truoc roi, dang cho duyet.", szTen);\n'
 '\telse if (nKq == 11)\n'
 '\t\tsprintf(szMsg, "Bang %s dat nguong cap cao hon cap cua minh, chua xin duoc.", szTen);\n'
 '\telse\n'
 '\t\tsprintf(szMsg, "Minh xin vao bang %s khong duoc (ma %d).", szTen, nKq);\n'
 '\tpb_Log("[BotBang] %s nop don vao bang %s (id %u) tai NPC %s: ket qua=%d\\n",\n'
 '\t       Player[nIdx].m_PlayerName, szTen, (unsigned int)b.dwBangID, s_facNpc[nFac].szTen, nKq);\n'
 '\tif (bNguoiCon)\n'
 '\t\tpb_GuiChatMat(nAi, nIdx, szMsg, (int)strlen(szMsg));\n',
 '\tconst KTongJX2Tong* pT = g_TongJX2.FindTong(b.dwBangID);\n'
 '\tconst char* szTen = (pT && pT->szName[0]) ? pT->szName : "cua ban";\n'
 '\t// [f 02/09] (chu game: "bot tu vao bang hoi cu am tham vao thoi, ko can chat mat lai")\n'
 '\t// KHONG nhan mat bao ket qua nua - chi ghi bot.log.\n'
 '\tpb_Log("[BotBang] %s nop don vao bang %s (id %u) tai NPC %s: ket qua=%d\\n",\n'
 '\t       Player[nIdx].m_PlayerName, szTen, (unsigned int)b.dwBangID, s_facNpc[nFac].szTen, nKq);\n',
 "Minh da gia nhap bang")

# ------------------------------------------------ F5: bo hai bien khong con dung
ap("F5 bo nAi/bNguoiCon (khong con noi dung)",
 '\tconst int nAi = b.nBangNguoiIdx;\n'
 '\tconst int bNguoiCon = (nAi > 0 && nAi < MAX_PLAYER && Player[nAi].m_dwID == b.dwBangNguoiID\n'
 '\t                    && Player[nAi].m_nIndex > 0);\n'
 '\tconst int bDaCoBang',
 '\tconst int bDaCoBang',
 "const int bNguoiCon")

# ------------------------------------------------ F6: PB_WhisperReply - nhanh bang hoi IM LANG
ap("F6 nhanh bang hoi khong tra loi",
 '\t\tif (Player[nBot].m_cTong.m_nFlag)\n'
 '\t\t\tsprintf(szTraLoi, "Minh co bang hoi roi, cam on ban nhe.");\n'
 '\t\telse if (!pTg)\n'
 '\t\t\tsprintf(szTraLoi, "Ban chua co bang hoi ma, ru minh vao dau?");\n'
 '\t\telse if (!bBangChu)\n'
 '\t\t\tsprintf(szTraLoi, "Ban khong phai bang chu bang %s, nho bang chu nhan cho minh nhe.", pTg->szName);\n'
 '\t\telse if (nCapBot < PB_CAP_VAOBANG)\n'
 '\t\t\tsprintf(szTraLoi, "Minh moi cap %d, phai tu cap %d tro len minh moi dam xuat su xin vao bang.",\n'
 '\t\t\t        nCapBot, PB_CAP_VAOBANG);\n'
 '\t\telse if (pB->nBangPha)\n'
 '\t\t\tsprintf(szTraLoi, "Minh dang tren duong ve NPC mon phai xin vao bang roi, cho chut.");\n'
 '\t\telse if (pB->nBanSap)\n'
 '\t\t\tsprintf(szTraLoi, "Minh dang ngoi ban sap, de khi khac minh xin vao bang nhe.");\n'
 '\t\telse\n'
 '\t\t{\n'
 '\t\t\tpB->nBangPha = 1;  pB->dwBangID = dwNg;\n'
 '\t\t\tpB->nBangNguoiIdx = p->nSenderIdx;  pB->dwBangNguoiID = Player[p->nSenderIdx].m_dwID;\n'
 '\t\t\tpB->nBangTick = 0;  pB->nBangThu = 0;  pB->nBangNghiToi = 0;  pB->nBangDoiMapTick = 0;\n'
 '\t\t\tsprintf(szTraLoi, "Ok! Minh ve Ba Lang Huyen gap NPC mon phai xuat su roi xin vao bang %s ngay day.", pTg->szName);\n',
 '\t\t// [f 02/09] (chu game: "bot tu vao bang hoi cu am tham vao thoi, ko can chat mat lai")\n'
 '\t\t// MOI nhanh o day deu IM LANG: nhan duoc thi lang le di lam, khong nhan duoc thi thoi.\n'
 '\t\t// Ly do tung truong hop chi di vao bot.log (gion 3 giay toan cuc).\n'
 '\t\tif (Player[nBot].m_cTong.m_nFlag || !pTg || !bBangChu || nCapBot < PB_CAP_VAOBANG\n'
 '\t\t || pB->nBangPha || pB->nBanSap)\n'
 '\t\t{\n'
 '\t\t\tstatic unsigned int s_uBoQuaLog = 0;\n'
 '\t\t\tconst unsigned int uNowBq = (unsigned int)GetTickCount();\n'
 '\t\t\tif (uNowBq - s_uBoQuaLog >= 3000)\n'
 '\t\t\t{\n'
 '\t\t\t\ts_uBoQuaLog = uNowBq;\n'
 '\t\t\t\tpb_Log("[BotBang] %s BO QUA loi ru vao bang cua %s (da co bang %d / nguoi ru khong co bang %d"\n'
 '\t\t\t\t       " / khong phai bang chu %d / cap %d < %d / dang di xin %d / ban sap %d)\\n",\n'
 '\t\t\t\t       Player[nBot].m_PlayerName, Player[p->nSenderIdx].m_PlayerName,\n'
 '\t\t\t\t       (int)(Player[nBot].m_cTong.m_nFlag != 0), (int)(pTg == NULL), (int)(!bBangChu),\n'
 '\t\t\t\t       nCapBot, PB_CAP_VAOBANG, pB->nBangPha, pB->nBanSap);\n'
 '\t\t\t}\n'
 '\t\t}\n'
 '\t\telse\n'
 '\t\t{\n'
 '\t\t\tpB->nBangPha = 1;  pB->dwBangID = dwNg;\n'
 '\t\t\tpB->nBangNguoiIdx = p->nSenderIdx;  pB->dwBangNguoiID = Player[p->nSenderIdx].m_dwID;\n'
 '\t\t\tpB->nBangTick = 0;  pB->nBangThu = 0;  pB->nBangNghiToi = 0;  pB->nBangDoiMapTick = 0;\n',
 "Ok! Minh ve Ba Lang Huyen")

# ------------------------------------------------ F7: khong soan cau tra loi, khong roi xuong cau chung
ap("F7 khong tra loi gi cho nhanh bang hoi",
 '\t\tpB->nPmCamToi = 0;                 // cau nay quan trong - khong "lam ngo" theo han cam\n'
 '\t\tpB->nPmDenHan = 0;                 // [b] bo ca cau dang cho, uu tien cau nay\n'
 '\t}\n',
 '\t\t// [f 02/09] IM LANG: chi vong lai tieng cua CHINH nguoi gui (de ho van thay dong minh vua\n'
 '\t\t// go trong cua so chat rieng) roi thoi - khong soan cau tra loi nao, va KHONG de roi\n'
 '\t\t// xuong khoi "cau chung" ben duoi.\n'
 '\t\tpb_GuiChatMatEcho(p->nSenderIdx, Player[nBot].m_PlayerName, p->szMsg, nLen, p->nPackageID);\n'
 '\t\treturn 1;\n'
 '\t}\n',
 "cau nay quan trong - khong \"lam ngo\" theo han cam")

if THU:
    print("KIEM NEO XONG: %d loi" % loi)
    sys.exit(1 if loi else 0)

io.open(P, "w", encoding="latin-1", newline="").write(s)
sau = sum(1 for c in s if ord(c) > 127)
print("Xong: %d hunk moi. high-byte truoc=%d sau=%d (%s)"
      % (n, truoc, sau, "OK" if truoc == sau else "LECH!"))
