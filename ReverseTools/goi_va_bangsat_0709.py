#!/usr/bin/env python
# -*- coding: ascii -*-
r"""
[BANGSAT2 07/09] Bang (dong bang / lam cham) nhu Linux: KHONG phu thuoc sat thuong bang > 0.
Linux ReceiveDamage 0x0808A6D1 -> 0x0808B1E0: sau o bang, neu chua bang (+0x1d8 <= 0) va v1 > 0 thi dat bang,
khong xet ket qua CalcDamage (0x08089C90 tra 1 khi min+max <= 0, chi tra 0 khi muc tieu chet/hoi sinh/khong hop le).
JX1 truoc day: if (CalcDamage(cold)) { bang } ma CalcDamage tra FALSE khi sat thuong <= 0 (bao ne) => khang het thi khong bang.
Sua KNpc.cpp (1 cho): goi CalcDamage lay ket qua, dat bang khi trung HOAC muc tieu con hop le (giu cac chan JX1 rieng: van tieu Owner).
Chay: python goi_va_bangsat_0709.py <goc cay nguon>   (doc/ghi latin-1)
"""
import sys, os, re, io

root = sys.argv[1] if len(sys.argv) > 1 else r"D:\GAMEDEVNEW"
p = os.path.join(root, "Sources", "Core", "Src", "KNpc.cpp")

def rd(f):
    with io.open(f, "r", encoding="latin-1", newline="") as h: return h.read()
def wr(f, s):
    with io.open(f, "w", encoding="latin-1", newline="") as h: h.write(s)
def hb(s): return sum(1 for ch in s if ord(ch) > 127)

s = rd(p); before = hb(s)

pat = re.compile(r"^\tif \(CalcDamage\(nLauncher, pTemp->nValue\[0\], pTemp->nValue\[2\], damage_cold, ([^\r\n]*)\)\)\r\n\t\{\r\n", re.M)
ms = pat.findall(s)
if len(ms) != 1: raise SystemExit("H1: neo if(CalcDamage cold) x %d" % len(ms))
def rep(m):
    args = m.group(1)
    return ("\t// [BANGSAT2 07/09] Linux ReceiveDamage 0x0808A6D1 -> 0x0808B1E0: dat bang KHONG phu thuoc sat thuong bang > 0\r\n"
            "\t// (CalcDamage Linux 0x08089C90 tra 1 khi min+max <= 0, chi tra 0 khi muc tieu chet / hoi sinh / khong hop le).\r\n"
            "\t// JX1 CalcDamage tra FALSE khi sat thuong <= 0 (bao ne) => truoc day bi khang het bang thi khong dong bang.\r\n"
            "\tBOOL bBangTrung = CalcDamage(nLauncher, pTemp->nValue[0], pTemp->nValue[2], damage_cold, " + args + ");\r\n"
            "\tif (bBangTrung || (m_Doing != do_death && m_Doing != do_revive && m_RegionIndex >= 0 && !Owner[0]))\r\n"
            "\t{\r\n")
s = pat.sub(rep, s, count=1)

old2 = "bang LUON con it nhat 23% thoi luong.\r\n\t\tif (m_FreezeState.nTime <= 0)\r\n"
new2 = "bang LUON con it nhat 23% thoi luong.\r\n\t\tif (m_FreezeState.nTime <= 0 && pTemp->nValue[1] > 0)\t// [BANGSAT2 07/09] Linux: v1 > 0 moi dat\r\n"
if s.count(old2) != 1: raise SystemExit("H2: neo x %d" % s.count(old2))
s = s.replace(old2, new2)

if hb(s) != before: raise SystemExit("KNpc.cpp: so byte cao lech")
wr(p, s)
print("OK KNpc.cpp H1 (CalcDamage cold -> bBangTrung), H2 (v1 > 0)")
