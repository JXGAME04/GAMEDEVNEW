# -*- coding: utf-8 -*-
"""b5b_sua_nanma_sai.py -- sua 6 ma da bi buoc "nan ma" truoc do ghi SAI vao cay lua54.

Buoc nan ma chay luc 12:59 dung bang 34 ma cu, chon dong JX1 theo SPRITE GIONG LINUX.
Do la tieu chi sai: o JX1 dong "giong Linux" thuong la ban Trung Quoc cu DA CHET, da bi
thay bang dong tieng Viet ma he thong song thuc su tieu thu. Bang chung tep:dong:

  6,1,3203 Ho Mach Don    3821 -> 4844
      script/player/setmeridian.lua:21   KM_ITEM_HMD = 4844
      KM_DemItem()/DelItem() chi dem 4844 -> phat 3821 la phat vien VO DUNG.
  6,1,3308 Chan Nguyen Dan 3926 -> 4752
      script/item/event/kinhmach/channguyendan.lua  KM_CND_GIATRI[4752]=10
      (cung bang voi [4846]=5 va [4847]=10 ma 30228/30229 anh xa toi).
  6,1,3455 Trai tao       4073 -> 4876
  6,1,3456 Mia            4074 -> 4877
  6,1,3457 Bap            4075 -> 4878
  6,1,3458 Khoai lang     4076 -> 4879
      script/petsys/feed.lua:3-6  ITEM_APPLE/MAIZE/SWEET_POTATO/SUGARCANE = 4876..4879.
      Dong 4073-4076 co cung sprite nhung Script=0, khong he thong nao dung.

Chay:
  set PYTHONIOENCODING=utf-8
  python b5b_sua_nanma_sai.py           # chay thu, chi in ra
  python b5b_sua_nanma_sai.py --that    # ghi that (giu ban luu *.truoc_suananma)
"""
import os, re, sys, shutil

ROOT = r"D:\GAMEDEVNEW_wt_vantieu\serverscript_jx2\vantieu\lua54"

SUA = {
    "3821": ("4844", "setmeridian.lua:21 KM_ITEM_HMD = 4844"),
    "3926": ("4752", "kinhmach/channguyendan.lua KM_CND_GIATRI[4752]"),
    "4073": ("4876", "petsys/feed.lua:3 ITEM_APPLE = 4876"),
    "4074": ("4877", "petsys/feed.lua:6 ITEM_SUGARCANE = 4877"),
    "4075": ("4878", "petsys/feed.lua:4 ITEM_MAIZE = 4878"),
    "4076": ("4879", "petsys/feed.lua:5 ITEM_SWEET_POTATO = 4879"),
}


def chay(that):
    # chi bat dung dang "6,1,<ma>" (co the co khoang trang), khong dung toi so khac
    rx = re.compile(r"(6\s*,\s*1\s*,\s*)(%s)(?![0-9])" % "|".join(SUA))
    tong = 0
    for dp, dn, fn in os.walk(ROOT):
        for f in sorted(fn):
            if not f.lower().endswith(".lua"):
                continue
            p = os.path.join(dp, f)
            d = open(p, "rb").read().decode("latin-1")     # latin-1: giu nguyen byte TCVN3
            hit = []

            def thay(m):
                hit.append((m.group(2), SUA[m.group(2)][0]))
                return m.group(1) + SUA[m.group(2)][0]

            d2 = rx.sub(thay, d)
            if not hit:
                continue
            tong += len(hit)
            print("%s : %d cho  %s" % (os.path.relpath(p, ROOT), len(hit),
                                       ", ".join("%s->%s" % h for h in hit)))
            if that:
                b = p + ".truoc_suananma"
                if not os.path.exists(b):
                    shutil.copy2(p, b)
                open(p, "wb").write(d2.encode("latin-1"))
    print("\ntong %d cho%s" % (tong, "" if that else "  (CHAY THU -- chua ghi gi)"))
    if tong == 0:
        print("khong con ma sai nao -- co the da sua roi")


if __name__ == "__main__":
    chay("--that" in sys.argv)
