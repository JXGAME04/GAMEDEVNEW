# -*- coding: utf-8 -*-
r"""[ANDROID 12/09 GOI] Dong goi du lieu cho DIEN THOAI: tu D:\jx1_android_data (da rut gon tinh) -> D:\jx1_android_data_dt

  * Gom TAT CA muc du lieu ma game co the mo thanh MOT danh sach phang, dung dung thu tu tim cua game:
      1. tep ROI trong spr / maps / maps2 / settings / script / ui  (tep roi thang pak; id = FileNameToId cua '\spr\...')
      2. cac pak cu theo thu tu package.ini                        (pak dung truoc thang pak dung sau)
    -> cung id chi giu ban DAU TIEN (ban game thuc su thay), cac ban bi che bo di khong mat gi.
  * Ghi lai thanh cac pak 'mobile_NN.pak' <= --co-goi MB (mac dinh 512): tep to, it tep -> tai nhanh, 4 luong song song, tai tiep.
    Muc pak cu chep NGUYEN byte (giu co nen UCL, chi doi offset), muc roi ghi tho -> XPackFile doc nhu cu.
  * Giu ROI (game mo bang fopen, khong qua pak): config.ini, package.ini (moi), settings\serverlist.ini (chu doi may chu),
    settings\datau_toado.txt (da tau), ui\uitoado_macdinh.ini (bo cuc mac dinh), userdata\ (game ghi), apdata\ (WAuto ghi).
  * --chi-dung <jx_tep_dung.log ...>: CHI giu muc co trong nhat ky tep dung ('R <tep roi>' / 'P <uid> <ten>', xem
    va_nguon_android_tepdung1.py) -> buoc "rut gon toi da". Gop nhieu nhat ky cua nhieu nguoi choi / nhieu ngay cang du.
    Muc thieu -> game hien anh trong (anh_null), KHONG do; nhung ban do chua tung vao se thieu anh.
Dung: python android\dong_goi_du_lieu_dien_thoai.py [--nguon D:\jx1_android_data] [--dich D:\jx1_android_data_dt]
        [--co-goi 512] [--chi-dung log1 [log2 ...]]
"""
import io
import os
import re
import shutil
import struct
import sys
import time

sys.stdout.reconfigure(encoding="utf-8", errors="replace", line_buffering=True)
GOC = os.path.join(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, GOC)
sys.path.insert(0, os.path.join(GOC, "..", "ReverseTools", "pak_vltk"))
from pakdump import name2id  # noqa: E402

THU_MUC_PAK = ("spr", "maps", "maps2", "settings", "script", "ui")
GIU_ROI = ("config.ini", "settings\\serverlist.ini", "settings\\datau_toado.txt", "ui\\uitoado_macdinh.ini")
THU_MUC_ROI = ("userdata", "apdata")
MB = 1048576
HEADER = 32


CHI_DEM = False        # --chi-dem: chi dem / phan loai, khong ghi gi
TEN_UID = {}           # uid -> ten xin (tu dong 'P' trong nhat ky) de phan loai muc pak
NEN_MUC = 0            # --nen-roi [muc]: nen NRV2B (UCL) cac tep roi khi dua vao pak (0 = khong nen)
NEN_ROI = {}           # duong dan tep roi -> (tep blob da nen, co nen)
SO_TIEN_TRINH = 4      # so tien trinh ucl_nen.exe chay song song


def doc_tham_so():
    global CHI_DEM, NEN_MUC
    a = sys.argv[1:]
    nguon = r"D:\jx1_android_data"; dich = r"D:\jx1_android_data_dt"; logs = []; co_goi = 512
    i = 0
    while i < len(a):
        if a[i] == "--nguon": nguon = a[i + 1]; i += 2
        elif a[i] == "--dich": dich = a[i + 1]; i += 2
        elif a[i] == "--co-goi": co_goi = int(a[i + 1]); i += 2
        elif a[i] == "--chi-dung": i += 1; logs = []
        elif a[i] == "--chi-dem": CHI_DEM = True; i += 1
        elif a[i] == "--nen-roi":
            NEN_MUC = 7; i += 1
            if i < len(a) and a[i].isdigit():
                NEN_MUC = int(a[i]); i += 1
        else:
            logs.append(a[i]); i += 1
    return nguon, dich, logs, co_goi * MB


