# -*- coding: utf-8 -*-
"""kiem_npcres_thieu_0909.py - Kiem tra MOI anh bo phan nguoi choi (nam/nu) trong cac bang settings/NpcRes/*.txt
co nam trong .pak khong (bam FileNameToId nhu KPakList: ha chu thuong A-Z, char > 127 am, them '\\' dau), va SUA bang TOC.

Cach dung:  python kiem_npcres_thieu_0909.py [<thu muc client>] [--sua]
  - khong --sua: chi bao cao (moi bang: hang hong, ten thieu) - KHONG ghi gi.
  - --sua      : chi sua bang TOC (ManHair/LadyHair + *Info): sao luu <bang>.truoc_thieu_0909 roi ghi.

Ket qua kiem 09/09 (client TESTLOFFF): 202.971 o, 40.919 o tro toi tep KHONG co trong 43 pak (khong co o dau tren may:
1.056 pak + tep roi). Phan lon la hang du lieu cu khong bao gio duoc dung; nhung TOC (Hair) thi vat pham mu (HelmRes /
GolditemRes / platina) tham chieu deu chi so 0..61 -> nguoi doi mu roi vao hang hong = TROC DAU (ph?n dau chi ve mat).
Log jx_rep3.log hom nay: MA/FM_HR_015, MA/FM_HR_002, FM_HR_012 nap hong hang van lan.

Chinh sach --sua (chu 09/09: "uu tien tang trai nghiem game", "that can than"):
  - CHI bang Hair (ManHair.txt, LadyHair.txt) va bang Info di kem (frames,dirs,interval phai di cung ten tep):
    * hang hong (>= 1/3 o thieu, chi so hang <= 62): CHEP NGUYEN hang du phong (ten + Info) -> kieu toc thong nhat
      moi dong tac. Hang du phong = hang du >= 90 % o va ton tai 100 %, uu tien id nguoi truoc da tron vao hang
      (vd 141), roi hang gan chi so nhat.
    * o thieu le: o khac CUNG HANG, CUNG HO dong tac (WK03 -> WK01), chep ca Info.
  - Cac bang khac (vai, ngua, ao choang, vu khi, than, tay): CHI BAO CAO - thieu vai/ngua co the la co y (khong ve),
    thay bang cai khac = doi noi dung nguoi choi.
Chi sua du lieu TRONG THU MUC CLIENT, khong dong ma nguon.
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
HANG_TOI_DA = 62
BANG_SUA = ("Hair", "Shoulder", "LeftWeapon", "RightWeapon")
# [09/09 b] theo log sau khi sua toc: nap hong con lai = VAI (MA/FM_SH_015/017/012/002: hang 1-46 khong co art, 50/50 o)
# va TAY KHONG (LW_000 AT04/AT05/IJ03/RN03/ST05/WK03 o moi hang) + vu khi phai DE02 (023/025). Chinh sach:
#   Shoulder (hang <= 62): o thieu -> cung hang cung id cung ho, khong co -> DE TRONG (dang khong ve gi; het tra pak/nap hong moi khung)
#   LeftWeapon (moi hang): o thieu mang id 000 (tay khong) -> DE TRONG; o khac -> cung hang cung id cung ho, khong co -> giu
#   RightWeapon (hang <= 62): cung hang cung id cung ho (DE02 -> DE01), khong co -> giu
#   Hair: nhu tren (chep hang du phong)
HAU_TO_BAK = ".truoc_thieu_0909"


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
    if len(d) < 16 or d[:4] != b"PACK":
        return set()
    magic, count, ioff, hsize = struct.unpack("<IIII", d[:16])
    ids = set()
    for i in range(count):
        o = ioff + i * 16
        if o + 16 > len(d):
            break
        ids.add(struct.unpack("<I", d[o:o + 4])[0])
    return ids


IDS = set()
n_pak = 0
for f in sorted(os.listdir(PAKDIR)):
    if f.lower().endswith(".pak"):
        IDS |= load_pak_ids(PAKDIR + "/" + f); n_pak += 1
print("pak: %d tep, %d id" % (n_pak, len(IDS)))


def co_trong_pak(duong_dan):
    s = duong_dan
    while BS + BS in s:
        s = s.replace(BS + BS, BS)
    if name2id(BS + s.lstrip(BS)) in IDS:
        return True
    # tep roi tren dia (KFile::Open du phong)
    return os.path.isfile(CLIENT + "/" + s.lstrip(BS).replace(BS, "/"))


def doc_bang(p):
    raw = io.open(p, "r", encoding="latin-1", newline="").read()
    nl = "\r\n" if "\r\n" in raw else "\n"
    return [l.split("\t") for l in raw.split(nl)], nl


def ghi_bang(p, rows, nl):
    if not os.path.exists(p + HAU_TO_BAK):
        io.open(p + HAU_TO_BAK, "wb").write(io.open(p, "rb").read())
    io.open(p, "w", encoding="latin-1", newline="").write(nl.join("\t".join(r) for r in rows))


# ------------------------------------------------------------ kind file: hang SpecialNpc (nguoi choi nam/nu)
kind, _ = doc_bang(KIND)
col = {h: i for i, h in enumerate(kind[0])}
SECTS = ["Head", "Hair", "Shoulder", "Body", "LeftHand", "RightHand", "LeftWeapon", "RightWeapon", "HorseFront", "HorseMiddle", "HorseBack", "Mantle"]
bang_can = {}
for r in kind[1:]:
    if len(r) <= col["CharacterType"] or r[col["CharacterType"]] != "SpecialNpc":
        continue
    path = r[col["ResFilePath"]]
    for sname in SECTS:
        if sname in col and len(r) > col[sname] and r[col[sname]].strip():
            bang_can[r[col[sname]].strip()] = path
print("bang bo phan: %d" % len(bang_can))

HO = re.compile(r"^([A-Z]{2}_[A-Z]{2}_)(\d+)_([A-Z]{2})(\d*)\.spr$", re.I)   # gioi tinh+bo phan, id, ho dong tac, so


def tach(f):
    m = HO.match(f.strip())
    return (m.group(1).upper(), int(m.group(2)), m.group(3).upper(), m.group(4)) if m else None


def la_spr(x):
    return x.strip().lower().endswith(".spr")


tong_o = tong_thieu = 0
bao_cao = []
for ten_bang in sorted(bang_can):
    p = NPCRES + "/" + ten_bang
    if not os.path.isfile(p):
        print("!! khong co bang", ten_bang); continue
    path = bang_can[ten_bang]
    rows, nl = doc_bang(p)
    hdr = rows[0]
    cache = {}

    def co(f):
        f = f.strip()
        if f not in cache:
            cache[f] = co_trong_pak(path + BS + f)
        return cache[f]

    theo_hang = {}
    so_o_hang = {}
    for ri in range(1, len(rows)):
        row = rows[ri]
        so_o_hang[ri] = sum(1 for x in row[1:] if la_spr(x))
        for ci in range(1, len(row)):
            if not la_spr(row[ci]):
                continue
            tong_o += 1
            if not co(row[ci]):
                theo_hang.setdefault(ri, []).append(ci); tong_thieu += 1
    if not theo_hang:
        print("OK  %-22s %4d hang, 0 thieu" % (ten_bang, len(rows) - 1)); continue
    hong = [ri for ri, ds in theo_hang.items() if len(ds) >= max(3, so_o_hang[ri] // 3)]
    n_thieu = sum(len(v) for v in theo_hang.values())
    print("!!  %-22s %4d hang, %5d o thieu, hang hong (>=1/3): %d [<=62: %s]" % (
        ten_bang, len(rows) - 1, n_thieu, len(hong), ",".join(str(r) for r in sorted(hong) if r <= HANG_TOI_DA)))
    bao_cao.append((ten_bang, len(rows) - 1, n_thieu, sorted(hong)))
    if not any(k in ten_bang for k in BANG_SUA):
        continue

    # ---------------------------------------------------- bang TOC: de xuat + sua
    p_info = NPCRES + "/" + ten_bang[:-4] + "Info.txt"
    info, nl_info = doc_bang(p_info) if os.path.isfile(p_info) else (None, None)

    def id_chinh(ri):
        ids = [tach(x)[1] for x in rows[ri][1:] if la_spr(x) and tach(x)]
        return max(set(ids), key=ids.count) if ids else None

    # hang du phong = hang du (>= 60 o, thieu <= 20 o - chi cac o 'HR_160'/'_HD'): 12/19/28/33/34... la kieu toc co ban
    du = [ri for ri in range(1, len(rows)) if so_o_hang[ri] >= 60 and len(theo_hang.get(ri, [])) <= 20]

    def cung_hang(ri, ci, cung_id):
        """o khac cung hang, cung ho dong tac (WK03 -> WK01); cung_id=True: cung id, False: id khac (o 'HR_160' -> toc chinh)"""
        t = tach(rows[ri][ci])
        if not t:
            return None
        for k in range(1, len(rows[ri])):
            t2 = tach(rows[ri][k]) if la_spr(rows[ri][k]) else None
            if t2 and k != ci and t2[0] == t[0] and t2[2] == t[2] and (t2[1] == t[1]) == cung_id and co(rows[ri][k]):
                return k
        return None

    def id_rieng(ri):
        """id 'cua hang': id CO ART (xuat hien o o ton tai) va chiem >= 1/3 so o cua hang; None = hang khong co art rieng"""
        ton = {}
        thieu_o = set(theo_hang.get(ri, []))
        for ci in range(1, len(rows[ri])):
            t = tach(rows[ri][ci]) if la_spr(rows[ri][ci]) else None
            if t and ci not in thieu_o:
                ton[t[1]] = ton.get(t[1], 0) + 1          # chi dem o TON TAI
        ung = [(n, -i, i) for i, n in ton.items() if n >= max(3, so_o_hang[ri] // 3)]
        return max(ung)[2] if ung else None

    def cung_hang_id(ri, ci, idm):
        """o khac cung hang, cung ho dong tac, mang id idm, ton tai"""
        t = tach(rows[ri][ci])
        if not t:
            return None
        for k in range(1, len(rows[ri])):
            t2 = tach(rows[ri][k]) if la_spr(rows[ri][k]) else None
            if t2 and k != ci and t2[0] == t[0] and t2[1] == idm and t2[2] == t[2] and co(rows[ri][k]):
                return k
        return None

    loai = "Hair" if "Hair" in ten_bang else ("Shoulder" if "Shoulder" in ten_bang else ("LeftWeapon" if "LeftWeapon" in ten_bang else "RightWeapon"))
    thay = []   # (ri, ci, rj, cj, cach); rj == -1: DE TRONG
    for ri in sorted(theo_hang):
        if ri > HANG_TOI_DA and loai != "LeftWeapon":
            continue          # hang > 62: vat pham khong tham chieu -> chi bao cao, khong sua
        ds = theo_hang[ri]
        if loai != "Hair":
            # CHI sua nhung gi log da chi ra (khong doan): vai = de trong; tay khong (id 000) = de trong (dang khong ve gi);
            # vu khi phai: chi ho DE (chet) cung vu khi (DE02 -> DE01). Moi thu khac giu nguyen (de bao cao).
            for ci in ds:
                t = tach(rows[ri][ci])
                if loai == "Shoulder":
                    k = cung_hang(ri, ci, True)
                    if k is not None:
                        thay.append((ri, ci, ri, k, "cung hang, ho %s <- cot %d" % (t[2], k))); continue
                    thay.append((ri, ci, -1, -1, "DE TRONG")); continue
                if loai == "LeftWeapon":
                    if t and t[1] == 0:
                        thay.append((ri, ci, -1, -1, "DE TRONG")); continue
                    thay.append((ri, ci, None, None, "KHONG CO DE XUAT")); continue
                if loai == "RightWeapon":
                    k = cung_hang(ri, ci, True) if (t and t[2] == "DE") else None
                    if k is not None:
                        thay.append((ri, ci, ri, k, "cung hang, ho %s <- cot %d" % (t[2], k))); continue
                    thay.append((ri, ci, None, None, "KHONG CO DE XUAT")); continue
            continue
        idr = id_rieng(ri)
        # hang HONG THAT: khong co art rieng (vd toc 01..11, 15: chi con 9 o 'Guqin' 141) -> lay tu hang du phong gan nhat
        hong_that = idr is None
        ung = sorted((rj for rj in du if rj != ri), key=lambda rj: (abs(rj - ri), rj))
        rj = ung[0] if ung else None
        for ci in ds:
            t = tach(rows[ri][ci])
            k = cung_hang(ri, ci, True)                       # (a) cung hang, cung id, cung ho dong tac
            if k is not None:
                thay.append((ri, ci, ri, k, "cung hang, ho %s <- cot %d" % (t[2], k))); continue
            if idr is not None:                               # (a2) cung hang, id RIENG cua hang, cung ho (o 'HR_160' -> toc chinh)
                k = cung_hang_id(ri, ci, idr)
                if k is not None:
                    thay.append((ri, ci, ri, k, "cung hang, ho %s, id khac <- cot %d" % (t[2] if t else "?", k))); continue
            if hong_that and rj and ci < len(rows[rj]) and la_spr(rows[rj][ci]) and co(rows[rj][ci]):
                thay.append((ri, ci, rj, ci, "chep hang %d (id %s)" % (rj, id_chinh(rj)))); continue   # (b) hang du phong, cung cot
            if hong_that and rj and t:                        # (b2) hang du phong: o cung ho dong tac mang id chinh cua no
                idj = id_chinh(rj); k2 = None
                for k in range(1, len(rows[rj])):
                    t2 = tach(rows[rj][k]) if la_spr(rows[rj][k]) else None
                    if t2 and t2[0] == t[0] and t2[1] == idj and t2[2] == t[2] and co(rows[rj][k]):
                        k2 = k; break
                if k2 is not None:
                    thay.append((ri, ci, rj, k2, "chep hang %d (id %s), ho %s <- cot %d" % (rj, idj, t[2], k2))); continue
            k = cung_hang(ri, ci, False)                      # (c) cung hang, id khac (vd HR_160 -> toc chinh cua hang)
            if k is not None:
                thay.append((ri, ci, ri, k, "cung hang, ho %s, id khac <- cot %d" % (t[2] if t else "?", k))); continue
            thay.append((ri, ci, None, None, "KHONG CO DE XUAT"))
    # tom tat theo hang: so o moi cach + id dich
    tom = {}
    for ri, ci, rj, cj, cach in thay:
        kieu = "chep hang" if cach.startswith("chep hang") else ("cung id" if cach.startswith("cung hang, ho") and "id khac" not in cach else ("id khac" if "id khac" in cach else ("trong" if cach == "DE TRONG" else "KHONG")))
        d = tom.setdefault(ri, {})
        d[kieu] = d.get(kieu, 0) + 1
        if rj is not None and rj >= 0:
            t2 = tach(rows[rj][cj])
            if t2:
                d.setdefault("id", {}); d["id"][t2[1]] = d["id"].get(t2[1], 0) + 1
    tong_kieu = {}
    for ri in sorted(tom):
        d = tom[ri]
        for k, v in d.items():
            if k != "id": tong_kieu[k] = tong_kieu.get(k, 0) + v
        if loai == "Hair" or len(tom) <= 70:
            ids = ", ".join("%d x%d" % (k, v) for k, v in sorted(d.get("id", {}).items(), key=lambda kv: -kv[1]))
            print("      hang %3d %-16s thieu %2d/%2d: chep hang %2d | cung id %2d | id khac %2d | trong %2d | khong %2d | id dich: %s" % (
                ri, rows[ri][0][:16], len(theo_hang[ri]), so_o_hang[ri], d.get("chep hang", 0), d.get("cung id", 0), d.get("id khac", 0), d.get("trong", 0), d.get("KHONG", 0), ids))
    print("      TONG %s: %s (hang co de xuat: %d)" % (ten_bang, ", ".join("%s %d" % kv for kv in sorted(tong_kieu.items())), len(tom)))
    if SUA:
        n = 0
        for ri, ci, rj, cj, cach in thay:
            if rj is None:
                continue
            if rj < 0:
                rows[ri][ci] = ""
                if info and ri < len(info) and ci < len(info[ri]):
                    info[ri][ci] = ""
                n += 1; continue
            rows[ri][ci] = rows[rj][cj]
            if info and ri < len(info) and rj < len(info) and ci < len(info[ri]) and cj < len(info[rj]):
                info[ri][ci] = info[rj][cj]
            n += 1
        if n:
            ghi_bang(p, rows, nl)
            if info:
                ghi_bang(p_info, info, nl_info)
            print("    DA SUA %d o trong %s (+Info), sao luu %s" % (n, ten_bang, HAU_TO_BAK))
print("TONG: %d o, thieu %d" % (tong_o, tong_thieu))
