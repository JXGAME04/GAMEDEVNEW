# -*- coding: ascii -*-
"""bat_binlog.py -- Them khoi cau hinh binlog vao cuoi my.ini (section [mysqld]).

An toan:
  - Doc/ghi bang latin-1 (anh xa 1:1 byte, khong lam hong byte nao).
  - Neu da co dau moc thi KHONG them lan hai (chay lai duoc nhieu lan).
  - Da co ban sao my.ini.goc trong thu muc backup truoc khi goi script nay.

KHONG dong toi sql-mode: cot Account_Habitus.iClientID la varchar(0); dat
STRICT_ALL_TABLES toan cuc co the lam HONG DUONG DANG NHAP.
"""
import io
import sys

INI = r"C:\ProgramData\MySQL\MySQL Server 5.7\my.ini"
MOC = "# === JX1 MYSQL MIGRATION 20/08/2026"

KHOI = """
# === JX1 MYSQL MIGRATION 20/08/2026 -- bat binlog de khoi phuc toi tung giay ===
# Ly do: truoc day khong co bat ky duong khoi phuc theo thoi diem nao.
# Binlog dat tren o D (3,4 TB trong); datadir nam tren C chi con ~18 GB.
# binlog_row_image=MINIMAL: khong ghi anh TRUOC cua blob -> giam ~mot nua khoi luong.
log-bin="D:/mysql_binlog/mysql-bin"
binlog_format=ROW
binlog_row_image=MINIMAL
max_binlog_size=256M
expire_logs_days=7
sync_binlog=1
# blob nhan vat lon nhat do duoc 70.646 byte; 16M du rong cho nhap theo lo
max_allowed_packet=16M
"""


def main():
    raw = io.open(INI, encoding='latin-1', newline='').read()
    if MOC in raw:
        print("my.ini DA co khoi cau hinh nay roi -- khong them lai.")
        return 0
    if '[mysqld]' not in raw:
        print("!! khong thay [mysqld] trong my.ini -- dung lai.")
        return 2
    # [mysqld] la section CUOI CUNG (da kiem: cac section o dong 55/63/75, tep 335 dong)
    sau = raw.rfind('[mysqld]')
    for s in ('[client]', '[mysql]', '[mysqldump]', '[mysqladmin]', '[isamchk]'):
        if raw.rfind(s) > sau:
            print("!! [mysqld] KHONG phai section cuoi -- phai chen tay, dung lai.")
            return 3
    if not raw.endswith('\r\n') and not raw.endswith('\n'):
        raw += '\r\n'
    raw += KHOI.replace('\n', '\r\n')
    io.open(INI, 'w', encoding='latin-1', newline='').write(raw)
    print("da them khoi binlog vao cuoi my.ini")
    return 0


if __name__ == '__main__':
    sys.exit(main())
