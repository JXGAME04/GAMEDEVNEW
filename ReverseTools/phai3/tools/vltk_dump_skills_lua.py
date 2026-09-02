# -*- coding: utf-8 -*-
"""vltk_dump_skills_lua.py - nap bang SKILLS cua huashan/wuhuntang/xiaoyao.lua bang lua4.exe (Lua 4 cua engine),
in TSV (bang, thuoc tinh, gia tri) roi so JX1 (cay chay that) voi VLTK (vltk_raw). DUNG: python vltk_dump_skills_lua.py [--full]
"""
import io, os, sys, subprocess, tempfile, re

sys.stdout.reconfigure(encoding="utf-8")
LUA4 = r"D:\GAMEDEVNEW\ReverseTools\lua4\lua4.exe"
OURS = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\skill"
VLTK = r"D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw"
TMP = os.path.join(os.environ.get("TEMP", "."), "vltk_lua_dump")
os.makedirs(TMP, exist_ok=True)
FULL = "--full" in sys.argv

DUMP = r'''
function dumpv(v)
  if type(v) == "table" then
    local s = "{"
    local n = getn(v)
    if n > 0 then
      for i = 1, n do s = s .. dumpv(v[i]) .. "," end
    else
      for k, x in v do s = s .. tostring(k) .. "=" .. dumpv(x) .. "," end
    end
    return s .. "}"
  end
  return tostring(v)
end
for tname, t in SKILLS do
  if type(t) == "table" then
    for attr, val in t do
      print(tname .. "\t" .. tostring(attr) .. "\t" .. dumpv(val))
    end
  end
end
'''

def dump(path):
    wrap = os.path.join(TMP, "wrap.lua")
    io.open(wrap, "w", encoding="latin-1", newline="\n").write('SKILLS = {}\ndofile("%s")\n' % path.replace("\\", "/") + DUMP)
    r = subprocess.run([LUA4, "-s16384", wrap], capture_output=True)
    out = r.stdout.decode("latin-1")
    if r.returncode != 0 or not out.strip():
        print("LOI lua4", path, r.returncode, r.stderr.decode("latin-1")[:300], out[:300])
    d = {}
    for line in out.splitlines():
        p = line.split("\t")
        if len(p) == 3:
            d[(p[0], p[1])] = p[2]
    return d

def main():
    tot = 0
    for nm in ["huashan", "wuhuntang", "xiaoyao"]:
        a = dump(os.path.join(OURS, nm + ".lua"))
        b = dump(os.path.join(VLTK, "script__skill__%s.lua" % nm))
        keys = sorted(set(a) | set(b))
        print("=" * 16, nm, "JX1 %d muc, VLTK %d muc" % (len(a), len(b)))
        tabs_a = set(k[0] for k in a); tabs_b = set(k[0] for k in b)
        print("bang chi VLTK:", sorted(tabs_b - tabs_a)); print("bang chi JX1:", sorted(tabs_a - tabs_b))
        n = 0
        for k in keys:
            va, vb = a.get(k), b.get(k)
            if va == vb: continue
            n += 1
            if not FULL and n > 60: continue
            print("%s.%s\n    JX1 : %s\n    VLTK: %s" % (k[0], k[1], (va or "-")[:200], (vb or "-")[:200]))
        print("=> %d muc khac" % n); tot += n
    print("TONG", tot)

if __name__ == "__main__":
    main()
