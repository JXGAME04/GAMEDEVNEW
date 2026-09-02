# -*- coding: utf-8 -*-
"""vhtd_engine_patch1c.py - vá theo PHẢN BIỆN 02/09 (tác tử đối kháng) trên KNpc.cpp / KNpcAttribModify.cpp, marker [VHTD 02/09c]:
  A1 HS_OnStateRemoved: cấp -1 mà chưa học kỹ năng tham chiếu (1984) -> KHÔNG cast (trước ép cấp 1 -> 1991 hồi 30% máu miễn phí).
  A2 HS_AutoCastTick chạy MỖI KHUNG (trước nằm trong khối % GAME_UPDATE_TIME = 10 khung -> Âm Luật chậm, hộ thuẫn hở 3 khung);
     AutoCastSkill: mục MỚI tạo -> lần đầu sau 'wait' khung (chặn khai thác đổi trang bị/lên cấp = +tầng ngay).
  B1 invincibility: đòn của ĐỊCH -> return FALSE (không sát thương, không trạng thái); buff phe ta/tự thân vẫn qua.
  B2 lock_life chế độ 1: máu đã <= khoá -> sát thương 0 (đúng nghĩa "không thể giảm dưới X"; VLTK 1982) - cả CalcDamage và khe chí tử.
  B3 KNpc::Cast(int,int): kiểm NULL pOrdinSkill (id từ dữ liệu).
  B4 DoSkill: chỉ ép (KSkill*) khi style != SKILL_SS_Thief.
Doc/ghi latin-1. Idempotent. DUNG: python vhtd_engine_patch1c.py [--kiem]
"""
import io, os, sys
ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
KIEM = "--kiem" in sys.argv
M = "[VHTD 02/09]"; MC = "[VHTD 02/09c]"
hib = lambda t: sum(1 for c in t if ord(c) >= 0x80)

class F:
    def __init__(self, name):
        self.p = os.path.join(ROOT, name); self.name = name
        self.s = io.open(self.p, "r", encoding="latin-1", newline="").read(); self.orig = self.s; self.n = 0
    def rep(self, old, new, tag):
        c = self.s.count(old)
        if c == 0:
            if new in self.s: print("  [=] %s: da ap (%s)" % (self.name, tag)); return
            raise SystemExit("KHONG THAY anchor %s (%s):\n%r" % (self.name, tag, old[:150]))
        if c != 1: raise SystemExit("anchor khong duy nhat (%d) %s (%s)" % (c, self.name, tag))
        self.s = self.s.replace(old, new); self.n += 1; print("  [+] %s: %s" % (self.name, tag))
    def save(self):
        if self.s == self.orig: print("  (khong doi) %s" % self.name); return
        if hib(self.s) != hib(self.orig) or "\xef\xbf\xbd" in self.s: raise SystemExit("LECH BYTE CAO %s" % self.name)
        if not KIEM: io.open(self.p, "w", encoding="latin-1", newline="").write(self.s)
        print("  => ghi %s (%d cho)%s" % (self.name, self.n, " KIEM" if KIEM else ""))

k = F("KNpc.cpp")
# A1
k.rep("\t\tif (nLevel <= 0) nLevel = 1;\r\n",
      "\t\tif (nLevel <= 0) continue;\t// " + MC + " phan bien A1: chua hoc ky nang tham chieu (1984 cap 120) -> KHONG cast (truoc ep cap 1 -> 1991 hoi 30% mau mien phi)\r\n", "A1 cast_when_buff_removed cap -1")
