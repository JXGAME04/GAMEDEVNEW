# -*- coding: utf-8 -*-
"""C34 - quet TOAN CAY script tim loi goi AddItem THIEU tham so (JX1 doi >= 7,
thieu la KHONG TAO ITEM VA IM LANG).
Ban truoc chi doc MOT DONG nen bao nham cac loi goi trai NHIEU DONG (vd
lib\\awardtype\\item_jx1.lua that ra co du 7 tham so tren 2 dong).
Ban nay doc theo BIEU THUC TRAI DONG: gom tiep cac dong den khi dong het ngoac.
Chi BAO CAO, khong tu sua (ngoai pham vi 3 hoat dong thi de chu game quyet).
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
OUT = r"D:\GAMEDEVNEW\ReverseTools\port_3hd\audit\C34_additem_toancay.md"


def bo_chu_thich(dong):
    """bo phan sau '--' NEU khong nam trong chuoi"""
    trong = None
    for i, ch in enumerate(dong):
        if trong:
            if ch == trong:
                trong = None
            continue
        if ch in "\"'":
            trong = ch
        elif ch == "-" and i + 1 < len(dong) and dong[i + 1] == "-":
            return dong[:i]
    return dong


def dem_arg(s):
    """dem tham so cua chuoi BAT DAU ngay sau dau '(' - tra (so, da_dong_ngoac)"""
    out, cur, dep = [], "", 0
    for ch in s:
        if ch in "({[":
            dep += 1
        elif ch in ")}]":
            if dep == 0:
                if cur.strip():
                    out.append(cur)
                return len(out), True
            dep -= 1
        if ch == "," and dep == 0:
            out.append(cur)
            cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur)
    return len(out), False


loi = []
for dp, dn, fs in os.walk(SRV):
    for f in fs:
        if not f.endswith(".lua"):
            continue
        p = os.path.join(dp, f)
        try:
            lines = io.open(p, encoding="latin-1", newline="").read().split("\n")
        except Exception:
            continue
        for i, line in enumerate(lines):
            s = bo_chu_thich(line)
            if s.lstrip().startswith("--"):
                continue
            for m in re.finditer(r"(?<![A-Za-z_0-9])AddItem\s*\(", s):
                # gom tiep toi da 6 dong cho den khi dong ngoac
                buf = s[m.end():]
                n, xong = dem_arg(buf)
                k = i
                while not xong and k + 1 < len(lines) and k - i < 6:
                    k += 1
                    buf += " " + bo_chu_thich(lines[k])
                    n, xong = dem_arg(buf)
                if xong and n < 7:
                    loi.append((os.path.relpath(p, SRV), i + 1, n, s.strip()[:70]))

theotep = {}
for (rel, d, n, t) in loi:
    theotep[rel] = theotep.get(rel, 0) + 1
print("=== AddItem THIEU tham so (doc da dong) ===")
print("Tong %d cho / %d tep\n" % (len(loi), len(theotep)))
for k in sorted(theotep, key=lambda x: -theotep[x]):
    print("  %3d  %s" % (theotep[k], k))

os.makedirs(os.path.dirname(OUT), exist_ok=True)
with io.open(OUT, "w", encoding="utf-8") as f:
    f.write("# C34 - AddItem thieu tham so (toan cay script)\n\n")
    f.write("JX1 `LuaAddItem`: `if (nParamNum < 7) return 0;` => **khong tao item, im lang**.\n\n")
    f.write("Tong **%d** cho / **%d** tep.\n\n" % (len(loi), len(theotep)))
    cur = None
    for (rel, d, n, t) in sorted(loi):
        if rel != cur:
            cur = rel
            f.write("\n## %s\n" % rel)
        f.write("- dong %d (%d tham so): `%s`\n" % (d, n, t.replace("`", "'")))
print("\nda ghi", OUT)
