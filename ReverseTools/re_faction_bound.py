#!/usr/bin/env python3
# -*- coding: ascii -*-
"""re_faction_bound.py - Cho biet MOT nhi phan may chu Linux (jx_linux_y) do bao nhieu MON PHAI.

Cach do: tim chuoi duong dan bang mon phai, lan nguoc ra ham nap,
doc hang so bien vong lap (cmp <reg>, N) va buoc mang (add <reg>, 0xCC).
Ngoai ra dem bang ten thuoc tinh phep (khoi chuoi bat dau bang 'skill_begin').

DUNG:  python re_faction_bound.py <duong dan jx_linux_y>
"""
import struct, re, sys
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

FACINI = bytes([0x5C]) + b"settings" + bytes([0x5C]) + b"faction" + bytes([0x5C])

def load(p):
    d = open(p, "rb").read()
    if d[:4] != b"\x7fELF":
        return d, None
    e_phoff, = struct.unpack_from("<I", d, 0x1C)
    e_phentsize, e_phnum = struct.unpack_from("<HH", d, 0x2A)
    segs = []
    for i in range(e_phnum):
        o = e_phoff + i * e_phentsize
        t, off, va, pa, fsz, msz, fl, al = struct.unpack_from("<8I", d, o)
        if t == 1 and fsz:
            segs.append((va, fsz, off))
    return d, segs

def main(p):
    d, segs = load(p)
    print("TEP  : %s  (%d byte)" % (p, len(d)))
    if segs is None:
        print("  khong phai ELF32"); return
    o2v = lambda off: next((v + (off - o) for v, sz, o in segs if o <= off < o + sz), None)
    v2o = lambda va: next((o + (va - v) for v, sz, o in segs if v <= va < v + sz), None)

    # 1) bang ten thuoc tinh phep
    i = d.find(b"skill_begin\x00")
    if i < 0:
        print("  BANG THUOC TINH: khong thay 'skill_begin'")
    else:
        n = 0; k = i
        while True:
            j = d.find(b"\x00", k)
            if j < 0 or j == k or j - k > 60: break
            s = d[k:j]
            if not re.match(rb"^[a-z_][a-z0-9_]*$", s): break
            n += 1; k = j + 1
            if n > 900: break
        print("  BANG THUOC TINH PHEP: %d muc (bat dau offset 0x%X)" % (n, i))
        for t in (b"colddamage_v", b"special_point_base", b"cost_sp", b"lightingdamage_p", b"autocastskill"):
            has = any(m.start() and d[m.start()-1:m.start()] == b"\x00"
                      for m in re.finditer(re.escape(t) + rb"\x00", d))
            print("     %-20s %s" % (t.decode(), "CO" if has else "khong"))

    # 2) bien so mon phai
    pos = d.find(FACINI)
    if pos < 0:
        print("  MON PHAI: khong thay chuoi bang mon phai"); return
    st = d.rfind(b"\x00", 0, pos) + 1
    sva = o2v(st)
    print("  chuoi ini @VA 0x%X : %r" % (sva, d[st:d.find(b'\x00', st)]))
    xr = [o2v(m.start()) for m in re.finditer(re.escape(struct.pack("<I", sva)), d)]
    xr = [x for x in xr if x]
    if not xr:
        print("  MON PHAI: khong tim duoc cho tham chieu"); return
    use = xr[0]
    # lan nguoc tim prologue
    fn = None
    for s in range(use - 0x600, use):
        o = v2o(s)
        if o and d[o] == 0x55 and d[o+1] == 0x89 and d[o+2] == 0xE5:
            fn = s
    if fn is None:
        print("  MON PHAI: khong tim duoc dau ham"); return
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    off = v2o(fn)
    bounds = []; stride = set()
    for ins in md.disasm(d[off:off + 0x600], fn):
        if ins.mnemonic == "cmp":
            m = re.search(r",\s*(0x[0-9a-f]+|\d+)$", ins.op_str)
            if m:
                v = int(m.group(1), 0)
                if 2 <= v <= 64: bounds.append((hex(ins.address), v))
        if ins.mnemonic == "add" and "0xcc" in ins.op_str: stride.add(0xCC)
        if ins.mnemonic == "ret": break
    print("  HAM NAP MON PHAI @0x%X" % fn)
    print("     buoc mang: %s" % ([hex(x) for x in stride] or "?"))
    print("     cac bien so sanh trong ham: %s" % bounds)
    cands = [v for _, v in bounds if 8 <= v <= 20]
    if cands:
        print("     ==> SO MON PHAI DOC DUOC = %d  (muc 0..%d)" % (max(cands), max(cands) - 1))

if __name__ == "__main__":
    for p in sys.argv[1:]:
        main(p); print()
