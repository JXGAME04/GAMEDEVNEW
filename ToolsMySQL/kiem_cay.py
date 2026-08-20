# -*- coding: ascii -*-
"""kiem_cay.py -- PHAN BIEN bo doc jx_bdb: duyet BTREE tu goc (root) thay vi quet
trang tuan tu, roi doi chieu hai tap ket qua.

Ly do: quet tuan tu moi trang P_LBTREE co the nhat phai trang la DA BI BO khoi cay
(vd trang cu chua duoc dua vao danh sach free). Duyet tu goc la nguon su that.

Bo cuc BTMETA (db_page.h): root page nam tai offset 88 cua trang meta.
Trang noi P_IBTREE chua BINTERNAL:
    00-01 db_indx_t len
    02    u_int8_t  type
    03    u_int8_t  unused
    04-07 db_pgno_t pgno
    08-11 db_recno_t nrecs
    12..  data[len]
"""
import struct
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from jx_bdb import BdbFile, PAGE_HDR, P_LBTREE, P_IBTREE   # noqa: E402


def walk_tree(f):
    """Duyet BTREE tu goc, tra ve (danh_sach_cap, tap_trang_la_trong_cay)."""
    e = f.endian
    root = struct.unpack_from(e + 'I', f.raw, 88)[0]
    key_count = struct.unpack_from(e + 'I', f.raw, 40)[0]
    record_count = struct.unpack_from(e + 'I', f.raw, 44)[0]

    out = []
    leaf_pages = []
    seen = set()
    stack = [root]
    while stack:
        pgno = stack.pop()
        if pgno in seen or pgno >= f.npages:
            continue
        seen.add(pgno)
        buf = f.page(pgno)
        h = f.page_hdr(buf)
        if h['type'] == P_IBTREE:
            n = h['entries']
            offs = struct.unpack_from(e + '%dH' % n, buf, PAGE_HDR)
            for o in offs:
                child = struct.unpack_from(e + 'I', buf, o + 4)[0]
                stack.append(child)
        elif h['type'] == P_LBTREE:
            leaf_pages.append(pgno)
            n = h['entries']
            offs = struct.unpack_from(e + '%dH' % n, buf, PAGE_HDR)
            for i in range(0, n, 2):
                kk, kv = f._item(buf, offs[i])
                dk, dv = f._item(buf, offs[i + 1])
                if kk == 'data' and dk == 'data':
                    out.append((kv, dv))
    return out, set(leaf_pages), root, key_count, record_count


def main():
    path = sys.argv[1]
    f = BdbFile(path)
    tree, tree_leaves, root, kc, rc = walk_tree(f)

    scan = [(k, v) for k, v, _p, _s in f.items()]
    scan_leaves = set()
    for pgno in range(f.npages):
        if f.page(pgno)[25] == P_LBTREE:
            scan_leaves.add(pgno)

    print("=" * 74)
    print("PHAN BIEN BO DOC: %s" % path)
    print("=" * 74)
    print("root page                     = %d" % root)
    print("meta key_count / record_count = %d / %d  (co the la so cu, chi tham khao)"
          % (kc, rc))
    print("trang la QUET tuan tu         = %d" % len(scan_leaves))
    print("trang la TRONG CAY            = %d" % len(tree_leaves))
    orphan = scan_leaves - tree_leaves
    print("trang la MO COI (quet thay, cay khong co) = %d %s"
          % (len(orphan), sorted(orphan)[:10]))
    missing = tree_leaves - scan_leaves
    print("trang la cay co ma quet sot   = %d %s" % (len(missing), sorted(missing)[:10]))
    print()
    print("cap (key,value) QUET  = %d" % len(scan))
    print("cap (key,value) CAY   = %d" % len(tree))

    dt = {}
    dup_tree = 0
    for k, v in tree:
        if k in dt:
            dup_tree += 1
        dt[k] = v
    ds = {}
    dup_scan = 0
    for k, v in scan:
        if k in ds:
            dup_scan += 1
        ds[k] = v
    print("khoa TRUNG trong cay  = %d" % dup_tree)
    print("khoa TRUNG khi quet   = %d" % dup_scan)

    only_scan = set(ds) - set(dt)
    only_tree = set(dt) - set(ds)
    print()
    print("khoa CHI CO khi quet (KHONG thuoc cay -> phai LOAI) = %d" % len(only_scan))
    for k in sorted(only_scan)[:15]:
        print("     %r  len=%d" % (k[:24], len(ds[k])))
    print("khoa CHI CO trong cay (quet sot) = %d" % len(only_tree))
    for k in sorted(only_tree)[:15]:
        print("     %r  len=%d" % (k[:24], len(dt[k])))

    diff = [k for k in (set(dt) & set(ds)) if dt[k] != ds[k]]
    print("khoa co O CA HAI nhung VALUE KHAC NHAU = %d %s" % (len(diff), diff[:5]))
    print()
    print(">>> KET LUAN: nguon su that la CAY (%d ban ghi)." % len(dt))


if __name__ == '__main__':
    main()
