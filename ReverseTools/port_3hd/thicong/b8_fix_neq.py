# -*- coding: utf-8 -*-
"""B8 - VA NOT dang so sanh '~=' (KHAC) ma b7 bo sot (b7 chi bat '==').

fld_head.lua:274  'if (itemgenre ~= 6 or particular ~= 2745) then'
  = CONG KIEM VE chinh cua gio ton phi. Sot = nguoi choi nop dung Lenh Bai Thuy Tac
  van bi tu choi (vi JX1 2745 la "Thung go").

Quet lai TOAN BO tep da chep cho CA 44 bo so (42 goc + 2745 + 196) o MOI dang:
  '== N'  '~= N'  '6,1,N'  '{6,1,N'  ', N)'  -> chi doi khi ro rang la ID vat pham.
Cach an toan: chi doi trong cac bieu thuc so sanh voi bien co ten
  itemgenre/detailtype/particular/parttype/np/nd/ng/nParticular...
"""
import io, os, re, json, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
HERE = os.path.dirname(os.path.abspath(__file__))

remap = json.load(io.open(os.path.join(HERE, "remap_resolved.json"), encoding="utf-8"))
# bang particular: nguon -> dich (chi lay khi genre+detail giu nguyen 6,1)
PT = {}
for src, dst in remap.items():
    sg, sd, sp = src.split(",")
    dg, dd, dp = dst.split(",")
    if (sg, sd) == (dg, dd):
        PT[sp] = dp
PT["2745"] = "3363"
PT["196"] = "195"

# ten bien thuong dung cho particular / detailtype
VARS = r"(?:particular|parttype|nParticular|nPart|np|particulartype|nParticularType)"
files = [x.strip().replace("/", os.sep) for x in
         io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n")
         if x.strip().lower().endswith(".lua")]
files += [r"script\item\hd3_admin.lua", r"script\tinhnang\3hoatdong\hd3_driver.lua",
          r"script\missions\fengling_ferry\hd3_thuyenphu.lua"]

total_files = 0
total_hits = 0
for rel in sorted(set(files)):
    p = os.path.join(JX1, rel)
    if not os.path.isfile(p):
        continue
    d = io.open(p, encoding="latin-1", newline="").read()
    orig = d
    notes = []
    for src, dst in PT.items():
        # '<bien> ~= N'  hoac  '<bien> == N'
        pat = re.compile(r"(\b%s\s*[~=]=\s*)%s(?![0-9])" % (VARS, src))
        d, c = pat.subn(lambda m, dd=dst: m.group(1) + dd, d)
        if c:
            notes.append("%s->%s x%d" % (src, dst, c))
            total_hits += c
    if d != orig:
        io.open(p, "w", encoding="latin-1", newline="").write(d)
        m = os.path.join(MIRROR, rel)
        os.makedirs(os.path.dirname(m), exist_ok=True)
        io.open(m, "w", encoding="latin-1", newline="").write(d)
        total_files += 1
        print("  %-46s %s" % (rel.rsplit(os.sep, 1)[-1], "; ".join(notes)))

print()
print("Sua %d cho trong %d tep." % (total_hits, total_files))
