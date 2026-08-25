# -*- coding: utf-8 -*-
"""ins_block.py - chen MOT KHOI ma nguon (co the co tieng Viet) vao file TCVN3.

Vi sao: safe_edit.py tu choi non-ASCII, con vn_edit.py chi nhan chuoi ngan tren
dong lenh. Khoi ma dai (may trang thai) can duoc soan trong mot file UTF-8 roi
chuyen sang TCVN3 va chen vao dung cho.

Dung:
    python ins_block.py <file-dich> <file-khoi-utf8> --after  "<neo ASCII>"
    python ins_block.py <file-dich> <file-khoi-utf8> --before "<neo ASCII>"
    python ins_block.py <file-dich> <file-khoi-utf8> --replace "<neo ASCII>"

Bao dam:
  * file dich doc/ghi latin-1 (anh xa 1:1 byte) - khong pha byte TCVN3 co san.
  * neo phai la ASCII va xuat hien DUNG 1 lan.
  * in so byte >127 truoc/sau; khoi moi chi duoc lam tang dung so byte cua no.
  * giu CRLF cua file dich cho moi dong chen vao.
"""
import argparse
import io
import os
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes


def hi(s):
    return sum(1 for c in s if ord(c) > 127)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("target")
    ap.add_argument("block")
    ap.add_argument("--after")
    ap.add_argument("--before")
    ap.add_argument("--replace")
    a = ap.parse_args()

    neo = a.after or a.before or a.replace
    if not neo:
        print("PHAI cho mot trong --after / --before / --replace")
        return 2
    if any(ord(c) > 127 for c in neo):
        print("neo phai la ASCII")
        return 2

    with io.open(a.target, "r", encoding="latin-1", newline="") as f:
        src = f.read()
    with io.open(a.block, "r", encoding="utf-8", newline="") as f:
        blk = f.read()

    n = src.count(neo)
    if n != 1:
        print("neo khop %d lan (phai dung 1): %r" % (n, neo))
        return 3

    blk_b = unicode_to_tcvn3_bytes(blk).decode("latin-1")
    blk_b = blk_b.replace("\r\n", "\n").replace("\n", "\r\n")
    hi_src, hi_blk = hi(src), hi(blk_b)

    if a.replace:
        out = src.replace(neo, blk_b.rstrip("\r\n"))
    else:
        i = src.index(neo)
        if a.after:
            j = src.find("\n", i)
            j = len(src) if j < 0 else j + 1
            out = src[:j] + blk_b + src[j:]
        else:
            j = src.rfind("\n", 0, i)
            j = 0 if j < 0 else j + 1
            out = src[:j] + blk_b + src[j:]

    with io.open(a.target, "w", encoding="latin-1", newline="") as f:
        f.write(out)
    print("OK %s: high byte %d -> %d (khoi them %d), %d -> %d dong"
          % (os.path.basename(a.target), hi_src, hi(out), hi_blk,
             src.count("\n"), out.count("\n")))
    return 0


if __name__ == "__main__":
    sys.exit(main())
