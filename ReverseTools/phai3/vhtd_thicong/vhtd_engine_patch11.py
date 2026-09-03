# -*- coding: utf-8 -*-
"""vhtd_engine_patch11.py [VHTD 02/09r] - BONG MO khi di chuyen (Huyen Nhan Van Yen 1358, thuoc tinh walkrunshadow).
GOC: engine JX1 CO DU he ve bong mo theo khung - KNpcRes::Draw (dong ~667) moi khung ha mot anh mo khi m_nBlurState == TRUE,
KNpcBlur::ChangeAlpha lam nhat dan; KNpcRes::SetBlur bat/tat. NHUNG thuoc tinh walkrunshadow chi di den m_WalkRun.nTime
(KNpcAttribModify::WalkRunShadow) roi dong bo qua co STATE_WALKRUN (KNpc.cpp:6526 -> KProtocolProcess.cpp:2368) va KHONG AI DOC
DE VE: ca cay nguon chi co 4 cho nhac m_WalkRun (dat, giam moi khung, dong bo, nhan) - khong mot dong ve nao.
VA: trong KNpc::Activate (client, chay moi khung) khi nhan vat DANG DI/CHAY thi bat/tat bong mo theo m_WalkRun.nTime.
Chi ap khi dang do_walk/do_run nen khong dung toi bong mo cua chieu NeedShadow (dat trong DoSkill) hay cac cho SetBlur(FALSE) san co.
walkrunshadow chi duoc DUY NHAT ky nang 1358 dung trong toan bo skills.txt -> khong chieu co dien nao bi anh huong.
Marker [VHTD 02/09r]. DUNG: python vhtd_engine_patch11.py [--kiem]
"""
import io, sys

KIEM = "--kiem" in sys.argv
P = r"D:\GAMEDEVNEW\Sources\Core\Src\KNpc.cpp"
MK = "[VHTD 02/09r]"
T = "\t"; NL = "\r\n"

def main():
    b = io.open(P, "rb").read(); assert b[:3] != b"\xef\xbb\xbf", "BOM"
    s = b.decode("latin-1")
    if MK in s: print("  [=] KNpc.cpp da co %s" % MK); return
    old = (T*2 + "SubWorld[0].Map2Mps(m_RegionIndex, m_MapX, m_MapY, m_OffX, m_OffY, &nMpsX, &nMpsY);" + NL +
           T*2 + "m_DataRes.SetPos(m_Index, nMpsX, nMpsY, m_Height, FALSE);" + NL +
           T + "}" + NL)
    assert s.count(old) == 1, "neo SetPos: %d lan" % s.count(old)
    new = (old +
           NL +
           T + "// " + MK + " BONG MO khi di chuyen (walkrunshadow - Huyen Nhan Van Yen 1358, ky nang DUY NHAT dung thuoc tinh nay)." + NL +
           T + "// Engine da co du he ve bong mo theo khung (KNpcRes::Draw ha mot anh mo moi khung khi m_nBlurState, KNpcBlur::ChangeAlpha" + NL +
           T + "// lam nhat dan) nhung walkrunshadow chi di den m_WalkRun.nTime roi dong bo co STATE_WALKRUN - KHONG dong nao doc de ve." + NL +
           T + "// Chi cham khi dang di/chay nen khong dung toi bong mo cua chieu NeedShadow (dat trong DoSkill) va cac SetBlur(FALSE) san co." + NL +
           T + "if (m_Doing == do_walk || m_Doing == do_run)" + NL +
           T*2 + "m_DataRes.SetBlur(m_WalkRun.nTime > 0 ? TRUE : FALSE);" + NL)
    s = s.replace(old, new)
    nb = s.encode("latin-1")
    assert sum(1 for c in nb if c >= 0x80) == sum(1 for c in b if c >= 0x80)
    print("  [+] KNpc.cpp Activate: bat bong mo theo m_WalkRun khi di/chay")
    if not KIEM: io.open(P, "wb").write(nb)
    print("  => %s %s" % ("KIEM" if KIEM else "ghi", P))

if __name__ == "__main__":
    print("vhtd_engine_patch11 %s%s" % (MK, " (KIEM)" if KIEM else "")); main(); print("XONG.")
