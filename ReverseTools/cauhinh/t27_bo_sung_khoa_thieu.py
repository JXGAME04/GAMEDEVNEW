# -*- coding: utf-8 -*-
"""t27_bo_sung_khoa_thieu.py - bo sung cac khoa SCRIPT DA DOC nhung TEP CAU HINH
CHUA KHAI, va kiem ca hai chieu cho toan bo he cau hinh.

VI SAO CO CHUYEN NAY: bo khung noi_cauhinh (ban dau) kiem "nhan da co trong tep
cau hinh thi bo qua". Nhung mot dot co the do vao CUNG mot tep cau hinh nhieu
lan (nhieu tep dich cung ghi vao ch_thuong.lua chang han) - tu lan thu hai tro
di bi bo qua het. Ket qua: script DOC khoa ma tep cau hinh KHONG KHAI.

Hau qua that: hoat dong VAN CHAY DUNG (ham doc tra ve mac dinh = so cu), nhung
chu game mo tep cau hinh khong thay khoa dau ma chinh - tuc mat dung cai loi ich
ma ca dot nay sinh ra. Da sua goc trong noi_cauhinh.py (kiem TUNG KHOA).

Tep nay QUET HAI CHIEU:
  (1) khoa script DOC ma cau hinh KHONG KHAI  -> bo sung vao tep cau hinh
  (2) khoa cau hinh KHAI ma khong script nao doc -> chi bao, khong xoa
      (co the la khoa dat truoc cho dot sau)

Mac dinh DIEN TAP; --ghi moi ghi that.
"""
import io
import os
import re
import shutil
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
TM_CFG = os.path.join(S, "cauhinh")
NHAN = "[BOSUNG 30/08]"

# khoa nao thuoc tep cau hinh nao (theo tien to)
THUOC = [
    ("ch_lich.lua", "tbCFG_LICH", ("BAT_", "TK_", "CTLD_", "CTC_CHUKY_QUET")),
    ("ch_drop.lua", "tbCFG_DROP", ("SKD_", "DRQ_")),
    ("ch_exp.lua", "tbCFG_EXP", ("BRXP_",)),
    ("ch_thuong.lua", "tbCFG_THUONG", ("VT_", "TKT_")),
    ("ch_chung.lua", "tbCFG_CHUNG", ("CH_", "GLB_", "CTC_")),
]

