r"""so_globals.py - so bang bien toan cuc moi state do boot_gia.py ghi (BOOT_DUMP_GLOBALS) TRUOC va SAU khi local hoa (PA-2).
Dung: moi tep sau = truoc - (ten da local hoa trong local_hoa_ketqua.txt); ten khac = LOI (in ra).
  python so_globals.py g_truoc.txt g_sau.txt local_hoa_ketqua.txt
"""
import sys, io
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
truoc, sau, kq = sys.argv[1:4]
def load(p):
    d = {}
    for ln in io.open(p, encoding="utf-8", errors="replace"):
        ln = ln.rstrip("\n")
        if "\t" not in ln: continue
        f, names = ln.split("\t", 1)
        d[f.lower()] = set(x for x in names.split(",") if x)
    return d
A, B = load(truoc), load(sau)
L = {}
for ln in io.open(kq, encoding="utf-8", errors="replace"):
    ln = ln.rstrip("\n")
    if "\t" not in ln: continue
    f, names = ln.split("\t", 1)
    L[f.lower()] = set(x for x in names.split(",") if x)
n_ok = n_loi = n_khong = n_inc = 0
UNION = set()
for v in L.values(): UNION |= v
for f, ga in A.items():
    gb = B.get(f)
    if gb is None: n_khong += 1; continue
    loc = L.get(f, set())
    them = gb - ga
    thieu = ga - gb
    # thieu hop le: ten da local hoa o chinh tep, hoac o tep duoc Include (thuoc hop tat ca ten da local hoa)
    thieu_la = thieu - UNION
    if not them and not thieu_la:
        n_ok += 1
        if thieu - loc: n_inc += 1
    else:
        n_loi += 1
        if n_loi <= 40:
            print("KHAC %s: them=%s thieu_la=%s" % (f, ",".join(sorted(them))[:120], ",".join(sorted(thieu_la))[:160]))
print("tep dung mong doi: %d (trong do %d chi mat ten local hoa cua tep Include); KHAC that: %d; khong co o SAU (doi ten): %d; tep local hoa: %d" % (n_ok, n_inc, n_loi, n_khong, len(L)))
sys.exit(0 if n_loi == 0 else 2)
