# -*- coding: ascii -*-
"""hs_engine_patch4b.py - HOA SON dot f, sua theo phan bien (02/09):
  (1) BO cong 'bHSTrung' (Linux 0x0808AAC7 khong gate theo ket qua BeHurt: [ebp-0x48] la do dich khang ngu hanh, khong phai ket qua don) -> tu phong
      moi don qua cua trung (CheckHitTarget), chi gate quan he DICH. Tra 5 loi goi CalcDamage ve nguyen ban.
  (2) CastAutoSkillAt: dat hoi chieu server nhu KNpc::Cast(int,int) (client s2cDirectlyCastSkill luon SetNextCastTime -> dong nhat).
  (3) so sanh hoi chieu: ban khi now >= nextTime (Linux 0x08188EB8 jb), thay '>=' bang '>'.
Chay sau hs_engine_patch4.py. Idempotent. DUNG: python hs_engine_patch4b.py [--kiem]
"""
import io, os, sys

ROOT = r"D:\GAMEDEVNEW\Sources\Core\Src"
KIEM = "--kiem" in sys.argv
p = os.path.join(ROOT, "KNpc.cpp")
s = io.open(p, "r", encoding="latin-1", newline="").read()
orig = s
hib = lambda t: sum(1 for c in t if ord(c) >= 0x80)

def rep(old, new, tag, count=1):
    global s
    c = s.count(old)
    if c == 0:
        if new in s:
            print("  [=] da ap: %s" % tag); return
        raise SystemExit("KHONG THAY anchor (%s):\n%s" % (tag, old[:160]))
    if c != count:
        raise SystemExit("anchor khong duy nhat (%d) (%s)" % (c, tag))
    s = s.replace(old, new)
    print("  [+] %s" % tag)

# (1) bo bHSTrung
rep("\tKMagicAttrib *pTemp = NULL;\r\n"
    "\tBOOL bHSTrung = FALSE;\t// [HOASON 02/09] co it nhat mot he gay sat thuong (Linux 0x0808AAC7: ket qua BeHurt >= 0) -> moi tung tu phong\r\n",
    "\tKMagicAttrib *pTemp = NULL;\r\n", "bo khai bao bHSTrung")
rep("\tbHSTrung |= CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_physics, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, nStolenLifeP, nStolenManaP, nStolenStaminaP, bIsDS, FALSE, nTotalAvg);\t// [HOASON 02/09]",
    "\tCalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_physics, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, nStolenLifeP, nStolenManaP, nStolenStaminaP, bIsDS, FALSE, nTotalAvg);", "RD physics nguyen ban")
rep("\tBOOL bHSCold = CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_cold, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, FALSE, nTotalAvg);\t// [HOASON 02/09]\r\n"
    "\tbHSTrung |= bHSCold;\r\n"
    "\tif (bHSCold)\r\n",
    "\tif (CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_cold, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, FALSE, nTotalAvg))\r\n", "RD cold nguyen ban")
rep("\tbHSTrung |= CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_fire, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, FALSE, nTotalAvg);\t// [HOASON 02/09]",
    "\tCalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_fire, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, FALSE, nTotalAvg);", "RD fire nguyen ban")
rep("\tbHSTrung |= CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_light, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, FALSE, nTotalAvg);\t// [HOASON 02/09]",
    "\tCalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_light, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE, FALSE, nTotalAvg);", "RD light nguyen ban")
rep("\tBOOL bHSPoison = CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_poison, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE);\t// [HOASON 02/09]\r\n"
    "\tbHSTrung |= bHSPoison;\r\n"
    "\tif (bHSPoison)\r\n",
    "\tif (CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_poison, nMissleSeries, bIsPhysical, bIsMelee, FALSE, nFiveElementsDamageP, 0, 0, 0, FALSE))\r\n", "RD poison nguyen ban")
