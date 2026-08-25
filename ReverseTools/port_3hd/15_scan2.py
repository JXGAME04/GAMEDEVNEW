import os,re,json,sys
ROOT=r'D:\ServerLinux\server1\script'
ROOT2=r'D:\ServerLinux\server1\vng_script'
CL=json.load(open(r'D:\GAMEDEVNEW\ReverseTools\port_3hd\closure3.json',encoding='utf-8'))
BS=chr(92)

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

def strip_str_com(s):
    out=[]; i=0; n=len(s)
    while i<n:
        c=s[i]
        if c=='-' and i+1<n and s[i+1]=='-':
            j=s.find(chr(10),i)
            if j<0: j=n
            out.append(' '*(j-i)); i=j
        elif c=='"' or c=="'":
            q=c; j=i+1
            while j<n and s[j]!=q:
                if s[j]==BS: j+=1
                if j<n and s[j]==chr(10): break
                j+=1
            j=min(j+1,n)
            out.append(' '*(j-i)); i=j
        elif c=='[' and i+1<n and s[i+1]=='[':
            j=s.find(']]',i)
            j = n if j<0 else j+2
            seg=s[i:j]
            out.append(''.join(ch if ch==chr(10) else ' ' for ch in seg)); i=j
        else:
            out.append(c); i+=1
    return ''.join(out)

CALL=re.compile(r'(?<![A-Za-z0-9_.:%])([A-Za-z_][A-Za-z0-9_]*)\s*[({"' + chr(39) + r']')
DOTCALL=re.compile(r'([A-Za-z_][A-Za-z0-9_]*)\s*([.:])\s*([A-Za-z_][A-Za-z0-9_]*)\s*[({"' + chr(39) + r']')
UPVAL=re.compile(r'%([A-Za-z_][A-Za-z0-9_]*)\s*[({"' + chr(39) + r']')

def readfile(rel):
    for base in (ROOT,ROOT2):
        p=os.path.join(base,rel.replace('/',BS))
        if os.path.exists(p):
            return degbk(open(p,'rb').read()),p
    return None,None

calls={}
dotcalls={}
upvals={}
missing=[]
for feat,lst in CL.items():
    for e in lst:
        rel=e['rel']
        txt,p=readfile(rel)
        if txt is None:
            missing.append((feat,rel)); continue
        st=strip_str_com(txt)
        for ln,line in enumerate(st.split(chr(10)),1):
            for m in CALL.finditer(line):
                calls.setdefault(m.group(1),[]).append((feat,rel,ln))
            for m in DOTCALL.finditer(line):
                dotcalls.setdefault(m.group(1)+m.group(2)+m.group(3),[]).append((feat,rel,ln))
            for m in UPVAL.finditer(line):
                upvals.setdefault(m.group(1),[]).append((feat,rel,ln))

json.dump({'calls':calls,'dot':dotcalls,'up':upvals,'missing':missing},open('scan2.json','w',encoding='utf-8'),ensure_ascii=False)
print('bare-call names',len(calls))
print('dot/colon call names',len(dotcalls))
print('upvalue call names',len(upvals))
print('files missing on disk',missing)
