# -*- coding: utf-8 -*-
r"""[BDH 28/08] Rut 5 ini DONG HANH ban VIET tu pak client VLTK, de vao client JX1.

Ban thang theo thu tu nap (memory lo ren): update03 > update01 > slistcache > 1024.
Giai nen UCL nrv2b (ex2-style). Backup ban cu .truoc_vltk. In khac biet section.
"""
import os
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
UUTIEN = ["update03.pak", "update01.pak", "slistcache.pak", "1024.pak"]
INIS = ["同伴属性", "同伴技能", "同伴技能树", "同伴背包", "同伴快捷栏"]


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
    if fl == 0 or cs in (0, size):
        return raw[:size] if size > 0 else raw
    if fl == 1:
        return ucl.nrv2b_decompress_8(raw, size)
    import bz2, zlib
    if fl == 2:
        return bz2.decompress(raw)
    return zlib.decompress(raw)


# duong trong pak da xac nhan hit o buoc scan: \ui\{U}\<ten>.ini ? kiem lai:
# scan hit voi to hop nao khong ghi ra - thu lai tung base cho chac.
BASES = ["ui" + SEP + "UI3" + SEP, "ui" + SEP + "Ui3" + SEP, "ui" + SEP + "ui3" + SEP,
         "ui" + SEP + "ui3_1024" + SEP, "ui" + SEP, "UI3" + SEP, "Ui3" + SEP,
         "ui3" + SEP, "ini" + SEP]

import re


def secs(data):
    return re.findall(rb"^\[([^\]\r\n]+)\]", data, re.M)


for ten in INIS:
    got = None
    for pkname in UUTIEN:
        pk = os.path.join(ROOT, pkname)
        if not os.path.isfile(pk):
            continue
        f, es = P.entries(pk)
        idx = {e[0]: e for e in es}
        for base in BASES:
            duong = SEP + base + g(ten) + ".ini"
            u = fid(duong)
            if u in idx:
                data = blob_of(f, idx[u])
                got = (pkname, duong, data)
                break
        f.close()
        if got:
            break
    if not got:
        print("!! KHONG rut duoc:", ten)
        continue
    pkname, duong, data = got
    dst = os.path.join(CLI, "Ui", "Ui3", g(ten) + ".ini")
    cu = open(dst, "rb").read() if os.path.isfile(dst) else b""
    trang_thai = "GIONG HET" if cu == data else "KHAC (%d -> %d byte)" % (len(cu), len(data))
    print("%s: %s tu %s  [%s]" % (ten, len(data), pkname, trang_thai))
    if cu != data:
        s_cu = set(secs(cu))
        s_moi = set(secs(data))
        mat = [x.decode("gbk", "replace") for x in sorted(s_cu - s_moi)]
        them = [x.decode("gbk", "replace") for x in sorted(s_moi - s_cu)]
        if mat:
            print("   section MAT so ban cu:", ", ".join(mat))
        if them:
            print("   section MOI:", ", ".join(them))
        if cu and not os.path.exists(dst + ".truoc_vltk"):
            open(dst + ".truoc_vltk", "wb").write(cu)
        open(dst, "wb").write(data)
        print("   -> DA DE ban VLTK vao client (backup .truoc_vltk)")
print("XONG p14b")
