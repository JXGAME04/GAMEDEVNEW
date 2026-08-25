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
agk=set(ag)

print('=== A. Ten bi goi, KHONG co trong 04, KHONG stdlib/keyword, KHONG script dinh nghia o dau ca ===')
undef=[]
for k,v in calls.items():
    if k in agk or k in KW or k in STD: continue
    if k in defs: continue
    undef.append(k)
for k in sorted(undef):
    print('  %-30s luamap=%-12s jx1=%-4s n=%d  %s'%(k, lm.get(k,'-'), 'YES' if k in reg else 'NO', len(calls[k]), calls[k][:2]))

print()
print('=== B. Ten trong luamap, JX1 KHONG co, KHONG trong 04, co dinh nghia script NHUNG dinh nghia NGOAI bao dong ===')
for k,v in calls.items():
    if k in KW or k in STD: continue
    if k not in lm: continue
    if k in reg: continue
    ds=defs.get(k,[])
    # per-feature check
    for feat in sorted({c[0] for c in v}):
        cf=closure_files[feat]
        inside=[d for d in ds if d.split(':')[0] in cf]
        if not inside:
            print('  %-28s feat=%-12s luamap=%s  defs_outside=%s  in04=%s'%(k,feat,lm[k],ds[:3] if ds else 'NONE', k in agk))

print()
print('=== C. Ham nhom B cua 04: kiem lai dinh nghia script co che khong ===')
B=[k for k,v in ag.items() if v.get('nhom')=='B']
for k in B:
    ds=defs.get(k,[])
    if ds: print('  %-28s DEFS: %s'%(k,ds[:5]))
