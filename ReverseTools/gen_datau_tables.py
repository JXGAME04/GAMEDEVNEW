# -*- coding: utf-8 -*-
# gen_datau_tables.py - sinh KDaTauTables.h cho auto Da Tau (WAuto/ExtAuto)
# Doc bang tasklink_*.txt + trich marker TCVN3 truc tiep tu cac file Lua server
# (anchor theo ten callback ASCII de bao dam byte-chuan). Moi marker deu assert.
# Chay:  python gen_datau_tables.py   -> ghi D:\GAMEDEVNEW\Sources\Core\Src\KDaTauTables.h
import re, sys, io

ROOT = r"D:\GAMEDEVNEW"
TASKDIR = ROOT + r"\serverscript_jx2\datau_tasklink\settings\task"
SEA  = ROOT + r"\serverscript_jx2\datau_tasklink\script\global\seasonnpc.lua"
HEAD = ROOT + r"\serverscript_jx2\datau_tasklink\script\task\newtask\tasklink\tasklink_head.lua"
GOODS= ROOT + r"\serverscript_jx2\datau_tasklink\script\item\tasklink_goods.lua"
XAPHU= ROOT + r"\serverscript_jx2\jx1_edits\xaphu.lua"
MAPIDX=ROOT + r"\serverscript_jx2\datau_tasklink\script\task\newtask\map_index.lua"
NAMEDEF=ROOT + r"\Sources\Core\Src\CoreUseNameDef.h"
OUT  = ROOT + r"\Sources\Core\Src\KDaTauTables.h"

def rd(p):
    return open(p, 'rb').read()

def c_escape(b):
    # emit C literal, 3-digit octal cho moi byte ngoai ASCII in duoc
    out = []
    for ch in b:
        if ch == 0x22: out.append('\\"')
        elif ch == 0x5C: out.append('\\\\')
        elif 0x20 <= ch <= 0x7E: out.append(chr(ch))
        else: out.append('\\%03o' % ch)
    return '"' + ''.join(out) + '"'

def find_lit_before(data, anchor, what):
    # tim chuoi "text/anchor" trong Lua -> tra text (bytes). Lay TAT CA cac bien the.
    pat = re.compile(rb'"([^"\r\n]*?)/' + re.escape(anchor) + rb'"')
    hits = [m.group(1) for m in pat.finditer(data)]
    assert hits, 'KHONG TIM THAY anchor %s (%s)' % (anchor, what)
    return hits

def first_string_in_func(data, funcname, what):
    m = re.search(rb'function\s+' + re.escape(funcname) + rb'\s*\(', data)
    assert m, 'KHONG THAY function %s (%s)' % (funcname, what)
    body = data[m.end():m.end()+2000]
    for m2 in re.finditer(rb'"([^"\r\n]*)"', body):  # moi chuoi "..." khong chong lan
        s = m2.group(1)
        if len(s) >= 10 and any(b >= 0x80 for b in s):  # phai co byte TCVN3 that
            return s
    raise AssertionError('KHONG THAY chuoi TCVN3 trong %s (%s)' % (funcname, what))

def assert_in(data, lit, what):
    assert lit in data, 'MARKER KHONG CO TRONG FILE: %s : %r' % (what, lit)
    return lit

def parse_tab(path):
    raw = rd(path)
    lines = raw.replace(b'\r\n', b'\n').replace(b'\r', b'\n').split(b'\n')
    hdr = lines[0].split(b'\t')
    col = {}
    for i, h in enumerate(hdr):
        col[h.strip().decode('latin-1')] = i
    rows = []
    for j, ln in enumerate(lines[1:], start=2):  # j = physical row (2-based nhu Lua)
        if not ln.strip(): continue
        rows.append((j, ln.split(b'\t')))
    return col, rows

def cell(col, cells, name):
    i = col[name]
    return cells[i].strip() if i < len(cells) else b''

