# -*- coding: utf-8 -*-
"""nhap_giaodich_shop.py -- Dua NHAT KY GIAO DICH va MUA SHOP len MySQL dang tra cuu duoc.

VI SAO: bang `jx1_game.game_log` hien gio chi la "moi dong nhat ky mot hang" --
role_name NULL, gio nam trong text, ban ghi nhieu dong bi tach roi. Khong hoi duoc
"ai ban mon gi cho ai", "thang nay ban duoc bao nhieu Xu".
Tep nay doc THANG tu nhat ky goc va do vao hai bang co cot ro rang.

NGUON:
  bin/server/dulieu/player_log/log_trade/*.txt  -> giao_dich + giao_dich_item
  bin/server/dulieu/player_log/log_ktc/*.txt    -> mua_shop

DINH DANG (da doc tep that de xac nhan, KHONG doan):

  log_trade -- moi ban ghi bat dau bang mot dong "*****Start Log Trade*****":
    HH:MM:SS <TAB> Account chu: X <TAB> PlayerName chu: Y <TAB> IP chu: A : P
             <TAB> Account khach: X <TAB> PlayerName Khach: Y <TAB> IP khach: A : P
             <TAB> Vi tri giao dich (x/y) TenBanDo
             <TAB> Tien giao dich [chu nhan: N khach nhan: M]
             <TAB> [Item khach nhan:] <TAB> [Ten] <TAB> [nGoldId: G] <TAB> ...
    Ten tep: DD_MM_YYYY.txt

  log_ktc -- moi ban ghi bat dau bang mot dong "*******":
    HH:MM:SS <TAB> Tai khoan: X <TAB> Ten: Y <TAB> IP: A : P
             <TAB> [nKindShop: N] <TAB> [TenMon] <TAB> [Number: N] <TAB> [nGoldId: G]
             <TAB> [nGen:..] [nDetail:..] [nParti:..] [nLevel:..] [nSeries:..]
             <TAB> [nLine:..] [Xu:..] [Tong tien van:..]
    Ten tep: YYYY_MM_DD.txt   (KHAC dinh dang cua log_trade -- de y!)

NHAN TRUONG bang tieng Viet (TCVN3) nen KHONG doi chieu bang chuoi tieng Viet
go tay (bay da mac nhieu lan). Chi dua vao: vi tri truong sau khi tach TAB,
va cac nhan ASCII trong ngoac vuong.

CHI DOC nhat ky goc. Chi GHI vao 3 bang dan xuat, game khong dung chung.
Chay: python nhap_giaodich_shop.py [--chi-thu] [--goc <duong dan player_log>]
"""
import argparse
import datetime
import glob
import io
import os
import re
import sys

sys.path.insert(0, 'D:/GAMEDEVNEW/ToolsMySQL')
import tcvn3      # noqa: E402

try:
    import pymysql
except ImportError:
    pymysql = None

GOC_MD = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\dulieu\player_log'

TAB = b'\t'
RE_SO = re.compile(rb'-?\d+')
RE_VITRI = re.compile(rb'\((\d+)\s*/\s*(\d+)\)\s*(.*)$')
RE_NGOAC = re.compile(rb'\[([^\]]*)\]')


def sau_dau_hai_cham(x):
    """'Nhan: gia tri' -> b'gia tri'. Khong co ':' thi tra nguyen."""
    i = x.find(b':')
    return x[i + 1:].strip() if i >= 0 else x.strip()


def so_dau(x, mac_dinh=None):
    m = RE_SO.search(x)
    return int(m.group(0)) if m else mac_dinh


def vn(b):
    return tcvn3.sang_unicode(b) if b else None


def ngay_tu_ten(ten, kieu):
    """kieu='dmy' -> DD_MM_YYYY.txt ; kieu='ymd' -> YYYY_MM_DD.txt"""
    g = os.path.splitext(os.path.basename(ten))[0].split('_')
    if len(g) != 3 or not all(x.isdigit() for x in g):
        return None
    try:
        if kieu == 'dmy':
            return datetime.date(int(g[2]), int(g[1]), int(g[0]))
        return datetime.date(int(g[0]), int(g[1]), int(g[2]))
    except ValueError:
        return None


