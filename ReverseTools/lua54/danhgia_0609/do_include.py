import os, re, sys, time, ctypes
os.environ["LUA54_BO_KIEM"] = "1"
ROOT = "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server"
OUTD = sys.argv[1]
BS = chr(92)
files, real = {}, {}
for top in ("script", "scriptjx2"):
    for dp, dn, fn in os.walk(os.path.join(ROOT, top)):
        for f in fn:
            if f.lower().endswith(".lua"):
                p = os.path.join(dp, f)
                rel = os.path.relpath(p, ROOT).replace(os.sep, "/").lower()
                files[rel] = os.path.getsize(p); real[rel] = p
rx = re.compile(r'Include\s*\(\s*"([^"]+)"')
def norm(s):
    s = s.replace(BS + BS, BS).replace(BS, "/").lower()
    while s.startswith("/"): s = s[1:]
    return s
def resolve(p):
    if p in files: return p
    if p.startswith("script/lib/"):
        q = "scriptjx2/lib/" + p[len("script/lib/"):]
        if q in files: return q
    if p.startswith("script/tong/"):
        q = "scriptjx2/tong_vn/" + p[len("script/tong/"):]
        if q in files: return q
    return None
inc, missing, n_inc_lines = {}, {}, 0
for rel in files:
    with open(real[rel], "rb") as fh:
        data = fh.read().decode("latin-1")
    lines = []
    for ln in data.split("\n"):
        i = ln.find("--")
        if i >= 0: ln = ln[:i]
        lines.append(ln)
    data = "\n".join(lines)
    lst = []
    for m in rx.finditer(data):
        n_inc_lines += 1
        p = norm(m.group(1)); r = resolve(p)
        if r is None: missing[p] = missing.get(p, 0) + 1
        else: lst.append(r)
    inc[rel] = lst
sys.setrecursionlimit(20000)
memo, onstack = {}, set()
def cost(rel):
    if rel in memo: return memo[rel]
    if rel in onstack: return (0, 0)
    onstack.add(rel)
    b, n = files[rel], 1
    for c in inc[rel]:
        cb, cn = cost(c); b += cb; n += cn
    onstack.discard(rel)
    memo[rel] = (b, n); return memo[rel]
def uniq(rel):
    seen, st = set(), [rel]
    while st:
        x = st.pop()
        if x in seen: continue
        seen.add(x); st.extend(inc[x])
    return seen
tot_tree = sum(files.values())
tot_mult, tot_files_mult, tot_uniq = 0, 0, 0
roots_with_inc = 0
per_root = []
times_included = {}
for rel in files:
    b, n = cost(rel); u = uniq(rel)
    ub = sum(files[x] for x in u)
    tot_mult += b; tot_files_mult += n; tot_uniq += ub
    if inc[rel]: roots_with_inc += 1
    per_root.append((b, n, ub, len(u), rel))
    for x in u:
        if x != rel: times_included[x] = times_included.get(x, 0) + 1
out = []
out.append("CAY SCRIPT LIVE (script + scriptjx2): %d tep, %.2f MB nguon" % (len(files), tot_tree / 1048576))
out.append("Dong Include (ngoai chu thich): %d; tep co Include: %d; Include khong tim thay tep: %d (%d dong)" % (n_inc_lines, roots_with_inc, len(missing), sum(missing.values())))
out.append("Moi tep = 1 lua_State rieng, Include = bien dich lai tep vao state do (khong dedupe).")
out.append("  Tong byte BIEN DICH luc boot (Include lap ke ca trung trong cung state): %.2f MB = %.1f x cay nguon; so lan bien dich tep: %d" % (tot_mult / 1048576, tot_mult / tot_tree, tot_files_mult))
out.append("  Neu dedupe trong tung state (moi tep 1 lan/state):                  %.2f MB = %.1f x" % (tot_uniq / 1048576, tot_uniq / tot_tree))
out.append("  Neu MOT state chung + cache bytecode (giai doan 2):                 %.2f MB = 1.0 x" % (tot_tree / 1048576))
per_root.sort(reverse=True)
out.append("")
out.append("Top 12 state nang nhat (byte bien dich ke ca Include lap | so tep | duy nhat):")
for b, n, ub, un, rel in per_root[:12]:
    out.append("  %8.0f KB  %4d tep  (duy nhat %8.0f KB / %3d tep)  %s" % (b / 1024, n, ub / 1024, un, rel))
