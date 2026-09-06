import os, re, sys, hashlib, collections
ROOT = "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server"
SRC = "D:/GAMEDEVNEW/Sources"
OUTD = sys.argv[1]
BS = chr(92)

def to_key(p_unicode):
    # duong dan tren dia (unicode) -> chuoi byte kieu latin-1 (GBK cho ten Han) de so voi token trong tep du lieu
    s = p_unicode.replace(os.sep, "/")
    try:
        b = s.encode("gbk")
    except Exception:
        b = s.encode("cp1252", errors="replace")
    return b.decode("latin-1").lower()

files = {}   # key -> dict(size, real, sha1, lines, top)
for top in ("script", "scriptjx2"):
    for dp, dn, fn in os.walk(os.path.join(ROOT, top)):
        for f in fn:
            if not (f.lower().endswith(".lua") or f.lower().endswith(".txt")):
                continue
            real = os.path.join(dp, f)
            rel = os.path.relpath(real, ROOT)
            key = to_key(rel)
            try:
                data = open(real, "rb").read()
            except Exception:
                continue
            files[key] = dict(size=len(data), real=real, sha1=hashlib.sha1(data).hexdigest(), lines=data.count(b"\n"), top=key.split("/")[0], data=data, unicode=rel.replace(os.sep, "/"))

def norm_token(t):
    t = t.replace(BS + BS, BS).replace(BS, "/").lower().strip()
    while t.startswith("/"):
        t = t[1:]
    if t.startswith("./"):
        t = t[2:]
    i = t.find("script/")
    j = t.find("scriptjx2/")
    if j >= 0 and (i < 0 or j <= i):
        t = t[j:]
    elif i >= 0:
        t = t[i:]
    return t

def resolve(t):
    if t in files:
        return t
    if t.startswith("script/lib/"):
        q = "scriptjx2/lib/" + t[len("script/lib/"):]
        if q in files: return q
    if t.startswith("script/tong/"):
        q = "scriptjx2/tong_vn/" + t[len("script/tong/"):]
        if q in files: return q
    return None

refs = collections.defaultdict(set)      # key -> set(nguon)
unresolved = collections.Counter()
rx_lua = re.compile(r'([A-Za-z0-9_\-\.\x80-\xff/\\]+\.lua)', re.I)

# (a) tham chieu tu script (moi literal .lua)
for key, info in files.items():
    txt = info["data"].decode("latin-1")
    for m in rx_lua.finditer(txt):
        t = norm_token(m.group(1))
        if not (t.startswith("script/") or t.startswith("scriptjx2/")):
            continue
        r = resolve(t)
        if r:
            if r != key: refs[r].add("script")
        else:
            unresolved[t] += 1

# (b) settings + Maps + data (tep van ban)
def scan_dir(d, tag, maxmb=8):
    n = 0
    for dp, dn, fn in os.walk(os.path.join(ROOT, d)):
        for f in fn:
            real = os.path.join(dp, f)
            try:
                if os.path.getsize(real) > maxmb * 1048576:
                    continue
                data = open(real, "rb").read()
            except Exception:
                continue
            if b"\x00" in data[:4096]:
                continue
            if b".lua" not in data.lower():
                continue
            n += 1
            txt = data.decode("latin-1")
            for m in rx_lua.finditer(txt):
                t = norm_token(m.group(1))
                if not (t.startswith("script/") or t.startswith("scriptjx2/")):
                    # duong dan tuong doi khong co 'script/' -> thu ghep
                    t2 = "script/" + t
                    if t2 in files:
                        t = t2
                    else:
                        continue
                r = resolve(t)
                if r:
                    refs[r].add(tag)
                else:
                    unresolved[t] += 1
    return n
n_set = scan_dir("settings", "settings")
n_map = scan_dir("Maps", "maps")
n_dul = scan_dir("dulieu", "dulieu")

# (c) C++ literal
rx_cpp = re.compile(r'([A-Za-z0-9_\-\.\\/]+\.lua)', re.I)
n_cpp_files = 0
for dp, dn, fn in os.walk(SRC):
    if "Library" in dp or "x64" in dp or "Release" in dp or "Debug" in dp:
        continue
    for f in fn:
        if not (f.endswith(".cpp") or f.endswith(".h")):
            continue
        real = os.path.join(dp, f)
        try:
            data = open(real, "rb").read()
        except Exception:
            continue
        if b".lua" not in data.lower():
            continue
        n_cpp_files += 1
        txt = data.decode("latin-1")
        for m in rx_cpp.finditer(txt):
            t = norm_token(m.group(1))
            if not (t.startswith("script/") or t.startswith("scriptjx2/")):
                continue
            r = resolve(t)
            if r:
                refs[r].add("cpp")
            else:
                unresolved[t] += 1

