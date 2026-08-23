import re, os, sys, types
RT = r'D:\GAMEDEVNEW\ReverseTools'
src = open(os.path.join(RT, 'api_gap2.py'), encoding='utf-8').read()
src = src.replace('\nmain()\n', '\n')
A = types.ModuleType('A')
exec(compile(src, 'api_gap2', 'exec'), A.__dict__)
reg = A.project_registered()
S = r'D:\ServerLinux\server1\script'
files = [os.path.join(S, p) for p in [
    r'vng_event\give_support_item.lua',
    r'vng_lib\bittask_lib.lua',
    r'item\huangzhendan.lua',
    r'event\BeiDouChuanGong\head.lua',
    r'vng_event\item\biggoldenseed.lua',
    r'missions\tongwar\npc\chefu.lua',
    r'missions\tongwar\npc\doctor.lua',
    r'missions\tongwar\npc\openbox.lua',
    r'missions\tongwar\match\smalltimer.lua',
    r'missions\tongwar\match\totaltimer.lua',
    r'missions\tongwar\match\hometrap1.lua',
    r'missions\tongwar\match\homeouttrap1.lua',
    r'event\tongwar\tongwar_gmscript.lua',
    r'event\tongwar\headinfo.lua',
    r'event\tongwar\head.lua',
    r'event\tongwar\npc_shizhe.lua',
    r'event\tongwar\event.lua',
    r'event\tongwar\score.lua',
    r'missions\tongwar\match\head.lua',
    r'missions\tongwar\match\mission.lua',
    r'missions\tongwar\trap\tongwar_trap.lua',
    r'missions\tongwar\tongwar_autoexec.lua',
    r'missions\tongwar\tongwar_main.lua',
]]
files.append(r'D:\ServerLinux\gateway\s3relay\relaysetting\task\tongwar.lua')
elf = set()
for l in open(A.ELF, encoding='utf-8', errors='replace'):
    p = l.split()
    if len(p) >= 2 and p[0].startswith('0x'):
        elf.add(p[1])
defs = {}
for f in files:
    if not os.path.exists(f):
        print('MISSING FILE', f); continue
    t = re.sub(r'--[^\n]*', '', A.read(f))
    for m in re.finditer(r'function\s+([A-Za-z_][A-Za-z0-9_.:]*)\s*\(', t):
        defs.setdefault(m.group(1).split('.')[-1].split(':')[-1], []).append(f)
    for m in re.finditer(r'([A-Za-z_][A-Za-z0-9_]*)\s*=\s*function\s*\(', t):
        defs.setdefault(m.group(1), []).append(f)
# lib cua du an (script\ + scriptjx2\) dinh nghia
PR = r'E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server'
prjdefs = set()
for root in (os.path.join(PR, 'script'), os.path.join(PR, 'scriptjx2')):
    for dp, _, fs in os.walk(root):
        for f in fs:
            if f.lower().endswith('.lua'):
                t = A.read(os.path.join(dp, f))
                for m in re.finditer(r'function\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(', t):
                    prjdefs.add(m.group(1))
for f in files:
    if not os.path.exists(f):
        continue
    t = re.sub(r'--[^\n]*', '', A.read(f))
    t2 = re.sub(r'"[^"\n]*"', '""', t)
    calls = set(re.findall(r'(?<![\w.:])([A-Za-z_][A-Za-z0-9_]*)\s*\(', t2))
    miss = [c for c in sorted(calls) if c not in reg and c not in A.LUA_KW and c not in defs]
    eng = [c for c in miss if c in elf]
    oth = [c for c in miss if c not in elf]
    oth2 = [c + ('(prj-lua)' if c in prjdefs else '(?)') for c in oth]
    print(os.path.relpath(f, S) if f.startswith(S) else f)
    print('   ENG thieu:', eng)
    print('   khac:', oth2)
