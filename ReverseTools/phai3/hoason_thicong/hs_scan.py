# -*- coding: utf-8 -*-
"""hs_scan.py : quet 79 tep Linux script\global\huashan2013 (+ trap) -> moi tep: Include, item id (6,1,P), task id, NPC template, ham goi."""
import os, re, io, sys, importlib.util, collections
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
spec = importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec); spec.loader.exec_module(dec2)
ROOT = r"D:\ServerLinux\server1\script\global\huashan2013"
BS = chr(92)
rx_inc = re.compile(r'Include(?:Lib)?\(\s*"([^"]+)"')
rx_item = re.compile(r'\(\s*6\s*,\s*1\s*,\s*(\d+)')
rx_task = re.compile(r'(?:Set|Get|nt_set|nt_get)Task\(\s*(\d+)')
rx_call = re.compile(r'\b([A-Za-z_][A-Za-z0-9_]*)\s*\(')
KW = {"if","for","while","function","return","and","or","not","local","then","else","elseif","end","do","repeat","until","nil"}
items = collections.Counter(); tasks = collections.Counter(); incs = collections.Counter(); calls = collections.Counter()
per = {}
for dp, ds, fs in os.walk(ROOT):
    for f in sorted(fs):
        if not f.endswith(".lua"): continue
        p = os.path.join(dp, f)
        lines = [dec2.decline2(l.rstrip(b"\r")) for l in open(p, "rb").read().split(b"\n")]
        live = [l for l in lines if l.strip() and not l.strip().startswith("--")]
        code = "\n".join(l.split("--")[0] for l in live)
        fi = sorted(set(rx_inc.findall(code))); it = sorted(set(int(x) for x in rx_item.findall(code)))
        tk = sorted(set(int(x) for x in rx_task.findall(code))); fc = sorted(set(rx_call.findall(code)) - KW)
        per[os.path.relpath(p, ROOT)] = (len(live), fi, it, tk, fc)
        for x in fi: incs[x] += 1
        for x in it: items[x] += 1
        for x in tk: tasks[x] += 1
        for x in fc: calls[x] += 1
for f, (n, fi, it, tk, fc) in per.items():
    print("== %s (%d dong song) inc=%s item=%s task=%s" % (f, n, [i.replace(BS + BS, BS) for i in fi], it, tk))
print("\n## ITEM tong:", sorted(items.items()))
print("## TASK tong:", sorted(tasks.items()))
print("## INCLUDE tong:", sorted(incs.items()))
# engine/registered functions in JX1
reg = set()
for fn in os.listdir(r"D:\GAMEDEVNEW\Sources\Core\Src"):
    if fn.endswith(".cpp"):
        s = open(os.path.join(r"D:\GAMEDEVNEW\Sources\Core\Src", fn), "rb").read().decode("latin-1")
        reg |= set(re.findall(r'\{\s*"([A-Za-z_][A-Za-z0-9_]*)"\s*,', s))
# lua functions defined in JX1 script tree (global dir + lib) — quick: scan whole tree for 'function NAME'
defs = set()
for dp, ds, fs in os.walk(r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"):
    for f in fs:
        if f.endswith(".lua"):
            try: s = open(os.path.join(dp, f), "rb").read().decode("latin-1")
            except: continue
            defs |= set(re.findall(r'function\s+(?:[A-Za-z_][A-Za-z0-9_.:]*[.:])?([A-Za-z_][A-Za-z0-9_]*)\s*\(', s))
loc = set()
for dp, ds, fs in os.walk(ROOT):
    for f in fs:
        if f.endswith(".lua"):
            s = open(os.path.join(dp, f), "rb").read().decode("latin-1")
            loc |= set(re.findall(r'function\s+(?:[A-Za-z_][A-Za-z0-9_.:]*[.:])?([A-Za-z_][A-Za-z0-9_]*)\s*\(', s))
            loc |= set(re.findall(r'local\s+([A-Za-z_][A-Za-z0-9_]*)\s*=\s*function', s))
lua_builtin = {"getn","tinsert","tremove","floor","random","format","strlen","strsub","strfind","gsub","mod","tonumber","tostring","type","print","dofile","date","time","clock","max","min","abs","sort","ceil","strupper","strlower","strrep","call","next","rawget","rawset","setglobal","getglobal","unpack","pack","tostring","%CheckPosition","strbyte","strchar"}
print("\n## HAM GOI KHONG CO o JX1 (khong dang ky C++, khong dinh nghia Lua JX1, khong dinh nghia trong huashan2013):")
for f, c in sorted(calls.items()):
    if f in reg or f in defs or f in loc or f in lua_builtin: continue
    users = [k for k, v in per.items() if f in v[4]]
    print("   %-24s x%d  %s" % (f, c, users[:4]))
