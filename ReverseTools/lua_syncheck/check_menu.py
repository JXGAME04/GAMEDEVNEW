# -*- coding: utf-8 -*-
r"""Kiem menu Lenh Bai Admin: moi lua chon '.../TenHam' phai co ham TenHam that,
va nhan (phan truoc dau '/' DAU TIEN) phai KHONG chua '/'.
Ham co the dinh nghia trong hoatdong_admin.lua, lenhbaiadmin.lua hoac cac tep duoc Include.
"""
import io, re, os

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MENU = [os.path.join(E, r"script\item\hoatdong_admin.lua"),
        os.path.join(E, r"script\item\lenhbaiadmin.lua")]

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()

# 1) gom ham dinh nghia tu lenhbaiadmin + moi tep no Include (1 tang) + hoatdong_admin
defined = set()
scanned = set()

def add_defs(p):
    if p in scanned or not os.path.isfile(p): return
    scanned.add(p)
    s = rd(p)
    for m in re.finditer(r"function\s+([\w:.]+)\s*\(", s):
        defined.add(m.group(1))
    for m in re.finditer(r'Include\("([^"]+)"\)', s):
        rel = m.group(1).replace("\\\\", "\\").lstrip("\\")
        add_defs(os.path.join(E, rel))

for p in MENU:
    add_defs(p)

# them ham engine hay dung trong menu (dang dau ham dac biet)
ENGINE_OK = {"no", "main"}

print("Da quet %d tep, %d ham dinh nghia" % (len(scanned), len(defined)))

# 2) kiem tung lua chon
bad_fn, bad_label = [], []
for p in MENU:
    s = rd(p)
    for i, line in enumerate(s.split("\n"), 1):
        for m in re.finditer(r'"([^"]*)"', line):
            t = m.group(1)
            if "/" not in t: continue
            head, _, tail = t.partition("/")
            # chi xet chuoi trong dang "<nhan>/<TenHam>" - ten ham la dinh danh hop le
            if not re.match(r"^[A-Za-z_]\w*$", tail):
                # co the la nhan chua '/' o giua -> canh bao neu phan cuoi giong ten ham
                if re.search(r"/[A-Za-z_]\w*$", t):
                    bad_label.append((p, i, t))
                continue
            if tail in defined or tail in ENGINE_OK:
                continue
            bad_fn.append((p, i, t, tail))

print("\n--- LUA CHON TRO TOI HAM KHONG TON TAI ---")
if not bad_fn:
    print("  (khong co)")
for p, i, t, fn in bad_fn:
    print("  %s:%d  ham '%s'  <- %r" % (os.path.basename(p), i, fn, t[:60]))

print("\n--- NHAN CHUA DAU '/' (engine se cat sai) ---")
if not bad_label:
    print("  (khong co)")
for p, i, t in bad_label:
    print("  %s:%d  %r" % (os.path.basename(p), i, t[:70]))
