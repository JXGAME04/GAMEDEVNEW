# -*- coding: utf-8 -*-
# Quet moi ham trong .so, in ra dong co chuoi khop mau
import sys, re
sys.path.insert(0, ".")
from dis3 import Lib

lib = Lib(sys.argv[1])
pat = re.compile(sys.argv[2], re.I) if len(sys.argv) > 2 else None
for nm in sorted(lib.by_name):
    try:
        rows = lib.full(nm, 3000)
    except Exception:
        continue
    for a, t, ann in rows:
        if ann and (pat is None or pat.search(ann)):
            print("%-70s %06x %-40s%s" % (nm[:70], a, t, ann))
