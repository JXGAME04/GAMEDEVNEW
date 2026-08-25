# -*- coding: utf-8 -*-
"""B3 — sinh 2 tep Lua PHU cho driver (byte TCVN3 lay thang tu Linux):
  1) script\\global\\autoexec_npc_hd3.lua : bang 7 NPC 769 + add_dialognpc_hd3()
  2) script\\missions\\fengling_ferry\\hd3_thuyenphu.lua : wrapper dat BOATID
Ghi vao ca JX1 lan MIRROR. Chi ASCII + byte TCVN3 goc (khong dung Edit/Write).
"""
import io, os, re, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")

LNX = r"D:\ServerLinux\server1"
JX1 = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MIRROR = r"D:\GAMEDEVNEW\serverscript_jx2\3hoatdong"


def write_both(rel, data_bytes):
    for base in (JX1, MIRROR):
        p = os.path.join(base, rel.replace("/", os.sep))
        os.makedirs(os.path.dirname(p), exist_ok=True)
        open(p, "wb").write(data_bytes)
    print("  ghi", rel, len(data_bytes), "byte")


# ---- 1) trich 7 dong NPC 769 tu autoexec_npc.lua (giu nguyen byte TCVN3) ----
src = open(os.path.join(LNX, "script", "global", "autoexec_npc.lua"), "rb").read()
rows = []
for ln in src.split(b"\n"):
    s = ln.strip()
    # dong bat dau bang {769, ... nieshichen.lua ...}
    if s.startswith(b"{769,") and b"nieshichen.lua" in s:
        rows.append(s.rstrip(b","))
assert len(rows) == 7, "tim thay %d dong NPC 769 (can 7)" % len(rows)

head = (
b"-- ============================================================================\r\n"
b"-- AUTOEXEC_NPC_HD3.LUA - 7 NPC 769 \"Nhiep Thi Tran\" cua he SAN BOSS SAT THU\r\n"
b"-- Trich NGUYEN BYTE tu ban Linux (autoexec_npc.lua). add_dialognpc_hd3 dung\r\n"
b"-- AddNpcEx (khong dung AddNpc - tranh loi ngu hanh Kim, xem BANGIAO 3HD).\r\n"
b"-- Cot: {npcId, mapId, x, y, script, name}\r\n"
b"-- ============================================================================\r\n"
b"adddialognpc_hd3 = {\r\n"
)
body = b""
for r in rows:
    body += b"\t" + r + b",\r\n"
tail = (
b"}\r\n\r\n"
b"function add_dialognpc_hd3(Tab)\r\n"
b"\tfor i = 1, getn(Tab) do\r\n"
b"\t\tlocal it = Tab[i]\r\n"
b"\t\tlocal SId = SubWorldID2Idx(it[2])\r\n"
b"\t\tif (SId >= 0) then\r\n"
b"\t\t\tlocal npc = AddNpcEx(it[1], 1, random(0,4), SId, it[3]*32, it[4]*32, 0, it[6])\r\n"
b"\t\t\tif (npc ~= nil and npc > 0) then\r\n"
b"\t\t\t\tSetNpcScript(npc, it[5])\r\n"
b"\t\t\tend\r\n"
b"\t\tend\r\n"
b"\tend\r\n"
b"end\r\n"
)
write_both("script/global/autoexec_npc_hd3.lua", head + body + tail)

# ---- 2) wrapper thuyen phu: dat BOATID = GetNpcValue roi goi fld_wanttakeboat ----
wrap = (
b"-- ============================================================================\r\n"
b"-- HD3_THUYENPHU.LUA - wrapper NPC thuyen phu Phong Lang Do (logic 100% Linux).\r\n"
b"-- Dat bien toan cuc BOATID theo gia tri NPC (1/2/3) roi goi fld_wanttakeboat\r\n"
b"-- cua ban Linux (fld_head.lua). Toan bo dang ky/le vat do fld_head xu ly.\r\n"
b"-- ============================================================================\r\n"
b"Include(\"\\\\script\\\\missions\\\\fengling_ferry\\\\fld_head.lua\")\r\n"
b"\r\n"
b"function main(NpcIndex)\r\n"
b"\tBOATID = GetNpcValue()\r\n"
b"\tif (BOATID == nil or BOATID < 1 or BOATID > 3) then BOATID = 1 end\r\n"
b"\tfld_wanttakeboat(BOATID)\r\n"
b"end\r\n"
)
write_both("script/missions/fengling_ferry/hd3_thuyenphu.lua", wrap)

print("B3 helpers: xong 2 tep.")
