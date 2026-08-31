# -*- coding: utf-8 -*-
"""t41_quet_menu_chet.py - quet MOI muc menu tro toi ham CHI CO o thu vien he cu.

Sau khi go Include cua he cu, cac muc menu dang "Nhan hien/ten_ham" van con thi
bam vao la goi mot ham KHONG TON TAI. Cong cu nay tim het chung.

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

S = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
CU = ["tinhnang/vuot_ai", "tinhnang/phonglangdo", "tinhnang/boss_satthu",
      "tinhnang/loidai", "tinhnang/loidaihonchien", "tinhnang/congthanhchien",
      "tinhnang/datau"]

RE_HAM = re.compile(r"^\s*function\s+([A-Za-z_]\w*)", re.M)
RE_MUC = re.compile(r'"[^"]*/([A-Za-z_]\w*)"')


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def chuan(s):
    return re.sub(r"\\+", "\\\\", s.replace("/", "\\").lower())


def main():
    can = [chuan(x) for x in CU]
    trong, ngoai = set(), set()
    for goc, _, tep in os.walk(S):
        for t in tep:
            if not t.lower().endswith(".lua"):
                continue
            p = os.path.join(goc, t)
            rel = chuan(os.path.relpath(p, S))
            tap = trong if any(rel.startswith(c) for c in can) else ngoai
            try:
                d = re.sub(r"--[^\n]*", " ", doc(p))
            except OSError:
                continue
            for m in RE_HAM.finditer(d):
                tap.add(m.group(1))
    rieng = trong - ngoai
    print("ham CHI CO o cac thu vien he cu: %d" % len(rieng))
    print()

    n = 0
    for goc, _, tep in os.walk(S):
        for t in tep:
            if not t.lower().endswith(".lua"):
                continue
            p = os.path.join(goc, t)
            rel = os.path.relpath(p, S)
            if any(chuan(rel).startswith(c) for c in can):
                continue
            try:
                dong = doc(p).replace("\r\n", "\n").split("\n")
            except OSError:
                continue
            for i, l in enumerate(dong, 1):
                if l.strip().startswith("--"):
                    continue
                for m in RE_MUC.finditer(l):
                    if m.group(1) in rieng:
                        print("  %s:%d" % (rel, i))
                        print("      %s" % l.strip()[:110])
                        n = n + 1
    print()
    print("=> %d muc menu tro toi ham cua he cu (bam vao se goi ham nil)" % n)
    return 0


if __name__ == "__main__":
    sys.exit(main())
