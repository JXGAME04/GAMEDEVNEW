# -*- coding: utf-8 -*-
r"""[ANDROID 11/09 WAUTO B0] Doc / ghi tep cau hinh WAuto  APdata\<ma nhan vat>.dat  (= struct autoData ghi nguyen, pack(1)).

Bo cuc struct doc THANG tu Sources/Core/Src/ipc_shared.h (khong chep tay), nen them truong o cuoi struct la tu theo.
Cung tep cho ca WAuto.exe ben PC lan bang WAuto trong game ban mobile (B0: JxWAutoNoiBo.cpp).

Dung:
  python android/wauto_dat.py sizeof                       in sizeof(autoData) (phai = 7644 neu chua them truong)
  python android/wauto_dat.py truong [loc]                 liet ke truong: offset, kieu, ten (loc = chuoi con cua ten)
  python android/wauto_dat.py xem  <tep.dat> [--het]       in cac truong khac 0 (--het: in het)
  python android/wauto_dat.py dat  <tep.dat> ten=gia_tri ...   dat truong roi ghi lai (tep chua co -> tao tu mac dinh
                                                           cua constructor). Mang: bTKGio[2]=1; chuoi: szChat=xin chao
  python android/wauto_dat.py so   <a.dat> <b.dat>         in cac truong khac nhau giua hai tep
"""
import io
import os
import re
import struct
import sys

GOC = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
IPC = os.path.join(GOC, "Sources", "Core", "Src", "ipc_shared.h")

KIEU = {"int": ("<i", 4), "UINT": ("<I", 4), "unsigned int": ("<I", 4), "short": ("<h", 2),
        "char": ("<b", 1), "__int64": ("<q", 8)}


def doc_struct():
    """-> (danh sach truong [(ten, kieu, dims, offset, co)], tong co, mac dinh {ten_or_ten[i]: gia tri})"""
    src = io.open(IPC, encoding="latin-1").read()
    m = re.search(r"struct autoData\s*\{(.*?)\n\tautoData\(\)\s*\{(.*?)\n\t\}", src, re.S)
    if not m:
        raise SystemExit("khong tim thay struct autoData / constructor trong " + IPC)
    than, ctor = m.group(1), m.group(2)
    truong = []
    off = 0
    for dong in than.splitlines():
        dong = dong.split("//")[0].strip()
        if not dong.endswith(";"):
            continue
        mm = re.match(r"^(unsigned int|__int64|UINT|int|short|char|autoCoord)\s+(\w+)((?:\[\d+\])*)\s*;$", dong)
        if not mm:
            continue
        kieu, ten, mang = mm.groups()
        dims = [int(x) for x in re.findall(r"\[(\d+)\]", mang or "")]
        if kieu == "autoCoord":
            co1 = 8
        else:
            co1 = KIEU[kieu][1]
        n = 1
        for d in dims:
            n *= d
        truong.append((ten, kieu, dims, off, co1 * n))
        off += co1 * n
    macdinh = {}
    for dong in ctor.splitlines():
        dong = dong.split("//")[0].strip()
        mm = re.match(r"^(\w+(?:\[\d+\])?)\s*=\s*(-?\d+)\s*;$", dong)
        if mm:
            macdinh[mm.group(1)] = int(mm.group(2))
    return truong, off, macdinh


def tim(truong, ten):
    for t in truong:
        if t[0] == ten:
            return t
    raise SystemExit("khong co truong: " + ten)


def lay(buf, t, chi_so=None):
    ten, kieu, dims, off, co = t
    if kieu == "char" and dims:
        # chuoi (1 chieu) hoac mang chuoi (2 chieu)
        if len(dims) == 1:
            s = buf[off:off + dims[0]].split(b"\0", 1)[0]
            return s.decode("latin-1")
        out = []
        for i in range(dims[0]):
            s = buf[off + i * dims[1]: off + (i + 1) * dims[1]].split(b"\0", 1)[0]
            out.append(s.decode("latin-1"))
        return out
    if kieu == "autoCoord":
        return [struct.unpack_from("<ii", buf, off + i * 8) for i in range(dims[0] if dims else 1)]
    fmt, co1 = KIEU[kieu]
    n = co // co1
    vals = [struct.unpack_from(fmt, buf, off + i * co1)[0] for i in range(n)]
    if chi_so is not None:
        return vals[chi_so]
    return vals[0] if not dims else vals


