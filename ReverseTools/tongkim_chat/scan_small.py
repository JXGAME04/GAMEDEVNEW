# -*- coding: utf-8 -*-
import sys, os, io, glob, time
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk"); sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk\vltk2")
from pakdump import entries
import nrv
KW = [b"Label_songPoint", b"Label_jinPoint", "淮河河畔".encode("gbk"), b"TipsWindow", "宋金快报".encode("gbk")]
MAXSZ = 40 * 1024
t0 = time.time()
for pak in sorted(glob.glob(r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky 2.0\data\*.pak")):
    base = os.path.basename(pak)
    try: f, es = entries(pak)
    except Exception as ex: print("bo qua", base, ex); continue
    n = 0
    for e in es:
        uid, off, size, cf = e
        if size <= 0 or size > MAXSZ: continue
        csize, flag = cf & 0xFFFFFF, cf >> 24
        f.seek(off); blob = f.read(csize if csize else size)
        try: data = blob if (flag == 0 or csize in (0, size)) else nrv.nrv2b_8(blob, size)
        except Exception: continue
        n += 1
        hits = [k.decode("gbk", errors="replace") for k in KW if k in data]
        if hits:
            fn = "hit_%s_%08X.bin" % (base.replace(".pak", ""), uid)
            io.open(fn, "wb").write(data)
            print("HIT", base, "%08X" % uid, size, hits, flush=True)
    f.close()
    print("xong", base, "quet", n, "entry nho", "%.0fs" % (time.time() - t0), flush=True)
print("XONG TAT CA")
