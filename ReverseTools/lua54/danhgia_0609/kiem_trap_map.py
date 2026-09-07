r"""kiem_trap_map.py - doi chieu ID trap + ten script NPC trong Maps\map_publish\**\NNN_Region_S.dat
   voi cay script hien tai (+ bi danh _duongdan_cu.txt) va kho luu tru disan_jx / gương git truoc sap xep.
   python kiem_trap_map.py            -> bao cao
   python kiem_trap_map.py sua        -> chuyen tep con thieu tu disan_jx ve DUNG duong dan cu trong cay chay
"""
import os, sys, struct, subprocess, collections, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
DISAN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\_luutru\0609\disan_jx"
MIRROR = r"D:\GAMEDEVNEW\serverscript_live"
MIRROR_COMMIT = "84675cc3"
SUA = len(sys.argv) > 1 and sys.argv[1] == "sua"
BS = chr(92)

def ansi_bytes(s):
    out = bytearray()
    for ch in s:
        o = ord(ch)
        if o < 128 or o in (0x81, 0x8D, 0x8F, 0x90, 0x9D): out.append(o)
        else:
            try: out += ch.encode("cp1252")
            except UnicodeEncodeError: out += b"?"
    return bytes(out)

def lower_bytes(b):
    return bytes([c + 32 if 65 <= c <= 90 else c for c in b])

def key_of(rel_unicode):
    """duong dan tuong doi goc may chu (unicode cp1252-decoded) -> bytes '\\script\\...' chu thuong"""
    b = ansi_bytes(rel_unicode.replace("/", BS))
    b = lower_bytes(b).lstrip(BS.encode())
    return BS.encode() + b

def fid(b):
    Id = 0
    for i, ch in enumerate(b):
        c = ch if ch < 128 else ch - 256
        Id = ((Id + (i + 1) * c) & 0xFFFFFFFF) % 0x8000000B
        Id = (Id * 0xFFFFFFEF) & 0xFFFFFFFF
    return Id ^ 0x12345678

def norm_script_name(b):
    """ten script trong du lieu map (bytes) -> key chuan '\\script\\...' chu thuong"""
    b = b.replace(b"/", BS.encode())
    b = lower_bytes(b)
    while b.startswith(b".\\"): b = b[2:]
    b = b.lstrip(BS.encode())
    return BS.encode() + b

# ---------- 1) cay script hien tai: ID cua moi tep .lua/.txt trong \script + \scriptjx2\tong_vn ----------
live = {}   # key bytes -> rel path (unicode)
for sub in ("script", os.path.join("scriptjx2", "tong_vn")):
    base = os.path.join(ROOT, sub)
    for dp, dn, fn in os.walk(base):
        dn[:] = [d for d in dn if not d.startswith("_")]
        for f in fn:
            if not f.lower().endswith((".lua", ".txt")): continue
            if f.startswith("_"): continue
            rel = os.path.relpath(os.path.join(dp, f), ROOT)
            live[key_of(rel)] = rel
live_ids = {fid(k): k for k in live}
print("Cay chay: %d tep script (ID %d)" % (len(live), len(live_ids)))

# ---------- 2) bi danh ----------
alias = {}  # key cu -> key moi
with open(os.path.join(ROOT, "script", "_duongdan_cu.txt"), "rb") as f:
    for line in f:
        if not line.startswith(b"--@"): continue
        body = line[3:].strip()
        if b"=" not in body: continue
        cu, moi = body.split(b"=", 1)
        alias[norm_script_name(cu.strip())] = norm_script_name(moi.strip())
alias_ids = {}
n_alias_ok = 0
for cu, moi in alias.items():
    if moi in live:
        alias_ids[fid(cu)] = cu; n_alias_ok += 1
print("Bi danh: %d dong, ten moi co trong cay: %d" % (len(alias), n_alias_ok))

# ---------- 3) kho luu tru disan_jx + guong git truoc sap xep ----------
disan = {}
for dp, dn, fn in os.walk(DISAN):
    for f in fn:
        rel = os.path.relpath(os.path.join(dp, f), DISAN)
        disan[key_of(rel)] = rel
