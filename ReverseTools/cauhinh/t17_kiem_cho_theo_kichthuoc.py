# -*- coding: utf-8 -*-
"""t17_kiem_cho_theo_kichthuoc.py - vá nốt lỗ hong cuoi cua cong trao thuong.

LOI (bo phan bien neu, toi da TU DOC ma xac nhan tung mat xich):
  ScriptFuns.cpp:4997-5006 - trong LuaAddItem, khi tui KHONG nhet vua mon moi:
      if (CheckCanPlaceInEquipment(w, h, &x, &y)) { AddKIL(...); }
      else {
          int nIdx = m_ItemList.Hand();
          if (nIdx) { m_ItemList.Remove(nIdx);  ... ObjSet.Add(...) ... }
      }
  Tuc may LAY MON DANG CAM TREN TAY, GO RA va NEM XUONG DAT de lay cho dat mon
  moi len tay. Mon roi xuong dat co han song - het gio la mat han.
  Va ham VAN tra ve chi so > 0, nen phep kiem gia tri tra ve (vua them o t11)
  KHONG phan biet duoc "vao tui" voi "len tay + hat mon khac xuong dat".

  Phep kiem tui cu goi `CalcFreeItemCellCount()` khong tham so - doc
  ScriptFuns.cpp:5981-5987 thi thay no chi doc w/h khi co TREN 2 tham so, nen
  goi tran la dem so o trong 1x1 RAI RAC. No khong noi duoc mon 1x3 (moi thanh
  vu khi) co nhet vua hay khong.

MIENG VA: dung `CheckRoom(w, h, 1)` (ScriptFuns.cpp LuaCheckRoom - voi tu 3
  tham so tro len thi lay w,h truyen vao, goi SearchPosition va tra 1/0) de hoi
  DUNG cau "mon w x h co cho dat khong" TRUOC tung lan trao.

  Muc thuong nay co the khai kich thuoc:
      { <so luong>, {g,d,p}, "<ten>", <cap do>, <rong>, <cao> }
  Khong khai thi coi la 1x1 - dung cho moi vat pham xep chong (thuoc, ngoc,
  nguyen lieu). Trang bi thi PHAI khai, vi do la loai lam mat do tren tay.

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import shutil
import sys

sys.path.insert(0, os.path.join(r"D:\GAMEDEVNEW\ReverseTools", "viemde"))
from bangtxt import uni2tcvn  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

P = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
     r"\script\cauhinh\ch_thuong_lib.lua")
NHAN = "[CHOTRONG 29/08]"

# ---- doi ham G_TraoMotMon: them w,h + CheckRoom ----
CU_HAM = """function G_TraoMotMon(nGenre, nDetail, nParticular, nSoLuong, nCapDo, szTen,
\t\tszNguon)
\tlocal nSL = nSoLuong
\tif (nSL == nil or nSL < 1) then
\t\tnSL = 1
\tend
\tlocal nCap = nCapDo
\tif (nCap == nil) then
\t\tnCap = 0
\tend
\tlocal szT = G_Chuoi(szTen)
\tlocal szN = G_Chuoi(szNguon, "?")
"""

MOI_HAM = """function G_TraoMotMon(nGenre, nDetail, nParticular, nSoLuong, nCapDo, szTen,
\t\tszNguon, nRong, nCao)
\tlocal nSL = nSoLuong
\tif (nSL == nil or nSL < 1) then
\t\tnSL = 1
\tend
\tlocal nCap = nCapDo
\tif (nCap == nil) then
\t\tnCap = 0
\tend
\tlocal szT = G_Chuoi(szTen)
\tlocal szN = G_Chuoi(szNguon, "?")

