# -*- coding: ascii -*-
"""
jx_bdb.py -- Doc thang file Berkeley DB BTREE (db 18.1, pagesize bat ky) bang Python
             thuan, KHONG can thu vien libdb, KHONG mo moi truong, KHONG ghi mot byte.

Muc dich: rut toan bo cap (key, value) tu ban SAO LANH cua `roledb` / `friend` /
`TongTable` ... de nap len MySQL, va de doi chieu cheo voi file .bak cua Goddess.

Tham chieu bo cuc trang: Sources/db-18.1.40/src/dbinc/db_page.h
    PAGE header (26 byte):
        00-07 DB_LSN lsn
        08-11 db_pgno_t pgno
        12-15 db_pgno_t prev_pgno
        16-19 db_pgno_t next_pgno
        20-21 db_indx_t entries
        22-23 db_indx_t hf_offset
        24    u_int8_t  level
        25    u_int8_t  type
    Sau do la mang chi muc inp[entries], moi phan tu 2 byte = offset trong trang.

    BKEYDATA: 00-01 len, 02 type, 03.. data
    BOVERFLOW: 00-01 unused, 02 type, 03 unused, 04-07 pgno, 08-11 tlen
    Trang P_OVERFLOW: du lieu bat dau tai offset 26, dai = hf_offset, noi tiep qua next_pgno.

CANH BAO: mo file o che do 'rb'. Neu tien trinh Goddess dang chay, anh chup se lem
          -> luon chay tren ban sao lanh, va dung is_locked() de kiem tra truoc.
"""

import struct
import os

# --- ma trang ---
P_INVALID = 0
P_IBTREE = 3
P_LBTREE = 5
P_OVERFLOW = 7
P_BTREEMETA = 9

# --- ma phan tu ---
B_KEYDATA = 1
B_DUPLICATE = 2
B_OVERFLOW = 3
B_BLOB = 4

BTREE_MAGIC = 0x00053162
HASH_MAGIC = 0x00061561

PAGE_HDR = 26


class BdbError(Exception):
    pass


