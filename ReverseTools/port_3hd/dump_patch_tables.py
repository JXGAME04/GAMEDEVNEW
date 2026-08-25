# -*- coding: utf-8 -*-
"""Xuat cac BANG TOA DO chi ton tai o goc B (`D:\\ServerLinux\\Patch\\settings`)
ma 3 tinh nang can - dac biet Phong Lang Do.

Ten thu muc tieng Trung tren dia la byte GBK tho; Python doc theo ma trang ANSI (CP1258)
nen ra mojibake. Vi vay o day duyet cay va so khop bang TEN DA GIAI MA, khong so bang str tho.

Chay: python dump_patch_tables.py
Ra:   port_3hd/src_utf8/_patch_settings/<duong-dan-da-giai-ma>
      port_3hd/09_bang_toado_patch.md
"""
import io, os, sys, re

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools")
from dec2 import decline2 as decline  # noqa  [va v2]

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = r"D:\ServerLinux\Patch\settings"
A_ROOT = r"D:\ServerLinux\server1\settings"
OUT = os.path.join(HERE, "src_utf8", "_patch_settings")

# chi lay nhung bang lien quan 3 tinh nang (khop tren TEN DA GIAI MA)
KEEP = re.compile(
    "|".join([
        "\u98ce\u9675\u6e21",      # Phong Lang Do
        "\u6e21\u8239",            # do thuyen (boat)
        "challengeoftime",
        "killer", "shashou", "\u6740\u624b",   # sat thu
        "goldboss", "boss",
    ]), re.I)


def dec(name):
    return decline(name.encode("mbcs", "replace"))


def main():
    rows = []
    for dp, dn, fn in os.walk(ROOT):
        for f in fn:
            src = os.path.join(dp, f)
            rel_raw = os.path.relpath(src, ROOT)
            rel = "\\".join(dec(part) for part in rel_raw.split(os.sep))
            if not KEEP.search(rel):
                continue
            data = open(src, "rb").read()
            body = "\n".join(decline(l.rstrip(b"\r")) for l in data.split(b"\n"))
            dst = os.path.join(OUT, *rel.split("\\"))
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            with io.open(dst, "w", encoding="utf-8", newline="\n") as fh:
                fh.write(body)
            head = [l for l in body.split("\n")[:3]]
            # co tep cung duong dan ben goc A khong? (so bang BYTE cua ten thu muc)
            a = os.path.join(A_ROOT, rel_raw)
            in_a = os.path.isfile(a)
            same = in_a and open(a, "rb").read() == data
            rows.append((rel, len(data), len(body.split("\n")), " / ".join(head), in_a, same))

    rows.sort()
    only_b = [r for r in rows if not r[4]]
    md = ["# Bang toa do / bang du lieu LIEN QUAN 3 tinh nang, trich tu goc B",
          "(`D:\\ServerLinux\\Patch\\settings`)",
          "",
          "> **DINH CHINH (vong 2).** Ban dau tep nay ghi *\"nhung tep nay KHONG co trong server1\"*",
          "> cho ca %d bang — **noi qua**. Bo loc chi chon theo TEN, khong kiem goc A." % len(rows),
          "> Do lai: **%d/%d bang thuc su CHI CO O B**, so con lai co san o goc A" % (len(only_b), len(rows)),
          "> (phan lon trung byte). Cot `Chi co o B` duoi day la ket qua do that.",
          "",
          "Vi du bang CHI CO O B va la du lieu loi: `fld_head.lua:18` doc",
          "`\\settings\\maps\\\u4e2d\u539f\u5317\u533a\\\u6e21\u8239\\\u6e21\u8239\u5237\u602a\u70b9.txt`.",
          "Ban da giai ma UTF-8 nam o `src_utf8/_patch_settings/`.",
          "",
          "## Bang CHI CO O goc B (%d)" % len(only_b),
          "",
          "| Tep | Byte | Dong | 3 dong dau |",
          "|---|---|---|---|"]
    for rel, nb, nl, head, in_a, same in only_b:
        md.append("| `%s` | %d | %d | `%s` |" % (rel, nb, nl, head.replace("|", "\\|")))
    md += ["",
           "## Toan bo %d bang da trich (ke ca bang co san o goc A)" % len(rows),
           "",
           "| Tep | Byte | Dong | Co o goc A | Trung byte |",
           "|---|---|---|---|---|"]
    for rel, nb, nl, head, in_a, same in rows:
        md.append("| `%s` | %d | %d | %s | %s |"
                  % (rel, nb, nl, "co" if in_a else "**KHONG**", "y het" if same else ("LECH" if in_a else "-")))
    md.append("")
    with io.open(os.path.join(HERE, "09_bang_toado_patch.md"), "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(md))
    print("da xuat", len(rows), "bang ->", OUT)
    print("trong do CHI CO O goc B:", len(only_b))
    for rel, nb, nl, _, in_a, _s in only_b:
        print("  [chi-B] %-62s %6d B  %3d dong" % (rel, nb, nl))


if __name__ == "__main__":
    main()
