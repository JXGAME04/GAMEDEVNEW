# -*- coding: utf-8 -*-
r"""vhtd_spr_pak.py -- dong goi tai san hinh anh (.spr) Vu Hon / Tieu Dao tu pak client VLTK
(Level Up) sang pak MOI cua client JX1, dang ky package.ini, noi bang trang thai
state_magic_table_name.txt (Status224..Status243).

  python vhtd_spr_pak.py --kiem              chay thu: doc + doi chieu + giai mau, KHONG ghi gi
  python vhtd_spr_pak.py                     thi cong (idempotent: chay lai khong doi ket qua)
  python vhtd_spr_pak.py --ghi-de            cho phep THAY sprvuhontieudao.pak neu da co ma khac noi dung
  python vhtd_spr_pak.py --buoc=pak,ini,state   chi chay cac buoc neu ten (mac dinh ca ba)
  python vhtd_spr_pak.py --mau=5             so tep giai ma ngau nhien de doi chung (mac dinh 5)

Dinh dang pak (D:\GAMEDEVNEW\Sources\Engine\Src\XPackFile.cpp, do lai tren pak that JX1 + VLTK):
  header 32 byte  {'PACK', count, indexOffset, dataOffset=32, crc32=0, 12 byte 0}
  du lieu tu offset 32, xep theo uid TANG
  bang index 16 byte/muc {uid, offset, size, cf} SAP XEP uid tang (FindElemFile tim nhi phan), nam CUOI tep
  cf = (flag<<24) | compressedSize (3 byte thap) ; flag 0x00 RAW, 0x01 UCL, 0x20 UCL-VNG (ExtractRead coi nhu UCL),
  0x10 nen theo frame (chi spr). XPackFile::Open TU CHOI header 16 byte (uIndexTableOffset < sizeof(Header)=32).
uid = KPakList::FileNameToId('\\' + duong dan goc thuong hoa) == pakdump.name2id (char CO DAU).
Moi tep game chi doc/ghi NHI PHAN (khong Edit/Write), ban luu <tep>.truoc_vhtd_0209 chi tao 1 lan.
"""
import os, sys, io, struct, hashlib, random, time, shutil, collections

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools\pak_vltk")
import pakdump as P
import ucl

VLTK_DATA  = r"C:\Users\nguye\Level Up Games\Vo Lam Truyen Ky\data"
PH         = r"D:\GAMEDEVNEW\ReverseTools\phai3\phantich"
VITRI      = os.path.join(PH, "vltk_vitri_tai_san.txt")
IDX_JX1    = os.path.join(PH, "jx1_client_pak_index.tsv")
CLIENT     = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\client"
OUT_NAME   = "sprvuhontieudao.pak"
OUT_PAK    = os.path.join(CLIENT, "data", OUT_NAME)
PKG_INI    = os.path.join(CLIENT, "package.ini")
STATE_JX1  = os.path.join(CLIENT, "settings", "NpcRes", "state_magic_table_name.txt")
STATE_VLTK = os.path.join(r"D:\GAMEDEVNEW\ReverseTools\phai3\vltk_raw",
                          "settings__npcres__" + "\u72b6\u6001\u56fe\u5f62\u5bf9\u7167\u8868.txt")
JX1_REF_PAK = os.path.join(CLIENT, "data", "updatejx16.pak")   # pak JX1 dang chay, de doi chung co nen
SUFFIX     = ".truoc_vhtd_0209"
STATE_FROM, STATE_TO = 224, 243
HERE       = os.path.dirname(os.path.abspath(__file__))
INDEX_OUT  = os.path.join(HERE, "vhtd_spr_pak_index.tsv")
HDR_SIZE   = 32
FL_RAW, FL_UCL, FL_VNG, FL_FRAME = 0x00, 0x01, 0x20, 0x10
SEED       = 20260902


def log(*a):
    print(*a)
    sys.stdout.flush()


