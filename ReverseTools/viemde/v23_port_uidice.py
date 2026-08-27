# -*- coding: utf-8 -*-
"""VIEM DE - port GIAO DIEN CUA SO DO XUC XAC tu ban CN/VNG sang client JX1.

Nguon (tim ra 26/08 sau khi chu game bao soi lai D:\\ServerLinux):
    D:\\ServerLinux\\Patch\\ui\\ui3_1024\\投色子.ini        (mot khung 312x83)
    D:\\ServerLinux\\Patch\\ui\\ui3_1024\\投色子-公用.ini    (khung chua 4 luot)
Chu trong ini DA DICH TIENG VIET san (ban VNG): "Can" / "Tham du nhan" /
"Huy bo nhan" / "Thoi gian con lai %d giay".

Bo va nay:
  1. Chep NGUYEN VAN 2 tep sang client\\Ui\\Ui3\\  (giu tung byte, ke ca ten GBK)
  2. KIEM tung duong dan Image= xem client JX1 co that khong (dia + 37 pak)
  3. Chi nhung duong dan THIEU moi thay bang anh tuong duong CO THAT, va GHI RO
     tung cho thay - khong tu y dong vao cai gi dang chay duoc.

Da do: 2 anh \\Spr\\Ui3\\DICEITEM\\*.spr KHONG co trong cay client nay. Ly do:
Patch la ban VNG MOI HON - 508/617 duong dan anh cua no cung khong co o day.
"""
import io
import os
import re
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

PATCH_UI = r"D:\ServerLinux\Patch\ui\ui3_1024"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
DICH = CLI + r"\Ui\Ui3"
GUONG = r"D:\GAMEDEVNEW\serverscript_jx2\viemde\client\Ui\Ui3"
SEP = "\\"

TEP = ["\u6295\u8272\u5b50.ini", "\u6295\u8272\u5b50-\u516c\u7528.ini"]

# anh thay the: chi dung khi duong dan goc KHONG co. Da kiem la CO THAT.
THAY = {
    # khung hop thoai 312x83  <- dung khung hop thoai cung ho co san cua JX1
    "NPC\u5bf9\u8bdd\u6846_\u4e2d.spr":
        SEP + "Spr" + SEP + "Ui3" + SEP + "NPC\u5bf9\u8bdd\u6761" + SEP
        + "\u6eda\u52a8\u9009\u62e9\u5bf9\u8bdd\u6761-\u5c0f3.spr",
}


def pid(name):
    n = name[1:] if name[:1] in (SEP, "/") else name
    pb = bytes(b + 32 if 65 <= b <= 90 else b for b in (SEP + n).encode("latin-1"))
    i = 0
    idx = 0
    for b in pb:
        idx += 1
        c = b + 32 if 65 <= b <= 90 else b
        i = ((((i + idx * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF) & 0xFFFFFFFF
    return i ^ 0x12345678


def nap_pak():
    s = set()
    root = CLI + SEP + "data"
    for f in sorted(os.listdir(root)):
        if not f.lower().endswith(".pak"):
            continue
        fh = open(os.path.join(root, f), "rb")
        head = fh.read(32)
        if head[:4] != b"PACK":
            fh.close()
            continue
        cnt, itoff, _ = struct.unpack_from("<III", head, 4)
        fh.seek(itoff)
        raw = fh.read(16 * cnt)
        fh.close()
        for k in range(cnt):
            s.add(struct.unpack_from("<I", raw, 16 * k)[0])
    return s


def gb(u):
    return u.encode("gbk").decode("latin-1")


def show(s):
    try:
        return s.encode("latin-1").decode("gbk")
    except Exception:
        return s


def co_that(v, pak):
    return pid(v) in pak or os.path.isfile(CLI + ("" if v.startswith(SEP) else SEP) + v)


def main():
    pak = nap_pak()
    print("pak client: %d muc" % len(pak))
    thay_l1 = {gb(k): gb(v) if any(ord(c) > 127 for c in v) else v for k, v in THAY.items()}
    # chuan hoa: khoa la TEN TEP (byte gbk), gia tri la duong dan day du (byte gbk)
    thay_l1 = {}
    for k, v in THAY.items():
        thay_l1[gb(k)] = "".join(gb(c) if ord(c) > 127 else c for c in v)

    for t in TEP:
        src = os.path.join(PATCH_UI, gb(t))
        if not os.path.isfile(src):
            print("!! khong thay nguon: %s" % t)
            continue
        d = open(src, "rb").read().decode("latin-1")
        print()
        print("=== %s (%d byte) ===" % (t, len(d)))
        doi = 0
        thieu_khac = []

        def sua(m):
            nonlocal doi
            v = m.group(1).strip()
            if not v.lower().endswith(".spr"):
                return m.group(0)
            if co_that(v, pak):
                return m.group(0)
            ten = v.split(SEP)[-1]
            if ten in thay_l1:
                moi = thay_l1[ten]
                doi += 1
                print("   THAY: %s" % show(v))
                print("      -> %s" % show(moi))
                return m.group(0).replace(v, moi)
            thieu_khac.append(v)
            return m.group(0)

        d2 = re.sub(r"(?mi)^\s*Image\s*=\s*(.+?)\s*$", sua, d)
        for v in sorted(set(thieu_khac)):
            print("   !! THIEU va CHUA CO ANH THAY: %s" % show(v))
        for goc in (DICH, GUONG):
            os.makedirs(goc, exist_ok=True)
            p = os.path.join(goc, gb(t))
            tam = p + ".dangghi"
            with open(tam, "wb") as f:
                f.write(d2.encode("latin-1"))
            os.replace(tam, p)
        print("   da chep sang %s  (%d cho thay anh)" % (DICH, doi))

    print()
    print("=== KIEM LAI ===")
    for t in TEP:
        p = os.path.join(DICH, gb(t))
        print("   %-24s %s  %d byte" % (t, os.path.isfile(p),
                                        os.path.getsize(p) if os.path.isfile(p) else 0))
        if os.path.isfile(p):
            d = open(p, "rb").read().decode("latin-1")
            for m in re.finditer(r"(?mi)^\s*Image\s*=\s*(.+?)\s*$", d):
                v = m.group(1).strip()
                print("        Image=%-46s %s" % (show(v), "CO" if co_that(v, pak) else "<< VAN THIEU"))


main()
