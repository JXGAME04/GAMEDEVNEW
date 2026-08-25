# -*- coding: utf-8 -*-
"""BO SOT #2 cua A4: ID NHIEM VU (task id) cua 101 tep port co dung chung voi he JX1 khong."""
import sys, os, io, re
sys.path.insert(0, r'D:\GAMEDEVNEW\ReverseTools\port_3hd')
sys.stdout.reconfigure(encoding='utf-8', errors='replace')
from dec2 import decline2

JX1 = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server'
HERE = r'D:\GAMEDEVNEW\ReverseTools\port_3hd\thicong'
man = set(x.strip().lower() for x in io.open(os.path.join(HERE, 'b1_manifest.txt'), encoding='utf-8').read().split('\n') if x.strip())
NEWFILES = set(x.lower() for x in [
    r'script\tinhnang\3hoatdong\hd3_driver.lua',
    r'script\item\hd3_admin.lua',
    r'script\global\autoexec_npc_hd3.lua',
    r'script\missions\fengling_ferry\hd3_thuyenphu.lua',
])
PORT = man | NEWFILES

# thu thap SetTask/GetTask/AddTask... voi so nguyen truc tiep
PAT = re.compile(r'\b(SetTask|GetTask|nt_setTask|nt_getTask|SetTaskTemp|GetTaskTemp|AddTaskValue)\s*\(\s*(\d+)')
port_ids = {}
jx1_ids = {}
for dp, dn, fs in os.walk(os.path.join(JX1, 'script')):
    for f in fs:
        if not f.lower().endswith('.lua'):
            continue
        p = os.path.join(dp, f)
        rel = os.path.relpath(p, JX1).lower()
        try:
            data = open(p, 'rb').read().split(b'\n')
        except Exception:
            continue
        tgt = port_ids if rel in PORT else jx1_ids
        for i, ln in enumerate(data, 1):
            s = ln.decode('latin-1')
            if s.strip().startswith('--'):
                continue
            for m in PAT.finditer(s):
                tgt.setdefault(int(m.group(2)), []).append('%s:%d' % (rel, i))

print('task id trong 101+4 tep PORT :', len(port_ids))
print('task id trong phan con lai JX1:', len(jx1_ids))
common = sorted(set(port_ids) & set(jx1_ids))
print()
print('== TASK ID DUNG CHUNG (port <-> he JX1 san co): %d ==' % len(common))
for t in common:
    print('  task %-6d  PORT: %-58s  JX1: %s' % (t, '; '.join(sorted(set(port_ids[t]))[:2])[:58], '; '.join(sorted(set(jx1_ids[t]))[:2])[:70]))
print()
print('== task id CHI co o PORT (moi) ==')
only = sorted(set(port_ids) - set(jx1_ids))
print(' ', only)
