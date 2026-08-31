# -*- coding: utf-8 -*-
"""t36_do_phu_thuoc_he_cu.py - do CHINH XAC phan nao cua thu vien CU con duoc goi.

Include mot tep khong co nghia la dang DUNG no. Cong cu nay:
  1. Doc moi tep trong thu muc he CU, liet ke MOI TEN TOAN CUC no dinh nghia
     (function <ten>, <TEN> = {, <TEN> = <so>).
  2. Voi moi tep BEN NGOAI co Include he cu, tim xem no goi ten nao trong so do.
  3. In ra: tep nao Include ma KHONG dung gi (go Include la xong) va tep nao
     THUC SU dung (phai xu ly tay).

Nho vay biet duoc go mot he cu ton bao nhieu cong.
CHI DOC.
"""
import io
import os
import re
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
except AttributeError:
    pass

SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
S = os.path.join(SV, "script")
RA = os.path.join(os.path.dirname(os.path.abspath(__file__)), "phuthuoc_he_cu.txt")

HE = [
    ("tinhnang/datau", "Da Tau CU"),
    ("tinhnang/congthanhchien", "Cong Thanh CU"),
    ("tinhnang/phonglangdo", "Phong Lang Do ban VN"),
    ("tinhnang/vuot_ai", "Vuot Ai ban VN"),
    ("tinhnang/boss_satthu", "Boss Sat Thu ban VN"),
    ("tinhnang/loidai", "Loi Dai CU"),
    ("tinhnang/loidaihonchien", "Loi Dai Hon Chien"),
]

RE_HAM = re.compile(r"^\s*function\s+([A-Za-z_]\w*)\s*\(", re.M)
RE_BANG = re.compile(r"^\s*([A-Za-z_]\w*)\s*=\s*[{\d\"']", re.M)


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def bo_chu_thich(s):
    """Bo comment va chuoi de dem ten trong MA THAT."""
    s = re.sub(r"--\[\[.*?\]\]", " ", s, flags=re.S)
    s = re.sub(r"--[^\n]*", " ", s)
    s = re.sub(r'"[^"\n]*"', '""', s)
    s = re.sub(r"'[^'\n]*'", "''", s)
    return s


def ten_toan_cuc(thu_muc):
    """Tra ve {ten: tep dinh nghia} cua mot thu muc he cu."""
    ra = {}
    p0 = os.path.join(S, thu_muc.replace("/", os.sep))
    for goc, _, tep in os.walk(p0):
        for t in tep:
            if not t.lower().endswith(".lua"):
                continue
            p = os.path.join(goc, t)
            try:
                d = bo_chu_thich(doc(p))
            except OSError:
                continue
            rel = os.path.relpath(p, S)
            for m in RE_HAM.finditer(d):
                ra.setdefault(m.group(1), rel)
            for m in RE_BANG.finditer(d):
                ten = m.group(1)
                if ten.upper() == ten and len(ten) > 3:   # chi bang HOA
                    ra.setdefault(ten, rel)
    return ra


def _chuan(s):
    """Chuan hoa duong dan: / -> \\, gom \\\\ thanh mot, ha chu thuong.

    QUAN TRONG: trong ma Lua duong dan viet "\\\\script\\\\tinhnang\\\\datau" -
    HAI dau gach. So bang mot dau gach thi TRUOT HET. Da vap loi nay.
    """
    return re.sub(r"\\+", "\\\\", s.replace("/", "\\").lower())


def tep_include(thu_muc):
    """Tep BEN NGOAI co nhac toi thu muc he cu."""
    can = _chuan(thu_muc)
    ra = []
    for goc, _, tep in os.walk(S):
        for t in tep:
            if not t.lower().endswith(".lua"):
                continue
            p = os.path.join(goc, t)
            rel = os.path.relpath(p, S)
            if _chuan(rel).startswith(can):
                continue
            try:
                d = doc(p)
            except OSError:
                continue
            for i, l in enumerate(d.replace("\r\n", "\n").split("\n"), 1):
                if l.strip().startswith("--"):
                    continue
                if can in _chuan(l):
                    ra.append((rel, i, l.strip()[:110]))
                    break
    return ra


