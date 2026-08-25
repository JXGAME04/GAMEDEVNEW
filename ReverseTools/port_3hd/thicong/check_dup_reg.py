# -*- coding: utf-8 -*-
"""Quet TRUNG TEN trong bang dang ky GameScriptFuns[] cua ScriptFuns.cpp.
Lua_State::RegisterFunctions dang ky tuan tu -> ten dang ky SAU se DE LEN ten truoc
(hoac nguoc lai tuy cai dat). Trung ten = mot trong hai ban KHONG BAO GIO duoc goi.
Bao rieng nhung ten do dot [3HD 25/08] them vao.
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

p = r"D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp"
d = io.open(p, encoding="latin-1", newline="").read()
lines = d.split("\n")

start = next(i for i, l in enumerate(lines) if "TLua_Funcs GameScriptFuns[]" in l)
end = next(i for i in range(start, len(lines)) if lines[i].strip() == "};")

rx = re.compile(r'\{\s*"([A-Za-z_][A-Za-z0-9_]*)"\s*,\s*([A-Za-z_][A-Za-z0-9_]*)')
seen = {}
hd3_lines = set()
in_hd3 = False
for i in range(start, end + 1):
    l = lines[i]
    if "[3HD 25/08]" in l:
        in_hd3 = True
    # khoi 3HD ket thuc khi gap comment khac hoac dong trong sau day
    m = rx.search(l)
    if not m:
        continue
    if l.strip().startswith("//"):
        continue
    name, fn = m.group(1), m.group(2)
    seen.setdefault(name, []).append((i + 1, fn, in_hd3))
    if in_hd3:
        hd3_lines.add(i + 1)

dups = {k: v for k, v in seen.items() if len(v) > 1}
print("Tong ten dang ky:", len(seen))
print("Ten dang ky nhieu lan:", len(dups))
print()
hd3_dup = []
for k, v in sorted(dups.items()):
    involves_hd3 = any(x[2] for x in v)
    tag = "  <== DOT 3HD" if involves_hd3 else ""
    print("  %-28s %s%s" % (k, " | ".join("dong %d -> %s" % (a, b) for a, b, _ in v), tag))
    if involves_hd3:
        hd3_dup.append(k)
print()
print("TRUNG LIEN QUAN DOT 3HD:", len(hd3_dup), hd3_dup)