def as_int(b, wild=None):
    s = b.strip()
    if s in (b'n', b'N', b''):
        assert wild is not None, 'gia tri trong/n ngoai du kien: %r' % b
        return wild
    return int(s)

sea, head, goods, xaphu, mapidx, namedef = rd(SEA), rd(HEAD), rd(GOODS), rd(XAPHU), rd(MAPIDX), rd(NAMEDEF)

# ============ MARKERS ============
MK = {}
MK['OPT_CONFIRM']     = find_lit_before(sea, b'Task_Confirm', 'course0 nhan nv')[0]
MK['OPT_TASKPROCESS'] = find_lit_before(sea, b'Task_TaskProcess', 'course3 nv tiep')[0]
MK['OPT_ACCEPT']      = find_lit_before(sea, b'Task_Accept', 'course1 tra nv')[0]
MK['OPT_CANCELCONF']  = find_lit_before(sea, b'Task_CancelConfirm', 'course1 huy')[0]
cancel1 = find_lit_before(sea, b'#Task_Cancel(1)', 'xac nhan huy (2 bien the)')
MK['OPT_CANCEL1A']    = cancel1[0]
MK['OPT_CANCEL1B']    = cancel1[-1]
MK['OPT_CANCEL2']     = find_lit_before(sea, b'#Task_Cancel(2)', 'huy 100 manh')[0]
MK['OPT_NORMALCANCEL']= find_lit_before(sea, b'Task_NormalCancel', 'huy thuong (reset chuoi)')[0]
MK['OPT_GODATAU']     = find_lit_before(xaphu, b'godatau', 'xa phu di lam nv')[0]
MK['MSG_PUNISH']      = first_string_in_func(sea, b'Task_Punish', 'phat 10 phut')[:40]
MK['MSG_LIMIT']       = assert_in(sea, b'M\xe7i ng\xb5y l\xb5m 40 l\xc7n', 'du 40/ngay')
MK['MSG_FAILREQ']     = assert_in(sea, b'ch\xada \xae\xb9t \xae\xf3ng y\xaau c\xc7u', 'tra fail')
MK['MSG_FAILSHXT']    = assert_in(sea, b'thu th\xcbp \xae\xf1 m\xb6nh s\xacn H\xb5', 'thieu manh SHXT')
MK['MSG_BAGFULL']     = assert_in(sea, b'H\xb5nh trang \xae\xb7 \xae\xc7y', 'tui day')
MK['MSG_CLAIMED']     = assert_in(sea, b'l\xb7nh th\xad\xebng r\xe5i', 'da lanh thuong')
MK['T1_MUA']          = assert_in(head, b'Mua gi\xf3p ta', 'template loai 1')
MK['T23_TIM']         = assert_in(head, b'H\xb7y gi\xf3p ta \xaei t\xd7m m\xe3n n\xb5y', 'template loai 2/3')
MK['T2_1CAI']         = assert_in(head, b'1 c\xb8i', 'loai 2 khong magic')
MK['T3_XEMXONG']      = assert_in(head, b'Xem xong ta s\xcf tr\xb6 l\xb9i', 'duoi loai 3')
MK['T4_TIMGIUP']      = assert_in(head, b't\xd7m gi\xf3p ta', 'template loai 4')
MK['T4_QUYEN']        = assert_in(head, b'quy\xd3n', 'chu quyen loai 4')
MK['T4_DIADO']        = assert_in(head, b'\xa7\xdea \xa7\xe5 ch\xdd', 'Dia Do chi')
MK['T4_MATCHI']       = assert_in(head, b'M\xcbt ch\xdd', 'Mat chi')
MK['T5_NANGCAP']      = assert_in(head, b'H\xb7y \xaei n\xa9ng c\xcap', 'template loai 5')
MK['T5_EXP']          = assert_in(head, b'Tr\xde kinh nghi\xd6m', 'ten exp')
MK['T5_DANHVONG']     = assert_in(head, b'Danh v\xe4ng', 'ten danh vong')
MK['T5_PHUCDUYEN']    = assert_in(head, b'Ph\xf3c Duy\xaan', 'ten phuc duyen')
MK['T5_PK']           = assert_in(head, b'\xaei\xd3m PK', 'ten PK')
MK['T5_TONGKIM']      = assert_in(head, b'T\xe8ng Kim', 'ten Tong Kim')
MK['T6_MANH']         = assert_in(head, b'm\xb6nh b\xb6n \xae\xe5 S\xacn H\xb5 X\xb7 T\xbec', 'template loai 6')
MK['MIN_MARK']        = assert_in(head, b'nh\xe1 nh\xcat:', 'nho nhat')
MK['MAX_MARK']        = assert_in(head, b'l\xedn nh\xcat:', 'lon nhat')
MK['MSG_TAM_PRE']     = assert_in(goods, b'B\xb9n nh\xcbn \xae\xad\xeec m\xe9t t\xcam', 'msg nhat cuon')
MK['MSG_TONGCONG']    = assert_in(goods, b't\xe6ng c\xe9ng', 'tong cong')
MK['MSG_TAM_SUF']     = assert_in(goods, b' t\xcam.', 'duoi tam.')
MK['SENDER_HETHONG']  = assert_in(namedef, b'H\xd6 Th\xe8ng', 'nguoi gui He Thong')