def co_luu(m):
    """so byte se ghi vao pak cua mot muc"""
    u, ng, size, cf = m
    if isinstance(ng, tuple):
        return ng[2]
    return NEN_ROI[ng][1] if ng in NEN_ROI else size


def nen_tep_roi(ds):
    r"""[ANDROID 12/09 NEN] nen NRV2B cac tep roi bang android\ucl_nen.exe (cung UCL cua XPackFile) -> muc TYPE_UCL,
    kiem lai bang ucl.nrv2b_decompress_8 (pakdump) tren 20 muc ngau nhien. Tra danh sach muc moi (cf da doi)."""
    import random
    import subprocess
    import tempfile
    from ucl import nrv2b_decompress_8
    exe = os.path.abspath(os.path.join(GOC, "ucl_nen.exe"))
    if not os.path.isfile(exe):
        raise SystemExit("thieu %s (dich: xem dau android/ucl_nen.c)" % exe)
    tam = os.path.join(tempfile.gettempdir(), "ucl_nen")
    os.makedirs(tam, exist_ok=True)
    roi = [m for m in ds if not isinstance(m[1], tuple)]
    if not roi:
        return ds
    t0 = time.time()
    phan = [[] for _ in range(SO_TIEN_TRINH)]
    for i, m in enumerate(roi):
        phan[i % SO_TIEN_TRINH].append((i, m))
    tien_trinh = []
    for k, p in enumerate(phan):
        p_ds = os.path.join(tam, "danh_sach_%d.txt" % k)
        with io.open(p_ds, "w", encoding="utf-8", newline="\n") as f:
            for i, m in p:
                f.write("%s\t%s\n" % (m[1], os.path.join(tam, "%06d.bin" % i)))
        tien_trinh.append((p, subprocess.Popen([exe, p_ds, str(NEN_MUC)], stdout=subprocess.PIPE)))
    goc = nen = so_nen = 0
    for p, pr in tien_trinh:
        out = pr.stdout.read().decode("ascii", "replace").splitlines()
        if pr.wait() != 0 or len(out) != len(p):
            raise SystemExit("ucl_nen loi (ma %d, %d/%d dong)" % (pr.returncode, len(out), len(p)))
        for (i, m), dong in zip(p, out):
            a, b = dong.split("\t")
            a = int(a); b = int(b)
            if a < 0:
                raise SystemExit("ucl_nen khong doc duoc " + m[1])
            goc += a
            if b > 0:
                NEN_ROI[m[1]] = (os.path.join(tam, "%06d.bin" % i), b)
                nen += b; so_nen += 1
            else:
                nen += a
    print("nen tep roi (UCL muc %d, %d tien trinh): %d/%d tep nen duoc, %.0f MB -> %.0f MB (%.0f s)"
          % (NEN_MUC, SO_TIEN_TRINH, so_nen, len(roi), goc / MB, nen / MB, time.time() - t0))
    # kiem lai 20 muc: giai nen bang Python phai ra dung byte goc
    mau = [m for m in roi if m[1] in NEN_ROI]
    for m in random.sample(mau, min(20, len(mau))):
        blob = open(NEN_ROI[m[1]][0], "rb").read()
        goc_b = open(m[1], "rb").read()
        if nrv2b_decompress_8(blob, len(goc_b)) != goc_b:
            raise SystemExit("giai nen sai: " + m[1])
    print("  kiem giai nen 20 muc: dung")
    ra = []
    for u, ng, size, cf in ds:
        if not isinstance(ng, tuple) and ng in NEN_ROI:
            ra.append((u, ng, size, 0x01000000 | NEN_ROI[ng][1]))
        else:
            ra.append((u, ng, size, cf))
    return ra