# ----------------------------------------------------------------------------- danh sach
def doc_danhsach():
    """vltk_vitri_tai_san.txt -> [(path, pak, uid)] duy nhat theo path; bo '## ' va KHONG-CO-O-DAU."""
    items, seen, uid2path, trung = [], set(), {}, 0
    with open(VITRI, "rb") as f:
        for raw in f:
            line = raw.decode("latin-1").rstrip("\r\n")
            if not line or line.startswith("##"):
                continue
            c = line.split("\t")
            if len(c) < 2:
                continue
            path, pak = c[0].strip(), c[1].strip()
            if not pak or pak == "KHONG-CO-O-DAU":
                continue
            if path in seen:
                trung += 1
                continue
            seen.add(path)
            u = P.name2id(path)
            if u in uid2path and uid2path[u] != path:
                raise SystemExit("TRUNG UID %08X giua %r va %r" % (u, uid2path[u], path))
            uid2path[u] = path
            items.append((path, pak, u))
    return items, uid2path, trung


def doc_idx_jx1():
    """jx1_client_pak_index.tsv -> {uid: [(pak, size, flag)]} (bo dong header lap)."""
    jidx = {}
    with open(IDX_JX1, "rb") as f:
        for raw in f:
            c = raw.decode("latin-1").rstrip("\r\n").split("\t")
            if len(c) < 4 or c[1] == "uid":
                continue
            jidx.setdefault(int(c[1], 16), []).append((c[0], int(c[2]), int(c[3])))
    return jidx


# ----------------------------------------------------------------------------- pak
def mo_pak(p):
    f, es = P.entries(p)
    return f, {e[0]: e for e in es}


def doc_blob(f, e):
    """Doc NGUYEN blob tren dia (dang nen neu co)."""
    uid, off, size, cf = e
    cs = cf & 0xFFFFFF
    n = cs if cs else size
    f.seek(off)
    b = f.read(n)
    if len(b) != n:
        raise IOError("doc thieu %08X: %d/%d" % (uid, len(b), n))
    return b


def giai_nen(blob, e):
    uid, off, size, cf = e
    fl = cf >> 24
    if fl == FL_RAW:
        return blob[:size]
    if fl in (FL_UCL, FL_VNG):
        return ucl.nrv2b_decompress_8(blob, size)
    raise ValueError("flag 0x%02X khong giai duoc" % fl)


def spr_hop_le(d):
    """Kiem SPR theo KSprite.h: 'SPR\\0' + 8 WORD + 6 WORD reserved, palette Colors*3, SPROFFS[Frames], frame."""
    if len(d) < HDR_SIZE:
        return False, "ngan hon SPRHEAD"
    if d[:4] != b"SPR\x00":
        return False, "khong co 'SPR' + 0"
    w, h, cx, cy, frames, colors, dirs, itv = struct.unpack_from("<8H", d, 4)
    base = HDR_SIZE + colors * 3 + frames * 8
    if base > len(d):
        return False, "palette/offset vuot kich thuoc"
    for i in range(frames):
        o, l = struct.unpack_from("<II", d, HDR_SIZE + colors * 3 + i * 8)
        if base + o + l > len(d):
            return False, "frame %d vuot kich thuoc" % i
    return True, "%dx%d frames=%d colors=%d dirs=%d" % (w, h, frames, colors, dirs)


def kiem_tuong_thich_jx1():
    """Doc lai pak JX1 dang chay: pak JX1 co dung co 0x20 (UCL-VNG) nhu pak VLTK khong, giai ra SPR duoc khong."""
    f, es = P.entries(JX1_REF_PAK)
    hist = collections.Counter(e[3] >> 24 for e in es)
    log("  doi chung pak JX1 %s: %d muc, co (flag) = %s" % (os.path.basename(JX1_REF_PAK), len(es),
                                                            dict(("0x%02X" % k, v) for k, v in sorted(hist.items()))))
    ok = False
    thu = 0
    for e in es:
        if (e[3] >> 24) != FL_VNG or e[2] > 300000:
            continue
        d = giai_nen(doc_blob(f, e), e)
        ok, why = spr_hop_le(d)
        thu += 1
        if ok or thu >= 10:
            log("  giai 1 muc co 0x20 cua JX1: uid %08X size=%d -> %s (%s)" % (e[0], e[2], "SPR hop le" if ok else "khong phai SPR", why))
            break
    f.close()
    return set(hist), ok


