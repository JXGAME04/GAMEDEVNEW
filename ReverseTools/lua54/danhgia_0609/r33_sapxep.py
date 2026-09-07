"""r33_sapxep.py - sap xep lai cay script theo bo cuc tieng Viet khong dau + sinh script/_duongdan_cu.txt (bi danh).
   python r33_sapxep.py <scratch> [thuc_hien]
   Quy tac: chi DOI CHO, khong sua noi dung; moi tep doi cho co mot dong "--@ cu=moi" de engine (KSortScript ID cu,
   Lua54Dll Include/dofile) van tim duoc theo duong dan cu. Tep bat dau '_' bi engine bo qua (Core moi)."""
import os, sys, json, shutil, collections
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
SP = sys.argv[1]
DO = len(sys.argv) > 2 and sys.argv[2] == "thuc_hien"
ROOT = os.environ.get("SAPXEP_ROOT", r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server")
SCRIPT = os.path.join(ROOT, "script")
LUU = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\_luutru\0609"
BS = chr(92)

# ---- bo cuc: thu muc cap 1 cu -> thu muc cha moi (giu ten con). None = giu nguyen (he thong, goc).
NHOM = {
    # nhan vat
    "skill": "nhanvat/kynang", "player": "nhanvat/nguoichoi", "petsys": "nhanvat/thucung", "partner": "nhanvat/donghanh",
    # vat pham (item len thang vatpham/), giao dich
    "item": "vatpham", "mail": "giaodich/thu", "auction_house": "giaodich/daugia",
    # tinh nang (thu muc tinhnang da co san, giu nguyen noi dung cu)
    "missions": "tinhnang/phuban", "event": "tinhnang/sukien", "battles": "tinhnang/chientruong", "leaguematch": "tinhnang/liendau",
    "songbac": "tinhnang/songbac", "vng_event": "tinhnang/vng_event", "vng_feature": "tinhnang/vng_feature",
    "activitysys": "tinhnang/activitysys", "nationalwar": "tinhnang/nationalwar", "huoyuedu": "tinhnang/huoyuedu",
    "baucua": "tinhnang/baucua", "bonusvlmc": "tinhnang/bonusvlmc", "mission": "tinhnang/congthanh", "chienlenh": "tinhnang/chienlenh",
    # nhiem vu (task len thang nhiemvu/) + npc (npcthon len thang npc/)
    "task": "nhiemvu", "dailogsys": "nhiemvu/hoithoai", "tagnewplayer": "nhiemvu/tanthu",
    "npcthon": "npc", "npcthanhthi": "npc/thanhthi", "npckhac": "npc/khac",
    # kiem thu
    "test": "kiemthu/test",
}
# thu muc ten Han con lai (trap map con dung, tham chieu theo ID) -> ten pinyin Viet
HAN_GBK = {bytes([0xd6, 0xd0, 0xd4, 0xad, 0xb1, 0xb1, 0xc7, 0xf8]): "tinhnang/trapcu/trungnguyenbac",   # trung nguyen bac
           bytes([0xce, 0xf7, 0xc4, 0xcf, 0xc4, 0xcf, 0xc7, 0xf8]): "tinhnang/trapcu/taynamnam",        # tay nam nam
           bytes([0xc1, 0xbd, 0xba, 0xfe, 0xc7, 0xf8]): "tinhnang/trapcu/luongho",
           bytes([0xbd, 0xad, 0xc4, 0xcf, 0xc7, 0xf8]): "tinhnang/trapcu/giangnam"}                     # giang nam                    # luong ho
# tep le o goc -> du lieu
GOC = {"codenew.lua": "dulieu/codenew.lua", "giftcode_new.lua": "dulieu/giftcode_new.lua", "giftcode_fancung.lua": "dulieu/giftcode_fancung.lua"}
# tep kiem thu / admin nam lan trong thu muc chay (duong dan tuong doi script/, dung /)
KIEMTHU = [
    "item/bdh_admin.lua", "item/datau_admin.lua", "item/hd3_admin.lua", "item/hoatdong_admin.lua", "item/liendau_admin.lua",
    "item/simcity_admin.lua", "item/test_dungluyen_admin.lua", "item/test_hoatdong_admin.lua", "item/test_kinhmach_admin.lua",
    "item/test_loren_admin.lua", "item/test_phiphong_admin.lua", "partner/partner_test_bdh.lua", "task/system/task_debug.lua",
]
KIEMTHU_DIR = ["item/event/copy of 30thang4"]

# tep phai GIU NGUYEN CHO (C++ nap truc tiep theo duong dan / he thong) - duong dan tuong doi script/ dung /
GIU_TAI_CHO = set()
LOAI = json.load(open(os.path.join(SP, "r33_loai_tru.json")))
# chi giu tai cho nhung tep C++ nap TRUC TIEP (khong qua ID). Danh sach xac dinh tay tu KIEM TRA C++:
GIU_TAI_CHO.update([
    "npclevelscript/npclevelscript.lua", "global/script_protocol.lua", "script_protocol/protocol_def_gs.lua",
    "script_protocol/protocol_def_c.lua", "timerserver.lua", "protocol.lua", "startgame.lua", "gmscript.lua",
])

def to_disk_bytes(s):
    out = bytearray()
    for ch in s:
        o = ord(ch)
        if o < 128 or o in (0x81, 0x8D, 0x8F, 0x90, 0x9D): out.append(o)
        else:
            try: out += ch.encode("cp1252")
            except UnicodeEncodeError: out += b"?"
    return bytes(out)
def ansi_str(b):
    # byte ANSI -> unicode dung nhu Windows (cp1252; 5 byte khong dinh nghia -> U+0081...)
    return "".join(chr(x) if (x < 128 or x in (0x81, 0x8D, 0x8F, 0x90, 0x9D)) else bytes([x]).decode("cp1252") for x in b)
HAN = {ansi_str(k): v for k, v in HAN_GBK.items()}

def rel_of(path):
    return os.path.relpath(path, SCRIPT).replace(os.sep, "/")

plan = []   # (rel_cu, rel_moi)
TEN_UNICODE = []
def add(rel_cu, rel_moi):
    if rel_cu in GIU_TAI_CHO:
        return
    if rel_cu == rel_moi:
        return
    plan.append((rel_cu, rel_moi))

for dp, dn, fn in os.walk(SCRIPT):
    for f in fn:
        p = os.path.join(dp, f)
        rel = rel_of(p)
        if f.startswith("_") or f.lower() == "lua54_da_chuyen.txt" or f.lower() == "scripterror.log":
            continue
        if not (f.lower().endswith(".lua") or f.lower().endswith(".txt")):
            continue                                  # rac (.bak/.truoc_*/.dat/.zip) don rieng, khong bi danh
        if any(ord(ch) > 255 for ch in rel):
            TEN_UNICODE.append(rel); continue        # ten Unicode that: engine ANSI (cp1252) khong mo duoc -> giu nguyen, bao rieng
        parts = rel.split("/")
        top = parts[0]
        if len(parts) == 1:
            if f in GOC: add(rel, GOC[f])
            continue
        if rel in KIEMTHU:
            add(rel, "kiemthu/" + rel); continue
        if any(rel.startswith(d + "/") for d in KIEMTHU_DIR):
            add(rel, "kiemthu/" + rel); continue
        if top in HAN:
            add(rel, HAN[top] + "/" + "/".join(parts[1:])); continue
        if top in NHOM:
            add(rel, NHOM[top] + "/" + "/".join(parts[1:])); continue
        # thu muc khac: giu nguyen (lib, header, cauhinh, cauhinh_web, class, startgame, global, misc, maps, log_game,
        # vng_lib, npclevelscript, timertask, tinhnang, ...)

# kiem tra dich khong trung nhau va khong de len tep dang co
dich = collections.Counter(m for _, m in plan)
trung = [m for m, n in dich.items() if n > 1]
tontai = [m for _, m in plan if os.path.exists(os.path.join(SCRIPT, m.replace("/", os.sep)))]
print("Ke hoach: %d tep doi cho; dich trung: %d; dich da ton tai: %d" % (len(plan), len(trung), len(tontai)))
if TEN_UNICODE:
    print("CANH BAO: %d tep ten Unicode that (engine ANSI khong mo duoc, khong doi cho, can doi ten): %s" % (len(TEN_UNICODE), "; ".join(TEN_UNICODE[:6])))
for m in trung[:10]: print("  TRUNG", m)
for m in tontai[:10]: print("  TON TAI", m)
by = collections.Counter((c.split("/")[0], "/".join(m.split("/")[:2])) for c, m in plan)
for (c, m), n in sorted(by.items(), key=lambda kv: -kv[1]):
    print("  %4d  %-22s -> %s" % (n, c, m))
# bi danh + closure timerserver canh bao
closure = set(k.replace("script/", "", 1) for k in LOAI["timer_closure"] if k.startswith("script/"))
canh_bao = [c for c, _ in plan if c.lower() in closure]
print("Tep trong closure hot-reload timerserver se doi cho: %d (chi an toan khi server DA TAT)" % len(canh_bao))
open(os.path.join(SP, "r33_kehoach.txt"), "w", encoding="utf-8").write("\n".join("%s -> %s" % pm for pm in plan) + "\n")

def viet_bidanh(plan_list):
    lines = ["-- [SAPXEP 06/09] BI DANH DUONG DAN SCRIPT - tep nay la chu thich Lua hop le; engine doc cac dong \"--@ cu=moi\".",
             "-- Cu phap: --@ script\\duong\\dan\\cu.lua=script\\duong\\dan\\moi.lua  (tuong doi goc may chu, khong phan biet hoa thuong)",
             "-- Dung cho: KSortScript (ID bam cu -> script moi: trap Region_S.dat, cot script settings, g_GetScript ten cu),",
             "--           Lua54Dll lua4_dofile (Include C++) va shim dofile khi tep goc khong ton tai.",
             "-- Sinh boi ReverseTools\\lua54\\danhgia_0609\\r33_sapxep.py - CO THE them dong tay khi doi cho tep sau nay."]
    for c, m in plan_list:
        lines.append("--@ script" + BS + c.replace("/", BS) + "=script" + BS + m.replace("/", BS))
    return "\r\n".join(lines) + "\r\n"

if DO:
    n = 0
    for c, m in plan:
        src = os.path.join(SCRIPT, c.replace("/", os.sep)); dst = os.path.join(SCRIPT, m.replace("/", os.sep))
        if not os.path.exists(src): continue
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        os.rename(src, dst); n += 1
    # gop bi danh voi tep cu (neu co) - giu dong cu
    p_alias = os.path.join(SCRIPT, "_duongdan_cu.txt")
    cu_lines = []
    if os.path.exists(p_alias):
        for ln in ansi_str(open(p_alias, "rb").read()).split("\n"):
            if ln.startswith("--@"): cu_lines.append(ln.rstrip("\r"))
    txt = viet_bidanh(plan)
    if cu_lines:
        txt = txt + "\r\n".join(cu_lines) + "\r\n"
    open(p_alias, "wb").write(to_disk_bytes(txt))
    for rel in TEN_UNICODE:
        src = os.path.join(SCRIPT, rel.replace("/", os.sep)); dst = os.path.join(LUU, "ten_unicode", rel.replace("/", os.sep))
        if os.path.exists(src):
            os.makedirs(os.path.dirname(dst), exist_ok=True); shutil.move(src, dst)
    if TEN_UNICODE: print("tep ten Unicode da chuyen vao _luutru/0609/ten_unicode: %d" % len(TEN_UNICODE))
    # rac trong cay script (.bak, .dat, .log, back/...) -> _luutru/0609/rac_trong_script (giu .lua va .txt)
    nrac = 0
    for dp, dn, fn in os.walk(SCRIPT):
        for f in fn:
            fl = f.lower()
            if fl.endswith(".lua") or fl.endswith(".txt") or f.startswith("_"):
                continue
            src = os.path.join(dp, f); rel = os.path.relpath(src, ROOT)
            dst = os.path.join(LUU, "rac_trong_script", rel); os.makedirs(os.path.dirname(dst), exist_ok=True)
            shutil.move(src, dst); nrac += 1
    print("rac trong script/ da don: %d" % nrac)
    # ScriptError.log rai rac trong cay -> _luutru
    nlog = 0
    for top in ("script", "scriptjx2"):
        for dp, dn, fn in os.walk(os.path.join(ROOT, top)):
            for f in fn:
                if f.lower().endswith(".log"):
                    src = os.path.join(dp, f); rel = os.path.relpath(src, ROOT)
                    dst = os.path.join(LUU, "log_trong_script", rel); os.makedirs(os.path.dirname(dst), exist_ok=True)
                    shutil.move(src, dst); nlog += 1
    # xoa thu muc rong
    removed = 0
    for dp, dn, fn in os.walk(SCRIPT, topdown=False):
        if dp != SCRIPT and not os.listdir(dp):
            os.rmdir(dp); removed += 1
    print("DA DOI CHO %d tep; bi danh %d dong -> %s; don %d ScriptError.log; xoa %d thu muc rong" % (n, len(plan), p_alias, nlog, removed))
else:
    print("(kho: chua thuc hien; xem r33_kehoach.txt)")
