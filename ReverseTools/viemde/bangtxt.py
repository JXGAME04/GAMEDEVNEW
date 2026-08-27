# -*- coding: utf-8 -*-
"""Doc/ghi bang .txt cua JX1 mot cach AN TOAN VE BYTE.

Ba bai hoc phai tra gia (26/08):
  1. Cac bang nay dung CRLF. Doc bang newline="" roi ghi bang "\\n".join(...)
     se BIEN CRLF THANH LF cho CA TEP - 2036 dong doi byte trong khi ta chi
     dinh sua 32 dong. Lop nay do va giu nguyen kieu xuong dong cua tep goc.
  2. Ghi thang bang open(p,"w") ma nua chung nem loi thi TEP GOC BI CAT CUT
     (da lam magicscript.txt cua may chu ve 0 byte). Lop nay ghi ra tep tam
     roi moi thay the.
  3. Chu Viet phai la byte TCVN3 (mot byte). Chuoi lay tu ban Linux da duoc
     dec2 giai ma thanh Unicode nen PHAI ma hoa lai, KHONG duoc de nguyen.
"""
import io
import os
import shutil
import sys

sys.path.insert(0, r"C:\Users\nguye\.claude\skills\swordonline-dev\scripts")
from vn_to_octal import unicode_to_tcvn3_bytes

_REV = {}
for _cp in range(0x20, 0x2000):
    _ch = chr(_cp)
    try:
        _b = unicode_to_tcvn3_bytes(_ch)
    except Exception:
        continue
    if len(_b) == 1 and _b[0] >= 0x80:
        _REV.setdefault(_b[0], _ch)


def tcvn2uni(s):
    """chuoi doc bang latin-1 tu tep JX1 -> chu Viet doc duoc"""
    return "".join(_REV.get(ord(c), c) if ord(c) >= 0x80 else c for c in s)


def uni2tcvn(s):
    """chu Viet -> chuoi latin-1 ghi thang duoc vao tep JX1.

    🔴 PHAI ma hoa CA CHUOI mot lan. Goi TUNG KY TU la SAI: TCVN3 ma hoa nhieu
    chu Viet thanh HAI byte (chu goc + dau) va unicode_to_tcvn3_bytes lam viec do
    theo NGU CANH, nen dua tung ky tu vao thi no nem loi -> ky tu bi bo -> chu
    "Cach choi vuot ai moi" thanh "Cch chi vt i míi" (da dinh 26/08).

    Chi khi ca chuoi khong ma hoa duoc (thuong vi lan chu Trung) moi lui ve xu
    tung ky tu, va luc do ky tu nao chiu thua thi BO chu khong lam hong ca o."""
    if not s:
        return s
    try:
        return unicode_to_tcvn3_bytes(s).decode("latin-1")
    except Exception:
        pass
    ra = []
    for c in s:
        if ord(c) < 128:
            ra.append(c)
            continue
        try:
            ra.append(unicode_to_tcvn3_bytes(c).decode("latin-1"))
        except Exception:
            pass          # ky tu khong thuoc bang ma TCVN3 (vd chu Trung) -> bo
    return "".join(ra)


class Bang:
    """bang .txt phan cach bang TAB, giu nguyen kieu xuong dong va byte."""

    def __init__(self, path):
        self.path = path
        raw = open(path, "rb").read()
        self.crlf = raw.count(b"\r\n") > (raw.count(b"\n") - raw.count(b"\r\n"))
        self.cuoi_co_xuongdong = raw.endswith(b"\n")
        d = raw.decode("latin-1")
        dong = d.split("\r\n") if self.crlf else d.split("\n")
        if dong and dong[-1] == "":
            dong.pop()
        self.rows = [l.rstrip("\r").split("\t") for l in dong]
        self.nCot = len(self.rows[0])

    def hdr(self):
        return {n: i for i, n in enumerate(self.rows[0])}

    def ghi(self, hau_to_sao_luu=None):
        nl = "\r\n" if self.crlf else "\n"
        noi_dung = nl.join("\t".join(r) for r in self.rows)
        if self.cuoi_co_xuongdong:
            noi_dung += nl
        # kiem TRUOC khi dung den tep goc
        b = noi_dung.encode("latin-1")     # nem loi o day thi tep goc con nguyen
        if hau_to_sao_luu and not os.path.isfile(self.path + hau_to_sao_luu):
            shutil.copyfile(self.path, self.path + hau_to_sao_luu)
        tam = self.path + ".dangghi"
        with open(tam, "wb") as f:
            f.write(b)
        os.replace(tam, self.path)
        return len(b)


def so_sanh_byte(path, hau_to_sao_luu, cho_phep_them):
    """kiem sau khi va: chi duoc THEM/SUA dung nhung dong da khai, kieu xuong
    dong khong doi, khong sinh ky tu hong."""
    a = open(path + hau_to_sao_luu, "rb").read()
    b = open(path, "rb").read()
    ra = {
        "crlf_truoc": a.count(b"\r\n"), "crlf_sau": b.count(b"\r\n"),
        "lf_don_truoc": a.count(b"\n") - a.count(b"\r\n"),
        "lf_don_sau": b.count(b"\n") - b.count(b"\r\n"),
        "byte_cao_truoc": sum(1 for x in a if x >= 0x80),
        "byte_cao_sau": sum(1 for x in b if x >= 0x80),
        "fffd": b.count(b"\xef\xbf\xbd"),
    }
    ra["eol_giu_nguyen"] = (ra["lf_don_truoc"] == ra["lf_don_sau"] and
                            ra["crlf_sau"] >= ra["crlf_truoc"])
    return ra