def gio_tu(x):
    m = re.match(rb'(\d{1,2}):(\d{2}):(\d{2})', x.strip())
    if not m:
        return None
    return tuple(int(m.group(i)) for i in (1, 2, 3))


# ===========================================================================
# log_trade
# ===========================================================================
def doc_giaodich(thumuc):
    ra = []
    for tep in sorted(glob.glob(os.path.join(thumuc, '*.txt'))):
        ngay = ngay_tu_ten(tep, 'dmy')
        dong = io.open(tep, 'rb').read().replace(b'\r\n', b'\n').split(b'\n')
        i = 0
        while i < len(dong):
            if b'Start Log Trade' not in dong[i]:
                i += 1
                continue
            dau_dong = i + 1
            khoi = []
            j = i + 1
            while j < len(dong) and b'Start Log Trade' not in dong[j]:
                if dong[j].strip():
                    khoi.append(dong[j])
                j += 1
            i = j
            if len(khoi) < 2:
                continue

            gd = {'nguon_tep': os.path.basename(tep), 'dong_goc': dau_dong,
                  'ngay': ngay, 'items': []}

            f = khoi[0].split(TAB)
            hms = gio_tu(f[0]) if f else None
            if ngay and hms:
                gd['at'] = datetime.datetime(ngay.year, ngay.month, ngay.day, *hms)
            else:
                gd['at'] = None
            if len(f) > 1:
                gd['acc_chu'] = sau_dau_hai_cham(f[1])
            if len(f) > 2:
                gd['ten_chu'] = sau_dau_hai_cham(f[2])
            if len(f) > 3:
                gd['ip_chu'] = sau_dau_hai_cham(f[3])

            for l in khoi[1:]:
                f = [x for x in l.split(TAB) if x.strip()]
                if not f:
                    continue
                mot = f[0]
                # dong khach: co du 3 truong Account/PlayerName/IP
                if len(f) >= 3 and b'Account' in mot:
                    gd['acc_khach'] = sau_dau_hai_cham(f[0])
                    gd['ten_khach'] = sau_dau_hai_cham(f[1])
                    gd['ip_khach'] = sau_dau_hai_cham(f[2])
                    continue
                m = RE_VITRI.search(mot)
                if m and len(f) == 1:
                    gd['vi_tri_x'] = int(m.group(1))
                    gd['vi_tri_y'] = int(m.group(2))
                    gd['ban_do'] = m.group(3).strip()
                    continue
                if b'[' in mot and len(f) == 1:
                    # dong tien: "... [chu nhan: N khach nhan: M]"
                    so = RE_SO.findall(mot)
                    if len(so) >= 2:
                        gd['tien_chu_nhan'] = int(so[0])
                        gd['tien_khach_nhan'] = int(so[1])
                    continue
                # dong vat pham
                if mot.startswith(b'[') and len(f) >= 2:
                    ben = 'khach'
                    o = {'ben_nhan': ben, 'options': []}
                    o['ten_item'] = f[1].strip().strip(b'[]')
                    for x in f[2:]:
                        n = RE_NGOAC.search(x)
                        if not n:
                            continue
                        v = n.group(1)
                        if b'|' in v:
                            o['options'].append(v.decode('latin-1'))
                        elif b':' in v:
                            nhan = v.split(b':')[0].strip().lower()
                            gt = so_dau(v)
                            if nhan == b'ngoldid':
                                o['gold_id'] = gt
                            elif nhan == b'ngen':
                                o['gen'] = gt
                            elif nhan == b'ndetail':
                                o['detail_type'] = gt
                            elif nhan == b'nparti':
                                o['particular'] = gt
                            elif nhan == b'nlevel':
                                o['cap'] = gt
                            elif nhan == b'nseries':
                                o['he'] = gt
                    gd['items'].append(o)
            ra.append(gd)
    return ra


