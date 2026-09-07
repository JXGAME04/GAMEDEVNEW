"""ptich_54.py - phan tich tinh cay script may chu: cho nao ap dung tinh nang Lua 5.4 co loi.
   Ra: <out>/PTICH_54_baocao.md + PTICH_54_data.json
   python ptich_54.py <out dir>"""
import os, re, sys, json, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
OUT = sys.argv[1] if len(sys.argv) > 1 else "."
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
SRC_CPP = r"D:\GAMEDEVNEW\Sources\Core\Src"
BS = chr(92)

KW_OPEN = {"function", "if", "do", "repeat"}
LOOP_KW = {"for", "while", "repeat"}
ENGINE_NAMES = set("""main OnDialog OnDeath OnCreate OnTimer OnEnter OnLeave OnGame RunTime OnTrigger OnNewWorld OnLeaveWorld
OnUse OnCheck OnKill OnKillNpc OnRevive OnLogin OnLogout OnLevelUp OnPlayerLeave OnPlayerEnter OnPlayerLogin OnPlayerLogout
OnArrive OnBeKilled OnHit OnUseItem OnEquip OnUnEquip OnStart OnEnd OnOpen OnClose OnActivate OnSkill OnCastSkill OnEvent
OnMsg OnMessage OnRelayMsg OnTick OnMinute OnHour OnDay OnSelect OnChoose""".split())

# ---------------------------------------------------------------- tokenizer (bo comment, tach string)
def tokenize(src):
    """tra (code_tokens: list of (line, tok)), strings: list of str) - tok la identifier/keyword/toan tu"""
    i, n, line = 0, len(src), 1
    toks, strs = [], []
    while i < n:
        c = src[i]
        if c == "\n":
            line += 1; i += 1; continue
        if c == " " or c == "\t" or c == "\r":
            i += 1; continue
        if c == "-" and src.startswith("--", i):
            if src.startswith("--[[", i) or src.startswith("--[=", i):
                m = re.match(r"--\[(=*)\[", src[i:])
                close = "]" + m.group(1) + "]"
                j = src.find(close, i + len(m.group(0)))
                if j < 0: j = n
                line += src.count("\n", i, j); i = j + len(close); continue
            j = src.find("\n", i)
            if j < 0: j = n
            i = j; continue
        if c in "\"'":
            j = i + 1
            while j < n and src[j] != c:
                if src[j] == BS: j += 1
                if j < n and src[j] == "\n": break
                j += 1
            strs.append(src[i + 1:j]); i = j + 1; continue
        if c == "[" and re.match(r"\[=*\[", src[i:]):
            m = re.match(r"\[(=*)\[", src[i:]); close = "]" + m.group(1) + "]"
            j = src.find(close, i + len(m.group(0)))
            if j < 0: j = n
            strs.append(src[i + len(m.group(0)):j]); line += src.count("\n", i, j); i = j + len(close); continue
        m = re.match(r"[A-Za-z_][A-Za-z0-9_]*", src[i:])
        if m:
            toks.append((line, m.group(0))); i += len(m.group(0)); continue
        m = re.match(r"\d[\w.]*", src[i:])
        if m:
            toks.append((line, "<num>")); i += len(m.group(0)); continue
        if src.startswith("..", i):
            toks.append((line, "..")); i += 2 if not src.startswith("...", i) else 3; continue
        toks.append((line, c)); i += 1
    return toks, strs

