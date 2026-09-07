r"""local_hoa2.py - PA-2 lan 2 (luat chat hon sau su co WLLS_HEAD): CHI local hoa HAM toan cuc chi dung trong chinh tep,
KHONG local hoa bien (bien cap tep co the giu trang thai qua cac lan Include: X = X or {}, if not X_HEAD then ...).
Them dieu kien cho ham: lan xuat hien dau tien o cap tep phai la dinh nghia `function Ten(`; ten khong xuat hien trong
bieu thuc dieu kien cap 0 (if/while/or/and/not/==/~=) va khong duoc gan kieu `Ten = ...` o bat ky dau (chi dinh nghia bang function).
Cac loai tru khac nhu local_hoa.py (thu muc, goi ten dong, chuoi, tep khac nhac toi, khong ai goi, gioi han 180).
  python local_hoa2.py            -> ke hoach
  python local_hoa2.py sua        -> ghi (latin-1, CRLF); ghi local_hoa2_ketqua.txt
"""
import os, sys, re, json, io, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
HERE = os.path.dirname(os.path.abspath(__file__))
JSON = os.path.join(HERE, "PTICH_54_data.json")
SUA = len(sys.argv) > 1 and sys.argv[1] == "sua"
MAX_LOCAL = 180
LOAI_DIR = ("scriptjx2\\", "script\\tinhnang\\activitysys\\", "script\\script_protocol\\", "script\\kiemthu\\", "script\\lib\\",
            "script\\cauhinh_web\\", "script\\bando\\", "script\\cauhinh\\", "script\\header\\")
RX_DONG = re.compile(r"\b(getglobal|setglobal|rawgetglobal|rawsetglobal|dostring|load|loadstring|globals|foreachvar|nextvar|setfenv|getfenv)\s*\(|\b_G\b|\b_ENV\b|\bdebug\s*\.")
ENGINE = {"main", "OnCreate", "OnDeath", "OnTimer", "OnCall", "OnRevive", "OnEnter", "OnLeave", "OnPlayerTimerIdle", "OnPlayerKillNpc", "OnPlayerDeath",
          "trademoney", "tradeitem", "linebegin", "PickItem", "write_log_tax", "win", "thaycai", "tg_quit", "go", "creattong", "cancel", "PermitSuperShop"}

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()

def tach(src):
    code = []; khac = []
    i = 0; n = len(src)
    while i < n:
        c = src[i]
        if c == "-" and src.startswith("--", i):
            j = i + 2
            m = re.match(r"\[(=*)\[", src[j:j + 10])
            if m:
                end = src.find("]" + m.group(1) + "]", j + len(m.group(0)))
                end = n if end < 0 else end + len(m.group(1)) + 2
                khac.append(src[i:end]); code.append(re.sub(r"[^\n]", " ", src[i:end])); i = end; continue
            end = src.find("\n", j); end = n if end < 0 else end
            khac.append(src[i:end]); code.append(" " * (end - i)); i = end; continue
        if c == '"' or c == "'":
            j = i + 1
            while j < n and src[j] != c:
                if src[j] == "\\": j += 1
                j += 1
            j = min(j + 1, n)
            khac.append(src[i + 1:j - 1]); code.append('""'.ljust(j - i)); i = j; continue
        m = re.match(r"\[(=*)\[", src[i:i + 10]) if c == "[" else None
        if m:
            end = src.find("]" + m.group(1) + "]", i + len(m.group(0)))
            end = n if end < 0 else end + len(m.group(1)) + 2
            khac.append(src[i + len(m.group(0)):end]); code.append(re.sub(r"[^\n]", " ", src[i:end])); i = end; continue
        code.append(c); i += 1
    return "".join(code), "\n".join(khac)

def cap0_lines(code):
    """cac dong o cap 0 (ngoai moi function/if/for/while/do) - do sau tinh tho theo tu khoa"""
    out = []; depth = 0
    for ln in code.split("\n"):
        s = ln.strip()
        if depth == 0 and s: out.append(s)
        for t in re.findall(r"\b(function|if|for|while|do|repeat|end|until)\b", s):
            if t in ("function", "if", "for", "while", "repeat"): depth += 1
            elif t == "do":
                if not re.search(r"\b(for|while)\b", s): depth += 1
            elif t in ("end", "until"): depth -= 1
    return out

d = json.load(open(JSON, encoding="utf-8"))
print("Lap chi muc ten...")
mention = collections.defaultdict(set); files = {}
for top in ("script", "scriptjx2"):
    for dp, dn, fn in os.walk(os.path.join(ROOT, top)):
        for f in fn:
            if not f.lower().endswith((".lua", ".txt")): continue
            p = os.path.join(dp, f); rel = os.path.relpath(p, ROOT)
            try: s = rd(p)
            except Exception: continue
            files[rel] = s
            for w in set(re.findall(r"[A-Za-z_][A-Za-z0-9_]*", s)): mention[w].add(rel)
