# -*- coding: utf-8 -*-
import collections
import io
import os
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
p = os.path.join(os.path.dirname(os.path.abspath(__file__)), "tep_thua.txt")
t = io.open(p, encoding="utf-8").read()
i = t.find("## 3)")
j = t.find("## 4)")
d = [l.strip() for l in t[i:j].split("\n") if l.strip().lower().startswith("script")]
print("so tep:", len(d))
print("--- 12 mau ---")
for x in d[:12]:
    print("  ", x)
c = collections.Counter(x.split(os.sep)[1] if os.sep in x else "?" for x in d)
print("--- theo thu muc cap 1 ---")
for k, v in c.most_common(14):
    print("  %-26s %d" % (k, v))
