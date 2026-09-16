# -*- coding: utf-8 -*-
# Liet ke bang nhap (import) cua PE 32 bit, ke ca nhap theo SO THU TU (ordinal) - vi rainbow.dll the he moi
# nhap WS2_32 theo ordinal nen grep chuoi 'setsockopt' khong thay duoc.  Chi doc. ASCII only.
import struct, sys
WS2 = {1:"accept",2:"bind",3:"closesocket",4:"connect",5:"getpeername",6:"getsockname",7:"getsockopt",8:"htonl",
       9:"htons",10:"ioctlsocket",11:"inet_addr",12:"inet_ntoa",13:"listen",14:"ntohl",15:"ntohs",16:"recv",
       17:"recvfrom",18:"select",19:"send",20:"sendto",21:"setsockopt",22:"shutdown",23:"socket",
       51:"gethostbyaddr",52:"gethostbyname",57:"gethostname",101:"WSAAsyncSelect",111:"WSAGetLastError",
       112:"WSASetLastError",115:"WSAStartup",116:"WSACleanup"}
def imports(path):
    d = open(path, "rb").read()
    pe = struct.unpack_from("<I", d, 0x3c)[0]
    nsec = struct.unpack_from("<H", d, pe + 6)[0]
    opt = pe + 24
    magic = struct.unpack_from("<H", d, opt)[0]
    is64 = magic == 0x20b
    dd_off = opt + (112 if is64 else 96)
    imp_rva, imp_sz = struct.unpack_from("<II", d, dd_off + 8)
    secs = []
    so = opt + struct.unpack_from("<H", d, pe + 20)[0]
    for i in range(nsec):
        s = so + i * 40
        name = d[s:s+8].rstrip(b"\0").decode("latin-1")
        vsz, va, rsz, ro = struct.unpack_from("<IIII", d, s + 8)
        secs.append((va, vsz, ro, rsz, name))
    def rva2off(rva):
        for va, vsz, ro, rsz, nm in secs:
            if va <= rva < va + max(vsz, rsz): return ro + (rva - va)
        return None
    def cstr(off):
        e = d.index(b"\0", off); return d[off:e].decode("latin-1")
    out = {}
    o = rva2off(imp_rva)
    while True:
        ilt, ts, fc, name_rva, iat = struct.unpack_from("<IIIII", d, o)
        if name_rva == 0: break
        dll = cstr(rva2off(name_rva))
        thunk = rva2off(ilt or iat)
        names = []
        while True:
            if is64:
                v = struct.unpack_from("<Q", d, thunk)[0]; thunk += 8
                if v == 0: break
                if v >> 63: names.append("#%d" % (v & 0xffff))
                else: names.append(cstr(rva2off(v & 0x7fffffff) + 2))
            else:
                v = struct.unpack_from("<I", d, thunk)[0]; thunk += 4
                if v == 0: break
                if v >> 31: names.append("#%d" % (v & 0xffff))
                else: names.append(cstr(rva2off(v & 0x7fffffff) + 2))
        out[dll] = names
        o += 20
    return out, is64

for p in sys.argv[1:]:
    print("\n=== %s ===" % p)
    try:
        imps, is64 = imports(p)
    except Exception as e:
        print("   loi:", e); continue
    print("   PE%s" % ("32+" if is64 else "32"))
    for dll, names in imps.items():
        if dll.lower().startswith("ws2") or dll.lower().startswith("wsock"):
            pretty = []
            for n in names:
                if n.startswith("#"):
                    k = int(n[1:]); pretty.append("%s(#%d)" % (WS2.get(k, "?"), k))
                else: pretty.append(n)
            print("   %-12s %s" % (dll, ", ".join(pretty)))
        else:
            print("   %-12s %d ham: %s" % (dll, len(names), ", ".join(names[:14]) + (" ..." if len(names) > 14 else "")))
