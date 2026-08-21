# -*- coding: utf-8 -*-
"""Doc lap voi tool C: kiem tra pak sau repack_typeframe.

- header/count/index: uId + lSize tung muc phai GIU NGUYEN, dung thu tu.
- muc copy (flag giu nguyen): blob phai GIONG TUNG BYTE ban goc.
- muc da chuyen (0xNN -> 0x11): kiem cau truc TYPE_FRAME bang so hoc:
    SPRHEAD magic 'SPR\\0', Colors/Frames khop ban goc (giai nen? khong can -
    doc truc tiep tu blob moi vi header luu THO), va
    32 + Colors*3 + Frames*8 + sum(lCompressSize) == (flag & 0xFFFFFF)
    sum(|lSize_i|) == lSize goc - (32 + Colors*3 + Frames*8)
  (noi dung tung khung da duoc tool C round-trip verify khi chuyen)

Usage: python verify_pak.py <old.pak> <new.pak>
"""
import struct, sys

def read_pak(path):
    f = open(path, 'rb')
    hdr = f.read(32)
    sig, count, idx_off, data_off = struct.unpack_from('<4sIII', hdr, 0)
    assert sig == b'PACK', 'bad signature in %s' % path
    f.seek(idx_off)
    raw = f.read(count * 16)
    assert len(raw) == count * 16, 'short index in %s' % path
    idx = [struct.unpack_from('<IIiI', raw, i * 16) for i in range(count)]
    return f, count, idx

def main(old_path, new_path):
    fo, n_old, idx_old = read_pak(old_path)
    fn, n_new, idx_new = read_pak(new_path)
    assert n_old == n_new, 'entry count differs: %d vs %d' % (n_old, n_new)

    n_same = n_conv = 0
    for i in range(n_old):
        oid, ooff, osz, oflag = idx_old[i]
        nid, noff, nsz, nflag = idx_new[i]
        assert oid == nid, 'entry %d: id changed %08x -> %08x' % (i, oid, nid)
        assert osz == nsz, 'entry %d: lSize changed %d -> %d' % (i, osz, nsz)
        ocomp, ometh = oflag & 0xFFFFFF, oflag >> 24
        ncomp, nmeth = nflag & 0xFFFFFF, nflag >> 24
        if oflag == nflag or (ometh == nmeth and ocomp == ncomp):
            fo.seek(ooff); fn.seek(noff)
            CHUNK = 4 << 20
            left = ocomp
            while left:
                take = min(left, CHUNK)
                a = fo.read(take); b = fn.read(take)
                assert a == b, 'entry %d: copied blob differs at ~%d' % (i, ocomp - left)
                left -= take
            n_same += 1
        else:
            assert ometh in (0x01, 0x20) and nmeth == 0x11, \
                'entry %d: unexpected method change %02x -> %02x' % (i, ometh, nmeth)
            fn.seek(noff)
            blob = fn.read(ncomp)
            assert len(blob) == ncomp, 'entry %d: short new blob' % i
            assert blob[:3] == b'SPR' and blob[3:4] == b'\x00', 'entry %d: no SPR magic' % i
            frames, colors = struct.unpack_from('<HH', blob, 12)
            lead = 32 + colors * 3
            base = lead + frames * 8
            assert base <= ncomp, 'entry %d: tables past blob end' % i
            total_comp = base
            total_raw = base  # SPROFFS table in the original occupies the same 8*frames
            for fidx in range(frames):
                csz, rsz = struct.unpack_from('<ii', blob, lead + fidx * 8)
                assert csz >= 0, 'entry %d frame %d: negative lCompressSize' % (i, fidx)
                total_comp += csz
                total_raw += abs(rsz)
                if rsz < 0:
                    assert csz == -rsz, 'entry %d frame %d: raw frame size mismatch' % (i, fidx)
            assert total_comp == ncomp, \
                'entry %d: frame sizes sum %d != blob %d' % (i, total_comp, ncomp)
            assert total_raw == osz, \
                'entry %d: decompressed sum %d != original lSize %d' % (i, total_raw, osz)
            n_conv += 1
    print('OK: %d entries (%d byte-identical copies, %d frame-mode conversions, all invariants hold)'
          % (n_old, n_same, n_conv))

if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