disan_ids = {fid(k): k for k in disan}
print("disan_jx: %d tep" % len(disan))
mirror = set()
try:
    out = subprocess.run(["git", "-c", "core.quotepath=false", "ls-tree", "-r", "--name-only", MIRROR_COMMIT],
                         cwd=MIRROR, capture_output=True).stdout.decode("utf-8", "replace")
    for line in out.splitlines():
        if line.lower().endswith((".lua", ".txt")):
            parts = line.split("/")
            idx = [i for i, c in enumerate(parts) if c.lower() in ("script", "scriptjx2")]
            if not idx: continue
            mirror.add(key_of("/".join(parts[idx[0]:])))
except Exception as e:
    print("khong doc duoc guong git:", e)
mirror_ids = {fid(k): k for k in mirror}
print("Guong git truoc sap xep (%s): %d tep script" % (MIRROR_COMMIT, len(mirror)))

# ---------- 4) MapList.ini: map nao dang nap ----------
maplist_dirs = set()
try:
    for line in open(os.path.join(ROOT, "settings", "MapList.ini"), "rb"):
        line = line.strip()
        if b"=" in line and line.split(b"=")[0].strip().isdigit():
            v = line.split(b"=", 1)[1].strip()
            maplist_dirs.add(lower_bytes(v.replace(b"/", BS.encode())))
except Exception as e:
    print("MapList:", e)
print("MapList.ini: %d ban do" % len(maplist_dirs))

# ---------- 5) quet Region_S.dat ----------
trap_ref = collections.defaultdict(set)     # trap id -> set(map dir)
npc_ref = collections.defaultdict(set)      # script key -> set(map dir)
n_files = 0; n_trap_cells = 0; n_npc = 0; n_bad = 0
maps_root = os.path.join(ROOT, "Maps")
for dp, dn, fn in os.walk(maps_root):
    for f in fn:
        if not f.lower().endswith("region_s.dat"): continue
        p = os.path.join(dp, f)
        # map dir = phan sau map_publish\ , bo v_XXX
        rel_map = os.path.relpath(dp, maps_root)
        rel_map = BS.join(rel_map.split(os.sep)[:-1])
        data = open(p, "rb").read()
        n_files += 1
        if len(data) < 4: continue
        cnt = struct.unpack_from("<I", data, 0)[0]
        if cnt < 2 or cnt > 64: n_bad += 1; continue
        head = 4 + 8 * cnt
        if len(data) < head: n_bad += 1; continue
        secs = [struct.unpack_from("<II", data, 4 + 8 * i) for i in range(cnt)]
        # trap
        off, ln = secs[1]
        if ln >= 12 and head + off + ln <= len(data):
            n = struct.unpack_from("<I", data, head + off)[0]
            if 12 + n * 8 == ln:
                for i in range(n):
                    cx, cy, ncell, res, tid = struct.unpack_from("<BBBBI", data, head + off + 12 + i * 8)
                    if tid: trap_ref[tid].add(rel_map); n_trap_cells += 1
        # npc
        if cnt > 2:
            off, ln = secs[2]
            if ln >= 12 and head + off + ln <= len(data):
                n = struct.unpack_from("<I", data, head + off)[0]
                pos = head + off + 12
                for i in range(n):
                    if pos + 60 > len(data): break
                    slen = struct.unpack_from("<H", data, pos + 58)[0]
                    name = data[pos + 60: pos + 60 + slen]
                    pos += 60 + slen
                    n_npc += 1
                    name = name.split(b"\0")[0].strip()
                    if name: npc_ref[norm_script_name(name)].add(rel_map)
print("Region_S.dat tren dia: %d tep (%d hong), o trap %d -> %d ID trap khac nhau; NPC %d -> %d ten script khac nhau"
      % (n_files, n_bad, n_trap_cells, len(trap_ref), n_npc, len(npc_ref)))
# ---------- 5b) quet Pak\*.pak (968/1021 ban do trong MapList KHONG co tren dia -> nam trong pak) ----------
import pak_region
print("Quet Pak:")
ptrap, pnpc, pstats = pak_region.scan_paks(os.path.join(ROOT, "Pak"))
for t, ms in ptrap.items(): trap_ref[t] |= ms
for k, ms in pnpc.items(): npc_ref[norm_script_name(k)] |= ms
print("Tong sau pak: %d ID trap khac nhau, %d ten script NPC khac nhau" % (len(trap_ref), len(npc_ref)))

def map_in_list(md):
    if md.startswith("pak:"): return True
    return lower_bytes(ansi_bytes(md)) in maplist_dirs

