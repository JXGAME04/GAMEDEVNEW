r"""rutgon_bando.py - KHAN 06/09 toi: rut gon duong dan script\bando\... qua dai (>= 96 ky tu ke ca '\' dau) vi
KLuaScript::Load strcpy vao m_szScriptName[100] khong gioi han -> GameServer sap luc boot (19:34).
Quy tac: ten tep = <stem>.lua; stem moi = bo tien to trung ten thu muc cha ("<thanh>to_" -> "to_", "<thanh>_to_" -> "to_"),
neu van dai: cat stem con 32 ky tu + "_" + 4 hex FNV cua stem goc. Bao dam duy nhat trong thu muc va ID bam khong trung.
Cap nhat: script\_duongdan_cu.txt (ve phai '=') va script\bando\_DOICHIEU_TEN.txt. Ghi nhat ky rutgon_bando_log.txt.
  python rutgon_bando.py        -> ke hoach
  python rutgon_bando.py sua    -> thuc hien
"""
import os, sys, io, re
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
SCRIPT = os.path.join(ROOT, "script")
ALIAS = os.path.join(SCRIPT, "_duongdan_cu.txt")
DOICHIEU = os.path.join(SCRIPT, "bando", "_DOICHIEU_TEN.txt")
SUA = len(sys.argv) > 1 and sys.argv[1] == "sua"
GIOI_HAN = 90          # do dai toi da cua "\script\..." (ke ca '\' dau); engine: m_szScriptName[100]
BS = chr(92)

def fnv(s):
    h = 2166136261
    for c in s.encode("latin-1", "replace"):
        h ^= c; h = (h * 16777619) & 0xFFFFFFFF
    return h

def fid(b):
    Id = 0
    for i, ch in enumerate(b):
        c = ch if ch < 128 else ch - 256
        Id = ((Id + (i + 1) * c) & 0xFFFFFFFF) % 0x8000000B
        Id = (Id * 0xFFFFFFEF) & 0xFFFFFFFF
    return Id ^ 0x12345678
def key_of(rel):
    return (BS + rel.replace("/", BS).lower()).encode("latin-1", "replace")

# ID hien co cua moi tep + khoa bi danh (tranh trung)
ids = set()
for top in ("script", os.path.join("scriptjx2", "tong_vn")):
    for dp, dn, fn in os.walk(os.path.join(ROOT, top)):
        for f in fn:
            if f.lower().endswith((".lua", ".txt")):
                ids.add(fid(key_of(os.path.relpath(os.path.join(dp, f), ROOT))))
alias_raw = open(ALIAS, "rb").read()
for line in alias_raw.split(b"\n"):
    if line.startswith(b"--@") and b"=" in line:
        cu = line[3:].split(b"=", 1)[0].strip().lower().replace(b"/", BS.encode()).lstrip(BS.encode())
        ids.add(fid(BS.encode() + cu))

plan = []   # (rel_cu, rel_moi)
used = set()
for dp, dn, fn in os.walk(os.path.join(SCRIPT, "bando")):
    for f in fn:
        rel = os.path.relpath(os.path.join(dp, f), ROOT)
        used.add(rel.lower())
for dp, dn, fn in os.walk(os.path.join(SCRIPT, "bando")):
    thanh = os.path.basename(dp)
    for f in sorted(fn):
        rel = os.path.relpath(os.path.join(dp, f), ROOT)
        if len(rel) + 1 <= GIOI_HAN or f.startswith("_"): continue
        stem, ext = os.path.splitext(f)
        s2 = stem
        for pre in (thanh + "to_", thanh + "_to_", thanh + "to", thanh + "_"):
            if s2.startswith(pre) and len(s2) > len(pre) + 2:
                s2 = "to_" + s2[len(pre):] if "to" in pre else s2[len(pre):]
                break
        dir_len = len(os.path.relpath(dp, ROOT)) + 1 + 1   # '\' dau + '\' truoc ten
        conmax = GIOI_HAN - dir_len - len(ext)
        if len(s2) > conmax:
            keep = max(8, conmax - 5)
            s2 = s2[:keep].rstrip("_") + "_" + ("%04x" % (fnv(stem) & 0xFFFF))
        cand = s2 + ext
        k = 2
        while os.path.join(os.path.relpath(dp, ROOT), cand).lower() in used or fid(key_of(os.path.join(os.path.relpath(dp, ROOT), cand))) in ids:
            cand = "%s_%d%s" % (s2, k, ext); k += 1
        rel_moi = os.path.join(os.path.relpath(dp, ROOT), cand)
        if len(rel_moi) + 1 > GIOI_HAN:
            print("KHONG RUT DU:", rel_moi); continue
        used.add(rel_moi.lower()); ids.add(fid(key_of(rel_moi)))
        plan.append((rel, rel_moi))
print("Duong dan > %d: %d tep can doi ten" % (GIOI_HAN, len(plan)))
for a, b in plan[:6]: print("  ", a[-70:], "->", os.path.basename(b))
mx = max((len(b) + 1 for a, b in plan), default=0)
print("dai nhat sau khi doi: %d" % mx)
if not SUA: sys.exit(0)

# thuc hien
log = []
alias_map = {a.lower().encode("latin-1"): b.encode("latin-1") for a, b in plan}
for a, b in plan:
    os.rename(os.path.join(ROOT, a), os.path.join(ROOT, b))
    log.append(a + " -> " + b)
# bi danh: ve phai
out = []
n_alias = 0
for line in alias_raw.split(b"\n"):
    if line.startswith(b"--@") and b"=" in line:
        cu, moi = line[3:].split(b"=", 1)
        moi_s = moi.strip().rstrip(b"\r")
        key = moi_s.lower().replace(b"/", BS.encode())
        if key in alias_map:
            line = b"--@ " + cu.strip() + b"=" + alias_map[key] + (b"\r" if line.endswith(b"\r") else b"")
            n_alias += 1
    out.append(line)
open(ALIAS + ".truoc_rutgon_0609", "wb").write(alias_raw)
open(ALIAS, "wb").write(b"\n".join(out))
# doi chieu
dc = io.open(DOICHIEU, "r", encoding="utf-8", errors="replace").read().split("\n")
dc2 = []; n_dc = 0
for ln in dc:
    if " = " in ln and not ln.startswith("#"):
        left, right = ln.split(" = ", 1)
        k = left.strip().lower().encode("latin-1", "replace")
        if k in alias_map:
            ln = alias_map[k].decode("latin-1") + " = " + right; n_dc += 1
    dc2.append(ln)
io.open(DOICHIEU, "w", encoding="utf-8", newline="").write("\n".join(dc2))
io.open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "rutgon_bando_log.txt"), "w", encoding="utf-8").write("\n".join(log) + "\n")
print("DA DOI TEN %d tep; bi danh cap nhat %d dong; doi chieu %d dong" % (len(plan), n_alias, n_dc))
