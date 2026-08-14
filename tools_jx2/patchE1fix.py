# -*- coding: ascii -*-
# E1-fix (theo phan bien E1+E2): PUBG doi timer 12/13->14/15 con sot diem TAT + DOC
# + ngat wrapper CTC cu infocenter.lua (chiem ten file citywar goc)
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"

def rd(p): return open(p, 'rb').read()
def wr(p, d): open(p, 'wb').write(d)
def hb(d): return sum(1 for b in d if b >= 0x80)

def patch(path, pairs):
    d = rd(path); h0 = hb(d)
    for a, b in pairs:
        assert d.count(a) == 1, (path, a)
        d = d.replace(a, b)
    assert hb(d) == h0
    wr(path, d)
    print('OK', path)

# 1) pubg.lua: diem TAT (:25) + diem DOC (:7)
patch(SV + r"\script\timertask\pubg.lua", [
    (b'StopMissionTimer(MS_PUBG,12)', b'StopMissionTimer(MS_PUBG,14)'),
    (b'GetMSRestTime(MS_PUBG,12)', b'GetMSRestTime(MS_PUBG,14)'),
])
# 2) pubgend.lua: diem TAT (:26) + diem DOC (:6)
patch(SV + r"\script\timertask\pubgend.lua", [
    (b'StopMissionTimer(MS_PUBG, 13)', b'StopMissionTimer(MS_PUBG, 15)'),
    (b'GetMSRestTime(MS_PUBG,12)', b'GetMSRestTime(MS_PUBG,14)'),
])
# 3) pubgutils.lua:69 doc timer ket thuc
patch(SV + r"\script\tinhnang\pubg\pubgutils.lua", [
    (b'GetMSRestTime(MS_PUBG,13)', b'GetMSRestTime(MS_PUBG,15)'),
])
# 4) hotrotest.lua:75
patch(SV + r"\script\npcthon\balanghuyen\hotrotest.lua", [
    (b'GetMSRestTime(MS_PUBG,12)', b'GetMSRestTime(MS_PUBG,14)'),
])
# 5) infocenter.lua: ngat Include CTC cu (file se duoc thay ruot ban goc o E5)
p = SV + r"\script\missions\citywar_global\infocenter.lua"
d = rd(p); h0 = hb(d)
a = b'Include("\\\\script\\\\tinhnang\\\\congthanhchien\\\\congthanhquan.lua")'
assert d.count(a) == 1
d = d.replace(a, b'-- [DOT-E1 bo he CTC cu - E5 thay ruot ban goc Linux] ' + a)
assert hb(d) == h0
wr(p, d)
print('OK', p)

# kiem het: khong con diem nao dung timer id 12/13 cua MS_PUBG
import os, re
bad = []
for root, ds, fs in os.walk(SV + r"\script"):
    for f in fs:
        if not f.lower().endswith('.lua'):
            continue
        fp = os.path.join(root, f)
        d = rd(fp)
        for m in re.finditer(rb'(StartMissionTimer|StopMissionTimer|GetMSRestTime)\s*\(\s*MS_PUBG\s*,\s*(1[23])\b', d):
            bad.append((fp, m.group(0)))
assert not bad, bad
print('QUET SACH: khong con MS_PUBG voi timer 12/13')
print('=== E1-fix DONE ===')
