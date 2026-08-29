# -*- coding: utf-8 -*-
r"""[BDH 28/08] Mo client VLTK (Level Up Games) tim bo UI "Ban Dong Hanh" ban PC
(anh chup cua chu game): quet NOI DUNG moi entry moi pak, tim chuoi nhan
TCVN3/UTF8; ghi hit ra p16_hits.txt (id entry + pak + ngu canh)."""
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
import pakdump as P  # noqa: E402
import ucl  # noqa: E402
from vn_edit import vn  # noqa: E402

OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "p16_hits.txt")


def blob_of(f, e):
    u, off, size, cf = e
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    if fl == 0x20:
        fl = 1
    fl &= 0x0F
    f.seek(off)
    raw = f.read(cs if cs else size)
    try:
        if fl == 0 or cs in (0, size):
            return raw[:size] if size > 0 else raw
        if fl == 1:
            return ucl.nrv2b_decompress_8(raw, size)
        import bz2
        import zlib
        return bz2.decompress(raw) if fl == 2 else zlib.decompress(raw)
    except Exception:
        return b""


PATS = {}
for lbl, t in [("hao-huu", "hảo hữu"), ("Dong-Hanh", "Đồng Hành"),
               ("Duc-lai", "Đúc lại"), ("Tu-Chan", "Tu Chân"),
               ("tang-truong", "tăng trưởng"), ("Than-phap", "Thân pháp")]:
    try:
        PATS["tcvn3:" + lbl] = vn(t).encode("latin-1")
    except Exception:
        pass
PATS["utf8:hao-huu"] = "hảo hữu".encode("utf-8")
PATS["utf8:Dong-Hanh"] = "Đồng Hành".encode("utf-8")

root = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky"
paks = []
for dp, ds, fs in os.walk(root):
    for fn in fs:
        if fn.lower().endswith((".pak", ".mps")):
            paks.append(os.path.join(dp, fn))
# uu tien update/1024 truoc
paks.sort(key=lambda p: (0 if ("update" in p.lower() or "1024" in p.lower() or
                               "slist" in p.lower()) else 1, p))

out = open(OUT, "w", encoding="utf-8")
tong = 0
for pk in paks:
    try:
        f, es = P.entries(pk)
    except Exception:
        continue
    n_e = 0
    for e in es:
        if e[2] <= 0 or e[2] > 400_000:
            continue
        data = blob_of(f, e)
        if len(data) < 8:
            continue
        for name, pat in PATS.items():
            if pat in data:
                i = data.find(pat)
                loai = data[:4]
                ctx = data[max(0, i-90):i+120]
                out.write("HIT [%s] %s id=%08x size=%d dau=%r\n  ctx=%r\n" %
                          (name, os.path.basename(pk), e[0], e[2], loai, ctx))
                out.flush()
                tong += 1
                n_e += 1
                break
    f.close()
    print("da quet", os.path.basename(pk), "- hit toi gio:", tong)
out.write("TONG HIT: %d\n" % tong)
out.close()
print("XONG p16, tong hit:", tong)
