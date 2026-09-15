# -*- coding: utf-8 -*-
r"""[PTLOG 14/09] Doc mot phien nhat ky dien thoai va in ra bang tom tat - gop toan bo phep doc tay
cua phien do nhip ngay 14/09 (60 phut log Fold 7) thanh MOT lenh.

Dung:
    python android\phan_tich_log_fold7.py                 # phien moi nhat trong D:\jx1_android_log
    python android\phan_tich_log_fold7.py <thu muc phien>
    python android\phan_tich_log_fold7.py --thu-muc D:\jx1_android_data    # doc log may ao

In ra:
  1. Thiet bi / ban / do dai phien
  2. Suc khoe chung: fps, dien, nhiet, CPU/GPU, bo nho, khoi atlas
  3. Goi mang nang ([MANG-CHAM]) - GIAI MA ten goi tu Headers\KProtocolDef.h
  4. Khung giat ([VE-GIAT]) phan loai theo nguyen nhan: nap / trong lop ve / trinh chieu
  5. Vao map ([VAOMAP], [VAOMAP-MO], [VAOMAP-DONG]) - tam pha
  6. Ghep nen dat ([PGND-R]) va cua so giao dien nang nhat ([PDET-UI], co ten muc ini tu ban 109141701)
  7. Hoan ve / cho pak ([VE-TAI], [VE-NAP]) - dem luot bo ve
  8. Anh nap hong nhieu nhat (loc san cac tep KHONG CO tren ca cay PC, xem MEMORY may-ao-anh-chup...)

Khong sua gi, chi doc.
"""
import io
import os
import re
import sys
import glob
import collections

GOC = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
THU_NHAT_KY = r"D:\jx1_android_log"
HOP_MAC_DINH = (0, 250, 700, 560)
# cac tep .spr thieu tren CA cay PC -> khong phai loi dong goi dien thoai (14/09)
BO_QUA_HONG = ("regiontiledefault.spr", "minimap.spr")


def doc(p):
    try:
        return io.open(p, "r", encoding="latin-1", newline="").read()
    except Exception:
        return ""


def ten_goi():
    r"""Doc enum s2c_PROTOCOL trong Headers\KProtocolDef.h -> {so: ten}."""
    s = doc(os.path.join(GOC, "Headers", "KProtocolDef.h"))
    than = None
    for m in re.finditer(r"enum\s*(\w*)\s*\{(.*?)\}\s*;", s, re.S):
        if "s2c_multiserverbegin" in m.group(2) or "s2c_end" in m.group(2):
            than = m.group(2)
            break
    bang, gia = {}, -1
    if than:
        for dong in than.split("\n"):
            dong = re.sub(r"//.*", "", dong)
            dong = re.sub(r"/\*.*?\*/", "", dong)
            for t in dong.split(","):
                mm = re.match(r"^([A-Za-z_]\w*)\s*(?:=\s*([0-9xXa-fA-F]+))?$", t.strip())
                if not mm:
                    continue
                gia = int(mm.group(2), 0) if mm.group(2) else gia + 1
                bang.setdefault(gia, mm.group(1))
    return bang


def muc(ten):
    print("\n" + ten)
    print("-" * len(ten))


