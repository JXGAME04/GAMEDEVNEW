# -*- coding: utf-8 -*-
r"""In bang nhap PE (DLL phu thuoc) de nhan dang CRT: ucrtbased/VCRUNTIME140D = DEBUG,
api-ms-win-crt-*/ucrtbase = RELEASE (/MD), khong co gi = CRT TINH (/MT)."""
import struct, io, sys

def imports(path):
    b = io.open(path, "rb").read()
    e = struct.unpack_from("<I", b, 0x3C)[0]
    nsect = struct.unpack_from("<H", b, e + 6)[0]
    opt = e + 24
    magic = struct.unpack_from("<H", b, opt)[0]
    dd = opt + (96 if magic == 0x10B else 112)
    irva, isz = struct.unpack_from("<II", b, dd + 8)   # DataDirectory[1] = Import
    sect_off = opt + struct.unpack_from("<H", b, e + 20)[0]
    sects = []
    for i in range(nsect):
        o = sect_off + i * 40
        vsize, vaddr, rawsize, rawptr = struct.unpack_from("<IIII", b, o + 8)
        sects.append((vaddr, max(vsize, rawsize), rawptr))
    def r2o(rva):
        for va, sz, rp in sects:
            if va <= rva < va + sz:
                return rp + (rva - va)
        return None
    out = []
    off = r2o(irva)
    if off is None: return out
    i = 0
    while True:
        ent = b[off + i*20: off + i*20 + 20]
        if len(ent) < 20 or ent == b"\0"*20: break
        namerva = struct.unpack_from("<I", ent, 12)[0]
        if not namerva: break
        no = r2o(namerva)
        if no is None: break
        nm = b[no:b.index(b"\0", no)].decode("latin-1")
        out.append(nm)
        i += 1
    return out

for p in sys.argv[1:]:
    imps = imports(p)
    crt = [x for x in imps if "crt" in x.lower() or "vcruntime" in x.lower() or "msvcr" in x.lower()]
    dbg = any("d.dll" in x.lower() and ("ucrtbased" in x.lower() or "140d" in x.lower()) for x in crt)
    kind = "DEBUG (/MDd)" if dbg else ("RELEASE (/MD)" if crt else "CRT TINH (/MT) - khong nhap CRT")
    print("%-34s %-22s %s" % (p.split("\\")[-1], kind, ",".join(crt[:4]) if crt else ""))
