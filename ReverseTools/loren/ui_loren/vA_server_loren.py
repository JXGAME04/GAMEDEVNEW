# -*- coding: utf-8 -*-
"""
vA_server_loren.py - va 3 loi phia MAY CHU cua he UI lo ren 7 khung (JX1).

    python vA_server_loren.py          -> DIEN TAP (khong ghi gi)
    python vA_server_loren.py --ghi    -> ghi that (tao sao luu .truoc_uiloren2)

MIENG 1 (do tim/dup nguyen lieu xep chong) - 4 tep Lua (2 cay):
    compound_header.lua : removeItems() goi RemoveItemByIndex 1 doi = tru 1 DON VI,
    ore_upgrade.lua     : nhung ITEM_CalcItemValue lai NHAN gia tri theo CA CHONG
                          (KItemCompound.cpp:918-935). Nguoi choi tra gia bang 1 vien
                          ma huong gia tri ca chong -> them doi thu hai
                          GetItemStackCount(idx) de an dung so da tinh tien.

MIENG 2 (tieu nguyen lieu khong kiem do KHOA) - KItemCompound.cpp:
    duong tieu removeItems -> RemoveItemByIndex -> KItemList::RemoveItemIdx KHONG
    doc co khoa, do khoa lot vao o nguyen lieu bi huy vinh vien. Them chot bo qua
    ngay trong vong loc, dung chinh khuon guard cua duong ban/giao dich.

MIENG 3 (cua so lo ren khong bao gio dong phia may chu) - KProtocolProcess.cpp:
    LuaEndCompoundItem (ScriptFuns.cpp:10314) xoa m_dwCompoundItemId nhung KHONG
    kich ban nao goi -> id song den luc relog, nut lo ren van bam duoc khi da roi
    NPC. Dat lai 0 tren duong c2s_recoveritem giong het cach tremble lam o dong
    ngay tren.

LUAT:
  - doc/ghi bang io.open rb/wb + latin-1 (tep TCVN3/GBK, cam encoding khac).
  - moi moc neo phai xuat hien DUNG 1 LAN; lech -> in "LOI TO" va KHONG ghi gi
    (kiem het moi tep TRUOC, chi ghi khi ca luot deu sach).
  - idempotent: chay lan 2 bao "DA CO - bo qua".
  - giu nguyen kieu xuong dong cua tung tep (KItemCompound.cpp la LF-only,
    5 tep con lai CRLF) - moc neo duoc rap bang dung EOL cua chinh tep do.
"""

import io
import os
import shutil
import sys

BAKEXT = ".truoc_uiloren2"
GHI = "--ghi" in sys.argv[1:]


class LoiTo(Exception):
    pass


# ---------------------------------------------------------------------------
# Tien ich doc/ghi giu nguyen byte
# ---------------------------------------------------------------------------
def doc(p):
    if not os.path.isfile(p):
        raise LoiTo("khong thay tep: %s" % p)
    return io.open(p, "rb").read().decode("latin-1")


def ghi_dia(p, s):
    io.open(p, "wb").write(s.encode("latin-1"))


def dem_eol(s):
    crlf = s.count("\r\n")
    lf = s.count("\n")
    return crlf, lf


def eol_cua(s):
    """EOL chu dao cua tep. Tep tron thi van tra ve cai chu dao - moc neo rap
    bang EOL nay se khong khop (dem = 0) va tu no thanh LOI TO, khong am tham."""
    crlf, lf = dem_eol(s)
    return "\r\n" if crlf * 2 >= lf else "\n"


# ---------------------------------------------------------------------------
# Dinh nghia 3 mieng
#   cu  : danh sach DONG (khong kem EOL) - rap lai bang EOL cua chinh tep
#   moi : danh sach DONG thay the
#   dau : chuoi nhan dien "da va roi" (phai nam trong 'moi', khong nam trong 'cu')
# ---------------------------------------------------------------------------

E_LUA = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\item\compound"
D_LUA = r"D:\GAMEDEVNEW\ReverseTools\loren\ra\lua\item\compound"

M1A_CU = [
    "\t\t\tRemoveItemByIndex( arynItemIdx[i] );",
]
M1A_MOI = [
    "\t\t\t-- [UILOREN 27/08] an CA CHONG, khong phai 1 vien: ITEM_CalcItemValue",
    "\t\t\t-- nhan gia tri theo so luong chong (KItemCompound.cpp:918-935) nen",
    "\t\t\t-- tru 1 don vi la nguoi choi huong gia tri ca chong ma chi mat 1 vien.",
    "\t\t\tRemoveItemByIndex( arynItemIdx[i], GetItemStackCount( arynItemIdx[i] ) );",
]

