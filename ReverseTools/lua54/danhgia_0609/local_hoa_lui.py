r"""local_hoa_lui.py - LUI PA-2: bo khoi dau '-- [LOCAL54 ...' + cac dong 'local a, b, ...' do local_hoa.py chen (theo local_hoa_ketqua.txt).
Giu nguyen phan con lai (latin-1, CRLF). In so tep da lui.
  python local_hoa_lui.py [local_hoa_ketqua.txt]
"""
import io, os, sys
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
KQ = sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.path.dirname(os.path.abspath(__file__)), "local_hoa_ketqua.txt")
n_ok = n_bo = 0
for ln in io.open(KQ, encoding="utf-8"):
    ln = ln.rstrip("\n")
    if "\t" not in ln: continue
    rel, names = ln.split("\t", 1)
    ten = set(x for x in names.split(",") if x)
    p = os.path.join(ROOT, rel)
    if not os.path.exists(p):
        print("khong thay", rel); n_bo += 1; continue
    s = io.open(p, "r", encoding="latin-1", newline="").read()
    bom = ""
    if s.startswith("\xef\xbb\xbf"): bom = s[:3]; s = s[3:]
    nl = "\r\n" if "\r\n" in s else "\n"
    lines = s.split(nl)
    if not lines or not lines[0].startswith("-- [LOCAL54 "):
        print("khong co khoi LOCAL54:", rel); n_bo += 1; continue
    i = 1
    while i < len(lines) and lines[i].startswith("local "):
        ds = set(x.strip() for x in lines[i][6:].split(","))
        if not ds <= ten: break
        i += 1
    s2 = nl.join(lines[i:])
    io.open(p, "w", encoding="latin-1", newline="").write(bom + s2)
    n_ok += 1
print("DA LUI %d tep, bo qua %d" % (n_ok, n_bo))
