# -*- coding: ascii -*-
"""
nhap_relay.py -- Di tru 5 kho Berkeley DB cua S3Relay sang bang MySQL `relay_kv`.

Cung nguyen tac an toan nhu nhap_role.py:
  * CHI DOC nguon, khong ghi mot byte nao vao Berkeley DB.
  * Tu choi chay neu tep nguon dang bi khoa (server con chay).
  * Blob giu NGUYEN VEN tung byte -> chung minh duoc bang doi chieu nguoc.
  * Ban ghi lech kich thuoc bi TU CHOI RO RANG kem ly do, khong loai im lang.
  * Kiem chung nguoc 100%: doc lai tu MySQL va so tung byte.

Bo cuc khoa phu (lay tu ma nguon, da doi chieu voi du lieu that):
  MemberTable : TMemberStruct.szTong   @32   (szName[32] dung truoc)
  ZhaoMuTable : TZhaoMuStruct.szTong   @0
  MoneyTable  : TMoneyStruct.m_dwMoney @32   -> cot money, BIGINT CO DAU

Chay:  python nhap_relay.py --thumuc <bin\\multiserver> [--xoa-truoc] [--chi-thu]
"""

import argparse
import hashlib
import os
import struct
import sys
import datetime

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from jx_bdb import BdbFile, is_locked      # noqa: E402

try:
    import pymysql
except ImportError:
    pymysql = None

# (ten store, duong dan tuong doi, kich thuoc ban ghi bat buoc (0 = bien thien),
#  offset szTong (-1 = khong co), do dai toi da szTong, offset tien (-1 = khong co))
KHO = [
    ("friend",      "dbfriend/friend",           0,  -1,  0, -1),
    ("TongTable",   "TongDB/TongTable",       6860,  -1,  0, -1),
    ("MemberTable", "TongMemberDB/MemberTable", 404,  32, 32, -1),
    ("MoneyTable",  "TongMoneyDB/MoneyTable",    36,  -1,  0, 32),
    ("ZhaoMuTable", "TongZhaoMuDB/ZhaoMuTable",  76,   0, 32, -1),
]


def cstr(buf, off, maxlen):
    raw = buf[off:off + maxlen]
    i = raw.find(b'\x00')
    return raw if i < 0 else raw[:i]


def sha256_file(p):
    h = hashlib.sha256()
    with open(p, 'rb') as fh:
        for blk in iter(lambda: fh.read(1 << 20), b''):
            h.update(blk)
    return h.hexdigest()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--thumuc', required=True, help='thu muc bin\\multiserver')
    ap.add_argument('--host', default='127.0.0.1')
    ap.add_argument('--port', type=int, default=3306)
    ap.add_argument('--user', default='root')
    ap.add_argument('--matkhau', default='123456')
    ap.add_argument('--db', default='jx1_role')
    ap.add_argument('--xoa-truoc', action='store_true', dest='xoa_truoc')
    ap.add_argument('--chi-thu', action='store_true', dest='chi_thu')
    ap.add_argument('--cho-phep-khoa', action='store_true', dest='cho_phep_khoa')
    a = ap.parse_args()

    print("=" * 74)
    print("DI TRU 5 KHO S3RELAY -> MySQL `relay_kv`")
    print("thu muc: %s" % a.thumuc)
    print("=" * 74)

    tatca = []      # (store, key_khong_nul, blob, idx0 hoac None, money hoac None)
    loai = []
    for ten, rel, cosize, ioff, imax, moff in KHO:
        p = os.path.join(a.thumuc, rel.replace('/', os.sep))
        if not os.path.exists(p):
            print("  %-14s KHONG CO TEP -> bo qua (%s)" % (ten, p))
            continue
        if is_locked(p) and not a.cho_phep_khoa:
            print("!! %s DANG BI KHOA -- server con chay. Dung lai." % ten)
            sys.exit(3)
        f = BdbFile(p)
        n = 0
        for k, v, _pg, _s in f.items():
            kk = k.rstrip(b'\x00')
            if not kk or len(kk) > 64:
                loai.append((ten, k, "khoa dai %d byte" % len(kk)))
                continue
            if cosize and len(v) != cosize:
                loai.append((ten, k, "ban ghi %d byte, can %d" % (len(v), cosize)))
                continue
            idx0 = None
            if ioff >= 0 and len(v) >= ioff + imax:
                idx0 = cstr(v, ioff, imax)
                if not idx0:
                    idx0 = None
            money = None
            if moff >= 0 and len(v) >= moff + 4:
                money = struct.unpack_from('<I', v, moff)[0]
            tatca.append((ten, kk, v, idx0, money))
            n += 1
        print("  %-14s %4d ban ghi  (%s, sha256 %s...)"
              % (ten, n, "%d byte" % os.path.getsize(p), sha256_file(p)[:12]))

    print()
    print("TONG: %d ban ghi hop le, %d bi loai" % (len(tatca), len(loai)))
    for ten, k, why in loai[:10]:
        print("     LOAI %-14s %-24r %s" % (ten, k[:20], why))

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
    try:
        if a.xoa_truoc:
            cur.execute("DELETE FROM relay_kv")
            print("da xoa sach relay_kv (%d dong cu)" % cur.rowcount)
        if tatca:
            cur.executemany(
                "INSERT INTO relay_kv (store,k,v,v_len,idx0,money) VALUES (%s,%s,%s,%s,%s,%s)",
                [(t, k, v, len(v), i, m) for t, k, v, i, m in tatca])
        cn.commit()
        print("da nhap %d ban ghi" % len(tatca))
    except Exception as e:
        cn.rollback()
        print("!! LOI -> DA ROLLBACK: %s" % e)
        raise

    # -------------------------------------------------- kiem chung nguoc
    print("\n" + "-" * 74)
    print("KIEM CHUNG NGUOC: doc lai tung ban ghi tu MySQL va so tung byte")
    print("-" * 74)
    cur.execute("SELECT store, k, v FROM relay_kv")
    # LUU Y: bang dat DEFAULT CHARSET=binary nen VARCHAR(24) thanh VARBINARY(24),
    # MySQL tra ve `bytes` chu khong phai chuoi. Quy ve bytes cho ca hai ben.
    def b(x):
        return x.encode('latin-1') if isinstance(x, str) else x
    tu_db = {(b(t), b(k)): v for t, k, v in cur.fetchall()}
    nguon = {(b(t), b(k)): v for t, k, v, _i, _m in tatca}
    khop = sum(1 for kk, v in nguon.items() if tu_db.get(kk) == v)
    thieu = [k for k in nguon if k not in tu_db]
    thua = [k for k in tu_db if k not in nguon]
    print("ban ghi nguon       : %d" % len(nguon))
    print("ban ghi trong MySQL : %d" % len(tu_db))
    print("KHOP TUNG BYTE      : %d" % khop)
    print("THIEU / THUA        : %d / %d" % (len(thieu), len(thua)))
    ok = (khop == len(nguon) == len(tu_db) and not thieu and not thua)
    print("\n>>> KET QUA: %s" % ("DAT -- 100% khop tung byte" if ok else "KHONG DAT"))
    cur.close()
    cn.close()
    sys.exit(0 if ok else 6)


if __name__ == '__main__':
    main()
