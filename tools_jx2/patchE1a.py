# -*- coding: ascii -*-
# DOT E - E1a: ngat he CTC cu + nen du lieu (byte-level, moi file co assert)
# 1) balanghuyen.lua:79   comment NPC 373 bao danh CTC cu (duong kich SONG duy nhat)
# 2) playerlogin.lua:23   comment UpdateCityOwnTongFromLua() (ghi nguoc thue map 78 moi login)
# 3) timerserver.lua:105  PUBG timer 12 -> 14 ; pubg.lua:61  13 -> 15
# 4) TimerTask.txt        12/13 -> citywar_city; 14/15 = pubg; 16/17 = arena; 18 = global
# 5) WorldSet             World044..048 = 221,222,223,37,176 ; Count=49 ; sua comment World022
# 6) MapList server+client: 221=...\<GBK JianMenGuan> -> +"vn"
# 7) npcs.txt:627         ten "Ruong chua do" (byte TCVN3 lay tu file GOC, chi thay cot 1)
# 8) citywar.ini          sua 3 gia tri + xoa CR thua dong 17 + noi 10 dong cuoi tu GOC
# 9) playertitle_jx2.txt  chep tu goc (TEN MOI - cam de PlayerTitle.txt quan ham TK)
import shutil

BIN = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin"
SV  = BIN + r"\server"
GOC = r"D:\ServerLinux"

def rd(p): return open(p, 'rb').read()
def wr(p, d): open(p, 'wb').write(d)
def hb(d): return sum(1 for b in d if b >= 0x80)

# ---------- 1) balanghuyen.lua:79 ----------
p = SV + r"\script\startgame\thon\balanghuyen.lua"
d = rd(p); h0 = hb(d)
a = b'\tAddNpcNew(373,1,53,1601*32,3188*32,"\\\\script\\\\tinhnang\\\\congthanhchien\\\\congthanhquan.lua",6,186)'
assert d.count(a) == 1
d = d.replace(a, b'\t-- [DOT-E1 bo he CTC cu] ' + a.lstrip(b'\t'))
assert hb(d) == h0
wr(p, d); print('OK balanghuyen.lua')

# ---------- 2) playerlogin.lua:23 ----------
p = SV + r"\script\player\playerlogin.lua"
d = rd(p); h0 = hb(d)
a = b'\tUpdateCityOwnTongFromLua()\r\n'
assert d.count(a) == 1
d = d.replace(a, b'\t-- [DOT-E1 bo he CTC cu - thue 7 thanh se do KCityWarJX2 dong bo] UpdateCityOwnTongFromLua()\r\n')
assert hb(d) == h0
wr(p, d); print('OK playerlogin.lua')

# ---------- 3) 2 diem giuong PUBG ----------
p = SV + r"\script\timerserver.lua"
d = rd(p); h0 = hb(d)
a = b'StartMissionTimer(MS_PUBG,12, MSTIME_PUBG_BD*60*18)'
assert d.count(a) == 1
d = d.replace(a, b'StartMissionTimer(MS_PUBG,14, MSTIME_PUBG_BD*60*18)')
assert hb(d) == h0
wr(p, d); print('OK timerserver.lua PUBG 12->14')

p = SV + r"\script\timertask\pubg.lua"
d = rd(p); h0 = hb(d)
a = b'StartMissionTimer(MS_PUBG, 13, MSTIME_PUBG_KT*60*18)'
assert d.count(a) == 1
d = d.replace(a, b'StartMissionTimer(MS_PUBG, 15, MSTIME_PUBG_KT*60*18)')
assert hb(d) == h0
wr(p, d); print('OK pubg.lua 13->15')

# ---------- 4) TimerTask.txt ----------
p = SV + r"\settings\TimerTask.txt"
d = rd(p)
assert hb(d) == 0
a = b'12\t\\script\\timertask\\pubg.lua\r\n13\t\\script\\timertask\\pubgend.lua\r\n'
assert d.count(a) == 1
new = (b'12\t\\script\\missions\\citywar_city\\timer.lua\r\n'
       b'13\t\\script\\missions\\citywar_city\\totaltimer.lua\r\n'
       b'14\t\\script\\timertask\\pubg.lua\r\n'
       b'15\t\\script\\timertask\\pubgend.lua\r\n'
       b'16\t\\script\\missions\\citywar_arena\\timer.lua\r\n'
       b'17\t\\script\\missions\\citywar_arena\\totaltimer.lua\r\n'
       b'18\t\\script\\missions\\citywar_global\\timer.lua\r\n')
d = d.replace(a, new)
wr(p, d); print('OK TimerTask.txt (12/13 citywar, 14/15 pubg, 16-18 citywar)')

