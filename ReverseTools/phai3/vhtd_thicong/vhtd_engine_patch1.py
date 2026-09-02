# -*- coding: ascii -*-
"""vhtd_engine_patch1.py - VU HON (11) / TIEU DAO (12) engine dot 1 (02/09), marker [VHTD 02/09].
Nguon: client VLTK Level Up (skills.txt + wuhuntang.lua/xiaoyao.lua/advancedskill.lua) - Linux KHONG co 2 phai nay; cac thuoc tinh
Linux co (autocastskill 0x08097420, invincibility 0x080961B0, forbit_attack 0x08096150, melee_returnres_p 0x08096390,
anti_lightingres_p 0x08096B50) lam theo asm; phan con lai thiet ke theo mo ta client VLTK (skill_desc + magicdesc.ini).
  1. SkillDef.h  MAX_MISSLESTYLE 450 -> 700 (dan VLTK toi 649)
  2. KMagicAttrib.h / KMagicDesc.cpp: +16 ten thuoc tinh NOI DUOI (310..325), magic_normal_end -> 326
  3. KNpcAttribModify: handler moi + dang ky
  4. KNpc.h/.cpp: truong moi, reset, tick tu phong theo chu ky, bat tu, cam danh, khoa mau, hoi mau, dem tang (No / Am Luat),
     cast khi buff het, dat lai buff, Loi % (lightingdamage_p / addlightingmagic_p), khang phan don can chien
  5. KSkills.h/.cpp: cost_sp (dieu kien tang), lightingdamage_p vao o damage[12], reset_bufftime = immediate
  6. KPlayer.cpp: reset truong moi trong UpdataCurData
  7. S3Client UiSkillsNew.cpp: bang o ky nang phai 11/12 + nut dong
  8. KNpcAttribModify AddPhysicsDamageP: ma 10/11/12 -> o vu khi 6/7/8 (VLTK: 11 = Dao Thuan, 12 = Thuan Dao); MAX_MELEE_WEAPON 6 -> 9
Doc/ghi latin-1, giu CRLF. Idempotent. DUNG: python vhtd_engine_patch1.py [--kiem]
"""
import io, os, sys
ROOT = r"D:\GAMEDEVNEW\Sources"
KIEM = "--kiem" in sys.argv
MARK = "[VHTD 02/09]"
hib = lambda t: sum(1 for c in t if ord(c) >= 0x80)

class F:
    def __init__(self, rel):
        self.p = os.path.join(ROOT, rel); self.name = rel
        self.s = io.open(self.p, "r", encoding="latin-1", newline="").read(); self.orig = self.s; self.n = 0
    def rep(self, old, new, tag, count=1):
        c = self.s.count(old)
        if c == 0:
            if new in self.s: print("  [=] %s: da ap (%s)" % (self.name, tag)); return
            raise SystemExit("KHONG THAY anchor %s (%s):\n%s" % (self.name, tag, old[:200]))
        if c != count: raise SystemExit("anchor khong duy nhat (%d) %s (%s)" % (c, self.name, tag))
        self.s = self.s.replace(old, new); self.n += 1; print("  [+] %s: %s" % (self.name, tag))
    def rep_first_after(self, fn, old, new, tag):
        i0 = self.s.find(fn)
        if i0 < 0: raise SystemExit("khong thay %s trong %s" % (fn, self.name))
        i = self.s.find(old, i0)
        if i < 0:
            if new in self.s[i0:]: print("  [=] %s: da ap (%s)" % (self.name, tag)); return
            raise SystemExit("KHONG THAY anchor sau %s trong %s (%s)" % (fn, self.name, tag))
        self.s = self.s[:i] + new + self.s[i + len(old):]; self.n += 1; print("  [+] %s: %s" % (self.name, tag))
    def save(self):
        if self.s == self.orig: print("  (khong doi) %s" % self.name); return
        if hib(self.s) != hib(self.orig) or "\xef\xbf\xbd" in self.s: raise SystemExit("LECH BYTE CAO / EF BF BD %s" % self.name)
        if not KIEM: io.open(self.p, "w", encoding="latin-1", newline="").write(self.s)
        print("  => ghi %s (%d cho)%s" % (self.name, self.n, " KIEM" if KIEM else ""))

NEW_ATTR = [  # (ten, ghi chu)
    ("autocastskill",          "310 VLTK/Linux 0x08097420: tu thi trien ky nang con theo chu ky {id*256+cap, 1=tinh CD/-1, khung*256+ty le}"),
    ("special_point_base",     "311 VLTK: bo dem tang (No 1976 / Am Luat 2116) {id khoa, -1 buff/0, tran tang}"),
    ("special_point_add",      "312 VLTK: cong tang vao bo dem {id khoa, 0, so tang}"),
    ("cost_sp",                "313 VLTK: dieu kien thi trien = du tang, thi trien thi tru {id khoa, 0, so tang} (xu ly o KSkill/KNpc::DoSkill)"),
    ("lock_life",              "314 VLTK: khoa sinh luc {gia tri, khung, che do 1 = khong thap hon}"),
    ("cast_when_buff_removed", "315 VLTK: buff het -> phong ky nang {id, cap (-1 = cap cua v2), id tham chieu}"),
    ("reset_bufftime",         "316 VLTK: dat lai thoi gian buff {id buff, -1, 0} tren muc tieu (immediate)"),
    ("resume_life_p",          "317 VLTK: hoi % sinh luc toi da {%, khung}"),
    ("lifereplenish_dec_p",    "318 VLTK: giam hieu suat hoi sinh luc {%, khung}"),
    ("unravel_effect",         "319 VLTK: Ta luc {diem} - CHUA RO CO CHE, chi luu + mo ta"),
    ("lightingdamage_p",       "320 VLTK: sat thuong Loi % (theo noi cong Loi cua nguoi phat) - o damage[12]"),
    ("addlightingmagic_p",     "321 VLTK: noi cong Loi %"),
    ("hidebodyunlock",         "322 VLTK: an than {1, khung}"),
    ("invincibility",          "323 VLTK/Linux 0x080961B0: bat tu {1, khung}"),
    ("forbit_attack",          "324 VLTK/Linux 0x08096150: cam cong kich {1, khung}"),
    ("melee_returnres_p",      "325 Linux 0x08096390 [0x1264]: khang phan don can chien % (ten da co enum tu truoc? kiem)"),
]

# ------------------------------------------------------------------------------------------------ 1
d = F(r"Core\Src\SkillDef.h")
d.rep("#define MAX_MISSLESTYLE\t\t\t\t\t\t450\r\n", "#define MAX_MISSLESTYLE\t\t\t\t\t\t700\t// " + MARK + " dan VLTK Vu Hon/Tieu Dao toi 649 (missles.txt)\r\n", "MAX_MISSLESTYLE 700")
d.save()

