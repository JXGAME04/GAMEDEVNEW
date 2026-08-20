# -*- coding: ascii -*-
"""
nhap_kv.py -- Dua cac TEP TRANG THAI len bang jx1_game.game_kv.

Cac tep nay giu nguyen dinh dang cu; MySQL chi la noi cat giu + ban sao. Nho vay
KHONG phai viet lai bo tuan tu hoa cua tung tinh nang (rui ro cao, khong them gia
tri). Khi tep bi mat, CoreServer tu dung lai tu day luc khoi dong
(KGameKV::GetFileToDisk trong g_InitCore).

Chay: python nhap_kv.py --thumuc <bin\\server> [--multiserver <bin\\multiserver>] [--chi-thu]
"""

import argparse
import datetime
import hashlib
import os
import sys

try:
    import pymysql
except ImportError:
    pymysql = None

# (ns, key, duong dan tuong doi so voi thu muc goc, thuoc thu muc nao)
MUC = [
    ("jx2.citywar",  "file", "settings/jx2citywar.txt",  "server"),
    ("jx2.ladder",   "file", "settings/jx2ladder.txt",   "server"),
    ("jx2.league",   "file", "settings/jx2league.txt",   "server"),
    ("statdata",     "file", "StatData.dat",             "multiserver"),
    ("lua.codenew",  "file", "script/codenew.lua",           "server"),
    ("lua.fancung",  "file", "script/giftcode_fancung.lua",  "server"),
    ("lua.nganluong", "file", "script/songbac/datanganluong.lua", "server"),
    ("lua.tienvang",  "file", "script/songbac/datatienvang.lua",  "server"),
]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--thumuc', required=True, help='bin\\server')
    ap.add_argument('--multiserver', default='', help='bin\\multiserver')
    ap.add_argument('--host', default='127.0.0.1')
    ap.add_argument('--port', type=int, default=3306)
    ap.add_argument('--user', default='root')
    ap.add_argument('--matkhau', default='123456')
    ap.add_argument('--db', default='jx1_game')
    ap.add_argument('--chi-thu', action='store_true', dest='chi_thu')
    a = ap.parse_args()

    print("=" * 74)
    print("DUA TEP TRANG THAI -> jx1_game.game_kv")
    print("=" * 74)

    hang = []
    for ns, k, rel, thuoc in MUC:
        goc = a.multiserver if thuoc == 'multiserver' else a.thumuc
        if not goc:
            print("  %-16s (bo qua: chua cho duong dan %s)" % (ns, thuoc))
            continue
        p = os.path.join(goc, rel.replace('/', os.sep))
        if not os.path.exists(p):
            print("  %-16s KHONG CO tep (%s)" % (ns, rel))
            continue
        d = open(p, 'rb').read()
        if len(d) > 8 * 1024 * 1024:
            print("  %-16s BO QUA vi %d byte > tran 8 MB" % (ns, len(d)))
            continue
        hang.append((ns, k, d))
        print("  %-16s %9d byte  sha %s  <- %s"
              % (ns, len(d), hashlib.sha256(d).hexdigest()[:12], rel))

    print()
    print("TONG: %d muc, %d byte" % (len(hang), sum(len(d) for _n, _k, d in hang)))

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
    # LUU Y: max_allowed_packet CHI doi duoc o cap GLOBAL (MySQL 5.7). Muc lon
    # nhat o day la codenew.lua ~399 KB, con xa han 4 MB mac dinh nen khong can.
    now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    try:
        for ns, k, d in hang:
            cur.execute(
                "INSERT INTO game_kv (ns,k,v,v_len) VALUES (%s,%s,%s,%s) "
                "ON DUPLICATE KEY UPDATE v=VALUES(v), v_len=VALUES(v_len)",
                (ns, k.encode('latin-1'), d, len(d)))
        cur.execute(
            "INSERT INTO migrate_audit_game (run_at,phan,nguon,nguon_sha256,n_doc,n_loai,"
            "n_nhap,n_kiem,ghi_chu) VALUES (%s,%s,%s,NULL,%s,%s,%s,%s,%s)",
            (now, 'game_kv', a.thumuc, len(hang), 0, len(hang), 0, ''))
        cn.commit()
        print("da nhap %d muc" % len(hang))
    except Exception as e:
        cn.rollback()
        print("!! LOI -> DA ROLLBACK: %s" % e)
        raise

    # -------------------------------------------------- kiem chung nguoc
    print("\n" + "-" * 74)
    print("KIEM CHUNG NGUOC: doc lai va so TUNG BYTE")
    print("-" * 74)
    khop = 0
    for ns, k, d in hang:
        cur.execute("SELECT v FROM game_kv WHERE ns=%s AND k=%s", (ns, k.encode('latin-1')))
        r = cur.fetchone()
        ok = (r is not None and r[0] == d)
        print("  %-16s %s (%d byte)" % (ns, "KHOP" if ok else "!! LECH", len(d)))
        if ok:
            khop += 1
    cur.execute("UPDATE migrate_audit_game SET n_kiem=%s WHERE phan='game_kv' "
                "ORDER BY id DESC LIMIT 1", (khop,))
    cn.commit()
    print("\n>>> KET QUA: %s (%d/%d)" % ("DAT" if khop == len(hang) else "KHONG DAT",
                                         khop, len(hang)))
    cur.close()
    cn.close()
    sys.exit(0 if khop == len(hang) else 6)


if __name__ == '__main__':
    main()
