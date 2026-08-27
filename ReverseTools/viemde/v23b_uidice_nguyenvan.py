# -*- coding: utf-8 -*-
"""v23b - chep NGUYEN VAN 2 tep giao dien xuc xac tu ban Linux sang client JX1.

Vi sao co ban 'b': ban v23 truoc do da NAN 5 cho Image= sang mot anh khac, vi
cong cu do pak cua toi bao 2 anh DICEITEM "khong ton tai". Ket luan do SAI:
ham bam ten tep cua toi dung so KHONG DAU, trong khi `KPakList::FileNameToId`
duyet bang `const char*` - la CHAR CO DAU tren MSVC. Moi byte GBK >= 0x80 vao
phep nhan phai la so am. Sau khi sua, 714/714 tep ten tieng Trung co that tren
dia deu tra ve DUNG, va ca 3 duong dan anh cua 2 tep ini deu CO trong
`data\\updatejx06.pak` cua chinh client dich.

=> Khong can thay anh, khong can rut anh ra khoi pak. Chep y nguyen la khop
   100% ban goc.
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

GOC = r"D:\ServerLinux\Patch\ui\ui3_1024"
DICH = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client\Ui\Ui3"
# ban sao trong kho kich ban de doi chieu ve sau
KHO = r"D:\GAMEDEVNEW\serverscript_jx2\viemde\client\Ui\Ui3"


def gb(u):
    """ten tep tren dia la byte GBK giu nguyen tung ky tu (latin-1)"""
    return u.encode("gbk").decode("latin-1")


TEP = [gb("投色子") + ".ini", gb("投色子-公用") + ".ini"]


def hien(s):
    try:
        return s.encode("latin-1").decode("gbk")
    except Exception:
        return s


def main():
    for thumuc in (DICH, KHO):
        os.makedirs(thumuc, exist_ok=True)

    for ten in TEP:
        src = os.path.join(GOC, ten)
        if not os.path.isfile(src):
            print("!! thieu ban goc:", hien(src))
            return 1
        raw = io.open(src, "rb").read()

        for thumuc in (DICH, KHO):
            dst = os.path.join(thumuc, ten)
            # giu lai ban da bi v23 nan, phong khi can doi chieu
            if os.path.isfile(dst):
                cu = io.open(dst, "rb").read()
                if cu == raw:
                    print("   = %-24s da dung nguyen ban" % hien(ten))
                    continue
                bak = dst + ".v23_da_nan"
                if not os.path.isfile(bak):
                    shutil.copy2(dst, bak)
            tmp = dst + ".tmp"
            io.open(tmp, "wb").write(raw)
            os.replace(tmp, dst)
            print("   > %-24s %5d B  -> %s" % (hien(ten), len(raw), thumuc))

    # kiem lai: byte phai trung khop tuyet doi
    loi = 0
    for ten in TEP:
        a = io.open(os.path.join(GOC, ten), "rb").read()
        for thumuc in (DICH, KHO):
            b = io.open(os.path.join(thumuc, ten), "rb").read()
            if a != b:
                print("!! LECH:", hien(ten), thumuc)
                loi += 1
    print("\nket qua: %s" % ("CO LOI" if loi else "2 tep khop nguyen van ban Linux"))
    return 1 if loi else 0


if __name__ == "__main__":
    raise SystemExit(main())
