# -*- coding: utf-8 -*-
r"""Kiem DynamicExecute / DynamicExecuteByPlayer / RemoteExc / ReLoadScript:
tep dich ton tai + ham dich co dinh nghia. Co PHAN GIAI BIEN chua duong dan.
"""
import io, re, os

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
ROOTS = [
    r"script\missions\tongwar", r"script\event\tongwar", r"script\missions\bairenleitai",
    r"script\missions\bw", r"script\missions\tongcastle", r"script\mission\tongcastle",
    r"script\item\hoatdong_admin.lua", r"script\header\cauhinh_hoatdong.lua",
    r"script\startgame.lua", r"script\lib\awardtype", r"script\lib\remoteexc.lua",
]

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()

files = []
for r in ROOTS:
    p = os.path.join(E, r)
    if os.path.isfile(p): files.append(p)
    elif os.path.isdir(p):
        for dp, _, fs in os.walk(p):
            for f in fs:
                if f.lower().endswith(".lua"): files.append(os.path.join(dp, f))

_defcache = {}
def defs_of(path):
    if path in _defcache: return _defcache[path]
    if not os.path.isfile(path):
        _defcache[path] = None; return None
    s = rd(path)
    d = set()
    for m in re.finditer(r"function\s+([\w:.]+)\s*\(", s): d.add(m.group(1))
    for m in re.finditer(r"([\w.:]+)\s*=\s*function\s*\(", s): d.add(m.group(1))
    for m in re.finditer(r'Include\("([^"]+)"\)', s):
        q = os.path.join(E, m.group(1).replace("\\\\", "\\").lstrip("\\"))
        if os.path.isfile(q):
            for m2 in re.finditer(r"function\s+([\w:.]+)\s*\(", rd(q)): d.add(m2.group(1))
            for m2 in re.finditer(r"([\w.:]+)\s*=\s*function\s*\(", rd(q)): d.add(m2.group(1))
    _defcache[path] = d
    return d

PAT = re.compile(r'(DynamicExecuteByPlayer|DynamicExecute|RemoteExc|ReLoadScript)\s*\(([^;\n]*)')
bad, checked, skipped = [], 0, 0

for f in files:
    s = rd(f)
    # bang bien duong dan khai bao trong chinh tep: VAR = "\\script\\..."
    varmap = {}
    for m in re.finditer(r'^\s*(\w+)\s*=\s*"((?:\\\\|[^"])*?\.lua)"', s, re.M):
        varmap[m.group(1)] = m.group(2)
    for i, line in enumerate(s.split("\n"), 1):
        for m in PAT.finditer(line):
            fn, args = m.group(1), m.group(2)
            parts = [x.strip() for x in args.split(",")]
            if fn == "DynamicExecuteByPlayer" and parts: parts = parts[1:]
            if not parts: continue
            p0 = parts[0]
            path_raw = None
            if p0.startswith('"'):
                q = re.match(r'"((?:\\\\|[^"])*)"', p0)
                if q: path_raw = q.group(1)
            elif re.match(r"^%?\w+$", p0):
                path_raw = varmap.get(p0.lstrip("%"))
            if not path_raw or ".lua" not in path_raw:
                skipped += 1; continue
            target = os.path.join(E, path_raw.replace("\\\\", "\\").lstrip("\\"))
            checked += 1
            if not os.path.isfile(target):
                bad.append((f, i, "TEP KHONG TON TAI", path_raw, "")); continue
            if fn == "ReLoadScript": continue
            func = None
            if len(parts) > 1:
                q = re.match(r'"([^"]*)"', parts[1])
                if q: func = q.group(1)
            if not func: continue
            d = defs_of(target) or set()
            base = func.split(":")[-1].split(".")[-1]
            if not (func in d or any(x.split(":")[-1].split(".")[-1] == base for x in d)):
                bad.append((f, i, "HAM KHONG CO TRONG TEP DICH", path_raw, func))

print("Da kiem %d loi goi co duong dan ro rang (%d bo qua vi duong dan dong), %d tep\n" % (checked, skipped, len(files)))
if not bad:
    print("KET QUA: 0 loi")
for f, i, why, p, fn in bad:
    print("  %-26s:%-4d %-28s %s  %s" % (os.path.basename(f), i, why, p, fn))
