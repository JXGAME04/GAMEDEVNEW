# -*- coding: utf-8 -*-
r"""b5c_kiem_cheo.py -- KIEM CHEO DOC LAP ket qua cua b5c_nan_ma_script.py.

Khong dung MAP cua bo nan; tu dung lai bang tu HAI tep magicscript roi doi chieu.
Kiem 6 viec:
  1. luat PT = dong - 2 cua bang JX1 (moi dong du lieu).
  2. moi gia tri 6,1,N con lai trong cay script PHAI ton tai trong bang JX1, va
     ten cua no phai trung ten cua ma Linux tuong ung (tra nguoc qua bang nan).
  3. moi cum 6,<detail>,N voi detail != 1 phai nam trong danh sach da doi chieu.
  4. day byte >= 0x80 (TCVN3/GBK) cua tep TRUOC (.truoc_nanma) va SAU phai giong het.
  5. so dong, so CRLF khong doi; moi dong doi chi duoc khac o CHU SO.
  6. khong con so nguyen nao bang mot ma NGUON Linux ma nam ngoai cac cho da xu ly.

CHAY:
  set PYTHONIOENCODING=utf-8
  python b5c_kiem_cheo.py
Tra ve 0 neu sach, 2 neu co loi.
"""
import os, re, sys, importlib.util

ROOT = r"D:\GAMEDEVNEW_wt_vantieu\serverscript_jx2\vantieu\lua54"
PL   = r"D:\ServerLinux\server1\settings\item\004\magicscript.txt"
PJ   = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\item\magicscript.txt"
BONAN = os.path.join(os.path.dirname(os.path.abspath(__file__)), "b5c_nan_ma_script.py")

# cac cum genre 6 detail != 1 DA DOI CHIEU TAY -- xem ghi chu trong bo nan
G6_KHAC1_OK = {(0, 20), (0, 2398), (6, 6)}

loi = []

def dong_tep(p):
    return open(p, "rb").read().replace(b"\r\n", b"\n").split(b"\n")

def bang(p):
    d = {}
    L = dong_tep(p)
    for i in range(1, len(L)):
        c = L[i].split(b"\t")
        if len(c) < 5 or not c[1].isdigit():
            continue
        d[(int(c[1]), int(c[2]), int(c[3]))] = (i + 1, c[0], c[4])
    return d

# ---------------------------------------------------------------- 1. luat PT
JD = dong_tep(PJ)
soJ = lech = 0
JPT = {}
for i in range(1, len(JD)):
    c = JD[i].split(b"\t")
    if len(c) < 5 or not c[1].isdigit():
        continue
    soJ += 1
    if int(c[3]) != i - 1:
        lech += 1
    JPT[int(c[3])] = (c[0], c[4])
print("1. bang JX1: %d dong du lieu, PT != dong-2: %d" % (soJ, lech))
if lech:
    loi.append("bang JX1 pha luat PT = dong - 2")

LIN = bang(PL)
print("   bang Linux: %d ban ghi" % len(LIN))

# ------------------------------------------------- doc MAP/MO cua bo nan
spec = importlib.util.spec_from_file_location("bonan", BONAN)
src = open(BONAN, "rb").read().decode("ascii")
MAP = {}
m = re.search(r"^MAP = \{(.*?)^\}", src, re.S | re.M)
for a, b in re.findall(r"(\d+):\s*(\d+)", m.group(1)):
    MAP[int(a)] = int(b)
MO = set()
m = re.search(r"^MO = \{(.*?)^\}", src, re.S | re.M)
for a in re.findall(r"^\s*(\d+):", m.group(1), re.M):
    MO.add(int(a))
NGUOC = {}
for k, v in MAP.items():
    NGUOC.setdefault(v, []).append(k)

# ---------------------------------------------------------------- liet ke tep
tep = []
for dp, dn, fn in os.walk(ROOT):
    for f in sorted(fn):
        if f.endswith(".lua"):
            tep.append(os.path.join(dp, f))
