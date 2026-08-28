# -*- coding: utf-8 -*-
"""w2_rut_nen_2the.py - Rut ANH NEN cua hai the con thieu tu pak VLTK.

Client JX1 hien co nen cua Duc / Che tao / Trich lay:
    \\spr\\Ui3\\<lo ren>\\phan trang da tao .spr   (3 tep "phan trang ...")
nhung THIEU nen cua hai the:
    phan trang DO PHO   -> Dopho.ini doi (Image=...\\phan trang do pho.spr)
    phan trang KHAM NAM -> nen the Kham nam phai muon bo cuc Trich lay
Vi vay the Do pho khong the ve duoc, va the Kham nam dang muon bo cuc.

Ten trong pak la GBK, KHONG truyen qua dong lenh (Git Bash/argv lam hong byte);
o day dung THANG chuoi byte latin-1 nen chac chan dung.

Bay da biet, tuan thu het:
  - Nen la UCL NRV2B, KHONG phai zlib (ex1/pakdump.read_entry dung zlib -> ra
    RAC CO LAN CHU DOC DUOC, de tuong nham la sai bang ma).
  - name2id phai duyet bang `char` CO DAU (c > 127 -> c -= 256); dung so khong
    dau thi MOI ten tieng Trung bam sai.
  - Cung mot duong dan nam trong NHIEU pak voi noi dung KHAC NHAU; pak nap SAU
    de len pak nap truoc. Thu tu nap theo ten tep, va `update__x` sap SAU
    `update04__x` (dau `_` > chu so) - nen KHONG duoc lay sorted()[-1] mot cach
    hon nhien. O day in RA HET moi ban tim thay kem ten pak de chon co can nhac.

Mac dinh chi DO va IN; --ghi moi ghi ra thu muc ra_nen2the.
"""
import io
import os
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump as P                                    # noqa: E402
import ucl                                             # noqa: E402

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

ROOT = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"
OUT = r"D:\GAMEDEVNEW\ReverseTools\loren\ui_loren\ra_nen2the"

BS = chr(92)
# GBK: 铸造装备 (lo ren) / 分页图谱 (phan trang do pho) / 分页镶嵌 (phan trang kham nam)
THUMUC = "\xd6\xfd\xd4\xec\xd7\xb0\xb1\xb8"
DOPHO = "\xb7\xd6\xd2\xb3\xcd\xbc\xc6\xd7"
KHAMNAM = "\xb7\xd6\xd2\xb3\xcf\xe2\xc7\xb6"
# doi chieu: mot tep DA CO trong client, de tu kiem tra duong tra cuu la dung
DOITCHIEU = "\xb7\xd6\xd2\xb3\xb4\xf2\xd4\xec"        # 分页打造 (phan trang duc)

CAN = [
    ("do pho  (THIEU)", BS + "spr" + BS + "Ui3" + BS + THUMUC + BS + DOPHO + ".spr"),
    ("kham nam(THIEU)", BS + "spr" + BS + "Ui3" + BS + THUMUC + BS + KHAMNAM + ".spr"),
    ("duc     (co roi)", BS + "spr" + BS + "Ui3" + BS + THUMUC + BS + DOITCHIEU + ".spr"),
]


# Bam ten: DUNG THANG pakdump.name2id, khong tu viet lai.
# (Lan dau toi tu viet mot ham bam kieu h*131+c -> tra 0/3, KE CA tep chac chan
#  co san trong client => biet ngay la duong tra cuu sai chu khong phai pak thieu.
#  Cong thuc that: uid = ((uid + idx*c) % 0x8000000B) * 0xFFFFFFEF, cuoi cung
#  XOR 0x12345678, chu hoa ha thanh chu thuong, char CO DAU.)
name2id = P.name2id


def blob_of(f, e):
    u, off, size, cf = e
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    f.seek(off)
    raw = f.read(cs)
    if fl == 0:
        return raw
    return ucl.nrv2b_decompress_8(raw, size)


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== w2_rut_nen_2the - %s ===" % ("GHI THAT" if ghi else "CHI DO"))
    if not os.path.isdir(ROOT):
        print("!!! LOI TO: khong thay client VLTK: %s" % ROOT)
        return 1

    paks = sorted(p for p in os.listdir(ROOT) if p.lower().endswith(".pak"))
    print("  %d pak trong %s" % (len(paks), ROOT))

    ids = {}
    for nhan, duong in CAN:
        ids[name2id(duong)] = (nhan, duong)

    thay = {}          # id -> [(ten_pak, blob)]
    for tenpak in paks:
        p = os.path.join(ROOT, tenpak)
        try:
            f, es = P.entries(p)
        except Exception:
            continue
        try:
            for e in es:
                if e[0] in ids:
                    try:
                        b = blob_of(f, e)
                    except Exception as ex:
                        print("   ! %s: giai nen hong (%s)" % (tenpak, ex))
                        continue
                    thay.setdefault(e[0], []).append((tenpak, b))
        finally:
            f.close()

    print()
    ok = 0
    for nhan, duong in CAN:
        i = name2id(duong)
        ds = thay.get(i, [])
        try:
            hienthi = duong.encode("latin-1").decode("gbk")
        except Exception:
            hienthi = duong
        print("  %-16s %s" % (nhan, hienthi))
        if not ds:
            print("        -> KHONG THAY trong pak nao")
            continue
        ok += 1
        for tenpak, b in ds:
            print("        -> %-24s %8d byte  %s"
                  % (tenpak, len(b), "SPR hop le" if b[:4] in (b"SPR\x00", b"SPRI") or b[:2] == b"SP" else "dau tep: %r" % b[:4]))
        if ghi:
            if not os.path.isdir(OUT):
                os.makedirs(OUT)
            for tenpak, b in ds:
                ten = "%s__%s" % (os.path.splitext(tenpak)[0],
                                  os.path.basename(duong))
                dst = os.path.join(OUT, ten)
                with io.open(dst, "wb") as g:
                    g.write(b)
                print("        GHI %s" % ten)

    print("\n  tim thay %d/%d duong dan." % (ok, len(CAN)))
    if not ghi:
        print("  (chay lai voi --ghi de rut ra %s)" % OUT)
    return 0


if __name__ == "__main__":
    sys.exit(main())
