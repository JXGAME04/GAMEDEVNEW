#!/usr/bin/env python3
# -*- coding: latin-1 -*-
"""sinh_cfgw_meta.py - [CFGW 04/09] SINH TU DONG bang mo ta khoa cau hinh cho trang web admin.

Doc cac tep cau hinh Lua (script\cauhinh\ch_*.lua + script\header\cauhinh_hoatdong.lua) bang
latin-1 (giu nguyen byte TCVN3), lay: ten khoa, gia tri, chu thich (cung dong hoac khoi chu thich
ngay tren), suy ra NHOM / KIEU / KHOANG / AP_DUNG, quet ca cay script xem khoa duoc doc o dau
(trong than ham = an ngay; chi o cap tep = can nap lai script / restart) roi ghi:

    <script>\cauhinh_web\cfgw_meta.lua      tbCFGW_META = { KHOA = {"NHOM","mo ta",kieu,min,max,ap,"nguon"}, ... }
    <ReverseTools>\cauhinh_web\cfgw_meta_baocao.txt   thong ke + danh sach khoa can restart

Chay:  python sinh_cfgw_meta.py <thu muc script may chu>
Vi du: python sinh_cfgw_meta.py "E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
"""
import io, os, re, sys

if len(sys.argv) < 2:
    sys.exit(__doc__)
S = sys.argv[1]
OUT_LUA = os.path.join(S, "cauhinh_web", "cfgw_meta.lua")
OUT_BC = os.path.join(os.path.dirname(os.path.abspath(__file__)), "cfgw_meta_baocao.txt")

NGUON = [
    ("cauhinh/ch_chung.lua", "CHUNG"),
    ("cauhinh/ch_lich.lua", "LICH"),
    ("cauhinh/ch_thuong.lua", "THUONG"),
    ("cauhinh/ch_exp.lua", "EXP"),
    ("cauhinh/ch_drop.lua", "ROTDO"),
    ("header/cauhinh_hoatdong.lua", "HOATDONG"),
]

# tien to -> nhom hien thi tren web
NHOM_TIENTO = [
    ("GLB_TILE_EXP", "EXP"), ("GLB_TILE_TIEN", "TIEN"),
    ("GLB_CHE_DO_TEST", "HETHONG"), ("GLB_GIO_MO_SERVER", "HETHONG"), ("CH_NAPLAI_PHUT", "HETHONG"),
    ("GLB_", "CHUNG"), ("CH_", "HETHONG"),
    ("DT_", "DATAU"), ("BDH_", "BANDONGHANH"), ("BHK_", "BOSS"),
    ("CTC_", "CONGTHANH"), ("CTLD_", "LOIDAI"),
    ("DRQ_", "ROTDO"), ("SKD_", "ROTDO"), ("BRXP_", "EXP"),
    ("TKT_", "TONGKIM"), ("VT_", "VANTIEU"), ("TBH_", "TRONGBANG"),
    ("TW_", "BANGCHIEN"), ("BR_", "BACHNHAN"), ("BW_", "TYVO"), ("TC_", "THANHBAO"),
    ("YDBZ_", "VIEMDE"), ("TS_", "THANHBAO"), ("BAT_", "HOATDONG"),
]


DONVI_TU = [("_PHUT", "phut"), ("PHUT_", "phut"), ("_GIAY", "giay"), ("GIAY_", "giay"), ("_GIO_", "gio"), ("_GIO", "gio"),
            ("TILE", "x"), ("HESO", "x"), ("_RATE", "x"), ("PHANTRAM", "%"), ("_LAN", "lan"), ("LAN_", "lan"),
            ("_EXP", "exp"), ("EXP_", "exp"), ("_TIEN", "luong"), ("PHI_", "luong"), ("GIA_", "luong"), ("_XU", "xu"),
            ("_DIEM", "diem"), ("DIEM_", "diem"), ("_MAU", "mau"), ("MAU_", "mau"), ("_CAP", "cap"), ("CAP_", "cap"),
            ("_MAP", "map"), ("_O_", "o"), ("_NGUOI", "nguoi"), ("NGUOI_", "nguoi"), ("_MANG", "mang"), ("_NGAY", "ngay"), ("NGAY_", "ngay")]