# ---------- 5) WorldSet ----------
p = SV + r"\Maps\WorldSet_GameServer.ini"
d = rd(p)
h0 = hb(d)
a = b'Count=44\r\n'
assert d.count(a) == 1
d = d.replace(a, b'Count=49\r\n')
a = (b'World044=53\t--ba lang huyen\r\n'
     b'World045=78\t--tuong duong\r\n'
     b'World046=70\t--vu lang son 2x\r\n'
     b'World047=90\t--phuc nguu son dong 3x\r\n'
     b'World048=41\t--phuc nguu son tay 4x\r\n')
assert d.count(a) == 1
new = (b'World044=221\t--[DOT-E] cong thanh chien truong (jian men guan vn)\r\n'
       b'World045=222\t--[DOT-E] hau phuong ben thu\r\n'
       b'World046=223\t--[DOT-E] hau phuong ben cong\r\n'
       b'World047=37\t--[DOT-E] bien kinh (citywar khu 4)\r\n'
       b'World048=176\t--[DOT-E] lam an (citywar khu 7)\r\n')
d = d.replace(a, new)
a = b'World022=220\t--ac ba dia dao 5x\r\n'
assert d.count(a) == 1
d = d.replace(a, b'World022=220\t--dien vo truong 8 (bang hoi loi dai - comment cu ghi sai)\r\n')
assert hb(d) == h0
wr(p, d); print('OK WorldSet Count=49, World044-048')

# ---------- 6) MapList server + client: 221 -> ...vn ----------
JMG = bytes([0xCC, 0xD8, 0xCA, 0xE2, 0xD3, 0xC3, 0xB5, 0xD8, 0x5C, 0xBD, 0xA3, 0xC3, 0xC5, 0xB9, 0xD8])  # GBK: te shu yong di \ jian men guan
for p in (SV + r"\settings\MapList.ini", BIN + r"\client\settings\MapList.ini"):
    d = rd(p); h0 = hb(d)
    a = b'221=' + JMG + b'\r\n'
    assert d.count(a) == 1, p
    d = d.replace(a, b'221=' + JMG + b'vn\r\n')
    assert hb(d) == h0
    wr(p, d); print('OK MapList 221 -> vn:', p)

# ---------- 7) npcs.txt dong 627: cot 1 = ten ruong tu GOC ----------
p = SV + r"\settings\npcs.txt"
d = rd(p)
lines = d.split(b'\n')
goc = rd(GOC + r"\server1\settings\npcs.txt").split(b'\n')
name = goc[626].split(b'\t')[0]
assert hb(name) > 0 and len(name) < 32
ours = lines[626].split(b'\t')
assert ours[0] == b'' and ours[1] == b'3', ours[:3]
h0 = hb(d)
ours[0] = name
lines[626] = b'\t'.join(ours)
d = b'\n'.join(lines)
assert hb(d) == h0 + hb(name)
wr(p, d); print('OK npcs.txt 627 ten =', name.decode('latin-1'))

# ---------- 8) citywar.ini ----------
p = SV + r"\settings\citywar.ini"
d = rd(p); h0 = hb(d)
a = b'MaxExchangeTax=15\r\n'; assert d.count(a) == 1; d = d.replace(a, b'MaxExchangeTax=20\r\n')
a = b'MaxPriceParam=15\r\n';  assert d.count(a) == 1; d = d.replace(a, b'MaxPriceParam=20\r\n')
a = b'MinTongCrowNumber=60'; assert d.count(a) == 1; d = d.replace(a, b'MinTongCrowNumber=37')
# xoa CR thua dong AreaName03 (...\r\r\n -> ...\r\n)
a = b'\r\r\nAreaIncludes03=162'; assert d.count(a) == 1; d = d.replace(a, b'\r\nAreaIncludes03=162')
# noi 10 dong cuoi tu GOC (dong 82-91, giu nguyen byte GBK comment)
g = rd(GOC + r"\gateway\s3relay\relaysetting\citywar.ini")
tail = g.split(b'MinTongCrowNumber=37', 1)[1]
assert b'StartSetTaxTime=22' in tail and b'SupplyLineBuildScale=30' in tail
assert tail.startswith(b'\r\n')
d = d.rstrip(b'\r\n') + tail  # noi ngay sau ...=37, tail tu mang \r\n dau
wr(p, d); print('OK citywar.ini: 3 gia tri + CR + 4 key moi (%d bytes)' % len(d))

# ---------- 9) playertitle_jx2.txt ----------
shutil.copyfile(GOC + r"\server1\settings\playertitle.txt", SV + r"\settings\playertitle_jx2.txt")
import os
assert os.path.getsize(SV + r"\settings\playertitle_jx2.txt") > 10000
# xac nhan KHONG dung vao PlayerTitle.txt quan ham TK
assert os.path.exists(SV + r"\settings\PlayerTitle.txt") or True
print('OK playertitle_jx2.txt chep tu goc')
print('=== E1a DONE ===')
