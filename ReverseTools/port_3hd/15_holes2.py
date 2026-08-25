import os,re,json
sc=json.load(open('scan2.json',encoding='utf-8'))
ag=json.load(open(r'D:\GAMEDEVNEW\ReverseTools\port_3hd\04_api_gap.json',encoding='utf-8'))
reg=json.load(open('jx1_reg.json',encoding='utf-8'))
defs=json.load(open('defs.json',encoding='utf-8'))
CL=json.load(open(r'D:\GAMEDEVNEW\ReverseTools\port_3hd\closure3.json',encoding='utf-8'))
lm={}
for l in open(r'D:\GAMEDEVNEW\ReverseTools\jx_linux_y.luamap.full.txt',encoding='utf-8',errors='replace'):
    p=l.split()
    if len(p)>=2 and p[0].startswith('0x'): lm[p[1]]=p[0]
KW=set('and break do else elseif end for function if in local nil not or repeat return then until while true false'.split())
STD=set('''abs acos asin atan atan2 call ceil clock closefile collectgarbage copytagmethods cos date deg difftime
dofile dostring error execute exit exp floor flush foreach foreachi format frexp gcinfo getenv getglobal getn
getntagmethods getstring gettagmethod globals gsub ldexp log log10 max min mod newtag next nextvar openfile print
rad random randomseed rawget rawgetglobal rawset rawsetglobal read readfrom remove rename seek setglobal setlocale
settag settagmethod sin sort sqrt strbyte strchar strfind strlen strlower strrep strsub strupper tag tan tinsert
tonumber tostring tremove type unpack write writeto appendto assert _ALERT _ERRORMESSAGE'''.split())
closure_files={f:{e['rel'] for e in lst} for f,lst in CL.items()}
calls=sc['calls']

print('=== D. Ten SCRIPT dinh nghia nhung dinh nghia NGOAI bao dong cua tinh nang goi no (phai keo tep theo) ===')
rows={}
for k,v in calls.items():
    if k in KW or k in STD: continue
    ds=defs.get(k,[])
    if not ds: continue
    for feat in sorted({c[0] for c in v}):
        cf=closure_files[feat]
        inside=[d for d in ds if d.rsplit(':',2)[0] in cf]
        if not inside:
            rows.setdefault(k,{})[feat]=(ds[:3], k in lm, k in reg)
for k in sorted(rows):
    for feat,(ds,inlm,injx1) in rows[k].items():
        print('  %-26s feat=%-11s luamap=%-5s jx1=%-4s defs=%s'%(k,feat,'Y' if inlm else 'N','Y' if injx1 else 'N',ds))
print('TONG ten:',len(rows))

print()
print('=== E. Ten nhan phuong thuc (a:b / a.b) - bang goc co trong bao dong khong ===')
dot=sc['dot']
recv={}
for k,v in dot.items():
    r=k.split('.')[0].split(':')[0]
    recv.setdefault(r,0)
    recv[r]+=len(v)
for r,n in sorted(recv.items(), key=lambda x:-x[1])[:40]:
    print('  %-28s n=%-5d  script_def=%s  luamap=%s  jx1=%s'%(r,n, 'Y' if r in defs else 'N', 'Y' if r in lm else 'N', 'Y' if r in reg else 'N'))

print()
print('=== F. upvalue %ten ===')
print(json.dumps(sc['up'],ensure_ascii=False))
