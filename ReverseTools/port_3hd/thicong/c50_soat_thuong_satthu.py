# -*- coding: utf-8 -*-
"""C50 - chu game: "moi lan danh boss sat thu se nhan duoc gi? toi thay co mot so
lenh bai ma kich dung khong duoc".

Soat TOAN BO vat pham trong chuoi thuong giet boss sat thu:
  kill_level.lua  OnDeath -> kill_level20 -> SetMemberTask -> add_shashouling
                                          -> HD3_ST_ThuongBoss
                  EventSys "KillerBoss" OnFinish -> OnFinishKillerTask
  + bang roi mat dat settings\\droprate\\boss\\bosstask_lev90.ini

Voi tung ma bao cao:
  - co trong bang item JX1 khong (server VA client - hai tep RIENG)
  - ten JX1  (giai ma TCVN3) va ten ban Linux de doi chieu
  - cot Script: 0 = BAM CHUOT PHAI KHONG CO TAC DUNG GI
  - cot nMaxStack
  - co nam trong danh sach trang cua engine sIsJx2ItemScript khong
    (KItemList.cpp - engine chi TU TRU item khi script nam trong danh sach nay)
"""
import io, os, re, sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

_TB = {}
for cp in range(0x20, 0x2000):
    ch = chr(cp)
    try:
        b = unicode_to_tcvn3_bytes(ch)
    except Exception:
        continue
    if len(b) == 1 and b[0] >= 0x80:
        _TB.setdefault(b[0], ch)
tcvn = lambda s: "".join(_TB.get(ord(c), c) for c in s)

import importlib.util
spec = importlib.util.spec_from_file_location(
    "dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py")
dec2 = importlib.util.module_from_spec(spec)
spec.loader.exec_module(dec2)

SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CLI = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
LNX = r"D:\ServerLinux\server1"
CORE = r"D:\GAMEDEVNEW\Sources\Core\Src"


def bang(p, giaima):
    """(genre,detail,particular) -> (ten, script, stack, dong)"""
    tb = {}
    if not os.path.exists(p):
        return tb
    raw = io.open(p, encoding="latin-1", newline="").read().split("\n")
    for i, l in enumerate(raw, 1):
        c = l.split("\t")
        if len(c) > 10 and c[1].isdigit() and c[2].lstrip("-").isdigit() and c[3].lstrip("-").isdigit():
            ten = giaima(c[0])
            tb.setdefault((c[1], c[2], c[3]),
                          (ten, c[9], c[12] if len(c) > 12 else "?", i))
    return tb


def bang_lnx(p):
    tb = {}
    if not os.path.exists(p):
        return tb
    for l in open(p, "rb").read().split(b"\n"):
        c = dec2.decline2(l.rstrip(b"\r")).split("\t")
        if len(c) > 10 and c[1].isdigit() and c[2].lstrip("-").isdigit() and c[3].lstrip("-").isdigit():
            tb.setdefault((c[1], c[2], c[3]), (c[0], c[9]))
    return tb


JX = bang(os.path.join(SRV, r"settings\item\magicscript.txt"), tcvn)
CL = bang(os.path.join(CLI, r"settings\item\magicscript.txt"), tcvn)
LX = bang_lnx(os.path.join(LNX, r"settings\item\004\magicscript.txt"))

# danh sach trang cua engine: script nao thi engine TU TRU item
wl = set()
p = os.path.join(CORE, "KItemList.cpp")
d = io.open(p, encoding="latin-1", newline="").read()
m = re.search(r"szJx2\s*\[\s*\]\s*=\s*\{(.*?)\}\s*;", d, re.S)
if m:
    for s in re.findall(r'"([^"]+)"', m.group(1)):
        wl.add(s.lower().replace("\\\\", "\\"))
print("Danh sach trang sIsJx2ItemScript cua engine: %d muc\n" % len(wl))

