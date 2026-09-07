# -*- coding: utf-8 -*-
"""Scan the fixed gamecl image for byte patterns / immediates, report VA + enclosing func."""
import sys, re, struct
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
FIXED = r"C:\Users\nguye\AppData\Local\Temp\claude\J--CayChay-Src-Auto-Ngoai-WAuto-WAuto\17c6a10c-c4c9-46ff-96ed-31ae14294d55\scratchpad\gamecl_fixed.bin"
BASE = 0x401000
d = open(FIXED, "rb").read()

def is_funcstart(b, o):
    if o < 0 or o + 3 > len(b): return False
    if b[o:o+3] == b"\x55\x8b\xec": return True
    if b[o-1] == 0xCC and b[o] != 0xCC: return True
    if b[o-1] == 0xC3 and b[o] in (0x53,0x55,0x56,0x57,0x6A,0x81,0x83,0x8B,0xA1,0xB8,0xE8,0xE9,0x33,0x51,0x52): return True
    return False

def funcstart(va):
    o = va - BASE
    while o > 0 and not is_funcstart(d, o): o -= 1
    return BASE + o

mode = sys.argv[1]
if mode == "bytes":
    pat = bytes.fromhex(sys.argv[2])
    lo = int(sys.argv[3], 16) if len(sys.argv) > 3 else BASE
    hi = int(sys.argv[4], 16) if len(sys.argv) > 4 else BASE + len(d)
    o = 0; n = 0
    while True:
        o = d.find(pat, o)
        if o < 0: break
        va = BASE + o
        if lo <= va < hi:
            print("  %06X   in func ~ %06X" % (va, funcstart(va)))
            n += 1
        o += 1
    print("hits", n)
elif mode == "imm32":
    # find any occurrence of a 32-bit immediate value
    v = int(sys.argv[2], 16)
    pat = struct.pack("<I", v)
    o = 0; n = 0
    while True:
        o = d.find(pat, o)
        if o < 0: break
        va = BASE + o
        print("  %06X   in func ~ %06X" % (va, funcstart(va)))
        n += 1; o += 1
        if n > 60: print("  ..."); break
    print("hits", n)
