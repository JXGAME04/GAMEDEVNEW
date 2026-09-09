# -*- coding: utf-8 -*-
"""kiem_npcres_thieu_0909.py - Kiem tra MOI anh bo phan NPC (nguoi choi nam/nu + NPC dac biet) trong cac bang
settings/NpcRes/*.txt co nam trong .pak khong (bam FileNameToId nhu KPakList), va de xuat thay the.

Cach dung:  python kiem_npcres_thieu_0909.py [<thu muc client>] [--sua]
  - khong --sua: chi in bao cao (bang, hang, cot, ten thieu) + de xuat.
  - --sua      : ghi bang moi (sao luu <bang>.truoc_thieu_0909), ap dung de xuat.
De xuat cho mot o thieu (hang r, cot c, ten f):
  1) hang r co o khac cung ho dong tac (2 chu dau cua hau to, vd WK03 -> WK01) ma ton tai -> dung o do;
  2) neu hang r thieu qua nhieu (>= 1/3 so o) -> chep NGUYEN hang du phong (hang dau tien du 100 % co cung tien to
     giong 'MA_HR_' + gan id nhat, uu tien id do nguoi truoc da chon trong hang, vd 160) de bo phan khong doi kieu
     giua cac dong tac;
  3) con lai: o cung cot cua hang du phong.
Chi sua bang TRONG THU MUC CLIENT (du lieu), khong dong den ma nguon.
"""
import io
import os
import re
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
CLIENT = "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/client"
SUA = "--sua" in sys.argv
args = [a for a in sys.argv[1:] if not a.startswith("--")]
if args:
    CLIENT = args[0]
NPCRES = CLIENT + "/settings/NpcRes"
KIND = NPCRES + "/npc_res_kind_file_name.txt"
PAKDIR = CLIENT + "/data"


def name2id(s):
    uid = 0
    idx = 0
    for ch in s:
        c = ord(ch)
        if 65 <= c <= 90:
            c += 32
        elif c > 127:
            c -= 256
        idx += 1
        uid = (((uid + idx * c) & 0xFFFFFFFF) % 0x8000000B) * 0xFFFFFFEF
        uid &= 0xFFFFFFFF
    return uid ^ 0x12345678


def load_pak_ids(p):
    d = open(p, "rb").read()
    magic, count, ioff, hsize = struct.unpack("<IIII", d[:16])
    ids = set()
    for i in range(count):
        o = ioff + i * 16
        if o + 16 > len(d):
            break
        uid = struct.unpack("<I", d[o:o + 4])[0]
        ids.add(uid)
    return ids


IDS = set()
for f in sorted(os.listdir(PAKDIR)):
    if f.lower().endswith(".pak"):
        try:
            n0 = len(IDS)
            IDS |= load_pak_ids(PAKDIR + "/" + f)
            print("pak %-28s +%d muc" % (f, len(IDS) - n0))
        except Exception as e:
            print("pak %s LOI %s" % (f, e))
print("tong id trong pak:", len(IDS))


def co_trong_pak(duong_dan):
    s = duong_dan
    while BS + BS in s:
        s = s.replace(BS + BS, BS)
    return name2id(BS + s.lstrip(BS)) in IDS


def doc_bang(p):
    raw = io.open(p, "r", encoding="latin-1", newline="").read()
    nl = "\r\n" if "\r\n" in raw else "\n"
    lines = raw.split(nl)
    rows = [l.split("\t") for l in lines]
    return rows, nl


def ghi_bang(p, rows, nl):
    io.open(p, "w", encoding="latin-1", newline="").write(nl.join("\t".join(r) for r in rows))


# ------------------------------------------------------------ doc kind file: hang SpecialNpc (nguoi choi nam/nu)
kind, _ = doc_bang(KIND)
hdr = kind[0]
col = {h: i for i, h in enumerate(hdr)}
SECTS = ["Head", "Hair", "Shoulder", "Body", "LeftHand", "RightHand", "LeftWeapon", "RightWeapon", "HorseFront", "HorseMiddle", "HorseBack", "Mantle"]
bang_can = {}   # (ten bang) -> duong dan res
for r in kind[1:]:
    if len(r) <= col["CharacterType"] or r[col["CharacterType"]] != "SpecialNpc":
        continue
    path = r[col["ResFilePath"]] if len(r) > col["ResFilePath"] else ""
    for sname in SECTS:
        if sname in col and len(r) > col[sname] and r[col[sname]].strip():
            bang_can[r[col[sname]].strip()] = path
print("bang bo phan can kiem:", len(bang_can), "|", ", ".join(sorted(bang_can)))

HO = re.compile(r"^(.*_)([A-Z]{2})(\d+)\.spr$", re.I)   # tien to, ho dong tac, so


def tach(f):
    m = HO.match(f)
    return (m.group(1), m.group(2).upper(), m.group(3)) if m else (None, None, None)


