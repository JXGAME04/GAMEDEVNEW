# -*- coding: utf-8 -*-
"""dec_grep.py <root> <ext,ext> <kw1> [kw2...] : giai ma tung dong (dec2) roi tim tu khoa (khong phan biet hoa/thuong).
In: file:line| noi dung (cat 300 ky tu). Bien moi truong DG_MAX = so dong toi da moi tep (mac dinh 40)."""
import os, sys, io, importlib.util
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
spec = importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec); spec.loader.exec_module(dec2)
root, exts = sys.argv[1], tuple(sys.argv[2].lower().split(","))
kws = [k.lower() for k in sys.argv[3:]]
MAX = int(os.environ.get("DG_MAX", "40"))
tot = 0
for dp, ds, fs in os.walk(root):
    for f in fs:
        if not f.lower().endswith(exts): continue
        p = os.path.join(dp, f)
        try: raw = open(p, "rb").read()
        except: continue
        n = 0
        for i, l in enumerate(raw.split(b"\n"), 1):
            try: s = dec2.decline2(l.rstrip(b"\r"))
            except Exception: s = l.decode("latin-1")
            low = s.lower()
            if any(k in low for k in kws):
                n += 1; tot += 1
                if n <= MAX:
                    print("%s:%d| %s" % (os.path.relpath(p, root), i, s[:300]))
        if n > MAX:
            print("   ... %s: tong %d dong khop" % (os.path.relpath(p, root), n))
print("# tong khop:", tot)
