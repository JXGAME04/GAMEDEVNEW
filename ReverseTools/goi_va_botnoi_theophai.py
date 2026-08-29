# -*- coding: utf-8 -*-
# [BotNoi-PHAI 28/08] Chu game: "fix vu khi lam LOI mot so phai TOAN ky nang ngoai
# cong can vu khi (Thien Vuong - Duong Mon)" - bot noi (dwID le) cua cac phai do bi
# tuoc vu khi -> khong con chieu nao dung duoc.
# Sua: duong NOI chi ap khi PHAI THAT SU co chieu danh tay khong mang don PHEP -
# quet danh sach chieu cua chinh bot bang DUNG bo loc pb_PickSkill (nWant=-1):
#   series khop he + radius>0 + !aura + !self + enemy + style Missles/Melee +
#   eqt in {-1,-2} + du cap (rq kep 80) + co 1 trong 5 don phep that.
# TV/DM (0 chieu phep) tu roi ve duong NGOAI giu vu khi - khong can bang liet ke tay,
# phai nao sau nay duoc them chieu phep thi tu chuyen theo du lieu.
# 3 diem quyet dinh vu khi doi tu pb_BotNoi -> pb_BotNoiThat (parity + co chieu noi);
# bThienNoi trong pb_PickSkill GIU parity (chi la uu tien xep hang, vo hai).
# AP SAU goi_va_botnoingoai.py. Chi KPlayerBot.cpp.
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

# ---- H1: them bo quet + wrapper sau pb_BotNoi ----
ap("H1 pb_CoChieuNoiTayKhong + pb_BotNoiThat",
 'static int pb_BotNoi(int nIdx)\n'
 '{\n'
 '\treturn (int)(Player[nIdx].m_dwID & 1);\n'
 '}\n',
 'static int pb_BotNoi(int nIdx)\n'
 '{\n'
 '\treturn (int)(Player[nIdx].m_dwID & 1);\n'
 '}\n'
 '\n'
 '// [BotNoi-PHAI 28/08] Chu game: "fix vu khi lam loi mot so phai TOAN ky nang ngoai\n'
 '// cong can vu khi (Thien Vuong - Duong Mon)". Duong NOI chi ap dung khi PHAI THAT SU\n'
 '// co chieu danh duoc TAY KHONG mang don sat thuong PHEP - quet danh sach chieu cua\n'
 '// chinh bot bang DUNG bo loc cua pb_PickSkill voi nWant = -1 (series khop he,\n'
 '// radius > 0, khong aura/self, nham dich, style Missles/Melee, eqt -1/-2, du cap).\n'
 '// TV/DM khong co chieu nao nhu vay -> tu roi ve duong NGOAI giu vu khi; phai khac\n'
 '// (Ngu Doc/Nga My/Thuy Yen/Vo Dang/Con Lon...) van chia doi nhu chu muon. Bot chua\n'
 '// du cap dung chieu phep thi TAM giu vu khi, len cap pb_TrangBiTheoCap se thao sau.\n'
 'static int pb_CoChieuNoiTayKhong(int nNpcIdx)\n'
 '{\n'
 '\tKSkillList& sl = Npc[nNpcIdx].m_SkillList;\n'
 '\tfor (int i = 1; i < MAX_NPCSKILL; i++)\n'
 '\t{\n'
 '\t\tconst int id = sl.m_Skills[i].SkillId;\n'
 '\t\tif (id <= 0 || id >= MAX_SKILL)\n'
 '\t\t\tcontinue;\n'
 '\t\tint lv = sl.m_Skills[i].CurrentSkillLevel;\n'
 '\t\tif (lv <= 0) lv = sl.m_Skills[i].SkillLevel;\n'
 '\t\tif (lv <= 0) lv = 1;\n'
 '\t\tKSkill* p = (KSkill*)g_SkillManager.GetSkill(id, lv);\n'
 '\t\tif (!p)\n'
 '\t\t\tcontinue;\n'
 '\t\t{\n'
 '\t\t\tconst int nSr = p->GetSkillSeries();\n'
 '\t\t\tif (nSr >= 0 && nSr < series_num && nSr != Npc[nNpcIdx].m_Series)\n'
 '\t\t\t\tcontinue;\n'
 '\t\t}\n'
 '\t\tif (p->GetAttackRadius() <= 0) continue;\n'
 '\t\tif (p->IsAura())               continue;\n'
 '\t\tif (p->IsTargetSelf())         continue;\n'
 '\t\tif (!p->IsTargetEnemy())       continue;\n'
 '\t\t{\n'
 '\t\t\tconst int nSt = p->GetSkillStyle();\n'
 '\t\t\tif (nSt != SKILL_SS_Missles && nSt != SKILL_SS_Melee)\n'
 '\t\t\t\tcontinue;\n'
 '\t\t}\n'
 '\t\tconst int eq = p->GetEquipLimit();\n'
 '\t\tif (eq != -2 && eq != -1)\n'
 '\t\t\tcontinue;              // can vu khi ho cu the -> tay khong khong dung duoc\n'
 '\t\tint nRq = p->GetSkillReqLevel();\n'
 '\t\tif (nRq > 80) nRq = 80;\n'
 '\t\tif (Npc[nNpcIdx].m_Level < nRq)\n'
 '\t\t\tcontinue;              // chua du cap - cast se bi tu choi im lang\n'
 '\t\tKMagicAttrib* pDa = p->GetDamageAttribs();\n'
 '\t\tconst int nDaNum = p->GetDamageAttribsNum();\n'
 '\t\tfor (int a = 0; a < nDaNum && a < MAX_MISSLE_DAMAGEATTRIB; a++)\n'
 '\t\t{\n'
 '\t\t\tconst int nT = pDa[a].nAttribType;\n'
 '\t\t\tif (nT == magic_magicdamage_v  || nT == magic_colddamage_v\n'
 '\t\t\t || nT == magic_firedamage_v   || nT == magic_lightingdamage_v\n'
 '\t\t\t || nT == magic_poisondamage_v)\n'
 '\t\t\t\treturn 1;\n'
 '\t\t}\n'
 '\t}\n'
 '\treturn 0;\n'
 '}\n'
 '\n'
 '// duong NOI THAT SU = dwID le VA phai co chieu noi tay khong dung duoc ngay bay gio\n'
 'static int pb_BotNoiThat(int nIdx)\n'
 '{\n'
 '\tif (!pb_BotNoi(nIdx))\n'
 '\t\treturn 0;\n'
 '\tconst int nNpcIdx = Player[nIdx].m_nIndex;\n'
 '\tif (nNpcIdx <= 0 || nNpcIdx >= MAX_NPC)\n'
 '\t\treturn 0;\n'
 '\treturn pb_CoChieuNoiTayKhong(nNpcIdx);\n'
 '}\n')

