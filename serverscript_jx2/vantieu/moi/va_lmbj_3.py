#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""va_lmbj_3.py - [VANTIEU 06/09 - KIEM CHUNG] va LOI #5:
item.lua phan phat theo MA VAT PHAM CU (4775 / 4776 / 4778), nhung ban VN da
danh so lai thanh 4204 / 4205 / 4207.

Bang chung (D:\\ServerLinux\\server1\\settings\\item\\004\\magicscript.txt):
    :4764  Khoai Ma Gia Tien  6 1 4204  -> \\script\\event\\longmenbiaoju\\item.lua
    :4765  Kien Bat Kha Toa   6 1 4205  -> \\script\\event\\longmenbiaoju\\item.lua
    :4766  Thiet Xa Mat Bo    6 1 4206  -> \\script\\event\\longmenbiaoju\\item.lua
    :4767  Tieu Xa Di Vi      6 1 4207  -> \\script\\event\\longmenbiaoju\\item.lua
  Trong CA settings\\item cua ban VN KHONG CO vat pham 4775/4776/4778
  (4775 chi ton tai o magicscriptTQ.txt va la mon KHAC han: box_dali.lua).

=> main() cua item.lua khong khop nhanh nao -> ba mon tieu hao 4204/4205/4207
   bam la KHONG CO GI XAY RA; cua hang Tieu Cuc (dialog.lua:336-338 ban theo
   tbProp cua ItemList = 4204/4205/4207) do do ban ra mon vo dung.

Va: doi 6 cho ghi ma cu trong item.lua sang ma VN
  main()              : 4775 -> 4204 | 4776 -> 4205 | 4778 -> 4207
  UseForTongBiaoChe() : 4775 -> 4204 | 4776 -> 4205 | 4778 -> 4207

CHU Y: day la tep NGUON LINUX (khong phai tep tu viet). Co ban luu
.truoc_lmbj3. Neu chu game muon giu ma cu thi cach khac la them 3 hang vat pham
4775/4776/4778 vao magicscript - KHONG lam ca hai.

Chay:  python va_lmbj_3.py         (xem truoc)
       python va_lmbj_3.py --ghi   (ghi that)
"""
import os
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

TEP = os.path.join("D:\\", "GAMEDEVNEW_wt_vantieu", "serverscript_jx2", "vantieu",
                   "lua54", "script", "event", "longmenbiaoju", "item.lua")
GHI = "--ghi" in sys.argv

VA = [
    ('if szGDP == "6,1,4775" then', 'if szGDP == "6,1,4204" then', 1),
    ('elseif szGDP == "6,1,4776" then', 'elseif szGDP == "6,1,4205" then', 1),
    ('elseif szGDP == "6,1,4778" then', 'elseif szGDP == "6,1,4207" then', 1),
    ('if (nP == 4775) then', 'if (nP == 4204) then', 1),
    ('elseif (nP == 4776) then', 'elseif (nP == 4205) then', 1),
    ('elseif (nP == 4778) then', 'elseif (nP == 4207) then', 1),
]


def main():
    with open(TEP, "rb") as f:
        noi = f.read().decode("latin-1")
    ok = True
    for cu, moi, solan in VA:
        n = noi.count(cu)
        print("%-38s -> %-38s  tim thay %d" % (cu, moi, n))
        if n != solan:
            ok = False
            continue
        noi = noi.replace(cu, moi, solan)
    if not ok:
        print("\n!! CO MUC KHONG KHOP - KHONG GHI GI CA.")
        return 1
    if not GHI:
        print("\n(xem truoc - chua ghi. Them --ghi de ghi that)")
        return 0
    luu = TEP + ".truoc_lmbj3"
    if not os.path.exists(luu):
        shutil.copy2(TEP, luu)
    with open(TEP, "wb") as f:
        f.write(noi.encode("latin-1"))
    print("DA GHI %s (ban luu %s)" % (TEP, os.path.basename(luu)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
