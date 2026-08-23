import re, sys
sep = chr(92)
def lines(p):
    return open(p, 'rb').read().split(b'\n')
ev = lines(r'D:\ServerLinux\server1\script\event\tongwar\event.lua')[26]
hi = lines(r'D:\ServerLinux\server1\script\event\tongwar\headinfo.lua')
for i, l in enumerate(hi):
    if b'TONGWAR_CITY' in l and b'{' in l:
        print('headinfo line', i+1, l)
        hi_l = l
print('event.lua:27', ev)
names_ev = re.findall(rb'"([^"]*)"', ev)
names_hi = re.findall(rb'"([^"]*)"', hi_l)
print('event == headinfo:', names_ev == names_hi)
for p in [r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\citywar.ini',
          r'D:\ServerLinux\gateway\s3relay\relaysetting\citywar.ini']:
    d = open(p, 'rb').read()
    m = re.search(rb'\[CityArea\](.*?)(\r?\n\[|$)', d, re.S)
    sec = m.group(1) if m else b''
    an = re.findall(rb'AreaName\s*=\s*([^\r\n]*)', sec)
    print(p)
    print('  AreaName:', an)
    print('  == TONGWAR_CITY bytes:', [x.strip() for x in an] == names_ev)
    for k, (a, b) in enumerate(zip([x.strip() for x in an], names_ev)):
        if a != b:
            print('   DIFF', k+1, a, b)
