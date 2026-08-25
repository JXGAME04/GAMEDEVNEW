import re,io,json,os
src=r'D:\GAMEDEVNEW\Sources'
files=[(r'Core\Src\ScriptFuns.cpp',None),(r'Core\Src\LuaFuns.cpp',None),(r'Engine\Src\LuaFuns.cpp',None)]
out={}
for rel,_ in files:
    p=os.path.join(src,rel)
    txt=open(p,'rb').read().decode('latin-1')
    lines=txt.split('\n')
    for i,l in enumerate(lines,1):
        m=re.search(r'\{\s*"([A-Za-z_][A-Za-z0-9_]*)"\s*,\s*([A-Za-z_][A-Za-z0-9_:]*)\s*\}',l)
        if m:
            out.setdefault(m.group(1),[]).append(f'{rel}:{i} -> {m.group(2)}')
print(len(out))
json.dump(out,open('jx1_reg.json','w',encoding='utf-8'),ensure_ascii=False,indent=0)