def analyze(path, src):
    toks, strs = tokenize(src)
    lines = src.split("\n")
    res = dict(loc=len(lines), concat_loop=[], getn_while=[], strfind_loop=[], dostring=[], call=[], floor_div=0,
               gdefs=[], gassign=set(), idents=set(), str_words=set(), while_n=0, for_n=0, methods=0, tag=0, longfn=[])
    stack = []        # ('for'|'while'|'repeat'|'function'|'if'|'do', line)
    expect_do = False
    fn_start = None
    idents = res["idents"]
    for k, (ln, t) in enumerate(toks):
        idents.add(t)
        if t in ("for", "while"):
            stack.append((t, ln)); expect_do = True
            res["while_n" if t == "while" else "for_n"] += 1
            # while ... getn(  trong dieu kien (cung dong)
            if t == "while":
                seg = lines[ln - 1] if ln - 1 < len(lines) else ""
                if "getn" in seg: res["getn_while"].append(ln)
            continue
        if t == "do":
            if expect_do: expect_do = False
            else: stack.append(("do", ln))
            continue
        if t == "repeat":
            stack.append(("repeat", ln)); continue
        if t == "function":
            stack.append(("function", ln))
            # ten ham
            nxt = toks[k + 1][1] if k + 1 < len(toks) else ""
            prev = toks[k - 1][1] if k > 0 else ""
            if re.match(r"[A-Za-z_]", nxt) and nxt not in ("(",):
                # function Name / function A.B / function A:B
                nm = nxt
                sep = toks[k + 2][1] if k + 2 < len(toks) else ""
                if sep in (".", ":"):
                    res["methods"] += 1
                elif prev != "local":
                    res["gdefs"].append((nm, ln))
            elif prev == "=" and k >= 2:
                nm = toks[k - 2][1]
                if re.match(r"[A-Za-z_]", nm) and (k < 3 or toks[k - 3][1] not in (".", ":", "local")):
                    res["gdefs"].append((nm, ln))
            continue
        if t == "if":
            stack.append(("if", ln)); continue
        if t in ("end", "until"):
            if stack:
                kind, l0 = stack.pop()
                if kind == "function" and ln - l0 > 200: res["longfn"].append((l0, ln - l0))
            continue
        in_loop = any(s[0] in LOOP_KW for s in stack)
        if t == ".." and in_loop and k >= 3:
            # mau  X = X ..   (X cung ten truoc dau =)
            if toks[k - 1][1] == toks[k - 3][1] and toks[k - 2][1] == "=" and re.match(r"[A-Za-z_]", toks[k - 1][1]):
                res["concat_loop"].append(ln)
        if t == "strfind" and in_loop:
            res["strfind_loop"].append(ln)
        if t == "dostring": res["dostring"].append(ln)
        if t == "call" and k + 1 < len(toks) and toks[k + 1][1] == "(": res["call"].append(ln)
        if t == "floor" and k + 1 < len(toks) and toks[k + 1][1] == "(":
            # floor( a / b )  don gian
            seg = lines[ln - 1] if ln - 1 < len(lines) else ""
            if re.search(r"floor\s*\(\s*[\w.\[\]]+\s*/\s*[\w.\[\]]+\s*\)", seg): res["floor_div"] += 1
        if t in ("newtag", "settag", "settagmethod", "gettagmethod", "copytagmethods"): res["tag"] += 1
        # gan bien toan cuc o muc ngoai ham:  X = ...  (khong local, khong nam trong function)
        if t == "=" and k >= 1 and not any(s[0] == "function" for s in stack):
            prev = toks[k - 1][1]
            if re.match(r"[A-Za-z_]\w*$", prev) and (k < 2 or toks[k - 2][1] not in (".", ":", "local", ",", "[", "]")) and (k + 1 < len(toks) and toks[k + 1][1] != "="):
                res["gassign"].add(prev)
    for s in strs:
        for w in re.findall(r"[A-Za-z_][A-Za-z0-9_]*", s):
            res["str_words"].add(w)
    res["gassign"] = sorted(res["gassign"]); res["idents"] = None; res["str_words"] = None
    return res, idents, res["str_words"] or set(), set(w for s in strs for w in re.findall(r"[A-Za-z_][A-Za-z0-9_]*", s))

files = {}
ident_index = collections.defaultdict(set)   # ident -> set(files) (code)
strword_index = collections.defaultdict(set) # word in strings -> set(files)
for top in ("script", "scriptjx2"):
    for dp, dn, fn in os.walk(os.path.join(ROOT, top)):
        for f in fn:
            if not f.lower().endswith(".lua"): continue
            p = os.path.join(dp, f)
            rel = os.path.relpath(p, ROOT).replace(os.sep, "/")
            try:
                src = open(p, "rb").read().decode("latin-1")
            except Exception:
                continue
            try:
                res, idents, _, strw = analyze(p, src)
            except Exception as e:
                print("LOI", rel, e); continue
            files[rel] = res
            for w in idents: ident_index[w].add(rel)
            for w in strw: strword_index[w].add(rel)
