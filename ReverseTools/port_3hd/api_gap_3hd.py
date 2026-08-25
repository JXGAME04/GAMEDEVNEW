# -*- coding: utf-8 -*-
"""
BANG KHE HO API ENGINE cho 3 tinh nang port tu ban Linux (JX2) sang JX1.
  satthu      = task/tollgate/killer + killbosshead
  phonglangdo = missions/fengling_ferry
  vuotai      = missions/challengeoftime

Nguon danh sach tep: port_3hd/closure3.json (bao dong Include).
Doi chieu voi bang dang ky ham Lua cua engine JX1 (Sources/Core/Src).
"""
import io, os, re, sys, json

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)

HERE = os.path.dirname(os.path.abspath(__file__))
LNX_SCRIPT = r"D:\ServerLinux\server1\script"
LNX_VNG = r"D:\ServerLinux\server1\vng_script"
JX1_SRC = r"D:\GAMEDEVNEW\Sources\Core\Src"
LUAMAP = r"D:\GAMEDEVNEW\ReverseTools\jx_linux_y.luamap.full.txt"

# ---------------------------------------------------------------- Lua 4.0 core
# Tu khoa Lua 4.0 (khong co goi/module)
LUA_KEYWORDS = set("""
and break do else elseif end false for function if in local nil not or repeat
return then true until while
""".split())

# Thu vien chuan Lua 4.0 (ham TOAN CUC - Lua 4.0 chua co bang string/table/math)
# Nguon: Lua 4.0 Reference Manual, muc 6 (Standard Libraries).
LUA40_STDLIB = set("""
_ALERT _ERRORMESSAGE assert call collectgarbage copytagmethods dofile dostring
error foreach foreachi getglobal getn gcinfo globals newtag next nextvar rawget
rawgetglobal rawset rawsetglobal require setglobal settag settagmethod
gettagmethod sort tag tinsert tonumber tostring tremove type unpack print
abs acos asin atan atan2 ceil cos deg exp floor log log10 max min mod rad
random randomseed sin sqrt tan frexp ldexp
format gsub strbyte strchar strfind strlen strlower strrep strsub strupper
openfile closefile readfrom writeto appendto read write remove rename tmpname
date clock time difftime getenv execute exit seek flush setlocale
""".split())

# ---------------------------------------------------------------- helpers
def read_lua(path):
    b = io.open(path, "rb").read()
    try:
        return b.decode("gbk", "replace")
    except Exception:
        return b.decode("latin-1")


def strip_lua(t):
    """Xoa comment va NOI DUNG chuoi, GIU NGUYEN so dong."""
    out = []
    i, n = 0, len(t)
    while i < n:
        c = t[i]
        # long comment / long string  [[...]]
        if c == "-" and t[i:i + 4] == "--[[":
            j = t.find("]]", i + 4)
            j = n if j < 0 else j + 2
            out.append("\n" * t.count("\n", i, j))
            i = j
            continue
        if c == "-" and t[i:i + 2] == "--":
            j = t.find("\n", i)
            j = n if j < 0 else j
            out.append("")
            i = j
            continue
        if t[i:i + 2] == "[[":
            j = t.find("]]", i + 2)
            j = n if j < 0 else j + 2
            out.append('""' + "\n" * t.count("\n", i, j))
            i = j
            continue
        if c in "'\"":
            q = c
            j = i + 1
            while j < n and t[j] != q:
                if t[j] == BS:
                    j += 2
                elif t[j] == "\n":
                    break
                else:
                    j += 1
            j = min(j + 1, n)
            out.append('""' + "\n" * t.count("\n", i, j))
            i = j
            continue
        out.append(c)
        i += 1
    return "".join(out)


CALL_RE = re.compile(r"(?<![.:\w])([A-Za-z_][A-Za-z0-9_]*)\s*\(")
DEF_RE1 = re.compile(r"function\s+([A-Za-z_][A-Za-z0-9_.:]*)\s*\(")
DEF_RE2 = re.compile(r"([A-Za-z_][A-Za-z0-9_.:]*)\s*=\s*function\s*\(")