def lap_kehoach(items):
    """Tim tung muc trong pak VLTK -> [(uid, path, pak, entry, mode)]; mode copy (nguyen blob+cf) | raw (giai nen, flag 0)."""
    theo_pak = collections.defaultdict(list)
    for path, pak, u in items:
        theo_pak[pak].append((u, path))
    plan, thieu, warn = [], [], collections.Counter()
    for pak, lst in sorted(theo_pak.items()):
        p = os.path.join(VLTK_DATA, pak)
        if not os.path.exists(p):
            for u, path in lst:
                thieu.append((path, pak + " (KHONG CO PAK)"))
            continue
        f, idx = mo_pak(p)
        fsz = os.path.getsize(p)
        for u, path in lst:
            e = idx.get(u)
            if e is None:
                thieu.append((path, pak))
                continue
            uid, off, size, cf = e
            fl, cs = cf >> 24, cf & 0xFFFFFF
            if off + (cs if cs else size) > fsz:
                thieu.append((path, pak + " (offset vuot tep)"))
                continue
            if fl in (FL_VNG, FL_UCL):
                mode = "copy"
            elif fl == FL_RAW:
                mode = "copy" if cs == size else "raw"
            elif (fl & FL_FRAME) and (fl & 0x0F) in (0, 1) and not (fl & 0x20):
                mode = "copy"
                warn["frame 0x%02X" % fl] += 1
            else:
                mode = "raw"
                warn["giai nen 0x%02X" % fl] += 1
            plan.append((u, path, pak, e, mode))
        f.close()
    return plan, thieu, warn


def mong_doi(plan_sorted):
    """{uid: (size, cf)} se co trong pak dich."""
    want = {}
    for u, path, pak, e, mode in plan_sorted:
        want[u] = (e[2], e[3]) if mode == "copy" else (e[2], e[2])
    return want


def pak_da_dung(plan_sorted):
    """OUT_PAK ton tai va index trung ke hoach -> True (idempotent: khong xay lai)."""
    if not os.path.exists(OUT_PAK):
        return False
    try:
        f, es = P.entries(OUT_PAK)
        f.close()
    except Exception:
        return False
    have = {e[0]: (e[2], e[3]) for e in es}
    return have == mong_doi(plan_sorted) and [e[0] for e in es] == sorted(have)


def ghi_pak(plan_sorted, out_tmp):
    srcs, index, loi = {}, [], []
    with open(out_tmp, "wb") as w:
        w.write(b"\x00" * HDR_SIZE)
        off = HDR_SIZE
        for u, path, pak, e, mode in plan_sorted:
            if pak not in srcs:
                srcs[pak] = open(os.path.join(VLTK_DATA, pak), "rb")
            blob = doc_blob(srcs[pak], e)
            if mode == "copy":
                size, cf = e[2], e[3]
            else:
                try:
                    blob = giai_nen(blob, e)
                except Exception as ex:
                    loi.append((path, str(ex)))
                    continue
                size = len(blob)
                if size >= 0x1000000:
                    loi.append((path, "RAW >= 16MB khong ghi duoc vao 3 byte cf"))
                    continue
                cf = size                      # flag 0 RAW, csize = size (nhu hs_map_pak.py)
            w.write(blob)
            index.append((u, off, size, cf, path))
            off += len(blob)
        ioff = off
        for u, o, size, cf, path in index:
            w.write(struct.pack("<IIiI", u, o, size, cf))
        w.seek(0)
        w.write(struct.pack("<4sIIII", b"PACK", len(index), ioff, HDR_SIZE, 0) + b"\x00" * 12)
    for f in srcs.values():
        f.close()
    return index, loi