def don_vi_cua(k, kieu):
    if kieu in (2, 3, 4):
        return "" if kieu != 4 else "HHMM"
    for tt, dv in DONVI_TU:
        if tt in k:
            return dv
    if k.endswith("_O"):
        return "o"
    return ""

KEY_RE = re.compile(r'^\s*([A-Z][A-Z0-9_]+)\s*=\s*(.+?)\s*,?\s*(?:--\s*(.*))?$')


def nhom_cua(k, macdinh):
    for tt, nh in NHOM_TIENTO:
        if k.startswith(tt) or k == tt:
            return nh
    return macdinh


def lua_str(s):
    s = s.replace("\\", "\\\\").replace('"', '\\"')
    return '"' + s + '"'


def quet_su_dung(khoa):
    """Tra ve (so_lan_trong_ham, so_lan_cap_tep) cho mot khoa tren ca cay script."""
    return CACHE_SD.get(khoa, (0, 0))


# --- quet ca cay script MOT lan: tim moi cho goi X_CFG("KHOA" / X_CFG_BUOC("KHOA" ----------------
CACHE_SD = {}
GOI_RE = re.compile(r'_CFG(?:_BUOC)?\(\s*"([A-Z][A-Z0-9_]+)"')
for goc, thumuc, tep in os.walk(S):
    for f in tep:
        if not f.endswith(".lua"):
            continue
        p = os.path.join(goc, f)
        try:
            t = io.open(p, "r", encoding="latin-1").read()
        except Exception:
            continue
        for line in t.split("\n"):
            for m in GOI_RE.finditer(line):
                k = m.group(1)
                trong_ham = line[:1] in (" ", "\t")   # co thut dau dong = trong than ham/khoi
                a, b = CACHE_SD.get(k, (0, 0))
                CACHE_SD[k] = (a + (1 if trong_ham else 0), b + (0 if trong_ham else 1))

meta = []      # (khoa, nhom, mota, kieu, mn, mx, ap, nguon)
bo_qua = []    # (khoa, ly do)
for rel, nhom_tep in NGUON:
    p = os.path.join(S, rel)
    if not os.path.exists(p):
        print("KHONG CO", p)
        continue
    L = io.open(p, "r", encoding="latin-1").read().split("\n")
    cho_chuthich = []          # khoi chu thich ngay tren (reset khi gap dong trong)
    trong_bang = False
    for line in L:
        s = line.rstrip("\r")
        st = s.strip()
        if st.startswith("tb") and "=" in st and st.endswith("{"):
            trong_bang = True
            cho_chuthich = []
            continue
        if not trong_bang:
            continue
        if st == "}":
            trong_bang = False
            continue
        if st == "":
            cho_chuthich = []
            continue
        if st.startswith("--"):
            cho_chuthich.append(st[2:].strip())
            continue
        m = KEY_RE.match(s)
        if not m:
            continue
        k, val, cm = m.group(1), m.group(2).strip(), (m.group(3) or "").strip()
        if val.startswith("{"):
            bo_qua.append((k, "bang con"))
            continue
        if val == "nil":
            bo_qua.append((k, "nil"))
            continue
        kieu = 0
        if val.startswith('"') or val.startswith("'"):
            kieu = 2
        else:
            try:
                fv = float(val)
                if "." in val:
                    kieu = 1
            except ValueError:
                bo_qua.append((k, "gia tri khong phai so/chuoi: " + val[:30]))
                continue
        mota = cm if cm else " ".join(x for x in cho_chuthich if not x.startswith("=") and not x.startswith("["))
        mota = re.sub(r"\s+", " ", mota).strip()
        if len(mota) > 220:
            mota = mota[:217] + "..."
        nhom = nhom_cua(k, nhom_tep)
        tag = (cm + " " + " ".join(cho_chuthich)).upper()
        # kieu dac biet
        mn = mx = 0
        if kieu == 0:
            if "_GIO_" in k or k.endswith("_GIO"):
                kieu = 4
                mn, mx = 0, 2359
            elif k.startswith("BAT_") or k.endswith("_BAT") or k.endswith("_BAT_TAT") or (val in ("0", "1") and re.search(r"\b(BAT|TAT|1 =|0 =)\b", tag)):
                kieu = 3
                mn, mx = 0, 1
        # ap dung
        if nhom == "HETHONG":
            ap = 2
        elif "[RESTART]" in tag:
            ap = 2
        elif "[LIVE]" in tag:
            ap = 1
        else:
            a, b = quet_su_dung(k)
            if a > 0:
                ap = 1          # co cho doc trong than ham -> an ngay o cho do
            elif b > 0:
                ap = 2          # chi doc o cap tep (hang so chot luc nap script)
            else:
                ap = 1          # khong thay cho doc (khoa cua driver/HD_CFG o tick) -> coi la <= 1 phut
        meta.append((k, nhom, mota, kieu, mn, mx, ap, os.path.basename(rel), don_vi_cua(k, kieu)))

