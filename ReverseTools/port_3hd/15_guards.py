import os,re,json
BS=chr(92)
ROOTS=[r'D:\ServerLinux\server1\script', r'D:\ServerLinux\server1\vng_script']
ag=json.load(open(r'D:\GAMEDEVNEW\ReverseTools\port_3hd\04_api_gap.json',encoding='utf-8'))
sc=json.load(open('scan2.json',encoding='utf-8'))
B=[k for k,v in ag.items() if v.get('nhom')=='B']+['JoinMission']
calls=sc['calls']

def degbk(b):
    out=bytearray(); i=0; n=len(b)
    while i<n:
        c=b[i]
        if 0x81<=c<=0xFE and i+1<n and 0x40<=b[i+1]<=0xFE and b[i+1]!=0x7F:
            out+=b'  '; i+=2
        elif c>=0x80:
            out+=b' '; i+=1
        else:
            out.append(c); i+=1
    return out.decode('ascii')

cache={}
def readf(rel):
    if rel in cache: return cache[rel]
    for base in ROOTS:
        p=os.path.join(base,rel.replace('/',BS))
        if os.path.exists(p):
            t=degbk(open(p,'rb').read()).split(chr(10)); cache[rel]=t; return t
    cache[rel]=None; return None

print('%-24s %-6s %s'%('HAM','#SITE','CHO GOI (unique) + GUARD'))
out={}
for k in sorted(B):
    sites=sorted({(c[1],c[2]) for c in calls.get(k,[])})
    rows=[]
    for rel,ln in sites:
        t=readf(rel)
        line=t[ln-1].strip() if t and ln-1<len(t) else '??'
        guarded = bool(re.search(r'\bif\s*\(?\s*(not\s+)?'+re.escape(k)+r'\s*\)?\s*(then|~=|==)', line)) or \
                  bool(re.search(r'\bif\s+'+re.escape(k)+r'\s+then', line))
        rows.append((rel,ln,guarded,line[:110]))
    out[k]=rows
    print('%-24s %-6d'%(k,len(sites)))
    for rel,ln,g,line in rows:
        print('      %s%s:%d  %s'%('[GUARD] ' if g else '        ',rel,ln,line))
json.dump(out,open('guards.json','w',encoding='utf-8'),ensure_ascii=False,indent=0)