# A2: tick moi khung
old_tick = "#ifdef _SERVER\r\n\t\tHS_AutoCastTick();\t// " + M + " autocastskill (Linux 0x0808BEC0: moi khung Fire danh sach +0x182c)\r\n#endif\r\n\t\tif (m_ActiveAuraID)\r\n"
k.rep(old_tick, "\t\tif (m_ActiveAuraID)\r\n", "A2 bo tick trong khoi 10 khung")
old_blk = "\tif (!(m_LoopFrames % GAME_UPDATE_TIME))\r\n\t{\r\n#ifdef _SERVER\r\n\t\tif (m_Doing == do_sit)\r\n"
k.rep(old_blk, "#ifdef _SERVER\r\n\tHS_AutoCastTick();\t// " + MC + " autocastskill MOI KHUNG (Linux 0x0808BEC0 Fire +0x182c moi khung; phan bien A2: ban truoc nam trong khoi % GAME_UPDATE_TIME = 10 khung)\r\n#endif\r\n" + old_blk, "A2 tick moi khung")
# B1 invincibility
k.rep("\tif (m_bHSInvincible)\t// " + M + " invincibility (Thap Bo Nhat Sat_Buff 2130): khong nhan sat thuong/trang thai\r\n\t\treturn TRUE;\r\n",
      "\t// " + MC + " invincibility (Thap Bo Nhat Sat_Buff 2130): don cua DICH -> FALSE = truot (khong sat thuong, khong trang thai - nguoi goi chi ap trang thai khi TRUE);\r\n"
      "\t// buff tu than / phe ta (2130, 2131, 1989, 2117 di qua ReceiveDamage(self)) van qua binh thuong. (phan bien B1)\r\n"
      "\tif (m_bHSInvincible && nLauncher != m_Index && (NpcSet.GetRelation(nLauncher, m_Index) & relation_enemy))\r\n\t\treturn FALSE;\r\n", "B1 bat tu chan ca trang thai dich")
# B2 lock_life
k.rep("\tif (m_nHSLockLife > 0 && m_nHSLockLifeMode == 1 && nDamage > 0 && m_CurrentLife > m_nHSLockLife && m_CurrentLife - nDamage < m_nHSLockLife)\r\n\t\tnDamage = m_CurrentLife - m_nHSLockLife;\r\n",
      "\tif (m_nHSLockLife > 0 && m_nHSLockLifeMode == 1 && nDamage > 0)\t// " + MC + " phan bien B2: mau da <= khoa -> khong mat them (dung nghia 'khong the giam duoi X')\r\n"
      "\t{\r\n\t\tif (m_CurrentLife <= m_nHSLockLife) nDamage = 0;\r\n\t\telse if (m_CurrentLife - nDamage < m_nHSLockLife) nDamage = m_CurrentLife - m_nHSLockLife;\r\n\t}\r\n", "B2 lock_life CalcDamage")
k.rep("\t\t\tif (m_nHSLockLife > 0 && m_nHSLockLifeMode == 1 && m_CurrentLife > m_nHSLockLife && m_CurrentLife - nFSDamage < m_nHSLockLife)\t// " + M + " lock_life\r\n\t\t\t\tnFSDamage = m_CurrentLife - m_nHSLockLife;\r\n",
      "\t\t\tif (m_nHSLockLife > 0 && m_nHSLockLifeMode == 1 && nFSDamage > 0)\t// " + M + " lock_life (" + MC + " B2)\r\n"
      "\t\t\t{\r\n\t\t\t\tif (m_CurrentLife <= m_nHSLockLife) nFSDamage = 0;\r\n\t\t\t\telse if (m_CurrentLife - nFSDamage < m_nHSLockLife) nFSDamage = m_CurrentLife - m_nHSLockLife;\r\n\t\t\t}\r\n", "B2 lock_life chi tu")
# B3 Cast(int,int) NULL
k.rep("\t\tKSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillId, nSkillLevel);\r\n\t\tpOrdinSkill->Cast(m_Index, nMpsX, nMpsY);\r\n",
      "\t\tKSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(nSkillId, nSkillLevel);\r\n\t\tif (!pOrdinSkill)\t// " + MC + " phan bien B3: id tu du lieu (autocastskill/cast_when_buff_removed) co the khong co dong skills.txt\r\n\t\t\treturn;\r\n\t\tpOrdinSkill->Cast(m_Index, nMpsX, nMpsY);\r\n", "B3 Cast NULL")
