# -*- coding: utf-8 -*-
"""dec_u8.py <in> <out> : giai ma tep client VLTK: tung dong thu UTF-8 nghiem ngat truoc, hong thi dung dec2 (VNI/TCVN+GBK)."""
import sys, io, importlib.util
spec = importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec); spec.loader.exec_module(dec2)
src, dst = sys.argv[1], sys.argv[2]
n8 = n2 = 0
out = []
for l in open(src, "rb").read().split(b"\n"):
    l = l.rstrip(b"\r")
    try:
        s = l.decode("utf-8"); n8 += 1
    except UnicodeDecodeError:
        s = dec2.decline2(l); n2 += 1
    out.append(s)
io.open(dst, "w", encoding="utf-8", newline="\n").write("\n".join(out))
print("%s -> %s : %d dong utf8, %d dong dec2" % (src, dst, n8, n2))
