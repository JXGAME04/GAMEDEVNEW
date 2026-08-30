# -*- coding: utf-8 -*-
"""ktr_cauhinh.py - BO KIEM TRA CAU HINH (ban mau, chay THU tren du lieu HIEN TAI).

Muc dich: chung minh "chay phan tich TRUOC khi khoi dong" bat duoc loi that.
Cong cu nay KHONG SUA GI - chi doc va bao cao.

Sau nay khi co bang cauhinh moi (thongso.txt / lich.txt / thuong.txt), cung bo
kiem nay chay trong quy trinh: sua bang -> chay ktr -> sach thi moi nap.

7 phep kiem:
  K1  Khoa khai trong bang cau hinh ma KHONG script nao doc  (khoa chet)
  K2  Khoa script doc ma bang KHONG co                        (dua vao mac dinh am)
  K3  Cung mot con so khai o NHIEU noi                        (sua cho nay quen cho kia)
  K4  Ma vat pham trong bang thuong KHONG co trong bang du an (thuong vao hu khong)
  K5  Gio chay sai dinh dang / trung nhau
  K6  Gia tri ngoai khoang hop ly (cap, ti le, so luot)
  K7  Bang thuong: tong ti le va so o hanh trang

Chay:  python ktr_cauhinh.py            (bao cao day du)
       python ktr_cauhinh.py --gon      (chi dem, khong liet ke chi tiet)
"""
import io
import os
import re
import sys
import collections

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
SCRIPT = os.path.join(SV, "script")
CFG = os.path.join(SCRIPT, "header", "cauhinh_hoatdong.lua")
MS = os.path.join(SV, "settings", "item", "magicscript.txt")
RAC = (".truoc", ".bo_p", ".hong", ".bak", ".cu_", " - Copy", "Copy of", ".goc")

GON = "--gon" in sys.argv[1:]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1").replace("\r\n", "\n")


def moi_tep_lua():
    for tm, _, ts in os.walk(SCRIPT):
        for t in ts:
            if not t.lower().endswith(".lua"):
                continue
            if any(x in t for x in RAC):
                continue
            yield os.path.join(tm, t)


def in_muc(n, ten, so, ghi=""):
    print("\n[K%d] %s: %s%s" % (n, ten, so, ghi))