# --- ghi cfgw_meta.lua (latin-1: giu nguyen byte TCVN3 trong mo ta) --------------------------------
os.makedirs(os.path.dirname(OUT_LUA), exist_ok=True)
out = []
out.append("-- ============================================================================")
out.append("-- CFGW_META.LUA - SINH TU DONG boi ReverseTools\\cauhinh_web\\sinh_cfgw_meta.py - DUNG SUA TAY")
out.append("-- [CFGW 04/09] mo ta / nhom / kieu / khoang / ap_dung cua tung khoa cau hinh, de cfgw_driver.lua")
out.append("-- khai len bang MySQL gcfg (web admin doc). Khoa = { nhom, mo ta, kieu, min, max, ap_dung, nguon, don_vi }")
out.append("--   kieu: 0 so nguyen | 1 so thuc | 2 chuoi | 3 bat/tat | 4 gio HHMM")
out.append("--   ap_dung: 1 = an trong <= 1 phut (G_CFG doc song / timerserver tu nap lai) | 2 = can nap lai script hoac restart")
out.append("-- TEP NAY LA 'LA': CAM Include bat cu gi vao day.")
out.append("-- ============================================================================")
out.append("tbCFGW_META = {")
for k, nhom, mota, kieu, mn, mx, ap, nguon, donvi in meta:
    out.append("%s = {%s, %s, %d, %d, %d, %d, %s, %s}," % (k, lua_str(nhom), lua_str(mota), kieu, mn, mx, ap, lua_str(nguon), lua_str(donvi)))
out.append("}")
io.open(OUT_LUA, "w", encoding="latin-1", newline="\r\n").write("\n".join(out) + "\n")

# --- bao cao ----------------------------------------------------------------------------------------
bc = []
bc.append("cfgw_meta: %d khoa khai, %d bo qua" % (len(meta), len(bo_qua)))
tk = {}
for k, nhom, mota, kieu, mn, mx, ap, nguon, donvi in meta:
    tk.setdefault(nhom, [0, 0, 0])
    tk[nhom][0] += 1
    tk[nhom][ap] += 0 if ap not in (1, 2) else 0
    if ap == 1:
        tk[nhom][1] += 1
    elif ap == 2:
        tk[nhom][2] += 1
bc.append("%-14s %5s %8s %8s" % ("NHOM", "khoa", "ap<=1ph", "restart"))
for nhom in sorted(tk):
    bc.append("%-14s %5d %8d %8d" % (nhom, tk[nhom][0], tk[nhom][1], tk[nhom][2]))
bc.append("")
bc.append("KHOA CAN NAP LAI SCRIPT / RESTART (ap_dung = 2):")
for k, nhom, mota, kieu, mn, mx, ap, nguon, donvi in meta:
    if ap == 2:
        bc.append("  %-30s %-12s %s" % (k, nhom, nguon))
bc.append("")
bc.append("BO QUA (khong dua len web):")
for k, ly in bo_qua:
    bc.append("  %-30s %s" % (k, ly))
io.open(OUT_BC, "w", encoding="latin-1").write("\n".join(bc) + "\n")
print("\n".join(bc[:len(tk) + 2]))
print("da ghi", OUT_LUA, "va", OUT_BC)