# tu C++: moi chuoi trong Core/Src -> tap tu (ham duoc C++ goi theo ten)
cpp_words = set()
for dp, dn, fn in os.walk(SRC_CPP):
    for f in fn:
        if not (f.endswith(".cpp") or f.endswith(".h")): continue
        try: t = open(os.path.join(dp, f), "rb").read().decode("latin-1")
        except Exception: continue
        for s in re.findall(r'"([^"\n]{1,120})"', t):
            for w in re.findall(r"[A-Za-z_][A-Za-z0-9_]{2,}", s): cpp_words.add(w)
# tu settings (ten ham trong bang du lieu)
set_words = set()
for dp, dn, fn in os.walk(os.path.join(ROOT, "settings")):
    for f in fn:
        if not f.lower().endswith((".txt", ".ini")): continue
        try: t = open(os.path.join(dp, f), "rb").read().decode("latin-1")
        except Exception: continue
        if len(t) > 4_000_000: continue
        for w in re.findall(r"[A-Za-z_][A-Za-z0-9_]{2,}", t): set_words.add(w)

# ---------------------------------------------------------------- tong hop
tot = collections.Counter()
rows = []
private_total = 0
for rel, r in files.items():
    tot["loc"] += r["loc"]; tot["concat_loop"] += len(r["concat_loop"]); tot["getn_while"] += len(r["getn_while"])
    tot["strfind_loop"] += len(r["strfind_loop"]); tot["dostring"] += len(r["dostring"]); tot["call"] += len(r["call"])
    tot["floor_div"] += r["floor_div"]; tot["gdefs"] += len(r["gdefs"]); tot["methods"] += r["methods"]; tot["tag"] += r["tag"]
    tot["longfn"] += len(r["longfn"]); tot["gassign"] += len(r["gassign"])
    # ham toan cuc chi dung trong tep nay (khong tep khac, khong chuoi, khong C++/settings, khong ten engine)
    priv = []
    for nm, ln in r["gdefs"]:
        if nm in ENGINE_NAMES or nm.startswith("On") and nm[2:3].isupper(): continue
        others = (ident_index.get(nm, set()) | strword_index.get(nm, set())) - {rel}
        if others: continue
        if nm in cpp_words or nm in set_words: continue
        if nm in strword_index and rel in strword_index[nm]:   # goi theo ten trong chuoi cung tep (Say callback) -> giu global
            continue
        priv.append((nm, ln))
    r["private_fn"] = priv
    private_total += len(priv)
    # bien toan cuc muc tep chi dung noi bo
    gv = [v for v in r["gassign"] if not ((ident_index.get(v, set()) | strword_index.get(v, set())) - {rel}) and v not in cpp_words and v not in set_words and v not in ENGINE_NAMES]
    r["private_var"] = gv
    tot["private_var"] += len(gv)
    score = 3 * len(r["concat_loop"]) + 5 * len(r["getn_while"]) + 2 * len(r["strfind_loop"]) + 2 * len(r["dostring"]) + 1 * len(r["call"]) + 0.5 * len(priv)
    rows.append((score, rel, r))
rows.sort(key=lambda x: -x[0])