def nhom_cua(ten):
    r"""'\spr\npcres\man\x.spr' -> 'spr\npcres' (2 cap dau) de phan loai dung luong"""
    p = [x for x in ten.replace("/", "\\").lower().split("\\") if x]
    return "\\".join(p[:2]) if len(p) > 2 else (p[0] if p else "?")


def bang_phan_loai(ds, nguon):
    """in bang MB theo nhom (muc roi theo duong dan, muc pak theo ten trong nhat ky, con lai = 'pak (khong biet ten)')"""
    tong = {}
    for u, ng, size, cf in ds:
        luu = ng[2] if isinstance(ng, tuple) else size
        if isinstance(ng, tuple):
            ten = TEN_UID.get(u)
            k = ("pak " + nhom_cua(ten)) if ten else "pak (khong biet ten)"
        else:
            k = "roi " + nhom_cua(os.path.relpath(ng, nguon))
        tong[k] = tong.get(k, 0) + luu
    for k, v in sorted(tong.items(), key=lambda kv: -kv[1])[:25]:
        print("   %8.0f MB  %s" % (v / MB, k))


def ten_sang_l1(s):
    """Ten tren dia (chu cp1252 cua byte GBK, hoac chu Han that) -> chuoi Latin-1 cua byte GBK ma game bam (FileNameToId)"""
    ra = []
    for ch in s:
        o = ord(ch)
        if o < 256:
            ra.append(ch)
            continue
        try:
            b = ch.encode("cp1252")
        except UnicodeEncodeError:
            try:
                b = ch.encode("gbk")
            except UnicodeEncodeError:
                b = b"?"
        ra.append(b.decode("latin-1"))
    return "".join(ra)


def uid_cua(rel_win):
    r"""rel_win: '\spr\...' kieu Windows (ten tren dia)"""
    return name2id(ten_sang_l1(rel_win))


def doc_nhat_ky_dung(logs):
    """jx_tep_dung.log: 'R <duong dan POSIX>' (tep roi) / 'P <uid hex> <ten>' -> tap uid (tep roi cung doi sang uid)"""
    uid = set(); so_r = 0; so_p = 0
    for p in logs:
        for ln in io.open(p, encoding="utf-8", errors="replace"):
            ln = ln.rstrip("\r\n")
            if ln.startswith("P "):
                phan = ln[2:].split(" ", 1)
                try:
                    u = int(phan[0], 16)
                    uid.add(u); so_p += 1
                    if len(phan) > 1 and u not in TEN_UID:
                        TEN_UID[u] = phan[1]
                except ValueError:
                    pass
            elif ln.startswith("R "):
                phan = ln[2:].replace("\\", "/").split("/")
                for i, ph in enumerate(phan):
                    if ph.lower() in THU_MUC_PAK:
                        uid.add(uid_cua("\\" + "\\".join(phan[i:]))); so_r += 1
                        break
    print("nhat ky tep dung: %d dong P, %d dong R -> %d id" % (so_p, so_r, len(uid)))
    return uid


