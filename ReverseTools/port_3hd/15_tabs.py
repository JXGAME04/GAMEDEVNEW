import os,re,json
BS=chr(92)
ROOTS=[r'D:\ServerLinux\server1\script', r'D:\ServerLinux\server1\vng_script']
sc=json.load(open('scan2.json',encoding='utf-8'))
CL=json.load(open(r'D:\GAMEDEVNEW\ReverseTools\port_3hd\closure3.json',encoding='utf-8'))
closure_files={f:{e['rel'] for e in lst} for f,lst in CL.items()}

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

dot=sc['dot']
recv={}
for k,v in dot.items():
    r=k.split('.')[0].split(':')[0]
    for feat,rel,ln in v:
        recv.setdefault(r,{}).setdefault(feat,set()).add(rel)

names=set(recv)-{'self','tb','pData','tfn','lib'}
# find creation sites: "<name> = {" or "<name> = X" or "function <name>."
CRE={n:re.compile(r'(?<![A-Za-z0-9_.:])'+re.escape(n)+r'\s*=\s*[{A-Za-z_]') for n in names}
sites={}
for root in ROOTS:
    for dp,dn,fn in os.walk(root):
        for f in fn:
            if not f.lower().endswith('.lua'): continue
            p=os.path.join(dp,f)
            base=[r for r in ROOTS if p.startswith(r)][0]
            rel=p[len(base)+1:].replace(BS,'/')
            try: txt=degbk(open(p,'rb').read())
            except Exception: continue
            for n,rx in CRE.items():
                for ln,line in enumerate(txt.split(chr(10)),1):
                    if line.lstrip().startswith('--'): continue
                    if rx.search(line):
                        sites.setdefault(n,[]).append(rel+':'+str(ln))
                        break
bad=[]
for n in sorted(names):
    ss=sites.get(n,[])
    for feat,rels in recv[n].items():
        cf=closure_files[feat]
        inside=[s for s in ss if s.rsplit(':',1)[0] in cf]
        if not inside:
            bad.append((n,feat,len(dot and [1]),ss[:3],sorted(rels)[:2]))
print('=== BANG (table) bi goi phuong thuc ma NOI TAO KHONG NAM TRONG BAO DONG ===')
for n,feat,_,ss,rels in bad:
    print('  %-26s feat=%-11s tao_o=%s  goi_tai=%s'%(n,feat,ss if ss else 'KHONG THAY O DAU',rels))
print('tong',len(bad))
json.dump({'sites':sites},open('tabs.json','w',encoding='utf-8'),ensure_ascii=False)
