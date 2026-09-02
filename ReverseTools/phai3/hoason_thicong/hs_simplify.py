# -*- coding: utf-8 -*-
"""hs_simplify.py [HOASON 01/09 b] - chu quyet: KHONG can nhiem vu, nhan ky nang nhu 9 phai co san.
- npc_hoason.lua: chi con Hoa Son Kiem Khach 8 thon + Nam Cung Tuyet / Lan Hao Thien / Ruong o map 987, script = npcmonphai\\hoason.lua
- 74 tep nhiem vu huashan2013 (tru npc_hoason.lua, hs_shim.lua, trap\\) chuyen ra kho ReverseTools (khong nap luc boot)
- npcmonphai\\hoason.lua: go() = gianhapmonphai(10) nhu conlon.lua
- factionhead.lua hotrokn: nCurFac > 10 -> > 12
"""
import io, os, sys, shutil
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes
MARK = "[HOASON 01/09]"
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
KHO = r"D:\GAMEDEVNEW\ReverseTools\phai3\hoason_thicong\huashan2013_nhiemvu_linux_khongdung"
def V(s): return unicode_to_tcvn3_bytes(s).decode("latin-1")
def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, d):
    os.makedirs(os.path.dirname(p), exist_ok=True)
    io.open(p, "w", encoding="latin-1", newline="").write(d); print("  ghi", p)
CRLF = "\r\n"
SCRIPT = "\\\\script\\\\npcthon\\\\npcmonphai\\\\hoason.lua"
KK = V("Hoa Sơn Kiếm Khách")

# 1. npc_hoason.lua
d = CRLF.join([
    "-- " + MARK + " NPC Hoa Son - RUT GON theo y chu (khong nhiem vu, nhan ky nang nhu 9 phai co san).",
    "-- Toa do 8 thon + map 987 lay nguyen bang Linux script\\global\\huashan2013\\npc_hoason.lua (hoason_parserby).",
    "-- Quai/NPC nhiem vu Linux (2090 luyen kiem, 2092 cuong dao, 2097 Son Phi, 2103/2108/2110, Van Tu Vien...) KHONG spawn;",
    "-- kho luu: ReverseTools\\phai3\\hoason_thicong\\huashan2013_nhiemvu_linux_khongdung.",
    "hoason_parserby = {",
    "{2096,53,1632,3191,\"" + SCRIPT + "\",\"" + KK + "\"},",
    "{2096,20,3567,6190,\"" + SCRIPT + "\",\"" + KK + "\"},",
    "{2096,99,1641,3189,\"" + SCRIPT + "\",\"" + KK + "\"},",
    "{2096,100,1653,3129,\"" + SCRIPT + "\",\"" + KK + "\"},",
    "{2096,101,1693,3167,\"" + SCRIPT + "\",\"" + KK + "\"},",
    "{2096,121,1966,4508,\"" + SCRIPT + "\",\"" + KK + "\"},",
    "{2096,153,1638,3240,\"" + SCRIPT + "\",\"" + KK + "\"},",
    "{2096,174,1596,3271,\"" + SCRIPT + "\",\"" + KK + "\"},",
    "{2098,987,1420,3049,\"" + SCRIPT + "\",\"" + V("Nam Cung Tuyết") + "\"},\t-- chuong mon, cung menu nhu De Tu o thon",
    "{2096,987,1358,3051,\"" + SCRIPT + "\",\"" + V("Lận Hạo Thiên") + "\"},",
    "{625,987,1391,3114,\"\\\\script\\\\global\\\\npcchucnang\\\\ruongchua.lua\",\"" + V("Rương chứa đồ") + "\"},",
    "}",
    "",
    "function add_npc_hoason()",
    "\thoason_hoasonbynpc(hoason_parserby)",
    "end",
    "",
    "function hoason_hoasonbynpc(Tab)",
    "\tfor i = 1 , getn(Tab) do",
    "\t\tlocal SId = SubWorldID2Idx(Tab[i][2]);",
    "\t\tif (SId >= 0) then",
    "\t\t\tlocal npcindex = AddNpc(Tab[i][1],1,SId,Tab[i][3]*32,Tab[i][4]*32,1,Tab[i][6]);",
    "\t\t\tif (npcindex > 0) then",
    "\t\t\t\tSetNpcScript(npcindex, Tab[i][5]);",
    "\t\t\t\tSetNpcValue(npcindex, 10);",
    "\t\t\tend",
    "\t\tend;",
    "\tend",
    "end;",
    ""])
wr(os.path.join(SRV, r"script\global\huashan2013\npc_hoason.lua"), d)

# 2. chuyen tep nhiem vu ra kho
root = os.path.join(SRV, r"script\global\huashan2013")
giu = {"npc_hoason.lua", "hs_shim.lua"}
n = 0
for f in sorted(os.listdir(root)):
    p = os.path.join(root, f)
    if os.path.isdir(p) or f in giu or not f.endswith(".lua"): continue
    os.makedirs(KHO, exist_ok=True)
    shutil.move(p, os.path.join(KHO, f)); n += 1
print("  chuyen %d tep nhiem vu -> %s" % (n, KHO))
print("  con lai:", sorted(os.listdir(root)))

# 3. npcmonphai\hoason.lua: go() rut gon
p = os.path.join(SRV, r"script\npcthon\npcmonphai\hoason.lua")
d = rd(p)
old = "function go()\r\n\tgianhapmonphai(10)\r\n\tSetLastFactionNumber(10)\r\n\tSetTask(3481, 10*256)\t-- bien nhiem vu mon phai Hoa Son (Linux nTaskId_Fact)\r\n\tSetRevPos(987,1)\r\nend;"
assert d.count(old) == 1, "hoason.lua go() neo"
d = d.replace(old, "function go()\r\n\tgianhapmonphai(10)\t-- nhu conlon.lua: SetFaction/Camp/Rank + hockynang (SKILLNORMAL[11] 15 chieu 10-70)\r\nend;")
wr(p, d)

# 4. factionhead hotrokn: > 10 -> > 12
p = os.path.join(SRV, r"script\header\factionhead.lua")
d = rd(p)
old = "\tif(nCurFac < 0 or nCurFac > 10) then"
if d.count(old) == 1:
    d = d.replace(old, "\tif(nCurFac < 0 or nCurFac > 12) then\t-- " + MARK + " 13 phai")
    wr(p, d)
else: print("  hotrokn: neo", d.count(old))
print("XONG")
