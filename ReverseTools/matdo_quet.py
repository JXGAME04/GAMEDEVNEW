# -*- coding: utf-8 -*-
# quet_mat.py - quet TOAN BO anh chup role_history cua mot nhan vat, tim mon BIEN MAT
# ma KHONG phai thuoc/binh (genre 1) - tuc do that su mat.
# Chay: python quet_mat.py [ten_nhan_vat] [so_ngay]
import struct, sys, collections
import pymysql

ITEM = 233
OFF_NITEM, OFF_ITEMOFF = 713, 733
VI_TRI = {1: 'tren tay', 2: 'dang mac', 3: 'hanh trang', 4: 'ruong',
          5: 'ruong mr1', 6: 'ruong mr2', 7: 'ruong mr3', 8: 'hanh trang mr',
          9: 'giao dich', 12: 'o phim tat', 24: 'trang bi du phong'}
GENRE = {0: 'TRANG BI', 1: 'thuoc', 2: 'khoang', 3: 'g3', 4: 'nhiem vu',
         5: 'tho dia phu', 6: 'phu/bua', 7: 'g7', 8: 'van cuong', 9: 'g9'}


def mon(m):
    (iid, cls, ilocal, ix, iy, code, dt, pt, lv, se, ver, seed) = struct.unpack_from('<12i', m, 0)
    (lucky, ident, dura, goldid, stack, ench, point,
     nam, thang, ngay, gio, lockbh, giomo) = struct.unpack_from('<13i', m, 112)
    baitan, locksell, locktrade, lockdrop = struct.unpack_from('<4i', m, 200)
    return dict(cls=cls, place=ilocal, x=ix, y=iy, code=code, dt=dt, pt=pt, lv=lv, se=se,
                seed=seed, goldid=goldid, stack=stack, ench=ench, dura=dura, lucky=lucky,
                baitan=baitan, locksell=locksell)


def doc(blob):
    b = bytes(blob)
    n = struct.unpack_from('<h', b, OFF_NITEM)[0]
    oi = struct.unpack_from('<i', b, OFF_ITEMOFF)[0]
    out = []
    for k in range(n):
        p = oi + k * ITEM
        if p + ITEM <= len(b):
            out.append(mon(b[p:p + ITEM]))
    return out


def khoa(d):
    return (d['cls'], d['code'], d['dt'], d['pt'], d['lv'], d['se'], d['seed'], d['goldid'])


def ten(d):
    return ("%-9s dt=%-3d pt=%-5d cap=%-3d he=%-2d chong=%-3d gold=%-5d ench=%d o=%s(%d,%d)%s"
            % (GENRE.get(d['cls'], str(d['cls'])), d['dt'], d['pt'], d['lv'], d['se'],
               d['stack'], d['goldid'], d['ench'], VI_TRI.get(d['place'], str(d['place'])),
               d['x'], d['y'], ' [BAY BAN]' if d['baitan'] else ''))


def main():
    role = sys.argv[1] if len(sys.argv) > 1 else 'CaiBang'
    ngay = int(sys.argv[2]) if len(sys.argv) > 2 else 30
    c = pymysql.connect(host='127.0.0.1', port=3306, user='root', password='123456', database='jx1_role')
    cur = c.cursor()
    cur.execute("""SELECT id, saved_at, n_item, reason, role_blob FROM role_history
                   WHERE role_name=%s AND saved_at >= DATE_SUB(NOW(), INTERVAL %s DAY)
                   ORDER BY id""", (role, ngay))
    rows = cur.fetchall()
    print("anh chup: %d" % len(rows))
    truoc = None
    tong = collections.Counter()
    sukien = []
    for hid, at, ni, reason, blob in rows:
        it = doc(blob)
        if truoc is not None:
            k0 = collections.Counter(khoa(d) for d in truoc[4])
            k1 = collections.Counter(khoa(d) for d in it)
            mat = k0 - k1
            mat_quy = {k: v for k, v in mat.items() if k[0] != 1}   # bo genre 1 = thuoc
            if mat_quy:
                ds = []
                for k, v in mat_quy.items():
                    d = next(x for x in truoc[4] if khoa(x) == k)
                    ds.append((v, d))
                    tong[(d['cls'], d['dt'], d['pt'], d['lv'], d['goldid'])] += v
                sukien.append((truoc[1], at, truoc[0], hid, ds))
        truoc = (hid, at, ni, reason, it)
    print("so lan co mon KHAC THUOC bien mat: %d\n" % len(sukien))
    for t0, t1, h0, h1, ds in sukien[-25:]:
        print("[%s -> %s] (id %d->%d) mat %d mon:" % (t0, t1, h0, h1, sum(v for v, _ in ds)))
        for v, d in ds[:8]:
            print("    x%d %s" % (v, ten(d)))
    print("\n=== TONG theo loai mon (genre, detail, particular, cap, goldid) -> so lan mat:")
    for k, v in tong.most_common(20):
        print("   %s x%d" % (str(k), v))


if __name__ == '__main__':
    main()