# ---------- 6) doi chieu trap ----------
res = collections.Counter()
thieu_trap = []   # (tid, maps, nguon)
for tid, maps in trap_ref.items():
    if tid in live_ids: res["live"] += 1
    elif tid in alias_ids: res["alias"] += 1
    elif tid in disan_ids: res["DISAN"] += 1; thieu_trap.append((tid, maps, "disan:" + disan_ids[tid].decode("latin-1")))
    elif tid in mirror_ids: res["MIRROR_ONLY"] += 1; thieu_trap.append((tid, maps, "mirror:" + mirror_ids[tid].decode("latin-1")))
    else: res["khong_biet"] += 1; thieu_trap.append((tid, maps, "?"))
print("\nTRAP ID: co trong cay %d, qua bi danh %d, NAM TRONG disan_jx %d, chi co o guong cu %d, khong biet (chet tu truoc) %d"
      % (res["live"], res["alias"], res["DISAN"], res["MIRROR_ONLY"], res["khong_biet"]))
# ---------- 7) doi chieu NPC ----------
resn = collections.Counter(); thieu_npc = []
for k, maps in npc_ref.items():
    if k in live: resn["live"] += 1
    elif k in alias and alias[k] in live: resn["alias"] += 1
    elif k in disan: resn["DISAN"] += 1; thieu_npc.append((k, maps, "disan"))
    elif k in mirror: resn["MIRROR_ONLY"] += 1; thieu_npc.append((k, maps, "mirror"))
    else: resn["khong_biet"] += 1; thieu_npc.append((k, maps, "?"))
print("NPC script (ten trong map): co trong cay %d, qua bi danh %d, NAM TRONG disan_jx %d, chi co o guong cu %d, khong biet %d"
      % (resn["live"], resn["alias"], resn["DISAN"], resn["MIRROR_ONLY"], resn["khong_biet"]))

# ---------- 8) liet ke ----------
def show(lst, title):
    print("\n== %s: %d" % (title, len(lst)))
    by_map = collections.Counter()
    for _, maps, src in lst:
        for m in maps: by_map[(m, map_in_list(m))] += 1
    for (m, inlist), c in by_map.most_common(40):
        print("  map %-40s %s : %d" % (m[:40], "DANG NAP" if inlist else "khong trong MapList", c))
    print("  -- mau:")
    for tid, maps, src in lst[:12]:
        print("   ", tid if isinstance(tid, int) else tid.decode("latin-1"), "<-", src, "|", ",".join(sorted(maps))[:80])
show([t for t in thieu_trap if t[2] != "?"], "TRAP MAT DO SAP XEP (tep con trong kho)")
show([t for t in thieu_trap if t[2] == "?"], "TRAP khong tim thay tep o dau (chet tu truoc)")
show([t for t in thieu_npc if t[2] != "?"], "NPC MAT DO SAP XEP")
show([t for t in thieu_npc if t[2] == "?"], "NPC khong tim thay tep (chet tu truoc)")

# ---------- 9) sua: chuyen ve duong dan cu ----------
if SUA:
    moved = 0
    todo = set()
    for tid, maps, src in thieu_trap:
        if src.startswith("disan:"): todo.add(disan_ids[tid])
    for k, maps, src in thieu_npc:
        if src == "disan": todo.add(k)
    for k in sorted(todo):
        rel = disan[k]
        s = os.path.join(DISAN, rel); d = os.path.join(ROOT, rel)
        os.makedirs(os.path.dirname(d), exist_ok=True)
        shutil.move(s, d); moved += 1
    print("\nDA CHUYEN VE cay chay (duong dan cu): %d tep" % moved)

# ---------- 10) chi tiet 28 trap trong du lieu map ----------
def gbk(b):
    try: return b.decode("gbk")
    except Exception: return b.decode("latin-1")
print("\n== CHI TIET TRAP TRONG DU LIEU MAP:")
for tid, maps in sorted(trap_ref.items(), key=lambda kv: -len(kv[1])):
    src = live_ids.get(tid) or alias_ids.get(tid) or disan_ids.get(tid) or mirror_ids.get(tid)
    print("  %08X  %-38s <- %s" % (tid, gbk(src)[:38] if src else "?", ", ".join(gbk(ansi_bytes(m)) for m in sorted(maps))[:70]))