M1B_CU = [
    "\tRemoveItemByIndex( arynNecessaryItemIdx[1] );",
]
M1B_MOI = [
    "\t-- [UILOREN 27/08] an CA CHONG - cung ly do voi removeItems() trong",
    "\t-- compound_header.lua (ITEM_CalcItemValue nhan theo so luong chong).",
    "\tRemoveItemByIndex( arynNecessaryItemIdx[1], GetItemStackCount( arynNecessaryItemIdx[1] ) );",
]

M2_CU = [
    "\t\t\t\tif (pList->FindSame(nIdx) == 0)\t\t// khong phai do cua minh",
    "\t\t\t\t\tcontinue;",
]
M2_MOI = [
    "\t\t\t\tif (pList->FindSame(nIdx) == 0)\t\t// khong phai do cua minh",
    "\t\t\t\t\tcontinue;",
    "\t\t\t\t// [UILOREN 27/08] SKIP LOCKED ITEMS.",
    "\t\t\t\t// The consume path (removeItems -> RemoveItemByIndex ->",
    "\t\t\t\t// KItemList::RemoveItemIdx) never reads the lock flag, so a locked",
    "\t\t\t\t// item dropped into a forge slot would be destroyed for good.",
    "\t\t\t\t// Same guard the trade / drop paths use: KBuySell.cpp:366-368 and",
    "\t\t\t\t// CoreShell.cpp:3659-3660. The -2 / -3 literals are",
    "\t\t\t\t// LOCK_STATE_FOREVER / LOCK_STATE_CHARACTER (GameDataDef.h:292-293),",
    "\t\t\t\t// written as numbers the way every other call site here does.",
    "\t\t\t\t// Dropping the item from the list (instead of failing outright) is",
    "\t\t\t\t// the safe direction: a required material that vanishes makes",
    "\t\t\t\t// g_FoundryResDemand::Check refuse the craft, and nothing is eaten.",
    "\t\t\t\tif (Item[nIdx].GetPlayerItemLock() > 0 ||",
    "\t\t\t\t\tItem[nIdx].GetPlayerItemHLock() > 0 ||",
    "\t\t\t\t\tItem[nIdx].GetPlayerItemLock() == -2 ||",
    "\t\t\t\t\tItem[nIdx].GetPlayerItemLock() == -3)",
    "\t\t\t\t\tcontinue;",
]

M3_CU = [
    "\tPlayer[nIndex].m_ItemList.RecoverItem(pRecoverItem->m_nPos);",
    "\tPlayer[nIndex].m_dwTrembleItemId = 0;",
]
M3_MOI = [
    "\tPlayer[nIndex].m_ItemList.RecoverItem(pRecoverItem->m_nPos);",
    "\tPlayer[nIndex].m_dwTrembleItemId = 0;",
    "\t// [UILOREN 27/08] close the forge session server side, the way the line",
    "\t// above closes the tremble one. LuaEndCompoundItem (ScriptFuns.cpp:10314)",
    "\t// clears m_dwCompoundItemId but no script ever calls it, so the id used to",
    "\t// survive until the player slot was reused at relog (KPlayer.cpp:321) and",
    "\t// the forge buttons (case 7, KProtocolProcess.cpp:6509) kept firing long",
    "\t// after the player had walked away from the NPC.",
    "\t// Safe against the multi packet close: KItemList::RecoverItem",
    "\t// (KItemList.cpp:5813) switches on nPos alone and never reads",
    "\t// m_dwCompoundItemId, so the remaining per room packets still recover.",
    "\tif (pRecoverItem->m_nPos >= pos_compone && pRecoverItem->m_nPos <= pos_enchase)",
    "\t\tPlayer[nIndex].m_dwCompoundItemId = 0;",
]

