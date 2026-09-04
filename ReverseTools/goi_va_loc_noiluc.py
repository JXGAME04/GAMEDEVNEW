# -*- coding: utf-8 -*-
# [MANA 31/08] Tang loi THU HAI cua "bot Duong Mon di toi quai roi dung yen"
# (workflow 5 huong + phan bien doi khang tim ra, do tu jx_auto_server.log):
#   - Chieu 303 o CAP KY NANG 20 ton 60 noi luc (tangmen.lua duci_gu:
#     skill_cost_v={{1,20},{20,60}}), ma NHAN VAT CAP 20 co TRAN noi luc chi 45-57.
#     Do that: 1.963/1.963 mau [E4_SKILL_COST] skill=303 deu cost=60 va mana DAY
#     nhung < 60 => KHONG MOT LAN NAO du.
#   - KNpc.cpp:2604-2606 `case 1: ... if (!IsPlayer() || Cost(...))` - Cost tra FALSE
#     thi CA than lenh bi bo, dong dieu khien roi thang xuong nhan `Exit:` (2712)
#     -> `DoStand()` = DUNG YEN, KHONG mot dong bao loi (nhanh bao "het noi luc"
#     nam trong #ifndef _SERVER). Dem: 5.061/5.079 dong [E4_SKILL_ABORT] la 303.
#   - CanCastSkill (KSkills.cpp:195-397) TUYET DOI khong kiem cost => bo tham do
#     cua bot cung khong bat duoc.
# => Bot chon phai chieu VUOT TRAN noi luc la te liet vinh vien, im lang. Day la
#    BAY CHUNG cho moi phai/moi cap, khong rieng Duong Mon.
# Va: pb_PickSkill loai ung vien co chi phi VUOT TRAN tai nguyen cua bot
#     (mana/stamina/life theo GetSkillCostType). Nhan PB_DIAG "COST".
#     Chieu 45 Phich Lich Don ton 12 o moi cap -> luon qua duoc.
# Chi KPlayerBot.cpp. AP SAU goi_va_damageattrib_thua.py.
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

# ---- H1: loc chi phi trong pb_PickSkill (ngay sau gac du cap) ----
ap("H1 loc vuot tran noi luc",
 '\t\tif (Npc[nNpcIdx].m_Level < nRq)\n'
 '\t\t{ PB_DIAG(" %d:CAP=%d", id, nRq); continue; }\n',
 '\t\tif (Npc[nNpcIdx].m_Level < nRq)\n'
 '\t\t{ PB_DIAG(" %d:CAP=%d", id, nRq); continue; }\n'
 '\t\t// [MANA 31/08] Chi phi chieu VUOT TRAN tai nguyen cua bot = te liet IM LANG:\n'
 '\t\t// KNpc::DoSkill (KNpc.cpp:2604-2606) chay `if (!IsPlayer() || Cost(...))`, Cost\n'
 '\t\t// tra FALSE thi bo ca than lenh va roi thang xuong nhan Exit: -> DoStand()\n'
 '\t\t// = DUNG YEN, khong mot dong bao loi (nhanh bao het noi luc chi co ban client).\n'
 '\t\t// CanCastSkill KHONG kiem cost nen bo tham do cung khong bat duoc.\n'
 '\t\t// Do that 31/08: chieu 303 cap 20 ton 60 mana, nhan vat cap 20 tran chi 45-57\n'
 '\t\t// -> 1.963/1.963 mau deu thieu, 0 lan ra chieu, bot Duong Mon ket vinh vien.\n'
 '\t\t// Loai han ung vien khong bao gio tra noi (so voi TRAN, khong so voi hien tai:\n'
 '\t\t// hien tai con dao dong theo hoi mana / uong thuoc).\n'
 '\t\t{\n'
 '\t\t\tconst int nGia = p->GetSkillCost((void*)&Npc[nNpcIdx]);\n'
 '\t\t\tif (nGia > 0)\n'
 '\t\t\t{\n'
 '\t\t\t\tint nTran = -1;\n'
 '\t\t\t\tswitch (p->GetSkillCostType())\n'
 '\t\t\t\t{\n'
 '\t\t\t\tcase attrib_mana_v:     nTran = Npc[nNpcIdx].m_CurrentManaMax;    break;\n'
 '\t\t\t\tcase attrib_stamina_v:  nTran = Npc[nNpcIdx].m_CurrentStaminaMax; break;\n'
 '\t\t\t\tcase attrib_life_v:     nTran = Npc[nNpcIdx].m_CurrentLifeMax;    break;\n'
 '\t\t\t\tdefault: break;      // loai tai nguyen khac -> khong chan\n'
 '\t\t\t\t}\n'
 '\t\t\t\tif (nTran >= 0 && nGia > nTran)\n'
 '\t\t\t\t{ PB_DIAG(" %d:COST=%d>%d", id, nGia, nTran); continue; }\n'
 '\t\t\t}\n'
 '\t\t}\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