LOCAL_RE = re.compile(r"\blocal\s+([A-Za-z_][A-Za-z0-9_]*(?:\s*,\s*[A-Za-z_][A-Za-z0-9_]*)*)")
PARAM_RE = re.compile(r"\bfunction\s*[A-Za-z_0-9_.:]*\s*\(([^)]*)\)")
FORIN_RE = re.compile(r"\bfor\s+([A-Za-z_][A-Za-z0-9_]*(?:\s*,\s*[A-Za-z_][A-Za-z0-9_]*)*)\s*(?:=|in\b)")


def locals_of(text):
    """Ten BIEN CUC BO / tham so trong tep -> khong the la ham engine toan cuc."""
    out = set()
    for rx in (LOCAL_RE, PARAM_RE, FORIN_RE):
        for m in rx.finditer(text):
            for nm in m.group(1).split(","):
                nm = nm.strip()
                if re.match(r"^[A-Za-z_][A-Za-z0-9_]*$", nm or ""):
                    out.add(nm)
    return out


def defs_of(text):
    out = set()
    for m in DEF_RE1.finditer(text):
        out.add(m.group(1))
        out.add(m.group(1).split(".")[-1].split(":")[-1])
    for m in DEF_RE2.finditer(text):
        out.add(m.group(1))
        out.add(m.group(1).split(".")[-1].split(":")[-1])
    return out


# ---------------------------------------------------------------- 1. ham do CHINH cay script Linux dinh nghia
print("[1] Quet dinh nghia ham tren toan bo cay script Linux ...")
lib_defs = set()
lib_def_where = {}
n_scan = 0
for R in (LNX_SCRIPT, LNX_VNG):
    for root, _dirs, fs in os.walk(R):
        for f in fs:
            if not f.lower().endswith(".lua"):
                continue
            p = os.path.join(root, f)
            n_scan += 1
            t = strip_lua(read_lua(p))
            for rx in (DEF_RE1, DEF_RE2):
                for m in rx.finditer(t):
                    full = m.group(1)
                    # CHI ham TOAN CUC tran (khong co '.' hay ':') moi che duoc ten
                    # toan cuc cua engine. "function Player:OpenProgressBar(...)"
                    # la PHUONG THUC cua bang Player, KHONG dinh nghia global
                    # OpenProgressBar -> khong duoc dung de loai.
                    if "." in full or ":" in full:
                        continue
                    lib_defs.add(full)
                    lib_def_where.setdefault(full, (p, t[:m.start()].count("\n") + 1))
print("    %d tep .lua, %d ten ham do script tu dinh nghia" % (n_scan, len(lib_defs)))

# ---------------------------------------------------------------- 2. quet loi goi trong bao dong 3 tinh nang
closure = json.load(io.open(os.path.join(HERE, "closure3.json"), encoding="utf-8"))
FEATS = ["satthu", "phonglangdo", "vuotai"]

calls = {}          # ten -> {feat -> [(relpath, line, is_seed)]}
per_feat_names = {f: set() for f in FEATS}
seed_names = {f: set() for f in FEATS}
nfile_feat = {}

for feat in FEATS:
    cnt = 0
    for row in closure[feat]:
        if not row["linux"]:
            continue
        rel = row["rel"]
        p = os.path.join(LNX_SCRIPT, rel.replace("/", os.sep))
        if not os.path.isfile(p):
            p = os.path.join(LNX_VNG, rel.replace("/", os.sep))
            if not os.path.isfile(p):
                continue
        cnt += 1
        t = strip_lua(read_lua(p))
        loc = locals_of(t)
        for m in CALL_RE.finditer(t):
            nm = m.group(1)
            if nm in LUA_KEYWORDS:
                continue
            if nm in loc:      # bien cuc bo / tham so giu ham -> khong phai ham engine
                continue
            # bo qua chinh dong "function ten("
            pre = t[max(0, m.start() - 12):m.start()]
            if re.search(r"function\s+$", pre):
                continue
            ln = t[:m.start()].count("\n") + 1
            calls.setdefault(nm, {}).setdefault(feat, []).append((rel, ln, row["seed"]))
            per_feat_names[feat].add(nm)
            if row["seed"]:
                seed_names[feat].add(nm)
    nfile_feat[feat] = cnt
    print("[2] %-12s : %d tep doc duoc, %d ten duoc goi" % (feat, cnt, len(per_feat_names[feat])))

engine_calls = {n: v for n, v in calls.items()
                if n not in LUA40_STDLIB and n not in lib_defs}
