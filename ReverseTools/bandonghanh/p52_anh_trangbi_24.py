# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] Anh 6 trang bi pet TRAN O: do that cho thay MOI item pet
ban goc deu 24x24 (fuyuanlu_small 24x24, wulincaiquan_red 24x23, yumi 24x24,
obj_item_lection02 24x24) trong khi anh trang bi that (obj-sword01...) la
anh nhieu o -> tran. Rut 6 anh trang bi tu pak VLTK (nguon hop le), thu ve
24x24, dat \spr\item\petequip\, gan vao 6 dong item 4881..4886.
"""
import io
import os
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\bandonghanh")
import pakdump as P  # noqa: E402
import ucl  # noqa: E402

CR = chr(13)
BS = chr(92)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
VL = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"

GOC = [r"\spr\item\equip\closeweapon\obj-sword01.spr",
       r"\spr\item\equip\cap\obj-ma-cap01-1.spr",
       r"\spr\item\equip\armor\obj-ma-cloth01-1.spr",
       r"\spr\item\equip\sash\obj-sash01.spr",
       r"\spr\item\equip\boots\obj-shoes01.spr",
       r"\spr\item\equip\nick\obj_neck01.spr"]
TEN = ["pet_weapon.spr", "pet_cap.spr", "pet_armor.spr",
       "pet_sash.spr", "pet_boots.spr", "pet_neck.spr"]

THU_TU = ["1024.pak", "serverlist.pak", "slistcache.pak"] + \
    ["updatejx%02d.pak" % i for i in range(17, 0, -1)] + \
    ["updatejx07_jxf06.pak", "update02.pak", "update.pak", "update04.pak",
     "update03.pak", "update01.pak", "spr.pak", "resource.pak"]


def blob(f, e):
    u, off, size, cf = e
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    if fl == 0x20:
        fl = 1
    fl &= 0x0F
    f.seek(off)
    raw = f.read(cs if cs else size)
    try:
        if fl == 0 or cs in (0, size):
            return raw[:size] if size > 0 else raw
        if fl == 1:
            return ucl.nrv2b_decompress_8(raw, size)
        import bz2
        import zlib
        return bz2.decompress(raw) if fl == 2 else zlib.decompress(raw)
    except Exception:
        return b""


tam = r"D:\GAMEDEVNEW\ReverseTools\bandonghanh\ra_pet\eqspr"
os.makedirs(tam, exist_ok=True)
ids = {P.name2id(g): (g, TEN[i]) for i, g in enumerate(GOC)}
rut = {}
for pak in THU_TU:
    p = os.path.join(VL, pak)
    if not os.path.exists(p) or not ids:
        continue
    f, es = P.entries(p)
    for e in es:
        if e[0] in ids:
            g, ten = ids.pop(e[0])
            d = blob(f, e)
            if d[:3] == b"SPR":
                open(os.path.join(tam, ten), "wb").write(d)
                rut[ten] = g
    f.close()
print("rut tu VLTK:", len(rut), "| thieu:", [v[1] for v in ids.values()])

import p15_vietsub_spr as p15  # noqa: E402
from PIL import Image  # noqa: E402

ra_dir = os.path.join(CL, "spr", "item", "petequip")
os.makedirs(ra_dir, exist_ok=True)
xong = {}
for ten in TEN:
    src = os.path.join(tam, ten)
    if not os.path.exists(src):
        continue
    sp = p15.Spr(src)
    im = sp.imgs[0]
    bb = im.getbbox()
    if bb:
        im = im.crop(bb)
    r = min(24 / im.width, 24 / im.height)
    im = im.resize((max(1, int(im.width * r)), max(1, int(im.height * r))), Image.LANCZOS)
    cv = Image.new("RGBA", (24, 24), (0, 0, 0, 0))
    cv.paste(im, ((24 - im.width) // 2, (24 - im.height) // 2), im)
    sp.w = sp.h = 24
    sp.frames = 1
    sp.dirs = 1
    sp.imgs = [cv]
    sp.path = os.path.join(ra_dir, ten)
    if not os.path.exists(sp.path):
        open(sp.path, "wb").write(b"")
    bak = sp.path + ".truoc_vietsub"
    if os.path.exists(bak):
        os.remove(bak)
    sp.save()
    if os.path.exists(bak):
        os.remove(bak)
    xong[ten] = BS + "spr" + BS + "item" + BS + "petequip" + BS + ten
print("sinh 24x24:", len(xong))

for pth in (SV + r"\settings\item\magicscript.txt", CL + r"\settings\item\magicscript.txt"):
    s = io.open(pth, "r", encoding="latin-1", newline="").read()
    lf = s.replace(CR + "\n", "\n")
    rows = [d.split("\t") for d in lf.split("\n") if d.strip()]
    n = 0
    for c in rows:
        if len(c) > 4 and c[3] in [str(x) for x in range(4881, 4887)]:
            k = int(c[3]) - 4881
            if TEN[k] in xong:
                c[4] = xong[TEN[k]]
                n += 1
    io.open(pth, "w", encoding="latin-1", newline="").write(
        ("\n".join("\t".join(c) for c in rows) + "\n").replace("\n", CR + "\n"))
    print("gan anh 24x24:", n, "dong |", pth[:2])
print("XONG p52")
