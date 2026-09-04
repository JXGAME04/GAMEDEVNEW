# doc_dump.py <file.dmp> - doc MINIDUMP: luong ngoai le (ma, dia chi, thread), module chua dia chi, RIP/RSP cua thread loi,
# va doc 64 khung stack tho (quet RSP tim dia chi tro vao module) - khong can cdb. Chi doc vai KB + vung stack, khong doc 12 GB.
import struct, sys, os
sys.stdout.reconfigure(encoding='utf-8', errors='replace')
p = sys.argv[1]
f = open(p, 'rb')
def rd(off, n):
    f.seek(off); return f.read(n)
sig, ver, nstreams, stream_rva, cs, ts, flags = struct.unpack('<4sIIIIIQ', rd(0, 32))
print('MDMP:', sig, 'streams:', nstreams, 'flags: 0x%x' % flags, 'thoi diem:', ts)
import datetime
print('thoi diem dump (UTC):', datetime.datetime.utcfromtimestamp(ts))
streams = {}
for i in range(nstreams):
    t, sz, rva = struct.unpack('<III', rd(stream_rva + i * 12, 12))
    streams.setdefault(t, []).append((sz, rva))
# 4 = ModuleList, 3 = ThreadList, 6 = Exception, 9 = Memory64List, 7 = SystemInfo
def read_str(rva):
    n = struct.unpack('<I', rd(rva, 4))[0]
    return rd(rva + 4, n).decode('utf-16-le', 'replace')
mods = []
if 4 in streams:
    sz, rva = streams[4][0]
    n = struct.unpack('<I', rd(rva, 4))[0]
    for i in range(n):
        b = rd(rva + 4 + i * 108, 108)
        base, size, cksum, tstamp, name_rva = struct.unpack('<QIIII', b[:24])
        mods.append((base, size, read_str(name_rva)))
print('modules:', len(mods))
def mod_of(addr):
    for base, size, name in mods:
        if base <= addr < base + size:
            return name, addr - base
    return None, 0
# Memory64List de doc vung stack
mem = []
if 9 in streams:
    sz, rva = streams[9][0]
    n, base_rva = struct.unpack('<QQ', rd(rva, 16))
    off = base_rva
    for i in range(n):
        start, size = struct.unpack('<QQ', rd(rva + 16 + i * 16, 16))
        mem.append((start, size, off)); off += size
def read_mem(addr, n):
    for start, size, off in mem:
        if start <= addr < start + size:
            k = min(n, start + size - addr)
            return rd(off + (addr - start), k)
    return b''
if 6 in streams:
    sz, rva = streams[6][0]
    b = rd(rva, sz)
    tid, _, code, eflags, record, addr = struct.unpack('<IIIIQQ', b[:32])
    nparams = struct.unpack('<I', b[32:36])[0]
    params = struct.unpack('<15Q', b[40:160])
    ctx_sz, ctx_rva = struct.unpack('<II', b[160:168])
    name, off = mod_of(addr)
    print('EXCEPTION: thread %d  code 0x%08X  tai 0x%X  = %s + 0x%X' % (tid, code, addr, name, off))
    if code == 0xC0000005:
        print('   ACCESS_VIOLATION: %s dia chi 0x%X' % ('GHI' if params[0] == 1 else 'DOC' if params[0] == 0 else 'DEP?', params[1]))
    ctx = rd(ctx_rva, ctx_sz)
    # CONTEXT x64: Rip o offset 0xF8, Rsp 0x98, Rax 0x78, Rcx 0x80, Rdx 0x88, Rbx 0x90, Rbp 0xA0, Rsi 0xA8, Rdi 0xB0, R8 0xB8.. R15 0xF0
    if len(ctx) >= 0x100:
        regs = {}
        for nm, o in [('rax', 0x78), ('rcx', 0x80), ('rdx', 0x88), ('rbx', 0x90), ('rsp', 0x98), ('rbp', 0xA0), ('rsi', 0xA8), ('rdi', 0xB0),
                      ('r8', 0xB8), ('r9', 0xC0), ('r10', 0xC8), ('r11', 0xD0), ('r12', 0xD8), ('r13', 0xE0), ('r14', 0xE8), ('r15', 0xF0), ('rip', 0xF8)]:
            regs[nm] = struct.unpack('<Q', ctx[o:o + 8])[0]
        print('   ' + '  '.join('%s=%X' % (k, v) for k, v in regs.items()))
        rip_name, rip_off = mod_of(regs['rip'])
        print('   RIP = %s + 0x%X' % (rip_name, rip_off))
        # quet stack: 4096 slot tu RSP
        rsp = regs['rsp']
        blob = read_mem(rsp, 8 * 4096)
        print('   stack (dia chi tro vao module, %d slot doc duoc):' % (len(blob) // 8))
        shown = 0
        for i in range(0, len(blob) - 7, 8):
            v = struct.unpack('<Q', blob[i:i + 8])[0]
            nm, o = mod_of(v)
            if nm and (nm.lower().endswith('coreserver.dll') or nm.lower().endswith('gameserver.exe') or 'lua' in nm.lower() or 'mysql' in nm.lower()):
                print('      [rsp+%05X] %s + 0x%X' % (i, os.path.basename(nm), o)); shown += 1
                if shown >= 40: break
# module chinh
for base, size, name in mods:
    bn = os.path.basename(name).lower()
    if bn in ('coreserver.dll', 'gameserver.exe', 'libmysql.dll', 'lualib.dll', 'engine.dll'):
        print('   mod %-16s base 0x%X size 0x%X' % (bn, base, size))