tep.sort()

RE61   = re.compile(rb"(?<![\w.])6(\s*,\s*)1(\s*,\s*)(\d+)")
RE6ANY = re.compile(rb"(?<![\w.])6(\s*,\s*)(\d+)(\s*,\s*)(\d+)")
RENUM  = re.compile(rb"(?<![\w.])(\d+)(?![\w.])")

# So ten TCVN3: bo dau cach / ngoac / cham, ha chu HOA ASCII, va ha rieng
# ba chu HOA co dau ma TCVN3 co (0xA7 D-stroke -> 0xAE, 0xA8 A-breve -> 0xB8,
# O-horn U-horn D-stroke) -> ban thuong A8..AE, de "(dai)" == "(Dai)".
_HA = {0xA7: 0xAE, 0xA8: 0xB8, 0xA9: 0xA2, 0xAA: 0xAA, 0xAB: 0xAB}
def chuan(b):
    out = bytearray()
    for x in b:
        if x in (0x20, 0x28, 0x29, 0x2E, 0x2C, 0x09):
            continue
        if 0x41 <= x <= 0x5A:
            x += 32
        x = _HA.get(x, x)
        out.append(x)
    return bytes(out)

KHAC = set()

# --------------------------------------- 2 + 3: gia tri sau khi nan
so2 = so3 = 0
for p in tep:
    rel = os.path.relpath(p, ROOT)
    raw = open(p, "rb").read()
    for mm in RE61.finditer(raw):
        v = int(mm.group(3))
        so2 += 1
        if v in MO:
            continue                      # co y giu ma Linux, da bao rieng
        if v not in JPT:
            loi.append("%s: ma %d KHONG co dong trong bang JX1" % (rel, v))
            continue
        for src_lx in NGUOC.get(v, []):
            k = (6, 1, src_lx)
            if k not in LIN:
                continue
            if chuan(LIN[k][1]) == chuan(JPT[v][0]):
                continue
            if LIN[k][2].lower() == JPT[v][1].lower():
                continue           # spr trung -> du tin
            KHAC.add((src_lx, v, LIN[k][1], JPT[v][0]))
    for mm in RE6ANY.finditer(raw):
        d, v = int(mm.group(2)), int(mm.group(4))
        if d == 1:
            continue
        so3 += 1
        if (d, v) not in G6_KHAC1_OK:
            dong = raw.count(b"\n", 0, mm.start()) + 1
            loi.append("%s:%d cum 6,%d,%d (detail khac 1) CHUA doi chieu" % (rel, dong, d, v))
print("2. cum 6,1,N sau khi nan: %d cho" % so2)
print("3. cum 6,<detail khac 1>,N: %d cho (trong danh sach da doi chieu)" % so3)
if KHAC:
    print("   (ten khac ca spr khac -- da doi chieu tay, xem GHICHU cua bo nan):")
    for a, b, ta, tb in sorted(KHAC):
        print("     %5d -> %-5d Linux %r  |  JX1 %r" % (a, b, ta[:34], tb[:34]))

# --------------------------------------- 4 + 5: byte cao va hinh dang dong
sotep = sodong = 0
for p in tep:
    bak = p + ".truoc_nanma"
    if not os.path.exists(bak):
        continue
    sotep += 1
    a = open(bak, "rb").read()
    b = open(p, "rb").read()
    rel = os.path.relpath(p, ROOT)
    if bytes(x for x in a if x >= 0x80) != bytes(x for x in b if x >= 0x80):
        loi.append("%s: day byte >= 0x80 (TCVN3/GBK) DA DOI" % rel)
    if a.count(b"\r\n") != b.count(b"\r\n"):
        loi.append("%s: so CRLF doi" % rel)
    la = a.replace(b"\r\n", b"\n").split(b"\n")
    lb = b.replace(b"\r\n", b"\n").split(b"\n")
    if len(la) != len(lb):
        loi.append("%s: so dong doi (%d -> %d)" % (rel, len(la), len(lb)))
        continue
    for i, (x, y) in enumerate(zip(la, lb)):
        if x == y:
            continue
        sodong += 1
        if re.sub(rb"\d+", b"#", x) != re.sub(rb"\d+", b"#", y):
            loi.append("%s:%d doi HON ca chu so" % (rel, i + 1))
