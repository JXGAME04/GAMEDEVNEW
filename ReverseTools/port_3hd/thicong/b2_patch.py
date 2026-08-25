# -*- coding: utf-8 -*-
"""B2 — VÁ các tệp vừa chép sang JX1 cho đúng engine JX1, GIỮ hành vi 100% bản Linux.

Ba nhóm vá, đều byte-an toàn (đọc/ghi latin-1, không đụng cao-byte TCVN3/GBK):

 (1) AddNpc -> AddNpcEx  (chèn ngũ hành random(0,4) vào tham số 3)
     Ban Linux `AddNpc` tự sinh nseries = rand()%5 trong engine; JX1 `AddNpc` lấy
     tham số 6 = nseries -> chép nguyên se cho MOI NPC he Kim. AddNpcEx cua JX1
     nhan (id, lv, series, map, x, y, noRevive, name, isboss) - dung khuon Linux.

 (2) Ánh xạ 42 bộ ID vật phẩm TRÙNG NẶNG -> bộ số JX1 CÙNG TÊN vật phẩm
     (id_dungdo.csv). Giữ đúng vật phẩm bản Linux, chỉ đổi con số để không phát
     nhầm đồ của JX1. Chỉ vá trong 20 tệp vừa chép (không đụng kinh tế JX1 cũ).

 (3) Bỏ Include tệp KHÔNG TỒN TẠI  boss.lua:11  \script\global\路人_礼官.lua
     (không có ở bất kỳ gốc nào - BS-1 của báo cáo 15).

Chạy: python b2_patch.py [--apply]
"""
import io, os, re, csv, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
APPLY = "--apply" in sys.argv

HERE = os.path.dirname(os.path.abspath(__file__))
P3 = os.path.dirname(HERE)
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"

log = []


def load(rel):
    p = os.path.join(JX1, rel.replace("/", os.sep))
    return p, open(p, "rb").read().decode("latin-1")


def save(rel, p, d):
    if APPLY:
        open(p, "wb").write(d.encode("latin-1"))
        m = os.path.join(MIRROR, rel.replace("/", os.sep))
        os.makedirs(os.path.dirname(m), exist_ok=True)
        open(m, "wb").write(d.encode("latin-1"))


# ---------- (1) AddNpc -> AddNpcEx ----------
# Chỉ khớp lời gọi có >= 6 tham số (id, lv, map, x, y, ...) — chèn random(0,4)
# sau tham số 2. Không khớp AddNpcEx/AddNpcSet... (bọc \bAddNpc\( + không phải Ex).
RE_ADDNPC = re.compile(r"\bAddNpc\(([^,]+),([^,]+),")
NPC_FILES = [
    "script/task/tollgate/killbosshead.lua",
    "script/missions/fengling_ferry/boss.lua",
    "script/missions/fengling_ferry/fld_smalltimer.lua",
    "script/missions/fengling_ferry/mission.lua",
]
for rel in NPC_FILES:
    p, d = load(rel)
    n = 0

    def sub(m):
        global n
        n += 1
        return "AddNpcEx(%s,%s,random(0,4)," % (m.group(1), m.group(2))
    d2 = RE_ADDNPC.sub(sub, d)
    save(rel, p, d2)
    log.append("AddNpc->AddNpcEx  %-52s %d cho" % (rel.rsplit("/", 1)[-1], n))

# ---------- (2) item remap ----------
# Bang anh xa DA GIAI NHAP NHANG bang TEN vat pham (resolve_remap.py) - KHONG
# lay dich dau tien (399->2,1,399 sai genre); tat ca deu "ten trung".
import json as _json
remap = _json.load(io.open(os.path.join(HERE, "remap_resolved.json"), encoding="utf-8"))

manifest = io.open(os.path.join(HERE, "b1_manifest.txt"), encoding="utf-8").read().split("\n")
total_remap = 0
for rel in manifest:
    if not rel.lower().endswith(".lua"):
        continue
    p = os.path.join(JX1, rel.replace("/", os.sep))
    if not os.path.isfile(p):
        continue
    d = open(p, "rb").read().decode("latin-1")
    changed = 0
    for src_id, dst_id in remap.items():
        g, dd, pt = src_id.split(",")
        # bat "6,1,399" hoac "6, 1, 399" nhung KHONG bat khi la mot phan cua so lon hon
        pat = re.compile(r"(?<![0-9])%s(\s*,\s*)%s(\s*,\s*)%s(?![0-9])" % (g, dd, pt))
        newval = dst_id.split(",")

        def rep(m, nv=newval):
            return "%s%s%s%s%s" % (nv[0], m.group(1), nv[1], m.group(2), nv[2])
        d, c = pat.subn(rep, d)
        changed += c
    if changed:
        save(rel, p, d)
        total_remap += changed
        log.append("item remap        %-52s %d cho" % (rel.rsplit("/", 1)[-1], changed))

# ---------- (3) bỏ Include chết trong boss.lua ----------
rel = "script/missions/fengling_ferry/boss.lua"
p, d = load(rel)
# dòng Include("\script\global\<GBK>.lua") — nhận diện bằng byte GBK c2 b7 c8 cb
lines = d.split("\n")
out = []
removed = 0
for ln in lines:
    raw = ln.encode("latin-1")
    if b"Include(" in raw and b"\xc2\xb7\xc8\xcb" in raw:
        out.append("-- [3HD 25/08] bo Include tep khong ton tai o moi goc (BS-1): " + ln.strip())
        removed += 1
    else:
        out.append(ln)
if removed:
    save(rel, p, "\n".join(out))
    log.append("bo Include chet    %-52s %d dong" % ("boss.lua", removed))

# ---------- (4) dạng SO SÁNH TRẦN (parttype == N) mà regex 6,1,N bỏ sót ----------
# Chỉ 2 chỗ (scan_bareforms.py): nieshichen 399->398 (cổng gộp Sát Thủ lệnh),
# shuizei np==2015 -> 2024 (Truy công lệnh). Phải khớp producer đã remap ở trên.
# dich lay tu remap_resolved: 6,1,399->6,1,398 ; 6,1,2015->6,1,2024
bare = [
    ("script/task/tollgate/killer/nieshichen.lua", "parttype == 399  )",
     "parttype == %s  )" % remap["6,1,399"].split(",")[2]),
    ("script/event/jiefang_jieri/200904/shuizei/shuizei.lua", "np == 2015)",
     "np == %s)" % remap["6,1,2015"].split(",")[2]),
]
for rel, old, new in bare:
    p, d = load(rel)
    c = d.count(old)
    if c == 1:
        save(rel, p, d.replace(old, new))
        log.append("bare-form remap   %-52s 1 cho (%s)" % (rel.rsplit("/", 1)[-1], old.strip()))
    else:
        log.append("!! bare-form KHONG KHOP (%d): %s [%s]" % (c, rel.rsplit("/", 1)[-1], old))

print("== B2 %s ==" % ("APPLY" if APPLY else "(xem truoc)"))
for l in log:
    print("  " + l)
print("  Tong cho item remap:", total_remap)