def doi_chung_pak(p, plan_sorted, n_mau, so_nguon=True):
    """Doc lai pak bang pakdump: so muc, sap xep, index, header, so BYTE voi nguon, giai n_mau tep ngau nhien."""
    ok = True
    want = mong_doi(plan_sorted)
    info = {u: (path, pak, e, mode) for u, path, pak, e, mode in plan_sorted}
    f, es = P.entries(p)
    fsz = os.path.getsize(p)
    with open(p, "rb") as h:
        hdr = h.read(HDR_SIZE)
    sig, cnt, ioff, doff, crc = struct.unpack("<4sIIII", hdr[:20])
    log("  header: sig=%r count=%d indexOffset=%d dataOffset=%d crc=%d reserved=%s size=%d" % (
        sig, cnt, ioff, doff, crc, hdr[20:].hex(), fsz))
    if not (sig == b"PACK" and cnt == len(es) and doff == HDR_SIZE and ioff >= HDR_SIZE and ioff + cnt * 16 == fsz):
        log("  LOI header/kich thuoc")
        ok = False
    if len(es) != len(want):
        log("  LOI so muc %d != %d" % (len(es), len(want)))
        ok = False
    uids = [e[0] for e in es]
    if uids != sorted(uids):
        log("  LOI index khong sap xep uid tang")
        ok = False
    lech = sum(1 for e in es if want.get(e[0]) != (e[2], e[3]))
    if lech:
        log("  LOI %d muc (uid,size,cf) khac ke hoach" % lech)
        ok = False
    # so byte tung muc voi nguon (mode copy) -- chi ~2x dung luong I/O
    if so_nguon and ok:
        srcs, khac = {}, 0
        for e in es:
            path, pak, se, mode = info[e[0]]
            if mode != "copy":
                continue
            if pak not in srcs:
                srcs[pak] = open(os.path.join(VLTK_DATA, pak), "rb")
            if doc_blob(f, e) != doc_blob(srcs[pak], se):
                khac += 1
        for s in srcs.values():
            s.close()
        log("  so byte %d muc voi pak nguon: %s" % (len(es), "khop 100%" if khac == 0 else "LOI %d muc khac" % khac))
        if khac:
            ok = False
    # giai ma mau ngau nhien
    rnd = random.Random(SEED)
    for e in rnd.sample(es, min(n_mau, len(es))):
        path = info[e[0]][0]
        t = time.time()
        try:
            d = giai_nen(doc_blob(f, e), e)
            hop, why = spr_hop_le(d)
            hop = hop and len(d) == e[2]
        except Exception as ex:
            hop, why = False, str(ex)
        log("  mau %08X flag=0x%02X size=%-7d %-48s -> %s (%s, %.1fs)" % (
            e[0], e[3] >> 24, e[2], path, "SPR OK" if hop else "LOI", why, time.time() - t))
        if not hop:
            ok = False
    f.close()
    return ok


def md5_tep(p):
    h = hashlib.md5()
    with open(p, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 22), b""):
            h.update(chunk)
    return h.hexdigest()


def ghi_index_tsv(plan_sorted):
    f, es = P.entries(OUT_PAK)
    f.close()
    info = {u: path for u, path, pak, e, mode in plan_sorted}
    with open(INDEX_OUT, "wb") as w:
        w.write(b"uid\toffset\tsize\tflag\tcsize\tpath\r\n")
        for u, o, s, cf in es:
            w.write(("%08X\t%d\t%d\t%d\t%d\t%s\r\n" % (u, o, s, cf >> 24, cf & 0xFFFFFF, info.get(u, "?"))).encode("latin-1"))
    log("  chi muc pak moi: %s (%d dong)" % (INDEX_OUT, len(es)))


