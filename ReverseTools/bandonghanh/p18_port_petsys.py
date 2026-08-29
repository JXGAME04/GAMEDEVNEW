# -*- coding: utf-8 -*-
r"""[PETSYS 28/08] PORT HE "BAN DONG HANH" BAN PC (petsys) TU LINUX -> JX1.

Nguon 100%: D:\ServerLinux\server1\script\petsys (13 lua) + settings\petsys
(3 bang) + client VLTK pet.ini (ra_pet\a069d4c0.ini). He nay KHAC he partner
mobile (song song tren Linux); backend C "PET" lib phai tu viet (p19).

Buoc nay (thuan du lieu, khong build):
 1. Chep 13 lua -> server script\petsys, nan:
    - protocol_process_gs.lua: + wrapper global PetSys_Protocol(nOp)
    - item id: 3453..3458 cua ban private DUNG bang khac -> them 6 ITEM MOI
      cuoi magicscript JX1 (server+client), id ghi vao pet_items.lua sinh ra,
      cac lua port duoc nan tbProp theo id moi.
 2. Chep 3 bang settings\petsys (server + client).
 3. npcs.txt: noi 2 template 2184/2185 tu Linux (server + client) - lot cac
    dong giua bang dong RONG dung khuon (nhu cac hang trong san co).
 4. Sinh 4 skill AURA vao skills.txt (server+client) tu pet_skill_def.txt
    (20 cap, attrib enhancehit_rate/skill_enhance/allres_yan_p/block_rate)
    -> ap len CHU qua AddNpcSkillState (p19 goi).
Sao luu .truoc_petsys. Idempotent.
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

SRC = r"D:\ServerLinux\server1"
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
BS = chr(92)
CR = chr(13)
LF = chr(10)
T = chr(9)

# ---------------------------------------------------------------- 1. lua
d_src = os.path.join(SRC, "script", "petsys")
d_dst = os.path.join(SV, "script", "petsys")
os.makedirs(d_dst, exist_ok=True)
n = 0
for f in os.listdir(d_src):
    if not f.endswith(".lua"):
        continue
    data = open(os.path.join(d_src, f), "rb").read()
    dst = os.path.join(d_dst, f)
    if not os.path.isfile(dst) or open(dst, "rb").read() != data:
        open(dst, "wb").write(data)
        n += 1
print("chep %d lua petsys" % n)

# wrapper global cho protocol
p = os.path.join(d_dst, "protocol_process_gs.lua")
s = io.open(p, "r", encoding="latin-1", newline="").read()
if "PetSys_Protocol" not in s:
    s = s.rstrip() + CR + LF + CR + LF + \
        "-- [JX1 28/08] wrapper global de C goi (CallFunction chi goi ham global)" + CR + LF + \
        "function PetSys_Protocol(nOp)" + CR + LF + \
        T + "PetSys:ProtocolProcess(nOp)" + CR + LF + \
        "end" + CR + LF
    io.open(p, "w", encoding="latin-1", newline="").write(s)
    print("da them wrapper PetSys_Protocol")

# ---------------------------------------------------------------- 2. bang
for base in (SV, CL):
    dd = os.path.join(base, "settings", "petsys")
    os.makedirs(dd, exist_ok=True)
    for f in ["feature.txt", "levelup.txt", "pet_skill_def.txt"]:
        src = os.path.join(SRC, "settings", "petsys", f)
        dst = os.path.join(dd, f)
        data = open(src, "rb").read()
        if not os.path.isfile(dst) or open(dst, "rb").read() != data:
            open(dst, "wb").write(data)
            print("bang:", os.path.basename(base), f)

# ---------------------------------------------------------------- 3. npcs.txt + 2 template
def noi_npc(p):
    raw = open(p, "rb").read()
    if b"linhaotian" in raw:
        print("  npcs da co linhaotian:", p)
        return
    rows = raw.split(b"\n")
    # bo phan tu rong cuoi neu co
    du = b""
    if rows and rows[-1] == b"":
        rows = rows[:-1]
        du = b"\n"
    n_cot = len(rows[0].split(b"\t"))
    # dong rong chuan: ten rong + cac cot 0
    trong = ("\t".join(["0"] * n_cot)).encode("latin-1")
    lr = open(os.path.join(SRC, "settings", "npcs.txt"), "rb").read().split(b"\n")
    can = {2184: lr[2185].rstrip(b"\r"), 2185: lr[2186].rstrip(b"\r")}
    while len(rows) < 2186:                     # can toi dong index 2185+1
        idx = len(rows) - 1                     # id cua dong sap them
        if idx in can:
            rows.append(can[idx] + b"\r")
        else:
            rows.append(trong + b"\r")
    if not os.path.exists(p + ".truoc_petsys"):
        shutil.copyfile(p, p + ".truoc_petsys")
    open(p, "wb").write(b"\n".join(rows) + du)
    print("  npcs.txt noi toi 2185:", p, "->", len(rows), "dong")


noi_npc(os.path.join(SV, "settings", "npcs.txt"))
noi_npc(os.path.join(CL, "settings", "npcs.txt"))

# ---------------------------------------------------------------- 4. 6 item moi
# khuon: chep dong item 6,1 co san (3455 Hoan Hon Don) roi doi ten/script.
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes as MA  # noqa: E402

ITEMS = [  # (ten, script)
    ("Thiệp Đồng Hành", BS + "script" + BS + "petsys" + BS + "petcard.lua"),
    ("Thuốc Tăng Trưởng", ""),          # tieu hao qua levelup, khong script
    ("Trái táo", BS + "script" + BS + "petsys" + BS + "feed.lua"),
    ("Mía", BS + "script" + BS + "petsys" + BS + "feed.lua"),
    ("Bắp", BS + "script" + BS + "petsys" + BS + "feed.lua"),
    ("Khoai lang", BS + "script" + BS + "petsys" + BS + "feed.lua"),
]


def them_item(p):
    raw = open(p, "rb").read()
    rows = raw.split(b"\n")
    du = b""
    if rows and rows[-1] == b"":
        rows = rows[:-1]
        du = b"\n"
    ten0 = MA("Thiệp Đồng Hành")
    for r in rows:
        if r.startswith(ten0 + b"\t"):
            print("  item da co:", p)
            # van tra id: tim vi tri
            for i, rr in enumerate(rows):
                if rr.startswith(ten0 + b"\t"):
                    return i - 1
    khuon = rows[3456].rstrip(b"\r").split(b"\t")   # id 3455 Hoan Hon Don (6,1)
    id_dau = len(rows) - 1                          # id cua dong sap them dau tien
    for ten, scr in ITEMS:
        c = list(khuon)
        c[0] = MA(ten)
        if len(c) > 9:
            c[9] = scr.encode("latin-1") if scr else b"0"
        rows.append(b"\t".join(c) + b"\r")
    if not os.path.exists(p + ".truoc_petsys"):
        shutil.copyfile(p, p + ".truoc_petsys")
    open(p, "wb").write(b"\n".join(rows) + du)
    print("  them 6 item vao:", p, "id dau =", id_dau)
    return id_dau


id_sv = them_item(os.path.join(SV, "settings", "item", "magicscript.txt"))
id_cl = them_item(os.path.join(CL, "settings", "item", "magicscript.txt"))
assert id_sv == id_cl, (id_sv, id_cl)
ID = {"CARD": id_sv, "MED": id_sv + 1, "APPLE": id_sv + 2, "SUGAR": id_sv + 3,
      "MAIZE": id_sv + 4, "POTATO": id_sv + 5}
print("ID item petsys:", ID)

# ---------------------------------------------------------------- 5. nan id item trong lua port
# levelup.lua: 3454 -> MED ; feed.lua: 3455 tao,3456 mia,3457 bap,3458 khoai
for f, doi in [("levelup.lua", {b"3454": str(ID["MED"]).encode()}),
               ("feed.lua", {b"3455": str(ID["APPLE"]).encode(),
                             b"3456": str(ID["SUGAR"]).encode(),
                             b"3457": str(ID["MAIZE"]).encode(),
                             b"3458": str(ID["POTATO"]).encode()})]:
    p = os.path.join(d_dst, f)
    raw = open(p, "rb").read()
    if b"[JX1 nan id]" not in raw:
        for cu, moi in doi.items():
            raw = raw.replace(b"{6,1," + cu + b",", b"{6,1," + moi + b",")
        raw += ("\r\n-- [JX1 nan id] item id da nan theo bang JX1: %s\r\n" %
                {k: v for k, v in ID.items()}).encode("latin-1")
        open(p, "wb").write(raw)
        print("nan id:", f)

# ---------------------------------------------------------------- 6. 4 skill AURA
# doc pet_skill_def -> sinh 4 skill moi cuoi skills.txt (server+client).
# khuon: dong skill 549 (khang partner - state skill) lam nen; sua cot Name +
# 3 cot attrib dau (AttribName/Param1/Param2) theo tung cap... Bang skills.txt
# JX co dinh dang rong; de AN TOAN + du dung cho AddNpcSkillState, ta sinh
# skill voi cot attrib "SrcAttrib1..": xem hang mau roi dien.
sk_src = open(os.path.join(SV, "settings", "petsys", "pet_skill_def.txt"), "rb").read()
sk_rows = [r.split(b"\t") for r in sk_src.split(b"\n") if r.strip()]
AURA = [[], [], [], []]                # [loai][cap] = (attrib, p1)
for r in sk_rows[1:]:
    lv = int(r[0])
    for k in range(4):
        a = r[1 + k * 5].decode("latin-1")
        p1 = r[2 + k * 5].decode("latin-1")
        AURA[k].append((lv, a, p1))
print("pet_skill_def: 4 loai x", len(AURA[0]), "cap")

skills_p = os.path.join(SV, "settings", "skills.txt")
sk = open(skills_p, "rb").read().split(b"\n")
hdr = sk[0].split(b"\t")
print("skills.txt cot:", len(hdr), "| dong:", len(sk))
# ghi chu: viec sinh skill aura lam o p19 (can hieu cot attrib cua skills.txt
# truoc khi dien - in thu cot mau de doi chieu tay):
i549 = 550
c549 = sk[i549].split(b"\t")
attrib_cols = [j for j, h in enumerate(hdr) if b"Attrib" in h or b"attrib" in h]
print("cot chua 'Attrib':", [(j, hdr[j].decode("latin-1")) for j in attrib_cols[:12]])
print("XONG p18 (phan aura de p19 sau khi doi chieu cot)")
