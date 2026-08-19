#!/usr/bin/env python3
# -*- coding: ascii -*-
"""
re_elf_luamap.py - Trich TEN HAM tu may chu Linux goc jx_linux_y.

VI SAO CAN: jx_linux_y da bi luoc bang muc (e_shoff = 0) va .dynsym chi con 25 ky
hieu -> KHONG co ten ham game. Nhung may chu dang ky ham cho Lua bang mot BANG
gom cac cap (con tro toi chuoi ten, con tro toi ma). Quet bang do la lay lai duoc
hang tram ten ham that.

DUNG:
    python re_elf_luamap.py D:/ServerLinux/server1/jx_linux_y > luamap.txt
    python re_elf_luamap.py <elf> --min 8
"""
import struct, re, sys

def load_elf(path):
    d = open(path, 'rb').read()
    e_phoff, = struct.unpack_from('<I', d, 0x1C)
    e_phentsize, e_phnum = struct.unpack_from('<HH', d, 0x2A)
    segs = []
    for i in range(e_phnum):
        o = e_phoff + i * e_phentsize
        ptype, off, vaddr, paddr, filesz, memsz, flags, align = struct.unpack_from('<8I', d, o)
        if ptype == 1 and filesz:
            segs.append(dict(va=vaddr, sz=filesz, off=off, flags=flags))
    return d, segs

def make_v2o(segs):
    def v2o(va):
        for s in segs:
            if s['va'] <= va < s['va'] + s['sz']:
                return s['off'] + (va - s['va'])
        return None
    return v2o

def scan(path, minrun=8):
    d, segs = load_elf(path)
    text = [s for s in segs if s['flags'] & 1]          # PF_X = doan ma
    def in_text(va):
        return any(s['va'] <= va < s['va'] + s['sz'] for s in text)

    # 1) chi muc moi chuoi dinh danh C -> dia chi ao
    idpat = re.compile(rb'[A-Za-z_][A-Za-z0-9_]{3,60}\x00')
    straddr = {}
    for s in segs:
        blob = d[s['off']:s['off'] + s['sz']]
        for m in idpat.finditer(blob):
            straddr[s['va'] + m.start()] = m.group()[:-1].decode('latin-1')

    # 2) tim day (con tro chuoi, con tro ham) lien tiep
    tables = []
    for s in segs:
        blob = d[s['off']:s['off'] + s['sz']]
        n = len(blob) // 4
        w = struct.unpack_from('<%dI' % n, blob, 0)
        i = 0
        while i < n - 1:
            if w[i] in straddr and in_text(w[i + 1]):
                j = i; run = []
                while j < n - 1 and w[j] in straddr and in_text(w[j + 1]):
                    run.append((straddr[w[j]], w[j + 1])); j += 2
                if len(run) >= minrun:
                    tables.append((s['va'] + i * 4, run))
                i = j
            else:
                i += 1
    return tables

if __name__ == '__main__':
    path = sys.argv[1] if len(sys.argv) > 1 else 'D:/ServerLinux/server1/jx_linux_y'
    minrun = 8
    if '--min' in sys.argv:
        minrun = int(sys.argv[sys.argv.index('--min') + 1])
    tabs = scan(path, minrun)
    tabs.sort(key=lambda x: -len(x[1]))
    seen = {}
    for va, run in tabs:
        for nm, fn in run:
            seen.setdefault(nm, fn)
    print("# ten ham trich tu bang dang ky Lua cua %s" % path)
    print("# %d bang, %d ten ham duy nhat" % (len(tabs), len(seen)))
    print("# dinh dang:  <dia chi ao>  <ten ham>")
    for nm, fn in sorted(seen.items(), key=lambda kv: kv[1]):
        print("0x%08X  %s" % (fn, nm))
