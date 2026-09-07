# -*- coding: utf-8 -*-
# so_snapshot.py - so hai anh chup role lien tiep de biet MON NAO bien mat.
# Bo cuc TDBItemData 233 byte theo ToolsMySQL/sinh_role_item.py (da kiem chung).
# Offset header do tren blob that: nItemCount @713, dwItemOffset @733, dwDataLen @741.
import struct, sys, collections
import pymysql

ITEM = 233
OFF_NITEM, OFF_ITEMOFF, OFF_DATALEN = 713, 733, 741
VI_TRI = {1: 'tren tay', 2: 'dang mac', 3: 'hanh trang', 4: 'ruong',
          5: 'ruong mr1', 6: 'ruong mr2', 7: 'ruong mr3', 8: 'hanh trang mr',
          9: 'giao dich', 12: 'o phim tat', 24: 'trang bi du phong'}
GENRE = {0: 'trang bi', 1: 'thuoc', 2: 'khoang', 3: '?', 4: 'nhiem vu',
         5: 'tho dia phu', 6: 'phu/bua', 7: '?', 8: 'van cuong', 9: 'khac'}


def mon(m):
    (iid, cls, ilocal, ix, iy, code, dt, pt, lv, se, ver, seed) = struct.unpack_from('<12i', m, 0)
    (lucky, ident, dura, goldid, stack, ench, point,
     nam, thang, ngay, gio, lockbh, giomo) = struct.unpack_from('<13i', m, 112)
    baitan, locksell, locktrade, lockdrop = struct.unpack_from('<4i', m, 200)
    return dict(iid=iid, cls=cls, place=ilocal, x=ix, y=iy, code=code, dt=dt, pt=pt,
                lv=lv, se=se, ver=ver, seed=seed, lucky=lucky, dura=dura, goldid=goldid,
                stack=stack, ench=ench, locksell=locksell, lockdrop=lockdrop, baitan=baitan)


def doc(blob):
    b = bytes(blob)
    n = struct.unpack_from('<h', b, OFF_NITEM)[0]
    oi = struct.unpack_from('<i', b, OFF_ITEMOFF)[0]
    return [mon(b[oi + k * ITEM: oi + (k + 1) * ITEM]) for k in range(n)]


def khoa(d):
    # nhan dang mot mon: khong dung vi tri/so chong (co the doi), dung seed + ma
    return (d['cls'], d['code'], d['dt'], d['pt'], d['lv'], d['se'], d['seed'], d['goldid'])


def ten(d):
    return ("%-9s g=%d code=%-4d dt=%-3d pt=%-5d cap=%-3d he=%-2d chong=%-3d gold=%-4d o=%s(%d,%d)%s"
            % (GENRE.get(d['cls'], str(d['cls'])), d['cls'], d['code'], d['dt'], d['pt'], d['lv'],
               d['se'], d['stack'], d['goldid'], VI_TRI.get(d['place'], str(d['place'])),
               d['x'], d['y'], ' [BAY BAN]' if d['baitan'] else ''))


def main():
    role = sys.argv[1] if len(sys.argv) > 1 else 'CaiBang'
    lim = int(sys.argv[2]) if len(sys.argv) > 2 else 8
    c = pymysql.connect(host='127.0.0.1', port=3306, user='root', password='123456', database='jx1_role')
    cur = c.cursor()
    cur.execute("""SELECT id, saved_at, n_item, reason, role_blob FROM role_history
                   WHERE role_name=%s ORDER BY id DESC LIMIT %s""", (role, lim))
    rows = list(cur.fetchall())[::-1]
    snaps = [(r[0], r[1], r[2], r[3].decode('latin-1'), doc(r[4])) for r in rows]
    for i in range(1, len(snaps)):
        h0, t0, n0, r0, i0 = snaps[i - 1]
        h1, t1, n1, r1, i1 = snaps[i]
        k0 = collections.Counter(khoa(d) for d in i0)
        k1 = collections.Counter(khoa(d) for d in i1)
        mat = k0 - k1
        them = k1 - k0
        print("\n=== %s (id %d, %d mon) -> %s (id %d, %d mon) : mat %d, them %d"
              % (t0, h0, n0, t1, h1, n1, sum(mat.values()), sum(them.values())))
        for k, cnt in mat.items():
            d = next(x for x in i0 if khoa(x) == k)
            print("   - MAT  x%d %s" % (cnt, ten(d)))
        for k, cnt in list(them.items())[:6]:
            d = next(x for x in i1 if khoa(x) == k)
            print("   + THEM x%d %s" % (cnt, ten(d)))


if __name__ == '__main__':
    main()
