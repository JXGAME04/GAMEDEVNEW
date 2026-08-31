# -*- coding: utf-8 -*-
# [DM20 30/08] Chu game: "KHONG duoc thay doi skills.txt; phai VIET: cap 20 khong co
# skill danh duoc thi lay skill danh duoc cap thap hon de danh".
# Boi canh: chieu som nhat cua Duong Mon rq=30 (47/50/54), chu luc rq=80 -> bot DM
# cap < 30 khong co ung vien nao qua loc CAP trong pb_PickSkill; RemoveAllSkill luc
# tao bot da xoa ca DON DANH THUONG thua ke tu mau, hockynang chi day chieu phai
# -> nBest = 0 -> b.nAtkSkill = 0 -> pb_Fight khong danh gi ca = "khong danh duoc npc".
# Va: cuoi pb_PickSkill, nBest == 0 -> hoc (neu chua co) va dung DON DANH THUONG
# id 1 ("Cong kich vat ly": rq=0, eqt=-2 moi vu khi, series=-1 khong vuong loc he,
# da kiem skills.txt) - y het nguoi choi thap cap danh don trang cho toi khi du cap
# chieu phai (tu cap 30 pick lai chon chieu DM vi gate doi-cap reset moi lan len cap).
# Chi KPlayerBot.cpp (server-only). AP SAU goi_va_botcast_repick.py.
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

ap("H1 fallback don danh thuong",
 '\t// 1000 bot: chi in nhat ky chon chieu khi THAT BAI (nBest = 0) - truong hop\n'
 '\t// thanh cong da duoc kiem chung xong 18/08 sang, in nua la ngap log.\n'
 '\tif (nBest == 0)\n',
 '\t// [DM20 30/08] (chu game: "KHONG doi skills.txt; cap thap khong co skill phai\n'
 '\t// thi lay skill danh duoc cap thap hon"). Duong Mon: chieu som nhat rq=30 ->\n'
 '\t// bot cap < 30 khong co ung vien nao qua loc CAP; RemoveAllSkill luc tao bot\n'
 '\t// da xoa ca don danh thuong cua mau, hockynang chi day chieu phai -> nBest=0\n'
 '\t// -> bot dung nhin. Day lai DON DANH THUONG id 1 (rq=0, eqt=-2 moi vu khi,\n'
 '\t// series=-1 khong vuong loc he) roi dung no danh tam - y het nguoi choi thap\n'
 '\t// cap danh don trang; du cap chieu phai thi gate doi-cap tu pick lai chieu xin.\n'
 '\tif (nBest == 0)\n'
 '\t{\n'
 '\t\tint nCoCb = 0;\n'
 '\t\tfor (int q9 = 1; q9 < MAX_NPCSKILL; q9++)\n'
 '\t\t\tif (sl.m_Skills[q9].SkillId == 1)\n'
 '\t\t\t{\n'
 '\t\t\t\tnCoCb = 1;\n'
 '\t\t\t\tbreak;\n'
 '\t\t\t}\n'
 '\t\tif (!nCoCb)\n'
 '\t\t\tsl.Add(1, 1);\n'
 '\t\tif (g_SkillManager.GetSkill(1, 1))\n'
 '\t\t{\n'
 '\t\t\tnBest   = 1;\n'
 '\t\t\tnBestLv = 1;\n'
 '\t\t\tpb_Log("[BotChon] %s (he %d, vu khi %d) khong co chieu phai du cap"\n'
 '\t\t\t       " -> dung DON DANH THUONG (id 1)\\n",\n'
 '\t\t\t       Player[nIdx].m_PlayerName, (int)Npc[nNpcIdx].m_Series, nWant);\n'
 '\t\t}\n'
 '\t}\n'
 '\t// 1000 bot: chi in nhat ky chon chieu khi THAT BAI (nBest = 0) - truong hop\n'
 '\t// thanh cong da duoc kiem chung xong 18/08 sang, in nua la ngap log.\n'
 '\tif (nBest == 0)\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
