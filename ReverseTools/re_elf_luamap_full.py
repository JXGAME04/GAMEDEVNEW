# -*- coding: utf-8 -*-
"""
lm_full.py - quet TOAN BO bang dang ky Lua trong 1 ELF, co xu ly tail-merge.
Y tuong: moi cap dword lien tiep (a,b):
   a tro toi mot chuoi dinh danh C hop le (bat ky vi tri nao, ke ca giua chuoi khac)
   b nam trong doan ma (PF_X)
=> ghi nhan (ten, dia chi ham).
Loc nhieu: doi hoi cap nam trong mot day >= 2 cap lien tiep hop le.
"""
import struct, sys, re


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


IDCHR = re.compile(rb'[A-Za-z_][A-Za-z0-9_]{2,63}\x00')


def scan(path, minrun=2):
    d, segs = load_elf(path)
    text = [s for s in segs if s['flags'] & 1]

    def in_text(va):
        return any(s['va'] <= va < s['va'] + s['sz'] for s in text)

    def v2o(va):
        for s in segs:
            if s['va'] <= va < s['va'] + s['sz']:
                return s['off'] + (va - s['va'])
        return None

    def strat(va):
        o = v2o(va)
        if o is None:
            return None
        m = IDCHR.match(d, o)
        if not m:
            return None
        return m.group()[:-1].decode('latin-1')

    out = {}
    for s in segs:
        blob = d[s['off']:s['off'] + s['sz']]
        n = len(blob) // 4
        w = struct.unpack_from('<%dI' % n, blob, 0)
        i = 0
        while i < n - 1:
            nm = strat(w[i]) if w[i] else None
            if nm and in_text(w[i + 1]):
                j = i
                run = []
                while j < n - 1:
                    nm2 = strat(w[j]) if w[j] else None
                    if not (nm2 and in_text(w[j + 1])):
                        break
                    run.append((nm2, w[j + 1]))
                    j += 2
                if len(run) >= minrun:
                    for a, b in run:
                        out.setdefault(a, b)
                i = j
            else:
                i += 1
    return out


if __name__ == '__main__':
    p = sys.argv[1]
    mr = int(sys.argv[2]) if len(sys.argv) > 2 else 2
    o = scan(p, mr)
    print("# lm_full %s  minrun=%d  %d ten" % (p, mr, len(o)))
    for nm, va in sorted(o.items(), key=lambda kv: kv[1]):
        print("0x%08X  %s" % (va, nm))
