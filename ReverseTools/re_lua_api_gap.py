import os, re

SRC = r'D:\GAMEDEVNEW\Sources\Core\Src\ScriptFuns.cpp'
s = open(SRC, 'rb').read().decode('latin-1')
reg = set(re.findall(r'\{\s*"([A-Za-z_][A-Za-z0-9_]*)"\s*,\s*[A-Za-z_]', s))
for root, dirs, files in os.walk(r'D:\GAMEDEVNEW\Sources\Core\Src'):
    for f in files:
        if not f.endswith('.cpp'):
            continue
        p = os.path.join(root, f)
        if p == SRC:
            continue
        t = open(p, 'rb').read().decode('latin-1')
        reg |= set(re.findall(r'\{\s*"([A-Za-z_][A-Za-z0-9_]*)"\s*,\s*Lua[A-Za-z_]', t))
print('Ham Lua dang ky trong ma nguon du an:', len(reg))

LUA_KW = set('''and break do else elseif end false for function if in local nil not or repeat return then true until while
print type tostring tonumber format strfind strsub strlen strlower strupper strrep gsub sort getn tinsert tremove
table math string io os random randomseed floor ceil abs min max mod sqrt date time clock dofile assert error
next pairs ipairs setmetatable getmetatable rawget rawset unpack call foreach foreachi tag settag
openfile closefile read write writeto readfrom appendto remove rename tmpname seek gcinfo collectgarbage
strbyte strchar globals rawgetglobal rawsetglobal setglobal getglobal newtag copytagmethods settagmethod gettagmethod'''.split())

# ham do CHINH cay script cua ban Linux dinh nghia (ngoai leaguematch) -> khong phai ham engine
LIB_ROOTS = [r'D:\ServerLinux\server1\script']
lib_defs = set()
for R in LIB_ROOTS:
    for root, dirs, fs in os.walk(R):
        for f in fs:
            if f.lower().endswith('.lua'):
                p = os.path.join(root, f)
                try:
                    t = open(p, 'rb').read().decode('gbk', 'replace')
                except Exception:
                    t = open(p, 'rb').read().decode('latin-1')
                t = re.sub(r'--[^\n]*', '', t)
                for m in re.finditer(r'function\s+([A-Za-z_][A-Za-z0-9_.:]*)\s*\(', t):
                    lib_defs.add(m.group(1).split('.')[-1].split(':')[-1])

ROOTS = [r'D:\ServerLinux\server1\script\leaguematch',
         r'D:\ServerLinux\server1\script\missions\leaguematch']
calls = {}
nfile = 0
for R in ROOTS:
    for root, dirs, fs in os.walk(R):
        for f in fs:
            if not f.lower().endswith('.lua'):
                continue
            nfile += 1
            p = os.path.join(root, f)
            try:
                t = open(p, 'rb').read().decode('gbk', 'replace')
            except Exception:
                t = open(p, 'rb').read().decode('latin-1')
            t = re.sub(r'--[^\n]*', '', t)
            for m in re.finditer(r'(?<![.:\w])([A-Za-z_][A-Za-z0-9_]*)\s*\(', t):
                n = m.group(1)
                if n in LUA_KW:
                    continue
                calls.setdefault(n, []).append((p, t[:m.start()].count('\n') + 1))

print('Tep .lua leaguematch:', nfile, ' | ten duoc goi:', len(calls))
thieu = sorted(n for n in calls if n not in reg and n not in lib_defs)
print()
print('=== HAM ENGINE MA leaguematch GOI NHUNG DU AN CHUA CO ===')
print('TONG:', len(thieu))
for n in thieu:
    p, l = calls[n][0]
    print('  %-30s x%-4d %s:%d' % (n, len(calls[n]), os.path.relpath(p, r'D:\ServerLinux\server1\script').replace('\\', '/'), l))
