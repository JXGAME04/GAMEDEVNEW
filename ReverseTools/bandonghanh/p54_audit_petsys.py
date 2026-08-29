# -*- coding: utf-8 -*-
r"""[PETSYS 29/08] BANG KIEM TOAN DIEN: doi chieu ban LINUX (D:\ServerLinux)
voi JX1 dang chay - tra loi "port dung het chua" bang SO LIEU, khong doan.
"""
import io
import os
import re
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_edit import doc_vn  # noqa: E402

LX = r"D:\ServerLinux\server1"
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"


def doc(p):
    return io.open(p, "r", encoding="latin-1").read().replace(chr(13), "")


print("=" * 72)
print("1. SCRIPT petsys: Linux -> JX1")
lxf = sorted(os.listdir(os.path.join(LX, "script", "petsys")))
jxf = sorted(os.listdir(os.path.join(SV, "script", "petsys")))
for f in lxf:
    if not f.endswith(".lua"):
        continue
    co = f in jxf
    hl = len(re.findall(r"function\s", doc(os.path.join(LX, "script", "petsys", f))))
    hj = len(re.findall(r"function\s", doc(os.path.join(SV, "script", "petsys", f)))) if co else 0
    print("   %-24s %s  ham Linux=%-3d JX1=%-3d" % (f, "CO " if co else "THIEU", hl, hj))
them = [f for f in jxf if f.endswith(".lua") and f not in lxf]
print("   (JX1 them:", ", ".join(them), ")")

print("=" * 72)
print("2. BANG settings/petsys")
for f in ("feature.txt", "levelup.txt", "pet_skill_def.txt"):
    a = os.path.join(LX, "settings", "petsys", f)
    b = os.path.join(SV, "settings", "petsys", f)
    na = len([x for x in doc(a).split("\n") if x.strip()]) if os.path.exists(a) else -1
    nb = len([x for x in doc(b).split("\n") if x.strip()]) if os.path.exists(b) else -1
    print("   %-20s Linux %-5s dong | JX1 %-5s dong" % (f, na, nb))

print("=" * 72)
print("3. ITEM pet (id JX1 = id Linux + 1421)")
lxi = {c[3]: c for c in [d.split("\t") for d in doc(os.path.join(LX, "settings", "item", "004", "magicscript.txt")).split("\n")] if len(c) > 8}
jxi = {c[3]: c for c in [d.split("\t") for d in doc(os.path.join(SV, "settings", "item", "magicscript.txt")).split("\n")] if len(c) > 8}
for idl in ("3453", "3454", "3455", "3456", "3457", "3458"):
    idj = str(int(idl) + 1421)
    a, b = lxi.get(idl), jxi.get(idj)
    if not b:
        print("   %s -> %s THIEU BEN JX1" % (idl, idj))
        continue
    kten = "OK" if a[0] == b[0] else "LECH"
    kanh = "OK" if a[4] == b[4] else "LECH"
    kmt = "OK" if a[8] == b[8] else "LECH"
    print("   %s->%s  ten %-5s anh %-5s mo-ta %-5s  | %s" % (idl, idj, kten, kanh, kmt, doc_vn(b[0])[:22]))

print("=" * 72)
print("4. HAM PET_* (Linux binary vs JX1 dang ky)")
lm = doc(r"D:\GAMEDEVNEW\ReverseTools\jx_linux_y.luamap.full.txt")
apil = sorted(set(re.findall(r"\b(PET_\w+)", lm)))
sf = doc(r"D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp")
apij = sorted(set(re.findall(r'"(PET_\w+)"', sf)))
print("   Linux:", len(apil), "| JX1:", len(apij))
print("   JX1 THIEU:", ", ".join(x for x in apil if x not in apij) or "khong")

print("=" * 72)
print("5. MENU / CALLBACK: ham duoc goi ma khong ton tai")
dinh = set()
goi = set()
for thu in (os.path.join(SV, "script", "petsys"), os.path.join(SV, "script", "item")):
    for f in os.listdir(thu):
        if not f.endswith(".lua"):
            continue
        s = doc(os.path.join(thu, f))
        dinh |= set(re.findall(r"function\s+([\w:.]+)\s*\(", s))
        goi |= set(re.findall(r'/(\w+)"', s))
        goi |= set(re.findall(r"PetSys\.(\w+)", s))
xau = sorted(x for x in goi if x not in dinh and not x.startswith("no")
             and ("PetSys:" + x) not in dinh and x not in ("g_DailogBack",))
print("   thieu:", ", ".join(xau) if xau else "KHONG")

print("=" * 72)
print("6. SKILL / AURA")
sk = doc(os.path.join(SV, "settings", "skills.txt")).split("\n")
n16 = len([1 for d in sk if len(d.split("\t")) > 2 and d.split("\t")[2].strip() in [str(x) for x in range(1600, 1604)]])
n17 = len([1 for d in sk if len(d.split("\t")) > 2 and d.split("\t")[2].strip().isdigit() and 1670 <= int(d.split("\t")[2]) <= 1687])
print("   aura 1600-1603: %d/4 | ext skill 1670-1687: %d/18" % (n16, n17))
for f in (r"script\skill\petsys\aura.lua", r"script\skill\petskill.lua"):
    print("   %-30s %s" % (f, "CO" if os.path.exists(os.path.join(SV, f)) else "THIEU"))
print("=" * 72)