# ---- H2: pb_GiveFactionWeapon dung pb_BotNoiThat ----
ap("H2 give weapon theo phai",
 '\tif (pb_BotNoi(nIdx))\n'
 '\t{\n'
 '\t\tpb_Log("[BotNoi] %s phai %s: duong NOI CONG, khong nhan vu khi\\n",\n',
 '\tif (pb_BotNoiThat(nIdx))   // [BotNoi-PHAI] TV/DM khong co chieu noi -> van nhan vu khi\n'
 '\t{\n'
 '\t\tpb_Log("[BotNoi] %s phai %s: duong NOI CONG, khong nhan vu khi\\n",\n')

# ---- H3: khoi phat lai vu khi ----
ap("H3 regive theo phai",
 '\t\t\t && !pb_BotNoi(nIdx)   // [BotNoi] duong noi cong: tay khong la CO Y, khong phat lai\n',
 '\t\t\t && !pb_BotNoiThat(nIdx)   // [BotNoi-PHAI] chi bot noi THAT (phai co chieu noi) moi mien\n')

# ---- H4: pb_TrangBiTheoCap thao vu khi ----
ap("H4 trangbi strip theo phai",
 '\tif (pb_BotNoi(nIdx))\n'
 '\t{\n'
 '\t\tconst int nWn = Player[nIdx].m_ItemList.GetEquipment(itempart_weapon);\n',
 '\tif (pb_BotNoiThat(nIdx))   // [BotNoi-PHAI] TV/DM (khong chieu noi) giu vu khi, khong thao\n'
 '\t{\n'
 '\t\tconst int nWn = Player[nIdx].m_ItemList.GetEquipment(itempart_weapon);\n')

sau = sum(1 for c in s if ord(c) > 127)
if truoc != sau:
    print("LOI: high-byte doi %d -> %d" % (truoc, sau)); sys.exit(1)
if n:
    io.open(P, "w", encoding="latin-1", newline="").write(s)
print("Tong hunk ap: %d" % n)
