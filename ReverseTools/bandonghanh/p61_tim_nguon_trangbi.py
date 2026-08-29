# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] Tim NGUON THAT cua he trang bi pet trong pak VLTK:
- ini cua so duc lai da co: 720a151f.ini "Duc lai trang bi Dong Hanh"
  (tieu hao "Dong Hanh Ket Tinh x%d", 2 danh sach thuoc tinh cu/moi,
  nut Tu bo / Giu lai / Trang bi tay luyen, o objEquip)
- item da tim: 5063 Ket Tinh Dong Hanh, 5064/65/66 Ruong trang bi Dong Hanh,
  5067 Chia khoa ruong
Con thieu: BANG dinh nghia trang bi + script logic. Do thu nhieu duong dan.
"""
import glob
import os
import sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump as P  # noqa: E402
import ucl  # noqa: E402

BS = chr(92)
VL = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"
RA = r"D:\GAMEDEVNEW\ReverseTools\bandonghanh\ra_pet"


def d(*parts):
    return BS + BS.join(parts)


cands = []
for thu in ("settings", d("settings", "petsys")[1:], d("settings", "item")[1:]):
    for f in ("companionequip.txt", "petequip.txt", "equip.txt", "equip_def.txt",
              "companion_equip.txt", "pet_equip.txt", "equipattrib.txt",
              "companionequip_def.txt", "equiprebuild.txt", "tongbanequip.txt"):
        cands.append(BS + thu + BS + f)
for f in ("companionequip.lua", "equip.lua", "equip_c.lua", "companionequip_c.lua",
          "ui_equip.lua", "rebuild.lua", "equip_gs.lua", "dataload_equip.lua"):
    cands.append(d("script", "petsys", f))
for f in ("companionequip.lua", "companionequip_c.lua"):
    cands.append(d("script", "ui", f))
    cands.append(d("script", "global", f))

ids = {P.name2id(c): c for c in cands}
print("do", len(ids), "duong dan...")


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


thay = 0
for pak in sorted(glob.glob(VL + r"\*.pak")):
    try:
        f, es = P.entries(pak)
    except Exception:
        continue
    for e in es:
        if e[0] in ids:
            duong = ids[e[0]]
            data = blob(f, e)
            ten = "vltk_eq_" + duong.split(BS)[-1]
            open(os.path.join(RA, ten), "wb").write(data)
            print("CO:", duong, "|", os.path.basename(pak), len(data), "->", ten)
            thay += 1
    f.close()
print("tim thay:", thay)
