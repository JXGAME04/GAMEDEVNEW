r"""chon_includeonce.py - PA-3/C6: chon tep an toan de danh dau '-- @IncludeOnce' (Lua54Dll chi chay than tep mot lan / state).
An toan = than tep (cap 0) CHI gom: chu thich, dong trong, `function X(...)`/`function X.Y(...)`/`function X:Y(...)` ... `end`,
`Include("...")` ma tep duoc Include cung an toan (de quy), `local function`. KHONG co: gan bien/bang cap tep, `local x = ...`,
goi ham cap tep, if/for/while cap tep. Ngoai ra ten ham cap 0 KHONG duoc dinh nghia o tep khac (Include lai co the la de
"khoi phuc" dinh nghia -> giu nguyen hanh vi cu).
  python chon_includeonce.py           -> bao cao (so tep an toan + so luot Include (tinh) roi vao tep an toan)
  python chon_includeonce.py sua       -> chen dong dau '-- @IncludeOnce ...' (latin-1, giu CRLF)
"""
import os, sys, re, io, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
SUA = len(sys.argv) > 1 and sys.argv[1] == "sua"
BS = chr(92)
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()

def tach(src):
    """xoa noi dung chuoi/chu thich (giu so dong va dau ngoac kep)"""
    code = []
    i = 0; n = len(src)
    while i < n:
        c = src[i]
        if c == "-" and src.startswith("--", i):
            j = i + 2
            m = re.match(r"\[(=*)\[", src[j:j + 10])
            if m:
                end = src.find("]" + m.group(1) + "]", j + len(m.group(0)))
                end = n if end < 0 else end + len(m.group(1)) + 2
                code.append(re.sub(r"[^\n]", " ", src[i:end])); i = end; continue
            end = src.find("\n", j); end = n if end < 0 else end
            code.append(" " * (end - i)); i = end; continue
        if c == '"' or c == "'":
            j = i + 1
            while j < n and src[j] != c:
                if src[j] == "\\": j += 1
                j += 1
            j = min(j + 1, n)
            code.append('"' + " " * (j - i - 2) + '"' if j - i >= 2 else " "); i = j; continue
        m = re.match(r"\[(=*)\[", src[i:i + 10]) if c == "[" else None
        if m:
            end = src.find("]" + m.group(1) + "]", i + len(m.group(0)))
            end = n if end < 0 else end + len(m.group(1)) + 2
            code.append(re.sub(r"[^\n]", " ", src[i:end])); i = end; continue
        code.append(c); i += 1
    return "".join(code)

files = {}
for top in ("script", os.path.join("scriptjx2", "tong_vn"), os.path.join("scriptjx2", "lib")):
    for dp, dn, fn in os.walk(os.path.join(ROOT, top)):
        for f in fn:
            if f.lower().endswith(".lua"):
                p = os.path.join(dp, f)
                files[os.path.relpath(p, ROOT).lower()] = p

RX_INC_CODE = re.compile(r'^\s*Include\s*\(\s*"[^"]*"\s*\)\s*;?\s*$')
RX_INC_SRC = re.compile(r'Include\s*\(\s*"([^"]+)"')
def norm_inc(s):
    s = s.replace(BS + BS, BS).replace("/", BS).lower().lstrip(BS)
    if s.startswith("script" + BS) or s.startswith("scriptjx2" + BS): return s
    return None

info = {}
for rel, p in files.items():
    src = rd(p)
    code = tach(src)
    ok = True; incs = []; fns = []
    depth = 0
    src_lines = src.split("\n")
    for idx, raw in enumerate(code.split("\n")):
        ln = raw.strip()
        if not ln: continue
        if depth == 0:
            m = re.match(r"^function\s+([A-Za-z_][A-Za-z0-9_.:]*)\s*\(", ln)
            if m:
                fns.append(m.group(1))
            elif RX_INC_CODE.match(ln):
                m2 = RX_INC_SRC.search(src_lines[idx] if idx < len(src_lines) else "")
                k = norm_inc(m2.group(1)) if m2 else None
                if k: incs.append(k)
                else: ok = False
            elif re.match(r"^local\s+function\s", ln):
                pass
            elif re.match(r'^[A-Za-z_][A-Za-z0-9_]*\s*=\s*(-?\d+(\.\d+)?|"[^"]*"|true|false|nil)\s*;?$', ln):
                fns.append(ln.split("=")[0].strip())      # hang so bat bien: gan lai cung gia tri = idempotent (kiem trung ten nhu ham)
            else:
                ok = False
        toks = re.findall(r"\b(function|if|for|while|do|repeat|end|until)\b", ln)
        for t in toks:
            if t in ("function", "if", "for", "while", "repeat"): depth += 1
            elif t == "do":
                if not re.search(r"\b(for|while)\b", ln): depth += 1
            elif t in ("end", "until"): depth -= 1
    info[rel] = (ok, incs, fns)

dem = collections.Counter()
for rel, (ok, incs, fns) in info.items():
    for f in fns: dem[f] += 1
antoan = {}
def kiem(rel, dang=()):
    if rel in antoan: return antoan[rel]
    if rel in dang: return False
    ok, incs, fns = info.get(rel, (False, [], []))
    if not ok or not fns or any(dem[f] > 1 for f in fns):
        antoan[rel] = False; return False
    for k in incs:
        if k not in info:
            antoan[rel] = False; return False
        if not kiem(k, dang + (rel,)):
            antoan[rel] = False; return False
    antoan[rel] = True; return True
for rel in info: kiem(rel)
tot = [r for r, v in antoan.items() if v]
# so luot moi tep duoc Include (truc tiep + gian tiep) khi nap TOAN BO cay (moi tep goc mot state) - tinh, khong khu trung
count = collections.Counter()
def thu(rel, dang):
    for k in info.get(rel, (False, [], []))[1]:
        count[k] += 1
        if k not in dang and k in info:
            thu(k, dang | {k})
for rel in info:
    thu(rel, {rel})
tong_inc = sum(count.values())
tiet = sum(count[r] for r in tot)
n_khong_ok = sum(1 for r, (ok, i, f) in info.items() if not ok)
print("Tep .lua: %d; than tep toan ham: %d; an toan (de quy Include): %d; luot Include tinh tong %d, roi vao tep an toan %d" % (
    len(info), len(info) - n_khong_ok, len(tot), tong_inc, tiet))
print("Tep bi Include nhieu nhat (an toan? / so luot):")
for r, c in count.most_common(20):
    print("   %5d  %s  %s" % (c, "AN TOAN" if antoan.get(r) else "khong  ", r))
if SUA:
    n = 0
    for r in tot:
        p = files[r]
        s = rd(p)
        if "@IncludeOnce" in s[:512]: continue
        nl = "\r\n" if "\r\n" in s else "\n"
        bom = ""
        if s.startswith("\xef\xbb\xbf"): bom = s[:3]; s = s[3:]
        s = bom + "-- @IncludeOnce  [LUA54 06/09 toi] tep toan ham: Lua54Dll chi chay than mot lan moi state (chon_includeonce.py)" + nl + s
        io.open(p, "w", encoding="latin-1", newline="").write(s); n += 1
    print("DA DANH DAU %d tep" % n)
