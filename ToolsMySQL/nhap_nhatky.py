# -*- coding: ascii -*-
"""
nhap_nhatky.py -- Di tru NHAT KY tu tep text sang bang jx1_game.game_log.

Nguon:
  baucua\\game_log.txt          -> loai 'baucua'   (104 MB / ~1,42 trieu dong)
  dulieu\\player_log\\**\\*      -> loai theo ten thu muc con (log_game, log_ktc, ...)
  dulieu\\*.txt (mot so tep log) -> loai theo ten tep

Bang game_log PHAN VUNG THEO THANG. Cac dong cu khong co dau thoi gian ro rang thi
gan vao ngay SUA DOI CUOI cua tep -- ghi ro trong ghi_chu de sau nay khong hieu nham
day la thoi diem that.

Chay: python nhap_nhatky.py --thumuc <bin\\server> [--gioihan N] [--chi-thu]
"""

import argparse
import datetime
import os
import re
import sys

try:
    import pymysql
except ImportError:
    pymysql = None

# Dong dang "[Tue Aug 19 11:22:33 2026] noi dung"  (ctime)
RE_CTIME = re.compile(rb'^\[\s*\w{3}\s+(\w{3})\s+(\d{1,2})\s+\d{2}:\d{2}:\d{2}\s+(\d{4})')
THANG = {b'Jan': 1, b'Feb': 2, b'Mar': 3, b'Apr': 4, b'May': 5, b'Jun': 6,
         b'Jul': 7, b'Aug': 8, b'Sep': 9, b'Oct': 10, b'Nov': 11, b'Dec': 12}


def ngay_cua_dong(line, ngay_mac_dinh):
    m = RE_CTIME.match(line)
    if not m:
        return ngay_mac_dinh, False
    try:
        return datetime.date(int(m.group(3)), THANG[m.group(1)], int(m.group(2))), True
    except Exception:
        return ngay_mac_dinh, False


def gom_tep(thumuc):
    """Tra ve danh sach (loai, duong_dan)."""
    ra = []
    p = os.path.join(thumuc, 'baucua', 'game_log.txt')
    if os.path.exists(p):
        ra.append(('baucua', p))
    goc = os.path.join(thumuc, 'dulieu', 'player_log')
    if os.path.isdir(goc):
        for r, _d, fs in os.walk(goc):
            loai = os.path.basename(r)
            if loai == 'player_log':
                loai = 'player'
            for f in fs:
                if f.lower().endswith(('.zip', '.rar', '.7z')):
                    continue
                ra.append((loai[:32], os.path.join(r, f)))
    for f in ('LogQuanLyBoss.txt', 'XoaSkillLog.txt', 'logDoiKvThanhTienXu.txt',
              'logmuaF.txt', 'NhanMocEventTet.txt', 'LoiDaiHonChien_MangTam.txt'):
        p = os.path.join(thumuc, 'dulieu', f)
        if os.path.exists(p):
            ra.append((os.path.splitext(f)[0][:32], p))
    return ra


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--thumuc', required=True)
    ap.add_argument('--host', default='127.0.0.1')
    ap.add_argument('--port', type=int, default=3306)
    ap.add_argument('--user', default='root')
    ap.add_argument('--matkhau', default='123456')
    ap.add_argument('--db', default='jx1_game')
    ap.add_argument('--gioihan', type=int, default=0,
                    help='chi nhap toi da bao nhieu dong moi tep (0 = khong gioi han)')
    ap.add_argument('--chi-thu', action='store_true', dest='chi_thu')
    a = ap.parse_args()

    tep = gom_tep(a.thumuc)
    print("=" * 74)
    print("DI TRU NHAT KY -> jx1_game.game_log")
    print("tim thay %d tep nhat ky" % len(tep))
    print("=" * 74)
    tong_dong = 0
    for loai, p in tep[:12]:
        try:
            n = os.path.getsize(p)
        except Exception:
            n = 0
        print("  %-14s %10d byte  %s" % (loai, n, os.path.basename(p)))
    if len(tep) > 12:
        print("  ... con %d tep nua" % (len(tep) - 12))

    if a.chi_thu:
        print("\n[--chi-thu] Dung o day, KHONG ghi gi vao MySQL.")
        return
    if pymysql is None:
        print("!! thieu pymysql"); sys.exit(5)

    cn = pymysql.connect(host=a.host, port=a.port, user=a.user, password=a.matkhau,
                         database=a.db, charset='latin1', autocommit=False,
                         binary_prefix=True)
    cur = cn.cursor()
    cur.execute("SET SESSION sql_mode='STRICT_ALL_TABLES'")
    now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    SQL = ("INSERT INTO game_log (ngay,loai,role_name,noi_dung,at) "
           "VALUES (%s,%s,NULL,%s,%s)")
    tong = 0
    co_thoigian = 0
    try:
        for loai, p in tep:
            try:
                mt = datetime.date.fromtimestamp(os.path.getmtime(p))
            except Exception:
                mt = datetime.date.today()
            lo = []
            n_tep = 0
            with open(p, 'rb') as f:
                for line in f:
                    line = line.rstrip(b'\r\n')
                    if not line:
                        continue
                    if len(line) > 1024:
                        line = line[:1024]
                    ng, cothat = ngay_cua_dong(line, mt)
                    if cothat:
                        co_thoigian += 1
                    lo.append((ng.strftime('%Y-%m-%d'), loai, line,
                               ng.strftime('%Y-%m-%d') + ' 00:00:00'))
                    n_tep += 1
                    if a.gioihan and n_tep >= a.gioihan:
                        break
                    if len(lo) >= 2000:
                        cur.executemany(SQL, lo)
                        tong += len(lo)
                        lo = []
                        if tong % 100000 == 0:
                            cn.commit()
                            print("   ... da nhap %d dong" % tong)
            if lo:
                cur.executemany(SQL, lo)
                tong += len(lo)
            cn.commit()
        cur.execute(
            "INSERT INTO migrate_audit_game (run_at,phan,nguon,nguon_sha256,n_doc,n_loai,"
            "n_nhap,n_kiem,ghi_chu) VALUES (%s,%s,%s,NULL,%s,%s,%s,%s,%s)",
            (now, 'nhatky', a.thumuc, tong, 0, tong, 0,
             "%d/%d dong co dau thoi gian that; so con lai gan theo ngay sua doi cuoi cua tep"
             % (co_thoigian, tong)))
        cn.commit()
    except Exception as e:
        cn.rollback()
        print("!! LOI -> DA ROLLBACK: %s" % e)
        raise

    print("\nda nhap %d dong (%d dong co dau thoi gian that trong noi dung)"
          % (tong, co_thoigian))

    # -------------------------------------------------- kiem chung
    cur.execute("SELECT loai, COUNT(*) FROM game_log GROUP BY loai ORDER BY 2 DESC LIMIT 12")
    print("\n%-20s %12s" % ("LOAI", "SO DONG"))
    print("-" * 34)
    tongdb = 0
    for l, n in cur.fetchall():
        if isinstance(l, bytes):
            l = l.decode('latin-1')
        print("%-20s %12d" % (l, n))
    cur.execute("SELECT COUNT(*) FROM game_log")
    tongdb = cur.fetchone()[0]
    print("-" * 34)
    print("%-20s %12d" % ("TONG TRONG BANG", tongdb))
    print("\n>>> KET QUA: %s" % ("DAT" if tongdb >= tong else "KHONG DAT"))
    cur.close()
    cn.close()


if __name__ == '__main__':
    main()