def buoc_pak(plan, kiem, ghi_de, n_mau):
    plan_sorted = sorted(plan, key=lambda x: x[0])
    tong_nen = sum(((x[3][3] & 0xFFFFFF) or x[3][2]) for x in plan_sorted)
    tong_goc = sum(x[3][2] for x in plan_sorted)
    modes = collections.Counter(x[4] for x in plan_sorted)
    flags = collections.Counter(x[3][3] >> 24 for x in plan_sorted)
    log("[PAK] %d muc -> %s" % (len(plan_sorted), OUT_PAK))
    log("  co nguon: %s ; cach ghi: %s ; du lieu nen %.1f MB (goc %.1f MB)" % (
        dict(("0x%02X" % k, v) for k, v in sorted(flags.items())), dict(modes), tong_nen / 1048576.0, tong_goc / 1048576.0))
    flags_jx1, ok_ref = kiem_tuong_thich_jx1()
    la = set(flags) - flags_jx1
    if la:
        log("  CANH BAO: co nguon %s khong xuat hien trong pak JX1 doi chung" % ["0x%02X" % x for x in la])
    if not ok_ref:
        log("  CANH BAO: khong giai duoc muc 0x20 cua pak JX1 -> xem lai gia thiet tuong thich")
    if kiem:
        # giai thu n_mau muc NGUON
        rnd = random.Random(SEED)
        srcs = {}
        for u, path, pak, e, mode in rnd.sample(plan_sorted, min(n_mau, len(plan_sorted))):
            if pak not in srcs:
                srcs[pak] = open(os.path.join(VLTK_DATA, pak), "rb")
            t = time.time()
            try:
                d = giai_nen(doc_blob(srcs[pak], e), e)
                hop, why = spr_hop_le(d)
                hop = hop and len(d) == e[2]
            except Exception as ex:
                hop, why = False, str(ex)
            log("  [KIEM] nguon %08X flag=0x%02X size=%-7d %-48s -> %s (%s, %.1fs)" % (
                u, e[3] >> 24, e[2], path, "SPR OK" if hop else "LOI", why, time.time() - t))
        for s in srcs.values():
            s.close()
        log("  [KIEM] khong ghi pak. Dich %s %s" % (OUT_PAK, "DA TON TAI" if os.path.exists(OUT_PAK) else "chua co"))
        return None
    if pak_da_dung(plan_sorted):
        log("  pak dich da ton tai, index trung ke hoach -> KHONG xay lai, chi doi chung")
        ok = doi_chung_pak(OUT_PAK, plan_sorted, n_mau)
        md5 = md5_tep(OUT_PAK)
        log("  md5 %s  size %d  %s" % (md5, os.path.getsize(OUT_PAK), "DOI CHUNG OK" if ok else "DOI CHUNG LOI"))
        if ok:
            ghi_index_tsv(plan_sorted)
        return md5 if ok else None
    tmp = OUT_PAK + ".tmp"
    t = time.time()
    index, loi = ghi_pak(plan_sorted, tmp)
    log("  ghi tam %s: %d muc, %d byte, %.1fs; loi %d" % (tmp, len(index), os.path.getsize(tmp), time.time() - t, len(loi)))
    for path, why in loi:
        log("   LOI GHI:", path, why)
    if loi:
        plan_sorted = [x for x in plan_sorted if x[1] not in set(p for p, _ in loi)]
    ok = doi_chung_pak(tmp, plan_sorted, n_mau)
    if not ok:
        log("  DOI CHUNG LOI -> giu %s de xem, KHONG dua vao data" % tmp)
        return None
    md5 = md5_tep(tmp)
    if os.path.exists(OUT_PAK):
        if md5_tep(OUT_PAK) == md5:
            os.remove(tmp)
            log("  pak dich da co, noi dung y het -> giu nguyen")
        elif ghi_de:
            os.replace(tmp, OUT_PAK)
            log("  DA THAY pak dich (--ghi-de)")
        else:
            log("  pak dich DA CO nhung KHAC noi dung; khong ghi de (them --ghi-de). Ban moi giu o %s" % tmp)
            return None
    else:
        os.rename(tmp, OUT_PAK)
        log("  DA TAO %s" % OUT_PAK)
    log("  md5 %s  size %d byte (%.1f MB)" % (md5, os.path.getsize(OUT_PAK), os.path.getsize(OUT_PAK) / 1048576.0))
    ghi_index_tsv(plan_sorted)
    return md5


# ----------------------------------------------------------------------------- package.ini
def ban_luu(p):
    b = p + SUFFIX
    if os.path.exists(b):
        log("  ban luu da co: %s (giu nguyen)" % b)
    else:
        shutil.copy2(p, b)
        log("  ban luu: %s" % b)


