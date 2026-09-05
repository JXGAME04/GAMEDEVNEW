# -*- coding: utf-8 -*-
import re, sys, struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
FIX = "C:/Users/nguye/AppData/Local/Temp/claude/J--CayChay-Src-Auto-Ngoai-WAuto-WAuto/17c6a10c-c4c9-46ff-96ed-31ae14294d55/scratchpad/gamecl_fixed.bin"
IMG = "C:/Users/nguye/AppData/Local/Temp/claude/J--CayChay-Src-Auto-Ngoai-WAuto-WAuto/3949e8f5-ab2f-4400-8e30-5234b8579d31/scratchpad/cl/gamecl.img"
d = open(FIX, "rb").read(); img = open(IMG, "rb").read()
BASE = 0x401000
md = Cs(CS_ARCH_X86, CS_MODE_32); md.detail = True
def strat(a):
    o = a - BASE
    if o < 0 or o >= len(img): return None
    m = re.match(rb"(?:[\x20-\x7e]|[\x81-\xfe][\x40-\xfe]){3,70}", img[o:o+80])
    if not m: return None
    try: return m.group(0).decode("gbk")
    except: return m.group(0).decode("latin-1")
def note(ins):
    out = []
    for op in ins.operands:
        v = None
        if op.type == 2: v = op.imm
        elif op.type == 3 and op.mem.base == 0 and op.mem.index == 0: v = op.mem.disp
        if v is None: continue
        if BASE <= v < BASE + len(img):
            s = strat(v)
            if s and not (ins.mnemonic in ("call", "jmp")): out.append('"%s"' % s)
            elif ins.mnemonic in ("call", "jmp"): out.append("-> 0x%X" % v)
    return "  ; " + " ".join(out) if out else ""
def dis(addr, n=120, stop_ret=True):
    code = d[addr - BASE: addr - BASE + n * 10]
    cnt = 0; calls = []
    for ins in md.disasm(code, addr):
        print("%08X  %-6s %-36s%s" % (ins.address, ins.mnemonic, ins.op_str, note(ins)))
        if ins.mnemonic == "call" and ins.operands and ins.operands[0].type == 2: calls.append(ins.operands[0].imm)
        cnt += 1
        if cnt >= n: break
        if stop_ret and ins.mnemonic == "ret" and cnt > 6: break
    return calls
if __name__ == "__main__":
    a = int(sys.argv[1], 16); n = int(sys.argv[2]) if len(sys.argv) > 2 else 120
    dis(a, n)
