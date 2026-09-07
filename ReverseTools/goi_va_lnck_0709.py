#!/usr/bin/env python
# -*- coding: ascii -*-
r"""
[LNCK 07/09] Khien tinh (staticmagicshield_p - Luong Nghi Chan Khi 721, Phat Y 2134, Tieu Dao Vu 2139) nhu Linux:
Linux CalcDamage 0x08089D5D-0x08089DD9 hap thu MOI sat thuong di qua CalcDamage, KHONG xet co bReturn
(nhip doc 0x0808BDF9 goi CalcDamage(type 4, [esp+0x24]=1) van bi khien chan; phan don type 6 cung vay).
JX1 truoc day: if (!bReturn && khien > 0 && dmg > 0) -> nhip DOC / CHAY (goi voi bReturn = TRUE) va sat thuong
phan don xuyen qua khien => nguoi choi dang co Luong Nghi Chan Khi van chet vi doc.
Chay: python goi_va_lnck_0709.py <goc cay nguon>   (doc/ghi latin-1)
"""
import sys, os, io

root = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW"
p = os.path.join(root, "Sources", "Core", "Src", "KNpc.cpp")

def rd(f):
    with io.open(f, "r", encoding="latin-1", newline="") as h: return h.read()
def wr(f, s):
    with io.open(f, "w", encoding="latin-1", newline="") as h: h.write(s)
def hb(s): return sum(1 for ch in s if ord(ch) > 127)

s = rd(p); before = hb(s)
old = "\t\tif (!bReturn && m_CurrentStaticMagicShieldP > 0 && nDamage > 0)\r\n"
new = ("\t\t// [LNCK 07/09] Linux CalcDamage 0x08089D5D: khien tinh chan MOI sat thuong, ke ca nhip doc/chay (bReturn = TRUE)\r\n"
       "\t\t// va sat thuong phan don. Ban cu bo qua khi bReturn => Luong Nghi Chan Khi con ma van chet vi doc.\r\n"
       "\t\tif (m_CurrentStaticMagicShieldP > 0 && nDamage > 0)\r\n")
if s.count(old) != 1: raise SystemExit("neo x %d" % s.count(old))
s = s.replace(old, new)
if hb(s) != before: raise SystemExit("KNpc.cpp: so byte cao lech")
wr(p, s)
print("OK KNpc.cpp: khien tinh khong xet bReturn")
