# -*- coding: utf-8 -*-
r"""v34 - chep 3 nhi phan sang cay may chu that de chu game test.

    Sources\Core\x64\ServerRelease\CoreServer.dll  ->  bin\server\CoreServer.dll
    Sources\Core\ClientRelease\CoreClient.dll      ->  bin\client\CoreClient.dll
    Sources\S3Client\Release\Game.exe              ->  bin\client\Game.exe

BA DIEU BAT BUOC:

  1. PHAI THA CA BA CUNG LUC. Giao thuc moi (c2s_diceitem=172, s2c_diceitem=214)
     nam o ca hai dau; lech mot ben la hai dau hieu sai do dai goi.

  2. Tien trinh dang chay GIU tep -> khong ghi de duoc. Ta DOI TEN ban cu thanh
     '<ten>.cu_<gio>' roi moi chep ban moi vao. Doi ten thanh cong ngay ca khi tep
     dang bi mo (khac voi xoa/ghi de). Ban cu van con de quay lui.

  3. Chi lam viec nay khi chu game da bao co the khoi dong lai.

Chay:  python v34_chep_vao_may_chu.py          -> chi XEM se lam gi
       python v34_chep_vao_may_chu.py --that   -> lam that
"""
import hashlib
import io
import os
import shutil
import sys
import time

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

DEV = r"D:\GAMEDEVNEW"
LIVE = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE"

CAP = [
    (os.path.join(DEV, r"Sources\Core\x64\ServerRelease\CoreServer.dll"),
     os.path.join(LIVE, r"bin\server\CoreServer.dll"), "may chu"),
    (os.path.join(DEV, r"Sources\Core\ClientRelease\CoreClient.dll"),
     os.path.join(LIVE, r"bin\client\CoreClient.dll"), "client"),
    (os.path.join(DEV, r"Sources\S3Client\Release\Game.exe"),
     os.path.join(LIVE, r"bin\client\Game.exe"), "client"),
]


def md5(p):
    return hashlib.md5(io.open(p, "rb").read()).hexdigest()


def main():
    that = "--that" in sys.argv
    dau = time.strftime("%H%M%S")

    print("=" * 74)
    print("  %s" % ("CHEP THAT" if that else "CHI XEM TRUOC (them --that de lam that)"))
    print("=" * 74)

    # kiem truoc: nguon phai co va phai MOI hon dich
    ok = True
    canlam = []
    for src, dst, ai in CAP:
        if not os.path.isfile(src):
            print("!! THIEU NGUON: %s" % src)
            ok = False
            continue
        t_src = os.path.getmtime(src)
        t_dst = os.path.getmtime(dst) if os.path.isfile(dst) else 0
        # 'dich moi hon nguon' KHONG phai luc nao cung la loi: co the ai do vua
        # chep CHINH ban vua build sang (gio sua doi la gio chep). Neu md5 trung
        # thi coi nhu da xong, khong phai lam gi.
        trung = os.path.isfile(dst) and md5(src) == md5(dst)
        if trung:
            tinhtrang = "DA TRUNG - bo qua"
        elif t_src > t_dst:
            tinhtrang = "nguon MOI hon - se chep"
        else:
            tinhtrang = "!! nguon CU hon dich VA khac noi dung"
            ok = False
        print("  %-8s %-22s nguon %s  dich %s  %s"
              % (ai, os.path.basename(src),
                 time.strftime("%H:%M:%S", time.localtime(t_src)),
                 time.strftime("%H:%M:%S", time.localtime(t_dst)) if t_dst else "(chua co)",
                 tinhtrang))
        canlam.append(not trung)
    if not ok:
        print("\n!! Dung lai - nguon chua san sang. Build lai truoc da.")
        return 2
    if not that:
        print("\n(chua lam gi ca)")
        return 0

    print()
    for k, (src, dst, ai) in enumerate(CAP):
        if k < len(canlam) and not canlam[k]:
            print("   = %-22s da trung, bo qua" % os.path.basename(dst))
            continue
        if os.path.isfile(dst):
            bak = dst + ".cu_" + dau
            try:
                os.rename(dst, bak)
                print("   doi ten ban cu -> %s" % os.path.basename(bak))
            except OSError as e:
                print("!! KHONG doi ten duoc %s: %s" % (dst, e))
                print("   (tep dang bi khoa? dong tro choi / may chu roi chay lai)")
                return 2
        shutil.copy2(src, dst)
        print("   > chep %-22s  md5=%s" % (os.path.basename(dst), md5(dst)[:12]))

    print()
    print("kiem lai: dich co trung nguon khong")
    for src, dst, ai in CAP:
        print("   %-22s %s" % (os.path.basename(dst),
                               "TRUNG" if md5(src) == md5(dst) else "!! LECH"))
    print()
    print("=" * 74)
    print("  DA CHEP XONG. Bay gio phai KHOI DONG LAI may chu va tro choi.")
    print("  Ban cu nam canh do, duoi '.cu_%s' - muon quay lui thi doi ten nguoc." % dau)
    print("=" * 74)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
