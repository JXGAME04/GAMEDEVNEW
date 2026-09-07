import sys,re
keep=re.compile(r'\b(call|cmp|test|j[a-z]+|idiv|imul|ret)\b|\[e[a-z]{2} \+ 0x(1[0-9a-f]{3}|[2-9a-f][0-9a-f]{2})\]|dword ptr \[esp( \+ \w+)?\], (0x[0-9a-f]+|\d+)$|0x836eae0|0x8fc81e0')
for f in sys.argv[1:]:
    print("=====",f)
    for l in open(f):
        l=l.rstrip()
        if l.startswith('--'): print(l); continue
        parts=l.split(None,2)
        if len(parts)<3: continue
        ins=parts[2]
        if keep.search(ins) and not ins.startswith('mov dword ptr [ebp -') and not re.match(r'mov e[a-z]{2}, dword ptr \[ebp',ins):
            print(parts[0], ins)
