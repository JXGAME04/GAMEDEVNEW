# -*- coding: utf-8 -*-
"""So tep trong bao dong 3 tinh nang giua BAN LINUX va BAN JX1 dang chay.

"JX1 da co" trong closure3.json chi co nghia LA CO TEP CUNG DUONG DAN.
Tep do co the la ban KHAC HAN (cu hon / da bi sua khi port). Script nay do do lech that:
  - IDENTICAL : byte y het
  - NEAR      : >= 90% dong trung
  - DIVERGED  : lech nhieu  -> khi port phai HOP NHAT bang tay, khong duoc de len
  - MISSING   : JX1 chua co

Chay: python cmp_jx1.py
Ra:   port_3hd/07_doi_chieu_tep.md
"""
import io, os, sys, json, difflib

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools")
from gbktool import decline  # noqa

HERE = os.path.dirname(os.path.abspath(__file__))
LNXS = r"D:\ServerLinux\server1\script"
JX1S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"


def lines(p):
    d = open(p, "rb").read()
    return [decline(l.rstrip(b"\r")) for l in d.split(b"\n")], d


def main():
    cj = json.load(io.open(os.path.join(HERE, "closure3.json"), encoding="utf-8"))
    out = ["# Doi chieu tep bao dong: ban Linux (JX2) vs may chu JX1 dang chay",
           "",
           "Nguon A: `D:\\ServerLinux\\server1\\script`",
           "Nguon B: `E:\\SourceTuanLe\\SourceVs22\\TESTLOFFF_ONLINE\\bin\\server\\script`",
           "",
           "**Y NGHIA CAC MUC**",
           "",
           "| Muc | Nghia | Viec phai lam khi port |",
           "|---|---|---|",
           "| `MISSING` | JX1 chua co tep nay | chep moi |",
           "| `IDENTICAL` | byte y het | khong lam gi |",
           "| `NEAR` | >= 90% dong trung | doc diff, thuong an toan |",
           "| `DIVERGED` | lech nhieu | **CAM de len** - phai hop nhat bang tay |",
           ""]
    tally = {}
    for feat, rows in cj.items():
        out += ["## %s" % feat, "",
                "| Tep | Trang thai | Dong (Linux) | Dong (JX1) | %% trung |",
                "|---|---|---|---|---|"]
        for r in sorted(rows, key=lambda x: x["rel"]):
            rel = r["rel"]
            a = os.path.join(LNXS, rel.replace("/", os.sep))
            b = os.path.join(JX1S, rel.replace("/", os.sep))
            if not os.path.isfile(a):
                st, la, lb, pct = "KHONG CO O BAN LINUX", "-", "-", "-"
            elif not os.path.isfile(b):
                A, _ = lines(a)
                st, la, lb, pct = "MISSING", len(A), "-", "0"
            else:
                A, da = lines(a)
                B, db = lines(b)
                if da == db:
                    st, pct = "IDENTICAL", "100"
                else:
                    sm = difflib.SequenceMatcher(None, A, B, autojunk=False)
                    ratio = sm.ratio() * 100
                    st = "NEAR" if ratio >= 90 else "DIVERGED"
                    pct = "%.1f" % ratio
                la, lb = len(A), len(B)
            tally[st] = tally.get(st, 0) + 1
            mark = "**%s**" % st if st == "DIVERGED" else st
            out.append("| `%s` | %s | %s | %s | %s |" % (rel, mark, la, lb, pct))
        out.append("")

    out += ["## Tong ket", "", "| Trang thai | So tep |", "|---|---|"]
    for k in sorted(tally, key=lambda x: -tally[x]):
        out.append("| %s | %d |" % (k, tally[k]))
    out.append("")
    with io.open(os.path.join(HERE, "07_doi_chieu_tep.md"), "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(out))
    for k in sorted(tally, key=lambda x: -tally[x]):
        print("%-22s %d" % (k, tally[k]))


if __name__ == "__main__":
    main()
