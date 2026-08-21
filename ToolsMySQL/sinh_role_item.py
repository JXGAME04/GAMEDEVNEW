# -*- coding: utf-8 -*-
"""sinh_role_item.py -- Bung vat pham tu role_blob ra bang tra cuu `role_item`.

VI SAO CO TEP NAY: vat pham cua nhan vat nam TRONG blob nhi phan `role.role_blob`
(dung nhu thoi Berkeley DB), nen SQL khong nhin thay ben trong. Khong the hoi
"ai dang giu mon X", "co bao nhieu mon dang bay ban", "mon nao sap het do ben"...
Bang `role_item` la BAN SAO DAN XUAT chi de doc/thong ke.

AN TOAN: game KHONG doc bang nay. Xoa roi sinh lai luc nao cung duoc. Nguoc lai,
sua du lieu trong bang nay KHONG lam doi gi trong game.

Bo cuc TDBItemData (Lib/S3DBInterface.h:177-226, #pragma pack(push,1)), da kiem
cong lai dung 233 byte:
    0 iid            48  iparam[16] (64B)  164 iiduphong1..9 (36B)
    4 iequipclasscode 112 ilucky           200 iBaiTanPrice
    8 ilocal          116 iidentify        204 ilocksell
   12 ix              120 idurability      208 ilocktrade
   16 iy              124 igoldid          212 ilockdrop
   20 iequipcode      128 istacknum        216 imantle (1B)
   24 idetailtype     132 ienchance        217 ifortune
   28 iparticulartype 136 ipoint           221 iowner
   32 ilevel          140 iyear            225 irow
   36 iseries         144 imonth           229 iequipnaturecode
   40 iequipversion   148 iday             233 = het
   44 irandseed       152 ihour
                      156 ilockbh
                      160 igiomokhoa

LUU Y: ilocal KHONG phai INVENTORY_ROOM ma la nPlace (ITEM_POSITION,
Core/Src/GameDataDef.h:316) -- xem KPlayerDBFuns.cpp:1002.

Chay:  python sinh_role_item.py [--chi-thu]
"""
import argparse
import datetime
import struct
import sys

sys.path.insert(0, 'D:/GAMEDEVNEW/ToolsMySQL')
import jx_role      # noqa: E402
import tcvn3        # noqa: E402

try:
    import pymysql
except ImportError:
    pymysql = None

ITEM = jx_role.ITEM_SIZE          # 233

VI_TRI = {
    1: 'tren tay', 2: 'mang tren nguoi', 3: 'hanh trang', 4: 'ruong',
    5: 'ruong mo rong 1', 6: 'ruong mo rong 2', 7: 'ruong mo rong 3',
    8: 'hanh trang mo rong', 9: 'giao dich', 10: 'OTT', 11: 'giao dich 1',
    12: 'o phim tat', 13: 'o giao', 14: 'o giao nhiem vu',
    15: 'kham nam/nang cap xanh', 16: 'luyen huyen tinh',
    17: 'nang cap huyen tinh', 18: 'nang cap khoang thach thuoc tinh',
    19: 'rut option', 20: 'che tao trang bi tim', 21: 'kham nam trang bi',
    22: 'skill phim tat (nhac len)', 23: 'skill phim tat (dat xuong)',
    24: 'trang bi du phong',
}


def doc_mon(m):
    """Giai ma 233 byte thanh tuple theo dung thu tu cot cua bang role_item."""
    (iid, cls, ilocal, ix, iy, code, dt, pt, lv, se, ver, seed) = \
        struct.unpack_from('<12i', m, 0)
    param = m[48:112]
    (lucky, ident, dura, goldid, stack, ench, point,
     nam, thang, ngay, gio, lockbh, giomo) = struct.unpack_from('<13i', m, 112)
    baitan, locksell, locktrade, lockdrop = struct.unpack_from('<4i', m, 200)
    mantle = m[216]
    fortune, owner, row, nature = struct.unpack_from('<i I i i', m, 217)
    return (ilocal, VI_TRI.get(ilocal), ix, iy,
            iid, cls, code, dt, pt, lv, se, ver, seed, param,
            lucky, ident, dura, goldid, stack, ench, point,
            nam, thang, ngay, gio,
            lockbh, giomo, baitan, locksell, locktrade, lockdrop,
            mantle, fortune, owner, row, nature)


COT = ("role_name,role_name_vn,acc_name,stt,"
       "vi_tri,vi_tri_ten,o_x,o_y,"
       "iid,equip_class_code,equip_code,detail_type,particular_type,cap,he,"
       "equip_version,rand_seed,param,"
       "may_man,giam_dinh,do_ben,gold_id,so_chong,cuong_hoa,diem,"
       "nam,thang,ngay,gio,"
       "khoa_bh,gio_mo_khoa,gia_bay_ban,khoa_ban,khoa_trao,khoa_vut,"
       "hinh_thoi_trang,van_may,chu_so_huu,hang,equip_nature_code,sinh_luc")