def main():
    print("=" * 74)
    print("BO KIEM TRA CAU HINH - chay tren du lieu HIEN TAI (khong sua gi)")
    print("=" * 74)

    # ---- nap bang cau hinh hien co ----
    if not os.path.isfile(CFG):
        print("!!! khong thay %s" % CFG)
        return 1
    tcfg = doc(CFG)
    khoa = {}          # ten khoa -> (dong, gia tri tho)
    mau = re.compile(r"^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(.+?),?\s*$")
    trong_bang = False
    for i, l in enumerate(tcfg.split("\n"), 1):
        s = l.strip()
        if s.startswith("tbCHD"):
            trong_bang = True
            continue
        if trong_bang and s == "}":
            trong_bang = False
            continue
        if not trong_bang or s.startswith("--") or not s:
            continue
        m = mau.match(l)
        if m:
            khoa[m.group(1)] = (i, m.group(2).rstrip(","))
    print("\nBang cau hinh: %d khoa (%s)" % (len(khoa), os.path.basename(CFG)))

    # ---- gom moi lan goi HD_CFG trong toan cay ----
    dung = collections.defaultdict(list)
    goi = re.compile(r'HD_CFG\s*\(\s*"([A-Za-z0-9_]+)"')
    truc_tiep = re.compile(r'tbCHD\s*\.\s*([A-Za-z0-9_]+)')
    for p in moi_tep_lua():
        if os.path.abspath(p) == os.path.abspath(CFG):
            continue
        d = doc(p)
        rel = os.path.relpath(p, SCRIPT)
        for i, l in enumerate(d.split("\n"), 1):
            if l.strip().startswith("--"):
                continue
            for m in goi.finditer(l):
                dung[m.group(1)].append((rel, i))
            for m in truc_tiep.finditer(l):
                dung[m.group(1)].append((rel, i))

    # ---- K1: khoa chet ----
    chet = sorted(k for k in khoa if k not in dung)
    in_muc(1, "Khoa khai ma khong script nao doc", "%d / %d" % (len(chet), len(khoa)))
    if not GON:
        for k in chet:
            print("      %-26s dong %-4d = %s" % (k, khoa[k][0], khoa[k][1][:28]))

    # ---- K2: script doc ma bang khong co ----
    thieu = sorted(k for k in dung if k not in khoa)
    in_muc(2, "Script doc khoa ma bang KHONG co", "%d" % len(thieu),
           "  (dang chay bang gia tri mac dinh trong script)")
    if not GON:
        for k in thieu[:14]:
            print("      %-26s vd %s:%d" % (k, dung[k][0][0][:44], dung[k][0][1]))
        if len(thieu) > 14:
            print("      ... con %d khoa nua" % (len(thieu) - 14))

    # ---- K3: con so khai nhieu noi ----
    # doi chieu mot so cap khoa/hang da biet chac
    doi = [
        ("YDBZ_CAP_TOITHIEU", r"YDBZ_LIMIT_PLAYER_LEVEL\s*=\s*(\d+)"),
        ("YDBZ_LAN_TUAN", r"YDBZ_LIMIT_WEEK_COUNT\s*=\s*(\d+)"),
        ("YDBZ_LAN_NGAY", r"YDBZ_LIMIT_DAY_COUNT\s*=\s*(\d+)"),
        ("YDBZ_DOI_MIN", r"YDBZ_TEAM_COUNT_LIMIT\s*=\s*(\d+)"),
        ("YDBZ_DOI_MAX", r"YDBZ_TEAM_COUNT_MAXLIMIT\s*=\s*(\d+)"),
        ("YDBZ_PHUT_TRAN", r"YDBZ_LIMIT_FINISH\s*=\s*(\d+)\s*\*\s*60"),
    ]
    trung = []
    kho_lua = {}
    for p in moi_tep_lua():
        kho_lua[os.path.relpath(p, SCRIPT)] = doc(p)
    for ten_khoa, mau_hang in doi:
        if ten_khoa not in khoa:
            continue
        gt_cfg = khoa[ten_khoa][1].strip()
        for rel, d in kho_lua.items():
            m = re.search(mau_hang, d)
            if m:
                gt_hang = m.group(1)
                dong = d[:m.start()].count("\n") + 1
                trung.append((ten_khoa, gt_cfg, rel, dong, gt_hang,
                              "KHAC NHAU" if gt_cfg != gt_hang else "trung khop"))
                break
    in_muc(3, "Con so khai o HAI noi", "%d cap" % len(trung))
    if not GON:
        for a, b, c, dg, e, f in trung:
            print("      %-22s cau hinh=%-6s | %s:%d = %-6s  %s"
                  % (a, b, c[:38], dg, e, f))

    # ---- K4: ma vat pham trong bang thuong ----
    # [SUA 29/08] Tra bang theo CHI SO DONG, khong theo cot ParticularType.
    # Ly do: KItemGenerator.CPP:1660 dat `const int i = nParticularType;` roi
    # GetMagicScript(i) -> KBasPropTbl.cpp:1058 tra ve `m_pBuf + i`, tuc PHAN TU
    # THU i. Cot ParticularType chi la du lieu, may KHONG tra theo no. Hai thu
    # nay dang LECH o 35 dong ke tu chi so 4881 (them dong vao giua bang la xo
    # lech toan bo phan sau), nen tra theo cot cho ket qua SAI o vung do.
    ms = {}
    n_lech = 0
    if os.path.isfile(MS):
        for k_dong, l in enumerate(doc(MS).split("\n")[1:]):
            c = [x.strip() for x in l.split("\t")]
            if len(c) > 3 and c[3].isdigit():
                # khoa theo CHI SO dong (dung cach may tra)
                ms[(c[1], c[2], str(k_dong))] = c[0]
                if int(c[3]) != k_dong:
                    n_lech += 1
    if n_lech:
        print("  !! magicscript.txt co %d dong cot ParticularType KHAC chi so"
              " dong." % n_lech)
        print("     May tra theo CHI SO, nen o vung do script va bang khong"
              " khop nhau.")
    la = []
    mau_item = re.compile(r"\{\s*6\s*,\s*([01])\s*,\s*(\d+)")
    # CHI xet dong THAT SU la bang vat pham: co tbProp / szName / AddItem /
    # DropItem / ConsumeItem / CalcItemCount ben canh. Bo qua bang du lieu khac
    # (vd player\meridian_data.lua co {6,0,1000,500,0} - KHONG phai ma vat pham).
    dau_hieu = ("tbProp", "szName", "AddItem", "DropItem", "ConsumeItem",
                "CalcItemCount", "GiveAward", "tbItem", "tbAward")
    for rel, d in kho_lua.items():
        for i, l in enumerate(d.split("\n"), 1):
            if l.strip().startswith("--"):
                continue
            if not any(x in l for x in dau_hieu):
                continue
            for m in mau_item.finditer(l):
                k = ("6", m.group(1), m.group(2))
                if k not in ms:
                    la.append((rel, i, "6,%s,%s" % (m.group(1), m.group(2))))
    in_muc(4, "Ma vat pham trong bang thuong KHONG co trong bang du an",
           "%d" % len(la))
    if not GON:
        for rel, i, ma in la[:12]:
            print("      %-46s:%-5d %s" % (rel[:46], i, ma))
        if len(la) > 12:
            print("      ... con %d" % (len(la) - 12))

    # ---- K5: gio chay ----
    loi_gio = []
    for k, (dg, v) in khoa.items():
        if "GIO" not in k:
            continue
        s = v.strip().strip('"')
        if re.match(r'^[\d,\s]+$', s) and "," in s:
            moc = [x.strip() for x in s.split(",") if x.strip()]
            xau = [x for x in moc if not (len(x) == 4 and x.isdigit()
                   and int(x[:2]) < 24 and int(x[2:]) < 60)]
            if xau:
                loi_gio.append((k, dg, "moc sai dinh dang: %s" % ", ".join(xau)))
            if len(set(moc)) != len(moc):
                loi_gio.append((k, dg, "co moc gio trung nhau"))
    in_muc(5, "Gio chay sai dinh dang hoac trung", "%d" % len(loi_gio))
    if not GON:
        for k, dg, ly in loi_gio:
            print("      %-26s dong %-4d %s" % (k, dg, ly))

    # ---- K6: gia tri ngoai khoang ----
    ngoai = []
    for k, (dg, v) in khoa.items():
        s = v.strip()
        if not re.match(r'^-?\d+$', s):
            continue
        n = int(s)
        if "CAP" in k and not (0 <= n <= 200):
            ngoai.append((k, dg, n, "cap phai trong 0..200"))
        if ("TI_LE" in k or "TILE" in k or "RATE" in k) and not (0 <= n <= 100):
            ngoai.append((k, dg, n, "ti le phai trong 0..100"))
        if ("LAN_" in k or "LUOT" in k) and not (0 <= n <= 1000):
            ngoai.append((k, dg, n, "so luot bat thuong"))
    in_muc(6, "Gia tri ngoai khoang hop ly", "%d" % len(ngoai))
    if not GON:
        for k, dg, n, ly in ngoai:
            print("      %-26s dong %-4d = %-6d %s" % (k, dg, n, ly))

    # ---- K7: bang thuong - tong ti le ----
    # CHI xet bang QUAY-CHON-MOT (di qua GivByRandom): chinh awardtemplet.lua:17-30
    # cong don nRate roi boc mot muc => tong < 100 la co luot khong nhan gi.
    # Bang "danh sach doc lap" (moi muc quay rieng) thi tong bao nhieu cung dung,
    # nen KHONG dua vao day - do la ly do ban dau bao nham npcwalk 4800%.
    tong_ti = []
    for rel, d in kho_lua.items():
        if "GivByRandom" not in d:
            continue
        r = [float(x) for x in re.findall(r"nRate\s*=\s*([\d.]+)", d)]
        if len(r) >= 2:
            t = sum(r)
            if t < 95 or t > 105:
                tong_ti.append((rel, len(r), t))
    in_muc(7, "Bang QUAY-CHON-MOT co tong ti le khac 100%", "%d bang" % len(tong_ti),
           "  (thieu thi co luot khong nhan gi)")
    if not GON:
        for rel, n, t in sorted(tong_ti, key=lambda x: -x[2])[:12]:
            print("      %-52s %2d muc, tong %.2f%%" % (rel[:52], n, t))
        if len(tong_ti) > 12:
            print("      ... con %d bang" % (len(tong_ti) - 12))

    print("\n" + "=" * 74)
    tong = (len(chet) + len(thieu) + len([x for x in trung if x[5] == "KHAC NHAU"])
            + len(la) + len(loi_gio) + len(ngoai))
    print("TONG: %d diem can xem lai (chua ke %d bang thuong khong tron 100%%)"
          % (tong, len(tong_ti)))
    print("=" * 74)
    return 0


if __name__ == "__main__":
    sys.exit(main())
