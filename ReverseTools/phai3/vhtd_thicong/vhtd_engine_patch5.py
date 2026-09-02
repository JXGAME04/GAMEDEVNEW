# -*- coding: utf-8 -*-
"""vhtd_engine_patch5.py [VHTD 02/09h] - dot 5 Vu Hon / Tieu Dao (10 muc chu 02/09 ~10:30 sau swap dot 4):
 H1 KNpc.cpp DoSkill   client: thieu tang No/Am Luat -> CHAN tai cho + thong bao (truoc chi server chan im lang, client van dien hoat/luot).
 H2 CoreShell.cpp      GDI_NPC_STATE_SKILL: bo node m_LeftTime == 0 (2117 Am Luat tuc thoi tao node 0 giay -> icon thuoc '00s' rac).
 H3 KNpc.cpp DoBlurMove chieu con (2119 'Khoi') phong TAI DIEM DEN (buoc 2) thay vi diem xuat phat - kem sat thuong (du lieu patch5).
 H4 KSkills.cpp        tooltip: dong 'Sat thuong Loi (theo noi luc toi da): X' cho lightingdamage_p (cam Tieu Dao) - 'khong hien luc tay'.
Doc/ghi latin-1, NL theo tung tep, marker [VHTD 02/09h], chay lai = [=]. DUNG: python vhtd_engine_patch5.py [--kiem]
"""
import io, os, sys, re

KIEM = "--kiem" in sys.argv
ROOT = r"D:\GAMEDEVNEW"
CORE = os.path.join(ROOT, "Sources", "Core", "Src")
MK = "[VHTD 02/09h]"
NL = "\r\n"
T = "\t"

def rd(p):
    return io.open(p, "r", encoding="latin-1", newline="").read()

def hib(s):
    return sum(1 for c in s if ord(c) >= 0x80)

class F:
    def __init__(self, path):
        self.path = path
        self.s = rd(path)
        self.orig = self.s
        self.n = 0
        self.nl = "\r\n" if self.s.count("\r\n") * 2 > self.s.count("\n") else "\n"
    def rep(self, old, new, tag, count=1):
        if self.nl != "\r\n":
            old = old.replace("\r\n", self.nl); new = new.replace("\r\n", self.nl)
        if tag in self.s:
            print("  [=] %s: %s" % (os.path.basename(self.path), tag)); return
        c = self.s.count(old)
        if c != count:
            raise SystemExit("neo %s: %d lan (can %d) trong %s\n%r" % (tag, c, count, self.path, old[:160]))
        self.s = self.s.replace(old, new)
        self.n += 1
        print("  [+] %s: %s (%d cho)" % (os.path.basename(self.path), tag, count))
    def save(self):
        if self.s == self.orig:
            return
        if "\xef\xbf\xbd" in self.s:
            raise SystemExit("EF BF BD trong " + self.path)
        # chuoi TCVN3 moi (thong bao) duoc viet bang ESCAPE bat phan -> byte cao KHONG doi
        assert hib(self.s) == hib(self.orig), "byte cao doi trong " + self.path
        if not KIEM:
            io.open(self.path, "w", encoding="latin-1", newline="").write(self.s)
        print("  => ghi %s (%d hunk)%s" % (self.path, self.n, " KIEM" if KIEM else ""))

