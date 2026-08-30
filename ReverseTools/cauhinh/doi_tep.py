# -*- coding: utf-8 -*-
"""doi_tep.py - DOI tep/thu muc ra khoi cay script mot cach HOI LAI DUOC.

KHONG BAO GIO XOA. Moi thu deu duoc DOI sang cay guong:
    E:\\...\\bin\\server\\_dara\\script\\<duong dan cu>
giu nguyen cau truc thu muc, nen muon hoi lai chi can chep nguoc.

VI SAO KHONG XOA: bo mot tinh nang la viec de sai. Neu sau khi tat may chu moi
phat hien thieu, phai lay lai duoc trong mot phut - khong the doi git.

CO CHOT AN TOAN:
 - Truoc khi doi, quet xem con AI GOI tep do khong (Include / dofile /
   DynamicExecute / ReLoadScript / duong dan trong settings). Con nguoi goi ma
   van muon doi thi phai truyen `du_biet_con_goi=True` - de khong bao gio doi
   nham vi so sot.
 - Ghi mot tep nhat ky `_dara\\NHATKY_DOI.txt` moi lan doi: doi gi, luc nao,
   ly do, va ai con goi (neu co).
 - Ham `hoan_tac()` chep nguoc tat ca.
"""
import io
import os
import re
import shutil
import time

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
S = os.path.join(SV, "script")
DARA = os.path.join(SV, "_dara")
NHATKY = os.path.join(DARA, "NHATKY_DOI.txt")

NAP = re.compile(
    r"(?:Include|dofile|DynamicExecute\w*|ReLoadScript|SetNpcDropScript"
    r"|ExecuteScript\w*|AddNpc\w*)\s*\([^)]*?([\w\\/.-]+\.lua)", re.I)


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def _chuan(s):
    s = s.replace("/", "\\").lower()
    s = re.sub(r"\\+", "\\\\", s)
    return s.lstrip("\\")


def ai_goi(duong_tuong_doi, bo_qua=None):
    """Tim moi tep con nhac toi tep/thu muc nay (khong tinh chinh no).

    duong_tuong_doi: vd "tinhnang\\phonglangdo" hoac
                     "tinhnang\\phonglangdo\\drop.lua"
    Tra ve danh sach (tep_goi, so_dong, noi_dung_dong).
    """
    can = _chuan(duong_tuong_doi)
    bo = [_chuan(x) for x in (bo_qua or [])]
    ra = []
    for goc_cay in (S, os.path.join(SV, "settings")):
        if not os.path.isdir(goc_cay):
            continue
        for goc, _, tep in os.walk(goc_cay):
            for t in tep:
                if not t.lower().endswith((".lua", ".txt", ".ini")):
                    continue
                p = os.path.join(goc, t)
                rel = _chuan(os.path.relpath(p, S if goc_cay == S else SV))
                if rel.startswith(can) or any(rel.startswith(b) for b in bo):
                    continue          # chinh no hoac phan cung bi doi
                if ".truoc_" in t or t.endswith(".rar"):
                    continue
                try:
                    d = doc(p).replace("\r\n", "\n").split("\n")
                except OSError:
                    continue
                for i, l in enumerate(d, 1):
                    if l.strip().startswith("--"):
                        continue
                    if can in _chuan(l):
                        ra.append((os.path.relpath(p, SV), i, l.strip()[:120]))
    return ra


def ghi_nhat_ky(dong):
    if not os.path.isdir(DARA):
        os.makedirs(DARA)
    with io.open(NHATKY, "a", encoding="utf-8", newline="") as f:
        f.write(dong + "\n")


def doi(duong_tuong_doi, ly_do, ghi=False, du_biet_con_goi=False):
    """Doi mot tep hoac ca thu muc sang _dara. Tra ve (ok, thong_diep)."""
    nguon = os.path.join(S, duong_tuong_doi.replace("/", os.sep))
    if not os.path.exists(nguon):
        return False, "khong ton tai: %s" % duong_tuong_doi
    goi = ai_goi(duong_tuong_doi)
    if goi and not du_biet_con_goi:
        return False, ("CON %d CHO GOI - khong doi. Cho dau tien: %s:%d | %s"
                       % (len(goi), goi[0][0], goi[0][1], goi[0][2]))
    dich = os.path.join(DARA, "script", duong_tuong_doi.replace("/", os.sep))
    if not ghi:
        return True, ("se doi (%d cho con goi)" % len(goi))
    tm = os.path.dirname(dich)
    if not os.path.isdir(tm):
        os.makedirs(tm)
    if os.path.exists(dich):
        return False, "dich da ton tai: %s" % dich
    shutil.move(nguon, dich)
    ghi_nhat_ky("[%s] %s  <- %s%s"
                % (time.strftime("%Y-%m-%d %H:%M:%S"), duong_tuong_doi, ly_do,
                   ("  (CON %d CHO GOI)" % len(goi)) if goi else ""))
    for g in goi:
        ghi_nhat_ky("        con goi tai %s:%d | %s" % g)
    return True, "da doi"


def hoan_tac(duong_tuong_doi, ghi=False):
    """Chep nguoc tu _dara ve cay script."""
    nguon = os.path.join(DARA, "script", duong_tuong_doi.replace("/", os.sep))
    dich = os.path.join(S, duong_tuong_doi.replace("/", os.sep))
    if not os.path.exists(nguon):
        return False, "khong co trong _dara: %s" % duong_tuong_doi
    if os.path.exists(dich):
        return False, "cho cu da co gi do: %s" % dich
    if not ghi:
        return True, "se hoan tac"
    tm = os.path.dirname(dich)
    if not os.path.isdir(tm):
        os.makedirs(tm)
    shutil.move(nguon, dich)
    ghi_nhat_ky("[%s] HOAN TAC %s"
                % (time.strftime("%Y-%m-%d %H:%M:%S"), duong_tuong_doi))
    return True, "da hoan tac"


def dem(duong_tuong_doi):
    """Dem so tep va so dong trong mot duong dan (de bao cao quy mo)."""
    p = os.path.join(S, duong_tuong_doi.replace("/", os.sep))
    if os.path.isfile(p):
        try:
            return 1, doc(p).count("\n") + 1
        except OSError:
            return 1, 0
    n_tep = n_dong = 0
    for goc, _, tep in os.walk(p):
        for t in tep:
            n_tep += 1
            try:
                n_dong += doc(os.path.join(goc, t)).count("\n") + 1
            except OSError:
                pass
    return n_tep, n_dong
