# -*- coding: utf-8 -*-
"""rut_ten_pak_0809.py - tim/rut cac tep theo TEN (duong dan trong pak) tu pak cua game + client tham khao, ghi thanh tep roi
vao bin\\client (engine uu tien tep roi truoc pak). Dung: python rut_ten_pak_0809.py [--rut] ten1 ten2 ...  (ten dung '/', vd
/spr/npcres/man/MA_HR_015_HR01.spr). Khong co ten -> bo mac dinh: toan bo hoat anh HR_015 nam/nu (do 12:38: xin ~600 luot/s, thieu tep)."""
import os, sys, io
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pakdump as P
import ucl
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
GAME = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
PAKDIRS = [("game", GAME + r"\data"), ("jx1-levelup", r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"), ("vltk2.0", r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\data")]
SUF = ["AT01", "AT02", "AT03", "AT04", "AT05", "AT06", "AT07", "AT08", "MG01", "MG02", "MG03", "MG04", "DE01", "DE02", "HR01", "HI01", "RN01", "RN02",
       "ST01", "ST02", "WK01", "WK02", "RD01", "RD02", "HA01", "HA02", "SI01", "JP01", "RA01", "RS01", "SR01", "RB01", "DF01", "BK01", "FI01", "GD01"]
names = [a.replace("/", BS) for a in sys.argv[1:] if not a.startswith("--")]
if not names:
    for sex, d in (("MA", "man"), ("FM", "woman")):
        for sfx in SUF:
            for hr in ("HR_015", "HR_012"):
                names.append(BS + "spr" + BS + "npcres" + BS + d + BS + "%s_%s_%s.spr" % (sex, hr, sfx))
want = {P.name2id(n): n for n in names}

def read_raw(f, e):
    uid, off, size, cf = e
    csize = cf & 0xFFFFFF; flag = cf >> 24
    f.seek(off); blob = f.read(csize if csize else size)
    if flag in (0x01, 0x20) and csize and csize != size: return ucl.nrv2b_decompress_8(blob, size)
    return blob

found = {}
for tag, d in PAKDIRS:
    if not os.path.isdir(d): continue
    for fn in sorted(os.listdir(d)):
        if not fn.lower().endswith((".pak", ".mps")): continue
        try: f, es = P.entries(os.path.join(d, fn))
        except Exception: continue
        for e in es:
            if e[0] in want and want[e[0]] not in found:
                try: found[want[e[0]]] = (tag + "/" + fn, read_raw(f, e))
                except Exception as ex: print("  loi giai nen", fn, want[e[0]], ex)
print("tim %d ten: thay %d" % (len(names), len(found)))
for n in sorted(found): print("  %-52s <- %s (%d B)" % (n, found[n][0], len(found[n][1])))
miss = [n for n in names if n not in found]
print("khong thay %d, vd: %s" % (len(miss), miss[:6]))
if "--rut" in sys.argv:
    nW = 0
    for n, (src, data) in found.items():
        p = GAME + n
        if os.path.exists(p): continue
        os.makedirs(os.path.dirname(p), exist_ok=True)
        open(p, "wb").write(data); nW += 1
    print("da ghi %d tep roi" % nW)