tong_thieu = 0
tong_o = 0
ket_qua = {}
for ten_bang in sorted(bang_can):
    p = NPCRES + "/" + ten_bang
    if not os.path.isfile(p):
        print("!! khong co bang", ten_bang); continue
    path = bang_can[ten_bang]
    rows, nl = doc_bang(p)
    hdr = rows[0]
    thieu = []          # (r, c, f)
    ton_tai = {}        # f -> bool (cache)

    def co(f):
        if f not in ton_tai:
            ton_tai[f] = co_trong_pak(path + BS + f)
        return ton_tai[f]

    for ri in range(1, len(rows)):
        row = rows[ri]
        for ci in range(1, len(row)):
            f = row[ci].strip()
            if not f or not f.lower().endswith(".spr"):
                continue
            tong_o += 1
            if not co(f):
                thieu.append((ri, ci, f))
    tong_thieu += len(thieu)
    if not thieu:
        print("OK  %-24s %4d hang, 0 thieu" % (ten_bang, len(rows) - 1)); continue
    print("!!  %-24s %4d hang, %d o thieu, %d ten khac nhau" % (ten_bang, len(rows) - 1, len(thieu), len(set(f for _, _, f in thieu))))
    # gom theo hang
    theo_hang = {}
    for ri, ci, f in thieu:
        theo_hang.setdefault(ri, []).append((ci, f))
    de_xuat = []   # (ri, ci, cu, moi, cach)
    for ri, ds in sorted(theo_hang.items()):
        row = rows[ri]
        so_o = sum(1 for x in row[1:] if x.strip().lower().endswith(".spr"))
        # hang du phong: cung tien to (vd MA_HR_), du 100 %, uu tien id nguoi truoc da tron vao hang nay, roi id gan nhat
        tien_to = None
        ids_trong_hang = []
        for x in row[1:]:
            tt, ho, so = tach(x.strip())
            if tt:
                m = re.match(r"^([A-Z]{2}_[A-Z]{2}_)(\d+)_$", tt, re.I)
                if m:
                    tien_to = m.group(1); ids_trong_hang.append(int(m.group(2)))
        id_hang = None
        m = re.search(r"(\d+)\s*$", row[0])
        if m:
            id_hang = int(m.group(1))
        if ids_trong_hang and id_hang is None:
            id_hang = max(set(ids_trong_hang), key=ids_trong_hang.count)
        ung = []
        for rj in range(1, len(rows)):
            if rj == ri:
                continue
            rw = rows[rj]
            cells = [x.strip() for x in rw[1:] if x.strip().lower().endswith(".spr")]
            if not cells or len(cells) < so_o * 0.9:
                continue
            if any(not co(x) for x in cells):
                continue
            ids = []
            for x in cells:
                tt, ho, so = tach(x)
                if tt:
                    m2 = re.match(r"^([A-Z]{2}_[A-Z]{2}_)(\d+)_$", tt, re.I)
                    if m2:
                        ids.append(int(m2.group(2)))
            if not ids:
                continue
            idj = max(set(ids), key=ids.count)
            uu_tien = 0 if idj in ids_trong_hang else 1
            ung.append((uu_tien, abs(idj - (id_hang if id_hang is not None else idj)), rj, idj))
        ung.sort()
        du_phong = ung[0] if ung else None
        if len(ds) >= max(3, so_o // 3) and du_phong:
            rj = du_phong[2]
            for ci in range(1, min(len(row), len(rows[rj]))):
                cu = row[ci].strip(); moi = rows[rj][ci].strip()
                if cu.lower().endswith(".spr") and moi.lower().endswith(".spr") and cu != moi:
                    de_xuat.append((ri, ci, cu, moi, "chep hang %d (id %d)" % (rj, du_phong[3])))
            continue
        for ci, f in ds:
            tt, ho, so = tach(f)
            moi = None; cach = ""
            if tt:
                for x in row[1:]:
                    x = x.strip()
                    tt2, ho2, so2 = tach(x)
                    if tt2 == tt and ho2 == ho and x != f and co(x):
                        moi = x; cach = "cung hang, cung ho %s" % ho; break
            if moi is None and du_phong and ci < len(rows[du_phong[2]]):
                x = rows[du_phong[2]][ci].strip()
                if x.lower().endswith(".spr") and co(x):
                    moi = x; cach = "cung cot, hang %d (id %d)" % (du_phong[2], du_phong[3])
            de_xuat.append((ri, ci, f, moi, cach if moi else "KHONG CO DE XUAT"))
    ket_qua[ten_bang] = (rows, nl, hdr, de_xuat, theo_hang)
    for ri, ds in sorted(theo_hang.items()):
        print("    hang %3d %-22s thieu %2d/%d: %s" % (ri, rows[ri][0][:22], len(ds), sum(1 for x in rows[ri][1:] if x.strip().lower().endswith('.spr')), ", ".join(sorted(set(f for _, f in ds)))[:200]))
    for ri, ci, cu, moi, cach in de_xuat:
        print("      -> hang %3d cot %2d %-28s %-24s => %-24s (%s)" % (ri, ci, hdr[ci][:28] if ci < len(hdr) else "?", cu, moi or "?", cach))
    if SUA:
        n = 0
        for ri, ci, cu, moi, cach in de_xuat:
            if moi and rows[ri][ci].strip() == cu:
                rows[ri][ci] = moi; n += 1
        if n:
            bak = p + ".truoc_thieu_0909"
            if not os.path.exists(bak):
                io.open(bak, "wb").write(io.open(p, "rb").read())
            ghi_bang(p, rows, nl)
            print("    DA SUA %d o, sao luu %s" % (n, os.path.basename(bak)))
print("TONG: %d o, thieu %d" % (tong_o, tong_thieu))
