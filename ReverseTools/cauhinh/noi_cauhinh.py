# -*- coding: utf-8 -*-
"""noi_cauhinh.py - bo khung dung chung de NOI HANG SO ra tep cau hinh.

Rut ra tu 5 dot da lam tay (t18 t19 t20 t21 t22). Moi dot deu lap lai dung mot
khuon, va moi lan lap lai la mot lan co the go sai. Tep nay gom khuon do lai.

KHUON:
 1) Trong tep dich, doi mot dong khai kieu TEN_BIEN = 123 thanh
    TEN_BIEN = <CFG>("KHOA", 123). Gia tri mac dinh LAY TU CHINH DONG DO -
    khong bao gio go tay, nen khong the lam doi can bang.
 2) Chen mot ham <CFG> nho o dau tep. Ham do goi G_CFG neu co, khong thi tra ve
    mac dinh. Nho vay tep nao chua nap bo cau hinh van chay y nhu cu.
 3) Chen Include ch_lib + tep cau hinh tuong ung (ca hai deu la LA).
 4) Sinh cac dong khoa de do vao tep cau hinh.

CHOT TU KIEM (chay truoc khi ghi):
 - dong khai phai co DUNG MOT LAN
 - so byte tieng Viet (TCVN3) khong doi
 - can bang tu khoa Lua khong doi (function+then+do-elseif == end)
 - doc lai sau khi ghi phai khop 100%

CACH DUNG:
    import noi_cauhinh as nc
    kq = nc.noi(tep=..., tep_cfg=..., bang_cfg="tbCFG_CHUNG",
                ten_ham="ABC_CFG", nhan="[CFGABC 30/08]",
                muc=[("TEN_BIEN", "ABC_KHOA", "mo ta")],
                tieu_de=["dong chu thich"])
    if nc.in_ket(kq) and ghi_that:
        nc.ghi(kq)
"""
import io
import os
import re
import shutil


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def eol_cua(s):
    return "\r\n" if s.count("\r\n") >= (s.count("\n") - s.count("\r\n")) else "\n"


def can_bang(s):
    """Do lech giua (function+then+do-elseif) va so `end`, bo chu thich + chuoi."""
    t = re.sub(r"--[^\n]*", "", s)
    t = re.sub(r'"[^"]*"', '""', t)
    t = re.sub(r"'[^']*'", "''", t)

    def d(w):
        return len(re.findall(r"\b%s\b" % w, t))

    return (d("function") + d("then") + d("do") - d("elseif")) - d("end")


def _ham_cfg(ten_ham, nhan, eol):
    return eol.join([
        "",
        "-- " + nhan + " Bo doc cau hinh cho tep nay. Tra ve MAC DINH (= so cu)",
        "-- khi bo cau hinh chua nap, nen kem nhat cung khong the doi hanh vi.",
        "function %s(szKhoa, macdinh)" % ten_ham,
        "\tif (G_CFG ~= nil) then",
        "\t\treturn G_CFG(szKhoa, macdinh)",
        "\tend",
        "\treturn macdinh",
        "end",
        "",
    ])


