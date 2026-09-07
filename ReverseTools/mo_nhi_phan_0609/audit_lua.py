import importlib.util,sys,re,os,json
spec=importlib.util.spec_from_file_location("dec2", r"D:\GAMEDEVNEW\ReverseTools\port_3hd\dec2.py"); dec2=importlib.util.module_from_spec(spec); spec.loader.exec_module(dec2)
sys.stdout.reconfigure(encoding='utf-8',errors='replace')
L=r"D:\ServerLinux\server1\script\skill"; J=r"E:\SourceTuanLe\SourceVs22\TESTLOFFF_ONLINE\bin\server\script\nhanvat\kynang"
def load(p): return [dec2.decline2(l.rstrip(b"\r")) for l in open(p,'rb').read().split(b"\n")]
def strip_comment(l):
    # keep strings; drop -- comments outside strings (approx)
    out=''; i=0; q=None
    while i<len(l):
        c=l[i]
        if q:
            out+=c
            if c=='\\' and i+1<len(l): out+=l[i+1]; i+=2; continue
            if c==q: q=None
        else:
            if c in '"\'': q=c; out+=c
            elif l.startswith('--',i): break
            else: out+=c
        i+=1
    return out
def parse(lines):
    """SKILLS = { key = { attr = ..., }, } -> {key: {attr: normalized string}}"""
    res={}; key=None; attr=None; buf=''; depth=0; inSKILLS=False
    for raw in lines:
        l=strip_comment(raw).strip()
        if not l: continue
        if re.match(r'SKILLS\s*=\s*\{', l): inSKILLS=True; depth=1; continue
        if not inSKILLS: continue
        if key is None:
            m=re.match(r'([A-Za-z_][A-Za-z0-9_]*)\s*=\s*\{(.*)$', l)
            if m:
                key=m.group(1); res[key]={}; depth=1+m.group(2).count('{')-m.group(2).count('}')
                if depth<=0: key=None
                continue
            if l.startswith('}'): inSKILLS=False
            continue
        # inside a key table
        if attr is None:
            m=re.match(r'([A-Za-z_][A-Za-z0-9_]*)\s*=\s*(.*)$', l)
            if m:
                attr=m.group(1); buf=m.group(2); d=buf.count('{')-buf.count('}')
                if attr=='skill_desc':  # function ... end: capture until 'end,' at depth
                    if buf.strip().startswith('function') and re.search(r'\bend\s*,?\s*$',buf): res[key][attr]='<fn>'; attr=None
                    continue
                if d<=0:
                    res[key][attr]=re.sub(r'\s+','',buf).rstrip(','); attr=None
                continue
            if l.startswith('}'):
                depth-=1
                if depth<=0: key=None
            continue
        else:
            buf+=' '+l
            if attr=='skill_desc':
                if re.search(r'\bend\s*,?\s*$',l): res[key][attr]='<fn>'; attr=None
                continue
            d=buf.count('{')-buf.count('}')
            if d<=0:
                res[key][attr]=re.sub(r'\s+','',buf).rstrip(','); attr=None
    return res
report=[]; tot=0
files=sorted(set(f for f in os.listdir(L) if f.endswith('.lua')) | set(f for f in os.listdir(J) if f.endswith('.lua')))
summary=[]
for f in files:
    pl=os.path.join(L,f); pj=os.path.join(J,f)
    if not os.path.exists(pl) or not os.path.exists(pj):
        summary.append((f,'CHI '+('LINUX' if os.path.exists(pl) else 'JX1'))); continue
    a=parse(load(pl)); b=parse(load(pj))
    keys=sorted(set(a)|set(b)); nd=0
    for k in keys:
        if k not in a: report.append("%s | %s | CHI JX1"%(f,k)); nd+=1; continue
        if k not in b: report.append("%s | %s | CHI LINUX"%(f,k)); nd+=1; continue
        attrs=sorted(set(a[k])|set(b[k]))
        for at in attrs:
            va=a[k].get(at); vb=b[k].get(at)
            if va!=vb and at!='skill_desc':
                report.append("%s | %s | %s | L=%s | J=%s"%(f,k,at,va,vb)); nd+=1
    summary.append((f,nd)); tot+=nd
open('audit_lua_diff.txt','w',encoding='utf-8').write("\n".join(report))
print("tong lech:",tot)
for f,n in summary: print("  %-28s %s"%(f,n))
