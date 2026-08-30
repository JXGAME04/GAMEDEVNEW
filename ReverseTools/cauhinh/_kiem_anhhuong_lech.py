# -*- coding: utf-8 -*-
"""Do anh huong THAT SU cua 35 dong lech giua cot ParticularType va chi so dong.

May tra theo CHI SO (KItemGenerator.CPP:1660 -> KBasPropTbl.cpp:1058), nen voi
moi ma {6,1,P} ma script dang dung, mon NGUOI CHOI NHAN duoc la dong o CHI SO P,
con nguoi viet script thi tra bang theo COT P. O vung lech, hai thu khac nhau.
"""
import io
import os
import re
import sys

sys.path.insert(0, os.path.join(r"D:\GAMEDEVNEW\ReverseTools", "viemde"))
from bangtxt import tcvn2uni  # noqa: E402

sys.stdout.reconfigure(encoding="utf-8", errors="backslashreplace")
SV = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server"
MS = os.path.join(SV, "settings", "item", "magicscript.txt")
S = os.path.join(SV, "script")

d = io.open(MS, "rb").read().decode("latin-1").replace("\r\n", "\n").split("\n")
theo_chiso = {}
theo_cot = {}
for k, l in enumerate(d[1:]):
    c = l.split("\t")
    if len(c) < 4 or not c[3].strip().isdigit():
        continue
    ten = tcvn2uni(c[0]).strip()
    theo_chiso[k] = ten
    theo_cot.setdefault(int(c[3]), ten)

lech = {k for k in theo_chiso
        if theo_cot.get(k) is not None and theo_cot[k] != theo_chiso[k]}
print("so chi so ma 'tra theo cot' va 'tra theo chi so' cho MON KHAC NHAU: %d"
      % len(lech))
print()

# 5 ma toi da nan trong t01
print("=== 5 ma da nan trong t01 - co nam trong vung lech khong ===")
for p, mong in ((3, "Đại Lực hoàn"), (6, "Phi Tốc hoàn"),
                (3211, "Thiên Sơn Thánh Thủy (đại)"),
                (4869, "Đồ Phổ Đằng Long Bội"),
                (4866, "Đằng Long Thạch - Hạ")):
    tc = theo_chiso.get(p, "(khong co)")
    print("  %-6d chi so -> %-34s | %s"
          % (p, tc[:34], "AN TOAN" if p not in lech else "!!! NAM TRONG VUNG LECH"))

# quet script: ma nao dang dung nam trong vung lech
print()
print("=== script dang dung ma nao roi vao vung lech ===")
mau = re.compile(r"\{\s*6\s*,\s*1\s*,\s*(\d+)")
mau2 = re.compile(r"AddItem\s*\(\s*6\s*,\s*1\s*,\s*(\d+)")
gap = {}
for goc, _, tep in os.walk(S):
    for t in tep:
        if not t.lower().endswith(".lua") or ".truoc_" in t:
            continue
        p = os.path.join(goc, t)
        try:
            x = io.open(p, "rb").read().decode("latin-1")
        except OSError:
            continue
        for i, l in enumerate(x.replace("\r\n", "\n").split("\n"), 1):
            if l.strip().startswith("--"):
                continue
            for m in list(mau.finditer(l)) + list(mau2.finditer(l)):
                v = int(m.group(1))
                if v in lech:
                    gap.setdefault(v, []).append(
                        "%s:%d" % (os.path.relpath(p, S), i))

if not gap:
    print("  (khong cho nao) - 35 dong lech hien KHONG anh huong script nao")
else:
    for v in sorted(gap):
        print("  ma %d: script tuong la %-30s | may tra ve %-30s"
              % (v, (theo_cot.get(v) or "?")[:30], (theo_chiso.get(v) or "?")[:30]))
        for x in gap[v][:4]:
            print("        %s" % x)
