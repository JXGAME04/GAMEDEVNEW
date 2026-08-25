import os,re,json
BS=chr(92)
ROOTS=[r'D:\ServerLinux\server1\script', r'D:\ServerLinux\server1\vng_script']

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

DEFG=re.compile(r'(?<![A-Za-z0-9_.:])function\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(')
ASGF=re.compile(r'(?<![A-Za-z0-9_.:])([A-Za-z_][A-Za-z0-9_]*)\s*=\s*function\s*\(')
ALIAS=re.compile(r'(?<![A-Za-z0-9_.:])([A-Za-z_][A-Za-z0-9_]*)\s*=\s*([A-Za-z_][A-Za-z0-9_]*)\s*;?\s*(?:--.*)?$')

defs={}
nfile=0
for root in ROOTS:
    for dp,dn,fn in os.walk(root):
        for f in fn:
            if not f.lower().endswith('.lua'): continue
            p=os.path.join(dp,f)
            nfile+=1
            try: txt=degbk(open(p,'rb').read())
            except Exception: continue
            base=None
            for r in ROOTS:
                if p.startswith(r): base=r
            rel=p[len(base)+1:].replace(BS,'/')
            for ln,line in enumerate(txt.split(chr(10)),1):
                if line.lstrip().startswith('--'): continue
                for m in DEFG.finditer(line):
                    defs.setdefault(m.group(1),[]).append(rel+':'+str(ln)+':def')
                for m in ASGF.finditer(line):
                    defs.setdefault(m.group(1),[]).append(rel+':'+str(ln)+':asg')
                m=ALIAS.search(line)
                if m and m.group(2) not in ('nil','true','false'):
                    defs.setdefault(m.group(1),[]).append(rel+':'+str(ln)+':alias='+m.group(2))
json.dump(defs,open('defs.json','w',encoding='utf-8'),ensure_ascii=False)
print('files',nfile,'global names defined',len(defs))