# ===========================================================================
# log_ktc
# ===========================================================================
def doc_muashop(thumuc):
    ra = []
    for tep in sorted(glob.glob(os.path.join(thumuc, '*.txt'))):
        ngay = ngay_tu_ten(tep, 'ymd')
        dong = io.open(tep, 'rb').read().replace(b'\r\n', b'\n').split(b'\n')
        for k, l in enumerate(dong, 1):
            if not l.strip() or l.strip().startswith(b'*'):
                continue
            f = l.split(TAB)
            hms = gio_tu(f[0]) if f else None
            if not hms:
                continue
            r = {'nguon_tep': os.path.basename(tep), 'dong_goc': k, 'ngay': ngay}
            r['at'] = (datetime.datetime(ngay.year, ngay.month, ngay.day, *hms)
                       if ngay else None)
            if len(f) > 1:
                r['acc_name'] = sau_dau_hai_cham(f[1])
            if len(f) > 2:
                r['role_name'] = sau_dau_hai_cham(f[2])
            if len(f) > 3:
                r['ip'] = sau_dau_hai_cham(f[3])
            # phan trong ngoac vuong
            chua_ten = True
            for x in f[4:]:
                n = RE_NGOAC.search(x)
                if not n:
                    continue
                v = n.group(1)
                if b':' not in v:
                    if chua_ten:
                        r['ten_item'] = v.strip()
                        chua_ten = False
                    continue
                nhan = v.split(b':')[0].strip().lower()
                gt = so_dau(v)
                if nhan == b'nkindshop':
                    r['kind_shop'] = gt
                elif nhan == b'number':
                    r['so_luong'] = gt
                elif nhan == b'ngoldid':
                    r['gold_id'] = gt
                elif nhan == b'ngen':
                    r['gen'] = gt
                elif nhan == b'ndetail':
                    r['detail_type'] = gt
                elif nhan == b'nparti':
                    r['particular'] = gt
                elif nhan == b'nlevel':
                    r['cap'] = gt
                elif nhan == b'nseries':
                    r['he'] = gt
                elif nhan == b'nline':
                    r['line'] = gt
                elif nhan == b'xu':
                    r['xu'] = gt
                else:
                    # nhan tieng Viet duy nhat con lai la "Tong tien van"
                    if r.get('tong_tien_van') is None:
                        r['tong_tien_van'] = gt
            ra.append(r)
    return ra


