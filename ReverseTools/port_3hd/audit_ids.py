# -*- coding: utf-8 -*-
"""Kiem toan DINH DANH truoc khi thi cong (yeu cau 4 cua chu game):
moi task id / ladder id / mission id ma 3 hoat dong dung, tra TOAN BO cay script
JX1 dang chay + serverscript_jx2 + Sources de xem co ai dung chua.

Chay: python audit_ids.py > audit_ids.out.txt
"""
import io, os, re, sys

sys.stdout.reconfigure(encoding="utf-8", errors="replace")

JX1 = [r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script",
       r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\scriptjx2",
       r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\settings\task"]

# (loai, so, dung boi ban Linux)
IDS = [
    ("task", 1082, "satthu: chi so boss dang nhan (1..160)"),
    ("task", 1192, "satthu: ngay YYMMDD reset dem"),
    ("task", 1193, "satthu: so lan/ngay"),
    ("task", 1217, "satthu: tong tich luy (ladder)"),
    ("task", 1550, "vuotai: so luot con lai"),
    ("task", 1551, "vuotai: ngay tham gia"),
    ("task", 2636, "vuotai: rank lan 1 - ngay"),
    ("task", 2637, "vuotai: rank lan 1 - diem"),
    ("task", 2638, "vuotai: rank lan 2 - ngay"),
    ("task", 2639, "vuotai: rank lan 2 - diem"),
    ("task", 2641, "vuotai: Long Huyet Hoan - ngay"),
    ("task", 2642, "vuotai: Long Huyet Hoan - dem"),
    ("task", 4018, "vuotai: Long Huyet Hoan kha dung"),
    ("task", 2852, "vuotai: map nguoi choi (chuangguan30)"),
    ("task", 1505, "vuotai: F11? (bao cao 13)"),
    ("task", 3070, "phonglangdo: qua kinh mach /ngay (CheckTaskDaily)"),
    ("task", 2863, "phonglangdo: diem nang dong?"),
    ("ladder", 10119, "satthu: bang xep hang giet boss"),
    ("ladder", 10179, "vuotai: ladder so cap"),
    ("ladder", 10180, "vuotai: ladder cao cap"),
    ("ladder", 10235, "vuotai: bang xep hang ngay"),
    ("mission", 15, "phonglangdo"),
    ("mission", 22, "vuotai"),
]

pat_task = re.compile(rb"(?:GetTask|SetTask|nt_setTask|nt_getTask|SyncTaskValue|CheckTask|AddTaskDaily|CheckTaskDaily)\s*\(\s*(\d+)")
pat_ladder = re.compile(rb"Ladder_(?:NewLadder|GetLadderInfo|ClearLadder|List)\s*\(\s*(\d+)")
pat_const = {}

use_task = {}
use_ladder = {}
for root in JX1:
    for dp, dn, fs in os.walk(root):
        for f in fs:
            if not f.lower().endswith((".lua", ".txt")):
                continue
            p = os.path.join(dp, f)
            try:
                d = open(p, "rb").read()
            except Exception:
                continue
            rel = p
            for m in pat_task.finditer(d):
                n = int(m.group(1))
                use_task.setdefault(n, []).append(rel)
            for m in pat_ladder.finditer(d):
                n = int(m.group(1))
                use_ladder.setdefault(n, []).append(rel)

print("| Loai | So | Ban Linux dung cho | JX1 dang dung? | Noi dung |")
print("|---|---|---|---|---|")
for kind, n, desc in IDS:
    if kind == "task":
        hits = use_task.get(n, [])
    elif kind == "ladder":
        hits = use_ladder.get(n, [])
    else:
        hits = []
    # loc trung + rut gon duong dan
    seen = []
    for h in hits:
        s = h.split("bin\\server\\")[-1]
        if s not in seen:
            seen.append(s)
    flag = "TRONG" if not seen else ("%d tep" % len(seen))
    print("| %s | %s | %s | %s | %s |" % (kind, n, desc, flag, "; ".join(seen[:4])))

# tim dai task TRONG lien tiep quanh 1100-1300 va 4100+ de co cho remap
all_used = set(use_task)
print()
print("Dai TRONG lien tiep de remap (1100-1400):")
run = []
for n in range(1100, 1400):
    if n not in all_used:
        run.append(n)
    else:
        if len(run) >= 4:
            print("  %d-%d (%d so)" % (run[0], run[-1], len(run)))
        run = []
print("Dai TRONG 4100-4200:")
run = []
for n in range(4100, 4200):
    if n not in all_used:
        run.append(n)
    else:
        if len(run) >= 4:
            print("  %d-%d (%d so)" % (run[0], run[-1], len(run)))
        run = []
if len(run) >= 4:
    print("  %d-%d (%d so)" % (run[0], run[-1], len(run)))
