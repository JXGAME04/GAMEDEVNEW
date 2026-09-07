# -*- coding: utf-8 -*-
"""Disassemble the unpacked gamecl.exe image of VLTK 2.0 (VA = 0x401000 + offset).
Reuses the UPX call-filter fix from gdis.py (session 17c6a10c). Commands:
  dis <VA> [n]        disassemble n bytes
  func <VA> [max]     disassemble until ret+pad
  calls <VA> [max]    only calls/jumps + annotated lines
  xref <VA>           find absolute dword references
  str <text>          find string (ascii)
  strat <VA>          print string at VA
  funcstart <VA>      find function start containing VA
  vtable <VA>         dump vtable entries at VA (n dwords)
  rtti <name>         locate RTTI type descriptor + vftables for class name
"""
import sys, re, struct, os
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
FIXED = r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\17c6a10c-c4c9-46ff-96ed-31ae14294d55\scratchpad\gamecl_fixed.bin"
BASE = 0x401000
md = Cs(CS_ARCH_X86, CS_MODE_32); md.detail = False
d = open(FIXED, "rb").read()
END = BASE + len(d)

def is_funcstart(b, o):
    if o < 0 or o + 3 > len(b): return False
    if b[o:o+3] == b"\x55\x8b\xec": return True
    if b[o-1] == 0xCC and b[o] != 0xCC: return True
    if b[o-1] == 0xC3 and b[o] in (0x53, 0x55, 0x56, 0x57, 0x6A, 0x81, 0x83, 0x8B, 0xA1, 0xB8, 0xE8, 0xE9, 0x33, 0x51, 0x52): return True
    return False

def rd(va, n):
    o = va - BASE
    if o < 0 or o >= len(d): return b""
    return d[o:o+n]

def cstr(va):
    b = rd(va, 120)
    m = re.match(rb"[\x20-\x7e]{3,}", b)
    if m: return m.group(0).decode()
    # GBK / high-byte text
    m = re.match(rb"[\x20-\x7e\x80-\xfe]{4,}", b)
    if m:
        try: return m.group(0).decode("gbk")
        except Exception: return m.group(0).decode("latin-1")
    return None

def annotate(ins):
    s = "%s %s" % (ins.mnemonic, ins.op_str)
    for m in re.finditer(r"0x([0-9a-f]{6,8})", ins.op_str):
        va = int(m.group(1), 16)
        if BASE <= va < END:
            t = cstr(va)
            if t and not ins.mnemonic.startswith(("call", "j")):
                s += '   ; "%s"' % t[:70]
            elif ins.mnemonic in ("mov", "cmp", "push", "lea"):
                b = rd(va, 4)
                if len(b) == 4:
                    dv = struct.unpack("<I", b)[0]
                    if BASE <= dv < END:
                        s += "   ; [%06X]=%08X" % (va, dv)
    return s

def dis(va, n):
    for ins in md.disasm(rd(va, n), va):
        print("%06X  %-16s %s" % (ins.address, ins.bytes.hex(), annotate(ins)))

def func(va, mx=0x800, only_calls=False):
    for ins in md.disasm(rd(va, mx), va):
        line = "%06X  %-16s %s" % (ins.address, ins.bytes.hex(), annotate(ins))
        if not only_calls or ins.mnemonic in ("call",) or ";" in line or ins.mnemonic.startswith("j") or ins.mnemonic == "ret":
            print(line)
        if ins.mnemonic == "ret":
            nxt = rd(ins.address + ins.size, 3)
            if nxt[:1] in (b"\xcc", b"\x90") or nxt == b"\x55\x8b\xec" or nxt[:2] in (b"\x83\xec", b"\x81\xec", b"\x6a\xff"):
                break
        if ins.mnemonic == "int3": break

def xref(va):
    pat = struct.pack("<I", va); o = 0
    out = []
    while True:
        o = d.find(pat, o)
        if o < 0: break
        out.append(BASE + o); o += 1
    for r in out: print("  ref %06X" % r)
    return out

def funcstart(va):
    o = va - BASE
    while o > 0 and not is_funcstart(d, o): o -= 1
    return BASE + o

cmd = sys.argv[1]
if cmd == "dis": dis(int(sys.argv[2], 16), int(sys.argv[3], 0) if len(sys.argv) > 3 else 0x100)
elif cmd == "func": func(int(sys.argv[2], 16), int(sys.argv[3], 0) if len(sys.argv) > 3 else 0x800)
elif cmd == "calls": func(int(sys.argv[2], 16), int(sys.argv[3], 0) if len(sys.argv) > 3 else 0x800, True)
elif cmd == "xref":
    for r in xref(int(sys.argv[2], 16)):
        print("     in func ~ %06X" % funcstart(r))
elif cmd == "funcstart": print("func start ~ %06X" % funcstart(int(sys.argv[2], 16)))
elif cmd == "str":
    b = sys.argv[2].encode("latin-1"); o = 0
    while True:
        o = d.find(b, o)
        if o < 0: break
        s = o
        while s > 0 and 0x20 <= d[s-1] < 0x7f: s -= 1
        e = o
        while e < len(d) and d[e] != 0: e += 1
        print("  %06X (start %06X): %s" % (BASE + o, BASE + s, d[s:e].decode("latin-1")[:150])); o += 1
elif cmd == "strat":
    va = int(sys.argv[2], 16); o = va - BASE
    s = o
    while s > 0 and 0x20 <= d[s-1] < 0x7f: s -= 1
    e = o
    while e < len(d) and d[e] != 0: e += 1
    print("start %06X: %s" % (BASE + s, d[s:e].decode("latin-1")))
elif cmd == "vtable":
    va = int(sys.argv[2], 16); n = int(sys.argv[3], 0) if len(sys.argv) > 3 else 24
    for i in range(n):
        b = rd(va + 4*i, 4)
        if len(b) < 4: break
        v = struct.unpack("<I", b)[0]
        print("  [%2d] %06X -> %08X %s" % (i, va + 4*i, v, "" if BASE <= v < END else "(out)"))
elif cmd == "rtti":
    name = sys.argv[2].encode()
    # TypeDescriptor: vtable ptr, spare, name ".?AV<name>@@"
    pat = b".?AV" + name + b"@@"
    o = d.find(pat)
    while o >= 0:
        td = BASE + o - 8
        print("TypeDescriptor %06X name %s" % (td, d[o:o+len(pat)].decode()))
        # find CompleteObjectLocator referencing td: COL = {sig, offset, cdOffset, pTypeDescriptor, pClassHierarchy}
        pat2 = struct.pack("<I", td); p = 0
        while True:
            p = d.find(pat2, p)
            if p < 0: break
            colva = BASE + p - 12
            sig, off, cdoff = struct.unpack("<III", d[p-12:p])
            if sig == 0 and off < 0x10000 and cdoff == 0:
                # vftable is the dword right after a pointer to this COL
                pat3 = struct.pack("<I", colva); q = 0
                while True:
                    q = d.find(pat3, q)
                    if q < 0: break
                    print("  COL %06X (offset %d) -> vftable %06X" % (colva, off, BASE + q + 4))
                    q += 4
            p += 4
        o = d.find(pat, o + 1)
