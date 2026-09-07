import re, io, json, struct, sys, collections
import pymysql

S = 'E:/SourceTuanLe/SourceVs22/TESTLOFFF_ONLINE/bin/server'
skm = {int(k): v for k, v in json.load(open(sys.argv[1])).items()}

# --- SKILLNORMAL tu factionhead.lua (so khop ngoac de lay tung phai)
txt = io.open(S + '/script/header/factionhead.lua', 'r', encoding='latin-1').read()
a = re.search(r'^SKILLNORMAL\s*=\s*\{', txt, re.M).start()
e = re.search(r'^SKILL90_ARRAY\s*=', txt, re.M).start()
blk = txt[a:e]
blk = re.sub(r'--[^\n]*', '', blk)
norm = {}
for m in re.finditer(r'\[(\d+)\]\s*=\s*\{', blk):
    k = int(m.group(1)); i = m.end(); depth = 1; j = i
    while j < len(blk) and depth:
        if blk[j] == '{':
            depth += 1
        elif blk[j] == '}':
            depth -= 1
        j += 1
    ids = [int(x) for x in re.findall(r'\{\s*(\d+)\s*,\s*\d+\s*\}', blk[i:j])]
    norm[k] = ids
ten = {1: 'Thieu Lam', 2: 'Thien Vuong', 3: 'Duong Mon', 4: 'Ngu Doc', 5: 'Nga Mi', 6: 'Thuy Yen',
       7: 'Cai Bang', 8: 'Thien Nhan', 9: 'Vo Dang', 10: 'Con Lon', 11: 'Hoa Son', 12: 'Vu Hon', 13: 'Tieu Dao'}
print("SKILLNORMAL:", {ten.get(k, k): len(v) for k, v in norm.items()})

# --- ban luu moi nhat cua tung bot
c = pymysql.connect(host='127.0.0.1', port=3306, user='root', password='123456', database='jx1_role')
cur = c.cursor()
cur.execute("SELECT role_name, MAX(id) FROM role_history WHERE saved_at >= '2026-09-07 07:28:00' AND role_name <> 'CaiBang' GROUP BY role_name")
pairs = cur.fetchall()
print("bot co ban luu sau 07:28:", len(pairs))
thieu = collections.Counter(); cobot = collections.Counter(); duCap = collections.Counter()
passive = collections.Counter(); notinlist = collections.Counter(); lvdist = collections.Counter()
for name, hid in pairs:
    cur.execute("SELECT role_blob FROM role_history WHERE id=%s", (hid,))
    b = bytes(cur.fetchone()[0])
    ofs = struct.unpack_from('<i', b, 725)[0]; oss = struct.unpack_from('<i', b, 729)[0]
    n = max(0, (oss - ofs) // 8)
    sk = {}
    for k in range(n):
        sid, lv, val = struct.unpack_from('<hhi', b, ofs + k * 8)
        sk[sid] = lv
    if not sk or not norm:
        continue
    fac = max(norm, key=lambda f: len(set(norm[f]) & set(sk)))
    cobot[fac] += 1
    lv60 = any(lv >= 20 and skm.get(s, {}).get('req', 0) >= 60 for s, lv in sk.items())
    if not lv60:
        continue
    duCap[fac] += 1
    for s in norm[fac]:
        d = skm.get(s)
        if not d or d['req'] > 60 or d['max'] <= 0:
            continue
        lv = sk.get(s)
        if lv is None:
            notinlist[(fac, s)] += 1
            continue
        if lv < 20:
            (passive if d['style'] == '3' else thieu)[(fac, s)] += 1
            lvdist[(fac, s, lv)] += 1
print("\n=== bot theo phai (tat ca / du cap 6x):", {ten[f]: "%d/%d" % (cobot[f], duCap[f]) for f in sorted(cobot)})
print("\n=== CHIEU CHU DONG 1x-6x CHUA FULL 20 (bot du cap 6x):")
for (f, s), n in sorted(thieu.items()):
    d = skm[s]
    print("  %-11s id=%-4d %-24s req=%-2d series=%-2d : %d/%d bot" % (ten[f], s, d['name'], d['req'], d['series'], n, duCap[f]))
print("\n=== CHIEU BI DONG (style 3) chua full:")
for (f, s), n in sorted(passive.items()):
    d = skm[s]
    print("  %-11s id=%-4d %-24s req=%-2d series=%-2d max=%d : %d/%d bot" % (ten[f], s, d['name'], d['req'], d['series'], d['max'], n, duCap[f]))
print("\n=== chieu trong SKILLNORMAL nhung KHONG CO trong danh sach bot:")
for (f, s), n in sorted(notinlist.items()):
    d = skm.get(s, {'name': '?', 'req': -1})
    print("  %-11s id=%-4d %-24s req=%-2d : %d/%d bot" % (ten[f], s, d['name'], d['req'], n, duCap[f]))
print("\n=== phan bo cap cua chieu chua full (phai, id, cap) -> so bot:", sorted(lvdist.items())[:40])