N_COT = len(COT.split(','))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--host', default='127.0.0.1')
    ap.add_argument('--port', type=int, default=3306)
    ap.add_argument('--user', default='root')
    ap.add_argument('--matkhau', default='123456')
    ap.add_argument('--chi-thu', action='store_true', dest='chi_thu')
    a = ap.parse_args()

    if pymysql is None:
        print('!! thieu pymysql')
        sys.exit(5)

    cn = pymysql.connect(host=a.host, port=a.port, user=a.user,
                         password=a.matkhau, database='jx1_role',
                         charset='latin1', autocommit=False, binary_prefix=True)
    c = cn.cursor()
    c.execute("SELECT role_name, acc_name, role_blob FROM role")
    hang = c.fetchall()
    print('doc %d nhan vat tu bang role' % len(hang))

    bay_gio = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    goi = []
    n_mon = n_loi = 0
    ten_loi = []
    for ten, acc, blob in hang:
        try:
            d = jx_role.parse(blob, strict=False)
            o = d['off_item']
            het = len(blob) - 4 if d.get('has_crc') else len(blob)
            vung = blob[o:het]
            if len(vung) % ITEM:
                raise ValueError('vung vat pham %d byte khong chia het %d'
                                 % (len(vung), ITEM))
        except Exception as e:
            n_loi += 1
            if len(ten_loi) < 10:
                ten_loi.append((ten, str(e)))
            continue
        ten_vn = tcvn3.sang_unicode(ten)
        for i in range(len(vung) // ITEM):
            goi.append((ten, ten_vn, acc, i) + doc_mon(vung[i * ITEM:(i + 1) * ITEM])
                       + (bay_gio,))
            n_mon += 1

    print('bung ra %d vat pham (nhan vat khong doc duoc: %d)' % (n_mon, n_loi))
    for t, e in ten_loi:
        print('   !! %s: %s' % (t, e))

    if goi and len(goi[0]) != N_COT:
        print('!! so cot lech: dong co %d gia tri, bang can %d -- HUY'
              % (len(goi[0]), N_COT))
        sys.exit(3)

    if a.chi_thu:
        print('\n[--chi-thu] Khong ghi gi vao MySQL.')
        return

    sql = ("INSERT INTO role_item (%s) VALUES (%s)"
           % (COT, ','.join(['%s'] * N_COT)))
    try:
        c.execute("SET SESSION sql_mode='STRICT_ALL_TABLES'")
        c.execute("TRUNCATE TABLE role_item")
        for i in range(0, len(goi), 2000):
            c.executemany(sql, goi[i:i + 2000])
        cn.commit()
    except Exception as e:
        cn.rollback()
        print('!! LOI -> DA HUY (rollback): %s' % e)
        raise
    print('da ghi %d dong vao role_item' % n_mon)

    # ------------------------------------------------ kiem chung nguoc
    print()
    print('-' * 70)
    print('KIEM CHUNG NGUOC')
    print('-' * 70)
    c.execute("SELECT COUNT(*) FROM role_item")
    n_db = c.fetchone()[0]
    c.execute("SELECT SUM(n_item) FROM role")
    n_khai = c.fetchone()[0]
    print('  dong trong role_item      : %d' % n_db)
    print('  SUM(n_item) cua bang role : %s' % n_khai)
    print('  %s' % ('KHOP' if n_db == n_khai == n_mon else '!! LECH'))

    c.execute("SELECT COUNT(DISTINCT role_name) FROM role_item")
    print('  so nhan vat co vat pham   : %d' % c.fetchone()[0])
    c.execute("SELECT vi_tri, vi_tri_ten, COUNT(*) FROM role_item "
              "GROUP BY vi_tri, vi_tri_ten ORDER BY 3 DESC")
    print('  phan bo theo vi tri:')
    for r in c.fetchall():
        print('     %-3d %-24s %7d' % (r[0], r[1] or '?', r[2]))
    c.execute("SELECT COUNT(*) FROM role_item WHERE gia_bay_ban > 0")
    print('  mon dang BAY BAN tren sap : %d' % c.fetchone()[0])
    c.execute("SELECT COUNT(*) FROM role_item WHERE khoa_bh <> 0")
    print('  mon dang KHOA bao hiem    : %d' % c.fetchone()[0])
    cn.close()
    sys.exit(0 if n_db == n_mon else 6)


if __name__ == '__main__':
    main()
