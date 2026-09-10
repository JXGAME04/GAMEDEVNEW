# -*- coding: utf-8 -*-
r"""[ANDROID 12/09 GOI] Dong goi du lieu cho DIEN THOAI: tu D:\jx1_android_data (da rut gon) -> D:\jx1_android_data_dt
  * Moi tep ROI trong spr / maps / maps2 / settings / script / ui -> mot pak 'mobile_roi.pak' (ghi tho, id = FileNameToId cua
    duong dan Windows '\spr\...'), dat DAU package.ini -> tep roi van thang tep cung ten trong pak cu (thu tu tim: dau tien thang).
    -> tai ve nhanh (vai chuc tep to thay vi 100.000 tep nho), khong con lo ten GBK / hoa thuong tren he tep dien thoai.
  * Pak cu: chep nguyen (chi nhung pak con trong package.ini).
  * Giu roi: config.ini, package.ini (moi), userdata\ (game ghi), APdata\ (WAuto ghi), settings\serverlist.ini? -> KHONG: doc qua
    KIniFile -> KFile -> pak duoc; nhung de chu sua may chu de dang, chep them ban roi (tep roi thang pak).
  * --chi-dung <jx_tep_dung.log ...>: chi lay tep / id co trong nhat ky tep dung (buoc rut gon dong, xem BANGIAO_ANDROID_DULIEU_1109.md).
Dung: python android\dong_goi_du_lieu_dien_thoai.py [--nguon D:\jx1_android_data] [--dich D:\jx1_android_data_dt] [--chi-dung log ...]
"""
import io
import os
import re
import shutil
import sys
import time

sys.stdout.reconfigure(encoding="utf-8", errors="replace")
GOC = os.path.join(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, GOC)
sys.path.insert(0, os.path.join(GOC, "..", "ReverseTools", "pak_vltk"))
from pak_ghi import ghi_pak, kiem_pak  # noqa: E402
from pakdump import name2id  # noqa: E402

THU_MUC_PAK = ("spr", "maps", "maps2", "settings", "script", "ui")
GIU_ROI = ("config.ini", "package.ini")
THU_MUC_ROI = ("userdata", "apdata")
MB = 1048576.0


def doc_tham_so():
    a = sys.argv[1:]
    nguon = r"D:\jx1_android_data"; dich = r"D:\jx1_android_data_dt"; logs = []
    i = 0
    while i < len(a):
        if a[i] == "--nguon": nguon = a[i + 1]; i += 2
        elif a[i] == "--dich": dich = a[i + 1]; i += 2
        elif a[i] == "--chi-dung": i += 1; logs = []
        else:
            logs.append(a[i]); i += 1
    return nguon, dich, logs


def doc_nhat_ky_dung(logs):
    """jx_tep_dung.log: moi dong 'R <duong dan>' (tep roi) hoac 'P <uid hex> <duong dan xin>' -> tap duong dan (ha ascii) + tap uid"""
    duong = set(); uid = set()
    for p in logs:
        for ln in io.open(p, encoding="latin-1"):
            ln = ln.strip()
            if ln.startswith("R "):
                duong.add(ln[2:].lower().replace("/", "\\"))
            elif ln.startswith("P "):
                phan = ln[2:].split(" ", 1)
                try:
                    uid.add(int(phan[0], 16))
                except ValueError:
                    pass
                if len(phan) > 1:
                    duong.add(phan[1].lower().replace("/", "\\"))
    return duong, uid