def ten_dinh_nghia_noi_khac(thu_muc):
    """Tap ten toan cuc duoc dinh nghia O NGOAI thu muc he cu.

    Vi sao can: 'main', 'no', 'OnTimer', 'OnCancel', 'NOW_END_SAY'... duoc dinh
    nghia o HANG TRAM tep. Neu khong loc, moi tep Include deu bi cham 'co dung'
    -> danh sach viec phong to gia. Chi ten CHI RIENG he cu moi la phu thuoc
    that.
    """
    can = _chuan(thu_muc)
    ra = set()
    for goc, _, tep in os.walk(S):
        for t in tep:
            if not t.lower().endswith(".lua"):
                continue
            p = os.path.join(goc, t)
            if _chuan(os.path.relpath(p, S)).startswith(can):
                continue
            try:
                d = bo_chu_thich(doc(p))
            except OSError:
                continue
            for m in RE_HAM.finditer(d):
                ra.add(m.group(1))
            for m in RE_BANG.finditer(d):
                ra.add(m.group(1))
    return ra


def main():
    f = io.open(RA, "w", encoding="utf-8", newline="")

    def ra(s=""):
        print(s)
        f.write(s + "\n")

    ra("DO PHU THUOC THAT SU VAO CAC HE CU")
    ra("Sinh boi ReverseTools/cauhinh/t36_do_phu_thuoc_he_cu.py")
    ra("=" * 78)
    for thu_muc, ten_he in HE:
        p0 = os.path.join(S, thu_muc.replace("/", os.sep))
        if not os.path.isdir(p0):
            continue
        tc_tho = ten_toan_cuc(thu_muc)
        ngoai = ten_dinh_nghia_noi_khac(thu_muc)
        tc = dict((k, v) for k, v in tc_tho.items() if k not in ngoai)
        ds = tep_include(thu_muc)
        ra("")
        ra("#" * 78)
        ra("# %s   (%s)" % (ten_he, thu_muc))
        ra("#   %d ten toan cuc, trong do %d ten CHI RIENG he nay"
           % (len(tc_tho), len(tc)))
        ra("#   (%d ten bi loai vi noi khac cung dinh nghia: main, no, OnTimer...)"
           % (len(tc_tho) - len(tc)))
        ra("#   %d tep ngoai co nhac toi" % len(ds))
        ra("#" * 78)
        sach, ban = [], []
        for rel, dong, noi_dung in ds:
            try:
                d = bo_chu_thich(doc(os.path.join(S, rel)))
            except OSError:
                continue
            dung = sorted(set(t for t in tc
                              if re.search(r"\b%s\b" % re.escape(t), d)))
            if dung:
                ban.append((rel, dong, noi_dung, dung))
            else:
                sach.append((rel, dong, noi_dung))
        ra("")
        ra("  [A] CHI Include, KHONG dung ten nao -> go dong Include la xong (%d)"
           % len(sach))
        for rel, dong, noi_dung in sach:
            ra("      %s:%d" % (rel, dong))
            ra("          %s" % noi_dung)
        ra("")
        ra("  [B] CO dung -> phai xu ly tay (%d)" % len(ban))
        for rel, dong, noi_dung, dung in ban:
            ra("      %s:%d  dung %d ten" % (rel, dong, len(dung)))
            ra("          %s" % noi_dung)
            ra("          -> %s" % ", ".join(dung[:14]))
            if len(dung) > 14:
                ra("             (+%d nua)" % (len(dung) - 14))
    f.close()
    print()
    print("=> da ghi %s" % RA)
    return 0


if __name__ == "__main__":
    sys.exit(main())
