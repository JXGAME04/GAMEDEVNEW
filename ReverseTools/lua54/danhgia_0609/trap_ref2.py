import os, sys, struct, collections
ROOT = "E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server"
OUTD = sys.argv[1]
BS = chr(92)
def fid(b):
    Id = 0
    for i, ch in enumerate(b):
        c = ch if ch < 128 else ch - 256
        Id = ((Id + (i + 1) * c) & 0xFFFFFFFF) % 0x8000000B
        Id = (Id * 0xFFFFFFEF) & 0xFFFFFFFF
    return Id ^ 0x12345678
def to_bytes(u):
    try:
        return u.encode("gbk")
    except Exception:
        return u.encode("cp1252", errors="replace")
scripts = {}
for top in ("script", "scriptjx2"):
    for dp, dn, fn in os.walk(os.path.join(ROOT, top)):
        for f in fn:
            if f.lower().endswith(".lua") or f.lower().endswith(".txt"):
                rel = os.path.relpath(os.path.join(dp, f), ROOT).replace("/", BS)
                b = to_bytes(rel)
                bl = bytes(ch + 32 if 65 <= ch <= 90 else ch for ch in b)
                scripts[rel] = bl
idmap_a = {fid(b"\\" + bl): rel for rel, bl in scripts.items()}
idmap_b = {fid(bl): rel for rel, bl in scripts.items()}
trap_ids = collections.Counter()
trap_where = collections.defaultdict(set)
n_files = n_bad = n_npc_lua = 0
for dp, dn, fn in os.walk(os.path.join(ROOT, "Maps")):
    for f in fn:
        if not f.lower().endswith("region_s.dat"):
            continue
        p = os.path.join(dp, f)
        data = open(p, "rb").read()
        n_files += 1
        if len(data) < 4 + 8 * 6:
            n_bad += 1; continue
        cnt = struct.unpack_from("<I", data, 0)[0]
        if cnt < 6 or cnt > 64:
            n_bad += 1; continue
        secs = [struct.unpack_from("<II", data, 4 + 8 * i) for i in range(cnt)]
        head = 4 + 8 * cnt
        off, ln = secs[1]
        if ln >= 12:
            base = head + off
            n = struct.unpack_from("<I", data, base)[0]
            if 12 + n * 8 == ln:
                mapname = os.path.relpath(dp, os.path.join(ROOT, "Maps")).split(os.sep)[0]
                for i in range(n):
                    cx, cy, ncell, res, tid = struct.unpack_from("<BBBBI", data, base + 12 + i * 8)
                    trap_ids[tid] += 1
                    trap_where[tid].add(mapname)
            else:
                n_bad += 1
        off2, ln2 = secs[2]
        if ln2 > 0 and b".lua" in data[head + off2: head + off2 + ln2].lower():
            n_npc_lua += 1
out = []
out.append("Region_S.dat: %d tep (%d loi cau truc); o trap: %d; ID trap khac nhau: %d; khoi NPC co chuoi .lua: %d tep" % (
    n_files, n_bad, sum(trap_ids.values()), len(trap_ids), n_npc_lua))
ha = [t for t in trap_ids if t in idmap_a]; hb = [t for t in trap_ids if t in idmap_b]
out.append("Khop dang '\script\...': %d ID; dang 'script\...': %d ID" % (len(ha), len(hb)))
idmap = idmap_a if len(ha) >= len(hb) else idmap_b
matched = {t: idmap[t] for t in trap_ids if t in idmap}
unmatched = [t for t in trap_ids if t not in idmap]
out.append("ID trap KHONG khop tep script nao (script da xoa/di chuyen -> trap chet): %d ID, %d o, o %d map" % (
    len(unmatched), sum(trap_ids[t] for t in unmatched), len(set().union(*[trap_where[t] for t in unmatched])) if unmatched else 0))
by_top = collections.Counter()
for t, rel in matched.items():
    parts = rel.split(BS)
    by_top[BS.join(parts[:2])] += 1
out.append("")
out.append("Tep script duoc TRAP tham chieu, theo thu muc cap 2 (so tep):")
for k, v in by_top.most_common(30):
    try:
        ku = k.encode("gbk", "replace").decode("gbk") if False else k
    except Exception:
        ku = k
    out.append("  %4d  %s" % (v, k))
ref_files = sorted(set(matched.values()))
gbk_all = [r for r in scripts if any(ord(ch) > 127 for ch in r)]
gbk_ref = [r for r in ref_files if any(ord(ch) > 127 for ch in r)]
out.append("")
out.append("Tong tep script duoc trap tham chieu: %d; trong do ten/thu muc Han: %d / %d tep Han" % (len(ref_files), len(gbk_ref), len(gbk_all)))
maps_unm = collections.Counter()
for t in unmatched:
    for m in trap_where[t]:
        maps_unm[m] += 1
out.append("Map co nhieu trap chet nhat: " + ", ".join("%s(%d)" % kv for kv in maps_unm.most_common(8)))
open(os.path.join(OUTD, "trap_ref_files.txt"), "w", encoding="utf-8", errors="replace").write("\n".join(ref_files))
txt = "\n".join(out)
open(os.path.join(OUTD, "trap_ref_baocao.txt"), "w", encoding="utf-8", errors="replace").write(txt)
print(txt)