def buoc_ini(kiem):
    raw = open(PKG_INI, "rb").read()
    eol = b"\r\n" if b"\r\n" in raw else b"\n"
    lines = raw.split(eol)
    in_sec, last_num, keys, da_co = False, -1, [], None
    for i, l in enumerate(lines):
        s = l.strip()
        if s.startswith(b"[") and s.endswith(b"]"):
            in_sec = (s.lower() == b"[package]")
            continue
        if not in_sec or b"=" not in s:
            continue
        k, v = s.split(b"=", 1)
        k, v = k.strip(), v.strip()
        if k.isdigit():
            keys.append(int(k))
            last_num = i
            if v.lower() == OUT_NAME.encode("latin-1").lower():
                da_co = (i + 1, s.decode("latin-1"))
    if da_co:
        log("[INI] %s da dang ky o dong %d: %s -> bo qua" % (PKG_INI, da_co[0], da_co[1]))
        return
    if last_num < 0:
        raise SystemExit("[INI] khong thay [Package]/khoa so trong " + PKG_INI)
    mx = max(keys)
    if set(keys) != set(range(mx + 1)):
        log("  CANH BAO: khoa [Package] khong lien tuc 0..%d (thieu %s) -> KPakList::Open dung o khoa thieu" % (
            mx, sorted(set(range(mx + 1)) - set(keys))))
    if mx + 1 >= 64:
        raise SystemExit("[INI] vuot MAX_PAK=64 (KPakList.h)")
    new_line = ("%d=%s" % (mx + 1, OUT_NAME)).encode("latin-1")
    out = lines[:last_num + 1] + [new_line] + lines[last_num + 1:]
    log("[INI] %s: them dong %d sau '%s': %s" % (PKG_INI, last_num + 2, lines[last_num].decode("latin-1"), new_line.decode("latin-1")))
    if kiem:
        log("  [KIEM] khong ghi.")
        return
    ban_luu(PKG_INI)
    with open(PKG_INI, "wb") as w:
        w.write(eol.join(out))
    log("  DA GHI package.ini (%d byte -> %d byte)" % (len(raw), len(eol.join(out))))