# ------------------------------------------------------------------ H1 + H3 KNpc.cpp
def h_knpc():
    f = F(os.path.join(CORE, "KNpc.cpp"))
    # H1: client gate truoc Cost(...)
    old = (T*4 + "if(!IsPlayer() || Cost(pSkill->GetSkillCostType(), pSkill->GetSkillCost(this)))" + NL +
           T*4 + "{" + NL +
           "#ifdef _SERVER" + NL +
           T*5 + "if (IsPlayer() && eStyle != SKILL_SS_Thief && ((KSkill*)pSkill)->GetCostSpKey() > 0)" + T + "// [VHTD 02/09c] B4" + NL)
    new = ("#ifndef _SERVER" + NL +
           T*4 + "// " + MK + " CLIENT: thieu tang No (1976) / Am Luat (2116) -> chan tai cho + thong bao. So tang da dong bo qua s2c_syncvhtd" + NL +
           T*4 + "// (dot 4). Truoc chi server chan im lang -> client van dien hoat / tu luot (Tap Dap Luu Tinh) roi bi keo ve = 'het van dung duoc'." + NL +
           T*4 + "if (IsPlayer() && eStyle != SKILL_SS_Thief && ((KSkill*)pSkill)->GetCostSpKey() > 0 && HS_SpGet(((KSkill*)pSkill)->GetCostSpKey()) < ((KSkill*)pSkill)->GetCostSp())" + NL +
           T*4 + "{" + NL +
           T*5 + "KSystemMessage sMsgSp;" + NL +
           T*5 + "sprintf(sMsgSp.szMessage, \"Kh\\253ng \\256\\361 %d t\\307ng %s\", ((KSkill*)pSkill)->GetCostSp(), (((KSkill*)pSkill)->GetCostSpKey() == 2116) ? \"\\242m Lu\\313t\" : \"N\\351\");" + NL +
           T*5 + "sMsgSp.eType = SMT_NORMAL;" + NL +
           T*5 + "sMsgSp.byConfirmType = SMCT_NONE;" + NL +
           T*5 + "sMsgSp.byPriority = 0;" + NL +
           T*5 + "sMsgSp.byParamSize = 0;" + NL +
           T*5 + "CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsgSp, 0);" + NL +
           T*5 + "goto Exit;" + NL +
           T*4 + "}" + NL +
           "#endif" + NL + old)
    f.rep(old, new, MK + " client cost_sp")
    # H3a: bo khoi phong chieu con o buoc 0 (dot 4) - giu 2 dong m_Doing/break
    old0 = (T*2 + "// [VHTD 02/09g] Tap Dap Luu Tinh 2118: chieu con 2119 ('Khoi' - dan 638 dung yen, khong thuoc tinh) phong tai diem xuat phat huong" + NL +
            T*2 + "// diem den, ca client (ve hieu ung) lan server (vo hai). Cac chieu MoveWithBlur cu (710/732/995) ChildSkillId = 0 -> khong doi." + NL +
            T*2 + "if (pSkill->GetChildSkillId() > 0 && pSkill->GetChildSkillId() < MAX_SKILL)" + NL +
            T*2 + "{" + NL +
            T*3 + "KSkill* pChildBlur = (KSkill*)g_SkillManager.GetSkill(pSkill->GetChildSkillId(), pSkill->GetSkillLevel());" + NL +
            T*3 + "if (pChildBlur)" + NL +
            T*4 + "pChildBlur->Cast(m_Index, m_DesX, m_DesY);" + NL +
            T*2 + "}" + NL +
            T*2 + "m_Doing = do_blurmove;" + NL + T*2 + "break;" + NL + T + "case 1:" + NL)
    new0 = (T*2 + "// " + MK + " chieu con cua chieu luot (2119) nay phong o BUOC 2 (diem den) - xem duoi." + NL +
            T*2 + "m_Doing = do_blurmove;" + NL + T*2 + "break;" + NL + T + "case 1:" + NL)
    f.rep(old0, new0, MK + " bo cast buoc 0")
    # H3b: phong tai diem den (buoc 2, sau khi doi vung/toa do thanh cong)
    old2 = ("#ifndef _SERVER" + NL + T*2 + "m_DataRes.CreateBlur(m_Index, g_GetDistance(nX, nY, m_DesX, m_DesY), m_Dir);" + NL + "#endif" + NL +
            T*2 + "}" + NL + T*2 + "break;" + NL + T + "}" + NL + T + "return TRUE;" + NL + "}" + NL)
    new2 = (T*2 + "// " + MK + " Tap Dap Luu Tinh 2118 (form 13): chieu con 2119 phong TAI DIEM DEN, form 7 (tai nguoi phat) + dan 638 dung yen" + NL +
            T*2 + "// CollidRange/DmgRange 2 (du lieu vhtd_data_patch5) -> hieu ung 'Khoi' + sat thuong dien rong quanh diem den (ca client ve, server tinh)." + NL +
            T*2 + "// Cac chieu MoveWithBlur cu (710/732/995) ChildSkillId = 0 -> khong doi." + NL +
            T*2 + "if (pSkill->GetChildSkillId() > 0 && pSkill->GetChildSkillId() < MAX_SKILL)" + NL +
            T*2 + "{" + NL +
            T*3 + "KSkill* pChildBlur = (KSkill*)g_SkillManager.GetSkill(pSkill->GetChildSkillId(), pSkill->GetSkillLevel());" + NL +
            T*3 + "if (pChildBlur)" + NL +
            T*4 + "pChildBlur->Cast(m_Index, -1, m_Index);" + NL +
            T*2 + "}" + NL +
            "#ifndef _SERVER" + NL + T*2 + "m_DataRes.CreateBlur(m_Index, g_GetDistance(nX, nY, m_DesX, m_DesY), m_Dir);" + NL + "#endif" + NL +
            T*2 + "}" + NL + T*2 + "break;" + NL + T + "}" + NL + T + "return TRUE;" + NL + "}" + NL)
    f.rep(old2, new2, MK + " cast tai diem den")
    f.save()

