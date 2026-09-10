# -*- coding: utf-8 -*-
r"""[ANDROID 11/09 DULIEU] Rut gon D:\jx1_android_data theo bang phan tich (phan_tich_du_lieu.py) - DOI CHO, khong xoa:
moi thu bo di duoc DOI sang D:\jx1_android_data_bo\<duong dan cu> de chu con lay lai duoc.

Bo:  A. log o goc, tep sao luu (.truoc_* .moi .bak .old __tmp__), Thumbs.db
     B. ban do khong co trong settings\maplist.ini: thu muc maps2\<khu>\<ban do>\ + tep di kem
     C. pak bi che 100 % theo thu tu tim (updatejx01, updatejx02, updatejx13) + xoa dong trong package.ini tro toi pak khong ton tai
     D. pak trong data\ khong ke trong package.ini (sprgame, vlngaothe1, vltkcache, serverlistfree, update05, .truoc_*)
Giu:  spr roi, maps\*.fp, settings, script, ui, userdata, cac pak con lai.
Dung:  python android\rut_gon_du_lieu.py [--thu]      (--thu: chi in, khong doi)
"""
import io
import os
import re
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
GOC = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
sys.path.insert(0, os.path.join(GOC, "ReverseTools", "pak_vltk"))
import pakdump  # noqa: E402

AN = r"D:\jx1_android_data"
BO = r"D:\jx1_android_data_bo"
MB = 1048576.0
THU = "--thu" in sys.argv
tong = [0, 0]


def doi(p):
    rel = os.path.relpath(p, AN)
    try:
        s = os.path.getsize(p) if os.path.isfile(p) else sum(os.path.getsize(os.path.join(r, f)) for r, _, fs in os.walk(p) for f in fs)
    except Exception:
        s = 0
    tong[0] += s; tong[1] += 1
    if THU:
        return
    d = os.path.join(BO, rel)
    os.makedirs(os.path.dirname(d), exist_ok=True)
    if os.path.exists(d):
        if os.path.isdir(d):
            shutil.rmtree(d)
        else:
            os.remove(d)
    shutil.move(p, d)


def ha_ascii(s):
    return "".join(c.lower() if ord(c) < 128 else c for c in s)


def muc_a():
    n0 = tong[1]
    for f in os.listdir(AN):
        p = os.path.join(AN, f)
        if os.path.isfile(p) and f.lower().endswith(".log"):
            doi(p)
    mau = re.compile(r"(\.truoc_[^\\/]*|\.moi|\.bak|\.old|__tmp__|\.orig|\.tmp)$", re.I)
    for root, ds, fs in os.walk(AN):
        for f in fs:
            if mau.search(f) or f.lower() in ("thumbs.db", "desktop.ini"):
                doi(os.path.join(root, f))
    print("A. rac: %d muc" % (tong[1] - n0))


def muc_b():
    n0 = tong[1]
    dung = set()
    for ln in io.open(os.path.join(AN, "settings", "maplist.ini"), encoding="latin-1"):
        if "=" in ln:
            k, v = ln.split("=", 1)
            if k.strip().isdigit():
                dung.add(ha_ascii(v.strip().replace("\\\\", "\\")).lower())
    goc2 = os.path.join(AN, "maps2")
    for khu in os.listdir(goc2):
        pk = os.path.join(goc2, khu)
        if not os.path.isdir(pk):
            continue
        for ten in list(os.listdir(pk)):
            p = os.path.join(pk, ten)
            if os.path.isdir(p) and (khu + "\\" + ten).lower() not in dung:
                for f in os.listdir(pk):
                    fp = os.path.join(pk, f)
                    if os.path.isfile(fp) and f.lower().startswith(ten.lower()):
                        doi(fp)
                doi(p)
    print("B. ban do thua: %d muc" % (tong[1] - n0))


def muc_c_d():
    n0 = tong[1]
    p_ini = os.path.join(AN, "package.ini")
    ini = io.open(p_ini, encoding="latin-1", newline="").read()
    thu = re.search(r"Path=(.*)", ini).group(1).strip().strip("\\").replace("\\", "/")
    ds = sorted((int(m.group(1)), m.group(2).strip()) for m in re.finditer(r"^(\d+)=(.*)$", ini, re.M))
    thang = {}
    che100 = []
    ke = set()
    for i, ten in ds:
        p = os.path.join(AN, thu, ten)
        ke.add(ten.lower())
        if not os.path.isfile(p):
            continue
        f, ents = pakdump.entries(p); f.close()
        s_win = 0
        for uid, off, size, cf in ents:
            if uid not in thang:
                thang[uid] = ten; s_win += (cf & 0xFFFFFF) or size
        if s_win < 64 * 1024:           # con duoi 64 KB dang dung: coi nhu che het
            che100.append((i, ten))
    for i, ten in che100:
        doi(os.path.join(AN, thu, ten))
    # dong package.ini: bo pak da doi + pak khong ton tai (giu so thu tu con lai, danh so lai lien tuc)
    con = [(i, t) for i, t in ds if (i, t) not in che100 and os.path.isfile(os.path.join(AN, thu, t))]
    if not THU:
        dau = ini[:ini.index("0=")] if "0=" in ini else ini
        nl = "\r\n" if "\r\n" in ini else "\n"
        moi = dau + nl.join("%d=%s" % (k, t) for k, (i, t) in enumerate(con)) + nl
        io.open(p_ini, "w", encoding="latin-1", newline="").write(moi)
    print("C. pak che 100%%: %s; package.ini con %d dong" % (", ".join(t for _, t in che100), len(con)))
    # D. pak khong ke trong package.ini
    for f in os.listdir(os.path.join(AN, "data")):
        p = os.path.join(AN, "data", f)
        if os.path.isfile(p) and f.lower() not in ke:
            doi(p)
    print("C+D: %d muc" % (tong[1] - n0))


if __name__ == "__main__":
    if not THU:
        os.makedirs(BO, exist_ok=True)
    muc_a()
    muc_b()
    muc_c_d()
    print("%s %d muc, %.0f MB -> %s" % ("SE doi" if THU else "DA doi", tong[1], tong[0] / MB, BO))
