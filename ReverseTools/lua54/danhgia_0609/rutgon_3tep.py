import os, io
ROOT = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
BS = chr(92)
D = "script" + BS + "bando" + BS + "trungnguyen_bac" + BS + "thiennhangiao" + BS + "thiennhangiaothatnoihuu2" + BS + "trap" + BS
doi = {
    D + "thiennhangiaothatnoihuu2to_thatsatdong.lua": D + "to_thatsatdong.lua",
    D + "thiennhangiaothatnoihuu2to_thiennhangiaothatnoi1.lua": D + "to_thatnoi1.lua",
    D + "thiennhangiaothatnoihuu2to_thiennhangiaothatnoi3.lua": D + "to_thatnoi3.lua",
}
for a, b in doi.items():
    os.rename(os.path.join(ROOT, a), os.path.join(ROOT, b))
    print(len(b) + 1, b)
ALIAS = os.path.join(ROOT, "script", "_duongdan_cu.txt")
raw = open(ALIAS, "rb").read()
m = {a.lower().encode("latin-1"): b.encode("latin-1") for a, b in doi.items()}
out = []; n = 0
for line in raw.split(b"\n"):
    if line.startswith(b"--@") and b"=" in line:
        cu, moi = line[3:].split(b"=", 1)
        key = moi.strip().rstrip(b"\r").lower()
        if key in m:
            line = b"--@ " + cu.strip() + b"=" + m[key] + (b"\r" if line.endswith(b"\r") else b""); n += 1
    out.append(line)
open(ALIAS, "wb").write(b"\n".join(out))
DC = os.path.join(ROOT, "script", "bando", "_DOICHIEU_TEN.txt")
s = io.open(DC, "r", encoding="utf-8", errors="replace").read().split("\n")
k2 = 0; o2 = []
for ln in s:
    if " = " in ln and not ln.startswith("#"):
        left, right = ln.split(" = ", 1)
        key = left.strip().lower().encode("latin-1", "replace")
        if key in m:
            ln = m[key].decode("latin-1") + " = " + right; k2 += 1
    o2.append(ln)
io.open(DC, "w", encoding="utf-8", newline="").write("\n".join(o2))
print("bi danh cap nhat", n, "; doi chieu", k2)
