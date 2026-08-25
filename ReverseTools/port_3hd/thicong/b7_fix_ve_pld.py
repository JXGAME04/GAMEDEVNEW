# -*- coding: utf-8 -*-
"""B7 - VA VE VAO PHONG LANG DO (bo sot khoi bang 42 item vi xuat hien duoi dang
SO SANH TRAN 'particular == N' chu khong phai bo so '6,1,N'):

  Lenh Bai Thuy Tac : Linux 6,1,2745 -> JX1 6,1,3363   (JX1 2745 = "Thung go"!)
  Mat do than bi    : Linux 6,1,196  -> JX1 6,1,195    (JX1 196 = "Thuong Thien lenh"!)
  Lenh bai Phong Lang Do : 4,489 GIONG NHAU ca hai ben -> KHONG doi.

Khong va thi: nguoi choi phai nop "Thung go" / "Thuong Thien lenh" moi len duoc
thuyen, va nut admin phat nham vat pham.
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
HERE = os.path.dirname(os.path.abspath(__file__))

REMAP = {"2745": "3363", "196": "195"}

# tep can quet: manifest + 4 tep moi
man = [x.strip() for x in io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n") if x.strip()]
extra = [
    r"script\item\hd3_admin.lua",
    r"script\tinhnang\3hoatdong\hd3_driver.lua",
    r"script\missions\fengling_ferry\hd3_thuyenphu.lua",
    r"script\global\autoexec_npc_hd3.lua",
]
files = sorted(set([m.replace("/", os.sep) for m in man if m.lower().endswith(".lua")] + extra))

total = 0
for rel in files:
    p = os.path.join(JX1, rel)
    if not os.path.isfile(p):
        continue
    d = io.open(p, encoding="latin-1", newline="").read()
    orig = d
    hits = []
    for src, dst in REMAP.items():
        # (a) bo so day du 6,1,N
        pat_full = re.compile(r"(?<![0-9])6(\s*,\s*)1(\s*,\s*)%s(?![0-9])" % src)
        d, c1 = pat_full.subn(lambda m, dd=dst: "6%s1%s%s" % (m.group(1), m.group(2), dd), d)
        # (b) so sanh tran: particular == N / parttype == N / np == N / detailtype == N
        pat_cmp = re.compile(r"(==\s*)%s(?![0-9])" % src)
        d, c2 = pat_cmp.subn(lambda m, dd=dst: "%s%s" % (m.group(1), dd), d)
        if c1 or c2:
            hits.append("%s->%s (bo so %d, so sanh %d)" % (src, dst, c1, c2))
    if d != orig:
        io.open(p, "w", encoding="latin-1", newline="").write(d)
        m = os.path.join(MIRROR, rel)
        os.makedirs(os.path.dirname(m), exist_ok=True)
        io.open(m, "w", encoding="latin-1", newline="").write(d)
        n = sum(int(x) for x in re.findall(r"\d+", " ".join(hits)) if True) # chi de dem tho
        total += 1
        print("  %-58s %s" % (rel.rsplit(os.sep, 1)[-1], "; ".join(hits)))

print()
print("Da sua %d tep." % total)
