#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""sinh_cfgw_meta.py - [CFGW 04/09, 05/09] SINH TU DONG bang mo ta khoa cau hinh cho trang web admin.

Doc cac tep cau hinh Lua (script\\cauhinh\\ch_*.lua + script\\header\\cauhinh_hoatdong.lua) bang
latin-1 (giu nguyen byte TCVN3), lay: ten khoa, gia tri, chu thich (cung dong hoac khoi chu thich
ngay tren), suy ra NHOM / KIEU / KHOANG / AP_DUNG, quet ca cay script xem khoa duoc doc o dau
(trong than ham = an ngay; chi o cap tep = can nap lai script / restart), ghep TU DIEN TIENG VIET
(cfgw_vietngu: ten, giai thich, canh bao, nguy co - UTF-8 -> TCVN3) roi ghi:

    <script>\\cauhinh_web\\cfgw_meta.lua
        tbCFGW_META     = { KHOA = {"NHOM","chu thich goc",kieu,min,max,ap,"nguon","don_vi",
                                    "ten","giai_thich","canh_bao",nguy_co}, ... }
        tbCFGW_META_CPP = { ["ServerConfig.ExpRate"] = {"ten","giai_thich","canh_bao",nguy_co}, ... }
    <ReverseTools>\\cauhinh_web\\cfgw_meta_baocao.txt   thong ke + danh sach khoa can restart
    <ReverseTools>\\cauhinh_web\\cfgw_tudien.md         TU DIEN doc duoc (UTF-8) cho chu game / web

BAO LOI (exit 1, KHONG ghi cfgw_meta.lua) khi: khoa script thieu tieng Viet, chu khong doi duoc
sang TCVN3 (chu hoa co dau, ky tu la), qua dai (ten 96 / giai thich 1500 / canh bao 800 byte).

