# -*- coding: ascii -*-
"""
nhap_baucua.py -- Di tru so du TIEN XU tu baucua\\deposits.json sang
                  jx1_game.baucua_balance.

BAY DA KIEM CHUNG:
  * Khoa trong JSON la base64 cua TEN NHAN VAT dang BYTE THO (ANSI/TCVN3), khong
    phai UTF-8. Giai ma xong phai giu nguyen byte -> cot VARBINARY.
  * Tep that co the THIEU khoa "locked" (vong ghi rong nen nlohmann khong tao
    khoa). Phai chiu duoc, neu khong la nem ngoai le va mat sach so du.
  * Tep that co the co khoa RONG "" (do balances[""] bi tao khi currentHost rong).
    Bo qua, va bao ro.

Chay: python nhap_baucua.py --tep <duong_dan deposits.json> [--xoa-truoc] [--chi-thu]
"""

import argparse
import base64
import datetime
import hashlib
import json
import os
import sys

try:
    import pymysql
except ImportError:
    pymysql = None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--tep', required=True)
    ap.add_argument('--host', default='127.0.0.1')
    ap.add_argument('--port', type=int, default=3306)
    ap.add_argument('--user', default='root')
    ap.add_argument('--matkhau', default='123456')
    ap.add_argument('--db', default='jx1_game')
    ap.add_argument('--xoa-truoc', action='store_true', dest='xoa_truoc')
    ap.add_argument('--chi-thu', action='store_true', dest='chi_thu')
    a = ap.parse_args()

    print("=" * 74)
    print("DI TRU TIEN XU -> jx1_game.baucua_balance")
    print("nguon: %s" % a.tep)
    print("=" * 74)

    if not os.path.exists(a.tep):
        print("!! khong thay tep nguon"); sys.exit(2)
    raw = open(a.tep, 'rb').read()
    print("kich thuoc %d byte, sha256 %s" % (len(raw), hashlib.sha256(raw).hexdigest()[:16]))
    if not raw.strip():
        print("!! TEP RONG (0 byte) -- khong co gi de nhap.")
        print("   Day dung la trieu chung cua loi ghi khong nguyen tu da duoc va o dot nay.")
        sys.exit(3)

    try:
        j = json.loads(raw.decode('latin-1'))
    except Exception as e:
        print("!! JSON hong: %s" % e); sys.exit(3)

    sodu = {}
    khoa = {}
    bo_rong = 0
    for nhan, dich in (("balances", sodu), ("locked", khoa)):
        if nhan not in j:
            print("  (tep khong co khoa \"%s\" -- binh thuong, khong phai loi)" % nhan)
            continue
        for k, v in j[nhan].items():
            try:
                ten = base64.b64decode(k)
            except Exception:
                print("  !! khoa base64 hong, bo qua: %r" % k[:32])
                continue
            if not ten:
                bo_rong += 1
                continue
            if len(ten) > 32:
                print("  !! ten dai %d byte, bo qua: %r" % (len(ten), ten[:32]))
                continue
            dich[ten] = int(v)

    if bo_rong:
        print("  (bo qua %d muc co ten RONG -- do balances[\"\"] bi tao khi khong co nha cai)"
              % bo_rong)

    hang = []
    for ten in set(list(sodu.keys()) + list(khoa.keys())):
        hang.append((ten, sodu.get(ten, 0), khoa.get(ten, 0)))
    hang.sort()

    print()
    print("%-28s %12s %12s" % ("TEN NHAN VAT (hex neu co dau)", "SO DU", "DANG KHOA"))
    print("-" * 56)
    tong = 0
    for ten, sd, kh in hang:
        hienthi = ten.decode('latin-1') if all(32 <= b < 127 for b in ten) else ten.hex()
        print("%-28s %12d %12d" % (hienthi, sd, kh))
        tong += sd + kh
    print("-" * 56)
    print("%-28s %12d" % ("TONG XU", tong))

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
    try:
        if a.xoa_truoc:
            cur.execute("DELETE FROM baucua_balance")
            print("\nda xoa sach baucua_balance (%d dong cu)" % cur.rowcount)
        if hang:
            cur.executemany(
                "INSERT INTO baucua_balance (role_name,balance,locked) VALUES (%s,%s,%s) "
                "ON DUPLICATE KEY UPDATE balance=VALUES(balance), locked=VALUES(locked)",
                hang)
            # ghi so cai mot dong "di tru" cho tung nguoi, de co diem xuat phat
            cur.executemany(
                "INSERT INTO baucua_ledger (role_name,viec,so_tien,so_du_sau,at) "
                "VALUES (%s,'ditru',%s,%s,%s)",
                [(t, sd, sd, now) for t, sd, _k in hang])
        cur.execute(
            "INSERT INTO migrate_audit_game (run_at,phan,nguon,nguon_sha256,n_doc,n_loai,"
            "n_nhap,n_kiem,ghi_chu) VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s)",
            (now, 'baucua', a.tep, hashlib.sha256(raw).hexdigest(), len(hang), bo_rong,
             len(hang), 0, "tong xu = %d" % tong))
        cn.commit()
        print("da nhap %d nguoi choi" % len(hang))
    except Exception as e:
        cn.rollback()
        print("!! LOI -> DA ROLLBACK: %s" % e)
        raise

    # ------------------------------------------------ kiem chung nguoc
    print("\n" + "-" * 74)
    print("KIEM CHUNG NGUOC")
    print("-" * 74)
    cur.execute("SELECT role_name, balance, locked FROM baucua_balance")
    tu_db = {t: (int(s), int(k)) for t, s, k in cur.fetchall()}
    nguon = {t: (s, k) for t, s, k in hang}
    khop = sum(1 for t, v in nguon.items() if tu_db.get(t) == v)
    tong_db = sum(s + k for s, k in tu_db.values())
    print("nguoi choi o nguon : %d" % len(nguon))
    print("nguoi choi o MySQL : %d" % len(tu_db))
    print("KHOP (ten + so du + dang khoa): %d" % khop)
    print("TONG XU nguon / MySQL: %d / %d  %s" % (tong, tong_db,
          "KHOP" if tong == tong_db else "!! LECH"))
    ok = (khop == len(nguon) == len(tu_db) and tong == tong_db)
    cur.execute("UPDATE migrate_audit_game SET n_kiem=%s WHERE phan='baucua' "
                "ORDER BY id DESC LIMIT 1", (khop,))
    cn.commit()
    print("\n>>> KET QUA: %s" % ("DAT" if ok else "KHONG DAT"))
    cur.close()
    cn.close()
    sys.exit(0 if ok else 6)


if __name__ == '__main__':
    main()