ti = sorted(((c * files[x], c, x) for x, c in times_included.items()), reverse=True)
out.append("")
out.append("Top 15 tep bi chep vao nhieu state nhat (so state x kich thuoc = byte trung):")
for tb, c, x in ti[:15]:
    out.append("  %4d state x %6.1f KB = %8.1f KB  %s" % (c, files[x] / 1024, tb / 1024, x))
out.append("")
out.append("Include khong tim thay (top 10): " + "; ".join("%s x%d" % (k, v) for k, v in sorted(missing.items(), key=lambda kv: -kv[1])[:10]))

# ---- do bang Lua54Dll: bien dich tung tep (thoi gian + bo nho proto), state co ban
D = ctypes.CDLL("D:/GAMEDEVNEW/Lib/lua54/x64/Lua54Dll.dll")
D.lua4_open.restype = ctypes.c_void_p; D.lua4_open.argtypes = [ctypes.c_int]
D.lua4_close.argtypes = [ctypes.c_void_p]
D.lua4_baselibopen.argtypes = [ctypes.c_void_p]
D.lua4_dostring.argtypes = [ctypes.c_void_p, ctypes.c_char_p]; D.lua4_dostring.restype = ctypes.c_int
D.lua4_settop.argtypes = [ctypes.c_void_p, ctypes.c_int]
D.lua4_getglobal.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
D.lua4_tonumber.argtypes = [ctypes.c_void_p, ctypes.c_int]; D.lua4_tonumber.restype = ctypes.c_double
D.lua4_tostring.argtypes = [ctypes.c_void_p, ctypes.c_int]; D.lua4_tostring.restype = ctypes.c_char_p
CFN = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_void_p)
D.lua4_pushcclosure.argtypes = [ctypes.c_void_p, CFN, ctypes.c_int]
D.lua4_setglobal.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
def num(L, name):
    D.lua4_getglobal(L, name.encode()); v = D.lua4_tonumber(L, -1); D.lua4_settop(L, 0); return v
def sstr(L, name):
    D.lua4_getglobal(L, name.encode()); v = D.lua4_tostring(L, -1); D.lua4_settop(L, 0); return (v or b"").decode("latin-1")
# NOTE: shim _G.collectgarbage bi de quy vo han (lua4compat.lua:175) -> phai lay ban that qua debug.getregistry? khong co.
# Dung lua_gc tu C: DLL xuat lua_gc? thu; neu khong co thi dung lua4_getgccount (KB).
try:
    D.lua4_getgccount.argtypes = [ctypes.c_void_p]; D.lua4_getgccount.restype = ctypes.c_int
    def kb(L):
        return float(D.lua4_getgccount(L))
    co_gccount = True
except Exception:
    co_gccount = False
try:
    D.lua_gc.argtypes = [ctypes.c_void_p, ctypes.c_int]; D.lua_gc.restype = ctypes.c_int
    LUA_GCCOLLECT, LUA_GCCOUNT, LUA_GCCOUNTB = 2, 3, 4
    def collect(L): D.lua_gc(L, LUA_GCCOLLECT)
    def kb_exact(L): return D.lua_gc(L, LUA_GCCOUNT) + D.lua_gc(L, LUA_GCCOUNTB) / 1024.0
    co_luagc = True
except Exception:
    co_luagc = False
    def collect(L): pass
    def kb_exact(L): return kb(L)
out.append("")
out.append("(DLL xuat lua_gc: %s; lua4_getgccount: %s)" % (co_luagc, co_gccount))
# state co ban
t0 = time.perf_counter()
for i in range(100):
    L = D.lua4_open(100); D.lua4_baselibopen(L); D.lua4_close(L)
ms_state = (time.perf_counter() - t0) * 10
L = D.lua4_open(100); D.lua4_baselibopen(L)
collect(L); kb0 = kb_exact(L)
def dummy(Lx): return 0
cb = CFN(dummy)
t0 = time.perf_counter()
for i in range(742):
    D.lua4_pushcclosure(L, cb, 0); D.lua4_setglobal(L, ("GameFun_%03d" % i).encode())
ms_reg = (time.perf_counter() - t0) * 1000
collect(L); kb1 = kb_exact(L)
out.append("STATE CO BAN (lua4_open + openlibs + shim lua4compat): %.1f KB, tao+huy %.2f ms; +742 ham C dang ky: +%.1f KB, %.2f ms" % (kb0, ms_state, kb1 - kb0, ms_reg))
# bien dich tung tep: bo nho proto (do tu Python: collect -> load -> collect -> count, giu ham song trong bien toan cuc F)
lst = os.path.join(OUTD, "ds_tep.txt")
bad = 0
order = []
with open(lst, "wb") as fh:
    for rel in sorted(files):
        try:
            fh.write(real[rel].replace("/", BS).encode("mbcs") + b"\n"); order.append(rel)
        except Exception:
            bad += 1
