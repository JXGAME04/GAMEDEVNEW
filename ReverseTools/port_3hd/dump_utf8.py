# -*- coding: utf-8 -*-
"""Xuat ban DA GIAI MA (UTF-8) cua toan bo tep Lua thuoc bao dong 3 tinh nang,
cong them cac bang du lieu .txt/.ini lien quan, de phien sau doc thang khong can cong cu.

Chay:  python dump_utf8.py
Ra:    port_3hd/src_utf8/<tinh_nang>/<duong_dan_goc>.lua      (UTF-8, giu nguyen so dong)
       port_3hd/src_utf8/_settings/...
       port_3hd/src_utf8/INDEX.md
"""
import io, os, sys, json

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools")
from gbktool import detect  # noqa
from dec2 import decline2 as decline, detect2  # noqa  [va v2: dau cau CP1252 lan trong TCVN3]

HERE = os.path.dirname(os.path.abspath(__file__))
LNX = r"D:\ServerLinux\server1"
LNXS = os.path.join(LNX, "script")
OUT = os.path.join(HERE, "src_utf8")

EXTRA = [
    "settings/killer.ini",
    "settings/goldboss.txt",
    "settings/trigger_challengeoftime.lua",
    "settings/maplist.ini",
    "settings/map_type.txt",
    "settings/cavelist.ini",
    "settings/dungeonmap.ini",
    "settings/huoyuedu/huoyuedu.txt",
    "settings/systemtimetask.txt",
    "settings/revivepos.ini",
]


def conv(src, dst):
    data = open(src, "rb").read()
    enc = detect2(data)
    lines = data.split(b"\n")
    body = "\n".join(decline(l.rstrip(b"\r")) for l in lines)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    with io.open(dst, "w", encoding="utf-8", newline="\n") as f:
        f.write(body)
    return enc, len(lines), len(data)


def main():
    cj = json.load(io.open(os.path.join(HERE, "closure3.json"), encoding="utf-8"))
    idx = ["# Ban da giai ma (UTF-8) cua ma nguon Lua ban Linux",
           "",
           "Nguon goc: `D:\\ServerLinux\\server1` (Lua 4.0, tep goc tron GBK + TCVN3).",
           "Moi tep giu NGUYEN SO DONG so voi tep goc -> trich dan `tep.lua:123` dung cho ca hai ban.",
           ""]
    total = 0
    for feat, rows in cj.items():
        idx.append("## %s" % feat)
        idx.append("")
        idx.append("| # | Tep (tuong doi voi script\\) | Byte | Dong | Ma hoa goc | JX1 da co |")
        idx.append("|---|---|---|---|---|---|")
        n = 0
        for r in sorted(rows, key=lambda x: x["rel"]):
            if not r["linux"]:
                idx.append("| - | `%s` | - | - | - | **KHONG CO TRONG BAN LINUX** |" % r["rel"])
                continue
            src = os.path.join(LNXS, r["rel"].replace("/", os.sep))
            dst = os.path.join(OUT, feat, r["rel"].replace("/", os.sep))
            enc, nl, nb = conv(src, dst)
            n += 1
            total += 1
            idx.append("| %d | `%s` | %d | %d | %s | %s |"
                       % (n, r["rel"], nb, nl, enc, "co" if r["in_jx1"] else "**CHUA CO**"))
        idx.append("")
        print("%-14s %3d tep" % (feat, n))

    idx.append("## _settings (bang du lieu ngoai)")
    idx.append("")
    idx.append("| Tep | Byte | Dong | Ma hoa goc |")
    idx.append("|---|---|---|---|")
    for rel in EXTRA:
        src = os.path.join(LNX, rel.replace("/", os.sep))
        if not os.path.isfile(src):
            idx.append("| `%s` | - | - | KHONG TIM THAY |" % rel)
            continue
        dst = os.path.join(OUT, "_settings", rel.replace("/", os.sep))
        enc, nl, nb = conv(src, dst)
        total += 1
        idx.append("| `%s` | %d | %d | %s |" % (rel, nb, nl, enc))
    idx.append("")

    with io.open(os.path.join(OUT, "INDEX.md"), "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(idx))
    print("TONG:", total, "tep -> ", OUT)


if __name__ == "__main__":
    main()
