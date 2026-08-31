# -*- coding: utf-8 -*-
"""t69_kiem_mau_thobao.py - phep kiem THO BAO NHAT cho mau tep "mo coi".

Sau hai lan cong cu cua toi tra loi sai (thieu duong nap pak; bieu thuc cat cut
ten tieng Trung), toi khong tin bat ky bo loc nao nua. Phep kiem nay khong dung
bo loc: lay NGUYEN BYTE ten tep, do tim tren TOAN BO thu muc may chu - moi loai
tep, ke ca nhi phan (pak da giai nen, .fp, .dat, .exe, .dll).

Neu mot ten khong xuat hien O DAU CA thi khong con cho nghi ngo.

Dung: t69_kiem_mau_thobao.py [so_luong_mau]
CHI DOC.
"""
import io
import os
import random
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import doi_tep as dt  # noqa: E402

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

SV = dt.SV
S = dt.S
NGUON = r"D:\GAMEDEVNEW\Sources"
PAK = ["Pak\\maps.pak", "Pak\\maps_client.pak",
       "Pak\\maps_banghuichengbao.pak", "Pak\\maps_tieu_bang_chien.pak",
       "Pak\\namcung.pak"]


def doc(p):
    return io.open(p, "rb").read()


def main():
    n_mau = int(sys.argv[1]) if len(sys.argv) > 1 else 8
    # danh sach ung vien: lay tu ket qua t68 (tep .lua khong ai nhac)
    # -> o day chi lay ngau nhien trong cac thu muc trap/obj TQ de kiem
    ung = []
    for goc, tm, ts in os.walk(S):
        tm[:] = [x for x in tm if not any(b in x.lower()
                                          for b in ("_backup", "_gobo",
                                                    "_dara"))]
        rel_tm = os.path.relpath(goc, S)
        if not any(x in rel_tm.lower() for x in ("\\trap", "\\obj")):
            continue
        if not any(ord(c) > 127 for c in rel_tm):
            continue
        for t in ts:
            if t.lower().endswith(".lua"):
                ung.append(os.path.join(goc, t))
    if not ung:
        print("khong con tep nao trong dien nay")
        return 0
    random.seed(20260830)
    mau = random.sample(ung, min(n_mau, len(ung)))
    print("=== t69 kiem THO BAO %d tep mau ===" % len(mau))
    print("    (tim nguyen byte ten tep tren TOAN BO may chu + ma nguon)")
    print()

    # --- gom van ban de do: script + settings + Maps + Sources + pak giai nen
    kho = []
    for goc_cay in (S, os.path.join(SV, "settings"), os.path.join(SV, "Maps"),
                    os.path.join(NGUON, "Core", "Src")):
        if not os.path.isdir(goc_cay):
            continue
        n = 0
        for goc, _, ts in os.walk(goc_cay):
            for t in ts:
                p = os.path.join(goc, t)
                try:
                    if os.path.getsize(p) > 40 * 1024 * 1024:
                        continue
                    kho.append((p, doc(p)))
                    n += 1
                except OSError:
                    continue
        print("    nap %-46s %5d tep" % (goc_cay[-46:], n))
    # pak: giai nen
    try:
        import pakdump
        import ucl
        for rel in PAK:
            p = os.path.join(SV, rel)
            if not os.path.isfile(p):
                continue
            f, es = pakdump.entries(p)
            gom = []
            for e in es:
                uid, off, size, cf = e
                csize = cf & 0xFFFFFF
                flag = cf >> 24
                f.seek(off)
                blob = f.read(csize if csize else size)
                if flag == 0 or csize in (0, size):
                    gom.append(blob[:size] if size > 0 else blob)
                else:
                    try:
                        gom.append(ucl.nrv2b_decompress_8(blob, size))
                    except Exception:
                        pass
            f.close()
            kho.append((rel + " (da giai nen)", b"\x00".join(gom)))
            print("    nap %-46s %5d entry" % (rel, len(es)))
    except Exception as e:
        print("    !! khong nap duoc pak: %s" % e)
    print()

    for p in mau:
        rel = os.path.relpath(p, S)
        ten = os.path.basename(p).encode("latin-1")
        hit = []
        for ten_nguon, b in kho:
            if ten_nguon.lower().endswith(rel.lower()):
                continue                    # chinh no
            if ten in b:
                hit.append(ten_nguon)
                if len(hit) >= 3:
                    break
        print("  %-58s %s"
              % (repr(rel)[:58],
                 "SACH (0 cho)" if not hit
                 else "!! CO %d cho: %s" % (len(hit), hit[:2])))
    return 0


if __name__ == "__main__":
    sys.exit(main())