def gom_muc(nguon, loc_uid):
    """Danh sach phang (uid, nguon, size, cf) theo thu tu tim cua game; nguon = duong dan tep | (pak, off, so byte luu)"""
    muc = {}; thu_tu = []
    tong = 0; bo_che = 0; bo_loc = 0
    # 1. tep roi
    for thu in THU_MUC_PAK:
        goc = os.path.join(nguon, thu)
        if not os.path.isdir(goc):
            continue
        for root, ds, fs in os.walk(goc):
            for f in fs:
                p = os.path.join(root, f)
                rel = "\\" + os.path.relpath(p, nguon).replace("/", "\\")
                u = uid_cua(rel)
                if u in muc:
                    bo_che += 1; continue
                if loc_uid is not None and u not in loc_uid:
                    bo_loc += 1; continue
                co = os.path.getsize(p)
                muc[u] = (u, p, co, co)          # cf: kieu 0 | co nen = size
                thu_tu.append(u); tong += co
    n_roi = len(thu_tu)
    print("tep roi: giu %d (%.0f MB), bo che %d, bo ngoai nhat ky %d" % (n_roi, tong / MB, bo_che, bo_loc))
    # 2. pak cu theo package.ini
    ini = io.open(os.path.join(nguon, "package.ini"), encoding="latin-1", newline="").read()
    thu_pak = re.search(r"Path=(.*)", ini).group(1).strip().strip("\\")
    ds = sorted((int(m.group(1)), m.group(2).strip()) for m in re.finditer(r"^(\d+)=(.*)$", ini, re.M))
    for _, ten in ds:
        pp = os.path.join(nguon, thu_pak, ten)
        if not os.path.isfile(pp):
            print("  bo dong package.ini (khong co tep):", ten); continue
        giu = 0; giu_byte = 0; che = 0; loc = 0
        with open(pp, "rb") as f:
            sig, count, ioff, doff = struct.unpack("<4sIII", f.read(16))
            if sig != b"PACK":
                print("  KHONG PHAI PACK, bo:", ten); continue
            f.seek(ioff)
            raw = f.read(count * 16)
        for i in range(count):
            u, off, size, cf = struct.unpack("<IIiI", raw[i * 16:i * 16 + 16])
            if u in muc:
                che += 1; continue
            if loc_uid is not None and u not in loc_uid:
                loc += 1; continue
            luu = (cf & 0xFFFFFF) or size
            muc[u] = (u, (pp, off, luu), size, cf)
            thu_tu.append(u); giu += 1; giu_byte += luu; tong += luu
        print("  %-24s %6d muc: giu %6d (%5.0f MB), bi che %6d, ngoai nhat ky %6d" % (ten, count, giu, giu_byte / MB, che, loc))
    print("tong: %d muc, %.0f MB" % (len(thu_tu), tong / MB))
    return [muc[u] for u in thu_tu], tong


def ghi_goi(duong_dan, ds, tay_pak):
    """ghi mot pak: ds = [(uid, nguon, size, cf)] (sap theo uid tang), muc pak cu chep nguyen byte"""
    ds = sorted(ds, key=lambda m: m[0])
    index = []
    with open(duong_dan, "wb") as f:
        f.write(b"\0" * HEADER)
        off = HEADER
        for u, ng, size, cf in ds:
            if isinstance(ng, tuple):
                pp, o, n = ng
                g = tay_pak.get(pp)
                if g is None:
                    g = tay_pak[pp] = open(pp, "rb")
                g.seek(o)
                con = n
                while con > 0:
                    b = g.read(min(con, 8 * MB)); f.write(b); con -= len(b)
                    if not b:
                        raise SystemExit("doc thieu %s @%d" % (pp, o))
                n_luu = n
            else:
                with open(NEN_ROI[ng][0] if ng in NEN_ROI else ng, "rb") as g:   # tep roi (da nen UCL neu --nen-roi)
                    b = g.read()
                f.write(b); n_luu = len(b)
            index.append((u, off, size, cf))
            off += n_luu
        ioff = off
        for u, o, size, cf in index:
            f.write(struct.pack("<IIiI", u, o, size, cf))
        f.seek(0)
        f.write(b"PACK" + struct.pack("<IIII", len(index), ioff, HEADER, 0) + b"\0" * 12)
    return len(index), ioff


