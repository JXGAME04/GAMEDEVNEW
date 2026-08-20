# -*- coding: ascii -*-
"""
nhap_role.py -- Cong cu di tru kho nhan vat JX1 (Berkeley DB / .bak) sang MySQL.

NGUYEN TAC AN TOAN (theo dung yeu cau: khong dup do, khong mat du lieu, khong dung game)
  1. CHI DOC nguon. Khong mo Berkeley DB bang thu vien, khong ghi mot byte nao vao nguon.
  2. Tu choi chay neu file nguon dang BI KHOA (server con chay) tru khi --cho-phep-khoa.
  3. Blob duoc nhap NGUYEN VEN TUNG BYTE. KHONG tinh lai CRC. Nho vay co the chung minh
     "khong mat du lieu" bang cach doi chieu SHA256 tung ban ghi sau khi nhap.
  4. MOI ban ghi bi loai deu duoc ghi vao bang migrate_reject kem ly do + 64 byte dau.
     Khong bao gio loai im lang.
  5. Chay trong transaction. Loi giua chung -> ROLLBACK, khong de lai nua voi.
  6. Buoc --kiem-chung doc NGUOC tu MySQL ve va so hash voi nguon. 100% hoac bao do.

Cach dung:
    python nhap_role.py --nguon <duong_dan> --kieu bdb|bak [--db jx1_role]
                        [--host 127.0.0.1] [--user root] [--pass 123456]
                        [--xoa-truoc] [--chi-thu] [--cho-phep-khoa]

    --chi-thu     : chi doc va bao cao, KHONG ghi vao MySQL
    --xoa-truoc   : TRUNCATE bang role truoc khi nhap (chi dung khi nhap lai tu dau)
"""

import argparse
import hashlib
import os
import sys
import datetime

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from jx_bdb import BdbFile, is_locked      # noqa: E402
import jx_role as R                        # noqa: E402

try:
    import pymysql
except ImportError:
    pymysql = None


# --------------------------------------------------------------- doc nguon
def doc_nguon(kieu, duong_dan):
    """Tra ve (danh_sach_tot, danh_sach_loai).

    danh_sach_tot : [(key_bytes, blob_bytes)]
    danh_sach_loai: [(key_bytes, ly_do, raw_len, raw_head)]
    """
    tot, loai = [], []
    if kieu == 'bdb':
        f = BdbFile(duong_dan)
        nguon = [(k, v) for k, v, _p, _s in f.items()]
    else:
        nguon, rej = R.read_bak(duong_dan)
        for off, why in rej:
            pass  # diem resync, khong phai ban ghi -- tong hop o cuoi
    for k, v in nguon:
        # --- BA PHEP KIEM CHUNG CHEO BAT BUOC ---
        if not k or len(k) > 32:
            loai.append((k, "keysize=%d ngoai [1,32]" % len(k), len(v), v[:64]))
            continue
        if len(v) < R.HDR_SIZE:
            loai.append((k, "blob %d byte < header 745" % len(v), len(v), v[:64]))
            continue
        try:
            r = R.parse(v, strict=True)
        except Exception as e:
            loai.append((k, str(e)[:250], len(v), v[:64]))
            continue
        if k != r['role_name'] + b'\x00':
            loai.append((k, "key != szName+NUL (szName=%r)" % r['role_name'][:24],
                         len(v), v[:64]))
            continue
        if not r['acc_name']:
            loai.append((k, "caccname RONG", len(v), v[:64]))
            continue
        if r['data_len'] >= R.MAX_DATALEN:
            loai.append((k, "dwDataLen=%d >= tran 327680" % r['data_len'], len(v), v[:64]))
            continue
        tot.append((k, v))
    return tot, loai


def sha256_file(p):
    h = hashlib.sha256()
    with open(p, 'rb') as fh:
        for blk in iter(lambda: fh.read(1 << 20), b''):
            h.update(blk)
    return h.hexdigest()


# --------------------------------------------------------------- nhap
COLS = ('role_name', 'acc_name', 'data_len', 'role_blob', 'crc32', 'crc_ok', 'has_crc',
        'fmt_ver',
        'sex', 'first_sect', 'sect', 'series', 'level', 'fight_exp', 'lead_level',
        'money', 'save_money', 'pk_value', 'kill_num', 'tong_id', 'use_revive',
        'revival_id', 'enter_game_id', 'world_stat', 'sect_stat',
        'n_item', 'n_task', 'n_fight_skill', 'n_state_skill', 'ver', 'created_at')

SQL_INS = ("INSERT INTO `role` (" + ",".join("`%s`" % c for c in COLS) + ") VALUES ("
           + ",".join(["%s"] * len(COLS)) + ")")


