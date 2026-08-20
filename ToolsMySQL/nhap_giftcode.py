# -*- coding: ascii -*-
"""
nhap_giftcode.py -- Di tru giftcode tu tep text sang bang jx1_game.giftcode.

Nguon:
  dulieu\\giftcode\\giftcode_new_list.txt   -> lo 'new'
  dulieu\\giftcode\\giftcode_new_used.txt   -> danh dau da dung cho lo 'new'
  dulieu\\giftcode\\giftcode_tuan_list.txt  -> lo 'tuan'
  dulieu\\giftcode\\giftcode_tuan_used.txt  -> danh dau da dung cho lo 'tuan'

CANH BAO da kiem chung: giftcode_tuan_used.txt HIEN KHONG TON TAI tren may dang
chay. Neu no tung ton tai roi bi mat thi 5.000 ma tuan da quay ve trang thai
"chua dung" tu luc do. Cong cu nay khong dung lai duoc thong tin da mat -- no chi
ghi lai dung nhung gi con tren dia, va BAO RO so ma duoc danh dau la da dung.

Chay: python nhap_giftcode.py --thumuc <bin\\server> [--xoa-truoc] [--chi-thu]
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

LO = [
    ("new",  "giftcode_new_list.txt",  "giftcode_new_used.txt"),
    ("tuan", "giftcode_tuan_list.txt", "giftcode_tuan_used.txt"),
]


def doc_ma(p):
    """Doc tep ma, moi dong mot ma. Tra ve danh sach byte tho, giu thu tu, bo trung."""
    if not os.path.exists(p):
        return None
    ra = []
    thay = set()
    with open(p, 'rb') as f:
        for line in f:
            c = line.strip()
            if not c:
                continue
            if len(c) > 64:
                continue
            if c in thay:
                continue
            thay.add(c)
            ra.append(c)
    return ra


def sha256(p):
    if not os.path.exists(p):
        return None
    h = hashlib.sha256()
    with open(p, 'rb') as f:
        for b in iter(lambda: f.read(1 << 20), b''):
            h.update(b)
    return h.hexdigest()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--thumuc', required=True, help='thu muc bin\\server')
    ap.add_argument('--host', default='127.0.0.1')
    ap.add_argument('--port', type=int, default=3306)
    ap.add_argument('--user', default='root')
    ap.add_argument('--matkhau', default='123456')
    ap.add_argument('--db', default='jx1_game')
    ap.add_argument('--xoa-truoc', action='store_true', dest='xoa_truoc')
    ap.add_argument('--chi-thu', action='store_true', dest='chi_thu')
    a = ap.parse_args()

    goc = os.path.join(a.thumuc, 'dulieu', 'giftcode')
    print("=" * 74)
    print("DI TRU GIFTCODE -> jx1_game.giftcode")
    print("thu muc: %s" % goc)
    print("=" * 74)

    hang = []      # (code, batch, used)
    tomtat = []
    for lo, tep_list, tep_used in LO:
        p_list = os.path.join(goc, tep_list)
        p_used = os.path.join(goc, tep_used)
        ds = doc_ma(p_list)
        if ds is None:
            print("  lo %-5s KHONG CO tep danh sach (%s) -> bo qua" % (lo, tep_list))
            continue
        du = doc_ma(p_used)
        if du is None:
            print("  lo %-5s !! KHONG CO tep da-dung (%s) -- coi nhu 0 ma da dung."
                  % (lo, tep_used))
            print("           Neu tep nay tung ton tai roi bi mat thi so ma da dung"
                  " DA BI MAT TU TRUOC, khong khoi phuc duoc.")
            du = []
        tap_used = set(du)
        # Ma nam trong tep da-dung nhung KHONG co trong danh sach: van phai ghi lai,
        # neu khong thi lan sau no thanh "chua dung".
        ngoai = [c for c in du if c not in set(ds)]
        for c in ds:
            hang.append((c, lo, c in tap_used))
        for c in ngoai:
            hang.append((c, lo, True))
        print("  lo %-5s: %d ma trong danh sach, %d da dung (%d ma da dung nam"
              " NGOAI danh sach)" % (lo, len(ds), len(tap_used), len(ngoai)))
        tomtat.append((lo, p_list, sha256(p_list), len(ds), len(tap_used)))

    print()
    print("TONG: %d ma, trong do %d da dung" % (len(hang), sum(1 for _c, _b, u in hang if u)))

    # Kiem trung khoa ngay tai nguon (code la KHOA CHINH, khong duoc trung giua 2 lo)
    thay = {}
    trung = []
    for c, b, u in hang:
        if c in thay and thay[c] != b:
            trung.append((c, thay[c], b))
        thay[c] = b
    if trung:
        print("!! CO %d ma xuat hien o CA HAI lo -- code la khoa chinh nen phai xu ly tay:"
              % len(trung))
        for c, b1, b2 in trung[:5]:
            print("     %r : lo %s va lo %s" % (c, b1, b2))
        sys.exit(4)

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
            cur.execute("DELETE FROM giftcode")
            print("da xoa sach bang giftcode (%d dong cu)" % cur.rowcount)
        lo_kich = 500
        for i in range(0, len(hang), lo_kich):
            cur.executemany(
                "INSERT INTO giftcode (code,batch,used_at,created_at) VALUES (%s,%s,%s,%s)",
                [(c, b, (now if u else None), now) for c, b, u in hang[i:i + lo_kich]])
        cur.execute(
            "INSERT INTO migrate_audit_game (run_at,phan,nguon,nguon_sha256,n_doc,n_loai,"
            "n_nhap,n_kiem,ghi_chu) VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s)",
            (now, 'giftcode', goc, None, len(hang), 0, len(hang), 0,
             "; ".join("%s: %d ma / %d da dung (sha %s)" % (l, n, nu, (h or '')[:12])
                       for l, _p, h, n, nu in tomtat)))
        cn.commit()
        print("da nhap %d ma" % len(hang))
    except Exception as e:
        cn.rollback()
        print("!! LOI -> DA ROLLBACK: %s" % e)
        raise

    # ------------------------------------------------ kiem chung nguoc
    print("\n" + "-" * 74)
    print("KIEM CHUNG NGUOC")
    print("-" * 74)
    cur.execute("SELECT code, batch, used_at IS NOT NULL FROM giftcode")
    tu_db = {}
    for c, b, u in cur.fetchall():
        if isinstance(b, str):
            b = b.encode('latin-1')
        tu_db[c] = (b, bool(u))
    nguon = {c: (b.encode('latin-1'), u) for c, b, u in hang}
    khop = sum(1 for c, v in nguon.items() if tu_db.get(c) == v)
    print("ma o nguon        : %d" % len(nguon))
    print("ma trong MySQL    : %d" % len(tu_db))
    print("KHOP (ma + lo + trang thai da dung): %d" % khop)
    thieu = [c for c in nguon if c not in tu_db]
    thua = [c for c in tu_db if c not in nguon]
    print("THIEU / THUA      : %d / %d %s" % (len(thieu), len(thua), thieu[:3]))
    ok = (khop == len(nguon) == len(tu_db) and not thieu and not thua)
    cur.execute("UPDATE migrate_audit_game SET n_kiem=%s WHERE phan='giftcode' "
                "ORDER BY id DESC LIMIT 1", (khop,))
    cn.commit()
    print("\n>>> KET QUA: %s" % ("DAT" if ok else "KHONG DAT"))
    cur.close()
    cn.close()
    sys.exit(0 if ok else 6)


if __name__ == '__main__':
    main()