# ============ BANG BUYGOODS ============
col, rows = parse_tab(TASKDIR + r'\tasklink_buygoods.txt')
buy = []
for j, c in rows:
    buy.append((j, as_int(cell(col,c,'Genre')), as_int(cell(col,c,'Detail')),
                as_int(cell(col,c,'Particular')), as_int(cell(col,c,'Level')),
                as_int(cell(col,c,'GoodsFive')), cell(col,c,'TaskInfo'), cell(col,c,'TaskInfo1')))

# ============ BANG FINDGOODS ============
col, rows = parse_tab(TASKDIR + r'\tasklink_findgoods.txt')
find_ = []
for j, c in rows:
    mg = cell(col,c,'MagicEnName')
    magic = 0 if mg in (b'n', b'N') else int(mg)
    find_.append((j, as_int(cell(col,c,'Genre')), as_int(cell(col,c,'Detail')),
                  as_int(cell(col,c,'Particular')), as_int(cell(col,c,'Level'), wild=-1),
                  as_int(cell(col,c,'GoodsFive'), wild=-1), magic,
                  as_int(cell(col,c,'MinValue'), wild=0), as_int(cell(col,c,'MaxValue'), wild=0),
                  cell(col,c,'TaskInfo'), cell(col,c,'MagicCnName')))

# ============ BANG SHOWGOODS ============
col, rows = parse_tab(TASKDIR + r'\tasklink_showgoods.txt')
show = []
for j, c in rows:
    show.append((j, int(cell(col,c,'MagicEnName')), as_int(cell(col,c,'MinValue')),
                 as_int(cell(col,c,'MaxValue')), cell(col,c,'MagicCnName')))

# ============ MAP LOAI 4 (TL_MAPTRAPINDEX) ============
QUEST_MAPS = [1,11,37,53,78,80,162,176,21,122,75,225,226,227]
maps = []
for m in re.finditer(rb'\{\s*(\d+)\s*,\s*"([^"]*)"\s*,\s*(\d+)\s*,\s*(\d+)\s*\}', mapidx):
    mid = int(m.group(1))
    if mid in QUEST_MAPS and mid not in [x[0] for x in maps]:
        maps.append((mid, m.group(2), int(m.group(3)), int(m.group(4))))
assert len(maps) == len(QUEST_MAPS), 'thieu map loai 4: co %d/%d' % (len(maps), len(QUEST_MAPS))

# ============ NPC DA TAU (10 thanh, don vi O - AUTO_DATAU_SPEC.md muc 1) ============
DTNPC = [(1,1620,3089),(11,3154,5067),(20,3537,6231),(37,1736,3101),(53,1626,3172),
         (78,1595,3288),(80,1745,2967),(121,1960,4501),(162,1651,3226),(176,1562,2979)]