# ---------------------------------------------------------------- vat pham can soat
MUC = [
    # (nhom, genre, detail, particular, mo ta lay tu dau)
    ("MOI LAN GIET BOSS (moi nhom cap)", 6, 1, 398,
     "Sat thu lenh - lib_killlevel.lua:99 AddItem(6,1,398,nCapNhom,nSeries,0,0)"),
    ("CHI NHOM CAP 90", 6, 1, 2356,
     "Sat Thu Bi Bao - lib_killlevel.lua:130, ti le 50%"),
    ("CHI NHOM CAP 90 (bang thuong OnFinishKillerTask)", 6, 1, 4429, "Tinh Tinh Khoang 1.04%"),
    ("CHI NHOM CAP 90 (bang thuong OnFinishKillerTask)", 6, 1, 4428, "Tinh Thiet Khoang 0.52%"),
    ("CHI NHOM CAP 90 (bang thuong OnFinishKillerTask)", 6, 1, 4813, "Tui Duoc Pham 68.26%"),
    ("CHI NHOM CAP 90 (bang thuong OnFinishKillerTask)", 6, 1, 3, "Dai Luc hoan 15% (DA VA C51: Linux 6,0,3 -> JX1 6,1,3)"),
    ("CHI NHOM CAP 90 (bang thuong OnFinishKillerTask)", 6, 1, 6, "Phi Toc hoan 15% (DA VA C51: Linux 6,0,6 -> JX1 6,1,6)"),
    ("CHI NHOM CAP 90 (bang thuong OnFinishKillerTask)", 6, 1, 30528, "Do Pho Dang Long Khoi 0.0003%"),
    ("CHI NHOM CAP 90 (bang thuong OnFinishKillerTask)", 6, 1, 30530, "Do Pho Dang Long Hai 0.0003%"),
    ("CHI NHOM CAP 90 (bang thuong OnFinishKillerTask)", 6, 1, 30531, "Do Pho Dang Long Yeu Dai 0.0003%"),
    ("CHI NHOM CAP 90 (bang thuong OnFinishKillerTask)", 6, 1, 30532, "Do Pho Dang Long Ho Uyen 0.0003%"),
    ("CHI NHOM CAP 90 (bang thuong OnFinishKillerTask)", 6, 1, 30533, "Do Pho Dang Long Hang Lien 0.0003%"),
    ("CHI NHOM CAP 90 (bang thuong OnFinishKillerTask)", 6, 1, 30534, "Do Pho Dang Long Boi 0.0003%"),
    ("CHI NHOM CAP 90 (bang thuong OnFinishKillerTask)", 6, 1, 30535, "Do Pho Dang Long Thuong Gioi Chi 0.0003%"),
    ("CHI NHOM CAP 90 (bang thuong OnFinishKillerTask)", 6, 1, 30536, "Do Pho Dang Long Ha Gioi Chi 0.0003%"),
    ("CHI NHOM CAP 90 (bang thuong OnFinishKillerTask)", 6, 1, 30538, "Dang Long Thach - Ho 0.16%"),
    ("CHI NHOM CAP 90 (Bac Dau)", 6, 1, 4138, "Lenh bai Bac Dau - Boss sat thu"),
    ("CHI NHOM CAP 90 (Bac Dau, khoa QUYDOI)", 6, 1, 4126, "Lenh bai Bac Dau (doi thuong)"),
    ("SU KIEN 2011 DA HET HAN - KHONG BAO GIO RA", 6, 1, 1827, "Huy Chuong Chien Cong"),
    ("SU KIEN 2011 DA HET HAN - KHONG BAO GIO RA", 6, 1, 2183, "Cay But"),
    ("SU KIEN 2011 DA HET HAN - KHONG BAO GIO RA", 6, 1, 3444, "Phu Hieu"),
    ("GHEP TU 5 SAT THU LENH (NPC Nhiep Thi Tran)", 6, 1, 399, "Sat thu gian"),
]

