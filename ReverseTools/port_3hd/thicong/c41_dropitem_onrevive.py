# -*- coding: utf-8 -*-
"""C41 - 2 loi tren man hinh chu game:

(A) "KItemSet::AddItemSet2 khong tim thay nItemGenre=51563 / 51089"
    LECH CHU KY DropItem giua hai engine:
      Linux : DropItem(world, x, y, nPlayerIdx, genre, detail, particular, level, series, luck, ...)
      JX1   : DropItem(nNpcIndex, genre, detail, particular, level, series, luck [, magiclevel])
              (ScriptFuns.cpp LuaDropItem: tham so 1 = NPC index, 2 = genre)
    => tham so 2 cua loi goi Linux la toa do X (vd 51563) bi JX1 hieu la GENRE
       => AddItemSet2 khong tim thay genre => KHONG RA DO + spam loi.
    Va: doi 6 loi goi sang dung chu ky JX1 (giu nguyen genre/detail/particular/
    level/series/luck; vi tri roi = ngay tai NPC do, dung y ban goc vi ban goc
    cung lay x,y,world tu chinh NPC do).

(B) ScriptError "(shuizeideath.lua) cFuncName:(OnRevive)" moi lan quai hoi sinh:
    engine JX1 goi OnRevive cho MOI NPC co ActionScript/DeathScript; ban Linux
    khong dinh nghia ham nay (engine JX2 khong goi) => "attempt to call a nil
    value" lap lai. Them ham RONG (cung khuon da xu ly cho kill_level.lua).
"""
import io, os, re, sys, shutil
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"

# (tep, bien NPC index dung cho DropItem)
SUA = {
    r"script\missions\fengling_ferry\bossdeath.lua": "nNpcIndex",
    r"script\missions\fengling_ferry\shuizeideath.lua": "nNpcIndex",
    r"script\missions\challengeoftime\award.lua": "index",
}
# cac tep script CHET cua 3 hoat dong can co OnRevive rong
CAN_ONREVIVE = [
    r"script\missions\fengling_ferry\shuizeideath.lua",
    r"script\missions\fengling_ferry\bossdeath.lua",
    r"script\missions\fengling_ferry\fld_death.lua",
    r"script\missions\challengeoftime\npc_death.lua",
    r"script\missions\challengeoftime\player_death.lua",
]

RE_DROP = re.compile(r"DropItem\(\s*world\s*,\s*x\s*,\s*y\s*,\s*[^,]+,\s*([^)]*)\)")


def sync(rel):
    dst = os.path.join(MIR, rel)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.copyfile(os.path.join(JX1, rel), dst)


# ---------- (A) DropItem ----------
tongA = 0
for rel, bien in SUA.items():
    p = os.path.join(JX1, rel)
    d = io.open(p, encoding="latin-1", newline="").read()

    def thay(m):
        global tongA
        args = [a.strip() for a in m.group(1).split(",")]
        # args[0..5] = genre, detail, particular, level, series, luck
        if len(args) < 6:
            return m.group(0)
        tongA += 1
        return "DropItem(%s, %s)" % (bien, ", ".join(args[:6]))

    d2 = RE_DROP.sub(thay, d)
    if d2 != d:
        io.open(p, "w", encoding="latin-1", newline="").write(d2)
        sync(rel)
        print("  OK", rel.rsplit("\\", 1)[-1])
print("(A) da doi %d loi goi DropItem sang chu ky JX1" % tongA)

# ---------- (B) OnRevive rong ----------
tongB = 0
for rel in CAN_ONREVIVE:
    p = os.path.join(JX1, rel)
    if not os.path.exists(p):
        print("  (khong co)", rel)
        continue
    d = io.open(p, encoding="latin-1", newline="").read()
    if re.search(r"^function\s+OnRevive\s*\(", d, re.M):
        continue
    nl = "\r\n" if "\r\n" in d else "\n"
    d = d.rstrip("\r\n") + nl + nl.join([
        "",
        "-- [3HD 25/08 C41] Engine JX1 goi OnRevive cho MOI NPC co script chet moi lan",
        "-- hoi sinh; ban Linux khong dinh nghia (engine JX2 khong goi) => ScriptError",
        "-- \"attempt to call a nil value\" lap lai moi nhip. Ham rong = giu nguyen hanh vi.",
        "function OnRevive()",
        "end",
        "",
    ])
    io.open(p, "w", encoding="latin-1", newline="").write(d)
    sync(rel)
    tongB += 1
    print("  OK OnRevive ->", rel.rsplit("\\", 1)[-1])
print("(B) da them OnRevive rong vao %d tep" % tongB)
