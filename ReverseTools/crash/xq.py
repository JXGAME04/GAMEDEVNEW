# -*- coding: utf-8 -*-
"""Phu tro mo PE x64 khong PDB:
  exp <rva>...    -> export dung truoc/sau moi rva (dinh vi vung ma)
  str <rvaBd> <rvaKt>  -> moi chuoi ma doan ma nay tro toi bang lea rip-relative
"""
import struct, sys, bisect, re
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from capstone import Cs, CS_ARCH_X86, CS_MODE_64
sys.path.insert(0, __import__("os").path.dirname(__import__("os").path.abspath(__file__)))
from mo_x64 import PE

pe = PE(sys.argv[1]); cmd = sys.argv[2]

def exports():
    rva, sz = pe.dirs[0]
    o = pe.off(rva)
    nfun, nname = struct.unpack_from("<II", pe.d, o + 20)
    afun, anam, aord = struct.unpack_from("<III", pe.d, o + 28)
    of, on, oo = pe.off(afun), pe.off(anam), pe.off(aord)
    out = []
    for i in range(nname):
        nrva, = struct.unpack_from("<I", pe.d, on + 4 * i)
        no = pe.off(nrva)
        end = pe.d.index(b"\0", no)
        nm = pe.d[no:end].decode("latin-1")
        ordi, = struct.unpack_from("<H", pe.d, oo + 2 * i)
        frva, = struct.unpack_from("<I", pe.d, of + 4 * ordi)
        out.append((frva, nm))
    out.sort()
    return out

if cmd == "exp":
    ex = exports(); ks = [e[0] for e in ex]
    print("tong export:", len(ex))
    for a in sys.argv[3:]:
        rva = int(a, 16)
        i = bisect.bisect_right(ks, rva) - 1
        b = ex[i] if i >= 0 else None
        n = ex[i + 1] if i + 1 < len(ex) else None
        print("\n  rva %s" % hex(rva))
        if b: print("     truoc: %-70s (+0x%X)" % (b[1][:70], rva - b[0]))
        if n: print("     sau  : %-70s (-0x%X)" % (n[1][:70], n[0] - rva))
elif cmd == "str":
    b, e = int(sys.argv[3], 16), int(sys.argv[4], 16)
    md = Cs(CS_ARCH_X86, CS_MODE_64); md.detail = False
    code = pe.read(b, e - b)
    seen = []
    for ins in md.disasm(code, pe.base + b):
        m = re.search(r"\[rip \+ (0x[0-9a-f]+)\]", ins.op_str)
        if not m: continue
        tgt = ins.address + ins.size + int(m.group(1), 16) - pe.base
        raw = pe.read(tgt, 120)
        t = re.match(rb"[\x20-\x7e]{4,}", raw)
        if t:
            s = t.group(0).decode("latin-1")
            if s not in seen:
                seen.append(s)
                print("  %08X %-6s -> \"%s\"" % (ins.address - pe.base, ins.mnemonic, s[:100]))
    if not seen: print("  (khong co chuoi nao)")
