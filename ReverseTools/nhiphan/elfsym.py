# -*- coding: utf-8 -*-
import io, struct, sys, re
class ELF32:
    def __init__(self, path):
        self.d = d = io.open(path, "rb").read()
        e_phoff, = struct.unpack_from("<I", d, 0x1C)
        e_phentsize, e_phnum = struct.unpack_from("<HH", d, 0x2A)
        self.loads = []; dyn = None
        for i in range(e_phnum):
            o = e_phoff + i*e_phentsize
            ptype, off, va, pa, filesz, memsz, flags, align = struct.unpack_from("<8I", d, o)
            if ptype == 1: self.loads.append((va, filesz, memsz, off))
            if ptype == 2: dyn = (off, filesz)
        self.T = {}
        off, sz = dyn; i = 0
        while i < sz:
            tag, val = struct.unpack_from("<iI", d, off+i)
            if tag == 0: break
            self.T.setdefault(tag, []).append(val); i += 8
        self.strtab = self.v2o(self.T[5][0]); self.symtab = self.v2o(self.T[6][0])
        self.syment = self.T.get(11, [16])[0]
    def v2o(self, va):
        for va0, fsz, msz, off in self.loads:
            if va0 <= va < va0+fsz: return off + (va-va0)
        for va0, fsz, msz, off in self.loads:      # tolerant: .bss etc.
            if va0 <= va < va0+msz: return None
        return None
    def s(self, stoff):
        if stoff is None: return ""
        e = self.d.index(b"\0", stoff); return self.d[stoff:e].decode("latin-1")
    def sym(self, i):
        o = self.symtab + i*self.syment
        st_name, st_value, st_size, st_info, st_other, st_shndx = struct.unpack_from("<IIIBBH", self.d, o)
        return self.s(self.strtab+st_name) if st_name else "", st_value, st_size, st_info, st_shndx
    def nsyms(self):
        # dung hash/gnu_hash de biet so ky hieu; fallback: quet toi khi ten rong lien tiep
        #
        # [SUA 16/09] Ban cu quet mu toi 200000 muc va goi self.sym() khong chan bo -> tren
        # librainbow.so (khong co DT_HASH) no doc qua cuoi tep roi chet ValueError trong s().
        # Nay chan bang HAI moc chac chan:
        #   - tran cung: (strtab - symtab) // syment, dung cach Lib trong dis2.py van tinh;
        #   - va do dai tep.
        n = 0
        if 4 in self.T:                      # DT_HASH
            h = self.v2o(self.T[4][0])
            nbucket, nchain = struct.unpack_from("<II", self.d, h); return nchain
        tran = 200000
        if self.strtab > self.symtab:        # bang chuoi thuong nam ngay sau bang ky hieu
            tran = min(tran, (self.strtab - self.symtab) // self.syment)
        tran = min(tran, max(0, (len(self.d) - self.symtab) // self.syment))
        while n < tran:
            try:
                nm, v, sz, info, shndx = self.sym(n)
            except Exception:
                break
            if n > 0 and not nm and v == 0 and sz == 0 and info == 0: break
            n += 1
        return n
    def needed(self): return [self.s(self.strtab+v) for v in self.T.get(1, [])]