def main():
    nguon, dich, logs = doc_tham_so()
    loc_duong, loc_uid = doc_nhat_ky_dung(logs) if logs else (None, None)
    os.makedirs(dich, exist_ok=True)
    t0 = time.time()
    # 1. tep roi -> mobile_roi.pak
    muc = {}
    tong = 0; bo_loc = 0
    for thu in THU_MUC_PAK:
        goc = os.path.join(nguon, thu)
        if not os.path.isdir(goc):
            continue
        for root, ds, fs in os.walk(goc):
            for f in fs:
                p = os.path.join(root, f)
                rel = "\\" + os.path.relpath(p, nguon).replace("/", "\\")
                if loc_duong is not None and rel.lower() not in loc_duong:
                    bo_loc += 1
                    continue
                uid = name2id(rel)
                if uid in muc:
                    print("  TRUNG id %08x: %s  ~  %s" % (uid, rel.encode("latin-1", "replace"), muc[uid].encode("latin-1", "replace")))
                    continue
                muc[uid] = p
                tong += os.path.getsize(p)
    print("tep roi -> pak: %d tep, %.0f MB%s" % (len(muc), tong / MB, (" (bo %d tep ngoai nhat ky dung)" % bo_loc) if logs else ""))
    os.makedirs(os.path.join(dich, "data"), exist_ok=True)
    p_pak = os.path.join(dich, "data", "mobile_roi.pak")
    n, ioff = ghi_pak(p_pak, list(muc.items()), tien_trinh=lambda i, n, off: print("  ... %d/%d (%.0f MB)" % (i, n, off / MB)))
    print("da ghi %s: %d muc, %.0f MB; kiem: %s" % (p_pak, n, os.path.getsize(p_pak) / MB, kiem_pak(p_pak)[:2]))
    # 2. pak cu theo package.ini
    ini = io.open(os.path.join(nguon, "package.ini"), encoding="latin-1", newline="").read()
    thu_pak = re.search(r"Path=(.*)", ini).group(1).strip().strip("\\")
    ds = sorted((int(m.group(1)), m.group(2).strip()) for m in re.finditer(r"^(\d+)=(.*)$", ini, re.M))
    con = []
    for _, ten in ds:
        src = os.path.join(nguon, thu_pak, ten)
        if not os.path.isfile(src):
            print("  bo dong package.ini (khong co tep):", ten); continue
        dst = os.path.join(dich, "data", ten)
        if not os.path.isfile(dst) or os.path.getsize(dst) != os.path.getsize(src):
            if os.path.isfile(dst):
                os.remove(dst)
            try:
                os.link(src, dst)          # cung o dia: lien ket cung, khong ton cho
            except OSError:
                shutil.copy2(src, dst)
        con.append(ten)
    nl = "\r\n" if "\r\n" in ini else "\n"
    dau = ini[:ini.index("0=")] if "0=" in ini else "[Package]" + nl + "Path=\\data" + nl
    moi = dau + nl.join("%d=%s" % (k, t) for k, t in enumerate(["mobile_roi.pak"] + con)) + nl
    io.open(os.path.join(dich, "package.ini"), "w", encoding="latin-1", newline="").write(moi)
    print("package.ini: %d pak (mobile_roi.pak dau tien)" % (len(con) + 1))
    # 3. tep roi giu lai
    for f in GIU_ROI:
        if f != "package.ini" and os.path.isfile(os.path.join(nguon, f)):
            shutil.copy2(os.path.join(nguon, f), os.path.join(dich, f))
    for t in THU_MUC_ROI:
        src = os.path.join(nguon, t)
        if os.path.isdir(src):
            dst = os.path.join(dich, t)
            if os.path.isdir(dst):
                shutil.rmtree(dst)
            shutil.copytree(src, dst)
    # serverlist roi de chu sua may chu (tep roi thang pak)
    os.makedirs(os.path.join(dich, "settings"), exist_ok=True)
    for f in ("serverlist.ini",):
        src = os.path.join(nguon, "settings", f)
        if os.path.isfile(src):
            shutil.copy2(src, os.path.join(dich, "settings", f))
    tong_dich = sum(os.path.getsize(os.path.join(r, f)) for r, _, fs in os.walk(dich) for f in fs)
    print("XONG %.0f s: %s = %.0f MB" % (time.time() - t0, dich, tong_dich / MB))


if __name__ == "__main__":
    main()
