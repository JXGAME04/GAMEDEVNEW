# -*- coding: utf-8 -*-
# [BotNoi 28/08] Chu game: "bot dang 100% danh ky nang NGOAI CONG (vi ai cung duoc
# phat vu khi, len 81 con duoc nang vu khi cap 10) -> fix random ti le bot 2 duong
# noi - ngoai cho can bang; NOI CONG la KHONG can vu khi".
# Do that 27-28/08 bot.log: 15/16 chieu duoc dung co IsPhysical=1 (chi 303 Doc Thach
# Cot la phep) - dung nhu chu bao.
# Thiet ke: dwID le = duong NOI (tay khong; trung voi bThienNoi cu cua pb_PickSkill
# nen tu uu tien chieu phep), dwID chan = duong NGOAI (nhu cu). ~50/50, on dinh giua
# cac phien. 4 diem cham:
#   H1 pb_GiveFactionWeapon: bot noi -> thao vu khi cu roi return (di nhanh tay khong)
#   H2 khoi [BotVuKhi] phat-lai-vu-khi: bo qua bot noi (tay khong la CO Y)
#   H3 pb_TrangBiTheoCap: bot noi dang cam vu khi (di san cu) -> thao huy + chon lai
#      chieu; buoc "81 len vu khi cap 10" tu thanh no-op vi khong con vu khi
#   H4 pb_PickSkill: bThienNoi doc qua pb_BotNoi (mot nguon su that)
# Idempotent, latin-1, CRLF-aware. Chi KPlayerBot.cpp (server-only).
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

# ---- H0: helper mot nguon su that (dat sau prototype pb_ODat da chen dot BotTHP) ----
ap("H0 helper pb_BotNoi",
 'static int pb_ODat(int nSubIdx, int nOX, int nOY, int nLech, int nRMax,\n'
 '                   int* pnX, int* pnY);\n',
 'static int pb_ODat(int nSubIdx, int nOX, int nOY, int nLech, int nRMax,\n'
 '                   int* pnX, int* pnY);\n'
 '\n'
 '// [BotNoi 28/08] Chia dan 2 duong (chu game: "random ti le bot 2 duong noi - ngoai\n'
 '// cho can bang; NOI CONG la KHONG can vu khi"): dwID LE = duong NOI CONG (tay khong,\n'
 '// pb_PickSkill von uu tien chieu phep cho dung nhom dwID le - bThienNoi 23/08),\n'
 '// dwID CHAN = duong NGOAI CONG (cam vu khi nhu cu). dwID roledb cap tuan tu nen\n'
 '// ~50/50 va ON DINH giua cac phien - bot khong doi duong sau restart.\n'
 'static int pb_BotNoi(int nIdx)\n'
 '{\n'
 '\treturn (int)(Player[nIdx].m_dwID & 1);\n'
 '}\n')

# ---- H1: pb_GiveFactionWeapon - bot noi di nhanh tay khong ----
ap("H1 give weapon: bot noi tay khong",
 '\t\tPlayer[nIdx].m_ItemList.RemoveItemIdx(nOld, Item[nOld].GetStackNum());\n'
 '\t}\n'
 '\n'
 '\tif (w.d < 0)\n',
 '\t\tPlayer[nIdx].m_ItemList.RemoveItemIdx(nOld, Item[nOld].GetStackNum());\n'
 '\t}\n'
 '\n'
 '\t// [BotNoi 28/08] bot duong NOI CONG: co y tay khong (nhu nhanh duong quyen duoi) -\n'
 '\t// vu khi cu (ke ca cua nhan vat mau) da duoc thao o tren.\n'
 '\tif (pb_BotNoi(nIdx))\n'
 '\t{\n'
 '\t\tpb_Log("[BotNoi] %s phai %s: duong NOI CONG, khong nhan vu khi\\n",\n'
 '\t\t       Player[nIdx].m_PlayerName, s_facNpc[nFaction].szTen);\n'
 '\t\treturn;\n'
 '\t}\n'
 '\n'
 '\tif (w.d < 0)\n')

# ---- H2: khoi phat lai vu khi - bo qua bot noi ----
ap("H2 regive: bo qua bot noi",
 '\t\t\tif (b.nGaveWeapon && b.nFaction >= 0\n'
 '\t\t\t && Player[nIdx].m_ItemList.GetWeaponType() < 0\n',
 '\t\t\tif (b.nGaveWeapon && b.nFaction >= 0\n'
 '\t\t\t && !pb_BotNoi(nIdx)   // [BotNoi] duong noi cong: tay khong la CO Y, khong phat lai\n'
 '\t\t\t && Player[nIdx].m_ItemList.GetWeaponType() < 0\n')

# ---- H3: pb_TrangBiTheoCap - thao vu khi di san cua bot noi ----
ap("H3 trangbi: thao vu khi bot noi",
 '\t// ---- 4. dat cap 81: doi vu khi len CAP 10 cung loai dang cam ----\n',
 '\t// ---- 3b. [BotNoi 28/08] bot duong NOI CONG khong cam vu khi: bot cu con cam\n'
 '\t// (di san truoc khi chia duong, hoac blob mau) thi thao huy mot lan tai day\n'
 '\t// (ham nay chay ca luc login vi gate nTrangBiLevel=0 != cap) roi chon lai chieu\n'
 '\t// -> nWant=-1, bThienNoi (cung nhom dwID le) tu uu tien chieu phep. ----\n'
 '\tif (pb_BotNoi(nIdx))\n'
 '\t{\n'
 '\t\tconst int nWn = Player[nIdx].m_ItemList.GetEquipment(itempart_weapon);\n'
 '\t\tif (nWn > 0)\n'
 '\t\t{\n'
 '\t\t\tPlayer[nIdx].m_ItemList.RemoveItemIdx(nWn, Item[nWn].GetStackNum());\n'
 '\t\t\tb.nAtkSkill = 0;\n'
 '\t\t\tpb_Log("[BotNoi] %s cap %d: thao vu khi (duong NOI CONG danh tay khong)\\n",\n'
 '\t\t\t       Player[nIdx].m_PlayerName, nLevel);\n'
 '\t\t}\n'
 '\t}\n'
 '\n'
 '\t// ---- 4. dat cap 81: doi vu khi len CAP 10 cung loai dang cam ----\n')

# ---- H4: bThienNoi doc qua helper ----
ap("H4 bThienNoi = pb_BotNoi",
 '\tconst int bThienNoi = (int)(Player[nIdx].m_dwID & 1);\n',
 '\tconst int bThienNoi = pb_BotNoi(nIdx);   // [BotNoi 28/08] mot nguon su that voi duong tay-khong\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
