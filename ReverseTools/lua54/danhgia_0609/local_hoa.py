r"""local_hoa.py - PA-2: bien ham/bien TOAN CUC chi dung trong chinh tep thanh `local` (khai bao mot dong o dau tep).
Du lieu: PTICH_54_data.json (private_fn / private_var / gdefs cua ptich_54.py) + kiem lai tren cay hien tai:
  - tep bi loai: thu muc scriptjx2/, tinhnang/activitysys/, script_protocol/, kiemthu/, lib/ (thu vien dung chung),
    va tep co goi ten dong: getglobal/setglobal/rawgetglobal/rawsetglobal/dostring/load/loadstring/globals/foreachvar/nextvar/_G/_ENV/setfenv/getfenv/debug.
  - ten bi loai: xuat hien trong CHUOI hoac chu thich cua chinh tep (callback theo ten), duoc tep KHAC nhac toi (ke ca trong chuoi),
    khong co tham chieu nao ngoai dinh nghia (ham chet / co the goi dong), ten trong danh sach C++ / settings (da loai o ptich_54).
  - moi tep toi da 180 ten (gioi han 200 local cua mot ham Lua).
  python local_hoa.py            -> ke hoach (khong doi)
  python local_hoa.py sua        -> ghi (giu CRLF/byte goc: doc/ghi latin-1)
  python local_hoa.py sua <thu muc con>  -> chi ap dung trong thu muc (vd nhanvat)
Ghi: local_hoa_ketqua.txt (tep -> ten da local hoa)
"""
import os, sys, re, json, io, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
JSON = os.path.join(os.path.dirname(os.path.abspath(__file__)), "PTICH_54_data.json")
SUA = len(sys.argv) > 1 and sys.argv[1] == "sua"
CHI = sys.argv[2].replace("/", "\\").lower() if len(sys.argv) > 2 else None
MAX_LOCAL = 180
LOAI_DIR = ("scriptjx2\\", "script\\tinhnang\\activitysys\\", "script\\script_protocol\\", "script\\kiemthu\\", "script\\lib\\",
            "script\\cauhinh_web\\", "script\\bando\\")
RX_DONG = re.compile(r"\b(getglobal|setglobal|rawgetglobal|rawsetglobal|dostring|load|loadstring|globals|foreachvar|nextvar|setfenv|getfenv)\s*\(|\b_G\b|\b_ENV\b|\bdebug\s*\.")

def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()

def tach(src):
    """-> (code khong chuoi/chu thich, noi dung chuoi + chu thich) - tach tho theo Lua"""
    code = []; khac = []
    i = 0; n = len(src)
    while i < n:
        c = src[i]
        if c == "-" and src.startswith("--", i):
            j = i + 2
            if src.startswith("[", j):
                m = re.match(r"\[(=*)\[", src[j:])
                if m:
                    end = src.find("]" + m.group(1) + "]", j + len(m.group(0)))
                    end = n if end < 0 else end + len(m.group(1)) + 2
                    khac.append(src[i:end]); code.append(" " * (end - i)); i = end; continue
            end = src.find("\n", j)
            end = n if end < 0 else end
            khac.append(src[i:end]); code.append(" " * (end - i)); i = end; continue
        if c == '"' or c == "'":
            j = i + 1
            while j < n and src[j] != c:
                if src[j] == "\\": j += 1
                j += 1
            j = min(j + 1, n)
            khac.append(src[i + 1:j - 1]); code.append('""'.ljust(j - i)); i = j; continue
        if c == "[" and re.match(r"\[=*\[", src[i:]):
            m = re.match(r"\[(=*)\[", src[i:])
            end = src.find("]" + m.group(1) + "]", i + len(m.group(0)))
            end = n if end < 0 else end + len(m.group(1)) + 2
            khac.append(src[i + len(m.group(0)):end - len(m.group(1)) - 2]); code.append(" " * (end - i)); i = end; continue
        code.append(c); i += 1
    return "".join(code), "\n".join(khac)