def noi(tep, tep_cfg, bang_cfg, ten_ham, nhan, muc, tieu_de=None,
        tep_cfg_include=None):
    """Tra ve dict mo ta viec se lam. KHONG ghi gi.

    muc: danh sach (ten_bien, ten_khoa, mo_ta)
    """
    kq = {"tep": tep, "tep_cfg": tep_cfg, "loi": [], "khoa": [], "log": []}
    if not os.path.isfile(tep):
        kq["loi"].append("thieu tep dich: %s" % tep)
        return kq
    raw = doc(tep)
    if nhan in raw:
        kq["log"].append("%s DA VA - bo qua" % os.path.basename(tep))
        kq["bo_qua"] = True
        return kq

    eol = eol_cua(raw)
    hi0 = sum(1 for c in raw if ord(c) > 127)
    cb0 = can_bang(raw)
    nd = raw

    for ten_bien, ten_khoa, mo_ta in muc:
        so_khai = len(re.findall(r"^[ \t]*%s\s*=" % re.escape(ten_bien), nd, re.M))
        if so_khai != 1:
            kq["loi"].append("%s duoc khai %d lan (can dung 1)"
                             % (ten_bien, so_khai))
            return kq
        m = re.search(r"^([ \t]*%s\s*=\s*)(-?\d+)(.*)$" % re.escape(ten_bien),
                      nd, re.M)
        if not m:
            kq["loi"].append("%s co khai nhung gia tri khong phai so nguyen"
                             % ten_bien)
            return kq
        gia_tri = int(m.group(2))
        nd = nd[:m.start()] + '%s%s("%s", %d)%s' % (m.group(1), ten_ham,
                                                    ten_khoa, gia_tri,
                                                    m.group(3)) + nd[m.end():]
        kq["khoa"].append((ten_khoa, gia_tri, mo_ta))
        kq["log"].append("  %-26s %s = %d" % (ten_khoa, ten_bien, gia_tri))

    duong = tep_cfg_include
    if duong is None:
        duong = "\\\\script\\\\cauhinh\\\\" + os.path.basename(tep_cfg)
    m = re.search(r'^Include\("[^"]+"\)', nd, re.M)
    them = eol.join([
        "-- " + nhan + " hai tep duoi day la LA (khong Include gi).",
        'Include("\\\\script\\\\cauhinh\\\\ch_lib.lua")',
        'Include("%s")' % duong,
    ]) + _ham_cfg(ten_ham, nhan, eol)
    if m:
        nd = nd.replace(m.group(0), m.group(0) + eol + them, 1)
    else:
        nd = them + eol + nd

    if sum(1 for c in nd if ord(c) > 127) != hi0:
        kq["loi"].append("byte tieng Viet doi")
        return kq
    cb1 = can_bang(nd)
    if cb1 != cb0:
        kq["loi"].append("can bang tu khoa Lua doi (%d -> %d)" % (cb0, cb1))
        return kq
    kq["noi_dung"] = nd
    kq["raw"] = raw

    if not os.path.isfile(tep_cfg):
        kq["loi"].append("thieu tep cau hinh: %s" % tep_cfg)
        return kq
    raw_c = doc(tep_cfg)
    kq["raw_cfg"] = raw_c
    if nhan in raw_c:
        kq["log"].append("%s DA VA - bo qua" % os.path.basename(tep_cfg))
        kq["noi_dung_cfg"] = raw_c
        return kq
    eol_c = eol_cua(raw_c)
    dong = [""] + ["-- " + x for x in (tieu_de or [])] + [""]
    for k, v, mo_ta in kq["khoa"]:
        dong.append("%-26s= %-12d,\t-- %s" % (k, v, mo_ta))
    moc = bang_cfg + " = {"
    if raw_c.count(moc) != 1:
        kq["loi"].append("tep cau hinh khong co dung mot moc %s" % moc)
        return kq
    kq["noi_dung_cfg"] = raw_c.replace(moc, moc + eol_c + eol_c.join(dong))
    kq["log"].append("  => %d khoa, can bang tu khoa giu nguyen (%d)"
                     % (len(kq["khoa"]), cb1))
    return kq


def in_ket(kq):
    for x in kq.get("log", []):
        print(x)
    for x in kq.get("loi", []):
        print("!!! LOI TO: %s" % x)
    return not kq.get("loi")


def ghi(kq, hau_to=".truoc_noicfg"):
    """Ghi that. Chi goi khi in_ket() tra True."""
    if kq.get("loi") or kq.get("bo_qua"):
        return False
    for p, moi, cu in ((kq["tep"], kq.get("noi_dung"), kq.get("raw")),
                       (kq["tep_cfg"], kq.get("noi_dung_cfg"),
                        kq.get("raw_cfg"))):
        if moi is None or moi == cu:
            continue
        sao = p + hau_to
        if not os.path.isfile(sao):
            shutil.copy2(p, sao)
        with io.open(p, "wb") as f:
            f.write(moi.encode("latin-1"))
        if doc(p) != moi:
            print("!!! LOI TO: doc lai KHONG khop: %s" % p)
            return False
        print("  DA GHI %s" % os.path.basename(p))
    return True
