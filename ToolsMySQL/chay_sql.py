# -*- coding: utf-8 -*-
"""chay_sql.py -- chay mot tep .sql qua pymysql (khong can mysql.exe trong PATH).

Tach lenh theo dau ';' o cuoi dong, ton trong lenh USE. Chi dung cho cac tep
schema cua bo cong cu nay (khong co trigger/procedure nen khong can DELIMITER).
"""
import io
import sys

import pymysql


def main():
    if len(sys.argv) < 2:
        print('dung: python chay_sql.py <tep.sql> [host] [user] [matkhau]')
        sys.exit(1)
    p = sys.argv[1]
    host = sys.argv[2] if len(sys.argv) > 2 else '127.0.0.1'
    user = sys.argv[3] if len(sys.argv) > 3 else 'root'
    pw = sys.argv[4] if len(sys.argv) > 4 else '123456'

    s = io.open(p, encoding='utf-8').read()
    # bo dong chu thich '--'
    dong = []
    for l in s.split('\n'):
        t = l.strip()
        if t.startswith('--'):
            continue
        dong.append(l)
    s = '\n'.join(dong)

    lenh = [x.strip() for x in s.split(';')]
    lenh = [x for x in lenh if x]

    cn = pymysql.connect(host=host, port=3306, user=user, password=pw,
                         charset='utf8mb4', autocommit=True)
    c = cn.cursor()
    print('chay %d lenh tu %s' % (len(lenh), p))
    for i, q in enumerate(lenh, 1):
        dau = ' '.join(q.split())[:70]
        try:
            c.execute(q)
            print('  [%2d/%d] OK  %s' % (i, len(lenh), dau))
        except Exception as e:
            print('  [%2d/%d] !! %s' % (i, len(lenh), dau))
            print('         LOI: %s' % e)
            cn.close()
            sys.exit(2)
    cn.close()
    print('xong.')


if __name__ == '__main__':
    main()
