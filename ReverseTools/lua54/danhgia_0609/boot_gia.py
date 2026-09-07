"""boot_gia.py - mo phong boot LoadAllScript cua GameServer bang Lua54Dll (moi tep mot state, Include that qua lua4_dofile)
   de doi chung cache Include (2a+2b) vs khong cache: thoi gian + tap loi _ALERT phai GIONG NHAU.
   python boot_gia.py <Lua54Dll.dll> cache|khongcache [gioihan_so_tep] [tep_ghi_loi]
   Chay voi cwd = thu muc server (dofile tuong doi), nhung MOI ham ghi/xoa tep deu bi vo hieu (sandbox)."""
import ctypes, os, sys, time
DLL = sys.argv[1]
MODE = sys.argv[2]
LIMIT = int(sys.argv[3]) if len(sys.argv) > 3 else 0
LOG = sys.argv[4] if len(sys.argv) > 4 else None
ROOT = os.environ.get("SAPXEP_ROOT", r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server")
os.environ["LUA54_BO_KIEM"] = "1"
if MODE == "khongcache":
    os.environ["LUA54_KHONG_CACHE"] = "1"
os.chdir(ROOT)
D = ctypes.CDLL(DLL)
vp, ci, cp = ctypes.c_void_p, ctypes.c_int, ctypes.c_char_p
D.lua4_open.restype = vp; D.lua4_open.argtypes = [ci]
D.lua4_close.argtypes = [vp]
D.lua4_baselibopen.argtypes = [vp]
D.lua4_dostring.argtypes = [vp, cp]; D.lua4_dostring.restype = ci
D.lua4_dofile.argtypes = [vp, cp]; D.lua4_dofile.restype = ci
D.lua4_dobuffer.argtypes = [vp, cp, ctypes.c_size_t, cp]; D.lua4_dobuffer.restype = ci
D.lua4_settop.argtypes = [vp, ci]
D.lua4_gettop.argtypes = [vp]; D.lua4_gettop.restype = ci
D.lua4_tostring.argtypes = [vp, ci]; D.lua4_tostring.restype = cp
D.lua4_getglobal.argtypes = [vp, cp]
D.lua4_setglobal.argtypes = [vp, cp]
D.lua4_pushnumber.argtypes = [vp, ctypes.c_double]
D.lua4_tonumber.argtypes = [vp, ci]; D.lua4_tonumber.restype = ctypes.c_double
CFN = ctypes.CFUNCTYPE(ci, vp)
D.lua4_pushcclosure.argtypes = [vp, CFN, ci]
try:
    D.lua4_inc_stats.argtypes = [ctypes.POINTER(ctypes.c_longlong)] * 5
    HAS_STATS = True
except Exception:
    HAS_STATS = False

ROOT_B = ROOT.encode("latin-1")
def include_cb(L):
    try:
        p = D.lua4_tostring(L, 1)
        if not p:
            return 0
        if p[:1] in (b"\\", b"/"):
            full = ROOT_B + b"\\" + p[1:].lower()
        else:
            full = os.getcwd().encode("latin-1") + b"\\" + p.lower()
        # remap JX2 nhu sJX2RemapScriptPath: chi khi tep khong ton tai
        low = full.lower()
        try:
            exists = os.path.exists(full.decode("ascii"))
            asc = True
        except UnicodeDecodeError:
            exists, asc = True, False
        if asc and not exists:
            for a, b in ((b"\\script\\lib\\", b"\\scriptjx2\\lib\\"), (b"\\script\\tong\\", b"\\scriptjx2\\tong_vn\\")):
                i = low.find(a)
                if i >= 0:
                    full = full[:i] + b + full[i + len(a):]
                    break
        D.lua4_dofile(L, full)
    except Exception as e:
        sys.stderr.write("include_cb loi: %r\n" % (e,))
    return 0
INCLUDE_CB = CFN(include_cb)

STUB = b"""
L4_LOI = {}
_ALERT = function(m) L4_LOI[#L4_LOI + 1] = tostring(m) end
_ERRORMESSAGE = function(m) _ALERT("error: " .. tostring(m)) end
local function rong() return nil end
setmetatable(_G, {__index = function(t, k) return rong end})
-- sandbox: khong ghi/xoa tep
local io_open = io.open
io.open = function(n, m) m = m or "r" if string.find(m, "[wa+]") then return nil, "sandbox" end return io_open(n, m) end
os.remove = function() return nil end  os.rename = function() return nil end  os.execute = function() return nil end
os.exit = function() end  os.tmpname = function() return "sandbox.tmp" end
remove = os.remove  rename = os.rename  execute = os.execute  exit = os.exit  tmpname = os.tmpname
writeto = function() return nil end  appendto = function() return nil end
debug.sethook(function() error("QUA HAN lenh (vong lap?)") end, "", 20000000)
"""

files = []
for top in ("script", os.path.join("scriptjx2", "tong_vn")):
    for dp, dn, fn in os.walk(os.path.join(ROOT, top)):
        for f in fn:
            if f.lower().endswith(".lua") or f.lower().endswith(".txt"):
                files.append(os.path.join(dp, f))
files.sort()
if LIMIT:
    files = files[:LIMIT]
print("MODE=%s, %d tep goc" % (MODE, len(files)))
t_all = time.perf_counter()
n_err_states = 0
loi_all = []
t_open = t_run = 0.0
for i, path in enumerate(files):
    t0 = time.perf_counter()
    L = D.lua4_open(100)
    D.lua4_baselibopen(L)
    D.lua4_dostring(L, STUB); D.lua4_settop(L, 0)
    D.lua4_pushcclosure(L, INCLUDE_CB, 0); D.lua4_setglobal(L, b"Include")
    rel = os.path.relpath(path, ROOT)
    if "scriptjx2" in rel or "script_protocol" in rel:
        D.lua4_pushnumber(L, 1.0); D.lua4_setglobal(L, b"MODEL_GAMESERVER")
    t1 = time.perf_counter()
    try:
        data = open(path, "rb").read()
    except Exception:
        data = b""
    name = ("@" + rel).encode("mbcs", "replace")
    D.lua4_dobuffer(L, data, len(data), name); D.lua4_settop(L, 0)
    t2 = time.perf_counter()
    # thu loi
    D.lua4_dostring(L, b"L4_N = #L4_LOI  L4_S = table.concat(L4_LOI, ' || ')"); D.lua4_settop(L, 0)
    D.lua4_getglobal(L, b"L4_N"); n = int(D.lua4_tonumber(L, -1)); D.lua4_settop(L, 0)
    if n > 0:
        n_err_states += 1
        D.lua4_getglobal(L, b"L4_S"); s = D.lua4_tostring(L, -1) or b""; D.lua4_settop(L, 0)
        loi_all.append("%s\t%d\t%s" % (rel, n, s[:300].decode("latin-1", "replace")))
    D.lua4_close(L)
    t_open += t1 - t0; t_run += t2 - t1
dt = time.perf_counter() - t_all
print("TONG %.2f s (tao state+shim %.2f s, chay than tep + Include %.2f s); state co loi _ALERT: %d / %d" % (dt, t_open, t_run, n_err_states, len(files)))
if HAS_STATS and MODE == "cache":
    v = [ctypes.c_longlong(0) for _ in range(5)]
    D.lua4_inc_stats(*[ctypes.byref(x) for x in v])
    print("cache Include: dung lai cung state %d, bytecode chung %d, bien dich %d, bo qua phan tich %.1f MB, bytecode giu %.1f MB" % (
        v[0].value, v[1].value, v[2].value, v[3].value / 1048576.0, v[4].value / 1048576.0))
if LOG:
    open(LOG, "w", encoding="utf-8").write("\n".join(sorted(loi_all)) + "\n")
    print("ghi loi ->", LOG)