Chay:  python sinh_cfgw_meta.py <thu muc script may chu>
Vi du: python sinh_cfgw_meta.py "E:\\SourceTuanLe\\SourceVs22\\TESTLOFFF_ONLINE\\bin\\server\\script"
"""
import io, os, re, sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")   # console Windows cp1252 khong in duoc chu Viet
except Exception:
    pass

if len(sys.argv) < 2:
    sys.exit(__doc__)
S = sys.argv[1]
HERE = os.path.dirname(os.path.abspath(__file__))
OUT_LUA = sys.argv[2] if len(sys.argv) > 2 else os.path.join(S, "cauhinh_web", "cfgw_meta.lua")   # doi so 2: ghi ra cho khac (kiem lua4 truoc khi chep vao cay that)
OUT_BC = os.path.join(HERE, "cfgw_meta_baocao.txt")
OUT_TD = os.path.join(HERE, "cfgw_tudien.md")

sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(os.path.expanduser("~"), ".claude", "skills", "swordonline-dev", "scripts"))
from cfgw_vietngu import VN, NHOM_VN, NGUY_CO_VN          # noqa: E402
try:
    from vn_to_octal import unicode_to_tcvn3_bytes         # noqa: E402
except ImportError:
    sys.exit("KHONG import duoc vn_to_octal (skill swordonline-dev/scripts) - can bo chuyen UTF-8 -> TCVN3")

MAX_TEN, MAX_GT, MAX_CB = 96, 1500, 800

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

# ky tu UTF-8 hay gap trong van ban -> ASCII (TCVN3 khong co)
THAY_KYTU = {"\u2014": "-", "\u2013": "-", "\u2026": "...", "\u201c": '"', "\u201d": '"', "\u2018": "'", "\u2019": "'",
             "\u2192": "->", "\u2265": ">=", "\u2264": "<=", "\u00d7": "x", "\u00a0": " ", "\u2022": "-"}


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
    """Chuoi latin-1 (byte TCVN3 tho) -> literal Lua 4 (giu nguyen byte cao)."""
    s = s.replace("\\", "\\\\").replace('"', '\\"').replace("\r", "").replace("\n", "\\n")
    return '"' + s + '"'


def so(v):
    if isinstance(v, float) and v != int(v):
        return ("%.10g" % v)
    return "%d" % int(v)


LOI = []       # (khoa, thong diep)


def tcvn3(k, truong, s, max_byte):
    """UTF-8 -> chuoi latin-1 mang byte TCVN3. Loi -> ghi vao LOI, tra ''."""
    if s is None:
        s = ""
    for a, b in THAY_KYTU.items():
        s = s.replace(a, b)
    try:
        b = unicode_to_tcvn3_bytes(s)
    except Exception as e:
        LOI.append((k, "%s: %s" % (truong, str(e).split(". ")[0][:120])))
        return ""
    if len(b) > max_byte:
        LOI.append((k, "%s dai %d byte > %d" % (truong, len(b), max_byte)))
        return ""
    return b.decode("latin-1")


def muc_vn(k):
    """VN[k] -> (ten, gt, cb, nguy_co, ghi_de dict) hoac None."""
    m = VN.get(k)
    if m is None:
        return None
    ten, gt, cb, nc = m[0], m[1], m[2], int(m[3])
    gd = m[4] if len(m) > 4 and isinstance(m[4], dict) else {}
    if nc not in (0, 1, 2):
        LOI.append((k, "nguy_co %r khong phai 0/1/2" % (nc,)))
        nc = 1
    return ten, gt, cb, nc, gd


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
                kk = m.group(1)
                trong_ham = line[:1] in (" ", "\t")   # co thut dau dong = trong than ham/khoi
                a, b = CACHE_SD.get(kk, (0, 0))
                CACHE_SD[kk] = (a + (1 if trong_ham else 0), b + (0 if trong_ham else 1))


def quet_su_dung(khoa):
    return CACHE_SD.get(khoa, (0, 0))


meta = []      # dict: k nhom mota kieu mn mx ap nguon donvi ten gt cb nc val
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
                float(val)
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
        mn = mx = 0
        if kieu == 0:
            if "_GIO_" in k or k.endswith("_GIO"):
                kieu = 4
                mn, mx = 0, 2359
            elif k.startswith("BAT_") or k.endswith("_BAT") or k.endswith("_BAT_TAT") or (val in ("0", "1") and re.search(r"\b(BAT|TAT|1 =|0 =)\b", tag)):
                kieu = 3
                mn, mx = 0, 1
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
        donvi = don_vi_cua(k, kieu)
        # --- tu dien tieng Viet + ghi de ---------------------------------------------------
        vn = muc_vn(k)
        ten = gt = cb = ""
        nc = 1
        if vn is None:
            LOI.append((k, "THIEU tieng Viet trong cfgw_vietngu"))
        else:
            ten0, gt0, cb0, nc, gd = vn
            if "k" in gd:
                kieu = int(gd["k"])
                if kieu == 3:
                    mn, mx = 0, 1
                elif kieu == 4:
                    mn, mx = 0, 2359
                else:
                    mn = mx = 0
            if "dv" in gd:
                donvi = gd["dv"]
            if "mn" in gd or "mx" in gd:
                mn = gd.get("mn", 0)
                mx = gd.get("mx", 0)
            ten = tcvn3(k, "ten", ten0, MAX_TEN)
            gt = tcvn3(k, "giai_thich", gt0, MAX_GT)
            cb = tcvn3(k, "canh_bao", cb0, MAX_CB)
        meta.append(dict(k=k, nhom=nhom, mota=mota, kieu=kieu, mn=mn, mx=mx, ap=ap, nguon=os.path.basename(rel),
                         donvi=donvi, ten=ten, gt=gt, cb=cb, nc=nc, val=val, ten0=(vn[0] if vn else ""),
                         gt0=(vn[1] if vn else ""), cb0=(vn[2] if vn else "")))

# --- khoa C++ (ServerConfig.* / Exp.*) - chi tieng Viet, metadata do C++ khai ----------------------
meta_cpp = []
da_co = set(m["k"] for m in meta)
for k in sorted(VN):
    if k in da_co:
        continue
    if "." not in k:
        LOI.append((k, "co trong cfgw_vietngu nhung khong con trong script (THUA)"))
        continue
    ten0, gt0, cb0, nc, gd = muc_vn(k)
    meta_cpp.append(dict(k=k, ten=tcvn3(k, "ten", ten0, MAX_TEN), gt=tcvn3(k, "giai_thich", gt0, MAX_GT),
                         cb=tcvn3(k, "canh_bao", cb0, MAX_CB), nc=nc, ten0=ten0, gt0=gt0, cb0=cb0))

# --- bao cao --------------------------------------------------------------------------------------
bc = []
bc.append("cfgw_meta: %d khoa script, %d khoa C++, %d bo qua, %d LOI" % (len(meta), len(meta_cpp), len(bo_qua), len(LOI)))
tk = {}
for m in meta:
    tk.setdefault(m["nhom"], [0, 0, 0])
    tk[m["nhom"]][0] += 1
    if m["ap"] in (1, 2):
        tk[m["nhom"]][m["ap"]] += 1
bc.append("%-14s %5s %8s %8s" % ("NHOM", "khoa", "ap<=1ph", "restart"))
for nhom in sorted(tk):
    bc.append("%-14s %5d %8d %8d" % (nhom, tk[nhom][0], tk[nhom][1], tk[nhom][2]))
bc.append("")
if LOI:
    bc.append("LOI (%d) - KHONG ghi cfgw_meta.lua:" % len(LOI))
    for k, ly in LOI:
        bc.append("  %-30s %s" % (k, ly))
    bc.append("")
bc.append("NGUY CO: %d an toan, %d can than, %d nguy hiem" % tuple(sum(1 for m in meta + meta_cpp if m["nc"] == i) for i in (0, 1, 2)))
bc.append("")
bc.append("KHOA CAN NAP LAI SCRIPT / RESTART (ap_dung = 2):")
for m in meta:
    if m["ap"] == 2:
        bc.append("  %-30s %-12s %s" % (m["k"], m["nhom"], m["nguon"]))
bc.append("")
bc.append("BO QUA (khong dua len web):")
for k, ly in bo_qua:
    bc.append("  %-30s %s" % (k, ly))
io.open(OUT_BC, "w", encoding="utf-8").write("\n".join(bc) + "\n")
print("\n".join(bc[:len(tk) + 2]))
if LOI:
    print("\n".join(bc[len(tk) + 3:len(tk) + 5 + len(LOI)]))

# --- tu dien UTF-8 (cho chu game / phien web doc) ---------------------------------------------------
KIEU_TEN = {0: "số nguyên", 1: "số thực", 2: "chuỗi", 3: "bật/tắt", 4: "giờ HH:MM"}
AP_TEN = {0: "trong 30 giây", 1: "trong 1,5 phút", 2: "cần khởi động lại máy chủ"}
td = []
td.append("# TỪ ĐIỂN CẤU HÌNH GAME (sinh tự động từ cfgw_vietngu - đừng sửa tay)")
td.append("")
td.append("Mỗi khoá: tên - `KHOÁ` - giá trị hiện tại trong tệp - kiểu - hiệu lực - mức nguy cơ. Web admin hiện đúng nội dung này (bảng `gcfg` cột `ten`, `giai_thich`, `canh_bao`, `nguy_co`).")
td.append("")
thu_tu_nhom = list(NHOM_VN.keys())
for nhom in thu_tu_nhom:
    ds = [m for m in meta if m["nhom"] == nhom]
    if nhom == "EXP":
        ds = [m for m in meta_cpp if m["k"].startswith("Exp.") or m["k"] == "ServerConfig.ExpRate"] + ds
    if nhom == "EXP_KYNANG":
        ds = [m for m in meta_cpp if m["k"].startswith("ServerConfig.Skill")] + ds
    if nhom == "TIEN":
        ds = [m for m in meta_cpp if m["k"] == "ServerConfig.MoneyRate"] + ds
    if not ds:
        continue
    ten_nhom, mota_nhom = NHOM_VN[nhom]
    td.append("## %s (`%s`) - %d khoá" % (ten_nhom, nhom, len(ds)))
    td.append("")
    td.append(mota_nhom)
    td.append("")
    for m in ds:
        dong = "### %s  `%s`" % (m["ten0"], m["k"])
        td.append(dong)
        if "val" in m:
            td.append("- Giá trị trong tệp: `%s` %s · kiểu %s%s · hiệu lực: %s · nguy cơ: **%s**" % (
                m["val"], m["donvi"], KIEU_TEN.get(m["kieu"], m["kieu"]),
                (" · khoảng %s..%s" % (so(m["mn"]), so(m["mx"]))) if (m["mn"] or m["mx"]) else "",
                AP_TEN[m["ap"]], NGUY_CO_VN[m["nc"]]))
        else:
            td.append("- Khoá C++ (gamesetting.ini) · hiệu lực: trong 30 giây · nguy cơ: **%s**" % NGUY_CO_VN[m["nc"]])
        td.append("- " + m["gt0"].replace("\n", "\n  "))
        td.append("- **Cảnh báo:** " + m["cb0"].replace("\n", "\n  "))
        td.append("")
io.open(OUT_TD, "w", encoding="utf-8").write("\n".join(td) + "\n")

if LOI:
    print("LOI: %d muc - xem %s. KHONG ghi %s" % (len(LOI), OUT_BC, OUT_LUA))
    sys.exit(1)

# --- ghi cfgw_meta.lua (latin-1: giu nguyen byte TCVN3) --------------------------------------------
os.makedirs(os.path.dirname(OUT_LUA), exist_ok=True)
out = []
out.append("-- ============================================================================")
out.append("-- CFGW_META.LUA - SINH TU DONG boi ReverseTools\\cauhinh_web\\sinh_cfgw_meta.py - DUNG SUA TAY")
out.append("-- [CFGW 04/09, 05/09] mo ta / nhom / kieu / khoang / ap_dung + TIENG VIET (ten, giai thich,")
out.append("-- canh bao, nguy co) cua tung khoa cau hinh, de cfgw_driver.lua khai len bang MySQL gcfg.")
out.append("-- Khoa = { nhom, chu thich goc, kieu, min, max, ap_dung, nguon, don_vi, ten, giai_thich, canh_bao, nguy_co }")
out.append("--   kieu: 0 so nguyen | 1 so thuc | 2 chuoi | 3 bat/tat | 4 gio HHMM")
out.append("--   ap_dung: 1 = an trong <= 1,5 phut | 2 = can nap lai script hoac restart")
out.append("--   nguy_co: 0 an toan | 1 can than | 2 nguy hiem")
out.append("-- Chu tieng Viet la byte TCVN3 tho (web doc bang ChuViet::sangUtf8). Sua chu: sua cfgw_vietngu\\vn_*.py roi sinh lai.")
out.append("-- TEP NAY LA 'LA': CAM Include bat cu gi vao day.")
out.append("-- ============================================================================")
out.append("tbCFGW_META = {")
for m in meta:
    out.append("%s = {%s, %s, %d, %s, %s, %d, %s, %s, %s, %s, %s, %d}," % (
        m["k"], lua_str(m["nhom"]), lua_str(m["mota"]), m["kieu"], so(m["mn"]), so(m["mx"]), m["ap"],
        lua_str(m["nguon"]), lua_str(m["donvi"]), lua_str(m["ten"]), lua_str(m["gt"]), lua_str(m["cb"]), m["nc"]))
out.append("}")
out.append("")
out.append("-- Khoa C++ (ServerConfig.* / Exp.*): metadata do C++ khai, day chi la tieng Viet -> CFGW_MoTa()")
out.append("tbCFGW_META_CPP = {")
for m in meta_cpp:
    out.append("[%s] = {%s, %s, %s, %d}," % (lua_str(m["k"]), lua_str(m["ten"]), lua_str(m["gt"]), lua_str(m["cb"]), m["nc"]))
out.append("}")
io.open(OUT_LUA, "w", encoding="latin-1", newline="\r\n").write("\n".join(out) + "\n")
print("da ghi", OUT_LUA, "|", OUT_BC, "|", OUT_TD)