# ============ EMIT ============
o = io.StringIO()
o.write('// KDaTauTables.h - SINH TU DONG boi ReverseTools/gen_datau_tables.py - DUNG SUA TAY\n')
o.write('// Nguon: serverscript_jx2/datau_tasklink (bang + marker TCVN3 trich truc tiep tu Lua)\n')
o.write('#ifndef KDATAUTABLES_H\n#define KDATAUTABLES_H\n\n')

for k in sorted(MK):
    o.write('static const char DTM_%s[] = %s;\n' % (k, c_escape(MK[k])))

o.write('\nstruct DTBuyRow { int nRow; int nGenre, nDetail, nParticular, nLevel, nFive; const char* szShop; const char* szItem; };\n')
o.write('static const DTBuyRow g_DTBuy[] = {\n')
for j,g,d,p,l,f,shop,item in buy:
    o.write('\t{ %d, %d, %d, %d, %d, %d, %s, %s },\n' % (j,g,d,p,l,f,c_escape(shop),c_escape(item)))
o.write('};\nstatic const int g_nDTBuyCount = sizeof(g_DTBuy)/sizeof(g_DTBuy[0]);\n')

o.write('\n// nLevel/nFive == -1 nghia la bo qua (o "n"); nMagic == 0 nghia la khop 5 truong\n')
o.write('struct DTFindRow { int nRow; int nGenre, nDetail, nParticular, nLevel, nFive; int nMagic, nMin, nMax; const char* szItem; const char* szMagic; };\n')
o.write('static const DTFindRow g_DTFind[] = {\n')
for j,g,d,p,l,f,mg,mn,mx,item,mname in find_:
    o.write('\t{ %d, %d, %d, %d, %d, %d, %d, %d, %d, %s, %s },\n' % (j,g,d,p,l,f,mg,mn,mx,c_escape(item),c_escape(mname)))
o.write('};\nstatic const int g_nDTFindCount = sizeof(g_DTFind)/sizeof(g_DTFind[0]);\n')

o.write('\nstruct DTShowRow { int nRow; int nMagic, nMin, nMax; const char* szMagic; };\n')
o.write('static const DTShowRow g_DTShow[] = {\n')
for j,mg,mn,mx,mname in show:
    o.write('\t{ %d, %d, %d, %d, %s },\n' % (j,mg,mn,mx,c_escape(mname)))
o.write('};\nstatic const int g_nDTShowCount = sizeof(g_DTShow)/sizeof(g_DTShow[0]);\n')

o.write('\n// toa do don vi O (cell) - nhan 32 ra mps truoc khi dung\n')
o.write('struct DTMapRow { int nMapId, nX, nY; const char* szName; };\n')
o.write('static const DTMapRow g_DTQuestMap[] = {\n')
for mid,name,x,y in maps:
    o.write('\t{ %d, %d, %d, %s },\n' % (mid,x,y,c_escape(name)))
o.write('};\nstatic const int g_nDTQuestMapCount = sizeof(g_DTQuestMap)/sizeof(g_DTQuestMap[0]);\n')

o.write('\nstruct DTNpcRow { int nMapId, nX, nY; };\n')
o.write('static const DTNpcRow g_DTNpc[] = {\n')
for mid,x,y in DTNPC:
    o.write('\t{ %d, %d, %d },\n' % (mid,x,y))
o.write('};\nstatic const int g_nDTNpcCount = sizeof(g_DTNpc)/sizeof(g_DTNpc[0]);\n')
o.write('\n#endif // KDATAUTABLES_H\n')

data = o.getvalue().replace('\n', '\r\n').encode('ascii')
open(OUT, 'wb').write(data)
print('OK: ghi %s (%d byte, buy=%d find=%d show=%d map=%d marker=%d)' % (
    OUT, len(data), len(buy), len(find_), len(show), len(maps), len(MK)))
