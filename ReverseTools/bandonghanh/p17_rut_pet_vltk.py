# -*- coding: utf-8 -*-
r"""[BDH 28/08] Rut TRON bo "Ban Dong Hanh" ban PC tu client VLTK Level Up.

Quet moi entry <= 400KB trong cac pak theo THU TU package.ini (index nho thang),
lay moi entry TEXT chua mot trong cac dau hieu:
  - duong anh "spr\Ui3\pet"  (bo ini/lua cua he)
  - "DONG HANH" TCVN3 / "hao huu" TCVN3
Ghi ra ra_pet/<id>.<ext> (bo qua id da lay o pak uu tien cao hon).
Sau do gom moi "Image=\spr\..." trong cac ini da rut -> danh sach anh can rut them.
"""
import os
import re
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
import pakdump as P  # noqa: E402
import ucl  # noqa: E402
from vn_edit import vn  # noqa: E402

RA = os.path.join(os.path.dirname(os.path.abspath(__file__)), "ra_pet")
os.makedirs(RA, exist_ok=True)
ROOT = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"

THU_TU = ["1024.pak", "serverlist.pak", "slistcache.pak"] + \
    ["updatejx%02d.pak" % i for i in range(17, 0, -1)] + \
    ["updatejx07_jxf06.pak", "update02.pak", "update.pak", "update04.pak",
     "update03.pak", "update01.pak", "skills.pak", "spr.pak", "resource.pak",
     "maps.pak", "settings.pak", "ui.pak", "script.pak", "slistfree.pak"]


def blob_of(f, e):
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


DAU_HIEU = [b"spr" + b"\x5c" + b"Ui3" + b"\x5c" + b"pet",
            b"spr" + b"\x5c" + b"ui3" + b"\x5c" + b"pet",
            vn("Đồng Hành").encode("latin-1"),
            vn("hảo hữu").encode("latin-1"),
            vn("đồng hành").encode("latin-1")]


def ext_cua(data):
    h = data[:64]
    if h[:3] == b"SPR":
        return "spr"
    t = data[:400]
    if b"function" in t or b"local " in t or t.startswith(b"--"):
        return "lua"
    if b"[Main]" in data[:2000] or t.startswith(b"[") or t.startswith(b";"):
        return "ini"
    if b"\t" in t[:200]:
        return "txt"
    return "bin"


da_lay = set()
n = 0
for pkname in THU_TU:
    pk = os.path.join(ROOT, pkname)
    if not os.path.isfile(pk):
        continue
    try:
        f, es = P.entries(pk)
    except Exception:
        continue
    for e in es:
        if e[0] in da_lay or e[2] <= 0 or e[2] > 400_000:
            continue
        data = blob_of(f, e)
        if len(data) < 8 or data[:3] == b"SPR":
            continue
        if any(d in data for d in DAU_HIEU):
            ext = ext_cua(data)
            open(os.path.join(RA, "%08x.%s" % (e[0], ext)), "wb").write(data)
            da_lay.add(e[0])
            n += 1
    f.close()
    print("quet xong", pkname, "- tong file:", n)

print("=== da rut", n, "entry text ===")

# gom duong anh tu cac ini/lua da rut
anh = set()
for fn in os.listdir(RA):
    if not fn.endswith((".ini", ".lua", ".txt")):
        continue
    data = open(os.path.join(RA, fn), "rb").read().decode("latin-1", "replace")
    for m in re.findall(r"[\x5c/](?:spr|Spr|SPR)[\x5c/][^\r\n\"';,]+?\.spr", data):
        anh.add(m.replace("/", "\x5c"))
open(os.path.join(RA, "_danh_sach_anh.txt"), "w", encoding="latin-1").write(
    "\n".join(sorted(anh)))
print("duong anh tim thay trong ini/lua:", len(anh))
for a in sorted(anh)[:40]:
    try:
        print("  ", a.encode("latin-1").decode("gbk"))
    except Exception:
        print("  ", a)