\t-- %(nhan)s HOI DUNG CAU "mon nay co cho dat khong" TRUOC KHI TRAO.
\t-- Ly do that: neu tui khong nhet vua, AddItem KHONG bao loi ma LAY MON
\t-- DANG CAM TREN TAY, go ra va NEM XUONG DAT (ScriptFuns.cpp:5001-5006) de
\t-- lay cho dat mon moi len tay. Mon roi xuong dat co han song - het gio la
\t-- mat han. Va ham VAN tra ve chi so > 0 nen kiem tri tra ve khong phat hien
\t-- duoc. CheckRoom(w, h, 1) hoi SearchPosition nen tra loi dung.
\tlocal nW = nRong
\tlocal nH = nCao
\tif (nW == nil or nW < 1) then
\t\tnW = 1
\tend
\tif (nH == nil or nH < 1) then
\t\tnH = 1
\tend
\tif (CheckRoom ~= nil) then
\t\tif (CheckRoom(nW, nH, 1) ~= 1) then
\t\t\tG_LogThuong("KHONG DU CHO ("..nW.."x"..nH.."): "..szN.." | "..szT)
\t\t\tMsg2Player("Hành trang không đủ chỗ cho "..szT..".")
\t\t\treturn 0
\t\tend
\tend
""" % dict(nhan=NHAN)

# ---- truyen w,h tu bang thuong ----
CU_GOI = """\t\t\t\tnDaTrao = nDaTrao + G_TraoMotMon(tbMa[1], tbMa[2], tbMa[3],
\t\t\t\t\ttbMuc[1], tbMuc[4], tbMuc[3], szN.." | "..szKhoa)"""
MOI_GOI = """\t\t\t\t-- %(nhan)s muc [5], [6] la RONG x CAO cua mon (mac dinh 1x1).
\t\t\t\t-- Trang bi PHAI khai - do la loai lam mat do tren tay khi thieu cho.
\t\t\t\tnDaTrao = nDaTrao + G_TraoMotMon(tbMa[1], tbMa[2], tbMa[3],
\t\t\t\t\ttbMuc[1], tbMuc[4], tbMuc[3], szN.." | "..szKhoa,
\t\t\t\t\ttbMuc[5], tbMuc[6])""" % dict(nhan=NHAN)

# ---- G_TraoMon le: them w,h ----
# LUU Y: chuoi tieng Viet o day phai viet BANG KY TU THAT (Unicode) roi cho
# uni2tcvn doi - viet "\\u00e0" la Python hieu thanh backslash-u nen moc khong
# bao gio khop.
CU_LE = """function G_TraoMon(nGenre, nDetail, nParticular, nSoLuong, szTen, szNguon)
\tlocal szN = G_Chuoi(szNguon, "?")
\tlocal nOTrong = CalcFreeItemCellCount()
\tif (nOTrong ~= nil and nOTrong < 1) then
\t\tG_LogThuong("TUI DAY khi trao "..G_Chuoi(szTen).." (nguon "..szN..")")
\t\tMsg2Player("Hành trang đã đầy, không"
\t\t\t.." nhận được phần thưởng.")
\t\treturn 0
\tend
\treturn G_TraoMotMon(nGenre, nDetail, nParticular, nSoLuong, 0, szTen, szN)
end"""
MOI_LE = """function G_TraoMon(nGenre, nDetail, nParticular, nSoLuong, szTen, szNguon,
\t\tnRong, nCao)
\t-- %(nhan)s phep kiem cho nam trong G_TraoMotMon (CheckRoom theo dung kich
\t-- thuoc mon), khong con dem o 1x1 rai rac o day nua.
\treturn G_TraoMotMon(nGenre, nDetail, nParticular, nSoLuong, 0, szTen,
\t\tszNguon, nRong, nCao)
end""" % dict(nhan=NHAN)

# ---- sua chu thich rao so bo cua G_TraoThuong cho dung su that ----
CU_RAO = """\t-- [PHANBIEN 29/08] Kiem so o trong TRUOC khi trao. Luu y that: goi khong tham so
\t-- thi CalcFreeItemCellCount chi DEM SO O 1x1 CON TRONG, RAI RAC
\t-- (ScriptFuns.cpp:5981 -> KInventory::FindFreeCell(1,1)). No KHONG bao dam
\t-- mon to (1x3, 2x3) nhet vua. Vi vay day chi la RAO SO BO de tranh truong
\t-- hop tui day han - khong hua chac chan."""
MOI_RAO = """\t-- %(nhan)s Rao SO BO: dem o trong 1x1 de loai truong hop tui day han.
\t-- Phep kiem THAT SU nam trong G_TraoMotMon - no goi CheckRoom voi dung
\t-- kich thuoc tung mon, nen mon 1x3 hay 2x3 cung duoc hoi dung cau.""" % dict(nhan=NHAN)


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t17_kiem_cho_theo_kichthuoc - %s ==="
          % ("GHI THAT" if ghi else "DIEN TAP"))
    raw = doc(P)
    if NHAN in raw:
        print("  DA VA - bo qua")
        return 0

    nd = raw
    for ten, cu_u, moi_u in (
        ("ham G_TraoMotMon (them w,h + CheckRoom)", CU_HAM, MOI_HAM),
        ("cho goi tu bang thuong", CU_GOI, MOI_GOI),
        ("ham G_TraoMon le", CU_LE, MOI_LE),
        ("chu thich rao so bo", CU_RAO, MOI_RAO),
    ):
        cu = uni2tcvn(cu_u).replace("\n", "\r\n")
        moi = uni2tcvn(moi_u).replace("\n", "\r\n")
        n = nd.count(cu)
        if n != 1:
            print("!!! LOI TO: %s - moc khop %d lan (can 1)" % (ten, n))
            return 1
        nd = nd.replace(cu, moi)
        print("  %s" % ten)

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    sao = P + ".truoc_chotrong"
    if not os.path.isfile(sao):
        shutil.copy2(P, sao)
    with io.open(P, "wb") as f:
        f.write(nd.encode("latin-1"))
    if doc(P) != nd:
        print("!!! LOI TO: doc lai KHONG khop")
        return 1
    print("  DA GHI ch_thuong_lib.lua")
    return 0


if __name__ == "__main__":
    sys.exit(main())