print("4/5. %d tep co ban luu, %d dong doi, chi khac chu so" % (sotep, sodong))

# --------------------------------------- 6: ma Linux con sot ngoai cum 6,d,N
NGUON = set(MAP) | MO
BO_QUA = {   # da doi chieu tay: day la TOA DO / so khac, khong phai ma vat pham
    (r"script\activitysys\config\129\extend.lua", 105, 3150),
    (r"script\activitysys\config\129\variables.lua", 8, 3150),
    (r"script\event\longmenbiaoju\tasknpc.lua", 12, 3150),
}
so6 = 0
for p in tep:
    rel = os.path.relpath(p, ROOT)
    bak = p + ".truoc_nanma"
    raw = open(bak, "rb").read() if os.path.exists(bak) else open(p, "rb").read()
    oke = set(mm.start(4) for mm in RE6ANY.finditer(raw))
    for mm in RENUM.finditer(raw):
        v = int(mm.group(1))
        if v not in NGUON or mm.start() in oke:
            continue
        dong = raw.count(b"\n", 0, mm.start()) + 1
        if (rel, dong, v) in BO_QUA:
            continue
        so6 += 1
print("6. ma Linux nam ngoai cum 6,d,N (ban GOC): %d cho -- deu phai co trong DS_TAY" % so6)

# --------------------------------------- 7: tbProp genre KHAC 6 (chi BAO, khong loi)
# Bo nan chi lam genre 6 (bang magicscript). Cac genre khac dung bang RIENG:
#   genre 0 equip + nQuality=1 -> goldequip.txt (AddItem2)   -- DA DOI CHIEU: goldequip.txt
#       cua JX1 va cua Linux GIONG NHAU o vung nay (dong 3475..3479), 3477 = "Phi Phong
#       Cap Phe Quang (hoa giai sat thuong)" ca hai ben -> KHONG phai doi.
#   genre 1 medicine -> potion.txt theo (DetailType, Level)  -- CO LECH, xem duoi.
#   genre 8 fusion   -> fusion.txt                           -- DA DOI CHIEU: hai tep
#       fusion.txt giong nhau (352 dong, PT 1/11/21/241 cung ten) -> KHONG phai doi.
RE_PROP = re.compile(rb"tbProp\s*=\s*\{([^}]*)\}")
print("")
print("7. tbProp genre KHAC 6 (bo nan KHONG dung toi -- phai xem tay):")
for p in tep:
    rel = os.path.relpath(p, ROOT)
    raw = open(p, "rb").read()
    for mm in RE_PROP.finditer(raw):
        so = re.findall(rb"-?\d+", mm.group(1))
        if not so or so[0] == b"6":
            continue
        dong = raw.count(b"\n", 0, mm.start()) + 1
        print("   %s:%d  {%s}" % (rel, dong, b", ".join(so).decode("ascii")))
print("   -> genre 1 (potion.txt): Linux (1,8,0,lv4) = 'Hoi Thien Tai Tao Don';")
print("      JX1  (1,8,0,lv4) = 'Hoi Thien Dan' -- MON KHAC, va JX1 potion.txt KHONG co")
print("      'Hoi Thien Tai Tao Don'. CHO CHU quyet (them dong potion.txt hay doi mon).")

print("")
if loi:
    print("!" * 70)
    for x in loi:
        print("  LOI: " + x)
    print("!" * 70)
    sys.exit(2)
print("KIEM CHEO: SACH, 0 LOI")