# ----------------------------------------------------------------------------- bang trang thai
def buoc_state(kiem, uid_moi, jidx):
    raw = open(STATE_JX1, "rb").read()
    eol = b"\r\n" if b"\r\n" in raw else b"\n"
    rows = [l.rstrip(b"\r") for l in raw.split(b"\n")]
    while rows and rows[-1] == b"":
        rows.pop()
    hdr = rows[0].split(b"\t")
    ncol = len(hdr)
    data = rows[1:]
    for i, r in enumerate(data):
        lab = r.split(b"\t")[0]
        if lab != b"Status%d" % (i + 1):
            raise SystemExit("[STATE] JX1 dong %d nhan %r != Status%d (CStateMagicTable::Init doc theo SO DONG i+2)" % (i + 2, lab, i + 1))
    n_hien = len(data)
    log("[STATE] %s: header %d cot, %d dong Status1..Status%d, EOL %s" % (STATE_JX1, ncol, n_hien, n_hien, "CRLF" if eol == b"\r\n" else "LF"))

    vraw = open(STATE_VLTK, "rb").read()
    vrows = [l.rstrip(b"\r") for l in vraw.split(b"\n")]
    while vrows and vrows[-1] == b"":
        vrows.pop()
    vhdr = vrows[0].split(b"\t")
    if len(vhdr) != ncol + 1:
        raise SystemExit("[STATE] VLTK header %d cot, JX1 %d: khong phai 'them 1 cot nu'" % (len(vhdr), ncol))
    vmap = {}
    for i, r in enumerate(vrows[1:]):
        c = r.split(b"\t")
        if c[0] != b"Status%d" % (i + 1):
            raise SystemExit("[STATE] VLTK dong %d nhan %r lech" % (i + 2, c[0]))
        vmap[i + 1] = c
    log("  VLTK: header %d cot (them cot 3 = nu), %d dong; JX1 cot 2..9 engine doc = VLTK cot 2,4..10 (bo cot 3)" % (len(vhdr), len(vmap)))

    them, ghichu = [], []
    for n in range(STATE_FROM, STATE_TO + 1):
        c = vmap[n]
        if len(c) != ncol + 1:
            raise SystemExit("[STATE] VLTK Status%d co %d cot" % (n, len(c)))
        nam, nu, vitri = c[1], c[2], c[3]
        if nam != nu:
            ghichu.append("Status%d: cot nam != cot nu, lay cot nam" % n)
        if vitri not in (b"Head", b"Foot", b"Body"):
            ghichu.append("Status%d: vi tri %r la -> Body" % (n, vitri))
            vitri = b"Body"
        new = [c[0], nam, vitri] + c[4:]
        assert len(new) == ncol
        them.append(new)
    # spr co trong pak JX1 / pak moi?
    thieu_spr = []
    for new in them:
        path = new[1].decode("latin-1")
        u = P.name2id(path)
        noi = ["JX1:" + ",".join(sorted(set(x[0] for x in jidx[u])))] if u in jidx else []
        if u in uid_moi:
            noi.append("pak moi")
        if not noi:
            thieu_spr.append(new[0].decode())
        log("  %-9s %-5s %-46s uid %08X -> %s" % (new[0].decode(), new[2].decode(), new[1].decode("gbk", "replace"), u,
                                                    " + ".join(noi) if noi else "KHONG PAK NAO CO"))
    for g in ghichu:
        log("  ghi chu:", g)
    if n_hien >= STATE_TO:
        log("  JX1 da co %d dong (>= Status%d) -> bo qua noi" % (n_hien, STATE_TO))
        return thieu_spr
    if n_hien != STATE_FROM - 1:
        raise SystemExit("[STATE] JX1 co %d dong, mong %d de noi Status%d.." % (n_hien, STATE_FROM - 1, STATE_FROM))
    if kiem:
        log("  [KIEM] se noi %d dong Status%d..Status%d (CRLF), khong ghi." % (len(them), STATE_FROM, STATE_TO))
        return thieu_spr
    ban_luu(STATE_JX1)
    body = raw if raw.endswith(eol) else raw + eol
    body += eol.join(b"\t".join(r) for r in them) + eol
    with open(STATE_JX1, "wb") as w:
        w.write(body)
    log("  DA NOI %d dong -> %d dong (Status1..Status%d), %d byte" % (len(them), 1 + n_hien + len(them), STATE_TO, len(body)))
    return thieu_spr


# ----------------------------------------------------------------------------- main
def main():
    args = sys.argv[1:]
    kiem = "--kiem" in args
    ghi_de = "--ghi-de" in args
    buoc = "pak,ini,state"
    n_mau = 5
    for a in args:
        if a.startswith("--buoc="):
            buoc = a.split("=", 1)[1]
        if a.startswith("--mau="):
            n_mau = int(a.split("=", 1)[1])
    t0 = time.time()
    log("=== vhtd_spr_pak %s ===" % ("CHAY THU (--kiem)" if kiem else "THI CONG"))
    items, uid2path, trung = doc_danhsach()
    paks = collections.Counter(pak for _, pak, _ in items)
    log("[DS] %s: %d tep duy nhat (%d dong lap ten), pak nguon %s" % (VITRI, len(items), trung, dict(paks)))
    plan, thieu, warn = lap_kehoach(items)
    log("[NGUON] tim thay %d/%d ; KHONG TIM THAY %d ; canh bao %s" % (len(plan), len(items), len(thieu), dict(warn) or "-"))
    for path, pak in thieu:
        log("   THIEU:", path, "|", pak)
    md5 = None
    if "pak" in buoc:
        md5 = buoc_pak(plan, kiem, ghi_de, n_mau)
    if "ini" in buoc:
        buoc_ini(kiem)
    thieu_spr = []
    if "state" in buoc:
        thieu_spr = buoc_state(kiem, set(x[0] for x in plan), doc_idx_jx1())
    log("=== TOM TAT: %d tep dong goi, %d khong tim thay, md5 pak %s, spr trang thai khong pak nao co: %s, %.0fs ===" % (
        len(plan), len(thieu), md5 or "-", ",".join(thieu_spr) or "-", time.time() - t0))


if __name__ == "__main__":
    main()
