# -*- coding: utf-8 -*-
"""D:\\ServerLinux co HAI goc du lieu may chu, khong phai mot:
     A = D:\\ServerLinux\\server1   (thu muc chay)
     B = D:\\ServerLinux\\Patch     (lop cap nhat - CO CA script\\ va settings\\)
Script nay do do lech giua hai goc, va liet ke nhung tep CHI CO O B
(vi du bang toa do sinh quai cua Phong Lang Do nam o B ma khong co o A).

Ten thu muc/tep tieng Trung tren dia duoc luu bang byte GBK tho -> Python doc ra
theo ma trang ANSI (CP1258) nen ra mojibake. Phai so sanh bang BYTE, khong so bang str.

Chay: python scan_patch_root.py
Ra:   port_3hd/08_hai_goc_dulieu.md  +  port_3hd/chi_co_o_patch.txt
"""
import io, os, sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools")
from gbktool import decline  # noqa

HERE = os.path.dirname(os.path.abspath(__file__))
A = r"D:\ServerLinux\server1"
B = r"D:\ServerLinux\Patch"
SUBS = ["settings", "script", "maps"]


def show(rel):
    """rel la str doc tu he thong tep -> in ra dang doc duoc."""
    try:
        return decline(rel.encode("mbcs", "replace"))
    except Exception:
        return rel


def index(root, sub):
    d = os.path.join(root, sub)
    out = {}
    if not os.path.isdir(d):
        return out
    for dp, dn, fn in os.walk(d):
        for f in fn:
            p = os.path.join(dp, f)
            rel = os.path.relpath(p, d)
            out[rel.lower()] = (rel, os.path.getsize(p))
    return out


def main():
    lines = ["# HAI GOC DU LIEU cua ban Linux - dung nham la mat du lieu",
             "",
             "| Goc | Duong dan |",
             "|---|---|",
             "| A (thu muc chay) | `D:\\ServerLinux\\server1` |",
             "| B (lop cap nhat) | `D:\\ServerLinux\\Patch` |",
             "",
             "**B KHONG chi la client**: no co ca `script\\`, `settings\\`, `maps\\`.",
             "Mot so bang du lieu chi ton tai o B (vi du toan bo cac thu muc khu vuc",
             "ten tieng Trung trong `settings\\maps\\`).",
             ""]
    only_b_all = []
    for sub in SUBS:
        ia, ib = index(A, sub), index(B, sub)
        only_a = sorted(set(ia) - set(ib))
        only_b = sorted(set(ib) - set(ia))
        both = sorted(set(ia) & set(ib))
        diff = [k for k in both if ia[k][1] != ib[k][1]]
        lines += ["## %s\\" % sub, "",
                  "| Chi tieu | So tep |", "|---|---|",
                  "| co o A | %d |" % len(ia),
                  "| co o B | %d |" % len(ib),
                  "| CHI CO O A | %d |" % len(only_a),
                  "| **CHI CO O B** | **%d** |" % len(only_b),
                  "| co ca hai, LECH KICH THUOC | %d |" % len(diff),
                  ""]
        if only_b:
            lines += ["### Tep CHI CO O B (`Patch\\%s`) - toi da 200 dong dau" % sub, "",
                      "| Tep | Byte |", "|---|---|"]
            for k in only_b[:200]:
                rel, sz = ib[k]
                lines.append("| `%s` | %d |" % (show(rel), sz))
                only_b_all.append("%s\\%s\t%d" % (sub, show(rel), sz))
            if len(only_b) > 200:
                lines.append("| ... con %d tep nua (xem chi_co_o_patch.txt) |  |" % (len(only_b) - 200))
                for k in only_b[200:]:
                    rel, sz = ib[k]
                    only_b_all.append("%s\\%s\t%d" % (sub, show(rel), sz))
            lines.append("")
        if diff:
            lines += ["### Tep co ca hai nhung LECH KICH THUOC (`%s`) - toi da 100 dong" % sub, "",
                      "| Tep | Byte A | Byte B |", "|---|---|---|"]
            for k in diff[:100]:
                lines.append("| `%s` | %d | %d |" % (show(ia[k][0]), ia[k][1], ib[k][1]))
            if len(diff) > 100:
                lines.append("| ... con %d tep nua |  |  |" % (len(diff) - 100))
            lines.append("")
        print("%-10s A=%-5d B=%-5d chiA=%-5d chiB=%-5d lech=%d"
              % (sub, len(ia), len(ib), len(only_a), len(only_b), len(diff)))

    with io.open(os.path.join(HERE, "08_hai_goc_dulieu.md"), "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines))
    with io.open(os.path.join(HERE, "chi_co_o_patch.txt"), "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(only_b_all))


if __name__ == "__main__":
    main()