rep("\t// [HOASON 02/09] TU PHONG CHUAN LINUX ReceiveDamage 0x0808AACF-0x0808B1D8: sau khi tru mau, CHI khi quan he la DICH\r\n"
    "\t// ([ebp+0x24] & 0xC == 8) va don co sat thuong: (1) NAN NHAN ban autoreplyskill (+0x1850) nham ke danh/minh,\r\n"
    "\t// (2) KE DANH ban autoattackskill (+0x1874) nham nan nhan/minh. Moi don MOT lan. Hai bang cong 'khong ban lai'\r\n"
    "\t// (0x8fc62a0/0x8fc4360) cua Linux khong bao gio duoc dien (vector 0x8fbfe04/0x8fbfe10 khong co noi ghi) -> luon mo.\r\n"
    "\tif (bHSTrung && nLauncher > 0 && nLauncher < MAX_NPC && Npc[nLauncher].m_Index > 0\r\n",
    "\t// [HOASON 02/09] TU PHONG CHUAN LINUX ReceiveDamage 0x0808AACF-0x0808B1D8: sau khi tru mau, CHI khi quan he la DICH\r\n"
    "\t// ([ebp+0x24] & 0xC == 8): (1) NAN NHAN ban autoreplyskill (+0x1850) nham ke danh/minh, (2) KE DANH ban autoattackskill\r\n"
    "\t// (+0x1874) nham nan nhan/minh. Moi don qua cua trung (CheckHitTarget) MOT lan, KHONG gate theo sat thuong tung he\r\n"
    "\t// (phan bien 02/09: [ebp-0x48] Linux la do dich khang ngu hanh, khong phai ket qua don). Hai bang cong 'khong ban lai'\r\n"
    "\t// (0x8fc62a0/0x8fc4360) cua Linux khong bao gio duoc dien (vector 0x8fbfe04/0x8fbfe10 khong co noi ghi) -> luon mo.\r\n"
    "\tif (nLauncher > 0 && nLauncher < MAX_NPC && Npc[nLauncher].m_Index > 0\r\n", "RD gate chi quan he dich")

# (2) hoi chieu server trong CastAutoSkillAt (giong KNpc::Cast(int,int))
rep("\tpSkill->Cast(m_Index, -1, nTarget);\r\n}\r\n",
    "\tpSkill->Cast(m_Index, -1, nTarget);\r\n"
    "\t// hoi chieu server nhu KNpc::Cast(int,int): client s2cDirectlyCastSkill luon SetNextCastTime cho nguoi phat -> dong nhat hai ben\r\n"
    "\tif (!pSkill->IsAura())\r\n"
    "\t{\r\n"
    "\t\tDWORD dwCastTime = 0;\r\n"
    "\t\teSkillStyle eStyle = (eSkillStyle)pSkill->GetSkillStyle();\r\n"
    "\t\tif (eStyle == SKILL_SS_Missles || eStyle == SKILL_SS_Melee || eStyle == SKILL_SS_InitiativeNpcState || eStyle == SKILL_SS_PassivityNpcState)\r\n"
    "\t\t\tdwCastTime = pSkill->GetDelayPerCast(m_bRideHorse);\r\n"
    "\t\telse if (eStyle == SKILL_SS_Thief)\r\n"
    "\t\t\tdwCastTime = ((KThiefSkill*)pSkill)->GetDelayPerCast();\r\n"
    "\t\tm_SkillList.SetNextCastTime(nSkillId, SubWorld[m_SubWorldIndex].m_dwCurrentTime, SubWorld[m_SubWorldIndex].m_dwCurrentTime + dwCastTime);\r\n"
    "\t}\r\n"
    "}\r\n", "CastAutoSkillAt SetNextCastTime")

# (3) now >= nextTime thi ban (Linux jb) : 3 cho
rep("\t\tif (rA.dwNextCastTime >= SubWorld[m_SubWorldIndex].m_dwCurrentTime)\r\n\t\t\tcontinue;\r\n",
    "\t\tif (rA.dwNextCastTime > SubWorld[m_SubWorldIndex].m_dwCurrentTime)\t// Linux 0x08188EB8: now < nextTime -> bo qua\r\n\t\t\tcontinue;\r\n",
    "hoi chieu >= -> >", count=3)

if s == orig:
    print("(khong doi)")
else:
    if hib(s) != hib(orig) or "\xef\xbf\xbd" in s:
        raise SystemExit("LECH BYTE CAO / EF BF BD")
    if not KIEM:
        io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("=> ghi KNpc.cpp%s" % (" (KIEM - khong ghi)" if KIEM else ""))
print("XONG%s." % (" (KIEM)" if KIEM else ""))
