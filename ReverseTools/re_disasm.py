#!/usr/bin/env python3
# -*- coding: ascii -*-
"""
re_disasm.py - Dich nguoc mot doan ma theo DIA CHI AO, cho ca hai ban goc.

Ho tro 2 dinh dang:
  * ELF32 (jx_linux_y)          : anh xa dia chi ao qua bang PT_LOAD
  * anh PE DA GIAI NEN (.bin)   : dia chi ao = 0x401000 + do lech trong tep

DUNG:
    python re_disasm.py D:/ServerLinux/server1/jx_linux_y 0x080FB820 60
    python re_disasm.py D:/ServerLinux/Patch/game_y_unpacked.bin 0x006429A0 40
    python re_disasm.py <tep> --find "chuoi can tim"     # tim ai tham chieu chuoi

Can: pip install capstone
"""
import struct, sys
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

PE_IMAGE_BASE = 0x401000   # cho anh game_y da giai nen

def map_file(path):
    d = open(path, 'rb').read()
    if d[:4] == b'\x7fELF':
        e_phoff, = struct.unpack_from('<I', d, 0x1C)
        e_phentsize, e_phnum = struct.unpack_from('<HH', d, 0x2A)
        segs = []
        for i in range(e_phnum):
            o = e_phoff + i * e_phentsize
            ptype, off, va, pa, filesz, memsz, flags, align = struct.unpack_from('<8I', d, o)
            if ptype == 1 and filesz:
                segs.append((va, filesz, off))
        def v2o(va):
            for v, sz, off in segs:
                if v <= va < v + sz:
                    return off + (va - v)
            return None
        def o2v(off):
            for v, sz, o in segs:
                if o <= off < o + sz:
                    return v + (off - o)
            return None
        return d, v2o, o2v, 'ELF32'
    # anh PE phang da giai nen
    return (d,
            lambda va: va - PE_IMAGE_BASE if 0 <= va - PE_IMAGE_BASE < len(d) else None,
            lambda off: off + PE_IMAGE_BASE,
            'PE-GIAINEN')

def disasm(path, va, count):
    d, v2o, o2v, kind = map_file(path)
    off = v2o(va)
    if off is None:
        print("dia chi 0x%08X khong nam trong tep (%s)" % (va, kind)); return
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = False
    print("# %s  %s  bat dau 0x%08X (do lech tep 0x%X)" % (path, kind, va, off))
    for ins in md.disasm(d[off:off + count * 16], va):
        print("0x%08X  %-22s %s %s" % (ins.address, ins.bytes.hex(), ins.mnemonic, ins.op_str))
        count -= 1
        if count <= 0:
            break

def find_str(path, s):
    """Tim chuoi roi tim moi lenh nap DIA CHI cua no (push imm / mov imm)."""
    d, v2o, o2v, kind = map_file(path)
    raw = s.encode('latin-1')
    pos = -1
    hits = []
    while True:
        pos = d.find(raw, pos + 1)
        if pos < 0:
            break
        hits.append(pos)
    if not hits:
        print("khong thay chuoi"); return
    for h in hits:
        va = o2v(h)
        print("chuoi tai do lech 0x%X = dia chi ao 0x%08X" % (h, va if va else 0))
        if va is None:
            continue
        pat = struct.pack('<I', va)
        p = -1
        n = 0
        while n < 12:
            p = d.find(pat, p + 1)
            if p < 0:
                break
            ref = o2v(p)
            if ref:
                print("    duoc tham chieu tai 0x%08X (do lech 0x%X)" % (ref, p))
                n += 1

if __name__ == '__main__':
    path = sys.argv[1]
    if '--find' in sys.argv:
        find_str(path, sys.argv[sys.argv.index('--find') + 1])
    else:
        va = int(sys.argv[2], 0)
        cnt = int(sys.argv[3]) if len(sys.argv) > 3 else 40
        disasm(path, va, cnt)
