# -*- coding: utf-8 -*-
"""Do goc %s cua duong ini thu: doc ma may quanh xref, brute-force ten voi ident tu exe,
kiem tra sprite hop thu trong pak."""
import sys, os, re, struct
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump

BS = "\\"
HERE = os.path.dirname(os.path.abspath(__file__))
img = open(os.path.join(HERE, "gamecl_img.bin"), "rb").read()
base = 0x1000
ib = 0x400000


def off_of(va):
    return va - ib - base


print("=== ma may truoc xref mail_manager / mail_list ===")
for va in (0x575F68, 0x5736D8):
    o = off_of(va)
    seg = img[o - 64:o + 24]
    print(" VA %08X:" % (va - 64), seg.hex(" "))
    for m in re.finditer(rb"[\xa1\x8b\xff\x68\xb8\xb9\xba\xbb\xbe\xbf\x05\x0d\x15\x1d\x35](....)", seg):
        v = struct.unpack("<I", m.group(1))[0]
        if ib + base <= v < ib + base + len(img):
            oo = off_of(v)
            s = img[oo:oo + 48]
            mm = re.match(rb"[\x20-\x7e]{1,48}", s)
            print("    op %02x -> %08X : %r" % (m.group()[0], v, (mm.group() if mm else s[:16])))

print("=== brute-force root = " + BS + "ui" + BS + "<ident> ===")
root = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\data"
idx = {}
for fn in os.listdir(root):
    if not fn.endswith(".pak"):
        continue
    f = open(os.path.join(root, fn), "rb")
    sig, count, ioff, doff = struct.unpack("<IIII", f.read(16))
    f.seek(ioff)
    raw = f.read(count * 16)
    for i in range(count):
        uid, off, size, cf = struct.unpack_from("<IIiI", raw, i * 16)
        idx.setdefault(uid, []).append((fn, size, cf >> 24))

idents = set()
for m in re.finditer(rb"[A-Za-z0-9_]{2,24}", img):
    idents.add(m.group().decode().lower())
for m in re.finditer(rb"[\x20-\x7e]{3,120}", img):
    s = m.group().decode("latin-1")
    if BS in s:
        for part in s.split(BS):
            if 2 <= len(part) <= 24 and re.match(r"^[a-z0-9_]+$", part.lower()):
                idents.add(part.lower())
print("  so ident:", len(idents))
want = {0xCDBBBE0F: "icon(750)", 0xE772B550: "icon(990)"}
hits = []
for idn in idents:
    for p in (
        BS + "ui" + BS + idn + BS + "mail" + BS + "mail_icon.ini",
        BS + idn + BS + "mail" + BS + "mail_icon.ini",
        BS + "ui" + BS + "ui3" + BS + idn + BS + "mail" + BS + "mail_icon.ini",
        BS + "ui" + BS + idn + BS + "mail_icon.ini",
        BS + "ui" + BS + "mail" + BS + idn + ".ini",
        BS + "ui" + BS + "mail" + BS + "mail_icon_" + idn + ".ini",
        BS + "ui" + BS + "mail" + BS + "mail_icon" + idn + ".ini",
        BS + "ui" + BS + "mail" + BS + idn + BS + "mail_icon.ini",
        BS + "ui" + BS + idn + BS + "mail" + BS + "mail_icon_1024.ini",
        BS + "ui" + BS + idn + BS + "mail" + BS + "mail_icon_1280.ini",
        idn + BS + "mail" + BS + "mail_icon.ini",
        BS + "ui" + BS + idn + BS + "email" + BS + "mail_icon.ini",
    ):
        h = pakdump.name2id(p)
        if h in want:
            hits.append((want[h], p))
for h in hits:
    print("  HIT", h)

print("=== sprite hop thu trong pak? ===")
for nm in ["邮箱图标", "银两图标"]:
    s = (BS + "spr" + BS + "ui4" + BS + "email" + BS).encode("latin-1") + nm.encode("gbk") + b".spr"
    h = pakdump.name2id(s.decode("latin-1"))
    print("  %08X" % h, s, idx.get(h))