d = json.load(open(JSON, encoding="utf-8"))
# --- chi muc: ten -> tap tep nhac toi (moi noi: code, chuoi, chu thich) tren CAY HIEN TAI (script + scriptjx2 + settings)
print("Lap chi muc ten tren cay hien tai...")
mention = collections.defaultdict(set)
files = {}
for top in ("script", "scriptjx2"):
    for dp, dn, fn in os.walk(os.path.join(ROOT, top)):
        for f in fn:
            if not f.lower().endswith((".lua", ".txt")): continue
            p = os.path.join(dp, f)
            rel = os.path.relpath(p, ROOT)
            try: s = rd(p)
            except Exception: continue
            files[rel] = s
            for w in set(re.findall(r"[A-Za-z_][A-Za-z0-9_]*", s)):
                mention[w].add(rel)
for dp, dn, fn in os.walk(os.path.join(ROOT, "settings")):
    for f in fn:
        p = os.path.join(dp, f)
        try:
            if os.path.getsize(p) > 6 * 1048576: continue
            s = rd(p)
        except Exception: continue
        for w in set(re.findall(r"[A-Za-z_][A-Za-z0-9_]*", s)):
            mention[w].add("settings")
print("  %d tep, %d ten" % (len(files), len(mention)))

ke_hoach = []      # (rel, [ten...], ly do bo)
tk = collections.Counter()
for key, v in d.items():
    rel = key.replace("/", "\\")
    low = rel.lower()
    if CHI and not low.startswith(("script\\" + CHI + "\\").lower()) and low != ("script\\" + CHI).lower(): continue
    if any(low.startswith(x) for x in LOAI_DIR): tk["loai_thu_muc"] += 1; continue
    if rel not in files:                     # tep da doi cho / khong con
        tk["khong_con"] += 1; continue
    src = files[rel]
    code, khac = tach(src)
    if RX_DONG.search(code): tk["goi_ten_dong"] += 1; continue
    ung = []
    khac_words = set(re.findall(r"[A-Za-z_][A-Za-z0-9_]*", khac))
    cand_fn = [nm for nm, ln in v.get("private_fn", [])]
    cand_var = list(v.get("private_var", []))
    for nm in cand_fn + cand_var:
        if nm in khac_words: tk["trong_chuoi"] += 1; continue
        others = mention.get(nm, set()) - {rel}
        if others: tk["tep_khac_nhac"] += 1; continue
        n_ref = len(re.findall(r"\b" + re.escape(nm) + r"\b", code))
        if n_ref < 2: tk["khong_ai_goi"] += 1; continue     # chi co dinh nghia -> co the goi dong / chet: de yen
        if nm in ("main", "OnCreate", "OnDeath", "OnTimer", "OnCall", "OnRevive", "OnEnter", "OnLeave"): continue
        ung.append(nm)
    # gioi han local
    n_local_cu = len(re.findall(r"^\s*local\s", code, re.M))
    if len(ung) + n_local_cu > MAX_LOCAL:
        ung = ung[:max(0, MAX_LOCAL - n_local_cu)]
        tk["cat_gioi_han"] += 1
    if not ung: tk["khong_co"] += 1; continue
    ke_hoach.append((rel, ung))
n_ten = sum(len(u) for _, u in ke_hoach)
print("Ke hoach: %d tep, %d ten; thong ke bo: %s" % (len(ke_hoach), n_ten, dict(tk)))
for rel, ung in sorted(ke_hoach, key=lambda x: -len(x[1]))[:12]:
    print("   %-60s %3d  %s" % (rel[:60], len(ung), ",".join(ung[:6])))
if not SUA:
    sys.exit(0)
out = []
for rel, ung in ke_hoach:
    p = os.path.join(ROOT, rel)
    src = files[rel]
    nl = "\r\n" if "\r\n" in src else "\n"
    lines = []
    for i in range(0, len(ung), 25):
        lines.append("local " + ", ".join(ung[i:i + 25]))
    header = ("-- [LOCAL54 06/09 toi] %d ham/bien chi dung trong tep nay -> local (PA-2 tu dong, local_hoa.py); bo dong nay = ve toan cuc" % len(ung)) + nl + nl.join(lines) + nl
    bom = ""
    if src.startswith("\xef\xbb\xbf"):
        bom = src[:3]; src = src[3:]
    io.open(p, "w", encoding="latin-1", newline="").write(bom + header + src)
    out.append("%s\t%s" % (rel, ",".join(ung)))
io.open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "local_hoa_ketqua.txt"), "a", encoding="utf-8").write("\n".join(out) + "\n")
print("DA GHI %d tep" % len(ke_hoach))
