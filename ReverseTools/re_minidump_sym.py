# -*- coding: utf-8 -*-
r"""Ghep minidump + PDB: in call stack co TEN HAM, tach rieng ham UNG DUNG vs CRT."""
import struct, io, bisect, sys

PDB = r"D:\bin\multiserver\S3Relay.pdb"
DUMPS = [
 r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\multiserver\DumpInfo\s3relay-v1.0-20260824-091415-2756827572.dmp",
 r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\multiserver\DumpInfo\s3relay-v1.0-20260824-155246-278324196.dmp",
]

# ---------- PDB ----------
p = io.open(PDB, "rb").read()
blocksz, _f, _nb, numdirbytes, _u, blockmapaddr = struct.unpack_from("<IIIIII", p, 32)
ndir = (numdirbytes + blocksz - 1)//blocksz
dirblocks = struct.unpack_from("<%dI" % ndir, p, blockmapaddr*blocksz)
d = b"".join(p[x*blocksz:(x+1)*blocksz] for x in dirblocks)[:numdirbytes]
nstreams = struct.unpack_from("<I", d, 0)[0]
sizes = list(struct.unpack_from("<%dI" % nstreams, d, 4))
pos = 4 + nstreams*4
blocks = []
for sz in sizes:
    nb = 0 if sz == 0xFFFFFFFF else (sz + blocksz - 1)//blocksz
    blocks.append(struct.unpack_from("<%dI" % nb, d, pos)); pos += nb*4
def stream(i):
    if i >= len(sizes) or sizes[i] == 0xFFFFFFFF: return b""
    return b"".join(p[x*blocksz:(x+1)*blocksz] for x in blocks[i])[:sizes[i]]

dbi = stream(3)
(sig, ver, age, gsSym, bvers, psSym, pdbver, symRecStream, rbld,
 modSize, secConSize, secMapSize, srcInfoSize, tsMapSize, mfcIdx,
 dbgHdrSize, ecSize, flags, machine, resv) = struct.unpack_from("<iIIHHHHHHiiiiiiiiHHI", dbi, 0)
dbghdr_off = 64 + modSize + secConSize + secMapSize + srcInfoSize + tsMapSize + ecSize
sechdr_stream = struct.unpack_from("<11h", dbi, dbghdr_off)[5]
sh = stream(sechdr_stream)
sect_va = [struct.unpack_from("<II", sh, i*40 + 8)[1] for i in range(len(sh)//40)]

pubs = []
sym = stream(symRecStream); off = 0; n = len(sym)
while off + 4 <= n:
    reclen, rectyp = struct.unpack_from("<HH", sym, off)
    if reclen < 2: break
    if rectyp == 0x110E and off + 14 <= n:
        _fl, soff, seg = struct.unpack_from("<IIH", sym, off + 4)
        name = sym[off+14: off+2+reclen].split(b"\0")[0].decode("latin-1", "replace")
        if 1 <= seg <= len(sect_va): pubs.append((sect_va[seg-1] + soff, name))
    off += reclen + 2
pubs.sort()
keys = [x[0] for x in pubs]

CRT_PREF = ("__", "_", "?type_case", "?state_case", "?write_string", "?process_state",
            "??$__Internal", "?FrameUnwind", "?_UnwindNested", "??_", "?_Xlen", "?_Xran")
def resolve(rva):
    i = bisect.bisect_right(keys, rva) - 1
    if i < 0: return ("???", 0)
    return (pubs[i][1], rva - pubs[i][0])
def is_app(nm):
    if nm.startswith("?") and ("@@" in nm):
        # ham C++; loai cac ham CRT/std quen thuoc
        if "@std@@" in nm and "CFriend" not in nm and "CTong" not in nm: return False
        if nm.startswith(CRT_PREF[3:]): return False
        return True
    if nm.startswith("__") or nm.startswith("_"): return False
    return True

# ---------- DUMP ----------
for DP in DUMPS:
    b = io.open(DP, "rb").read()
    nstr = struct.unpack_from("<I", b, 8)[0]; diroff = struct.unpack_from("<I", b, 12)[0]
    streams = {}
    for i in range(nstr):
        st, sz, rva = struct.unpack_from("<III", b, diroff + i*12); streams[st] = (sz, rva)
    mods = []
    sz, rva = streams[4]; nm_ = struct.unpack_from("<I", b, rva)[0]; off = rva + 4
    for i in range(nm_):
        base, msize = struct.unpack_from("<QI", b, off)
        namerva = struct.unpack_from("<I", b, off + 20)[0]
        nlen = struct.unpack_from("<I", b, namerva)[0]
        nmm = b[namerva+4:namerva+4+nlen].decode("utf-16-le", "replace")
        mods.append((base, msize, nmm)); off += 108
    exe_base = [m for m in mods if m[2].lower().endswith("s3relay.exe")][0][0]
    def mod_of(a):
        for base, msize, nmm in mods:
            if base <= a < base + msize: return nmm.split("\\")[-1], a - base
        return None, 0

    sz, rva = streams[6]
    tid = struct.unpack_from("<I", b, rva)[0]
    code, fl, rec, addr = struct.unpack_from("<IIQQ", b, rva + 8)
    params = struct.unpack_from("<8Q", b, rva + 8 + 32)
    ctx_sz, ctx_rva = struct.unpack_from("<II", b, rva + 8 + 152)
    eip = struct.unpack_from("<I", b, ctx_rva + 0xB8)[0]
    esp = struct.unpack_from("<I", b, ctx_rva + 0xC4)[0]
    mems = []
    sz, rva = streams[5]; nmem = struct.unpack_from("<I", b, rva)[0]; off = rva + 4
    for i in range(nmem):
        start = struct.unpack_from("<Q", b, off)[0]
        dsz, drva = struct.unpack_from("<II", b, off + 8)
        mems.append((start, dsz, drva)); off += 16
    def rd(a, s):
        for st, ds, dr in mems:
            if st <= a and a + s <= st + ds: return b[dr + (a-st): dr + (a-st) + s]
        return None

    print("\n" + "="*78)
    print("DUMP: %s" % DP.split("\\")[-1])
    m, o = mod_of(addr)
    nm, delta = resolve(o)
    print("Exception 0x%08X doc 0x%X | thread %d" % (code, params[1], tid))
    print("CRASH tai: %s +0x%X   [%s +0x%X]" % (nm, delta, m, o))
    print("--- khung UNG DUNG tren stack (bo CRT) ---")
    cnt = 0
    for spos in range(esp, esp + 0x4000, 4):
        dd = rd(spos, 4)
        if not dd: break
        val = struct.unpack("<I", dd)[0]
        mm, oo = mod_of(val)
        if mm and mm.lower() == "s3relay.exe":
            nm2, dl2 = resolve(oo)
            if is_app(nm2):
                print("  [esp+0x%04X] %s +0x%X" % (spos-esp, nm2, dl2))
                cnt += 1
                if cnt >= 22: break
