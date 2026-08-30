# -*- coding: utf-8 -*-
"""Doi chieu 3 noi cho TUNG he so exp, de chac chan KHONG doi can bang:
   (a) so cung trong ban goc KPlayer.cpp.truoc_cfgexp
   (b) gia tri mac dinh trong KCore.cpp hien tai
   (c) gia tri trong settings\\gamesetting.ini nhom [Exp]
Ba noi phai khop tuyet doi. Lech mot cho la DOI CAN BANG.
"""
import io
import os
import re
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"
INI = (r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
       r"\settings\gamesetting.ini")


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


# so cung mong doi, doc TU BAN GOC (khong go tay)
goc_p = os.path.join(CORE, "KPlayer.cpp.truoc_cfgexp")
if not os.path.isfile(goc_p):
    print("!!! khong con ban goc KPlayer.cpp.truoc_cfgexp - khong doi chieu duoc")
    sys.exit(1)
goc = doc(goc_p)

# rut so cung tu ban goc
MONG = {}
m = re.search(r"if \(level < (\d+)\)\s*\n\s*nxExpTanThu = (\d+) \+ nxExpVip;\s*\n"
              r"\s*else if \(level < (\d+)\)\s*\n\s*nxExpTanThu = (\d+) \+ nxExpVip;\s*\n"
              r"\s*else if \(level < (\d+)\)\s*\n\s*nxExpTanThu = (\d+) \+ nxExpVip;\s*\n"
              r"\s*else\s*\n\s*nxExpTanThu = (\d+) \+ nxExpVip;",
              goc.replace("\r\n", "\n"))
if m:
    (MONG["MocCap1"], MONG["HeSo1"], MONG["MocCap2"], MONG["HeSo2"],
     MONG["MocCap3"], MONG["HeSo3"], MONG["HeSo4"]) = [int(x) for x in m.groups()]
else:
    print("!!! khong rut duoc he so theo cap tu ban goc")

m = re.search(r"m_CurrentExpSkillsVip == 2\)\s*\n\s*nxExpVip = (\d+);",
              goc.replace("\r\n", "\n"))
if m:
    MONG["VipCong"] = int(m.group(1))

m = re.search(r"GetReBornNum\(\) > (\d+) && Map == (\d+)\)\s*\n\s*\{\s*\n"
              r"\s*if \(level < (\d+)\)\s*\n\s*nxExpTanThu = (\d+) \+ nxExpVip;\s*\n"
              r"\s*else\s*\n\s*nxExpTanThu = (\d+) \+ nxExpVip;",
              goc.replace("\r\n", "\n"))
if m:
    g = [int(x) for x in m.groups()]
    MONG["CsLanToiThieu"], MONG["CsMap"] = g[0], g[1]
    MONG["CsDuoi"], MONG["CsTren"] = g[3], g[4]

m = re.search(r"if \(nSubLevel <= (\d+)\)", goc)
if m:
    MONG["ChenhCapMax"] = int(m.group(1))
m = re.search(r"nGetExp = nExp /(\d+);", goc)
if m:
    MONG["ChiaKhiChenh"] = int(m.group(1))
m = re.search(r"if \(nTarLevel >= (\d+) &&", goc)
if m:
    MONG["MienTruCap"] = int(m.group(1))
for n in range(2, 9):
    m = re.search(r"nTotalPlayer == %d\)\s*\n\s*ShareExp = (\d+);" % n,
                  goc.replace("\r\n", "\n"))
    if m:
        MONG["ToDoi%d" % n] = int(m.group(1))
m = re.search(r"ShareExp = 50;\s*\n\s*else\s*\n\s*ShareExp = (\d+);",
              goc.replace("\r\n", "\n"))
if m:
    MONG["ToDoiKhac"] = int(m.group(1))

# (b) mac dinh trong KCore.cpp
kc = doc(os.path.join(CORE, "KCore.cpp"))
MD = {}
for m in re.finditer(r"int\s+g_nExp(\w+)\s*=\s*(\d+)\s*;", kc):
    MD[m.group(1)] = int(m.group(2))

# (c) gamesetting.ini nhom [Exp]
ini = doc(INI).replace("\r\n", "\n")
kh = ini.find("[Exp]")
IN = {}
if kh >= 0:
    doan = ini[kh:]
    ket = doan.find("\n[", 1)
    if ket > 0:
        doan = doan[:ket]
    for m in re.finditer(r"^\s*(\w+)\s*=\s*(\d+)", doan, re.M):
        IN[m.group(1)] = int(m.group(2))

print("%-16s %-10s %-10s %-10s %s" % ("KHOA", "GOC(cu)", "KCore(md)", "ini", "ket qua"))
print("-" * 62)
lech = 0
for k in sorted(set(list(MONG) + list(MD) + list(IN))):
    a = MONG.get(k)
    b = MD.get(k)
    c = IN.get(k)
    if a is None:
        kq = "(khong rut duoc tu ban goc)"
    elif a == b == c:
        kq = "khop"
    else:
        kq = "!!! LECH"
        lech += 1
    print("%-16s %-10s %-10s %-10s %s" % (k, a, b, c, kq))

print()
if lech:
    print("!!! CO %d KHOA LECH - DOI CAN BANG, PHAI SUA" % lech)
    sys.exit(1)
print("Tat ca khop: mac dinh = so cung cu => KHONG doi can bang.")
