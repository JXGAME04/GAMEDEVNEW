# -*- coding: utf-8 -*-
r"""[PETSYS 28/08] Sinh 4 SKILL AURA (id 1600..1603) tu pet_skill_def.txt.

Khuon dung chuan engine (nhu skill khang partner 550):
  skills.txt: SkillName/Property(ho tro bi dong)/SkillId/Attrib=1008/MaxLevel=20/
              LvlSetScript=\script\skill\petsys\aura.lua/LvlSetting1=<attrib>/
              LvlData1=<ten mang>
  aura.lua:   SKILLS={ pet_aura_k={ attrib={diem noi suy theo cap} } }
Ap len CHU bang AddNpcSkillState(1599+loai, petLevel, thoi gian) - p19b.
Server + client dong bo. Sao luu .truoc_petsys. Idempotent.
"""
import io
import os
import shutil
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes as MA  # noqa: E402

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
CL = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
BS = chr(92)
CR = b"\r"

# ---- doc pet_skill_def: 4 day (cap -> gia tri param1) ----
rows = [r.split(b"\t") for r in
        open(os.path.join(SV, "settings", "petsys", "pet_skill_def.txt"), "rb").read().split(b"\n")
        if r.strip()]
DAY = [[], [], [], []]
ATTRIB = [None] * 4
for r in rows[1:]:
    lv = int(r[0])
    for k in range(4):
        a = r[1 + k * 5].decode("latin-1").strip()
        p1 = r[2 + k * 5].decode("latin-1").strip()
        if a not in ("-1", "") and ATTRIB[k] is None:
            ATTRIB[k] = a
        v = 0 if p1 in ("-1", "") else int(p1)
        DAY[k].append((lv, v))
print("attrib 4 loai:", ATTRIB)
for k in range(4):
    print("  loai", k + 1, DAY[k])

TEN = ["Pet: Tăng Trọng Kích", "Pet: Tăng Kỹ Năng Công Kích",
       "Pet: Tăng Kháng Tất Cả", "Pet: Hóa Giải Sát Thương"]
MANG = ["pet_aura_1", "pet_aura_2", "pet_aura_3", "pet_aura_4"]
ID0 = 1600

# ---- 1. aura.lua (ASCII thuan) ----
lua = ["-- [PETSYS 28/08] du lieu 4 skill AURA cua Ban Dong Hanh (pet_skill_def.txt)",
       "-- diem noi suy (cap, gia tri); engine noi suy Line giua cac diem.",
       "SKILLS={"]
for k in range(4):
    diem = ",".join("{%d,%d}" % (lv, v) for lv, v in DAY[k])
    lua.append("\t%s={" % MANG[k])
    lua.append("\t\t%s={{%s},{{1,-1},{20,-1}}}" % (ATTRIB[k], diem))
    lua.append("\t},")
lua.append("}")
lua_txt = "\r\n".join(lua) + "\r\n"
for base in (SV, CL):
    d = os.path.join(base, "script", "skill", "petsys")
    os.makedirs(d, exist_ok=True)
    p = os.path.join(d, "aura.lua")
    io.open(p, "w", encoding="latin-1", newline="").write(lua_txt)
print("ghi aura.lua (server+client)")

# ---- 2. skills.txt: 4 dong moi (khuon dong 550 - skill 549 khang partner) ----
def them(p):
    raw = open(p, "rb").read()
    if b"pet_aura_1" in raw:
        print("  skills.txt da co aura:", p)
        return
    rows = raw.split(b"\n")
    du = b""
    if rows and rows[-1] == b"":
        rows = rows[:-1]
        du = b"\n"
    hdr = rows[0].split(b"\t")
    idx = {h.decode("latin-1").strip(): j for j, h in enumerate(hdr)}
    khuon = rows[550].rstrip(CR).split(b"\t")
    for k in range(4):
        c = list(khuon)
        c[0] = MA(TEN[k])
        c[idx["SkillId"]] = str(ID0 + k).encode()
        c[idx["MaxLevel"]] = b"20"
        c[idx["ReqLevel"]] = b"1"
        c[idx["SkillIcon"]] = (BS + "spr" + BS + "Ui3" + BS + "pet" + BS +
                               ["skill_enhancehit", "skill_attack",
                                "skill_resist_all", "skill_block"][k]).encode("latin-1")
        c[idx["LvlSetScript"]] = (BS + "script" + BS + "skill" + BS + "petsys" +
                                  BS + "aura.lua").encode("latin-1")
        c[idx["LvlSetting1"]] = ATTRIB[k].encode("latin-1")
        c[idx["LvlData1"]] = MANG[k].encode("latin-1")
        # xoa cac LvlSetting/Data 2.. thua tu khuon
        for m in range(2, 21):
            for key in ("LvlSetting%d" % m, "LvlData%d" % m):
                if key in idx:
                    c[idx[key]] = b"0"
        c[idx["SkillDesc"]] = MA("Trợ lực từ Bạn Đồng Hành")
        rows.append(b"\t".join(c) + CR)
    if not os.path.exists(p + ".truoc_petsys"):
        shutil.copyfile(p, p + ".truoc_petsys")
    open(p, "wb").write(b"\n".join(rows) + du)
    print("  them 4 skill aura:", p, "->", len(rows), "dong")


them(os.path.join(SV, "settings", "skills.txt"))
them(os.path.join(CL, "settings", "skills.txt"))
print("XONG p19a (aura id %d..%d)" % (ID0, ID0 + 3))
