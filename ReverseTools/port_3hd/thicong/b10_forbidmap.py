# -*- coding: ascii -*-
"""B10 - VA N3: chan vat pham dich chuyen tren ban do 3 hoat dong.

VAN DE: ban va DisabledUseTownP thanh "ban that" KHONG CO TAC DUNG vi
GetDisabledUseTownP() co 0 loi goi trong toan cay; duong chan THAT SU cua JX1 la
CheckAllMaps() o script\\header\\forbidmap.lua (moi item dich chuyen deu goi:
townportal_l.lua:48, ib\\thodiafu.lua, ib\\shenxingfu.lua, bosscharm.lua:76...).
Dot TONGCASTLE 23/08 da lam dung kieu nay cho map 984.

Them: 337/338/339 (3 map thuyen Phong Lang Do) + 464-495 (32 map Vuot Ai)
      + 957 (map che do chuangguan30).
KHONG them 336 (bo Bac la ban do mo - ban goc chi goi DisabledUseTownP khi LEN THUYEN).
"""
import io, os, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"
rel = os.path.join("script", "header", "forbidmap.lua")
p = os.path.join(JX1, rel)
d = io.open(p, encoding="latin-1", newline="").read()
NL = "\r\n" if "\r\n" in d else "\n"

if "check3HDMaps" in d:
    print("da va roi")
    raise SystemExit

# 1) them ham kiem
fn = NL.join([
    "",
    "-- [3HD 25/08] 3 hoat dong ban Linux: cam moi item dich chuyen.",
    "--   337/338/339 = 3 map thuyen Phong Lang Do (ban goc goi DisabledUseTownP(1)",
    "--                 khi len thuyen - fld_head.lua:142)",
    "--   464..495     = 32 map Vuot Ai (2 cap do, challengeoftime include.lua tbLevelMaps)",
    "--   957          = map che do chuangguan30",
    "-- KHONG cam 336 (bo Bac la ban do mo, ban goc khong cam).",
    "-- LY DO dung duong nay: DisabledUseTownP cua JX1 khong co duong doc nao",
    "-- (GetDisabledUseTownP 0 loi goi); CheckAllMaps moi la cho MOI item dich chuyen",
    "-- that su hoi - y het cach dot TONGCASTLE 23/08 lam cho map 984.",
    "function check3HDMaps(mapid)",
    "\tif (mapid == 337 or mapid == 338 or mapid == 339) then",
    "\t\treturn 1",
    "\tend",
    "\tif (mapid >= 464 and mapid <= 495) then",
    "\t\treturn 1",
    "\tend",
    "\tif (mapid == 957) then",
    "\t\treturn 1",
    "\tend",
    "\treturn 0",
    "end",
    "",
])
anchor_fn = "function CheckAllMaps(mapid)"
assert d.count(anchor_fn) == 1, "anchor ham=%d" % d.count(anchor_fn)
d = d.replace(anchor_fn, fn.lstrip(NL) + NL + anchor_fn)

# 2) goi trong CheckAllMaps (ngay sau nhanh 984)
anchor_call = NL.join([
    "\tif mapid == 984 then",
    "\t\treturn 1",
    "\tend",
])
assert d.count(anchor_call) == 1, "anchor goi=%d" % d.count(anchor_call)
d = d.replace(anchor_call, anchor_call + NL + NL.join([
    "\t-- [3HD 25/08] 3 hoat dong ban Linux (xem check3HDMaps phia tren)",
    "\tif check3HDMaps(mapid) == 1 then",
    "\t\treturn 1",
    "\tend",
]))

io.open(p, "w", encoding="latin-1", newline="").write(d)
m = os.path.join(MIRROR, rel)
os.makedirs(os.path.dirname(m), exist_ok=True)
io.open(m, "w", encoding="latin-1", newline="").write(d)
print("da them check3HDMaps + loi goi trong CheckAllMaps")
