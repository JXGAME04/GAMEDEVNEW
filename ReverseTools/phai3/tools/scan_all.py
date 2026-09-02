# -*- coding: utf-8 -*-
"""scan_all.py <outdir> [pak1,pak2,...] : quet MOI pak client VLTK, giai nen tung entry,
ghi index (pak, uid, size, kind) + gom moi duong dan tep xuat hien trong cac entry dang van ban
-> outdir/index_all.tsv, outdir/names_all.txt (ten -> uid -> pak chua)"""
import os, sys, io, re, time
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump as P
import ucl

ROOT = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"
RX = re.compile(rb'[\\/][\x21-\x7e\x80-\xff]{2,160}?\.(?:lua|ini|txt|spr|wav|mps|dat|bin|ani|scp|flt|cfg)\b', re.I)

def blob_of(f, e):
    u, off, size, cf = e
    cs = cf & 0xFFFFFF
    fl = cf >> 24
    f.seek(off)
    raw = f.read(cs if cs else size)
    if fl == 0:
        return raw
    try:
        return ucl.nrv2b_decompress_8(raw, size)
    except Exception:
        return raw

def kind(d):
    h = d[:4]
    if h.startswith(b"SPR"): return "spr"
    if h.startswith(b"RIFF"): return "wav"
    if h.startswith(b"MPS"): return "mps"
    if not d: return "empty"
    s = d[:2048]
    ok = sum(1 for b in s if 9 <= b <= 13 or 32 <= b < 127 or b >= 0x80)
    if ok >= len(s) * 0.97 and b"\x00" not in s:
        return "txt"
    return "bin"

def main():
    outdir = sys.argv[1]
    os.makedirs(outdir, exist_ok=True)
    paks = sys.argv[2].split(",") if len(sys.argv) > 2 else sorted(x for x in os.listdir(ROOT) if x.lower().endswith(".pak"))
    idx = io.open(os.path.join(outdir, "index_all.tsv"), "w", encoding="utf-8")
    idx.write("pak\tuid\tsize\tflag\tkind\n")
    names = {}
    uid2 = {}
    t0 = time.time()
    for pk in paks:
        p = os.path.join(ROOT, pk)
        try:
            f, es = P.entries(p)
        except Exception as ex:
            print("LOI", pk, ex, flush=True); continue
        ntxt = 0
        for e in es:
            d = blob_of(f, e)
            k = kind(d)
            idx.write("%s\t%08X\t%d\t%d\t%s\n" % (pk, e[0], e[2], e[3] >> 24, k))
            uid2.setdefault(e[0], []).append(pk)
            if k == "txt":
                ntxt += 1
                for m in RX.finditer(d):
                    s = m.group(0).replace(b"/", b"\\")
                    names.setdefault(s, set()).add(pk)
        idx.flush()
        print("== %-24s %5d entry, %4d txt, names so far %d, %.0fs" % (pk, len(es), ntxt, len(names), time.time() - t0), flush=True)
        f.close()
    idx.close()
    # ten -> uid -> pak thuc su chua uid do
    with io.open(os.path.join(outdir, "names_all.txt"), "w", encoding="utf-8") as g:
        g.write("uid\tname(latin1)\tpak_chua_uid\tpak_nhac_ten\n")
        for s in sorted(names):
            nm = s.decode("latin-1")
            u = P.name2id(nm)
            g.write("%08X\t%s\t%s\t%s\n" % (u, nm, ",".join(sorted(uid2.get(u, []))), ",".join(sorted(names[s]))))
    print("XONG names=%d uids=%d %.0fs" % (len(names), len(uid2), time.time() - t0), flush=True)

main()
