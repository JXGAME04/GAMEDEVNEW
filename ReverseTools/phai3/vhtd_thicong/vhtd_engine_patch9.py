# -*- coding: utf-8 -*-
"""vhtd_engine_patch9.py [VHTD 02/09n] - KICH NO khi truong (Hoa Son: Than Quang Toan Nhieu 1384 candetonate1/2 = 419*256 / 428*256
-> no Ma Van Kiem Khi 419 / Kiem Khi Vo Cap 428 trong ban kinh): KNpc::DetonateMissles so `Missle[i].m_nMissleId == nStyle` nhung
m_nMissleId la CHI SO instance (KMissleSet::Add ghi de) -> khong bao gio bang kieu dan 419 -> khong kich no (chu 02/09: "dung tiep
Than Quang Toan Nhieu se kich no Ma Van Kiem Khi trong pham vi"). Cung bay voi khoi va dot 4 (m_nMissleId >= 500).
Vá: so KIEU dan qua ky nang tao dan (KSkill::GetChildSkillId cua Missle[i].m_nSkillId/m_nLevel). Marker [VHTD 02/09n].
DUNG: python vhtd_engine_patch9.py [--kiem]
"""
import io, sys

KIEM = "--kiem" in sys.argv
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KNpc.cpp"
MK = "[VHTD 02/09n]"
T = "\t"; NL = "\r\n"

def main():
    b = io.open(P, "rb").read(); assert b[:3] != b"\xef\xbb\xbf", "BOM"
    s = b.decode("latin-1")
    if MK in s: print("  [=] KNpc.cpp da co %s" % MK); return
    old = (T*3 + "if (nIdx > 0 && nIdx < MAX_MISSLE && Missle[nIdx].m_nMissleId == nStyle" + NL)
    assert s.count(old) == 1, "neo DetonateMissles (%d)" % s.count(old)
    new = (T*3 + "// " + MK + " m_nMissleId = CHI SO instance (KMissleSet::Add ghi de), khong phai kieu dan -> so kieu qua ky nang tao dan (ChildSkillId)" + NL +
           T*3 + "KSkill* pVhMs = (nIdx > 0 && nIdx < MAX_MISSLE) ? (KSkill*)g_SkillManager.GetSkill(Missle[nIdx].m_nSkillId, Missle[nIdx].m_nLevel) : NULL;" + NL +
           T*3 + "if (nIdx > 0 && nIdx < MAX_MISSLE && pVhMs && pVhMs->GetChildSkillId() == nStyle" + NL)
    s = s.replace(old, new)
    nb = s.encode("latin-1"); assert sum(1 for c in nb if c >= 0x80) == sum(1 for c in b if c >= 0x80)
    print("  [+] KNpc.cpp DetonateMissles: so kieu dan qua ChildSkillId")
    if not KIEM: io.open(P, "wb").write(nb)
    print("  => %s %s" % ("KIEM" if KIEM else "ghi", P))

if __name__ == "__main__":
    print("vhtd_engine_patch9 %s%s" % (MK, " (KIEM)" if KIEM else "")); main(); print("XONG.")