D.lua4_dostring(L, ('DS = "%s"' % lst.replace(BS, "/")).encode()); D.lua4_settop(L, 0)
D.lua4_dostring(L, b'DSL = {} for line in io.lines(DS) do DSL[#DSL+1] = line end N_DS = #DSL'); D.lua4_settop(L, 0)
n_ds = int(num(L, "N_DS"))
mem_rel = {}
n_ok = n_err = 0
loi = ""
for i in range(n_ds):
    collect(L); m0 = kb_exact(L)
    D.lua4_dostring(L, ('F, LOI_F = loadfile(DSL[%d])' % (i + 1)).encode()); D.lua4_settop(L, 0)
    collect(L); m1 = kb_exact(L)
    D.lua4_getglobal(L, b"F")
    isnil = D.lua4_tostring(L, -1) is None and True
    D.lua4_settop(L, 0)
    # kiem F ~= nil bang Lua
    D.lua4_dostring(L, b'F_OK = (F ~= nil) and 1 or 0'); D.lua4_settop(L, 0)
    ok = num(L, "F_OK") > 0
    if ok: n_ok += 1
    else:
        n_err += 1
        if len(loi) < 400: loi += sstr(L, "LOI_F")[:120] + " | "
    mem_rel[order[i]] = m1 - m0
    D.lua4_dostring(L, b'F = nil'); D.lua4_settop(L, 0)
# thoi gian bien dich thuan: 2 luot, lay luot nhanh
code2 = 'local t0 = os.clock() local n = 0 for i = 1, #DSL do local f = loadfile(DSL[i]) if f then n = n + 1 end end T_COMPILE = (os.clock() - t0) * 1000'
best = 1e9
for i in range(2):
    D.lua4_dostring(L, code2.encode()); D.lua4_settop(L, 0)
    v = num(L, "T_COMPILE"); best = min(best, v)
D.lua4_close(L)
memo2, onstack2 = {}, set()
def mcost(rel):
    if rel in memo2: return memo2[rel]
    if rel in onstack2: return 0.0
    onstack2.add(rel)
    m = mem_rel.get(rel, 0.0)
    for c in inc[rel]: m += mcost(c)
    onstack2.discard(rel)
    memo2[rel] = m; return m
tot_mem_mult = sum(mcost(rel) for rel in files)
tot_mem_uniq = sum(mem_rel.values())
thr = tot_tree / 1048576 / (best / 1000) if best > 0 else 0
out.append("BIEN DICH (loadfile, khong chay) toan cay %d tep %.2f MB: %.0f ms mot luot (%.1f MB/s); loi %d (%s); tep khong ma hoa duoc ten: %d" % (n_ok + n_err, tot_tree / 1048576, best, thr, n_err, loi[:300], bad))
out.append("BO NHO PROTO (bytecode+hang+debug) khi bien dich moi tep 1 lan: %.1f MB (= %.2f x nguon)" % (tot_mem_uniq / 1024, tot_mem_uniq * 1024 / tot_tree))
out.append("UOC LUONG LUC BOOT, %d state (chi phan bien dich, chua tinh du lieu bang do chunk tao ra):" % len(files))
out.append("  thoi gian bien dich: %.1f MB / %.1f MB/s = %.1f s (+ %d x %.2f ms tao state = %.1f s)" % (tot_mult / 1048576, thr, tot_mult / 1048576 / thr if thr else 0, len(files), ms_state, len(files) * ms_state / 1000))
out.append("  bo nho Lua: proto %.0f MB (lap Include) + state co ban %d x %.0f KB = %.0f MB  => tong ~%.0f MB; neu mot state chung: proto %.0f MB + %.0f KB" % (
    tot_mem_mult / 1024, len(files), kb1, len(files) * kb1 / 1024, tot_mem_mult / 1024 + len(files) * kb1 / 1024, tot_mem_uniq / 1024, kb1))
top_mem = sorted(((mem_rel.get(r, 0.0), r) for r in files), reverse=True)[:8]
out.append("  8 tep ton bo nho proto nhat: " + "; ".join("%s %.0f KB" % (r, m) for m, r in top_mem))
txt = "\n".join(out)
open(os.path.join(OUTD, "include_baocao.txt"), "w", encoding="utf-8").write(txt)
print(txt)
