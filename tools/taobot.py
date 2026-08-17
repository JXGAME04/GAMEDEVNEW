#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
taobot.py - tao san TAI KHOAN + NHAN VAT cho bot SimCity / KBotManager.

MAC DINH LA CHAY THU (dry-run): in ra ke hoach, KHONG ghi mot dong nao vao CSDL.
Chi khi them --apply moi that su ghi, va toan bo nam trong MOT giao dich
(loi giua chung -> rollback sach, khong de lai rac).

CACH DUNG
---------
  # 1) xem cac nhan vat co the dung lam MAU (chon 1 cai lam khuon nhan ban)
  python taobot.py --list-mau

  # 2) chay thu: xem no dinh lam gi, khong ghi gi ca
  python taobot.py --mau <ten_tai_khoan_mau>

  # 3) lam that
  python taobot.py --mau <ten_tai_khoan_mau> --apply

  # 4) go sach neu can lam lai
  python taobot.py --undo --apply

THAM SO
-------
  --tu 1 --den 1000      dai tai khoan bot (mac dinh 1..1000)
  --matkhau 1000bot      mat khau chung (luu MD5 - dung dang cua Account_Info)
  --mau <caccname>       tai khoan co nhan vat dung lam khuon nhan ban
  --phai-deu             rai deu mon phai cho bot thay vi giu nguyen phai cua mau
  --apply                THAT SU GHI (khong co thi chi chay thu)
  --undo                 xoa cac tai khoan/nhan vat bot da tao trong dai

VI SAO NHAN BAN THAY VI TAO MOI
-------------------------------
Mot nhan vat hoan chinh khong chi la mot dong RoleBaseInfo - no con keo theo
Items / Skills / Tasks (deu khoa theo RoleID). Tu dung mot nhan vat trong rong se
cho ra bot khong co ky nang, khong co do, va co the ket o buoc nap. Nhan ban tu mot
nhan vat CO THAT bao dam moi bang deu nhat quan. Ca hai he bot tham khao deu lam vay:
  - KBotManager.cpp:1526  "Every bot is cloned from the same role blob"
  - ban goc JX2 kem san thu muc database_backups\\MauCloneAcc\\

AN TOAN
-------
* Truoc khi ghi, tool KIEM va TU CHOI neu bat ky tai khoan dich nao DA TON TAI.
  (Da do truoc: dai 1..1000 hien KHONG co tai khoan nao - nhung van kiem lai moi lan chay.)
* Moi thu nam trong 1 giao dich; loi -> rollback.
* --undo chi xoa nhung tai khoan co ten la SO trong dai va CO nhan vat khop khuon bot,
  khong bao gio dung toi tai khoan nguoi choi that.
