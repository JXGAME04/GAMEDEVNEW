# -*- coding: utf-8 -*-
"""Noi day Tin Su vao du an: tinsu_addnpc.lua (NPC ai), startgame.lua, dichquan.lua, station.lua,
go tinhnang\thienbaokho. Tat ca doc/ghi latin-1 newline=''."""
import io, os, re, shutil, sys
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
BS = chr(92)
SRV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
SC = os.path.join(SRV, "script")
LNX = r"D:\ServerLinux\server1\script"
BK = os.path.join(SRV, "_backup_tinsu_2108")


def rd(p): return io.open(p, "r", encoding="latin-1", newline="").read()
def wr(p, s): io.open(p, "w", encoding="latin-1", newline="").write(s)
def hi(s): return sum(1 for c in s if ord(c) > 127)
def L(*a): return BS * 2 + (BS * 2).join(a)   # duong dan lua co escape


# ---------- 1. tinsu_addnpc.lua: chep NGUYEN BYTE 2 khoi NPC cua global\autoexec_npc.lua Linux ----------
src = rd(os.path.join(LNX, r"global\autoexec_npc.lua")).split("\n")
# dong 3..11 (3 ai: Canh trang / Nap Lan Thanh Thanh / Tieu Tran) + 13..23 (Dich quan 3 ai)
rows = [l.rstrip("\r") for l in src[2:11] + src[12:15] + src[16:19] + src[20:23]]
assert len(rows) == 18 and all(l.lstrip().startswith("{") for l in rows), rows[:2]
assert all((",387," in l or ",388," in l or ",389," in l or ",390," in l or ",391," in l or ",392," in l
            or ",393," in l or ",394," in l or ",395," in l) for l in rows)
body = "\r\n".join([
    "-- ============================================================",
    "-- TIN SU (port Linux 21/08): dat NPC cho 3 ai Tin Su + quai/bao ruong cua ai",
    "-- Linux: global\\autoexec.lua main() goi add_alltollgatenpc() (task\\tollgate\\addtollgatenpc.lua)",
    "-- va add_dialognpc(adddialognpc) voi bang trong global\\autoexec_npc.lua. Du an khong co",
    "-- autoexec.lua -> gom vao day, startgame.lua goi tinsu_addnpc() (thay addnpcthienbaokho() cu).",
    "-- Bang tinsu_dialognpc = chep NGUYEN VAN dong 3-11 + 13-23 cua autoexec_npc.lua Linux",
    "-- (map 387-394 du an chua co .wor -> SubWorldID2Idx < 0 -> tu bo qua, giong Linux).",
    "-- ============================================================",
    'Include("' + L("script", "task", "tollgate", "addtollgatenpc.lua") + '")',
    "",
    "tinsu_dialognpc = {",
] + rows + [
    "}",
    "",
    "-- y het add_dialognpc() cua global\\autoexec.lua Linux (dong 233-242)",
    "function tinsu_add_dialognpc(Tab)",
    "\tfor i = 1 , getn(Tab) do ",
    "\t\tlocal itemlist = Tab[i]",
    "\t\tlocal SId = SubWorldID2Idx(itemlist[2]);",
    "\t\tif (SId >= 0) then",
    "\t\t\tlocal npcindex = AddNpc(itemlist[1], 1, SId, itemlist[3] * 32, itemlist[4] * 32, 0, itemlist[6]);",
    "\t\t\tSetNpcScript(npcindex, itemlist[5]);",
    "\t\tend;",
    "\tend\t",
    "end;",
    "",
    "function tinsu_addnpc()",
    "\tadd_alltollgatenpc();\t\t\t\t-- 9 Bao Ruong + 9 Bao Kho Thu Ho Gia map 395 (killbosshead.lua)",
    "\ttinsu_add_dialognpc(tinsu_dialognpc);\t-- Tieu Tran + Dich quan trong ai",
    "end",
    "",
])
p = os.path.join(SC, r"task\tollgate\tinsu_addnpc.lua")
wr(p, body); print("  tao task/tollgate/tinsu_addnpc.lua (%d dong NPC, %d byte cao)" % (len(rows), hi(body)))

