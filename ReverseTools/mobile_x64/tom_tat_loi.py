# -*- coding: utf-8 -*-
"""Tom tat loi MSBuild: python tom_tat_loi.py <log> [--per-file N] [--code Cxxxx]
In: so loi theo ma, theo tep, va toi da N dong moi tep (file(line): code: msg)."""
import io, re, sys, collections
log = sys.argv[1]
per = 8
only = None
a = sys.argv[2:]
i = 0
while i < len(a):
    if a[i] == "--per-file": per = int(a[i + 1]); i += 1
    elif a[i] == "--code": only = a[i + 1]; i += 1
    i += 1
rx = re.compile(r"^\s*(.+?)\((\d+)(?:,\d+)?\):\s+(?:fatal )?error\s+([A-Z]+\d+):\s+(.*?)(?:\s+\[[^\]]+\])?\s*$")
seen = set(); errs = []
for line in io.open(log, encoding="utf-8", errors="replace"):
    m = rx.match(line)
    if not m: continue
    f, ln, code, msg = m.group(1), int(m.group(2)), m.group(3), m.group(4)
    key = (f, ln, code)
    if key in seen: continue
    seen.add(key)
    if only and code != only: continue
    errs.append((f, ln, code, msg))
print("tong loi (khong trung):", len(errs))
bycode = collections.Counter(e[2] for e in errs)
print("theo ma:", dict(bycode.most_common(12)))
byfile = collections.OrderedDict()
for e in errs: byfile.setdefault(e[0], []).append(e)
print("theo tep:", len(byfile))
for f, lst in sorted(byfile.items(), key=lambda kv: -len(kv[1])):
    short = f.replace("D:\\GAMEDEVNEW_wt_mobile\\Sources\\", "")
    print(f"--- {short} ({len(lst)})")
    for e in sorted(lst, key=lambda x: x[1])[:per]:
        print(f"   {e[1]}: {e[2]}: {e[3][:150]}")
