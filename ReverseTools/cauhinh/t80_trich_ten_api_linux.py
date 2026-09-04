# -*- coding: utf-8 -*-
"""t80_trich_ten_api_linux.py - trich TOAN BO bang dang ky ham Lua {char* ten, void* fn}
cua D:\\ServerLinux\\server1\\jx_linux_y (ELF i386, bang dang ky con nguyen van trong .rodata).

Ket qua: D:\\GAMEDEVNEW\\ReverseTools\\linux_api_names.txt  (ten<TAB>dia_chi, sap xep theo ten)
Tham so tuy chon: regex loc de in ra man hinh (mac dinh in so luong).
Chi DOC file.
"""
import io
import re
import struct
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ELF = r"D:\ServerLinux\server1\jx_linux_y"
OUT = r"D:\GAMEDEVNEW\ReverseTools\linux_api_names.txt"

d = open(ELF, "rb").read()
e_phoff, = struct.unpack_from("<I", d, 0x1C)
e_phentsize, e_phnum = struct.unpack_from("<HH", d, 0x2A)
segs = []
for i in range(e_phnum):
    o = e_phoff + i * e_phentsize
    ptype, off, va, pa, filesz, memsz, flags, align = struct.unpack_from("<8I", d, o)
    if ptype == 1 and filesz:
        segs.append((va, filesz, off))
CODE_LO, CODE_HI = 0x8048000, 0x82da338     # vung ma that (LOAD dau)


def v2o(va):
    for v, sz, off in segs:
        if v <= va < v + sz:
            return off + (va - v)
    return None


IDENT = re.compile(rb"^[A-Za-z_][A-Za-z0-9_]{1,63}$")


def cstr(va):
    o = v2o(va)
    if o is None:
        return None
    end = d.find(b"\x00", o, o + 65)
    if end < 0:
        return None
    s = d[o:end]
    return s.decode("ascii") if IDENT.match(s) else None


def main():
    loc = re.compile(sys.argv[1], re.I) if len(sys.argv) > 1 else None
    ten2fn = {}
    for v, sz, off in segs:
        end = off + sz - 8
        for o in range(off, end, 4):
            pname, pfn = struct.unpack_from("<II", d, o)
            if not (CODE_LO <= pfn < CODE_HI):
                continue
            if not (0x8048000 <= pname < 0x8400000):
                continue
            # cap {ten, fn} that: phan tu ke tiep cung phai la cap hop le hoac 0 (cuoi bang)
            s = cstr(pname)
            if not s:
                continue
            ten2fn.setdefault(s, set()).add(pfn)
    # loc nhieu: chi giu ten xuat hien trong mot day >= 3 cap lien tiep (bang dang ky)
    # -> don gian hoa: giu tat ca, nhung ghi so dia chi (ten trung nhieu bang la binh thuong)
    rows = sorted(ten2fn.items())
    with io.open(OUT, "w", encoding="utf-8", newline="\n") as f:
        for s, fns in rows:
            f.write("%s\t%s\n" % (s, ",".join("0x%08X" % a for a in sorted(fns))))
    print("tong ten:", len(rows), "->", OUT)
    if loc:
        hits = [(s, fns) for s, fns in rows if loc.search(s)]
        print("khop regex %r: %d" % (sys.argv[1], len(hits)))
        for s, fns in hits:
            print("  %-36s %s" % (s, ",".join("0x%08X" % a for a in sorted(fns))))
    return 0


if __name__ == "__main__":
    sys.exit(main())