# ===========================================================================
def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--goc', default=GOC_MD)
    ap.add_argument('--host', default='127.0.0.1')
    ap.add_argument('--user', default='root')
    ap.add_argument('--matkhau', default='123456')
    ap.add_argument('--chi-thu', action='store_true', dest='chi_thu')
    a = ap.parse_args()

    print('=' * 74)
    print('DOC NHAT KY GOC: %s' % a.goc)
    print('=' * 74)
    gd = doc_giaodich(os.path.join(a.goc, 'log_trade'))
    ms = doc_muashop(os.path.join(a.goc, 'log_ktc'))
    n_item = sum(len(x['items']) for x in gd)
    print('  giao dich : %5d ban ghi, %d luot vat pham' % (len(gd), n_item))
    print('  mua shop  : %5d ban ghi' % len(ms))

    thieu_gd = sum(1 for x in gd if not x.get('ten_chu') or not x.get('ten_khach'))
    thieu_ms = sum(1 for x in ms if not x.get('ten_item'))
    print('  giao dich thieu ten chu/khach : %d' % thieu_gd)
    print('  mua shop thieu ten mon        : %d' % thieu_ms)

    print()
    print('  --- 3 giao dich dau ---')
    for x in gd[:3]:
        print('    %s  %s -> %s  tien %s/%s  tai %s (%s/%s)'
              % (x.get('at'), vn(x.get('ten_chu')), vn(x.get('ten_khach')),
                 x.get('tien_chu_nhan'), x.get('tien_khach_nhan'),
                 vn(x.get('ban_do')), x.get('vi_tri_x'), x.get('vi_tri_y')))
        for o in x['items']:
            print('        mon: %s  cap %s he %s' % (vn(o.get('ten_item')),
                                                     o.get('cap'), o.get('he')))
    print('  --- 3 luot mua shop dau ---')
    for x in ms[:3]:
        print('    %s  %s / %s  shop %s  mon %s x%s  Xu %s'
              % (x.get('at'), x.get('acc_name'), vn(x.get('role_name')),
                 x.get('kind_shop'), vn(x.get('ten_item')),
                 x.get('so_luong'), x.get('xu')))

    if a.chi_thu:
        print('\n[--chi-thu] Khong ghi gi vao MySQL.')
        return
    if pymysql is None:
        print('!! thieu pymysql')
        sys.exit(5)

    # charset utf8mb4: cac cot *_vn la VARCHAR Unicode. Cot VARBINARY van nhan
    # byte tho an toan vi binary_prefix=True lam pymysql viet _binary'...'.
    cn = pymysql.connect(host=a.host, port=3306, user=a.user, password=a.matkhau,
                         database='jx1_game', charset='utf8mb4', autocommit=False,
                         binary_prefix=True)
    c = cn.cursor()
    try:
        c.execute("SET FOREIGN_KEY_CHECKS=0")
        c.execute("TRUNCATE TABLE giao_dich_item")
        c.execute("TRUNCATE TABLE giao_dich")
        c.execute("TRUNCATE TABLE mua_shop")
        c.execute("SET FOREIGN_KEY_CHECKS=1")

        for x in gd:
            c.execute(
                "INSERT INTO giao_dich (at,ngay,acc_chu,ten_chu,ten_chu_vn,ip_chu,"
                "acc_khach,ten_khach,ten_khach_vn,ip_khach,vi_tri_x,vi_tri_y,"
                "ban_do,ban_do_vn,tien_chu_nhan,tien_khach_nhan,nguon_tep,dong_goc) "
                "VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)",
                (x.get('at'), x.get('ngay'), x.get('acc_chu'), x.get('ten_chu'),
                 vn(x.get('ten_chu')), x.get('ip_chu'), x.get('acc_khach'),
                 x.get('ten_khach'), vn(x.get('ten_khach')), x.get('ip_khach'),
                 x.get('vi_tri_x'), x.get('vi_tri_y'), x.get('ban_do'),
                 vn(x.get('ban_do')), x.get('tien_chu_nhan'),
                 x.get('tien_khach_nhan'), x['nguon_tep'], x['dong_goc']))
            gid = c.lastrowid
            for o in x['items']:
                c.execute(
                    "INSERT INTO giao_dich_item (giao_dich_id,ben_nhan,ten_item,"
                    "ten_item_vn,gold_id,gen,detail_type,particular,cap,he,options) "
                    "VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)",
                    (gid, o['ben_nhan'], o.get('ten_item'), vn(o.get('ten_item')),
                     o.get('gold_id'), o.get('gen'), o.get('detail_type'),
                     o.get('particular'), o.get('cap'), o.get('he'),
                     ' '.join(o['options'])[:128] or None))

        for x in ms:
            c.execute(
                "INSERT INTO mua_shop (at,ngay,acc_name,role_name,role_name_vn,ip,"
                "kind_shop,ten_item,ten_item_vn,so_luong,gold_id,gen,detail_type,"
                "particular,cap,he,line,xu,tong_tien_van,nguon_tep,dong_goc) "
                "VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)",
                (x.get('at'), x.get('ngay'), x.get('acc_name'), x.get('role_name'),
                 vn(x.get('role_name')), x.get('ip'), x.get('kind_shop'),
                 x.get('ten_item'), vn(x.get('ten_item')), x.get('so_luong'),
                 x.get('gold_id'), x.get('gen'), x.get('detail_type'),
                 x.get('particular'), x.get('cap'), x.get('he'), x.get('line'),
                 x.get('xu'), x.get('tong_tien_van'), x['nguon_tep'], x['dong_goc']))
        cn.commit()
    except Exception as e:
        cn.rollback()
        print('!! LOI -> DA HUY (rollback): %s' % e)
        raise

    print()
    print('-' * 74)
    print('KIEM CHUNG NGUOC')
    print('-' * 74)
    for bang, mong in (('giao_dich', len(gd)), ('giao_dich_item', n_item),
                       ('mua_shop', len(ms))):
        c.execute('SELECT COUNT(*) FROM `%s`' % bang)
        n = c.fetchone()[0]
        print('  %-16s %6d dong  (doc duoc %6d)  %s'
              % (bang, n, mong, 'KHOP' if n == mong else '!! LECH'))
    cn.close()


if __name__ == '__main__':
    main()
