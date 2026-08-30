# -*- coding: utf-8 -*-
"""Kiem bay TRUNG TEN BIEN TOAN CUC giua lib_ctc.lua va lib_tktc.lua.

Hai tep cung khai TAB_QUANHAM nhung KHAC CAU TRUC:
    lib_ctc.lua:158   {100},{200},{400},{600},{800}      -> MOT cot
    lib_tktc.lua:121  {0,1000,100},{1000,3000,200},...   -> BA cot
Nhieu tep doc TAB_QUANHAM[i][2]. Neu trong mot Lua state ma ban MOT COT thang
(tuc lib_ctc duoc Include SAU lib_tktc) thi [2] tra nil => loi hoac so sanh sai.

Tep nay tim moi .lua co Include CA HAI, va cho biet ban nao thang.
"""
import io
import os
import re
import sys

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"

CTC = "congthanhchien\\lib_ctc.lua"
TKTC = "tong_kim_tcap\\lib_tktc.lua"

ca_hai = []
chi_ctc = []
chi_tktc = []
doc_cot2 = []

for goc, _, tep in os.walk(S):
    for t in tep:
        if not t.lower().endswith(".lua") or ".truoc_" in t:
            continue
        p = os.path.join(goc, t)
        try:
            d = io.open(p, "rb").read().decode("latin-1").replace("\r\n", "\n")
        except OSError:
            continue
        rel = os.path.relpath(p, S)
        # vi tri dong Include (bo dong da comment)
        i_ctc = i_tktc = -1
        for i, l in enumerate(d.split("\n")):
            if l.strip().startswith("--"):
                continue
            if "Include" in l and "lib_ctc.lua" in l:
                i_ctc = i
            if "Include" in l and "lib_tktc.lua" in l:
                i_tktc = i
        co_cot2 = re.search(r"TAB_QUANHAM\s*\[[^\]]+\]\s*\[\s*2\s*\]", d) is not None
        if co_cot2:
            doc_cot2.append((rel, i_ctc, i_tktc))
        if i_ctc >= 0 and i_tktc >= 0:
            ca_hai.append((rel, i_ctc, i_tktc))
        elif i_ctc >= 0:
            chi_ctc.append(rel)
        elif i_tktc >= 0:
            chi_tktc.append(rel)

print("Tep Include CA HAI thu vien: %d" % len(ca_hai))
for rel, a, b in ca_hai:
    thang = "lib_tktc (3 cot) THANG - an toan" if b > a \
        else "lib_ctc (1 cot) THANG - [2] se la nil"
    print("  %-52s ctc@%-4d tktc@%-4d  %s" % (rel[:52], a + 1, b + 1, thang))

print()
print("Tep DOC TAB_QUANHAM[..][2]: %d" % len(doc_cot2))
for rel, a, b in doc_cot2:
    if a >= 0 and b >= 0:
        tt = "ca hai (tktc %s)" % ("sau - OK" if b > a else "TRUOC - HONG")
    elif b >= 0:
        tt = "chi lib_tktc - OK"
    elif a >= 0:
        tt = "chi lib_ctc - HONG ([2] nil)"
    else:
        tt = "KHONG Include ban nao - phu thuoc noi goi"
    print("  %-52s %s" % (rel[:52], tt))

print()
print("Tep chi Include lib_ctc : %d" % len(chi_ctc))
print("Tep chi Include lib_tktc: %d" % len(chi_tktc))
