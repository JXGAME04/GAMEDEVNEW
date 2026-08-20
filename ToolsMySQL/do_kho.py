# -*- coding: ascii -*-
"""do_kho.py -- Do dac mot kho roledb (BDB) hoac mot file .bak, in bang so lieu.

Dung de:
  1) Lay so lieu that truoc khi thiet ke bang MySQL (kich thuoc, va cham ten, CRC).
  2) Doi chieu cheo BDB <-> .bak.

Chay:  python do_kho.py bdb <duong_dan_roledb>
       python do_kho.py bak <duong_dan_file.bak>
"""
import sys
import os
import collections

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from jx_bdb import BdbFile          # noqa: E402
import jx_role                      # noqa: E402


def load(kind, path):
    if kind == 'bdb':
        f = BdbFile(path)
        recs = [(k, v) for k, v, _p, _s in f.items()]
        return recs, []
    good, rej = jx_role.read_bak(path)
    return good, rej


def main():
    kind, path = sys.argv[1], sys.argv[2]
    recs, rej = load(kind, path)
    print("=" * 74)
    print("KHO: %s  (%s)" % (path, kind))
    print("kich thuoc file: %d byte" % os.path.getsize(path))
    print("=" * 74)
    if rej:
        print("!! %d diem bi tu choi khi doc .bak (rac dau/cuoi file):" % len(rej))
        for off, why in rej[:6]:
            print("     offset %-10d %s" % (off, why))
        if len(rej) > 6:
            print("     ... con %d diem nua" % (len(rej) - 6))
        print()

    n = len(recs)
    sizes = sorted(len(v) for _k, v in recs)
    total = sum(sizes)
    accs = collections.Counter()
    crc_bad = 0
    hi_names = 0
    parse_err = []
    ci = collections.defaultdict(list)
    per_acc_roles = collections.defaultdict(list)
    max_task = 0
    max_item = 0
    over_255_task = 0
    for k, v in recs:
        try:
            r = jx_role.parse(v, strict=True)
        except Exception as e:
            parse_err.append((k, str(e)))
            continue
        accs[r['acc_name'].lower()] += 1
        per_acc_roles[r['acc_name'].lower()].append(r['role_name'])
        if not r['crc_ok']:
            crc_bad += 1
        if any(b >= 0x80 for b in r['role_name']):
            hi_names += 1
        ci[r['role_name'].lower()].append(r['role_name'])
        max_task = max(max_task, r['n_task'])
        max_item = max(max_item, r['n_item'])
        if r['n_task_derived'] > 255:
            over_255_task += 1

    def pct(x):
        return 100.0 * x / n if n else 0.0

    print("So nhan vat hop le           : %d" % n)
    print("Loi giai ma blob             : %d %s" % (len(parse_err), parse_err[:3]))
    print("So tai khoan (ha chu thuong) : %d" % len(accs))
    dist = collections.Counter(accs.values())
    print("Phan bo nv/tai khoan         : %s"
          % ", ".join("%d nv: %d" % (k, dist[k]) for k in sorted(dist)))
    if sizes:
        print("Kich thuoc blob              : min=%d trungvi=%d p95=%d max=%d tb=%.0f"
              % (sizes[0], sizes[n // 2], sizes[int(n * 0.95)], sizes[-1], total / float(n)))
    print("Tong du lieu                 : %d byte (%.2f MB)" % (total, total / 1048576.0))
    print("Ten chua byte >=0x80 (TCVN3) : %d (%.1f%%)" % (hi_names, pct(hi_names)))
    print("CRC32 SAI                    : %d (%.1f%%)  <== CAM loc theo CRC khi nhap"
          % (crc_bad, pct(crc_bad)))
    print("So bien nhiem vu toi da      : %d (vuot 255: %d ban ghi)" % (max_task, over_255_task))
    print("So vat pham toi da           : %d" % max_item)

    coll = {k: v for k, v in ci.items() if len(v) > 1}
    lost = sum(len(v) - 1 for v in coll.values())
    print()
    print("VA CHAM TEN khi bo phan biet HOA/thuong: %d nhom -> MAT %d ban ghi"
          % (len(coll), lost))
    for k, v in list(coll.items())[:8]:
        print("     %s" % " / ".join(x.decode('latin-1') for x in v))
    if len(coll) > 8:
        print("     ... con %d nhom nua" % (len(coll) - 8))

    over3 = {a: r for a, r in per_acc_roles.items() if len(r) > 3}
    print()
    print("Tai khoan co >3 nhan vat     : %d %s"
          % (len(over3), list(over3.items())[:3]))
    big = [ln for ln in sizes if ln > 65535]
    print("Blob > 65535 byte (can MEDIUMBLOB): %d" % len(big))
    print("Blob >= 327680 (tran cung)        : %d" % len([x for x in sizes if x >= 327680]))


if __name__ == '__main__':
    main()
