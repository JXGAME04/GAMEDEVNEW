# -*- coding: utf-8 -*-
r"""Tim khoang TASK ID con TRONG (MAX_TASK = 4200 => hop le 0..4199).
Chi lay so trong NGU CANH task: SetTask/GetTask/nt_setTask/AddTask/... va cac hang
dat ten kieu  TEN = <so>  nam trong tep co dinh nghia task."""
import io, os, re

E = r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script"
CALL = re.compile(r"\b(?:nt_setTask|nt_getTask|SetTask|GetTask|AddTask|SetTaskTemp|GetTaskTemp|"
                  r"AddTaskDaily\w*|GetTaskDaily\w*|get_task_daily|add_task_daily|SetTaskBit|GetTaskBit|"
                  r"SyncTaskValue|SetByte|GetByte|SetNumber|GetNumber)\s*\(\s*(\d{1,4})\b")
# hang so dat ten: chi nhan khi ten goi y task
CONST = re.compile(r"^\s*(T_\w+|TSK_\w+|TASK_\w+|MS_\w+|\w*TASKID\w*|\w*_TASK\w*)\s*=\s*(\d{1,4})\s*(?:;|--|$)",
                   re.M)

used = {}
nfile = 0
for dp, _, fs in os.walk(E):
    for f in fs:
        if not f.lower().endswith(".lua"): continue
        if f.lower() in ("codenew.lua", "npcpos.lua"): continue
        p = os.path.join(dp, f)
        try: s = io.open(p, "r", encoding="latin-1", newline="").read()
        except Exception: continue
        nfile += 1
        body = "\n".join(l for l in s.split("\n") if not l.lstrip().startswith("--"))
        for m in CALL.finditer(body):
            used.setdefault(int(m.group(1)), set()).add(f)
        for m in CONST.finditer(body):
            used.setdefault(int(m.group(2)), set()).add(f)

print("quet %d tep, %d task id dang dung" % (nfile, len(used)))

MAXT = 4200
free, run = [], None
for i in range(1, MAXT):
    if i not in used:
        if run is None: run = i
    else:
        if run is not None and i - run >= 5:
            free.append((run, i - 1))
        run = None
if run is not None and MAXT - run >= 5:
    free.append((run, MAXT - 1))

print("\n--- CAC KHOANG TRONG >= 5 id (id lon nhat truoc) ---")
for a, b in sorted(free, key=lambda x: -x[0])[:10]:
    print("   %4d - %4d   (%d id)" % (a, b, b - a + 1))

print("\n--- kiem tra rieng vai id ---")
for i in (250, 251, 1205, 1218, 3399, 4061):
    print("   %4d: %s" % (i, ", ".join(sorted(used.get(i, set()))[:3]) or "TRONG"))