def main():
    nguon, dich, logs, co_goi = doc_tham_so()
    loc_uid = doc_nhat_ky_dung(logs) if logs else None
    t0 = time.time()
    if CHI_DEM:
        ds, tong = gom_muc(nguon, loc_uid)
        print("phan loai (MB luu tru):")
        bang_phan_loai(ds, nguon)
        return
    os.makedirs(os.path.join(dich, "data"), exist_ok=True)
    ds, tong = gom_muc(nguon, loc_uid)
    if NEN_MUC:
        ds = nen_tep_roi(ds)
    # chia goi <= co_goi (theo thu tu gom; sau khi bo trung thi thu tu goi khong con quan trong)
    goi = []; hien = []; co = 0
    for m in ds:
        luu = co_luu(m)
        if hien and co + luu > co_goi:
            goi.append(hien); hien = []; co = 0
        hien.append(m); co += luu
    if hien:
        goi.append(hien)
    ten_goi = []
    tay_pak = {}
    for i, g in enumerate(goi):
        ten = "mobile_%02d.pak" % (i + 1)
        p = os.path.join(dich, "data", ten)
        n, ioff = ghi_goi(p, g, tay_pak)
        ten_goi.append(ten)
        print("  da ghi %s: %d muc, %.0f MB (%.0f s)" % (ten, n, ioff / MB, time.time() - t0))
    for g in tay_pak.values():
        g.close()
    # xoa pak cu khong con trong danh sach
    for f in os.listdir(os.path.join(dich, "data")):
        if f.lower().endswith(".pak") and f not in ten_goi:
            os.remove(os.path.join(dich, "data", f)); print("  xoa pak cu:", f)
    # package.ini moi
    ini = io.open(os.path.join(nguon, "package.ini"), encoding="latin-1", newline="").read()
    nl = "\r\n" if "\r\n" in ini else "\n"
    io.open(os.path.join(dich, "package.ini"), "w", encoding="latin-1", newline="").write(
        "[Package]" + nl + "Path=\\data" + nl + nl.join("%d=%s" % (k, t) for k, t in enumerate(ten_goi)) + nl)
    # tep roi giu lai
    for f in GIU_ROI:
        src = os.path.join(nguon, f)
        if os.path.isfile(src):
            os.makedirs(os.path.dirname(os.path.join(dich, f)), exist_ok=True)
            shutil.copy2(src, os.path.join(dich, f))
        else:
            print("  (khong co tep roi de giu: %s)" % f)
    # [HANHTRANG 12/09] ca lop ghi de Android (android\du_lieu_ghi_de: ini giao dien, anh VNKU) giu ROI trong goi: tep roi thang pak,
    # doi giao dien chi can chep tep + sinh manifest, dien thoai tai vai tram KB, khong phai dong goi lai 7 GB
    ghi_de = os.path.join(GOC, "du_lieu_ghi_de")
    so_gd = 0
    for root, ds, fs in os.walk(ghi_de):
        for f in fs:
            rel = os.path.relpath(os.path.join(root, f), ghi_de)
            if rel.lower() == "config.ini":
                continue
            dst = os.path.join(dich, rel)
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            shutil.copy2(os.path.join(root, f), dst); so_gd += 1
    print("lop ghi de giu roi: %d tep" % so_gd)
    for t in THU_MUC_ROI:
        src = os.path.join(nguon, t)
        if os.path.isdir(src):
            dst = os.path.join(dich, t)
            if os.path.isdir(dst):
                shutil.rmtree(dst)
            shutil.copytree(src, dst)
    for f in ("manifest.txt",):                     # manifest cu -> may chu tai sinh lai
        if os.path.isfile(os.path.join(dich, f)):
            os.remove(os.path.join(dich, f))
    tong_dich = sum(os.path.getsize(os.path.join(r, f)) for r, _, fs in os.walk(dich) for f in fs)
    print("XONG %.0f s: %s = %.0f MB, %d goi pak (<= %d MB)" % (time.time() - t0, dich, tong_dich / MB, len(ten_goi), co_goi // MB))


if __name__ == "__main__":
    main()