MIENG = [
    ("MIENG 1  dup nguyen lieu xep chong (compound_header.lua)", [
        os.path.join(E_LUA, "compound_header.lua"),
        os.path.join(D_LUA, "compound_header.lua"),
    ], M1A_CU, M1A_MOI, "[UILOREN 27/08] an CA CHONG, khong phai 1 vien"),

    ("MIENG 1  dup nguyen lieu xep chong (ore_upgrade.lua)", [
        os.path.join(E_LUA, "ore_upgrade.lua"),
        os.path.join(D_LUA, "ore_upgrade.lua"),
    ], M1B_CU, M1B_MOI, "[UILOREN 27/08] an CA CHONG - cung ly do"),

    ("MIENG 2  bo qua nguyen lieu dang KHOA (KItemCompound.cpp)", [
        r"D:\GAMEDEVNEW\Sources\Core\Src\KItemCompound.cpp",
    ], M2_CU, M2_MOI, "[UILOREN 27/08] SKIP LOCKED ITEMS"),

    # MIENG 3 DA HOAN 27/08 - KHONG AP. Ly do (to va client doc that):
    #   ca 6 pad gui GOI_RECOVER_ITEM tu nut "Huy bo" MA KHONG dong cua so
    #   (UiCompoundItem.cpp:869, 1346, 1901, 2484, 2866, 3661). Dat
    #   m_dwCompoundItemId = 0 tren duong c2s_recoveritem thi bam Huy bo MOT lan
    #   la cong "if (m_dwCompoundItemId > 0)" (KProtocolProcess.cpp:6510) khoa,
    #   moi cu bam ep sau do IM LANG khong phan hoi cho toi khi noi chuyen lai NPC.
    #   Huong dung cho dot sau: them callback dong cua so goi EndCompoundItem,
    #   hoac reset theo khoang cach NPC / doi ban do. Giu M3_CU/M3_MOI de dung lai.
]


# ---------------------------------------------------------------------------
def lap_ke_hoach():
    """Kiem het moi tep. Tra ve danh sach viec ghi. Bat ky lech nao -> LoiTo."""
    viec = []
    boqua = []
    for ten, dstep, cu_l, moi_l, dau in MIENG:
        print("")
        print("-" * 74)
        print(ten)
        print("-" * 74)
        for p in dstep:
            noidung = doc(p)
            eol = eol_cua(noidung)
            crlf0, lf0 = dem_eol(noidung)
            ten_eol = "CRLF" if eol == "\r\n" else "LF"

            if dau in noidung:
                print("  DA CO - bo qua   %s" % p)
                print("                   EOL=%s CRLF=%d LF=%d" % (ten_eol, crlf0, lf0))
                boqua.append(p)
                continue

            cu = eol.join(cu_l)
            moi = eol.join(moi_l)
            n = noidung.count(cu)
            print("  moc neo trung %d lan   %s" % (n, p))
            if n != 1:
                raise LoiTo(
                    "moc neo trung %d lan (can DUNG 1) trong %s\n"
                    "         moc: %r" % (n, p, cu_l[0])
                )

            moi_nd = noidung.replace(cu, moi, 1)
            crlf1, lf1 = dem_eol(moi_nd)
            them = len(moi_l) - len(cu_l)
            print("                   EOL=%s   CRLF %d -> %d   LF %d -> %d   (+%d dong)"
                  % (ten_eol, crlf0, crlf1, lf0, lf1, them))

            # chot an toan: chi duoc them dong, khong duoc doi kieu xuong dong
            if eol == "\r\n" and crlf1 != lf1:
                raise LoiTo("ghi hong kieu xuong dong CRLF o %s" % p)
            if eol == "\n" and crlf1 != 0:
                raise LoiTo("lot CRLF vao tep LF-only %s" % p)
            if crlf1 - crlf0 != them and eol == "\r\n":
                raise LoiTo("so dong them khong khop o %s" % p)

            viec.append((p, moi_nd))
    return viec, boqua


def main():
    print("=" * 74)
    print("vA_server_loren.py - %s" % ("GHI THAT" if GHI else "DIEN TAP (khong ghi gi)"))
    print("=" * 74)

    try:
        viec, boqua = lap_ke_hoach()
    except LoiTo as e:
        print("")
        print("!" * 74)
        print("LOI TO: %s" % e)
        print("KHONG GHI GI CA - dung lai toan bo.")
        print("!" * 74)
        return 2

    print("")
    print("=" * 74)
    print("Ke hoach: %d tep can va, %d tep da co san." % (len(viec), len(boqua)))
    for p, _ in viec:
        print("   sua  %s" % p)

    if not viec:
        print("Khong con gi de lam.")
        return 0

    if not GHI:
        print("")
        print("DIEN TAP - chua dong vao dia. Chay lai voi --ghi de ap that.")
        return 0

    for p, moi_nd in viec:
        bak = p + BAKEXT
        if not os.path.exists(bak):
            shutil.copyfile(p, bak)
            print("   sao luu -> %s" % bak)
        ghi_dia(p, moi_nd)
        print("   DA GHI  %s" % p)

    print("")
    print("Xong. Nho build lai Core (KItemCompound.cpp + KProtocolProcess.cpp).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
