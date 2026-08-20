# -*- coding: ascii -*-
"""xuat_ref.py -- Xuat kho Berkeley DB ra tep tham chieu nhi phan cho bo kiem thu C++.

Khuon: [4B keysize][key][4B datasize][data] lap lai. Khoa GIU NGUYEN byte NUL cuoi
nhu Berkeley DB (strlen+1) de bo kiem thu doi chieu dung quy uoc that.

Chay: python xuat_ref.py <roledb> <ref_roledb.bin>
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from jx_bdb import BdbFile     # noqa: E402
import jx_role as R            # noqa: E402


def main():
    src, dst = sys.argv[1], sys.argv[2]
    f = BdbFile(src)
    n = bad = 0
    with open(dst, 'wb') as o:
        for k, v, _p, _s in f.items():
            try:
                r = R.parse(v, strict=True)
            except Exception:
                bad += 1
                continue
            if k != r['role_name'] + b'\x00':
                bad += 1
                continue
            o.write(struct.pack('<I', len(k)))
            o.write(k)
            o.write(struct.pack('<I', len(v)))
            o.write(v)
            n += 1
    print("da xuat %d ban ghi ra %s (%d byte), bo qua %d ban ghi rac"
          % (n, dst, os.path.getsize(dst), bad))


if __name__ == '__main__':
    main()
