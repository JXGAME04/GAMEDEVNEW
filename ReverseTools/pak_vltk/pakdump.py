# -*- coding: utf-8 -*-
"""Doc pak JX1: liet ke entry, giai nen, tim chuoi."""
import io, os, struct, sys, zlib

sys.stdout.reconfigure(encoding="utf-8", errors="replace")


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


def entries(p):
    f = open(p, "rb")
    sig, count, ioff, doff = struct.unpack("<IIII", f.read(16))
    f.seek(ioff)
    raw = f.read(count * 16)
    out = []
    for i in range(count):
        uid, off, size, cf = struct.unpack("<IIiI", raw[i * 16:i * 16 + 16])
        out.append((uid, off, size, cf))
    return f, out


def read_entry(f, e):
    uid, off, size, cf = e
    csize = cf & 0xFFFFFF
    flag = cf >> 24
    f.seek(off)
    blob = f.read(csize if csize else size)
    if flag == 0 or csize in (0, size):
        return blob[:size] if size > 0 else blob
    try:
        return zlib.decompress(blob)
    except Exception:
        try:
            return zlib.decompressobj().decompress(blob)
        except Exception:
            return blob


if __name__ == "__main__":
    mode = sys.argv[1]
    if mode == "grep":
        pak = sys.argv[2]
        pats = [x.encode("latin-1") for x in sys.argv[3:]]
        f, es = entries(pak)
        print("entries", len(es))
        for e in es:
            d = read_entry(f, e)
            for pt in pats:
                if pt in d:
                    print("HIT uid=%08X size=%d cf=%08X pat=%r" % (e[0], e[2], e[3], pt))
                    break
    elif mode == "id":
        for s in sys.argv[2:]:
            print("%08X" % name2id(s), s)
    elif mode == "find":
        # tim uid trong tat ca pak
        root = sys.argv[2]
        names = sys.argv[3:]
        want = {name2id(n): n for n in names}
        for fn in sorted(os.listdir(root)):
            if not fn.lower().endswith((".pak", ".mps")):
                continue
            try:
                f, es = entries(os.path.join(root, fn))
            except Exception:
                continue
            for e in es:
                if e[0] in want:
                    print("%-24s %-60s size=%d" % (fn, want[e[0]], e[2]))