"""
import argparse
import hashlib
import io
import os
import random
import sys

try:
    import pyodbc
except ImportError:
    print('THIEU thu vien pyodbc. Cai bang:  pip install pyodbc')
    sys.exit(1)

# ---------------------------------------------------------------- ket noi
INI_MAC_DINH = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\DataBase.ini'


def doc_ini(path):
    cfg, sec = {}, None
    if not os.path.exists(path):
        return cfg
    for ln in io.open(path, encoding='latin-1'):
        ln = ln.strip()
        if ln.startswith('[') and ln.endswith(']'):
            sec = ln[1:-1].lower()
            cfg[sec] = {}
        elif '=' in ln and sec:
            k, v = ln.split('=', 1)
            cfg[sec][k.strip().lower()] = v.strip()
    return cfg


def ket_noi(args):
    """Thu lan luot cac duong vao. DataBase.ini ghi 127.0.0.1:1433 nhung may nay
    chay instance CO TEN (.\\SQLEXPRESS) va KHONG mo TCP 1433, nen phai thu ca hai."""
    cfg = doc_ini(args.ini).get('account', {})
    user = cfg.get('user', '')
    pwd = cfg.get('password', '')
    db = args.db or cfg.get('database', 'account')

    ung_vien = []
    if args.server:
        ung_vien.append(args.server)
    ung_vien += [r'(local)\SQLEXPRESS', r'.\SQLEXPRESS', 'localhost\\SQLEXPRESS',
                 '%s,%s' % (cfg.get('server', '127.0.0.1'), cfg.get('port', '1433'))]

    drivers = ['ODBC Driver 18 for SQL Server', 'ODBC Driver 17 for SQL Server',
               'SQL Server Native Client 11.0', 'SQL Server']
    loi_cuoi = None
    for srv in ung_vien:
        for drv in drivers:
            for cach, extra in (('Windows', 'Trusted_Connection=yes;'),
                                ('SQL', 'UID=%s;PWD=%s;' % (user, pwd))):
                cs = 'DRIVER={%s};SERVER=%s;DATABASE=%s;%s' % (drv, srv, db, extra)
                if '18' in drv:
                    cs += 'Encrypt=no;TrustServerCertificate=yes;'
                try:
                    cn = pyodbc.connect(cs, timeout=5)
                    print('Ket noi: SERVER=%s  DB=%s  (%s auth, %s)' % (srv, db, cach, drv))
                    return cn
                except Exception as e:
                    loi_cuoi = e
    print('KHONG ket noi duoc CSDL. Loi cuoi: %s' % str(loi_cuoi)[:200])
    sys.exit(2)


# ---------------------------------------------------------------- ten nhan vat
HO = ['Tran', 'Le', 'Nguyen', 'Pham', 'Hoang', 'Vo', 'Dang', 'Bui', 'Do', 'Ngo',
      'Duong', 'Ly', 'Ho', 'Truong', 'Dinh', 'Mai', 'Phan', 'Vu', 'Cao', 'Ha']
TEN = ['Phong', 'Vu', 'Long', 'Ha', 'Minh', 'Tuan', 'Khoa', 'Nam', 'Lam', 'Hai',
       'Anh', 'Bao', 'Thang', 'Quan', 'Hung', 'Son', 'Trung', 'Kiet', 'Dat', 'Vinh',
       'Nhat', 'Duy', 'Tai', 'Loc', 'Phuc', 'Thinh', 'Khang', 'Hieu', 'Toan', 'Nghia']


def sinh_ten(i, da_dung):
    """Ten nhan vat bot: <Ho><Ten><so>. Bat buoc DUY NHAT - trung ten se lam
    NpcSet.SearchName tra nham doi tuong (PM, moi bang hoi, theo-sau-theo-ten)."""
    for _ in range(200):
        t = '%s%s%d' % (random.choice(HO), random.choice(TEN), i)
        if t.lower() not in da_dung:
            da_dung.add(t.lower())
            return t
    t = 'Bot%d' % i
    da_dung.add(t.lower())
    return t


# ---------------------------------------------------------------- lam viec
def lay_cot(cur, bang):
    cur.execute("""SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS
                   WHERE TABLE_NAME=? ORDER BY ORDINAL_POSITION""", bang)
    return [r[0] for r in cur.fetchall()]


def cot_identity(cur, bang):
    cur.execute("SELECT name FROM sys.columns WHERE object_id=OBJECT_ID(?) AND is_identity=1", bang)
    return set(r[0] for r in cur.fetchall())


def liet_ke_mau(cur):
    print()
    print('Cac nhan vat co the dung lam MAU (cap cao + du ky nang/do thi bot cang giong that):')
    print('  %-16s %-18s %-6s %-5s %-5s %-6s %-7s %-6s' %
          ('taikhoan', 'ten nhan vat', 'cap', 'phai', 'gtinh', 'so do', 'so ky nang', 'so nv'))
    cur.execute("""
        SELECT TOP 25 r.caccname, r.szName, r.ifightlevel, r.nSect, r.bSex,
               (SELECT COUNT(*) FROM Items  i WHERE i.RoleID = r.RoleID),
               (SELECT COUNT(*) FROM Skills s WHERE s.RoleID = r.RoleID),
               (SELECT COUNT(*) FROM Tasks  t WHERE t.RoleID = r.RoleID)
        FROM RoleBaseInfo r
        ORDER BY r.ifightlevel DESC, r.RoleID
    """)
    for r in cur.fetchall():
        print('  %-16s %-18s %-6s %-5s %-5s %-6s %-7s %-6s'
              % ((r[0] or '')[:15], (r[1] or '')[:17], r[2], r[3], r[4], r[5], r[6], r[7]))
    print()
    print('Chon mot cai roi chay lai voi:  --mau <taikhoan>')


CANH_BAO = """
*** DUNG LAI - TOOL NAY CHUA DUNG DUOC, DANG NHAM NOI LUU DU LIEU ***

