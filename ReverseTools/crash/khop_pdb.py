# -*- coding: utf-8 -*-
"""Khop PDB voi DLL dang chay / voi module ghi trong dump, bang GUID+age (RSDS), khong doan theo mtime."""
import os, struct, sys, glob
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

def guid_str(b):
    d1, d2, d3 = struct.unpack_from("<IHH", b, 0)
    rest = b[8:16]
    return "%08X-%04X-%04X-%s-%s" % (d1, d2, d3, rest[:2].hex().upper(), rest[2:].hex().upper())

def pe_rsds(path):
    """Doc debug directory cua PE -> (guid, age, pdbpath)"""
    f = open(path, "rb")
    mz = f.read(0x40)
    e_lfanew = struct.unpack_from("<I", mz, 0x3C)[0]
    f.seek(e_lfanew)
    sig = f.read(4)
    coff = f.read(20)
    nsec, = struct.unpack_from("<H", coff, 2)
    optsz, = struct.unpack_from("<H", coff, 16)
    opt = f.read(optsz)
    magic, = struct.unpack_from("<H", opt, 0)
    ddoff = 112 if magic == 0x20B else 96          # PE32+ vs PE32
    dbg_rva, dbg_sz = struct.unpack_from("<II", opt, ddoff + 6 * 8)
    secs = []
    for i in range(nsec):
        s = f.read(40)
        va, = struct.unpack_from("<I", s, 12)
        raw, praw = struct.unpack_from("<II", s, 16)
        secs.append((va, raw, praw))
    def rva2off(rva):
        for va, raw, praw in secs:
            if va <= rva < va + max(raw, 1) + 0x1000:
                return praw + (rva - va)
        return None
    off = rva2off(dbg_rva)
    if off is None:
        return None
    for i in range(dbg_sz // 28):
        f.seek(off + i * 28)
        e = f.read(28)
        typ, = struct.unpack_from("<I", e, 12)
        szdata, rvadata, ptrdata = struct.unpack_from("<III", e, 16)
        if typ != 2:      # IMAGE_DEBUG_TYPE_CODEVIEW
            continue
        f.seek(ptrdata)
        cv = f.read(szdata)
        if cv[:4] != b"RSDS":
            continue
        return guid_str(cv[4:20]), struct.unpack_from("<I", cv, 20)[0], cv[24:].split(b"\0")[0].decode("latin-1")
    return None

def pdb_guid(path):
    """PDB 7.0 (MSF): doc stream 1 -> guid + age"""
    f = open(path, "rb")
    hdr = f.read(52)
    if not hdr.startswith(b"Microsoft C/C++ MSF 7.00"):
        return None
    page, = struct.unpack_from("<I", hdr, 32)
    numpages, = struct.unpack_from("<I", hdr, 40)
    dirsz, = struct.unpack_from("<I", hdr, 44)
    dirmap, = struct.unpack_from("<I", hdr, 52 - 4)
    # trang chua danh sach trang cua stream directory
    f.seek(dirmap * page)
    npages_dir = (dirsz + page - 1) // page
    ptrs = struct.unpack("<%dI" % npages_dir, f.read(4 * npages_dir))
    data = b""
    for p in ptrs:
        f.seek(p * page); data += f.read(page)
    data = data[:dirsz]
    nstream, = struct.unpack_from("<I", data, 0)
    sizes = struct.unpack_from("<%dI" % nstream, data, 4)
    pos = 4 + 4 * nstream
    streams = []
    for s in sizes:
        n = 0 if s in (0, 0xFFFFFFFF) else (s + page - 1) // page
        streams.append(struct.unpack_from("<%dI" % n, data, pos) if n else ())
        pos += 4 * n
    if len(streams) < 2:
        return None
    buf = b""
    for p in streams[1]:
        f.seek(p * page); buf += f.read(page)
    ver, sig, age = struct.unpack_from("<III", buf, 0)
    return guid_str(buf[12:28]), age

LIVE = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\CoreServer.dll"
r = pe_rsds(LIVE)
print("DLL DANG CHAY:", LIVE)
if r:
    print("   GUID %s  age %d" % (r[0], r[1]))
    print("   pdb ghi trong DLL: %s" % r[2])
target = r[0] if r else None

print("\nCac PDB ung vien:")
found = []
for d in ["D:\\GAMEDEVNEW"] + sorted(glob.glob("D:\\GAMEDEVNEW_wt_*")):
    p = os.path.join(d, "Sources", "Core", "x64", "Server Release", "CoreServer.pdb")
    if not os.path.isfile(p):
        continue
    try:
        g = pdb_guid(p)
    except Exception as ex:
        print("   %-34s LOI doc: %s" % (os.path.basename(d), ex)); continue
    if not g:
        print("   %-34s khong doc duoc GUID" % os.path.basename(d)); continue
    ok = (g[0] == target)
    if ok:
        found.append(p)
    print("   %-34s %s age %-3d %s" % (os.path.basename(d), g[0], g[1], "<<< KHOP DLL DANG CHAY" if ok else ""))
print("\nKET LUAN:", ("KHOP: " + found[0]) if found else "*** KHONG PDB NAO KHOP - phai build lai dung commit ***")