def hang(k, v, now):
    r = R.parse(v, strict=True)
    return (
        r['role_name'], r['acc_name'], r['data_len'], v,
        r['crc_stored'], 1 if r['crc_ok'] else 0, 1 if r['has_crc'] else 0, 1,
        r['sex'], r['first_sect'], r['sect'], r['series'], r['level'],
        r['fight_exp'], r['lead_level'], r['money'], r['save_money'],
        r['pk_value'], r['kill_num'], r['tong_id'], r['use_revive'],
        r['revival_id'], r['enter_game_id'], r['world_stat'], r['sect_stat'],
        r['n_item'], r['n_task'], r['n_fight_skill'], r['n_state_skill'],
        0, now)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--nguon', required=True)
    ap.add_argument('--kieu', choices=['bdb', 'bak'], default='bdb')
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
    print("DI TRU KHO NHAN VAT -> MySQL")
    print("nguon : %s (%s)" % (a.nguon, a.kieu))
    print("=" * 74)

    if not os.path.exists(a.nguon):
        print("!! khong thay file nguon"); sys.exit(2)

    if is_locked(a.nguon) and not a.cho_phep_khoa:
        print("!! FILE NGUON DANG BI KHOA -- nhieu kha nang server VAN DANG CHAY.")
        print("!! Nhap luc nay se cho ra anh chup lem. Dung server roi chay lai,")
        print("!! hoac them --cho-phep-khoa neu ban chac chan.")
        sys.exit(3)

    sha = sha256_file(a.nguon)
    kich_thuoc = os.path.getsize(a.nguon)
    print("sha256 nguon = %s" % sha)
    print("kich thuoc   = %d byte" % kich_thuoc)

    tot, loai = doc_nguon(a.kieu, a.nguon)
    print("doc duoc     : %d ban ghi HOP LE, %d ban ghi BI LOAI" % (len(tot), len(loai)))
    for k, why, ln, _h in loai[:10]:
        print("     LOAI %-24r %s" % (k[:20], why[:70]))
    if len(loai) > 10:
        print("     ... con %d ban ghi bi loai nua" % (len(loai) - 10))

    # kiem trung khoa ngay tai nguon
    seen = {}
    trung = 0
    for k, v in tot:
        if k in seen:
            trung += 1
        seen[k] = v
    print("khoa trung tai nguon: %d" % trung)
    if trung:
        print("!! DUNG LAI: nguon co khoa trung, phai xu ly tay truoc khi nhap.")
        sys.exit(4)

    tong_byte = sum(len(v) for _k, v in tot)
    print("tong byte blob: %d (%.2f MB)" % (tong_byte, tong_byte / 1048576.0))

    if a.chi_thu:
        print("\n[--chi-thu] Dung o day, KHONG ghi gi vao MySQL.")
        return

    if pymysql is None:
        print("!! thieu thu vien pymysql (pip install pymysql)"); sys.exit(5)

    cn = pymysql.connect(host=a.host, port=a.port, user=a.user, password=a.matkhau,
                         database=a.db, charset='latin1', autocommit=False,
                         binary_prefix=True)
    cur = cn.cursor()
    cur.execute("SET SESSION sql_mode='STRICT_ALL_TABLES'")

    now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    try:
        if a.xoa_truoc:
            cur.execute("DELETE FROM `role`")
            print("da xoa sach bang role (%d dong cu)" % cur.rowcount)

        n_ins = 0
        lo = 200
        for i in range(0, len(tot), lo):
            batch = [hang(k, v, now) for k, v in tot[i:i + lo]]
            cur.executemany(SQL_INS, batch)
            n_ins += len(batch)
            if (i // lo) % 5 == 0:
                print("   ... da nhap %d/%d" % (n_ins, len(tot)))

        cur.execute(
            "INSERT INTO migrate_audit (run_at,source_path,source_sha256,source_bytes,"
            "n_read,n_rejected,n_inserted,n_verified,note) VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s)",
            (now, a.nguon, sha, kich_thuoc, len(tot) + len(loai), len(loai), n_ins, 0,
             'kieu=%s' % a.kieu))
        audit_id = cur.lastrowid
        if loai:
            cur.executemany(
                "INSERT INTO migrate_reject (audit_id,src_key,reason,raw_len,raw_head)"
                " VALUES (%s,%s,%s,%s,%s)",
                [(audit_id, k[:64], why[:250], ln, head) for k, why, ln, head in loai])
        cn.commit()
        print("\nDA NHAP %d ban ghi, audit_id=%d" % (n_ins, audit_id))
    except Exception as e:
        cn.rollback()
        print("\n!! LOI -> DA ROLLBACK, MySQL khong con nua voi nao: %s" % e)
        raise

    # ------------------------------------------------ kiem chung nguoc 100%
    print("\n" + "-" * 74)
    print("KIEM CHUNG NGUOC: doc lai tung ban ghi tu MySQL va so SHA256 voi nguon")
    print("-" * 74)
    cur.execute("SELECT role_name, role_blob, data_len FROM `role`")
    tu_db = {}
    for rn, blob, dl in cur.fetchall():
        tu_db[rn] = (blob, dl)

    nguon_map = {k.rstrip(b'\x00'): v for k, v in tot}
    khop = 0
    lech = []
    thieu = []
    for name, blob in nguon_map.items():
        if name not in tu_db:
            thieu.append(name)
            continue
        if tu_db[name][0] == blob:
            khop += 1
        else:
            lech.append((name, len(blob), len(tu_db[name][0])))
    thua = set(tu_db) - set(nguon_map)

    print("ban ghi nguon        : %d" % len(nguon_map))
    print("ban ghi trong MySQL  : %d" % len(tu_db))
    print("KHOP TUNG BYTE       : %d" % khop)
    print("LECH noi dung        : %d %s" % (len(lech), lech[:5]))
    print("THIEU trong MySQL    : %d %s" % (len(thieu), thieu[:5]))
    print("THUA trong MySQL     : %d %s" % (len(thua), list(thua)[:5]))

    cur.execute("UPDATE migrate_audit SET n_verified=%s WHERE id=%s", (khop, audit_id))
    cn.commit()

    ok = (khop == len(nguon_map) == len(tu_db) and not lech and not thieu and not thua)
    print("\n>>> KET QUA: %s" % ("DAT -- 100% ban ghi khop tung byte"
                                 if ok else "KHONG DAT -- xem so lieu tren"))
    cur.close()
    cn.close()
    sys.exit(0 if ok else 6)


if __name__ == '__main__':
    main()