# mo ta cho khoa bo sung (biet truoc tu cac dot da lam)
MOTA = {
    "TKT_PHI_TRINHSAT": "phi moi lan dung chuc nang trinh sat",
    "TKT_GIA_DOI_NHACVUONGKIEM": "so tien can de doi Nhac Vuong Kiem",
    "TKT_SL_HONTHACH_DOI_KIEM":
        "so Nhac Vuong Hon Thach can de doi mot Nhac Vuong Kiem",
}


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def main():
    ghi = "--ghi" in sys.argv[1:]
    print("=== t27_bo_sung_khoa_thieu - %s ===" % ("GHI THAT" if ghi else "DIEN TAP"))

    # ---- gom moi khoa MA SCRIPT DOC (qua cac ham *_CFG) ----
    doc_boi_script = {}
    # Ten ham doc cau hinh khong theo mot khuon: G_CFG, SKD_CFG, VT_CFG,
    # TK_CFG, TKT_CFG, GLB_CFG, CTC_CFG, CTLD_CFG va ca CFG_EXP (chu CFG dung
    # DAU ten). Regex phai bat chu CFG o BAT KY dau trong ten ham - lan dau
    # toi viet \w*_?CFG nen bo sot CFG_EXP va bao nham 4 khoa BRXP_ la "khong
    # ai doc".
    mau = re.compile(r'\b[A-Za-z_]*CFG[A-Za-z_]*\s*\(\s*"([A-Z][A-Z0-9_]*)"'
                     r'\s*,\s*(-?\d+)')
    for goc, _, tep in os.walk(S):
        if os.path.basename(goc).lower() == "cauhinh":
            continue
        for t in tep:
            if not t.lower().endswith(".lua") or ".truoc_" in t:
                continue
            p = os.path.join(goc, t)
            try:
                d = doc(p)
            except OSError:
                continue
            for m in mau.finditer(d):
                doc_boi_script.setdefault(m.group(1),
                                          (os.path.relpath(p, S),
                                           int(m.group(2))))
    print("  script doc %d khoa cau hinh" % len(doc_boi_script))

    # ---- gom moi khoa DA KHAI trong tep cau hinh ----
    # PHAI ke ca bang CU cauhinh_hoatdong.lua (tbCHD, 120 khoa tien to TW_ BR_
    # BW_ TC_ HD3_ HCD_ QDHK_ YDBZ_). Bo sot no thi bo quet bao nham gan 100
    # khoa "thieu" - da mac dung loi do o lan chay dau.
    da_khai = {}
    for ten, _bang, _tien in THUOC:
        p = os.path.join(TM_CFG, ten)
        if not os.path.isfile(p):
            continue
        for m in re.finditer(r"^\s*([A-Z][A-Z0-9_]*)\s*=", doc(p), re.M):
            da_khai[m.group(1)] = ten
    p_cu = os.path.join(S, "header", "cauhinh_hoatdong.lua")
    if os.path.isfile(p_cu):
        n_cu = 0
        for m in re.finditer(r"^\s*([A-Z][A-Z0-9_]*)\s*=", doc(p_cu), re.M):
            if m.group(1) not in da_khai:
                da_khai[m.group(1)] = "cauhinh_hoatdong.lua"
                n_cu += 1
        print("  bang cu cauhinh_hoatdong.lua khai them %d khoa" % n_cu)
    print("  tong cong da khai %d khoa" % len(da_khai))

    thieu = {k: v for k, v in doc_boi_script.items() if k not in da_khai}
    thua = [k for k in da_khai if k not in doc_boi_script]
    print()
    print("  (1) script DOC ma cau hinh CHUA KHAI: %d" % len(thieu))
    for k, (tep, v) in sorted(thieu.items()):
        print("      %-30s = %-12d  (%s)" % (k, v, tep))
    print("  (2) cau hinh KHAI ma khong ai doc  : %d" % len(thua))
    for k in sorted(thua)[:12]:
        print("      %-30s (%s)" % (k, da_khai[k]))
    if len(thua) > 12:
        print("      ... con %d" % (len(thua) - 12))

    if not thieu:
        print("\nKhong co gi de bo sung.")
        return 0

    # ---- bo sung ----
    theo_tep = {}
    for k, (tep_nguon, v) in thieu.items():
        dich = None
        for ten, bang, tien in THUOC:
            if any(k.startswith(x) for x in tien):
                dich = (ten, bang)
                break
        if dich is None:
            print("!!! LOI TO: khong biet dat khoa %s vao tep nao" % k)
            return 1
        theo_tep.setdefault(dich, []).append((k, v, tep_nguon))

    viec = []
    for (ten, bang), ds in theo_tep.items():
        p = os.path.join(TM_CFG, ten)
        raw = doc(p)
        eol = "\r\n" if raw.count("\r\n") >= (raw.count("\n")
                                             - raw.count("\r\n")) else "\n"
        dong = ["",
                "-- " + NHAN + " bo sung cac khoa script DA DOC nhung tep nay",
                "-- chua khai (loi cua bo khung, da sua goc trong"
                " noi_cauhinh.py).", ""]
        for k, v, tep_nguon in sorted(ds):
            mo = MOTA.get(k, "doc tai " + tep_nguon)
            dong.append("%-26s= %-12d,\t-- %s" % (k, v, mo))
        moc = bang + " = {"
        if raw.count(moc) != 1:
            print("!!! LOI TO: %s khong co dung mot moc %s" % (ten, moc))
            return 1
        viec.append((p, raw.replace(moc, moc + eol + eol.join(dong)), raw,
                     ten, len(ds)))
        print("  se them %d khoa vao %s" % (len(ds), ten))

    if not ghi:
        print("\nDIEN TAP - chua ghi. Chay lai voi --ghi de ap that.")
        return 0
    for p, nd, raw, ten, n in viec:
        sao = p + ".truoc_bosung"
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(nd.encode("latin-1"))
        if doc(p) != nd:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return 1
        print("  DA GHI %-16s (+%d khoa)" % (ten, n))
    return 0


if __name__ == "__main__":
    sys.exit(main())