Tool viet theo gia dinh "nhan vat nam trong SQL Server (bang RoleBaseInfo)".
Gia dinh do SAI. Da kiem lai tan goc:

  * Goddess (RoleServer) doc BERKELEY DB, khong he dung SQL:
      Goddess/IDBRoleServer.cpp:58   db_table = new ZDBTable("database", "roledb")
      Goddess/db.h:63                "Berkeley DB 18.1.40"
      grep ODBC/SQLConnect trong Goddess = 0 ket qua
  * Tep du lieu that dang song: bin\\multiserver\\database\\roledb (+ roledb.0, log.*)
  * DataBase.ini CHI duoc S3Relay doc (S3Relay/Global.cpp), khong phai Goddess.
  * => CSDL SQL "account" (Account_Info / RoleBaseInfo) la BAN SAO cho relay/web,
       KHONG phai noi Goddess lay du lieu nhan vat.

Neu chay tool nay, no se tao 1000 dong trong SQL ma Goddess KHONG BAO GIO DOC ->
bot van that bai o buoc GetRoleListOfAccount, con CSDL thi da bi ghi them rac.

Them nua: he bot tham khao KHONG dang nhap bang mat khau - BOT_PASSWORD trong
KBotConfig.h khong he duoc dung o bat ky dong ma nao. Bot di thang toi Goddess xin
danh sach nhan vat theo TEN TAI KHOAN. Nen mat khau "1000bot" chi can neu NGUOI muon
dang nhap tay vao cac tai khoan do.

