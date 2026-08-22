# Quet ELF: tim moi lenh doc/ghi [base + disp] voi disp cho truoc trong vung code
# gan mot lenh tham chieu dia chi tuyet doi (vd Player base 0x8baee60).
# dung: python scan_offset.py <elf> <abs_addr_hex> <disp_hex> [window]
import sys, struct
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools")
from re_disasm import map_file
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

path, absaddr, disp = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
window = int(sys.argv[4]) if len(sys.argv) > 4 else 25
d, v2o, o2v, kind = map_file(path)
# .text cua jx_linux_y: PT_LOAD dau tien; quet tu 0x08048000 .. 0x0823F000 (ma)
START, END = 0x0804A000, 0x08240000
off = v2o(START)
md = Cs(CS_ARCH_X86, CS_MODE_32)
md.detail = True
ins_list = []
code = d[off:off + (END - START)]
pos = 0
while pos < len(code):
    got = False
    for ins in md.disasm(code[pos:pos + 4096], START + pos):
        ins_list.append(ins); pos += ins.size; got = True
        if pos >= len(code): break
    if not got:
        pos += 1
print("so lenh:", len(ins_list))
hits = []
for i, ins in enumerate(ins_list):
    s = ins.op_str
    if ("0x%x" % absaddr) in s:
        for j in range(i, min(i + window, len(ins_list))):
            k = ins_list[j]
            for op in k.operands:
                if op.type == 3 and op.mem.disp == disp:  # X86_OP_MEM
                    hits.append((ins.address, k.address, k.mnemonic + " " + k.op_str))
seen = set()
for a, b, t in hits:
    if b in seen: continue
    seen.add(b)
    print("ref 0x%08X -> 0x%08X  %s" % (a, b, t))
