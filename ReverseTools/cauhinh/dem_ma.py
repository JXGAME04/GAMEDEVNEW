# -*- coding: utf-8 -*-
"""dem_ma.py - dem so lan mot ten xuat hien trong MA THAT cua tep Lua.

Ly do co tep nay: trong phien 28-29/08 toi da BA LAN viet chot tu kiem cho
patcher ma dem CA chu nam trong chu thich do CHINH BAN VA chen vao, roi bao
dong gia va dung ca dot va. Tach ra dung chung de khong lap lai lan thu tu.

Bo:
  - chu thich dong  -- ...
  - chuoi "..." va '...'  (tuy chon, mac dinh BO - vi ten trong log khong phai
    la cho dung that)
KHONG bo chu thich khoi --[[ ]] (hiem trong cay script nay); neu can thi them.
"""
import re


def _bo_chuoi(l):
    l = re.sub(r'"[^"]*"', '""', l)
    l = re.sub(r"'[^']*'", "''", l)
    return l


def dem_ma(s, tu, ke_chuoi=False):
    """Dem `tu` (khop tron tu) trong ma that. ke_chuoi=True thi tinh ca trong chuoi."""
    mau = re.compile(r"\b%s\b" % re.escape(tu))
    n = 0
    for l in s.splitlines():
        j = l.find("--")
        if j >= 0:
            l = l[:j]
        if not ke_chuoi:
            l = _bo_chuoi(l)
        n += len(mau.findall(l))
    return n


def dong_ma(s, tu, ke_chuoi=False):
    """Tra ve danh sach so dong (1-based) co `tu` trong ma that."""
    mau = re.compile(r"\b%s\b" % re.escape(tu))
    ra = []
    for i, l in enumerate(s.splitlines(), 1):
        j = l.find("--")
        if j >= 0:
            l = l[:j]
        if not ke_chuoi:
            l = _bo_chuoi(l)
        if mau.search(l):
            ra.append(i)
    return ra