for dp, dn, fn in os.walk(os.path.join(ROOT, "settings")):
    for f in fn:
        p = os.path.join(dp, f)
        try:
            if os.path.getsize(p) > 6 * 1048576: continue
            s = rd(p)
        except Exception: continue
        for w in set(re.findall(r"[A-Za-z_][A-Za-z0-9_]*", s)): mention[w].add("settings")

ke_hoach = []; tk = collections.Counter()
for key, v in d.items():
    rel = key.replace("/", "\\"); low = rel.lower()
    if any(low.startswith(x) for x in LOAI_DIR): tk["loai_thu_muc"] += 1; continue
    if rel not in files: tk["khong_con"] += 1; continue
    src = files[rel]
    if src.lstrip().startswith("-- [LOCAL54"): tk["da_lam"] += 1; continue
    code, khac = tach(src)
    if RX_DONG.search(code): tk["goi_ten_dong"] += 1; continue
    khac_words = set(re.findall(r"[A-Za-z_][A-Za-z0-9_]*", khac))
    c0 = cap0_lines(code)
    ung = []
    for nm, ln in v.get("private_fn", []):
        if nm in ENGINE: continue
        if nm in khac_words: tk["trong_chuoi"] += 1; continue
        if mention.get(nm, set()) - {rel}: tk["tep_khac_nhac"] += 1; continue
        n_ref = len(re.findall(r"\b" + re.escape(nm) + r"\b", code))
        if n_ref < 2: tk["khong_ai_goi"] += 1; continue
        # khong duoc gan kieu 'nm =' (chi dinh nghia bang function) va khong nam trong bieu thuc dieu kien cap 0
        if re.search(r"(^|[^A-Za-z0-9_.:])" + re.escape(nm) + r"\s*=[^=]", code): tk["bi_gan"] += 1; continue
        dau = None
        for s in c0:
            if re.search(r"\b" + re.escape(nm) + r"\b", s):
                dau = s; break
        if dau is None or not re.match(r"^function\s+" + re.escape(nm) + r"\s*\(", dau):
            tk["doc_truoc_dinh_nghia"] += 1; continue
        if any(re.search(r"\b(if|while|elseif|until)\b.*\b" + re.escape(nm) + r"\b", s) or re.search(r"\b" + re.escape(nm) + r"\b.*\b(or|and)\b|\b(or|and|not)\b.*\b" + re.escape(nm) + r"\b", s) for s in c0):
            tk["dieu_kien_cap0"] += 1; continue
        ung.append(nm)
    n_local_cu = len(re.findall(r"^\s*local\s", code, re.M))
    if len(ung) + n_local_cu > MAX_LOCAL:
        ung = ung[:max(0, MAX_LOCAL - n_local_cu)]; tk["cat_gioi_han"] += 1
    if not ung: tk["khong_co"] += 1; continue
    ke_hoach.append((rel, ung))
n_ten = sum(len(u) for _, u in ke_hoach)
print("Ke hoach: %d tep, %d ham; thong ke bo: %s" % (len(ke_hoach), n_ten, dict(tk)))
for rel, ung in sorted(ke_hoach, key=lambda x: -len(x[1]))[:10]:
    print("   %-60s %3d  %s" % (rel[:60], len(ung), ",".join(ung[:5])))
if not SUA: sys.exit(0)
out = []
for rel, ung in ke_hoach:
    p = os.path.join(ROOT, rel); src = files[rel]
    nl = "\r\n" if "\r\n" in src else "\n"
    lines = ["local " + ", ".join(ung[i:i + 25]) for i in range(0, len(ung), 25)]
    header = ("-- [LOCAL54 06/09 toi] %d ham chi dung trong tep nay -> local (PA-2 tu dong, local_hoa2.py); bo dong nay va dong local duoi = ve toan cuc" % len(ung)) + nl + nl.join(lines) + nl
    bom = ""
    if src.startswith("\xef\xbb\xbf"): bom = src[:3]; src = src[3:]
    io.open(p, "w", encoding="latin-1", newline="").write(bom + header + src)
    out.append("%s\t%s" % (rel, ",".join(ung)))
io.open(os.path.join(HERE, "local_hoa2_ketqua.txt"), "w", encoding="utf-8").write("\n".join(out) + "\n")
print("DA GHI %d tep" % len(ke_hoach))