def main():
    ds = [a for a in sys.argv[1:] if not a.startswith("--")]
    if "--thu-muc" in sys.argv:
        thu = sys.argv[sys.argv.index("--thu-muc") + 1]
    elif ds:
        thu = ds[0]
    else:
        cac = sorted(glob.glob(os.path.join(THU_NHAT_KY, "*")), key=os.path.getmtime, reverse=True)
        cac = [c for c in cac if os.path.isdir(c)]
        if not cac:
            raise SystemExit("khong thay phien nao trong %s" % THU_NHAT_KY)
        thu = cac[0]
    rep3 = doc(os.path.join(thu, "jx_rep3.log"))
    paint = doc(os.path.join(thu, "jx_paint.log"))
    thiet = doc(os.path.join(thu, "jx_thietbi.log"))
    if not rep3 and not paint:
        raise SystemExit("khong doc duoc jx_rep3.log / jx_paint.log trong %s" % thu)

    print("=" * 78)
    print("PHIEN: %s" % thu)
    m = re.search(r"\[THIETBI\][^\n]*may=([^\(]+)\(([^\)]*)\)[^\n]*soc=([^\s]+(?:\s+\S+)?)\s+android=(\d+)", thiet)
    if m:
        print("May: %s(%s) | SoC %s | Android %s" % (m.group(1).strip(), m.group(2), m.group(3), m.group(4)))
    m = re.search(r"\[GPU\] man hinh (\d+x\d+)@(\d+)", rep3)
    if m:
        print("Man hinh: %s @ %s Hz" % (m.group(1), m.group(2)))
    m = re.search(r"backbuffer (\d+x\d+)", rep3)
    if m:
        print("Backbuffer: %s" % m.group(1))
    t = re.findall(r"^\[(\d+)\]", rep3, re.M)
    if t:
        print("Do dai: %.0f phut (%s dong rep3, %s dong paint)" % (int(t[-1]) / 60000.0, rep3.count("\n"), paint.count("\n")))
    co = [k for k, v in (("[VAOMAP-DONG]", "[VAOMAP-DONG]"), ("[VAOMAP-MO]", "[VAOMAP-MO]"), ("[VAOMAP]", "[VAOMAP]"),
                         ("MapList dung lai", "MapList dung lai"), ("muc ini [", "muc ini ["),
                         ("[SOLUONG]", "[SOLUONG]"), ("[WACHON]", "[WACHON]")) if v in rep3 or v in paint]
    if co:
        print("Dau hieu ban: %s" % ", ".join(co))

    # ---- 2. suc khoe chung
    muc("1. SUC KHOE CHUNG")
    fps = re.findall(r"fps TB (\d+)", rep3)
    if fps:
        print("fps TB (cua so cuoi): %s | trung binh ca phien: %.0f" % (fps[-1], sum(int(x) for x in fps) / len(fps)))
    m = re.findall(r"cpu tien trinh (\d+) %[^|]*luong chinh (\d+) %", rep3)
    if m:
        print("CPU tien trinh: %s %% (luong chinh %s %%)" % (m[-1][0], m[-1][1]))
    mau = re.findall(r"nhiet=(\d+).*?pin=(\d+)% ([\d.]+)C.*?p=([\d.]+) W \| gpu=(\d+)%", thiet)
    if mau:
        p = [float(x[3]) for x in mau]
        g = [int(x[4]) for x in mau]
        print("Dien: %.2f W TB (min %.2f, max %.2f) | GPU %d %% TB (max %d)" % (sum(p) / len(p), min(p), max(p), sum(g) // len(g), max(g)))
        print("Pin: %s %% -> %s %% | nhiet %s -> %s do C | muc nhiet max %s" % (mau[0][1], mau[-1][1], mau[0][2], mau[-1][2], max(int(x[0]) for x in mau)))
    m = re.findall(r"RAM rieng (\d+) MB.*?texture (\d+) MB.*?gpu tex \d+ \((\d+) MB", rep3)
    if m:
        print("RAM rieng %s MB | cache texture %s MB | texture GPU %s MB" % m[-1])
    k = re.findall(r"tong (\d+) khoi, (\d+) MB", rep3)
    if k:
        print("Khoi atlas: %s khoi, %s MB (khoi tao GIUA game: %d lan)" % (k[-1][0], k[-1][1], rep3.count("[KHOI] khoi atlas moi")))

    # ---- 3. goi mang nang
    muc("2. GOI MANG NANG ([MANG-CHAM], >= 8 ms)")
    bang = ten_goi()
    r = re.findall(r"\[MANG-CHAM\] t=(\d+) msg=(\d+) ([\d.]+) ms", paint)
    if r:
        tong, dem, lon = collections.defaultdict(float), collections.Counter(), collections.defaultdict(float)
        for _, mid, ms in r:
            ms = float(ms)
            tong[mid] += ms
            dem[mid] += 1
            lon[mid] = max(lon[mid], ms)
        print("%-28s %5s %9s %8s" % ("goi", "lan", "tong ms", "max ms"))
        for mid, _ in sorted(tong.items(), key=lambda kv: -kv[1]):
            print("%-28s %5d %9.0f %8.1f" % (bang.get(int(mid), "msg %s" % mid), dem[mid], tong[mid], lon[mid]))
        print("Tong: %.0f ms trong %d goi nang" % (sum(tong.values()), sum(dem.values())))
        c = re.findall(r"\[MANG-CAT\].*?tong cat (\d+)", paint)
        if c:
            print("Ngan sach mang cat vong lap: %s lan" % c[-1])
    else:
        print("(khong co dong nao)")

    # ---- 4. khung giat
    muc("3. KHUNG GIAT ([VE-GIAT])")
    g = re.findall(r"\[VE-GIAT\] khung \d+: ([\d.]+) ms.*?ve CPU ([\d.]+) \[trong lop ve ([\d.]+).*?\(nap ([\d.]+) ms.*?\+ trinh chieu ([\d.]+)", rep3)
    if g:
        n_nap = sum(1 for x in g if float(x[3]) >= 10)
        n_lop = sum(1 for x in g if float(x[2]) >= 20)
        n_tc = sum(1 for x in g if float(x[4]) >= 10)
        tong = sorted((float(x[0]) for x in g), reverse=True)
        print("%d khung giat, tong %.0f ms" % (len(g), sum(tong)))
        print("  do NAP >= 10 ms          : %d" % n_nap)
        print("  do VE (trong lop) >= 20  : %d" % n_lop)
        print("  do TRINH CHIEU >= 10 ms  : %d  (cho dong bo man hinh, thuong vo hai)" % n_tc)
        print("  5 khung to nhat: %s ms" % ", ".join("%.0f" % x for x in tong[:5]))
    else:
        print("(khong co dong nao)")

    # ---- 5. vao map
    muc("4. VAO MAP ([VAOMAP] / [VAOMAP-MO] / [VAOMAP-DONG])")
    v = re.findall(r"\[VAOMAP\] map (\d+) vung \((\d+),(\d+)\) map_moi=(\d+): tong ([\d.]+) ms = dong ([\d.]+) \+ mo map ([\d.]+) \+ ini ([\d.]+) \+ vung giua ([\d.]+) \+ 8 vung ke ([\d.]+) \+ trang tri ([\d.]+) \+ noi vung ([\d.]+) \+ luoi duong ([\d.]+)", paint)
    if v:
        print("%d lan doi map / cuon vung >= 30 ms" % len(v))
        ten = ["dong", "mo map", "ini", "vung giua", "8 vung ke", "trang tri", "noi vung", "luoi duong"]
        tongp = [0.0] * 8
        for x in v:
            for i in range(8):
                tongp[i] += float(x[5 + i])
        tt = sum(float(x[4]) for x in v)
        print("Tong %.0f ms, TB %.0f ms/lan, nang nhat %.0f ms" % (tt, tt / len(v), max(float(x[4]) for x in v)))
        for i, t2 in sorted(enumerate(tongp), key=lambda kv: -kv[1]):
            if t2 > 0:
                print("   %-12s %7.0f ms (%4.1f %%)" % (ten[i], t2, 100.0 * t2 / max(tt, 1e-9)))
    else:
        print("(khong co dong [VAOMAP] - ban truoc 109141701)")
    for nhan in ("[VAOMAP-MO]", "[VAOMAP-DONG]"):
        d = [l for l in paint.split("\n") if nhan in l]
        if d:
            print("\n%s: %d dong, ba dong nang nhat:" % (nhan, len(d)))
            for l in sorted(d, key=lambda l: -float(re.search(r"tong ([\d.]+)", l).group(1) if re.search(r"tong ([\d.]+)", l) else 0))[:3]:
                print("   " + l.strip()[:200])

    # ---- 6. nen dat + giao dien
    muc("5. GHEP NEN DAT ([PGND-R]) VA CUA SO GIAO DIEN ([PDET-UI])")
    r = re.findall(r"\[PGND-R\] vung \((\d+),(\d+)\)[^:]*: tong ([\d.]+) ms", paint)
    if r:
        ms = sorted((float(x[2]) for x in r), reverse=True)
        print("Ghep nen: %d lan >= 3 ms, tong %.0f ms, nang nhat %s ms" % (len(r), sum(ms), ", ".join("%.0f" % x for x in ms[:3])))
    u = [l.strip() for l in paint.split("\n") if "[PDET-UI]" in l]
    if u:
        print("Cua so nang nhat: %d dong" % len(u))
        for l in sorted(u, key=lambda l: -float(re.search(r"nhat ([\d.]+) ms", l).group(1) if re.search(r"nhat ([\d.]+) ms", l) else 0))[:4]:
            print("   " + l[:190])

    # ---- 7. hoan ve
    muc("6. HOAN VE / CHO PAK")
    b = re.findall(r"bo ve (\d+) luot, dong bo trong ngan sach (\d+)", rep3)
    if b:
        print("bo ve (khung sprite tre mot khung): %d luot | nap dong bo trong ngan sach: %d" % (sum(int(x) for x, _ in b), sum(int(y) for _, y in b)))
    p = [int(x) for x in re.findall(r"pak ban giao nen: (\d+)", rep3)]
    if p:
        print("pak ban giao nen: %d luot, don nhat %d luot/cua so 30 s" % (sum(p), max(p)))
    t2 = [int(x) for x in re.findall(r"khung to \(>= \d+ KB\) giao nen thay vi rut dong bo: (\d+)", rep3)]
    if t2:
        print("khung to giao nen: %d" % sum(t2))

    # ---- 8. anh hong
    muc("7. ANH NAP HONG (da bo cac tep thieu tren ca cay PC)")
    h = collections.Counter()
    for m2 in re.findall(r"LoadImage FAIL[^:]*: (.+)", rep3):
        ten2 = m2.strip().split("\\")[-1].lower()
        if ten2 in BO_QUA_HONG:
            continue
        h[m2.strip()[:70]] += 1
    if h:
        for k2, v2 in h.most_common(8):
            print("   %4d  %s" % (v2, k2))
    else:
        print("   (khong co, hoac chi con cac tep thieu san tren PC)")
    print()


if __name__ == "__main__":
    main()