# ---------- 2. startgame.lua ----------
p = os.path.join(SC, "startgame.lua"); s = rd(p); h = hi(s)
old = 'Include("' + L("script", "tinhnang", "thienbaokho", "addnpctbk.lua") + '")'
assert s.count(old) == 1, s.count(old)
s = s.replace(old, 'Include("' + L("script", "task", "tollgate", "tinsu_addnpc.lua") + '")\t-- [TIN SU 21/08] thay tinhnang\\thienbaokho (Ken Nguyen) bang ban Linux')
assert s.count("\taddnpcthienbaokho()") == 1
s = s.replace("\taddnpcthienbaokho()", "\ttinsu_addnpc()\t-- [TIN SU 21/08] NPC ai Thien Bao Kho ban Linux (thay addnpcthienbaokho)")
assert hi(s) == h
wr(p, s); print("  startgame.lua: Include + goi tinsu_addnpc()")

# ---------- 3. dichquan.lua (Dich Quan 7 thanh + Ba Lang Huyen) ----------
p = os.path.join(SC, r"global\npcchucnang\dichquan.lua"); s = rd(p); h = hi(s)
old = 'Include("' + L("script", "lib", "lib_map.lua") + '")'
assert s.count(old) == 1
s = s.replace(old, old + '\r\nInclude("' + L("script", "task", "tollgate", "messenger", "posthouse.lua") + '")\t-- [TIN SU 21/08] luong Tin Su ban Linux (especiallymessenger)')
# menu: "Thien bao kho /thienbao" -> "Nhiem vu Tin Su/especiallymessenger" (chuoi TCVN3: thay theo byte)
m = re.search(r'\t\t"[^"]*/thienbao",\r?\n', s)
assert m, "khong thay dong menu /thienbao"
line = m.group(0)
s = s.replace(line, '\t\t"Nhi\xd6m v\xf4 T\xddn S\xf8/especiallymessenger",\t-- [TIN SU 21/08] thay "Thien bao kho /thienbao" (tinh nang cu da go)\r\n', 1)
# danh dau ham cu thienbao/gothienbao/phanthuong = khong con duoc goi (giu nguyen de doi chieu)
s = s.replace("function thienbao()", "-- [TIN SU 21/08] 3 ham duoi (thienbao/gothienbao/phanthuong) la Thien Bao Kho tu che cu, KHONG con trong menu\r\nfunction thienbao()", 1)
wr(p, s); print("  dichquan.lua: menu -> especiallymessenger (byte cao %d -> %d)" % (h, hi(s)))

# ---------- 4. station.lua (Xa Phu): them muc 'Di noi dac biet lam Nhiem vu Tin Su' nhu Linux muc 9 ----------
p = os.path.join(SC, r"global\station.lua"); s = rd(p); h = hi(s)
old = 'Include("' + L("script", "missions", "citywar_global", "station_ctc.lua") + '")'
assert s.count(old) == 1
s = s.replace(old, old + '\r\nInclude("' + L("script", "task", "tollgate", "messenger", "wagoner.lua") + '")\t-- [TIN SU 21/08] messenger_wagoner (Linux station.lua muc 9)', 1)
wr(p, s); print("  station.lua: Include wagoner.lua")
# NPC Xa Phu that = global\npcchucnang\xaphu.lua (Include station.lua) -> them muc menu, chu TCVN3 lay NGUYEN BYTE tu Linux
lnx_st = rd(os.path.join(LNX, r"global\station.lua"))
m = re.search(r'"([^"]+)/messenger_wagoner"', lnx_st)
assert m, "khong thay muc 9 trong station.lua Linux"
label = m.group(1).strip()
p = os.path.join(SC, r"global\npcchucnang\xaphu.lua"); s = rd(p); h = hi(s)
old = '"C\xb8c h\xb9 mu\xe8n \xae i \xae\xa9u?", 10, '
if old not in s:
    m2 = re.search(r'Say\("[^"]*", 10, ', s)
    assert m2, "khong thay Say(...,10,"
    old = m2.group(0)
s = s.replace(old, old.replace(", 10, ", ", 11, "), 1)
m3 = re.search(r'(\t\t"[^"]*/godatau",\r?\n)', s)
assert m3
s = s.replace(m3.group(1), m3.group(1) + '\t\t"' + label + '/messenger_wagoner",\t-- [TIN SU 21/08] Linux station.lua muc 9\r\n', 1)
assert hi(s) == h + hi(label)
wr(p, s); print("  xaphu.lua: menu 10 -> 11, them '.../messenger_wagoner'")

# ---------- 5. go tinhnang\thienbaokho -> backup ----------
src = os.path.join(SC, r"tinhnang\thienbaokho"); dst = os.path.join(BK, r"script\tinhnang\thienbaokho")
if os.path.isdir(src):
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    shutil.move(src, dst); print("  da doi tinhnang/thienbaokho -> _backup_tinsu_2108/script/tinhnang/thienbaokho")
print("OK")
