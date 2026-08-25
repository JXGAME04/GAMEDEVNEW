# -*- coding: ascii -*-
"""C2 - doi cac diem sinh NPC sang HD3_AddNpc / HD3_AddNpcEx (ngu nghia Linux).

1) 14 cho b2 da doi 'AddNpcEx(a,b,random(0,4),...)' -> tra ve dang goc Linux
   nhung goi HD3_AddNpc: 'HD3_AddNpc(a,b,...)' (bo random - C tu random).
2) challengeoftime\\npc.lua:431 'AddNpcEx(' (ban Linux goc, p7=1 -> camp1 = ALLY
   voi nguoi choi camp1) -> 'HD3_AddNpcEx(' (khong camp, p7=bNoRevive dung goc).
3) 2 helper cua toi: autoexec_npc_hd3.lua + hd3_driver.lua -> HD3_AddNpc.
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"


def patch(rel, fn):
    p = os.path.join(JX1, rel.replace("/", os.sep))
    d = io.open(p, encoding="latin-1", newline="").read()
    d2, n = fn(d)
    if n:
        io.open(p, "w", encoding="latin-1", newline="").write(d2)
        m = os.path.join(MIRROR, rel.replace("/", os.sep))
        os.makedirs(os.path.dirname(m), exist_ok=True)
        io.open(m, "w", encoding="latin-1", newline="").write(d2)
    print("  %-52s %d cho" % (rel.rsplit("/", 1)[-1], n))
    return n


# (1) dao nguoc b2: AddNpcEx(a,b,random(0,4), -> HD3_AddNpc(a,b,
RX_REVERT = re.compile(r"\bAddNpcEx\(([^,]+),([^,]+),random\(0,4\),")
def revert(d):
    return RX_REVERT.subn(lambda m: "HD3_AddNpc(%s,%s," % (m.group(1), m.group(2)), d)

total = 0
for rel in ["script/task/tollgate/killbosshead.lua",
            "script/missions/fengling_ferry/boss.lua",
            "script/missions/fengling_ferry/fld_smalltimer.lua",
            "script/missions/fengling_ferry/mission.lua"]:
    total += patch(rel, revert)
assert total == 14, "doi nguoc %d cho (can 14)" % total

# (2) npc.lua: AddNpcEx -> HD3_AddNpcEx (ca loi goi lan chuoi bao loi debug)
def fix_npc(d):
    n = 0
    d, c = re.subn(r"= AddNpcEx\(", "= HD3_AddNpcEx(", d)
    n += c
    d, c = re.subn(r'"Failed to AddNpcEx', '"Failed to HD3_AddNpcEx', d)
    n += c
    return d, n
patch("script/missions/challengeoftime/npc.lua", fix_npc)

# (3) helpers
def fix_helper(d):
    return re.subn(r"\bAddNpcEx\(", "HD3_AddNpc(", d)
patch("script/global/autoexec_npc_hd3.lua", fix_helper)
patch("script/tinhnang/3hoatdong/hd3_driver.lua", fix_helper)

# (3b) helper boatman: bo random(0,4) thua (HD3_AddNpc khong nhan series)
def fix_driver_args(d):
    # HD3_AddNpc(240, 1, random(0,4), nIdx, ...) -> HD3_AddNpc(240, 1, nIdx, ...)
    return re.subn(r"HD3_AddNpc\(240, 1, random\(0,4\), ", "HD3_AddNpc(240, 1, ", d)
patch("script/tinhnang/3hoatdong/hd3_driver.lua", fix_driver_args)

def fix_npc769_args(d):
    # HD3_AddNpc(it[1], 1, random(0,4), SId, ...) -> HD3_AddNpc(it[1], 1, SId, ...)
    return re.subn(r"HD3_AddNpc\(it\[1\], 1, random\(0,4\), ", "HD3_AddNpc(it[1], 1, ", d)
patch("script/global/autoexec_npc_hd3.lua", fix_npc769_args)
print("xong C2")
