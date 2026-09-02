# -*- coding: utf-8 -*-
"""dec.py <file> [--grep kw] [--range a b] : giai ma tep Linux (VNI/TCVN + GBK tron) ra utf-8 va in ra."""
import sys, importlib.util, io
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
spec = importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec); spec.loader.exec_module(dec2)

def decode_file(p):
    return [dec2.decline2(l.rstrip(b"\r")) for l in open(p, "rb").read().split(b"\n")]

if __name__ == "__main__":
    p = sys.argv[1]
    args = sys.argv[2:]
    grep = None; a = 1; b = 10**9
    if "--grep" in args:
        grep = args[args.index("--grep") + 1].lower()
    if "--range" in args:
        i = args.index("--range"); a = int(args[i + 1]); b = int(args[i + 2])
    out = decode_file(p)
    print("# %s : %d dong" % (p, len(out)))
    for i, l in enumerate(out, 1):
        if i < a or i > b: continue
        if grep and grep not in l.lower(): continue
        print("%5d| %s" % (i, l))
