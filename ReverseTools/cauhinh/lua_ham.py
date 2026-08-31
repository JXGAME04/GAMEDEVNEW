# -*- coding: utf-8 -*-
"""lua_ham.py - tim RANH GIOI mot ham Lua 4 theo can bang tu khoa.

Lua 4 khong co dau ngoac khoi, chi co function/then/do ... end. Muon xoa tron
mot ham thi phai dem can bang - dem tay la sai. Dung chung cho cac cong cu go
he cu.
"""
import re

TU_MO = re.compile(r"\b(function|then|do)\b")
TU_DONG = re.compile(r"\bend\b")
ELSEIF = re.compile(r"\belseif\b")


def sach(s):
    """Bo comment va chuoi - de dem tu khoa trong MA THAT."""
    s = re.sub(r"--\[\[.*?\]\]", " ", s, flags=re.S)
    s = re.sub(r"--[^\n]*", " ", s)
    s = re.sub(r'"[^"\n]*"', '""', s)
    s = re.sub(r"'[^'\n]*'", "''", s)
    return s


def can_bang(s):
    t = sach(s)
    return (len(TU_MO.findall(t)) - len(ELSEIF.findall(t))
            - len(TU_DONG.findall(t)))


def tim_khoi(dong, neo_chuoi):
    """Tim khoi bat dau tu dong CHUA neo_chuoi cho toi 'end' can bang.

    Dung de go tron mot nhanh `if (...) then ... end`. Tra ve (i_dau, i_cuoi)
    hoac None neu neo khong khop dung mot dong.
    """
    chi_so = [i for i, l in enumerate(dong)
              if neo_chuoi in l and not l.lstrip().startswith("--")]
    if len(chi_so) != 1:
        return None
    dau = chi_so[0]
    cb = 0
    for i in range(dau, len(dong)):
        cb = cb + can_bang(dong[i])
        if cb == 0:
            return (dau, i)
    return None


def tim_ham(dong, ten):
    """Tra ve (i_dau, i_cuoi) chi so 0-based trong danh sach dong, hoac None.

    i_cuoi la dong chua 'end' dong ham. Duyet tu dong khai bao, cong don can
    bang cho toi khi ve 0.
    """
    neo = re.compile(r"^\s*function\s+%s\s*\(" % re.escape(ten))
    dau = None
    for i, l in enumerate(dong):
        if neo.match(l) and not l.lstrip().startswith("--"):
            if dau is not None:
                return None          # trung ten - khong doan
            dau = i
    if dau is None:
        return None
    cb = 0
    for i in range(dau, len(dong)):
        cb = cb + can_bang(dong[i])
        if cb == 0 and i > dau:
            return (dau, i)
        if cb == 0 and i == dau:
            return (dau, i)          # ham mot dong
    return None