# B4 DoSkill ep kieu
k.rep("\t\t\t\tif (IsPlayer() && ((KSkill*)pSkill)->GetCostSpKey() > 0 && HS_SpGet(((KSkill*)pSkill)->GetCostSpKey()) < ((KSkill*)pSkill)->GetCostSp())\r\n",
      "\t\t\t\tif (IsPlayer() && eStyle != SKILL_SS_Thief && ((KSkill*)pSkill)->GetCostSpKey() > 0 && HS_SpGet(((KSkill*)pSkill)->GetCostSpKey()) < ((KSkill*)pSkill)->GetCostSp())\t// " + MC + " B4: KThiefSkill khong phai KSkill\r\n", "B4 ep kieu (1)")
k.rep("\t\t\t\t\tif (IsPlayer() && ((KSkill*)pSkill)->GetCostSpKey() > 0)\r\n",
      "\t\t\t\t\tif (IsPlayer() && eStyle != SKILL_SS_Thief && ((KSkill*)pSkill)->GetCostSpKey() > 0)\t// " + MC + " B4\r\n", "B4 ep kieu (2)")
k.save()

a = F("KNpcAttribModify.cpp")
old_fn = "{\r\n\tHS_AutoSkillModify(pNpc->m_CastSkill, pNpc, (KMagicAttrib *)pData);\r\n}\r\n"
new_fn = ("{\r\n"
          "\tKMagicAttrib* pMagic = (KMagicAttrib *)pData;\r\n"
          "\tint nKey = abs(pMagic->nValue[0]) & 0xffffff;\r\n"
          "\tBOOL bCoTruoc = FALSE;\r\n"
          "\tint i;\r\n"
          "\tfor (i = 0; i < MAX_AUTOSKILL; i++)\r\n"
          "\t\tif (pNpc->m_CastSkill[i].nRate > 0 && pNpc->m_CastSkill[i].nSkillId * 256 + pNpc->m_CastSkill[i].nSkillLevel == nKey) { bCoTruoc = TRUE; break; }\r\n"
          "\tHS_AutoSkillModify(pNpc->m_CastSkill, pNpc, pMagic);\r\n"
          "\t// " + MC + " phan bien A2: muc MOI tao (ke ca sau HS_ResetVhtd + ReCalcState khi doi trang bi/len cap) -> lan phong dau sau 'wait' khung,\r\n"
          "\t// khong phong ngay (chan khai thac doi trang bi = +tang No/Am Luat tuc thi). Muc da co giu dong ho cu.\r\n"
          "\tif (!bCoTruoc && pNpc->m_SubWorldIndex >= 0)\r\n"
          "\t\tfor (i = 0; i < MAX_AUTOSKILL; i++)\r\n"
          "\t\t\tif (pNpc->m_CastSkill[i].nRate > 0 && pNpc->m_CastSkill[i].nSkillId * 256 + pNpc->m_CastSkill[i].nSkillLevel == nKey)\r\n"
          "\t\t\t\tpNpc->m_CastSkill[i].dwNextCastTime = SubWorld[pNpc->m_SubWorldIndex].m_dwCurrentTime + pNpc->m_CastSkill[i].nWaitCastTime;\r\n"
          "}\r\n")
i0 = a.s.find("void KNpcAttribModify::AutoCastSkill(")
if i0 < 0: raise SystemExit("khong thay AutoCastSkill")
j0 = a.s.find(old_fn, i0)
if j0 < 0 or j0 - i0 > 400:
    if MC in a.s[i0:i0 + 1500]: print("  [=] KNpcAttribModify.cpp: AutoCastSkill da ap")
    else: raise SystemExit("AutoCastSkill body anchor")
else:
    a.s = a.s[:j0] + new_fn + a.s[j0 + len(old_fn):]; a.n += 1; print("  [+] KNpcAttribModify.cpp: A2 AutoCastSkill muc moi -> now + wait")
a.save()
print("XONG%s." % (" (KIEM)" if KIEM else ""))