class BdbFile(object):
    """Bo doc chi-doc cho mot file BTREE Berkeley DB."""

    def __init__(self, path):
        self.path = path
        self.raw = open(path, 'rb').read()
        self.size = len(self.raw)
        if self.size < 512:
            raise BdbError("file qua nho: %d byte" % self.size)
        self._read_meta()

    # ---------------- metadata ----------------
    def _read_meta(self):
        # thu little-endian truoc, roi big-endian
        for endian in ('<', '>'):
            magic = struct.unpack_from(endian + 'I', self.raw, 12)[0]
            if magic in (BTREE_MAGIC, HASH_MAGIC):
                self.endian = endian
                self.magic = magic
                break
        else:
            raise BdbError("khong nhan ra magic Berkeley DB (doc duoc 0x%08X)"
                           % struct.unpack_from('<I', self.raw, 12)[0])
        e = self.endian
        self.version = struct.unpack_from(e + 'I', self.raw, 16)[0]
        self.pagesize = struct.unpack_from(e + 'I', self.raw, 20)[0]
        self.encrypt_alg = self.raw[24]
        self.meta_type = self.raw[25]
        self.free_pgno = struct.unpack_from(e + 'I', self.raw, 28)[0]
        self.last_pgno = struct.unpack_from(e + 'I', self.raw, 32)[0]
        if self.magic != BTREE_MAGIC:
            raise BdbError("chi ho tro BTREE, file nay magic=0x%08X" % self.magic)
        if self.encrypt_alg != 0:
            raise BdbError("file duoc ma hoa (encrypt_alg=%d) - khong doc duoc"
                           % self.encrypt_alg)
        if self.pagesize == 0 or (self.size % self.pagesize) != 0:
            raise BdbError("pagesize=%d khong chia het kich thuoc file %d"
                           % (self.pagesize, self.size))
        self.npages = self.size // self.pagesize

    # ---------------- trang ----------------
    def page(self, pgno):
        if pgno >= self.npages:
            raise BdbError("pgno %d vuot qua so trang %d" % (pgno, self.npages))
        off = pgno * self.pagesize
        return self.raw[off:off + self.pagesize]

    def page_hdr(self, buf):
        e = self.endian
        pgno, prev_pgno, next_pgno = struct.unpack_from(e + 'III', buf, 8)
        entries, hf_offset = struct.unpack_from(e + 'HH', buf, 20)
        level = buf[24]
        ptype = buf[25]
        return dict(pgno=pgno, prev=prev_pgno, next=next_pgno,
                    entries=entries, hf_offset=hf_offset,
                    level=level, type=ptype)

    # ---------------- overflow ----------------
    def _read_overflow(self, start_pgno, tlen):
        out = bytearray()
        pgno = start_pgno
        guard = 0
        while pgno != 0 and pgno < self.npages:
            guard += 1
            if guard > self.npages + 4:
                raise BdbError("chuoi trang overflow lap vo tan tai pgno=%d" % start_pgno)
            buf = self.page(pgno)
            h = self.page_hdr(buf)
            if h['type'] != P_OVERFLOW:
                raise BdbError("trang %d khong phai P_OVERFLOW (type=%d)"
                               % (pgno, h['type']))
            # OV_LEN(p) == hf_offset : so byte du lieu nam tren chinh trang nay
            n = h['hf_offset']
            if n > self.pagesize - PAGE_HDR:
                raise BdbError("OV_LEN=%d vuot kich thuoc trang" % n)
            out += buf[PAGE_HDR:PAGE_HDR + n]
            pgno = h['next']
            if len(out) >= tlen:
                break
        if len(out) != tlen:
            raise BdbError("overflow doc duoc %d byte, tlen bao %d" % (len(out), tlen))
        return bytes(out)

    # ---------------- phan tu ----------------
    def _item(self, buf, off):
        """Tra ve (kind, payload). kind in {'data','dup','blob'}"""
        e = self.endian
        itype = buf[off + 2]
        if itype == B_KEYDATA:
            ln = struct.unpack_from(e + 'H', buf, off)[0]
            return 'data', buf[off + 3:off + 3 + ln]
        if itype == B_OVERFLOW:
            pgno, tlen = struct.unpack_from(e + 'II', buf, off + 4)
            return 'data', self._read_overflow(pgno, tlen)
        if itype == B_DUPLICATE:
            pgno, tlen = struct.unpack_from(e + 'II', buf, off + 4)
            return 'dup', (pgno, tlen)
        if itype == B_BLOB:
            return 'blob', None
        raise BdbError("ma phan tu la: %d tai offset %d" % (itype, off))

    # ---------------- duyet ----------------
    def items(self, strict=True):
        """Sinh (key_bytes, value_bytes, pgno, slot).

        Duyet MOI trang la (P_LBTREE) theo so trang tang dan. Khong theo con tro
        next_pgno vi trang la co the dut chuoi neu file lem; quet tuan tu an toan hon
        va van cho dung tap ban ghi (khoa trung se bi phat hien o tang goi).
        """
        for pgno in range(self.npages):
            buf = self.page(pgno)
            h = self.page_hdr(buf)
            if h['type'] != P_LBTREE:
                continue
            n = h['entries']
            if n % 2 != 0:
                if strict:
                    raise BdbError("trang %d co entries le = %d" % (pgno, n))
                continue
            inp_end = PAGE_HDR + n * 2
            if inp_end > self.pagesize:
                if strict:
                    raise BdbError("trang %d: mang inp tran trang" % pgno)
                continue
            offs = struct.unpack_from(self.endian + '%dH' % n, buf, PAGE_HDR)
            for i in range(0, n, 2):
                ko, do = offs[i], offs[i + 1]
                if ko + 3 > self.pagesize or do + 3 > self.pagesize:
                    if strict:
                        raise BdbError("trang %d slot %d: offset ra ngoai trang"
                                       % (pgno, i))
                    continue
                kkind, kval = self._item(buf, ko)
                dkind, dval = self._item(buf, do)
                if kkind != 'data' or dkind != 'data':
                    # khoa phu DB_DUP dung B_DUPLICATE -> tang goi tu xu ly
                    yield (kval if kkind == 'data' else None,
                           dval if dkind == 'data' else None,
                           pgno, i // 2)
                    continue
                yield kval, dval, pgno, i // 2

    def stats(self):
        c = {}
        for pgno in range(self.npages):
            t = self.page(pgno)[25]
            c[t] = c.get(t, 0) + 1
        return c


def is_locked(path):
    """True neu file dang bi tien trinh khac giu (server con chay)."""
    try:
        fh = os.open(path, os.O_RDWR)
        os.close(fh)
        return False
    except OSError:
        return True


if __name__ == '__main__':
    import sys
    f = BdbFile(sys.argv[1])
    print("pagesize=%d version=%d endian=%s trang=%d last_pgno=%d"
          % (f.pagesize, f.version, f.endian, f.npages, f.last_pgno))
    names = {0: 'INVALID', 3: 'IBTREE', 5: 'LBTREE', 7: 'OVERFLOW', 9: 'BTREEMETA'}
    for t, c in sorted(f.stats().items()):
        print("   trang loai %-10s (%d): %d" % (names.get(t, '?'), t, c))
    n = 0
    total = 0
    mn, mx = 1 << 30, 0
    for k, v, pg, slot in f.items():
        n += 1
        total += len(v)
        mn = min(mn, len(v))
        mx = max(mx, len(v))
        if n <= 3:
            print("   vd: key=%r len(value)=%d" % (k[:40], len(v)))
    print("ban ghi=%d tong byte=%d min=%d max=%d" % (n, total, mn, mx))
