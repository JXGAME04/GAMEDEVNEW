# -*- coding: utf-8 -*-
"""vhtd_engine_patch8.py [VHTD 02/09m] - ky nang SU KIEN (Start/Fly/Collide/Vanish) voi EventSkillLevel = -1 (du lieu VLTK: 1965 -> 1966 vong khien
Thinh Anh De Nhue Lu, 1376 -> 1377): JX1 doi m_nEventSkillLevel > 0 (KSkill::Cast + OnMissleEvent + tooltip) -> khong bao gio phong (log 02/09 14:43:
0 dong 1966/526 ca server lan client) -> "vong tron khong duy tri, nho" (chi con PreCastSpr). VLTK: -1 = cap cua chinh ky nang (nhu ChildSkillLevel -1).
Ap dung cho ky nang thoi VLTK (id >= 1347: Hoa Son 1347+, 3 phai); 23 ky nang co dien (248, 429-438, 567, 569, 580, 899) giu hanh vi cu.
Marker [VHTD 02/09m]. DUNG: python vhtd_engine_patch8.py [--kiem]
"""
import io, os, sys, re

KIEM = "--kiem" in sys.argv
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KSkills.cpp"
MK = "[VHTD 02/09m]"
T = "\t"; NL = "\r\n"

def main():
    b = io.open(P, "rb").read(); assert b[:3] != b"\xef\xbb\xbf"
    s = b.decode("latin-1"); orig = s
    if MK in s: print("  [=] KSkills.cpp da co %s" % MK); return
    # helper dat SAU #include "KCore.h" (include DAU, ngoai moi #ifdef; include cuoi Text.h nam trong #ifndef _SERVER -> server khong thay helper)
    a = "#include \"KCore.h\"" + NL
    assert s.count(a) == 1, "neo include KCore.h"
    helper = ("// " + MK + " EventSkillLevel -1 (VLTK) = cap cua chinh ky nang; JX1 doi > 0 nen su kien Start/Fly/Collide/Vanish KHONG phong" + NL +
              "// (1965 -> 1966 vong khien Thinh Anh De Nhue Lu, 1376 -> 1377). Chi ap cho ky nang thoi VLTK (id >= 1347); ky nang co dien giu cu." + NL +
              "static inline int VhEventLevel(int nEventSkillLevel, int nSkillId, int nSkillLevel)" + NL + "{" + NL +
              T + "if (nEventSkillLevel > 0) return nEventSkillLevel;" + NL +
              T + "if (nSkillId >= 1347 && nSkillLevel > 0 && nSkillLevel < MAX_SKILLLEVEL) return nSkillLevel;" + NL +
              T + "return 0;" + NL + "}" + NL + NL)
    s = s.replace(a, a + NL + helper)
    # Cast: start event
    old = (T + "if (m_bStartEvent && m_nStartSkillId > 0 && m_nEventSkillLevel > 0)" + NL + T + "{" + NL +
           T*2 + "KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nStartSkillId, m_nEventSkillLevel);" + NL)
    new = (T + "int nVhEvLv = VhEventLevel(m_nEventSkillLevel, (int)m_nId, (int)m_ulLevel);" + T + "// " + MK + NL +
           T + "if (m_bStartEvent && m_nStartSkillId > 0 && nVhEvLv > 0)" + NL + T + "{" + NL +
           T*2 + "KSkill * pOrdinSkill = (KSkill *) g_SkillManager.GetSkill(m_nStartSkillId, nVhEvLv);" + NL)
    assert s.count(old) == 1, "neo Cast start"; s = s.replace(old, new)
    # OnMissleEvent: 4 case
    n = 0
    for fl, sk in (("m_bFlyingEvent", "m_nFlySkillId"), ("m_bStartEvent", "m_nStartSkillId"), ("m_bVanishedEvent", "m_nVanishedSkillId"), ("m_bCollideEvent", "m_nCollideSkillId")):
        old = (T*2 + "if (!%s || %s <= 0 || m_nEventSkillLevel <= 0)" % (fl, sk) + NL + T*3 + "return FALSE;" + NL +
               T*2 + "nEventSkillId = %s" % sk)
        new = (T*2 + "if (!%s || %s <= 0 || VhEventLevel(m_nEventSkillLevel, (int)m_nId, (int)m_ulLevel) <= 0)" % (fl, sk) + NL + T*3 + "return FALSE;" + NL +
               T*2 + "nEventSkillId = %s" % sk)
        assert s.count(old) == 1, "neo case " + fl; s = s.replace(old, new); n += 1
    old = T*2 + "nEventSkillLevel = m_nEventSkillLevel;" + NL
    assert s.count(old) == 4, "neo nEventSkillLevel = (%d)" % s.count(old)
    s = s.replace(old, T*2 + "nEventSkillLevel = VhEventLevel(m_nEventSkillLevel, (int)m_nId, (int)m_ulLevel);" + T + "// " + MK + NL)
    # tooltip
    old = T*4 + "KSkill * pTempSkill = (KSkill *) g_SkillManager.GetSkill(m_ImmediateAttribs[i].nValue[2], m_nEventSkillLevel);" + NL
    assert s.count(old) == 1, "neo tooltip 1"; s = s.replace(old, T*4 + "KSkill * pTempSkill = (KSkill *) g_SkillManager.GetSkill(m_ImmediateAttribs[i].nValue[2], VhEventLevel(m_nEventSkillLevel, (int)m_nId, (int)m_ulLevel));" + T + "// " + MK + NL)
    old = (T*3 + "if(m_bStartEvent && m_nStartSkillId && m_nEventSkillLevel)" + NL + T*3 + "{" + NL +
           T*4 + "KSkill * pTempSkill = (KSkill *) g_SkillManager.GetSkill(m_nStartSkillId, m_nEventSkillLevel);" + NL)
    assert s.count(old) == 1, "neo tooltip 2"
    s = s.replace(old, T*3 + "if(m_bStartEvent && m_nStartSkillId && VhEventLevel(m_nEventSkillLevel, (int)m_nId, (int)m_ulLevel))" + T + "// " + MK + NL + T*3 + "{" + NL +
                  T*4 + "KSkill * pTempSkill = (KSkill *) g_SkillManager.GetSkill(m_nStartSkillId, VhEventLevel(m_nEventSkillLevel, (int)m_nId, (int)m_ulLevel));" + NL)
    nb = s.encode("latin-1")
    assert sum(1 for c in nb if c >= 0x80) == sum(1 for c in b if c >= 0x80)
    print("  [+] KSkills.cpp: helper + Cast start + 4 case OnMissleEvent + 2 tooltip")
    if not KIEM: io.open(P, "wb").write(nb)
    print("  => %s %s" % ("KIEM" if KIEM else "ghi", P))

if __name__ == "__main__":
    print("vhtd_engine_patch8 %s%s" % (MK, " (KIEM)" if KIEM else "")); main(); print("XONG.")