out = []
out.append("# Phan tich tinh: cho ap dung tinh nang Lua 5.4 trong cay script may chu (%d tep, %d dong)" % (len(files), tot["loc"]))
out.append("")
out.append("| Mau | Tong | Y nghia / tinh nang 5.4 |")
out.append("|---|---:|---|")
out.append("| `x = x .. y` TRONG vong lap | %d | table.concat (23x o 20k phan tu) |" % tot["concat_loop"])
out.append("| `while ... getn(` | %d | #t (O(1)) |" % tot["getn_while"])
out.append("| `strfind` trong vong lap (tach chuoi thu cong) | %d | string.gmatch / gsub |" % tot["strfind_loop"])
out.append("| `dostring(` | %d | load() 1 lan + cache |" % tot["dostring"])
out.append("| `call(` bat loi kieu Lua 4 | %d | pcall/xpcall (22x) |" % tot["call"])
out.append("| `floor(a/b)` | %d | a // b (4,4x) |" % tot["floor_div"])
out.append("| tag method | %d | metatable |" % tot["tag"])
out.append("| ham toan cuc dinh nghia | %d (phuong thuc A:B %d) | |" % (tot["gdefs"], tot["methods"]))
out.append("| **ham toan cuc CHI dung trong tep minh** (khong engine/C++/settings/chuoi goi ten) | **%d** | local function (1,3x + het va cham ten giua Include) |" % private_total)
out.append("| bien toan cuc muc tep chi dung noi bo | %d | local (2,9x doc trong vong lap; het ro ri trang thai) |" % tot["private_var"])
out.append("| ham > 200 dong | %d | tach ham |" % tot["longfn"])
out.append("")
out.append("## Top 40 tep theo diem co hoi (3*concat_loop + 5*getn_while + 2*strfind_loop + 2*dostring + call + 0.5*ham_noi_bo)")
out.append("| tep | dong | concat/vong | getn while | strfind vong | dostring | call | ham noi bo | bien noi bo | ham>200 |")
out.append("|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|")
for score, rel, r in rows[:40]:
    out.append("| %s | %d | %d | %d | %d | %d | %d | %d | %d | %d |" % (rel, r["loc"], len(r["concat_loop"]), len(r["getn_while"]), len(r["strfind_loop"]), len(r["dostring"]), len(r["call"]), len(r["private_fn"]), len(r["private_var"]), len(r["longfn"])))
out.append("")
out.append("## Chi tiet: x = x .. trong vong lap (tep: dong)")
for score, rel, r in rows:
    if r["concat_loop"]:
        out.append("- %s: %s" % (rel, ", ".join(str(x) for x in r["concat_loop"][:12]) + (" ..." if len(r["concat_loop"]) > 12 else "")))
out.append("")
out.append("## Chi tiet: while getn")
for score, rel, r in rows:
    if r["getn_while"]: out.append("- %s: %s" % (rel, ", ".join(str(x) for x in r["getn_while"])))
out.append("")
out.append("## Chi tiet: strfind trong vong lap (tach chuoi)")
for score, rel, r in rows:
    if r["strfind_loop"]: out.append("- %s: %s" % (rel, ", ".join(str(x) for x in r["strfind_loop"][:10])))
out.append("")
out.append("## Chi tiet: dostring")
for score, rel, r in rows:
    if r["dostring"]: out.append("- %s: %s" % (rel, ", ".join(str(x) for x in r["dostring"])))
out.append("")
out.append("## Chi tiet: call(")
for score, rel, r in rows:
    if r["call"]: out.append("- %s: %s" % (rel, ", ".join(str(x) for x in r["call"][:10])))
out.append("")
out.append("## Top 30 tep co nhieu ham chi dung noi bo (ung vien `local function`)")
for score, rel, r in sorted(rows, key=lambda x: -len(x[2]["private_fn"]))[:30]:
    if r["private_fn"]:
        out.append("- %s: %d/%d ham (vd %s)" % (rel, len(r["private_fn"]), len(r["gdefs"]), ", ".join(n for n, l in r["private_fn"][:6])))
out.append("")
out.append("## Ham > 200 dong")
for score, rel, r in rows:
    for l0, ln in r["longfn"]: out.append("- %s: dong %d (%d dong)" % (rel, l0, ln))
open(os.path.join(OUT, "PTICH_54_baocao.md"), "w", encoding="utf-8").write("\n".join(out))
json.dump({rel: {k: v for k, v in r.items() if k not in ("idents", "str_words")} for rel, r in files.items()}, open(os.path.join(OUT, "PTICH_54_data.json"), "w", encoding="utf-8"), ensure_ascii=True, default=list)
print("\n".join(out[:16]))
print("... tong tep:", len(files))