# them cac muc trong bang roi mat dat cua boss cap 90
ini = os.path.join(SRV, r"settings\droprate\boss\bosstask_lev90.ini")
roi = []
if os.path.exists(ini):
    sec, cur = None, {}
    dd = io.open(ini, encoding="latin-1", newline="").read().split("\n")
    rr = 0
    cnt = 0
    for l in dd:
        s = l.strip()
        mm = re.match(r"RandRange=(\d+)", s)
        if mm:
            rr = int(mm.group(1))
        mm = re.match(r"^Count=(\d+)", s)
        if mm:
            cnt = int(mm.group(1))
        if s.startswith("[") and s.endswith("]"):
            if sec and cur.get("Genre"):
                roi.append((sec, cur))
            sec, cur = s[1:-1], {}
        elif "=" in s and sec:
            k, v = s.split("=", 1)
            cur[k.strip()] = v.strip()
    if sec and cur.get("Genre"):
        roi.append((sec, cur))

print("=" * 100)
print("BANG A - VAT PHAM TRAO THANG VAO TUI")
print("=" * 100)
print("%-13s %-34s %-6s %-6s %s" % ("ma", "ten trong bang item JX1", "script", "stack", "ket luan"))
print("-" * 100)
nhom_cu = None
for nhom, g, d2, p2, mota in MUC:
    if nhom != nhom_cu:
        print("\n### %s" % nhom)
        nhom_cu = nhom
    k = (str(g), str(d2), str(p2))
    j = JX.get(k)
    c = CL.get(k)
    lx = LX.get(k)
    ma = "%d,%d,%d" % (g, d2, p2)
    if j is None:
        print("  %-13s %-34s %-6s %-6s <== JX1 KHONG CO MA NAY -> AddItem/Give IM LANG KHONG RA GI"
              % (ma, "(khong co)", "-", "-"))
        if lx:
            print("  %-13s     ban Linux la: %s" % ("", lx[0][:60]))
        continue
    ten, sc, stack, dong = j
    kl = []
    if sc in ("0", "", "-"):
        kl.append("BAM CHUOT PHAI KHONG LAM GI (cot Script = 0)")
    else:
        s = sc.lower().replace("\\\\", "\\")
        if not os.path.exists(os.path.join(SRV, s.lstrip("\\"))):
            kl.append("TRO TOI SCRIPT KHONG TON TAI: " + sc)
        elif s not in wl:
            kl.append("co script nhung KHONG trong danh sach trang -> engine KHONG tu tru item")
        else:
            kl.append("dung duoc")
    if c is None:
        kl.append("CLIENT KHONG CO DONG NAY (o do trang)")
    elif c[0] != ten:
        kl.append("CLIENT ten khac: " + c[0][:24])
    if lx and lx[0].strip() != ten.strip():
        kl.append("Linux la '%s'" % lx[0][:26])
    print("  %-13s %-34s %-6s %-6s %s" % (ma, ten[:34], sc if sc != "0" else "0", stack, " | ".join(kl)))
    print("  %-13s     nguon: %s" % ("", mota))

print()
print("=" * 100)
print("BANG B - ROI XUONG DAT (chi boss NHOM CAP 90) - %s" % ini)
print("=" * 100)
if not roi:
    print("  (khong doc duoc tep bang roi)")
else:
    print("  [Main] Count=%d  RandRange=%d" % (cnt, rr))
    tong = 0
    for sec, cur in roi:
        try:
            rate = int(re.sub(r"[^0-9-]", "", cur.get("RandRate", "0")) or 0)
        except Exception:
            rate = 0
        tong += rate
        g = cur.get("Genre", "0")
        d2 = cur.get("Detail", "0")
        p2 = re.sub(r"[^0-9-]", "", cur.get("Particular", "0")) or "0"
        k = (g, d2, p2)
        j = JX.get(k)
        ten = j[0] if j else "(JX1 KHONG CO MA NAY)"
        if g == "0":
            ten = "(trang bi - bang goldequip/platinaequip)"
        if rate > 0:
            print("  [%-3s] %s,%s,%-6s %-36s ti le %7.4f%%" %
                  (sec, g, d2, p2, ten[:36], rate * 100.0 / rr if rr else 0))
    print("  --> tong ti le ra do: %.3f%%  (con lai %.3f%% la khong roi gi)"
          % (tong * 100.0 / rr, (rr - tong) * 100.0 / rr))
