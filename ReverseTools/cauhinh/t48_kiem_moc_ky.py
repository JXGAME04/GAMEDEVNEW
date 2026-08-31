# -*- coding: utf-8 -*-
"""t48_kiem_moc_ky.py - kiem KY 6 moc hoat dong: quet ca TEN va SO HIEU.

t47 chi tim dang SetTask(TEN_TASK...). Thieu cac dang khac:
    SetTask(315, ...)                    ghi bang SO
    nt_setTask(43, ...)                  ham boc cua he moi
    CallPlayerFunction(idx, SetTask, X)  ghi cho nguoi khac
    gb_SetTask / SetTaskTemp / AddTask   bien the
Bao thieu la ket luan SAI ("khong ai ghi" trong khi co). Cong cu nay quet het.

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

MOC = [
    ("1", "PLD",       "TSK_NUMPLDNGAY",  "315"),
    ("2", "VA",        "T_NVVATRONGNGAY", "43"),
    ("3", "VanTieu",   "T_NVVTTRONGNGAY", "49"),
    ("4", "HangNgay",  "TASK_TANTHU3",    "345"),
    ("5", "Vip",       "TASK_NEWTHOREN6", "377"),
    ("6", "DaTau",     "T_SoNVTrongNgay", "87"),
]

HE_MOI = ["missions\\challengeoftime", "missions\\fengling_ferry",
          "missions\\citywar", "missions\\leaguematch", "leaguematch\\",
          "task\\newtask", "task\\tollgate", "global\\seasonnpc.lua",
          "missions\\tong", "missions\\bw", "missions\\tongcastle",
          "missions\\bairenleitai", "tinhnang\\3hoatdong",
          "missions\\yandibaozang", "vng_feature"]
HE_CU = ["tinhnang\\datau", "tinhnang\\vuot_ai", "tinhnang\\phonglangdo",
         "tinhnang\\boss_satthu", "tinhnang\\loidai",
         "tinhnang\\congthanhchien"]


def doc(p):
    return io.open(p, "rb").read().decode("latin-1")


def loai(rel):
    r = rel.replace("/", "\\").lower()
    for x in HE_MOI:
        if x.lower() in r:
            return "MOI"
    for x in HE_CU:
        if x.lower() in r:
            return "CU"
    return "chung"


def main():
    for so, ten, ten_task, sh in MOC:
        # moi dang GHI: ...SetTask( <ten hoac so> ...
        pat = re.compile(
            r"(?:SetTask|setTask|SetTaskTemp|AddTask)\s*\(\s*(?:%s|%s)\s*[,)]"
            % (re.escape(ten_task), sh))
        # dang CallPlayerFunction(idx, SetTask, TASK, ...)
        pat2 = re.compile(
            r"CallPlayerFunction\s*\([^,]+,\s*SetTask\s*,\s*(?:%s|%s)\s*[,)]"
            % (re.escape(ten_task), sh))
        ra = []
        for goc, _, tep in os.walk(S):
            for t in tep:
                if not t.lower().endswith(".lua") or ".truoc_" in t \
                        or t.endswith(".goc"):
                    continue
                p = os.path.join(goc, t)
                rel = os.path.relpath(p, S)
                try:
                    dong = doc(p).replace("\r\n", "\n").split("\n")
                except OSError:
                    continue
                for i, l in enumerate(dong, 1):
                    if l.strip().startswith("--"):
                        continue
                    if pat.search(l) or pat2.search(l):
                        # bo dong reset ve 0 trong lib_ham (khong phai "ghi luot")
                        if rel.lower().endswith("lib_ham.lua") \
                                and re.search(r",\s*0\s*\)", l):
                            continue
                        ra.append((rel, i, l.strip()[:92], loai(rel)))
        ls = sorted(set(x[3] for x in ra))
        print("[%s] %-9s task %-18s so %-4s  ->  %d cho ghi %s"
              % (so, ten, ten_task, sh, len(ra),
                 ("(" + "/".join(ls) + ")") if ls else ""))
        for x in ra[:8]:
            print("      [%-5s] %s:%d | %s" % (x[3], x[0], x[1], x[2]))
        if len(ra) > 8:
            print("      (+%d cho nua)" % (len(ra) - 8))
        print()
    return 0


if __name__ == "__main__":
    sys.exit(main())
