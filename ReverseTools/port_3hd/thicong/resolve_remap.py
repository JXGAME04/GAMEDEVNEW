# -*- coding: utf-8 -*-
"""Giai NHAP NHANG bang anh xa item: voi moi bo so nguon (ban Linux), chon dich
JX1 co TEN vat pham TRUNG KHIT (khong lay dich dau tien mu quang).

Bang item:
  Linux: D:\\ServerLinux\\Patch\\settings\\item\\004\\magicscript.txt  (dong = pt+2)
  JX1  : E:\\...\\bin\\server\\settings\\item\\magicscript.txt
Ket qua: remap_resolved.json = { "6,1,399": "6,1,398", ... }  (dich CUNG GENRE, dung ten)
"""
import io, os, re, csv, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools")
from dec2 import decline2  # bo giai ma da va

HERE = os.path.dirname(os.path.abspath(__file__))
P3 = os.path.dirname(HERE)
LNX_ITEM = r"D:\ServerLinux\Patch\settings\item\004\magicscript.txt"
JX1_ITEM = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt"


def load_items(path):
    """Tra dict (genre,detail,particular) -> ten. Cot: Name Genre DetailType ParticularType."""
    d = open(path, "rb").read().split(b"\n")
    hdr = decline2(d[0].rstrip(b"\r")).split("\t")
    iName = 0
    iG = hdr.index("Genre") if "Genre" in hdr else hdr.index("ItemGenre")
    iD = hdr.index("DetailType")
    iP = hdr.index("ParticularType")
    out = {}
    for ln in d[1:]:
        c = decline2(ln.rstrip(b"\r")).split("\t")
        if len(c) <= max(iG, iD, iP):
            continue
        try:
            key = (int(c[iG]), int(c[iD]), int(c[iP]))
        except ValueError:
            continue
        out[key] = c[iName].strip()
    return out


def norm(s):
    return re.sub(r"\s+", " ", s.strip().lower())


def main():
    lnx = load_items(LNX_ITEM)
    jx1 = load_items(JX1_ITEM)
    # ten JX1 -> danh sach key
    jx1_by_name = {}
    for k, v in jx1.items():
        jx1_by_name.setdefault(norm(v), []).append(k)

    rows = list(csv.reader(io.open(os.path.join(P3, "id_dungdo.csv"), encoding="utf-8")))
    resolved = {}
    report = []
    for r in rows[1:]:
        if r[0].strip("\ufeff") != "ITEM" or "NANG" not in r[4]:
            continue
        src = r[1].strip()
        cands = re.findall(r"[0-9]+,[0-9]+,[0-9]+", r[5].split("->", 1)[-1])
        g, dd, pt = map(int, src.split(","))
        src_name = lnx.get((g, dd, pt), "")
        # uu tien: dich co TEN trung ten nguon Linux
        pick = None
        why = ""
        if src_name:
            for c in cands:
                cg, cdd, cpt = map(int, c.split(","))
                if norm(jx1.get((cg, cdd, cpt), "")) == norm(src_name):
                    pick = c
                    why = "ten trung"
                    break
        # phu: cung genre+detail (6,1) va la ung vien duy nhat cung genre
        if not pick:
            same = [c for c in cands if c.split(",")[0] == str(g) and c.split(",")[1] == str(dd)]
            if len(same) == 1:
                pick = same[0]
                why = "cung genre (duy nhat)"
        if not pick and cands:
            pick = cands[-1]  # cuoi cung thay vi dau (dau hay la genre khac)
            why = "MAC DINH cuoi - CAN KIEM TAY"
        resolved[src] = pick
        report.append((src, src_name, pick, jx1.get(tuple(map(int, pick.split(","))), "?") if pick else "?", why))

    json = __import__("json")
    json.dump(resolved, io.open(os.path.join(HERE, "remap_resolved.json"), "w", encoding="utf-8"),
              ensure_ascii=False, indent=1)
    print("| Nguon (Linux) | Ten Linux | -> Dich JX1 | Ten JX1 | Ly do |")
    print("|---|---|---|---|---|")
    for src, sn, pk, jn, why in report:
        mark = "**%s**" % why if "CAN KIEM" in why else why
        print("| %s | %s | %s | %s | %s |" % (src, sn[:24], pk, jn[:24], mark))


if __name__ == "__main__":
    main()