print("[2] Tong ten duoc goi: %d  -> sau khi tru stdlib + ham script tu dinh nghia: %d"
      % (len(calls), len(engine_calls)))

# ---------------------------------------------------------------- 3. bang dang ky JX1
print("[3] Trich bang dang ky ham Lua cua engine JX1 ...")
reg = {}    # ten -> (tep, dong, ham C)
REG_FILES = [
    os.path.join(JX1_SRC, "ScriptFuns.cpp"),
    os.path.join(JX1_SRC, "LuaFuns.cpp"),
]
ENTRY_RE = re.compile(r'\{\s*"([A-Za-z_][A-Za-z0-9_]*)"\s*,\s*([A-Za-z_][A-Za-z0-9_:]*)\s*\}')
for p in REG_FILES:
    if not os.path.isfile(p):
        continue
    t = io.open(p, "rb").read().decode("latin-1")
    for m in ENTRY_RE.finditer(t):
        nm, cf = m.group(1), m.group(2)
        ln = t[:m.start()].count("\n") + 1
        reg.setdefault(nm, (os.path.basename(p), ln, cf))
# quet them cac .cpp khac (chi nhan cap {"Ten", LuaXxx})
ENTRY_RE2 = re.compile(r'\{\s*"([A-Za-z_][A-Za-z0-9_]*)"\s*,\s*(Lua[A-Za-z0-9_]*)\s*\}')
for root, _d, fs in os.walk(JX1_SRC):
    for f in fs:
        if not f.endswith(".cpp"):
            continue
        p = os.path.join(root, f)
        if p in REG_FILES:
            continue
        t = io.open(p, "rb").read().decode("latin-1")
        for m in ENTRY_RE2.finditer(t):
            nm, cf = m.group(1), m.group(2)
            ln = t[:m.start()].count("\n") + 1
            reg.setdefault(nm, (os.path.basename(p), ln, cf))
print("    %d ten ham Lua da dang ky trong engine JX1" % len(reg))

# ---------------------------------------------------------------- 4. luamap ban Linux
luamap = {}
for line in io.open(LUAMAP, encoding="utf-8", errors="replace"):
    line = line.strip()
    if not line or line.startswith("#"):
        continue
    parts = line.split()
    if len(parts) >= 2 and parts[0].startswith("0x"):
        luamap.setdefault(parts[1], parts[0])
print("[4] luamap ban Linux: %d ten" % len(luamap))

# ---------------------------------------------------------------- 5. phan nhom
groupA = sorted(n for n in engine_calls if n in reg)
groupB = sorted(n for n in engine_calls if n not in reg)

out = {}
for n in sorted(engine_calls):
    feats = sorted(engine_calls[n].keys())
    sites = []
    for f in feats:
        for (rel, ln, seed) in engine_calls[n][f][:3]:
            sites.append("%s:%d" % (rel, ln))
    total = sum(len(v) for v in engine_calls[n].values())
    out[n] = {
        "nhom": "A" if n in reg else "B",
        "dang_ky_jx1": ("%s:%d -> %s" % reg[n]) if n in reg else None,
        "dia_chi_linux": luamap.get(n),
        "so_lan_goi": total,
        "dung_boi": feats,
        "trong_seed": sorted(f for f in feats if n in seed_names[f]),
        "vi_tri_goi": sites[:6],
        "chu_ky": None,
        "mo_ta": None,
        "do_tin_cay": None,
    }

json.dump(out, io.open(os.path.join(HERE, "api_gap_raw.json"), "w", encoding="utf-8"),
          ensure_ascii=False, indent=1, sort_keys=True)

print()
print("=== NHOM A (JX1 DA CO): %d ===" % len(groupA))
print("=== NHOM B (CHUA CO - CAN VIET MOI): %d ===" % len(groupB))
for n in groupB:
    d = out[n]
    print("  %-32s x%-5d %-12s seed=%-22s %s   %s"
          % (n, d["so_lan_goi"], d["dia_chi_linux"] or "-",
             ",".join(d["trong_seed"]) or "-",
             ",".join(d["dung_boi"]), d["vi_tri_goi"][0] if d["vi_tri_goi"] else ""))

print()
print("--- CHI TIET NHOM A (de doi chieu nghia) ---")
for n in groupA:
    d = out[n]
    print("  %-32s %-40s linux=%s" % (n, d["dang_ky_jx1"], d["dia_chi_linux"] or "-"))
