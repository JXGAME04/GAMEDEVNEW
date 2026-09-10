# -*- coding: utf-8 -*-
r"""[ANDROID 12/09 GOI] Ghi pak JX1 ('PACK') dung dac ta XPackFile.cpp:
  header 32 byte: 'PACK', count, indexOffset, dataOffset=32, crc=0, 12 byte 0
  du lieu tu byte 32, xep theo uid TANG; bang chi muc 16 byte/muc (uid, offset, size, flag) sap uid tang, o CUOI tep.
  flag = (kieu nen << 24) | co nen; o day ghi THO (kieu 0, co nen = size) -> XPackFile doc thang, khong can UCL.
Bay: header 16 byte bi XPackFile::Open bo qua im lang (memory pak-jx1-layout-header16-trap).
Id ten tep: FileNameToId (pakdump.name2id) tren duong dan kieu Windows '\spr\...' (A-Z ha thuong, byte > 127 tinh am).
"""
import os
import struct
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "ReverseTools", "pak_vltk"))
from pakdump import name2id  # noqa: E402

HEADER = 32


def ghi_pak(duong_dan, cac_muc, tien_trinh=None):
    """cac_muc: iterable (uid, bytes | duong dan tep tho). Ghi tuan tu theo uid tang; tra (so muc, tong byte)."""
    muc = sorted(cac_muc, key=lambda m: m[0])
    # kiem trung uid
    for a, b in zip(muc, muc[1:]):
        if a[0] == b[0]:
            raise SystemExit("trung uid %08x" % a[0])
    index = []
    with open(duong_dan, "wb") as f:
        f.write(b"\0" * HEADER)
        off = HEADER
        for i, (uid, du_lieu) in enumerate(muc):
            if isinstance(du_lieu, str):
                with open(du_lieu, "rb") as g:
                    du_lieu = g.read()
            f.write(du_lieu)
            index.append((uid, off, len(du_lieu), len(du_lieu)))   # flag: kieu 0 (tho) | co nen = size
            off += len(du_lieu)
            if tien_trinh and i % 2000 == 0:
                tien_trinh(i, len(muc), off)
        ioff = off
        for uid, o, size, flag in index:
            f.write(struct.pack("<IIiI", uid, o, size, flag))
        f.seek(0)
        f.write(b"PACK" + struct.pack("<IIII", len(index), ioff, HEADER, 0) + b"\0" * 12)
    return len(index), ioff


def kiem_pak(duong_dan, so_thu=3):
    """doc lai header + chi muc, tra (count, ioff) va thu doc vai muc"""
    with open(duong_dan, "rb") as f:
        d = f.read(HEADER)
        sig, count, ioff, doff, crc = struct.unpack("<4sIIII", d[:20])
        assert sig == b"PACK" and doff == HEADER, (sig, doff)
        f.seek(ioff)
        raw = f.read(count * 16)
        uids = []
        for i in range(min(count, so_thu)):
            uid, off, size, flag = struct.unpack("<IIiI", raw[i * 16:i * 16 + 16])
            f.seek(off); b = f.read(min(size, 16))
            uids.append((uid, size, b[:4]))
        truoc = -1
        for i in range(count):
            uid = struct.unpack("<I", raw[i * 16:i * 16 + 4])[0]
            assert uid > truoc, "chi muc khong tang o %d" % i
            truoc = uid
    return count, ioff, uids


if __name__ == "__main__":
    # tu kiem: ghi 3 muc roi doc lai
    p = os.path.join(os.environ.get("TEMP", "."), "thu_pak_ghi.pak")
    n, ioff = ghi_pak(p, [(name2id("\\thu\\a.txt"), b"AAAA"), (name2id("\\thu\\b.txt"), b"BB"), (name2id("\\thu\\c.txt"), b"C" * 100)])
    print("ghi", n, "muc, index @", ioff, "->", kiem_pak(p))