# ---- thong ke
out = []
tot = len(files); tot_sz = sum(f["size"] for f in files.values())
out.append("# Kiem ke cay script live (bin/server/script + scriptjx2) - %d tep, %.1f MB" % (tot, tot_sz / 1048576))
out.append("Nguon tham chieu da quet: script literal; settings (%d tep co .lua); Maps (%d); dulieu (%d); C++ (%d tep)" % (n_set, n_map, n_dul, n_cpp_files))
out.append("")
by_top = collections.defaultdict(lambda: dict(n=0, sz=0, ref=0, unref=0, gbk=0))
gbk_dirs = set()
for key, info in files.items():
    parts = key.split("/")
    top2 = "/".join(parts[:2]) if len(parts) > 2 else parts[0]
    d = by_top[top2]
    d["n"] += 1; d["sz"] += info["size"]
    if key in refs: d["ref"] += 1
    else: d["unref"] += 1
    if any(ord(c) > 127 for c in key):
        d["gbk"] += 1
        gbk_dirs.add("/".join(parts[:2]))
out.append("## 1. Thu muc cap 2: tep | KB | co tham chieu | KHONG tham chieu (ngoai autoload) | ten Han")
out.append("| thu muc | tep | KB | ref | khong ref | ten GBK |")
out.append("|---|---:|---:|---:|---:|---:|")
for k, d in sorted(by_top.items(), key=lambda kv: -kv[1]["n"]):
    out.append("| %s | %d | %.0f | %d | %d | %d |" % (k, d["n"], d["sz"] / 1024, d["ref"], d["unref"], d["gbk"]))
out.append("")
# tep khong tham chieu theo nhom
unref = [k for k in files if k not in refs]
out.append("## 2. Tep KHONG duoc tham chieu tu dau (chi duoc engine tu nap luc boot): %d / %d tep, %.1f MB" % (
    len(unref), tot, sum(files[k]["size"] for k in unref) / 1048576))
grp = collections.defaultdict(list)
for k in unref:
    parts = k.split("/")
    grp["/".join(parts[:2]) if len(parts) > 2 else parts[0]].append(k)
for g, lst in sorted(grp.items(), key=lambda kv: -len(kv[1]))[:40]:
    out.append("- `%s`: %d tep (vd %s)" % (g, len(lst), ", ".join(os.path.basename(x) for x in sorted(lst)[:4])))
out.append("")
# tep test/admin/backup
rx_bad = re.compile(r'(test|_admin|admin_|backup|_cu\b|_old|\.bak|truoc|copy|tmp|\bthu\b|debug)', re.I)
bad = [k for k in files if rx_bad.search(k)]
out.append("## 3. Tep ten kieu test/admin/backup dang nam trong cay chay that: %d" % len(bad))
for k in sorted(bad)[:60]:
    out.append("- `%s` %s" % (k, "(ref)" if k in refs else ""))
out.append("")
# trung noi dung
byhash = collections.defaultdict(list)
for k, f in files.items():
    byhash[f["sha1"]].append(k)
dups = [v for v in byhash.values() if len(v) > 1]
out.append("## 4. Tep TRUNG NOI DUNG (cung sha1, khac duong dan): %d nhom, %d tep thua, %.1f KB" % (
    len(dups), sum(len(v) - 1 for v in dups), sum((len(v) - 1) * files[v[0]]["size"] for v in dups) / 1024))
for v in sorted(dups, key=lambda v: -files[v[0]]["size"])[:25]:
    out.append("- %.1f KB x%d: %s" % (files[v[0]]["size"] / 1024, len(v), " | ".join(sorted(v))))
out.append("")
# script/lib vs scriptjx2/lib bong (cung ten)
shadow = []
for k in files:
    if k.startswith("scriptjx2/lib/"):
        q = "script/lib/" + k[len("scriptjx2/lib/"):]
        if q in files:
            shadow.append((q, k, files[q]["sha1"] == files[k]["sha1"]))
out.append("## 5. script/lib va scriptjx2/lib cung ten (remap chi dung khi script/lib THIEU): %d cap, giong het: %d" % (len(shadow), sum(1 for s in shadow if s[2])))
for q, k, same in sorted(shadow)[:30]:
    out.append("- %s <-> %s %s" % (q, k, "GIONG" if same else "KHAC"))
out.append("")
# tep to
big = sorted(files.items(), key=lambda kv: -kv[1]["lines"])[:15]
out.append("## 6. Tep dai nhat (dong)")
for k, f in big:
    out.append("- %d dong, %.0f KB: `%s` %s" % (f["lines"], f["size"] / 1024, k, "(ref)" if k in refs else ""))
out.append("")
out.append("## 7. Thu muc ten Han (GBK) - %d thu muc, %d tep" % (len(gbk_dirs), sum(d["gbk"] for d in by_top.values())))
for g in sorted(gbk_dirs):
    try:
        u = g.encode("latin-1").decode("gbk")
    except Exception:
        u = "?"
    out.append("- `%s` = %s" % (g, u))
out.append("")
out.append("## 8. Tham chieu toi tep KHONG ton tai (top 30, tu moi nguon)")
for t, n in unresolved.most_common(30):
    out.append("- x%d `%s`" % (n, t))
out.append("")
# nguon tham chieu theo loai
cnt = collections.Counter()
for k, s in refs.items():
    for tag in s: cnt[tag] += 1
out.append("## 9. So tep duoc tham chieu theo nguon: " + ", ".join("%s=%d" % kv for kv in cnt.most_common()))
txt = "\n".join(out)
open(os.path.join(OUTD, "saplai_baocao.md"), "w", encoding="utf-8").write(txt)
print(txt[:12000])