def dat(buf, t, gia_tri, chi_so=None):
    ten, kieu, dims, off, co = t
    if kieu == "char" and dims:
        if len(dims) != 1:
            raise SystemExit("mang chuoi %s: chua ho tro dat" % ten)
        b = gia_tri.encode("latin-1")[: dims[0] - 1] + b"\0"
        buf[off:off + len(b)] = b
        return
    fmt, co1 = KIEU[kieu]
    i = chi_so or 0
    if i * co1 >= co:
        raise SystemExit("chi so %d vuot mang %s" % (i, ten))
    struct.pack_into(fmt, buf, off + i * co1, int(gia_tri))


def tao_mac_dinh(truong, tong, macdinh):
    buf = bytearray(tong)
    for k, v in macdinh.items():
        mm = re.match(r"^(\w+)(?:\[(\d+)\])?$", k)
        t = tim(truong, mm.group(1))
        dat(buf, t, v, int(mm.group(2)) if mm.group(2) else None)
    return buf


def doc_tep(p, tong):
    d = bytearray(io.open(p, "rb").read())
    if len(d) < tong:
        print("  (tep %d byte ngan hon struct %d: phan duoi coi la 0)" % (len(d), tong))
        d += bytearray(tong - len(d))
    return d


def in_truong(truong, buf, het=False):
    for t in truong:
        v = lay(buf, t)
        if isinstance(v, list):
            if not het and not any(v if not isinstance(v[0], tuple) else [any(x) for x in v]):
                continue
            if isinstance(v[0], str):
                v = [x for x in v if x] if not het else v
            print("  %-16s = %s" % (t[0], v))
        else:
            if not het and (v == 0 or v == ""):
                continue
            print("  %-16s = %s" % (t[0], v))


def main(argv):
    truong, tong, macdinh = doc_struct()
    if not argv or argv[0] == "sizeof":
        print("sizeof(autoData) =", tong, "byte;", len(truong), "truong;", len(macdinh), "mac dinh trong constructor")
        return
    lenh = argv[0]
    if lenh == "truong":
        loc = argv[1].lower() if len(argv) > 1 else ""
        for ten, kieu, dims, off, co in truong:
            if loc in ten.lower():
                print("  %5d  %-13s %s%s" % (off, kieu, ten, "".join("[%d]" % d for d in dims)))
        return
    if lenh == "xem":
        buf = doc_tep(argv[1], tong)
        print("%s (%d byte):" % (argv[1], len(buf)))
        in_truong(truong, buf, "--het" in argv)
        return
    if lenh == "dat":
        p = argv[1]
        buf = doc_tep(p, tong) if os.path.isfile(p) else tao_mac_dinh(truong, tong, macdinh)
        for cap in argv[2:]:
            k, v = cap.split("=", 1)
            mm = re.match(r"^(\w+)(?:\[(\d+)\])?$", k)
            t = tim(truong, mm.group(1))
            dat(buf, t, v, int(mm.group(2)) if mm.group(2) else None)
            print("  dat %s = %s" % (k, v))
        d = os.path.dirname(p)
        if d and not os.path.isdir(d):
            os.makedirs(d)
        io.open(p, "wb").write(bytes(buf[:tong]))
        print("da ghi", p, tong, "byte")
        return
    if lenh == "so":
        a, b = doc_tep(argv[1], tong), doc_tep(argv[2], tong)
        for t in truong:
            va, vb = lay(a, t), lay(b, t)
            if va != vb:
                print("  %-16s %s  ->  %s" % (t[0], va, vb))
        return
    raise SystemExit(__doc__)


if __name__ == "__main__":
    main(sys.argv[1:])
