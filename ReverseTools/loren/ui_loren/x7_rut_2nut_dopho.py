# -*- coding: utf-8 -*-
"""x7_rut_2nut_dopho.py - Rut hai ANH NUT con thieu cua trang Do pho.

`Dopho.ini` doi:
    [AtlasBtn]   Image=\\Spr\\Ui3\\<lo ren>\\<dung hop>.spr   (nut "Che tao")
    [PreviewBtn] Image=\\Spr\\Ui3\\<lo ren>\\<xem truoc>.spr  (nut "Xem truoc ti le")
Hai tep nay KHONG co trong client (thu muc spr cua he lo ren moi co 18 tep, gom
16 tep goc + 2 anh nen vua rut o w2). Thieu anh thi nut khong ve duoc dung.

Dung lai duong rut da nghiem thu o w2: pakdump.name2id (KHONG tu viet lai ham
bam) + giai nen UCL NRV2B, va rut kem MOT tep DA CO san lam doi chieu.

Mac dinh chi DO va IN; --ghi moi rut va dat vao client.
"""
import io
import os
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump as P                                    # noqa: E402
import ucl                                             # noqa: E402

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

ROOT = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"
DST = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
       r"\spr\Ui3\\" + "\xd6\xfd\xd4\xec\xd7\xb0\xb1\xb8")
BS = chr(92)
THUMUC = "\xd6\xfd\xd4\xec\xd7\xb0\xb1\xb8"        # <lo ren>
DUNGHOP = "\xc8\xda\xba\xcf"                        # <dung hop>  (nut Che tao)
XEMTRUOC = "\xd4\xa4\xc0\xc0"                       # <xem truoc>
DOITCHIEU = "\xb4\xf2\xd4\xec"                      # <duc>  - DA CO san, de doi chieu

CAN = [
    ("nut CHE TAO  (THIEU)", DUNGHOP + ".spr"),
    ("nut XEM TRUOC(THIEU)", XEMTRUOC + ".spr"),
    ("nut duc     (co roi)", DOITCHIEU + ".spr"),
]


def blob_of(f, e):
    u, off, size, cf = e
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    f.seek(off)
    raw = f.read(cs)
    return raw if fl == 0 else ucl.nrv2b_decompress_8(raw, size)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== x7_rut_2nut_dopho - %s ===" % ("GHI THAT" if ghi else "CHI DO"))
    if not os.path.isdir(ROOT):
        print("!!! khong thay client VLTK: %s" % ROOT)
        return 1

    ids = {}
    for nhan, ten in CAN:
        duong = BS + "spr" + BS + "Ui3" + BS + THUMUC + BS + ten
        ids[P.name2id(duong)] = (nhan, ten, duong)

    thay = {}
    for tenpak in sorted(p for p in os.listdir(ROOT) if p.lower().endswith(".pak")):
        try:
            f, es = P.entries(os.path.join(ROOT, tenpak))
        except Exception:
            continue
        try:
            for e in es:
                if e[0] in ids:
                    try:
                        thay.setdefault(e[0], []).append((tenpak, blob_of(f, e)))
                    except Exception as ex:
                        print("   ! %s: giai nen hong (%s)" % (tenpak, ex))
        finally:
            f.close()

    ok = 0
    for nhan, ten in CAN:
        duong = BS + "spr" + BS + "Ui3" + BS + THUMUC + BS + ten
        ds = thay.get(P.name2id(duong), [])
        try:
            hien = ten.encode("latin-1").decode("gbk")
        except Exception:
            hien = ten
        print("  %-22s %s" % (nhan, hien))
        if not ds:
            print("        -> KHONG THAY trong pak nao")
            continue
        ok += 1
        for tenpak, b in ds:
            print("        -> %-22s %7d byte" % (tenpak, len(b)))
        if ghi and "co roi" not in nhan:
            dst = os.path.join(DST, ten)
            if os.path.isfile(dst):
                print("        DA CO trong client - bo qua")
                continue
            with io.open(dst, "wb") as g:
                g.write(ds[-1][1])
            print("        DAT vao client (%d byte)" % os.path.getsize(dst))
        if "co roi" in nhan:
            # doi chieu byte voi ban da co
            cli = os.path.join(DST, ten)
            if os.path.isfile(cli):
                same = io.open(cli, "rb").read() == ds[-1][1]
                print("        doi chieu voi ban trong client: %s"
                      % ("TRUNG BYTE - duong rut CHUAN" if same else "KHAC - xem lai"))

    print("\n  tim thay %d/%d." % (ok, len(CAN)))
    if not ghi:
        print("  (chay lai voi --ghi de dat vao client)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
