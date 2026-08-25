# -*- coding: utf-8 -*-
"""Bo giai ma DONG-THEO-DONG ban vá (v2) cho cay script Linux.

VI SAO CAN v2 — loi that do vong 2 tim ra:
  `gbktool.detect()` coi mot dong la TCVN3 chi khi MOI byte cao nam trong bang 74 byte
  TCVN3 (0xA1-0xFE). Nhung nhieu dong tieng Viet con lan **dau nhay thong minh cua CP1252**
  (0x93 " , 0x94 " , 0x85 ... , 0x91/0x92 ' ' , 0x96/0x97 - -) do soan bang Word.
  Chi mot byte nhu vay la ca dong bi doan nham sang GBK va giai ra chu Han vun.
  Vi du: nieshichen.lua:21 va :48 (toan bo luat Vuot Ai), mibao_head.lua:21.

CACH VA: coi dong la TCVN3 neu moi byte cao nam trong  TCVN3 ∪ {dau cau CP1252},
va giai rieng nhom dau cau do bang CP1252.

Dung nhu thu vien:  from dec2 import decline2, detect2
"""
import os, sys

sys.path.insert(0, r"D:\GAMEDEVNEW\ReverseTools")
from gbktool import _T2U, _TSET, runs  # noqa

# dau cau CP1252 hay lot vao tep TCVN3 (soan tren Word)
_PUNCT = {
    0x82: "\u201a", 0x84: "\u201e", 0x85: "\u2026", 0x8B: "\u2039",
    0x91: "\u2018", 0x92: "\u2019", 0x93: "\u201c", 0x94: "\u201d",
    0x95: "\u2022", 0x96: "\u2013", 0x97: "\u2014", 0x9B: "\u203a",
}
_TSET2 = _TSET | set(_PUNCT)


def detect2(b):
    hi = [x for x in b if x > 127]
    if not hi:
        return "ascii"
    if all(x in _TSET2 for x in hi) and max(runs(b)) <= 3:
        return "tcvn3"
    return "gbk"


def _tcvn3_dec2(b):
    out = []
    for c in b:
        if c in _T2U:
            out.append(_T2U[c])
        elif c in _PUNCT:
            out.append(_PUNCT[c])
        else:
            out.append(chr(c))
    return "".join(out)


def _dec_one(b):
    e = detect2(b)
    if e == "gbk":
        return b.decode("gbk", errors="replace")
    if e == "tcvn3":
        return _tcvn3_dec2(b)
    return b.decode("latin-1")


# Byte dung de CAT DOAN. An toan tuyet doi vi:
#   - chu GBK: byte dan 0x81-0xFE, byte theo 0x40-0xFE  -> khong bao gio chua 0x22 hay 0x2D
#   - chu TCVN3: 0xA1-0xFE (+ dau cau 0x82-0x9B)        -> cung khong chua
# Cat roi noi lai bang chinh ky tu do => khong mat mot byte nao.
_SPLIT = b'"-'


def decline2(b):
    """Giai ma MOT DONG co the tron HAI bang ma.

    Vi du that (mibao_head.lua:21): ve trai la chuoi tieng Viet TCVN3,
    ve phai sau '--' la chu thich chu Han GBK. Doan theo DONG se hong mot ve.
    Nen cat dong thanh doan tai cac ky tu '"' va '-' roi doan ma hoa TUNG DOAN.
    """
    if not any(x > 127 for x in b):
        return b.decode("latin-1")
    out, seg = [], bytearray()
    for c in b:
        if c in _SPLIT:
            if seg:
                out.append(_dec_one(bytes(seg)))
                seg = bytearray()
            out.append(chr(c))
        else:
            seg.append(c)
    if seg:
        out.append(_dec_one(bytes(seg)))
    return "".join(out)


if __name__ == "__main__":
    # do xem bao nhieu dong trong ca cay bi ban v1 doan nham
    from gbktool import detect as detect1
    root = sys.argv[1] if len(sys.argv) > 1 else r"D:\ServerLinux\server1\script"
    nf = nl = nbad = 0
    badfiles = {}
    for dp, dn, fs in os.walk(root):
        for f in fs:
            if not f.lower().endswith((".lua", ".txt", ".ini")):
                continue
            p = os.path.join(dp, f)
            nf += 1
            for i, ln in enumerate(open(p, "rb").read().split(b"\n"), 1):
                ln = ln.rstrip(b"\r")
                nl += 1
                if detect1(ln) != detect2(ln):
                    nbad += 1
                    badfiles.setdefault(p, []).append(i)
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    print("quet %d tep / %d dong -> %d dong bi ban v1 doan NHAM (%d tep)"
          % (nf, nl, nbad, len(badfiles)))
    for p in sorted(badfiles, key=lambda x: -len(badfiles[x]))[:25]:
        print("  %-95s %d dong" % (p, len(badfiles[p])))
