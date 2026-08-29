# -*- coding: utf-8 -*-
r"""[PETSYS 28/08] Rut anh + ini cua so pet tu pak VLTK -> client JX1 (loose).

- pet.ini (ban thang tu 1024.pak, id a069d4c0 da rut ra_pet) -> \ui\pet\pet.ini?
  KHONG - UiPet C++ cua ta doc truc tiep tu \Ui\Ui3\pet.ini (dat canh cac ini
  khac): ghi ra "\Ui\Ui3\pet_main.ini" (ASCII ten de khoi GBK).
- Anh: pet_main_1/close/btn (spr\Ui3\pet\) + 4 icon skill + nut 门客\按钮-中 +
  khung 道具框 (Ui4) + icon_west/east: hash quet moi pak theo thu tu nap.
"""
import os
import shutil
import sys
import importlib.util

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump as P  # noqa: E402
import ucl  # noqa: E402

spec = importlib.util.spec_from_file_location(
    "pak_id", r"D:\GAMEDEVNEW\ReverseTools\viemde\pak_id.py")
pak_id = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pak_id)
fid = pak_id.file_name_to_id

SEP = chr(92)
ROOT = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"

THU_TU = ["1024.pak", "slistcache.pak"] + \
    ["updatejx%02d.pak" % i for i in range(17, 0, -1)] + \
    ["updatejx07_jxf06.pak", "update02.pak", "update.pak", "update04.pak",
     "update03.pak", "update01.pak", "spr.pak", "ui.pak", "resource.pak",
     "slistfree.pak", "skills.pak"]


def g(s):
    return s.encode("gbk").decode("latin-1")


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


ANH = [
    SEP + "spr" + SEP + "Ui3" + SEP + "pet" + SEP + "pet_main_1.spr",
    SEP + "spr" + SEP + "Ui3" + SEP + "pet" + SEP + "close.spr",
    SEP + "spr" + SEP + "Ui3" + SEP + "pet" + SEP + "btn.spr",
    SEP + "spr" + SEP + "Ui3" + SEP + "pet" + SEP + "skill_enhancehit.spr",
    SEP + "spr" + SEP + "Ui3" + SEP + "pet" + SEP + "skill_attack.spr",
    SEP + "spr" + SEP + "Ui3" + SEP + "pet" + SEP + "skill_resist_all.spr",
    SEP + "spr" + SEP + "Ui3" + SEP + "pet" + SEP + "skill_block.spr",
    SEP + "spr" + SEP + "Ui3" + SEP + g("门客") + SEP + g("按钮-中") + ".spr",
    SEP + "spr" + SEP + "Ui3" + SEP + g("门客") + SEP + "icon_west.spr",
    SEP + "spr" + SEP + "Ui3" + SEP + g("门客") + SEP + "icon_east.spr",
    SEP + "spr" + SEP + "Ui4" + SEP + g("主界面") + SEP + g("新五行印") + SEP +
    "ui" + SEP + g("道具框") + ".spr",
    SEP + "spr" + SEP + "Ui4" + SEP + "common" + SEP + g("分页") + ".spr",
]
# bien the case
CANDS = {}
for a in ANH:
    for c1 in ("Ui3", "UI3", "ui3"):
        for c2 in ("Ui4", "UI4", "ui4"):
            p2 = a.replace(SEP + "Ui3" + SEP, SEP + c1 + SEP).replace(
                SEP + "Ui4" + SEP, SEP + c2 + SEP)
            CANDS.setdefault(fid(p2), a)

da = set()
n = 0
for pkname in THU_TU:
    pk = os.path.join(ROOT, pkname)
    if not os.path.isfile(pk):
        continue
    try:
        f, es = P.entries(pk)
    except Exception:
        continue
    idx = {e[0]: e for e in es}
    for uid, duong in CANDS.items():
        if duong in da or uid not in idx:
            continue
        data = blob_of(f, idx[uid])
        if len(data) < 8:
            continue
        dst = CLI + duong
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        open(dst, "wb").write(data)
        da.add(duong)
        n += 1
        try:
            print("RUT %-46s %7d  tu %s" %
                  (duong.encode("latin-1").decode("gbk", "replace"), len(data), pkname))
        except Exception:
            print("RUT", n)
    f.close()
print("rut %d/%d anh" % (len(da), len(ANH)))
thieu = [a for a in ANH if a not in da]
for t in thieu:
    print("  THIEU:", t.encode("latin-1").decode("gbk", "replace"))

# ---- pet_main.ini: ban VLTK -> client, doi ten ASCII + giu nguyen noi dung ----
src = r"D:\GAMEDEVNEW\ReverseTools\bandonghanh\ra_pet\a069d4c0.ini"
dst = os.path.join(CLI, "Ui", "Ui3", "pet_main.ini")
data = open(src, "rb").read()
if not os.path.isfile(dst) or open(dst, "rb").read() != data:
    open(dst, "wb").write(data)
    print("ghi Ui\\Ui3\\pet_main.ini (%d byte)" % len(data))
