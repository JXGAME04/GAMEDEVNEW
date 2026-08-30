# -*- coding: utf-8 -*-
"""t33_quet_hangso_conlai.py - QUET TOAN CAY tim moi hang so CAP TEP con lai.

Muc dich: khong dua vao danh sach khao sat nua ma tu do, de biet CHINH XAC con
bao nhieu hang so co the noi day duoc.

TIEU CHI LAY:
  - Dong dang `TEN = <so nguyen>` o CAP TEP (khong thut dau qua mot tab,
    khong nam trong than ham).
  - Ten bien VIET HOA hoac co gach duoi (kieu hang so).
  - Chua duoc noi day (chua co dang `TEN = *_CFG(`).
  - Tep nam trong cac thu muc TINH NANG (khong phai bang du lieu, khong phai
    tep ban do, khong phai thu muc cauhinh).

TIEU CHI LOAI (ghi ro de nguoi sau hieu vi sao):
  - Gia tri khong phai so nguyen (bang, chuoi, bieu thuc goi ham).
  - Ten bien co ve la ID / toa do / chi so (MAP, MID, ID, IDX, POS, X, Y,
    TASK, TSK, MS_, FILE, SCRIPT, NPC_ ...).
  - Tep trong danh sach bo qua (du lieu ban do, log, ban sao).

CHI DOC - khong sua gi. Sinh ra `hangso_conlai.txt`.
"""
import io
import os
import re
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
RA = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                  "hangso_conlai.txt")

# thu muc dang xet (cac he tinh nang)
THUMUC = [
    "tinhnang", "missions", "event", "vng_event", "task", "petsys",
    "global", "lib", "header", "startgame", "item", "npcthon",
    "activitysys", "leaguematch", "mission", "huoyuedu", "songbac",
    "timertask", "battles",
]

# ten bien co ve la dinh danh / toa do -> loai
LOAI_TEN = re.compile(
    r"(^|_)(MAP|MID|ID|IDX|POS|FILE|SCRIPT|NPC|ITEM|TASK|TSK|MS|CAMP|SERIES|"
    r"COLOR|ICON|MODEL|SOUND|X|Y|Z|W|H|TYPE|KIND|GENRE|DETAIL|STATE|FLAG)($|_)",
    re.I)

# tep bo qua
BOQUA = re.compile(r"(\\maps\\|\\cauhinh\\|\.truoc_|test|_bak|backup|"
                   r"data|_log|log_)", re.I)

DONG = re.compile(r"^([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(-?\d+)\s*(;)?\s*(--.*)?$")


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def main():
    ds = []
    n_tep = 0
    for tm in THUMUC:
        goc0 = os.path.join(S, tm)
        if not os.path.isdir(goc0):
            continue
        for goc, _, tep in os.walk(goc0):
            for t in tep:
                if not t.lower().endswith(".lua"):
                    continue
                p = os.path.join(goc, t)
                rel = os.path.relpath(p, S)
                if BOQUA.search("\\" + rel):
                    continue
                try:
                    d = doc(p).replace("\r\n", "\n").split("\n")
                except OSError:
                    continue
                n_tep += 1
                trong_ham = False
                for i, l in enumerate(d, 1):
                    ls = l.strip()
                    if ls.startswith("function ") or re.match(
                            r"^\s*function\s", l):
                        trong_ham = True
                    elif l == "end" or l == "end;":
                        trong_ham = False
                    if trong_ham:
                        continue
                    if l[:1] in (" ", "\t"):     # thut dau => khong phai cap tep
                        continue
                    m = DONG.match(l.rstrip())
                    if not m:
                        continue
                    ten, gt = m.group(1), int(m.group(2))
                    if ten.islower():            # bien thuong, khong phai hang
                        continue
                    if LOAI_TEN.search(ten):
                        continue
                    ds.append((rel, i, ten, gt, (m.group(4) or "").strip()))

    print("Da quet %d tep .lua trong %d thu muc" % (n_tep, len(THUMUC)))
    print("Tim thay %d hang so cap tep chua noi day" % len(ds))

    theo_tep = {}
    for rel, i, ten, gt, cm in ds:
        theo_tep.setdefault(rel, []).append((i, ten, gt, cm))

    with io.open(RA, "w", encoding="utf-8", newline="") as f:
        f.write("HANG SO CAP TEP CHUA NOI VAO HE CAU HINH\n")
        f.write("Sinh boi ReverseTools/cauhinh/t33_quet_hangso_conlai.py"
                " - CHI DOC\n")
        f.write("Tieu chi: dong dang TEN = <so nguyen> o cap tep, ten kieu hang"
                " so,\n")
        f.write("chua co dang TEN = *_CFG(...). Da loai ten co ve la ID/toa do.\n")
        f.write("=" * 78 + "\n\n")
        f.write("Tong: %d hang so o %d tep\n\n" % (len(ds), len(theo_tep)))
        for rel, hs in sorted(theo_tep.items(), key=lambda x: -len(x[1])):
            f.write("## %s  (%d)\n" % (rel, len(hs)))
            for i, ten, gt, cm in hs:
                f.write("    :%-5d %-34s = %-12d %s\n" % (i, ten, gt, cm[:60]))
            f.write("\n")
    print("=> da ghi %s" % RA)
    print()
    print("Top 15 tep nhieu hang so nhat:")
    for rel, hs in sorted(theo_tep.items(), key=lambda x: -len(x[1]))[:15]:
        print("  %-56s %3d" % (rel[:56], len(hs)))


if __name__ == "__main__":
    main()
