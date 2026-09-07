# -*- coding: utf-8 -*-
# theo_vitri.py - dem so mon theo VI TRI (dang mac / hanh trang / ruong ...) qua tung anh chup,
# de phan biet "mat that" voi "ban luu thieu vung roi quay lai".
import struct, sys, collections
import pymysql

ITEM = 233
OFF_NITEM, OFF_ITEMOFF = 713, 733
VI_TRI = {1: 'tay', 2: 'DANG MAC', 3: 'hanh trang', 4: 'RUONG',
          5: 'ruong mr1', 6: 'ruong mr2', 7: 'ruong mr3', 8: 'ht mr',
          9: 'giao dich', 12: 'phim tat', 24: 'du phong'}


def doc(blob):
    b = bytes(blob)
    n = struct.unpack_from('<h', b, OFF_NITEM)[0]
    oi = struct.unpack_from('<i', b, OFF_ITEMOFF)[0]
    out = []
    for k in range(n):
        p = oi + k * ITEM
        if p + ITEM > len(b):
            break
        (iid, cls, ilocal, ix, iy, code, dt, pt, lv, se, ver, seed) = struct.unpack_from('<12i', b, p)
        out.append(dict(cls=cls, place=ilocal, dt=dt, pt=pt, lv=lv, se=se, seed=seed))
    return out


def main():
    role = sys.argv[1] if len(sys.argv) > 1 else 'CaiBang'
    lim = int(sys.argv[2]) if len(sys.argv) > 2 else 40
    c = pymysql.connect(host='127.0.0.1', port=3306, user='root', password='123456', database='jx1_role')
    cur = c.cursor()
    cur.execute("""SELECT id, saved_at, n_item, reason, role_blob FROM role_history
                   WHERE role_name=%s ORDER BY id DESC LIMIT %s""", (role, lim))
    rows = list(cur.fetchall())[::-1]
    print("%-20s %8s %5s %-12s | %s" % ("thoi diem", "id", "tong", "ly do", "theo vi tri"))
    for hid, at, ni, reason, blob in rows:
        it = doc(blob)
        c2 = collections.Counter(VI_TRI.get(d['place'], str(d['place'])) for d in it)
        thuoc = sum(1 for d in it if d['cls'] == 1)
        s = "  ".join("%s=%d" % (k, v) for k, v in sorted(c2.items()))
        print("%-20s %8d %5d %-12s | %s  (thuoc=%d)"
              % (str(at), hid, ni, reason.decode('latin-1'), s, thuoc))


if __name__ == '__main__':
    main()