# ------------------------------------------------------------------------------------------------ 2
h = F(r"Core\Src\KMagicAttrib.h")
has_mrr = "\tmagic_melee_returnres_p," in h.s
lines = []
idx = 310
for nm, note in NEW_ATTR:
    if nm == "melee_returnres_p" and has_mrr: continue
    lines.append("\tmagic_%s,%s//%d\t%s %s\r\n" % (nm, "\t" * max(1, 6 - (len(nm) + 7) // 4), idx, MARK, note)); idx += 1
h.rep("\tmagic_anti_sorbdamage_p,\t\t\t\t//309\t[KM 01/09] ban KHONG-DUONG cua 269 anti_sorbdamage_yan_p (cung handler, don vi phan nghin)\r\n\tmagic_normal_end,\t\t\t\t\t\t//310\r\n",
      "\tmagic_anti_sorbdamage_p,\t\t\t\t//309\t[KM 01/09] ban KHONG-DUONG cua 269 anti_sorbdamage_yan_p (cung handler, don vi phan nghin)\r\n" + "".join(lines) + "\tmagic_normal_end,\t\t\t\t\t\t//%d\t%s\r\n" % (idx, MARK),
      "enum +%d" % len(lines))
h.save()
NAMES_ADDED = [nm for nm, _ in NEW_ATTR if not (nm == "melee_returnres_p" and has_mrr)]

m = F(r"Core\Src\KMagicDesc.cpp")
slines = "".join('\t"%s",%s//%d %s\r\n' % (nm, "\t" * max(1, 7 - (len(nm) + 3) // 4), 310 + i, MARK) for i, nm in enumerate(NAMES_ADDED))
m.rep('\t"anti_sorbdamage_p",\t\t\t\t//309  [KM 01/09]\r\n\t"normal_end",\t\t\t\t\t\t//310\r\n',
      '\t"anti_sorbdamage_p",\t\t\t\t//309  [KM 01/09]\r\n' + slines + '\t"normal_end",\t\t\t\t\t\t//%d %s\r\n' % (310 + len(NAMES_ADDED), MARK), "ten thuoc tinh")
m.save()

# ------------------------------------------------------------------------------------------------ 3 KNpcAttribModify
ah = F(r"Core\Src\KNpcAttribModify.h")
ah.rep("\tvoid\tAutoRescueSkill(KNpc* pNpc, void* pData);\r\n",
       "\tvoid\tAutoRescueSkill(KNpc* pNpc, void* pData);\r\n"
       "\tvoid\tAutoCastSkill(KNpc* pNpc, void* pData);\t\t// " + MARK + "\r\n"
       "\tvoid\tSpecialPointBase(KNpc* pNpc, void* pData);\t// " + MARK + "\r\n"
       "\tvoid\tSpecialPointAdd(KNpc* pNpc, void* pData);\t// " + MARK + "\r\n"
       "\tvoid\tLockLife(KNpc* pNpc, void* pData);\t\t\t// " + MARK + "\r\n"
       "\tvoid\tResumeLifeP(KNpc* pNpc, void* pData);\t\t// " + MARK + "\r\n"
       "\tvoid\tLifeReplenishDecP(KNpc* pNpc, void* pData);\t// " + MARK + "\r\n"
       "\tvoid\tUnravelEffect(KNpc* pNpc, void* pData);\t\t// " + MARK + "\r\n"
       "\tvoid\tAddLightingMagicP(KNpc* pNpc, void* pData);\t// " + MARK + "\r\n"
       "\tvoid\tHideBodyUnlock(KNpc* pNpc, void* pData);\t// " + MARK + "\r\n"
       "\tvoid\tInvincibility(KNpc* pNpc, void* pData);\t\t// " + MARK + "\r\n"
       "\tvoid\tForbitAttack(KNpc* pNpc, void* pData);\t\t// " + MARK + "\r\n"
       "\tvoid\tMeleeReturnResP(KNpc* pNpc, void* pData);\t// " + MARK + "\r\n", "decl handler moi")
ah.save()

ac = F(r"Core\Src\KNpcAttribModify.cpp")
ac.rep("\tProcessFunc[magic_anti_lightingres_yan_p] = &KNpcAttribModify::AntiLightingResYanP;\t// [PF 31/08k]\r\n",
       "\tProcessFunc[magic_anti_lightingres_yan_p] = &KNpcAttribModify::AntiLightingResYanP;\t// [PF 31/08k]\r\n"
       "\t// " + MARK + " Vu Hon / Tieu Dao (client VLTK)\r\n"
       "\tProcessFunc[magic_anti_lightingres_p] = &KNpcAttribModify::AntiLightingResYanP;\t// chu game bo (Duong): cung handler (Linux 0x08096B50 cung += vao [0x12d8])\r\n"
       "\tProcessFunc[magic_autocastskill] = &KNpcAttribModify::AutoCastSkill;\r\n"
       "\tProcessFunc[magic_special_point_base] = &KNpcAttribModify::SpecialPointBase;\r\n"
       "\tProcessFunc[magic_special_point_add] = &KNpcAttribModify::SpecialPointAdd;\r\n"
       "\tProcessFunc[magic_lock_life] = &KNpcAttribModify::LockLife;\r\n"
       "\tProcessFunc[magic_resume_life_p] = &KNpcAttribModify::ResumeLifeP;\r\n"
       "\tProcessFunc[magic_lifereplenish_dec_p] = &KNpcAttribModify::LifeReplenishDecP;\r\n"
       "\tProcessFunc[magic_unravel_effect] = &KNpcAttribModify::UnravelEffect;\r\n"
       "\tProcessFunc[magic_addlightingmagic_p] = &KNpcAttribModify::AddLightingMagicP;\r\n"
       "\tProcessFunc[magic_hidebodyunlock] = &KNpcAttribModify::HideBodyUnlock;\r\n"
       "\tProcessFunc[magic_invincibility] = &KNpcAttribModify::Invincibility;\r\n"
       "\tProcessFunc[magic_forbit_attack] = &KNpcAttribModify::ForbitAttack;\r\n"
       "\tProcessFunc[magic_melee_returnres_p] = &KNpcAttribModify::MeleeReturnResP;\r\n", "dang ky handler moi")
HANDLERS = (
"// " + MARK + " ==================== VU HON / TIEU DAO (client VLTK) ====================\r\n"
"void KNpcAttribModify::AutoCastSkill( KNpc* pNpc, void* pData )\t// Linux 0x08097420: danh sach +0x182c, Fire moi khung voi ke danh -1 (chi nham minh)\r\n"
"{\r\n"
"\tHS_AutoSkillModify(pNpc->m_CastSkill, pNpc, (KMagicAttrib *)pData);\r\n"
"}\r\n"
"\r\n"
"// bo dem tang (No / Am Luat): v0 = id ky nang khoa, v2 = tran. Ap qua trang thai thu dong (v1 = -1) -> ap lai moi lan tinh lai thuoc tinh;\r\n"
"// so tang hien co GIU (khong reset theo UpdataCurData), chi tran duoc dat lai. Gia tri dao dau (go trang thai) -> bo qua.\r\n"
"void KNpcAttribModify::SpecialPointBase( KNpc* pNpc, void* pData )\r\n"
"{\r\n"
"\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\r\n"
"\tif (pMagic->nValue[0] <= 0)\r\n"
"\t\treturn;\r\n"
"\tint nFree = -1;\r\n"
"\tfor (int i = 0; i < MAX_HS_SP; i++)\r\n"
"\t{\r\n"
"\t\tif (pNpc->m_HSSp[i].nKey == pMagic->nValue[0])\r\n"
"\t\t{\r\n"
"\t\t\tpNpc->m_HSSp[i].nMax = pMagic->nValue[2];\r\n"
"\t\t\tif (pNpc->m_HSSp[i].nCount > pNpc->m_HSSp[i].nMax) pNpc->m_HSSp[i].nCount = pNpc->m_HSSp[i].nMax;\r\n"
"\t\t\treturn;\r\n"
"\t\t}\r\n"
"\t\tif (nFree < 0 && pNpc->m_HSSp[i].nKey == 0) nFree = i;\r\n"
"\t}\r\n"
"\tif (nFree >= 0)\r\n"
"\t{\r\n"
"\t\tpNpc->m_HSSp[nFree].nKey = pMagic->nValue[0];\r\n"
"\t\tpNpc->m_HSSp[nFree].nMax = pMagic->nValue[2];\r\n"
"\t\tpNpc->m_HSSp[nFree].nCount = 0;\r\n"
"\t}\r\n"
"}\r\n"
"\r\n"
"void KNpcAttribModify::SpecialPointAdd( KNpc* pNpc, void* pData )\t// v0 = id khoa, v2 = so tang cong (byte thap; 1990 cu dung max*256+add)\r\n"
"{\r\n"
"\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\r\n"
"\tif (pMagic->nValue[0] <= 0 || pMagic->nValue[2] == 0)\r\n"
"\t\treturn;\r\n"
"\tpNpc->HS_SpAdd(pMagic->nValue[0], pMagic->nValue[2] & 0xff);\r\n"
"}\r\n"
"\r\n"
"void KNpcAttribModify::LockLife( KNpc* pNpc, void* pData )\t// {gia tri khoa, khung, che do}: 1 = mau khong thap hon gia tri (Vu Muc Di Thu 1982)\r\n"
"{\r\n"
"\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\r\n"
"\tif (pMagic->nValue[0] > 0)\r\n"
"\t{\r\n"
"\t\tpNpc->m_nHSLockLife = pMagic->nValue[0];\r\n"
"\t\tpNpc->m_nHSLockLifeMode = pMagic->nValue[2];\r\n"
"\t}\r\n"
"\telse\r\n"
"\t{\r\n"
"\t\tpNpc->m_nHSLockLife = 0;\r\n"
"\t\tpNpc->m_nHSLockLifeMode = 0;\r\n"
"\t}\r\n"
"}\r\n"
"\r\n"
"void KNpcAttribModify::ResumeLifeP( KNpc* pNpc, void* pData )\t// {%, khung=1}: hoi ngay % sinh luc toi da khi ap (Trung Vu Luu Phong BUFF 1991); go -> bo qua\r\n"
"{\r\n"
"\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\r\n"
"\tif (pMagic->nValue[0] <= 0 || pNpc->m_CurrentLifeMax <= 0)\r\n"
"\t\treturn;\r\n"
"\tif (pNpc->m_Doing == do_death || pNpc->m_Doing == do_revive)\r\n"
"\t\treturn;\r\n"
"\tint nAdd = (int)((__int64)pNpc->m_CurrentLifeMax * pMagic->nValue[0] / MAX_PERCENT);\r\n"
"\tpNpc->m_CurrentLife += nAdd;\r\n"
"\tif (pNpc->m_CurrentLife > pNpc->m_CurrentLifeMax) pNpc->m_CurrentLife = pNpc->m_CurrentLifeMax;\r\n"
"}\r\n"
"\r\n"
"void KNpcAttribModify::LifeReplenishDecP( KNpc* pNpc, void* pData )\t// giam hieu suat hoi sinh luc % (debuff Ham Son Kich 1988)\r\n"
"{\r\n"
"\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\r\n"
"\tpNpc->m_CurrentLifeReplenishPercent -= pMagic->nValue[0];\r\n"
"}\r\n"
"\r\n"
"void KNpcAttribModify::UnravelEffect( KNpc* pNpc, void* pData )\t// 'Ta luc' (Hiep Cot Nhu Tinh 2122) - co che chua ro, chi luu de mo ta\r\n"
"{\r\n"
"\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\r\n"
"\tpNpc->m_nHSUnravel += pMagic->nValue[0];\r\n"
"}\r\n"
"\r\n"
"void KNpcAttribModify::AddLightingMagicP( KNpc* pNpc, void* pData )\t// noi cong Loi % (Tieu Dao Cam Phap 2137, Thien Ly Doc Hanh 2123)\r\n"
"{\r\n"
"\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\r\n"
"\tpNpc->m_nHSAddLightMagicP += pMagic->nValue[0];\r\n"
"}\r\n"
"\r\n"
"void KNpcAttribModify::HideBodyUnlock( KNpc* pNpc, void* pData )\t// {1, khung}: an than (Thap Bo Nhat Sat_Buff 2130) - dung m_HideState nhu magic_hide\r\n"
"{\r\n"
"\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\r\n"
"\tif (pMagic->nValue[0] == 1 && pMagic->nValue[1] > 0)\r\n"
"\t\tpNpc->m_HideState.nTime = pMagic->nValue[1];\r\n"
"}\r\n"
"\r\n"
"void KNpcAttribModify::Invincibility( KNpc* pNpc, void* pData )\t// Linux 0x080961B0: byte [0x147b] = (v0 == 1)\r\n"
"{\r\n"
"\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\r\n"
"\tpNpc->m_bHSInvincible = (pMagic->nValue[0] == 1) ? TRUE : FALSE;\r\n"
"}\r\n"
"\r\n"
"void KNpcAttribModify::ForbitAttack( KNpc* pNpc, void* pData )\t// Linux 0x08096150: byte [0x1478] = (v0 == 1)\r\n"
"{\r\n"
"\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\r\n"
"\tpNpc->m_bHSForbidAttack = (pMagic->nValue[0] == 1) ? TRUE : FALSE;\r\n"
"}\r\n"
"\r\n"
"void KNpcAttribModify::MeleeReturnResP( KNpc* pNpc, void* pData )\t// Linux 0x08096390: [0x1264] += v0 - khang phan don CAN CHIEN (Co Tuong Bach Chien Ham 1980)\r\n"
"{\r\n"
"\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\r\n"
"\tpNpc->m_nHSMeleeReturnResP += pMagic->nValue[0];\r\n"
"}\r\n"
"\r\n")
ac.rep("void KNpcAttribModify::Hide( KNpc* pNpc, void* pData )\r\n", HANDLERS + "void KNpcAttribModify::Hide( KNpc* pNpc, void* pData )\r\n", "than handler moi")
# 8. AddPhysicsDamageP: ma 10/11/12 (VLTK/Linux 0x0809A7F0 map) -> o 6/7/8
ac.rep("\telse if (nType >= 0 && nType < MAX_MELEE_WEAPON)\r\n\t{\r\n\t\tpNpc->m_CurrentMeleeEnhance[nType] += pMagic->nValue[0];\r\n\t}\r\n",
       "\telse if (nType >= 0 && nType < MAX_MELEE_WEAPON)\r\n\t{\r\n\t\tpNpc->m_CurrentMeleeEnhance[nType] += pMagic->nValue[0];\r\n\t}\r\n"
       "\t// " + MARK + " VLTK (Linux 0x0809A7F0 map): ma 10 -> o 6, 11 -> o 7 (Dao Thuan, Vu Hon Dao Phap 1975), 12 -> o 8 (Thuan Dao, Vu Hon Thuan Phap 1964).\r\n"
       "\t// 6..9 van la ma dac biet ALL/RANGE/MELEE/NONE cua du lieu item JX1 (KHONG doi).\r\n"
       "\telse if (nType >= 10 && nType <= 12)\r\n\t{\r\n\t\tpNpc->m_CurrentMeleeEnhance[nType - 4] += pMagic->nValue[0];\r\n\t}\r\n", "AddPhysicsDamageP ma 10-12")
ac.rep("\t\tfor (int i = 0; i < (MAX_MELEE_WEAPON + 1); i++)\r\n\t\t{\r\n\t\t\tpNpc->m_CurrentMeleeEnhance[i] += pMagic->nValue[0];\r\n",
       "\t\tfor (int i = 0; i < MAX_MELEE_WEAPON_VHTD; i++)\t// " + MARK + " 0..8 (them o 7/8 thuan)\r\n\t\t{\r\n\t\t\tpNpc->m_CurrentMeleeEnhance[i] += pMagic->nValue[0];\r\n", "AddPhysicsDamageP vong ALL/MELEE_ALL", count=2)
ac.save()

# ------------------------------------------------------------------------------------------------ 4 KNpc.h / KNpc.cpp
g = F(r"Core\Src\GameDataDef.h")
g.rep("#define\t\tMAX_MELEE_WEAPON\t\t\t6\r\n", "#define\t\tMAX_MELEE_WEAPON\t\t\t6\r\n#define\t\tMAX_MELEE_WEAPON_VHTD\t\t9\t// " + MARK + " o vu khi can chien 0..8: 6 Trien Thu, 7 Dao Thuan, 8 Thuan Dao (VLTK)\r\n#define\t\tMAX_HS_SP\t\t\t\t\t4\t// " + MARK + " so bo dem tang (No/Am Luat) moi NPC\r\n", "MAX_MELEE_WEAPON_VHTD + MAX_HS_SP")
g.save()

kh = F(r"Core\Src\KNpc.h")
kh.rep("\tint\t\t\t\t\tm_CurrentMeleeEnhance[MAX_MELEE_WEAPON+1];",
       "\tint\t\t\t\t\tm_CurrentMeleeEnhance[MAX_MELEE_WEAPON_VHTD+1];\t// " + MARK + " 0..8 vu khi (7/8 thuan) + 9 du", "mang MeleeEnhance 10 o")
kh.rep("\tKMagicAutoSkill \tm_DeathSkill[MAX_AUTOSKILL];\r\n",
       "\tKMagicAutoSkill \tm_DeathSkill[MAX_AUTOSKILL];\r\n"
       "\tKMagicAutoSkill \tm_CastSkill[MAX_AUTOSKILL];\t// " + MARK + " autocastskill: tu phong theo chu ky (Linux +0x182c)\r\n"
       "\tstruct { int nKey; int nCount; int nMax; } m_HSSp[MAX_HS_SP];\t// " + MARK + " bo dem tang No (1976) / Am Luat (2116)\r\n"
       "\tint\t\t\t\t\tm_nHSLockLife;\t\t\t// " + MARK + " lock_life gia tri\r\n"
       "\tint\t\t\t\t\tm_nHSLockLifeMode;\t\t// " + MARK + " lock_life che do (1 = khong thap hon)\r\n"
       "\tBOOL\t\t\t\tm_bHSInvincible;\t\t// " + MARK + " invincibility\r\n"
       "\tBOOL\t\t\t\tm_bHSForbidAttack;\t\t// " + MARK + " forbit_attack\r\n"
       "\tint\t\t\t\t\tm_nHSAddLightMagicP;\t// " + MARK + " addlightingmagic_p\r\n"
       "\tint\t\t\t\t\tm_nHSMeleeReturnResP;\t// " + MARK + " melee_returnres_p\r\n"
       "\tint\t\t\t\t\tm_nHSUnravel;\t\t\t// " + MARK + " unravel_effect (chua co co che)\r\n", "truong KNpc moi")
kh.rep("\tvoid\t\t\t\tCastAutoSkillAt(int nSkillId, int nSkillLevel, int nTarget);",
       "\tvoid\t\t\t\tCastAutoSkillAt(int nSkillId, int nSkillLevel, int nTarget);\r\n"
       "\tvoid\t\t\t\tHS_AutoCastTick();\t\t\t\t// " + MARK + " moi khung: autocastskill\r\n"
       "\tvoid\t\t\t\tHS_OnStateRemoved(KStateNode* pNode);\t// " + MARK + " cast_when_buff_removed\r\n"
       "\tvoid\t\t\t\tHS_ResetBuffTime(int nBuffSkillId);\t// " + MARK + " reset_bufftime\r\n"
       "\tint\t\t\t\t\tHS_SpGet(int nKey);\t\t\t\t// " + MARK + " so tang hien co\r\n"
       "\tvoid\t\t\t\tHS_SpAdd(int nKey, int nAdd);\t\t// " + MARK + "\r\n"
       "\tBOOL\t\t\t\tHS_SpCost(int nKey, int nCost);\t\t// " + MARK + " tru tang, FALSE neu khong du\r\n"
       "\tvoid\t\t\t\tHS_ResetVhtd();\t\t\t\t\t// " + MARK + " reset truong tinh lai thuoc tinh (khong reset so tang)", "decl ham moi")
kh.save()

k = F(r"Core\Src\KNpc.cpp")
# 4a resets
k.rep("\tm_CurrentManaToSkillEnhanceP = 0;\r\n\tm_CurrentSorbDamageP = 0;\r\n",
      "\tm_CurrentManaToSkillEnhanceP = 0;\r\n\tm_CurrentSorbDamageP = 0;\r\n\tHS_ResetVhtd(); memset(m_HSSp, 0, sizeof(m_HSSp));\t// " + MARK + "\r\n", "reset khoi tao")
k.rep("\tm_CurrentManaToSkillEnhanceP = 0;\t\t\t\t\t//#khi noi cong day tang ky nang cong kich\r\n",
      "\tm_CurrentManaToSkillEnhanceP = 0;\t\t\t\t\t//#khi noi cong day tang ky nang cong kich\r\n\tHS_ResetVhtd();\t// " + MARK + "\r\n", "reset RestoreNpcBaseInfo")
# 4b tick moi khung (server) truoc khoi aura trong ProcessState
k.rep_first_after("BOOL KNpc::ProcessState()",
      "\t\tif (m_ActiveAuraID)\r\n\t\t{\r\n\t\t\tif (m_SkillList.GetLevel(m_ActiveAuraID) > 0)\r\n",
      "#ifdef _SERVER\r\n\t\tHS_AutoCastTick();\t// " + MARK + " autocastskill (Linux 0x0808BEC0: moi khung Fire danh sach +0x182c)\r\n#endif\r\n"
      "\t\tif (m_ActiveAuraID)\r\n\t\t{\r\n\t\t\tif (m_SkillList.GetLevel(m_ActiveAuraID) > 0)\r\n", "ProcessState tick")
# 4c ReceiveDamage: bat tu
RD = "BOOL KNpc::ReceiveDamage(int nLauncher, int nMissleSeries, BOOL bIsPhysical, BOOL bIsMelee, void *pData, BOOL bUseAR, int nDoHurtP, int nMissRate)"
k.rep_first_after(RD, "\tif (Npc[nLauncher].m_Doing == do_death || Npc[nLauncher].m_Doing == do_revive)\r\n\t\treturn TRUE;\r\n",
      "\tif (Npc[nLauncher].m_Doing == do_death || Npc[nLauncher].m_Doing == do_revive)\r\n\t\treturn TRUE;\r\n"
      "\tif (m_bHSInvincible)\t// " + MARK + " invincibility (Thap Bo Nhat Sat_Buff 2130): khong nhan sat thuong/trang thai\r\n\t\treturn TRUE;\r\n", "ReceiveDamage bat tu")
# 4d DoSkill: cam danh + dieu kien tang (server)
k.rep("\t\t\t\tif(!IsPlayer() || Cost(pSkill->GetSkillCostType(), pSkill->GetSkillCost(this)))\r\n\t\t\t\t{\r\n",
      "#ifdef _SERVER\r\n"
      "\t\t\t\t// " + MARK + " forbit_attack (2131) va cost_sp (dieu kien tang No/Am Luat) - chi may chu (client khong biet so tang)\r\n"
      "\t\t\t\tif (IsPlayer() && m_bHSForbidAttack)\r\n\t\t\t\t\tgoto Exit;\r\n"
      "\t\t\t\tif (IsPlayer() && ((KSkill*)pSkill)->GetCostSpKey() > 0 && HS_SpGet(((KSkill*)pSkill)->GetCostSpKey()) < ((KSkill*)pSkill)->GetCostSp())\r\n\t\t\t\t\tgoto Exit;\r\n"
      "#endif\r\n"
      "\t\t\t\tif(!IsPlayer() || Cost(pSkill->GetSkillCostType(), pSkill->GetSkillCost(this)))\r\n\t\t\t\t{\r\n"
      "#ifdef _SERVER\r\n\t\t\t\t\tif (IsPlayer() && ((KSkill*)pSkill)->GetCostSpKey() > 0)\r\n\t\t\t\t\t\tHS_SpCost(((KSkill*)pSkill)->GetCostSpKey(), ((KSkill*)pSkill)->GetCostSp());\t// " + MARK + "\r\n#endif\r\n", "DoSkill cam danh + cost_sp")
# 4e lock_life trong CalcDamage (truoc SyncDamageInfo) va khe chi tu
k.rep("\tSyncDamageInfo(nAttacker, nDamage > m_CurrentLife ? m_CurrentLife : nDamage, COMBAT_INFO_DAMAGE_LIFE, 0, bIsDS);\t// [CHITU 01/09] chi tu khong con di qua day\r\n",
      "\t// " + MARK + " lock_life (Vu Muc Di Thu 1982): che do 1 - mau khong the giam duoi gia tri khoa\r\n"
      "\tif (m_nHSLockLife > 0 && m_nHSLockLifeMode == 1 && nDamage > 0 && m_CurrentLife > m_nHSLockLife && m_CurrentLife - nDamage < m_nHSLockLife)\r\n"
      "\t\tnDamage = m_CurrentLife - m_nHSLockLife;\r\n"
      "\tSyncDamageInfo(nAttacker, nDamage > m_CurrentLife ? m_CurrentLife : nDamage, COMBAT_INFO_DAMAGE_LIFE, 0, bIsDS);\t// [CHITU 01/09] chi tu khong con di qua day\r\n", "lock_life CalcDamage")
k.rep("\t\t\tSyncDamageInfo(nLauncher, nFSDamage > m_CurrentLife ? m_CurrentLife : nFSDamage, COMBAT_INFO_DAMAGE_LIFE, 0, TRUE);\r\n",
      "\t\t\tif (m_nHSLockLife > 0 && m_nHSLockLifeMode == 1 && m_CurrentLife > m_nHSLockLife && m_CurrentLife - nFSDamage < m_nHSLockLife)\t// " + MARK + " lock_life\r\n"
      "\t\t\t\tnFSDamage = m_CurrentLife - m_nHSLockLife;\r\n"
      "\t\t\tSyncDamageInfo(nLauncher, nFSDamage > m_CurrentLife ? m_CurrentLife : nFSDamage, COMBAT_INFO_DAMAGE_LIFE, 0, TRUE);\r\n", "lock_life chi tu")
# 4f khang phan don can chien
k.rep("\t\t\t\t\tint nCurrentDmgRetPercentResist = Npc[nAttacker].m_CurrentReturnResPercent;\r\n",
      "\t\t\t\t\tint nCurrentDmgRetPercentResist = Npc[nAttacker].m_CurrentReturnResPercent;\r\n"
      "\t\t\t\t\tif (bIsMelee)\t// " + MARK + " melee_returnres_p (Linux [0x1264]): khang phan don rieng cho don can chien\r\n"
      "\t\t\t\t\t\tnCurrentDmgRetPercentResist += Npc[nAttacker].m_nHSMeleeReturnResP;\r\n", "melee_returnres_p")
# 4g het han trang thai -> cast_when_buff_removed
k.rep("\t\tif (pTempNode->m_LeftTime == 0)\r\n\t\t{\r\n\t\t\tint i;\r\n\t\t\tfor (i = 0; i < MAX_SKILL_STATE; i++)\r\n\t\t\t{\r\n\t\t\t\tif (pTempNode->m_State[i].nAttribType)\r\n\t\t\t\t{\r\n\t\t\t\t\tModifyAttrib(m_Index, &pTempNode->m_State[i]);\r\n",
      "\t\tif (pTempNode->m_LeftTime == 0)\r\n\t\t{\r\n#ifdef _SERVER\r\n\t\t\tHS_OnStateRemoved(pTempNode);\t// " + MARK + " cast_when_buff_removed (Vu Muc Di Thu 1982 -> 1991)\r\n#endif\r\n\t\t\tint i;\r\n\t\t\tfor (i = 0; i < MAX_SKILL_STATE; i++)\r\n\t\t\t{\r\n\t\t\t\tif (pTempNode->m_State[i].nAttribType)\r\n\t\t\t\t{\r\n\t\t\t\t\tModifyAttrib(m_Index, &pTempNode->m_State[i]);\r\n", "het han trang thai -> cast")
# 4h ModifyAttrib intercept reset_bufftime
k.rep("\t\tif (nAttacker > 0 && nAttacker < MAX_NPC && Npc[nAttacker].m_Index > 0 && pMA->nValue[0] > 0)\r\n\t\t\tNpc[nAttacker].DetonateMissles(pMA->nValue[0] >> 8, pMA->nValue[2], pMA->nValue[0] & 0xff);\r\n\t\treturn;\r\n\t}\r\n#endif\r\n",
      "\t\tif (nAttacker > 0 && nAttacker < MAX_NPC && Npc[nAttacker].m_Index > 0 && pMA->nValue[0] > 0)\r\n\t\t\tNpc[nAttacker].DetonateMissles(pMA->nValue[0] >> 8, pMA->nValue[2], pMA->nValue[0] & 0xff);\r\n\t\treturn;\r\n\t}\r\n"
      "\tif (pMA->nAttribType == magic_reset_bufftime)\t// " + MARK + " Tru Gian Diet Ninh 1985: dat lai thoi gian debuff 1988 tren nan nhan (this)\r\n"
      "\t{\r\n\t\tif (pMA->nValue[0] > 0)\r\n\t\t\tHS_ResetBuffTime(pMA->nValue[0]);\r\n\t\treturn;\r\n\t}\r\n#endif\r\n", "ModifyAttrib reset_bufftime")
# 4i AppendSkillEffect: lightingdamage_p
k.rep("\tif (pTemp->nAttribType == magic_lightingdamage_v)\r\n\t{\r\n\t\tpDes->nAttribType = magic_lightingdamage_v;\r\n",
      "\tif (pTemp->nAttribType == magic_lightingdamage_p)\r\n"
      "\t{\r\n"
      "\t\t// " + MARK + " lightingdamage_p (Tieu Dao cam 2136/2138/2140/2141/2142/2143): sat thuong Loi = % cua (noi cong co ban + noi cong Loi cua nguoi phat\r\n"
      "\t\t// x (100 + addlightingmagic_p)/100). Thiet ke theo mo ta client VLTK 'Sat thuong Loi: #d1-%' (khong co ban server chuan de doi chieu).\r\n"
      "\t\tint nLMin = m_PhysicsMagic.nValue[0] + m_CurrentLightMagic.nValue[0] * (MAX_PERCENT + m_nHSAddLightMagicP) / MAX_PERCENT;\r\n"
      "\t\tint nLMax = m_PhysicsMagic.nValue[2] + m_CurrentLightMagic.nValue[2] * (MAX_PERCENT + m_nHSAddLightMagicP) / MAX_PERCENT;\r\n"
      "\t\tif (nLMax < nLMin) nLMax = nLMin;\r\n"
      "\t\tpDes->nAttribType = magic_lightingdamage_v;\r\n"
      "\t\tpDes->nValue[0] = (int)((__int64)nLMin * pTemp->nValue[0] / MAX_PERCENT * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT);\r\n"
      "\t\tpDes->nValue[1] = pTemp->nValue[1];\r\n"
      "\t\tpDes->nValue[2] = (int)((__int64)nLMax * pTemp->nValue[0] / MAX_PERCENT * (MAX_PERCENT + nAddDamageP) / MAX_PERCENT);\r\n"
      "\t\tpDes->nValue[0] += (pDes->nValue[0] * DamePecentToLevel) / MAX_PERCENT;\r\n"
      "\t\tpDes->nValue[2] += (pDes->nValue[2] * DamePecentToLevel) / MAX_PERCENT;\r\n"
      "\t}\r\n"
      "\telse if (pTemp->nAttribType == magic_lightingdamage_v)\r\n\t{\r\n\t\tpDes->nAttribType = magic_lightingdamage_v;\r\n", "AppendSkillEffect lightingdamage_p")
# 4j ham moi (sau CastAutoSkillAt, trong #ifdef _SERVER)
k.rep("\tpSkill->Cast(m_Index, -1, nTarget);\r\n\t// hoi chieu server nhu KNpc::Cast(int,int)",
      "\tpSkill->Cast(m_Index, -1, nTarget);\r\n\t// hoi chieu server nhu KNpc::Cast(int,int)", "anchor CastAutoSkillAt ok")
NEWFN = (
"// " + MARK + " ==================== VU HON / TIEU DAO ====================\r\n"
"void KNpc::HS_AutoCastTick()\t// autocastskill: Linux 0x0808BEC0 moi khung Fire(+0x182c, this, -1) -> chi nham CHINH MINH (loai 1 -> muc tieu -1 = hong)\r\n"
"{\r\n"
"\tif (!m_Index || m_RegionIndex < 0)\r\n\t\treturn;\r\n"
"\tif (m_Doing == do_death || m_Doing == do_revive)\r\n\t\treturn;\r\n"
"\tfor (int i = 0; i < MAX_AUTOSKILL; i++)\r\n"
"\t{\r\n"
"\t\tKMagicAutoSkill& rA = m_CastSkill[i];\r\n"
"\t\tif (rA.nSkillId <= 0 || rA.nSkillId >= MAX_SKILL || rA.nSkillLevel <= 0 || rA.nSkillLevel >= MAX_SKILLLEVEL)\r\n\t\t\tcontinue;\r\n"
"\t\tif (rA.dwNextCastTime > SubWorld[m_SubWorldIndex].m_dwCurrentTime)\r\n\t\t\tcontinue;\r\n"
"\t\tif (rA.nRate < 100 && !g_RandPercent(rA.nRate))\r\n"
"\t\t{\r\n\t\t\trA.dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + rA.nWaitCastTime;\t// Linux: hut ty le van dat lai chu ky\r\n\t\t\tcontinue;\r\n\t\t}\r\n"
"\t\tif (rA.nType != 1)\r\n\t\t\tthis->Cast(rA.nSkillId, rA.nSkillLevel);\r\n"
"\t\trA.dwNextCastTime = SubWorld[m_SubWorldIndex].m_dwCurrentTime + rA.nWaitCastTime;\r\n"
"\t}\r\n"
"}\r\n"
"\r\n"
"void KNpc::HS_OnStateRemoved(KStateNode* pNode)\t// cast_when_buff_removed {id, cap (-1 = cap ky nang v2 cua chu), id tham chieu} - node luu GIA TRI DAO DAU\r\n"
"{\r\n"
"\tif (!pNode || !m_Index || m_RegionIndex < 0)\r\n\t\treturn;\r\n"
"\tif (m_Doing == do_death || m_Doing == do_revive)\r\n\t\treturn;\r\n"
"\tfor (int i = 0; i < MAX_SKILL_STATE; i++)\r\n"
"\t{\r\n"
"\t\tif (pNode->m_State[i].nAttribType != magic_cast_when_buff_removed)\r\n\t\t\tcontinue;\r\n"
"\t\tint nSkill = -pNode->m_State[i].nValue[0];\r\n"
"\t\tint nLevel = -pNode->m_State[i].nValue[1];\r\n"
"\t\tint nRef = -pNode->m_State[i].nValue[2];\r\n"
"\t\tif (nLevel <= 0 && nRef > 0 && nRef < MAX_SKILL)\r\n\t\t\tnLevel = m_SkillList.GetCurrentLevel(nRef);\r\n"
"\t\tif (nLevel <= 0) nLevel = 1;\r\n"
"\t\tif (nSkill > 0 && nSkill < MAX_SKILL && nLevel < MAX_SKILLLEVEL)\r\n\t\t\tthis->Cast(nSkill, nLevel);\r\n"
"\t}\r\n"
"}\r\n"
"\r\n"
"void KNpc::HS_ResetBuffTime(int nBuffSkillId)\t// reset_bufftime: tra thoi gian con lai cua trang thai nBuffSkillId ve thoi luong goc (nValue[1] thuoc tinh trang thai dau)\r\n"
"{\r\n"
"\tKStateNode* pNode = (KStateNode*)m_StateSkillList.GetTail();\r\n"
"\twhile (pNode)\r\n"
"\t{\r\n"
"\t\tif (pNode->m_SkillID == nBuffSkillId && pNode->m_LeftTime > 0)\r\n"
"\t\t{\r\n"
"\t\t\tKSkill* pS = (KSkill*)g_SkillManager.GetSkill(nBuffSkillId, pNode->m_Level);\r\n"
"\t\t\tif (pS && pS->GetStateAttribsNum() > 0 && pS->GetStateAttribs()[0].nValue[1] > 0)\r\n\t\t\t\tpNode->m_LeftTime = pS->GetStateAttribs()[0].nValue[1];\r\n"
"\t\t\treturn;\r\n"
"\t\t}\r\n"
"\t\tpNode = (KStateNode*)pNode->GetPrev();\r\n"
"\t}\r\n"
"}\r\n"
"\r\n"
"int KNpc::HS_SpGet(int nKey)\r\n"
"{\r\n"
"\tfor (int i = 0; i < MAX_HS_SP; i++)\r\n\t\tif (m_HSSp[i].nKey == nKey) return m_HSSp[i].nCount;\r\n"
"\treturn 0;\r\n"
"}\r\n"
"\r\n"
"void KNpc::HS_SpAdd(int nKey, int nAdd)\r\n"
"{\r\n"
"\tfor (int i = 0; i < MAX_HS_SP; i++)\r\n"
"\t{\r\n"
"\t\tif (m_HSSp[i].nKey != nKey) continue;\r\n"
"\t\tm_HSSp[i].nCount += nAdd;\r\n"
"\t\tif (m_HSSp[i].nMax > 0 && m_HSSp[i].nCount > m_HSSp[i].nMax) m_HSSp[i].nCount = m_HSSp[i].nMax;\r\n"
"\t\tif (m_HSSp[i].nCount < 0) m_HSSp[i].nCount = 0;\r\n"
"\t\treturn;\r\n"
"\t}\r\n"
"}\r\n"
"\r\n"
"BOOL KNpc::HS_SpCost(int nKey, int nCost)\r\n"
"{\r\n"
"\tfor (int i = 0; i < MAX_HS_SP; i++)\r\n"
"\t{\r\n"
"\t\tif (m_HSSp[i].nKey != nKey) continue;\r\n"
"\t\tif (m_HSSp[i].nCount < nCost) return FALSE;\r\n"
"\t\tm_HSSp[i].nCount -= nCost;\r\n"
"\t\treturn TRUE;\r\n"
"\t}\r\n"
"\treturn FALSE;\r\n"
"}\r\n"
"#endif\r\n"
"\r\n"
"void KNpc::HS_ResetVhtd()\t// goi o khoi tao / RestoreNpcBaseInfo / KPlayer::UpdataCurData (truoc khi ap lai trang thai + trang bi)\r\n"
"{\r\n"
"\tmemset(m_CastSkill, 0, sizeof(m_CastSkill));\r\n"
"\tm_nHSLockLife = 0; m_nHSLockLifeMode = 0;\r\n"
"\tm_bHSInvincible = FALSE; m_bHSForbidAttack = FALSE;\r\n"
"\tm_nHSAddLightMagicP = 0; m_nHSMeleeReturnResP = 0; m_nHSUnravel = 0;\r\n"
"}\r\n"
"#ifdef _SERVER\r\n")
k.rep_first_after("void KNpc::CastAutoSkillAt(int nSkillId, int nSkillLevel, int nTarget)", "}\r\n\r\n// [HOASON 02/09] autoreplyskill:",
      "}\r\n\r\n" + NEWFN + "// [HOASON 02/09] autoreplyskill:", "ham HS_* moi")
k.save()

# ------------------------------------------------------------------------------------------------ 5 KSkills
sh = F(r"Core\Src\KSkills.h")
sh.rep("\tint\t\t\t\t\tGetSkillCost(void *)const{return m_nCost;};",
       "\tint\t\t\t\t\tGetSkillCost(void *)const{return m_nCost;};\r\n"
       "\tint\t\t\t\t\tGetCostSpKey() const { return m_nCostSpKey; };\t// " + MARK + " cost_sp: id ky nang khoa (1976 No / 2116 Am Luat)\r\n"
       "\tint\t\t\t\t\tGetCostSp() const { return m_nCostSp; };\t\t// " + MARK + " cost_sp: so tang can\r\n"
       "\tint\t\t\t\t\tGetStateAttribsNum() const { return m_nStateAttribsNum; };\t// " + MARK, "getter cost_sp")
sh.rep("\tint\t\t\t\t\tm_nCost;\t\t\t\t//", "\tint\t\t\t\t\tm_nCostSpKey;\t\t\t// " + MARK + " cost_sp\r\n\tint\t\t\t\t\tm_nCostSp;\t\t\t\t// " + MARK + " cost_sp\r\n\tint\t\t\t\t\tm_nCost;\t\t\t\t//", "member cost_sp")
sh.save()
sc = F(r"Core\Src\KSkills.cpp")
sc.rep("\tm_nCost = 0;\r\n", "\tm_nCost = 0;\r\n\tm_nCostSpKey = 0; m_nCostSp = 0;\t// " + MARK + "\r\n", "ctor cost_sp")
sc.rep("\tm_nStateAttribsNum\t= 0;\t\t\r\n", "\tm_nStateAttribsNum\t= 0;\t\t\r\n\tm_nCostSpKey = 0; m_nCostSp = 0;\t// " + MARK + "\r\n", "LoadSkillLevelData reset cost_sp")
sc.rep("\t\t\tif (i > magic_skill_begin && i < magic_skill_end)\r\n\t\t\t{\r\n\t\t\t\tswitch(i)\r\n\t\t\t\t{\r\n\t\t\t\tcase magic_skill_cost_v:",
       "\t\t\t// " + MARK + " thuoc tinh moi Vu Hon / Tieu Dao (client VLTK)\r\n"
       "\t\t\tif (i == magic_cost_sp)\t// dieu kien tang: {id khoa, 0, so tang} -> muc ky nang, KHONG ap len muc tieu\r\n"
       "\t\t\t{\r\n\t\t\t\tm_nCostSpKey = nValue1;\r\n\t\t\t\tm_nCostSp = nValue3;\r\n\t\t\t\treturn TRUE;\r\n\t\t\t}\r\n"
       "\t\t\tif (i == magic_lightingdamage_p)\t// sat thuong Loi % -> o damage[12] (cung o voi lightingdamage_v)\r\n"
       "\t\t\t{\r\n\t\t\t\tm_DamageAttribs[12].nAttribType = i;\r\n\t\t\t\tm_DamageAttribs[12].nValue[0] = nValue1;\r\n\t\t\t\tm_DamageAttribs[12].nValue[1] = nValue2;\r\n\t\t\t\tm_DamageAttribs[12].nValue[2] = nValue3;\r\n\t\t\t\tm_nDamageAttribsNum ++;\r\n\t\t\t\treturn TRUE;\r\n\t\t\t}\r\n"
       "\t\t\tif (i == magic_reset_bufftime)\t// ap NGAY moi don trung (khong tao nut trang thai -1 tren nan nhan)\r\n"
       "\t\t\t{\r\n\t\t\t\tm_ImmediateAttribs[m_nImmediateAttribsNum].nAttribType = i;\r\n\t\t\t\tm_ImmediateAttribs[m_nImmediateAttribsNum].nValue[0] = nValue1;\r\n\t\t\t\tm_ImmediateAttribs[m_nImmediateAttribsNum].nValue[1] = nValue2;\r\n\t\t\t\tm_ImmediateAttribs[m_nImmediateAttribsNum].nValue[2] = nValue3;\r\n\t\t\t\tm_nImmediateAttribsNum ++;\r\n\t\t\t\treturn TRUE;\r\n\t\t\t}\r\n"
       "\t\t\tif (i > magic_skill_begin && i < magic_skill_end)\r\n\t\t\t{\r\n\t\t\t\tswitch(i)\r\n\t\t\t\t{\r\n\t\t\t\tcase magic_skill_cost_v:", "Parse cost_sp/lightingdamage_p/reset_bufftime")
# mo ta han che vu khi: 7 Dao Thuan, 8 Thuan Dao (VLTK meleeweapon Particular 7/8), 103 = Cam (rangeweapon Particular 3 + 100) - khoa [WeaponLimit] gamesetting.ini
sc.rep('\t\tcase 101:\r\n\t\t\tg_GameSetting.GetString("WeaponLimit", "101", "", szTemp, sizeof(szTemp));',
       '\t\tcase 7:\t// ' + MARK + ' Dao Thuan (Vu Hon)\r\n\t\t\tg_GameSetting.GetString("WeaponLimit", "7", "", szTemp, sizeof(szTemp));\r\n\t\t\tstrcat(pszMsg, szTemp);\r\n\t\t\tbreak;\r\n'
       '\t\tcase 8:\t// ' + MARK + ' Thuan Dao (Vu Hon)\r\n\t\t\tg_GameSetting.GetString("WeaponLimit", "8", "", szTemp, sizeof(szTemp));\r\n\t\t\tstrcat(pszMsg, szTemp);\r\n\t\t\tbreak;\r\n'
       '\t\tcase 103:\t// ' + MARK + ' Cam (Tieu Dao) = range particular 3 + 100\r\n\t\t\tg_GameSetting.GetString("WeaponLimit", "103", "", szTemp, sizeof(szTemp));\r\n\t\t\tstrcat(pszMsg, szTemp);\r\n\t\t\tbreak;\r\n'
       '\t\tcase 101:\r\n\t\t\tg_GameSetting.GetString("WeaponLimit", "101", "", szTemp, sizeof(szTemp));', "WeaponLimit mo ta 7/8/103")
sc.save()

# ------------------------------------------------------------------------------------------------ 6 KPlayer
kp = F(r"Core\Src\KPlayer.cpp")
kp.rep("\tNpc[m_nIndex].m_CurrentAntiDoHurtP = 0;\t// [PF 31/08k]\r\n",
       "\tNpc[m_nIndex].m_CurrentAntiDoHurtP = 0;\t// [PF 31/08k]\r\n\tNpc[m_nIndex].HS_ResetVhtd();\t// " + MARK + " (so tang No/Am Luat GIU)\r\n", "UpdataCurData reset")
kp.save()

# ------------------------------------------------------------------------------------------------ 7 S3Client UI
ui = F(r"S3Client\Ui\UiCase\UiSkillsNew.cpp")
ui.rep("\t{ 1370, 20 }   // Hao Nhien Chi Khi (tien giai)\r\n",
       "\t{ 1370, 20 },  // Hao Nhien Chi Khi (tien giai)\r\n"
       "\t// Faction 11 Vu Hon " + MARK + " - o theo [Skill_11_i] (cot Thuan Phap / Dao Phap / Ho Tro / 120+)\r\n"
       "\t{ 1972, 0 }, { 1965, 1 }, { 1963, 2 }, { 1967, 3 }, { 1969, 4 },\r\n"
       "\t{ 1974, 6 }, { 1977, 7 }, { 1979, 8 }, { 1981, 9 }, { 1983, 10 }, { 1985, 11 },\r\n"
       "\t{ 1964, 12 }, { 1975, 13 }, { 1976, 14 }, { 1980, 15 }, { 1971, 16 }, { 1982, 17 },\r\n"
       "\t{ 1984, 18 }, { 1986, 19 },\r\n"
       "\t// Faction 12 Tieu Dao " + MARK + " - o theo [Skill_12_i] (cot Kiem Phap / Cam Phap / Ho Tro / 120+)\r\n"
       "\t{ 2114, 0 }, { 2118, 1 }, { 2120, 2 }, { 2124, 3 }, { 2129, 4 },\r\n"
       "\t{ 2136, 6 }, { 2138, 7 }, { 2140, 8 }, { 2141, 9 }, { 2142, 10 }, { 2123, 11 },\r\n"
       "\t{ 2115, 12 }, { 2137, 13 }, { 2116, 14 }, { 2121, 15 }, { 2122, 16 },\r\n"
       "\t{ 2127, 18 }, { 2132, 19 }\r\n", "bang o ky nang 11/12")
ui.rep("\t\t\t\t|| Info.nFirstAddFaction == 5 || Info.nFirstAddFaction == 6 || Info.nFirstAddFaction == 10) {\t// [HOASON 01/09]\r\n",
       "\t\t\t\t|| Info.nFirstAddFaction == 5 || Info.nFirstAddFaction == 6 || Info.nFirstAddFaction == 10\r\n\t\t\t\t|| Info.nFirstAddFaction == 11 || Info.nFirstAddFaction == 12) {\t// [HOASON 01/09] " + MARK + "\r\n", "nut dong 11/12")
ui.save()
print("XONG%s." % (" (KIEM)" if KIEM else ""))
