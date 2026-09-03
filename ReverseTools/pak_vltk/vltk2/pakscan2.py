# -*- coding: utf-8 -*-
"""Giai nen toan bo entry cua mot pak (UCL NRV2B) roi quet tu khoa 'thu/mail'.
Dung: python pakscan2.py <pak> <outdir> [--dump]
"""
import os, sys, re, struct
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump, ucl

pak = sys.argv[1]
outdir = sys.argv[2]
dump = "--dump" in sys.argv
os.makedirs(outdir, exist_ok=True)

f, es = pakdump.entries(pak)
print("pak", pak, "entries", len(es))

KW = [
    (b"mail", "ascii mail"),
    (b"Mail", "ascii Mail"),
    (b"MAIL", "ascii MAIL"),
    (b"\xd3\xca\xbc\xfe", "GBK 邮件"),
    (b"\xd3\xca\xcf\xe4", "GBK 邮箱"),
    (b"\xd0\xc5\xbc\xfe", "GBK 信件"),
    (b"\xbc\xc4\xd0\xc5", "GBK 寄信"),
    (b"\xb7\xa2\xd0\xc5", "GBK 发信"),
    (b"\xca\xd5\xd0\xc5", "GBK 收信"),
    (b"\xd3\xca\xd5\xfe", "GBK 邮政"),
    (b"\xd0\xc5\xcf\xe4", "GBK 信箱"),
    ("thư".encode("utf-8"), "utf8 thư"),
    ("Thư".encode("utf-8"), "utf8 Thư"),
    (b"Th\xdd", "cp1258 Thư?"),
    (b"th\xdd", "cp1258 thư?"),
    (b"Th\xad", "tcvn3 Thư?"),
    (b"th\xad", "tcvn3 thư?"),
]

def is_text(b):
    if not b:
        return False
    sample = b[:4000]
    bad = sum(1 for c in sample if c < 9 or (13 < c < 32))
    return bad * 50 < len(sample)

stats = {"raw": 0, "ucl": 0, "fail": 0, "text": 0}
hits = []
for e in es:
    uid, off, size, cf = e
    csize = cf & 0xFFFFFF
    flag = cf >> 24
    f.seek(off)
    blob = f.read(csize if csize else size)
    if flag == 0 or csize in (0, size):
        data = blob[:size]
        stats["raw"] += 1
    else:
        try:
            data = ucl.nrv2b_decompress_8(blob, size)
            stats["ucl"] += 1
        except Exception as ex:
            data = blob
            stats["fail"] += 1
    txt = is_text(data)
    if txt:
        stats["text"] += 1
    if dump:
        ext = ".txt" if txt else ".bin"
        open(os.path.join(outdir, "%08X%s" % (uid, ext)), "wb").write(data)
    found = []
    for k, label in KW:
        n = data.count(k)
        if n:
            found.append((label, n))
    if found:
        hits.append((uid, size, flag, txt, found))

print("stats", stats)
print("HITS", len(hits))
for uid, size, flag, txt, found in hits:
    print("uid=%08X size=%d flag=%d text=%s -> %s" % (uid, size, flag, txt, found))