# ------------------------------------------------------------------ H2 CoreShell.cpp
def h_coreshell():
    f = F(os.path.join(CORE, "CoreShell.cpp"))
    old = "if(pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles || pOrdinSkill->GetSkillStyle() == SKILL_SS_InitiativeNpcState)"
    new = ("if((pOrdinSkill->GetSkillStyle() == SKILL_SS_Missles || pOrdinSkill->GetSkillStyle() == SKILL_SS_InitiativeNpcState) && pNode->m_LeftTime != 0)" +
           T + "// " + MK + " node 0 giay (thuoc tinh tuc thoi 2117 Am Luat...) = rac, khong ve icon '00s'")
    f.rep(old, new, MK + " bo node 0 giay", count=2)
    f.save()

# ------------------------------------------------------------------ H4 KSkills.cpp tooltip
def h_kskills():
    f = F(os.path.join(CORE, "KSkills.cpp"))
    old = (T + "int nGetCost = GetSkillCost(NULL);" + NL + NL + T + "if (nGetCost && !bAddSkillDamage)" + NL)
    new = (T + "// " + MK + " lightingdamage_p (cam Tieu Dao 2136/2138/2140/2141/2142/2143): so sat thuong Loi tinh theo NOI LUC TOI DA cua chinh minh" + NL +
           T + "// (cong thuc KNpc::AppendSkillEffect dot 4) - tooltip truoc chi co dong '%' nen 'khong hien luc tay'." + NL +
           T + "for (i = 0; i < MAX_MISSLE_DAMAGEATTRIB; i++)" + NL +
           T + "{" + NL +
           T*2 + "if ((DamageAttribs + i)->nAttribType != magic_lightingdamage_p) continue;" + NL +
           T*2 + "KNpc* pMeLoi = &Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex];" + NL +
           T*2 + "int nGocLoi = (int)((__int64)(pMeLoi->m_CurrentManaMax + pMeLoi->m_PhysicsMagic.nValue[0] + pMeLoi->m_CurrentLightMagic.nValue[0]) * (MAX_PERCENT + pMeLoi->m_nHSAddLightMagicP) / MAX_PERCENT);" + NL +
           T*2 + "int nDmgLoi = (int)((__int64)nGocLoi * (DamageAttribs + i)->nValue[0] / MAX_PERCENT);" + NL +
           T*2 + "sprintf(pszInfo, \"S\\270t th\\255\\254ng L\\253i (theo n\\351i l\\371c t\\350i \\256a): %d\", nDmgLoi);" + NL +
           T*2 + "strcat(pszMsg, pszInfo);" + NL +
           T*2 + "strcat(pszMsg, \"\\n\");" + NL +
           T + "}" + NL +
           T + "int nGetCost = GetSkillCost(NULL);" + NL + NL + T + "if (nGetCost && !bAddSkillDamage)" + NL)
    f.rep(old, new, MK + " tooltip Loi")
    f.save()

if __name__ == "__main__":
    print("vhtd_engine_patch5 %s%s" % (MK, " (KIEM)" if KIEM else ""))
    h_knpc(); h_coreshell(); h_kskills()
    print("XONG.")