Tool se duoc viet lai cho Berkeley DB. Tam thoi khong chay.
"""


def main():
    print(CANH_BAO)
    if '--toi-hieu-va-van-muon-chay-tren-sql' not in sys.argv:
        sys.exit(9)
    ap = argparse.ArgumentParser(description='Tao san tai khoan + nhan vat cho bot')
    ap.add_argument('--toi-hieu-va-van-muon-chay-tren-sql', action='store_true')
    ap.add_argument('--tu', type=int, default=1)
    ap.add_argument('--den', type=int, default=1000)
    ap.add_argument('--matkhau', default='1000bot')
    ap.add_argument('--mau', default=None, help='caccname cua tai khoan co nhan vat lam khuon')
    ap.add_argument('--phai-deu', action='store_true', help='rai deu mon phai thay vi giu phai cua mau')
    ap.add_argument('--so-phai', type=int, default=10, help='so mon phai (mac dinh 10)')
    ap.add_argument('--apply', action='store_true', help='THAT SU ghi (khong co = chay thu)')
    ap.add_argument('--undo', action='store_true', help='xoa tai khoan/nhan vat bot da tao')
    ap.add_argument('--list-mau', action='store_true')
    ap.add_argument('--ini', default=INI_MAC_DINH)
    ap.add_argument('--server', default=None)
    ap.add_argument('--db', default=None)
    args = ap.parse_args()

    if args.tu < 1 or args.den < args.tu:
        print('Dai tai khoan khong hop le.')
        sys.exit(1)

    cn = ket_noi(args)
    cn.autocommit = False
    cur = cn.cursor()

    if args.list_mau:
        liet_ke_mau(cur)
        cn.close()
        return

    ten_tk = [str(i) for i in range(args.tu, args.den + 1)]
    print()
    print('Dai tai khoan : %s .. %s  (%d tai khoan)' % (ten_tk[0], ten_tk[-1], len(ten_tk)))
    print('Mat khau      : %s  -> luu MD5 %s' % (args.matkhau, hashlib.md5(args.matkhau.encode()).hexdigest()))
    print('Che do        : %s' % ('GHI THAT (--apply)' if args.apply else 'CHAY THU (khong ghi gi)'))

    # ---------------- UNDO ----------------
    if args.undo:
        cur.execute("""SELECT r.RoleID, r.caccname FROM RoleBaseInfo r
                       WHERE r.caccname IN (%s)""" % ','.join('?' * len(ten_tk)), *ten_tk)
        rows = cur.fetchall()
        print('Se xoa %d nhan vat + %d tai khoan trong dai.' % (len(rows), len(ten_tk)))
        if not args.apply:
            print('(chay thu - chua xoa gi. Them --apply de xoa that)')
            cn.close()
            return
        try:
            for rid, acc in rows:
                cur.execute('DELETE FROM Items  WHERE RoleID=?', rid)
                cur.execute('DELETE FROM Skills WHERE RoleID=?', rid)
                cur.execute('DELETE FROM Tasks  WHERE RoleID=?', rid)
            cur.execute("DELETE FROM RoleBaseInfo WHERE caccname IN (%s)" % ','.join('?' * len(ten_tk)), *ten_tk)
            cur.execute("DELETE FROM Account_Habitus WHERE cAccName IN (%s)" % ','.join('?' * len(ten_tk)), *ten_tk)
            cur.execute("DELETE FROM Account_Info WHERE cAccName IN (%s)" % ','.join('?' * len(ten_tk)), *ten_tk)
            cn.commit()
            print('DA XOA XONG.')
        except Exception as e:
            cn.rollback()
            print('LOI -> da rollback, CSDL khong doi: %s' % e)
        cn.close()
        return

    # ---------------- KIEM AN TOAN ----------------
    cur.execute("SELECT cAccName FROM Account_Info WHERE cAccName IN (%s)" % ','.join('?' * len(ten_tk)), *ten_tk)
    trung = [r[0] for r in cur.fetchall()]
    if trung:
        print()
        print('DUNG LAI: %d tai khoan trong dai DA TON TAI: %s%s'
              % (len(trung), ', '.join(trung[:15]), ' ...' if len(trung) > 15 else ''))
        print('Neu day la tai khoan NGUOI CHOI THAT thi TUYET DOI khong duoc de bot dung.')
        print('Hay chon dai khac (--tu/--den), hoac --undo neu day la bot cu can lam lai.')
        cn.close()
        sys.exit(3)
    print('Kiem trung   : OK, khong tai khoan nao trong dai da ton tai.')

    if not args.mau:
        print()
        print('Thieu --mau. Chay "--list-mau" de xem cac nhan vat co the dung lam khuon.')
        cn.close()
        sys.exit(1)

    cur.execute("SELECT RoleID, szName, ifightlevel, nSect, nFirstSect, bSex FROM RoleBaseInfo WHERE caccname=?", args.mau)
    row = cur.fetchone()
    if not row:
        print('Khong tim thay nhan vat cua tai khoan mau %r.' % args.mau)
        cn.close()
        sys.exit(1)
    mau_rid, mau_ten, mau_cap, mau_sect, mau_sect1, mau_sex = row
    cur.execute('SELECT COUNT(*) FROM Items  WHERE RoleID=?', mau_rid); n_item = cur.fetchone()[0]
    cur.execute('SELECT COUNT(*) FROM Skills WHERE RoleID=?', mau_rid); n_skill = cur.fetchone()[0]
    cur.execute('SELECT COUNT(*) FROM Tasks  WHERE RoleID=?', mau_rid); n_task = cur.fetchone()[0]
    print('Nhan vat mau : %s (RoleID=%s) cap %s, phai %s, %d do / %d ky nang / %d nhiem vu'
          % (mau_ten, mau_rid, mau_cap, mau_sect, n_item, n_skill, n_task))
    if args.phai_deu:
        print('Mon phai     : RAI DEU 0..%d' % (args.so_phai - 1))
    else:
        print('Mon phai     : giu nguyen cua mau (%s)' % mau_sect)

    # cot de nhan ban
    rb_cols = lay_cot(cur, 'RoleBaseInfo')
    rb_ident = cot_identity(cur, 'RoleBaseInfo')
    rb_chep = [c for c in rb_cols if c not in rb_ident and c.lower() not in ('caccname', 'szname')]
    it_cols = lay_cot(cur, 'Items')
    sk_cols = lay_cot(cur, 'Skills')
    tk_cols = lay_cot(cur, 'Tasks')

    md5pw = hashlib.md5(args.matkhau.encode()).hexdigest()

    print()
    print('KE HOACH: tao %d tai khoan, moi tai khoan 1 nhan vat nhan ban tu mau'
          % len(ten_tk))
    print('          => %d dong Account_Info, %d dong RoleBaseInfo,'
          % (len(ten_tk), len(ten_tk)))
    print('             ~%d dong Items, ~%d dong Skills, ~%d dong Tasks'
          % (n_item * len(ten_tk), n_skill * len(ten_tk), n_task * len(ten_tk)))
    if not args.apply:
        print()
        print('CHAY THU - khong ghi gi ca. Them --apply de lam that.')
        cn.close()
        return

    # ---------------- GHI THAT ----------------
    da_dung = set()
    cur.execute("SELECT szName FROM RoleBaseInfo WHERE szName IS NOT NULL")
    for (t,) in cur.fetchall():
        da_dung.add((t or '').lower())

    try:
        for k, acc in enumerate(ten_tk):
            ten_nv = sinh_ten(args.tu + k, da_dung)

            cur.execute("""INSERT INTO Account_Info (cAccName, cPassWord, cRealName, dRegDate)
                           VALUES (?, ?, ?, GETDATE())""", acc, md5pw, 'BOT')

            # nhan ban RoleBaseInfo
            cur.execute("INSERT INTO RoleBaseInfo (caccname, szName, %s) SELECT ?, ?, %s FROM RoleBaseInfo WHERE RoleID=?"
                        % (', '.join('[%s]' % c for c in rb_chep),
                           ', '.join('[%s]' % c for c in rb_chep)),
                        acc, ten_nv, mau_rid)
            cur.execute('SELECT RoleID FROM RoleBaseInfo WHERE caccname=?', acc)
            rid_moi = cur.fetchone()[0]

            if args.phai_deu:
                ph = (args.tu + k) % args.so_phai
                cur.execute('UPDATE RoleBaseInfo SET nSect=?, nFirstSect=? WHERE RoleID=?', ph, ph, rid_moi)

            for bang, cols in (('Items', it_cols), ('Skills', sk_cols), ('Tasks', tk_cols)):
                khac = [c for c in cols if c != 'RoleID']
                cur.execute('INSERT INTO %s (RoleID, %s) SELECT ?, %s FROM %s WHERE RoleID=?'
                            % (bang, ', '.join('[%s]' % c for c in khac),
                               ', '.join('[%s]' % c for c in khac), bang),
                            rid_moi, mau_rid)

            if (k + 1) % 100 == 0:
                print('   ... da tao %d/%d' % (k + 1, len(ten_tk)))

        cn.commit()
        print()
        print('XONG. Da tao %d tai khoan bot (mat khau %r).' % (len(ten_tk), args.matkhau))
        print('Muon lam lai tu dau:  python taobot.py --tu %d --den %d --undo --apply'
              % (args.tu, args.den))
    except Exception as e:
        cn.rollback()
        print()
        print('LOI -> DA ROLLBACK, CSDL khong thay doi mot dong nao:')
        print('   %s' % e)
        sys.exit(4)
    finally:
        cn.close()


if __name__ == '__main__':
    main()
